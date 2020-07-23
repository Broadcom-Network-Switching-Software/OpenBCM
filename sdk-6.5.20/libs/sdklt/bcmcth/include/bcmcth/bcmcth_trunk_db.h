/*! \file bcmcth_trunk_db.h
 *
 * TRUNK databse function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_DB_H
#define BCMCTH_TRUNK_DB_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_trunk_types.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmcth/bcmcth_trunk_util.h>

/*!
 * \brief Initialize trunk group info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or Coldboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_trunk_grp_init(int unit, bool warm);

/*!
 * \brief Cleanup all trunk group info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * It always return success.
 */
extern int
bcmcth_trunk_grp_cleanup(int unit);

/*!
 * \brief Initialize trunk_fast group info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or Coldboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_trunk_fast_grp_init(int unit, bool warm);

/*!
 * \brief Cleanup trunk_fast group info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * It always return success.
 */
extern int
bcmcth_trunk_fast_grp_cleanup(int unit);

/*!
 * \brief Check trunk is in normal mode or in fast trunk mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE in FAST TRUNK mode.
 * \retval FALSE in normal TRUNK mode.
 */
extern bool
bcmcth_trunk_fast_check(int unit);

/*!
 * \brief Stub function to check whether a port is local.
 *
 * \param [in] unit Unit number.
 * \param [in] modid Module identifier.
 * \param [in] modport Modport identifier.
 *
 * \retval TRUE port is local.
 * \retval FALSE port is not local.
 */
extern bool
bcmcth_trunk_localport_check(int unit, uint8_t modid, uint8_t modport);

/*!
 * \brief Initialize trunk failover bookeeping info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or Coldboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_trunk_failover_init(int unit, bool warm);

/*!
 * \brief Cleanup trunk_failover bookkeeping info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 *
 * It always return success.
 */
extern int
bcmcth_trunk_failover_cleanup(int unit);

/*!
 * \brief TRUNK component init.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or coldboot.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_init(int unit, bool warm);

/*!
 * \brief TRUNK component cleanup.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_cleanup(int unit);

/*!
 * \brief trunk table fields parse
 *
 * \param [in]  unit          Unit Number.
 * \param [in] in             Input field values.
p* \param [out]  param        Returned group parameter.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_trunk_field_parse(int unit,
                         const bcmltd_fields_t *in,
                         bcmcth_trunk_group_param_t **param);


/*!
 * \brief Tranform the normal trunk group to LT fields.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Trunk group pointer.
 * \param [out] out Output LT fields to LTA.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmcth_trunk_grp_to_api(int unit,
                        bcmcth_trunk_group_t *grp,
                        bcmltd_fields_t *out);

/*!
 * \brief Initialize sytem trunk group info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot or Coldboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_trunk_imm_sys_grp_init(int unit, bool warm);

/*!
 * \brief Cleanup all system trunk group info.
 *
 * \param [in] unit Unit number.
 * \param [in] ha_free Flag to clean the HA memory when true.
 *
 * \retval SHR_E_NONE No errors.
 *
 * It always return success.
 */
extern int
bcmcth_trunk_imm_sys_grp_cleanup(int unit, bool ha_free);

/*!
 * \brief TRUNK LT imm initialization.
 *
 * \param [in] unit Unit number.
 * \param [warm] warm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
extern int
bcmcth_trunk_imm_grp_init(int unit, bool warm);

/*!
 * \brief TRUNK LT imm cleanup.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] ha_free Flag to clean the HA memory when true.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
extern int
bcmcth_trunk_imm_grp_cleanup(int unit, bool ha_free);

#endif /* BCMCTH_TRUNK_DB_H */
