/** \file src/soc/common/hbm_phy.c
 *  
 * common HBM Phy register and fields read/write functions 
 * implementations. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/phy.h>
#include <soc/hbm_phy_regs.h>
#include <soc/shmoo_hbm16.h>


/*
 * }
 */


/*
 * MACROs
 * {
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_DDR
#define HBM16_MIDSTACK_REG_INFO(reg) (soc_hbm16_midstack_reg_list[reg])
#define HBM16_MIDSTACK_REG_IS_VALID(reg) ((reg) >= 0 && (reg) < HBM16_MIDSTACK_NUM_REGS)
#define HBM16_CHANNEL_REG_INFO(reg) (soc_hbm16_channel_reg_list[reg])
#define HBM16_CHANNEL_REG_IS_VALID(reg) ((reg) >= 0 && (reg) < HBM16_CHANNEL_NUM_REGS)
/*
 * }
 */


/*
 * APIs
 * {
 */
int soc_hbm16_midstack_read(int unit, int hbm_idx, soc_reg_t reg, int instance, uint32 *data)
{
     uint32 reg_addr = HBM16_MIDSTACK_REG_INFO(reg).offset & 0xfff;

    return soc_hbm16_phy_reg_read(unit, hbm_idx, SHMOO_HBM16_MIDSTACK_CHANNEL, reg_addr, data);
}

int soc_hbm16_midstack_write(int unit, int hbm_idx, soc_reg_t reg, int instance, uint32 data)
{
    uint32 reg_addr = HBM16_MIDSTACK_REG_INFO(reg).offset & 0xfff;

    return soc_hbm16_phy_reg_write(unit, hbm_idx, SHMOO_HBM16_MIDSTACK_CHANNEL, reg_addr, data);
}

int soc_hbm16_midstack_field_set(int unit, soc_reg_t reg, uint32 *regval, soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!HBM16_MIDSTACK_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_MIDSTACK_REG_INFO(reg).fields, HBM16_MIDSTACK_REG_INFO(reg).nFields, finfop);
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
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "soc_hbm16_midstack_field_set(): value too big for field\n")));
            return SOC_E_PARAM;
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
    return SOC_E_NONE;
}



int soc_hbm16_midstack_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field, uint32* field_val)
{
    soc_field_info_t *finfop;
    uint32           val;

    if (!HBM16_MIDSTACK_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_MIDSTACK_REG_INFO(reg).fields, HBM16_MIDSTACK_REG_INFO(reg).nFields, finfop);
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

int soc_hbm16_midstack_field_size_get(int unit, soc_reg_t reg, soc_field_t field, uint32* field_size)
{
    soc_field_info_t *finfop;

    if (!HBM16_MIDSTACK_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_MIDSTACK_REG_INFO(reg).fields, HBM16_MIDSTACK_REG_INFO(reg).nFields, finfop);
    if (finfop == NULL) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Field %d not found \n"), field));
        return SOC_E_NOT_FOUND;
    }

    *field_size = finfop->len;
    return SOC_E_NONE;
}

int soc_hbm16_channel_read(int unit, int hbm_idx, int channel, soc_reg_t reg, int instance, uint32 *data)
{
    uint32 reg_addr = HBM16_CHANNEL_REG_INFO(reg).offset & 0xfff;

    return soc_hbm16_phy_reg_read(unit, hbm_idx, channel, reg_addr, data);
}

int soc_hbm16_channel_write(int unit, int hbm_idx, int channel,  soc_reg_t reg, int instance, uint32 data)
{
    uint32 reg_addr = HBM16_CHANNEL_REG_INFO(reg).offset & 0xfff;

    return soc_hbm16_phy_reg_write(unit, hbm_idx, channel, reg_addr, data);
}

int soc_hbm16_channel_field_set(int unit, soc_reg_t reg, uint32 *regval, soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!HBM16_CHANNEL_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_CHANNEL_REG_INFO(reg).fields, HBM16_CHANNEL_REG_INFO(reg).nFields, finfop);
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
            LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "soc_hbm16_channel_field_set(): value too big for field\n")));
            return SOC_E_PARAM;
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
    return SOC_E_NONE;
}

int soc_hbm16_channel_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field, uint32* field_val)
{
    soc_field_info_t *finfop;
    uint32           val;

    if (!HBM16_CHANNEL_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_CHANNEL_REG_INFO(reg).fields, HBM16_CHANNEL_REG_INFO(reg).nFields, finfop);
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

int soc_hbm16_channel_field_size_get(int unit, soc_reg_t reg, soc_field_t field, uint32* field_size)
{
    soc_field_info_t *finfop;

    if (!HBM16_CHANNEL_REG_IS_VALID(reg)) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Register %d is not valid\n"), reg));
        return SOC_E_PARAM;
    }

    SOC_FIND_FIELD(field, HBM16_CHANNEL_REG_INFO(reg).fields, HBM16_CHANNEL_REG_INFO(reg).nFields, finfop);
    if (finfop == NULL) 
    {
        LOG_ERROR(BSL_LS_SOC_DDR, (BSL_META_U(unit, "Field %d not found \n"), field));
        return SOC_E_NOT_FOUND;
    }

    *field_size = finfop->len;

    return SOC_E_NONE;
}


/*
 * }
 */

