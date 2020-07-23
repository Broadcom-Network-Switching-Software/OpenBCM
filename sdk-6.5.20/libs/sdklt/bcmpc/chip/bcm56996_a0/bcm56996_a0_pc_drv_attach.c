/*! \file bcm56996_a0_pc_drv_attach.c
 *
 * Chip stub for BCMPC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <phymod/phymod.h>
#include <phymod/phymod_sim_control.h>

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_cmicx_pmac.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pm_internal.h>

#include <bcmbd/chip/bcm56996_a0_acc.h>
#include <bcmpmac/chip/bcm56996_a0_pmac_drv_list.h>
#include <bcmpmac/chip/bcmpmac_common_cdmac_defs.h>
#include <bcmpmac/chip/bcmpmac_common_xlmac_defs.h>
#include <bcmbd/chip/bcm56996_a0_port_intr.h>


#include "bcm56996_a0_pc_internal.h"
#include "bcm56996_a0_pc_miim.h"



#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif



/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*
 * BCM56996 has 16 pipelines, each has 17 general ports.
 * 256 (Front panel ports) + 1 (CPU port) + 2 (management ports) +
 * 8 (loopback ports) + 5 (Spare) = 272.
 */
#define BCM56996_MAX_LPORTS 272
/*
 * 256 (64(PM8X50) * 4) + 2 (1 * PM4X10) +
 * 1 CPU port + 8 (Internal Loopback ports).
 */
#define BCM56996_MAX_PPORTS 267

typedef enum th4g_pm_type_e {
    TH4G_PM_TYPE_CPU,
    TH4G_PM_TYPE_LB,
    TH4G_PM_TYPE_PM4X10,
    TH4G_PM_TYPE_PM8X100
} th4g_pm_type_t;

typedef struct pm_lane_layout_s {
    int lane_map[4];
} pm_lane_layout_t;

#define TSC_REG_ADDR_TSCID_SET(_reg_addr, _phyad)    \
            ((_reg_addr) |= ((_phyad) & 0x1f) << 19)


#define TSC_REG_PMD_DEVICE 0x08000000

/*******************************************************************************
 * TSC Bus Driver
 */

/*!
 * \brief Register write function for TSCE.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsce_reg_write(void *user_acc, uint32_t phy_addr,
                           uint32_t reg_addr, uint32_t val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[1] = ((val & 0xffff) << 16) |
              ((~val & 0xffff0000) >> 16);
    data[2] = 1; /* for TSC register write */

    for (idx = 0; idx < 4; idx++) {
        XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, idx, data[idx]);
    }
    rv = WRITE_XLPORT_WC_UCMEM_DATAm(unit, 0, xl_ucmem_data, port);

    return rv;
}

/*!
 * \brief Register read function for TSCE.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsce_reg_read(void *user_acc, uint32_t phy_addr,
                          uint32_t reg_addr, uint32_t *val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0; /* for TSC register READ */

    for (idx = 0; idx < 4; idx++) {
        XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, idx, data[idx]);
    }
    rv = WRITE_XLPORT_WC_UCMEM_DATAm(unit, 0, xl_ucmem_data, port);

    if (SHR_SUCCESS(rv)) {
        rv = READ_XLPORT_WC_UCMEM_DATAm(unit, 0, &xl_ucmem_data, port);
    }

    /*
     * The read data returned will be stored in bits [47:32] of
     * XLPORT_WC_UCMEM_DATA.
     */
    *val = XLPORT_WC_UCMEM_DATAm_GET(xl_ucmem_data, 1);

    return rv;
}

/*!
 * \brief Register write function for TSCO.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_reg_write(void *user_acc, uint32_t phy_addr,
                            uint32_t reg_addr, uint32_t val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    CDPORT_TSC_UCMEM_DATAm_t cd_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;


    /* for TH4G, PMD register can only be access throguh PML0
     * need to check if register is PMD, if so then physical need to
     * changed to use PML0
     */
    if (reg_addr & TSC_REG_PMD_DEVICE) {
        port = ((port - 1) / 8) * 8 + 1;
    }

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[1] = ((val & 0xffff) << 16) |
              ((~val & 0xffff0000) >> 16);
    data[2] = 1; /* for TSC register write */

    for (idx = 0; idx < 4; idx++) {
        CDPORT_TSC_UCMEM_DATAm_SET(cd_ucmem_data, idx, data[idx]);
    }
    rv = WRITE_CDPORT_TSC_UCMEM_DATAm(unit, 0, cd_ucmem_data, port);

    return rv;
}

/*!
 * \brief Register read function for TSCO.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param phy_addr Only 5 bits of PHY id used in case of sbus.
 * \param reg_addr The register addrress
 * \param val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_reg_read(void *user_acc, uint32_t phy_addr,
                           uint32_t reg_addr, uint32_t *val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    CDPORT_TSC_UCMEM_DATAm_t cd_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

    /* for TH4G, PMD register can only be access throguh PML0
     * need to check if register is PMD, if so then physical need to
     * changed to use PML0
     */
    if (reg_addr & TSC_REG_PMD_DEVICE) {
        port = ((port - 1) / 8) * 8 + 1;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0; /* for TSC register READ */

    for (idx = 0; idx < 4; idx++) {
        CDPORT_TSC_UCMEM_DATAm_SET(cd_ucmem_data, idx, data[idx]);
    }
    rv = WRITE_CDPORT_TSC_UCMEM_DATAm(unit, 0, cd_ucmem_data, port);

    if (SHR_SUCCESS(rv)) {
        rv = READ_CDPORT_TSC_UCMEM_DATAm(unit, 0, &cd_ucmem_data, port);
    }

    /*
     * The read data returned will be stored in bits [63:48] of
     * CDPORT_TSC_UCMEM_DATA.
     */
    *val = (CDPORT_TSC_UCMEM_DATAm_GET(cd_ucmem_data, 1) >> 16) & 0xffff;

    return rv;
}

static int
bcm56996_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport)
{
    /*
     * For PM8x100, use indirect acessing scheme.
     * Every 16-PMs uses one internal bus. Each PM use
     * one port on the chain.
     *
     * And the management ports (PM4x10) use the internal bus 11.
     * Direct accessing scheme is used on PM4x10.
     *
     * Internal phy address:
     * Bus  0: PHYID 1-16 are mapped to physical ports 1-64
     * Bus  1: PHYID 1-16 are mapped to physical ports 65-128
     * Bus  2: PHYID 1-16 are mapped to physical ports 129-192
     * Bus  3: PHYID 1-16 are mapped to physical ports 193-256
     * Bus 11: PHYID 1 and 3 are mapped to physical ports 257-258
     */

    uint32_t ibus = 0, offset = 0;
    if (pport > 258 || pport == CMIC_PORT) {
        return -1;
    }

    if (pport >= 257) {
        ibus = 11;
        /* Each PM4x10 (4 ports) could share one MDIO address. */
        return (1 + BCM56996_A0_PC_MIIM_IBUS(ibus));
    } else {
        /* Every continous 64 PM8x50 ports use one bus. */
        ibus = (pport - 1) / 64;
        offset = ibus * 64 + 1;
        /* Each PM8x50, 4 ports share one MDIO address. */
        return ((pport - offset) / 4 + 1 + BCM56996_A0_PC_MIIM_IBUS(ibus));
    }


}

