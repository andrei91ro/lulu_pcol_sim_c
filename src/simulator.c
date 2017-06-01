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

char* printProgram(Program_t *program) {
    memset(outputBuffer, '\0', 255);
    for (uint8_t i = 0; i < program->nr_rules; i++)
        //if this is a non-conditional rule
        if (program->rules[i].type < RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION)
            // x -> y
            sprintf(outputBuffer, "%s %s%s%s, ", outputBuffer,
                    objectNames[program->rules[i].lhs],
                    ruleNames[program->rules[i].type],
                    objectNames[program->rules[i].rhs]);
        else
            // x -> y / z -> w
            sprintf(outputBuffer, "%s %s%s%s / %s%s%s, ", outputBuffer,
                    objectNames[program->rules[i].lhs],
                    ruleNames[lookupFirst[program->rules[i].type - RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION]],
                    objectNames[program->rules[i].rhs],
                    // alternative fields
                    objectNames[program->rules[i].alt_lhs],
                    ruleNames[lookupSecond[program->rules[i].type - RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION]],
                    objectNames[program->rules[i].alt_rhs]);
    
    return outputBuffer;
}

void printColonyState(Pcolony_t *pcol, bool with_programs) {

    printf("\n \e[32m   Pcolony.env = [%s]\e[0m", printMultisetEnv(&pcol->env));
    printf("\n \e[34m   Pswarm.global_env = [%s]\e[0m", printMultisetEnv(&pcol->pswarm.global_env));
    printf("\n \e[35m   Pswarm.in_global_env = [%s]\e[0m", printMultisetEnv(&pcol->pswarm.in_global_env));
    printf("\n \e[36m   Pswarm.out_global_env = [%s]\e[0m", printMultisetEnv(&pcol->pswarm.out_global_env));
    for (uint8_t i = 0; i < pcol->nr_agents; i++) {
        printf("\n    %s.obj = [%s];", agentNames[i], printMultisetObj(&pcol->agents[i].obj));
        if (with_programs) {
            printf(" nr_programs = %d", pcol->agents[i].nr_programs);
            for (uint8_t prg_nr = 0; prg_nr < pcol->agents[i].nr_programs; prg_nr++)
                printf("\n        P%d = < %s >", prg_nr, printProgram(&pcol->agents[i].programs[prg_nr]));
        }
    }
}

int main(int argc, char **argv) {
    //Pcolony_t pcol;
    uint8_t step_nr = 0;

    srand(8312);

    //lulu_init(&pcol);

    printi("Initial configuration:");
    printColonyState(&pcol, TRUE);

#ifdef NEEDING_WILDCARD_EXPANSION
    printi("Configuration after wildcard expansion:");
    expand_pcolony(&pcol, 0);
    printColonyState(&pcol, TRUE);
#endif

    while (1) {
        sim_step_result_t result = SIM_STEP_RESULT_FINISHED;

        printi("Running simulation step %d", step_nr);

        result = pcolony_runSimulationStep(&pcol);

        printColonyState(&pcol, FALSE);

        if (result == SIM_STEP_RESULT_NO_MORE_EXECUTABLES) {
            printi("Simulation finished sucesfully");
            //lulu_destroy(&pcol);
            return 0;
        }
        else if (result == SIM_STEP_RESULT_ERROR) {
            printe("Error encountered");
            //lulu_destroy(&pcol);
            return 1;
        }

        step_nr++;
    }

    //lulu_destroy(&pcol);

    return 0;
}
