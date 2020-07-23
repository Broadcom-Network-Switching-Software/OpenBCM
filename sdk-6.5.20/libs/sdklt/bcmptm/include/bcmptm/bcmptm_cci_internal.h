/*! \file bcmptm_cci_internal.h
 *
 * Counter Collection Infrastructure(CCI)  interfaces
 *
 * This file contains APIs, defines for CCI interfaces Used by PTM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CCI_INTERNAL_H
#define BCMPTM_CCI_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cci.h>

/*!
 * \brief Eviction counters enabled by default.
 */
#define CTR_EVICT_ENABLE_DEFAULT_MAX 2

/*!
 * \brief SER request information to do counters correction
 */
typedef struct bcmptm_cci_ser_req_info_s {
    bcmdrd_sid_t        sid;        /* Counter SID */
    bcmbd_pt_dyn_info_t dyn_info;   /* Dynamic info associated with SID */
} bcmptm_cci_ser_req_info_t;

/******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize CCI (initialize and allocate resources  etc)
 * \n Called during System Manager INIT state for each unit.
 *
 * \param [in] unit Logical device id.
 * \param [in] warm  warm boot.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_init(int unit,
                    bool warm);

/*!
 * \brief Register counter with CCI.
 * \n Called during System Manager CONFIGURE state for each unit.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum to specify reg, mem.
 * \param [out] map_id, id in counter map table.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_ctr_reg(int unit,
                    bcmdrd_sid_t sid,
                    uint32_t *map_id);


/*!
 * \brief  Configure CCI
 * \n Called during System Manager CONFIG state for each unit.
 *
 * \param [in] unit Logical device id.
 * \param [in] warm  warm boot.
 * \param [in] phase configuration phase (1,2,3).
 *
 * Phase 1: CCI will determine the size of Counter.
 * Map Table and Counter Table in HA sace.
 * Phase 2: CCI will initialize  Counter Map Table.
 * Phase 3: CCI set up  the CCI Configuration information.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_comp_config(int unit,
                       bool warm,
                       bcmptm_sub_phase_t phase);


/*!
 * \brief Allocate DMA resources, configure Hardware, Start threads.
 * \n Called during System Manager RUN state for each unit.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_run(int unit);

/*!
 * \brief Read Dynamic fileds of Counter SID.
 *
 * \param [in] unit Logical device id.
 * \param [in] info counter Info.
 * \param [out] entry_words, counter value.
 * \param [in] entry_size, buffer size.
 * \pRead from Counter Cache.
 *
 * \LT:           Counter only
 * \PYS SID:    Counter (Counter Fileds)
 * \Buffer:       64 bit counts
 * \CCI Cache: DONT_USE_CACHE=0
 *                  No Change
 *                  DONT_USE_CACHE=1
 *                  Increment with value read from HW
 * \PT Cache:  No Change
 * \HW:          DONT_USE_CACHE=0
 *                  No op
 *                  DONT_USE_CACHE=1
 *                  Issue Read to HW.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_read(int unit,
                    bcmptm_cci_ctr_info_t *info,
                    uint32_t *rsp_entry_words,
                    size_t   rsp_entry_size);

/*!
 * \brief Read Configuration fields Counter HW Entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] info counter Info.
 * \param [out] entry_words, counter value.
 * \param [in] entry_size, buffer size.
 *
 * \LT:            Config fields
 * \PYS SID:    Counter (Config Fileds)
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue Read
 * \HW:          Issue read from HW if PTcache read was not successful.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_ctr_config_read(int unit,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *rsp_entry_words,
                           size_t   rsp_entry_size);

/*!
 * \brief Read Counter HW Entry in pass through mode.
 *
 * \param [in] unit Logical device id.
 * \param [in] info counter Info.
 * \param [out] entry_words, counter value.
 * \param [in] entry_size, buffer size.
 *
 * \LT:            Physial COunter Table
 * \PYS SID:    Physial COunter Table
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue Read
 * \HW:          Issue read from HW if PTcache read was not successful.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_passthru_read(int unit,
                             bcmptm_cci_ctr_info_t *info,
                             uint32_t *rsp_entry_words,
                             size_t   rsp_entry_size);

/*!
 * \brief Write Counter value.
 *
 * \param [in] unit Logical device id.
 * \param [in] counter Info.
 * \param [in] entry_words, counter value.
 *
 * \LT:           Counter only
 * \PYS SID:    Counter (Counter Fileds)
 * \Buffer:       64 bit counts
 * \CCI Cache: Will be updated to user-provided value
 * \PT Cache:  No Change
 * \HW:          Zero out counter-only fields by doing Read-modify-Write
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_ctr_write(int unit,
                     bcmptm_cci_ctr_info_t *info,
                     uint32_t *entry_words);

/*!
 * \brief Write Configuration fields Counter HW Entry.
 *
 * \param [in] unit Logical device id.
 * \param [in] counter Info.
 * \param [in] entry_words, counter value.
 *
 * \LT:            Config fields
 * \PYS SID:    Counter (Config Fileds)
 * \Buffer:       HW_entry format
 * \CCI Cache: Clear CCI cache
 * \PT Cache:  Issue update
 * \HW:          Directly write entry (no read-modify-write).
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_config_write(int unit,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *entry_words);

/*!
 * \brief Write Counter HW Entry in pass through mode.
 *
 * \param [in] unit Logical device id.
 * \param [in] counter Info.
 * \param [in] entry_words, counter value.
 *
 * \LT:            Physial Counter Table
 * \PYS SID:    Physial COunter Table
 * \Buffer:       HW_entry format
 * \CCI Cache: No Change
 * \PT Cache:  Issue update
 * \HW:          Issue Write
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_ctr_passthru_write(int unit,
                              bcmptm_cci_ctr_info_t *info,
                              uint32_t *entry_words);




/*!
 * \brief Request to handle SER event on Counter SID.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid , symbol ID of the counter.
 * \param [in] pt_dyn_info, Ptr to dynamic info.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_ctr_ser_req(int unit,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info);

/*!
 * \brief  Request to handle SER event on Counter SIDs to clear counters.
 *
 * \param [in] unit             Logical device id.
 * \param [in] ser_req_info     Request SID and dynamic address.
 * \param [in] array_count      Request array count of the ser_req_info.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_pt_cci_ser_array_req(int unit,
                            bcmptm_cci_ser_req_info_t *ser_req_info,
                            int array_count);

/*!
 * \brief Stop (terminate thread , free resources).
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_stop(int unit);


/*!
 * \brief De-Initialize CCI software resources.
 *
 * \param [in] unit Logical device id.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_cci_cleanup(int unit);

/*!
 * \brief Get Counter Type.
 *
 * \param [in] unit Logical device id.
 * \param [in] unit Symbol ID.
 *
 * \retval Counter Type on Success.
 */
