/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_local_lif.c
 * Purpose:     Local Lif allocation manager for SOC_SAND chips.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_lif.h>



STATIC uint32
_bcm_dpp_am_local_inlif_internal_alloc(int unit, uint8 tag, uint8 bank_id, int flags, int *local_lif);

STATIC uint32
_bcm_dpp_am_local_inlif_internal_dealloc(int unit, int flags, int local_lif);

STATIC uint32
_bcm_dpp_am_local_outlif_internal_dealloc(int unit, int local_lif);

STATIC uint32
_bcm_dpp_am_local_inlif_is_pack_empty(int unit,uint8 bank_id, uint8 flags, int elem, uint8 *is_pack_empty);

STATIC uint32
_bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_app_t app_type, uint32 *bitmap_phase_number);


STATIC uint32
_bcm_dpp_am_local_outlif_bank_alloc(int unit, int flags, bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info, uint8 tag, uint8 ext_type,int failover_id, int *bank_id , uint8 *bank_found);

STATIC uint32 
_bcm_dpp_am_local_outlif_internal_alloc(int unit, uint8 bank_id,  bcm_dpp_am_local_out_lif_info_t *lif_info,  int flags, int *local_lif);

STATIC uint32
_bcm_dpp_am_local_outlif_extension_lif_id_get(int unit, int base_lif,  int *ext_lif);

STATIC uint8 _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[SOC_PPC_EG_ENCAP_ACCESS_PHASE_COUNT] = {
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE      */        BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1,
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO*/              BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2,
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE*/            BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_3,
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR*/             BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_4,
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_FIVE*/             BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_5,
                    /*SOC_PPC_EG_ENCAP_ACCESS_PHASE_SIX*/              BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_6
                                                                        }; 

STATIC uint8 _bcm_dpp_am_local_outlif_bank_phase_to_access_phase(int unit, int bank_phase)
{
    switch (bank_phase)
    {
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC:
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_3:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_4:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_5:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_FIVE;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_6:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_SIX;
        default:
            return SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE;
    }
}


/*
 * LOCAL LIF DEFINES AND MACROS
 */
/* Validate Ingress LIF */
#define _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(init) \
    do {                                                                \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_IS_JERICHO(unit) || !SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for ingress local lif alloc\n" ), (unit))); \
        } \
        BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.initalized.get(unit, &init )); \
        if (init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize ingress local lif alloc\n" ), (unit))); \
        } \
    } while (0);

#define _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_ENTRIES_PER_BANK )
#define _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK  (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK / 3 )
#define _BCM_DPP_AM_INGRESS_LIF_NOF_LIFS_IN_BANK(is_wide) ( (is_wide) ? _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK : _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK)

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_ADJUSTMENT(object_off, is_wide) ( ( is_wide ) ?  object_off * 3 :  object_off * 2 ) 
#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(object_off, bank_id, is_wide) (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK * bank_id +  _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_ADJUSTMENT(object_off, is_wide)) 

#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) (lif_id /_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) (lif_id % _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, is_wide) ( ( is_wide ) ? ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK ) : ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK ) )

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(object_id, is_wide) ( ( is_wide ) ? (object_id % _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK ) : (object_id % _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK ) )
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(lif_id, is_wide) ( ( is_wide ) ? ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, is_wide) + (_BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) / 3) ) : ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, is_wide) + (_BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) / 2 ) ) ) 
#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_OBJECT_ID(object_off, bank_id, is_wide) ( ( is_wide ) ?  ( bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + object_off ) :  ( bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + object_off ) )

#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide,base_lif_id, ext_lif_id) ( (is_wide) ? ((base_lif_id < ext_lif_id ) ? ( 2 * base_lif_id) :  ( 2 * base_lif_id - 1)) : ( 2 * base_lif_id) )

#define _BCM_DPP_AM_INGRESS_LIF_CONFLICT_FOUND(phase_bmp)  \
        ( ( ( SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_tt_lookup_res_0)) && (SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_tt_lookup_res_1)) ) || \
          ( ( SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_vt_lookup_res_0)) && (SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_vt_lookup_res_1)) ) )

#define _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_TO_EXTENSION(app_type) ( (app_type == bcm_dpp_am_egress_encap_app_linker_layer ) ? TRUE : FALSE) 

#define _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY(pool_id, app_type) \
       ( ( pool_id == dpp_am_res_eg_out_rif ) || \
         ( pool_id == dpp_am_res_eg_out_ac)   || \
         ( app_type == bcm_dpp_am_egress_encap_app_out_ac )|| \
         ( pool_id ==  dpp_am_res_eg_data_trill_invalid_entry) )

#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(bank_phase, lif_phase) (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[lif_phase] == bank_phase)
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(bank_type, lif_type) (lif_type  == bank_type)
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(bank_extension, lif_extension) (lif_extension == bank_extension)
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXT_TYPE(bank_ext_type, lif_ext_type) (lif_ext_type == bank_ext_type)

#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, lif_phase, lif_type, lif_extension, lif_ext_type) \
                                                    ( (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(eedb_bank_info.phase,lif_phase ) ) && \
                                                      (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(eedb_bank_info.type,lif_type ) ) && \
                                                      (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(eedb_bank_info.is_extended,lif_extension ) ) && \
                                                      (!eedb_bank_info.is_extended || (eedb_bank_info.is_extended && (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXT_TYPE(eedb_bank_info.ext_type,lif_ext_type) ) ) ) )      

/*check that bank attributes are forced by user. if forced - check that bank attributes fits lif allocation. if not - return true*/
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, lif_phase, lif_type, lif_extension, lif_ext_type) \
           (  ( (eedb_bank_info.force_flags &  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE ) && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(eedb_bank_info.phase,lif_phase) )  ) || \
              ( (eedb_bank_info.force_flags &  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE )  && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(eedb_bank_info.type,lif_type) )  ) || \
              ( (eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXTENSION)  && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(eedb_bank_info.is_extended,lif_extension ) )  ) || \
              ( (eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXT_TYPE)  && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXT_TYPE(eedb_bank_info.ext_type,lif_ext_type ) )  ) )

/*
 *Convert from sw state bank id and bank offset to local outlif
 ------------------------------------------------------------------
 |                 |            |              |                  |
 |  bank_id[5:1]   |  rsv bits  |  bank_id[0]  |   offset         |
 |                 |            |              |                  |
 ------------------------------------------------------------------
 */
#define _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id, offset)      \
    (((bank_id >> 1) << (SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs) + 1)) +  ((bank_id & 1) <<  SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) + offset)

/* Convert from local outlif to sw state bank id */
#define _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id)          \
    (((lif_id >> (SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs) + 1)) << 1) \
        + ((lif_id >> (SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) & 1)))

/* Convert from local outlif to internal offset inside sw state bank */
#define _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id)         \
    (lif_id & ((1 << SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) - 1))

#define _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(unit, top_bank_id)     \
    (_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_BANKS(unit) * 2 + top_bank_id / 2)

#define _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(top_bank_id) \
    (_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit) / 2 * ( top_bank_id % 2 ))

#define _BCM_DPP_AM_LOCAL_IN_LIF_COUNTING_PROFILE_TO_TAG_ID(counting_profile) \
    ((counting_profile == BCM_DPP_AM_COUNTING_PROFILE_RESERVED || \
    counting_profile == BCM_DPP_AM_COUNTING_PROFILE_NONE) ? 0 : \
    ((counting_profile % _BCM_DPP_AM_LOCAL_IN_LIF_NOF_COUNTING_PROFILES) + 1))

#define _BCM_DPP_AM_LOCAL_OUT_LIF_COUNTING_PROFILE_TO_TAG_ID(unit,counting_profile) \
    ((counting_profile == BCM_DPP_AM_COUNTING_PROFILE_RESERVED || \
    counting_profile == BCM_DPP_AM_COUNTING_PROFILE_NONE) ? 0 : \
    ((counting_profile % (_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_COUNTING_PROFILES_GET(unit))) + 1))


#define ALLOC_MNGR_LOCAL_LIF_ACCESS                     sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif

/*    ****linked list implementation**** */

/*************
* FUNCTIONS *
 */
/*********************************************************************
* NAME:
*     failover_linked_list_memory_init
* TYPE:
*   PROC
* DATE:
*   NOV 12 2015
* FUNCTION:
*     Initialize all memory required for implementation of
*     failover_free_lif_linked_list.
* INPUT:
*   int unit -
*     Identifier of the device to access.
*   uint32 max_nof_lists -
*     Maximal number of linked lists allowed.
*   uint32 max_nof_list_members -
*     Maximal number of linked list members allowed on all linked
*     lists, together.
* REMARKS:
*     BCMDNX error code.
* RETURNS:
*     OK or ERROR indication.
 */
