/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam_hw_db.c
 * Purpose: OAM HW DB (MEP-DB, RMEP-DB, ...)
 */

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#define OAM_HASHS_ACCESS  OAM_ACCESS.oam_hashs

#define ENDP_LST_ACCESS          OAM_ACCESS.endp_lst
#define ENDP_LST_ACCESS_DATA     OAM_ACCESS.endp_lst.endp_lsts_array
#define ENDP_LST_M_ACCESS        OAM_ACCESS.endp_lst_m
#define ENDP_LST_M_ACCESS_DATA   OAM_ACCESS.endp_lst_m.endp_lst_ms_array


/**
 * Set the port_status, interface_status along with approriate
 * flags in endpoint_info accirdingly to the remote_state.
 * The mapping is {PortState[0:1], InterfaceState[2:0]}
 */
#define _BCM_DPP_OAM_RMEP_STATE_TO_PORT_INTERFACE_STATUS(remote_state, endpoint_info)\
do {\
	if ((remote_state) & 0x7) {\
		(endpoint_info)->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;\
		(endpoint_info)->interface_state = (remote_state) & 0x7;\
	}\
	if (((remote_state)>>3) & 0x3) {\
		(endpoint_info)->flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;\
		(endpoint_info)->port_state = ((remote_state)>>3) & 0x3;\
	}\
} while (0)

#define _BCM_DPP_OAM_RMEP_STATE_FROM_PORT_INTERFACE_STATUS(remote_state, endpoint_info)\
do {\
        (remote_state) = (((endpoint_info)->port_state << 3) | ((endpoint_info)->interface_state));\
} while (0)


/* May also return error*/
#define _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(bcm_mep_type, mep_db_mep_type) \
 	switch (bcm_mep_type) {\
	case bcmOAMEndpointTypeEthernet:\
		mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_ETH_OAM;\
		break;\
	case bcmOAMEndpointTypeBHHMPLS:\
		mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ;\
		break;\
	case bcmOAMEndpointTypeBHHPwe:\
		mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;\
		break;\
    case bcmOAMEndpointTypeBHHPweGAL:\
		mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL ;\
		break;\
    case bcmOAMEndpointTypeMPLSNetwork:\
        mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS;\
        break;\
    case bcmOAMEndpointTypeBhhSection:\
		mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION;\
		break;\
    case bcmOAMEndpointTypeMplsLmDmPw:\
        mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE;\
        break;\
    case bcmOAMEndpointTypeMplsLmDmLsp:\
        mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP;\
        break;\
    case bcmOAMEndpointTypeMplsLmDmSection:\
        mep_db_mep_type = SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION;\
        break;\
	default:\
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));\
	}\

/* in OAM statistics per mep functionality the counter Id is inserted into the LEM*/
int _bcm_oam_stat_lem_entry_add(uint32 unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, bcm_oam_endpoint_info_t *endpoint_info) {
	int rv;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_LEM_ACCESS_PAYLOAD payload;
	_bcm_dpp_gport_info_t gport_info;
	BCMDNX_INIT_FUNC_DEFS;

	/* add entry for eth-am up mep*/
    if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {

        ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
    	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
    	payload.dest = endpoint_info->id;
    	qual_vals[0].val.arr[0] = classifier_mep_entry->lif;
    	qual_vals[0].val.arr[1] = 0;
    	qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    	qual_vals[1].val.arr[0] = SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM;
    	qual_vals[1].val.arr[1] = 0;
    	qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_OPCODE;
	    qual_vals[2].val.arr[0] = ((classifier_mep_entry->md_level)*2);
    	qual_vals[2].val.arr[1] = 0;
    	qual_vals[2].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;
    	rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, qual_vals ,0, &payload, &success);
    	BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint32 sys_port;

        if (BCM_GPORT_IS_MCAST(endpoint_info->tx_gport)){
            /* add entry for eth-oam down mep untagged*/
            sys_port = BCM_GPORT_MCAST_GET(endpoint_info->tx_gport);
        } else {
            /* add entry for eth-oam down mep untagged*/
            rv = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
            BCMDNX_IF_ERR_EXIT(rv);
            /* 0xc0000 - encoding of dest system port */
            sys_port = (gport_info.sys_port | 0xC0000);
        }
    	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
    	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
    	payload.dest = endpoint_info->id;
    	qual_vals[0].val.arr[0] = sys_port;
    	qual_vals[0].val.arr[1] = 0;
    	qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    	qual_vals[1].val.arr[0] = (endpoint_info->level<<1);
    	qual_vals[1].val.arr[1] = 0;
    	qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED;

    	rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS, qual_vals ,0, &payload, &success);
    	BCMDNX_IF_ERR_EXIT(rv);


    	/* add entry for eth-oam down mep single tag*/
    	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
    	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
    	payload.dest = endpoint_info->id;
    	qual_vals[0].val.arr[0] = sys_port;
    	qual_vals[0].val.arr[1] = 0;
    	qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    	qual_vals[1].val.arr[0] = (endpoint_info->level<<1);
    	qual_vals[1].val.arr[1] = 0;
    	qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG;
    	qual_vals[2].val.arr[0] = ((endpoint_info->pkt_pri & 0xf)<<12) | (endpoint_info->vlan & 0xfff) ;
    	qual_vals[2].val.arr[1] = 0;
    	qual_vals[2].type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;

    	rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS, qual_vals ,0, &payload, &success);
    	BCMDNX_IF_ERR_EXIT(rv);

    	/* add entry for eth-oam down mep double tag*/
    	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
    	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
    	payload.dest = endpoint_info->id;
    	qual_vals[0].val.arr[0] = sys_port;
    	qual_vals[0].val.arr[1] = 0;
    	qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD;
    	qual_vals[1].val.arr[0] = (endpoint_info->level<<1);
    	qual_vals[1].val.arr[1] = 0;
    	qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG;
    	qual_vals[2].val.arr[0] = ((endpoint_info->pkt_pri & 0xf)<<12) | (endpoint_info->vlan & 0xfff) ;
    	qual_vals[2].val.arr[1] = 0;
    	qual_vals[2].type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    	qual_vals[3].val.arr[0] = ((endpoint_info->inner_pkt_pri & 0xf)<<12) | (endpoint_info->inner_vlan & 0xfff) ;
    	qual_vals[3].val.arr[1] = 0;
    	qual_vals[3].type = SOC_PPC_FP_QUAL_TM_INNER_TAG;

    	rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS, qual_vals ,0, &payload, &success);
    	BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Free all data associated with an RMEP-DB entry struct.
 *
 * @author sinai (16/04/2015)
 *
 * @param unit
 * @param rmep_index_internal
 *
 * @return int
 */
int _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(int unit, uint32 rmep_index_internal) {
    uint32 soc_sand_rv;
    int is_last;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS(unit)) {
        SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
        SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA punt_profile;

        soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_index_internal, &rmep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* rmep_db_entry is initialized by soc_ppd_oam_oamp_rmep_get */
        /* coverity[uninit_use_in_call:FALSE] */
        rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_free(unit, rmep_db_entry.punt_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_punt_event_hendling_profile_set,
                                                    (unit, rmep_db_entry.punt_profile, &punt_profile));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Fill RMEP-entry struct */
int _bcm_oam_rmep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry) {
	int rv = 0;
	SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	int is_allocated;
	int profile;
	uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;


	rmep_db_entry->ccm_period = endpoint_info->ccm_period*1000;

	rmep_db_entry->loc_clear_threshold = endpoint_info->loc_clear_threshold;

	rmep_db_entry->last_ccm_lifetime_valid_on_create = ((endpoint_info->faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) == BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) ? 1 : 0;

	if (SOC_IS_ARADPLUS(unit)) {
		SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
		punt_profile_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
		punt_profile_data.punt_rate = (endpoint_info->sampling_ratio > 0) ? endpoint_info->sampling_ratio - 1 : 0;
		punt_profile_data.rx_state_update_enable =   (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE ) ? 1 :
			                                         (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE ) ? 3 : 2;
		punt_profile_data.scan_state_update_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                     (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 :
			                                         (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) ? 3 : 2;
		punt_profile_data.mep_rdi_update_loc_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC) !=0;
		punt_profile_data.mep_rdi_update_loc_clear_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR) !=0;
		punt_profile_data.mep_rdi_update_rx_enable = (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI) !=0;
		rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(unit, 0 /*flags*/, &punt_profile_data, &is_allocated, &profile);
		BCMDNX_IF_ERR_EXIT(rv);
		if (is_allocated) {
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_punt_event_hendling_profile_set,
                                                    (unit, profile, &punt_profile_data));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            BCM_DPP_UNIT_CHECK(unit);
		}
		rmep_db_entry->punt_profile = profile;
		if (SOC_IS_ARADPLUS(unit)) {
			_BCM_DPP_OAM_RMEP_STATE_FROM_PORT_INTERFACE_STATUS(rmep_db_entry->rmep_state, endpoint_info);
		}

	}
	else {
		rmep_db_entry->is_state_auto_handle = ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE) != 0);
		rmep_db_entry->is_event_mask = ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) != 0);
	}


