#!/usr/bin/env python
# encoding: utf-8
"""
perfgraphics.py

Created by Lucien Schreiber on 2011-04-28.
Copyright (c) 2011 CREALP. All rights reserved.
"""

"""
Simple code for plotting:
from pylab import *
title ("vroomGIS performance: Debug VS Release")
xlabel("Number of layers")
ylabel("Loading time [ms]")
...
plot instructions
...
legend(loc="best")
show()
"""


import sys
import os
try:
	from pylab import *
except:
	print ("Fatal error, unable to load matplotlib!")
	exit()


def splitList(listoflist):
	"""Spliting list of list in two lists"""
	listx = []
	listy = []
	for itemlist in listoflist:
		listx.append(itemlist[0])
		listy.append(itemlist[1])
	return (listx, listy)


print ("******************************************************")
print ("*** vroomGIS performance, graphical representation ***")
print ("***            (c) L. Schreiber, 2011              ***")
print ("******************************************************")

homedir = os.path.expanduser('~')
perfilepath = homedir + os.sep + "vroomgis-performance.txt"

if (raw_input("Use file: '{0}' (Y/N): ".format(perfilepath)).upper() != 'Y'):
	perfilepath = raw_input("Enter file path and name: ")
	
try:
	perfile = open(perfilepath, "r")
except:
	print("Error opening file: '{0}'".format(perfilepath))
	exit()
	
mylines = perfile.readlines()
perfile.close()


myfirstline = str(mylines[:1])
myfirstline = myfirstline[2:-5]
headernames = myfirstline.split(";")
for (counter, item) in enumerate(headernames):
	print ("{0}.-- {1}".format(counter, item))
xinput = input ("Select x Axis field number: ")
yinput = input ("Select y Axis field number: ")
filternum = raw_input ("Select filter field number: ")
filtertext = raw_input ("Keep only those values: ")
xname = headernames[xinput]
yname = headernames[yinput]

print ("file '{0}' contain: {1} lines".format(perfilepath,len(mylines)))

maxdico = {}
mindico = {}
histodico = {}
meancumulate = {}

myx = []
myy = []

for line in mylines[1:]:
	mylinelist = line.split(";")
	
	# filter
	if (len(filtertext) != 0):
		if (filtertext != mylinelist[int(filternum)]):
			continue
		
	tmpx = int(mylinelist[int(xinput)])
	tmpy = int(mylinelist[int(yinput)])
	
	myx.append(tmpx)
	myy.append(tmpy)
	if(maxdico.get(tmpx, 0) < tmpy):
		maxdico[tmpx] = tmpy
	if(mindico.get(tmpx, 10000000000000) > tmpy):
		mindico[tmpx] = tmpy
	histodico[tmpx] = histodico.get(tmpx, 0) + 1
	meancumulate[tmpx] = meancumulate.get(tmpx, 0) + tmpy

#print (maxdico)
#print (mindico)
#print (histodico)
#print (meancumulate)

maxlist = list(maxdico.items())
maxlist.sort()
maxlistx, maxlisty = splitList(maxlist)

minlist = list(mindico.items())
minlist.sort()
minlistx ,minlisty = splitList(minlist)

histolist = list(histodico.items())
histolist.sort()
histolistx, histolisty = splitList(histolist)

meancumulatelist = list(meancumulate.items())
meancumulatelist.sort()
meanx, meany = splitList(meancumulatelist)
for (counter, item) in enumerate(meany):
	meany[counter] = float(meany[counter] / histolisty[counter])


#plotting all Pts
print ("plotting {0} raw data".format(len(myx)))
figure1 = figure()
ax1 = figure1.add_subplot(111)
title("vroomGIS loading Time {0} samples".format(len(myx)))
ax1.plot(maxlistx, maxlisty, label="maximum")
ax1.plot(minlistx, minlisty, label="minimum")
ax1.plot(meanx, meany, label="mean")
ax1.set_xlabel(xname)
ax1.set_ylabel(yname)
ax1.plot(myx, myy, 'o', label="samples")
ax1.legend(loc="best")

myaxis = input("Change axis ? Type 0 to use actual axis: ({0})".format(ax1.axis()))
if myaxis:
	try:
		ax1.axis(myaxis)
	except:
		print ("error setting axis with following parameters ", myaxis)

figure2 = figure()
ax2 = figure2.add_subplot(111)
for (counter, item) in enumerate(histolisty):
	ax2.bar(histolistx[counter], item)
ax2.set_xlabel(xname)
ax2.set_ylabel("Samples")
show()

savemean = raw_input("Save mean to file ? (Y/N): ")
if(savemean.upper() == 'Y'):
	meanxtxt = [str(x) for x in meanx]
	meanytxt = [str(x) for x in meany]	
	savefile = open(homedir + os.sep + "saved-mean.txt", "a")
	savefile.write("\t".join(meanxtxt) + "\n")
	mylegendname = ""
	mylegendresult = "off"
	try:
		mylegendname = headernames[int(filternum)]
	except:
		mylegendname = "filter"
	if (filtertext != ""):
		mylegendresult = filtertext
	savefile.write("\t".join(meanytxt) + "\tplot({0},{1}, label=\"mean: {2}={3}\")\n".format(meanx, meany, mylegendname, mylegendresult))
	savefile.close()
	print ("mean writted to : '{0}'".format(homedir + os.sep + "saved-mean.txt"))