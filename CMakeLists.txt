cmake_minimum_required(VERSION 3.0)
project(audio)
set(SOURCES
    main.cpp
    audio.cpp
    blockingqueue.cpp
    )

set(HEADERS
    audio.h 
    blockingqueue.h
    )

add_executable(audio ${SOURCES} ${HEADERS})

find_package(lpuse-simple lpulse)