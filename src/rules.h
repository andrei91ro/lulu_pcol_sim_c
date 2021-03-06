// vim:filetype=c
/**
 * @file lulu.h
 * @brief Lulu P colony simulator rule types
 * This header defines all of the rule types that are accepted by the simulator.
 * This file was auto-generated by lulu_pcol_sim --ruleheader rules.h on 23 Jul 2017 at 14:24
 * @author Andrei G. Florea
 * @author Catalin Buiu
 * @date 2016-02-08
 */
#ifndef RULES_H
#define RULES_H

#include <stdint.h>
typedef enum _rule_type {
    //non-conditional (single rules)
    RULE_TYPE_NONE = 0,
            
    RULE_TYPE_EVOLUTION,
    RULE_TYPE_COMMUNICATION,
    RULE_TYPE_EXTEROCEPTIVE,
    RULE_TYPE_IN_EXTEROCEPTIVE,
    RULE_TYPE_OUT_EXTEROCEPTIVE,
    //conditional (pair of rules)
    RULE_TYPE_CONDITIONAL_EVOLUTION_EVOLUTION = 10,
    RULE_TYPE_CONDITIONAL_EVOLUTION_COMMUNICATION,
    RULE_TYPE_CONDITIONAL_EVOLUTION_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_EVOLUTION_IN_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_EVOLUTION_OUT_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_COMMUNICATION_EVOLUTION,
    RULE_TYPE_CONDITIONAL_COMMUNICATION_COMMUNICATION,
    RULE_TYPE_CONDITIONAL_COMMUNICATION_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_COMMUNICATION_IN_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_COMMUNICATION_OUT_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_EXTEROCEPTIVE_EVOLUTION,
    RULE_TYPE_CONDITIONAL_EXTEROCEPTIVE_COMMUNICATION,
    RULE_TYPE_CONDITIONAL_EXTEROCEPTIVE_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_EXTEROCEPTIVE_IN_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_EXTEROCEPTIVE_OUT_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_IN_EXTEROCEPTIVE_EVOLUTION,
    RULE_TYPE_CONDITIONAL_IN_EXTEROCEPTIVE_COMMUNICATION,
    RULE_TYPE_CONDITIONAL_IN_EXTEROCEPTIVE_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_IN_EXTEROCEPTIVE_IN_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_IN_EXTEROCEPTIVE_OUT_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_OUT_EXTEROCEPTIVE_EVOLUTION,
    RULE_TYPE_CONDITIONAL_OUT_EXTEROCEPTIVE_COMMUNICATION,
    RULE_TYPE_CONDITIONAL_OUT_EXTEROCEPTIVE_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_OUT_EXTEROCEPTIVE_IN_EXTEROCEPTIVE,
    RULE_TYPE_CONDITIONAL_OUT_EXTEROCEPTIVE_OUT_EXTEROCEPTIVE,
} rule_type_t;

// the tables are generated according to the order of the rules defined in rule_type_t
extern rule_type_t lookupFirst[];
extern rule_type_t lookupSecond[];

#ifdef PCOL_SIM
    extern char* ruleNames[];
#endif

#endif