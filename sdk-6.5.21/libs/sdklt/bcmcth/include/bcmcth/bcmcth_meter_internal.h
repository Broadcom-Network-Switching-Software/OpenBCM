/*! \file bcmcth_meter_internal.h
 *
 * This file contains FP Meter Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_INTERNAL_H
#define BCMCTH_METER_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_meter_util.h>

/*! Meter LT insert operation. */
#define METER_FP_LT_INSERT       0
/*! Meter LT update operation. */
#define METER_FP_LT_UPDATE       1

/*! Default meter id used to detach. */
#define METER_DEFAULT_METER_ID   0

/*! Meter LT meter mode default. */
#define METER_FP_MODE_DEFAULT    0
/*! Meter LT meter mode flow. */
#define METER_FP_MODE_FLOW       1
/*! Meter LT meter mode srtcm. */
#define METER_FP_MODE_SRTCM      2
/*! Meter LT meter mode modsrtcm. */
#define METER_FP_MODE_MODSRTCM   3
/*! Meter LT meter mode trtcm. */
#define METER_FP_MODE_TRTCM      4
/*! Meter LT meter mode modtrtcm. */
#define METER_FP_MODE_MODTRTCM   5

/*! Meter LT meter mode color blind. */
#define METER_FP_COLOR_BLIND     0
/*! Meter LT meter mode color aware. */
#define METER_FP_COLOR_AWARE     1

/*! Meter LT meter byte mode. */
#define METER_FP_MODE_BYTES      false
/*! Meter LT meter packet mode. */
#define METER_FP_MODE_PKTS       true

/*! FP Meter number of meters in pairing mode. */
#define METER_FP_PAIR_NUM        2
/*! FP Meter meter pair odd meter index. */
#define METER_FP_INDEX_ODD       1
/*! FP Meter meter pair even meter index. */
#define METER_FP_INDEX_EVEN      0

/*! Check meter mode and determine if meter is in paired mode. */
#define METER_FP_PAIRING_CHECK_ASSIGN(meter_mode, pairing)   \
    if (meter_mode > METER_FP_MODE_FLOW) {                   \
        pairing = true;                                      \
    }

/*!
 * \brief FP Meter LT fields structure.
 *
 * LT parameters of FP meters are stored in this structure.
 */
typedef struct bcmcth_meter_sw_params_s {
    /*! Software meter id. */
    uint16_t    meter_id;
    /*! Pipe number. */
    uint8_t     pipe;
    /*! Meter pool id. */
    uint8_t     meter_pool_id;
    /*! Meter mode, byte or packet. */
    uint8_t     byte_mode;
    /*! Meter operating mode. */
    uint8_t     meter_mode;
    /*! Meter color mode. */
    uint8_t     color_mode;
    /*! Committed meter rate. */
    uint64_t    min_rate_kbps;
    /*! Committed burst size. */
    uint32_t    min_burst_kbits;
    /*! Excess meter rate. */
    uint64_t    max_rate_kbps;
    /*! Excess burst size. */
    uint32_t    max_burst_kbits;
    /*! Committed meter rate. */
    uint64_t    min_rate_pps;
    /*! Committed burst size. */
    uint32_t    min_burst_pkts;
    /*! Excess meter rate. */
    uint64_t    max_rate_pps;
    /*! Excess burst size. */
    uint32_t    max_burst_pkts;
    /*! Committed meter burst size. */
    uint32_t    min_burst_oper;
    /*! Excess meter burst size. */
    uint32_t    max_burst_oper;
    /*! Committed meter operational rate. */
    uint64_t    min_rate_oper;
    /*! Excess meter operational rate. */
    uint64_t    max_rate_oper;
    /*! Meter harwdare format. */
    uint32_t    meter_format;
} bcmcth_meter_sw_params_t;

/*!
 * \brief FP Meter meter granularity information structure.
 *
 * Maintains min and max meter rate information at each granularity
 * level.
 */
typedef struct bcmcth_meter_fp_gran_info_s {
    /*! Minimum meter rate. */
    uint64_t    min_rate;
    /*! Maximum meter rate. */
    uint64_t    max_rate;
    /*! Minimum burst size. */
    uint32_t    min_burst;
} bcmcth_meter_fp_gran_info_t;

/*!
 * \brief Enable FP slice for h/w access.
 */
