/**
 * @file ColouringReduction.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief An implementation of the colouring problem seen in exercise session. Converts a graph g to a formula true if and only if it is possible to colour it with a fixed number of colours such that neighbouring node have different colours.
 * Provides functions to generate the formula and the necessary variables, alongside function to decode a solution from a model of the formula.
 * @version 1
 * @date 2023-09-22
 *
 * @copyright Creative Commons
 *
 */

#ifndef COLOURING_RED_H_
#define COLOURING_RED_H_

#include "Graph.h"
#include "ColouredGraph.h"
#include <z3.h>

/**
 * @brief Generates a propositional formula satisfiable if and only if there is a partition which satisfies every player and all components are connected.
 *
 * @param ctx The solver context.
 * @param graph A ColouredGraph.
 * @param num_colours The number of colours available for colouring the graph.
 * @return Z3_ast The formula.
 * @pre @p graph must be initialized.
 */
Z3_ast colouring_reduction(Z3_context ctx, const ColouredGraph graph, int num_colours);

/**
 * @brief Colours @p graph according to @p model.
 *
 * @param ctx The solver context.
 * @param model A variable assignment.
 * @param graph A ColouredGraph.
 * @param num_colours The number of expected colours.
 * @pre @p model must be a valid model which has a truth value for each variable representing a pair colour, node.
 * @pre @p graph must be the ColouredGraph used to obtain @p model.
 */
void colour_graph_from_model(Z3_context ctx, Z3_model model, ColouredGraph graph, int num_colours);

/**
 * @brief Prints the values of the variables in @p model. @p graph and @p num_colours are used to determine which values to print. @p model should have been obtained through the satisfaction of a formula obtained with colouring_reduction.
 *
 * @param ctx The solver context.
 * @param model A model.
 * @param graph A ColouredGraph.
 * @param num_colours The number of expected colours.
 * @pre @p model must be a valid model which has a truth value for each variable representing a pair colour, node.
 * @pre @p graph must be the ColouredGraph used to obtain @p model
 */
void colouring_print_model(Z3_context ctx, Z3_model model, ColouredGraph graph, int num_colours);

#endif