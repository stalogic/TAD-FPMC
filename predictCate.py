#!/usr/bin/env python
# coding=utf-8
import math
import scipy as sp

#read test datasets
fc = open("test.txt")
ftest=open("test.txt")
fr = open("result.txt","w")

#new test method , use orgin test file
u_l=sp.genfromtxt("u_l.txt")
t_l=sp.genfromtxt("t_l.txt")
p_l=sp.genfromtxt("p_l.txt")
l_u=sp.genfromtxt("l_u.txt")
l_t=sp.genfromtxt("l_t.txt")
l_p=sp.genfromtxt("l_p.txt")

#get all location categories for predict
loca={}
line=fc.readline().strip()
while 1:
    if not line:
        break
    llist = line.split("||")
    lllist = llist[4].split("\t")
    for i in range(0,len(lllist)):
        loca[lllist[i]] = 0.0
    line = fc.readline().strip()
fc.close()
'''
res=0.0
for f in range(0,64):
    res = res +u_l[1,f]*l_u[104,f]+t_l[3,f]*l_t[104,f]+p_l[57,f]*l_p[104,f]
print res
input(1)
'''
#predict top categories
tline = ftest.readline().strip()
i=0
while 1:
    if not tline:
        break
    # print tline
    tlist = tline.split("||")
    user = int(tlist[0])
    time = int(tlist[1])
    deltaT = float(tlist[3])
    prelist = tlist[2].split("\t")
    udotl=0.0
    tdotl=0.0
    pdotl=0.0
    result = 0.0

    for  locacate in loca:
        l = int(locacate)

        result = 0.0
        udotl=0.0
        tdotl=0.0
        pdotl=0.0

        for f in range(0,64):
            udotl = udotl + u_l[user,f]*l_u[l,f]
            tdotl = tdotl + t_l[time,f]*l_t[l,f]
            temppdotl=0.0
            for precate in range(0,len(prelist)):
                temppdotl = temppdotl + p_l[int(prelist[precate]),f]*l_p[l,f]
            pdotl = math.exp(-0.04*deltaT)*temppdotl/len(prelist)
        result = udotl+tdotl+pdotl
        #print user,time,l,result
        loca[locacate] = result

    res=""
    for key in sorted(loca.items(),key = lambda d: d[1],reverse = True):
        res = res + str(key[0])+"+"+str(key[1])+"\t"
    resline = tline+"::"+res+"\n"
    fr.write(resline)
    tline = ftest.readline().strip()
    if i % 1000==0:
    	print i
    i+=1
print "finished"