int
  failover_linked_list_memory_init(
    int                          unit,
    uint32                       max_nof_lists,
    uint32                       max_nof_list_members
  )
{
  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.alloc(unit)) ;
  /*
   * It is assumed here that 'alloc' will also load all array members by zero's!
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.alloc(unit, max_nof_lists)) ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_lists.set(unit, max_nof_lists)) ;
  /*
   * It is assumed here that 'alloc' will also load all array members by zero's!
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.alloc(unit, max_nof_list_members)) ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_list_members.set(unit, max_nof_list_members)) ;

exit:
   BCMDNX_FUNC_RETURN ;
}

static int failover_linked_list_member_add(int unit, int linked_list, int lif_id,  uint8 is_even)
{
  int
    first_member,
    lif_id_handle ;
  uint32
    size,
    max_nof_lif_linked_lists,
    max_nof_lif_linked_list_members ;

  BCMDNX_INIT_FUNC_DEFS ;
  /*
   * Make sure 'linked_list' and 'lif_id' are in range.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_lists.get(unit, &max_nof_lif_linked_lists)) ;
  if (linked_list >= max_nof_lif_linked_lists)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list (%d) is beyond range (max is %lu)!" ),
      __func__,linked_list,max_nof_lif_linked_lists)) ;
  }
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_list_members.get(unit, &max_nof_lif_linked_list_members)) ;
  if (lif_id >= max_nof_lif_linked_list_members)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list member (%d) is beyond range (max is %lu)!" ),
      __func__,lif_id,max_nof_lif_linked_list_members)) ;
  }
  /*
   * Verify that member corresponding to 'lif_id' is not already occupied.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, lif_id, &lif_id_handle)) ;
  if (lif_id_handle)
  {
    BCMDNX_ERR_EXIT_MSG(_SHR_E_FULL,(_BSL_BCM_MSG("%s(): This member (lif %d) is already in use!" ),__func__,lif_id)) ;
  }
  /*
   * We mark this member with id 'lif_id + 1'. This way, a value of '0' indicates 'empty entry'
   */
  lif_id_handle = lif_id + 1 ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, lif_id, lif_id_handle)) ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.is_even.set(unit, lif_id, is_even)) ;
  /*
   * Add this new entry (indexed 'lif_id') as first on link list indexed 'linked_list'
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.get(unit, linked_list, &first_member)) ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.set(unit, lif_id, first_member)) ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.set(unit, linked_list, lif_id_handle)) ;
  /*
   * Increment counter of number of elements on this linked list.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
  size += 1 ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;

exit:
  BCMDNX_FUNC_RETURN;
}


static int failover_linked_list_member_find(
  /*in*/
  int unit,
  int linked_list,
  uint8 is_with_id,
  uint8 is_even,
  uint8 delete_linked,
  /*out*/
  int *lif_id,
  uint8 *found)
{
  int
    first_member,
    lif_id_handle,
    local_lif_id,
    next_handle,
    next_index,
    previous_index ;
  uint8
    local_is_even ;
  uint32
    size,
    max_nof_lif_linked_lists,
    max_nof_lif_linked_list_members ;

  BCMDNX_INIT_FUNC_DEFS;
  BCMDNX_NULL_CHECK(found) ;
  BCMDNX_NULL_CHECK(lif_id) ;
  *found = FALSE ;
  next_index = 0;
  /*
   * Make sure 'linked_list' is in range.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_lists.get(unit, &max_nof_lif_linked_lists)) ;
  if (linked_list >= max_nof_lif_linked_lists)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list (%d) is beyond range (max is %lu)!" ),
      __func__,linked_list,max_nof_lif_linked_lists)) ;
  }
  if (is_with_id) 
  {
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_list_members.get(unit, &max_nof_lif_linked_list_members)) ;
    if (*lif_id >= max_nof_lif_linked_list_members)
    {
      BCMDNX_ERR_EXIT_MSG(
        _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list member (%d) is beyond range (max is %lu)!" ),
        __func__,*lif_id,max_nof_lif_linked_list_members)) ;
    }
  }
  /*
   * If this linked list is empty then quit.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.get(unit, linked_list, &first_member)) ;
  if (first_member == PTR_TO_INT(NULL))
  {
     BCMDNX_FUNC_RETURN ;
  }
  /*
   * There is at least one non-empty member of this linked list.
   */
  if (is_with_id) 
  {
    /*
     * Enter if, on input, 'lif_id' points to the requested member index.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, first_member - 1, &lif_id_handle)) ;
    local_lif_id = lif_id_handle - 1 ;
    if (local_lif_id == *lif_id)
    {
      /*
       * Match on the first member on this linked list.
       */
      *found = TRUE;
      if (delete_linked)
      {
        /*
         * If this member is to be deleted, disconnect it from the linked list.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, first_member - 1, &next_handle)) ;
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.set(unit, linked_list, next_handle)) ;
        /*
         * Set 'lif_id' to '0' to indicate this member is empty.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, first_member - 1, PTR_TO_INT(NULL))) ;
        /*
         * Decrease counter of number of members on this linked list.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
        size -= 1 ;
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;
      }
      BCMDNX_FUNC_RETURN;
    }
    /*
     * Reach here if first member does not match. Go search along the whole linked list.
     */
    /*
     * Get the handle of the second member.
     */
    previous_index = first_member - 1 ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, previous_index, &next_handle)) ;
    while (next_handle != PTR_TO_INT(NULL))
    {
      next_index = next_handle - 1 ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, next_index, &lif_id_handle)) ;
      local_lif_id = lif_id_handle - 1 ;
      if (local_lif_id == *lif_id)
      {
        *found = TRUE ;
        break ;
      }
      /*
       * No match. Go to next member if it is not NULL.
       */
      previous_index = next_index ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
    }
  }
  else
  {
    /*
     * Enter if, on input, 'lif_id' does not point to meaningful index.
     * The search is for the first match on 'is_even'.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.is_even.get(unit, first_member - 1, &local_is_even)) ;
    if (local_is_even == is_even)
    {
      /*
       * Match on first member!!!
       */
      *found = TRUE ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, first_member - 1, &local_lif_id)) ;
      local_lif_id -= 1 ;
      *lif_id = local_lif_id ;
      if (delete_linked)
      {
        /*
         * If this member is to be deleted, disconnect it from the linked list.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, first_member - 1, &next_handle)) ;
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.set(unit, linked_list, next_handle)) ;
        /*
         * Set 'lif_id' to '0' to indicate this member is empty.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, first_member - 1, PTR_TO_INT(NULL))) ;
        /*
         * Decrease counter of number of members on this linked list.
         */
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
        size -= 1 ;
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;
      }
      BCMDNX_FUNC_RETURN ;
    }
    /*
     * Reach here if first member does not match. Go search along the whole linked list.
     */
    /*
     * Get the handle of the second member.
     */
    previous_index = first_member - 1 ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, previous_index, &next_handle)) ;
    while (next_handle != PTR_TO_INT(NULL))
    {
      next_index = next_handle - 1 ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.is_even.get(unit, next_index, &local_is_even)) ;
      if (local_is_even == is_even)
      {
        *found = TRUE ;
        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, next_index, &local_lif_id)) ;
        local_lif_id -= 1 ;
        *lif_id = local_lif_id ;
        break ;
      }
      /*
       * No match. Go to next member if it is not NULL.
       */
      previous_index = next_index ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
    }
  }
  if (*found == TRUE)
  {
    if (delete_linked)
    {
      /*
       * If this member is to be deleted, disconnect it from the linked list.
       */
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.set(unit, previous_index, next_handle)) ;
      /*
       * Set 'lif_id' to '0' to indicate this member is empty.
       */
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, next_index, PTR_TO_INT(NULL))) ;
      /*
       * Decrease counter of number of members on this linked list.
       */
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
      size -= 1 ;
      BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;
   }
 }
exit:
  BCMDNX_FUNC_RETURN;
}


static int failover_linked_list_member_remove(
  /*in*/
  int unit,
  int linked_list,
  int lif_id,
  /*out*/
  uint8 *found)
{
  int
    first_member,
    first_index,
    lif_id_handle,
    local_lif_id,
    next_handle,
    next_index,
    previous_index ;
  uint32
    size,
    max_nof_lif_linked_lists,
    max_nof_lif_linked_list_members ;

  BCMDNX_INIT_FUNC_DEFS;
  BCMDNX_NULL_CHECK(found) ;
  *found = FALSE ;
  next_index = 0;
  /*
   * Make sure 'linked_list' is in range.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_lists.get(unit, &max_nof_lif_linked_lists)) ;
  if (linked_list >= max_nof_lif_linked_lists)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list (%d) is beyond range (max is %lu)!" ),
      __func__,linked_list,max_nof_lif_linked_lists)) ;
  }
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_list_members.get(unit, &max_nof_lif_linked_list_members)) ;
  if (lif_id >= max_nof_lif_linked_list_members)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list member (%d) is beyond range (max is %lu)!" ),
      __func__,lif_id,max_nof_lif_linked_list_members)) ;
  }
  /*
   * If this linked list is empty then quit.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.get(unit, linked_list, &first_member)) ;
  if (first_member == PTR_TO_INT(NULL))
  {
     BCMDNX_FUNC_RETURN ;
  }
  first_index = first_member - 1 ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, first_index, &lif_id_handle)) ;
  local_lif_id = lif_id_handle - 1 ;
  if (local_lif_id == lif_id)
  {
    /*
     * Match on the first member on this linked list.
     */
    *found = TRUE;
    /*
     * This member is to be deleted: Disconnect it from the linked list.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, first_index, &next_handle)) ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.set(unit, linked_list, next_handle)) ;
    /*
     * Set 'lif_id' to '0' to indicate this member is empty.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, first_index, PTR_TO_INT(NULL))) ;
    /*
     * Decrease counter of number of members on this linked list.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
    size -= 1 ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;
    BCMDNX_FUNC_RETURN;
  }

  /*
   * Reach here if first member does not match. Go search along the whole linked list.
   */
  /*
   * Get the handle of the second member.
   */
  previous_index = first_index ;
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, first_index, &next_handle)) ;
  while (next_handle != PTR_TO_INT(NULL))
  {
    next_index = next_handle - 1 ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, next_index, &lif_id_handle)) ;
    local_lif_id = lif_id_handle - 1 ;
    if (local_lif_id == lif_id)
    {
      *found = TRUE ;
      break ;
    }
    /*
     * No match. Go to next member if it is not NULL.
     */
    previous_index = next_index ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
  }
  if (*found == TRUE)
  {
    /*
     * This member is to be deleted: Disconnect it from the linked list.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.set(unit, previous_index, next_handle)) ;
    /*
     * Set 'lif_id' to '0' to indicate this member is empty.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.set(unit, next_index, PTR_TO_INT(NULL))) ;
    /*
     * Decrease counter of number of members on this linked list.
     */
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
    size -= 1 ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.set(unit, linked_list, size)) ;
  }
exit:
  BCMDNX_FUNC_RETURN;
}


int failover_linked_list_print(
  /*in*/
  int unit,
  int linked_list)
{
  int ii = 0 ;
  int
    first_member,
    lif_id_handle,
    local_lif_id,
    next_handle,
    next_index ;
  uint32
    size,
    max_nof_lif_linked_lists ;

  BCMDNX_INIT_FUNC_DEFS ;
  /*
   * Make sure 'linked_list' is in range.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_ACCESS.max_nof_lif_linked_lists.get(unit, &max_nof_lif_linked_lists)) ;
  if (linked_list >= max_nof_lif_linked_lists)
  {
    BCMDNX_ERR_EXIT_MSG(
      _SHR_E_FULL,(_BSL_BCM_MSG("%s(): This linked list (%d) is beyond range (max is %lu)!" ),
      __func__,linked_list,max_nof_lif_linked_lists)) ;
  }
  /*
   * If this linked list is empty then quit.
   */
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.first_member.get(unit, linked_list, &first_member)) ;
  if (first_member == PTR_TO_INT(NULL))
  {
    cli_out("Linked list %d is empty:\r\n", linked_list ) ;
    BCMDNX_FUNC_RETURN ;
  }
  BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, linked_list, &size)) ;
  cli_out("linked_list %d: size= %d:\r\n", linked_list, (int)size ) ;
  next_index = first_member - 1 ;
  do
  {
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.lif_id.get(unit, next_index, &lif_id_handle)) ;
    local_lif_id = lif_id_handle - 1 ;
    cli_out("element %d: lif-id= %d:  ", ii, local_lif_id ) ;
    ii++ ;
    BCMDNX_IF_ERR_EXIT(LIF_LINKED_LIST_MEMBERS_ARRAY_ACCESS.next.get(unit, next_index, &next_handle)) ;
    next_index = next_handle - 1 ;
  } while (next_handle != PTR_TO_INT(NULL)) ; 
  cli_out("\r\n" ) ;
exit:
  BCMDNX_FUNC_RETURN;
}


#if 0
static int failover_linked_list_destroy(int unit, failover_free_lif_linked_list_t *linked_list) {
   failover_free_lif_linked_list_member_t *linked_p;
   failover_free_lif_linked_list_member_t *linked_temp_p;

   BCMDNX_INIT_FUNC_DEFS;
   linked_p = linked_list->first_member;
   while (linked_p != NULL) {
      linked_temp_p = linked_p;
      linked_p = linked_p->next;
      BCM_FREE(linked_temp_p);
   }
   linked_list->first_member = 0;
   linked_list->size = 0;

   BCMDNX_FUNC_RETURN;
}

