/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam.c
 * Purpose: Manages L3 interface table, forwarding table, routing table
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_BFD

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
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/bfd_uc.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/bfd.h>

#include <shared/gport.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/ARAD/arad_ports.h>

/***************************************************************/
/***************************************************************/

/*
 * Global defines
 */

/* No warmboot - reconstructed using other resources */

/***************************************************************/
/***************************************************************/

/*
 * Define validation
 */

#if BCM_BFD_ENDPOINT_DEFAULT0 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS0
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT1 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS1
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT2 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS2
#error "BFD and OAM default endpoint IDs must be compatible"
#endif
#if BCM_BFD_ENDPOINT_DEFAULT3 != BCM_OAM_ENDPOINT_DEFAULT_INGRESS3
#error "BFD and OAM default endpoint IDs must be compatible"
#endif

/***************************************************************/
/***************************************************************/

/*
 * Local defines
 */

#define _BCM_BFD_UDP_SRC_PORT_MIN (49152)
#define _BCM_BFD_UDP_SRC_PORT_MAX (65535)

#define _BCM_BFD_IS_BFD_O_MPLS_SPECIAL_TTL(type)              ((type == bcmBFDTunnelTypeMpls) && (SOC_DPP_CONFIG(unit)->pp.bfd_mpls_special_ttl_support == 1))

#define _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(type) ((type == bcmBFDTunnelTypeUdp) || (SOC_IS_JERICHO(unit) &&  type==bcmBFDTunnelTypeMpls))

#define _BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(endpoint_info)  ( ((endpoint_info).flags & BCM_BFD_ENDPOINT_IN_HW) && \
             (((endpoint_info).remote_gport == BCM_GPORT_INVALID) || ((endpoint_info).flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET)) )

#define _BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) ((SOC_DPP_CONFIG(unit))->pp.bfd_ipv4_flex_iptos != 0)

/* Macros used by diagnostics functions */
#define IPV4_PRINT_FMT "%u.%u.%u.%u"
#define IPV4_PRINT_ARG(addr) ((uint8*)&addr)[0], ((uint8*)&addr)[1],((uint8*)&addr)[2],((uint8*)&addr)[3]

/* Represents the maximum waiting time in miliseconds for a bfd endpoint to receive a message before assuming it's peer is dead */
#define _BCM_BFD_MAX_LIFETIME_IN_MS 2000000

/* Represents the maximum waiting time in micro for a bfd endpoint to receive a message before assuming it's peer is dead */
#define _BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME_USEC_MAX (1000*(_BCM_BFD_MAX_LIFETIME_IN_MS))

/* bfd echo udp dest port */
#define _BCM_BFD_ECHO_UDP_PORT 0xec9

/* bfd echo ttl */
#define _BCM_BFD_ECHO_TTL 0xfe

/* next protocol udp*/
#define _BCM_BFD_ECHO_NEXT_PROTOCOL_UDP 0x11

#define _BCM_BFD_RMEP_STATE_TO_REMOTE_STATE_DIAG_AND_FLAGS(rmep_state, remote_state, remote_diag, flags_prof) \
     do {\
            flags_prof = (rmep_state & 0x1c0) >> 6; \
            remote_diag = rmep_state & 0xf;\
            remote_state = (rmep_state >>4) & 0x3;\
        } while(0)

#define _BCM_BFD_RMEP_STATE_FROM_REMOTE_STATE_DIAG_AND_FLAGS(rmep_state, remote_state, remote_diag, flags_prof_indx)\
              rmep_state = (flags_prof_indx <<6)  | ( (3 &remote_state) <<4 ) | (remote_diag & 0xf )

#define _BCM_BFD_IS_MEP_ID_DEFAULT(_unit, _epid) \
                (SOC_IS_ARAD_B1_AND_BELOW(_unit)\
                     ? ((_epid) == -1)\
                     : (((_epid) == BCM_BFD_ENDPOINT_DEFAULT0)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT1)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT2)\
                        ||((_epid) == BCM_BFD_ENDPOINT_DEFAULT3)))


/**
 * Convert IPv4 (in uint32 format) to IPv6 (in uint8[16] 
 * format.) 
 * result should be 0::FFFF:IPv4-DIP 
 */
#define _BCM_BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(ipv4_ip, ipv6_ip) \
    do {\
          soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_ip);\
          ipv6_ip.address[0] = ipv4_ip;\
          ipv6_ip.address[1] = 0xffff;\
    } while (0)


/*OAM statistics*/
#define _BCM_BFD_CFM_ETHER_TYPE	(0x8902)

/*Endpoint update procedure should be atomic.  It goes like this: 
  1) Allocate new profiles (HW still points to old profiles)
  2) Update HW to point to new profiles
  3) Free old profiles.
However if the allocation in (1) failed, try freeing the existing profile and then allocating a new one.
This would also require temporarily stoping traffic. */
#define _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv,what_to_free, free_function, free_function_stuff, alloc_function, alloc_function_stuff) \
do {\
if (rv==BCM_E_MEMORY && existing_mep_db_entry ) {\
    free_function free_function_stuff;\
    BCMDNX_IF_ERR_EXIT(rv);\
    alloc_function alloc_function_stuff;\
    BCMDNX_IF_ERR_EXIT(rv);\
    what_to_free=0;\
     /* To avoid grabage tx during/after the deallocation, first disable the endpoint packets creation */\
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_acc_endpoint_tx_disable, (unit, endpoint_info->id, existing_mep_db_entry));\
    BCMDNX_IF_ERR_EXIT(rv);\
} else {\
    BCMDNX_IF_ERR_EXIT(rv);\
    what_to_free=1;\
}\
} while (0) 

#define _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(your_discriminator)  \
    (SOC_IS_QUX(unit) ? ((((your_discriminator) >> 2) & 0x700) | ((your_discriminator) & 0xff)) : (your_discriminator))

/* Due to H/W limitation time gap between
 * last free and new tx rate configuration with different rate
 * should be more than 1.667 seconds
 */
#define TX_RATE_MAX_TIME       0xFFFFFFFF
#define TX_RATE_SEC            3
#define TX_RATE_USEC           0x19A280 /* 1680000 micro seconds */

/***************************************************************/
/*********** BFD Flags Support *********************************/

 /** FOR PFD flags support (default):
 * There is a 1 to 1 mapping between 3 bit flag profile indexes 
 * the the 6 bit content of each profile. The mapping is as 
 * such:  (P,F,D are 0/1)
 *  PF00D0 |---> PFD
 *  PFD    |--->  PF00D0
 *  
 *  In addition the flags PFD, may not be used together, so the
 *  addtional condition is
 *  111 |---> 000000
 *  110010 |---> 0
 *  
 *  Likewise the profie 0 is reserved for unsupported flag
 *  combinations. Thus profile_index 0 returns -1.
 *  Profiles 6,7 are used for Flags combination 0
 *  
 *  All input is assumed to be uint32/8.

 * FOR PFC flags support:
 * There is a 1 to 1 mapping between 3 bit flag profile indexes 
 * to the 6 bit content of each profile. The mapping is as 
 * such:  (P,F,C are 0/1)
 *  PFC000 |---> PFC
 *  PFC    |--->  PFC000
 *  
 *  In addition the flags PFC, may not be used together, so the
 *  addtional condition is
 *  111     |---> 000000
 *  111000  |---> 0
 *  
 *  Likewise the profie 0 is reserved for unsupported flag
 *  combinations. Thus profile_index 0 returns -1.
 *  Profiles 6,7 are used for Flags combination 0
 */

int bcm_bfd_flags_profile_index_from_flags(int unit, uint8 *flags_profile, uint32 flags){

int   bfd_supported_flags_bitfield_local;

   BCMDNX_INIT_FUNC_DEFS;

   bfd_supported_flags_bitfield_local = SOC_DPP_CONFIG(unit)->pp.bfd_supported_flags_bitfield;

   switch (bfd_supported_flags_bitfield_local) {
      case 0x32: /* PFD */
         *flags_profile = (flags==0)? 7:  (((flags & 0x2) >>1) | ((flags & 0x10) >>3) | ((flags & 0x20) >>3));
         break;
      case 0x38: /* PFC */
         *flags_profile = (flags==0)? 7:  (((flags & 0x8) >>3) | ((flags & 0x10) >>3) | ((flags & 0x20) >>3));
         break;
      default: /* Unsupported flag combiation */
         *flags_profile = 7;
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported BFD Flag Bitfield\n")));
         break;
   }
exit:
    BCMDNX_FUNC_RETURN; 
}

int bcm_bfd_flags_profile_index_to_flags(int unit, int profile_index, uint32 *flags){

int   bfd_supported_flags_bitfield_local;

   BCMDNX_INIT_FUNC_DEFS;

   bfd_supported_flags_bitfield_local = (SOC_DPP_CONFIG(unit))->pp.bfd_supported_flags_bitfield;

   switch (bfd_supported_flags_bitfield_local) {
      case 0x32: /* PFD */
         *flags = (profile_index ==7)? 0 : (profile_index==0)? -1 : (((profile_index & 0x1) << 1 ) | ((profile_index & 0x2) << 3) | ((profile_index & 0x4) << 3));
         break;
      case 0x38: /* PFC */
         *flags = (profile_index ==7)? 0 : (profile_index==0)? -1 : (((profile_index & 0x1) << 3 ) | ((profile_index & 0x2) << 3) | ((profile_index & 0x4) << 3));
         break;
      default: /* Unsupported flag combiation */
         *flags = 0;
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported BFD Flag Bitfield\n")));
         break;
   }
exit:
    BCMDNX_FUNC_RETURN; 
}

int bcm_bfd_flag_combination_is_not_supported(int unit, uint32 flags, int *flag_combination_not_supported){

int   bfd_supported_flags_bitfield_local;

   BCMDNX_INIT_FUNC_DEFS;

   bfd_supported_flags_bitfield_local = SOC_DPP_CONFIG(unit)->pp.bfd_supported_flags_bitfield;

   switch (bfd_supported_flags_bitfield_local) {
      case 0x32: /* PFD */
         *flag_combination_not_supported = (flags & 0xd  ||  (flags == 0x32 ));
         break;
      case 0x38: /* PFC */
         *flag_combination_not_supported = (flags & 0x7  ||  (flags == 0x38 ));
         break;
   default: /* Unsupported flag combiation */
         *flag_combination_not_supported = 1;
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported BFD Flag Bitfield\n")));
         break;
   }
exit:
    BCMDNX_FUNC_RETURN; 
}


/* in OAM statistics per mep functionality the counter Id is inserted into the LEM*/
int _bcm_bfd_stat_lem_entry_add(uint32 unit, bcm_bfd_endpoint_info_t *endpoint_info) {
	int rv;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_LEM_ACCESS_PAYLOAD payload;
	BCMDNX_INIT_FUNC_DEFS;
	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
	payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
	payload.dest = (uint16)endpoint_info->id;
	qual_vals[0].val.arr[0] = endpoint_info->local_discr;
    qual_vals[0].val.arr[1] = 0;
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        qual_vals[0].type = SOC_PPC_FP_QUAL_MY_DISCR_IPV4;
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmBFDTunnelTypeMpls:
        qual_vals[0].type = SOC_PPC_FP_QUAL_MY_DISCR_MPLS;
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmBFDTunnelTypePweControlWord:
        qual_vals[0].type = SOC_PPC_FP_QUAL_MY_DISCR_PWE;
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS, qual_vals, 0, &payload, &success);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type with statistics feature")));
    }
exit:
    BCMDNX_FUNC_RETURN; 
}


/* add entry to the LEM for BFD echo*/
int _bcm_bfd_echo_lem_entry_add(uint32 unit, bcm_bfd_endpoint_info_t *endpoint_info) {
	int rv;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_LEM_ACCESS_PAYLOAD payload;
	BCMDNX_INIT_FUNC_DEFS;
	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);

	
    payload.flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
	payload.dest = (0xe3 | 0x600); /* 0x6e3;*/

    
    payload.asd = (endpoint_info->local_discr&0xffff);

    qual_vals[0].val.arr[0] = _BCM_BFD_ECHO_UDP_PORT;
    qual_vals[0].val.arr[1] = 0;
    qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT;

    qual_vals[1].val.arr[0] = endpoint_info->dst_ip_addr;
    qual_vals[1].val.arr[1] = 0;
    qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;

    qual_vals[2].val.arr[0] = _BCM_BFD_ECHO_TTL;
    qual_vals[2].val.arr[1] = 0;
    qual_vals[2].type = SOC_PPC_FP_QUAL_HDR_IPV4_TTL;

    qual_vals[3].val.arr[0] = _BCM_BFD_ECHO_NEXT_PROTOCOL_UDP;
    qual_vals[3].val.arr[1] = 0;
    qual_vals[3].type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL; 

             

    rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM, qual_vals, 0, &payload, &success);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN; 
}

/* add entry to the LEM for BFD echo*/
int _bcm_bfd_echo_lem_entry_delete(uint32 unit, uint32 dst_ip) {
	int rv;
	SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	
	BCMDNX_INIT_FUNC_DEFS;

	SOC_PPC_FP_QUAL_VAL_clear(qual_vals);
  

    qual_vals[0].val.arr[0] = _BCM_BFD_ECHO_UDP_PORT;
    qual_vals[0].val.arr[1] = 0;
    qual_vals[0].type = SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT;

    qual_vals[1].val.arr[0] = dst_ip;
    qual_vals[1].val.arr[1] = 0;
    qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;

    qual_vals[2].val.arr[0] = _BCM_BFD_ECHO_TTL;
    qual_vals[2].val.arr[1] = 0;
    qual_vals[2].type = SOC_PPC_FP_QUAL_HDR_IPV4_TTL;

    qual_vals[3].val.arr[0] = _BCM_BFD_ECHO_NEXT_PROTOCOL_UDP;
    qual_vals[3].val.arr[1] = 0;
    qual_vals[3].type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL; 

    

    
    rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM, qual_vals, &success);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN; 
}



/*
 * Function manages BFD PDU static registers 
 * In Arad+ the register "BFD PDU Static Register" is used for all endpoint types. 
 * In Arad that register is used for all types except MPLS-TP CC endpoint types, which uses the 
 * "BFD CC Packet Static Register". 
 * All paramters should be self explanatory except is_updating. This should be 1 when endpoint_create() is called with 
 * endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE, 
 * 0 otherwise. 
*/
int _bcm_bfd_manage_static_registers(int unit, bcm_bfd_endpoint_info_t *endpoint_info, uint32 ref_counter, uint8 is_mplsCC, uint8 is_updating) {
    SOC_PPC_BFD_PDU_STATIC_REGISTER bfd_pdu;
    uint32 soc_sand_rv;
    SOC_PPC_BFD_PDU_STATIC_REGISTER *bfd_pdu_to_update;
    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);

    bfd_pdu_to_update = (SOC_IS_ARAD_B1_AND_BELOW(unit) && is_mplsCC) ? &(bfd_cc_packet.bfd_static_reg_fields) : &bfd_pdu;

    if (ref_counter == 0 || ((ref_counter == 1) && is_updating)) {
		/* Set the static registers in one of 2 cases:
		   1. This is the first endpoint (ref count is at 0)
		   2, Only one endpoint is using the static registers (ref count==1) and that endpoint is attempting to update these fields.*/
        if (is_updating && ref_counter==0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG("Something went wrong - internal counters mismanaged.")));
        }

        bfd_pdu_to_update->bfd_req_min_echo_rx_interval = endpoint_info->local_min_echo;
        bfd_pdu_to_update->bfd_vers = SOC_PPC_BFD_PDU_VERSION;
        bfd_pdu_to_update->bfd_length = SOC_PPC_BFD_PDU_LENGTH;
		if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            bfd_pdu_to_update->bfd_sta = endpoint_info->local_state;
            bfd_pdu_to_update->bfd_flags =  endpoint_info->local_flags ;
            bfd_pdu_to_update->bfd_diag = endpoint_info->local_diag;
            if (is_mplsCC) {
                bfd_cc_packet.bfd_your_discr = endpoint_info->remote_discr;
                bfd_cc_packet.bfd_my_discr = endpoint_info->local_discr;
                soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_set(unit, &bfd_cc_packet);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        if (SOC_IS_ARADPLUS(unit) || !is_mplsCC) {  
            soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_set(unit, &bfd_pdu);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
		if (is_mplsCC && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
			soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_get(unit, &bfd_cc_packet);
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
			if (bfd_cc_packet.bfd_my_discr != endpoint_info->local_discr) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
									(_BSL_BCM_MSG("Error: Supporting single local_discr value for device. Existing value: %d"),
									 bfd_cc_packet.bfd_my_discr));
			}
			if (bfd_cc_packet.bfd_your_discr != endpoint_info->remote_discr) {
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
									(_BSL_BCM_MSG("Error: Supporting single remote_discr value for device. Existing value: %d"),
									 bfd_cc_packet.bfd_your_discr));
			}
		} else {
			soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 1) {
             if((endpoint_info->local_min_echo != 0) && (bfd_pdu_to_update->bfd_req_min_echo_rx_interval != endpoint_info->local_min_echo)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                        (_BSL_BCM_MSG("Error: Only 2 local_min_echo values are allowed 0 and %d for device: Current passed value:%d"),
                         bfd_pdu_to_update->bfd_req_min_echo_rx_interval, endpoint_info->local_min_echo));
            }
        } else {
            if (bfd_pdu_to_update->bfd_req_min_echo_rx_interval != endpoint_info->local_min_echo) {
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Supporting single local_min_echo value for device. Existing value: %d"),
                     bfd_pdu_to_update->bfd_req_min_echo_rx_interval));
            }
        }
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            if (bfd_pdu_to_update->bfd_diag != endpoint_info->local_diag) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single local_diag value for device. Existing value: %d"),
                                  bfd_pdu_to_update->bfd_diag));
            }
            if (bfd_pdu_to_update->bfd_sta != endpoint_info->local_state) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single local_state value for device. Existing value: %d"), bfd_pdu_to_update->bfd_sta));
            }
            if (bfd_pdu_to_update->bfd_flags != (endpoint_info->local_flags)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: Supporting single C,A,D,M value for device. Existing value: C=%d, A=%d, D=%d, M=%d"),
                                  (bfd_pdu_to_update->bfd_flags & 0x1),(bfd_pdu_to_update->bfd_flags & 0x2),(bfd_pdu_to_update->bfd_flags & 0x4),(bfd_pdu_to_update->bfd_flags & 0x8)));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * OAM Module Helper functions
 */

/* Fill RMEP-entry struct */
int _bcm_bfd_rmep_db_entry_struct_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry) {

    SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
    int is_allocated;
    int profile;
    uint32 rmep_scan_time;
    uint8  flags_profile;
    uint32 soc_sand_rv;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    rmep_db_entry->loc_clear_threshold = 0;
	rmep_db_entry->loc_clear_threshold = endpoint_info->loc_clear_threshold;

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME) {
        rmep_db_entry->ccm_period = endpoint_info->bfd_detection_time;
    }
    else {
        rmep_db_entry->ccm_period = endpoint_info->local_min_rx * endpoint_info->remote_detect_mult;
    }

    rmep_db_entry->last_ccm_lifetime_valid_on_create = endpoint_info->faults & BCM_BFD_ENDPOINT_REMOTE_LOC;

    if(!SOC_IS_QAX(unit))
    {
        /*lifetime_units=2 not supported in ARAD and Jericho */
        rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
        if (SOC_SAND_DIV_ROUND_DOWN(rmep_db_entry->ccm_period/100, rmep_scan_time) > 0x3FF){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Maximal period should be less than %d micro seconds ."),((0x400 * rmep_scan_time)* 100) -1));
        }
    }

    if (SOC_IS_ARADPLUS(unit)) {
       SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
       punt_profile_data.punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
       punt_profile_data.punt_rate = (endpoint_info->sampling_ratio > 0) ? endpoint_info->sampling_ratio - 1 : 0;
       punt_profile_data.rx_state_update_enable =   (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                    (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE) ? 1 :
       	                                         (endpoint_info->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE) ? 3 : 2;
       punt_profile_data.scan_state_update_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) ? 0 :
                                                    (endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) ? 1 :
       	                                         (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE ) ? 3 : 2;
       punt_profile_data.mep_rdi_update_loc_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_ON_LOC);
       punt_profile_data.mep_rdi_update_loc_clear_enable = (endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR );
       punt_profile_data.mep_rdi_update_rx_enable = 0;
       rv = _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(unit, 0 /*flags*/, &punt_profile_data, &is_allocated, &profile);
       BCMDNX_IF_ERR_EXIT(rv);
       if (is_allocated) {
           soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_punt_event_hendling_profile_set,
		                                           (unit, profile, &punt_profile_data));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }
       rmep_db_entry->punt_profile = profile;

      if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0))
      {
          flags_profile = 0;
      } else {
          soc_sand_rv = bcm_bfd_flags_profile_index_from_flags(unit, &flags_profile, (uint32)endpoint_info->remote_flags);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      }

      _BCM_BFD_RMEP_STATE_FROM_REMOTE_STATE_DIAG_AND_FLAGS(rmep_db_entry->rmep_state, endpoint_info->remote_state, endpoint_info->remote_diag, flags_profile);

    }
    else {
        rmep_db_entry->is_state_auto_handle = ((endpoint_info->flags & BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE) != 0);
        rmep_db_entry->is_event_mask = ((endpoint_info->flags & BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE) != 0);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Read RMEP-entry struct */
int _bcm_bfd_rmep_db_entry_struct_get(int unit, SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry, bcm_bfd_endpoint_info_t *endpoint_info,
                                      SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
   SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA punt_profile_data;
   uint32 soc_sand_rv;
   uint32 flags_profile;

   BCMDNX_INIT_FUNC_DEFS;

   BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

   endpoint_info->loc_clear_threshold = rmep_db_entry->loc_clear_threshold;
   endpoint_info->remote_detect_mult = 0;
   endpoint_info->faults |= rmep_db_entry->loc ? BCM_BFD_ENDPOINT_REMOTE_LOC : 0;
   if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_TIMEOUT_SET_EXPLICITLY) {
      endpoint_info->bfd_detection_time = rmep_db_entry->ccm_period;
      endpoint_info->flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
   } else if (endpoint_info->local_min_rx != 0) {
      endpoint_info->remote_detect_mult = SOC_SAND_DIV_ROUND(rmep_db_entry->ccm_period, endpoint_info->local_min_rx);
   }

   if (SOC_IS_ARADPLUS(unit)) {
      SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(&punt_profile_data);
      soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_punt_event_hendling_profile_get,
                                        (unit, rmep_db_entry->punt_profile,&punt_profile_data));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      endpoint_info->sampling_ratio = punt_profile_data.punt_enable ? punt_profile_data.punt_rate + 1 : 0;
      switch (punt_profile_data.rx_state_update_enable) {
      case 0:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
         break;
      case 1:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE;
         break;
      case 3:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE;
         break;
      default:
         break;
      }
      switch (punt_profile_data.scan_state_update_enable) {
      case 0:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE;
         break;
      case 1:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE;
         break;
      case 3:
         endpoint_info->flags |= BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE;
         break;
      default:
         break;
      }
      endpoint_info->flags |= punt_profile_data.mep_rdi_update_loc_enable ? BCM_BFD_ENDPOINT_RDI_ON_LOC : 0;
      endpoint_info->flags |= punt_profile_data.mep_rdi_update_loc_clear_enable ? BCM_BFD_ENDPOINT_RDI_CLEAR_ON_LOC_CLEAR : 0;
      /*endpoint_info->flags |= punt_profile_data.mep_rdi_update_rx_enable ? 0 : 0; No such option for BFD */

      _BCM_BFD_RMEP_STATE_TO_REMOTE_STATE_DIAG_AND_FLAGS(rmep_db_entry->rmep_state, endpoint_info->remote_state, endpoint_info->remote_diag, flags_profile);

      if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
         soc_sand_rv = soc_ppd_oam_bfd_flags_profile_get(unit, flags_profile, &endpoint_info->remote_flags);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      } else {
         soc_sand_rv = bcm_bfd_flags_profile_index_to_flags(unit, flags_profile, &(endpoint_info->remote_flags));
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }

   } else {
      endpoint_info->flags |= rmep_db_entry->is_state_auto_handle ? BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE : 0;
      endpoint_info->flags |= rmep_db_entry->is_event_mask ? BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE : 0;
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}


