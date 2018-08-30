/**
 * Copyright (C) 2018 Linagora
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CIRCULAR_H
#include <iostream>
#include <cstdint>
#include <cstring>
using namespace std;

class Circular_Buffer {
    public:
    Circular_Buffer(uint32_t size);
    uint32_t getIndex();
    void setIndex(uint32_t new_index);
    bool add(int16_t* data,uint32_t size);
    int16_t* getBuffer();
    uint32_t getSize();
    ~Circular_Buffer();
    
    private:
    int16_t* buffer;
    uint32_t circular_size;
    uint32_t index;
};



#endif