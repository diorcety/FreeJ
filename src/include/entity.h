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

FREEJ_FORWARD_PTR(Entry)
class Entry : public EnableSharedFromThis<Entry> {
public:
    Entry(const std::string &name = "", const std::string &description = "");
    virtual ~Entry();

    const std::string &getName() const;
    const std::string &getDescription() const;

protected:
    std::string name;
    std::string description;
};

#endif //__entity_h__
