import csv
import sys

if len(sys.argv) == 0:
  print("No .csv file passed")

# Read passed CSV file
with open(sys.argv[1], 'r') as csvfile:
  rows = csv.reader(csvfile, delimiter=',')
  next(rows, None)

  totalBandwidthUsed = 0
  for row in rows:
    totalBandwidthUsed += int(row[2])
  
  print(f"Total bandwidth used: {totalBandwidthUsed}B")
  print(f"Average network usage of: {totalBandwidthUsed/60} B/s")
