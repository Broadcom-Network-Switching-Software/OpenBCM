/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * OOB Flow Control and OOB Stats
 * Purpose: API to set different OOB Flow Control and OOB Stats registers.
 */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <sal/core/libc.h>

#include <shared/bsl.h>
#include <soc/defs.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>

#include <bcm/error.h>
#include <bcm/oob.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/oob.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/stat.h>

#define _BCM_TH3_PORT_EN0_WIDTH 64
#define _BCM_TH3_PORT_EN1_WIDTH 64
#define _BCM_TH3_OOB_FC_EGR_PROFILE_MAX 4
#define _BCM_TH3_PORT_EN2_BEGIN 128
#define _BCM_TH3_OOB_PORT_MAX 155
#define _BCM_TH3_OOB_PORT_INVALID_HW 0xff
#define _BCM_TH3_OOB_PORT_VALID(port) \
               ((port) >= 0 && (port) < _BCM_TH3_OOB_PORT_MAX)

#define _BCM_LPORT_IS_SPARE(port) ((58 == port) || (78 == port) || \
                                  (98 == port) || (138 == port) || (158 == port))


/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_egr_mode_get
 * Purpose:
 *      Get OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_egr_mode_get(int unit,
                                  int *mode)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, MMU_INTFO_CONFIG0r,
                       REG_PORT_ANY, 0, &rval));

    *mode = soc_reg_field_get(unit, MMU_INTFO_CONFIG0r,
                                             rval, ENG_PORT_WIDTHf);

    return BCM_E_NONE;
}

STATIC
int _bcm_th3_oob_port_set_hw(
    int unit,
    int port,
    int oob_port)
{
    int mmu_chip_port, max_index;
    soc_mem_t mem;
    soc_field_t field = OOB_PORT_NUMf;
    mmu_intfo_mmu_port_to_oob_port_mapping_thdi_entry_t ingress_entry;
    mmu_intfo_mmu_port_to_oob_port_mapping_thdo0_entry_t egress_entry;

    mem = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDIm;
    max_index = soc_mem_index_max(unit, mem);
    mmu_chip_port = SOC_TH3_MMU_CHIP_PORT(unit, port);

    if(mmu_chip_port < 0 || mmu_chip_port > max_index) {
        return BCM_E_INTERNAL;
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_chip_port, &ingress_entry));
    soc_mem_field32_set(unit, mem, &ingress_entry, field, oob_port);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                          mmu_chip_port, &ingress_entry));

    mem = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO0m;
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_chip_port, &egress_entry));
    soc_mem_field32_set(unit, mem, &egress_entry, field, oob_port);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                          mmu_chip_port, &egress_entry));

    return BCM_E_NONE;
}

STATIC int _bcm_th3_oob_port_get_hw(
    int unit,
    int port,
    int *oob_port)
{
    int mmu_chip_port, max_index;
    soc_mem_t mem;
    soc_field_t field = OOB_PORT_NUMf;
    mmu_intfo_mmu_port_to_oob_port_mapping_thdi_entry_t ingress_entry;

    if(!SOC_PORT_VALID(unit, port) || oob_port == NULL) {
        return BCM_E_PARAM;
    }
    mem = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDIm;
    max_index = soc_mem_index_max(unit, mem);
    mmu_chip_port = SOC_TH3_MMU_CHIP_PORT(unit, port);

    if(mmu_chip_port < 0 || mmu_chip_port > max_index) {
        return BCM_E_INTERNAL;
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_chip_port, &ingress_entry));
    *oob_port = soc_mem_field32_get(unit, mem, &ingress_entry, field);

    return BCM_E_NONE;
}

