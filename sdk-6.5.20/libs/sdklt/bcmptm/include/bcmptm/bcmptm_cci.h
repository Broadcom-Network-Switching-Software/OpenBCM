/*! \file bcmptm_cci.h
 *
 * Counter Collection Infrastructure(CCI)  interfaces
 *
 * This file contains APIs, defines for top-level CCI interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CCI_H
#define BCMPTM_CCI_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>

/*!
 * \brief Counter update mode.
 */

typedef enum bcmptm_cci_update_mode_s {
    /*! No Operation */
    NOOP = 0,

    /*! Set counter value. */
    SET_TO_CVALUE,

    /*! Increment counter value. */
    INC_BY_CVALUE,

    /*! Decrement counter value. */
    DEC_BY_CVALUE,

    /*! Set MAX of counter and CVALUE. */
    MAX_VALUE,

    /*! Set MIN of counter and CVALUE. */
    MIN_VALUE,

    /*! Exponentially weighted Average. */
    AVERAGE_VALUE,

    /*! Set Bits. */
    SETBIT,

    /*! Reverse Decrement. */
    RDEC_FROM_CVALUE,

    /*! XOR with CVALUE. */
    XOR,

    /*! Clear the specified bit number in the new counter value . */
    CLRBIT,

    /*! Xor the specified bit number in the new counter value. */
    XORBIT,

    /*! Max update modes. */
    UPDATE_MODE_MAX

} bcmptm_cci_update_mode_t;


/*!
 * \brief Counter modes.
 */

typedef enum bcmptm_cci_ctr_mode_s {

    /*! CCI counter is 64 bit wide */
    NORMAL_MODE = 0,

    /*! CCI counter is 128 bit wide */
    WIDE_MODE,

    /*! CCI counter is 32 bit wide */
    SLIM_MODE,

    /*! CCI counter is 32 bit wide */
    NORMAL_DOUBLE_MODE,

} bcmptm_cci_ctr_mode_t;

/*!
 * \brief Counter eviction modes
 */
typedef enum bcmptm_cci_ctr_evict_mode_e {
    CTR_EVICT_MODE_DISABLE,
    CTR_EVICT_MODE_RANDOM,
    CTR_EVICT_MODE_THD,
    CTR_EVICT_MODE_CONDITIONAL,
    CTR_EVICT_MODE_COUNT
} bcmptm_cci_ctr_evict_mode_t;

/*!
 * \brief Counter eviction modes
 */
typedef enum bcmptm_cci_pool_state_e {
    DISABLE = 0,
    ENABLE,
    SHADOW
}  bcmptm_cci_pool_state_t;
/*!
 * \brief Description of information associated to counter SID
 */
typedef struct bcmptm_cci_ctr_info_s {
    uint64_t flags;               /*!< flags to alter default behavior */
    uint32_t  map_id;             /*!< map_id counter map id */
    bcmdrd_sid_t sid;             /*!< sid Enum to specify reg, mem */
    int  txfm_fld;                /*! Transformed field Num */
    bcmbd_pt_dyn_info_t dyn_info; /*! dynamic info */
    uint32_t lt_map_id;           /*!< transformed counter map id */
    void *in_pt_dyn_info;         /*!< pt_dyn_info Ptr dynamic info */
    void *in_pt_ovrr_info;        /*!< pt_ovrr_info Ptr, leave unchanged */
    bcmltd_sid_t req_ltid;        /*!< req_ltid Logical Table enum */
    bcmltd_sid_t *rsp_ltid;       /* rsp_ltid,set req_ltid=rsp_ltid */
    uint32_t *rsp_flags;          /*rsp_flags, response flags */
} bcmptm_cci_ctr_info_t;


