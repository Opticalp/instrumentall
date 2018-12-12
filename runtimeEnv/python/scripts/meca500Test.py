# -*- coding: utf-8 -*-

## @file   python/scripts/meda500Test.py
## @date   Oct. 2018
## @author PhRG - opticalp.fr
##
## Test the Mecademic Meca500

#
# Copyright (c) 2017 Ph. Renaud-Goud / Opticalp
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

    print("Test the Meca500. ")

    from instru import * 
    
    fac = Factory("DeviceFactory").select("motion")
    NIClist = fac.selectValueList()

    for nic in NIClist:
        if nic.find("192.168.0.") is 0:
            fac = fac.select(nic)
            break
        
    print("Retrieve robot factory at the default robot IP address... ")
    fac = fac.select("192.168.0.100")
    
    print("Create robot from its factory")
    robo = fac.create("robo")
    print("Robot module: " + robo.name + " was created. (" + robo.internalName + ") ")

    print("Assuming that the TRF and WRF were nicely set. ")

    print("Create coord sources")
    seqGen = Factory("DataGenFactory").select("seq").create("seqGen")
    dataGen = Factory("DataGenFactory").select("dblFloat").create("dataGen")
    zeroGen = Factory("DataGenFactory").select("dblFloat").create("zeroGen")
    zeroGen.setParameterValue("value",0)
    dataGenArray = Factory("DataGenFactory").select("dblFloat").create("dataGenArray")

    print("Prepare dataFlow")
    bind(seqGen.outPort("data"),dataGen.inPort("trig"))
    bind(seqGen.outPort("data"),zeroGen.inPort("trig"))
    bind(seqGen.outPort("data"),dataGenArray.inPort("trig"))
    bind(zeroGen.outPort("data"), robo.inPort("xAxis"))
    bind(zeroGen.outPort("data"), robo.inPort("yAxis"))
    bind(zeroGen.outPort("data"), robo.inPort("zAxis"))
    bind(zeroGen.outPort("data"), robo.inPort("aAxis"))
    bind(dataGen.outPort("data"), robo.inPort("bAxis"))
    bind(dataGenArray.outPort("data"), robo.inPort("cAxis"))


    print("Prepare movement")
    seqSize = 5
    rows = 5
    interlacing = 1

    print("go row after row")
    for row in range(-(rows-1)/2, (rows+1)/2):
        seqGen.setParameterValue("seqSize",seqSize)
        for x in range(-(seqSize-1)/2, (seqSize+1)/2):
            dataGenArray.setParameterValue("value",interlacing*2*float(x))
        dataGen.setParameterValue("value",2*float(row))

        print("Run row (" + str(row) + ")")
        runModule(seqGen)
        waitAll()
        
        interlacing = interlacing * -1
    
    robo.setParameterValue("query","movePose(0,0,0,0,0,0)")
    robo.getParameterValue("query")
    
    print("End of script meca500Test.py")
    
# main body    
import sys
import os
from os.path import dirname
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print("current script: ",os.path.realpath(__file__))
        
        baseDir = dirname(dirname(__file__))
        
        myMain(baseDir)
        exit(0)

print("Presumably not called from InstrumentAll >> Exiting...")

exit("This script has to be launched from inside InstrumentAll")
