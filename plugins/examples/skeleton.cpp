// This is an example plugin that documents and implements all the plugin
// callbacks and features. You can include it in the regular build by setting
// the BUILD_SKELETON option in CMake to ON. Play with loading and unloading
// the plugin in various game states (e.g. with and without a world loaded),
// and see the debug messages get printed to the console.
//
// See the other example plugins in this directory for plugins that are
// configured for specific use cases (but don't come with as many comments as
// this one does).

#include <string>
#include <vector>

#include "df/world.h"

#include "Core.h"
#include "Debug.h"
#include "PluginManager.h"

#include "modules/Persistence.h"
#include "modules/World.h"

using std::string;
using std::vector;

using namespace DFHack;

// Expose the plugin name to the DFHack core, as well as metadata like the
// DFHack version that this plugin was compiled with. This macro provides a
// variable for the plugin name as const char * plugin_name.
// The name provided must correspond to the filename --
// skeleton.plug.so, skeleton.plug.dylib, or skeleton.plug.dll in this case
DFHACK_PLUGIN("skeleton");

// The identifier declared with this macro (i.e. is_enabled) is used to track
// whether the plugin is in an "enabled" state. If you don't need enablement
// for your plugin, you don't need this line. This variable will also be read
// by the `plug` builtin command; when true the plugin will be shown as enabled.
DFHACK_PLUGIN_IS_ENABLED(is_enabled);

// Any globals a plugin requires (e.g. world) should be listed here.
// For example, this line expands to "using df::global::world" and prevents the
// plugin from being loaded if df::global::world is null (i.e. missing from
// symbols.xml).
REQUIRE_GLOBAL(world);

// logging levels can be dynamically controlled with the `debugfilter` command.
// Actual plugins will likely want to set the default level to LINFO or LWARNING
// instead of the LDEBUG used here.
namespace DFHack {
    // for configuration-related logging
    DBG_DECLARE(skeleton, status, DebugCategory::LDEBUG);
    // for plugin_onupdate logging
    DBG_DECLARE(skeleton, onupdate, DebugCategory::LDEBUG);
    // for command-related logging
    DBG_DECLARE(skeleton, command, DebugCategory::LDEBUG);
}

static command_result command_callback1(color_ostream &out, vector<string> &parameters);

// run when the plugin is loaded
DFhackCExport command_result plugin_init(color_ostream &out, std::vector<PluginCommand> &commands) {
    DEBUG(status,out).print("initializing %s\n", plugin_name);

    // For in-tree plugins, don't use the "usage" parameter of PluginCommand.
    // Instead, add an .rst file with the same name as the plugin to the
    // docs/plugins/ directory.
    commands.push_back(PluginCommand(
        "skeleton",
        "Short (~54 character) description of command.", // to use one line in the ``[DFHack]# ls`` output
        command_callback1));
    return CR_OK;
}

// run when the plugin is unloaded
DFhackCExport command_result plugin_shutdown(color_ostream &out) {
    DEBUG(status,out).print("shutting down %s\n", plugin_name);

    // You *MUST* kill all threads you created before this returns.
    // If everything fails, just return CR_FAILURE. Your plugin will be
    // in a zombie state, but things won't crash.
    return CR_OK;

}

// run when the `enable` or `disable` command is run with this plugin name as
// an argument
DFhackCExport command_result plugin_enable(color_ostream &out, bool enable) {
    DEBUG(status,out).print("%s from the API\n", enable ? "enabled" : "disabled");

    // you have to maintain the state of the is_enabled variable yourself. it
    // doesn't happen automatically.
    is_enabled = enable;
    return CR_OK;
}

// Called to notify the plugin about important state changes.
// Invoked with DF suspended, and always before the matching plugin_onupdate.
// More event codes may be added in the future.
DFhackCExport command_result plugin_onstatechange(color_ostream &out, state_change_event event) {
    switch (event) {
        case SC_UNKNOWN:
            DEBUG(status,out).print("game state changed: SC_UNKNOWN\n");
            break;
        case SC_WORLD_LOADED:
            DEBUG(status,out).print("game state changed: SC_WORLD_LOADED\n");
            break;
        case SC_WORLD_UNLOADED:
            DEBUG(status,out).print("game state changed: SC_WORLD_UNLOADED\n");
            break;
        case SC_MAP_LOADED:
            DEBUG(status,out).print("game state changed: SC_MAP_LOADED\n");
            break;
        case SC_MAP_UNLOADED:
            DEBUG(status,out).print("game state changed: SC_MAP_UNLOADED\n");
            break;
        case SC_VIEWSCREEN_CHANGED:
            DEBUG(status,out).print("game state changed: SC_VIEWSCREEN_CHANGED\n");
            break;
        case SC_CORE_INITIALIZED:
            DEBUG(status,out).print("game state changed: SC_CORE_INITIALIZED\n");
            break;
        case SC_BEGIN_UNLOAD:
            DEBUG(status,out).print("game state changed: SC_BEGIN_UNLOAD\n");
            break;
        case SC_PAUSED:
            DEBUG(status,out).print("game state changed: SC_PAUSED\n");
            break;
        case SC_UNPAUSED:
            DEBUG(status,out).print("game state changed: SC_UNPAUSED\n");
            break;
    }

    return CR_OK;
}

// Whatever you put here will be done in each game frame refresh. Don't abuse it.
// Note that if the plugin implements the enabled API, this function is only called
// if the plugin is enabled.
DFhackCExport command_result plugin_onupdate (color_ostream &out) {
    DEBUG(onupdate,out).print(
        "onupdate called (run 'debugfilter set info skeleton onupdate' to stop"
        " seeing these messages)\n");

    return CR_OK;
}

// If you need to save or load world-specific data, define these functions.
// plugin_save_data is called when the game might be about to save the world,
// and plugin_load_data is called whenever a new world is loaded. If the plugin
// is loaded or unloaded while a world is active, plugin_save_data or
// plugin_load_data will be called immediately.
DFhackCExport command_result plugin_save_data (color_ostream &out) {
    DEBUG(status,out).print("save or unload is imminent; time to persist state\n");

    // Call functions in the Persistence module here. If your PersistantDataItem
    // objects are already up to date, then they will get persisted with the
    // save automatically and there is nothing extra you need to do here.
    return CR_OK;
}

DFhackCExport command_result plugin_load_data (color_ostream &out) {
    DEBUG(status,out).print("world is loading; time to load persisted state\n");

    // Call functions in the Persistence module here. See
    // persistent_per_save_example.cpp for an example.
    return CR_OK;
}

// This is the callback we registered in plugin_init. Note that while plugin
// callbacks are called with the core suspended, command callbacks are called
// from a different thread and need to explicity suspend the core if they
// interact with Lua or DF game state (most commands do at least one of these).
static command_result command_callback1(color_ostream &out, vector<string> &parameters) {
    DEBUG(command,out).print("%s command called with %zu parameters\n",
        plugin_name, parameters.size());

    // I'll say it again: always suspend the core in command callbacks unless
    // all your data is local.
    CoreSuspender suspend;

    // Return CR_WRONG_USAGE to print out your help text. The help text is
    // sourced from the associated rst file in docs/plugins/. The same help will
    // also be returned by 'help your-command'.

    // simple commandline parsing can be done in C++, but there are lua libraries
    // that can easily handle more complex commandlines. see the blueprint plugin
    // for an example.

    // TODO: do something according to the flags set in the options struct

    return CR_OK;
}