int
bcm_tomahawk3_oob_fc_tx_port_mapping_get(
    int unit,
    int max_count,
    int *port_array,
    int *oob_port_array,
    int *count)
{
    int i;
    int num;
    if (port_array == NULL || oob_port_array == NULL ||
        count == NULL) {
        return BCM_E_PARAM;
    }
    *count = 0;
    num = (max_count > _TH3_DEV_PORTS_PER_DEV) ?
                _TH3_DEV_PORTS_PER_DEV : max_count;
    for(i = 0; i < num; i++) {
        if(_BCM_LPORT_IS_SPARE(port_array[i])) {
            oob_port_array[i] = -1;
            continue;
        }
        BCM_IF_ERROR_RETURN(
                _bcm_th3_oob_port_get_hw(unit, port_array[i],
                                                        &oob_port_array[i]));
        if(_BCM_TH3_OOB_PORT_INVALID_HW == oob_port_array[i]) {
            oob_port_array[i] = -1;
        }
    }
    *count = i;
    return BCM_E_NONE;
}


int
bcm_tomahawk3_oob_fc_tx_port_mapping_set(
    int unit,
    int count,
    int *port_array,
    int *oob_port_array)
{
    int i, port, oob_port_used[_BCM_TH3_OOB_PORT_MAX] = { 0 };
    int *lport_allocated = NULL;
    int rv = BCM_E_NONE;

    if (port_array == NULL || oob_port_array == NULL
        || count == 0) {
        return BCM_E_PARAM;
    }
    lport_allocated = sal_alloc(_TH3_DEV_PORTS_PER_DEV * sizeof(int),
                                        "logical port to oob mapping");
    if (lport_allocated == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < _TH3_DEV_PORTS_PER_DEV; i++) {
        lport_allocated[i] = -1;
    }
    /* valid input before writing to HW*/
    for (i = 0; i < count; i++) {
        if(!SOC_PORT_VALID(unit, port_array[i])
            || !_BCM_TH3_OOB_PORT_VALID(oob_port_array[i])) {
            rv = BCM_E_PARAM;
            goto exit;
        }
        /* if more than one port mapped to the same oob port,
         * or one port is mapped to different oob ports
         * input is invalid
        */
        if (oob_port_used[oob_port_array[i]] ||
                  lport_allocated[port_array[i]] != -1) {
            rv = BCM_E_PARAM;
            goto exit;
        }
        oob_port_used[oob_port_array[i]] = 1;
        lport_allocated[port_array[i]] = oob_port_array[i];
    }
    /* populate the entire table */
    PBMP_ALL_ITER(unit, port) {
        if(_BCM_LPORT_IS_SPARE(port)) {
            /* spare port is not mapped to any oob port */
            continue;
        }
        if (lport_allocated[port] != -1) {
            rv = _bcm_th3_oob_port_set_hw(unit, port, lport_allocated[port]);
            if (rv != BCM_E_NONE) {
                goto exit;
            }
        } else {
            rv = _bcm_th3_oob_port_set_hw(unit, port,
                                          _BCM_TH3_OOB_PORT_INVALID_HW);
            if (rv != BCM_E_NONE) {
                goto exit;
            }

        }

    }
exit:
    sal_free(lport_allocated);
    return rv;
}

