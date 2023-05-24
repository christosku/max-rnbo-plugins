/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define floor(x) ((long)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, 760652352, false);
    getEngine()->flushClockEvents(this, 1812006465, false);
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

SampleIndex currentsampletime() {
    return this->audioProcessSampleCount + this->sampleOffsetIntoNextAudioBuffer;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number safesqrt(number num) {
    return (num > 0.0 ? rnbo_sqrt(num) : 0.0);
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo();
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    SampleValue * out3 = (numOutputs >= 3 && outputs[2] ? outputs[2] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    this->line_01_perform(out1, n);

    this->average_rms_tilde_01_perform(
        in1,
        this->average_rms_tilde_01_windowSize,
        this->average_rms_tilde_01_reset,
        this->signals[0],
        n
    );

    this->slide_tilde_01_perform(
        this->signals[0],
        this->slide_tilde_01_up,
        this->slide_tilde_01_down,
        out2,
        n
    );

    this->dspexpr_01_perform(in1, this->signals[0], n);

    this->rampsmooth_tilde_01_perform(
        this->signals[0],
        this->rampsmooth_tilde_01_up,
        this->rampsmooth_tilde_01_down,
        out3,
        n
    );

    this->peakamp_01_perform(in1, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 1; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->average_rms_tilde_01_dspsetup(forceDSPSetup);
    this->rampsmooth_tilde_01_dspsetup(forceDSPSetup);
    this->peakamp_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->average_rms_tilde_01_av_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 1;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->average_rms_tilde_01_av_buffer = new Float64Buffer(this->average_rms_tilde_01_av_bufferobj);
    }
}

void initialize() {
    this->average_rms_tilde_01_av_bufferobj = initDataRef("average_rms_tilde_01_av_bufferobj", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->average_rms_tilde_01_av_bufferobj->setIndex(0);
    this->average_rms_tilde_01_av_buffer = new Float64Buffer(this->average_rms_tilde_01_av_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "rampup"));
    this->param_02_getPresetValue(getSubState(preset, "rampdown"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "rampup"));
    this->param_02_setPresetValue(getSubState(preset, "rampdown"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 2;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "rampup";
        }
    case 1:
        {
        return "rampdown";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "rampup";
        }
    case 1:
        {
        return "rampdown";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 200;
            info->min = 0;
            info->max = 44100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1000;
            info->min = 0;
            info->max = 44100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 44100 ? 44100 : value));
            ParameterValue normalizedValue = (value - 0) / (44100 - 0);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (44100 - 0);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterChange(
            this->paramInitIndices[i],
            this->getParameterValue(this->paramInitIndices[i]),
            0
        );
    }
}

void processClockEvent(MillisecondTime time, ClockId index, bool hasValue, ParameterValue value) {
    RNBO_UNUSED(hasValue);
    this->updateTime(time);

    switch (index) {
    case 760652352:
        {
        this->line_01_target_bang();
        break;
        }
    case 1812006465:
        {
        this->peakamp_01_output_set(value);
        break;
        }
    }
}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->rampsmooth_tilde_01_up_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->rampsmooth_tilde_01_down_set(v);
}

void line_01_target_bang() {}

void peakamp_01_output_set(number v) {
    this->peakamp_01_output = v;

    {
        list converted = {v};
        this->line_01_segments_set(converted);
    }
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 1;
}

Index getNumOutputChannels() const {
    return 3;
}

void allocateDataRefs() {
    this->average_rms_tilde_01_av_buffer = this->average_rms_tilde_01_av_buffer->allocateIfNeeded();

    if (this->average_rms_tilde_01_av_bufferobj->hasRequestedSize()) {
        if (this->average_rms_tilde_01_av_bufferobj->wantsFill())
            this->zeroDataRef(this->average_rms_tilde_01_av_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }
}

void initializeObjects() {
    this->average_rms_tilde_01_av_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 44100 ? 44100 : (v < 0 ? 0 : v));
    return v;
}

void rampsmooth_tilde_01_up_set(number v) {
    this->rampsmooth_tilde_01_up = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 44100 ? 44100 : (v < 0 ? 0 : v));
    return v;
}

void rampsmooth_tilde_01_down_set(number v) {
    this->rampsmooth_tilde_01_down = v;
}

