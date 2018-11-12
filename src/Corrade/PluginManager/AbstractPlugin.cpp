/*
    This file is part of Corrade.

    Copyright © 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,
                2017, 2018 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "AbstractPlugin.h"

#include "Corrade/PluginManager/AbstractManager.h"
#include "Corrade/PluginManager/PluginMetadata.h"
#include "Corrade/Utility/ConfigurationGroup.h"

namespace Corrade { namespace PluginManager {

std::string AbstractPlugin::pluginInterface() { return {}; }

#ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
std::vector<std::string> AbstractPlugin::pluginSearchPaths() { return {}; }
#endif

void AbstractPlugin::initialize() {}

void AbstractPlugin::finalize() {}

AbstractPlugin::AbstractPlugin(): _manager{nullptr}, _metadata{nullptr} {
    _configuration.reset(new Utility::ConfigurationGroup);
}

AbstractPlugin::AbstractPlugin(AbstractManager& manager, const std::string& plugin): _manager{&manager}, _plugin{plugin} {
    manager.registerInstance(plugin, *this, _metadata);
    _configuration.reset(new Utility::ConfigurationGroup(_metadata->configuration()));
}

AbstractPlugin::~AbstractPlugin() {
    /* Unregister the instance only if the plugin was instantiated through
       plugin manager. Note that instantiating using
       AbstractManagingPlugin::AbstractManagingPlugin(AbstractManager&) is
       *not* instantiating through the manager, in that case the _metadata
       field would be nullptr */
    if(_manager && _metadata)
        _manager->unregisterInstance(_plugin, *this);
}

bool AbstractPlugin::canBeDeleted() { return false; }

}}
