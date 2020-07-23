/*! \file bcm56880_a0_pc_drv_attach.c
 *
 * BCM56880 device specific PC driver.
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
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_cmicx_pmac.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pm_internal.h>


#include <bcmbd/chip/bcm56880_a0_acc.h>
#include <bcmpmac/chip/bcm56880_a0_pmac_drv_list.h>
#include <bcmpmac/chip/bcmpmac_common_cdmac_defs.h>
#include <bcmpmac/chip/bcmpmac_common_xlmac_defs.h>

#include "bcm56880_a0_pc_internal.h"
#include "bcm56880_a0_pc_miim.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

#define BCM56880_MAX_LPORTS 160
/*
 * 256 (32 * PM8X50) + 4 (1 * PM4X10) +
 * 1 CPU port + 4 (Internal Loopback ports).
 */
#define BCM56880_MAX_PPORTS 265

typedef enum td4_pm_type_e {
    TD4_PM_TYPE_CPU,
    TD4_PM_TYPE_LB,
    TD4_PM_TYPE_PM4X10,
    TD4_PM_TYPE_PM8X50
} td4_pm_type_t;

typedef struct pm_lane_layout_s {
    int lane_map[4];
} pm_lane_layout_t;

#define TSC_REG_ADDR_TSCID_SET(_reg_addr, _phyad)    \
            ((_reg_addr) |= ((_phyad) & 0x1f) << 19)

