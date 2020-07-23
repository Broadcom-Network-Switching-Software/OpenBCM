/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMICx MIIM (MDIO) functions
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_MIIM

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/cmicx_miim.h>
#include <soc/iproc.h>
#endif

#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/dnxc_cmic.h>
#endif

#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#endif

#include <soc/phyreg.h>
#include <soc/linkctrl.h>

#ifdef BCM_CMICX_SUPPORT

/* determine if device supports iProc15 */
int soc_is_miim_12r(int unit)
{
    int is_miim_12r = 0;

    /* iProc 15 devices support 12 MDIO rings */
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_HELIX5(unit) ||
        (SOC_IS_HURRICANE4(unit) || soc_feature(unit, soc_feature_iproc_15))
        || SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
        is_miim_12r = 1;
    }

    return is_miim_12r;
}

uint32 soc_cmicx_miim_cycle_type_get(int unit, int is_write, int clause45, uint32 phy_devad)
{
    uint32 cycle_type = 0;
    int is_miim_12r = 0;

    /* iProc 15 devices support 12 MDIO rings */
    if (soc_is_miim_12r(unit)) {
        is_miim_12r = 1;
    }

    if (clause45) {
        if(is_write) {
            if (phy_devad & MIIM_CYCLE_C45_WR) {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_REG_WR : MIIM_CYCLE_C45_REG_WR;
            } else if (phy_devad & MIIM_CYCLE_C45_WR_AD) {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_REG_AD : MIIM_CYCLE_C45_REG_AD;
            } else {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_AUTO_WR : MIIM_CYCLE_AUTO;
            }
        } else {
            if (phy_devad & MIIM_CYCLE_C45_RD) {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_REG_RD : MIIM_CYCLE_C45_REG_RD;
            } else if (phy_devad & MIIM_CYCLE_C45_RD_ADINC) {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_REG_RD_ADINC : MIIM_CYCLE_C45_REG_RD_ADINC;
            } else {
                cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C45_AUTO_RD : MIIM_CYCLE_AUTO;
            }
        }
    } else {
        /* clause 22 */
        if (is_write) {
            cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C22_REG_WR : MIIM_CYCLE_C22_REG_WR;
        } else {
            cycle_type = is_miim_12r ? MIIM_CYCLE_12R_C22_REG_RD : MIIM_CYCLE_C22_REG_RD;
        }
    }

    LOG_DEBUG(BSL_LS_SOC_MIIM, (BSL_META_U(unit, "is_miim_12r %d, cycle_type %d\n"), is_miim_12r, cycle_type));

    return cycle_type;
}

/** see .h file   */
int
soc_cmicx_miim_init(int unit)
{
    uint32 reg_val;

    /* Give MDIO control to IPROC */
    READ_MIIM_COMMON_CONTROLr(unit, &reg_val);
    soc_reg_field_set(unit, MIIM_COMMON_CONTROLr, &reg_val, OVRD_EXT_MDIO_MSTR_CNTRLf, 1);
    WRITE_MIIM_COMMON_CONTROLr(unit, reg_val);

    return SOC_E_NONE;
}

/**
 * \brief
 *   Function to get the correct register name and the register address
 *   in dependence of the ring that should be access
 *
 * \param [in] unit - Unit #
 * \param [in] ring_index - ring index to be access #
 * \param [out] ring_reg - ring register name #
 * \param [out] reg_addr - ring register address #
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
soc_cmicx_miim_ring_reg_addr_get(int unit, int ring_index, soc_reg_t * ring_reg, uint32 * reg_addr)
{
    soc_reg_t ring_control_8r_reg[] = {MIIM_RING0_CONTROLr,
                                       MIIM_RING1_CONTROLr,
                                       MIIM_RING2_CONTROLr,
                                       MIIM_RING3_CONTROLr,
                                       MIIM_RING4_CONTROLr,
                                       MIIM_RING5_CONTROLr,
                                       MIIM_RING6_CONTROLr,
                                       MIIM_RING7_CONTROLr,
                                       INVALIDr};

    soc_reg_t ring_control_12r_reg[] = {MIIM_RING0_CONTROLr,
                                        MIIM_RING1_CONTROLr,
                                        MIIM_RING2_CONTROLr,
                                        MIIM_RING3_CONTROLr,
                                        MIIM_RING4_CONTROLr,
                                        MIIM_RING5_CONTROLr,
                                        MIIM_RING6_CONTROLr,
                                        MIIM_RING7_CONTROLr,
                                        MIIM_RING8_CONTROLr,
                                        MIIM_RING9_CONTROLr,
                                        MIIM_RING10_CONTROLr,
                                        MIIM_RING11_CONTROLr,
                                        INVALIDr};
    soc_reg_t *ring_control_reg = NULL;

    int ring_idx_end = 0;

    /* determine mdio ring end based on if this is iProc14 of iProc15 device */
    ring_idx_end = soc_is_miim_12r(unit) ? CMICX_MIIM_12R_RING_INDEX_END : CMICX_MIIM_RING_INDEX_END;

    if ((ring_index < CMICX_MIIM_RING_INDEX_START) ||
        (ring_index > ring_idx_end)) {
        LOG_ERROR(BSL_LS_SOC_MIIM, (BSL_META_U(unit, "Invalid CMICx MIIM Ring index %d\n"), ring_index));
        return SOC_E_PARAM;
    }

    ring_control_reg = soc_is_miim_12r(unit) ? ring_control_12r_reg : ring_control_8r_reg;

    *ring_reg = ring_control_reg[ring_index];

    *reg_addr = soc_reg_addr(unit, *ring_reg, REG_PORT_ANY, 0);

    return SOC_E_NONE;
}

