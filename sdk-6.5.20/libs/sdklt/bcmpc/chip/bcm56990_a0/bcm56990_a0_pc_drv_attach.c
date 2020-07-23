/*! \file bcm56990_a0_pc_drv_attach.c
 *
 * BCM56990_A0 device specific PC driver.
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

#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmpmac/chip/bcm56990_a0_pmac_drv_list.h>
#include <bcmpmac/chip/bcmpmac_common_cdmac_defs.h>
#include <bcmpmac/chip/bcmpmac_common_xlmac_defs.h>
#include <bcmbd/chip/bcm56990_a0_port_intr.h>


#include "bcm56990_a0_pc_internal.h"
#include "bcm56990_a0_pc_miim.h"


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*
 * BCM56990 has 16 pipelines, each has 17 general ports.
 * 256 (Front panel ports) + 1 (CPU port) + 2 (management ports) +
 * 8 (loopback ports) + 5 (Spare) = 272.
 */
#define BCM56990_MAX_LPORTS 272
/*
 * 256 (64(PM8X50) * 4) + 2 (1 * PM4X10) +
 * 1 CPU port + 8 (Internal Loopback ports).
 */
#define BCM56990_MAX_PPORTS 267

typedef enum th4_pm_type_e {
    TH4_PM_TYPE_CPU,
    TH4_PM_TYPE_LB,
    TH4_PM_TYPE_PM4X10,
    TH4_PM_TYPE_PM8X50
} th4_pm_type_t;

typedef struct pm_lane_layout_s {
    int lane_map[4];
} pm_lane_layout_t;

#define TSC_REG_ADDR_TSCID_SET(_reg_addr, _phyad)    \
            ((_reg_addr) |= ((_phyad) & 0x1f) << 19)