/*!
 * \brief Register read function for TSCE.
 * This function is used to read phy register before phymod driver attached.
 * \param unit Unit number.
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress.
 * \param val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsce_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = XLPORT_WC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_XLPORT;
    uint32_t phy_addr;
    phymod_bus_t *phymod_sim_bus;

    sal_memset(data, 0, sizeof(data));

    phy_addr = (uint32_t)bcm56996_a0_pc_phy_addr_get(unit, base_port);
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = phymod_sim_bus->read(NULL, phy_addr, reg_addr, val);
        return rv;
    }

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);
    data[0] = reg_addr & 0xffffffff ;
    data[2] = 0;

    rv = bcmbd_cmicx_mem_blocks_write(unit, blkacc, base_port,
                                 offset, 0, data, 4);
    if (SHR_SUCCESS(rv)) {
        rv = bcmbd_cmicx_mem_blocks_read(unit, blkacc, base_port,
                                offset, 0, data, 4);
        *val = data[1] & 0xffff;
    }

    return rv;
}

/*!
 * \brief Register write function for TSCE.
 * This function is used to write to register before phymod driver attached.
 *
 * \param unit Unit number
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsce_raw_reg_write(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = XLPORT_WC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_XLPORT;
    uint32_t phy_addr;
    phymod_bus_t *phymod_sim_bus;

    sal_memset(data, 0, sizeof(data));

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    phy_addr = (uint32_t)bcm56996_a0_pc_phy_addr_get(unit, base_port);

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = phymod_sim_bus->write(NULL, phy_addr, reg_addr, val);
        return rv;
    }

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);
    data[0] = reg_addr & 0xffffffff ;
    data[1] = ((val & 0xffff) << 16) |
              ((~val & 0xffff0000) >> 16);
    data[2] = 1;

    rv = bcmbd_cmicx_mem_blocks_write(unit, blkacc, base_port,
                                 offset, 0, data, 4);

    return rv;
}

/*!
 * \brief Register read function for TSCO.
 * This function is used to read phy register before phymod driver attached.
 * \param unit Unit number.
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress.
 * \param val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = CDPORT_TSC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_CDPORT;
    phymod_bus_t *phymod_sim_bus;
    uint32_t phy_addr;

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        phy_addr = (uint32_t)bcm56996_a0_pc_phy_addr_get(unit, base_port);
        rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = phymod_sim_bus->read(NULL, phy_addr, reg_addr, val);
        return rv;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0;

    rv = bcmbd_cmicx_mem_blocks_write(unit, blkacc, base_port,
                                 offset, 0, data, 4);
    if (SHR_SUCCESS(rv)) {
        rv = bcmbd_cmicx_mem_blocks_read(unit, blkacc, base_port,
                                offset, 0, data, 4);
        *val = (data[1] >> 16) & 0xffff;
    }

    return rv;
}

/*!
 * \brief Register write function for TSCO.
 * This function is used to write to register before phymod driver attached.
 *
 * \param unit Unit number.
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress.
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_raw_reg_write(int unit, bcmpc_pport_t base_port,
                                uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = CDPORT_TSC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_CDPORT;
    phymod_bus_t *phymod_sim_bus;
    uint32_t phy_addr;

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        phy_addr = (uint32_t)bcm56996_a0_pc_phy_addr_get(unit, base_port);
        rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        rv = phymod_sim_bus->write(NULL, phy_addr, reg_addr, val);
        return rv;
    }

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[1] = ((val & 0xffff) << 16) |
              ((~val & 0xffff0000) >> 16);
    data[2] = 1;

    rv = bcmbd_cmicx_mem_blocks_write(unit, blkacc, base_port,
                                 offset, 0, data, 4);

    return rv;
}


/*!
 * \brief Raw register read function.
 * This function is used to read phy register before phymod driver attached.
 * \param unit Unit number.
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress.
 * \param val The value read from the register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
       SHR_IF_ERR_EXIT
            (bcm56996_a0_tsce_raw_reg_read(unit, base_port,  reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tsco_raw_reg_read(unit, base_port,  reg_addr, val));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Raw register write function.
 * This function is used to write to register before phymod driver attached.
 *
 * \param unit Unit number.
 * \param base_port Base port of the PM.
 * \param reg_addr The register addrress.
 * \param val The value to write to register. 16 bits value + 16 bits mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_raw_reg_write(int unit, bcmpc_pport_t base_port,
                          uint32_t reg_addr, uint32_t val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tsce_raw_reg_write(unit, base_port, reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tsco_raw_reg_write(unit, base_port, reg_addr, val));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function will adjust the core_addr before
 * calling sim_bus_read if the reg_addr belongs to PMD.
 */
static int
bcm56996_a0_physim_tsco_reg_read(void *user_acc, uint32_t core_addr,
                                 uint32_t reg_addr, uint32_t *val)
{
    bcmpc_pport_t pport;
    bcmpc_phy_access_data_t *user_data = user_acc;
    uint32_t core_addr_adjust = core_addr;
    int rv = SHR_E_NONE;
    uint16_t tmp = 0;

    if (user_acc && (reg_addr & TSC_REG_PMD_DEVICE)) {
        pport = ((user_data->pport - 1) / 8) * 8 + 1;
        core_addr_adjust = (uint32_t)bcm56996_a0_pc_phy_addr_get(0, pport);
    } else if (user_acc) {
        core_addr_adjust = (uint32_t)bcm56996_a0_pc_phy_addr_get(0,
                                                             user_data->pport);
    }

    *val = 0;
    rv = phymod_control_physim_read(0, core_addr_adjust, reg_addr, &tmp);
    *val = tmp;

    return rv;
}

/*
 * This function will adjust the core_addr before
 * calling sim_bus_write if the reg_addr belongs to PMD.
 */
static int
bcm56996_a0_physim_tsco_reg_write(void *user_acc, uint32_t core_addr,
                                  uint32_t reg_addr, uint32_t val)
{
    bcmpc_pport_t pport;
    bcmpc_phy_access_data_t *user_data = user_acc;
    uint32_t core_addr_adjust = core_addr;
    int rv = SHR_E_NONE;
    uint16_t tmp = 0;
    uint16_t msk = 0;

    if (user_acc && (reg_addr & TSC_REG_PMD_DEVICE)) {
        pport = ((user_data->pport - 1) / 8) * 8 + 1;
        core_addr_adjust = (uint32_t)bcm56996_a0_pc_phy_addr_get(0, pport);
    } else if (user_acc) {
        core_addr_adjust = (uint32_t)bcm56996_a0_pc_phy_addr_get(0,
                                                            user_data->pport);
    }

    tmp = (val & 0xFFFF);
    msk = ((val >> 16) & 0xFFFF);

    rv = phymod_control_physim_wrmask(0, core_addr_adjust, reg_addr, tmp, msk);


    return rv;
}


