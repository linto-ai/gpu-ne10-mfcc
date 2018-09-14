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
#include <chrono>
#include "circular.h" 
#include "mqtt_client.h"
using namespace std;
using namespace std::chrono;

enum event {Recording, Meeting, None};

class Record_Manager {
    public:
    Record_Manager(string filename,bool pipe_mode,string meeting_file_name,string mfcc_file_name,int32_t buffer_size,int32_t chunkSize);
    bool writeData(int16_t* data, int elements);
    bool sendBinaryFlow(int16_t* data,int size);
    void setAudioInput(BlockingQueue<int16_t*>* queue);
    void setMFCCInput(BlockingQueue<int16_t*>* queue);
    void setMFCCInput(BlockingQueue<float*>* queue);
    bool sendMFCCFeatures(float* MFCCFeatures,int num_cep,ofstream f);
    void OpenMeetingFile();
    void run();
    ~Record_Manager();
    
    bool recording = false;
    bool meeting_recording = false;
    bool mfcc_on = false;

    private:
    string name;
    ofstream stream;
    string meeting_file_name;
    ofstream meeting_stream;
    string mfcc_file_name;
    ofstream mfcc_stream;
    Circular_Buffer *buffer;
    BlockingQueue<int16_t*> *audio_queue;
    BlockingQueue<float*> *mfcc_queue;
    int32_t chunkSize;
    enum event event;
};

#endif