/**
 * @file lulu.c
 * @brief Lulu P colony simulator internal structure.
 * In this file we implement all of the structures that are used to store a parsed P colony / P swarm for use in simulation
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-27
 */
#include "lulu.h"
#include "debug_print.h"
#include <stdlib.h> //for rand() on PC and malloc on PC and AVR

//if building Pcolony simulator for AVR (Kilobot)
#ifdef KILOBOT
    #include "kilolib.h" //for rand_seed, rand_soft
#else
    #include <time.h> //for time(0) used as seed in initPcolony
#endif

#ifdef DEBUG_PRINT
    //error messages that can be printed by agent_executeProgram()
    const char* execErrMsgs[] = {"Obj %d req in AG rule %d NOT found",
        "Obj %d req in ENV rule %d NOT found",
        "Obj %d req in GLOBAL_ENV rule %d NOT found",
        "Obj %d req in IN_GLOBAL_ENV rule %d NOT found",
        "Obj %d req in OUT_GLOBAL_ENV rule %d NOT found"};
#endif

void initMultisetEnv(multiset_env_t *multiset, uint8_t size) {
    multiset->items = (multiset_env_item_t *)malloc(sizeof(multiset_env_item_t) * size);
    for (uint8_t i = 0; i < size; i++) {
        multiset->items[i].id = NO_OBJECT;
        multiset->items[i].nr = 0;
    }
    multiset->size = size;
}

void initMultisetObj(multiset_obj_t *multiset, uint8_t size) {
    multiset->items = (uint8_t *)malloc(sizeof(uint8_t) * size);
    for (uint8_t i = 0; i < size; i++)
        multiset->items[i] = NO_OBJECT;
    multiset->size = size;
}

void clearMultisetEnv(multiset_env_t *multiset) {
    for (uint8_t i = 0; i < multiset->size; i++) {
        multiset->items[i].id = NO_OBJECT;
        multiset->items[i].nr = 0;
    }
}
void clearMultisetObj(multiset_obj_t *multiset) {
    for (uint8_t i = 0; i < multiset->size; i++)
        multiset->items[i] = NO_OBJECT;
}

void destroyMultisetEnv(multiset_env_t *multiset) {
    if (multiset->size <= 0)
        return; //the multiset may have already been cleaned so there is nothing to do

    free(multiset->items);
    multiset->size = 0;
}

void destroyMultisetObj(multiset_obj_t *multiset) {
    if (multiset->size <= 0)
        return; //the multiset may have already been cleaned so there is nothing to do

    free(multiset->items);
    multiset->size = 0;
}

bool areObjectsInMultisetEnv(multiset_env_t *multiset, uint8_t obj1, uint8_t obj2) {
    for (uint8_t i = 0; i < multiset->size; i++)
        // there is no point in checking for NO_OBJECT as this is the initial value
        if ((multiset->items[i].id == obj1 && obj1 != NO_OBJECT) ||
                (multiset->items[i].id == obj2 && obj2 != NO_OBJECT))
            return TRUE;

    return FALSE;
}

bool areObjectsInMultisetObj(multiset_obj_t *multiset, uint8_t obj1, uint8_t obj2) {
    for (uint8_t i = 0; i < multiset->size; i++)
        if ((multiset->items[i] == obj1 && obj1 != NO_OBJECT) ||
                (multiset->items[i] == obj2 && obj2 != NO_OBJECT))
            return TRUE;

    return FALSE;
}

uint8_t getObjectCountFromMultisetEnv(multiset_env_t *multiset, uint8_t obj) {
    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i].id == obj)
            return multiset->items[i].nr;

    return 0;
}

uint8_t getObjectCountFromMultisetObj(multiset_obj_t *multiset, uint8_t obj) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i] == obj)
            count++;

    return count;
}

bool setObjectCountFromMultisetEnv(multiset_env_t *multiset, uint8_t obj, uint8_t newCount) {
    //get current count of obj.
    uint8_t count = getObjectCountFromMultisetEnv(multiset, obj);
    if (newCount == COUNT_INCREMENT)
        newCount = count + 1;
    else if (newCount == COUNT_DECREMENT)
        newCount = (count > 0)? count - 1: 0;

    if (newCount == count)
        return FALSE; // There is no change in count so skip the operation

    for (uint8_t i = 0; i < multiset->size; i++)
        // if we are requested to delete an object from the multiset
        if (newCount <= 0) {
            // if i is the target object
            if (multiset->items[i].id == obj) {
                //mark this position as empty from now on
                multiset->items[i].id = NO_OBJECT;
                multiset->items[i].nr = 0;
            }
        }
        // we just need to modify the count of an object from the multiset
        else {
            //if the object was not present in the multiset and we find an empty slot
            if (count == 0 && multiset->items[i].nr == 0) {
                multiset->items[i].id = obj;
                multiset->items[i].nr = newCount;
                return TRUE;
            }
            // if the object was in the multiset and we find it
            else if (count > 0 && multiset->items[i].id == obj) {
                multiset->items[i].nr = newCount;
                return TRUE;
            }
        }

    return FALSE; //the multiset is full (no more empty slots available)
}

bool setObjectCountFromMultisetObj(multiset_obj_t *multiset, uint8_t obj, uint8_t newCount) {
    //get current count of obj.
    uint8_t count_abs_diff = 0;
    uint8_t count = getObjectCountFromMultisetObj(multiset, obj);
    if (newCount == COUNT_INCREMENT)
        newCount = count + 1;
    else if (newCount == COUNT_DECREMENT)
        newCount = (count > 0)? count - 1: 0;

    if (newCount == count)
        return FALSE; // There is no change in count so skip the operation

    count_abs_diff = (newCount > count)? newCount - count: count - newCount;

    //repeat the add / del operation as many times as needed to make count == newCount
    for (uint8_t i = 0; i < count_abs_diff; i++)
        // if we are requested to delete an object from the multiset
        if (newCount < count) {
            if (!delObjectFromMultisetObj(multiset, obj))
                return FALSE; //failed to delele one object from multiset
        }
        // we are requested to add an object to the multiset
        else {
            if (!addObjectToMultisetObj(multiset, obj))
                return FALSE; //failed to add one object to the multiset
        }

    //the operation finished succesfully
    return TRUE;
}

