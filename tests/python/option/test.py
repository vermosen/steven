#!/usr/bin/env python

import os, sys
import unittest

class test_option(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_option, self).__init__(*args, **kwargs)
    pass

  def test_create_american_option(self):
    
    from steven import date, americanoption, option_type
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff

    today = date(2020, 1, 4)
    expiry = date(2021, 1, 4)
    ex = americanexercise(today, expiry)
    pay = plainvanillapayoff(type=option_type.call, strike=50)
    cv = flatforward(dt, 0.001, actual365(), compounding.simple)
    hdl = handles.yieldtermstructure(cv)
    self.assertIsInstance(hdl, handles.yieldtermstructure)

if __name__ == '__main__':
  unittest.main()