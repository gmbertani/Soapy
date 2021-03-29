/*
Basic C API example

Compile with: gcc -std=c99 example.c -lSoapySDR && ./a.out
*/

#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

int main(void)
{
    size_t length;

    //enumerate devices

    SoapySDRKwargs *results = SoapySDRDevice_enumerate(NULL, &length);
    if(length == 0)
    {
        printf("\nno devices found\n");
        return EXIT_FAILURE;
    }

    size_t i;
    for (i = 0; i < length; i++)
    {
        printf("Found device #%u: ", (uint8_t)i);
        for (size_t j = 0; j < results[i].size; j++)
        {
            printf("%s=%s, ", results[i].keys[j], results[i].vals[j] );
        }
        printf("\n");
    }

    for(;;)
    {
        do
        {
            printf("Enter the device# to test (0 .. %u) or <SPACE> to quit: ", (uint8_t)length);
            i = (size_t)getchar();
            if(i == ' ')
            {
                printf("interrupted by user\n");
                return EXIT_SUCCESS;
            }
            i -= '0';
        }
        while(i >= length);
        printf("************\n");
        printf("Testing device#%u", (uint8_t)i);
        char* driver = NULL;
        for (size_t j = 0; j < results[i].size; j++)
        {
            printf("%s=%s, ", results[i].keys[j], results[i].vals[j]);
            if( strncmp(results[i].keys[j], "driver", 6 ) == 0 )
            {
                driver = results[i].vals[j];
            }
        }


        //create device instance
        //args can be user defined or from the enumeration result
        if(driver == NULL)
        {
            printf("No driver key found for device# %u\n", (uint8_t)i);
            return EXIT_FAILURE;
        }

        SoapySDRKwargs args = {};
        //SoapySDRKwargs_set(&args, "driver", "rtlsdr");
        SoapySDRKwargs_set(&args, "driver", driver);
        SoapySDRDevice *sdr = SoapySDRDevice_make(&args);
        if (sdr == NULL)
        {
            printf("SoapySDRDevice_make fail: %s\n", SoapySDRDevice_lastError());
            return EXIT_FAILURE;
        }

        SoapySDRKwargsList_clear(results, length);
        SoapySDRKwargs_clear(&args);

        //query device info
        char** names = SoapySDRDevice_listAntennas(sdr, SOAPY_SDR_RX, 0, &length);
        printf("Rx antennas: ");
        for (size_t i = 0; i < length; i++) printf("%s, ", names[i]);
        printf("\n");
        SoapySDRStrings_clear(&names, length);

        names = SoapySDRDevice_listGains(sdr, SOAPY_SDR_RX, 0, &length);
        printf("Rx gains: ");
        for (size_t i = 0; i < length; i++) printf("%s, ", names[i]);
        printf("\n");
        SoapySDRStrings_clear(&names, length);

        SoapySDRRange *ranges = SoapySDRDevice_getFrequencyRange(sdr, SOAPY_SDR_RX, 0, &length);
        printf("Rx freq ranges: ");
        for (size_t i = 0; i < length; i++) printf("[%g Hz -> %g Hz], ", ranges[i].minimum, ranges[i].maximum);
        printf("\n");

        //GMB: crash because *ranges has been allocated by a DLL
        //free(ranges);

        //apply settings
        if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_RX, 0, 1e6) != 0)
        {
            printf("setSampleRate fail: %s\n", SoapySDRDevice_lastError());
        }
        if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_RX, 0, 912.3e6, NULL) != 0)
        {
            printf("setFrequency fail: %s\n", SoapySDRDevice_lastError());
        }

        //setup a stream (complex floats)
        SoapySDRStream *rxStream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_RX, SOAPY_SDR_CF32, NULL, 0, NULL);
        // not compiled at least since 0.7.1
        // use below instead { // just for build
        //SoapySDRStream *rxStream;
        //SoapySDRDevice_setupStream(sdr, &rxStream, SOAPY_SDR_RX, SOAPY_SDR_CF32, NULL, 0 , NULL );
        // }
        if (rxStream == NULL)
        {
            printf("setupStream fail: %s\n", SoapySDRDevice_lastError());
            SoapySDRDevice_unmake(sdr);
            return EXIT_FAILURE;
        }
        SoapySDRDevice_activateStream(sdr, rxStream, 0, 0, 0); //start streaming

        //create a re-usable buffer for rx samples
        complex float buff[1024];

        //receive some samples
        for (size_t i = 0; i < 10; i++)
        {
            void *buffs[] = {buff}; //array of buffers
            int flags; //flags set by receive operation
            long long timeNs; //timestamp for receive buffer
            int ret = SoapySDRDevice_readStream(sdr, rxStream, buffs, 1024, &flags, &timeNs, 100000);
            printf("ret=%d, flags=%d, timeNs=%I64d\n", ret, flags, timeNs);
        }

        //shutdown the stream
        SoapySDRDevice_deactivateStream(sdr, rxStream, 0, 0); //stop streaming
        SoapySDRDevice_closeStream(sdr, rxStream);

        //cleanup device handle
        SoapySDRDevice_unmake(sdr);

       printf("Done\n");
    }
    return EXIT_SUCCESS;
}
