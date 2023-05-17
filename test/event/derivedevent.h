#pragma once

#include <functional>
#include <string>

#include <chrono>

using namespace std::chrono;

enum class EventId {
    Any,
    Tick,
    DataReadyRead,
    StateChanged,
    ConfigChanged,
    CallBackEvent,
    Event1,
    Event2,
    Event3,
    Event4,
    Event5,
    Error,
    BigEvent,
};

struct EventBase {
    EventId id{};
    EventBase(EventId id) : id{id} { }
    virtual ~EventBase() { }
};

// struct Tick : public Event { 
//     Tick() : Event{EventId::Tick} { }
//     size_t delta{};
// };

// struct DataReadyRead: public Event { 
//     char* src{};
//     size_t len{};
//     DataReadyRead() : Event{EventId::DataReadyRead} {}
// };

// struct StateChanged : public Event {
//     StateChanged() : Event{EventId::StateChanged} {}
//     int newState{};
// };
// struct ConfigChanged : public Event {
//     ConfigChanged() : Event{EventId::ConfigChanged} {}
//     int id{};
// };

// struct CallBackEvent : public Event  { 
//     CallBackEvent() : Event{EventId::CallBackEvent} {}
//     std::function<void()> cb{}; 
// };

struct DerivedEvent1 : public EventBase {
    DerivedEvent1() :
    EventBase{EventId::Event1}
    // start{high_resolution_clock::now()} 
    { }
    // time_point<high_resolution_clock> start;
    // char data[1024];
};
struct DerivedEvent2 : public EventBase {
    DerivedEvent2() : EventBase{EventId::Event2} { }
    // char data[1024];
};

struct DerivedEvent3 : public EventBase {
    DerivedEvent3() : EventBase{EventId::Event3} { }
    // char data[1024];
};


struct DerivedEvent4 : public EventBase {
    DerivedEvent4() : EventBase{EventId::Event4} { }
    // char data[1024];
};


struct DerivedEvent5 : public EventBase {
    DerivedEvent5() : EventBase{EventId::Event5} { }
    // char data[1024];
};


// struct Error : public Event {
//     Error() : Event{EventId::Error} { }
//     int id{};
//     std::string msg{}; 
// };
// struct BigEvent : public Event { 
//     BigEvent() : Event{EventId::BigEvent} { }
//     // char data[1024] = {0}; 
// };
