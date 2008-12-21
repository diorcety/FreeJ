/*  FreeJ
 *  (c) Copyright 2001 - 2007 Denis Rojo aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * "$Id$"
 *
 */

#include <config.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <context.h>

#include <sdl_screen.h>

#ifdef WITH_OPENGL
#include <sdlgl_screen.h>
#include <gl_screen.h>
#endif

#include <SDL_imageFilter.h>
#include <SDL_framerate.h>
#include <jsparser.h>
#include <video_encoder.h>
#include <audio_collector.h>
#include <signal.h>
#include <errno.h>

#include <jutils.h>
#include <fastmemcpy.h>

#include <jsparser_data.h>

#include <impl_layers.h>
#include <impl_video_encoders.h>

void fsigpipe (int Sig);
int got_sigpipe;
FPSmanager FPS;

Context::Context() {

  screen          = NULL;
  console         = NULL;
  audio           = NULL;

  /* initialize fps counter */
  //  framecount      = 0; 
  magnification   = 0;
  changeres       = false;
  clear_all       = false;
  start_running   = true;
  quit            = false;
  pause           = false;
  save_to_file    = false;
  interactive     = true;
  poll_events     = true;

  fps_speed       = 24;

  layers_description =
" .  - ImageLayer for image files (png, jpeg etc.)\n"
" .  - GeometryLayer for scripted vectorial primitives\n"
#ifdef WITH_V4L
" .  - Video4Linux devices as of BTTV cards and webcams\n"
" .    you can specify the size  /dev/video0%160x120\n"
#endif
#ifdef WITH_FFMPEG
" .  - MovieLayer for movie files, urls and firewire devices\n"
#endif
#ifdef WITH_FT2
" .  - TextLayer for text rendered with freetype2 library\n"
#endif
#ifdef WITH_FLASH
" .  - FlashLayer for SWF flash v.3 animations\n"
#endif
"\n";



}

Context::~Context() {
  func("%s this=%p",__PRETTY_FUNCTION__, this);

  Layer *lay;
  Controller *ctrl;
  VideoEncoder *enc;


#ifdef WITH_FT2
  // free font path array
  int c;
  if(font_files) {
    for(c=0; c<num_fonts; c++)
      free( font_files[c] );
    num_fonts = 0;
    free(font_files);
  }
#endif
  
  
  if (console) {
    console->close ();
    delete console;
    console = NULL;
  }

  //js->gc(); 
  if(js) { // invokes JSGC and all gc call on our JSObjects!
    delete js;
    js = NULL;
  }

  layers.unlock(); // in case we crashed while cafudda'ing
  lay = (Layer *)layers.begin ();
  while (lay) {
	lay-> stop();
    lay-> rem(); // does layers.lock()
    delete lay;
    lay = (Layer *)layers.begin ();
  }

  encoders.unlock();
  enc = (VideoEncoder *)encoders.begin();
  while(enc) {
	enc->stop();
    enc->rem();
    delete enc;
    enc = (VideoEncoder *)encoders.begin();
  }

  ctrl = (Controller *)controllers.begin();
  while(ctrl) {
    //    ctrl->lock();
    controllers.rem(1);
    //    ctrl->quit = true;
    //    ctrl->signal_feed();
    //    ctrl->unlock();
    delete ctrl;
    ctrl = (Controller *)controllers.begin();
  }

  // close MLT
  //  mlt_factory_close();

  if (screen) {
    delete screen;
    screen = NULL;
  }

  if(audio) {
    delete audio;
    audio = NULL;
  }

  //  plugger.close();

  notice ("cu on http://freej.dyne.org");
}

