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

#include "recordmanager.h"
//#include "audio.h"
using namespace std;

int main(int argc, char* argv[])
{
    /*AudioParameter audioParams;
        audioParams.sampleRate = 16000;
        audioParams.channels = 1;
        audioParams.chunkSize = 1024;
    AudioInput* input = new AudioInput(&audioParams);
    VADFeatProcessor* vadFeatProcessor = new VADFeatProcessor();
    VADEngine* vadEngine = new VADEngine();*/
    Record_Manager *manager = new Record_Manager("test.raw",false,"meeting",4000,1024);
    
    // Link modules
    /*vadFeatProcessor->set_input(input->subscribe());
    vadEngine->setInput(vadFeatProcessor->subscribe());
    manager->setInput(input->subscribe());
    meeting_manager->setInput(input->subscribe());*/

    // Run modules in threads
    /*std::thread vadfeat_thread(&VADFeatProcessor::run, vadFeatProcessor);
    std::thread audio_thread(&AudioInput::run, input);
    std::thread vad_thread(&VADEngine::run, vadEngine);
    std::thread manager_thread(&Record_Manager::run, manager);
    std::thread meeting_thread(&Record_Manager::run_meeting, meeting_manager) */
    

    /*vadEngine->detectUtterance();
    std::cin.ignore();

    audio_thread.join();
    vadfeat_thread.join();*/

    if(manager->test()==false) {
        cout << "Test run failed..." << endl;
    }
    return 0;
}