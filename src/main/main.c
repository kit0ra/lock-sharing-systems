/**
 * @file main.c
 * @author Vincent Penelle (vincent.penelle@u-bordeaux.fr)
 * @brief  File putting the whole program together.
 * @version 1
 * @date 2019-07-22
 *
 * @copyright Creative Commons.
 *
 */

#include "Graph.h"
#include "Parsing.h"
#include "Z3Tools.h"
#include "Parser.h"
#ifdef REPARTITION
#include "RepartitionGraph.h"
#include "RepartitionResolution.h"
#include "RepartitionReduction.h"
#endif
#ifdef COLOURING
#include "ColouredGraph.h"
#include "ColouringResolution.h"
#include "ColouringReduction.h"
#endif
#ifdef DEADLOCK_CHECKING
#include "LockAutomaton.h"
#include "DeadlockResolution.h"
#include "DeadlockReduction.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void usage()
{
    printf("Use: graphProblemSolver [options] files\n");
    printf(" files should each contain an input in dot format.\n The program will solve one problem for the inputs.\nIn this version, possible problems are:\n");
#ifdef COLOURING
    printf("- Colouring problem\n");
#endif
#ifdef REPARTITION
    printf("- Equitable Repartition problem.\n");
#endif
#ifdef DEADLOCK_CHECKING
    printf("- Bounded Deadlock Checking of concurrent processes.\n");
#endif
    printf(" Can apply a brute force algorithm or a reduction to SAT.\n Can display the result both on the command line or in dot format.\n For the reduction, can print the formula generated and give the raw model satisfying it (for debugging purposes).\n");
    printf("Options: \n");
    printf(" -h         Displays this help\n");
    printf(" -P PROBLEM Selects the problem to decide on the input. Valid options are");
#ifdef COLOURING
    printf(" \"Colouring\"");
#endif
#ifdef REPARTITION
    printf(" \"Repartition\"");
#endif
#ifdef DEADLOCK_CHECKING
    printf(" \"BoundedDeadlockChecking\"");
#endif
    printf(". If not present or given another string, defaults to Bounded Deadlock Checking Problem.\n");
    printf(" -c VAL     Fixes the value associated with the problem if some value is expected in the problem.");
#ifdef COLOURING
    printf(" Colouring interprets this as the number of colours for the colouring, and defaults to 3 if absent or not a number.");
#endif
#ifdef REPARTITION
    printf(" Repartition expects no value and will ignore this option.");
#endif
#ifdef DEADLOCK_CHECKING
    printf(" Bounded Deadlock Checking interprets this number as the length of the lock desired, and defaults to 10 if absent or not a number.\n");
#endif
    printf(" -v         Activate verbose mode (displays parsed graphs)\n");
    printf(" -B         Solves the problem using the brute force algorithm\n");
    printf(" -R         Solves the problem using a reduction\n");
    printf(" -F         Displays the formula computed ");
#ifdef SUBJECT
    printf("(obviously not in this version)");
#endif
    printf(". Only active if -R is active. Writes it in a file in the folder 'sol' (see option -o)\n");
    printf(" -M         Displays the model of the satisfied formula, to help understanding why it is true, especially when there are variables not representing a part of the solution.\n");
    printf(" -t         Displays the solution found [if not present, only displays the existence of the solution].\n");
    printf(" -f         Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'sol'.\n");
    printf(" -o NAME    Writes the output graph in \"NAME_Brute.dot\" or \"NAME_SAT.dot\" depending of the algorithm used and the formula in \"NAME.formula\". [if not present: \"default_SAT.dot\", \"default_Brute.dot\" and \"default.formula\"]\n");
}

