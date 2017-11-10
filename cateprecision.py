#!/usr/bin/env python
#coding=utf-8
#种类预测
fw = open("at100.txt","w")
catepre = open("catepre.txt","w")

for atN in [1,5,10,20,30,40,50,60,70,80,90,100]:
    f = open('result.txt')
    count  = 0.0
    all = 0.0
    for line in f:
        s = ""
        aline = line.split('::')
        right = aline[0].split("||")[-3].split("\t")
        s = s + aline[0]+"::"
        testlist = aline[1].split('\t')
        all += 1
        for i in range(0,atN):
            temp = testlist[i].split('+')
            id = temp[0]

            if id in right:
                count += 1
                for j in range(0,atN):
                    s = s+testlist[j]+"\t"
                s = s + "\n"
                if atN == 100:
                    fw.write(s)
                break
    print str(count)+"@"+str(all)+"\n"
    print count/all
    catepre.write(str(count/all)+"\t")

