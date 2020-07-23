
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/trunk_sw_db.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/PPD/ppd_api_general.h>






#define SOC_PPC_FRWRD_FEC_PROTECT_TYPE_MAX                       (SOC_PPC_NOF_FRWRD_FEC_PROTECT_TYPES-1)
#define ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MAX                   (ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT-1) 
#define ARAD_PP_FRWRD_FEC_UP_MAX                                 (1)

#define ARAD_PP_FRWRD_FEC_TRAP_ABLE_NOF_FECS                      (64)
#define ARAD_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX                    (SOC_DPP_DEFS_GET(unit, nof_fecs) - 1)
#define ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX                    (SOC_DPP_DEFS_GET(unit, nof_fecs) - ARAD_PP_FRWRD_FEC_TRAP_ABLE_NOF_FECS)
#define ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_MAX                     (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_FEC_TYPE_MAX                               (SOC_PPC_NOF_FEC_TYPES_ARAD-1)
#define ARAD_PP_FRWRD_FEC_EEP_MIN                                (1)
#define ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_MAX                    (ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT-1)
#define ARAD_PP_FRWRD_FEC_VALUE_MAX                              (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_FEC_KEY_SHIFT_MAX                          (15)
#define ARAD_PP_FRWRD_FEC_ECMP_MIN_SIZE                          (1)


#define ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT    \
        (SOC_IS_QUX(unit)?(SOC_DPP_DEFS_GET(unit, nof_failover_ingress_ids) - 1):(SOC_DPP_DEFS_GET(unit, nof_failover_fec_ids) + 1))

#define ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT   (ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT - 1)



#define ARAD_PP_FRWRD_FEC_ECMP_IS_STATEFUL_BITMAP_SIZE_IN_BITS    (4)





#define ARAD_PP_FRWRD_FEC_EVEN_FEC(fec_ndx) (((fec_ndx)%2) == 0)
#define ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx) ((fec_ndx)+1)
#define ARAD_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx) ((fec_ndx)/2)

#define ARAD_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_ins) \
  (oam_ins>>3)


#define ARAD_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(port_num) \
  (port_num>>3)

#define ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id) \
  ((fec_id>>4)*2 + ((fec_id%2==0)?0:1))

#define ARAD_PP_IHB_FEC_ENTRY_ACCESSED_GET_BIT_POSITION(fec_id_ndx) \
    ((fec_id_ndx >> 1) & 0x7)









CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_frwrd_fec[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_GET_ERRS_PTR),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ONE_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_ONE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_SIZE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_FEC_PROTECT_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_DEST_ENCODE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_FEC_DEST_DECODE),
   
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_fec[] =
{
  
  {
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'protect_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_FEC_PROTECT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_FEC_PROTECT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 32768.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'oam_instance_ndx' is out of range. \n\r "
    "The range is: Arad - B: 0-2*1024-2, T20E:0-16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR",
    "The parameter 'up' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR",
    "The parameter 'ecmp_sizes' is out of range. \n\r "
    "in Arad, supported EMCP sizes, are 0..511 \n\r "
    "and info->ecmp_sizes[ind] must be equal to indx \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'ecmp_sizes_nof_entries' is out of range. \n\r "
    "The range is: 1 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'rpf_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_FEC_RPF_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'dist_tree_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FEC_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 2 - 12K.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'oam_instance_id' is out of range. \n\r "
    "The range is: Arad - B:0-2*1024-2, T20E:0-16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR",
    "The parameter 'value' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR",
    "The parameter 'seed' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'hash_func_id' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'key_shift' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_headers' is out of range. \n\r "
    "The range is: 1 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR,
    "ARAD_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR",
    "the ECMP's size (to add) doesn't \n\r "
    "match any of the size set by \n\r "
    "soc_ppd_frwrd_fec_glbl_info_set()\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR",
    "ECMP size index is out of range \n\r "
    "range is 0-min(15,global configuration \n\r "
    "(ecmp_sizes_nof_entries)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR,
    "ARAD_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR",
    "protected FEC has to start in even index (fec_ndx)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR",
    "ECMP has to start in even index (fec_ndx)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,
    "ARAD_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR",
    "update ECMP using soc_ppd_frwrd_fec_ecmp_update()\n\r "
    "where end/start index is out of ECMP size \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR",
    "update ECMP using soc_ppd_frwrd_fec_ecmp_update()\n\r "
    "where end < start \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR",
    "illegal destination type. in FEC entry \n\r "
    "destination cannot be route,FEC,Trap\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR",
    "value of the destination is out of range\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR,
    "ARAD_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR",
    "add/update FEC entry for IP-MC application\n\r "
    "where FEC.destination is not Multicast\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR",
    "add/update FEC entry for IP-MC application\n\r "
    "where RPF check is for IP-UC\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR",
    "add/update FEC, exactly one UC RPF mode has\n\r "
    "to be present UC_STRICT or UC_LOOSE \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR,
    "ARAD_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR",
    "calling soc_ppd_frwrd_fec_entry_get() where fec_ndx \n\r "
    "points to ECMP\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR,
    "ARAD_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR",
    "calling soc_ppd_frwrd_fec_ecmp_get() where fec_ndx \n\r "
    "points to protected FEC\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR,
    "ARAD_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR",
    "for physical system port > 4k - 256 status \n\r "
    "must be UP = true\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR",
    "trap if accessed supported only to FEC-ids in the \n\r "
    "range 0-63 \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR",
    "accessed cannot be set, relevant only for get\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_ROUTING_ERR,
    "ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_ROUTING_ERR",
    "FEC entry type is deprecated use _ROUTING instead \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_TUNNEL_ERR,
    "ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_TUNNEL_ERR",
    "FEC entry type is deprecated use _TUNNELING instead \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_NDX_OUT_OF_RANGE_ERR",
    "ecmp-index out range. Range: 0 - 1023. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_SIZE_OUT_OF_RANGE_ERR",
    "ecmp-size out range. Range: 1 - 511. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_RPF_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_RPF_NDX_OUT_OF_RANGE_ERR",
    "rpf_fec_index out range. has to be one of EMCP members. \n\r "
    "Range: fec-base - fec-base+size - 1. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_ECMP_LAG_HASH_MATCH_ERR,
    "ARAD_PP_FRWRD_FEC_ECMP_LAG_HASH_MATCH_ERR",
    "cannot use same hash function for LAG and ECMP  \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_FEC_SELECTED_POLYNOMIAL_IN_USE,
    "ARAD_PP_FRWRD_FEC_SELECTED_POLYNOMIAL_IN_USE",
    "The selected polynomial is already in use.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  



  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};







STATIC uint32
  arad_pp_frwrd_fec_dest_encode(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO               *fec_entry,
    SOC_SAND_OUT  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA    *dest_enc
  )
{
  SOC_PPC_FRWRD_DECISION_INFO
    fwd_decision;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_DEST_ENCODE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry);
  SOC_SAND_CHECK_NULL_INPUT(dest_enc);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);

  
  if (fec_entry->type == SOC_PPC_FEC_TYPE_DROP || fec_entry->type == SOC_PPC_NOF_FEC_TYPES_ARAD)
  {
    fwd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
  }
  
  if (fec_entry->type != SOC_PPC_FEC_TYPE_DROP && fec_entry->type != SOC_PPC_NOF_FEC_TYPES_ARAD)
  {
      res = soc_ppd_sand_dest_to_fwd_decision(unit,&(fec_entry->dest),&fwd_decision);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  
  switch(fec_entry->type)
  {
    case SOC_PPC_FEC_TYPE_TRILL_MC:    
      fwd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_TRILL;
      fwd_decision.additional_info.eei.val.trill_dest.dest_nick = fec_entry->app_info.dist_tree_nick;
      fwd_decision.additional_info.eei.val.trill_dest.is_multicast = (fec_entry->type == SOC_PPC_FEC_TYPE_TRILL_MC)?1:0;
    break;
    case SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF:
    case SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF:
      fwd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;
      fwd_decision.additional_info.eei.val.outlif = fec_entry->eep;
    break;
    case SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND:
      fwd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
      fwd_decision.additional_info.eei.val.mpls_command.label = SOC_PPD_MPLS_LABEL_FROM_EEI_COMMAND_ENCODING(fec_entry->eep);
      if (SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(fec_entry->eep) < SOC_PPC_EEI_IDENTIFIER_SWAP_VAL) {
          fwd_decision.additional_info.eei.val.mpls_command.push_profile = SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(fec_entry->eep);
          fwd_decision.additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH;
      } else {
          fwd_decision.additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_SWAP;
      }
    break;
    default:
      fwd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
    break;
  }


  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          &fwd_decision,
          &dest_enc->destination,
          &dest_enc->eei
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_dest_encode()", 0, 0);
}

