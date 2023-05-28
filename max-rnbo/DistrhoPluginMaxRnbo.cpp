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

namespace rnbo = RNBO;

START_NAMESPACE_DISTRHO
// --------------------------------------------------------------------------------------------------------------------

DistrhoPluginMaxRnbo::DistrhoPluginMaxRnbo()
    : Plugin(getRnbo()->getNumParameters(), 0, 0)
{
    rnboObject = getRnbo();
    rnboObject->prepareToProcess(getSampleRate(), getBufferSize());
}

DistrhoPluginMaxRnbo::~DistrhoPluginMaxRnbo()
{
}

// --------------------------------------------------------------------------------------------------------------------
// Init

void DistrhoPluginMaxRnbo::initAudioPort(const bool input, const uint32_t index, AudioPort& port)
{
    if (DISTRHO_PLUGIN_NUM_INPUTS == DISTRHO_PLUGIN_NUM_OUTPUTS)
    {
        switch (DISTRHO_PLUGIN_NUM_INPUTS)
        {
        case 1:
            port.groupId = kPortGroupMono;
            break;
        case 2:
            port.groupId = kPortGroupStereo;
            break;
        }
    }

    Plugin::initAudioPort(input, index, port);
}

void DistrhoPluginMaxRnbo::initParameter(const uint32_t index, Parameter& parameter)
{
    rnbo::ParameterInfo info;
    rnbo::ConstCharPointer name = rnboObject->getParameterName(index);
    rnbo::ConstCharPointer id = DistrhoPluginMaxRnbo::rnboObject->getParameterId(index);
    rnboObject->getParameterInfo(index, &info);

    

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
    rnbo::ParameterValue value = rnboObject->getParameterValue(index);
    return value;
}

void DistrhoPluginMaxRnbo::setParameterValue(const uint32_t index, const float value)
{
    rnboObject->setParameterValue(index, value);
}

// --------------------------------------------------------------------------------------------------------------------
// Process

void DistrhoPluginMaxRnbo::run(const float** const inputs, float** const outputs, const uint32_t frames)
{
    rnbo::SampleValue** rnboInputs = nullptr;

    rnboObject->process(rnboInputs, 0, outputs, 2, frames);
}

// --------------------------------------------------------------------------------------------------------------------

Plugin* createPlugin()
{
    return new DistrhoPluginMaxRnbo();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#include "rnbo/RNBO.cpp"
