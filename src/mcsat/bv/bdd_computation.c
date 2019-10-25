/*
 * The Yices SMT Solver. Copyright 2015 SRI International.
 *
 * This program may only be used subject to the noncommercial end user
 * license agreement which is downloadable along with this program.
 */

#include "bdd_computation.h"
#include "bv_utils.h"

#include "terms/bv64_constants.h"
#include "bdd_manager.h"

// will output attachment data, you can get the counts with
//   |& grep tach | sort | uniq -c
//#define DEBUG_ATTACH
//#define DEBUG_ATTACH_ID 0

static inline
void ATTACH_REF(BDD* x) {
#ifdef DEBUG_ATTACH
  int x_id = Cudd_NodeReadIndex(x);
  fprintf(stderr, "attach(%d)\n", x_id);
  if (x_id == DEBUG_ATTACH_ID) {
    fprintf(stderr, "this\n");
  }
#endif
  Cudd_Ref(x);
}

static inline
void DETACH_REF(DdManager* cudd, BDD* x) {
#ifdef DEBUG_ATTACH
  int x_id = Cudd_NodeReadIndex(x);
  fprintf(stderr, "detach(%d)\n", x_id);
  if (x_id == DEBUG_ATTACH_ID) {
    fprintf(stderr, "this\n");
  }
#endif
  Cudd_IterDerefBdd(cudd, x);
}

static inline
void bdds_reverse(BDD** bdds, uint32_t n) {
  assert(n > 0);
  BDD** p = bdds;
  BDD** q = bdds + n - 1;
  BDD* tmp;
  while (p < q) {
    tmp = *p; *p = *q; *q = tmp;
    p ++; q --;
  }
}

static inline
void bdds_swap(BDD** a, BDD** b, uint32_t n) {
  BDD* tmp;
  for (uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    assert(b[i] != NULL);
    tmp = a[i]; a[i] = b[i]; b[i] = tmp;
  }
}

static inline
void bdds_copy(BDD** out, BDD** a, uint32_t n) {
  for (uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    assert(out[i] == NULL);
    out[i] = a[i];
    ATTACH_REF(out[i]);
  }
}

static inline
void bdds_move(BDD** out, BDD** a, uint32_t n) {
  for (uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    assert(out[i] == NULL);
    out[i] = a[i];
    a[i] = NULL;
  }
}

static void cudd_out_of_mem(size_t s) {
  fflush(stdout);
  fprintf(stderr, "\nCUDD: failed to allocate %zu bytes\n", s);
  out_of_memory();
}

CUDD* bdds_new(bdd_manager_t* bddm) {
  CUDD* cudd = (CUDD*) safe_malloc(sizeof(CUDD));
  cudd->bddm = bddm;
  cudd->cudd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS,0);
  (void) Cudd_RegisterOutOfMemoryCallback(cudd->cudd, cudd_out_of_mem);
  //  Cudd_AutodynDisable(cudd->cudd);
  cudd->tmp_alloc_size = 0;
  cudd->tmp_inputs = NULL;
  cudd->tmp_model = NULL;

  return cudd;
}

void bdds_delete(CUDD* cudd) {
  int leaks = Cudd_CheckZeroRef(cudd->cudd);
  (void) leaks;
  assert(leaks == 0);
  Cudd_Quit(cudd->cudd);
  safe_free(cudd->tmp_inputs);
  safe_free(cudd->tmp_model);
  safe_free(cudd);
}

void bdds_init(BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    a[i] = NULL;
  }
}

void bdds_clear(CUDD* cudd, BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    if (a[i] != NULL) {
      DETACH_REF(cudd->cudd, a[i]);
    }
    a[i] = NULL;
  }
}

void bdds_detach(CUDD* cudd, BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    DETACH_REF(cudd->cudd, a[i]);
  }
}

void bdds_attach(BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    ATTACH_REF(a[i]);
  }
}

