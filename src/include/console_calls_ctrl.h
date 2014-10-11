/*  FreeJ - S-Lang console
 *
 *  (c) Copyright 2004-2009 Denis Roio <jaromil@dyne.org>
 *
 * This source code  is free software; you can  redistribute it and/or
 * modify it under the terms of the GNU Public License as published by
 * the Free Software  Foundation; either version 3 of  the License, or
 * (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but  WITHOUT ANY  WARRANTY; without  even the  implied  warranty of
 * MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  Please refer
 * to the GNU Public License for more details.
 *
 * You should  have received  a copy of  the GNU Public  License along
 * with this source code; if  not, write to: Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __CONSOLE_CALLS_CTRL_H__
#define __CONSOLE_CALLS_CTRL_H__

#include "sharedptr.h"

FREEJ_FORWARD_PTR(Context)

int console_open_layer(ContextPtr env, char *cmd);

int console_param_selection(ContextPtr env, char *cmd);
int console_param_completion(ContextPtr env, char *cmd);

int console_blit_selection(ContextPtr env, char *cmd);
int console_blit_completion(ContextPtr env, char *cmd);

int console_blit_param_selection(ContextPtr env, char *cmd);
int console_blit_param_completion(ContextPtr env, char *cmd);

int console_filter_selection(ContextPtr env, char *cmd);
int console_filter_completion(ContextPtr env, char *cmd);

#ifdef WITH_JAVASCRIPT
int console_exec_script(ContextPtr env, char *cmd);
int console_exec_script_command(ContextPtr env, char *cmd);
#endif //WITH_JAVASCRIPT

int console_generator_selection(ContextPtr env, char *cmd);
int console_generator_completion(ContextPtr env, char *cmd);

int console_print_text_layer(ContextPtr env, char *cmd);
int console_open_text_layer(ContextPtr env, char *cmd);

int console_filebrowse_completion(ContextPtr env, char *cmd);


#endif