/*!
 * \brief Memory read function for TSCO.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param mem_type TSC external memory type.
 * \param idx Entry index in the memory to read.
 * \param val The value read from the memory.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_mem_read(void *user_acc, phymod_mem_type_t mem_type,
                           uint32_t idx, uint32_t *val)
{
    int rv, unit, port, vdx, wsize;
    bcmpc_phy_access_data_t *user_data = user_acc;
    SPEED_ID_TABLEm_t spd_id;
    SPEED_PRIORITY_MAP_TBLm_t spd_pri;
    AM_TABLEm_t am;
    UM_TABLEm_t um;
    
    TX_LKUP_1588_MEMm_t tx_lkup;
    RX_LKUP_1588_MEM_MPP0m_t rx_lkup_mpp0;
    RX_LKUP_1588_MEM_MPP1m_t rx_lkup_mpp1;

    unit = user_data->unit;
    port = user_data->pport;

    switch (mem_type) {
        case phymodMemSpeedIdTable:
            wsize = SHR_BYTES2WORDS(SPEED_ID_TABLEm_SIZE);
            rv = READ_SPEED_ID_TABLEm(unit, idx, &spd_id, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = SPEED_ID_TABLEm_GET(spd_id, vdx);
            }
            break;
        case phymodMemSpeedPriorityMapTable:
            wsize = SHR_BYTES2WORDS(SPEED_PRIORITY_MAP_TBLm_SIZE);
            rv = READ_SPEED_PRIORITY_MAP_TBLm(unit, idx, &spd_pri, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = SPEED_PRIORITY_MAP_TBLm_GET(spd_pri, vdx);
            }
            break;
        case phymodMemAMTable:
            wsize = SHR_BYTES2WORDS(AM_TABLEm_SIZE);
            rv = READ_AM_TABLEm(unit, idx, &am, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = AM_TABLEm_GET(am, vdx);
            }
            break;
        case phymodMemUMTable:
            wsize = SHR_BYTES2WORDS(UM_TABLEm_SIZE);
            rv = READ_UM_TABLEm(unit, idx, &um, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = UM_TABLEm_GET(um, vdx);
            }
            break;
        case phymodMemTxLkup1588Mpp0:
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEMm_SIZE);
            rv = READ_TX_LKUP_1588_MEMm(unit, idx, &tx_lkup, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = TX_LKUP_1588_MEMm_GET(tx_lkup, vdx);
            }
            break;
        case phymodMemRxLkup1588Mpp0:
            wsize = SHR_BYTES2WORDS(RX_LKUP_1588_MEM_MPP0m_SIZE);
            rv = READ_RX_LKUP_1588_MEM_MPP0m(unit, idx, &rx_lkup_mpp0, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = RX_LKUP_1588_MEM_MPP0m_GET(rx_lkup_mpp0, vdx);
            }
            break;
        case phymodMemRxLkup1588Mpp1:
            wsize = SHR_BYTES2WORDS(RX_LKUP_1588_MEM_MPP1m_SIZE);
            rv = READ_RX_LKUP_1588_MEM_MPP1m(unit, idx, &rx_lkup_mpp1, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = RX_LKUP_1588_MEM_MPP1m_GET(rx_lkup_mpp1, vdx);
            }
            break;
        default:
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Memory type [%d] is not supported.\n"),
                      mem_type));
            return SHR_E_FAIL;
    }

    return rv;
}

/*!
 * \brief Memory read function for TSCO.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 * \param mem_type TSC external memory type.
 * \param idx Entry index in the memory to read.
 * \param val The value read from the memory.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56996_a0_tsco_mem_write(void *user_acc, phymod_mem_type_t mem_type,
                            uint32_t idx, const uint32_t *val)
{
    int rv, unit, port, vdx, wsize;
    bcmpc_phy_access_data_t *user_data = user_acc;
    SPEED_ID_TABLEm_t spd_id;
    SPEED_PRIORITY_MAP_TBLm_t spd_pri;
    AM_TABLEm_t am;
    UM_TABLEm_t um;
    TX_LKUP_1588_MEMm_t tx_lkup;
    RX_LKUP_1588_MEM_MPP0m_t rx_lkup_mpp0;
    RX_LKUP_1588_MEM_MPP1m_t rx_lkup_mpp1;

    unit = user_data->unit;
    port = user_data->pport;

    switch (mem_type) {
        case phymodMemSpeedIdTable:
            wsize = SHR_BYTES2WORDS(SPEED_ID_TABLEm_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                SPEED_ID_TABLEm_SET(spd_id, vdx, val[vdx]);
            }
            rv = WRITE_SPEED_ID_TABLEm(unit, idx, spd_id, port);
            break;
        case phymodMemSpeedPriorityMapTable:
            wsize = SHR_BYTES2WORDS(SPEED_PRIORITY_MAP_TBLm_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                SPEED_PRIORITY_MAP_TBLm_SET(spd_pri, vdx, val[vdx]);
            }
            rv = WRITE_SPEED_PRIORITY_MAP_TBLm(unit, idx, spd_pri, port);
            break;
        case phymodMemAMTable:
            wsize = SHR_BYTES2WORDS(AM_TABLEm_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                AM_TABLEm_SET(am, vdx, val[vdx]);
            }
            rv = WRITE_AM_TABLEm(unit, idx, am, port);
            break;
        case phymodMemUMTable:
            wsize = SHR_BYTES2WORDS(UM_TABLEm_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                UM_TABLEm_SET(um, vdx, val[vdx]);
            }
            rv = WRITE_UM_TABLEm(unit, idx, um, port);
            break;
        case phymodMemTxLkup1588Mpp0:
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEMm_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                TX_LKUP_1588_MEMm_SET(tx_lkup, vdx, val[vdx]);
            }
            rv = WRITE_TX_LKUP_1588_MEMm(unit, idx, tx_lkup, port);
            break;
        case phymodMemRxLkup1588Mpp0:
            wsize = SHR_BYTES2WORDS(RX_LKUP_1588_MEM_MPP0m_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                RX_LKUP_1588_MEM_MPP0m_SET(rx_lkup_mpp0, vdx, val[vdx]);
            }
            rv = WRITE_RX_LKUP_1588_MEM_MPP0m(unit, idx, rx_lkup_mpp0, port);
            break;
        case phymodMemRxLkup1588Mpp1:
            wsize = SHR_BYTES2WORDS(RX_LKUP_1588_MEM_MPP1m_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                RX_LKUP_1588_MEM_MPP1m_SET(rx_lkup_mpp1, vdx, val[vdx]);
            }
            rv = WRITE_RX_LKUP_1588_MEM_MPP1m(unit, idx, rx_lkup_mpp1, port);
            break;
        default:
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Memory type [%d] is not supported.\n"),
                      mem_type));
            return SHR_E_FAIL;
    }

    return rv;
}

static phymod_bus_t bcm56996_a0_tsce_bus = {
    .bus_name = "TSCE Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56996_a0_tsce_reg_read,
    .write = bcm56996_a0_tsce_reg_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

static phymod_bus_t bcm56996_a0_tsco_bus = {
    .bus_name = "TSCO Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56996_a0_tsco_reg_read,
    .write = bcm56996_a0_tsco_reg_write,
    .mem_read = bcm56996_a0_tsco_mem_read,
    .mem_write = bcm56996_a0_tsco_mem_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

static phymod_bus_t bcm56996_a0_tsco_physim_bus = {
    .bus_name = "sdklt_bcm56996_a0_tsco_physim_bus",
    .read = bcm56996_a0_physim_tsco_reg_read,
    .write = bcm56996_a0_physim_tsco_reg_write,
    .mem_read = bcm56996_a0_tsco_mem_read,
    .mem_write = bcm56996_a0_tsco_mem_write,
    .is_write_disabled = NULL,
    .mutex_take = NULL,
    .mutex_give = NULL,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

/*******************************************************************************
 * Device interrupt handlers.
 */
/*
 * This function is FDR interrupt handler and gets called in the
 * context of the BCMBD port secondary interrupt handlers.
 * The intr_param is packed with the device physical port
 * and the PC interrupt number (which is an enumeration of
 * the interrupt types supported by PC). It can be viewed
 * as and equivalent of the XXPORT_INTR_ENABLE register.
 * The device physical port is extracted using MACROs defined
 * in BCMBD.
 * MACROS to be used
 * PORT_INTR_INST_GET(intr_param) used to extract the physical
 * port from the intr_param. The physical port, tied to a Port Macro
 * can be used to retireve the PM ID.
 * PORT_INTR_PARAM_GET(intr_param) used to extract the PC interrupt
 * type enumeration.
 *
 * FDR interrupt handling notes.
 * FDR interrupt is triggered, when the "symbol error threshold" is
 * reaches or exceeds the configured value. An interrupt trigger should
 * follow with a FDR status/statistics read. These statistics are updated
 * to the read-only FDR_STATUS LT and the operational status is marked as
 * valid.
 */
