#include "shared.h"
#include "../lib/includes.h"

void sigHandler(int signumber) {
    printf("Signal: %d\n", signumber);
}