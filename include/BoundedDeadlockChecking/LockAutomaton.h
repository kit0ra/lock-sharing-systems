/**
 * @file LockAutomaton.h
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to represent an automaton with lock.
 * @version 1
 * @date 2024-10-01
 *
 * @copyright Creative Commons.
 *
 */

#ifndef COCA_LOCK_AUTOMATON_H
#define COCA_LOCK_AUTOMATON_H

#include <stdbool.h>
#include "Graph.h"

/**
 * @brief The struct containing the automaton (an oriented graph, initial states information, and transition labels).
 */
typedef struct LockAutomaton_s *LockAutomaton;

/**
 * @brief Structure to store a step of an execution path over several automata.
 *
 */
typedef struct step
{
    int automaton; //< The automaton number advancing at this step.
    int source;    //< The node number source of this step in the above automaton.
    int target;    //< The node number target of this step in the above automaton.
    int action;    //< The action code of this step.
} step;

/**
 * @brief Initializes a LockAutomaton from a Graph for use in the project. Parses the node parameters to determine which are initial, and the edges parameters to determine what is their action.
 * The graph is NOT copied (it is not supposed to be modified).
 * Edge parameters parsed are "xlabel". Values of the form "acq(%d)" will take the lock numbered. Values of the form "rel(%d)" will release the lock numbered. Any other value (or missing values) will be treated as noop. Warning, lock must be strictly positive integers.
 * The initial node is determined by a node having the "shape" parameter. Only one initial state can exist. If several nodes have the "shape" parameter defined, only the last one (in parsing order) will be considered as initial (here, automata are expected to have only one initial node).
 *
 * @param graph The Graph that is the input of the problem.
 * @return LockAutomaton The structure LockAutomaton described above.
 */
LockAutomaton la_initialize(Graph graph);

/**
 * @brief Deallocates memory used by @a automaton (initialized by la_initialize). Does NOT deallocates the graph.
 *
 * @param automaton
 */
void la_delete(LockAutomaton automaton);

/**
 * @brief Prints an action code as its textual representation.
 *
 * @param action
 */
void la_print_action(int action);

/**
 * @brief Printer function to display information about @p automaton.
 *
 * @param automaton
 */
void la_print(LockAutomaton automaton);

/**
 * @brief Returns the number of nodes of @p automaton.
 *
 * @param automaton
 * @return int
 */
int la_get_num_nodes(LockAutomaton automaton);

/**
 * @brief Returns the number of edges of @p automaton
 *
 * @param automaton
 * @return int
 */
int la_get_num_edges(LockAutomaton automaton);

/**
 * @brief Returns true if (@p source, @p target) is an edge in @p automaton.
 *
 * @pre @p source and @p target must be between 0 and la_get_num_nodes(@p automaton).
 * @param automaton
 * @param source
 * @param target
 * @return true
 * @return false
 */
bool la_is_edge(LockAutomaton automaton, int source, int target);

/**
 * @brief Returns the name of @p node in @p automaton.
 *
 * @pre @p node must be between 0 and la_get_num_nodes(@p automaton)
 * @param automaton
 * @param node
 * @return char*
 */
char *la_get_node_name(LockAutomaton automaton, int node);

/**
 * @brief Returns the action associated with edge (@p source,@p target) in @p automaton, if it exists (otherwise, exits the program).
 *
 * @pre @p source and @p target must be between 0 and la_get_num_nodes(@p automaton).
 * @pre la_is_edge(automaton,source,target) must return true.
 * @param automaton
 * @param source
 * @param target
 * @return int
 */
int la_get_edge_action(LockAutomaton automaton, int source, int target);

/**
 * @brief Returns true if @p node is initial in @p automaton, and false otherwise.
 *
 * @pre @p node must be between 0 and la_get_num_nodes(@p automaton).
 * @param automaton
 * @param node
 * @return true
 * @return false
 */
bool la_is_initial(LockAutomaton automaton, int node);

/**
 * @brief Gets the initial node of @p automaton.
 *
 * @param automaton
 * @return int
 */
int la_get_initial(LockAutomaton automaton);

/**
 * @brief Gets the biggest lock used by @p automaton.
 *
 * @param automaton
 * @return int
 */
int la_get_max_lock(LockAutomaton automaton);

/**
 * @brief Gets the name of the automaton
 *
 * @param automaton
 * @return int
 */
char *la_get_name(LockAutomaton automaton);

/**
 * @brief Prints the path @p path.
 *
 * @param automata
 * @param num_automata
 * @param path
 * @param size_path
 */
void la_print_path(LockAutomaton *automata, int num_automata, step *path, int size_path);

/**
 * @brief Generates a dot file reprensenting all the automata in @p automata, along with the path described by @p path (in red). The file will have name <@p name>.dot
 * 
 * @param automata 
 * @param num_automata 
 * @param path 
 * @param size_path 
 * @param name 
 */
void la_create_dot(LockAutomaton *automata, int num_automata, step *path, int size_path, char *name);

/**
 * @brief Creates a dummy step with all values set to 0.
 * 
 * @return step 
 */
step la_step_empty();

/**
 * @brief Creates a step with values given in argument (name matches definition).
 * 
 * @param automaton 
 * @param source 
 * @param target 
 * @param action 
 * @return step 
 */
step la_step_create(int automaton, int source, int target, int action);
#endif