static void
bcm56996_a0_pc_fdr_intr_func(int unit, uint32_t intr_param)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = PORT_INTR_INST_GET(intr_param);
    uint32_t intr_num = PORT_INTR_PARAM_GET(intr_param);

    switch (intr_num) {
        case CDPORT_INTR_FDR_INTERRUPT:
            /*
             * Process the FDR interrupt.
             * Read the FDR statistics and update FDR STATUS LT.
             */
            lport = bcmpc_pport_to_lport(unit, pport);
            if (lport == BCMPC_INVALID_LPORT) {
                LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                         "Invalid lport - FDR intr on physical port(%d)\n"),
                          pport));
            } else {
                rv = bcmpc_internal_port_fdr_stats_update(unit, lport, true);
                if (SHR_FAILURE(rv)) {
                    LOG_WARN(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "Failed to update FDR STATUS (%d)\n"), lport));
                }

                /* Clear interrupt in controller */
                rv = bcmbd_port_intr_clear(unit, intr_param);
                if (SHR_FAILURE(rv)) {
                    LOG_WARN(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "Failed to clear FDR interrupt(%d)\n"), lport));
                }
            }
            break;
        default:
            /* Unexpected interrupt */
            break;
    }

    return;
}

static int
bcm56996_a0_fdr_interrupt_control(int unit, int pport,
                                  bcmpc_intr_enable_t intr_op)
{
    uint32_t intr_num = CDPORT_INTR_FDR_INTERRUPT;
    SHR_FUNC_ENTER(unit);

    /* Enable FDR interrupt. */
    if (intr_op == INTR_ENABLE) {
        SHR_IF_ERR_EXIT
            (bcmpc_interrupt_enable(unit, pport, intr_num,
                                    bcm56996_a0_pc_fdr_intr_func,
                                    INTR_ENABLE));
    } else {
        /* Disable the FDR interript. */
        SHR_IF_ERR_EXIT
            (bcmpc_interrupt_enable(unit, pport, intr_num,
                                    NULL, INTR_DISABLE));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_fdr_port_enable_set(int unit, int pport, bool enable)
{
    uint32_t wsize;
    uint32_t port_arr_idx = 0;
    uint32_t port_pos = 0;
    uint32_t fdr_bmap = 0;
    TOP_PM_FDR_MEMm_t fdr_mem;
    SHR_FUNC_ENTER(unit);

    /*
     * Enable FDR on the physical port.
     * TOP_PM_FDR_MEM is a bitmap of all the physical ports
     * belonging to the Port Macros which support the FDR feature.
     * The index starts from bit position 0, which indicates
     * the physical port 1 (pport - 1).
     */
    TOP_PM_FDR_MEMm_CLR(fdr_mem);

    wsize = SHR_BYTES2WORDS(TOP_PM_FDR_MEMm_SIZE);

    /*
     * Get the port index in the physical port bmap array.
     * There are 8 words in the memory, each word is 4 bytes.
     * 1 Word = 4 x 8 = 32 bits,
     * Total bmap size (Number of Words * Word Size) 8 * 32 = 256.
     * To get the port index in the list,
     * (pport - 1) / (Number of Words), Array index in the bmap.
     * (pport - 1) % (Word Size).
     */
    port_arr_idx = ((pport - 1)/SHR_WORDS2BYTES(wsize));
    port_pos = ((pport - 1) % (SHR_WORDS2BYTES(1) * 8));

    /* Physical port array index cannot be great than the word size. */
    if (port_arr_idx >= wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (READ_TOP_PM_FDR_MEMm(unit, 0, 0, &fdr_mem));
    fdr_bmap = (TOP_PM_FDR_MEMm_GET(fdr_mem, port_arr_idx));

    if (enable) {
        fdr_bmap |= ((uint32_t)1 << port_pos);
    } else {
        fdr_bmap &= ~((uint32_t)1 << port_pos);
    }

    TOP_PM_FDR_MEMm_SET(fdr_mem, port_arr_idx, fdr_bmap);
    SHR_IF_ERR_EXIT
        (WRITE_TOP_PM_FDR_MEMm(unit, 0, 0, fdr_mem));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Device init and cleanup functions
 */


static int
bcm56996_a0_pm_type_create(int unit)
{
    bcmpc_topo_type_t tinfo;

    SHR_FUNC_ENTER(unit);

    /* Create PM4X10 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4G_PM_TYPE_PM4X10));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM4X10";
    tinfo.num_lanes = 4;
    tinfo.num_ports = 2;
    tinfo.num_plls = 1;
    tinfo.ref_clk = phymodRefClk156Mhz;
    tinfo.pm_drv = &bcmpc_pm_drv_common;
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_CORE_INIT_FULL);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_1588_EGRESS_TIMESTAMP);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_MGMT_PORT_MACRO);
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4G_PM_TYPE_PM4X10, &tinfo));

    /* Create PM8X100 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4G_PM_TYPE_PM8X100));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM8X100";
    tinfo.num_lanes = 8;
    tinfo.num_ports = 8;
    tinfo.num_plls = 1;
    tinfo.tvco_pll_index = 0;
    tinfo.ref_clk = phymodRefClk312Mhz;
    tinfo.pm_drv = &bcmpc_pm_drv_common;
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_CORE_INIT_SKIP_FW_LOAD);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_RLM);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_SW_STATE_UPDATE);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_FDR_SUPPORTED);
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4G_PM_TYPE_PM8X100, &tinfo));

    /* Create PM type for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4G_PM_TYPE_CPU));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_CPU";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4G_PM_TYPE_CPU, &tinfo));

    /* Create PM type for Loopback port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4G_PM_TYPE_LB));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_LOOPBACK";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4G_PM_TYPE_LB, &tinfo));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_pm_instance_create(int unit, bool warm)
{
    int pm_id = 0, inst_cnt;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    /* Create instance for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_create(unit, warm, pm_id));
    bcmpc_topo_t_init(&pm_info);
    pm_info.tid = TH4G_PM_TYPE_CPU;
    pm_info.base_pport = CMIC_PORT;
    pm_info.is_active = TRUE;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_set(unit, pm_id, &pm_info));
    pm_id++;

    /* Create instances for CDPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_CDPORT, TH4G_PM_TYPE_PM8X100,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for XLPORT/MGMT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_XLPORT, TH4G_PM_TYPE_PM4X10,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for LBPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_LBPORT, TH4G_PM_TYPE_LB,
                                     warm, pm_id, &inst_cnt));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_type_destroy(int unit, int tid, bcmpc_topo_type_t *tinfo,
                         void *cookie)
{
    return bcmpc_topo_type_destroy(unit, tid);
}

static int
bcm56996_a0_inst_destroy(int unit, int pm_id, bcmpc_topo_t *pm_info,
                         void *cookie)
{
    return bcmpc_topo_destroy(unit, pm_id);
}

/*******************************************************************************
 * Port Control Driver
 */

static int
bcm56996_a0_pc_pblk_get(int unit, bcmpc_pport_t pport,
                        int *blk_num, int *blk_port)
{
    int rv, pm_id, blktype;
    bcmpc_topo_t pm_info;
    const bcmdrd_chip_info_t *ci;
    bcmdrd_pblk_t pblk;

    ci = bcmdrd_dev_chip_info_get(unit);
    if (ci == NULL) {
        return SHR_E_INTERNAL;
    }

    if (pport == CMIC_PORT) {
        if (blk_num) {
            *blk_num = ci->hmi_block;
        }
        if (blk_port) {
            *blk_port = 0;
        }
        return SHR_E_NONE;
    }

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    switch (pm_info.tid) {
        case TH4G_PM_TYPE_PM4X10:
            blktype = BLKTYPE_XLPORT;
            break;
        case TH4G_PM_TYPE_PM8X100:
            blktype = BLKTYPE_CDPORT;
            break;
        case TH4G_PM_TYPE_LB:
            blktype = BLKTYPE_LBPORT;
            break;
        default:
            return SHR_E_INTERNAL;
    }

    if (bcmdrd_chip_port_block(ci, pport, blktype, &pblk)) {
        return SHR_E_INTERNAL;
    }

    if (blk_num) {
        *blk_num = pblk.blknum;
    }
    if (blk_port) {
        if (pport == 258) {
            *blk_port = 2;
        } else {
            *blk_port = pblk.lane;
        }
    }

    return SHR_E_NONE;
}

static bcmpmac_drv_t *
bcm56996_a0_pc_pmac_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
        return &bcm56996_a0_pmac_xlport;
    } else if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
        return &bcm56996_a0_pmac_cdport;
    }

    return NULL;
}

static bool
bcm56996_a0_pc_link_update_required(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return false;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return false;
    }

    /* Do nothing for system port. */
    if (pm_info.tid == TH4G_PM_TYPE_CPU || pm_info.tid == TH4G_PM_TYPE_LB) {
        return false;
    }

    return true;
}


static bcmpc_cfg_profile_t *
bcm56996_a0_pc_port_up_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_up_prof;
    static bcmpc_operation_t port_up_ops[] = {
        { "tm", "ep_credit", BCMPC_OP_PARAM_NONE },
        { "tm", "egr_up", 1 },
        { "tm", "ing_up", 1 },
        { "pmac", "speed_set", BCMPC_OP_PARAM_NONE },
        { "phy", "link_up_event", 0 },
        { "pmac", "rx_enable", 1 },
        { "pmac", "tx_enable", 1 },
        { "pmac", "mac_reset", 0 },
        { "tm", "fwd_enable", 1 },
        { "pmac", "failover_toggle", BCMPC_OP_PARAM_NONE },
    };

    if (!bcm56996_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_up_prof.ops = port_up_ops;
    port_up_prof.op_cnt = COUNTOF(port_up_ops);
    return &port_up_prof;
}

static bcmpc_cfg_profile_t *
bcm56996_a0_pc_port_down_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_down_prof;
    static bcmpc_operation_t port_down_ops[] = {
        { "tm", "fwd_enable", 0 },
        { "pmac", "rx_enable", 0 },
        { "pmac", "tx_flush", 1 },
        { "pmac", "force_pfc_xon_set", 1 },
        { "pmac", "force_pfc_xon_set", 0 },
        { "phy", "link_down_event", 0 },
        { "tm", "ing_up", 0 },
        { "tm", "mmu_up", 0 },
        { "tm", "egr_up", 0 },
        { "pmac", "mac_reset", 0 },
        { "pmac", "tx_flush", 0 },
        { "pmac", "mac_reset", 1 },
    };

    if (!bcm56996_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_down_prof.ops = port_down_ops;
    port_down_prof.op_cnt = COUNTOF(port_down_ops);
    return &port_down_prof;
}

static bcmpc_cfg_profile_t *
bcm56996_a0_pc_port_resume_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_resume_prof;
    static bcmpc_operation_t port_resume_ops[] = {
        { "tm", "ep_credit", BCMPC_OP_PARAM_NONE },
        { "tm", "egr_up", 1 },
        { "tm", "ing_up", 1 },
        { "pmac", "speed_set", BCMPC_OP_PARAM_NONE },
        { "pmac", "rx_enable", 1 },
        { "pmac", "tx_enable", 1 },
        { "pmac", "mac_reset", 0 },
        { "tm", "fwd_enable", 1 },
        { "pmac", "failover_toggle", BCMPC_OP_PARAM_NONE },
    };

    port_resume_prof.ops = port_resume_ops;
    port_resume_prof.op_cnt = COUNTOF(port_resume_ops);

    return &port_resume_prof;
}

