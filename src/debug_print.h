/**
 * @file debug.h
 * @brief Debugging utility funtions
 * In this header we define a set of macros that help with message logging
 * and can be easily removed from the code
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-23
 */
#ifndef DEBUG_H
#define DEBUG_H

#ifndef DEBUG
    #define printd(fmt)  do { } while(0)
    #define printi(fmt)  do { } while(0)
    #define printw(fmt)  do { } while(0)
    #define printe(fmt)  do { } while(0)
#else
    #include <stdio.h>

    #if DEBUG == 0
        #define printd(fmt) printf("\nDEBUG: "); printf fmt
    #else
        #define printd(fmt)  do { } while(0)
    #endif

    #if DEBUG <= 1
        #define printi(fmt) printf("\nINFO: "); printf fmt
    #else
        #define printi(fmt)  do { } while(0)
    #endif

    #if DEBUG <= 2
        #define printw(fmt) printf("\nWARNING: "); printf fmt
    #else
        #define printw(fmt)  do { } while(0)
    #endif

    #define printe(fmt) printf("\nERROR: "); printf fmt
#endif

#endif