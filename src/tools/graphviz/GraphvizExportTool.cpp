/**
 * @file	src/tools/graphviz/GraphvizExportTool.cpp
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

#include "core/Module.h"
#include "core/InPort.h"
#include "core/OutPort.h"
#include "core/Dispatcher.h"

#include "Poco/Util/Application.h"

#include <fstream>
#include <sstream>

void GraphvizExportTool::writeDotFile(Poco::Path filePath)
{
	std::ofstream file(filePath.toString().c_str());

	if (file.is_open())
		exportGraph(file);
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
	exportGraph(stream);

	return stream.str();
}

void GraphvizExportTool::exportModuleNode(std::ostream& out,
		SharedPtr<Module*> mod)
{
	ParameterSet pSet;
	(*mod)->getParameterSet(&pSet);

	std::vector<InPort*> inP = (*mod)->getInPorts();
	std::vector<OutPort*> outP = (*mod)->getOutPorts();

#ifdef OBSOLETE_RECORD
	out << "    " << (*mod)->name() << " [shape=record, label=\"{";

	// parameters
	for (ParameterSet::iterator it = pSet.begin(),
			ite = pSet.end(); it != ite; )
	{
		out << "<param_" << it->name << "> " << it->name;
		it++;
		if (it != ite)
			out << " | ";
	}
	out << "} | { {";

	// input ports
	for (std::vector<InPort*>::iterator it = inP.begin(),
			ite = inP.end(); it != ite; )
	{
		SharedPtr<InPort*> port =
				Poco::Util::Application::instance()
				.getSubsystem<Dispatcher>()
				.getInPort(*it);

		out << "<inPort_" << (*port)->name() << "> " << (*port)->name();
		it++;
		if (it != ite)
			out << " | ";
	}
	out << "} | " << (*mod)->name() << " | {";

	// output ports
	for (std::vector<OutPort*>::iterator it = outP.begin(),
			ite = outP.end(); it != ite; )
	{
		SharedPtr<OutPort*> port =
			Poco::Util::Application::instance()
			.getSubsystem<Dispatcher>()
			.getOutPort(*it);

		out << "<outPort_" << (*port)->name() << "> " << (*port)->name();
		it++;
		if (it != ite)
			out << " | ";
	}
	out << "} }\"];" << std::endl;
#else
	size_t rows = pSet.size();
	bool paramSpan = false;
	if (rows < 3)
	{
		if (pSet.size()==2)
			rows = 4;
		else
			rows = 3;
		paramSpan = true;
	}

	size_t cols;
	bool colSpan = false;
	if (inP.empty() && outP.empty())
		cols = 2;
	else if (inP.empty())
		cols = 1 + outP.size();
	else if (outP.empty())
		cols = 1+ inP.size();
	else if (inP.size() == outP.size())
		cols = 1 + inP.size();
	else
	{
		cols = 1 + inP.size() * outP.size();
		colSpan = true;
	}

	out << "    " << (*mod)->name() << " [shape=plaintext, label=<" << std::endl;
	out << "      <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">" << std::endl;

	for (size_t row=0; row<rows; row++)
	{
		out << "      <TR>" << std::endl;
		for (size_t col=0; col<cols; col++)
		{

			if (col == 0) 			           // parameter
			{
				if (!paramSpan) // row == param index
				{
					out << "        <TD PORT=\"param_" << pSet.at(row).name << "\">";
					out << pSet.at(row).name << "</TD>" << std::endl;
				}
				else
				{
					switch(pSet.size())
					{
					case 2:
						if (row%2==0)
						{
							out << "        <TD ROWSPAN=\"2\" PORT=\"param_" << pSet.at(row/2).name << "\">";
							out << pSet.at(row/2).name << "</TD>" << std::endl;
						}
						break;
					case 1:
						if (row==0)
						{
							out << "        <TD ROWSPAN=\"3\" PORT=\"param_" << pSet.at(row).name << "\">";
							out << pSet.at(row).name << "</TD>" << std::endl;
						}
						break;
					case 0:
						if (row==0)
						{
							out << "        <TD ROWSPAN=\"3\"> </TD>" << std::endl;
						}
						break;
					default:
						poco_bugcheck_msg("wrong parameterSet size");
					}
				}

			}
			else if (row == 0)                 // input port
			{
				if (inP.empty())
				{
					if (col==1)
					{
						out << "        <TD COLSPAN=\"" << cols-1 << "\"> </TD>" << std::endl;
					}
				}
				else if (!colSpan)
				{
					SharedPtr<InPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getInPort(inP.at(col-1));

					out << "        <TD PORT=\"inPort_" << (*port)->name() << "\">";
					out << (*port)->name() << "</TD>" << std::endl;
				}
				else if ((col-1)%outP.size() == 0)
				{
					SharedPtr<InPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getInPort(inP.at((col-1)/outP.size()));

					out << "        <TD COLSPAN=\"" << outP.size() << "\" ";
					out << "PORT=\"inPort_" << (*port)->name() << "\">";
					out << (*port)->name() << "</TD>" << std::endl;;
				}
			}
			else if ((row == 1) && (col == 1)) // module name
			{
				out << "        <TD ROWSPAN=\"" << rows-2 << "\" ";
				out << "COLSPAN=\"" << cols-1 << "\"  ";
				out << "PORT=\"" << (*mod)->name() << "\"> ";
				out << "<FONT POINT-SIZE=\"20\"><B>";
				out << (*mod)->name() << "</B></FONT>";
				out << "<I> (" << (*mod)->internalName() << ") </I><BR/>";
				out << (*mod)->description() ;
				out << " </TD> " << std::endl;
			}
			else if (row == rows-1)
			{
				if (outP.empty())
				{
					if (col==1)
					{
						out << "        <TD COLSPAN=\"" << cols-1 << "\"> </TD>" << std::endl;
					}
				}
				else if (!colSpan)
				{
					SharedPtr<OutPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getOutPort(outP.at(col-1));

					out << "        <TD PORT=\"outPort_" << (*port)->name() << "\">";
					out << (*port)->name() << "</TD>" << std::endl;
				}
				else if ((col-1)%inP.size() == 0)
				{
					SharedPtr<OutPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getOutPort(outP.at((col-1)/inP.size()));

					out << "        <TD COLSPAN=\"" << inP.size() << "\" ";
					out << "PORT=\"outPort_" << (*port)->name() << "\">";
					out << (*port)->name() << "</TD>" << std::endl;;
				}
			}
		}
		out << "      </TR>" << std::endl;
	}

	out << "      </TABLE>\n    >];" << std::endl;
#endif
}
