#pragma once

#define DISTRHO_PLUGIN_BRAND   "RNBO"
#define DISTRHO_PLUGIN_NAME    "RNBO"
#define DISTRHO_PLUGIN_URI     "http://example.com/try/rnbo"

#define DISTRHO_PLUGIN_HAS_UI        0
#define DISTRHO_PLUGIN_IS_RT_SAFE    1

#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 1

#define DISTRHO_PLUGIN_LV2_CATEGORY    "lv2:UtilityPlugin"

#define DISTRHO_PLUGIN_UNIQUE_ID d_cconst('z', 'w', 'm', 'z')
#define DISTRHO_PLUGIN_VERSION d_version(0, 0, 0)