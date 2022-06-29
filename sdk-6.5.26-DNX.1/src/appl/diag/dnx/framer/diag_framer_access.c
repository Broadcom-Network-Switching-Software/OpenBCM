/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Symbol accessing.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SYMTAB

#include <shared/bsl.h>
#include <shared/error.h>
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_access.h"

static int access_debug = 0;

int
framer_reg_raw_read(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 dw,
    J2X_U32 * data)
{
    J2X_U32 i;
    SHR_FUNC_INIT_VARS(unit);
    for (i = 0; i < dw; i++)
    {
        SHR_IF_ERR_EXIT_NO_MSG(framer_regp_read(unit, base_addr, offset + i, &data[i]));
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_reg_raw_write(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 dw,
    J2X_U32 * data)
{
    J2X_U32 i;
    SHR_FUNC_INIT_VARS(unit);
    for (i = 0; i < dw; i++)
    {
        SHR_IF_ERR_EXIT_NO_MSG(framer_regp_write(unit, base_addr, offset + i, data[i]));
    }
exit:
    SHR_FUNC_EXIT;
}

int
framer_mem_raw_read(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 index,
    J2X_U32 width,
    J2X_U32 * data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_NO_MSG(framer_ramp_read(unit, base_addr, offset, index, width, data));
exit:
    SHR_FUNC_EXIT;
}

int
framer_mem_raw_write(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 index,
    J2X_U32 width,
    J2X_U32 * data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_NO_MSG(framer_ramp_write(unit, base_addr, offset, index, width, data));
exit:
    SHR_FUNC_EXIT;
}

int
framer_field_set(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 * value)
{
    int j, msb, lsb;
    int wmsb, wlsb, msbm, lsbm, msb_j, lsb_j;
    J2X_U32 fvalue, fwidth, fmask, prev_value;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    lsb = shift;
    msb = shift + width - 1;
    wmsb = msb / 32;
    wlsb = lsb / 32;
    msbm = msb % 32;
    lsbm = lsb % 32;
    prev_value = 0;
    for (j = wlsb; j <= wmsb; j++)
    {
        if (access_debug)
        {
            cli_out("msb = %d, lsb = %d, wmsb = %d, wlsb = %d, msbm = %d, lsbm = %d\n",
                    msb, lsb, wmsb, wlsb, msbm, lsbm);
            cli_out("data[%d] = 0x%08x, value[%d] = 0x%08x\n", j, data[j], j - wlsb, value[j - wlsb]);
        }
        fvalue = 0;
        if (wlsb == wmsb)
        {
            msb_j = msbm;
            lsb_j = lsbm;
            fwidth = msb_j - lsb_j + 1;
            fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
            fvalue |= (value[j - wlsb] << lsb_j) & fmask;
            data[j] = (data[j] & ~fmask) | fvalue;
        }
        else if (j == wlsb)
        {
            msb_j = 32 - 1;
            lsb_j = lsbm;
            fwidth = msb_j - lsb_j + 1;
            fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
            fvalue |= (value[j - wlsb] << lsb_j) & fmask;
            data[j] = (data[j] & ~fmask) | fvalue;
        }
        else if (j == wmsb)
        {
            if (msbm >= lsbm)
            {
                msb_j = msbm;
                lsb_j = lsbm;
                fwidth = msb_j - lsb_j + 1;
                fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
                fvalue |= (value[j - wlsb] << lsb_j) & fmask;
                msb_j = lsbm - 1;
                lsb_j = 0;
                fwidth = msb_j - lsb_j + 1;
                fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
                fvalue |= (prev_value >> (32 - msb_j)) & fmask;
                msb_j = msbm;
                lsb_j = 0;
                fwidth = msb_j - lsb_j + 1;
                fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
                data[j] = (data[j] & ~fmask) | fvalue;
            }
            else
            {
                msb_j = lsbm;
                lsb_j = 0;
                fwidth = msb_j - lsb_j + 1;
                fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
                fvalue |= (prev_value >> (32 - msb_j)) & fmask;
                msb_j = msbm;
                lsb_j = 0;
                fwidth = msb_j - lsb_j + 1;
                fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
                data[j] = (data[j] & ~fmask) | fvalue;
            }
        }
        else
        {
            msb_j = 32 - 1;
            lsb_j = lsbm;
            fwidth = msb_j - lsb_j + 1;
            fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
            fvalue |= (value[j - wlsb] << lsb_j) & fmask;
            msb_j = lsbm - 1;
            lsb_j = 0;
            fwidth = msb_j - lsb_j + 1;
            fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
            fvalue |= (prev_value >> (32 - msb_j)) & fmask;
            msb_j = 32 - 1;
            lsb_j = 0;
            fwidth = msb_j - lsb_j + 1;
            fmask = fwidth == 32 ? 0xffffffff : ((0x1 << fwidth) - 1) << lsb_j;
            data[j] = (data[j] & ~fmask) | fvalue;
        }
        prev_value = value[j - wlsb];
        if (access_debug)
        {
            cli_out("data[%d] = 0x%08x\n", j, data[j]);
        }
    }

    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NONE);
exit:
    SHR_FUNC_EXIT;
}

int
framer_field32_set(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 value)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_IF_ERR_EXIT_NO_MSG(framer_field_set(data, dw, shift, width, &value));
exit:
    SHR_FUNC_EXIT;
}

