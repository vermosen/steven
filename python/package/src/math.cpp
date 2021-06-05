#include "math.h"

#include <ql/instrument.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/math/optimization/simplex.hpp>

#include <steven/math/rootfinder.h>

namespace ql = QuantLib;
namespace py = pybind11;
using namespace pybind11::literals;

void init_submodule_math(pybind11::module& m) {

  auto sub = m.def_submodule("_math");

  py::class_<ql::OptimizationMethod>(sub, "optimizationmethod")
    ;

  py::class_<ql::Simplex>(sub, "simplex")
    .def(py::init<ql::Real>()
    , py::arg("lambda") = 1.0)
    ;
  
  py::class_<steven::rootfinder>(sub, "rootfinder")
    .def(py::init<const std::shared_ptr<ql::Instrument>&, const std::shared_ptr<ql::SimpleQuote>&>()
      , py::arg("instrument")
      , py::arg("quote")
    )
    .def("solve", [](steven::rootfinder& rf
          , double guess, double npv
          , double acc, double xmax
          , double xmin, bool verbose) {

        try {
          return rf.solve(guess, npv, acc, xmax, xmin);
        } catch(const std::exception& ex) {
            
          if (verbose) {
            py::print(ex.what());
          }
          throw ex;
        }
      }
      , py::arg("guess")
      , py::arg("npv")
      , py::arg("accuracy") = 1e-12
      , py::arg("xmax")     = 10
      , py::arg("xmin")     = 1e-2
      , py::arg("verbose")  = false
    )
    ;
}