bool addObjectToMultisetObj(multiset_obj_t *multiset, uint8_t obj) {
    for (uint8_t i = 0; i < multiset->size; i++)
        //if we find an empty slot
        if (multiset->items[i] == NO_OBJECT) {
            multiset->items[i] = obj;
            return TRUE;
        }

    //the target obj was not found (or no empty slot)
    return FALSE;
}

bool delObjectFromMultisetObj(multiset_obj_t *multiset, uint8_t obj) {
    for (uint8_t i = 0; i < multiset->size; i++)
        // if i is the target object
        if (multiset->items[i] == obj) {
            //mark this position as empty from now on
            multiset->items[i] = NO_OBJECT;
            return TRUE;
        }

    //the target obj was not found
    return FALSE;
}

bool isMultisetEnvIncluded(multiset_env_t *parent, multiset_env_t *child) {
    for (uint8_t i = 0; i < child->size; i++) {
        // if child[i] object does not appear at least as many times in parent multiset as it does in the child
        if (getObjectCountFromMultisetEnv(parent, child->items[i].id) < child->items[i].nr)
            return FALSE;
    }
    return TRUE;
}

bool isMultisetObjIncluded(multiset_obj_t *parent, multiset_obj_t *child) {
    for (uint8_t i = 0; i < child->size; i++)
        // if child[i] is not NO_OBJECT and
        //  child[i] object does not appear in parent multiset
        if (child->items[i] != NO_OBJECT &&
                getObjectCountFromMultisetObj(parent, child->items[i]) < getObjectCountFromMultisetObj(child, child->items[i]))
            return FALSE;

    return TRUE;
}

bool replaceObjInMultisetEnv(multiset_env_t *multiset, uint8_t initial_obj, uint8_t final_obj) {
    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i].id == initial_obj) {
            multiset->items[i].id = final_obj;
            // we replaced the initial_obj and there should be no other entry in the multiset with
            // this id, so we return
            return TRUE;
        }

    //the initial object was not found
    return FALSE;
}

bool replaceObjInMultisetObj(multiset_obj_t *multiset, uint8_t initial_obj, uint8_t final_obj) {
    bool initialObjectFound = FALSE;

    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i] == initial_obj) {
            multiset->items[i] = final_obj;
            initialObjectFound = TRUE;
            //replace all instaces of the inital_obj
        }

    //the initial object was not found
    return initialObjectFound;
}

bool replaceOneObjInMultisetObj(multiset_obj_t *multiset, uint8_t initial_obj, uint8_t final_obj) {

    for (uint8_t i = 0; i < multiset->size; i++)
        if (multiset->items[i] == initial_obj) {
            multiset->items[i] = final_obj;
            return TRUE;
        }

    //the initial object was not found
    return FALSE;
}