int
bcm_tomahawk3_oob_fc_tx_queue_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_oob_fc_tx_queue_config_t *config,
    int *count)
{
    int i;
    soc_reg_t reg;
    uint64 rval64;
    soc_reg_t profile_regs[] = {
                                MMU_INTFO_OOBFC_ENG_Q_MAP0r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP1r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP2r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP3r
                               };
    soc_field_t offset_fields[] = {
                         MMUQ0_TO_OOB_BIT_OFFSETf, MMUQ1_TO_OOB_BIT_OFFSETf,
                         MMUQ2_TO_OOB_BIT_OFFSETf, MMUQ3_TO_OOB_BIT_OFFSETf,
                         MMUQ4_TO_OOB_BIT_OFFSETf, MMUQ5_TO_OOB_BIT_OFFSETf,
                         MMUQ6_TO_OOB_BIT_OFFSETf, MMUQ7_TO_OOB_BIT_OFFSETf,
                         MMUQ8_TO_OOB_BIT_OFFSETf, MMUQ9_TO_OOB_BIT_OFFSETf,
                         MMUQ10_TO_OOB_BIT_OFFSETf, MMUQ11_TO_OOB_BIT_OFFSETf
                       };
    soc_field_t en_fields[] = {
                         MMUQ0_TO_OOB_ENf, MMUQ1_TO_OOB_ENf,
                         MMUQ2_TO_OOB_ENf, MMUQ3_TO_OOB_ENf,
                         MMUQ4_TO_OOB_ENf, MMUQ5_TO_OOB_ENf,
                         MMUQ6_TO_OOB_ENf, MMUQ7_TO_OOB_ENf,
                         MMUQ8_TO_OOB_ENf, MMUQ9_TO_OOB_ENf,
                         MMUQ10_TO_OOB_ENf, MMUQ11_TO_OOB_ENf
                       };

    if(config == NULL || profile_id < 0 ||
               profile_id >= _BCM_TH3_OOB_FC_EGR_PROFILE_MAX) {
        return BCM_E_PARAM;
    }
    reg = profile_regs[profile_id];
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN(
       soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
    for(i = 0; i < max_count && i < SOC_TH3_NUM_GP_QUEUES; i++) {
        config[i].cosq = i;
        config[i].queue_enable =
                        soc_reg64_field32_get(unit, reg, rval64, en_fields[i]);
        config[i].queue_offset =
                        soc_reg64_field32_get(unit, reg, rval64, offset_fields[i]);
    }
    *count = i;
    return BCM_E_NONE;
}

int
bcm_tomahawk3_oob_fc_tx_queue_profile_set(
    int unit,
    int profile_id,
    int count,
    bcm_oob_fc_tx_queue_config_t *config)
{
#define _BCM_TH3_OOB_FC_EGR_MODE0_WIDTH 8
#define _BCM_TH3_OOB_FC_EGR_MODE1_WIDTH 16
   int i, mode, num_bit;
   uint64 rval64;
   soc_reg_t reg;
   bcm_oob_fc_tx_queue_config_t entire_profile[SOC_TH3_NUM_GP_QUEUES];
   int offset_ref[_BCM_TH3_OOB_FC_EGR_MODE1_WIDTH] = {0};
   int ret_count = 0;
   soc_reg_t profile_regs[] = {
                                MMU_INTFO_OOBFC_ENG_Q_MAP0r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP1r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP2r,
                                MMU_INTFO_OOBFC_ENG_Q_MAP3r
                               };
   soc_field_t offset_fields[] = {
                         MMUQ0_TO_OOB_BIT_OFFSETf, MMUQ1_TO_OOB_BIT_OFFSETf,
                         MMUQ2_TO_OOB_BIT_OFFSETf, MMUQ3_TO_OOB_BIT_OFFSETf,
                         MMUQ4_TO_OOB_BIT_OFFSETf, MMUQ5_TO_OOB_BIT_OFFSETf,
                         MMUQ6_TO_OOB_BIT_OFFSETf, MMUQ7_TO_OOB_BIT_OFFSETf,
                         MMUQ8_TO_OOB_BIT_OFFSETf, MMUQ9_TO_OOB_BIT_OFFSETf,
                         MMUQ10_TO_OOB_BIT_OFFSETf, MMUQ11_TO_OOB_BIT_OFFSETf
                       };
   soc_field_t en_fields[] = {
                         MMUQ0_TO_OOB_ENf, MMUQ1_TO_OOB_ENf,
                         MMUQ2_TO_OOB_ENf, MMUQ3_TO_OOB_ENf,
                         MMUQ4_TO_OOB_ENf, MMUQ5_TO_OOB_ENf,
                         MMUQ6_TO_OOB_ENf, MMUQ7_TO_OOB_ENf,
                         MMUQ8_TO_OOB_ENf, MMUQ9_TO_OOB_ENf,
                         MMUQ10_TO_OOB_ENf, MMUQ11_TO_OOB_ENf
                       };
   if(config == NULL || count <=0 || count > SOC_TH3_NUM_GP_QUEUES ||
        profile_id < 0 || profile_id >= _BCM_TH3_OOB_FC_EGR_PROFILE_MAX) {
        return BCM_E_PARAM;
   }
   num_bit = _BCM_TH3_OOB_FC_EGR_MODE0_WIDTH;
   BCM_IF_ERROR_RETURN(_bcm_th3_oob_fc_egr_mode_get(unit, &mode));
   if(mode == 0) {
        num_bit = _BCM_TH3_OOB_FC_EGR_MODE0_WIDTH;
   } else if (mode == 1) {
        /* Limit to max cos. Bits [15:12] are unused or driven to 0 */
        num_bit = SOC_TH3_COS_MAX;
   }
   reg = profile_regs[profile_id];
   COMPILER_64_ZERO(rval64);
   BCM_IF_ERROR_RETURN(
       soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
   /* get old profile */
   BCM_IF_ERROR_RETURN(
        bcm_tomahawk3_oob_fc_tx_queue_profile_get(unit, profile_id,
                SOC_TH3_NUM_GP_QUEUES, entire_profile, &ret_count));
   /* overwrite existing profile with new entry */
   for (i = 0; i < count; i++) {
        int mmu_q_id;
        if (config[i].queue_offset >= num_bit) {
            return BCM_E_PARAM;
        }
        if (config[i].queue_enable != 0 && config[i].queue_enable != 1) {
            return BCM_E_PARAM;
        }
        if ((config[i].cosq < 0) || (config[i].cosq >= SOC_TH3_NUM_GP_QUEUES))  {
            return BCM_E_PARAM;
        }
        mmu_q_id = config[i].cosq;
        sal_memcpy(&entire_profile[mmu_q_id], &config[i],
                        sizeof(bcm_oob_fc_tx_queue_config_t));
   }
   /* write profile to HW */
   for (i = 0; i < SOC_TH3_NUM_GP_QUEUES; i++) {
        if (entire_profile[i].queue_enable) {
            offset_ref[entire_profile[i].queue_offset]++;
            if (offset_ref[entire_profile[i].queue_offset] > 2) {
                /* at most two queues can be mapped to the same bit offset */
                return BCM_E_PARAM;
            }
        }
        soc_reg64_field32_set(unit, reg, &rval64, offset_fields[i],
                                    entire_profile[i].queue_offset);
        soc_reg64_field32_set(unit, reg, &rval64, en_fields[i],
                                    entire_profile[i].queue_enable);
   }
   BCM_IF_ERROR_RETURN(
       soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));

#undef _BCM_TH3_OOB_FC_EGR_MODE0_WIDTH
#undef _BCM_TH3_OOB_FC_EGR_MODE1_WIDTH

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_port_ing_egr_set
 * Purpose:
 *      Set OOB FC Tx Port reporting configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      port - (IN) Local port
 *      status - (IN) Status of Congestion State reporting
 *      dir - (IN) Direction 0 = Ingreess, 1 = Egress,
 *                           2 = Internal congestion reporting.
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_th3_oob_fc_tx_port_ing_egr_set(int unit, bcm_port_t port,
                                    int status, int dir)
{
    uint32 rval = 0;
    uint64 rval64;
    int oob_port = -1;
    static const soc_field_t field[3] = {ING_PORT_ENf,
                                         ENG_PORT_ENf, CONGST_ST_ENf};
    static const soc_reg_t reg[3][3] = {
        {MMU_INTFO_OOBFC_ING_PORT_EN0_64r, MMU_INTFO_OOBFC_ING_PORT_EN1_64r,
         MMU_INTFO_OOBFC_ING_PORT_EN2r},
        {MMU_INTFO_OOBFC_ENG_PORT_EN0_64r, MMU_INTFO_OOBFC_ENG_PORT_EN1_64r,
         MMU_INTFO_OOBFC_ENG_PORT_EN2r},
        {MMU_INTFO_OOBFC_CONGST_ST_EN0_64r, MMU_INTFO_OOBFC_CONGST_ST_EN1_64r,
         MMU_INTFO_OOBFC_CONGST_ST_EN2r}};

    if ((status != 1) && (status != 0)) {
        /* 0: disable
         * 1: enable
        */
        return BCM_E_PARAM;
    }

    if (dir > 2) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN(
        _bcm_th3_oob_port_get_hw(unit, port, &oob_port));
    if(_BCM_TH3_OOB_PORT_INVALID_HW == oob_port) {
        return BCM_E_PORT;
    }

    if ((oob_port >= 0) && (oob_port < _BCM_TH3_PORT_EN0_WIDTH)) {
        /* For OOB port 0-63 */

        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][0], REG_PORT_ANY, 0, &rval64));
        if (((status == 1) && (!COMPILER_64_BITTEST(rval64, oob_port))) ||
            ((status == 0) && (COMPILER_64_BITTEST(rval64, oob_port)))) {
            if (status == 1) {
                COMPILER_64_BITSET(rval64, oob_port);
            } else {
                COMPILER_64_BITCLR(rval64, oob_port);
            }
            soc_reg64_field_set(unit, reg[dir][0], &rval64,
                                field[dir], rval64);
            BCM_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg[dir][0], REG_PORT_ANY, 0, rval64));
        }
    } else if ((oob_port >= _BCM_TH3_PORT_EN0_WIDTH) &&
                (oob_port < _BCM_TH3_PORT_EN2_BEGIN)) {
        /* For OOB port 64-127 */

        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][1], REG_PORT_ANY, 0, &rval64));
        if (((status == 1) && (!COMPILER_64_BITTEST(rval64, (oob_port % _BCM_TH3_PORT_EN0_WIDTH)))) ||
            ((status == 0) && (COMPILER_64_BITTEST(rval64, (oob_port % _BCM_TH3_PORT_EN0_WIDTH))))) {
            if (status == 1) {
                COMPILER_64_BITSET(rval64, (oob_port % _BCM_TH3_PORT_EN0_WIDTH));
            } else {
                COMPILER_64_BITCLR(rval64, (oob_port % _BCM_TH3_PORT_EN0_WIDTH));
            }
            soc_reg64_field_set(unit, reg[dir][1], &rval64,
                                field[dir], rval64);
            BCM_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg[dir][1], REG_PORT_ANY, 0, rval64));
        }
    } else if ((oob_port >= _BCM_TH3_PORT_EN2_BEGIN)
                            && (oob_port < _BCM_TH3_OOB_PORT_MAX)) {
        /* For oob port 128-154 */

        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg[dir][2], REG_PORT_ANY, 0, &rval));
        if (((status == 1) && (!(rval & (0x1 << (oob_port % _BCM_TH3_PORT_EN2_BEGIN))))) ||
            ((status == 0) && ((rval & (0x1 << (oob_port % _BCM_TH3_PORT_EN2_BEGIN)))))) {
            if (status == 1) {
                rval |= 0x1 << (oob_port % _BCM_TH3_PORT_EN2_BEGIN);
            } else {
                rval &= ~(0x1 << (oob_port % _BCM_TH3_PORT_EN2_BEGIN));
            }
            soc_reg_field_set(unit, reg[dir][2], &rval, field[dir], rval);

            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg[dir][2], REG_PORT_ANY, 0, rval));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

