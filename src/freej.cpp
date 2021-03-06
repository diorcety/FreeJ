/*  FreeJ
 *  (c) Copyright 2001-2009 Denis Roio <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
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

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>

#include "context.h"

// #include <osd.h>
#ifndef WITH_COCOA
#include "slang_console_ctrl.h"
#endif
#include "video_encoder.h"
#include "plugger.h"
#include "jutils.h"

#include "impl_layers.h"
#include "impl_screens.h"
#include "impl_video_encoders.h"

//[of]cli options parser:commandline
#define MAX_CLI_CHARS 4096

/* ================ command line options
   (scroll down about 100 lines for the real stuff)
 */

static const char *help =
    " .  Usage: freej [options] [layers]\n"
    " .\n"
    " .   -h   print this help\n"
    " .   -v   version information\n"
    " .   -D   debug verbosity level - default 1\n"
    " .   -s   size of screen - default 400x300\n"
    " .   -S   screen type - default 'sdl'\n"
    " .   -a   initialize audio (using jack)\n"
    " .   -n   start with deactivated layers\n"
    " .   -c   no interactive text console\n"
    " .   -f   <frame_per_second>  select global fps for freej\n"
    " .   -F   start in fullscreen\n"
#ifdef WITH_OPENGL
    " .   -g   experimental opengl engine! (better pow(2) res as 256x256)\n"
#endif
    " .   -j   <javascript.js>  execute a javascript file\n"
    " .   -p   <processing.pde> execute a processing script (experimental)\n"
    " .\n";

// we use only getopt, no _long
static const char *short_options = "-hvD:gas:S:nj:p:cgf:F";

/* this is the global FreeJ context */
ContextPtr freej;

// the runtime will open one screen by default
ViewPortPtr screen;
char screen_name[16];

int debug_level = 0;
char layer_files[MAX_CLI_CHARS];
int cli_chars = 0;
int width = 400;
int height = 300;
int magn = 0;
char javascript[512];  // script filename
char processing[512];  // script filename

int fps = 25;

bool startstate = true;
bool gtkgui = false;

int audiomode = 0;
bool noconsole = false;
bool fullscreen = false;
bool opengl = false;

