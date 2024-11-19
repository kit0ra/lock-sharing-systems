#include "DeadlockReduction.h"
#include "Z3Tools.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


Z3_ast variable_node_on_path(Z3_context ctx, int automaton, int node, int step)
{
    char name[60];
    snprintf(name, 60, "step %d: (aut: %d, node: %d)", step, automaton, node);
    return mk_bool_var(ctx, name);
}

/**
 * @brief Creates a formula containing only the variable representing that at step @p step, lock @p lock it taken by some process.
 *
 * @param ctx The solver context.
 * @param lock
 * @param step
 * @return Z3_ast
 */
Z3_ast variable_lock_at_step(Z3_context ctx, int lock, int step)
{
    char name[40];
    snprintf(name, 40, "step %d : lock %d", step, lock);
    return mk_bool_var(ctx, name);
}

/*
Here, you should put formula producing the formula. Cut it in subfunctions, similarly to what is present in ColouringReduction.c.
*/

// Z3_ast deadlock_reduction(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound)
// {
//     return Z3_mk_false(ctx);
// }

void la_path_from_model(Z3_context ctx, Z3_model model, LockAutomaton *automata, int num_automata, step *path, int bound)
{
    int max_lock = 0;
    for (int aut = 0; aut < num_automata; aut++)
    {
        int aux = la_get_max_lock(automata[aut]);
        if (max_lock < aux)
            max_lock = aux;
    }
    for (int step = 0; step < bound; step++)
    {
        int action_taken = 0;
        for (int lock = 1; lock <= max_lock; lock++)
        {
            bool prev = value_of_var_in_model(ctx, model, variable_lock_at_step(ctx, lock, step));
            bool next = value_of_var_in_model(ctx, model, variable_lock_at_step(ctx, lock, step + 1));
            if (prev && !next)
            {
                action_taken = -lock;
                break;
            }
            if (!prev && next)
            {
                action_taken = lock;
                break;
            }
        }
        for (int automaton = 0; automaton < num_automata; automaton++)
        {
            int num_nodes = la_get_num_nodes(automata[automaton]);
            for (int source = 0; source < num_nodes; source++)

                for (int target = 0; target < num_nodes; target++)
                {

                    if (source == target || !la_is_edge(automata[automaton], source, target))
                        continue;
                    if (la_get_edge_action(automata[automaton], source, target) != action_taken)
                        continue;
                    Z3_ast var_source = variable_node_on_path(ctx, automaton, source, step);
                    Z3_ast var_target = variable_node_on_path(ctx, automaton, target, step + 1);
                    if (value_of_var_in_model(ctx, model, var_source) && value_of_var_in_model(ctx, model, var_target))
                    {
                        path[step] = la_step_create(automaton, source, target, action_taken);
                        goto Next_step;
                    }
                }
        }
        for (int automaton = 0; automaton < num_automata; automaton++)
        {
            int num_nodes = la_get_num_nodes(automata[automaton]);
            for (int source = 0; source < num_nodes; source++)
            {
                if (!la_is_edge(automata[automaton], source, source))
                    continue;
                if (la_get_edge_action(automata[automaton], source, source) != action_taken)
                    continue;
                Z3_ast var_source = variable_node_on_path(ctx, automaton, source, step);
                Z3_ast var_target = variable_node_on_path(ctx, automaton, source, step + 1);
                if (value_of_var_in_model(ctx, model, var_source) && value_of_var_in_model(ctx, model, var_target))
                {
                    path[step] = la_step_create(automaton, source, source, action_taken);
                    goto Next_step;
                }
            }
        }
    Next_step:
    }
}

void la_print_model(Z3_context ctx, Z3_model model, LockAutomaton *automata, int num_automata, int bound)
{
    int max_lock = 0;
    for (int aut = 0; aut < num_automata; aut++)
    {
        int aux = la_get_max_lock(automata[aut]);
        if (max_lock < aux)
            max_lock = aux;
    }
    printf("Information deduced from the model of the formula:\n\n");
    for (int step = 0; step <= bound; step++)
    {
        printf("At step %d:\n", step);
        printf("Locks taken:\n");
        for (int lock = 1; lock <= max_lock; lock++)
        {
            if (value_of_var_in_model(ctx, model, variable_lock_at_step(ctx, lock, step)))
                printf("%d ", lock);
        }
        printf("\n");
        for (int aut = 0; aut < num_automata; aut++)
        {
            int num_nodes = la_get_num_nodes(automata[aut]);
            printf("Automaton %s(%d) is in state : ", la_get_name(automata[aut]), aut);
            for (int node = 0; node < num_nodes; node++)
            {
                if (value_of_var_in_model(ctx, model, variable_node_on_path(ctx, aut, node, step)))
                    printf("%s ", la_get_node_name(automata[aut], node));
            }
            printf("\n");
        }
    }
}