/**
 * Translate all direct fields from bcm_bfd_endpoint_info_t to 
 * SOC_PPC_OAM_OAMP_MEP_DB_ENTRY . 
 * In otherwords fields not requiring pointers. 
 *  
 *  
 * @author sinai (17/09/2015)
 * 
 * @param unit 
 * @param endpoint_info 
 * @param mep_db_entry 
 */
int _bcm_bfd_enpoint_info_to_mep_db_entry(int unit,const bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry ) {
    int single_hop_advance_a_plus=0;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && SOC_IS_ARADPLUS_A0(unit) 
        && endpoint_info->type == bcmBFDTunnelTypeUdp && !(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) ) {
        /* In arad Plus ONE hop MEP DB entriers don't work. under the above conditions use a multi hop entry instead*/
        single_hop_advance_a_plus = 1;
    }
    
    mep_db_entry->egress_if = BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
    mep_db_entry->pbit = ((endpoint_info->flags & BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE) != 0);
    mep_db_entry->fbit = 0; 

    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeMpls:
        mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS;
        if(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP){
            mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
            mep_db_entry->is_lsp = 1;
        }
        break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS;
        mep_db_entry->is_vccv = 1;
        break;
    case bcmBFDTunnelTypeMplsTpCc:
    case bcmBFDTunnelTypePweGal:
        /* PWE -ACH should always be on in these cases.*/
        if (SOC_IS_ARADPLUS(unit)) {
            mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE; /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB */
            mep_db_entry->bfd_pwe_ach = SOC_PPC_BFD_ACH_TYPE_GACH_CC;
        }
        else {
            mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP;
        }
        break;
    case bcmBFDTunnelTypeUdp:
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP || 
            single_hop_advance_a_plus || 
            _BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) ||
            _BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info)) {
            mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
        } else {
            mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP;
        }
        break; 
    case bcmBFDTunnelTypePweRouterAlert:
    case bcmBFDTunnelTypePweTtl:
        mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE;
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH ) {
            mep_db_entry->bfd_pwe_ach = SOC_PPC_BFD_ACH_TYPE_PWE_CW;
        }
        break; 
    case bcmBFDTunnelTypePweControlWord:
        mep_db_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE;
        mep_db_entry->bfd_pwe_ach = SOC_PPC_BFD_ACH_TYPE_PWE_CW;
        break; 
    default:
        /* Type has already been verified. Should never reach this point.*/
       BCM_EXIT;
    }
    if ((endpoint_info->type == bcmBFDTunnelTypePweGal || endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) && SOC_IS_JERICHO(unit)) {
        mep_db_entry->bfd_pwe_gal = 1;
    }

    if (endpoint_info->type == bcmBFDTunnelTypePweRouterAlert) {
        mep_db_entry->bfd_pwe_router_alert = 1;
    }

    mep_db_entry->remote_discr = endpoint_info->remote_discr;

    if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) { /* ONE HOP IPV4 */
        mep_db_entry->tunnel_is_mpls = (endpoint_info->ip_ttl == 1);
        mep_db_entry->ip_subnet_len =  endpoint_info->ip_subnet_length == 32 ? 31 : endpoint_info->ip_subnet_length; /* value 31 in the HW indicates full check*/
        mep_db_entry->micro_bfd = (endpoint_info->flags & BCM_BFD_ENDPOINT_MICRO_BFD) != 0;
    } else if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) && 
              _BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit)) {
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_MICRO_BFD) != 0) {
            mep_db_entry->micro_bfd = 1;
            mep_db_entry->flex_tos_s_hop_m_bfd_flag = SOC_PPC_OAM_FLEX_TOS_MICRO_BFD;
        } else if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)){
            mep_db_entry->flex_tos_s_hop_m_bfd_flag = SOC_PPC_OAM_FLEX_TOS_S_HOP;
        }
    }

    mep_db_entry->s_bfd_flag = _BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info)?SOC_PPC_OAM_S_BFD_INITIATOR:0;

    /* BFD over multihop and Single hop BFD over bundle interface*/
    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) &&
        ((endpoint_info->local_discr & (1 << 31)) == 0)) {
         mep_db_entry->disc_type_update = 1;
    }

    /* Will be ignored when not relevant*/
    mep_db_entry->dst_ip_add = endpoint_info->dst_ip_addr * ( SOC_IS_JERICHO(unit) || mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP);

    mep_db_entry->local_detect_mult = endpoint_info->local_detect_mult;

    if ((endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB  */
        if (((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) ||  (endpoint_info->type == bcmBFDTunnelTypePweGal)) && SOC_IS_ARADPLUS_A0(unit)) {
            mep_db_entry->label =  SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
        }
        else {
            mep_db_entry->label =  endpoint_info->egress_label.label;
        }
    }

    if (SOC_IS_ARADPLUS(unit)) {
        mep_db_entry->sta = endpoint_info->local_state;
        mep_db_entry->diag_profile = endpoint_info->local_diag;
        if (endpoint_info->flags & BCM_BFD_ECHO) {
            mep_db_entry->diag_profile = 0xf;
        }
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)==0 ) {
            rv = bcm_bfd_flags_profile_index_from_flags(unit, &(mep_db_entry->flags_profile), endpoint_info->local_flags);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }
exit:
    BCMDNX_FUNC_RETURN;
}
 
/**
 * Reset h/w tx rate to zero.
 * Update s/w db reference count and time period's
 *
 * @author CH SIVAKAMESWARA RAO (09/03/2017)
 *
 * @param unit
 * @param tx_rate_index  - Tx rate profile index used by EP
 *
 * @return int - BCM_E_xxxx value
 */
int _bcm_bfd_tx_rate_profile_index_free(int unit, uint8 tx_rate_index)
{
    uint32       tx_rate_ref_counter = 0;
    uint32       soc_sand_rv = SOC_SAND_OK;
    int          rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                            tx_rate_index,
                                            &tx_rate_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);
    update_bfd_tx_rates.ref_count[tx_rate_index] = tx_rate_ref_counter;

    if (update_bfd_tx_rates.ref_count[tx_rate_index] > 1) {
        /* In case of more than one EP refer's same tx rate
         * decreament reference count of tx rate by one
         */
        update_bfd_tx_rates.ref_count[tx_rate_index] -= 1;
    } else if (update_bfd_tx_rates.ref_count[tx_rate_index] == 1) {
        /* This is last EP ---
         * Set reference count of tx profile to zero.
         * Reset the tx rate in H/W to zero.
         * Record current time in sec's in update_time_secs
         * And micro sec's in update_time_usecs in s/w DB
         */
        soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit,
                                                  tx_rate_index,
                                                  0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        update_bfd_tx_rates.ref_count[tx_rate_index] = 0;
        update_bfd_tx_rates.update_time_secs[tx_rate_index] = sal_time();
        update_bfd_tx_rates.update_time_usecs[tx_rate_index] = sal_time_usecs();
    }
    rv = BFD_ACCESS.tx_rate_ref_counter.set(unit,
                                            tx_rate_index,
                                            update_bfd_tx_rates.ref_count[tx_rate_index]);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Update s/w db reference count and time period's
 *
 * @author CH SIVAKAMESWARA RAO (09/03/2017)
 *
 * @param unit
 * @param tx_rate- Tx rate profile used by EP
 *
 * @return int - BCM_E_xxxx value
 */