enum problemType
{
    Repartition,
    Colouring,
    LockChecking
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage();
        return 0;
    }

    enum problemType problem = LockChecking;
    bool verbose = false;
    bool displayTerminal = false;
    bool outputFile = false;
    bool printformula = false;
    bool bruteForce = false;
    bool reduction = false;
    bool printModel = false;
    char *problem_parameter = "";
    char *solutionName = "default";
    /*char *realArgs[argc];
    int numArgs = 0;*/

    int option;

    while ((option = getopt(argc, argv, ":hP:c:vFBGRMtfo:")) != -1)
    {
        switch (option)
        {
        case 'h':
            usage();
            return EXIT_SUCCESS;
        case 'P':
            char *pb = optarg;
            if (strcmp(pb, "Colouring") == 0)
                problem = Colouring;
            if (strcmp(pb, "Repartition") == 0)
                problem = Repartition;
            if (strcmp(pb, "BoundedDeadlockChecking") == 0)
                problem = LockChecking;
            break;
        case 'c':
            problem_parameter = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'B':
            bruteForce = true;
            break;
        case 'R':
            reduction = true;
            break;
        case 'F':
            // printf("Don't insist, I'm not showing you the solution of the assignment yet!\n");
            printformula = true;
            break;
        case 'M':
            printModel = true;
            break;
        case 't':
            displayTerminal = true;
            break;
        case 'f':
            outputFile = true;
            break;
        case 'o':
            solutionName = optarg;
            break;
        case '?':
            printf("unknown option: %c\n", optopt);
            break;
        }
    }

    if (argc - optind < 1)
    {
        printf("No argument given. Exiting.\n");
        return 0;
    }

    int num_graphs = argc - optind;
    Graph graphs[argc - optind];
    for (int i = optind; i < argc; i++)
    {
        graphs[i - optind] = get_graph_from_file(argv[i]);
        // graph_print(graphs[i - optind]);
        // printf("\nA\n");
    }

    Graph graph = graphs[0];

#ifdef REPARTITION
    if (problem == Repartition)
    {
        printf("\n*************************************\n*** Equitable Repartition Problem ***\n*************************************\n\n");
        RepartitionGraph rep_graph = rg_initialize(graph);

        if (verbose)
            rg_print(rep_graph);

        if (bruteForce)
        {
            printf("\n*******************\n*** Brute Force ***\n*******************\n\n");
            clock_t start = clock();
            bool res = repartition_brute_force(rep_graph);
            double end = (double)(clock() - start) / CLOCKS_PER_SEC;
            printf("Brute force computed the solution in %g seconds:\n", end);
            if (res)
            {
                printf("There is an equitable repartition.\n");
                if (displayTerminal)
                    rg_print_partition(rep_graph);
                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Brute", solutionName);
                    rg_create_dot(rep_graph, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }
            }
            else
                printf("There is no equitable repartition.\n");
            rg_reinitialize_partition(rep_graph);
        }

        if (reduction)
        {
            printf("\n************************\n*** Reduction to SAT ***\n************************\n\n");

            Z3_context ctx = make_context();

            clock_t start = clock();

            Z3_ast formula;
            formula = repartition_reduction(ctx, rep_graph);

            clock_t timeFormula = clock();

            printf("formula computed in %g seconds\n", (double)(timeFormula - start) / CLOCKS_PER_SEC);

            if (printformula)
            {
#ifndef SUBJECT
                struct stat st = {0};
                if (stat("./sol", &st) == -1)
                    mkdir("./sol", 0777);
                int length = strlen(solutionName) + 13;
                char nameFile[length];
                snprintf(nameFile, length, "sol/%s.formula", solutionName);
                FILE *file = fopen(nameFile, "w");
                fprintf(file, "%s\n", Z3_ast_to_string(ctx, formula));
                fclose(file);
                printf("Formula printed in sol/%s.formula\n", solutionName);
#else
                printf("Nah, I'm not displaying the formula in the given executable\n");
#endif
            }

            Z3_model model;
            Z3_lbool isSat = solve_formula(ctx, formula, &model);

            clock_t timeSat = clock();

            printf("solution computed in %g seconds\n", (double)(timeSat - timeFormula) / CLOCKS_PER_SEC);

            switch (isSat)
            {
            case Z3_L_FALSE:
                printf("No equitable repartition of nodes between players is possible\n");
                break;

            case Z3_L_UNDEF:
                printf("Not able to decide if there is an equitable repartition of nodes between players.\n");
                break;

            case Z3_L_TRUE:
                printf("There is an equitable repartition of nodes between players.\n");

                if (displayTerminal || outputFile)
                    repartition_set_partition_from_model(ctx, model, rep_graph);

                //            if (displayModel)
                //                printModel(ctx, model, biGraph, numComponent);

                if (displayTerminal)
                {
                    rg_print_partition(rep_graph);
                }
                if (printModel)
                    repartition_print_model(ctx, model, rep_graph);

                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Sat", solutionName);
                    rg_create_dot(rep_graph, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }

                break;
            }

            Z3_del_context(ctx);
        }

        rg_delete(rep_graph);
    }