#endif


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_init
 * Purpose:
 *      Initalize ingress lif allocation 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_init(int unit)
{
    bcm_error_t rv;
    /* Add global flag of initalization*/
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.alloc(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        /*set the local inlif intialization flag to TRUE*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.initalized.set(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.lif_allocated.set(unit, FALSE ));
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *     _bcm_dpp_am_local_inlif_alloc
 * Purpose:
 *      Allocate new ingress local LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) BCM_DPP_AM_IN_LIF_FLAG_* to indicate the lif type (common/wide).
 *     lif_info         - (IN) Lif info struct
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_alloc(int unit, int flags, bcm_dpp_am_local_inlif_info_t *lif_info)
{
    int rv , bank_id_iter;
    uint8 is_init, lif_allocated;
    uint8 is_wide_lif;
    int local_inlif_id;
    uint32 bitmap_access_phase , tmp_phase_bmp;
    uint8 bank_found = FALSE;
    uint8 bank_id = 0;
    uint8 checked_bank_bmp = 0;
    uint8 inlif_tag;
    bcm_dpp_am_local_inlif_application_type_info_t app_type_info;
    bcm_dpp_am_local_inlif_table_bank_info_t       *bank_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    is_wide_lif = (lif_info->local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE;

    rv = _bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(unit, lif_info->app_type, &bitmap_access_phase);
    BCMDNX_IF_ERR_EXIT(rv);

    inlif_tag = _BCM_DPP_AM_LOCAL_IN_LIF_COUNTING_PROFILE_TO_TAG_ID(lif_info->counting_profile_id);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.application_type_info.get(unit, lif_info->app_type, &app_type_info);
    BCMDNX_IF_ERR_EXIT(rv);

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS ; bank_id_iter++)
    {
        /* This application is already allocated inLif in this bank*/
        if ( app_type_info.banks_bmp & ( 1 << bank_id_iter) )
        {
            rv = _bcm_dpp_am_local_inlif_internal_alloc(unit, inlif_tag, bank_id_iter,
                            (lif_info->local_lif_flags) | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_CHECK_ONLY,
                            &local_inlif_id);
            if (rv == BCM_E_RESOURCE) {
                checked_bank_bmp |= ( 1 << bank_id_iter);
                continue;
            } else if (rv == BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(
                    _bcm_dpp_am_local_inlif_internal_alloc(unit, inlif_tag, bank_id_iter,
                                        (lif_info->local_lif_flags), &local_inlif_id));
                bank_found = TRUE;
                bank_id = bank_id_iter;
                break;
            } else {
                BCM_EXIT;
            }
        }
    }

    BCMDNX_ALLOC(bank_info, sizeof(bcm_dpp_am_local_inlif_table_bank_info_t),
                 "_bcm_dpp_am_local_inlif_alloc:bank_info");
    if (bank_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                            (_BSL_BCM_MSG("failed to allocate memory")));
    }

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS && !bank_found; bank_id_iter++)
    {
        if (checked_bank_bmp & ( 1 << bank_id_iter)) {
            /*no need to check this bank - it is full */
            continue;
        }

        /* For jericho, phase_bmp of the bank should be checked to avoid conflicts.
         * For QAX, QUX and some other devices, no conflicts will happen.
         */
        if (!SOC_IS_QAX(unit)) {
            /*get the bank info*/
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.get(unit, bank_id_iter, bank_info);
            BCMDNX_IF_ERR_EXIT(rv);

            tmp_phase_bmp = bank_info->phase_bmp | bitmap_access_phase;
            /*check for application conflicts according to bank phase */
            if (_BCM_DPP_AM_INGRESS_LIF_CONFLICT_FOUND(tmp_phase_bmp)) {
                continue;
            }
        }
         /* If no conflict found - this bank can be used*/
        rv = _bcm_dpp_am_local_inlif_internal_alloc(unit,inlif_tag, bank_id_iter,
                                            lif_info->local_lif_flags, &local_inlif_id);
        if (rv == BCM_E_RESOURCE) {
            continue;
        } else if (rv == BCM_E_NONE) {
            bank_found = TRUE;
            bank_id = bank_id_iter;
            break;
        } else {
            BCM_EXIT;
        }
    }

    if (!bank_found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("No more free inlifs." )));
    }
    /* else: Allocation succeeded */
    lif_info->base_lif_id = BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(local_inlif_id);
    lif_info->ext_lif_id  = ( is_wide_lif ) ? BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_LIF_ID(local_inlif_id) : _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED;
    lif_info->valid = TRUE;

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.get(unit, bank_id, bank_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set application in bank applications bitmap*/
    bank_info->phase_bmp |= bitmap_access_phase;

    /*set the inlif bank info into SW database*/
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.set(unit, bank_id, bank_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set bank in bank applications bitmap*/
    app_type_info.banks_bmp |= ( 1 << bank_id );

    /*set the app-type  info into SW database*/
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.application_type_info.set(unit, lif_info->app_type, &app_type_info);
    BCMDNX_IF_ERR_EXIT(rv);


    /*set the local inlif info into SW database*/
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.data_base.set(unit, lif_info->base_lif_id, lif_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /*set the local inlif intialization flag to TRUE*/
    BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.lif_allocated.get(unit, &lif_allocated ));
    if (!lif_allocated && (lif_info->counting_profile_id != BCM_DPP_AM_COUNTING_PROFILE_RESERVED))
    {
        BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.lif_allocated.set(unit, TRUE ));
    }

    /*map local inlif to global inlif*/
    rv= _bcm_dpp_global_lif_mapping_create(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_info->glif, lif_info->base_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;

exit:
    BCM_FREE(bank_info);
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_dealloc
 * Purpose:
 *      De-Allocate new ingress local LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     lif_index        - (IN) local inlif handle id
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_dealloc(int unit, int lif_index)
{
    int rv; 
    uint8 flags ; 
    uint8 is_init , is_wide_lif;
    bcm_dpp_am_local_inlif_info_t inlif_info;
    int internal_inlif_id;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.data_base.get(unit, lif_index, &inlif_info);
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  inlif_info.local_lif_flags ; 

    is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE;

    /* covert base lif to handle id*/
    internal_inlif_id =  _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide_lif,inlif_info.base_lif_id,inlif_info.ext_lif_id);

    rv = _bcm_dpp_am_local_inlif_internal_dealloc(unit,flags,internal_inlif_id); 
    BCMDNX_IF_ERR_EXIT(rv);

    inlif_info.valid = FALSE;

    /*set the local inlif info into SW database*/
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.data_base.set(unit, lif_index, &inlif_info);
    BCMDNX_IF_ERR_EXIT(rv);     

    /*destroy mapping of local inlif to global inlif*/
    rv= _bcm_dpp_global_lif_mapping_remove(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL | _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, 0,  lif_index);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_internal_alloc
 * Purpose:
 *     Allocate either a wide/ common lif according to lif type. calculate the LIF id, and update the corresponding pool id.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     bank_id      - (IN) Ingress LIF bank id (0-3).
 *     flags        - (IN) BCM_DPP_AM_IN_LIF_FLAG_WIDE, BCM_DPP_AM_IN_LIF_FLAG_COMMON or BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID.
 *     local_lif   - (INOUT) Will be filled with the local lif's id. If WITH_ID alloc flag is provided, Id must be provided.
 * Returns: 
 *     BCM_E_NONE       - If allocation is successful. 
 *     BCM_E_PARAM      - If WITH_ID flag is set and given global lif is illegal.
 *     BCM_E_RESOURCE   - If lif banks are full.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_local_inlif_internal_alloc(int unit, uint8 tag, uint8 bank_id, int flags, int *local_lif){
    uint32 rv;

    int elem, alloc_elem_id ,elem_in_bank, elem_to_alloc , nof_elem_to_alloc;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    uint8 tag_id;
    bcm_dpp_am_local_inlif_table_bank_info_t       *bank_info = NULL;
    uint32 alloc_flags;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(local_lif);

    rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,flags, &tag, &elem);
    if (flags & BCM_DPP_AM_IN_LIF_FLAG_ALLOC_CHECK_ONLY)  
    { 
        BCM_RETURN_VAL_EXIT(rv); 
    } 
    else 
    { 
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* sucees in allocation*/

    /*compute new LIF id*/
    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);
    *local_lif = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(elem_in_bank, bank_id, is_wide_lif);

    BCMDNX_ALLOC(bank_info, sizeof(bcm_dpp_am_local_inlif_table_bank_info_t), "_bcm_dpp_am_local_inlif_internal_alloc:bank_info");
    if (bank_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /*update corresponding LIF pool*/
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.get(unit, bank_id, bank_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_wide_lif)
    {
        /*check if this is first entry allocated in 6-pack*/
        if (!bank_info->packs_info[elem_in_bank/2].is_used) /* wide lif : 2 wide lifs per 6-pack */ 
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + (elem_in_bank /2  * 3);
            /* usually alloc 3 entries in common pull, except last six-pack.*/
            nof_elem_to_alloc = (elem_in_bank/2 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 2 : 3;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id < elem_to_alloc + nof_elem_to_alloc ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_tag_get(unit,bank_id,alloc_flags,alloc_elem_id,&tag_id);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,alloc_flags, &tag_id, &alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info->packs_info[elem_in_bank/2].is_used  = TRUE;  
            bank_info->packs_info[elem_in_bank/2].lif_type =  BCM_DPP_AM_IN_LIF_FLAG_WIDE;
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.set(unit, bank_id, bank_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        /*check if this is first entry allocated in 6-pack*/
        if (!bank_info->packs_info[elem_in_bank/3].is_used) /* common lif : 3 common lifs per 6-pack */ 
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + (elem_in_bank /3  * 2 );
            /* usually alloc 2 entries in wide pull, except last six-pack.*/
            nof_elem_to_alloc = (elem_in_bank/3 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 1 : 2;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id < elem_to_alloc + nof_elem_to_alloc ; alloc_elem_id ++  ) 
            {

                rv = bcm_dpp_am_local_inlif_tag_get(unit,bank_id,alloc_flags,alloc_elem_id,&tag_id);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,alloc_flags, &tag_id, &alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info->packs_info[elem_in_bank/3].is_used  = TRUE;   
            bank_info->packs_info[elem_in_bank/3].lif_type =  BCM_DPP_AM_IN_LIF_FLAG_COMMON;
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.set(unit, bank_id, bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    }

exit:
    BCM_FREE(bank_info);
    BCMDNX_FUNC_RETURN;

}

/*checks weather a six-pack is empty or not*/
STATIC uint32
_bcm_dpp_am_local_inlif_is_pack_empty(int unit,uint8 bank_id, uint8 flags, int elem, uint8 *is_pack_empty)
{

    uint32 rv1, rv2;
    int elem_to_check, elem_in_bank;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    BCMDNX_INIT_FUNC_DEFS;

    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);

    if ( is_wide_lif )
    {
        /*JR and JR +" special 6-pack: the last one includes only 1-Wide LIF and 2-common LIFs
         *QAX: 2-Wide LIF and 2-common LIFs
         *QUX: 0-Wide LIF and 0-common LIFs
         */
        
        if ((elem_in_bank == _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK - 1) && !SOC_IS_QAX(unit)) 
        {
            *is_pack_empty = TRUE;
        }
        else
        {
            elem_to_check = (elem_in_bank % 2 == 0) ? (elem + 1) : (elem -1) ;
            rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem_to_check);
            if (rv1 != BCM_E_EXISTS) {
                if (rv1 == BCM_E_NOT_FOUND  )
                {
                    *is_pack_empty = TRUE;
                }
                else
                {
                    BCMDNX_IF_ERR_EXIT(rv1);
                }
            }
        }
    }
    else 
    {
        if ( elem_in_bank == _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK - 2 ) 
        {
            rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+1);
            if ( rv1 != BCM_E_EXISTS  )
            {
                *is_pack_empty = TRUE;
            }
        }
        else if ( elem_in_bank == _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK - 1 )
        {
            rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-1);
            if ( rv1 != BCM_E_EXISTS  )
            {
                *is_pack_empty = TRUE;
            }
        }
        else 
        {
            switch (elem_in_bank % 3) 
            {
            case 0 : 
               rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+1);
               rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+2);

               if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
               {
                    *is_pack_empty = TRUE;
               }

               break;
            case 1:
               rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-1);
               rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+1);

               if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
               {
                    *is_pack_empty = TRUE;
               }

               break;
             case 2:
               rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-1);
               rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-2);

               if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
               {
                    *is_pack_empty = TRUE;
               }

               break;
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_internal_dealloc
 * Purpose:
 *     Allocate either a wide/ common lif according to lif type. calculate the LIF id, and update the corresponding pool id.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     bank_id      - (IN) Ingress LIF bank id (0-3).
 *     flags        - (IN) BCM_DPP_AM_IN_LIF_FLAG_WIDE, BCM_DPP_AM_IN_LIF_FLAG_COMMON or BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID.
 *     local_lif   - (INOUT) Will be filled with the local lif's id. If WITH_ID alloc flag is provided, Id must be provided.
 * Returns: 
 *     BCM_E_NONE       - If allocation is successful. 
 *     BCM_E_PARAM      - If WITH_ID flag is set and given global lif is illegal.
 *     BCM_E_RESOURCE   - If lif banks are full.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_local_inlif_internal_dealloc(int unit, int flags, int local_lif){

    uint32 rv;
    uint8 bank_id;
    int elem, alloc_elem_id, elem_in_bank, elem_to_alloc , nof_elem_to_alloc;

    bcm_dpp_am_local_inlif_table_bank_info_t       *bank_info = NULL;
    uint32 alloc_flags;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    uint8 is_six_pack_empty = FALSE ;

    BCMDNX_INIT_FUNC_DEFS;


    elem = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(local_lif,is_wide_lif);
    bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_lif);

    rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,flags,elem);
    BCMDNX_IF_ERR_EXIT(rv);

    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);

    /*check whether there are another entries in that 6-pack: if not update 6-pack as empty*/
    rv = _bcm_dpp_am_local_inlif_is_pack_empty(unit,bank_id,flags,elem,&is_six_pack_empty);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_ALLOC(bank_info, sizeof(bcm_dpp_am_local_inlif_table_bank_info_t), "_bcm_dpp_am_local_inlif_internal_dealloc:bank_info");
    if (bank_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /*update corresponding LIF pool*/
    if (is_six_pack_empty) 
    {
        /*update corresponding LIF pool*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.get(unit, bank_id, bank_info);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_wide_lif)
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + (elem_in_bank /2  * 3)  ;
            /* usually dealloc 3 entries in common pull, except last six-pack.*/
            if (SOC_IS_QUX(unit)) {
                nof_elem_to_alloc = (elem_in_bank/2 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 0 : 3;
            } else if (SOC_IS_QAX(unit)) {
                nof_elem_to_alloc = (elem_in_bank/2 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 1 : 3;
            } else {
                nof_elem_to_alloc = (elem_in_bank/2 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 2 : 3;
            }

            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id < elem_to_alloc + nof_elem_to_alloc ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,alloc_flags,alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info->packs_info[elem_in_bank/2].is_used  = FALSE;  
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.set(unit, bank_id, bank_info);
            BCMDNX_IF_ERR_EXIT(rv);

        } else {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + (elem_in_bank /3  * 2 )  ;
            /* usually dealloc 2 entries in wide pull, except last six-pack.*/
            nof_elem_to_alloc = (elem_in_bank/3 == _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK/6 ) ? 1 : 2;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id < elem_to_alloc + nof_elem_to_alloc ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,alloc_flags,alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info->packs_info[elem_in_bank/3].is_used  = FALSE;  
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.bank_info.set(unit, bank_id, bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        }
    }

exit:
    BCM_FREE(bank_info);
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *     _bcm_dpp_am_local_inlif_is_alloc
 * Purpose:
 *      Checks whether ingress local LIF is allocated 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     lif_index        - (IN) local inlif handle id
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_is_alloc(int unit, int lif_index)
{
    int rv; 
    uint8 bank_id;
    uint8 is_init, is_wide_lif;
    int elem;
    uint32 flags ; 
    int internal_inlif_id;
    bcm_dpp_am_local_inlif_info_t inlif_info;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.data_base.get(unit, lif_index, &inlif_info);
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  inlif_info.local_lif_flags ; 

    is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE;

    /* covert base lif to handle id*/
    internal_inlif_id =  _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide_lif,inlif_info.base_lif_id,inlif_info.ext_lif_id);
    
    elem = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(internal_inlif_id,is_wide_lif);
    bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(internal_inlif_id);

    rv = bcm_dpp_am_local_inlif_is_alloc(unit,bank_id,flags,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}

/* Convert application type to access phase number */
STATIC uint32
_bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_app_t app_type, uint32 *bitmap_phase_number)
{    
    BCMDNX_INIT_FUNC_DEFS;

    *bitmap_phase_number = 0;

     switch (app_type) {
        case bcm_dpp_am_ingress_lif_app_mpls_term:
        /* In case of Unindexed supported up to two terminations */
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
      /* MPLS Indexed Support */
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:
            if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);  /* TT only lookups, tt lookup res 0 is MPLS_1 */
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            } else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
        	} else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
           	} else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	}
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
        case bcm_dpp_am_ingress_lif_app_ingress_ac:
        case bcm_dpp_am_ingress_lif_app_vpn_ac:
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21 || SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
            }
		    else {
		        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_0);
		    }
            /* In case trill is enabled, make sure the ingress nick lookup does conflict with VD-vlan-vlan lookup */
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_ingress_isid:
        case bcm_dpp_am_ingress_lif_app_ip_term:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        break;
        /* FRR */
        case bcm_dpp_am_ingress_lif_app_mpls_frr_term:          
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
        		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
        	} else {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
        	}
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        break;
        case bcm_dpp_am_ingress_lif_app_trill_nick:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);
          break;
		case bcm_dpp_am_ingress_lif_app_l2tp:
        case bcm_dpp_am_ingress_lif_app_mpls_term_mldp:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
        default:
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported app type %d"), app_type));
        }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_counting_profile_set
 * Purpose:
 *      Set a counting profile for ingress LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     counting_profile_id        - (IN) counter profile id
 *     base                       - (IN) base LIF ID for the counting profile
 *     size                       - (IN) number of LIFs in the couner profile
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_counting_profile_set(int unit, int counting_profile_id, int base, int size)
{
    int rv; 
    bcm_dpp_am_local_lif_counter_profile_info_t counter_profile_info;
    uint8 profile_tag, start_bank_id, end_bank_id, bank_id_iter, is_wide_lif, lif_allocated;
    int start_elem, remaining_size, start_elem_iter,nof_elements_to_tag_in_bank ;
    uint32 flags=0; 
    int internal_base_id;
    int nof_base_id;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify that no other inLIFs were allocated so far*/
    BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.lif_allocated.get(unit, &lif_allocated ));
    if (lif_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("No Ingress LIFs should be allocated prior to counting profile set." )));
    }

    /*verify base is not in the reserved range*/
    if (base < BCM_DPP_AM_LOCAL_IN_LIF_NOF_RESERVED_LIFS) 
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("base is in the local lif reserved range." )));
    }

    /*base in the base local lif -> should be converted to base internal lif id  */
    internal_base_id =  2*base;

    /*base lif id is in half bank*/
    nof_base_id = size * 2;

    for (is_wide_lif = FALSE ; is_wide_lif <= TRUE ; is_wide_lif ++)
    {
        flags |= ( is_wide_lif ) ? BCM_DPP_AM_IN_LIF_FLAG_WIDE :  BCM_DPP_AM_IN_LIF_FLAG_COMMON;

        /*set tags for common/wide pools*/
        start_elem = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(internal_base_id, is_wide_lif);
        start_bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(internal_base_id);

        if (is_wide_lif)
        {
            int base_lif_id;
            int local_lif;
            int elem_in_bank;

            /* Check the first allocated entry in 6-pack according to start_elem */
            elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(start_elem, is_wide_lif);
            local_lif = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(elem_in_bank, start_bank_id, is_wide_lif);
            base_lif_id = BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(local_lif);
            if (base_lif_id < base)
            {
               /*
                * The first allocated entry is out of this configured range
                * We should use the next entry in 6-pack to make sure that local lif id is in the range
                */
                start_elem += 1;
                internal_base_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(start_elem, start_bank_id, is_wide_lif);
                start_bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(internal_base_id);

                /* Re-calculate the number of base lif IDs due to the updates to start_elem*/
                nof_base_id = nof_base_id - (internal_base_id - 2 * base);
            }
        }

        end_bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID((internal_base_id + nof_base_id - 1));
        profile_tag = _BCM_DPP_AM_LOCAL_IN_LIF_COUNTING_PROFILE_TO_TAG_ID(counting_profile_id);

        remaining_size = ( is_wide_lif ) ? (nof_base_id / 3) : (nof_base_id / 2);

        /*check same bank - If same bank just tag all LIFs in this range in the bank*/
        if (start_bank_id == end_bank_id) 
        {
            rv = bcm_dpp_am_local_inlif_range_set(unit,start_bank_id, flags, profile_tag, start_elem, remaining_size );
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            for (bank_id_iter=start_bank_id; bank_id_iter <= end_bank_id; bank_id_iter++) 
            {
                start_elem_iter = (bank_id_iter == start_bank_id) ? start_elem : _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_OBJECT_ID(0, bank_id_iter, is_wide_lif);
                nof_elements_to_tag_in_bank = SOC_SAND_MIN((_BCM_DPP_AM_INGRESS_LIF_NOF_LIFS_IN_BANK(is_wide_lif)-_BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(start_elem_iter, is_wide_lif)),remaining_size);
                remaining_size -= nof_elements_to_tag_in_bank;

                rv = bcm_dpp_am_local_inlif_range_set(unit,bank_id_iter, flags, profile_tag, start_elem_iter, nof_elements_to_tag_in_bank);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /*set the counter profile in sw-state*/
    counter_profile_info.base_offset = base;
    counter_profile_info.size = size ;
    /*Currently not relevant for InLif*/
    counter_profile_info.double_entry = FALSE;    
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.counter_profile_info.set(unit, counting_profile_id, &counter_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



/* Egress - Local Outlif allocation*/

/* Initalize egress lif allocation */
uint32
_bcm_dpp_am_local_outlif_init(int unit)
{
    int rv , bank_id_iter, rif_max_id, rif_id_iter ;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;
    bcm_dpp_am_local_out_lif_info_t rif_info;
    int elem, bank_id;
    uint8 allocate_rif_bank, outlif_tag;
    uint8 is_allocated;

    /* Add global flag of initalization*/
    BCMDNX_INIT_FUNC_DEFS;

    /* If warmboot mode, all this data will be restored */
    if (!SOC_WARM_BOOT(unit)) {

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.alloc(unit, _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.alloc(unit, _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        rif_max_id = SOC_DPP_CONFIG(unit)->l3.nof_rifs;

        for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit) ; bank_id_iter++) 
        {
            sal_memset(&eedb_bank_info,0,sizeof(arad_pp_eg_encap_eedb_bank_info_t));

            eedb_bank_info.valid = FALSE ; 
            eedb_bank_info.is_extended = FALSE;
            eedb_bank_info.ext_type = BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION;
            eedb_bank_info.force_flags = 0;

            /* In QAX, rifs are not in the EEDB. Don't allocate banks for them. */
            allocate_rif_bank = SOC_IS_JERICHO_AND_BELOW(unit) && bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_PHASE(rif_max_id);

            eedb_bank_info.phase =         ( allocate_rif_bank ) ? BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2 : BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC ;
            eedb_bank_info.force_flags |=  ( allocate_rif_bank ) ? BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE : 0;

            if ( ( bank_id_iter % 2 ) ==0 ) {
                rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id_iter/2,eedb_bank_info.phase);
                BCMDNX_IF_ERR_EXIT(rv); 
            }

            /* In QAX, rifs are not in the EEDB. Don't allocate banks for them. */
            allocate_rif_bank = SOC_IS_JERICHO_AND_BELOW(unit) && bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id);

            eedb_bank_info.type =   ( allocate_rif_bank ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED;
            eedb_bank_info.force_flags |=  ( allocate_rif_bank ) ? BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE : 0;

            eedb_bank_info.nof_free_entries = _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit);
            eedb_bank_info.app_bmp =0;
            eedb_bank_info.extension_bank =0;
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id_iter, &eedb_bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit) ; bank_id_iter++) 
        {
     
            top_bank_info.extension_bank_owner = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED ; 

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.set(unit, bank_id_iter, &top_bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        {
            /*
             * Initialize failover linked lists, including members array.
             */
            uint32                       max_nof_list_members ;

            max_nof_list_members = SOC_DPP_DEFS_GET(unit,nof_out_lifs) ;
            BCMDNX_IF_ERR_EXIT(failover_linked_list_memory_init(unit,_BCM_DPP_AM_LOCAL_LIF_NOF_FAILOVER_IDS,max_nof_list_members) ) ;
        }
        /* 
         * In case Routing is enabled the rif banks must be reserved.
         * mapping out-rif to VSI and remark-profile.
         * Also Bank 0 is used in order to use entry 0 as a NULL entry.
         * In that case Bank 0 must be not allocated for Out-AC.
         * In QAX, the rifs are not in the EEDB, so we don't need to allocate outrifs. 
         */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            /*
             * we only allocate outlif 0 because it's the null entry. 
             * We do it directly since we don't want to set the bank's phase. 
             */
            elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);
            bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);
            outlif_tag = 0;

            /* Allocate a double entry, in case the bank won't be set to ac. */
            rv = bcm_dpp_am_local_outlif_alloc_align(unit, bank_id,BCM_DPP_AM_FLAG_ALLOC_WITH_ID,&outlif_tag, 2,0,2,&elem);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 

            eedb_bank_info.nof_free_entries -= 2;

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id, &eedb_bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 
        } else if (SOC_IS_JERICHO_AND_BELOW(unit)) {
            sal_memset(&rif_info,0,sizeof(bcm_dpp_am_local_out_lif_info_t));
            rif_info.app_alloc_info.pool_id = dpp_am_res_eg_out_rif;
            rif_info.local_lif_flags        = BCM_DPP_AM_OUT_LIF_FLAG_DIRECT;
            rif_info.counting_profile_id    = BCM_DPP_AM_COUNTING_PROFILE_RESERVED;


            for (rif_id_iter = 0 ; rif_id_iter < rif_max_id ; rif_id_iter++) {
                rif_info.base_lif_id = rif_id_iter;
                rif_info.glif = rif_id_iter;

                rv = _bcm_dpp_am_local_outlif_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &rif_info);
            }
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Initialize arp-next-lif info.*/
        if (SOC_IS_JERICHO(unit)) {
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.alloc_bitmap(unit, (_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED + 1));
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.alloc_bitmap(unit, (_BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED + 1));
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.lif_base.set(unit, -1);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.set(unit, 0);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.ready.set(unit, 0);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /*set the local outlif intialization flag to TRUE*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.initalized.set(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.lif_allocated.set(unit, FALSE ));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_local_outlif_alloc(int unit, int flags, bcm_dpp_am_local_out_lif_info_t *lif_info)
{

    int rv ,second_half_id;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access = 0;
    uint8 ext_bank_found = FALSE;
    int ext_bank_id;
    uint8 type, lif_allocated,outlif_tag ;
    int local_outlif_id = 0;
    int bank_id = 0, extension_type_bank_id;
    uint8 bank_found = FALSE, lif_extension = FALSE, lif_even, lif_found = FALSE, lif_ext_type=0, is_half_entry= FALSE;
    uint32 size ;

    BCMDNX_INIT_FUNC_DEFS;

    lif_extension = (DPP_IS_FLAG_SET(lif_info->local_lif_flags, BCM_DPP_AM_OUT_LIF_FLAG_WIDE ) );
    lif_ext_type = (lif_extension && (lif_info->failover_id == 0) ) ? BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_DATA : BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION;
    is_half_entry = (_BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY( lif_info->app_alloc_info.pool_id, lif_info->app_alloc_info.application_type)) ? TRUE : FALSE;
    outlif_tag = _BCM_DPP_AM_LOCAL_OUT_LIF_COUNTING_PROFILE_TO_TAG_ID(unit,lif_info->counting_profile_id);

    /*For J+/QAX the counter profile is allocated differently for wide lif or half entry
        Hence need to check that the counter profile was not allocated with double entry for half_entry. (otherwise allowed)*/
    if(outlif_tag != 0)
    {
        if(SOC_IS_JERICHO_PLUS(unit) == TRUE)
        {
            int counter_for_double_entry = 0;
            rv = _bcm_dpp_am_local_outlif_counting_profile_get(unit,lif_info->counting_profile_id,NULL,NULL,&counter_for_double_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            if((is_half_entry == TRUE) && (counter_for_double_entry == TRUE))
            {
                BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }
        }    
    }
    
    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) 
    {
        /* See documentation of ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX. */
        local_outlif_id = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_info->base_lif_id);
    }
    
    if (is_half_entry && (lif_ext_type == BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION) ) 
    {
        lif_even = (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_EVEN_HALF) ? TRUE : FALSE ; 

        BCMDNX_IF_ERR_EXIT(LIF_LINKED_LISTS_ARRAY_ACCESS.size.get(unit, lif_info->failover_id, &size)) ;
        if (size != 0)
        {
            rv = failover_linked_list_member_find(unit,lif_info->failover_id,(flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) , lif_even,TRUE,&local_outlif_id,&lif_found);
            BCMDNX_IF_ERR_EXIT(rv);

            if (lif_found) 
            {
                goto local_outlif_allocation;
            }
        }
    }

    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
        bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_outlif_id);

        if ((bank_id >= _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_HALF_BANKS(unit)) && lif_extension) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Outlif entry (bank_id %d) doesn't support wide-data!"), bank_id / 2));
        }

        type = ( lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_DIRECT ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ;

        rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, &(lif_info->app_alloc_info), &phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (!eedb_bank_info.valid) /*bank is not allocated yet - allocate if possible*/
        {
            /*force validation - if bank attribures are forced and lif allocated attributes doesn't fit bank attributes, this bank cannot be used*/
            if (_BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, phase_access, type, lif_extension, lif_ext_type))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Outlif entry doesn't match bank type" )));
            }

            second_half_id = ( bank_id % 2) ? bank_id - 1 : bank_id + 1 ; 
            /*set also the phase of other half bank*/
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, second_half_id, &eedb_bank_info2);
            BCMDNX_IF_ERR_EXIT(rv); 

            if  (  ( (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access] != eedb_bank_info.phase) && (eedb_bank_info.phase!= BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC) ) || 
                   ( eedb_bank_info2.valid && (lif_extension != eedb_bank_info.is_extended) ) ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Outlif entry doesn't match bank type" )));
            }

            if (lif_extension && !eedb_bank_info.is_extended) /*Need to allocate new extension bank*/
            {
                    rv = _bcm_dpp_am_local_outlif_extension_bank_alloc(unit,(bank_id/2), &ext_bank_id, &ext_bank_found );
                    BCMDNX_IF_ERR_EXIT(rv); 

                    if (ext_bank_found) 
                    {
                        eedb_bank_info.extension_bank = ext_bank_id;
                        eedb_bank_info.is_extended = lif_extension;
                        eedb_bank_info.ext_type = lif_ext_type;
                        eedb_bank_info2.extension_bank = ext_bank_id;
                        eedb_bank_info2.is_extended = lif_extension;
                        eedb_bank_info2.ext_type = lif_ext_type;

                        /*set extension in HW*/
                        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id/2), TRUE ,(uint32)ext_bank_id ));
                        BCMDNX_IF_ERR_EXIT(rv); 

                        /* Set extension type in HW. This should be done by outlif bank index */
                        extension_type_bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, 0)) 
                                                + bank_id;

                        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_type_set,(unit,(extension_type_bank_id/2), lif_ext_type));
                        BCMDNX_IF_ERR_EXIT(rv);  
                    }
            }

            eedb_bank_info.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];
            eedb_bank_info2.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];

            /* Set HW */
            rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id/2,eedb_bank_info.phase);
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, second_half_id, &eedb_bank_info2);
            BCMDNX_IF_ERR_EXIT(rv); 

            eedb_bank_info.type = type;
            eedb_bank_info.valid = TRUE;

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id,( type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ) ? TRUE : FALSE));
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id, &eedb_bank_info);
            BCMDNX_IF_ERR_EXIT(rv); 

            bank_found = TRUE;
        }
        if ( _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, phase_access, type, lif_extension, lif_ext_type) && eedb_bank_info.nof_free_entries > 0 ) 
        {
            bank_found = TRUE;/*This bank can be used.*/
        }
        else
        {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Requested EEDB bank is full." )));
        }
    }
    else
    {
        rv = _bcm_dpp_am_local_outlif_bank_alloc(unit, lif_info->local_lif_flags, &(lif_info->app_alloc_info), outlif_tag, lif_ext_type,lif_info->failover_id, &bank_id, &bank_found);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

    if ( !bank_found ) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("No free eedb entry." )));
    }

    /* else: Allocation succeeded */

    /*allocate a lif in this bank*/
    rv = _bcm_dpp_am_local_outlif_internal_alloc(unit,bank_id,lif_info, flags,&local_outlif_id);
    BCMDNX_IF_ERR_EXIT(rv); 

