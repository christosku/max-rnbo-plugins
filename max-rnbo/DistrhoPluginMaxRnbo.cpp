/*
 * DPF Max Gen
 * Copyright (C) 2015-2023 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPluginMaxRnbo.hpp"

#include "rnbo_source.cpp"
#define RNBO_USE_FLOAT32

START_NAMESPACE_DISTRHO
// --------------------------------------------------------------------------------------------------------------------

DistrhoPluginMaxRnbo::DistrhoPluginMaxRnbo()
    : Plugin(getNumVisibleParameters(), 0, 0)
{
    rnboObject = getRnbo();
    mapParameters();
    rnboObject->prepareToProcess(getSampleRate(), getBufferSize());
}

DistrhoPluginMaxRnbo::~DistrhoPluginMaxRnbo()
{
}

// --------------------------------------------------------------------------------------------------------------------
// Init

void DistrhoPluginMaxRnbo::initAudioPort(const bool input, const uint32_t index, AudioPort& port)
{
    // Check if the port is supposed to be a cv port
    uint32_t cvPorts[ DISTRHO_PLUGIN_NUM_INPUTS + DISTRHO_PLUGIN_NUM_OUTPUTS] = DISTRHO_PLUGIN_CV_PORTS;
    bool isCv = false;
    if (input)
    {
        isCv = cvPorts[index];
    } 
    else {
        isCv = cvPorts[index + DISTRHO_PLUGIN_NUM_INPUTS];
    }

    if (isCv)
    {
        port.hints = kAudioPortIsCV | kCVPortHasBipolarRange | kCVPortHasScaledRange;
    }

    Plugin::initAudioPort(input, index, port);
}

void DistrhoPluginMaxRnbo::initParameter(const uint32_t index, Parameter& parameter)
{
    RNBO::ParameterInfo info;
    RNBO::ConstCharPointer name = rnboObject->getParameterName(getMappedParameterIndex(index));
    RNBO::ConstCharPointer id = DistrhoPluginMaxRnbo::rnboObject->getParameterId(getMappedParameterIndex(index));
    rnboObject->getParameterInfo(getMappedParameterIndex(index), &info);

    

    parameter.hints      = kParameterIsAutomatable;
    parameter.name       = name;
    parameter.symbol     = name;
    parameter.unit       = info.unit;
    parameter.ranges.def = info.initialValue;
    parameter.ranges.min = info.min;
    parameter.ranges.max = info.max;

    parameter.symbol.toBasic();
}

// --------------------------------------------------------------------------------------------------------------------
// Internal data

float DistrhoPluginMaxRnbo::getParameterValue(const uint32_t index) const
{
    RNBO::ParameterValue value = rnboObject->getParameterValue(getMappedParameterIndex(index));
    return value;
}

void DistrhoPluginMaxRnbo::setParameterValue(const uint32_t index, const float value)
{
    rnboObject->setParameterValue(getMappedParameterIndex(index), value);
}

void DistrhoPluginMaxRnbo::mapParameters()
{
    uint32_t paramCount = rnboObject->getNumParameters();
    for (uint32_t i = 0; i < paramCount; i++) {
        RNBO::ParameterInfo info;
        rnboObject->getParameterInfo(i, &info);
        if (info.visible) {
            paramMapping.push_back(i);
        }
    }
}

uint32_t DistrhoPluginMaxRnbo::getMappedParameterIndex(const uint32_t index) const
{
    return paramMapping[index];
}

// --------------------------------------------------------------------------------------------------------------------
// Process
#if DISTRHO_PLUGIN_WANT_MIDI_INPUT || DISTRHO_PLUGIN_WANT_MIDI_OUTPUT
void DistrhoPluginMaxRnbo::run(const float** const inputs, float** const outputs, const uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    // Convert and push incoming Distrho MIDI events to an RNBO::MidiEventList
    RNBO::MidiEventList rnboMidiInEvents;
    for (uint32_t i=0; i<midiEventCount; ++i) {
        MidiEvent midiEvent = midiEvents[i];
        RNBO::MidiEvent rnboMidiEvent = RNBO::MidiEvent(
            0, // have no idea what these are for
            0, // this too :/
            midiEvent.data,
            midiEvent.size
        );

        rnboMidiInEvents.addEvent(rnboMidiEvent);
    }
    
    RNBO::MidiEventList rnboMidiOutEvents;

    rnboObject->process(
        (float**)inputs, 
        rnboObject->getNumInputChannels(), 
        outputs, 
        rnboObject->getNumOutputChannels(), 
        frames,
        &rnboMidiInEvents,
        &rnboMidiOutEvents
    );

    // Convert outgoing RNBO::MidiEventList to Distrho MIDI events and push them out
    for (RNBO::MidiEvent rnboMidiOutEvent : rnboMidiOutEvents) {
        MidiEvent midiEvent;
        midiEvent.frame = 0;
        midiEvent.size = rnboMidiOutEvent.getLength();
        RNBO::ConstByteArray midiData = rnboMidiOutEvent.getData();
        for (uint32_t i=0; i<midiEvent.size; ++i) {
            midiEvent.data[i] = midiData[i];
        }
        writeMidiEvent(midiEvent);
    }
}
#else
void DistrhoPluginMaxRnbo::run(const float** const inputs, float** const outputs, const uint32_t frames)
{
    rnboObject->process(
        (float**)inputs, 
        rnboObject->getNumInputChannels(), 
        outputs, 
        rnboObject->getNumOutputChannels(), 
        frames
    );
}
#endif

// --------------------------------------------------------------------------------------------------------------------

Plugin* createPlugin()
{
    return new DistrhoPluginMaxRnbo();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#include "rnbo/RNBO.cpp"
