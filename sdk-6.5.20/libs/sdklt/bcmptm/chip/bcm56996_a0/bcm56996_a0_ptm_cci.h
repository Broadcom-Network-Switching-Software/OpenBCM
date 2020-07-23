/*! \file bcm56996_a0_ptm_cci.h
 *
 * APIs of bcm56996_a0_ptm_cci.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_PTM_CCI_H
#define BCM56996_A0_PTM_CCI_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cci_internal.h>

/*!
* Get counter Type
*/
extern int
bcm56996_a0_pt_cci_ctrtype_get(int unit,
                               bcmdrd_sid_t sid);

/*!
 * Check if Symbol is in bypass mode.
 */
extern bool
bcm56996_a0_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid);

/*!
* Map the counter to appropriate symbol.
*/
extern int
bcm56996_a0_pt_cci_ctr_sym_map(int unit,
                               bcmptm_cci_ctr_info_t *ctr_info);

/*!
* Get format symbol ID if exist.
*/
extern int
bcm56996_a0_pt_cci_frmt_sym_get(int unit, int index, int field,
                                bcmdrd_sid_t *sid, size_t *width);

/*!
* Get index range in formatted table for given port.
*/
extern int
bcm56996_a0_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                        int *index_min, int *index_max);

/*!
* Check if physical table index is valid.
*/
extern bool
bcm56996_a0_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);

/*!
 * Get register address gap by specify the symbol ID
 */
extern uint32_t
bcm56996_a0_pt_cci_reg_addr_gap_get(int unit, bcmdrd_sid_t sid);

/*!
 * Get eviction counter control register.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_control_get(int unit, bcmdrd_sid_t sid,
                                         bcmdrd_sid_t *ctrl_sid);

/*!
 * Get Eviction counter enable field.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_enable_field_get(int unit,
                                              bcmdrd_fid_t *fid);

/*!
 * Get Eviction counter clr on read field.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_clr_on_read_field_get(int unit,
                                                   bcmdrd_fid_t *fid);

/*!
 * Get list of eviction polls which will be enabled by default
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_default_enable_list_get(int unit,
                                                     size_t *size,
                                                     bcmdrd_sid_t *sid);

/*!
 * Get central eviction FIFO enable register info.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_fifo_enable_sym_get(int unit,
                                                 bcmdrd_sid_t *sid,
                                                 bcmdrd_fid_t *fid);

/*!
 * Get FIFO DMA channel.
 */
int
bcm56996_a0_pt_cci_fifodma_chan_get(int unit);

/*!
 * Get Eviction FIFO Symbol
 */
int
bcm56996_a0_pt_cci_ctr_evict_fifo_sym_get(int unit, bcmdrd_sid_t *sid);

/*!
 * Get size (max number of entires) in eviction fifo buffer.
 */
int
bcm56996_a0_pt_cci_ctr_evict_fifo_entry_get(int unit,
                                            uint32_t *evict,
                                            uint32_t *pool,
                                            bcmptm_cci_ctr_info_t *info);

/*!
 * Get size (max number of entires) in eviction fifo buffer.
 */
extern uint32_t
bcm56996_a0_pt_cci_ctr_evict_fifo_buf_size_get(int unit);

/*!
 * Get field number for specified counter modes.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_field_num_get(int unit,
                                           bcmptm_cci_ctr_mode_t ctr_mode,
                                           size_t *num);

/*!
 * Get field width for specified counter modes.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_field_width_get(int unit,
                                             bcmptm_cci_ctr_mode_t ctr_mode,
                                             size_t *width);

/*!
 * Get field values for normal counter mode.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_normal_val_get(int unit,
                                            size_t fld_width,
                                            uint32_t *entry,
                                            size_t size_e,
                                            uint64_t *ctr,
                                            size_t size_ctr);

/*!
 * Get field values for wide counter mode.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_wide_val_get(int unit,
                                          size_t fld_width,
                                          uint32_t *entry,
                                          size_t size_e,
                                          uint64_t *ctr,
                                          size_t size_ctr);

/*!
 * Get field values for slim counter mode.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_slim_val_get(int unit,
                                          size_t fld_width,
                                          uint32_t *entry,
                                          size_t size_e,
                                          uint32_t *ctr,
                                          size_t size_ctr);

/*!
 * Get number of counter update modes.
 */
extern int
bcm56996_a0_pt_cci_ctr_evict_update_mode_num_get(int unit,
                                                 size_t *num);
#endif /* BCM56996_A0_PTM_CCI_H */
