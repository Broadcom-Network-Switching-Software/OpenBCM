/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam_resource.c
 * Purpose: OAM Resources module (traps, profiles, IDs, ...)
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

#define BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(is_upmep, mep_type, destination_trap_code,is_lsp) \
  do { \
	if (is_upmep) { /* Can be only Eth. OAM */ \
			rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_OAMP, &destination_trap_code); \
			BCMDNX_IF_ERR_EXIT(rv); \
    } \
	else { \
		if (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) { \
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP, &destination_trap_code); \
			BCMDNX_IF_ERR_EXIT(rv); \
		} \
		else if ((mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) || (mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL)) { \
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_MPLS, &destination_trap_code); \
			BCMDNX_IF_ERR_EXIT(rv); \
		} \
		else if((mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE)&& is_lsp){\
		  rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE, &destination_trap_code); \
		  BCMDNX_IF_ERR_EXIT(rv); \
		} \
		else /*(classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) */ { \
            rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE, &destination_trap_code); \
			BCMDNX_IF_ERR_EXIT(rv); \
		} \
	} \
  } while (0)

#define BCM_OAM_CPU_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, destination_trap_code) \
  do { \
	if (is_upmep) { \
        rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_CPU, &destination_trap_code); \
        BCMDNX_IF_ERR_EXIT(rv); \
    } \
	else { \
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &destination_trap_code); \
	   BCMDNX_IF_ERR_EXIT(rv); \
    } \
  } while (0)

#define BCM_OAM_RCY_DESTINATION_TRAP_CODE_GET(is_upmep, meta_data, destination_trap_code) \
  do { \
	if (is_upmep) { \
        rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_RECYCLE, &destination_trap_code); \
        BCMDNX_IF_ERR_EXIT(rv); \
    } \
	else { \
       rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &destination_trap_code); \
	   BCMDNX_IF_ERR_EXIT(rv); \
    } \
  } while (0)

