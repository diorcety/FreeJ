/*  C++ Entity class
 *
 *  (c) Copyright 2001-2006 Denis Rojo <jaromil@dyne.org>
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
 */

#ifndef __entity_h__
#define __entity_h__

class BaseLinklist;

template <class T> class Linklist;

// javascript context
class JSContext;
// , class
class JSClass;
// and object
class JSObject;

class Entry {
    friend class Linklist<Entry>;

public:
    Entry();
    virtual ~Entry();

    void setName(const std::string &name);
    const std::string &getName() const;
    void setDescription(const std::string &description);
    const std::string &getDescription() const;
    bool up();
    bool down();
    bool move(int pos);
    bool swap(int pos);
    void rem();
    void sel(bool on);

    Entry *next;
    Entry *prev;

    BaseLinklist *list;

    bool select;

protected:
    std::string name;
    std::string description;

public:

    // generic data pointer, so far only used in console
    // and now also as JSObject -> jsval
    void *data;

#ifdef WITH_JAVASCRIPT
    JSContext *jsenv; ///< pointer to the javasript context
    JSClass *jsclass; ///< pointer to the javascript class
    JSObject *jsobj; ///< pointer to the javascript instantiated object
    JSContext *context() {
        return jsenv;
    }                                    // TO REMOVE

    JSObject  *object() {
        return jsobj;
    }                                   // TO REMOVE

#endif //WITH_JAVASCRIPT

};

#endif //__entity_h__
