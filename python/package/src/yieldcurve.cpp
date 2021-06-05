#include "yieldcurve.h"

#include <pybind11/eigen.h>
#include <ql/instruments/bond.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/fittedbonddiscountcurve.hpp>

namespace ql = QuantLib;
namespace py = pybind11;
using namespace pybind11::literals;

void init_submodule_yieldcurve(pybind11::module& m) {

  auto sub = m.def_submodule("_yieldcurve");
  sub.doc() = "yieldcurve submodule";

  py::enum_<ql::Bond::Price::Type>(sub, "bondpricetype")
    .value("clean", ql::Bond::Price::Type::Clean)
    .value("dirty", ql::Bond::Price::Type::Dirty)
    .export_values()
    ;

  py::class_<
      ql::FittedBondDiscountCurve::FittingMethod
  >(sub, "fittingmethod")
    ;

/*   py::class_<
      ql::CubicBSplinesFitting
    ,  
  >(sub, "cubicbsplinefitting")
    ; */

  py::class_<
      ql::YieldTermStructure
    , std::shared_ptr<ql::YieldTermStructure>
  >(sub, "yieldtermstructure")
    .def("zerorate", [](
          std::shared_ptr<ql::YieldTermStructure> ts
        , const ql::Date& dt
        , const ql::DayCounter& dc
        , ql::Compounding c
        , ql::Frequency freq
        , bool extrapolate) -> double {
        return ts->zeroRate(dt, dc, c, freq, extrapolate).rate();
      }
      , py::arg("date")
      , py::arg("daycounter")
      , py::arg("compounding")
      , py::arg("frequency")
      , py::arg("extrapolate") = false
    )
    ;

  py::class_<
      ql::FlatForward
    , std::shared_ptr<ql::FlatForward>
    , ql::YieldTermStructure
  >(sub, "flatforward")
    .def(py::init<const ql::Date&, ql::Rate, const ql::DayCounter&, ql::Compounding>())
    ;

  py::class_<
      ql::BondHelper
    , std::shared_ptr<ql::BondHelper>
  >(sub, "bondhelper")
  .def(py::init<
          ql::Handle<ql::Quote>
        , std::shared_ptr<ql::Bond>&
        , ql::Bond::Price::Type>()
        , py::arg("quote")
        , py::arg("bond")
        , py::arg("pricetype") = ql::Bond::Price::Type::Clean)
  ;

  py::class_<
      ql::FittedBondDiscountCurve
  >(sub, "fittedbonddiscountcurve")
  .def(py::init<>([](
          ql::Natural settlementDays
        , const ql::Calendar& calendar
        , std::vector<std::shared_ptr<ql::BondHelper>> bonds
        , const ql::DayCounter& dc
        , const ql::FittedBondDiscountCurve::FittingMethod& method
        , ql::Real accuracy
        , ql::Size maxit
        , Eigen::Ref<const Eigen::VectorXd> guess
        , ql::Real lambda
        , ql::Size maxstat) {

          // TODO: convert eigen array to ql
          auto arr = ql::Array();

          return ql::FittedBondDiscountCurve(settlementDays
            , calendar, bonds, dc, method
            , accuracy, maxit, arr
            , lambda, maxstat);
        })
      , py::arg("settlementdays")
      , py::arg("calendar")
      , py::arg("bonds")
      , py::arg("daycounter")
      , py::arg("method")
      , py::arg("accuracy") = 1.0e-10
      , py::arg("maxit") = 10000
      , py::arg("guess") = Eigen::VectorXd()
      , py::arg("lambda") = 1.0
      , py::arg("maxstationaryiteration") = 100
    )
  ;
}