bool Context::init
(int wx, int hx, VideoMode videomode, int audiomode) {

  notice("initializing context environment", wx, hx);

  switch(videomode) {
  case SDL:
    act("SDL video output");
    screen = new SdlScreen();
    break;
#ifdef WITH_OPENGL
  case SDLGL:
    act("SDL-GL video output");
    screen = new SdlGlScreen();
    break;
  case GL_HEADLESS:
    act("GL headless output");
    screen = new GlScreen();
    break;
#endif
  }
   
  
  if (! screen->init (wx, hx)) {
    error ("Can't initialize the viewport");
    error ("This is a fatal error");
    return (false);
  }
  
  // create javascript object
  js = new JsParser (this);

#ifdef WITH_FT2
  num_fonts = 0;
  font_files = NULL;

  // parse all font directories  
  scanfonts("/usr/X11R6/lib/X11/fonts", 1);
  scanfonts("/usr/share/truetype", 0);
  scanfonts("/usr/share/fonts/truetype", 1);
  scanfonts("/usr/share/fonts", 1);


  if(!num_fonts) {
    error("no truetype fonts found on your system");
    error("you should install .ttf fonts in one of the directories above.");
  } else
    notice("Found %i fonts installed",num_fonts);

#endif

  
  // a fast benchmark to select the best memcpy to use
  find_best_memcpy ();
  
  if( SDL_imageFilterMMXdetect () )
    act ("using MMX accelerated blit");

  SDL_initFramerate(&FPS);
  SDL_setFramerate(&FPS, fps_speed);

//  if(init_audio) 
//    audio = new AudioCollector("alsa_pcm:capture_1", 2048, 44100);

  // initialize MLT
  //  mlt_factory_init( NULL );



  // register SIGPIPE signal handler (stream error)
  got_sigpipe = false;
  if (signal (SIGPIPE, fsigpipe) == SIG_ERR) {
    error ("Couldn't install SIGPIPE handler"); 
    //   exit (0); lets not be so drastical...
  }
  
  return true;
}


void Context::start() {
  while(!quit) {
    cafudda(0.0);
    SDL_framerateDelay(&FPS); // synced with desired fps here
  }
}

/*
 * Main loop called fps_speed times a second
 */
void Context::cafudda(double secs) {
  VideoEncoder *enc;
  Layer *lay;
  
  if(secs>0.0) /* if secs == 0 will go out after one cycle */
    now = dtime();
  
  // Change resolution if needed 
  if (changeres) {
    handle_resize();
    changeres = false;
  } /////////////////////////////
  
  
    ///////////////////////////////
    // update the console
  if (console && interactive) 
    console->cafudda ();
  ///////////////////////////////
  
  
  ///////////////////////////////
  // start layers threads
  rocknroll ();
  ///////////////////////////////
  
  
  ///////////////////////////////
  // clear screen if needed
  if (clear_all) screen->clear();
  else if (osd.active) osd.clean();
  ///////////////////////////////
  
  
  ///////////////////////////////
  //// process controllers
  if(poll_events)
    handle_controllers();
  ///////////////////////////////
  
  
  ///////////////////////////////
  /// cafudda all active LAYERS
  lay = (Layer *)layers.end ();
  ///// process each layer in the chain
  if (lay) {
    layers.lock ();
    
    while (lay) {
      if (!pause)
#ifdef WITH_OPENGL
	if(screen->opengl)
          {
            lay->offset = lay->buffer;
	    ((GlScreen*)screen)->glblit(lay);
	  }
	else
#endif	  
	  lay->cafudda ();
      
      lay = (Layer *)lay->prev;
      
    }
    /////////// finish processing layers
    //////////////////////////////////////
    
    layers.unlock ();
  }
  /////////// finish processing layers
  //////////////////////////////////////
  
  
  //////////////////////////////////////
  // cafudda all active ENCODERS
  enc = (VideoEncoder*)encoders.end();
  ///// process each encoder in the chain
  if(enc) {
    encoders.lock();
    while(enc) {
      if(!pause)
	enc->cafudda();
      enc = (VideoEncoder*)enc->prev;
    }
    encoders.unlock();
  }
  /////////// finish processing encoders
  //////////////////////////////////////
  
  
  /////////////////////////////
  /// print on screen display 
  if (osd.active && interactive) 
    osd.print ();
  /////////////////////////////
  
  
  
  /////////////////////////////
  // show result on non GL screen 
  screen->show();
  /////////////////////////////
  
  
  /////////////////////////////
  // handle timing 
  //    if(!secs) break; // just one pass
  
  /////////////////////////////
  // honour quit requests
  //if(quit) break; // quit was called

  
  
  /// FPS calculation
  if(secs>0.0)
    SDL_framerateDelay(&FPS); // synced with desired fps here
  // continues N seconds or quits after one cycle
  //    riciuca = (dtime() - now < secs) ? true : false;
  //////////////////////////////////
  
  if (got_sigpipe) {
    quit=true;
    return;
  }
  
}

void Context::handle_resize() {
  screen->lock ();
  if (magnification) {
    screen->set_magnification (magnification);
    magnification = 0;
  }
  if(resizing) {
    screen->resize (resize_w, resize_h);
    resizing = false;
  }
  osd.resize ();
  screen->unlock();
  
  /* crop all layers to new screen size */
  Layer *lay = (Layer *) layers.begin ();
  while (lay) {
    lay -> lock ();
    lay -> blitter.crop (screen);
    lay -> unlock ();
    lay = (Layer*) lay -> next;
  } 
}

