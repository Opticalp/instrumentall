# -*- coding: utf-8 -*-

## @file   testsuite/python/parameterGetterTest.py
## @date   Jul 2016
## @author PhRG - opticalp.fr
##
## Test the features of the parameter getters 

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

def myMain():
    """Main function. Run the tests. """
    
    print "Test the basic features of the parameter getters. "
    
    fac = Factory("DemoRootFactory")
    print "Retrieved factory: " + fac.name
    
    print "Create module from leafParam factory"
    modParam = fac.select("branch").select("leafParam").create("modParam")
    print "module " + modParam.name + " created. "
    
    print "Retrieve the module parameters: "
    params = modParam.getParameterSet()

    for param in params:
        value = modParam.getParameterValue(param["name"])
        if not isinstance(value, basestring):
            value = str(value)
        print ( " - " + param["name"] + ": " + param["descr"] +
                " ; value = " + value )
        
    print "Trying to set some parameter values: "
    print " - set intParam to 666"
    modParam.setParameterValue("intParam", 666)
    print " - set floatParam to 0"
    modParam.setParameterValue("floatParam", 0)
    print " - set strParam to mojo"
    modParam.setParameterValue("strParam", "mojo")

    print "Build param getters using those parameters..."
    getInt = modParam.buildParameterGetter("intParam")
    getFloat = modParam.buildParameterGetter("floatParam")
    getStr = modParam.buildParameterGetter("strParam")

    print "Parameter names: "
    print " - " + getInt.parameterName()
    print " - " + getFloat.parameterName()
    print " - " + getStr.parameterName()

    print "Create a module to trig the parameter getters"
    trigger = Factory("DataGenFactory").select("float").create("trigger")
    trigPort = trigger.outPorts()[0]

    print "Test DataTarget cast (ParameterGetter)"
    targetInt = DataTarget(getInt)
    print "target: " + targetInt.name + " (" + targetInt.description + ")"

    print "Bind the trigger to the param getters"
    bind(trigPort, DataTarget(getInt))
    bind(trigPort, DataTarget(getFloat))
    bind(trigPort, DataTarget(getStr))

    print "Test param int getter connexion"
    print "Source is: " + targetInt.getDataSource().name

    print "Create Data loggers to print the output of the param getters"
    loggerInt = DataLogger("DataPocoLogger")
    loggerFloat = DataLogger("DataPocoLogger")
    loggerStr = DataLogger("DataPocoLogger")

    print "Test DataTarget cast (DataLogger)"
    targetLogger = DataTarget(loggerInt)
    print "target: " + targetLogger.name + " (" + targetLogger.description + ")"

    print "Test DataSource cast (ParameterGetter)"
    sourceInt = DataSource(getInt)
    print "source: " + sourceInt.name + " (" + sourceInt.description + ")"

    print "Bind the loggers to the param getters"
    bind(DataSource(getInt), DataTarget(loggerInt))
    bind(DataSource(getFloat), DataTarget(loggerFloat))
    bind(DataSource(getStr), DataTarget(loggerStr))

    print "Trig the getters"
    runModule(trigger)
    waitAll()

    if (DataSource(getInt).getDataValue() != 666):
        raise RuntimeError("wrong param getter int forwarded value")

    if (abs(DataSource(getFloat).getDataValue()) > 0.1):
        raise RuntimeError("wrong param getter float forwarded value")
    
    if (DataSource(getStr).getDataValue() != "mojo"):
        raise RuntimeError("wrong param getter str forwarded value")

    print "Returned values OK"

    print "The data attribute forwarding is not tested here..."

    print "End of script parameterGetterTest.py"
    
# main body    
import sys
import os
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print "current script: ",os.path.realpath(__file__)
        
        from instru import *

        myMain()
        exit(0)

print "Presumably not called from InstrumentAll >> Exiting..."

exit("This script has to be launched from inside InstrumentAll")
