/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_OAM_H_
#define _BCM_INT_DPP_OAM_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <bcm/sat.h>
#include <shared/hash_tbl.h>


#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>
#include <shared/swstate/sw_state_hash_tbl.h>   
/*
 * Global defines
 */

/**
 * Snoop commands used for advanced egress snooping.
 */
#define _BCM_OAM_MIP_SNOOP_CMND_DOWN 1
#define _BCM_OAM_MIP_SNOOP_CMND_UP 2

/*OAM statistics*/
#define ARAD_PP_CFM_ETHER_TYPE	(0x8902)

/* Max number of words in an event data (as read by the interrupt handler) */
#define _BCM_OAM_EVENT_DATA_WORDS_MAX 2

#define _BCM_OAM_ILLEGAL_DESTINATION 0xffff

/*_BCM_OAM_MAX_L3_INTF needs to match EEP of mep_db*/
#define _BCM_OAM_MAX_L3_INTF (SOC_IS_JERICHO(unit) ? ((1<<18) -1) : ((SOC_IS_ARAD_B0_AND_ABOVE(unit) ? ((1<<15) -1) : ((1<<17) -1))))

#define OAM_ACCESS        sw_state_access[unit].dpp.bcm.oam



#ifdef BCM_ARAD_SUPPORT
#define _BCM_OAM_ENABLED_GET(oam_is_init)                     \
  do {		                                                  \
    if (!SOC_DPP_PP_ENABLE(unit)) {    \
        oam_is_init = 0;                                      \
    } else {                                                  \
        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.get(unit, &oam_is_init);\
        BCMDNX_IF_ERR_EXIT(rv);                                  \
    }                                                         \
  } while (0)

#define _BCM_OAM_ENABLED_SET(oam_is_init)                     \
  do {		                                                  \
	rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.set(unit, oam_is_init);\
	BCMDNX_IF_ERR_EXIT(rv);                                      \
  } while (0)

#define _BCM_BFD_ENABLED_GET(is_bfd_init)                     \
  do {		                                                  \
    if (!SOC_DPP_PP_ENABLE(unit)) {    \
      is_bfd_init = 0;                                        \
    } else {                                                  \
        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.get(unit, &is_bfd_init);\
        BCMDNX_IF_ERR_EXIT(rv);                                  \
    }                                                         \
  } while (0)

#define _BCM_BFD_ENABLED_SET(is_bfd_init)                     \
  do {		                                                  \
	rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.set(unit, is_bfd_init);\
	BCMDNX_IF_ERR_EXIT(rv);                                      \
  } while (0)

#else
#define _BCM_OAM_ENABLED_GET(oam_is_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_BFD_ENABLED_GET(is_bfd_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_OAM_ENABLED_SET(oam_is_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));

#define _BCM_BFD_ENABLED_SET(is_bfd_init)                         \
  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Error: Unsupported device. Support only ARAD.\n")));
#endif /*BCM_ARAD_SUPPORT*/

#define _BCM_OAM_REMOTE_MEP_INDEX_BIT 25
#define _BCM_OAM_BFD_MEP_INDEX_BIT 26
#define _BCM_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT 27
#define _BCM_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT 28 /* Uses 3 bits*/
#define _BCM_OAM_SERVER_CLIENT_SIDE_BIT SOC_PPD_OAM_SERVER_CLIENT_SIDE_BIT
#define _BCM_BFD_UC_MEP_INDEX_BIT 23
#define _BCM_OAM_DOWN_MEP_LEVEL_0_NON_ACC_BIT 22
#define _BCM_OAM_BFD_SERVER_MEP_INDEX_BIT 21

/* Jericho only.  For now only one bit is used*/
#define _BCM_OAM_NUMBER_OF_BITS_USED_BY_OUTLIF_PCP               SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_PCP  
#define _BCM_OAM_OUTLIF_PROFILE_FOR_PCP_MAX_VALUE   (_BCM_OAM_NUMBER_OF_BITS_USED_BY_OUTLIF_PCP? ((1 << _BCM_OAM_NUMBER_OF_BITS_USED_BY_OUTLIF_PCP) -1) : 0)


#define _BCM_OAM_MEP_ID_MASK (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) -1)