bool agent_choseProgram(Agent_t *agent) {
    Rule_t *rule;
    multiset_env_t required_env, required_global_env, required_in_global_env, required_out_global_env;
    multiset_obj_t required_obj;
    uint8_t chosen_prg_count = 0, last_chosen_prg_nr = 0;
    //possiblePrograms[2] = True -> program[2] is executable
    bool possiblePrograms[agent->nr_programs];

    //init the entire array to FALSE
    initArray(possiblePrograms, agent->nr_programs, FALSE);

    initMultisetObj(&required_obj, agent->pcolony->n);
    initMultisetEnv(&required_env, agent->pcolony->nr_A);
    initMultisetEnv(&required_global_env, agent->pcolony->nr_A);
    initMultisetEnv(&required_in_global_env, agent->pcolony->nr_A);
    initMultisetEnv(&required_out_global_env, agent->pcolony->nr_A);

    for (uint8_t prg_nr = 0; prg_nr < agent->nr_programs; prg_nr++) {
        bool executable = TRUE;
        //by clearing the multisets before checking each program, we fix the bug related to required_env failed for more than one program
        clearMultisetObj(&required_obj);
        clearMultisetEnv(&required_env);
        clearMultisetEnv(&required_global_env);
        clearMultisetEnv(&required_in_global_env);
        clearMultisetEnv(&required_out_global_env);

        //if this program contains less rules than the P colony capacity, then the missing rules were e->e
        //so it is safe to assume that we need one e object in required_obj for each missing rule
        for (uint8_t i = 0; i < (agent->pcolony->n - agent->programs[prg_nr].nr_rules); i++)
            setObjectCountFromMultisetObj(&required_obj, OBJECT_ID_E, COUNT_INCREMENT);

        for (uint8_t rule_nr = 0; rule_nr < agent->programs[prg_nr].nr_rules; rule_nr++) {
            rule = &agent->programs[prg_nr].rules[rule_nr];

            //if rule is a simple, non-conditional rule
            if (rule->type < RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION) {

                //all types of rules require the left hand side obj to be available in the agent
                //if (rule.lhs not in self.obj):
                if (!areObjectsInMultisetObj(&agent->obj, rule->lhs, NO_OBJECT)) {
                    executable = FALSE;
                    break; //stop checking
                }

                //communication rules require the right hand side obj to be available in the environement
                //if (rule.main_type == RuleType.communication and rule.rhs not in self.colony.env):
                if (rule->type == RULE_TYPE_COMMUNICATION &&
                        !areObjectsInMultisetEnv(&agent->pcolony->env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                    break; //stop checking
                }

                //exteroceptive rules require the right hand side obj to be available in the global Pswarm environment
                //if (rule.main_type == RuleType.exteroceptive and rule.rhs not in self.colony.parentSwarm.global_env):
                if (rule->type == RULE_TYPE_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                    break; //stop checking
                }

                //in_exteroceptive rules require the right hand side obj to be available in the INPUT global Pswarm environment
                //if (rule.main_type == RuleType.in_exteroceptive and rule.rhs not in self.colony.parentSwarm.in_global_env):
                if (rule->type == RULE_TYPE_IN_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                    break; //stop checking
                }

                //out_exteroceptive rules require the right hand side obj to be available in the OUTPUT global Pswarm environment
                //if (rule.main_type == RuleType.out_exteroceptive and rule.rhs not in self.colony.parentSwarm.out_global_env):
                if (rule->type == RULE_TYPE_OUT_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                    break; //stop checking
                }

                rule->exec_rule_nr = RULE_EXEC_OPTION_FIRST; //the only option available

                //if we reach this step, then the rule is executable
                //required_obj[rule.lhs] += 1 //all rules need the lhs to be in obj
                setObjectCountFromMultisetObj(&required_obj, rule->lhs, COUNT_INCREMENT); //all rules need the lhs to be in obj

                if (rule->type == RULE_TYPE_COMMUNICATION)
                    //required_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pcolony environment
                    setObjectCountFromMultisetEnv(&required_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pcolony environment

                if (rule->type == RULE_TYPE_EXTEROCEPTIVE)
                    //required_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pswarm global environment
                    setObjectCountFromMultisetEnv(&required_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pswarm global environment

                if (rule->type == RULE_TYPE_IN_EXTEROCEPTIVE)
                    //required_in_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the INPUT Pswarm global environment
                    setObjectCountFromMultisetEnv(&required_in_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the INPUT Pswarm global environment

                if (rule->type == RULE_TYPE_OUT_EXTEROCEPTIVE)
                    //required_out_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the OUTPUT Pswarm global environment
                    setObjectCountFromMultisetEnv(&required_out_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the OUTPUT Pswarm global environment
            }

            // if this is a conditional rule
            else {

                //we first check the first part of the conditional rule (as a normal rule)

                //all types of rules require the left hand side obj to be available in the agent
                //if (rule.lhs not in self.obj):
                if (!areObjectsInMultisetObj(&agent->obj, rule->lhs, NO_OBJECT)) {
                    executable = FALSE;
                }

                //communication rules require the right hand side obj to be available in the environement
                //if (rule.main_type == RuleType.communication and rule.rhs not in self.colony.env):
                if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION &&
                        !areObjectsInMultisetEnv(&agent->pcolony->env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                }

                //exteroceptive rules require the right hand side obj to be available in the global Pswarm environment
                //if (rule.main_type == RuleType.exteroceptive and rule.rhs not in self.colony.parentSwarm.global_env):
                if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                }

                //in exteroceptive rules require the right hand side obj to be available in the global Pswarm environment
                //if (rule.main_type == RuleType.exteroceptive and rule.rhs not in self.colony.parentSwarm.global_env):
                if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                }

                //out exteroceptive rules require the right hand side obj to be available in the global Pswarm environment
                //if (rule.main_type == RuleType.exteroceptive and rule.rhs not in self.colony.parentSwarm.global_env):
                if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE &&
                        !areObjectsInMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->rhs, NO_OBJECT)) {
                    executable = FALSE;
                }

                //if the first part of the conditional rule was executable
                if (executable) {
                    rule->exec_rule_nr = RULE_EXEC_OPTION_FIRST; //the only option available

                    //if we reach this step, then the rule is executable
                    //required_obj[rule.lhs] += 1 //all rules need the lhs to be in obj
                    setObjectCountFromMultisetObj(&required_obj, rule->lhs, COUNT_INCREMENT); //all rules need the lhs to be in obj

                    if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION)
                        //required_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pcolony environment
                        setObjectCountFromMultisetEnv(&required_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pcolony environment

                    if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE)
                        //required_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pswarm global environment

                    if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE)
                        //required_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_in_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pswarm IN global environment

                    if (getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE)
                        //required_global_env[rule.rhs] += 1 //rhs part of the rule has to be in the Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_out_global_env, rule->rhs, COUNT_INCREMENT); //rhs part of the rule has to be in the Pswarm OUT global environment
                }
                // if not then check the alternative part of the conditional rule
                else {
                    //by clearing the multisets before checking each program, we fix potential bugs related to previously required objects
                    clearMultisetObj(&required_obj);
                    clearMultisetEnv(&required_env);
                    clearMultisetEnv(&required_global_env);
                    clearMultisetEnv(&required_in_global_env);
                    clearMultisetEnv(&required_out_global_env);

                    executable=TRUE;
                    printd("Checking alternative of conditional for P%d", prg_nr);

                    //all types of rules require the left hand side obj to be available in the agent
                    //if (rule.alt_lhs not in self.obj):
                    if (!areObjectsInMultisetObj(&agent->obj, rule->alt_lhs, NO_OBJECT)) {
                        executable = FALSE;
                        break; //stop checking
                    }

                    //communication rules require the right hand side obj to be available in the environement
                    //if (rule.main_type == RuleType.communication and rule.alt_rhs not in self.colony.env):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION &&
                            !areObjectsInMultisetEnv(&agent->pcolony->env, rule->alt_rhs, NO_OBJECT)) {
                        executable = FALSE;
                        break; //stop checking
                    }

                    //exteroceptive rules require the right hand side obj to be available in the global Pswarm environment
                    //if (rule.main_type == RuleType.exteroceptive and rule.alt_rhs not in self.colony.parentSwarm.global_env):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE &&
                            !areObjectsInMultisetEnv(&agent->pcolony->pswarm.global_env, rule->alt_rhs, NO_OBJECT)) {
                        executable = FALSE;
                        break; //stop checking
                    }

                    //IN exteroceptive rules require the right hand side obj to be available in the global IN Pswarm environment
                    //if (rule.main_type == RuleType.in_exteroceptive and rule.alt_rhs not in self.colony.parentSwarm.global_env):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE &&
                            !areObjectsInMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->alt_rhs, NO_OBJECT)) {
                        executable = FALSE;
                        break; //stop checking
                    }

                    //OUT exteroceptive rules require the right hand side obj to be available in the global OUT Pswarm environment
                    //if (rule.main_type == RuleType.out_exteroceptive and rule.alt_rhs not in self.colony.parentSwarm.global_env):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE &&
                            !areObjectsInMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->alt_rhs, NO_OBJECT)) {
                        executable = FALSE;
                        break; //stop checking
                    }

                    rule->exec_rule_nr = RULE_EXEC_OPTION_SECOND; //the only option available

                    //if we reach this step, then the rule is executable
                    //required_obj[rule.alt_lhs] += 1 //all rules need the alt_lhs to be in obj
                    setObjectCountFromMultisetObj(&required_obj, rule->alt_lhs, COUNT_INCREMENT); //all rules need the alt_lhs to be in obj

                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION)
                        //required_env[rule.alt_rhs] += 1 //alt_rhs part of the rule has to be in the Pcolony environment
                        setObjectCountFromMultisetEnv(&required_env, rule->alt_rhs, COUNT_INCREMENT); //alt_rhs part of the rule has to be in the Pcolony environment

                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE)
                        //required_global_env[rule.alt_rhs] += 1 //alt_rhs part of the rule has to be in the Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_global_env, rule->alt_rhs, COUNT_INCREMENT); //alt_rhs part of the rule has to be in the Pswarm global environment

                    //if (rule.alt_type == RuleType.in_exteroceptive):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE)
                        //required_in_global_env[rule.alt_rhs] += 1 // alt_rhs part of the rule has to be in the INPUT Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_in_global_env, rule->alt_rhs, COUNT_INCREMENT); //alt_rhs part of the rule has to be in the INPUT Pswarm global environment

                    //if (rule.alt_type == RuleType.out_exteroceptive):
                    if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE)
                        //required_out_global_env[rule.alt_rhs] += 1 // alt_rhs part of the rule has to be in the OUTPUT Pswarm global environment
                        setObjectCountFromMultisetEnv(&required_out_global_env, rule->alt_rhs, COUNT_INCREMENT); //alt_rhs part of the rule has to be in the OUTPUT Pswarm global environment
                }
            }
        //end for rule
        }

        // if all previous rule tests confirm that this program is executable
        if (executable) {
            // check that the Agent obj requirements of the program are met
            //for k, v in required_obj.items():
                //if (self.obj[k] < v):
            if (!isMultisetObjIncluded(&agent->obj, &required_obj)) {
                    printd("req_obj fail P%d", prg_nr);
                    executable = FALSE; // this program is not executable, check another program
                    continue;
            }

            // if e object is among the required objects in the Pcolony environment
            //if ('e' in required_env):
                // ignore this requirement because in theory, there are always enough e objects in the environment
                //del required_env['e']
            setObjectCountFromMultisetEnv(&required_env, OBJECT_ID_E, 0);
            // check that the Pcolony env requirements of the program are met
            //for k, v in required_env.items():
                //if (self.colony.env[k] < v):
            if (!isMultisetEnvIncluded(&agent->pcolony->env, &required_env)) {
                    printd("req_env fail P%d", prg_nr);
                    executable = FALSE; // this program is not executable, check another program
                    continue;
            }

            // if e object is among the required objects in the Pswarm global_environment
            //if ('e' in required_global_env):
                // ignore this requirement because in theory, there are always enough e objects in the global_environment
                //del required_global_env['e']
            setObjectCountFromMultisetEnv(&required_global_env, OBJECT_ID_E, 0);
            // check that the Pswarm global_env requirements of the program are met
            //for k, v in required_global_env.items():
                //if (self.colony.parentSwarm.global_env[k] < v):
            if (!isMultisetEnvIncluded(&agent->pcolony->pswarm.global_env, &required_global_env)) {
                    printd("req_global_env fail P%d", prg_nr);
                    executable = FALSE; // this program is not executable, check another program
                    continue;
            }

            // if e object is among the required objects in the INPUT Pswarm global_environment
            //if ('e' in required_in_global_env):
                // ignore this requirement because in theory, there are always enough e objects in the INPUT global_environment
                //del required_in_global_env['e']
            setObjectCountFromMultisetEnv(&required_in_global_env, OBJECT_ID_E, 0);
            // check that the INPUT Pswarm global_env requirements of the program are met
            //for k, v in required_in_global_env.items():
                //if (self.colony.parentSwarm.in_global_env[k] < v):
            if (!isMultisetEnvIncluded(&agent->pcolony->pswarm.in_global_env, &required_in_global_env)) {
                    printd("req_in_global_env fail P%d", prg_nr);
                    executable = FALSE; // this program is not executable, check another program
                    continue;
            }

            // if e object is among the required objects in the OUTPUT Pswarm global_environment
            //if ('e' in required_out_global_env):
                // ignore this requirement because in theory, there are always enough e objects in the out_global_environment
                //del required_out_global_env['e']
            setObjectCountFromMultisetEnv(&required_out_global_env, OBJECT_ID_E, 0);
            // check that the Pswarm out_global_env requirements of the program are met
            //for k, v in required_out_global_env.items():
                //if (self.colony.parentSwarm.out_global_env[k] < v):
            if (!isMultisetEnvIncluded(&agent->pcolony->pswarm.out_global_env, &required_out_global_env)) {
                    printd("req_out_global_env fail P%d", prg_nr);
                    executable = FALSE; // this program is not executable, check another program
                    continue;
            }
        }

        if (executable) {
            // if we reach this step then this program is executable
            //possiblePrograms.append(nr)
            possiblePrograms[prg_nr] = TRUE;
            last_chosen_prg_nr = prg_nr;
            chosen_prg_count++;
        }
    }//end for program

    //printd("possiblePrograms = %s" % possiblePrograms);
    // if there is only 1 executable program
    //if (len(possiblePrograms) == 1):
    if (chosen_prg_count == 1) {
        //self.chosenProgramNr = possiblePrograms[0];
        agent->chosenProgramNr = last_chosen_prg_nr;
        printd("chosen_prg=%d", agent->chosenProgramNr);

        //release dinamically allocated variables
        destroyMultisetObj(&required_obj);
        destroyMultisetEnv(&required_env);
        destroyMultisetEnv(&required_global_env);
        destroyMultisetEnv(&required_in_global_env);
        destroyMultisetEnv(&required_out_global_env);

        return TRUE; // this agent has an executable program
    }
    // there is more than 1 executable program
    //elif (len(possiblePrograms) > 1)
    else if (chosen_prg_count > 1) {
        //define and construct a reduced array that contains the numbers of the executable programs
        uint8_t aux[chosen_prg_count];
        uint8_t c = 0, rand_value = 0;

        printd("possiblePrgs(nr=%d)=", chosen_prg_count);
        for (int p = 0; p < agent->nr_programs; p++)
            // if program p was previously marked as executable
            if (possiblePrograms[p]) {
                printd("  %d", p);
                aux[c++] = p; //store the program number in the aux array
            }

        //rand_value = random.randint(0, len(possiblePrograms) - 1)
        #ifndef KILOBOT
            //use rand() from stdlib.h
            //rand_value in [0; chosen_prg_count-1] interval
            rand_value = rand() % chosen_prg_count;
        #else
            //use rand_soft from kilolib.h
            //rand_value in [0; chosen_prg_count-1] interval
            rand_value = rand_soft() % chosen_prg_count;
        #endif
        //self.chosenProgramNr = possiblePrograms[rand_value];
        agent->chosenProgramNr = aux[rand_value];
        printd("rand_chosen_prg=%d", agent->chosenProgramNr);

        //release dinamically allocated variables
        destroyMultisetObj(&required_obj);
        destroyMultisetEnv(&required_env);
        destroyMultisetEnv(&required_global_env);
        destroyMultisetEnv(&required_in_global_env);
        destroyMultisetEnv(&required_out_global_env);

        return TRUE; // this agent has an executable program
    }

    agent->chosenProgramNr = -1; // no program can be executed
    printd("no exec prg");

    //release dinamically allocated variables
    destroyMultisetObj(&required_obj);
    destroyMultisetEnv(&required_env);
    destroyMultisetEnv(&required_global_env);
    destroyMultisetEnv(&required_in_global_env);
    destroyMultisetEnv(&required_out_global_env);

    return FALSE;
}