bool bdds_eq(BDD** a, BDD** b, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

void bdds_print(CUDD* cudd, BDD** a, uint32_t n, FILE* out) {
  if (n == 1) {
    fprintf(out, "bdd(%p): ", a[0]);
  } else {
    fprintf(out, "bdds(%p): ", a);
  }
  Cudd_DumpFactoredForm(cudd->cudd, n, a, NULL, NULL, out);
}

void bdds_mk_variable(CUDD* cudd, BDD** out, uint32_t n) {
  BDD* bdd_i = NULL;
  for (uint32_t i = 0; i < n; ++i) {
    bdd_i = Cudd_bddNewVar(cudd->cudd);
    /*
     * BD: bdd_i can be NULL here. There's a hard-coded limit on the
     * number of variables in cudd (and that's 2^16 -1 on 32bit
     * machines).
     *
     * We should check for this and die cleanly rather than seg fault.
     */
    out[n-i-1] = bdd_i;
    // We do increase the reference count so that we are uniform when dereferencing
    ATTACH_REF(bdd_i);
  }
  if (bdd_i) {
    // Max index: last allocated variable
    unsigned int needed_size = Cudd_NodeReadIndex(bdd_i) + 1;
    if (needed_size > cudd->tmp_alloc_size) {
      if (cudd->tmp_alloc_size == 0) {
        cudd->tmp_alloc_size = 10;
      }
      while (needed_size > cudd->tmp_alloc_size) {
        cudd->tmp_alloc_size += cudd->tmp_alloc_size >> 1;
      }
      cudd->tmp_inputs = (int*) safe_realloc(cudd->tmp_inputs, sizeof(int)*cudd->tmp_alloc_size);
      cudd->tmp_model = (char*) safe_realloc(cudd->tmp_model, sizeof(char)*cudd->tmp_alloc_size);
    }
  }
}

void bdds_mk_repeat(CUDD* cudd, BDD** out, BDD* b, uint32_t n) {
  assert(b != NULL);
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = b;
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_zero(CUDD* cudd, BDD** out, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = Cudd_ReadLogicZero(cudd->cudd);
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_one(CUDD* cudd, BDD** out, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    if (i) {
      out[i] = Cudd_ReadLogicZero(cudd->cudd);
    } else {
      out[i] = Cudd_ReadOne(cudd->cudd);
    }
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_constant_64(CUDD* cudd, BDD** out, uint32_t n, uint64_t c) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    bool bit_i = i < 64 ? tst_bit64(c, i) : false;
    out[i] = bit_i ? Cudd_ReadOne(cudd->cudd) : Cudd_ReadLogicZero(cudd->cudd);
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_constant_raw(CUDD* cudd, BDD** out, uint32_t n, const uint32_t* c) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    bool bit_i = bvconst_tst_bit(c, i);
    out[i] = bit_i ? Cudd_ReadOne(cudd->cudd) : Cudd_ReadLogicZero(cudd->cudd);
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_constant(CUDD* cudd, BDD** out, uint32_t n, const bvconstant_t* c) {
  assert(n == c->bitsize);
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    bool bit_i = bvconst_tst_bit(c->data, i);
    out[i] = bit_i ? Cudd_ReadOne(cudd->cudd) : Cudd_ReadLogicZero(cudd->cudd);
    ATTACH_REF(out[i]);
  }
}

bool bdds_is_constant(CUDD* cudd, BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    if (!Cudd_IsConstant(a[i])) {
      return false;
    }
  }
  return true;
}

bool bdds_is_constant_zero(CUDD* cudd, BDD** a, uint32_t n) {
  BDD* zero = Cudd_ReadLogicZero(cudd->cudd);
  for(uint32_t i = 0; i < n; ++ i) {
    if (a[i] != zero) {
      return false;
    }
  }
  return true;
}

bool bdds_is_constant_one(CUDD* cudd, BDD** a, uint32_t n) {
  if (a[0] != Cudd_ReadOne(cudd->cudd)) {
    return false;
  }
  BDD* zero = Cudd_ReadLogicZero(cudd->cudd);
  for (uint32_t i = 1; i < n; ++i) {
    if (a[i] != zero) {
      return false;
    }
  }
  return true;
}

bool bdds_is_constant_neg_one(CUDD* cudd, BDD** a, uint32_t n) {
  BDD* one = Cudd_ReadOne(cudd->cudd);
  for(uint32_t i = 0; i < n; ++ i) {
    if (a[i] != one) {
      return false;
    }
  }
  return true;
}

// 000000100000 for positive 2^k
// 111111100000 for negative -2^k
// 100000000000
// returns k for positive, -k for negative assumes k != 0, i.e. x != 1, -1
int32_t bdds_is_constant_pow2(CUDD* cudd, BDD** a, uint32_t n) {
  int32_t pow = 0;
  BDD* zero = Cudd_ReadLogicZero(cudd->cudd);
  BDD* one = Cudd_ReadOne(cudd->cudd);
  // Find first one
  while (pow < n && a[pow] == zero) {
    pow ++;
  }
  if (pow == n) {
    return 0;
  }
  if (pow == n-1) {
    return pow;
  }
  // a[pow] = 1, and at
  // Check if power: rest is either all 0, or all 1
  for (int32_t i = pow + 1; i + 1 < n; ++ i) {
    if (a[i] != a[i+1]) {
      return 0;
    }
  }
  // Sign is the sign of the top bit
  if (a[n-1] == one) {
    return -pow;
  } else {
    return pow;
  }
}

static
void bdd_min_max(CUDD* cudd, BDD* a, unsigned int* min, unsigned int* max) {

  DdGen* a_gen;
  BDD* current;
  Cudd_ForeachNode(cudd->cudd, a, a_gen, current)
  {
    unsigned int current_index = Cudd_NodeReadIndex(current);
    if (current_index > *max)
      *max = current_index;
    if (current_index < *min)
      *min = current_index;
  }
}

bool bdds_are_disjoint(CUDD* cudd, BDD* a, BDD* b) {
  assert(a != NULL);
  assert(b != NULL);

  // could use Cudd_ClassifySupport but it does the same and more, we want
  // this to be as efficient as possible
  int* a_indices = NULL;
  int* b_indices = NULL;
  int a_indices_size = Cudd_SupportIndices(cudd->cudd, a, &a_indices);
  int b_indices_size = Cudd_SupportIndices(cudd->cudd, b, &b_indices);

  int a_i = 0, b_i = 0;
  bool disjoint = true;
  while (a_i < a_indices_size && b_i < b_indices_size) {
    int cmp = a_indices[a_i] - b_indices[b_i];
    if (cmp > 0) {
      // a_indices[a_i] > b_indices[b_i]
      b_i ++;
    } else if (cmp < 0) {
      // a_indices[a_i] < b_indices[b_i]
      a_i ++;
    } else {
      // a_indices[a_i] == b_indices[b_i]
      disjoint = false; // not disjoint
      break;
    }
  }

  // release
  safe_free(a_indices);
  safe_free(b_indices);

  return disjoint;
}


void bdds_mk_not(CUDD* cudd, BDD** out, BDD** a, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = Cudd_Not(a[i]);
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_and(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = Cudd_bddAnd(cudd->cudd, a[i], b[i]);
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_conjunction(CUDD* cudd, BDD** out, BDD** a, uint32_t n) {
  assert(out[0] == NULL);
  BDD* tmp;
  BDD* current = a[0];
  ATTACH_REF(current);
  for(uint32_t i = 1; i < n; ++ i) {
    tmp = Cudd_bddAnd(cudd->cudd, a[i], current);
    ATTACH_REF(tmp);
    DETACH_REF(cudd->cudd, current);
    current = tmp;
  }
  out[0] = current;
}

void bdds_mk_or(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  for(uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = Cudd_bddOr(cudd->cudd, a[i], b[i]);
    ATTACH_REF(out[i]);
  }
}

/** - (a << shift) */
void bdds_mk_2s_complement_with_shift(CUDD* cudd, BDD** out, BDD** a, uint32_t shift, uint32_t n) {
  BDD* carry = Cudd_ReadOne(cudd->cudd);
  ATTACH_REF(carry);

  BDD* one = Cudd_ReadOne(cudd->cudd);
  ATTACH_REF(one);

  for(uint32_t i = 0; i < n; ++ i) {
    BDD* a_shl_neg = i >= shift ? Cudd_Not(a[i-shift]) : one;
    BDD* sum = Cudd_bddXor(cudd->cudd, carry, a_shl_neg);
    ATTACH_REF(sum);

    BDD* new_carry = Cudd_bddAnd(cudd->cudd, carry, a_shl_neg);
    ATTACH_REF(new_carry);
    DETACH_REF(cudd->cudd, carry);
    carry = new_carry;

    assert(out[i] == NULL);
    out[i] = sum;
  }

  DETACH_REF(cudd->cudd, carry);
  DETACH_REF(cudd->cudd, one);
}


void bdds_mk_2s_complement(CUDD* cudd, BDD** out, BDD** a, uint32_t n) {
  bdds_mk_2s_complement_with_shift(cudd, out, a, 0, n);
}

void bdds_mk_shl_const(CUDD* cudd, BDD** out, BDD** a, uint32_t shift, uint32_t n) {
  for (uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    if (i < shift) {
      out[i] = Cudd_ReadLogicZero(cudd->cudd);
    } else {
      out[i] = a[i-shift];
    }
    ATTACH_REF(out[i]);
  }
}

void bdds_mk_shl(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bddvec_t shift_const_v = bdd_manager_new_vec(cudd->bddm, n);
  BDD** shift_const = bdd_manager_get_bdds(cudd->bddm, shift_const_v);
  BDD* b_eq_shift_const = NULL;

  bdds_mk_zero(cudd, out, n);

  for (uint32_t shift = 0; shift < n; ++ shift) {

    bdds_mk_constant_64(cudd, shift_const, n, shift);
    bdds_mk_eq(cudd, &b_eq_shift_const, b, shift_const, n);

    for (uint32_t i = 0; i < n; ++ i) {
      BDD* shifted_a_i = i < shift ? Cudd_ReadLogicZero(cudd->cudd) : a[i-shift];
      ATTACH_REF(shifted_a_i);

      BDD* else_case = out[i];
      out[i]  = Cudd_bddIte(cudd->cudd, b_eq_shift_const, shifted_a_i, else_case);
      ATTACH_REF(out[i]);

      DETACH_REF(cudd->cudd, shifted_a_i);
      DETACH_REF(cudd->cudd, else_case);
    }

    bdds_clear(cudd, shift_const, n);
    bdds_clear(cudd, &b_eq_shift_const, 1);
  }

  bdd_manager_delete_vec(cudd->bddm, shift_const_v);
}

void bdds_mk_lshr(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bddvec_t shift_const_v = bdd_manager_new_vec(cudd->bddm, n);
  BDD** shift_const = bdd_manager_get_bdds(cudd->bddm, shift_const_v);
  BDD* b_eq_shift_const = NULL;

  bdds_mk_zero(cudd, out, n);

  for (uint32_t shift = 0; shift < n; ++ shift) {

    bdds_mk_constant_64(cudd, shift_const, n, shift);
    bdds_mk_eq(cudd, &b_eq_shift_const, b, shift_const, n);

    for (uint32_t i = 0; i < n; ++ i) {
      BDD* shifted_a_i = i + shift >= n ? Cudd_ReadLogicZero(cudd->cudd) : a[i+shift];
      ATTACH_REF(shifted_a_i);

      BDD* else_case = out[i];
      out[i]  = Cudd_bddIte(cudd->cudd, b_eq_shift_const, shifted_a_i, else_case);
      ATTACH_REF(out[i]);

      DETACH_REF(cudd->cudd, shifted_a_i);
      DETACH_REF(cudd->cudd, else_case);
    }

    bdds_clear(cudd, shift_const, n);
    bdds_clear(cudd, &b_eq_shift_const, 1);
  }

  bdd_manager_delete_vec(cudd->bddm, shift_const_v);
}

void bdds_mk_ashr(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bddvec_t shift_const_v = bdd_manager_new_vec(cudd->bddm, n);
  BDD** shift_const = bdd_manager_get_bdds(cudd->bddm, shift_const_v);
  BDD* b_eq_shift_const = NULL;

  bdds_mk_repeat(cudd, out, a[n-1], n);

  for (uint32_t shift = 0; shift < n; ++ shift) {

    bdds_mk_constant_64(cudd, shift_const, n, shift);
    bdds_mk_eq(cudd, &b_eq_shift_const, b, shift_const, n);

    for (uint32_t i = 0; i < n; ++ i) {
      BDD* shifted_a_i = i + shift >= n ? a[n-1] : a[i+shift];
      ATTACH_REF(shifted_a_i);

      BDD* else_case = out[i];
      out[i]  = Cudd_bddIte(cudd->cudd, b_eq_shift_const, shifted_a_i, else_case);
      ATTACH_REF(out[i]);

      DETACH_REF(cudd->cudd, shifted_a_i);
      DETACH_REF(cudd->cudd, else_case);
    }

    bdds_clear(cudd, shift_const, n);
    bdds_clear(cudd, &b_eq_shift_const, 1);
  }

  bdd_manager_delete_vec(cudd->bddm, shift_const_v);
}

/** Make a Boolean or: a[0] || ... || a[n] */
static
void bdds_mk_bool_or(CUDD* cudd, BDD** out, const pvector_t* a) {
  uint32_t n = a->size;
  out[0] = Cudd_ReadLogicZero(cudd->cudd);
  ATTACH_REF(out[0]);
  for (uint32_t i = 0; i < n; i ++ ) {
    BDD* tmp = out[0];
    BDD** child_i = (BDD**) a->data[i];
    out[0] = Cudd_bddOr(cudd->cudd, tmp, child_i[0]);
    ATTACH_REF(out[0]);
    DETACH_REF(cudd->cudd, tmp);
  }
}

void bdds_mk_eq(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);
  assert(out[0] == NULL);
  bdds_reverse(a, n);
  bdds_reverse(b, n);
  out[0] = Cudd_Xeqy(cudd->cudd, n, a, b);
  bdds_reverse(a, n);
  bdds_reverse(b, n);
  ATTACH_REF(out[0]);
}

/** Compute equality, but try to make it over disjoint */
bddvec_t bdds_mk_eq_manual(CUDD* cudd, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);

  // Allocate a temporary storage for individual equalities
  bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, n);
  BDD** tmp_bdds = bdd_manager_get_bdds(cudd->bddm, tmp_v);

  // Create individual equalities
  for (uint32_t i = 0; i < n; ++ i) {
    assert(a[i] != NULL);
    assert(b[i] != NULL);
    tmp_bdds[i] = Cudd_bddXnor(cudd->cudd, a[i], b[i]);
    ATTACH_REF(tmp_bdds[i]);
  }

  // Add them all together and construct the result vector
  pvector_t result_bdds;
  init_pvector(&result_bdds, 0);
  bdds_disjoint_set_add(cudd, tmp_bdds, n, &result_bdds);
  bddvec_t result = bdd_manager_new_vec_from(cudd->bddm, (BDD**) result_bdds.data, result_bdds.size);

  // Clear up the temporaries
  bdds_clear(cudd, tmp_bdds, n);
  bdd_manager_delete_vec(cudd->bddm, tmp_v);
  delete_pvector(&result_bdds);

  return result;
}

void bdds_mk_eq0(CUDD* cudd, BDD** out, BDD** a, uint32_t n) {
  assert(n > 0);
  assert(out[0] == NULL);

  BDD* result = Cudd_ReadOne(cudd->cudd);
  ATTACH_REF(result);

  BDD* zero = Cudd_ReadLogicZero(cudd->cudd);
  ATTACH_REF(zero);

  for (uint32_t i = 0; i < n; ++ i) {
    BDD* new_result = Cudd_bddAnd(cudd->cudd, result, Cudd_Not(a[i]));
    ATTACH_REF(new_result);
    DETACH_REF(cudd->cudd, result);
    result = new_result;
    // Short-circuit
    if (result == zero) {
      break;
    }
  }

  DETACH_REF(cudd->cudd, zero);
  out[0] = result;
}

/** out += cond*a << shift (out must be allocated) */
void bdds_mk_plus_in_place(CUDD* cudd, BDD** out, BDD** a, BDD* cond, uint32_t n, uint32_t shift) {

//  assert(shift < n);

  // Constant optimization
  if (cond != NULL) {
    if (cond == Cudd_ReadLogicZero(cudd->cudd)) {
      // out += zero, noop
      return;
    }
  }

  BDD* carry = Cudd_ReadLogicZero(cudd->cudd);
  ATTACH_REF(carry);

  for (uint32_t i = shift, j = 0; i < n; ++ i, ++ j) {
    // What we are adding (with condition if there)
    BDD* to_add = cond == NULL ? a[j] : Cudd_bddAnd(cudd->cudd, cond, a[j]);
    ATTACH_REF(to_add);
    // Sum up the bits
    BDD* sum1 = Cudd_bddXor(cudd->cudd, out[i], to_add);
    ATTACH_REF(sum1);
    BDD* sum2 = Cudd_bddXor(cudd->cudd, sum1, carry);
    ATTACH_REF(sum2);
    // Compute carry
    BDD* carry1 = Cudd_bddAnd(cudd->cudd, out[i], to_add);
    ATTACH_REF(carry1);
    BDD* carry2 = Cudd_bddAnd(cudd->cudd, sum1, carry);
    ATTACH_REF(carry2);
    DETACH_REF(cudd->cudd, carry);
    carry = Cudd_bddOr(cudd->cudd, carry1, carry2);
    ATTACH_REF(carry);
    // Save to output
    DETACH_REF(cudd->cudd, out[i]);
    out[i] = sum2; // Don't deref sum2, it's in out now
    // Remove temps
    DETACH_REF(cudd->cudd, to_add);
    DETACH_REF(cudd->cudd, sum1);
    DETACH_REF(cudd->cudd, carry1);
    DETACH_REF(cudd->cudd, carry2);
  }

  DETACH_REF(cudd->cudd, carry);
}

void bdds_mk_plus(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bool a_is_const = bdds_is_constant(cudd, a, n);
  bool b_is_const = bdds_is_constant(cudd, b, n);
  if (a_is_const && !b_is_const) {
    // Work with b as const
    BDD** tmp = a; a = b; b = tmp;
  }
  bdds_copy(out, a, n);
  bdds_mk_plus_in_place(cudd, out, b, NULL, n, 0);
}

/** If one of the arguments is a constant, it's always b */
static
void bdds_mk_mult_core(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {

  uint32_t i, j;

  bdds_mk_zero(cudd, out, n);

  for(i = 0; i < n; ++ i) {
    // Skip zeros
    if (b[i] == Cudd_ReadLogicZero(cudd->cudd)) { continue; }
    // Now, see if you can find a sequence of same bits
    j = i + 1;
    while (j < n && b[j] == b[i]) { j ++; }
    // If there is more than 2 bits same, let's optimize
    if (j - i > 2) {
      // We have b_j, ..., b_i the same (v), so we compute the whole block
      // for example
      //  11100*a = 28*a = 2^5*a - 2^2*a = (a << 5) -- (a << 2)
      if (j < n) {
        bdds_mk_plus_in_place(cudd, out, a, b[i], n, j);
      }
      bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, n);
      BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);
      bdds_mk_2s_complement_with_shift(cudd, tmp, a, i, n);
      bdds_mk_plus_in_place(cudd, out, tmp, b[i], n, 0);
      bdds_clear(cudd, tmp, n);
      bdd_manager_delete_vec(cudd->bddm, tmp_v);
      // Done, we continue with j
      i = j - 1;
    } else {
      bdds_mk_plus_in_place(cudd, out, a, b[i], n, i);
    }
  }
}

