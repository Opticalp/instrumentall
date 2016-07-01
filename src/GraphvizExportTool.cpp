/**
 * @file	src/GraphvizExportTool.cpp
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

#include "GraphvizExportTool.h"

#include "Module.h"

#include <fstream>
#include <sstream>

GraphvizExportTool::GraphvizExportTool(std::vector< Poco::SharedPtr<Module*> > modules):
	modulesList(modules)
{

}

void GraphvizExportTool::writeDotFile(Poco::Path filePath, bool withEdges)
{
	std::ofstream file(filePath.toString().c_str());

	if (file.is_open())
		exportGraph(file, withEdges);
	else
		throw Poco::FileException("writeDotFile",
				"not able to open the file "
				+ filePath.toString()
				+ " for writing");

	file.close();
}

std::string GraphvizExportTool::getDotString(bool withEdges)
{
	std::ostringstream stream;
	exportGraph(stream, withEdges);

	return stream.str();
}

void GraphvizExportTool::exportGraph(std::ostream& out, bool withEdges)
{
	out << "/* instrumentall workflow */" << std::endl;
	out << "digraph G {" << std::endl;

	exportNodes(out);

	if (withEdges)
		exportEdges(out);

	out << "}" << std::endl;
}

void GraphvizExportTool::exportNodes(std::ostream& out)
{
	out << "node [shape=record];" << std::endl;
	out << "/* available nodes */" << std::endl;

	for (std::vector< Poco::SharedPtr<Module*> >::iterator it = modulesList.begin(),
			ite = modulesList.end(); it != ite; it++)
	{
		exportNode(out, *it);

		// TODO: for each out port
		// if it has a target, push it into outPorts.
	}
}

void GraphvizExportTool::exportNode(std::ostream& out,
		Poco::SharedPtr<Module*> mod)
{
	size_t rows;
	size_t cols;

	out << "\"" << (*mod)->name() << "\";" << std::endl;
}

void GraphvizExportTool::exportEdges(std::ostream& out)
{
	out << "/* edges */" << std::endl;
}

void GraphvizExportTool::exportSeqEdges(std::ostream& out)
{
	out << "/* sequence edges */" << std::endl;
}
