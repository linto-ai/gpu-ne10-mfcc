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

Record_Manager::Record_Manager(string filename,bool pipe_mode,string meeting_file_name,int32_t buffer_size,int32_t chunkSize) {
    if (pipe_mode) {
        mkfifo(name.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    }
    
    this->name = filename;
    this->chunkSize = chunkSize;
    stream.open (name, ios::out | ios::binary);
    this->meeting_file_name = meeting_file_name;
    this->buffer = new Circular_Buffer(buffer_size);
}


bool Record_Manager::writeData(int16_t* data, int elements, enum event e) {
    if (e == Recording) {
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
    else if (e == Meeting) {
        if (meeting_stream.is_open())
        {
            // If it is open we can do our writing to the file.
            meeting_stream.write((char*)data, sizeof(int16_t)*elements);
            return true;
        }
        else
        {
            // If the file isn't open something went wrong. Point that out.
            cout << "Something went wrong with opening the file!";
            return false;
        }
    }
    return false;
}

void Record_Manager::OpenMeetingFile() {
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    this->meeting_file_name += to_string(ms.count()) + ".raw";
    this->meeting_stream.open(this->meeting_file_name, ios::out | ios::binary);
}

/*void Record_Manager::setInput(BlockingQueue<int16_t*>* queue){
    input_queue = queue;
}*/


bool Record_Manager::sendMFCCFeatures(void* MFCCFeatures,int size,ofstream f) {

    return true;
}

bool  Record_Manager::test() {
    int16_t* data = NULL;
    data = (int16_t*)malloc(sizeof(int16_t)*1024);
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> dis(-32767,32767);
    for (int k=0; k < 10 ; k++) {
        for (int n=0; n<1024;n++) {
            data[n]=dis(gen);
        }
        buffer->add(data,1024);
        cout << buffer->getIndex() << endl;
    }
    bool ret = Record_Manager::writeData(buffer->getBuffer(),buffer->getSize(),Recording);
    return ret;
}

void Record_Manager::setEvent(enum event new_event) {
    this->event = new_event;
}


void Record_Manager::run() {
    while(true) {
        //int16_t* input = queue->pop();
        if (event == Meeting) {
            if (meeting_stream.is_open() == false) {
                OpenMeetingFile();
            }
            //writeData(input,chunkSize,meeting_stream,event);

        }
        if (event == Recording){   
            //writeData(input,chunkSize,stream,event);
        }
        if (event == None) {
            if (meeting_stream.is_open()) {
                meeting_stream.close();
            }
            if (stream.is_open()) {
                stream.close();
            }
        }
    }
}

Record_Manager::~Record_Manager() {
    stream.close();
}