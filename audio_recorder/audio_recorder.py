#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import subprocess
import time
import argparse
import shlex
import logging
import configparser
import json


def main():
    # Logging
    logging.basicConfig(level=logging.DEBUG, format="%(levelname)8s %(asctime)s %(message)s ")
    # Read default config from file
    config = configparser.ConfigParser()
    config.read(os.path.dirname(os.path.abspath(__file__)) + "/config.conf")
    sampling_rate = config['ENGINE']['sampling_rate']
    circular_buffer_time = config['ENGINE']['circular_buffer_time']
    channels = config['ENGINE']['channels']
    broker_ip = config['BROKER']['broker_ip']
    broker_port = config['BROKER']['broker_port']
    broker_listening_topic = config['BROKER']['broker_listening_topic']
    broker_listening_keyword_start = config['BROKER']['broker_listening_keyword_start']
    broker_listening_keyword_stop = config['BROKER']['broker_listening_keyword_stop']
    broker_listening_topic_bis = config['BROKER']['broker_listening_topic_bis']
    pipe = config['FILES']['pipe_name']
    file = config['FILES']['file_name']
    parser = argparse.ArgumentParser(
        description='Module to command audio recorder easily')
    parser.add_argument('--mode', dest = 'mode' , default='file',
                        help="Mode to save data, file or pipe")
    args = parser.parse_args()
    if args.mode == 'file':
        param = ' ' + file + ' ' + sampling_rate + ' ' + channels + ' ' + circular_buffer_time + ' ' + broker_ip + ' ' + broker_port + ' ' + broker_listening_topic
        param += ' ' + broker_listening_keyword_start + ' ' +broker_listening_keyword_stop + ' ' + broker_listening_topic_bis + ' '
        while 1:
            output = subprocess.call(shlex.split('/home/pi/audio_recorder/audio_recorder'+param))
            logging.info(param)
            if output != 0:
                logging.info("Audio Recorder failed.")
                logging.info("Trying to restart in 5s.")
                time.sleep(5)
    elif args.mode == 'pipe':
        param = ' ' + pipe + ' ' + sampling_rate + ' ' + channels + ' ' + circular_buffer_time + ' ' + broker_ip + ' ' + broker_port + ' ' + broker_listening_topic
        param += ' ' + broker_listening_keyword_start + ' ' +broker_listening_keyword_stop + ' ' + broker_listening_topic_bis + ' '
        param += ' ' + "pipe"
        while 1:
            output = subprocess.call(shlex.split('/home/pi/audio_recorder/audio_recorder'+param))
            logging.info(param)
            if output != 0:
                logging.info("Audio Recorder failed.")
                logging.info("Trying to restart in 5s.")
                time.sleep(5)

if __name__ == '__main__':
    main()
