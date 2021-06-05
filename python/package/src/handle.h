#pragma once
#ifndef STEVEN_PYTHON_HANDLE_H
#define STEVEN_PYTHON_HANDLE_H

#include <pybind11/pybind11.h>

void init_submodule_handle(pybind11::module&);

#endif // STEVEN_PYTHON_HANDLE_H