#endif

#ifdef COLOURING
    if (problem == Colouring)
    {

        printf("\n*************************\n*** Colouring Problem ***\n*************************\n\n");

        int num_colours = 3;
        if (strcmp(problem_parameter, "") != 0)
            num_colours = atoi(problem_parameter);

        if (verbose)
            printf("We will try to colour the following graph with %d colours\n", num_colours);

        ColouredGraph coloured_graph = cg_initialize(graph);

        if (verbose)
            cg_print(coloured_graph);

        if (bruteForce)
        {
            printf("\n*******************\n*** Brute Force ***\n*******************\n\n");
            clock_t start = clock();
            bool res = colouring_brute_force(coloured_graph, num_colours);
            double end = (double)(clock() - start) / CLOCKS_PER_SEC;
            printf("Brute force computed the solution in %g seconds:\n", end);
            if (res)
            {
                printf("There is a %d-colouring of this graph.\n", num_colours);
                if (displayTerminal)
                    cg_print_colors(coloured_graph);
                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Brute", solutionName);
                    cg_create_dot(coloured_graph, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }
            }
            else
                printf("There is no %d-colouring of this graph.\n", num_colours);
        }

        if (reduction)
        {
            printf("\n************************\n*** Reduction to SAT ***\n************************\n\n");

            Z3_context ctx = make_context();

            clock_t start = clock();

            Z3_ast formula;
            formula = colouring_reduction(ctx, coloured_graph, num_colours);

            clock_t timeFormula = clock();

            printf("formula computed in %g seconds\n", (double)(timeFormula - start) / CLOCKS_PER_SEC);

            if (printformula)
            {
                struct stat st = {0};
                if (stat("./sol", &st) == -1)
                    mkdir("./sol", 0777);
                int length = strlen(solutionName) + 13;
                char nameFile[length];
                snprintf(nameFile, length, "sol/%s.formula", solutionName);
                FILE *file = fopen(nameFile, "w");
                fprintf(file, "%s\n", Z3_ast_to_string(ctx, formula));
                fclose(file);
                printf("Formula printed in sol/%s.formula\n", solutionName);
            }

            Z3_model model;
            Z3_lbool isSat = solve_formula(ctx, formula, &model);

            clock_t timeSat = clock();

            printf("solution computed in %g seconds\n", (double)(timeSat - timeFormula) / CLOCKS_PER_SEC);

            switch (isSat)
            {
            case Z3_L_FALSE:
                printf("No %d-colouring of this graph is possible\n", num_colours);
                break;

            case Z3_L_UNDEF:
                printf("Not able to decide if there is a %d-colouring of this graph.\n", num_colours);
                break;

            case Z3_L_TRUE:
                printf("There is a %d-colouring of this graph.\n", num_colours);

                if (displayTerminal || outputFile)
                    colour_graph_from_model(ctx, model, coloured_graph, num_colours);

                //            if (displayModel)
                //                printModel(ctx, model, biGraph, numComponent);

                if (displayTerminal)
                {
                    cg_print_colors(coloured_graph);
                }
                if (printModel)
                    colouring_print_model(ctx, model, coloured_graph, num_colours);

                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Sat", solutionName);
                    cg_create_dot(coloured_graph, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }

                break;
            }

            Z3_del_context(ctx);
        }

        cg_delete(coloured_graph);
    }
#endif

