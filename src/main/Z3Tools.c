
#include "Z3Tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

Z3_context make_context(void)
{
    Z3_config config = Z3_mk_config();
    Z3_context ctx = Z3_mk_context(config);
    Z3_del_config(config);
    return ctx;
}

Z3_ast mk_var(Z3_context ctx, const char *name, Z3_sort ty)
{
    Z3_symbol s = Z3_mk_string_symbol(ctx, name);
    return Z3_mk_const(ctx, s, ty);
}

Z3_ast mk_bool_var(Z3_context ctx, const char *name)
{
    Z3_sort ty = Z3_mk_bool_sort(ctx);
    return mk_var(ctx, name, ty);
}

Z3_ast uniqueFormula(Z3_context ctx, Z3_ast *formulae, int size)
{
    Z3_ast result[size * size];
    result[0] = Z3_mk_or(ctx, size, formulae);
    int count = 1;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < i; j++)
        {
            Z3_ast subFor[2];
            subFor[0] = Z3_mk_not(ctx, formulae[j]);
            subFor[1] = Z3_mk_not(ctx, formulae[i]);
            result[count] = Z3_mk_or(ctx, 2, subFor);
            count++;
        }
    }
    return Z3_mk_and(ctx, count, result);
}

Z3_lbool is_formula_sat(Z3_context ctx, Z3_ast formula)
{
    Z3_solver s = Z3_mk_solver(ctx);
    Z3_solver_inc_ref(ctx, s);
    Z3_solver_assert(ctx, s, formula);

    Z3_lbool result = Z3_solver_check(ctx, s);
    Z3_solver_dec_ref(ctx, s);
    return result;
}

Z3_model get_model_from_sat_formula(Z3_context ctx, Z3_ast formula)
{
    Z3_solver s = Z3_mk_solver(ctx);
    Z3_solver_inc_ref(ctx, s);
    Z3_solver_assert(ctx, s, formula);

    Z3_model m = 0;
    Z3_lbool result = Z3_solver_check(ctx, s);

    switch (result)
    {
    case Z3_L_FALSE:
        fprintf(stderr, "Error: Trying to get a model from an unsat formula.\n");
        Z3_solver_dec_ref(ctx, s);
        exit(1);
    case Z3_L_UNDEF:
        printf("Warning: Getting a partial model from a formula of unknown satisfiability.\n");
        break;
    case Z3_L_TRUE:
        break;
    }

    m = Z3_solver_get_model(ctx, s);
    if (m)
        Z3_model_inc_ref(ctx, m);
    Z3_solver_dec_ref(ctx, s);
    return m;
}

Z3_lbool solve_formula(Z3_context ctx, Z3_ast formula, Z3_model *model)
{
    Z3_solver s = Z3_mk_solver(ctx);
    Z3_solver_inc_ref(ctx, s);
    Z3_solver_assert(ctx, s, formula);

    Z3_lbool result = Z3_solver_check(ctx, s);

    switch (result)
    {
    case Z3_L_FALSE:
        fprintf(stderr, "Warning: Formula unsatisfiable, no model produced, if you try to use it, it will probably crash.\n");
        break;
    case Z3_L_UNDEF:
        printf("Warning: Getting a partial model from a formula of unknown satisfiability.\n");
        break;
    case Z3_L_TRUE:
        *model = Z3_solver_get_model(ctx, s);
        if (*model)
            Z3_model_inc_ref(ctx, *model);
    }

    Z3_solver_dec_ref(ctx, s);
    return result;
}

bool value_of_var_in_model(Z3_context ctx, Z3_model model, Z3_ast variable)
{
    Z3_ast result;
    bool has_value = Z3_model_eval(ctx, model, variable, Z3_L_TRUE, &result);
    if (!has_value)
    {
        fprintf(stderr, "warning: %s has no value in model. Treated as false.\n", Z3_ast_to_string(ctx, variable));
        return false;
    }

    if (Z3_mk_true(ctx) == result)
        return true;
    if (Z3_mk_false(ctx) == result)
        return false;

    fprintf(stderr, "Error: Used on a non-boolean formula, or other unknown error\n");
    exit(1);
}
