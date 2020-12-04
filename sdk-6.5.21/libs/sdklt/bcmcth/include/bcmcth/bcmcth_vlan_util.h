/*! \file bcmcth_vlan_util.h
 *
 * VLAN component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_VLAN_UTIL_H
#define BCMCTH_VLAN_UTIL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Generic hw mem table write for BCMVLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_vlan_mreq_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw mem table read for BCMVLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_vlan_mreq_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                      bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interrupt read function for VLAN component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register/Memory identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_vlan_ireq_read(int unit, bcmltd_sid_t lt_id,
                      bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interrupt write function for VLAN component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register/Memory identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_vlan_ireq_write(int unit, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

#endif /* BCMCTH_VLAN_UTIL_H */
