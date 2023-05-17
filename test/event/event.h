#pragma once

#include <functional>
#include <string>
#include <chrono>

using namespace std::chrono;

struct Tick { size_t delta;};
struct DataReadyRead{ char* src; size_t len; };
struct StateChanged {int newState;};
struct ConfigChanged {int id;};
struct CallBackEvent  { 
    std::function<void()> cb; 
};
struct Event1 {
    // time_point<high_resolution_clock> start = high_resolution_clock::now();
};
struct Event2 {
    // char data[1024];
};
struct Event3 {
    // char data[1024];
};
struct Event4 {
    // char data[1024];
};
struct Event5 {
    // char data[1024];
};

struct Error { 
    int id; std::string msg; 
};
struct BigEvent{ 
    char data[1024]; 
};
