#include "ColouringReduction.h"
#include "Z3Tools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @brief Creates a formula containing only the variable representing that node @p node has color @p color.
 * 
 * @param ctx The solver context.
 * @param node A node.
 * @param color A color.
 * @return Z3_ast 
 */
Z3_ast variable_node_color(Z3_context ctx, int node, int color)
{
    char name[40];
    snprintf(name, 40, "node %d, color %d", node, color);
    return mk_bool_var(ctx, name);
}

/**
 * @brief Creates the formula stating that the edge (@p node1,@p node2) has its ends of different colours.
 * 
 * @param ctx The solver context.
 * @param node1 A node.
 * @param node2 A node.
 * @param num_colours The expected number of colours.
 * @return Z3_ast The formula.
 */
Z3_ast edge_formula(Z3_context ctx, int node1, int node2, int num_colours)
{
    Z3_ast edge_diff[num_colours];
    for (int colour = 0; colour < num_colours; colour++)
    {
        Z3_ast col_diff[2];
        col_diff[0] = Z3_mk_not(ctx, variable_node_color(ctx, node1, colour));
        col_diff[1] = Z3_mk_not(ctx, variable_node_color(ctx, node2, colour));
        edge_diff[colour] = Z3_mk_or(ctx, 2, col_diff);
    }
    return Z3_mk_and(ctx, num_colours, edge_diff);
}

/**
 * @brief Creates the formula stating that all edges have their ends of different colours.
 * 
 * @param ctx The solver context.
 * @param graph A ColouredGraph.
 * @param num_colours The expected number of colours.
 * @return Z3_ast The formula.
 */
Z3_ast edges_have_different_colours_formula(Z3_context ctx, const ColouredGraph graph, int num_colours)
{
    int num_nodes = cg_get_num_nodes(graph);
    int current = 0;
    Z3_ast edges_formula[num_nodes * num_nodes];
    for (int node1 = 0; node1 < num_nodes; node1++)
    {
        for (int node2 = node1 + 1; node2 < num_nodes; node2++)
        {
            if (!cg_is_edge(graph, node1, node2))
                continue;
            edges_formula[current] = edge_formula(ctx, node1, node2, num_colours);
            current++;
        }
    }
    return Z3_mk_and(ctx, current, edges_formula);
}

/**
 * @brief Creates a formula stating that every node has exactly one colour.
 * 
 * @param ctx The solver context.
 * @param num_nodes The number of nodes.
 * @param num_colours The expected number of colours.
 * @return Z3_ast The formula.
 */
Z3_ast each_node_has_one_colour_formula(Z3_context ctx, int num_nodes, int num_colours)
{

    Z3_ast nodes_coloured[num_nodes];
    for (int node = 0; node < num_nodes; node++)
    {
        Z3_ast node_color_vars[num_colours];
        for (int colour = 0; colour < num_colours; colour++)
        {
            node_color_vars[colour] = variable_node_color(ctx, node, colour);
        }
        nodes_coloured[node] = uniqueFormula(ctx, node_color_vars, num_colours);
    }
    return Z3_mk_and(ctx, num_nodes, nodes_coloured);
}

Z3_ast colouring_reduction(Z3_context ctx, const ColouredGraph graph, int num_colours)
{
    int num_nodes = cg_get_num_nodes(graph);
    Z3_ast result[2];
    result[0] = edges_have_different_colours_formula(ctx, graph, num_colours);
    result[1] = each_node_has_one_colour_formula(ctx, num_nodes, num_colours);
    return Z3_mk_and(ctx, 2, result);
}

void colour_graph_from_model(Z3_context ctx, Z3_model model, ColouredGraph graph, int num_colours)
{
    int num_nodes = cg_get_num_nodes(graph);
    for (int node = 0; node < num_nodes; node++)
    {
        for (int colour = 0; colour < num_colours; colour++)
        {
            if (value_of_var_in_model(ctx, model, variable_node_color(ctx, node, colour)))
            {
                cg_set_node_colour(graph, node, colour);
                break;
            }
        }
    }
}

void colouring_print_model(Z3_context ctx, Z3_model model, ColouredGraph graph, int num_colours)
{
    int num_nodes = cg_get_num_nodes(graph);
    for (int node = 0; node < num_nodes; node++)
        for (int colour = 0; colour < num_colours; colour++)
            printf("[%d:%d] = %d\n", node, colour, value_of_var_in_model(ctx, model, variable_node_color(ctx, node, colour)));
}