#define _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, cpu_trap_code, jer_keep_orig_headers) \
  do { \
      bcm_mirror_destination_t mirror_dest={0}; \
      uint8 found_match = 0; \
      rv = _bcm_dpp_oam_free_mirror_profile_find(unit, &mirror_id, *(cpu_trap_code), &found_match); \
      BCMDNX_IF_ERR_EXIT(rv); \
      if (!found_match) { \
          mirror_dest.sample_rate_dividend = mirror_dest.sample_rate_divisor = 1; \
          mirror_dest.flags = BCM_MIRROR_DEST_WITH_ID; \
          if (SOC_IS_JERICHO(unit) && jer_keep_orig_headers) { \
              mirror_dest.flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER; \
          } \
          BCM_GPORT_MIRROR_MIRROR_SET(mirror_dest.mirror_dest_id, mirror_id); \
          if (_bcm_petra_mirror_or_snoop_destination_create(unit, &mirror_dest, 0) != BCM_E_NONE) { \
              BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: Failed to allocate mirroring destination\n"))); \
          } \
          /* Mark this new mirror profile as internally allocated */ \
          rv = OAM_ACCESS.is_internally_allocated_mirror.set(unit, mirror_id, 1); \
          BCMDNX_IF_ERR_EXIT(rv); \
      } \
  } while (0)

#define _BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)      \
  ((trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_CPU_MIRROR) || \
  (trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_OAMP_MIRROR)  )

#define _BCM_OAM_RX_TRAP_TYPE_TO_PPD_TRAP_TYPE(trap_type, ppd_trap_type) \
  do {   \
	 switch (trap_type) {   \
		case bcmRxTrapOampTrapErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_TRAP_ERR;   \
			break;   \
		case bcmRxTrapOampTypeErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_TYPE_ERR;   \
			break;   \
		case bcmRxTrapOampRmepErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_RMEP_ERR;   \
			break;   \
		case bcmRxTrapOampMaidErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_MAID_ERR;   \
			break;   \
		case bcmRxTrapOampMdlErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_MDL_ERR;  \
			break;   \
		case bcmRxTrapOampCcmIntrvErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_CCM_INTERVAL_ERR;  \
			break;   \
		case bcmRxTrapOampMyDiscErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_MY_DISC_ERR;   \
			break;   \
		case bcmRxTrapOampSrcIpErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_SRC_IP_ERR;   \
			break;   \
		case bcmRxTrapOampYourDiscErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_YOUR_DISC_ERR;   \
			break;   \
		case bcmRxTrapOampSrcPortErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_SRC_PORT_ERR;   \
			break;   \
		case bcmRxTrapOampRmepStateChange:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_RMEP_STATE_CHANGE;  \
			break;   \
		case bcmRxTrapOampParityErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_PARITY_ERR;  \
			break;   \
		case bcmRxTrapOampTimestampErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_TIMESTAMP_ERR;  \
			break;   \
		case bcmRxTrapOampProtection:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_PROTECTION;  \
			break;   \
		case bcmRxTrapOampChanTypeMissErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_CHANNEL_TYPE_ERR;  \
			break;   \
		case bcmRxTrapOampFlexCrcMissErr:   \
			ppd_trap_type = SOC_PPC_OAM_OAMP_FLEX_CRC_ERR;  \
      break;   \
    case bcmRxTrapOampMplsLmDmErr:   \
      ppd_trap_type = SOC_PPC_OAM_OAMP_RFC_PUNT_ERR;  \
			break;   \
		default:   \
			BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OAMP trap type is not supported")));   \
	}   \
  } while (0)

int _bcm_dpp_bfd_mep_id_alloc(int unit, uint32 flags, uint32 *mep_index) {
   int rv;

   if (SOC_IS_QAX(unit)) {
       /* In QAX there's no limit on the MEP ID in regard to the MAID format. */
       return bcm_dpp_am_qax_oam_mep_id_alloc(unit, flags, 0, mep_index);
   }  else if ( _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        /* In order to make sure DMM scan rate, Jumbo DM steps is 16.
          Jumbo TLV needs to 1k MEPs as well */
          if (flags == SHR_RES_ALLOC_WITH_ID) {
              if (*mep_index % 16 == 0) { /* Jumbo every 16 MEP */
                     rv = bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(unit, flags, mep_index);
              } else {
                    rv = bcm_dpp_am_oam_mep_id_no_jumbo_tlv_alloc(unit, flags, mep_index);
              }
              return rv;
          }
          else{/*allocate id*/
              rv = bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(unit, flags, mep_index);
              return rv;
          }
  } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0))  {
       if (flags == SHR_RES_ALLOC_WITH_ID) {
           if (*mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
               /* Long ID MEPs are limited to the size of UMC_TABLE */
               if (*mep_index % 4 == 0) {
                   rv = bcm_dpp_am_oam_mep_id_long_alloc_48_maid(unit, flags, mep_index);
               } else {
                   rv = bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(unit, flags, mep_index);
               }
           } else {
               rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
           }
       } else {
           rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, 1, 0, 0, mep_index);
       }
   } else {
       if (flags == SHR_RES_ALLOC_WITH_ID) {
           if (*mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
               /* Long ID MEPs are limited to the size of UMC_TABLE */
               rv = bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index);
           } else {
               /* Short ID MEPs are not limited */
               rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
           }
       } else {
           rv = _bcm_dpp_oam_mep_id_alloc(unit, flags, 1, 0, 0, mep_index);
       }
   }
   return rv;
}
     

/*
 * Function:
 *      _bcm_dpp_oam_mep_id_alloc
 *
 * Purpose:
 *      Allocate an index in the MEP-DB
 *
 * Parameters:
 *      unit       - (IN)     SOC unit number.
 *      flags      - (IN)     SW_STATE_RES_ALLOC_WITH_ID or 0
 *      is_short   - (IN)     1 indicates MAID uses short MA format
 *      is_maid_48 - (IN)     1 indicates MAID uses 48B MA format. used only for Jericho\Arad+ in order to allocate
 *                            specifec MEP ID which is valid for 48B maid(every 8th MEP)
 *      type       - (IN)     BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP or 0. Indicates if this is MEP entry or a LM/DM entry
 *      mep_index  - (IN/OUT) Index in the MEP-DB
 *
 * Returns:
 *      BCM_E_XXX
 *
 * Notes:
 *      When WITH_ID flag is set, is_short parameter is ignored
 */
int _bcm_dpp_oam_mep_id_alloc(int unit, uint32 flags, uint8 is_short, uint8 is_maid_48, uint8 type, uint32 *mep_index) {
    int rv;
    int max_db_entries = SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit); /* JER has 16K mep entries*/
    if (SOC_IS_QAX(unit)) {
        /* In QAX there's no limit on the MEP ID in regard to the MAID format. */
        return bcm_dpp_am_qax_oam_mep_id_alloc(unit, flags, type, mep_index);
    } else if ( _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        /* In order to make sure DMM scan rate, Jumbo DM steps is 16.
          Jumbo TLV needs to 1k MEPs as well */
          if (flags == SHR_RES_ALLOC_WITH_ID) {
              if (*mep_index % 16 == 0) { /* Jumbo every 16 MEP */
                     rv = bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(unit, flags, mep_index);
              } else {
                    rv = bcm_dpp_am_oam_mep_id_no_jumbo_tlv_alloc(unit, flags, mep_index);
              }
          }
          else{/*allocate id*/
              rv = bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(unit, flags, mep_index);
          }
          /*last entry need to resered entries for LM/DM*/
          if(*mep_index == max_db_entries){
              *mep_index = *mep_index-4;
          }
          return rv;
    } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
        if (flags == SHR_RES_ALLOC_WITH_ID) {
            if (*mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
                /* Long ID MEPs are limited to the size of UMC_TABLE */
                if (*mep_index % 4 == 0) { /* 48B MAID every 4th MEP */
                    rv = bcm_dpp_am_oam_mep_id_long_alloc_48_maid(unit, flags, mep_index);
                } else {
                    rv = bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(unit, flags, mep_index);
                }
            } else {
                rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);

            }
            return rv;
        }
        if (is_short) { /*  only for short MAID MEPs*/
            rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
            if (rv == BCM_E_NONE) {
                return rv;
            }
        }
        if (!is_maid_48) { /* only for long or shorts  MAID MEPs */
            rv =  bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(unit, flags, mep_index);
            if (rv == BCM_E_NONE) {
                return rv;
            }
        }
        rv = bcm_dpp_am_oam_mep_id_long_alloc_48_maid(unit, flags, mep_index);
        return rv;

    } else {
        if (flags == SHR_RES_ALLOC_WITH_ID) {
            return (*mep_index < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) ?
                   bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index) :  bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
        }

        if (is_short) {
            rv = bcm_dpp_am_oam_mep_id_short_alloc(unit, flags, mep_index);
            if (rv == BCM_E_NONE) {
                return rv;
            }
        }
        return  bcm_dpp_am_oam_mep_id_long_alloc(unit, flags, mep_index);
    }
}


int _bcm_dpp_oam_bfd_mep_id_is_alloced(int unit, uint32 mep_id) {
    int max_db_entries = SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)-4; /* actually max ccm entry*/

    if (SOC_IS_QAX(unit)) {
        return bcm_dpp_am_qax_oam_mep_id_is_alloced(unit, mep_id);
    } else if ( _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        if(mep_id == max_db_entries){
            mep_id = mep_id + 4;
        }
        /* In order to make sure DMM scan rate, Jumbo DM steps is 16.
        Jumbo TLV needs to 1k MEPs as well */
        if (mep_id % 16 == 0) { /* Jumbo every 16 MEP */
               return bcm_dpp_am_oam_mep_id_jumbo_tlv_is_alloced(unit, mep_id);
        } else {
              return bcm_dpp_am_oam_mep_id_no_jumbo_tlv_is_alloced(unit, mep_id);
        }
    } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
        if (mep_id < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
            if (mep_id % 4 == 0) {
                return bcm_dpp_am_oam_mep_id_long_is_alloced_48_maid(unit, mep_id);
            } else {
                return bcm_dpp_am_oam_mep_id_long_is_alloced_non_48_maid(unit, mep_id);
            }
        } else {
            return bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);

        }
    } else {
        if (mep_id < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
            return bcm_dpp_am_oam_mep_id_long_is_alloced(unit, mep_id);
        } else {
            return bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);

        }
    }
}