/**
 * \brief
 *   Function to get the divisor delay of the given ring
 *
 * \param [in] unit - Unit #
 * \param [in] ring_index - ring index to be access #
 * \param [out] out_delay - ring out divisor delay value #
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
soc_cmicx_miim_divider_ring_delay_set(int unit, int ring_index, int out_delay)
{
    uint32 reg_val, reg_addr;
    soc_reg_t ring_reg;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(soc_cmicx_miim_ring_reg_addr_get(unit, ring_index, &ring_reg, &reg_addr));

    soc_iproc_getreg(unit, reg_addr, &reg_val);

    soc_reg_field_set(unit, ring_reg, &reg_val, MDIO_OUT_DELAYf, out_delay);

    soc_iproc_setreg(unit, reg_addr, reg_val);

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief
 *   Function to set the divisor delay of the given ring
 *
 * \param [in] unit - Unit #
 * \param [in] ring_index - ring index to be access #
 * \param [int] out_delay - ring out divisor delay value #
 *
 * \return
 *     err - see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
soc_cmicx_miim_divider_ring_delay_get(int unit, int ring_index, int * out_delay)
{
    uint32 reg_val, reg_addr;
    soc_reg_t ring_reg;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(soc_cmicx_miim_ring_reg_addr_get(unit, ring_index, &ring_reg, &reg_addr));

    soc_iproc_getreg(unit, reg_addr, &reg_val);

    *out_delay = soc_reg_field_get(unit, ring_reg, reg_val, MDIO_OUT_DELAYf);

exit:
    SOC_FUNC_RETURN;
}

/** see .h file   */
int
soc_cmicx_miim_divider_set_ring(int unit, int ring_index, int int_divider, int ext_divider, int out_delay)
{
    uint32 reg_val, reg_addr;
    soc_reg_t ring_reg;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(soc_cmicx_miim_ring_reg_addr_get(unit, ring_index, &ring_reg, &reg_addr));

    soc_iproc_getreg(unit, reg_addr, &reg_val);

    if (int_divider != -1) {
        soc_reg_field_set(unit, ring_reg, &reg_val, CLOCK_DIVIDER_INTf, int_divider);
    }

    if (ext_divider != -1) {
        soc_reg_field_set(unit, ring_reg, &reg_val, CLOCK_DIVIDER_EXTf, ext_divider);
    }

    if (out_delay != -1) {
        soc_reg_field_set(unit, ring_reg, &reg_val, MDIO_OUT_DELAYf, out_delay);
    }

    soc_iproc_setreg(unit, reg_addr, reg_val);

exit:
    SOC_FUNC_RETURN;
}

/** see .h file   */
int
soc_cmicx_miim_divider_set_all(int unit, int int_divider, int ext_divider, int out_delay)
{
    int ring_idx = 0;
    int ring_idx_end = 0;

    /* determine mdio ring end based on if this is iProc14 of iProc15 device */
    ring_idx_end = soc_is_miim_12r(unit) ? CMICX_MIIM_12R_RING_INDEX_END : CMICX_MIIM_RING_INDEX_END;

    for (ring_idx = CMICX_MIIM_RING_INDEX_START; ring_idx <= ring_idx_end; ring_idx++) {
        soc_cmicx_miim_divider_set_ring(unit, ring_idx, int_divider, ext_divider, out_delay);
    }

    return SOC_E_NONE;
}

