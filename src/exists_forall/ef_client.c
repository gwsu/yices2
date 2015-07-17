/*
 * The Yices SMT Solver. Copyright 2015 SRI International.
 *
 * This program may only be used subject to the noncommercial end user
 * license agreement which is downloadable along with this program.
 */

/*
 * Client utilities for EF-solving
 */

#if defined(CYGWIN) || defined(MINGW)
#ifndef __YICES_DLLSPEC__
#define __YICES_DLLSPEC__ __declspec(dllexport)
#endif
#endif

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

#include "api/yices_globals.h"

#include "exists_forall/ef_client.h"

#include "yices.h"
#include "yices_exit_codes.h"

/*
 * Conversion of EF preprocessing codes to string
 */
static const char * const efcode2error[NUM_EF_CODES] = {
  "no error",
  "assertions contain uninterpreted functions",
  "invalid quantifier nesting (not an exists/forall problem)",
  "non-atomic universal variables",
  "non-atomic existential variables",
  "internal error",
};


/*
 * Table to convert  ef-solver status to a string
 */
static const char* const ef_status2string[] = {
  "idle",
  "searching",
  "unknown",
  "sat",
  "unsat",
  "interrupted",
  "subst error",
  "tval error",
  "check error",
  "assert error",
  "error",
};

/*
 * Conversion of internalization code to an error message
 */
static const char * const code2error[NUM_INTERNALIZATION_ERRORS] = {
  "no error",
  "internal error",
  "type error",
  "formula contains free variables",
  "logic not supported",
  "the context does not support uninterpreted functions",
  "the context does not support scalar types",
  "the context does not support tuples",
  "the context does not support uninterpreted types",
  "the context does not support arithmetic",
  "the context does not support bitvectors",
  "the context does not support function equalities",
  "the context does not support quantifiers",
  "the context does not support lambdas",
  "not an IDL formula",
  "not an RDL formula",
  "non-linear arithmetic not supported",
  "too many variables for the arithmetic solver",
  "too many atoms for the arithmetic solver",
  "arithmetic solver exception",
  "bitvector solver exception",
};


static void report_ok(frontend_t client){
  switch(client){
  case YICES_MAIN: print_ok(); break;
  case YICES_SMT2: report_success(); break;
  }
}


/*
 * Initialize the ef_parameters to default values
 * We need to be able to tweak these parameters in a similar fashion to yices_reval.
 */
static inline void init_ef_params(ef_client_t *efc){
  efc->ef_parameters.flatten_iff = false;
  efc->ef_parameters.flatten_ite = false;
  efc->ef_parameters.gen_mode = EF_GEN_AUTO_OPTION;
  efc->ef_parameters.max_samples = 5;
  efc->ef_parameters.max_iters = 100;
}

void init_ef_client(frontend_t client, ef_client_t *efc) {
  efc->client = client;
  init_ef_params(efc);
  efc->efprob = NULL;
  efc->efsolver = NULL;
  efc->efcode = EF_NO_ERROR;
  efc->efdone = false;
}

void delete_ef_client(ef_client_t *efc) {
  if (efc->efprob != NULL) {
    delete_ef_prob(efc->efprob);
    safe_free(efc->efprob);
    efc->efprob = NULL;
  }
  if (efc->efsolver != NULL) {
    delete_ef_solver(efc->efsolver);
    safe_free(efc->efsolver);
    efc->efsolver = NULL;
  }
  efc->efdone = false;
}


/*
 * Build the EF-problem descriptor from the set of delayed assertions
 * - do nothing if efprob exists already
 * - store the internalization code in the global efcode flag
 */
void build_ef_problem(ef_client_t *efc, ivector_t *assertions) {
  ef_analyzer_t analyzer;
  ivector_t *v;

  if (efc->efprob == NULL) {
    v = assertions;

    efc->efprob = (ef_prob_t *) safe_malloc(sizeof(ef_prob_t));
    init_ef_analyzer(&analyzer, __yices_globals.manager);
    init_ef_prob(efc->efprob, __yices_globals.manager);
    efc->efcode = ef_analyze(&analyzer, efc->efprob, v->size, v->data,
			     efc->ef_parameters.flatten_ite,
			     efc->ef_parameters.flatten_iff);
    delete_ef_analyzer(&analyzer);
  }
}

/*
 * Print the translation code returned by assert
 */
void print_internalization_code(int32_t code, uint32_t verbosity, frontend_t client) {
  assert(-NUM_INTERNALIZATION_ERRORS < code && code <= TRIVIALLY_UNSAT);
  if (code == TRIVIALLY_UNSAT) {
    fprintf(stderr, "unsat\n");
    fflush(stderr);
  } else if (verbosity > 0 && code == CTX_NO_ERROR) {
    report_ok(client);
  } else if (code < 0) {
    code = - code;
    fprint_error(stderr, client, code2error[code]);
  }
}

/*
 * Print the translation code returned by ef_analyze
 */
void print_ef_analyze_code(ef_code_t code, frontend_t client) {
  if (code == EF_NO_ERROR) {
    report_ok(client);
  } else {
    fprint_error(stderr, client, efcode2error[code]);
  }
}