/** Multiplication with repeated addition (we index over bits of b) */
void bdds_mk_mult(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  // Check for constants
  bool a_is_const = bdds_is_constant(cudd, a, n);
  bool b_is_const = bdds_is_constant(cudd, b, n);
  if (a_is_const && !b_is_const) {
    // Work with b as const
    BDD** tmp = a; a = b; b = tmp;
  }
  // Ok now, check for special constants
  if (b_is_const) {
    if (bdds_is_constant_zero(cudd, b, n)) {
      // a*0 = 0
      bdds_mk_zero(cudd, out, n);
      return;
    }
    if (bdds_is_constant_one(cudd, b, n)) {
      bdds_copy(out, a, n);
      return;
    }
    if (bdds_is_constant_neg_one(cudd, b, n)) {
      bdds_mk_2s_complement(cudd, out, a, n);
      return;
    }
    int32_t b_pow = bdds_is_constant_pow2(cudd, b, n);
    if (b_pow != 0) {
      if (b_pow > 0) {
        bdds_mk_shl_const(cudd, out, a, b_pow, n);
      } else {
        bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, n);
        BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);
        bdds_mk_shl_const(cudd, tmp, a, -b_pow, n);
        bdds_mk_2s_complement(cudd, out, tmp, n);
        bdds_clear(cudd, tmp, n);
        bdd_manager_delete_vec(cudd->bddm, tmp_v);
      }
      return;
    }
  }

  bdds_mk_mult_core(cudd, out, a, b, n);
}

