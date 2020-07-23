/*! \file bcmcth_mon_flowtracker_flow.h
 *
 * This file contains Flowtracker custom handler fucntions
 * for handling insert, delete, update ,lookup and
 * validate of MON_FLOWTRACKER_FLOW_DATA table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_FLOW_H
#define BCMCTH_MON_FLOWTRACKER_FLOW_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

/*! Data structure for the fields in MON_FLOWTRACKER_FLOW_DATA LT. */
typedef struct bcmcth_mon_flowtracker_flow_data_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_FLOW_DATAt_FIELD_COUNT);

    /*! Flow group id. */
    uint32_t group_id;

    /*! Indicates if flow was statically installed by application. */
    bool     is_static;
    /*! Source IPv4 address. */
    shr_ip_t src_ipv4;

    /*! Destination IPv4 address. */
    shr_ip_t dst_ipv4;

    /*! Layer 4 UDP source port. */
    uint16_t src_l4_port;

    /*! Layer 4 UDP destination port. */
    uint16_t dst_l4_port;

    /*! IP protocol used. */
    uint8_t ip_proto;

    /*! Inner source IPv4 address. */
    shr_ip_t inner_src_ipv4;

    /*! Inner destination IPv4 address. */
    shr_ip_t inner_dst_ipv4;

    /*! Inner layer 4 UDP source port. */
    uint16_t inner_src_l4_port;

    /*! Inner layer 4 UDP destination port. */
    uint16_t inner_dst_l4_port;

    /*! Inner IP protocol used. */
    uint8_t inner_ip_proto;

    /*! VxLAN network ID. */
    uint32_t vnid;

    /*! Custom user defined upper key. */
    uint64_t custom_key_upper;

    /*! Custom user defined lower key. */
    uint64_t custom_key_lower;

    /*! The ingress port of the packet. */
    shr_port_t ing_port_id;

    /*! Inner source IPv6 address. */
    shr_ip6_t inner_src_ipv6;

    /*! Inner destination IPv6 address. */
    shr_ip6_t inner_dst_ipv6;

    /*! Count of packets of the flow. */
    uint64_t pkt_count;

    /*! Count of bytes of the flow. */
    uint64_t byte_count;

    /*!
     * Timestamp from epoch at which the flow was learnt.
     * Applicable when the field FLOW_START_TIMSTAMP_ENABLE in
     * the MON_FLOWTRACKER_CONTROL table is set.
     */
    uint64_t flow_start_time_msecs;

    /*! Timestamp from epoch at which the flow was last observed. */
    uint64_t observation_time_msecs;
} bcmcth_mon_flowtracker_flow_data_t;

/*!
 * \brief Flowtracker flow data validation
 *
 * Validates fields in MON_FLOWTRACKER_FLOW_DATA LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_flowtracker_flow_validate(int unit,
                                     bcmlt_opcode_t opcode,
                                     const bcmltd_fields_t *in,
                                     const bcmltd_generic_arg_t *arg);
/*!
 * \brief Flowtracker flow data insert.
 *
 * Insert fields into MON_FLOWTRACKER_FLOW_DATA LT.
 * It is not a VALID operation since this is an
 * UPDATE_ONLY LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_flowtracker_flow_insert(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);
/*!
 * \brief Flowtracker flow data lookup.
 *
 * Lookup fields in MON_FLOWTRACKER_FLOW_DATA LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_flowtracker_flow_lookup(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);
/*!
 * \brief Flowtracker flow data delete.
 *
 * Delete fields in MON_FLOWTRACKER_FLOW_DATA LT.
 * It is not a VALID operation since this is an
 * UPDATE_ONLY LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_flowtracker_flow_delete(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);
/*!
 * \brief Flowtracker flow data traverse.
 *
 * Fetch fields in MON_FLOWTRACKER_FLOW_DATA LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_flowtracker_flow_first(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg);
/*!
 * \brief Flowtracker flow data traverse.
 *
 * Fetch fields in MON_FLOWTRACKER_FLOW_DATA LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_flowtracker_flow_next(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_generic_arg_t *arg);

/*!
 * \brief Flowtracker flow data update.
 *
 * Update fields in MON_FLOWTRACKER_FLOW_DATA LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_flowtracker_flow_update(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);
#endif /* BCMCTH_MON_FLOWTRACKER_FLOW_H */
