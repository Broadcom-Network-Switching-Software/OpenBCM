/*! \file bcm56780_a0_phy_fw_load.c
 *
 * BCM56780 PHY firmware loader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_cmic_bcast.h>
#include <phymod/phymod.h>

#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_util_internal.h>

#include "bcm56780_a0_pc_internal.h"

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

#define FW_ALIGN_BYTES 16
#define FW_ALIGN_MASK  (FW_ALIGN_BYTES - 1)

#define BCM56780_CDPORT_TSC_UCMEM_DATA_SIZE 65536

static int
tsce_firmware_helper(const phymod_core_access_t *ca,
                     uint32_t fw_size, const uint8_t *fw_data)
{
    bcmpc_phy_access_data_t *phy_data = PHYMOD_ACC_USER_ACC(&ca->access);
    int unit = phy_data->unit;
    int port = phy_data->pport;
    int ioerr = 0;
    int rv = SHR_E_NONE;
    XLPORT_WC_UCMEM_CTRLr_t xl_ucmem_ctrl;
    XLPORT_WC_UCMEM_DATAm_t xl_ucmem_data;
    uint8_t buf[FW_ALIGN_BYTES];
    const uint8_t *data;
    uint32_t val;
    uint32_t size, idx, wdx, bdx;
    uint32_t ucmem_idx, ucmem_idx_max;

    if (fw_size == 0) {
        return SHR_E_INTERNAL;
    }

    /* Aligned firmware size */
    size = (fw_size + FW_ALIGN_MASK) & ~FW_ALIGN_MASK;

    ucmem_idx_max = XLPORT_WC_UCMEM_DATAm_MAX;
    /* Enable parallel bus access */
    ioerr += READ_XLPORT_WC_UCMEM_CTRLr(unit, &xl_ucmem_ctrl, port);
    XLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(xl_ucmem_ctrl, 1);
    ioerr += WRITE_XLPORT_WC_UCMEM_CTRLr(unit, xl_ucmem_ctrl, port);

    for (idx = 0; idx < size; idx += 16) {
        if (idx + 15 < fw_size) {
            data = fw_data + idx;
        } else {
            /* Use staging buffer for modulo bytes */
            sal_memset(buf, 0, sizeof(buf));
            sal_memcpy(buf, fw_data + idx, FW_ALIGN_BYTES - (size - fw_size));
            data = buf;
        }
        for (wdx = 0; wdx < 4; wdx++) {
            val = 0;
            for (bdx = 0; bdx < 4; bdx++) {
                val |= data[bdx + wdx * 4] << (bdx * 8);
            }
            XLPORT_WC_UCMEM_DATAm_SET(xl_ucmem_data, wdx, val);
        }
        ucmem_idx = idx >> 4;
        if (ucmem_idx > ucmem_idx_max) {
            ucmem_idx = ucmem_idx % ucmem_idx_max;
        }
        WRITE_XLPORT_WC_UCMEM_DATAm(unit, ucmem_idx, xl_ucmem_data, port);
    }

    /* Disable parallel bus access */
    XLPORT_WC_UCMEM_CTRLr_ACCESS_MODEf_SET(xl_ucmem_ctrl, 0);
    ioerr += WRITE_XLPORT_WC_UCMEM_CTRLr(unit, xl_ucmem_ctrl, port);

    return ioerr ? SHR_E_ACCESS : rv;
}

static int
tscbh_firmware_helper(const phymod_core_access_t *ca,
                      uint32_t fw_size, const uint8_t *fw_data)
{
    bcmpc_phy_access_data_t *phy_data = PHYMOD_ACC_USER_ACC(&ca->access);
    int unit = phy_data->unit;
    int port = phy_data->pport;
    int ioerr = 0;
    int rv = SHR_E_NONE;
    uint8_t buf[FW_ALIGN_BYTES];
    const uint8_t *data;
    uint32_t val;
    uint32_t size, idx, wdx, bdx;
    uint32_t ucmem_idx, ucmem_idx_max;
    uint32_t blkacc = BLKTYPE_CDPORT;
    uint32_t offset_tsc_mem = CDPORT_TSC_MEM_CTRLr_OFFSET;
    uint32_t access_mode;
    CDPORT_TSC_UCMEM_DATAm_t cl_ucmem_data;

    if (fw_size == 0) {
        return SHR_E_INTERNAL;
    }

    ucmem_idx_max = BCM56780_CDPORT_TSC_UCMEM_DATA_SIZE / FW_ALIGN_BYTES - 1;

    /* Aligned firmware size */
    size = (fw_size + FW_ALIGN_MASK) & ~FW_ALIGN_MASK;

    /* Enable parallel bus access */
    access_mode = 1;
    ioerr += bcmbd_cmicx_reg_blocks_write(unit, blkacc, port, offset_tsc_mem, 0,
                                          &access_mode, 1);

    for (idx = 0; idx < size; idx += 16) {
        if (idx + 15 < fw_size) {
            data = fw_data + idx;
        } else {
            /* Use staging buffer for modulo bytes */
            sal_memset(buf, 0, sizeof(buf));
            sal_memcpy(buf, fw_data + idx, FW_ALIGN_BYTES - (size - fw_size));
            data = buf;
        }
        for (wdx = 0; wdx < 4; wdx++) {
            val = 0;
            for (bdx = 0; bdx < 4; bdx++) {
                val |= data[bdx + wdx * 4] << (bdx * 8);
            }
            CDPORT_TSC_UCMEM_DATAm_SET(cl_ucmem_data, wdx, val);
        }
        ucmem_idx = idx >> 4;
        if (ucmem_idx > ucmem_idx_max) {
            ucmem_idx = ucmem_idx % ucmem_idx_max;
        }
        WRITE_CDPORT_TSC_UCMEM_DATAm(unit, ucmem_idx, cl_ucmem_data, port);
    }

    /* Disable parallel bus access */
    access_mode = 0;
    ioerr += bcmbd_cmicx_reg_blocks_write(unit, blkacc, port, offset_tsc_mem, 0,
                                          &access_mode, 1);

    return ioerr ? SHR_E_ACCESS : rv;
}