/* unaccelerated mep index calculation */
#define _BCM_OAM_UNACCELERATED_MEP_INDEX_GET(lif, mdl, is_up)  \
	(lif + (mdl << _BCM_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT) + (is_up << _BCM_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT) + \
    ((!is_up && !mdl) << _BCM_OAM_DOWN_MEP_LEVEL_0_NON_ACC_BIT) )

/* unaccelerated mep index calculation */
#define _BCM_BFD_UNACCELERATED_MEP_INDEX_GET(lif)  \
	(lif + (1 << _BCM_OAM_BFD_MEP_INDEX_BIT))

/* client classifier of bfd server mep index calculation */
#define _BCM_BFD_SERVER_CLASSIFICATION_MEP_INDEX_GET(lif)  \
	(lif + (1 << _BCM_OAM_BFD_SERVER_MEP_INDEX_BIT))

#define _BCM_BFD_IS_SERVER_CLASSIFICATION_MEP(endpoint) \
        ((endpoint & (1 << _BCM_OAM_BFD_SERVER_MEP_INDEX_BIT)) >> _BCM_OAM_BFD_SERVER_MEP_INDEX_BIT)

#define _BCM_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_id_internal, endpoint)  \
	(endpoint = rmep_id_internal + (1 << _BCM_OAM_REMOTE_MEP_INDEX_BIT))

#define _BCM_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint)  \
	(rmep_id_internal = endpoint & 0xFFFFFF)

#define _BCM_OAM_MEP_INDEX_IS_REMOTE(endpoint)  \
	(((endpoint >> _BCM_OAM_REMOTE_MEP_INDEX_BIT) & 1) == 1)


#define _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(mep_id_internal, endpoint)  \
	(endpoint = mep_id_internal + (1 << _BCM_BFD_UC_MEP_INDEX_BIT))

#define _BCM_BFD_UC_MEP_INDEX_TO_INTERNAL(mep_id_internal, endpoint)  \
	(mep_id_internal = endpoint & ((1<<_BCM_BFD_UC_MEP_INDEX_BIT)-1) )

#define _BCM_BFD_MEP_INDEX_IS_UC(endpoint)  \
	(((endpoint >> _BCM_BFD_UC_MEP_INDEX_BIT) & 1) == 1)

#define _BCM_OAM_SET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_id, server_endpoint_id) \
	endpoint_id = (server_endpoint_id & _BCM_OAM_MEP_ID_MASK) + (1 << _BCM_OAM_SERVER_CLIENT_SIDE_BIT)

#define _BCM_OAM_GET_MEP_INDEX_FOR_SERVER_CLIENT_SIDE(endpoint_id)  (endpoint_id & _BCM_OAM_MEP_ID_MASK)

#define _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE  SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE

#define _BCM_OAM_TRAP_FWD_OFFSET_INDEX bcmRxTrapForwardingHeaderOamBfdPdu
#define _BCM_OAM_INVALID_LIF ((uint32)-1)
#define _BCM_OAM_INVALID_DISABLE_COUNTER_ID (0)

#define _BCM_OAM_NUM_OAMP_PORTS(unit) (SOC_IS_JERICHO(unit)?2:1)

#define _BCM_OAM_BFD_DEFAULT_SNP_STRENGTH_GET(_unit) soc_property_get((_unit), spn_DEFAULT_SNOOP_STRENGTH, 3)

#define _BCM_OAM_MA_NAME_WITHOUT_HEADERS 13
#define _BCM_OAM_MA_NAME_48 48

#define _BCM_OAM_RMEP_ID_MAX ((1<<13)-1) /* rmep id is 13 bits */
#define _BCM_OAM_MEP_PER_MA_MAX_NUM 256

#define _BCM_OAM_PASSIVE_PROFILE 3

/* Macros for custom_feature_oamp_flexible_da */
#define _BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit) ((SOC_IS_QAX(unit)) && (SOC_DPP_CONFIG(unit)->pp.oamp_flexible_da != 0)) 
#define _BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit) ((SOC_IS_JERICHO_AND_BELOW(unit))&& (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "dm_jumbo_tlv", 0)))
#define _BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_MIN (0)
#define _BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_MAX (2047) /* Max 2k endpoints allowed. */
#define _BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_CHECK(ep_id) (ep_id >= _BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_MIN && \
                                                                   ep_id <= _BCM_OAM_OAMP_FLEX_DA_FEATURE_VALID_EP_RANGE_MAX)