bool agent_executeProgram(Agent_t *agent) {
    Program_t *program;
    Rule_t *rule;

    if (agent->chosenProgramNr < 0)
        return FALSE;

    program = &agent->programs[agent->chosenProgramNr];
    for (uint8_t rule_nr = 0; rule_nr < program->nr_rules; rule_nr++) {
        rule = &program->rules[rule_nr];
        // if this is a non-conditional or the first rule of a conditional rule was chosen
        //if (rule.exec_rule_nr == RuleExecOption.first):
        if (rule->exec_rule_nr == RULE_EXEC_OPTION_FIRST) {

            // if the rule.lhs object is not in obj any more
            //if (self.obj[rule.lhs] <= 0):
            //if (getObjectCountFromMultiset(&agent->obj, rule->lhs))
            if (!areObjectsInMultisetObj(&agent->obj, rule->lhs, NO_OBJECT)) {
                // this is an error, there was a bug in choseProgram() that shouldn't have chosen this program
                printe(execErrMsgs[0], rule->lhs, rule_nr);
                return FALSE;
            }
            // remove one instance of rule.lhs from obj
            // evolution and communication and {,in_,out_}exteroceptive need this part
            //self.obj[rule.lhs] -= 1;
            setObjectCountFromMultisetObj(&agent->obj, rule->lhs, COUNT_DECREMENT);

            //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

            // 0 counts are allowed so if this is the case
            //if (self.obj[rule.lhs] == 0):
                // remove the entry from the obj counter
                //del self.obj[rule.lhs]

            //if (rule.type == RuleType.evolution):
            if (rule->type == RULE_TYPE_EVOLUTION || getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_EVOLUTION)
                // add the rule.rhs object to obj
                //self.obj[rule.rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->rhs, COUNT_INCREMENT);

            //elif (rule.type == RuleType.communication):
            else if (rule->type == RULE_TYPE_COMMUNICATION || getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION) {
                // if the rule.rhs object is not in the environement any more
                //if (self.colony.env[rule.rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->env, rule->rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[1], rule->rhs, rule_nr);
                    return FALSE;
                }
                // remove one instance of rule.rhs from env only if it is not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.rhs != self.colony.e):
                if (rule->rhs != OBJECT_ID_E) {
                    // remove one instance of rule.rhs from env
                    //self.colony.env[rule.rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->env, rule->rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.env[rule.rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.env[rule.rhs]
                }
                // only modify the environment if the lhs object is not e
                //if (rule.lhs != self.colony.e):
                if (rule->lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to environment
                    //self.colony.env[rule.lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->env, rule->lhs, COUNT_INCREMENT);

                // transfer object from environment to agent.obj
                //self.obj[rule.rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->rhs, COUNT_INCREMENT);
            }

            //elif (rule.type == RuleType.exteroceptive):
            else if (rule->type == RULE_TYPE_EXTEROCEPTIVE || getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE) {
                // if the rule.rhs object is not in the global swarm environement any more
                //if (self.colony.parentSwarm.global_env[rule.rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.global_env, rule->rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[2], rule->rhs, rule_nr);
                    return FALSE;
                }
                // remove one instance of rule.rhs from global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.rhs != self.colony.e):
                if (rule->rhs != OBJECT_ID_E) {
                    // remove one instance of rule.rhs from global swarm env
                    //self.colony.parentSwarm.global_env[rule.rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.global_env, rule->rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.global_env[rule.rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.global_env[rule.rhs]
                }
                // only modify the global swarm environment if the lhs object is not e
                //if (rule.lhs != self.colony.e):
                if (rule->lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to global swarm environment
                    //self.colony.parentSwarm.global_env[rule.lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.global_env, rule->lhs, COUNT_INCREMENT);

                // transfer object from global swarm environment to agent.obj
                //self.obj[rule.rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->rhs, COUNT_INCREMENT);
            }

            //elif (rule.type == RuleType.in_exteroceptive):
            else if (rule->type == RULE_TYPE_IN_EXTEROCEPTIVE || getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE) {
                // if the rule.rhs object is not in the INPUT global swarm environement any more
                //if (self.colony.parentSwarm.in_global_env[rule.rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[3], rule->rhs, rule_nr);
                    return FALSE;
                }
                // remove one instance of rule.rhs from INPUT global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.rhs != self.colony.e):
                if (rule->rhs != OBJECT_ID_E) {
                    // remove one instance of rule.rhs from INPUT global swarm env
                    //self.colony.parentSwarm.in_global_env[rule.rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.in_global_env[rule.rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.in_global_env[rule.rhs]
                }
                // only modify the INPUT global swarm environment if the lhs object is not e
                //if (rule.lhs != self.colony.e):
                if (rule->lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to INPUT global swarm environment
                    //self.colony.parentSwarm.in_global_env[rule.lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->lhs, COUNT_INCREMENT);

                // transfer object from INPUT global swarm environment to agent.obj
                //self.obj[rule.rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->rhs, COUNT_INCREMENT);
            }

            //elif (rule.type == RuleType.out_exteroceptive):
            else if (rule->type == RULE_TYPE_OUT_EXTEROCEPTIVE || getFirstRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE) {
                // if the rule.rhs object is not in the OUTPUT global swarm environement any more
                //if (self.colony.parentSwarm.out_global_env[rule.rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[4], rule->rhs, rule_nr);
                    return FALSE;
                }
                // remove one instance of rule.rhs from OUTPUT global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.rhs != self.colony.e):
                if (rule->rhs != OBJECT_ID_E) {
                    // remove one instance of rule.rhs from OUTPUT global swarm env
                    //self.colony.parentSwarm.out_global_env[rule.rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.out_global_env[rule.rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.out_global_env[rule.rhs]
                }
                // only modify the OUTPUT global swarm environment if the lhs object is not e
                //if (rule.lhs != self.colony.e):
                if (rule->lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to OUTPUT global swarm environment
                    //self.colony.parentSwarm.out_global_env[rule.lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->lhs, COUNT_INCREMENT);

                // transfer object from OUTPUT global swarm environment to agent.obj
                //self.obj[rule.rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->rhs, COUNT_INCREMENT);
            }
        }
        // if this is a conditional rule and the second rule was chosen for execution
        //elif (rule.exec_rule_nr == RuleExecOption.second):
        else if (rule->exec_rule_nr == RULE_EXEC_OPTION_SECOND) {

            // if the rule.alt_lhs object is not in obj any more
            //if (self.obj[rule.alt_lhs] <= 0):
            if (!areObjectsInMultisetObj(&agent->obj, rule->alt_lhs, NO_OBJECT)) {
                // this is an error, there was a bug in choseProgram() that shouldn't have chosen this program
                printe(execErrMsgs[0], rule->alt_lhs, rule_nr);
                return FALSE;
            }
            // remove one instance of rule.alt_lhs from obj
            // evolution and communication and {,in_,out_}exteroceptive need this part
            //self.obj[rule.alt_lhs] -= 1;
            setObjectCountFromMultisetObj(&agent->obj, rule->alt_lhs, COUNT_DECREMENT);

            //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()
            // 0 counts are allowed so if this is the case
            //if (self.obj[rule.alt_lhs] == 0):
                // remove the entry from the obj counter
                //del self.obj[rule.alt_lhs]

            //if (rule.alt_type == RuleType.evolution):
            if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_EVOLUTION)
                // add the rule.alt_rhs object to obj
                //self.obj[rule.alt_rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->alt_rhs, COUNT_INCREMENT);

            //elif (rule.alt_type == RuleType.communication):
            else if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_COMMUNICATION) {
                // if the rule.alt_rhs object is not in the environement any more
                //if (self.colony.env[rule.alt_rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->env, rule->alt_rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[1], rule->alt_rhs, rule_nr);
                    return FALSE;
                }
                // remove one instance of rule.alt_rhs from env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.alt_rhs != self.colony.e):
                if (rule->alt_rhs != OBJECT_ID_E) {
                    // remove one instance of rule.alt_rhs from env
                    //self.colony.env[rule.alt_rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->env, rule->alt_rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.env[rule.alt_rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.env[rule.alt_rhs]
                }
                // transfer object from environment to agent.obj
                //self.obj[rule.alt_rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->alt_rhs, COUNT_INCREMENT);

                // only modify the environment if the alt_lhs object is not e
                //if (rule.alt_lhs != self.colony.e):
                if (rule->alt_lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to environment
                    //self.colony.env[rule.alt_lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->env, rule->alt_lhs, COUNT_INCREMENT);
            }

            //elif (rule.alt_type == RuleType.exteroceptive):
            else if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_EXTEROCEPTIVE) {
                // if the rule.alt_rhs object is not in the global swarm environement any more
                //if (self.colony.parentSwarm.global_env[rule.alt_rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.global_env, rule->alt_rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[2], rule->alt_rhs, rule_nr);
                    return FALSE;
                }

                // remove one instance of rule.alt_rhs from global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.alt_rhs != self.colony.e):
                if (rule->alt_rhs != OBJECT_ID_E) {
                    // remove one instance of rule.alt_rhs from global swarm env
                    //self.colony.parentSwarm.global_env[rule.alt_rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.global_env, rule->alt_rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.global_env[rule.alt_rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.global_env[rule.alt_rhs]
                }
                // transfer object from environment to agent.obj
                //self.obj[rule.alt_rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->alt_rhs, COUNT_INCREMENT);

                // only modify the global swarm environment if the alt_lhs object is not e
                //if (rule.alt_lhs != self.colony.e):
                if (rule->alt_lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to environment
                    //self.colony.parentSwarm.global_env[rule.alt_lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.global_env, rule->alt_lhs, COUNT_INCREMENT);
            }

            //elif (rule.alt_type == RuleType.in_exteroceptive):
            else if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_IN_EXTEROCEPTIVE) {
                // if the rule.alt_rhs object is not in the INPUT global swarm environement any more
                //if (self.colony.parentSwarm.in_global_env[rule.alt_rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->alt_rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[3], rule->alt_rhs, rule_nr);
                    return FALSE;
                }

                // remove one instance of rule.alt_rhs from INPUT global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.alt_rhs != self.colony.e):
                if (rule->alt_rhs != OBJECT_ID_E) {
                    // remove one instance of rule.alt_rhs from INPUT global swarm env
                    //self.colony.parentSwarm.in_global_env[rule.alt_rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->alt_rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.in_global_env[rule.alt_rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.in_global_env[rule.alt_rhs]
                }
                // transfer object from environment to agent.obj
                //self.obj[rule.alt_rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->alt_rhs, COUNT_INCREMENT);

                // only modify the INPUT global swarm environment if the alt_lhs object is not e
                //if (rule.alt_lhs != self.colony.e):
                if (rule->alt_lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to environment
                    //self.colony.parentSwarm.in_global_env[rule.alt_lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.in_global_env, rule->alt_lhs, COUNT_INCREMENT);
            }

            //elif (rule.alt_type == RuleType.out_exteroceptive):
            else if (getSecondRuleTypeFromConditional(rule->type) == RULE_TYPE_OUT_EXTEROCEPTIVE) {
                // if the rule.alt_rhs object is not in the OUTPUT global swarm environement any more
                //if (self.colony.parentSwarm.out_global_env[rule.alt_rhs] <= 0):
                if (!areObjectsInMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->alt_rhs, NO_OBJECT)) {
                    // this is an error, some other agent modified the environement
                    printe(execErrMsgs[4], rule->alt_rhs, rule_nr);
                    return FALSE;
                }

                // remove one instance of rule.alt_rhs from OUTPUT global swarm env only if it not 'e'
                // 'e' object should remain constant in the environment
                //if (rule.alt_rhs != self.colony.e):
                if (rule->alt_rhs != OBJECT_ID_E) {
                    // remove one instance of rule.alt_rhs from OUTPUT global swarm env
                    //self.colony.parentSwarm.out_global_env[rule.alt_rhs] -= 1;
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->alt_rhs, COUNT_DECREMENT);
                    //THE OBJECT DELETION IS HANDLED BY setObjectCountFromMultiset()

                    // 0 counts are allowed so if this is the case
                    //if (self.colony.parentSwarm.out_global_env[rule.alt_rhs] == 0):
                        // remove the entry from the env counter
                        //del self.colony.parentSwarm.out_global_env[rule.alt_rhs]
                }
                // transfer object from environment to agent.obj
                //self.obj[rule.alt_rhs] += 1
                setObjectCountFromMultisetObj(&agent->obj, rule->alt_rhs, COUNT_INCREMENT);

                // only modify the OUTPUT global swarm environment if the alt_lhs object is not e
                //if (rule.alt_lhs != self.colony.e):
                if (rule->alt_lhs != OBJECT_ID_E)
                    // transfer object from agent.obj to environment
                    //self.colony.parentSwarm.out_global_env[rule.alt_lhs] += 1
                    setObjectCountFromMultisetEnv(&agent->pcolony->pswarm.out_global_env, rule->alt_lhs, COUNT_INCREMENT);
            }
        }    // end elif exec_rule_nr == second
    }
    // rule execution finished succesfully
    return TRUE;
}