/*!
 * \brief Set state of the flex counter pool.
 * \n Called by Flex couner module
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] state disable, enable, shadow
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_flex_counter_pool_set_state(int unit,
                                       bcmdrd_sid_t sid,
                                       bcmptm_cci_pool_state_t state);

/*!
 * \brief Get state of the flex counter pool.
 * \n Called by Flex couner module
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] state disable, enable, shadow
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_flex_counter_pool_get_state(int unit,
                                       bcmdrd_sid_t sid,
                                       bcmptm_cci_pool_state_t *state);
/*!
 * \brief Set counter mode.
 * \n Called by Flex couner module
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum to specify reg, mem.
 * \param [in] index_min start index.
 * \param [in] index_max end index.
  * \param [in] tbl_inst , Pipe number.
 * \param [in] ctr_mode-counter mode.
 * \param [in] update_mode-counter update mode.
 * \param [in] size, number of update modes.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_flex_counter_mode_set(int unit,
                                 bcmdrd_sid_t sid,
                                 uint32_t index_min,
                                 uint32_t index_max,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t size);

/*!
 * \brief Get counter mode.
 * \n Called by Flex couner module
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum to specify reg, mem.
 * \param [in] index index of the table.
 * \param [in] tbl_inst , Pipe number.
 * \param [out] ctr_mode, counter mode.
 * \param [out] update_mode-counter update mode.
 * \param [in, out] size, number of update modes.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_flex_counter_mode_get(int unit,
                                 bcmdrd_sid_t sid,
                                 uint32_t index,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t *ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t *size);


/*!
 * \brief Copy field  bits between two word arrays.
 *
 * \param [in, out] d, destination buffer pointer.
 * \param [in] offd, destination field offset (in bit).
 * \param [in] s, source buffer pointer.
 * \param [in] offs, source field offset (in bits).
 * \param [in] n, field size bits.
 */
void
bcmptm_cci_buff32_field_copy(uint32_t *d, int offd, const uint32_t *s,
                             int offs, int n);

/*!
 * \brief Get field number for specified counter modes.
 *
 * \param [in] unit Unit number.
 * \param [in] ctr_mode counter mode (SLIM. WIDE, NORMAL).
 * \param [OUT] num, number of counter fields
 *
 * \retval Number of fields.
 */
extern int
bcmptm_pt_cci_ctr_evict_field_num_get(int unit,
                                      bcmptm_cci_ctr_mode_t ctr_mode,
                                      size_t *num);

/*!
 * \brief Get field width for specified counter modes.
 *
 * \param [in] unit Unit number.
 * \param [in] ctr_mode counter mode (SLIM. WIDE, NORMAL).
 * \param [OUT] num, number of counter fields
 *
 * \retval Number of fields.
 */
extern int
bcmptm_pt_cci_ctr_evict_field_width_get(int unit,
                                      bcmptm_cci_ctr_mode_t ctr_mode,
                                      size_t *width);

/*!
 * \brief Get format symbol ID if exist.
 *
 * \param [in] unit, Unit number.
 * \param [in] index, entry index, value -1 returns genertic format.
 * \param [in] field, field number for valid entry index.
 * \param [in, out] sid Symbol ID, out format Symbol ID, if exist.
 * \param [in, out] width, field width.
 * \retval SHR_E_NONE No errors.
 */

extern int
bcmptm_pt_cci_frmt_sym_get(int unit, int index, int field,
                           bcmdrd_sid_t *sid, size_t *width);

/*!
 * \brief Check collection on port counters is enabled.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port ID.
 * \retval true if enabled, otherwise false.
 */
extern bool
bcmptm_cci_port_collect_enabled(int unit, int port);

/*!
 * \brief Check collection on evict counters is enabled.
 *
 * \param [in] unit Unit number.
 * \retval true if enabled, otherwise false.
 */
extern bool
bcmptm_cci_evict_collect_enabled(int unit);

/*!
 * \brief Eviction counter control set.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] clr_on_read, TRUE(Enable), FALSE(Disable).
 * \param [in] mode, eviction mode.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_evict_control_set(int unit,
                             bcmdrd_sid_t sid,
                             bool clr_on_read,
                             bcmptm_cci_ctr_evict_mode_t mode);

/*!
 * \brief Set Flex counter hit bit.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] pipe Pipe number.
 * \param [in] index Index of entry.
 * \param [in] bit Bit of the entry needs to be set.
 *
 * \retval SHR_E_NONE No errors.
 */

extern int
bcmptm_cci_hit_bit_set(int unit,
                       bcmdrd_sid_t sid,
                       uint32_t pipe,
                       uint32_t index,
                       uint32_t bit);