/** see .h file   */
int
soc_cmicx_miim_operation(int unit, int is_write, int clause, uint32 phy_id, uint32 phy_reg_addr, uint16 *phy_data)
{
    uint32 reg_val = 0, internal_select, real_phy_id, real_bus_id, is_done, is_error;
    uint32 devad = 0;
    soc_timeout_t to;
    int clause45 = 0;
    uint32 cycle_type = 0, phy_devad = 0;
    uint32 is_broadcast, len, max_val;
#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    int out_delay=0, new_out_delay = 0, int_divisor, ext_divisor, int_delay, ext_delay;
#endif
    SOC_INIT_FUNC_DEFS;

#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        if (is_write && dnxc_ha_is_access_disabled(unit,UTILEX_SEQ_ALLOW_SCHAN))
        {
            LOG_WARN(BSL_LS_SOC_MIIM,
                (BSL_META_U(unit,
                                "soc_cmicx_miim_operation: ERROR, It is not currently allowed to modify HW\n")));
            _SOC_IF_ERR_EXIT(SOC_E_FAIL);
        }
        soc_dnxc_data_mdio_config_get(unit, &int_divisor, &ext_divisor, &int_delay, &ext_delay);
    }
#endif

#if defined(BCM_DNX_SUPPORT) && defined(DNX_ERR_RECOVERY_VALIDATION)
    
    if (SOC_IS_DNX(unit)) {
        if (is_write && !DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
        {
            LOG_WARN(BSL_LS_SOC_MIIM,
                (BSL_META_U(unit,
                                "soc_cmicx_miim_operation: ERROR, Invalid error recovery hw access\n")));
            _SOC_IF_ERR_EXIT(SOC_E_FAIL);
        }
    }
#endif

    clause45 = (SOC_CLAUSE_45 == clause);

    /* parse phy_id to bus, phy, and internal indication
    *   phy_id encoding:
    *       bit7, 1: internal MDIO bus, 0: external MDIO bus
    *       bit10, broadcast mode
    *       bit9,8,6,5, mdio bus number
    *       bit4-0,   mdio addresses
    */
    internal_select = PHY_ID_INTERNAL(phy_id);
    real_phy_id = PHY_ID_ADDR(phy_id);
    real_bus_id = PHY_ID_BUS_NUM(phy_id);
    is_broadcast = PHY_ID_BROADCAST(phy_id);

    MIIM_LOCK(unit);

#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        /**
         * Workaround to artificially support different divider delays for INTERNAL and EXTERNAL DIVISORs
         * Each access update the delay according to the access type
         */
        _SOC_IF_ERR_EXIT(soc_cmicx_miim_divider_ring_delay_get(unit, real_bus_id, &out_delay));
        new_out_delay = internal_select ? int_delay : ext_delay;
        _SOC_IF_ERR_EXIT(soc_cmicx_miim_divider_ring_delay_set(unit, real_bus_id, new_out_delay));
    }