int _bcm_dpp_oam_bfd_mep_id_dealloc(int unit, uint32 mep_id) {
    int rv;
    int max_db_entries = SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)-4; /* actually max ccm entry*/
    if (SOC_IS_QAX(unit)) {
        rv = bcm_dpp_am_qax_oam_mep_id_is_alloced(unit, mep_id);
        if (rv == BCM_E_EXISTS) {
            return bcm_dpp_am_qax_oam_mep_id_dealloc(unit, mep_id);
        }
    } else if ( _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
        /* In order to make sure DMM scan rate, Jumbo DM steps is 16.
          Jumbo TLV needs to 1k MEPs as well */
        if(mep_id == max_db_entries){
            mep_id = mep_id + 4;
        }
        if (mep_id % 16 == 0) { /* Jumbo every 16 MEP */
            rv = bcm_dpp_am_oam_mep_id_jumbo_tlv_is_alloced(unit, mep_id);
            if (rv == BCM_E_EXISTS) {
                return bcm_dpp_am_oam_mep_id_jumbo_tlv_dealloc(unit, mep_id);
            }
        } else {
            rv = bcm_dpp_am_oam_mep_id_no_jumbo_tlv_is_alloced(unit, mep_id);
            if (rv == BCM_E_EXISTS) {
                return bcm_dpp_am_oam_mep_id_no_jumbo_tlv_dealloc(unit, mep_id);
            }
        }
        return rv;
    } else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
        if (mep_id < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
            if (mep_id % 4 == 0) {
                rv = bcm_dpp_am_oam_mep_id_long_is_alloced_48_maid(unit, mep_id);
                if (rv == BCM_E_EXISTS) {
                    return bcm_dpp_am_oam_mep_id_long_dealloc_48_maid(unit, mep_id);
                }
            } else {
                rv = bcm_dpp_am_oam_mep_id_long_is_alloced_non_48_maid(unit, mep_id);
                if (rv == BCM_E_EXISTS) {
                    return bcm_dpp_am_oam_mep_id_long_dealloc_non_48_maid(unit, mep_id);
                }
            }
        } else {
            rv = bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);
            if (rv == BCM_E_EXISTS) {
                return bcm_dpp_am_oam_mep_id_short_dealloc(unit, mep_id);
            }
        }
    } else {
        if (mep_id < SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) {
            rv = bcm_dpp_am_oam_mep_id_long_is_alloced(unit, mep_id);
            if (rv == BCM_E_EXISTS) {
                return bcm_dpp_am_oam_mep_id_long_dealloc(unit, mep_id);
            }
        } else {
            rv = bcm_dpp_am_oam_mep_id_short_is_alloced(unit, mep_id);
            if (rv == BCM_E_EXISTS) {
                return bcm_dpp_am_oam_mep_id_short_dealloc(unit, mep_id);
            }
        }
    }

    return BCM_E_PARAM;
}

