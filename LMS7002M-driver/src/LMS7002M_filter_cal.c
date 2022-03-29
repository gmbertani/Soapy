///
/// \file LMS7002M_filter_cal.c
///
/// Common filter calibration functions for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2016-2016 Fairwaves, Inc.
/// Copyright (c) 2016-2016 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include "LMS7002M_impl.h"
#include "LMS7002M_filter_cal.h"
#include <LMS7002M/LMS7002M_time.h>

static long long cal_rssi_sleep_ticks(void)
{
    return (LMS7_time_tps())/1000; //1 ms -> ticks
}

uint16_t cal_read_rssi(LMS7002M_t *self, const LMS7002M_chan_t channel)
{
    LMS7_sleep_for(cal_rssi_sleep_ticks());
    return LMS7002M_rxtsp_read_rssi(self, channel);
}

void set_addrs_to_default(LMS7002M_t *self, const LMS7002M_chan_t channel, const int start_addr, const int stop_addr)
{
    LMS7002M_set_mac_ch(self, channel);
    for (int addr = start_addr; addr <= stop_addr; addr++)
    {
        int value = LMS7002M_regs_default(addr);
        if (value == -1) continue; //not in map
        LMS7002M_regs_set(LMS7002M_regs(self), addr, value);
        LMS7002M_regs_spi_write(self, addr);
    }
}

int cal_gain_selection(LMS7002M_t *self, const LMS7002M_chan_t channel)
{
    while (true)
    {
        const int rssi_value_50k = cal_read_rssi(self, channel);
        if (rssi_value_50k < 0x8400) break;

        LMS7002M_regs(self)->reg_0x0108_cg_iamp_tbb++;
        if (LMS7002M_regs(self)->reg_0x0108_cg_iamp_tbb > 63)
        {
            if (LMS7002M_regs(self)->reg_0x0119_g_pga_rbb > 31) break;
            LMS7002M_regs(self)->reg_0x0108_cg_iamp_tbb = 1;
            LMS7002M_regs(self)->reg_0x0119_g_pga_rbb += 6;
        }

        LMS7002M_regs_spi_write(self, 0x0108);
        LMS7002M_regs_spi_write(self, 0x0119);
    }
    return cal_read_rssi(self, channel);
}

int cal_setup_cgen(LMS7002M_t *self, const double bw)
{
    double cgen_freq = bw*20;
    if (cgen_freq < 60e6) cgen_freq = 60e6;
    if (cgen_freq > 640e6) cgen_freq = 640e6;
    while ((int)(cgen_freq/1e6) == (int)(bw/16e6)) cgen_freq -= 10e6;
    return LMS7002M_set_data_clock(self, self->cgen_fref, cgen_freq, NULL);
}
