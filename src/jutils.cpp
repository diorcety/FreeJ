/*  FreeJ
 *  (c) Copyright 2001 Denis Roio aka jaromil <jaromil@dyne.org>
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

 * this file includes a malloc wrapper, it acts very verbose when debug
 * mode allow it
 */

#include <stdio.h>

#ifdef linux
/* we try to use the realtime linux clock on /dev/rtc */
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

/* freej specific: */
#include <console.h>

#include <jutils.h>
#include <config.h>

#define MAX_DEBUG 2

#define FUNC 2 /* se il debug level e' questo
		  ci sono le funzioni chiamate */
#define WARN 1 /* ... blkbblbl */

char msg[255];

static int verbosity;

static char *osd_msg;

static Console *console = NULL;

void set_debug(int lev) {
  lev = lev<0 ? 0 : lev;
  lev = lev>MAX_DEBUG ? MAX_DEBUG : lev;
  verbosity = lev;
}

int get_debug() {
  return(verbosity);
}

void set_osd(char *st) {
  osd_msg = st;
  osd_msg[0] = '\0';
}

void show_osd() {
  strncpy(osd_msg,msg,49);
  osd_msg[50] = '\0';
  if(console)
    console->notice(osd_msg);
}

void show_osd(char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  vsnprintf(osd_msg,49, format, arg);
  osd_msg[50] = '\0';
  va_end(arg);

  if(console)
    console->notice(osd_msg);
}

void set_console(Console *c) {
  console = c;
}

void notice(char *format, ...) {
  va_list arg;
  va_start(arg, format);

  vsnprintf(msg, 254, format, arg);
  if(console) console->notice(msg);
  else fprintf(stderr,"[*] %s\n",msg);
  
  va_end(arg);
}

void func(char *format, ...) {
  if(verbosity>=FUNC) {
    va_list arg;
    va_start(arg, format);
    
    vsnprintf(msg, 254, format, arg);
    if(console) console->func(msg);
    else fprintf(stderr,"[F] %s\n",msg);

    va_end(arg);
  }
}

void error(char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  vsnprintf(msg, 254, format, arg);
  if(console) console->error(msg);
  else fprintf(stderr,"[!] %s\n",msg);

  va_end(arg);
}

void act(char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  vsnprintf(msg, 254, format, arg);
  if(console) console->act(msg);
  else fprintf(stderr," .  %s\n",msg);
  
  va_end(arg);
}

void warning(char *format, ...) {
  if(verbosity>=WARN) {
    va_list arg;
    va_start(arg, format);
    
    vsnprintf(msg, 254, format, arg);
    if(console) console->warning(msg);
    else fprintf(stderr,"[W] %s\n",msg);
  
    va_end(arg);
  }
}

void *jalloc(void *point,size_t size) {
  if(point!=NULL)
    if(verbosity>0) warning("requested malloc on a non-NULL pointer");
  
  point = malloc(size);

  if(point==NULL) {
    error("cannot allocate %u bytes of memory",size);
    return(NULL);
  } else if(verbosity>=FUNC) {
    fprintf(stderr,"[M] allocated memory at %p sized %u bytes\n",point,(unsigned int)size);
  }
  return(point);
}

bool jfree(void *point) {

  if(point==NULL) {
    warning("requested free on a NULL pointer");
    return(false);
  }
  
  if(verbosity>=FUNC)
    fprintf(stderr,"[M] freeing memory at address %p\n",point);

  free(point);
  point = NULL;
  return(true);
}

/*
 * fastrand - fast fake random number generator
 * by Fukuchi Kentarou
 * Warning: The low-order bits of numbers generated by fastrand()
 *          are bad as random numbers. For example, fastrand()%4
 *          generates 1,2,3,0,1,2,3,0...
 *          You should use high-order bits.
 *
 */

static uint32_t randval;

uint32_t fastrand()
{
  //    kentaro's original one:
  //	return (randval=randval*1103515245+12345);
	//15:55  <salsaman2> mine uses two prime numbers and the cycling is much reduced
	//15:55  <salsaman2>   return (randval=randval*1073741789+32749);
  return(randval = randval * 1073741789 + 32749 );
}

