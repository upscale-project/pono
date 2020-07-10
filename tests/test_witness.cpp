#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "core/fts.h"
#include "core/rts.h"
#include "core/unroller.h"
#include "engines/bmc.h"
#include "engines/bmc_simplepath.h"
#include "engines/interpolantmc.h"
#include "engines/kinduction.h"
#include "utils/exceptions.h"

#include "available_solvers.h"

using namespace pono;
using namespace smt;
using namespace std;

namespace pono_tests {

class WitnessUnitTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<SolverEnum>
{};

TEST_P(WitnessUnitTests, SimpleDefaultSolver)
{
  // use default solver
  FunctionalTransitionSystem fts;
  Sort bvsort8 = fts.make_sort(BV, 8);
  Sort boolsort = fts.make_sort(BOOL);
  Term one = fts.make_term(1, bvsort8);
  Term eight = fts.make_term(8, bvsort8);
  Term x = fts.make_statevar("x", bvsort8);

  fts.set_init(fts.make_term(Equal, x, fts.make_term(0, bvsort8)));
  fts.assign_next(x, fts.make_term(BVAdd, x, one));

  Term prop_term = fts.make_term(BVUlt, x, eight);
  Property prop(fts, prop_term);

  Bmc bmc(prop, GetParam());
  ProverResult r = bmc.check_until(9);
  ASSERT_EQ(r, FALSE);

  vector<UnorderedTermMap> witness;
  bool ok = bmc.witness(witness);
  ASSERT_TRUE(ok);
  ASSERT_EQ(witness.size(), 9);
  ASSERT_NE(witness[8].find(x), witness[8].end());
  ASSERT_EQ(witness[8][x], eight);
}

INSTANTIATE_TEST_SUITE_P(ParameterizedWitnessUnitTests,
                         WitnessUnitTests,
                         testing::ValuesIn(available_solver_enums()));

}
