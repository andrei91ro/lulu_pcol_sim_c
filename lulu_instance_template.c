#include "lulu_instance_template.h"
#include <malloc.h>
#ifdef PCOL_SIM
    #include <string.h>
    char **objectNames;
    char **agentNames;
#endif
void lulu_init(Pcolony_t *pcol) {
    //init Pcolony with alphabet size = 4, nr of agents = 2, capacity = 2
    initPcolony(pcol, 4, 2, 2);
    //Pcolony.alphabet = ['e', 'f', 'l_p']

    #ifdef PCOL_SIM
        // objectNames_size = (alphabet_size = 4) + 1
        // because object_id start at 1 not 0
        objectNames = (char **) malloc(sizeof(char *) * 4);
        for (uint8_t i = 0; i < 4; i++)
            //objectNames of 10 chars max
            objectNames[i] = (char *) malloc(sizeof(char) * 10);

        strcpy(objectNames[OBJECT_ID_E], "e");
        strcpy(objectNames[OBJECT_ID_F], "f");

        // agentNames_size = nr_agents = 2
        agentNames = (char **) malloc(sizeof(char *) * 2);
        for (uint8_t i = 0; i < 2; i++)
            //agentNames of 20 chars max
            agentNames[i] = (char *) malloc(sizeof(char) * 20);

        strcpy(agentNames[AGENT_MOTION], "motion");
        strcpy(agentNames[AGENT_COMMAND], "command");
    #endif
    //init environment
        pcol->env.items[0].id = OBJECT_ID_E;
        pcol->env.items[0].nr = 1;

        pcol->env.items[1].id = OBJECT_ID_F;
        pcol->env.items[1].nr = 2;
    //end init environment

    //init global pswarm environment
        pcol->pswarm.global_env.items[0].id = OBJECT_ID_E;
        pcol->pswarm.global_env.items[0].nr = 2;
    //end init global pswarm environment

    //init agent command
    initAgent(&pcol->agents[AGENT_COMMAND], pcol, 2);
        //init obj multiset
        pcol->agents[AGENT_COMMAND].obj.items[0] = OBJECT_ID_E;
        pcol->agents[AGENT_COMMAND].obj.items[1] = OBJECT_ID_E;

        //init programs
        initProgram(&pcol->agents[AGENT_COMMAND].programs[0], pcol->n);
            //init program 0: < e->f, e<->d >
                //init rule 0: e->f
                initRule(&pcol->agents[AGENT_COMMAND].programs[0].rules[0], RULE_TYPE_EVOLUTION, OBJECT_ID_E, OBJECT_ID_F, NO_OBJECT, NO_OBJECT);
                //init rule 1: e<->d
                initRule(&pcol->agents[AGENT_COMMAND].programs[0].rules[0], RULE_TYPE_COMMUNICATION, OBJECT_ID_E, OBJECT_ID_D, NO_OBJECT, NO_OBJECT);
            //end program 0
        //end init programs
    //end agent command
}

void lulu_destroy(Pcolony_t *pcol) {
    //destroys all of the subcomponents
    destroyPcolony(pcol);
    #ifdef PCOL_SIM
        // objectNames_size = alphabet_size = 4
        for (uint8_t i = 0; i < 4; i++)
            free(objectNames[i]);
        free(objectNames);
        //agentNames_size == nr of agents = 2
        for (uint8_t i = 0; i < 2; i++)
            free(agentNames[i]);
        free(agentNames);
    #endif
}
