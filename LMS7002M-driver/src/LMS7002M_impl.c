///
/// \file LMS7002M_impl.c
///
/// Implementation details for the LMS7002M C driver.
/// This file defines the LMS7002M_t structure,
/// and the SPI transaction helper functions.
///
/// \copyright
/// Copyright (c) 2016-2017 Skylark Wireless
/// Copyright (c) 2014-2015 Fairwaves, Inc.
/// Copyright (c) 2014-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "LMS7002M_impl.h"
#include <LMS7002M/LMS7002M_logger.h>

/***********************************************************************
 * Create/destroy implementations
 **********************************************************************/
LMS7002M_t *LMS7002M_create(LMS7002M_spi_transact_t transact, void *handle)
{
    LMS7002M_t *self = (LMS7002M_t *)malloc(sizeof(LMS7002M_t));
    if (self == NULL) return NULL;
    self->spi_transact = transact;
    self->spi_transact_handle = handle;
    LMS7002M_regs_init(&self->_regs[0]);
    LMS7002M_regs_init(&self->_regs[1]);
    self->regs = self->_regs;
    self->cgen_freq = 0.0;
    self->sxr_freq = 0.0;
    self->sxt_freq = 0.0;
    self->cgen_fref = 0.0;
    self->sxr_fref = 0.0;
    self->sxt_fref = 0.0;
    return self;
}

void LMS7002M_destroy(LMS7002M_t *self)
{
    free(self);
}

/***********************************************************************
 * Helper calls to format SPI transactions
 **********************************************************************/
void LMS7002M_spi_write(LMS7002M_t *self, const int addr, const int value)
{
    uint32_t data = (((uint32_t)1) << 31) | (((uint32_t)addr) << 16) | (value & 0xffff);
    self->spi_transact(self->spi_transact_handle, data, false/*no readback*/);
}

int LMS7002M_spi_read(LMS7002M_t *self, const int addr)
{
    uint32_t data = (((uint32_t)addr) << 16);
    return self->spi_transact(self->spi_transact_handle, data, true/*readback*/) & 0xffff;
}

void LMS7002M_regs_spi_write(LMS7002M_t *self, const int addr)
{
    int value = LMS7002M_regs_get(self->regs, addr);
    LMS7002M_spi_write(self, addr, value);

    //for CHAB mode: duplicate to the CHB register shadow
    if (self->regs->reg_0x0020_mac == REG_0X0020_MAC_CHAB)
    {
        LMS7002M_regs_set(&self->_regs[1], addr, value);
    }
}

void LMS7002M_regs_spi_read(LMS7002M_t *self, const int addr)
{
    LMS7002M_regs_set(self->regs, addr, LMS7002M_spi_read(self, addr));
}

LMS7002M_regs_t *LMS7002M_regs(LMS7002M_t *self)
{
    return self->regs;
}

void LMS7002M_regs_to_rfic(LMS7002M_t *self)
{
    LMS7002M_set_mac_ch(self, LMS_CHA);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_write(self, *addrp);
    }

    LMS7002M_set_mac_ch(self, LMS_CHB);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        //ignore registers that do not depend on MAC
        if (*addrp < 0x0100) continue;

        LMS7002M_regs_spi_write(self, *addrp);
    }

    //ensure that we cache the chips actual version register
    //used in the codebase to handle hardware rev differences
    LMS7002M_regs_spi_read(self, 0x002f);
}

void LMS7002M_rfic_to_regs(LMS7002M_t *self)
{
    LMS7002M_set_mac_ch(self, LMS_CHA);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_read(self, *addrp);
    }

    LMS7002M_set_mac_ch(self, LMS_CHB);
    for (const int *addrp = LMS7002M_regs_addrs(); *addrp != 0; addrp++)
    {
        LMS7002M_regs_spi_read(self, *addrp);
    }
}

int LMS7002M_dump_ini(LMS7002M_t *self, const char *path)
{
    FILE *p = fopen(path, "w");
    if (p == NULL) return -1;

    fprintf(p, "[FILE INFO]\n");
    fprintf(p, "type=LMS7002 configuration\n");
    fprintf(p, "version=1\n");

    fprintf(p, "[Frequencies]\n");
    fprintf(p, "CGEN frequency MHz=%f\n", self->cgen_freq/1e6);
    fprintf(p, "SXR frequency MHz=%f\n", self->sxr_freq/1e6);
    fprintf(p, "SXT frequency MHz=%f\n", self->sxt_freq/1e6);

    size_t i = 0;
    const int *addrs = LMS7002M_regs_addrs();

    fprintf(p, "[LMS7002 registers ch.A]\n");
    LMS7002M_set_mac_ch(self, LMS_CHA);
    i = 0;
    while (addrs[i] != 0x0000)
    {
        fprintf(p, "0x%04x=0x%04x\n", addrs[i], LMS7002M_spi_read(self, addrs[i]));
        i++;
    }

    fprintf(p, "[LMS7002 registers ch.B]\n");
    LMS7002M_set_mac_ch(self, LMS_CHB);
    i = 0;
    while (addrs[i] != 0x0000)
    {
        fprintf(p, "0x%04x=0x%04x\n", addrs[i], LMS7002M_spi_read(self, addrs[i]));
        i++;
    }

    fprintf(p, "[Reference clocks]\n");
    fprintf(p, "CGEN reference frequency MHz=%f\n", self->cgen_fref/1e6);
    fprintf(p, "SXR reference frequency MHz=%f\n", self->sxr_fref/1e6);
    fprintf(p, "SXT reference frequency MHz=%f\n", self->sxt_fref/1e6);

    return fclose(p);
}

int LMS7002M_load_ini(LMS7002M_t *self, const char *path)
{
    FILE *p = fopen(path, "r");
    if (p == NULL) return -1;

    bool write_reg_ok = false;
    LMS7002M_chan_t chan = LMS_CHA;

    while (true)
    {
        char *line = NULL;
        size_t n = 0;
        int ret = getline(&line, &n, p);
        if (ret < 0)
        {
            free(line);
            break; //EOF
        }
        while (ret > 0 && isspace(line[ret-1]))
        {
            line[ret-1] = '\0'; //strip newline
            ret--;
        }

        //parse ini sections
        if (line[0] == '[')
        {
            if (strcmp(line, "[LMS7002 registers ch.A]") == 0)
            {
                LMS7_logf(LMS7_INFO, "Found section %s", line);
                write_reg_ok = true;
                chan = LMS_CHA;
            }
            else if (strcmp(line, "[LMS7002 registers ch.B]") == 0)
            {
                LMS7_logf(LMS7_INFO, "Found section %s", line);
                write_reg_ok = true;
                chan = LMS_CHB;
            }
            else write_reg_ok = false;
        }

        //parse values
        else if (write_reg_ok)
        {
            unsigned int addr = 0, value = 0;
            ret = sscanf(line, "0x%04x=0x%04x", &addr, &value);
            if (ret > 0)
            {
                LMS7002M_set_mac_ch(self, chan);
                LMS7002M_spi_write(self, addr, value);
                LMS7002M_regs_set(self->regs, addr, value);
                LMS7_logf(LMS7_DEBUG, "Load: 0x%04x=0x%04x", addr, value);
            }
        }

        free(line);
    }

    return fclose(p);
}
