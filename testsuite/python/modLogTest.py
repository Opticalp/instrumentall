# -*- coding: utf-8 -*-

## @file   testsuite/python/modLogTest.py
## @date   May 2016
## @author PhRG - opticalp.fr
##
## Test the features of the module and module factory loggers

#
# Copyright (c) 2016 Ph. Renaud-Goud / Opticalp
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


def myMain(baseDir):
    """Main function. Run the tests. """
    
    print "Test the basic features of the module factories. "

    from instru import *
    
    facList = getRootFactories()
    
    print "Available factories: "
    for fac in facList:
        print " - " + fac.name + ": " + fac.description
    
    fac = Factory("DemoRootFactory")
    print "Using DemoRootFactory. "
    
    ## select
    print fac.selectDescription()
    print "Possible selectors: "
    for val in fac.selectValueList():
        print " - " + val + " >> " + fac.select(val).name
        print "   " + fac.select(val).description

    fac = fac.select("branch").select("leaf")
    
    print "Create module from leaf factory"
    demoMod = fac.create()

    print fac.name + " verbosity is: " + str(fac.getVerbosity())
    print demoMod.name + " verbosity is: " + str(demoMod.getVerbosity())

    print "Setting " + fac.name + " verbosity to 3"
    fac.setVerbosity(3)
    fac.create()
        
    print "Setting " + fac.name + " verbosity to 8"
    fac.setVerbosity(8)
    fac.create()

    print "End of script modLogTest.py"
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print "current script: ",os.path.realpath(__file__)
        
        baseDir = dirname(dirname(__file__))
        
        myMain(baseDir)
        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")
