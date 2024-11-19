#include <stdio.h>
#include <Graph.h>
#include <Parsing.h>
#include <LockAutomaton.h>

void usage()
{
    printf("Usage: graphParser file\n");
    printf(" Displays information on the graph discribed by file, seen as a repartition graph. It should be provided with a .dot file\n");
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        usage();
        return 0;
    }

    Graph graph;
    graph = get_graph_from_file(argv[1]);

    LockAutomaton automaton = la_initialize(graph);

    la_print(automaton);

    printf("The automaton has %d nodes\n", la_get_num_nodes(automaton));

    printf("The automaton has %d edges\n", la_get_num_edges(automaton));

    printf("Node %s is ", la_get_node_name(automaton, 0));
    if (!la_is_initial(automaton, 0))
        printf("not ");
    printf("initial\n");

    printf("There is ");
    if (!la_is_edge(automaton, 0, 1))
        printf("no ");
    printf("edge between nodes %s and %s\n", la_get_node_name(automaton, 0), la_get_node_name(automaton, 1));

    if (la_is_edge(automaton, 0, 1))
    {
        printf("The action between node %s and %s is ", la_get_node_name(automaton, 0), la_get_node_name(automaton, 1));
        la_print_action(la_get_edge_action(automaton, 0, 1));
        printf("\n");
    }

    la_delete(automaton);

    graph_delete(graph);
    printf("Graph successfully deleted.\n");
    return 0;
}