typedef struct bcmcth_meter_fp_slice_ctrl_s {
    /*! Per-slice control register id. */
    uint32_t    sid;
    /*! Number of fields. */
    uint32_t    fid_count;
    /*! Array of fids. */
    uint32_t    *fids;
} bcmcth_meter_fp_slice_ctrl_t;

/*!
 * \brief FP meter LT fields.
 */
typedef struct bcmcth_meter_fp_fid_info_s {
    /*! PIPE field ID. */
    uint32_t pipe_fid;
    /*! Pool instance ID. */
    uint32_t pool_fid;
    /*! Packet mode field ID. */
    uint32_t pkt_mode_fid;
    /*! Meter mode field ID. */
    uint32_t meter_mode_fid;
    /*! Color mode field ID. */
    uint32_t color_mode_fid;
    /*! Min meter rate in kbps field ID. */
    uint32_t min_rate_kbps_fid;
    /*! Max meter rate in kbps field ID. */
    uint32_t max_rate_kbps_fid;
    /*! Min burst size in kbits field ID. */
    uint32_t min_burst_kbits_fid;
    /*! Max burst size in kbits field ID. */
    uint32_t max_burst_kbits_fid;
    /*! Min meter rate in pps field ID. */
    uint32_t min_rate_pps_fid;
    /*! Max meter rate in pps field ID. */
    uint32_t max_rate_pps_fid;
    /*! Min burst size in packets field ID. */
    uint32_t min_burst_pkts_fid;
    /*! Max burst size in packets field ID. */
    uint32_t max_burst_pkts_fid;
    /*! Operational min meter rate in kbps field ID. */
    uint32_t min_rate_kbps_oper_fid;
    /*! Operational max meter rate in kbps field ID. */
    uint32_t max_rate_kbps_oper_fid;
    /*! Operational min burst size in kbits field ID. */
    uint32_t min_burst_kbits_oper_fid;
    /*! Operational max burst size in kbits field ID. */
    uint32_t max_burst_kbits_oper_fid;
    /*! Operational min meter rate in pps field ID. */
    uint32_t min_rate_pps_oper_fid;
    /*! Operational max meter rate in pps field ID. */
    uint32_t max_rate_pps_oper_fid;
    /*! Operational min burst size in packets field ID. */
    uint32_t min_burst_pkts_oper_fid;
    /*! Operational max burst size in packets field ID. */
    uint32_t max_burst_pkts_oper_fid;
    /*! Meter hw action set ID. */
    uint32_t meter_action_set_fid;
} bcmcth_meter_fp_fid_info_t;

/*!
 * Meter format structure fields.
 */
typedef enum bcmcth_meter_format_field_e {
    /*! Meter pair index. */
    METER_PAIR_INDEX,
    /*! Meter pair mode. */
    METER_PAIR_MODE,
    /*! Meter pair mode modifier. */
    METER_PAIR_MODE_MODIFIER,
    /*! Test even meter. */
    METER_TEST_EVEN,
    /*! Test odd meter. */
    METER_TEST_ODD,
    /*! Update even meter. */
    METER_UPDATE_EVEN,
    /*! Update odd meter. */
    METER_UPDATE_ODD,
    /*! Number of fields. */
    METER_FIELD_COUNT,
} bcmcth_meter_format_field_t;

/*!
 * \brief FP meter format structure.
 */
typedef struct bcmcth_meter_format_set_s {
    /*! Field name. */
    bcmcth_meter_format_field_t field;
    /*! Field start bit. */
    uint32_t s_bit;
    /*! Field end bit. */
    uint32_t e_bit;
} bcmcth_meter_format_set_t;

/*!
 * \brief FP meter chip attribute structure.
 *
 * Chip specific attributes for FP meter.
 */
