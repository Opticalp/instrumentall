/**
 * @file	src/GvExportWorkFlow.h
 * @date	Jul. 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_GVEXPORTWORKFLOW_H_
#define SRC_GVEXPORTWORKFLOW_H_

#include "GraphvizExportTool.h"

class OutPort;

/**
 * Export the workflow as a graphviz dot graph
 *
 *  - parameter names are prefixed with "param_"
 *  - input port names are prefixed with "inPort_"
 *  - output port names are prefixed with "outPort_"
 *
 */
class GvExportWorkFlow: public GraphvizExportTool
{
public:
	GvExportWorkFlow(std::vector< SharedPtr<Module*> > modules, bool withEdges = true);
	virtual ~GvExportWorkFlow() { }

private:
	/// Should not be used
	GvExportWorkFlow();

	void exportGraph(std::ostream& out);

	void exportNodes(std::ostream& out);
	void exportEdges(std::ostream& out);
	void exportSeqEdges(std::ostream& out);

	std::vector< SharedPtr<Module*> > modulesList; ///< all the modules
	std::vector< SharedPtr<OutPort*> > outPorts; ///< output ports that have targets
	std::vector< SharedPtr<OutPort*> > outSeqPorts; ///< output ports that have seq targets

	bool drawEdges;
};

#endif /* SRC_GVEXPORTWORKFLOW_H_ */