exit:
    BCMDNX_FUNC_RETURN;
}


/* Read RMEP-entry struct */
int _bcm_oam_rmep_db_entry_struct_get(int unit, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY * rmep_db_entry, uint16 rmep_id, uint32 local_id, bcm_oam_endpoint_info_t *endpoint_info) {
	SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
	uint32 soc_sand_rv;

	BCMDNX_INIT_FUNC_DEFS;
	endpoint_info->name = rmep_id;
	endpoint_info->local_id = local_id;
    endpoint_info->ccm_period = SOC_SAND_DIV_ROUND(rmep_db_entry->ccm_period,1000);
    endpoint_info->loc_clear_threshold = rmep_db_entry->loc_clear_threshold;
	endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE;
	endpoint_info->faults |= rmep_db_entry->rdi_received ? BCM_OAM_ENDPOINT_FAULT_REMOTE : 0;
	endpoint_info->faults |= rmep_db_entry->loc ? BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT : 0;
	if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
		SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_punt_event_hendling_profile_get,
                                                 (unit, rmep_db_entry->punt_profile, &punt_profile_data));
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
        endpoint_info->sampling_ratio = punt_profile_data.punt_enable ? punt_profile_data.punt_rate + 1 : 0;
        switch (punt_profile_data.rx_state_update_enable) {
        case 0:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE;
            break;
        default:
            break;
        }
        switch (punt_profile_data.scan_state_update_enable) {
        case 0:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case 1:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE;
            break;
        case 3:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE;
            break;
        default:
			break;
        }

		if (SOC_IS_ARADPLUS(unit)) {
			_BCM_DPP_OAM_RMEP_STATE_TO_PORT_INTERFACE_STATUS(rmep_db_entry->rmep_state, endpoint_info);
		}

        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_loc_enable ? BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC : 0;
        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_loc_clear_enable ? BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR : 0;
        endpoint_info->flags2 |= punt_profile_data.mep_rdi_update_rx_enable ? BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI : 0;
	} else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
		endpoint_info->flags |= rmep_db_entry->is_state_auto_handle ? BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE : 0;
		endpoint_info->flags |= rmep_db_entry->is_event_mask ? BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE : 0;
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Free all data associated with a MEP DB entry:
 * MPLS-push profile, local_port_2_system_port profile, ITMH
 * attributes, and address
 *
 * @author sinai (16/04/2015)
 *
 * @param unit
 * @param mep_index
 * @param mep_db_entry
 *
 * @return int
 */
