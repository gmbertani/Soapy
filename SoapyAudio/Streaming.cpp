/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Charles J. Cliffe

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "SoapyAudio.hpp"
#include <SoapySDR/Logger.hpp>
#include <algorithm> //min
#include <climits> //SHRT_MAX
#include <cstring> // memcpy


std::vector<std::string> SoapyAudio::getStreamFormats(const int direction, const size_t channel) const {
    std::vector<std::string> formats;

    formats.push_back("CS8");
    formats.push_back("CS16");
    formats.push_back("CF32");

    return formats;
}

std::string SoapyAudio::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const {
     fullScale = 65536;
     return "CS16";
}

SoapySDR::ArgInfoList SoapyAudio::getStreamArgsInfo(const int direction, const size_t channel) const {
    SoapySDR::ArgInfoList streamArgs;

    SoapySDR::ArgInfo chanArg;
    chanArg.key = "chan";
    chanArg.value = "mono_l";
    chanArg.name = "Channel Setup";
    chanArg.description = "Input channel configuration.";
    chanArg.type = SoapySDR::ArgInfo::STRING;
    
    std::vector<std::string> chanOpts;
    std::vector<std::string> chanOptNames;
    
    chanOpts.push_back("mono_l");
    chanOptNames.push_back("Mono Left");
    chanOpts.push_back("mono_r");
    chanOptNames.push_back("Mono Right");
    chanOpts.push_back("stereo_iq");
    chanOptNames.push_back("Complex L/R = I/Q");
    chanOpts.push_back("stereo_qi");
    chanOptNames.push_back("Complex L/R = Q/I");

    chanArg.options = chanOpts;
    chanArg.optionNames = chanOptNames;

    streamArgs.push_back(chanArg);

    return streamArgs;
}

/*******************************************************************
 * Async thread work
 ******************************************************************/


static int _rx_callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status,
        void *ctx)
{
    //printf("_rx_callback\n");
    SoapyAudio *self = (SoapyAudio *)ctx;
    return self->rx_callback(inputBuffer, nBufferFrames, streamTime, status);
}

int SoapyAudio::rx_callback(void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status)
{
    std::unique_lock<std::mutex> lock(_buf_mutex);

    if (sampleRateChanged.load()) {
        return 1;
    }

    //printf("_rx_callback %d _buf_head=%d, numBuffers=%d\n", len, _buf_head, _buf_tail);

    //overflow condition: the caller is not reading fast enough
    if (_buf_count == numBuffers)
    {
        _overflowEvent = true;
        return 0;
    }

    //copy into the buffer queue
    auto &buff = _buffs[_buf_tail];
    buff.resize(nBufferFrames * elementsPerSample);
    std::memcpy(buff.data(), inputBuffer, nBufferFrames * elementsPerSample * sizeof(float));

    //increment the tail pointer
    _buf_tail = (_buf_tail + 1) % numBuffers;
    _buf_count++;

    //notify readStream()
    _buf_cond.notify_one();
    
    return 0;
}

/*******************************************************************
 * Stream API
 ******************************************************************/

SoapySDR::Stream *SoapyAudio::setupStream(
        const int direction,
        const std::string &format,
        const std::vector<size_t> &channels,
        const SoapySDR::Kwargs &args)
{
    //check the channel configuration
    if (channels.size() > 1 or (channels.size() > 0 and channels.at(0) != 0))
    {
        throw std::runtime_error("setupStream invalid channel selection");
    }

    //check the format
    if (format == "CF32")
    {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CF32.");
        asFormat = AUDIO_FORMAT_FLOAT32;
    }
    else if (format == "CS16")
    {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CS16.");
        asFormat = AUDIO_FORMAT_INT16;
    }
    else if (format == "CS8") {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CS8.");
        asFormat = AUDIO_FORMAT_INT8;
    }
    else
    {
        throw std::runtime_error(
                "setupStream invalid format '" + format
                        + "' -- Only CS8, CS16 and CF32 are supported by SoapyAudio module.");
    }

    if (args.count("chan") != 0)
    {
        std::string chanOpt = args.at("chan");        
        cSetup = chanSetupStrToEnum(chanOpt);
    } else {
        cSetup = FORMAT_MONO_L;
    }

    inputParameters.deviceId = deviceId;
    
    switch (cSetup) {
        case FORMAT_MONO_L:
            inputParameters.nChannels = 1;
            inputParameters.firstChannel = 0;
            bufferLength = DEFAULT_BUFFER_LENGTH;
            elementsPerSample = 1;
            break;
        case FORMAT_MONO_R:
            inputParameters.nChannels = 1;
            inputParameters.firstChannel = 1;
            bufferLength = DEFAULT_BUFFER_LENGTH;
            elementsPerSample = 1;
            break;        
        case FORMAT_STEREO_IQ:
            inputParameters.nChannels = 2;
            inputParameters.firstChannel = 0;
            bufferLength = DEFAULT_BUFFER_LENGTH*2;
            elementsPerSample = 2;
            break;        
        case FORMAT_STEREO_QI:
            inputParameters.nChannels = 2;
            inputParameters.firstChannel = 0;
            bufferLength = DEFAULT_BUFFER_LENGTH*2;
            elementsPerSample = 2;
            break;
    }

    //clear async fifo counts
    _buf_tail = 0;
    _buf_count = 0;
    _buf_head = 0;

    //allocate buffers
    _buffs.resize(numBuffers);
    for (auto &buff : _buffs) buff.reserve(bufferLength);
    for (auto &buff : _buffs) buff.resize(bufferLength);

    return (SoapySDR::Stream *) this;
}

