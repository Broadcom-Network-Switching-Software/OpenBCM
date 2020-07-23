/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PPP

#include <shared/bsl.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/tunnel.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>

#include <soc/dpp/drv.h>

#include <bcm/ppp.h>

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 l2tp_terminator_sem_add(int unit, uint16 l2tp_session, uint16 l2tp_tunnel, uint32 vrf, int local_in_lif)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION;
	qual_vals[0].val.arr[0] = (l2tp_tunnel << 16) | l2tp_session;
		
	qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_VRF;
	qual_vals[1].val.arr[0] = vrf & 0xfff;
		
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0/*priority*/, (void *)&local_in_lif, &success));
	SOCDNX_SAND_IF_ERR_EXIT(success);

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 l2tp_terminator_sem_get(int unit, uint16 l2tp_session, uint16 l2tp_tunnel, uint32 vrf, int *local_in_lif, uint8 *found)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION;
	qual_vals[0].val.arr[0] = (l2tp_tunnel << 16) | l2tp_session;
		
	qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_VRF;
	qual_vals[1].val.arr[0] = vrf & 0xfff;
		
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, (void *)local_in_lif, NULL/*priority*/, NULL/*hit_bit*/, found));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 l2tp_terminator_sem_del(int unit, uint16 l2tp_session, uint16 l2tp_tunnel, uint32 vrf)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION;
	qual_vals[0].val.arr[0] = (l2tp_tunnel << 16) | l2tp_session;
		
	qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_VRF;
	qual_vals[1].val.arr[0] = vrf & 0xfff;
		
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
    SOCDNX_SAND_IF_ERR_EXIT(success);

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 pppoe_terminator_sem_add(int unit, uint32 global_in_ac, uint16 pppoe_session, int local_in_lif)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
	qual_vals[0].val.arr[0] = global_in_ac;
	
	qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID;
	qual_vals[1].val.arr[0] = pppoe_session;
		
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0/*=priority*/, (void *)&local_in_lif, &success));
    SOCDNX_SAND_IF_ERR_EXIT(success);
		
exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 pppoe_terminator_sem_get(int unit, uint32 global_in_ac, uint16 pppoe_session, int *local_in_lif, uint8 *found)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
	qual_vals[0].val.arr[0] = global_in_ac;
	
	qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID;
	qual_vals[1].val.arr[0] = pppoe_session;

	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, (void *)local_in_lif, NULL/*priority*/, NULL/*hit_bit*/, found));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
