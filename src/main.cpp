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
using namespace std;

int main(int argc, char* argv[])
{
    //TODO Add config file
    int frame_size = 400,sliding_samples = 160,num_cep = 13;
    MFCC *mfcc = new MFCC(frame_size,sliding_samples,povey); // MFCC window size and sliding part
    AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 2*frame_size-sliding_samples; // 2 windows for MFCC less sliding part
    Record_Manager *manager = new Record_Manager("test.raw",false,"meeting","mfcc.out",800,audioParams.chunkSize,num_cep);
    AudioInput* input = new AudioInput(&audioParams);
    string topics[4] = {"wuw/wuw-spotted","utterance/start","utterance/stop","lintoclient/action"};
    MQTT_Client *mqtt = new MQTT_Client("localhost",1883,1,topics,4);
    // Link modules
    mfcc->setInput(input->subscribe());
    manager->setAudioInput(input->subscribe());
    manager->setMFCCInput(mfcc->subscribe());
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