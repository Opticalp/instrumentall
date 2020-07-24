# -*- coding: utf-8 -*-

## @file   testsuite/python/bufferToTrig.py
## @date   Mar 2019
## @author PhRG
##
## test script to debug buffer issue outputing to trig port

#
# Copyright (c) 2019 Ph. Renaud-Goud
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


def main(baseDir):
    from instru import *
    from os.path import join

    print("Add a python module to collect the results")
    pyMod = Factory("ExternFactory").select("python").select("data").create("pyMod")

    scriptFile = join(join(baseDir,"resources"),"pyModBuffTest.py")
    print("Load script file: " + scriptFile)
    pyMod.setParameterValue("scriptFilePath",scriptFile)

    print("Build data generators for asynch scan")
    dataGenX = Factory("DataGenFactory").select("float").create("dataGenX")

    # using the buffering to free the output port to allow
    # the next value to be generated. 
    print("bind the positions to the pyMod, via buffers")
    buffX = DataProxy("DataBuffer")
    bind(dataGenX.outPort("data"), DataTarget(buffX))
    bind(DataSource(buffX), pyMod.inPort("data"))

    print("################### RUN #############################")
    runModule(dataGenX)
    waitAll()
    print("################### RUN DONE. #######################")

    print("Test done. ") 
   
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

        main(baseDir)

        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")