int _bcm_oam_default_profile_get(int unit,
                                 SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
                                 SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                 uint8 is_accelerated,
                                 uint8 is_default,
                                 uint8 is_upmep) {
   uint32 opcode;
   uint32 upmep_trap_id_cpu;
   uint32 trap_id_cpu;
   uint32 trap_id_err_level, trap_id_snoop, trap_id_recycle;
   uint8 internal_opcode;
   int rv;
   uint8 ccm_pkt_counting_enable = 0;

   BCMDNX_INIT_FUNC_DEFS;

   if (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)
       || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {
      profile_data->is_default = is_default;
   } else {
      if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)) {
         /* New Arad+ Classifier: Setting the mp_type_passive_active_mix to 1 enables MIP Level filtering.
            Go by soc property*/
         profile_data->mp_type_passive_active_mix = SOC_DPP_CONFIG(unit)->pp.oam_mip_level_filtering;
      } else {
         profile_data->mp_type_passive_active_mix = (!_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry)) ? 1 : 0;
      }
   }

   profile_data->is_1588 = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0);

   profile_data->flags = (classifier_mep_entry->counter != 0) ? SOC_PPC_OAM_LIF_PROFILE_FLAG_COUNTED : 0;

   rv = OAM_ACCESS.trap_info.upmep_trap_ids.get(unit, SOC_PPC_OAM_UPMEP_TRAP_ID_CPU, &upmep_trap_id_cpu);
   BCMDNX_IF_ERR_EXIT(rv);
   rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &trap_id_cpu);
   BCMDNX_IF_ERR_EXIT(rv);
   rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_ERR_LEVEL, &trap_id_err_level);
   BCMDNX_IF_ERR_EXIT(rv);

   rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_SNOOP, &trap_id_snoop);
   BCMDNX_IF_ERR_EXIT(rv);
   rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_id_recycle);
   BCMDNX_IF_ERR_EXIT(rv);

   if (soc_property_get(unit, spn_OAM_RCY_PORT, -1) == -1) {
      /* Recycle port disabled, forward LBMs to CPU*/
      trap_id_recycle = trap_id_cpu;
   } else {
      rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE, &trap_id_recycle);
      BCMDNX_IF_ERR_EXIT(rv);
   }

   ccm_pkt_counting_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_ccm_pkt_counting", 0);
   /* configure non accelerated */
   for (opcode = 1; opcode < SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT; opcode++) {
      rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcode, &internal_opcode);
      BCMDNX_IF_ERR_EXIT(rv);
      /* configure counter disable */
      SHR_BITSET(profile_data->mep_profile_data.counter_disable, internal_opcode);
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
         if ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR) ||
             (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM)) {
            SHR_BITCLR(profile_data->mep_profile_data.counter_disable, internal_opcode);
         }
      }
      if (SOC_IS_JERICHO_PLUS(unit)) {
          if ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM)) {
             SHR_BITCLR(profile_data->mep_profile_data.counter_disable, internal_opcode);
          }
      }

      if ((ccm_pkt_counting_enable) &&
          (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM)) {
          SHR_BITCLR(profile_data->mep_profile_data.counter_disable, internal_opcode);
      }

      /* configure meter disable */
      SHR_BITSET(profile_data->mep_profile_data.meter_disable, internal_opcode);

      /* configure multicast behavior */
      if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
         if ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM) ||
             (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) ||
             (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_AIS) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_TST) ||
             (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM) ||
             (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CSF) || (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM))   { /* These opcodes trap in multicast */
            /* All supported multicast packets are trapped to the CPU.*/
            if (is_upmep) {
               profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = upmep_trap_id_cpu;
            } else {
               profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
            }
         } else {
            /* Non supported opcode: trap to CPU with error level.*/
            profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_err_level;
         }
      } else { /* MPLS and PWE: Only MC packets supported */
         profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
      }

      /* configure unicast behavior */
      if (classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM) {
         if (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) {
            if (is_upmep) {
                    /* if up mep LBM is enabled */
                    if ( SOC_DPP_CONFIG(unit)->pp.upmep_lbm_is_configured ){
                        uint32 trap_id_lbm;
                        rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE_LBM, &trap_id_lbm);
                        BCMDNX_IF_ERR_EXIT(rv);
                        profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_lbm;
                    } else {
                    profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = upmep_trap_id_cpu;
                    }
            } else {
               /* recycle */
               profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_recycle;
            }
         } else if (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) {
            profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
         } else {
            if (is_upmep) {
               
               profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = upmep_trap_id_cpu;

            } else {
               profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_cpu;
            }
         }

      } else { /* MPLS and PWE: Only MC packets supported */
         profile_data->mep_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
      }

      /* configure MIP: Only LBM and LTM are handled in MIP */
      if ((opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM) ||
          (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM) ||
          (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM)) {
         SHR_BITSET(profile_data->mip_profile_data.counter_disable, internal_opcode);
         SHR_BITSET(profile_data->mip_profile_data.meter_disable, internal_opcode);

         if (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM) {
            profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_cpu;
            /* 
               LTMs may be either snooped (in which case they are forwarded without updating the TTL) or trapped,
               where the CPU will be responsible for forwarding. It can done by custom_feature_oam_mip_snoop_multicast_ltm:
               costum_feature_oam_mip_snoop_multicast_ltm = 0 : trap to cpu
               costum_feature_oam_mip_snoop_multicast_ltm = 1 : snoop (default)
            */
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mip_snoop_multicast_ltm", 1)) {
               profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_snoop;
            } else {
               profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
            }
         } else {
            if (opcode == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM) {
               profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
               /* 
                  According to 802.1ag CCMs may be optionally be snooped by MIPs. It can done by custom_feature_oam_mip_snoop_multicast_ccm:
                  costum_feature_oam_mip_snoop_multicast_ccm = 0 : forward (default)
                  costum_feature_oam_mip_snoop_multicast_ccm = 1 : snoop
               */
               if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mip_snoop_multicast_ccm", 0)) {
                  profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_snoop;
               } else {
                  profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = _BCM_PETRA_UD_DFLT_TRAP; /* Forward */
               }
            } else {
               /* recycle */
               profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_recycle;
               profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = _BCM_PETRA_UD_DFLT_TRAP; /* Forward */
            }
         }
      } else { /* MIPs are transparent to all but LTM, LBM. forward the M-C packets, drop  (trap with err level) the U-C paclkets*/
         SHR_BITSET(profile_data->mip_profile_data.counter_disable, internal_opcode);
         profile_data->mip_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = _BCM_PETRA_UD_DFLT_TRAP; /* Forward */
         profile_data->mip_profile_data.opcode_to_trap_code_unicast_map[internal_opcode] = trap_id_err_level;
      }
   }
   /* Configure trap OAMP destination if packet is accelerated (only multicast) */
   if (is_accelerated) {
      BCM_OAM_OAMP_DESTINATION_TRAP_CODE_GET(
         is_upmep, classifier_mep_entry->mep_type, profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM],(classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP));
   }

   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

/* Configure default accelerated/non-accelerated profile for RFC6374 endpoints */
int _bcm_oam_rfc6374_default_profile_get(int unit,
                                         SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
                                         SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry,
                                         uint8 is_accelerated) {
  int rv;
  int opcode_index;
  int opcode_count = 3;
  uint32 opcodes[] = {SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM};
  uint32 trap_id_cpu;
  uint8 internal_opcode;

  BCMDNX_INIT_FUNC_DEFS;

  if (!is_accelerated) {
    /* Relevant only for OEM-1 table configuration */
    profile_data->flags = (classifier_mep_entry->counter != 0) ? SOC_PPC_OAM_LIF_PROFILE_FLAG_COUNTED : 0;
  }

  profile_data->is_1588 = ((classifier_mep_entry->flags & SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588) != 0);

  rv = OAM_ACCESS.trap_info.trap_ids.get(unit, SOC_PPC_OAM_TRAP_ID_CPU, &trap_id_cpu);
  BCMDNX_IF_ERR_EXIT(rv);

  for (opcode_index = 0; opcode_index < opcode_count; ++opcode_index) {
    rv = OAM_ACCESS.y1731_opcode_to_intenal_opcode_map.get(unit, opcodes[opcode_index], &internal_opcode);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Configure counter disable */
    SHR_BITSET(profile_data->mep_profile_data.counter_disable, internal_opcode);

    /**
     * LMMs and related packets are always counted according to the COUNTER_INCREMENT register (handled separately),
     * other packets through the counter disable field.
     */
    if ((opcodes[opcode_index] == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM) || (opcodes[opcode_index] == SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM)) {
      /* Counter disable must be set to 0 to allow stamping for DLM and ILM */
      SHR_BITCLR(profile_data->mep_profile_data.counter_disable, internal_opcode);
    }

    /* Configure meter disable */
    SHR_BITSET(profile_data->mep_profile_data.meter_disable, internal_opcode);

    /* Only MC packets are supported */
    profile_data->mep_profile_data.opcode_to_trap_code_multicast_map[internal_opcode] = trap_id_cpu;
  }

exit:
   BCMDNX_FUNC_RETURN;
}

/**
 * Function finds a mirror profile, first searching for one
 * where the CPU trap code matches the given paramter and then a
 * new one.
 *
 * @param unit
 * @param free_mirror_profile
 * @param cpu_trap_code
 * @param found_match
 *
 * @return int
 */
