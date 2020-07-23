/*! \file bcma_testutil_ctr.h
 *
 * Counter operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_CTR_H
#define BCMA_TESTUTIL_CTR_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>


/*!
 * \brief Queue Type Enum.
 *
 * Enum for UC/MC queue type.
 */
typedef enum traffic_queue_type_e {
    TRAFFIC_Q_TYPE_UC = 0,
    TRAFFIC_Q_TYPE_MC
} traffic_queue_type_t;

/*!
 * \brief Add a counter eviction entity.
 *
 * \param [in] unit Unit number.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_ctr_evict_add(int unit, void *user_data);

/*!
 * \brief Delete a counter eviction entity.
 *
 * \param [in] unit Unit number.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_ctr_evict_delete(int unit, void *user_data);

/*!
 * \brief Create per queue counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_ctr_egr_perq_stat_create(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Destroy per queue counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_ctr_egr_perq_stat_destroy(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Retrieve Egress PERQ Transmit counter for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 * \param [in] q_id Queue index.
 * \param [in] q_type UC/MC Queue type.
 * \param [in] pkt_counter Packet/Byte counter.
 * \param [out] count Counter value.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_ctr_egr_perq_stat_get(int unit, int port,
                                    int q_id, traffic_queue_type_t q_type,
                                    bool pkt_counter, uint64_t *count);
/*!
 * \brief Clear Egress PERQ Transmit counter for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 * \param [in] q_id Queue index.
 * \param [in] q_type UC/MC Queue type.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcma_testutil_ctr_egr_perq_stat_clear(int unit, int port,
                                      int q_id, traffic_queue_type_t q_type);


/*!
 * \brief Add a counter eviction entity.
 *
 * \param [in] unit Unit number.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_ctr_evict_add_f)(int unit, void *user_data);

/*!
 * \brief Delete a counter eviction entity.
 *
 * \param [in] unit Unit number.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_ctr_evict_delete_f)(int unit, void *user_data);

/*!
 * \brief Create per queue counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_ctr_egr_perq_stat_create_f)(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Destroy per queue counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_ctr_egr_perq_stat_destroy_f)(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Retrieve Egress PERQ Transmit counter for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 * \param [in] q_id Queue index.
 * \param [in] q_type UC/MC Queue type.
 * \param [in] pkt_counter Packet/Byte counter.
 * \param [out] count Counter value.
 *
 * \retval SHR_E_NONE No error.
 */
typedef int
(*bcma_testutil_ctr_egr_perq_stat_get_f)(int unit, int port,
                                         int q_id, traffic_queue_type_t q_type,
                                         bool pkt_counter, uint64_t *count);

/*!
 * \brief Clear Egress PERQ Transmit counter for a port, queue.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port.
 * \param [in] q_id Queue index.
 * \param [in] q_type UC/MC Queue type.
 *
 * \retval SHR_E_NONE No error.
 */
typedef int
(*bcma_testutil_ctr_egr_perq_stat_clear_f)(int unit, int port,
                                           int q_id, traffic_queue_type_t q_type);


/*! CTR operation functions. */
typedef struct bcma_testutil_ctr_op_s {
    /*! CTR eviction entry add function. */
    bcma_testutil_ctr_evict_add_f evict_add;

    /*! CTR eviction entry delete function. */
    bcma_testutil_ctr_evict_delete_f evict_delete;

    /*! CTR egress per queue statistic create function. */
    bcma_testutil_ctr_egr_perq_stat_create_f egr_perq_stat_create;

    /*! CTR egress per queue statistic destroy function. */
    bcma_testutil_ctr_egr_perq_stat_destroy_f egr_perq_stat_destroy;

    /*! CTR egress per queue statistic get function. */
    bcma_testutil_ctr_egr_perq_stat_get_f egr_perq_stat_get;

    /*! CTR egress per queue statistic clear function. */
    bcma_testutil_ctr_egr_perq_stat_clear_f egr_perq_stat_clear;
} bcma_testutil_ctr_op_t;

/*! Per-port counter operation functions. */
extern bcma_testutil_ctr_op_t *bcma_testutil_port_ctr_op_get(int unit);

/*! Per-queue counter operation functions. */
extern bcma_testutil_ctr_op_t *bcma_testutil_queue_ctr_op_get(int unit);

#endif /* BCMA_TESTUTIL_CTR_H */
