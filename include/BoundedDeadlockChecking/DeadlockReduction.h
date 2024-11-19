/**
 * @file DeadlockReduction.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief A reduction from the Bounded Deadlock Checking problem towards SAT (using Z3). Contains as well helper function to decode the solution, and print the model of the generated formula (when satisfiable).
 * @version 1
 * @date 2024-10-18
 * 
 * @copyright Creative Commons.
 * 
 */
#ifndef DEADLOCK_RED_H_
#define DEADLOCK_RED_H_

#include "Graph.h"
#include "LockAutomaton.h"
#include <z3.h>

/**
 * @brief Generates a propositional formula satisfiable if and only if there exists a parallel execution of size @p bound between the automata in @p automata.
 * 
 * @param ctx The solver context.
 * @param automata The LockAutomata considered.
 * @param num_automata The number of automata.
 * @param bound The size of the deadlock searched for.
 * @return Z3_ast The formula
 * 
 * @pre @p automata must be an array of valid LockAutomata of size @p num_automata.
 * @pre bound >= 0.
 */
Z3_ast deadlock_reduction(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound);

/**
 * @brief Constructs a path from a @p model.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param automata The LockAutomata considered.
 * @param num_automata The number of automata.
 * @param path An array representing a path.
 * @param bound The size of the path.
 * 
 * @pre @p automata must be an array of valid LockAutomata of size @p num_automata.
 * @pre bound >= 0.
 * @pre @p model must be a valid model having a truth value for variables used by deadlock_reduction with @p automata and @p bound.
 */
void la_path_from_model(Z3_context ctx, Z3_model model, LockAutomaton *automata, int num_automata, step *path, int bound);

/**
 * @brief Prints (in pretty format) which variables used by deadlock_reduction are true in @p model.
 * 
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param automata The LockAutomata considered.
 * @param num_automata The number of automata.
 * @param bound The size of the expected deadlock.
 * 
 * @pre @p automata must be an array of valid LockAutomata of size @p num_automata.
 * @pre bound >= 0.
 * @pre @p model must be a valid model having a truth value for variables used by deadlock_reduction with @p automata and @p bound.
 */
void la_print_model(Z3_context ctx, Z3_model model, LockAutomaton *automata, int num_automata, int bound);

#endif