//make sure that we are compiling for pcolony simulator on PC
#ifndef PCOL_SIM
#define PCOL_SIM
#endif
#include "lulu.h"
#include "instance.h"
#include "debug_print.h"
#include <stdlib.h> //for rand(), srand()
#include <stdio.h>
#include <string.h> //for strcpy

static char outputBuffer[255];

char* printMultisetEnv(multiset_env_t *multiset) {
    memset(outputBuffer, '\0', 255);
    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i].id != NO_OBJECT)
            sprintf(outputBuffer, "%s '%s': %d, ", outputBuffer, objectNames[multiset->items[i].id], multiset->items[i].nr);
    return outputBuffer;
}

char* printMultisetObj(multiset_obj_t *multiset) {
    memset(outputBuffer, '\0', 255);
    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i] != NO_OBJECT)
            sprintf(outputBuffer, "%s '%s', ", outputBuffer, objectNames[multiset->items[i]]);
    return outputBuffer;
}

void printColonyState(Pcolony_t *pcol) {

    printf("\n    Pcolony.env = [%s]", printMultisetEnv(&pcol->env));
    printf("\n    Pswarm.env = [%s]", printMultisetEnv(&pcol->pswarm.global_env));
    for (uint8_t i = 0; i < pcol->nr_agents; i++)
        printf("\n    %s.obj = [%s]", agentNames[i], printMultisetObj(&pcol->agents[i].obj));
}

int main(int argc, char **argv) {
    Pcolony_t pcol;
    uint8_t step_nr = 0;

    srand(8312);

    lulu_init(&pcol);

    printi(("Initial configuration:"));
    printColonyState(&pcol);

    expand_pcolony(&pcol, 71);
    printi(("Configuration after wildcard expansion:"));
    printColonyState(&pcol);

    while (1) {
        sim_step_result_t result = SIM_STEP_RESULT_FINISHED;

        printi(("Running simulation step %d", step_nr));

        result = pcolony_runSimulationStep(&pcol);

        printColonyState(&pcol);

        if (result == SIM_STEP_RESULT_NO_MORE_EXECUTABLES) {
            printi(("Simulation finished sucesfully"));
            return 0;
        }
        else if (result == SIM_STEP_RESULT_ERROR) {
            printe(("Error encountered"));
            return 1;
        }

        step_nr++;
    }

    lulu_destroy(&pcol);

    return 0;
}
