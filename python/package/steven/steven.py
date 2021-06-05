
import sys

if sys.version_info < (3, 9):
  from backports.zoneinfo import ZoneInfo
else:
  from zoneinfo import ZoneInfo

from ._steven import date, timeunit, businessdayconvention
from ._steven import option_type, compounding, frequency
from ._steven import set_evaluation_date, get_evaluation_date
from ._steven import blackvoltermstructure, blackconstantvol
from ._steven import vanillaoption
from ._steven import dategenerationrule, period, schedule
from ._steven import fixedratebond, zerocouponbond

class settings(object):

  @property
  def value_date(self):
    return get_evaluation_date()

  @value_date.setter
  def value_date(self, date):
    set_evaluation_date(date)


__all__ = [ 
    'date', 'timeunit', 'businessdayconvention'
  , 'compounding', 'frequency', 'option_type'
  , 'vanillaoption', 'blackvoltermstructure'
  , 'blackconstantvol', 'settings'
  , 'dategenerationrule', 'period'
  , 'schedule', 'fixedratebond', 'zerocouponbond'
]