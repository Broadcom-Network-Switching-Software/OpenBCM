/** \file src/bcm/dnx/trunk/trunk_temp_structs_to_skip_papi.h
 * 
 * Internal DNX TRUNK sw db APIs to be used in trunk module 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_TRUNK_TRUNKTEMPSTRUCTSTOSKIPPAPI_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_TRUNK_TRUNKTEMPSTRUCTSTOSKIPPAPI_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** all of the content of this file MUST move to PAPI later
 *  on and this file deleted */


#define BCM_TRUNK_FLAG_ALLOW_LOCAL_MEMBERS 0x800

/** }  */


typedef struct bcm_flow_agg_info_s
{
    int place_holder;
} bcm_flow_agg_info_t;

/** }  */

#endif /* _BCM_DNX_TRUNK_TRUNKTEMPSTRUCTSTOSKIPPAPI_H_INCLUDED */
