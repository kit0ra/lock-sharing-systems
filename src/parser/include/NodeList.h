/**
 * @file NodeList.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to store a list of graph nodes that can be dynamically modified. Used as a temporary structure during parsing before translating into a more static structure.
 *         Includes automata features (initial and final nodes), and color of nodes.
 * @version 2
 * @date 2019-06-24
 *
 * @copyright Creative Commons.
 *
 */

#ifndef COCA_NODELIST_H_
#define COCA_NODELIST_H_

#include <stdbool.h>
#include "Graph.h"

/**
 * @brief The NodeList structure
 */
typedef struct tagSNodeList
{
    char *node;
    parameterList *parameters;
    struct tagSNodeList *next;
} SNodeList;

/**
 * @brief Adds a node in front of a list (works if list is null).
 * @param n1 the node
 * @param list the list to append to
 * @return the new list or NULL in case of no memory.
 */

SNodeList *addNode(char *n1, SNodeList *list);

/**
 * @brief If n is present in the list, does nothing. Otherwise, adds the node at the end of the list.
 *
 * @param n the node to add.
 * @param list the list to modify.
 */
void addOrUpdateNode(char *n, SNodeList *list);

/**
 * @brief Adds the parameter list parameters to node node if node is present in the list of nodes list.
 *
 * @param node the node to which to add a parameter.
 * @param parameters the list of parameters to add to node.
 * @param list the list of nodes.
 */
void add_parameters_to_node(char *node, parameterList *parameters, SNodeList *list);

/**
 * @brief Prints a NodeList.
 *
 * @param e a NodeList to print.
 */
void printNodeList(SNodeList *e);

/**
 * @brief Deletes a node list
 * @param b The node list
 */
void deleteNodeList(SNodeList *b);

#endif /* DOT_PARSER_NODELIST_H_ */