void SoapyAudio::closeStream(SoapySDR::Stream *stream)
{
    _buffs.clear();
}

size_t SoapyAudio::getStreamMTU(SoapySDR::Stream *stream) const
{
    return bufferLength / elementsPerSample;
}

int SoapyAudio::activateStream(
        SoapySDR::Stream *stream,
        const int flags,
        const long long timeNs,
        const size_t numElems)
{
    if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;
    resetBuffer = true;
    bufferedElems = 0;

    try {
#ifndef _MSC_VER
        opts.priority = sched_get_priority_max(SCHED_FIFO);
#endif
        //    opts.flags = RTAUDIO_MINIMIZE_LATENCY;
        opts.flags = RTAUDIO_SCHEDULE_REALTIME;

        sampleRateChanged.store(false);
        dac.openStream(NULL, &inputParameters, RTAUDIO_FLOAT32, sampleRate, &bufferLength, &_rx_callback, (void *) this, &opts);
        dac.startStream();

        streamActive = true;
    } catch (RtAudioError& e) {
        throw std::runtime_error("RtAudio init error '" + e.getMessage());
    }
    
    return 0;
}

int SoapyAudio::deactivateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs)
{
    if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;

    if (dac.isStreamRunning()) {
        dac.stopStream();
    }
    if (dac.isStreamOpen()) {
        dac.closeStream();
    }
    
    streamActive = false;
    
    return 0;
}

