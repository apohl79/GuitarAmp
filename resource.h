#define PLUG_MFR "OSAR Music"
#define PLUG_NAME "GuitarAmp"

#define PLUG_CLASS_NAME GuitarAmp

#define BUNDLE_MFR "e47"
#define BUNDLE_NAME "GuitarAmp"

#define PLUG_ENTRY GuitarAmp_Entry
#define PLUG_VIEW_ENTRY GuitarAmp_ViewEntry

#define PLUG_ENTRY_STR "GuitarAmp_Entry"
#define PLUG_VIEW_ENTRY_STR "GuitarAmp_ViewEntry"

#define VIEW_CLASS GuitarAmp_View
#define VIEW_CLASS_STR "GuitarAmp_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID '4705'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'xe47'


// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
#define _PIDS_
const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'};  // AudioSuite
#endif

#define PLUG_MFR_PT "e47o\ne47o\nxe47"
#define PLUG_NAME_PT "GuitarAmp\n0E47"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation",
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect"
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0

// GUI default dimensions
#define GUI_WIDTH 800
#define GUI_HEIGHT 303


// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.e47.org"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx|Filter"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
