/*  C++ Entity class
 *
 *  (c) Copyright 2001-2004 Denis Roio aka jaromil <jaromil@dyne.org>
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

#include <stdlib.h>
#include <string.h>

#include <jutils.h>
#include <entity.h>
#include <linklist.h>

Entry::Entry() {
    data = NULL;
#ifdef WITH_JAVASCRIPT
    jsclass = NULL;
    jsobj = NULL;
    jsenv = NULL;
#endif //WITH_JAVASCRIPT
}

Entry::~Entry() {
    if(data != NULL) {
        free(data);
    }
}

void Entry::setName(const std::string &name) {
    this->name = name;
}

const std::string &Entry::getName() const {
    return name;
}

void Entry::setDescription(const std::string &description) {
    this->description = description;
}

const std::string &Entry::getDescription() const {
    return description;
}

bool Entry::up() {
    /*
     *TODO
    if(!list) return(false);
    if(!prev) return(false);

#ifdef THREADSAFE
    list->lock();
#endif

    Entry *tprev = prev,
    *tnext = next,
    *pp = prev->prev;

    if(!next)
        list->last = prev;

    if(tnext)
        tnext->prev = tprev;

    next = tprev;
    prev = pp;
    tprev->next = tnext;
    tprev->prev = this;

    if(pp)
        pp->next = this;

    if(!prev)
        list->first = this;

#ifdef THREADSAFE
    list->unlock();
#endif
*/
    return(true);
}

bool Entry::down() {
    /*
     *TODO
    if(!list) return(false);
    if(!next) return(false);

#ifdef THREADSAFE
    list->lock();
#endif

    Entry *tprev = prev,
    *tnext = next,
    *nn = next->next;

    if(!prev)
        list->first = next;

    if(tprev)
        tprev->next = tnext;

    prev = tnext;
    next = nn;
    tnext->prev = tprev;
    tnext->next = this;
    if(nn)
        nn->prev = this;

    if(!next)
        list->last = this;

#ifdef THREADSAFE
    list->unlock();
#endif
    */
    return(true);

}

bool Entry::move(int pos) {
    /*
     *TODO
    func("Entry::move(%i)", pos);
    if(!list)
        return(false);
#ifdef THREADSAFE
    list->lock();
#endif
    Entry *displaced;

    // find our position
    Entry *search = list->first;
    int mypos = 1;
    while(search && search != this) {
        mypos++;
        search = search->next;
    }

    // no move is necessary
    if(mypos == pos) {
#ifdef THREADSAFE
        list->unlock();
#endif
        return(true);
    }
    displaced = list->_pick(pos);

    // detach ourselves from the list
    if(next) {
        next->prev = prev;
        if(prev)
            prev->next = next;
        else
            list->first = next;
    } else {
        list->last = prev;
    }
    if(prev) {
        prev->next = next;
        if(next)
            next->prev = prev;
        else
            list->last = prev;

    } else {
        list->first = next;
    }
    prev = NULL;
    next = NULL;
    // now insert ourselves at the new position
    if(pos >= list->length) {  // shortcut if we are going to be the last entry
        list->last->next = this;
        prev = list->last;
        list->last = this;
    } else if(pos == 1) {  // shortcut if we are going to be the first entry
        list->first->prev = this;
        next = list->first;
        list->first = this;
    } else {
        if(mypos > pos) {
            prev = displaced->prev;
            if(prev)
                prev->next = this;
            else
                list->first = this;
            next = displaced;
            displaced->prev = this;
        } else if(mypos < pos) {
            next = displaced->next;
            if(next)
                next->prev = this;
            else
                list->last = this;
            prev = displaced;
            displaced->next = this;
        }
    }
#ifdef THREADSAFE
    list->unlock();
#endif
    */
    return(true);

}