#define SDL_KEYEVENTMASK (SDL_KEYDOWNMASK|SDL_KEYUPMASK)

void Context::handle_controllers() {
  int res;
  Controller *ctrl;

  event.type = SDL_NOEVENT;

  SDL_PumpEvents();

  // peep if there are quit or fullscreen events
  res = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_KEYEVENTMASK|SDL_QUITMASK);

  // force quit when SDL does
  if (event.type == SDL_QUIT) {
    quit = true;
    return;
  }
  
  // fullscreen switch (ctrl-f)
  if(event.type == SDL_KEYDOWN)
    if(event.key.state == SDL_PRESSED)
      if(event.key.keysym.mod & KMOD_CTRL)
	if(event.key.keysym.sym == SDLK_f) {
	  screen->fullscreen();
	  res = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYEVENTMASK|SDL_QUITMASK);  
	}
  
  ctrl = (Controller *)controllers.begin();
  //if(res) {
      if(ctrl) {
        controllers.lock();
        while(ctrl) {
          if(ctrl->active)
            ctrl->poll();
            ctrl = (Controller*)ctrl->next;
        }
        controllers.unlock();
      }
  //}

  // flushes all events that are leftover
  while( SDL_PeepEvents(&event,1,SDL_GETEVENT, SDL_ALLEVENTS) > 0 ) continue;
  memset(&event, 0x0, sizeof(SDL_Event));

}

bool Context::register_controller(Controller *ctrl) {
  func("%u:%s:%s",__LINE__,__FILE__,__FUNCTION__);
  if(!ctrl) {
    error("%s called on a NULL object", __PRETTY_FUNCTION__);
    return false;
  }

  if(! ctrl->initialized )
    ctrl->init(js->js_context, js->global_object);


  ctrl->activate(true);

  controllers.append(ctrl);
  
  act("registered %s controller", ctrl->name);
  return true;
}

bool Context::rem_controller(Controller *ctrl) {
  func("%s",__PRETTY_FUNCTION__);
  if(!ctrl) {
    error("%s called on a NULL object", __PRETTY_FUNCTION__);
    return false;
  }
  js->gc(); // ?!
  ctrl->rem();
  // mh, the JS_GC callback does the delete ...
  if (ctrl->jsobj == NULL) {
    func("controller JSObj is null, deleting ctrl");
    delete ctrl;
  } else {
	ctrl->activate(false);
    notice("removed controller %s, deactivated it but not deleting!", ctrl->name);
  }
  return true;
}

void Context::add_layer(Layer *lay) {
  func("%u:%s:%s",__LINE__,__FILE__,__FUNCTION__);
  if(lay->list) lay->rem();
  //  lay->geo.fps = fps;

  lay->env = this;
  lay->screen = screen;
  lay->blitter.screen = screen;

  // center the position
  //lay->geo.x = (screen->w - lay->geo.w)/2;
  //lay->geo.y = (screen->h - lay->geo.h)/2;
  lay->blitter.crop( true );
  layers.prepend(lay);
  layers.sel(0);
  lay->sel(true);
  func("layer %s succesfully added",lay->name);
}

void Context::rem_layer(Layer *lay) {
  func("%u:%s:%s",__LINE__,__FILE__,__FUNCTION__);
  /*
  if (!lay->list) {
        error("removing Layer %p which is not in list", lay);
        return;
  }*/
  js->gc();
  lay->rem();
  if (lay->data == NULL) {
      notice("Layer: no JS data: deleting");
	  lay->stop();
      delete lay;
  } else {
      notice("removed layer %s but still present as JSObject, not deleting!", lay->name);
  }
}

void Context::add_encoder(VideoEncoder *enc) {
  func("%u:%s:%s",__LINE__,__FILE__,__FUNCTION__); 
  if(enc->list) enc->rem();
  
  enc->init(this);

  encoders.append(enc);

  encoders.sel(0);

  enc->sel(true);

  func("encoder %s succesfully added", enc->name);
}

int Context::open_script(char *filename) {
  return js->open(filename);
}

