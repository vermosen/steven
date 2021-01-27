
#include <ctime>
#include <iomanip>

#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <pybind11/stl.h>

#include <ql/settings.hpp>
#include <ql/userconfig.hpp>

namespace ql = QuantLib;

namespace py = pybind11;
using namespace pybind11::literals;
using clock_type = std::chrono::system_clock;

PYBIND11_MODULE(_steven, m) {

  m.doc() = "heston model bindings";

  py::class_<ql::Date>(m, "date")
    .def(py::init<>([](int year, int month, int day) {
        return ql::Date(day, static_cast<ql::Month>(month), year);
      }
    ))
    .def_property_readonly("year" , &ql::Date::year)
    .def_property_readonly("month", [](const ql::Date& dt) {
        return static_cast<int>(dt.month());
      }
    )
    .def_property_readonly("day"  , &ql::Date::dayOfMonth)
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