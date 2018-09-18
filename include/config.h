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
#ifndef CONFIG_H
#define CONFIG_H
#include <cstdio>
#include <iostream>
#include <fstream>
#include "mfcc.h"
#include "mqtt_client.h"
#include "recordmanager.h"
#include "rapidjson/filereadstream.h"

struct audio_config {
    int sample_rate;
    int channels;
};

struct mqtt_config {
    string topics[5];
    string mqtt_addr;
    int mqtt_port;
    int mqtt_qos;
    int topics_number;
};

struct mfcc_config {
    int frame_size;
    int sliding_samples;
    int num_cep;
    int num_bins;
    windows_type window;
    int low_frequency;
    int high_frequency;
    float pre_emph_coef;
    float cepstral_lifter_coef;
};

struct files_config {
    string file_name;
    bool pipe_mode;
    string meeting_file;
    string mfcc_file;
    string mfcc_string_file;
    int circular_buffer_size;
};

struct config {
    mqtt_config mqtt;
    mfcc_config mfcc;
    files_config files;
    audio_config audio;
};


void parseConfigFile(string pathname,config *conf);

#endif