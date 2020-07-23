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

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JERP/jerp_intr.h>
#include <soc/dpp/QAX/qax_intr.h>
#include <soc/dpp/QUX/qux_intr.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#endif /*BCM_ARAD_SUPPORT*/

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>

#include <soc/dpp/ARAD/arad_ports.h>

extern int _bcm_oam_custom_feature_oamp_flex_da_mac_set(int unit, int id, uint8 *loss_delay_da);
/***************************************************************/
/***************************************************************/
/*
 * Local defines
 * {
 */
#define OAM_HASHS_ACCESS  OAM_ACCESS.oam_hashs

#define ENDP_LST_ACCESS          OAM_ACCESS.endp_lst
#define ENDP_LST_ACCESS_DATA     OAM_ACCESS.endp_lst.endp_lsts_array
#define ENDP_LST_M_ACCESS        OAM_ACCESS.endp_lst_m
#define ENDP_LST_M_ACCESS_DATA   OAM_ACCESS.endp_lst_m.endp_lst_ms_array

#define _JER_PP_OAM_TC_NOF_BITS 3

#define _JER_PP_OAM_DP_NOF_BITS 2

#define SAT_REPORT_VALUE 0xffffffff


/* } */
/***************************************************************/
/***************************************************************/

_bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_rmep_info_db_lock[BCM_MAX_NUM_UNITS];
_bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_mep_info_db_lock[BCM_MAX_NUM_UNITS];

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
/* OAM */
static bcm_oam_event_cb _g_oam_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* OAM LM/DM statistics reports handlers */
static bcm_oam_performance_event_cb _g_oam_performance_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* User data */
static void*            _g_oam_event_ud[BCM_MAX_NUM_UNITS][bcmOAMEventCount];
/* BFD */
static bcm_bfd_event_cb _g_bfd_event_cb[BCM_MAX_NUM_UNITS][bcmBFDEventCount];
/* User data */
static void*            _g_bfd_event_ud[BCM_MAX_NUM_UNITS][bcmBFDEventCount];

/* SAT */
static bcm_sat_event_cb _g_sat_event_cb[BCM_MAX_NUM_UNITS][bcmSATEventCount];
/* User data */
static void *_g_sat_event_ud[BCM_MAX_NUM_UNITS][bcmSATEventCount];



/* global variables used in bcm_petra_oam_(endpoint|group)_traverse( ) */
static bcm_oam_group_traverse_cb_w_param_t _group_cb_with_param;

/***************************************************************/
/***************************************************************/

/*
 * MACROS
 */

#define _BCM_OAM_PPD_EVENT_PORT_BLOCKED(rmep_state) \
  (((rmep_state >> 3) & 3) == 1)

#define _BCM_OAM_PPD_EVENT_PORT_UP(rmep_state) \
  (((rmep_state >> 3) & 3) == 2)

#define _BCM_OAM_PPD_EVENT_INTERFACE_UP(rmep_state) \
  ((rmep_state & 7) == 1)

#define _BCM_OAM_PPD_EVENT_INTERFACE_DOWN(rmep_state) \
  ((rmep_state & 7) == 2)

#define _BCM_OAM_PPD_EVENT_INTERFACE_TESTING(rmep_state) \
  ((rmep_state & 7) == 3)

#define _BCM_OAM_PPD_EVENT_INTERFACE_UNKNOWN(rmep_state) \
  ((rmep_state & 7) == 4)

#define _BCM_OAM_PPD_EVENT_INTERFACE_DORMANT(rmep_state) \
  ((rmep_state & 7) == 5)

#define _BCM_OAM_PPD_EVENT_INTERFACE_NOT_PRESENT(rmep_state) \
  ((rmep_state & 7) == 6)

#define _BCM_OAM_PPD_EVENT_INTERFACE_LL_DOWN(rmep_state) \
  ((rmep_state & 7) == 7)

/* When upmep oam id is to be added to the FHEI, opcode is defined to include meta-data*/
/* Currently all opcodes include meta-data. This macro remains nonetheless */
#define _BCM_OAM_IS_OPCODE_WITH_META_DATA(opcode) 1

#define _BCM_OAM_MIRROR_PROFILE_DEALLOC(mirror_profile_dest) \
    do { \
        int dont_care; \
        if (_bcm_dpp_am_template_mirror_action_profile_free(unit, mirror_profile_dest, &dont_care) != BCM_E_NONE) { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to deallocate mirroring destination\n"))); \
        } \
    } while (0)

#define OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW 1024

#define GET_MSB_FROM_MAC_ADDR(mac_addr) \
   mac_addr[2] | ( mac_addr[1] << 8) | (mac_addr[0] << 16)

#define GET_LSB_FROM_MAC_ADDR(mac_addr) \
   mac_addr[5] | ( mac_addr[4] << 8) | (mac_addr[3] << 16)

/* Macro expects the LSBs and MSBs in uint32 form*/
#define SET_MAC_ADDRESS_BY_MSB_AND_LSB(mac_addr, lsb,msb)\
               mac_addr[5] = lsb & 0xff;\
               mac_addr[4] = (lsb & 0xff00 )>>8;\
               mac_addr[3] = (lsb & 0xff0000 )>>16;\
               mac_addr[2] = msb & 0xff;\
               mac_addr[1] = (msb & 0xff00 )>>8;\
               mac_addr[0] = (msb & 0xff0000 )>>16;

/* Macro used by diagnostics functions */
#define MAC_PRINT_FMT "%02x:%02x:%02x:%02x:%02x:%02x\n"
#define MAC_PRINT_ARG(mac_addr) mac_addr[0], mac_addr[1], \
        mac_addr[2], mac_addr[3],                         \
        mac_addr[4], mac_addr[5]

/**
 * Macro allocates profiles from the NIC/OUI tables and updates
 * the mep_db_entry accordingly.
 */
#define CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, eth1731_profile_index, endpoint_id)\
    if (is_last) {\
        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;\
        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile_data);\
        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_id, 0, eth1731_profile_index, &eth1731_profile_data);\
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); \
    }

#define FREE_NIC_AND_OUI_PROFILES(endpoint_id) \
do {\
    if (!_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {\
        int nic_profile, oui_profile, is_last1, is_last2;\
        rv = _bcm_dpp_am_template_oam_lmm_nic_tables_free(unit, endpoint_id, &nic_profile, &is_last1);\
        BCMDNX_IF_ERR_EXIT(rv);\
        rv = _bcm_dpp_am_template_oam_lmm_oui_tables_free(unit, endpoint_id, &oui_profile, &is_last2);\
        BCMDNX_IF_ERR_EXIT(rv);\
        if (is_last1 && is_last2) {\
            soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit,endpoint_id,0,0,oui_profile,nic_profile);\
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);\
        }\
    }\
} while(0)


#define UPDATE_NIC_AND_OUI_REFERENCE_COUNTER( _msb, _lsb, _nic_profile, _oui_profile)\
    do {\
        int dont_care;\
        rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &_lsb, &dont_care, &_nic_profile);\
        BCMDNX_IF_ERR_EXIT(rv);\
        rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &_msb, &dont_care, &_oui_profile);\
        BCMDNX_IF_ERR_EXIT(rv);\
    } while (0)


#define _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(ptr, mep_id)\
    do {\
    uint32 lsb, msb;\
    int dont_care;\
        rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit, mep_id,  &dont_care, &lsb);\
        BCMDNX_IF_ERR_EXIT(rv); \
        rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit, mep_id,  &dont_care, &msb);\
        BCMDNX_IF_ERR_EXIT(rv); \
        SET_MAC_ADDRESS_BY_MSB_AND_LSB(ptr->peer_da_mac_address, lsb, msb);\
    } while (0)
            
            
            
#define _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(rdi, rx_disable, scanner_disable) \
               rdi = ( (rx_disable)==0) <<1 | ((scanner_disable)==0)

#define _BCM_OAM_GET_RDI_STATUS_FROM_ETH1731_MEP_PROF_RDI_FIELD(rdi_field, is_rdi_from_rx, is_rdi_from_scanner) \
               do {\
                       is_rdi_from_rx = (rdi_field & 0x2)==0x2;\
                       is_rdi_from_scanner = (rdi_field & 0x1);\
               } while (0)

/* Rate 1 means TX every other scan. Rate 2 means one every 6 scans. Higher rate acts as one per 24 scans.
   Zero gets you zero.*/
  /* Jericho only.*/
#define _OAM_RATE_TO_TX_PER_24_SCANS(rate) (((rate<=2)? (20 -8*rate) : 1) * (rate>0)) 

#define _BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(_bcm_mac_addr, level) \
do {\
     bcm_mac_t ccm_group_da_mac_addr_msbs = {0x1,0x80,0xc2, 0x00, 0x00, 0x30 };\
    sal_memcpy(_bcm_mac_addr,ccm_group_da_mac_addr_msbs, 6 );\
    _bcm_mac_addr[5] |= level;\
} while(0)

#define _BCM_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit) \
                (SOC_IS_QUX(unit) ? QUX_INT_OAMP_PENDING_EVENT : \
                (SOC_IS_QAX(unit) ? QAX_INT_OAMP_PENDING_EVENT : \
                (SOC_IS_JERICHO_PLUS(unit) ? JERP_INT_OAMP_PENDING_EVENT : \
                (SOC_IS_JERICHO(unit)? JER_INT_OAMP_PENDING_EVENT : \
                ARAD_INT_OAMP_PENDINGEVENT))))

#define _BCM_OAM_GET_OAMP_STAT_PENDING_EVENT_BY_DEVICE(unit) \
                (SOC_IS_QUX(unit) ? QUX_INT_OAMP_STAT_PENDING_EVENT : \
                (SOC_IS_QAX(unit) ? QAX_INT_OAMP_STAT_PENDING_EVENT : \
                (SOC_IS_JERICHO_PLUS(unit) ? JERP_INT_OAMP_STAT_PENDING_EVENT : \
                 JER_INT_OAMP_STAT_PENDING_EVENT)))

#define _BCM_OAM_MAP_RFC6374_OPCODE_TO_INTERNAL(rfc6374_opcode, internal_opcode) \
  switch (rfc6374_opcode) {\
    case bcmOamMplsLmDmOpcodeTypeLm:\
      internal_opcode = SOC_PPC_OAM_OPCODE_MAP_LMM;\
      break;\
    case bcmOamMplsLmDmOpcodeTypeDm:\
      internal_opcode = SOC_PPC_OAM_OPCODE_MAP_DMM ;\
      break;\
    case bcmOamMplsLmDmOpcodeTypeIlm:\
      internal_opcode = SOC_PPC_OAM_OPCODE_MAP_SLM;\
      break;\
    default:\
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported RFC-6374 opcode type.")));\
  }\

/***************************************************************/
/***************************************************************/
/*
 * OAM Module Helper functions
 */

/**
 * Use to calculate the LM/DM offsets
 * 
 * @author Ivan (24/10/2019)
 * 
 * @param mep_db_entry  - MEP DB entry information.
 * @param is_piggy_back - is piggy-pack indication.
 * @param is_slm entry  -  is SLM entry indication.
 * @param is_reply      - is response packet indication.
 * 
 * @return value        - offset that will be set.
 */
int _oam_set_lm_dm_offset(const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY * mep_db_entry, uint8 is_piggy_back, uint8 is_slm, uint8 is_dm, uint8 is_reply)
{
    int value = 0;
    uint8 is_rfc_6374 = _BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry->mep_type);

    /** 
    * Only one of these could be active at a time
    *   For piggy backed CCMs an extra 54 bytes are added.
    *   For slm an extra 8 bytes are added.
    *   RFC-6374 LM an extra 24 bytes are added for OAM PDU.
    *   RFC-6374 DM an extra 12 bytes are added for OAM PDU.
    */
    value += (is_piggy_back * 54) + (is_slm * 8) + (is_rfc_6374 * (24 - (is_dm * 12)));

    /** Offsets are calculated in the following way*/
    switch (mep_db_entry->mep_type) {
        case SOC_PPC_OAM_MEP_TYPE_ETH_OAM:
          /** ETH: DA, SA addresses (12B) + num-tags (taken from mep db) *4B */
            value += mep_db_entry->tags_num * 4 + 0x12;
            break;
          /** MPLS: GAL, GACH, MPLS/PWE (4B each), offset in OAM PDU * (4B) */ 
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP:
        case SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL:
        case SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS:
        case SOC_PPC_OAM_MEP_TYPE_Y1711_PWE:
            value += 0x10;
            break;
          /** RFC6374 PWE/Section: GAL/PWE, GACH (4B each) + value (calculated with the RFC OAM PDU) */ 
        case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE:
        case SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION:
            value += 0x08;
            break;
        default:
            value += 0x0c;
    }

    if (is_rfc_6374)
    {
        /** The offset for the Query and Response the same*/
        return value;
    }

    if (is_reply & is_dm) {
      /** DMR case */
      value += 0x10;
    } else if (is_reply) {
      /* For SLR the stamping is done 4 bytes after the LM stamping, for LMR 8.*/
      value += 4 + (!is_slm * 4);
    }

    return value;
}

/* RMEP state event is handles separately */
int _bcm_oam_ppd_to_bcm_oam_event_mep(int unit, uint32 ppd_event, bcm_oam_event_type_t *event_type, uint8 is_oam)
{
   BCMDNX_INIT_FUNC_DEFS;

   switch (ppd_event){
       case SOC_PPC_OAM_EVENT_LOC_SET:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimeout : bcmBFDEventEndpointTimeout;
               break;
       case SOC_PPC_OAM_EVENT_ALMOST_LOC_SET:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimeoutEarly : bcmBFDEventEndpointTimeoutEarly;
               break;
       case SOC_PPC_OAM_EVENT_LOC_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimein : bcmBFDEventEndpointTimein;
               break;
       case SOC_PPC_OAM_EVENT_RDI_SET:
               *event_type = is_oam ? bcmOAMEventEndpointRemote : bcmBFDEventEndpointRemote;
               break;
       case SOC_PPC_OAM_EVENT_RDI_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointRemoteUp : bcmBFDEventEndpointRemoteUp;
               break;
       case SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE:
               *event_type = is_oam ? 0 : bcmBFDEventStateChange;
               break;
       case SOC_PPC_OAM_EVENT_REPORT_RX_LM:
               *event_type = is_oam ? bcmOAMEventEndpointLmStatistics : -1;
               break;
       case SOC_PPC_OAM_EVENT_REPORT_RX_DM:
               *event_type = is_oam ? bcmOAMEventEndpointDmStatistics : -1;
               break;
       case SOC_PPC_OAM_EVENT_SD_SET:
               *event_type = is_oam ? bcmOAMEventEndpointSdSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SD_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointSdClear :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SF_SET:
               *event_type = is_oam ? bcmOAMEventEndpointSfSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SF_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointSfClear :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DEXCESS_SET:
               *event_type = is_oam ? bcmOAMEventEndpointDExcessSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DMISSMATCH:
               *event_type = is_oam ? bcmOAMEventEndpointDMissmatch :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DMISSMERGE:
               *event_type = is_oam ? bcmOAMEventEndpointDMissmerge :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DALL_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointDAllClear :  -1;
               break;
       default:
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Illegal PPD event \n")));
   }
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}
int _bcm_oam_ppd_to_bcm_bfd_event_mep(uint32 ppd_event, bcm_bfd_event_type_t *event_type, uint8 is_oam)
{
   BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

   switch (ppd_event){
       case SOC_PPC_OAM_EVENT_LOC_SET:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimeout : bcmBFDEventEndpointTimeout;
               break;
       case SOC_PPC_OAM_EVENT_ALMOST_LOC_SET:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimeoutEarly : bcmBFDEventEndpointTimeoutEarly;
               break;
       case SOC_PPC_OAM_EVENT_LOC_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointCCMTimein : bcmBFDEventEndpointTimein;
               break;
       case SOC_PPC_OAM_EVENT_RDI_SET:
               *event_type = is_oam ? bcmOAMEventEndpointRemote : bcmBFDEventEndpointRemote;
               break;
       case SOC_PPC_OAM_EVENT_RDI_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointRemoteUp : bcmBFDEventEndpointRemoteUp;
               break;
       case SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE:
               *event_type = is_oam ? 0 : bcmBFDEventStateChange;
               break;
       case SOC_PPC_OAM_EVENT_REPORT_RX_LM:
               *event_type = is_oam ? bcmOAMEventEndpointLmStatistics : -1;
               break;
       case SOC_PPC_OAM_EVENT_REPORT_RX_DM:
               *event_type = is_oam ? bcmOAMEventEndpointDmStatistics : -1;
               break;
       case SOC_PPC_OAM_EVENT_SD_SET:
               *event_type = is_oam ? bcmOAMEventEndpointSdSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SD_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointSdClear :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SF_SET:
               *event_type = is_oam ? bcmOAMEventEndpointSfSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_SF_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointSfClear :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DEXCESS_SET:
               *event_type = is_oam ? bcmOAMEventEndpointDExcessSet :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DMISSMATCH:
               *event_type = is_oam ? bcmOAMEventEndpointDMissmatch :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DMISSMERGE:
               *event_type = is_oam ? bcmOAMEventEndpointDMissmerge :  -1;
               break;
       case SOC_PPC_OAM_EVENT_DALL_CLR:
               *event_type = is_oam ? bcmOAMEventEndpointDAllClear :  -1;
               break;
       default:
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Illegal PPD event \n")));
      }
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

int _bcm_oam_ppd_from_bcm_event_mep(SOC_PPC_OAM_EVENT *soc_ppd_event_type, bcm_oam_event_type_t event_type, uint8 is_oam){

   BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

   switch (event_type) {
         case bcmOAMEventEndpointCCMTimeout:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_LOC_SET;
            break;
         case bcmOAMEventEndpointCCMTimein:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_LOC_CLR;
            break;
         case bcmOAMEventEndpointRemote:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_RDI_SET;
            break;
         case bcmOAMEventEndpointRemoteUp:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_RDI_CLR;
            break;
         case bcmOAMEventEndpointPortDown:
         case bcmOAMEventEndpointPortUp:
         case bcmOAMEventEndpointInterfaceDown:
         case bcmOAMEventEndpointInterfaceUp:
         case bcmOAMEventEndpointInterfaceTesting:
         case bcmOAMEventEndpointInterfaceUnkonwn:
         case bcmOAMEventEndpointInterfaceDormant:
         case bcmOAMEventEndpointInterfaceNotPresent:
         case bcmOAMEventEndpointInterfaceLLDown:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE;
            break;
         case bcmOAMEventEndpointSdSet:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_SD_SET;
            break;
         case bcmOAMEventEndpointSdClear:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_SD_CLR;
            break;
         case bcmOAMEventEndpointSfSet:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_SF_SET;
            break;
         case bcmOAMEventEndpointSfClear:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_SF_CLR;
            break;
         case bcmOAMEventEndpointDExcessSet:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_DEXCESS_SET;
            break;
         case bcmOAMEventEndpointDMissmatch:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_DMISSMATCH;
            break;
         case bcmOAMEventEndpointDMissmerge:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_DMISSMERGE;
            break;
         case bcmOAMEventEndpointDAllClear:
            *soc_ppd_event_type = SOC_PPC_OAM_EVENT_DALL_CLR;
            break;
         default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Illegal PPD event \n")));
   }
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

int _bcm_oam_profile_single_opcode_set(int unit,
                                       SOC_PPC_OAM_MEP_PROFILE_DATA *profile_data,
                                       uint8 opcode, bcm_oam_endpoint_action_t *action,
                                       uint32 trap_code,
                                       uint8 trap_strength,
                                       uint8 snoop_strength) {
   uint32   trap_id_err_level;
   int      rv  = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable)) {
      SHR_BITCLR(profile_data->counter_disable, opcode);
   } else {
      SHR_BITSET(profile_data->counter_disable, opcode);
   }
   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMeterEnable)) {
      SHR_BITCLR(profile_data->meter_disable, opcode);
   } else {
      SHR_BITSET(profile_data->meter_disable, opcode);
   }

   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwd) && trap_code) {
      profile_data->opcode_to_trap_code_unicast_map[opcode] = trap_code;
      /* Set user provided trap strength only for trap code given by user */
      profile_data->opcode_to_trap_strength_unicast_map[opcode] = trap_strength;
      profile_data->opcode_to_snoop_strength_unicast_map[opcode] = snoop_strength;
   }
   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwd) && trap_code) {
      profile_data->opcode_to_trap_code_multicast_map[opcode] = trap_code;
      /* Set user provided trap strength only for trap code given by user */
      profile_data->opcode_to_trap_strength_multicast_map[opcode] = trap_strength;
      profile_data->opcode_to_snoop_strength_multicast_map[opcode] = snoop_strength;
   }

   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcDrop)) {
      rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);
      BCMDNX_IF_ERR_EXIT(rv);
      profile_data->opcode_to_trap_code_unicast_map[opcode] = trap_id_err_level;
      /* Set trap strength to default for default trap codes */
      profile_data->opcode_to_trap_strength_unicast_map[opcode] = _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH;
      profile_data->opcode_to_snoop_strength_unicast_map[opcode] = 0;
   }
   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcDrop)) {
      rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);
      BCMDNX_IF_ERR_EXIT(rv);
      profile_data->opcode_to_trap_code_multicast_map[opcode] = trap_id_err_level;
      /* Set trap strength to default for default trap codes */
      profile_data->opcode_to_trap_strength_multicast_map[opcode] = _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH;
      profile_data->opcode_to_snoop_strength_multicast_map[opcode] = 0;
   }

   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionUcFwdAsData)) {
      profile_data->opcode_to_trap_code_unicast_map[opcode] = _BCM_PETRA_UD_DFLT_TRAP;
      /* Set trap strength to default for default trap codes */
      profile_data->opcode_to_trap_strength_unicast_map[opcode] = _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH;
      profile_data->opcode_to_snoop_strength_unicast_map[opcode] = 0;
   }
   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionMcFwdAsData)) {
      profile_data->opcode_to_trap_code_multicast_map[opcode] = _BCM_PETRA_UD_DFLT_TRAP;
      /* Set trap strength to default for default trap codes */
      profile_data->opcode_to_trap_strength_multicast_map[opcode] = _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH;
      profile_data->opcode_to_snoop_strength_multicast_map[opcode] = 0;
   }

   /* Relevant for RFC-6374 endpoints */
   if (BCM_OAM_ACTION_GET(*action, bcmOAMActionFwd) && trap_code) {
      profile_data->opcode_to_trap_code_multicast_map[opcode] = trap_code;
      /* Set user provided trap strength only for trap code given by user */
      profile_data->opcode_to_trap_strength_multicast_map[opcode] = trap_strength;
      profile_data->opcode_to_snoop_strength_multicast_map[opcode] = 0;
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

uint32 _bcm_oam_mep_exist_on_lif_and_direction( int unit,
                                                SOC_PPC_LIF_ID lif,
                                                int is_upmep,
                                                int find_mip,
                                                uint8 *found,
                                                uint8 *is_mip,
                                                uint32 *profile)
{

   uint8 __mdl;
   uint8 __found_profile;
   uint8 __is_mp_type_flexible_dummy;
   uint32 soc_sand_rv;
   
   BCMDNX_INIT_FUNC_DEFS;

   if (is_mip==NULL) {
      BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
   }

   for (__mdl=0; __mdl<=7; __mdl++) {
     soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                  lif,
                                                                                   __mdl,
                                                                                   is_upmep,
                                                                                   found,
                                                                                   profile,
                                                                                   &__found_profile,
                                                                                   &__is_mp_type_flexible_dummy,
                                                                                    is_mip);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      if (!__found_profile) {
              break;
      }
      if (*found && (!find_mip || *is_mip)) {
              break;
      }
  }
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/*
The following structs are used for event fifo implementation. 
The implementation is as such: 
    The hash table is used to store the multi-event indication and potentially reference counter
    The following linked list is then itereated on in the end.
When an event is encounter, look for it in the hash table. 
    if it exists, update the reference counter in the hash table and set the multi event indication
    else insert it into the linked list and the hash table.
 
In the end the events are popped one by one (fifo) from the hash table and the reference counter and multi-event indication is gotten from the hash table
*/
typedef struct  _bcm_oam_eventt_Node_s{
   _bcm_oam_event_and_rmep_info eventt;
   struct _bcm_oam_eventt_Node_s * next;
}_bcm_oam_eventt_Node_t;

typedef struct  {
   _bcm_oam_eventt_Node_t * head;
   _bcm_oam_eventt_Node_t * tail;
}_bcm_oam_eventt_list_t;


/**
 * Initialize the linked list (fifo) to empty value.
 * 
 * @author sinai (24/08/2016)
 * 
 * @param event_list 
 */
void _bcm_oam_eventt_list_init(_bcm_oam_eventt_list_t * event_list) {
   event_list->head = event_list->tail = NULL;
}

/**
 * Push an entry to the end of the linked list
 * 
 * @author sinai (24/08/2016)
 * 
 * @param event_list -linked list on which entry is added
 * @param eventt entry - to get pushed.
 * 
 * @return int BCM_E_MEMORY if memory allocation fails, 
 *         BCM_E_NONE otherwise.
 */
int _bcm_oam_eventt_list_push(_bcm_oam_eventt_list_t *event_list, const _bcm_oam_event_and_rmep_info *eventt) {
    _bcm_oam_eventt_Node_t *new_node = sal_alloc(sizeof(_bcm_oam_eventt_Node_t), "oam event fifo");
    if (new_node == NULL) {
        return  BCM_E_MEMORY;
    }
    new_node->next = NULL;
    new_node->eventt = *eventt;
    if (event_list->head != NULL) {
       event_list->tail->next = new_node;
       event_list->tail = new_node;
    } else {
       event_list->head = event_list->tail = new_node;
    }
    return BCM_E_NONE;
}

/**
 * Pop an event from the front of the linked list (fifo)
 * 
 * @author sinai (24/08/2016)
 * 
 * @param event_list - linked list where event is pushed.
 * @param eventt - event is set here.
 * 
 * @return int BCM_E_EMPTY if linked list is empty, BCM_E_NONE 
 *         otherwise.
 */
int _bcm_oam_eventt_list_pop(_bcm_oam_eventt_list_t * event_list, _bcm_oam_event_and_rmep_info * eventt) {
   if (event_list->head) {
      _bcm_oam_eventt_Node_t *temp_head = event_list->head; 
      *eventt = event_list->head->eventt;
      if (event_list->head == event_list->tail) {
          event_list->head = event_list->tail = NULL;
      } else {
          event_list->head = event_list->head->next; 
      }
      sal_free(temp_head);
      return BCM_E_NONE;
   } else {
      return BCM_E_EMPTY;
   }
}

/**
 * Insert an event to the event hash table. 
 * If specific event has already been inserted, update the data
 * to signify multiple events 
 *  
 * Insert event to a fifo only once per event keeping the order. 
 * Keep track of the events on hash table and manage the 
 * occurance counter for each event. 
 *
 * Use a custom SOC property to allow this approach only for BFD
 * and to avoid using BCM_OAM_EVENT_FLAGS_MULTIPLE for OAM events:
 * oam_multiple_event_count_enable = 1 means record the counter for OAM
 * oam_multiple_event_count_enable = 2 means do not use of BCM_OAM_EVENT_FLAGS_MULTIPLE for OAM
 */
int _bcm_dpp_oam_insert_event_into_htbl_and_fifo(int unit, shr_htb_hash_table_t htbl, _bcm_oam_eventt_list_t *event_list,  _bcm_oam_event_and_rmep_info *eventt) {

    int rv;
    int count_enable;
    BCMDNX_INIT_FUNC_DEFS;

    count_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_multiple_event_count_enable", 0);



    rv = shr_htb_insert(htbl,  (shr_htb_key_t)eventt, (shr_htb_data_t)0);
    /* for implementation details see comment above _bcm_oam_eventt_Node_t*/
    if (count_enable == 2 && eventt->is_oam) {
        /* in this special case add to the fifo in any case.*/
        /** OAM and no MULTIPLE flag/count enabled. Per the above, the
        *   event was already inserted in hash-table,
        *  here, we don't care about the event count, so just insert
        *  event to list to make sure the event is recorded
       */

       if (rv != _SHR_E_EXISTS) {
          BCMDNX_IF_ERR_EXIT(rv);
       }
        rv = _bcm_oam_eventt_list_push(event_list, eventt);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (rv == _SHR_E_EXISTS ) {
       /* normal case, adding a second event. update the existing event*/
       shr_htb_data_t data;
        int count;

        rv = shr_htb_find(htbl, (shr_htb_key_t)eventt, &data, 1 /* remove */);
        BCMDNX_IF_ERR_EXIT(rv);
        count = PTR_TO_INT(data);
        if (count_enable) {
            count = (count >> 16) + 1;
        } else {
            count = 0;
        }
        rv = shr_htb_insert(htbl, (shr_htb_key_t)eventt, INT_TO_PTR((count << 16) | BCM_OAM_EVENT_FLAGS_MULTIPLE));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* Normal case, adding first event. Insert event to FIFO as well */
        rv = _bcm_oam_eventt_list_push(event_list, eventt);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* multiple by 3.5 */
void multiply_by_3_5(uint32 *_whole,uint32 *_thousandth)
{
    uint32 _res_whole;
    uint32 _res_thousandth;
    _res_whole = ((*_whole) * 35) / 10;
    _res_thousandth = ((*_thousandth) * 35) / 10;
    if((((*_whole) * 35) % 10) > 0) {
        _res_thousandth += ((*_whole) * 35) % 10;
    }
    if(_res_thousandth > 1000) {
        _res_whole += _res_thousandth / 1000;
        _res_thousandth = _res_thousandth % 1000;
    }
    (*_whole) = _res_whole;
    (*_thousandth) = _res_thousandth;
}

/*
*  Sets an action. Opcode in action should be set beforehand.
*  opcode_map should be on of the SOC_PPD_OAM_OPCODE_MAP_XXX enum (for meter enable/ counter enable).
*/
int set_classifier_action_general(int unit, int mep_id, bcm_oam_endpoint_action_t *action, int opcode_map, SOC_PPC_OAM_MEP_TYPE mep_type) {
    SOC_PPC_OAM_LIF_PROFILE_DATA lif_profile;
    int                          rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, mep_id, &lif_profile);
    BCM_OAM_ACTION_CLEAR_ALL(*action);
    action->flags = 0;
    if (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
       BCM_OAM_ACTION_SET(*action, bcmOAMActionUcFwd);
    } else {
       BCM_OAM_ACTION_SET(*action, bcmOAMActionMcFwd);
    }
    if (!SHR_BITGET(lif_profile.mep_profile_data.counter_disable, opcode_map)) {
       BCM_OAM_ACTION_SET(*action, bcmOAMActionCountEnable);
    }
    if (!SHR_BITGET(lif_profile.mep_profile_data.meter_disable, opcode_map)) {
       BCM_OAM_ACTION_SET(*action, bcmOAMActionMeterEnable);
    }
    rv = _bcm_oam_endpoint_action_set_validity_check(unit, mep_id, action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_petra_oam_endpoint_action_set_internal(unit, mep_id, action, 2/*ignore*/, 2/*ignore*/);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Condition for failure in Arad+ : one rate is at 1 and another rate is postive. */
/* Condition for failure in jericho:
   [ (number of packets to be sent with interval 1) * 3 +  (number of packets to be sent with interval 2) + (number of packets to be sent with interval >2) / 60  ] > 6
  In Jericho calculate the phase as well.
   Assumes Opcode_N_ use low transmission rates.
    The pahse is calculated in the following manner:
    (if CCM rate = 1, as an example):
    CCM --> could be phase 0 or phase 1(in QAX only)
    highest of LM/DM --> phase 1 (for case that ccm use phase 1, LM/DM will use phase 0)
    Lowerst of LM/DM --> phase 3 (for case that ccm use phase 1, LM/DM will use phase 2)
    opcode_0  -->  331  (sufficiently high prime)
    opcode_1 -->  499   (sufficiently high prime)
   */
int _oam_oamp_verify_maximal_tx_per_mep(int                                    unit,
                                        uint8                                  ccm_interval,
                                        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  existing_eth1731_profile,
                                        unsigned int                           new_rate,
                                        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  *new_eth1731_mep_profile,
                                        uint8                                  is_dm) {

    /* define phase for ccm_transmit */
    int ccm_transmit_phase_is_odd = 0;
    BCMDNX_INIT_FUNC_DEFS;

   if (SOC_IS_JERICHO(unit)) {
      int   tx_per_24_scans;
      int   old_rate = is_dm ? existing_eth1731_profile.lmm_rate : existing_eth1731_profile.dmm_rate;

      tx_per_24_scans = _OAM_RATE_TO_TX_PER_24_SCANS(ccm_interval);
      tx_per_24_scans +=  _OAM_RATE_TO_TX_PER_24_SCANS(old_rate);
      tx_per_24_scans +=  _OAM_RATE_TO_TX_PER_24_SCANS((new_rate));
      tx_per_24_scans += _OAM_RATE_TO_TX_PER_24_SCANS(existing_eth1731_profile.opcode_0_rate);
      tx_per_24_scans += _OAM_RATE_TO_TX_PER_24_SCANS(existing_eth1731_profile.opcode_1_rate);
      if (tx_per_24_scans > 24) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Unsupported number of packets requested to be transmitted from a single MEP.")));
      }
      /* This function is called for LM/DM/LB cases only. So copy
       * ccm_cnt from existing profile.
       */
      new_eth1731_mep_profile->ccm_cnt = existing_eth1731_profile.ccm_cnt;
      ccm_transmit_phase_is_odd = existing_eth1731_profile.ccm_cnt & 1;
      /* Now set the phase*/
      if(_BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)){
          if (ccm_interval >= 2) {
             /* 
              * DM phase was allocated in advance, don't change it here again 
              * Jumbo TLV: CCM>=3.33ms DMM/DMR>=100ms LMM/LMR>=100ms
              */
             new_eth1731_mep_profile->dmm_cnt = existing_eth1731_profile.dmm_cnt;
             if (existing_eth1731_profile.lmm_rate || new_eth1731_mep_profile->lmm_rate) {
                 new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 3: 2;
             }
          } 
          else {
            /*
              * CCM take 1,3,5,7,9,11,13,15... DM: 0,2,4,6,8,10,12,14... LM: can take 16
              * CCM take 0,2,4,6,8,10,12,14... DM: 1,3,5,7,9,11,13,15... LM: can take 17
             */
             new_eth1731_mep_profile->dmm_cnt = existing_eth1731_profile.dmm_cnt;
             if (existing_eth1731_profile.lmm_rate || new_eth1731_mep_profile->lmm_rate) {
                 new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 16 : 17;
             }
          }
          BCM_EXIT;
      }

      if (ccm_interval >= 2) {
         /* this case is easy*/
         if (existing_eth1731_profile.dmm_rate == 1 || new_eth1731_mep_profile->dmm_rate == 1) {
            new_eth1731_mep_profile->dmm_cnt =  (ccm_transmit_phase_is_odd) ? 0 : 1;
            if (existing_eth1731_profile.lmm_rate || new_eth1731_mep_profile->lmm_rate) {
                new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
         } else {
             if (existing_eth1731_profile.dmm_rate || new_eth1731_mep_profile->dmm_rate) {
                 new_eth1731_mep_profile->dmm_cnt =  (ccm_transmit_phase_is_odd) ? 2 : 3;
             }
             if (existing_eth1731_profile.lmm_rate || new_eth1731_mep_profile->lmm_rate) {
                 new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
             }
         }
      } else {
         if (((old_rate == 0) && is_dm) || ((new_rate) >= old_rate && is_dm && old_rate) || (old_rate > (new_rate) && !is_dm && old_rate)) {
            new_eth1731_mep_profile->dmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
         }
         if (((old_rate == 0) && !is_dm) || ((new_rate) >= old_rate && !is_dm && old_rate) || (old_rate > (new_rate) && is_dm && old_rate)) {
            new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
         }
         if ((old_rate && is_dm && (new_rate) >= old_rate) || (old_rate && !is_dm && (new_rate) < old_rate)) {
            new_eth1731_mep_profile->lmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
         }
         if ((old_rate && !is_dm && (new_rate) >= old_rate) || (old_rate && is_dm && (new_rate) < old_rate)) {
            new_eth1731_mep_profile->dmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
         }
      }
        
      if (existing_eth1731_profile.opcode_1_rate || new_eth1731_mep_profile->opcode_1_rate) {
          new_eth1731_mep_profile->op_1_cnt = (ccm_transmit_phase_is_odd) ? 4 : 5; /*fifth slot modulo 6 is always available. */
      }
      if (existing_eth1731_profile.opcode_0_rate || new_eth1731_mep_profile->opcode_0_rate) {
          new_eth1731_mep_profile->op_0_cnt = (ccm_transmit_phase_is_odd) ? 10 :11;
      }
   } else {
      if ((ccm_interval == 1 && (new_rate)) || ((new_rate) == 1 && ccm_interval)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Unsupported number of packets requested to be transmitted from a single MEP.")));
      }
   }
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_group_name_to_ma_name_struct_convert(int unit, uint8 *group_name, _bcm_oam_ma_name_t *ma_name_struct) {

    uint32 i;

    BCMDNX_INIT_FUNC_DEFS;

    /*fully flexible 48B MAID format*/
    if (ma_name_struct->name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE &&
        (SOC_IS_QAX(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)))) {
        int is_allocated;
        int rv;
        if (!SOC_IS_QAX(unit)) {
            rv = _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_alloc(unit, 0/*flags*/, group_name, &is_allocated, &ma_name_struct->index);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (SOC_IS_QAX(unit)) {
            sal_memcpy(ma_name_struct->name, group_name, BCM_OAM_GROUP_NAME_LENGTH);
        } else if (SOC_IS_JERICHO(unit)) {
            /*OAMP_PE_GEN_MEM [0-16383] entry size is 2B, we need 24 entries for each MAID*/
            ma_name_struct->index *= 24;
            if (is_allocated) {
                for (i = 0; i < 48; i += 2) {
                    int data = group_name[i];
                    data <<= 8;
                    data |= group_name[i + 1];
                    ma_name_struct->name[i] = group_name[i];
                    ma_name_struct->name[i + 1] = group_name[i + 1];
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, ma_name_struct->index + (i / 2), data));
                    BCMDNX_IF_ERR_EXIT(rv);
                }

            }
        } else { /*SOC is Arad+*/
            /*OAMP_PE_GEN_MEM [256] entry size is 1B, we need 48 entries for each MAID*/
            ma_name_struct->index *= 48;
            if (is_allocated) {
                for (i = 0; i < 48; i += 1) {
                    int data = group_name[i];
                    ma_name_struct->name[i] = group_name[i];
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, ma_name_struct->index + (i), data));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    } else {
        switch (group_name[0]) { /* MD Format */
        case 0x01: /* ICC/Short */
            switch (group_name[1]) {
            case 3: /*BCM_OAM_MA_NAME_FORMAT_SHORT*/
                if (group_name[2] != 2) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Short name format supports only length 2.\n"), group_name[2]));
                }
                ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_SHORT;
                break;
            case 0x20: /*BCM_OAM_MA_NAME_FORMAT_ICC_BASED*/
                if (group_name[2] != 13) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                        (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. Long name format supports only length 13.\n"), group_name[2]));
                }
                ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_ICC;
                break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Unsupported group name format: %d.\n"), group_name[0]));
                break;
            }
            for (i=0; i<group_name[2]; i++) {
                ma_name_struct->name[i] = group_name[i+3];
            }
            break;
        case 0x04: /* String based */
            if (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0) == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Error in group name. String based 11b MAID is disabled.\n")));
            }
            if (group_name[1] > 5) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. MD Length supports up to 5.\n"), group_name[1]));
            }
            if (group_name[8] > 6) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Error in group name. Unsupported length: %d. MA Length supports up to 6.\n"), group_name[8]));
            }
            ma_name_struct->name_type = _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES;
            /* Copy MD */
            for (i=0; i<6; i++) {
                ma_name_struct->name[i] = group_name[i+1];
            }
            /* Copy MA */
            for (i=0; i<7; i++) {
                ma_name_struct->name[i+6] = group_name[i+8];
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error: Error in group name. Unsupported name: %d.\n"), group_name[0]));
            break;
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_name_struct_to_group_name_convert(int unit, _bcm_oam_ma_name_t *ma_name_struct, uint8 *group_name) {

    uint32 i, next;

    BCMDNX_INIT_FUNC_DEFS;

    if (ma_name_struct->name_type < _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES) {
        /* Long (ICC) or short MAID */

        group_name[0] = 1;
        if (ma_name_struct->name_type == _BCM_OAM_MA_NAME_TYPE_SHORT) {
            group_name[1]=3;
            group_name[2]=2;
        }
        else {
            group_name[1]=0x20;
            group_name[2]=13;
        }

        for (i=0; i<group_name[2]; i++) {
            group_name[i+3] = ma_name_struct->name[i];
        }
        i=i+3;

        while (i<BCM_OAM_GROUP_NAME_LENGTH) {
            group_name[i]=0;
            i++;
        }
    }
    else if(ma_name_struct->name_type < _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
        /* 11 bytes string based MAID
         * Saved as {MD_Len[1B],MD[5B],MA_Len[1B],MA[6B]}
         * Should be extracted to:
         * {MD_Format=0x04,MD_Len,MD[5B],MA_Format=0x02,MA_Len,MA[6B]}
         */

        next = 0;

        /* Write MD Format (4) */
        group_name[next++] = 4;

        /* Copy MD */
        for (i = 0; i < 6; i++) {
            group_name[next++] = ma_name_struct->name[i];
        }

        /* Write MA Format (2) */
        group_name[next++] = 2;

        /* Copy MD */
        for (; i < 13; i++) {
            group_name[next++] = ma_name_struct->name[i];
        }
    } else { /* 48 bytes string based MAID*/
        for (i = 0; i < BCM_OAM_GROUP_NAME_LENGTH; i++) {
            group_name[i] = ma_name_struct->name[i];
        }      
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Function sets the system port that goes on the ITMH for 
 * transmitted  packets from the OAMP. 
 * Shared by BFD and OAM Down MEP. 
 * 
 * @author sinai (29/04/2015)
 * 
 * @param unit 
 * @param gport 
 * @param sys_port 
 * 
 * @return int 
 */
int _bcm_dpp_oam_bfd_tx_gport_to_sys_port(int unit, bcm_gport_t tx_gport, uint32 * sys_port) {
    _bcm_dpp_gport_info_t gport_info;
    uint32 soc_sand_rv;
    int ret;
    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_TRUNK(tx_gport)) {
        /* The tx gport is a LAG group. extract the trunk ID and that goes on the ITMH*/

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_build_with_device, (unit, TRUE, BCM_GPORT_TRUNK_GET(tx_gport), 0, 0,sys_port)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else /* MC gport */
        if (BCM_GPORT_IS_MCAST(tx_gport) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_itmh_mc", 0)) {
            *sys_port = BCM_GPORT_MCAST_GET(tx_gport);
        } else {
            ret = _bcm_dpp_gport_to_phy_port(unit, tx_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
            BCMDNX_IF_ERR_EXIT(ret);
            *sys_port = gport_info.sys_port;
        }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Getting the destination from the ITMH in the OAMP, returning 
 * the tx_gport 
 * 
 * @author sinai (30/04/2015)
 * 
 * @param unit 
 * @param tx_gport 
 * @param sys_port 
 * 
 * @return int 
 */
int _bcm_dpp_oam_bfd_sys_port_to_tx_gport(int unit, bcm_gport_t * tx_gport, uint32 sys_port) {
    int ret;
    ARAD_DEST_INFO destination;
    uint8 is_lag_not_phys = 0;
    uint32  lag_id  = 0, lag_member_id, sys_phys_port_id,soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    arad_ARAD_DEST_INFO_clear(&destination);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_logical_sys_id_parse, (unit, sys_port & 0xffff,&is_lag_not_phys,&lag_id,&lag_member_id,&sys_phys_port_id)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (is_lag_not_phys) {
        destination.type = ARAD_DEST_TYPE_LAG;
        destination.id = lag_id;
        ret = _bcm_dpp_gport_from_tm_dest_info(unit, tx_gport, &destination);
        BCMDNX_IF_ERR_EXIT(ret);
    } else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(*tx_gport,sys_port );
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add mapping of local outlif to global outlif in lem, when advance egress snooping is enabled on Jericho */
int _bcm_dpp_oam_outlif_l2g_lem_entry_add(uint32 unit, int local_lif, int global_lif, int is_add) {
    int rv;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    BCMDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    payload.flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
    payload.dest = 0x10123;
    payload.asd = global_lif;

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_DSP_EXTENSION_AFTER_FTMH;
    qual_vals[0].val.arr[0]= local_lif;
    qual_vals[0].is_valid.arr[0]= 0xffff; /* outlif [15:0] */

    qual_vals[1].type= SOC_PPC_FP_QUAL_HDR_FTMH;
    qual_vals[1].val.arr[0]= (local_lif>>16)&0x7;
    qual_vals[1].is_valid.arr[0]= 0x7;    /* outlif_msb [18:16] */

    if (is_add) {
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM, qual_vals, 0, &payload, &success);
    } else {
        rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM, qual_vals, &success);
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Marking inlif as OAM LIF */
STATIC int _bcm_dpp_oam_lif_table_set(int unit, SOC_PPC_LIF_ID lif_index, uint8 value) {
    uint32 soc_sand_rv;
    int local_lif;
    int ret;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;

    BCMDNX_INIT_FUNC_DEFS;

    if (lif_index == _BCM_OAM_INVALID_LIF) {
        /*Invalid lif. simply exit.*/
        BCM_EXIT;
    }
    /* lif index is a global lif. translate to local lif.
       Only ingress here. */
    ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_index, &local_lif);
    BCMDNX_IF_ERR_EXIT(ret);

    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, local_lif, &lif_entry_info);

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (lif_entry_info.type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        lif_entry_info.value.ac.oam_valid = value;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        lif_entry_info.value.pwe.oam_valid = value;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_ISID:
        lif_entry_info.value.isid.oam_valid = value;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        lif_entry_info.value.ip_term_info.oam_valid = value;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        lif_entry_info.value.mpls_term_info.oam_valid = value;
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        lif_entry_info.value.extender.oam_valid = value;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index %d (extracted from gport)."), local_lif));
    }

    soc_sand_rv = soc_ppd_lif_table_entry_update(unit, local_lif, &lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Marking oulif as OAM LIF based on MEP type */
STATIC int _bcm_dpp_oam_outlif_table_set(int unit, SOC_PPC_LIF_ID local_lif, bcm_oam_endpoint_type_t ep_type, uint8 value)
{
    uint32 soc_sand_rv;
    SOC_PPC_EG_AC_INFO ac_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;

    BCMDNX_INIT_FUNC_DEFS;

    if (local_lif == _BCM_OAM_INVALID_LIF) {
        /*Invalid lif. simply exit.*/
        BCM_EXIT;
    }

    switch (ep_type) {
    case bcmOAMEndpointTypeBHHPwe:
    case bcmOAMEndpointTypeBHHPweGAL:
    case bcmOAMEndpointTypeMplsLmDmPw:
        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP,
                                                 local_lif, 1, encap_entry_info, next_eep,
                                                 &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        encap_entry_info->entry_val.pwe_info.oam_lif_set = value;

        soc_sand_rv = soc_ppd_eg_encap_pwe_entry_add(unit, local_lif, &encap_entry_info->entry_val.pwe_info, next_eep[0]);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmOAMEndpointTypeBHHMPLS:
    case bcmOAMEndpointTypeMplsLmDmLsp:
        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                                 local_lif, 1, encap_entry_info, next_eep,
                                                 &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        encap_entry_info->entry_val.mpls_encap_info.oam_lif_set = value;

        soc_sand_rv = soc_ppd_eg_encap_mpls_entry_add(unit, local_lif, &encap_entry_info->entry_val.mpls_encap_info, next_eep[0]);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmOAMEndpointTypeBhhSection:
    case bcmOAMEndpointTypeMplsLmDmSection:
        soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_LL,
                local_lif, 1, encap_entry_info, next_eep,
                &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        encap_entry_info->entry_val.ll_info.oam_lif_set = value;

        soc_sand_rv = soc_ppd_eg_encap_ll_entry_add(unit, local_lif, &encap_entry_info->entry_val.ll_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;

    case bcmOAMEndpointTypeEthernet:
        soc_sand_rv = soc_ppd_eg_ac_info_get(unit, local_lif, &ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        ac_info.edit_info.oam_lif_set = value;

        soc_sand_rv = soc_ppd_eg_ac_info_set(unit, local_lif, &ac_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OAM MEP type not supported"), local_lif));
        break;
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_dpp_oam_set_counter(int unit, int counter_base, uint8 is_pcp, uint32 lif)
{
   uint32 soc_sand_rv;
   uint32 counter_range_min, counter_range_max;

   BCMDNX_INIT_FUNC_DEFS;

   if (SOC_IS_JERICHO(unit)) {
      soc_sand_rv = soc_ppd_oam_counter_range_get(unit, &counter_range_min, &counter_range_max);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      if (is_pcp) { /* if min==max==0 it must be is_pcp */
         if ((!((counter_range_min == 0) && (counter_range_max == 0))) &&
             (counter_range_min <= counter_base) &&
             (counter_range_max >= counter_base)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: lm_counter_base_id %d should be outside the counter-singleton range (%d-%d).\n"), counter_base,counter_range_min,counter_range_max));
         }
         /* For PCP 8 counters used for every counter_base - counter_base should be aligned to 8 */
         if ((counter_base & 0x7) != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: For PCP, lm_counter_base_id %d should be aligned to 8.\n"), counter_base,counter_range_min,counter_range_max));
         }
      } else {
         /* if min==max==0 it must be is_pcp */
         if (((counter_range_min == 0) && (counter_range_max == 0)) ||
             (counter_range_min > counter_base) ||
             (counter_range_max < counter_base)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: lm_counter_base_id %d should be inside the counter-singleton range (%d-%d).\n"), counter_base,counter_range_min,counter_range_max));
         }
      }
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * Sets the relevant counters of an endpoint. 
 * LIF reperesents the global (in) LIF (passive side for Up MEP). 
 *
 * NOTE: When set_value is 0, it means OAM LIF need to be reset to 0.
 * In such a case, do not bother about validity checks of counter base id
 * and setting counters to 0.
 */
STATIC int _bcm_dpp_endpoint_counters_set(int unit, bcm_oam_endpoint_info_t *endpoint_info, SOC_PPC_LIF_ID inlif, SOC_PPC_LIF_ID outlif, int set_value)
{
    int rv;



    SOC_TMC_CNT_CUSTOM_MODE_PARAMS custom_mode_params;

    bcm_oam_endpoint_type_t type = bcmOAMEndpointTypeEthernet; 

 

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&custom_mode_params, 0x0, sizeof(SOC_TMC_CNT_CUSTOM_MODE_PARAMS));

    if (endpoint_info->lm_counter_base_id > 0) {
        /* Set in LIF as OAM LIF */
        rv = _bcm_dpp_oam_lif_table_set(unit, inlif, set_value);
        BCMDNX_IF_ERR_EXIT(rv);
        /* Set out LIF as OAM LIF */
        if(SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)){
            rv = _bcm_oam_y1711_lm_validity_checks(unit, endpoint_info);
            BCMDNX_IF_ERR_EXIT(rv);
            type = (endpoint_info->mpls_network_info.function_type == SOC_PPC_OAM_Y1711_LM_MPLS) ? bcmOAMEndpointTypeBHHMPLS : bcmOAMEndpointTypeBHHPwe;
        }else{
            type = endpoint_info->type;
        }
        rv = _bcm_dpp_oam_outlif_table_set(unit, outlif, type, set_value);
        BCMDNX_IF_ERR_EXIT(rv);

        if (set_value) {
            rv = _bcm_dpp_oam_set_counter(unit, endpoint_info->lm_counter_base_id, (endpoint_info->lm_flags & BCM_OAM_LM_PCP), inlif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (SOC_IS_JERICHO(unit)) {
        /* Just set the OAM LIF*/
        rv = _bcm_dpp_oam_lif_table_set(unit, inlif, set_value);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * _bcm_dpp_oam_core_from_lif_get
 *
 * Gets the relevant core ID from a global lif.
 */
int _bcm_dpp_oam_core_from_lif_get(int unit, uint32 global_lif, uint32 *core){

    bcm_error_t rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    bcm_gport_t gport;
    uint32      pp_port;
    uint32 lif2gport_flags = (_BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF |
        _BCM_DPP_LIF_TO_GPORT_INGRESS);

    BCMDNX_INIT_FUNC_DEFS;


    rv = _bcm_dpp_lif_to_gport(unit, global_lif, lif2gport_flags, &gport);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, (int*)core));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Build the entries needed to calculate CRC
 * for QAX 48-byte MAID 
 */
int _bcm_oam_init_48B_MAID_Entries(int unit) {
   SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO flex_ver_info;
   bcm_error_t rv = BCM_E_NONE;
   uint32 pe_prof;

   BCMDNX_INIT_FUNC_DEFS;

   /* Configure for 48 Byte MAID CRC masking */
   flex_ver_info.mep_idx = -1; /* A general setting, for multiple MEPs */
   sal_memset(flex_ver_info.crc_info.mask.msb_mask, 0, SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE); /* First 8 bytes */
   /* Calculation CRC for bytes 10 - 57, mask everything else */
   flex_ver_info.crc_info.mask.lsbyte_mask[0] = 0; /* Order of masking reversed - last 32 bytes masked */
   flex_ver_info.crc_info.mask.lsbyte_mask[1] = 0; /* 32 bytes masked */
   flex_ver_info.crc_info.mask.lsbyte_mask[2] = 0xFFFFFFC0; /* 26 bytes not masked, 6 bytes masked */
   flex_ver_info.crc_info.mask.lsbyte_mask[3] = 0x003FFFFF; /* 2 bytes masked, 22 bytes not masked */
   flex_ver_info.crc_tcam_info.opcode_bmp = SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_CCM_CCM_LM; /* Use this program for CCM messages */;
   flex_ver_info.crc_tcam_info.opcode_bmp_mask = 0;
   flex_ver_info.crc_tcam_info.oam_bfd = 1; /* 1 - OAM, 0 - BFD */
   flex_ver_info.crc_tcam_info.oam_bfd_mask = 0;
   /* flexible MAID 48 byte - mep pe profile is used for the OAMP PRGE program selection */
   MBCM_PP_DRIVER_CALL_VOID(unit, mbcm_pp_oamp_pe_program_profile_get, (unit, ARAD_PP_OAMP_PE_PROGS_MAID_48, &pe_prof));

   flex_ver_info.crc_tcam_info.mep_pe_profile = pe_prof;
   flex_ver_info.crc_tcam_info.mep_pe_profile_mask = 0;

   flex_ver_info.crc_tcam_info.crc_select = 0; 
   flex_ver_info.crc_tcam_index = QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID;
   flex_ver_info.crc_tcam_info.mask_tbl_index = 0;
   flex_ver_info.mask_tbl_index = QAX_OAMP_FLEX_VER_MASK_TEMP_48_BYTE_MAID;
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_flexible_verification_set, (unit, &flex_ver_info));
   BCMDNX_IF_ERR_EXIT(rv);

   /* Set TCAMs for the 4 ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_48_MAID_QAX program's pe profiles */
   flex_ver_info.mask_tbl_index = -1; /* No need for more masks - just use the one */
   MBCM_PP_DRIVER_CALL_VOID(unit, mbcm_pp_oamp_pe_program_profile_get, (unit, ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_48_MAID_QAX, &pe_prof));
   flex_ver_info.crc_tcam_info.mep_pe_profile = pe_prof;
   flex_ver_info.crc_tcam_info.mep_pe_profile_mask = 0x7; /* The masked bits allow two parameters: bits 0-1 for out-lif, bit 2 for SLM yes/no */
   flex_ver_info.crc_tcam_index = QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID_CCM_COUNT;
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_flexible_verification_set, (unit, &flex_ver_info));
   BCMDNX_IF_ERR_EXIT(rv);

   /* Set TCAM for the ARAD_PP_OAMP_PE_PROGS_EGR_INJ_48_MAID_QAX program's pe profile */
   flex_ver_info.mask_tbl_index = -1; /* Again, the same mask is fine */
   MBCM_PP_DRIVER_CALL_VOID(unit, mbcm_pp_oamp_pe_program_profile_get, (unit, ARAD_PP_OAMP_PE_PROGS_EGR_INJ_48_MAID_QAX, &pe_prof));
   flex_ver_info.crc_tcam_info.mep_pe_profile = pe_prof;
   flex_ver_info.crc_tcam_info.mep_pe_profile_mask = 0x7; /* The masked bits allow two parameters: bits 0-1 for out-lif, bit 2 for SLM yes/no */
   flex_ver_info.crc_tcam_index = QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID_EGRESS_INJ;
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_flexible_verification_set, (unit, &flex_ver_info));
   BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Utility function: in a give buffer, reverse the order 
 * of every 16 byte segment 
 */
int _bcm_oam_buffer_piecewise_reverse(int unit, uint8 *dst_buffer, uint8 *src_buffer, int len)
{
    int piece_index, byte_index, pieces;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(dst_buffer);
    BCMDNX_NULL_CHECK(src_buffer);

    pieces = (len >> 4);
    for (piece_index = 0; piece_index < pieces; piece_index++) {
        for (byte_index = 0; byte_index < 16; byte_index++) {
            dst_buffer[(piece_index << 4) + byte_index] = src_buffer[(piece_index << 4) + 15 - byte_index];
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Write the MEG info to the extra data entries
 */
int _bcm_oam_write_group_data_to_oamp_mep_db(int unit, bcm_oam_group_info_t *group_info) {
   bcm_error_t rv = BCM_E_NONE;
   SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO oamp_db_group_info;
    SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO maid_CRC_info;
   SOC_PPC_OAM_OAMP_CRC_MASK CRC_Calc_Mask;
    uint16 Calculated_CRC16;
   _BCM_OAM_OAMP_CRC_BUFFER CRC_place_buff, CRC_calc_buff;
   int i, word_index;
   uint32 Entry_Index = group_info->group_name_index;
   uint8 *CRC_Calc_Src;

   BCMDNX_INIT_FUNC_DEFS;

   /* Calculate the CHECK_CRC_VALUE1 field
      Assuming the CRC calculation starts at the start of the PDU, the calculcation should
      be performed on the MAID field, bytes 10-57.  All other bytes should be masked.
      For the function used, 0 means bit/byte masked and 1 means unmasked */
   sal_memset(CRC_Calc_Mask.msb_mask, 0xFF, sizeof(CRC_Calc_Mask.msb_mask)); /* Mask the first 8 bytes */
   CRC_Calc_Mask.lsbyte_mask[0] = 0xFFFFFFFF; 
   CRC_Calc_Mask.lsbyte_mask[1] = 0xFFFFFFFF; 
   CRC_Calc_Mask.lsbyte_mask[2] = 0xFFFFFFFF; 
   CRC_Calc_Mask.lsbyte_mask[3] = 0xFFFFFFFF; 
   sal_memset(&CRC_place_buff, 0, sizeof(CRC_place_buff));

   CRC_Calc_Src = (group_info->flags & BCM_OAM_GROUP_RX_NAME) ? group_info->rx_name : group_info->name;

   /* Copy the MAID to the correct offset in the buffer */
   sal_memcpy(&CRC_place_buff[10], CRC_Calc_Src, _BCM_OAM_MA_NAME_48);

   /* Create the piecewise-reversed buffer */
   _bcm_oam_buffer_piecewise_reverse(unit, CRC_calc_buff, CRC_place_buff, sizeof(CRC_calc_buff));

   _bcm_dpp_oam_bfd_flexible_verification_expected_crc16_get(CRC_calc_buff, &CRC_Calc_Mask, &Calculated_CRC16);
   maid_CRC_info.crc_info.crc16_val1 = Calculated_CRC16;
   maid_CRC_info.crc_info.crc16_val2 = 0;

   if ((group_info->flags & BCM_OAM_GROUP_REPLACE) == 0) {
       /* Reserve the necessary entries */
       for (i = 0; i < 3; i++, Entry_Index += (SOC_IS_QUX(unit) ? 0x100 : 0x400)) {
           rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, 0, 0, 0, &Entry_Index);
           BCMDNX_IF_ERR_EXIT(rv);
       }
   }

   /* Overwrite OAMP DB extended data entries */
   for (word_index = 0; word_index < (_BCM_OAM_MA_NAME_48 / sizeof(uint32)); word_index++) {
       /* Make sure the 32 bit dword array is arranged big-endian, no matter what CPU is used */
       oamp_db_group_info.data[word_index] = (group_info->name[word_index * 4] << 24) |
                                             (group_info->name[word_index * 4 + 1] << 16) |
                                             (group_info->name[word_index * 4 + 2] << 8) |
                                              group_info->name[word_index * 4 + 3];
   }
   oamp_db_group_info.mep_idx = -1; /* MEPs are left untouched */
   oamp_db_group_info.extension_idx = group_info->group_name_index;
   oamp_db_group_info.data_size_in_bits = 384; /* 48 bytes */
   oamp_db_group_info.opcode_bmp = SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_CCM_CCM_LM; /* Prepend 80 byte data to CCM messages */
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_mep_db_ext_data_set, (unit, &oamp_db_group_info));
   BCMDNX_IF_ERR_EXIT(rv);

   /* Overwrite the CHECK_CRC_VALUE1 and CHECK_CRC_VALUE2 fields */
   maid_CRC_info.mep_idx = group_info->group_name_index;
   maid_CRC_info.mask_tbl_index = -1;
   maid_CRC_info.crc_tcam_index = -1; /* Don't reset these for every group! */
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_bfd_flexible_verification_set, (unit, &maid_CRC_info));
   BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/***************************************************************/
/***************************************************************/

/*
 * Event handling
 */

int bcm_petra_oam_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_event_cb cb,
    void *user_data) {

    bcm_oam_event_type_t   event_i;
    int                    rv;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             SOC_PPC_OAM_EVENT event_ppd; /* Used to verify the registered event.*/
             if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
               rv = _bcm_oam_ppd_from_bcm_event_mep(&event_ppd , event_i, 1 /* is oam*/);
               BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Event ID is not supported: %d"), event_i));

             (void) event_ppd; /* For compilation*/
             _g_oam_event_cb[unit][event_i] = cb;
             _g_oam_event_ud[unit][event_i] = user_data;
         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Unregister a callback for handling OAM events */
int bcm_petra_oam_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_event_cb cb) {

    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_event_cb[unit][event_i] && (_g_oam_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_oam_event_cb[unit][event_i] = NULL;
             _g_oam_event_ud[unit][event_i] = NULL;

         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_performance_event_register(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb,
    void *user_data)
{
    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_performance_event_cb[unit][event_i] && (_g_oam_performance_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
             _g_oam_performance_event_cb[unit][event_i] = cb;
             _g_oam_event_ud[unit][event_i] = user_data;
         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_performance_event_unregister(
    int unit,
    bcm_oam_event_types_t event_types,
    bcm_oam_performance_event_cb cb)
{
    bcm_oam_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmOAMEventCount; event_i++) {
         if (BCM_OAM_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_oam_performance_event_cb[unit][event_i] && (_g_oam_performance_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_oam_performance_event_cb[unit][event_i] = NULL;
             _g_oam_event_ud[unit][event_i] = NULL;

         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_bfd_event_register(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb,
    void *user_data) {

    bcm_bfd_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;


    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
         if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is already registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("EVENT %d already has a registered callback"), event_i));
             }
             _g_bfd_event_cb[unit][event_i] = cb;
             _g_bfd_event_ud[unit][event_i] = user_data;
         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Unregister a callback for handling BFD events */
int _bcm_petra_bfd_event_unregister(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb) {

    bcm_bfd_event_type_t event_i;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
         if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
             if (_g_bfd_event_cb[unit][event_i] && (_g_bfd_event_cb[unit][event_i] != cb)) {
                 /* A different calblack is registered for this event. Return error */
                 BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("A different callback is registered for %d"), event_i));
             }
             _g_bfd_event_cb[unit][event_i] = NULL;
             _g_bfd_event_ud[unit][event_i] = NULL;

         }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*Register a callback for handling SAT events*/
int _bcm_jer_sat_event_register(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb,
    void *user_data) {
    
    bcm_sat_event_type_t type;

    BCMDNX_INIT_FUNC_DEFS;

    for (type = 0; type < bcmSATEventCount; type++) {
        if (type == event_type) {
            _g_sat_event_cb[unit][type] = cb;
            _g_sat_event_ud[unit][type] = user_data;
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Unregister a callback for handling SAT events */
int _bcm_jer_sat_event_unregister(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb) {
    
    bcm_sat_event_type_t type;

    BCMDNX_INIT_FUNC_DEFS;

    for (type = 0; type < bcmSATEventCount; type++) {
        if (type == event_type) {
            _g_sat_event_cb[unit][type] = NULL;
            _g_sat_event_ud[unit][type] = NULL;
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*SAT report handle*/
int bcm_oam_sat_ctf_report_process(int unit, uint32 *event_id,uint32 *event_data) {
    int rv;
    bcm_sat_report_event_data_t report;
    bcm_sat_event_type_t type = bcmSATEventReport;

    BCMDNX_INIT_FUNC_DEFS;

    if (!event_id || !event_data){
        rv = BCM_E_PARAM;
        BCMDNX_IF_ERR_EXIT(rv);
    }

    sal_memset(&report, 0, sizeof(bcm_sat_report_event_data_t));
    
    report.ctf_id = event_id[SOC_PPC_OAM_EVENT_SAT_FLOW_ID];    

    if (event_id[SOC_PPC_OAM_EVENT_REPORT_RX_DM]) {  /*delay report*/
        report.delay = event_data[0];
        report.delay_valid = 1;    
        
        LOG_VERBOSE(BSL_LS_BCM_OAM,
        (BSL_META_U(unit,
                    "SAT Delay Report: %d \n"),event_data[0]));
    }
    
    if (event_id[SOC_PPC_OAM_EVENT_REPORT_RX_LM]){ /*sequence report*/
        if (report.delay_valid == 1) {
            report.sequence_number = event_data[1];
        }
        else {
            report.sequence_number = event_data[0];
        }
        
        report.sequence_number_valid = 1;
        LOG_VERBOSE(BSL_LS_BCM_OAM,
        (BSL_META_U(unit,
                    "SAT Sequence Report: %d \n"),report.sequence_number ));
    }
    
    if (_g_sat_event_cb[unit][type]) {
        _g_sat_event_cb[unit][type](unit, type, (void *)&report, _g_sat_event_ud[unit][type]);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Function is intended for usage after inserting all the events
 * in the event hash table. Function gets information on each 
 * stored event and calls the proper call back with that 
 * information. 
 * 
 * @author sinai (11/11/2013)
 * 
 * @param unit 
 * @param really_the_event_data 
 * @param really_the_flags 
 * 
 * @return int 
 */
int _dpp_oam_fifo_interrupt_handle_user_callback(int unit, _bcm_oam_event_and_rmep_info *event_info,shr_htb_data_t really_the_flags ){
    int rv, rmeb_db_ndx=0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    bcm_bfd_event_types_t event_types;
    uint8 found;
    int flags = PTR_TO_INT(really_the_flags);
    BCMDNX_INIT_FUNC_DEFS;


    if (event_info->is_oam) {
        if (_g_oam_event_cb[unit][event_info->event_number]) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info->mep_or_rmep_indx, rmeb_db_ndx);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
               LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Endpoint already destroyed.\n")));
               BCM_EXIT; 
            }

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                LOG_WARN(BSL_LS_BCM_OAM,(BSL_META_U(unit,"Warning: Endpoint already destroyed.\n")));
                BCM_EXIT;
            }
            /*finally, the callback*/
            _g_oam_event_cb[unit][event_info->event_number](unit, flags, event_info->event_number, mep_info.ma_index, rmeb_db_ndx, _g_oam_event_ud[unit][event_info->event_number]);
        }
        else if (_g_oam_performance_event_cb[unit][event_info->event_number]) {
            bcm_oam_performance_event_data_t report_data;
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, event_info->mep_or_rmep_indx, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
               LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Endpoint already destroyed.\n")));
               BCM_EXIT; 
            }
            /* Fill a statistics report struct for the callback */
            bcm_oam_performance_event_data_t_init(&report_data);
            if (event_info->event_number == bcmOAMEventEndpointLmStatistics) { /* LM */
                report_data.delta_FCB = event_info->event_data[0];
                report_data.delta_FCf = event_info->event_data[1];
            }
            else { /* DM */
                COMPILER_64_SET(report_data.last_delay, (event_info->event_data[1]) << 2, (event_info->event_data[0])<<2);
            }
            /*finally, the callback*/
            _g_oam_performance_event_cb[unit][event_info->event_number](unit, event_info->event_number, mep_info.ma_index, event_info->mep_or_rmep_indx,
                                                                        &report_data, _g_oam_event_ud[unit][event_info->event_number]);
        }
    } else { /* bfd*/
        if (_g_bfd_event_cb[unit][event_info->event_number]) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info->mep_or_rmep_indx, rmeb_db_ndx);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                LOG_WARN(BSL_LS_BCM_OAM,(BSL_META_U(unit,"Warning: Endpoint already destroyed.\n")));
                BCM_EXIT;
            }

            BCM_BFD_EVENT_TYPE_CLEAR_ALL(event_types);
            BCM_BFD_EVENT_TYPE_SET(event_types, event_info->event_number);
            _g_bfd_event_cb[unit][event_info->event_number](unit, flags, event_types, rmep_info.mep_index, _g_bfd_event_ud[unit][event_info->event_number]);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/**
 * Function handling the interrupt. 
 * Should not be called directly, rather through callback in soc 
 * layer. 
 * 
 * @param unit 
 * @param event_type 
 * @param interrupt_data : "global" data used by soc layer. The 
 *                       reason this is not used as a global
 *                       variable is so that the interrupts will
 *                       be reentrable. The data is only global
 *                       per interrupt.
 * 
 * @return int 
 */
int _bcm_dpp_oam_fifo_interrupt_handler(int unit, SOC_PPC_OAM_DMA_EVENT_TYPE event_type, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * interrupt_data) {
    uint32 rmeb_db_ndx = 0;
    uint32 rmeb_db_ndx_internal, valid, soc_sand_rv;
    uint32 event_id_bitmap[SOC_PPC_OAM_EVENT_COUNT];
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    uint8 found;
    int rv;
    uint32 event_index;
    bcm_bfd_event_type_t bfd_event_type;
    bcm_oam_event_type_t oam_event_type;
    uint32 rmep_state;
    _bcm_oam_event_and_rmep_info event_found;
    shr_htb_data_t data;

    shr_htb_hash_table_t event_htbl = NULL;
    _bcm_oam_eventt_list_t event_list;
    int  fifo_read_counter = 0;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&event_found, 0, sizeof(_bcm_oam_event_and_rmep_info));

    /* Create event_list */
    _bcm_oam_eventt_list_init(&event_list);

    soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = shr_htb_create(&event_htbl,
                        OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW,
                        sizeof(_bcm_oam_event_and_rmep_info),
                        "OAM event hash table.");
    BCMDNX_IF_ERR_EXIT(rv);

    while (valid) {
        if (event_type == SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT) {
            if (rmeb_db_ndx_internal > SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit)) {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Internal error: event occured on non existent RMEP-index: %d. Discontinuing proccessing incoming events.\n")
                                        ,rmeb_db_ndx_internal));
                break;
            }

            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmeb_db_ndx_internal, rmeb_db_ndx);
            event_found.mep_or_rmep_indx = rmeb_db_ndx_internal;
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, rmeb_db_ndx,  &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                /* The only reasonable way for an RMEP not to be found is that it was destroyed after the interrupt has occured. In this case there is nothing to do.*/
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Warning: Processing event for MEP ID 0x%x failed. MEP not found. Continuing.\n"),rmeb_db_ndx));
                soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                continue;
            }
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG("Internal error in FIFO event processing: MEP index matching RMEP index wasn't found. rmep_db_ndx=%d, mep_db_ndx=%d"),
                                  rmeb_db_ndx, rmep_info.mep_index));
            }
        } else {
            if (rmeb_db_ndx_internal != SAT_REPORT_VALUE) {
                /* Other event types contain the OAM-ID as it arrives to the OAMP on the packet.
                           The 14 lsb of this value is the MEP DB index so there's no need to retrieve from the RMEP DB */
                event_found.mep_or_rmep_indx = rmeb_db_ndx_internal & ((1<<14)-1);
                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, event_found.mep_or_rmep_indx, &mep_info, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!found) {
                    /* The only reasonable way for an RMEP not to be found is that it was destroyed after the interrupt has occured. In this case there is nothing to do.*/
                    LOG_WARN(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit, 
                                         "Warning: Processing event for MEP ID 0x%x failed. MEP not found. Continuing.\n"),rmeb_db_ndx));
                    soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                    continue;
                }
            }
        }

        if (rmeb_db_ndx_internal != SAT_REPORT_VALUE) {
            if ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)  ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ) {
                event_found.is_oam =1;

                for (event_index = 0; event_index < SOC_PPC_OAM_EVENT_COUNT; event_index++) {
                    if (event_id_bitmap[event_index]) {
                        int event_ind=-1;
                        rv = _bcm_oam_ppd_to_bcm_oam_event_mep(unit, event_index, &oam_event_type, 1/*is_oam*/);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Event ID is not supported")));

                        rmep_state = event_found.event_data[0];
                        if (event_index == SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE) {
                            /* Check port status */
                            if (_BCM_OAM_PPD_EVENT_PORT_BLOCKED(rmep_state)) {
                                event_ind = bcmOAMEventEndpointPortDown;
                            }
                            else if (_BCM_OAM_PPD_EVENT_PORT_UP(rmep_state)) {
                                event_ind = bcmOAMEventEndpointPortUp;
                            }

                            /* If event found, insert to hash table */
                            if (event_ind !=-1) {
                                event_found.event_number = event_ind;
                                rv = _bcm_dpp_oam_insert_event_into_htbl_and_fifo(unit, event_htbl, &event_list, &event_found);
                                BCMDNX_IF_ERR_EXIT(rv);
                                /* reset "flag" */
                                event_ind =-1;
                            }

                            /* Check interfase status */
                            if (_BCM_OAM_PPD_EVENT_INTERFACE_UP(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceUp;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_DOWN(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceDown;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_TESTING(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceTesting;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_UNKNOWN(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceUnkonwn;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_DORMANT(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceDormant;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_NOT_PRESENT(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceNotPresent;
                            }
                            else if (_BCM_OAM_PPD_EVENT_INTERFACE_LL_DOWN(rmep_state)) {
                                event_ind = bcmOAMEventEndpointInterfaceLLDown;
                            }

                            /* If event found, insert to hash table */
                            if (event_ind !=-1) {
                                event_found.event_number = event_ind;
                                rv = _bcm_dpp_oam_insert_event_into_htbl_and_fifo(unit, event_htbl, &event_list, &event_found);
                                BCMDNX_IF_ERR_EXIT(rv);
                            }
                        }
                        else {
                            event_found.event_number = oam_event_type;
                            rv = _bcm_dpp_oam_insert_event_into_htbl_and_fifo(unit, event_htbl, &event_list, &event_found);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }

            } else { /* BFD */
                event_found.is_oam =0;
                for (event_index = 0; event_index < SOC_PPC_OAM_EVENT_COUNT; event_index++) {
                    if (event_id_bitmap[event_index]) {
                        rv = _bcm_oam_ppd_to_bcm_bfd_event_mep(event_index, &bfd_event_type, 0/*is_oam*/);
                        BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Event ID is not supported")));
                        if (bfd_event_type != -1) {
                            event_found.event_number = (uint8)bfd_event_type;
                            rv = _bcm_dpp_oam_insert_event_into_htbl_and_fifo(unit, event_htbl, &event_list, &event_found);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }
            }
        }
        else { /*for SAT */
            bcm_oam_sat_ctf_report_process(unit,event_id_bitmap, event_found.event_data);

        }

        /* Bailout condition - Preventing the while loop to run forever in case of interrupts arriving at a
           very high rate */
        if (++fifo_read_counter >= OAM_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW) {
           valid = 0;
           fifo_read_counter = 0;
        }

        soc_sand_rv = soc_ppd_oam_event_fifo_read(unit, event_type, &rmeb_db_ndx_internal, event_id_bitmap, &valid, event_found.event_data, interrupt_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    /*Empty the data bases where events were stored and call user callbacks.*/
    /* For complete implementation details see comment above _bcm_oam_eventt_Node_t*/
    rv = _bcm_oam_eventt_list_pop(&event_list,&event_found);
    while (rv != BCM_E_EMPTY){
        rv = shr_htb_find(event_htbl, &event_found, &data, 0 /* don't remove */ );
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_oam_fifo_interrupt_handle_user_callback(unit, &event_found, data);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_oam_eventt_list_pop(&event_list,&event_found);
    }

exit:

    if(event_htbl) {
        /* Empty event_htbl in case of error */
        rv = _bcm_oam_eventt_list_pop(&event_list,&event_found);
        while (rv != BCM_E_EMPTY){
            rv = shr_htb_find(event_htbl, &event_found, &data, 1 /* remove */ );
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_oam_eventt_list_pop(&event_list,&event_found);
        }
        rv = shr_htb_destroy(&event_htbl, NULL);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "shr_htb_destroy failed\n")));
        }
    }
    BCMDNX_FUNC_RETURN;
}

/* Enable oamp interrupts in case DMA is not enabled */
int _bcm_dpp_oamp_interrupts_init(int unit, uint8 enable_interrupts) {
    bcm_switch_event_control_t event_control;
    int rv,is_stat_event;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_WARM_BOOT(unit)) {
        /* Init OAM events and OAM stat events */
        for (is_stat_event = 0; is_stat_event <= 1; ++is_stat_event) {
            uint32 flags;
            event_control.event_id = _BCM_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit);

            /* If the DMA is configured do not init the interrupts of this event */
            if (SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma && !is_stat_event) {
                continue; 
            }

            /* If the DMA is configured do not init the interrupts of this event */
            if (SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma && is_stat_event) {
                continue;
            }

            if (is_stat_event) {
                if (SOC_IS_JERICHO(unit)) {
                    /* Enable Stat events in Jerichco */
                    event_control.event_id = _BCM_OAM_GET_OAMP_STAT_PENDING_EVENT_BY_DEVICE(unit);
                } else {
                    /* No such event in Arad*/
                    continue;
                }
            }

            rv = soc_interrupt_flags_get(unit, event_control.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rv);

            flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK;

            rv = soc_interrupt_flags_set(unit, event_control.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rv);

            event_control.index = 0;
            event_control.action = bcmSwitchEventMask;
            rv = bcm_petra_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, !enable_interrupts);
            BCMDNX_IF_ERR_EXIT(rv);

            if (enable_interrupts) {
                /* Disabling prints*/
                event_control.action = bcmSwitchEventLog;
                rv = bcm_petra_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, 0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * callback used for bcm_petra_oam_group_traverse()
 */
STATIC int
_bcm_petra_oam_group_traverse_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t user_data){
    int rv;
    bcm_oam_group_info_t group_info_for_cb;
    uint32 group_info_id = * (uint32*)key;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_group_info_t_init(&group_info_for_cb); 

    /* user_data here is the name of the oam group */
    rv = _bcm_dpp_oam_ma_name_struct_to_group_name_convert(unit, (_bcm_oam_ma_name_t*) user_data, (&group_info_for_cb)->name );
    BCMDNX_IF_ERR_EXIT(rv);
    group_info_for_cb.id = group_info_id;

    /* now call user callback. Everything but the name and key on the group_info _for_cb is 0 */
    rv = _group_cb_with_param.cb(unit, &group_info_for_cb, _group_cb_with_param.user_data);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * returns OAMP trap type based on endpoint type and direction
 */
STATIC int _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry, int* trap_code) {
    int rv;
    uint8 is_upmep;
    bcm_rx_trap_t trap_type;

    BCMDNX_INIT_FUNC_DEFS;

    is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);

    if (is_upmep) {
        trap_type = bcmRxTrapOamUpMEP2;
    } else {
        trap_type = classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM ? bcmRxTrapOamEthAccelerated :
                    classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP ? bcmRxTrapOamY1731MplsTp :
                    classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE ? bcmRxTrapOamY1731Pwe :
                    classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL ? bcmRxTrapOamY1731MplsTp:
                    classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION ? bcmRxTrapOamY1731Pwe:
                    bcmRxTrapOamEthAccelerated;
    }

    rv = bcm_rx_trap_type_get(unit, 0, trap_type, trap_code);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Update profile in SW state
   INPUT:
     Endpoint
     New Profile
     is_passive
*/
STATIC int bcm_oam_replace_mep_profile_in_db(int unit,
                                     bcm_oam_endpoint_info_t *endpoint_info ,
                                     int profile,
                                     int is_passive )
{

   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
   uint8 found;
   int rv;

   BCMDNX_INIT_FUNC_DEFS;

   /* Replace existing profile in endpoint SW DB */
   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
   BCMDNX_IF_ERR_EXIT(rv);
   if (!found) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: _bcm_oam_endpoint_exchange_profile_cb internal error.\n")));
   }
   /* it's the active profile of the existinfg MEP */
   if (is_passive) {
      classifier_mep_entry.non_acc_profile_passive = profile;
   } else {
      classifier_mep_entry.non_acc_profile = profile;
   }

   if (endpoint_info->timestamp_format) {
      classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588;
   } else {
      classifier_mep_entry.flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588;
   }

   rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info->id, &classifier_mep_entry);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/*
 * callbacks used for profile exchange
 */
/* SLM per LIF support:
 * There are 5 scenarios:
 * 1. No additional MEP found - Set ARAD_PP_SLM_PASSIVE_PROFILE on passive side - done in action set
 * 2. No MEP on passive side - set ARAD_PP_SLM_PASSIVE_PROFILE on passive side
 * 3. Profile ARAD_PP_SLM_PASSIVE_PROFILE on passive side - nop ( there is already other MEP allocated on the same side )
 * 4. Profile SLM active on passive side - nop ( there is already other SLM-MEP allocated on the opposite side )
 * 5. Profile allocated on passive side - exchange its profile with an SLM one.
*/
STATIC int _bcm_oam_endpoint_exchange_profile_cb( int unit,
                                                  bcm_oam_endpoint_info_t *endpoint_info,
                                                  void *user_data)
{
     bcm_oam_endpoint_traverse_cb_profile_info_t * profile_info = NULL;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data_old;
    int old_profile = 0, is_last = 0, profile = 0, is_allocated = 0;
    uint32 lif = 0;
    uint8 is_passive = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int global_in_lif = 0, local_out_lif = 0;
    uint32 flags = 0;
    int rv = 0;
    int is_passive_valid = 0;
    uint32 soc_sand_rv = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;

    profile_info = (bcm_oam_endpoint_traverse_cb_profile_info_t * )user_data;


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
        rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->gport, flags, &gport_hw_resources);
        profile_info->cb_rv += rv;
        BCMDNX_IF_ERR_EXIT(rv);

        global_in_lif = gport_hw_resources.global_in_lif;
        local_out_lif = gport_hw_resources.local_out_lif;
        lif = global_in_lif;
    } 
    
    
    if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID) {
        flags = _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK;

        rv = _bcm_dpp_gport_to_hw_resources(unit, endpoint_info->mpls_out_gport, flags, &gport_hw_resources);
        profile_info->cb_rv += rv;
        BCMDNX_IF_ERR_EXIT(rv);

        local_out_lif = gport_hw_resources.local_out_lif;
        /* MPLS Egress only OEM entries */
        if ((endpoint_info->gport == BCM_GPORT_INVALID) &&
            ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLS) ||
             (endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork) ||
             (endpoint_info->type == bcmOAMEndpointTypeBHHPwe) ||
             (endpoint_info->type == bcmOAMEndpointTypeBHHPweGAL) ||
             (endpoint_info->type == bcmOAMEndpointTypeBhhSection))) {
            lif =gport_hw_resources.global_in_lif;
        }
    }

    if (SOC_IS_JERICHO(unit) && !profile_info->is_ingress) {
        /* Egress OEM entries using local lif */
        lif = local_out_lif;
    }

    if (lif == profile_info->lif) {
       if (profile_info->profile_data) {
          /* MIP is never passive */
          if (_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)) {
             is_passive = !profile_info->is_ingress;
          } else {
             is_passive = !_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ^ profile_info->is_ingress;
             /* 
             SLM per lif support:
             We either need to set the passive side to ARAD_PP_SLM_PASSIVE_PROFILE, or in case there is already another MEP exists there,
             exchange the other MEP's profile with SLM one.
             */
             if (SOC_IS_JERICHO_PLUS(unit) && profile_info->profile_data->is_slm && is_passive) {
                /* check if the passive profile is valid */
                rv = _bcm_dpp_am_template_oam_mep_profile_data_validate(unit, endpoint_info->id, is_passive, &profile_data_old, &is_passive_valid);
                profile_info->cb_rv += rv;
                BCMDNX_IF_ERR_EXIT(rv);

                if (!is_passive_valid) {
                   BCM_EXIT;
                }
                profile_info->is_passive_profile_allocated = 1;   /* To track if there is no EP on passive side of the current EP. 
                                                                     In which case we need to alloc ARAD_PP_SLM_PASSIVE_PROFILE */
                if (is_passive_valid == 1) {
                   rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(unit, endpoint_info->id, is_passive, &old_profile);
                   BCMDNX_IF_ERR_EXIT(rv);
                   if (old_profile == ARAD_PP_SLM_PASSIVE_PROFILE) {
                      BCM_EXIT;
                   }
                   rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint_info->id, is_passive, &profile_data_old);
                   profile_info->cb_rv += rv;
                   BCMDNX_IF_ERR_EXIT(rv);

                   if (profile_data_old.is_slm == 1) {
                      /* There is also an active MEP with SLM on this LIF. No need to update profile */
                      BCM_EXIT;
                   }
                   profile_data_old.is_slm = 1;
                   rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, is_passive, &profile_data_old,
                                                                                      &old_profile, &is_last, &profile, &is_allocated);
                   profile_info->cb_rv += rv;
                   BCMDNX_IF_ERR_EXIT(rv);
                } else { /* is_passive_valid == 2 - Accelerated profile exists on lif */
                   rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint_info->id, &profile_data_old);
                   profile_info->cb_rv += rv;
                   BCMDNX_IF_ERR_EXIT(rv);

                   if (profile_data_old.is_slm == 1) {
                      /* There is also an active MEP with SLM on this LIF. No need to update profile */
                      BCM_EXIT;
                   }
                   profile_data_old.is_slm = 1;
                   rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, &profile_data_old,
                                                                                      &old_profile, &is_last, &profile, &is_allocated);
                   profile_info->cb_rv += rv;
                   BCMDNX_IF_ERR_EXIT(rv);
                }


                if (profile != old_profile) {
                   rv = bcm_oam_replace_mep_profile_in_db(unit, endpoint_info, profile, 0/*is_passive*/);
                   profile_info->cb_rv += rv;
                   /* update counter disable_outer/inner regs */
                   soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1); /* disable counting data packets for SLM */
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                   /* update counter disable_outer/inner regs */
                   soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1); /* disable counting data packets for SLM */
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
             }
             if (is_passive && SOC_IS_JERICHO(unit)) {
                /* Passive entries in Jericho did not allocate a non accelerated profile, except for MIPs. */
                BCM_EXIT;
             }
          }
          if (SOC_IS_JERICHO_PLUS(unit)) {
              rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(unit, endpoint_info->id, is_passive, &old_profile);
              BCMDNX_IF_ERR_EXIT(rv);
              if (old_profile != ARAD_PP_SLM_PASSIVE_PROFILE) {
                  rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, is_passive, profile_info->profile_data,
                          &old_profile, &is_last, &profile, &is_allocated);
                  profile_info->cb_rv += rv;
                  BCMDNX_IF_ERR_EXIT(rv);
              }
          } else {
              rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0/*flags*/,  endpoint_info->id, is_passive, profile_info->profile_data,
                      &old_profile, &is_last, &profile, &is_allocated);
              profile_info->cb_rv += rv;
              BCMDNX_IF_ERR_EXIT(rv);
          }

          /* Save first endpoint id on this lif and if new profile was allocated*/
          if (profile_info->first_endpoint_id == BCM_OAM_ENDPOINT_INVALID) {
             profile_info->first_endpoint_id = endpoint_info->id;
             profile_info->is_alloced = is_allocated;
          }

          profile_info->is_last = is_last;

          if (profile != old_profile) {
             rv = bcm_oam_replace_mep_profile_in_db(unit, endpoint_info, profile, is_passive);
             profile_info->cb_rv += rv;
          }
       }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_oam_endpoint_exchange_profile_group_cb(
   int unit,
   bcm_oam_group_info_t *group_info,
   void *user_data)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = bcm_petra_oam_endpoint_traverse(unit, group_info->id, _bcm_oam_endpoint_exchange_profile_cb, user_data);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;   
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Initialize snoop commands for MIP snooping. 
 * Snoop command 1 is set for down (ingress), 2 for up. Traps 
 * are also allocated. 
 * Function should only be called when the appropriate soc prop 
 * is set. 
 * 
 * @author sinai (29/01/2014)
 * 
 * @param unit 
 * @param snoop_destination - presumably the CPU port
 * 
 * @return int 
 */
int _bcm_oam_init_snoop_commands_for_advanced_egress_snooping(int unit, int snoop_destination) {
    int trap_code_up;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;
    bcm_rx_trap_config_t  trap_config_snoop;
    int trap_id_sw;
    int trap_code;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* start with the up-mep (ingress)*/
    snoop_cmnd =_BCM_OAM_MIP_SNOOP_CMND_UP;
    rv = bcm_petra_rx_snoop_create(unit, BCM_RX_SNOOP_WITH_ID, &snoop_cmnd);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = snoop_destination;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv = bcm_petra_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_code_up = SOC_PPC_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
    trap_id_sw = trap_code_up;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code));
    rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine,  &trap_code); /* this trap hasn't been allocated above with the others.*/
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;
    trap_config_snoop.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config_snoop.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
    rv = bcm_petra_rx_trap_set(unit, trap_code, &trap_config_snoop);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * _bcm_oam_default_id_from_ep_id
 *
 * If an endpoint ID is one of the default BCM endpoint IDs, trnaslates the ID to a default endpoint ID for the soc layer.
 * If not a default BCM endpoint ID, returns a parameter error.
 */
int _bcm_oam_default_id_from_ep_id(int unit, int epid, ARAD_PP_OAM_DEFAULT_EP_ID *default_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (epid != -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not supported in Arad.")));
        }
    }
    else {
        switch (epid) {
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_0;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS1:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS2:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS3:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_INGRESS_3;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS0:
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_0;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS1:
            if (SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_DEFAULT < 1) { 
                /* This is allowed only if more than 1 oam outlif profile is available*/
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is supported only in Jericho/QMX B0 and above.")));
            }
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS2:
            /*Coverity false positives: for this condition and the next will always fail.
             The define below in the future may be based on soc property, so leave this check as is.
             Supress false positives (deadcode, constant expression result) */
            /* coverity[result_independent_of_operands:false]*/
            if (SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_DEFAULT < 2) { 
                /* This is allowed only if more than 2 oam outlif profile is available*/
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is supported only in Jericho/QMX B0 and above.")));
            }
            /* coverity[dead_error_begin:false]*/
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS3:
            /* coverity[result_independent_of_operands:false]*/
            if (SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_DEFAULT < 3) { 
                /* This is allowed only if more than 3 oam outlif profile is available*/
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is supported only in Jericho/QMX B0 and above.")));
            }
            /* coverity[dead_error_begin:false]*/
            *default_id = ARAD_PP_OAM_DEFAULT_EP_EGRESS_3;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint ID is not default.")));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/*
 * Validity Functions
 */

int _bcm_petra_oam_opcode_map_set_verify(int unit, int opcode, int profile) {

    BCMDNX_INIT_FUNC_DEFS;

    if (opcode > SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode out of range.")));
    }

    if (profile > SOC_PPC_OAM_OPCODE_MAP_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Profile out of range.")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_oam_opcode_map_get_verify(int unit, int opcode) {

    BCMDNX_INIT_FUNC_DEFS;

    if (opcode > SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode out of range.")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * _bcm_petra_oam_is_tlv_used
 *
 * Check if port/interface state TLV is used for the endpoint
 *
 * PARAMS:
 *  endpoint_id - Assumed to also be the MEP_ID entry
 *  used - (OUT) set to 0 if TLV is not used, 1 otherwise
 * RETURN:
 *  BCM_E_NONE if no error occured
 */
int _bcm_petra_oam_is_tlv_used(int unit, int endpoint_id, uint8 *used) {
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(used);

    soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_id, &mep_db_entry);
    if (mep_db_entry.interface_status_tlv_control || mep_db_entry.port_status_tlv_en) {
        *used = 1;
    } else {
        *used = 0;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Retrieve ccm_period field from the MEP-DB saved ccm_interval */
int _bcm_dpp_oam_endpoint_ccm_period_get(int unit, int endpoint_id, int *ccm_period) {
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    uint32 ccm_period_ms, ccm_period_micro_s;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(ccm_period);

    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_id, &mep_db_entry));

    SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, mep_db_entry.ccm_interval);
        multiply_by_3_5(&ccm_period_ms, &ccm_period_micro_s);
    *ccm_period = ccm_period_ms;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Wrapper function to use bcm_petra_oam_endpoint_get internally */
STATIC int _bcm_oam_endpoint_get_internal_wrapper(int unit, int endpoint_id,
                                                  bcm_oam_endpoint_info_t *endpoint_info) {
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(endpoint_info);

    bcm_oam_endpoint_info_t_init(endpoint_info);
    rv = bcm_petra_oam_endpoint_get(unit, endpoint_id, endpoint_info);
    BCMDNX_IF_ERR_EXIT(rv);

     /* if the use_hw_id soc property is enabled, then the trap_id is the HW
       id and thus we need to convert it to the sw id becuase the oam logic uses
       the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info->remote_gport);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Prepare trap configuration for single core (arad,qax) */
int prepare_and_set_trap_config_recycle_single_core(int unit, bcm_gport_t lbm_recycle_port){
   int                  core = SOC_CORE_INVALID;
   uint32               pp_port;
   uint32               soc_sand_rv = 0;
   bcm_error_t          rv;
   bcm_rx_trap_config_t trap_config_recycle;
   SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;

   BCMDNX_INIT_FUNC_DEFS;

   BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, lbm_recycle_port,&pp_port,&core)));

   soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: LBM Recycle port network headers incorrect. Should be header_type_in=INJECTED_2_PP, header_type_out=ETH.\n")));
   }

   bcm_rx_trap_config_t_init(&trap_config_recycle);
   trap_config_recycle.flags       = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
   trap_config_recycle.dest_port   = lbm_recycle_port;

   rv = _bcm_oam_trap_and_mirror_profile_up_lbm_set(unit, &trap_config_recycle);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/* Prepare trap configuration for jericho (2 cores) */
int prepare_and_set_trap_config_recycle_dual_core(int unit, bcm_gport_t *lbm_recycle_port){
   
   int         core        = SOC_CORE_INVALID;
   uint32      pp_port;
   uint32      soc_sand_rv = 0;
   bcm_error_t rv;
   bcm_rx_trap_config_t       trap_config_recycle;
   bcm_rx_trap_core_config_t  core_config_arr[2];
   SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;

   BCMDNX_INIT_FUNC_DEFS;

   /* get core 0's pp_port */
   BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, lbm_recycle_port[0],&pp_port,&core)));

   soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: LBM Recycle port network headers incorrect. Should be header_type_in=INJECTED_2_PP, header_type_out=ETH.\n")));
   }

   /* get core 1's pp_port */
   BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, lbm_recycle_port[1],&pp_port,&core)));

   soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: LBM Recycle port network headers incorrect. Should be header_type_in=INJECTED_2_PP, header_type_out=ETH.\n")));
   }

   bcm_rx_trap_config_t_init(&trap_config_recycle);
   
   core_config_arr[0].dest_port              = lbm_recycle_port[0];
   core_config_arr[1].dest_port              = lbm_recycle_port[1];

   trap_config_recycle.flags                = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
   trap_config_recycle.dest_port            = lbm_recycle_port[0];
   trap_config_recycle.core_config_arr_len  = 2;
   trap_config_recycle.core_config_arr      = core_config_arr;
   
   rv = _bcm_oam_trap_and_mirror_profile_up_lbm_set(unit, &trap_config_recycle);
   BCMDNX_IF_ERR_EXIT(rv);

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/*
 * Begin BCM Functions
 */
 
/*
 *  Initialize y1711-LM, create trap for 1711-lm and set it to egress PG selector for value_1
 */
int _bcm_oam_y1711_init(int unit){
    int rv = 0;
    int trap_code_y1711 = 0;
    int trap_id_sw;
    int index = 0;
    int dont_care; 
    uint32 soc_sand_rv = 0;
    bcm_rx_trap_config_t trap_config_Y1711;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    SOC_PPC_OAM_MEP_TYPE  mep_type;
    ARAD_EGR_PROG_EDITOR_PROGRAMS   program;
    uint64 arg64;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    /* Get the OAMP port(s)   */
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &dont_care);
    BCMDNX_IF_ERR_EXIT(rv);

    /* create Y1711 trap */
    for(index= 0; index < 2; index++){
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code_y1711);
        BCMDNX_IF_ERR_EXIT(rv);
        
        bcm_rx_trap_config_t_init(&trap_config_Y1711);
        trap_config_Y1711.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP |BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
        
        trap_config_Y1711.dest_port = oamp_port[0];
        trap_config_Y1711.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
        
        rv = bcm_rx_trap_set(unit,trap_code_y1711, &trap_config_Y1711);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_rx_trap_hw_id_to_sw_id(unit,trap_code_y1711, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_type = (index == 0) ? SOC_PPC_OAM_TRAP_ID_Y1711_MPLS : SOC_PPC_OAM_TRAP_ID_Y1711_PWE;
        rv = OAM_ACCESS.trap_info.trap_ids.set(unit, mep_type, (uint32)trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_type = (index == 0) ? SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP : SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_set, (unit, mep_type, trap_id_sw));
        BCMDNX_IF_ERR_EXIT(rv);
        
        program = (index == 0) ? ARAD_EGR_PROG_EDITOR_PROG_OAM_1711_MPLS_TP : ARAD_EGR_PROG_EDITOR_PROG_OAM_1711_PWE;
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_prog_editor_prog_sel_value1_set, (unit, program,trap_code_y1711));
        BCMDNX_IF_ERR_EXIT(rv);
        
        COMPILER_64_SET(arg64,0,trap_code_y1711);
        soc_sand_rv = arad_pp_prge_program_var_set(unit,program,arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    
    BCM_EXIT;
         
exit:
        BCMDNX_FUNC_RETURN;
}

/* Initialize the OAM subsystem */
int bcm_petra_oam_init(int unit) {
    uint32 soc_sand_rv = 0;
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_oamp, trap_config_cpu, trap_config_snoop, trap_config_recycle;
    bcm_rx_snoop_config_t snoop_config_cpu;
    bcm_port_config_t *port_config = NULL;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)], cpu_port, recycle_port, recycle_port_iter , lbm_recycle_port[2], port_i;
    int count_erp, snoop_cmnd, trap_code, trap_code_with_meta, fake_destination, up_mep_recycle_port, core = SOC_CORE_INVALID;
    int lb_trap_id, tst_trap_id;
    uint32 mirror_id_wb, oamp_local_port, pp_port, trap_id, flags = 0;
    uint8 oam_is_init, is_bfd_init, is_allocated, is_advanced_egress_snooping = 0;
    _bcm_dpp_gport_info_t gport_info;
    int     ccm_interval=0;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t sat_port = 0;
    int found = 0;
    bcm_oam_event_type_t oam_event_i;
    bcm_bfd_event_type_t bfd_event_i;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data = NULL;
    ARAD_PORT_HEADER_TYPE header_type_out = SOC_TMC_PORT_HEADER_TYPE_NONE;

#ifdef BCM_ARAD_SUPPORT
    SOC_TMC_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;
#endif /*BCM_ARAD_SUPPORT*/

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (oam_is_init) {
        if (!SOC_WARM_BOOT(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: OAM already initialized.\n")));
        }
    } else {
        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    is_advanced_egress_snooping = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0);

    if (SOC_WARM_BOOT(unit)) {
        BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_arad_pp_oam_dma_set, (unit)));

        rv = soc_ppd_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_oam_sw_db_locks_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        /* init internal_opcode_init[internal_opcode].additional_info for usage in loss_add */
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_eth_oam_opcode_map_set_unsafe,(unit));
        BCMDNX_IF_ERR_EXIT(rv); 
        BCM_EXIT;
    }


    /* Validating oam_enable SOC property is set */
    if(!soc_property_get(unit, spn_OAM_ENABLE, 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("Error: oam_enable SOC property must be set in order to initialize OAM.\n")));
    }

    /* Validating at least one OAMP port is configured */
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
    BCMDNX_IF_ERR_EXIT(rv);
    if (count_erp < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: Oamp port disabled, can't init OAM.\n")));
    }

     /* Software state init */
     rv = OAM_ACCESS.is_allocated(unit, &is_allocated);
     BCMDNX_IF_ERR_EXIT(rv);

     if(!is_allocated) {
         rv = OAM_ACCESS.alloc(unit);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     /* We need also that bfd will be allocated */
     rv = sw_state_access[unit].dpp.bcm.bfd.is_allocated(unit, &is_allocated);
     BCMDNX_IF_ERR_EXIT(rv);

     if(!is_allocated) {
         rv = sw_state_access[unit].dpp.bcm.bfd.alloc(unit);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     soc_sand_rv = bcm_dpp_endp_lst_init(unit,MAX_NOF_ENDP_LSTS);
     SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
     soc_sand_rv = bcm_dpp_endp_lst_m_init(unit,MAX_NOF_ENDP_LST_MS);
     SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!is_bfd_init) {

        rv = _bcm_dpp_oam_dbs_init(unit, TRUE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_ARADPLUS(unit)) {
        rv = _bcm_dpp_oam_error_trap_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_ALLOC(port_config, sizeof(bcm_port_config_t),
        "bcm_petra_oam_init.port_config");
    if (port_config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
     rv = bcm_petra_port_config_get(unit, port_config);
     BCMDNX_IF_ERR_EXIT(rv);

     if (SOC_IS_JERICHO(unit)) {
         /* 1. create SAT trap for TST */
         rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapSat0, &tst_trap_id);
         BCMDNX_IF_ERR_EXIT(rv);
         rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_OAM_TRAP_ID_SAT0_TST, (uint32)tst_trap_id);
         BCMDNX_IF_ERR_EXIT(rv);
         /* 2. create SAT trap for for LB */
         rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapSat1, &lb_trap_id);
         BCMDNX_IF_ERR_EXIT(rv);
         rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_OAM_TRAP_ID_SAT0_LB, (uint32)lb_trap_id);
         BCMDNX_IF_ERR_EXIT(rv);
         found = 0;

         /* Get the SAT port */
         BCM_PBMP_ITER(port_config->sat, sat_port) {
             found = 1;
             break;
         }
         bcm_rx_trap_config_t_init(&trap_config);
         /*And then*/
         trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
         trap_config.dest_port =  (SOC_IS_QAX(unit))?(found?sat_port:oamp_port[0]):(oamp_port[0]);
         /* Update the FWD header:*/
         trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
         trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;

         rv = bcm_petra_rx_trap_set(unit, tst_trap_id, &trap_config); 
         if(rv != BCM_E_EXISTS) {
             BCMDNX_IF_ERR_EXIT(rv);
         }
         bcm_rx_trap_config_t_init(&trap_config);
         /*And then*/
         trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
         trap_config.dest_port =  (SOC_IS_QAX(unit))?(found?sat_port:oamp_port[0]):(oamp_port[0]);
         /* Update the FWD header:*/
         trap_config.flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
         trap_config.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
         rv = bcm_petra_rx_trap_set(unit, lb_trap_id, &trap_config); 
         if(rv != BCM_E_EXISTS) {
             BCMDNX_IF_ERR_EXIT(rv);
         }
     }

     /* Allocate 3 FTMH traps
        1. CPU -       SOC_PPC_TRAP_CODE_OAM_CPU_MIRROR
        2. OAMP -      SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR
        3. CPU SNOOP - SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP (Jericho only) */
     for (trap_code_with_meta = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
          trap_code_with_meta <= SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP;
          trap_code_with_meta++) {

         if (!SOC_IS_JERICHO(unit) && trap_code_with_meta == SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP) {
             /* SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP not used in Arad */
             continue;
         }

         BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit, trap_code_with_meta, &trap_code));
         rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_code);
         BCMDNX_IF_ERR_EXIT(rv);
     }



     /* Set OAM_Port_profile to 1 for all the ports which header type is CPU */
     BCM_PBMP_ITER(port_config->all, port_i) {
         rv = _bcm_dpp_gport_to_phy_port(unit, port_i, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
         BCMDNX_IF_ERR_EXIT(rv);

         rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, gport_info.local_port, &header_type_out);
         BCMDNX_IF_ERR_EXIT(rv);

         if (header_type_out == SOC_TMC_PORT_HEADER_TYPE_CPU)
         {
             BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&pp_port,&core)));
             soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, ARAD_PRGE_PP_SELECT_PP_OAM_PORT);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         }

     }

     /* Configure trapping to CPU */

     /* Get the first CPU port */
     BCM_PBMP_ITER(port_config->cpu, cpu_port) {
         BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, cpu_port,&pp_port,&core)));
         break;
     }

     /* Set OAM port profile to ARAD_PRGE_PP_SELECT_PP_OAM_PORT, Setting program selection for OAM programs in PRGE */
     rv = _bcm_dpp_gport_to_phy_port(unit, cpu_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
     BCMDNX_IF_ERR_EXIT(rv);

     BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&pp_port,&core)));
     soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, ARAD_PRGE_PP_SELECT_PP_OAM_PORT);
     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

     rv = _bcm_dpp_gport_to_phy_port(unit, oamp_port[0], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
     BCMDNX_IF_ERR_EXIT(rv);

     BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&pp_port,&core)));
     oamp_local_port = pp_port;

     bcm_rx_trap_config_t_init(&trap_config_cpu);

     soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, ARAD_PRGE_PP_SELECT_PP_OAM_PORT);
     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

     trap_config_cpu.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
     trap_config_cpu.dest_port = cpu_port;
     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_CPU,
                                               SOC_PPC_OAM_UPMEP_TRAP_ID_CPU,
                                               &trap_config_cpu,
                                               SOC_PPC_TRAP_CODE_OAM_CPU,
                                               SOC_PPC_TRAP_CODE_OAM_CPU_MIRROR,
                                               SOC_PPC_OAM_MIRROR_ID_CPU,
                                               1 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configure trapping to recycle port (used for LB) */

     recycle_port = soc_property_get(unit, spn_OAM_RCY_PORT, -1);
     if (recycle_port != -1) {
         BCM_PBMP_ITER(port_config->rcy, recycle_port_iter) {
             if (recycle_port_iter == recycle_port) {
                 break;
             }
         }
         if (recycle_port_iter != recycle_port) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
         }

         BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, recycle_port,&pp_port,&core)));

         soc_sand_rv = arad_port_header_type_get(unit, core, pp_port, &header_type_incoming, &header_type_outgoing);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

         if ((header_type_incoming != SOC_TMC_PORT_HEADER_TYPE_TM) || (header_type_outgoing != SOC_TMC_PORT_HEADER_TYPE_ETH)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Recycle port network headers incorrect. Should be header_type_in=TM, header_type_out=ETH.\n")));
         }

         bcm_rx_trap_config_t_init(&trap_config_recycle);
         trap_config_recycle.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
         trap_config_recycle.dest_port = recycle_port;

         rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                                   SOC_PPC_OAM_TRAP_ID_RECYCLE,
                                                   0/*SOC_PPC_OAM_UPMEP_TRAP_ID_RECYCLE*/,
                                                   &trap_config_recycle,
                                                   SOC_PPC_TRAP_CODE_OAM_RECYCLE,
                                                   0/*SOC_PPC_TRAP_CODE_OAM_RECYCLE_MIRROR*/,
                                                   0/*SOC_PPC_OAM_MIRROR_ID_RECYCLE*/,
                                                   0 /*set_upmep*/);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_CPU, &trap_id);
     BCMDNX_IF_ERR_EXIT(rv);
     rv = OAM_ACCESS.trap_info.mirror_ids.set(unit, SOC_PPC_OAM_MIRROR_ID_RECYCLE, trap_id);
     BCMDNX_IF_ERR_EXIT(rv);

     rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_CPU, &trap_id);
     BCMDNX_IF_ERR_EXIT(rv);
     rv = OAM_ACCESS.trap_info.upmep_trap_ids.set(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_RECYCLE, trap_id);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configure snooping to CPU (used for MIP LT) */

     /* creating snoop trap id */
     snoop_cmnd = 0;
     if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
         /* In this case create a snoop_cmnd for down meps and up meps seperatley. */
         rv = _bcm_oam_init_snoop_commands_for_advanced_egress_snooping(unit, cpu_port);
         BCMDNX_IF_ERR_EXIT(rv);

         snoop_cmnd = _BCM_OAM_MIP_SNOOP_CMND_DOWN;
         flags = BCM_RX_SNOOP_WITH_ID;
     }

     /* Now the other snoop cmnd for advanced egress snooping, or the first one for other cases. */
     rv = bcm_petra_rx_snoop_create(unit, flags, &snoop_cmnd);
     BCMDNX_IF_ERR_EXIT(rv);

     bcm_rx_snoop_config_t_init(&snoop_config_cpu);
     snoop_config_cpu.flags = BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO;
     snoop_config_cpu.dest_port = cpu_port;
     snoop_config_cpu.size = -1;
     snoop_config_cpu.probability = 100000;

     rv = bcm_petra_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
     BCMDNX_IF_ERR_EXIT(rv);

     bcm_rx_trap_config_t_init(&trap_config_snoop);
     trap_config_snoop.flags = 0;
     trap_config_snoop.trap_strength = 0;
     trap_config_snoop.snoop_cmnd = snoop_cmnd;

     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_SNOOP,
                                               SOC_PPC_OAM_UPMEP_TRAP_ID_SNOOP,
                                               &trap_config_snoop,
                                               SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP,
                                               ((SOC_IS_ARADPLUS_AND_BELOW(unit) || is_advanced_egress_snooping) ? SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP : SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP_UP),
                                               SOC_PPC_OAM_MIRROR_ID_SNOOP,
                                               1 /*set upmep - already set for advanced snooping.*/);
     BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_QUX(unit) && is_advanced_egress_snooping) {
        /* 
         * On Qux, if oam_sub_type is 6, then it map hit two programs in PRGE, ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM and 
         * ARAD_EGR_PROG_EDITOR_PROG_OAM_EGRESS_SNOOP_WITH_OUTLIF_INFO. 
         * In order to distinguish between two Prge programs, added mirror command as a key to program selection
         * for ARAD_EGR_PROG_EDITOR_PROG_OAM_EGRESS_SNOOP_WITH_OUTLIF_INFO.
         */
        SOCDNX_IF_ERR_EXIT(OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_SNOOP, &mirror_id_wb));

        soc_sand_rv = arad_egr_prog_editor_prog_sel_mirror_command_set(unit, ARAD_EGR_PROG_EDITOR_PROG_OAM_EGRESS_SNOOP_WITH_OUTLIF_INFO, mirror_id_wb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

     /* Supporting MIP LTM, LBM mirroring */
     rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_CPU, &mirror_id_wb);
     BCMDNX_IF_ERR_EXIT(rv);

     rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &trap_id);
     BCMDNX_IF_ERR_EXIT(rv);
     rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_id, mirror_id_wb);
     BCMDNX_IF_ERR_EXIT(rv);

     rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_id);
     BCMDNX_IF_ERR_EXIT(rv);
     rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_id, mirror_id_wb);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configuration trapping to OAMP (used for accelerated packets) */

     bcm_rx_trap_config_t_init(&trap_config_oamp);
     trap_config_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
     trap_config_oamp.dest_port = oamp_port[0];
     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_OAMP,
                                               SOC_PPC_OAM_UPMEP_TRAP_ID_OAMP,
                                               &trap_config_oamp,
                                               SOC_PPC_TRAP_CODE_TRAP_ETH_OAM,
                                               SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR,
                                               SOC_PPC_OAM_MIRROR_ID_OAMP,
                                               1 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configuration trapping to OAMP Y1731 MPLS (used for accelerated packets) */

     bcm_rx_trap_config_t_init(&trap_config_oamp);
     trap_config_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
     trap_config_oamp.dest_port = oamp_port[0];

     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_MPLS,
                                               0,
                                               &trap_config_oamp,
                                               SOC_PPC_TRAP_CODE_TRAP_Y1731_O_MPLS_TP,
                                               0,
                                               0,
                                               0 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configure trapping to OAMP Y1731 PWE (used for accelerated packets) */

     bcm_rx_trap_config_t_init(&trap_config_oamp);
     trap_config_oamp.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
     trap_config_oamp.dest_port = oamp_port[0];

     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE,
                                               0,
                                               &trap_config_oamp,
                                               SOC_PPC_TRAP_CODE_TRAP_Y1731_O_PWE,
                                               0,
                                               0,
                                               0 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Configure trapping to CPU error packets */

     /* Creating traps for errors: Level error */
     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_ERR_LEVEL,
                                               SOC_PPC_OAM_UPMEP_TRAP_ID_ERR_LEVEL,
                                               &trap_config_cpu,
                                               SOC_PPC_TRAP_CODE_OAM_LEVEL,
                                               SOC_PPC_TRAP_CODE_OAM_LEVEL,
                                               SOC_PPC_OAM_MIRROR_ID_ERR_LEVEL,
                                               1 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* Creating traps for errors: Passive error */
     rv = _bcm_oam_trap_and_mirror_profile_set(unit,
                                               SOC_PPC_OAM_TRAP_ID_ERR_PASSIVE,
                                               SOC_PPC_OAM_UPMEP_TRAP_ID_ERR_PASSIVE,
                                               &trap_config_cpu,
                                               SOC_PPC_TRAP_CODE_OAM_PASSIVE,
                                               SOC_PPC_TRAP_CODE_OAM_PASSIVE,
                                               SOC_PPC_OAM_MIRROR_ID_ERR_PASSIVE,
                                               1 /*set_upmep*/);
     BCMDNX_IF_ERR_EXIT(rv);

     /* SOC init */
     soc_sand_rv = soc_ppd_oam_init(unit, FALSE/*not bfd*/);
     BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

     _BCM_OAM_ENABLED_SET(TRUE);

     /* Set FLP PTC profile for OAMP port */
     if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics) {
         SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE success_vt;
         SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE success_tt;
         ARAD_PP_ISEM_ACCESS_KEY               sem_key;
         ARAD_PP_ISEM_ACCESS_ENTRY             sem_entry;

         ARAD_PORTS_PROGRAMS_INFO port_prog_info;
         arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);

         soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, oamp_local_port, &port_prog_info);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
         soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, oamp_local_port, &port_prog_info);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

         up_mep_recycle_port = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_up_mep_client_recycle_port", -1);
         /* For up mep server client mode, the client receives packets on device recycle port. 
            Therefore for client- Set FLP PTC profile on device recycle port */
         if (up_mep_recycle_port != -1) {
             arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
             soc_sand_rv = arad_ports_programs_info_get_unsafe(unit, up_mep_recycle_port, &port_prog_info);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
             port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_OAMP;
             soc_sand_rv = arad_ports_programs_info_set_unsafe(unit, up_mep_recycle_port, &port_prog_info);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         }
         /* add entry to tt stage - to indentify the tag format */
         sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_TT;
         sem_key.key_info.oam_stat.cfm_eth_type = ARAD_PP_CFM_ETHER_TYPE;
         rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_tt);
         BCMDNX_IF_ERR_EXIT(rv);

         /* add entry to vt stage - down mep */
         sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_VT;
         sem_key.key_info.oam_stat.pph_type = 0x0;
         sem_key.key_info.oam_stat.opaque = 0x7;
         rv = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, &success_vt);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     /* Configuring "fake" destination for recieving CCMs but not sending. The user can determine the local port for the fake destination by soc property.
        If this is set configure this destination. */

     fake_destination = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_local_port_for_fake_destination", -1);
     if (fake_destination != -1) {
         uint32 invalid_destination = _BCM_OAM_ILLEGAL_DESTINATION;
         int ignored;
         /* validity checks:*/
         if (SOC_IS_JERICHO(unit)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" not needed for Jericho")));
         }
         if (SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx == 0) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" may only be set together with"
                                                            " \"custom_feature_oam_ccm_rx_wo_tx\" \n")));
         }
         if (fake_destination < 0 || fake_destination > 255) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("soc property \"oam_local_port_for fake_destination\" Must be set between 0 and 255"
                                                            " (this value will then be unusable as an LSB for src_mac_address in endpoint_create()")));
         }

         /* Allocate the entry */
         rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, SHR_RES_ALLOC_WITH_ID, &invalid_destination, &ignored, &fake_destination);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     /* Count egress PWE - Set the OAM_DROP_1ST_ENC program selection
        In Jericho 1st_Out_LIF_Profile is used while in QAX / Jer+ 3rd_Out_LIF_Profile is used */
     if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "eg_pwe_counting", 0)) {
         int outlif_proflie_index = 1;
         if (SOC_IS_JERICHO_PLUS(unit)) {
             /* when enabling custom_feature_four_independent_mpls_encapsulation_enable, 
              * the PW entry will use native LL phase, so the outlif profile should be SECOND_OUT_LIF_PROFILE
              */
             outlif_proflie_index = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "four_independent_mpls_encapsulation_enable", 0) ? 
                 2 : 3;
         } 
         rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_EG_PWE_COUNTING,
                                                               ARAD_EGR_PROG_EDITOR_PROG_OAM_DROP_1ST_ENC, 1 /*value*/,
                                                               outlif_proflie_index /*outlif-profile1*/);
         BCMDNX_IF_ERR_EXIT(rv);
     }
     if (SOC_DPP_CONFIG(unit)->pp.oam_use_double_outlif_injection) {
         rv = arad_egr_prog_editor_prog_sel_outlif_profile_set(unit, SOC_OCC_MGMT_OUTLIF_APP_EG_PWE_COUNTING,
                                                               ARAD_EGR_PROG_EDITOR_PROG_OAM_DROP_1ST_ENC, 1 /*value*/,
                                                               3 /*outlif-profile1*/);
         BCMDNX_IF_ERR_EXIT(rv);
     }

     /* QAX should configure sd/sf scanner */
     if(SOC_IS_QAX(unit)){
          for(ccm_interval=1; ccm_interval<=7; ccm_interval++){
             soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_scanner_set,(unit,ccm_interval));
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         }
         if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1){
             /* Y1711 init*/                   
             rv = _bcm_oam_y1711_init(unit);
             BCMDNX_IF_ERR_EXIT(rv);
         }
     }

     if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
         /* Arad mode classifier: set up static entries for passive side MIP.*/
         /* Need to create a default profile_data to pass to the soc layer. For that we need a dummy classifier mep entry.*/
         SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;

         BCMDNX_ALLOC(profile_data, sizeof(*profile_data),
             "bcm_petra_oam_init.profile_data");
         if (port_config == NULL) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
         }

         SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data);
         SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

         classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_ETH_OAM;

         rv = _bcm_oam_default_profile_get(unit, profile_data, &classifier_mep_entry, 0 /* non accelerated */, 0 /* is default */, 0 /* is upmep */);
         BCMDNX_IF_ERR_EXIT(rv);

         soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier1_mip_passive_entries,
                                           (unit,profile_data));
         BCMDNX_IF_ERR_EXIT(rv);
     }

    /* Following settings should be done in WB as well. */

    rv = soc_ppd_oam_register_dma_event_handler_callback(unit, _bcm_dpp_oam_fifo_interrupt_handler);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma == 0 || SOC_DPP_CONFIG(unit)->pp.oam_use_report_fifo_dma == 0 || !SOC_IS_JERICHO(unit)) {
        /* Thess lines enable the interrupts and disables the bypass interrupt mechanism
           In Jericho when using the DMA this interrupt shouldn't be enabled.
           In Arad the ECI_GENERAL_CONFIGURATION_2 register sets this "automatically" */
        rv = _bcm_dpp_oamp_interrupts_init(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*
     * configuring a trap for up-mep LBM
     */
    if (SOC_DPP_DEFS_GET(unit, nof_cores) == 2) {
       lbm_recycle_port[0] = SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up_core_0;
       lbm_recycle_port[1] = SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up_core_1;
       if(SOC_DPP_CONFIG(unit)->pp.upmep_lbm_is_configured){
          BCM_PBMP_ITER(port_config->rcy, recycle_port_iter) {

             if (recycle_port_iter == lbm_recycle_port[0]) {
                break;
             }
          }
          if (recycle_port_iter != lbm_recycle_port[0]) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM LBM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
          }
          BCM_PBMP_ITER(port_config->rcy, recycle_port_iter) {
             if (recycle_port_iter == lbm_recycle_port[1]) {
                break;
             }
          }
          if (recycle_port_iter != lbm_recycle_port[1]) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM LBM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
          }
          rv = prepare_and_set_trap_config_recycle_dual_core(unit, lbm_recycle_port);
          BCMDNX_IF_ERR_EXIT(rv);
       }

    } else {
       lbm_recycle_port[0] = SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up;
       if (SOC_DPP_CONFIG(unit)->pp.upmep_lbm_is_configured) {

          BCM_PBMP_ITER(port_config->rcy, recycle_port_iter) {
             if (recycle_port_iter == lbm_recycle_port[0]) {
                break;
             }
          }
          if (recycle_port_iter != lbm_recycle_port[0]) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Network Interface of OAM LBM recycling port should be configured as RCY (ucode_port_<Local-Port-Id>=RCY).\n")));
          }
          rv = prepare_and_set_trap_config_recycle_single_core(unit, lbm_recycle_port[0]);
          BCMDNX_IF_ERR_EXIT(rv);
       }
    }

    rv = _bcm_dpp_oam_sw_db_locks_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    if(SOC_IS_QAX(unit)) {
       rv = _bcm_oam_init_48B_MAID_Entries(unit);
       BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_JERICHO_AND_BELOW(unit)) {
       /*Initialize the SLM enabled endpoints to zero*/
       rv = OAM_ACCESS.no_of_slm_endpoints.set(unit, 0);
       BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_JERICHO_PLUS(unit)) {
       /* set disable_outer/inner to disable counting data packets for SLM MEPs */
       rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, ARAD_PP_SLM_PASSIVE_PROFILE, 1); /* disable counting data packets for SLM */
       BCM_SAND_IF_ERR_EXIT(rv);
    }

    /* oam event handler init */
    for (oam_event_i = 0; oam_event_i < bcmOAMEventCount; oam_event_i++) {
         _g_oam_event_cb[unit][oam_event_i] = NULL;
         _g_oam_performance_event_cb[unit][oam_event_i] = NULL;
         _g_oam_event_ud[unit][oam_event_i] = NULL;
    }
    /* bfd event handler init */
    for (bfd_event_i = 0; bfd_event_i < bcmBFDEventCount; bfd_event_i++) {
         _g_bfd_event_cb[unit][bfd_event_i] = NULL;
         _g_bfd_event_ud[unit][bfd_event_i] = NULL;
    }

exit:
    BCM_FREE(port_config);
    BCM_FREE(profile_data);
    BCMDNX_FUNC_RETURN;
}

/* Create or replace an OAM group object */
int bcm_petra_oam_group_create(int unit, bcm_oam_group_info_t *group_info) {
    int flags = 0;
    uint32 ma_index = 0;
    int rv = 0;
    _bcm_oam_ma_name_t ma_name_struct;
    int i;
    uint8 new_id_alloced = FALSE, IsGroupDataIn_MEP_DB = FALSE;
    uint8 group_is_found;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_oam_group_create_verify(unit, group_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /*init ma_name*/
    for (i=0; i<_BCM_OAM_MA_NAME_WITHOUT_HEADERS; i++) {
        ma_name_struct.name[i] = 0;
    }

    /* init ma_type and index */
    ma_name_struct.name_type = 0;
    ma_name_struct.index = 0;

    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        flags = SHR_RES_ALLOC_WITH_ID | SHR_RES_ALLOC_REPLACE;
        ma_index = group_info->id;
        rv = _bcm_dpp_oam_ma_db_get(unit, ma_index, &ma_name_struct, &group_is_found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (group_is_found == FALSE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                 (_BSL_BCM_MSG("Error: group not found in database")));
        }
        if (ma_name_struct.name_type != _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL,
                 (_BSL_BCM_MSG("Error: cannot change a non-48-byte-MAID group name")));
        }
        sal_memcpy(ma_name_struct.name, group_info->name, 48);
        /* Deleting entry before creating a new one */
        rv = _bcm_dpp_oam_ma_db_delete(unit, group_info->id);
        BCMDNX_IF_ERR_EXIT(rv);
        IsGroupDataIn_MEP_DB = TRUE;
    }
    else
    {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
            flags = SHR_RES_ALLOC_WITH_ID;
            ma_index = group_info->id;
        }
        /* fully flexible 48B MAID format */
        if ((SOC_IS_QAX(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0))) && (group_info->flags & BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE)) { 
            ma_name_struct.name_type = _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE;
            if (SOC_IS_QAX(unit)) {
                IsGroupDataIn_MEP_DB = TRUE;
                ma_name_struct.index = group_info->group_name_index; /* These entries must be managed by the user */
            }
        }
        rv = _bcm_dpp_oam_group_name_to_ma_name_struct_convert(unit, group_info->name, &ma_name_struct);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_dpp_am_oam_ma_id_alloc(unit, flags, &ma_index);
        BCMDNX_IF_ERR_EXIT(rv);
        new_id_alloced = TRUE;
        group_info->id = ma_index;
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM: Creating group %d\n"),
                         ma_index));
    }

    rv = _bcm_dpp_oam_ma_db_insert(unit, ma_index, &ma_name_struct);
    BCMDNX_IF_ERR_EXIT(rv);

    if (IsGroupDataIn_MEP_DB) {
        rv = _bcm_oam_write_group_data_to_oamp_mep_db(unit, group_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    if ((rv != BCM_E_NONE) && new_id_alloced) {
        /* If error after new id was alloced we should free it */
        rv = _bcm_dpp_oam_ma_db_delete(unit, group_info->id);
        rv = bcm_dpp_am_oam_ma_id_dealloc(unit, group_info->id);
    }
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM group object */
int bcm_petra_oam_group_get(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info) {

    int rv;
    uint8 found;
    _bcm_oam_ma_name_t ma_name_struct;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    if (group < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
    }

    group_info->id = group;
    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)group, &ma_name_struct, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("Error: Group %d is not valid\n"), group));
    }

    rv = _bcm_dpp_oam_ma_name_struct_to_group_name_convert(unit, &ma_name_struct, group_info->name);
    if (SOC_IS_QAX(unit) && (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
        group_info->group_name_index = ma_name_struct.index;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* OAMP Server feature requires storing additional information */
STATIC int _bcm_oam_endpoint_server_store_data(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry, bcm_oam_endpoint_info_t *endpoint_info) {
    BCMDNX_INIT_FUNC_DEFS;

    /* additional information needs to be stored. Shove it in other fields.*/
    if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
        classifier_mep_entry->remote_gport = endpoint_info->tx_gport;
    } else {
        classifier_mep_entry->remote_gport = endpoint_info->remote_gport;
    }

    classifier_mep_entry->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Update outlif profile of PWE endpoint outlif for egress PWE counting */
STATIC int _bcm_oam_set_outlif_profile_to_count_eg_pwe(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl) {
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    bcm_gport_t out_gport;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries = 0;
    int out_lif_id = 0;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;

    out_gport = endpoint_info_lcl->mpls_out_gport != BCM_GPORT_INVALID ? endpoint_info_lcl->mpls_out_gport : endpoint_info_lcl->gport;

    rv = _bcm_dpp_gport_to_hw_resources(unit, out_gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    out_lif_id = gport_hw_resources.local_out_lif;

    rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, out_lif_id, NULL, &lif_usage);
    BCMDNX_IF_ERR_EXIT(rv);

    if (lif_usage != _bcmDppLifTypeMplsPort) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently only PWE outlif supported")));
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "_bcm_oam_set_outlif_profile_to_count_eg_pwe.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, out_lif_id, 1, \
                                             encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, \
                  (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_EG_PWE_COUNTING, 1,
                      &(encap_entry_info->entry_val.mpls_encap_info.outlif_profile)));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

    /* add encap entry, set outlif profile additional encapsulation bit */
    soc_sand_rv = soc_ppd_eg_encap_mpls_entry_add(unit, out_lif_id, &(encap_entry_info->entry_val.mpls_encap_info), next_eep[0]);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


/* Perform validity checks and get profile of the reference endpoint */
int _bcm_oam_get_reference_profile(int unit, 
                                   bcm_oam_endpoint_info_t *endpoint_info_lcl, 
                                   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry, 
                                   uint8  is_passive,
                                   uint8  is_accelerated,
                                   int    *ref_profile)
{
int   rv = 0;
uint8 found;
uint8 action_source_is_mip;
uint8 action_source_is_upmep;
uint8 is_upmep;
uint8 is_mip;
SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY action_classifier_mep_entry;

   BCMDNX_INIT_FUNC_DEFS;

   is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);
   is_mip = _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl);

   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->action_reference_id, &action_classifier_mep_entry, &found);
   BCMDNX_IF_ERR_EXIT(rv);
   if (!found) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No reference endpoint for action source with id %d found.\n"), endpoint_info_lcl->action_reference_id));
   }
   action_source_is_mip    = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&action_classifier_mep_entry);
   action_source_is_upmep  = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&action_classifier_mep_entry);
   if (is_mip != action_source_is_mip) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Reference endpoint type mismatch. Enpoint is_mip: %d, Reference is_mip: %d.\n"), is_mip, action_source_is_mip));
   }
   if (is_upmep != action_source_is_upmep) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Reference endpoint type mismatch. Enpoint is_upmep: %d, Reference is_upmep: %d.\n"), is_upmep, action_source_is_upmep));
   }
   if (is_accelerated) {
         *ref_profile = action_classifier_mep_entry.acc_profile;
   }else{
      if (is_passive) {
         *ref_profile = action_classifier_mep_entry.non_acc_profile_passive;
      } else {
         *ref_profile = action_classifier_mep_entry.non_acc_profile;
      }
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/* Handles creation of a new local MEP/MIP. */
STATIC int _bcm_oam_endpoint_local_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    int flags = 0;
    int is_new_ep = 1;
    SOC_PPC_OAM_ICC_MAP_DATA data;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data_pointer;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data_passive;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data_acc;
    uint8 non_acc_profile_alloc=0;
    uint8 non_acc_profile_passive_alloc=0;
    uint8 acc_profile_alloc=0;
    _bcm_oam_ma_name_t ma_name_struct;
    uint8 allocate_icc_ndx = 0;
    uint8 found, found_profile;
    uint8 new_id_alloced = FALSE;
    int icc_ndx = 0;
    int is_allocated;
    int is_maid_48_jericho = 0;
    int profile;
    int dmm_phase = 0;
    uint32 profile_dummy;
    uint32 endpoint_ref_counter;
    uint8 is_mp_type_flexible;
    uint8 updated_mp_type = 0;
    uint8 update_profile = 0;
    uint8 is_accelerated = 0;
    uint8 is_upmep;
    uint8 is_passive;
    uint8 is_mip;
    uint8 found_mip;
    int   ref_profile;
    int check_lif_profile_counter;
    uint32 check_lif_profile;
    uint8 check_lif_profile_found;
    uint8 is_slm_local=0;
    uint8 passive_found;
    uint8 passive_found_profile;
    uint8 passive_is_mp_type_flexible;
    uint8 passive_found_mip;
    uint32 passive_profile_temp;
    uint32 count_disabled_counter_id = 0;
    SOC_PPC_OAM_MEP_TYPE  mep_type;
    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    int nof_tpids = 0;
    int inner_tpid_only = 0;
    uint32 tpid_profile = 0;
    int is_piggyback_lm_enabled = 0;

    BCMDNX_INIT_FUNC_DEFS;

    is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info_lcl);
    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);
    is_mip = _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl);

    if ((endpoint_info_lcl->gport == BCM_GPORT_INVALID)
        && (endpoint_info_lcl->tx_gport == BCM_GPORT_INVALID)
        && !_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Both gport and"
                                          " tx_gport may not be invalid\n")));
    }

    if ((is_accelerated
         || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl))
        && _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info_lcl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: Intermediate endpoint"
                                          " can not be accelerated in HW.\n")));
    }

    /* Check that the group exist - needed for both accelerated and not accelerated */
    rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info_lcl->group, &ma_name_struct, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Error: group %d does not exist.\n"), endpoint_info_lcl->group));
    }
    

    /*
     * For coverity
     */
    BCM_DPP_UNIT_CHECK(unit);

    if (is_accelerated) { /* TX - OAMP */
        if ((endpoint_info_lcl->tx_gport == BCM_GPORT_INVALID) && (!is_upmep)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG
                                              ("Error: tx_gport invalid, can not add accelerated downward facing endpoint.\n")));
        }

        if ((ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)
            && (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID)
            && (endpoint_info_lcl->id % 4 != 0)
            && (SOC_IS_JERICHO_PLUS_AND_BELOW(unit))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: 48B maid MEPs can only created with MEPs IDs that are multiplication of four. MEP id = %d.\n"),
                                 endpoint_info_lcl->id));
        }

        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Accelerated endpoint create\n")));

        SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

        if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID) {
            rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
            if (rv == BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                    (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
            } else if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /*Jumbo TLV MEPs steps is 16 in order to send Jumbo frams, it support 1k MEPs ccm as well */
            if( _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit) && (endpoint_info_lcl->id % 16 !=0)){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                     (_BSL_BCM_MSG("Error:  MEPs for Jumbo TLV can only created with MEPs IDs that are multiplication of 16. MEP id = %d.\n"),
                           endpoint_info_lcl->id));
            }
            /* mep id given by the user */
            flags = SHR_RES_ALLOC_WITH_ID;
        }
        if ((ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) && SOC_IS_JERICHO_PLUS_AND_BELOW(unit)){
           is_maid_48_jericho = 1; /*For 48 maid in jericho\Arad+, need to allocate specifec MEP IDs */
        }
        rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_SHORT), is_maid_48_jericho,
                                       BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP, (uint32 *)(&endpoint_info_lcl->id));
        BCMDNX_IF_ERR_EXIT(rv);
        new_id_alloced = TRUE;
        if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
            /* 
             * Verify that the endpoint ID is from 0 to 2k - 1 range.
             * If it fails, return BCM_E_PARAM
             */
            if (!_BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_CHECK(endpoint_info_lcl->id)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG
                                              ("Error: Valid endpoint range when custom_feature_oamp_flexible_da is set is 0 to 2k - 1.\n")));
            }
        }

        /*icc index allocation*/
        if (ma_name_struct.name_type != _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)
        {
            if (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_SHORT) {
                icc_ndx = 15;
            } else { /*allocate a pointer to ICCMap register that holds Bytes 5:0 of SW.group_name_data_base(group).name. */
                SOC_PPC_OAM_GROUP_NAME_TO_ICC_MAP_DATA(ma_name_struct.name, data);
                rv = _bcm_dpp_am_template_oam_icc_map_ndx_alloc(unit, 0 /*flags*/, &data, &is_allocated, &icc_ndx);
                BCMDNX_IF_ERR_EXIT(rv);
                allocate_icc_ndx = is_allocated;
            }
        }

        rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry, icc_ndx, &ma_name_struct, 0);
        BCMDNX_IF_ERR_EXIT(rv);
        rv =  OAM_ACCESS.acc_ref_counter.get(unit, &endpoint_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_ref_counter++;
        rv = OAM_ACCESS.acc_ref_counter.set(unit, endpoint_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl) && !is_upmep) {
            /* server down MEP. Set the traps.*/
            uint32 server_trap_ref_counter;

            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit,mep_db_entry.mep_type,
                                                            BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport),
                                                            &server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);
            server_trap_ref_counter++;
            rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit,mep_db_entry.mep_type,
                                                            BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport),
                                                            server_trap_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);

            if (server_trap_ref_counter == 1) {
                /* This trap code hasen't been allocated yet */
                if(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS){
                    mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP;
                } else if((mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)||(mep_db_entry.mep_type  == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)){
                    mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
                } else {
                    mep_type = mep_db_entry.mep_type ;
                }
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_set, (unit, mep_type, BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        /* At this stage we still may not configure the MEP-DB. The classifier must first be configured so that injected packets are not counted.*/
    }

    /*RX - classifier*/
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    /*tx_gport use to store system_port, which will use to build ITMH dest port for lb/tst when oam_ccm_rx_wo_tx soc enabled*/
    if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx){
        rv = _bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info_lcl->tx_gport, &(classifier_mep_entry.tx_gport));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if ((endpoint_info_lcl->gport == BCM_GPORT_INVALID) &&
        ((endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY) == 0) &&
        (endpoint_info_lcl->mpls_out_gport == BCM_GPORT_INVALID)) {
        if (!is_accelerated)  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: TX only: endpoint must be accelerated.\n")));
        }
        LOG_WARN(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM: Gport invalid - adding only TX.\n")));

        classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED;
        classifier_mep_entry.flags |= is_upmep ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;
        classifier_mep_entry.lif = _BCM_OAM_INVALID_LIF; /* Indication that RX does not exist */
        classifier_mep_entry.ma_index = endpoint_info_lcl->group;

        /* Store server data */
        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl)) {
            rv = _bcm_oam_endpoint_server_store_data(unit, &classifier_mep_entry, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (!is_upmep && BCM_GPORT_IS_TRUNK(endpoint_info_lcl->tx_gport))  {
            classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWNMEP_TX_GPORT_IS_LAG;
        }

        /* update sw db of ma->mep & mep_info with new mep */
        rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint32 profile_temp;
        rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
            /* It has already been verified that _WITH_ID flag has been set. Update the ID.*/
            _BCM_OAM_SET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_info_lcl->id, endpoint_info_lcl->id);
        } else if (!is_accelerated) { /* non-accelerated - allocate non-accelerated ID */
            endpoint_info_lcl->id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif, endpoint_info_lcl->level,
                                                                         _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry));
        }

        /* inserting an entry to the lem- the key is the in-lif the payload is the counter ID (mep ID)*/
        if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics && is_accelerated) {
            rv =  _bcm_oam_stat_lem_entry_add(unit, &classifier_mep_entry, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Arad+ in oam_classifier_advanced_mode=1 do not support MEP & MIP co-existance on same LIF, check and return error */
        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) &&
            !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
            /* MPLS Egress OEM-1 entries does not exist */
            if (!((endpoint_info_lcl->gport == BCM_GPORT_INVALID) &&
                  ((endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS) ||
                   (endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork) ||
                   (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
                   (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) ||
                   (endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection)))) {
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_oem_mep_mip_conflict_check, (unit, classifier_mep_entry.lif, is_mip));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        /* Configure two profile - one for actives side and one for passive side */
        for (is_passive = 0; is_passive <= 1; is_passive++) {
            SOC_PPC_LIF_ID lif   = is_passive ? classifier_mep_entry.passive_side_lif  : classifier_mep_entry.lif;
            profile_data_pointer = is_passive ? &profile_data_passive                  : &profile_data;


            /* In Arad Mode Passive profile is fixed and configured separately.
               In Jericho mode there is no need to configure the Passive profile except for MIPs where the other side isn't really passive.*/
            /* In Jericho for MIPs is_passive represents egress*/
            if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && is_passive && !(SOC_IS_JERICHO(unit) && is_mip)) {
                continue;
            }
            /* Skip Passive/Active profile create for MPLS Invalid LIF */
            if (is_passive) {
                if ((endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY) ||
                    ((classifier_mep_entry.passive_side_lif == _BCM_OAM_INVALID_LIF) &&
                     ((endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection)))) {
                    continue;
                }
            } else {
                if ((endpoint_info_lcl->gport == BCM_GPORT_INVALID) &&
                     ((endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) ||
                      (endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection))) {
                    continue;
                }
            }

            /* Check if action has been set on this LIF already for passive/active side
            *  The following loop runs and finds profile and mep by LIF and level
            *  Runs for Level = 0-7 and checks if it finds a profile.
            *  Result is the allocated profile for LIF for passive/active side.
            */
            check_lif_profile_found=0;
            for (check_lif_profile_counter=0; check_lif_profile_counter<=7; check_lif_profile_counter++) {
                if (check_lif_profile_found==0) {
                    soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                                 lif,
                                                                                                 check_lif_profile_counter,
                                                                                                 is_passive ^ is_upmep,
                                                                                                 &found,
                                                                                                 &check_lif_profile,
                                                                                                 &check_lif_profile_found,
                                                                                                 &is_mp_type_flexible,
                                                                                                 &found_mip);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

            /* Validate that mep with same lif&mdlevel does not exist with same direction */
            soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                         lif,
                                                                                         classifier_mep_entry.md_level,
                                                                                         is_passive ^ is_upmep,
                                                                                         &found,
                                                                                         &profile_temp,
                                                                                         &found_profile,
                                                                                         &is_mp_type_flexible,
                                                                                         &found_mip);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Check if endpoint already exist (only in case of active side) */
            if (!is_passive && found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                    (_BSL_BCM_MSG("Error: Local endpoint with lif %d, mdlevel %d and direction %d(1-up,0-down) exists.\n"),
                                     lif, endpoint_info_lcl->level,_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)));
            }
            if (SOC_IS_JERICHO_PLUS(unit)) {
               /* If there is an active SLM-MEP on the opposite side, we will find that the profile is ARAD_PP_SLM_PASSIVE_PROFILE
                  In this case we need to allocate a new active profile for this MEP */
               if ( (found_profile) && (profile_temp == ARAD_PP_SLM_PASSIVE_PROFILE)){
                     /* It must be the passive profile of an SLM-MEP on the other side,
                        so we need to allocate a new profile (active SLM profile) for this MEP */
                     is_slm_local = 1;
               } else {
                  if (found_profile == 2) {
                     /* if there is already a non-slm mep allocated on the lif, we'll get passive profile 0 */
                     /* if a passive profile found, update the non_acc_profile_passive with the passive profile */
                     soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(unit,
                                                                                                  classifier_mep_entry.passive_side_lif,
                                                                                                  classifier_mep_entry.md_level,
                                                                                                  !is_passive ^ is_upmep,
                                                                                                  &passive_found,
                                                                                                  &passive_profile_temp,
                                                                                                  &passive_found_profile,
                                                                                                  &passive_is_mp_type_flexible,
                                                                                                  &passive_found_mip);
                     classifier_mep_entry.non_acc_profile_passive = passive_profile_temp;
                  }
               }
            }

            profile = (int)profile_temp;  /* in case profile was not found, the value will be irrelevant */

            if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
               rv = _bcm_oam_get_reference_profile(unit, endpoint_info_lcl, classifier_mep_entry, is_passive, 0/*is_accelerated*/, &ref_profile);
               BCMDNX_IF_ERR_EXIT(rv);
            }

            if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit) ?
                found_profile == 1 /* In Jericho found_profile may equal 2 in which case only passive entries exist on OEM entry and  mp-profile may be overwriten.*/ :
                found_profile /* No such distinction yet in Arad*/) {

                if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
                  if ( check_lif_profile_found==1) {
                     if ( ref_profile != check_lif_profile) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Action profile is set on this lif, which is different from the reference profile.\n")));
                     }
                  }
                  if ( ref_profile != profile) {
                     BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Reference endpoint profile mismatch.\n")));
                  }
                  profile = ref_profile; /* Use Reference profile */
                }

                if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                    /* get profile data existing on this lif */
                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                        /* passive_active_mix and mep_mip mix configuration in Arad+ mode */
                        if (profile_data_pointer->mp_type_passive_active_mix && is_mip) {
                            /* Previous type was up/down, now trying to add mip */
                            if (is_mp_type_flexible) {
                                profile_data_pointer->mp_type_passive_active_mix = FALSE;
                                updated_mp_type = 1;
                            } else {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                    (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                  "Existing meps conflict with this constraint.\n")));
                            }
                        } else if (!profile_data_pointer->mp_type_passive_active_mix
                                   && !is_mip
                                   && !is_passive) {
                                  /* Previous type was mep/mip, now trying to add mep */
                                  /* Valid only in case the mep is on same direction as the existing mep, so looking for meps on the other direction */
                                  /* (active side check only) */
                                    rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                                 lif,
                                                                                 !_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry),
                                                                                 0/*find_mip*/,
                                                                                 &found,
                                                                                 &found_mip,
                                                                                 &profile_dummy);
                                    BCMDNX_IF_ERR_EXIT(rv);


                                  if (found & !found_mip) {
                                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                          (_BSL_BCM_MSG("Error: Only two combinations of meps on same lif are possible - mips and meps (all up or down) OR only meps (up and down)."
                                                                        "Existing meps conflict with this constraint.\n")));
                                  }
                        }
                    }
                } else if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
                    /* Arad mode */
                    if (!is_mip) {
                        if (profile == _BCM_OAM_PASSIVE_PROFILE) {
                            /* Get a new profile */
                            SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data_pointer);
                            rv = _bcm_oam_default_profile_get(unit, profile_data_pointer, &classifier_mep_entry, 0 /* non accelerated */, 0 /* is default */, is_upmep);
                            BCMDNX_IF_ERR_EXIT(rv);
                        } else {
                            /* get profile data existing on this lif */
                            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    } else {
                        /* This is a mip and adopts the direction of the existing mep */
                        if (profile == _BCM_OAM_PASSIVE_PROFILE) {
                            /* The existing mep is upmep */
                            classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP;
                            /* Getting the active profile */
                            soc_sand_rv = soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
                               unit, lif, classifier_mep_entry.md_level,1/*is_upmep*/,
                               &found, &profile_temp, &found_profile, &is_mp_type_flexible, &found_mip);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                            profile = (int)profile_temp;
                        }
                        /* get profile data existing on this lif */
                        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else { /* Jericho mode*/
                    /* Simply get profile data existing on this lif */
                    if (SOC_IS_JERICHO_PLUS(unit)) {
                        /* Mep mapping of template is invalid for reserved profiles */
                        if (profile != ARAD_PP_SLM_PASSIVE_PROFILE) {
                            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    } else {
                        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, profile_data_pointer);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                    is_piggyback_lm_enabled = profile_data_pointer->is_piggybacked;
                }
            } else { /* Profile not found */
               if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
                  rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, ref_profile, profile_data_pointer);
                  BCMDNX_IF_ERR_EXIT(rv);
               } else {
                  SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data_pointer);
                  rv = _bcm_oam_default_profile_get(unit, profile_data_pointer, &classifier_mep_entry, 0 /* non accelerated */, 0 /* is default */, is_passive ^ is_upmep);
                  BCMDNX_IF_ERR_EXIT(rv);
               }
            }

            /* Update timestamp format on profile if necessary */
            update_profile = 0;
            if (!is_mip && (profile_data_pointer->is_1588 != endpoint_info_lcl->timestamp_format)) {
                /* timestamp_format on new MEP is different from the other MEP/MIPs sharing the same profile.
                   updating profile according to the latest timeformat requested */
                profile_data_pointer->is_1588 = endpoint_info_lcl->timestamp_format;

                if (endpoint_info_lcl->timestamp_format) {
                    classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588;
                } else {
                    classifier_mep_entry.flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588;
                }

                update_profile = 1;
            }

            if (SOC_IS_JERICHO_PLUS(unit)) {
               profile_data_pointer->is_slm |= is_slm_local;
            }

            /* Allocate profile */
            if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
               profile = ref_profile;
               rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, SHR_TEMPLATE_MANAGE_SET_WITH_ID/*flags*/, profile_data_pointer, &is_allocated, &ref_profile);
               BCMDNX_IF_ERR_EXIT(rv); 
            } else {
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    if (profile != ARAD_PP_SLM_PASSIVE_PROFILE) {
                        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, profile_data_pointer, &is_allocated, &profile);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                } else {
                    rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, profile_data_pointer, &is_allocated, &profile);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                if ( SOC_IS_JERICHO_PLUS(unit) && (profile_data_pointer->is_slm) && (is_allocated)){
                    soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1); /* disable counting data packets for SLM */
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }


            if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type)) || update_profile) {
                /* Move all existing endpoints on this lif to the new profile */
                bcm_oam_endpoint_traverse_cb_profile_info_t profile_info;
                profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                profile_info.lif = lif;
                profile_info.profile_data = profile_data_pointer;
                profile_info.is_ingress = is_upmep ^ !is_passive;
                profile_info.cb_rv = BCM_E_NONE;
                /* Iterate over all groups to find all the endpoints on same lif and switch their profile */
                rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                BCMDNX_IF_ERR_EXIT(rv);

                if (profile_info.cb_rv != BCM_E_NONE) {
                    /* Function may fail inside the traverse API but not return an error. Handle errors through this field.*/
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: _bcm_oam_endpoint_exchange_profile_cb returned failure.\n")));
                }
            }

            if (is_passive) {
                classifier_mep_entry.non_acc_profile_passive = (uint8)profile;
                non_acc_profile_passive_alloc = is_allocated;
            } else {
                classifier_mep_entry.non_acc_profile = (uint8)profile;
                non_acc_profile_alloc = is_allocated;
            }

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type || !found_profile)) {
                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, profile, profile_data_pointer->mp_type_passive_active_mix);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } /* End of passive/active for loop*/

        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && (updated_mp_type)) {
            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry, 1 /*update mp-type*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc);
        /* New Arad+ Classifier: in new mode this should always be done */
        /* When using a remote OAMP as a server OAM-ID must be stamped on the FHEI. This is taken from the O-EM2 table*/
        if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !is_mip) ||
            is_accelerated || _BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {

           if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
              if( _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info_lcl->id) != _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info_lcl->action_reference_id)){
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Reference endpoint server/client mismatch.\n")));
              }
              rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint_info_lcl->action_reference_id, &profile_data_acc);
              BCMDNX_IF_ERR_EXIT(rv);
           } else {
              rv = _bcm_oam_default_profile_get(unit, &profile_data_acc, &classifier_mep_entry, is_accelerated, 0 /* is default */, is_upmep);
              BCMDNX_IF_ERR_EXIT(rv);
           }

           if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info_lcl)) {
              uint32 trap_code;
              uint8 trap_strength = 0;
              /* Gets the trap code from the remote gport, Sets mirror profile if required, Sets OAMP RX trap if required */
              rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                         endpoint_info_lcl->remote_gport, BCM_GPORT_INVALID, &trap_code, &trap_code);
              BCMDNX_IF_ERR_EXIT(rv);

              if (endpoint_info_lcl->remote_gport != BCM_GPORT_INVALID) {
                 trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info_lcl->remote_gport);
              }

              /* set the trap code for CCM m-cast to the above trap code.*/
              profile_data_acc.mep_profile_data.opcode_to_trap_code_multicast_map[SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM] = trap_code;
              profile_data_acc.mep_profile_data.opcode_to_trap_strength_multicast_map[SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM] = trap_strength;
              classifier_mep_entry.remote_gport = endpoint_info_lcl->remote_gport; /* Also used by OAM-1-2 and for get APIs*/

              if (!is_upmep) {
                 int trap_id;
                 int trap_id_sw;
                 /* update the trap code to calculate the FWD header offset*/
                 bcm_rx_trap_config_t trap_config;
                 bcm_rx_trap_config_t_init(&trap_config);

                 trap_id_sw = BCM_GPORT_TRAP_GET_ID(endpoint_info_lcl->remote_gport);
                 BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit, trap_id_sw, &trap_id));
                 rv = bcm_petra_rx_trap_get(unit, trap_id, &trap_config);
                 BCMDNX_IF_ERR_EXIT(rv);
                 trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                 trap_config.forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                 rv = bcm_petra_rx_trap_set(unit, trap_id, &trap_config);
                 BCMDNX_IF_ERR_EXIT(rv);
              }
           }

           if (endpoint_info_lcl->action_reference_id != BCM_OAM_ENDPOINT_INVALID) {
              rv = _bcm_oam_get_reference_profile(unit, endpoint_info_lcl, classifier_mep_entry, is_passive, 1/*is_accelerated*/, &ref_profile);
              BCMDNX_IF_ERR_EXIT(rv);
              profile = ref_profile;
              rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(unit, SHR_TEMPLATE_MANAGE_SET_WITH_ID/*flags*/, &profile_data_acc, &is_allocated, &ref_profile);
              BCMDNX_IF_ERR_EXIT(rv);
           } else {
              rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(unit, 0/*flags*/, &profile_data_acc, &is_allocated, &profile);
              BCMDNX_IF_ERR_EXIT(rv);
           }

           classifier_mep_entry.acc_profile = (uint8)profile;
           acc_profile_alloc = is_allocated;
        }

        count_disabled_counter_id = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_disable_lm_counting", _BCM_OAM_INVALID_DISABLE_COUNTER_ID);
        /* Do not enable counting if the counter_id that is passed indicates disabling of counting. */
        if (count_disabled_counter_id != endpoint_info_lcl->lm_counter_base_id) {
            /* Set counters and mark in/out LIF as OAM LIF */
            if (is_upmep) {
                 rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, classifier_mep_entry.passive_side_lif, classifier_mep_entry.lif, 1);
                 /* Piggy back LM is enabled on this LIF */
                 if(is_piggyback_lm_enabled) {
                     SHR_BITSET(profile_data.mep_profile_data.counter_disable, SOC_PPC_OAM_OPCODE_MAP_LMM);
                     profile_data.is_piggybacked = 1;
                 }
            } else {
                rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, classifier_mep_entry.lif, classifier_mep_entry.passive_side_lif, 1);
            }
        }
        BCMDNX_IF_ERR_EXIT(rv);

        /*don't create active profile if MPLS Egress only enable*/
        if (!((endpoint_info_lcl->gport == BCM_GPORT_INVALID) &&
              ((endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS) ||
               (endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork) ||
               (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
               (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) ||
               (endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection)))) {
            /* Active profile OAM table configuration */
            if(acc_profile_alloc)
            {
                soc_sand_rv = arad_pp_oam_classifier_entries_insert_accelerated(unit, &classifier_mep_entry, &profile_data_acc);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if(non_acc_profile_alloc)
            {
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    if (profile_temp != ARAD_PP_SLM_PASSIVE_PROFILE) {
                        soc_sand_rv = arad_pp_oam_classifier_entries_insert_active_non_accelerated(unit, _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info_lcl->id), &classifier_mep_entry,
                                &profile_data,
                                &profile_data_acc);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                } else {
                    soc_sand_rv = arad_pp_oam_classifier_entries_insert_active_non_accelerated(unit, _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_info_lcl->id), &classifier_mep_entry,
                            &profile_data,
                            &profile_data_acc);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }

        if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || (SOC_IS_JERICHO(unit) && is_mip)) {
            if(non_acc_profile_passive_alloc) {
                /* Don`t set the passive profile if MPLS Ingress only*/
                if ((endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS) ||
                    (endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork) ||
                    (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
                    (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) ||
                    (endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection)) {
                    /* Passive profile OAM table configuration */
                    if (endpoint_info_lcl->mpls_out_gport != BCM_GPORT_INVALID) {
                        soc_sand_rv = arad_pp_oam_classifier_entries_insert_passive_non_accelerated(unit, &classifier_mep_entry, &profile_data_passive);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    } else if ((endpoint_info_lcl->lm_counter_base_id > 0) &&
                            (((SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info_lcl)) &&
                              (endpoint_info_lcl->mpls_network_info.function_type != SOC_PPC_OAM_Y1711_LM_MPLS)) ||
                             (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe) ||
                             (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL))) {
                        soc_sand_rv = arad_pp_oam_classifier_entries_insert_passive_non_accelerated(unit, &classifier_mep_entry, &profile_data_passive);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                } else {
                    /* Passive profile OAM table configuration */
                    soc_sand_rv = arad_pp_oam_classifier_entries_insert_passive_non_accelerated(unit, &classifier_mep_entry, &profile_data_passive);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }

        
        /** Egress injection: set SW state before saving */
        if (SOC_IS_QAX(unit) && !is_upmep && (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && endpoint_info_lcl->vpn != 0){
            classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWN_MEP_INJECTION;
        }

        soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &classifier_mep_entry, update_profile ? 2 : 0 /*update*/, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Store server data */
        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl)) {
            rv = _bcm_oam_endpoint_server_store_data(unit, &classifier_mep_entry, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* update sw db of ma->mep & mep_info with new mep */
        rv = _bcm_dpp_oam_ma_to_mep_db_insert(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_accelerated) {
        if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) &&
            (ma_name_struct.name_type ==  _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
            mep_db_entry.maid_48_index = ma_name_struct.index;
            mep_db_entry.is_maid_48 = 1;
        }
        /* For Egress Injection(ETH OAM Counting) set VSI and 16LSB bits to Gen Mem */
        if(SOC_IS_QAX(unit) && !is_upmep && (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && ((endpoint_info_lcl->vpn != 0) || (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))) {

            mep_db_entry.out_lif = classifier_mep_entry.lif;

            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_set_mep_data_in_gen_mem, (unit, endpoint_info_lcl->id, classifier_mep_entry.lif/*lif_2*/, endpoint_info_lcl->vpn/*vsi*/));
            BCMDNX_IF_ERR_EXIT(rv);

            if ( endpoint_info_lcl->outer_tpid) {
                tpids[nof_tpids++] =  endpoint_info_lcl->outer_tpid;
                if ( endpoint_info_lcl->inner_tpid) {
                    tpids[nof_tpids++] =  endpoint_info_lcl->inner_tpid;
                }
                inner_tpid_only = ( endpoint_info_lcl->inner_tpid && ! endpoint_info_lcl->outer_tpid);

                /* get the TPID Profile */
                rv = _bcm_petra_vlan_translation_tpids_to_tpid_profile(unit,
                    tpids, nof_tpids, inner_tpid_only,&tpid_profile);
                BCMDNX_IF_ERR_EXIT(rv);
                /*TPID Profile should be set MEP_DB counter_pointer field(bits 0:1).*/
                mep_db_entry.counter_pointer = (tpid_profile & 0x3);
            }

        }

        /* Set Outlif for PWE+LSP */
        if ( (SOC_IS_QAX(unit)) && 
             (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) && 
             (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ)) {

            mep_db_entry.out_lif = classifier_mep_entry.lif;

            /* 16 lsb of the outlif (total 18bit) are stored in gen_mem the 2 msbs are determined by choosing one out of four oamp_pe_programs */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_set_mep_data_in_gen_mem, (unit, endpoint_info_lcl->id, (BCM_L3_ITF_VAL_GET(endpoint_info_lcl->mpls_out_gport) & 0xffff)/*lif_1*/, 0 /*lif_2*/));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Now that the classifier has been updated it is safe to transmit CCMs.*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, allocate_icc_ndx, ma_name_struct.name, is_new_ep);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_ARADPLUS(unit)) {
            /* Get the RDI in the eth1731 profile.*/
            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth_prof;
            uint32 user_header_size, user_header_0_size, user_header_1_size;
            uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth_prof);
            /* By default RDI is taken from scanner | RX */
            _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(eth_prof.rdi_gen_method, endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE,
                                                       endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE);

            eth_prof.maid_check_dis = SOC_IS_QAX(unit) && (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE);
            soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
                    unit,
                    &user_header_0_size,
                    &user_header_1_size,
                    &user_header_egress_pmf_offset_0_dummy,
                    &user_header_egress_pmf_offset_1_dummy
                    );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            user_header_size = (user_header_0_size + user_header_1_size) / 8;

            /*Set DM offsets */
            eth_prof.dmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 0 /* LMM */) + user_header_size;
            eth_prof.dmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 1 /* DMR */) + user_header_size;
            /*Set LM offsets */
            eth_prof.lmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_prof.piggy_back_lm, eth_prof.slm_lm, 0 /* not DM */, 0 /* LMM */) + user_header_size;
            eth_prof.lmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_prof.piggy_back_lm, eth_prof.slm_lm, 0 /* not DM */, 1 /* LMR */) + user_header_size;
            /* Phase for ccm transmit is defined by LSB bit of endpoint id and it's relevant for 48 maid only.
             * The reason for this is that maid 48 in QAX using external data ant it's take 80 clocks to transmit packets with external data.
             */          
            if (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)
                eth_prof.ccm_cnt = endpoint_info_lcl->id & 1;

                /* For MAID 48 in Jericho/Arad+ for each 8 Consecutive EPs, 
                 *  The first 4 EPs will be in phase 0 and the next 4 EPs will be in phase 1  
                 */
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)
                && (SOC_IS_JERICHO_PLUS_AND_BELOW(unit))
                && (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
                if (endpoint_info_lcl->id & 4) {
                    eth_prof.ccm_cnt = 1;
                } else {
                    eth_prof.ccm_cnt = 0;
                }
            }
            
            /* 
              *  Split DMM to 8 phases accoding to id 16, 32, 48, 64.... 
              *  Set DM phase here is used to allocate mep_profile in advance.
              */
            if  (_BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
                dmm_phase = ((endpoint_info_lcl->id/16) %8);
                if (dmm_phase  == 1) {
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 0 : 1;
                } else if (dmm_phase  == 2){
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 2 : 3;
                } else if (dmm_phase  == 3) {
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 4 : 5;
                } else if (dmm_phase  == 4){
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 6 : 7;
                } else if (dmm_phase  == 5){
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 8 : 9;
                } else if (dmm_phase  == 6) {
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 10 : 11;
                } else if (dmm_phase  == 7){
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 12 : 13;
                } else if (dmm_phase  == 0){
                    eth_prof.dmm_cnt = eth_prof.ccm_cnt ? 14 : 15;
                }
            }

            rv = _bcm_dpp_am_template_oam_eth_1731_mep_profile_RDI_alloc(unit, &eth_prof, &is_allocated, &profile);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info_lcl->id, !is_allocated, profile, &eth_prof);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* Update outlif profile for egress PWE counting */
    if (SOC_IS_JERICHO(unit) && (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe || endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) &&
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "eg_pwe_counting", 0) &&
        (endpoint_info_lcl->gport != BCM_GPORT_INVALID || endpoint_info_lcl->mpls_out_gport != BCM_GPORT_INVALID)) {
        rv = _bcm_oam_set_outlif_profile_to_count_eg_pwe(unit,endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ) {
       rv = _bcm_oam_set_outlif_profile_to_count_eg_pwe(unit,endpoint_info_lcl);
       BCMDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) && 
        (is_upmep || is_mip))
    {
        int local_out_lif = 0;
        int global_out_lif = 0;
        /* 
         * For egress snoop with egress multicast, outlif info are not in system header in snoop copy. 
         * If outlif info is needed, soc egress snooping advanced should be set. 
         * In etpp for 1st pass, HW get local outlif on Jericho, not global outlif. So mapping of local outlif to global outlif is done in 2nd pass. 
         * Here, mapping of local outlif to global outlif is added in LEM, when creating upmep or mip.
         */
        local_out_lif = (is_upmep) ? classifier_mep_entry.lif : classifier_mep_entry.passive_side_lif;

        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, local_out_lif, &global_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_oam_outlif_l2g_lem_entry_add(unit, local_out_lif, global_out_lif, 1/*add*/);
        BCMDNX_IF_ERR_EXIT(rv);

        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Inserting mapping of local outlif(%d) to global outlif(%d \n"),
                                local_out_lif, global_out_lif));
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Inserting local endpoint with id %d to group with id %d\n"),
                 endpoint_info_lcl->id, endpoint_info_lcl->group));
exit:
    /* In order to prevent unintended usage of the reference ep,  reset the ip after ep_create */
    endpoint_info_lcl->action_reference_id = BCM_OAM_ENDPOINT_INVALID;
    if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_id_alloced) {
        /* If error after new id was alloced we should free it */
        if (is_accelerated) { /* TX - OAMP */
            /* There's no point in checking the return value here. This
               is already an error containment code */
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint_info_lcl->id);
            if (allocate_icc_ndx) {
                /* coverity[check_return:FALSE] */
                _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, icc_ndx, &is_allocated/*dummy*/);
            }
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_ma_to_mep_db_mep_delete(unit, endpoint_info_lcl->group, endpoint_info_lcl->id);
            /* coverity[check_return:FALSE] */
            _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint_info_lcl->id);
        }
    }
    BCMDNX_FUNC_RETURN;
}


/* Handles creation of a new local RFC-6374 MEP. */
STATIC int _bcm_oam_rfc6374_endpoint_local_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl)
{
  int rv = BCM_E_NONE;
  uint32 soc_sand_rv = SOC_SAND_OK;
  uint8 is_accelerated = 0;
  uint8 is_inferred_lm = 0;
  int is_allocated = 0;
  int profile_ndx = 0;
  int profile;
  uint32 flags = 0;
  _bcm_oam_ma_name_t ma_name_struct;
  uint8 new_id_alloced = 0;
  uint8 found_profile;
  uint8 non_acc_profile_alloc=0;
  uint8 acc_profile_alloc = 0;
  uint32 count_disabled_counter_id = 0;
  uint32 user_header_size, user_header_0_size, user_header_1_size;
  uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
  uint32 no_of_slm_endpoints = 0, no_of_dlm_endpoints = 0;
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
  SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
  SOC_PPC_OAM_LIF_PROFILE_DATA profile_data_acc;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY oem1_key;
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD oem1_payload;
  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
  SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth_prof;

  BCMDNX_INIT_FUNC_DEFS;

  /* For coverity */
  BCM_DPP_UNIT_CHECK(unit);

  is_accelerated = endpoint_info_lcl->tx_gport != BCM_GPORT_INVALID;
  is_inferred_lm = ((endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) != 0);

  /* Fill classifier entry struct according to endpoint info */
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
  rv = _bcm_oam_classifier_rfc6374_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Check if OEM entry already exists */
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(&oem1_key);
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(&oem1_payload);
  oem1_key.oam_lif = classifier_mep_entry.lif;
  oem1_key.ingress = 1;
  soc_sand_rv = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, &oem1_key, &oem1_payload, &found_profile);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (found_profile) {
    /* Endpoint already exists on this LIF */
    BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Local endpoint on lif %d already exists.\n"), classifier_mep_entry.lif));
  }

  if (!is_accelerated) {
    /* Allocate non-accelerated endpoint ID */
    endpoint_info_lcl->id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(classifier_mep_entry.lif, 0 /* MD-Level */, 0 /* Down-MEP */);
  }

  if (is_inferred_lm) {
    rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
    BCMDNX_IF_ERR_EXIT(rv);
    no_of_slm_endpoints++;
    rv = OAM_ACCESS.no_of_slm_endpoints.set(unit, no_of_slm_endpoints);
    BCMDNX_IF_ERR_EXIT(rv);
  } else {
    rv = OAM_ACCESS.no_of_dlm_endpoints.get(unit, &no_of_dlm_endpoints);
    BCMDNX_IF_ERR_EXIT(rv);
    no_of_dlm_endpoints++;
    rv = OAM_ACCESS.no_of_dlm_endpoints.set(unit, no_of_dlm_endpoints);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  /* Configure non-accelerated active side profile */
  SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);
  rv = _bcm_oam_rfc6374_default_profile_get(unit, &profile_data, &classifier_mep_entry, 0 /* non-accelerated */);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Allocate non-accelerated profile */
  rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0 /* flags */, &profile_data, &is_allocated, &profile);
  BCMDNX_IF_ERR_EXIT(rv);

  if (is_allocated) {
    if (is_inferred_lm) {
      /* Disable counting of data packets for Inferred LM */
      soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0 /* packet_is_oam */, profile, 1 /* counter disable */);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      if (!SOC_IS_JERICHO_PLUS(unit)) {
        /* Enable counting of SLM packets */
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_slm_set, (unit, 1 /* is_slm */));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_counter_increment_bitmap_set,
                                         (unit, 1 /* enable_counting*/));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    } else {
      /* Enable counting of data packets for Direct LM */
      soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0 /* packet_is_oam */, profile, 0 /* counter enable */);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      if (!SOC_IS_JERICHO_PLUS(unit)) {
        /* Disable counting of SLM packets */
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_counter_increment_bitmap_set,
                                         (unit, 0 /* enable_counting */));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
    }
  }

  classifier_mep_entry.non_acc_profile = (uint8)profile;
  non_acc_profile_alloc = is_allocated;

  if (is_accelerated) {
    LOG_VERBOSE(BSL_LS_BCM_OAM,
            (BSL_META_U(unit,
                        "OAM: Accelerated endpoint create\n")));

    /* Configure accelerated profile */
    SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data_acc);

    rv = _bcm_oam_rfc6374_default_profile_get(unit, &profile_data_acc, &classifier_mep_entry, is_accelerated);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate accelerated profile */
    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(unit, 0 /* flags */, &profile_data_acc, &is_allocated, &profile);
    BCMDNX_IF_ERR_EXIT(rv);

    classifier_mep_entry.acc_profile = (uint8)profile;
    acc_profile_alloc = is_allocated;

    /* Allocate ednpoint ID */
    if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID) {
      rv = _bcm_dpp_oam_bfd_mep_id_is_alloced(unit, endpoint_info_lcl->id);
      if (rv == BCM_E_EXISTS) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                              (_BSL_BCM_MSG("Error: Local endpoint with id %d exists.\n"), endpoint_info_lcl->id));
      } else if (rv != BCM_E_NOT_FOUND) {
          BCMDNX_IF_ERR_EXIT(rv);
      }
      /* mep id given by the user */
      flags = SHR_RES_ALLOC_WITH_ID;
    }
    rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, 1, 0,
                                       BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP, (uint32 *)(&endpoint_info_lcl->id));
    BCMDNX_IF_ERR_EXIT(rv);
    new_id_alloced = TRUE;

    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    /* Translate the endpoint_info_t to oamp_mep_db struct */
    rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry, 0 /** icc_indx not relevant */, &ma_name_struct, 0 /** do_not_set_mep_db_ccm_interval not relevant*/);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_lm_dm_config, (unit, ((endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM) != 0), (endpoint_info_lcl->timestamp_format == bcmOAMTimestampFormatIEEE1588v1)));
    BCMDNX_IF_ERR_EXIT(rv);
  }
  /* Do not enable counting if the counter_id that is passed indicates disabling of counting */
  count_disabled_counter_id = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_disable_lm_counting", _BCM_OAM_INVALID_DISABLE_COUNTER_ID);
  if (count_disabled_counter_id != endpoint_info_lcl->lm_counter_base_id) {
    /* Set counters and mark In/Out-LIF as OAM LIF */
    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, classifier_mep_entry.lif, classifier_mep_entry.passive_side_lif, 1 /* OAM LIF */);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  /* Active profile OAMB table configuration */
  if (acc_profile_alloc) {
    soc_sand_rv = arad_pp_oam_classifier_rfc6374_entries_insert_accelerated(unit, &classifier_mep_entry, &profile_data_acc);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }

  /* Active profile OAMA table configuration */
  if (non_acc_profile_alloc) {
    soc_sand_rv = arad_pp_oam_classifier_rfc6374_entries_insert_active_non_accelerated(unit, &classifier_mep_entry, &profile_data);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }

  /* Exact match tables configuration */
  soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &classifier_mep_entry, 0 /* update */, 0 /* is_arm */);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  /* Insert endpoint info in SW state */
  rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
  BCMDNX_IF_ERR_EXIT(rv);
  /* After the classifier is set, it's safe to set up the OAMP MEP DB */
  if (is_accelerated) {
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, 0, new_id_alloced);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth_prof);

    soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0_dummy,
            &user_header_egress_pmf_offset_1_dummy
            );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    user_header_size = (user_header_0_size + user_header_1_size) / 8;

    /*Set DM offsets */
    eth_prof.dmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 0 /* LMM */) + user_header_size;
    eth_prof.dmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 1 /* DMR */) + user_header_size;
    /*Set LM offsets */
    eth_prof.lmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_prof.piggy_back_lm, eth_prof.slm_lm, 0 /* not DM */, 0 /* LMM */) + user_header_size;
    eth_prof.lmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_prof.piggy_back_lm, eth_prof.slm_lm, 0 /* not DM */, 1 /* LMR */) + user_header_size;

    /* No existing profile matches. Allocate a new one. Note: Add allocation check !*/
    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(unit ,&eth_prof, 0 /* flags */, &is_allocated, &profile_ndx);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info_lcl->id, !is_allocated, profile_ndx, &eth_prof);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }
  LOG_VERBOSE(BSL_LS_BCM_OAM,
              (BSL_META_U(unit, "Inserting local endpoint with id %d\n"), endpoint_info_lcl->id));

exit:
  BCMDNX_FUNC_RETURN;
}

/* Create new remote endpoint */
STATIC int _bcm_oam_endpoint_remote_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    int flags = 0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    uint8 found;
    uint8 new_id_alloced = FALSE;
    uint32 rmep_id_internal;
    uint32 rmep_scan_time;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Remote endpoint create.\n")));

    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    if (endpoint_info_lcl->loc_clear_threshold < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"),
                                          endpoint_info_lcl->loc_clear_threshold));
    }

    /* find associated MEP and check if it's accelerated */
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->local_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                            (_BSL_BCM_MSG("Error: Endpoint with id %d does not exist.\n"), endpoint_info_lcl->local_id));
    }
    if (! _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Endpoint with id %d is not accelerated, remote endpoint can not be added\n"),
                                          endpoint_info_lcl->local_id));
    }

    if ((endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
        if (!_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint_info_lcl->id)) {
            _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(endpoint_info_lcl->id, endpoint_info_lcl->id);
        }
        rv = bcm_dpp_am_oam_rmep_id_is_alloced(unit, endpoint_info_lcl->id);
        if (rv == BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: Remote endpoint with id %d exists\n"), endpoint_info_lcl->id));
        } else if (rv != BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* new rmep id given by the user */
        flags = SHR_RES_ALLOC_WITH_ID;
    }
    rv = bcm_dpp_am_oam_rmep_id_alloc(unit, flags, (uint32 *)&endpoint_info_lcl->id);
    BCMDNX_IF_ERR_EXIT(rv);
    new_id_alloced = TRUE;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: Inserting remote endpoint with id %d to mep with id %d\n"),
                 endpoint_info_lcl->id, endpoint_info_lcl->local_id));

    /*ccm_period - If not specified 3.5 times the transmitter MEP ccm_period*/
    if (endpoint_info_lcl->ccm_period == 0) {
        rv = _bcm_dpp_oam_endpoint_ccm_period_get(unit, endpoint_info_lcl->local_id,
                                                  &endpoint_info_lcl->ccm_period);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(!SOC_IS_QAX(unit))
    {
        /*lifetime_units = 2 not supported in ARAD and Jericho */
        rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
        if (SOC_SAND_DIV_ROUND_DOWN((endpoint_info_lcl->ccm_period)*1000/100, rmep_scan_time) > 0x3FF){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Maximal period should be less than %d mili seconds ."), (((0x400 * rmep_scan_time)* 100)/1000) -1));
        }
    }

    rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sw db of mep->rmep with the new rmep */
    rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(unit, endpoint_info_lcl->local_id, endpoint_info_lcl->id);
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_PPC_OAM_RMEP_INFO_DATA_clear(&rmep_info);
    rmep_info.mep_index = endpoint_info_lcl->local_id;
    rmep_info.rmep_id = endpoint_info_lcl->name;
    rv = _bcm_dpp_oam_bfd_rmep_info_db_insert(unit, endpoint_info_lcl->id, &rmep_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update entry in rmep db and rmep index db*/
    _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);
    soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal,  endpoint_info_lcl->name,
                                            endpoint_info_lcl->local_id, SOC_PPC_OAM_MEP_TYPE_ETH_OAM, &rmep_db_entry, 0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    if ((SOC_SAND_FAILURE(soc_sand_rv) || (BCM_FAILURE(rv))) && new_id_alloced) {
        /* If error after new id was alloced we should free it */
        bcm_dpp_am_oam_rmep_id_dealloc(unit, endpoint_info_lcl->id);
        _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(unit, endpoint_info_lcl->local_id, endpoint_info_lcl->id);
        _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint_info_lcl->id);
    }
    BCMDNX_FUNC_RETURN;
}

/* Create new default endpoint */
STATIC int _bcm_oam_endpoint_default_create(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                            ARAD_PP_OAM_DEFAULT_EP_ID default_id)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;

    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data = NULL;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    int profile, is_allocated;
    uint8 is_upmep, found;

    BCMDNX_INIT_FUNC_DEFS;

    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);

    BCMDNX_ALLOC(profile_data, sizeof(*profile_data),"_bcm_oam_endpoint_default_create.encap_entry_info");
    if(profile_data == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) { /* Arad default profile */

        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
        SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data);

        classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_ETH_OAM; /* Irrelevant to default. Setting to 0 */
        classifier_mep_entry.flags |= is_upmep ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP : 0;
        classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
        classifier_mep_entry.non_acc_profile = SOC_PPC_OAM_PROFILE_DEFAULT;
        rv = _bcm_oam_default_profile_get(unit, profile_data, &classifier_mep_entry, 0 /* non accelerated */, 1 /* is default */, is_upmep);
        BCMDNX_IF_ERR_EXIT(rv);

        profile = 0;
        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, SHR_TEMPLATE_MANAGE_SET_WITH_ID, profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, -1, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, profile_data, &classifier_mep_entry, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    } else if (SOC_IS_ARADPLUS(unit)) { /* Arad+/Jericho default profiles */

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                                (_BSL_BCM_MSG("Error: Default endpoint already exists.\n")));
        }

        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
        SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data);

        rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info_lcl, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_oam_default_profile_get(unit, profile_data, &classifier_mep_entry,
                                          0 /* non accelerated */, 1 /* is default */, is_upmep);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(unit, 0/*flags*/, profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);

        classifier_mep_entry.non_acc_profile = (uint8)profile;

        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                          (unit, default_id,&classifier_mep_entry, 0/*update_action_only*/));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_DPP_UNIT_CHECK(unit);
        soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, profile_data, &classifier_mep_entry, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_dpp_oam_bfd_mep_info_db_insert(unit, endpoint_info_lcl->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCM_FREE(profile_data);
    BCMDNX_FUNC_RETURN;
}

/* The state of the internal registers determining outgoing packets may have been changed, In this case outgoing LBMs
   must reflect this change.*/
STATIC int _bcm_oam_endpoint_loopback_update_for_local_mep_replace(int unit, bcm_oam_endpoint_info_t *endpoint)
{

    int rv = 0;
    bcm_oam_loopback_t lb = { 0 };
    bcm_oam_loopback_t lb_get = { 0 };
    uint32 lsbs, msbs;
    int dummy;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_loopback_t_init(&lb);
    bcm_oam_loopback_t_init(&lb_get);

    lb_get.id = endpoint->id;
    
    /* bcm_petra_oam_loopback_get sets values to fields that
       are forbidden during set. Using a different object for
       the update copying only the relevant fields */
    rv = bcm_petra_oam_loopback_get(unit, &lb_get);
    BCMDNX_IF_ERR_EXIT(rv);

    if (lb_get.int_pri ==-1 && lb_get.pkt_pri==0xff ) { 
        /* If the ITMH TC,DP or the VLAN priority fields are set directly with loopback_add() then there is no reason to update here.
           int_pri is set to -1 in init and will be updated by the user only if the user wants to set it directly. Similarly for pkt_pri*/
        lb.id = endpoint->id;
        lb.flags = lb_get.flags | BCM_OAM_LOOPBACK_UPDATE;
        lb.period = lb_get.period;
        if (endpoint->type == bcmOAMEndpointTypeEthernet) {
            rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit, endpoint->id, &dummy, &msbs);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit, endpoint->id, &dummy, &lsbs);
            BCMDNX_IF_ERR_EXIT(rv);
            SET_MAC_ADDRESS_BY_MSB_AND_LSB(lb.peer_da_mac_address, lsbs, msbs);
        }

        if (lb_get.num_tlvs) {
            sal_memcpy(&lb.tlvs[0], &lb_get.tlvs[0], sizeof(bcm_oam_tlv_t) * lb_get.num_tlvs);
            lb.num_tlvs = lb_get.num_tlvs;
        }

        rv = bcm_petra_oam_loopback_add(unit, &lb);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*Update scan phase count of different PDUs when CCM transmission is enabled on a RX-only MEP */
STATIC void _bcm_oam_endpoint_local_scan_count_update_after_ccm_tx_enable(int unit,
                                                                            uint8 set_ccm_cnt_in_scan_profile,
                                                                            int ccm_period,
                                                                            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *old_prof) {
    uint8 ccm_transmit_phase_is_odd=0;

    /*Now recalculate the scan_phase count for LM/DM*/
    ccm_transmit_phase_is_odd = set_ccm_cnt_in_scan_profile & 1;
    /* Now set the phase*/
    if (ccm_period >= 2) {
        /* this case is easy*/
        if (old_prof->dmm_rate == 1) {
            old_prof->dmm_cnt =  (ccm_transmit_phase_is_odd) ? 0 : 1;
            if (old_prof->lmm_rate) {
                old_prof->lmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
        } else {
            if (old_prof->dmm_rate) {
                old_prof->dmm_cnt =  (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
            if (old_prof->lmm_rate) {
                old_prof->lmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
            }
        }
    } else {
        uint8 updated_lmm_cnt=0, updated_dmm_cnt=0;
        if ((old_prof->lmm_rate == 0) || ((old_prof->dmm_rate > old_prof->lmm_rate) && old_prof->lmm_rate)) {
            updated_dmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
        }
        if ((old_prof->dmm_rate == 0)|| ((old_prof->lmm_rate > old_prof->dmm_rate) && old_prof->dmm_rate)) {
            updated_lmm_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
        }
        if (((old_prof->dmm_rate >= old_prof->lmm_rate) && old_prof->lmm_rate)) {
            updated_lmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
        }
        if (((old_prof->lmm_rate >= old_prof->dmm_rate) && old_prof->dmm_rate)) {
            updated_dmm_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
        }

        old_prof->lmm_cnt = updated_lmm_cnt;
        old_prof->dmm_cnt = updated_dmm_cnt;
        old_prof->op_1_cnt = (ccm_transmit_phase_is_odd) ? 4 : 5; /*fifth slot modulo 6 is always available. */
        old_prof->op_0_cnt = (ccm_transmit_phase_is_odd) ? 10 :11;
    }

}

/* Update local MEP/MIP */
STATIC int _bcm_oam_endpoint_local_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                                 bcm_oam_endpoint_info_t *existing_endpoint_info,
                                                 SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    int is_new_ep = 0;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    _bcm_oam_ma_name_t ma_name_struct;
    int is_last;
    int icc_ndx = 0;
    int is_allocated;
    int profile;
    uint8 is_accelerated = 0;
    uint8 is_upmep;
    uint8 found;
    int do_not_set_mep_db_ccm_interval = 0, set_ccm_cnt_in_scan_profile = 0, prev_ccm_period_ms = 0, prev_ccm_period_micro_s = 0;
    int prev_ctr_base_id = 0, curr_ctr_base_id = 0;
    uint32 count_disabled_counter_id = 0;
    uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT] = {0};
    int nof_tpids = 0;
    int inner_tpid_only = 0;
    uint32 tpid_profile = 0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_oam_endpoint_local_replace_validity_check(unit, endpoint_info_lcl,
                                                        existing_endpoint_info,
                                                        classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    is_upmep = _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info_lcl);
    is_accelerated = _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info_lcl) || (classifier_mep_entry->is_rfc_6374_entry && existing_endpoint_info->tx_gport != BCM_GPORT_INVALID);

    if (!_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info_lcl)) {
      /* Check that the group exist - needed for both accelerated and not accelerated */
      rv = _bcm_dpp_oam_ma_db_get(unit, (uint32)endpoint_info_lcl->group, &ma_name_struct, &found);
      BCMDNX_IF_ERR_EXIT(rv);
      if (!found) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                              (_BSL_BCM_MSG("Error: group %d does not exist.\n"), endpoint_info_lcl->group));
      }

      /* Claculating the phase for ccm transmit.
       * For MAID 48 in QAX the phase is according the lsb of endpoint id all other cases the phase is 0.
       */
      if (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)
           set_ccm_cnt_in_scan_profile = endpoint_info_lcl->id & 1;

      /* For MAID 48 in Jericho/Arad+ for each 8 Consecutive EPs,
       *  The first 4 EPs will be in phase 0 and the next 4 EPs will be in phase 1
       */
      if ((ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)
          && (SOC_IS_JERICHO_PLUS_AND_BELOW(unit))
          && (ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
          if (endpoint_info_lcl->id & 4) {
              set_ccm_cnt_in_scan_profile = 1;
          } else {
              set_ccm_cnt_in_scan_profile = 0;
          }
      }
    }

    if (is_accelerated) { /* TX - OAMP */
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Accelerated endpoint update\n")));

        /* First, clear the initial data*/
        SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

        /*get entry of mep db*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_info_lcl->id, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* Do not update ccm interval in mep db if it is a replace case from
         * (previous) ccm_period = NON-ZERO to (current) ccm_period = ZERO.
         * This is to avoid setting 0 in HW MEP_DB entry, so that period check on RX still works.
         * Instead we would modify ccm_cnt to 0xfffff to stop TX of CCM packets.
         *
         * NOTE: If we happen to go back to a non-zero value, automatically
         * mep_db_entry would get updated since endpoint_info->ccm_period will be non-zero
         * and hence do_not_set_mep_db_ccm_interval will be equal to 0.
         */
        if (SOC_IS_QAX(unit)) {
            SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(prev_ccm_period_ms, prev_ccm_period_micro_s, mep_db_entry.ccm_interval); 
            if (!endpoint_info_lcl->ccm_period && (prev_ccm_period_ms || prev_ccm_period_micro_s)) {
                do_not_set_mep_db_ccm_interval = 1;
                set_ccm_cnt_in_scan_profile = SOC_PPC_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT; /* 20 bits CCM_CNT field size */
            }
        }
        /* Down MEP, may be switching to "fake" destination. Save the original one. */
        classifier_mep_entry->dip_profile_ndx = existing_endpoint_info->tx_gport;
        classifier_mep_entry->src_mac_lsb = existing_endpoint_info->src_mac_address[5];
        rv = _bcm_oam_mep_db_entry_dealloc(unit, endpoint_info_lcl->id, &mep_db_entry, classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        icc_ndx = mep_db_entry.icc_ndx;

        SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

        rv = _bcm_oam_mep_db_entry_struct_set(unit, endpoint_info_lcl, &mep_db_entry, icc_ndx, &ma_name_struct, do_not_set_mep_db_ccm_interval);
        BCMDNX_IF_ERR_EXIT(rv);

       if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) &&
           (ma_name_struct.name_type ==  _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE)) {
            mep_db_entry.maid_48_index = ma_name_struct.index;
            mep_db_entry.is_maid_48 = 1;
       }

       /* For Egress Injection(ETH OAM Counting) set VSI and 16LSB bits to Gen Mem */
       if(SOC_IS_QAX(unit) && !is_upmep && (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && ((endpoint_info_lcl->vpn != 0) || (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))) {

            mep_db_entry.out_lif = classifier_mep_entry->lif;

            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_set_mep_data_in_gen_mem, (unit, endpoint_info_lcl->id, classifier_mep_entry->lif, endpoint_info_lcl->vpn));
            BCMDNX_IF_ERR_EXIT(rv);

            if ( endpoint_info_lcl->outer_tpid) {
                tpids[nof_tpids++] =  endpoint_info_lcl->outer_tpid;
                if ( endpoint_info_lcl->inner_tpid) {
                    tpids[nof_tpids++] =  endpoint_info_lcl->inner_tpid;
                }
                inner_tpid_only = ( endpoint_info_lcl->inner_tpid && ! endpoint_info_lcl->outer_tpid);

                /* Resolve the TPID Profile */
                rv = _bcm_petra_vlan_translation_tpids_to_tpid_profile(unit,
                    tpids, nof_tpids, inner_tpid_only,&tpid_profile);
                BCMDNX_IF_ERR_EXIT(rv);

                /*TPID Profile should be set MEP_DB counter_pointer field(bits 0:1).*/
                mep_db_entry.counter_pointer = (tpid_profile & 0x3);
            }

       }


       /* Set Outlif for PWE+LSP */
       if ( (SOC_IS_QAX(unit)) && 
            (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) && 
            (endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ)) {

           mep_db_entry.out_lif = classifier_mep_entry->lif;

           /* 16 lsb of the outlif (total 18bit) are stored in gen_mem the 2 msbs are determined by choosing one out of four oamp_pe_programs */
           rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_set_mep_data_in_gen_mem, (unit, endpoint_info_lcl->id, (BCM_L3_ITF_VAL_GET(endpoint_info_lcl->mpls_out_gport) & 0xffff), 0));
           BCMDNX_IF_ERR_EXIT(rv);
       }

        /*update entry in mep db*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, endpoint_info_lcl->id, &mep_db_entry, 0, ma_name_struct.name , is_new_ep);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_ARADPLUS(unit)) {
            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  old_prof;
            uint8 new_rdi;
            int new_profile;
            /* RDI may have been updated 
             *        (OR) 
             * In case of QAX(and above), CCM_CNT might need to be moved from 0/1(in QAX and maid 48) to 0xfffff
             * or viceversa for disabling/enabling CCM TX
 */
            _BCM_OAM_SET_RDI_ON_ETH1731_PROF_RDI_FIELD(new_rdi, endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE,
                                                       endpoint_info_lcl->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE);
            rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, endpoint_info_lcl->id, &profile, &old_prof);
            BCMDNX_IF_ERR_EXIT(rv);

            if (SOC_IS_QAX(unit)) {
                if ((endpoint_info_lcl->ccm_period) && (old_prof.ccm_cnt == SOC_PPC_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT)) {
                    _bcm_oam_endpoint_local_scan_count_update_after_ccm_tx_enable(unit,
                            set_ccm_cnt_in_scan_profile, /* CCM_CNT updated from local endpoit id (0/1) */ 
                            endpoint_info_lcl->ccm_period, /* period of CCM transmission */
                            &old_prof); /* scan profile to be updated after transmission enable. older one is updated and used */
                }
            }

            if ((old_prof.rdi_gen_method != new_rdi)|| ((old_prof.ccm_cnt != set_ccm_cnt_in_scan_profile)&&(SOC_IS_QAX(unit)))) {
                old_prof.ccm_cnt = set_ccm_cnt_in_scan_profile;
                old_prof.rdi_gen_method = new_rdi;
                rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, profile, &old_prof, &is_allocated, &is_last, &new_profile);
                BCMDNX_IF_ERR_EXIT(rv);

                if (profile != new_profile) {
                    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, profile, endpoint_info_lcl->id);
                }

                soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_info_lcl->id, !is_allocated, new_profile, &old_prof);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
                /* Make sure LBMs are also updated */
                rv = _bcm_oam_endpoint_loopback_update_for_local_mep_replace(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    /*RX - classifier*/

    if (endpoint_info_lcl->gport != BCM_GPORT_INVALID ||
        _BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl) ||
        ((classifier_mep_entry->lif == _BCM_OAM_INVALID_LIF) &&
         ((classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
          (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
          (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
          (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
          (classifier_mep_entry->mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
          (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;
        SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
        new_classifier_mep_entry.flags = classifier_mep_entry->flags; /* copy the flags separatly. These are not all updated in the next function.*/
        if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info_lcl)) {
          rv = _bcm_oam_classifier_rfc6374_mep_entry_struct_set(unit, endpoint_info_lcl, &new_classifier_mep_entry);
          BCMDNX_IF_ERR_EXIT(rv);
        } else {
          rv = _bcm_oam_classifier_mep_entry_struct_set(unit, endpoint_info_lcl, &new_classifier_mep_entry);
          BCMDNX_IF_ERR_EXIT(rv);
        }
        /* mep profiles weren't changed. Update the pointers from the old entry*/
        new_classifier_mep_entry.non_acc_profile = classifier_mep_entry->non_acc_profile;
        new_classifier_mep_entry.acc_profile = classifier_mep_entry->acc_profile;
        new_classifier_mep_entry.non_acc_profile_passive = classifier_mep_entry->non_acc_profile_passive;
        new_classifier_mep_entry.dip_profile_ndx = classifier_mep_entry->dip_profile_ndx;
        new_classifier_mep_entry.sd_sf_id= classifier_mep_entry->sd_sf_id;
        new_classifier_mep_entry.y1711_sd_sf_id= classifier_mep_entry->y1711_sd_sf_id;
        new_classifier_mep_entry.session_map_flag= classifier_mep_entry->session_map_flag;
        new_classifier_mep_entry.tx_gport= classifier_mep_entry->tx_gport;
        count_disabled_counter_id = 
            soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_disable_lm_counting", _BCM_OAM_INVALID_DISABLE_COUNTER_ID);
        prev_ctr_base_id = classifier_mep_entry->counter;
        curr_ctr_base_id = endpoint_info_lcl->lm_counter_base_id;
        if (curr_ctr_base_id != prev_ctr_base_id) {
            if ((prev_ctr_base_id == count_disabled_counter_id) && (curr_ctr_base_id != count_disabled_counter_id)) {
                /* prev_ctr_base_id == disable -> curr_ctr_base_id != disable --> Set OAM LIF  */
                if (is_upmep) {
                    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, new_classifier_mep_entry.passive_side_lif, new_classifier_mep_entry.lif, 1);
                } else {
                    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, new_classifier_mep_entry.lif, new_classifier_mep_entry.passive_side_lif, 1);
                }
            } else if ((prev_ctr_base_id != count_disabled_counter_id) && (curr_ctr_base_id == count_disabled_counter_id)) { 
                /* prev_ctr_base_id != disable -> curr_ctr_base_id == disable --> Reset OAM LIF */
                if (is_upmep) {
                    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, new_classifier_mep_entry.passive_side_lif, new_classifier_mep_entry.lif, 0);
                } else {
                    rv = _bcm_dpp_endpoint_counters_set(unit, endpoint_info_lcl, new_classifier_mep_entry.lif, new_classifier_mep_entry.passive_side_lif, 0);
                }
            } else if (endpoint_info_lcl->lm_counter_base_id > 0) { 
                /* prev_ctr_base_id != disable -> curr_ctr_base_id !=disable with just change in counter ids */
                rv = _bcm_dpp_oam_set_counter(unit, endpoint_info_lcl->lm_counter_base_id,
                                              (endpoint_info_lcl->lm_flags & BCM_OAM_LM_PCP),
                                              is_upmep ? new_classifier_mep_entry.passive_side_lif :
                                                         new_classifier_mep_entry.lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info_lcl->id, &new_classifier_mep_entry, 1/*update*/, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* Store server data */
        if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info_lcl)) {
            rv = _bcm_oam_endpoint_server_store_data(unit, &new_classifier_mep_entry, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info_lcl->id, &new_classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Update Remote MEP */
STATIC int _bcm_oam_endpoint_remote_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl)
{

    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;
    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    uint8 found;
    uint32 rmep_id_internal;
    uint32 rmep_scan_time;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint_info_lcl->id, &rmep_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
           (_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"), endpoint_info_lcl->id));
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                                            "OAM: Remote endpoint replace.\n")));

    if (endpoint_info_lcl->loc_clear_threshold < 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: loc_clear_threshold must be positive. Current value:%d\n"),
                                endpoint_info_lcl->loc_clear_threshold));
    }

    /* Check parameters that can not be changed */
    if (endpoint_info_lcl->local_id != rmep_info.mep_index) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local id can not be changed. Current value:%d\n"),
                                rmep_info.mep_index));
    }

    /*ccm_period*/
    if (endpoint_info_lcl->ccm_period==0) {
        rv = _bcm_dpp_oam_endpoint_ccm_period_get(unit, endpoint_info_lcl->local_id,
                                                  &endpoint_info_lcl->ccm_period);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(!SOC_IS_QAX(unit))
    {
        /*lifetime_units = 2 not supported in ARAD and Jericho */
        rmep_scan_time = _ARAD_PP_OAM_RMEP_SCAN(unit,arad_chip_kilo_ticks_per_sec_get(unit));
        if (SOC_SAND_DIV_ROUND_DOWN(((endpoint_info_lcl->ccm_period*1000)/100), rmep_scan_time) > 0x3FF){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Maximal period should be less than %d mili seconds ."), (((0x400 * rmep_scan_time)* 100)/1000) -1));
       }
    }


    SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);

    _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);

    rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_id_internal);
    BCMDNX_IF_ERR_EXIT(rv);



    rv = _bcm_oam_rmep_db_entry_struct_set(unit, endpoint_info_lcl, &rmep_db_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sw db of rmep with the new rmep */
    rv = _bcm_dpp_oam_bfd_rmep_to_mep_db_update(unit, endpoint_info_lcl->id, &rmep_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update entry in rmep db and rmep index db*/
    _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint_info_lcl->id);
    soc_sand_rv = soc_ppd_oam_oamp_rmep_set(unit, rmep_id_internal, endpoint_info_lcl->name,
                                            endpoint_info_lcl->local_id, endpoint_info_lcl->type,
                                            &rmep_db_entry, 1);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Update default MEP (LIF-Profile endpoint)*/
STATIC int _bcm_oam_endpoint_default_replace(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                             bcm_oam_endpoint_info_t *existing_endpoint_info,
                                             SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                             ARAD_PP_OAM_DEFAULT_EP_ID default_id)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = 0;

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY new_classifier_mep_entry;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_oam_endpoint_local_replace_validity_check(unit, endpoint_info_lcl,
                                                        existing_endpoint_info,
                                                        classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&new_classifier_mep_entry);
    rv = _bcm_oam_classifier_default_mep_entry_struct_set(unit, endpoint_info_lcl,
                                                          &new_classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* mep profiles weren't changed. Update the pointers from the old entry*/
    new_classifier_mep_entry.non_acc_profile = classifier_mep_entry->non_acc_profile;

    if (endpoint_info_lcl->lm_counter_base_id != classifier_mep_entry->counter) {
        if (endpoint_info_lcl->lm_counter_base_id > 0) {
            rv = _bcm_dpp_oam_set_counter(unit, endpoint_info_lcl->lm_counter_base_id,
                                          ((endpoint_info_lcl->lm_flags & BCM_OAM_LM_PCP) != 0),
                                          _BCM_OAM_INVALID_LIF);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                      (unit, default_id,&new_classifier_mep_entry, 0/*update_action_only*/));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_info_lcl->id, &new_classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 found;
    uint8 is_default = 0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint8 oam_is_init;
    bcm_oam_endpoint_info_t endpoint_info_lcl_t;
    bcm_oam_endpoint_info_t *endpoint_info_lcl = NULL;
    bcm_oam_endpoint_info_t existing_endpoint_info; /* For replacing */

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(endpoint_info);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    endpoint_info_lcl = &endpoint_info_lcl_t;
    sal_memcpy(endpoint_info_lcl, endpoint_info, sizeof(bcm_oam_endpoint_info_t));

    /*
     * If the GPORT contains a trap, since the trap id inside
     * the GPORT can be either the HW id or the SW_id (according
     * to the use_hw_id SOC peroperty), and this function assumes
     * that the trap id is the SW id, it will use a copy of the
     * endpoint_info with the SW id
     */
    if (BCM_GPORT_IS_TRAP(endpoint_info_lcl->remote_gport)) {
        _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, endpoint_info_lcl->remote_gport);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info_lcl->id);

    /* preliminary validity check of endpoint_info. Relavent only when type==Eth */

    rv = _bcm_oam_endpoint_validity_checks_all(unit, endpoint_info_lcl);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_default) {
        /* Check flags limitations */
        rv = _bcm_oam_default_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_oam_default_id_from_ep_id(unit, endpoint_info->id, &default_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* Not default */
        rv = _bcm_oam_generic_endpoint_validity_checks(unit, endpoint_info_lcl);
        BCMDNX_IF_ERR_EXIT(rv);

        if (endpoint_info_lcl->type == bcmOAMEndpointTypeEthernet) {
            rv = _bcm_oam_ethernet_endpoint_validity_checks(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (endpoint_info_lcl->type == bcmOAMEndpointTypeBHHMPLS || 
                   endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPwe  ||
                   endpoint_info_lcl->type == bcmOAMEndpointTypeMPLSNetwork ||
                   endpoint_info_lcl->type == bcmOAMEndpointTypeBhhSection ||
                   endpoint_info_lcl->type == bcmOAMEndpointTypeBHHPweGAL) {
            rv = _bcm_oam_mpls_pwe_endpoint_validity_checks(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info_lcl)) {
            rv = _bcm_oam_rfc6374_endpoint_validity_checks(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: Supporting only Ethernet, BHH-MPLS%stype.\n"),
                                 SOC_IS_JERICHO(unit) ? ", BHH-PWE and RFC6374 " : " "));
        }
    }
    /* preliminary validity check complete */

    /* Endpoint replace */
    if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_REPLACE) {
        /* Retrieve the existing endpoint information */
        bcm_oam_endpoint_info_t_init(&existing_endpoint_info);
        rv = _bcm_oam_endpoint_get_internal_wrapper(unit, endpoint_info_lcl->id, &existing_endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (existing_endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
            rv = _bcm_oam_endpoint_remote_replace(unit, endpoint_info_lcl);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info_lcl->id, &classifier_mep_entry, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                   (_BSL_BCM_MSG("Local endpoint with id %d can not be found.\n"),
                    endpoint_info_lcl->id));
            }

            if (is_default) {
                rv = _bcm_oam_endpoint_default_replace(unit, endpoint_info_lcl,
                                                             &existing_endpoint_info,
                                                             &classifier_mep_entry,
                                                             default_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                rv = _bcm_oam_endpoint_local_replace(unit, endpoint_info_lcl,
                                                           &existing_endpoint_info,
                                                           &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    else { /* New endpoint */
        /* Default OAM trap */
        if (is_default) {
            rv = _bcm_oam_endpoint_default_create(unit, endpoint_info_lcl, default_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else { /* Non-default new endpoint */
            if (endpoint_info_lcl->flags & BCM_OAM_ENDPOINT_REMOTE) {
                /* New remote MEP */
                rv = _bcm_oam_endpoint_remote_create(unit, endpoint_info_lcl);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* New local MIP/MEP */
                if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info_lcl)) {
                  rv = _bcm_oam_rfc6374_endpoint_local_create(unit, endpoint_info_lcl);
                  BCMDNX_IF_ERR_EXIT(rv);
                } else {
                  rv = _bcm_oam_endpoint_local_create(unit, endpoint_info_lcl);
                  BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    BCM_EXIT;

exit:
    if (endpoint_info_lcl) {
        sal_memcpy(endpoint_info, endpoint_info_lcl, sizeof(bcm_oam_endpoint_info_t));
    }
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW
       id and thus we need to convert it to the HW id becuase the oam logic uses the SW id */
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport, rv);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, bcm_oam_endpoint_info_t *endpoint_info)
{
    uint32 soc_sand_rv=0;
    int rv;
    uint8 found;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 oam_is_init;
    uint8 is_default;
    uint32 dummy, read_vpn;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint);

    if ((endpoint < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), endpoint));
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    endpoint_info->id = endpoint;
    /* Default OAM trap */
    if (is_default) {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Default profile not configured.\n")));
        }

        endpoint_info->flags |= _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry) ? BCM_OAM_ENDPOINT_UP_FACING : 0;
        if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
            endpoint_info->group = classifier_mep_entry.ma_index;
            endpoint_info->level = classifier_mep_entry.md_level;
            endpoint_info->lm_counter_base_id = classifier_mep_entry.counter;
        }

        BCM_EXIT;
    }

    if (_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)) {
        /* remote endpoint */ 
        SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
        uint32 rmep_id_internal;
        SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry;

        SOC_PPC_OAM_RMEP_INFO_DATA_clear(&rmep_info);
        SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(&rmep_db_entry);
        /* verify that rmep exists */
        rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint, &rmep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }

        /* get entry from oamp */
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint);
        soc_sand_rv = soc_ppd_oam_oamp_rmep_get(unit, rmep_id_internal, &rmep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_oam_rmep_db_entry_struct_get(unit, &rmep_db_entry, rmep_info.rmep_id, rmep_info.mep_index, endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* local endpoint */
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
        }
        /* RX - classifier: Must be first. Accelerated parts relay on fields in "endpoint_info" filled here. */
        if ((classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) ||
            ((classifier_mep_entry.lif == _BCM_OAM_INVALID_LIF) &&
             ((classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
              (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
              (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
              (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
              (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
              (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
            rv = _bcm_oam_classifier_mep_entry_struct_get(unit, endpoint_info, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint)) {
               /* In addition the remote gport field needs to be filled (This was set in the classifier struct accordingly) */
               endpoint_info->remote_gport = classifier_mep_entry.remote_gport;
            }
        } else {
            endpoint_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
            endpoint_info->group = classifier_mep_entry.ma_index;
            endpoint_info->gport = BCM_GPORT_INVALID;
        }
        /*TX - OAMP */
        if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
            /*get entry of mep db*/

            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

            soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER) {
                /* Fill the OAMP server fields.*/
                if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) {
                    /* Server - up mep.*/
                    uint32 system_port;
                    endpoint_info->tx_gport = classifier_mep_entry.remote_gport;
                    rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(unit, mep_db_entry.local_port, &system_port);
                    BCMDNX_IF_ERR_EXIT(rv);
                    BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->remote_gport, system_port);
                    endpoint_info->flags |= BCM_OAM_ENDPOINT_UP_FACING;
                } else {
                    /*Server: down MEP*/
                    endpoint_info->remote_gport = classifier_mep_entry.remote_gport;
                }
            }

            if (_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(classifier_mep_entry.mep_type))
            {
                endpoint_info->session_id = mep_db_entry.session_identifier_id;
            }
            rv = _bcm_oam_mep_db_entry_struct_get(unit, endpoint_info, &mep_db_entry, classifier_mep_entry.dip_profile_ndx);
            BCMDNX_IF_ERR_EXIT(rv);

            endpoint_info->src_mac_address[5] = classifier_mep_entry.src_mac_lsb;
            
            /*get correct 1711 type*/
            if((classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) || (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)){
                endpoint_info->type = bcmOAMEndpointTypeMPLSNetwork;
                endpoint_info->mpls_network_info.function_type = (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ? SOC_PPC_OAM_Y1711_LM_MPLS : SOC_PPC_OAM_Y1711_LM_PWE;
            }

            if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWNMEP_TX_GPORT_IS_LAG) {
                /* the tx gport is a TRUNK. override what was written in entry_struct_get()*/
                BCM_GPORT_TRUNK_SET(endpoint_info->tx_gport, (0x3FFF & endpoint_info->tx_gport));
            }
            if (SOC_IS_ARADPLUS(unit) && !_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(classifier_mep_entry.mep_type)) {
                /* get the 2 RDI flags from the MEP professor.*/
                SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  mep_prof;
                uint8 rdi_from_rx, rdi_from_scannner;
                int dont_care;
                rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, endpoint_info->id, &dont_care, &mep_prof);
                BCMDNX_IF_ERR_EXIT(rv);

                _BCM_OAM_GET_RDI_STATUS_FROM_ETH1731_MEP_PROF_RDI_FIELD(mep_prof.rdi_gen_method, rdi_from_rx, rdi_from_scannner);
                endpoint_info->flags2 |= rdi_from_rx ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
                endpoint_info->flags2 |= rdi_from_scannner ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE;
            }
            if (SOC_IS_QAX(unit) && 
               (_BCM_OAM_DISSECT_IS_CLASSIFIER_DOWN_MEP_INJECTION(&classifier_mep_entry))) {
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_get_mep_data_in_gen_mem, (unit, endpoint_info->id, &dummy, &read_vpn));
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_info->vpn = read_vpn;
            }
        }
    }

    BCM_EXIT;
exit:
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport))
    {
        _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit, endpoint_info->remote_gport, rv);
    }
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_destroy (int unit, bcm_oam_endpoint_t endpoint)
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv;
    _bcm_oam_ma_name_t ma_name_struct;
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR rmep_iter_p = 0;
    ENDPOINT_LIST_MEMBER_PTR member_next;
    bcm_oam_endpoint_t current_endpoint;
    uint8 found;
    uint8 deallocate_icc_ndx = FALSE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data_acc;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int is_last, prof_ndx;
    uint32 rmep_id_internal;
    int endpoint_ref_counter;
    uint8 oam_is_init;
    uint8 is_passive;
    uint8 is_default;
    uint8 is_mip = 0;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    uint8 is_mip_dummy;
    int profile, old_profile;
    uint32 profile_u32, profile_dummy;
    uint32 no_of_slm_endpoints = 0, no_of_dlm_endpoints = 0;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint);

    if ((endpoint < 0) && !is_default) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Endpoint id must be positive or default. Given id: %d\n"), endpoint));
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&mep_info);

    /* Default OAM trap */
    if (is_default) {
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }

            if (mep_info.non_acc_profile != SOC_PPC_OAM_PROFILE_DEFAULT) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Error: Internal error in default profile configuration.\n")));
            }
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, mep_info.non_acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);

            SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);

            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, &profile_data, &mep_info, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, -1);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else if (SOC_IS_ARADPLUS(unit)) {

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }

            /* Free profile */
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, 0 /*is_passive*/, &profile_data);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, mep_info.non_acc_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_oam_default_id_from_ep_id(unit, endpoint, &default_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_remove,
                                                                                    (unit, default_id));
            BCMDNX_IF_ERR_EXIT(rv);

            /* remove from mep info database */
            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else { /* Destroy non-default endpoint */
        if (_BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)) {
            /* remote endpoint */
            SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;

            /*update sw db of mep ->rmep: delete rmep; delete entry in rmep->mep db */
            SOC_PPC_OAM_RMEP_INFO_DATA_clear(&rmep_info);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, endpoint, &rmep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Error: Remote endpoint %d not found.\n"), endpoint));
            }

            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, rmep_info.mep_index, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint);

            rv = _bcm_dpp_oam_bfd_rmep_db_entry_dealloc(unit, rmep_id_internal);
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = soc_ppd_oam_oamp_rmep_delete(unit, rmep_id_internal, rmep_info.rmep_id, rmep_info.mep_index, mep_info.mep_type);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(unit, rmep_info.mep_index, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_oam_bfd_rmep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = bcm_dpp_am_oam_rmep_id_dealloc(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "OAM: Remote endpoint with id %d deleted\n"),
                                 endpoint));
        }

        /* local endpoint */
        else {
            rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!found) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
            }

            if (!((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ||
                (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))) {
                /*update sw db of ma->mep : delete mep */
                rv = _bcm_dpp_oam_ma_to_mep_db_mep_delete(unit, mep_info.ma_index, endpoint);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /*TX - OAMP clear */

            /*delete entry in mep db*/
            if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&mep_info)) {
                if (SOC_IS_ARADPLUS(unit)) {
                    bcm_oam_loss_t loss_obj;
                    bcm_oam_delay_t delay_obj;
                    int was_removed = 0;

                    LOG_DEBUG(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM: Attempting to delete loss/delay entries for endpoint %d\n"), endpoint));

                    bcm_oam_delay_t_init(&delay_obj);
                    bcm_oam_loss_t_init(&loss_obj);
                    loss_obj.id = endpoint;
                    rv = bcm_petra_oam_loss_delete(unit, &loss_obj);
                    if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }
                    delay_obj.id = endpoint;
                    rv = bcm_petra_oam_delay_delete(unit, &delay_obj);
                    if (rv != BCM_E_NOT_FOUND) {
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }
                    if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) {
                        bcm_oam_loopback_t loopback = { 0 };
                        loopback.id = endpoint;
                        rv = bcm_petra_oam_loopback_delete(unit, &loopback);
                        BCMDNX_IF_ERR_EXIT(rv);
                        was_removed = 1;
                    }

                    if (was_removed) {
                        /* The mep_info may have been changed.*/
                        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint, &mep_info, &found);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (!found) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), endpoint));
                        }
                    }

                    if (SOC_IS_JERICHO(unit)) {
                        bcm_oam_ais_t ais;
                        int dont_care[1];
                        uint8 found;
                        ais.id = endpoint;
                        rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, endpoint, dont_care, &found);
                        BCMDNX_IF_ERR_EXIT(rv);
                        if (found) {
                            rv = bcm_oam_ais_delete(unit, &ais);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }

                        if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER) {
                            bcm_oam_loopback_t loopback = { 0 };
                            loopback.id = endpoint;
                            rv = bcm_petra_oam_loopback_delete(unit, &loopback);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER) {
                            bcm_oam_tst_tx_t tst_tx = { 0 };
                            tst_tx.endpoint_id = endpoint;
                            rv = bcm_petra_oam_tst_tx_delete(unit, &tst_tx);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                        if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER) {
                            bcm_oam_tst_rx_t tst_rx = { 0 };
                            tst_rx.endpoint_id = endpoint;
                            rv = bcm_petra_oam_tst_rx_delete(unit, &tst_rx);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }

                    /*free the mep profile entry.*/
                    rv =  _bcm_dpp_am_template_oam_eth1731_mep_profile_free(unit, endpoint,  &prof_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, prof_ndx, endpoint);


                }
                /*update sw db of mep->rmep : delete all rmeps */
                rv = _bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint, &rmep_list_p, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found)
                {
                    /* this mep has rmeps */
                    /*Loop on all RMEPs associated with each MEP*/
                    rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &rmep_iter_p) ;
                    BCMDNX_IF_ERR_EXIT(rv);

                    while (rmep_iter_p != 0)
                    {
                        uint32 member_index ;

                        rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, rmep_iter_p, &member_index) ;
                        BCMDNX_IF_ERR_EXIT(rv);
                        current_endpoint = (bcm_oam_endpoint_t)member_index ;
                        rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, rmep_iter_p, &member_next) ;
                        BCMDNX_IF_ERR_EXIT(rv);
                        rmep_iter_p = member_next ;
                        rv = bcm_petra_oam_endpoint_destroy(unit, current_endpoint);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                rv = _bcm_dpp_oam_ma_db_get(unit, mep_info.ma_index, &ma_name_struct, &found);
                BCMDNX_IF_ERR_EXIT(rv);

                /*get entry of mep db*/
                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint, &mep_db_entry);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (ma_name_struct.name_type != _BCM_OAM_MA_NAME_TYPE_SHORT && ma_name_struct.name_type != _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE ) {

                    /*Deallocate the pointer to ICCMap register*/

                    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */
                    /* coverity[uninit_use_in_call:FALSE] */
                    rv = _bcm_dpp_am_template_oam_icc_map_ndx_free(unit, mep_db_entry.icc_ndx, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_last) {
                        deallocate_icc_ndx = TRUE;
                    }
                }

                rv = _bcm_oam_mep_db_entry_dealloc(unit,endpoint,&mep_db_entry,&mep_info);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = OAM_ACCESS.acc_ref_counter.get(unit, (uint32 *)&endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_ref_counter--;
                rv =  OAM_ACCESS.acc_ref_counter.set(unit, (uint32)endpoint_ref_counter);
                BCMDNX_IF_ERR_EXIT(rv);


                if (BCM_GPORT_INVALID != mep_info.remote_gport && (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0) {
                    /* Server, down MEP. Free the trap.*/
                    uint32 server_trap_ref_counter;
                    rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.get(unit, mep_info.mep_type, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport), &server_trap_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);
                    server_trap_ref_counter--;
                    rv = OAM_ACCESS.oamp_rx_trap_code_ref_count.set(unit, mep_info.mep_type, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport), server_trap_ref_counter);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (!server_trap_ref_counter) {
                        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, mep_info.mep_type, BCM_GPORT_TRAP_GET_ID(mep_info.remote_gport)));
                        BCMDNX_IF_ERR_EXIT(rv); 
                    }
                }

                soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, endpoint, &mep_db_entry, deallocate_icc_ndx, (endpoint_ref_counter == 0));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, endpoint);
                BCMDNX_IF_ERR_EXIT(rv);

                if (SOC_IS_QAX(unit)) {
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_set_mep_data_in_gen_mem, (unit, endpoint, 0, 0));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
            
            /*RX - classifier*/

            /* Free profiles - Only if RX is set*/
            if ((mep_info.lif != _BCM_OAM_INVALID_LIF) ||
                ((mep_info.lif == _BCM_OAM_INVALID_LIF) &&
                 ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                  (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                  (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                  (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                  (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                  (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
                /* accelerated profile */
                if ((SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)
                      && !_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&mep_info))
                    || _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&mep_info)
                    || _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint)) {
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Freeing accelerated profile for MEP %d\n"),endpoint));
                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint, &profile_data_acc);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_free(unit, mep_info.acc_profile, &is_last);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* for accelerated end point remove the entry from the Lem used for statistics*/
                    if (SOC_IS_ARADPLUS_A0(unit) && SOC_DPP_CONFIG(unit)->pp.oam_statistics && _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)) {
                         SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
                         SOC_SAND_SUCCESS_FAILURE success;
                         SOC_PPC_FP_QUAL_VAL_clear(qual_vals);

                         qual_vals[0].val.arr[0] = mep_info.lif;
                         qual_vals[0].val.arr[1] = 0;
                         qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
                         qual_vals[1].val.arr[0] = SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM;
                         qual_vals[1].val.arr[1] = 0;
                         qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_OPCODE;
                         qual_vals[2].val.arr[0] = ((mep_info.md_level)*2);
                         qual_vals[2].val.arr[1] = 0;
                         qual_vals[2].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;

                         rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, qual_vals , &success);
                         BCMDNX_IF_ERR_EXIT(rv);
                     }
                }

                is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&mep_info);
                /* non-accelerated profile */
                for (is_passive = 0; is_passive <= 1; is_passive++) {
                   /* In Arad Mode Passive profile is fixed and configured separately */
                   /* In Jericho for MIPs is_passive represents egress */
                   if (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && is_passive && !(SOC_IS_JERICHO(unit) && is_mip)) {
                       continue;
                   }
                   /* Skip passive profile if MPLS Ingress only and classifier is not in advanced mode */
                   if (is_passive && (mep_info.passive_side_lif == _BCM_OAM_INVALID_LIF)) {
                       continue;
                   }
                   /* Skip passive profile for MIP if set to SLM_PASSIVE_PROFILE */
                   if (SOC_IS_JERICHO_PLUS(unit) && is_passive && is_mip && (mep_info.non_acc_profile_passive == ARAD_PP_SLM_PASSIVE_PROFILE)) {
                       continue;
                   }

                   /* Skip passive profile for RFC-6374 endpoints since it is not allocated */
                   if ((is_passive) && ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
                      (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ||
                      (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))) {
                      continue;
                   }

                   /* MPLS Egress only OEM entries */
                   if (!((mep_info.lif == _BCM_OAM_INVALID_LIF) &&
                         ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                          (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                          (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                          (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                          (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                          (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
                       if (SOC_IS_JERICHO_PLUS(unit)) {
                           rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(unit, endpoint, is_passive, &old_profile);
                           BCMDNX_IF_ERR_EXIT(rv);
                           if (old_profile != ARAD_PP_SLM_PASSIVE_PROFILE) {
                               rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, is_passive, &profile_data);
                               BCMDNX_IF_ERR_EXIT(rv);

                               rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, (is_passive ? mep_info.non_acc_profile_passive : mep_info.non_acc_profile), &is_last);
                               BCMDNX_IF_ERR_EXIT(rv);
                           }
                       } else {
                           rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, endpoint, is_passive, &profile_data);
                           BCMDNX_IF_ERR_EXIT(rv);

                           rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(unit, (is_passive ? mep_info.non_acc_profile_passive : mep_info.non_acc_profile), &is_last);
                           BCMDNX_IF_ERR_EXIT(rv);
                       }
                   }
                }

                soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, endpoint, &mep_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* Find if there are other meps on same lif */
                if (!((mep_info.lif == _BCM_OAM_INVALID_LIF) &&
                      ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                       (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                       (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                       (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
                       (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
                       (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
                    rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                 mep_info.lif,
                                                                 _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info),
                                                                 0/*find_mip*/,
                                                                 &found,
                                                                 &is_mip_dummy,
                                                                 &profile_dummy);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (!found) { /* Serch meps on the other directions */
                        /*If  MPLS Ingress only, don't need to do this */
                        if(mep_info.passive_side_lif != _BCM_OAM_INVALID_LIF) {
                            rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                         mep_info.passive_side_lif,
                                                                         (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)),
                                                                         0/*find_mip*/,
                                                                         &found,
                                                                         &is_mip_dummy,
                                                                         &profile_dummy);
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }

                    if (!found) { /* No more meps on this lif */
                        bcm_oam_endpoint_type_t mep_type;
                        /* Ingress */
                        rv = _bcm_dpp_oam_lif_table_set(unit, _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info) ? mep_info.passive_side_lif : mep_info.lif, 0);
                        BCMDNX_IF_ERR_EXIT(rv);
                        /* Egress */
                        mep_type = (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) ? bcmOAMEndpointTypeEthernet :
                            ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)) ? bcmOAMEndpointTypeBHHMPLS :
                            ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE))? bcmOAMEndpointTypeBHHPwe :
                            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ? bcmOAMEndpointTypeBhhSection :
                            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ? bcmOAMEndpointTypeBHHPweGAL :
                            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ? bcmOAMEndpointTypeMplsLmDmPw :
                            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) ? bcmOAMEndpointTypeMplsLmDmLsp :
                            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION) ? bcmOAMEndpointTypeMplsLmDmSection : -1;
                        if((SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit)) && (SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE == mep_info.mep_type)
                            &&(mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP)){
                               mep_type = bcmOAMEndpointTypeBHHMPLS;
                        }
                        rv = _bcm_dpp_oam_outlif_table_set(unit, _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info) ? mep_info.lif
                                : mep_info.passive_side_lif, mep_type, 0);
                        BCMDNX_IF_ERR_EXIT(rv);

                        if (SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) && 
                            (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info) || is_mip)) {
                            int local_out_lif = 0;
                            int global_out_lif = 0;

                            /* Removed mapping of local outlif to global outlif in LEM, when destroying last upmep/mip on the lif.*/           
                            local_out_lif = (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)) ? mep_info.lif : mep_info.passive_side_lif;

                            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, local_out_lif, &global_out_lif);
                            BCMDNX_IF_ERR_EXIT(rv);

                            rv = _bcm_dpp_oam_outlif_l2g_lem_entry_add(unit, local_out_lif, global_out_lif, 0/*delete*/);
                            BCMDNX_IF_ERR_EXIT(rv);

                            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                        (BSL_META_U(unit,
                                                    "OAM: removing mapping of local outlif(%d) to global outlif(%d \n"),
                                                    local_out_lif, global_out_lif));
                        }


                    }
                    else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && !SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
                        /* If this is the last MIP on this Lif, change profile to active_passive and add passive endpoint to passive bitmap */
                        if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&mep_info)) {
                            /* Check if no more MIPs left on lif, direction (up/down) where looking for the MIP doesnt make difference */
                            rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                         mep_info.lif,
                                                                         0/*is_upmep*/,
                                                                         1/*find_mip*/,
                                                                         &found,
                                                                         &is_mip_dummy,
                                                                         &profile_u32);
                            BCMDNX_IF_ERR_EXIT(rv);

                            if (!found) {
                                bcm_oam_endpoint_traverse_cb_profile_info_t profile_info;
                                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry_dummy;
                                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry_dummy);

                                /* If no more MIPs, we must have at least one MEP on any of the sides. */
                                /* If we have more then one they must be on the same side. */
                                /* Need to find which is the active side. */
                                rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                             mep_info.lif,
                                                                             0/*is_upmep*/,
                                                                             0/*find_mip*/,
                                                                             &found,
                                                                             &is_mip_dummy,
                                                                             &profile_u32);
                                BCMDNX_IF_ERR_EXIT(rv);

                                if (!found) {
                                    /* Remaining MEPs on the LIF are up directed. */
                                    /* Finding the active profile */
                                    rv = _bcm_oam_mep_exist_on_lif_and_direction(unit,
                                                                                 mep_info.lif,
                                                                                 1/*is_upmep*/,
                                                                                 0/*find_mip*/,
                                                                                 &found,
                                                                                 &is_mip_dummy,
                                                                                 &profile_u32);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                }

                                /* Existing endpoints must return to active_passive mp_type */
                                for (is_passive = 0; is_passive<=1; is_passive++) {
                                    if (!is_passive) {
                                        /* If active - fill active side on which MEP/s already exist, take profile from existing endpoint */
                                        profile = (int)profile_u32;
                                        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(unit, profile, &profile_data);
                                        BCMDNX_IF_ERR_EXIT(rv);
                                        profile_data.mp_type_passive_active_mix = 1;

                                        /* Find first endpoint on this Lif */
                                        profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                                        profile_info.lif = mep_info.lif;
                                        profile_info.profile_data = NULL;
                                        profile_info.cb_rv = BCM_E_NONE;
                                        /* Iterate over all groups to find all the endpoints on same lif to find the first */
                                        rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                                        BCMDNX_IF_ERR_EXIT(rv);

                                        if (profile_info.cb_rv != BCM_E_NONE) {
                                            /* Function may fail inside the traverse API but not return an error. Handle errors through this field.*/
                                            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: _bcm_oam_endpoint_exchange_profile_cb returned failure.\n")));
                                        }

                                        /* get the existing endpoint */
                                        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, profile_info.first_endpoint_id, &classifier_mep_entry_dummy, &found);
                                        BCMDNX_IF_ERR_EXIT(rv);
                                    }
                                    else {
                                         /* Skip if Ingress_only  */
                                         if(mep_info.passive_side_lif != _BCM_OAM_INVALID_LIF) {
                                            /* Passive side - Get Default profile for passive side. mp_type_passive_active_mix will be set to 1 */
                                            rv = _bcm_oam_default_profile_get(unit, &profile_data, &classifier_mep_entry_dummy, 0 /* non accelerated */, 0 /* is default */,
                                                                             (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry_dummy)));
                                            BCMDNX_IF_ERR_EXIT(rv);
                                         }
                                    }
                                    /* Skip for passive side if Ingress_only  */
                                    if(is_passive && (mep_info.passive_side_lif == _BCM_OAM_INVALID_LIF)) {
                                        continue;
                                    }
                                    /* Move all existing endpoints on this lif to the new active profile (only SW DB) */
                                    profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
                                    profile_info.lif = mep_info.lif;
                                    profile_info.profile_data = &profile_data;
                                    profile_info.is_ingress = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry_dummy)) ^ is_passive;
                                    profile_info.cb_rv = BCM_E_NONE;
                                    /* Iterate over all groups to find all the endpoints on same lif and switch their profile */
                                    rv = bcm_petra_oam_group_traverse(unit, _bcm_oam_endpoint_exchange_profile_group_cb, &profile_info);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                    if (profile_info.cb_rv != BCM_E_NONE) {
                                        /* Function may fail inside the traverse API but not return an error. Handle errors through this field.*/
                                        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: _bcm_oam_endpoint_exchange_profile_cb returned failure.\n")));
                                    }
                                    /* get the existing endpoint on active side */
                                    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, profile_info.first_endpoint_id, &classifier_mep_entry_dummy, &found);
                                    BCMDNX_IF_ERR_EXIT(rv);

                                    if (!found) {
                                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                                         (_BSL_BCM_MSG("Error: Internal error, endpoint not found. \n")));
                                    }

                                    /* Update HW */
                                    soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit,
                                                                                            is_passive ? classifier_mep_entry_dummy.non_acc_profile_passive : classifier_mep_entry_dummy.non_acc_profile,
                                                                                            1/*enable active_passive_mix*/);
                                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                                    if(!is_passive) {
                                        if(profile_info.is_alloced) {
                                            soc_sand_rv = arad_pp_oam_classifier_entries_insert_active_non_accelerated( unit, 0, &classifier_mep_entry_dummy,
                                                                                                                                 &profile_data,
                                                                                                                                 &profile_data_acc);
                                            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                        }
                                    } else {
                                        if(profile_info.is_alloced) {
                                               soc_sand_rv = arad_pp_oam_classifier_entries_insert_passive_non_accelerated( unit, &classifier_mep_entry_dummy, &profile_data);
                                               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                        }
                                    }
                                }

                                /* Update HW - OEM1 table*/
                                soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry_dummy, 1 /*update mp-type*/);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                            }
                        }
                    }
                }
            }

            /* remove from mep info database */
            rv = _bcm_dpp_oam_bfd_mep_info_db_delete(unit, endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            if (mep_info.is_rfc_6374_entry) {
              if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM) {
                /* Inferred LM */
                rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
                BCMDNX_IF_ERR_EXIT(rv);
                no_of_slm_endpoints--;
                rv = OAM_ACCESS.no_of_slm_endpoints.set(unit, no_of_slm_endpoints);
                BCMDNX_IF_ERR_EXIT(rv);
              } else {
                /* Direct LM */
                rv = OAM_ACCESS.no_of_dlm_endpoints.get(unit, &no_of_dlm_endpoints);
                BCMDNX_IF_ERR_EXIT(rv);
                no_of_dlm_endpoints--;
                rv = OAM_ACCESS.no_of_dlm_endpoints.set(unit, no_of_dlm_endpoints);
                BCMDNX_IF_ERR_EXIT(rv);
              }
            }

            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "Log: Local endpoint with id %d deleted\n"), endpoint));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Assigns slm passive profile to a given classifier entry
   and disables data packet counting */
int bcm_oam_assign_slm_passive_profile(int unit, uint32 mep_id){
   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
   uint8 found;
   int   rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Replace existing profile in endpoint SW DB */
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, mep_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: assign_slm_passive_profile error.\n")));
    }
  
    classifier_mep_entry.non_acc_profile_passive = ARAD_PP_SLM_PASSIVE_PROFILE;

    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, mep_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

/* Set OAM endpoint opcode actions */
int bcm_petra_oam_endpoint_action_set_internal(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t *action,
    int is_piggy_backed,
    int is_sat_loopback)
{

    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data_pointer;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data_arr_pointer = NULL;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data = NULL, *old_profile_data = NULL;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data_acc = NULL, *old_profile_data_acc = NULL;
    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data_passive = NULL;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 acc_profile_alloc=0, non_acc_profile_alloc = 0;
    int opcode, profile;
    uint8 internal_opcode;
    int is_allocated, old_profile, old_profile_acc, is_last;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint32 trap_code=0, trap_code2=0, no_of_slm_endpoints = 0;
    uint8 trap_strength = 0, snoop_strength = 0, snoop_strength2 = 0;
    uint8 profile_change = 0;
    uint8 found, is_accelerated, is_mip, is_passive, is_default;
    ARAD_PP_OAM_DEFAULT_EP_ID default_id;
    bcm_oam_endpoint_traverse_cb_profile_info_t profile_info;
    bcm_oam_endpoint_action_t action_lcl_t;
    bcm_oam_endpoint_action_t *action_lcl;
    uint8 passive_profile_force_change = 0;
    uint8 is_slm = 0, is_slm_old = 0;
    int opcode_count = 0;
    uint8 is_rfc6374_lm = 0, is_rfc6374_dm = 0;
    BCMDNX_INIT_FUNC_DEFS;

    action_lcl = &action_lcl_t; /* If the GPORT contains a trap, since the trap id inside the GPORT can be either the HW id or the SW_id ( according to the use_hw_id SOC peroperty) , and this function 
    assumes that the trap id is the SW id, it will use a copy of the action with the SW id */
    sal_memcpy(action_lcl, action, sizeof(bcm_oam_endpoint_action_t)); /* from here till the end of the function, the function uses the copy of the action */

    /* if the use_hw_id soc property is enabled, then the trap_id is the HW id and thus we need to convert it to the sw id becuase the oam logic uses the SW id*/
    _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, action_lcl->destination);
    _BCM_RX_CONVERT_HW_GPORT_TO_SW_GPORT(unit, action_lcl->destination2);

    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    is_default = _BCM_OAM_IS_MEP_ID_DEFAULT(unit, id);
    if (is_default) {
        rv = _bcm_oam_default_id_from_ep_id(unit, id, &default_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    
    is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(&classifier_mep_entry);
    opcode_count = (classifier_mep_entry.is_rfc_6374_entry ? (bcmOamMplsLmDmOpcodeTypeIlm + 1) : SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT);

    if (classifier_mep_entry.is_rfc_6374_entry) {
        is_rfc6374_lm = (BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeLm) || (BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeIlm)));
        is_rfc6374_dm = BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeDm);
    }

    /* All opcodes include meta data.*/

    is_accelerated = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)
                     || _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id); /* Classifier entries in the latter case act as accelerated entries.*/
    BCMDNX_ALLOC(profile_data_arr_pointer, sizeof(SOC_PPC_OAM_LIF_PROFILE_DATA) * 5,
        "bcm_petra_oam_endpoint_action_set_internal.profile_data_arr_pointer");
    if (profile_data_arr_pointer == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    profile_data = profile_data_arr_pointer;
    old_profile_data = profile_data_arr_pointer + 1;
    profile_data_acc = profile_data_arr_pointer + 2;
    old_profile_data_acc = profile_data_arr_pointer + 3;
    profile_data_passive = profile_data_arr_pointer + 4;
    /* Get existing profile */
    if (!((classifier_mep_entry.lif == _BCM_OAM_INVALID_LIF) &&
          ((classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
           (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
           (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
           (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||
           (classifier_mep_entry.mep_type ==SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
           (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)))) {
        SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data);
        rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(unit, id, 0 /*is_passive*/, &old_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_JERICHO_PLUS(unit)) {
            if (old_profile != ARAD_PP_SLM_PASSIVE_PROFILE) {
                rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 0/*is_passive*/, profile_data);
                BCMDNX_IF_ERR_EXIT(rv);
                SOC_PPC_OAM_LIF_PROFILE_DATA_clear(old_profile_data);
                rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 0/*is_passive*/, old_profile_data);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 0/*is_passive*/, profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
            SOC_PPC_OAM_LIF_PROFILE_DATA_clear(old_profile_data);
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 0/*is_passive*/, old_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (is_mip) {
        /* In case of MIP, both up and down profiles should be updated.
           In case of MEP, only the active profile should be updated. */
        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)) {
            /* For ARAD mode, we cannot access and get the profile data in PASSIVE profile 
             * So, we copy the active data into passive data. 
             */
            *profile_data_passive = *profile_data;
        } else {
            if (!(SOC_IS_JERICHO_PLUS(unit) && (classifier_mep_entry.non_acc_profile_passive == ARAD_PP_SLM_PASSIVE_PROFILE))) {
                SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data_passive);
                rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(unit, id, 1/*is_passive*/, profile_data_passive);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    SOC_PPC_OAM_LIF_PROFILE_DATA_clear(profile_data_acc);
    /* New Arad+ Classifier: in the new implementation this should always be done because OEM2 has entry for non-accelerated endpoints as well */
    if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
        rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, profile_data_acc);
        BCMDNX_IF_ERR_EXIT(rv);
        SOC_PPC_OAM_LIF_PROFILE_DATA_clear(old_profile_data_acc);
        rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, id, old_profile_data_acc);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Gets the trap codes from the destination gports, Sets mirror profile if required, Handles advanced egress snooping,
       Sets OAMP RX trap if required */
    rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, is_rfc6374_lm, is_rfc6374_dm,
                                                               action_lcl->destination, action_lcl->destination2, &trap_code, &trap_code2);
    BCMDNX_IF_ERR_EXIT(rv);

    if (action_lcl->destination != BCM_GPORT_INVALID) {
        trap_strength = BCM_GPORT_TRAP_GET_STRENGTH(action_lcl->destination);
        snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_lcl->destination);
    }
    if (action_lcl->destination2 != BCM_GPORT_INVALID) {
        snoop_strength2 = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(action_lcl->destination2);
    }

    /* Fill new profile */
    for (opcode = 0; opcode < opcode_count; opcode++) {
        if (BCM_OAM_OPCODE_GET(*action_lcl, opcode)) {
            if (classifier_mep_entry.is_rfc_6374_entry) {
                /* Map RFC-6374 opcode to internal */
                _BCM_OAM_MAP_RFC6374_OPCODE_TO_INTERNAL(opcode, internal_opcode);
            } else {
                /* Translate to internal opcode */
                rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit, "\n------------------------------ trap code is %d, internal trap code: %d, opcode: %d\n"),
                       trap_code, internal_opcode, opcode));

            if (!is_mip) {
                if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
                     rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data_acc->mep_profile_data), internal_opcode, action_lcl, trap_code, trap_strength, snoop_strength);
                     BCMDNX_IF_ERR_EXIT(rv);
                } else { /* Modify only to non-acc profile */
                     rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data->mep_profile_data), internal_opcode, action_lcl, trap_code, trap_strength, snoop_strength);
                     BCMDNX_IF_ERR_EXIT(rv);
                }
            } else { /*MIP*/
                /* In case of MIP, modify both directions */
                 rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data->mip_profile_data), internal_opcode, action_lcl, trap_code, trap_strength, snoop_strength);
                 BCMDNX_IF_ERR_EXIT(rv);
                 rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data_passive->mip_profile_data), internal_opcode, action_lcl, trap_code2, _ARAD_PP_OAM_TRAP_STRENGTH, snoop_strength2);
                 BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    if (!is_mip) {
        /* SLM support - set profile */
        is_slm = BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) ? 1 : 0;
        profile_data->is_slm = (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) || old_profile_data->is_slm) ? 1 : 0;
        if (is_accelerated) {
            profile_data_acc->is_slm =  (BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionSLMEnable) || old_profile_data_acc->is_slm) ? 1 : 0;
            is_slm_old = old_profile_data_acc->is_slm;
        } else {
            is_slm_old = old_profile_data->is_slm;
        }
    } else {
        /* Set the is_slm variables to old values in case of MIP */
        profile_data->is_slm = is_slm = is_slm_old = old_profile_data->is_slm;
    }

    if (is_slm){
        /* Increment the number of slm endpoints if we did not have
         * slm action previously enabled for this endpoint.
         */
        if (!is_slm_old) {
            rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
            BCMDNX_IF_ERR_EXIT(rv);
            no_of_slm_endpoints++;
            rv = OAM_ACCESS.no_of_slm_endpoints.set(unit, no_of_slm_endpoints);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (is_slm_old) {
        rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
        BCMDNX_IF_ERR_EXIT(rv);
        no_of_slm_endpoints--;
        rv = OAM_ACCESS.no_of_slm_endpoints.set(unit, no_of_slm_endpoints);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) && is_piggy_backed != 2) {
        if (is_accelerated) {
            profile_data_acc->is_piggybacked = is_piggy_backed;
        }
        profile_data->is_piggybacked = is_piggy_backed;
    }

    if (((BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR)) ||
       (BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_TST)))
         && (is_sat_loopback != 2)) {
        if (is_accelerated) {
            profile_data_acc->flags |= is_sat_loopback ? SOC_PPC_OAM_LIF_PROFILE_FLAG_SAT_LOOPBACK : 0;
        } else {
            profile_data->flags |= is_sat_loopback ? SOC_PPC_OAM_LIF_PROFILE_FLAG_SAT_LOOPBACK : 0;
        }
    }

    /* For MIP - update passive profile as well */
    for (is_passive=0; is_passive<=is_mip; is_passive++) {

        profile_data_pointer = is_passive ? profile_data_passive : profile_data;
        /* get the old profile */
        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive) {
            /* In ARAD mode, passive profile is fixed . */
            old_profile = _BCM_OAM_PASSIVE_PROFILE; 
        } else {
            old_profile = is_passive ? classifier_mep_entry.non_acc_profile_passive : classifier_mep_entry.non_acc_profile;
        }

        if (classifier_mep_entry.is_rfc_6374_entry) {
            /* Update non-accelerated profile */
            rv = _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(unit, 0 /* flags */, id, 0 /* is_passive */, profile_data,
                                                                               &old_profile, &is_last, &profile, &is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);

            non_acc_profile_alloc = is_allocated;
            profile_info.is_last = is_last;

            if (classifier_mep_entry.non_acc_profile != profile) {
                classifier_mep_entry.non_acc_profile = profile;

                rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, id, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            /* Move all existing endpoints on this lif to the new profile */
            profile_info.first_endpoint_id = BCM_OAM_ENDPOINT_INVALID;
            profile_info.lif = is_passive? classifier_mep_entry.passive_side_lif: classifier_mep_entry.lif;
            profile_info.profile_data = profile_data_pointer;
            profile_info.is_ingress = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) ^ is_passive;
            if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive) {
                /* Lets hard code it to 1 so that it is properly written into HW */
                profile_info.is_alloced = 1;
            } else {
                profile_info.is_alloced = profile_info.is_last = 0;
            }
            profile_info.is_passive_profile_allocated  = 0;
            profile_info.cb_rv = BCM_E_NONE;
            /* For ARAD passive mode, lets not call the profile exchange since
             * the ARAD passive profile is not managed by the alloc_mngr APIs.
             */
            if (!(SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive)) {
                /* Iterate over lif and direction to find all endpoints and switch their profile */
                rv = bcm_petra_oam_endpoint_on_lif_and_direction(unit, classifier_mep_entry.lif,
                        classifier_mep_entry.passive_side_lif,
                        (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry) ^ is_passive),
                        is_passive,
                        is_mip,
                        _bcm_oam_endpoint_exchange_profile_cb,
                        &profile_info);
                if (rv != 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback in bcm_petra_oam_endpoint_on_lif_and_direction returned error message %s"),bcm_errmsg(rv)));
                }
            }

          /* in slm, if there was no previous passive profile then a SLM_PASSIVE_PROFILE should be assigned.
                     if there was previous passive profile, opcode_set should be called for passive side to
                     clr counter_disable */
          if ((SOC_IS_JERICHO_PLUS(unit)) && (profile_info.profile_data->is_slm)){
             if (profile_info.is_passive_profile_allocated) {
                /* passive profile changed to slm */
                rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM, &internal_opcode);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data_passive->mep_profile_data), internal_opcode, action_lcl, trap_code, trap_strength, snoop_strength);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR, &internal_opcode);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_oam_profile_single_opcode_set(unit, &(profile_data_passive->mep_profile_data), internal_opcode, action_lcl, trap_code, trap_strength, snoop_strength);
                BCMDNX_IF_ERR_EXIT(rv);
             } else {
                rv = bcm_oam_assign_slm_passive_profile(unit, id);
                BCMDNX_IF_ERR_EXIT(rv);
                profile_change = 1;
             }
          }
        }

        /* get the new profile through reading it from the SW. It might have been changed inside the callback */
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, id, &classifier_mep_entry/*_dummy*/, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Internal Error: Mep id %d does not exist.\n"), id));
        }

        if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) && is_passive) {
            /* In ARAD mode, the passive profile is fixed. */
            profile = _BCM_OAM_PASSIVE_PROFILE; 
        } else {
            profile = is_passive ? classifier_mep_entry/*_dummy*/.non_acc_profile_passive : classifier_mep_entry/*_dummy*/.non_acc_profile;
        }

        /* SLM support - disable/enable data counting */
        if (SOC_IS_ARADPLUS(unit)) {
            /* SLM support - disable data counting  only for new action*/
            if ( is_slm || (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM)) {
                soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 1);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
                BCMDNX_IF_ERR_EXIT(rv);
                if ((SOC_IS_JERICHO_AND_BELOW(unit) && no_of_slm_endpoints == 0) || SOC_IS_JERICHO_PLUS(unit)) {
                    soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, profile, 0);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }

            /* in jer+ we dont want to count data packets */
            if (!SOC_IS_JERICHO_PLUS(unit)) {
                if (profile_data->is_slm || (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM)) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_slm_set,
                            (unit, profile_data->is_slm));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else {
                    /* If slm is disabled, do not reset the LM subtype if there are still
                     * other SLM endpoints in the system, since this is a system wide
                     * register setting.
                     */
                    rv = OAM_ACCESS.no_of_slm_endpoints.get(unit, &no_of_slm_endpoints);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (no_of_slm_endpoints == 0) {
                        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_slm_set,
                                (unit, profile_data->is_slm));
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
        }

        if((SOC_IS_QAX(unit))&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)
            &&((SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS == classifier_mep_entry.mep_type) || (SOC_PPC_OAM_MEP_TYPE_Y1711_PWE == classifier_mep_entry.mep_type))
            &&(BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) ||BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR) )){
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_1711_pe_profile_update, (unit,classifier_mep_entry.mep_type,id,BCM_OAM_ACTION_GET(*action_lcl, bcmOAMActionCountEnable)));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (!is_mip) {
           if (!SOC_IS_JERICHO_PLUS(unit)) {
              if ((BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) ||
                  BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR) ||
                  BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM) ||
                  BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR)) ||
                  (classifier_mep_entry.is_rfc_6374_entry &&
                   (BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeLm) ||
                    BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeIlm)))) {
                 /* configure whether SLM/LMM and related packets are counted*/
                 /* By default count none. If one is set to count then count all of them.*/
                 soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_counter_increment_bitmap_set,
                                                   (unit,BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable)));
                 BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
           } else {
              /* jericho+ && !is_mip :
                 set increment_bitmap for LMM only */
              if (((!profile_data->is_slm) &&
                  ((BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM)) ||
                  (BCM_OAM_OPCODE_GET(*action_lcl, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR)))) ||
                  (classifier_mep_entry.is_rfc_6374_entry &&
                   !(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM) &&
                   (BCM_OAM_OPCODE_GET(*action_lcl, bcmOamMplsLmDmOpcodeTypeLm)))) {
                        /* configure LMM */
                        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_counter_increment_bitmap_set,
                                                          (unit,BCM_OAM_ACTION_GET(*action, bcmOAMActionCountEnable)));
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
           }
        }

        if (old_profile != profile) {
            profile_change = 1;

            if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {
                /* Set HW passive_active for the new allocated profile */
                soc_sand_rv = soc_ppd_oam_mep_passive_active_enable_set(unit, profile, profile_data_pointer->mp_type_passive_active_mix);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            if (SOC_IS_JERICHO_PLUS(unit) && profile_info.is_last) {
                /* SLM support - enable data counting */
                if (SOC_IS_ARADPLUS(unit) && profile_change) {
                    soc_sand_rv = soc_ppd_oam_classifier_counter_disable_map_set(unit, 0/*packet_is_oam*/, old_profile, 0);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }

        /* Accelerated profile - relevant only for active side */
        /* New Arad+ Classifier: in the new implementation this should always be done */
        if (!is_passive) {
            if (_BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit,&classifier_mep_entry,id) && !is_default) {
            rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(unit, 0/*flags*/, id, profile_data_acc,
                                                         &old_profile_acc, &is_last, &profile, &is_allocated);
                BCMDNX_IF_ERR_EXIT(rv);

                acc_profile_alloc = is_allocated;

                if (classifier_mep_entry.acc_profile != profile) {
                    classifier_mep_entry.acc_profile = profile;
                    profile_change = 1;

                    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, id, &classifier_mep_entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        /* Insert into classifier OAM tables */
        if (is_default) {
            if (SOC_IS_ARADPLUS(unit) && (old_profile != classifier_mep_entry.non_acc_profile)) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_classifier_default_profile_add,
                                                  (unit, default_id,&classifier_mep_entry, 1/*update_action_only*/));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            soc_sand_rv = soc_ppd_oam_classifier_oam1_entries_insert_default_profile(unit, profile_data_pointer, &classifier_mep_entry, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            if(!is_passive) {
                if (classifier_mep_entry.is_rfc_6374_entry && non_acc_profile_alloc) {
                    soc_sand_rv = arad_pp_oam_classifier_rfc6374_entries_insert_active_non_accelerated(unit, &classifier_mep_entry, profile_data);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else if(profile_info.is_alloced) {
                    soc_sand_rv = arad_pp_oam_classifier_entries_insert_active_non_accelerated(unit, _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(id), &classifier_mep_entry,
                                                                                                                                                   profile_data_pointer,
                                                                                                                                                   profile_data_acc);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                } else if (old_profile != profile && is_mip) {
                    /* For MIP cases in JERICHO, active profile insertion does the passive profile insertion as well.
                     * It could so happen that active profile changes into something without being newly allocated and passive profile also
                     * could remain in such a way that it is not newly allocated. So if active profile changes to something, make sure you
                     * change the passive profile also.
                     */
                    passive_profile_force_change = 1;
                 }
                if(acc_profile_alloc) {
                    if (classifier_mep_entry.is_rfc_6374_entry) {
                        soc_sand_rv = arad_pp_oam_classifier_rfc6374_entries_insert_accelerated(unit, &classifier_mep_entry, profile_data_acc);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    } else {
                        soc_sand_rv = arad_pp_oam_classifier_entries_insert_accelerated(unit, &classifier_mep_entry, profile_data_acc);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            } else {
                if(profile_info.is_alloced || passive_profile_force_change) {
                    soc_sand_rv = arad_pp_oam_classifier_entries_insert_passive_non_accelerated(unit, &classifier_mep_entry, profile_data_pointer);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
    }

    /* Insert into classifier OEM tables */
    if (profile_change) {
        if (id != -1) {
            soc_sand_rv = soc_ppd_oam_classifier_oem_mep_profile_replace(unit, &classifier_mep_entry, 0 /*update mp type*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCM_EXIT;
exit:
    sal_memcpy(action, action_lcl, sizeof(bcm_oam_endpoint_action_t));
    /* if the use_hw_id soc property is enabled, then the trap_id is the SW id and thus we need to convert it to the HW id becuase the oam logic uses the SW id*/
    _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit,action->destination,rv);
    _BCM_RX_CONVERT_SW_GPORT_TO_HW_GPORT(unit,action->destination2,rv);
    BCM_FREE(profile_data_arr_pointer);
    profile_data = NULL;
    old_profile_data = NULL;
    profile_data_acc = NULL;
    old_profile_data_acc = NULL;
    profile_data_passive = NULL;

    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_action_set(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t *action) {

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* Validation */
    rv = _bcm_oam_endpoint_action_set_validity_check(unit,id,action);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_oam_endpoint_action_set_internal(unit,id,action,2/*ignore*/,2/*ignore*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN; 
}

/** Internal call-back that go over all of the created endpoints and if they are of type MplsLmDm deletes them */
int _bcm_oam_destroy_all_iterate_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t user_data) {
    bcm_oam_endpoint_info_t endpoint_info;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *mep_entry =  (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *)user_data;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_endpoint_info_t_init(&endpoint_info);

    /*get the endpoint_info*/
    if ( (*(int*)key != -2  ) && ((mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
        (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))) 
    { 
        /** If the endpoint is of type MPLS LM DM delete it */
        endpoint_info.id = *(int *)key;
        BCMDNX_IF_ERR_EXIT(bcm_petra_oam_endpoint_destroy(unit, endpoint_info.id));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group) {
    bcm_oam_endpoint_t current_endpoint;
    ENDPOINT_LIST_PTR mep_list_p ;
    ENDPOINT_LIST_MEMBER_PTR mep_iter_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    uint8 found;
    int rv;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    if (group == BCM_OAM_GROUP_INVALID)
    {
       /** In case of MplsLmDM go over all created endpoints */
       BCMDNX_IF_ERR_EXIT(_bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(unit, _bcm_oam_destroy_all_iterate_cb));
    }
    else
    {
      if (group < 0) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                           (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
      }

      LOG_VERBOSE(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM: All endpoints of group with id %d deleted\n"),
                       group));

      rv = _bcm_dpp_oam_ma_to_mep_db_get(unit, group, &mep_list_p, &found);
      BCMDNX_IF_ERR_EXIT(rv); 

      if (found)
      {
          /*Loop on all the MEPs of this MA*/
          rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, mep_list_p, &mep_iter_p) ;
          BCMDNX_IF_ERR_EXIT(rv);

          while (mep_iter_p != 0)
          {
              uint32 member_index ;

              rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, mep_iter_p, &member_index) ;
              BCMDNX_IF_ERR_EXIT(rv);
              current_endpoint = (bcm_oam_endpoint_t)member_index ;
              rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, mep_iter_p, &member_next) ;
              BCMDNX_IF_ERR_EXIT(rv);
              mep_iter_p = member_next ;

              rv = bcm_petra_oam_endpoint_destroy(unit, current_endpoint);
              BCMDNX_IF_ERR_EXIT(rv); 
          } 
      }
      else {
          LOG_WARN(BSL_LS_BCM_OAM,
                           (BSL_META_U(unit,
                                       " OAM Warning: Group %d has no endpoints.\n"), group));
      }

      LOG_VERBOSE(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM: All endpoints of group with id %d deleted\n"), group));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Destroy an OAM group object.  All OAM endpoints associated with the
 * group will also be destroyed.
 */
int bcm_petra_oam_group_destroy(
    int unit, 
    bcm_oam_group_t group) {

    int rv, i;
    _bcm_oam_ma_name_t ma_name;
    uint8 found;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    if (group < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: Group id must be positive. Given id: %d\n"), group));
    }

    rv = _bcm_dpp_oam_ma_db_get(unit, group, &ma_name, &found);
    BCMDNX_IF_ERR_EXIT(rv); 
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
            (_BSL_BCM_MSG("Error: Group %d not found.\n"), group));
    }

    /* maid48 delete entry from OAMP_PE_GEN_MEM */
    if ((SOC_IS_QAX(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)))
        && ma_name.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
        int is_last, rv, index;
        /* 
         * ma_name.imdex is the index to offset in generic memory,
         * the index in free counter is the index of entity in memory allocator
         * in order to translate it to index in free function we need to divide by 24 on JER and 48 on arad 
         * because on ARAD the genric memory  has one  byte on each entry and on jerico it has 2 bytes 
         */
        if (SOC_IS_QAX(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_bfd_mep_db_ext_data_delete,(unit, ma_name.index));
            /* Free the extended entries */
            for (i = 0; i < 3; i++, ma_name.index += (SOC_IS_QUX(unit) ? 0x100 : 0x400)) {
                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, ma_name.index);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            if (SOC_IS_JERICHO(unit)) {
                index = ma_name.index / 24;
            } else {
                index = ma_name.index / 48;
            }
            rv = _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_free(unit, index, &is_last);
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }


    rv = bcm_petra_oam_endpoint_destroy_all(unit, group);
    BCMDNX_IF_ERR_EXIT(rv); 

    /*delete MA name from SW db*/
    rv = _bcm_dpp_oam_ma_db_delete(unit, group);
    BCMDNX_IF_ERR_EXIT(rv); 

    rv = bcm_dpp_am_oam_ma_id_dealloc(unit, group);
    BCMDNX_IF_ERR_EXIT(rv);

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: Group with id %d deleted\n"),
                     group));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function traverses the entire set of a OAM group, calling a given callbeack for each one. 
 */
int bcm_petra_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb, void* user_data ) {
    int rv;
    uint8 oam_is_init;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    /*filling the global data*/
    _group_cb_with_param.cb = cb;
    _group_cb_with_param.user_data = user_data;

    /*from here the iterate function takes over.*/
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_iterate(unit,_bcm_petra_oam_group_traverse_cb);
       
    BCMDNX_IF_ERR_EXIT(rv);
        

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_endpoint_traverse(int unit, bcm_oam_group_t group,bcm_oam_endpoint_traverse_cb cb,void *user_data)
{  
    int rv;
    ENDPOINT_LIST_PTR mep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR mep_iter_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    uint8 found;
    bcm_oam_endpoint_info_t current_endpoint;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_ma_to_mep_db_get(unit,group,&mep_list_p, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (found)
    {
        /*Go over the endpoints in mep_list_p, calling the user's callback on each one.*/
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, mep_list_p, &mep_iter_p) ;
        BCMDNX_IF_ERR_EXIT(rv);

        while (mep_iter_p)
        {
            uint32 member_index ;

            bcm_oam_endpoint_info_t_init(&current_endpoint);
            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, mep_iter_p, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv);
            current_endpoint.id = member_index ;

            rv = bcm_petra_oam_endpoint_get(unit,current_endpoint.id, &current_endpoint);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, mep_iter_p, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv);
            mep_iter_p = member_next ;
            
            rv = cb(unit,&current_endpoint, user_data);
            if (rv != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback in bcm_oam_endpoint_traverse returned error message %s"),bcm_errmsg(rv)));
            }

        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_group_destroy_all(int unit) {

    int rv;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_iterate(unit, _bcm_dpp_oam_ma_db_delete_ma_cb);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_sw_db_hash_oam_ma_name_destroy(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_sw_db_hash_oam_ma_name_create(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Clear all OAM (used in debug) */
int _bcm_dpp_oam_clear(int unit) {
    int rv = BCM_E_NONE;
    bcm_port_config_t port_config;
    bcm_gport_t ethernet_port;
    uint32 soc_sand_rv;
    int trap_code;
    uint32 local_port;
    bcm_rx_trap_config_t trap_config;
    uint8 mirror_profile;
    uint8 oam_is_init, is_bfd_init;
    int trap_code_with_meta;
    SOC_PPC_PORT pp_port;
    int core = SOC_CORE_INVALID;
    uint32 trap_id_sw;
    int trap_code_converted;
    int i;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }

    rv = bcm_petra_oam_group_destroy_all(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    for (trap_code_with_meta = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
          trap_code_with_meta<(SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST+SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM);
          trap_code_with_meta++) {
        trap_id_sw = trap_code_with_meta;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted );
        BCMDNX_IF_ERR_EXIT(rv);
    }

    for (i = 0 ; i< 3; i++) {
        if ( i == 0 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP, &trap_id_sw);
        } else if ( i == 1 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_MPLS, &trap_id_sw);
        } else {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE, &trap_id_sw);
        }
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, TRUE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_CPU, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get snoop command */
    bcm_rx_trap_config_t_init(&trap_config);

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SNOOP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_get(unit, trap_code_converted, &trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy snoop trap id*/
    rv = bcm_petra_rx_snoop_destroy(unit, 0, trap_config.snoop_cmnd);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_get(unit, spn_OAM_RCY_PORT, -1)!=-1) {
        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_OAMP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(unit, trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
        /* free all resources used for egress snooping*/
        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_UP);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_rx_snoop_destroy(unit, 0,_BCM_OAM_MIP_SNOOP_CMND_DOWN);
        BCMDNX_IF_ERR_EXIT(rv);

        trap_id_sw = SOC_PPC_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit,trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /*LoopBack upmep is not supported in ARAD*/
    rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_RECYCLE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
    rv = bcm_petra_rx_trap_type_destroy(unit,trap_code_converted);
    BCMDNX_IF_ERR_EXIT(rv);
#endif /*LoopBack upmep is not supported in ARAD*/

    for (i = 0 ; i< 2; i++) {
        if ( i == 0 ) {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_id_sw);
        } else {
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_PASSIVE, &trap_id_sw);
        }
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
        rv = bcm_petra_rx_trap_type_destroy(unit, trap_code_converted);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*destroy all the mirror profiles*/
    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_OAMP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_CPU, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef _BCM_OAM_LOOPBACK_UPMEP_SUPPORTED /* LoopBack upmep not supported - trap to CPU using CPU trap id */
    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_RECYCLE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
#endif

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_SNOOP, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_ERR_LEVEL, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_ERR_PASSIVE, &trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);
    _BCM_OAM_MIRROR_PROFILE_DEALLOC(trap_id_sw);
    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, trap_id_sw, 0, 0, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* deinit _bcm_oam_cpu_trap_code_to_mirror_profile_map */
    for (trap_code = 1; trap_code < SOC_PPC_NOF_TRAP_CODES; trap_code++) {
        mirror_profile = 0;
        rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, mirror_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Get Ethernet ports */
    rv = bcm_petra_port_config_get(unit, &port_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Creating mirror profiles for egress functionality */
    BCM_PBMP_ITER(port_config.e, ethernet_port) {
        bcm_petra_mirror_port_dest_delete(unit, ethernet_port, BCM_MIRROR_PORT_EGRESS_ACL, 0);
    }

    for (local_port = 0; local_port < 255; local_port++) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &pp_port, &core)));

        soc_sand_rv = soc_ppd_oam_pp_pct_profile_set(unit, core, pp_port, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    _BCM_OAM_ENABLED_SET(FALSE);

    rv = _bcm_dpp_oamp_interrupts_init(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_oam_deinit(unit, 0/*is_bfd*/, !is_bfd_init);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_detach(int unit) {

    int rv = BCM_E_NONE;
    uint8 oam_is_init, is_bfd_init;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    } 

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    if (!is_bfd_init) {
        rv = _bcm_dpp_oam_dbs_destroy(unit, TRUE/*is_oam*/);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* deinit dma */
    BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_arad_pp_oam_dma_clear, (unit)));


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_opcode_map_set(int unit, int opcode, int profile) {

    int rv = BCM_E_NONE;
    uint8 internal_opcode;
    uint8 oam_is_init;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    } 

    rv = _bcm_petra_oam_opcode_map_set_verify(unit, opcode, profile);
    BCMDNX_IF_ERR_EXIT(rv);

    internal_opcode = (uint8)profile;
    rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.set(unit, opcode, internal_opcode);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_oam_eth_oam_opcode_map_set(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_opcode_map_get(int unit, int opcode, int *profile) {

    int rv = BCM_E_NONE;
    uint8 internal_opcode;
    uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    } 

    rv = _bcm_petra_oam_opcode_map_get_verify(unit, opcode);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
    BCMDNX_IF_ERR_EXIT(rv);
    *profile = (uint32)internal_opcode;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/***************** LM, DM, and more (ARAD+ and above)*/

/**
 * Function manages the troika of profiles eth1731, NIC and OUI.
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile - may be NULL when called by 
   *                    loopback_add()
 *   @param is_update set to 1 if loopback/delay/loss_add() was
 *                    called with the update flag set
 *   @param is_ok returns 1 if all checks were sucessfull, 0
 *                otherwise/
 * @param is_dm - "1" means DM entry, "0" means LM entry 
 * @param lm_dm_period - loss or delay period in milliseconds
 * 
 * @return int 
 */
int _manage_DA_with_existing_entries_internal(int unit , int mep_id ,SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb, 
                                              SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *eth_1731_profile, int is_update, uint8* is_ok, uint8 is_dm, int lm_dm_period) {
    uint8  has_other_entry;
    uint32 soc_sand_rv, three_bytes_from_mac_addr;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY old_eth_data;
    int only_piggy_back_exists=0, is_last, nic_profile, oui_profile, old_eth1731_profile;
    int rv, is_allocated, profile_indx;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *eth1731_profile_to_use= eth_1731_profile? eth_1731_profile: &old_eth_data;
    int (*alloc_function) (int, int,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY * , int *, int *, int*, int*, int, uint8) = eth_1731_profile==NULL?  _bcm_oam_am_template_oam_eth_1731_mep_profile_loopback_alloc:
        (is_dm)? _bcm_dpp_am_template_oam_eth_1731_mep_profile_delay_alloc :
        _bcm_dpp_am_template_oam_eth_1731_mep_profile_loss_alloc;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    uint32 found_bitmap[1] = {0};
    int on_demand_dmm=0;
    int dmm_phase = 0;
    uint8 is_scan_count_updated = 0; 
    uint8 lm_dm_rate_index = 0, old_lm_dm_rate_index = 0; 
    uint32 old_tx_rate = 0;
    uint32 tx_rate = 0;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth_data);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    if (is_dm && (classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE ) && eth_1731_profile &&
        eth_1731_profile->dmm_rate && is_update) {
        /* An on demand service is active for this mep, adding pro-active version. Turn off the flag*/
        classifier_mep->flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
    }

    /*First, see if any other entries are using the profiles*/
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, mep_id, found_bitmap));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (eth_1731_profile==NULL) { /* go here only if this function was called by LB.*/
        has_other_entry = *found_bitmap > 0;
    } else {
        on_demand_dmm = (!is_update && is_dm && (eth_1731_profile->dmm_rate == 0));
        /* for LM, search for DM and vice-a-versa*/
        if ((classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY)|| 
            classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) {
            /* in this case the LB is using the profiles. There definitely was another entry.*/
            has_other_entry=1;
        } else {
            /* There was only another entry if an LM/DM entry was found.*/
            has_other_entry= (is_dm)? ( SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM)  !=0) : SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM) !=0;
        }
        BCM_DPP_UNIT_CHECK(unit);
        /* Update the offsets*/
        soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, mep_id, &mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

        soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
           unit,
           &user_header_0_size,
           &user_header_1_size,
           &user_header_egress_pmf_offset_0_dummy,
           &user_header_egress_pmf_offset_1_dummy
         );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

        user_header_size = (user_header_0_size + user_header_1_size) / 8; 

        if (is_dm) {
            /*Set DM offsets */
            eth_1731_profile->dmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 0 /* LMM */) + user_header_size;
            eth_1731_profile->dmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, 0 /* not PG-back */, 0 /* not SLM */, 1 /* not DM */, 1 /* DMR */) + user_header_size;
        } else {
            /*Set LM offsets */
            eth_1731_profile->lmm_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm, 0 /* not DM */, 0 /* LMM */) + user_header_size;
            eth_1731_profile->lmr_offset = _oam_set_lm_dm_offset(&mep_db_entry, eth_1731_profile->piggy_back_lm, eth_1731_profile->slm_lm, 0 /* not DM */, 1 /* LMR */) + user_header_size;
        }
    }


    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, mep_id, &old_eth1731_profile, &old_eth_data);
    BCMDNX_IF_ERR_EXIT(rv);

    if (_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry.mep_type) && eth_1731_profile!=NULL)
    {
        classifier_mep->is_rfc_6374_entry = 1;
        if (is_update)
        {
            old_lm_dm_rate_index = is_dm ? old_eth_data.dmm_rate : old_eth_data.lmm_rate;
            rv = BFD_ACCESS.tx_rate_value.get(unit,
                                          old_lm_dm_rate_index,
                                          &old_tx_rate);
            lm_dm_rate_index = old_lm_dm_rate_index;
            BCMDNX_IF_ERR_EXIT(rv);
            eth_1731_profile->lmm_cnt = old_eth_data.lmm_cnt;
            eth_1731_profile->dmm_cnt = old_eth_data.dmm_cnt;
        }

        /** In case of RFC6374 lm_dm_period 0 set the tx_rate value to 0 */
        tx_rate = lm_dm_period ? _BCM_BFD_ENDPOINT_INFO_BFD_PERIOD_TO_MEP_DB_TX_RATE(lm_dm_period) : 0;
        rv = _bcm_bfd_set_and_allocate_bfd_rates(unit, tx_rate, old_tx_rate, &lm_dm_rate_index, is_update);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_dm)
        {
            eth_1731_profile->dmm_rate = lm_dm_rate_index;
            /** Set the tx phase to bigger value to disable the transmission, any value bigger then 0 will work */
            eth_1731_profile->dmm_cnt = lm_dm_period ? 0 : 14;
        } 
        else
        {
            eth_1731_profile->lmm_rate = lm_dm_rate_index;
            /** Set the tx phase to bigger value to disable the transmission, any value bigger then 0 will work */
            eth_1731_profile->lmm_cnt = lm_dm_period ? 0 : 15;
        }

        is_scan_count_updated = 1;
    }

    /* Next, see if only an LM piggy back entry is using the profiles.*/
    if (has_other_entry) {
        if (old_eth_data.piggy_back_lm && (SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM))==0 &&
            !(classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ) {
            only_piggy_back_exists =1;
        }

        /* Quick check that that piggy-back, slm status have not been changed through update*/
        if (is_update && eth_1731_profile && !is_dm ) {
            /* LM + update*/
            if (old_eth_data.piggy_back_lm != eth_1731_profile->piggy_back_lm) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Piggy back mode may not be changed.")));
            }
            if (old_eth_data.slm_lm != eth_1731_profile->slm_lm) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM mode may not be changed.")));
            }
        }
    }


    /* It may be the case that this has been called for one shot on demand DMM */
    if (is_dm && on_demand_dmm) {
        /* Two cases: on demand on top of proactive or not */
        /* profile.report_mode is 2 bit bitmap that indicate which report is active:
           00 - None    01 - LM     10 - DM     11-Both
           In order to check if DM reports are active we need to check the 2-nd bit.
        */
        if ( SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM) ==0 && !((old_eth_data.report_mode & 2) && old_eth_data.dmm_rate) )  {
            /* only on demand mode. turn on the flag.*/
            classifier_mep->flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
        } else {
            has_other_entry=1; /* At the very least there is a proactive dmm entry. Compare against it.*/
            eth_1731_profile->dmm_rate = old_eth_data.dmm_rate; /* So as not to change existing settings.*/
        }
    }

    /* In case of delay/loss disable summation of lmm_rate and dmm_rate will be zero.
     * So, for delay_disable, lmm_count will not be updated. So, we have notify that with a flag that it is noy updated.
     * Same will happen for dmm_count in loss_disable. 
     * In this case is_scan_counted will be zero, and alloc_function will take it from old_profile.
     */
    if (eth_1731_profile!=NULL && (eth_1731_profile->lmm_rate + eth_1731_profile->dmm_rate) && !eth_1731_profile->piggy_back_lm &&
        on_demand_dmm==0 && !_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(mep_db_entry.mep_type)) {
        /* verify amount of outgoing packets, but not if this function was called by looback_add() , one shot DMM, P-B*/
        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY actual_transmission ={0}; /* It may be the case that the LM/DM rate is positive but no LMM/DMMs are actually being tranmsitted.
        This object represents the DMMs/LMMs actually transmitted*/
        if (old_eth_data.report_mode) {
            /* When report mode is used the above comment is untrue. In this case the tranmsmission rate stated in the MEP PROFILE is the actual transmission rate.*/
            actual_transmission = old_eth_data;
        } else {
            actual_transmission.opcode_0_rate  = old_eth_data.opcode_0_rate;
            actual_transmission.opcode_1_rate  = old_eth_data.opcode_1_rate;
            actual_transmission.dmm_rate = ( SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM)  !=0) * old_eth_data.dmm_rate ;
            actual_transmission.lmm_rate = ( SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM)  !=0) * old_eth_data.lmm_rate ;
            actual_transmission.ccm_cnt  = old_eth_data.ccm_cnt;
        }
        if(_BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)){
            dmm_phase = ((mep_id/16) %8);
            if (dmm_phase  == 1) {
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 0 : 1;
            } else if (dmm_phase  == 2){
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 2 : 3;
            } else if (dmm_phase  == 3) {
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 4 : 5;
            } else if (dmm_phase  == 4){
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 6 : 7;
            } else if (dmm_phase  == 5){
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 8 : 9;
            } else if (dmm_phase  == 6) {
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 10 : 11;
            } else if (dmm_phase  == 7){
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 12 : 13;
            } else if (dmm_phase  == 0){
                actual_transmission.dmm_cnt = old_eth_data.ccm_cnt ? 14 : 15;
            }
        }

        rv = _oam_oamp_verify_maximal_tx_per_mep(unit,
                                                 mep_db_entry.ccm_interval,
                                                 actual_transmission,
                                                 eth_1731_profile->lmm_rate + eth_1731_profile->dmm_rate  /*The one that isn't used should be 0.*/,
                                                 eth_1731_profile,
                                                 is_dm);
        BCMDNX_IF_ERR_EXIT(rv);
        is_scan_count_updated=1;

    }

    /* Now verify and manage the MAC address profiles.*/
    if ( _BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
        bcm_mac_t loss_delay_da = {0};
        *is_ok = 1;
        /* Go ahead and write the new value if its either LM/DM. */
        if (eth_1731_profile &&
            ((eth_1731_profile->lmm_rate != 0) || (eth_1731_profile->dmm_rate != 0))) {
            *is_ok = 0;
            SET_MAC_ADDRESS_BY_MSB_AND_LSB(loss_delay_da, lsb, msb);
            rv = _bcm_oam_custom_feature_oamp_flex_da_mac_set(unit, mep_id, loss_delay_da);
            BCMDNX_IF_ERR_EXIT(rv);
            *is_ok = 1;
        }
    } else {
        if (!has_other_entry || only_piggy_back_exists) {
            /* at the very least the DA is available*/
            *is_ok =1;
            if (classifier_mep->mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM &&  !(eth_1731_profile && eth_1731_profile->dmm_rate==0 && eth_1731_profile->piggy_back_lm==1 ) ) {
                /* update the DA registers only in the case of ETH-OAM and non piggy back entry*/
                if (is_update) {
                    /* Free existing DA profiles for this MEP.*/
                    FREE_NIC_AND_OUI_PROFILES(mep_id);
                }
                /* Now alloc.*/
                rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, 0/* flags*/, &lsb, &is_allocated, &profile_indx);
                BCMDNX_IF_ERR_EXIT(rv);
                nic_profile = profile_indx ;

                rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, 0/* flags*/, &msb, &is_allocated, &profile_indx);
                BCMDNX_IF_ERR_EXIT(rv);
                oui_profile = profile_indx;

                soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit, mep_id, msb, lsb, oui_profile, nic_profile);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
                eth1731_profile_to_use->lmm_da_oui_prof = profile_indx; /* oui_profile may be -1*/
            } else {
                eth1731_profile_to_use->lmm_da_oui_prof =old_eth_data.lmm_da_oui_prof;
            }
        } else {
            /* Someone else is using the profiles. Verify that the DA addresses are OK and update the SW/HW*/
            /*First, verifying */
            if (classifier_mep->mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM && !(eth_1731_profile && eth_1731_profile->dmm_rate==0 && eth_1731_profile->piggy_back_lm==1 ) ) {
                /* update the DA registers only in the case of ETH-OAM and non piggy back entry*/
                rv = _bcm_dpp_am_template_oam_lmm_nic_tables_get(unit,mep_id, &nic_profile, &three_bytes_from_mac_addr);
                BCMDNX_IF_ERR_EXIT(rv);
                if (three_bytes_from_mac_addr != lsb) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Cannot insert 0x%x as the LSB of the MAC address. Existing LSB: 0x%x \n"), lsb, three_bytes_from_mac_addr));
                    *is_ok = 0;
                    goto exit;
                }
                rv = _bcm_dpp_am_template_oam_lmm_oui_tables_get(unit,mep_id, &oui_profile, &three_bytes_from_mac_addr);
                BCMDNX_IF_ERR_EXIT(rv);
                if (three_bytes_from_mac_addr != msb) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Cannot insert 0x%x as the LSB of the MAC address. Existing LSB: 0x%x \n"), lsb, three_bytes_from_mac_addr));
                    *is_ok = 0;
                    goto exit;
                }
                eth1731_profile_to_use->lmm_da_oui_prof = oui_profile;
                /* Still alive. First update the reference counters for the DA registers...*/
                if (is_update==0) {
                    /* ...but only in the case that update hasn't been selected (otherwise we already verified that these are alright.*/
                    UPDATE_NIC_AND_OUI_REFERENCE_COUNTER(msb,lsb,nic_profile, oui_profile);
                }
            }
            eth1731_profile_to_use->lmm_da_oui_prof =old_eth_data.lmm_da_oui_prof; /* Use the existing lmm_da_oui profile.*/
            *is_ok = 1;
        }
    }


    /*finally, the eth1731 profile*/
    rv = alloc_function(unit, mep_id, eth1731_profile_to_use, &is_allocated, &profile_indx, &old_eth1731_profile, &is_last, !has_other_entry,is_scan_count_updated);
    BCMDNX_IF_ERR_EXIT(rv);
    if (old_eth1731_profile != profile_indx) {
        CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, old_eth1731_profile, mep_id);
    }
    soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, mep_id, !is_allocated, profile_indx, eth1731_profile_to_use);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

exit:
    BCMDNX_FUNC_RETURN;
}





/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address, if 
 * such exists. If all is well function allocates profiles and 
 * updates HW. 
 *  
 * Function should only be called when MEP_TPYE is Ethernet. 
 *  
 * @author sinai (30/10/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param is_ok  returns 1 if incoming MAC address agrees with 
 *               existing MAC adress, 0 otherwise.
 * @param lm_dm_period  - loss period
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_loopback(int unit,int mep_id, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb ,  uint8 * is_ok, int lm_dm_period) {

  int rv = BCM_E_NONE;
  BCMDNX_INIT_FUNC_DEFS;
  rv = _manage_DA_with_existing_entries_internal(unit,mep_id,classifier_mep,msb,lsb,NULL,
                                                     classifier_mep->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY,is_ok, 0 /* LB entry */, 0 /* Not relevant */);
  BCMDNX_IF_ERR_EXIT(rv);

  exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address and 
 * then updates the SW, HW. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile 
 * @param is_ok 
* @param lm_dm_period  - loss period
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_loss(int unit, int mep_id, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb , 
                                            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY * eth_1731_profile,int is_update, uint8 * is_ok, int lm_dm_period) {

  int rv = BCM_E_NONE;
  BCMDNX_INIT_FUNC_DEFS;
  rv = _manage_DA_with_existing_entries_internal(unit, mep_id, classifier_mep, msb, lsb, eth_1731_profile, is_update, is_ok, 0/*is dm*/, lm_dm_period);
  BCMDNX_IF_ERR_EXIT(rv);

  exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Function verifies that current 
 * DA settings for endpoint agree with given MAC address and 
 * then updates the SW, HW. 
 *  
 * When function is called in the on-demand mode 
 * classifier_mep->flags is updated if there is no  pro-active 
 * service active. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param classifier_mep 
 * @param msb 
 * @param lsb 
 * @param eth_1731_profile 
 * @param is_ok 
 * @param lm_dm_period  - delay period
 * 
 * @return int 
 */
int  _manage_DA_with_existing_entries_delay(int unit, int mep_id, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep, uint32 msb, uint32 lsb , 
                                            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY * eth_1731_profile,int is_update, uint8 * is_ok, int lm_dm_period) {

  int rv = BCM_E_NONE;
  BCMDNX_INIT_FUNC_DEFS;
  rv = _manage_DA_with_existing_entries_internal(unit,mep_id,classifier_mep,msb,lsb,eth_1731_profile, is_update,is_ok, 1/*is dm*/, lm_dm_period);
  BCMDNX_IF_ERR_EXIT(rv);

  exit:
    BCMDNX_FUNC_RETURN;
}


/* Add shared entry/ies for LM/DM in QAX style MEP-DB (splitted to banks) */
int _bcm_oam_lm_dm_pointed_add_shared(int unit, SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *lm_dm_mep_db_entry) {

    uint32 soc_sand_rv;
    bcm_error_t rv = BCM_E_NONE;
    int entries_allocated = 0, entries_deallocated = 0;
    uint32 entry;

    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_entry;
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO shared_entries_data = {0};
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 found = 0;
    _bcm_oam_ma_name_t ma_name_struct;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    /* Get some extra info */
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, lm_dm_mep_db_entry->mep_id, &mep_entry);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit,  lm_dm_mep_db_entry->mep_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Make sure the call sequence logic checks out */
    rv = _bcm_oam_lm_dm_pointed_add_shared_verify(unit, lm_dm_mep_db_entry, &mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Init aux struct */
    shared_entries_data.endpoint_id = lm_dm_mep_db_entry->mep_id;
    sal_memcpy(&shared_entries_data.mep_entry, &mep_entry, sizeof(SOC_PPC_OAM_OAMP_MEP_DB_ENTRY));
    shared_entries_data.lm_dm_entry = lm_dm_mep_db_entry;
    shared_entries_data.action_type =
        ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_ADD_UPDATE;
    shared_entries_data.out_lif = mep_entry.out_lif;

    rv = _bcm_dpp_oam_ma_db_get(unit, classifier_mep_entry.ma_index, &ma_name_struct, &found);
    SOC_SAND_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("Error: Group %d is not valid\n"),  classifier_mep_entry.ma_index));
    }
    if(ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
        shared_entries_data.is_maid_48 = 1;
    } else{
        shared_entries_data.is_maid_48 = 0;
    }

    /* Find entries to add/remove */
    rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_lm_dm_pointed_find,
                             (unit, &shared_entries_data));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocate entries if necessary */
    for (entries_allocated = 0;
          entries_allocated < shared_entries_data.num_entries_to_add;
          ++entries_allocated) {
        entry = shared_entries_data.entries_to_add[entries_allocated];
        rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, 0,
                                       0 /*ignored anyways*/ , 0, &entry);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP-DB entry %d allocated\n"), entry));
    }

    /* Set/Update the entries */
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lm_dm_set,
                             (unit, &shared_entries_data));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Deallocate entries if necessary */
    for (entries_deallocated = 0;
          entries_deallocated < shared_entries_data.num_entries_to_remove;
          ++entries_deallocated) {
        entry = shared_entries_data.entries_to_remove[entries_deallocated];
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP-DB entry %d deallocated\n"), entry));
    }
    /*Update OAMP PE MEP profile in 1 DM case */
    if ((lm_dm_mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM) &&
        (lm_dm_mep_db_entry->is_1DM)) {
        /*Now let soc take over.*/
        soc_sand_rv = soc_ppd_oam_oamp_lm_dm_set(unit,lm_dm_mep_db_entry);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    if (rv != BCM_E_NONE) {
        /* In case of error try deallocating allocated entries */
        for (; entries_allocated ; --entries_allocated) {
            entry = shared_entries_data.entries_to_add[entries_allocated-1];
            LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Deallocating MEP-DB entry %d\n"), entry));
            rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Error: couldn't free unused MEP-DB entry: %d\n "), entry ));
            }

        }
    }
    BCMDNX_FUNC_RETURN;
}


/**
 * Function adds aditional entries in the MEP DB in the SW and 
 * HW. 
 * 
 * @author sinai (13/11/2013)
 * 
 * @param unit 
 * @param mep_db_entry 
 * 
 * @return int 
 */
int _bcm_oam_lm_dm_add_shared(int unit,SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry){
    uint8  entries_alloced=0,has_dm;
    uint32 entry = 0, soc_sand_rv;
    int rv=0;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO_PLUS(unit)) {
       if (mep_db_entry->is_slm) {
          soc_sand_rv = arad_pp_oam_oamp_slm_set(unit, mep_db_entry);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }
    }
    if (SOC_IS_QAX(unit)) {
       rv = _bcm_oam_lm_dm_pointed_add_shared(unit, mep_db_entry);
       BCMDNX_IF_ERR_EXIT(rv);
       BCM_EXIT;
    }

    soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, mep_db_entry->mep_id, &entry, &has_dm);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    if ( !mep_db_entry->is_update) {
        if ( entry ==0 || entry==mep_db_entry->mep_id + 4) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("LM/DM can not be added.")));
        }
        mep_db_entry->allocated_id = entry;
        rv =  _bcm_dpp_oam_mep_id_alloc(unit,SHR_RES_ALLOC_WITH_ID ,0 /*ignored anyways*/, 0 , 0, &entry); 
        BCMDNX_IF_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("request for entry %d in MEP DB denied."),entry) );
        ++entries_alloced;
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "entry %d allocated\n"), entry));
        if (mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) { /*grab another one*/
            ++entry ;
            rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID,0 /*ignored anyways*/ , 0, 0, &entry);
            BCMDNX_IF_ERR_EXIT(rv);
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                                    (BSL_META_U(unit,
                                                "extended statistics requested: entry %d allocated as well\n"),entry));
            ++entries_alloced;
        }
    } else {
        /* in the LM case, see if EXTENDED STATISTICS has been changed */
        if (mep_db_entry->entry_type != SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM) {
            int num_entries = entry - mep_db_entry->mep_id - 1; /* "entry" gives us the next available entry. This gives us the # of entries
             associated with this endpoint. */
            if (num_entries == 1) {
                if (mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
                    /* From LM only to LM + EXTENDED*/
                    entry = mep_db_entry->mep_id + 2;
                    rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, 0 /*ignored anyways*/, 0, 0, &entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else if (num_entries == 2) {
                if (has_dm && mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
                    /* From LM,DM to LM + EXTENDED, DM*/
                    entry = mep_db_entry->mep_id + 3;
                    rv = _bcm_dpp_oam_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, 0 /*ignored anyways*/, 0, 0, &entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else if (!has_dm && mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM) {
                    /* From LM + EXTENDED to LM ONLY*/
                    entry = mep_db_entry->mep_id + 2;
                    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else if (num_entries == 3) {
                if (has_dm && mep_db_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM) {
                    /* From LM+ EXTENDED , DM to LM, DM*/
                    entry = mep_db_entry->mep_id + 3;
                    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    /*Now let soc take over.*/
    soc_sand_rv = soc_ppd_oam_oamp_lm_dm_set(unit,mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


exit:
    if ((_rv!=0 || rv !=0 ) && entries_alloced) {/*the damage we can undo is free the mep-db's allocated.*/
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Freeing unused MEP-DB entry %d \n"), entry ));
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
        if (rv !=0) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Error: couldn't free unused MEP-DB entry: %d\n "), entry ));
            return rv;
        }
        if (entries_alloced==2) {
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Freeing second unused MEP-DB entry: %d \n"), entry-1 ));
            rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry-1);
            if (rv != 0) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Error: couldn't free second unused MEP-DB entry: %d \n"), entry));
                return rv;
            }
        }
    }

    BCMDNX_FUNC_RETURN;
}

/*
 * Delete shared entry/ies for LM/DM in QAX style MEP-DB (splitted to
 * banks)
 */
int _bcm_oam_lm_dm_pointed_remove_shared(int unit, bcm_oam_endpoint_t endpoint_id, uint8 is_lm)
{
    uint32 soc_sand_rv;
    bcm_error_t rv = BCM_E_NONE;

    int entries_deallocated = 0;
    uint32 entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 found = 0;
    _bcm_oam_ma_name_t ma_name_struct;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO shared_entries_data = {0};

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit,  endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get some extra info */
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, endpoint_id, &shared_entries_data.mep_entry);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Init aux struct */
    shared_entries_data.endpoint_id = endpoint_id;
    shared_entries_data.action_type = is_lm ?
        ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_LM :
        ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_DM ;

    shared_entries_data.out_lif = shared_entries_data.mep_entry.out_lif;

    rv = _bcm_dpp_oam_ma_db_get(unit, classifier_mep_entry.ma_index, &ma_name_struct, &found);
    SOC_SAND_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("Error: Group %d is not valid\n"),  classifier_mep_entry.ma_index));
    }
    if(ma_name_struct.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
        shared_entries_data.is_maid_48 = 1;
    } else{
        shared_entries_data.is_maid_48 = 0;
    }

    /* Find entries to remove */
    rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_lm_dm_pointed_find,
                             (unit, &shared_entries_data));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Delete the entries */
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lm_dm_delete,
                             (unit, &shared_entries_data));
    BCMDNX_IF_ERR_EXIT(rv);

    /* Deallocate entries */
    for (entries_deallocated = 0;
          entries_deallocated < shared_entries_data.num_entries_to_remove;
          ++entries_deallocated) {
        entry = shared_entries_data.entries_to_remove[entries_deallocated];
        rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, entry);
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP-DB entry %d deallocated\n"), entry));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
* Function sends LM/DM packets to the destination in the action. (unicast forward). 
* when is_lm is set, the last parameter acts as is_piggy_backed, otherwise as is_1dm.
* All other properties in the classifier remain uncahnged.
*/
int _bcm_oam_set_classifier_action(int unit, uint32 is_add, uint32 mep_id, bcm_oam_endpoint_action_t action,int is_lm,int is_1dm_or_piggy_back,SOC_PPC_OAM_MEP_TYPE mep_type ,int is_slm) {
    int is_piggy_backed = is_1dm_or_piggy_back;
    SOC_PPC_OAM_LIF_PROFILE_DATA *lif_profile_ptr = NULL;
    int meter_counter_opcode;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(lif_profile_ptr, sizeof(*lif_profile_ptr),"_bcm_oam_set_classifier_action.lif_profile_ptr");

    if (lif_profile_ptr == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, mep_id, lif_profile_ptr);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);
    action.flags = 0;

    if (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
    } else {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
    }

    if (is_slm) {
        BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM);
        BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR);
        BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
        BCM_OAM_ACTION_SET(action, bcmOAMActionSLMEnable);
    } else if (!is_lm) {
        if (is_1dm_or_piggy_back) {
            BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM);
        }
        if (SOC_IS_ARADPLUS_A0(unit)) { /*Reply packets handled seperatly in Jericho*/
            BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM);
        }
        BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR);
        if (!SHR_BITGET(lif_profile_ptr->mep_profile_data.counter_disable, SOC_PPC_OAM_OPCODE_MAP_DMM)) {
            BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
        }
        if (!SHR_BITGET(lif_profile_ptr->mep_profile_data.meter_disable, SOC_PPC_OAM_OPCODE_MAP_DMM)) {
            BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable);
        }
        is_piggy_backed = 2; /*ignore*/
    } else {
        meter_counter_opcode = is_piggy_backed ? SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM : SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM;
        if (is_1dm_or_piggy_back) {
            if (is_add) {
                BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM);
            }
        }
        else {
            BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR);
            if((SOC_IS_QAX(unit))&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)
                &&((mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)||(mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE))){
                BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM);
            }
            if (SOC_IS_ARADPLUS_A0(unit)) {/*Reply packets handled seperatly in Jericho*/
                BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM);
            }
        }
        if (!SHR_BITGET(lif_profile_ptr->mep_profile_data.meter_disable, meter_counter_opcode)) {
            BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable);
        }
    }
    rv = _bcm_oam_endpoint_action_set_validity_check(unit,mep_id,&action);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_petra_oam_endpoint_action_set_internal(unit,mep_id,&action,is_piggy_backed,2/*ignore*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCM_FREE(lif_profile_ptr);
    BCMDNX_FUNC_RETURN;
}

int _bcm_oam_lm_dm_remove_shared(int unit,bcm_oam_endpoint_t endpoint_id , int is_lm, SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry) {
    int rv, is_removed, is_allocated;
    int  new_eth_1731_prof,eth1731_profile;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *old_profile = NULL;
    uint32 soc_sand_rv, removed_index;
    uint8 num_removed;
    bcm_oam_endpoint_action_t action;
    char l_or_d = is_lm? 'L':'D';
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *eth1731_profile_data = NULL;
    uint32 found[1] = {0};
    SOC_PPC_OAM_MEP_TYPE ccm_entry_type = classifier_mep_entry->mep_type;
    uint8 has_LBR = (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) >0;
    uint8 only_on_demand_dmm = !is_lm &&  classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
    uint8 exists_down_mep_piggy_back=0;
    uint8 is_upmep;
    int trap_code = 0;

    BCMDNX_INIT_FUNC_DEFS;

    (void)l_or_d; /*for compilation*/

    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    BCMDNX_ALLOC(old_profile, sizeof(*old_profile),"_bcm_oam_lm_dm_remove_shared.old_profile");
    if (old_profile == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    BCMDNX_ALLOC(eth1731_profile_data, sizeof(*eth1731_profile_data),"_bcm_oam_lm_dm_remove_shared.eth1731_profile_data");
    if (eth1731_profile_data == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_id) == 0) {
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit,endpoint_id , &eth1731_profile, old_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!only_on_demand_dmm && !old_profile->report_mode) {
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, endpoint_id, found));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            *found = is_lm ? SHR_BITGET(found, SOC_PPC_OAM_MEP_TYPE_LM) != 0 : SHR_BITGET(found, SOC_PPC_OAM_MEP_TYPE_DM) != 0;

            if (*found == 0) {
                LOG_DEBUG(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "No %cM entry to remove for endpoint %d.\n"),l_or_d, endpoint_id));
                BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
            }

            /*Now we know there is something to remove so we can delete in the HW and SW*/
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "About to remove %cM entry for endpoint %d.\n"), l_or_d, endpoint_id));
        }

        /*De-alloc from the mep_db, da_nic, da_oui and eth1731-profile tables.*/
        if ((!old_profile->piggy_back_lm || !is_lm) &&  ccm_entry_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
            /*piggy back entries did not allocate entries from the DA MAC tables*/
            FREE_NIC_AND_OUI_PROFILES(endpoint_id);
        }
        if (old_profile->piggy_back_lm  && (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry)) && SOC_IS_ARADPLUS_A0(unit)) {
            /* In case we are removing a DM entry and the existing a piggybacked LM exists for an LM, and the direction is down, the MEP PE profile should be 0
               so as not to select the UP-MEP-MAC fix LSB program.*/
            exists_down_mep_piggy_back = 1;
        }

        SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(eth1731_profile_data);

        /*Next, get a different profile. Fill all the fields from the old entry.*/
        sal_memcpy(eth1731_profile_data, old_profile, sizeof(SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY));

        /* report mode will be updated based upon follwoing if logic.*/
        eth1731_profile_data->report_mode = 0;

        if (is_lm) {
            if (classifier_mep_entry->is_rfc_6374_entry) {
                rv = _bcm_bfd_tx_rate_profile_index_free(unit, eth1731_profile_data->lmm_rate);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* During loss_delete, clear all the lm relaated parameters */
            eth1731_profile_data->lmm_rate = 0;
            eth1731_profile_data->piggy_back_lm = 0;
            eth1731_profile_data->slm_lm = 0;
            eth1731_profile_data->lmm_cnt = 0;

            if (old_profile->dmm_rate != 0) {
                /* Still using the report mode bit */
                eth1731_profile_data->report_mode = old_profile->report_mode;
            }
        } else {
            if (classifier_mep_entry->is_rfc_6374_entry) {
                rv = _bcm_bfd_tx_rate_profile_index_free(unit, eth1731_profile_data->dmm_rate);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* During delay_delete, clear all the dm relaated parameters */
            eth1731_profile_data->dmm_rate = 0;
            eth1731_profile_data->dmm_cnt = 0;
            eth1731_profile_data->dmm_measure_one_way = 0;

            if ((old_profile->lmm_rate != 0) || old_profile->piggy_back_lm) {
                /* Still using the report mode bit */
                eth1731_profile_data->report_mode = old_profile->report_mode;
            }
        }

        if (has_LBR || eth1731_profile_data->dmm_rate || eth1731_profile_data->lmm_rate
            || (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE)) {
            eth1731_profile_data->lmm_da_oui_prof = old_profile->lmm_da_oui_prof;
        }
        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, eth1731_profile, eth1731_profile_data, &is_allocated, &is_removed, &new_eth_1731_prof);
        if (eth1731_profile != new_eth_1731_prof) {
            CLEAR_ETH_1731_PROF_IF_IS_LAST(is_removed, eth1731_profile, endpoint_id);
        }
        soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, endpoint_id, !is_allocated, new_eth_1731_prof, eth1731_profile_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (!only_on_demand_dmm && !old_profile->report_mode) {
            if (SOC_IS_QAX(unit)) {
                rv = _bcm_oam_lm_dm_pointed_remove_shared(unit, endpoint_id, is_lm);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                soc_sand_rv = soc_ppd_oam_oamp_lm_dm_remove(unit, endpoint_id, is_lm, mep_db_entry,  exists_down_mep_piggy_back,   &num_removed, &removed_index);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* Now free the the mep db entries*/
                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index);
                BCMDNX_IF_ERR_EXIT(rv);
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "entry %d freed\n"), removed_index));
                if (num_removed == 2) { /*Go here if we want to delete an LM entry and there is also a LM-STAT entry.*/
                    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, removed_index + 1);
                    BCMDNX_IF_ERR_EXIT(rv);
                    LOG_VERBOSE(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "entry %d freed as well\n"), removed_index + 1));
                }
            }
        }
    }

    if ((classifier_mep_entry->lif != _BCM_OAM_INVALID_LIF) && (!classifier_mep_entry->is_rfc_6374_entry)) { /* OAM server: do not set the classifier*/
        uint8 using_1dm =0;
        /* Have the classifier send LM/DM packets back to the CPU. */
        is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);
        rv = bcm_rx_trap_type_get(unit, 0, is_upmep ? bcmRxTrapOamUpMEP : bcmRxTrapBfdOamDownMEP, &trap_code);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_oam_endpoint_action_t_init(&action);
        BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

        /* check for 1DM if it is DM delete*/
        if(!is_lm) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,endpoint_id, &using_1dm));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /*
         * second parameter is_add : 1 for adding a loss/dm; 0 for deleting a loss/dm 
         * fourth parameter piggy_back_or_1DM : for DM always 1, for LM 0 iff removed entry was P-B
         */
        rv = _bcm_oam_set_classifier_action(unit, 0/*is_add*/, endpoint_id,action,is_lm, (using_1dm || (is_lm && old_profile->piggy_back_lm)) ,ccm_entry_type, old_profile->slm_lm/*is_slm*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (only_on_demand_dmm) {
        uint8 found;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found.\n"), endpoint_id));
        }

        classifier_mep_entry->flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;

        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, endpoint_id, classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCM_FREE(old_profile);
    BCM_FREE(eth1731_profile_data);
    BCMDNX_FUNC_RETURN;
}



/**
 * Function removes the Loopback reply mechanism by updating the
 * classifier and the DA registers (the latter in case of 
 * Ethernet OAM). Note that the function does not update the 
 * classifier_mep_entry for the given endpoint. 
 *  
 * It is the callers responsibility to ensure that these actions 
 * are safe (i.e. oam was initialized, is arad+, etc.). 
 * 
 * @author sinai (06/11/2013)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param classifier_mep_entry 
 * 
 * @return int 
 */
int _remove_loopback_reply(int unit, int endpoint_id, const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    bcm_oam_endpoint_action_t action;
    uint8 is_upmep;
    int trap_code = 0;
    uint32 soc_sand_rv;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { /* DA address profiles*/
        FREE_NIC_AND_OUI_PROFILES(endpoint_id);
        /* The eth1731 profile will remain as it is. */
    }

    is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);

    if (is_upmep || soc_property_get(unit, spn_OAM_RCY_PORT, -1) != -1) {
        /* Send LBRs to the to the CPU. */
        rv = bcm_rx_trap_type_get(unit, 0, is_upmep ? bcmRxTrapOamUpMEP : bcmRxTrapBfdOamDownMEP, &trap_code);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* send to the recycle port so that PRGE will answer LBMs with LBRs */
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapRecycleOamPacket, &trap_code);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    bcm_oam_endpoint_action_t_init(&action);
    BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

    BCM_OAM_OPCODE_CLEAR_ALL(action);
    BCM_OAM_OPCODE_SET(action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM);
    rv = set_classifier_action_general(unit, endpoint_id, &action, SOC_PPC_OAM_OPCODE_MAP_LBM, classifier_mep_entry->mep_type);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Function removes the loopback mechanism from the HW by
 * reseting the classifier and and clearing the TST/LB
 * registers. 
 *  
 * 
 * @author sinai (06/11/2013)
 * 
 * @param unit 
 * @param mep_id 
 * @param mep_type 
 * 
 * @return int
 */
int _remove_loopback(int unit, int mep_id,int mep_type, const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    bcm_oam_endpoint_action_t action;
    uint8 is_upmep;
    int trap_code = 0;
    int rv;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_oam_oamp_loopback_remove(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);

    /* Send LBRs to the to the CPU.*/
    rv = bcm_rx_trap_type_get(unit, 0, is_upmep ? bcmRxTrapOamUpMEP : bcmRxTrapBfdOamDownMEP, &trap_code);
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_oam_endpoint_action_t_init(&action);
    BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

    BCM_OAM_OPCODE_CLEAR_ALL(action);
    BCM_OAM_OPCODE_SET(action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR);
    rv = set_classifier_action_general(unit, mep_id, &action, SOC_PPC_OAM_OPCODE_MAP_LBR, mep_type );
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Internal function for handling loss and delay add on the 
 * client side. 
 *  
 * @author sinai (05/01/2015)
 * 
 * @param unit 
 * @param endpoint_id 
 * @param is_single_ended_or_1dm 
 * @param is_lm 
 * 
 * @return STATIC int 
 */
STATIC int loss_delay_add_client_side(int unit,int endpoint_id, int is_single_ended_or_1dm, int is_lm,
                                      SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {
    int rv;
    int is_single_ended = is_single_ended_or_1dm;
    bcm_oam_endpoint_action_t action;
    SOC_PPC_OAM_LIF_PROFILE_DATA  lif_profile;
    uint32 opcode = is_lm? is_single_ended? SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM : SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM :
            is_single_ended_or_1dm? SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM: SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_endpoint_action_t_init(&action);

    rv = _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(unit, endpoint_id, &lif_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    /* For down MEPs this will be encoded as a trap code. For Up MEPs as a system port. Either way the destination will be the remote OAMP. */
    action.destination = classifier_mep_entry->remote_gport;

    if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
    } else {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
    }

    if (!SHR_BITGET(lif_profile.mep_profile_data.counter_disable, opcode)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
    }
    if (!SHR_BITGET(lif_profile.mep_profile_data.meter_disable, opcode)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMeterEnable);
    }

    BCM_OAM_OPCODE_SET(action,opcode);

    if ( (is_single_ended_or_1dm && is_lm) || (!is_single_ended_or_1dm && !is_lm ) ) {
        BCM_OAM_OPCODE_SET(action,opcode -1); /* Response packet's OpCode is one less.*/
    }

    /* Validation */
    rv = _bcm_oam_endpoint_action_set_validity_check(unit,endpoint_id,&action);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_oam_endpoint_action_set_internal(unit,endpoint_id, &action,is_lm && !is_single_ended_or_1dm,2/*ignore*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/** 
 *  loss_add / loss_delete for non-accelerated MEPs is done by
 *  setting the classifier only. Counter will be stamped on CCMs
 *  (piggyback). CCMs will be generated by an externel dedicated
 *  device.
 *  
 * @author avive (08/11/2015)
 * 
 * @param unit
 * @param id
 * @param action
 * 
 * @return int 
 */
int bcm_petra_oam_loss_update_non_accelerated_internal(int unit,
                                                       bcm_oam_endpoint_t id,
                                                       bcm_oam_endpoint_action_t *action,
                                                       int set_unset /* 1 = set, 0 = unset*/)
{

    BCMDNX_INIT_FUNC_DEFS;

    action->destination = BCM_GPORT_INVALID;
    action->destination2 = BCM_GPORT_INVALID;
    BCM_OAM_OPCODE_SET(*action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM);
    /* Validation */
    BCMDNX_IF_ERR_EXIT(_bcm_oam_endpoint_action_set_validity_check(unit,id,action));
    BCMDNX_IF_ERR_EXIT(bcm_petra_oam_endpoint_action_set_internal(unit,id,action,set_unset /* piggyback */,2/*sat-loopback*/));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets MAC in the OAMP gen mem for that endpoint and fills the passed
 * MaC.
 */
int _bcm_oam_custom_feature_oamp_flex_da_mac_get(int unit, int id, bcm_mac_t loss_delay_da) {
    uint32 da_mac_1 = 0, da_mac_2 = 0, da_mac_3 = 0;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* First 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET, &da_mac_1));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Second 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+1, &da_mac_2));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Third 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+2, &da_mac_3));
    BCMDNX_IF_ERR_EXIT(rv);

    loss_delay_da[1] = da_mac_1 & 0xff;
    loss_delay_da[0] = (da_mac_1 >>8) & 0xff;

    loss_delay_da[3] = da_mac_2 & 0xff;
    loss_delay_da[2] = (da_mac_2 >>8) & 0xff;

    loss_delay_da[5] = da_mac_3 & 0xff;
    loss_delay_da[4] = (da_mac_3 >>8) & 0xff;

exit:
    BCMDNX_FUNC_RETURN;
}
/* Sets MAC in the OAMP gen mem for that endpoint with the passed
 * MaC.
 */
int _bcm_oam_custom_feature_oamp_flex_da_mac_set(int unit, int id, uint8 *loss_delay_da) {
    uint32 da_mac_1 = 0, da_mac_2 = 0, da_mac_3 = 0;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    da_mac_1 |= ((loss_delay_da[1]) & 0xff);
    da_mac_1 |= (((loss_delay_da[0]) << 8) & 0xffff);

    da_mac_2 |= ((loss_delay_da[3]) & 0xff);
    da_mac_2 |= (((loss_delay_da[2]) << 8) & 0xffff);

    da_mac_3 |= ((loss_delay_da[5]) & 0xff);
    da_mac_3 |= (((loss_delay_da[4]) << 8) & 0xffff);

    /* First 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET, da_mac_1));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Second 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+1, da_mac_2));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Third 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+2, da_mac_3));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/* Compares existing MAC in the OAMP gen mem for that endpoint with the passed
 * MaC.
 * Returns BCM_E_NONE if both da matches.
 * Returns BCM_E_PARAM if da does not match.
 */
int _bcm_oam_custom_feature_oamp_flex_da_mac_compare(int unit, int id, bcm_mac_t loss_delay_da) {
    uint32 da_mac_1 = 0, da_mac_2 = 0, da_mac_3 = 0;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    /* First 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET, &da_mac_1));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Second 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+1, &da_mac_2));
    BCMDNX_IF_ERR_EXIT(rv);
    /* Third 2 bytes */ 
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_get, (unit, (id * _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP) + _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET+2, &da_mac_3));
    BCMDNX_IF_ERR_EXIT(rv);

    if ((loss_delay_da[1] != (da_mac_1 & 0xff)) || 
        (loss_delay_da[0] != ((da_mac_1 >> 8) & 0xff))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("First 2 octets of DA configured does not match with already configured")));
    }
    if ((loss_delay_da[3] != (da_mac_2 & 0xff)) || 
        (loss_delay_da[2] != ((da_mac_2 >> 8) & 0xff))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Middle 2 octets of DA configured does not match with already configured")));
    }
    if ((loss_delay_da[5] != (da_mac_3 & 0xff)) || 
        (loss_delay_da[4] != ((da_mac_3 >> 8) & 0xff))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Last 2 octets of DA configured does not match with already configured")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_loss_add(int unit, bcm_oam_loss_t *loss_ptr)
{
    uint8 oam_is_init,found, is_ok, tlv_used;
    uint32 correct_flags =0, lsb, msb,entry_exist;
    int dont_care;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile, old_eth1731_profile;
    bcm_oam_endpoint_action_t action;
    int rv;
    uint32 soc_sand_rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    uint8 report_mode;
    int trap_code = 0;
    uint32 y1711_lm_trap_code = 0;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    bcm_oam_endpoint_action_t_init(&action);

    if (!oam_is_init) {
        BCM_EXIT;
    }

   SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
   SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

   /*
    * For coverity
    */
   BCM_DPP_UNIT_CHECK(unit);



   if (SOC_IS_QUX(unit)) {
       uint8 qux_slm_lm_mode;

       qux_slm_lm_mode = SOC_DPP_CONFIG(unit)->pp.qux_slm_lm_mode;

       if ((qux_slm_lm_mode == 1) && ((loss_ptr->flags & BCM_OAM_LOSS_SLM) != BCM_OAM_LOSS_SLM)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM mode enabled. Normal LM is not allowed.")));
       }

       if ((qux_slm_lm_mode == 0) && ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LM mode enabled. SLM is not allowed.")));
       }
   }

   if (SOC_IS_JERICHO(unit)) {
       if((SOC_DPP_CONFIG(unit)->pp.user_header_size > 0) && (soc_property_get(unit, spn_OAM_MAID_11_BYTES_ENABLE, 0))
           && ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM can't work with 11B MAID if UDH enabled\n")));
       }
   }

   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
   BCMDNX_IF_ERR_EXIT(rv);
   if (!found) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
   }

   if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
        /* SLM and dual ended LM are not allowed 
         * if custom feature oamp flex da is set */
       if ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM) { 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM not allowed when custom feature oamp flexible DA is set.")));
       } else if ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) != BCM_OAM_LOSS_SINGLE_ENDED) { 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Dual ended LM not allowed when custom feature oamp flexible DA is set.")));
       }
   }

   if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id) != 0) {
      /* Redirect CCMs/LMMs to the remote OAMP and that's it.*/
      BCMDNX_IF_ERR_EXIT(loss_delay_add_client_side(unit, loss_ptr->id, (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) != 0, 1, &classifier_mep_entry));

      BCM_EXIT;
   }

   if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    report_mode = ((loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE) == BCM_OAM_LOSS_REPORT_MODE);

    /*First, verify that the parameters are correct and that the endpoint is indeed accelrated.*/
    correct_flags |= BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED | BCM_OAM_LOSS_UPDATE;
    if (SOC_IS_JERICHO(unit)) {
        correct_flags |= BCM_OAM_LOSS_REPORT_MODE|BCM_OAM_LOSS_SLM;
        if ((loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED)
            && (loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_OAM_LOSS_REPORT_MODE and BCM_OAM_LOSS_STATISTICS_EXTENDED cannot be used together.")));
        }
        if (SOC_IS_QAX(unit)) {
            correct_flags |= BCM_OAM_LOSS_WITH_ID;
        }

        if (classifier_mep_entry.is_rfc_6374_entry && (loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_OAM_LOSS_SLM is not supported over MplsLmDm.")));
        }
    }

    if (loss_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal flag combination set.")));
    }

    if (loss_ptr->loss_farend ||  loss_ptr->loss_nearend || loss_ptr->gport || loss_ptr->int_pri || loss_ptr->loss_threshold ||
        loss_ptr->pkt_dp_bitmap || loss_ptr->pkt_pri || loss_ptr->rx_farend || loss_ptr->rx_nearend ||
        loss_ptr->rx_oam_packets || loss_ptr->tx_farend || loss_ptr->tx_nearend || loss_ptr->tx_oam_packets ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal field set.")));
    }

    if ( (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)==0 && !BCM_MAC_IS_ZERO(loss_ptr->peer_da_mac_address)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When LOSS_SINGLE_ENDED flag is set peer_da_mac_address must be zero.")));
    }

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, loss_ptr->id, &entry_exist));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        entry_exist = SHR_BITGET(&entry_exist, SOC_PPC_OAM_MEP_TYPE_LM);
        if (entry_exist &&
            !(loss_ptr->flags & BCM_OAM_LOSS_UPDATE) &&
            !SOC_IS_QAX(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LM entry is found for Endpoint %d and update flag should be set to update loss object"), loss_ptr->id));
        }
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
       if (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated but BCM_OAM_LOSS_SINGLE_ENDED is set. Non-accelerated endpoints supports only piggyback LM (on CCM)"), loss_ptr->id));
       } else {
           /* MEP is Not accelerated but CCM packets might be generated by an external dedicated device like FPGA.
              In such cases, only the classifier settings to stamp counter are required */
           if((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM){
               BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM);
               BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR);
               BCM_OAM_ACTION_SET(action, bcmOAMActionSLMEnable);
           }
           rv = bcm_petra_oam_loss_update_non_accelerated_internal(unit,loss_ptr->id,&action,1);
           BCMDNX_IF_ERR_EXIT(rv);
       }
    } else {
        if ( SOC_IS_ARADPLUS_A0(unit) && ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED)==0) ) {
            rv = _bcm_petra_oam_is_tlv_used(unit, loss_ptr->id, &tlv_used);
            BCMDNX_IF_ERR_EXIT(rv);
            if (tlv_used) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                                    (_BSL_BCM_MSG("Endpoint %d has status TLV so it can't use piggy backed LM."), loss_ptr->id));
            }
        }

        if (SOC_IS_JERICHO(unit)) {
            /* Jericho validations - Check compatability between the report mode in the MEP profile and the requested report mode */
            SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth1731_profile);
            rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, loss_ptr->id, &dont_care, &old_eth1731_profile);
            BCMDNX_IF_ERR_EXIT(rv);
            /* profile.report_mode is 2 bit bitmap that indicate which report is active:
               00 - None    01 - LM     10 - DM     11-Both
               In order to check if LM reports are active we need to check the 1-st bit.
            */
            if ((!report_mode) && (old_eth1731_profile.report_mode & 1) && !SOC_IS_QAX(unit)) {
                /* This ep uses reports, but trying to add loss not in report mode */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses reports for LM/DM but loss added without reports flag"), loss_ptr->id));
            }

            if (report_mode && !(old_eth1731_profile.report_mode & 1)) {
                /* Requested report mode but the profile is either no-report-mode or don't-care.
                   Check if there's a shared entry for LM/DM or piggy-backed LM (which would mean No-Report mode) */
                if (old_eth1731_profile.piggy_back_lm) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses piggy-back LM but loss added with report flag"), loss_ptr->id));
                }

                /* Not Piggy-backed. Check for no-report LM/DM */
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, loss_ptr->id, &entry_exist));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (entry_exist != 0 && !SOC_IS_QAX(unit)) { /* LM/DM entry found. MEP uses no-report mode */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d uses shared entry for LM/DM but loss added with report flag"), loss_ptr->id));
                }
            }
            
        }

        /* Custom feature FLEX DA: 
         * If not update and delay was previously configured, 
         * ensure that the DA sent is same as stored DA if it is non-zero */
        if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit) && !(loss_ptr->flags & BCM_OAM_LOSS_UPDATE)) {
            if (old_eth1731_profile.dmm_rate != 0) {
                rv = _bcm_oam_custom_feature_oamp_flex_da_mac_compare(unit, loss_ptr->id, loss_ptr->peer_da_mac_address);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        /* QAX check. Down MEP injection + SLM does not work. Fixing this would require OAMP-PE changes*/
        if (SOC_IS_QAX(unit) && (loss_ptr->flags & BCM_OAM_LOSS_SLM)  && 
               (_BCM_OAM_DISSECT_IS_CLASSIFIER_DOWN_MEP_INJECTION(&classifier_mep_entry)))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SLM on Down MEP egress injection endpoint not supported.")));
        }

        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "passed initial tests")));


        /* First, the troika of profiles.*/
        lsb =  GET_LSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address);
        msb =  GET_MSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address);
        if ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) || (loss_ptr->flags & BCM_OAM_LOSS_SLM)){
            SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(loss_ptr->period, 0, eth1731_profile.lmm_rate);
        } else {
            eth1731_profile.lmm_rate = 0; /*the given rate is ignored, LMMs are not sent by the OAMP anyways.*/
            eth1731_profile.piggy_back_lm = 1;
        }

        if ((SOC_IS_JERICHO(unit)) && (loss_ptr->flags & BCM_OAM_LOSS_UPDATE)) {
            eth1731_profile.slm_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM);
            /* SLM to LM or LM to SLM modes cannot be changed using update.
             * Instead loss_delete should be called followed by loss_add.
             */
            if (old_eth1731_profile.slm_lm != eth1731_profile.slm_lm) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Change of loss mode from LM to SLM or vice versa is not allowed")));
            }
        } else {
            eth1731_profile.slm_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM);
        }

        if (SOC_IS_JERICHO(unit)) {
           eth1731_profile.ccm_cnt = old_eth1731_profile.ccm_cnt;
           if(report_mode) {
               /* profile.report_mode is 2 bit bitmap that indicate which report is active:
                  00 - None    01 - LM     10 - DM     11-Both
                
                  In Jericho the reports are global for the profile.
                  In QAX we could have separated reports for LM and DM in the same profile.
                   * In order to enable the LM reports set the 1-st bit in the profile.report_mode to "1"
               */
               eth1731_profile.report_mode = old_eth1731_profile.report_mode | (3 >> SOC_IS_QAX(unit));
               if (!entry_exist) {
                   /* We previously validated that the report mode in the profile was don't care (for this case)
                      beause no LM or DM was defined for the ep, so now the DMM rate needs to be set to 0 (since we're adding LM)*/
                   eth1731_profile.dmm_rate = 0;
               }
           } else if (((old_eth1731_profile.report_mode & 1) == 1) && SOC_IS_QAX(unit)) {
               /* QAX Only: The LM reports can be disabled when update the LM entry without BCM_OAM_LOSS_REPORT_MODE flag.
                  * In order to do that set the 1-st bit of profile.report_mode to "0".
                  * When disable the LM report, make sure that keep the DM reports unchanged.*/

               /* In order to remove LM report and keep the DM report unchanged, perform binary XNOR with 2'b10.
                  * In that way only the 1-st bit(LM) is changed and the 2-nd bit (DM) is unchange*/
               eth1731_profile.report_mode = ~(old_eth1731_profile.report_mode ^ 2);
           } else {
               /* In case that LM report is unchanged, keep the old profile.report_mode unchanged */
               eth1731_profile.report_mode =  old_eth1731_profile.report_mode;
           }
        }

        rv = _manage_DA_with_existing_entries_loss(unit, loss_ptr->id, &classifier_mep_entry,msb,lsb,
                                                   &eth1731_profile, (loss_ptr->flags &BCM_OAM_LOSS_UPDATE) == BCM_OAM_LOSS_UPDATE, &is_ok, loss_ptr->period);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!is_ok) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
        }


        if (SOC_IS_QAX(unit) && (loss_ptr->flags & BCM_OAM_LOSS_WITH_ID)) {
            if (loss_ptr->loss_id == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Explicit loss ID cannot be 0")));
            }
            mep_db_entry.lm_dm_id = loss_ptr->loss_id;
        }
        mep_db_entry.entry_type  = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT : SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM;
        mep_db_entry.mep_id      = loss_ptr->id;
        mep_db_entry.is_update   = ((loss_ptr->flags & BCM_OAM_LOSS_UPDATE) == BCM_OAM_LOSS_UPDATE);
        mep_db_entry.is_slm      = ((loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM);
        mep_db_entry.is_piggyback_down = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0) &&
                                            (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry));

        /* Add aditional entries to MEP_DB_ENTRY*/
        rv = _bcm_oam_lm_dm_add_shared(unit, &mep_db_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!classifier_mep_entry.is_rfc_6374_entry)
        {
            if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) { /*Have the classifier send LM packets to the OAMP, but only if the endpoint exists on this device.*/
                rv = _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(unit, &classifier_mep_entry, &trap_code);
                BCMDNX_IF_ERR_EXIT(rv);
                if((SOC_IS_QAX(unit))&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)
                  &&((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS)||(classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE))){
                   rv = OAM_ACCESS.trap_info.trap_ids.get(unit, ((classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ? SOC_PPC_OAM_TRAP_ID_Y1711_MPLS : SOC_PPC_OAM_TRAP_ID_Y1711_PWE), &y1711_lm_trap_code);
                   BCMDNX_IF_ERR_EXIT(rv);
                   BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit, (int)y1711_lm_trap_code, &trap_code));
                }
                BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);
                rv = _bcm_oam_set_classifier_action(unit, 1/*is_add*/, loss_ptr->id, action, 1 /*is_lm */, (loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0, classifier_mep_entry.mep_type,(loss_ptr->flags & BCM_OAM_LOSS_SLM) == BCM_OAM_LOSS_SLM /* is_slm */);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_loss_get(int unit, bcm_oam_loss_t * loss_ptr) {
    uint8 oam_is_init, found, has_dm;
    uint32 soc_sand_rv, entry;
    SOC_PPC_OAM_OAMP_LM_INFO_GET lm_info;
    int rv, dont_care;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;
    uint32 ccm_period_ms, ccm_period_micro_s;
    uint32 tx_rate, tx_rate_ref_counter;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    int num_entries;

    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(ccm_period_micro_s);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }
    
    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function may not be called for cleint side endpoint: Nothing to get")));
    }


    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, &classifier_mep_entry, &found);
            BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loss_ptr->id));
    }

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, loss_ptr->id, &dont_care, &eth1731_profile_data);
    BCMDNX_IF_ERR_EXIT(rv); 
    /* profile.report_mode is 2 bit bitmap that indicate which report is active:
       00 - None    01 - LM     10 - DM     11-Both
       In order to check if LM reports are active we need to check the 1-st bit.
    */
    if (SOC_IS_QAX(unit) || !(eth1731_profile_data.report_mode & 1)) {

        SOC_PPC_OAM_OAMP_LM_INFO_GET_clear(&lm_info);
        lm_info.entry_id = loss_ptr->id;
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lm_get,(unit, &lm_info));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* profile.report_mode is 2 bit bitmap that indicate which report is active:
       00 - None    01 - LM     10 - DM     11-Both
       In order to check if LM reports are active we need to check the 1st bit.
    */
    if (!(eth1731_profile_data.report_mode & 1)) {
        loss_ptr->rx_farend = lm_info.peer_rx;
        loss_ptr->tx_farend =  lm_info.peer_tx;
        loss_ptr->tx_nearend = lm_info.my_tx;
        loss_ptr->rx_nearend = lm_info.my_rx;
        if (lm_info.is_extended) {
            loss_ptr->loss_farend = (lm_info.my_tx == 0) ? 0 : (100 * lm_info.acc_lm_far) / lm_info.my_tx;
            loss_ptr->loss_nearend = (lm_info.peer_tx == 0) ? 0 : (100 * lm_info.acc_lm_near) / lm_info.peer_tx;
            loss_ptr->loss_nearend_max = lm_info.max_lm_near;
            loss_ptr->loss_nearend_acc = lm_info.acc_lm_near;
            loss_ptr->loss_farend_max = lm_info.max_lm_far;
            loss_ptr->loss_farend_acc = lm_info.acc_lm_far;
        }
    }

    /* Non statistics*/
    /* profile.report_mode is 2 bit bitmap that indicate which report is active:
       00 - None    01 - LM     10 - DM     11-Both
       In order to check if LM reports are active we need to check the 1st bit.
    */
    loss_ptr->flags = ((eth1731_profile_data.report_mode & 1) ? BCM_OAM_LOSS_REPORT_MODE : 0);
    if (!eth1731_profile_data.piggy_back_lm) {
        loss_ptr->flags |= BCM_OAM_LOSS_SINGLE_ENDED;

        if (_BCM_OAM_BCM_MEP_TYPE_IS_RFC6374(classifier_mep_entry.mep_type)) {
            rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                     eth1731_profile_data.lmm_rate,
                                                    &tx_rate_ref_counter);
            BCMDNX_IF_ERR_EXIT(rv);

            if (tx_rate_ref_counter > 0) {
                /* Extract tx rate from software for given index
                 * if reference count is greater than zero
                 */
                rv = BFD_ACCESS.tx_rate_value.get(unit,
                                                  eth1731_profile_data.lmm_rate,
                                                  &tx_rate);
                BCMDNX_IF_ERR_EXIT(rv);
                ++tx_rate; /* TX rate is one lower than what is calculated.*/
            } else {
                tx_rate = 0;
            }

            loss_ptr->period = SOC_SAND_DIV_ROUND((tx_rate*(SOC_IS_ARADPLUS_AND_BELOW(unit)?333:167)), 100); /* Tx Rate is in 3.33 (JER - 1.67) ms units */
        } else {
            /* Get the rate and maybe the DA address */
            SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, eth1731_profile_data.lmm_rate);
            loss_ptr->period = ccm_period_ms;
        }
        if (classifier_mep_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_ETH_OAM ) {
            if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
                rv = _bcm_oam_custom_feature_oamp_flex_da_mac_get(unit, loss_ptr->id, loss_ptr->peer_da_mac_address);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loss_ptr,loss_ptr->id );
            }
        }
        loss_ptr->flags |= BCM_OAM_LOSS_SLM * eth1731_profile_data.slm_lm;
    }

    if (SOC_IS_QAX(unit)) {
        if(lm_info.entry_id) {
            loss_ptr->flags |= BCM_OAM_LOSS_WITH_ID;
            loss_ptr->loss_id = lm_info.entry_id;
        }
        loss_ptr->flags |= (lm_info.is_extended ? BCM_OAM_LOSS_STATISTICS_EXTENDED : 0);
    }
    else if (!(eth1731_profile_data.report_mode & 1)) {
        soc_sand_rv = soc_ppd_oam_oamp_next_index_get(unit, loss_ptr->id, &entry, &has_dm);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        num_entries = entry - loss_ptr->id - 1;

        if ((num_entries == 2 && !has_dm) || num_entries == 3) {
            loss_ptr->flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_loss_delete(int unit, bcm_oam_loss_t *loss_ptr)
{
    uint8 oam_is_init,found;
    int rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry = NULL;
    bcm_oam_endpoint_action_t *action = NULL;
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
    
    BCMDNX_ALLOC(action, sizeof(*action), "bcm_petra_oam_loss_delete.action");
    if (action == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    bcm_oam_endpoint_action_t_init(action);

    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (!SOC_IS_ARADPLUS(unit) && !_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(loss_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    BCMDNX_ALLOC(classifier_mep_entry, sizeof(*classifier_mep_entry),"bcm_petra_oam_loss_delete.classifier_mep_entry");
    if (classifier_mep_entry == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loss_ptr->id, classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Endpoint %d not found."),loss_ptr->id));
    }

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(classifier_mep_entry)) {
        /*Do what we can in the shared function*/
        rv = _bcm_oam_lm_dm_remove_shared(unit,loss_ptr->id ,1/*is lm*/, &mep_db_entry, classifier_mep_entry );
        if (rv == BCM_E_NOT_FOUND) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Unset the classifier settings */
    rv = bcm_petra_oam_loss_update_non_accelerated_internal(unit,loss_ptr->id,action,0);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCM_FREE(action);
    BCM_FREE(classifier_mep_entry);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_delay_add(int unit, bcm_oam_delay_t *delay_ptr) {
    uint8 found, oam_is_init, is_ok;
    int rv;
    bcm_oam_endpoint_action_t action;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile, old_eth1731_profile;
    _bcm_oam_ma_name_t ma_name_struct;
    uint32 lsb = 0, msb = 0;
    int trap_code = 0;
    int dont_care;
    uint8 report_mode, demand_mode;
    uint16 data_pattern_lsb, data_pattern_msb;
    int gen_mem_index = 0;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&old_eth1731_profile);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
    }

    if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id) != 0) {
        /* Redirect CCMs/LMMs to the remote OAMP and that's it.*/
        BCMDNX_IF_ERR_EXIT(loss_delay_add_client_side(unit, delay_ptr->id, (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) != 0, 0, &classifier_mep_entry));

        BCM_EXIT;
    }

    rv = _bcm_oam_delay_validity_checks(unit, delay_ptr, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    report_mode = ((delay_ptr->flags & BCM_OAM_DELAY_REPORT_MODE) == BCM_OAM_DELAY_REPORT_MODE);
    /*The demand mode can't be entered on existing entry*/
    demand_mode = (delay_ptr->period == 0 && !(delay_ptr->flags & BCM_OAM_DELAY_UPDATE));

    if (!classifier_mep_entry.is_rfc_6374_entry) {
      /* First, the three profiles */
      lsb = GET_LSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address);
      msb = GET_MSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address);
      SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(delay_ptr->period, 0, eth1731_profile.dmm_rate);
    }

    if (SOC_IS_JERICHO(unit)) {
        uint32 entry_exist;

        rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, delay_ptr->id, &dont_care, &old_eth1731_profile);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, delay_ptr->id, &entry_exist));
        BCM_SAND_IF_ERR_EXIT(rv);
        eth1731_profile.ccm_cnt = old_eth1731_profile.ccm_cnt;

        if (report_mode && !demand_mode) {
            /* profile.report_mode is 2 bit bitmap that indicate which report is active:
               00 - None    01 - LM     10 - DM     11-Both

               In Jericho the reports are global for the profile(once report_mode is set, it's always = 11)
               In QAX we could have separated reports for LM and DM in the same profile.
                * In order to enable the DM reports set the 2-nd bit in the profile.report_mode to "1"
            */
            eth1731_profile.report_mode = old_eth1731_profile.report_mode | (3 << SOC_IS_QAX(unit));
            if (!entry_exist) {
                /* We previously validated that the report mode in the profile was don't care (for this case)
                   beause no LM or DM was defined for the ep, so now the LMM rate needs to be set to 0 (since we're adding DM)*/
                eth1731_profile.lmm_rate = 0;
            }
        } else if (((old_eth1731_profile.report_mode & 2) == 2) && SOC_IS_QAX(unit)) {
            /* QAX Only: The LM reports can be disabled when update the LM entry without BCM_OAM_LOSS_REPORT_MODE flag.
               * In order to do that set the 1-st bit of profile.report_mode to "0".
               * When disable the LM report, make sure that keep the DM reports unchanged.*/

            /* In order to remove the DM report and keep the LM report unchanged, perform binary XNOR with 2'b01.
               * In that way only the 2-nd bit(DM) is changed and the 1-st bit (LM) is unchange*/
            eth1731_profile.report_mode = ~(old_eth1731_profile.report_mode ^ 1);
        } else {
            /* In case that DM report is unchanged, keep the old profile.report_mode unchanged */
            eth1731_profile.report_mode =  old_eth1731_profile.report_mode;
        }
        /* Custom feature FLEX DA: 
         * If not update and loss was previously configured, 
         * ensure that the DA sent is same as stored DA if it is non-zero */
        if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit) && !(delay_ptr->flags & BCM_OAM_DELAY_UPDATE)) {
            if (old_eth1731_profile.lmm_rate != 0) {
                rv = _bcm_oam_custom_feature_oamp_flex_da_mac_compare(unit,delay_ptr->id, delay_ptr->peer_da_mac_address);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        eth1731_profile.dmm_measure_one_way = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY)/ BCM_OAM_DELAY_ONE_WAY;
    }
    
    rv = _manage_DA_with_existing_entries_delay(unit,delay_ptr->id, &classifier_mep_entry,msb,lsb,
                                               &eth1731_profile, (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) == BCM_OAM_DELAY_UPDATE, &is_ok, delay_ptr->period);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ok) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
    }

    /*filling the mep_db_entry and having the shared function do its thing*/
    mep_db_entry.entry_type = SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM;
    mep_db_entry.mep_id = delay_ptr->id;

    mep_db_entry.is_1DM = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) / BCM_OAM_DELAY_ONE_WAY; /*1 if the flag is set.*/

    mep_db_entry.is_update = ((delay_ptr->flags & BCM_OAM_DELAY_UPDATE) == BCM_OAM_DELAY_UPDATE);
    if (SOC_IS_QAX(unit) && (delay_ptr->flags & BCM_OAM_DELAY_WITH_ID)) {
        if (delay_ptr->delay_id == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Explicit delay ID cannot be 0")));
        }
        mep_db_entry.lm_dm_id = delay_ptr->delay_id;
    }

    if ((delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData) && _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        data_pattern_lsb = delay_ptr->tlvs[0].four_byte_repeatable_pattern & 0xffff;
        data_pattern_msb = (delay_ptr->tlvs[0].four_byte_repeatable_pattern >> 16) & 0xffff;
        /*
          * For jumbo tlv entry, mep_id should be 16*N
          *  TLV DATA index start from 12K, the first 12K reserved by 48b MA_ID, (512*24)
          */
        gen_mem_index = (delay_ptr->id /16) + (12*1024);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index, data_pattern_msb));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index+ 256, data_pattern_lsb));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index+ 512, delay_ptr->tlvs[0].tlv_length));
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry.is_jumbo_dm = 1;
        rv = _bcm_dpp_oam_ma_db_get(unit, classifier_mep_entry.ma_index, &ma_name_struct, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry.name_type = ma_name_struct.name_type;
    }
    rv = _bcm_oam_lm_dm_add_shared(unit,&mep_db_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    if (eth1731_profile.dmm_rate) {
        uint8 using_1dm =0;
        /* the DMM rate in the eth1731 profile will be positive if and only if there was found to be an proactive service as well for this MEP.*/
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,delay_ptr->id, &using_1dm));
        BCMDNX_IF_ERR_EXIT(rv);

        if (mep_db_entry.is_1DM != using_1dm) {
                LOG_WARN(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Proactive session with differing 1DM/DMM status exists."
                                      "BCM_OAM_DELAY_ONE_WAY flag ingnored \n")));
        }
    }

    if (demand_mode) {
        /* on demand DM */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_dm_trigger_set, (unit,delay_ptr->id));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!classifier_mep_entry.is_rfc_6374_entry) {
      if (classifier_mep_entry.lif != _BCM_OAM_INVALID_LIF) {
          /*Have the classifier send DM packets to the OAMP.*/
          rv = _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(unit, &classifier_mep_entry, &trap_code);
          BCMDNX_IF_ERR_EXIT(rv);

          bcm_oam_endpoint_action_t_init(&action);
          BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

          rv = _bcm_oam_set_classifier_action(unit, 1/*is_add*/, delay_ptr->id, action, 0 /*is_lm */, delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY, classifier_mep_entry.mep_type, 0/* is_slm */);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }

    if (demand_mode) {
        /* Update the classifier mep entry (The flags may have changed)*/
        uint8 only_on_demand = (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) !=0;
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
        }

        if (only_on_demand) {
            classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
        } else {
            classifier_mep_entry.flags &= ~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE;
        }

        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, delay_ptr->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_delay_get(int unit, bcm_oam_delay_t *delay_ptr) {
    uint8 oam_is_init, found, is_1dm=0;
    uint32 soc_sand_rv;
    SOC_PPC_OAM_OAMP_DM_INFO_GET dm_info;
    int rv, dont_care;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 ccm_period_ms, ccm_period_micro_s;
    uint32 tx_rate, tx_rate_ref_counter;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile_data;
    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(ccm_period_micro_s);

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    BCM_DPP_UNIT_CHECK(unit); /* for Coverity */

    if (_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function may not be called for cleint side endpoint (nothing to get).")));
    }

    if (!SOC_IS_ARADPLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }

    SOC_PPC_OAM_OAMP_DM_INFO_GET_clear(&dm_info);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), delay_ptr->id));
    }

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, delay_ptr->id, &dont_care, &eth1731_profile_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* profile.report_mode is 2 bit bitmap that indicate which report is active:
       00 - None    01 - LM     10 - DM     11-Both
       In order to check if DM reports are active we need to check if the 2nd bit is set.
    */
    delay_ptr->flags = (eth1731_profile_data.report_mode & 2) ? BCM_OAM_DELAY_REPORT_MODE : 0;

    dm_info.entry_id = delay_ptr->id;
    dm_info.Y1731_profile_index = dont_care;
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_dm_get,(unit, &dm_info, &is_1dm ));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (SOC_IS_QAX(unit) && dm_info.entry_id) {
       delay_ptr->delay_id = dm_info.entry_id;
       delay_ptr->flags |= BCM_OAM_DELAY_WITH_ID;
    }

    if (!(eth1731_profile_data.report_mode & 2)) {

        COMPILER_64_SET(delay_ptr->delay.seconds,0,  dm_info.last_delay_second );
        delay_ptr->delay.nanoseconds = dm_info.last_delay_sub_seconds;

        COMPILER_64_SET(delay_ptr->delay_max.seconds,0,  dm_info.max_delay_second );
        delay_ptr->delay_max.nanoseconds = dm_info.max_delay_sub_seconds;

        COMPILER_64_SET(delay_ptr->delay_min.seconds,0,  dm_info.min_delay_second );
        delay_ptr->delay_min.nanoseconds = dm_info.min_delay_sub_seconds;

    }

    /* Now the period and maybe the DA address */
    if (classifier_mep_entry.is_rfc_6374_entry) {
        rv = BFD_ACCESS.tx_rate_ref_counter.get(unit,
                                                eth1731_profile_data.dmm_rate,
                                                &tx_rate_ref_counter);
        BCMDNX_IF_ERR_EXIT(rv);

        if (tx_rate_ref_counter > 0) {
            /* Extract tx rate from software for given index
             * if reference count is greater than zero
             */
            rv = BFD_ACCESS.tx_rate_value.get(unit,
                                              eth1731_profile_data.dmm_rate,
                                              &tx_rate);
            BCMDNX_IF_ERR_EXIT(rv);
            ++tx_rate; /* TX rate is one lower than what is calculated */
        } else {
            tx_rate = 0;
        }

        delay_ptr->period = SOC_SAND_DIV_ROUND((tx_rate*(SOC_IS_ARADPLUS_AND_BELOW(unit)?333:167)), 100); /* Tx Rate is in 3.33 (JER - 1.67) ms units */
    } else {
        SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, eth1731_profile_data.dmm_rate);
        delay_ptr->period = ccm_period_ms;
    }

    if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
            rv = _bcm_oam_custom_feature_oamp_flex_da_mac_get(unit, delay_ptr->id, delay_ptr->peer_da_mac_address);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(delay_ptr,delay_ptr->id);
        }
    }

    if (classifier_mep_entry.is_rfc_6374_entry) {
        delay_ptr->timestamp_format = ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0) ? bcmOAMTimestampFormatIEEE1588v1 : bcmOAMTimestampFormatNTP;
    } else {
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_1dm_get, (unit,delay_ptr->id, &is_1dm));
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_1dm) {
            delay_ptr->flags |= BCM_OAM_DELAY_ONE_WAY;
        }

        delay_ptr->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_delay_delete(int unit, bcm_oam_delay_t *delay_ptr) {
    uint8 oam_is_init,found;
    int rv;
    int gen_mem_index = 0;
    _bcm_oam_ma_name_t ma_name_struct;
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (!SOC_IS_ARADPLUS(unit)  && !_BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(delay_ptr->id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+.")));
    }
    
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, delay_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Endpoint %d not found."),delay_ptr->id));
    }

    if (!classifier_mep_entry.is_rfc_6374_entry) {
      rv = _bcm_dpp_oam_ma_db_get(unit, classifier_mep_entry.ma_index, &ma_name_struct, &found);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    /*Clear the Gen mem if tlv present*/
    if ((delay_ptr->tlvs[0].tlv_type == bcmOamTlvTypeData) && _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        gen_mem_index = (delay_ptr->id /16) + (12*1024);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index, 0));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index+ 256, 0));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_gen_mem_set, (unit, gen_mem_index+ 512,0));
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry.is_jumbo_dm = 1;
        mep_db_entry.name_type = ma_name_struct.name_type;
    }
    /*Do what we can in the shared function*/
    rv = _bcm_oam_lm_dm_remove_shared(unit,delay_ptr->id ,0/*is lm*/, &mep_db_entry, &classifier_mep_entry);
    if (rv == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/** Accelerated loop-back */

/* Adds TLV extension to an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_tlv_set(int unit, bcm_oam_loopback_t *loopback_ptr,
                                         ARAD_PP_OAM_LOOPBACK_INFO *soc_loopback_info) {

    bcm_oam_tlv_t *tlv_ptr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    /* Validation */
    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(soc_loopback_info);
    tlv_ptr = &(loopback_ptr->tlvs[0]);
    /* Currently supporting only 1 TST-TLV */
    if (loopback_ptr->num_tlvs != 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Supporting only 1 TLV on loopback messages")));
    }
    if (tlv_ptr->tlv_length & (~(0xffff))) {
        /* Negative or too long */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Unsupported TLV Length."
                                          " Supporting only lengths 0-%u"),0xffff));
    }
    if ((tlv_ptr->tlv_length < 4)
        && ((tlv_ptr->tlv_type == bcmOamTlvTypeTestNullWithCRC)
            ||(tlv_ptr->tlv_type == bcmOamTlvTypeTestPrbsWithCRC))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("To use CRC in Test TLV,"
                                          " TLV-Length must be greater than 4")));
    }
    /* Validation over */

    /* Parse type */
    switch (tlv_ptr->tlv_type) {
    case bcmOamTlvTypeTestNullWithoutCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_NULL_NO_CRC;
        break;
    case bcmOamTlvTypeTestNullWithCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_NULL_WITH_CRC;
        break;
    case bcmOamTlvTypeTestPrbsWithoutCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_PRBS_NO_CRC;
        break;
    case bcmOamTlvTypeTestPrbsWithCRC:
        soc_loopback_info->tst_tlv_type = ARAD_PP_OAM_DATA_TST_TLV_PRBS_WITH_CRC;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG("Unsupported TLV type for loopback messages"
                                          " (Only Test type supported)")));
    }

    /* Parse length */
    soc_loopback_info->tst_tlv_len = tlv_ptr->tlv_length;

    /* Mark the TLV existance */
    soc_loopback_info->has_tst_tlv = 1;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add Packet generation for a loopback object */
int _bcm_oam_loopback_acc_packet_generation_add(int unit, bcm_oam_loopback_t *loopback_ptr,
                                                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                                uint32 *new_flags) {
    int rv = 0;
    uint32 soc_sand_rv;
    ARAD_PP_OAM_LOOPBACK_INFO soc_loopback_info = {0};

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(classifier_mep_entry);
    BCMDNX_NULL_CHECK(new_flags);

    /* Set SOC layer info */
    soc_loopback_info.endpoint_id = loopback_ptr->id;
    soc_loopback_info.tx_period = loopback_ptr->period;
    soc_loopback_info.is_period_in_pps =
        ((loopback_ptr->flags & BCM_OAM_LOOPBACK_PERIOD_IN_PPS) == BCM_OAM_LOOPBACK_PERIOD_IN_PPS);
    soc_loopback_info.is_added = (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) &&
        (classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK);
    /* DA Mac address */
    rv = _bcm_petra_mac_to_sand_mac(loopback_ptr->peer_da_mac_address, &soc_loopback_info.mac_address);
    BCMDNX_IF_ERR_EXIT(rv);
    /* Optional TLV */
    if (loopback_ptr->num_tlvs) {
        rv = _bcm_oam_loopback_acc_packet_tlv_set(unit, loopback_ptr, &soc_loopback_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        soc_loopback_info.has_tst_tlv = 0;
    }
    /* Optional priority (PCP/DEI) */
    soc_loopback_info.pkt_pri = loopback_ptr->pkt_pri;
    soc_loopback_info.inner_pkt_pri = loopback_ptr->inner_pkt_pri;
    soc_loopback_info.int_pri = loopback_ptr->int_pri;
    soc_loopback_info.is_up_mep = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
    if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx){
        soc_loopback_info.sys_port = classifier_mep_entry->tx_gport;
        soc_loopback_info.local_port = classifier_mep_entry->port | classifier_mep_entry->port_core << 8; 
    }

    /*
       in the case of loopback update, the state of the HW is to be
       ignored only if the current endpoint previously set the LB
       mechanism. Otherwise, it may be the case that update has been
       called with the intention of adding the LB mechanism on top
       of the LBR mechanism. In this case, setting the LB registers
       is allowed only if no other endpoint has set the LB registers.
    */
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_loopback_set,(unit, &soc_loopback_info));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (soc_loopback_info.is_added == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Only one Loopback session allowed.")));
    }

    /* Flags to add to the classifier db - done outside this function */
    *new_flags |= (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK
                   | (soc_loopback_info.is_period_in_pps ?
                      SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_PPS : 0));

exit:
    BCMDNX_FUNC_RETURN;
}

int _insert_oam_data_tlv_head_for_tst_lb(uint8* buffer, int length, int byte_start_offset) {
    int offset = byte_start_offset;

    buffer[offset++] = 0;                /* Resever 4bytes for sequence Number */
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 3;                /* Data TLV Type */
    buffer[offset++] = (length>>8)&0xff; /* Data TLV Length */
    buffer[offset++] = (length&0xff);    /* Data TLV Length */

    return offset;
}

int _insert_oam_test_tlv_head_for_tst_lb(uint8* buffer, int length,int patten_type, int byte_start_offset)
{
    int offset = byte_start_offset;
    offset += 4; 
    buffer[offset++] = 32;               /* Test TLV Type */
    buffer[offset++] = (length>>8)&0xff; /* Test TLV Length */
    buffer[offset++] = (length&0xff);    /* Test TLV Length */
    
    /* Test TLV patten_type */
    switch(patten_type)
    {
       case bcmOamTlvTypeTestPrbsWithCRC:
          buffer[offset++] = 3;                
          break;
       case bcmOamTlvTypeTestPrbsWithoutCRC:
           buffer[offset++] = 2;               
           break;
       case bcmOamTlvTypeTestNullWithCRC:
          buffer[offset++] = 1;                
          break;
       case bcmOamTlvTypeTestNullWithoutCRC:
           buffer[offset++] = 0;
           break;
       default:
          break;
    }

    return offset;
}

int sat_loopback_add_rx(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 found = 0;
    int rv = BCM_E_NONE;
    int trap_id = 0;
    uint32 hw_trap_id = 0;
    uint32 lb_trap_id = 0;
    int trap_index = 0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    uint32 soc_sand_rv = 0;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    bcm_sat_ctf_packet_info_t packet_info;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
    int index = 0;
    int index_2 = 0;
    bcm_sat_ctf_identifier_t identifier;
    bcm_oam_endpoint_action_t action;
    int session_id = 0;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;
    uint32 lb_trap_used_cnt = 0;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_SAT_CTF_ENTRY_clear(&ctf_info);

    /*get entry of mep db*/
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, loopback_ptr->id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if ((!(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE))&&(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d has already add LB TX&RX objext."), loopback_ptr->id));
    }
    if((loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  lb objext don't exist ."), loopback_ptr->id));
    }


    bcm_oam_endpoint_action_t_init(&action);
    BCM_OAM_OPCODE_SET(action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR);
    /*Action should be:*/
    if (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
    } else if ((mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP)
               || (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                  (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
                  (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Currently Jericho LB just support in ETH / MPLS/PWE ")));
    }


    if ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0) { /* Down mep */

        /******  Down MEP Classification    *****/
        /* Set up an ingress trap to the SAT and set it as the action
           destination. */
        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SAT0_LB, &lb_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_id = (int)lb_trap_id;
        
        /*4 Call the API endpoint_action_set.The destination should be set set to the trap code above:*/
        BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
        /*After calling bcm_oam_endpoint_action_set with the above sequence, Down MEP packets will be redirected to the SAT.*/
        rv = bcm_petra_oam_endpoint_action_set(unit, loopback_ptr->id, &action);
        BCMDNX_IF_ERR_EXIT(rv);
        
    } else {
        /******  Up MEP Classification    ****   */
        uint64 arg64;
        /* Set the Eg.PRGE program variable to the correct trap code   */
        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SAT0_LB, &lb_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if (SOC_IS_QAX(unit)) {
            /* In QAX, lb trap id is already set with SAT port. use that. */
            trap_id = (int)lb_trap_id;
        } else {
            /* Send LBRs to the OAMP port (This also sets the corrct OAM-subtype
               to select the Eg.PRGE program that sets the trap-code. */
            rv = _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(unit, &classifier_mep_entry, &trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
        rv = bcm_petra_oam_endpoint_action_set_internal(unit, loopback_ptr->id, &action, 2/*piggyback(ignored)*/, 1/*sat_loopback*/);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_id = (int)lb_trap_id;
        
        soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM, &arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
        COMPILER_64_SET(arg64, lb_trap_id, COMPILER_64_LO(arg64));
        soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM, arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    /* Both UP & Down MEP need to re-get  classifier entry, since action set will update it */
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);


    /******  CTF configuration.    *****/
    if (loopback_ptr->flags & BCM_OAM_LOOPBACK_WITH_CTF_ID) {  /* if flag is update, user should give the cft_id*/
        ctf_id = loopback_ptr->ctf_id;
        /* Create a CTF with ID*/
        rv = bcm_common_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) { /* flag is update, don't need to create a new Ctf*/
        ctf_id = loopback_ptr->ctf_id;
        rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), ctf_id));
        }
    } else {
        /* Create a CTF */
        rv = bcm_common_sat_ctf_create(unit, 0, &ctf_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    loopback_ptr->ctf_id = ctf_id; /* save gtf_id */
    ctf_info.flag = 1; /*1:lb enalbe / 2:tst enable */
    ctf_info.mepid = loopback_ptr->id;

    if (!(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)) {
        if(!_BCM_RX_EXPOSE_HW_ID(unit)){
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_trap_ppd_to_hw(unit, trap_id, &hw_trap_id));
        }
        else{
            hw_trap_id = trap_id;
        }
        /* LB trap ID used by all the MEP. */
        rv = sw_state_access[unit].dpp.soc.arad.pp.oam.lb_trap_used_cnt.get(unit, &lb_trap_used_cnt);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if(lb_trap_used_cnt == 0){
            rv = bcm_common_sat_ctf_trap_add(unit, hw_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        lb_trap_used_cnt ++;
        rv = sw_state_access[unit].dpp.soc.arad.pp.oam.lb_trap_used_cnt.set(unit, lb_trap_used_cnt);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_data = loopback_ptr->id;

        /* map OAM-ID + trap code to CTF ID */
        /* map trap data to session ID */
        if(!classifier_mep_entry.session_map_flag){ /* lb&tst share one session_id in the same mep*/
           for (session_id = 0; session_id < 16; session_id++) {
               if(TRUE ==bcm_common_sat_session_inuse(unit, session_id)){
                   continue;  /*find next valid session*/
               }
               rv = bcm_common_sat_ctf_trap_data_to_session_map(unit, trap_data, 0, session_id); /*OAM-ID is endpoint ID and session ID*/
               BCMDNX_IF_ERR_EXIT(rv);
               break;
           }
        }
        classifier_mep_entry.session_map_flag++;

        rv = bcm_common_sat_trap_idx_get(unit, hw_trap_id, &trap_index);
        BCMDNX_IF_ERR_EXIT(rv);

        /*(2)  Map identification(four content) to specific flow-id */
        bcm_sat_ctf_identifier_t_init(&identifier);
        identifier.session_id = session_id;  /*oam ID*/
        identifier.trap_id = trap_index;
        for (dp = 0; dp <= dp_max; dp++) {
            for (tc = 0; tc <= tc_max; tc++) {
                identifier.color = dp;
                identifier.tc = tc;
                rv = bcm_common_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        /*save identifier for tst*/
        ctf_info.identifier_session_id = session_id;  /*oam ID*/
        ctf_info.identifier_trap_id = trap_index;
    }
    /* Set the recieved packet information */
    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderY1731;

    /*  Seq no offset set 4, should be the same with GTF send packet */
    packet_info.offsets.seq_number_offset = 4;

    /* LB just have one of Data/Test TLV*/
    for (index = 0; index < BCM_OAM_MAX_NUM_TLVS_FOR_LBM; index++) {
        if (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData) {
            packet_info.offsets.payload_offset = 11;
           }
          else {
            packet_info.offsets.payload_offset = 12; /* for test tlv*/
        }

        /* Test TLV */
        if ((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)) {
            packet_info.payload.payload_type = bcmSatPayloadPRBS;
          }
          else if((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC)
                   || (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithoutCRC)) {
            packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
            for (index_2 = 0; index_2 < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index_2++) {
                packet_info.payload.payload_pattern[index_2] = 0;
            }
        }

        /* Data TLV */
        if (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData) {
            packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
            packet_info.payload.payload_pattern[0] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            packet_info.payload.payload_pattern[1] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            packet_info.payload.payload_pattern[2] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF00) >> 8;
            packet_info.payload.payload_pattern[3] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF);
            packet_info.payload.payload_pattern[4] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            packet_info.payload.payload_pattern[5] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            packet_info.payload.payload_pattern[6] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF00) >> 8;
            packet_info.payload.payload_pattern[7] = (loopback_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF);
        }

        ctf_info.tlv.tlv_length=loopback_ptr->tlvs[index].tlv_length;

        /*End TLV and CRC*/
        if ((loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (loopback_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC)) {
            /*CRC should cover from the type field to last byte according to protocol*/
            packet_info.offsets.crc_byte_offset = 8; 
            packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
            }
           else
            {
            packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }

    rv = bcm_common_sat_ctf_packet_config(unit, ctf_id, &packet_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) {
        rv = _bcm_dpp_oam_sat_ctf_info_db_update(unit, ctf_id, &ctf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = _bcm_dpp_oam_sat_ctf_info_db_insert(unit, ctf_id, &ctf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int sat_loopback_add_tx(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 found = 0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_sat_gtf_packet_config_t *config = NULL;
    bcm_pkt_t *sat_header = NULL;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
    JER_PP_OAM_LOOPBACK_TST_INFO lb_info;
    uint32 user_header_0_size = 0, user_header_1_size = 0,udh_size = 0,min_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;
    uint32  gtf_seq_number_offset;
    int offset = 0;
    uint8 is_test_tlv = 0;
    int is_mem_free = 0;
    int i = 0, priority = 0;
    int i2 = 0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 }, { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 } };

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_loopback_tst_info_init, (unit,&lb_info));
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }

    if (loopback_ptr->flags & BCM_OAM_LOOPBACK_WITH_GTF_ID) {  /* user request a specify gtf_id*/
        gtf_id = loopback_ptr->gtf_id;
        rv = bcm_common_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID, &gtf_id);
        BCMDNX_IF_ERR_EXIT(rv);
   }
   else if(loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE){
        gtf_id = loopback_ptr->gtf_id;
        rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, gtf_id, &gtf_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), gtf_id));
        }
   }
   else {
        rv = bcm_common_sat_gtf_create(unit, 0, &gtf_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /*loopback_ptr->gtf_id = gtf_id; */
    loopback_ptr->gtf_id = gtf_id; /* save gtf_id */
    gtf_info.flag = 1; /*1:lb enalbe / 2:tst enable */
    gtf_info.mepid = loopback_ptr->id;

    /* Configure header info need to free this malloc memery*/
    rv = bcm_pkt_alloc(unit, 127, 0, &sat_header);
    if (rv == BCM_E_NONE) {
        is_mem_free = 1;
    } else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    lb_info.flags= JER_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM;
    lb_info.endpoint_id =  loopback_ptr->id;
    lb_info.int_pri = loopback_ptr->int_pri;
    lb_info.pkt_pri = loopback_ptr->pkt_pri;
    lb_info.inner_pkt_pri = loopback_ptr->inner_pkt_pri;
    lb_info.is_up_mep = ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);
    if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx){
        lb_info.sys_port = classifier_mep_entry.tx_gport;
        lb_info.local_port = classifier_mep_entry.port | classifier_mep_entry.port_core << 8; 
    }

    /* DA Mac address */
    for(i=0; i<6;i++){
        lb_info.mac_address.address[i] = loopback_ptr->peer_da_mac_address[i];
    }
    /*build tst header*/
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lb_tst_header_set, ( unit, &lb_info,sat_header->pkt_data->data,&offset));
    BCMDNX_IF_ERR_EXIT(rv);

    gtf_seq_number_offset = offset;

    /* Configure packet */
    BCMDNX_ALLOC(config, sizeof(*config), "sat_loopback_add_tx.config");
    if (config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    bcm_sat_gtf_packet_config_t_init(config);

    config->header_info.pkt_data = sat_header->pkt_data;
    config->header_info.pkt_data->len = gtf_seq_number_offset + 4;
    config->header_info.blk_count = 1;
    /*Configure header type*/
    config->sat_header_type = bcmSatHeaderY1731;

    /* add sequence no*/
    config->offsets.seq_number_offset = gtf_seq_number_offset; /* offset =internal header+OAM header */
    
    /*Loop for support 2 TLV in the future , currently LB just have one TLV, either Data TLV or Test TLV*/
    /*BCM_OAM_MAX_NUM_TLVS_FOR_LBM*/
    for (i = 0; i < loopback_ptr->num_tlvs; i++) {
        /* insert data TLV header*/

        if (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData) {
            if(loopback_ptr->tlvs[i].tlv_length < 34){
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" TLV length should be longer than 34 (Since packet should be longer than 64 bytes) .")));
            }
            offset = _insert_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, loopback_ptr->tlvs[i].tlv_length, offset);
        }
        /* insert test TLV header*/
        else if ((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
                 || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
                 || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
                 || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC)) {
            if(loopback_ptr->tlvs[i].tlv_length < 34){
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" TLV length should be longer than 34 ( Since packet should be longer than 64 bytes) .")));
            }
            is_test_tlv = 1;
            offset = _insert_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, loopback_ptr->tlvs[i].tlv_length, loopback_ptr->tlvs[i].tlv_type, offset);
        }
        else if(loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeNone){
            if(loopback_ptr->tlvs[i].tlv_length != 0){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("  TLV length should be 0 for tlvNone \n")));
            }
            continue;
        }

        sat_header->pkt_data->len = offset;  /*whole packet( from internal header -to- TLV length and pattern type)- before TLV patten*/

        /* configure Test TLV payload type*/
        if ((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)) {
            config->payload.payload_type = bcmSatPayloadPRBS;
            /* pattern will generate by system,don't need to fill it */
          }
          else if((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
              ||(loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
          {
            config->payload.payload_type = bcmSatPayloadConstant4Bytes; /*default value?*/
            for (i2 = 0; i2 < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i2++) {
                config->payload.payload_pattern[i2] = 0;
            }
        }

        /* configure Data TLV payload type and pattern */
        if ((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData)) {
            config->payload.payload_type = bcmSatPayloadConstant4Bytes; /*tst_ptr->tlv.four_byte_repeatable_pattern*/
            config->payload.payload_pattern[0] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            config->payload.payload_pattern[1] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            config->payload.payload_pattern[2] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF00) >> 8;
            config->payload.payload_pattern[3] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF);
            config->payload.payload_pattern[4] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            config->payload.payload_pattern[5] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            config->payload.payload_pattern[6] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF00) >> 8;
            config->payload.payload_pattern[7] = (loopback_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF);
        }
        gtf_info.tlv.tlv_type = loopback_ptr->tlvs[i].tlv_type;
        gtf_info.tlv.tlv_length = loopback_ptr->tlvs[i].tlv_length;
    }

    gtf_info.lb_tlv_num = loopback_ptr->num_tlvs;
    /* Store priority fields */
    gtf_info.int_pri = loopback_ptr->int_pri;
    gtf_info.pkt_pri = loopback_ptr->pkt_pri;
    gtf_info.inner_pkt_pri = loopback_ptr->inner_pkt_pri;
    i = 0;

    /* First read the size of the UDH header that may or may not need to be inserted*/
    rv = arad_pmf_db_fes_user_header_sizes_get(
        unit,
        &user_header_0_size,
        &user_header_1_size,
        &user_header_egress_pmf_offset_0_dummy,
        &user_header_egress_pmf_offset_1_dummy
        );
    BCMDNX_IF_ERR_EXIT(rv);
    udh_size = (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry))? 0 : (user_header_1_size / 8 + user_header_0_size / 8);
    /*config Whole packet length, all the packets have the same length*/
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
        for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i2++) {
            /* length shouldn't include tlv pattern for test tlv, so reduce 1 byte */
            config->packet_edit[priority].packet_length[i] =  offset - (is_test_tlv ? 1 : 0) +  loopback_ptr->tlvs[i].tlv_length +1 ; /*add end tlv*/
        }
        min_size = (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry))? 62: 66;
        if(config->packet_edit[priority].packet_length[i] < (min_size + udh_size)){
            config->packet_edit[priority].packet_length[i] = min_size + udh_size;  /* 66total(62 for Up mep) - 6sys_header(2 for Up mep) - udh + 4crc = 64 eth packet*/
        }
        for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i2++) {
            config->packet_edit[priority].packet_length_pattern[i2] =  gtf_packet_length_pattern[0][i2];
        }

        config->packet_edit[priority].pattern_length = 1;
        config->packet_edit[priority].number_of_ctfs = 1;
        /*add end tlv and CRC*/
        if ((loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (loopback_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)) {
            /*CRC should be from the type field to last byte according to protocol*/
            config->offsets.crc_byte_offset = gtf_seq_number_offset + 4; 
            config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        } else {
            config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) { /* Up mep */
        /* Get core from LIF (For upmeps, passive_side_lif is the global-lif) */
        rv = _bcm_dpp_oam_core_from_lif_get(unit, classifier_mep_entry.passive_side_lif, &(config->packet_context_id));
        BCMDNX_IF_ERR_EXIT(rv);
    }


    rv = bcm_common_sat_gtf_packet_config(unit, gtf_id, config);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if ctf and gtf set successfully, need to set LB flag*/
    classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER;

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    if (loopback_ptr->flags & BCM_OAM_LOOPBACK_UPDATE) {
        rv = _bcm_dpp_oam_sat_gtf_info_db_update(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = _bcm_dpp_oam_sat_gtf_info_db_insert(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCM_FREE(config);
    if (1 == is_mem_free) {
        bcm_pkt_free(unit, sat_header);
    }

    BCMDNX_FUNC_RETURN;
}

/* Add an OAM loopback object */
int bcm_petra_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 found;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 is_ok, is_period_in_kpps;
    bcm_oam_endpoint_action_t action;
    int rv = BCM_E_NONE;
    uint32 msb_to_oui;
    uint32 lsb_to_nic;
    uint32 new_flags = 0, remove_flags = 0;
    int trap_code = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Validation of input + classifier retrieval */
    rv = _bcm_oam_loopback_validity_check(unit,loopback_ptr,&classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Jericho SAT usage mode */
    if(SOC_IS_JERICHO(unit)) {
        if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
        }
         /* Step 1, create sat_CTF for RX  */
        rv = sat_loopback_add_rx(unit, loopback_ptr);
        BCMDNX_IF_ERR_EXIT(rv);
         
        /* Step 2, create sat_GTF for TX  */
       rv = sat_loopback_add_tx(unit, loopback_ptr);
       BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* LBR responses to LBMs*/
        if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { /* DA address profiles*/
            msb_to_oui = GET_MSB_FROM_MAC_ADDR(loopback_ptr->peer_da_mac_address);
            lsb_to_nic = GET_LSB_FROM_MAC_ADDR(loopback_ptr->peer_da_mac_address);

            rv = _manage_DA_with_existing_entries_loopback(unit, loopback_ptr->id, &classifier_mep_entry, msb_to_oui, lsb_to_nic, &is_ok, 0 /** Relevant only for RFC6374 */);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!is_ok) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MAC adddress may not be changed")));
            }
        }

        /* Have the classifier redirect LBMs to the OAMP.*/
        rv = _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(unit, &classifier_mep_entry, &trap_code);
        BCMDNX_IF_ERR_EXIT(rv);

        bcm_oam_endpoint_action_t_init(&action);
        BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

        BCM_OAM_OPCODE_CLEAR_ALL(action);
        BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM);
        if (loopback_ptr->period != 0) {
            /* OAMP will soon start transmitting LBMs. Make sure LBRs are caught and rediercted to OAMP.*/
            /* Have the classifier redirect LBRs to the OAMP.*/
            BCM_OAM_OPCODE_SET(action,SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR);
        }
        rv = set_classifier_action_general(unit, loopback_ptr->id, &action, SOC_PPC_OAM_OPCODE_MAP_LBM, classifier_mep_entry.mep_type );
        BCMDNX_IF_ERR_EXIT(rv);

        is_period_in_kpps = ((loopback_ptr->flags & BCM_OAM_LOOPBACK_PERIOD_IN_PPS) == BCM_OAM_LOOPBACK_PERIOD_IN_PPS);

        if (loopback_ptr->period != 0) { /* Use LBM functionality as well*/
            /* Store priority fields */
            sw_state_access[unit].dpp.bcm.oam.loopback_object_int_pri.set(unit,loopback_ptr->int_pri);
            sw_state_access[unit].dpp.bcm.oam.loopback_object_pkt_pri.set(unit,loopback_ptr->pkt_pri);
            sw_state_access[unit].dpp.bcm.oam.loopback_object_inner_pkt_pri.set(unit,loopback_ptr->inner_pkt_pri);

            rv = _bcm_oam_loopback_acc_packet_generation_add(unit, loopback_ptr, &classifier_mep_entry, &new_flags);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
                /* This is a special case of UPDATE where the loopback mechanism is to be removed while the LBR mechanism is
                   to remain.*/
                rv = _remove_loopback(unit,loopback_ptr->id, classifier_mep_entry.mep_type, &classifier_mep_entry);
                BCMDNX_IF_ERR_EXIT(rv);
                remove_flags |= (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK
                                 | (is_period_in_kpps ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_PPS : 0));
            }
        }
        new_flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY;

        /* The classifier may have been updated in action_set(). get the latest version and then update.*/
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
        }

        classifier_mep_entry.flags =  classifier_mep_entry.flags | (new_flags & ~remove_flags); /*stupid compiler doesn't know what associative means*/
        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets TLV extension from an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_tlv_get(int unit, bcm_oam_loopback_t *loopback_ptr,
                                         ARAD_PP_OAM_LOOPBACK_INFO *soc_loopback_info) {

    bcm_oam_tlv_t *tlv_ptr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(soc_loopback_info);
    tlv_ptr = &(loopback_ptr->tlvs[0]);

    /* Retrieve Length */
    tlv_ptr->tlv_length = soc_loopback_info->tst_tlv_len;

    /* Retrieve type */
    switch (soc_loopback_info->tst_tlv_type) {
    case ARAD_PP_OAM_DATA_TST_TLV_NULL_NO_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestNullWithoutCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_NULL_WITH_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestNullWithCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_PRBS_NO_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestPrbsWithoutCRC;
        break;
    case ARAD_PP_OAM_DATA_TST_TLV_PRBS_WITH_CRC:
        tlv_ptr->tlv_type = bcmOamTlvTypeTestPrbsWithCRC;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                            (_BSL_BCM_MSG("Shouldn't get here. Something is wrong")));
    }

    /*Set num_tlvs to the only supported value */
    loopback_ptr->num_tlvs = 1;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets the configuration and status of an accelerated loopback object */
int _bcm_oam_loopback_acc_packet_generation_get(int unit, bcm_oam_loopback_t *loopback_ptr,
                                                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry) {

    int rv = 0;
    uint32 soc_sand_rv = SOC_E_NONE;
    ARAD_PP_OAM_LOOPBACK_INFO soc_loopback_info = {0};

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(loopback_ptr);
    BCMDNX_NULL_CHECK(classifier_mep_entry);

    soc_loopback_info.is_period_in_pps =
        ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_PPS) != 0);

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_loopback_get,(unit, &soc_loopback_info));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    loopback_ptr->flags = soc_loopback_info.is_period_in_pps ? BCM_OAM_LOOPBACK_PERIOD_IN_PPS : 0;
    loopback_ptr->period = soc_loopback_info.tx_period;
    loopback_ptr->rx_count = soc_loopback_info.rx_packet_count;
    loopback_ptr->tx_count = soc_loopback_info.tx_packet_count;
    loopback_ptr->drop_count = soc_loopback_info.discard_count;

    if (soc_loopback_info.has_tst_tlv) {
        rv = _bcm_oam_loopback_acc_packet_tlv_get(unit, loopback_ptr, &soc_loopback_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int sat_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr) 
{
     uint8 oam_is_init=0;
     uint8 found=0;
     int priority = 0;
     int rv = BCM_E_NONE;
     SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
     SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
     SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
     bcm_sat_ctf_stat_t stat;
     uint64 gtf_tx_count[2];
     uint64 total_tx;
     int index;
     BCMDNX_INIT_FUNC_DEFS;
     _BCM_OAM_ENABLED_GET(oam_is_init);
     
     if (!oam_is_init) {
          BCM_EXIT;
     }
     if (!SOC_IS_JERICHO(unit)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
     }
    
    bcm_sat_ctf_stat_t_init(& stat);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add LB TX&RX objext."), loopback_ptr->id));
    } 
    
    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
        rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (ctf_info.flag == 1)){
            if(ctf_info.mepid== loopback_ptr->id){/* find it*/
                 loopback_ptr->ctf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    rv = bcm_common_sat_ctf_stat_get (unit, loopback_ptr->ctf_id , 0, &stat);
    BCMDNX_IF_ERR_EXIT(rv);
    COMPILER_64_ZERO(total_tx);
    COMPILER_64_TO_32_LO(loopback_ptr->rx_count,stat.received_packet_cnt);
    COMPILER_64_TO_32_LO(loopback_ptr->invalid_tlv_count,stat.err_packet_cnt);
    COMPILER_64_TO_32_LO(loopback_ptr->out_of_sequence,stat.out_of_order_packet_cnt);
    
    /*get gtf info from gtf DB*/
    for(index =0; index < 8; index++){
        rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (gtf_info.flag == 1)){
            if(gtf_info.mepid== loopback_ptr->id){/* find it*/
                 loopback_ptr->gtf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    for (priority = 0; priority < 2; priority++) {
         rv = bcm_common_sat_gtf_stat_get (unit,
                  loopback_ptr->gtf_id,
                  priority,
                  0,
                  bcmSatGtfStatPacketCount,
                  &gtf_tx_count[priority]);
         BCMDNX_IF_ERR_EXIT(rv);
         COMPILER_64_ADD_64(total_tx,gtf_tx_count[priority]);
    }

    COMPILER_64_TO_32_LO(loopback_ptr->tx_count,total_tx);

    loopback_ptr->drop_count = loopback_ptr->tx_count - loopback_ptr->rx_count; 
    
    loopback_ptr->int_pri = gtf_info.int_pri;
    loopback_ptr->pkt_pri = gtf_info.pkt_pri;
    loopback_ptr->inner_pkt_pri = gtf_info.inner_pkt_pri;
    
    loopback_ptr->num_tlvs = gtf_info.lb_tlv_num;
    if(loopback_ptr->num_tlvs == 1){
        loopback_ptr->tlvs[0].tlv_length = gtf_info.tlv.tlv_length;
        loopback_ptr->tlvs[0].tlv_type= gtf_info.tlv.tlv_type;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM loopback object */
int bcm_petra_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_ptr) {
    int rv=0;
    uint8 oam_is_init,found;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;

    if ((!SOC_IS_ARADPLUS_A0(unit))&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+ and Jericho.")));
    }
    _BCM_OAM_ENABLED_GET(oam_is_init);
        if (!oam_is_init) {
        BCM_EXIT;
    }
    if(SOC_IS_JERICHO(unit)) {
        if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
        }
        rv = sat_loopback_get(unit, loopback_ptr);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
        }

        if ( (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) ==0 ){
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Loopback session not associated with endpoint %d."), loopback_ptr->id));
        }

        if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
            rv = _bcm_oam_loopback_acc_packet_generation_get(unit, loopback_ptr, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv);

            sw_state_access[unit].dpp.bcm.oam.loopback_object_int_pri.get(unit,&loopback_ptr->int_pri);
            sw_state_access[unit].dpp.bcm.oam.loopback_object_pkt_pri.get(unit,&loopback_ptr->pkt_pri);
            sw_state_access[unit].dpp.bcm.oam.loopback_object_inner_pkt_pri.get(unit,&loopback_ptr->inner_pkt_pri);
        }

        /* Now the DA address and the period (when applicable)*/
        if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
            _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(loopback_ptr, loopback_ptr->id);
        }

        BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int sat_loopback_delete(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_ctf_t ctf_id = 0;
    bcm_sat_gtf_t gtf_id = 0;
    int trap_id=0;
    uint32 hw_trap_id = 0;
    int trap_data=0;
    uint32 lb_trap_id=0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
    bcm_sat_ctf_identifier_t identifier;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max =3;
    int index = 0;
    uint32 lb_trap_used_cnt = 0;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add LB TX&RX objext."), loopback_ptr->id));
    }

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SAT0_LB, &lb_trap_id);
    trap_id= (int)lb_trap_id;
    BCMDNX_IF_ERR_EXIT(rv);


    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 1)){
           if(ctf_info.mepid== loopback_ptr->id){/* find it*/
                ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf id %d information can't be found."), ctf_id));
    }
    /*get identifier for lb*/
    identifier.session_id = ctf_info.identifier_session_id;  /*oam ID*/
    identifier.trap_id = ctf_info.identifier_trap_id; 
    /* clean up */
    /* Set the CTF flow as invalid  */
    for(dp = 0; dp <= dp_max; dp++)
    {
        for(tc = 0; tc <= tc_max; tc++)
          {
            identifier.color = dp; 
            identifier.tc =tc;   
            rv = bcm_common_sat_ctf_identifier_unmap(unit, &identifier);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    if(!_BCM_RX_EXPOSE_HW_ID(unit)){
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_trap_ppd_to_hw(unit, trap_id, &hw_trap_id));
    }
    else{
        hw_trap_id = trap_id;
    }
    rv = sw_state_access[unit].dpp.soc.arad.pp.oam.lb_trap_used_cnt.get(unit, &lb_trap_used_cnt);
    BCMDNX_IF_ERR_EXIT(rv);
    lb_trap_used_cnt --;
    /* no other mep using it*/
    if(lb_trap_used_cnt == 0){
        rv = bcm_common_sat_ctf_trap_remove(unit, hw_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = sw_state_access[unit].dpp.soc.arad.pp.oam.lb_trap_used_cnt.set(unit, lb_trap_used_cnt);
    BCMDNX_IF_ERR_EXIT(rv);


    classifier_mep_entry.session_map_flag--;
    if(classifier_mep_entry.session_map_flag == 0)/* no one using this map, so delete it*/
    {
        trap_data = loopback_ptr->id;   /*MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/
        rv = bcm_common_sat_ctf_trap_data_to_session_unmap(unit,trap_data,0);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Destroy the CTF flow */
    rv = bcm_common_sat_ctf_destroy(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv);

    for(index =0; index < 8; index++){
        rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
        if(!found){
              continue;
        }
        else if(found && (gtf_info.flag == 1)){
            if(gtf_info.mepid== loopback_ptr->id){/* find it*/
                 gtf_id = index;
                 break;
            }
            else{/* other mep enable lb*/
                continue;
            }
        }
        else{
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* Destroy the GTF Object */
    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /*if ctf and gtf delet successfully, need to clear LB flag*/
    classifier_mep_entry.flags &= (~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER);

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

    /*delete ctf db */
    rv =  _bcm_dpp_oam_sat_ctf_info_db_delete(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv); 

    /*delete gtf db */
    rv = _bcm_dpp_oam_sat_gtf_info_db_delete(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete an OAM loopback object */
int bcm_petra_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_ptr) {
    uint8 oam_is_init,found;
    int rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint8 remove_flags =0;
    BCMDNX_INIT_FUNC_DEFS; 

    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    if ((!SOC_IS_ARADPLUS_A0(unit))&&(!SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Arad+ and Jericho.")));
    }
    
    if(SOC_IS_JERICHO(unit)){
        if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
        }
        rv = sat_loopback_delete(unit, loopback_ptr);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
        }

        if ((classifier_mep_entry.flags &SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint with id %d does not have loopback session."), loopback_ptr->id));
        }

        rv = _remove_loopback_reply(unit, loopback_ptr->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK) {
            rv = _remove_loopback(unit, loopback_ptr->id, classifier_mep_entry.mep_type, &classifier_mep_entry);
            BCMDNX_IF_ERR_EXIT(rv); 
            remove_flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK;
        }
        remove_flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY;

        /*get the latest version and then update.*/
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, loopback_ptr->id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), loopback_ptr->id));
        }
        classifier_mep_entry.flags &= ~remove_flags;

        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, loopback_ptr->id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_ais_add(int unit, bcm_oam_ais_t * ais_ptr)
{
    uint8 oam_is_init, found;
    int rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 correct_flags = 0, lsb, msb, soc_sand_rv;
    int used_ais_id[1];
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int profile_indx, is_allocated;
    int eth1731_prof_indx, eth1731_was_previously_allocated;
    SOC_PPC_OAM_MA_NAME name = { 0 };
    uint8 nic_profile, oui_profile;
    int ignored, local_port;
    uint32 alloc_flags;
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_gport_t local_gport;
    _bcm_dpp_gport_info_t gport_info;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    int tpid_index, core = 0;
    uint32   pp_port;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    if (!oam_is_init) {
        BCM_EXIT;
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }

    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, ais_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), ais_ptr->id));
    }

    if (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), ais_ptr->id));
    }

    if ((classifier_mep_entry.mep_type != SOC_PPC_OAM_MEP_TYPE_ETH_OAM) && (ais_ptr->flags & BCM_OAM_AIS_MULTICAST)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS may use Multicast flag only for ETH type.")));
    }

    rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    correct_flags |=  BCM_OAM_AIS_MULTICAST | BCM_OAM_AIS_WITH_AIS_ID | BCM_OAM_AIS_UPDATE;

    if (found && (ais_ptr->flags & BCM_OAM_AIS_UPDATE) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry exists. ais_add() may only be called with the UPDATE flag.")));
    }

    if (ais_ptr->flags & BCM_OAM_AIS_MULTICAST && !BCM_MAC_IS_ZERO(ais_ptr->peer_da_mac_address)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When MULTICAST flag is set peer_da_mac_address field may not be filled.")));
    }

    if (ais_ptr->level < classifier_mep_entry.md_level || ais_ptr->level > 7) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS level must be greateer than the endpoint level and less than 8.")));
    }

    if (ais_ptr->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1S &&  ais_ptr->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1M) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS period must be 1 frame per second (1000) or one frame per minute (60000).")));
    }


    if ((ais_ptr->flags & BCM_OAM_AIS_WITH_AIS_ID) && ais_ptr->ais_id > SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When _WITH_AIS_ID flag is set id must be less than %d."), SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)));
    }

    if ((ais_ptr->outer_tpid != 0 && ais_ptr->outer_vlan == 0)
        || (ais_ptr->inner_tpid != 0 && ais_ptr->inner_vlan == 0)
        || (ais_ptr->outer_tpid == 0 && ais_ptr->outer_vlan != 0)
        || (ais_ptr->inner_tpid == 0 && ais_ptr->inner_vlan != 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG("Error: AIS VLAN fields inconsistent.\n")));
    }    

    if (ais_ptr->flags & ~correct_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("llegal flag combination set.")));
    }
    /* prelimanery checks complete.*/

    if (ais_ptr->flags & BCM_OAM_AIS_UPDATE) {
        /* Destroy and reset, and nothing is lost.*/
        ais_ptr->flags &= ~BCM_OAM_AIS_UPDATE;
        rv = bcm_petra_oam_ais_delete(unit, ais_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_oam_ais_add(unit, ais_ptr);
        BCMDNX_IF_ERR_EXIT(rv);

        ais_ptr->flags |= BCM_OAM_AIS_UPDATE;
    }

    if (ais_ptr->flags & BCM_OAM_AIS_MULTICAST) {
        /* set mac address*/
        _BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(ais_ptr->peer_da_mac_address, ais_ptr->level);
    }

    /* Step 1: MEP profile and friends*/
    if (ais_ptr->period == BCM_OAM_ENDPOINT_CCM_PERIOD_1S) {
        /* period 1s uses opcode 0*/
        SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ais_ptr->period, 0, eth1731_profile.opcode_0_rate);
    } else {
        /* period 1m uses opcode 1*/
        SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ais_ptr->period, 0, eth1731_profile.opcode_1_rate);
    }

    /* Allocating NIC and OUI profile , only of MEP is of type ETH */
    if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {

        lsb = GET_LSB_FROM_MAC_ADDR(ais_ptr->peer_da_mac_address);
        msb = GET_MSB_FROM_MAC_ADDR(ais_ptr->peer_da_mac_address);

        rv = _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(unit, 0/* flags*/, &lsb, &is_allocated, &profile_indx);
        BCMDNX_IF_ERR_EXIT(rv);
        nic_profile = profile_indx;

        rv = _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(unit, 0/* flags*/, &msb, &is_allocated, &profile_indx);
        BCMDNX_IF_ERR_EXIT(rv);
        oui_profile =  profile_indx;

        eth1731_profile.lmm_da_oui_prof = profile_indx; /* oui_profile may be -1.*/
    } else {
    /* Else, skip allocation */
        nic_profile=0;
        oui_profile=0;
    }

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(unit, &eth1731_profile, 0 /*flags*/, &is_allocated, &eth1731_prof_indx);
    BCMDNX_IF_ERR_EXIT(rv);
    eth1731_was_previously_allocated = !is_allocated;

    /* Step 2: MEP DB*/
    /*allocate new entry*/
    if (ais_ptr->flags & BCM_OAM_AIS_WITH_AIS_ID) {
        rv = _bcm_dpp_bfd_mep_id_alloc(unit, SHR_RES_ALLOC_WITH_ID, (uint32 *)&(ais_ptr->ais_id));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        if (SOC_IS_QAX(unit)) {
            /* In QAX all mep IDs are allocated on the same pool. */
            rv = bcm_dpp_am_qax_oam_mep_id_alloc(unit, 0, 0, (uint32 *)&(ais_ptr->ais_id));
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            /* search look for a short mep ID and if that fails try a long one.*/
            rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, 0, (uint32 *)&(ais_ptr->ais_id));
            if (rv == BCM_E_RESOURCE) {
                /* No short mep ID, look for a long one*/
                rv = bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(unit, 0, (uint32 *)&(ais_ptr->ais_id));
                if (rv == BCM_E_RESOURCE) {
                    rv = bcm_dpp_am_oam_mep_id_long_alloc_48_maid(unit, 0, (uint32 *)&(ais_ptr->ais_id));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            /* search look for a short mep ID and if that fails try a long one.*/
            rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, 0, (uint32 *)&(ais_ptr->ais_id));
            if (rv == BCM_E_RESOURCE) {
                /* No short mep ID, look for a long one*/
                rv = bcm_dpp_am_oam_mep_id_long_alloc(unit, 0, (uint32 *)&(ais_ptr->ais_id));
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, ais_ptr->id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    mep_db_entry.is_ais_entry = 1;
    mep_db_entry.ccm_interval = 0;
    mep_db_entry.mdl = ais_ptr->level;
    mep_db_entry.icc_ndx = 0xf;

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) {
        /* Local gport */
        BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, ais_ptr->id, &endpoint_info));
        local_gport = endpoint_info.gport;

        rv = _bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, local_gport, &mep_db_entry.system_port);
        BCMDNX_IF_ERR_EXIT(rv);

        alloc_flags = SOC_IS_JERICHO(unit) ? 0 : SHR_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, alloc_flags,
                &mep_db_entry.system_port, &ignored, &local_port);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Index to system port */
        mep_db_entry.local_port = local_port;
        mep_db_entry.priority = ais_ptr->pkt_pri;
        mep_db_entry.is_upmep = 0;
    } else {
        rv = _bcm_dpp_oam_bfd_sys_port_to_tx_gport(unit, &local_gport, mep_db_entry.system_port);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_to_phy_port(unit, local_gport, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, (int*)&core));
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_JERICHO(unit) && core != 0) {
            pp_port |= (core << 8);
        }
        mep_db_entry.local_port = pp_port;
        mep_db_entry.priority = ais_ptr->pkt_pri;
        mep_db_entry.is_upmep = 1;
    }
    
    if (ais_ptr->outer_tpid == 0) { /* when single tag - outer tag fields are used */
        if (ais_ptr->inner_tpid != 0) { 
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Inner tpid can not be configured without an outer tpid.\n")));
        }    
    } else {
        soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, ais_ptr->outer_tpid, &tpid_vals, &tpid_index);
        BCMDNX_IF_ERR_EXIT(rv);
        mep_db_entry.outer_tpid = (uint8)tpid_index;
        mep_db_entry.outer_vid_dei_pcp =  ais_ptr->outer_vlan + (ais_ptr->outer_pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
        if (ais_ptr->inner_tpid != 0) { /* double tag */
            mep_db_entry.tags_num = 2; 
            rv = _bcm_petra_vlan_translate_match_tpid_value_to_index(unit, ais_ptr->inner_tpid, &tpid_vals, &tpid_index);
            BCMDNX_IF_ERR_EXIT(rv);
            mep_db_entry.inner_tpid = (uint8)tpid_index;
            mep_db_entry.inner_vid_dei_pcp = ais_ptr->inner_vlan + (ais_ptr->inner_pkt_pri << 12); /*pcp=3 dei=1 vlan=12*/
        } else { /* double tag */
            mep_db_entry.tags_num = 1; 
        }    
    }

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_set(unit, ais_ptr->ais_id, &mep_db_entry, 0 /* allocate ICC index - no*/, name /*name - affects only CCMs*/,0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Set troika of profiles in the HW*/
    soc_sand_rv = soc_ppd_oam_oamp_create_new_eth1731_profile(unit, ais_ptr->ais_id, eth1731_was_previously_allocated, eth1731_prof_indx, &eth1731_profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Set the NIC and OUI registers , only of MEP is of type ETH */
    if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
        soc_sand_rv = soc_ppd_oam_oamp_set_oui_nic_registers(unit, ais_ptr->ais_id, msb, lsb, oui_profile, nic_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Update the SW and we are done!*/
    rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_insert(unit, ais_ptr->id, ais_ptr->ais_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_ais_get(int unit, bcm_oam_ais_t * ais_ptr) {
    int rv;
    int used_ais_id[1];
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY eth1731_profile;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    int mep_prof;
    bcm_mac_t mcast_mac;
    uint8 found;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry does not exist.")));
    }

    ais_ptr->ais_id = *used_ais_id;

    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(&eth1731_profile);

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, ais_ptr->ais_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    ais_ptr->level = mep_db_entry.mdl;
    
    if (mep_db_entry.tags_num != 0) { /* not single tag */
        soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry.outer_tpid, &tpid_vals, &ais_ptr->outer_tpid);
        BCMDNX_IF_ERR_EXIT(rv);
        if (mep_db_entry.tags_num == 2) {
            rv = _bcm_petra_vlan_translate_match_tpid_index_to_value(unit, mep_db_entry.inner_tpid, &tpid_vals, &ais_ptr->inner_tpid);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    ais_ptr->inner_vlan = mep_db_entry.inner_vid_dei_pcp & 0xFFF;
    ais_ptr->inner_pkt_pri = mep_db_entry.inner_vid_dei_pcp >> 12 & 0xF;
    ais_ptr->outer_vlan = mep_db_entry.outer_vid_dei_pcp & 0xFFF;
    ais_ptr->outer_pkt_pri = mep_db_entry.outer_vid_dei_pcp >> 12 & 0xF;

    rv = _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit,ais_ptr->ais_id,&mep_prof,&eth1731_profile);
            BCMDNX_IF_ERR_EXIT(rv);

    if (eth1731_profile.opcode_0_rate) {
        ais_ptr->period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    } else {
        ais_ptr->period = BCM_OAM_ENDPOINT_CCM_PERIOD_1M;
    }

    if (_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit)) {
        rv = _bcm_oam_custom_feature_oamp_flex_da_mac_get(unit, ais_ptr->ais_id, ais_ptr->peer_da_mac_address);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        _BCM_OAM_MAC_ADDR_GET_FROM_NIC_AND_OUI(ais_ptr, ais_ptr->ais_id);
    }

    _BCM_OAM_SET_CCM_GROUP_DA_MAC_ADDRESS(mcast_mac, ais_ptr->level);
    if (sal_memcmp(mcast_mac, ais_ptr->peer_da_mac_address,6)==0) {
        sal_memset(ais_ptr->peer_da_mac_address, 0, 6);
        ais_ptr->flags = BCM_OAM_AIS_MULTICAST;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_ais_delete(int unit, bcm_oam_ais_t * ais_ptr)
{
    int rv;
    int used_ais_id[1];
    int mep_prof, is_last;
    uint8 found;
    uint32 soc_sand_rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, ais_ptr->id, used_ais_id, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("AIS entry does not exist.")));
    }

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, ais_ptr->id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Free NIC and OUI profiles, only of MEP is of type ETH */
    if (classifier_mep_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { /* DA address profiles*/
        FREE_NIC_AND_OUI_PROFILES(*used_ais_id);
        /* The eth1731 profile will remain as it is. */
    }

    rv =  _bcm_dpp_am_template_oam_eth1731_mep_profile_free(unit, *used_ais_id,  &mep_prof, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);
    CLEAR_ETH_1731_PROF_IF_IS_LAST(is_last, mep_prof, *used_ais_id);

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_delete(unit, *used_ais_id, NULL /*mep_db_entry*/, 0 /* dealloc icc index */, 0 /*is last*/);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, *used_ais_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(unit, ais_ptr->id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add an OAM TST RX object */
int bcm_petra_oam_tst_rx_add(int unit,bcm_oam_tst_rx_t *tst_ptr) {
    uint8 oam_is_init,found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv=0;
    int trap_id=0;
    uint32 hw_trap_id=0;
    uint32 tst_trap_id = 0;
    int trap_index = 0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    bcm_sat_ctf_packet_info_t packet_info;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_oam_endpoint_action_t action;
    int index =0;
    bcm_sat_ctf_identifier_t identifier;
    int session_id=0;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;
    uint32 tst_trap_used_cnt = 0;


    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
         BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
   
    /*get entry of mep db*/
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, tst_ptr->endpoint_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), tst_ptr->endpoint_id));
    }

    if ((!(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE))&&(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Endpoint %d has already added tst RX objext."), tst_ptr->endpoint_id));
    } 
    if((tst_ptr->flags & BCM_OAM_TST_RX_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER))){
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  tst RX objext don't exist ."), tst_ptr->endpoint_id));
    }


    /* Get SAT TST trap   */
    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SAT0_TST, &tst_trap_id);
    BCMDNX_IF_ERR_EXIT(rv);
    trap_id= (int)tst_trap_id;

    /* Prepare action to redirect TST to SAT (on OAMP)  */
    bcm_oam_endpoint_action_t_init(&action);
    /*Action should be:*/
    if(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM){
        BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
    }else if((mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP)
                ||(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) ||
                  (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
                (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TST just support in ETH / MPLS/PWE")));
    }
    /*Opcode should be TST */
    BCM_OAM_OPCODE_SET(action, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_TST);

    /* Set action destination and set the action */
    if((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == 0)/* Down mep */
    {
       /******  Down MEP Classification    *****/

       /* Call the API endpoint_action_set.The destination should be set set
          to the trap code above: */
       BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
       
       /*After calling bcm_oam_endpoint_action_set with the above sequence, Down MEP packets will be redirected to the SAT.*/
       rv = bcm_petra_oam_endpoint_action_set(unit,tst_ptr->endpoint_id,&action);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    else 
    {
        /******  Up MEP Classification    ****   */
        uint64 arg64;
        if (SOC_IS_QAX(unit)) {
            /* In QAX, tst trap id is already set with SAT port. use that. */
            trap_id = (int)tst_trap_id;
        } else {
            /* Send TSTs to the OAMP port (This also sets the corrct OAM-subtype
               to select the Eg.PRGE program that sets the trap-code. */
            rv = _bcm_petra_oam_oamp_trap_code_by_ep_type_and_direction(unit, &classifier_mep_entry, &trap_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 0); /*Taken from default values*/
        rv = bcm_petra_oam_endpoint_action_set_internal(unit, tst_ptr->endpoint_id, &action, 2/*piggyback(ignored)*/, 1/*sat_loopback*/);
        BCMDNX_IF_ERR_EXIT(rv);
        trap_id = (int)tst_trap_id;
        
        /* Set the Eg.PRGE program variable to the correct trap code   */
        soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM, &arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
        COMPILER_64_SET(arg64, tst_trap_id, COMPILER_64_LO(arg64));
        soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_OAM_CCM, arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Both UP & Down MEP need to re-get  classifier entry, since action set will update it */
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);


       /******  CTF configuration.    *****/
       if(tst_ptr->flags & BCM_OAM_TST_RX_WITH_ID)  /* if flag is update, user should give the cft_id*/
        {
           ctf_id = tst_ptr->ctf_id;
           /* Create a CTF */
           rv = bcm_common_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       else if(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE) /* flag is update, don't need to create a new Ctf*/
       {
           ctf_id = tst_ptr->ctf_id;
           rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, ctf_id, &ctf_info, &found);
           BCMDNX_IF_ERR_EXIT(rv);
           if (!found) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ctf_id %d not found."), ctf_id));
           }
       }
       else
       {
           /* Create a CTF */
           rv = bcm_common_sat_ctf_create(unit, 0, &ctf_id);
           BCMDNX_IF_ERR_EXIT(rv);
       }
       
       tst_ptr->ctf_id= ctf_id; /* save ctf_id */
       ctf_info.flag = 2;  /* 2:tst enable */
       ctf_info.mepid= tst_ptr->endpoint_id;
       
       if(!(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE)){        
         if(!_BCM_RX_EXPOSE_HW_ID(unit)){
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_trap_ppd_to_hw(unit, trap_id, &hw_trap_id));
         }
         else{
            hw_trap_id = trap_id;
         }


         rv = sw_state_access[unit].dpp.soc.arad.pp.oam.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt);
         BCMDNX_IF_ERR_EXIT(rv);
         
         if(tst_trap_used_cnt == 0){
             rv = bcm_common_sat_ctf_trap_add(unit, hw_trap_id);
             BCMDNX_IF_ERR_EXIT(rv);
         }
         tst_trap_used_cnt ++;


         rv = sw_state_access[unit].dpp.soc.arad.pp.oam.tst_trap_used_cnt.set(unit, tst_trap_used_cnt);
         BCMDNX_IF_ERR_EXIT(rv);

         trap_data = tst_ptr->endpoint_id;/*o   MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/

         /*(1) map trap qualifier and mask to session ID */
         if(!classifier_mep_entry.session_map_flag){ /* lb&tst share one session_id in the same mep*/
           for(session_id=0;session_id <16;session_id++){
              if(TRUE == bcm_common_sat_session_inuse(unit, session_id)){
                    continue;  /*find next valid session*/
              }
              rv = bcm_common_sat_ctf_trap_data_to_session_map(unit, trap_data, 0, session_id); /*OAM-ID is endpoint ID and session ID*/
              BCMDNX_IF_ERR_EXIT(rv);
              break;
           }
         }
         classifier_mep_entry.session_map_flag++;
         
         rv = bcm_common_sat_trap_idx_get(unit, hw_trap_id, &trap_index);
         BCMDNX_IF_ERR_EXIT(rv);
         
         /*(2)  Map identification(four content) to specific flow-id */
         
         bcm_sat_ctf_identifier_t_init(&identifier); 
         for(dp = 0; dp <= dp_max; dp++)
         {
             for(tc = 0; tc <= tc_max; tc++)
               {
                 identifier.session_id = session_id;  
                 identifier.trap_id = trap_index; 
                 identifier.color = dp; 
                 identifier.tc =tc;   
                 rv = bcm_common_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                 BCMDNX_IF_ERR_EXIT(rv);
             }
         }
         /*save identifier for tst*/
         ctf_info.identifier_session_id = session_id;  /*oam ID*/
         ctf_info.identifier_trap_id = trap_index; /* trap idx 1-3 */
         
       }       
       
       /* Set the recieved packet information */
       bcm_sat_ctf_packet_info_t_init(&packet_info);
       packet_info.sat_header_type = bcmSatHeaderY1731;
       
       /*  Seq no offset set 4, should be the same with GTF send packet */
       packet_info.offsets.seq_number_offset = 4;
       
       if(tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeData){
           packet_info.offsets.payload_offset = 11;
       }else {
           packet_info.offsets.payload_offset = 12; /* for test tlv*/
       }
       
       /* Test TLV */
       if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
           || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)){
             packet_info.payload.payload_type = bcmSatPayloadPRBS;
       }else if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
           ||(tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC)) {
             packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes; 
          for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++) {
               packet_info.payload.payload_pattern[index]=0; 
           }
       }
       
       /* Check End TLV & CRC */
       if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
         || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)){
           /*CRC should cover from the type field to last byte according to protocol*/
           packet_info.offsets.crc_byte_offset = 8; 
           packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
       }
       else
       {
           packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
       }
       rv = bcm_common_sat_ctf_packet_config(unit, ctf_id, &packet_info);
       BCMDNX_IF_ERR_EXIT(rv);

       if((tst_ptr->expected_tlv.tlv_type != bcmOamTlvTypeNone)&&(tst_ptr->expected_tlv.tlv_length < 34)){
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" TLV length should be larger than 34 ( Make sure that packet will be larger than 64 bytes) .")));
       }
       else if((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeNone)&&(tst_ptr->expected_tlv.tlv_length != 0)){
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("  TLV length should be 0 for tlvNone \n")));
       }
       
       /*if ctf set successfully, need to set TST RX flag*/
       classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER;
       ctf_info.tlv.tlv_type =tst_ptr->expected_tlv.tlv_type;
       ctf_info.tlv.tlv_length=tst_ptr->expected_tlv.tlv_length;
       
      /*update sw db mep_info with new mep */
      rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
      BCMDNX_IF_ERR_EXIT(rv); 

      if(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE) {
          rv = _bcm_dpp_oam_sat_ctf_info_db_update(unit, ctf_id, &ctf_info);
          BCMDNX_IF_ERR_EXIT(rv);
      }
      else{
          rv = _bcm_dpp_oam_sat_ctf_info_db_insert(unit, ctf_id, &ctf_info);
          BCMDNX_IF_ERR_EXIT(rv);
      }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM TST RX object */ 
int bcm_petra_oam_tst_rx_get(int unit,bcm_oam_tst_rx_t *tst_ptr) {
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_stat_t stat;
    int index = 0;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
         BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }
    
    bcm_sat_ctf_stat_t_init(& stat);

    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }

    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add tst rx objext."), tst_ptr->endpoint_id));
    } 

    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 2)){
           if(ctf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                tst_ptr->ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    tst_ptr->expected_tlv.tlv_type = ctf_info.tlv.tlv_type;
    tst_ptr->expected_tlv.tlv_length = ctf_info.tlv.tlv_length;

    rv = bcm_sat_ctf_stat_get (unit, tst_ptr->ctf_id , 0, &stat);
    BCMDNX_IF_ERR_EXIT(rv);

    tst_ptr->rx_count = stat.received_packet_cnt; 
    tst_ptr->invalid_tlv_count =  stat.err_packet_cnt;
    tst_ptr->out_of_sequence = stat.out_of_order_packet_cnt;    

exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete an OAM TST RX object */ 
int bcm_petra_oam_tst_rx_delete(int unit, bcm_oam_tst_rx_t *tst_ptr) {
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    int trap_id=0;
    uint32 hw_trap_id=0;
    int trap_data = 0;
    uint32 tst_trap_id = 0;
    bcm_sat_ctf_t ctf_id = 0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_identifier_t identifier;
    int tc = 0;
    int dp = 0; 
    int tc_max = 7;
    int dp_max =3;
    int index = 0;
    uint32 tst_trap_used_cnt = 0;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);

    if (!oam_is_init) {
         BCM_EXIT;
    }
    
    if (!SOC_IS_JERICHO(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't enable tst rx objext."), tst_ptr->endpoint_id));
    } 

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SAT0_TST, &tst_trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_id= (int)tst_trap_id;

    classifier_mep_entry.session_map_flag--;
    if(classifier_mep_entry.session_map_flag == 0)/* no one using this map*/
    {
        trap_data = tst_ptr->endpoint_id;   /*MEP_DB Index <= OAM_ID(14) <= PPH.Trap_Qualifier*/
        rv = bcm_common_sat_ctf_trap_data_to_session_unmap(unit,trap_data,0);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /*get ctf info from ctf DB*/
    for(index =0; index < 32; index++){
       rv = _bcm_dpp_oam_sat_ctf_info_db_get(unit, index, &ctf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (ctf_info.flag == 2)){
           if(ctf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                ctf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    
    /*get identifier for tst*/
    identifier.session_id = ctf_info.identifier_session_id;  /*oam ID*/
    identifier.trap_id = ctf_info.identifier_trap_id; 

    
    
    /* clean up */
    /* Set the CTF flow as invalid  */
    for(dp = 0; dp <= dp_max; dp++)
    {
        for(tc = 0; tc <= tc_max; tc++)
        {
            identifier.color = dp; 
            identifier.tc =tc;   
            rv = bcm_common_sat_ctf_identifier_unmap(unit, &identifier);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* Destroy the CTF flow */
    rv = bcm_common_sat_ctf_destroy(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if(!_BCM_RX_EXPOSE_HW_ID(unit)){
       BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_trap_ppd_to_hw(unit, trap_id, &hw_trap_id));
    }
    else{
       hw_trap_id = trap_id;
    }

    rv = sw_state_access[unit].dpp.soc.arad.pp.oam.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt);
    BCMDNX_IF_ERR_EXIT(rv);
    tst_trap_used_cnt --;
    /*no other MEP using it*/
    if(tst_trap_used_cnt == 0){
        rv = bcm_common_sat_ctf_trap_remove(unit, hw_trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = sw_state_access[unit].dpp.soc.arad.pp.oam.tst_trap_used_cnt.set(unit, tst_trap_used_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if ctf and ctf delet successfully, need to clear tst rx flag*/
    classifier_mep_entry.flags &= (~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER);
    
    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    /*delete ctf db */
    rv =  _bcm_dpp_oam_sat_ctf_info_db_delete(unit, ctf_id);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_add(int unit, bcm_oam_tst_tx_t *tst_ptr)
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_sat_gtf_packet_config_t *p_config = NULL;
    bcm_pkt_t *sat_header = NULL;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
    JER_PP_OAM_LOOPBACK_TST_INFO tst_info;
    uint32 user_header_0_size = 0, user_header_1_size = 0,udh_size = 0, min_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;
    uint32  gtf_seq_number_offset;
    int offset = 0;
    uint8 is_test_tlv = 0;
    int i = 0,priority=0;
    int is_mem_free = 0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}};
        
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
         BCM_EXIT;
    }
    
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }

    BCMDNX_ALLOC(p_config, sizeof(bcm_sat_gtf_packet_config_t),"bcm_petra_oam_tst_tx_add");
    if(p_config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

   /* Configure header info*/
   rv = bcm_pkt_alloc(unit, 127, 0, &sat_header); 
   if(rv == BCM_E_NONE){
       is_mem_free = 1;
   }else{
       BCMDNX_IF_ERR_EXIT(rv);
   }
   
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_loopback_tst_info_init, (unit,&tst_info));
   BCMDNX_IF_ERR_EXIT(rv);

   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
   BCMDNX_IF_ERR_EXIT(rv);
   
   if (!found) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
   }
   if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d is not accelerated."), tst_ptr->endpoint_id));
   }
   if ((!(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE))&&(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Endpoint %d has already add tst Tx objext."), tst_ptr->endpoint_id));
   } 
   if((tst_ptr->flags & BCM_OAM_TST_TX_UPDATE)&&(!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER))){
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d  tst Tx objext don't exist ."), tst_ptr->endpoint_id));
   }

   if(tst_ptr->flags & BCM_OAM_TST_TX_WITH_ID){  /* user request a specify gtf_id*/
       gtf_id = tst_ptr->gtf_id;
       rv = bcm_common_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID,&gtf_id);
       BCMDNX_IF_ERR_EXIT(rv);
   }
   else if(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE){/*flag is update, don't need to create a new gtf*/
       gtf_id = tst_ptr->gtf_id;
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, gtf_id, &gtf_info, &found);
       BCMDNX_IF_ERR_EXIT(rv);
       if (!found) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("gtf_id %d not found."), gtf_id));
       }
    }
   else {
        rv = bcm_common_sat_gtf_create(unit, 0,&gtf_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
   
    /*tst_ptr->gtf_id = gtf_id;*/
    tst_ptr->gtf_id = gtf_id; /* save gtf_id */
    
    gtf_info.flag = 2;  /* 2:tst enable */
    gtf_info.mepid= tst_ptr->endpoint_id;
    
    tst_info.flags= JER_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST;
    tst_info.endpoint_id =  tst_ptr->endpoint_id;
    /* DA Mac address */
    for(i=0; i<6;i++){
        tst_info.mac_address.address[i] = tst_ptr->peer_da_mac_address[i];
    }
    
    tst_info.is_up_mep = ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) != 0);

    if(SOC_DPP_CONFIG(unit)->pp.oam_ccm_rx_wo_tx){
        tst_info.sys_port = classifier_mep_entry.tx_gport;
        tst_info.local_port = classifier_mep_entry.port | classifier_mep_entry.port_core << 8; 
    }

    /*build tst header*/
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_lb_tst_header_set,( unit,&tst_info,sat_header->pkt_data->data,&offset));
    BCMDNX_IF_ERR_EXIT(rv);
    
    gtf_seq_number_offset = offset;
    /* Configure packet */
    bcm_sat_gtf_packet_config_t_init(p_config);
    
    p_config->header_info.pkt_data = sat_header->pkt_data;    
    p_config->header_info.pkt_data->len = gtf_seq_number_offset + 4;
    p_config->header_info.blk_count = 1;

    /*Configure header type*/
    p_config->sat_header_type = bcmSatHeaderY1731;

    /* add sequence no*/
    p_config->offsets.seq_number_offset = gtf_seq_number_offset; /* offset =internal header+OAM header */
    
    /* insert data TLV header*/
    if(tst_ptr->tlv.tlv_type == bcmOamTlvTypeData)
    {
        offset= _insert_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length, offset);
    }
    /* insert test TLV header*/
    else if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
    || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        is_test_tlv = 1;
        offset= _insert_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length,tst_ptr->tlv.tlv_type, offset);
    }

    if(tst_ptr->tlv.tlv_length < 34){
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(" TLV length should be longer than 34 ( Since packet should be longer than 64 bytes) .")));
    }

    sat_header->pkt_data->len = offset;  /*whole packet( from internal header -to- TLV length and pattern type)- before TLV patten*/

  
    /* configure Test TLV payload type*/
    if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        ||(tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
    {
        p_config->payload.payload_type = bcmSatPayloadPRBS; 
       /* pattern will generate by system,don't need to fill it */ 
    }
    else if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
        ||(tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        p_config->payload.payload_type = bcmSatPayloadConstant4Bytes; /*default value?*/
        for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++) {
        p_config->payload.payload_pattern[i]=0; 
        }
    }
    else /* TST can only have Test TLV */
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Endpoint %d ,the TLV bype of  tst Tx was wrong ."), tst_ptr->endpoint_id));
    }
    /* First read the size of the UDH header that may or may not need to be inserted*/
    rv = arad_pmf_db_fes_user_header_sizes_get(
        unit,
        &user_header_0_size,
        &user_header_1_size,
        &user_header_egress_pmf_offset_0_dummy,
        &user_header_egress_pmf_offset_1_dummy
        );
    BCMDNX_IF_ERR_EXIT(rv);
    udh_size = (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry))? 0 : (user_header_1_size / 8 + user_header_0_size / 8);
    
    /*config Whole packet length, all the packets have the same length*/
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++)
    {
        /*config Whole packet length, all the packets have the same length*/
        for (i = 0; i < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++) {
           /* length shouldn't include tlv pattern for test tlv, so reduce 1 byte */
           p_config->packet_edit[priority].packet_length[i] =  offset  - (is_test_tlv ? 1 : 0) +  tst_ptr->tlv.tlv_length +1 ; /*add end tlv*/
           min_size = (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry))? 62: 66;
           if(p_config->packet_edit[priority].packet_length[i] < (min_size + udh_size)){
               p_config->packet_edit[priority].packet_length[i] = min_size + udh_size; /* 66total(62 for Up mep) - 6sys_header(2 for Up mep) - udh + 4crc = 64 eth packet*/
           }
        }
        for (i = 0; i < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i++) {
           p_config->packet_edit[priority].packet_length_pattern[i] =  gtf_packet_length_pattern[0][i];;
        }
        p_config->packet_edit[priority].pattern_length = 1; /*1*/
        p_config->packet_edit[priority].number_of_ctfs = 1;
        /*add end tlv and CRC*/
        if((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)){
            /*CRC should be from the type field to last byte according to protocol*/
            p_config->offsets.crc_byte_offset = gtf_seq_number_offset + 4; 
            p_config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        }else{
            p_config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&classifier_mep_entry)) { /* Up mep */
        /* Get core from LIF (For upmeps, passive_side_lif is global-lif)  */
        rv = _bcm_dpp_oam_core_from_lif_get(unit, classifier_mep_entry.passive_side_lif, &(p_config->packet_context_id));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = bcm_common_sat_gtf_packet_config(unit, gtf_id, p_config);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /*if gtf set successfully, need to set TST TX flag*/
    classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER;
    gtf_info.tlv.tlv_type =tst_ptr->tlv.tlv_type;
    gtf_info.tlv.tlv_length=tst_ptr->tlv.tlv_length;
    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 
    
    if(tst_ptr->flags & BCM_OAM_TST_TX_UPDATE) {
        rv = _bcm_dpp_oam_sat_gtf_info_db_update(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else{
        rv = _bcm_dpp_oam_sat_gtf_info_db_insert(unit, gtf_id, &gtf_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    if(1 == is_mem_free){
        bcm_pkt_free(unit, sat_header); 
    }
    if (p_config != NULL) {
        BCM_FREE(p_config);
    }
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_get(int unit,bcm_oam_tst_tx_t *tst_ptr) 
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
    int index = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add tst tx objext."), tst_ptr->endpoint_id));
    } 
    
    /*get ctf info from ctf DB*/
    for(index =0; index < 8; index++){
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (gtf_info.flag == 2)){
           if(gtf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                tst_ptr->gtf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    tst_ptr->tlv.tlv_type = gtf_info.tlv.tlv_type;
    tst_ptr->tlv.tlv_length = gtf_info.tlv.tlv_length;

exit:
   BCMDNX_FUNC_RETURN;
}

/* Delete an OAM TST TX object */ 
int bcm_petra_oam_tst_tx_delete(int unit,bcm_oam_tst_tx_t *tst_ptr) 
{
    uint8 oam_is_init=0;
    uint8 found=0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info;
    int index = 0;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_JERICHO(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in Jericho.")));
    }
    if( !soc_property_get(unit, spn_SAT_ENABLE, 0)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Please enable SAT feature in advance")));
    }
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, tst_ptr->endpoint_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), tst_ptr->endpoint_id));
    }
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't enable tst Tx objext."), tst_ptr->endpoint_id));
    } 
    
    /*get gtf info from gtf DB*/
    for(index =0; index < 8; index++){
       rv = _bcm_dpp_oam_sat_gtf_info_db_get(unit, index, &gtf_info, &found);
       if(!found){
             continue;
       }
       else if(found && (gtf_info.flag == 2)){
           if(gtf_info.mepid== tst_ptr->endpoint_id){/* find it*/
                gtf_id = index;
                break;
           }
           else{/* other mep enable lb*/
               continue;
           }
       }
       else{
           BCMDNX_IF_ERR_EXIT(rv);
       }
    }
    /* Destroy the GTF Object */
    rv = bcm_sat_gtf_destroy(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /*if gtf delet successfully, need to clear tst tx flag*/
    classifier_mep_entry.flags &= (~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER);

    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, tst_ptr->endpoint_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv); 

   /*delete gtf db */
    rv = _bcm_dpp_oam_sat_gtf_info_db_delete(unit, gtf_id);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

/****************************************************
  OAM Control
 */

/**
* \brief
*  Set an OAM Control entry.
* \param [in] unit  -
*   Relevant unit.
* \param [in] key  - Type of Control being used:
*   key.type - Type of Control being used
*       bcmOamControlMplsLmDmOampActionPerQueryControlCode - Enable punt/ignore per control code for query packets.
*       bcmOamControlMplsLmDmOampActionPerResponseControlCode - Enable punt/ignore per control code for response packets.
*   key.index - control code.
* \param [in] arg -
*   0 - pass, 1 - ignore, 2 - Punt
* \retval
*   shr_error_param - Wrong type was selected.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_indexed_get
*/
int
_bcm_dpp_oam_control_indexed_set(
    int unit,
    bcm_oam_control_key_t key,
    uint64 arg)
{
    int rv = BCM_E_NONE;
    uint32 action;
    uint64 MAX_RANGE;

    BCMDNX_INIT_FUNC_DEFS;

    /**
    * 0 - accept the packet
    * 1 - Ignore the packet
    * 2 - Punt the packet
    */
    action = COMPILER_64_LO(arg);

    switch (key.type)
    {
        /** Set action to Query packets */
        case bcmOamControlMplsLmDmOampActionPerQueryControlCode:
        {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }

            COMPILER_64_SET(MAX_RANGE, 0, 0x02);
            if (COMPILER_64_GT(key.index, MAX_RANGE)) {
                /* Valid values are 0-2 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal query control code value\n")));
            }
            COMPILER_64_SET(MAX_RANGE, 0, 0x2);
            if (COMPILER_64_GT(action, MAX_RANGE)) {
                /* Valid values are 0-2 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Illegal arg value! Valid values are: 0 - accept the packet, 1 - Ignore the packet, 2 - Punt the packet, 3 - Set Punt and Ignore\n")));
            }

            /** Set the action */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_set, (unit, key.index, 0 /* Query */, action));
            BCMDNX_IF_ERR_EXIT(rv);
        }
            break;
        /** Set action to Response packets */
        case bcmOamControlMplsLmDmOampActionPerResponseControlCode:
        {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }

            COMPILER_64_SET(MAX_RANGE, 0, 0x1D);
            if (COMPILER_64_GT(key.index, MAX_RANGE)) {
                /* Valid values are 0-29 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal response control code value\n")));
            }
            COMPILER_64_SET(MAX_RANGE, 0, 0x2);
            if (COMPILER_64_GT(action, MAX_RANGE)) {
                /* Valid values are 0-2 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Illegal arg value! Valid values are: 0 - accept the packet, 1 - Ignore the packet, 2 - Punt the packet, 3 - Set Punt and Ignore\n")));
            }

             /** Set the action */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_set, (unit, key.index, 1 /* Response */, action));
            BCMDNX_IF_ERR_EXIT(rv);
        }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
* \brief
*  get an OAM Control entry.
* \param [in] unit  -
*   Relevant unit.
* \param [in] key  - Type of Control being used:
*   key.type - Type of Control being used
*       bcmOamControlMplsLmDmOampActionPerQueryControlCode - Enable punt/ignore per control code for query packets.
*       bcmOamControlMplsLmDmOampActionPerResponseControlCode - Enable punt/ignore per control code for response packets.
*   key.index - control code.
* \param [in] arg -
*   0 - pass, 1 - ignore, 2 - Punt
* \retval
*   shr_error_param - Wrong type was selected.
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_control_indexed_set
*/
int
_bcm_dpp_oam_control_indexed_get(
    int unit,
    bcm_oam_control_key_t key,
    uint64 *arg)
{
    int rv = BCM_E_NONE;
    uint32 action;
    uint64 MAX_RANGE;

    BCMDNX_INIT_FUNC_DEFS;

    switch (key.type)
    {
        /** Set action to Query packets */
        case bcmOamControlMplsLmDmOampActionPerQueryControlCode:
        {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }

            COMPILER_64_SET(MAX_RANGE, 0, 0x02);
            if (COMPILER_64_GT(key.index, MAX_RANGE)) {
                /* Valid values are 0-2 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal query control code value\n")));
            }

            /** Set the action */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_get, (unit, key.index, 1 /** Query */, &action));
            BCMDNX_IF_ERR_EXIT(rv);

            COMPILER_64_SET(*arg, 0, action);
        }
            break;
        /** Set action to Response packets */
        case bcmOamControlMplsLmDmOampActionPerResponseControlCode:
        {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }

            COMPILER_64_SET(MAX_RANGE, 0, 0x1D);
            if (COMPILER_64_GT(key.index, MAX_RANGE)) {
                /* Valid values are 0-2 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal response control code value\n")));
            }

            /** Set the action */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_get, (unit, key.index, 1 /** Response */, &action));
            BCMDNX_IF_ERR_EXIT(rv);

            COMPILER_64_SET(*arg, 0, action);
        }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_dpp_oam_control_get(
    int unit,
    bcm_oam_control_type_t type,
    uint64 *arg)
{
    int rv = BCM_E_NONE;
    uint32 fieldvalue32 = 0;
    uint32 dummy;

    BCMDNX_INIT_FUNC_DEFS;

    switch (type) {
    case bcmOamControlReportMode:
        {
            if (SOC_IS_JERICHO(unit)) {
                SOC_PPC_OAM_REPORT_MODE mode;
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_report_mode_get, (unit, &mode));
                BCMDNX_IF_ERR_EXIT(rv);

                switch (mode) {
                case SOC_PPC_OAM_REPORT_MODE_COMPACT:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeCompact);
                    break;
                case SOC_PPC_OAM_REPORT_MODE_NORMAL:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeNormal);
                    break;
                case SOC_PPC_OAM_REPORT_MODE_RAW:
                    COMPILER_64_SET(*arg, 0, bcmOamReportModeTypeRaw);
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error")));
                }
            }
            else
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));

        }
        break;

    case bcmOamControl1588ToD:
      {
         rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_get, (unit, 0, arg));
         BCMDNX_IF_ERR_EXIT(rv);         
      }
      break;
    case bcmOamControlNtpToD:
      {
         rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_get, (unit, 1, arg));
         BCMDNX_IF_ERR_EXIT(rv);
      }
      break;

    case bcmOamControlLmPcpCounterRangeMin:
        {
            /* Read both values, write the min to the output parameter */
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }
            rv = soc_ppd_oam_counter_range_get(unit, &fieldvalue32, &dummy);
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;

    case bcmOamControlLmPcpCounterRangeMax:
        {
            /* Read both values, write the max to the output parameter */
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
            }
            rv = soc_ppd_oam_counter_range_get(unit, &dummy, &fieldvalue32);
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;

    case bcmOamControlOampPuntPacketIntPri:
        {
            /* Read the above 64 bit register, extract the fields, combine them and write to the output parameter */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_control_punt_packet_int_pri_get, (unit, &fieldvalue32));
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;

    case  bcmOamControlOampCCMWeight:
        {
            /* Read the register, extract the field and write to the output parameter */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_control_ccm_weight_get, (unit, &fieldvalue32));
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;

    case  bcmOamControlOampSATWeight:
        {
            /* Read the register, extract the field and write to the output parameter */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_control_sat_weight_get, (unit, &fieldvalue32));
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;

    case  bcmOamControlOampResponseWeight:
        {
            /* Read the register, extract the field and write to the output parameter */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_control_response_weight_get, (unit, &fieldvalue32));
            COMPILER_64_SET(*arg, 0, fieldvalue32);
        }
        break;
    case bcmOamControlMplsLmQueryControlCode:
      {
        /* Read the register, extract the field and write to the output parameter */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_get, (unit, 0 /* LM query */, &fieldvalue32));
        COMPILER_64_SET(*arg, 0, fieldvalue32);
      }
      break;
    case bcmOamControlMplsLmResponderControlCode:
      {
        /* Read the register, extract the field and write to the output parameter */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_get, (unit, 1 /* LM response */, &fieldvalue32));
        COMPILER_64_SET(*arg, 0, fieldvalue32);
      }
      break;
    case bcmOamControlMplsDmQueryControlCode:
      {
        /* Read the register, extract the field and write to the output parameter */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_get, (unit, 0 /* DM query */, &fieldvalue32));
        COMPILER_64_SET(*arg, 0, fieldvalue32);
      }
      break;
    case bcmOamControlMplsDmResponderControlCode:
      {
        /* Read the register, extract the field and write to the output parameter */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_get, (unit, 1 /* DM response */, &fieldvalue32));
        COMPILER_64_SET(*arg, 0, fieldvalue32);
      }
      break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_control_get(
   int unit,
   bcm_oam_control_type_t type,
   uint64 *arg) {
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

   rv = _bcm_dpp_oam_control_get(unit, type, arg);
   BCMDNX_IF_ERR_EXIT(rv);

exit:
   BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_oam_control_set(
   int unit,
   bcm_oam_control_type_t type,
   uint64 arg) {
   uint32 soc_sand_rv;
   int rv = BCM_E_NONE;
   uint64   MAX_RANGE,counter_range_min_u64,counter_range_max_u64;

   BCMDNX_INIT_FUNC_DEFS;

   switch (type) {
   case bcmOamControlReportMode:
      {
         if (SOC_IS_JERICHO(unit)) {
            SOC_PPC_OAM_REPORT_MODE mode;
            uint32 arg_lw = COMPILER_64_LO(arg);
            switch (arg_lw) {
            case bcmOamReportModeTypeCompact:
               mode = SOC_PPC_OAM_REPORT_MODE_COMPACT;
               break;
            case bcmOamReportModeTypeNormal:
               mode = SOC_PPC_OAM_REPORT_MODE_NORMAL;
               break;
            case bcmOamReportModeTypeRaw:
               /*mode = SOC_PPC_OAM_REPORT_MODE_RAW;
                 break;*/ /* Not supported */
            default:
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported bcm oam report mode")));
            }
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oamp_report_mode_set, (unit, mode));
            BCMDNX_IF_ERR_EXIT(rv);
         } else BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));

      }
      break;
   case bcmOamControl1588ToD:
      {
         soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_set, (unit, 0, arg));
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);         }
      break;
   case bcmOamControlNtpToD:
      {
         soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_set, (unit, 1, arg));
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
      break;

   case bcmOamControlSrcMacLsbReserve:
      {
         int port;
         if (SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The control type should not be used on this device\n")));
         }
         port = COMPILER_64_LO(arg);
         rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_reserve(unit, port);
         BCMDNX_IF_ERR_EXIT(rv);
      }
      break;
   case bcmOamControlSrcMacLsbUnReserve:
      {
         int port;
         if (SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The control type should not be used on this device\n")));
         }
         port = COMPILER_64_LO(arg);
         rv = _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_unreserve(unit, port);
         BCMDNX_IF_ERR_EXIT(rv);
      }
      break;
   case bcmOamControlLmPcpCounterRangeMin:
      {
         SOC_SAND_OUT  uint32   counter_range_min;
         SOC_SAND_OUT  uint32   counter_range_max;
         if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
         }
         COMPILER_64_SET(MAX_RANGE, 0, 0x7fff);
         if (COMPILER_64_GT(arg, MAX_RANGE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal LM counter range min value\n")));
         }
         soc_sand_rv = soc_ppd_oam_counter_range_get(unit, &counter_range_min, &counter_range_max);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         COMPILER_64_SET(counter_range_max_u64, 0, counter_range_max);
         if (COMPILER_64_GT(arg, counter_range_max_u64)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Counter Range Min must be smaller than Counter Range Max\n")));
         }
         soc_sand_rv = soc_ppd_oam_counter_range_set(unit, COMPILER_64_LO(arg), counter_range_max);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
      break;
   case bcmOamControlLmPcpCounterRangeMax:
      {
         SOC_SAND_OUT  uint32   counter_range_min;
         SOC_SAND_OUT  uint32   counter_range_max;
         if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
         }
         COMPILER_64_SET(MAX_RANGE, 0, 0x7fff);
         if (COMPILER_64_GT(arg, MAX_RANGE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal LM counter range max value\n")));
         }
         soc_sand_rv = soc_ppd_oam_counter_range_get(unit, &counter_range_min, &counter_range_max);
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
         COMPILER_64_SET(counter_range_min_u64, 0, counter_range_min);
         if (COMPILER_64_LT(arg, counter_range_min_u64)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Counter Range Max must be larger than Counter Range Min\n")));
         }
         soc_sand_rv = soc_ppd_oam_counter_range_set(unit, counter_range_min, COMPILER_64_LO(arg));
         BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
      break;
   case bcmOamControlOampPuntPacketIntPri: 
      {
         /* Bits [1:0] of given value determine
            the color (DP), bits [4:2] determine
            the Traffic Class */
         uint32                  DP_MASK,TC_MASK;
         uint32                  dp,tc;

         if(SOC_IS_ARADPLUS(unit)){
            COMPILER_64_SET(MAX_RANGE, 0, 0x001F);
            if (COMPILER_64_GT(arg, MAX_RANGE)) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal DP and TC values\n")));
            }
            DP_MASK  =  0x0003;
            TC_MASK  =  0x001C;
            dp = COMPILER_64_LO(arg) & DP_MASK;          /* prepare dp value: (arg &0x3) */
            tc = (COMPILER_64_LO(arg) & TC_MASK) >> 2;   /* prepare tc value: (arg >> 2)&0x7 */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_arad_oamp_cpu_port_dp_tc_set, (unit,dp,tc));
            BCMDNX_IF_ERR_EXIT(rv);
         }
      }
      break;
   case  bcmOamControlOampCCMWeight:
      {
         uint32   txm_arbiter_weight;

         if(SOC_IS_ARADPLUS(unit)){
            COMPILER_64_SET(MAX_RANGE, 0, 0x6);
            if (COMPILER_64_GT(arg, MAX_RANGE)) {  /* Valid values 0-6 */
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal CCM weight value\n")));
            }
            txm_arbiter_weight   = COMPILER_64_LO(arg);
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_arad_oamp_tmx_arb_weight_set, (unit,txm_arbiter_weight));
            BCMDNX_IF_ERR_EXIT(rv);


         }

      }
      break;
   case  bcmOamControlOampSATWeight:
      {
         uint32   sat_arbiter_weight;

         if(SOC_IS_ARADPLUS(unit)){
            COMPILER_64_SET(MAX_RANGE, 0, 0x6);
            if (COMPILER_64_GT(arg, MAX_RANGE)) {  /* Valid values 0-6 */
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal SAT/TST weight value\n")));
            }
            sat_arbiter_weight   = COMPILER_64_LO(arg);
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_sat_arb_weight_set, (unit,sat_arbiter_weight));
            BCMDNX_IF_ERR_EXIT(rv);
         }
      }

      break;
   case  bcmOamControlOampResponseWeight:
      {
         uint32   rsp_arbiter_weight;

         if(SOC_IS_ARADPLUS(unit)){
            COMPILER_64_SET(MAX_RANGE, 0, 0x6);
            if (COMPILER_64_GT(arg, MAX_RANGE)) {  /* Valid values 0-6 */
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal RSP weight value\n")));
            }
            rsp_arbiter_weight   = COMPILER_64_LO(arg);
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_arad_oamp_response_weight_set, (unit,rsp_arbiter_weight));
            BCMDNX_IF_ERR_EXIT(rv);

         }
      }
      break;
   case  bcmOamControlMplsLmQueryControlCode:
      {
        uint32 lm_query_control_code;

        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
        }

        COMPILER_64_SET(MAX_RANGE, 0, 0x2);
        if (COMPILER_64_GT(arg, MAX_RANGE)) {
          /* Valid values are 0-2 */
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal LM query control code value\n")));
        }
        lm_query_control_code = COMPILER_64_LO(arg);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_set, (unit, 0 /* LM query */, lm_query_control_code));
        BCMDNX_IF_ERR_EXIT(rv);
      }
      break;
   case  bcmOamControlMplsLmResponderControlCode:
      {
        uint32 lm_response_control_code;

        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
        }

        COMPILER_64_SET(MAX_RANGE, 0, 0x1D);
        if (COMPILER_64_GT(arg, MAX_RANGE)) {
          /* Valid values are 0-29 */
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal LM response control code value\n")));
        }
        lm_response_control_code = COMPILER_64_LO(arg);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_set, (unit, 1 /* LM response */, lm_response_control_code));
        BCMDNX_IF_ERR_EXIT(rv);
      }
      break;
   case  bcmOamControlMplsDmQueryControlCode:
      {
        uint32 dm_query_control_code;

        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
        }

        COMPILER_64_SET(MAX_RANGE, 0, 0x2);
        if (COMPILER_64_GT(arg, MAX_RANGE)) {
          /* Valid values are 0-2 */
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal DM query control code value\n")));
        }
        dm_query_control_code = COMPILER_64_LO(arg);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_set, (unit, 0 /* DM query */, dm_query_control_code));
        BCMDNX_IF_ERR_EXIT(rv);
      }
      break;
   case  bcmOamControlMplsDmResponderControlCode:
      {
        uint32 dm_response_control_code;

        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This control type should not be used on this device\n")));
        }

        COMPILER_64_SET(MAX_RANGE, 0, 0x1D);
        if (COMPILER_64_GT(arg, MAX_RANGE)) {
          /* Valid values are 0-29 */
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Illegal DM response control code value\n")));
        }
        dm_response_control_code = COMPILER_64_LO(arg);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_set, (unit, 1 /* DM response */, dm_response_control_code));
        BCMDNX_IF_ERR_EXIT(rv);
      }
      break;

   default:
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm oam control type")));
   }

exit:
   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_control_set(
    int unit,
    bcm_oam_control_type_t type,
    uint64 arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_control_set(unit, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_control_indexed_set(
    int unit,
    bcm_oam_control_key_t key,
    uint64 arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_control_indexed_set(unit, key, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_oam_control_indexed_get(
    int unit, 
    bcm_oam_control_key_t key, 
    uint64 *arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_control_indexed_get(unit, key, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_tod_leap_control_get(
   int unit,
   bcm_oam_timestamp_format_t ts_format,
   bcm_oam_tod_leap_control_t *leap_info)
{
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;
   
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_leap_control_get, (unit, ts_format, leap_info));

   BCMDNX_IF_ERR_EXIT(rv);

exit:
   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_oam_tod_leap_control_set(
   int unit,
   bcm_oam_timestamp_format_t ts_format,
   bcm_oam_tod_leap_control_t leap_info)
{
   int rv = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;
    
   rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_tod_leap_control_set, (unit, ts_format, leap_info));

   BCMDNX_IF_ERR_EXIT(rv);

exit:
   BCMDNX_FUNC_RETURN;
}


/* ***************************************************************/
/********************OAM DIGNOSTICS SECTION************************/
/* **************************************************************/



/* Function recieves a endpoint info presumed to be remote and prints it information.
   Second parameter is the local endpoint. If none is given function will search for the local endpoint.*/
void bcm_oam_remote_endpoint_print(int unit, bcm_oam_endpoint_info_t *rmep_info, bcm_oam_endpoint_info_t *local_endpoint)
{
    uint32 ccm_ms, ccm_micro_s;
    BCMDNX_INIT_FUNC_DEFS;
    LOG_CLI((BSL_META_U(unit,
                        "=====Remote Endpoint. ID: 0X%x name: %d \n"),
                        rmep_info->id, rmep_info->name));

    if (rmep_info->ccm_period == 0) {
        if (local_endpoint == NULL) {
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
            SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
            BCM_SAND_IF_ERR_EXIT(soc_ppd_oam_oamp_mep_db_entry_get(unit, rmep_info->local_id, &mep_db_entry));
            SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_ms, ccm_micro_s, mep_db_entry.ccm_interval);
            multiply_by_3_5(&ccm_ms, &ccm_micro_s);
        } else {
            ccm_ms = local_endpoint->ccm_period;
            ccm_micro_s = 0;
            multiply_by_3_5(&ccm_ms, &ccm_micro_s);
        }
    } else {
        ccm_ms =  rmep_info->ccm_period;
        ccm_micro_s = 0;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\t\tCCM period (in MS): %d.%.2u "),
                        ccm_ms, ccm_micro_s));
    if (rmep_info->loc_clear_threshold) {
        LOG_CLI((BSL_META_U(unit,
                            "Loss of continuity clear threshold: %d \n"),
                            rmep_info->loc_clear_threshold));
    } else {
        LOG_CLI((BSL_META_U(unit,
                             "Automatic clearing loss of continuity disabled.\n")));
    }
    if (rmep_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
        if (rmep_info->port_state == BCM_OAM_PORT_TLV_BLOCKED) {
            LOG_CLI((BSL_META("\t\tPort state - blocked\n")));
        }
        else if (rmep_info->port_state == BCM_OAM_PORT_TLV_UP) {
            LOG_CLI((BSL_META("\t\ttPort state - up\n")));
        }
    }
    if (rmep_info->faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) {
           LOG_CLI((BSL_META("\t\tAn RDI has been received\n")));
    }
    if (rmep_info->faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) {
        LOG_CLI((BSL_META("\t\tRMEP is in LOC state\n")));
    }
    if (rmep_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) {
        if (rmep_info->interface_state ==  BCM_OAM_INTERFACE_TLV_UP) {
            LOG_CLI((BSL_META("\t\tInterface is up\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_DOWN) {
            LOG_CLI((BSL_META("\t\tInterface is down\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_DORMANT) {
            LOG_CLI((BSL_META("\t\tInterface is dormant\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_TESTING) {
            LOG_CLI((BSL_META("\t\tInterface is testing\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_UNKNOWN) {
            LOG_CLI((BSL_META("\t\tInterface is unknown\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_NOTPRESENT) {
            LOG_CLI((BSL_META("\t\tInterface is not present\n")));
        }
        else if (rmep_info->interface_state == BCM_OAM_INTERFACE_TLV_LLDOWN) {
            LOG_CLI((BSL_META("\t\tInterface is LL down\n")));
        }
    }
exit:
    BCMDNX_FUNC_RETURN_VOID
}

/*Function prints fields that are relevant to oam over mpls only*/
void print_oamOmpls_information(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    LOG_CLI((BSL_META_U(unit,
                        "\t\tInterface ID: %d\n"),
                        endpoint_info->intf_id));
    LOG_CLI((BSL_META_U(unit,
                        "\t\tInnermost MPLS label encapsulating the OAM BPDU: ")));
    bcm_dpp_print_mpls_egress_label(&(endpoint_info->egress_label));
}


int print_oam_RFC6374_information(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    uint32 soc_sand_rv;
    uint32 found_bitmap[1] = {0};

    BCMDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META_U(unit,
                        "\t\tLoss management counter base id: %d\n"),
                        endpoint_info->lm_counter_base_id));

    LOG_CLI((BSL_META_U(unit,
                        "\t\tGport 0X%x\n"),
                        endpoint_info->gport));

    if (endpoint_info->mpls_out_gport!=BCM_GPORT_INVALID) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tMPLS out gport 0X%x\n"),
                            endpoint_info->mpls_out_gport));
    }

    /** Accelerated endpoint */
    if (endpoint_info->tx_gport != BCM_GPORT_INVALID)
    {
        LOG_CLI((BSL_META_U(unit,
                              "\t\tEndpoint is accelerated in HW. TX fields:\n")));

        LOG_CLI((BSL_META_U(unit,
                              "\t\tSession ID: %d, TX gport: 0X%x, \n"),endpoint_info->session_id, endpoint_info->tx_gport));

       /* look for additional entries in the MEP-DB*/
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, endpoint_info->id, found_bitmap));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM) != 0 ))
        {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tMEP has additional LM entry.\n")));
        }

        if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM_STAT) != 0 ))
        {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tMEP has additional LM STAT entry.\n")));
        }

        if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM) != 0 ))
        {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tMEP has additional DM entry.\n")));
        }

    } 
    else 
    {
        LOG_CLI((BSL_META_U(unit,
                             "\t\tEndpoint is not accelerated . TX fields unavailable.\n")));
    }

exit:
    BCMDNX_FUNC_RETURN
}

int bcm_oam_endpoint_info_diag_print(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR cur_member = 0 ;
    ENDPOINT_LIST_MEMBER_PTR member_next ;
    bcm_oam_endpoint_info_t rmep_info;
    uint8 found;
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /*first, print the information about this endpoint*/
    if (_BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info->id)) { /*default mep*/
        LOG_CLI((BSL_META_U(unit,
                            "=====Default endpoint. ID: %d \n\t\t***Properties:\n"),
                            endpoint_info->id));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "=====Endpoint ID: 0X%x Group: %d \n\t\t***Properties:\n"),
                            endpoint_info->id,  endpoint_info->group));
    }

    if (_BCM_OAM_DISSECT_IS_ENDPOINT_MIP(endpoint_info)) {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tMaintenance entity is intermediate\n")));
    } else if (endpoint_info->type == bcmOAMEndpointTypeEthernet ) {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tEndpoint direction: ")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info)) {
            LOG_CLI((BSL_META_U(unit,
                                "Up facing\n")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Down facing\n")));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                         "\t\t=====Type: %d======\n"),
                         endpoint_info->type));
    switch (endpoint_info->type) {
    case bcmOAMEndpointTypeEthernet:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM over Ethernet\n")));
        break;
    case bcmOAMEndpointTypeBHHMPLS:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type BHH MPLS\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeBHHPwe:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type BHH - PWE\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeBHHPweGAL:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type BHH - PWEoGAL\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeMPLSNetwork:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type MPLSNetwork\n")));
      if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
          print_oamOmpls_information(unit, endpoint_info);
      }
      break;
    case bcmOAMEndpointTypeBhhSection:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type BHH - Section\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeMplsLmDmLsp:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type MPLS-LM-DM - MPLS\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeMplsLmDmPw:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type MPLS-LM-DM - PWE\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    case bcmOAMEndpointTypeMplsLmDmSection:
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM type MPLS-LM-DM - Section\n")));
        if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
            print_oamOmpls_information(unit, endpoint_info);
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported endpoint type.")));
    }

    if (_BCM_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        rv = print_oam_RFC6374_information(unit, endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv); 
        BCM_EXIT;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\t\tEndpoint Level: %d\n"), endpoint_info->level));

    if (endpoint_info->lm_flags & BCM_OAM_LM_PCP) {
        LOG_CLI((BSL_META_U(unit,
                             "\t\tAn 8-tuple of LM counters are set.\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\t\tLoss management counter base id: %d\n"),
                        endpoint_info->lm_counter_base_id));

    if (_BCM_OAM_IS_MEP_ID_DEFAULT(unit, endpoint_info->id)) {
        BCM_EXIT;
    }

    if(!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE))
    {
        bcm_oam_group_info_t group_info;
        int i;
        char group_name[BCM_OAM_GROUP_NAME_LENGTH*2 + 1];

        rv = bcm_oam_group_get(unit, endpoint_info->group, &group_info);
        BCMDNX_IF_ERR_EXIT(rv);

        for(i=0; i<BCM_OAM_GROUP_NAME_LENGTH; i++) {
            sprintf(group_name, "%02x", group_info.name[i]);
        }
        LOG_CLI((BSL_META_U(unit,
                            "\t\tGroup id: %d Group name: %s\n"),
                            endpoint_info->group, group_name));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\t\tGport 0X%x\n"),
                        endpoint_info->gport));
    if (endpoint_info->mpls_out_gport!=BCM_GPORT_INVALID) {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tMPLS out gport 0X%x\n"),
                            endpoint_info->mpls_out_gport));
    }

    LOG_CLI((BSL_META_U(unit,
                         "\t\tDestination mac address: " MAC_PRINT_FMT),
              MAC_PRINT_ARG(endpoint_info->dst_mac_address)));

    if (_BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(endpoint_info)) {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tEndpoint is accelerated in HW. TX fields:\n")));
        LOG_CLI((BSL_META_U(unit,
                            "\t\tName: %d, TX gport: 0X%x, \n"),
                            endpoint_info->name, endpoint_info->tx_gport));
        LOG_CLI((BSL_META_U(unit,
                             "\t\tSource  mac address: " MAC_PRINT_FMT),
                  MAC_PRINT_ARG(endpoint_info->src_mac_address)));
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) {
            LOG_CLI((BSL_META_U(unit,
                                 "\t\tUpdate and transmit RDI\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\t\tCCM period is %d. COS of outgoing CCMs: %d\n"),
                            endpoint_info->ccm_period, endpoint_info->int_pri));
        if (endpoint_info->vlan) {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tVlan information: vlan: %d, packet priority: %d, TPID: 0x%x\n"),
                                endpoint_info->vlan, endpoint_info->pkt_pri,
                                 endpoint_info->outer_tpid));

            if (endpoint_info->inner_vlan) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tInner vlan information: vlan: %d priority: %d tpid: %d\n"),
                                    endpoint_info->inner_vlan,
                                    endpoint_info->inner_pkt_pri, endpoint_info->inner_tpid));
            }
        }
        if (SOC_IS_ARADPLUS(unit)){
            /* look for additional entries in the MEP-DB*/
            uint32 soc_sand_rv;
            uint32 found_bitmap[1] = {0};

            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_oamp_search_for_lm_dm,(unit, endpoint_info->id, found_bitmap));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM) != 0 )) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tMEP has additional LM entry.\n")));
            }

            if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_LM_STAT) != 0 )) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tMEP has additional LM STAT entry.\n")));
            }
            if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_DM) != 0 )) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tMEP has additional DM entry.\n")));
            }

            /* QAX ONLY: Check for Extra Data entries*/
            if (SOC_IS_QAX(unit)){
               if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR)) != 0 ) {
                  if ((SHR_BITGET(found_bitmap, SOC_PPC_OAM_MEP_TYPE_EXT_DATA_PLD)) != 0 ) {
                     LOG_CLI((BSL_META_U(unit,
                                         "\t\tMEP has additional Extra Header and Payload Data entries.\n")));
                  } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "\t\tMEP has additional Extra Header Data entry.\n")));
                  }
               }
            }

            if (SOC_IS_JERICHO(unit)) {
                SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
                int rv;
                int dont_care[1];
                uint8 found;

                rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_info->id, &classifier_mep_entry, &found);
                BCMDNX_IF_ERR_EXIT(rv); 

                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No endpoint with id %d found."), endpoint_info->id));
                }
            
                if ((classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE) !=0)  {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tOn demand DM session active.\n")));
                }

                rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(unit, endpoint_info->id, dont_care, &found);
                BCMDNX_IF_ERR_EXIT(rv);
                if (found) {
                    bcm_oam_ais_t ais;
                    ais.id = endpoint_info->id;
                    rv = bcm_oam_ais_get(unit, &ais); 
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (ais.flags & BCM_OAM_AIS_MULTICAST) {
                        LOG_CLI((BSL_META_U(unit, "\t\tMulticast \n")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "\t\tAIS frames are being transmitted with period one frame per %s.\n"),
                                        (ais.period == 1000)? "second" : "minute" ));
                }

            }

            if (_BCM_OAM_IS_SERVER_SERVER(endpoint_info)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\t\tOAM Server endpoint: server side. remote gport: 0x%x.\n"),
                                    endpoint_info->remote_gport ));
            }
        }

    } else {
        LOG_CLI((BSL_META_U(unit,
                             "\t\tEndpoint is not accelerated . TX fields unavailable.\n")));
    }
    if (_BCM_OAM_IS_SERVER_CLIENT(endpoint_info)) {
        LOG_CLI((BSL_META_U(unit,
                            "\t\tOAM Server endpoint: client side. remote gport: 0x%x.\n"),
                            endpoint_info->remote_gport));
    }

    /*now getting the endpoint's rmeps and printing them*/
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_oam_bfd_mep_to_rmep_db_get(unit, endpoint_info->id, &rmep_list_p, &found));
    if (found)
    {
        int rv ;

        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, rmep_list_p, &cur_member) ;
        BCMDNX_IF_ERR_EXIT(rv);

        while (cur_member)
        {
            uint32 member_index ;

            bcm_oam_endpoint_info_t_init(&rmep_info) ;
            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, cur_member, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv) ;
            rmep_info.id = (bcm_oam_endpoint_t)member_index ;
            rv = bcm_oam_endpoint_get(unit, rmep_info.id, &rmep_info);
            if (rv == BCM_E_NONE) {
                bcm_oam_remote_endpoint_print(unit, &rmep_info, endpoint_info);
            } else {
                if (rv != BCM_E_NOT_FOUND) {
                    LOG_CLI((BSL_META_U(unit,
                                    "\t\t Failure in remote MEP id (%d) get (%s)\n"),
                                rmep_info.id,bcm_errmsg(rv)));
                }
            }

            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, cur_member, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv) ;
            cur_member = member_next ;
        }
    }
    else
    {
        LOG_CLI((BSL_META_U(unit,
                             "\t\tEndpoint has no associated remote MEPS\n")));
    }


exit:
    BCMDNX_FUNC_RETURN
}
/*
 * callback used for diagnostics. Find an endpoint, determine if it is bfd or oam, 
 * then call proper function to print information. 
 */
int
_bcm_oam_diag_iterate_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t user_data) {
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_bfd_endpoint_info_t bfd_endpoint;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *mep_entry =  (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *)user_data;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint);
    bcm_oam_endpoint_info_t_init(&endpoint_info);

    /*get the endpoint_info*/
    if ( (*(int*)key != -2  ) && ((mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)  || 
        (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
        (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP) || (mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION))) { /*found an oam endpoint*/
        endpoint_info.id = *(int *)key;
        BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_get(unit, endpoint_info.id, &endpoint_info));
        BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_info_diag_print(unit, &endpoint_info));
    } else { /*try looking for bfd endpoints*/
        bfd_endpoint.id = (*(int *)key == -2) ? -1: *(int *)key;
        BCMDNX_IF_ERR_EXIT(bcm_bfd_endpoint_get(unit, bfd_endpoint.id, &bfd_endpoint));
        bcm_dpp_bfd_endpoint_diag_print(&bfd_endpoint);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/************ Diagnostics functions ********************/

/* "BCM.0>diag oam endpoints" initiates the following function.
   Function prints information on all endpoints and their rmep.*/
int _bcm_dpp_oam_bfd_diagnostics_endpoints(int unit) {
    uint8 oam_is_init, bfd_is_init;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    (void)rv; /*rv is used in macros under some compilations*/

    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(bfd_is_init);
    if (!oam_is_init && !bfd_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "===================================\n")));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(unit, _bcm_oam_diag_iterate_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

/* "BCM.0>diag oam endpoints id=<endpoint_id>" initiates the following function.
   Function prints information endpoint_ID and its rmeps if applicable.*/
int _bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(int unit, int endpoint_id) {
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_bfd_endpoint_info_t bfd_endpoint;
    uint8 found;
    uint8 oam_is_init, bfd_is_init; 
    int rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    BCMDNX_INIT_FUNC_DEFS;


    _BCM_OAM_ENABLED_GET(oam_is_init);
    _BCM_BFD_ENABLED_GET(bfd_is_init);
    if (!oam_is_init && !bfd_is_init) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
    }

    bcm_oam_endpoint_info_t_init(&endpoint_info);
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, &mep_info, &found));
    if (!found) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    }

    if ((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
        (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
            (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||
        (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION || mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE) ||
        (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP || mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION)) { /*found an oam endpoint*/
        rv = bcm_petra_oam_endpoint_get(unit, endpoint_id, &endpoint_info);
        BCMDNX_IF_ERR_EXIT(rv);
        if (endpoint_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
            bcm_oam_remote_endpoint_print(unit, &endpoint_info, NULL);
        } else {
            BCMDNX_IF_ERR_EXIT(bcm_oam_endpoint_info_diag_print(unit, &endpoint_info));
        }
    } else { /*found a bfd endpoint*/
        rv = bcm_petra_bfd_endpoint_get(unit, endpoint_id, &bfd_endpoint);
        BCMDNX_IF_ERR_EXIT(rv);
        bcm_dpp_bfd_endpoint_diag_print(&bfd_endpoint);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* ****************(********************************* */
/* Input :  EP counter number                         */
/* Output:  CRPS engine number and counter number     */
/* Method:  Finds which range the EP counter fits     */
/* in and calculates the crps counter number          */
/* Target:  Jericho and up                            */
/* ****************(********************************* */
int _bcm_oam_map_mep_info_counter_to_crps_counter(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info, bcm_dpp_oam_crps_counters_t *crps_counters) {
   bcm_stat_counter_engine_t  engine;
   bcm_stat_counter_config_t  config;
   int                        i=0, j=0;
   int                        rv;
   uint32                     mep_core;

   BCMDNX_INIT_FUNC_DEFS;

   crps_counters->crps_ingress_engine = -1;
   crps_counters->crps_egress_engine = -1;

   for (i = 0; i < (SOC_DPP_DEFS_GET(unit, nof_counter_processors)); i++) {  /* search 16 counter engines which are available for OAM */
      engine.engine_id = i;
      rv = bcm_stat_counter_config_get(unit, &engine, &config);
      if (rv != BCM_E_EMPTY) {
         BCMDNX_IF_ERR_EXIT(rv);

         /* Find core */
         if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(&mep_info)) {
            /* Get core from LIF (For upmeps, passive_side_lif is the global-lif) */
            rv = _bcm_dpp_oam_core_from_lif_get(unit, mep_info.passive_side_lif, &mep_core);
            BCMDNX_IF_ERR_EXIT(rv);
         } else {
            rv = _bcm_dpp_oam_core_from_lif_get(unit, mep_info.lif, &mep_core);
            BCMDNX_IF_ERR_EXIT(rv);
         }

         if (((mep_info.counter >= config.source.pointer_range.start) && (mep_info.counter <= config.source.pointer_range.end)) &&
            (mep_core == config.source.core_id)) {

            if (SOC_IS_JERICHO(unit)){
                if (config.source.engine_source == bcmStatCounterSourceIngressOam) {
                    crps_counters->crps_ingress_engine = i;
                    if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) {
                        for (j = 0; j < 8; j++) {
                            crps_counters->crps_ingress_counter[j] = mep_info.counter / 2 - 8 + j;
                        }
                    } else {
                        crps_counters->crps_ingress_counter[0] = mep_info.counter / 2;
                    }
                } else if (config.source.engine_source ==  bcmStatCounterSourceEgressOam) {
                    crps_counters->crps_egress_engine= i;
                    if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) {
                        for (j = 0; j < 8; j++) {
                            crps_counters->crps_egress_counter[j] = mep_info.counter / 2 - 8 + j;
                        }
                    } else {
                        crps_counters->crps_egress_counter[0] = mep_info.counter / 2;
                    }
                }
            } else {
                if (config.source.engine_source == bcmStatCounterSourceIngressOam || config.source.engine_source == bcmStatCounterSourceEgressOam) {
                    if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) {
                        for (j = 0; j < 8; j++) {
                            crps_counters->crps_ingress_counter[j] = mep_info.counter - config.source.pointer_range.start + j + 8;
                            crps_counters->crps_egress_counter[j] = mep_info.counter - config.source.pointer_range.start + j;
                        }
                    } else {
                        crps_counters->crps_ingress_counter[0] = mep_info.counter - config.source.pointer_range.start + 1;
                        crps_counters->crps_egress_counter[0] = mep_info.counter - config.source.pointer_range.start;
                    }
                    crps_counters->crps_ingress_engine = crps_counters->crps_egress_engine = i;
                    break;
                }
            }
         }
      }
   }
exit:
   BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_diagnostics_LM_counters(int unit, int endpoint_id) {
   uint8         found;
   int           rv;
   uint32        eg_val[1], ing_val[1];
   int           ingress_index = -1, egress_index = -1;
   uint8         oam_is_init, bfd_is_init;
   int           soc_port;
   unsigned int  command_id;
   int           i;
   bcm_dpp_oam_crps_counters_t        crps_counters;
   SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   mep_info;
   SOC_TMC_CNT_SRC_TYPE               src_type;

   BCMDNX_INIT_FUNC_DEFS;

   _BCM_OAM_ENABLED_GET(oam_is_init);
   _BCM_BFD_ENABLED_GET(bfd_is_init);
   if (!oam_is_init && !bfd_is_init) {
      BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
   }
   rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, endpoint_id, &mep_info, &found);
   if ((!found) || (!((mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
                      (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
                      (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||
                      (mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||(mep_info.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)))) {
      BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
   }

   if (SOC_IS_JERICHO(unit)) {

      rv = _bcm_oam_map_mep_info_counter_to_crps_counter(unit, mep_info, &crps_counters);
      BCMDNX_IF_ERR_EXIT(rv);

      if (crps_counters.crps_ingress_engine == -1 || crps_counters.crps_egress_engine == -1) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("CRPS counter engine not found.\n")));
      }

      if (mep_info.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP) {
         for (i = 0; i < 8; i++) {
            rv = soc_ppd_oam_get_crps_counter(unit, crps_counters.crps_ingress_engine, crps_counters.crps_ingress_counter[i], ing_val);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = soc_ppd_oam_get_crps_counter(unit, crps_counters.crps_egress_engine, crps_counters.crps_egress_counter[i], eg_val);
            BCMDNX_IF_ERR_EXIT(rv);

            if (eg_val[0]) {
               /* Changed Level range from (i+1) to (i) , changing the levels of the counter from 1-8 Level to 0-7 Level */
               LOG_INFO(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Level: %d - Egress counter: %d.\t"),(i), eg_val[0]));
            } else {
               /* Changed Level range from (i+1) to (i) , changing the levels of the counter from 1-8 Level to 0-7 Level */
               LOG_INFO(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Level: %d - Egress counter inactive.\t"),(i )));
            }
            if (ing_val[0]) {
               LOG_INFO(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Ingress counter: %d.\n"), ing_val[0]));
            } else {
               LOG_INFO(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Ingress counter inactive.\n")));
            }
         }
      } else {
         rv = soc_ppd_oam_get_crps_counter(unit, crps_counters.crps_ingress_engine, crps_counters.crps_ingress_counter[0], ing_val);
         BCMDNX_IF_ERR_EXIT(rv);
         rv = soc_ppd_oam_get_crps_counter(unit, crps_counters.crps_egress_engine, crps_counters.crps_egress_counter[0], eg_val);
         BCMDNX_IF_ERR_EXIT(rv);

         if (eg_val[0]) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Egress counter: %d.\t"), eg_val[0]));
         } else {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Egress counter inactive.\t")));
         }
         if (ing_val[0]) {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Ingress counter: %d.\n"), ing_val[0]));
         } else {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Ingress counter inactive.\n")));
         }
      }
   } else {
      for (soc_port = 0; soc_port < SOC_DPP_DEFS_GET(unit, nof_counter_processors); ++soc_port) {
         rv = bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_source, soc_port, &src_type);
         BCMDNX_IF_ERR_EXIT(rv);

         if ((src_type == SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM) || (src_type == SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM)) {
            rv = bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_command_id, soc_port, &command_id);
            BCMDNX_IF_ERR_EXIT(rv);

            if (command_id == 0) {
               /*soc_port is an egress oam port*/
               egress_index = soc_port;
            } else if (command_id == 1) {
               /*soc_port is an ingress oam port*/
               ingress_index = soc_port;
            }
         }
      }

      if ((ingress_index == -1) || (egress_index == -1)) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("CRPS counter engine not found.\n")));
      }

      rv = soc_ppd_oam_get_crps_counter(unit, egress_index, mep_info.counter, eg_val);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = soc_ppd_oam_get_crps_counter(unit, ingress_index, mep_info.counter, ing_val);
      BCMDNX_IF_ERR_EXIT(rv);

      if (eg_val[0]) {
         LOG_INFO(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Egress counter: %d.\t"), eg_val[0]));
      } else {
         LOG_INFO(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Egress counter inactive.\t")));
      }
      if (ing_val[0]) {
         LOG_INFO(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Ingress counter: %d.\n"), ing_val[0]));
      } else {
         LOG_INFO(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Ingress counter inactive.\n")));
      }
   }

exit:
   BCMDNX_FUNC_RETURN;
}


int bcm_petra_oam_protection_packet_header_set(
      int unit,
      bcm_pkt_blk_t *packet_header)
{
    int rv;
    SOC_PPC_OAM_OAMP_PROTECTION_HEADER     packet_protection_header;
  
    BCMDNX_INIT_FUNC_DEFS;

    if (packet_header->len > SOC_PPC_OAM_MAX_PROTECTION_HEADER_SIZE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: protection packet header should be smaller than 80 ")));
    }
    sal_memcpy(&packet_protection_header.data,packet_header->data,packet_header->len);
    packet_protection_header.len = packet_header->len;

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_dpp_oam_oamp_protection_packet_header_raw_set, (unit, &packet_protection_header)); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* user must provide the lenght and alloc the data size acordingly */
int bcm_petra_oam_protection_packet_header_get(
      int unit,
      bcm_pkt_blk_t *packet_header)
{
    int rv;
    SOC_PPC_OAM_OAMP_PROTECTION_HEADER     packet_protection_header ;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (!(packet_header->len > 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: the user must provide the length ")));
    }
    if (packet_header->len > SOC_PPC_OAM_MAX_PROTECTION_HEADER_SIZE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: protection packet header should be smaller than 80 ")));
    }
    
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_dpp_oam_oamp_protection_packet_header_raw_get, (unit, &packet_protection_header)); 
    BCMDNX_IF_ERR_EXIT(rv);
    
    sal_memcpy(packet_header->data, packet_protection_header.data,packet_header->len);
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Add an OAM signal degradation/ signal fault object */
int  bcm_petra_oam_sd_sf_detection_add(int unit,bcm_oam_sd_sf_detection_t  *sd_sf_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB sd_sf_profile;
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY  sd_sf_entry;
    uint32 rmep_id_internal = 0;
    uint32 sd_sf_db_index = 0;
    uint32 sd_sf_profile_id = 0;
    uint32 soc_sand_rv=0;
    int rv = 0;
    int flags = 0;
    int is_allocated = 0;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(&sd_sf_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_clear(&sd_sf_profile);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sd_sf_ptr->mep_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), sd_sf_ptr->mep_id));
    }
    rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, sd_sf_ptr->rmep_id, &rmep_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  sd_sf_ptr->rmep_id));
    }
    if (sd_sf_ptr->mep_id != rmep_info.mep_index) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
    }

    _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, sd_sf_ptr->rmep_id);

    if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
       soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, sd_sf_ptr->mep_id, &mep_db_entry);
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else{
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only support in accelerated Mep.")));
    }

    if(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF){
        sd_sf_db_index = classifier_mep_entry.sd_sf_id;
    }
    else {
        rv = bcm_dpp_am_oam_sd_sf_id_alloc(unit, flags,&sd_sf_db_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*get the mep type*/
    if((mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) ||
       (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) ||
       (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) ||(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)){
       sd_sf_entry.entry_format = 0x0;
    }
    else{
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Don't support such Mep type")));
    }


    if(sd_sf_ptr->flags&BCM_OAM_SD_SF_FLAGS_ALERT_METHOD){
      sd_sf_profile.alert_method = 1;
    }
    if(sd_sf_ptr->flags&BCM_OAM_SD_SF_FLAGS_ALERT_SUPRESS){
      sd_sf_profile.supress_alerts=1 ;
    }
    sd_sf_profile.sd_set_thresh = sd_sf_ptr->sd_set_threshold;
    sd_sf_profile.sd_clr_thresh= sd_sf_ptr->sd_clear_threshold;
    sd_sf_profile.sf_set_thresh = sd_sf_ptr->sf_set_threshold;
    sd_sf_profile.sf_clr_thresh = sd_sf_ptr->sf_clear_threshold;
    sd_sf_profile.wnd_lngth = sd_sf_ptr->sliding_window_length;

    rv = _bcm_dpp_am_template_oam_sd_sf_profile_alloc(unit, 0, &sd_sf_profile, &is_allocated, (int*)(&sd_sf_profile_id));
    BCMDNX_IF_ERR_EXIT(rv);


    if (is_allocated) {
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_profile_set,(unit,sd_sf_profile_id, &sd_sf_profile));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /*Save the sd/sf index to sw db */
    classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF;
    classifier_mep_entry.sd_sf_id= sd_sf_db_index;
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, sd_sf_ptr->mep_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    sd_sf_entry.ccm_tx_rate= mep_db_entry.ccm_interval;
    sd_sf_entry.thresh_profile= sd_sf_profile_id;
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_db_set,(unit, rmep_id_internal,classifier_mep_entry.sd_sf_id,&sd_sf_entry));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_DPP_UNIT_CHECK(unit);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM signal degradation/ signal fault object */
int  bcm_petra_oam_sd_sf_detection_get(int unit, bcm_oam_sd_sf_detection_t  *sd_sf_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY  sd_sf_entry;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB sd_sf_profile;
    int rv = 0;
    uint32 soc_sand_rv=0;
    int index = 0;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(&sd_sf_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_clear(&sd_sf_profile);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sd_sf_ptr->mep_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), sd_sf_ptr->mep_id));
    }
    rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, sd_sf_ptr->rmep_id, &rmep_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  sd_sf_ptr->rmep_id));
    }
    if (sd_sf_ptr->mep_id != rmep_info.mep_index) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
    }
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add SD/SF objext."), sd_sf_ptr->mep_id));
    } 
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_db_get,(unit, classifier_mep_entry.sd_sf_id,&sd_sf_entry));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_DPP_UNIT_CHECK(unit);
    
   sd_sf_ptr->sd_indication = sd_sf_entry.sd;
   sd_sf_ptr->sf_indication = sd_sf_entry.sf;
   sd_sf_ptr->sum_cnt= sd_sf_entry.sum_cnt; /* or sliding_wnd_cntr*/
   
   for( index=0; index<BCM_OAM_MAX_NUM_SLINDING_WINDOWS; index++){
       sd_sf_ptr->sliding_window_cnt[index] = sd_sf_entry.sliding_wnd_cntr[index];
   }
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_profile_get,(unit,sd_sf_entry.thresh_profile, &sd_sf_profile));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_DPP_UNIT_CHECK(unit);
    
    if(sd_sf_profile.alert_method) {
        sd_sf_ptr->flags |=BCM_OAM_SD_SF_FLAGS_ALERT_METHOD;
    }
    if(sd_sf_profile.supress_alerts) {
        sd_sf_ptr->flags |=BCM_OAM_SD_SF_FLAGS_ALERT_SUPRESS;
    }
    
    sd_sf_ptr->sd_set_threshold = sd_sf_profile.sd_set_thresh;
    sd_sf_ptr->sd_clear_threshold = sd_sf_profile.sd_clr_thresh;
    sd_sf_ptr->sf_set_threshold = sd_sf_profile.sf_set_thresh;
    sd_sf_ptr->sf_clear_threshold = sd_sf_profile.sf_clr_thresh;
    sd_sf_ptr->sliding_window_length = sd_sf_profile.wnd_lngth;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete an OAM signal degradation/ signal fault object */
int  bcm_petra_oam_sd_sf_detection_delete(int unit, bcm_oam_sd_sf_detection_t  *sd_sf_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY  sd_sf_entry;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB sd_sf_profile;
    uint32 profile_id = 0;
    int rv = 0;
    uint32 soc_sand_rv=0;
    int is_last = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(&sd_sf_entry);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_clear(&sd_sf_profile);
    
    rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, sd_sf_ptr->mep_id, &classifier_mep_entry, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), sd_sf_ptr->mep_id));
    }
    rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, sd_sf_ptr->rmep_id, &rmep_info, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  sd_sf_ptr->rmep_id));
    }
    if (sd_sf_ptr->mep_id != rmep_info.mep_index) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
    }
    if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add SD/SF objext."), sd_sf_ptr->mep_id));
    } 

    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_db_get,(unit, classifier_mep_entry.sd_sf_id,&sd_sf_entry));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_DPP_UNIT_CHECK(unit);
    
    /*clear sd/sf profile */
    profile_id = sd_sf_entry.thresh_profile;
    rv = _bcm_dpp_am_template_oam_sd_sf_profile_free(unit, (int)profile_id, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_last) {
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_profile_set,(unit,profile_id, &sd_sf_profile));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
    }
    
    /*clear sd/sf db */
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(&sd_sf_entry);
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_db_set,(unit, 0,classifier_mep_entry.sd_sf_id,&sd_sf_entry));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    BCM_DPP_UNIT_CHECK(unit);
    
    rv = bcm_dpp_am_oam_sd_sf_id_dealloc(unit, classifier_mep_entry.sd_sf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /*if delete successfully, need to clear SD/SF flag*/
    classifier_mep_entry.flags &= (~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF);
    classifier_mep_entry.sd_sf_id= 0;
    /*update sw db mep_info with new mep */
    rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, sd_sf_ptr->mep_id, &classifier_mep_entry);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/* Add an OAM y_1711 alarm object */
int  bcm_petra_oam_y_1711_alarm_add(int unit, bcm_oam_y_1711_alarm_t  *alarm_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 rmep_id_internal = 0;
    uint32 y1711_sd_sf_db_format = 0;
    uint32 y1711_sd_sf_id = 0;
    uint32 soc_sand_rv=0;
    int rv = 0;
    int flags = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    if((alarm_ptr->flags&BCM_OAM_Y_1711_FLAGS_THRESHOLD)&&(alarm_ptr->mep_id == -1)&&(alarm_ptr->rmep_id == -1)){/*global configration*/
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_config_set,(unit,alarm_ptr->d_excess_threshold,alarm_ptr->indicator_low_threshold ,alarm_ptr->indicator_high_threshold, 16));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
    }
    else {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, alarm_ptr->mep_id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), alarm_ptr->mep_id));
        }
        rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, alarm_ptr->rmep_id, &rmep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  alarm_ptr->rmep_id));
        }
        if (alarm_ptr->mep_id != rmep_info.mep_index) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
        }
        if(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM){
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Endpoint %d has already added 1711 alarm objext."), alarm_ptr->mep_id));
        }
        
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, alarm_ptr->rmep_id);
        
        if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
           soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, alarm_ptr->mep_id, &mep_db_entry);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else{
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only support in accelerated Mep.")));
        }
        /*get the mep type*/
        if((mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) || 
           (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION) || 
           (mep_db_entry.mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) || (mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS) ||(mep_db_entry.mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE)){
           y1711_sd_sf_db_format = 0x0;
        }
        else{
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Don't support such Mep type")));
        }
        
        rv = bcm_dpp_am_oam_y_1711_sd_sf_id_alloc(unit, flags,&y1711_sd_sf_id);
        BCMDNX_IF_ERR_EXIT(rv);
        
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_db_set,( unit,rmep_id_internal,y1711_sd_sf_id,y1711_sd_sf_db_format,mep_db_entry.ccm_interval,alarm_ptr->flags&BCM_OAM_Y_1711_FLAGS_ALERT_METHOD));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
        
        /*Save the sd/sf index to sw db */
        classifier_mep_entry.y1711_sd_sf_id= y1711_sd_sf_id;
        classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM;
        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, alarm_ptr->mep_id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv); 
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get an OAM y_1711 alarm object */
int  bcm_petra_oam_y_1711_alarm_get(int unit, bcm_oam_y_1711_alarm_t  * alarm_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY  sd_sf_1711_entry;
    uint32 soc_sand_rv =0;
    uint8   num_entry = 0;
    uint8 d_excess_threshold = 0;
    uint8 indicator_low_threshold  = 0;
    uint8 indicator_high_threshold = 0;
    int rv = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY_clear(&sd_sf_1711_entry);
    if((alarm_ptr->flags&BCM_OAM_Y_1711_FLAGS_THRESHOLD)&&(alarm_ptr->mep_id == -1)&&(alarm_ptr->rmep_id == -1)){
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_config_get,(unit,&d_excess_threshold,&indicator_low_threshold ,&indicator_high_threshold, &num_entry));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
        alarm_ptr->d_excess_threshold = d_excess_threshold;
        alarm_ptr->indicator_low_threshold = indicator_low_threshold;
        alarm_ptr->indicator_high_threshold = indicator_high_threshold;
    }
    else {
       rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, alarm_ptr->mep_id, &classifier_mep_entry, &found);
       BCMDNX_IF_ERR_EXIT(rv);
       if (!found) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), alarm_ptr->mep_id));
       }
       rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, alarm_ptr->rmep_id, &rmep_info, &found);
       BCMDNX_IF_ERR_EXIT(rv);
       if (!found) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  alarm_ptr->rmep_id));
       }
       if (alarm_ptr->mep_id != rmep_info.mep_index) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
       }
       
       if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't add 1711 Alarm objext."), alarm_ptr->mep_id));
       } 
       
       if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
          soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, alarm_ptr->mep_id, &mep_db_entry);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       }
       else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only support in accelerated Mep.")));
       }
       
       soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_db_get,(unit,classifier_mep_entry.y1711_sd_sf_id,&sd_sf_1711_entry));
       BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       BCM_DPP_UNIT_CHECK(unit);
       alarm_ptr->d_excess = sd_sf_1711_entry.d_excess;
       alarm_ptr->d_mismatch = sd_sf_1711_entry.d_mismatch;
       alarm_ptr->d_mismerge = sd_sf_1711_entry.d_mismerge;
       
       if(sd_sf_1711_entry.allert_method) {
           alarm_ptr->flags |=BCM_OAM_Y_1711_FLAGS_ALERT_METHOD;
       }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete an OAM y_1711 alarm object */
int  bcm_petra_oam_y_1711_alarm_delete(int unit, bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    uint8 oam_is_init = 0;
    uint8 found = 0;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    uint32 rmep_id_internal = 0;
    uint32 soc_sand_rv=0;
    int rv = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_OAM_ENABLED_GET(oam_is_init);
    
    if (!oam_is_init) {
        BCM_EXIT;
    }
    if (!SOC_IS_QAX(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Function available only in QAX.")));
    }
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);
    if((alarm_ptr->flags&BCM_OAM_Y_1711_FLAGS_THRESHOLD)&&(alarm_ptr->mep_id == -1)&&(alarm_ptr->rmep_id == -1)){
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_config_set,(unit,0,0 ,0, 0));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
    }
    else {
        rv = _bcm_dpp_oam_bfd_mep_info_db_get(unit, alarm_ptr->mep_id, &classifier_mep_entry, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found.\n"), alarm_ptr->mep_id));
        }
        rv = _bcm_dpp_oam_bfd_rmep_info_db_get(unit, alarm_ptr->rmep_id, &rmep_info, &found);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Error: Remote endpoint with id %d can not be found.\n"),  alarm_ptr->rmep_id));
        }
        if (alarm_ptr->mep_id != rmep_info.mep_index) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: local mep id error. Current value:%d\n"),rmep_info.mep_index));
        }
        if (!(classifier_mep_entry.flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Endpoint %d don't enable 1711 alarm objext."), alarm_ptr->mep_id));
        } 
        
        _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, alarm_ptr->rmep_id);
        
        if (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(&classifier_mep_entry)) {
           soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, alarm_ptr->mep_id, &mep_db_entry);
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else{
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only support in accelerated Mep.")));
        }
        
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_sd_sf_1711_db_set,(unit,rmep_id_internal,0,0,0,0));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        BCM_DPP_UNIT_CHECK(unit);
        
        rv = bcm_dpp_am_oam_y_1711_sd_sf_id_dealloc(unit, classifier_mep_entry.y1711_sd_sf_id);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /*if delete successfully, need to clear SD/SF flag*/
        classifier_mep_entry.flags &= (~SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM);
        classifier_mep_entry.sd_sf_id= 0;
        /*update sw db mep_info with new mep */
        rv = _bcm_dpp_oam_bfd_mep_info_db_update(unit, alarm_ptr->mep_id, &classifier_mep_entry);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(bcm_oam_mpls_tp_channel_type_t channel_type)
{
    switch(channel_type) {
        case bcmOamMplsTpChannelPweBfd:
            return _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM;
            break;
        case bcmOamMplsTpChannelPweonoam:
            return _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM;
            break;
        case bcmOamMplsTpChannelY1731:
            return _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM;
            break;
        default:
            return BCM_E_NOT_FOUND;
            break;
    }
    return BCM_E_NOT_FOUND;
}

int bcm_petra_oam_mpls_tp_channel_type_tx_set(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int value)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);

    /* Validate channel type value parameter */
    rv = _bcm_oam_mpls_tp_tx_channel_type_value_validity_check(unit,value);
    BCMDNX_IF_ERR_EXIT(rv);

    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_tx_set(unit, channel_type_ndx, value);
    BCMDNX_IF_ERR_EXIT(rv);
    

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_mpls_tp_channel_type_tx_get(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int *value)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);
    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_tx_get(unit, channel_type_ndx, value);
    BCMDNX_IF_ERR_EXIT(rv);
    

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_mpls_tp_channel_type_tx_delete(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int value)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);
    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_tx_delete(unit, channel_type_ndx, value);
    BCMDNX_IF_ERR_EXIT(rv);
 
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_mpls_tp_channel_type_rx_set(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int num_values, int *list_of_values)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    if(channel_type == bcmOamMplsTpChannelPweonoam) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmOamMplsTpChannelPweonoam not supported in RX direction\n")));
    }
    /* Validate num_values and list_of_values parameters */
    rv = _bcm_oam_mpls_tp_rx_channel_type_values_validity_check(unit, num_values,
                                                                list_of_values);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);
    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_rx_set(unit, channel_type_ndx, num_values, list_of_values);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_mpls_tp_channel_type_rx_delete(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int num_values, int *list_of_values)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    if(channel_type == bcmOamMplsTpChannelPweonoam) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmOamMplsTpChannelPweonoam not supported in RX direction\n")));
    }
    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);
    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_rx_delete(unit, channel_type_ndx, num_values, list_of_values);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_oam_mpls_tp_channel_type_rx_get(int unit, bcm_oam_mpls_tp_channel_type_t channel_type, int num_values, int *list_of_values, int *value_count)
{
    int channel_type_ndx = 0;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    if(channel_type == bcmOamMplsTpChannelPweonoam) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmOamMplsTpChannelPweonoam not supported in RX direction\n")));
    }
    /* Fill OAM Channel types CAM */
    channel_type_ndx = _bcm_petra_oam_mpls_tp_channel_type_to_channel_type_enum_ndx(channel_type);
    if (channel_type_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unknown channel type\n")));
    }
    
    rv = arad_pp_oam_channel_type_rx_get(unit, channel_type_ndx, num_values, list_of_values, value_count);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}

typedef struct _bcm_dpp_diag_print_hier_count_info_s {
    int oam_lif;
    int ing;
    int inner_outer;
    int counter;
    int engine_id;
    int base_ptr;
    uint32 ing_value;
    uint32 egr_value;
}_bcm_dpp_diag_print_hier_count_info_t;


#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM
int _bcm_dpp_oam_diag_print_hier_count_formatted(int unit,
                                                 _bcm_dpp_diag_print_hier_count_info_t *info,
                                                 int num_info)
{
    int info_idx;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("OAM HIER COUNT INFO");

    PRT_COLUMN_ADD("LIF");
    PRT_COLUMN_ADD("INGRESS");
    PRT_COLUMN_ADD("INNER_OUTER");
    PRT_COLUMN_ADD("COUNTER");
    PRT_COLUMN_ADD("BANK");
    PRT_COLUMN_ADD("BASE IDX");
    PRT_COLUMN_ADD("ING_VALUE");
    PRT_COLUMN_ADD("EGR_VALUE");

    for (info_idx = 0; info_idx < num_info; info_idx++) {
        PRT_ROW_ADD(PRT_ROW_SEP_EQUAL);
        PRT_CELL_SET("0x%x", info[info_idx].oam_lif);
        PRT_CELL_SET("%d", info[info_idx].ing);
        PRT_CELL_SET("%d", info[info_idx].inner_outer);
        PRT_CELL_SET("0x%x", info[info_idx].counter);
        PRT_CELL_SET("%d", info[info_idx].engine_id);
        PRT_CELL_SET("0x%x", info[info_idx].base_ptr);
        PRT_CELL_SET("%u", info[info_idx].ing_value);
        PRT_CELL_SET("%u", info[info_idx].egr_value);
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

int _bcm_dpp_oam_diag_print_hier_counters(
      int                                    unit,
      SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY  *oem1_key
    )
{
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint8 found = 0, is_pcp = 0;
    bcm_error_t rv;
    uint32 counter_range_min = 0, counter_range_max = 0;
    uint32 counter_ndx = 0;
    bcm_stat_counter_engine_t  engine;
    bcm_stat_counter_config_t  config;
    uint32                     mep_core;
    int      crps_engine, i, j;
    uint32   crps_ingress_counter[8];
    uint32   crps_egress_counter[8];
    uint32        eg_val, ing_val;
    SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD oem1_payload = {0};
    _bcm_dpp_diag_print_hier_count_info_t prt_info[16]; /* Max 16 since inner/outer and PCP */
    int num_info = 0;
    int num_ydvs = 1, ydv;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Not supported in current device \n")));
    }


    if (SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm) {
        num_ydvs = 2;
    }

    for (ydv = 0; ydv < num_ydvs; ydv++) {
        oem1_key->your_discr = ydv;
        soc_sand_rv = arad_pp_oam_classifier_oem1_entry_get_unsafe(unit, oem1_key, &oem1_payload, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        counter_ndx = oem1_payload.counter_ndx;
        if (found) {
            soc_sand_rv = soc_ppd_oam_counter_range_get(unit, &counter_range_min, &counter_range_max);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* Is PCP = true if outside the singleton range or 
             *               if range is not defined at all */
            if (((counter_range_min == 0) && (counter_range_max == 0)) ||
                    (counter_ndx < counter_range_min) ||
                    (counter_ndx > counter_range_max)) {
                is_pcp = 1;
            }
            crps_engine = -1;

            for (i = 0; i < (SOC_DPP_DEFS_GET(unit, nof_counter_processors)); i++) {  /* search 16 counter engines which are available for OAM */
                engine.engine_id = i;
                rv = bcm_stat_counter_config_get(unit, &engine, &config);
                if (rv != BCM_E_EMPTY) {
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Find core */
                    rv = _bcm_dpp_oam_core_from_lif_get(unit, oem1_key->oam_lif, &mep_core);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (((counter_ndx >= config.source.pointer_range.start) && 
                                (counter_ndx <= config.source.pointer_range.end)) &&
                            (mep_core == config.source.core_id)) {
                        /* seek only for oam counter */
                        if (config.source.engine_source == bcmStatCounterSourceIngressOam || config.source.engine_source == bcmStatCounterSourceEgressOam) {
                            crps_engine = i;
                            if (is_pcp) {
                                for (j = 0; j < 8; j++) {
                                    crps_ingress_counter[j] = counter_ndx - config.source.pointer_range.start + j;
                                    /* Since egress gets the actual counter base id - Do a -8*/
                                    crps_egress_counter[j] = counter_ndx - config.source.pointer_range.start + j - 8;
                                    rv = soc_ppd_oam_get_crps_counter(unit, crps_engine, crps_ingress_counter[j], &ing_val);
                                    BCMDNX_IF_ERR_EXIT(rv);
                                    rv = soc_ppd_oam_get_crps_counter(unit, crps_engine, crps_egress_counter[j], &eg_val);
                                    BCMDNX_IF_ERR_EXIT(rv);

                                    prt_info[num_info].oam_lif = oem1_key->oam_lif;
                                    prt_info[num_info].ing     = oem1_key->ingress;
                                    prt_info[num_info].inner_outer = oem1_key->your_discr; 
                                    prt_info[num_info].counter = counter_ndx - 8; /* Since the counter base id is in the egress. */
                                    prt_info[num_info].engine_id = crps_engine;
                                    prt_info[num_info].base_ptr = crps_egress_counter[j];/* Since egress gets the lm_counter_base_id */
                                    prt_info[num_info].ing_value = ing_val;
                                    prt_info[num_info].egr_value = eg_val;
                                    num_info++;
                                }
                            } else {
                                crps_ingress_counter[0] = counter_ndx - config.source.pointer_range.start;
                                /* Since egress gets the actual counter base id - Do a -1*/
                                crps_egress_counter[0] = counter_ndx - config.source.pointer_range.start - 1;
                                rv = soc_ppd_oam_get_crps_counter(unit, crps_engine, crps_ingress_counter[0], &ing_val);
                                BCMDNX_IF_ERR_EXIT(rv);
                                rv = soc_ppd_oam_get_crps_counter(unit, crps_engine, crps_egress_counter[0], &eg_val);
                                BCMDNX_IF_ERR_EXIT(rv);
                                prt_info[num_info].oam_lif = oem1_key->oam_lif;
                                prt_info[num_info].ing     = oem1_key->ingress;
                                prt_info[num_info].inner_outer = oem1_key->your_discr; 
                                prt_info[num_info].counter = counter_ndx - 1; /* Since egress gets the lm_counter_base_id */
                                prt_info[num_info].engine_id = crps_engine ;
                                prt_info[num_info].base_ptr = crps_egress_counter[0]; /* Since egress gets the lm_counter_base_id */
                                prt_info[num_info].ing_value = ing_val;
                                prt_info[num_info].egr_value = eg_val;
                                num_info++;
                            }
                            break;
                        }
                    }
                }
            }
        } 
    }
    rv = _bcm_dpp_oam_diag_print_hier_count_formatted(unit, prt_info, num_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Internal function based LIF and direction
 * Extract all endpoints existing on different MDL levels
 * Update action set profile for all existing MEP&MIP
 * is_passive--
 *             MEP update passive profile
 *             MIP update another active profile drive EP_ID based on active Dir
 *
 * @author Ch Sivakameswara Rao (05/01/2017)
 *
 * @param unit
 * @param lif
 * @param passive_side_lif
 * @param direction (Up/Down)
 * @param Passive/Active
 * @param MIP/MEP
 * @param Profile update cb function
 * @param Updated Profile Information
 *
 * @return STATIC uint32
 */
uint32 bcm_petra_oam_endpoint_on_lif_and_direction( int unit,
                                                SOC_PPC_LIF_ID lif,
                                                SOC_PPC_LIF_ID passive_side_lif,
                                                int is_upmep,
                                                uint8 is_passive,
                                                uint8 is_mip,
                                                bcm_oam_endpoint_traverse_cb cb,
                                                void *user_data)
{

    bcm_oam_endpoint_info_t current_endpoint;
    bcm_oam_endpoint_t      oam_id;
    int                     rv;
    uint8                   mdl, found, found_mip, found_count = 0;

   BCMDNX_INIT_FUNC_DEFS;


   for (mdl = 0; mdl < 8; ++mdl) {
       if (is_passive) {
           if (passive_side_lif == _BCM_OAM_INVALID_LIF) {
               BCM_EXIT;
           }
           rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl,
                                    (unit,
                                     passive_side_lif,
                                     mdl,
                                     is_upmep,
                                     &found_mip,
                                     &found,
                                     &oam_id));
       } else {
           if (lif == _BCM_OAM_INVALID_LIF) {
               BCM_EXIT;
           }
           rv = MBCM_PP_DRIVER_CALL(unit,
                                    mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl,
                                    (unit,
                                     lif,
                                     mdl,
                                     is_upmep,
                                     &found_mip,
                                     &found,
                                     &oam_id));
       }
       BCMDNX_IF_ERR_EXIT(rv);
       /* Found = 2 --- entry exist in OEM-2 Table
        *               OAM-ID Taken from oem-2 payload
        * Found = 1 --- entry exist in OEM-1 Table
        *               Derive the OAM-ID from lif, mdl and dir
        * Found = 0 --- MEP/MIP does not exist on this combination
        */
       if ((found == 2) || (found == 1) || (found ==3)) {

           SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
           uint8 int_found, ep_id_iter, ep_id_match = 0;
           bcm_oam_endpoint_t      ep_id = BCM_OAM_ENDPOINT_INVALID;
            
           /* Non accelerated derive OAM_ID from LIF, MDL and DIR */
           if (found == 1) {
               /*  MIP derive OAM-ID from LIF and MDL
                *  MIP use active lif for both ingress and egress side
                */
               if (found_mip) {
                   if (is_mip) {
                       oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, 0);
                   } else {
                       if (is_upmep) {
                           oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(passive_side_lif, mdl, 0);
                       } else {
                           oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, 0);
                       }
                   }
               } else if (is_passive) {
                   oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(passive_side_lif, mdl, is_upmep);
               } else {
                   oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, is_upmep);
               }
           }

           /* MEP exists in opposite direction */
           if (found == 3) {
                 if (is_passive) {
                    oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(passive_side_lif, mdl, !is_upmep);
                 } else {
                    oam_id = _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, !is_upmep);
                 }
           }

           /* Derive endpoint ID from OAM-ID
            * Accelerated case OAM_ID is EP_ID
            * Non accelerated there are two option
            *    Multiple meps on lif there can two mip derive with below logic
            *    Non Multiple meps on lif OAM_ID is EP_ID
            * Server client case validate LIF from below logic and obtaind EP_ID
            */
           for (ep_id_iter = 0; ep_id_iter <= 1; ++ep_id_iter) {
               ep_id_match = 0;
               if (!_bcm_dpp_oam_bfd_mep_info_db_get(unit, oam_id,
                                                     &mep_info, &int_found)) {
                   if (int_found) {
                       if (found_mip) {
                           if (is_mip) {

                               /* Skip passive profile for MIP if set to SLM_PASSIVE_PROFILE */
                               if (SOC_IS_JERICHO_PLUS(unit) && is_passive && (mep_info.non_acc_profile_passive == ARAD_PP_SLM_PASSIVE_PROFILE)) {
                                   continue;
                               }

                               /* Passed endpoint info is MIP: Compare MIP's active LIF to MIP's active LIF */
                               if (mep_info.lif == lif) {
                                   ep_id_match = 1;
                                   ep_id = oam_id;
                               }
                           } else {
                               if ((!is_upmep && (mep_info.lif == lif)) || /* Passed endpoint info is DownMEP: compare MIP's active LIF to DOWNMEP's active LIF */
                                   (is_upmep && (mep_info.lif == passive_side_lif)) ) { /* Passed endpoint info is UPMEP: compare MIP's active LIF to UPMEP's passive LIF */
                                   ep_id_match = 1;
                                   ep_id = oam_id;
                               }
                           }
                       } else if (is_passive) {
                            if (mep_info.passive_side_lif == passive_side_lif) {
                                ep_id_match = 1;
                                ep_id = oam_id;
                            } else if (is_mip && is_passive) {
                                /* If we are mip and we are trying to switch other MEPs on the same LIF
                                 * to our profile, it should be an UPMEP in which case we should actually
                                 * compare mep_info's active side lif to our MIP's passive side lif.
                                 */
                                if (mep_info.lif == passive_side_lif) {
                                    ep_id_match = 1;
                                    ep_id = oam_id;
                                }
                            }
                       } else {
                           if (mep_info.lif == lif) {
                               ep_id_match = 1;
                               ep_id = oam_id;
                           }
                       }
                   }
                   if (ep_id_match) {
                       bcm_oam_endpoint_info_t_init(&current_endpoint);
                       if (!bcm_petra_oam_endpoint_get(unit, ep_id, &current_endpoint)) {
                           rv = cb(unit, &current_endpoint, user_data);
                           if (rv != 0) {
                               BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG(
                                                   "callback in _bcm_oam_endpoint_exchange_profile_cb returned error message %s"),
                                                   bcm_errmsg(rv)));
                           }
                           if (found == 1) {
                               ++found_count;
                           }
                           break;
                       }
                   }
               }
               oam_id = (1 << SOC_PPD_OAM_SERVER_CLIENT_SIDE_BIT) + oam_id;
           }
       }
   }

   /* MP Profile is been update */
   if (SOC_IS_ARADPLUS(unit) && found_count && !is_passive) {
       for (mdl = 0; mdl < 8; ++mdl) {
           rv = MBCM_PP_DRIVER_CALL(unit,
                   mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl,
                   (unit,
                    lif,
                    mdl,
                    !is_upmep,
                    &found_mip,
                    &found,
                    &oam_id));
           BCMDNX_IF_ERR_EXIT(rv);
           if (found == 2) {
               bcm_oam_endpoint_info_t_init(&current_endpoint);
               if (!bcm_petra_oam_endpoint_get(unit, oam_id, &current_endpoint)) {
                   if (current_endpoint.type == bcmOAMEndpointTypeEthernet) {
                       rv = cb(unit, &current_endpoint, user_data);
                       if (rv != 0) {
                           BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG(
                                               "callback in _bcm_oam_endpoint_exchange_profile_cb returned error message %s"),
                                               bcm_errmsg(rv)));
                       }
                   }
               }
           }
       }
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

