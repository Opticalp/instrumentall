# -*- coding: utf-8 -*-

## @file   python/scripts/meda500Ctrl.py
## @date   Oct. 2018
## @author PhRG - opticalp.fr
##
## Move the Mecademic Meca500, set WRF

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

class App:
    def __init__(self, top):
        import Tkinter as tk
        from instru import Factory
        
        label = tk.Label(top, text="Set increment")
        label.pack()

        self.e = tk.Entry(top)
        self.e.pack()

        btn = tk.Button(top, text="X", command=self.X)
        btn.pack()
        btn = tk.Button(top, text="Y", command=self.Y)
        btn.pack()
        btn = tk.Button(top, text="Z", command=self.Z)
        btn.pack()
        btn = tk.Button(top, text="A", command=self.A)
        btn.pack()
        btn = tk.Button(top, text="B", command=self.B)
        btn.pack()
        btn = tk.Button(top, text="C", command=self.C)
        btn.pack()

        btn = tk.Button(top, text="setWRF", command=self.setWRF, bd=10)
        btn.pack(side=tk.BOTTOM)
        
        fac = Factory("DeviceFactory").select("motion")
        NIClist = fac.selectValueList()

        for nic in NIClist:
            if nic.find("192.168.0.") is 0:
                fac = fac.select(nic)
                break

        print("Retrieve robot factory at the default robot IP address... ")
    
        try:
            fac = fac.select("192.168.0.100")
        except:
            print "Unexpected error:", sys.exc_info()[0]
            raise
    
        print("Create robot from its factory")
        self.robo = fac.create("robo")
        print("Robot module: " + self.robo.name + " was created. (" + self.robo.internalName + ") ")

        
    def X(self):
        pos = self.robo.getParameterValue("xAxisPos") + float(self.e.get())
        print("move to new x pos: " + str(pos))
        self.robo.setParameterValue("xAxisPos",pos)
        
    def Y(self):
        pos = self.robo.getParameterValue("yAxisPos") + float(self.e.get())
        print("move to new y pos: " + str(pos))
        self.robo.setParameterValue("yAxisPos",pos)

    def Z(self):
        pos = self.robo.getParameterValue("zAxisPos") + float(self.e.get())
        print("move to new z pos: " + str(pos))
        self.robo.setParameterValue("zAxisPos",pos)

    def A(self):
        pos = self.robo.getParameterValue("aAxisPos") + float(self.e.get())
        print("move to new a pos: " + str(pos))
        self.robo.setParameterValue("aAxisPos",pos)

    def B(self):
        pos = self.robo.getParameterValue("bAxisPos") + float(self.e.get())
        print("move to new b pos: " + str(pos))
        self.robo.setParameterValue("bAxisPos",pos)

    def C(self):
        pos = self.robo.getParameterValue("cAxisPos") + float(self.e.get())
        print("move to new c pos: " + str(pos))
        self.robo.setParameterValue("cAxisPos",pos)

    def setWRF(self):
        print("setWRF with current coord (TBD)")
    

    
# main body    
import sys
import os
import Tkinter as tk
    
if len(sys.argv) >= 1:
    # probably called from InstrumentAll
    checker = os.path.basename(sys.argv[0])
    if checker == "instrumentall" or checker == "instrumentall.exe":
        print("current script: ",os.path.realpath(__file__))
        
        top = tk.Tk()
        top.title("Robot motion control")
        top.minsize(300,300)
        app = App(top)

        top.mainloop()

        exit(0)

print("Presumably not called from InstrumentAll >> Exiting...")

exit("This script has to be launched from inside InstrumentAll")
