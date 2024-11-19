%{
/**
 * @file Parser.l
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  Parser for a graphviz parser, intended to serve for a master's project at University of Bordeaux. Adapted from gvizparse by Nikolaos Kavvadias (https://github.com/nkkav/gvizparse v1.0.0).
 *         Convert a .dot file into a GraphList structure.
           Does not support subgraphs for now.
           This version supports automata with custom syntax (nodes are declared initial (resp. final) with an option of the form "[initial=N]" (resp. "[final=N]"), with N standing for any string), and stores the color of the node (if any).
 * @version 2
 * @date 2020-06-24
 * 
 * @copyright Creative Commons.
 * 
 */
 
#include "GraphList.h"
#include "Parser.h"
#include "Lexer.h"
#include "Graph.h"

int yyerror(GraphList *expression, yyscan_t scanner, const char *msg) {
    /* Add error handling routine as needed */
    printf("Erreur: %s\n",msg);
    return 0;
}
 
%}

%code requires {
  typedef void* yyscan_t;
  typedef struct {
      parameterList* parameters;
  } parameterInformation;
}

/* for normal Make.
%output  "Parser.c"
%defines "Parser.h"
*/
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { GraphList *graph }
%parse-param { yyscan_t scanner }


%union {
    char* name;
    parameterInformation parameterInfo;
}


%token T_LPAREN
%token T_RPAREN
%token T_COMMA
%token T_COLON
%token T_SEMI
%token T_AT
%token T_LBRACKET
%token T_RBRACKET
%token T_LBRACE
%token T_RBRACE
%token <name> T_STRING
%token T_EQ
%token T_DIGRAPH
%token T_EDGE
%token T_DEDGE
%token T_UEDGE
%token T_GRAPH
%token <name> T_ID
%token T_NODE
%token T_STRICT
%token T_SUBGRAPH


/*declare non-terminal symbols here.*/
//%type <expression> edgeDescription

%type <name> node_id;
%type <name> edgerhs;
%type <parameterInfo> attr_assignment;
%type <parameterInfo> a_list;
%type <parameterInfo> attr_list;
%type <name> idrhs;



 
%%
 
input : strict graph_type idrhs T_LBRACE stmt_list T_RBRACE {graph->name = $3;}
    ;

strict : /* empty */ 
    | T_STRICT
    ;

graph_type : T_DIGRAPH  { graph->directed = true;}
    | T_GRAPH           { graph->directed = false;}
    ;

stmt_list	:	stmt_list1
    | /* empty */
		;

stmt_list1 :	stmt
    | stmt_list1 stmt
		;
stmt :	stmt1
		|	stmt1 T_SEMI
		;

stmt1 : attr_stmt
    | node_stmt
    | edge_stmt
    | subgraph
    | attr_assignment
    ;

attr_stmt : T_GRAPH attr_list
    | T_NODE attr_list
    | T_EDGE attr_list
    ;

attr_list : T_LBRACKET a_list T_RBRACKET        { $$ = $2; }
    | T_LBRACKET T_RBRACKET                     { $$.parameters=NULL;}
    | T_LBRACKET a_list T_RBRACKET attr_list    { 
                                                $$.parameters = parameter_lists_merge($2.parameters,$4.parameters);
                                                }
    | T_LBRACKET T_RBRACKET attr_list           { $$ = $3; }
    ;

a_list : attr_assignment                { $$ = $1;}
    | attr_assignment T_COMMA a_list    { 
                                            $$.parameters = parameter_lists_merge($1.parameters,$3.parameters);
                                          }
    | attr_assignment a_list            { 
                                            $$.parameters = parameter_lists_merge($1.parameters,$2.parameters);
                                          }
    ;

attr_assignment : idrhs T_EQ idrhs   { 
      $$.parameters = parameter_list_add_parameter(NULL,$1,$3);
             free($1); free($3);}
    ;
								
idrhs : T_ID        { $$ = (char*)malloc((strlen($1)+1)*sizeof(char)); strcpy($$,$1);
                    }
    | T_STRING      { $$ = (char*)malloc((strlen($1)+1)*sizeof(char)); strcpy($$,$1);
                    }
		;        

node_stmt : node_id         { free($1); }
    | node_id attr_list     {   
                                add_parameters_to_node($1,$2.parameters,graph->nodes);
                                free($1);
                            }
    ;

node_id : T_ID      { 
                      $$ = (char*)malloc((strlen($1)+1)*sizeof(char)); strcpy($$,$1);
                      if(graph->nodes == NULL) graph->nodes = addNode($1,NULL); else addOrUpdateNode($1,graph->nodes);
                    }
    | T_ID port     { 
                      $$ = (char*)malloc((strlen($1)+1)*sizeof(char)); strcpy($$,$1);
                      if(graph->nodes == NULL) graph->nodes = addNode($1,NULL); else addOrUpdateNode($1,graph->nodes);
                    }
    ;

port : port_location 
    | port_angle 
    | port_location port_angle 
    | port_angle port_location 
    ;

port_location : T_COLON T_ID
    | T_COLON T_ID T_LPAREN T_ID T_COMMA T_ID T_RPAREN
    ;

port_angle : T_AT T_ID
    ;

edge_stmt : node_id edgerhs         { //printf("edge seen: (%s,%s)\n",$1,$2);
                                      graph->edges = addEdge($1,$2,graph->edges,NULL);
                                      free($1); free($2);
                                    }
    | node_id edgerhs attr_list     { //printf("edge seen: (%s,%s)\n",$1,$2);
                                      graph->edges = addEdge($1,$2,graph->edges,$3.parameters);
                                      free($1); free($2);
                                    }
    | subgraph edgerhs 
    | subgraph edgerhs attr_list 
    ;

edgerhs : edgeop node_id        { //printf("edge end seen\n");
                                  $$ = $2;
                                }
    | edgeop node_id edgerhs    {
                                  graph->edges = addEdge($2,$3,graph->edges,NULL);
                                  free($3);
                                  $$ = $2;
                                }
    ;

subgraph : T_SUBGRAPH T_ID T_LBRACE stmt_list T_RBRACE
    | T_SUBGRAPH T_LBRACE stmt_list T_RBRACE
    | T_LBRACE stmt_list T_RBRACE
    ;

edgeop   : T_UEDGE
    | T_DEDGE
    ;

%%

#include <stdio.h>
#include <string.h>    /* For strcmp, strlen */
#include <sys/types.h> /* Needed in print_out */
#include <sys/stat.h>  /* Needed in print_out */
#include <fcntl.h>
#include <errno.h>

extern int column;
//extern FILE *yyin; //remove for version 3.0.4 and g++ v6.3.0

/*int yyerror(char *s)
{
  fflush(stdout);
  printf("\nErreur: %s\n", s);
  
  return 0;
}*/