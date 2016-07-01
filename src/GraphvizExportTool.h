/**
 * @file	src/GraphvizExportTool.h
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

#ifndef SRC_GRAPHVIZEXPORTTOOL_H_
#define SRC_GRAPHVIZEXPORTTOOL_H_

#include "Poco/SharedPtr.h"
#include "Poco/Path.h"

#include <vector>
#include <ostream>

class Module;
class OutPort;

/**
 * Tool to export the workflow structure into a dot (graphviz) graph.
 *
 * This dot can either be retrieved as a file or as a string.
 */
class GraphvizExportTool
{
public:
	GraphvizExportTool(std::vector< Poco::SharedPtr<Module*> > modules);
	virtual ~GraphvizExportTool() { }

	/**
	 * Export the workflow structure in a file.
	 *
	 * @param filePath file path in which to write the graph
	 * @param withEdges Determine if only the nodes/modules are to be
	 * exported (false) or if the edges are to be exported
	 * too (true).
	 */
	void writeDotFile(Poco::Path filePath, bool withEdges = true);

	/**
	 * Retrieve the workflow structure as a string.
	 *
	 * @param withEdges Determine if only the nodes/modules are to be
	 * exported (false) or if the edges are to be exported
	 * too (true).
	 */
	std::string getDotString(bool withEdges = true);

private:
	/// Should not be used
	GraphvizExportTool();

	void exportGraph(std::ostream& out, bool withEdges);

	void exportNodes(std::ostream& out);
	void exportNode(std::ostream& out, Poco::SharedPtr<Module*> mod);
	void exportEdges(std::ostream& out);
	void exportSeqEdges(std::ostream& out);

	std::vector< Poco::SharedPtr<Module*> > modulesList; ///< all the modules
	std::vector< Poco::SharedPtr<OutPort*> > outPorts; ///< output ports that have targets
};

#endif /* SRC_GRAPHVIZEXPORTTOOL_H_ */
