#!/usr/bin/env python
 
from steven import date, settings

def main():

  dt = date(2021, 1, 10)
  settings().value_date = dt
  print(settings().value_date)

if __name__ == '__main__':
  main()