sim_step_result_t pcolony_runSimulationStep(Pcolony_t *pcolony) {
    //runnableAgents = [] // the list of agents that have an executable program
    uint8_t executable_agents_count = 0;
    //possiblePrograms[2] = True -> program[2] is executable
    bool runnableAgents[pcolony->nr_agents];
    Agent_t *agent;

    initArray(runnableAgents, pcolony->nr_agents, FALSE);

    //for agent_name, agent in self.agents.items():
    for (uint8_t agent_nr = 0; agent_nr < pcolony->nr_agents; agent_nr++) {
        agent = &pcolony->agents[agent_nr];
        printd("Check AG%d", agent_nr);
        // if the agent choses 1 program to execute
        //if (agent.choseProgram()) {
        if (agent_choseProgram(agent)) {
            printi("AG%d runnable", agent_nr);
            //runnableAgents.append(agent_name)
            runnableAgents[agent_nr] = TRUE;
            executable_agents_count++;
        }
    }
    printi("%d runnable ag", executable_agents_count);

    // if there are no runnable agents
    //if (len(runnableAgents) == 0):
    if (executable_agents_count == 0)
        return SIM_STEP_RESULT_NO_MORE_EXECUTABLES; // simulation cannot continue

    //for agent_name in runnableAgents:
    for (uint8_t agent_nr = 0; agent_nr < pcolony->nr_agents; agent_nr++)
        if (runnableAgents[agent_nr]) {
            agent = &pcolony->agents[agent_nr];
            //printi("Running Agent %s  P%d = < %s >" % (agent_name, self.agents[agent_name].chosenProgramNr, self.agents[agent_name].programs[self.agents[agent_name].chosenProgramNr].print(onlyExecutable = True)))
            printi("Run AG%d  P%d", agent_nr, agent->chosenProgramNr);
            // if there were errors encountered during program execution
            if (!agent_executeProgram(agent)) {
                printe("Exec fail AG%d, STOP_SIM", agent_nr);
                return SIM_STEP_RESULT_ERROR;
            }
        }
    printi("Sim_step_ok");
    //return SimStepResult.finished
    return SIM_STEP_RESULT_FINISHED;
}