int _bcm_bfd_tx_rate_profile_free(int unit, uint32 tx_rate)
{
    uint8        tx_rate_index = 0;
    uint32       temp_tx_rate_value = 0;
    int          rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* Iterate over all tx profiles */
    for (tx_rate_index = 0; tx_rate_index < 8; ++tx_rate_index) {
        rv = BFD_ACCESS.tx_rate_value.get(unit,
                                          tx_rate_index,
                                          &temp_tx_rate_value);
        BCMDNX_IF_ERR_EXIT(rv);

        if (tx_rate == temp_tx_rate_value) {
            /* Found a match free the index Ep is update to new rate */
            rv = _bcm_bfd_tx_rate_profile_index_free(unit, tx_rate_index);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Function manages the bfd_scan_profile for lm/dm/bfd periods in the SW and HW.
 * 
 * @author Ivan (23/10/2019)
 * 
 * @param unit 
 * @param period - lm or dm rates in miliseconds
 * @param eth_1731_profile - mep profile information
 * @param old_eth_1731_profile - may be NULL if it is not update
 * @param is_dm - 1 if it is DM entry, 0
 *                if it is LM entry
 * 
 * @return int 
 */
int _bcm_bfd_set_and_allocate_bfd_rates(int unit, uint32 tx_rate, uint32 existing_tx_rate, uint8 *tx_rate_index, uint8 is_update) {

    int     temp_rv = BCM_E_MEMORY, rv = BCM_E_NONE;
    uint8   rate_index = 0;
    uint32  temp_tx_rate_value = 0, tx_rate_ref_counter = 0, soc_sand_rv = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (is_update == 0 || existing_tx_rate != tx_rate) {

        if ((is_update != 0) &&
            (existing_tx_rate != tx_rate) &&
            (update_bfd_tx_rates.ref_count[*tx_rate_index] == 1) &&
            (tx_rate > existing_tx_rate)) {
            /* In case of tx rate update for existing EP
             * 1. Reference count of particular tx rate is one.
             * 2. Current value is greater than existing value.
             * Update old profile to new rate.
             */

            /* Iterate over all tx profiles */
            for (rate_index = 0; rate_index < 8; ++rate_index) {
                rv = BFD_ACCESS.tx_rate_value.get(unit,
                                                  rate_index,
                                                  &temp_tx_rate_value);
                BCMDNX_IF_ERR_EXIT(rv);

                if (tx_rate == temp_tx_rate_value) {
                    rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                            rate_index,
                                                            &tx_rate_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (tx_rate_ref_counter == 0) {
                        /*
                         * This is First EP ---
                         * Set the tx rate in H/W.
                         */
                        soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit,
                                                                  rate_index,
                                                                  tx_rate);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    tx_rate_ref_counter += 1;
                    update_bfd_tx_rates.ref_count[rate_index] = tx_rate_ref_counter;
                    rv = BFD_ACCESS.tx_rate_ref_counter.set(unit,
                                                            rate_index,
                                                            tx_rate_ref_counter);

                    rv = _bcm_bfd_tx_rate_profile_index_free(unit,
                                                             *tx_rate_index);
                    BCMDNX_IF_ERR_EXIT(rv);
                    *tx_rate_index = rate_index;

                    temp_rv = BCM_E_NONE;
                    break;
                }
            }
            if (temp_rv != BCM_E_NONE) {
                soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit,
                                                          *tx_rate_index,
                                                          tx_rate);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                update_bfd_tx_rates.tx_rate[*tx_rate_index] = tx_rate;
                rv = BFD_ACCESS.tx_rate_value.set(unit,
                                                  *tx_rate_index,
                                                  tx_rate);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            rv = _bcm_bfd_tx_rate_profile_alloc(unit, tx_rate, tx_rate_index);
            BCMDNX_IF_ERR_EXIT(rv);

            if ((is_update != 0) &&
                (existing_tx_rate != tx_rate)) {
                /* Update tx rate and free existing rate */
                rv = _bcm_bfd_tx_rate_profile_free(unit, existing_tx_rate);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Set MEP DB CCM_INTERVAL with tx rate profile index
 * Update h/w profile index new tx rate.
 * Update s/w db reference count and tx rate value
 *
 * @author CH SIVAKAMESWARA RAO (09/03/2017)
 *
 * @param unit
 * @param tx_rate  - BFD Tx rate in millisecond unit
 *
 * @param mep_db_entry struct
 * @param ccm_interval - Update with tx rate profile index
 *
 * @return int - Three possible return values
 *             - BCM_E_MEMORY   No free tx profile, all are in use and
 *                              Request tx rate does not match with existing value
 *
 *             - BCM_E_TIMEOUT  Free profile exits, due to H/W limitation time gap
 *                              between last free and new tx rate configuration
 *                              with different rate should be more than 1.667 seconds
 *
 *             - BCM_E_NONE     Tx rate is allocated successfully
 */
int _bcm_bfd_tx_rate_profile_alloc(int unit, uint32 tx_rate,
                                   uint8 *mep_db_ccm_interval)
{
    int is_allocated = 0;
    int temp_index = 0;
    uint32 soc_sand_rv = SOC_SAND_OK;
    sal_time_t   local_sec = sal_time();
    sal_usecs_t  local_usec = sal_time_usecs();
    uint8        tx_rate_index = 0;
    int          rv = BCM_E_NONE, temp_rv = BCM_E_MEMORY;
    uint32       temp_tx_rate_value = 0, tx_rate_ref_counter = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Iterate over all tx profiles */
    for (tx_rate_index = 0; tx_rate_index < 8; ++tx_rate_index) {
        rv = BFD_ACCESS.tx_rate_value.get(unit,
                                          tx_rate_index,
                                          &temp_tx_rate_value);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                tx_rate_index,
                                                &tx_rate_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((tx_rate == temp_tx_rate_value) && (tx_rate_ref_counter > 0)) {
            /* Found a match and it is been in use */
            is_allocated = 0;
            temp_index = tx_rate_index;
            temp_rv = BCM_E_NONE;
            break;
        } else if (tx_rate == temp_tx_rate_value) {
            /* Found a match but not in use.
             * Perviously some EP configured same tx rate
             * but now that EP deleted.
             * By using same index to configure tx rate no need wait for 1.667 sec
             */
            is_allocated = 1;
            temp_index = tx_rate_index;
            temp_rv = BCM_E_NONE;
            break;
        }
    }

    rv = temp_rv;
    if (rv == BCM_E_MEMORY) {
        /* Request tx rate does not match with existing rates */
        for (tx_rate_index = 0; tx_rate_index < 8; ++tx_rate_index) {
            rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                    tx_rate_index,
                                                    &tx_rate_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            update_bfd_tx_rates.ref_count[tx_rate_index] = tx_rate_ref_counter;

            if (!update_bfd_tx_rates.ref_count[tx_rate_index]) {
                /* Due to H/W limitation time gap between
                 * last free and new tx rate configuration with different rate
                 * should be more than 1.667 seconds
                 */

                int temp_time_out = BCM_E_NONE;
                sal_usecs_t temp_usecs = (sal_usecs_t) (update_bfd_tx_rates.update_time_usecs[tx_rate_index] +
                                                        TX_RATE_USEC);
                sal_usecs_t temp_local_usec = (sal_usecs_t) (update_bfd_tx_rates.update_time_usecs[tx_rate_index] +
                                                             ((TX_RATE_SEC * 1000 * 1000) - 1));

                sal_time_t temp_sec = (sal_time_t) (update_bfd_tx_rates.update_time_secs[tx_rate_index] +
                                                    TX_RATE_SEC);
                local_usec = sal_time_usecs();
                local_sec = sal_time();

                if (temp_sec < update_bfd_tx_rates.update_time_secs[tx_rate_index]) {
                    /* In case of pervious + 3 sec wrap - around */
                    /* Range is from 0xFFFFFFFD (TX_RATE_MAX_TIME - TX_RATE_SEC) to
                     * Maximum of 32 bit 0xFFFFFFFF(TX_RATE_MAX_TIME) value
                     */
                    if ((local_sec >= (TX_RATE_MAX_TIME - TX_RATE_SEC)) &&
                        (local_sec <= TX_RATE_MAX_TIME)) {
                        temp_time_out = BCM_E_TIMEOUT;
                    }
                }
                /*
                 * In case of second time period wrap around
                 * The fix had to be done in many place where ever
                 * sal_time used in SDK code base
                 */
                if ((temp_time_out != BCM_E_TIMEOUT) &&
                    (local_sec > temp_sec)) {
                    is_allocated = 1;
                    temp_index = tx_rate_index;
                    temp_rv = BCM_E_NONE;
                    break;
                }

                temp_time_out = BCM_E_NONE;
                if (temp_usecs < update_bfd_tx_rates.update_time_usecs[tx_rate_index]) {
                    /* In case of pervious value + 1680000 usec wrap - around */
                    /* Range is from 0xFFE65D7F (TX_RATE_MAX_TIME - TX_RATE_USEC) to
                     * Maximum of 32 bit 0xFFFFFFFF(TX_RATE_MAX_TIME) value
                     */
                    if ((local_usec >= (TX_RATE_MAX_TIME - TX_RATE_USEC)) &&
                        (local_usec <= TX_RATE_MAX_TIME)) {
                        temp_time_out = BCM_E_TIMEOUT;
                    }
                }
                if (temp_time_out != BCM_E_TIMEOUT) {
                    if (local_usec > temp_usecs) {
                        is_allocated = 1;
                        temp_index = tx_rate_index;
                        temp_rv = BCM_E_NONE;
                        break;
                    }

                    /* In case of local/latest usec time wrap-around
                     * Value is between 0xFFD23940 (TX_RATE_MAX_TIME - (1000 * 1000 * TX_RATE_SEC) + 1)
                     * to 0xFFE65D7F (TX_RATE_MAX_TIME - TX_RATE_USEC)
                     */
                    if ((temp_local_usec < update_bfd_tx_rates.update_time_usecs[tx_rate_index]) &&
                        (update_bfd_tx_rates.update_time_usecs[tx_rate_index] >=
                         (TX_RATE_MAX_TIME - (1000 * 1000 * TX_RATE_SEC) + 1)) &&
                        (update_bfd_tx_rates.update_time_usecs[tx_rate_index] <=
                         (TX_RATE_MAX_TIME - TX_RATE_USEC)) &&
                        (local_usec < temp_usecs)) {
                        is_allocated = 1;
                        temp_index = tx_rate_index;
                        temp_rv = BCM_E_NONE;
                        break;
                    }
                }
                temp_rv = BCM_E_TIMEOUT;
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(temp_rv);

    if (mep_db_ccm_interval != NULL) {
        *mep_db_ccm_interval = (uint8)temp_index;
    }

    /* Allocate new rate in index update value in s/w and h/w */
    if (is_allocated) {
        soc_sand_rv = soc_ppd_oam_bfd_tx_rate_set(unit, temp_index, tx_rate);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        update_bfd_tx_rates.ref_count[temp_index] = 1;
        update_bfd_tx_rates.tx_rate[temp_index] = tx_rate;
        rv = BFD_ACCESS.tx_rate_value.set(unit,
                                          temp_index,
                                          update_bfd_tx_rates.tx_rate[temp_index]);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* Using existing rate update reference count */
        rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                temp_index,
                                                &tx_rate_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        update_bfd_tx_rates.ref_count[temp_index] = tx_rate_ref_counter;
        update_bfd_tx_rates.ref_count[temp_index] += 1;
    }
    rv = BFD_ACCESS.tx_rate_ref_counter.set(unit,
                                            temp_index,
                                            update_bfd_tx_rates.ref_count[temp_index]);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Set the pointers and alloc manager from MEP DB entry.
 *
 * @author sinai (17/09/2015)
 * 
 * @param unit 
 * @param endpoint_info 
 * @param existing_endpoint_info should be null for new 
 *                               endpoints.
 * @param existing_mep_db_entry  - should be null for new 
 *                               endpoints.
 * @param mep_db_entry 
 * @param pointers_to_free - Should be NULL when creating a new 
 *                         endpoint
 * 
 * @return int 
 */
int _bcm_bfd_mep_db_entry_struct_set_pointers(int unit, const bcm_bfd_endpoint_info_t *endpoint_info,
                                              const bcm_bfd_endpoint_info_t *existing_endpoint_info,
                                              const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *existing_mep_db_entry,
                                              SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
                                              _bcm_bfd_mep_db_entry_pointers_to_free * pointers_to_free)
{
    int is_allocated;
    uint32 soc_sand_rv;
    int rv;
    uint32 tx_rate, existing_tx_rate = 0;
    int temp_index;
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES old_tx_itmh_attributes;
    uint32 ipv4_multi_hop_acc_ref_counter;
    int priority;
	  int ref_counter=0;
    int single_hop_advance_a_plus=0;
    int dont_care;
    int is_last = 0;
    

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && SOC_IS_ARADPLUS_A0(unit) 
        && endpoint_info->type == bcmBFDTunnelTypeUdp && !(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) ) {
        /* In arad Plus ONE hop MEP DB entriers don't work. under the above conditions use a multi hop entry instead*/
        single_hop_advance_a_plus = 1;
    }


    tx_rate = _BCM_BFD_ENDPOINT_INFO_BFD_PERIOD_TO_MEP_DB_TX_RATE(endpoint_info->bfd_period);
    if (existing_endpoint_info) {
        existing_tx_rate = _BCM_BFD_ENDPOINT_INFO_BFD_PERIOD_TO_MEP_DB_TX_RATE(existing_endpoint_info->bfd_period);
    }
    
    rv = _bcm_bfd_set_and_allocate_bfd_rates(unit, tx_rate, existing_tx_rate, &mep_db_entry->ccm_interval, (existing_endpoint_info != NULL));
    BCMDNX_IF_ERR_EXIT(rv);


    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) || (endpoint_info->type == bcmBFDTunnelTypeMpls)
        || (endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv)) {
        if (existing_endpoint_info==NULL || existing_endpoint_info->src_ip_addr !=endpoint_info->src_ip_addr ) {
         /* Stupid coverity doesn't know that rv is checked in the macro..*/
        /* coverity[check_return : FALSE] */
            rv = _bcm_dpp_am_template_bfd_src_ip_profile_alloc(unit, 0/*flags*/, &(endpoint_info->src_ip_addr), &is_allocated, &temp_index);
             _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv,pointers_to_free->free_src_ip_profile,_bcm_dpp_am_template_bfd_src_ip_profile_free,
                                                      (unit, existing_mep_db_entry->src_ip_add_ptr, &dont_care),_bcm_dpp_am_template_bfd_src_ip_profile_alloc,
                                                       (unit, 0/*flags*/, &(endpoint_info->src_ip_addr), &is_allocated, &temp_index));
            mep_db_entry->src_ip_add_ptr = (uint8)temp_index;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_bfd_ipv4_src_addr_select_set(unit, mep_db_entry->src_ip_add_ptr, endpoint_info->src_ip_addr);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP ||  (endpoint_info->type == bcmBFDTunnelTypeMpls) 
            || (endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv)) {
            is_allocated = 0;
            if (existing_endpoint_info==NULL) {
                /* Only allocate a new TOS/TTL profile for a new endpoint*/
                SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data; 
                SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
                tos_ttl_data.tos = endpoint_info->ip_tos;
                tos_ttl_data.ttl = endpoint_info->ip_ttl;
        
                /*if we are in bfd ipv4 single hop extended mode allocate profile 15 
                if it is the first time profile 15 is used we set the tos value*/
                if (single_hop_advance_a_plus || 
                   (_BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) 
                    && (!(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)))) {
                    is_allocated = 1;
                    if ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 1) && (endpoint_info->local_min_echo == 0)) {
                        mep_db_entry->tos_ttl_profile = (uint8)_BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE;
                        rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &ref_counter);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (ref_counter == 0) {
                            rv = BFD_ACCESS.s_hop.tos_value.set(unit, endpoint_info->ip_tos);
                            BCMDNX_IF_ERR_EXIT(rv);
                        } else {
                            int tos = 0;
                            rv = BFD_ACCESS.s_hop.tos_value.get(unit, &tos);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (tos != endpoint_info->ip_tos) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: Supporting single IP TOS value per device. Existing value: %d")
                                                     , tos));
                            }
                        }
                        ref_counter++;
                        rv = BFD_ACCESS.s_hop.ref_counter.set(unit, ref_counter);
                        BCMDNX_IF_ERR_EXIT(rv);
                    } else {
                        mep_db_entry->tos_ttl_profile = (uint8)_BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE;

                        rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &ref_counter);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (ref_counter == 0) {
                            int temp = endpoint_info->ip_tos;
                            rv = BFD_ACCESS.s_hop.tos_value.set(unit, temp);
                            BCMDNX_IF_ERR_EXIT(rv);
                        } else {
                            int tos = 0;
                            rv = BFD_ACCESS.s_hop.tos_value.get(unit, &tos);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (tos != endpoint_info->ip_tos) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: Supporting single IP TOS value for bcmBFDTunnelTypeUdp for single hop per device. Existing value: %d")
                                                     , tos));
                            }
                        }
                        ref_counter++;
                        rv = BFD_ACCESS.s_hop.ref_counter.set(unit, ref_counter);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else {
                     /* Stupid coverity doesn't know that rv is checked in the macro..*/
                    /* coverity[check_return : FALSE] */
                    rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_alloc(unit, 0/*flags*/, &tos_ttl_data, &is_allocated, &temp_index);
                    _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv,pointers_to_free->free_tos_ttl_profile,_bcm_dpp_am_template_bfd_tos_ttl_profile_free,
                                                             (unit, existing_mep_db_entry->tos_ttl_profile, &dont_care),_bcm_dpp_am_template_bfd_tos_ttl_profile_alloc,
                                                              (unit, 0/*flags*/, &tos_ttl_data, &is_allocated, &temp_index));

                    mep_db_entry->tos_ttl_profile = (uint8)temp_index;
                }
                
                if (is_allocated) {
                    soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            } else { /* existing_endpoint_info != NULL meaning REPLACE case. Handle replacing of TTL/TOS profile here */
                /* Allowing updation of IP TTL */
                if ((existing_endpoint_info->ip_ttl !=endpoint_info->ip_ttl) || 
                    (existing_endpoint_info->ip_tos !=endpoint_info->ip_tos)) {
                    SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data; 
                    SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
                    tos_ttl_data.tos = endpoint_info->ip_tos;
                    tos_ttl_data.ttl = endpoint_info->ip_ttl;
                    /* Stupid coverity doesn't know that rv is checked in the macro..*/
                    /* coverity[check_return : FALSE] */
                    rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_alloc(unit, 0/*flags*/, &tos_ttl_data, &is_allocated, &temp_index);
                    _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv,pointers_to_free->free_tos_ttl_profile,_bcm_dpp_am_template_bfd_tos_ttl_profile_free,
                            (unit, existing_mep_db_entry->tos_ttl_profile, &dont_care),_bcm_dpp_am_template_bfd_tos_ttl_profile_alloc,
                            (unit, 0/*flags*/, &tos_ttl_data, &is_allocated, &temp_index));

                    mep_db_entry->tos_ttl_profile = (uint8)temp_index;
                    if (is_allocated) {
                        soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
                if (_BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) && (!(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP))) {
                    /* Allow updation of IPTOS */    
                    if (existing_endpoint_info->ip_tos !=endpoint_info->ip_tos) {
                        SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data; 
                        SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
                        tos_ttl_data.tos = endpoint_info->ip_tos;
                        tos_ttl_data.ttl = endpoint_info->ip_ttl;
                        mep_db_entry->tos_ttl_profile = (uint8)_BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE;
                        /* Do not increment ref count since 
                         * this will be the same endpoint referring to new value */
                        rv = BFD_ACCESS.s_hop.tos_value.set(unit, tos_ttl_data.tos);
                        BCMDNX_IF_ERR_EXIT(rv);
                        soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }

        } 
    }

    if (existing_endpoint_info == NULL ||
        existing_endpoint_info->local_min_rx != endpoint_info->local_min_rx) {
        /* Stupid coverity doesn't know that rv is checked in the macro..*/
        /* coverity[check_return : FALSE] */
        rv = _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(unit, 0/*flags*/, &(endpoint_info->local_min_rx), &is_allocated, &temp_index); 
        _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv, pointers_to_free->free_req_interval_pointer_local_min_rx, _bcm_dpp_am_template_bfd_req_interval_pointer_free,
                                                 (unit, mep_db_entry->min_rx_interval_ptr, &dont_care), _bcm_dpp_am_template_bfd_req_interval_pointer_alloc,
                                                 (unit, 0/*flags*/, &(endpoint_info->local_min_rx), &is_allocated, &temp_index));
        mep_db_entry->min_rx_interval_ptr = (uint8)temp_index;
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_rx_interval_ptr, endpoint_info->local_min_rx);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (existing_endpoint_info == NULL || existing_endpoint_info->local_min_tx != endpoint_info->local_min_tx) {
         /* Stupid coverity doesn't know that rv is checked in the macro..*/
        /* coverity[check_return : FALSE] */
        rv = _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(unit, 0/*flags*/, &(endpoint_info->local_min_tx), &is_allocated, &temp_index);
        _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv, pointers_to_free->free_req_interval_pointer_local_min_tx, _bcm_dpp_am_template_bfd_req_interval_pointer_free,
                                                 (unit, mep_db_entry->min_tx_interval_ptr,&dont_care), _bcm_dpp_am_template_bfd_req_interval_pointer_alloc,
                                                 (unit, 0/*flags*/, &(endpoint_info->local_min_tx), &is_allocated, &temp_index)); 

        mep_db_entry->min_tx_interval_ptr = (uint8)temp_index;
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_tx_interval_ptr, endpoint_info->local_min_tx);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (SOC_IS_ARADPLUS(unit)) {
		if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0) && 
            ( existing_endpoint_info==NULL || existing_endpoint_info->local_flags !=endpoint_info->local_flags )) {
            int flags_profile;
            /* Stupid coverity doesn't know that rv is checked in the macro..*/
            /* coverity[check_return : FALSE] */
            rv = _bcm_dpp_am_template_bfd_flags_profile_alloc(unit, 0/*flags*/, &(endpoint_info->local_flags), &is_allocated, &flags_profile);
            _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv, pointers_to_free->free_flags, _bcm_dpp_am_template_bfd_flags_profile_free,
                                                     (unit, existing_mep_db_entry->flags_profile, &dont_care), _bcm_dpp_am_template_bfd_flags_profile_alloc,
                                                     (unit, 0/*flags*/, &(endpoint_info->local_flags), &is_allocated, &flags_profile)); 

            mep_db_entry->flags_profile = (uint8)flags_profile;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, mep_db_entry->flags_profile, endpoint_info->local_flags);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } 
    }

    if ( existing_endpoint_info==NULL ) {
        /* BFD transmit diasble,we forward traffic to invalid port */
        if (endpoint_info->bfd_period == 0) {
            mep_db_entry->system_port = _BCM_OAM_ILLEGAL_DESTINATION;
        } else {
            /* Following fields will only be updated for new endpoints*/
            rv = _bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info->tx_gport, &(mep_db_entry->system_port));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
            /* When updating the endpoint the tx_gport may not be updated anyways.
               In that case it is assumed that this hasn't been freed and thus shouldn't be allocated again.*/
            int local_port, ignored;

            rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, 0, &(mep_db_entry->system_port), &ignored, &local_port);
            BCMDNX_IF_ERR_EXIT(rv);
            mep_db_entry->local_port = local_port;
        }
        
        /* ITMH attributes profile allocate */
        SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
        /* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
        tx_itmh_attributes.tc = ((uint32)(endpoint_info->int_pri & 0x1F)) >> 2;
        tx_itmh_attributes.dp = ((uint32)endpoint_info->int_pri) & 0x3;
        if (!SOC_IS_QAX(unit) && mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
            rv = BFD_ACCESS.ipv4_multi_hop_acc_ref_counter.get(unit, &ipv4_multi_hop_acc_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            if (ipv4_multi_hop_acc_ref_counter == 0) {
                soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(unit, &tx_itmh_attributes);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(unit, &tx_itmh_attributes);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (tx_itmh_attributes.tc != (((uint32)endpoint_info->int_pri & 0x1F) >> 2) || (tx_itmh_attributes.dp != (((uint32)endpoint_info->int_pri) & 0x3))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: Supporting single int_pri value for bcmBFDTunnelTypeUdp multihop. Existing value: %d"),
                                         (((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3))));
                }
            }
            ipv4_multi_hop_acc_ref_counter++;
            rv = BFD_ACCESS.ipv4_multi_hop_acc_ref_counter.set(unit, ipv4_multi_hop_acc_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = _bcm_dpp_am_template_oam_tx_priority_alloc(unit, 0/*flags*/, &tx_itmh_attributes, &is_allocated, &priority);
            BCMDNX_IF_ERR_EXIT(rv);
            mep_db_entry->priority = (uint8)priority;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        
        if ((endpoint_info->type != bcmBFDTunnelTypeUdp)) {
            SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;
            /* May not be updated*/

            SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
            /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB  */
            if (((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) ||  (endpoint_info->type == bcmBFDTunnelTypePweGal)) && SOC_IS_ARADPLUS_A0(unit)) {
                mep_db_entry->label =  SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
            } else {
                mep_db_entry->label =  endpoint_info->egress_label.label;
            }
            push_data.ttl = endpoint_info->egress_label.ttl;
            push_data.exp = endpoint_info->egress_label.exp;
            rv = _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(unit, 0/*flags*/, &push_data, &is_allocated, &temp_index);
            BCMDNX_IF_ERR_EXIT(rv);
            mep_db_entry->push_profile = (uint8)temp_index;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } 

    } else {
        /* Check if we want to disable/enable BFD transmit: in both cases we need to change destination*/
        /* In update, we support changing the tx_gport as well now */
        if (((endpoint_info->bfd_period == 0) && (existing_endpoint_info->bfd_period != 0)) ||
                ((endpoint_info->bfd_period != 0) && (existing_endpoint_info->bfd_period == 0)) ||
                (endpoint_info->tx_gport != existing_endpoint_info->tx_gport)) {

            /* If we want to disable BFD transmit,we forward traffic to invalid port*/
            if (endpoint_info->bfd_period == 0)
                mep_db_entry->system_port = _BCM_OAM_ILLEGAL_DESTINATION;
            else {
                rv = _bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info->tx_gport, &(mep_db_entry->system_port));
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type)) {
                int local_port, ignored, is_last;

                /* Free previous allocation and re-allocate*/
                rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit, mep_db_entry->local_port, &is_last);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, 0, &(mep_db_entry->system_port), &ignored, &local_port);
                BCMDNX_IF_ERR_EXIT(rv);
                mep_db_entry->local_port = local_port;
            }
        } else {
            mep_db_entry->local_port = -1; /* Value of -1 signifies to the soc layer not to update the HW.*/
        }
        /* Allowing updation of EGRESS LABEL.TTL and EXP */
        if ((existing_endpoint_info->egress_label.ttl != endpoint_info->egress_label.ttl) ||
            (existing_endpoint_info->egress_label.exp != endpoint_info->egress_label.exp)) {
            SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;
            SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
            push_data.ttl = endpoint_info->egress_label.ttl;
            push_data.exp = endpoint_info->egress_label.exp;
            /* coverity[check_return : FALSE] */
            rv = _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(unit, 0/*flags*/, &push_data, &is_allocated, &temp_index);
            _BCM_BFD_MEP_DB_ENTRY_SET_POINTER_VERIFY(rv,pointers_to_free->free_pwe_push_profile,_bcm_dpp_am_template_mpls_pwe_push_profile_free,
                    (unit, existing_mep_db_entry->push_profile, &dont_care),_bcm_dpp_am_template_mpls_pwe_push_profile_alloc,
                    (unit, 0/*flags*/, &push_data, &is_allocated, &temp_index));

            mep_db_entry->push_profile = (uint8)temp_index;
            if (is_allocated) {
                soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        /* ITMH attributes profile allocate */
        SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
        SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&old_tx_itmh_attributes);
        /* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
        tx_itmh_attributes.tc = ((uint32)(endpoint_info->int_pri & 0x1F)) >> 2;
        tx_itmh_attributes.dp = ((uint32)endpoint_info->int_pri) & 0x3;
        if (!SOC_IS_QAX(unit) && mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
            soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(unit, &old_tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Call set only if old value does not match the new value */
            if ((old_tx_itmh_attributes.tc != tx_itmh_attributes.tc) || 
                (old_tx_itmh_attributes.dp != tx_itmh_attributes.dp)) {
                /* Update will update the TC/DP for all M-HOP endpoints.
                 * Not incrementing the ref count since its the 
                 * same set of endpoints referring to new value 
                 */
                soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(unit, &tx_itmh_attributes);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } else {
            soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_get(unit, existing_mep_db_entry->priority, &old_tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Call set only if old value does not match the new value */
            if ((old_tx_itmh_attributes.tc != tx_itmh_attributes.tc) ||
                (old_tx_itmh_attributes.dp != tx_itmh_attributes.dp)) {
                rv = _bcm_dpp_am_template_oam_tx_priority_exchange(unit, 0/*flags*/, &tx_itmh_attributes, &is_allocated, &priority, 
                                                                    existing_mep_db_entry->priority, &is_last);
                BCMDNX_IF_ERR_EXIT(rv);
                mep_db_entry->priority = (uint8)priority;
                if (is_allocated) {
                    soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/* Read MEP-entry struct */
int _bcm_bfd_mep_db_entry_struct_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                                     SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry)
{
    int rv;
    SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data;
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    uint32 soc_sand_rv;
    SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;
    uint32 tx_rate, tx_rate_ref_counter;

    int single_hop_advance_a_plus=0;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && SOC_IS_ARADPLUS_A0(unit) 
        && mep_db_entry->mep_type== SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP && ((mep_db_entry->tos_ttl_profile==_BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE)
                           || ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval== 1) &&
                                    mep_db_entry->tos_ttl_profile== _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE) )) {
        /* In arad Plus ONE hop MEP DB entriers don't work. under the conditions:
           1. Arad plus
           2. Soc prop is on
           3. MEP DB entry is m-hop
           4. TTL profile is either 15 or (soc prop is on and 14)*/
        single_hop_advance_a_plus = 1;
    }



    rv = _bcm_dpp_oam_bfd_sys_port_to_tx_gport(unit, &endpoint_info->tx_gport, mep_db_entry->system_port);
    BCMDNX_IF_ERR_EXIT(rv);

    /* we use _bcm_dpp_gport_to_phy_port for set. Is it OK?
    ret = _bcm_dpp_gport_to_phy_port(unit, endpoint_info->gport, 0, &pbmp_local_ports,
                                     &(mep_db_entry->system_port), &is_local_ports, &is_lag);*/

    if (mep_db_entry->egress_if < (SOC_DPP_CONFIG(unit))->l3.nof_rifs) {
        BCM_L3_ITF_SET(endpoint_info->egress_if, BCM_L3_ITF_TYPE_RIF, mep_db_entry->egress_if);
    } else {
        BCM_L3_ITF_SET(endpoint_info->egress_if, BCM_L3_ITF_TYPE_LIF, mep_db_entry->egress_if);
    }
    endpoint_info->flags |= mep_db_entry->pbit ? BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE : 0;
    switch (mep_db_entry->mep_type) {
    case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
        endpoint_info->type = bcmBFDTunnelTypeMplsTpCc;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
	case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:	

        /* check if we are in bfd ipv4 single hop*/
         if (single_hop_advance_a_plus){
            endpoint_info->type = bcmBFDTunnelTypeUdp ;
            endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add;
		}else{
            if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
                endpoint_info->type = bcmBFDTunnelTypeUdp ;
                if(mep_db_entry->is_lsp == 1){
                    endpoint_info->type = bcmBFDTunnelTypeMpls;
                }
                if (_BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) && 
                        (mep_db_entry->flex_tos_s_hop_m_bfd_flag == SOC_PPC_OAM_FLEX_TOS_MICRO_BFD)) {
                     endpoint_info->flags |= BCM_BFD_ENDPOINT_MICRO_BFD; 
                } else if (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) && 
                        (mep_db_entry->s_bfd_flag == SOC_PPC_OAM_S_BFD_INITIATOR)) {
                    endpoint_info->flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
                } else if (!_BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit) || 
                        !(mep_db_entry->flex_tos_s_hop_m_bfd_flag == SOC_PPC_OAM_FLEX_TOS_S_HOP)) {
                    endpoint_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
                }
                endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add;
            } else {
				endpoint_info->type = bcmBFDTunnelTypeMpls ;
                if(mep_db_entry->is_vccv == 1){
                    endpoint_info->type = bcmBFDTunnelTypeMplsTpCcCv;
                }
                if (_BCM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) && 
                        (mep_db_entry->s_bfd_flag == SOC_PPC_OAM_S_BFD_INITIATOR)) {
                    endpoint_info->flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
                }
			}
		}

		if (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE) {
			int temp;
			rv = BFD_ACCESS.s_hop.tos_value.get(unit, &temp);
			BCMDNX_IF_ERR_EXIT(rv);
			tos_ttl_data.ttl =255;
			tos_ttl_data.tos =temp;
        } else if ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval== 1) && 
                (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE)) {
                int temp;
                       
                tos_ttl_data.ttl = 255;
                        endpoint_info->local_min_echo = 0;

                rv = BFD_ACCESS.s_hop.tos_value.get(unit, &temp);
                BCMDNX_IF_ERR_EXIT(rv);
                        tos_ttl_data.tos = temp;
		} else {
			SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data); 
			rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_data_get(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		
        endpoint_info->ip_tos = tos_ttl_data.tos;
        endpoint_info->ip_ttl = tos_ttl_data.ttl;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
        endpoint_info->type = bcmBFDTunnelTypeUdp;
        endpoint_info->ip_ttl = mep_db_entry->tunnel_is_mpls ? 1 : 255;
        /* Following fields should be non zero only for Jericho*/
        endpoint_info->dst_ip_addr = mep_db_entry->dst_ip_add; 
        endpoint_info->ip_subnet_length = mep_db_entry->ip_subnet_len ==31? 32 : mep_db_entry->ip_subnet_len;  /* value 31 in the HW indicates full check*/
        endpoint_info->flags |= (mep_db_entry->micro_bfd) ? BCM_BFD_ENDPOINT_MICRO_BFD : 0;
        break;
    case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
        SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
        BCMDNX_IF_ERR_EXIT(rv);

        if ( (SOC_IS_ARADPLUS(unit) && (mep_db_entry->label == SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL)) || (mep_db_entry->bfd_pwe_gal && SOC_IS_JERICHO(unit))) {
            endpoint_info->type = bcmBFDTunnelTypeMplsTpCc; /* In ARAD+ supporting MPLSTPCC through PWE entry in mep DB */
            /* This endpoint can also be of type bcmBFDTunnelTypePweGal, but the type field is rewritten in the classifier info get anyway */
        }
        else if (mep_db_entry->bfd_pwe_router_alert == 1) {
            endpoint_info->type = bcmBFDTunnelTypePweRouterAlert;
        }
        else if (push_data.ttl==1) {
            endpoint_info->type = bcmBFDTunnelTypePweTtl;
        }
        else {
            endpoint_info->type = bcmBFDTunnelTypePweControlWord;
        }

        if (mep_db_entry->bfd_pwe_ach == SOC_PPC_BFD_ACH_TYPE_PWE_CW && endpoint_info->type==bcmBFDTunnelTypePweRouterAlert ) {
            /* PWE ACH is optional only for RA types. This bit in the MEP DB must be set for all other types.*/
            endpoint_info->flags |= BCM_BFD_ENDPOINT_PWE_ACH;
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: Error in mep_db_entry struct.")));
    }

    endpoint_info->remote_discr = mep_db_entry->remote_discr;
    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) || (endpoint_info->type == bcmBFDTunnelTypeMpls)
        || (endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv)) {
        rv = _bcm_dpp_am_template_bfd_src_ip_profile_data_get(unit, mep_db_entry->src_ip_add_ptr, &(endpoint_info->src_ip_addr));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    endpoint_info->local_detect_mult = mep_db_entry->local_detect_mult;
    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(unit, mep_db_entry->min_rx_interval_ptr, &(endpoint_info->local_min_rx));
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(unit, mep_db_entry->min_tx_interval_ptr, &(endpoint_info->local_min_tx));
    BCMDNX_IF_ERR_EXIT(rv);

    rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                            mep_db_entry->ccm_interval,
                                            &tx_rate_ref_counter);
    BCMDNX_IF_ERR_EXIT(rv);

    if (tx_rate_ref_counter > 0) {
        /* Extract tx rate from software for given index
         * if reference count is greater than zero
         */
        rv = BFD_ACCESS.tx_rate_value.get(unit,
                                          mep_db_entry->ccm_interval,
                                          &tx_rate);
        BCMDNX_IF_ERR_EXIT(rv);
        ++tx_rate; /* TX rate is one lower than what is calculated.*/
    } else {
        tx_rate = 0;
    }
    if(mep_db_entry->system_port == _BCM_OAM_ILLEGAL_DESTINATION) {
        endpoint_info->bfd_period = 0;
    } else {
        endpoint_info->bfd_period = SOC_SAND_DIV_ROUND((tx_rate*(SOC_IS_ARADPLUS_AND_BELOW(unit)?333:167)), 100); /* Tx Rate is in 3.33 (JER - 1.67) ms units */
    }

    if ((endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        endpoint_info->egress_label.label = mep_db_entry->label;
        SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
        rv = _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(unit, mep_db_entry->push_profile, &push_data);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_info->egress_label.ttl = push_data.ttl;
        endpoint_info->egress_label.exp = push_data.exp;
    }
    else {
        endpoint_info->egress_label.exp = 0xFF; /* default */
        endpoint_info->egress_label.label = BCM_MPLS_LABEL_INVALID; /* default */
    }

    /* ITMH attributes profile get */
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
    if (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP && !SOC_IS_QAX(unit)) {
        soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(unit, &tx_itmh_attributes);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        rv = _bcm_dpp_am_template_oam_tx_priority_data_get(unit, mep_db_entry->priority, &tx_itmh_attributes);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* int_pri:  ((COS & 0x7) << 2) + (DP & 0x3)) */
    endpoint_info->int_pri = ((tx_itmh_attributes.tc & 0x7) << 2) + (tx_itmh_attributes.dp & 0x3);

    if (SOC_IS_ARADPLUS(unit)) {
        endpoint_info->local_state = mep_db_entry->sta;
		endpoint_info->local_diag = mep_db_entry->diag_profile;
        if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0) ) {
            rv = _bcm_dpp_am_template_bfd_flags_profile_data_get(unit, mep_db_entry->flags_profile, &endpoint_info->local_flags);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = bcm_bfd_flags_profile_index_to_flags(unit, mep_db_entry->flags_profile, &(endpoint_info->local_flags));
            BCMDNX_IF_ERR_EXIT(rv);
        }
		if (endpoint_info->local_diag == 0xf) {
			endpoint_info->flags |= BCM_BFD_ECHO;
			endpoint_info->local_diag = 0;
		}
	}

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Free all MEP-entry struct relevant info
 * This is a sequel to the defunct _bcm_mep_db_entry_dealloc. 
 *  
 * @author sinai (17/09/2015)
 * 
 * @param unit 
 * @param mep_index - Should be the existing entry.
 * @param mep_db_entry 
 * @param pointers_to_free - Should be NULL when creating a new 
 *                         endpoint
 * 
 * @return int 
 */