typedef struct bcmcth_meter_fp_attrib_s {
    /*! Logical table ID. */
    uint32_t ltid;
    /*! Logical table key field ID. */
    uint32_t key_fid;
    /*! Logical field count. */
    uint32_t fld_count;
    /*! Number of meters per pipe. */
    uint32_t num_meters_per_pipe;
    /*! Number of pipes. */
    uint32_t num_pipes;
    /*! Number of meters per pool. */
    uint32_t num_meters_per_pool;
    /*! Number of pools.*/
    uint32_t num_pools;
    /*! Maximum granularity. */
    uint32_t max_granularity;
    /*! Maximum refresh count. */
    uint32_t max_refresh_cnt;
    /*! Maximum bucket size. */
    uint32_t max_bucket_size;
    /*! Bucket size granularity in tokens. */
    uint32_t bucket_size_gran;
    /*! Refresh count adjustment required. */
    bool refreshcount_adjust;
    /*! Meter pool global status. */
    bool meter_pool_global;
    /*! Number of hw tables. */
    uint8_t  num_meter_sids;
    /*! Hardware meter table id list. */
    uint32_t *meter_sid;
    /*! Hardware meter byte mode field name. */
    uint32_t byte_mode_fid;
    /*! Hardware meter granularity field name. */
    uint32_t meter_gran_fid;
    /*! Hardware meter refreh mode field name. */
    uint32_t refresh_mode_fid;
    /*! Hardware meter refreh count field name. */
    uint32_t refresh_count_fid;
    /*! Hardware meter bucket size field name. */
    uint32_t bucket_size_fid;
    /*! Hardware meter bucket count field name. */
    uint32_t bucket_count_fid;
    /*! Meter refresh enable configuration regsiter. */
    uint32_t refresh_cfg_sid;
    /*! Meter refresh enable field name. */
    uint32_t refresh_en_fid;
    /*! Meter refresh mode field name. */
    uint32_t fp_refresh_mode_fid;
    /*! Pointer to LT field ID array. */
    bcmcth_meter_fp_fid_info_t *fid_info;
    /*! Pointer to meter granularity information structure in byte mode. */
    bcmcth_meter_fp_gran_info_t *gran_info_bytes;
    /*! Pointer to meter granularity information structure in packet mode. */
    bcmcth_meter_fp_gran_info_t *gran_info_pkts;
    /*! FP meter slice control. */
    bcmcth_meter_fp_slice_ctrl_t *slice_ctrl;
    /*! Meter format info. */
    bcmcth_meter_format_set_t *meter_format_info;
} bcmcth_meter_fp_attrib_t;

/*! FP meter device config. */
typedef int (*meter_fp_hw_config_f)(int unit,
                                    uint32_t refresh_en);

/*! FP meter convert to HW params. */
typedef int (*meter_fp_compute_hw_param_f)(int unit,
                                           bcmcth_meter_sw_params_t *lt_entry);

/*! FP meter get HW oper values. */
typedef int (*meter_fp_get_oper_param_f)(int unit,
                                         bcmcth_meter_sw_params_t *lt_entry);

/*! FP meter alloc HW index. */
typedef int (*meter_fp_index_alloc_f)(int unit,
                                      int pool,
                                      uint8_t oper_mode,
                                      bcmcth_meter_sw_params_t *lt_entry,
                                      uint32_t *index);

/*! FP meter free HW index. */
typedef int (*meter_fp_index_free_f)(int unit,
                                     uint32_t index,
                                     bcmcth_meter_sw_params_t *lt_entry);

/*! FP meter write HW entry. */
typedef int (*meter_fp_entry_write_f)(int unit,
                                      uint8_t oper_mode,
                                      uint32_t trans_id,
                                      bcmcth_meter_sw_params_t *lt_entry);

/*! FP meter clear HW entry. */
typedef int (*meter_fp_entry_clear_f)(int unit,
                                      uint8_t oper_mode,
                                      uint32_t trans_id,
                                      int pool,
                                      uint32_t hw_index,
                                      bcmcth_meter_sw_params_t *lt_entry);

/*! FP meter hw fields get. */
typedef int (*meter_fp_policy_action_get_f)(int unit,
                                            bcmcth_meter_sw_params_t *lt_entry);

/*! Get FP meter attributes. */
typedef int (*meter_fp_attrib_get_f)(int unit,
                                     bcmcth_meter_fp_attrib_t **attr);

/*!
 * \brief FP Meter chip specific driver structure.
 */