extern int
bcmptm_pt_cci_ctrtype_get(int unit,
                          bcmdrd_sid_t sid);

/*!
 * \brief Map the counter to appropriate symbol.
 *
 * \param [in] unit Logical device id.
 * \param [in] ctr_info Counter information.
 *
 * \retval Counter Type on Success.
 */
int
bcmptm_pt_cci_ctr_sym_map(int unit,
                          bcmptm_cci_ctr_info_t *ctr_info);


/*!
 * \brief Check if physical table index is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] tbl_inst Table instance.
 * \param [in] index Table index to check.
 *
 * \retval true Index is valid.
 * \retval false Index is not valid.
 */
extern bool
bcmptm_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index);

/*!
 * \brief Get eviction counter control register.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] sysmbol ID of control register.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_control_get(int unit, bcmdrd_sid_t sid,
                                    bcmdrd_sid_t *ctrl_sid);

/*!
 * \brief Get Eviction counter enable field
 *
 * \param [in] unit Unit number
 * \param [out] field id
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_pt_cci_ctr_evict_enable_field_get(int unit, bcmdrd_fid_t *fid);

/*!
 * \brief Get Eviction counter clr on read field
 *
 * \param [in] unit Unit number
 * \param [out] field id
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_pt_cci_ctr_evict_clr_on_read_field_get(int unit, bcmdrd_fid_t *fid);

/*!
 * \brief Get list of eviction polls which will be enabled by default
 *
 * \param [in] unit Unit number
 * \param [out] size of list
 * \param [out] symbol ID list
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_pt_cci_ctr_evict_default_enable_list_get(int unit, size_t *size,
                                                bcmdrd_sid_t *sid);


/*!
 * \brief Eviction counters is capable to support clear on read.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE(Supported), FALSE(Not available to support).
 */