int _bcm_mep_db_entry_dealloc2(int unit, uint32 mep_index, SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
                               const _bcm_bfd_mep_db_entry_pointers_to_free * pointers_to_free)
{
    int is_last;
    uint32 soc_sand_rv;
    int rv;
    SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
    uint32 ipv4_multi_hop_acc_ref_counter;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_QAX(unit) && mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {

        /* Do not decrement during update */
        if (!pointers_to_free) {
            rv = BFD_ACCESS.ipv4_multi_hop_acc_ref_counter.get(unit, &ipv4_multi_hop_acc_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            ipv4_multi_hop_acc_ref_counter--;
            rv = BFD_ACCESS.ipv4_multi_hop_acc_ref_counter.set(unit, ipv4_multi_hop_acc_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (pointers_to_free == NULL && ipv4_multi_hop_acc_ref_counter == 0) {
            SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
            soc_sand_rv = soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(unit, &tx_itmh_attributes);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        if (pointers_to_free==NULL || pointers_to_free->free_tx_itmh_attributes) {
            rv = _bcm_dpp_am_template_oam_tx_priority_free(unit, mep_db_entry->priority, &is_last); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES tx_itmh_attributes;
                SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(&tx_itmh_attributes);
                soc_sand_rv = soc_ppd_oam_oamp_tx_priority_registers_set(unit, mep_db_entry->priority, &tx_itmh_attributes);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    if (((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS))
            && !((mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE) || 
                    (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE))){
        if (pointers_to_free==NULL || pointers_to_free->free_tos_ttl_profile) {
            rv = _bcm_dpp_am_template_bfd_tos_ttl_profile_free(unit, mep_db_entry->tos_ttl_profile, &is_last); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA tos_ttl_data;
                SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(&tos_ttl_data);
                soc_sand_rv = soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(unit, mep_db_entry->tos_ttl_profile, &tos_ttl_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }
    if ( pointers_to_free==NULL && ((mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_SINGLE_HOP_TTL_PROFILE) || ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 1) &&
                                                                                    (mep_db_entry->tos_ttl_profile == _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE)))) {
        /* Only applicable when deleting.*/
        int tmp;
        rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &tmp);
        BCMDNX_IF_ERR_EXIT(rv);
        if (tmp > 0) {
            tmp--;
            rv = BFD_ACCESS.s_hop.ref_counter.set(unit, tmp);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if ((mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||
        (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
        (mep_db_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS)) {

        if (pointers_to_free==NULL || pointers_to_free->free_src_ip_profile) {
            rv = _bcm_dpp_am_template_bfd_src_ip_profile_free(unit, mep_db_entry->src_ip_add_ptr, &is_last); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                soc_sand_rv = soc_ppd_oam_bfd_ipv4_src_addr_select_set(unit, mep_db_entry->src_ip_add_ptr, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    if ((mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) &&
        (mep_db_entry->mep_type != SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP)) {
        if (pointers_to_free==NULL || pointers_to_free->free_pwe_push_profile) {
            rv = _bcm_dpp_am_template_mpls_pwe_push_profile_free(unit, mep_db_entry->push_profile, &is_last); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                SOC_PPC_MPLS_PWE_PROFILE_DATA push_data;
                SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(&push_data);
                soc_sand_rv = soc_ppd_oam_mpls_pwe_profile_set(unit, mep_db_entry->push_profile, &push_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    if (pointers_to_free == NULL || pointers_to_free->free_bfd_tx_rate) {
        rv = _bcm_bfd_tx_rate_profile_index_free(unit, mep_db_entry->ccm_interval);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (pointers_to_free==NULL || pointers_to_free->free_req_interval_pointer_local_min_rx) {
        rv = _bcm_dpp_am_template_bfd_req_interval_pointer_free(unit, mep_db_entry->min_rx_interval_ptr, &is_last); 
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_rx_interval_ptr, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    if (pointers_to_free==NULL || pointers_to_free->free_req_interval_pointer_local_min_tx) {
        rv = _bcm_dpp_am_template_bfd_req_interval_pointer_free(unit, mep_db_entry->min_tx_interval_ptr, &is_last); 
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            soc_sand_rv = soc_ppd_oam_bfd_req_interval_pointer_set(unit, mep_db_entry->min_tx_interval_ptr, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    if (SOC_IS_ARADPLUS(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
        if (pointers_to_free==NULL || pointers_to_free->free_flags) {
            rv = _bcm_dpp_am_template_bfd_flags_profile_free(unit, mep_db_entry->flags_profile, &is_last); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, mep_db_entry->flags_profile, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }


    if (PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_db_entry->mep_type) && pointers_to_free==NULL) {
        /* This is freed only under endpoint destroy.*/
        rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit,mep_db_entry->local_port, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN;
}



/* Fill classifier entry struct */
int _bcm_bfd_classifier_mep_entry_struct_set(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, int dip_ndx) {
    uint32 ret;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    classifier_mep_entry->tx_gport = endpoint_info->tx_gport; /* Used for re-creating tx_gport in case bfd transmit is disable */
    classifier_mep_entry->ma_index = endpoint_info->local_discr; /* Used for IP single hop*/
    classifier_mep_entry->remote_gport = endpoint_info->remote_gport;
    classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
    classifier_mep_entry->flags |= ((endpoint_info->flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME) != 0) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_TIMEOUT_SET_EXPLICITLY : 0;
    classifier_mep_entry->your_discriminator = endpoint_info->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK;
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) {
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;
    }
    if (((!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type)) ||
        _BCM_BFD_IS_BFD_O_MPLS_SPECIAL_TTL(endpoint_info->type)) &&
        (endpoint_info->gport != BCM_GPORT_INVALID)) {
        ret = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS|_BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(ret);

        classifier_mep_entry->lif = gport_hw_resources.global_in_lif;
    }
    else {
         /* LSBs of your discriminator in this case are used for classification in the O-EM tables.*/
        classifier_mep_entry->lif = _BCM_OAM_INVALID_LIF;
    }

    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeMpls:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS;
        if(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP){
            classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
            classifier_mep_entry->dip_profile_ndx = dip_ndx;
            classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP;
        }
        break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS;
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP_CC_CV;
        break;
    case bcmBFDTunnelTypeMplsTpCc:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP;
        break;
    case bcmBFDTunnelTypePweGal:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL;
        break;
    case bcmBFDTunnelTypeUdp:
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
            classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP;
            classifier_mep_entry->dip_profile_ndx = dip_ndx;
        } else {
            classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP;
        }
        break;
    case bcmBFDTunnelTypePweControlWord:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE;
        break;
    case bcmBFDTunnelTypePweRouterAlert:
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE;
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_RA;
        break;
    case bcmBFDTunnelTypePweTtl:
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_TTL_1;
        classifier_mep_entry->mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type")));
    }

    if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info)) {
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR;
    }
    else if(_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info)){
        classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Read classifier entry struct */
int _bcm_bfd_classifier_mep_entry_struct_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, int *dip_ndx) {
    int ret;
    BCMDNX_INIT_FUNC_DEFS;


    endpoint_info->flags |= ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) ? BCM_BFD_ENDPOINT_IN_HW : 0;
    endpoint_info->remote_gport = classifier_mep_entry->remote_gport;

        switch (classifier_mep_entry->mep_type) {
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS:
            if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_MPLS_PHP) {
                endpoint_info->type = bcmBFDTunnelTypeMplsPhp;
            } else if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP_CC_CV){
                endpoint_info->type = bcmBFDTunnelTypeMplsTpCcCv;
            } else {
                endpoint_info->type = bcmBFDTunnelTypeMpls;
            }
            break;
        case SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP:
            endpoint_info->type = bcmBFDTunnelTypeMplsTpCc;
            break;
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL:
            endpoint_info->type = bcmBFDTunnelTypePweGal;
            break;
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP:
            endpoint_info->type = bcmBFDTunnelTypeUdp;
            endpoint_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
            if(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP){
                endpoint_info->type = bcmBFDTunnelTypeMpls;
                endpoint_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
            }
            break;
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP:
            endpoint_info->type = bcmBFDTunnelTypeUdp;
            break;
        case SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE:
            if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_TTL_1) {
                endpoint_info->type = bcmBFDTunnelTypePweTtl;
            }
            else if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_RA) {
                endpoint_info->type = bcmBFDTunnelTypePweRouterAlert;
            }
            else {
                endpoint_info->type = bcmBFDTunnelTypePweControlWord;
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Internal error")));
        }
    if ((classifier_mep_entry->mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||
        (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR)) {
        *dip_ndx = classifier_mep_entry->dip_profile_ndx; 
    }


    endpoint_info->local_discr = classifier_mep_entry->your_discriminator;
    if ((!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type) ||
        _BCM_BFD_IS_BFD_O_MPLS_SPECIAL_TTL(endpoint_info->type)) &&
        (classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF)) {
        _bcm_dpp_gport_hw_resources gport_hw_resources;

        gport_hw_resources.global_in_lif = classifier_mep_entry->lif;
        ret = _bcm_dpp_gport_from_hw_resources(unit, &endpoint_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(ret);
    }
    else {
        endpoint_info->gport = BCM_GPORT_INVALID;
    }

    if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) {
        endpoint_info->flags |= BCM_BFD_ENDPOINT_HW_ACCELERATION_SET;
    }

    if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR) {
        endpoint_info->flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
    }
    if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR) {
        endpoint_info->flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_dpp_oam_bfd_mep_info_construct_bfd_mep_list_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data) {
    int rv = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *mep_entry_p;
    uint32 mep_index;

    BCMDNX_INIT_FUNC_DEFS;

    /*get the endpoint_info*/
    mep_entry_p = (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *)data;
    mep_index = *(uint32 *)key;

    if (SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_entry_p->mep_type))
    {
        ENDPOINT_LIST_PTR endpoint_list ;
        rv = BFD_ACCESS._bcm_bfd_endpoint_list.get(unit, &endpoint_list) ;
    	BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_endpoint_list_member_add(unit, endpoint_list, mep_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
#endif /*BCM_WARM_BOOT_SUPPORT*/

/*
 * _bcm_bfd_default_id_from_ep_id
 *
 * If an endpoint ID is one of the default BCM endpoint IDs,
 * trnaslates the ID to a default endpoint ID in the soc layer.
 * If not a default BCM endpoint ID, returns a parameter error.
 */
int _bcm_bfd_default_id_from_ep_id(int unit, int endpoint, ARAD_PP_OAM_DEFAULT_EP_ID *default_id)
{

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(default_id);

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (endpoint != -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad.")));
        }
    }
    else {
        switch (endpoint) {
        case BCM_BFD_ENDPOINT_DEFAULT0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_0;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT1:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_1;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT2:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_2;
            break;
        case BCM_BFD_ENDPOINT_DEFAULT3:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_3;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not default.")));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Checks whether a combination of flags and gport represent
 * endpoint that is trapped to an OAMP.
 * The generity of this is necessary since sometimes this needs
 * to be checked for bcm_bfd_endpoint_info_t and sometimes for
 * SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY. This function can be used
 * in both cases.
 */
int _bcm_bfd_acceleration_to_oamp_by_trap_and_flags_check( int unit,
                                                           /* Inputs */
                                                           uint32 flags,
                                                           uint32 acceleration_flag,
                                                           uint32 server_flag,
                                                           bcm_gport_t gport, /* Potentially a trap */
                                                           /* Outputs */
                                                           uint8 *is_acc_to_oamp,
                                                           uint8 *is_oamp_server
   ) {
    uint32 rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    uint32 trap_code;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    int count=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(is_acc_to_oamp);
    BCMDNX_NULL_CHECK(is_oamp_server);

    *is_oamp_server = ((flags & server_flag) != 0);

    if ((flags & acceleration_flag) == 0) {
        /* Definitely not accelerated endpoint */
        *is_acc_to_oamp = 0;
    }
    else if (gport == BCM_GPORT_INVALID) {
        /* Classic acceleration */
        *is_acc_to_oamp = 1;
    }
    else {
        if (*is_oamp_server) {
            /* Accelerated to OAMP on BFD server */
            *is_acc_to_oamp = 1;
        }
        else if (BCM_GPORT_IS_TRAP(gport)){
            /* Endpoint is flagged accelerated, not to server
               and the remote GPort is a trap. Check if this trap's
               destination is the local OAMP */

            /* Get trap configuration for the dest-port */
            _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, gport, rv);
            BCMDNX_IF_ERR_EXIT(rv);
            
            trap_code = BCM_GPORT_TRAP_GET_ID(gport);
            bcm_rx_trap_config_t_init(&trap_config);
            rv = bcm_petra_rx_trap_get(unit, trap_code, &trap_config);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get OAMP port(s) */
            rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP,
                                             _BCM_OAM_NUM_OAMP_PORTS(unit),
                                             oamp_port, &count);
            BCMDNX_IF_ERR_EXIT(rv);
            if (count) {
                /* Compare the 2 */
                rv = _bcm_dpp_gport_compare(unit, oamp_port[0],
                                            trap_config.dest_port,
                                            _bcmDppGportCompareTypeSystemPort,
                                            is_acc_to_oamp);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* OAMP port isn't configured so the trap destination
                   cannot be it */
                *is_acc_to_oamp = 0;
            }
        }
        else {
            /* Not accelerated to OAMP */
            *is_acc_to_oamp = 0;
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Checks whether the endpoint is accelerated to the OAMP
 */
int _bcm_bfd_endpoint_acceleration_to_oamp_get( int unit,
                                                bcm_bfd_endpoint_info_t *endpoint_info,
                                                uint8 *is_acc_to_oamp, uint8 *is_oamp_server)
{
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(endpoint_info);
    BCMDNX_NULL_CHECK(is_acc_to_oamp);
    BCMDNX_NULL_CHECK(is_oamp_server);

    rv = _bcm_bfd_acceleration_to_oamp_by_trap_and_flags_check(
                unit,
                endpoint_info->flags,
                BCM_BFD_ENDPOINT_IN_HW,
                BCM_BFD_ENDPOINT_HW_ACCELERATION_SET,
                endpoint_info->remote_gport,
                is_acc_to_oamp,
                is_oamp_server);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Fill LEM info struct from enpoint info
 * 
 * @author sinai (24/08/2015)
 * 
 * @param unit 
 * @param ep_info 
 * @param lem_info 
 */
int _bcm_bfd_endpoint_info_to_lem_info(int unit, const bcm_bfd_endpoint_info_t *ep_info, ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO *lem_info, int trap_code, uint8 fwd_strenght, int snp_strength) {
    BCMDNX_INIT_FUNC_DEFS;
    if (lem_info->soc_sand_magic_num !=SOC_SAND_MAGIC_NUM_VAL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("LEM info struct not initialized.")));
    }
    lem_info->is_ipv6 = (ep_info->flags & BCM_BFD_ENDPOINT_IPV6) != 0;
    lem_info->local_discriminator = ep_info->local_discr;
    lem_info->oam_id = ep_info->id;
    lem_info->is_accelerated = (ep_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0;
    lem_info->trap_code = trap_code;
    lem_info->fwd_strngth = fwd_strenght;
    lem_info->snp_strength = snp_strength;
    lem_info->remote_detect_mult = ep_info->remote_detect_mult; /* May be ignored, depends on soc prop*/
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Checks whether the endpoint is accelerated to the OAMP
 */
int _bcm_bfd_classifier_entry_acceleration_to_oamp_get( int unit,
                           SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                      uint8 *is_acc_to_oamp, uint8 *is_oamp_server)
{
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(classifier_mep_entry);
    BCMDNX_NULL_CHECK(is_acc_to_oamp);
    BCMDNX_NULL_CHECK(is_oamp_server);

    rv = _bcm_bfd_acceleration_to_oamp_by_trap_and_flags_check(
                unit,
                classifier_mep_entry->flags,
                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED,
                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER,
                classifier_mep_entry->remote_gport,
                is_acc_to_oamp,
                is_oamp_server);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_bfd_preallocate_fixed_tx_rates(int unit)
{
    int rv;
    int rfc_rates[] = { 4, 10, 20, 50, 100, 1000 };
    int rate;
    uint32 tx_rate;
    int i;
    char name[40];

    BCMDNX_INIT_FUNC_DEFS;

    for (i = 0; i < 6; i++) {
        sal_sprintf(name, "bfd_fixed_tx_rate_mgmt_rate%d", i);
        rate = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, name, rfc_rates[i]);
        tx_rate = _BCM_BFD_ENDPOINT_INFO_BFD_PERIOD_TO_MEP_DB_TX_RATE(rate);
        if (tx_rate <= 0x3ff) {
            rv = _bcm_bfd_tx_rate_profile_alloc(unit, tx_rate, NULL);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/*
 * Begin BCM Functions
 */


/* Initialize the BFD subsystem */
int bcm_petra_bfd_init(int unit)
{
    uint32 soc_sand_rv;
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_oamp, trap_config_cpu, trap_config_uc;
    bcm_port_config_t *port_config = NULL;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)], cpu_port;
    int count_erp, bfd_ipv6_trap_port, trap_code_converted, tmp=0;
    uint8 oam_is_init, is_bfd_init, is_allocated;
    uint32 prof_ind, flags_value, trap_id_sw;
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE success_vt;
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE success_tt;
    ARAD_PP_ISEM_ACCESS_KEY               sem_key;
    ARAD_PP_ISEM_ACCESS_ENTRY             sem_entry;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (is_bfd_init) {
#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_WARM_BOOT(unit))
#endif /*BCM_WARM_BOOT_SUPPORT*/
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: BFD already initialized.\n")));
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    else {
        if (SOC_WARM_BOOT(unit)) {
             if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable
                 && (soc_property_get(unit, spn_BFD_IPV6_ENABLE, 0) != 2)) {
                    rv = bcm_petra_bfd_uc_init(unit);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            BCM_EXIT;
        }
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

    if (!SOC_WARM_BOOT(unit)) {
        /* Software state init */
        rv = BFD_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = BFD_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* We need also that oam will be allocated */
        rv = OAM_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = OAM_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        soc_sand_rv = bcm_dpp_endp_lst_init(unit,MAX_NOF_ENDP_LSTS);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = bcm_dpp_endp_lst_m_init(unit,MAX_NOF_ENDP_LST_MS);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

        /*init global variables*/
        rv = BFD_ACCESS.s_hop.tos_value.set(unit, tmp);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.s_hop.ref_counter.set(unit, tmp);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    /* In warmboot oam init is called first and inits DBs, so we don't need to init them from BFD */
    if (!oam_is_init) {
        rv = _bcm_dpp_oam_dbs_init(unit, FALSE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_ARADPLUS(unit)) {
            rv = soc_ppd_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Validating bfd_enable SOC property is set */
    if(!soc_property_get(unit, spn_BFD_ENABLE, 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("Error: bfd_enable SOC property must be set in order to initialize BFD.\n")));
    }

    /* Validating at least one OAMP port is configured */
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
    BCMDNX_IF_ERR_EXIT(rv);
    if (count_erp < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oamp port disabled, can't init OAM.")));
    }

    /* Init the uKernel BFD application */
    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable
        && (soc_property_get(unit, spn_BFD_IPV6_ENABLE, 0) != 2)) {
        rv = bcm_petra_bfd_uc_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!SOC_WARM_BOOT(unit)) {
        ENDPOINT_LIST_PTR endpoint_list ;

        rv = _bcm_dpp_oam_endpoint_list_init(unit,&endpoint_list);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = BFD_ACCESS._bcm_bfd_endpoint_list.set(unit, endpoint_list);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_rx_trap_config_t_init(&trap_config_oamp);
        bcm_rx_trap_config_t_init(&trap_config_cpu);

        /*get CPU port*/
        BCMDNX_ALLOC(port_config, sizeof(bcm_port_config_t),
            "bcm_petra_bfd_init.port_config");
        if (port_config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
        }
        rv = bcm_petra_port_config_get(unit, port_config);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(port_config->cpu, cpu_port) {
            break; /*get first CPU port*/
        }

        /*creating cpu trap id*/
        rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_CPU, SOC_PPC_TRAP_CODE_OAM_CPU);
        BCMDNX_IF_ERR_EXIT(rv);

        /* BFD traps setup - unnecessary in WB */

        if (!oam_is_init) {
            rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_CPU, &trap_id_sw);
            BCMDNX_IF_ERR_EXIT(rv);

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,
                                                                    trap_id_sw,
                                                                    &trap_code_converted));

            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);

            trap_config_cpu.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
            trap_config_cpu.dest_port = cpu_port;
            trap_config_cpu.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_cpu.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_cpu);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_OAM_TRAP_ID_CPU, trap_id_sw /*transfering the SW id*/);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &trap_id_sw);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_CPU, trap_id_sw);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /*creating oamp trap ids*/
        trap_config_oamp.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_oamp.dest_port = *oamp_port;

        /*configue bfd echo trap code*/
        if (soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0) == 1) {
            trap_id_sw = SOC_PPC_TRAP_CODE_BFD_ECHO;

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,
                                                                    trap_id_sw,
                                                                    &trap_code_converted));

            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);
            trap_config_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_oamp.forwarding_header = bcmRxTrapForwardingHeaderThirdHeader;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
            BCMDNX_IF_ERR_EXIT(rv);

            /*configure table in the LEM - bfd echo*/
            if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1) {
               rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_flp_dbal_bfd_echo_program_tables_init, (unit));
               BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        /*configue bfd ipv4 single hop your discriminator not found trap code*/
        if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop || SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable) {
            trap_id_sw = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "common_bfd_discr_not_found_trap", 0)) ?
                          SOC_PPC_TRAP_CODE_BFD_IPV4_IPV6_YOUR_DISCR_NOT_FOUND : SOC_PPC_TRAP_CODE_BFD_IPV4_YOUR_DISCR_NOT_FOUND;
            /* It is possible (according to soc prop) to use the same trap for IPv6 and IPv4 your discr not found.*/
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,
                                                                    trap_id_sw,
                                                                    &trap_code_converted));
            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);
            trap_config_cpu.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
            trap_config_cpu.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config_cpu.dest_port = cpu_port;
            trap_config_cpu.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_cpu);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_OAMP_IPV4, SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV4);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdIpv4, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_config_oamp.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
        trap_config_oamp.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_OAMP_MPLS, SOC_PPC_TRAP_CODE_TRAP_BFD_O_MPLS);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdMpls, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_OAMP_PWE, SOC_PPC_TRAP_CODE_TRAP_BFD_O_PWE);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdPwe, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_OAMP_CC_MPLS_TP, SOC_PPC_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamBfdCcMplsTp, (int *)&trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_oamp);
        BCMDNX_IF_ERR_EXIT(rv);

        bfd_ipv6_trap_port = soc_property_get(unit, spn_BFD_IPV6_TRAP_PORT, -1);
        if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable) {
            if (bfd_ipv6_trap_port != -1) {
                bcm_rx_trap_config_t_init(&trap_config_uc);
                trap_config_uc.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
                trap_config_uc.dest_port = bfd_ipv6_trap_port;
                rv = BFD_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_BFD_TRAP_ID_UC_IPV6, SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV6);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_UC_IPV6, &trap_id_sw);
                BCMDNX_IF_ERR_EXIT(rv);

                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,
                                                                        trap_id_sw,
                                                                        &trap_code_converted));

                rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
                BCMDNX_IF_ERR_EXIT(rv);
                trap_config_uc.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config_uc.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_uc);
                BCMDNX_IF_ERR_EXIT(rv);

                /*BFD Ipv6 Echo Trap reservation */
                trap_code_converted = 0;
                bcm_rx_trap_config_t_init(&trap_config_uc);
                trap_config_uc.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
                trap_config_uc.dest_port = bfd_ipv6_trap_port;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,
                                                                        SOC_PPC_TRAP_CODE_BFD_ECHO_O_IPV6,
                                                                        &trap_code_converted));

                rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
                BCMDNX_IF_ERR_EXIT(rv);
                trap_config_uc.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config_uc.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, &trap_config_uc);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BFD over IPv6 can't be enabled without BFD IPv6 trap port configuration")));
            }
        }

        soc_sand_rv = soc_ppd_oam_init(unit, TRUE /*BFD*/);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _BCM_BFD_ENABLED_SET(TRUE);

        if (SOC_IS_ARADPLUS(unit)) {
            /* Set the diag values (1-15)*/
            for (prof_ind = 1;  prof_ind < 16 /* 16 profiles */; ++prof_ind) {
                soc_sand_rv = soc_ppd_oam_bfd_diag_profile_set(unit,prof_ind, prof_ind );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0) == 0) {
                /* Set the flags values (1-7)*/
                for (prof_ind = 1;  prof_ind < 8 /* 7 profiles */; ++prof_ind) {
                    soc_sand_rv = bcm_bfd_flags_profile_index_to_flags(unit, prof_ind, &flags_value);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    soc_sand_rv = soc_ppd_oam_bfd_flags_profile_set(unit, prof_ind, flags_value);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_fixed_tx_rate_mgmt", 0)) {
            rv = _bcm_bfd_preallocate_fixed_tx_rates(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }



    if (SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma == 0 ||
        SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma == 0 ||
        !SOC_IS_JERICHO(unit)) {
        /* Thess lines enable the interrupts and disables the bypass interrupt mechanism
           In Jericho when using the DMA this interrupt shouldn't be enabled.
           In Arad the ECI_GENERAL_CONFIGURATION_2 register sets this "automatically"*/
        rv = _bcm_dpp_oamp_interrupts_init(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* if non bfd fowarding Set FLP PTC profile for rcy portto SOC_TMC_PORTS_FLP_PROFILE_NON_BFD_FRWD*/
        if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)) && (!SOC_WARM_BOOT(unit))) {
        uint32 bfd_non_fowarding_port;
        uint32 core_i;
        ARAD_PORTS_PROGRAMS_INFO port_prog_info;
        arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
        bfd_non_fowarding_port = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0));
        /* Soc property indicates core 0 rcy port. core 1 rcy port should be defined to next local port */
        for (core_i = 0; core_i < SOC_DPP_DEFS_GET(unit, nof_cores); core_i++) {
            soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, (bfd_non_fowarding_port+core_i), &port_prog_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_NON_BFD_FRWD;
            soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, (bfd_non_fowarding_port+core_i), &port_prog_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

    }

    if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics && (!SOC_WARM_BOOT(unit))) {
        /* Set FLP PTC profile for OAMP port */
        uint32 oamp_local_port;
        ARAD_PORTS_PROGRAMS_INFO port_prog_info;
        _bcm_dpp_gport_info_t gport_info; 
        arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
        soc_sand_rv = _bcm_dpp_gport_to_phy_port(unit, *oamp_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        oamp_local_port = gport_info.local_port;
        soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, oamp_local_port, &port_prog_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
        soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, oamp_local_port, &port_prog_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* inserting an entry to the SEM- For Identifying BFD packets for statistics feature */

        /* add entry to tt stage */
        sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISA;
        sem_key.key_info.oam_stat.bfd_format = 0x1; /*pph fwd code ipv4*/
        rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
        sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISB;
        sem_key.key_info.oam_stat.bfd_format = 0x4; /*v4oLSP*/
        rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
        BCMDNX_IF_ERR_EXIT(rv);
        /* add entry to vt stage opaque=5 bfd*/
        sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_VT;
        sem_key.key_info.oam_stat.pph_type = 0x4;
        sem_key.key_info.oam_stat.opaque = 0x7;
        rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_vt);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_oam_sw_db_locks_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCM_FREE(port_config);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to start TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_tx_start(int unit)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_tx_start(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to stop TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_tx_stop(int unit)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_tx_stop(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_bfd_ipv6_endpoint_create(int unit, bcm_bfd_endpoint_info_t *endpoint_info_lcl) {

    bcm_error_t         rv = BCM_E_NONE;
    bcm_bfd_endpoint_t  id_internal;

    BCMDNX_INIT_FUNC_DEFS;

    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
        _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint_info_lcl->id);
        endpoint_info_lcl->id = id_internal;
    }
    rv = bcm_petra_bfd_uc_endpoint_create(unit, endpoint_info_lcl);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->id, endpoint_info_lcl->id);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Specific validity checks for Seamless BFD Initiator endpoint create/update */
int _bcm_bfd_sbfd_initiator_endpoint_validity_checks(int unit, bcm_bfd_endpoint_info_t *endpoint_info_lcl)
{
    uint8 ttl = 0;
    BCMDNX_INIT_FUNC_DEFS;

    /* An endpoint cannot be both initiator and reflector */
    if (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: An endpoint cannot be both Seamless BFD initiator and reflector.\n")));
    }

    /* Tunnel types UDP and MPLS are only supported currently for SBFD initiator */
    if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && 
        (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint is supported only on UDP and MPLS tunnel types.\n")));
    }

    /* UDP src port = 7784 should not be allowed for SBFD initiator endpoints */
    if (endpoint_info_lcl->udp_src_port == _BCM_BFD_RESERVED_UDP_PORT_FOR_SBFD) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint cannot use the reserved UDP port number %d as UDP source port.\n"), _BCM_BFD_RESERVED_UDP_PORT_FOR_SBFD));
    }

    /* Check the tx params only for accelerated endpoints */
    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
        /* Demand BIT(1st bit) must be set for SBFD initiator endpoints */ 
        if (!(endpoint_info_lcl->local_flags & (1 << 1))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint needs to have DEMAND bit set in local flags.\n")));
        }

        /* INIT state is not a valid state for SBFD initiator endpoints */
        if (endpoint_info_lcl->local_state == bcmBFDStateInit) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint cannot have INIT as local state.\n")));
        }

        /* Remote discr should be NON zero for SBFD initiator endpoints */
        if (endpoint_info_lcl->remote_discr == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint should have a non zero remote discriminator.\n")));
        }

        /* local_min_rx should be 0 for SBFD initiator endpoints */
        if (endpoint_info_lcl->local_min_rx != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint cannot have non zero local_min_rx.\n")));
        }

        /* local_min_echo should be 0 for SBFD initiator endpoints */
        if (endpoint_info_lcl->local_min_echo != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint cannot have non zero local_min_echo.\n")));
        }


        /* TTL should be set to 255 */
        if (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) {
            ttl = endpoint_info_lcl->ip_ttl; 
        } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeMpls) {
            ttl = endpoint_info_lcl->egress_label.ttl; 
        }
        if (ttl != 255) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Seamless BFD Initiator endpoint should have ttl set to 255.\n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Specific validity checks for Seamless BFD Reflector endpoint create/update */