/* The port abilities that bcm56880 PM8x50 doesn't support. */
static const bcmpc_dev_ability_entry_t pm8x50_nonability_table[] = {
        { 20000,  1, PM_PORT_FEC_NONE },
        { 20000,  1, PM_PORT_FEC_BASE_R}
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
bcm56880_a0_tsce_reg_write(void *user_acc, uint32_t phy_addr,
                          uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
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
    data[2] = 1; /* For TSC register write. */

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
bcm56880_a0_tsce_reg_read(void *user_acc, uint32_t phy_addr,
                          uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
    uint32_t data[16];
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;
    bcmpc_phy_access_data_t *user_data = (bcmpc_phy_access_data_t *)user_acc;

    unit = user_data->unit;
    port = user_data->pport;

    TSC_REG_ADDR_TSCID_SET(reg_addr, phy_addr);

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0; /* For TSC register read. */

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
bcm56880_a0_tscbh_reg_write(void *user_acc, uint32_t phy_addr,
                            uint32_t reg_addr, uint32_t val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
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
    data[2] = 1; /* For TSC register write. */

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
bcm56880_a0_tscbh_reg_read(void *user_acc, uint32_t phy_addr,
                           uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    int unit, port, idx;
    uint32_t data[16];
    bcmpc_phy_access_data_t *user_data = user_acc;
    CDPORT_TSC_UCMEM_DATAm_t cd_ucmem_data;

    unit = user_data->unit;
    port = user_data->pport;

    sal_memset(data, 0, sizeof(data));
    data[0] = reg_addr & 0xffffffff;
    data[2] = 0; /* For TSC register read. */

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
bcm56880_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport);

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
bcm56880_a0_tsce_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    int rv = SHR_E_NONE;
    uint32_t data[16];
    uint32_t offset = XLPORT_WC_UCMEM_DATAm_OFFSET;
    uint32_t blkacc = BLKTYPE_XLPORT;
    uint32_t phy_addr;

    sal_memset(data, 0, sizeof(data));

    phy_addr = (uint32_t)bcm56880_a0_pc_phy_addr_get(unit, base_port);
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
bcm56880_a0_tsce_raw_reg_write(int unit, bcmpc_pport_t base_port,
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

    phy_addr = (uint32_t)bcm56880_a0_pc_phy_addr_get(unit, base_port);
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
bcm56880_a0_tscbh_raw_reg_read(int unit, bcmpc_pport_t base_port,
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
bcm56880_a0_tscbh_raw_reg_write(int unit, bcmpc_pport_t base_port,
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
bcm56880_a0_raw_reg_read(int unit, bcmpc_pport_t base_port,
                               uint32_t reg_addr, uint32_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
       SHR_IF_ERR_EXIT
            (bcm56880_a0_tsce_raw_reg_read(unit, base_port,  reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tscbh_raw_reg_read(unit, base_port,  reg_addr, val));
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
bcm56880_a0_raw_reg_write(int unit, bcmpc_pport_t base_port,
                          uint32_t reg_addr, uint32_t val)
{
    SHR_FUNC_ENTER(unit);

    if (base_port == 257) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tsce_raw_reg_write(unit, base_port, reg_addr, val));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tscbh_raw_reg_write(unit, base_port, reg_addr, val));
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
bcm56880_a0_tscbh_mem_read(void *user_acc, phymod_mem_type_t mem_type,
                           uint32_t idx, uint32_t *val)
{
    int rv, unit, port, vdx, wsize;
    bcmpc_phy_access_data_t *user_data = user_acc;
    SPEED_ID_TABLEm_t spd_id;
    SPEED_PRIORITY_MAP_TBLm_t spd_pri;
    AM_TABLEm_t am;
    UM_TABLEm_t um;
    TX_LKUP_1588_MEM_MPP0m_t tx_lkup_mpp0;
    TX_LKUP_1588_MEM_MPP1m_t tx_lkup_mpp1;
    RX_LKUP_1588_MEM_MPP0m_t rx_lkup_mpp0;
    RX_LKUP_1588_MEM_MPP1m_t rx_lkup_mpp1;
    TX_TWOSTEP_1588_TSm_t tx_twostep_1588_ts;

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
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEM_MPP0m_SIZE);
            rv = READ_TX_LKUP_1588_MEM_MPP0m(unit, idx, &tx_lkup_mpp0, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = TX_LKUP_1588_MEM_MPP0m_GET(tx_lkup_mpp0, vdx);
            }
            break;
        case phymodMemTxLkup1588Mpp1:
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEM_MPP1m_SIZE);
            rv = READ_TX_LKUP_1588_MEM_MPP1m(unit, idx, &tx_lkup_mpp1, port);
            for (vdx = 0; vdx < wsize; vdx++) {
                val[vdx] = TX_LKUP_1588_MEM_MPP1m_GET(tx_lkup_mpp1, vdx);
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
        default:
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
bcm56880_a0_tscbh_mem_write(void *user_acc, phymod_mem_type_t mem_type,
                            uint32_t idx, const uint32_t *val)
{
    int rv, unit, port, vdx, wsize;
    bcmpc_phy_access_data_t *user_data = user_acc;
    SPEED_ID_TABLEm_t spd_id;
    SPEED_PRIORITY_MAP_TBLm_t spd_pri;
    AM_TABLEm_t am;
    UM_TABLEm_t um;
    TX_LKUP_1588_MEM_MPP0m_t tx_lkup_mpp0;
    TX_LKUP_1588_MEM_MPP1m_t tx_lkup_mpp1;
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
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEM_MPP0m_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                TX_LKUP_1588_MEM_MPP0m_SET(tx_lkup_mpp0, vdx, val[vdx]);
            }
            rv = WRITE_TX_LKUP_1588_MEM_MPP0m(unit, idx, tx_lkup_mpp0, port);
            break;
        case phymodMemTxLkup1588Mpp1:
            wsize = SHR_BYTES2WORDS(TX_LKUP_1588_MEM_MPP1m_SIZE);
            for (vdx = 0; vdx < wsize; vdx++) {
                TX_LKUP_1588_MEM_MPP1m_SET(tx_lkup_mpp1, vdx, val[vdx]);
            }
            rv = WRITE_TX_LKUP_1588_MEM_MPP1m(unit, idx, tx_lkup_mpp1, port);
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
            return SHR_E_FAIL;
    }

    return rv;
}
static phymod_bus_t bcm56880_a0_tsce_bus = {
    .bus_name = "TSCE Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56880_a0_tsce_reg_read,
    .write = bcm56880_a0_tsce_reg_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

static phymod_bus_t bcm56880_a0_tscbh_bus = {
    .bus_name = "TSCBH Bus",
    .mutex_take = bcmpc_phymod_bus_mutex_take,
    .mutex_give = bcmpc_phymod_bus_mutex_give,
    .read = bcm56880_a0_tscbh_reg_read,
    .write = bcm56880_a0_tscbh_reg_write,
    .mem_read = bcm56880_a0_tscbh_mem_read,
    .mem_write = bcm56880_a0_tscbh_mem_write,
    .bus_capabilities = PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

/*******************************************************************************
 * Port Macro mode validation function
 */


/*******************************************************************************
 * Port operating mode validation function
 */




/*******************************************************************************
 * Device init and cleanup functions
 */

static int
bcm56880_a0_pm_type_create(int unit)
{
    bcmpc_topo_type_t tinfo;

    SHR_FUNC_ENTER(unit);

    /* Create PM4X10 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TD4_PM_TYPE_PM4X10));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM4X10";
    tinfo.num_lanes = 4;
    tinfo.num_ports = 4;
    tinfo.num_plls = 1;
    tinfo.ref_clk = phymodRefClk156Mhz;
    tinfo.pm_drv = &bcmpc_pm_drv_common;
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_CORE_INIT_FULL);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_1588_EGRESS_TIMESTAMP);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_MGMT_PORT_MACRO);
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TD4_PM_TYPE_PM4X10, &tinfo));

    /* Create PM8X50 */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TD4_PM_TYPE_PM8X50));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_PM8X50_GEN2";
    tinfo.num_lanes = 8;
    tinfo.num_ports = 8;
    tinfo.num_plls = 2;
    tinfo.tvco_pll_index = 0;
    tinfo.ref_clk = phymodRefClk156Mhz;
    tinfo.pm_drv = &bcmpc_pm_drv_common;
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_CORE_INIT_SKIP_FW_LOAD);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_RLM);
    SHR_BITSET(tinfo.pm_feature, BCMPC_FT_SW_STATE_UPDATE);
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TD4_PM_TYPE_PM8X50, &tinfo));

    /* Create PM type for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TD4_PM_TYPE_CPU));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_CPU";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TD4_PM_TYPE_CPU, &tinfo));

    /* Create PM type for Loopback port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_create(unit, TD4_PM_TYPE_LB));
    bcmpc_topo_type_t_init(&tinfo);
    tinfo.name = "PC_PM_TYPE_LOOPBACK";
    tinfo.num_lanes = 1;
    tinfo.num_ports = 1;
    tinfo.num_plls = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_set(unit, TD4_PM_TYPE_LB, &tinfo));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_pm_instance_create(int unit, bool warm)
{
    int pm_id = 0, inst_cnt;
    bcmpc_topo_t pm_info;

    SHR_FUNC_ENTER(unit);

    /* Create instance for CPU port */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_create(unit, warm, pm_id));
    bcmpc_topo_t_init(&pm_info);
    pm_info.tid = TD4_PM_TYPE_CPU;
    pm_info.base_pport = CMIC_PORT;
    pm_info.is_active = TRUE;
    SHR_IF_ERR_EXIT
        (bcmpc_topo_set(unit, pm_id, &pm_info));
    pm_id++;

    /* Create instances for CDPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_CDPORT, TD4_PM_TYPE_PM8X50,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for XLPORT/MGMT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_XLPORT, TD4_PM_TYPE_PM4X10,
                                     warm, pm_id, &inst_cnt));
    pm_id += inst_cnt;

    /* Create instances for LBPORT. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_instances_create(unit, BLKTYPE_LBPORT, TD4_PM_TYPE_LB,
                                     warm, pm_id, &inst_cnt));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_type_destroy(int unit, int tid, bcmpc_topo_type_t *tinfo,
                         void *cookie)
{
    return bcmpc_topo_type_destroy(unit, tid);
}

static int
bcm56880_a0_inst_destroy(int unit, int pm_id, bcmpc_topo_t *pm_info,
                         void *cookie)
{
    return bcmpc_topo_destroy(unit, pm_id);
}

/*******************************************************************************
 * Port Control Driver
 */

static int
bcm56880_a0_pc_pblk_get(int unit, bcmpc_pport_t pport,
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
        case TD4_PM_TYPE_PM4X10:
            blktype = BLKTYPE_XLPORT;
            break;
        case TD4_PM_TYPE_PM8X50:
            blktype = BLKTYPE_CDPORT;
            break;
        case TD4_PM_TYPE_LB:
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
        *blk_port = pblk.lane;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_pc_phy_addr_get(int unit, bcmpc_pport_t pport)
{
    /*
     * For PM8x50, every 3-PMs (i.e. 3 * 8 = 24 ports) uses one internal bus.
     * And the management ports (PM4x10) use the internal bus 11.
     *
     * Internal phy address:
     * Bus  0: PHYID 1-24 are mapped to physical ports 1-24
     * Bus  1: PHYID 1-24 are mapped to physical ports 25-48
     * Bus  2: PHYID 1-24 are mapped to physical ports 49-72
     * Bus  3: PHYID 1-24 are mapped to physical ports 73-96
     * Bus  4: PHYID 1-24 are mapped to physical ports 97-120
     * Bus  5: PHYID 1-24 are mapped to physical ports 121-144
     * Bus  6: PHYID 1-24 are mapped to physical ports 145-168
     * Bus  7: PHYID 1-24 are mapped to physical ports 169-192
     * Bus  8: PHYID 1-24 are mapped to physical ports 193-216
     * Bus  9: PHYID 1-24 are mapped to physical ports 217-240
     * Bus 10: PHYID 1-24 are mapped to physical ports 241-256
     * Bus 11: PHYID 1 are mapped to physical ports 257-260
     */

    uint32_t ibus = 0, offset = 0, addr_mask = 0;

    if (pport > 260 || pport == CMIC_PORT) {
        return -1;
    }

    if (pport >= 257) {
        ibus = 11;
        offset = 257;
        /* Each PM4x10 (4 ports) could share one MDIO address. */
        addr_mask = ~0x3;
    } else {
        /* Every continous 24 PM8x50 ports use one bus. */
        ibus = (pport - 1) / 24;
        offset = ibus * 24 + 1;
        /* Each PM8x50 (8 ports) could share one MDIO address. */
        addr_mask = ~0x7;
    }

    return ((pport - offset) & addr_mask) + 1 + BCM56880_A0_PC_MIIM_IBUS(ibus);
}

static phymod_bus_t *
bcm56880_a0_pc_phy_bus_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
            if (SHR_FAILURE(rv)) {
                return NULL;
            }
            return phymod_sim_bus;
        }
        return &bcm56880_a0_tsce_bus;
    } else if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            rv = phymod_control_physim_bus_get(unit, &phymod_sim_bus);
            if (SHR_FAILURE(rv)) {
                return NULL;
            }
            phymod_sim_bus->mem_read = bcm56880_a0_tscbh_mem_read;
            phymod_sim_bus->mem_write = bcm56880_a0_tscbh_mem_write;
            return phymod_sim_bus;
        }
        return &bcm56880_a0_tscbh_bus;
    }

    return NULL;
}

