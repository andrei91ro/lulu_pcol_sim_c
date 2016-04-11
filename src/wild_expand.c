#include "wild_expand.h"

//bitmasks that show where is the searched object (isObjectInProgram()) located within the program
const uint8_t OBJECT_IS_PROGRAM_LHS     = 1;    //Left Hand Side object is the searched object
const uint8_t OBJECT_IS_PROGRAM_RHS     = 1<<1; //Right Hand Side object is the searched object
const uint8_t OBJECT_IS_PROGRAM_ALT_LHS = 1<<2; //Alternative Left Hand Side is the searched object
const uint8_t OBJECT_IS_PROGRAM_ALT_RHS = 1<<3; //Alternative Right Hand Side is the searched object

bool replaceObjInProgram(Program_t *program, uint8_t initial_obj, uint8_t final_obj) {
    bool initialObjectFound = FALSE;

    for (uint8_t rule_nr = 0; rule_nr < program->nr_rules; rule_nr++) {
        if (program->rules[rule_nr].lhs == initial_obj) {
            initialObjectFound = TRUE;
            program->rules[rule_nr].lhs = final_obj;
        }
        if (program->rules[rule_nr].rhs == initial_obj) {
            initialObjectFound = TRUE;
            program->rules[rule_nr].rhs = final_obj;
        }
        if (program->rules[rule_nr].alt_lhs == initial_obj) {
            initialObjectFound = TRUE;
            program->rules[rule_nr].alt_lhs = final_obj;
        }
        if (program->rules[rule_nr].alt_rhs == initial_obj) {
            initialObjectFound = TRUE;
            program->rules[rule_nr].alt_rhs = final_obj;
        }
    }

    return initialObjectFound;
}

bool isWildcardAnyInProgram(Program_t *program, uint8_t obj_any[], uint8_t obj_any_size) {
    for (uint8_t i = 0; i < obj_any_size; i++)
        if (isObjectInProgram(program, obj_any[i]))
            return TRUE;

    return FALSE;
}

bool isObjectInProgram(Program_t *program, uint8_t obj) {
    for (uint8_t rule_nr = 0; rule_nr < program->nr_rules; rule_nr++)
        if (isObjectInRule(&program->rules[rule_nr], obj))
            return TRUE;
    return FALSE;
}

uint8_t isObjectInRule(Rule_t *rule, uint8_t obj) {
    uint8_t response = 0;

    if (rule->lhs == obj)
        response |= OBJECT_IS_PROGRAM_LHS;
    else if (rule->rhs == obj)
        response |= OBJECT_IS_PROGRAM_RHS;
    else if (rule->alt_lhs == obj)
        response |= OBJECT_IS_PROGRAM_ALT_LHS;
    else if (rule->alt_rhs == obj)
        response |= OBJECT_IS_PROGRAM_ALT_RHS;

    return response;
}

void replacePcolonyWildID(Pcolony_t *pcol, uint8_t obj_with_id[], uint8_t obj_with_id_size, uint8_t my_symbolic_id) {
    Agent_t *agent;

    for (uint8_t i = 0; i < obj_with_id_size; i++) {
        //OBJECT_ID_B_W_ID is followed by OBJECT_ID_B_0
        replaceObjInMultisetEnv(&pcol->env, obj_with_id[i], obj_with_id[i] + 1 + my_symbolic_id);
        replaceObjInMultisetEnv(&pcol->pswarm.global_env, obj_with_id[i], obj_with_id[i] + 1 + my_symbolic_id);

        for (uint8_t agent_nr = 0; agent_nr < pcol->nr_agents; agent_nr++) {
            agent = &pcol->agents[agent_nr];
            //replace W_ID in each agent's obj
            //OBJECT_ID_B_W_ID is followed by OBJECT_ID_B_0
            replaceObjInMultisetObj(&agent->obj, obj_with_id[i], obj_with_id[i] + 1 + my_symbolic_id);

            for (uint8_t program_nr = 0; program_nr < agent->nr_programs; program_nr++)
                replaceObjInProgram(&agent->programs[program_nr], obj_with_id[i], obj_with_id[i] + 1 + my_symbolic_id);
        }
    }
}

