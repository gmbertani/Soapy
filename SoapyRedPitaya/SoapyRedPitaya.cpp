/*
 *  SoapyRedPitaya: Soapy SDR plug-in for Red Pitaya SDR transceiver
 *  Copyright (C) 2015  Pavel Demin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <sstream>
#include <stdexcept>

#include <stdint.h>
#include <string.h>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#define INVSOC INVALID_SOCKET
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef SOCKET
#define SOCKET int
#define INVSOC (-1)
#endif
#endif

#include "SoapySDR/Device.hpp"
#include "SoapySDR/Registry.hpp"

#if defined(__APPLE__) || defined(__MACH__)
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL SO_NOSIGPIPE
#endif
#endif

using namespace std;

/***********************************************************************
 * Device interface
 **********************************************************************/

class SoapyRedPitaya : public SoapySDR::Device
{
public:
    SoapyRedPitaya(const SoapySDR::Kwargs &args):
        _addr("192.168.1.100"), _port(1001)
    {
        size_t i;

        #if defined(_WIN32)
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif

        for(i = 0; i < 2; ++i)
        {
            _freq[i] = 6.0e5;
            _rate[i] = 1.0e5;
        }

        for(i = 0; i < 4; ++i)
        {
            _sockets[i] = INVSOC;
        }

        if(args.count("addr")) _addr = args.at("addr");
        if(args.count("port")) stringstream(args.at("port")) >> _port;
    }

    ~SoapyRedPitaya()
    {
        #if defined(_WIN32)
        WSACleanup();
        #endif
    }

    /*******************************************************************
     * Identification API
     ******************************************************************/

    string getDriverKey() const
    {
        return "redpitaya";
    }

    string getHardwareKey() const
    {
        return "redpitaya";
    }

    /*******************************************************************
     * Channels API
     ******************************************************************/

    size_t getNumChannels(const int direction) const
    {
        if(direction == SOAPY_SDR_RX || direction == SOAPY_SDR_TX) return 1;
        return SoapySDR::Device::getNumChannels(direction);
    }

    /*******************************************************************
     * Stream API
     ******************************************************************/

    vector<string> getStreamFormats(const int direction, const size_t channel) const
    {
        vector<string> formats;
        formats.push_back("CF32");
        return formats;
    }

    string getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const
    {
        fullScale = 1.0;
        return "CF32";
    }

    SoapySDR::Stream *setupStream(
        const int direction,
        const string &format,
        const vector<size_t> &channels = vector<size_t>(),
        const SoapySDR::Kwargs &args = SoapySDR::Kwargs())
    {
        if(format != "CF32") throw runtime_error("setupStream invalid format " + format);

        return (SoapySDR::Stream *)(new int(direction));
    }

    void closeStream(SoapySDR::Stream *stream)
    {
        delete (int *)stream;
    }

    int activateStream(
        SoapySDR::Stream *stream,
        const int flags = 0,
        const long long timeNs = 0,
        const size_t numElems = 0)
    {
        int direction = *(int *)stream;

        uint32_t command;
        double frequency = 0.0, rate = 0.0;
        size_t i, start = 0;

        if(direction == SOAPY_SDR_RX && _sockets[0] == INVSOC)
        {
            start = 0;
            frequency = _freq[0];
            rate = _rate[0];
        }
        else if(direction == SOAPY_SDR_TX && _sockets[2] == INVSOC)
        {
            start = 2;
            frequency = _freq[1];
            rate = _rate[1];
        }
        else return 0;

        for(i = start; i < start + 2; ++i)
        {
            _sockets[i] = openConnection();

            command = (uint32_t)i;
            sendCommand(_sockets[i], command);
        }

        setFrequency(direction, 0, "RF", frequency);
        setSampleRate(direction, 0, rate);

        return 0;
    }

    int deactivateStream(
        SoapySDR::Stream *stream,
        const int flags = 0,
        const long long timeNs = 0)
    {
        int direction = *(int *)stream;

        if(direction == SOAPY_SDR_RX)
        {
            #if defined(_WIN32)
            ::closesocket(_sockets[1]);
            ::closesocket(_sockets[0]);
            #else
            ::close(_sockets[1]);
            ::close(_sockets[0]);
            #endif

            _sockets[1] = INVSOC;
            _sockets[0] = INVSOC;
        }
        else if(direction == SOAPY_SDR_TX)
        {
            #if defined(_WIN32)
            ::closesocket(_sockets[3]);
            ::closesocket(_sockets[2]);
            #else
            ::close(_sockets[3]);
            ::close(_sockets[2]);
            #endif

            _sockets[3] = INVSOC;
            _sockets[2] = INVSOC;
        }

        return 0;
    }

