/*! \file bcmcth_ldh_internal.h
 *
 * Latency Distribution Histogram internal header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LDH_INTERNAL_H
#define BCMCTH_LDH_INTERNAL_H

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_dq.h>
#include <shr/shr_thread.h>
#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>

/* Debug log target definition. */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LDH

/*! Invalid data. */
#define INVALID_DATA    -1

/*!
 * \brief Define operation type of LDH LT.
 */
typedef enum {
    LDH_LT_INSERT,
    LDH_LT_UPDATE
} ldh_lt_imm_op_e;

/*!
 * \brief Define opcode of LDH_STATSt.
 */
typedef enum {
    LDH_STATS,
    LDH_STATS_BYTES,
    LDH_STATS_PACKETS
} ldh_stats_seq_op_e;

/*!
 * \brief Define opcode of LDH_MON_CONTROLt.
 * The data structure defines an orderly process for LDH_MON_CONTROLt.
 */
typedef enum {
    LDH_MON_ID,
    LDH_MON_COUNT_MODE,
    LDH_MON_COLL_MODE,
    LDH_MON_POOL_ID,
    LDH_MON_OFFSET_MODE,
    LDH_MON_OFFSET_IDX,
    LDH_MON_CTR_INCR,
    LDH_MON_GROUP_MAX,
    LDH_MON_TIME_STEP,
    LDH_MON_ENABLE
} ldh_ctrl_seq_op_e;

/*!
 * \brief Convert fid of ldh control to opcode.
 */
typedef struct ldh_ctrl_fid2op_s {

    /*! Field ID. */
    uint32_t          fid;

    /*! Opcode ID. */
    ldh_ctrl_seq_op_e opcode;

} ldh_ctrl_fid2op_t;

/*!
 * \brief Define key of LDH_STATSt entry.
 */
typedef struct ldh_stats_key_s {

    /*! Monitor ID. */
    uint32_t mon_id;

    /*! Queue ID. */
    uint32_t q_id;

    /*! Bucket ID. */
    uint32_t bkt_id;

} ldh_stats_key_t;

/*!
 * \brief Define data of LDH_STATSt entry.
 */
typedef struct ldh_stats_data_s {

    /*! Bytes counter. */
    uint64_t bytes;

    /*! Packets counter. */
    uint64_t packets;

} ldh_stats_data_t;

/*!
 * \brief Define LDH_STATSt entry data structure.
 */
typedef struct bcmcth_ldh_stats_inst_s {

    /*! Key of structure. */
    ldh_stats_key_t  key;

    /*! Value of structure. */
    ldh_stats_data_t data;

} bcmcth_ldh_stats_inst_t;

/*!
 * \brief Define key of LDH_MON_CONTROLt entry.
 */
typedef struct ldh_mon_ctrl_key_s {

    /*! Monitor ID. */
    uint32_t mon_id;

} ldh_mon_ctrl_key_t;

/*!
 * \brief Define data of LDH_MON_CONTROLt entry.
 */
typedef struct ldh_mon_ctrl_data_s {

    /*! Enable switch. */
    uint32_t enable;

    /*! Count mode. */
    uint32_t count_mode;

    /*! Collection mode. */
    uint32_t coll_mode;

    /*! Time step. */
    uint32_t time_step;

    /*! Pool ID. */
    uint32_t pool_id;

    /*! Offset mode. */
    uint32_t offset_mode;

    /*! Offset index. */
    uint32_t offset_idx;

    /*! Counter increment. */
    uint32_t ctr_incr;

    /*! Histo group maximum. */
    uint32_t group_max;

} ldh_mon_ctrl_data_t;

/*!
 * \brief Define LDH_MON_CONTROLt entry data structure.
 */
typedef struct bcmcth_ldh_mon_ctrl_inst_s {

    /*! Key instance of ldh control. */
    ldh_mon_ctrl_key_t key;

    /*! Data content of ldh control. */
    ldh_mon_ctrl_data_t data;

} bcmcth_ldh_mon_ctrl_inst_t;