void expandPcolonyWildAny(Pcolony_t *pcol, uint8_t obj_with_any[], uint8_t is_obj_with_any_followed_by_id[], uint8_t obj_with_any_size, uint8_t my_symbolic_id, uint8_t nr_swarm_robots) {
    for (uint8_t any_id = 0; any_id < obj_with_any_size; any_id++) {
        //if for e.g B_W_ALL exists in the environment, then replace it with the expansion
        if (areObjectsInMultisetEnv(&pcol->env, obj_with_any[any_id], NO_OBJECT)) {
            for (uint8_t robot_id = 0; robot_id < nr_swarm_robots; robot_id++)
                if (robot_id != my_symbolic_id)
                    setObjectCountFromMultisetEnv(&pcol->env,
                            obj_with_any[any_id] + is_obj_with_any_followed_by_id[any_id] + 1 + robot_id,
                            COUNT_INCREMENT);

            //now that we replaced this wildcarded object with it's expansions, we can remove it from this multiset
            setObjectCountFromMultisetEnv(&pcol->env, obj_with_any[any_id], 0);
        }

        //if for e.g B_W_ALL exists in the global swarm environment, then replace it with the expansion
        if (areObjectsInMultisetEnv(&pcol->pswarm.global_env, obj_with_any[any_id], NO_OBJECT)) {
            for (uint8_t robot_id = 0; robot_id < nr_swarm_robots; robot_id++)
                if (robot_id != my_symbolic_id)
                    setObjectCountFromMultisetEnv(&pcol->pswarm.global_env,
                            obj_with_any[any_id] + is_obj_with_any_followed_by_id[any_id] + 1 + robot_id,
                            COUNT_INCREMENT);

            //now that we replaced this wildcarded object with it's expansions, we can remove it from this multiset
            setObjectCountFromMultisetEnv(&pcol->pswarm.global_env, obj_with_any[any_id], 0);
        }

        for (uint8_t agent_nr = 0; agent_nr < pcol->nr_agents; agent_nr++) {
            //if for e.g B_W_ALL exists in this agent's obj, then replace it with the expansion
            if (areObjectsInMultisetObj(&pcol->agents[agent_nr].obj, obj_with_any[any_id], NO_OBJECT)) {
                //we remove this wildcarded object from the multiset to free up space for the generated objects (as multisetObj is of limited capacity)
                setObjectCountFromMultisetObj(&pcol->agents[agent_nr].obj, obj_with_any[any_id], 0);
                for (uint8_t robot_id = 0; robot_id < nr_swarm_robots; robot_id++)
                    setObjectCountFromMultisetObj(&pcol->agents[agent_nr].obj,
                            obj_with_any[any_id] + is_obj_with_any_followed_by_id[any_id] + 1 + robot_id,
                            COUNT_INCREMENT);

            }
        }
    }

    // begin program expansion
    for (uint8_t agent_nr = 0; agent_nr < pcol->nr_agents; agent_nr++) {
        //if all programs have been initialised (init_program_nr == nr_programs), then there is no need for wildcard any W_ALL expansion
        if (pcol->agents[agent_nr].init_program_nr != pcol->agents[agent_nr].nr_programs) {
            uint8_t nr_expanded_programs = 0;

            for (uint8_t program_nr = 0; program_nr < pcol->agents[agent_nr].nr_programs; program_nr++) {

                if (isWildcardAnyInProgram(&pcol->agents[agent_nr].programs[program_nr], obj_with_any, obj_with_any_size)) {
                    for (uint8_t robot_id = 0; robot_id < nr_swarm_robots; robot_id++)
                        if (robot_id != my_symbolic_id) {
                            //create a copy of the current program and place it at the end of the initialized program list for this agent
                            copyProgram(&pcol->agents[agent_nr].programs[pcol->agents[agent_nr].init_program_nr],
                                    &pcol->agents[agent_nr].programs[program_nr]);

                            for (uint8_t any_id = 0; any_id < obj_with_any_size; any_id++)
                                //if wild_position > 0 then this wildcard object exists within the rules of the program
                                if (isObjectInProgram(&pcol->agents[agent_nr].programs[program_nr], obj_with_any[any_id]))
                                    //replace any occurence of the W_ALL wildcard in any rule of the program with the current robot_id value, for e.g. B_0, B_1, ...
                                    replaceObjInProgram(&pcol->agents[agent_nr].programs[pcol->agents[agent_nr].init_program_nr],
                                            obj_with_any[any_id],
                                            obj_with_any[any_id] + is_obj_with_any_followed_by_id[any_id] + 1 + robot_id);

                            //another program has been initialized
                            pcol->agents[agent_nr].init_program_nr++;
                        }
                    //keep track of the number of expanded programs so far
                    nr_expanded_programs++;

                    //we finished expanding this wildcarded program, so remove this program
                    destroyProgram(&pcol->agents[agent_nr].programs[program_nr]);
                }
            }
            if (nr_expanded_programs > 0) {
                //construct a new program list for this agent that does not contain empty (no rules) programs - programs that were originally
                //wildcard-any programs and were expanded and afterwards deleted
                Program_t *new_programs = (Program_t *) malloc(sizeof(Program_t) * (pcol->agents[agent_nr].nr_programs - nr_expanded_programs));
                uint8_t current_pos = 0;
                for (uint8_t i = 0; i < pcol->agents[agent_nr].nr_programs; i++)
                    if (pcol->agents[agent_nr].programs[i].nr_rules > 0) {
                        copyProgram(&new_programs[current_pos++], &pcol->agents[agent_nr].programs[i]);
                        destroyProgram(&pcol->agents[agent_nr].programs[i]);
                    }
                //remove the old list
                free(pcol->agents[agent_nr].programs);
                //assign the new list (will be cleared by destroyAgent() just as the original list)
                pcol->agents[agent_nr].programs = new_programs;
                //we removed the wildcard any programs
                pcol->agents[agent_nr].nr_programs -= nr_expanded_programs;
            }
        }
    }
}