static bcmpc_cfg_profile_t *
bcm56996_a0_pc_port_suspend_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_suspend_prof;
    static bcmpc_operation_t port_suspend_ops[] = {
        { "tm", "fwd_enable", 0 },
        { "pmac", "rx_enable", 0 },
        { "pmac", "tx_flush", 1 },
        { "tm", "ing_up", 0 },
        { "tm", "mmu_up", 0 },
        { "tm", "egr_up", 0 },
        { "pmac", "tx_flush", 0 },
        { "pmac", "mac_reset", 1 },
    };

    port_suspend_prof.ops = port_suspend_ops;
    port_suspend_prof.op_cnt = COUNTOF(port_suspend_ops);

    return &port_suspend_prof;
}

static int
bcm56996_a0_pc_topo_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56996_a0_pm_type_create(unit));

    SHR_IF_ERR_EXIT
        (bcm56996_a0_pm_instance_create(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_pc_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_traverse(unit, bcm56996_a0_inst_destroy, NULL));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_traverse(unit, bcm56996_a0_type_destroy, NULL));

    bcmpc_phymod_bus_mutex_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_pc_dev_init(int unit, bool pre_load, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_phymod_bus_mutex_init(unit));

/* will revisit later
    if (!warm && pre_load) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_phy_firmware_loader(unit, 0 , 0, NULL));
    }
*/

exit:
    SHR_FUNC_EXIT();
}

static phymod_bus_t *
bcm56996_a0_pc_phy_bus_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;
    phymod_bus_t *phymod_sim_bus;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
            if (SHR_FAILURE(rv)) {
                return NULL;
            }
            return phymod_sim_bus;
        }
        return &bcm56996_a0_tsce_bus;
    } else if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            return &bcm56996_a0_tsco_physim_bus;
        }
        return &bcm56996_a0_tsco_bus;
    }

    return NULL;
}

static phymod_dispatch_type_t
bcm56996_a0_pc_phy_drv_get(int unit, bcmpc_pport_t pport)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return phymodDispatchTypeCount;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return phymodDispatchTypeCount;
    }

    if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
        return phymodDispatchTypeTsce7;
    } else if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
        return phymodDispatchTypeCount;
    }

    return phymodDispatchTypeCount;
}


static void
bcm56996_a0_pc_pm_pport_get(int unit, bcmpc_pport_t pport, int *pm_pport)
{
    /* Remap PM physical port for device physical port 258. */
    if (pport == 258) {
        *pm_pport = 2;
    }
}

static uint8_t
bcm56996_a0_pc_phy_pll_idx_get(int unit, bcmpc_pport_t pport,
                               bcmpc_pll_type_t pll_type)
{
    /* Both PM4x10 and PM8x50 only support single PLL on BCM56996.
     * So always return pll_idx as '0' in this function.
     */
    return 0;
}

static void
bcm56996_a0_pc_port_lbmp_get(int unit,
                             bcmpc_pport_t pport,
                             int pm_phys_port,
                             uint32_t *lbmp)
{
    /* On BCM56996:
     *     For PM8x100, only one  PMD lanes is allocated to one PCS lane.
     *     For PM4x10, one PMD lane is mapped to one PCS lane.
     */
    *lbmp <<= pm_phys_port;
}

static int
bcm56996_a0_pc_pm_firmware_loader(int unit, bcmpc_pport_t pport,
                                  uint32_t fw_loader_req,
                                  phymod_firmware_loader_f *fw_loader)
{
    return 0;

/*    will revisit later
 *    int port = 0;
    SHR_FUNC_ENTER(unit);

    port = pport;
    SHR_IF_ERR_EXIT
        (bcm56996_a0_phy_firmware_loader(unit, port, fw_loader_req, fw_loader));

exit:
    SHR_FUNC_EXIT();
*/
}


