/*! \file bcmsec_utils.h
 *
 * BCMSEC utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_UTILS_H
#define BCMSEC_UTILS_H

#include <bcmsec/bcmsec_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmsec/bcmsec_port_info.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>

/*!
 * \brief Verify logical port for cpu port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is CPU port.
 * \retval FALSE logical port is not CPU port.
 */
extern int
bcmsec_port_is_cpu(int unit,
        bcmsec_lport_t lport);

/*!
 * \brief Verify logical port for loopback port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is loopback port.
 * \retval FALSE logical port is not loopback port.
 */
extern int
bcmsec_port_is_lb(int unit, bcmsec_lport_t lport);

/*!
 * \brief Verify logical port for front panel port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is front panel port.
 * \retval FALSE logical port is not front panel port.
 */
extern int
bcmsec_port_is_fp(int unit, bcmsec_lport_t lport);

/*!
 * \brief Verify logical port for management port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is management port.
 * \retval FALSE logical port is not management port.
 */
extern int
bcmsec_port_is_mgmt(int unit, bcmsec_lport_t lport);

/*!
 * \brief Verify logical port for rdb port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is RDB port.
 * \retval FALSE logical port is not RDB port.
 */
extern int
bcmsec_port_is_rdb(int unit, bcmsec_lport_t lport);

/*!
 * \brief Reset port info
 *
 * \param [in] unit Unit number.
 */
extern void
bcmsec_dev_info_portmap_reset(int unit);

/*!
 * \brief Port operations for SEC module
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op_name Operation string.
 * \param [in] op_param Operation parameter.
 *
 * \retval TRUE logical port is RDB port.
 * \retval FALSE logical port is not RDB port.
 */
extern int
bcmsec_port_op_exec(int unit, bcmsec_pport_t pport,
                    char *op_name, uint32_t op_param);

/*!
 * \brief Get the range for an LT field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table.
 * \param [in] fid LT Field.
 * \param [out] min Field min value.
 * \param [out] max Field Max value.
 *
 * \retval SHR_E_NONE   No failures.
 */
extern int
bcmsec_field_value_range_get(int unit,
                             bcmlrd_sid_t sid,
                             uint32_t fid,
                             uint64_t *min, uint64_t *max);

/*!
 * \brief Get the physical port for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport logical port.
 * \param [out] pport physical port.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_NOT_FOUND  Port not found
 */
extern int
bcmsec_port_phys_port_get(int unit,
        bcmsec_lport_t lport,
        int *pport);
/*!
 * \brief Update IMM during port add/delete.
 *
 * \param [in] unit Unit number.
 * \param [in] lport logical port.
 * \param [in] port_op Port operation.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_NOT_FOUND  Port not found
 */
extern int
bcmsec_imm_port_update(int unit, int lport,
                       bcmsec_port_internal_update_t port_op);

/*!
 * \brief Post an event to event queue.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event information.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_NOT_FOUND  Port not found
 */
extern int
bcmsec_post_event(int unit, bcmsec_event_trigger_t *event);

/*!
 * \brief Retrieve num SA per SC configuration
 *
 * \param [in] unit Unit number.
 * \param [in] blk_id Block ID.
 * \param [in] encrypt Encrypt or Decrypt.
 * \param [out] num_sa Number of SA per SC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_num_sa_per_sc_get(int unit, int blk_id,
                         bool encrypt, int *num_sa);

/*!
 * \brief Set the num SA per SC configuration
 *
 * \param [in] unit Unit number.
 * \param [in] blk_id Block ID.
 * \param [in] encrypt Encrypt or Decrypt.
 * \param [in] num_sa Number of SA per SC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_num_sa_per_sc_set(int unit, int blk_id,
                         bool encrypt, int num_sa);

/*!
 * \brief Get the SEC PT port information.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] info PT port information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmsec_port_info_get(int unit, int lport,
                     sec_pt_port_info_t *info);

#endif /* BCMSEC_UTILS_H */
