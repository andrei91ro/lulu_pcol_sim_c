// vim:filetype=c
/**
 * @file lulu.h
 * @brief Lulu P colony simulator internal structure.
 * In this header we define all of the structures that are used to store a parsed P colony / P swarm for use in simulation
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-08
 */
#ifndef LULU_H
#define LULU_H

#include <stdint.h>
#include "rules.h" //dynamically generated RULE_TYPE_* enum

#define TRUE 1
#define FALSE 0

#define NO_OBJECT 0 // -1 is not available for uint

#define COUNT_INCREMENT 254
#define COUNT_DECREMENT 255

#define OBJECT_ID_E 1
#define OBJECT_ID_F 2

typedef uint8_t bool;

/**
 * @brief Enumeration of rule selection options (used mainly for marking the executable rule from a conditional rule)
 */
typedef enum _rule_exec_option {
    RULE_EXEC_OPTION_NONE,
    RULE_EXEC_OPTION_FIRST,
    RULE_EXEC_OPTION_SECOND
} rule_exec_option_t;

/**
 * @brief Enumeration of possible results of running a simulation step
 */
typedef enum _sim_step_result {
    SIM_STEP_RESULT_FINISHED,
    SIM_STEP_RESULT_NO_MORE_EXECUTABLES,
    SIM_STEP_RESULT_ERROR
} sim_step_result_t;

/**
 * @brief Structure used to retain a symbolic object present in multiset containers such as Pcolony.env, Pswarm.global_env
 */
typedef struct _multiset_env_item {
    uint8_t id, // object id (index from alphabet)
            nr; // nr of objects of this type
} multiset_env_item_t;

/**
 * @brief Structure used to define a multiset that can hold simbolic objects and the number of times they appear for use with environment / global environment of P colonies or P swarms
 * This structure is adequate for environments because it eficiently stores the number of appereances of a symbolic object.
 */
typedef struct _multiset_env {
    multiset_env_item_t *items;
    uint8_t size;
} multiset_env_t;

/**
 * @brief Multiset structure used for limited storage components such as the objects from an Agent (that are limited by the capacity of the P colony)
 */
typedef struct _multiset_obj {
    uint8_t *items;
    uint8_t size;
} multiset_obj_t;

typedef struct _Pswarm Pswarm_t;
typedef struct _Pcolony Pcolony_t;
typedef struct _Agent Agent_t;
typedef struct _Program Program_t;
typedef struct _Rule Rule_t;
//typedef union _Rule Rule_t;

/**
 * @brief Rule struct used to represent rules that compose a program.
 */
struct _Rule {
    rule_type_t type; // defines the type of the entire rule (including conditional combinations) using rule_type_t
    rule_exec_option_t exec_rule_nr; // retains the rule marked for execution (none, first, second)
    uint8_t lhs, // Left Hand Side operand
            rhs, // Right Hand Side operand
            alt_lhs, // Left Hand Side operand for alternative rule
            alt_rhs; // Right Hand Side operand for alternative rule
};
//union _Rule {
    //struct {
        //rule_type_t type; // defines the type of the entire rule (including conditional combinations) using rule_type_t
        //rule_exec_option_t exec_rule_nr; // retains the rule marked for execution (none, first, second)
        //uint8_t lhs, // Left Hand Side operand
                //rhs; // Right Hand Side operand
    //};

    //struct {
        //rule_type_t type1; // defines the type of the entire rule (including conditional combinations) using rule_type_t
        //rule_exec_option_t exec_rule_nr1; // retains the rule marked for execution (none, first, second)
        //uint8_t lhs1, // Left Hand Side operand
                //rhs1, // Right Hand Side operand
                //alt_lhs, // Left Hand Side operand for alternative rule
                //alt_rhs; // Right Hand Side operand for alternative rule
    //};
//};

/**
 * @brief Program struct used to encapsulate a list o rules.
 */
struct _Program {
    uint8_t nr_rules;
    Rule_t *rules;
};

/**
 * @brief Agent struct used to represent a P colony agent.
 */
struct _Agent {
    uint8_t nr_programs,
            chosenProgramNr, // the program number that was chosen for execution
            init_program_nr; //the number of programs that were initialized
    Pcolony_t *pcolony; // reference to my parent colony (for acces to env)

