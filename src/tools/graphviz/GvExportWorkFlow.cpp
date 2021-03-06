/**
 * @file	src/tools/graphviz/GvExportWorkFlow.cpp
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

#include "core/Module.h"
#include "core/InPort.h"
#include "core/SeqSource.h"
#include "core/SeqTarget.h"
#include "core/OutPort.h"
#include "core/DataProxy.h"
#include "core/DataLogger.h"
#include "core/DuplicatedSource.h"
#include "core/Dispatcher.h"

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

        for (std::vector<OutPort*>::iterator pit = ports.begin(),
                pite = ports.end(); pit != pite; pit++)
        {
            // retrieve shared port from dispatcher
            SharedPtr<OutPort*> port =
                    Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getOutPort(*pit);

            if ((*port)->getDataTargets().size())
                propagateTopDown(out, *port);

            std::set<SeqTarget*> seqTgts = (*pit)->getSeqTargets();
            seqTargets.insert(seqTgts.begin(), seqTgts.end());
        }

        std::vector<InPort*> morePorts = (**it)->getInPorts();

        for (std::vector<InPort*>::iterator pit = morePorts.begin(),
                pite = morePorts.end(); pit != pite; pit++)
        {
            // retrieve shared port from dispatcher
            SharedPtr<InPort*> morePort =
                    Poco::Util::Application::instance()
                    .getSubsystem<Dispatcher>()
                    .getInPort(*pit);

            propagateBottomUp(out, *morePort);

            if ((*pit)->getSeqSource())
                seqTargets.insert(*pit);
        }

        // check for parameter workers
        std::set< Poco::AutoPtr<ParameterGetter> > paramGetters = (**it)->getParameterGetters();
        for (std::set< Poco::AutoPtr<ParameterGetter> >::iterator git = paramGetters.begin(),
                gite = paramGetters.end(); git != gite; git++)
        {
            propagateTopDown(out, const_cast<ParameterGetter*>(git->get()));
        }

        std::set< Poco::AutoPtr<ParameterSetter> > paramSetters = (**it)->getParameterSetters();
        for (std::set< Poco::AutoPtr<ParameterSetter> >::iterator sit = paramSetters.begin(),
                site = paramSetters.end(); sit != site; sit++)
        {
            propagateBottomUp(out, const_cast<ParameterSetter*>(sit->get()));
        }
    }

    out << std::endl;
}

void GvExportWorkFlow::propagateTopDown(std::ostream& out, DataSource* source)
{
    std::set<DataTarget*> targets = source->getDataTargets();

    for (std::set<DataTarget*>::iterator it = targets.begin(),
            ite = targets.end(); it != ite; it++)
    {
        involvedTargets.insert(*it);

        // target is: a module
        if ( dynamic_cast<InPort*>(*it)
                || dynamic_cast<ParameterSetter*>(*it) )
            continue;

        // target is: data proxy
        DataProxy* proxy = dynamic_cast<DataProxy*>(*it);
        if (proxy)
        {
            if (proxies.insert(proxy).second)
            {
                exportDataProxyNode(out, proxy);
                propagateTopDown(out, proxy);
            }
            continue;
        }

        // target is: data logger
        DataLogger* logger = dynamic_cast<DataLogger*>(*it);
        if (logger)
        {
            if (loggers.insert(logger).second)
                exportDataLoggerNode(out, logger);
            continue;
        }

        throw Poco::NotImplementedException("GraphvizExport->propagateTopDown",
                "The given data target is not recognized");
    }
}

void GvExportWorkFlow::propagateBottomUp(std::ostream& out, DataTarget* target)
{
    DataSource* source;

    try
    {
        source = target->getDataSource();
    }
    catch (Poco::NullPointerException&)
    {
        return;
    }

    involvedTargets.insert(target);

    SeqTarget* seqTgt = dynamic_cast<SeqTarget*>(target);
    if (seqTgt && seqTgt->getSeqSource())
        seqTargets.insert(seqTgt);

    // source is: module out port
    if ( dynamic_cast<OutPort*>(source)
            || dynamic_cast<ParameterGetter*>(source) )
        return;

    // source is: duplicated source
    DuplicatedSource* dupSrc = dynamic_cast<DuplicatedSource*>(source);
    if (dupSrc)
    {
        if (dupSources.insert(dupSrc).second)
            exportDuplicatedSourceNode(out, dupSrc);
        return;
    }

    // source is: data proxy
    DataProxy* proxy = dynamic_cast<DataProxy*>(source);
    if (proxy)
    {
        if (proxies.insert(proxy).second)
        {
            exportDataProxyNode(out, proxy);
            propagateBottomUp(out, proxy);
        }
        return;
    }

    throw Poco::NotImplementedException("GraphvizExport->propagateBottomUp",
            "The given data source is not recognized");

}

void GvExportWorkFlow::exportEdges(std::ostream& out)
{
    out << "    /* edges */" << std::endl;

    for (std::set<DataTarget*>::iterator it = involvedTargets.begin(),
            ite = involvedTargets.end(); it != ite; it++)
    {
        DataSource* source = (*it)->getDataSource();
        out << "    " << getPortName(source) << " -> ";
        out << getPortName(*it) ;
        out << " ;" << std::endl;
    }

    out << std::endl;
}

void GvExportWorkFlow::exportSeqEdges(std::ostream& out)
{
    out << "    /* sequence edges */" << std::endl;

    for (std::set<SeqTarget*>::iterator it = seqTargets.begin(),
            ite = seqTargets.end(); it != ite; it++)
    {
        SeqSource* source = (*it)->getSeqSource();
        out << "    " << getPortName(source) << " -> ";
        out << getPortName(*it) << " ";
        out << "[style=dotted];" << std::endl;
    }

    out << std::endl;
}

void GvExportWorkFlow::exportEdge(std::ostream& out, DataSource* source, DataTarget* target)
{
}

