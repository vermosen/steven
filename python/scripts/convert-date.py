#!/usr/bin/env python
 
from steven import date, get_evaluation_date, set_evaluation_date

def main():

  dt = date(2021, 1, 10)
  set_evaluation_date(dt)
  print(get_evaluation_date())

if __name__ == '__main__':
  main()