    //we could have used Program_t programs[] but in struct we are only alowed ONE variable lenght array
    Program_t *programs; // list of programs (each program is a list of n  Rule_t structs)
    multiset_obj_t obj; // objects stored by the agent (stored as a multiset using a pair id - nr_objects)
};

/**
 * @brief Pswarm class that holds all the components of an Pswarm (colony of colonies)
 */
struct _Pswarm {
    multiset_env_t global_env, // store the objects from the global (swarm) environemnt
                   in_global_env, // store the objects from the INPUT global (swarm) environemnt
                   out_global_env; // store the objects from the OUTPUT global (swarm) environemnt
};

/**
 * @brief Pcolony struct that holds all the components of a P colony.
 */
struct _Pcolony {
   uint8_t  nr_A, // nr of alphabet elements
            nr_agents,
            n; // capacity

    //we could have used multiset_env_item_t env[] but in struct we are only alowed ONE variable lenght array
    multiset_env_t env; // store array of objects found in the environment (stored as a multiset using a pair id - nr_objects)
    Agent_t *agents; // agent array
    Pswarm_t pswarm; //reference to Pswarm
};

/******************************************************************************************************************************/
//FUNCTION PROTOTYPES


/******************************************************************************************************************************/
//Multiset auxiliary functions
/**
 * @brief Create an environment multiset of the specified size
 *
 * @param multiset Pointer to the multiset that will be initialized
 * @param size The number of objects that this multiset will hold
 */
void initMultisetEnv(multiset_env_t *multiset, uint8_t size);

/**
 * @brief Create an object multiset of the specified size
 *
 * @param multiset Pointer to the multiset that will be initialized
 * @param size The number of objects that this multiset will hold
 */
void initMultisetObj(multiset_obj_t *multiset, uint8_t size);

/**
 * @brief Clear an initialized multiset by setting all objects to NO_OBJECT and count 0
 *
 * @param multiset Pointer to the multiset that will be cleared
 */
void clearMultisetEnv(multiset_env_t *multiset);

/**
 * @brief Clear an initialized multiset by setting all objects to NO_OBJECT
 *
 * @param multiset Pointer to the multiset that will be cleared
 */
void clearMultisetObj(multiset_obj_t *multiset);

/**
 * @brief Deallocate the space used by this environment multiset
 *
 * @param multiset Pointer to the multiset that will be destroyed
 */
void destroyMultisetEnv(multiset_env_t *multiset);

/**
 * @brief Deallocate the space used by this object multiset
 *
 * @param multiset Pointer to the multiset that will be destroyed
 */
void destroyMultisetObj(multiset_obj_t *multiset);

/**
 * @brief Check that at least one of the provided symbolic objects is present in the multiset
 * This method is a optimization motivated compromise that allows to simultaneously check
 * that either one of the objects is present in the multiset.
 *
 * If there is no second object, it can be replaced with NO_OBJECT
 *
 * @param multiset The multiset that is searched (haystack)
 * @param obj1 The object that is searched (needle)
 * @param obj2 The object that is searched (needle)
 *
 * @return TRUE / FALSE
 */
bool areObjectsInMultisetEnv(multiset_env_t *multiset, uint8_t obj1, uint8_t obj2);

/**
 * @brief Check that at least one of the provided symbolic objects is present in the multiset
 * This method is a optimization motivated compromise that allows to simultaneously check
 * that either one of the objects is present in the multiset.
 *
 * If there is no second object, it can be replaced with NO_OBJECT
 *
 * @param multiset The multiset that is searched (haystack)
 * @param obj1 The object that is searched (needle)
 * @param obj2 The object that is searched (needle)
 *
 * @return TRUE / FALSE
 */
bool areObjectsInMultisetObj(multiset_obj_t *multiset, uint8_t obj1, uint8_t obj2);

/**
 * @brief Get the number of occurences of a symbolic object in a multiset
 *
 * @param multiset The multiset that is searched (haystack)
 * @param obj The object that is searched (needle)
 *
 * @return The number of occurences of the object in the multiset
 */
uint8_t getObjectCountFromMultisetEnv(multiset_env_t *multiset, uint8_t obj);

