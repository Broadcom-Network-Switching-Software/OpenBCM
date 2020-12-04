/**
 * \file bcm_int/dnx/switch/switch_em_false_hit.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_MDB_EM_FALSE_HIT_H_INCLUDED
/* { */
#define SWITCH_MDB_EM_FALSE_HIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes
 * {
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 *   Initialize configuration and IRE packets for resolving the MDB EM False hit
 * \param [in] unit -
 *   The unit number.
 * \param [in] verify -
 *   Verify the counters, this has a significant performance impact.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 */
shr_error_e dnx_switch_control_mdb_exact_match_false_hit_set(
    int unit,
    int verify);
/*
 * }
 */
/* } */
#endif /* SWITCH_MDB_EM_FALSE_HIT_H_INCLUDED */
