
from steven import date
from steven.calendars import unitedstates
from steven import zerocouponbond
from steven.yieldcurve import bondhelper 
from steven.quotes import simplequote
from steven.handles import quote as hquote

cdr   = unitedstates()

bstarts = [date(2021, 1, 8)]
bends   = [date(2031, 1, 8)]

tstarts = [date(2021, 1, 8)]
ttenors = ['2Y', '3Y', '4Y', '5Y', '7Y', '10Y', '20Y', '30Y']
tcpns   = [1.1, 1.4, 1.7, 1.9, 2.6]
tylds   = [1.1, 1.4, 1.7, 1.9, 2.6]


bonds = [zerocouponbond(settlementdays=1, calendar=cdr, faceamount=100.0, maturity=e, issuedate=start) for e in ends]

q = simplequote(99.987)
h = hquote(q)

hlp = bondhelper(h, b)