static int
_bcm_th3_oob_fc_tx_congst_set(int unit, bcm_port_t port) {
    int ing_status = 0, egr_status = 0;

    BCM_IF_ERROR_RETURN(
        bcm_th3_oob_fc_tx_port_control_get(unit, port, &ing_status, 0));
    BCM_IF_ERROR_RETURN(
        bcm_th3_oob_fc_tx_port_control_get(unit, port, &egr_status, 1));

    /* turn off congestion reporting from MMU to OOB
     * only when ingress and egress reporting are both turned off
    */
    if (!ing_status && !egr_status) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_oob_fc_tx_port_ing_egr_set(unit, port, 0, 2));
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_oob_fc_tx_port_ing_egr_set(unit, port, 1, 2));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_oob_fc_tx_port_control_set
 * Purpose:
 *      Set OOB FC Tx Port configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      port - (IN) Local port
 *      status - (IN) Status of Congestion State reporting
 *      dir - (IN) Direction 0 = Ingreess, 1 = Egress
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                              int status, int dir)
{
    if ((status != 1) && (status != 0)) {
        /* 0: disable
         * 1: enable
        */
        return BCM_E_PARAM;
    }
    if (dir > 1) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        _bcm_th3_oob_fc_tx_port_ing_egr_set(unit, port, status, dir));
    BCM_IF_ERROR_RETURN(
        _bcm_th3_oob_fc_tx_congst_set(unit, port));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_oob_fc_tx_port_control_get
 * Purpose:
 *      Get OOB FC Tx side Port configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      port - (IN) Logical port
 *      status - (OUT) Status of Congestion State reporting
 *      dir - (IN) Direction 0 = Ingreess, 1 = Egress
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                 int *status, int dir)
{
    uint32 rval = 0;
    uint64 rval64;
    int oob_port = -1;
    static const soc_reg_t reg[2][3] = {
        {MMU_INTFO_OOBFC_ING_PORT_EN0_64r, MMU_INTFO_OOBFC_ING_PORT_EN1_64r,
            MMU_INTFO_OOBFC_ING_PORT_EN2r},
        {MMU_INTFO_OOBFC_ENG_PORT_EN0_64r, MMU_INTFO_OOBFC_ENG_PORT_EN1_64r,
            MMU_INTFO_OOBFC_ENG_PORT_EN2r}};

    if (dir > 1) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        _bcm_th3_oob_port_get_hw(unit, port, &oob_port));
    if(_BCM_TH3_OOB_PORT_INVALID_HW == oob_port) {
        return BCM_E_NOT_FOUND;
    }
    COMPILER_64_ZERO(rval64);

    if ((oob_port >= 0) && (oob_port < _BCM_TH3_PORT_EN0_WIDTH)) {
        /* For logical port 0-63 */
        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][0], REG_PORT_ANY, 0, &rval64));
        if (COMPILER_64_BITTEST(rval64, oob_port)) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else if ((oob_port >= _BCM_TH3_PORT_EN0_WIDTH) &&
                    (oob_port < _BCM_TH3_PORT_EN2_BEGIN)) {
        /* For logical port 64-127 */
        BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg[dir][1], REG_PORT_ANY, 0, &rval64));

        if (COMPILER_64_BITTEST(rval64, (oob_port % _BCM_TH3_PORT_EN0_WIDTH))) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else if ((oob_port >= _BCM_TH3_PORT_EN2_BEGIN) &&
                    (oob_port < _BCM_TH3_OOB_PORT_MAX)) {
        /* For logical port 128-154 */
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg[dir][2], REG_PORT_ANY, 0, &rval));
        if (rval & (0x1 << (oob_port % _BCM_TH3_PORT_EN2_BEGIN))) {
            *status = 0x1;
        } else {
            *status = 0x0;
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int bcm_th3_oob_fx_egress_profile_id_set(int unit, bcm_port_t port,
                                           int profile_id)
{
    soc_mem_t mem = MMU_INTFO_OOBFC_ENG_PORT_PSELm;
    int max_index, mmu_chip_port;
    soc_field_t field = ENG_PORT_PROFILE_SELf;
    mmu_intfo_oobfc_eng_port_psel_entry_t entry;
    if(!(SOC_PORT_VALID(unit, port)) || profile_id < 0 ||
           profile_id > _BCM_TH3_OOB_FC_EGR_PROFILE_MAX) {
        return BCM_E_PARAM;
    }
    max_index = soc_mem_index_max(unit, mem);
    mmu_chip_port = SOC_TH3_MMU_CHIP_PORT(unit, port);

    if(mmu_chip_port < 0 || mmu_chip_port > max_index) {
        return BCM_E_INTERNAL;
    }
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_chip_port, &entry));
    soc_mem_field32_set(unit, mem, &entry, field, profile_id);
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));

    return BCM_E_NONE;
}