Z3_ast generate_state_constraints(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound) {
    Z3_ast constraints[num_automata][bound + 1];
    for (int aut = 0; aut < num_automata; aut++) {
        int num_nodes = la_get_num_nodes(automata[aut]);
        for (int step = 0; step <= bound; step++) {
            Z3_ast node_vars[num_nodes];
            for (int node = 0; node < num_nodes; node++) {
                node_vars[node] = variable_node_on_path(ctx, aut, node, step);
            }
            // Automaton must be in one of its nodes
            constraints[aut][step] = Z3_mk_or(ctx, num_nodes, node_vars);

            // No two nodes simultaneously
            for (int i = 0; i < num_nodes; i++) {
                for (int j = i + 1; j < num_nodes; j++) {
                    Z3_ast not_both = Z3_mk_not(ctx, Z3_mk_and(ctx, 2, (Z3_ast[]){ node_vars[i], node_vars[j] }));
                    constraints[aut][step] = Z3_mk_and(ctx, 2, (Z3_ast[]){ constraints[aut][step], not_both });
                }
            }
        }
    }

    Z3_ast all_constraints[num_automata * (bound + 1)];
    for (int aut = 0; aut < num_automata; aut++) {
        for (int step = 0; step <= bound; step++) {
            all_constraints[aut * (bound + 1) + step] = constraints[aut][step];
        }
    }

    return Z3_mk_and(ctx, num_automata * (bound + 1), all_constraints);
}

Z3_ast generate_lock_constraints(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound) {
    // Determine the maximum number of locks across all automata
    int max_locks = 0;
    for (int i = 0; i < num_automata; i++) {
        int locks = la_get_max_lock(automata[i]); // Assumes `la_get_max_lock` is defined
        if (locks > max_locks) max_locks = locks;
    }

    Z3_ast constraints[bound];
    for (int step = 0; step < bound; step++) {
        Z3_ast lock_constraints[max_locks];
        for (int lock = 1; lock <= max_locks; lock++) {
            Z3_ast lock_var = variable_lock_at_step(ctx, lock, step);
            Z3_ast lock_next_var = variable_lock_at_step(ctx, lock, step + 1);

            // Lock acquisition/release consistency
            lock_constraints[lock - 1] = Z3_mk_implies(ctx, lock_var, lock_next_var);
        }
        constraints[step] = Z3_mk_and(ctx, max_locks, lock_constraints);
    }

    return Z3_mk_and(ctx, bound, constraints);
}

int la_get_trying_node(LockAutomaton automaton) {
    int num_nodes = la_get_num_nodes(automaton); // Total number of nodes
    for (int i = 0; i < num_nodes; i++) {
        const char *node_name = la_get_node_name(automaton, i);
        if (strstr(node_name, "Trying") != NULL) { // Check if "Trying" is part of the name
            return i; // Return the index of the "Trying" node
        }
    }
    return -1; // Return -1 if no "Trying" node is found
}




Z3_ast generate_deadlock_constraints(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound) {
    Z3_ast deadlock_conditions[num_automata];
    for (int aut = 0; aut < num_automata; aut++) {
        int trying_node = la_get_trying_node(automata[aut]);
        if (trying_node == -1) {
            fprintf(stderr, "Error: No 'Trying' node found for automaton %d\n", aut);
            exit(EXIT_FAILURE);
        }
        deadlock_conditions[aut] = variable_node_on_path(ctx, aut, trying_node, bound);
    }

    Z3_ast all_deadlocked = Z3_mk_and(ctx, num_automata, deadlock_conditions);

    // No transitions possible at the last step
    int max_locks = 0;
    for (int aut = 0; aut < num_automata; aut++) {
        int locks = la_get_max_lock(automata[aut]);
        if (locks > max_locks) max_locks = locks;
    }

    Z3_ast no_transitions[max_locks];
    for (int lock = 0; lock < max_locks; lock++) {
        no_transitions[lock] = Z3_mk_not(ctx, variable_lock_at_step(ctx, lock + 1, bound));
    }
    Z3_ast no_transition_possible = Z3_mk_and(ctx, max_locks, no_transitions);

    return Z3_mk_and(ctx, 2, (Z3_ast[]){ all_deadlocked, no_transition_possible });
}


Z3_ast deadlock_reduction(Z3_context ctx, LockAutomaton *automata, int num_automata, int bound) {
    Z3_ast state_constraints = generate_state_constraints(ctx, automata, num_automata, bound);
    Z3_ast lock_constraints = generate_lock_constraints(ctx, automata, num_automata, bound);
    Z3_ast deadlock_constraints = generate_deadlock_constraints(ctx, automata, num_automata, bound);

    Z3_ast all_constraints[] = { state_constraints, lock_constraints, deadlock_constraints };
    return Z3_mk_and(ctx, 3, all_constraints);
}
