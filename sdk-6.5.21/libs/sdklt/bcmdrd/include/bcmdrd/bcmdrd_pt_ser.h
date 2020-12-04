/*! \file bcmdrd_pt_ser.h
 *
 * DRD PT interface for SER functions.
 *
 * Soft Error Recovery (SER) is supported in some memories, which
 * might be physical tables (PTs) or internal buffers that can not
 * be accessed from the host CPU.
 *
 * The SER handling is beyond the DRD scope, but usually the process will be:
 * 1. SER Enable
 * 2. SER Detection
 * 3. SER Correction
 *
 * The DRD PT SER API provides the relevant data for SER handling.
 *
 * For example, a specific register/field needs to be set to enable
 * the SER for a specific memory. The DRD PT SER API provides such enable
 * control information for each specific memory.
 * The SER detection/correction mechanism might be different depending
 * on the memory type or other related information, and the DRD PT SER API
 * provides the required information for this as well.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_PT_SER_H
#define BCMDRD_PT_SER_H

#include <bcmdrd/bcmdrd_ser.h>

/*!
 * The SER enable control (usually a register and a field specified
 * by symbol ID and field ID) returned from \ref bcmdrd_pt_ser_en_ctrl_get
 * or \ref bcmdrd_pt_ser_rmr_en_ctrl_get.
 */
typedef struct bcmdrd_pt_ser_en_ctrl_s {

    /*! Symbol ID to enable SER protection. */
    bcmdrd_sid_t sid;

    /*! Field ID to enable SER protection. */
    bcmdrd_fid_t fid;

    /*!
     * Field mask of valid field bits for \c fid if non-zero.
     * Field mask should be ignored if \c fmask is zero.
     */
    uint32_t fmask;

} bcmdrd_pt_ser_en_ctrl_t;

/*!
 * The SER information returned from \ref bcmdrd_pt_ser_info_get
 * or \ref bcmdrd_pt_ser_rmr_info_get.
 */
typedef struct bcmdrd_pt_ser_info_s {

    /*! SER type. */
    bcmdrd_ser_type_t type;

    /*!
     * Number of banks for \c type with XOR attrbute.
     * e.g. BCMDRD_SER_TYPE_DIRECT_XOR or BCMDRD_SER_TYPE_HASH_XOR.
     */
    int num_xor_banks;

    /*!
     * Whether packet is dropped if error is detected
     * for packet processing.
     */
    bool drop_pkt;

    /*! SER response type. */
    bcmdrd_ser_resp_t resp;

} bcmdrd_pt_ser_info_t;

