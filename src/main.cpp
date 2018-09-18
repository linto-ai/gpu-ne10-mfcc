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
#include <iostream>
#include <thread>
#include <unistd.h>

#include "../include/recordmanager.h"
#include "../include/audio.h"
#include "../include/mfcc.h"
#include "../include/config.h"
using namespace std;

int main(int argc, char* argv[])
{
    //TODO Add config file
    config conf;
    parseConfigFile("config.json",&conf);
    MFCC *mfcc = new MFCC(conf.mfcc.frame_size,conf.mfcc.sliding_samples,conf.mfcc.window); // MFCC window size and sliding part
    AudioParameter audioParams;
        audioParams.sampleRate = conf.audio.sample_rate;
        audioParams.channels = conf.audio.channels;
        audioParams.chunkSize = 2*conf.mfcc.frame_size-conf.mfcc.sliding_samples; 
    Record_Manager *manager = new Record_Manager(conf.files.file_name,conf.files.pipe_mode,conf.files.meeting_file,conf.files.mfcc_file,conf.files.mfcc_string_file,conf.files.circular_buffer_size,audioParams.chunkSize,conf.mfcc.num_cep);
    AudioInput* input = new AudioInput(&audioParams);
    MQTT_Client *mqtt = new MQTT_Client(conf.mqtt.mqtt_addr,conf.mqtt.mqtt_port,conf.mqtt.mqtt_qos,conf.mqtt.topics,conf.mqtt.topics_number);
    // Link modules
    mfcc->setInput(input->subscribe());
    manager->setAudioInput(input->subscribe());
    manager->setMFCCInput(mfcc->subscribe());
    manager->setStringMFCCInput(mfcc->stringSubscribe());
    // start thread
    std::thread manager_thread(&Record_Manager::run,manager);
    std::thread audio_thread(&AudioInput::run, input);
    std::thread mfcc_thread(&MFCC::compute, mfcc);
    cout << "All thread started" << endl;
    mfcc_thread.join();
    manager_thread.join();
    audio_thread.join();
    return 0;
}