uint32 pppoe_terminator_sem_del(int unit, uint32 global_in_ac, uint16 pppoe_session)
{
	SOC_DPP_DBAL_SW_TABLE_IDS table_id = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	int i;
	BCMDNX_INIT_FUNC_DEFS;

	for(i = 0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
		qual_vals[i].type = 0xffffffff;
		qual_vals[i].val.arr[0] = 0; qual_vals[i].is_valid.arr[0] = 0;
		qual_vals[i].val.arr[1] = 0; qual_vals[i].is_valid.arr[1] = 0;
	}
		
	qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
	qual_vals[0].val.arr[0] = global_in_ac;
	
	qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID;
	qual_vals[1].val.arr[0] = pppoe_session;
		
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
    SOCDNX_SAND_IF_ERR_EXIT(success);

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int ppp_terminator_lif_set_sw_state(int unit, int local_in_lif, bcm_ppp_terminator_t *info)
{
	_bcm_dpp_gport_sw_resources gport_sw_resources;
	BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

	/*
	 * for pppoe terminator we are using the ac_lif and not allocating a new pppoe tunnel lif
	 * we store the data for the TT lookup in the x-connect fields 
	 */

	if(info->type == bcmPPPTypePPPoE && gport_sw_resources.in_lif_sw_resources.lif_type == _bcmDppLifTypeVlan) {
		gport_sw_resources.in_lif_sw_resources.gport_id = info->ppp_terminator_id; /* tunnel gport */
		gport_sw_resources.in_lif_sw_resources.match1 = info->session_id;
		gport_sw_resources.in_lif_sw_resources.peer_gport = info->network_domain; /* ac gport */
	} else {
		gport_sw_resources.in_lif_sw_resources.flags = info->flags;
		gport_sw_resources.in_lif_sw_resources.port = info->ppp_terminator_id;
		gport_sw_resources.in_lif_sw_resources.match1 = info->session_id;
		gport_sw_resources.in_lif_sw_resources.match2 = info->network_domain;
		gport_sw_resources.in_lif_sw_resources.match_tunnel = info->l2tpv2_tunnel_id;
		gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeL2tpTunnel;
	}

	BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int ppp_terminator_lif_get_sw_state(int unit, int local_in_lif, bcm_ppp_terminator_t *info)
{
	_bcm_dpp_gport_sw_resources gport_sw_resources;
	BCMDNX_INIT_FUNC_DEFS;

	BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

	sal_memset(info, 0, sizeof(*info));

	if(gport_sw_resources.in_lif_sw_resources.lif_type == _bcmDppLifTypeVlan && gport_sw_resources.in_lif_sw_resources.peer_gport != 0) {
		info->type = bcmPPPTypePPPoE;
		info->flags = 0;
		info->ppp_terminator_id = gport_sw_resources.in_lif_sw_resources.gport_id; /* tunnel gport */
		info->session_id = gport_sw_resources.in_lif_sw_resources.match1;
		info->network_domain = gport_sw_resources.in_lif_sw_resources.peer_gport; /* ac gport */
	} else if(gport_sw_resources.in_lif_sw_resources.lif_type == _bcmDppLifTypeL2tpTunnel) {
		info->l2tpv2_tunnel_id = gport_sw_resources.in_lif_sw_resources.match_tunnel;
		info->type = bcmPPPTypeL2TPv2;
		info->flags = gport_sw_resources.in_lif_sw_resources.flags;
		info->ppp_terminator_id = gport_sw_resources.in_lif_sw_resources.port;
		info->session_id = gport_sw_resources.in_lif_sw_resources.match1;
		info->network_domain = gport_sw_resources.in_lif_sw_resources.match2;
	} else
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not a ppp tunnel terminator.")));
	

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int pppoe_terminator_lif_alloc(int unit, int with_id, bcm_ppp_terminator_t *info, int *local_in_lif)
{
	_bcm_dpp_gport_hw_resources gport_hw_resources_in_ac;
	int global_in_lif;
	BCMDNX_INIT_FUNC_DEFS;

	if(with_id)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't create PPPoE terminator using WITH_ID.")));

	/* get global and local lif */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, info->network_domain, 
		_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
		&gport_hw_resources_in_ac));
	global_in_lif = gport_hw_resources_in_ac.global_in_lif;
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_in_lif, local_in_lif));

	BCM_GPORT_TUNNEL_ID_SET(info->ppp_terminator_id, global_in_lif);

	/* 
	 * Since all we need from the PPPoE lif is to set data in the wide extension and we have a pppoe_lif per in_ac
	 * we don't allocate a new lif, we are just using the existing in_ac and returning it's ID as tunnel gport 
	 */
		
exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int l2tp_terminator_lif_alloc(int unit, int with_id, bcm_ppp_terminator_t *info, int *local_in_lif)
{
	bcm_dpp_am_local_inlif_info_t local_inlif_info;
	int global_in_lif;
	uint32 alloc_flags = 0;
	BCMDNX_INIT_FUNC_DEFS;

    if (with_id) {
        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        global_in_lif =  BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id);
    }

	/* Set the local lif info */
	sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
	local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_l2tp;
	local_inlif_info.local_lif_flags = (info->flags & BCM_PPP_TERM_WIDE) ? 
											BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
	local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

	/* Allocate ingress-only global and local in_lif */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_in_lif, &local_inlif_info, NULL));

	/* Retrieve the allocated local lif */
	*local_in_lif = local_inlif_info.base_lif_id;
	LOG_VERBOSE(BSL_LS_BCM_PPP,
					(BSL_META_U(unit,
							"pppoe_terminator_lif_alloc: global_lif=0x%08x local_lif=0x%08x\n"), global_in_lif, *local_in_lif));

	BCM_GPORT_TUNNEL_ID_SET(info->ppp_terminator_id, global_in_lif);

	/* 
	 * Don't set any lif fields, we use only the wide data which will be set by the caller
	 */
		
exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int pppoe_terminator_verify(int unit, bcm_ppp_terminator_t *info)
{
	bcm_gport_t in_ac = info->network_domain;
	int is_local;
	BCMDNX_INIT_FUNC_DEFS;

	if(!SOC_DPP_PPPOE_IS_ENABLE(unit)) 
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PPPoE is not enabled")));
	
	if((info->flags & BCM_PPP_TERM_SESSION_ANTI_SPOOFING) == 0)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PPPoE terminator without anti-spoofing is not supported.")));

	if((info->flags & BCM_PPP_TERM_CROSS_CONNECT) == 0)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PPPoE terminator without cross-connect is not supported.")));

	if(info->flags & ~(BCM_PPP_TERM_SESSION_ANTI_SPOOFING|BCM_PPP_TERM_WITH_ID|BCM_PPP_TERM_REPLACE|BCM_PPP_TERM_WIDE|BCM_PPP_TERM_CROSS_CONNECT))
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("some flags are unsupported.")));

	/* verify that pppoe session is 16 bits */
	if (info->session_id > SOC_DPP_PPPOE_SESSION_ID_MAX)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PPPoE session id is longer than 16 bits")));

	/* verify in_ac is vlan port */
	if (!BCM_GPORT_IS_VLAN_PORT(in_ac)) 
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("in_ac is not vlan port.")));

	/* verify that in_ac is local */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_is_local(unit, in_ac, &is_local));
	if (!is_local)
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("in_ac is not local.")));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int l2tp_terminator_verify(int unit, bcm_ppp_terminator_t *info)
{
	BCMDNX_INIT_FUNC_DEFS;

	if(!SOC_DPP_L2TP_IS_ENABLE(unit)) 
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP is not enabled")));
	
	if((info->flags & BCM_PPP_TERM_CROSS_CONNECT) == 0)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP terminator without cross-connect is not supported.")));

	if(info->flags & ~(BCM_PPP_TERM_WITH_ID|BCM_PPP_TERM_REPLACE|BCM_PPP_TERM_WIDE|BCM_PPP_TERM_CROSS_CONNECT))
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("some flags are unsupported.")));

	/* verify that l2tp session is 16 bits */
	if (info->session_id > SOC_DPP_L2TP_SESSION_ID_MAX)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP session id is longer than 16 bits")));

	/* verify that l2tp tunnel is 16 bits */
	if (info->l2tpv2_tunnel_id > SOC_DPP_L2TP_TUNNEL_ID_MAX)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP tunnel id is longer than 16 bits")));

	/* verify that vrf is 12 bits */
	if (info->network_domain > SOC_DPP_L2TP_VRF_MAX)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP tunnel id is longer than 12 bits")));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
