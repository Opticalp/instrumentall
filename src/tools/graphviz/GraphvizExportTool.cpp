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
#include "core/DataProxy.h"
#include "core/DataLogger.h"
#include "core/DuplicatedSource.h"
#include "core/ParameterGetter.h"
#include "core/ParameterSetter.h"
#include "core/Dispatcher.h"

#include "Poco/Util/Application.h"
#include "Poco/String.h"

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

		out << "<" << portNameBase(getPortName(*port)) << "> " << (*port)->name();
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

		out << "<" << portNameBase(getPortName(*port)) << "> " << (*port)->name();
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

					out << "        <TD PORT=\"" << portNameBase(getPortName(*port)) << "\">";
					out << (*port)->name() << "</TD>" << std::endl;
				}
				else if ((col-1)%outP.size() == 0)
				{
					SharedPtr<InPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getInPort(inP.at((col-1)/outP.size()));

					out << "        <TD COLSPAN=\"" << outP.size() << "\" ";
					out << "PORT=\"" << portNameBase(getPortName(*port)) << "\">";
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
#if POCO_VERSION > 0x01030600
				out << Poco::replace((*mod)->description(),"\n","<BR/>") ;
#else
				// Poco::replace has a bug in amd64 debian build for rev <= 1.3.6p1
				out << (*mod)->description() ;
#endif
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

					out << "        <TD PORT=\"" << portNameBase(getPortName(*port)) << "\">";
					out << (*port)->name() << "</TD>" << std::endl;
				}
				else if ((col-1)%inP.size() == 0)
				{
					SharedPtr<OutPort*> port =
							Poco::Util::Application::instance()
							.getSubsystem<Dispatcher>()
							.getOutPort(outP.at((col-1)/inP.size()));

					out << "        <TD COLSPAN=\"" << inP.size() << "\" ";
					out << "PORT=\"" << portNameBase(getPortName(*port)) << "\">";
					out << (*port)->name() << "</TD>" << std::endl;;
				}
			}
		}
		out << "      </TR>" << std::endl;
	}

	out << "      </TABLE>\n    >];" << std::endl;
#endif
}

void GraphvizExportTool::exportDataProxyNode(std::ostream& out,
        DataProxy* proxy)
{
    out << "    " << proxy->name() << " [shape=box, label=\"";
    out << "data proxy: \\n" << proxy->name() << "\"];" << std::endl;
}

void GraphvizExportTool::exportDataLoggerNode(std::ostream& out,
        DataLogger* logger)
{
    out << "    " << logger->name() << " [shape=box, label=\"";
    out << "data logger: \\n" << logger->name() << "\"];" << std::endl;
}

void GraphvizExportTool::exportDuplicatedSourceNode(std::ostream& out,
        DuplicatedSource* source)
{
    out << "    " << source->name()
        << " [shape=box, label=\""
                  << source->name()
        << "\"];"
        << std::endl;
}

std::string GraphvizExportTool::getPortName(DataSource* source)
{
    // source is: module out port
    OutPort* outPort = dynamic_cast<OutPort*>(source);
    if (outPort)
        return outPort->parent()->name() + ":outPort_" + outPort->name() + ":s";

    // source is: parameter getter
    ParameterGetter* paramGet = dynamic_cast<ParameterGetter*>(source);
    if (paramGet)
        return paramGet->getParent()->name() + ":param_" + paramGet->getParameterName() + ":w";

    // source is: duplicated source
    DuplicatedSource* dupSrc = dynamic_cast<DuplicatedSource*>(source);
    if (dupSrc)
        return dupSrc->name() + ":s";

    // source is: data proxy
    DataProxy* proxy = dynamic_cast<DataProxy*>(source);
    if (proxy)
        return proxy->name() + ":s";

    throw Poco::NotImplementedException("GraphvizExport->getPortName",
            "The given data source is not recognized: " + source->name());
}

std::string GraphvizExportTool::getPortName(DataTarget* target)
{
    // target is: module in port
    InPort* inPort = dynamic_cast<InPort*>(target);
    if (inPort)
        return inPort->parent()->name() + ":inPort_" + inPort->name() + ":n";

    // target is: parameter getter
    ParameterGetter* paramGet = dynamic_cast<ParameterGetter*>(target);
    if (paramGet)
        return paramGet->getParent()->name() + ":param_" + paramGet->getParameterName() + ":w";

    // target is: parameter setter
    ParameterSetter* paramSet = dynamic_cast<ParameterSetter*>(target);
    if (paramSet)
        return paramSet->getParent()->name() + ":param_" + paramSet->getParameterName() + ":w";

    // target is: data proxy
    DataProxy* proxy = dynamic_cast<DataProxy*>(target);
    if (proxy)
        return proxy->name() + ":n";

    // target is: data logger
    DataLogger* logger = dynamic_cast<DataLogger*>(target);
    if (logger)
        return logger->name() + ":n";

    throw Poco::NotImplementedException("GraphvizExport->getPortName",
            "The given data target is not recognized: " + target->name());
}

#include "Poco/StringTokenizer.h"

std::string GraphvizExportTool::portNameBase(std::string complete)
{
    Poco::StringTokenizer tok(complete, ":");
    if (tok.count() == 3)
        return tok[1];
    else
        poco_bugcheck_msg(("bad port name: " + complete
                + ", can not be split").c_str() );

    throw Poco::BugcheckException();
}
