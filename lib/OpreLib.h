#ifndef OPRE_LIB_H
#define OPRE_LIB_H

#include "boolean.h"
#include <sys/types.h>
#include <semaphore.h>

struct NormalPipe {
    int p[2];
    bool isWritingOpen;
    bool isReadingOpen;
};

struct SharedMemory {
    bool active;
    int memId;
    void* memLoc;
};

struct Semaphore {
    char *name;
    sem_t *sem;
};

struct MessageQueue {
    int msqid;
};

struct MessageQueueData {
    long int msgtype;
    void* msgdata;
};

bool createChild(int (*)(), pid_t *);
bool sendSignal(pid_t, int);

int createNamedPipe(const char *);
void removeNamedPipe(const char *);

struct NormalPipe createPipe();
void pipeWrite(struct NormalPipe *, const void *, size_t);
ssize_t pipeRead(struct NormalPipe *, void *, size_t);
void closeReading(struct NormalPipe *);
void closeWriting(struct NormalPipe *);
void closePipe(struct NormalPipe *);

key_t createKey(const char *, int);

struct SharedMemory createSharedMemory(const char *, int,  size_t) ;
void* getMemoryLocation(struct SharedMemory *);
void releaseSharedMemory(struct SharedMemory *);
void deleteSharedMemory(struct SharedMemory *);

struct Semaphore createSemaphore(char *, int);
void waitForSemaphore(struct Semaphore *);
void activateSemaphore(struct Semaphore *);
void deleteSemaphore(struct Semaphore *);

struct MessageQueue createMessageQueue(const char *, int);
bool sendMessage(struct MessageQueue *, long int, void *, size_t);
ssize_t receiveMessage(struct MessageQueue *, long int, void *, size_t);
void deleteMessageQueue(struct MessageQueue *);

#endif