/* Create a new PPP terminator. */
int bcm_petra_ppp_terminator_create(
    int unit, 
    bcm_ppp_terminator_t *info)
{
	uint8 found;
	int local_in_lif, global_in_lif;
	int replace = 0, with_id = 0;
    bcm_ppp_terminator_t pt;
	BCMDNX_INIT_FUNC_DEFS;

	BCM_DPP_UNIT_CHECK(unit);
	BCMDNX_NULL_CHECK(info);

	if(info->flags & BCM_PPP_TERM_WITH_ID)
		with_id = 1;
	if(info->flags & BCM_PPP_TERM_REPLACE)
		replace = 1;
	
    if (replace && !with_id)
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_PPP_TERM_REPLACE flag is set, then BCM_PPP_TERM_WITH_ID flag must also be set")));

	if(replace) {
	
		/* get global and local lif */
		global_in_lif =  BCM_GPORT_TUNNEL_ID_GET(info->ppp_terminator_id);
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_in_lif, &local_in_lif));
	
		/* get lif data from sw state */
		BCMDNX_IF_ERR_EXIT(ppp_terminator_lif_get_sw_state(unit, local_in_lif, &pt));
	}
		
	if(info->type == bcmPPPTypePPPoE) {
		
		_bcm_dpp_gport_hw_resources gport_hw_resources_in_ac;
		bcm_gport_t in_ac;
		uint16 pppoe_session;
		int global_in_ac;

		/* verify params */
		BCMDNX_IF_ERR_EXIT(pppoe_terminator_verify(unit, info));
		
		/* if updating than del existing sem entry */
		if(replace) {

			if(pt.type != bcmPPPTypePPPoE)
				BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("sw state tunnel type does not match.")));
			pppoe_session = pt.session_id;
			in_ac = pt.network_domain;
			/* get global lif_id */
			BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, in_ac, 
				_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
				&gport_hw_resources_in_ac));
			global_in_ac = gport_hw_resources_in_ac.global_in_lif;
		
			BCMDNX_IF_ERR_EXIT(pppoe_terminator_sem_del(unit, global_in_ac, pppoe_session));
		}

		pppoe_session = info->session_id;
		in_ac = info->network_domain;
		/* get global lif_id */
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, in_ac, 
			_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
			&gport_hw_resources_in_ac));
		global_in_ac = gport_hw_resources_in_ac.global_in_lif;

		/* verify that match does not exist in SEM table */
		BCMDNX_IF_ERR_EXIT(pppoe_terminator_sem_get(unit, global_in_ac, pppoe_session, &local_in_lif, &found));
		if(found)
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Trying to add existing match. in_ac: %d, pppoe_session: %d."), in_ac, pppoe_session));

		/* allocate lif */
		if(!replace)
			BCMDNX_IF_ERR_EXIT(pppoe_terminator_lif_alloc(unit, with_id, info, &local_in_lif));

		/* add SEM match */
		BCMDNX_IF_ERR_EXIT(pppoe_terminator_sem_add(unit, global_in_ac, pppoe_session, local_in_lif));

	} else if(info->type == bcmPPPTypeL2TPv2) {
		
		uint32 vrf;
		uint16 l2tp_session, l2tp_tunnel;

		/* verify params */
		BCMDNX_IF_ERR_EXIT(l2tp_terminator_verify(unit, info));
		
		/* if updating than del existing sem entry */
		if(replace) {

			if(pt.type != bcmPPPTypeL2TPv2)
				BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("sw state tunnel type does not match.")));
			
			vrf = pt.network_domain;
			l2tp_session = pt.session_id;
			l2tp_tunnel = pt.l2tpv2_tunnel_id;
		
			BCMDNX_IF_ERR_EXIT(l2tp_terminator_sem_del(unit, l2tp_session, l2tp_tunnel, vrf));
		}

		vrf = info->network_domain;
		l2tp_session = info->session_id;
		l2tp_tunnel = info->l2tpv2_tunnel_id;
		
		/* verify that match does not exist in SEM table */
		BCMDNX_IF_ERR_EXIT(l2tp_terminator_sem_get(unit, l2tp_session, l2tp_tunnel, vrf, &local_in_lif, &found));
		if(found)
			BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Trying to add existing match. l2tp_tunnel: %d, session: %d, vrf: %d."), l2tp_tunnel, l2tp_session, vrf));

		/* allocate lif */
		if(!replace)
			BCMDNX_IF_ERR_EXIT(l2tp_terminator_lif_alloc(unit, with_id, info, &local_in_lif));

		/* add SEM match */
		BCMDNX_IF_ERR_EXIT(l2tp_terminator_sem_add(unit, l2tp_session, l2tp_tunnel, vrf, local_in_lif));

	} else 
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API not yet supported.")));

	/* store in SW state */
	BCMDNX_IF_ERR_EXIT(ppp_terminator_lif_set_sw_state(unit, local_in_lif, info));

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
/* Delete a PPP terminator. */
int bcm_petra_ppp_terminator_delete(
    int unit, 
    bcm_ppp_terminator_t *info)
{
	int global_in_lif, local_in_lif;
	_bcm_dpp_gport_hw_resources gport_hw_resources_in_lif;
	bcm_ppp_terminator_t pt;
	uint16 pppoe_session;
	_bcm_dpp_gport_hw_resources gport_hw_resources_in_ac;
	bcm_gport_t in_ac;
	int global_in_ac;
	uint32 vrf;
	uint16 l2tp_session, l2tp_tunnel;
	BCMDNX_INIT_FUNC_DEFS;

	BCM_DPP_UNIT_CHECK(unit);
	BCMDNX_NULL_CHECK(info);

	/* get local lif */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, info->ppp_terminator_id, 
		_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
		&gport_hw_resources_in_lif));
	local_in_lif = gport_hw_resources_in_lif.local_in_lif;

	/* get lif data from sw state */
	BCMDNX_IF_ERR_EXIT(ppp_terminator_lif_get_sw_state(unit, local_in_lif, &pt));

	if(pt.ppp_terminator_id != info->ppp_terminator_id)
		BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("sw state port_id does not match.")));

	/* del sem entry */
	if(pt.type == bcmPPPTypePPPoE) {

		pppoe_session = pt.session_id;

		in_ac = pt.network_domain;
		/* get global lif_id */
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, in_ac, 
			_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
			&gport_hw_resources_in_ac));
		global_in_ac = gport_hw_resources_in_ac.global_in_lif;

		BCMDNX_IF_ERR_EXIT(pppoe_terminator_sem_del(unit, global_in_ac, pppoe_session));

		/* no need to delete lif, just delete tunnel from sw state */
		pt.ppp_terminator_id = 0;
		pt.session_id = 0;
		pt.network_domain = 0;
		BCMDNX_IF_ERR_EXIT(ppp_terminator_lif_set_sw_state(unit, local_in_lif, &pt));
		
	} else {

		vrf = pt.network_domain;
		l2tp_session = pt.session_id;
		l2tp_tunnel = pt.l2tpv2_tunnel_id;

		BCMDNX_IF_ERR_EXIT(l2tp_terminator_sem_del(unit, l2tp_session, l2tp_tunnel, vrf));

		/* get global lif */
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, info->ppp_terminator_id, 
			_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
			&gport_hw_resources_in_lif));
		global_in_lif = gport_hw_resources_in_lif.global_in_lif;

		/* de-allocate lif */
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_delete_global_and_local_lif(unit, global_in_lif, local_in_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID));

		/* delete sw state */
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_delete(unit, local_in_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS));
	}

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
/* Get a PPP terminator. */
int bcm_petra_ppp_terminator_get(
    int unit, 
    bcm_ppp_terminator_t *info)
{
	int local_in_lif;
	_bcm_dpp_gport_hw_resources gport_hw_resources_in_lif;
	bcm_ppp_terminator_t pt;
	BCMDNX_INIT_FUNC_DEFS;

	BCM_DPP_UNIT_CHECK(unit);
	BCMDNX_NULL_CHECK(info);

	/* get local lif */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, info->ppp_terminator_id, 
		_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
		&gport_hw_resources_in_lif));
	local_in_lif = gport_hw_resources_in_lif.local_in_lif;

	/* get lif data from sw state */
	BCMDNX_IF_ERR_EXIT(ppp_terminator_lif_get_sw_state(unit, local_in_lif, &pt));

	if(pt.ppp_terminator_id != info->ppp_terminator_id)
		BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("sw state port_id does not match.")));

	*info = pt;