/* The port abilities that bcm56990 PM8x50 gen3 doesn't support. */
static const bcmpc_dev_ability_entry_t pm8x50_gen3_nonability_table[] = {
        { 20000,  1, PM_PORT_FEC_NONE }
};


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
bcm56990_a0_tsce_reg_write(void *user_acc, uint32_t phy_addr,
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
bcm56990_a0_tsce_reg_read(void *user_acc, uint32_t phy_addr,
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
 * \brief Register write function for TSCBH.
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
bcm56990_a0_tscbh_reg_write(void *user_acc, uint32_t phy_addr,
                            uint32_t reg_addr, uint32_t val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    CDPORT_TSC_UCMEM_DATAm_t cd_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

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
 * \brief Register read function for TSCBH.
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
bcm56990_a0_tscbh_reg_read(void *user_acc, uint32_t phy_addr,
                           uint32_t reg_addr, uint32_t *val)
{
    int rv, unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    CDPORT_TSC_UCMEM_DATAm_t cd_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

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
bcm56990_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport);

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
bcm56990_a0_tsce_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = XLPORT_WC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_XLPORT;
    uint32_t phy_addr;

    sal_memset(data, 0, sizeof(data));

    phy_addr = (uint32_t)bcm56990_a0_pc_phy_addr_get(unit, base_port);
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
bcm56990_a0_tsce_raw_reg_write(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = XLPORT_WC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_XLPORT;
    uint32_t phy_addr;

    sal_memset(data, 0, sizeof(data));

    if ((val & 0xffff0000) == 0) {
        val |= 0xffff0000;
    }

    phy_addr = (uint32_t)bcm56990_a0_pc_phy_addr_get(unit, base_port);
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
 * \brief Register read function for TSCBH.
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
bcm56990_a0_tscbh_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = CDPORT_TSC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_CDPORT;

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
 * \brief Register write function for TSCBH.
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
bcm56990_a0_tscbh_raw_reg_write(int unit, bcmpc_pport_t base_port,
                                uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = CDPORT_TSC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_CDPORT;

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
bcm56990_a0_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
       SHR_IF_ERR_EXIT
            (bcm56990_a0_tsce_raw_reg_read(unit, base_port,  reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tscbh_raw_reg_read(unit, base_port,  reg_addr, val));
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
bcm56990_a0_raw_reg_write(int unit, bcmpc_pport_t base_port,
                          uint32_t reg_addr, uint32_t val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tsce_raw_reg_write(unit, base_port, reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_tscbh_raw_reg_write(unit, base_port, reg_addr, val));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Memory read function for TSCBH.
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
bcm56990_a0_tscbh_mem_read(void *user_acc, phymod_mem_type_t mem_type,
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
    TX_TWOSTEP_1588_TSm_t tx_twostep_1588_ts;
    RSFEC_SYMBOL_ERROR_MIBm_t rsfec_symbol_error_mib;

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
        case phymodMemTxTwostep1588Ts:
            wsize = SHR_BYTES2WORDS(TX_TWOSTEP_1588_TSm_SIZE);
            rv = READ_TX_TWOSTEP_1588_TSm(unit, idx, &tx_twostep_1588_ts, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = TX_TWOSTEP_1588_TSm_GET(tx_twostep_1588_ts, vdx);
            }
            break;
        case phymodMemRsFecSymbErrMib:
            wsize = SHR_BYTES2WORDS(RSFEC_SYMBOL_ERROR_MIBm_SIZE);
            rv = READ_RSFEC_SYMBOL_ERROR_MIBm(unit, idx, &rsfec_symbol_error_mib, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = RSFEC_SYMBOL_ERROR_MIBm_GET(rsfec_symbol_error_mib, vdx);
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
 * \brief Memory read function for TSCBH.
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
bcm56990_a0_tscbh_mem_write(void *user_acc, phymod_mem_type_t mem_type,
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

static phymod_bus_t bcm56990_a0_tsce_bus = {
    .bus_name = "TSCE Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56990_a0_tsce_reg_read,
    .write = bcm56990_a0_tsce_reg_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

static phymod_bus_t bcm56990_a0_tscbh_bus = {
    .bus_name = "TSCBH Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56990_a0_tscbh_reg_read,
    .write = bcm56990_a0_tscbh_reg_write,
    .mem_read = bcm56990_a0_tscbh_mem_read,
    .mem_write = bcm56990_a0_tscbh_mem_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};


/*******************************************************************************
 * Port Macro mode validation function
 */


/*******************************************************************************
 * Port operating mode validation function
 */


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
bcm56990_a0_pc_fdr_intr_func(int unit, uint32_t intr_param)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = PORT_INTR_INST_GET(intr_param);
    uint32_t intr_num = PORT_INTR_PARAM_GET(intr_param);
    uint32_t is_handled = 0;


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

                /*
                 * Call the PHYMOD handler for FDR interrupt.
                 * The PHYMOD handler only clears the interrupt and
                 * there is nothing to handle for this interrupt.
                 * The high level interrupt is cleared by calling
                 * BCMBD function call. The BD infrastructure code
                 * only clears the top level interrupts in the
                 * interrupt hierarchy.
                 */
                rv = bcmpc_interrupt_process(unit, lport, BCMPC_INTR_FDR,
                                             &is_handled);
                if (SHR_FAILURE(rv)) {
                    LOG_WARN(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "Failed to clear PHYMOD FDR interrupt(%d)\n"),
                              lport));
                }

                /* Clear interrupt in controller */
                rv = bcmpc_interrupt_enable(unit, pport, intr_num,
                                            NULL, INTR_CLEAR);
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
bcm56990_a0_fdr_interrupt_control(int unit, int pport,
                                  bcmpc_intr_enable_t intr_op)
{
    uint32_t intr_num = CDPORT_INTR_FDR_INTERRUPT;
    SHR_FUNC_ENTER(unit);

    /* Enable FDR interrupt. */
    if (intr_op == INTR_ENABLE) {
        SHR_IF_ERR_EXIT
            (bcmpc_interrupt_enable(unit, pport, intr_num,
                                    bcm56990_a0_pc_fdr_intr_func,
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
bcm56990_a0_fdr_port_enable_set(int unit, int pport, bool enable)
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
bcm56990_a0_pm_type_create(int unit)
{
    bcmpc_topo_type_t tinfo;

    SHR_FUNC_ENTER(unit);

    /* Create PM4X10 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4_PM_TYPE_PM4X10));
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
        (bcmpc_topo_type_set(unit, TH4_PM_TYPE_PM4X10, &tinfo));

    /* Create PM8X50 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4_PM_TYPE_PM8X50));
    bcmpc_topo_type_t_init(&tinfo);
    /*
     * The PM8x50 on TH4 still has 8 lanes, but only has 4 physical ports and
     * 1 PLL.
     */
    tinfo.name = "PC_PM_TYPE_PM8X50_GEN3";
    tinfo.num_lanes = 8;
    tinfo.num_ports = 4;
    tinfo.num_plls = 1;
    tinfo.tvco_pll_index = 0;
    tinfo.ref_clk = phymodRefClk156Mhz;
    tinfo.pm_drv = &bcmpc_pm_drv_common;
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_CORE_INIT_SKIP_FW_LOAD);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_RLM);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_SW_STATE_UPDATE);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_FDR_SUPPORTED);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4_PM_TYPE_PM8X50, &tinfo));

    /* Create PM type for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4_PM_TYPE_CPU));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_CPU";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4_PM_TYPE_CPU, &tinfo));

    /* Create PM type for Loopback port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TH4_PM_TYPE_LB));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_LOOPBACK";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TH4_PM_TYPE_LB, &tinfo));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pm_instance_create(int unit, bool warm)
{
    int pm_id = 0, inst_cnt;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    /* Create instance for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_create(unit, warm, pm_id));
    bcmpc_topo_t_init(&pm_info);
    pm_info.tid = TH4_PM_TYPE_CPU;
    pm_info.base_pport = CMIC_PORT;
    pm_info.is_active = TRUE;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_set(unit, pm_id, &pm_info));
    pm_id++;

    /* Create instances for CDPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_CDPORT, TH4_PM_TYPE_PM8X50,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for XLPORT/MGMT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_XLPORT, TH4_PM_TYPE_PM4X10,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for LBPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_LBPORT, TH4_PM_TYPE_LB,
                                     warm, pm_id, &inst_cnt));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_type_destroy(int unit, int tid, bcmpc_topo_type_t *tinfo,
                         void *cookie)
{
    return bcmpc_topo_type_destroy(unit, tid);
}

static int
bcm56990_a0_inst_destroy(int unit, int pm_id, bcmpc_topo_t *pm_info,
                         void *cookie)
{
    return bcmpc_topo_destroy(unit, pm_id);
}

/*******************************************************************************
 * Port Control Driver
 */

static int
bcm56990_a0_pc_pblk_get(int unit, bcmpc_pport_t pport,
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
        case TH4_PM_TYPE_PM4X10:
            blktype = BLKTYPE_XLPORT;
            break;
        case TH4_PM_TYPE_PM8X50:
            blktype = BLKTYPE_CDPORT;
            break;
        case TH4_PM_TYPE_LB:
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

static int
bcm56990_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport)
{
    /*
     * For PM8x50, use indirect acessing scheme.
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
        return (1 + BCM56990_A0_PC_MIIM_IBUS(ibus));
    } else {
        /* Every continous 64 PM8x50 ports use one bus. */
        ibus = (pport - 1) / 64;
        offset = ibus * 64 + 1;
        /* Each PM8x50, 4 ports share one MDIO address. */
        return ((pport - offset) / 4 + 1 + BCM56990_A0_PC_MIIM_IBUS(ibus));
    }


}

static phymod_bus_t *
bcm56990_a0_pc_phy_bus_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
            if (SHR_FAILURE(rv)) {
                return NULL;
            }
            return phymod_sim_bus;
        }
        return &bcm56990_a0_tsce_bus;
    } else if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
            if (SHR_FAILURE(rv)) {
                return NULL;
            }
            phymod_sim_bus->mem_read = bcm56990_a0_tscbh_mem_read;
            phymod_sim_bus->mem_write = bcm56990_a0_tscbh_mem_write;
            return phymod_sim_bus;
        }
        return &bcm56990_a0_tscbh_bus;
    }

    return NULL;
}

static phymod_dispatch_type_t
bcm56990_a0_pc_phy_drv_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
        return phymodDispatchTypeTsce7;
    } else if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
        return phymodDispatchTypeTscbh_gen3;
    }

    return phymodDispatchTypeCount;
}