    int readStream(
        SoapySDR::Stream *stream,
        void * const *buffs,
        const size_t numElems,
        int &flags,
        long long &timeNs,
        const long timeoutUs = 100000)
    {
        struct timeval timeout;

        #if defined(_WIN32)
        u_long total = 8 * (u_long)numElems;
        u_long size = 0;
        ::ioctlsocket(_sockets[1], FIONREAD, &size);
        #else
        int total = 8 * numElems;
        int size = 0;
        ::ioctl(_sockets[1], FIONREAD, &size);
        #endif

        if(size < total)
        {
            timeout.tv_sec = timeoutUs / 1000000;
            timeout.tv_usec = timeoutUs % 1000000;

            ::select(0, 0, 0, 0, &timeout);

            #if defined(_WIN32)
            ::ioctlsocket(_sockets[1], FIONREAD, &size);
            #else
            ::ioctl(_sockets[1], FIONREAD, &size);
            #endif
        }

        if(size < total) return SOAPY_SDR_TIMEOUT;

        #if defined(_WIN32)
        ::recv(_sockets[1], (char *)buffs[0], total, MSG_WAITALL);
        #else
        ::recv(_sockets[1], buffs[0], total, MSG_WAITALL);
        #endif

        return (int)numElems;
    }

    int writeStream(
        SoapySDR::Stream *stream,
        const void * const *buffs,
        const size_t numElems,
        int &flags,
        const long long timeNs = 0,
        const long timeoutUs = 100000)
    {
        #if defined(_WIN32)
        int total = 8 * (int)numElems;
        int size;
        size = ::send(_sockets[3], (char *)buffs[0], total, 0);
        #else
        ssize_t total = 8 * numElems;
        ssize_t size;
        size = ::send(_sockets[3], buffs[0], total, MSG_NOSIGNAL);
        #endif

        if(size < total) return SOAPY_SDR_TIMEOUT;

        return (int)numElems;
    }

    /*******************************************************************
     * Frequency API
     ******************************************************************/

    void setFrequency(const int direction, const size_t channel, const string &name, const double frequency, const SoapySDR::Kwargs &args = SoapySDR::Kwargs())
    {
        uint32_t command = 0;

        if(name == "BB") return;
        if(name != "RF") throw runtime_error("setFrequency invalid name " + name);

        command = (uint32_t)floor(frequency + 0.5);

        if(direction == SOAPY_SDR_RX)
        {
            if(frequency < _rate[0] / 2.0 || frequency > 6.0e7) return;

            sendCommand(_sockets[0], command);

            _freq[0] = frequency;
        }
        else if(direction == SOAPY_SDR_TX)
        {
            if(frequency < _rate[1] / 2.0 || frequency > 6.0e7) return;

            sendCommand(_sockets[2], command);

            _freq[1] = frequency;
        }
    }

    double getFrequency(const int direction, const size_t channel, const string &name) const
    {
        double frequency = 0.0;

        if(name == "BB") return 0.0;
        if(name != "RF") throw runtime_error("getFrequency invalid name " + name);

        if(direction == SOAPY_SDR_RX)
        {
            frequency = _freq[0];
        }
        else if(direction == SOAPY_SDR_TX)
        {
            frequency = _freq[1];
        }

        return frequency;
    }

    vector<string> listFrequencies(const int direction, const size_t channel) const
    {
        vector<string> names;
        names.push_back("RF");
        return names;
    }

    SoapySDR::RangeList getFrequencyRange(const int direction, const size_t channel, const string &name) const
    {
        double rate = 0.0;

        if (name == "BB") return SoapySDR::RangeList(1, SoapySDR::Range(0.0, 0.0));
        if (name != "RF") throw runtime_error("getFrequencyRange invalid name " + name);

        if(direction == SOAPY_SDR_RX)
        {
            rate = _rate[0];
        }
        else if(direction == SOAPY_SDR_TX)
        {
            rate = _rate[1];
        }

        return SoapySDR::RangeList(1, SoapySDR::Range(rate / 2.0, 60.0e6));
    }

    /*******************************************************************
     * Sample Rate API
     ******************************************************************/

