#include "GraphListToGraph.h"
#include "EdgeList.h"
#include "NodeList.h"
#include <stdlib.h>
#include <string.h>

/*
 * @brief Auxilary function to determine the index of a node name.
 *
 * @param list the array of node to search in.
 * @param size the size of list.
 * @param target the node to search
 * @return int the index of target in list.
 */
int findNode(char **list, int size, char *target)
{
	for (int i = 0; i < size; i++)
	{
		if (strcmp(list[i], target) == 0)
			return i;
	}
	return -1;
}

Graph createGraph(GraphList source)
{
	Graph res;
	res.name = source.name;
	res.numNodes = 0;
	res.numEdges = 0;
	SNodeList *explore = source.nodes;

	int count = 0;
	while (explore != NULL)
	{
		count++;
		explore = explore->next;
	}
	res.numNodes = count;

	// printf("nodes: %d\n",count);

	res.edges = (bool *)malloc(res.numNodes * res.numNodes * sizeof(bool));
	res.nodes = (char **)malloc(res.numNodes * sizeof(char *));

	count = 0;
	explore = source.nodes;

	// Paramètres

	res.parameters = (parameterList **)malloc(res.numNodes * sizeof(parameterList *));
	res.edge_parameters = (parameterList **)malloc(res.numNodes * res.numNodes * sizeof(parameterList *));
	for (int i = 0; i < res.numNodes * res.numNodes; i++)
		res.edge_parameters[i] = NULL;

	while (explore != NULL)
	{
		res.nodes[count] = (char *)malloc((strlen(explore->node) + 1) * sizeof(char));
		strcpy(res.nodes[count], explore->node);

		// Paramètres

		res.parameters[count] = parameter_list_copy(explore->parameters);

		count++;
		explore = explore->next;
	}

	for (int i = 0; i < res.numNodes; i++)
		for (int j = 0; j < res.numNodes; j++)
			res.edges[i * res.numNodes + j] = false;

	SEdgeList *exploreBis = source.edges;
	while (exploreBis != NULL)
	{
		int n1, n2;
		n1 = findNode(res.nodes, res.numNodes, exploreBis->node1);
		n2 = findNode(res.nodes, res.numNodes, exploreBis->node2);
		res.edges[n1 * res.numNodes + n2] = true;
		res.edge_parameters[n1 * res.numNodes + n2] = parameter_list_copy(exploreBis->parameters);
		if (!source.directed)
		{
			res.edges[n2 * res.numNodes + n1] = true;
			res.edge_parameters[n2 * res.numNodes + n1] = parameter_list_copy(exploreBis->parameters);
		}
		exploreBis = exploreBis->next;
		res.numEdges++;
	}

	return res;
}