void bdds_mk_div_rem(CUDD* cudd, BDD** out_div, BDD** out_rem, BDD** a, BDD** b, uint32_t n) {

  /*
     How to make a long division:

     - input: a[n-1], ..., a[0] / b[n-1], ..., b[0]

     - a_extended = 0, ..., 0, ..., 0, a[n-1], ..., a[n-i], a[0]
     - a_slice    =                  , X     , ..., X
                            -------------------------: n bits
                    b[n-1], ..., b[i], b[i-1], ...., b[0]

     At each step, we check if we can substract b from a_exteneded:
       - b[n-1], ..., b[i] = 0, 0, ...., 0
       - a[n-1], ..., a[n-i] >= b[i-1], ..., b[0]
     If yes, we subtract, and set 1 to division bit
     If no, we keep, and set 0 to division bit

     We do this symbolically with BDDs, while trying to shortcircuit as much
     as possible.
   */

  uint32_t tmp_size = 2*n;
  bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
  BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);

  BDD** a_copy = tmp; // n bits
  BDD** a_slice_sub_b = tmp + n; // n bits at most

  BDD* zero = Cudd_ReadLogicZero(cudd->cudd); ATTACH_REF(zero);
  BDD* one = Cudd_ReadOne(cudd->cudd); ATTACH_REF(one);
  
  bdds_copy(a_copy, a, n);

  for (uint32_t i = 1; i <= n; ++ i) {
    // current slice of a we are working on
    BDD** a_slice = a_copy + n - i;
    // compare a_slice to b:
    // 0000000 == b[n-1:i] && a_slice[i-1:0] >= b[i-1:0]
    BDD* b_slice_eq_0 = NULL;
    BDD* a_slice_ge_b = NULL;
    BDD* can_subtract = NULL;
    if (i < n) {
      bdds_mk_eq0(cudd, &b_slice_eq_0, b+i, n-i);
    } else {
      b_slice_eq_0 = one;
      ATTACH_REF(b_slice_eq_0);
    }
    if (b_slice_eq_0 == zero) {
      can_subtract = zero;
    } else {
      bdds_mk_ge(cudd, &a_slice_ge_b, a_slice, b, i);
      can_subtract = Cudd_bddAnd(cudd->cudd, a_slice_ge_b, b_slice_eq_0);
    }
    ATTACH_REF(can_subtract);
    // record division bit
    if (out_div != NULL) {
      assert(out_div[n-i] == NULL);
      out_div[n-i] = can_subtract;
      ATTACH_REF(out_div[n-i]);
    }
    // Short-circuit if not possible to subtract
    if (can_subtract != zero) {
      // compute a_slice - b
      bdds_mk_2s_complement(cudd, a_slice_sub_b, b, i);
      bdds_mk_plus_in_place(cudd, a_slice_sub_b, a_slice, NULL, i, 0);
      // update slice
      for (uint32_t k = 0; k < i; ++ k) {
        BDD* tmp = a_slice[k];
        a_slice[k] = Cudd_bddIte(cudd->cudd, can_subtract, a_slice_sub_b[k], a_slice[k]);
        ATTACH_REF(a_slice[k]);
        DETACH_REF(cudd->cudd, tmp);
      }
      // remove temp
      bdds_clear(cudd, a_slice_sub_b, i);
    }
    DETACH_REF(cudd->cudd, b_slice_eq_0);
    if (a_slice_ge_b != NULL) {
      DETACH_REF(cudd->cudd, a_slice_ge_b);
    }
    DETACH_REF(cudd->cudd, can_subtract);
  }

  if (out_rem != NULL) {
    bdds_move(out_rem, a_copy, n);
  } else {
    bdds_clear(cudd, a_copy, n);
  }

  bdd_manager_delete_vec(cudd->bddm, tmp_v);

  DETACH_REF(cudd->cudd, zero);
  DETACH_REF(cudd->cudd, one);
}

void bdds_mk_div(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bdds_mk_div_rem(cudd, out, NULL, a, b, n);
}

void bdds_mk_rem(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  bdds_mk_div_rem(cudd, NULL, out, a, b, n);
}

