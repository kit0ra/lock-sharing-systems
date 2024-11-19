/**
 * @file ColouringResolution.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Algorithms to solve directly the Colouring problem
 * @version 1
 * @date 2023-09-22
 *
 * @copyright Creative Commons.
 *
 */

#ifndef COCA_COLOURING_RESOLUTION_H
#define COCA_COLOURING_RESOLUTION_H

#include "ColouredGraph.h"

/**
 * @brief Brute Force Algorithm to solve the colouring problem. If it is solvable, @p graph is modified so at the return of the algorithm, the nodes are coloured. If there is no solution, @p graph has all colours set to -1.
 *
 * @param graph A ColouredGraph.
 * @param num_colours The number of colours available.
 * @return true if there is a solution.
 * @return false if there is no solution.
 *
 * @pre @p graph must be valid.
 * @post @p if returns true, the colours of @p graph is a solution to the Colouring problem with @p num_colours colors.
 */
bool colouring_brute_force(ColouredGraph graph, int num_colours);

#endif