#define _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_NUM_ENTRIES_PER_EP 8 /* 8 OAMP gen mem entries per endpoint */

#define _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_DATA1_OFFSET 0 /* Offset 0 for DATA1 typically outlif. */
#define _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_DATA2_OFFSET 1 /* Offset 0 for DATA2 typically VSI. */
#define _BCM_OAM_CUST_FEAT_OAMP_FLEX_DA_MAC_OFFSET 2 /* Offset 2 for DA */

typedef enum _bcm_oam_ma_name_type_e{
    _BCM_OAM_MA_NAME_TYPE_ICC = 0,
    _BCM_OAM_MA_NAME_TYPE_SHORT,
    _BCM_OAM_MA_NAME_TYPE_STRING_11_BYTES,
    _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE,

    _BCM_OAM_MA_NAME_TYPE_NOF
} _bcm_oam_ma_name_type_t;


typedef struct _bcm_oam_ma_name_s {
	_bcm_oam_ma_name_type_t name_type;
	uint8 name[_BCM_OAM_MA_NAME_48];
    int index; /* oamp_pe_gen_mem pointer for flexible maid 48 byte */
} _bcm_oam_ma_name_t;


typedef struct _bcm_dpp_oam_bfd_sw_db_lock_s {
    sal_mutex_t lock;
    int is_init;
} _bcm_dpp_oam_bfd_sw_db_lock_t;

/*
 * Struct used to wrap callback for group together with its parameter.
 * Used in  bcm_petra_oam_group_traverse( )
*/
typedef struct bcm_oam_group_traverse_cb_w_param_s {
    bcm_oam_group_traverse_cb cb;
    void* user_data;
}bcm_oam_group_traverse_cb_w_param_t;

/*
 * Struct used to wrap callback for endpoint together with its parameter.
 * Used in  bcm_petra_oam_endpoint_traverse( )
*/
typedef struct bcm_oam_endpoint_traverse_cb_w_param_s {
    bcm_oam_endpoint_traverse_cb cb;
    bcm_oam_group_t group;
    void* user_data;
}bcm_oam_endpoint_traverse_cb_w_param_t;

/*
 * Struct used to wrap callback for endpoint together with its parameter.
 * Used in  bcm_petra_oam_endpoint_create( )
*/
typedef struct bcm_oam_endpoint_traverse_cb_profile_info {
    uint32 lif;
    uint32 first_endpoint_id; /* first endpoint id of given lif which we meet during traverse */
    uint8 is_ingress;
    uint8 is_alloced; /* profile was allocated during the traverse */
    uint8 is_last; /* profile was freed during the traverse */
    uint8 cb_rv; /* Function may fail inside the traverse API but not return an error. Handle errors through this field.*/
    uint8 is_passive_profile_allocated;
    SOC_PPC_OAM_LIF_PROFILE_DATA * profile_data;
}bcm_oam_endpoint_traverse_cb_profile_info_t;

/*struct should not exceed size 4 (in bytes).
  Used to store data on all events. from the FIFO.*/
typedef struct {
    uint16 mep_or_rmep_indx ; /* index in the (R)MEP DB (not the actual (R)MEP ID)*/
    uint8 is_oam;
    uint8 event_number ; /* number from the bcm_bfd/oam_event_type_t enum*/
    uint32 event_data[_BCM_OAM_EVENT_DATA_WORDS_MAX]; /* Extra data in the event */
} _bcm_oam_event_and_rmep_info;

/*
 * Endpoint linked list
 * {
 */
/*
 * Maximal number of endpoint list member entries to assign at init.
 * See bcm_dpp_endp_lst_init(), _bcm_dpp_oam_endpoint_list_member_add()
 */
#define MAX_NOF_ENDP_LST_MS     SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit)
/*
 * Maximal number of endpoint list entries to assign at init.
 * See bcm_dpp_endp_lst_init(), _bcm_dpp_oam_endpoint_list_init()
 */
