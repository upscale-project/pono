/*********************                                                  */
/*! \file mbic3.h
** \verbatim
** Top contributors (to current version):
**   Makai Mann, Ahmed Irfan
** This file is part of the pono project.
** Copyright (c) 2019 by the authors listed in the file AUTHORS
** in the top-level source directory) and their institutional affiliations.
** All rights reserved.  See the file LICENSE in the top-level source
** directory for licensing information.\endverbatim
**
** \brief Simple implementation of IC3 using model values
**
**/

#pragma once

#include "engines/ic3base.h"

namespace pono {

class ModelBasedIC3 : public IC3Base
{
 public:
  ModelBasedIC3(Property & p, smt::SolverEnum se);
  ModelBasedIC3(Property & p, const smt::SmtSolver & s);
  ModelBasedIC3(const PonoOptions & opt, Property & p, smt::SolverEnum se);
  ModelBasedIC3(const PonoOptions & opt,
                Property & p,
                const smt::SmtSolver & s);
  virtual ~ModelBasedIC3() {}

  typedef IC3Base super;

 protected:
  // for mbic3_indgen_mode == 2
  // interpolant based generalization
  smt::SmtSolver interpolator_;
  std::unique_ptr<smt::TermTranslator> to_interpolator_;
  std::unique_ptr<smt::TermTranslator> to_solver_;

  // pure virtual method implementations

  IC3Formula get_ic3_formula(smt::TermVec * out_inputs = nullptr,
                             smt::TermVec * out_nexts = nullptr) const override;

  bool ic3_formula_check_valid(const IC3Formula & u) const override;

  std::vector<IC3Formula> inductive_generalization(
      size_t i, const IC3Formula & c) override;

  IC3Formula generalize_predecessor(size_t i, const IC3Formula & c) override;

  void check_ts() const override;

  bool intersects_bad() override;

  void initialize() override;
};
}  // namespace pono