int _bcm_bfd_sbfd_reflector_endpoint_validity_checks(int unit, bcm_bfd_endpoint_info_t *endpoint_info_lcl)
{
    uint8 no_of_sbfd_reflector = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* An endpoint cannot be both initiator and reflector */
    if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: An endpoint cannot be both Seamless BFD initiator and reflector.\n")));
    }
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_reflector.get(unit, &no_of_sbfd_reflector));
    if(!(endpoint_info_lcl->flags&BCM_BFD_ENDPOINT_UPDATE)&&(no_of_sbfd_reflector == 1)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
           (_BSL_BCM_MSG("Error: reflector can only support one endpoint.\n")));
    }

    /* Tunnel types UDP and MPLS are only supported currently for SBFD Reflector */
    if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && 
        (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls))  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Tunnel types UDP and MPLS are only supported currently for SBFD Reflector.\n")));
    }

    /* bfd_period must set to zero for SBFD Reflector since it doesn't use OAMP to send BFD packet*/
    if ((endpoint_info_lcl->bfd_period != 0) ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: bfd_period must set to zero for SBFD Reflector since it doesn't use OAMP to send BFD packet\n")));
    }

    /* Check the tx params only for accelerated endpoints */
    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD Reflector doesn't use OAMP.\n")));
    }
    /* Demand BIT(1st bit) must not be set for SBFD Reflector endpoints */
    if ((endpoint_info_lcl->local_flags & (1 << 1))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD Reflector endpoint must not have DEMAND bit set in local flags.\n")));
    }

    /*  valid state checking for SBFD initiator endpoints */
    if ((endpoint_info_lcl->local_state != bcmBFDStateAdminDown) && ((endpoint_info_lcl->local_state != bcmBFDStateUp))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD Reflector endpoint can be AdminDown or Up state only.\n")));
    }

    /* local_min_rx shouldn't be 0 for SBFD initiator endpoints */
    if (endpoint_info_lcl->local_min_rx == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD Reflector endpoint cannot have zero local_min_rx.\n")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* General validity checks for BFD endpoint create/update */
int _bcm_bfd_endpoint_validity_checks(int unit, bcm_bfd_endpoint_info_t *endpoint_info_lcl,
                                      uint8 *is_endpoint_acc_to_oamp)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv = 0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry_temp;
    ENDPOINT_LIST_PTR rmep_list_p;
    uint32 bfd_period_max;
    int    remote_flags_not_supported;
    int    local_flags_not_supported;
    int    correct_flags = 0;
    uint8  is_oamp_server = 0;
    uint8  is_default;
    uint8  found;
    uint16 udp_sport;
    uint8  found_profile;
    uint32 profile_temp;
    uint32 oem_key;
    uint8  is_mp_type_flexible_dummy, is_mip_dummy;
    uint32 rmep_index = (uint32)(-1);
    uint8  is_1_hop_extended = 0;
    int    dip_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(endpoint_info_lcl);
    BCMDNX_NULL_CHECK(is_endpoint_acc_to_oamp);

    /* Seamless BFD is available only for JERICHO and above devices */
    if ((!SOC_IS_JERICHO(unit)) && 
        (_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Seamless BFD feature is available only for JERICHO and above devices.\n")));
    }

    if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
        rv = _bcm_bfd_sbfd_initiator_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)) {
        rv = _bcm_bfd_sbfd_reflector_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    correct_flags |= BCM_BFD_ENDPOINT_REMOTE_WITH_ID;
    correct_flags |= BCM_BFD_ENDPOINT_UPDATE | BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE |
                     BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE | BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE | BCM_BFD_ENDPOINT_REMOTE_EVENT_DISABLE |
                     BCM_BFD_ENDPOINT_RDI_AUTO_UPDATE | BCM_BFD_ENDPOINT_RDI_ON_LOC | BCM_BFD_ENDPOINT_HW_ACCELERATION_SET |
                     BCM_BFD_ECHO | BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME | BCM_BFD_ENDPOINT_MICRO_BFD |
                     BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE; 

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id);

    /*check if we are in bfd ipv4 single hop extended mode*/
    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) {
        if ((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) && (is_default==0)) {
            /* includes micro bfd.*/
            is_1_hop_extended = 1;
        }
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) || (endpoint_info_lcl->type == bcmBFDTunnelTypeMpls)
        || (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv)) {
        correct_flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0) && (BCM_L3_ITF_VAL_GET(endpoint_info_lcl->egress_if) & 0xfffe0000) > 0){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Arad Mode support only 2^17 interfaces.")));
    }

    if ((endpoint_info_lcl->flags & BCM_BFD_ECHO) && !soc_property_get(unit, spn_BFD_ECHO_ENABLED, 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ECHO is supported only on ARAD+.")));
    }

    if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->flags & BCM_BFD_ECHO)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BFD ECHO suppoerted only on IPv4 type.")));
    }

    if ((endpoint_info_lcl->flags & BCM_BFD_ECHO) && ((!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) || (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP)))) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ECHO flag must be set with BCM_BFD_ENDPOINT_IN_HW and BCM_BFD_ENDPOINT_MULTIHOP flags")));
    }

    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) && (SOC_IS_ARAD_B1_AND_BELOW(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_HW_ACCELERATION_SET is supported only on ARAD+ and above devices.")));
    }

    if ((endpoint_info_lcl->local_state != bcmBFDStateAdminDown) && (endpoint_info_lcl->local_state != bcmBFDStateDown) && (endpoint_info_lcl->local_state != bcmBFDStateUp)
        && (endpoint_info_lcl->local_state != bcmBFDStateInit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: local state incorrect. Allowed values are only bcmBFDStateAdminDown, bcmBFDStateDown, bcmBFDStateUp, bcmBFDStateInit.")));
    }
    if ((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls) && (endpoint_info_lcl->type != bcmBFDTunnelTypePweControlWord)
        && (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCc) && (endpoint_info_lcl->type != bcmBFDTunnelTypePweGal)&& (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCcCv)) {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit) || (endpoint_info_lcl->type != bcmBFDTunnelTypePweRouterAlert)) {
            if (SOC_IS_JERICHO_AND_BELOW(unit) || (endpoint_info_lcl->type != bcmBFDTunnelTypePweTtl)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: type incorrect. Allowed values are only bcmBFDTunnelTypeUdp, bcmBFDTunnelTypeMpls, bcmBFDTunnelTypePweControlWord, bcmBFDTunnelTypeMplsTpCc, bcmBFDTunnelTypeMplsTpCcCv"
                                               "%s."),SOC_IS_JERICHO(unit)? ", bcmBFDTunnelTypePweGal, bcmBFDTunnelTypePweRouterAlert" : "",SOC_IS_JERICHO_PLUS(unit)? ", bcmBFDTunnelTypePweTtl" : ""));
            }
        }
    }

    if (SOC_IS_JERICHO(unit) && ((endpoint_info_lcl->type == bcmBFDTunnelTypePweRouterAlert) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl))) {
        correct_flags |= BCM_BFD_ENDPOINT_PWE_ACH;
    }
    if (endpoint_info_lcl->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: invalid flag combination is set.")));
    }
    if (((endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMpls) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCcCv))
        && (endpoint_info_lcl->src_ip_addr != 0 || endpoint_info_lcl->udp_src_port != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: src_ip_addr and udp_src_port only supported for BFDoMPLS and BFDoIPV4.")));
    }

    if (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl) {
        if (endpoint_info_lcl->egress_label.ttl != 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: In PWE TTL type egress_label.ttl must be 1.")));
        }
    }

    if ( (endpoint_info_lcl-> flags & BCM_BFD_ENDPOINT_IN_HW) ==0 || (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp)) {
        /* NON acc endpoint or IP endpoint. Either way TX MPLS fields must be 0. */
        if (endpoint_info_lcl->egress_label.ttl ||  endpoint_info_lcl->egress_label.flags || endpoint_info_lcl->egress_label.tunnel_id  || endpoint_info_lcl->egress_label.qos_map_id
            || endpoint_info_lcl->egress_label.l3_intf_id   || endpoint_info_lcl->egress_label.pkt_cfi  || (endpoint_info_lcl->egress_label.label != BCM_MPLS_LABEL_INVALID) ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: egress_label must be set only for accelerated MPLS endpoints.")));
        }
    }

    if (((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) && (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE))
        || ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE) && (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE))
        || ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_RX_RDI_AUTO_UPDATE) && (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: Flags BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE ,  BCM_BFD_ENDPOINT_RX_REMOTE_EVENT_DISABLE , BCM_BFD_ENDPOINT_REMOTE_UPDATE_STATE_DISABLE "
                                           "are mutually exclusive")));
    }

    rv = _bcm_bfd_endpoint_acceleration_to_oamp_get(unit, endpoint_info_lcl, is_endpoint_acc_to_oamp, &is_oamp_server);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!(*is_endpoint_acc_to_oamp)) {
        /* non accelerated EP. TX fields should be 0.*/
        if ( endpoint_info_lcl->int_pri || endpoint_info_lcl->local_diag || endpoint_info_lcl->remote_discr || endpoint_info_lcl->remote_flags || endpoint_info_lcl->local_flags || endpoint_info_lcl->loc_clear_threshold
              || endpoint_info_lcl->remote_state || endpoint_info_lcl->remote_diag || (endpoint_info_lcl->local_state && (!_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl))) || endpoint_info_lcl->local_min_echo || endpoint_info_lcl->local_min_tx || endpoint_info_lcl->local_detect_mult
              || endpoint_info_lcl->dst_ip_addr || endpoint_info_lcl->bfd_period || endpoint_info_lcl->remote_detect_mult) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: TX fields may not be set for non accelerated endpoint.")));
        }

        if (endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: tx_gport may not be set for non accelerated endpoints.")));

        }

        if ((!_BCM_BFD_IS_BFD_O_MPLS_SPECIAL_TTL(endpoint_info_lcl->type)) &&
            (!_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type)) &&
            endpoint_info_lcl->local_discr) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: local discriminator is not used in this endpoint but a value was given.")));
        }

        if (endpoint_info_lcl->bfd_detection_time || (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: timeout is not supported on non accelerated endpoints.")));
        }
    } else {
        if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info_lcl->egress_if)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported egress_if.")));
        }

        if (endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID &&  !BCM_GPORT_IS_SYSTEM_PORT(endpoint_info_lcl->tx_gport) &&
            !BCM_GPORT_IS_MODPORT(endpoint_info_lcl->tx_gport) &&  !BCM_GPORT_IS_TRUNK(endpoint_info_lcl->tx_gport)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: tx-gport must be either invalid or a system port.")));
        }
        if (endpoint_info_lcl->int_pri >0x1f) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: int_pri field should be {traffic class[2:0], drop precedence[0:1]}")));
        }
        if (SOC_IS_ARAD_B1_AND_BELOW(unit) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_flags_manual_mgmt", 0)) {
            if(endpoint_info_lcl->remote_flags != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG("Error: remote_flags must be 0.")));
            }
            if ( endpoint_info_lcl->local_flags& ~0x3f ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: local_flags 6 bit fields.")));
            }
        } else {
            rv = bcm_bfd_flag_combination_is_not_supported(unit, endpoint_info_lcl->remote_flags, &remote_flags_not_supported);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = bcm_bfd_flag_combination_is_not_supported(unit, endpoint_info_lcl->local_flags, &local_flags_not_supported);
            BCMDNX_IF_ERR_EXIT(rv);
            if (remote_flags_not_supported || local_flags_not_supported) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Only the flags 0x%x are supported. Given local_flags: %d, remote_flags: %d"),
                     SOC_DPP_CONFIG(unit)->pp.bfd_supported_flags_bitfield,
                     endpoint_info_lcl->local_flags , endpoint_info_lcl->remote_flags ));
            }
        }
        if (endpoint_info_lcl->local_diag > 16) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: local_diag field must be between 0 and 16.")));
        }
        if (endpoint_info_lcl->remote_diag > 16) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: remote_diag field must be between 0 and 16.")));
        }
        if (endpoint_info_lcl->loc_clear_threshold > 3) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: loc_clear_threshold may not be greater than 3")));
        }

        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME) {
            if (endpoint_info_lcl->bfd_detection_time > _BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME_USEC_MAX) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: bfd_detection_time is too big")));
            }
            if (endpoint_info_lcl->remote_detect_mult > 0 && SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop!=2) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: remote_detect_mult isn't used since detection time is set explicitly")));
            }
        }
        else {
            if (endpoint_info_lcl->local_min_rx * endpoint_info_lcl->remote_detect_mult > _BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME_USEC_MAX) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: local_min_rx * remote_detect_mult is too big")));
            }
            if (endpoint_info_lcl->bfd_detection_time > 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: bfd_detection_time isn't used when the flag BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME is not set.")));
            }
        }

        /* BFD period is a 10bit field. In Arad the units are 3.33 ms. In Jericho - 1.67 */
        bfd_period_max = SOC_IS_ARADPLUS_AND_BELOW(unit) ? 3406 /* floor(3.33*0x3ff) */ : 1703 /* floor(1.67*0x3ff) */ ;

        if (endpoint_info_lcl->bfd_period > bfd_period_max ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: bfd rate may not be greater than %d"),bfd_period_max));
        } else if (endpoint_info_lcl->bfd_period <4 && endpoint_info_lcl->bfd_period!=0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                 (_BSL_BCM_MSG("Error: bfd_period 1 - 3 unavailable.")));
        }

        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) {
            if ((endpoint_info_lcl->sampling_ratio < 0) || (endpoint_info_lcl->sampling_ratio > 8)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                               (_BSL_BCM_MSG("Error: Sampling ratio can only be 0-8.\n")));
            }
        }
    }

    /* Micro BFD checks*/
    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MICRO_BFD) != 0) {
        if (endpoint_info_lcl->type != bcmBFDTunnelTypeUdp) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: For micro BFD only tunnel type bcmBFDTunnelTypeUdp is defined.")));
        }
        if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) != 0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_MULTIHOP flag may not be defined with BCM_BFD_ENDPOINT_MICRO_BFD.")));
        }
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_MICRO_BFD only supported on Jericho and above.")));
        }
        if (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support==SOC_DPP_ARAD_MICRO_BFD_SUPPORT_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_MICRO_BFD flag may only be set with the soc property micro_bfd_support.")));
        }
    }

    if ( (endpoint_info_lcl->type != bcmBFDTunnelTypeUdp && endpoint_info_lcl->type != bcmBFDTunnelTypeMpls && endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCcCv) && (endpoint_info_lcl->ip_ttl || endpoint_info_lcl->ip_tos)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: IP fields should only be set only for BFDoIP, BFDoIPoMPLS.")));

    }

    if ((!_BCM_BFD_IS_BFD_O_MPLS_SPECIAL_TTL(endpoint_info_lcl->type)) &&
        _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type) &&
        (endpoint_info_lcl->gport != BCM_GPORT_INVALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: For BFD endpoints of type UDP gport field must be set to BCM_GPORT_INVALID")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (SOC_DPP_CONFIG(unit)->pp.bfd_mpls_lsp_support==0 && SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop==0) {
            if (endpoint_info_lcl->type ==bcmBFDTunnelTypeMpls ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: BFDoIPoMPLS not supported without suitable soc property.")));
            }
        }
    }

    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_UPDATE) {
        if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_REPLACE flag can be used only with BCM_BFD_ENDPOINT_WITH_ID specification.\n")));
        }

        if (is_default) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: Can not replace default mep.\n")));
        }
    } else {
        if (is_default) {
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                if (endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG("Error: BFD Default trap - remote_gport must be specified")));
                }
            } else {
                /* Check that this endpoint ID isn't occupied */
                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry_temp, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                        (_BSL_BCM_MSG("Error: Default endpoint already exists.\n")));
                }
            }
        } else { /* Non default */
            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint_info_lcl->id, &rmep_list_p, &found);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Id should be equal to lowest 13 bits of your discriminator in case packet is accelerating to OAMP */
                if (*is_endpoint_acc_to_oamp) {
                    if (SOC_IS_QUX(unit) && (endpoint_info_lcl->local_discr & 0x300)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("The bits 8 and 9 has been reserved in local discriminator")));
                    }

                    if (endpoint_info_lcl->id != _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: Error in id %d: id should be equal to lowest %d bits of local_discr"), endpoint_info_lcl->id, SOC_DPP_DEFS_GET(unit,oam_2_id_nof_bits)));
                    }
                }
            }

            if (*is_endpoint_acc_to_oamp &&
                ((endpoint_info_lcl->type == bcmBFDTunnelTypeMpls) || (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv) ||(endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweControlWord) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweRouterAlert) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweGal))) {
                if ((endpoint_info_lcl->type == bcmBFDTunnelTypeMpls)|| (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv)) {
                    if (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl) &&
                        ((endpoint_info_lcl->udp_src_port < _BCM_BFD_UDP_SRC_PORT_MIN) || 
                         (endpoint_info_lcl->udp_src_port > _BCM_BFD_UDP_SRC_PORT_MAX))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("Error: udp_src_port value should be between %d and %d. Given value: %d"),
                                          _BCM_BFD_UDP_SRC_PORT_MIN, _BCM_BFD_UDP_SRC_PORT_MAX, endpoint_info_lcl->udp_src_port));
                    }

                    if ((endpoint_info_lcl->ip_ttl != 1) && (endpoint_info_lcl->type != bcmBFDTunnelTypeMplsTpCcCv)) {
                        /*TTL&TOS*/
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: For BFD over MPLS supporting only ttl 1. Given value: %d"), endpoint_info_lcl->ip_ttl));
                    }

                    if ((endpoint_info_lcl->ip_ttl != 255) && (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: For VCCV type BFD supporting only ttl 255. Given value: %d"), endpoint_info_lcl->ip_ttl));
                    }

                    if (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl)) {
                        soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_get(unit, &udp_sport);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (udp_sport && udp_sport != endpoint_info_lcl->udp_src_port) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Supporting single MPLS udp_src_port value for device. Existing value: %d"), udp_sport));
                        }
                    }
                } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) {
                    if (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl) &&
                        ((endpoint_info_lcl->udp_src_port < _BCM_BFD_UDP_SRC_PORT_MIN) || 
                         (endpoint_info_lcl->udp_src_port > _BCM_BFD_UDP_SRC_PORT_MAX))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: udp_src_port value should be between %d and %d. Given value: %d"),
                                             _BCM_BFD_UDP_SRC_PORT_MIN, _BCM_BFD_UDP_SRC_PORT_MAX, endpoint_info_lcl->udp_src_port));
                    }

                    if (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl)) {
                        soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_sport);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                        if (udp_sport && udp_sport != endpoint_info_lcl->udp_src_port) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Supporting single IPv6 udp_src_port value for device. Existing value: %d"), udp_sport));
                        }
                    }

                    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) == 0) {
                        int ref_counter_tos = 0;
                        if ((endpoint_info_lcl->ip_ttl != 255)) {
                            /*TTL&TOS*/
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG("Error: For IP One Hop supporting only ttl 255. Given value: %d"), endpoint_info_lcl->ip_ttl));
                        }
                        rv = BFD_ACCESS.s_hop.ref_counter.get(unit, &ref_counter_tos);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (ref_counter_tos != 0) {
                            int tos = 0;
                            rv = BFD_ACCESS.s_hop.tos_value.get(unit, &tos);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (!_BCM_BFD_IPv4_FLEX_IPTOS_FEATURE_SET(unit)) {
                                if (tos != (SOC_IS_JERICHO(unit) ? 255 : endpoint_info_lcl->ip_tos)) {
                                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                            (_BSL_BCM_MSG("Error: Supporting single IP TOS value per device. Existing value: %d")
                                             , tos));
                                }
                            }
                        }
                        if (!is_1_hop_extended) {
                            if (endpoint_info_lcl->dst_ip_addr && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: For IP One Hop: dest ip address field should remain blank")));
                            }
                            if (SOC_IS_JERICHO(unit) &&  endpoint_info_lcl->ip_subnet_length > 32) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: IP subnet length must be <=32")));
                            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit) &&  endpoint_info_lcl->ip_subnet_length) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: IP subnet length unsopported.")));
                            }
                        }
                    }
                }
            }

            /* Determining lif - Only for later check so oem_key can be deduced */
            SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
            rv = _bcm_bfd_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry, dip_ndx);
            BCMDNX_IF_ERR_EXIT(rv);

            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) {
                if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Device does not support remote OAMP")));
                }
                if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: In case of Server configuration BCM_BFD_ENDPOINT_IN_HW must be set.")));
                }
                /* Server: configuring only OAMP - add remote_gport trap code to OAMP traps list */
                if (!BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: In case of Server configuration remote_gport must be valid trap code.")));
                }

                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                    rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
                    if (rv == BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                            (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
                    } else if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else {
                    if (SOC_IS_QUX(unit) && (endpoint_info_lcl->local_discr & 0x300)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG("The bits 8 and 9 has been reserved in local discriminator")));
                    }

                    /* mep id callculated from the local_discr field */
                    endpoint_info_lcl->id = _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK);

                    /* Checking same id does not collide with existig endpoints */
                    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_temp);
                    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry_temp, &found);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (found) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Local endpoint with id %d exists."), endpoint_info_lcl->id));
                    }
                }
            } else {
                /* Endpoint ID validity check */
                if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW)) {
                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unaccelerated endpoint can not be created WITH_ID")));
                    }

                    /* Validate that mep with same lif does not exist */
                    oem_key = (classifier_mep_entry.lif==_BCM_OAM_INVALID_LIF) ? classifier_mep_entry.your_discriminator : classifier_mep_entry.lif;
                    soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
                    unit, oem_key, 0/*mdlevel*/, 0/*is_upmep*/, &found, &profile_temp, &found_profile, &is_mp_type_flexible_dummy, &is_mip_dummy);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (found) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                            (_BSL_BCM_MSG("Error: Local endpoint with lif/local_discr %d exists.\n"), oem_key));
                    }
                }

                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                        rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
                        if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_EXISTS)) {
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    } else {
                        if (*is_endpoint_acc_to_oamp) {
                            if (SOC_IS_QUX(unit) && (endpoint_info_lcl->local_discr & 0x300)) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG("The bits 8 and 9 has been reserved in local discriminator")));
                            }
                            endpoint_info_lcl->id = _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK);

                            /* Checking same id does not collide with existig endpoints */
                            SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_temp);
                            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry_temp, &found);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (found) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Local endpoint with id %d exists."), endpoint_info_lcl->id));
                            }
                        }
                    }
                }

                if (!(endpoint_info_lcl->flags & BCM_BFD_ECHO) && !is_1_hop_extended) {
                    if (!BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport) && endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: remote_gport must be a trap code or invalid.")));
                    }
                }
            }

            if (*is_endpoint_acc_to_oamp) {
                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) {
                    if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info_lcl->remote_id)) {
                        _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->remote_id, rmep_index);
                    }
                    else {
                        rmep_index = endpoint_info_lcl->remote_id;
                    }

                    rv = bcm_dpp_am_oam_rmep_id_is_alloced(unit, rmep_index);
                    if (rv == BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                 (_BSL_BCM_MSG("Error: Remote endpoint with id %d exists.\n"), endpoint_info_lcl->remote_id));
                    }
                    else if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }


            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validity checks for BFD endpoint updating */