int _bcm_dpp_oam_free_mirror_profile_find(int unit, uint32 *free_mirror_profile, uint32 cpu_trap_code, uint8 *found_match)
{
    int32 mirror_id;
    uint32 trap_code, ref_count;
    int rv;
    uint32 soc_sand_rv = SOC_SAND_OK;

    BCMDNX_INIT_FUNC_DEFS;

    *found_match = 0;
    *free_mirror_profile = DPP_MIRROR_ACTION_NDX_MAX + 1;

    for (mirror_id = DPP_MIRROR_ACTION_NDX_MAX; mirror_id > 0; mirror_id--) {
        rv = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit, mirror_id, &ref_count);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!ref_count) {
            if (*free_mirror_profile > DPP_MIRROR_ACTION_NDX_MAX) {
                /* first free mirror profile found */
                *free_mirror_profile = mirror_id;
            }
        } else {
            soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_get(unit, mirror_id, &trap_code, &ref_count/*dummy*/);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (trap_code == cpu_trap_code) {
                /* first matching mirror profile found */
                *free_mirror_profile = mirror_id;
                *found_match = 1;
                break;
            }
        }
    }

    if (*free_mirror_profile > DPP_MIRROR_ACTION_NDX_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                            (_BSL_BCM_MSG("Error: No free mirror profile found\n")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_mirror_data_free(int unit, uint32 trap_code) {
    uint32 rv, soc_sand_rv = SOC_SAND_OK;
    uint8  mirror_profile, is_internal_mirror;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS(unit)) {
        /* If this trap is used by oamp_rx_trap_codes[1-3] or the TCAM in QAX, delete it from there */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_ETH_OAM, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_delete, (unit, SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP, trap_code));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.get(unit, trap_code, &mirror_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mirror_profile) {
        /* Mirror profile exists for this trap */
        rv = OAM_ACCESS.is_internally_allocated_mirror.get(unit, mirror_profile, &is_internal_mirror);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_internal_mirror) {
            /* Mirror profile was previously created internally (by action_set) */
            int is_last;

            /* Free the mirror profile */
            rv = _bcm_dpp_am_template_mirror_action_profile_free(unit, mirror_profile, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_last) {
                soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile, 0, 0, 7);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            rv = OAM_ACCESS.is_internally_allocated_mirror.set(unit, mirror_profile, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, 0);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocates a mirror_profile if not yet allocated */
/* Call with is_snoop paramater to add original headers through mirror configuration. This should be done for snooped traps.
   (recall that for all other cases this is generally done through the prge)*/
STATIC int _bcm_dpp_oam_map_trap_code_to_mirror_profile(int unit, uint32 trap_code, int is_snoop) {
    uint32 mirror_profile, snoop_strength;
    uint32 soc_sand_rv = SOC_SAND_OK;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* Allocates a mirror_profile if not yet allocated */
    _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_profile, &trap_code, is_snoop);

    snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);

    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit, mirror_profile, trap_code, snoop_strength, 7);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Adding reverse mapping of trap code to mirror profile */
    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, mirror_profile);
    BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Handles A+ advanced egress snooping */
STATIC int _bcm_dpp_oam_mip_adv_snoop_set(int unit, uint32 trap_code, bcm_rx_trap_config_t* trap_config) {
    uint32 mirror_profile;
    int trap_id_sw;
    int trap_id;
    bcm_rx_snoop_config_t *snoop_config = NULL;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(snoop_config, sizeof(*snoop_config),"_bcm_dpp_oam_mip_adv_snoop_set.snoop_config");
    if (snoop_config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    /* In this case the user presumably set a trap for at the ingress (down). Setting a trap with the same configurations at the egress*/
    bcm_rx_snoop_config_t_init(snoop_config);
    rv = bcm_petra_rx_snoop_get(unit, _BCM_OAM_MIP_SNOOP_CMND_DOWN, snoop_config);
    BCMDNX_IF_ERR_EXIT(rv);

    rv =  bcm_petra_rx_snoop_set(unit, _BCM_OAM_MIP_SNOOP_CMND_UP, snoop_config);
    BCMDNX_IF_ERR_EXIT(rv);

    trap_config->snoop_cmnd = _BCM_OAM_MIP_SNOOP_CMND_UP;
    /* set the trap with the same values set by the user. In the other direction*/
    /*use the same trap code that was used for default MIPs */
    trap_id_sw = SOC_PPC_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_id));
    rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Use the same mirror profile as the one allocated by init. */
    rv = OAM_ACCESS.trap_info.mirror_ids.get(unit, SOC_PPC_OAM_MIRROR_ID_SNOOP, &mirror_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code, mirror_profile);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCM_FREE(snoop_config);
    BCMDNX_FUNC_RETURN;
}

/**
 * given a trap code (SW id) return the trap config (optionally) 
 * and whether or not that trap config is associated with a 
 * snoop. 
 * 
 * @author sinai (17/05/2016)
 * 
 * @param unit 
 * @param trap_code 
 * @param is_snoop 
 * 
 * @return int 
 */
static int _bcm_dpp_oam_is_trap_snoop(int unit, bcm_gport_t trap_code ,bcm_rx_trap_config_t * trap_config, int * is_snoop) {
    int rv = BCM_E_NONE;
    int trap_code_sw, trap_code_hw;
    bcm_rx_trap_config_t trap_config_local;
    bcm_rx_trap_config_t * trap_config_pointer =trap_config? trap_config : &trap_config_local ;
    BCMDNX_INIT_FUNC_DEFS;
                                     
    trap_code_sw = BCM_GPORT_TRAP_GET_ID(trap_code);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit, trap_code_sw, &trap_code_hw));

    /* Get the trap */
    bcm_rx_trap_config_t_init(trap_config_pointer);
    rv = bcm_petra_rx_trap_get(unit, trap_code_hw, trap_config_pointer);
    BCMDNX_IF_ERR_EXIT(rv);

    *is_snoop = trap_config_pointer->snoop_cmnd > 0; 

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Compare trap destination gport with oamp gport.
 * As the local device does not know about the
 * remote OAMP port details,it is not needed to
 * add checks on module id.Compare only the port values
 *
 * @param unit
 * @param dest_gport
 * @param is_equal
 *
 * @return int
 */
