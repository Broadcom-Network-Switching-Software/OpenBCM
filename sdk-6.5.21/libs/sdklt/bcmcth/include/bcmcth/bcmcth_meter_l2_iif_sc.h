/*! \file bcmcth_meter_l2_iif_sc.h
 *
 * This file contains storm control meter custom handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_L2_IIF_SC_H
#define BCMCTH_METER_L2_IIF_SC_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc_drv.h>

/*! Convert LT fields into HW fields. */
typedef int (*meter_l2_iif_sc_compute_hw_param_f)(int unit,
                                                  bcmcth_meter_l2_iif_sc_entry_t *entry);

/*! Storm control meter driver structure. */
typedef struct bcmcth_meter_l2_iif_sc_drv_s {
    /*! Config table ID. */
    uint32_t config_ptid;
    /*! Byte mode field ID. */
    uint32_t byte_fid;
    /*! Quantum field ID. */
    uint32_t quantum_fid;
    /*! Meter table ID. */
    uint32_t meter_ptid;
    /*! Rerfresh count field ID. */
    uint32_t rc_fid;
    /*! Bucket count field ID. */
    uint32_t bc_fid;
    /*! Bucket size field ID. */
    uint32_t bs_fid;
    /*! Number of offsets per meter ID. */
    uint32_t num_offset;
    /*! Convert LT fields to HW fields. */
    meter_l2_iif_sc_compute_hw_param_f compute_hw_param;
} bcmcth_meter_l2_iif_sc_drv_t;

/*! Macro for storm control meter driver. */
#define METER_L2_IIF_SC_DRV(unit)    meter_l2_iif_sc_drv[unit]

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_meter_l2_iif_sc_drv_t *_bc##_cth_meter_l2_iif_sc_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Packet quantum information structure.
 *
 * Metering rate and burst size details at each quantum level.
 */
typedef struct bcmcth_meter_l2_iif_sc_pkt_quantum_info_s {
    /*! Packet quantum value at this quantum level. */
    uint32_t    quantum;
    /*! Minimum metering rate at this quantum level. */
    uint32_t    min_rate;
    /*! Maximum metering rate at this quantum level. */
    uint32_t    max_rate;
} bcmcth_meter_l2_iif_sc_pkt_quantum_info_t;

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_l2_iif_sc_drv_init(int unit);

/*!
 * \brief Cleanup device driver.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_meter_l2_iif_sc_drv_cleanup(int unit);

/*!
 * \brief Register storm control meter with IMM.
 *
 * Register with IMM to receive notifications for
 * storm control meter LT changes.
 *
 * \param [in] unit Unit number.
 */
extern int bcmcth_meter_l2_iif_sc_imm_register(int unit);

/*!
 * \brief Cleanup storm control meter IMM DB.
 *
 * \param [in] unit Unit number.
 */
extern void bcmcth_meter_l2_iif_sc_imm_db_cleanup(int unit);

/*!
 * \brief Get pointer to L2 IFF storm control meter driver structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure pointer
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_meter_l2_iif_sc_drv_get(int unit,
                               bcmcth_meter_l2_iif_sc_drv_t **drv);
/*!
 * \brief Get storm control meter field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_meter_l2_iif_sc_arr_hdl_get(int unit,
                                   shr_famm_hdl_t *hdl);

#endif /* BCMCTH_METER_L2_IIF_SC_H */
