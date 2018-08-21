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

#include "recordmanager.h"
using namespace std;

Record_Manager::Record_Manager(string name,int queue_max_size,bool pipe_mode,int32_t buffer_size) {
    if (pipe_mode) {
        mkfifo(name.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    }
    this->name = name;
    stream.open (name, ios::out | ios::binary);
    this->buffer = new Circular_Buffer(buffer_size);
    //BlockingQueue<int16_t> queue(queue_max_size);
    //this->queue = &queue;
}

bool Record_Manager::writeData(int16_t* data, int elements) {
    if (stream.is_open())
    {
        // If it is open we can do our writing to the file.
        stream.write((char*)data, sizeof(int16_t)*elements);
        return true;
    }
    else
    {
        // If the file isn't open something went wrong. Point that out.
        cout << "Something went wrong with opening the file!";
        return false;
    }
}

bool Record_Manager::sendMFCCFeatures(void* MFCCFeatures,int size,ofstream f) {
    return true;
}

bool  Record_Manager::run() {
    int16_t* data = NULL;
    data = (int16_t*)malloc(sizeof(int16_t)*1000);
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> dis(-32767,32767);
    for (int k=0; k < 10 ; k++) {
        for (int n=0; n<10000;n++) {
            data[n]=dis(gen);
        }
        buffer->add(data,1000);
        cout << buffer->getIndex() << endl;
    }
    return Record_Manager::writeData(buffer->getBuffer(),buffer->getSize());
}

Record_Manager::~Record_Manager() {
    stream.close();
}