static int _bcm_dpp_oamp_gport_compare(int unit, bcm_gport_t dest_gport, uint8 * is_equal) {
    int rv = BCM_E_NONE;
    int count_erp, dest_port, core = SOC_CORE_INVALID;
    uint32 oamp_port_1;
#if SOC_DPP_DEFS_MAX(NOF_CORES) > 1
    uint32 oamp_port_2;
#endif
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    _bcm_dpp_gport_info_t gport_info;
    SOC_SAND_PP_DESTINATION_ID dest_id1, dest_id2;
#if SOC_DPP_DEFS_MAX(NOF_CORES) > 1
    SOC_SAND_PP_DESTINATION_ID dest_id3;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_equal);
    dest_port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);

    /* Get oamp ports */
    rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_to_phy_port(unit, oamp_port[0], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&oamp_port_1,&core)));
    /* Get pp destination id for gports */
    rv = _bcm_dpp_gport_to_sand_pp_dest(unit, oamp_port[0], &dest_id1);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_to_sand_pp_dest(unit, dest_gport, &dest_id2);
    BCMDNX_IF_ERR_EXIT(rv);

    /* compare oamp port with dest port for all cores */
    if (count_erp == 1) {
        *is_equal = ((oamp_port_1 == dest_port) && (dest_id1.dest_type == dest_id2.dest_type));
    }

#if SOC_DPP_DEFS_MAX(NOF_CORES) > 1
    if (count_erp == 2) {
        rv = _bcm_dpp_gport_to_phy_port(unit, oamp_port[1], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port,&oamp_port_2,&core)));
        rv = _bcm_dpp_gport_to_sand_pp_dest(unit, oamp_port[1], &dest_id3);
        BCMDNX_IF_ERR_EXIT(rv);

        *is_equal = ((oamp_port_1 == dest_port) && (oamp_port_2 == dest_port) &&
                (dest_id1.dest_type == dest_id2.dest_type) && (dest_id3.dest_type == dest_id2.dest_type));
    }
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

/* Gets the trap codes from the destination gports, Sets mirror profile if required, Handles advanced egress snooping,
   Sets OAMP RX trap if required */
int _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(int unit, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY* classifier_mep_entry,
                                                          uint8 is_rfc6374_lm, uint8 is_rfc6374_dm,
                                                          bcm_gport_t destination, bcm_gport_t destination2,
                                                          uint32* trap_code, uint32* trap_code2) {
    uint8 is_mip, is_upmep,  is_equal, is_advanced_egress_snooping;
    bcm_rx_trap_config_t *trap_config = NULL;
    bcm_gport_t oamp_port[SOC_DPP_DEFS_MAX(NOF_CORES)];
    int count_erp;
    SOC_PPC_OAM_MEP_TYPE mep_type;

    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    is_advanced_egress_snooping = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0);

    is_mip = _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(classifier_mep_entry);
    is_upmep = _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(classifier_mep_entry);

    BCMDNX_ALLOC(trap_config, sizeof(*trap_config),"_bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action.trap_config");
    if (trap_config == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    if (destination != BCM_GPORT_INVALID) {
        /* if destination is valid, it's a trap */
        int is_snoop;
        *trap_code = BCM_GPORT_TRAP_GET_ID(destination);

        _bcm_dpp_oam_is_trap_snoop(unit, destination, trap_config, &is_snoop);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_snoop && is_mip && is_advanced_egress_snooping) {
            /* Arad+ MIP with advanced egress snooping */
            rv = _bcm_dpp_oam_mip_adv_snoop_set(unit, *trap_code, trap_config);
            BCMDNX_IF_ERR_EXIT(rv);
            /* Advanced egress snooping special case, using same trap_code for both directions */
            *trap_code2 = *trap_code;
        }

        if (is_upmep) {
            /* If the destination is the OAMP, trap code should be added to the OAMP rx trap code collection */
            rv = _bcm_dpp_oamp_gport_compare(unit, trap_config->dest_port, &is_equal);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Allocate mirror profile if required */
            if (is_equal) {
                rv = _bcm_dpp_oam_map_trap_code_to_mirror_profile(unit, *trap_code,0);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                rv = _bcm_dpp_oam_map_trap_code_to_mirror_profile(unit, *trap_code,is_snoop);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            /* If the destination is the OAMP, trap code should be added to the OAMP rx trap code collection */
            rv = bcm_petra_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, _BCM_OAM_NUM_OAMP_PORTS(unit), oamp_port, &count_erp);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = _bcm_dpp_gport_compare(unit, oamp_port[0], trap_config->dest_port, _bcmDppGportCompareTypeSystemPort, &is_equal);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_equal) {
                if (classifier_mep_entry->is_rfc_6374_entry) {
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_soc_jer_oamp_rfc6374_rx_trap_codes_set, (unit, is_rfc6374_lm, is_rfc6374_dm, *trap_code));
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    if(classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS){
                        mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP;
                    } else if((classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1711_PWE) ||(classifier_mep_entry->mep_type == SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION)){
                        mep_type = SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE;
                    } else {
                        mep_type = classifier_mep_entry->mep_type ;
                    }
                    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oamp_rx_trap_codes_set, (unit, mep_type, *trap_code));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
    }

    if (is_mip && !is_advanced_egress_snooping && destination2 != BCM_GPORT_INVALID) {
        /* destination2 is invalid for advanced egress snooping */
        /* This is the egress-catch direction */
        int is_snoop;
        *trap_code2 = BCM_GPORT_TRAP_GET_ID(destination2);

        _bcm_dpp_oam_is_trap_snoop(unit, destination2, NULL, &is_snoop);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Allocate mirror profile if required */
        rv = _bcm_dpp_oam_map_trap_code_to_mirror_profile(unit, *trap_code2, is_snoop);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCM_FREE(trap_config);
    BCMDNX_FUNC_RETURN;
}