int SoapyAudio::readStream(
        SoapySDR::Stream *stream,
        void * const *buffs,
        const size_t numElems,
        int &flags,
        long long &timeNs,
        const long timeoutUs)
{    
    if (!dac.isStreamRunning()) {      
        SoapySDR_logf(SOAPY_SDR_DEBUG, "readStream(): no stream running");
        return 0;
    }
    
    if (sampleRateChanged.load()) {
        if (dac.isStreamRunning()) {
            dac.stopStream();
        }
        if (dac.isStreamOpen()) {
            dac.closeStream();
        }
        dac.openStream(NULL, &inputParameters, RTAUDIO_FLOAT32, sampleRate, &bufferLength, &_rx_callback, (void *) this, &opts);
        dac.startStream();
        sampleRateChanged.store(false);
    }

    //this is the user's buffer for channel 0
    void *buff0 = buffs[0];

    //are elements left in the buffer? if not, do a new read.
    if (bufferedElems == 0 || (sampleOffset && (bufferedElems < abs(sampleOffset))))
    {
        int ret = this->acquireReadBuffer(stream, _currentHandle, (const void **)&_currentBuff, flags, timeNs, timeoutUs);
        if (ret < 0) 
        {
            //SoapySDR_logf(SOAPY_SDR_DEBUG, "readStream(): acquireReadBuffer() returned %d", ret);
            return ret;
        }
        bufferedElems = ret;
    }

    size_t returnedElems = std::min(bufferedElems, numElems);

    if (sampleOffset && (bufferedElems < abs(sampleOffset))) 
    {
        //SoapySDR_logf(SOAPY_SDR_DEBUG, "readStream(): bufferedElems=%d, sampleOffset=%d", bufferedElems, sampleOffset);
        return 0;
    }

    //convert into user's buff0
    if (sampleOffset) {
        if (asFormat == AUDIO_FORMAT_FLOAT32)
        {
            float *ftarget = (float *) buff0;
            std::complex<float> tmp;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    ftarget[i * 2] = _currentBuff[i];
                    ftarget[i * 2 + 1] = 0;
                }            
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        ftarget[i * 2] = sampleOffsetBuffer[i];
                        ftarget[i * 2 + 1] = _currentBuff[i * 2 + 1]; 
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        ftarget[i * 2] = _currentBuff[(i + iStart) * 2];
                        ftarget[i * 2 + 1] = _currentBuff[i * 2 + 1];
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        ftarget[i * 2] = _currentBuff[i * 2];
                        ftarget[i * 2 + 1] = sampleOffsetBuffer[i];
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        ftarget[i * 2] = _currentBuff[i * 2];
                        ftarget[i * 2 + 1] = _currentBuff[(i + iStart) * 2 + 1];
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }
            else if (cSetup == FORMAT_STEREO_QI) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        ftarget[i * 2 + 1] = sampleOffsetBuffer[i];
                        ftarget[i * 2] = _currentBuff[i * 2 + 1]; 
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        ftarget[i * 2 + 1] = _currentBuff[(i + iStart) * 2];
                        ftarget[i * 2] = _currentBuff[i * 2 + 1];
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        ftarget[i * 2 + 1] = _currentBuff[i * 2];
                        ftarget[i * 2] = sampleOffsetBuffer[i];
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        ftarget[i * 2 + 1] = _currentBuff[i * 2];
                        ftarget[i * 2] = _currentBuff[(i + iStart) * 2 + 1];
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }            
        }
        else if (asFormat == AUDIO_FORMAT_INT16)
        {
            int16_t *itarget = (int16_t *) buff0;
            std::complex<int16_t> tmp;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int16_t(_currentBuff[i] * 32767.0);
                    itarget[i * 2 + 1] = 0;
                }
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2] = int16_t(sampleOffsetBuffer[i] * 32767.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[(i + iStart) * 2] * 32767.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[i * 2] * 32767.0);
                        itarget[i * 2 + 1] = int16_t(sampleOffsetBuffer[i] * 32767.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[i * 2] * 32767.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[(i + iStart) * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }
            if (cSetup == FORMAT_STEREO_QI) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2 + 1] = int16_t(sampleOffsetBuffer[i] * 32767.0);
                        itarget[i * 2] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[(i + iStart) * 2] * 32767.0);
                        itarget[i * 2] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2] * 32767.0);
                        itarget[i * 2] = int16_t(sampleOffsetBuffer[i] * 32767.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2] * 32767.0);
                        itarget[i * 2] = int16_t(_currentBuff[(i + iStart) * 2 + 1] * 32767.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }
        }
        else if (asFormat == AUDIO_FORMAT_INT8)
        {
            int8_t *itarget = (int8_t *) buff0;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int8_t(_currentBuff[i] * 127.0);
                    itarget[i * 2 + 1] = 0;
                }
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2] = int16_t(sampleOffsetBuffer[i] * 127.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2 + 1] * 127.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[(i + iStart) * 2] * 127.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2 + 1] * 127.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[i * 2] * 127.0);
                        itarget[i * 2 + 1] = int16_t(sampleOffsetBuffer[i] * 127.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2] = int16_t(_currentBuff[i * 2] * 127.0);
                        itarget[i * 2 + 1] = int16_t(_currentBuff[(i + iStart) * 2 + 1] * 127.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }
            if (cSetup == FORMAT_STEREO_QI) {
                if (sampleOffset > 0) {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2 + 1] = int16_t(sampleOffsetBuffer[i] * 127.0);
                        itarget[i * 2] = int16_t(_currentBuff[i * 2 + 1] * 127.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[(i + iStart) * 2] * 127.0);
                        itarget[i * 2] = int16_t(_currentBuff[i * 2 + 1] * 127.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2];
                    }                  
                } else {
                    size_t iStart = abs(sampleOffset);
                    for (size_t i = 0; i < iStart; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2] * 127.0);
                        itarget[i * 2] = int16_t(sampleOffsetBuffer[i] * 127.0);
                    }            
                    for (size_t i = iStart; i < returnedElems; i++) {
                        itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2] * 127.0);
                        itarget[i * 2] = int16_t(_currentBuff[(i + iStart) * 2 + 1] * 127.0);
                    }            
                    for (size_t i = 0; i < iStart; i++) {
                        sampleOffsetBuffer[i] = _currentBuff[(returnedElems-iStart+i) * 2 + 1];
                    }
                }
            }            
        } 
    } else {
        if (asFormat == AUDIO_FORMAT_FLOAT32)
        {
            float *ftarget = (float *) buff0;
            std::complex<float> tmp;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    ftarget[i * 2] = _currentBuff[i];
                    ftarget[i * 2 + 1] = 0;
                }            
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    ftarget[i * 2] = _currentBuff[i * 2];
                    ftarget[i * 2 + 1] = _currentBuff[i * 2 + 1];
                }            
            }
            else if (cSetup == FORMAT_STEREO_QI) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    ftarget[i * 2] = _currentBuff[i * 2 + 1];
                    ftarget[i * 2 + 1] = _currentBuff[i * 2];
                }            
            }            
        }
        else if (asFormat == AUDIO_FORMAT_INT16)
        {
            int16_t *itarget = (int16_t *) buff0;
            std::complex<int16_t> tmp;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int16_t(_currentBuff[i] * 32767.0);
                    itarget[i * 2 + 1] = 0;
                }
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int16_t(_currentBuff[i * 2] * 32767.0);
                    itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                }
            }
            else if (cSetup == FORMAT_STEREO_QI) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int16_t(_currentBuff[i * 2 + 1] * 32767.0);
                    itarget[i * 2 + 1] = int16_t(_currentBuff[i * 2] * 32767.0);
                }            
            }            
        }
        else if (asFormat == AUDIO_FORMAT_INT8)
        {
            int8_t *itarget = (int8_t *) buff0;
            if (cSetup == FORMAT_MONO_L || cSetup == FORMAT_MONO_R) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int8_t(_currentBuff[i] * 127.0);
                    itarget[i * 2 + 1] = 0;
                }
            }
            else if (cSetup == FORMAT_STEREO_IQ) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int8_t(_currentBuff[i * 2] * 127.0);
                    itarget[i * 2 + 1] = int8_t(_currentBuff[i * 2 + 1] * 127.0);
                }
            }
            else if (cSetup == FORMAT_STEREO_QI) {
                for (size_t i = 0; i < returnedElems; i++)
                {
                    itarget[i * 2] = int8_t(_currentBuff[i * 2 + 1] * 127.0);
                    itarget[i * 2 + 1] = int8_t(_currentBuff[i * 2] * 127.0);
                }            
            }            
        }
    }
    
    
    
    //bump variables for next call into readStream
    bufferedElems -= returnedElems;
    _currentBuff += returnedElems * elementsPerSample;

    //SoapySDR_logf(SOAPY_SDR_DEBUG, "readStream(): bufferedElems=%d, _currentBuff=%d", bufferedElems, _currentBuff);


    //return number of elements written to buff0
    if (bufferedElems != 0) flags |= SOAPY_SDR_MORE_FRAGMENTS;
    else this->releaseReadBuffer(stream, _currentHandle);
    return returnedElems;
}

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/