int _bcm_bfd_endpoint_replace_validity_checks(int unit, bcm_bfd_endpoint_info_t *endpoint_info_lcl,
                                              bcm_bfd_endpoint_info_t *existing_endpoint_info) {

    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int new_lif_id, existing_lif_id;
    uint32 range;
    uint32 disc_msb = 0;
    int single_hop_advance_a_plus=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(endpoint_info_lcl);
    BCMDNX_NULL_CHECK(existing_endpoint_info);

    if ((_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl) && 
        !(_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, existing_endpoint_info))) ||
        (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl) && 
        !(_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, existing_endpoint_info)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Seamless BFD role can not be updated.\n")));
    }

    if (endpoint_info_lcl->gport != BCM_GPORT_INVALID || existing_endpoint_info->gport != BCM_GPORT_INVALID) {

        rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info_lcl->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS|_BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        new_lif_id = gport_hw_resources.global_in_lif;

        rv = _bcm_dpp_gport_to_hw_resources(unit, existing_endpoint_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS|_BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        existing_lif_id = gport_hw_resources.global_in_lif;

        if (new_lif_id != existing_lif_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: gport may not be updated.\n") ));
        }
    }

    if (endpoint_info_lcl->type != existing_endpoint_info->type) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: type can not be updated.\n")));
    }

    if (endpoint_info_lcl->remote_id != existing_endpoint_info->remote_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: remote_id can not be updated.\n")));
    }

    if (((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) != 0) != ((existing_endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Acceleration in HW may not be added / removed.\n")));
    }

    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) || _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type)) { /* check discriminator */
        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if ((endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK ) != (existing_endpoint_info->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: LSBs of Local discriminator may not be updated.\n")));
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
            /* Set the MSB always */
            disc_msb = (1<< ((31-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START))) | (endpoint_info_lcl->local_discr >> SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
        } else {
            disc_msb = endpoint_info_lcl->local_discr >> SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;
        }
        if (range != disc_msb) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: MSBs of Local discriminator may not be updated.\n")));
        }
    }

    if (!_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl) &&
       ((endpoint_info_lcl->udp_src_port) != (existing_endpoint_info->udp_src_port))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: udp_src_port may not be updated.\n")));
    }

    if ((endpoint_info_lcl->src_ip_addr) != (existing_endpoint_info->src_ip_addr)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: src_ip_addr can not be updated.\n")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && SOC_IS_ARADPLUS_A0(unit)
            && endpoint_info_lcl->type == bcmBFDTunnelTypeUdp && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) ) {
        single_hop_advance_a_plus = 1;
    }

    /* Do not allow TOS profile updation on ARAD+ single hop since only TOS profile globally */
    if(single_hop_advance_a_plus && ((endpoint_info_lcl->ip_tos) != (existing_endpoint_info->ip_tos))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: ip_tos can not be updated. Supporting single IP TOS value per device.\n"))); 
    }

    if ((endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) != (existing_endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP))  {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Multihop configuration mismatch.")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_endpoint_create(int unit, bcm_bfd_endpoint_info_t *endpoint_info)
{
    uint32 rmep_index = (uint32)(-1);
    uint32 rmep_index_internal;
    uint32 soc_sand_rv = 0;
    bcm_error_t rv = BCM_E_NONE;
    int flags = 0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 found;
    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    uint8 new_rmep_id_alloced = FALSE;
    uint8 new_mep_id_alloced = FALSE;
    uint32 new_disc_range, range;
    SOC_PPC_OAM_MA_NAME name = { 0 };
    int is_allocated;
    int dip_ndx = 0;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    uint32 trap_code;
    uint8 trap_strength = 0;
    int profile;
    bcm_bfd_endpoint_info_t existing_endpoint_info;
    uint16 udp_sport;
    uint32 mpls_udp_sport_ref_counter;
    uint32 ipv4_udp_sport_ref_counter;
    int your_disc_ref_counter; /*used for static registers - bfd_pdu and bfd_cc_packet*/
    uint32 unsigned_ref_counter = 0;
    uint8 is_bfd_init;
    uint8 internal_bfd_opcode;
    uint32 server_trap_ref_counter;
    uint8 is_default;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint8 is_1_hop_extended = 0;
    bcm_bfd_endpoint_info_t endpoint_info_lcl_t;
    bcm_bfd_endpoint_info_t *endpoint_info_lcl;
    uint8 is_endpoint_acc_to_oamp = 0;
	uint8 is_endpoint_bfd_server = 0;
    uint32 snp_strength;
    uint8  fwd_strength = 0;
    uint32 is_m_hop_extended = 0;
    uint64 prg_var_64;
    uint32 prg_var_high;
    uint32 prg_var_low;
    uint8 no_of_sbfd_reflector = 0;
    uint8 is_mpls_php = 0;    /* 1 for bcmBFDTunnelTypeMplsPhp*/
    BCMDNX_INIT_FUNC_DEFS;


    endpoint_info_lcl = &endpoint_info_lcl_t; /* If the GPORT contains a trap, since the trap id inside the GPORT can be either the HW id or the SW_id ( according to the use_hw_id SOC peroperty) , and this function 
    assumes that the trap id is the SW id, it will use a copy of the endpoint_info with the SW id */
    sal_memcpy(endpoint_info_lcl, endpoint_info, sizeof(bcm_bfd_endpoint_info_t));  /* from here till the end of the function, the function uses the copy of the endpoint */

    _BCM_BFD_ENABLED_GET(is_bfd_init);
    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info_lcl->remote_gport);
    }

    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IPV6) {
            rv = _bcm_bfd_ipv6_endpoint_create(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_EXIT;           
        }
    }


    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id);

    /* 
     * For SBFD initiator endpoints, do not assign is_m_hop_extended to 1
     * even if the soc property is set. Because we want to use OEM classifier
     * for classifying SBFD initiator endpoints. M_HOP extended uses LEM.
     */
    if (!_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
        if (((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) || (endpoint_info_lcl->type == bcmBFDTunnelTypeMpls)) && (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) && (is_default==0)) {
            is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
        }
    }

    /* Transmission should be identical to BFD over MPLS LSP ,hence treat as bcmBFDTunnelTypeMpls*/
    if(endpoint_info_lcl->type == bcmBFDTunnelTypeMplsPhp) {
        is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
        if (!(SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) || is_m_hop_extended) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("Error: For tunnel type  bcmBFDTunnelTypeMplsPhp: soc property bfd_ipv4_single_hop_extended must be set and multi_hop_bfd_extra_dip_support must not be set")));
        }
        is_mpls_php = 1;
        is_1_hop_extended = 1;
        endpoint_info_lcl->type = bcmBFDTunnelTypeMpls;
    }

    /*initial input check*/
    rv = _bcm_bfd_endpoint_validity_checks(unit, endpoint_info_lcl, &is_endpoint_acc_to_oamp);
    BCMDNX_IF_ERR_EXIT(rv);


    /* 
     * check if we are in bfd ipv4 single hop extended mode. 
     * Do not care about single hop extended mode if it is a
     * SBFD initiator endpoint, because we want to use OEM classifier
     * for classification and not LEM
     */
    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && (!_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl))) {
        if ((endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) && !(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP) && (is_default==0)) {
            /* includes micro bfd.*/
            is_1_hop_extended = 1;
        }
    }

    /* get the internal bfd opcode */
    /* we use CCM opcode to signal BFD packets. This is not used in HW and isn't influence by opcode to internal opcode mapping*/
    rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM, &internal_bfd_opcode);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Endpoint replace */
    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_UPDATE) {
        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY new_classifier_entry;
        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_entry);

        bcm_bfd_endpoint_info_t_init(&existing_endpoint_info);
        rv = bcm_petra_bfd_endpoint_get(unit, endpoint_info_lcl->id, &existing_endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
        if (BCM_GPORT_IS_TRAP(existing_endpoint_info.remote_gport)) {
            _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, existing_endpoint_info.remote_gport);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Copy information from old data base. Whatever needs updating will be updated.*/
        new_classifier_entry = classifier_mep_entry;

        /* Was updated above to go thru endpoint validity checks */
        if(is_mpls_php) {
            endpoint_info_lcl->type = bcmBFDTunnelTypeMplsPhp;
        }
        /* Validate the replacement information */
        rv = _bcm_bfd_endpoint_replace_validity_checks(unit, endpoint_info_lcl, &existing_endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if(is_mpls_php) {
            endpoint_info_lcl->type = bcmBFDTunnelTypeMpls;
        }
        rv = _bcm_bfd_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &new_classifier_entry, classifier_mep_entry.dip_profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_endpoint_acc_to_oamp) {
            /* Accelerated - Inserting entries to OAMP dbs */
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY new_mep_db_entry;
            _bcm_bfd_mep_db_entry_pointers_to_free pointers_to_free = { 0 }; /* Initialize to 0: free nothing*/

            /*Setting the static registers.*/
            if (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc) {
                rv = BFD_ACCESS.mpls_tp_cc_ref_counter.get(unit, &unsigned_ref_counter);
            } else {
                rv = BFD_ACCESS.pdu_ref_counter.get(unit, &unsigned_ref_counter);
            }
            BCMDNX_IF_ERR_EXIT(rv);

            if (( SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 0 ||
                  !(endpoint_info_lcl->type == bcmBFDTunnelTypeUdp && (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) ||
                  (endpoint_info->local_min_echo != 0))) {
                /* if the bfd_echo_min_interval is enable we dont need to do update*/
                rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, unsigned_ref_counter, (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc), 1);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /*
            * MEP DB entry
            */
            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "BFD endpoint update: TX\n")));
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info_lcl->id, &mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* Atomic update of the MEP DB.*/
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&new_mep_db_entry);
            new_mep_db_entry = mep_db_entry; /* start by setting the new entry to the original entry. Whatever needs updating will be updated.*/

            /* First, translate from endpoint_info to mep_db_entry, excluding pointers*/
            soc_sand_rv = _bcm_bfd_enpoint_info_to_mep_db_entry(unit, endpoint_info_lcl, &new_mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* Next, update the pointers*/
            rv = _bcm_bfd_mep_db_entry_struct_set_pointers(unit, endpoint_info_lcl, &existing_endpoint_info, &mep_db_entry, &new_mep_db_entry, &pointers_to_free);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Now set the MEP DB entry*/
            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &new_mep_db_entry, 0, name, 0/*not in use in bfd case*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /*Finally, free pointers that should be freed*/
            rv = _bcm_mep_db_entry_dealloc2(unit, endpoint_info_lcl->id, &mep_db_entry, &pointers_to_free);
            BCMDNX_IF_ERR_EXIT(rv);

            /*           
            * RMEP DB entry
            */
            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "Updating remote entry.\n")));

            SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);

            rv = _bcm_bfd_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Update entry in rmep db and rmep index db*/
            _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, endpoint_info_lcl->remote_id); 
            /* First, free the resources*/
            rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_index_internal);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_index_internal, 0, endpoint_info_lcl->id, mep_db_entry.mep_type, &rmep_db_entry, 1);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* Only update required min Rx and local_state*/
        if (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)) {
            prg_var_low = endpoint_info_lcl->local_min_rx;
            prg_var_high = endpoint_info_lcl->src_ip_addr;
            COMPILER_64_SET(prg_var_64, prg_var_high, prg_var_low);
            soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_SBFD_REFLECTOR, prg_var_64);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = arad_egr_prog_editor_sbfd_reflector_status_update(unit, endpoint_info_lcl->local_state);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.reflector_status.set(unit, endpoint_info_lcl->local_state));
        }
        /* 
         * Update in UDP src port for SBFD Initiator endpoint should
         * be allowed only if no_of_sbfd_endpoints is 1 meaning there
         * is only one person using it and hence it can be changed.
         * Else, throw BCM_E_PARAM.
         */
        if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
            uint32 udp_src_port = 0;
            uint32 num_sbfd_endpoints = 0;
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_seamless_bfd_rx_dst_port_get,(unit, &udp_src_port)); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            _BCM_BFD_NOF_SBFD_ENDPOINTS_GET(unit, num_sbfd_endpoints);
            if (udp_src_port != endpoint_info_lcl->udp_src_port && 
                    num_sbfd_endpoints != 1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                        (_BSL_BCM_MSG("Error: SBFD initiator UDP src port cannot be updated\n")));
            } else if (udp_src_port != endpoint_info_lcl->udp_src_port) {
                if (is_endpoint_acc_to_oamp) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_pe_seamless_bfd_src_port_set, (unit, endpoint_info_lcl->udp_src_port)); 
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_seamless_bfd_rx_dst_port_set, (unit, _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR, endpoint_info_lcl->udp_src_port));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }

        if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
            SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;

            /* Use destination set to a pre defined trap code*/
            rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                       endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
            BCMDNX_IF_ERR_EXIT(rv);

            if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
            }

            BCM_SAND_IF_ERR_EXIT(
                    soc_ppd_trap_snoop_profile_info_get(unit,
                        trap_code,
                        &snoop_profile_info)
                    );
            snp_strength = (snoop_profile_info.snoop_cmnd > 0) ?
                _BCM_OAM_BFD_DEFAULT_SNP_STRENGTH_GET(unit) : 0;
            fwd_strength = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
        }
        else {
            trap_code = 0;
            snp_strength = 0;
            fwd_strength = 0;
        }

        if ((is_1_hop_extended || is_m_hop_extended) && endpoint_info_lcl->remote_detect_mult != existing_endpoint_info.remote_detect_mult && ((endpoint_info_lcl->flags & BCM_BFD_ECHO)==0)) {
            /* Update the LEM entry: change the payload.*/
            ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO lem_info;
            ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO_clear(&lem_info);
            rv = _bcm_bfd_endpoint_info_to_lem_info(unit,endpoint_info_lcl, &lem_info, trap_code, fwd_strength, snp_strength );
            BCMDNX_IF_ERR_EXIT(rv);
            lem_info.is_update = 1;
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_add, (unit, &lem_info));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* inserting an entry to the LEM for BFD echo*/
        if ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1) && (endpoint_info_lcl->flags & BCM_BFD_ECHO)) {
            rv =  _bcm_bfd_echo_lem_entry_add(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /*update sw db mep_info with new mep */
        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info_lcl->id, &new_classifier_entry);
        BCMDNX_IF_ERR_EXIT(rv);

    } /* END OF UPDATE */
    else {
        if (is_default) {
            /* Default endpoint */
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
                SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);

                classifier_mep_entry.non_acc_profile = SOC_PPC_OAM_PROFILE_DEFAULT;
                classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;

                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport;

                rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                           endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
                BCMDNX_IF_ERR_EXIT(rv);

                if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                    trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
                }

                profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */
                profile_data.mep_profile_data.opcode_to_trap_strength_multicast_map[internal_bfd_opcode] = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
                profile_data.is_default = 1;

                profile = 0;

                rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, BCM_DPP_AM_TEMPLATE_FLAG_ALLOC_WITH_ID, &profile_data, &is_allocated, &profile);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Default BFD is mapped to endpoint -2 to avoid colision with oam default endpoint */
                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, -2, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else { /* Arad+ and above default endpoint create */
                rv = _bcm_bfd_default_id_from_ep_id(unit, endpoint_info_lcl->id, &default_id);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Init classifier entry */
                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
                classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
                classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_COUNT; /* For diag callback only! This value is not used elsewhere */
                classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport;
                if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                    /* Get trap code */
                    rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                               endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                        trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
                    }
                }
                else {
                    rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_CPU, &trap_code);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* Allocate profile */
                SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);
                profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */
                profile_data.mep_profile_data.opcode_to_trap_strength_multicast_map[internal_bfd_opcode] = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;

                profile = 0;

                rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, &profile_data, &is_allocated, &profile);
                BCMDNX_IF_ERR_EXIT(rv);

                classifier_mep_entry.non_acc_profile = (uint8)profile;

                /* Load to HW */
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&classifier_mep_entry, 0/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &classifier_mep_entry, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /*update sw db mep_info with new mep */
                rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        else { /* Non-default new endpoint create */
            /* EP 0(dummy ep) in Jericho_PLUS and above used for trapping your-discriminator = '0' case
             * and should not be accounted in ranges
             */
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;

            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

            /* First, translate from endpoint_info to mep_db_entry, excluding pointers*/
            rv =  _bcm_bfd_enpoint_info_to_mep_db_entry(unit, endpoint_info_lcl,&mep_db_entry);
            BCMDNX_IF_ERR_EXIT(rv);
			 
            if(!((endpoint_info_lcl->local_discr == 0) && SOC_IS_JERICHO_PLUS(unit))) {
                /* If using YourDiscriminator, id should be taken from your discriminator LSBs and MSBs define a range that is checked in the identification */
                if (is_endpoint_acc_to_oamp ||
                        (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type))) {
                    /* This is the MSB of My Disc, that will be added to 16 bit MEP_DB_Index to get My discriminator on sent packets */
                    new_disc_range = endpoint_info_lcl->local_discr >> SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START;
                    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_discriminator_type_update", 0)) {
                        new_disc_range |= (1 << (31-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START));
                    }
                    rv = BFD_ACCESS.YourDiscriminator_ref_count.get(unit, &your_disc_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (your_disc_ref_counter > 0) {
                        /* Range should have already been set */
                        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        if (!is_endpoint_acc_to_oamp) {
                            /* If creating a non-accelerated endpoint, the relevant range for comparrison has less bits */
                            range          >>= (SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
                            new_disc_range >>= (SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
                        }

                        if (range != new_disc_range) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: BFD Your-Discriminator range should be constant for all endpoints. Existing value: %d (%d)"), range, new_disc_range));
                        }
                    }
                    else {
                        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_set(unit, new_disc_range);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
            if (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)) {
                prg_var_low = endpoint_info_lcl->local_min_rx;
                prg_var_high = endpoint_info_lcl->src_ip_addr;
                COMPILER_64_SET(prg_var_64, prg_var_high, prg_var_low);
                soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_SBFD_REFLECTOR, prg_var_64);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = arad_egr_prog_editor_sbfd_reflector_status_update(unit, endpoint_info_lcl->local_state);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.reflector_status.set(unit, endpoint_info_lcl->local_state));
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_reflector.get(unit, &no_of_sbfd_reflector));
                if(no_of_sbfd_reflector == 0){
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_reflector.set(unit, 1));
                }
            }
            /*
             * During create, set the UDP src port for SBFD endpoint
             * only if this is the first SBFD endpoint. Else it has
             * to be same as already configured value. If it is not, throw
             * BCM_E_PARAM
             */
            if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
                uint32 udp_src_port = 0;
                uint32 num_sbfd_endpoints = 0;
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_seamless_bfd_rx_dst_port_get,(unit, &udp_src_port)); 
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                _BCM_BFD_NOF_SBFD_ENDPOINTS_GET(unit, num_sbfd_endpoints);
                if (num_sbfd_endpoints == 0) {
                    /* First SBFD endpoint */
                    if (is_endpoint_acc_to_oamp) {
                        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_pe_seamless_bfd_src_port_set, (unit, endpoint_info_lcl->udp_src_port)); 
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_seamless_bfd_rx_dst_port_set, (unit, _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR, endpoint_info_lcl->udp_src_port));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else if (udp_src_port != endpoint_info_lcl->udp_src_port) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Only one UDP src port is supported for all SBFD endpoints.\n")));
                }
            }

            if (is_endpoint_acc_to_oamp &&
                ((endpoint_info_lcl->type == bcmBFDTunnelTypeMpls) || (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv) ||(endpoint_info_lcl->type == bcmBFDTunnelTypeUdp) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweControlWord) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweTtl) ||
                 (endpoint_info_lcl->type == bcmBFDTunnelTypePweRouterAlert) || (endpoint_info_lcl->type == bcmBFDTunnelTypePweGal))) {
                rv = BFD_ACCESS.pdu_ref_counter.get(unit, &unsigned_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, unsigned_ref_counter, 0, 0);
                BCMDNX_IF_ERR_EXIT(rv);
                /*Update the counter only when we are sure an endpoint will be created*/
                if (((endpoint_info_lcl->type == bcmBFDTunnelTypeMpls) || (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCcCv))&& (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl))) {
                    soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_get(unit, &udp_sport);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (udp_sport == 0) {
                        if(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP){
                            soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_set(unit, endpoint_info_lcl->udp_src_port);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        else {
                            soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_set(unit, endpoint_info_lcl->udp_src_port);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }
                    rv = BFD_ACCESS.mpls_udp_sport_ref_counter.get(unit, &mpls_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    mpls_udp_sport_ref_counter++;
                    rv = BFD_ACCESS.mpls_udp_sport_ref_counter.set(unit, mpls_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeUdp && (!_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl))) {
                    soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_sport);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (udp_sport == 0) {
                        soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_set(unit, endpoint_info_lcl->udp_src_port);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                    rv = BFD_ACCESS.ipv4_udp_sport_ref_counter.get(unit, &ipv4_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    ipv4_udp_sport_ref_counter++;
                    rv = BFD_ACCESS.ipv4_udp_sport_ref_counter.set(unit, ipv4_udp_sport_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /*preumably after this an endpoint will be created, so we can now update the counter.*/
                if ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 0 ||
                     !(endpoint_info_lcl->type == bcmBFDTunnelTypeUdp && (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) ||
                     (endpoint_info->local_min_echo != 0))) {
                    ++unsigned_ref_counter;
                    rv = BFD_ACCESS.pdu_ref_counter.set(unit, unsigned_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else if (endpoint_info_lcl->type == bcmBFDTunnelTypeMplsTpCc) {
                if (is_endpoint_acc_to_oamp) {
                    rv = BFD_ACCESS.mpls_tp_cc_ref_counter.get(unit, &unsigned_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = _bcm_bfd_manage_static_registers(unit, endpoint_info_lcl, unsigned_ref_counter, 1, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                    unsigned_ref_counter++;
                    rv = BFD_ACCESS.mpls_tp_cc_ref_counter.set(unit, unsigned_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }

            LOG_VERBOSE(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "BFD endpoint create: RX\n")));

            SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
            if(is_mpls_php) {
                classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_MPLS_PHP;
            }
            rv = _bcm_bfd_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry, dip_ndx);
            BCMDNX_IF_ERR_EXIT(rv);
            if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_HW_ACCELERATION_SET) {
                /* Allocating id:
                 *    If it's accelerated to the OAMP id should be taken from your discriminator LSBs
                 *    Else if it's accelerated to user destination it is allocated from the free range or can be set by the user,
                 *    Else, is taken from lif + indication bit that the endpoint is BFD
 */
                flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                if (!(endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID)) {
                    /* mep id callculated from the local_discr field */
                    endpoint_info_lcl->id = _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK);
                }
                rv = _bcm_dpp_bfd_mep_id_alloc(unit, flags, (uint32 *)&endpoint_info_lcl->id);
                BCMDNX_IF_ERR_EXIT(rv);
                new_mep_id_alloced = TRUE;

                rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport), &server_trap_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                server_trap_ref_counter++;
                rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport), server_trap_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);

                if (server_trap_ref_counter == 1) {
                    /* This trap code wasn't allocated yet */
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_set,(unit, mep_db_entry.mep_type, BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport)));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
            else {
                /* Allocating id:
                 *    If it's accelerated to the OAMP id should be taken from your discriminator LSBs
                 *    Else if it's accelerated to user destination it is allocated from the free range or can be set by the user,
                 *    Else, is taken from lif + indication bit that the endpoint is BFD
 */
                flags = 0;
                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_WITH_ID) {
                        /* mep id given by the user */
                        flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                    }
                    else {
                        if (is_endpoint_acc_to_oamp) {
                            endpoint_info_lcl->id = _BCM_BFD_YOUR_DISC_TO_LEGAL_OAMP_ID(endpoint_info_lcl->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK);

                            /* mep id callculated from the local_discr field */
                            flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                        }
                    }

                    rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
                    if (rv == BCM_E_EXISTS) {
                        /** Adding server side classifier to bfd server */
                        is_endpoint_bfd_server = 1;
                    }
                    else {
                         rv = _bcm_dpp_bfd_mep_id_alloc(unit, flags, (uint32*)&endpoint_info_lcl->id);
                         BCMDNX_IF_ERR_EXIT(rv);
                         new_mep_id_alloced = TRUE;
                    }
                }
                else {
                    /* Can not be created WITH_ID - checked already */
                    if (classifier_mep_entry.lif==_BCM_OAM_INVALID_LIF) {
                        endpoint_info_lcl->id = _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.your_discriminator);
                    }
                    else {
                        endpoint_info_lcl->id = _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif);
                    }
                }

                /*in case of BFD echo or bfd ipv4 single hop extended mode we don't use The OAM classification the PMF does it*/
                if (!(endpoint_info_lcl->flags & BCM_BFD_ECHO) && !is_1_hop_extended && !is_m_hop_extended) {
                    SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);

                    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
                        /* Use destination set to a pre defined trap code*/
                        rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                                   endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
                        BCMDNX_IF_ERR_EXIT(rv);
                        LOG_VERBOSE(BSL_LS_BCM_BFD,
                                    (BSL_META_U(unit,
                                                "Creating BFD endpoint with user defined trap code %d \n"), trap_code));

                        if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                            trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
                        }
                    } else if (endpoint_info_lcl->remote_gport == BCM_GPORT_INVALID) {
                        /* use default destinations*/
                        int trap_code_index;
                        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                            trap_code_index = (classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP ||
                                               classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ? SOC_PPC_BFD_TRAP_ID_OAMP_IPV4 :
                                (classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) ? SOC_PPC_BFD_TRAP_ID_OAMP_MPLS :
                                (classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE) ? SOC_PPC_BFD_TRAP_ID_OAMP_PWE :
                                (classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL) ? SOC_PPC_BFD_TRAP_ID_OAMP_PWE :
                                /* Only conceivable option left (otherwise an error would have already been printed)*/ SOC_PPC_BFD_TRAP_ID_OAMP_CC_MPLS_TP;
                                if (SOC_IS_ARADPLUS(unit) && (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP)) {
                                    trap_code_index = SOC_PPC_BFD_TRAP_ID_OAMP_PWE;
                                }
                        } else {
                            /* default destination: CPU port.*/
                            trap_code_index = SOC_PPC_BFD_TRAP_ID_CPU;
                        }

                        rv = BFD_ACCESS.trap_info.trap_ids.get(unit, trap_code_index, &trap_code);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    if(_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)){
                        /*  update trap code in classifier entry */
                        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.reflector_trap_id.get(unit, &trap_code));
                    }
                    /* Trap_code is gotten above. Insert into mep_profile_data*/
                    profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code; /* No unicast in BFD */
                    profile_data.mep_profile_data.opcode_to_trap_strength_multicast_map[internal_bfd_opcode] = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;

                    if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                        rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);

                        classifier_mep_entry.acc_profile = profile;
                    }
                    else {
                        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);

                        classifier_mep_entry.non_acc_profile = (uint8)profile;
                    }
                    if (is_allocated) {
                        if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_IN_HW) {
                            rv = arad_pp_oam_classifier_oam1_entries_insert_bfd_acccelerated_to_profile(unit, &classifier_mep_entry, &profile_data);
                        } else {
                            rv = arad_pp_oam_classifier_oam1_entries_insert_bfd_according_to_profile(unit, &classifier_mep_entry, &profile_data);
                        }
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    if(!_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info_lcl)){
                        soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &classifier_mep_entry, 0/*update*/, 0);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    if ((_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info_lcl)) || (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
                        /* IP multi hop - set the DIP table.*/
                        /* BFD DIP table compares against packet.DIP, which should equal the endpoint's SIP.*/
                        /* BFD DIP table stores IPv6 addresses, which may be IPv4 addresses. convert IPv4 to default IPv6 extension.*/
                        SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;

                        _BCM_BFD_IPV4_ADDR_TO_DEFAULT_IPV6_EXTENSION_ADDR(endpoint_info_lcl->src_ip_addr,ipv6_addr);
                        rv = _bcm_dpp_am_template_bfd_ip_dip_alloc(unit, 0 /*f  lags*/, &ipv6_addr, &is_allocated, &dip_ndx);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (is_allocated) {
                            soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, dip_ndx, &ipv6_addr);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                        classifier_mep_entry.dip_profile_ndx = dip_ndx;
                    }

                    /* RX end */
                }
            }

            /*if bfd ipv4 single hop extended we use the LEM instead of the classifier*/
            if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_profile_info;

                /* Use destination set to a pre defined trap code*/
                rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                           endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
                BCMDNX_IF_ERR_EXIT(rv);

                if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                    trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
                }

                BCM_SAND_IF_ERR_EXIT(
                        soc_ppd_trap_snoop_profile_info_get(unit,
                            trap_code,
                            &snoop_profile_info)
                        );
                snp_strength = (snoop_profile_info.snoop_cmnd > 0) ?
                    _BCM_OAM_BFD_DEFAULT_SNP_STRENGTH_GET(unit) : 0;
                fwd_strength = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
            }
            else {
                trap_code = 0;
                snp_strength = 0;
                fwd_strength = 0;
            }

            if ((is_1_hop_extended || is_m_hop_extended) && (endpoint_info_lcl->flags & BCM_BFD_ECHO)==0) {
                ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO lem_info;
                ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO_clear(&lem_info);
                if (is_mpls_php) {
                    lem_info.is_mpls_php  = 1;
                }
                rv = _bcm_bfd_endpoint_info_to_lem_info(unit,endpoint_info_lcl, &lem_info, trap_code, fwd_strength, snp_strength );
                BCMDNX_IF_ERR_EXIT(rv);
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_add, (unit, &lem_info));
                BCMDNX_IF_ERR_EXIT(rv);
                classifier_mep_entry.ma_index = endpoint_info_lcl->local_discr; /*in single hop the ma_index will save the local discriminator. 
                                                                                  This will be used for deleting the entry from the LEM*/
            }

            /* inserting an entry to the LEM for BFD echo*/
            if ((SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1) && (endpoint_info_lcl->flags & BCM_BFD_ECHO)) {
                rv =  _bcm_bfd_echo_lem_entry_add(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            if ((is_endpoint_acc_to_oamp) && (is_endpoint_bfd_server == 0)){
                /* Accelerated to OAMP - Inserting entries to OAMP dbs */
                _bcm_bfd_mep_db_entry_pointers_to_free pointers_to_free_dummy = {0}; /* Initialize to 0: free nothing*/           

                /*
                * MEP DB entry
                */
                LOG_VERBOSE(BSL_LS_BCM_BFD,
                                        (BSL_META_U(unit,
                                                    "BFD endpoint create: TX\n")));

                /* inserting an entry to the lem- the key is the my_discriminator the payload is the counter ID*/
                if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
                    rv =  _bcm_bfd_stat_lem_entry_add(unit, endpoint_info_lcl);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* Next, update the pointers*/
                rv = _bcm_bfd_mep_db_entry_struct_set_pointers(unit,endpoint_info_lcl,NULL,NULL,&mep_db_entry, &pointers_to_free_dummy);
                BCMDNX_IF_ERR_EXIT(rv);

                /*set entry in mep db*/
                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, name , 1/*not in use in bfd case*/);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /*
                * RMEP DB entry
                */

                LOG_VERBOSE(BSL_LS_BCM_BFD,
                                        (BSL_META_U(unit,
                                                    "Inserting remote entry.\n")));

                SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
                rv = _bcm_bfd_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
                BCMDNX_IF_ERR_EXIT(rv);

                flags = 0;
                if (endpoint_info_lcl->flags & BCM_BFD_ENDPOINT_REMOTE_WITH_ID) {
                    if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info_lcl->remote_id)) {
                        _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->remote_id, rmep_index);
                    }
                    else {
                        rmep_index = endpoint_info_lcl->remote_id;
                    }

                    /* mep id given by the user */
                    flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                }

                rv = bcm_dpp_am_oam_rmep_id_alloc(unit, flags, &rmep_index);
                BCMDNX_IF_ERR_EXIT(rv);
                new_rmep_id_alloced = TRUE;
                endpoint_info_lcl->remote_id = rmep_index;

                SOC_PPC_OAM_RMEP_INFO_DATA_clear(&rmep_info);
                rmep_info.mep_index = endpoint_info_lcl->id;
                rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, rmep_index, &rmep_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Update entry in rmep db and rmep index db*/
                _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, rmep_index);
                soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_index_internal, 0, endpoint_info_lcl->id, mep_db_entry.mep_type, &rmep_db_entry, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else {
                /* in case there is no entry in RMEP DB we still want to indicate the existance of a mep*/
                rmep_index = (uint32)(-1);
            }

            /* In case of server classification of bfd-server, sw db need should not be updated */
            if( is_endpoint_bfd_server == 0) {
               /*update sw db of mep->rmep with new mep */
               rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, endpoint_info_lcl->id, rmep_index);
               BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
               endpoint_info_lcl->id = _BCM_BFD_SERVER_CLASSIFICATION_MEP_INDEX_GET(classifier_mep_entry.your_discriminator);
            }

           /*update sw db mep_info with new mep */
           rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
           BCMDNX_IF_ERR_EXIT(rv);
           {
               ENDPOINT_LIST_PTR endpoint_list;

               rv = BFD_ACCESS._bcm_bfd_endpoint_list.get(unit, &endpoint_list);
               BCMDNX_IF_ERR_EXIT(rv);
               rv = _bcm_dpp_oam_endpoint_list_member_add(unit, endpoint_list, endpoint_info_lcl->id);
               BCMDNX_IF_ERR_EXIT(rv);
           }
           if (is_endpoint_acc_to_oamp ||
               (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info_lcl->type))) {
               /* Increase usage counter.
                  Increase should be in end of function in order to prevent increase before EP create is successful*/
               your_disc_ref_counter++;
               rv = BFD_ACCESS.YourDiscriminator_ref_count.set(unit, your_disc_ref_counter);
               BCMDNX_IF_ERR_EXIT(rv);
           }
           /* NOT UPDATE. Increment no_of_sbfd_endpoints */
           if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info_lcl)) {
               _BCM_BFD_NOF_SBFD_ENDPOINTS_INCR(unit);
           }
        }
    }

    if(is_mpls_php) {
        endpoint_info_lcl->type = bcmBFDTunnelTypeMplsPhp;
    }

    BCM_EXIT;