exit:
	BCMDNX_FUNC_RETURN;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
/* Create a new PPP initiator. */
int bcm_petra_ppp_initiator_create(
    int unit, 
    bcm_ppp_initiator_t *info)
{
    int global_out_lif, local_out_lif;
	BCMDNX_INIT_FUNC_DEFS;

	BCM_DPP_UNIT_CHECK(unit);
	BCMDNX_NULL_CHECK(info);

	if(info->type == bcmPPPTypeL2TPv2) {

		bcm_tunnel_initiator_t data_entry_tunnel;

		if(!SOC_DPP_L2TP_IS_ENABLE(unit)) 
			BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("PPP tunnel type is bcmPPPTypeL2TPv2 but L2TP is not enabled.")));

		/* verify that l2tp session is 16 bits */
		if (info->session_id & 0xffff0000)
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP session id is longer than 16 bits")));
		
		/* verify that l2tp tunnel is 16 bits */
		if (info->l2tpv2_tunnel_id& 0xffff0000)
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("L2TP tunnel id is longer than 16 bits")));
		
		/* create egress data_entry */
		bcm_tunnel_initiator_t_init(&data_entry_tunnel);
		data_entry_tunnel.l3_intf_id = info->l3_intf_id;
		data_entry_tunnel.aux_data = (info->session_id & 0xffff) << 16;
		data_entry_tunnel.flow_label = info->l2tpv2_tunnel_id & 0xffff;

		if(info->flags & BCM_PPP_INIT_WITH_ID) {
			data_entry_tunnel.flags |= BCM_TUNNEL_WITH_ID;
			data_entry_tunnel.tunnel_id = info->ppp_initiator_id;
		} 

		if(info->flags & BCM_PPP_INIT_REPLACE) {
			data_entry_tunnel.flags |= BCM_TUNNEL_REPLACE;
			data_entry_tunnel.tunnel_id = info->ppp_initiator_id;
		} 

		data_entry_tunnel.ttl = 2; /* allocate l2tp data entry */
		data_entry_tunnel.dip = BCM_L3_ITF_VAL_GET(info->l3_intf_id); /* l2tp data entry will point to this egress object */

		BCMDNX_IF_ERR_EXIT(bcm_petra_data_entry_set(unit, &data_entry_tunnel));

		/* update lif usage */
		global_out_lif = BCM_GPORT_TUNNEL_ID_GET(data_entry_tunnel.tunnel_id);
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_out_lif, &local_out_lif));
		BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_set(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif, 0, _bcmDppLifTypeL2tpTunnelDataEntry));

		info->ppp_initiator_id = data_entry_tunnel.tunnel_id;
	} else {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PPP initiator type.")));
	}