    void setSampleRate(const int direction, const size_t channel, const double rate)
    {
        uint32_t command = 0;

        if(2.0e4 == rate) command = 0;
        else if(5.0e4 == rate) command = 1;
        else if(1.0e5 == rate) command = 2;
        else if(2.5e5 == rate) command = 3;
        else if(5.0e5 == rate) command = 4;
        else if(1.25e6 == rate) command = 5;

        command |= 1<<28;

        if(direction == SOAPY_SDR_RX)
        {
            sendCommand(_sockets[0], command);

            _rate[0] = rate;
        }
        else if(direction == SOAPY_SDR_TX)
        {
            sendCommand(_sockets[2], command);

            _rate[1] = rate;
        }
    }

    double getSampleRate(const int direction, const size_t channel) const
    {
        double rate = 0.0;

        if(direction == SOAPY_SDR_RX)
        {
            rate = _rate[0];
        }
        else if(direction == SOAPY_SDR_TX)
        {
            rate = _rate[1];
        }

        return rate;
    }

    vector<double> listSampleRates(const int direction, const size_t channel) const
    {
        vector<double> rates;
        rates.push_back(2.0e4);
        rates.push_back(5.0e4);
        rates.push_back(1.0e5);
        rates.push_back(2.5e5);
        rates.push_back(5.0e5);
        rates.push_back(1.25e6);
        return rates;
    }

private:
    string _addr;
    unsigned short _port;
    double _freq[2], _rate[2];
    SOCKET _sockets[4];

    SOCKET openConnection()
    {
        stringstream message;
        struct sockaddr_in addr;
        fd_set writefds;
        struct timeval timeout;
        int result;
        SOCKET socket;

        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if(socket == INVSOC)
        {
            throw runtime_error("SoapyRedPitaya could not create TCP socket");
        }

        /* enable non-blocking mode */

        #if defined(_WIN32)
        u_long mode = 1;
        ::ioctlsocket(socket, FIONBIO, &mode);
        #else
        int flags = ::fcntl(socket, F_GETFL, 0);
        ::fcntl(socket, F_SETFL, flags | O_NONBLOCK);
        #endif

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, _addr.c_str(), &addr.sin_addr);
        addr.sin_port = htons(_port);

        ::connect(socket, (struct sockaddr *)&addr, sizeof(addr));

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&writefds);
        FD_SET(socket, &writefds);

        #if defined(_WIN32)
        result = ::select(0, 0, &writefds, 0, &timeout);
        #else
        result = ::select(socket + 1, 0, &writefds, 0, &timeout);
        #endif

        if(result <= 0)
        {
            message << "SoapyRedPitaya could not connect to " << _addr << ":" << _port;
            throw runtime_error(message.str());
        }

        /* disable non-blocking mode */

        #if defined(_WIN32)
        mode = 0;
        ::ioctlsocket(socket, FIONBIO, &mode);
        #else
        flags = ::fcntl(socket, F_GETFL, 0);
        ::fcntl(socket, F_SETFL, flags & ~O_NONBLOCK);
        #endif

        return socket;
    }

    void sendCommand(SOCKET socket, uint32_t command)
    {
        stringstream message;

        if(socket == INVSOC) return;

        #if defined(_WIN32)
        int total = sizeof(command);
        int size;
        size = ::send(socket, (char *)&command, total, 0);
        #else
        ssize_t total = sizeof(command);
        ssize_t size;
        size = ::send(socket, &command, total, MSG_NOSIGNAL);
        #endif

        if(size < total)
        {
            message << "sendCommand failed: " << hex << command;
            throw runtime_error(message.str());
        }
    }
};

/***********************************************************************
 * Find available devices
 **********************************************************************/
SoapySDR::KwargsList findSoapyRedPitaya(const SoapySDR::Kwargs &args)
{
    vector<SoapySDR::Kwargs> results;

    //the user explicitly specified the redpitaya driver
    if (args.count("driver") != 0 and args.at("driver") == "redpitaya")
    {
        //TODO perform a test connection to validate device presence
        results.push_back(args);
        return results;
    }

    //the user only passed an address
    if (args.count("addr") != 0)
    {
        //TODO this could refer to an address for other drivers
        //we need to test connect before its safe to yield
        //results.push_back(args);
        return results;
    }

    //otherwise, perform a discovery for devices on the LAN
    //TODO
    return results;
}

/***********************************************************************
 * Make device instance
 **********************************************************************/
SoapySDR::Device *makeSoapyRedPitaya(const SoapySDR::Kwargs &args)
{
    return new SoapyRedPitaya(args);
}

/***********************************************************************
 * Registration
 **********************************************************************/
static SoapySDR::Registry registerSoapyRedPitaya("redpitaya", &findSoapyRedPitaya, &makeSoapyRedPitaya, SOAPY_SDR_ABI_VERSION);
