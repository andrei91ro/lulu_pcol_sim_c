#include "lulu_instance_template.h"

#ifdef NEEDING_WILDCARD_EXPANSION
    #include "wild_expand.h"
#endif

#ifdef PCOL_SIM
    char* objectNames[] = {[NO_OBJECT] = "no_object", [OBJECT_ID_L_M] = "l_m", [OBJECT_ID_L_P] = "l_p",[OBJECT_ID_L_Z] = "l_z", [OBJECT_ID_E] = "e", [OBJECT_ID_F] = "f"};
    char* agentNames[] = {[AGENT_MOTION] = "motion"};
#endif

#ifdef NEEDING_WILDCARD_EXPANSION
    #include "wild_expand.h"
#endif

//the smallest kilo_uid from the swarm
const uint16_t smallest_robot_uid = 70;
//the number of robots that make up the swarm
const uint16_t nr_swarm_robots = 3;

void lulu_init(Pcolony_t *pcol) {
    //init Pcolony with alphabet size = 4, nr of agents = 2, capacity = 2
    //initialized alphabet_size includes all of the extended objects, after wildcard expansion
    initPcolony(pcol, 4, 2, 2);
    //Pcolony.alphabet = ['e', 'f', 'l_p']

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

    //init INPUT global pswarm environment
        pcol->pswarm.in_global_env.items[0].id = OBJECT_ID_E;
        pcol->pswarm.in_global_env.items[0].nr = 2;
    //end init global pswarm environment

    //init OUTPUT global pswarm environment
        pcol->pswarm.out_global_env.items[0].id = OBJECT_ID_E;
        pcol->pswarm.out_global_env.items[0].nr = 2;
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
             pcol->agents[AGENT_COMMAND].init_program_nr++;
        //end init programs
    //end agent command
}

void lulu_destroy(Pcolony_t *pcol) {
    //destroys all of the subcomponents
    destroyPcolony(pcol);
}

#ifdef NEEDING_WILDCARD_EXPANSION
    uint16_t expand_pcolony(Pcolony_t *pcol, uint16_t my_id) {
        uint8_t obj_with_id[] = {OBJECT_ID_B_$id, OBJECT_ID_S_$id, OBJECT_ID_ID_$id};
        uint8_t obj_with_id_size = 3;

        uint8_t obj_with_any[] = {OBJECT_ID_B_$};
        //is the $ object followed by a $ID object in the alphabet
        // 0 means that in the alphabet we have B_$, B_0 B_1, B_2, ...
        // 1 means that in the alphabet we have B_$, B_$ID, B_0 B_1, B_2, ...
        uint8_t is_obj_with_any_followed_by_id[] = {0};
        uint8_t obj_with_any_size = 1; //applies to both obj_with_any[] and is_obj_with_any_followed_by_id[]

        uint16_t my_symbolic_id = my_id - smallest_robot_uid;

        //do not expand the P colonies of robots that have a symbolic_id > nr_swarm_robots initially declared
        if (my_symbolic_id >= nr_swarm_robots)
            return my_symbolic_id;

        //replace $ID wildcarded objects with the object corresponding to the symbolic id
        //  e.g.: B_$ID -> B_0 for my_symbolic_id = 0
        replacePcolonyWildID(pcol, obj_with_id, obj_with_id_size, my_symbolic_id);

        //expand each obj_with_any[] element into nr_swarm_robots objects except my_symbolic id.
        //  e.g.: B_$ -> B_0, B_2 for nr_swarm_robots = 3 and my_symbolic_id = 1
        expandPcolonyWildAny(pcol, obj_with_any, is_obj_with_any_followed_by_id, obj_with_any_size, my_symbolic_id, nr_swarm_robots);

        return my_symbolic_id;
    }
#endif