int _bcm_oam_mep_db_entry_dealloc(int unit, uint32 mep_index, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info) {
    int rv;
    int dont_care,is_last;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit) && mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        rv = _bcm_dpp_am_template_oam_sa_mac_address_free(unit, mep_db_entry->src_mac_msb_profile, &dont_care);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ||
        mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION    ||
        mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE    ||
        mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS  ||
        mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE   ||
        mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL ) {
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_free(unit, mep_db_entry->push_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;
            SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
            soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type) &&
         ((!(mep_info->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)) || (mep_info->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) || mep_db_entry->system_port==0xffff)) {
        int ignored;
        rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit, mep_db_entry->local_port, &ignored);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((mep_info->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) && (mep_info->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER && mep_info->port)) {
            /* Free the entry in the OAMP-PE gen memory entry*/
            rv = _bcm_dpp_am_template_oam_oamp_pe_gen_mem_free(unit, mep_index, &ignored);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = _bcm_dpp_am_template_oam_tx_priority_free(unit, mep_db_entry->priority, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_last) {
        SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
        SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
        soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill MEP-entry struct */
int _bcm_oam_mep_db_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, int icc_ndx, _bcm_oam_ma_name_t *group_name_struct, int do_not_set_mep_db_ccm_interval)
{
    uint32 ret;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    uint32 soc_sand_rv;
    int tpid_index;
    uint32 ccm_period_ms;
    int is_allocated, push_profile;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    int priority;
    uint32 alloc_flags;
    _bcm_dpp_gport_info_t gport_info;
    int ignored, local_port;
    uint32      pp_port;
    int         core;
    int bank, entry_idx;
    int using_fake_destination = (SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx && !endpoint_info->ccm_period && (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) && !SOC_IS_QAX(unit));

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, mep_db_entry->mep_type);

    if(SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit) && (SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP == mep_db_entry->mep_type)
        &&(endpoint_info->mpls_out_gport == endpoint_info->intf_id)){
          mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;  /* pseudo PWE, it's real LSP type*/
          mep_db_entry->is_lsp = TRUE;
    }

    if (SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)) {
        mep_db_entry->mep_type =  (endpoint_info->mpls_network_info.function_type == 0x1) ? SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS : SOC_PPC_OAM_MEP_TYPE_Y1711_PWE;
    }

    /* For Section OAM re use Y1731_O_PWE MEP_DB type */
    if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) {
        mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
    }

    if (_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type))
    {
        mep_db_entry->is_ilm = ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) == BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM);
        mep_db_entry->session_identifier_id = endpoint_info->session_id;
        mep_db_entry->timestamp_format = (endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) ? 1 : 0;
    }

    mep_db_entry->remote_recycle_port = 0;

    if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        /* src mac handled differently for Arad and Jericho*/
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            /* common utility: always returns success */
            ret = _bcm_petra_mac_to_sand_mac(endpoint_info->src_mac_address, &(mep_db_entry->src_mac_address));
            BCMDNX_IF_ERR_EXIT(ret);
        } else {
            uint8 src_mac_address_msbs[5]={0};
            int profile;

            mep_db_entry->src_mac_lsb = endpoint_info->src_mac_address[5];
            sal_memcpy(src_mac_address_msbs,&(endpoint_info->src_mac_address),5 );

            ret = _bcm_dpp_am_template_oam_sa_mac_address_alloc(unit,0,src_mac_address_msbs,&is_allocated,&profile);
            BCMDNX_IF_ERR_EXIT_MSG(ret, (_BSL_BCM_MSG("Only two different values for the source MAC 5 MSBs allowed.")));
            mep_db_entry->src_mac_msb_profile =profile;

            if (is_allocated) {
                ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_sa_addr_msbs_set, (unit, profile, src_mac_address_msbs));
                BCMDNX_IF_ERR_EXIT(ret);
                BCM_DPP_UNIT_CHECK(unit);
            }
        }
    }

    if (!_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type))
    {
        mep_db_entry->icc_ndx = icc_ndx;
        if (SOC_IS_ARADPLUS(unit)) {
            mep_db_entry->is_11b_maid = (group_name_struct->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES);
        }

        if (SOC_IS_QAX(unit) && (group_name_struct->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
            /* QAX only - pointer to extended data header */
            mep_db_entry->is_maid_48 = TRUE;

            if (SOC_IS_QUX(unit)) {
                bank = OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(group_name_struct->index);
                entry_idx = OAMP_MEP_DB_ENTRY_ID_TO_INDEX(group_name_struct->index);
                mep_db_entry->flex_data_ptr = bank * 1024 + entry_idx;
            } else {
                mep_db_entry->flex_data_ptr = group_name_struct->index;
            }
        }
    }

    /* Handle the local port/system port*/
    if ((!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) || _BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
        /* Down MEP: system port (taken from tx-gport) should go on the ITMH.*/
        /* Server: down MEP acts like normal down MEP, up MEP allocates an entry in this table as well for an ITMH built by the OAMP-PE.
           In the latter case the destination is taken from the remote_gport field which should represent the remote device recycle port.*/
        uint32 sys_port=0;
        bcm_gport_t gport_to_use = (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) ?
            endpoint_info->remote_gport : endpoint_info->tx_gport;

        if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
                uint32 local_port_on_inner_ptch;
                uint32 tm_port = 0, modid_ignored;

                /* In this special case local port serves three purposes:
                   1) pointer to local_port_2_sys_port which will serve as the DSP on the ITMH
                      which should be the remote device recycle port and should be retreived from the remote_gport.
                   ARAD:
                   2)  the LSB of the src mac address.*/
                   

                /* From the tx gport we get the physical port and from that the local port, which presumably may be on a different device */
                ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->tx_gport, 0, &gport_info);
                BCMDNX_IF_ERR_EXIT(ret);
                ret = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sys_phys_to_local_port_map_get, (unit, gport_info.sys_port, &modid_ignored, &tm_port)));
                BCM_SAND_IF_ERR_EXIT(ret);
                BCM_DPP_UNIT_CHECK(unit);
                /*tm --> pp*/
                local_port_on_inner_ptch = tm_port; /* Core can be ignored, Inner PTCH accssed after packets reached the right core. */

                /* From the remote_gport get the remote_recycle_port */
                ret = _bcm_dpp_gport_to_phy_port(unit, gport_to_use, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                BCMDNX_IF_ERR_EXIT(ret);
                mep_db_entry->remote_recycle_port = gport_info.sys_port;

                if (SOC_IS_JERICHO(unit)) {
                    if (!using_fake_destination) {
                        ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, 0, &gport_info.sys_port, &ignored, &local_port);
                        BCMDNX_IF_ERR_EXIT(ret);
                    }
                    /* Store local port for inner PTCH on counter_pointer field on MEPDB which is free to use in up MEPs.
                       PRGE prgram will take it and paste it on the inner PTCH itself. */
                    mep_db_entry->counter_pointer = local_port_on_inner_ptch;
                } else { /* A+ */
                    local_port = endpoint_info->src_mac_address[5]; /* Core will always be 0 in Arad anyways.*/
                    /* Arad counter_pointer serves as the tx_gport, which is the local port on the inner ptch.*/
                    mep_db_entry->counter_pointer = tm_port << 1;  /* The leftmost bit cannot be used by the OAMP. shift by one.*/
                    if (!using_fake_destination) {
                        ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, SHR_RES_ALLOC_WITH_ID, &gport_info.sys_port, &ignored, &local_port);
                        BCMDNX_IF_ERR_EXIT(ret);
                    }
                }

                mep_db_entry->local_port = local_port;
            } else {
                ret = _bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info->tx_gport, &sys_port);
                BCMDNX_IF_ERR_EXIT(ret);

                /* check if destination is mc */
                if (BCM_GPORT_IS_MCAST(endpoint_info->tx_gport) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_itmh_mc", 0)){
                    mep_db_entry->is_mc = 1;
                }

                if (!using_fake_destination) {
                    /* If the above condition is satisfied an illegal destination will be allocated later in this function */
                    /* Normal case: Allocating for the LOCAL_PORT_2_SYS_PORT table, used only in down-MEPS, Arad B0 and above*/
                    BCM_DPP_UNIT_CHECK(unit);/* not really needed. just to aviod coverity defect */
                    alloc_flags = SOC_IS_JERICHO(unit) ? 0 : SHR_RES_ALLOC_WITH_ID;
                    local_port = endpoint_info->src_mac_address[5]; /* In Jericho this will be overriden.*/
                    ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit,alloc_flags,&sys_port,&ignored,&local_port);
                    BCMDNX_IF_ERR_EXIT(ret);
                    mep_db_entry->local_port = local_port;
                }
            }
        } else {
            ret = _bcm_dpp_gport_to_phy_port(unit, gport_to_use, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
            BCMDNX_IF_ERR_EXIT(ret);
            sys_port = gport_info.sys_port;
            mep_db_entry->local_port = gport_info.sys_port;
        }

        if (!_BCM_OAM_IS_SERVER_SERVER(endpoint_info) || !_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            /* all cases except server - up mep.*/
            mep_db_entry->system_port = sys_port;
        }
    } else {
        /* in this case local port is taken from the gport. set it. */
        _bcm_dpp_gport_info_t local_gport_info;
        ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &local_gport_info);
        BCMDNX_IF_ERR_EXIT(ret);
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_gport_info.local_port, &pp_port, &core)));

        mep_db_entry->local_port = pp_port | core << 8;/* The leftmost bit in the local port for Up-MEP represents the core in Jericho.*/
        if (!_BCM_OAM_IS_SERVER_SERVER(endpoint_info) && SOC_IS_ARADPLUS_A0(unit) ) {
            /* Up MEP case. The LSB of the MAC address will be set on the counter pointer (which is otherwise unused for up-MEPs)
               and taken by the OAMP PE and set on the actual src MAC addr*/
            mep_db_entry->counter_pointer = endpoint_info->src_mac_address[5] <<1;  /* The leftmost bit cannot be used by the OAMP. shift by one.*/
        }
    }

    /* Not relevant for RFC6374 */    
    if (!_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type))
    {
        mep_db_entry->mdl = endpoint_info->level;
        mep_db_entry->mep_id = endpoint_info->name;
        mep_db_entry->rdi = ((endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) != 0);
    }

    /* ITMH attributes profile allocate */
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
    /* int_pri: ((COS & 0x7) << 2) + (DP & 0x3)) */
    tx_itmh_attributes.tc = (endpoint_info->int_pri & 0x1F) >> 2;
    tx_itmh_attributes.dp = endpoint_info->int_pri & 0x3;
    ret = _bcm_dpp_am_template_oam_tx_priority_alloc(unit, 0/*flags*/, &tx_itmh_attributes, &is_allocated, &priority);
    BCMDNX_IF_ERR_EXIT(ret);
    mep_db_entry->priority = (uint8)priority;
    if (is_allocated) {
        soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

	if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
		mep_db_entry->is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);

		if (endpoint_info->outer_tpid == 0) { /* when single tag - outer tag fields are used */
			if (endpoint_info->inner_tpid != 0) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Inner tpid can not be configured without an outer tpid.\n")));
			}
			mep_db_entry->tags_num = 0;
		} else {
			soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			ret = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, endpoint_info->outer_tpid, &tpid_vals, &tpid_index);
			BCMDNX_IF_ERR_EXIT(ret);
			mep_db_entry->outer_tpid = (uint8)tpid_index;
			mep_db_entry->outer_vid_dei_pcp =  endpoint_info->vlan + (endpoint_info->pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
			if (endpoint_info->inner_tpid != 0) { /* double tag */
				mep_db_entry->tags_num = 2;
				ret = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, endpoint_info->inner_tpid, &tpid_vals, &tpid_index);
				BCMDNX_IF_ERR_EXIT(ret);
				mep_db_entry->inner_tpid = (uint8)tpid_index;
				mep_db_entry->inner_vid_dei_pcp = endpoint_info->inner_vlan + (endpoint_info->inner_pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
			} else { /* double tag */
				mep_db_entry->tags_num = 1;
			}
		}
	}
    else {

		SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;

		SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        if ((endpoint_info->type == bcmOAMEndpointTypeBhhSection) || (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmSection)) {
            mep_db_entry->label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
        } else {
            mep_db_entry->label = endpoint_info->egress_label.label;
        }
		push_data.ttl = endpoint_info->egress_label.ttl;
		push_data.exp = endpoint_info->egress_label.exp;
		ret = _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(unit, 0/*flags*/, &push_data, &is_allocated, &push_profile);
		BCMDNX_IF_ERR_EXIT(ret);
		mep_db_entry->push_profile = (uint8)push_profile;
		if (is_allocated) {
			soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}

		if((endpoint_info->flags2 &  BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY)&&(endpoint_info->intf_id == BCM_GPORT_INVALID)
		  &&((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS)||(endpoint_info->type == bcmOAMEndpointTypeBHHPwe) || 
		  (endpoint_info->type == bcmOAMEndpointTypeBhhSection) || (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) || (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork))){
			mep_db_entry->egress_if  = 0; 
		}
		else if(!mep_db_entry->is_lsp){
			mep_db_entry->egress_if  = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
		}
		if(mep_db_entry->is_lsp){
            /* pseudo PWE, it's real LSP type*/
            /* Put MEP's outlif to ITMH's oulitf, then lookup classifier to get correct count*/
            ret = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->intf_id,
                                                _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
                                                _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(ret);
            mep_db_entry->egress_if  = gport_hw_resources.global_out_lif;
        }
		if (mep_db_entry->egress_if > _BCM_OAM_MAX_L3_INTF) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error:interface id is out of range.\n")));
		}
	}
	if (SOC_IS_ARADPLUS(unit)) {
        if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            mep_db_entry->counter_pointer = BCM_FIELD_STAT_ID_COUNTER_GET(endpoint_info->lm_counter_base_id);
        }

		if (SOC_IS_ARADPLUS(unit) && !_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type)) {
			if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
				mep_db_entry->port_status_tlv_en = 1;
				mep_db_entry->port_status_tlv_val = endpoint_info->port_state - 1; /* see documentation on definition of this field.*/
			}
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) {
				mep_db_entry->interface_status_tlv_control = endpoint_info->interface_state;
			}
		}
	}

    if (using_fake_destination) {
        /* Handle the CCM period: in case of update and CCM_period ==0 use an illigal destination to allow recieving CCMs without transmitting. */
        mep_db_entry->is_upmep = 0;
        mep_db_entry->system_port = _BCM_OAM_ILLEGAL_DESTINATION; /* Illegal destination*/
        if (SOC_IS_ARAD_B0_AND_ABOVE(unit) &&  PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
            ret = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, 0, &mep_db_entry->system_port, &ignored, &local_port);
            BCMDNX_IF_ERR_EXIT(ret);
            mep_db_entry->local_port = local_port;
            mep_db_entry->do_not_set_interval = 1;

        }
    } else if (!_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type)) {
        if(SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info) || do_not_set_mep_db_ccm_interval){
           mep_db_entry->do_not_set_interval = 1; /*use 1731db to support 1711LM packet, so don't send ccm */
        }
        if (!do_not_set_mep_db_ccm_interval) {
            /* Handle CCM period in the normal fasion.*/
            ccm_period_ms = endpoint_info->ccm_period;
            SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ccm_period_ms, 0, mep_db_entry->ccm_interval);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Translation from HW information (mep_db_info) to information
 * returned to the user (endpoint_info).
 *
 *
 * @param unit
 * @param endpoint_info
 * @param mep_db_entry
 * @param original_local_port  : In the special case of
 *                             disabling TX with period >0 the
 *                             system port will be "fake" for
 *                             Down MEP. Original system port
 *                             given here.
 *
 * @return int
 */
