#include <iostream>
#include <cstdio>	//stdandard output
#include <cstdlib>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include <string>	// std::string
#include <vector>	// std::vector<...>
#include <map>		// std::map< ... , ... >



int main()
{

    // 0. enumerate devices (list all devices' information)
    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    uint8_t i;
    for(i = 0; i < results.size(); ++i)
    {
        std::cout << "Found device #" << (char)(i+'0') << " ";
        for( it = results[i].begin(); it != results[i].end(); ++it)
        {
             std::cout << it->first.c_str() << " = " << it->second.c_str() << std::endl;
        }
        std::cout << std::endl;
    }

    for(;;)
    {
        do
        {
            char c[10];
            std::cout << "Enter the device# to test (0 .. " << results.size() << " or <SPACE> to quit) : ";
            std::cin.getline(c,10);
            if(c[0] == ' ')
            {
                std::cout << "interrupted by user" << std::endl;
                return EXIT_SUCCESS;
            }
            i = c[0] - '0';
        }
        while(i >= results.size());

        std::cout << "************" << std::endl;
        std::cout << "Testing device#" << i;
        std::string driver = "";

        for( it = results[i].begin(); it != results[i].end(); ++it)
        {
             std::cout << it->first.c_str() << " = " << it->second.c_str() << std::endl;
             if( it->first == "driver" )
             {
                driver = it->second;
             }
        }

        if(driver == "")
        {
            std::cout << "No driver key found for device#" << i;
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
            std::cerr << "SoapySDR::Device::make failed" << std::endl;
            return EXIT_FAILURE;
        }

        // 2. query device info
        std::vector< std::string > str_list;	//string list

        //	2.1 antennas
        str_list = sdr->listAntennas( SOAPY_SDR_RX, 0 );

        std::cout << "Rx antennas: ";
        for(uint8_t i = 0; i < str_list.size(); ++i)
        {
            std::cout << str_list[i].c_str() << ", ";
        }
        std::cout << std::endl;

        //	2.2 gains
        str_list = sdr->listGains( SOAPY_SDR_RX, 0 );

        std::cout << "Rx Gains: ";
        for(uint8_t i = 0; i < str_list.size(); ++i)
        {
            std::cout << str_list[i].c_str() << ", ";
        }
        std::cout << std::endl;

        //	2.3. ranges(frequency ranges)
        SoapySDR::RangeList ranges = sdr->getFrequencyRange( SOAPY_SDR_RX, 0 );

        std::cout << "Rx freq ranges: ";
        for(uint8_t i = 0; i < ranges.size(); ++i)
        {
            std::cout << "[" << ranges[i].minimum() << "Hz -> " << ranges[i].maximum() << "Hz], ";
        }
        std::cout << std::endl;

        // 3. apply settings
        sdr->setSampleRate( SOAPY_SDR_RX, 0, 1e6 );

        sdr->setFrequency( SOAPY_SDR_RX, 0, 143e6 );

        // 4. setup a stream (complex floats)
        SoapySDR::Stream *rx_stream = sdr->setupStream( SOAPY_SDR_RX, SOAPY_SDR_CF32 );
        if( rx_stream == NULL )
        {
            std::cerr << "Failed" << std::endl;
            SoapySDR::Device::unmake( sdr );
            return EXIT_FAILURE;
        }
        sdr->activateStream( rx_stream, 0, 0, 0);

        // 5. create a re-usable buffer for rx samples
        std::complex<float> buff[1024];

        // 6. receive some samples
        for( int i = 0; i < 10; ++i)
        {
            void *buffs[] = {buff};
            int flags;
            long long time_ns;
            int ret = sdr->readStream( rx_stream, buffs, 1024, flags, time_ns, 1e5);
            std::cout << "ret = " << ret;
            std::cout << ", flags = " << flags;
            std::cout << ", time_ns = " << time_ns;
            std::cout << std::endl;
        }

        // 7. shutdown the stream
        sdr->deactivateStream( rx_stream, 0, 0);	//stop streaming
        sdr->closeStream( rx_stream );

        // 8. cleanup device handle
        SoapySDR::Device::unmake( sdr );
        std::cout << "Done" << std::endl;
    }
    return EXIT_SUCCESS;
}