STATIC uint32
  arad_pp_frwrd_fec_dest_decode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA      *dest_enc,
    SOC_SAND_OUT  SOC_PPC_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  SOC_PPC_FRWRD_DECISION_INFO
    fwd_decision;
  uint32
    asd=0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_DEST_DECODE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);

  if(dest_enc->format == ARAD_PP_IHB_FEC_FORMAT_C) {
      asd = dest_enc->eei;
  }

  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          dest_enc->destination,
          asd,
          ARAD_PP_FWD_DECISION_PARSE_EEI|ARAD_PP_FWD_DECISION_PARSE_DEST,
          &fwd_decision
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = soc_ppd_fwd_decision_to_sand_dest(unit,&fwd_decision, &(fec_entry->dest));
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  if (fwd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_TRAP)
  {
    fec_entry->dest.dest_type = SOC_SAND_PP_DEST_TRAP;
    ARAD_COPY(
      &(fec_entry->trap_info),
      &(fwd_decision.additional_info.trap_info.action_profile),
      SOC_PPC_ACTION_PROFILE,
      1
    );
  }
  else if (fwd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
      if (fwd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_TRILL) {
          fec_entry->app_info.dist_tree_nick = fwd_decision.additional_info.eei.val.trill_dest.dest_nick;
      }
  }
  if (fec_entry->dest.dest_type == SOC_SAND_PP_DEST_EXPLICIT_FLOW && fec_entry->dest.dest_val == ARAD_MAX_QUEUE_ID(unit))
  {
    fec_entry->dest.dest_type = SOC_SAND_PP_DEST_TYPE_DROP;
    fec_entry->dest.dest_val = 0;
  }

  if(fwd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS){
    int identifier = (fwd_decision.additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_SWAP) ?
            SOC_PPC_EEI_IDENTIFIER_SWAP_VAL  :  fwd_decision.additional_info.eei.val.mpls_command.push_profile;
    fec_entry->eep = SOC_PPC_EEI_ENCODING_MPLS_COMMAND(identifier, fwd_decision.additional_info.eei.val.mpls_command.label);
  }
  else if (fwd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
      fec_entry->eep = fwd_decision.additional_info.eei.val.outlif;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_dest_decode()", 0, 0);
}

STATIC uint32
  arad_pp_frwrd_fec_rpf_mode_to_hw_val_map(
    SOC_SAND_IN SOC_PPC_FRWRD_FEC_RPF_MODE  rpf_mode,
    SOC_SAND_IN   uint8   is_uc_rpf
  )
{
  if(is_uc_rpf) 
   {
      if(rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT)
      {
        return 0;
      }
      if(rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE)
      {
        return 1;
      }
      return 0;
  }
  else
  {
      if(rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT)
      {
        return 1;
      }
      if(rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP)
      {
        return 3;
      }
      return 0; 
  }
}



STATIC SOC_PPC_FRWRD_FEC_RPF_MODE
  arad_pp_frwrd_fec_rpf_mode_from_hw_val_map(
    SOC_SAND_IN   uint32    rpf_mode_val,
    SOC_SAND_IN   uint8   is_uc_rpf
  )
{
    if(is_uc_rpf)
    {
    switch(rpf_mode_val)
    {
      case 0:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT;
      case 1:
      default:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
    }
  }
  else
  {
    switch(rpf_mode_val)
    {
      case 0:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_NONE;
      case 1:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT;
      case 3:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP;
      case 2:
      default:
        return SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP;
    }
   }
}