int _bcm_oam_mep_db_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry, int original_tx_gport)
{
	uint32 ret, soc_sand_rv;
	SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    uint32 ccm_period_ms, ccm_period_micro_s;
	SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  mep_prof;
    int dont_care;
    int ccm_cnt = 0, rv = 0;


    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(ccm_period_micro_s);

	switch (mep_db_entry->mep_type) {
	case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
		endpoint_info->type = bcmOAMEndpointTypeEthernet;
		break;
	case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
        if (endpoint_info->type != bcmOAMEndpointTypeBHHPweGAL) {
            endpoint_info->type =  bcmOAMEndpointTypeBHHMPLS;
        }
		break;
	case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
        if (endpoint_info->type != bcmOAMEndpointTypeBhhSection) {
            endpoint_info->type =  bcmOAMEndpointTypeBHHPwe;
        }
        if (SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit) && mep_db_entry->is_lsp) {
            endpoint_info->type =  bcmOAMEndpointTypeBHHMPLS;
        }
		break;
	case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
		endpoint_info->type = bcmOAMEndpointTypeBHHPweGAL;
		break;
	case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
	case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
		endpoint_info->type =  bcmOAMEndpointTypeMPLSNetwork;
		break;
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP:
        endpoint_info->type =  bcmOAMEndpointTypeMplsLmDmLsp;
        break;
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
        endpoint_info->type =  bcmOAMEndpointTypeMplsLmDmPw;
        break;
    case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION:
        endpoint_info->type =  bcmOAMEndpointTypeMplsLmDmSection;
        break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
	}

    if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
        /* src mac: handled per device*/
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            /* common utility: always returns sucess */
            ret = _bcm_petra_mac_from_sand_mac(endpoint_info->src_mac_address, &(mep_db_entry->src_mac_address));
            BCMDNX_IF_ERR_EXIT(ret);
            if (mep_db_entry->is_upmep && SOC_IS_ARADPLUS_A0(unit)) {
                endpoint_info->src_mac_address[5] = mep_db_entry->counter_pointer >>1; /* The LSB was set on the counter pointer (without the leftmost bit) and applied via the OAMP-PE.*/
            }
        } else {
            uint8 data[5];
            ret = _bcm_dpp_am_template_oam_sa_mac_address_get(unit,mep_db_entry->src_mac_msb_profile,data);
            BCMDNX_IF_ERR_EXIT(ret);
            sal_memcpy(endpoint_info->src_mac_address, data, 5);
            endpoint_info->src_mac_address[5] = mep_db_entry->src_mac_lsb;
        }
    }

    if (SOC_IS_QAX(unit)) {
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, endpoint_info->id, &dont_care, &mep_prof);
        BCMDNX_IF_ERR_EXIT(rv);
        ccm_cnt = mep_prof.ccm_cnt;
    }

    if (mep_db_entry->ccm_interval>0 && mep_db_entry->system_port == 0xffff && SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx) {
        /* Using an illegal destination in order to disable CCM transmission*/
        endpoint_info->ccm_period = 0;
        if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            /* Down endpoint: Get the original TX_GPORT*/
            endpoint_info->tx_gport = original_tx_gport;
        }
    } else {
        if ((mep_db_entry->ccm_interval>0) && (SOC_IS_QAX(unit)) && (ccm_cnt == SOC_PPC_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT)) {
            /* Check for ccm_cnt = 0xfffff and in that case put ccm_period = 0 */
            endpoint_info->ccm_period = 0;
        } else {
            SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry->ccm_interval);
            endpoint_info->ccm_period = ccm_period_ms;
        }
        endpoint_info->flags |= mep_db_entry->is_upmep ? BCM_OAM_ENDPOINT_UP_FACING : 0;
        if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            ret = _bcm_dpp_oam_bfd_sys_port_to_tx_gport(unit, &endpoint_info->tx_gport, mep_db_entry->system_port);
            BCMDNX_IF_ERR_EXIT(ret);
        } else {
            /* For Up-MEPs the tx gport is not used unless OAMP server feature is being used. */
            if (!_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                endpoint_info->tx_gport = BCM_GPORT_INVALID;
            }
        }
    }

    endpoint_info->level = mep_db_entry->mdl;
	endpoint_info->name = mep_db_entry->mep_id;

	if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
		if (mep_db_entry->tags_num != 0) { /* not single tag */
			soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			ret = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry->outer_tpid, &tpid_vals, &endpoint_info->outer_tpid);
			BCMDNX_IF_ERR_EXIT(ret);
			if (mep_db_entry->tags_num == 2) {
				ret = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry->inner_tpid, &tpid_vals, &endpoint_info->inner_tpid);
				BCMDNX_IF_ERR_EXIT(ret);
			}
		}

		endpoint_info->inner_vlan = mep_db_entry->inner_vid_dei_pcp & 0xFFF;
		endpoint_info->inner_pkt_pri = mep_db_entry->inner_vid_dei_pcp >> 12 & 0xF;
		endpoint_info->vlan = mep_db_entry->outer_vid_dei_pcp & 0xFFF;
		endpoint_info->pkt_pri = mep_db_entry->outer_vid_dei_pcp >> 12 & 0xF;
	}
    else {
		SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;

		endpoint_info->egress_label.label = mep_db_entry->label;
		SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
		ret = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
		BCMDNX_IF_ERR_EXIT(ret);
		endpoint_info->egress_label.ttl = push_data.ttl;
		endpoint_info->egress_label.exp = push_data.exp;

		BCM_L3_ITF_SET(endpoint_info->intf_id, BCM_L3_ITF_TYPE_LIF, mep_db_entry->egress_if);
	}

	/* ITMH attributes profile get */
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
	ret = _bcm_dpp_am_template_oam_tx_priority_data_get(unit, mep_db_entry->priority, &tx_itmh_attributes);
	BCMDNX_IF_ERR_EXIT(ret);
	/* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
	endpoint_info->int_pri = ((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3);

	endpoint_info->flags |= (mep_db_entry->rdi )? BCM_OAM_ENDPOINT_RDI_TX : 0;
	if (SOC_IS_ARADPLUS(unit)) {
		if (mep_db_entry->interface_status_tlv_control) {
			endpoint_info->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
			endpoint_info->interface_state = mep_db_entry->interface_status_tlv_control;
		}
        if (mep_db_entry->port_status_tlv_en) {
			endpoint_info->flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
			endpoint_info->port_state = mep_db_entry->port_status_tlv_val +1 ; /* See struct definition for documentation.*/
		}
	}

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill classifier default entry struct */
int _bcm_oam_classifier_default_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
    BCMDNX_INIT_FUNC_DEFS;

    classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_ETH_OAM; /* Irrelevant to default. Setting to 0 */
    classifier_mep_entry->flags |= _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;
    classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->md_level = endpoint_info->level;

    classifier_mep_entry->counter = endpoint_info->lm_counter_base_id;
    classifier_mep_entry->flags |= (endpoint_info->lm_flags & BCM_OAM_LM_PCP) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP : 0;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill classifier entry struct */