exit:
	BCMDNX_FUNC_RETURN;
}


/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
/* Delete a PPP initiator. */
int bcm_petra_ppp_initiator_delete(
    int unit, 
    bcm_ppp_initiator_t *info)
{
	bcm_l3_intf_t l3_tunnel_intf;
	_bcm_dpp_gport_hw_resources gport_hw_resources_out_lif;
	int local_out_lif;
	_bcm_lif_type_e out_lif_usage;
	BCMDNX_INIT_FUNC_DEFS;

	BCM_DPP_UNIT_CHECK(unit);
	BCMDNX_NULL_CHECK(info);

	if(!SOC_DPP_L2TP_IS_ENABLE(unit)) 
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("PPP tunnel type is bcmPPPTypeL2TPv2 but L2TP is not enabled.")));

	/* get local lif */
	BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, info->ppp_initiator_id, 
		_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
		&gport_hw_resources_out_lif));
	local_out_lif = gport_hw_resources_out_lif.local_out_lif;

	/* verify lif usage */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif, NULL, &out_lif_usage));
    if(out_lif_usage != _bcmDppLifTypeL2tpTunnelDataEntry)
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF is not L2TP tunnel initiator")));

	bcm_l3_intf_t_init(&l3_tunnel_intf);
	l3_tunnel_intf.l3a_source_vp = info->ppp_initiator_id;

	BCMDNX_IF_ERR_EXIT(bcm_petra_data_entry_clear(unit, &l3_tunnel_intf));

	BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif, NULL, &out_lif_usage));

exit:
	BCMDNX_FUNC_RETURN;
}

