#include "worker.h"
#include <QThread>

#include <Autogame.hpp>

// --- CONSTRUCTOR ---
Worker::Worker() {
    _stop = false;
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
}

// --- PROCESS ---
// Start processing data.
void Worker::process() {
    while (!_stop)
    {
        autogame::Update();
        QThread::msleep(4000);
    }
    emit finished();
}
