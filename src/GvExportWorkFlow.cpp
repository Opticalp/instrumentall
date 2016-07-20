/**
 * @file	src/GvExportWorkFlow.cpp
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

#include "GvExportWorkFlow.h"

#include "Module.h"
#include "InPort.h"
#include "OutPort.h"
#include "Dispatcher.h"

#include "Poco/Util/Application.h"

GvExportWorkFlow::GvExportWorkFlow(std::vector<SharedPtr<Module*> > modules,
		bool withEdges):
		modulesList(modules),
		drawEdges(withEdges)
{
}

void GvExportWorkFlow::exportGraph(std::ostream& out)
{
	out <<
		   "/** \n"
		   " * Instrumentall workflow \n"
		   " * \n"
	       " * - parameter names are prefixed with \"param_\"\n"
           " * - input port names are prefixed with \"inPort_\"\n"
           " * - output port names are prefixed with \"outPort_\"\n"
           " * \n"
           " * A .png image can be generated using: \n"
           " * \n"
           " *     dot -Tpng -o <imageName>.png < <thisFile>\n"
           " * \n"
           " */"
		<< std::endl;

 	out << "digraph workflow {\n" << std::endl;

	exportNodes(out);

	if (drawEdges)
	{
		exportEdges(out);
		exportSeqEdges(out);
	}

	out << "}" << std::endl;
}

void GvExportWorkFlow::exportNodes(std::ostream& out)
{
    out << "    /* available nodes */" << std::endl;

    for (std::vector< SharedPtr<Module*> >::iterator it = modulesList.begin(),
            ite = modulesList.end(); it != ite; it++)
    {
        exportModuleNode(out, *it);

        std::vector<OutPort*> ports = (**it)->getOutPorts();

        for (std::vector<OutPort*>::iterator it = ports.begin(),
                ite = ports.end(); it != ite; it++)
        {
            // retrieve shared port from dispatcher
            SharedPtr<OutPort*> port =
                    Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getOutPort(*it);

            if ((*port)->getDataTargets().size())
                outPorts.push_back(port);

            if ((*port)->getSeqTargets().size())
                outSeqPorts.push_back(port);
        }
    }

    out << std::endl;
}

void GvExportWorkFlow::exportEdges(std::ostream& out)
{
    out << "    /* edges */" << std::endl;

    for (std::vector< SharedPtr<OutPort*> >::iterator it = outPorts.begin(),
            ite = outPorts.end(); it != ite; it++)
    {
        out << "    " << (**it)->parent()->name() << ":outPort_" << (**it)->name() << ":s -> { ";

//	FIXME
//        std::set<DataTargets*> targets = (**it)->getDataTargets();
//
//        for (std::vector<SharedPtr<InPort*> >::iterator tgtIt = targets.begin(),
//                tgtIte = targets.end(); tgtIt != tgtIte; tgtIt++)
//            out << (**tgtIt)->parent()->name() << ":inPort_" << (**tgtIt)->name() << ":n ";

        out << "};" << std::endl;
    }

    out << std::endl;
}

void GvExportWorkFlow::exportSeqEdges(std::ostream& out)
{
    out << "    /* sequence edges */" << std::endl;

    for (std::vector< SharedPtr<OutPort*> >::iterator it = outSeqPorts.begin(),
            ite = outSeqPorts.end(); it != ite; it++)
    {
        out << "    " << (**it)->parent()->name() << ":outPort_" << (**it)->name() << ":s -> { ";

        std::vector<SharedPtr<InPort*> > targets = (**it)->getSeqTargets();

        for (std::vector<SharedPtr<InPort*> >::iterator tgtIt = targets.begin(),
                tgtIte = targets.end(); tgtIt != tgtIte; tgtIt++)
            out << (**tgtIt)->parent()->name() << ":inPort_" << (**tgtIt)->name() << ":n ";

        out << "}[style=dotted];" << std::endl;
    }

    out << std::endl;
}
