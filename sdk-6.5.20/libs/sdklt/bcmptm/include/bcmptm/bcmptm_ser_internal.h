/*! \file bcmptm_ser_internal.h
 *
 * SER interfaces which can be used by other components of PTM
 *
 * APIs defines for top level of SER sub-component
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_INTERNAL_H
#define BCMPTM_SER_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_ser_cth.h>

/*!
*\brief Used for second parameter of /ref bcmptm_pt_clear
*\n Value if the second parameter of this routine can be \ref PT_CLEAR_BEFORE_SER_ENABLE
*\n or \ref PT_CLEAR_AFTER_SER_ENABLE or \ref PT_CLEAR_ALL
*/

/*! PTs need to be cleared before enabling SER parity or ecc checking */
#define PT_CLEAR_BEFORE_SER_ENABLE    (1 << 0)

/*! PTs need to be cleared after enabling SER parity or ecc checking */
#define PT_CLEAR_AFTER_SER_ENABLE     (1 << 1)

/*! All PTs need to be cleared, don't care whether SER parity or ecc checking of PTs is enable. */
#define PT_CLEAR_ALL     (PT_CLEAR_BEFORE_SER_ENABLE | PT_CLEAR_AFTER_SER_ENABLE)

/*!\brief Handler to map SID reported by H/W to SID used to correct SER error. */
typedef int (*bcmptm_ser_alpm_info_get_f)(int unit, bcmdrd_sid_t hw_sid,
                                          bcmbd_pt_dyn_info_t hw_dyn_info,
                                          int in_count,
                                          bcmdrd_sid_t *out_sid,
                                          bcmbd_pt_dyn_info_t *out_dyn_info,
                                          int *out_count);

/*!
 * \brief RM-ALPM can register callback routine into SER logic. Then
 * SER logic calls the routine to get right SER correction data of ALPM PTs.
 *
 * SER can't call APIs defined in RM-ALPM directly.
 *
 * \param [in] unit Unit number
 * \param [in] mtop mtop indication
 * \param [in] alpm_info_get registered callback routine.
 *
 * \retval NONE
 */
extern void
bcmptm_ser_alpm_info_cb_reg(int unit, bool mtop, bcmptm_ser_alpm_info_get_f alpm_info_get);

/*!
 * \brief To check whether SER_LOG is full or not, if full,
 * the oldest entry will be overlaied by new log.
 *
 * \param [in] unit Unit number
 *
 * \retval TRUE or FALSE
 */
extern bool
bcmptm_ser_log_overlaid_get(int unit);

/*!
 * \brief To record SER_LOG status, if ovrlaid == 1, the oldest entry will
 * be overlaied by new log. If ovrlaid == 0, unoccupied entry will be used to
 * record SER log.
 *
 * \param [in] unit Unit number
 * \param [in] ovrlaid SER_LOG is full
 *
 * \retval NONE
 */
extern void
bcmptm_ser_log_overlaid_set(int unit, bool ovrlaid);

