/*! \file switch_int.h
 *
 * BCM switch control module internal interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_SWITCH_INT_H
#define BCMINT_LTSW_SWITCH_INT_H

/*!
 * \brief priority for reserved protocol control & integrity check entry.
 */
#define BCMINT_SWITCH_PKT_CONTROL_RSVD_PRI 0

/*!
 * \brief The priority offset for user programmed protocol control & integrity check entry.
 */
#define BCMINT_SWITCH_PKT_CONTROL_PRI_OFFSET 1

/*!
 * \brief The priority offset for user programmed flow control control & integrity check entry.
 */
#define BCMINT_SWITCH_FLOW_CONTROL_PRI_OFFSET 2

/*!
 * \brief SDKLT logical table information structure.
 */
typedef struct bcmint_switch_map_info_s {

    /*! Enumeration. */
    int type;

    /*! Logic table name. */
    const char *table;

    /*! Logic table field name. */
    const char *field;

} bcmint_switch_map_info_t;

/*!
 * \brief Structure for switch variant specific information.
 */
typedef struct bcmint_switch_db_s {

    /*! Drop event ID database. */
    const bcmint_switch_map_info_t *drop_event_table_info;

    /*! Number of elements in drop event ID database. */
    int num_drop_event;

    /*! Trace event ID database. */
    const bcmint_switch_map_info_t *trace_event_table_info;

    /*! Number of elements in trace event ID database. */
    int num_trace_event;

    /*! EP Recirculate drop event ID database. */
    const bcmint_switch_map_info_t *ep_recirc_drop_event_table_info;

    /*! Number of elements in EP Recirculate drop event ID database. */
    int num_ep_recirc_drop_event;

    /*! EP Recirculate trace event ID database. */
    const bcmint_switch_map_info_t *ep_recirc_trace_event_table_info;

    /*! Number of elements in EP Recirculate trace event ID database. */
    int num_ep_recirc_trace_event;
} bcmint_switch_db_t;

#endif /* BCMINT_LTSW_SWITCH_INT_H */