#ifdef DEADLOCK_CHECKING
    if (problem == LockChecking)
    {
        printf("\n*****************************************\n*** Bounded Deadlock Checking Problem ***\n*****************************************\n\n");
        LockAutomaton automata[num_graphs];
        for (int i = 0; i < num_graphs; i++)
            automata[i] = la_initialize(graphs[i]);

        if (verbose)
        {
            for (int i = 0; i < num_graphs; i++)
            {
                la_print(automata[i]);
                if (i != num_graphs - 1)
                    printf("\n*****************************************\n*****************************************\n");
            }
        }

        int bound = 10;
        if (strcmp(problem_parameter, "") != 0)
            bound = atoi(problem_parameter);

        step path[bound];
        for (int step = 0; step < bound; step++)
        {
            path[step] = la_step_empty();
        }

        if (bruteForce)
        {
            printf("\n*******************\n*** Brute Force ***\n*******************\n\n");
            clock_t start = clock();
            bool res = deadlock_brute_force(automata, num_graphs, bound, path);
            double end = (double)(clock() - start) / CLOCKS_PER_SEC;
            printf("Brute force computed the solution in %g seconds:\n", end);
            if (res)
            {
                printf("There is a deadlock of size %d.\n", bound);
                if (displayTerminal)
                    la_print_path(automata, num_graphs, path, bound);
                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Brute", solutionName);
                    la_create_dot(automata, num_graphs, path, bound, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }
            }
            else
                printf("There is no deadlock of size %d.\n", bound);
        }

        if (reduction)
        {
            printf("\n************************\n*** Reduction to SAT ***\n************************\n\n");

            Z3_context ctx = make_context();

            clock_t start = clock();

            Z3_ast formula;
            formula = deadlock_reduction(ctx, automata, num_graphs, bound);

            clock_t timeFormula = clock();

            printf("formula computed in %g seconds\n", (double)(timeFormula - start) / CLOCKS_PER_SEC);

            if (printformula)
            {
#ifndef SUBJECT
                struct stat st = {0};
                if (stat("./sol", &st) == -1)
                    mkdir("./sol", 0777);
                int length = strlen(solutionName) + 13;
                char nameFile[length];
                snprintf(nameFile, length, "sol/%s.formula", solutionName);
                FILE *file = fopen(nameFile, "w");
                fprintf(file, "%s\n", Z3_ast_to_string(ctx, formula));
                fclose(file);
                printf("Formula printed in sol/%s.formula\n", solutionName);
#else
                printf("Nah, I'm not displaying the formula in the given executable\n");
#endif
            }

            Z3_model model;
            Z3_lbool isSat = solve_formula(ctx, formula, &model);

            clock_t timeSat = clock();

            printf("solution computed in %g seconds\n", (double)(timeSat - timeFormula) / CLOCKS_PER_SEC);

            switch (isSat)
            {
            case Z3_L_FALSE:
                printf("No deadlock is possible\n");
                break;

            case Z3_L_UNDEF:
                printf("Not able to decide if there is a deadlock.\n");
                break;

            case Z3_L_TRUE:
                printf("There is a deadlock.\n");

                if (!(displayTerminal || outputFile || printModel))
                    break;

                la_path_from_model(ctx, model, automata, num_graphs, path, bound);

                if (displayTerminal)
                {
                    la_print_path(automata, num_graphs, path, bound);
                }
                if (printModel)
                    la_print_model(ctx, model, automata, num_graphs, bound);

                if (outputFile)
                {
                    int length = strlen(solutionName) + 12;
                    char nameFile[length];
                    snprintf(nameFile, length, "%s_Sat", solutionName);
                    la_create_dot(automata, num_graphs, path, bound, nameFile);
                    printf("Solution printed in sol/%s.dot.\n", nameFile);
                }

                break;
            }

            Z3_del_context(ctx);
        }

        for (int i = 0; i < num_graphs; i++)
            la_delete(automata[i]);
    }
#endif

    for (int i = 0; i < num_graphs; i++)
        graph_delete(graphs[i]);

    return 0;
}