/*!
 * \brief Read Slim Counter.
 *
 * \param [in] unit, Unit number.
 * \param [in] info, Information of the slim counter to read.
 * \param [out] value, Pointer of read buffer.
 * \param [in] wsize,  size of bufferin words.
 *
 * wsize should be equal to entry size of counter table.
 * info->in_pt_dyn_info->index is used for index.
 * info->in_pt_dyn_info->tbl_inst is used for pipe.
 * info->txfm_fld is used for counter field.
 * info->flags is used for read flag e.g. BCMLT_ENT_ATTR_GET_FROM_HW
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_slim_ctr_read(int unit,
                         bcmptm_cci_ctr_info_t *info,
                         uint32_t *value,
                         size_t wsize);

/*!
 * \brief Write Slim Counter.
 *
 * \param [in] unit Unit number.
 * \param [in] info. Information of the slim counter to read.
 * \param [in] value, country value.
 * \param [in] value, Pointer of write buffer.
 *
 * info->in_pt_dyn_info->index is used for index.
 * info->in_pt_dyn_info->tbl_inst is used for pipe.
 * info->txfm_fld is used for counter field.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_slim_ctr_write(int unit,
                          bcmptm_cci_ctr_info_t *info,
                          uint32_t *value);
/*!
 * \brief Read Normal double Counter.
 *
 * \param [in] unit, Unit number.
 * \param [in] info, Information of the slim counter to read.
 * \param [out] value, Pointer of read buffer.
 * \param [in] wsize,  size of bufferin words.
 *
 * wsize should be equal to entry size of counter table.
 * info->in_pt_dyn_info->index is used for index.
 * info->in_pt_dyn_info->tbl_inst is used for pipe.
 * info->txfm_fld is used for counter field.
 * info->flags is used for read flag e.g. BCMLT_ENT_ATTR_GET_FROM_HW
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_normal_double_ctr_read(int unit,
                         bcmptm_cci_ctr_info_t *info,
                         uint32_t *value,
                         size_t wsize);

/*!
 * \brief Write Normal double Counter.
 *
 * \param [in] unit Unit number.
 * \param [in] info. Information of the slim counter to read.
 * \param [in] value, country value.
 * \param [in] value, Pointer of write buffer.
 *
 * info->in_pt_dyn_info->index is used for index.
 * info->in_pt_dyn_info->tbl_inst is used for pipe.
 * info->txfm_fld is used for counter field.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_normal_double_ctr_write(int unit,
                          bcmptm_cci_ctr_info_t *info,
                          uint32_t *value);



/*!
 * \brief Get number of counter update modes.
 *
 * \param [in] unit Unit number.
 * \param [OUT] num, number of counter update modes.
 *
 * \retval Number of counter update modes.
 */
extern int
bcmptm_pt_cci_ctr_evict_update_mode_num_get(int unit,
                                            size_t *num);

/*!
 * Enable/Disable central eviction FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] enable(TRUE) and disable(FALSE).
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_evict_fifo_enable(int unit, bool enable);

/*!
 * \brief Move flex counter table entry.
 * \param [in] unit Unit number.
 * \param [in] src_info Dynamic info of source entry.
 * \param [in] dst_info Dynamic info of destination entry.
 *
 * Value of counter cache entry will be copied from source to destination.
 * Value of destination counter hardware entry will not be modified.
 * Value of source counter cache and hardware entry will be set to zero.
 * Source and destination table should have same Counter mode (Wide, Normal, Slim).
 * In case of Slim counters, only specified counter field will be modified.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_ctr_flex_entry_move(int unit,
                               bcmptm_cci_ctr_info_t *src_info,
                               bcmptm_cci_ctr_info_t *dst_info);

/*!
 * \brief Get index range in formatted table for given port.
 *
 * \param [in] unit Unit number.
 * \param [in, out] sid Symbol ID, out format Symbol ID, if exist.
 * \retval SHR_E_NONE No errors.
 */

extern int
bcmptm_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                   int *index_min, int *index_max);

#endif /* BCMPTM_CCI_H */
