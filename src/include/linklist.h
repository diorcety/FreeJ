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
#include <list>
#include <utility>

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

template <typename T>
class LockedLinkList;

template <typename T>
class Linklist: private std::list<T*> {
    friend class LockedLinkList<T>;

private:
#ifdef THREADSAFE
    std::recursive_mutex mMutex;
#endif

public:
    Linklist();
    LockedLinkList<T> getLock();

};

template <typename T>
class LockedLinkList {
private:
    std::list<T*> &mList;
#ifdef THREADSAFE
    std::unique_lock<std::recursive_mutex> mLock;
#endif

public:
    typedef typename std::list<T*>::value_type               value_type;
    typedef typename std::list<T*>::pointer                  pointer;
    typedef typename std::list<T*>::iterator                 iterator;
    typedef typename std::list<T*>::const_iterator           const_iterator;
    typedef typename std::list<T*>::reference                reference;
    typedef typename std::list<T*>::const_reference          const_reference;
    typedef typename std::list<T*>::size_type                size_type;
    typedef typename std::list<T*>::const_reverse_iterator   const_reverse_iterator;
    typedef typename std::list<T*>::reverse_iterator         reverse_iterator;

public:
    LockedLinkList(Linklist<T> &list);
    void push_front(const value_type& val);
#if __cplusplus >= 201103L
    void push_front(value_type&& val);
#endif
    bool empty() const;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reference front();
    const_reference front() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    void push_back(const value_type& val);
#if __cplusplus >= 201103L
    void push_back(value_type&& val);
#endif
    size_type size() const;
    void pop_front();
    void pop_back();
    iterator erase(iterator position);
    void remove(const value_type& val);
    iterator insert(iterator position, const value_type& val);
    void insert(iterator position, size_type n, const value_type& val);
    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);
};

template <typename T>
Linklist<T>::Linklist() {
}

template <typename T>
LockedLinkList<T> Linklist<T>::getLock() {
    return LockedLinkList<T>(*this);
}

template <typename T>
LockedLinkList<T>::LockedLinkList(Linklist<T> &list):
    mList(list)
#ifdef THREADSAFE
   ,mLock(list.mMutex)
#endif
{

}

template <typename T>
bool LockedLinkList<T>::empty() const {
    return mList.empty();
}

template <typename T>
void LockedLinkList<T>::push_front(const typename LockedLinkList<T>::value_type& val) {
    mList.push_front(val);
}

#if __cplusplus >= 201103L
template <typename T>
void LockedLinkList<T>::push_front(typename LockedLinkList<T>::value_type&& val) {
    mList.push_front(std::forward<typename LockedLinkList<T>::value_type>(val));
}
#endif

template <typename T>
typename LockedLinkList<T>::iterator LockedLinkList<T>::begin() {
    return mList.begin();
}

template <typename T>
typename LockedLinkList<T>::const_iterator LockedLinkList<T>::begin() const {
    return mList.begin();
}

template <typename T>
typename LockedLinkList<T>::iterator LockedLinkList<T>::end() {
    return mList.end();
}

template <typename T>
typename LockedLinkList<T>::const_iterator LockedLinkList<T>::end() const{
    return mList.end();
}

template <typename T>
typename LockedLinkList<T>::reference LockedLinkList<T>::front() {
    return mList.front();
}

template <typename T>
typename LockedLinkList<T>::const_reference LockedLinkList<T>::front() const {
    return mList.front();
}

template <typename T>
typename LockedLinkList<T>::reverse_iterator LockedLinkList<T>::rbegin() {
    return mList.rbegin();
}

template <typename T>
typename LockedLinkList<T>::const_reverse_iterator LockedLinkList<T>::rbegin() const {
    return mList.rbegin();
}

template <typename T>
typename LockedLinkList<T>::reverse_iterator LockedLinkList<T>::rend() {
    return mList.rend();
}

template <typename T>
typename LockedLinkList<T>::const_reverse_iterator LockedLinkList<T>::rend() const {
    return mList.rend();
}

template <typename T>
void LockedLinkList<T>::push_back(const typename LockedLinkList<T>::value_type& val) {
    mList.push_back(val);
}

#if __cplusplus >= 201103L
template <typename T>
void LockedLinkList<T>::push_back(typename LockedLinkList<T>::value_type&& val) {
     mList.push_back(std::forward<typename LockedLinkList<T>::value_type>(val));
}
#endif

template <typename T>
typename LockedLinkList<T>::size_type LockedLinkList<T>::size() const {
    return mList.size();
}

template <typename T>
void LockedLinkList<T>::pop_front() {
    mList.pop_front();
}

template <typename T>
void LockedLinkList<T>::pop_back() {
    mList.pop_back();
}

template <typename T>
typename LockedLinkList<T>::iterator LockedLinkList<T>::erase(typename LockedLinkList<T>::iterator position) {
    return mList.erase(position);
}

template <typename T>
void LockedLinkList<T>::remove(const value_type& val) {
    mList.remove(val);
}

template <typename T>
typename LockedLinkList<T>::iterator LockedLinkList<T>::insert(typename LockedLinkList<T>::iterator position, const value_type& val) {
    return mList.insert(position, val);
}


template <typename T>
void LockedLinkList<T>::insert(typename LockedLinkList<T>::iterator position, size_type n, const value_type& val) {
    mList.insert(position, n, val);
}

template <typename T>
template <class InputIterator>
void LockedLinkList<T>::insert(typename LockedLinkList<T>::iterator position, InputIterator first, InputIterator last) {
    mList.insert(position, first, last);
}


#endif