/**
 * @brief Get the number of occurences of a symbolic object in a multiset
 *
 * @param multiset The multiset that is searched (haystack)
 * @param obj The object that is searched (needle)
 *
 * @return The number of occurences of the object in the multiset
 */
uint8_t getObjectCountFromMultisetObj(multiset_obj_t *multiset, uint8_t obj);

/**
 * @brief Set the number of occurences of a symbolic object in a multiset (and optionally delete the object)
 *
 * @param multiset The multiset that will be modified
 * @param obj The target object
 * @param newCount The new count that will corespond to the object
 *          if <=0 then the object is removed from the multiset
 *          newCount can be COUNT_INCREMENT / COUNT_DECREMENT to simplify the respective operations
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool setObjectCountFromMultisetEnv(multiset_env_t *multiset, uint8_t obj, uint8_t newCount);

/**
 * @brief Set the number of occurences of a symbolic object in a multiset (and optionally delete the object)
 *
 * @param multiset The multiset that will be modified
 * @param obj The target object
 * @param newCount The new count that will corespond to the object
 *          if <=0 then the object is removed from the multiset
 *          newCount can be COUNT_INCREMENT / COUNT_DECREMENT to simplify the respective operations
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool setObjectCountFromMultisetObj(multiset_obj_t *multiset, uint8_t obj, uint8_t newCount);


/**
 * @brief Adds one symbolic object to a multiset_obj
 *
 * @param multiset The multiset where we add an object
 * @param obj The id of the symbolic object that will be added
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool addObjectToMultisetObj(multiset_obj_t *multiset, uint8_t obj);

/**
 * @brief Deletes one symbolic object from a multiset_obj
 *
 * @param multiset The multiset from where we delete an object
 * @param obj The id of the symbolic object that will be deleted
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool delObjectFromMultisetObj(multiset_obj_t *multiset, uint8_t obj);

/**
 * @brief Check whether one multiset is included in a parent multiset
 *
 * @param parent The multiset that is checked whether it contains the secondary multiset
 * @param child The multiset that is checked for inclusion
 *
 * @return TRUE / FALSE depending on whether the inclusion is true or not
 */
bool isMultisetEnvIncluded(multiset_env_t *parent, multiset_env_t *child);

/**
 * @brief Check whether one multiset is included in a parent multiset
 *
 * @param parent The multiset that is checked whether it contains the secondary multiset
 * @param child The multiset that is checked for inclusion
 *
 * @return TRUE / FALSE depending on whether the inclusion is true or not
 */
bool isMultisetObjIncluded(multiset_obj_t *parent, multiset_obj_t *child);

/**
 * @brief Replaces one symbolic object from a multiset with another object
 * This method only changes the id of the initial object to the final one,
 * not the associated number of appearences. 
 *
 * @param multiset The multiset where the inital object resides
 * @param initial_obj The id of the symbolic object that will be replaced
 * @param final_obj The id of the symbolic object that will replace the initial object
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool replaceObjInMultisetEnv(multiset_env_t *multiset, uint8_t initial_obj, uint8_t final_obj);

/**
 * @brief Replaces one symbolic object from a multiset with another object
 * This method replaces all instances of the initial_obj found in the multiset
 *
 * @param multiset The multiset where the inital object resides
 * @param initial_obj The id of the symbolic object that will be replaced
 * @param final_obj The id of the symbolic object that will replace the initial object
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool replaceObjInMultisetObj(multiset_obj_t *multiset, uint8_t initial_obj, uint8_t final_obj);

/**
 * @brief Replace ONLY one symbolic object from a multiset with another object
 * This method replaces ONLY one instance of the initial_obj found in the multiset
 *
 * @param multiset The multiset where the inital object resides
 * @param initial_obj The id of the symbolic object that will be replaced
 * @param final_obj The id of the symbolic object that will replace the initial object
 *
 * @return TRUE / FALSE depending on the success of the operation
 */
bool replaceOneObjInMultisetObj(multiset_obj_t *multiset, uint8_t initial_obj, uint8_t final_obj);
/******************************************************************************************************************************/
//end Multiset auxiliary functions

/**
 * @brief Chose an executable program (or chose stochastically from a list of executable programs)
 *
 * @param agent Pointer to the agent that will chose one of it's programs for execution
 *
 * @return TRUE / FALSE depending on the availability of an executable program
 */
