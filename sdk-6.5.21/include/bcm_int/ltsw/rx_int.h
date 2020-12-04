/* \file rx_int.h
 *
 * ltsw RX module internal header file.
 * Include structure definitions and function declarations used by RX module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_LTSW_RX_INT_H
#define BCMI_LTSW_RX_INT_H

#include <bcm/types.h>
#include <sal/sal_types.h>

/*
 * \brief TM_COS_Q_CPU_MAP_INFO_ID_T type enum.
 */
typedef enum bcmint_tm_cos_q_cpu_map_info_id_type_s {
    /* Enumeration for TM_BASE_VIEW. */
    BCMINT_TM_BASE_VIEW = 0,

    /* The last one, not valid. */
    BCMINT_TM_COS_Q_CPU_MAP_INFO_ID_TYPE_COUNT = 1

} bcmint_tm_cos_q_cpu_map_info_id_type_t;

/*
 * \brief RX string map structure.
 */
typedef struct bcmint_rx_str_map_s {

    /*! Index of the mapped object. */
    int idx;

    /*! Map string. */
    const char * str;

    /*! key0. */
    const char * key0;

    /*! key1. */
    const char * ifp_opaque_ctrl_id;

    /*! key2. */
    const char * int_pri;

} bcmint_rx_str_map_t;

/*
 * \brief rx string map handle.
 */
/* TM_COS_Q_CPU_MAP_INFO_ID type handle. */
#define BCMINT_TM_COS_Q_CPU_MAP_INFO_ID 0

/*
 * \brief RX string map database structure.
 */
typedef struct bcmint_rx_str_map_db_s {

    /* rx string map handle. */
    int hdl;

    /* Map info. */
    const bcmint_rx_str_map_t *info;

    /* Num of elements in map info. */
    int num;

} bcmint_rx_str_map_db_t;


/*
 * \brief RX drop event structure.
 */
typedef struct bcmint_rx_drop_event_convert_s {

    /*! Pkt drop event. */
    bcm_pkt_drop_event_t drop_event;

    /*! Rx drop code. */
    uint16_t drop_code;

} bcmint_rx_drop_event_convert_t;

/*
 * \brief RX drop event structure.
 */
typedef struct bcmint_rx_trace_event_convert_s {

    /*! Pkt trace event. */
    bcm_pkt_trace_event_t trace_event;

    /*! Rx trace code. */
    uint16_t trace_code;

} bcmint_rx_trace_event_convert_t;

/*
 * \brief rx event handle.
 */
/* RX_EVENT_INFO_ID type handle. */
#define BCMINT_RX_EVENT_INFO_ID 0

/*
 * \brief RX event database structure.
 */
typedef struct bcmint_rx_event_db_s {

    /* rx event handle. */
    int hdl;

    /* drop info. */
    const bcmint_rx_drop_event_convert_t *drop_info;

    /* Num of elements in drop info. */
    int drop_num;

    /* trace info. */
    const bcmint_rx_trace_event_convert_t *trace_info;

    /* Num of elements in trace info. */
    int trace_num;

} bcmint_rx_event_db_t;

/*
 * \brief RX map database structure.
 */
typedef struct bcmint_rx_map_db_s {

    /*! String database. */
    const bcmint_rx_str_map_db_t *str_db;

    /*! Num of elements in string database. */
    int str_num;

    /*! Event database. */
    const bcmint_rx_event_db_t *event_db;

    /*! Number of elements in event database. */
    int event_num;

} bcmint_rx_map_db_t;

typedef struct bcmint_tm_cos_q_cpu_map_info_s {

    /* key0. */
    const char *key0;

    /* key1. */
    const char *key1;

    /* key2. */
    const char *key2;

} bcmint_tm_cos_q_cpu_map_info_t;

#endif /* !BCMI_LTSW_RX_INT_H */