typedef struct bcmcth_meter_fp_drv_s {
    /*! FP meter device config. */
    meter_fp_hw_config_f hw_config;
    /*! FP meter convert to HW params. */
    meter_fp_compute_hw_param_f hw_param_compute;
    /*! FP meter get actual oper rate. */
    meter_fp_get_oper_param_f hw_oper_get;
    /*! FP meter alloc HW index. */
    meter_fp_index_alloc_f hw_idx_alloc;
    /*! FP meter free HW index. */
    meter_fp_index_free_f hw_idx_free;
    /*! FP meter HW write. */
    meter_fp_entry_write_f hw_write;
    /*! FP meter HW clear. */
    meter_fp_entry_clear_f hw_clear;
    /*! FP meter action set prepare. */
    meter_fp_policy_action_get_f meter_format_get;
    /*! Get FP meter attributes. */
    meter_fp_attrib_get_f attrib_get;
} bcmcth_meter_fp_drv_t;

/*!
 * \brief Configure FP meter related device info.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_fp_dev_config(int unit, int stage, uint32_t refresh_en);


/*!
 * \brief Get FP Pipe mode configured.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] mode Operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_fp_oper_mode_get(int unit,
                              int stage,
                              uint8_t *mode);

/*!
 * \brief Function handle LT events from FP.
 * \param [in] unit Logical device id
 * \param [in] event Type of event
 * \param [in] ev_data Data associated with the event.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
void
bcmcth_meter_fp_event_cb(int unit,
                         const char *event,
                         uint64_t ev_data);

/*!
 * \brief Insert meter LT entry.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 * \param [in] data fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmcth_meter_template_insert(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data);

/*!
 * \brief Delete meter LT entry.
 *
 * Delete the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmcth_meter_template_delete(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key);
/*!
 * \brief Update the meter LT entry.
 *
 * Update the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage
 * \param [in] trans_id Transaction ID.
 * \param [in] key Key fields list.
 * \param [in] data fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmcth_meter_template_update(int unit,
                             int stage,
                             uint32_t trans_id,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data);
/*!
 * \brief Lookup the meter LT entry.
 *
 * Lookup the meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage
 * \param [in] trans_id Transaction ID.
 * \param [in] context Context pointer.
 * \param [in] lkup_type Lookup type.
 * \param [in] in Input fields list.
 * \param [in] out Output fields list.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmcth_meter_template_lookup(int unit,
                             int stage,
                             uint32_t trans_id,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out);

/*!
 * \brief Insert meter into hardware table.
 *
 * Find a free index and write this meter into hardware table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] pool Meter pool id.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 * \retval SHR_E_FULL Hardware meter table full.
 * \retval SHR_E_PARAM Invalid meter param.
 */
int
bcmcth_meter_entry_insert(int unit,
                          uint32_t trans_id,
                          int stage,
                          int pool,
                          uint32_t meter_id);

/*!
 * \brief Lookup an FP entry.
 *
 * Lookup the meter entry from Software meter table
 * and retrieve meter parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 * \param [out] meter_entry Pointer to meter entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_lookup(int unit,
                          int stage,
                          uint32_t meter_id,
                          bcmcth_meter_fp_policy_fields_t *meter_entry);

/*!
 * \brief Delete a meter entry.
 *
 * Delete the meter entry from hardware if ref count is zero and free the
 * hardware index.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_delete(int unit,
                          uint32_t trans_id,
                          int stage,
                          uint32_t meter_id);

/*!
 * \brief Detach an FP entry from the meter.
 *
 * Decrement the ref count of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_detach(int unit,
                          int stage,
                          uint32_t meter_id);

/*!
 * \brief Attach an FP entry to the meter.
 *
 * Increment the ref count of this meter entry to keep track of
 * number of FP entries pointing to this meter.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Unable to find the meter entry in S/W meter table.
 */
int
bcmcth_meter_entry_attach(int unit,
                          int stage,
                          uint32_t meter_id);

/*!
 * \brief Validate meter id.
 *
 * Check if the meter entry is present in s/w table.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] meter_id Software Meter id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Meter entry not present in s/w table.
 */
int
bcmcth_meter_entry_valid(int unit,
                         int stage,
                         uint32_t meter_id);

/*!
 * \brief Return global status of meter pool.
 *
 * Check the chip specific attributes and return whether the meter pool
 * is global or not.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] global Global pool status of meters in this FP stage.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_pool_global(int unit,
                         int stage,
                         bool *global);

/*!
 * \brief Return number of used meter entries.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [out] num_entries Number of used meter entries.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_meter_util_used_entries_calc(int unit,
                                    int stage,
                                    uint32_t *num_entries);

#endif /* BCMCTH_METER_INTERNAL_H */
