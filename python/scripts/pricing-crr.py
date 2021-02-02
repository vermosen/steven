from steven import handles
from steven import date, settings, timeunit
from steven import option_type, vanillaoption, compounding
from steven import businessdayconvention as bdc
from steven.daycounters import actual360, actual365
from steven.quotes import simplequote
from steven.calendars import unitedstates
from steven.exercises import americanexercise
from steven.payoffs import plainvanillapayoff
from steven import flatforward, blackconstantvol
from steven.processes import blackscholesmerton
from steven.engines import coxrossrubinstein

# calculation parameters
today = date(2021, 1, 8)
cdr = unitedstates()

# underlying quote
ud = handles.quote(simplequote(128.665))

# option setup
expiry  = date(2021, 1, 29)
ex      = americanexercise(today, expiry)
strikes = range(10, 150, 10)
pays    = [plainvanillapayoff(type=option_type.call, strike=k) for k in strikes]
opts    = [vanillaoption(p, ex) for p in pays]

# rate ts setup
settle = cdr.advance(today, 2, timeunit.days, bdc.following)
rt     = flatforward(today, 0.0015, actual360(), compounding.simple)
h_rt   = handles.yieldtermstructure(rt)

# dividend ts setup
settle = cdr.advance(today, 2, timeunit.days, bdc.following)
div    = flatforward(settle, 0.0, actual360(), compounding.simple)
h_div  = handles.yieldtermstructure(div)

# dividend ts setup
settle = cdr.advance(today, 1, timeunit.days, bdc.following)
h_v   = handles.quote(simplequote(5.00)) # 5 bps
vol   = blackconstantvol(settle, cdr, h_v, actual365())
h_vol = handles.blackvoltermstructure(vol)

# pricing engine
bsm = blackscholesmerton(ud, h_div, h_rt, h_vol)
eng = coxrossrubinstein(bsm, step=1000)
res = [o.setpricingengine(eng) for o in opts]

settings().value_date = today
res = settings().value_date

print(opts[8].delta)