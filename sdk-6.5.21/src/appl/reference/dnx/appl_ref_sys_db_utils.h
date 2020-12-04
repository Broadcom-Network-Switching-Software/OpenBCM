/** \file appl_ref_sys_db_utils.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_SYS_DB_UTILS_H_INCLUDED
/** { */
#define APPL_REF_SYS_DB_UTILS_H_INCLUDED

/** Include files. */
/** { */
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
/** } */

/**
 * \brief - utility function to interpret system port application property to actual mapping
 *
 * \param [in] unit - Unit ID
 * \param [in] sys_port_prop - string containing the system port property read
 * \param [out] sys_port_map - returned system port mapping
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_db_utils_prop_interpret(
    int unit,
    char *sys_port_prop,
    system_port_mapping_t * sys_port_map);

/** } */
#endif /* APPL_REF_SYS_DB_UTILS_H_INCLUDED */
