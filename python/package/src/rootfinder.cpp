#include "rootfinder.h"

#include <ql/instrument.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/math/solvers1d/brent.hpp>

namespace ql = QuantLib;

rootfinder::rootfinder(
    const std::shared_ptr<ql::Instrument>& in
  , const std::shared_ptr<ql::SimpleQuote>& qt)
  : m_in(in), m_qt(qt) {}

double rootfinder::solve(
    double guess
  , double tgt
  , double acc
  , double xmax
  , double xmin) {

  ql::Brent solver;
  auto prev = m_qt->value();
  double result = std::numeric_limits<double>::quiet_NaN();

  try {

    auto cost = [&](double x) {
      m_qt->setValue(x);
      auto value = m_in->NPV();
      return tgt - value;
    };

    result = solver.solve(cost, acc, m_qt->value(), xmin, xmax); 
    
  } catch(const std::exception& ex) {
    m_qt->setValue(prev);
    throw ex;
  }

  m_qt->setValue(prev);
  return result;
}