void bdds_mk_ite(CUDD* cudd, BDD** out, BDD* cond, BDD** a, BDD** b, uint32_t n) {
  if (cond == Cudd_ReadOne(cudd->cudd)) {
    bdds_copy(out, a, n);
    return;
  }
  if (cond == Cudd_ReadLogicZero(cudd->cudd)) {
    bdds_copy(out, b, n);
    return;
  }
  for (uint32_t i = 0; i < n; ++ i) {
    assert(out[i] == NULL);
    out[i] = Cudd_bddIte(cudd->cudd, cond, a[i], b[i]);
    ATTACH_REF(out[i]);
  }
}

//    (bvsdiv s t) abbreviates
//      (let ((?msb_s ((_ extract |m-1| |m-1|) s))
//            (?msb_t ((_ extract |m-1| |m-1|) t)))
//        (ite (and (= ?msb_s #b0) (= ?msb_t #b0))
//             (bvudiv s t)
//        (ite (and (= ?msb_s #b1) (= ?msb_t #b0))
//             (bvneg (bvudiv (bvneg s) t))
//        (ite (and (= ?msb_s #b0) (= ?msb_t #b1))
//             (bvneg (bvudiv s (bvneg t)))
//             (bvudiv (bvneg s) (bvneg t))))))
void bdds_mk_sdiv(CUDD* cudd, BDD** out_bdds, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);

  BDD* msb_a = a[n-1];
  BDD* msb_b = b[n-1];

  uint32_t tmp_size = 10*n;

  bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
  BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);

  BDD** case00_result = tmp; // n bits
  BDD** bvneg_a = tmp + n; // n bits
  BDD** bvdiv_bvneg_a_b = tmp + 2*n; // n bits
  BDD** case10_result = tmp + 3*n; // n bits
  BDD** bvneg_b = tmp + 4*n; // n bits
  BDD** bvdiv_a_bvneg_b = tmp + 5*n; // nbits
  BDD** case01_result = tmp + 6*n; // n bits
  BDD** case11_result = tmp + 7*n; // n bits
  BDD** ite1 = tmp + 8*n; // n bits
  BDD** ite2 = tmp + 9*n; // n bits

  BDD* zero = Cudd_ReadLogicZero(cudd->cudd);
  BDD* one = Cudd_ReadOne(cudd->cudd);

  bool can_shortcircuit = Cudd_IsConstant(msb_a) && Cudd_IsConstant(msb_b);
  if (can_shortcircuit) {
    bool msb_a_1 = msb_a == one;
    bool msb_b_1 = msb_b == one;
    if (!msb_a_1 && !msb_b_1) {
      // Case msb_a = 0, msb_b = 0
      bdds_mk_div(cudd, out_bdds, a, b, n);
    } else if (msb_a_1 && !msb_b_1) {
      // Case msb_a = 1, msb_b = 0
      bdds_mk_2s_complement(cudd, bvneg_a, a, n);
      bdds_mk_div(cudd, bvdiv_bvneg_a_b, bvneg_a, b, n);
      bdds_mk_2s_complement(cudd, out_bdds, bvdiv_bvneg_a_b, n);
    } else if (!msb_a_1 && msb_b_1) {
      // Case msb_a = 0, msb_b = 1
      bdds_mk_2s_complement(cudd, bvneg_b, b, n);
      bdds_mk_div(cudd, bvdiv_a_bvneg_b, a, bvneg_b, n);
      bdds_mk_2s_complement(cudd, out_bdds, bvdiv_a_bvneg_b, n);
    } else {
      // Case msb_a = 1, msb_b = 1
      if (bvneg_a[0] == NULL) {
        bdds_mk_2s_complement(cudd, bvneg_a, a, n);
      }
      if (bvneg_b[0] == NULL) {
        bdds_mk_2s_complement(cudd, bvneg_b, b, n);
      }
      bdds_mk_div(cudd, out_bdds, bvneg_a, bvneg_b, n);
    }
  } else {

    // Case msb_a = 0, msb_b = 0
    BDD* case00 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), Cudd_Not(msb_b));
    ATTACH_REF(case00);
    if (case00 != zero) {
      bdds_mk_div(cudd, case00_result, a, b, n);
    }

    // Case msb_a = 1, msb_b = 0
    BDD* case10 = Cudd_bddAnd(cudd->cudd, msb_a, Cudd_Not(msb_b));
    ATTACH_REF(case10);
    if (case10 != zero) {
      bdds_mk_2s_complement(cudd, bvneg_a, a, n);
      bdds_mk_div(cudd, bvdiv_bvneg_a_b, bvneg_a, b, n);
      bdds_mk_2s_complement(cudd, case10_result, bvdiv_bvneg_a_b, n);
    }

    // Case msb_a = 0, msb_b = 1
    BDD* case01 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), msb_b);
    ATTACH_REF(case01);
    if (case01 != zero) {
      bdds_mk_2s_complement(cudd, bvneg_b, b, n);
      bdds_mk_div(cudd, bvdiv_a_bvneg_b, a, bvneg_b, n);
      bdds_mk_2s_complement(cudd, case01_result, bvdiv_a_bvneg_b, n);
    }

    // Case msb_a = 1, msb_b = 1 (BDD not needed, but we make it to shortcircuit)
    BDD* case11 = Cudd_bddAnd(cudd->cudd, msb_a, msb_b);
    ATTACH_REF(case11);
    if (case11 != zero) {
      if (bvneg_a[0] == NULL) {
        bdds_mk_2s_complement(cudd, bvneg_a, a, n);
      }
      if (bvneg_b[0] == NULL) {
        bdds_mk_2s_complement(cudd, bvneg_b, b, n);
      }
      bdds_mk_div(cudd, case11_result, bvneg_a, bvneg_b, n);
    } else {
      // Doesn't happen, just make it 0
      bdds_mk_zero(cudd, case11_result, n);
    }

    // Final ITE result
    bdds_mk_ite(cudd, ite1, case01, case01_result, case11_result, n);
    bdds_mk_ite(cudd, ite2, case10, case10_result, ite1, n);
    bdds_mk_ite(cudd, out_bdds, case00, case00_result, ite2, n);

    // Clear temps
    DETACH_REF(cudd->cudd, case00);
    DETACH_REF(cudd->cudd, case10);
    DETACH_REF(cudd->cudd, case01);
    DETACH_REF(cudd->cudd, case11);

  }

  bdds_clear(cudd, tmp, tmp_size);
  bdd_manager_delete_vec(cudd->bddm, tmp_v);
}