bool agent_choseProgram(Agent_t *agent);

/**
 * @brief Execute the selected program and modify the agent and the environement according to the rules
 *
 * @param agent Pointer to the agent that will execute one of it's (previously chosen) programs
 *
 * @return TRUE / FALSE depending on the succesfull execution the program
 */
bool agent_executeProgram(Agent_t *agent);

/**
 * @brief Runs 1 simulation step consisting of chosing (if available) and executing a program for each agent in the colony
 *
 * @param pcolony Pointer to the P colony where the simulation will take place
 *
 * @return sim_step_result values depending on the success of the current run
 * @see sim_step_result_t
 */
sim_step_result_t pcolony_runSimulationStep(Pcolony_t *pcolony);

/**
 * @brief Return the type of the first rule from a conditional rule
 *
 * @param rule The conditional rule that has to be checked
 *
 * @return A valid rule type if rule was conditional or RULE_TYPE_NONE otherwise
 */
rule_type_t getFirstRuleTypeFromConditional(rule_type_t rule);

/**
 * @brief Return the type of the second rule from a conditional rule
 *
 * @param rule The conditional rule that has to be checked
 *
 * @return A valid rule type if rule was conditional or RULE_TYPE_NONE otherwise
 */
rule_type_t getSecondRuleTypeFromConditional(rule_type_t rule);

/**
 * @brief Initialize an already defined array with a default value
 *
 * @param array The unitialized array
 * @param array_size The of elements contained in the array
 * @param default_value The default value that will be assigned to each member of the array
 */
void initArray(uint8_t *array, uint8_t array_size, uint8_t default_value);

/**
 * @brief Initialize a P colony object
 *
 * @param pcol The p colony that will be initialized
 * @param nr_A The size of the alphabet (nr of objects)
 * @param nr_agents The number of agents
 * @param n The capacity of the P colony (nr of objects contained in each agent)
 */
void initPcolony(Pcolony_t *pcol, uint8_t nr_A, uint8_t nr_agents, uint8_t n);

/**
 * @brief Destroy a P colony object and deallocate all ocupied space
 * This method also destroys all of the contained agents
 *
 * @param pcol The P colony that will be destroyed
 */
void destroyPcolony(Pcolony_t *pcol);

/**
 * @brief Initialize an Agent object
 *
 * @param agent The agent that will be initialized
 * @param pcol Reference to the parent P colony
 * @param nr_programs The number of programs that make up this agent
 */
void initAgent(Agent_t *agent, Pcolony_t *pcol, uint8_t nr_programs);

/**
 * @brief Destroy an Agent object and deallocate all ocupied space
 * This method also destroys all of the contained programs
 *
 * @param agent The agent that will be destroyed
 */
void destroyAgent(Agent_t *agent);

/**
 * @brief Initialize a Program object
 *
 * @param program The program that will be initialized
 * @param nr_rules The number of rules that compose this program (equal to the capacity of the P colony)
 */
void initProgram(Program_t *program, uint8_t nr_rules);

/**
 * @brief Create a deep-copy of the source program and store it into the destination program
 *
 * @param destination Program where the copy will be stored
 * @param source Program that will be copied
 */
void copyProgram(Program_t *destination, Program_t *source);

/**
 * @brief Destroy a Program object and deallocate all ocupied space
 *
 * @param program The program that will be destroyed
 */
void destroyProgram(Program_t *program);

/**
 * @brief Initialize a Rule object with the specified parameters
 *
 * @param rule Reference to the Rule that will be initialized
 * @param type The RULE_TYPE_ value used for this program
 * @param lhs The OBJECT_ID_ value for the left-hand-side part of the main rule
 * @param rhs The OBJECT_ID_ value for the right-hand-side part of the main rule
 * @param alt_lhs The OBJECT_ID_ value for the left-hand-side part of the secondary rule (only set for conditional rules)
 * @param alt_rhs The OBJECT_ID_ value for the right-hand-side part of the secondary rule (only set for conditional rules)
 */
void initRule(Rule_t *rule, rule_type_t type, uint8_t lhs, uint8_t rhs, uint8_t alt_lhs, uint8_t alt_rhs);

#endif
