#include "LockAutomaton.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

struct LockAutomaton_s
{
    Graph graph;       ///< The graph supporting the automaton.
    int initial;       ///< The initial node of the automaton.
    int *edge_actions; ///< The action associated to each edge (positive for acquiring a lock, negative for releasing it, 0 for noop).
    int max_lock;      ///< The maximum lock number appearing in this automaton;
};

int la_get_edge_pos(LockAutomaton automaton, int source, int target)
{
    return source * graph_num_nodes(automaton->graph) + target;
}

LockAutomaton la_initialize(Graph graph)
{
    LockAutomaton result = (LockAutomaton)malloc(sizeof(*result));
    result->graph = graph;
    int num_nodes = graph_num_nodes(graph);
    result->edge_actions = (int *)malloc(num_nodes * num_nodes * sizeof(int));
    result->max_lock = 0;
    for (int node = 0; node < num_nodes; node++)
    {
        char *param = parameter_list_get_value(graph.parameters[node], "shape");
        if (param != NULL)
            result->initial = node;
    }
    for (int source = 0; source < num_nodes; source++)
        for (int target = 0; target < num_nodes; target++)
        {
            parameterList *params = graph_get_edge_parameter(graph, source, target);
            char *action = parameter_list_get_value(params, "xlabel");
            int val = 0;
            if (action != NULL)
            {
                int res;
                res = sscanf(action, "\"acq(%d)\"", &val);
                if (res == 0)
                {
                    res = sscanf(action, "\"rel(%d)\"", &val);
                    val = -val;
                    if (res == 0)
                        val = 0;
                }
            }
            result->edge_actions[source * num_nodes + target] = val;
            int lock = abs(val);
            if (result->max_lock < lock)
                result->max_lock = lock;
        }
    return result;
}

void la_delete(LockAutomaton automaton)
{
    free(automaton->edge_actions);
    free(automaton);
}

void la_print_action(int action)
{
    if (action == 0)
        printf("noop");
    else if (action > 0)
        printf("acq(%d)", action);
    else
        printf("rel(%d)", -action);
}

void la_print(LockAutomaton automaton)
{
    int numNodes = graph_num_nodes(automaton->graph);
    graph_print(automaton->graph);

    printf("\n\nInitial node :\n");
    printf("%d (%s) ", automaton->initial, automaton->graph.nodes[automaton->initial]);

    printf("\n\nEdges and actions:\n");
    for (int source = 0; source < numNodes; source++)
        for (int target = 0; target < numNodes; target++)
            if (graph_is_edge(automaton->graph, source, target))
            {
                printf("(%d -> %d) : ", source, target);
                la_print_action(automaton->edge_actions[source * numNodes + target]);
                printf("\n");
            }

    printf("Max lock used: %d\n", automaton->max_lock);
}

int la_get_num_nodes(LockAutomaton automaton)
{
    return graph_num_nodes(automaton->graph);
}

int la_get_num_edges(LockAutomaton automaton)
{
    return graph_num_edges(automaton->graph);
}

bool la_is_edge(LockAutomaton automaton, int source, int target)
{
    return (graph_is_edge(automaton->graph, source, target));
}

char *la_get_node_name(LockAutomaton automaton, int node)
{
    return graph_get_node_name(automaton->graph, node);
}

int la_get_edge_action(LockAutomaton automaton, int source, int target)
{
    assert(la_is_edge(automaton, source, target));
    return automaton->edge_actions[la_get_edge_pos(automaton, source, target)];
}

bool la_is_initial(LockAutomaton automaton, int node)
{
    return automaton->initial == node;
}

int la_get_initial(LockAutomaton automaton)
{
    return automaton->initial;
}

int la_get_max_lock(LockAutomaton automaton)
{
    return automaton->max_lock;
}

char *la_get_name(LockAutomaton automaton)
{
    return automaton->graph.name;
}

void la_print_path(LockAutomaton *automata, int num_automata, step *path, int size_path)
{
    for (int i = 0; i < size_path; i++)
    {
        printf("Step %i:\n %s : %i -> %i (", i + 1, la_get_name(automata[path[i].automaton]), path[i].source, path[i].target);
        la_print_action(path[i].action);
        printf(")\n");
    }
}

void la_create_dot(LockAutomaton *automata, int num_automata, step *path, int size_path, char *name)
{

    FILE *file;

    struct stat st = {0};
    if (stat("./sol", &st) == -1)
        mkdir("./sol", 0777);

    if (name == NULL)
    {
        char nameFile[30];
        snprintf(nameFile, 30, "sol/result.dot");
        file = fopen(nameFile, "w");
        fprintf(file, "digraph Sol{\n");
    }
    else
    {
        int length = strlen(name) + 12;
        char nameFile[length];
        snprintf(nameFile, length, "sol/%s.dot", name);
        file = fopen(nameFile, "w");
        fprintf(file, "digraph %s{\n", name);
    }

    for (int aut = 0; aut < num_automata; aut++)
    {
        int num_nodes = la_get_num_nodes(automata[aut]);

        fprintf(file, "%s__%s[shape=rectangle];\n", la_get_name(automata[aut]), la_get_node_name(automata[aut], la_get_initial(automata[aut])));

        for (int source = 0; source < num_nodes; source++)
        {
            for (int target = 0; target < num_nodes; target++)
            {
                if (!la_is_edge(automata[aut], source, target))
                    continue;
                fprintf(file, "%s__%s -> %s__%s[xlabel=", la_get_name(automata[aut]), la_get_node_name(automata[aut], source), la_get_name(automata[aut]), la_get_node_name(automata[aut], target));
                int action = la_get_edge_action(automata[aut], source, target);
                if (action == 0)
                    fprintf(file, "noop];\n");
                else if (action > 0)
                    fprintf(file, "\"acq(%d)\"];\n", action);
                else
                    fprintf(file, "\"rel(%d)\"];\n", -action);
            }
        }
    }

    for (int step = 0; step < size_path; step++)
    {
        fprintf(file, "%s__%s -> %s__%s [label=\"%d\",color=red, fontcolor=red];\n", la_get_name(automata[path[step].automaton]), la_get_node_name(automata[path[step].automaton], path[step].source), la_get_name(automata[path[step].automaton]), la_get_node_name(automata[path[step].automaton], path[step].target), step);
    }

    fprintf(file, "\n}\n");
    fclose(file);
}

step la_step_create(int automaton, int source, int target, int action)
{

    step result;
    result.automaton = automaton;
    result.source = source;
    result.target = target;
    result.action = action;
    return result;
}

step la_step_empty()
{
    return la_step_create(0, 0, 0, 0);
}