//   (bvsrem s t) abbreviates
//     (let ((?msb_s ((_ extract |m-1| |m-1|) s))
//           (?msb_t ((_ extract |m-1| |m-1|) t)))
//       (ite (and (= ?msb_s #b0) (= ?msb_t #b0))
//            (bvurem s t)
//       (ite (and (= ?msb_s #b1) (= ?msb_t #b0))
//            (bvneg (bvurem (bvneg s) t))
//       (ite (and (= ?msb_s #b0) (= ?msb_t #b1))
//            (bvurem s (bvneg t)))
//            (bvneg (bvurem (bvneg s) (bvneg t))))))
void bdds_mk_srem(CUDD* cudd, BDD** out_bdds, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);

  BDD* msb_a = a[n-1];
  BDD* msb_b = b[n-1];

  uint32_t tmp_size = 10*n;
  bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
  BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);

  BDD** case00_result = tmp; // n bits
  BDD** bvneg_a = tmp + n; // n bits
  BDD** bvrem_bvneg_a_b = tmp + 2*n; // n bits
  BDD** case10_result = tmp + 3*n; // n bits
  BDD** bvneg_b = tmp + 4*n; // n bits
  BDD** case01_result = tmp + 5*n; // n bits
  BDD** bvrem_bvneg_a_bvneg_b = tmp + 6*n; // nbits
  BDD** case11_result = tmp + 7*n; // n bits
  BDD** ite1 = tmp + 8*n; // n bits
  BDD** ite2 = tmp + 9*n; // n bits

  // Case msb_a = 0, msb_b = 0 -> (bvurem s t)
  BDD* case00 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), Cudd_Not(msb_b));
  ATTACH_REF(case00);
  bdds_mk_rem(cudd, case00_result, a, b, n);

  // Case msb_a = 1, msb_b = 0 -> (bvneg (bvurem (bvneg s) t))
  BDD* case10 = Cudd_bddAnd(cudd->cudd, msb_a, Cudd_Not(msb_b));
  ATTACH_REF(case10);
  bdds_mk_2s_complement(cudd, bvneg_a, a, n);
  bdds_mk_rem(cudd, bvrem_bvneg_a_b, bvneg_a, b, n);
  bdds_mk_2s_complement(cudd, case10_result, bvrem_bvneg_a_b, n);

  // Case msb_a = 0, msb_b = 1 -> (bvurem s (bvneg t)))
  BDD* case01 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), msb_b);
  ATTACH_REF(case01);
  bdds_mk_2s_complement(cudd, bvneg_b, b, n);
  bdds_mk_rem(cudd, case01_result, a, bvneg_b, n);

  // Case msb_a = 1, msb_b = 1 -> (bvneg (bvurem (bvneg s) (bvneg t)))
  bdds_mk_rem(cudd, bvrem_bvneg_a_bvneg_b, bvneg_a, bvneg_b, n);
  bdds_mk_2s_complement(cudd, case11_result, bvrem_bvneg_a_bvneg_b, n);

  // Final ITE result
  bdds_mk_ite(cudd, ite1, case01, case01_result, case11_result, n);
  bdds_mk_ite(cudd, ite2, case10, case10_result, ite1, n);
  bdds_mk_ite(cudd, out_bdds, case00, case00_result, ite2, n);

  // Clear temps
  DETACH_REF(cudd->cudd, case00);
  DETACH_REF(cudd->cudd, case10);
  DETACH_REF(cudd->cudd, case01);
  bdds_clear(cudd, tmp, tmp_size);
  bdd_manager_delete_vec(cudd->bddm, tmp_v);
}

//    (bvsmod s t) abbreviates
//      (let ((?msb_s ((_ extract |m-1| |m-1|) s))
//            (?msb_t ((_ extract |m-1| |m-1|) t)))
//        (let ((abs_s (ite (= ?msb_s #b0) s (bvneg s)))
//              (abs_t (ite (= ?msb_t #b0) t (bvneg t))))
//          (let ((u (bvurem abs_s abs_t)))
//            (ite (= u (_ bv0 m))
//                 u
//            (ite (and (= ?msb_s #b0) (= ?msb_t #b0))
//                 u
//            (ite (and (= ?msb_s #b1) (= ?msb_t #b0))
//                 (bvadd (bvneg u) t)
//            (ite (and (= ?msb_s #b0) (= ?msb_t #b1))
//                 (bvadd u t)
//                 (bvneg u))))))))
void bdds_mk_smod(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);

  BDD* msb_a = a[n-1];
  BDD* msb_b = b[n-1];

  uint32_t tmp_size = 11*n;
  bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
  BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);

  // Temporary storage
  BDD** bvneg_a = tmp;
  BDD** bvneg_b = tmp + n;
  BDD** abs_a = tmp + 2*n;
  BDD** abs_b = tmp + 3*n;
  BDD** u = tmp + 4*n;
  BDD** bvadd_u_b = tmp + 5*n;
  BDD** bvneg_u = tmp + 6*n;
  BDD** bvadd_bvneg_u_b = tmp + 7*n;
  BDD** ite2 = tmp + 8*n;
  BDD** ite3 = tmp + 9*n;
  BDD** ite4 = tmp + 10*n;

  // All the intermediary terms
  bdds_mk_2s_complement(cudd, bvneg_a, a, n);
  bdds_mk_2s_complement(cudd, bvneg_b, b, n);
  bdds_mk_ite(cudd, abs_a, msb_a, bvneg_a, a, n);
  bdds_mk_ite(cudd, abs_b, msb_b, bvneg_b, b, n);
  bdds_mk_rem(cudd, u, abs_a, abs_b, n);
  bdds_mk_plus(cudd, bvadd_u_b, u, b, n);
  bdds_mk_2s_complement(cudd, bvneg_u, u, n);
  bdds_mk_plus(cudd, bvadd_bvneg_u_b, bvneg_u, b, n);

  // ITE conditions
  BDD* cond1 = NULL;
  bdds_mk_eq0(cudd, &cond1, u, n);
  BDD* cond2 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), Cudd_Not(msb_b));
  ATTACH_REF(cond2);
  BDD* cond3 = Cudd_bddAnd(cudd->cudd, msb_a, Cudd_Not(msb_b));
  ATTACH_REF(cond3);
  BDD* cond4 = Cudd_bddAnd(cudd->cudd, Cudd_Not(msb_a), msb_b);
  ATTACH_REF(cond4);

  // (ite (and (= ?msb_s #b0) (= ?msb_t #b1))
  //   (bvadd u t) (bvneg u))
  bdds_mk_ite(cudd, ite4, cond4, bvadd_u_b, bvneg_u, n);
  // (ite (and (= ?msb_s #b1) (= ?msb_t #b0))
  //  (bvadd (bvneg u) t) ite4)
  bdds_mk_ite(cudd, ite3, cond3, bvadd_bvneg_u_b, ite4, n);
  // (ite (and (= ?msb_s #b0) (= ?msb_t #b0))
  //   u ite3)
  bdds_mk_ite(cudd, ite2, cond2, u, ite3, n);
  // (ite (= u (_ bv0 m)) u ite2)
  bdds_mk_ite(cudd, out, cond1, u, ite2, n);

  // Clear temps
  DETACH_REF(cudd->cudd, cond1);
  DETACH_REF(cudd->cudd, cond2);
  DETACH_REF(cudd->cudd, cond3);
  DETACH_REF(cudd->cudd, cond4);
  bdds_clear(cudd, tmp, tmp_size);
  bdd_manager_delete_vec(cudd->bddm, tmp_v);
}

