/* \file cosq_stat.h
 *
 * cosq_stat headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_COSQ_STAT_H
#define BCMINT_LTSW_TM_COSQ_STAT_H

#include <bcm/types.h>
#include <bcm/cosq.h>

#include <sal/sal_types.h>

/*
 * \brief Obm stat setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_obm_set(
    int unit,
    int gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief Obm stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_obm_get(
    int unit,
    int gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    int sync_mode,
    uint64_t *value);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_obm_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_obm_counter_detach(int unit);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_drop_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_drop_counter_detach(int unit);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_drop_counter_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_drop_counter_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_rqe_pool_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_egr_sp_shared_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_total_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_counter_detach(int unit);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_counter_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_counter_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_color_drop_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_color_drop_detach(int unit);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_color_drop_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_color_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_drop_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_drop_counter_detach(int unit);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_drop_counter_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_drop_counter_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_sp_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_qgroup_min_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_pg_current_usage_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_sp_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint64_t *value,
    int sync);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_hp_drop_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_hp_drop_counter_detach(int unit);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_hp_drop_counter_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint64_t *value,
    int sync);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_hp_drop_counter_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint64_t value);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_hp_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief object stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_port_drop_counter_init(int unit);

/*
 * \brief object stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_port_drop_counter_detach(int unit);

/*
 * \brief object stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_port_drop_counter_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint64_t value);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_port_drop_counter_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint64_t *value,
    int sync);

/*
 * \brief object stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_sp_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
 * \brief cosq ADT stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_init(int unit);

/*
 * \brief cosq ADT stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_detach(int unit);

/*
 * \brief ADT low priority packet stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_low_pri_pkt_set(int unit, bcm_gport_t gport,
                                               bcm_cos_queue_t cosq,
                                               bcm_cosq_stat_t stat,
                                               uint64_t value);

/*
 * \brief ADT low priority packet stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_low_pri_pkt_get(int unit, bcm_gport_t gport,
                                               bcm_cos_queue_t cosq,
                                               bcm_cosq_stat_t stat,
                                               uint64_t *value,
                                               int sync);

 /*
 * \brief ADT high priority packet stat set
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_high_pri_pkt_set(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_stat_t stat,
                                                uint64_t value);

/*
 * \brief ADT high priority packet stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_adt_high_pri_pkt_get(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_stat_t stat,
                                                uint64_t *value,
                                                int sync);

/*
 * \brief MMU debug counter stat init
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_debug_stat_init(int unit);

/*
 * \brief MMU debug counter stat detach
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_debug_stat_detach(int unit);

/*!
 * \brief Get MMU debug counter match configuration for a certain MMU debug counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number, used to deduce buffer ID.
 * \param [in] stat MMU debug counter whose match configuration to be retrieved.
 * \param [out] match Returned MMU debug counter match configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_debug_stat_match_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match);

/*!
 * \brief Set MMU debug counter match configuration for a certain MMU debug counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number, used to deduce buffer ID.
 * \param [in] stat MMU debug counter whose match configuration to be set.
 * \param [in] match MMU debug counter match configuration to be set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
tm_ltsw_cosq_stat_debug_stat_match_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match);

/*
 * \brief Debug counter packet stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue(not used and must be -1).
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_debug_stat_pkt_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync);

/*
* \brief Debug counter packet stat set
*
* \param [in] unit Unit Number.
* \param [in] gport GPORT ID.
* \param [in] cosq COS queue(not used and must be -1).
* \param [in] stat Feature.
* \param [in] value Feature value.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
tm_ltsw_cosq_stat_debug_stat_pkt_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value);

/*
 * \brief Clear stat of port queue counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port PORT ID.
 * \param [in] buffer_id Buffer ID.
 * \param [in] queue Queue ID.
 * \param [in] is_uc Queue ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_counter_clear(
    int unit,
    int port,
    int queue,
    bool is_uc);

/*
 * \brief Clear stat of port queue color drop
 *
 * \param [in] unit Unit Number.
 * \param [in] port PORT ID.
 * \param [in] buffer_id Buffer ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_color_drop_clear(
    int unit,
    int port,
    int buffer_id);

/*
 * \brief Clear stat of port queue drop counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port PORT ID.
 * \param [in] buffer_id Buffer ID.
 * \param [in] queue Queue ID.
 * \param [in] is_uc Queue ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_port_queue_drop_counter_clear(
    int unit,
    int port,
    int buffer_id,
    int queue,
    bool is_uc);

/*
 * \brief Clear stat of ingress port drop counter
 *
 * \param [in] unit Unit Number.
 * \param [in] port PORT ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_stat_ing_port_drop_counter_clear(
    int unit,
    int port);

#endif /* BCMINT_LTSW_TM_COSQ_STAT_H */
