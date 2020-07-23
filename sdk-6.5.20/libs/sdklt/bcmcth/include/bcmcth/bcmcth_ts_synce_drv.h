/*! \file bcmcth_ts_synce_drv.h
 *
 * BCMCTH TS SYNCE driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_SYNCE_DRV_H
#define BCMCTH_TS_SYNCE_DRV_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <shr/shr_bitop.h>
#include <bcmcth/generated/ts_synce_ha.h>

/*! Max SyncE clk type */
#define SYNCE_MAX_CLK_TYPE      SYNCE_CLK_TYPE_MAX

/*! SyncE invalid pm port */
#define SYNCE_INVALID_PM_PORT   -1

/*! SyncE clock control data structure */
typedef struct ts_synce_ctrl_s {
    /*! Port ID */
    shr_port_t port;
    /*! Clock divisor */
    uint16_t clk_divisor;
    /*! Valid status in override mode */
    bool ovr_valid;
    /*! Portmacro port */
    int pm_port;
    /*! Indicate to update config to HW */
    bool update;
    /*! Clock instance */
    int instance;
} ts_synce_ctrl_t;

/*! SyncE clock control lt entry operational value */
typedef struct ts_synce_ctrl_entry_oper_s {
    /*! Oper value of port ID. */
    shr_port_t port[SYNCE_MAX_CLK_TYPE];
    /*! Oper value of clock divisor. */
    int clk_divisor[SYNCE_MAX_CLK_TYPE];
    /*! Oper value of override mode. */
    bool override;
    /*! Oper value of valid status in override mode */
    bool ovr_valid[SYNCE_MAX_CLK_TYPE];
    /*! Operational state */
    int oper_stat[SYNCE_MAX_CLK_TYPE];
} ts_synce_ctrl_entry_oper_t;

/*! Synce clock control lt entry */
typedef struct ts_synce_ctrl_entry_s {
    /*! The logical table ID */
    bcmltd_sid_t ltid;
    /*! The operation arguments */
    const bcmltd_op_arg_t *op_arg;
    /*! LT out counts */
    int out_counts;
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(
        fbmp,
        DEVICE_TS_SYNCE_CLK_CONTROLt_FIELD_COUNT * SYNCE_MAX_CLK_TYPE);
    /*! Enable clock configure. */
    bool clk_recovery[SYNCE_MAX_CLK_TYPE];
    /*! Logical port ID. */
    shr_port_t port[SYNCE_MAX_CLK_TYPE];
    /*! Clock divisor. */
    int clk_divisor[SYNCE_MAX_CLK_TYPE];
    /*! Enable override. */
    bool override;
    /*! Valid status in override mode. */
    bool ovr_valid[SYNCE_MAX_CLK_TYPE];
} ts_synce_ctrl_entry_t;

/*!
 * \brief Enable/disable SyncE clk recovery config
 *
 * This device specific function is called by the IMM callback handler
 * to update hardware registers to enable/disable SyncE clock.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg The operation argument.
 * \param [in] inst_map The instance bitmap.
 * \param [in] enable Enable/disable SyncE clock.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*ts_synce_ctrl_enable) (
    int unit,
    bcmltd_sid_t ltid,
    const bcmltd_op_arg_t *op_arg,
    uint8_t inst_map,
    bool enable);

/*!
 * \brief Update SyncE clk recovery config
 *
 * This device specific function is called by the IMM callback handler
 * to update hardware configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg The operation argument.
 * \param [in] ctrl_num Numbers of SyncE ctrl configuration.
 * \param [in] ctrl SyncE configuration.
 * \param [in] override Enable override of SyncE clock recovery.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*ts_synce_ctrl_update) (
    int unit,
    bcmltd_sid_t ltid,
    const bcmltd_op_arg_t *op_arg,
    int ctrl_num,
    ts_synce_ctrl_t *ctrl,
    bool override);

/*!
 * \brief Validate the SyncE configuration.
 *
 * This device specific function is called by the IMM callback handler
 * to validate the SyncE configurations.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ctrl SyncE configuration. The values would be updated
 *                      after validation.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*ts_synce_ctrl_validate) (int unit, ts_synce_ctrl_t *ctrl);

/*!
 * \brief TS SyncE driver object
 *
 * TS SyncE driver is used to provide the chip specific information.
 */
typedef struct bcmcth_ts_synce_drv_s {
    /*! Enable/disable SyncE clk recovery config */
    ts_synce_ctrl_enable synce_ctrl_enable;
    /*! Update SyncE clk recovery config */
    ts_synce_ctrl_update synce_ctrl_update;
    /*! Validate SyncE clk recovery config. */
    ts_synce_ctrl_validate synce_ctrl_validate;
} bcmcth_ts_synce_drv_t;

/*!
 * \brief Get SyncE driver APIs.
 *
 * This function returns a structure which contains the device specific APIs
 * for SyncE.
 *
 * \param [in] unit Unit number.
 *
 * \return SyncE driver API structure
 */
extern bcmcth_ts_synce_drv_t *
bcmcth_ts_synce_drv_get(int unit);

/*!
 * \brief Register IMM callbacks for SyncE.
 *
 * The TS SyncE custom table handling is done via the IMM component.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_synce_imm_register(int unit);

/*!
 * \brief SyncE control LT operation function
 *
 * This function handles the operations for DEVICE_TS_SYNCE_CLK_CONTROL
 * to update HW according to the content of entry and reflect the
 * corresponding operational values.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Configuration values of the entry.
 * \param [out] oper Operational values of the entry.
 */
extern int
bcmcth_ts_synce_control_update(int unit,
                               ts_synce_ctrl_entry_t *entry,
                               ts_synce_ctrl_entry_oper_t *oper);

/*!
 * \brief SyncE resource alloc function
 *
 * Allocates global data needed for SyncE
 *
 * \param [in] unit Unit number.
 * \param [in] warm warmboot or not.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_ts_synce_sw_resources_alloc(int unit, bool warm);

/*!
 * \brief Free memory used by the SyncE.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmcth_ts_synce_free(int unit);

#endif /* BCMCTH_TS_SYNCE_DRV_H */
