#include "ColouredGraph.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct ColouredGraph_s
{
    Graph graph;  ///< The graph.
    int *colours; ///< The colours associated to each node.
};

ColouredGraph cg_initialize(Graph graph)
{
    ColouredGraph result = (ColouredGraph)malloc(sizeof(*result));
    int num_nodes = graph_num_nodes(graph);
    result->graph = graph;
    result->colours = (int *)malloc(num_nodes * sizeof(int));
    for (int node = 0; node < num_nodes; node++)
        result->colours[node] = -1;
    return result;
}

void cg_print(ColouredGraph graph)
{
    graph_print(graph->graph);
}

void cg_print_colors(ColouredGraph graph)
{
    printf("Colours of each node:\n");
    int num_nodes = graph_num_nodes(graph->graph);
    for (int node = 0; node < num_nodes; node++)
    {
        printf("%s(%d) : %d\n", graph_get_node_name(graph->graph, node), node, graph->colours[node]);
    }
}

void cg_delete(ColouredGraph graph)
{
    free(graph->colours);
    free(graph);
}

int cg_get_num_nodes(ColouredGraph graph)
{
    return graph_num_nodes(graph->graph);
}

int cg_get_num_edges(ColouredGraph graph)
{
    return graph_num_edges(graph->graph);
}

bool cg_is_edge(ColouredGraph graph, int source, int target)
{
    return (graph_is_edge(graph->graph, source, target));
}

char *cg_get_node_name(ColouredGraph graph, int node)
{
    return graph_get_node_name(graph->graph, node);
}

int cg_get_node_colour(ColouredGraph graph, int node)
{
    return graph->colours[node];
}

void cg_set_node_colour(ColouredGraph graph, int node, int colour)
{
    graph->colours[node] = colour;
}

void cg_create_dot(ColouredGraph graph, char *name)
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
        fprintf(file, "graph Sol{\n");
    }
    else
    {
        int length = strlen(name) + 12;
        char nameFile[length];
        snprintf(nameFile, length, "sol/%s.dot", name);
        file = fopen(nameFile, "w");
        fprintf(file, "graph %s{\n", name);
    }

    int num_nodes = graph_num_nodes(graph->graph);

    int num_colours = -1;
    for (int node = 0; node < num_nodes; node++)
    {
        if (num_colours <= graph->colours[node])
            num_colours = graph->colours[node];
    }
    num_colours++;

    char **colours = (char **)malloc(num_colours * sizeof(char *));
    srand(time(NULL));

    for (int col = 0; col < num_colours; col++)
    {
        if (col == 0)
            colours[col] = "green";
        else if (col == 1)
            colours[col] = "red";
        else if (col == 2)
            colours[col] = "blue";
        else if (col == 3)
            colours[col] = "yellow";
        else if (col == 4)
            colours[col] = "purple";
        else
        {
            colours[col] = (char *)malloc(10 * sizeof(char));
            colours[col][9] = '\0';
            colours[col][8] = '\"';
            colours[col][1] = '#';
            colours[col][0] = '\"';
            for (int digit = 2; digit < 8; digit++)
            {
                int r = rand() % 16;
                if (r < 10)
                    colours[col][digit] = '0' + r;
                else
                    colours[col][digit] = 'a' + r - 10;
            }
        }
    }

    for (int node = 0; node < num_nodes; node++)
    {
        fprintf(file, "%s", graph_get_node_name(graph->graph, node));
        if (graph->colours[node] >= 0)
        {
            fprintf(file, "[style=filled,fillcolor=%s]", colours[graph->colours[node]]);
        }
        fprintf(file, ";\n");
    }

    for (int node = 0; node < num_nodes; node++)
    {
        for (int node2 = 0; node2 < node; node2++)
        {
            if (graph_is_edge(graph->graph, node, node2))
            {
                fprintf(file, "%s -- %s", graph_get_node_name(graph->graph, node), graph_get_node_name(graph->graph, node2));
                fprintf(file, ";\n");
            }
        }
    }

    fprintf(file, "}\n");

    for (int col = 5; col < num_colours; col++)
        free(colours[col]);
    free(colours);

    fclose(file);
}