/*! \file bcmtm_device_info.h
 *
 * TM device information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_DEVICE_INFO_H
#define BCMTM_DEVICE_INFO_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd_config.h>

/*!
 * BCMTM device information.
 */
typedef struct bcmtm_device_info
{

    /*! Maximum packet size. */
    uint16_t max_pkt_sz;

    /*! Packet header size. */
    uint16_t pkt_hdr_sz;

    /*! Jumbo packet size. */
    uint16_t jumbo_pkt_sz;

    /*! Default MTU. */
    uint16_t default_mtu;

    /*! Cell size in bytes. */
    uint32_t cell_sz;

    /*! Number of cells. */
    uint32_t num_cells;

    /*! Number of port pririty groups. */
    uint16_t num_port_pg;

    /*! Number of service pools. */
    uint16_t num_sp;

    /*! Number of multicast entry cells. */
    uint16_t mcq_entry;

    /*! Number of rqe queue entry. */
    uint32_t rqe_q_entry;

    /*! Number of rqe queue number. */
    uint16_t rqe_q_num;

    /*! Maximum number of unicast and multicast queue for non-cpu ports. */
    uint8_t  num_q;

    /*! Maximum number of multicast queues for cpu ports. */
    uint8_t num_cpu_q;

    /*! Number of pipe. */
    uint8_t num_pipe;

    /*! Number of nhop entries in multicast repl list in sparse mode. */
    uint8_t num_nhop_sparse;

    /*! Number of nhop entries in multicast repl list in dense mode. */
    uint8_t num_nhop_dense;

    /*! Maximum number of replication for a given mc id. */
    uint16_t max_num_repl;

    /*! Number of itms or buffer pools . */
    uint8_t num_buffer_pool;
} bcmtm_device_info_t;

/*! BCMTM device information. */
extern bcmtm_device_info_t bcmtm_device_info[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief TM device information validation.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_device_info_validate(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief TM device information insert
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_device_info_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief TM device information lookup.
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
extern int
bcmtm_device_info_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief TM device information delete.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_device_info_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief TM device information update.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field value (not applicable).
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_device_info_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief TM device information table first.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmtm_device_info_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);
/*!
 * \brief TM device information table next.
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
extern int
bcmtm_device_info_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

#endif /* BCMTM_DEVICE_INFO_H */
