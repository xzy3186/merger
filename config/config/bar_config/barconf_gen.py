# coding: UTF-8
import json

f = open('VANDLEBars.txt')
lines = f.readlines()
f.close()
bars = []
for line in lines:
    data = line.split()
    bars.append({'BarNum':data[0], 'LROffset':data[1], 'ZOffset':data[2], 'Angle':data[3]})

of = open('VandleBars.json','w')
json.dump(bars,of)