void fastsrand(uint32_t seed)
{
	randval = seed;
}

#undef ARCH_X86
double dtime() {
#ifdef ARCH_X86
  double x;
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
  return x;
#else
  struct timeval mytv;
  gettimeofday(&mytv,NULL);
  return((double)mytv.tv_sec+1.0e-6*(double)mytv.tv_usec);
#endif
}

#ifdef linux
#include <sched.h>
/* sets the process to "policy" policy,  if max=1 then set at max priority,
   else use min priority */

bool set_rtpriority(bool max) {
  struct sched_param schp;
  // set the process to realtime privs

  memset(&schp, 0, sizeof(schp));
  
  if(max) 
    schp.sched_priority = sched_get_priority_max(SCHED_RR);
  else
    schp.sched_priority = sched_get_priority_min(SCHED_RR);
  
  if (sched_setscheduler(0, SCHED_RR, &schp) != 0)
    return false;
  else
    return true;
}
#endif

void jsleep(int sec, long nsec) {
  timespec timelap;
  timelap.tv_sec = sec;
  timelap.tv_nsec = nsec;
  nanosleep(&timelap,NULL);
}


/* small RTC interface by jaromil
   all comes from the Linux Kernel Documentation */
#ifdef linux
/* better to use /dev/rtc */
static int rtcfd = -1;
static fd_set readfds;
static timeval rtctv = { 0,0 };
static unsigned long rtctime;
int rtc_open() {
  int res;
  rtcfd = open("/dev/rtc",O_RDONLY);
  if(!rtcfd) {
    perror("/dev/rtc");
    return 0;
  }
  /* set the alarm event to 1 second */
  res = ioctl(rtcfd, RTC_UIE_ON, 0);
  if(res<0) {
    perror("rtc ioctl");
    return 0;
  }
  notice("realtime clock succesfully initialized");
  return 1;
}
/* tick returns 0 if 1 second didn't passed since last tick,
   positive number if 1 second passed */
unsigned long rtc_tick() {
  FD_ZERO(&readfds);
  FD_SET(rtcfd,&readfds);
  if ( ! select(rtcfd+1,&readfds,NULL,NULL,&rtctv) )
    return 0; /* a second didn't passed yet */
  read(rtcfd,&rtctime,sizeof(unsigned long));
  return rtctime;
}
void rtc_freq_set(unsigned long freq) {
  int res;

  res = ioctl(rtcfd,RTC_IRQP_SET,freq);
  if(res<0) { perror("rtc freq set"); }

  res = ioctl(rtcfd,RTC_IRQP_READ,&freq);
  if(res<0) { perror("rtc freq read"); }

  act("realtime clock frequency set to %ld",freq);

  res = ioctl(rtcfd,RTC_PIE_ON,0);
  if(res<0) { perror("rtc freq on"); return; }

}
void rtc_freq_wait() {
  int res;
  res = read(rtcfd,&rtctime,sizeof(unsigned long));
  if(res < 0) {
    perror("read rtc frequency interrupt");
    return;
  }
}
void rtc_close() {
  if(rtcfd<=0) return;
  ioctl(rtcfd, RTC_UIE_OFF, 0);
  //  ioctl(rtcfd,RTC_PIE_OFF,0);
  close(rtcfd);
}
#endif

void *(* jmemcpy)(void *to, const void *from, size_t len) = memcpy;

/*
 * memset(x,0,y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
void jmemset(void * s, unsigned long c ,size_t count)
{
#ifdef ARCH_X86
int d0, d1;
__asm__ __volatile__(
	"rep ; stosl\n\t"
	"testb $2,%b3\n\t"
	"je 1f\n\t"
	"stosw\n"
	"1:\ttestb $1,%b3\n\t"
	"je 2f\n\t"
	"stosb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
	:"memory");
#else
  memset(s,c,count);
#endif
}