static int
pm_tsce_firmware_load(int unit, bcmdrd_pbmp_t *pbmp)
{
    int port = 0;
    phymod_core_access_t core_access, *ca = &core_access;
    phymod_core_init_config_t core_init_config, *cic = &core_init_config;
    phymod_core_status_t core_status, *cs = &core_status;
    bcmpc_phy_access_data_t pc_phy_acc, *phy_data = &pc_phy_acc;
    bcmpc_pm_fw_cfg_t fw_cfg;
    int lane;
    uint32_t found = 0;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_fw_cfg_t_init(&fw_cfg);
    sal_memset(cic, 0, sizeof(*cic));
    SHR_IF_ERR_EXIT
        (bcmpc_pm_fw_load_config_get(unit, &fw_cfg));

    BCMDRD_PBMP_ITER(pbmp[1], port) {
        BCMPC_PHY_ACCESS_DATA_SET(phy_data, unit, port);
        bcmpc_phymod_core_access_t_init(unit, port, phy_data, ca);
        cic->firmware_loader = tsce_firmware_helper;
        SHR_IF_ERR_EXIT
            (bcmpc_phymod_fw_load_config_set(unit, &fw_cfg, cic));

        /* Probe phy */
        SHR_IF_ERR_EXIT
        (phymod_core_identify(ca, 0, &found));
        if (!found) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Fail to identify PHY core for "
                                  "port %d\n"), port));
        }

        cic->lane_map.num_of_lanes = 4;
        for (lane = 0; lane < 4; lane++) {
            cic->lane_map.lane_map_rx[lane] = lane;
            cic->lane_map.lane_map_tx[lane] = lane;
        }
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(cic);

        cs->pmd_active = 0;
        SHR_IF_ERR_EXIT
            (phymod_core_init(ca, cic, cs));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_info_update(int unit, int pm_id, bcmpc_topo_t *pm_info,
                            void *cookie)
{
    bcmpc_drv_t *pc = (bcmpc_drv_t *)cookie;
    int port = pm_info->base_pport;
    phymod_dispatch_type_t phy_type;

    phy_type = pc->phy_drv_get(unit, port);
    if ((phy_type == phymodDispatchTypeTscbh_gen2) ||
        (phy_type == phymodDispatchTypeTsce7)) {
        /* Firmware is downloaded on the PM. */
        (pm_info->pm_info->pm_data.pm_ha_info)->firmware_downloaded = 1;
    }

    return SHR_E_NONE;
}