/*!
 * \brief Clear some PTs before or after enabling SER parity or ecc checking.
 * \n If some PTs are not cleared, those PTs can trigger SER interrupt by mistakes.
 *
 * \param [in] unit   Unit number
 * \param [in] flags  \ref PT_CLEAR_AFTER_SER_ENABLE or
 * \n                 \ref PT_CLEAR_BEFORE_SER_ENABLE or \ref PT_CLEAR_ALL
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_pt_clear(int unit, int flags);

/*!
 * \brief Enable parity or ecc checking of all memory tables and registers.
 * \n Clear memory tables and registers.
 * \n Create interrupt message queue.
 * \n Enable SER interrupts
 *
 * \param [in] unit   Unit number
 * \param [in] cold   TRUE => coldboot, FALSE => warmboot
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_comp_config(int unit, bool cold);

/*!
 * \brief Run SERC thread to correct SER error.
 * Run SRAM and TCAM scan threads to scan PTs.
 *
 * \param [in] unit    Unit number
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_start(int unit);

/*!
 * \brief Terminate SERC thread,
 * SRAM and TCAM scan thread.
 *
 * \param [in] unit    Unit number
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_serc_stop(int unit);

/*!
 * \brief Enable or disable parity or ecc check for a PT
 *
 *
 * \param [in] unit           Unit number
 * \param [in] sid            SID of register or memory
 * \param [in] ctrl_type      SER control information type
 * \param [in] enable         0: disable; 1: enable
 *
 *
 * width of each field must be less than 32 bits
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_ser_enable(int unit, bcmdrd_sid_t sid,
                         bcmdrd_ser_en_type_t ctrl_type, int enable);

/*!
 * \brief Write data to a entry of a given PT.
 *
 * Only can be used by SER component.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] mem_sid      PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     If tbl_copy < 0, Table instance ID, otherwise, base index.
 * \param [in] tbl_copy     Some PTs have two dimensions, used to index appointed instance.
 * \param [in] data         New data for entry
 * \param [in] flags        e.g: BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_pt_write(int unit, bcmdrd_sid_t  mem_sid,
                    int index, int tbl_inst, int tbl_copy,
                    uint32_t *data, int flags);

/*!
 * \brief Read data from a entry of a given PT.
 *
 * Only can be used by SER component.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] sid          PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     If tbl_copy < 0, Table instance, otherwise, base index.
 * \param [in] tbl_copy     Some PTs have two dimensions,
 *                          used to index appointed instance.
 * \param [out] data        Data buffer, saved data from SID
 * \param [in] wsize        Data buffer size
 * \param [in] flags        e.g: BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_pt_read(int unit, bcmdrd_sid_t  sid,
                   int index, int tbl_inst, int tbl_copy,
                   uint32_t *data, size_t wsize, int flags);

/*!
 * \brief Enable or disable SER checking for a TCAM
 *
 * \param [in] unit        Unit number
 * \param [in] sid         PT ID of a TCAM
 * \param [in] enable      1:enable, 0: disable
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_pt_ser_enable(int unit, bcmdrd_sid_t sid, int enable);

/*!
 * \brief Get instance number of a PT. For some PTs which access type is
 * duplicate and which table instance number is not equal to '1',
 * we need to get copy_num, in order to access all instance of such PTs.
 * For writing operation, we do not care about copy no, but for reading
 * operation, we should assign copy no as instance number.
 *
 * \param [in] unit        Unit number
 * \param [in] sid         PT ID.
 * \param [out] inst_num   If copy_num ==0, instance number of SID,
 *                         otherwise, number of instance per base-type.
 * \param [out] copy_num   Block instance number of SID.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tbl_inst_num_get(int unit, bcmdrd_sid_t sid, int *inst_num, int *copy_num);

/*!
 * \brief Check whether index is valid or not.
 *
 * For UFT tables, their max index can be changed
 * during running time.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          PT ID.
 * \param [in] index        Index needed to check
 * \param [in] max_index    Max index of SID, only meaningful when routine returns TRUE.
 *
 * \retval TRUE for valid index, FALSE for invalid index
 */
extern int
bcmptm_ser_sram_index_valid(int unit, bcmdrd_sid_t sid, int index, int *max_index);