exit:
    sal_memcpy(endpoint_info, endpoint_info_lcl, sizeof(bcm_bfd_endpoint_info_t));
    if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_rmep_id_alloced) {
        /* If error after new id was alloced we should free it */
        if (rmep_index != (uint32)(-1)) {
            rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, rmep_index);
        }
    }
    if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_mep_id_alloced) {
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint_info_lcl->id);
    }
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP((endpoint_info->remote_gport)))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport, rv);
    }

    BCMDNX_FUNC_RETURN;
}


int bcm_petra_bfd_endpoint_get(
   int unit,
   bcm_bfd_endpoint_t endpoint,
   bcm_bfd_endpoint_info_t *endpoint_info)
{
    uint32 soc_sand_rv;
    int rv;
    SOC_PPC_BFD_PDU_STATIC_REGISTER bfd_pdu;
    SOC_PPC_BFD_PDU_STATIC_REGISTER *bfd_pdu_to_update;
    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    uint8 found;
    uint16 udp_src_port;
    uint32 range;
    int dip_ndx;
    uint8 is_bfd_init;
    uint32 rmep_index_internal;
    uint8 is_default;
    uint8 is_endpoint_acc_to_oamp = 0;
    uint8 is_bfd_server_client = 0;
    uint8 is_oamp_server = 0;
    ENDPOINT_LIST_PTR rmep_list_p = 0;
    uint32 member_index;
    ENDPOINT_LIST_MEMBER_PTR member;
    uint32 is_m_hop_extended = 0;
    uint64 arg64;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_get(unit, id_internal, endpoint_info);
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(id_internal, endpoint_info->id);
            
            return rv;
        }
    }       

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    endpoint_info->id = endpoint;

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint);
    /* Default BFD trap */
    if (is_default) {
        int sw_db_epid = endpoint;
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            /* Arad B1 and below create a default endpoint with id=-1 but save it in the SW DB
               with key (id) = -2 to differentiate from OAM default endpoint */
            sw_db_epid = -2;
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sw_db_epid, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Default profile not configured.\n")));
        }

        /* Get remote_gport */
        endpoint_info->remote_gport = classifier_mep_entry.remote_gport;

        BCM_EXIT;
    }

    is_bfd_server_client =  _BCM_BFD_IS_SERVER_CLASSIFICATION_MEP(endpoint);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
    }

    /* Test classifier entry for acceleration and
       whether an OAMP server is used */
    rv = _bcm_bfd_classifier_entry_acceleration_to_oamp_get(
            unit, &classifier_mep_entry, &is_endpoint_acc_to_oamp,&is_oamp_server);
    BCMDNX_IF_ERR_EXIT(rv);


    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

     if ((is_endpoint_acc_to_oamp)&&(!is_bfd_server_client)) {
        /* OAMP MEP DB */
        /*get entry of mep db*/
        
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_bfd_mep_db_entry_struct_get(unit, endpoint_info, &mep_db_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        /* In BFD transmit disable tx_gport will be invalid and it's store in tx_gport variable*/
        endpoint_info->tx_gport = classifier_mep_entry.tx_gport;


        /* get rmep id from sw db of mep->rmep */
        rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
        }

        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &member) ;
        BCMDNX_IF_ERR_EXIT(rv);
        _bcm_dpp_oam_endpoint_list_get_member_index(unit, member, &member_index) ;
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_info->remote_id = (bcm_bfd_endpoint_t)member_index ;
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, member_index);
        soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_index_internal, &rmep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_bfd_rmep_db_entry_struct_get(unit, &rmep_db_entry, endpoint_info, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    /*RX - classifier*/
    rv = _bcm_bfd_classifier_mep_entry_struct_get(unit, endpoint_info, &classifier_mep_entry, &dip_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_endpoint_acc_to_oamp ||
        (_BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type))) {
        soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_get(unit, &range);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* When this line is reached, the value in endpoint_info->local_discr is taken from the sw db holding the
           classifier_mep_entry, which contains the 16 LSB of the local_discr, that the endpoint was created with.
           The range is composed of the 19 MSB of the same value (18 MSB in Jericho), so need to truncate accordingly. */
        range >>= (SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START-SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START);
        endpoint_info->local_discr |= range << SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START;

        /*Turn off MSB bit */
        if(mep_db_entry.disc_type_update) {
            endpoint_info->local_discr = endpoint_info->local_discr & ~(1<<31);
        }
    }

    if (is_endpoint_acc_to_oamp && !is_oamp_server) {
        if ((endpoint_info->type == bcmBFDTunnelTypeMplsTpCc) && (SOC_IS_ARAD_B1_AND_BELOW(unit))) {
            /* If type is MPLSTP in ARAD+ it is set into OAMP as PWE */
            SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);
            soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_get(unit, &bfd_cc_packet);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            endpoint_info->local_discr =  bfd_cc_packet.bfd_my_discr;
            endpoint_info->remote_discr = bfd_cc_packet.bfd_your_discr;
            bfd_pdu_to_update = &(bfd_cc_packet.bfd_static_reg_fields);
        } else {
            SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
            soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            bfd_pdu_to_update = &(bfd_pdu);
        }
    if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval){    
        if (mep_db_entry.tos_ttl_profile == _BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE) {
            endpoint_info->local_min_echo = 0;
        } else {
            endpoint_info->local_min_echo = bfd_pdu_to_update->bfd_req_min_echo_rx_interval;
        }
    } else {
        endpoint_info->local_min_echo = bfd_pdu_to_update->bfd_req_min_echo_rx_interval;
    }

		if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            endpoint_info->local_diag = bfd_pdu_to_update->bfd_diag;
            endpoint_info->local_flags = bfd_pdu_to_update->bfd_flags; /* These are only C,A,D,M flags. P and F are added from the mep db */
            endpoint_info->local_state = bfd_pdu_to_update->bfd_sta;
        }
    }

    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) && 
        (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
    }

    if (endpoint_info->type == bcmBFDTunnelTypeMpls || endpoint_info->type == bcmBFDTunnelTypeMplsPhp || endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv) {
        if(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP){
            soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_src_port);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else {
            soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_get(unit, &udp_src_port);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        endpoint_info->udp_src_port = udp_src_port;
    } else if (endpoint_info->type == bcmBFDTunnelTypeUdp) {
        soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_get(unit, &udp_src_port);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->udp_src_port = udp_src_port;

        if (((((endpoint_info->flags & (BCM_BFD_ENDPOINT_MULTIHOP | BCM_BFD_ENDPOINT_HW_ACCELERATION_SET | BCM_BFD_ECHO))
              == BCM_BFD_ENDPOINT_MULTIHOP)) && !is_m_hop_extended) 
            || (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info))) {
            /* Strictly multi hop. multi hop*/
            SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;
            rv = _bcm_dpp_am_template_bfd_ip_dip_data_get(unit, dip_ndx, &ipv6_addr);
            BCMDNX_IF_ERR_EXIT(rv);

            endpoint_info->src_ip_addr = ipv6_addr.address[0];
        }
    }
    if (_BCM_BFD_ENDPOINT_IS_SBFD_REFLECTOR(unit, endpoint_info)) {
        COMPILER_64_ZERO(arg64);
        soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_SBFD_REFLECTOR, &arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        COMPILER_64_TO_32_LO(endpoint_info->local_min_rx, arg64);
        COMPILER_64_TO_32_HI(endpoint_info->src_ip_addr, arg64);
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.reflector_status.get(unit,&(endpoint_info->local_state)));
    }

    if (_BCM_BFD_ENDPOINT_IS_SBFD_INITIATOR(unit, endpoint_info)) {
        uint32 sbfd_udp_src_port = 0;
        /* Overwrite the UDP src port with SBFD value if it is SBFD endpoint */
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_seamless_bfd_rx_dst_port_get,(unit, &sbfd_udp_src_port));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        endpoint_info->udp_src_port = sbfd_udp_src_port;
    }

    BCM_EXIT;
exit:
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit,endpoint_info->remote_gport,rv);
    }
    BCMDNX_FUNC_RETURN;

}

