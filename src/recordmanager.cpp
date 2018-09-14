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


#include "../include/recordmanager.h"
using namespace std;

Record_Manager::Record_Manager(string filename,bool pipe_mode,string meeting_file_name,string mfcc_file_name,int32_t buffer_size,int32_t chunkSize) {
    if (pipe_mode) {
        mkfifo(name.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    }
    this->name = filename;
    this->chunkSize = chunkSize;
    this->mfcc_file_name = mfcc_file_name;
    stream.open (name, ios::out | ios::binary);
    mfcc_stream.open (mfcc_file_name, ios::out | ios::binary);
    this->meeting_file_name = meeting_file_name;
    this->buffer = new Circular_Buffer(buffer_size);
}

void Record_Manager::setAudioInput(BlockingQueue<int16_t*>* queue) {
        audio_queue=queue;
}

void Record_Manager::setMFCCInput(BlockingQueue<float*>* queue) {
    mfcc_queue=queue;
}


bool Record_Manager::writeData(int16_t* data, int elements) {
    if (recording) {
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
    if (meeting_recording) {
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
    if (mfcc_on) {
        if (mfcc_stream.is_open())
        {
            // If it is open we can do our writing to the file.
            mfcc_stream.write((char*)data, sizeof(int16_t)*elements);
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


bool Record_Manager::sendMFCCFeatures(float* MFCCFeatures,int num_cep,ofstream f) {

    return true;
}

void Record_Manager::run() {
    while(true) {
        int16_t* audio_input = audio_queue->pop();
        float* mfcc_input = mfcc_queue->pop();
        if (event == Meeting) {
            if (meeting_stream.is_open() == false) {
                OpenMeetingFile();
            }
            writeData(audio_input,chunkSize);

        }
        if (event == Recording){   
            writeData(audio_input,chunkSize);
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