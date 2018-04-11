#pragma once

#include "ITrigger.h"
#include "engine/AlicaClock.h"

#include <vector>
#include <thread>
#include <condition_variable>
#include <iostream>

using alica::AlicaTime;
using alica::AlicaClock;

namespace supplementary {

template <class NotificationClass>
using t_notificationcallback = void (NotificationClass::*)();

template <class NotificationClass>
class NotifyTimer : public virtual ITrigger {
public:
    NotifyTimer(AlicaTime interval, t_notificationcallback<NotificationClass> callback, NotificationClass* obj);
    ~NotifyTimer();
    bool start();
    bool stop();
    bool isRunning();
    bool isStarted();
    void setInterval(AlicaTime interval);
    const AlicaTime getInterval() const;
    void run(bool notifyAll = false);
    void registerCV(condition_variable* condVar);

private:
    thread* runThread;
    AlicaClock _clock;
    AlicaTime interval; /** < The time between two fired events */
    bool running, started;
    t_notificationcallback<NotificationClass> callback;
    NotificationClass* obj;
};

template <class NotificationClass>
NotifyTimer<NotificationClass>::NotifyTimer(
        AlicaTime interval, t_notificationcallback<NotificationClass> callback, NotificationClass* obj) {
    this->started = true;
    this->running = false;
    this->interval = interval;
    this->runThread = new thread(&NotifyTimer::run, this, false);
    this->callback = callback;
    this->obj = obj;
}

template <class NotificationClass>
void NotifyTimer<NotificationClass>::run(bool notifyAll) {
    while (this->started) {
        if (!this->started)  // for destroying the NotifyTimer
            return;

        AlicaTime start = _clock.now();
        if (this->running) {
            (obj->*callback)();
        }
        //			XXX: call function here
        AlicaTime dura = _clock.now() - start;
        //			cout << "NotifyTimerEvent: Duration is " <<
        // dura.inNanoseconds()
        //					<< " nanoseconds" << endl;
        _clock.sleep(interval - dura);
    }
}

template <class NotificationClass>
NotifyTimer<NotificationClass>::~NotifyTimer() {
    this->running = false;
    this->started = false;
    this->runThread->join();
    delete this->runThread;
}

template <class NotificationClass>
bool NotifyTimer<NotificationClass>::start() {
    if (this->started && !this->running) {
        this->running = true;
    }
    return this->started && this->running;
}

template <class NotificationClass>
bool NotifyTimer<NotificationClass>::stop() {
    if (this->started && this->running) {
        this->running = false;
    }
    return this->started && this->running;
}

template <class NotificationClass>
bool NotifyTimer<NotificationClass>::isRunning() {
    return this->running;
}

template <class NotificationClass>
bool NotifyTimer<NotificationClass>::isStarted() {
    return this->started;
}

template <class NotificationClass>
void NotifyTimer<NotificationClass>::setInterval(AlicaTime interval) {
    this->interval = interval;
}

template <class NotificationClass>
const AlicaTime NotifyTimer<NotificationClass>::getInterval() const {
    return interval;
}

} /* namespace supplementary */

template <class NotificationClass>
inline void supplementary::NotifyTimer<NotificationClass>::registerCV(condition_variable* condVar) {}
