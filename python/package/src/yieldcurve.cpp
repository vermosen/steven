#include "yieldcurve.h"

#include <pybind11/eigen.h>
#include <ql/instruments/bond.hpp>
#include <ql/termstructures/yield/bondhelpers.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/fittedbonddiscountcurve.hpp>
#include <ql/termstructures/yield/nonlinearfittingmethods.hpp>

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

  py::class_<
      ql::CubicBSplinesFitting
    , ql::FittedBondDiscountCurve::FittingMethod
  >(sub, "cubicbsplinefitting")
  .def(py::init<>([](
        const std::vector<ql::Time>& knots
      , bool constrainAtZero
      , Eigen::Ref<const Eigen::VectorXd> weights
      , const std::shared_ptr<ql::OptimizationMethod>& optimizer
      , Eigen::Ref<const Eigen::VectorXd> l2
      , ql::Real minCutoffTime
      , ql::Real maxCutoffTime
    ) {
      
      auto _weigths = ql::Array();
      auto _l2      = ql::Array();

      return ql::CubicBSplinesFitting(
            knots, constrainAtZero, _weigths
          , optimizer, _l2
          , minCutoffTime
          , maxCutoffTime
        );
    })
    , py::arg("knots")
    , py::arg("contrainatzero") = true
    , py::arg("weights") = Eigen::VectorXd()
    , py::arg("optimizer") = std::shared_ptr<ql::OptimizationMethod>()
    , py::arg("l2") = Eigen::VectorXd()
    , py::arg("mincutofftime") = 0.0
    , py::arg("maxcutofftime") = QL_MAX_REAL
    )
    ;

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