/* only for upmep LBM - create trap to a dedicated rcy port, allocate mirror profile , map the mirror command to the trap*/
int _bcm_oam_trap_and_mirror_profile_up_lbm_set(int unit, bcm_rx_trap_config_t * trap_config){
    int rv;
    uint32 trap_id;
    int trap_id_sw;
    uint32 mirror_id=0;
	uint8 mirror_id_wb;
    uint32 soc_sand_rv=0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, (int*)&trap_id);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
    BCMDNX_IF_ERR_EXIT(rv);

     /* we got the trap id from bcm rx trap type create which returns the HW id-> we need to convert it to the SW id*/
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_hw_id_to_sw_id(unit,trap_id, &trap_id_sw));

    rv = OAM_ACCESS.trap_info.trap_ids.set(unit, SOC_PPC_OAM_TRAP_ID_RECYCLE_LBM, trap_id_sw);
    BCMDNX_IF_ERR_EXIT(rv);

    /* allocate mirror profile */
    _BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, &trap_id_sw, 0 );
    mirror_id_wb = (uint8)mirror_id;

    rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_id_sw, mirror_id_wb);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit,
																 mirror_id,
																 trap_id_sw,
																 0 /*snoop_strength*/,
                                                                 7 /*forward_strength*/);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_oam_trap_and_mirror_profile_set(int unit,
                                             SOC_PPC_OAM_TRAP_ID trap_index,
                                             SOC_PPC_OAM_UPMEP_TRAP_ID upmep_trap_index,
                                             bcm_rx_trap_config_t * trap_config,
                                             uint32 trap_code,
                                             uint32 trap_code_upmep,
                                             SOC_PPC_OAM_MIRROR_ID mirror_ndx,
                                             uint8 set_upmep) {
    int rv;
    uint32 soc_sand_rv=0;
    uint32 snoop_strength = 0;
    uint32 forward_strength = 7;
    uint32 trap_id;
    uint32 mirror_id=0;
    uint8 mirror_id_wb;
    uint8 is_bfd_init;
    uint32 mirror_ind_uint32 = mirror_ndx;
    int trap_id_sw;
    uint8 got_into_switch_flag;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_ACCESS.trap_info.trap_ids.get(unit, trap_index, &trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    _BCM_BFD_ENABLED_GET(is_bfd_init);

    got_into_switch_flag = 0;
    trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
    trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
    /*creating trap id*/
    switch (trap_index) {
    case SOC_PPC_OAM_TRAP_ID_OAMP:
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamEthAccelerated, (int*)&trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_MPLS:
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731MplsTp, (int*)&trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE:
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamY1731Pwe, (int*)&trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPC_OAM_TRAP_ID_ERR_LEVEL:
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamLevel, (int*)&trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
	case SOC_PPC_OAM_TRAP_ID_ERR_PASSIVE:
        rv = bcm_petra_rx_trap_type_create(unit, 0, bcmRxTrapOamPassive, (int*)&trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = bcm_petra_rx_trap_set(unit, trap_id, trap_config);
        BCMDNX_IF_ERR_EXIT(rv);
        got_into_switch_flag = 1;
        break;
    default:
        if ((trap_index != SOC_PPC_OAM_TRAP_ID_CPU) || (!is_bfd_init)) { /* bfd init already created cpu trap */
            int trap_id_sw;
            int trap_code_converted;

            trap_id_sw = trap_code;
            got_into_switch_flag = 1;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
            trap_id = trap_code_converted;
            rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int *)&trap_code_converted);
            BCMDNX_IF_ERR_EXIT(rv);
            if ((trap_index == SOC_PPC_OAM_TRAP_ID_RECYCLE) || (trap_index == SOC_PPC_OAM_TRAP_ID_SNOOP)) { /* Snooped and recycled packets don't need OAM offset index */
                trap_config->flags &= ~BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config->forwarding_header = 0;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {

                trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
                rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    /* In this case the trap id holds the HW id - we need to convert it to SW id.*/
    if( got_into_switch_flag == 1 ) {
        /* we got the trap id from bcm rx trap type create which returns the HW id-> we need to convert it to the SW id*/
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_hw_id_to_sw_id(unit,trap_id, &trap_id_sw));
    } else {
        trap_id_sw = trap_id;
    }
    trap_id = trap_id_sw;
    rv = OAM_ACCESS.trap_info.trap_ids.set(unit, trap_index, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

	if (set_upmep) {
		/*creating trap id for egress*/
		if (_BCM_OAM_TRAP_CODE_UPMEP_FTMH(trap_code_upmep)) {
            int trap_id_sw;
            int trap_code_converted;
			/* These trap codes are already allocated - no need to call trap_create, just to indicate that they are used */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);

            trap_id_sw = trap_code_upmep;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
            trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
		}
		else if (trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_CPU_UP) {
            int trap_id_sw;
            int trap_code_converted;

            trap_id_sw = trap_code_upmep;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_code_converted));
			/* This is a new trap code we have to create */
			rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, (int*)&trap_code_converted);
			BCMDNX_IF_ERR_EXIT(rv);
            trap_config->flags |= BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
			BCMDNX_IF_ERR_EXIT(rv);
		}
        else if (SOC_IS_JERICHO(unit) && _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep)) {
            /* Using a PMF program to prevent the outer FTMH from being created. Need to use a trap code that's suitable
               for this program's selection */

            int trap_code_converted;

            /* Get suitable trap code */
			rv = bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(unit, SHR_RES_ALLOC_WITH_ID, &trap_code_upmep);
			BCMDNX_IF_ERR_EXIT(rv);
            /* Convert it to HW trap ID */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,(int)trap_code_upmep, &trap_code_converted));

            trap_config->flags &= ~BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
            trap_config->forwarding_header = 0;

            /* QAX cannot snoop and keep original outlif on system headers after recyclying.
               Trapping instead. */
            if (SOC_IS_QAX(unit) && trap_config->snoop_cmnd) {
                bcm_rx_snoop_config_t snoop_config;

                /* Get destination from snoop command configuration */
                rv = bcm_petra_rx_snoop_get(unit, trap_config->snoop_cmnd, &snoop_config);
                BCMDNX_IF_ERR_EXIT(rv);

                trap_config->dest_port = snoop_config.dest_port;
                trap_config->snoop_cmnd = 0;
                trap_config->flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER;
                trap_config->forwarding_header = _BCM_OAM_TRAP_FWD_OFFSET_INDEX;
            }

            rv = bcm_petra_rx_trap_set(unit, trap_code_converted, trap_config);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = OAM_ACCESS.trap_info.upmep_trap_ids.set(unit, upmep_trap_index, trap_code_upmep);
		BCMDNX_IF_ERR_EXIT(rv);

		/*creating mirror profile*/
        snoop_strength = 0;
		if (trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP || (SOC_IS_JERICHO(unit) && _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep))) {
			snoop_strength = soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3);
		}

        /* Assign a different fwd strength for passive/level error if user configured. */
        if (trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_PASSIVE) {
            forward_strength = (SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_passive)?(SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_passive):forward_strength;
        } else if (trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_LEVEL) {
            forward_strength = (SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_level)?(SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_level):forward_strength;
        }

        /* Allocate 5 mirror profiles total:
           1. OAMP trap
           2. CPU trap
           3. MD-Level trap
           4. Passive side trap
           5. Snoop*/
		_BCM_OAM_MIRROR_PROFILE_ALLOC(mirror_id, &trap_code_upmep, _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(trap_code_upmep));
		mirror_id_wb = (uint8)mirror_id;
		rv = OAM_ACCESS.trap_info.mirror_ids.set(unit, mirror_ind_uint32, mirror_id_wb);
		BCMDNX_IF_ERR_EXIT(rv);

		rv = OAM_ACCESS.cpu_trap_code_to_mirror_profile_map.set(unit, trap_code_upmep, mirror_id_wb);
		BCMDNX_IF_ERR_EXIT(rv);

        /* Advanced snooping in Arad/Arad+ */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0) &&
            trap_code_upmep == SOC_PPC_TRAP_CODE_OAM_CPU_SNOOP) {
            trap_code_upmep = SOC_PPC_TRAP_CODE_OAM_MIP_EGRESS_SNOOP_WITH_FTMH;
            forward_strength = 3; /*Use weaker trap strength to allow forwarding.*/
        }

		soc_sand_rv = soc_ppd_eg_mirror_recycle_command_trap_set(unit,
																 mirror_id,
																 trap_code_upmep,
																 snoop_strength,
                                                                 forward_strength);
		BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	}

	LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: traps were set: downmep trap=%d, upmep trap=%d, mirror profile=%d\n"), trap_id, trap_code_upmep, mirror_id));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* SDK-111413
 * Initialize the mapping of error trap type to trap id table
 * with INVALID values to avoid confusion between
 * initialized zero value and trap id that could be zero
 */

