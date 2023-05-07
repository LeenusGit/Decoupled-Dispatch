#pragma once

#include <functional>
#include <string>

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
    Error,
    BigEvent,
};

struct Event {
    EventId id{};
    Event(EventId id) : id{id} { }
    virtual ~Event() { }
};

struct Tick : public Event { 
    Tick() : Event{EventId::Tick} { }
    size_t delta{};
};

struct DataReadyRead: public Event { 
    char* src{};
    size_t len{};
    DataReadyRead() : Event{EventId::DataReadyRead} {}
};

struct StateChanged : public Event {
    StateChanged() : Event{EventId::StateChanged} {}
    int newState{};
};
struct ConfigChanged : public Event {
    ConfigChanged() : Event{EventId::ConfigChanged} {}
    int id{};
};

struct CallBackEvent : public Event  { 
    CallBackEvent() : Event{EventId::CallBackEvent} {}
    std::function<void()> cb{}; 
};

struct Event1 : public Event {
    Event1() : Event{EventId::Event1} { }
};
struct Event2 : public Event {
    Event2() : Event{EventId::Event2} { }
};
struct Event3 : public Event {
    Event3() : Event{EventId::Event3} { }
};
struct Error : public Event {
    Error() : Event{EventId::Error} { }
    int id{};
    std::string msg{}; 
};
struct BigEvent : public Event { 
    BigEvent() : Event{EventId::BigEvent} { }
    char data[1024] = {0}; 
};
