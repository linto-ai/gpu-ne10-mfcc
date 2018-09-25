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

#include "../include/config.h"
using namespace std;
using namespace rapidjson;

/**
 * Parse config json file
 * Param: Pathname of json file
 * Param: Struct to save data
 */
void parseConfigFile(string pathname,config *conf) {
    Document d;
    FILE* fp = fopen(pathname.c_str(), "rb");
    if(fp == 0) {
        std::cout << "Failed to open file." << std::endl;
    }
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    d.ParseStream(is);
    fclose(fp);
    conf->audio.sample_rate = (d["audio"]["sample_rate"]).GetInt();
    conf->audio.channels = (d["audio"]["channels"]).GetInt();
    const Value& topics = d["mqtt"]["topics"];
    assert(topics.IsArray());
    for (SizeType i = 0; i < topics.Size(); i++) // rapidjson uses SizeType instead of size_t.
        conf->mqtt.topics[i]=topics[i].GetString();
    conf->mqtt.topics_number = topics.Size();
    conf->mqtt.mqtt_addr = (d["mqtt"]["mqtt_addr"]).GetString(); 
    conf->mqtt.mqtt_port = (d["mqtt"]["mqtt_port"]).GetInt(); 
    conf->mqtt.mqtt_qos = (d["mqtt"]["mqtt_qos"]).GetInt(); 
    conf->files.file_name = (d["files"]["file_name"]).GetString(); 
    conf->files.pipe_mode = (d["files"]["pipe_mode"]).GetBool(); 
    conf->files.meeting_file = (d["files"]["meeting_file"]).GetString();
    conf->files.mfcc_file = (d["files"]["mfcc_file"]).GetString();
    conf->files.circular_buffer_size = (d["files"]["circular_buffer_size"]).GetInt();
    conf->mfcc.frame_size = (d["mfcc"]["frame_size"]).GetInt();
    conf->mfcc.sliding_samples = (d["mfcc"]["sliding_samples"]).GetInt();  
    conf->mfcc.num_cep = (d["mfcc"]["num_cep"]).GetInt();  
    conf->mfcc.num_bins = (d["mfcc"]["num_bins"]).GetInt();  
    const char* window_type = (d["mfcc"]["window_type"]).GetString(); 
    if (strcmp(window_type,"blackman")==0) {
        conf->mfcc.window = blackman;
    }   
    else if (strcmp(window_type,"povey")==0) {
        conf->mfcc.window = povey;
    }   
    else if (strcmp(window_type,"hamming")==0) {
        conf->mfcc.window = hamming;
    }   
    else if (strcmp(window_type,"hann")==0) {
        conf->mfcc.window = hann;
    } 
}