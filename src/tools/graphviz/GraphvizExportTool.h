/**
 * @file	src/tools/graphviz/GraphvizExportTool.h
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

#include "Poco/Path.h"
#include "Poco/SharedPtr.h"

#include <vector>
#include <ostream>

using Poco::SharedPtr;

// #define OBSOLETE_RECORD

class Module;
class DataProxy;
class DataLogger;
class DuplicatedSource;
class DataSource;
class DataTarget;

/**
 * Tool to export a data structure into a dot (graphviz) graph.
 *
 * This dot can either be retrieved as a file or as a string.
 */
class GraphvizExportTool
{
public:
	GraphvizExportTool() { }
	virtual ~GraphvizExportTool() { }

	/**
	 * Export the workflow structure in a file.
	 *
	 * @param filePath file path in which to write the graph
	 */
	void writeDotFile(Poco::Path filePath);

	/**
	 * Retrieve the workflow structure as a string.
	 */
	std::string getDotString(bool withEdges = true);

protected:
	virtual void exportGraph(std::ostream& out) = 0;

	/**
	 * Export a module
	 */
	void exportModuleNode(std::ostream& out, SharedPtr<Module*> mod);

    /**
     * Export a data proxy
     */
    void exportDataProxyNode(std::ostream& out, DataProxy* proxy);

    /**
     * Export a data logger
     */
    void exportDataLoggerNode(std::ostream& out, DataLogger* logger);

    /**
     * Export a duplicated data source
     */
    void exportDuplicatedDataNode(std::ostream& out, DuplicatedSource* source);

    std::string getPortName(DataSource* source);
    std::string getPortName(DataTarget* target);

    std::string portNameSuffix(std::string complete);
};

#endif /* SRC_GRAPHVIZEXPORTTOOL_H_ */
