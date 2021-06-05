#include "handle.h"

#include <ql/quote.hpp>
#include <ql/instrument.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>


namespace ql = QuantLib;
namespace py = pybind11;
using namespace pybind11::literals;

void init_submodule_handle(pybind11::module& m) {

  auto sub = m.def_submodule("_handles");

  py::class_<ql::Handle<ql::Quote>>(sub, "_quote")
    .def(py::init<std::shared_ptr<ql::Quote>>())
    .def("link", &ql::Handle<ql::Quote>::currentLink)
    .def("__str__", [](const ql::Handle<ql::Quote>& q) {
        std::stringstream ss; ss 
          << q->value()
          ;
        
        return ss.str();
      }
    )
    ;

  py::class_<ql::RelinkableHandle<ql::Quote>, ql::Handle<ql::Quote>>(sub, "quote")
    .def(py::init<std::shared_ptr<ql::Quote>>())
    .def("linkto", [](ql::RelinkableHandle<ql::Quote>& hdl, const std::shared_ptr<ql::Quote>& qt, bool reg) {
        hdl.linkTo(qt, reg);
      }
      , py::arg("quote")
      , py::arg("register") = true
    )
    ;

  py::class_<ql::Handle<ql::YieldTermStructure>>(sub, "yieldtermstructure")
    .def(py::init<std::shared_ptr<ql::YieldTermStructure>>())
    .def("link", &ql::Handle<ql::YieldTermStructure>::currentLink)
    ;

  py::class_<ql::Handle<ql::BlackVolTermStructure>>(sub, "blackvoltermstructure")
    .def(py::init<std::shared_ptr<ql::BlackVolTermStructure>>())
    .def("link", &ql::Handle<ql::BlackVolTermStructure>::currentLink)
    ;
}