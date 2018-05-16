#include "oktato.h"
#include "shared.h"
#include "../lib/includes.h"
#include "../lib/OpreLib.h"

int oktato_main(pid_t tanulopid) {

    closeReading(&countdownpipe);

    printf("OKTATO: Waiting for signal...\n");
    pause();

    for (int i = 1; i <= 3; ++i) {
        sleep(1);
        printf("OKTATO: %d\n", i);
        pipeWrite(&countdownpipe, &i, sizeof(i));
    }

    closeWriting(&countdownpipe);

    struct StrMessage msg;

    receiveMessage(&messages, 0, &msg, sizeof(char) * MESSAGESIZE);
    printf("OKTATO: Received message: %s My PID: %d\n", msg.message, getpid());

    msg.type = 2;
    strcpy(msg.message, "Rantsd mar meg az ejtoernyo kioldojat Te szerencsetlen!");
    printf("OKTATO: Sending message: %s\n", msg.message);
    sendMessage(&messages, 2, &msg, sizeof(char) * MESSAGESIZE);

    *pontszam = rand() % 11;
    printf("OKTATO: Random points: %d\n", *pontszam);
    activateSemaphore(&semf);

    return 0;
}