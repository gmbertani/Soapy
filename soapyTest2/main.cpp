#include <iostream>
#include <cstdio>	//stdandard output
#include <cstdlib>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Logger.hpp>

#include <string>	// string
#include <vector>	// vector<...>
#include <map>		// map< ... , ... >

#include <typeinfo>
#include <type_traits>

#define CF32_STREAM 1
//#define U8_STREAM 1
//#define CS8_STREAM 1


#ifdef CF32_STREAM
    #define TYPE_STREAM SOAPY_SDR_CF32
    #define TYPE_BUFF   complex<float>
#elif U8_STREAM
    #define TYPE_STREAM SOAPY_SDR_U8
    #define TYPE_BUFF   uint8_t
#elif CS8_STREAM
    #define TYPE_STREAM SOAPY_SDR_CS8
    #define TYPE_BUFF   complex<int8_t>
#endif






using namespace std;

int main()
{
    //SoapySDR::setLogLevel(SOAPY_SDR_DEBUG);
    complex<float> cf32;
    complex<int16_t> cf16;
    complex<int8_t> cf8;
    uint8_t  u8;
    int8_t  i8;
    string str;

    const std::type_info& ticf32 = typeid( cf32  );
    const std::type_info& ticf16 = typeid( cf16 );
    const std::type_info& ticf8 = typeid( cf8 );
    const std::type_info& tiu8 = typeid( u8 );
    const std::type_info& tii8 = typeid( i8 );
    const std::type_info& tistr = typeid( str );

    cout << "type infos: "  << ticf32.name() << endl;
    cout << "type infos: "  << ticf16.name() << endl;
    cout << "type infos: "  << ticf8.name() << endl;
    cout << "type infos: "  << tiu8.name() << endl;
    cout << "type infos: "  << tii8.name() << endl;
    cout << "type infos: "  << tistr.name() << endl;




    // 0. enumerate devices (list all devices' information)
    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    uint8_t i;
    for(i = 0; i < results.size(); ++i)
    {
        cout << "*******************************************" << endl;
        cout << "Found device #" << (char)(i+'0') << " " << endl;
        for( it = results[i].begin(); it != results[i].end(); ++it)
        {
             cout << it->first.c_str() << " = " << it->second.c_str() << endl;
        }
        cout << endl;
    }

    for(;;)
    {
        cout << "*******************************************" << endl;
        do
        {
            char c[10];
            cout << "Enter the device# to test (0 .. " << results.size()-1 << " or <SPACE> to quit) : ";
            cin.getline(c,10);
            if(c[0] == ' ')
            {
                cout << "interrupted by user" << endl;
                return EXIT_SUCCESS;
            }
            i = c[0] - '0';
        }
        while(i >= results.size());

        cout << "************" << endl;
        cout << "Testing device (" << (char)(i+'0')  << ") " << endl;
        string driver = "";

        for( it = results[i].begin(); it != results[i].end(); ++it)
        {
             cout << it->first.c_str() << " = " << it->second.c_str() << endl;
             if( it->first == "driver" )
             {
                driver = it->second;
             }
        }

        if(driver == "")
        {
            cout << "No driver key found for device#" << i;
            return EXIT_FAILURE;
        }

        // 1. create device instance

        //	1.1 set arguments
        //		args can be user defined or from the enumeration result
        SoapySDR::Kwargs args = results[i];

        //	1.2 make device
        SoapySDR::Device *sdr = SoapySDR::Device::make(args);

        if( sdr == NULL )
        {
            cerr << "SoapySDR::Device::make failed" << endl;
            return EXIT_FAILURE;
        }

        // 2. query device info

        SoapySDR::ArgInfoList si = sdr->getSettingInfo();
        int i = 0;
        cout << "----------------------" << endl;
        cout << " SETTINGS " << endl;
        for(i = 0; i < si.size(); i++)
        {
            SoapySDR::ArgInfo info = si.at(i);
            cout << "name: " << info.name << endl;
            cout << "desc: " << info.description << endl;
            cout << "type: " << info.type << endl;
            cout << "key: " << info.key << endl;
            cout << "value: " << info.value << endl;
            cout << "range: " << info.range.minimum() << ", " << info.range.maximum() << ", " << info.range.step() << endl;
            cout << "unit: " << info.units << endl;

            cout << "----------------------" << endl;
        }


        if( sdr->hasDCOffsetMode( SOAPY_SDR_RX, 0 ) )
        {
            bool autoDCoff = sdr->getDCOffsetMode( SOAPY_SDR_RX, 0 );
            cout << "Automatic DC offset mode: " << autoDCoff << endl;
        }

        if(sdr->hasDCOffset( SOAPY_SDR_RX, 0 ) )
        {
            complex<double> dcOff = sdr->getDCOffset( SOAPY_SDR_RX, 0 );
            cout << "Manual DC offset= " << dcOff.real() << "+" << dcOff.imag() << "j " << endl;
        }

        vector< string > str_list;	//string list

        //	2.1 antennas
        str_list = sdr->listAntennas( SOAPY_SDR_RX, 0 );

        cout << "Rx antennas: ";
        for(uint8_t i = 0; i < str_list.size(); ++i)
        {
            cout << str_list[i].c_str() << ", ";
        }

        string antenna = sdr->getAntenna( SOAPY_SDR_RX, 0 );
        cout << " current selected: " << antenna << endl;

        //	2.2 gains
        str_list = sdr->listGains( SOAPY_SDR_RX, 0 );

        cout << "Rx Gains: ";
        for(uint8_t i = 0; i < str_list.size(); ++i)
        {
            cout << str_list[i].c_str() << ", ";
        }
        cout << endl;

        bool autoGain = false;
        if( sdr->hasGainMode( SOAPY_SDR_RX, 0 ) )
        {
            autoGain = sdr->getGainMode( SOAPY_SDR_RX, 0 );
            cout << "Automatic gain (AGC) status: " << autoGain << endl;
        }

        if(autoGain == false)
        {
            SoapySDR::Range gainRange;
            double gain;
            for(uint8_t i = 0; i < str_list.size(); ++i)
            {
                gainRange = sdr->getGainRange( SOAPY_SDR_RX, 0, str_list[i].c_str() );
                gain = sdr->getGain( SOAPY_SDR_RX, 0, str_list[i].c_str() );
                cout << str_list[i].c_str() << " gain range =  ["
                    << gainRange.minimum() << ".." << gainRange.maximum() << "] = " << gain << " dB" << endl;
            }
        }



        //	2.4. frequency ranges
        SoapySDR::RangeList frequencies = sdr->getFrequencyRange( SOAPY_SDR_RX, 0 );

        cout << "Rx freq ranges: ";
        for(uint8_t i = 0; i < frequencies.size(); ++i)
        {
            cout << "[" << frequencies[i].minimum() << "Hz -> " << frequencies[i].maximum() << "Hz], ";
        }
        cout << endl;

        //	2.5. sample rate range
        SoapySDR::RangeList rates = sdr->getSampleRateRange(SOAPY_SDR_RX, 0);
        cout << "Rx sample rate ranges: ";
        for(uint8_t i = 0; i < rates.size(); ++i)
        {
            cout << "[" << rates[i].minimum() << "Hz -> " << rates[i].maximum() << "Hz], ";
        }
        cout << endl;

        // 3. apply settings
        if(driver == "audio")
        {
            cout << "Audio device, setting freq=4800Hz with SR=9600Hz" << endl;
            sdr->setSampleRate( SOAPY_SDR_RX, 0, 9600 );
            sdr->setFrequency( SOAPY_SDR_RX, 0, 4800 );
        }
        else
        {
            cout << "SDR device, setting freq=143.050.000Hz with SR=3MHz" << endl;
            //graves radar frequency, 1MHz SR
            sdr->setSampleRate( SOAPY_SDR_RX, 0, 3e6 );
            sdr->setFrequency( SOAPY_SDR_RX, 0, 143e6 );
            if(autoGain == true)
            {
                //disable AGC
                sdr->setGainMode( SOAPY_SDR_RX, 0, false );
            }
        }

        double streamFS;
        string nativeFormat = sdr->getNativeStreamFormat( SOAPY_SDR_RX, 0, streamFS );
        cout << "Native format: " << nativeFormat << endl;
        cout << "fullscale: " << streamFS << endl;

        // 4. setup a stream (complex floats)
        SoapySDR::Stream *rx_stream = sdr->setupStream( SOAPY_SDR_RX, TYPE_STREAM );
        if( rx_stream == NULL )
        {
            cerr << "Failed" << endl;
            SoapySDR::Device::unmake( sdr );
            return EXIT_FAILURE;
        }

        size_t streamMTU = sdr->getStreamMTU(rx_stream);

        cout << "activating stream, MTU=" << streamMTU << endl;

        int ret = sdr->activateStream( rx_stream, 0, 0, 0 );
        if( ret == 0 )
        {
            int bufTotalSize = 65536;
            int bufSize = 4096;
            // 5. create a re-usable buffer for rx samples
            TYPE_BUFF buff[bufTotalSize];

            // 6. receive some samples
            int totSamples = 20;

            cout << "receiving " << totSamples << " samples..." << endl;

            for( int i = 0; i < totSamples; ++i)
            {

                void *buffs[] = {buff};
                int flags = 0;
                long long time_ns = 0;

                cout << "sample#" << i << "/" << totSamples << endl;
                ret = sdr->readStream( rx_stream, buffs, bufSize, flags, time_ns, 1e5 );

                cout << "ret = " << ret << ", flags = " << flags << ", time_ns = " << time_ns << endl;
            }

            // 7. shutdown the stream
            sdr->deactivateStream( rx_stream, 0, 0 );	//stop streaming
        }
        else
        {
            cout << "activateStream() failed with code " << ret << endl;

        }

        sdr->closeStream( rx_stream );



        // 8. cleanup device handle
        SoapySDR::Device::unmake( sdr );
        cout << "Done" << endl;
    }
    return EXIT_SUCCESS;
}