void line_01_perform(SampleValue * out, Index n) {
    auto __line_01_currentValue = this->line_01_currentValue;
    Index i = 0;

    if ((bool)(this->line_01_activeRamps->length)) {
        while ((bool)(this->line_01_activeRamps->length) && i < n) {
            number destinationValue = this->line_01_activeRamps[0];
            number inc = this->line_01_activeRamps[1];
            number rampTimeInSamples = this->line_01_activeRamps[2] - this->audioProcessSampleCount - i;
            number val = __line_01_currentValue;

            while (rampTimeInSamples > 0 && i < n) {
                out[(Index)i] = val;
                val += inc;
                i++;
                rampTimeInSamples--;
            }

            if (rampTimeInSamples <= 0) {
                val = destinationValue;
                this->line_01_activeRamps->splice(0, 3);

                if ((bool)(!(bool)(this->line_01_activeRamps->length))) this->getEngine()->scheduleClockEventWithValue(
                    this,
                    760652352,
                    this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
                    0
                );;
            }

            __line_01_currentValue = val;
        }
    }

    while (i < n) {
        out[(Index)i] = __line_01_currentValue;
        i++;
    }

    this->line_01_currentValue = __line_01_currentValue;
}

void average_rms_tilde_01_perform(
    const Sample * x,
    number windowSize,
    number reset,
    SampleValue * out1,
    Index n
) {
    RNBO_UNUSED(reset);
    RNBO_UNUSED(windowSize);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = this->safesqrt(this->average_rms_tilde_01_av_next(x[(Index)i] * x[(Index)i], 0, 0));
    }
}

void slide_tilde_01_perform(const Sample * x, number up, number down, SampleValue * out1, Index n) {
    RNBO_UNUSED(down);
    RNBO_UNUSED(up);
    auto __slide_tilde_01_prev = this->slide_tilde_01_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(10)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(100)));
    Index i;

    for (i = 0; i < n; i++) {
        number temp = x[(Index)i] - __slide_tilde_01_prev;
        __slide_tilde_01_prev = __slide_tilde_01_prev + ((x[(Index)i] > __slide_tilde_01_prev ? iup : idown)) * temp;
        out1[(Index)i] = __slide_tilde_01_prev;
    }

    this->slide_tilde_01_prev = __slide_tilde_01_prev;
}