#define MAX_NOF_ENDP_LSTS       SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit)
/*
 * Verify specific endpoint list index is marked as 'occupied'. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   'unit' is assumed to be defined in the caller's scope.
 *   '_rv' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(_endp_lst_index) \
  { \
    uint8 bit_val ; \
    int res ; \
    uint32 max_nof_endp_lsts ; \
    res = ENDP_LST_ACCESS.max_nof_endp_lsts.get(unit, &max_nof_endp_lsts) ; \
    BCMDNX_IF_ERR_EXIT(res) ; \
    if (_endp_lst_index >= max_nof_endp_lsts) \
    { \
      /* \
       * If endplst handle is out of range then quit with error. \
       */ \
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,(_BSL_BCM_MSG("Error: Endp list %d is out of range (max: %d)"),_endp_lst_index,max_nof_endp_lsts)); \
    } \
    else \
    { \
      res = ENDP_LST_ACCESS.occupied_endp_lsts.bit_get(unit, (int)_endp_lst_index, &bit_val) ; \
      BCMDNX_IF_ERR_EXIT(res) ; \
    } \
    if (bit_val == 0) \
    { \
      /* \
       * If endplst structure is not indicated as 'occupied' then quit \
       * with error. \
       */ \
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,(_BSL_BCM_MSG("Error: Endp list %d is not occupied"),_endp_lst_index)); \
    } \
  }
/*
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   '_rv' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) \
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    int res ; \
    res = BCM_E_UNIT ; \
    BCMDNX_IF_ERR_EXIT(res) ; \
  }
/*
 * Convert input endpoint list handle to index in 'occupied_endp_lsts' array.
 * Convert input index in 'occupied_endp_lsts' array to endpoint list handle.
 * Indices go from 0 -> (occupied_endp_lsts - 1)
 * Handles go from 1 -> occupied_endp_lsts
 */
#define BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(_endp_lst_index,_handle) (_endp_lst_index = _handle - 1)
#define BCM_DPP_ENDP_LST_CONVERT_ENDPLST_INDEX_TO_HANDLE(_handle,_endp_lst_index) (_handle = _endp_lst_index + 1)
/*
 * Verify specific endpoint list member index is marked as 'occupied'. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   'unit' is assumed to be defined in the caller's scope.
 *   '_rv' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(_endp_lst_m_index) \
  { \
    uint8 bit_val ; \
    int res ; \
    uint32 max_nof_endp_lst_ms ; \
    res = ENDP_LST_M_ACCESS.max_nof_endp_lst_ms.get(unit, &max_nof_endp_lst_ms) ; \
    BCMDNX_IF_ERR_EXIT(res) ; \
    if (_endp_lst_m_index >= max_nof_endp_lst_ms) \
    { \
      /* \
       * If ENDPLSTM handle is out of range then quit with error. \
       */ \
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,(_BSL_BCM_MSG("Error: Endp member %d is out of range (max: %d)"),_endp_lst_m_index,max_nof_endp_lst_ms)); \
    } \
    else \
    { \
      res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_get(unit, (int)_endp_lst_m_index, &bit_val) ; \
      BCMDNX_IF_ERR_EXIT(res) ; \
    } \
    if (bit_val == 0) \
    { \
      /* \
       * If ENDPLSTM structure is not indicated as 'occupied' then quit \
       * with error. \
       */ \
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,(_BSL_BCM_MSG("Error: Endp member %d is not occupied"),_endp_lst_m_index)); \
    } \
  }
/*
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 *
 * Notes:
 *   '_rv' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define BCM_DPP_ENDP_LST_M_VERIFY_UNIT_IS_LEGAL(unit) \
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    int res ; \
    res = BCM_E_UNIT ; \
    BCMDNX_IF_ERR_EXIT(res) ; \
  }
/*
 * Convert input endpoint list member handle to index in 'occupied_endp_lst_ms' array.
 * Convert input index in 'occupied_endp_lst_ms' array to endpoint list member handle.
 * Indices go from 0 -> (occupied_endp_lst_ms - 1)
 * Handles go from 1 -> occupied_endp_lst_ms
 */
#define BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(_endp_lst_m_index,_handle) (_endp_lst_m_index = _handle - 1)
#define BCM_DPP_ENDP_LST_M_CONVERT_ENDPLSTM_INDEX_TO_HANDLE(_handle,_endp_lst_m_index) (_handle = _endp_lst_m_index + 1)
/*
 * This is an identifier of an element of type endpoint_list_member_t
 *
 * Replace: endpoint_list_member_t (struct endpoint_list_member_s *)
 * because the new software state does not use pointers, only handles.
 * So now, ENDPOINT_LIST_MEMBER_PTR is just a handle to the 'endpoint list member'
 * structure (actually, index into 'endp_lst_ms_array' {of pointers})
 *
 * Note that the name is selected so as to minimize changes in current code.
 */
