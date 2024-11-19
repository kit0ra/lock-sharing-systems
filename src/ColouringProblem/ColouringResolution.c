#include "ColouringResolution.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Recursive implementation of a brute force. Performs a depth-first search of a colouring, and prunes branches as soon as an inconsistency is detected. As such, if a full colouring is reached, it is a correct one.
 * Puts the colouring inside @p graph if a valid one exists, otherwise sets all colours to -1.
 * 
 * @param graph A ColouredGraph.
 * @param num_colours The expected number of colours.
 * @param node The node we are trying to colour.
 * @return true If there exist a colouring starting with the partial colouring given.
 * @return false Otherwise.
 * @pre All nodes smaller than @p node are already coloured without contradiction.
 */
bool recursive_bf(ColouredGraph graph, int num_colours, int node)
{
    int num_nodes = cg_get_num_nodes(graph);
    if (node == num_nodes)
        return true;
    for (int col = 0; col < num_colours; col++)
    {
        cg_set_node_colour(graph, node, col);
        bool same_colour_as_neighbour = false;
        for (int n = 0; n < node; n++)
        {
            if (!cg_is_edge(graph, node, n))
                continue;
            int col_n = cg_get_node_colour(graph, n);
            if (col_n == col)
            {
                same_colour_as_neighbour = true;
                break;
            }
        }
        if (same_colour_as_neighbour)
            continue;
        bool res = recursive_bf(graph, num_colours, node + 1);
        if (res)
            return true;
        if (node == 0)
            return false;
    }
    cg_set_node_colour(graph, node, -1);
    return false;
}

bool colouring_brute_force(ColouredGraph graph, int num_colours)
{
    return recursive_bf(graph, num_colours, 0);
}