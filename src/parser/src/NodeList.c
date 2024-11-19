/**
 * @file NodeList.c
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Structure to store a list of graph nodes that can be dynamically modified. Used as a temporary structure during parsing before translating into a more static structure.
 *         Includes automata features (initial and final nodes).
 * @version 1
 * @date 2019-07-22
 *
 * @copyright Creative Commons.
 *
 */

#include "NodeList.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates space for node list
 * @return The expression or NULL if not enough memory
 */
static SNodeList *allocateNodeList()
{
    SNodeList *b = (SNodeList *)malloc(sizeof(SNodeList));

    if (b == NULL)
        return NULL;

    b->node = NULL;

    b->next = NULL;

    b->parameters = NULL;

    return b;
}

SNodeList *addNode(char *n1, SNodeList *list)
{
    SNodeList *b = allocateNodeList();

    if (b == NULL)
        return NULL;

    b->node = (char *)malloc((strlen(n1) + 1) * sizeof(char));
    strcpy(b->node, n1);

    b->next = list;

    return b;
}

void addOrUpdateNode(char *n, SNodeList *list)
{
    if (list == NULL)
    {
        return;
    }

    if (strcmp(list->node, n) != 0)
    {
        if (list->next == NULL)
            list->next = addNode(n, NULL);
        else
            addOrUpdateNode(n, list->next);
        return;
    }
    return;
}

void add_parameters_to_node(char *node, parameterList *parameters, SNodeList *list)
{
    if (list == NULL)
        return;
    if (strcmp(node, list->node) != 0)
    {
        add_parameters_to_node(node, parameters, list->next);
        return;
    }
    list->parameters = parameter_lists_merge(list->parameters, parameters);
}

void printNodeList(SNodeList *e)
{
    if (e == NULL)
    {
        printf("\n");
        return;
    }
    printf("%s\n", e->node);
    printNodeList(e->next);
}

void deleteNodeList(SNodeList *b)
{
    if (b == NULL)
        return;

    deleteNodeList(b->next);

    free(b->node);

    parameter_list_delete(b->parameters);

    free(b);
}

/* Testing main.
int main (void){
    SNodeList *toto = NULL;
    toto = addNode("coucou",true,false,toto);
    toto = addNode("voilà",false,false,toto);
    addOrUpdateNode("coucou",false,true,toto);
    addOrUpdateNode("Hey",false,false,toto);
    SNodeList *arg = toto;
    while(arg!=NULL){
        printf("%s,%d,%d\n",arg->node,arg->initial,arg->final);
        arg=arg->next;
    }
    deleteNodeList(toto);
}
*/