typedef uint32 ENDPOINT_LIST_MEMBER_PTR ;
/*
 * This is an identifier of an element of type endpoint_list_t
 *
 * Replace: endpoint_list_t *
 * because the new software state does not use pointers, only handles.
 * So now, ENDPOINT_LIST_PTR is just a handle to the 'endpoint list'
 * structure (actually, index into 'endp_lsts_array' {of pointers})
 *
 * Note that the name is selected so as to minimize changes in current code.
 */
typedef uint32 ENDPOINT_LIST_PTR ;
/*
 * Endpoint list structs
 */
typedef struct endpoint_list_member_s {
  uint32                   index;
  ENDPOINT_LIST_MEMBER_PTR next;
} endpoint_list_member_t ;

typedef struct endpoint_list_s {
  ENDPOINT_LIST_MEMBER_PTR first_member;
  uint32                   size;
} endpoint_list_t ;
/*
 * Control Structure for all created endpoint lists. Each endpoint list
 * is pointed by endp_lsts_array. See bcm_dpp_endp_lst_init()
 */
typedef struct bcm_dpp_sw_state_endp_lst_s
{ 
  uint32                                max_nof_endp_lsts ;
  uint32                                in_use ;
  PARSER_HINT_ARR_PTR endpoint_list_t   **endp_lsts_array ;
  PARSER_HINT_ARR     SHR_BITDCL        *occupied_endp_lsts ;
} bcm_dpp_sw_state_endp_lst_t ;
/*
 * Control Structure for all created endpoint list members. Each member
 * is pointed by endp_lst_ms_array. See bcm_dpp_endp_lst_init()
 */
typedef struct bcm_dpp_sw_state_endp_lst_m_s
{ 
  uint32                                    max_nof_endp_lst_ms ;
  uint32                                    in_use ;
  PARSER_HINT_ARR_PTR endpoint_list_member_t **endp_lst_ms_array ;
  PARSER_HINT_ARR     SHR_BITDCL             *occupied_endp_lst_ms ;
} bcm_dpp_sw_state_endp_lst_m_t ;
/*
 * All hash tables used by OAM
 */
typedef struct bcm_dpp_sw_state_oam_hashs_s
{ 
  /*
   * hash tables
   */
  sw_state_htb_hash_table_t _bcm_dpp_oam_ma_index_to_name_db ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_ma_to_mep_db ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_rmep_info_db ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_bfd_mep_info_db ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_bfd_mep_to_rmep_db ;
  /*
   * Used in Jericho only
   */
  sw_state_htb_hash_table_t _bcm_dpp_oam_mep_to_ais_id ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_mep_sat_gtf_info_db ;
  sw_state_htb_hash_table_t _bcm_dpp_oam_mep_sat_ctf_info_db ;
} bcm_dpp_sw_state_oam_hashs_t ;

typedef struct bcm_dpp_oam_info_s{
  uint32                      acc_ref_counter;
  uint8                       y1731_opcode_to_intenal_opcode_map[SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT];
  uint32                      error_type_to_trap_id_map[SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT];
  uint32                      oamp_rx_trap_code_ref_count[SOC_PPC_OAM_MEP_TYPE_COUNT][SOC_PPC_NOF_TRAP_CODES];
  SOC_PPC_OAM_INIT_TRAP_INFO  trap_info;
  uint8                       is_internally_allocated_mirror[DPP_MIRROR_ACTION_NDX_MAX]; /* by action_set */
  uint8                       cpu_trap_code_to_mirror_profile_map[SOC_PPC_NOF_TRAP_CODES];
  int                         prog_used[ARAD_PP_OAMP_PE_PROGS_NOF_PROGS]; /* prog ID in HW (program profile), -1 if not used */
  uint32                      mep_pe_profile[ARAD_PP_OAMP_PE_PROGS_NOF_PROGS];
  int                         instruction_func_is_valid[ARAD_PP_OAMP_PE_PROGS_NOF_PROGS];
  /*
   * Linked list heads
   */
  PARSER_HINT_PTR bcm_dpp_sw_state_endp_lst_t     *endp_lst ;
  /*
   * Linked list members
   */
  PARSER_HINT_PTR bcm_dpp_sw_state_endp_lst_m_t   *endp_lst_m ;
  /*
   * hash tables
   */
  bcm_dpp_sw_state_oam_hashs_t                    oam_hashs ;
  /* loopback object details*/
  int                         loopback_object_int_pri;
  uint8                       loopback_object_pkt_pri;
  uint8                       loopback_object_inner_pkt_pri;
  uint32                      no_of_slm_endpoints; /*Number of SLM enabled EPs - used on JR and below devices where SLM is global, not per EP*/
  uint32                      no_of_dlm_endpoints; /* Number of RFC-6374 DLM endpoints */
} bcm_dpp_oam_info_t;