#endif

    /* preper transaction data */
    /* _SOC_IF_ERR_EXIT(READ_MIIM_CH0_ADDRESSr(unit, &reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_getreg(unit, soc_reg_addr(unit, MIIM_CH0_ADDRESSr, REG_PORT_ANY, 0), &reg_val));
    soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, PHY_IDf, real_phy_id);

    if (clause45) {
        /* cleanup of devad flags */
        devad = SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr) & 0x1f;

        soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_45_REG_ADRRf, SOC_PHY_CLAUSE45_REGAD(phy_reg_addr));

        /* IPROC 15 and higher devices */
        if (soc_is_miim_12r(unit)) {
            soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_22_REGADRR_OR_45_DTYPEf, devad);
        } else {
            soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_45_DTYPEf, devad);
        }
    } else {
        /* IPROC 15 and higher devices */
        if (soc_is_miim_12r(unit)) {
            soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_22_REGADRR_OR_45_DTYPEf, (phy_reg_addr & 0x1f));
        } else {
            soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_22_REG_ADRRf, (phy_reg_addr & 0x1f));
        }
    }
    /* _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_ADDRESSr(unit, reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_setreg(unit, soc_reg_addr(unit, MIIM_CH0_ADDRESSr, REG_PORT_ANY, 0), reg_val));

    if (clause45){
        LOG_VERBOSE(BSL_LS_SOC_MIIM,
                 (BSL_META_U(unit,
                             "soc_cmicx_miim_operation cl45 %s%s: "
                             "(id=0x%02x readr=0x%x dtype=0x%x data=0x%04x clause45=%d, real_phy_id=%d, real_bus_id=%d)\n"),
                  is_broadcast ? "broadcast " : "", is_write ? "write" : "read ", phy_id, SOC_PHY_CLAUSE45_REGAD(phy_reg_addr),
				  SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr), *phy_data, clause45, real_phy_id,real_bus_id));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_MIIM,
                 (BSL_META_U(unit,
                             "soc_cmicx_miim_operation cl22 %s%s: "
                             "(id=0x%02x addr=0x%02x data=0x%04x clause45=%d, real_phy_id=%d, real_bus_id=%d)\n"),
                  is_broadcast ? "broadcast " : "", is_write ? "write" : "read ", phy_id, phy_reg_addr,
				  *phy_data, clause45,real_phy_id,real_bus_id));
    }

    /* _SOC_IF_ERR_EXIT(READ_MIIM_CH0_PARAMSr(unit, &reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_getreg(unit, soc_reg_addr(unit, MIIM_CH0_PARAMSr, REG_PORT_ANY, 0), &reg_val));
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, SEL_INT_PHYf, internal_select);
    if (is_write && is_broadcast) {
        len = soc_reg_field_length(unit, MIIM_CH0_PARAMSr, RING_MAPf);
        max_val = (1 << len) - 1;
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, RING_MAPf, max_val);
    } else {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, RING_MAPf, (1 << real_bus_id));
    }

    /* Only applicable for iProc 14 devices */
    if (!soc_is_miim_12r(unit)) {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, SEL_CL45f, clause45);
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, RD_ENf, is_write ? 0 : 1);
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, WR_ENf, is_write ? 1 : 0);
    }

    /*
     * Configure correct MDIO CYCLE type depending on if this data only mode
     * without corresponding address cycle for MDIO transcation.
     */
    phy_devad = phy_reg_addr >> 16;
    cycle_type = soc_cmicx_miim_cycle_type_get(unit, is_write, clause45, phy_devad);

    /* IPROC 15 and higher devices */
    if (soc_is_miim_12r(unit)) {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, MDIO_OP_TYPEf, cycle_type);
    } else {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, MDIO_CYCLEf, cycle_type);
    }

    if(is_write) {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, PHY_WR_DATAf, *phy_data);
    } else {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, PHY_WR_DATAf, 0x0);
    }

    /* _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_PARAMSr(unit, reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_setreg(unit, soc_reg_addr(unit, MIIM_CH0_PARAMSr, REG_PORT_ANY, 0), reg_val));

#if (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        if(!internal_select && out_delay != new_out_delay)
        {
            sal_usleep(100);
        }
    }
#endif
    /* start transaction */
    reg_val = 0;
    soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x1);
    /* _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_CONTROLr(unit, reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_setreg(unit, soc_reg_addr(unit, MIIM_CH0_CONTROLr, REG_PORT_ANY, 0), reg_val));

    /* poll for DONE bit*/
    soc_timeout_init(&to, SOC_CONTROL(unit)->miimTimeout, 2500);

    while (1){
        /* _SOC_IF_ERR_EXIT(READ_MIIM_CH0_STATUSr(unit, &reg_val)); */
        _SOC_IF_ERR_EXIT(soc_iproc_getreg(unit, soc_reg_addr(unit, MIIM_CH0_STATUSr, REG_PORT_ANY, 0), &reg_val));
        is_done = soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, DONEf);
        if (is_done) {
            break; /* MIIM operation is done */
        }

        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_MIIM,
                      (BSL_META_U(unit,
                                  "MiimTimeOut: %s%s, "
                                  "timeout (id=0x%02x addr=0x%02x)\n"),
                                    is_broadcast ? "broadcast " : "", is_write ? "write" : "read" ,
                                    phy_id, phy_reg_addr));
            _SOC_IF_ERR_EXIT(SOC_E_TIMEOUT);
        }
    }

    LOG_DEBUG(BSL_LS_SOC_MIIM,
                (BSL_META_U(unit,
                            "  Done in %d polls\n"), to.polls));

    /* check for transaction error */
    is_error = soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, ERRORf);
    if (is_error) {
        LOG_ERROR(BSL_LS_SOC_MIIM,
                    (BSL_META_U(unit,
                                "  MDIO transaction Error\n")));
        _SOC_IF_ERR_EXIT(SOC_E_FAIL);
    }

    /* in case of read - get data */
    if (!is_write) {
        *phy_data = (uint16)soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, PHY_RD_DATAf);
        LOG_VERBOSE(BSL_LS_SOC_MIIM,
                     (BSL_META_U(unit,
                                 "soc_cmicx_miim_operation read data: %d \n"),*phy_data));
    }

    /* cleanup */
    reg_val = 0;
    soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x0);
    /* _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_CONTROLr(unit, reg_val)); */
    _SOC_IF_ERR_EXIT(soc_iproc_setreg(unit, soc_reg_addr(unit, MIIM_CH0_CONTROLr, REG_PORT_ANY, 0), reg_val));

exit:
    /* in case we failed - try to cleanup */
    if (SOC_FUNC_ERROR) {
        reg_val = 0;
        soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x0);
        /* WRITE_MIIM_CH0_CONTROLr(unit, reg_val); */ /* no need to catch error in case of failrue */
        _SOC_IF_ERR_EXIT(soc_iproc_setreg(unit, soc_reg_addr(unit, MIIM_CH0_CONTROLr, REG_PORT_ANY, 0), reg_val));
    }

    /* Release linkscan pause and MIIM lock */
    MIIM_UNLOCK(unit);

    SOC_FUNC_RETURN;
}

#endif /* BCM_CMICX_SUPPORT  */
