# -*- coding: utf-8 -*-

## @file   testsuite/python/dataProxyParamTest.py
## @date   Apr. 2018
## @author PhRG - opticalp.fr
##
## Test the features of the DataProxy

#
# Copyright (c) 2018 Ph. Renaud-Goud / Opticalp
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
    
    print("Test the DataProxy class with parameters. ")

    from instru import *
    import time

    fac = Factory("DataGenFactory")
    print("Retrieved factory: " + fac.name)
    
    print("Create module from intDataGen factory")
    mod1 = fac.select("int32").create("intGenerator")
    modTrigged = fac.select("int32").create("trigged")
    print("module " + mod1.name + " created (" + mod1.internalName + ") ")
    
    print("Set output value to 314")
    mod1.setParameterValue("value", 314)
    
    print("Run module")
    task = runModule(mod1)
    task.wait()
    
    print("Return value is: " + str(mod1.outPort("data").getDataValue()))
    if mod1.outPort("data").getDataValue() != 314 :
        raise RuntimeError("Wrong return value: 314 expected. ")

    fac = Factory("DemoRootFactory")
    print("Retrieved factory: " + fac.name)

    print("Create module from leafForwarder factory")
    mod2 = fac.select("branch").select("leafForwarder").create("mod2")
    print("module " + mod2.name + " created. ")

    # query the possible DataProxy class names for DataProxy creation
    proxyClasses = dataProxyClasses() # DataManager::dataProxyClasses()
    print("Available data proxy classes: ")
    for proxyClass in proxyClasses:
        print(" - " + proxyClass + ": " + proxyClasses[proxyClass])
    
    print('Proxy creation using the constructor: DataProxy("Delayer")')
    proxy = DataProxy("Delayer") 
    print(" - Name: " + proxy.name)
    print(" - Description: " + proxy.description)

    print("Bind the output of mod1 (data gen) to the forwarder via the proxy")
    bind(mod1.outPorts()[0], mod2.inPorts()[0], proxy)

    print("Run module mod1")
    t0 = time.time()
    runModule(mod1)
    waitAll()
    
    elapsed = time.time()-t0
    print("elapsed time (ms): " + str(elapsed*1000))
    if elapsed < 1:
        raise RuntimeError("the delay did not apply?")
    
    proxy.setParameterValue("duration",4000)
    
    print("Run module again, with delay duration set to 4s")
    t0 = time.time()
    runModule(mod1)
    waitAll()

    elapsed = time.time()-t0
    print("elapsed time (ms): " + str(elapsed*1000))
    if elapsed < 4:
        raise RuntimeError("the delay duration modification did not apply?")
    
    from os.path import join
    
    cfgFile = join(join(baseDir,"resources"),"modParamTest.properties")
    print("Load test config file: modParamTest.properties from " + cfgFile)
    loadConfiguration(cfgFile)

    print("change data proxy name")
    proxy.setName("testIt")
    
    print("get the new delay duration: " + str(proxy.getParameterValue("duration")))
    if proxy.getParameterValue("duration") != 3000:
        raise RuntimeError("The duration should be now 3000ms, as defined in hte property file")

    print("Reseting workflow")
    resetWorkflow()
    
    print("Test the delayer as bound to a trig port")
    print("Create a new delayer")
    delay2 = DataProxy("Delayer")
    delay2.setName("delay2")
    delay2.setParameterValue("duration",1000)

    print("Bind using bind directive with proxy")
    bind(mod1.outPort("data"), modTrigged.inPort("trig"), delay2)

    print("Run")
    runModule(mod1)
    waitAll()

    print("Reset worklfow once again")

    delay3 = DataProxy("Delayer")
    delay3.setName("delay3")
    delay3.setParameterValue("duration",1000)

    print("Bind delayer to trig using atomic bind directives")
    bind(mod1.outPort("data"),DataTarget(delay3))
    bind(DataSource(delay3), modTrigged.inPort("trig"))
    
    print("Run")
    runModule(mod1)
    waitAll()
    
    print("End of script dataProxyParamTest.py")
    
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