int
framer_field_get(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 * value)
{
    int i, j;
    int msb, lsb;
    int wmsb, wlsb, msbm, lsbm;
    J2X_U32 msb_mask, lsb_mask, lsb_mask_revert, lsb_mask_revert_low;
    J2X_U32 fvalue, fwidth, prev_data, curr_data;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    lsb = shift;
    msb = shift + width - 1;
    wmsb = msb / 32;
    wlsb = lsb / 32;
    msbm = msb % 32;
    lsbm = lsb % 32;
    msb_mask = (msbm + 1) >= 32 ? 0xffffffff : (0x1 << (msbm + 1)) - 1;
    lsb_mask = (0x1 << (lsbm)) - 1;
    lsb_mask_revert = (32 - lsbm) >= 32 ? 0 : 0xffffffff & ~lsb_mask;
    lsb_mask_revert_low = (32 - lsbm) >= 32 ? 0xffffffff : ((0x1 << (32 - lsbm)) - 1);
    prev_data = 0;
    i = 0;
    for (j = wmsb; j >= wlsb; j--)
    {
        if (j == wmsb)
        {
            curr_data = data[j] & msb_mask;
        }
        else
        {
            curr_data = data[j];
        }
        fvalue = ((prev_data << (32 - lsbm)) & lsb_mask_revert) | ((curr_data >> lsbm) & lsb_mask_revert_low);
        fwidth = msb - lsb + 1;
        if (access_debug)
        {
            cli_out("\n");
            cli_out("data[%d] = 0x%x, fvalue = 0x%x, fwidth = %d\n", j, data[j], fvalue, fwidth);
            cli_out("    wmsb = %d, wlsb = %d, msbm = %d, lsbm = %d\n", wmsb, wlsb, msbm, lsbm);
            cli_out("    msb_mask = 0x%08x, lsb_mask = 0x%08x\n", msb_mask, lsb_mask);
            cli_out("    lsb_mask_revert = 0x%08x, lsb_mask_revert_low = 0x%08x\n",
                    lsb_mask_revert, lsb_mask_revert_low);
            cli_out("    prev_data = 0x%08x, curr_data = 0x%08x\n", prev_data, curr_data);
            cli_out("\n");
        }
        if (wmsb != wlsb && j == wmsb && msbm <= lsbm)
        {
            continue;
        }
        if (access_debug)
        {
            cli_out("\n");
            cli_out("    value[%d] = 0x%x, fwidth = %d\n", i, fvalue, fwidth);
            cli_out("\n");
        }
        value[i++] = fvalue;
        prev_data = data[j];
    }

    SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NONE);
exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FRAMER_LIB_BUILD */