size_t SoapyAudio::getNumDirectAccessBuffers(SoapySDR::Stream *stream)
{
    return _buffs.size();
}

int SoapyAudio::getDirectAccessBufferAddrs(SoapySDR::Stream *stream, const size_t handle, void **buffs)
{
    buffs[0] = (void *)_buffs[handle].data();
    return 0;
}

int SoapyAudio::acquireReadBuffer(
    SoapySDR::Stream *stream,
    size_t &handle,
    const void **buffs,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    std::unique_lock <std::mutex> lock(_buf_mutex);

    //reset is issued by various settings
    //to drain old data out of the queue
    if (resetBuffer)
    {
        //drain all buffers from the fifo
        _buf_head = (_buf_head + _buf_count) % numBuffers;
        _buf_count = 0;
        resetBuffer = false;
        _overflowEvent = false;
    }

    //handle overflow from the rx callback thread
    if (_overflowEvent)
    {
        //drain the old buffers from the fifo
        _buf_head = (_buf_head + _buf_count) % numBuffers;
        _buf_count = 0;
        _overflowEvent = false;
        SoapySDR::log(SOAPY_SDR_SSI, "O");
        return SOAPY_SDR_OVERFLOW;
    }

    //wait for a buffer to become available
    while (_buf_count == 0)
    {
        _buf_cond.wait_for(lock, std::chrono::microseconds(timeoutUs));
        if (_buf_count == 0) return SOAPY_SDR_TIMEOUT;
    }

    //extract handle and buffer
    handle = _buf_head;
    _buf_head = (_buf_head + 1) % numBuffers;
    buffs[0] = (void *)_buffs[handle].data();
    flags = 0;

    //return number available
    return _buffs[handle].size() / elementsPerSample;
}

void SoapyAudio::releaseReadBuffer(
    SoapySDR::Stream *stream,
    const size_t handle)
{
    //TODO this wont handle out of order releases
    std::unique_lock <std::mutex> lock(_buf_mutex);
    _buf_count--;
}
