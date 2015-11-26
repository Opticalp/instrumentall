# -*- coding: utf-8 -*-

## @file     conf/console.py
## @date     jun. 2013
## @author   PhRG / opticalp.fr
## @license  MIT

#
# Copyright (c) 2013 Ph. Renaud-Goud / Opticalp
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

import code

class Quitter(object):
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return 'Use %s() to exit' % (self.name)
    def __call__(self, code=None):
        raise SystemExit(code)

def func():
    """main function"""
    
    # dictionary definition to transmit local variables
    dico = {} 
    dico['quit']=Quitter('quit')
    dico['exit']=Quitter('exit')
    
    try:
        import instru
    except ImportError:
        print 'WARNING: "instru" module is not available...'
    else:
        print '"instru" module loaded...'
        dico['instru']=instru
    
    # interactive console launch
    try:    
        code.interact(local=dico)
    except SystemExit:
        print "console.py: We just left the interactive console on SystemExit exception..."            

# Main logic: launch it all
func()
