#include "lulu_instance_template.h"
#include <malloc.h>

void lulu_init(Pcolony_t *pcol) {
    //init Pcolony with alphabet size = 4, nr of agents = 2, capacity = 2
    initPcolony(pcol, 4, 2, 2);

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
}