static phymod_dispatch_type_t
bcm56880_a0_pc_phy_drv_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
        return phymodDispatchTypeTsce7;
    } else if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
        return phymodDispatchTypeTscbh_gen2;
    }

    return phymodDispatchTypeCount;
}

static bcmpmac_drv_t *
bcm56880_a0_pc_pmac_drv_get(int unit, bcmpc_pport_t pport)
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

    if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
        return &bcm56880_a0_pmac_xlport;
    } else if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
        return &bcm56880_a0_pmac_cdport;
    }

    return NULL;
}

static uint8_t
bcm56880_a0_pc_phy_pll_idx_get(int unit, bcmpc_pport_t pport,
                               bcmpc_pll_type_t pll_type)
{
    int rv, pm_id;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_id_get(unit, pport, &pm_id);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    /* TD4 uses PLL0 as TVCO source for PM8x50. */
    if (pm_info.tid == TD4_PM_TYPE_PM8X50 && pll_type == BCMPC_PLL_TYPE_TVCO) {
        return 0;
    }

    return 0;
}

static bool
bcm56880_a0_pc_link_update_required(int unit, bcmpc_pport_t pport)
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
    if (pm_info.tid == TD4_PM_TYPE_CPU || pm_info.tid == TD4_PM_TYPE_LB) {
        return false;
    }

    return true;
}

