// vim:filetype=c
/**
 * @file lulu_instance.h
 * @brief Lulu P colony simulator internal structure.
 * In this header we define the structure of the Pcolony that will power the simulated robot
 * This file was generated automatically by lulu_c.py at DATETIME
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-29
 */
#ifndef LULU_INSTANCE_H
#define LULU_INSTANCE_H

#include "lulu.h"

enum objects {
    OBJECT_ID_D = 3,
    OBJECT_ID_S,
    OBJECT_ID_B,
    //objects expanded by lulu_c.py
    OBJECT_ID_B_$, //we cannot use * because it is an operator
    OBJECT_ID_B_$id, //this should be replaced with the robot's id
    OBJECT_ID_B_1,

    OBJECT_ID_S_$id,
    OBJECT_ID_S_0,
    OBJECT_ID_S_1,

    OBJECT_ID_ID_$id,
    OBJECT_ID_ID_0,
    OBJECT_ID_ID_1,
};

enum agents {
    AGENT_COMMAND,
    AGENT_MOTION,
};

#define USING_AGENT_MOTION //this ensures that the code associated with the MOTION agent is included in Lulu_kilobot
#define USING_AGENT_LED_RGB //this ensures that the code associated with the LED_RGB agent is included in Lulu_kilobot
#define USING_AGENT_MSG_DISTANCE //this ensures that the code associated with the MSG_DISTANCE agent is included in Lulu_kilobot

//if building Pcolony simulator for PC
#ifdef PCOL_SIM
    //define array of names for objects and agents for debug
    extern char* objectNames[];
    extern char* agentNames[];
#endif

/**
 * @brief The smallest kilo_uid from the swarm (is set in instance.c by lulu_c.py)
 */
extern const uint16_t smallest_robot_uid;

/**
 * @brief The number of robots that make up the swarm (is set in instance.c by lulu_c.py)
 */
extern const uint16_t nr_swarm_robots;

/**
 * @brief Initialises the pcol object and all of it's components
 *
 * @param pcol The P colony that will be initialized
 */
void lulu_init(Pcolony_t *pcol);

/**
 * @brief Destroys the pcol objects and all of it's components
 *
 * @param pcol The P colony that will be destroyed
 */
void lulu_destroy(Pcolony_t *pcol);

#ifdef NEEDING_WILDCARD_EXPANSION
    /**
     * @brief Expands and replaces wildcarded objects with the appropriate objects
     * Objects that end with _$ID are replaced with _i where i is the the id of the robot, provided with my_id parameter
     *
     * @param pcol The pcolony where the replacements will take place
     * @param my_id The kilo_uid of the robot
     * @return The symbolic id that corresponds to this robot (my_id - smallest_robot_uid)
     */
    uint16_t expand_pcolony(Pcolony_t *pcol, uint16_t my_id);
#endif

#endif
