#!/usr/bin/env python

import os, sys
import unittest

class test_time(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_time, self).__init__(*args, **kwargs)
    pass

  def test_set_date(self):

    from steven import date, set_evaluation_date
    
    try:
      d = date(2021, 1, 20)
      set_evaluation_date(d)
      throw = False
    except:
      throw = True

    self.assertFalse(throw)

  def test_get_date(self):

    from steven import date, set_evaluation_date, get_evaluation_date
    
    result = None
    try:
      d = date(2021, 1, 20)
      set_evaluation_date(d)
      result = get_evaluation_date()
      throw = False
    except:
      throw = True

    self.assertFalse(throw)
    self.assertIsInstance(result, date)
    self.assertEqual(result.year, d.year)
    self.assertEqual(result.month, d.month)
    self.assertEqual(result.day, d.day)

  def test_print_date(self):

    from steven import date
    
    d = date(2021, 1, 1)
    self.assertEqual(d.__str__(), '2021-01-01')

  def test_create_us_calendar(self):

    from steven.calendars import unitedstates
    
    cal = unitedstates()
    self.assertIsInstance(cal, unitedstates)

  def test_calendar_advance(self):

    from steven import date, businessdayconvention, timeunit
    from steven.calendars import unitedstates

    cal = unitedstates()
    res = cal.advance(date(2021, 1, 1), 3, timeunit.weeks, businessdayconvention.following)
    self.assertEqual(res.day, 22)
    self.assertEqual(res.year, 2021)
    self.assertEqual(res.month, 1)

  def test_act360_yearfraction(self):

    from steven import date, businessdayconvention, timeunit
    from steven.daycounters import actual360

    dc = actual360()
    start = date(2020, 1, 1)
    end = date(2021, 1, 1)
    res = dc.yearfraction(start=start, end=end)
    self.assertAlmostEqual(res, 1.016666666666)

if __name__ == '__main__':
  unittest.main()