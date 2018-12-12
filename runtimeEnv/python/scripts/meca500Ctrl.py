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
        import sys
        
        btn = tk.Button(top, text="Init", command=self.init)
        btn.pack()
        btn = tk.Button(top, text="Reset error", command=self.resetError)
        btn.pack()

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

        self.qu = tk.Entry(top)
        self.qu.pack()
        
        btn = tk.Button(top, text="Query", command=self.query)
        btn.pack()
        btn = tk.Button(top, text="Response", command=self.response)
        btn.pack()

        btn = tk.Button(top, text="Park", command=self.park)
        btn.pack()

        btn = tk.Button(top, text="setWRF", command=self.setWRF, bd=10)
        btn.pack(side=tk.BOTTOM)
        
        btn = tk.Button(top, text="Quit", command=top.quit)
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

    def init(self):
        import tkMessageBox as msg
        
        msg.showwarning(title="Set references", message="""
        Redefining TRF and WRF.

        Please, take care of the robot movements. """)

        self.robo.setParameterValue("query",
            "SetWRF(300,0,250,0,0,0)")

        if not msg.askokcancel("Movement", "Ready to move? "):
            return

        self.robo.setParameterValue("query","MovePose(0,0,0,0,0,0)")
        self.robo.getParameterValue("query")

        resp = msg.showinfo(title="Init", message="Initializing... ")
        self.robo.getParameterValue("query")

    def resetError(self):
        self.robo.setParameterValue("query","ResetError();ResumeMotion()")
        self.robo.getParameterValue("query")
        
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

    def query(self):
        self.robo.setParameterValue("query",self.qu.get())

    def response(self):
        import tkMessageBox as msg
        msg.showinfo(title="Response", message=self.robo.getParameterValue("query"))
        
    def setWRF(self):
        import tkMessageBox as msg
        self.robo.setParameterValue("query","SetWRF(0,0,0,0,0,0)")
        x = self.robo.getParameterValue("xAxisPos")
        y = self.robo.getParameterValue("yAxisPos")
        z = self.robo.getParameterValue("zAxisPos")
        a = self.robo.getParameterValue("aAxisPos")
        b = self.robo.getParameterValue("bAxisPos")
        c = self.robo.getParameterValue("cAxisPos")
        print("setWRF with current coord (" + str(x) + ";" +
              str(y) + ";" +
              str(z) + ";" +
              str(a) + ";" +
              str(b) + ";" +
              str(c) + ")")
        self.robo.setParameterValue("query","SetWRF(" +
              str(x) + "," +
              str(y) + "," +
              str(z) + "," +
              str(a) + "," +
              str(b) + "," +
              str(c) + ")")
        print("done. ")
        self.robo.getParameterValue("query")
        
        if (abs(self.robo.getParameterValue("xAxisPos"))>0.001 or
            abs(self.robo.getParameterValue("yAxisPos"))>0.001 or 
            abs(self.robo.getParameterValue("zAxisPos"))>0.001 or 
            abs(self.robo.getParameterValue("aAxisPos"))>0.001 or 
            abs(self.robo.getParameterValue("bAxisPos"))>0.001 or 
            abs(self.robo.getParameterValue("cAxisPos"))>0.001): 
            msg.showerror("setWRF","Set WRF failed! Pose is not zero. ")

    def park(self):
        import tkMessageBox as msg

        if not msg.askokcancel("Movement", "Ready to park? "):
            return
        self.robo.setParameterValue("query","MoveJoints(0,-45,70,0,20,0)")

        msg.showinfo(title="Parking", message="Parking... ")
        self.robo.getParameterValue("query")

        
    
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
