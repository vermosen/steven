
#include <ctime>
#include <iomanip>

#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <pybind11/stl.h>

#include <ql/settings.hpp>
#include <ql/compounding.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>

namespace ql = QuantLib;

namespace py = pybind11;
using namespace pybind11::literals;
using clock_type = std::chrono::system_clock;

PYBIND11_MODULE(_steven, m) {

  m.doc() = "heston model bindings";

  py::enum_<ql::TimeUnit>(m, "timeunit")
    .value("days"         , ql::TimeUnit::Days      )
    .value("weeks"        , ql::TimeUnit::Weeks     )
    .value("months"       , ql::TimeUnit::Months    )
    .value("years"        , ql::TimeUnit::Years     )
    .export_values()
    ;

  py::enum_<ql::BusinessDayConvention>(m, "businessdayconvention")
    // ISDA only
    .value("following"        , ql::BusinessDayConvention::Following        )
    .value("modifiedfollowing", ql::BusinessDayConvention::ModifiedFollowing)
    .value("preceding"        , ql::BusinessDayConvention::Preceding        )
    .value("unadjusted"       , ql::BusinessDayConvention::Unadjusted       )
    .export_values()
    ;

  py::enum_<ql::Compounding>(m, "compounding")
    // ISDA only
    .value("simple"              , ql::Compounding::Simple              )
    .value("compounded"          , ql::Compounding::Compounded          )
    .value("continuous"          , ql::Compounding::Continuous          )
    .value("simplethencompounded", ql::Compounding::SimpleThenCompounded)
    .value("compoundedthensimple", ql::Compounding::CompoundedThenSimple)
    .export_values()
    ;

  py::class_<ql::Date>(m, "date")
    .def(py::init<>())
    .def(py::init<>([](int year, int month, int day) {
        return ql::Date(day, static_cast<ql::Month>(month), year);
      }
    ))
    .def_property_readonly("year" , &ql::Date::year)
    .def_property_readonly("day"  , &ql::Date::dayOfMonth)
    .def_property_readonly("month", [](const ql::Date& dt) {
        return static_cast<int>(dt.month());
      }
    )
    .def("__eq__", [](const ql::Date& d1, const ql::Date& d2) { 
        return d1 == d2; 
      }
    )
    .def("__str__", [](const ql::Date& dt) {
      std::stringstream ss; ss
        << std::setfill('0') 
        << std::setw(4) << dt.year()  << "-"  
        << std::setw(2) << static_cast<int>(dt.month()) << "-" 
        << std::setw(2) << dt.dayOfMonth()
        ;

        return ss.str();
    })
    ;

  py::class_<ql::DayCounter>(m, "daycounter")
    .def("yearfraction", &ql::DayCounter::yearFraction
      , py::arg("start")
      , py::arg("end")
      , py::arg("refstart") = ql::Date()
      , py::arg("refend") = ql::Date()
    )
    ;

  py::class_<ql::Actual360, ql::DayCounter>(m, "actual360")
    .def(py::init<>())
    ;

  py::class_<ql::Actual365Fixed, ql::DayCounter>(m, "actual365")
    .def(py::init<>())
    ;

  py::class_<ql::Calendar>(m, "calendar")
    .def("advance", [](
          const ql::Calendar& cal
        , const ql::Date& dt
        , int n
        , ql::TimeUnit unit
        , ql::BusinessDayConvention bdc
        , bool eom) {

        return cal.advance(dt, n, unit, bdc, eom);
      }
      , py::arg("date")
      , py::arg("amount")
      , py::arg("unit")
      , py::arg("bdc")
      , py::arg("eom") = false
    )
    ;

  py::class_<
      ql::UnitedStates
    , ql::Calendar
  >(m, "unitedstates")
    .def(py::init<>())
    ;

  // access settings
  m.def("set_evaluation_date", [](const ql::Date& dt) {
      ql::Settings::instance().evaluationDate() = dt;
    }
  )
  ;

  m.def("get_evaluation_date", []() {
      ql::Date dt = ql::Settings::instance().evaluationDate();
      return dt;
    }
  )
  ;
}