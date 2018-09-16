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

Record_Manager::Record_Manager(string filename,bool pipe_mode,string meeting_file_name,string mfcc_file_name,int32_t buffer_size,int32_t chunkSize,int num_cep=13) {
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


void Record_Manager::writeData(int16_t* data,float *mfcc1,float* mfcc2,int num_cep) {
    if (recording) {
       if (!(stream.is_open())){
           stream.open (name, ios::out | ios::binary);
        } 
        stream.write((char*)data, sizeof(int16_t)*chunkSize);
    }
    if (meeting_recording) {
        if (!(meeting_stream.is_open())) {
            OpenMeetingFile(); 
        }
        meeting_stream.write((char*)data, sizeof(int16_t)*chunkSize); 
    }
    if (mfcc_on) {
        if (!(mfcc_stream.is_open())) {
            mfcc_stream.open (mfcc_file_name, ios::out | ios::binary);
        }
        mfcc_stream.write((char*)mfcc1, sizeof(float)*num_cep);
        mfcc_stream.write((char*)mfcc2, sizeof(float)*num_cep);
    }
}

void Record_Manager::OpenMeetingFile() {
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    this->meeting_file_name += to_string(ms.count()) + ".raw";
    this->meeting_stream.open(this->meeting_file_name, ios::out | ios::binary);
}


void Record_Manager::switchState() {
    if (new_message) { //TODO add mqtt protocol
        msg = mqtt_queue->pop();
        Document d;
        d.Parse((msg.payload).c_str());
        if ((msg.topic).compare("wuw/wuw-spotted")) {
            recording = true;
        }
        else if((msg.topic).compare("utterance/stop")) {
            recording = false;
            stream.close();
        }
        else if((msg.topic).compare("lintoclient/action")) {
            Value& s = d["value"];
            if (strcmp(s.GetString(),"start_meeting")==0 ) {
                meeting_recording = true;
            }
            else if (strcmp(s.GetString(),"pause_meeting")==0 ) {
                meeting_recording = false;
            }
            else if (strcmp(s.GetString(),"stop_meeting")==0 ) {
                meeting_recording = false;
                meeting_stream.close();
            }
            else if (strcmp(s.GetString(),"resume_meeting")==0 ) {
                meeting_recording = true;
            }
        }
        new_message = false;
    }
}

void Record_Manager::run() {
    int16_t* audio_input;
    float *mfcc_input_1,*mfcc_input_2;
    while(true) {
        switchState();
        audio_input = audio_queue->pop();
        mfcc_input_1 = mfcc_queue->pop();
        /*for (int i=0;i<num_cep;i++) {
            cout << mfcc_input_1[i] << " ";
        }*/
        mfcc_input_2 = mfcc_queue->pop();
        /*cout << endl;
        for (int i=0;i<num_cep;i++) {
            cout << mfcc_input_2[i] << " ";
        }
        cout << endl;*/
        writeData(audio_input,mfcc_input_1,mfcc_input_2,num_cep);
   }
}

Record_Manager::~Record_Manager() {
    stream.close();
    meeting_stream.close();
    mfcc_stream.close();
}