void cmdline(int argc, char **argv) {
    int res, optlen;
    FILE *fd; // tmp fd for checks

    /* initializing defaults */
    char *p                  = layer_files;
    javascript[0]            = 0;
    processing[0]            = 0;

    debug_level              = 0;

    // default screen
    snprintf(screen_name, 16, "sdl");

    do {
//    res = getopt_long (argc, argv, short_options); TODO getopt_long
        res = getopt(argc, argv, short_options);
        switch(res) {
        case 'h':
            fprintf(stdout, "%s", help);
            fprintf(stdout,
                    " .  Layers available:\n"
                    " .   you can specify any number of files or devices to be loaded,\n"
                    " .   this binary is compiled to support the following layer formats:\n");
            fprintf(stdout, "%s", freej->layers_description);
            fprintf(stdout, " .  Screens available:\n");
            fprintf(stdout, "%s", freej->screens_description);
            exit(0);
            break;
        case 'v':
            fprintf(stderr, "\n");
            exit(0);
            break;
        case 'D':
            debug_level = atoi(optarg);
            if(debug_level > 3) {
                warning("debug verbosity ranges from 1 to 3\n");
                debug_level = 3;
            }
            break;

        case 's':
            sscanf(optarg, "%ux%u", &width, &height);
//      freej.screen->resize(width,height);
            /* what the fuck ???
               if(width<320) {
               error("display width can't be smaller than 400 pixels");
               width = 320;
                 }
                 if(height<240) {
               error("display height can't be smaller than 300 pixels");
               width = 240;
                 }
             */
            break;
        case 'S':
            snprintf(screen_name, 16, "%s", optarg);
            break;

        case 'a':
            audiomode = 1;
            break;

        case 'm':
            sscanf(optarg, "%u", &magn);
            magn -= 1;
            magn = (magn > 3) ? 3 : (magn < 1) ? 0 : magn;
            break;

        case 'n':
            startstate = false;
            break;

        case 'c':
            noconsole = true;
            break;

        case 'f':
            sscanf(optarg, "%u", &fps);
            break;

        case 'F':
            fullscreen = true;
//      freej.screen->fullscreen();
            break;

        case 'j':
            fd = fopen(optarg, "r");
            if(!fd) {
                error("can't open JS file '%s': %s", optarg, strerror(errno));
                error("missing script, fatal error.");
                exit(1);
            } else {
                snprintf(javascript, 512, "%s", optarg);
                fclose(fd);
            }
            break;

        case 'p':
            fd = fopen(optarg, "r");
            if(!fd) {
                error("processing script file not found '%s': %s", optarg, strerror(errno));
            } else {
                snprintf(processing, 512, "%s", optarg);
                fclose(fd);
            }
            break;

#ifdef WITH_OPENGL
        case 'g':
            opengl = true;
            break;
#endif


        case '?':
            warning("unrecognized option: %s", optarg);
            break;

        case 1:
            // if it's a script, load it as such
            // this lets shell bangs work
            if(strstr(optarg, ".js")) {
                sprintf(javascript, "%s", optarg);
                continue;
            }

            optlen = strlen(optarg);
            if((cli_chars + optlen) < MAX_CLI_CHARS) {
                sprintf(p, "%s#", optarg);
                cli_chars += optlen + 1;
                p += optlen + 1;
            } else warning("too much files on commandline, list truncated");
            break;

        default:
            // act("received commandline parser code %i with optarg %s",res,optarg);
            break;
        }
    } while(res != -1);

#ifdef HAVE_DARWIN
    for(; optind < argc; optind++) {

        if(strstr(argv[optind], ".js")) {
            sprintf(javascript, "%s", argv[optind]);
            continue;
        }

        optlen = strlen(argv[optind]);
        if((cli_chars + optlen) < MAX_CLI_CHARS) {
            sprintf(p, "%s#", argv[optind]);
            cli_chars += optlen + 1;
            p += optlen + 1;
        } else
            warning("too many files on commandline, list truncated");
    }
#endif
}

/* ===================================== */

// scandir selection for .js or .freej
#if defined (HAVE_DARWIN) || defined (HAVE_FREEBSD)
int script_selector(struct dirent *dir)
#else
int script_selector(const struct dirent *dir)
#endif
{
    if(strstr(dir->d_name, ".freej")) return(1);
    if(strstr(dir->d_name, ".js")) return(1);
    return(0);
}

#ifdef WITH_JAVASCRIPT
int open_script(char *file) {
   return 0;
}

bool config_check(const char *filename) {
    char tmp[512];

    if(!js) {
        warning("javascript is not initialized");
        warning("no configuration is loaded");
        return(false);
    }

    snprintf(tmp, 512, "%s/.freej/%s", getenv("HOME"), filename);
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }

    snprintf(tmp, 512, "/etc/freej/%s", filename);
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }

#ifdef HAVE_DARWIN
    snprintf(tmp, 512, "%s/%s", "CHANGEME", filename);
#else
    snprintf(tmp, 512, "%s/%s", DATADIR, filename);
#endif
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }

    snprintf(tmp, 512, "/usr/lib/freej/%s", filename);
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }

    snprintf(tmp, 512, "/usr/local/lib/freej/%s", filename);
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }

    snprintf(tmp, 512, "/opt/video/lib/freej/%s", filename);
    if(filecheck(tmp)) {
        js->open(tmp);
        return(true);
    }
    return(false);
}


