/*! \file ser_lt_ser_injection_status.h
 *
 * Interface functions for SER_INJECTION_STATUS IMM LT
 *
 * Update data to cache of SER_INJECTION_STATUS
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_INJECTION_STATUS_H
#define SER_LT_SER_INJECTION_STATUS_H

/*!
 * \brief Update SER_INJECTION_STATUS table.
 *
 * Synchronize cache data for most recent SER injection status to
 * IMM component.
 *
 *
 * \param [in] unit          Unit number.
 * \param [in] insert        Select insert (1) or update (0) operation.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_injection_status_imm_update(int unit, int insert);

/*!
 * \brief Save value to cache of a field of LT SER_INJECTION_STATUS.
 *
 *
 * \param [in] unit          Unit number.
 * \param [in] fid_lt        Field id.
 * \param [in] data          Field data.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_injection_status_cache_update(int unit, uint32_t fid_lt, uint32_t data);

/*!
*\brief Used to save value to cache of every field of IMM LT SER_INJECTION_STATUS.
*/
/*! Update value to cache of field SER_INJECTION_STATUSt_PT_IDf */
#define  BCMPTM_SER_INJECTION_STATUS_PT_ID_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_PT_IDf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_PT_INSTANCEf */
#define  BCMPTM_SER_INJECTION_STATUS_PT_INSTANCE_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_PT_INSTANCEf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_PT_INSTANCEf */
#define  BCMPTM_SER_INJECTION_STATUS_PT_COPY_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_PT_COPYf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_PT_INDEXf */
#define  BCMPTM_SER_INJECTION_STATUS_PT_INDEX_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_PT_INDEXf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_XOR_BANKf */
#define  BCMPTM_SER_INJECTION_STATUS_XOR_BANK_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_XOR_BANKf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_SER_ERR_INJECTEDf */
#define  BCMPTM_SER_INJECTION_STATUS_SER_ERR_INJECTED_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_SER_ERR_INJECTEDf, _data)

/*! Update value to cache of field SER_INJECTION_STATUSt_SER_ERR_CORRECTEDf */
#define  BCMPTM_SER_INJECTION_STATUS_SER_ERR_CORRECTED_SET(_unit, _data) \
    (void)bcmptm_ser_injection_status_cache_update \
        (_unit, SER_INJECTION_STATUSt_SER_ERR_CORRECTEDf, _data)

#endif /* SER_LT_SER_INJECTION_STATUS_H */