static bcmpmac_drv_t *
bcm56990_a0_pc_pmac_drv_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
        return &bcm56990_a0_pmac_xlport;
    } else if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
        return &bcm56990_a0_pmac_cdport;
    }

    return NULL;
}

static uint8_t
bcm56990_a0_pc_phy_pll_idx_get(int unit, bcmpc_pport_t pport,
                               bcmpc_pll_type_t pll_type)
{
    /* Both PM4x10 and PM8x50 only support single PLL on BCM56990.
     * So always return pll_idx as '0' in this function.
     */
    return 0;
}

static bool
bcm56990_a0_pc_link_update_required(int unit, bcmpc_pport_t pport)
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
    if (pm_info.tid == TH4_PM_TYPE_CPU || pm_info.tid == TH4_PM_TYPE_LB) {
        return false;
    }

    return true;
}

static bcmpc_cfg_profile_t *
bcm56990_a0_pc_port_up_prof_get(int unit, bcmpc_pport_t pport)
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

    if (!bcm56990_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_up_prof.ops = port_up_ops;
    port_up_prof.op_cnt = COUNTOF(port_up_ops);
    return &port_up_prof;
}

static bcmpc_cfg_profile_t *
bcm56990_a0_pc_port_down_prof_get(int unit, bcmpc_pport_t pport)
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

    if (!bcm56990_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_down_prof.ops = port_down_ops;
    port_down_prof.op_cnt = COUNTOF(port_down_ops);
    return &port_down_prof;
}