int bcm_th3_oob_fx_egress_profile_id_get(int unit, bcm_port_t port,
                                           int *profile_id)
{
    soc_mem_t mem = MMU_INTFO_OOBFC_ENG_PORT_PSELm;
    soc_field_t field = ENG_PORT_PROFILE_SELf;
    int max_index, mmu_chip_port;
    mmu_intfo_oobfc_eng_port_psel_entry_t entry;
    if(!(SOC_PORT_VALID(unit, port)) || profile_id == NULL) {
        return BCM_E_PARAM;
    }
    max_index = soc_mem_index_max(unit, mem);
    mmu_chip_port = SOC_TH3_MMU_CHIP_PORT(unit, port);

    if(mmu_chip_port < 0 || mmu_chip_port > max_index) {
        return BCM_E_INTERNAL;
    }
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_chip_port, &entry));
    *profile_id = soc_mem_field32_get(unit, mem, &entry, field);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_flags_set
 * Purpose:
 *      Set OOB FC Tx enable flags
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      flags - (IN) OOB FC Tx flags
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_flags_set(int unit, uint32 flags)
{
    uint32 rval = 0;
    uint32 fval = 0;
    soc_reg_t reg = INVALIDr;

    if (flags & BCM_OOB_FC_TX_FCN_EN) {
        return BCM_E_UNAVAIL;
    }

    reg = MMU_INTFO_OOBFC_CHANNEL_BASE_64r;
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (flags & BCM_OOB_FC_TX_ING_EN) {
        fval = 1;
    }
    soc_reg_field_set(unit, reg, &rval, ING_ENf, fval);

    fval = 0;
    if (flags & BCM_OOB_FC_TX_EGR_EN) {
        fval = 1;
    }
    soc_reg_field_set(unit, reg, &rval, ENG_ENf, fval);

    fval = 0;
    if (flags & BCM_OOB_FC_TX_POOL_EN) {
        fval = 1;
    }
    soc_reg_field_set(unit, reg, &rval, POOL_ENf, fval);

    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_flags_get
 * Purpose:
 *      Get OOB FC Tx enable status
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_flags_get(int unit,
                              bcm_oob_fc_tx_config_t *config)
{
    uint32 fval;
    uint32 rval;
    soc_reg_t reg = MMU_INTFO_OOBFC_CHANNEL_BASE_64r;
    rval = 0;

    config->flags = 0x0;


    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    fval = soc_reg_field_get(unit, reg, rval, ING_ENf);
    if (fval) {
        config->flags |= BCM_OOB_FC_TX_ING_EN;
    }

    fval = soc_reg_field_get(unit, reg, rval, ENG_ENf);
    if (fval) {
        config->flags |= BCM_OOB_FC_TX_EGR_EN;
    }

    fval = soc_reg_field_get(unit, reg, rval, POOL_ENf);

    if (fval) {
        config->flags |= BCM_OOB_FC_TX_POOL_EN;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_gcs_id_set
 * Purpose:
 *      Set OOB FC Tx global congestion reporting
 *      service pool id
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      id - (IN) OOB FC Tx global congestion service pool id
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_gcs_id_set(int unit,
                             bcm_service_pool_id_t id)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFO_OOBFC_GCSr;
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval,
                      GCS_IDf, id);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_gcs_id_get
 * Purpose:
 *      Get OOB FC Tx global congestion reporting
 *      service pool id
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_gcs_id_get(int unit,
                              bcm_oob_fc_tx_config_t *config)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, MMU_INTFO_OOBFC_GCSr, REG_PORT_ANY, 0, &rval));

    config->gcs_id = soc_reg_field_get(unit, MMU_INTFO_OOBFC_GCSr, rval,
                                       GCS_IDf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_ipg_set
 * Purpose:
 *      Set OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      ipg - (IN) OOB FC Tx inter packet gap
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_ipg_set(int unit, uint8 ipg)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFO_OOBFC_TX_IDLEr;
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, IDLE_GAPf, ipg);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_oob_fc_tx_config_ipg_get
 * Purpose:
 *      Get OOB FC Tx inter packet gap
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_ipg_get(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, MMU_INTFO_OOBFC_TX_IDLEr, REG_PORT_ANY, 0, &rval));

    config->inter_pkt_gap = soc_reg_field_get(unit, MMU_INTFO_OOBFC_TX_IDLEr,
                                              rval, IDLE_GAPf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_egr_mode_set
 * Purpose:
 *      Set OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      mode - (IN) OOB FC Tx egress congestion reporting mode
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_egr_mode_set(int unit,
                             int mode)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFO_CONFIG0r;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    soc_reg_field_set(unit, reg, &rval, ENG_PORT_WIDTHf, mode);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_oob_fc_tx_config_egr_mode_get
 * Purpose:
 *      Get OOB FC Tx egress congestion reporting mode
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx configuration
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_oob_fc_tx_config_egr_mode_get(int unit,
                                  bcm_oob_fc_tx_config_t *config)
{
    int mode = 0;
    BCM_IF_ERROR_RETURN(_bcm_th3_oob_fc_egr_mode_get(unit, &mode));

    config->egr_mode = mode;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_oob_fc_tx_config_set
 * Purpose:
 *      Set OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (IN) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_oob_fc_tx_config_set(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if ((config->gcs_id < 0) || (config->gcs_id >= _TH3_MMU_NUM_POOL)) {
        return BCM_E_PARAM;
    }

    /* egress mode - 0: 8bit, 1: 16bit */
    if ((config->egr_mode < 0) || (config->egr_mode > 1)) {
        return BCM_E_PARAM;
    }

    /* Enabling/Disabling OOBFC Tx Ingress, Egress and Service Pool */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_flags_set(unit, config->flags));

    /* Setting Inter packet Gap between two HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_ipg_set(unit, config->inter_pkt_gap));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_gcs_id_set(unit, config->gcs_id));

    /* Setting Egress mode for reporting OOBFC message. */
    
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_egr_mode_set(unit, config->egr_mode));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_oob_fc_tx_config_get
 * Purpose:
 *      Get OOB FC Tx global configuration.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      config - (OUT) OOB FC Tx global configuration
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_oob_fc_tx_config_get(int unit,
                            bcm_oob_fc_tx_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the congestion state reporting status of FCN,
       OOBFC Tx Ingress, Egress and Service Pool */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_flags_get(unit, config));

    /* Get the configured Inter Packet Gap between OOBFC Tx message.*/
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_ipg_get(unit, config));

    /* Setting service pool id whose congestion state will be
       reported in every HCFC message */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_gcs_id_get(unit, config));

    /* Get the configured Egress mode of OOBFC Tx side */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_oob_fc_tx_config_egr_mode_get(unit, config));

    return BCM_E_NONE;
}

