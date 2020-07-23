/*! \file bcmpc_serdes_config.c
 *
 *  BCMPC serdes firmware download configuration information get API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/bcmcfg_lt.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_INIT

/*! Logical table ID for PC_SEREDS_CONFIG. */
#define CFG_SID PC_SERDES_CONFIGt


/*******************************************************************************
 * Public functions
 */

void
bcmpc_pm_fw_cfg_t_init(bcmpc_pm_fw_cfg_t *fw_cfg)
{
    sal_memset(fw_cfg, 0, sizeof(*fw_cfg));
}

int
bcmpc_pm_fw_load_config_get(int unit, bcmpc_pm_fw_cfg_t *fw_cfg)
{
    uint64_t value = 0;
    bcmltd_fid_t field;

    SHR_FUNC_ENTER(unit);
    /*SHR_NULL_CHECK(fw_cfg, SHR_E_PARAM); */
    bcmpc_pm_fw_cfg_t_init(fw_cfg);

    field = PC_SERDES_CONFIGt_FW_CRC_VERIFYf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, CFG_SID, field, &value));
    fw_cfg->fw_crc_verify = value;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Firmware CRC verfiy: %s\n"),
         value ? "TRUE" : "FALSE"));

    field = PC_SERDES_CONFIGt_FW_LOAD_VERIFYf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, CFG_SID, field, &value));
    fw_cfg->fw_load_verify = value;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Firmware LOAD verfiy: %s\n"),
         value ? "TRUE" : "FALSE"));

    field = PC_SERDES_CONFIGt_FW_LOAD_METHODf;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, CFG_SID, field, &value));
    fw_cfg->fw_load = value;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Firmware load method: %s\n"),
         value ? "SLOW" : "FAST"));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phymod_fw_load_config_set(int unit, bcmpc_pm_fw_cfg_t *fw_cfg,
                            phymod_core_init_config_t *core_init_cfg)
{
     if (fw_cfg->fw_load == PM_SERDES_FW_LOAD_FAST) {
         core_init_cfg->firmware_load_method = phymodFirmwareLoadMethodExternal;
     } else {
         core_init_cfg->firmware_load_method = phymodFirmwareLoadMethodInternal;
     }
     if (!fw_cfg->fw_crc_verify) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(core_init_cfg);
     }
     if (fw_cfg->fw_load_verify) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(core_init_cfg);
     }

    return SHR_E_NONE;
}

