#include "OpreLib.h"
#include "includes.h"

bool createChild(int (*childStartMethod)(), pid_t *pid) {
    *pid = fork();

    if (*pid < 0) {
        printf("Fork failed");
        return false;
    }

    if (*pid == 0) {
        int status = childStartMethod();
        exit(status);
    }

    return true;
}

bool sendSignal(pid_t process, int signal) {
    if (kill(process, signal) < 0) {
        printf("Failed to send signal: %d\n", errno);
        return false;
    } else {
        return true;
    }
}

int createNamedPipe(const char *name) {
    int fid = mkfifo(name, S_IRUSR|S_IWUSR);
    if (fid < 0) {
        if (errno == EEXIST) {
            printf("WARNING: The pipe named %s was left open! Do you want to remove the pipe? [Y/n] ", name);
            char rmchar;
            scanf(" %c", &rmchar);
            if (rmchar == 'Y') {
                removeNamedPipe(name);
                return createNamedPipe(name);
            } else {
                exit(-1);
            }
        } else {
            printf("Failed to create pipe: %s Error: %i\n", name, errno);
            exit(-1);
        }
    }
    return fid;
}

void removeNamedPipe(const char *name) {
    unlink(name);
}

struct NormalPipe createPipe() {
    struct NormalPipe newPipe;

    if (pipe(newPipe.p) < 0) {
        printf("Failed to open pipe.\n");
        exit(-1);
    }

    newPipe.isReadingOpen = true;
    newPipe.isWritingOpen = true;

    return newPipe;
}

void pipeWrite(struct NormalPipe *pp, const void *data, size_t size) {
    write(pp->p[1], data, size);
}

ssize_t pipeRead(struct NormalPipe *pp, void *data, size_t size) {
    return read(pp->p[0], data, size);
}

void closeReading(struct NormalPipe *pp) {
    if (pp->isReadingOpen) {
        close(pp->p[0]);
        pp->isReadingOpen = false;
    }
}

void closeWriting(struct NormalPipe *pp) {
    if (pp->isWritingOpen) {
        close(pp->p[1]);
        pp->isWritingOpen = false;
    }
}

void closePipe(struct NormalPipe *pp) {
    closeReading(pp);
    closeWriting(pp);
}

key_t createKey(const char *path, int keyid) {
    key_t key = ftok(path, keyid);
    if (key < 0) {
        printf("Error creating key: %d\n", errno);
        exit(-1);
    }
    return key;
}

struct SharedMemory createSharedMemory(const char* path, int keyid,  size_t size) {
    struct SharedMemory mem;

    key_t key = createKey(path, keyid);
    mem.memId = shmget(key, size, IPC_CREAT|S_IRUSR|S_IWUSR);
    if (mem.memId < 0) {
        printf("Error creating shared memory: %d\n", errno);
        exit(-1);
    }
    mem.memLoc = shmat(mem.memId, NULL, 0);
    if (mem.memLoc < 0) {
        printf("Error getting shared memory address: %d\n", errno);
        exit(-1);
    }
    mem.active = true;

    return mem;
}

void* getMemoryLocation(struct SharedMemory *mem) {
    return mem->memLoc;
}

void releaseSharedMemory(struct SharedMemory *mem) {
    mem->active = false;
    shmdt(mem->memLoc);
}

void deleteSharedMemory(struct SharedMemory *mem) {
    mem->active = false;
    shmctl(mem->memId, IPC_RMID, NULL);
}

struct Semaphore createSemaphore(char *name, int def) {
    struct Semaphore sm;
    sm.name = name;
    sm.sem = sem_open(name, O_CREAT, S_IRUSR|S_IWUSR, def);
    if (sm.sem == SEM_FAILED) {
        printf("Semaphore creation failed: %d\n", errno);
        exit(1);
    }
    return sm;
}

void deleteSemaphore(struct Semaphore *sm) {
    sem_unlink(sm->name);
}

void waitForSemaphore(struct Semaphore *sm) {
    sem_wait(sm->sem);
}

void activateSemaphore(struct Semaphore *sm) {
    sem_post(sm->sem);
}

/*struct SemaphoreGroup createSemaphoreGroup(const char *path, int keyid, int count) {
    struct SemaphoreGroup sm;

    key_t key = createKey(path, keyid);
    sm.semId = semget(key, count, IPC_CREAT|S_IRUSR|S_IWUSR);
    if (sm.semId < 0) {
        printf("Semaphore creation failed: %d\n", errno);
        exit(1);
    }

    return sm;
}*/

struct MessageQueue createMessageQueue(const char *path, int keyid) {
    struct MessageQueue ms;

    key_t key = createKey(path, keyid);
    ms.msqid = msgget(key, IPC_CREAT|S_IRUSR|S_IWUSR);
    if (ms.msqid < 0) {
        printf("Error creating message queue: %d\n", errno);
        exit(-1);
    }

    return ms;
}

bool sendMessage(struct MessageQueue *ms, long int type, void *data, size_t size) {
    int status = msgsnd(ms->msqid, data, size, 0);
    bool ok = (status >= 0) ? true : false;
    return ok;
}

ssize_t receiveMessage(struct MessageQueue *ms, long int type, void *data, size_t size) {
    return msgrcv(ms->msqid, data, size, type, 0);
}

void deleteMessageQueue(struct MessageQueue *ms) {
    msgctl(ms->msqid, IPC_RMID, NULL);
}