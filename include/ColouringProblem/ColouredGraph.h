/**
 * @file ColouredGraph.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to a graph with colours coded as an integer. The colours themselves are not part of the structure, only the fact they are same or different matters.
 * @version 1
 * @date 2023-09-20
 *
 * @copyright Creative Commons.
 *
 */

#ifndef COCA_COLOURED_GRAPH_H
#define COCA_COLOURED_GRAPH_H

#include <stdbool.h>
#include "Graph.h"

/**
 * @brief The struct containing a graph, and a colour for each node.
 */

typedef struct ColouredGraph_s *ColouredGraph;

/**
 * @brief Initializes a ColouredGraph from a Graph for use in the functions.
 * Does not color the graph (for use in colouring problem).
 * The graph is NOT copied (as it is not supposed to be modified).
 *
 * @param graph The Graph that is the input of the problem.
 * @return ColouredGraph The structure ColouredGraph described above.
 */
ColouredGraph cg_initialize(Graph graph);

/**
 * @brief Printer function to display information about @p graph in input.
 *
 * @param graph A ColouredGraph.
 */
void cg_print(ColouredGraph graph);

/**
 * @brief Printer function to display the colors of nodes in @p graph.
 *
 * @param graph A ColouredGraph.
 */
void cg_print_colors(ColouredGraph graph);

/**
 * @brief Deallocates memory used by a @p graph (what is allocated by rg_initialize). Warning: does NOT deallocate the Graph inside the @p graph. The Graph must be deallocated with adequate function from Graph.h.
 *
 * @param graph A ColouredGraph.
 */
void cg_delete(ColouredGraph graph);

/**
 * @brief Returns the number of nodes of @p graph.
 *
 * @param graph A ColouredGraph.
 * @return int Its number of nodes.
 */
int cg_get_num_nodes(ColouredGraph graph);

/**
 * @brief Returns the number of edges of @p graph.
 *
 * @param graph A ColouredGraph.
 * @return int Its number of edges.
 */
int cg_get_num_edges(ColouredGraph graph);

/**
 * @brief Gets if (@p source,@p target) is an edge in @p graph.
 *
 * @param graph A ColouredGraph.
 * @param source A node.
 * @param target A node.
 * @return true if (@p source,@p target) is an edge in @p graph.
 * @return false if (@p source,@p target) is not an edge in @p graph.
 */
bool cg_is_edge(ColouredGraph graph, int source, int target);

/**
 * @brief Gets the name of @p node in @p graph. The name is what appears in the .dot file, while its number is local to this program.
 *
 * @param graph A ColouredGraph.
 * @param node A node
 * @return char* Its name.
 */
char *cg_get_node_name(ColouredGraph graph, int node);

/**
 * @brief Gets the color of @p node in @p graph. If the color is not set, returns -1.
 *
 * @param graph A ColouredGraph.
 * @param node A node.
 * @return int Its color.
 */
int cg_get_node_colour(ColouredGraph graph, int node);

/**
 * @brief Sets the colour of @p node to be @p colour in @p graph.
 *
 * @param graph A ColouredGraph.
 * @param node A node.
 * @param colour A colour.
 */
void cg_set_node_colour(ColouredGraph graph, int node, int colour);

/**
 * @brief Writes a dot file named <@p name>.dot representing @p graph with partition information.
 *
 * @param graph A ColouredGraph.
 * @param name A name of file.
 */
void cg_create_dot(ColouredGraph graph, char *name);

#endif