/**
 * @file debug_print.h
 * @brief Debugging utility funtions
 * In this header we define a set of macros that help with message logging
 * and can be easily removed from the code
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-23
 */
#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#ifndef DEBUG_PRINT
    #define printd(...)  do { } while(0)
    #define printi(...)  do { } while(0)
    #define printw(...)  do { } while(0)
    #define printe(...)  do { } while(0)
#else
    #include <stdio.h>

    #if DEBUG_PRINT == 0
        #ifndef PCOL_SIM
            #define printd(...) (puts("\nD: "),printf(__VA_ARGS__))
        #else
            #define printd(...) (puts("\n\e[36mD: "),printf(__VA_ARGS__),puts("\e[0m"))
        #endif
    #else
        #define printd(...)  do { } while(0)
    #endif

    #if DEBUG_PRINT <= 1
        #ifndef PCOL_SIM
            #define printi(...) (puts("\nI: "),printf(__VA_ARGS__))
        #else
            #define printi(...) (puts("\n\e[32mI: "),printf(__VA_ARGS__),puts("\e[0m"))
        #endif
    #else
        #define printi(...)  do { } while(0)
    #endif

    #if DEBUG_PRINT <= 2
        #ifndef PCOL_SIM
            #define printw(...) (puts("\nW: "),printf(__VA_ARGS__))
        #else
            #define printw(...) (puts("\n\e[33mW: "),printf(__VA_ARGS__),puts("\e[0m"))
        #endif
    #else
        #define printw(...)  do { } while(0)
    #endif

    #ifndef PCOL_SIM
        #define printe(...) (puts("\nE: "),printf(__VA_ARGS__))
    #else
        #define printe(...) (puts("\n\e[31mE: "),printf(__VA_ARGS__),puts("\e[0m"))
    #endif
#endif

#endif