void dspexpr_01_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_abs(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void rampsmooth_tilde_01_perform(const Sample * x, number up, number down, SampleValue * out1, Index n) {
    auto __rampsmooth_tilde_01_increment = this->rampsmooth_tilde_01_increment;
    auto __rampsmooth_tilde_01_index = this->rampsmooth_tilde_01_index;
    auto __rampsmooth_tilde_01_prev = this->rampsmooth_tilde_01_prev;
    Index i;

    for (i = 0; i < n; i++) {
        if (this->rampsmooth_tilde_01_d_next(x[(Index)i]) != 0.) {
            if (x[(Index)i] > __rampsmooth_tilde_01_prev) {
                number _up = up;

                if (_up < 1)
                    _up = 1;

                __rampsmooth_tilde_01_index = _up;
                __rampsmooth_tilde_01_increment = (x[(Index)i] - __rampsmooth_tilde_01_prev) / _up;
            } else if (x[(Index)i] < __rampsmooth_tilde_01_prev) {
                number _down = down;

                if (_down < 1)
                    _down = 1;

                __rampsmooth_tilde_01_index = _down;
                __rampsmooth_tilde_01_increment = (x[(Index)i] - __rampsmooth_tilde_01_prev) / _down;
            }
        }

        if (__rampsmooth_tilde_01_index > 0) {
            __rampsmooth_tilde_01_prev += __rampsmooth_tilde_01_increment;
            __rampsmooth_tilde_01_index -= 1;
        } else {
            __rampsmooth_tilde_01_prev = x[(Index)i];
        }

        out1[(Index)i] = __rampsmooth_tilde_01_prev;
    }

    this->rampsmooth_tilde_01_prev = __rampsmooth_tilde_01_prev;
    this->rampsmooth_tilde_01_index = __rampsmooth_tilde_01_index;
    this->rampsmooth_tilde_01_increment = __rampsmooth_tilde_01_increment;
}

void line_01_segments_set(const list& v) {
    this->line_01_segments = jsCreateListCopy(v);

    if ((bool)(v->length)) {
        auto currentTime = this->currentsampletime();
        number lastRampValue = this->line_01_currentValue;
        number rampEnd = currentTime - this->sampleOffsetIntoNextAudioBuffer;

        for (Index i = 0; i < this->line_01_activeRamps->length; i += 3) {
            rampEnd = this->line_01_activeRamps[(Index)(i + 2)];

            if (rampEnd > currentTime) {
                this->line_01_activeRamps[(Index)(i + 2)] = currentTime;
                number diff = rampEnd - currentTime;
                number valueDiff = diff * this->line_01_activeRamps[(Index)(i + 1)];
                lastRampValue = this->line_01_activeRamps[(Index)i] - valueDiff;
                this->line_01_activeRamps[(Index)i] = lastRampValue;
                this->line_01_activeRamps->length = i + 3;
                rampEnd = currentTime;
            } else {
                lastRampValue = this->line_01_activeRamps[(Index)i];
            }
        }

        if (rampEnd < currentTime) {
            this->line_01_activeRamps->push(lastRampValue);
            this->line_01_activeRamps->push(0);
            this->line_01_activeRamps->push(currentTime);
        }

        number lastRampEnd = currentTime;

        for (Index i = 0; i < v->length; i += 2) {
            number destinationValue = v[(Index)i];
            number inc = 0;
            number rampTimeInSamples;

            if (v->length > i + 1) {
                rampTimeInSamples = this->mstosamps(v[(Index)(i + 1)]);
            } else {
                rampTimeInSamples = this->mstosamps(this->line_01_time);
            }

            if (rampTimeInSamples <= 0)
                rampTimeInSamples = 1;

            inc = (destinationValue - lastRampValue) / rampTimeInSamples;
            lastRampEnd += rampTimeInSamples;
            this->line_01_activeRamps->push(destinationValue);
            this->line_01_activeRamps->push(inc);
            this->line_01_activeRamps->push(lastRampEnd);
            lastRampValue = destinationValue;
        }
    }
}

void peakamp_01_perform(const SampleValue * input_signal, Index n) {
    auto __peakamp_01_index = this->peakamp_01_index;
    auto __peakamp_01_lastMaximum = this->peakamp_01_lastMaximum;
    auto __peakamp_01_maxIndex = this->peakamp_01_maxIndex;
    auto __peakamp_01_interval = this->peakamp_01_interval;

    for (Index i = 0; i < n; i++) {
        if ((bool)(this->peakamp_01_d_next(__peakamp_01_interval))) {
            __peakamp_01_maxIndex = this->mstosamps(__peakamp_01_interval);
        }

        number temp = rnbo_abs(input_signal[(Index)i]);

        if (temp > __peakamp_01_lastMaximum) {
            __peakamp_01_lastMaximum = temp;
        }

        __peakamp_01_index++;

        if (__peakamp_01_maxIndex > 0 && __peakamp_01_index >= __peakamp_01_maxIndex) {
            this->getEngine()->scheduleClockEventWithValue(
                this,
                1812006465,
                this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
                __peakamp_01_lastMaximum
            );;

            __peakamp_01_lastMaximum = 0;
        }

        if (__peakamp_01_maxIndex == 0 || __peakamp_01_index >= __peakamp_01_maxIndex) {
            __peakamp_01_index = 0;
        }
    }

    this->peakamp_01_maxIndex = __peakamp_01_maxIndex;
    this->peakamp_01_lastMaximum = __peakamp_01_lastMaximum;
    this->peakamp_01_index = __peakamp_01_index;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

number peakamp_01_d_next(number x) {
    number temp = (number)(x - this->peakamp_01_d_prev);
    this->peakamp_01_d_prev = x;
    return temp;
}

void peakamp_01_d_dspsetup() {
    this->peakamp_01_d_reset();
}

void peakamp_01_d_reset() {
    this->peakamp_01_d_prev = 0;
}

void peakamp_01_dspsetup(bool force) {
    if ((bool)(this->peakamp_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->peakamp_01_index = 0;
    this->peakamp_01_lastMaximum = 0;
    this->peakamp_01_lastOutput = 0;
    this->peakamp_01_setupDone = true;
    this->peakamp_01_d_dspsetup();
}

number average_rms_tilde_01_av_next(number x, int windowSize, bool reset) {
    if (windowSize > 0)
        this->average_rms_tilde_01_av_setwindowsize(windowSize);

    if (reset != 0) {
        if (this->average_rms_tilde_01_av_resetFlag != 1) {
            this->average_rms_tilde_01_av_wantsReset = 1;
            this->average_rms_tilde_01_av_resetFlag = 1;
        }
    } else {
        this->average_rms_tilde_01_av_resetFlag = 0;
    }

    if (this->average_rms_tilde_01_av_wantsReset == 1) {
        this->average_rms_tilde_01_av_doReset();
    }

    this->average_rms_tilde_01_av_accum += x;
    this->average_rms_tilde_01_av_buffer[(Index)this->average_rms_tilde_01_av_bufferPos] = x;
    number bufferSize = this->average_rms_tilde_01_av_buffer->getSize();

    if (this->average_rms_tilde_01_av_effectiveWindowSize < this->average_rms_tilde_01_av_currentWindowSize) {
        this->average_rms_tilde_01_av_effectiveWindowSize++;
    } else {
        number bufferReadPos = this->average_rms_tilde_01_av_bufferPos - this->average_rms_tilde_01_av_effectiveWindowSize;

        while (bufferReadPos < 0)
            bufferReadPos += bufferSize;

        this->average_rms_tilde_01_av_accum -= this->average_rms_tilde_01_av_buffer[(Index)bufferReadPos];
    }

    this->average_rms_tilde_01_av_bufferPos++;

    if (this->average_rms_tilde_01_av_bufferPos >= bufferSize) {
        this->average_rms_tilde_01_av_bufferPos -= bufferSize;
    }

    return this->average_rms_tilde_01_av_accum / this->average_rms_tilde_01_av_effectiveWindowSize;
}

void average_rms_tilde_01_av_setwindowsize(int wsize) {
    wsize = rnbo_trunc(wsize);

    if (wsize != this->average_rms_tilde_01_av_currentWindowSize && wsize > 0 && wsize <= this->sr) {
        this->average_rms_tilde_01_av_currentWindowSize = wsize;
        this->average_rms_tilde_01_av_wantsReset = 1;
    }
}

void average_rms_tilde_01_av_reset() {
    this->average_rms_tilde_01_av_wantsReset = 1;
}

void average_rms_tilde_01_av_dspsetup() {
    this->average_rms_tilde_01_av_wantsReset = 1;

    if (this->sr > this->average_rms_tilde_01_av_buffer->getSize()) {
        this->average_rms_tilde_01_av_buffer->setSize(this->sr + 1);
        updateDataRef(this, this->average_rms_tilde_01_av_buffer);
    }
}

void average_rms_tilde_01_av_doReset() {
    this->average_rms_tilde_01_av_accum = 0;
    this->average_rms_tilde_01_av_effectiveWindowSize = 0;
    this->average_rms_tilde_01_av_bufferPos = 0;
    this->average_rms_tilde_01_av_wantsReset = 0;
}

void average_rms_tilde_01_av_init() {
    this->average_rms_tilde_01_av_currentWindowSize = this->sr;
    this->average_rms_tilde_01_av_buffer->requestSize(this->sr + 1, 1);
    this->average_rms_tilde_01_av_doReset();
}

void average_rms_tilde_01_dspsetup(bool force) {
    if ((bool)(this->average_rms_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->average_rms_tilde_01_setupDone = true;
    this->average_rms_tilde_01_av_dspsetup();
}

number rampsmooth_tilde_01_d_next(number x) {
    number temp = (number)(x - this->rampsmooth_tilde_01_d_prev);
    this->rampsmooth_tilde_01_d_prev = x;
    return temp;
}

void rampsmooth_tilde_01_d_dspsetup() {
    this->rampsmooth_tilde_01_d_reset();
}

void rampsmooth_tilde_01_d_reset() {
    this->rampsmooth_tilde_01_d_prev = 0;
}

void rampsmooth_tilde_01_dspsetup(bool force) {
    if ((bool)(this->rampsmooth_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->rampsmooth_tilde_01_setupDone = true;
    this->rampsmooth_tilde_01_d_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    RNBO_UNUSED(sampleOffset);
    return (this->vs > 0 ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getTempo() {
    return this->globaltransport_getTempoAtSample(this->sampleOffsetIntoNextAudioBuffer);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    RNBO_UNUSED(sampleOffset);
    return (this->vs > 0 ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState() {
    return this->globaltransport_getStateAtSample(this->sampleOffsetIntoNextAudioBuffer);
}

number globaltransport_getBeatTimeAtMsTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState() == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(this->currenttime(), tempo);
        this->globaltransport_notify = true;
    } else if (this->globaltransport_getTempo() != tempo) {
        auto ct = this->currenttime();
        this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTimeAtMsTime(ct));
        this->globaltransport_beatTimeChanges->push(ct);

        fillSignal(
            this->globaltransport_tempo,
            this->vs,
            tempo,
            (Index)(this->sampleOffsetIntoNextAudioBuffer)
        );

        this->globaltransport_lastTempo = tempo;
        this->globaltransport_tempoNeedsReset = true;
        return true;
    }

    return false;
}

number globaltransport_getBeatTime() {
    return this->globaltransport_getBeatTimeAtMsTime(this->currenttime());
}

bool globaltransport_setState(number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(this->currenttime(), TransportState(state));
        this->globaltransport_notify = true;
    } else if (this->globaltransport_getState() != state) {
        fillSignal(
            this->globaltransport_state,
            this->vs,
            state,
            (Index)(this->sampleOffsetIntoNextAudioBuffer)
        );

        this->globaltransport_lastState = TransportState(state);
        this->globaltransport_stateNeedsReset = true;

        if (state == 0) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime());
            this->globaltransport_beatTimeChanges->push(this->currenttime());
        }

        return true;
    }

    return false;
}

bool globaltransport_setBeatTime(number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(this->currenttime(), beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime());
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(this->currenttime());
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTimeAtMsTime(this->currenttime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignatureAtMsTime(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignature() {
    return this->globaltransport_getTimeSignatureAtMsTime(this->currenttime());
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignatureAtMsTime(this->currenttime() + msOffset);
}

bool globaltransport_setTimeSignature(number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(this->currenttime(), (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature();

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(this->currenttime());
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    line_01_time = 100;
    peakamp_01_interval = 100;
    peakamp_01_output = 0;
    slide_tilde_01_x = 0;
    slide_tilde_01_up = 10;
    slide_tilde_01_down = 100;
    average_rms_tilde_01_x = 0;
    average_rms_tilde_01_windowSize = 0;
    average_rms_tilde_01_reset = 0;
    rampsmooth_tilde_01_x = 0;
    rampsmooth_tilde_01_up = 0;
    rampsmooth_tilde_01_down = 0;
    dspexpr_01_in1 = 0;
    param_01_value = 200;
    param_02_value = 1000;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    line_01_currentValue = 0;
    peakamp_01_index = 0;
    peakamp_01_maxIndex = 0;
    peakamp_01_lastMaximum = 0;
    peakamp_01_lastOutput = 0;
    peakamp_01_d_prev = 0;
    peakamp_01_setupDone = false;
    slide_tilde_01_prev = 0;
    average_rms_tilde_01_av_currentWindowSize = 44100;
    average_rms_tilde_01_av_accum = 0;
    average_rms_tilde_01_av_effectiveWindowSize = 0;
    average_rms_tilde_01_av_bufferPos = 0;
    average_rms_tilde_01_av_wantsReset = false;
    average_rms_tilde_01_av_resetFlag = false;
    average_rms_tilde_01_setupDone = false;
    rampsmooth_tilde_01_prev = 0;
    rampsmooth_tilde_01_index = 0;
    rampsmooth_tilde_01_increment = 0;
    rampsmooth_tilde_01_d_prev = 0;
    rampsmooth_tilde_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    list line_01_segments;
    number line_01_time;
    number peakamp_01_interval;
    number peakamp_01_output;
    number slide_tilde_01_x;
    number slide_tilde_01_up;
    number slide_tilde_01_down;
    number average_rms_tilde_01_x;
    number average_rms_tilde_01_windowSize;
    number average_rms_tilde_01_reset;
    number rampsmooth_tilde_01_x;
    number rampsmooth_tilde_01_up;
    number rampsmooth_tilde_01_down;
    number dspexpr_01_in1;
    number param_01_value;
    number param_02_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[1];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    list line_01_activeRamps;
    number line_01_currentValue;
    int peakamp_01_index;
    int peakamp_01_maxIndex;
    number peakamp_01_lastMaximum;
    number peakamp_01_lastOutput;
    number peakamp_01_d_prev;
    bool peakamp_01_setupDone;
    number slide_tilde_01_prev;
    int average_rms_tilde_01_av_currentWindowSize;
    number average_rms_tilde_01_av_accum;
    int average_rms_tilde_01_av_effectiveWindowSize;
    int average_rms_tilde_01_av_bufferPos;
    bool average_rms_tilde_01_av_wantsReset;
    bool average_rms_tilde_01_av_resetFlag;
    Float64BufferRef average_rms_tilde_01_av_buffer;
    bool average_rms_tilde_01_setupDone;
    number rampsmooth_tilde_01_prev;
    number rampsmooth_tilde_01_index;
    number rampsmooth_tilde_01_increment;
    number rampsmooth_tilde_01_d_prev;
    bool rampsmooth_tilde_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef average_rms_tilde_01_av_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