/*
 * Print the efsolver status
 */
void print_ef_status(ef_client_t *efc, uint32_t verbosity, FILE *err) {
  ef_status_t stat;
  int32_t error;
  ef_solver_t *efsolver;

  efsolver = efc->efsolver;

  assert(efsolver != NULL && efc->efdone);

  if (verbosity > 0) {
    printf("ef-solve: %"PRIu32" iterations\n", efsolver->iters);
  }

  stat = efsolver->status;
  error = efsolver->error_code;

  switch (stat) {
  case EF_STATUS_SAT:
  case EF_STATUS_UNKNOWN:
  case EF_STATUS_UNSAT:
  case EF_STATUS_INTERRUPTED:
    fputs(ef_status2string[stat], stdout);
    fputc('\n', stdout);
    if (verbosity > 0) {
      if (stat == EF_STATUS_SAT) {
        print_ef_solution(stdout, efsolver);
        fputc('\n', stdout);
      }
    }
    fflush(stdout);
    break;

  case EF_STATUS_SUBST_ERROR:
    if (error == -1) {
      fprint_error(err, efc->client, "EF solver failed: degree overflow in substitution");
    } else {
      assert(error == -2);
      freport_bug(err, efc->client, "EF solver failed: internal error");
    }
    break;

  case EF_STATUS_ASSERT_ERROR:
    assert(error < 0);
    print_internalization_code(error, verbosity, efc->client);
    break;

  case EF_STATUS_MDL_ERROR:
  case EF_STATUS_IMPLICANT_ERROR:
  case EF_STATUS_PROJECTION_ERROR:
  case EF_STATUS_TVAL_ERROR:
  case EF_STATUS_CHECK_ERROR:
  case EF_STATUS_ERROR:
  case EF_STATUS_IDLE:
  case EF_STATUS_SEARCHING:
    freport_bug(err, "ef-status: %s\n", ef_status2string[stat]);
    break;

  }
}


model_t *ef_get_model(ef_client_t *efc){
  ef_solver_t *efsolver;

  if (efc->efdone) {
    efsolver = efc->efsolver;
    assert(efsolver != NULL);
    if (efsolver->status == EF_STATUS_SAT) {
      assert(efsolver->exists_model != NULL);
      return efsolver->exists_model;
    } else {
      fprint_error(stderr, efc->client, "(ef-solve) did not find a solution. No model\n");
    }
  } else {
    fprint_error(stderr, efc->client, "Can't build a model. Call (ef-solve) first.\n");
  }
  return NULL;
}

void ef_solve(ef_client_t *efc, ivector_t *assertions, param_t *parameters, smt_logic_t logic_code, context_arch_t arch,
	      uint32_t verbosity, tracer_t *tracer, FILE *err) {
  build_ef_problem(efc, assertions);
  if (efc->efcode != EF_NO_ERROR) {
    // error in preprocessing
    print_ef_analyze_code(efc->efcode, efc->client);
  } else {
    if (! efc->efdone) {
      assert(efc->efsolver == NULL);
      efc->efsolver = (ef_solver_t *) safe_malloc(sizeof(ef_solver_t));
      init_ef_solver(efc->efsolver, efc->efprob, logic_code, arch);
      if (tracer != NULL) {
	ef_solver_set_trace(efc->efsolver, tracer);
      }
      /*
       * If the problem has real variables, we force GEN_BY_PROJ
       */
      ef_solver_check(efc->efsolver, parameters, efc->ef_parameters.gen_mode,
		      efc->ef_parameters.max_samples, efc->ef_parameters.max_iters);
      efc->efdone = true;
    }
    print_ef_status(efc, verbosity, err);
  }
}

void fprint_error(FILE* fp, frontend_t client, const char *format, ...) {
  va_list p;
  //FIXME
  //open_error();
  va_start(p, format);
  if (vfprintf(fp, format, p) < 0) {
    //failed_output();
  }
  va_end(p);
  //close_error();
}

void __attribute__((noreturn)) freport_bug(FILE *fp, const char *format, ...) {
  va_list p;

  fprintf(fp, "\n*************************************************************\n");
  fprintf(fp, "FATAL ERROR: ");
  va_start(p, format);
  vfprintf(fp, format, p);
  va_end(p);
  fprintf(fp, "\n*************************************************************\n");
  fprintf(fp, "Please report this bug to yices-bugs@csl.sri.com.\n");
  fprintf(fp, "To help us diagnose this problem, please include the\n"
                  "following information in your bug report:\n\n");
  fprintf(fp, "  Yices version: %s\n", yices_version);
  fprintf(fp, "  Build date: %s\n", yices_build_date);
  fprintf(fp, "  Platform: %s (%s)\n", yices_build_arch, yices_build_mode);
  fprintf(fp, "\n");
  fprintf(fp, "Thank you for your help.\n");
  fprintf(fp, "*************************************************************\n\n");
  fflush(fp);

  exit(YICES_EXIT_INTERNAL_ERROR);
}