local_outlif_allocation: 

    lif_info->ext_lif_id = _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED;
    if (lif_extension)
    {
        rv = _bcm_dpp_am_local_outlif_extension_lif_id_get(unit,local_outlif_id,&(lif_info->ext_lif_id));
        BCMDNX_IF_ERR_EXIT(rv); 
        if (lif_info->ext_lif_id != _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED) {
            lif_info->ext_lif_id = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, lif_info->ext_lif_id);
        }
    }

    /* See documentation of ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF. */
    lif_info->base_lif_id = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, local_outlif_id);

    /*set the local outlif info into SW database in the eedb index. */
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.set(unit, local_outlif_id, lif_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /*set the local inlif intialization flag to TRUE*/
    BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.lif_allocated.get(unit, &lif_allocated ));
    if (!lif_allocated && (lif_info->counting_profile_id != BCM_DPP_AM_COUNTING_PROFILE_RESERVED)) 
    {
       BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.lif_allocated.set(unit, TRUE ));
    }


    /*Map local inlif to global inlif. The glem maps the local outlif, not the eedb index. */
    if (lif_info->glif) {
        rv= _bcm_dpp_global_lif_mapping_create(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, lif_info->glif, lif_info->base_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
 
exit:
    BCMDNX_FUNC_RETURN;

}

uint32
_bcm_dpp_am_local_outlif_arp_pointed_alloc(int unit, int flags, bcm_dpp_am_local_out_lif_info_t *lif_info)
{
    int rv;
    int local_outlif_id, bank_id, elem, eedb_index;
    int lif_base, lif_offset, lif_id, start_lif_id, lif_block_id;
    int nof_lifs, lif_cnt;
    uint8 lif_range_ready, found = FALSE, is_idle, is_res, is_test_alloc = FALSE;
    int nof_lifs_per_bank = SOC_DPP_DEFS_GET(unit, nof_local_lifs_per_bank);
    uint32 temp_flags;

    BCMDNX_INIT_FUNC_DEFS;

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.lif_base.get(unit, &lif_base);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check if the given local-out-lif is available.*/
    local_outlif_id = -1;
    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)
    {
        local_outlif_id = lif_info->base_lif_id;
        if ((lif_base != -1) && ((lif_base ^ local_outlif_id) & (~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK))) {
            BCMDNX_ERR_EXIT_MSG(_SHR_E_PARAM, (_BSL_BCM_MSG("MSBs of given local out lif (%d) mismatches with the shared base (%d)."), local_outlif_id, lif_base));
        }
    }

    /* Check if the ARP pointed local-out-lif is reserved already.*/
    lif_range_ready = FALSE;
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.ready.get(unit, &lif_range_ready);
    BCMDNX_IF_ERR_EXIT(rv);

    if (lif_range_ready == FALSE) {
        /* The lifs are not reserved yet. Reserve them now.*/
        found = FALSE;
        if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
            eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, local_outlif_id);
            start_lif_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(eedb_index);
            bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(eedb_index);
            found = TRUE;
        } else if (lif_base != -1) {
            eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_base);
            start_lif_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(eedb_index);
            bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(eedb_index);
            found = TRUE;
        } else {
            /* Look for a proper start lif id following which there are most available lifs */
            start_lif_id = 0;
            nof_lifs = 0;

            /* Look for a proper bank by allocating with the given local-out-lif-info (testing)*/
            rv = _bcm_dpp_am_local_outlif_alloc(unit, flags, lif_info);
            BCMDNX_IF_ERR_EXIT(rv);
            is_test_alloc = TRUE;

            /* Store the local-lif-id for later usage.*/
            local_outlif_id = lif_info->base_lif_id;

            eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, local_outlif_id);
            elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(eedb_index);
            bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(eedb_index);

            /* Look for a proper start lif id */
            lif_cnt = 1;
            lif_block_id = elem & (~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK);
            for ( ; lif_block_id <= nof_lifs_per_bank; lif_block_id += _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED) {

                for (lif_offset = 0; lif_offset < _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED; lif_offset++) {
                    lif_id = lif_block_id + lif_offset;
                    lif_id = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id, lif_id);
                    lif_id = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, lif_id);
                    rv = _bcm_dpp_am_local_outlif_is_alloc(unit, lif_id);
                    if (rv == _SHR_E_NOT_FOUND) {
                        lif_cnt ++;
                    } else if (rv == _SHR_E_EXISTS) {
                        continue;
                    } else {
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }

                /* Check if current block is proper: with the most available LIFs.*/
                if (lif_cnt == _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED) {
                    nof_lifs = lif_cnt;
                    start_lif_id = lif_block_id;
                    break;
                } else if (lif_cnt > nof_lifs) {
                    nof_lifs = lif_cnt;
                    start_lif_id = lif_block_id;
                }

                /* For the next time iterating*/
                lif_cnt = 0;
            }

            if (nof_lifs != 0) {
                found = TRUE;
            }
        }

        if (found == FALSE) {
            BCMDNX_IF_ERR_EXIT(_SHR_E_FULL);
        }

        /* Calculate the possible lif range */
        lif_offset = start_lif_id & _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK;
        start_lif_id -= lif_offset;

        nof_lifs = _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED;
        if (start_lif_id + _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED >= nof_lifs_per_bank) {
            nof_lifs = nof_lifs_per_bank - start_lif_id;
        }

        /* Allocate all possible LIFs in the range */
        temp_flags = flags | BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        for (lif_offset = 0; lif_offset < nof_lifs; lif_offset++) {
            lif_id = start_lif_id + lif_offset;
            lif_id = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id, lif_id);
            if (lif_id == eedb_index) {
                /* Allocated already for arp pointed testing*/
                rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_set(unit, lif_offset);
                BCMDNX_IF_ERR_EXIT(rv);
                continue;
            }

            rv = _bcm_dpp_am_local_outlif_internal_alloc(unit, bank_id, lif_info, temp_flags, &lif_id);
            if ((rv == _SHR_E_EXISTS) || (rv == _SHR_E_RESOURCE)) {
                /* This local-out-lif is allocated already in some usage, skip it.*/
                continue;
            }
            BCMDNX_IF_ERR_EXIT(rv);

            /* Store the offset in sw state if success */
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_set(unit, lif_offset);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_set(unit, lif_offset);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Store the local outlif info into SW database in the eedb index. */
            lif_info->base_lif_id = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, lif_id);
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.set(unit, lif_id, lif_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Now, suppose the required local-out-lif is not allocated yet. */
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.set(unit, 0);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set the lif_base in sw state */
        if (lif_base == -1) {
            lif_base = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id, start_lif_id);
            lif_base = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, lif_base);
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.lif_base.set(unit, lif_base);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.ready.set(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        if (lif_base == -1) {
            BCMDNX_ERR_EXIT_MSG(_SHR_E_PARAM, (_BSL_BCM_MSG("MSBs of given local out lif (%d) should be allocated already."), lif_base));
        }
    }

    /* Allocate a local-out-lif from sw state.*/
    found = FALSE;
    lif_offset = 0;
    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
        eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, local_outlif_id);
        lif_offset = eedb_index & _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK;
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_get(unit, lif_offset, &is_res);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_get(unit, lif_offset, &is_idle);
        BCMDNX_IF_ERR_EXIT(rv);
        if ((is_res == TRUE) && (is_idle == TRUE)) {
            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_clear(unit, lif_offset);
            BCMDNX_IF_ERR_EXIT(rv);
            found = TRUE;
        }
    } else {
        /* Check if the test allocated one is applicable.*/
        if ((is_test_alloc == TRUE) && !((lif_base ^ local_outlif_id) & (~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK))) {
            found = TRUE;
            lif_info->base_lif_id = local_outlif_id;
        } else {
            if (is_test_alloc == TRUE) {
                /* De-alloc the test allocation.*/
                rv = _bcm_dpp_am_local_outlif_dealloc(unit, local_outlif_id);
                if (rv == _SHR_E_NOT_FOUND) {
                    rv = _SHR_E_NONE;
                }
                BCMDNX_IF_ERR_EXIT(rv);
            }
            is_test_alloc = FALSE;
            for (lif_offset = 0; lif_offset < _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED; lif_offset++) {
                rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_get(unit, lif_offset, &is_res);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_get(unit, lif_offset, &is_idle);
                BCMDNX_IF_ERR_EXIT(rv);

                if ((is_res == TRUE) && (is_idle == TRUE)) {
                    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_clear(unit, lif_offset);
                    BCMDNX_IF_ERR_EXIT(rv);
                    found = TRUE;
                    break;
                }
            }
        }
    }

    if ((is_test_alloc == FALSE) && (found == FALSE)) {
        BCMDNX_IF_ERR_EXIT(_SHR_E_FULL);
    }

    if ((is_test_alloc == FALSE) && (found == TRUE)) {
        eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_base);
        lif_id = (eedb_index & ~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK) | lif_offset;

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.set(unit, lif_id, lif_info);
        BCMDNX_IF_ERR_EXIT(rv);

        lif_info->base_lif_id = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, lif_id);

        /*Map local inlif to global inlif. The glem maps the local outlif, not the eedb index. */
        rv= _bcm_dpp_global_lif_mapping_create(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, lif_info->glif, lif_info->base_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* increase counter for the used local-out-lif*/
    lif_cnt = 0;
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.get(unit, &lif_cnt);
    BCMDNX_IF_ERR_EXIT(rv);
    lif_cnt++;
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.set(unit, lif_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_local_outlif_arp_pointed_dealloc(int unit, int lif_index)
{
    int rv;
    int lif_id, lif_offset, nof_lifs, eedb_index;
    uint8 is_res, is_idle;

    BCMDNX_INIT_FUNC_DEFS;

    /* See documentation of ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX. */
    eedb_index = lif_index;

    lif_offset = eedb_index & _BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK;
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_get(unit, lif_offset, &is_res);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_get(unit, lif_offset, &is_idle);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_res && !is_idle) {
        /* Alloc the lif by setting the alloc bitmap: 1 = not-used.*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_set(unit, lif_offset);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Decrease the allocation counter.*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.get(unit, &nof_lifs);
        BCMDNX_IF_ERR_EXIT(rv);
        nof_lifs --;
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.nof_alloc_lifs.set(unit, nof_lifs);
        BCMDNX_IF_ERR_EXIT(rv);

        if (nof_lifs == 0) {
            /* No arp pointed lif is in-use, de-alloc all the resource.*/
            for (lif_offset = 0; lif_offset < _BCM_DPP_AM_LOCAL_OUT_LIF_NOF_ARP_POINTED; lif_offset++) {
                rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_get(unit, lif_offset, &is_res);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_get(unit, lif_offset, &is_idle);
                BCMDNX_IF_ERR_EXIT(rv);
                if (is_res && is_idle) {
                    lif_id = (eedb_index & ~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK) | lif_offset;
                    rv = _bcm_dpp_am_local_outlif_internal_dealloc(unit, lif_id);
                    if (rv == _SHR_E_NOT_FOUND) {
                        rv = _SHR_E_NONE;
                    }
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.res_bmp.bit_clear(unit, lif_offset);
                    BCMDNX_IF_ERR_EXIT(rv);

                    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.alloc_bmp.bit_clear(unit, lif_offset);
                    BCMDNX_IF_ERR_EXIT(rv);
                } else {
                    /* Error in arp pointed lif management. If no lif is in-use, alloc_bmp should be equal to res_bmp.*/
                    BCMDNX_IF_ERR_EXIT(_SHR_E_INTERNAL);
                }
            }

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.ready.set(unit, FALSE);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.lif_base.set(unit, -1);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (is_res) {
        BCMDNX_IF_ERR_EXIT(_SHR_E_NONE);
    } else {
        BCMDNX_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

exit:
    BCMDNX_FUNC_RETURN;
} 


uint32
_bcm_dpp_am_local_outlif_dealloc(int unit,  int lif_index)
{
    int rv; 
    int local_outlif_id;
    int lif_base;

    BCMDNX_INIT_FUNC_DEFS;

    /* See documentation of ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX. */
    local_outlif_id = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_index);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.arp_next_lif.lif_base.get(unit, &lif_base);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((lif_base != -1) && !((lif_index ^ lif_base) & ~_BCM_DPP_AM_LOCAL_OUT_LIF_ARP_POINTED_LSB_MASK)) {
        rv = _bcm_dpp_am_local_outlif_arp_pointed_dealloc(unit, local_outlif_id); 
    } else {
        rv = _bcm_dpp_am_local_outlif_internal_dealloc(unit, local_outlif_id); 
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy mapping of local outlif to global inlif*/
    rv = _bcm_dpp_global_lif_mapping_remove(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL | _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, 0, lif_index);
    BCMDNX_IF_ERR_EXIT(rv) ;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_local_outlif_dealloc_only
 * Purpose:
 *      Dealloc egress local LIF only without dealloc the global LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     lif_index       - (IN) egress local LIF index.
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_outlif_dealloc_only(int unit,  int lif_index)
{
    int rv; 
    int local_outlif_id;

    BCMDNX_INIT_FUNC_DEFS;

    /* See documentation of ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX. */
    local_outlif_id = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_index);
 
    rv = _bcm_dpp_am_local_outlif_internal_dealloc(unit, local_outlif_id); 
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC uint32
_bcm_dpp_am_local_outlif_internal_dealloc(int unit, int local_lif)
{
    int rv ;
    int bank_id,  elem , flags, count , second_half_id,  neighbor_lif;
    uint8 lif_found = FALSE;
    bcm_dpp_am_local_out_lif_info_t outlif_info;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    int failover_link_list, eedb_nof_entries_per_half_bank, extension_type_bank_id;
    BCMDNX_INIT_FUNC_DEFS;

    elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(local_lif);
    bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_lif);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.data_base.get(unit, local_lif, &outlif_info);
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  outlif_info.local_lif_flags ; 

    /*number of entries to deallocate*/
    count = ( _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY(outlif_info.app_alloc_info.pool_id, outlif_info.app_alloc_info.application_type)) ? 1 : 2;

    if ((outlif_info.failover_id !=0) && (count==1)) /*protection outlif*/
    {
        failover_link_list = outlif_info.failover_id ;

        neighbor_lif = ( local_lif % 2  ) ? local_lif-1 : local_lif+1 ; 

        rv = failover_linked_list_member_remove(unit, failover_link_list, neighbor_lif, &lif_found);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (lif_found ) 
        {
            /*elem will always be the even lif id of this pair*/
            elem = ( local_lif % 2  ) ? (elem-1) : elem;
            count = 2;
        } 
        else 
        {
            /*Add lif_id to list*/
           rv = failover_linked_list_member_add( unit, failover_link_list, local_lif, ( ( local_lif % 2 ) ? FALSE : TRUE ));
           BCMDNX_IF_ERR_EXIT(rv);

           count = 0;
        }
    }

    if ( count != 0 ) {
        rv = bcm_dpp_am_local_outlif_dealloc(unit,bank_id,flags,count, elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv); 

    eedb_bank_info.nof_free_entries += count;

    eedb_nof_entries_per_half_bank = _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit);
    if (SOC_IS_JERICHO_PLUS(unit) && (bank_id == 0))
    {
        /* 
         * We have a double entry allocated for null entry during init in bank 0.
         * See _bcm_dpp_am_local_outlif_init.
         */
        eedb_nof_entries_per_half_bank -= 2;
    }
    if ( eedb_bank_info.nof_free_entries == eedb_nof_entries_per_half_bank) 
    {
        eedb_bank_info.valid = FALSE;

        second_half_id = ( bank_id % 2) ? bank_id - 1 : bank_id + 1 ;
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, second_half_id, &eedb_bank_info2);
        BCMDNX_IF_ERR_EXIT(rv);

        /*
          * If another half bank is still valid,
          * We should not change phase of
          * this half bank.Else, when this bank
          * is asked for new LIF again, it will
          * change the phase number wrongly.
          * Now this half bank is still avaliable
          * for new out LIF.
          * but phase number keep the current one.
          */
        if (eedb_bank_info2.valid) {
            goto eedb_info_write;
        }

        /*
          * Both half banks invalid, now we could 
          * reset the phase state.
          */
        if ( !(eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE) )
        {
            eedb_bank_info.phase = BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC;
        }
        if ( !(eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE) )
        {
            eedb_bank_info.type = BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED;
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id,TRUE));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        /*If the second half of the EEDB bank is also empty - clear phase, extension, and dealloc extension bank*/
        if ( !(eedb_bank_info2.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE) )
        {
            eedb_bank_info2.phase = BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC;
        }

        if (eedb_bank_info.is_extended)
        {
            eedb_bank_info.is_extended = FALSE;
            eedb_bank_info2.is_extended = FALSE;
            eedb_bank_info.ext_type = BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION;
            eedb_bank_info2.ext_type = BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_PROTECTION;
            eedb_bank_info2.extension_bank = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;

            rv = _bcm_dpp_am_local_outlif_extension_bank_dealloc(unit,bank_id/2 );
            BCMDNX_IF_ERR_EXIT(rv); 

            /*set extension in HW*/
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id/2), FALSE ,eedb_bank_info.extension_bank ));
            BCMDNX_IF_ERR_EXIT(rv); 
            eedb_bank_info.extension_bank =  _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;

            /*set extension type in HW*/

            /* Set extension type in HW. This should be done by outlif bank index */
            extension_type_bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, 0)) 
                                     + bank_id;
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_type_set,(unit,(extension_type_bank_id/2), FALSE));
            BCMDNX_IF_ERR_EXIT(rv);  
        }

        /* Set HW */
        rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id/2,BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC);
        BCMDNX_IF_ERR_EXIT(rv); 

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, second_half_id, &eedb_bank_info2);
        BCMDNX_IF_ERR_EXIT(rv); 

    }

