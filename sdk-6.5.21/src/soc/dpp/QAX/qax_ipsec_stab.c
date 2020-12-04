/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    qax_ipsec.c
 * Purpose: Manages IPSec block
 */

#include <shared/bsl.h>


#if !defined(INCLUDE_IPSEC)

#include <sal/compiler.h>
#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/ipsec.h>
#include <soc/portmode.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/QAX/qax_ipsec.h>


int
soc_qax_ipsec_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_qax_ipsec_config_set( int unit, bcm_ipsec_config_t *config)
{

    return SOC_E_UNAVAIL;
}



int
soc_qax_ipsec_tunnel_create( int unit, uint32  flags, uint32 *tunnel_id, const bcm_ipsec_tunnel_info_t *tunnel_info)
{
    return SOC_E_UNAVAIL;

}


int
soc_qax_ipsec_sa_create( int unit, uint32 flags, uint32 *spi_id, bcm_ipsec_sa_info_t *sa)
{
    return SOC_E_UNAVAIL;
}


int
soc_qax_ipsec_sa_key_update(
        int              unit,
        uint32           flags,
        uint32           spi_id,
    bcm_ipsec_sa_keys_t *sa_keys)
{
    return SOC_E_UNAVAIL;
}

int
soc_qax_ipsec_sa_to_tunnel_map_set(
        int             unit,
        uint32          tunnel_id,
        uint32          sa_id)

{
    return SOC_E_UNAVAIL;
}

int
soc_qax_ipsec_sa_to_tunnel_map_get(
        int             unit,
        uint32          sa_id,
        uint32          *tunnel_id)
{
    return SOC_E_UNAVAIL;
}

int soc_qax_ipsec_sa_delete(
        int             unit,
        uint32          flags,
        uint32          sa_id)
{
    return SOC_E_UNAVAIL;
}

int soc_qax_ipsec_tunnel_delete(
        int             unit,
        uint32          flags,
        uint32          tunnel_id)
{
    return SOC_E_UNAVAIL;
}

#endif