STATIC uint32
  arad_pp_frwrd_fec_one_entry_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA    
    entry_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ONE_ENTRY_SET);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fec.fec_entry_type.set(
          unit,
          fec_ndx,
          fec_entry->type
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_pp_frwrd_fec_dest_encode(
          unit,
          fec_entry,
          &entry_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  switch (fec_entry->type) {
  case SOC_PPC_FEC_TYPE_DROP:
  case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
  case SOC_PPC_NOF_FEC_TYPES_ARAD:
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_NULL;
      break;
  case SOC_PPC_FEC_TYPE_IP_MC: 
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_B;
      entry_data.out_lif = fec_entry->rpf_info.expected_in_rif;
      break;
  case SOC_PPC_FEC_TYPE_IP_UC:  
  case SOC_PPC_FEC_TYPE_ROUTING:
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_A;
      entry_data.arp_ptr = fec_entry->eep;
      entry_data.out_lif = fec_entry->app_info.out_rif;
      break;
  case SOC_PPC_FEC_TYPE_TRILL_MC:  
      
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_C;
      break;
  case SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL:
  case SOC_PPC_FEC_TYPE_MPLS_LSR:
  case SOC_PPC_FEC_TYPE_TUNNELING:
  case SOC_PPC_FEC_TYPE_TRILL_UC:
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_B;
      entry_data.out_lif = fec_entry->eep;
      break;
  case SOC_PPC_FEC_TYPE_FORWARD:
#ifdef BCM_88660_A0
      if (SOC_DPP_CONFIG(unit)->pp.router_over_ac) {
          
          entry_data.format = ARAD_PP_IHB_FEC_FORMAT_C;
          entry_data.eei = (fec_entry->eep | 0xF00000);
      }
      else
#endif
      {
          entry_data.format = ARAD_PP_IHB_FEC_FORMAT_B;
          entry_data.out_lif = fec_entry->eep;
      }
      break;
  case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_B;
      entry_data.out_lif = fec_entry->app_info.out_ac_id;
      break;
  case SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF:
  case SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND:
  case SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF:
      
      entry_data.format = ARAD_PP_IHB_FEC_FORMAT_C;
  default:  
      break;
  }

  
  entry_data.uc_rpf_mode  = arad_pp_frwrd_fec_rpf_mode_to_hw_val_map(fec_entry->rpf_info.rpf_mode, TRUE);
  entry_data.mc_rpf_mode = arad_pp_frwrd_fec_rpf_mode_to_hw_val_map(fec_entry->rpf_info.rpf_mode, FALSE);

  res = arad_pp_ihb_fec_entry_general_tbl_set_unsafe(
          unit,
          fec_ndx,
          &entry_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_one_entry_set()", 0, 0);
}




STATIC uint32
  arad_pp_frwrd_fec_one_entry_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       fec_ndx,
    SOC_SAND_OUT  SOC_PPC_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA    
    entry_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ONE_ENTRY_GET);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fec.fec_entry_type.get(
          unit,
          fec_ndx,
          &fec_entry->type
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_pp_ihb_fec_entry_general_tbl_get_unsafe(
          unit,
          fec_ndx,
          &entry_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_frwrd_fec_dest_decode(
          unit,
          &entry_data,
          fec_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  switch (fec_entry->type) {
  case SOC_PPC_FEC_TYPE_DROP:
  case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
  case SOC_PPC_NOF_FEC_TYPES_ARAD:
      break;
  case SOC_PPC_FEC_TYPE_IP_MC: 
      fec_entry->rpf_info.expected_in_rif = entry_data.out_lif;
      break;
  case SOC_PPC_FEC_TYPE_IP_UC:  
  case SOC_PPC_FEC_TYPE_ROUTING:
      fec_entry->eep = entry_data.arp_ptr;
      fec_entry->app_info.out_rif = entry_data.out_lif;
      break;
  case SOC_PPC_FEC_TYPE_TRILL_MC:  
      
      break;
  case SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL:
  case SOC_PPC_FEC_TYPE_TUNNELING:
  case SOC_PPC_FEC_TYPE_TRILL_UC:
  case SOC_PPC_FEC_TYPE_MPLS_LSR: 
      fec_entry->eep = entry_data.out_lif;
      break;
  case SOC_PPC_FEC_TYPE_FORWARD:
#ifdef BCM_88660_A0
      if (SOC_DPP_CONFIG(unit)->pp.router_over_ac) {
          
          fec_entry->eep = entry_data.eei & 0xFFFFF;
      }
      else
#endif
      {
          fec_entry->eep = entry_data.out_lif;
      }
      break;
  case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
      fec_entry->app_info.out_ac_id = entry_data.out_lif;
      break;
  default:
      break;
  }

  
  fec_entry->rpf_info.rpf_mode  = arad_pp_frwrd_fec_rpf_mode_from_hw_val_map(entry_data.uc_rpf_mode, TRUE);
  fec_entry->rpf_info.rpf_mode |= arad_pp_frwrd_fec_rpf_mode_from_hw_val_map(entry_data.mc_rpf_mode, FALSE);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_one_entry_get()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_entry_uc_rpf_mode_set(
    int                              unit,
    uint32                           fec_ndx,
    uint32                           uc_rpf_mode
  )
{
uint32 
	rv,
	hw_uc_rpf_mode;

	SOCDNX_INIT_FUNC_DEFS;

	hw_uc_rpf_mode = arad_pp_frwrd_fec_rpf_mode_to_hw_val_map(uc_rpf_mode, TRUE);

	rv = arad_pp_ihb_fec_entry_general_tbl_uc_rpf_mode_set(unit, fec_ndx, hw_uc_rpf_mode);
	SOCDNX_IF_ERR_EXIT(rv);

exit:
	SOCDNX_FUNC_RETURN;
}

STATIC uint32
  arad_pp_frwrd_fec_size_get(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   uint32           ecmp_ndx,
    SOC_SAND_OUT  uint32          *ecmp_size
  )
{
  ARAD_PP_IHB_FEC_ECMP_TBL_DATA
    ecmp_tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_SIZE_GET);

  SOC_SAND_CHECK_NULL_INPUT(ecmp_size);

  if(ecmp_ndx >= SOC_DPP_DEFS_GET(unit, nof_ecmp)) {
      *ecmp_size = 1;
      goto exit;
  }

  
  res = arad_pp_ihb_fec_ecmp_tbl_get_unsafe(
          unit,
          ecmp_ndx,
          &ecmp_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(ecmp_tbl_data.group_size == 0) 
  {
    *ecmp_size = 1 ;
  }
  else
  {
    *ecmp_size = ecmp_tbl_data.group_size;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_size_get()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_fec_protect_type_get(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   uint32                        fec_ndx,
    SOC_SAND_OUT  SOC_PPC_FRWRD_FEC_PROTECT_TYPE  *fec_protect_type
  )
{
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res;
  uint8
	is_protected;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_PROTECT_TYPE_GET);

  SOC_SAND_CHECK_NULL_INPUT(fec_protect_type);

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.is_protected.bit_get(
		unit, 
		ARAD_PP_FEC_PROTECT_BITMAP_FEC_TO_IDX(fec_ndx), 
		&is_protected
		);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!is_protected)
  {
    *fec_protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE;
    goto exit;
  }

  
  res = arad_pp_ihb_fec_super_entry_tbl_get_unsafe(
          unit,
          fec_ndx,
          &ihb_fec_super_entry_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  

  if (ihb_fec_super_entry_tbl_data.protection_pointer == ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT)
  {
    *fec_protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE;
  }
  else if (ihb_fec_super_entry_tbl_data.protection_pointer == ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT)
  {
    *fec_protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY;
  }
  else
  {
    *fec_protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protect_type_get()", 0, 0);
}

uint32
arad_pp_frwrd_fec_is_protected_get(
    int                         unit,
	uint32                      fec_ndx,
	uint8						*is_protected
)
{
uint32 
	res;

	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_NULL_CHECK(is_protected);

	res = sw_state_access[unit].dpp.soc.arad.pp.fec.is_protected.bit_get(
		unit, 
		ARAD_PP_FEC_PROTECT_BITMAP_FEC_TO_IDX(fec_ndx), 
		is_protected
		);
	SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
	SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_frwrd_fec_facility_port_to_index(
    SOC_SAND_IN  int                                           unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  )
{

  if (sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
    return sys_port_ndx->sys_id + SOC_DPP_DEFS_GET(unit, nof_fecs);
  }
  else
  {
    return sys_port_ndx->sys_id;
  }
}

uint32
  arad_pp_frwrd_fec_hash_index_to_hw_val(
    SOC_SAND_IN  uint8                   hash_index
  )
{
  switch(hash_index)
  {
  case SOC_PPC_FEC_LB_CRC_10861:
  case SOC_PPC_FEC_LB_CRC_0x8003: 
    return 0;
  case SOC_PPC_FEC_LB_CRC_10285:
  case SOC_PPC_FEC_LB_CRC_0x8011: 
    return 1;
  case SOC_PPC_FEC_LB_CRC_101A1:
  case SOC_PPC_FEC_LB_CRC_0x8423: 
    return 2;
  case SOC_PPC_FEC_LB_CRC_12499:
  case SOC_PPC_FEC_LB_CRC_0x8101: 
    return 3;
  case SOC_PPC_FEC_LB_CRC_1F801:
  case SOC_PPC_FEC_LB_CRC_0x84a1: 
    return 4;
  case SOC_PPC_FEC_LB_CRC_172E1:
  case SOC_PPC_FEC_LB_CRC_0x9019: 
    return 5;
  case SOC_PPC_FEC_LB_CRC_1EB21:
    return 6;
  case SOC_PPC_FEC_LB_CRC_13965:
    return 7;
  case SOC_PPC_FEC_LB_CRC_1698D:
    return 8;
  case SOC_PPC_FEC_LB_CRC_1105D:
    return 9;
  case SOC_PPC_FEC_LB_KEY:
    return 10;
  case SOC_PPC_FEC_LB_ROUND_ROBIN:
    return 11;
  case SOC_PPC_FEC_LB_2_CLOCK:
    return 12;
  default:
      return 12;
  }
}

STATIC uint32
  arad_pp_frwrd_fec_hash_index_from_hw_val(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                   hash_index_val
  )
{
  switch(hash_index_val)
  {
  case 0:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x8003;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_10861;
        }
  case 1:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x8011;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_10285;
        }
  case 2:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x8423;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_101A1;
        }
  case 3:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x8101;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_12499;
        }
  case 4:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x84a1;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_1F801;
        }
  case 5:
#ifdef BCM_88660_A0
        
        if (SOC_IS_ARADPLUS(unit)) {
            return SOC_PPC_FEC_LB_CRC_0x9019;
        }
        else
#endif 
        {
            return SOC_PPC_FEC_LB_CRC_172E1;
        }
  case 6:
    return SOC_PPC_FEC_LB_CRC_1EB21;
  case 7:
    return SOC_PPC_FEC_LB_CRC_13965;
  case 8:
    return SOC_PPC_FEC_LB_CRC_1698D;
  case 9:
    return SOC_PPC_FEC_LB_CRC_1105D;
  case 10:
    return SOC_PPC_FEC_LB_KEY;
  case 11:
    return SOC_PPC_FEC_LB_ROUND_ROBIN;
  case 12:
    return SOC_PPC_FEC_LB_2_CLOCK;
  default:
      return SOC_PPC_FEC_LB_2_CLOCK;
  }

}


uint32
  arad_pp_frwrd_fec_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
#ifndef PLISIM
  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;
#endif
  SOC_PPC_FRWRD_FEC_GLBL_INFO
    *glbl_info = NULL;
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32 val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_PCID_LITE_SKIP(unit);


  ihb_fec_entry_accessed_tbl_data.fec_entry_accessed = 0;
  res = arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
          unit,
          MEM_BLOCK_ANY,
          0,
          &ihb_fec_entry_accessed_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifndef PLISIM
  ihb_destination_status_tbl_data.destination_valid[0] = 1;
  ihb_destination_status_tbl_data.destination_valid[1] = 1;
  ihb_destination_status_tbl_data.destination_valid[2] = 1;
  ihb_destination_status_tbl_data.destination_valid[3] = 1;
  ihb_destination_status_tbl_data.destination_valid[4] = 1;
  ihb_destination_status_tbl_data.destination_valid[5] = 1;
  ihb_destination_status_tbl_data.destination_valid[6] = 1;
  ihb_destination_status_tbl_data.destination_valid[7] = 1;
#endif

  ARAD_CLEAR(ihb_fec_super_entry_tbl_data.data0, uint32, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);
  val = 0x7ffff; 
  soc_mem_field_set(unit, ARAD_PP_FEC_ENTRY_FORMAT_BY_CHIP(unit, FEC_ENTRY_GENERAL), ihb_fec_super_entry_tbl_data.data0, DESTINATIONf, &val);

  if (SOC_IS_JERICHO(unit)) {
      ARAD_CLEAR(ihb_fec_super_entry_tbl_data.data1, uint32, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);
      soc_mem_field_set(unit, ARAD_PP_FEC_ENTRY_FORMAT_BY_CHIP(unit, FEC_ENTRY_GENERAL), ihb_fec_super_entry_tbl_data.data1, DESTINATIONf, &val);
  }

  ihb_fec_super_entry_tbl_data.protection_pointer = ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT;


#ifndef PLISIM
  if (!SAL_BOOT_PLISIM) {
      res = arad_pp_ihb_destination_status_tbl_fill_unsafe(
              unit,
              &ihb_destination_status_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          
          res = arad_pp_ihb_fec_super_entry_tbl_fill_unsafe(
                  unit,
                  &ihb_fec_super_entry_tbl_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
#endif

  if (SOC_IS_JERICHO(unit)) {
    
    
    
    res = soc_reg_field32_modify(unit, IHB_FER_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, ENABLE_FEC_SUPER_ENTRY_SPLIT_DBf, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    res = soc_reg_field32_modify(unit, IHB_FER_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, ENABLE_FEC_ENTRY_STAMP_NATIVE_VSI_AND_OUT_LIFf, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  }

  ARAD_ALLOC(glbl_info, SOC_PPC_FRWRD_FEC_GLBL_INFO, 1, "arad_pp_frwrd_fec_init_unsafe.glbl_info");
 
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, glbl_info);

  res = arad_pp_frwrd_fec_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  ARAD_FREE(glbl_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_init_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_SET_VERIFY);

  res = SOC_PPC_FRWRD_FEC_GLBL_INFO_verify(unit, glbl_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
    uint32
      res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_glbl_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, glbl_info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_glbl_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  )
{

  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res = SOC_SAND_OK,
	is_protected;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  is_protected = (protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE) ? 1 : 0;

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.is_protected.bit_range_write(
	  unit,
	  ARAD_PP_FEC_PROTECT_BITMAP_FEC_TO_IDX(fec_ndx),
	  0,
	  1,
	  &is_protected
	  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_frwrd_fec_one_entry_set(
          unit,
          fec_ndx,
          working_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  if (protect_fec != NULL && ARAD_PP_FRWRD_FEC_EVEN_FEC(fec_ndx) && is_protected)
  {
    res = arad_pp_frwrd_fec_one_entry_set(
            unit,
            ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx),
            protect_fec
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  
  
  if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
	  ihb_fec_super_entry_tbl_data.protection_pointer = ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT;
  }
  
  else if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH)
  {
	  ihb_fec_super_entry_tbl_data.protection_pointer = protect_info->oam_instance_id;
  }
  else
  {
	  ihb_fec_super_entry_tbl_data.protection_pointer = ARAD_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT;
  }
  res = arad_pp_ihb_fec_super_entry_tbl_set_unsafe(
            unit,
            fec_ndx,
            &ihb_fec_super_entry_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_add_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(protect_type, SOC_PPC_FRWRD_FEC_PROTECT_TYPE_MAX, SOC_PPC_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  res = SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(unit, working_fec);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (protect_fec != NULL && ARAD_PP_FRWRD_FEC_EVEN_FEC(fec_ndx) && protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    res = SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(unit, protect_fec);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH)
    {
      res = SOC_PPC_FRWRD_FEC_PROTECT_INFO_verify(unit, protect_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_add_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    indx,
    fec_tbl_indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(success);

 
  for (fec_tbl_indx=fec_ndx, indx=0; indx < nof_entries; ++indx)
  {
    res = arad_pp_frwrd_fec_one_entry_set(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_add_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries
  )
{
  uint32
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx+nof_entries, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 15, exit);

  for (indx=0; indx < nof_entries; ++indx)
  {
    res = SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(unit, &fec_array[indx]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100+indx, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_NOF(nof_entries, SOC_DPP_DEFS_GET(unit, nof_fecs), ARAD_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR, 30, exit);

  
  if (nof_entries > 1 && !ARAD_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,40,exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_add_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO              *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    fec_tbl_indx,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(fec_range);

 
  for (fec_tbl_indx=fec_ndx+fec_range->start, indx=0; indx < fec_range->end - fec_range->start + 1; ++indx)
  {
    res = arad_pp_frwrd_fec_one_entry_set(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_update_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    indx,
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  
  if (fec_range->end < fec_range->start)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,20,exit);
  }
  for (indx=0; indx < fec_range->end - fec_range->start; ++indx)
  {
    res = SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(unit, &fec_array[indx]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100+indx, exit);
  }
  
  res = arad_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (fec_range->end >= fec_size)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,40 , exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_update_verify()", fec_ndx, 0);
}

#ifdef BCM_88660_A0

static uint32 
  _arad_pp_frwrd_fec_set_ecmp_is_stateful(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                 ecmp_ndx,
    SOC_SAND_IN  uint8                          is_stateful
    )
{
  uint32 res = SOC_SAND_OK;
  uint32 reg_val = 0;
  uint32 entry_ndx = ecmp_ndx / ARAD_PP_FRWRD_FEC_ECMP_IS_STATEFUL_BITMAP_SIZE_IN_BITS;
  uint32 bitmap_bit_offset = ecmp_ndx % ARAD_PP_FRWRD_FEC_ECMP_IS_STATEFUL_BITMAP_SIZE_IN_BITS;
  uint32 bitmap_val;
  uint32 is_stateful_32 = is_stateful;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_UNSAFE);
  
  
  
  
  
  

  res = READ_IHB_FEC_ECMP_IS_STATEFULm(unit, MEM_BLOCK_ANY, entry_ndx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bitmap_val = soc_mem_field32_get(unit, IHB_FEC_ECMP_IS_STATEFULm, &reg_val, IS_STATEFULf);

  SHR_BITCOPY_RANGE(&bitmap_val, bitmap_bit_offset, &is_stateful_32, 0, 1);

  soc_mem_field32_set(unit, IHB_FEC_ECMP_IS_STATEFULm, &reg_val, IS_STATEFULf, bitmap_val);
  
  res = WRITE_IHB_FEC_ECMP_IS_STATEFULm(unit, MEM_BLOCK_ANY, entry_ndx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_fec_set_ecmp_is_stateful()", ecmp_ndx, 0);
}

static uint32 
  _arad_pp_frwrd_fec_get_ecmp_is_stateful(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                 ecmp_ndx,
    SOC_SAND_OUT uint8                          *is_stateful
    )
{
  uint32 res = SOC_SAND_OK;
  uint32 reg_val = 0;
  uint32 entry_ndx = ecmp_ndx / ARAD_PP_FRWRD_FEC_ECMP_IS_STATEFUL_BITMAP_SIZE_IN_BITS;
  uint32 bitmap_bit_offset = ecmp_ndx % ARAD_PP_FRWRD_FEC_ECMP_IS_STATEFUL_BITMAP_SIZE_IN_BITS;
  uint32 bitmap_val;
  uint32 is_stateful_32 = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_UNSAFE);

  
  
  
  
  

  res = READ_IHB_FEC_ECMP_IS_STATEFULm(unit, MEM_BLOCK_ANY, entry_ndx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bitmap_val = soc_mem_field32_get(unit, IHB_FEC_ECMP_IS_STATEFULm, &reg_val, IS_STATEFULf);
  SHR_BITCOPY_RANGE(&is_stateful_32, 0, &bitmap_val, bitmap_bit_offset, 1);

  *is_stateful = is_stateful_32 & 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_frwrd_fec_get_ecmp_is_stateful()", ecmp_ndx, 0);
}

#endif 


uint32
  arad_pp_frwrd_fec_ecmp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO               *ecmp_info
  )
{
   
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_FEC_ECMP_TBL_DATA
    ecmp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ecmp_info);

  ecmp_tbl_data.group_size = ecmp_info->size;
  ecmp_tbl_data.group_start_pointer = ecmp_info->base_fec_id;
  ecmp_tbl_data.is_protected = SOC_SAND_BOOL2NUM(ecmp_info->is_protected);
  ecmp_tbl_data.rpf_fec_pointer = ecmp_info->rpf_fec_index + ecmp_info->base_fec_id;
  res = arad_pp_ihb_fec_ecmp_tbl_set_unsafe(
          unit,
          ecmp_ndx,
          &ecmp_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    res = _arad_pp_frwrd_fec_set_ecmp_is_stateful(unit, ecmp_ndx, ecmp_info->is_stateful);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_info_set_unsafe()", ecmp_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                            ecmp_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO               *ecmp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(ecmp_ndx, SOC_DPP_DEFS_GET(unit, nof_ecmp), ARAD_PP_FRWRD_FEC_ECMP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_FEC_ECMP_INFO_verify(unit, ecmp_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_info_set_verify()", ecmp_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              ecmp_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(ecmp_ndx, SOC_DPP_DEFS_GET(unit, nof_ecmp), ARAD_PP_FRWRD_FEC_ECMP_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_info_get_verify()", ecmp_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                         ecmp_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO            *ecmp_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_FEC_ECMP_TBL_DATA
    ecmp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ECMP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ecmp_info);

  
  
  res = arad_pp_ihb_fec_ecmp_tbl_get_unsafe(
          unit,
          ecmp_ndx,
          &ecmp_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(ecmp_tbl_data.group_size == 0) 
  {
    ecmp_info->size = 1 ;
  }
  else
  {
    ecmp_info->size = ecmp_tbl_data.group_size;
  }
  ecmp_info->base_fec_id = ecmp_tbl_data.group_start_pointer;
  ecmp_info->is_protected = SOC_SAND_NUM2BOOL(ecmp_tbl_data.is_protected);
  ecmp_info->rpf_fec_index = (uint32)((ecmp_tbl_data.rpf_fec_pointer > ecmp_info->base_fec_id)?
                                   (ecmp_tbl_data.rpf_fec_pointer - ecmp_info->base_fec_id) :0);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    res = _arad_pp_frwrd_fec_get_ecmp_is_stateful(unit, ecmp_ndx, &ecmp_info->is_stateful);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_info_get_unsafe()", ecmp_ndx, 0);
} 



uint32
  arad_pp_frwrd_fec_entry_use_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  )
{
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  uint32
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry_info);

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(fec_entry_info);


  
  res = arad_pp_frwrd_fec_protect_type_get(
          unit,
          fec_ndx,
          &protect_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  
  res = arad_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (fec_size > 1)
  {
    fec_entry_info->nof_entries = fec_size;
    fec_entry_info->type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP;
    if(protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE){
        protect_type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP_PROTECTED;
    }
    goto exit;
  }

  if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    fec_entry_info->type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC;
    fec_entry_info->nof_entries = 1;
  }
  else if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY)
  {
    fec_entry_info->type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT;
    fec_entry_info->nof_entries = 2;
  }
  else if (protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH)
  {
    fec_entry_info->type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT;
    fec_entry_info->nof_entries = 2;
  }
  else
  {
    fec_entry_info->type = SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_NONE;
    fec_entry_info->nof_entries = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_use_info_get_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_entry_use_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_use_info_get_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(protect_type);
  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(working_fec);
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(protect_fec);
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(protect_info);


  
  res = arad_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 res = arad_pp_frwrd_fec_protect_type_get(
          unit,
          fec_ndx,
          protect_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  if (*protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH)
  {
    res = arad_pp_ihb_fec_super_entry_tbl_get_unsafe(
            unit,
            fec_ndx,
            &ihb_fec_super_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    protect_info->oam_instance_id = ihb_fec_super_entry_tbl_data.protection_pointer;
  }
 
  res = arad_pp_frwrd_fec_one_entry_get(
          unit,
          fec_ndx,
          working_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
 
  if (*protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    res = arad_pp_frwrd_fec_one_entry_get(
            unit,
            ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx),
            protect_fec
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_get_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_get_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    fec_size,
    fec_tbl_indx,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(fec_array);

  res = arad_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
  for (fec_tbl_indx=fec_ndx, indx=0; indx < fec_size; ++indx)
  {
    SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&(fec_array[indx]));

    res = arad_pp_frwrd_fec_one_entry_get(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = ARAD_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

  *nof_entries = fec_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_get_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_get_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  )
{
  SOC_PPC_FRWRD_FEC_ENTRY_INFO
    fec_entry;
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO
    fec_entry_use;
  SOC_PPC_FRWRD_FEC_PROTECT_INFO
    protect_info;
  uint8
    success;
  uint32
    indx,
    tbl_indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_REMOVE_UNSAFE);
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&fec_entry);
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);
  
  fec_entry.type = SOC_PPC_FEC_TYPE_DROP;

  res = arad_pp_frwrd_fec_entry_use_info_get_unsafe(
          unit,
          fec_ndx,
          &fec_entry_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (fec_entry_use.nof_entries == 0)
  {
    goto exit;
  }

 
  res = arad_pp_frwrd_fec_entry_add_unsafe(
          unit,
          fec_ndx,
          SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE,
          &fec_entry,
          &fec_entry,
          &protect_info,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.fec.fec_entry_type.set(
          unit,
          fec_ndx,
          SOC_PPC_NOF_FEC_TYPES_ARAD
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

  if (fec_entry_use.nof_entries == 1)
  {
    goto exit;
  }

  
  fec_entry.type = SOC_PPC_NOF_FEC_TYPES_ARAD;
  tbl_indx = fec_ndx;
  for (indx = 0 ; indx<fec_entry_use.nof_entries; ++indx)
  {
    res = arad_pp_frwrd_fec_one_entry_set(
            unit,
            tbl_indx,
            &fec_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
    if (!ARAD_PP_FRWRD_FEC_EVEN_FEC(tbl_indx) &&
        (fec_entry_use.type == SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT)) {
        break;
    }
    tbl_indx = ARAD_PP_FRWRD_FEC_NEXT_ENTRY(tbl_indx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_remove_unsafe()", fec_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_remove_verify()", fec_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fec_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  ARAD_PP_IHB_PATH_SELECT_TBL_DATA
    ihb_path_select_tbl_data;
  uint32
    oam_instance_ndx = fec_instance_ndx,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE);

  res = arad_pp_ihb_path_select_tbl_get_unsafe(
          unit,
          ARAD_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihb_path_select_tbl_data.path_select[oam_instance_ndx%8] = SOC_SAND_BOOL2NUM_INVERSE(up);

  res = arad_pp_ihb_path_select_tbl_set_unsafe(
          unit,
          ARAD_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_oam_instance_status_set_unsafe()", oam_instance_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(oam_instance_ndx, SOC_DPP_DEFS_GET(unit, nof_failover_fec_ids), ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(up, ARAD_PP_FRWRD_FEC_UP_MAX, ARAD_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_oam_instance_status_set_verify()", oam_instance_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(oam_instance_ndx, ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MAX, ARAD_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_oam_instance_status_get_verify()", oam_instance_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fec_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  ARAD_PP_IHB_PATH_SELECT_TBL_DATA
    ihb_path_select_tbl_data;
  uint32
    oam_instance_ndx = fec_instance_ndx,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(up);

  res = arad_pp_ihb_path_select_tbl_get_unsafe(
          unit,
          ARAD_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *up = SOC_SAND_NUM2BOOL_INVERSE(ihb_path_select_tbl_data.path_select[oam_instance_ndx%8]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_oam_instance_status_get_unsafe()", oam_instance_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;
  uint32
    dest_index,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);

  dest_index = arad_pp_frwrd_fec_facility_port_to_index(unit, sys_port_ndx);

  res = arad_pp_ihb_destination_status_tbl_get_unsafe(
          unit,
          ARAD_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_BOOL2NUM(up);
  ihb_destination_status_tbl_data.destination_valid[dest_index % 8] = tmp;
         
  res = arad_pp_ihb_destination_status_tbl_set_unsafe(
          unit,
          ARAD_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_sys_port_status_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_protection_sys_port_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY);

  
  if (sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT
      && sys_port_ndx->sys_id > SOC_DPP_DEFS_GET(unit, nof_fecs) - 1
      && up
     )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR, 10,exit);
  }
  
  switch(sys_port_ndx->sys_port_type)
  {
    case SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_SYS_PORT_TYPE_LAG:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,ARAD_MAX_LAG_GROUP_ID,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_sys_port_status_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_protection_sys_port_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  )
{
  int max_nof_lags = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY);

  
  switch(sys_port_ndx->sys_port_type)
  {
    case SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_SYS_PORT_TYPE_LAG:
      SOC_SAND_CHECK_FUNC_RESULT(soc_dpp_trunk_sw_db_max_nof_trunks_get(unit, &max_nof_lags),17,exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,max_nof_lags - 1,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_sys_port_status_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;
  uint32
    dest_index,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(up);


  dest_index = arad_pp_frwrd_fec_facility_port_to_index(unit, sys_port_ndx);

  res = arad_pp_ihb_destination_status_tbl_get_unsafe(
          unit,
          ARAD_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = ihb_destination_status_tbl_data.destination_valid[dest_index % 8];
  *up = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_protection_sys_port_status_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    entry_indx,
    valid_entries=0;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE
    fec_protect_type;
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  SOC_PPC_FEC_TYPE
    fec_type;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA
    dest_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  if (SOC_SAND_TBL_ITER_IS_END(&block_range->iter))
  {
    *nof_entries = 0;
    goto exit;
  }
  if (block_range->iter > SOC_DPP_DEFS_GET(unit, nof_fecs) - 1)
  {
    SOC_SAND_TBL_ITER_SET_END(&block_range->iter);
    *nof_entries = 0;
    goto exit;
  }

  for (entry_indx = block_range->iter; ((entry_indx - block_range->iter < block_range->entries_to_scan) && (entry_indx <= SOC_DPP_DEFS_GET(unit, nof_fecs) - 1)); entry_indx = ARAD_PP_FRWRD_FEC_NEXT_ENTRY(entry_indx))
  {
    
    res = sw_state_access[unit].dpp.soc.arad.pp.fec.fec_entry_type.get(
            unit,
            entry_indx,
            &fec_type
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    if (fec_type == SOC_PPC_NOF_FEC_TYPES_ARAD)
    {
      continue;
    }
    
    if (rule->type == SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE)
    {
      if (fec_type != (SOC_PPC_FEC_TYPE)rule->value)
      {
        continue;
      }
    }

    if (rule->type == SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED)
    {
      res = arad_pp_frwrd_fec_protect_type_get(
              unit,
              entry_indx,
              &fec_protect_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if ((fec_protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH && rule->type != SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED) || 
          (fec_protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY && rule->type != SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED)
         )
      {
        continue;
      }
      
      
      if (rule->type == SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED)
      {
        res = arad_pp_ihb_fec_super_entry_tbl_get_unsafe(
                unit,
                entry_indx,
                &ihb_fec_super_entry_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        
        if (ihb_fec_super_entry_tbl_data.protection_pointer != rule->value)
        {
          continue;
        }
      }
      
      if (rule->type == SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED)
      {
        res = arad_pp_ihb_fec_entry_general_tbl_get_unsafe(
                unit,
                entry_indx,
                &dest_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        
        if (dest_tbl_data.destination != rule->value)
        {
          continue;
        }
      }
    }
    if (rule->type == SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED)
    {
      res = arad_pp_frwrd_fec_entry_accessed_info_get_unsafe(
              unit,
              entry_indx,
              FALSE,
              &accessed_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (accessed_info.accessed == FALSE)
      {
        continue;
      }
    }

    
    fec_array[valid_entries] = entry_indx;
    ++valid_entries;
    if (valid_entries >= block_range->entries_to_act)
    {
      break;
    }
  }

  *nof_entries = valid_entries;
  block_range->iter = entry_indx+1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_GET_BLOCK_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_FEC_MATCH_RULE, rule, 10, exit);
  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_get_block_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_entry_accessed_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
   
  uint32
    tmp,
    reg_arr[2];
  uint64
    reg_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  
  
  if (accessed_info->trap_if_accessed)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(fec_id_ndx, 
                             ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX, 
                             ARAD_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX,
                             ARAD_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,11,exit);
  }

  res = READ_IHB_TRAP_IF_ACCESSEDr(unit, REG_PORT_ANY,&reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = SOC_SAND_BOOL2NUM(accessed_info->trap_if_accessed);

  reg_arr[1] = COMPILER_64_HI(reg_val);
  reg_arr[0] = COMPILER_64_LO(reg_val);

  SHR_BITCOPY_RANGE(reg_arr,fec_id_ndx-ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX, &tmp, 0, 1);

  COMPILER_64_SET(reg_val, reg_arr[1], reg_arr[0]);
  
  res = WRITE_IHB_TRAP_IF_ACCESSEDr(unit, REG_PORT_ANY,reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_accessed_info_set_unsafe()", fec_id_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_entry_accessed_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_id_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO, accessed_info, 20, exit);

  
  if (accessed_info->trap_if_accessed)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(fec_id_ndx, 
                             ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX, 
                             ARAD_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX,
                             ARAD_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,30,exit);
  }

  if (accessed_info->accessed != 0 )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,40,exit)
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_accessed_info_set_verify()", fec_id_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_entry_accessed_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(fec_id_ndx, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_accessed_info_get_verify()", fec_id_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_FEC_ID                          fec_id_ndx,
    SOC_SAND_IN  uint8                                   clear_access_stat,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO   *accessed_info
  )
{
  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  uint32
    tmp,
    reg_arr[2];
  uint64
    reg_val;
  uint32
    res          = SOC_SAND_OK,
    bit_position = ARAD_PP_IHB_FEC_ENTRY_ACCESSED_GET_BIT_POSITION(fec_id_ndx),
    entry_offset = ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id_ndx);
  int
    core_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(accessed_info);

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
      res = arad_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
              unit,
              core_id,
              entry_offset,
              &ihb_fec_entry_accessed_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (SOC_SAND_GET_BIT(ihb_fec_entry_accessed_tbl_data.fec_entry_accessed, bit_position)) {

          accessed_info->accessed = TRUE;

          
          if (clear_access_stat) {
              tmp = 0;
              res = soc_sand_set_field(
                      &(ihb_fec_entry_accessed_tbl_data.fec_entry_accessed),
                      bit_position,
                      bit_position,
                      tmp );
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

              res = arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
                      unit,
                      core_id,
                      entry_offset,
                      &ihb_fec_entry_accessed_tbl_data );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

          }
      }
  }

  
  
  if (fec_id_ndx < ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX)
  {
    accessed_info->trap_if_accessed = FALSE;
    goto exit;
  }

  res = READ_IHB_TRAP_IF_ACCESSEDr(unit, REG_PORT_ANY,&reg_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  reg_arr[1] = COMPILER_64_HI(reg_val);
  reg_arr[0] = COMPILER_64_LO(reg_val);

  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reg_arr,fec_id_ndx-ARAD_PP_FRWRD_FEC_TRAP_ABLE_MIN_INDEX, 1);
  accessed_info->trap_if_accessed = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_entry_accessed_info_get_unsafe()", fec_id_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO    *glbl_hash_info
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;
  uint64
    reg_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  COMPILER_64_ZERO(reg_value);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_ECMP_LB_KEY_CFGr(unit, REG_PORT_ANY, &reg_value));

  
#ifdef BCM_88675_A0
  if (SOC_IS_JERICHO(unit)) {
      if (glbl_hash_info->hash_func_id == SOC_PPC_FEC_LB_KEY) {
          soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value, ECMP_LB_KEY_CONSISTENT_HASHING_DATA_0_SHIFTf, 0);
      } else {
          soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value, ECMP_LB_KEY_CONSISTENT_HASHING_DATA_0_SHIFTf, ARAD_PP_SLB_ECMP_HASH_KEY_0_OFFSET_DEFAULT);
      }
  }
#endif 
  tmp = arad_pp_frwrd_fec_hash_index_to_hw_val(glbl_hash_info->hash_func_id);

  res = arad_pp_frwrd_fec_unique_polynomial_check(unit, tmp, ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

  soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_LB_HASH_INDEXf, tmp);

  tmp = glbl_hash_info->key_shift;
  soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_LB_KEY_SHIFTf, tmp);
#ifdef BCM_88680
  if (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) {

      tmp = arad_pp_frwrd_fec_hash_index_to_hw_val(glbl_hash_info->hash_func_id_second_hier);

      res = arad_pp_frwrd_fec_unique_polynomial_check(unit, tmp, ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP_SECOND_HIER);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_2ND_HIER_LB_HASH_INDEXf, tmp);

      tmp = glbl_hash_info->key_shift_second_hier;
      soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_2ND_HIER_LB_KEY_SHIFTf, tmp);
  }
#endif 
  tmp = glbl_hash_info->seed;
  soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_LB_KEY_SEEDf, tmp);

  tmp = SOC_SAND_BOOL2NUM(glbl_hash_info->use_port_id);
  soc_reg64_field32_set(unit, IHB_ECMP_LB_KEY_CFGr, &reg_value,    ECMP_LB_KEY_USE_IN_PORTf, tmp);

  res = WRITE_IHB_ECMP_LB_KEY_CFGr(unit, REG_PORT_ANY, reg_value);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO, glbl_hash_info, 10, exit);
  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_global_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_global_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO  *glbl_hash_info
  )
{
  uint32
    tmp,
    res;
  uint64 
    reg_value64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  COMPILER_64_ZERO(reg_value64);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(glbl_hash_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_ECMP_LB_KEY_CFGr(unit, REG_PORT_ANY, &reg_value64));
  tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_LB_HASH_INDEXf);
  glbl_hash_info->hash_func_id = arad_pp_frwrd_fec_hash_index_from_hw_val(unit, tmp);
  
  tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_LB_KEY_SHIFTf);
  glbl_hash_info->key_shift = tmp;
#ifdef BCM_88680
  if (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) {
      tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_2ND_HIER_LB_HASH_INDEXf);
      glbl_hash_info->hash_func_id_second_hier = arad_pp_frwrd_fec_hash_index_from_hw_val(unit, tmp);

      tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_2ND_HIER_LB_KEY_SHIFTf);
      glbl_hash_info->key_shift_second_hier = tmp;
  }
#endif
  tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_LB_KEY_SEEDf);
  glbl_hash_info->seed = tmp;
  
  tmp = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, reg_value64,   ECMP_LB_KEY_USE_IN_PORTf);
  glbl_hash_info->use_port_id = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    ihb_pinfo_fer_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  ihb_pinfo_fer_tbl_data.ecmp_lb_key_count =
            (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) ? \
            port_hash_info->nof_headers : (port_hash_info->nof_headers-1);

  res = arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
          unit,
          core_id,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe()", port_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO, port_hash_info, 20, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(port_hash_info->nof_headers, ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MIN, ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MAX, ARAD_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_port_info_set_verify()", port_ndx, 0);
}

uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_port_info_get_verify()", port_ndx, 0);
}


uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  ARAD_PP_IHB_PINFO_FER_TBL_DATA
    ihb_pinfo_fer_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(port_hash_info);

  res = arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  
  port_hash_info->nof_headers =
        (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) ? \
                        (uint8)(ihb_pinfo_fer_tbl_data.ecmp_lb_key_count) :\
                        (uint8)(ihb_pinfo_fer_tbl_data.ecmp_lb_key_count+1);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe()", port_ndx, 0);
}



uint32
  arad_pp_frwrd_fec_unique_polynomial_check(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int                                         new_hw_val,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_HASH_INDEX_TYPE           hash_index_type
  )
{
  uint64
      ecmp_reg_value,
      lag_reg_value,
      flex_reg_value;
  uint32
    res = SOC_SAND_OK,
    hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_NOF];
  int
    max_index = ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG,
    index= 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   COMPILER_64_ZERO(ecmp_reg_value);
   COMPILER_64_ZERO(lag_reg_value);


   SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_ECMP_LB_KEY_CFGr(unit, REG_PORT_ANY, &ecmp_reg_value));
   hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP] = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, ecmp_reg_value, ECMP_LB_HASH_INDEXf);

   SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHB_LAG_LB_KEY_CFGr(unit, SOC_CORE_ALL, &lag_reg_value));
   hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG] = soc_reg64_field32_get(unit, IHB_LAG_LB_KEY_CFGr, lag_reg_value, LAG_LB_HASH_INDEXf);

   if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
       COMPILER_64_ZERO(flex_reg_value);
       max_index = ARAD_PP_FRWRD_FEC_HASH_INDEX_FLEXIBLE;
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_CONSISTENT_HASHING_LB_KEY_CFGr(unit, SOC_CORE_ALL, &flex_reg_value));
       hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_FLEXIBLE] = soc_reg64_field32_get(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, flex_reg_value, CONSISTENT_HASHING_LAG_LB_HASH_INDEXf);

       if(SOC_IS_JERICHO(unit)) {
           max_index = ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG_1;
           hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_LAG_1] = soc_reg64_field32_get(unit, IHB_LAG_LB_KEY_CFGr, lag_reg_value, LAG_LB_HASH_INDEX_1f);

           if (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) {
               max_index = ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP_SECOND_HIER;
               hw_values[ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP_SECOND_HIER] = soc_reg64_field32_get(unit, IHB_ECMP_LB_KEY_CFGr, ecmp_reg_value, ECMP_2ND_HIER_LB_HASH_INDEXf);
           }
       }
   }

   for(index = ARAD_PP_FRWRD_FEC_HASH_INDEX_ECMP; index <= max_index; index++) {
       if(index == (int)hash_index_type) {
           continue;
       }

       if (hw_values[index] == new_hw_val) {
           SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_SELECTED_POLYNOMIAL_IN_USE, 40, exit);
       }
   }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("Error in arad_pp_frwrd_fec_unique_polynomial_check",hash_index_type,index);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_fec_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_fec;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_fec_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_fec;
}
uint32
  SOC_PPC_FRWRD_FEC_GLBL_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_DPP_DEFS_GET(unit, ecmp_max_size); ++ind)
  {
      if(info->ecmp_sizes[ind] != ind) {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR, 10,exit);
      }
  }
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->ecmp_sizes_nof_entries, SOC_DPP_DEFS_GET(unit, ecmp_max_size), SOC_DPP_DEFS_GET(unit, ecmp_max_size), ARAD_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  
  if(SOC_SAND_NUM2BOOL((info->rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT)) && SOC_SAND_NUM2BOOL((info->rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR, 30,exit);
  }

  
  if(info->rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT && info->rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP) 
  {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR, 30,exit);
  }

  
  if(info->rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP) 
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR, 30,exit);
  }

  SOC_SAND_ERR_IF_ABOVE_NOF(info->expected_in_rif, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info,
    SOC_SAND_IN  SOC_PPC_FEC_TYPE                 entry_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (entry_type == SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC)
  {
    ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, info->out_ac_id, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (entry_type == SOC_PPC_FEC_TYPE_IP_UC || entry_type == SOC_PPC_FEC_TYPE_ROUTING)
  {
    SOC_SAND_ERR_IF_ABOVE_NOF(info->out_rif, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 11, exit);
  }

  if (entry_type == SOC_PPC_FEC_TYPE_TRILL_MC)
  {
   SOC_SAND_ERR_IF_ABOVE_MAX(info->dist_tree_nick, ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_MAX, ARAD_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  int max_nof_lags = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_FEC_TYPE_MAX, ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  

  if(info->type == SOC_PPC_FEC_TYPE_IP_UC) 
  {
    
    
  }

  if(info->type == SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL || info->type == SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_ENTRY_TYPE_DEPRECATED_TUNNEL_ERR, 20,exit);
  }

  if (info->type == SOC_PPC_FEC_TYPE_IP_UC || info->type == SOC_PPC_FEC_TYPE_ROUTING)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->eep, ARAD_PP_FRWRD_FEC_EEP_MIN, SOC_DPP_CONFIG(unit)->pp.nof_global_out_lifs - 1, ARAD_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR, 12, exit);
  }
  else if ((info->type == SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL) ||
           (info->type == SOC_PPC_FEC_TYPE_MPLS_LSR) || (info->type == SOC_PPC_FEC_TYPE_TUNNELING) ||
           (info->type == SOC_PPC_FEC_TYPE_TRILL_UC) || (info->type == SOC_PPC_FEC_TYPE_FORWARD)) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->eep, SOC_DPP_CONFIG(unit)->pp.nof_global_out_lifs - 1, ARAD_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_verify(unit, &(info->app_info),info->type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  
  if (SOC_DPP_CONFIG(unit)->l3.nof_rifs) {
      res = SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_verify(unit, &(info->rpf_info));
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  }
  
  switch(info->dest.dest_type)
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,0,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,15,exit);
  break;
    case SOC_SAND_PP_DEST_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,
        (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)?SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID:SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID_ARAD),
        ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_DEST_MULTICAST:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,ARAD_MAX_MC_ID(unit),ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,17,exit);
    break;
    case SOC_SAND_PP_DEST_LAG:
      SOC_SAND_CHECK_FUNC_RESULT(soc_dpp_trunk_sw_db_max_nof_trunks_get(unit, &max_nof_lags),23,exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,max_nof_lags - 1,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_DPP_NOF_FLOWS_ARAD-1,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,20,exit);
    break;
    case SOC_SAND_PP_DEST_TRAP:
      SOC_PPC_ACTION_PROFILE_verify(&(info->trap_info));
    break;
    case SOC_SAND_PP_DEST_TYPE_ROUTER:
    case SOC_SAND_PP_DEST_FEC:
        SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_DPP_DEFS_GET(unit,nof_fecs)-1,ARAD_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,20,exit);
    break; 
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
    break;
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ECMP_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_NOF(info->base_fec_id, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->size, ARAD_PP_FRWRD_FEC_ECMP_MIN_SIZE, SOC_DPP_DEFS_GET(unit, ecmp_max_size) - 1, ARAD_PP_FRWRD_FEC_ECMP_SIZE_OUT_OF_RANGE_ERR,11,exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->base_fec_id+info->size-1, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 20, exit);
  if(info->rpf_fec_index  > info->base_fec_id + info->size - 1) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_FEC_ECMP_RPF_NDX_OUT_OF_RANGE_ERR, 35,exit);
  }
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ECMP_INFO_verify()",0,0);
}


uint32
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->oam_instance_id, ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_MAX, ARAD_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_PROTECT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_FEC_TYPE_MAX, ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->nof_entries, SOC_DPP_DEFS_GET(unit, nof_fecs), ARAD_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_FEC_TYPE_MAX, ARAD_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value, ARAD_PP_FRWRD_FEC_VALUE_MAX, ARAD_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_MATCH_RULE_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_SAND_ERR_IF_ABOVE_MAX(arad_pp_frwrd_fec_hash_index_to_hw_val(info->hash_func_id), ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX, ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_shift, ARAD_PP_FRWRD_FEC_KEY_SHIFT_MAX, ARAD_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR, 13, exit);
#ifdef BCM_88680
  SOC_SAND_ERR_IF_ABOVE_MAX(arad_pp_frwrd_fec_hash_index_to_hw_val(info->hash_func_id_second_hier), ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX, ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_shift_second_hier, ARAD_PP_FRWRD_FEC_KEY_SHIFT_MAX, ARAD_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR, 15, exit);
#endif 
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


