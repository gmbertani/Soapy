#ifndef FCDCMD_INCLUDED__H
#define  FCDCMD_INCLUDED__H

#ifdef __cplusplus
extern "C"
{
#endif

#define FCD_HID_CMD_H 1
#define FCD_HID_CMD_QUERY              1 // Returns string with "FCDAPP version"

#define FCD_HID_CMD_SET_FREQUENCY_KHZ    100 // Send with 3 byte unsigned little endian frequency in kHz.
#define FCD_HID_CMD_SET_FREQUENCY_HZ 101 // Send with 4 byte unsigned little endian frequency in Hz, returns with actual frequency set in Hz
#define FCD_HID_CMD_GET_FREQUENCY_HZ 102 // Returns 4 byte unsigned little endian frequency in Hz.

#define FCD_HID_CMD_SET_LNA_GAIN     110 // Send one byte, 1 on, 0 off
#define FCD_HID_CMD_SET_RF_FILTER    113 // Send one byte enum, see TUNERRFFILTERENUM
#define FCD_HID_CMD_SET_MIXER_GAIN   114 // Send one byte, 1 on, 0 off
#define FCD_HID_CMD_SET_IF_GAIN      117 // Send one byte value, valid value 0 to 59 (dB)
#define FCD_HID_CMD_SET_IF_FILTER    122 // Send one byte enum, see TUNERIFFILTERENUM
#define FCD_HID_CMD_SET_BIAS_TEE     126 // Send one byte, 1 on, 0 off (places a voltage on the antenna)

#define FCD_HID_CMD_GET_LNA_GAIN     150 // Returns one byte, 1 on, 0 off
#define FCD_HID_CMD_GET_RF_FILTER    153 // Returns one byte enum, see TUNERRFFILTERENUM
#define FCD_HID_CMD_GET_MIXER_GAIN   154 // Returns one byte, 1 on, 0 off
#define FCD_HID_CMD_GET_IF_GAIN      157 // Returns one byte value, valid value 0 to 59 (dB)
#define FCD_HID_CMD_GET_IF_FILTER    162 // Returns one byte enum, see TUNERIFFILTERENUM
#define FCD_HID_CMD_GET_BIAS_TEE     166 // Returns one byte, 1 on, 0 off

#define FCD_RESET                    255 // Reset to bootloader

// FCD Pro (V1.x) gain commands.. we get to manage gain distribution across the amps
#define FCD_HID_CMD_SET_IF_GAIN1     117
#define FCD_HID_CMD_SET_IF_GAIN2     120
#define FCD_HID_CMD_SET_IF_GAIN3     121
#define FCD_HID_CMD_SET_IF_GAIN4     123
#define FCD_HID_CMD_SET_IF_GAIN5     124
#define FCD_HID_CMD_SET_IF_GAIN6     125

#define FCD_HID_CMD_GET_IF_GAIN1     157
#define FCD_HID_CMD_GET_IF_GAIN2     160
#define FCD_HID_CMD_GET_IF_GAIN3     161
#define FCD_HID_CMD_GET_IF_GAIN4     163
#define FCD_HID_CMD_GET_IF_GAIN5     164
#define FCD_HID_CMD_GET_IF_GAIN6     165

typedef enum
{
	TRFE_0_4,
	TRFE_4_8,
	TRFE_8_16,
	TRFE_16_32,
	TRFE_32_75,
	TRFE_75_125,
	TRFE_125_250,
	TRFE_145,
	TRFE_410_875,
	TRFE_435,
	TRFE_875_2000
} tuner_rf_filter_t;

typedef enum
{
  TIFE_200KHZ=0,
  TIFE_300KHZ=1,
  TIFE_600KHZ=2,
  TIFE_1536KHZ=3,
  TIFE_5MHZ=4,
  TIFE_6MHZ=5,
  TIFE_7MHZ=6,
  TIFE_8MHZ=7
} tuner_if_filter_t;
    
#ifdef __cplusplus
}
#endif

#endif
