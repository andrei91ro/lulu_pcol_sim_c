#include "lulu.h"
#include <stdlib.h>

//bitmasks that show where is the searched object (isObjectInRule()) located within the rule
extern const uint8_t OBJECT_IS_RULE_LHS; //Left Hand Side object is the searched object
extern const uint8_t OBJECT_IS_RULE_RHS; //Right Hand Side object is the searched object
extern const uint8_t OBJECT_IS_RULE_ALT_LHS; //Alternative Left Hand Side is the searched object
extern const uint8_t OBJECT_IS_RULE_ALT_RHS; //Alternative Right Hand Side is the searched object

/**
 * @brief Replaces one symbolic object from a program with another object
 * This method replaces all instances of the initial_obj found in the program
 *
 * @param program The program where the inital object resides
 * @param initial_obj The id of the symbolic object that will be replaced
 * @param final_obj The id of the symbolic object that will replace the initial object
 *
 * @return TRUE / FALSE depending on the success of the operation
 * @see replaceObjInMultisetEnv replaceObjInMultisetEnv
 */
bool replaceObjInProgram(Program_t *program, uint8_t initial_obj, uint8_t final_obj);

/**
 * @brief Check that the provided symbolic objects is present in the program
 *
 * @param program The program that is searched (haystack)
 * @param obj1 The object that is searched (needle)
 *
 * @return TRUE / FALSE depending on the presence of the object in the program
 */
uint8_t isObjectInProgram(Program_t *program, uint8_t obj);

/**
 * @brief Check that the provided symbolic object is present in the rule
 *
 * @param rule The rule that is searched (haystack)
 * @param obj1 The object that is searched (needle)
 *
 * @return OBJECT_IS_RULE_* values (in a bitwise or combination, depending on the presence of the object) e.g. OBJECT_IS_RULE_LHS | OBJECT_IS_RULE_ALT_RHS
 */
uint8_t isObjectInRule(Rule_t *rule, uint8_t obj);

/**
 * @brief Replaces the W_ID wildcard object into the object corresponding to the symbolic id, in all structures of the Pcolony
 * E.g for obj_with_id[OBJECT_ID_B_W_ID] and my_symbolic_id = 2 then OBJECT_ID_B_W_ID -> OBJECT_ID_B_2 in Pcolony.env,
 * Pswarm.global_env, any Agent.obj and in any rule that contains the wildcarded object
 *
 * @param pcol The Pcolony where the replacement takes place
 * @param obj_with_id[] The array of objects that contain the W_ID wildcard
 * @param obj_with_id_size The size (number of elements) in the obj_with_id array
 * @param my_symbolic_id The computed symbolic id of this robot
 */
void replacePcolonyWildID(Pcolony_t *pcol, uint8_t obj_with_id[], uint8_t obj_with_id_size, uint8_t my_symbolic_id);

/**
 * @brief Expands the W_ALL wildcard object into all of the objects (0 -> nr_swarm_robots) except my_symbolic_id in all of the structures of the Pcolony
 * E.g for obj_with_any[OBJECT_ID_B_W_ALL] and nr_swarm_robots = 3 then OBJECT_ID_B_W_ALL -> [OBJECT_ID_B_0, OBJECT_ID_B_1, OBJECT_ID_B_2]
 * in Pcolony.env, Pswarm.global_env, any Agent.obj and in any rule that contains the wildcarded object
 *
 * @param pcol The Pcolony where the expansion takes place
 * @param obj_with_any[] The array of objects that contain the W_ALL wildcard
 * @param is_obj_with_any_followed_by_id[] An array that specifies (with 0 / 1) whether each element in obj_with_any[] is or not followed by and W_ID object
 * @param obj_with_any_size The size (number of elements) in the obj_with_any array (applies to is_obj_with_any_followed_by_id[])
 * @param my_symbolic_id The computed symbolic id of this robot
 * @param nr_swarm_robots The total number of swarm robots. This number is used for the actual expansion (0 .. nr_swarm_robots - 1)
 */
void expandPcolonyWildAny(Pcolony_t *pcol, uint8_t obj_with_any[], uint8_t is_obj_with_any_followed_by_id[], uint8_t obj_with_any_size, uint8_t my_symbolic_id, uint8_t nr_swarm_robots);