// load all default scripts in $DATADIR/freej and ~/.freej
int scripts(char *path) {
    char *dir;
    struct dirent **filelist;
    int found;

    dir = strtok(path, ":");
    do {
        found = scandir(dir, &filelist, script_selector, alphasort);
        if(found < 0) {
            error("loading default scripts: scandir error: %s", strerror(errno));
            return(-1);
        }
        /* .so files found, check if they are plugins */
        while(found--) {
            char temp[256];
            snprintf(temp, 255, "%s/%s", dir, filelist[found]->d_name);
            // if it exist is a default one: source it
            open_script(temp);
        }
    } while((dir = strtok(NULL, ":")));

    return 1;
}
#endif //WITH_JAVASCRIPT

#ifndef HAVE_DARWIN
int main(int argc, char **argv) {
    LayerPtr lay;
    ConsoleControllerPtr con;
#ifdef WITH_JAVASCRIPT
    bool interactive = true;
#endif //WITH_JAVASCRIPT

    freej = MakeShared<Context>();

    notice("%s version %s   free the veejay", PACKAGE, VERSION);
    act("2001-2009 RASTASOFT :: %s", PACKAGE_URL);
    act("----------------------------------------------");

    cmdline(argc, argv);
    set_debug(debug_level);

    // create SDL screen by default at selected size
    screen = Factory<ViewPort>::get_instance("Screen", screen_name);
    //  screen = new SdlScreen();
    if(!screen) {
        error("no screen can be opened");
        exit(1);
    }

    screen->init(width, height, 32);
    setSelectedScreen(screen);

    // add the screen to the context
    freej->add_screen(screen);

    if(fullscreen) screen->fullscreen();

    /* sets realtime priority to maximum allowed for SCHED_RR (POSIX.1b)
       this hangs on some linux kernels - darwin doesn't even bothers with it
       anybody knows what's wrong when you turn it on? ouch! it hurts :|
       set_rtpriority is inside jutils.cpp
       if(set_rtpriority(true))
       notice("running as root: high priority realtime scheduling allowed.");
     */



    /* initialize the S-Lang text Console */
    if(!noconsole) {
        if(getenv("TERM")) {
            con = MakeShared<SlwConsole>(freej);
            freej->register_controller(con);
            GlobalLogger::register_logger(con);
            con->console_init();
        }
    }

#ifdef WITH_JAVASCRIPT
    // load default settings
    config_check("keyboard.js");

    /* execute javascript */
    if(javascript[0]) {
        interactive = false;
        open_script(javascript); // TODO: quit here when script failed??
        if(freej->isQuitting()) {
            //      freej.close();
            // here calling close directly we double the destructor
            // fixed omitting the explicit close() call
            // but would be better to make the destructor reentrant
            exit(1);
        } else {
            interactive = true;
        }
    }

    /* execute processing */
    if(processing[0]) {
        interactive = false;
        char tmp[1024];

        // parse includes our extra processing.js library
        snprintf(tmp, 1023, "include(\"processing.js\");script = read_file(\"%s\");Processing(script);", processing);
        freej->js->parse(tmp);
        if(freej->isQuitting()) {
            exit(1);
        } else {
            interactive = true;
        }
    }
#endif //WITH_JAVASCRIPT

    // Set fps
    freej->setFps(fps);

    // TODO freej->setStartRunning(startstate);

    /* create layers requested on commandline */
    {
        char *l, *p, *pp = layer_files;
        while(cli_chars > 0) {

            p = pp;

            while(*p != '#' && cli_chars > 0) {
                p++;
                cli_chars--;
            }
            l = p + 1;
            if(cli_chars <= 0) break;
            *p = '\0';

            func("creating layer for file %s", pp);

            lay = freej->open(pp); // hey, this already init and open the layer !!
            if(lay)  {
                screen->add_layer(lay);
            }

            pp = l;
        }
    }

    freej->start();

    /* initialize the S-Lang text Console */
    if(!noconsole) {
        if(getenv("TERM")) {
            freej->rem_controller(con);
            GlobalLogger::unregister_logger(con);
        }
    }

    return 0;
}

#endif
