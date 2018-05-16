#include "lib/OpreLib.h"
#include "lib/includes.h"

#include "mains/shared.h"
#include "mains/oktato.h"
#include "mains/tanitvany.h"

int main(int argc, char* argv[]) {
    printf("Initializing program...\n");
    pid_t tanitvanypid;
    countdownpipe = createPipe();
    messages = createMessageQueue(argv[0], 3);
    smem = createSharedMemory(argv[0], 1, sizeof(int));
    pontszam = (int*) getMemoryLocation(&smem);
    semf = createSemaphore("eredmeny_vedelem", 0);

    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);

    printf("Forking child...\n");
    if (!createChild(tanitvany_main, &tanitvanypid)) {
        exit(-1);
    }
    printf("Starting main process...\n");
    oktato_main(tanitvanypid);
    printf("Main process done.\n");

    printf("Waiting for child processes...\n");
    wait(NULL);
    printf("Child processes stopped.\n");

    printf("Deleting assets...\n");
    deleteMessageQueue(&messages);
    deleteSharedMemory(&smem);
    deleteSemaphore(&semf);

    printf("Exiting.\n");
    return 0;
}