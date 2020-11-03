#ifdef WITH_MSAT
// Only run this test with MathSAT

#include <utility>
#include <vector>

#include "core/fts.h"
#include "engines/ic3ia.h"
#include "gtest/gtest.h"
#include "smt/available_solvers.h"
#include "utils/ts_analysis.h"

using namespace pono;
using namespace smt;
using namespace std;

namespace pono_tests {

class IC3IAUnitTests : public ::testing::Test,
                       public ::testing::WithParamInterface<SolverEnum>
{
 protected:
  void SetUp() override
  {
    s = create_solver(GetParam());
    s->set_opt("incremental", "true");
    s->set_opt("produce-models", "true");
    s->set_opt("produce-unsat-cores", "true");
    boolsort = s->make_sort(BOOL);
    bvsort8 = s->make_sort(BV, 8);
    intsort = s->make_sort(INT);
  }
  SmtSolver s;
  Sort boolsort, bvsort8, intsort;
};

TEST_P(IC3IAUnitTests, SimpleSystemSafe)
{
  FunctionalTransitionSystem fts(s);
  Term s1 = fts.make_statevar("s1", boolsort);
  Term s2 = fts.make_statevar("s2", boolsort);

  // INIT !s1 & !s2
  fts.constrain_init(s->make_term(Not, s1));
  fts.constrain_init(s->make_term(Not, s2));

  // TRANS next(s1) = (s1 | s2)
  // TRANS next(s2) = s2
  fts.assign_next(s1, s->make_term(Or, s1, s2));
  fts.assign_next(s2, s2);

  Property p(fts, s->make_term(Not, s1));

  IC3IA ic3ia(p, s);
  ProverResult r = ic3ia.prove();
  ASSERT_EQ(r, TRUE);

  // get the invariant
  Term invar = ic3ia.invar();
  ASSERT_TRUE(check_invar(fts, p.prop(), invar));
}

TEST_P(IC3IAUnitTests, SimpleSystemUnsafe)
{
  FunctionalTransitionSystem fts(s);
  Term s1 = fts.make_statevar("s1", boolsort);
  Term s2 = fts.make_statevar("s2", boolsort);

  // INIT !s1 & s2
  fts.constrain_init(s->make_term(Not, s1));
  fts.constrain_init(s2);

  // TRANS next(s1) = (s1 | s2)
  // TRANS next(s2) = s2
  fts.assign_next(s1, s->make_term(Or, s1, s2));
  fts.assign_next(s2, s2);

  Property p(fts, s->make_term(Not, s1));

  IC3IA ic3ia(p, s);
  ProverResult r = ic3ia.prove();
  ASSERT_EQ(r, FALSE);
}

TEST_P(IC3IAUnitTests, InductiveIntSafe)
{
  FunctionalTransitionSystem fts(s);
  Term x = fts.make_statevar("x", intsort);

  fts.constrain_init(fts.make_term(Equal, x, fts.make_term(0, intsort)));
  fts.assign_next(
      x,
      fts.make_term(Ite,
                    fts.make_term(Lt, x, fts.make_term(10, intsort)),
                    fts.make_term(Plus, x, fts.make_term(1, intsort)),
                    fts.make_term(0, intsort)));

  Property p(fts, fts.make_term(Le, x, fts.make_term(10, intsort)));

  IC3IA ic3ia(p, s);
  ProverResult r = ic3ia.prove();
  ASSERT_EQ(r, TRUE);

  Term invar = ic3ia.invar();
  ASSERT_TRUE(check_invar(fts, p.prop(), invar));
}

TEST_P(IC3IAUnitTests, SimpleIntSafe)
{
  RelationalTransitionSystem rts(s);
  Term x = rts.make_statevar("x", intsort);
  Term y = rts.make_statevar("y", intsort);

  rts.constrain_init(rts.make_term(Equal, x, rts.make_term(0, intsort)));
  rts.constrain_init(rts.make_term(Equal, y, rts.make_term(0, intsort)));

  // x' > x
  rts.constrain_trans(rts.make_term(Gt, rts.next(x), x));
  // y' = y + (x' - x)
  rts.constrain_trans(rts.make_term(
      Equal,
      rts.next(y),
      rts.make_term(Plus, y, rts.make_term(Minus, rts.next(x), x))));
  Term wit = rts.make_statevar("propwit", boolsort);
  rts.constrain_init(wit);
  rts.assign_next(wit, rts.make_term(Equal, x, y));

  Property p(rts, wit);

  IC3IA ic3ia(p, s);
  ProverResult r = ic3ia.prove();
  ASSERT_EQ(r, TRUE);

  Term invar = ic3ia.invar();
  std::cout << "got invar " << invar << std::endl;
  ASSERT_TRUE(check_invar(rts, p.prop(), invar));
}

INSTANTIATE_TEST_SUITE_P(
    ParameterizedSolverIC3IAUnitTests,
    IC3IAUnitTests,
    // only using MathSAT for now, but could be more general in the future
    testing::ValuesIn({ MSAT }));
}  // namespace pono_tests

#endif
