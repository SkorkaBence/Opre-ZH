#include "tanitvany.h"
#include "shared.h"
#include "../lib/includes.h"
#include "../lib/OpreLib.h"

int tanitvany_main() {

    closeWriting(&countdownpipe);

    printf("TANITVANY: Getting ready...\n");
    sleep(1);
    pid_t oktatopid = getppid();
    printf("TANITVANY: Sending ready signal...\n");
    sendSignal(oktatopid, SIGUSR1);

    int countdown;
    while (pipeRead(&countdownpipe, &countdown, sizeof(countdown))) {
        printf("TANITVANY: %d\n", countdown);
    }

    closeReading(&countdownpipe);

    char message[MESSAGESIZE];

    struct StrMessage msg = {1, "Mit csinaljak?"};
    printf("TANITVANY: Sending message: %s\n", msg.message);
    if (sendMessage(&messages, 1, &msg, sizeof(char) * MESSAGESIZE)) {
        printf("TANITVANY: Sent!\n");
    } else {
        printf("TANITVANY: Failed!\n");
    }

    receiveMessage(&messages, 0, &msg, sizeof(char) * MESSAGESIZE);
    printf("TANITVANY: Received message: %s My PID: %d\n", msg.message, getpid());

    waitForSemaphore(&semf);
    printf("TANITVANY: My points: %d!\n", *pontszam);

    return 0;
}