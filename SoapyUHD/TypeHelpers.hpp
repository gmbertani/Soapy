// Copyright (c) 2014-2017 Josh Blum
// SPDX-License-Identifier: GPL-3.0

#pragma once
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.hpp> //feature constants
#include <uhd/types/device_addr.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/types/sensors.hpp>
#include <vector>

#define SOAPY_UHD_NO_DEEPER "soapy_uhd_no_deeper"

/***********************************************************************
 * Helpful type conversions
 **********************************************************************/
static inline SoapySDR::Kwargs dictToKwargs(const uhd::device_addr_t &addr)
{
    SoapySDR::Kwargs kwargs;
    const std::vector<std::string> keys = addr.keys();
    for (size_t i = 0; i < keys.size(); i++)
    {
        kwargs[keys[i]] = addr[keys[i]];
    }
    return kwargs;
}

static inline uhd::device_addr_t kwargsToDict(const SoapySDR::Kwargs &kwargs)
{
    uhd::device_addr_t addr;
    for (SoapySDR::Kwargs::const_iterator it = kwargs.begin(); it != kwargs.end(); ++it)
    {
        addr[it->first] = it->second;
    }
    return addr;
}

static inline SoapySDR::RangeList metaRangeToRangeList(const uhd::meta_range_t &metaRange)
{
    SoapySDR::RangeList out;
    for (size_t i = 0; i < metaRange.size(); i++)
    {
        #ifdef SOAPY_SDR_API_HAS_RANGE_TYPE_STEP
        out.push_back(SoapySDR::Range(metaRange[i].start(), metaRange[i].stop(), metaRange[i].step()));
        #else
        out.push_back(SoapySDR::Range(metaRange[i].start(), metaRange[i].stop()));
        #endif
    }
    return out;
}

static inline uhd::meta_range_t rangeListToMetaRange(const SoapySDR::RangeList &ranges)
{
    uhd::meta_range_t out;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        #ifdef SOAPY_SDR_API_HAS_RANGE_TYPE_STEP
        out.push_back(uhd::range_t(ranges[i].minimum(), ranges[i].maximum(), ranges[i].step()));
        #else
        out.push_back(uhd::range_t(ranges[i].minimum(), ranges[i].maximum()));
        #endif
    }
    if (out.empty()) out.push_back(uhd::range_t(0.0));
    return out;
}

static inline SoapySDR::Range metaRangeToRange(const uhd::meta_range_t &metaRange)
{
    #ifdef SOAPY_SDR_API_HAS_RANGE_TYPE_STEP
    return SoapySDR::Range(metaRange.start(), metaRange.stop(), metaRange.step());
    #else
    return SoapySDR::Range(metaRange.start(), metaRange.stop());
    #endif
}

static inline uhd::meta_range_t numberListToMetaRange(const std::vector<double> &nums)
{
    uhd::meta_range_t out;
    for (size_t i = 0; i < nums.size(); i++)
    {
        out.push_back(uhd::range_t(nums[i]));
    }
    if (out.empty()) out.push_back(uhd::range_t(0.0));
    return out;
}

static inline std::vector<double> metaRangeToNumericList(const uhd::meta_range_t &metaRange)
{
    std::vector<double> out;

    //in this case, the bounds are in element 0
    if (metaRange.size() == 1)
    {
        out.push_back(metaRange[0].start());
        out.push_back(metaRange[0].stop());
        return out;
    }

    for (size_t i = 0; i < metaRange.size(); i++)
    {
        //in these cases start == stop
        out.push_back(metaRange[i].start());
    }
    return out;
}

static inline uhd::meta_range_t rangeToMetaRange(const SoapySDR::Range &range, double step = 0.0)
{
    //when range step is supported, use it only if initialized to non-zero
    #ifdef SOAPY_SDR_API_HAS_RANGE_TYPE_STEP
    if (range.step() != 0.0) step = range.step();
    #endif

    return uhd::meta_range_t(range.minimum(), range.maximum(), step);
}

static inline SoapySDR::ArgInfo sensorToArgInfo(const uhd::sensor_value_t &sensor, const std::string &key)
{
    SoapySDR::ArgInfo argInfo;
    argInfo.key = key;
    argInfo.value = sensor.value;
    argInfo.name = sensor.name;
    argInfo.units = sensor.unit;
    switch (sensor.type)
    {
    case uhd::sensor_value_t::BOOLEAN: argInfo.type = SoapySDR::ArgInfo::BOOL; break;
    case uhd::sensor_value_t::INTEGER: argInfo.type = SoapySDR::ArgInfo::INT; break;
    case uhd::sensor_value_t::REALNUM: argInfo.type = SoapySDR::ArgInfo::FLOAT; break;
    case uhd::sensor_value_t::STRING: argInfo.type = SoapySDR::ArgInfo::STRING; break;
    }
    return argInfo;
}

static inline uhd::sensor_value_t argInfoToSensor(const SoapySDR::ArgInfo &argInfo, const std::string &value)
{
    switch (argInfo.type)
    {
    case SoapySDR::ArgInfo::BOOL: return uhd::sensor_value_t(argInfo.name, value == "true", argInfo.units, argInfo.units);
    case SoapySDR::ArgInfo::INT: return uhd::sensor_value_t(argInfo.name, atoi(value.c_str()), argInfo.units);
    case SoapySDR::ArgInfo::FLOAT: return uhd::sensor_value_t(argInfo.name, atof(value.c_str()), argInfo.units);
    case SoapySDR::ArgInfo::STRING: return uhd::sensor_value_t(argInfo.name, value, argInfo.units);
    }
    return uhd::sensor_value_t(argInfo.name, value, argInfo.units);
}
