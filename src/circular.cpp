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
#include "circular.h"

Circular_Buffer::Circular_Buffer(uint32_t size) {
    circular_size = size;
    index = 0;
    buffer = (int16_t*)malloc(sizeof(int16_t)*size);
}


bool Circular_Buffer::add(int16_t* data, uint32_t size) {
  if (circular_size >= size && size != 0) { // Useless case if bufsize > max_buf_size
   if (size + index >= circular_size) { //If there is not enough places at the end of the buffer
     unsigned int free_place = circular_size - index; //compute the free place in the buffer
     memcpy(buffer+index,data,free_place*sizeof(int16_t)); //copy data at the end of the buffer
     memcpy(buffer,data+free_place,(size-free_place)*sizeof(int16_t));//copy end of data at the start of the buffer
     index=size-free_place; //Return the new writing index
     return false;
   }
   memcpy(buffer+index,data,size*sizeof(int16_t));
   index+=size;
   return true;
 }
 return false;
}

int16_t* Circular_Buffer::getBuffer() {
    return buffer;
}

uint32_t Circular_Buffer::getSize() {
    return circular_size;
}

void Circular_Buffer::setIndex(uint32_t new_index) {
    index = new_index;
}

uint32_t Circular_Buffer::getIndex() {
    return index;
}

Circular_Buffer::~Circular_Buffer() {
    free(buffer);
}