rule_type_t getFirstRuleTypeFromConditional(rule_type_t rule) {
    // if this is not a conditional rule
    if (rule < RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION)
        return RULE_TYPE_NONE;

    // return values from the lookup table
    return lookupFirst[rule - RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION];
}

rule_type_t getSecondRuleTypeFromConditional(rule_type_t rule) {
    // if this is not a conditional rule
    if (rule < RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION)
        return RULE_TYPE_NONE;

    // return values from the lookup table
    return lookupSecond[rule - RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION];
}

void initArray(uint8_t *array, uint8_t array_size, uint8_t default_value) {
    for (uint8_t i = 0; i < array_size; i++)
        array[i] = default_value;
}


void initPcolony(Pcolony_t *pcol, uint8_t nr_A, uint8_t nr_agents, uint8_t n) {
    //generate and set a seed for random number generation
    //using methods specific to the platform
    #ifndef KILOBOT
        //on PC we use the time in seconds since the epoch as seed
        srand(time(NULL));
    #else
        //on the kilobot we use the battery voltage as seed
        rand_seed(rand_hard());
    #endif
    pcol->nr_A = nr_A;
    pcol->nr_agents = nr_agents;
    pcol->n = n;

    //init environment
    initMultisetEnv(&pcol->env, pcol->nr_A);
    //init pswarm global environment
    initMultisetEnv(&pcol->pswarm.global_env, pcol->nr_A);
    //init pswarm INPUT global environment
    initMultisetEnv(&pcol->pswarm.in_global_env, pcol->nr_A);
    //init pswarm OUTPUT global environment
    initMultisetEnv(&pcol->pswarm.out_global_env, pcol->nr_A);
    //init agents
    pcol->agents = (Agent_t *) malloc(sizeof(Agent_t) * pcol->nr_agents);
}

