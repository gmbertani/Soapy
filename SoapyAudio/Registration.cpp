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
#include <SoapySDR/Registry.hpp>
#include <cstdlib> //malloc

static std::vector<SoapySDR::Kwargs> findAudio(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

    RtAudio endac;

    int numDevices = endac.getDeviceCount();

    for (int i = 0; i < numDevices; i++) {
        RtAudio::DeviceInfo info = endac.getDeviceInfo(i);
        SoapySDR::Kwargs soapyInfo;

        soapyInfo["device_id"] = std::to_string(i);
        soapyInfo["label"] = info.name;
        soapyInfo["default_output"] = info.isDefaultOutput ? "True" : "False";
        soapyInfo["default_input"] = info.isDefaultInput ? "True" : "False";

        // std::cout << "\tInput channels: " << info.inputChannels << std::endl;
        // std::cout << "\tOutput channels: " << info.outputChannels << std::endl;
        // std::cout << "\tDuplex channels: " << info.duplexChannels << std::endl;

        // std::cout << "\t" << "Native formats:" << std::endl;
        // RtAudioFormat nFormats = info.nativeFormats;
        // if (nFormats & RTAUDIO_SINT8) {
        //     std::cout << "\t\t8-bit signed integer." << std::endl;
        // }
        // if (nFormats & RTAUDIO_SINT16) {
        //     std::cout << "\t\t16-bit signed integer." << std::endl;
        // }
        // if (nFormats & RTAUDIO_SINT24) {
        //     std::cout << "\t\t24-bit signed integer." << std::endl;
        // }
        // if (nFormats & RTAUDIO_SINT32) {
        //     std::cout << "\t\t32-bit signed integer." << std::endl;
        // }
        // if (nFormats & RTAUDIO_FLOAT32) {
        //     std::cout << "\t\t32-bit float normalized between plus/minus 1.0." << std::endl;
        // }
        // if (nFormats & RTAUDIO_FLOAT64) {
        //     std::cout << "\t\t32-bit float normalized between plus/minus 1.0." << std::endl;
        // }

        // filtering
        if (info.inputChannels == 0) { // filter output devices for now
            continue;
        }
        
        if (args.count("device_id") != 0)
        {
            if (args.at("device_id") != soapyInfo.at("device_id"))
            {
                continue;
            }
            SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by device_id %s", soapyInfo.at("device_id").c_str());
        }
        
        results.push_back(soapyInfo);
    }
    
#ifdef USE_HAMLIB
	rig_set_debug(RIG_DEBUG_ERR);
	rig_load_all_backends();
    SoapyAudio::rigCaps.clear();    
	rig_list_foreach(SoapyAudio::add_hamlib_rig, 0);    
    std::sort(SoapyAudio::rigCaps.begin(), SoapyAudio::rigCaps.end(), rigGreater());
#endif

    return results;
}

#ifdef USE_HAMLIB
int SoapyAudio::add_hamlib_rig(const struct rig_caps *rc, void* f)
{
    rigCaps.push_back(rc);
	return 1;
}
#endif

static SoapySDR::Device *makeAudio(const SoapySDR::Kwargs &args)
{
    return new SoapyAudio(args);
}

static SoapySDR::Registry registerAudio("audio", &findAudio, &makeAudio, SOAPY_SDR_ABI_VERSION);