static bcmpc_cfg_profile_t *
bcm56880_a0_pc_port_up_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_up_prof;
    static bcmpc_operation_t port_up_ops[] = {
        { "tm", "ep_credit", BCMPC_OP_PARAM_NONE },
        { "tm", "egr_up", 1 },
        { "tm", "ing_up", 1 },
        { "pmac", "speed_set", BCMPC_OP_PARAM_NONE },
        { "phy", "link_up_event", 0},
        { "pmac", "rx_enable", 1 },
        { "pmac", "tx_enable", 1 },
        { "pmac", "mac_reset", 0 },
        { "tm", "fwd_enable", 1 },
        { "pmac", "failover_toggle", BCMPC_OP_PARAM_NONE },
    };

    if (!bcm56880_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_up_prof.ops = port_up_ops;
    port_up_prof.op_cnt = COUNTOF(port_up_ops);
    return &port_up_prof;
}

static bcmpc_cfg_profile_t *
bcm56880_a0_pc_port_down_prof_get(int unit, bcmpc_pport_t pport)
{
    static bcmpc_cfg_profile_t port_down_prof;
    static bcmpc_operation_t port_down_ops[] = {
        { "tm", "fwd_enable", 0 },
        { "pmac", "rx_enable", 0 },
        { "pmac", "tx_flush", 1 },
        { "pmac", "force_pfc_xon_set", 1 },
        { "pmac", "force_pfc_xon_set", 0 },
        { "phy", "link_down_event", 0},
        { "tm", "ing_up", 0 },
        { "tm", "mmu_up", 0 },
        { "tm", "egr_up", 0 },
        { "pmac", "tx_flush", 0 },
        { "pmac", "mac_reset", 1 },
    };

    if (!bcm56880_a0_pc_link_update_required(unit, pport)) {
        return NULL;
    }

    port_down_prof.ops = port_down_ops;
    port_down_prof.op_cnt = COUNTOF(port_down_ops);
    return &port_down_prof;
}

