#ifndef SHARED_H
#define SHARED_H

#include "../lib/OpreLib.h"

struct NormalPipe countdownpipe;
struct MessageQueue messages;
struct SharedMemory smem;
int *pontszam;
struct Semaphore semf;

#define MESSAGESIZE 1024

void sigHandler(int signumber);

struct StrMessage {
    long type;
    char message[MESSAGESIZE];
};

#endif