#!/usr/bin/env python

import os, sys
import unittest

class test_termstructure(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_termstructure, self).__init__(*args, **kwargs)
    pass

  def test_create_yieldtermstructure(self):
    
    from steven import handles
    from steven import date, timeunit, compounding
    from steven.calendars import unitedstates
    from steven.daycounters import actual365
    from steven.yieldcurve import flatforward, yieldtermstructure

    dt = date(2020, 1, 4)
    cv = flatforward(dt, 0.001, actual365(), compounding.simple)
    hdl = handles.yieldtermstructure(cv)
    self.assertIsInstance(hdl.link(), yieldtermstructure)

  def test_create_schedule(self):

    from steven import date, timeunit
    from steven.calendars import unitedstates
    from steven import businessdayconvention as bdc
    from steven import period, schedule, dategenerationrule as drule

    start       = date(2021, 1, 8)
    first       = date(2021, 7, 8)
    end         = date(2031, 1, 8)
    penultimate = date(2030, 7, 8)

    tenor = period(6, timeunit.months)
    cdr = unitedstates()

    sdl = schedule(
      start, end, 
      tenor, cdr, 
      bdc.following, 
      bdc.following, 
      drule.backward,
      True,
      first, penultimate
    )

    self.assertIsInstance(sdl, schedule)

  def test_create_blackconstantvoltermstruture(self):

    from steven import handles
    from steven.calendars import unitedstates
    from steven.daycounters import actual365
    from steven.quotes import simplequote
    from steven import date, timeunit, compounding 
    from steven import blackconstantvol, blackvoltermstructure

    dt = date(2020, 1, 4)
    cdr = unitedstates()
    
    h_q = handles.quote(simplequote(1.23 * 1e-5))
    vol = blackconstantvol(dt, cdr, h_q, actual365())
    hdl = handles.blackvoltermstructure(vol)
    self.assertIsInstance(hdl.link(), blackvoltermstructure)

if __name__ == '__main__':
  unittest.main()