static bcmpc_cfg_profile_t *
bcm56880_a0_pc_port_resume_prof_get(int unit, bcmpc_pport_t pport)
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
bcm56880_a0_pc_port_suspend_prof_get(int unit, bcmpc_pport_t pport)
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
bcm56880_a0_pc_topo_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_pm_type_create(unit));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_pm_instance_create(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_pc_dev_init(int unit, bool pre_load, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_phymod_bus_mutex_init(unit));

    if (!bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        if (!warm && pre_load) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_phy_firmware_loader(unit, 0 , 0, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_pc_dev_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_topo_traverse(unit, bcm56880_a0_inst_destroy, NULL));

    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_traverse(unit, bcm56880_a0_type_destroy, NULL));

    bcmpc_phymod_bus_mutex_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_pc_pm_firmware_loader(int unit, bcmpc_pport_t pport,
                                  uint32_t fw_loader_req,
                                  phymod_firmware_loader_f *fw_loader)
{
    int port = 0;
    SHR_FUNC_ENTER(unit);

    port = pport;
    SHR_IF_ERR_EXIT
        (bcm56880_a0_phy_firmware_loader(unit, port, fw_loader_req, fw_loader));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_is_internal_port(int unit, bcmpc_pport_t pport,
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

    if ((pm_info.tid == TD4_PM_TYPE_LB) ||
        (pm_info.tid == TD4_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_max_num_ports_get(int unit, uint32_t *max_pport,
                              uint32_t *max_lport)
{
    *max_pport = BCM56880_MAX_PPORTS;
    *max_lport = BCM56880_MAX_LPORTS;

    return SHR_E_NONE;
}

static int
bcm56880_a0_is_internal_pm(int unit, int pm_id, bool *is_internal)
{
    int rv;
    bcmpc_topo_t pm_info;

    rv = bcmpc_topo_get(unit, pm_id, &pm_info);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }

    if ((pm_info.tid == TD4_PM_TYPE_LB) ||
        (pm_info.tid == TD4_PM_TYPE_CPU)) {
        *is_internal = true;
    } else {
        *is_internal = false;
    }

    return SHR_E_NONE;
}


static const bcmpc_dev_ability_entry_t *
bcm56880_a0_unsupported_abilities_get(int unit, bcmpc_pport_t pport, int *abil_size)
{
     phymod_dispatch_type_t phymod_type;

     phymod_type = bcm56880_a0_pc_phy_drv_get(unit, pport);
     if (phymod_type == phymodDispatchTypeTscbh_gen2) {
        *abil_size = COUNTOF(pm8x50_nonability_table);
        return  pm8x50_nonability_table;
    } else {
        *abil_size = 0;
    }

    return  NULL;
}

static int
bcm56880_a0_link_sequence_exec_timeout_get(int unit, uint32_t *to_up,
                                           uint32_t *to_down)
{
    *to_up = 300000 * BCM56880_MAX_LPORTS;
    *to_down = 300000 * BCM56880_MAX_LPORTS;
    return  SHR_E_NONE;
}

static int
bcm56880_a0_dev_revision_a0_get(int unit, bool *rev_id_a0)
{
    int rv = SHR_E_NONE;
    bcmdrd_dev_id_t dev_id;

    rv = bcmdrd_dev_id_get(unit, &dev_id);
    if (dev_id.revision == BCM56880_REV_A0) {
        *rev_id_a0 = true;
    } else {
        *rev_id_a0 = false;
    }

    return rv;
}

static int
bcm56880_a0_is_stall_tx_supported(int unit, bcmpc_pport_t pport,
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

    if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
        *is_supported = true;
    } else {
        *is_supported = false;
    }

    return SHR_E_NONE;
}

static int
bcm56880_a0_is_sec_supported (int unit)
{
    return 0;
}

static int
bcm56880_a0_is_pm_sec_enabled(int unit, int pm_id)
{
    return 0;
}

static int
bcm56880_a0_port_to_mac_rsv_mask(int unit, bcmpc_pport_t pport,
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
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_WRONG_SA;
                    mask_value = CDMAC_RSV_MASK_WRONG_SA;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_WRONG_SA;
                    mask_value = XLMAC_RSV_MASK_WRONG_SA;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = CDMAC_RSV_MASK_STACK_VLAN_DETECT;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                    mask_value = XLMAC_RSV_MASK_STACK_VLAN_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = CDMAC_RSV_MASK_PFC_DA_ERR;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_DA_ERR;
                    mask_value = XLMAC_RSV_MASK_PFC_DA_ERR;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                    mask_value = XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_CRC_ERR;
                    mask_value = CDMAC_RSV_MASK_CRC_ERR;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = CDMAC_RSV_MASK_FRAME_LEN_ERR;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_LEN_ERR;
                    mask_value = XLMAC_RSV_MASK_FRAME_LEN_ERR;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = CDMAC_RSV_MASK_TRUNCATED_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_TRUNCATED_FRAME;
                    mask_value = XLMAC_RSV_MASK_TRUNCATED_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = CDMAC_RSV_MASK_FRAME_RCV_OK;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_FRAME_RCV_OK;
                    mask_value = XLMAC_RSV_MASK_FRAME_RCV_OK;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_MCAST_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_MCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_BCAST_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_BCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_BCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = CDMAC_RSV_MASK_PROMISCUOUS_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                    mask_value = XLMAC_RSV_MASK_PROMISCUOUS_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = CDMAC_RSV_MASK_CONTROL_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_CONTROL_FRAME;
                    mask_value = XLMAC_RSV_MASK_CONTROL_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = CDMAC_RSV_MASK_PAUSE_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PAUSE_FRAME;
                    mask_value = XLMAC_RSV_MASK_PAUSE_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = CDMAC_RSV_MASK_OPCODE_ERR;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_OPCODE_ERR;
                    mask_value = XLMAC_RSV_MASK_OPCODE_ERR;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = CDMAC_RSV_MASK_VLAN_TAG_DETECT;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                    mask_value = XLMAC_RSV_MASK_VLAN_TAG_DETECT;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = CDMAC_RSV_MASK_UCAST_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_UCAST_FRAME;
                    mask_value = XLMAC_RSV_MASK_UCAST_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RX_FIFO_FULL;
                    mask_value = XLMAC_RSV_MASK_RX_FIFO_FULL;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = CDMAC_RSV_MASK_RUNT_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_RUNT_FRAME;
                    mask_value = XLMAC_RSV_MASK_RUNT_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags  |= CDMAC_RSV_MASK_PFC_FRAME;
                    mask_value = CDMAC_RSV_MASK_PFC_FRAME;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_PFC_FRAME;
                    mask_value = XLMAC_RSV_MASK_PFC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_SCH_CRC_ERR;
                    mask_value = XLMAC_RSV_MASK_SCH_CRC_ERR;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags  |= XLMAC_RSV_MASK_MACSEC_FRAME;
                    mask_value = XLMAC_RSV_MASK_MACSEC_FRAME;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
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
bcm56880_a0_mac_to_port_rsv_mask(int unit, bcmpc_pport_t pport,
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
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_WRONG_SA)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_STACK_VLAN_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_STACK_VLAN_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_DA_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value &
                                   XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CRC_ERROR_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_LEN_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_TRUNCATED_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_TRUNCATED_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_GOOD_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_FRAME_RCV_OK)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MULTICAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_MCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BROADCAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_BCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PROMISCUOUS_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PROMISCUOUS_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_CONTROL_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_CONTROL_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PAUSE_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PAUSE_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_BAD_OPCODE_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_OPCODE_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_VLAN_TAGGED_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_VLAN_TAG_DETECT)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_UNICAST_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_UCAST_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RX_FIFO_FULL:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RX_FIFO_FULL)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_RUNT_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_RUNT_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_PFC_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                    *out_flags |= (in_flags & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & CDMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_value & XLMAC_RSV_MASK_PFC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_SCH_CRC_ERROR:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_SCH_CRC_ERR)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_MACSEC_PKTS:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
                    *out_flags |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                    *out_value |= (in_flags & XLMAC_RSV_MASK_MACSEC_FRAME)?
                                  (1 << i): 0;
                }
                break;
            case BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR:
                if (pm_info.tid == TD4_PM_TYPE_PM8X50) {
                } else if (pm_info.tid == TD4_PM_TYPE_PM4X10) {
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
bcm56880_a0_pm_offset_get(int unit, int pm_id, uint32_t *pm_offset)
{

    if ((pm_id > 0) && (pm_id < 33)) {
        /*
         * CMIC to PCS in PM8x50 Port Macro(PM) delay in nanosecond.
         *   CMIC to PM staging delay: 32ns
         *   Staging delay within PM: 6ns
         *   pm_offset = 32ns + 6ns = 38ns
         */
        *pm_offset = 38;
    } else {
        *pm_offset = 0;
    }

    return SHR_E_NONE;
}

static bcmpc_drv_t bcm56880_a0_pc_drv = {
    .topology_init = bcm56880_a0_pc_topo_init,
    .dev_init = bcm56880_a0_pc_dev_init,
    .dev_cleanup = bcm56880_a0_pc_dev_cleanup,
    .phy_addr_get = bcm56880_a0_pc_phy_addr_get,
    .pblk_get = bcm56880_a0_pc_pblk_get,
    .phy_bus_get = bcm56880_a0_pc_phy_bus_get,
    .pmac_bus_get = bcmpc_cmicx_pmac_bus_get,
    .phy_drv_get = bcm56880_a0_pc_phy_drv_get,
    .pmac_drv_get = bcm56880_a0_pc_pmac_drv_get,
    .phy_pll_idx_get = bcm56880_a0_pc_phy_pll_idx_get,
    .port_up_prof_get = bcm56880_a0_pc_port_up_prof_get,
    .port_down_prof_get = bcm56880_a0_pc_port_down_prof_get,
    .port_suspend_prof_get = bcm56880_a0_pc_port_suspend_prof_get,
    .port_resume_prof_get = bcm56880_a0_pc_port_resume_prof_get,
    .pm_firmware_loader = bcm56880_a0_pc_pm_firmware_loader,
    .pm_phy_raw_read = bcm56880_a0_raw_reg_read,
    .pm_phy_raw_write = bcm56880_a0_raw_reg_write,
    .is_internal_port = bcm56880_a0_is_internal_port,
    .max_num_ports_get = bcm56880_a0_max_num_ports_get,
    .is_internal_pm = bcm56880_a0_is_internal_pm,
    .dev_unsupported_abilities_get = bcm56880_a0_unsupported_abilities_get,
    .link_sequence_exec_timeout_get =
                           bcm56880_a0_link_sequence_exec_timeout_get,
    .dev_revision_a0_get = bcm56880_a0_dev_revision_a0_get,
    .is_stall_tx_supported = bcm56880_a0_is_stall_tx_supported,
    .is_sec_supported = bcm56880_a0_is_sec_supported,
    .is_pm_sec_enabled = bcm56880_a0_is_pm_sec_enabled,
    .port_to_mac_rsv = bcm56880_a0_port_to_mac_rsv_mask,
    .mac_to_port_rsv = bcm56880_a0_mac_to_port_rsv_mask,
    .pm_offset_get = bcm56880_a0_pm_offset_get
};


/*******************************************************************************
 * Public functions
 */

int
bcm56880_a0_pc_drv_attach(int unit)
{
    return bcmpc_drv_set(unit, &bcm56880_a0_pc_drv);
}