bool Context::config_check(const char *filename) {
  char tmp[512];
  
  snprintf(tmp, 512, "%s/.freej/%s", getenv("HOME"), filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  snprintf(tmp, 512, "/etc/freej/%s", filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  snprintf(tmp, 512, "%s/%s", DATADIR, filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  snprintf(tmp, 512, "/usr/lib/freej/%s", filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  snprintf(tmp, 512, "/usr/local/lib/freej/%s", filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  snprintf(tmp, 512, "/opt/video/lib/freej/%s", filename);
  if( filecheck(tmp) ) {
    js->open(tmp);
    return(true);
  }

  return(false);
}
	  
void Context::resize(int w, int h) {
  resize_w = w;
  resize_h = h;
  resizing = true;
  changeres = true;
}

void Context::magnify(int algo) {
  if(magnification == algo) return;
  magnification = algo;
  changeres = true;
}

/* FPS */

void Context::set_fps(int fps) {

  fps_speed = fps;
  SDL_setFramerate(&FPS, fps_speed);
  act("engine running at %u FPS", SDL_getFramerate(&FPS));
}

void Context::rocknroll() {
  VideoEncoder *e;
  Layer *l;
  //  bool need_audio = false;

  ///////////////////////////////////////////
  // Layers - start the threads to be started
  l = (Layer *)layers.begin();
  
  // Show credits when no layers are present and interactive
  if (!l) // there are no layers
    if ( interactive ) { // engine running in interactive mode
      osd.credits ( true);
      return;
    }
  
  // Iterate throught linked list of layers and start them

  layers.lock();
  while (l) {
    if (!l->running) {
      if (l->start() == 0) {
	/*	while (!l->running) {
	  jsleep(0,500);
	  func("waiting %s layer thread to start", l->name);
	  } */
	l->active = start_running;
      }
      else { // problems starting thread
	func ("Context::rocknroll() : error creating thread");
      }
    }
    
    //    need_audio |= (l->running && l->use_audio);
    
    l = (Layer *)l->next;
  }
  layers.unlock();

  /////////////////////////////////////////////////////////////
  ///////////////////////////////////// end of layers rocknroll



  /////////////////////////////////////////////////////
  // Video Encoders - start the encoders to be launched
  e = (VideoEncoder*) encoders.begin();

  encoders.lock();
  while(e) {
    if(!e->running) {
      func("launching thread for %s",e->name);
      if( e->start() == 0 ) {
	act("waiting for %s thread to start...", e->name);
	while(! e->running ) jsleep(0,500);
	act("%s thread started", e->name);
	e->active = start_running;
      }
      else { // problems starting thread
	error("can't launch thread for %s", e->name);
      }
    }

    //    need_audio |= (e->running && e->use_audio);

    e = (VideoEncoder*)e->next;
  }
  encoders.unlock();
  //////////////////////////////////////////////////////////////
  //////////////////////////////////// end of encoders rocknroll

}

void fsigpipe (int Sig) {
  if (!got_sigpipe)
    warning ("SIGPIPE - Problems streaming video :-(");
  got_sigpipe = true;
}


//////// implemented layers



Layer *Context::open(char *file) {
  char *end_file_ptr,*file_ptr;
  FILE *tmp;
  Layer *nlayer = NULL;

  /* check that file exists */
  if(strncasecmp(file,"/dev/",5)!=0
     && strncasecmp(file,"http://",7)!=0
     && strncasecmp(file,"layer_",6)!=0) {
    tmp = fopen(file,"r");
    if(!tmp) {
      error("can't open %s to create a Layer: %s",
	    file,strerror(errno));
      return NULL;
    } else fclose(tmp);
  }
  /* check file type, add here new layer types */
  end_file_ptr = file_ptr = file;
  end_file_ptr += strlen(file);
//  while(*end_file_ptr!='\0' && *end_file_ptr!='\n') end_file_ptr++; *end_file_ptr='\0';


  /* ==== Unified caputure API (V4L & V4L2) */
  if( strncasecmp ( file_ptr,"/dev/video",10)==0) {
#ifdef WITH_UNICAP
    unsigned int w=screen->w, h=screen->h;
    while(end_file_ptr!=file_ptr) {
      if(*end_file_ptr!='%') end_file_ptr--;
      else { /* size is specified */
        *end_file_ptr='\0'; end_file_ptr++;
        sscanf(end_file_ptr,"%ux%u",&w,&h);
        end_file_ptr = file_ptr; 
      }
    }
    nlayer = new UnicapLayer();
    if(! ((UnicapLayer*)nlayer)->init( this, (int)w, (int)h) ) {
      error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
      delete nlayer; return NULL;
    }
    if(nlayer->open(file_ptr)) {
      notice("video camera source opened");
    //  ((V4lGrabber*)nlayer)->init_width = w;
    //  ((V4lGrabber*)nlayer)->init_heigth = h;
    } else {
      error("create_layer : V4L open failed");
      delete nlayer; nlayer = NULL;
    }
#else
    error("Video4Linux layer support not compiled");
    act("can't load %s",file_ptr);
#endif

  } else /* VIDEO LAYER */

    if( ( ( IS_VIDEO_EXTENSION(end_file_ptr) ) | ( IS_FIREWIRE_DEVICE(file_ptr) ) ) ) {
      func("is a movie layer");

      // // MLT experiments
      //       nlayer = new MovieLayer();
      //       func("MovieLayer instantiated");
      //       if(!nlayer->init(this)) {
      //  	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
      //  	delete nlayer; return NULL;
      //       }
      //       func("MovieLayer initialized");
      //       if(!nlayer->open(file_ptr)) {
      //  	error("create_layer : VIDEO open failed");
      //  	delete nlayer; nlayer = NULL;
      //       }


#ifdef WITH_FFMPEG
       nlayer = new VideoLayer();
       if(!nlayer->init( this )) {
 	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
 	delete nlayer; return NULL;
       }
       if(!nlayer->open(file_ptr)) {
 	error("create_layer : VIDEO open failed");
 	delete nlayer; nlayer = NULL;
       }
// #elif WITH_AVIFILE
//       if( strncasecmp(file_ptr,"/dev/ieee1394/",14)==0)
// 	  nlayer=NULL;
//       nlayer = new AviLayer();
//       if(!nlayer->init( this )) {
// 	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
// 	delete nlayer; return NULL;
//       }
//       if(!nlayer->open(file_ptr)) {
// 	error("create_layer : AVI open failed");
// 	delete nlayer; nlayer = NULL;
//       }
 #else
      error("VIDEO and AVI layer support not compiled");
      act("can't load %s",file_ptr);
#endif
  } else /* IMAGE LAYER */
      if( (IS_IMAGE_EXTENSION(end_file_ptr))) {
//		strncasecmp((end_file_ptr-4),".png",4)==0) 
	      nlayer = new ImageLayer();
              if(!nlayer->init( this )) {
                error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
                delete nlayer; return NULL;
              }
	      if(!nlayer->open(file_ptr)) {
		  error("create_layer : IMG open failed");
		  delete nlayer; nlayer = NULL;
	      }
  } else /* TXT LAYER */
    if(strncasecmp((end_file_ptr-4),".txt",4)==0) {
#ifdef WITH_FT2
	  nlayer = new TextLayer();

      if(!nlayer->init( this )) {
	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
	delete nlayer; return NULL;
      }

	  if(!nlayer->open(file_ptr)) {
	    error("create_layer : TXT open failed");
	    delete nlayer; nlayer = NULL;
	  }
#else
	  error("TXT layer support not compiled");
	  act("can't load %s",file_ptr);
	  return(NULL);
#endif

  } else /* XHACKS LAYER */
    if(strstr(file_ptr,"xscreensaver")) {
#ifdef WITH_XHACKS
	    nlayer = new XHacksLayer();

      if(!nlayer->init( this )) {
	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
	delete nlayer; return NULL;
      }

	    if (!nlayer->open(file_ptr)) {
	      error("create_layer : XHACK open failed");
	      delete nlayer; nlayer = NULL;
	    }
#else
	    error("no xhacks layer support");
	    act("can't load %s",file_ptr);
	    return(NULL);
#endif
	  }  else if(strncasecmp(file_ptr,"layer_goom",10)==0) {

#ifdef WITH_GOOM
            nlayer = new GoomLayer();

      if(!nlayer->init( this )) {
	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
	delete nlayer; return NULL;
      }
#else
      error("goom layer not supported");
      return(NULL);
#endif


  } else if(strncasecmp(end_file_ptr-4,".swf",4)==0) {

	    nlayer = new FlashLayer();
      if(!nlayer->init( this )) {
	error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
	delete nlayer; return NULL;
      }

	    if(!nlayer->open(file_ptr)) {
	      error("create_layer : SWF open failed");
	      delete nlayer; nlayer = NULL;
	    }

  } else { /* FALLBACK TO SCROLL LAYER */

    func("opening scroll layer on generic file type for %s",file_ptr);
    nlayer = new ScrollLayer();
    
    if(!nlayer->init( this )) {
      error("failed initialization of layer %s for %s", nlayer->name, file_ptr);
      delete nlayer; return NULL;
    }
       
       if(!nlayer->open(file_ptr)) {
	 error("create_layer : SCROLL open failed");
	 delete nlayer; nlayer = NULL;
       }
       
  }

  if(!nlayer)
    error("can't create a layer with %s",file);
  else
    func("create_layer succesful, returns %p",nlayer);
  return nlayer;
}

