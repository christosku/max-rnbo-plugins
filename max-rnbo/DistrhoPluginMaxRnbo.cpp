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
    : Plugin(gen::num_params(), 0, 0), // 0 programs, 0 states
      fGenState((CommonState*)gen::create(getSampleRate(), getBufferSize()))
{
    gen::reset(fGenState);
}

DistrhoPluginMaxRnbo::~DistrhoPluginMaxRnbo()
{
    gen::destroy(fGenState);
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
    ParamInfo& info(fGenState->params[index]);

    parameter.hints      = kParameterIsAutomatable;
    parameter.name       = info.name;
    parameter.symbol     = info.name;
    parameter.unit       = info.units;
    parameter.ranges.def = info.defaultvalue;
    parameter.ranges.min = info.outputmin;
    parameter.ranges.max = info.outputmax;

    parameter.symbol.toBasic();
}

// --------------------------------------------------------------------------------------------------------------------
// Internal data

float DistrhoPluginMaxRnbo::getParameterValue(const uint32_t index) const
{
    t_param value = 0.f;
    gen::getparameter(fGenState, index, &value);
    return value;
}

void DistrhoPluginMaxRnbo::setParameterValue(const uint32_t index, const float value)
{
    gen::setparameter(fGenState, index, value, nullptr);
}

// --------------------------------------------------------------------------------------------------------------------
// Process

void DistrhoPluginMaxRnbo::run(const float** const inputs, float** const outputs, const uint32_t frames)
{
    gen::perform(fGenState, (float**)inputs, gen::gen_kernel_numins, outputs, gen::gen_kernel_numouts, frames);
}

// --------------------------------------------------------------------------------------------------------------------

Plugin* createPlugin()
{
    return new DistrhoPluginMaxRnbo();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#include "rnbo/RNBO.cpp"