static int
bcm56996_a0_is_internal_port(int unit, bcmpc_pport_t pport,
                             bool *is_internal)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    if ((pm_info.tid == TH4G_PM_TYPE_LB) ||
        (pm_info.tid == TH4G_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_num_phys_ports_get(int unit, bcmpc_pport_t pport,
                                  int pmd_num_lane, int *num_phys_ports)
{
    /* there are 4 PMD lanes and only two managements port on PM4x10 */
    if ((pport == 257) || (pport == 258)) {
        *num_phys_ports = (pmd_num_lane + 1) / 2;
    } else {
        *num_phys_ports = pmd_num_lane;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_max_num_ports_get(int unit, uint32_t *max_pport,
                              uint32_t *max_lport)
{
    *max_pport = BCM56996_MAX_PPORTS;
    *max_lport = BCM56996_MAX_LPORTS;

    return SHR_E_NONE;
}

static int
bcm56996_a0_is_internal_pm(int unit, int pm_id, bool *is_internal)
{
    int rv;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    if ((pm_info.tid == TH4G_PM_TYPE_LB) ||
        (pm_info.tid == TH4G_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_skip_pm_instance(int unit, bcmpc_pport_t pport, bool *is_skip)
{
    /* for TH4G, need to skip the PCS block1 for all front panel port */
    *is_skip = FALSE;
    if (pport <= 256) {
        if ((pport - 1) % 8 == 4) {
            *is_skip = TRUE;
        }
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_phymod_core_init_pass1_pport_adjust(int unit, pm_access_t *pm_acc, bool *is_true)
{
    bcmpc_phy_access_data_t *phy_user_data;
    bcmpc_phy_access_data_t *core_user_data;

    /* for TH4G, need to adjust the pport for pass1  for all front panel ports */
    *is_true = FALSE;
    if (pm_acc) {
        if (pm_acc->pport <= 256){
            *is_true = TRUE;
            pm_acc->pport = ((pm_acc->pport - 1) / 8) * 8 + 1;
            phy_user_data = pm_acc->phy_acc.access.user_acc;
            core_user_data = pm_acc->core_acc.access.user_acc;
            phy_user_data->pport = pm_acc->pport;
            core_user_data->pport = pm_acc->pport;
        }
    } else {
        return SHR_E_INTERNAL;
    }
    return SHR_E_NONE;
}


static int
bcm56996_a0_is_dual_pml_supported(int unit, bcmpc_pport_t pport, bool *is_true)
{
    /* On TH4G, for all front panel ports, the PM consists of two PMLs. */
    *is_true = FALSE;
    if (pport <= 256) {
        *is_true = TRUE;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_phymod_core_init_pass2_u1_adjust(int unit, pm_access_t *pm_acc)
{
    bcmpc_phy_access_data_t *phy_user_data;
    bcmpc_phy_access_data_t *core_user_data;

    if (pm_acc) {
        pm_acc->pport = ((pm_acc->pport - 1) / 8) * 8 + 5;
        phy_user_data = pm_acc->phy_acc.access.user_acc;
        core_user_data = pm_acc->core_acc.access.user_acc;
        phy_user_data->pport = pm_acc->pport;
        core_user_data->pport = pm_acc->pport;
        return SHR_E_NONE;
    } else {
        return SHR_E_INTERNAL;
    }
}

static int
bcm56996_a0_phymod_core_init_pass2_u0_adjust(int unit, pm_access_t *pm_acc)
{
    bcmpc_phy_access_data_t *phy_user_data;
    bcmpc_phy_access_data_t *core_user_data;

    if (pm_acc) {
        pm_acc->pport = ((pm_acc->pport - 1) / 8) * 8 + 1;
        phy_user_data = pm_acc->phy_acc.access.user_acc;
        core_user_data = pm_acc->core_acc.access.user_acc;
        phy_user_data->pport = pm_acc->pport;
        core_user_data->pport = pm_acc->pport;
        return SHR_E_NONE;
    } else {
        return SHR_E_INTERNAL;
    }
}

static int
bcm56996_a0_pm_base_pport_adjust(int unit, bcmpc_pport_t base_pport,
                                 uint8_t pml_sel,
                                 bcmpc_pport_t *base_pport_adjust)
{
    if (base_pport <= 256) {
        if (pml_sel == 0) {
            *base_pport_adjust = ((base_pport - 1) / 8) * 8 + 1;
        } else {
            *base_pport_adjust = ((base_pport - 1) / 8) * 8 + 5;
        }
    } else {
        *base_pport_adjust = 257;
    }
    return SHR_E_NONE;
}

static int
bcm56996_a0_is_stall_tx_supported(int unit, bcmpc_pport_t pport,
                                  bool *is_supported)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
        *is_supported = true;
    } else {
        *is_supported = false;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_is_sec_supported (int unit)
{
    return 0;
}

static int
bcm56996_a0_is_pm_sec_enabled(int unit, int pm_id)
{
    return 0;
}

static int
bcm56996_a0_pm_physim_add(int unit, bcmpc_pport_t pport)
{
    phymod_sim_drv_t *sim_drv = NULL;
    bcmpc_pport_t pport_base;
    uint16_t core_mdio_addr;
    uint8_t pml = 2;

    SHR_FUNC_ENTER(unit);
    /*
     * TH4G PM8x100 has two PMLs within one PM.
     * So we need to allocate two PHYSIM instances for one PM.
     */
    if (pport <= 256) {
        /* PM8x100 */
        phymod_control_physim_drv_get(unit, phymodDispatchTypeCount, &sim_drv);
        if (sim_drv == NULL) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        while (pml--) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_pm_base_pport_adjust(unit, pport,
                                                  pml, &pport_base));
            core_mdio_addr=(uint16_t)bcm56996_a0_pc_phy_addr_get(unit,
                                                                 pport_base);
            phymod_control_physim_add(unit, core_mdio_addr, sim_drv);
        }
    } else {
        /* PM4x10 */
        phymod_control_physim_drv_get(unit, phymodDispatchTypeTsce7, &sim_drv);
        if (sim_drv == NULL) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        core_mdio_addr=(uint16_t)bcm56996_a0_pc_phy_addr_get(unit, pport);
        phymod_control_physim_add(unit, core_mdio_addr, sim_drv);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_port_to_mac_rsv_mask(int unit, bcmpc_pport_t pport,
                                 uint32_t in_flags, uint32_t in_value,
                                 uint32_t *out_flags, uint32_t *out_value)
{
    int rv, pm_id;
    bool is_set = false;
    uint32_t i = 0, mask_value = 0;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    for (i = BCMPC_MAC_RSV_WRONG_SA_PKTS; i <= PC_MAC_RSV_FIELD_ID_MAX; i++) {
        /* Skip if the RSV mask control not set. */
        if (!(in_flags & (1 << i))) {
            continue;
        }

        /* check is RSV control is being set or reset. */
        is_set = (in_value & (1 << i))? true: false;
        mask_value = 0;

        switch (i) {
            case BCMPC_MAC_RSV_WRONG_SA_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_WRONG_SA;
                    mask_value = CDMAC_RSV_MASK_WRONG_SA;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_WRONG_SA;
                    mask_value = XLMAC_RSV_MASK_WRONG_SA;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = CDMAC_RSV_MASK_PFC_DA_ERR;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = XLMAC_RSV_MASK_PFC_DA_ERR;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_CRC_ERR;
                    mask_value = CDMAC_RSV_MASK_CRC_ERR;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = CDMAC_RSV_MASK_FRAME_LEN_ERR;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = XLMAC_RSV_MASK_FRAME_LEN_ERR;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = CDMAC_RSV_MASK_TRUNCATED_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = XLMAC_RSV_MASK_TRUNCATED_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = CDMAC_RSV_MASK_FRAME_RCV_OK;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = XLMAC_RSV_MASK_FRAME_RCV_OK;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_MCAST_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_MCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_BCAST_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_BCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = CDMAC_RSV_MASK_CONTROL_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = XLMAC_RSV_MASK_CONTROL_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = CDMAC_RSV_MASK_PAUSE_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = XLMAC_RSV_MASK_PAUSE_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = CDMAC_RSV_MASK_OPCODE_ERR;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = XLMAC_RSV_MASK_OPCODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_UCAST_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_UCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RX_FIFO_FULL;
                    mask_value = XLMAC_RSV_MASK_RX_FIFO_FULL;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = CDMAC_RSV_MASK_RUNT_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = XLMAC_RSV_MASK_RUNT_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_FRAME;
                    mask_value = CDMAC_RSV_MASK_PFC_FRAME;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_FRAME;
                    mask_value = XLMAC_RSV_MASK_PFC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_SCH_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_SCH_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MACSEC_FRAME;
                    mask_value = XLMAC_RSV_MASK_MACSEC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR;
                    mask_value = XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR;
                }
                break;
            default:
                continue;
                break;
        }
        /* Based on the value is set or reset the bit position. */
        if (is_set) {
            *out_value |= mask_value;
        } else {
            *out_value &= ~mask_value;
        }
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_mac_to_port_rsv_mask(int unit, bcmpc_pport_t pport,
                                 uint32_t in_flags, uint32_t in_value,
                                 uint32_t *out_flags, uint32_t *out_value)
{
    int rv, pm_id;
    uint32_t i = 0;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    for (i = BCMPC_MAC_RSV_WRONG_SA_PKTS; i <= PC_MAC_RSV_FIELD_ID_MAX; i++) {

        switch (i) {
            case BCMPC_MAC_RSV_WRONG_SA_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TH4G_PM_TYPE_PM8X100) {
                } else if (pm_info.tid == TH4G_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags &
                                   XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR)?
                                   (1 << i): 0;
                    *out_value |= (in_flags &
                                   XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR)?
                                   (1 << i): 0;
                }
                break;
            default:
                break;
        }
    }

    return SHR_E_NONE;
}

/*****************************************************************************
 * Flight Data Recorder (FDR) related implementation.
 * 1. FDR control configuration get.
 * 2. FDR control configuration set.
 * 3. FDR stats get.
 */
static int
bcm56996_a0_port_fdr_control_get(int unit, bcmpc_pport_t pport,
                                 bcmpc_fdr_port_control_t *fdr_ctrl)
{
    int pm_id;
    uint32_t wsize;
    uint32_t port_arr_idx = 0;
    uint32_t port_pos = 0;
    uint32_t port_idx = 0;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    TOP_PM_FDR_MEMm_t fdr_mem;
    RX_FDR_CONTROLm_t fdr_ctrl_mem;

    SHR_FUNC_ENTER(unit);
    /*
     * Check if FDR is supported on this port (Port Macro).
     * Get Port Macro type and chck if FDR feature is supported.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));

    if (!SHR_BITGET(tinfo.pm_feature, BCMPC_FT_FDR_SUPPORTED)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Now set the FDR configuration on the port. */
    TOP_PM_FDR_MEMm_CLR(fdr_mem);

    wsize = SHR_BYTES2WORDS(TOP_PM_FDR_MEMm_SIZE);

    /*
     * Get the port index in the physical port bmap array.
     * There are 8 words in the memory, each word is 4 bytes.
     * 1 Word = 4 x 8 = 32 bits,
     * Total bmap size (Number of Words * Word Size) 8 * 32 = 256.
     * To get the port index in the list,
     * (pport - 1) / (Number of Words), Array index in the bmap.
     * (pport - 1) % (Word Size) (4 * 8 = 32 bits).
     */
    port_arr_idx = ((pport - 1)/SHR_WORDS2BYTES(wsize));
    port_pos = ((pport - 1) % (SHR_WORDS2BYTES(1) * 8));

    /* Physical port array index cannot be great than the word size. */
    if (port_arr_idx >= wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    READ_TOP_PM_FDR_MEMm(unit, 0, 0, &fdr_mem);
    fdr_ctrl->enable = (TOP_PM_FDR_MEMm_GET(fdr_mem, port_arr_idx) &
                                            ((uint32_t) 1 << port_pos));

    RX_FDR_CONTROLm_CLR(fdr_ctrl_mem);

    SHR_IF_ERR_EXIT
        (READ_RX_FDR_CONTROLm(unit, port_idx, &fdr_ctrl_mem, pport));

    fdr_ctrl->window_size = RX_FDR_CONTROLm_SYMERR_WINDOWf_GET(fdr_ctrl_mem);
    fdr_ctrl->symbol_error_start_value =
                       RX_FDR_CONTROLm_S_VALUEf_GET(fdr_ctrl_mem);
    fdr_ctrl->symbol_error_threshold =
                       RX_FDR_CONTROLm_SYMERR_THRESHOLDf_GET(fdr_ctrl_mem);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_port_fdr_control_set(int unit, bcmpc_pport_t pport,
                                 bcmpc_fdr_port_control_t fdr_ctrl)
{
    int pm_id;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    RX_FDR_CONTROLm_t fdr_ctrl_mem;

    SHR_FUNC_ENTER(unit);
    /*
     * Check if FDR is supported on this port (Port Macro).
     * Get Port Macro type and chck if FDR feature is supported.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));

    if (!SHR_BITGET(tinfo.pm_feature, BCMPC_FT_FDR_SUPPORTED)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Now set the FDR configuration on the port.
     * FDR enable should be the last step during the
     * configuration, if the feature is being enabled.
     */

    RX_FDR_CONTROLm_CLR(fdr_ctrl_mem);

    SHR_IF_ERR_EXIT
        (READ_RX_FDR_CONTROLm(unit, (pport - pm_info.base_pport),
                              &fdr_ctrl_mem, pport));

    RX_FDR_CONTROLm_SYMERR_WINDOWf_SET(fdr_ctrl_mem, fdr_ctrl.window_size);
    RX_FDR_CONTROLm_S_VALUEf_SET(fdr_ctrl_mem,
                                 fdr_ctrl.symbol_error_start_value);
    RX_FDR_CONTROLm_SYMERR_THRESHOLDf_SET(fdr_ctrl_mem,
                                          fdr_ctrl.symbol_error_threshold);

    SHR_IF_ERR_EXIT
        (WRITE_RX_FDR_CONTROLm(unit, (pport - pm_info.base_pport),
                               fdr_ctrl_mem, pport));

    /*
     * Perform interrupt operation only if being updated
     * by the application.
     * Enable/Disable the FDR interrupt.
     * Register/Unregister the interrupt callback.
     */
    if (SHR_BITGET(fdr_ctrl.fbmp, BCMPC_FDR_CTRL_LT_FLD_FDR_INTR_ENABLE)) {
        if (fdr_ctrl.interrupt_enable) {
            bcm56996_a0_fdr_interrupt_control(unit, pport, INTR_ENABLE);
        } else {
            bcm56996_a0_fdr_interrupt_control(unit, pport, INTR_DISABLE);
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_fdr_port_enable_set(unit, pport, fdr_ctrl.enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_port_fdr_stats_get(int unit, bcmpc_pport_t pport,
                               bcmpc_fdr_port_stats_t *fdr_stats)
{
    int pm_id;
    uint32_t wsize;
    uint32_t port_arr_idx = 0;
    uint32_t port_pos = 0;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    uint32_t fdr_bmap = 0;
    bool is_enabled;
    uint32_t timer[2];
    TOP_PM_FDR_MEMm_t fdr_mem;
    RX_FDR_STATUSm_t fdr_cnts;


    SHR_FUNC_ENTER(unit);
    /*
     * Check if FDR is supported on this port (Port Macro).
     * Get Port Macro type and chck if FDR feature is supported.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));

    if (!SHR_BITGET(tinfo.pm_feature, BCMPC_FT_FDR_SUPPORTED)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Check if FDR is enabled on the port.
     * TOP_PM_FDR_MEM is a bitmap of all the physical ports
     * belonging to the Port Macros which support the FDR feature.
     * The index starts from bit position 0, which indicates
     * the physical port 1 (pport - 1).
     */
    TOP_PM_FDR_MEMm_CLR(fdr_mem);

    wsize = SHR_BYTES2WORDS(TOP_PM_FDR_MEMm_SIZE);

    /*
     * Get the port index in the physical port bmap array.
     * There are 8 words in the memory, each word is 4 bytes.
     * 1 Word = 4 x 8 = 32 bits,
     * Total bmap size (Number of Words * Word Size) 8 * 32 = 256.
     * To get the port index in the list,
     * (pport - 1) / (Number of Words), Array index in the bmap.
     * (pport - 1) % (Word Size).
     */
    port_arr_idx = ((pport - 1)/SHR_WORDS2BYTES(wsize));
    port_pos = ((pport - 1) % (SHR_WORDS2BYTES(1) * 8));

    /* Physical port array index cannot be great than the word size. */
    if (port_arr_idx >= wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Read TOP FDR memory to check if FDR is enabled. */
    SHR_IF_ERR_EXIT
        (READ_TOP_PM_FDR_MEMm(unit, 0, 0, &fdr_mem));

    fdr_bmap = (TOP_PM_FDR_MEMm_GET(fdr_mem, port_arr_idx));
    is_enabled = fdr_bmap & ((uint32_t) 1 << port_pos);

    /* Return if not enabled. */
    if (!is_enabled) {
        SHR_EXIT();
    }

    /* Read the FDR statistics from the hardware. */
    RX_FDR_STATUSm_CLR(fdr_cnts);

    SHR_IF_ERR_EXIT
        (READ_RX_FDR_STATUSm(unit, (pport - pm_info.base_pport), &fdr_cnts,
                             pport));

    RX_FDR_STATUSm_START_TIMEf_GET(fdr_cnts, timer);
    fdr_stats->start_time |= timer[1];
    fdr_stats->start_time = (fdr_stats->start_time << 32);
    fdr_stats->start_time |= (timer[0]);
    RX_FDR_STATUSm_END_TIMEf_GET(fdr_cnts, timer);
    fdr_stats->end_time |= timer[1];
    fdr_stats->end_time = (fdr_stats->end_time << 32);
    fdr_stats->end_time |= (timer[0]);
    fdr_stats->num_uncorr_cws = RX_FDR_STATUSm_UNCORR_CWf_GET(fdr_cnts);
    fdr_stats->num_cws = RX_FDR_STATUSm_CWf_GET(fdr_cnts);
    fdr_stats->num_symerrs = RX_FDR_STATUSm_SYM_ERRf_GET(fdr_cnts);
    fdr_stats->s0_errs = RX_FDR_STATUSm_S0f_GET(fdr_cnts);
    fdr_stats->s1_errs = RX_FDR_STATUSm_S1f_GET(fdr_cnts);
    fdr_stats->s2_errs = RX_FDR_STATUSm_S2f_GET(fdr_cnts);
    fdr_stats->s3_errs = RX_FDR_STATUSm_S3f_GET(fdr_cnts);
    fdr_stats->s4_errs = RX_FDR_STATUSm_S4f_GET(fdr_cnts);
    fdr_stats->s5_errs = RX_FDR_STATUSm_S5f_GET(fdr_cnts);
    fdr_stats->s6_errs = RX_FDR_STATUSm_S6f_GET(fdr_cnts);
    fdr_stats->s7_errs = RX_FDR_STATUSm_S7f_GET(fdr_cnts);

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 *
 * bcm566696_A0 (Tomahawk_4G driver function vectors.
 */
static bcmpc_drv_t bcm56996_a0_pc_drv = {
    .topology_init = bcm56996_a0_pc_topo_init,
    .dev_init = bcm56996_a0_pc_dev_init,
    .dev_cleanup = bcm56996_a0_pc_dev_cleanup,
    .phy_addr_get = bcm56996_a0_pc_phy_addr_get,
    .pblk_get = bcm56996_a0_pc_pblk_get,
    .phy_bus_get = bcm56996_a0_pc_phy_bus_get,
    .pmac_bus_get = bcmpc_cmicx_pmac_bus_get,
    .phy_drv_get = bcm56996_a0_pc_phy_drv_get,
    .pmac_drv_get = bcm56996_a0_pc_pmac_drv_get,
    .phy_pll_idx_get = bcm56996_a0_pc_phy_pll_idx_get,
    .port_up_prof_get = bcm56996_a0_pc_port_up_prof_get,
    .port_down_prof_get = bcm56996_a0_pc_port_down_prof_get,
    .port_suspend_prof_get = bcm56996_a0_pc_port_suspend_prof_get,
    .port_resume_prof_get = bcm56996_a0_pc_port_resume_prof_get,
    .pm_pport_get = bcm56996_a0_pc_pm_pport_get,
    .port_lbmp_get = bcm56996_a0_pc_port_lbmp_get,
    .pm_firmware_loader = bcm56996_a0_pc_pm_firmware_loader,
    .pm_phy_raw_read = bcm56996_a0_raw_reg_read,
    .pm_phy_raw_write = bcm56996_a0_raw_reg_write,
    .is_internal_port = bcm56996_a0_is_internal_port,
    .num_phys_ports_get = bcm56996_a0_num_phys_ports_get,
    .max_num_ports_get = bcm56996_a0_max_num_ports_get,
    .is_internal_pm = bcm56996_a0_is_internal_pm,
    .skip_pm_instance   = bcm56996_a0_skip_pm_instance,
    .phymod_core_init_pass1_pport_adjust = bcm56996_a0_phymod_core_init_pass1_pport_adjust,
    .is_dual_pml_supported = bcm56996_a0_is_dual_pml_supported,
    .phymod_core_init_pass2_u1_adjust = bcm56996_a0_phymod_core_init_pass2_u1_adjust,
    .phymod_core_init_pass2_u0_adjust = bcm56996_a0_phymod_core_init_pass2_u0_adjust,
    .pm_base_pport_adjust = bcm56996_a0_pm_base_pport_adjust,
    .is_stall_tx_supported = bcm56996_a0_is_stall_tx_supported,
    .is_sec_supported = bcm56996_a0_is_sec_supported,
    .is_pm_sec_enabled = bcm56996_a0_is_pm_sec_enabled,
    .pm_physim_add = bcm56996_a0_pm_physim_add,
    .port_to_mac_rsv = bcm56996_a0_port_to_mac_rsv_mask,
    .mac_to_port_rsv = bcm56996_a0_mac_to_port_rsv_mask,
    .fdr_control_get = bcm56996_a0_port_fdr_control_get,
    .fdr_control_set = bcm56996_a0_port_fdr_control_set,
    .fdr_stats_get = bcm56996_a0_port_fdr_stats_get
};

/*******************************************************************************
 * Public functions
 */

int
bcm56996_a0_pc_drv_attach(int unit)
{
    return bcmpc_drv_set(unit, &bcm56996_a0_pc_drv);
}