typedef struct bcm_dpp_oam_crps_counters_s {
   int      crps_ingress_engine;
   int      crps_egress_engine;
   uint32   crps_ingress_counter[8];
   uint32   crps_egress_counter[8];
} bcm_dpp_oam_crps_counters_t;

uint32
  bcm_dpp_endp_lst_get_endp_lst_ptr_from_handle(
    int                      unit,
    ENDPOINT_LIST_PTR        endpoint_list,
    endpoint_list_t          **endp_lst_ptr_ptr
  ) ;
uint32
  bcm_dpp_endp_lst_get_endp_lst_m_ptr_from_handle(
    int                      unit,
    ENDPOINT_LIST_MEMBER_PTR endpoint_list_member,
    endpoint_list_member_t   **endp_lst_m_ptr_ptr
  ) ;
uint32
  bcm_dpp_endp_lst_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_endp_lsts
  ) ;
uint32
  bcm_dpp_endp_lst_m_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_endp_lst_ms
  ) ;
/*
 * }
 */

int _bcm_dpp_oam_bfd_mep_info_construct_bfd_mep_list_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);


int _bcm_dpp_oamp_interrupts_init(int unit, uint8 enable_interrupts);

int _bcm_dpp_oam_bfd_tx_gport_to_sys_port(int unit, bcm_gport_t gport, uint32 * sys_port);
int _bcm_dpp_oam_bfd_sys_port_to_tx_gport(int unit, bcm_gport_t * tx_gport, uint32 sys_port) ;

/*diagnostics functions*/
int _bcm_dpp_oam_bfd_diagnostics_endpoints(int unit);
int _bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(int unit, int endpoint_id);
int _bcm_dpp_oam_bfd_diagnostics_LM_counters(int unit, int endpoint_id);
int _bcm_dpp_oam_diag_print_hier_counters(int unit, SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY  *oem1_key);

int _bcm_dpp_oam_fifo_interrupt_handler(int unit, SOC_PPC_OAM_DMA_EVENT_TYPE event_type, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA * global_data);

int _bcm_petra_bfd_event_register(int unit, bcm_bfd_event_types_t event_types, bcm_bfd_event_cb cb, void *user_data);
int _bcm_petra_bfd_event_unregister( int unit, bcm_bfd_event_types_t event_types, bcm_bfd_event_cb cb);
int _bcm_jer_sat_event_register(int unit, bcm_sat_event_type_t event_type, bcm_sat_event_cb cb, void *user_data);
int _bcm_jer_sat_event_unregister(int unit, bcm_sat_event_type_t event_type, bcm_sat_event_cb cb);


int bcm_petra_oam_endpoint_action_set_internal(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t *action,
    int is_piggy_backed /* 0 - set to 0, 1 - set to 1, 2 - keep original */,
    int is_sat_loopback /* 0 - set to 0, 1 - set to 1, 2 - keep original */);

/*
 * Based on LIF and direction extract all endpoints of existing on different mdl
 * Update action set profile for all MEP's and MIP's.
 */
uint32 bcm_petra_oam_endpoint_on_lif_and_direction( int unit,
                                                SOC_PPC_LIF_ID lif,
                                                SOC_PPC_LIF_ID passive_side_lif,
                                                int is_upmep,
                                                uint8 is_passive,
                                                uint8 is_mip,
                                                bcm_oam_endpoint_traverse_cb cb,
                                                void *user_data);

#endif /* _BCM_INT_DPP_OAM_H_ */
