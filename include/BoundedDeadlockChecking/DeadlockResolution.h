/**
 * @file DeadlockResolution.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief A Brute Force algorithm for the Bounded Deadlock Checking problem (project CoCa 2024).
 * @version 1
 * @date 2024-10-18
 * 
 * @copyright Creative Commons.
 * 
 */

#ifndef COCA_DEADLOCK_RESOLUTION_H
#define COCA_DEADLOCK_RESOLUTION_H

#include "LockAutomaton.h"

/**
 * @brief Brute Force Algorithm solving the bounded deadlock checking problem.
 * 
 * @param automata The LockAutomata considered.
 * @param num_automata The number of LockAutomata considered.
 * @param bound The size of the deadlock searched for.
 * @param path An array of Step. In case there is a deadlock of size @p bound, after the execution, it will contain a path leading to a deadlock.
 * @return true if there is a deadlock of size @p bound.
 * @return false otherwise.
 * 
 * @pre @p automata must be an array of valid LockAutomata of size @p num_automata.
 * @pre @p path must be an array of Step of size @p bound.
 * @pre @p bound >= 0.
 * @post @p path is modified by this function and if true is returned, contains a path leading to a deadlock.
 */
bool deadlock_brute_force(LockAutomaton *automata, int num_automata, int bound, step *path);

#endif