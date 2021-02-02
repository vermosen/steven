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

  def set_blackscholesmerton_process(self):

    from steven import businessdayconvention as bdc
    from steven import date, timeunit, compounding
    from steven import vanillaoption, option_type
    from steven import blackconstantvol, flatforward
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff
    from steven.calendars import unitedstates
    from steven.daycounters import actual360, actual365
    from steven.quotes import simplequote
    from steven.processes import blackscholesmerton
    from steven import handles
    
    today = date(2020, 1, 4)
    cdr = unitedstates()

    # underlying quote
    ud = handles.quote(simplequote(100.12))

    # rate ts setup
    settle = cdr.advance(today, 2, timeunit.days, bdc.following)
    rt     = flatforward(settle, 0.0123, actual360(), compounding.simple)
    h_rt   = handles.yieldtermstructure(rt)

    # div ts setup
    div = flatforward(today, 0.0, actual360(), compounding.simple)
    h_div = handles.yieldtermstructure(div)

    # vol ts setup
    h_v   = handles.quote(simplequote(1.23 * 1e-5))
    vol   = blackconstantvol(today, cdr, h_v, actual365())
    h_vol = handles.blackvoltermstructure(vol)

    # create the bsm process
    bsm = blackscholesmerton(ud, h_div, h_rt, h_vol)
    self.assertIsInstance(bsm, blackscholesmerton)
    
  def set_coxrossrubinstein_pricingengine(self):

    from steven import businessdayconvention as bdc
    from steven import date, timeunit, compounding, settings
    from steven import vanillaoption, option_type
    from steven import blackconstantvol, flatforward
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff
    from steven.calendars import unitedstates
    from steven.daycounters import actual360, actual365
    from steven.quotes import simplequote
    from steven.processes import blackscholesmerton
    from steven.engines import coxrossrubinstein
    from steven import handles
    
    today = date(2021, 1, 8)
    cdr = unitedstates()
    settings().value_date = today

    # underlying quote
    ud = handles.quote(simplequote(100.0))

    # option setup
    expiry = date(2021, 1, 29)
    ex = americanexercise(today, expiry)
    pay = plainvanillapayoff(type=option_type.call, strike=90)
    opt = vanillaoption(pay, ex)

    # rate ts setup
    settle = cdr.advance(today, 2, timeunit.days, bdc.following)
    rt     = flatforward(settle, 0.0015, actual360(), compounding.simple)
    h_rt   = handles.yieldtermstructure(rt)

    # div ts setup
    div = flatforward(today, 0.0, actual360(), compounding.simple)
    h_div = handles.yieldtermstructure(div)

    # vol ts setup
    h_v   = handles.quote(simplequote(5.00)) # 5 bps
    vol   = blackconstantvol(today, cdr, h_v, actual365())
    h_vol = handles.blackvoltermstructure(vol)

    # set the pricing engine
    bsm = blackscholesmerton(ud, h_div, h_rt, h_vol)
    eng = coxrossrubinstein(bsm, step=1000)
    opt.setpricingengine(eng)

    # compare delta to cached value 
    self.assertAlmostEqual(opt.delta, 0.7389951587780792)

  def find_implied_vol(self):

    from steven import businessdayconvention as bdc
    from steven import date, timeunit, compounding, settings
    from steven import vanillaoption, option_type
    from steven import blackconstantvol, flatforward
    from steven.exercises import americanexercise
    from steven.payoffs import plainvanillapayoff
    from steven.calendars import unitedstates
    from steven.daycounters import actual360, actual365
    from steven.quotes import simplequote
    from steven.processes import blackscholesmerton
    from steven.engines import coxrossrubinstein
    from steven import handles
    from steven.solvers import rootfinder
    
    today = date(2021, 1, 8)
    cdr = unitedstates()
    settings().value_date = today

    # underlying quote
    ud = handles.quote(simplequote(100.0))

    # option setup
    expiry = date(2021, 1, 29)
    ex = americanexercise(today, expiry)
    pay = plainvanillapayoff(type=option_type.call, strike=90)
    opt = vanillaoption(pay, ex)

    # rate ts setup
    settle = cdr.advance(today, 2, timeunit.days, bdc.following)
    rt     = flatforward(settle, 0.0015, actual360(), compounding.simple)
    h_rt   = handles.yieldtermstructure(rt)

    # div ts setup
    div = flatforward(today, 0.0, actual360(), compounding.simple)
    h_div = handles.yieldtermstructure(div)

    # vol ts setup
    q_v   = simplequote(5.00)
    h_v   = handles.quote(q_v) # 5 bps
    vol   = blackconstantvol(today, cdr, h_v, actual365())
    h_vol = handles.blackvoltermstructure(vol)

    # set the pricing engine
    bsm = blackscholesmerton(ud, h_div, h_rt, h_vol)
    eng = coxrossrubinstein(bsm, step=1000)
    opt.setpricingengine(eng)

    solv = rootfinder(instrument=opt, quote=q_v)
    res = solv.solve(guess = 5.0, npv = 33.525, accuracy=1e-12)

    # compare delta to cached value 
    self.assertAlmostEqual(res, 3.5159186041652473)

if __name__ == '__main__':
  unittest.main()