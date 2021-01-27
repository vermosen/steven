#!/usr/bin/env python

import os, sys
import unittest

class test_settings(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_settings, self).__init__(*args, **kwargs)
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

if __name__ == '__main__':
  unittest.main()