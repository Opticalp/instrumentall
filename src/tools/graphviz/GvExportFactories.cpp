/**
 * @file	src/tools/graphviz/GvExportFactories.cpp
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

#include "GvExportFactories.h"

#include "core/ModuleFactory.h"
#include "core/ModuleFactoryBranch.h"

#include <list>

GvExportFactories::GvExportFactories(
        std::vector<SharedPtr<ModuleFactory*> > factories, bool withModules):
        drawModules(withModules)
{
    for (std::vector<SharedPtr<ModuleFactory*> >::iterator it = factories.begin(),
            ite = factories.end(); it != ite; it++)
        factorySet.insert(*it);

    seekChildrenFactories();

    if (drawModules)
        seekChildrenModules();
}

void GvExportFactories::exportGraph(std::ostream& out)
{
    out <<
           "/** \n"
           " * Instrumentall factories tree \n"
           " * \n"
           " * A .png image can be generated using: \n"
           " * \n"
           " *     dot -Tpng -o <imageName>.png < <thisFile>\n"
           " * \n"
           " */"
        << std::endl;

    out << "digraph facTree {\n"
            "    rankdir=LR;\n" << std::endl;
    exportFactories(out);
    exportFactoriesEdges(out);

    if (drawModules && modulesList.size())
    {
        exportModules(out);
        exportModulesEdges(out);
    }

    out << "}" << std::endl;
}

void GvExportFactories::seekChildrenFactories()
{
    std::list< SharedPtr<ModuleFactory*> > tmpFacList(factorySet.begin(), factorySet.end());

    while (!tmpFacList.empty())
    {
        SharedPtr<ModuleFactory*> item = tmpFacList.front();
        tmpFacList.pop_front();

        if ((*item)->isLeaf())
            continue;

        std::vector< Poco::SharedPtr<ModuleFactory*> > children = (*item)->getChildFactories();

        for (std::vector< SharedPtr<ModuleFactory*> >::iterator child = children.begin(),
                childe = children.end(); child != childe; child++)
        {
            tmpFacList.push_back(*child);
            factorySet.insert(*child);
        }
    }
}

void GvExportFactories::seekChildrenModules()
{
    for (std::set< SharedPtr<ModuleFactory*> >::iterator it = factorySet.begin(),
            ite = factorySet.end(); it != ite; it++)
    {
        if (!(**it)->isLeaf())
            continue;

        std::vector< Poco::SharedPtr<Module*> > children = (**it)->getChildModules();

        for (std::vector< SharedPtr<Module*> >::iterator child = children.begin(),
                childe = children.end(); child != childe; child++)
            modulesList.push_back(*child);
    }
}

void GvExportFactories::exportFactories(std::ostream& out)
{
    std::vector<std::string> rootFacts;

    out << "    /* factory nodes */" << std::endl;

    for (std::set< SharedPtr<ModuleFactory*> >::iterator it = factorySet.begin(),
            ite = factorySet.end(); it != ite; it++)
    {
        if ((**it)->isRoot())
            rootFacts.push_back((**it)->name());

        exportFactory(out, *it);
    }
    out << std::endl;

    // if more than 2 are roots -> same rank.
    if (rootFacts.size() >= 2)
    {
        out << "    /* root factories */" << std::endl;
        out << "    { rank = same; " << std::endl;
        for (std::vector<std::string>::iterator it = rootFacts.begin(),
                ite = rootFacts.end(); it != ite; it++)
        {
            out << "        \"" << *it << "\"; \n";
        }
        out << "    }" << std::endl;
    }
    out << std::endl;
}

void GvExportFactories::exportFactory(std::ostream& out,
        SharedPtr<ModuleFactory*> fact)
{
#ifdef OBSOLETE_RECORD
    out << "    " << (*fact)->name() << " [shape=record, label=\"{";
    out << (*fact)->name();

    if ((*fact)->isLeaf())
    {
        out << "}\"];" << std::endl;
        return;
    }

    out << " | { ";

    std::vector<std::string> selectors = (*fact)->selectValueList();

    // selectors
    for (std::vector<std::string>::iterator it = selectors.begin(),
            ite = selectors.end(); it != ite; )
    {
        out << "<" << *it << "> " << *it;
        it++;
        if (it != ite)
            out << " | ";
    }

    out << "} }\"];" << std::endl;
#else
    out << "    " << (*fact)->name() << " [shape=plaintext, label=<" << std::endl;
    out << "      <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">" << std::endl;

    if ((*fact)->isLeaf())
    {
        out << "        <TR><TD><B>" << (*fact)->name() << "</B></TD></TR>\n"
               "      </TABLE>\n"
               "    >];" <<std::endl;
        return;
    }

    std::vector<std::string> selectors = (*fact)->selectValueList();

    out << "        <TR>\n"
           "          <TD";

    if (selectors.empty())
    {
        out << "><B>" << (*fact)->name() << "</B></TD>\n"
               "          <TD> </TD>\n"
               "        </TR>\n"
               "      </TABLE>\n"
               "    >];" <<std::endl;
        return;
    }

    out << " ROWSPAN=\"" << selectors.size() << "\"><B>" << (*fact)->name() << "</B></TD>\n"
           "          <TD PORT=\"" << selectors[0] << "\"> " << selectors[0] << " </TD>\n"
           "        </TR>" << std::endl;

    if (selectors.size() > 1)
    {
        for (size_t ind = 1; ind < selectors.size(); ind++)
        {
            out << "        <TR>\n"
                   "          <TD PORT=\"" << selectors[ind] << "\"> " << selectors[ind] << "</TD>\n"
                   "        </TR>" << std::endl;
        }
    }

    out << "      </TABLE>\n"
           "    >];" << std::endl;
#endif
}

void GvExportFactories::exportFactoriesEdges(std::ostream& out)
{
    out << "    /* factories inheritance */" << std::endl;

    for (std::set< SharedPtr<ModuleFactory*> >::iterator it = factorySet.begin(),
            ite = factorySet.end(); it != ite; it++)
    {
        if ((**it)->isRoot())
            continue;

        ModuleFactoryBranch* tmp = reinterpret_cast<ModuleFactoryBranch*>(**it);
        out << "    " << tmp->parent()->name() << ":\"" << tmp->getSelector() << "\"";
        out << " -> " << tmp->name() << ":w;" << std::endl;
    }
}

void GvExportFactories::exportModules(std::ostream& out)
{
    out << "\n    /* modules */" << std::endl;

    for (std::vector< SharedPtr<Module*> >::iterator it = modulesList.begin(),
            ite = modulesList.end(); it != ite; it++)
        exportModuleNode(out, *it);
}

void GvExportFactories::exportModulesEdges(std::ostream& out)
{
    out << "\n    /* factory to module edges*/" << std::endl;

    for (std::vector< SharedPtr<Module*> >::iterator it = modulesList.begin(),
            ite = modulesList.end(); it != ite; it++)
        out << (**it)->parent()->name() << " -> " << (**it)->name() << ":w;" << std::endl;
}