static bcmpc_cfg_profile_t *
bcm56990_a0_pc_port_resume_prof_get(int unit, bcmpc_pport_t pport)
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
bcm56990_a0_pc_port_suspend_prof_get(int unit, bcmpc_pport_t pport)
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
bcm56990_a0_pc_topo_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56990_a0_pm_type_create(unit));

    SHR_IF_ERR_EXIT
        (bcm56990_a0_pm_instance_create(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pc_dev_init(int unit, bool pre_load, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_phymod_bus_mutex_init(unit));

    if (!warm && pre_load) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_phy_firmware_loader(unit, 0 , 0, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pc_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_traverse(unit, bcm56990_a0_inst_destroy, NULL));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_traverse(unit, bcm56990_a0_type_destroy, NULL));

    bcmpc_phymod_bus_mutex_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

static void
bcm56990_a0_pc_pm_pport_get(int unit, bcmpc_pport_t pport, int *pm_pport)
{
    /* Remap PM physical port for device physical port 258. */
    if (pport == 258) {
        *pm_pport = 2;
    }
}

static void
bcm56990_a0_pc_port_lbmp_get(int unit,
                             bcmpc_pport_t pport,
                             int pm_phys_port,
                             uint32_t *lbmp)
{
    /* On BCM56990:
     *     For PM8x50, two PMD lanes are allocated to one PCS lane.
     *     For PM4x10, one PMD lane is mapped to one PCS lane.
     */
    if (pport < 257) {
        *lbmp <<= (pm_phys_port * 2);
    } else {
        *lbmp <<= pm_phys_port;
    }
}

static int
bcm56990_a0_pc_pm_firmware_loader(int unit, bcmpc_pport_t pport,
                                  uint32_t fw_loader_req,
                                  phymod_firmware_loader_f *fw_loader)
{
    int port = 0;
    SHR_FUNC_ENTER(unit);

    port = pport;
    SHR_IF_ERR_EXIT
        (bcm56990_a0_phy_firmware_loader(unit, port, fw_loader_req, fw_loader));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_is_internal_port(int unit, bcmpc_pport_t pport,
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

    if ((pm_info.tid == TH4_PM_TYPE_LB) ||
        (pm_info.tid == TH4_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}

static int
bcm56990_a0_num_phys_ports_get(int unit, bcmpc_pport_t pport,
                                  int pmd_num_lane, int *num_phys_ports)
{
    *num_phys_ports = (pmd_num_lane + 1) / 2;

    return SHR_E_NONE;
}

static int
bcm56990_a0_max_num_ports_get(int unit, uint32_t *max_pport,
                              uint32_t *max_lport)
{
    *max_pport = BCM56990_MAX_PPORTS;
    *max_lport = BCM56990_MAX_LPORTS;

    return SHR_E_NONE;
}

static int
bcm56990_a0_is_internal_pm(int unit, int pm_id, bool *is_internal)
{
    int rv;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    if ((pm_info.tid == TH4_PM_TYPE_LB) ||
        (pm_info.tid == TH4_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}

static const bcmpc_dev_ability_entry_t *
bcm56990_a0_unsupported_abilities_get(int unit, bcmpc_pport_t pport, int *abil_size)
{
     *abil_size = COUNTOF(pm8x50_gen3_nonability_table);

     return  pm8x50_gen3_nonability_table;
}

static int
bcm56990_a0_link_sequence_exec_timeout_get(int unit, uint32_t *to_up,
                                           uint32_t *to_down)
{
    *to_up = 300000 * BCM56990_MAX_LPORTS;
    *to_down = 300000 * BCM56990_MAX_LPORTS;
    return  SHR_E_NONE;
}

static int
bcm56990_a0_dev_revision_a0_get(int unit, bool *rev_id_a0)
{
    int rv = SHR_E_NONE;
    bcmdrd_dev_id_t dev_id;

    rv = bcmdrd_dev_id_get(unit, &dev_id);
    if (dev_id.revision == BCM56990_REV_A0) {
        *rev_id_a0 = true;
    } else {
        *rev_id_a0 = false;
    }

    return rv;
}

static int
bcm56990_a0_is_stall_tx_supported(int unit, bcmpc_pport_t pport,
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

    if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
        *is_supported = true;
    } else {
        *is_supported = false;
    }

    return SHR_E_NONE;
}

static int
bcm56990_a0_is_sec_supported (int unit)
{
    return 0;
}

static int
bcm56990_a0_is_pm_sec_enabled(int unit, int pm_id)
{
    return 0;
}

static int
bcm56990_a0_port_to_mac_rsv_mask(int unit, bcmpc_pport_t pport,
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
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_WRONG_SA;
                    mask_value = CDMAC_RSV_MASK_WRONG_SA;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_WRONG_SA;
                    mask_value = XLMAC_RSV_MASK_WRONG_SA;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = CDMAC_RSV_MASK_PFC_DA_ERR;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = XLMAC_RSV_MASK_PFC_DA_ERR;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_CRC_ERR;
                    mask_value = CDMAC_RSV_MASK_CRC_ERR;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = CDMAC_RSV_MASK_FRAME_LEN_ERR;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = XLMAC_RSV_MASK_FRAME_LEN_ERR;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = CDMAC_RSV_MASK_TRUNCATED_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = XLMAC_RSV_MASK_TRUNCATED_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = CDMAC_RSV_MASK_FRAME_RCV_OK;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = XLMAC_RSV_MASK_FRAME_RCV_OK;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_MCAST_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_MCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_BCAST_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_BCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = CDMAC_RSV_MASK_CONTROL_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = XLMAC_RSV_MASK_CONTROL_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = CDMAC_RSV_MASK_PAUSE_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = XLMAC_RSV_MASK_PAUSE_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = CDMAC_RSV_MASK_OPCODE_ERR;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = XLMAC_RSV_MASK_OPCODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_UCAST_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_UCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RX_FIFO_FULL;
                    mask_value = XLMAC_RSV_MASK_RX_FIFO_FULL;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = CDMAC_RSV_MASK_RUNT_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = XLMAC_RSV_MASK_RUNT_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_FRAME;
                    mask_value = CDMAC_RSV_MASK_PFC_FRAME;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_FRAME;
                    mask_value = XLMAC_RSV_MASK_PFC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_SCH_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_SCH_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MACSEC_FRAME;
                    mask_value = XLMAC_RSV_MASK_MACSEC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
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
bcm56990_a0_mac_to_port_rsv_mask(int unit, bcmpc_pport_t pport,
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
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TH4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TH4_PM_TYPE_PM4X10) {
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

static int
bcm56990_a0_port_fdr_control_get(int unit, bcmpc_pport_t pport,
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
bcm56990_a0_port_fdr_control_set(int unit, bcmpc_pport_t pport,
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
            bcm56990_a0_fdr_interrupt_control(unit, pport, INTR_ENABLE);
        } else {
            bcm56990_a0_fdr_interrupt_control(unit, pport, INTR_DISABLE);
        }
    }

    /* Enable/Disable FDR on port if configured.*/
    SHR_IF_ERR_EXIT
        (bcm56990_a0_fdr_port_enable_set(unit, pport, fdr_ctrl.enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_port_fdr_stats_get(int unit, bcmpc_pport_t pport,
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

static int
bcm56990_a0_pm_offset_get(int unit, int pm_id, uint32_t *pm_offset)
{
    /*
     * CMIC to PCS in PM8x50 Port Macro(PM) delay in nanosecond.
     *   staging delay from CMIC to PM and,
     *   the staging delay within the PM: 36ns
     *
     * BCM56990_A0 has 64 PM8x50s
     */
    if ((pm_id > 0) && (pm_id < 65)) {
        *pm_offset = 36;
    } else {
        *pm_offset = 0;
    }

    return SHR_E_NONE;
}

static void
bcm56990_a0_pc_port_intr_func(int unit, uint32_t intr_param)
{
    int rv, i;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = PORT_INTR_INST_GET(intr_param);
    bcmpc_intr_type_t intr_type;
    uint32_t intr_num = PORT_INTR_PARAM_GET(intr_param);
    uint32_t is_handled = 0;
    CDPORT_INTR_STATUSr_t cdp_stat;
    uint32_t ecc_bitmap;
    uint32_t ecc_type_mask;

    enum {
        TSC_ECC_1B_ERR_RSFEC_RBUF_MPP0 = 0,
        TSC_ECC_1B_ERR_RSFEC_RBUF_MPP1 = 1,
        TSC_ECC_1B_ERR_RSFEC_MPP0 = 2,
        TSC_ECC_1B_ERR_RSFEC_MPP1 = 3,
        TSC_ECC_1B_ERR_DESKEW = 4,
        TSC_ECC_1B_ERR_SPEED = 5,
        TSC_ECC_1B_ERR_AM = 6,
        TSC_ECC_1B_ERR_UM = 7,
        TSC_ECC_1B_ERR_TX_1588 = 8,
        TSC_ECC_1B_ERR_RX_1588_MPP0 = 9,
        TSC_ECC_1B_ERR_RX_1588_MPP1 = 10,
    };
    enum {
        TSC_ECC_2B_ERR_RSFEC_RBUF_MPP0 = 0,
        TSC_ECC_2B_ERR_RSFEC_RBUF_MPP1 = 1,
        TSC_ECC_2B_ERR_RSFEC_MPP0 = 2,
        TSC_ECC_2B_ERR_RSFEC_MPP1 = 3,
        TSC_ECC_2B_ERR_DESKEW = 4,
        TSC_ECC_2B_ERR_SPEED = 5,
        TSC_ECC_2B_ERR_AM = 6,
        TSC_ECC_2B_ERR_UM = 7,
        TSC_ECC_2B_ERR_TX_1588 = 8,
        TSC_ECC_2B_ERR_RX_1588_MPP0 = 9,
        TSC_ECC_2B_ERR_RX_1588_MPP1 = 10,
    };



    lport = bcmpc_pport_to_lport(unit, pport);
    if (lport == BCMPC_INVALID_LPORT) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Invalid lport - ECC intr on physical port(%d)\n"),
                 pport));
        return;
    }

    switch (intr_num) {
        case CDPORT_INTR_TSC_ECC_1B_ERR:
        case CDPORT_INTR_TSC_ECC_2B_ERR:

            if (READ_CDPORT_INTR_STATUSr(unit, &cdp_stat, pport) != 0) {
                return;
            }

            /*
             * Call the PHYMOD handler for port ECC interrupt.
             */
            if (intr_num == CDPORT_INTR_TSC_ECC_1B_ERR) {
                ecc_bitmap = CDPORT_INTR_STATUSr_TSC_ECC_1B_ERRf_GET(cdp_stat);
                for (i = TSC_ECC_1B_ERR_RSFEC_RBUF_MPP0;
                        i <= TSC_ECC_1B_ERR_RX_1588_MPP1;
                        i++) {
                    ecc_type_mask = 1 << i;
                    if (ecc_bitmap & ecc_type_mask) {
                        break;
                    }
                }

                switch (i) {
                    case TSC_ECC_1B_ERR_RSFEC_RBUF_MPP0:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP0;
                        break;
                    case TSC_ECC_1B_ERR_RSFEC_RBUF_MPP1:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP1;
                        break;
                    case TSC_ECC_1B_ERR_RSFEC_MPP0:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP0;
                        break;
                    case TSC_ECC_1B_ERR_RSFEC_MPP1:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP1;
                        break;
                    case TSC_ECC_1B_ERR_DESKEW:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_DESKEW;
                        break;
                    case TSC_ECC_1B_ERR_SPEED:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_SPEED;
                        break;
                    case TSC_ECC_1B_ERR_AM:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_AM;
                        break;
                    case TSC_ECC_1B_ERR_UM:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_UM;
                        break;
                    case TSC_ECC_1B_ERR_TX_1588:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_TX_1588;
                        break;
                    case TSC_ECC_1B_ERR_RX_1588_MPP0:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP0;
                        break;
                    case TSC_ECC_1B_ERR_RX_1588_MPP1:
                        intr_type = BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP1;
                        break;
                    default:
                        /* Unexpected interrupt */
                        return;
                }

            } else if (intr_num == CDPORT_INTR_TSC_ECC_2B_ERR) {
                ecc_bitmap = CDPORT_INTR_STATUSr_TSC_ECC_2B_ERRf_GET(cdp_stat);
                for (i = TSC_ECC_2B_ERR_RSFEC_RBUF_MPP0;
                        i <= TSC_ECC_2B_ERR_RX_1588_MPP1;
                        i++) {
                    ecc_type_mask = 1 << i;
                    if (ecc_bitmap & ecc_type_mask) {
                        break;
                    }
                }

                switch (i) {
                    case TSC_ECC_2B_ERR_RSFEC_RBUF_MPP0:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP0;
                        break;
                    case TSC_ECC_2B_ERR_RSFEC_RBUF_MPP1:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP1;
                        break;
                    case TSC_ECC_2B_ERR_RSFEC_MPP0:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP0;
                        break;
                    case TSC_ECC_2B_ERR_RSFEC_MPP1:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP1;
                        break;
                    case TSC_ECC_2B_ERR_DESKEW:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_DESKEW;
                        break;
                    case TSC_ECC_2B_ERR_SPEED:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_SPEED;
                        break;
                    case TSC_ECC_2B_ERR_AM:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_AM;
                        break;
                    case TSC_ECC_2B_ERR_UM:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_UM;
                        break;
                    case TSC_ECC_2B_ERR_TX_1588:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_TX_1588;
                        break;
                    case TSC_ECC_2B_ERR_RX_1588_MPP0:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP0;
                        break;
                    case TSC_ECC_2B_ERR_RX_1588_MPP1:
                        intr_type = BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP1;
                        break;
                    default:
                        /* Unexpected interrupt */
                        return;
                }
            }

            rv = bcmpc_interrupt_process(unit, lport, intr_type,
                    &is_handled);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Failed to process PHYMOD ECC interrupt(%d)\n"),
                         lport));
            }

            /* Clear interrupt in controller */
            rv = bcmpc_interrupt_enable(unit, pport, intr_num,
                    NULL, INTR_CLEAR);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Failed to clear FDR interrupt(%d)\n"), lport));
            }
            break;
        default:
            /* Unexpected interrupt */
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unexpected interrupt (%d), on physical port(%d)\n"),
                     intr_num, pport));
            return;;
    }

    return;
}

static int
bcm56990_a0_port_interrupt_set(int unit, bcmpc_pport_t pport,
                               bool intr_op)
{
    uint32_t i = 0;
    int pm_id;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    bool is_internal = FALSE;

    uint32_t intr_nums[] = {
                            CDPORT_INTR_TSC_ECC_1B_ERR,
                            CDPORT_INTR_TSC_ECC_2B_ERR,
                           };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));

    SHR_IF_ERR_EXIT
        (bcm56990_a0_is_internal_port(unit, pport, &is_internal));

    if (is_internal ||
        SHR_BITGET(tinfo.pm_feature, BCMPC_FT_MGMT_PORT_MACRO)) {
        /*
         * Interrupts are not required to be enabled for
         * internal ports and management ports
         */
        SHR_ERR_EXIT(SHR_E_NONE);
    }


    /* Enable port interrupts. */
    for (i = 0; i < sizeof(intr_nums)/sizeof(intr_nums[0]); i++) {
        if (intr_op) {
            SHR_IF_ERR_EXIT
                (bcmpc_interrupt_enable(unit, pport, intr_nums[i],
                                        bcm56990_a0_pc_port_intr_func,
                                        INTR_ENABLE));
        } else {
            /* Disable the port interrupt. */
            SHR_IF_ERR_EXIT
                (bcmpc_interrupt_enable(unit, pport, intr_nums[i],
                                        NULL, INTR_DISABLE));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static bcmpc_drv_t bcm56990_a0_pc_drv = {
    .topology_init = bcm56990_a0_pc_topo_init,
    .dev_init = bcm56990_a0_pc_dev_init,
    .dev_cleanup = bcm56990_a0_pc_dev_cleanup,
    .phy_addr_get = bcm56990_a0_pc_phy_addr_get,
    .pblk_get = bcm56990_a0_pc_pblk_get,
    .phy_bus_get = bcm56990_a0_pc_phy_bus_get,
    .pmac_bus_get = bcmpc_cmicx_pmac_bus_get,
    .phy_drv_get = bcm56990_a0_pc_phy_drv_get,
    .pmac_drv_get = bcm56990_a0_pc_pmac_drv_get,
    .phy_pll_idx_get = bcm56990_a0_pc_phy_pll_idx_get,
    .port_up_prof_get = bcm56990_a0_pc_port_up_prof_get,
    .port_down_prof_get = bcm56990_a0_pc_port_down_prof_get,
    .port_suspend_prof_get = bcm56990_a0_pc_port_suspend_prof_get,
    .port_resume_prof_get = bcm56990_a0_pc_port_resume_prof_get,
    .pm_pport_get = bcm56990_a0_pc_pm_pport_get,
    .port_lbmp_get = bcm56990_a0_pc_port_lbmp_get,
    .pm_firmware_loader = bcm56990_a0_pc_pm_firmware_loader,
    .pm_phy_raw_read = bcm56990_a0_raw_reg_read,
    .pm_phy_raw_write = bcm56990_a0_raw_reg_write,
    .is_internal_port = bcm56990_a0_is_internal_port,
    .num_phys_ports_get = bcm56990_a0_num_phys_ports_get,
    .max_num_ports_get = bcm56990_a0_max_num_ports_get,
    .is_internal_pm = bcm56990_a0_is_internal_pm,
    .dev_unsupported_abilities_get = bcm56990_a0_unsupported_abilities_get,
    .link_sequence_exec_timeout_get =
                           bcm56990_a0_link_sequence_exec_timeout_get,
    .dev_revision_a0_get = bcm56990_a0_dev_revision_a0_get,
    .is_stall_tx_supported = bcm56990_a0_is_stall_tx_supported,
    .is_sec_supported = bcm56990_a0_is_sec_supported,
    .is_pm_sec_enabled = bcm56990_a0_is_pm_sec_enabled,
    .port_to_mac_rsv = bcm56990_a0_port_to_mac_rsv_mask,
    .mac_to_port_rsv = bcm56990_a0_mac_to_port_rsv_mask,
    .fdr_control_get = bcm56990_a0_port_fdr_control_get,
    .fdr_control_set = bcm56990_a0_port_fdr_control_set,
    .fdr_stats_get = bcm56990_a0_port_fdr_stats_get,
    .pm_offset_get = bcm56990_a0_pm_offset_get,
    .interrupt_set = bcm56990_a0_port_interrupt_set,
};


/*******************************************************************************
 * Public functions
 */

int
bcm56990_a0_pc_drv_attach(int unit)
{
    return bcmpc_drv_set(unit, &bcm56990_a0_pc_drv);
}
