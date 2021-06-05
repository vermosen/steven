#include "solver.h"

#include <ql/instrument.hpp>
#include <ql/quotes/simplequote.hpp>

#include <steven/math/rootfinder.h>

namespace ql = QuantLib;
namespace py = pybind11;
using namespace pybind11::literals;

void init_submodule_solver(pybind11::module& m) {

  auto sub = m.def_submodule("_solvers");

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