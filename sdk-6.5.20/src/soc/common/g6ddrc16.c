/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       g6ddrc16.c
 * Purpose:    Phy driver for 16nm combo GDDR6 phy
 */

#include <shared/bsl.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/phy.h>
#include <soc/g6ddrc16.h>
#include <soc/shmoo_g6phy16.h>

#define G6DDRC16_REG_INFO(reg) (soc_phy_gddr6_reg_list[reg])
#define G6DDRC16_REG_IS_VALID(reg) ((reg) >= 0 && (reg) < SOC_PHY_GDDR6_NUM_REGS)

int soc_phy_g6ddrc16_reg32_read(int unit, soc_reg_t reg, int instance, uint32 *data)
{
    uint32 reg_addr = G6DDRC16_REG_INFO(reg).offset;
    return soc_g6phy16_phy_reg_read(unit, instance, reg_addr, data);
}

int soc_phy_g6ddrc16_reg32_write(int unit, soc_reg_t reg, int instance, uint32 data)
{
    uint32 reg_addr = G6DDRC16_REG_INFO(reg).offset;
    return soc_g6phy16_phy_reg_write(unit, instance, reg_addr, data);
}

int soc_phy_g6ddrc16_reg_field_set(int unit, soc_reg_t reg, uint32 *regval, soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!G6DDRC16_REG_IS_VALID(reg))
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, G6DDRC16_REG_INFO(reg).fields, G6DDRC16_REG_INFO(reg).nFields, finfop);
    if (finfop == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Field %d not found \n"), field));
        return SOC_E_NOT_FOUND;
    }

    if (finfop->len < 32)
    {
        mask = (1 << finfop->len) - 1;
        if ((value & ~mask) != 0)
        {
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "soc_phy_ddrc16_reg_field_set(): value too big for field\n")));
            return SOC_E_PARAM;
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
    return SOC_E_NONE;
}

int soc_phy_g6ddrc16_reg_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field, uint32* field_val)
{
    soc_field_info_t *finfop;
    uint32           val;

    if (!G6DDRC16_REG_IS_VALID(reg))
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, G6DDRC16_REG_INFO(reg).fields, G6DDRC16_REG_INFO(reg).nFields, finfop);
    if (finfop == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Field %d not found \n"), field));
        return SOC_E_NOT_FOUND;
    }

    /* coverity[var_deref_op : FALSE] */
    val = regval >> finfop->bp;
    if (finfop->len < 32)
    {
        *field_val = val & ((1 << finfop->len) - 1);
    } else {
        *field_val = val;
    }

    return SOC_E_NONE;
}