int
bcm_th3_oob_port_mapping_init(int unit)
{
    int port, oob_port = 0;
    PBMP_ALL_ITER(unit, port) {
        if (_BCM_LPORT_IS_SPARE(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_oob_port_set_hw(unit, port, _BCM_TH3_OOB_PORT_INVALID_HW));
        }
        BCM_IF_ERROR_RETURN(
                _bcm_th3_oob_port_set_hw(unit, port, oob_port));
        oob_port++;
    }
    return BCM_E_NONE;
}

/*
* Function:
*      bcm_th3_oob_fc_tx_info_get
* Purpose:
*      Get OOB FC Tx global information.
* Parameters:
*      unit - (IN) StrataSwitch unit number.
*      info - (OUT) OOB FC Tx global information
* Returns:
*      BCM_E_XXX
*/
int
bcm_th3_oob_fc_tx_info_get(int unit, bcm_oob_fc_tx_info_t *info)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFO_OOBFC_CHANNEL_BASE_64r; /* 32 bit reg */
    if (info == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    /* Get the Channel base value for HCFC messages containing
       congestion state of different resources */
    /* Ingress Ports */
    info->ing_base =  (uint8)soc_reg_field_get(unit, reg,
                                                   rval, ING_BASEf);

    /* Egress Unicast Queue not supported in TH3*/
    info->ucast_base =  0;

    /* Egress Multicast Queue not supported in TH3*/
    info->mcast_base =  0;

    /* Egress OOB Class */
    info->cos_base =  (uint8)soc_reg_field_get(unit, reg,
                                                    rval, ENG_BASEf);

    /* Ingress and Egress Service Pool Congestion */

    info->pool_base =  (uint8)soc_reg_field_get(unit, reg,
                                                      rval, POOL_BASEf);

    return BCM_E_NONE;
}

#endif