/*!
 * \brief Modify multiple fields of all instances of a register,
 * and can return the old value of given fields.
 *
 * \param [in] unit                 Unit number
 * \param [in] reg                  PT ID
 * \param [in] index                Index of PT
 * \param [in] field_list           Field list of register or memory
 * \param [in] en                   All fields should be enabled or disabled.
 *                                  If new_fld_val_list != NULL, this parameter won't be used.
 * \param [in] new_fld_val_list     Values for each field, can be NULL.
 * \param [out] old_fld_val_list    Old values of each field, can be NULL.
 *
 * If 'val_list' is not NULL, set value from 'val_list' to each field,
 * if val_list is NULL, set parameter 'value' to each field
 *
 *\n Maximum width of each field must be less than 32 bits
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_reg_multi_fields_modify(int unit, bcmdrd_sid_t reg, int index,
                                   bcmdrd_fid_t *field_list, bool en,
                                   uint32_t *new_fld_val_list,
                                   uint32_t *old_fld_val_list);

/*!
 * \brief Start TCAM scan thread.
 *
 * \param [in] unit                        Unit number.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_scan_start(int unit);

/*!
 * \brief Stop TCAM scan thread.
 *
 * \param [in] unit                    Unit number.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_scan_stop(int unit);

/*!
 * \brief Start SRAM scan thread.
 *
 * \param [in] unit                   Unit number.
 * \param [in] beginning_sid          SRAM thread scan PTs from this SID.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sram_scan_start(int unit, bcmdrd_sid_t beginning_sid);

/*!
 * \brief Stop SRAM scan thread.
 *
 * \param [in] unit                    Unit number.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sram_scan_stop(int unit);

/*!
 * \brief Register routines in file cth/ser, ser needs to use them.
 * Routines in 'ser' can't call routines in 'cth/ser' directly.
 *
 * \param [in] cth_drv  Structure of callback routines.
 *
 * \retval  NONE
 */
extern void
bcmptm_ser_cth_drv_register(bcmptm_ser_cth_drv_t *cth_drv);

/*!
 * \brief Record depth of the SER message queue.
 *
 * The value can be defined by LT SER_CONFIG.
 *
 * \param [in] unit               Unit number.
 * \param [in] length             Length of interrupt message queue.
 *
 * \retval  NONE
 */
extern void
bcmptm_ser_intr_msg_q_length_set(int unit, uint32_t length);

/*!
 * \brief Create the SER message queue.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_intr_msg_q_create(int unit);

/*!
 * \brief Destroy the SER message queue.
 *
 * \param [in] unit Unit number
 *
 * \retval  NONE
 */
extern void
bcmptm_ser_intr_msg_q_destroy(int unit);

/*!
 * \brief Wake up SRAM scan thread.
 *
 * \param [in] unit     Unit number.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_sram_scan_resume(int unit);

/*!
 * \brief Allocate HA memory space used to save data for SER logical.
 *
 * \param [in] unit  Unit number
 * \param [in] warm  1: Warm boot, 0: cold boot
 *
 * \retval SHR_E_NONE for Success
 */
extern int
bcmptm_ser_ha_info_init(int unit, bool warm);


/*!
 * \brief Check whether table instance and base index is valid
 * or not for a given PT.
 *
 * \param [in] unit       Unit number
 * \param [in] sid        PT ID
 * \param [in] tbl_inst   If copy_no < 0, table instance number,
 *                        otherwise, table base index.
 * \param [in] copy_no    Table instance number.
 *
 * \retval SHR_E_NONE for Success
 */
extern bool
bcmptm_ser_pt_copy_no_valid(int unit, bcmdrd_sid_t sid,
                            int tbl_inst, int copy_no);

/*!
 * \brief  Disable or enable "H/W refresh".
 *
 * During SER injection and validation, "H/W refresh" should be disabled.
 * Otherwise, the injected SER error will be triggered by H/W.
 *
 * \param [in] unit      Unit number.
 * \param [in] disable Disable H/W refresh.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_refresh_regs_config(int unit, int disable);

/*!
 * \brief For some PTs reported by H/W, there is no PTcache.
 * So SER logic will map the reported view of the PTs to the view which has PTcache.
 *
 * \param [in] unit Unit number.
 * \param [in] sid PT view which does not have PTcache.
 * \param [in] sid_map PT view which has PTcache.
 * \param [in] index_map Index of PT view which has PTcache.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_sid_remap(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *sid_map, int *index_map);

/*!
 * \brief Enable or disable 1bit auto correction for TCAM.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: enable, 0: disable.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_tcam_single_bit_auto_correct_enable(int unit, int enable);

#endif /* BCMPTM_SER_INTERNAL_H */