/*!
 * \brief Get SER enable control for a physical table.
 *
 * Error correction must be enabled specifically for each physical table
 * that supports it, and this must be done through a separate set of
 * registers. This function will return the information of register and
 * field which enables error correction for a given physical table \c sid.
 *
 * In addition to the basic SER enable functionality, some tables also
 * support various error reporting modes. The register-fields to configure
 * this can also be retrieved using this function.
 * Please refer to \ref bcmdrd_ser_en_type_t for a list of supported SER
 * enable types.
 *
 * \param [in] unit Unit number.
 * \param [in] type SER enable type.
 * \param [in] sid Symbol ID.
 * \param [out] en_ctrl Returned SER enable control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No control information is associated with this table.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_en_ctrl_get(int unit,
                          bcmdrd_ser_en_type_t type,
                          bcmdrd_sid_t sid,
                          bcmdrd_pt_ser_en_ctrl_t *en_ctrl);

/*!
 * \brief Get SER packet drop control information for a physical table.
 *
 * Some physical tables are able to configure whether packet is dropped if
 * an error is detected for packet processing. This function provides the
 * registers-fields information for tables associated with such controls.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] en_ctrl Returned SER enable control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No control information is associated with this table.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_drop_pkt_ctrl_get(int unit, bcmdrd_sid_t sid,
                                bcmdrd_pt_ser_en_ctrl_t *en_ctrl);

/*!
 * \brief Get SER information for a physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] info Returned SER information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No SER information is associated with this table.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_info_get(int unit, bcmdrd_sid_t sid,
                       bcmdrd_pt_ser_info_t *info);

/*!
 * \brief Get list of all restricted memory resources with SER protection.
 *
 * A restricted memory resource (RMR) is an internal buffer which is
 * not directly visible to the host CPU, i.e. the contents of a restricted
 * memory resource cannot be accessed from the host CPU.
 *
 * Each SER-protected RMR is assigned a unique ID for SER processing purposes,
 * and a list of these IDs can be obtained through this function.
 *
 * The function will always return the total number of RMR IDs in \c num_rid,
 * irrespective of the value of \c list_max, i.e. if \c num_rid is greater
 * than \c list_max, then the returned \c rid_list was truncated.
 *
 * For example, if \c list_max is zero, then the number of valid RMR IDs is
 * returned in \c num_rid, but the \c rid_list is not updated (can be
 * specified as NULL). The returned \c num_rid can then be used to allocate
 * a sufficiently large \c rid_list array.
 *
 * \param [in] unit Unit number.
 * \param [in] list_max Maximum number of entries in \c rid_list
 * \param [out] rid_list List of RMR IDs.
 * \param [out] num_rid Total number of RMRs with SER protection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No RMR information is associated with this unit.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_rmr_id_list_get(int unit, size_t list_max,
                              bcmdrd_ser_rmr_id_t *rid_list, size_t *num_rid);

/*!
 * \brief Get SER enable control for a restricted memory resource.
 *
 * Error correction must be enabled specifically for each
 * restricted memory resource (RMR) that supports it, and
 * this must be done through a separate set of registers.
 * This function will return the information of register and field which
 * enables error correction for a given RMR.
 *
 * In addition to the basic SER enable functionality, some RMRs
 * also support various error reporting modes. The register-fields to
 * configure this can also be retrieved using this function. Please refer to
 * \ref bcmdrd_ser_en_type_t for a list of supported SER enable types.
 *
 * \param [in] unit Unit number.
 * \param [in] type SER enable type.
 * \param [in] rid Restricted memory resource ID.
 * \param [out] en_ctrl Returned SER enable control.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No control information is associated with this RMR.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_rmr_en_ctrl_get(int unit,
                              bcmdrd_ser_en_type_t type,
                              bcmdrd_ser_rmr_id_t rid,
                              bcmdrd_pt_ser_en_ctrl_t *en_ctrl);

/*!
 * \brief Get SER information for a restricted memory resource.
 *
 * \param [in] unit Unit number.
 * \param [in] rid Restricted memory resource ID.
 * \param [out] info Returned SER information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No SER information is associated with this RMR.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid input parameters.
 */
extern int
bcmdrd_pt_ser_rmr_info_get(int unit, bcmdrd_ser_rmr_id_t rid,
                           bcmdrd_pt_ser_info_t *info);

/*!
 * \brief Get restricted memory resource (RMR) ID from RMR name.
 *
 * \param [in] unit Unit number.
 * \param [in] name Restricted memory resource name.
 * \param [out] rid Restricted memory resource ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL No SER information is associated with this unit.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND RMR name not found
 */
extern int
bcmdrd_pt_ser_name_to_rmr(int unit, const char *name,
                          bcmdrd_ser_rmr_id_t *rid);

/*!
 * \brief Get restricted memory resource (RMR) name from RMR ID.
 *
 * \param [in] unit Unit number.
 * \param [in] rid Restricted memory resource ID.
 *
 * \retval Pointer to RMR name, or NULL if not found.
 */
extern const char *
bcmdrd_pt_ser_rmr_to_name(int unit, bcmdrd_ser_rmr_id_t rid);

#endif /* BCMDRD_PT_SER_H */
