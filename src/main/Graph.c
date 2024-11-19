/*
 * @file Graph.c
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to store a graph statically, and to access to its informations easily.
		   Includes unary automata informations (initial and final nodes).
		   Does not contain acces functions for now.
 * @version 0.5
 * @date 2018-11-18, 2019-07-23
 *
 * @copyright Creative Commons.
 *
 */

#include "Graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

parameterList *parameter_list_add_parameter(parameterList *list, char *name, char *value)
{
	if (list == NULL)
	{
		list = (parameterList *)malloc(sizeof(parameterList));
		list->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
		strcpy(list->name, name);
		list->value = (char *)malloc((strlen(value) + 1) * sizeof(char));
		strcpy(list->value, value);
		list->next = NULL;
		return list;
	}
	else if (strcmp(name, list->name) != 0)
		list->next = parameter_list_add_parameter(list->next, name, value);
	return list;
}

parameterList *parameter_lists_merge(parameterList *head, parameterList *tail)
{
	if (head == NULL)
		return tail;
	if (head->next != NULL)
	{
		parameter_lists_merge(head->next, tail);
		return head;
	}
	head->next = tail;
	return head;
}

parameterList *parameter_list_copy(parameterList *source)
{
	if (source == NULL)
		return NULL;
	parameterList *result = (parameterList *)malloc(sizeof(parameterList));
	result->name = (char *)malloc((strlen(source->name) + 1) * sizeof(char));
	strcpy(result->name, source->name);
	result->value = (char *)malloc((strlen(source->value) + 1) * sizeof(char));
	strcpy(result->value, source->value);
	result->next = parameter_list_copy(source->next);
	return result;
}

char *parameter_list_get_value(parameterList *list, char *name)
{
	while (list != NULL && strcmp(list->name, name) != 0)
		list = list->next;
	if (list == NULL)
	{
		return NULL;
		/* printf("Parameter list does not contain required field %s\n", name);
		exit(-1); */
	}
	return list->value;
}

void parameter_list_delete(parameterList *list)
{
	if (list == NULL)
		return;
	parameter_list_delete(list->next);
	free(list->name);
	free(list->value);
	free(list);
}

void graph_print(Graph graph)
{
	printf("\nName: %s\n", graph.name);
	printf("\nNodes:\n");
	for (int i = 0; i < graph.numNodes; i++)
		printf("%d : %s , ", i, graph.nodes[i]);

	printf("\nEdges:\n");
	for (int i = 0; i < graph.numNodes; i++)
	{
		for (int j = 0; j < graph.numNodes; j++)
		{
			printf("%d ", graph.edges[i * graph.numNodes + j]);
		}
		printf("\n");
	}

	// parameters

	printf("\nParameters:\n");
	for (int i = 0; i < graph.numNodes; i++)
	{
		printf("node %s:", graph.nodes[i]);
		parameterList *list = graph.parameters[i];
		while (list != NULL)
		{
			printf("(%s : %s), ", list->name, list->value);
			list = list->next;
		}
		printf("\n");
	}
}

Graph graph_copy(Graph graph)
{
	Graph copy;
	copy.name = graph.name;
	copy.numNodes = graph.numNodes;
	copy.numEdges = graph.numEdges;
	copy.nodes = (char **)malloc(copy.numNodes * sizeof(char *));
	copy.edges = (bool *)malloc(copy.numNodes * copy.numNodes * sizeof(bool));

	for (int i = 0; i < copy.numNodes * copy.numNodes; i++)
		copy.edges[i] = graph.edges[i];

	copy.parameters = (parameterList **)malloc(graph.numNodes * sizeof(parameterList *));
	for (int i = 0; i < graph.numNodes; i++)
		copy.parameters[i] = parameter_list_copy(graph.parameters[i]);

	copy.edge_parameters = (parameterList **)malloc(graph.numNodes * graph.numNodes * sizeof(parameterList *));
	for (int i = 0; i < graph.numNodes * graph.numNodes; i++)
		copy.edge_parameters[i] = parameter_list_copy(graph.edge_parameters[i]);

	return copy;
}

void graph_delete(Graph graph)
{
	if (graph.edges != NULL)
		free(graph.edges);
	if (graph.nodes != NULL)
	{
		for (int i = 0; i < graph.numNodes; i++)
		{
			if (graph.nodes[i] != NULL)
				free(graph.nodes[i]);
		}
		free(graph.nodes);
	}
	// Pour les automates.

	for (int i = 0; i < graph.numNodes; i++)
		parameter_list_delete(graph.parameters[i]);
	free(graph.parameters);

	for (int i = 0; i < graph.numNodes * graph.numNodes; i++)
		parameter_list_delete(graph.edge_parameters[i]);
	free(graph.edge_parameters);

	graph.numEdges = 0;
	graph.numNodes = 0;
	free(graph.name);
}

int graph_num_nodes(Graph graph)
{
	return graph.numNodes;
}

int graph_num_edges(Graph graph)
{
	return graph.numEdges;
}

bool graph_is_edge(Graph graph, int source, int target)
{
	return graph.edges[(source * graph.numNodes) + target];
}

parameterList *graph_get_edge_parameter(Graph graph, int source, int target)
{
	return graph.edge_parameters[(source * graph.numNodes) + target];
}

char *graph_get_node_name(Graph graph, int node)
{
	return graph.nodes[node];
}

void graph_fill_dot_content(Graph graph, FILE *file)
{
	int num_nodes = graph.numNodes;
	for (int node = 0; node < num_nodes; node++)
	{
		fprintf(file, "%s", graph_get_node_name(graph, node));
		if (graph.parameters[node] != NULL)
		{
			fprintf(file, "[");
			parameterList *param = graph.parameters[node];
			while (true)
			{
				fprintf(file, "%s=%s", param->name, param->value);
				param = param->next;
				if (param == NULL)
					break;
				fprintf(file, ",");
			}
			fprintf(file, "]");
		}
		fprintf(file, ";\n");
	}
	for (int node = 0; node < num_nodes; node++)
	{
		for (int node2 = 0; node2 < node; node2++)
		{
			if (graph_is_edge(graph, node, node2))
			{
				fprintf(file, "%s -- %s", graph_get_node_name(graph, node), graph_get_node_name(graph, node2));
				fprintf(file, ";\n");
			}
		}
	}
}