int _bcm_oam_classifier_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
    uint32 ret;
    _bcm_dpp_gport_info_t gport_info;
    uint32  pp_port;
    int     core;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int global_lif = 0, local_out_lif = 0;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    ret = _bcm_petra_mac_to_sand_mac(endpoint_info->dst_mac_address, &(classifier_mep_entry->dst_mac_address));
    BCMDNX_IF_ERR_EXIT(ret);

    classifier_mep_entry->src_mac_lsb = endpoint_info->src_mac_address[5];

    _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, classifier_mep_entry->mep_type)

    if((SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit)) && (SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP == classifier_mep_entry->mep_type)
        &&(endpoint_info->mpls_out_gport == endpoint_info->intf_id)){
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;  /* pseudo PWE, real LSP type*/
        classifier_mep_entry->flags |=SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP;
    }

    if(SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)){
        classifier_mep_entry->mep_type =  (endpoint_info->mpls_network_info.function_type == 0x1) ? SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS : SOC_PPC_OAM_MEP_TYPE_Y1711_PWE;
    }
    
    classifier_mep_entry->flags |= _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)? 0 : SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->md_level = (classifier_mep_entry->mep_type== SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP &&
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0))? 7 : endpoint_info->level; /* When the level is to be ignored use level 7: this will be the output of the TCAM.*/

    classifier_mep_entry->flags |= _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
    classifier_mep_entry->flags |= _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;

    classifier_mep_entry->counter = endpoint_info->lm_counter_base_id;
    classifier_mep_entry->flags |= (endpoint_info->lm_flags & BCM_OAM_LM_PCP) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP : 0;

    if (endpoint_info->gport != BCM_GPORT_INVALID) {
        /* LIF is  MPLS Ingress only */
        if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
            (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
            (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
            (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) ||
            (endpoint_info->type == bcmOAMEndpointTypeBhhSection)) {
            if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
                flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
            } else if ((endpoint_info->mpls_out_gport == BCM_GPORT_INVALID) &&
                (endpoint_info->lm_counter_base_id > 0) &&
                ((endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
                 (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                 (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL))) {
                if (SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)) {
                    if (endpoint_info->mpls_network_info.function_type != SOC_PPC_OAM_Y1711_LM_MPLS) {
                        flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                    } else {
                        flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
                    }
                } else if ((endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                           (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL)) {
                    flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
                } else {
                    flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
                }
            } else {
                flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
            }
        } else {
            flags = _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
        /* In Jericho the OAM-LIF in the ingress is the global LIF and at the egress it is the local LIF. */
        ret = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->gport,flags, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(ret);
        local_out_lif = gport_hw_resources.local_out_lif;
        global_lif = gport_hw_resources.global_in_lif;

        if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
            classifier_mep_entry->lif = global_lif;
            ret = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->mpls_out_gport,
                                                _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                 &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(ret);

            classifier_mep_entry->passive_side_lif = gport_hw_resources.local_out_lif; /* In Arad this will equal the global out LIF anyways. */
            classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USING_ASYMETRIC_MPLS_LIF;
            if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(endpoint_info->mpls_out_gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY) {
                /* LIF is MPLS Egress only */
                classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_EGRESS_ONLY;
            }
       } else {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (local_out_lif ==-1)) {
                /* In this case there is no distinction between the global and local lif*/
                if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
                    (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
                    (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                    (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) ||
                    (endpoint_info->type == bcmOAMEndpointTypeBhhSection)) {
                    if ((endpoint_info->lm_counter_base_id > 0) &&
                        ((endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
                         (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                         (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL))) {
                        if ((SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)) &&
                            (endpoint_info->mpls_network_info.function_type != SOC_PPC_OAM_Y1711_LM_MPLS)) {
                            local_out_lif = global_lif;
                        } else if ((endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                                   (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL)) {
                            local_out_lif = global_lif;
                        }
                    }
                } else {
                    local_out_lif = global_lif;
                }
            }
            /* The ingress LIF must be the global LIF, the egress the local LIF. In Arad these should be the same.*/
            if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)) {
                classifier_mep_entry->lif = local_out_lif;
                classifier_mep_entry->passive_side_lif = global_lif;
            } else {
                classifier_mep_entry->lif = global_lif;
                classifier_mep_entry->passive_side_lif = local_out_lif;
            }
         }
    } else {
        /* For MPLS Egress only MEP, use the outlif to get hw resouces since inlif is invalid */
        if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
            if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
                (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
                (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
                (endpoint_info->type == bcmOAMEndpointTypeBhhSection) ||
                (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL)) {
                ret = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->mpls_out_gport,
                                        _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                        _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                         &gport_hw_resources);
                BCMDNX_IF_ERR_EXIT(ret);

                classifier_mep_entry->lif =gport_hw_resources.global_in_lif;
                classifier_mep_entry->passive_side_lif = gport_hw_resources.local_out_lif; /* In Arad this will equal the global  out LIF anyways.*/
                classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USING_ASYMETRIC_MPLS_LIF;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: MPLS Egress only flag can support MPLS and PWE on Jericho")));
            }
        } else {
            classifier_mep_entry->lif = classifier_mep_entry->passive_side_lif = _BCM_OAM_INVALID_LIF;
        }
    }

    if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
        (endpoint_info->type == bcmOAMEndpointTypeBhhSection) ||
        (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
        (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
        (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL)) {
        if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) {
            classifier_mep_entry->passive_side_lif = _BCM_OAM_INVALID_LIF;
            classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_INGRESS_ONLY;
        } else if (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) {
            classifier_mep_entry->lif = _BCM_OAM_INVALID_LIF;
            classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_EGRESS_ONLY;
        }
    }

    classifier_mep_entry->flags |= (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USE_DOUBLE_OUTLIF_INJ : 0;

    if (endpoint_info->type == bcmOAMEndpointTypeEthernet) {
        ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(ret);
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, &core)));

        classifier_mep_entry->port = pp_port;
        classifier_mep_entry->port_core = core;
    }

    classifier_mep_entry->flags |= (endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588 : 0;
    classifier_mep_entry->ma_index = endpoint_info->group;

    if (!_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) && BCM_GPORT_IS_TRUNK(endpoint_info->tx_gport)) {
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWNMEP_TX_GPORT_IS_LAG;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Fill classifier entry struct for RFC-6374 endpoints */
int _bcm_oam_classifier_rfc6374_mep_entry_struct_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_BCM_MEP_TYPE_TO_MEP_DB_MEP_TYPE(endpoint_info->type, classifier_mep_entry->mep_type)

    /* Classifier entry flags configuration */
    classifier_mep_entry->is_rfc_6374_entry = 1;
    classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->flags |= endpoint_info->tx_gport != BCM_GPORT_INVALID ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
    classifier_mep_entry->flags |= (endpoint_info->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588 : 0;
    classifier_mep_entry->flags |= (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM : 0;

    /**
     * In Jericho the OAM-LIF in the ingress is the global In-LIF.
     * At the egress it is the local Out-LIF.
     */
    rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->gport,
                                        _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS,
                                        &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    classifier_mep_entry->lif = gport_hw_resources.global_in_lif;

    if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
        rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->mpls_out_gport,
                                            _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                            _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USING_ASYMETRIC_MPLS_LIF;
        classifier_mep_entry->passive_side_lif = gport_hw_resources.local_out_lif;
    } else {
        classifier_mep_entry->passive_side_lif = _BCM_OAM_INVALID_LIF;
    }

    classifier_mep_entry->counter = endpoint_info->lm_counter_base_id;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Read classifier entry struct */
int _bcm_oam_classifier_mep_entry_struct_get(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
	uint32 ret;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int global_outlif = 0;

    BCMDNX_INIT_FUNC_DEFS;

	switch (classifier_mep_entry->mep_type) {
        case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
            endpoint_info->type = bcmOAMEndpointTypeEthernet;
            break;
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
            endpoint_info->type = bcmOAMEndpointTypeBHHMPLS;
            break;
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE:
            endpoint_info->type = bcmOAMEndpointTypeBHHPwe;
            break;
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
            endpoint_info->type = bcmOAMEndpointTypeBHHPweGAL;
            break;
        case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
        case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
            endpoint_info->type = bcmOAMEndpointTypeMPLSNetwork;
            break;
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION:
            endpoint_info->type = bcmOAMEndpointTypeBhhSection;
            break;
        case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
            endpoint_info->type = bcmOAMEndpointTypeMplsLmDmPw;
            break;
        case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP:
            endpoint_info->type = bcmOAMEndpointTypeMplsLmDmLsp;
            break;
        case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION:
            endpoint_info->type = bcmOAMEndpointTypeMplsLmDmSection;
            break;
	default:
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
	}

    if((SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit)) && (SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE == classifier_mep_entry->mep_type)
         &&(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP)){
        endpoint_info->type = bcmOAMEndpointTypeBHHMPLS;  /* pseudo PWE, real LSP type*/
    }

	ret = _bcm_petra_mac_from_sand_mac(endpoint_info->dst_mac_address, &(classifier_mep_entry->dst_mac_address));
	BCMDNX_IF_ERR_EXIT(ret);

    if ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)) {
          /*MPLS Egress only mep */
         if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_EGRESS_ONLY) {
            endpoint_info->flags2 = BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY;
         }
         /* MPLS Ingress only mep*/
         if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_INGRESS_ONLY) {
            endpoint_info->flags2 = BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY;
         }
    }

	endpoint_info->flags |= _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry) ? BCM_OAM_ENDPOINT_INTERMEDIATE : 0;
	endpoint_info->opcode_flags = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry) ? BCM_OAM_OPCODE_CCM_IN_HW : 0;
	endpoint_info->flags |= _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)? BCM_OAM_ENDPOINT_UP_FACING : 0;
	endpoint_info->timestamp_format = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0) ? bcmOAMTimestampFormatIEEE1588v1 : bcmOAMTimestampFormatNTP;
    endpoint_info->flags2 |= ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USE_DOUBLE_OUTLIF_INJ) != 0) ? BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ : 0;
    endpoint_info->flags2 |= ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM) != 0) ? BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM : 0;

    endpoint_info->lm_counter_base_id = classifier_mep_entry->counter;
    endpoint_info->lm_flags |= (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) ? BCM_OAM_LM_PCP : 0;

    if (classifier_mep_entry->mep_type!= SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP || classifier_mep_entry->mep_type!= SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS|| soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_over_mpls_ignore_mdl", 0)==0) {
        endpoint_info->level = classifier_mep_entry->md_level;
    } else {
        /* When the level is to be ignored return 0, which is expected as the input. */
        /* If the endpoint is accelerated the level is expected to be set in the mep_db set portion. */
        if (!_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            endpoint_info->level = 0;
        }
    }

    if (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF) {
		/* The gport will be extracted from the global lif, which in Arad will be the LIF in any case.
		   In Jericho, there is a distinction between egress and ingress. In the egress (lif in the case of Up MEP, passive-lif in the case of Down),
		   the local LIF is used, in the ingress the global LIF is used.*/
        int local_lif;
        ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS,
                                                              _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)?
															        classifier_mep_entry->passive_side_lif : classifier_mep_entry->lif, &local_lif);
        BCMDNX_IF_ERR_EXIT(ret);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, local_lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));
        endpoint_info->gport = gport_sw_resources.in_lif_sw_resources.gport_id;

        if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USING_ASYMETRIC_MPLS_LIF) {
            /* In Jericho the passive side is already the local LIF. Translate to global LIF to return to user.*/
            int global_outlif;
            ret = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, classifier_mep_entry->passive_side_lif, &global_outlif);
            BCMDNX_IF_ERR_EXIT(ret);
            if ((endpoint_info->type==bcmOAMEndpointTypeBHHMPLS) ||
                (endpoint_info->type==bcmOAMEndpointTypeBhhSection) ||
                (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmLsp) ||
                (endpoint_info->type == bcmOAMEndpointTypeMplsLmDmSection) ||
                ((endpoint_info->type==bcmOAMEndpointTypeMPLSNetwork)&&(classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1))) {
                /* MPLS: should use tunnel type gport*/
                   BCM_GPORT_TUNNEL_ID_SET(endpoint_info->mpls_out_gport,global_outlif);
            } else {
                /* Out lif: match_gport_get() functions may not always work. Set the bits manually.*/
                if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_EGRESS_ONLY) {
                    /* LIF is MPLS Egress only, set relevant bit on gport */
                    BCM_GPORT_SUB_TYPE_LIF_SET(global_outlif, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_outlif);
                } else {
                    BCM_GPORT_SUB_TYPE_LIF_SET(global_outlif, 0, global_outlif);
                }
                /* PWE: MPLS type gport*/
                BCM_GPORT_MPLS_PORT_ID_SET(endpoint_info->mpls_out_gport,global_outlif);
            }
        }
    }else if ((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
            ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
             (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
             (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
             (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
             (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
             (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION))) {
        /* MPLS Egress only OEM entries */

        ret = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, classifier_mep_entry->passive_side_lif, &global_outlif);
        BCMDNX_IF_ERR_EXIT(ret);
        if ((endpoint_info->type==bcmOAMEndpointTypeBHHMPLS) || (endpoint_info->type==bcmOAMEndpointTypeBhhSection)) {
            /* MPLS: should use tunnel type gport*/
               BCM_GPORT_TUNNEL_ID_SET(endpoint_info->mpls_out_gport,global_outlif);
        } else {
        /* PWE: MPLS type gport*/
        
               BCM_GPORT_SUB_TYPE_LIF_SET(global_outlif, _SHR_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_outlif);
               BCM_GPORT_MPLS_PORT_ID_SET(endpoint_info->mpls_out_gport,global_outlif);
            }
        endpoint_info->gport = BCM_GPORT_INVALID;		
    } 
    else {
        endpoint_info->gport = BCM_GPORT_INVALID;
    }

	endpoint_info->group = classifier_mep_entry->ma_index;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_dpp_oam_bfd_flexible_verification_expected_crc16_get
 * Purpose:
 *      Given a buffer and a mask, returns the crc16 encoding for (buffer & mask).
 *      Using CRC16-X25. The polynomial is (x^16 + x^12 + x^5 + 1).
 *      This elegant implementation is taken directly from the verilog code :(
 *      It calcuates 16 byte at a time by translating them to a bit array.
 * Parameters:
 *      buffer  - (IN)  Buffer holding the data to be encoded.
 *      mask    - (IN)  A mask for the data.
 *      crc_p   - (OUT) A pointer that will be filled with the crc16 value.
 * Returns:
 *      void
 */
void 
_bcm_dpp_oam_bfd_flexible_verification_expected_crc16_get(_BCM_OAM_OAMP_CRC_BUFFER buffer, SOC_PPC_OAM_OAMP_CRC_MASK *mask, uint16 *crc_p){
    uint8 new_crc[16], data[_BCM_OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION * SOC_SAND_NOF_BITS_IN_BYTE], crc[16];
    uint16 return_val;
    int bit, byte_iteration, cur_byte, byte, byte_val;

    /* Set initial crc to 0xffff */
    for (bit = 0 ; bit < 16 ; bit++){
        crc[bit] = 0x1;
    }

    
    for (byte_iteration = 0 ; byte_iteration < (_BCM_OAM_OAMP_CRC_BUFFER_SIZE / _BCM_OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION) ; byte_iteration++){

        /* 
         *  Translate 16 bytes of the input into a bit array.
         */
        for (cur_byte = 0 ; cur_byte < _BCM_OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION ; cur_byte++) {
            byte = cur_byte + byte_iteration * _BCM_OAM_OAMP_CRC_CALC_NOF_BYTES_PER_ITERATION;
            if (byte < SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE) {
                byte_val = (buffer[byte] & mask->msb_mask[byte]); 
            } else {
                byte_val = (SHR_BITGET(mask->lsbyte_mask, byte - SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE)) ? buffer[byte] : 0;
            }

            for (bit = 0 ; bit < SOC_SAND_NOF_BITS_IN_BYTE ; bit++) {
                /* The LSB has the exact bits in the mask. The MSBs only have valid indication per byte. */
                data[cur_byte * SOC_SAND_NOF_BITS_IN_BYTE + bit] = (byte_val >> bit) & 0x1;
            }
        }


        /* This is copied directly from verilog. */
        new_crc[0] = crc[1] ^ crc[11] ^ crc[15] ^ crc[3] ^ crc[6] ^
                   crc[9] ^ data[0] ^ data[100] ^ data[102] ^ data[104] ^
                   data[111] ^ data[112] ^ data[114] ^ data[120] ^ data[122] ^
                   data[123] ^ data[124] ^ data[125] ^ data[126] ^ data[16] ^
                   data[20] ^ data[24] ^ data[27] ^ data[28] ^ data[35] ^
                   data[36] ^ data[38] ^ data[42] ^ data[43] ^ data[44] ^
                   data[48] ^ data[49] ^ data[51] ^ data[58] ^ data[64] ^
                   data[65] ^ data[67] ^ data[68] ^ data[71] ^ data[72] ^
                   data[74] ^ data[79] ^ data[80] ^ data[81] ^ data[82] ^
                   data[83] ^ data[86] ^ data[88] ^ data[90] ^ data[91] ^
                   data[93] ^ data[96] ^ data[97] ^ data[98];
        new_crc[1] = crc[10] ^ crc[12] ^ crc[2] ^ crc[4] ^ crc[7] ^
                           data[1] ^ data[101] ^ data[103] ^ data[105] ^ data[112] ^
                           data[113] ^ data[115] ^ data[121] ^ data[123] ^ data[124] ^
                           data[125] ^ data[126] ^ data[127] ^ data[17] ^ data[21] ^
                           data[25] ^ data[28] ^ data[29] ^ data[36] ^ data[37] ^
                           data[39] ^ data[43] ^ data[44] ^ data[45] ^ data[49] ^
                           data[50] ^ data[52] ^ data[59] ^ data[65] ^ data[66] ^
                           data[68] ^ data[69] ^ data[72] ^ data[73] ^ data[75] ^
                           data[80] ^ data[81] ^ data[82] ^ data[83] ^ data[84] ^
                           data[87] ^ data[89] ^ data[91] ^ data[92] ^ data[94] ^
                           data[97] ^ data[98] ^ data[99];
        new_crc[2] = crc[0] ^ crc[11] ^ crc[13] ^ crc[3] ^ crc[5] ^
                           crc[8] ^ data[100] ^ data[102] ^ data[104] ^ data[106] ^
                           data[113] ^ data[114] ^ data[116] ^ data[122] ^ data[124] ^
                           data[125] ^ data[126] ^ data[127] ^ data[18] ^ data[2] ^
                           data[22] ^ data[26] ^ data[29] ^ data[30] ^ data[37] ^
                           data[38] ^ data[40] ^ data[44] ^ data[45] ^ data[46] ^
                           data[50] ^ data[51] ^ data[53] ^ data[60] ^ data[66] ^
                           data[67] ^ data[69] ^ data[70] ^ data[73] ^ data[74] ^
                           data[76] ^ data[81] ^ data[82] ^ data[83] ^ data[84] ^
                           data[85] ^ data[88] ^ data[90] ^ data[92] ^ data[93] ^
                           data[95] ^ data[98] ^ data[99];
        new_crc[3] = crc[0] ^ crc[1] ^ crc[12] ^ crc[14] ^ crc[4] ^
                           crc[6] ^ crc[9] ^ data[100] ^ data[101] ^ data[103] ^
                           data[105] ^ data[107] ^ data[114] ^ data[115] ^ data[117] ^
                           data[123] ^ data[125] ^ data[126] ^ data[127] ^ data[19] ^
                           data[23] ^ data[27] ^ data[3] ^ data[30] ^ data[31] ^
                           data[38] ^ data[39] ^ data[41] ^ data[45] ^ data[46] ^
                           data[47] ^ data[51] ^ data[52] ^ data[54] ^ data[61] ^
                           data[67] ^ data[68] ^ data[70] ^ data[71] ^ data[74] ^
                           data[75] ^ data[77] ^ data[82] ^ data[83] ^ data[84] ^
                           data[85] ^ data[86] ^ data[89] ^ data[91] ^ data[93] ^
                           data[94] ^ data[96] ^ data[99];
        new_crc[4] = crc[0] ^ crc[1] ^ crc[10] ^ crc[13] ^ crc[15] ^
                           crc[2] ^ crc[5] ^ crc[7] ^ data[100] ^ data[101] ^
                           data[102] ^ data[104] ^ data[106] ^ data[108] ^ data[115] ^
                           data[116] ^ data[118] ^ data[124] ^ data[126] ^ data[127] ^
                           data[20] ^ data[24] ^ data[28] ^ data[31] ^ data[32] ^
                           data[39] ^ data[4] ^ data[40] ^ data[42] ^ data[46] ^
                           data[47] ^ data[48] ^ data[52] ^ data[53] ^ data[55] ^
                           data[62] ^ data[68] ^ data[69] ^ data[71] ^ data[72] ^
                           data[75] ^ data[76] ^ data[78] ^ data[83] ^ data[84] ^
                           data[85] ^ data[86] ^ data[87] ^ data[90] ^ data[92] ^
                           data[94] ^ data[95] ^ data[97];
        new_crc[5] = crc[0] ^ crc[14] ^ crc[15] ^ crc[2] ^ crc[8] ^
                           crc[9] ^ data[100] ^ data[101] ^ data[103] ^ data[104] ^
                           data[105] ^ data[107] ^ data[109] ^ data[111] ^ data[112] ^
                           data[114] ^ data[116] ^ data[117] ^ data[119] ^ data[120] ^
                           data[122] ^ data[123] ^ data[124] ^ data[126] ^ data[127] ^
                           data[16] ^ data[20] ^ data[21] ^ data[24] ^ data[25] ^
                           data[27] ^ data[28] ^ data[29] ^ data[32] ^ data[33] ^
                           data[35] ^ data[36] ^ data[38] ^ data[40] ^ data[41] ^
                           data[42] ^ data[44] ^ data[47] ^ data[5] ^ data[51] ^
                           data[53] ^ data[54] ^ data[56] ^ data[58] ^ data[63] ^
                           data[64] ^ data[65] ^ data[67] ^ data[68] ^ data[69] ^
                           data[70] ^ data[71] ^ data[73] ^ data[74] ^ data[76] ^
                           data[77] ^ data[80] ^ data[81] ^ data[82] ^ data[83] ^
                           data[84] ^ data[85] ^ data[87] ^ data[90] ^ data[95] ^
                           data[97];
        new_crc[6] = crc[0] ^ crc[1] ^ crc[10] ^ crc[15] ^ crc[3] ^
                           crc[9] ^ data[101] ^ data[102] ^ data[104] ^ data[105] ^
                           data[106] ^ data[108] ^ data[110] ^ data[112] ^ data[113] ^
                           data[115] ^ data[117] ^ data[118] ^ data[120] ^ data[121] ^
                           data[123] ^ data[124] ^ data[125] ^ data[127] ^ data[17] ^
                           data[21] ^ data[22] ^ data[25] ^ data[26] ^ data[28] ^
                           data[29] ^ data[30] ^ data[33] ^ data[34] ^ data[36] ^
                           data[37] ^ data[39] ^ data[41] ^ data[42] ^ data[43] ^
                           data[45] ^ data[48] ^ data[52] ^ data[54] ^ data[55] ^
                           data[57] ^ data[59] ^ data[6] ^ data[64] ^ data[65] ^
                           data[66] ^ data[68] ^ data[69] ^ data[70] ^ data[71] ^
                           data[72] ^ data[74] ^ data[75] ^ data[77] ^ data[78] ^
                           data[81] ^ data[82] ^ data[83] ^ data[84] ^ data[85] ^
                           data[86] ^ data[88] ^ data[91] ^ data[96] ^ data[98];
        new_crc[7] = crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[2] ^
                           crc[4] ^ data[102] ^ data[103] ^ data[105] ^ data[106] ^
                           data[107] ^ data[109] ^ data[111] ^ data[113] ^ data[114] ^
                           data[116] ^ data[118] ^ data[119] ^ data[121] ^ data[122] ^
                           data[124] ^ data[125] ^ data[126] ^ data[18] ^ data[22] ^
                           data[23] ^ data[26] ^ data[27] ^ data[29] ^ data[30] ^
                           data[31] ^ data[34] ^ data[35] ^ data[37] ^ data[38] ^
                           data[40] ^ data[42] ^ data[43] ^ data[44] ^ data[46] ^
                           data[49] ^ data[53] ^ data[55] ^ data[56] ^ data[58] ^
                           data[60] ^ data[65] ^ data[66] ^ data[67] ^ data[69] ^
                           data[7] ^ data[70] ^ data[71] ^ data[72] ^ data[73] ^
                           data[75] ^ data[76] ^ data[78] ^ data[79] ^ data[82] ^
                           data[83] ^ data[84] ^ data[85] ^ data[86] ^ data[87] ^
                           data[89] ^ data[92] ^ data[97] ^ data[99];
        new_crc[8] = crc[1] ^ crc[11] ^ crc[12] ^ crc[2] ^ crc[3] ^
                           crc[5] ^ data[100] ^ data[103] ^ data[104] ^ data[106] ^
                           data[107] ^ data[108] ^ data[110] ^ data[112] ^ data[114] ^
                           data[115] ^ data[117] ^ data[119] ^ data[120] ^ data[122] ^
                           data[123] ^ data[125] ^ data[126] ^ data[127] ^ data[19] ^
                           data[23] ^ data[24] ^ data[27] ^ data[28] ^ data[30] ^
                           data[31] ^ data[32] ^ data[35] ^ data[36] ^ data[38] ^
                           data[39] ^ data[41] ^ data[43] ^ data[44] ^ data[45] ^
                           data[47] ^ data[50] ^ data[54] ^ data[56] ^ data[57] ^
                           data[59] ^ data[61] ^ data[66] ^ data[67] ^ data[68] ^
                           data[70] ^ data[71] ^ data[72] ^ data[73] ^ data[74] ^
                           data[76] ^ data[77] ^ data[79] ^ data[8] ^ data[80] ^
                           data[83] ^ data[84] ^ data[85] ^ data[86] ^ data[87] ^
                           data[88] ^ data[90] ^ data[93] ^ data[98];
        new_crc[9] = crc[0] ^ crc[12] ^ crc[13] ^ crc[2] ^ crc[3] ^
                           crc[4] ^ crc[6] ^ data[101] ^ data[104] ^ data[105] ^
                           data[107] ^ data[108] ^ data[109] ^ data[111] ^ data[113] ^
                           data[115] ^ data[116] ^ data[118] ^ data[120] ^ data[121] ^
                           data[123] ^ data[124] ^ data[126] ^ data[127] ^ data[20] ^
                           data[24] ^ data[25] ^ data[28] ^ data[29] ^ data[31] ^
                           data[32] ^ data[33] ^ data[36] ^ data[37] ^ data[39] ^
                           data[40] ^ data[42] ^ data[44] ^ data[45] ^ data[46] ^
                           data[48] ^ data[51] ^ data[55] ^ data[57] ^ data[58] ^
                           data[60] ^ data[62] ^ data[67] ^ data[68] ^ data[69] ^
                           data[71] ^ data[72] ^ data[73] ^ data[74] ^ data[75] ^
                           data[77] ^ data[78] ^ data[80] ^ data[81] ^ data[84] ^
                           data[85] ^ data[86] ^ data[87] ^ data[88] ^ data[89] ^
                           data[9] ^ data[91] ^ data[94] ^ data[99];
        new_crc[10] = crc[0] ^ crc[1] ^ crc[13] ^ crc[14] ^ crc[3] ^
                            crc[4] ^ crc[5] ^ crc[7] ^ data[10] ^ data[100] ^
                            data[102] ^ data[105] ^ data[106] ^ data[108] ^ data[109] ^
                            data[110] ^ data[112] ^ data[114] ^ data[116] ^ data[117] ^
                            data[119] ^ data[121] ^ data[122] ^ data[124] ^ data[125] ^
                            data[127] ^ data[21] ^ data[25] ^ data[26] ^ data[29] ^
                            data[30] ^ data[32] ^ data[33] ^ data[34] ^ data[37] ^
                            data[38] ^ data[40] ^ data[41] ^ data[43] ^ data[45] ^
                            data[46] ^ data[47] ^ data[49] ^ data[52] ^ data[56] ^
                            data[58] ^ data[59] ^ data[61] ^ data[63] ^ data[68] ^
                            data[69] ^ data[70] ^ data[72] ^ data[73] ^ data[74] ^
                            data[75] ^ data[76] ^ data[78] ^ data[79] ^ data[81] ^
                            data[82] ^ data[85] ^ data[86] ^ data[87] ^ data[88] ^
                            data[89] ^ data[90] ^ data[92] ^ data[95];
        new_crc[11] = crc[0] ^ crc[1] ^ crc[14] ^ crc[15] ^ crc[2] ^
                            crc[4] ^ crc[5] ^ crc[6] ^ crc[8] ^ data[101] ^
                            data[103] ^ data[106] ^ data[107] ^ data[109] ^ data[11] ^
                            data[110] ^ data[111] ^ data[113] ^ data[115] ^ data[117] ^
                            data[118] ^ data[120] ^ data[122] ^ data[123] ^ data[125] ^
                            data[126] ^ data[22] ^ data[26] ^ data[27] ^ data[30] ^
                            data[31] ^ data[33] ^ data[34] ^ data[35] ^ data[38] ^
                            data[39] ^ data[41] ^ data[42] ^ data[44] ^ data[46] ^
                            data[47] ^ data[48] ^ data[50] ^ data[53] ^ data[57] ^
                            data[59] ^ data[60] ^ data[62] ^ data[64] ^ data[69] ^
                            data[70] ^ data[71] ^ data[73] ^ data[74] ^ data[75] ^
                            data[76] ^ data[77] ^ data[79] ^ data[80] ^ data[82] ^
                            data[83] ^ data[86] ^ data[87] ^ data[88] ^ data[89] ^
                            data[90] ^ data[91] ^ data[93] ^ data[96];
        new_crc[12] = crc[11] ^ crc[2] ^ crc[5] ^ crc[7] ^ data[100] ^
                            data[107] ^ data[108] ^ data[110] ^ data[116] ^ data[118] ^
                            data[119] ^ data[12] ^ data[120] ^ data[121] ^ data[122] ^
                            data[125] ^ data[127] ^ data[16] ^ data[20] ^ data[23] ^
                            data[24] ^ data[31] ^ data[32] ^ data[34] ^ data[38] ^
                            data[39] ^ data[40] ^ data[44] ^ data[45] ^ data[47] ^
                            data[54] ^ data[60] ^ data[61] ^ data[63] ^ data[64] ^
                            data[67] ^ data[68] ^ data[70] ^ data[75] ^ data[76] ^
                            data[77] ^ data[78] ^ data[79] ^ data[82] ^ data[84] ^
                            data[86] ^ data[87] ^ data[89] ^ data[92] ^ data[93] ^
                            data[94] ^ data[96] ^ data[98];
        new_crc[13] = crc[0] ^ crc[12] ^ crc[3] ^ crc[6] ^ crc[8] ^
                            data[101] ^ data[108] ^ data[109] ^ data[111] ^ data[117] ^
                            data[119] ^ data[120] ^ data[121] ^ data[122] ^ data[123] ^
                            data[126] ^ data[13] ^ data[17] ^ data[21] ^ data[24] ^
                            data[25] ^ data[32] ^ data[33] ^ data[35] ^ data[39] ^
                            data[40] ^ data[41] ^ data[45] ^ data[46] ^ data[48] ^
                            data[55] ^ data[61] ^ data[62] ^ data[64] ^ data[65] ^
                            data[68] ^ data[69] ^ data[71] ^ data[76] ^ data[77] ^
                            data[78] ^ data[79] ^ data[80] ^ data[83] ^ data[85] ^
                            data[87] ^ data[88] ^ data[90] ^ data[93] ^ data[94] ^
                            data[95] ^ data[97] ^ data[99];
        new_crc[14] = crc[1] ^ crc[13] ^ crc[4] ^ crc[7] ^ crc[9] ^
                            data[100] ^ data[102] ^ data[109] ^ data[110] ^ data[112] ^
                            data[118] ^ data[120] ^ data[121] ^ data[122] ^ data[123] ^
                            data[124] ^ data[127] ^ data[14] ^ data[18] ^ data[22] ^
                            data[25] ^ data[26] ^ data[33] ^ data[34] ^ data[36] ^
                            data[40] ^ data[41] ^ data[42] ^ data[46] ^ data[47] ^
                            data[49] ^ data[56] ^ data[62] ^ data[63] ^ data[65] ^
                            data[66] ^ data[69] ^ data[70] ^ data[72] ^ data[77] ^
                            data[78] ^ data[79] ^ data[80] ^ data[81] ^ data[84] ^
                            data[86] ^ data[88] ^ data[89] ^ data[91] ^ data[94] ^
                            data[95] ^ data[96] ^ data[98];
        new_crc[15] = crc[0] ^ crc[10] ^ crc[14] ^ crc[2] ^ crc[5] ^
                            crc[8] ^ data[101] ^ data[103] ^ data[110] ^ data[111] ^
                            data[113] ^ data[119] ^ data[121] ^ data[122] ^ data[123] ^
                            data[124] ^ data[125] ^ data[15] ^ data[19] ^ data[23] ^
                            data[26] ^ data[27] ^ data[34] ^ data[35] ^ data[37] ^
                            data[41] ^ data[42] ^ data[43] ^ data[47] ^ data[48] ^
                            data[50] ^ data[57] ^ data[63] ^ data[64] ^ data[66] ^
                            data[67] ^ data[70] ^ data[71] ^ data[73] ^ data[78] ^
                            data[79] ^ data[80] ^ data[81] ^ data[82] ^ data[85] ^
                            data[87] ^ data[89] ^ data[90] ^ data[92] ^ data[95] ^
                            data[96] ^ data[97] ^ data[99];

        /* Copy the new crc into the old crc for the next iteration. */
        for (bit = 0 ; bit < 16 ; bit++) {
            crc[bit] = new_crc[bit];
        }
    }
    
    /* Finally, translate the bit array to a uint16. */
    return_val = 0;
    for (bit = 0 ; bit < 16 ; bit++){
        return_val += (new_crc[bit] & 0x1) << bit;
    }
    *crc_p = return_val;
}
