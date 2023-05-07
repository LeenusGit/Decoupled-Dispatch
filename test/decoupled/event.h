#pragma once

#include <functional>
#include <string>

struct Tick { size_t delta;};
struct DataReadyRead{ char* src; size_t len; };
struct StateChanged {int newState;};
struct ConfigChanged {int id;};
struct CallBackEvent  { 
    std::function<void()> cb; 
};
struct Event1 {};
struct Event2 {};
struct Event3 {};
struct Error { 
    int id; std::string msg; 
};
struct BigEvent{ 
    char data[1024]; 
};