eedb_info_write:
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id, &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv); 

   
exit:
    BCMDNX_FUNC_RETURN;

}


STATIC uint32 
_bcm_dpp_am_local_outlif_internal_alloc(int unit, uint8 bank_id, bcm_dpp_am_local_out_lif_info_t *lif_info, int flags, int *local_lif)
{
    int rv, elem ,tmp_elem, free_lif;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    uint8 free_lif_even = FALSE;
    uint8 outlif_tag;

    BCMDNX_INIT_FUNC_DEFS;

    outlif_tag = _BCM_DPP_AM_LOCAL_OUT_LIF_COUNTING_PROFILE_TO_TAG_ID(unit,lif_info->counting_profile_id);

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
        elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(*local_lif);
        tmp_elem = elem;
    }

    if (_BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY( lif_info->app_alloc_info.pool_id, lif_info->app_alloc_info.application_type) )
    {
        if ( lif_info->failover_id != 0 ) /*protection outlif*/
        {
            rv = bcm_dpp_am_local_outlif_alloc_align(unit, bank_id,flags,&outlif_tag, 2,0,2,&tmp_elem);
            BCMDNX_IF_ERR_EXIT(rv);

            elem = (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_ODD_HALF ) ? tmp_elem+1 : tmp_elem;
            free_lif = (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_ODD_HALF ) ? _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id,tmp_elem) : _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id,tmp_elem+1);
            free_lif_even = (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_ODD_HALF ) ? TRUE : FALSE;

            rv = failover_linked_list_member_add( unit, lif_info->failover_id, free_lif, free_lif_even);
            BCMDNX_IF_ERR_EXIT(rv);
            eedb_bank_info.nof_free_entries -= 2;

        }
        else
        {
            if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.pon_dscp_remarking && (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_EVEN_HALF)) {
                rv = bcm_dpp_am_local_outlif_alloc_align(unit, bank_id,flags,&outlif_tag, 2,0,1,&elem);
                BCMDNX_IF_ERR_EXIT(rv);
                eedb_bank_info.nof_free_entries -= 2;
            } else {
                rv = bcm_dpp_am_local_outlif_alloc(unit,bank_id,flags,&outlif_tag,&elem);
                BCMDNX_IF_ERR_EXIT(rv);
                eedb_bank_info.nof_free_entries--;
            }
        }
    }
    else
    {
        rv = bcm_dpp_am_local_outlif_alloc_align(unit, bank_id,flags,&outlif_tag, 2,0,2,&elem);
        BCMDNX_IF_ERR_EXIT(rv);

        eedb_bank_info.nof_free_entries -= 2;
    }

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id, &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv); 

    *local_lif = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id,elem);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC uint32
