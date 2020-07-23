/*! \file bcmcth_ecn_protocol.h
 *
 * This file contains ECN protocol handler function declarations and
 * macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_ECN_PROTOCOL_H
#define BCMCTH_ECN_PROTOCOL_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * BCMCTH load balance hashing information.
 */
typedef struct bcmcth_ecn_proto_info {

    /*! Maximum number of protocols supported. */
    uint16_t max_proto;

    /*! Number of protocols supported per hardware entry. */
    uint16_t num_proto_per_entry;

    /*! Protocol match table ID. */
    uint32_t protocol_match_sid;

    /*! Protocol match field ID. */
    uint32_t responsive_fid;

} bcmcth_ecn_proto_info_t;

/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ecn_proto_info_init(int unit);

/*!
 * \brief ECN protocol insert handler.
 *
 * The method inserts the configuration for setting responsive nature
 * of a particular IP protocol.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  event         IMM event reason.
 * \param [in]  key           Key field list.
 * \param [in]  data          Data field list.
 *
 * \retval SHR_E_NONE         No errors.
 */
extern int
bcmcth_ecn_protocol_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data);
/*!
 * \brief ECN protocol delete handler.
 *
 * The method looks the incoming data and finds the index. It deletes
 * the entry for that index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  key           Key field list.
 *
 * \retval SHR_E_NONE         No errors.
 */
extern int
bcmcth_ecn_protocol_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_field_t *key);

/*!
 * \brief ECN protocol lookup handler.
 *
 * The method lookups the configuration for getting responsive nature
 * of a particular IP protocol.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  key           Key field list.
 * \param [out]  data          Data field list.
 *
 * \retval SHR_E_NONE         No errors.
 */
extern int
bcmcth_ecn_protocol_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *key,
                           bcmltd_fields_t *data);

#endif /* BCMCTH_ECN_PROTOCOL_H */