int bcm_petra_bfd_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint) {
    uint32 soc_sand_rv;
    int rv;
    uint8 found;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int is_last;
    uint32 trap_code;
    uint32 rmep_index_internal;
    uint32 mpls_udp_sport_ref_counter;
    uint32 mpls_cc_ref_counter;
    uint32 ipv4_udp_sport_ref_counter;
    uint32 pdu_ref_counter;
    int ref_counter;
    uint8 is_bfd_init;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    uint32 server_trap_ref_counter;
    uint8 is_default;
    int is_single_hop_extended=0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint8 is_endpoint_acc_to_oamp = 0;
    uint8 is_oamp_server = 0;
    ENDPOINT_LIST_PTR rmep_list_p = 0;
    uint32 member_index;
    ENDPOINT_LIST_MEMBER_PTR member;
    int is_empty;
    uint32 is_m_hop_extended = 0;
    uint8 no_of_sbfd_reflector = 0;
    uint8 is_bfd_server_client = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /* Destroy the endpoint that created on ukernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            return bcm_petra_bfd_uc_endpoint_destroy(unit, id_internal);
        }
    }     

    is_default = _BCM_BFD_IS_MEP_ID_DEFAULT(unit, endpoint);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    /*
     * For coverity
     */
    BCM_DPP_UNIT_CHECK(unit);

    if (is_default) {
        /* Default BFD trap */
        int sw_db_epid = endpoint;
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            sw_db_epid = -2;
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sw_db_epid, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }

        if ((SOC_IS_ARAD_B1_AND_BELOW(unit)) && (classifier_mep_entry.non_acc_profile != SOC_PPC_OAM_PROFILE_DEFAULT)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                (_BSL_BCM_MSG("Error: Internal error in default profile configuration.\n")));
        }

        /* Free profile */
        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(unit, endpoint, &profile_data);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(unit, classifier_mep_entry.non_acc_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);

        if (classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) {
            rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                       classifier_mep_entry.remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (SOC_IS_ARADPLUS(unit)) {
            rv = _bcm_bfd_default_id_from_ep_id(unit, endpoint, &default_id);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_remove,
                                                                                    (unit, default_id));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, sw_db_epid);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
    }


    /* Test classifier entry for acceleration and
       whether an OAMP server is used */
    rv = _bcm_bfd_classifier_entry_acceleration_to_oamp_get(
            unit, &classifier_mep_entry, &is_endpoint_acc_to_oamp, &is_oamp_server);
    BCMDNX_IF_ERR_EXIT(rv);

    is_single_hop_extended =((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) && ((classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
                                (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_MPLS_PHP)));
    is_bfd_server_client =  _BCM_BFD_IS_SERVER_CLASSIFICATION_MEP(endpoint);

    if ((is_endpoint_acc_to_oamp) && (!is_bfd_server_client)){
        /* delete OAMP MEP DB entry */

        /* deallocate all the allocated pointers */

        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


        rv = _bcm_mep_db_entry_dealloc2(unit, endpoint, &mep_db_entry, NULL /*"what to free" - free everything */);
        BCMDNX_IF_ERR_EXIT(rv);


        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, endpoint, &mep_db_entry, 0, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* delete remote endpoint entry */
        rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found) ;
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found."), endpoint));
        }
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &member) ;
        BCMDNX_IF_ERR_EXIT(rv);
        _bcm_dpp_oam_endpoint_list_get_member_index(unit, member, &member_index) ;
        BCMDNX_IF_ERR_EXIT(rv);

        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_index_internal, member_index) ;

        rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_index_internal);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_oam_oamp_rmep_delete(unit, rmep_index_internal, 0, endpoint, mep_db_entry.mep_type);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, member_index);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, member_index);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_oamp_server) {
            /* Manage OAMP supported traps */
            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport), &server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            server_trap_ref_counter--;
            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport), server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!server_trap_ref_counter) {
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, classifier_mep_entry.mep_type, BCM_GPORT_TRAP_GET_ID(classifier_mep_entry.remote_gport)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

    }

    if (((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) != 0) && (!is_bfd_server_client)){
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*RX - classifier*/

    if (is_endpoint_acc_to_oamp && !is_oamp_server) {
        if (((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
            (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)) &&
            (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR)) &&
            (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR))) {

            rv = BFD_ACCESS.ipv4_udp_sport_ref_counter.get(unit, &ipv4_udp_sport_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            ipv4_udp_sport_ref_counter--;
            rv = BFD_ACCESS.ipv4_udp_sport_ref_counter.set(unit, ipv4_udp_sport_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            if (ipv4_udp_sport_ref_counter == 0) {
                soc_sand_rv = soc_ppd_oam_bfd_ipv4_udp_sport_set(unit, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

        } else if ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) &&
            (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR)) &&
            (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR))) {
            rv = BFD_ACCESS.mpls_udp_sport_ref_counter.get(unit, &mpls_udp_sport_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            mpls_udp_sport_ref_counter--;
            rv = BFD_ACCESS.mpls_udp_sport_ref_counter.set(unit, mpls_udp_sport_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            if (mpls_udp_sport_ref_counter == 0) {
                soc_sand_rv = soc_ppd_oam_bfd_mpls_udp_sport_set(unit, 0);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } else if ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP)) {
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                rv = BFD_ACCESS.mpls_tp_cc_ref_counter.get(unit, &mpls_cc_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                mpls_cc_ref_counter--;
                rv = BFD_ACCESS.mpls_tp_cc_ref_counter.set(unit, mpls_cc_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                if (mpls_cc_ref_counter == 0) {
                    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER bfd_cc_packet;
                    SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(&bfd_cc_packet);
                    soc_sand_rv = soc_ppd_oam_bfd_cc_packet_static_register_set(unit, &bfd_cc_packet);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
        if ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) || (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||
            (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE) ||
            (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL)) {
				SOC_PPC_BFD_PDU_STATIC_REGISTER bfd_pdu;
				SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&bfd_pdu);
				soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_get(unit, &bfd_pdu);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = BFD_ACCESS.pdu_ref_counter.get(unit, &pdu_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            pdu_ref_counter--;
            if (!(SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval == 1)){
			rv = BFD_ACCESS.pdu_ref_counter.set(unit, pdu_ref_counter);
			BCMDNX_IF_ERR_EXIT(rv);
            } else if (_BCM_BFD_IPV4_ECHO_MIN_INTERVAL_PROFILE != mep_db_entry.tos_ttl_profile){
                rv = BFD_ACCESS.pdu_ref_counter.set(unit, pdu_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            if (pdu_ref_counter == 0) {

                bfd_pdu.bfd_diag = 0;
                bfd_pdu.bfd_flags = 1;
                bfd_pdu.bfd_req_min_echo_rx_interval = 0;
                bfd_pdu.bfd_sta = 3; /* UP */

                soc_sand_rv = soc_ppd_oam_bfd_pdu_static_register_set(unit, &bfd_pdu);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    if ((classifier_mep_entry.remote_gport != BCM_GPORT_INVALID) && !(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER)) {
        rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                   classifier_mep_entry.remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR) {
        soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_SBFD_REFLECTOR, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_reflector.get(unit, &no_of_sbfd_reflector));
        if(!no_of_sbfd_reflector){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error:  reflector Endpoint %d not exist."), endpoint));
        }
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.bfd.no_of_sbfd_reflector.set(unit, 0));
    }

    /* Do not care about multi hop feature if the endpoint is SBFD initiator. */
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR)) {
        if ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)) {
            is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
        }
    }

    if (!is_oamp_server
        && (mep_db_entry.diag_profile!=0xf /* diag profile will be 0xf only for BFD ECHO.*/)
        && !is_single_hop_extended
        && !is_m_hop_extended) {

        /* Free profile */
        if ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == 0) {
            SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);
            rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(unit, endpoint, &profile_data);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(unit, classifier_mep_entry.non_acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);
            rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_data_get(unit, endpoint, &profile_data);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_free(unit, classifier_mep_entry.acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if(!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR)){
            soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, endpoint, &classifier_mep_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        /* Multicast DIP - supporting only 16 */
        if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) {
            rv = _bcm_dpp_am_template_bfd_ip_dip_free(unit, classifier_mep_entry.dip_profile_ndx, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_last) {
                SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;
                soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_addr);

                soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, classifier_mep_entry.dip_profile_ndx, &ipv6_addr);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    /* Range is set if _BCM_BFD_ENDPOINT_IS_ACCELERATED_TO_OAMP(*endpoint_info) ||
        _BCM_BFD_IS_CLASSIFICATION_DONE_USING_YOUR_DISC(endpoint_info->type)
       This is equivalent to the following condition over the classifier entry*/
    if ( is_endpoint_acc_to_oamp
        || ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP)
            || (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP))
            || ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS)&&(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR))) {
        rv = BFD_ACCESS.YourDiscriminator_ref_count.get(unit, &ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        --ref_counter;
        rv = BFD_ACCESS.YourDiscriminator_ref_count.set(unit, ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(!is_bfd_server_client) {
      rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(unit, endpoint);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
    BCMDNX_IF_ERR_EXIT(rv);

    {
        ENDPOINT_LIST_PTR endpoint_list ;

        rv = BFD_ACCESS._bcm_bfd_endpoint_list.get(unit, &endpoint_list) ;
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_endpoint_list_member_find(unit, endpoint_list, endpoint, 1, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_endpoint_list_empty(unit, endpoint_list, &is_empty) ;
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_empty)
        {
            soc_sand_rv = soc_ppd_oam_bfd_discriminator_range_registers_set(unit, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /*delete entry from LEM in case this is a bfd ipv4 single hop*/
    if ((is_single_hop_extended || is_m_hop_extended) && (mep_db_entry.diag_profile!=0xf))/*sigle hop extended mode*/ {
        rv= MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_remove, (unit, classifier_mep_entry.ma_index,classifier_mep_entry.flags));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*delete entry from LEM in case of bdfd ECHO*/
    if (SOC_DPP_CONFIG(unit)->pp.bfd_echo_with_lem == 1 && mep_db_entry.diag_profile==0xf) {
        rv = _bcm_bfd_echo_lem_entry_delete(unit, mep_db_entry.dst_ip_add);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Decrement the no_of_sbfd_endpoints */
    if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR) {
        _BCM_BFD_NOF_SBFD_ENDPOINTS_DECR(unit);
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_endpoint_destroy_all(int unit) {
    ENDPOINT_LIST_MEMBER_PTR mep_list_p = 0 ;
    int rv;
    uint8 is_bfd_init;
    int is_empty ;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_EXIT;
    }

    /* Destroy all the endpoint that created on ukernel on matter it is exists or not*/
    if (bcm_petra_bfd_uc_is_init(unit)) {   
        rv = bcm_petra_bfd_uc_endpoint_destroy_all(unit);   
        BCMDNX_IF_ERR_EXIT(rv);
    }
    {    
        ENDPOINT_LIST_PTR endpoint_list ;

        rv = BFD_ACCESS._bcm_bfd_endpoint_list.get(unit, &endpoint_list) ;
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_endpoint_list_empty(unit, endpoint_list, &is_empty) ;
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_empty)
        {
            BCM_EXIT;
        }
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, endpoint_list, &mep_list_p) ;
        BCMDNX_IF_ERR_EXIT(rv);
    }

    while (mep_list_p != 0)
    {
        uint32 index ;
        ENDPOINT_LIST_MEMBER_PTR next ;

        rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, mep_list_p, &index) ;
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, mep_list_p, &next) ;
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_bfd_endpoint_destroy(unit, index);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_list_p = next;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
int
bcm_petra_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_poll(unit, id_internal);
            BCMDNX_IF_ERR_EXIT(rv);
            
            return rv;
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Check whether to register the events to ukernel*/
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_event_register(unit, event_types, cb, user_data);
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    rv = _bcm_petra_bfd_event_register(unit, event_types, cb, user_data); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Check whether to register the events to ukernel*/
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_event_unregister(unit, event_types, cb);
        BCMDNX_IF_ERR_EXIT(rv);

        return rv;
    } 

    rv = _bcm_petra_bfd_event_unregister(unit, event_types, cb);
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint32 clear)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        if (_BCM_BFD_MEP_INDEX_IS_UC(endpoint)) {
            bcm_bfd_endpoint_t id_internal;

            _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(id_internal, endpoint);
            rv = bcm_petra_bfd_uc_endpoint_stat_get(unit, id_internal, ctr_info, clear);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_sha1_set(unit, index, sha1);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_bfd_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_sha1_get(unit, index, sha1);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_simple_password_set(unit, index, sp);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_bfd_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    int rv;
    uint8 is_bfd_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /* Get the endpoint from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_auth_simple_password_get(unit, index, sp);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Clear all BFD (used in debug) */
int bcm_petra_bfd_clear(int unit) {
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint8 oam_is_init, is_bfd_init;
    int diag_ind;
    int trap_code_converted;
    uint32 trap_id_sw;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_EXIT;
    }

    rv = bcm_petra_bfd_endpoint_destroy_all(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, FALSE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_CPU, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_IPV4, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
    BCMDNX_IF_ERR_EXIT(rv);

    rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_MPLS, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_PWE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_OAMP_CC_MPLS_TP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable) {
        rv = BFD_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_UC_IPV6, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,SOC_PPC_TRAP_CODE_BFD_ECHO_O_IPV6, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _BCM_BFD_ENABLED_SET(FALSE);

    rv = _bcm_dpp_oamp_interrupts_init(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

	if (SOC_IS_ARADPLUS(unit)) {
		/* Set the diag values (1-8)*/
		for (diag_ind = 0;  diag_ind < 16 /* 16 profiles */; ++diag_ind) {
			soc_sand_rv = soc_ppd_oam_bfd_diag_profile_set(unit,diag_ind, 0 );
			BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
		}
	}

    soc_sand_rv = soc_ppd_oam_deinit(unit, 1/*is_bfd*/, !oam_is_init);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_detach(int unit) {

    bcm_error_t rv = BCM_E_NONE;
    uint8 oam_is_init, is_bfd_init;

    BCMDNX_INIT_FUNC_DEFS;
    
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_EXIT;
    }

    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_deinit(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, FALSE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* deinit dma */
    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_arad_pp_oam_dma_clear, (unit)));


exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_discard_stat_get(int unit, bcm_bfd_discard_stat_t *discarded_info)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Get the stats from uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_discard_stat_get(unit, discarded_info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_bfd_discard_stat_set(int unit, bcm_bfd_discard_stat_t *discarded_info)
{
    int rv;
    uint8 is_bfd_init;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    
    /* Set the stats in uKernel */
    if (bcm_petra_bfd_uc_is_init(unit)) {
        rv = bcm_petra_bfd_uc_discard_stat_set(unit, discarded_info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = BCM_E_UNAVAIL;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* ***************************************************************/
/*******************ENDPOINT DIAGNOSTICS SECTION********************/
/* ***************************************************************/


void bcm_dpp_print_mpls_egress_label(bcm_mpls_egress_label_t *egress_label)
{
    LOG_CLI((BSL_META("\n\t\tlabel: %u\n\t\texp: %u\n\t\tTTL: %u\n\t\tPacket Priority: %d\n, "), egress_label->label, egress_label->exp, egress_label->ttl, egress_label->pkt_pri));
}




/*function simply prints information about the endpoint, no questions asked*/
void
bcm_dpp_bfd_endpoint_diag_print(bcm_bfd_endpoint_info_t *endpoint_info) {
    LOG_CLI((BSL_META("=====BFD endpoint ID: 0X%x \n\t***Properties:\n"), endpoint_info->id));

    LOG_CLI((BSL_META("\tType: ")));
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        LOG_CLI((BSL_META("BFD over IPV4"))); break;
    case bcmBFDTunnelTypeMpls:
        LOG_CLI((BSL_META("BFD over MPLS LSP"))); break;
    case bcmBFDTunnelTypePweControlWord:
        LOG_CLI((BSL_META("BFD over PWE control word (VCCV Type 1)"))); break;
    case bcmBFDTunnelTypePweTtl:
        LOG_CLI((BSL_META("BFD over PWE ttl 1 (VCCV Type 3)"))); break;
    case bcmBFDTunnelTypeMplsTpCc:
        LOG_CLI((BSL_META("BFD over MPLS-TP proactive CC"))); break;
    case bcmBFDTunnelTypeMplsTpCcCv:
        LOG_CLI((BSL_META("BFD over MPLS-TP proactive CC&CV"))); break;
    case bcmBFDTunnelTypePweGal:
        LOG_CLI((BSL_META("BFD over PWE With GAL"))); break;
    case bcmBFDTunnelTypePweRouterAlert:
        LOG_CLI((BSL_META("BFD over PWE With Router Alert (VCCV Type 2)")));
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH) {
            LOG_CLI((BSL_META(" including PWE-ACH")));
        }
        break; 
    default:
        LOG_CLI((BSL_META("INVALID TYPE")));
    }

    if ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR) != 0) {
        LOG_CLI((BSL_META("\n\tSeamless BFD role: initiator")));
    } else if ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_REFLECTOR) != 0) {
        LOG_CLI((BSL_META("\n\tSeamless BFD role: reflector")));
    }

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MPLS_TP_POLL_SEQUENCE_ENABLE) {
        LOG_CLI((BSL_META("\n\tMPLS TP poll sequnce enabled.")));
    }
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
        LOG_CLI((BSL_META("\n\tBFD session is processed in OAMA (rather than CPU). TX gport: 0X%x"), endpoint_info->tx_gport));
    }
    if (endpoint_info->remote_gport != BCM_GPORT_INVALID) {
        LOG_CLI((BSL_META("\n\tRemote gport: 0X%x"), endpoint_info->remote_gport));
    } else {
        LOG_CLI((BSL_META("\n\tRemote gport disabled")));
    }
    if (endpoint_info->gport != BCM_GPORT_INVALID) {
        LOG_CLI((BSL_META("\n\tgport: 0X%x"), endpoint_info->gport));
    } else {
        LOG_CLI((BSL_META("\n\tgport disabled")));
    }

    if (endpoint_info->type == bcmBFDTunnelTypeUdp || endpoint_info->type == bcmBFDTunnelTypeMpls || endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv) {
        LOG_CLI((BSL_META("\n\tSource address: " IPV4_PRINT_FMT),
                 IPV4_PRINT_ARG(endpoint_info->src_ip_addr)));
        LOG_CLI((BSL_META(" UDP source port: %d"), endpoint_info->udp_src_port));
    }
    if (endpoint_info->type == bcmBFDTunnelTypeUdp) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP 
        || ((endpoint_info->flags2 & BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR) != 0)) {
            LOG_CLI((BSL_META("\n\tIPV4 multi hop")));
            if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
                LOG_CLI((BSL_META(", Destination IP address: " IPV4_PRINT_FMT),
                         IPV4_PRINT_ARG(endpoint_info->dst_ip_addr)));
                LOG_CLI((BSL_META(", IP TTL: %u, IP TOS: %u"), endpoint_info->ip_ttl, endpoint_info->ip_tos));
            }
        }
    } else if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW){
        LOG_CLI((BSL_META("\n\tFirst MPLS label on MPLS stack: ")));
        bcm_dpp_print_mpls_egress_label(&(endpoint_info->egress_label));
    }


    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
        LOG_CLI((BSL_META("\n\tLocal state: ")));
        switch (endpoint_info->local_state) {
        case bcmBFDStateAdminDown:
            LOG_CLI((BSL_META("Admin down"))); break;
        case bcmBFDStateDown:
            LOG_CLI((BSL_META("down"))); break;
        case bcmBFDStateUp:
            LOG_CLI((BSL_META("up"))); break;
        case bcmBFDStateInit:
            LOG_CLI((BSL_META("Init"))); break;
        default:
            LOG_CLI((BSL_META("INVALID LOCAL STATE")));
        }

        LOG_CLI((BSL_META(",\tRemote state: ")));
        switch (endpoint_info->remote_state) {
        case bcmBFDStateAdminDown:
            LOG_CLI((BSL_META("Admin down"))); break;
        case bcmBFDStateDown:
            LOG_CLI((BSL_META("down"))); break;
        case bcmBFDStateUp:
            LOG_CLI((BSL_META("up"))); break;
        case bcmBFDStateInit:
            LOG_CLI((BSL_META("Init"))); break;
        default:
            LOG_CLI((BSL_META("INVALID REMOTE STATE")));
        }

        LOG_CLI((BSL_META("\n\tLocal diagnostic: ")));
        switch (endpoint_info->local_diag) {
        case bcmBFDDiagCodeNone:
            LOG_CLI((BSL_META("No Diagnostic"))); break;
        case bcmBFDDiagCodeCtrlDetectTimeExpired:
            LOG_CLI((BSL_META("Control Detection Time Expired"))); break;
        case bcmBFDDiagCodeEchoFailed:
            LOG_CLI((BSL_META("Echo Function Failed"))); break;
        case bcmBFDDiagCodeNeighbourSignalledDown:
            LOG_CLI((BSL_META("Neighbor Signaled Session Down"))); break;
        case bcmBFDDiagCodeForwardingPlaneReset:
            LOG_CLI((BSL_META("Forwarding Plane Reset"))); break;
        case bcmBFDDiagCodePathDown:
            LOG_CLI((BSL_META("Path Down"))); break;
        case bcmBFDDiagCodeConcatPathDown:
            LOG_CLI((BSL_META("Concatenated Path Down"))); break;
        case bcmBFDDiagCodeAdminDown:
            LOG_CLI((BSL_META("Administratively Down"))); break;
        case bcmBFDDiagCodeRevConcatPathDown:
            LOG_CLI((BSL_META("Reverse Concatenated Path Down"))); break;
        default:
            LOG_CLI((BSL_META("INVALID DIAG")));
        }

        LOG_CLI((BSL_META(",\tRemote diagnostic: ")));
        switch (endpoint_info->remote_diag) {
        case bcmBFDDiagCodeNone:
            LOG_CLI((BSL_META("No Diagnostic"))); break;
        case bcmBFDDiagCodeCtrlDetectTimeExpired:
            LOG_CLI((BSL_META("Control Detection Time Expired"))); break;
        case bcmBFDDiagCodeEchoFailed:
            LOG_CLI((BSL_META("Echo Function Failed"))); break;
        case bcmBFDDiagCodeNeighbourSignalledDown:
            LOG_CLI((BSL_META("Neighbor Signaled Session Down"))); break;
        case bcmBFDDiagCodeForwardingPlaneReset:
            LOG_CLI((BSL_META("Forwarding Plane Reset"))); break;
        case bcmBFDDiagCodePathDown:
            LOG_CLI((BSL_META("Path Down"))); break;
        case bcmBFDDiagCodeConcatPathDown:
            LOG_CLI((BSL_META("Concatenated Path Down"))); break;
        case bcmBFDDiagCodeAdminDown:
            LOG_CLI((BSL_META("Administratively Down"))); break;
        case bcmBFDDiagCodeRevConcatPathDown:
            LOG_CLI((BSL_META("Reverse Concatenated Path Down"))); break;
        default:
            LOG_CLI((BSL_META("INVALID DIAG")));
        }
        LOG_CLI((BSL_META("\n\tRemote Flags: 0x%x,\t Local Flags 0x%x"), endpoint_info->remote_flags, endpoint_info->local_flags));
        LOG_CLI((BSL_META("\n\tQueing priority: %d"), endpoint_info->int_pri));
        LOG_CLI((BSL_META("\n\tBFD rate (ms): %d"), endpoint_info->bfd_period));
        LOG_CLI((BSL_META("\n\tDesired local min TX interval: %u, Required local RX interval: %u"), endpoint_info->local_min_tx, endpoint_info->local_min_rx));
        LOG_CLI((BSL_META("\n\tLocal discriminator: %u, Local detection multiplier: %u"), endpoint_info->local_discr, endpoint_info->local_detect_mult));
        LOG_CLI((BSL_META("\n\tRemote discriminator: %u"), endpoint_info->remote_discr));
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME) {
            LOG_CLI((BSL_META("\n\tRemote detection explicit timeout: %u"), endpoint_info->bfd_detection_time));
        }
        else {
            LOG_CLI((BSL_META("\n\tRemote detection multiplier: %u"), endpoint_info->remote_detect_mult));
        }
    }

    LOG_CLI((BSL_META("\n")));
}