/*!
 * \brief Initialize LDH control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_ldh_control_init(int unit);

/*!
 * \brief Clean up LDH control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_control_cleanup(int unit);

/*!
 * \brief Get device-specific latency distribution histogram information.
 *
 * \param [in] unit Unit number.
 * \param [out] device_info Device-specific information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_device_info_get(int unit, bcmcth_ldh_device_info_t *device_info);

/*!
 * \brief Initialize maximum number of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] max_num Maximum number of monitor.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_monitor_max_init(int unit, int max_num);

/*!
 * \brief Initialize maximum number of counter pool.
 *
 * \param [in] unit Unit number.
 * \param [in] max_num Maximum number of counter pool.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_ctr_pool_max_init(int unit, int max_num);

/*!
 * \brief Initialize maximum number of counter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] max_num Maximum number of counter entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_ctr_entry_max_init(int unit, int max_num);

/*!
 * \brief Get current working on histogram group of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Identifier of monitor.
 * \param [out] group Number of group.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_cur_histo_group_get(int unit, int mon_id, int *group);

/*!
 * \brief Get last counter index of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [out] index Number of index.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_last_ctr_idx_get(int unit, int mon_id, int *index);

/*!
 * \brief Get number of times of wrap around.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [out] num Number of times of wrap around.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_ctr_wrap_around_get(int unit, int mon_id, int *num);

/*!
 * \brief Set current working on histogram group of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] group Number of group.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_cur_histo_group_set(int unit, int mon_id, int group);

/*!
 * \brief Set last counter index of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] index Number of index.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_last_ctr_idx_set(int unit, int mon_id, int index);

/*!
 * \brief Set number of times of wrap around.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] num Number of times of wrap around.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_ctr_wrap_around_set(int unit, int mon_id, int num);

/*!
 * \brief Set Enable or Disable monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] en Bool variable.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_enable_set(int unit, int mon_id, bool en);

/*!
 * \brief Set count mode of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] mode Count mode.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_count_mode_set(int unit, int mon_id, int mode);

/*!
 * \brief Set time interval of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] step Interval of monitor.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_time_step_set(int unit, int mon_id, int step);

/*!
 * \brief Set counter pool id of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] pool_id Identifier of pool.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_pool_id_set(int unit, int mon_id, int pool_id);

/*!
 * \brief Set counter pool mode of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] mode Identifier of pool.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_pool_mode_set(int unit, int mon_id, int mode);

/*!
 * \brief Set offset of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] offset Offset index of monitor.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_base_offset_set(int unit, int mon_id, int offset);

/*!
 * \brief Set counter increment of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] incr Counter increment of monitor.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_counter_incr_set(int unit, int mon_id, int incr);

/*!
 * \brief Set maximum number of histogram group.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor identifier.
 * \param [in] max Maximum number of group.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_group_max_set(int unit, int mon_id, int max);

/*!
 * \brief Update the internal state of collector.
 *
 * \param [in] unit Unit number.
 * \param [in] op Opcode of state of collector.
 * \param [in] key Identifier of key.
 * \param [in] data Data of state of collector.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_stats_update(int unit,
    int op,
    ldh_stats_key_t key,
    uint64_t data);

/*!
 * \brief Update the internal state of ldh control.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry of ldh control.
 * \param [in] num Index of field.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_ctrl_update(int unit, uint32_t *entry, int num);

/*!
 * \brief Opcode handle of ldh control.
 *
 * \param [in] unit Unit number.
 * \param [in] op Opcode ID.
 * \param [in] key Instance ID.
 * \param [in] data Value of field.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_mon_ctrl_op(int unit,
                       int op,
                       uint32_t key,
                       uint32_t data);

/*!
 * \brief Set an entry to IMM LT MON_LDH_STATSt.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Identifier of monitor.
 * \param [in] queue_id Identifier of queue.
 * \param [in] bkt_id Identifier of bucket.
 * \param [in] pkt Number of packet.
 * \param [in] byte Number of byte.
 * \param [in] type Identifier of type.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_stats_imm_set(int unit,
                         uint32_t mon_id,
                         uint32_t queue_id,
                         uint32_t bkt_id,
                         uint32_t pkt,
                         uint32_t byte,
                         uint32_t type);

/*!
 * \brief Get an entry of IMM LT MON_LDH_STATSt.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Identifier of monitor.
 * \param [in] queue_id Identifier of queue.
 * \param [in] bkt_id Identifier of bucket.
 * \param [in] pkt Number of packet.
 * \param [in] byte Number of byte.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_stats_imm_get(int unit,
    uint32_t mon_id,
    uint32_t queue_id,
    uint32_t bkt_id,
    uint64_t *pkt,
    uint64_t *byte);

/*!
 * \brief Get an entry of IMM LT MON_LDH_CONTROLt.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Identifier of monitor.
 * \param [in] opcode Identifier of opcode.
 * \param [in] val Value of entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_control_imm_get(int unit,
    uint32_t mon_id,
    uint32_t opcode,
    uint32_t *val);

/*!
 * \brief Dump ldh internal information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_ldh_info_dump(int unit);

#endif /* BCMCTH_LDH_INTERNAL_H */