extern bool
bcmptm_pt_cci_evict_cor_supported(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Check if counter collection by DMA is supported
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE(Supported), FALSE(Not available to support).
 */
extern bool
bcmptm_pt_cci_ctr_col_dma_supported(int unit);

/*!
 * \brief Check if Symbol is in bypass mode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
* \retval true Bypass.
 */

extern bool
bcmptm_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get FIFO DMA channel.
 * \param [in] unit Unit number.
 *
 * \retval Channel#.
 */

extern int
bcmptm_pt_cci_fifodma_chan_get(int unit);

/*!
 * Get central eviction FIFO enable register info.
 */
extern int
bcmptm_pt_cci_ctr_evict_fifo_enable_sym_get(int unit,
                                            bcmdrd_sid_t *sid,
                                            bcmdrd_fid_t *fid);
/*!
 * \brief Get Eviction FIFO Symbol.
 *
 * \param [in] unit Unit number.
 * \param [out] sid Symbol ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_fifo_sym_get(int unit, bcmdrd_sid_t *sid);


/*!
 * \brief Get information from evicted entry.
 *
 * \param [in] unit Unit number.
 * \param [in] evict Eviction entry buffer.
 * \param [out] pool  Pool entry buffer.
 * \param [out] info  Eviction Information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_fifo_entry_get(int unit,
                                       uint32_t *evict,
                                       uint32_t *pool,
                                       bcmptm_cci_ctr_info_t *info);

/*!
 * \brief Get size (max number of entires) in eviction fifo buffer.
 *
 * \param [in] unit Unit number.
 * \retval Number of entries.
 */
extern uint32_t
bcmptm_pt_cci_ctr_evict_fifo_buf_size_get(int unit);


/*!
 * \brief Get field values for normal counter mode.
 *
 * \param [in] unit Unit number.
 * \param [in] fld_width, counter field width.
 * \param [in] entry, buffer associated with physical table.
 * \param [in] size_e, size of entry buffer in words.
 * \param [OUT] counter buffer.
 * \param [in] size_ctr, size of counter buffer.
 *
* \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_normal_val_get(int unit,
                                       size_t fld_width,
                                       uint32_t *entry,
                                       size_t size_e,
                                       uint64_t *ctr,
                                       size_t size_ctr);

/*!
 * \brief Get field values for wide counter mode.
 *
 * \param [in] unit Unit number.
 * \param [in] fld_width, counter field width.
 * \param [in] entry, buffer associated with physical table.
 * \param [in] size_e, size of entry buffer in words.
 * \param [OUT] counter buffer
 * \param [in] size_ctr, size of counter buffer.
 *
* \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_wide_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint64_t *ctr,
                                     size_t size_ctr);
/*!
 * \brief Get field values for slim counter mode.
 *
 * \param [in] unit Unit number.
 * \param [in] fld_width, counter field width.
 * \param [in] entry, buffer associated with physical table.
 * \param [in] size_e, size of entry buffer in words.
 * \param [OUT] counter buffer
 * \param [in] size_dw, size of counter buffer.
 *
* \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_slim_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint32_t *ctr,
                                     size_t size_ctr);
/*!
 * \brief Get field values for normal_double counter mode.
 *
 * \param [in] unit Unit number.
 * \param [in] fld_width, counter field width.
 * \param [in] entry, buffer associated with physical table.
 * \param [in] size_e, size of entry buffer in words.
 * \param [OUT] counter buffer
 * \param [in] size_dw, size of counter buffer.
 *
* \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_pt_cci_ctr_evict_normal_double_val_get(int unit,
                                     size_t fld_width,
                                     uint32_t *entry,
                                     size_t size_e,
                                     uint64_t *ctr,
                                     size_t size_ctr);
/*!
 * \brief Get Address shift value to group registers to perform by specifying SID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval Address shift value
 */
extern uint32_t
bcmptm_pt_cci_reg_addr_gap_get(int unit,
                               bcmdrd_sid_t sid);

/*!
 * \brief Check if the specified symbol ID is the new block to form DMA group
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \retval New block determination
 */
extern bool
bcmptm_pt_cci_reg_new_block(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Start sync the counter stats in cache.
 *
 * \param [in] unit Unit number.
 * \param [in] type counter type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_stat_sync_start(int unit, int type);

/*!
 * \brief Complete sync the counter stats in cache.
 *
 * \param [in] unit Unit number.
 * \param [in] type counter type.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_stat_sync_complete(int unit, int type);

#endif /* BCMPTM_CCI_INTERNAL_H */
