#!/usr/bin/env python

import os, sys
import unittest

class test_termstructure(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_termstructure, self).__init__(*args, **kwargs)
    pass

  def test_create_yieldtermstructure(self):
    
    from steven import handles
    from steven.calendars import unitedstates
    from steven.daycounters import actual365
    from steven import date, timeunit, compounding, flatforward

    dt = date(2020, 1, 4)
    cv = flatforward(dt, 0.001, actual365(), compounding.simple)
    hdl = handles.yieldtermstructure(cv)
    self.assertIsInstance(hdl, handles.yieldtermstructure)

if __name__ == '__main__':
  unittest.main()