/*  C++ Linked list class, threadsafe (boolean is atom)
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

#ifndef __linklist_h__
#define __linklist_h__

#include <string.h>
#include <string>

#include "entity.h"

/* void warning(const char *format, ...); */
void func(const char *format, ...);

// uncomment to have mutex locked operations
// can be slow on OSX and adds dependency to pthreads
#define THREADSAFE 1

#ifdef THREADSAFE
#include <mutex>          // std::mutex
#endif


// maximum number of members returned by the completion
#define MAX_COMPLETION 512


// javascript class
class JSClass;
// and object
class JSObject;

class BaseLinklist {
    friend class Entry;

public:
    BaseLinklist() {
    }

    virtual ~BaseLinklist() {
    }

#ifdef THREADSAFE
    void lock() {
        mtx.lock();
    }

    void unlock() {
        mtx.unlock();
    }
#endif

    virtual Entry *_pick(int pos) = 0;

private:
#ifdef THREADSAFE
    std::mutex mtx;
#endif

protected:
    /* don't touch these from outside
       use begin() and end() and len() methods */
    Entry *first;
    Entry *last;
    int length;
};

template <class T>
class Linklist : public BaseLinklist {
public:
    Linklist();
    virtual ~Linklist();

    T *begin() {
        return((T*)first);
    }

    T *end() {
        return((T*)last);
    }

    int len() {
        return(length);
    }

    void append(T *addr);
    void prepend(T *addr);
    void insert_after(T *addr, T *pos);
    void rem(int pos);
    void clear();
    T *pick(int pos);
    Entry *_pick(int pos);

    T *search(const char *name, int *idx = NULL);
    T **completion(char *needle);

    T *operator[](int pos) {
        return pick(pos);
    }
};


///////////////////////////////////////////////////////////////
/////////////// IMPLEMENTATIONS
// here and not in the cpp class for the template linking issue





template <class T> Linklist<T>::Linklist() {
    length = 0;
    first = NULL;
    last = NULL;
    // selection = NULL; TODA AAA
}

template <class T> Linklist<T>::~Linklist() {
    clear();
}

/* adds one element at the end of the list */
template <class T> void Linklist<T>::append(T *addr) {
    T *ptr = NULL;
    if(addr->list) addr->rem();
#ifdef THREADSAFE
    lock();
#endif

    if(!last) { /* that's the first entry */
        last = addr;
        last->next = NULL;
        last->prev = NULL;
        first = last;
    } else { /* add the entry to the end */
        ptr = (T*)last;
        ptr->next = addr;
        addr->next = NULL;
        addr->prev = ptr;
        last = addr;
    }
    /* save the pointer to this list */
    addr->list = this;
    length++;
#ifdef THREADSAFE
    unlock();
#endif
}

template <class T> void Linklist<T>::prepend(T *addr) {
    T *ptr = NULL;
    if(addr->list) {
        func("Entry %s is already present in linklist %p - skipping duplicate prepend",
             addr->getName().c_str(), this);
        return;
    }
#ifdef THREADSAFE
    lock();
#endif

    if(!first) { /* that's the first entry */
        first = addr;
        first->next = NULL;
        first->prev = NULL;
        last = first;
    } else { /* add an entry to the beginning */
        ptr = (T*)first;
        ptr->prev = addr;
        addr->next = ptr;
        addr->prev = NULL;
        first = addr;
    }
    addr->list = this;
    length++;
#ifdef THREADSAFE
    unlock();
#endif
}

// inserts an element after the given one
template <class T> void Linklist<T>::insert_after(T *addr, T *pos) {

    // take it out from other lists
    if(addr->list) addr->rem();

#ifdef THREADSAFE
    lock();
#endif
    if(pos->next) {
        pos->next->prev = addr;
        addr->next = pos->next;
    } else last = addr;  // it's the last

    addr->prev = pos;
    pos->next = addr;

    length++;
    addr->list = this;

#ifdef THREADSAFE
    unlock();
#endif
}

/* clears the list
   i don't delete filters here because they have to be deleted
   from the procedure creating them. so this call simply discards
   the pointers stored into the linked list. OBJECTS ARE NOT FREED */
template <class T> void Linklist<T>::clear() {
#ifdef THREADSAFE
    lock();
#endif
    length = 0;
    first = NULL;
    last = NULL;
#ifdef THREADSAFE
    unlock();
#endif
}

/* takes one element from the list
   === STARTING FROM 1 ===
   returns NULL if called with pos=0 or pos>length
   returns Entry pointer otherwise
   this function is then overloading the operator[]
 */
template <class T> T *Linklist<T>::pick(int pos) {
    if(pos < 1) {
        //	  warning("linklist access at element 0 while first element is 1");
        return(NULL);
    }
    if(length < pos) {
        //	  warning("linklist access out of boundary");
        return(NULL);
    }
    // shortcuts
    if(pos == 1) return((T*)first);
    if(pos == length) return((T*)last);

    T *ptr;
    int c;
    // start from beginning
    if(pos < length / 2) {
        ptr = (T*)first;
        for(c = 1; c < pos; c++)
            ptr = (T*)ptr->next;
    } else { /// | | | p | | |
        ptr = (T*)last;
        for(c = length; c > pos; c--)
            ptr = (T*)ptr->prev;  // to be checked
    }
    return(ptr);
}

// virtual implementation for typecasting workaround
// internal use only by the Entry
template <class T> Entry *Linklist<T>::_pick(int pos) {
    return((Entry*)pick(pos));
}

/* search the linklist for the entry matching *name
   returns the Entry* on success, NULL on failure */
template <class T> T *Linklist<T>::search(const char *name, int *idx) {
    if(!first) return NULL;
    int c = 1;
    T *ptr = (T*)first;
    while(ptr) {
        if(strcasecmp(ptr->getName().c_str(), name) == 0) {
            if(idx) *idx = c;
            return(ptr);
        }
        ptr = (T*)ptr->next;
        c++;
    }
    if(idx) *idx = 0;
    return(NULL);
}

/* searches all the linklist for entries starting with *needle
   returns a list of indexes where to reach the matches */
template <class T> T **Linklist<T>::completion(char *needle) {
    static T *compbuf[MAX_COMPLETION*sizeof(T*)]; // completion buffer
    int c;
    int found;
    int len = strlen(needle);

    /* cleanup */
    memset(compbuf, 0, MAX_COMPLETION * sizeof(T*));

    /* check it */
    T *ptr = (T*)last;
    if(!ptr) return compbuf;

    for(found = 0, c = 1; ptr; c++, ptr = (T*)ptr->prev) {
        if(!len) { // 0 lenght needle: return the full list
            compbuf[found] = ptr;
            found++;
        } else if(strncasecmp(needle, ptr->getName().c_str(), len) == 0) {
            compbuf[found] = ptr;
            found++;
        }
    }

    func("completion found %i hits", found);
    return compbuf;
}

/* removes one element from the list */
template <class T> void Linklist<T>::rem(int pos) {
    T *ptr = pick(pos);
    if(ptr == NULL) return;
    ptr->rem();
}

#endif
