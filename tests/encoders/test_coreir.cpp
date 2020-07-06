#ifdef WITH_COREIR

#include <string>
#include <tuple>
#include <vector>

#include "gtest/gtest.h"

#include "available_solvers.h"
#include "test_encoder_inputs.h"

#include "core/rts.h"
#include "frontends/coreir_encoder.h"

using namespace pono;
using namespace smt;
using namespace std;

namespace pono_tests {

class CoreIRUnitTests : public ::testing::Test,
                        public ::testing::WithParamInterface<tuple<SolverEnum, string>>
{};

TEST_P(CoreIRUnitTests, Encode)
{
  SmtSolver s = available_solvers().at(get<0>(GetParam()))(false);
  RelationalTransitionSystem rts(s);
  // PONO_SRC_DIR is a macro set using CMake PROJECT_SRC_DIR
  string filename = STRFY(PONO_SRC_DIR);
  filename += "/tests/encoders/inputs/coreir/";
  filename += get<1>(GetParam());
  cout << "Reading file: " << filename << endl;
  CoreIREncoder ce(filename, rts);
}

INSTANTIATE_TEST_SUITE_P(
    ParameterizedSolverCoreIRUnitTests,
    CoreIRUnitTests,
    testing::Combine(testing::ValuesIn(available_solver_enums()),
                     // from test_encoder_inputs.h
                     testing::ValuesIn(coreir_inputs)));

}  // namespace pono_tests
#endif