int _bcm_dpp_oam_error_trap_init(int unit)
{
    int rv;
    uint32 trap_type;
    BCMDNX_INIT_FUNC_DEFS;

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    for (trap_type=0; trap_type<SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.set(unit, trap_type, _BCM_DPP_OAM_ERROR_TRAP_ID_INVALID);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* OAMP error trap allocation and setting functions */
int _bcm_dpp_oam_error_trap_allocate(int unit, bcm_rx_trap_t trap_type, uint32 oamp_error_trap_id)
{
    int rv;
    uint32 ppd_trap_type;
	uint8 oam_is_init;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    _BCM_OAM_RX_TRAP_TYPE_TO_PPD_TRAP_TYPE(trap_type, ppd_trap_type);

    /* Mapping type to id */
	rv = OAM_ACCESS.error_type_to_trap_id_map.set(unit, ppd_trap_type, oamp_error_trap_id);
	BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_error_trap_destroy(int unit, uint32 oamp_error_trap_id)
{
    int rv;
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;
    SOC_TMC_DEST_INFO dest;
    int cpu_port;
    bcm_port_config_t port_config;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    /*Here should go over all the types mapped to this id and delete in soc*/
    for (trap_type=0; trap_type<SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if (trap_id == oamp_error_trap_id) {
            /* Reset destination */

            /*get CPU port*/
            rv = bcm_petra_port_config_get(unit, &port_config);
            BCMDNX_IF_ERR_EXIT(rv);
            BCM_PBMP_ITER(port_config.cpu, cpu_port) {
                break; /*get first CPU port*/
            }

            SOC_TMC_DEST_INFO_clear(&dest);
            dest.id = cpu_port;
            dest.type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;

            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, 0, dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_error_trap_set(int unit, uint32 oamp_error_trap_id, bcm_gport_t dest_port)
{
    int rv;
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;
    SOC_TMC_DEST_INFO dest;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);
    SOC_TMC_DEST_INFO_clear(&dest);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    rv = _bcm_dpp_gport_to_tm_dest_info(unit, dest_port, &dest);
    BCMDNX_IF_ERR_EXIT(rv);


    /*Here should go over all types mapped to this id and set destination in soc*/
    for (trap_type=0; trap_type<SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trap_id == oamp_error_trap_id) {
            /* Set destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_set(unit, trap_type, trap_id, dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}


int _bcm_dpp_oam_error_trap_get(int unit, uint32 oamp_error_trap_id, bcm_gport_t * dest_port)
{
    int rv;
	uint32 soc_sand_rv=0;
    uint32 trap_type;
    uint32 trap_id;
	uint8 oam_is_init;
    SOC_TMC_DEST_INFO dest;

    BCMDNX_INIT_FUNC_DEFS;

	_BCM_OAM_ENABLED_GET(oam_is_init);

	if (!oam_is_init) {
		BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
	}

	if (!SOC_IS_ARADPLUS(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: OAMP error traps are supported only in ARAD PLUS devices and above.\n")));
	}

    /*Here should go over all types mapped to this id and get destination from soc*/
    for (trap_type=0; trap_type<SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT; trap_type++) {
        rv = OAM_ACCESS.error_type_to_trap_id_map.get(unit, trap_type, &trap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trap_id == oamp_error_trap_id) {
            /* Get destination */
            soc_sand_rv = soc_ppd_oam_oamp_error_trap_id_and_destination_get(unit, trap_type, &trap_id, &dest);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    rv = _bcm_dpp_gport_from_tm_dest_info(unit, dest_port, &dest);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}
