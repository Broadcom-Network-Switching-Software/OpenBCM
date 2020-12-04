/*
 * $Id: ipsec.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Ipsec
 */

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/ipsec.h>

#include "bcm_int/common/debug.h"
#include <soc/dpp/QAX/qax_ipsec.h>
#include <soc/drv.h>


#define _ERR_MSG_MODULE_NAME BSL_BCM_IPSEC


int 
bcm_petra_ipsec_config_set(
    int unit, 
    bcm_ipsec_config_t *config)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_config_set(unit, config));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipsec_sa_create(
    int unit, 
    uint32 flags, 
    uint32 *sa_id, 
    bcm_ipsec_sa_info_t *sa)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_sa_create(unit, flags, sa_id, sa));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipsec_sa_delete(
    int unit, 
    uint32 flags, 
    uint32 sa_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_sa_delete(unit, flags, sa_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipsec_sa_to_tunnel_map_set(
    int unit, 
    uint32 tunnel_id, 
    uint32 sa_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_sa_to_tunnel_map_set(unit, tunnel_id, sa_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ipsec_sa_to_tunnel_map_get(
    int unit,
    uint32 sa_id,
    uint32 *tunnel_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_sa_to_tunnel_map_get(unit, sa_id, tunnel_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ipsec_sa_key_update(
    int unit, 
    uint32 flags, 
    uint32 sa_id, 
    bcm_ipsec_sa_keys_t *sa_keys)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_sa_key_update(unit, flags, sa_id, sa_keys));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipsec_tunnel_create(
    int unit,
    uint32 flags, 
    uint32 *tunnel_id, 
    bcm_ipsec_tunnel_info_t *tunnel_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_tunnel_create(unit, flags, tunnel_id, tunnel_info));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipsec_tunnel_delete(
    int unit, 
    uint32 flags, 
    uint32 tunnel_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit))           /* QAX only programs */
        BCMDNX_IF_ERR_EXIT(soc_qax_ipsec_tunnel_delete(unit, flags, tunnel_id));

exit:
    BCMDNX_FUNC_RETURN;
}