void destroyPcolony(Pcolony_t *pcol) {

    //free agents
    if (pcol->nr_agents > 0) {
        for (uint8_t i = 0; i < pcol->nr_agents; i++)
            destroyAgent(&pcol->agents[i]);
        //free agent list
        free(pcol->agents);
        pcol->nr_agents = 0;
    }

    destroyMultisetEnv(&pcol->env);
    destroyMultisetEnv(&pcol->pswarm.global_env);
    destroyMultisetEnv(&pcol->pswarm.in_global_env);
    destroyMultisetEnv(&pcol->pswarm.out_global_env);

    pcol->n = 0;
}

void initAgent(Agent_t *agent, Pcolony_t *pcol, uint8_t nr_programs) {
    agent->nr_programs = nr_programs;
    agent->chosenProgramNr = -1;
    agent->init_program_nr = 0;

    agent->pcolony = pcol;
    agent->programs = (Program_t *) malloc(sizeof(Program_t) * agent->nr_programs);

    //initialize the agent's multiset at the size of the P colonies capacity
    initMultisetObj(&agent->obj, pcol->n);
}

void destroyAgent(Agent_t *agent) {
    destroyMultisetObj(&agent->obj);

    //destroy programs
    if (agent->nr_programs > 0) {
        for (uint8_t i = 0; i < agent->nr_programs; i++)
            destroyProgram(&agent->programs[i]);
        //free the programs list
        free(agent->programs);
        agent->nr_programs = 0;
    }

    agent->pcolony = 0;
    agent->chosenProgramNr = -1;
}

