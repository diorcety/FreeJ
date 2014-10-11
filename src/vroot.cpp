/* -*- Mode: C; tab-width: 2 -*-                                             */
/*****************************************************************************/
/**                   Copyright 1991 by Andreas Stolcke                     **/
/**               Copyright 1990 by Solbourne Computer Inc.                 **/
/**                          Longmont, Colorado                             **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name of Solbourne not be used in advertising                         **/
/**    in publicity pertaining to distribution of the  software  without    **/
/**    specific, written prior permission.                                  **/
/**                                                                         **/
/**    ANDREAS STOLCKE AND SOLBOURNE COMPUTER INC. DISCLAIMS ALL WARRANTIES **/
/**    WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF    **/
/**    MERCHANTABILITY  AND  FITNESS,  IN  NO  EVENT SHALL ANDREAS STOLCKE  **/
/**    OR SOLBOURNE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL    **/
/**    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA   **/
/**    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER    **/
/**    TORTIOUS ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE    **/
/**    OR PERFORMANCE OF THIS SOFTWARE.                                     **/
/*****************************************************************************/
#include "vroot.h"

#if !defined(lint) && !defined(SABER)
const char vroot_rcsid[] =
    "#Id: vroot.h,v 1.5 2003/09/04 01:04:38 jwz Exp #" "\n"
    "#Id: vroot.h,v 1.4 1991/09/30 19:23:16 stolcke Exp stolcke #";
#endif

Window
#ifdef __STDC__ /* ANSIfication added by jwz, to avoid superfluous warnings. */
VirtualRootWindowOfScreen(Screen *screen)
#else /* !__STDC__ */
VirtualRootWindowOfScreen(screen) Screen *screen;
#endif /* !__STDC__ */
{
    static Screen *save_screen = (Screen *)0;
    static Window root = (Window)0;

    if(screen != save_screen) {
        Display *dpy = DisplayOfScreen(screen);
        Atom __SWM_VROOT = None;
        unsigned int i;
        Window rootReturn, parentReturn, *children;
        unsigned int numChildren;

        /* first check for a hex or decimal window ID in the environment */
        const char *xss_id = getenv("XSCREENSAVER_WINDOW");
        if(xss_id && *xss_id) {
            unsigned long id = 0;
            char c;
            if(1 == sscanf(xss_id, " 0x%lx %c", &id, &c) ||
               1 == sscanf(xss_id, " %lu %c",   &id, &c)) {
                root = (Window) id;
                save_screen = screen;
                return root;
            }
        }

        root = RootWindowOfScreen(screen);

        /* go look for a virtual root */
        __SWM_VROOT = XInternAtom(dpy, "__SWM_VROOT", False);
        if(XQueryTree(dpy, root, &rootReturn, &parentReturn,
                      &children, &numChildren)) {
            for(i = 0; i < numChildren; i++) {
                Atom actual_type;
                int actual_format;
                unsigned long nitems, bytesafter;
                Window *newRoot = (Window *)0;

                if(XGetWindowProperty(dpy, children[i],
                                      __SWM_VROOT, 0, 1, False, XA_WINDOW,
                                      &actual_type, &actual_format,
                                      &nitems, &bytesafter,
                                      (unsigned char **) &newRoot) == Success
                   && newRoot) {
                    root = *newRoot;
                    break;
                }
            }
            if(children)
                XFree((char *)children);
        }

        save_screen = screen;
    }

    return root;
}
