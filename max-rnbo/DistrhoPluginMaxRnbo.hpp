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

#pragma once
#define RNBO_USE_FLOAT32
#include "DistrhoPlugin.hpp"
#include "RNBO.h"
START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class DistrhoPluginMaxRnbo : public Plugin
{
public:
    DistrhoPluginMaxRnbo();
    ~DistrhoPluginMaxRnbo() override;

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return DISTRHO_PLUGIN_NAME;
    }

    const char* getDescription() const noexcept override
    {
        return "Generated from Max gen~ and github.com/moddevices/max-gen-plugins";
    }

    int64_t getUniqueId() const noexcept override
    {
        return DISTRHO_PLUGIN_UNIQUE_ID;
    }

    const char* getMaker() const noexcept override
    {
        return DISTRHO_PLUGIN_BRAND;
    }

    const char* getHomePage() const noexcept override
    {
        return "https://github.com/moddevices/max-gen-plugins";
    }

    const char* getLicense() const noexcept override
    {
        return "ISC";
    }

    uint32_t getVersion() const noexcept override
    {
        return DISTRHO_PLUGIN_VERSION;
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Init

    void initAudioPort(bool input, uint32_t index, AudioPort& port) override;
    void initParameter(uint32_t index, Parameter& parameter) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;
    static uint32_t getNumParameters();

    // ----------------------------------------------------------------------------------------------------------------
    // Process

    void run(const float** inputs, float** outputs, uint32_t frames) override;

    // ----------------------------------------------------------------------------------------------------------------

private:
    static RNBO::CoreObject* getRnbo() {
        static RNBO::CoreObject* rnbo = nullptr;
        if (!rnbo) {
            rnbo = new RNBO::CoreObject();
        }
        return rnbo;
    }

    RNBO::CoreObject* rnboObject;
    
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistrhoPluginMaxRnbo)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
