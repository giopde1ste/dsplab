#!/usr/bin/env python
#/************************************************************************
# Python script om cppcheck uit te voeren
# Copyright 2014 Hogeschool van Arnhem en Nijmegen
# Opleiding Embedded Systems Engineering
# $Id: grafiekVenster.cpp 1263 2013-11-25 10:43:05Z ewout $
#************************************************************************/

import os,subprocess,sys,getopt;

def main(argv):
       inputfile = ''
       sourcedir = ''
       outputdir = ''

       if platform.system()=="FreeBSD" :
              exedir = "/usr/local/bin";
       elif platform.system()=="Darwin" :
              exedir = "/opt/local/bin";
       else :
              print "Onbekend platform gebruikt, geen cppcheck mogelijk";
              sys.exit(2);
              
       try:
           opts, args = getopt.getopt(argv,"hi:s:o:",["ifile=","sdir=","odir="])
       except getopt.GetoptError:
           print 'cppcheck.py -i <inputfile> -s <sourcedir> -o<destdir>'
           sys.exit(2)
       for opt, arg in opts:
              if opt == '-h':
                     print 'cppcheck.py -i <inputfile> -s <sourcedir> -o<destdir>'
                     sys.exit()
              elif opt in ("-i", "--ifile"):
                     inputfile = arg
              elif opt in ("-s", "--sdir"):
                     sourcedir = arg
              elif opt in ("-o", "--odir"):
                     outputdir = arg

              call(
if __name__ == "__main__":
    main(sys.argv[1:])
                                                                                                  
