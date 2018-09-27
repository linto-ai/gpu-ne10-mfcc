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
using namespace rapidjson;


/**
* Init Record_Manager class
* Param: File name for the command
* Param: Boolean for pipe mode, true -> pipe, false -> file
* Param: File name for meeting files 
* Param: File name for mfcc file
* Param: Size of circular buffer
* Param: Size of chunk for audio data
* Param: Number of cepstral coefficients for MFCC (40 or 13)
*/
Record_Manager::Record_Manager(string filename,bool pipe_mode,string meeting_file_name,string mfcc_file_name,int32_t buffer_size,int32_t chunk_size,int num_cep,string server_pathname,boost::asio::io_service& io_service) {
    if (pipe_mode) {
        mkfifo(name.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    }
    this->name = filename;
    this->chunk_size = chunk_size;
    this->mfcc_file_name = mfcc_file_name;
    mfcc_stream.open (mfcc_file_name, ios::out | ios::binary);
    this->meeting_file_name = meeting_file_name;
    this->buffer = new Circular_Buffer(buffer_size);  
    this->server_pathname = server_pathname;
    unix_server = new server(io_service,server_pathname); 
    unix_server->set_num_cep(num_cep); 
}

/**
* Init the client tcp socket
* Param: String ip of server (can be "localhost")
* Param: String of port for server 
*/
void Record_Manager::initClient(string ip,string port) {
    client = new Client(port,ip);
}

/**
* Init the client unix socket
* Param: Pathname to socket.sock
*/
void Record_Manager::initClient(string pathname) {
    client = new Client(pathname);
}

/**
* Define the input queue for audio data
* Param: Pointer to audio queue
*/
void Record_Manager::setAudioInput(BlockingQueue<int16_t*>* queue) {
    audio_queue=queue;
}

/**
* Define the input queue for mfcc data
* Param: Pointer to float queue
*/
void Record_Manager::setMFCCInput(BlockingQueue<float*>* queue) {
    mfcc_queue=queue;
}

/**
* Write data to audio stream
* Param: Audio buffer
* Param: Data size
*/
void Record_Manager::writeAudio(int16_t* audio,int size) {
    if (!(stream.is_open())){
       cout << "File has been closed !" << endl;
    }
    else {
        stream.write((char*)audio, sizeof(int16_t)*size);
    } 
}

/**
* Write data to meeting stream
* Param: Audio buffer
*/
void Record_Manager::writeMeeting(int16_t* audio) {
    if (!(meeting_stream.is_open())) {
        cout << "Meeting File has been closed !" << endl;
    } else {
        meeting_stream.write((char*)audio, sizeof(int16_t)*chunk_size); 
    }
}

/**
* Write data to audio stream
* Param: Audio buffer
* Param: Data size
*/ 
void Record_Manager::writeMFCC(float* mfcc1,float* mfcc2) {
    if (!(mfcc_stream.is_open())) {
        cout << "MFCC File has been closed !" << endl;
    }
    mfcc_stream.write((char*)mfcc1, sizeof(float)*num_cep);
    mfcc_stream.write((char*)mfcc2, sizeof(float)*num_cep);
}

/**
* Open meeting stream with new file name
*/
void Record_Manager::OpenMeetingFile() {
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    this->meeting_file_name += to_string(ms.count()) + ".raw";
    this->meeting_stream.open(this->meeting_file_name, ios::out | ios::binary);
}

/**
* switch Record_Manager main states with MQTT messages
* You can add new messages here
*/
void Record_Manager::switchState() {
    if (new_message>0) { //TODO add mqtt protocol
        msg = mqtt_queue->pop();
        cout << "Topic: " << msg.topic << " Payload: " << msg.payload << endl;
        if ((msg.topic).compare("wuw/wuw-spotted") == 0) {
            recording = true;
            stream.open (name, ios::out | ios::binary);
            writeAudio(buffer->getBuffer(),buffer->getSize());
            cout << "Opening command file !" << endl;
        }
        else if((msg.topic).compare("utterance/stop") == 0) {
            recording = false;
            stream.close();
            cout << "Closing command file !" << endl;
        }
        else if((msg.topic).compare("lintoclient/action") == 0) {
            Document d;
            d.Parse((msg.payload).c_str());
            Value& s = d["value"];
            if (strcmp(s.GetString(),"start_meeting") == 0) {
                OpenMeetingFile();
                meeting_recording = true;
                cout << "Meeting Starting" << endl;
            }
            else if (strcmp(s.GetString(),"pause_meeting") == 0) {
                meeting_recording = false;
                cout << "Meeting on pause" << endl;
            }
            else if (strcmp(s.GetString(),"stop_meeting") == 0) {
                meeting_recording = false;
                meeting_stream.close();
                cout << "Meeting ended" << endl;
            }
            else if (strcmp(s.GetString(),"resume_meeting") == 0) {
                meeting_recording = true;
                cout << "Resuming meeting" << endl;
            }
        }
        new_message--;
    }
}


/**
* main thread function
*/
void Record_Manager::run() {
    int16_t* audio_input;
    while(true) {
        switchState();
        audio_input = audio_queue->pop();
        buffer->add(audio_input,chunk_size);
        if (recording) {
            writeAudio(audio_input,chunk_size);
        }
        if (meeting_recording) {
            writeMeeting(audio_input);
        }
        if (mfcc_on) { 
            unix_server->mfcc_input_1 = mfcc_queue->pop();
            unix_server->mfcc_input_2 = mfcc_queue->pop();
            writeMFCC(unix_server->mfcc_input_1,unix_server->mfcc_input_2);
        }
   }
}

/**
* Delete Record_Manager
* Close all streams and files.
*/
Record_Manager::~Record_Manager() {
    stream.close();
    meeting_stream.close();
    mfcc_stream.close();
}