static int
pm_port_pbmp_get(int unit, int pm_id, bcmpc_topo_t *pm_info, void *cookie)
{
    int port = pm_info->base_pport;
    bcmpc_drv_t *pc;
    bcmdrd_pbmp_t *tsc_pbmp = (bcmdrd_pbmp_t *)cookie;
    phymod_dispatch_type_t phy_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc));

    phy_type = pc->phy_drv_get(unit, port);
    if (phy_type == phymodDispatchTypeTscbh_gen2) {
         BCMDRD_PBMP_PORT_ADD(tsc_pbmp[0], port);
    } else if (phy_type == phymodDispatchTypeTsce7) {
         BCMDRD_PBMP_PORT_ADD(tsc_pbmp[1], port);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_tscbh_firmware_load(int unit, int *last_bcast_port, bcmdrd_pbmp_t *pbmp)
{
    int port = 0, bcast_port, phy_port, idx;
    phymod_core_access_t core_access, *ca = &core_access;
    phymod_core_init_config_t core_init_config, *cic = &core_init_config;
    phymod_core_status_t core_status, *cs = &core_status;
    bcmpc_phy_access_data_t pc_phy_acc, *phy_data = &pc_phy_acc;
    bcmpc_pm_fw_cfg_t fw_cfg;
    uint32_t found = 0;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(phy_data, unit, port);
    bcmpc_phymod_core_access_t_init(unit, port, phy_data, ca);
    bcmpc_pm_fw_cfg_t_init(&fw_cfg);

    SHR_IF_ERR_EXIT
        (bcmpc_pm_fw_load_config_get(unit, &fw_cfg));

    if (fw_cfg.fw_load == PM_SERDES_FW_LOAD_SLOW ) {
        BCMDRD_PBMP_ITER(pbmp[0], port) {
            BCMPC_PHY_ACCESS_DATA_SET(phy_data, unit, port);
            bcmpc_phymod_core_access_t_init(unit, port, phy_data, ca);
            sal_memset(cic, 0, sizeof(*cic));
            cic->firmware_loader = tscbh_firmware_helper;
            SHR_IF_ERR_EXIT
                (bcmpc_phymod_fw_load_config_set(unit, &fw_cfg, cic));
            PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(cic);

            /* Probe phy */
            SHR_IF_ERR_EXIT
                (phymod_core_identify(ca, 0, &found));
            if (!found) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Fail to identify PHY core for "
                                      "port %d\n"), port));
            }
            cs->pmd_active = 0;
            SHR_IF_ERR_EXIT
                (phymod_core_init(ca, cic, cs));
        }
    } else {
        for (idx = 0; idx < CDPORT_NUM_BCAST; idx++) {
            if (last_bcast_port[idx] > 0 ) {
                phy_port = last_bcast_port[idx];
                bcast_port = CDPORT_BCAST_PORT(idx);
                BCMPC_PHY_ACCESS_DATA_SET(phy_data, unit, bcast_port);
                bcmpc_phymod_core_access_t_init(unit, phy_port, phy_data, ca);

                sal_memset(cic, 0, sizeof(*cic));
                cic->firmware_loader = tscbh_firmware_helper;
                SHR_IF_ERR_EXIT
                    (bcmpc_phymod_fw_load_config_set(unit, &fw_cfg, cic));

                /* Probe phy */
                SHR_IF_ERR_EXIT
                    (phymod_core_identify(ca, 0, &found));
                if (!found) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Fail to identify PHY core for "
                                          "port %d\n"), port));
                }
                PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_SET(cic);
                PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(cic);
                cs->pmd_active = 0;
                SHR_IF_ERR_EXIT
                    (phymod_core_init(ca, cic, cs));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
cdport_last_bcast_port_get(bcmdrd_pbmp_t *pbmp, int *last_bcast_port)
{
    int port, bcast_domain;

    BCMDRD_PBMP_ITER(pbmp[0], port) {
        /* Get ring number that port belong to */
        bcast_domain = (port < CDPORT_BCST1_1ST_PORT)? 0 : 1;
        if (bcast_domain == 0) {
            /* Set last port number in the chain 0. */
            last_bcast_port[bcast_domain] = port;
        } else if (last_bcast_port[bcast_domain] < 0) {
            /* Set last port number in the chain 1. */
            last_bcast_port[bcast_domain] = port;
        }
    }
}

int
bcm56780_a0_phy_firmware_loader(int unit, int port, uint32_t fw_loader_req,
                                phymod_firmware_loader_f *fw_loader)
{
    bool real_hw = bcmdrd_feature_is_real_hw(unit);
    bcmpc_drv_t *pc;
    phymod_dispatch_type_t phy_type;
    bcmdrd_pbmp_t tsc_pbmp[2];
    int last_bcast_port[CDPORT_NUM_BCAST] = {-1, -1};

    SHR_FUNC_ENTER(unit);

    if (!real_hw) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc));

    if (!fw_loader_req) {
        /* TSCBH port bit map. */
        BCMDRD_PBMP_CLEAR(tsc_pbmp[0]);
        /* TSCE port bit map. */
        BCMDRD_PBMP_CLEAR(tsc_pbmp[1]);
        /* Get port bit map for each serdes core. */
        SHR_IF_ERR_EXIT
            (bcmpc_topo_traverse(unit, pm_port_pbmp_get, tsc_pbmp));
        /* Get last ports in the rings. */
        cdport_last_bcast_port_get(tsc_pbmp, last_bcast_port);
        /* Download firmware on TSCBH. */
        SHR_IF_ERR_EXIT
            (pm_tscbh_firmware_load(unit, last_bcast_port, tsc_pbmp));
        /* Download firmware on TSCE. */
        SHR_IF_ERR_EXIT
            (pm_tsce_firmware_load(unit, tsc_pbmp));
        /* Update PM firmware download status. */
        SHR_IF_ERR_EXIT
            (bcmpc_topo_traverse(unit, pm_info_update, pc));
   } else {
        phy_type = pc->phy_drv_get(unit, port);
        if (phy_type == phymodDispatchTypeTscbh_gen2) {
            *fw_loader = tscbh_firmware_helper;
        } else if (phy_type == phymodDispatchTypeTsce7) {
            *fw_loader = tsce_firmware_helper;
        }
   }

exit:
    SHR_FUNC_EXIT();
}
