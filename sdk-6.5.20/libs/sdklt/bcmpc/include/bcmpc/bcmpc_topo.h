/*! \file bcmpc_topo.h
 *
 * Public topology functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TOPO_H
#define BCMPC_TOPO_H

#include <bcmpc/bcmpc_types.h>

/*!
 * \brief Get port macro instance ID for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The base physical port number of a logical port.
 * \param [out] pm_id Instance ID.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_id_get(int unit, bcmpc_pport_t pport, int *pm_id);

#endif /* BCMPC_TOPO_H */
