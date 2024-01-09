import csv
import os
import sys

if len(sys.argv) == 0:
  print("No directory passed")

numFiles = 0
RSSTotal = 0
CPUTimeTotal = 0.0

rowCPUTime = 0.0
rowRSS = 0

# Read all client csv files in the directory
for fileName in os.listdir(sys.argv[1]):
    if "server" in fileName:
        continue
    f = os.path.join(sys.argv[1], fileName)
    if os.path.isfile(f):
        with open(f, 'r') as csvfile:
            for row in reversed(list(csv.reader(csvfile))):
                try:
                    rowCPUTime = float(row[0])
                    rowRSS = int(row[1])
                except ValueError:
                    continue
                if rowCPUTime == 0 or rowRSS == 0:
                    continue

                CPUTimeTotal += rowCPUTime
                RSSTotal += rowRSS
                break
        numFiles += 1

RSSAverage = RSSTotal / numFiles
CPUTimeAverage = CPUTimeTotal / numFiles

print(f"{CPUTimeAverage},{RSSAverage}")
