/**
 * @file Graph.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to store a graph statically, and to access to its informations easily.
		   Includes source and target node informations, and colors.
 * @version 2
 * @date 2018-11-18, 2019-07-23, 2019-08-02, 2020-06-24
 *
 * @copyright Creative Commons.
 *
 */

#ifndef COCA_GRAPH_H_
#define COCA_GRAPH_H_

#include <stdbool.h>
#include <stdio.h>

/**
 * @brief List of arbitrary parameters
 *
 */
typedef struct tag_paramList
{
	char *name;					///< The name (key) of the parameter.
	char *value;				///< The value of the parameter.
	struct tag_paramList *next; ///< Pointer to the next parameter (NULL if last).
} parameterList;

/**
 * @brief Adds a parameter if not already present
 *
 */
parameterList *parameter_list_add_parameter(parameterList *list, char *name, char *value);

/**
 * @brief Appends tail to head and returns a pointer to the result.
 *
 * @param head A parameter list.
 * @param tail A parameter list.
 */
parameterList *parameter_lists_merge(parameterList *head, parameterList *tail);

/**
 * @brief Copies a parameter list. Every inner field is copied.
 *
 * @param source the list to copy.
 * @return parameterList* A copy of source.
 */
parameterList *parameter_list_copy(parameterList *source);

/**
 * @brief Get the value associated with name within a list of parameters. Exits the program if not present.
 *
 * @param list A list of parameters.
 * @param name The name of the field to search.
 * @return char* The value associated with name in list.
 */
char *parameter_list_get_value(parameterList *list, char *name);

/**
 * @brief Frees a list of parameters
 *
 */
void parameter_list_delete(parameterList *list);

/** @brief: the graph type. The first four fields are needed to represent a directed graph. The rest depends on needs. Here, the rest represents initial and final states of an automaton.*/
typedef struct
{
	char *name; ///< The name of the graph/automaton
	int numNodes; ///< The number of nodes of the graph.
	int numEdges; ///< The number of edges of the graph.
	char **nodes; ///< The names of nodes of the graph.
	bool *edges;  ///< The edges of the graph.

	parameterList **parameters; ///< Parameters of the nodes.
	parameterList **edge_parameters; ///< Parameters of the edges
} Graph;

/**
 * @brief Creates a copy of the graph passed in argument.
 *
 * @param graph A graph.
 * @return graph A copy of graph.
 * @pre @p graph must be a valid graph.
 */
Graph graph_copy(Graph graph);

/**
 * @brief Displays a graph with a list of nodes and a matrix of edges.
 *
 * @param graph the graph to display.
 *
 * @pre @p graph must be a valid graph.
 */
void graph_print(Graph graph);

/**
 * @brief Frees all memory occupied by a graph.
 *
 * @param graph The graph to delete.
 *
 * @pre @p graph must be a valid graph.
 */
void graph_delete(Graph graph);

/**
 * @brief Returns the number of nodes of @p graph.
 *
 * @param graph A graph.
 * @return int Its number of nodes.
 *
 * @pre @p graph must be a valid graph.
 */
int graph_num_nodes(Graph graph);

/**
 * @brief Returns the number of edges of @p graph.
 *
 * @param graph A graph.
 * @return int Its number of edges.
 *
 * @pre @p graph must be a valid graph.
 */
int graph_num_edges(Graph graph);

/**
 * @brief Tells if (@p source, @p target) is an edge in @p graph.
 *
 * @param graph A graph.
 * @param source The source of the edge.
 * @param target The target of the edge.
 * @return true If the edge is present in @p graph.
 * @return false Otherwise.
 * @pre @p graph must be a valid graph.
 * @pre 0 <= @p source < @p graph.numNodes
 * @pre 0 <= @p target < @p graph.numNodes
 */
bool graph_is_edge(Graph graph, int source, int target);

/**
 * @brief Returns the parameter list associated to edge (@p source, @p target). Returns NULL if no parameter exists (or the edge doesn't exist).
 *
 * @param graph A graph.
 * @param source The source of the edge.
 * @param target The target of the edge.
 * @pre @p graph must be a valid graph.
 * @pre 0 <= @p source < @p graph.numNodes
 * @pre 0 <= @p target < @p graph.numNodes
 */
parameterList* graph_get_edge_parameter(Graph graph, int source, int target);

/**
 * @brief Returns the name of a node given its identifier.
 *
 * @param graph A graph.
 * @param node A node identifier. Must be lower than graph_num_nodes(@p graph).
 * @return char* The name of @p node.
 * @pre @p graph must be a valid graph.
 * @pre 0 <= @p node < @p graph.numNodes
 */
char *graph_get_node_name(Graph graph, int node);

/**
 * @brief Writes in @p file the content of @p graph (with parameters) in dot format.
 *
 * @param graph A graph.
 * @param file A file.
 * @pre @p file must be valid.
 */
void graph_fill_dot_content(Graph graph, FILE *file);

#endif /* DOT_PARSER_GRAPH_H_ */