void initProgram(Program_t *program, uint8_t nr_rules) {
    program->nr_rules = nr_rules;
    program->rules = (Rule_t *) malloc(sizeof(Rule_t) * program->nr_rules);
}

void copyProgram(Program_t *destination, Program_t *source) {
    initProgram(destination, source->nr_rules);
    for (uint8_t rule_nr = 0; rule_nr < source->nr_rules; rule_nr++)
        initRule(&destination->rules[rule_nr],
                source->rules[rule_nr].type,
                source->rules[rule_nr].lhs,
                source->rules[rule_nr].rhs,
                source->rules[rule_nr].alt_lhs,
                source->rules[rule_nr].alt_rhs);
}

void destroyProgram(Program_t *program) {
    if (program->nr_rules > 0) {
        free(program->rules);
        program->nr_rules = 0;
    }
}

void initRule(Rule_t *rule, rule_type_t type, uint8_t lhs, uint8_t rhs, uint8_t alt_lhs, uint8_t alt_rhs) {
    rule->type = type;
    rule->lhs = lhs;
    rule->rhs = rhs;

    if (rule->type >= RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION) {
        rule->alt_lhs = alt_lhs;
        rule->alt_rhs = alt_rhs;
    }
    // alt_ is not needed for non-conditional rules
    else {
        rule->alt_lhs = NO_OBJECT;
        rule->alt_rhs = NO_OBJECT;
    }
}
