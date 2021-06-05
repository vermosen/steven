import pandas as pd

exp = '20210423'
close_dt = '2021-03-19'

file = '~/options_calls_IBM_%s.csv' % close_dt
df = pd.read_csv(file)
df.set_index('time', inplace=True)

eod = df.tail(1).T[1:]
eod.columns = ['close']
ud_price = df.tail(1)['underlying']

strike_map = eod.to_dict()['close']
strike_map = {float(k): v for k, v in strike_map.items()}

from steven import handles
from steven import date, settings, timeunit
from steven import option_type, vanillaoption, compounding
from steven import businessdayconvention as bdc
from steven.daycounters import actual360, actual365
from steven.quotes import simplequote
from steven.calendars import unitedstates
from steven.exercises import europeanexercise
from steven.payoffs import plainvanillapayoff
from steven import flatforward, blackconstantvol
from steven.processes import blackscholesmerton
from steven.engines import coxrossrubinstein

# calculation parameters
today = date(2021, 3, 18)
cdr = unitedstates()
settings().value_date = today

# underlying quote
ud = handles.quote(simplequote(ud_price))

# option setup
expiry = date(int(exp[0:4]), int(exp[5:6]), int(exp[7:8]))
ex     = europeanexercise(expiry)
pays   = [plainvanillapayoff(type=option_type.call, strike=k) for k, v in strike_map.items()]
opts   = [vanillaoption(p, ex) for p in pays]

# rate ts setup
settle = cdr.advance(today, 2, timeunit.days, bdc.following)
rt     = flatforward(today, 0.0015, actual360(), compounding.simple)
h_rt   = handles.yieldtermstructure(rt)

# dividend ts setup
settle = cdr.advance(today, 2, timeunit.days, bdc.following)
div    = flatforward(settle, 0.0, actual360(), compounding.simple)
h_div  = handles.yieldtermstructure(div)

# vol ts setup
settle = cdr.advance(today, 1, timeunit.days, bdc.following)
h_vs   = [handles.quote(simplequote(5.00)) for o in opts] # 5 bps
vols   = [blackconstantvol(settle, cdr, h, actual365()) for h in h_vs]
h_vols = [handles.blackvoltermstructure(v) for v in vols]

# pricing engines
bsms = [blackscholesmerton(ud, h_div, h_rt, h) for h in h_vols]
engs = [coxrossrubinstein(b, step=1000) for b in bsms]

for o, e in dict(zip(opts, engs)).items():
  o = o.setpricingengine(e)

from steven.math import rootfinder

data = list(zip(opts, h_vs, strike_map.values()))

vol_map = {}

for (o, h, p) in data[8:9]:
  try:
    h.link().value = 0.5
    print('option strike: %f' % o.strike)
    print('option market price: %f' % p)
    print('implied volatility guess: %f' % h.link().value)
    print('option npv: %f' % o.npv()) 
    rf = rootfinder(o, h.link())
    result = rf.solve(5.0, p, verbose=True, xmin=1e-2, xmax=10)
    vol_map[o.strike] = result
  except RuntimeError as rt:
    print('error: %s' % rt)
    vol_map[o.strike] = None
  
i = 0
vol_map