bddvec_t bdds_compute_bdds(CUDD* cudd, term_table_t* terms, term_t t, const pvector_t* children_bdds) {

  assert(bv_term_has_children(terms, t));

  BDD** t0;
  BDD** t1;

  // Default, just make the same size
  uint32_t t_bitsize = bv_term_bitsize(terms, t);
  bddvec_t result_v = bdd_manager_new_vec(cudd->bddm, t_bitsize);
  BDD** result_bdds = bdd_manager_get_bdds(cudd->bddm, result_v);

  if (is_neg_term(t)) {
    // Negation
    assert(children_bdds->size == 1);
    t0 = (BDD**) children_bdds->data[0];
    bdds_mk_not(cudd, result_bdds, t0, t_bitsize);
  } else {
    term_kind_t kind = term_kind(terms, t);
    switch (kind) {
    case BV_DIV:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_div(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_REM:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_rem(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_SDIV:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_sdiv(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_SREM:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_srem(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_SMOD:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_smod(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_SHL:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_shl(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_LSHR:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_lshr(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_ASHR:
      assert(children_bdds->size == 2);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_ashr(cudd, result_bdds, t0, t1, t_bitsize);
      break;
    case BV_ARRAY: {
      assert(children_bdds->size == t_bitsize);
      for (uint32_t i = 0; i < children_bdds->size; ++ i) {
        assert(result_bdds[i] == NULL);
        result_bdds[i] = ((BDD**) children_bdds->data[i])[0];
        ATTACH_REF(result_bdds[i]);
      }
      break;
    }
    case BIT_TERM: {
      assert(t_bitsize == 1);
      assert(children_bdds->size == 1);
      select_term_t* desc = bit_term_desc(terms, t);
      uint32_t select_idx = desc->idx;
      BDD** child_bdds = ((BDD**)children_bdds->data[0]);
      BDD* bit_bdd = child_bdds[select_idx];
      assert(result_bdds[0] == NULL);
      result_bdds[0] = bit_bdd;
      ATTACH_REF(result_bdds[0]);
      break;
    }
    case BV_POLY: {
      uint32_t tmp_size = 2*t_bitsize;
      bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
      BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);
      BDD** const_bdds = tmp;
      BDD** mult_bdds = tmp + t_bitsize;
      bdds_mk_zero(cudd, result_bdds, t_bitsize);
      bvpoly_t* p = bvpoly_term_desc(terms, t);
      for (uint32_t i = 0, child_i = 0; i < p->nterms; ++ i) {
        uint32_t* c = p->mono[i].coeff;
        bdds_mk_constant_raw(cudd, const_bdds, t_bitsize, c);
        if (p->mono[i].var == const_idx) {
          // Just constant: out += c
          bdds_mk_plus_in_place(cudd, result_bdds, const_bdds, NULL, t_bitsize, 0);
          bdds_clear(cudd, const_bdds, t_bitsize);
          continue;
        } else {
          // Non constant: out += c*x
          BDD** child_bdds = ((BDD**)children_bdds->data[child_i]);
          bdds_mk_mult(cudd, mult_bdds, child_bdds, const_bdds, t_bitsize);
          bdds_mk_plus_in_place(cudd, result_bdds, mult_bdds, NULL, t_bitsize, 0);
          bdds_clear(cudd, const_bdds, t_bitsize);
          bdds_clear(cudd, mult_bdds, t_bitsize);
          ++ child_i;
        }
      }
      bdd_manager_delete_vec(cudd->bddm, tmp_v);
      break;
    }
    case BV64_POLY: {
      uint32_t tmp_size = 2*t_bitsize;
      bddvec_t tmp_v = bdd_manager_new_vec(cudd->bddm, tmp_size);
      BDD** tmp = bdd_manager_get_bdds(cudd->bddm, tmp_v);
      BDD** const_bdds = tmp;
      BDD** mult_bdds = tmp + t_bitsize;
      bdds_mk_zero(cudd, result_bdds, t_bitsize);
      bvpoly64_t* p = bvpoly64_term_desc(terms, t);
      for (uint32_t i = 0, child_i = 0; i < p->nterms; ++ i) {
        uint64_t c = p->mono[i].coeff;
        bdds_mk_constant_64(cudd, const_bdds, t_bitsize, c);
        if (p->mono[i].var == const_idx) {
          // Just constant: out += c
          bdds_mk_plus_in_place(cudd, result_bdds, const_bdds, NULL, t_bitsize, 0);
          bdds_clear(cudd, const_bdds, t_bitsize);
          continue;
        } else {
          // Non constant: out += c*x
          BDD** child_bdds = ((BDD**)children_bdds->data[child_i]);
          bdds_mk_mult(cudd, mult_bdds, child_bdds, const_bdds, t_bitsize);
          bdds_mk_plus_in_place(cudd, result_bdds, mult_bdds, NULL, t_bitsize, 0);
          bdds_clear(cudd, const_bdds, t_bitsize);
          bdds_clear(cudd, mult_bdds, t_bitsize);
          ++ child_i;
        }
      }
      bdd_manager_delete_vec(cudd->bddm, tmp_v);
      break;
    }
    case POWER_PRODUCT: {
      bddvec_t mult_bdds_v = bdd_manager_new_vec(cudd->bddm, t_bitsize);
      BDD** mult_bdds = bdd_manager_get_bdds(cudd->bddm, mult_bdds_v);
      bdds_mk_one(cudd, result_bdds, t_bitsize);
      pprod_t* t_pprod = pprod_term_desc(terms, t);
      for (uint32_t i = 0; i < t_pprod->len; ++ i) {
        uint32_t exp = t_pprod->prod[i].exp;
        BDD** child_bdds = ((BDD**)children_bdds->data[i]);
        for (uint32_t d = 0; d < exp; ++ d) {
          bdds_mk_mult(cudd, mult_bdds, result_bdds, child_bdds, t_bitsize);
          bdds_swap(mult_bdds, result_bdds, t_bitsize);
          bdds_clear(cudd, mult_bdds, t_bitsize);
        }
      }
      bdd_manager_delete_vec(cudd->bddm, mult_bdds_v);
      break;
    }
    case OR_TERM: {
      assert(children_bdds->size == or_term_desc(terms, t)->arity);
      bdds_mk_bool_or(cudd, result_bdds, children_bdds);
      break;
    }
    case EQ_TERM: // Boolean equality
    case BV_EQ_ATOM: {
      assert(children_bdds->size == 2);
      composite_term_t* comp = composite_term_desc(terms, t);
      term_t child = comp->arg[0];
      uint32_t child_bitsize = bv_term_bitsize(terms, child);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      // Make by hand, so that we can optimize
      bdd_manager_delete_vec(cudd->bddm, result_v);
      result_v = bdds_mk_eq_manual(cudd, t0, t1, child_bitsize);
      break;
    }
    case BV_GE_ATOM: {
      assert(children_bdds->size == 2);
      composite_term_t* comp = composite_term_desc(terms, t);
      term_t child = comp->arg[0];
      uint32_t child_bitsize = bv_term_bitsize(terms, child);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_ge(cudd, result_bdds, t0, t1, child_bitsize);
      break;
    }
    case BV_SGE_ATOM: {
      assert(children_bdds->size == 2);
      composite_term_t* comp = composite_term_desc(terms, t);
      term_t child = comp->arg[0];
      uint32_t child_bitsize = bv_term_bitsize(terms, child);
      t0 = (BDD**) children_bdds->data[0];
      t1 = (BDD**) children_bdds->data[1];
      bdds_mk_sge(cudd, result_bdds, t0, t1, child_bitsize);
      break;
    }
    default:
      // Not composite
      assert(false);
      break;
    }
  }

  return result_v;
}

void bdds_mk_ge(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  assert(n > 0);
  assert(out[0] == NULL);
  
  // Reverse to satisfy CUDD
  bdds_reverse(a, n);
  bdds_reverse(b, n);
  // a < b
  out[0] = Cudd_Xgty(cudd->cudd, n, NULL, b, a);
  ATTACH_REF(out[0]);
  // a >= b
  out[0] = Cudd_Not(out[0]);
  // Undo reversal
  bdds_reverse(a, n);
  bdds_reverse(b, n);
}

void bdds_mk_sge(CUDD* cudd, BDD** out, BDD** a, BDD** b, uint32_t n) {
  // signed comparison, just invert the first bits
  assert(n > 0);
  a[n-1] = Cudd_Not(a[n-1]);
  b[n-1] = Cudd_Not(b[n-1]);
  bdds_mk_ge(cudd, out, a, b, n);
  a[n-1] = Cudd_Not(a[n-1]);
  b[n-1] = Cudd_Not(b[n-1]);
}

bool bdds_is_point(CUDD* cudd, BDD** a, uint32_t n, uint32_t bitsize) {
  uint32_t i;
  uint32_t sum = 0;
  for (i = 0; i < n; i ++) {
    if (!Cudd_CheckCube(cudd->cudd, a[i])) {
      return false;
    }
    int dag_size = Cudd_DagSize(a[i]);
    sum += dag_size - 1;
  }
  return sum == bitsize;
}

bool bdds_is_model(CUDD* cudd, BDD** x, BDD* C_x, const bvconstant_t* x_value) {
  for (uint32_t i = 0; i < x_value->bitsize; ++ i) {
    unsigned int x_i = Cudd_NodeReadIndex(x[i]);
    bool bit_i_true = bvconst_tst_bit(x_value->data, i);
    cudd->tmp_inputs[x_i] = bit_i_true ? 1 : 0;
  }
  return Cudd_Eval(cudd->cudd, C_x, cudd->tmp_inputs) == Cudd_ReadOne(cudd->cudd);
}

typedef enum {
  PREFER_ZERO,
  PREFER_ONE,
  PREFER_RANDOM,
  PREFER_SHORT_ZERO
} pick_type_t;

/**
  Adapted from CUDD to control the picked value.

  @brief Picks one on-set cube randomly from the given %DD.
  @details The cube is written into an array of characters.  The array
  must have at least as many entries as there are variables.
  @return 1 if successful; 0 otherwise.
  @sideeffect None
  @see Cudd_bddPickOneMinterm
*/
int
bdds_Cudd_bddPickOneCube(CUDD* cudd, DdNode * node, pick_type_t pick)
{
  DdNode *N, *T, *E;
  DdNode *one, *bzero;
  char dir;
  int i;

  /* The constant 0 function has no on-set cubes. */
  one = Cudd_ReadOne(cudd->cudd);
  bzero = Cudd_Not(one);

  for (i = 0; i < cudd->tmp_alloc_size; i++)
    cudd->tmp_model[i] = 2;

  for (;;) {

    if (node == one)
      break;

    N = Cudd_Regular(node);

    T = Cudd_T(N);
    E = Cudd_E(N);
    if (Cudd_IsComplement(node)) {
      T = Cudd_Not(T);
      E = Cudd_Not(E);
    }

    unsigned int N_index = Cudd_NodeReadIndex(N);
    if (T == bzero) {
      cudd->tmp_model[N_index] = 0;
      node = E;
    } else if (E == bzero) {
      cudd->tmp_model[N_index] = 1;
      node = T;
    } else if (pick == PREFER_SHORT_ZERO && T == one) {
      cudd->tmp_model[N_index] = 1;
      node = T;
    } else if (pick == PREFER_SHORT_ZERO && E == one) {
      cudd->tmp_model[N_index] = 0;
      node = E;
    } else {
      switch (pick) {
      case PREFER_ZERO:
      case PREFER_SHORT_ZERO:
        dir = 0;
        break;
      case PREFER_ONE:
        dir = 1;
        break;
      case PREFER_RANDOM:
      default: // BD: GCC warning
        dir = (char) ((Cudd_Random(cudd->cudd) & 0x2000) >> 13);
        break;
      }
      cudd->tmp_model[N_index] = dir;
      node = dir ? T : E;
    }
  }
  return (1);

} /* end of Cudd_bddPickOneCube */

void bdds_get_model(CUDD* cudd, BDD** x, BDD* C_x, bvconstant_t* out) {
  bdds_Cudd_bddPickOneCube(cudd, C_x, PREFER_SHORT_ZERO);
  // Set the ones in the cube
  for (uint32_t i = 0; i < out->bitsize; ++ i) {
    unsigned int x_i = Cudd_NodeReadIndex(x[i]);
    char x_i_value = cudd->tmp_model[x_i];
    if (x_i_value == 1) {
      bvconst_set_bit(out->data, i);
    } else if (x_i_value == 0) {
      // We just take 0 as the default
      bvconst_clr_bit(out->data, i);
    } else {
      assert(x_i_value == 2);
    }
  }
}

void bdds_disjoint_set_add(CUDD* cudd, BDD** a, uint32_t n, pvector_t* set) {
  uint32_t a_i, set_i;
  bool done = false;

  assert(n > 0);

  BDD* bdd0 = Cudd_ReadLogicZero(cudd->cudd);
  BDD* bdd1 = Cudd_ReadOne(cudd->cudd);

  // Special case a[i] == false, do empty set
  for (a_i = 0; a_i < n; ++ a_i) {
    if (a[a_i] == bdd0) {
      bdds_clear(cudd, (BDD**) set->data, set->size);
      pvector_reset(set);
      pvector_push(set, bdd0);
      ATTACH_REF(bdd0);
      return;
    }
  }

  // Special case set[0] = false, do nothing
  if (set->size > 0 && set->data[0] == bdd0) {
    assert(set->size == 1);
    return;
  }

  // Special case set[0] = true, remove it and just keep adding a
  if (set->size > 0 && set->data[0] == bdd1) {
    assert(set->size == 1);
    bdds_clear(cudd, (BDD**) set->data, set->size);
    pvector_reset(set);
  }

  // Add the BDDs from a. For each BDD go through set and see if they are
  // disjoint. If yes, then we skip it. If not we need to intersect it and
  // continue with the intersection. True and false constants are treated
  // separately to ensure that T/F are size 1 vectors.
  for (a_i = 0; !done && a_i < n; ++ a_i) {
    BDD* to_add = a[a_i];
    // Special case: a[i] == true, do nothing
    if (a[a_i] == bdd1) {
      continue;
    }
    assert(to_add != bdd0 && to_add != bdd1);
    ATTACH_REF(to_add); // attach, we're copying
    for (set_i = 0; !done && set_i < set->size;) {
      // The BDD to_add is disjoint from all BDDs in set at positions < j
      BDD* set_i_bdd = (BDD*) set->data[set_i];
      assert(set_i_bdd != bdd0 && set_i_bdd != bdd1);
      // If to_add is disjoint from set[j], we can continue
      if (bdds_are_disjoint(cudd, to_add, set_i_bdd)) {
        // Disjoint, we can continue
        set_i ++;
        continue;
      }
      // If to_add is not disjoint from set[j], we intersect with it and keep
      // going with the rest of the set
      BDD* conjunction = Cudd_bddAnd(cudd->cudd, set_i_bdd, to_add);
      ATTACH_REF(conjunction);
      DETACH_REF(cudd->cudd, to_add);
      to_add = conjunction;
      if (to_add == bdd0) {
        // just return the false BDD
        bdds_clear(cudd, (BDD**) set->data, set->size);
        pvector_reset(set);
        done = true;
      } else {
        // Shrink the vector by moving the last one into position j. If
        // no such element, then we're done with adding this element
        DETACH_REF(cudd->cudd, set_i_bdd);
        BDD* last = pvector_pop2(set);
        if (set_i < set->size) { set->data[set_i] = last; }
      }
    }
    // We have a BDD disjoint from everything in set
    pvector_push(set, to_add);
  }

  // Empty set, just set as true
  if (set->size == 0) {
    pvector_push(set, bdd1);
    ATTACH_REF(bdd1);

  }
}
