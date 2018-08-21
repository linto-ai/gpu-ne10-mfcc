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
#ifndef RECORDER_H
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <sys/types.h>  
#include <sys/stat.h> 
#include "circular.h" 
using namespace std;

class Record_Manager {
    public:
    Record_Manager(string name, int queue_max_size,bool pipe_mode,int32_t buffer_size);
    bool writeData(int16_t* data, int elements);
    bool sendBinaryFlow(int16_t* data,int size);
    bool sendMFCCFeatures(void* MFCCFeatures,int size,ofstream f);
    bool run();
    ~Record_Manager();
    
    private:
    string name;
    ofstream stream;
    Circular_Buffer *buffer;
    //BlockingQueue<int16_t> *queue;
};

#endif