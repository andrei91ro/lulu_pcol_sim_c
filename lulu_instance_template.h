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
};

enum agents {
    AGENT_COMMAND,
    AGENT_MOTION,
};

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

#endif