_bcm_dpp_am_local_outlif_bank_alloc(int unit, int flags, bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info, uint8 tag, uint8 ext_type,int failover_id, int *bank_id , uint8 *bank_found  )
{
    int rv , bank_id_iter, second_half_id;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access = 0;
    uint64 checked_bank_bmp;
    uint8 lif_extension;
    uint8 ext_bank_found = FALSE;
    int ext_bank_id, tmp_elem;
    /*For regular lif we need full entry of EEDB means 2 entry*/
    int alloc_count = 2, alloc_align = 2;
    int nof_banks;

    uint8 type = ( flags & BCM_DPP_AM_OUT_LIF_FLAG_DIRECT ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ;

    BCMDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(checked_bank_bmp);

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, app_alloc_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    lif_extension = (DPP_IS_FLAG_SET(flags, BCM_DPP_AM_OUT_LIF_FLAG_WIDE ));

    /*For Half entry LIF types need to change the alloc count */
    if (_BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY( app_alloc_info->pool_id, app_alloc_info->application_type) &&
        (failover_id == 0))
    {
        alloc_align = 1;
        alloc_count = 1;
    }

    /* PON dscp remarking need even AC, so it will take 2 half entry. 
     * The odd entry is reserved and not used
     */
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.pon_dscp_remarking && (flags & BCM_DPP_AM_OUT_LIF_FLAG_EVEN_HALF))
    {
        alloc_align = 2;
        alloc_count = 2;
    }

    /** If lif extension is required, only regular banks are available. */
    if (lif_extension) {
        nof_banks = _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_HALF_BANKS(unit);
    } else {
        nof_banks = _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit);
    }

    /*try to find an allocated bank*/
    for (bank_id_iter = 0 ; bank_id_iter < nof_banks; bank_id_iter++) 
    {
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id_iter, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (!eedb_bank_info.valid) /*bank is not allocated yet - continue*/
        {
            continue;
        }
        if ( _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, phase_access, type, lif_extension, ext_type) && eedb_bank_info.nof_free_entries > 0 ) 
        {
            if ( bcm_dpp_am_local_outlif_alloc_align(unit, bank_id_iter,  SW_STATE_RES_ALLOC_CHECK_ONLY  ,&tag, alloc_align,0,alloc_count,&tmp_elem)== BCM_E_NONE ) 
            {
                *bank_found = TRUE;
                *bank_id = bank_id_iter;
                break;
            }

        } else {
            COMPILER_64_BITSET(checked_bank_bmp, bank_id_iter);
            continue;
        }
    }

    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; (bank_id_iter < nof_banks) && (*bank_found == FALSE); bank_id_iter++) 
    {
        /*if force in front 16 eedb bank, then verify bank_id_iter half bank*/
        if((flags & BCM_DPP_AM_OUT_LIF_FLAG_FORCE_FRONT_SIXTEEN) && (bank_id_iter >= 32 )) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("No free eedb entry in front 16 bank." )));
        }

        if (COMPILER_64_BITTEST(checked_bank_bmp, bank_id_iter))
        {
            continue; /*no need to check this bank  - it can't be used*/
        }

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, bank_id_iter, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 

        /*force validation - if bank attribures are forced and lif allocated attributes doesn't fit bank attributes, this bank cannot be used*/
        if (_BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, phase_access, type, lif_extension, ext_type))
        {
            continue;
        }

        if ( bcm_dpp_am_local_outlif_alloc_align(unit, bank_id_iter,  SW_STATE_RES_ALLOC_CHECK_ONLY ,&tag, alloc_align,0,alloc_count,&tmp_elem) != BCM_E_NONE ) 
        {
           continue;
        }

        second_half_id = ( bank_id_iter % 2) ? bank_id_iter - 1 : bank_id_iter + 1 ; 
        /*set also the phase of other half bank*/
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, second_half_id, &eedb_bank_info2);
        BCMDNX_IF_ERR_EXIT(rv); 

        if  (  ( (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access] != eedb_bank_info.phase) && (eedb_bank_info.phase!= BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC) ) || 
               ( eedb_bank_info2.valid && ((lif_extension != eedb_bank_info.is_extended) || (ext_type != eedb_bank_info2.ext_type)) ) ) 
        {
            continue;
        }

        if (lif_extension && !eedb_bank_info.is_extended) /*Need to allocate new extension bank*/
        {
                int extension_type_bank_id = 0;
                rv = _bcm_dpp_am_local_outlif_extension_bank_alloc(unit,(bank_id_iter/2), &ext_bank_id, &ext_bank_found );
                BCMDNX_IF_ERR_EXIT(rv); 

                if (ext_bank_found) 
                {
                    eedb_bank_info.extension_bank = ext_bank_id;
                    eedb_bank_info.is_extended = lif_extension;
                    eedb_bank_info.ext_type = ext_type;
                    eedb_bank_info2.extension_bank = ext_bank_id;
                    eedb_bank_info2.is_extended = lif_extension;
                    eedb_bank_info2.ext_type = ext_type;
                }

                /* Set extension in HW. Should be done by EEDB bank index.*/
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id_iter/2), TRUE ,(uint32)ext_bank_id ));
                BCMDNX_IF_ERR_EXIT(rv); 

                /* Set extension type. This should be done by outlif bank index */
                extension_type_bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, 0)) 
                                            + bank_id_iter;
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_type_set,(unit,(extension_type_bank_id/2), ext_type));
                BCMDNX_IF_ERR_EXIT(rv);

                BCMDNX_IF_ERR_EXIT(rv);  

        }

        eedb_bank_info.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];
        eedb_bank_info2.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];

        /* Set HW */
        rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id_iter/2,eedb_bank_info.phase);
        BCMDNX_IF_ERR_EXIT(rv); 

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, second_half_id, &eedb_bank_info2);
        BCMDNX_IF_ERR_EXIT(rv); 

        eedb_bank_info.type = type;
        eedb_bank_info.valid = TRUE;

        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id_iter,( type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ) ? TRUE : FALSE));
        BCMDNX_IF_ERR_EXIT(rv); 

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.set(unit, bank_id_iter, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 

        *bank_found = TRUE;
        *bank_id = bank_id_iter;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_local_outlif_is_alloc(int unit, int lif_index)
{
    int rv; 
    uint8 bank_id;
    int elem;
    int eedb_index;

    BCMDNX_INIT_FUNC_DEFS;

    eedb_index = ARAD_PP_EG_ENCAP_OUTLIF_TO_EEDB_INDEX(unit, lif_index);

    elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(eedb_index);
    bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(eedb_index);

    rv = bcm_dpp_am_local_outlif_is_alloc(unit,bank_id,0,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}


uint32
_bcm_dpp_am_local_outlif_extension_bank_alloc(int unit, int bank_id, int *ext_bank_id, uint8 *bank_found)
{
    int rv; 
    uint8 bank_id_iter;
    uint8 eedb_half_bank;
    int alloc_elem_id, start_elem;
    uint8 tag = 0;
    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; ( bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit) ) && (*bank_found == FALSE )  ; bank_id_iter++) 
    {
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.get(unit, bank_id_iter, &top_bank_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (top_bank_info.extension_bank_owner == _BCM_DPP_AM_EEDB_BANK_UNASSIGNED)   /*This top bank can be used as an extension*/
        {
            *bank_found = TRUE;
            *ext_bank_id = bank_id_iter;
            top_bank_info.extension_bank_owner = bank_id;

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.set(unit, bank_id_iter, &top_bank_info);
            BCMDNX_IF_ERR_EXIT(rv);

            eedb_half_bank = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(unit, bank_id_iter);
            start_elem = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(bank_id_iter);

            /* allocate the 1/4 bank in the regular bank pull-ids*/
            for ( alloc_elem_id = start_elem ; alloc_elem_id < start_elem + _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(unit) /4  ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_outlif_alloc(unit,eedb_half_bank,BCM_DPP_AM_FLAG_ALLOC_WITH_ID,&tag, &alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
    }

    if (*bank_found == FALSE)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("Out of extenstion banks." )));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


uint32
_bcm_dpp_am_local_outlif_extension_bank_dealloc(int unit, int bank_id)
{
    int rv; 
    uint8 bank_id_iter;
    uint8 eedb_half_bank;
    int alloc_elem_id, start_elem;
    uint8 bank_found = FALSE;
    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; ( bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS(unit) ) && (bank_found == FALSE )  ; bank_id_iter++) 
    {
        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.get(unit, bank_id_iter, &top_bank_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (top_bank_info.extension_bank_owner == bank_id)   
        {
            bank_found = TRUE;
            top_bank_info.extension_bank_owner = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;

            rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.top_banks.set(unit, bank_id_iter, &top_bank_info);
            BCMDNX_IF_ERR_EXIT(rv);

            eedb_half_bank = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(unit, bank_id_iter);
            start_elem = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(bank_id_iter);

            /* de-allocate the 1/4 bank in the regular bank pull-ids*/
            for ( alloc_elem_id = start_elem ; alloc_elem_id < start_elem + _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK(unit) /4   ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_outlif_dealloc(unit,eedb_half_bank,BCM_DPP_AM_FLAG_ALLOC_WITH_ID, 1, alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
    }

    if (bank_found == FALSE)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Given extension bank is illegal" )));
    }

exit:
    BCMDNX_FUNC_RETURN;

}

STATIC uint32
  _bcm_dpp_am_local_outlif_extension_lif_id_get(int unit, int base_lif,  int *ext_lif)
{
    int rv, ext_bank_id, ext_offset; 
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.eedb_info.banks.get(unit, _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(base_lif), &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (eedb_bank_info.is_extended)
    {
        ext_bank_id = (ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(unit) * 2 + eedb_bank_info.extension_bank / _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_HALF_BANK );

        /*get the extension offset in the extension TOP bank: 
          There are 2 lif entries per EEDB line. get the line number in the bank
          Each line corresponds to 1/4 line in top bank. get the offset lif id from offset line in extension bank with multiply by 2 */
        ext_offset =  ( ( ( base_lif / 2) %_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit) ) /4)* 2 + ( _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_TOP_BANK(unit) * ( eedb_bank_info.extension_bank % _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_EEDB_HALF_BANK )  );

        *ext_lif = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(ext_bank_id, ext_offset) ;
    }
    else
    {
        *ext_lif = _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


uint32
  _bcm_dpp_am_local_outlif_bank_phase_set(int unit, int bank_id,  int phase)
{

    SOC_PPC_EG_ENCAP_RANGE_INFO encap_range_info = {0};  
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Set HW */
    soc_sand_rv = soc_ppd_eg_encap_range_info_get(unit, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    encap_range_info.bank_access_phase[bank_id] = _bcm_dpp_am_local_outlif_bank_phase_to_access_phase(unit, phase);

    soc_sand_rv = soc_ppd_eg_encap_range_info_set(unit, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_outlif_counting_profile_set
 * Purpose:
 *      Set a counting profile for ingress LIF
 *  
 * Parameters:
 *     unit                       - (IN) Device number
 *     counting_profile_id        - (IN) counter profile id
 *     base                       - (IN) base LIF ID for the counting profile
 *     size                       - (IN) number of LIFs in the couner profile
 *     double_entry               - (IN) If the counter is allocated for wide Lif's
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_outlif_counting_profile_set(int unit, int counting_profile_id, int base, int size, int double_entry)
{
    int rv; 
    bcm_dpp_am_local_lif_counter_profile_info_t counter_profile_info;
    uint8 profile_tag, start_bank_id, end_bank_id, bank_id_iter,lif_allocated;
    int start_elem, remaining_size, start_elem_iter,nof_elements_to_tag_in_bank ;
    uint32 flags = 0; 
    int local_base = base;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify that no other inLIFs were allocated so far*/
    BCMDNX_IF_ERR_EXIT(ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.lif_allocated.get(unit, &lif_allocated ));
    if (lif_allocated) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("No Egress LIFs should be allocated prior to counting profile set." )));
    }

    /*verify base is not in the reserved range*/
    if(base < 0)
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("base < 0" )));        
    }
    if ((!SOC_IS_JERICHO_PLUS(unit)) && (base < SOC_DPP_CONFIG(unit)->l3.nof_rifs))
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("base is in the Egress local lif reserved range." )));
    }

    /* if Lif, need to save entries in the EEDB table. if RIF, no need to save*/
    if(base >= SOC_DPP_CONFIG(unit)->l3.nof_rifs)
    {
        /* in jericho plus the local lif will be shifted by the nof_rifs, therfore, we compensate on this offset here */
        if(SOC_IS_JERICHO_PLUS(unit))
        {
            local_base = base - SOC_DPP_CONFIG(unit)->l3.nof_rifs;    
        }
        else
        {
            local_base = base;    
        }    
        start_elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(local_base);
        start_bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_base);
        end_bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID((local_base + size - 1));
        profile_tag = _BCM_DPP_AM_LOCAL_OUT_LIF_COUNTING_PROFILE_TO_TAG_ID(unit,counting_profile_id);


        remaining_size = size;

        /*check same bank - If same bank just tag all LIFs in this range in the bank*/
        if (start_bank_id == end_bank_id) 
        {
            rv = bcm_dpp_am_local_outlif_range_set(unit,start_bank_id, flags, profile_tag, start_elem, remaining_size );
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            for (bank_id_iter=start_bank_id ; bank_id_iter <= end_bank_id ; bank_id_iter++) 
            {
                start_elem_iter = (bank_id_iter == start_bank_id) ? start_elem : 0;
                nof_elements_to_tag_in_bank = SOC_SAND_MIN(_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit)-start_elem_iter,remaining_size);
                remaining_size -= nof_elements_to_tag_in_bank;

                rv = bcm_dpp_am_local_outlif_range_set(unit,bank_id_iter, flags, profile_tag, start_elem_iter, nof_elements_to_tag_in_bank );
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }    
    }


    /*set the counter profile in sw-state*/
    counter_profile_info.base_offset = base;
    counter_profile_info.size = size ;
    counter_profile_info.double_entry = double_entry;    
    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.counter_profile_info.set(unit, counting_profile_id, &counter_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *     _bcm_dpp_am_local_outlif_counting_profile_get
 * Purpose:
 *      Get a counting profile for ingress LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     counting_profile_id        - (IN) counter profile id
 *     base                       - (OUT) base LIF ID for the counting profile
 *     size                       - (OUT) number of LIFs in the couner profile
  *    double_entry         - (OUT) If the counter is allocated for wide Lif's
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */

uint32
_bcm_dpp_am_local_outlif_counting_profile_get(int unit, int counting_profile_id, int *base, int *size, int *double_entry)
{
    int rv = BCM_E_NONE; 
    bcm_dpp_am_local_lif_counter_profile_info_t counter_profile_info;
    BCMDNX_INIT_FUNC_DEFS;

    rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.outlif_info.counter_profile_info.get(unit, counting_profile_id, &counter_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);
    if(base != NULL)
    {
        *base = counter_profile_info.base_offset;
    }
    if(size != NULL)
    {
        *size = counter_profile_info.size;
    }
    if(double_entry != NULL)
    {
        *double_entry = counter_profile_info.double_entry;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* indicate whether an allocated lif is a wide entry (fetch information from SW DB) */
uint32
_bcm_dpp_local_lif_is_wide_entry(int unit, int local_lif, int is_ingress, int *is_wide_entry)
{
    int rv = BCM_E_NONE; 

    BCMDNX_INIT_FUNC_DEFS;

    /* The default case is that the entry is not wide */
    *is_wide_entry = FALSE;


    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* Wide entries are irrelevant in aradplus and below */
        BCM_EXIT;
    }

    if (is_ingress) {
        bcm_dpp_am_local_inlif_info_t inlif_info;

        rv = ALLOC_MNGR_LOCAL_LIF_ACCESS.inlif_info.data_base.get(unit, local_lif, &inlif_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (DPP_IS_FLAG_SET(inlif_info.local_lif_flags, BCM_DPP_AM_IN_LIF_FLAG_WIDE)){
            *is_wide_entry = 1;
        }
    } else {
        bcm_dpp_am_local_out_lif_info_t outlif_info;

        rv = SOC_PPC_OUTLIF_SW_STATE_INFO_GET(unit, local_lif, &outlif_info);
        BCMDNX_IF_ERR_EXIT(rv);

        if (DPP_IS_FLAG_SET(outlif_info.local_lif_flags, BCM_DPP_AM_OUT_LIF_FLAG_WIDE)){
            *is_wide_entry = 1;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;

}





