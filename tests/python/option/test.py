#!/usr/bin/env python

import os, sys
import unittest

class test_option(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    super(test_option, self).__init__(*args, **kwargs)
    pass

  def test_create_american_option(self):
    
    from steven import date, vanillaoption, option_type
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff

    today = date(2020, 1, 4)
    expiry = date(2021, 1, 4)
    ex = americanexercise(today, expiry)
    pay = plainvanillapayoff(type=option_type.call, strike=50)
    opt = vanillaoption(pay, ex)
    self.assertIsInstance(opt, vanillaoption)

  def set_binomial_pricing_engine(self):

    from steven import date, time_unit, compounding
    from steven import vanillaoption, option_type
    from steven import blackconstantvol, flatforward
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff
    from steven.calendar import unitedstates
    from steven.daycounters import actual360
    from steven import handles
    
    today = date(2020, 1, 4)
    cdr = unitedstates()

    # option setup
    expiry = date(2021, 1, 4)
    ex = americanexercise(today, expiry)
    pay = plainvanillapayoff(type=option_type.call, strike=50)
    opt = vanillaoption(pay, ex)

    # rate ts setup
    settle = cdr.advance(today, 2, time_unit.days)
    rt     = flatforward(settle, 0.0123, actual360(), compounding.simple)
    h_rt   = handles.yieldtermstructure(rt)

    # div ts setup
    div = flatforward(today, 0.0, actual360(), compounding.simple)
    h_div = handles.yieldtermstructure(div)

    # vol ts setup
    h_v = handles.quote(simplequote(1.23 * 1e-5))
    vol = blackconstantvol(dt, cdr, h_v, actual365())
    h_vol = handles.blackvoltermstructure(vol)

if __name__ == '__main__':
  unittest.main()