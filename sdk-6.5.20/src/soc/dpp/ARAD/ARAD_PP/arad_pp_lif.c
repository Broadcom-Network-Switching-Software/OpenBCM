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

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF
#include <shared/bsl.h>
#include <soc/mem.h>



#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/mbcm_pp.h>

#ifdef BCM_88660_A0
#endif
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>





#define ARAD_PP_LIF_KEY_MAPPING_MAX                              (SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_ARAD-1)
#define ARAD_PP_LIF_HANDLE_TYPE_MAX                              (SOC_PPC_NOF_L2_LIF_L2CP_HANDLE_TYPES-1)
#define ARAD_PP_LIF_DEFAULT_FRWD_TYPE_MAX                        (SOC_PPC_NOF_L2_LIF_DFLT_FRWRD_SRCS-1)
#define ARAD_PP_LIF_ORIENTATION_MAX                              (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define ARAD_PP_LIF_NETWORK_GROUP_MAX                            (3)
#define ARAD_PP_LIF_TPID_PROFILE_INDEX_MAX                       (3)
#define ARAD_PP_LIF_COS_PROFILE_MAX                              (63)
#define ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_MAX                      (3)
#define ARAD_PP_LIF_PWE_TERM_PROFILE_MAX                         (7)
#define ARAD_PP_LIF_PWE_ACTION_PROFILE_MAX                       (7)
#define ARAD_PP_LIF_PROTECTION_PASS_VAL_MAX                      (1)
#define ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_MAX                    (3)
#define ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_MAX                (1)
#define ARAD_PP_LIF_PWE_SERVICE_TYPE_MAX                         (SOC_PPC_NOF_L2_LIF_PWE_SERVICE_TYPES-1)
#define ARAD_PP_LIF_RAW_KEY_MAX                                  (SOC_SAND_U32_MAX)
#define ARAD_PP_LIF_LEARN_TYPE_MAX                               (SOC_PPC_NOF_L2_LIF_AC_LEARN_TYPES-1)
#define ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_MAX                    (7)
#define ARAD_PP_LIF_EDIT_PCP_PROFILE_MAX                         (15)
#define ARAD_PP_LIF_L2CP_PROFILE_MAX                             (1)
#define ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_MAX                       (63)
#define ARAD_PP_LIF_OPCODE_ID_MAX                                (3)
#define ARAD_PP_LIF_NOF_LIF_ENTRIES_MAX                          (SOC_SAND_UINT_MAX)
#define ARAD_PP_LIF_ISID_DOMAIN_MAX                              (SOC_SAND_U32_MAX)
#define ARAD_PP_LIF_AC_SERVICE_TYPE_MAX                          (SOC_PPC_NOF_L2_LIF_AC_SERVICE_TYPES-1)
#define ARAD_PP_LIF_ISID_SERVICE_TYPE_MAX                        (SOC_PPC_NOF_L2_LIF_ISID_SERVICE_TYPES-1)

#define ARAD_PP_LIF_GRE_VPN_MAX                                  ((1 << 24)-1)
#define ARAD_PP_LIF_VXLAN_VPN_MAX                                  ((1 << 24)-1)

#define ARAD_PP_LIF_WIDE_DATA_WIDTH                              (57)
#define ARAD_PP_LIF_WIDE_DATA_FIRST_HALF_LSB                     (0)
#define ARAD_PP_LIF_WIDE_DATA_SECOND_HALF_LSB                     (ARAD_PP_LIF_WIDE_DATA_WIDTH)
#define SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB           (0)
#define SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB           (5)
#define SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT         (SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB)
#define SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK          (SOC_SAND_BITS_MASK(SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB, SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB))

#define SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB                 (6)
#define SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_MSB                 (6)
#define SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_SHIFT               (SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB)
#define SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_MASK                (SOC_SAND_BITS_MASK(SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_MSB, SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB))


#define ARAD_PP_LIF_DA_TYPE_MAX                                  (SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1)

#define ARAD_PP_LIF_ADD_VSI_MAX                                  (1)
#define ARAD_PP_LIF_OFFSET_MAX                                   (0x7FFFF)


#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_LSB     (0)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_MSB     (0)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_SHIFT   (SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_LSB)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_MASK    (SOC_SAND_BITS_MASK(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_MSB, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_LSB))

#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_LSB         (1)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_MSB         (2)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_SHIFT       (SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_LSB)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_MASK        (SOC_SAND_BITS_MASK(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_MSB, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_LSB))

#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_LSB             (3)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_MSB             (4)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_SHIFT           (SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_LSB)
#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_MASK            (SOC_SAND_BITS_MASK(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_MSB, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_LSB))


#define ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN           (0x2)





#define SOC_PPC_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_profile, da_mac_address_lsb)  \
          SOC_SAND_SET_FLD_IN_PLACE(da_mac_address_lsb, SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT, SOC_PPC_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(l2cp_profile, SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_SHIFT, SOC_PPC_L2_LIF_L2CP_KEY_L2CP_PROFILE_MASK)

#define ARAD_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(first_vid,last_vid) \
          ((SOC_SAND_GET_BITS_RANGE(first_vid,11,0)<<16) + SOC_SAND_GET_BITS_RANGE(last_vid,11,0))

#define SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(port_da_not_found_profile, da_type, lif_default_forward_profile)  \
          SOC_SAND_SET_FLD_IN_PLACE(port_da_not_found_profile, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_SHIFT, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_PORT_DA_PROFILE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(da_type, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_SHIFT, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_DA_TYPE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(lif_default_forward_profile, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_SHIFT, SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_LIF_PROFILE_MASK)










CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_lif[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_PWE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_L2_LIF_AC_MP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_WITH_COS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_AC_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_ISID_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_GRE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_VXLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_GET_ERRS_PTR),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_lif[] =
{
  
  {
    ARAD_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR",
    "The parameter 'key_mapping' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_KEY_MAPPING_ENTRY_EXISTS_ERR,
    "ARAD_PP_LIF_KEY_MAPPING_ENTRY_EXISTS_ERR",
    "This entry already exists. \n\r "
    "To replace the entry, remove it first.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_KEY_MAPPING_NOT_SIMPLE_VLAN_TRANSLATION_ERR,
    "ARAD_PP_LIF_KEY_MAPPING_NOT_SIMPLE_VLAN_TRANSLATION_ERR",
    "Simple vlan translation is not enbaled on this device. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'handle_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_L2CP_HANDLE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'default_frwd_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_DFLT_FRWRD_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'service_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_PWE_SERVICE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR",
    "The parameter 'vid_tag' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR,
    "ARAD_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR",
    "The parameter 'first_vid' is over 'last_vid'. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR,
    "ARAD_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR",
    "The range defined by: 'first_vid' and 'last_vid'. \n\r "
    "Part of the interval is already defined in the given 'local_port_ndx'.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_ARAD-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'raw_key' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'learn_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_LIF_AC_LEARN_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'ing_vlan_edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_pcp_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'l2cp_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'da_mac_address_lsb' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_id' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_lif_entries' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'isid_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_LIF_VSI_IS_NOT_P2P_ERR,
    "ARAD_PP_LIF_VSI_IS_NOT_P2P_ERR",
    "The VSI should not be used for P2P services. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR,
    "ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR",
    "AC key cannot be port x vlan x vlan when the packet format is port x vlan. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR,
    "ARAD_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR",
    "Inner vlan exists while the outer vlan is marked 'ignore'. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_assignment_mode' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_LIF_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_LIF_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'lif_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_TERM_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_TERM_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'termination_profile' for PWE is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'action_profile' for PWE is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'default_frwrd_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'port_da_not_found_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR",
    "The parameter 'default_frwrd_profile' is out of range. \n\r "
    "The range is: 0 - (16*1024)-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'protection_pass_val' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_MPLS_TUNNEL_MODEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_MPLS_TUNNEL_MODEL_OUT_OF_RANGE_ERR",
    "The parameter 'model' for PWE has to be 'pipe' for 'uniform'. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "ac_info->default_frwrd.default_frwd_type has to be LIF for P2P. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "ac_info->default_frwrd.default_frwd_type has to be VSI for MP. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "pwe_info->default_frwrd.default_frwd_type has to be LIF for P2P. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "pwe_info->default_frwrd.default_frwd_type has to be VSI for MP. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR",
    "orientation of P2P AC has to be SPOKE. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR",
    "orientation of P2P PWE has to be SPOKE. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_DA_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_DA_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'da_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_DA_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR,
    "ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR",
    "Key given for LIF is illegeal. \n\r ",    
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_ADD_VSI_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_ADD_VSI_OUT_OF_RANGE_ERR",
    "The parameter 'add_vsi' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_OFFSET_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'offset' is out of range. \n\r "
    "The range is: 0 - 0x7ffff.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_GRE_KEY_VPN_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_GRE_KEY_VPN_OUT_OF_RANGE_ERR",
    "The parameter 'gre-key ' is out of range. \n\r "
    "The range is: 0 - 2^24-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_GRE_KEY_VSI_NOT_EQ_LIF_ERR,
    "ARAD_PP_LIF_GRE_KEY_VSI_NOT_EQ_LIF_ERR",
    "'VSI' has to be equal to LIF index\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VXLAN_KEY_VPN_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_VXLAN_KEY_VPN_OUT_OF_RANGE_ERR",
    "The parameter 'vxlan-key ' is out of range. \n\r "
    "The range is: 0 - 2^24-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_VXLAN_KEY_VSI_NOT_EQ_LIF_ERR,
    "ARAD_PP_LIF_VXLAN_KEY_VSI_NOT_EQ_LIF_ERR",
    "'VSI' has to be equal to LIF index\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_PROFILE_OUT_OF_RANGE_ERR",
    "The inlif profile field is out of range.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32 
arad_pp_lif_index_to_entry_ndx( 
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID         lif_index) 
{
    if (SOC_IS_JERICHO(unit)) {
        return ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(lif_index);
    } else {
        return lif_index;
    }
}
int
arad_pp_lif_index_to_sem_result( 
    int                    unit,
    SOC_PPC_LIF_ID         base_lif_id,
    SOC_PPC_LIF_ID         ext_lif_id,
    uint32                 *sem_result) 
{
    soc_error_t rv;
    SOC_PPC_LIF_ID actual_ext = ext_lif_id;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
        if (actual_ext == 0) {
            bcm_dpp_am_local_inlif_info_t inlif_info;
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, base_lif_id, &inlif_info);
            SOCDNX_IF_ERR_EXIT(rv);
            actual_ext = inlif_info.ext_lif_id;
        }
        *sem_result = ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_SEM_RESULT_ID(base_lif_id,actual_ext);
    } else {
        *sem_result = base_lif_id;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 
arad_pp_sem_result_to_lif_index( 
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 sem_result) 
{
    bcm_dpp_am_local_inlif_info_t       inlif_info;
    uint32      lif_id;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
         
         rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, sem_result, &inlif_info);
         SOCDNX_IF_ERR_EXIT(rv);       
         lif_id = (inlif_info.valid) ? sem_result : (sem_result+1) ; 
         return lif_id;
    } else {
        return sem_result;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
arad_pp_lif_is_wide_entry(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_OUT uint8 *is_wide_entry,
        SOC_SAND_OUT uint8 *ext_type)
{
    soc_error_t rv = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    
    *is_wide_entry = FALSE;


    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        
        goto exit;
    }

    if (is_ingress) {
        bcm_dpp_am_local_inlif_info_t inlif_info;

        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, lif_id, &inlif_info);
        SOCDNX_IF_ERR_EXIT(rv);

        *is_wide_entry = inlif_info.local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ; 

    } else {
        bcm_dpp_am_local_out_lif_info_t outlif_info;

        rv = SOC_PPC_OUTLIF_SW_STATE_INFO_GET(unit, lif_id, &outlif_info);
        SOCDNX_IF_ERR_EXIT(rv);

        *is_wide_entry = outlif_info.local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_WIDE ;

        
        if (ext_type && (*is_wide_entry)) {
            
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_extension_type_get,
                                     (unit, ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_BANK_ID(unit, lif_id), 
                                      ext_type));
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
  SOCDNX_FUNC_RETURN;

}



void
arad_pp_l2_lif_convert_protection_fields_to_hw(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 protection_pointer,
        SOC_SAND_IN  uint8  protection_pass_value,
        SOC_SAND_OUT uint32 *hw_protection_pointer,
        SOC_SAND_OUT uint32 *hw_protection_path)
{
    *hw_protection_pointer = protection_pointer;
    *hw_protection_path = protection_pass_value;

    *hw_protection_pointer &= ARAD_PP_LIF_PROTECTION_POINTER_MASK_ARAD;

    if (SOC_IS_JERICHO(unit)) {
        
        if (!(SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit))) {
            *hw_protection_path = protection_pointer >> ARAD_PP_LIF_PROTECTION_POINTER_NOF_BITS_ARAD;
        }
    }
}



void
arad_pp_l2_lif_convert_protection_fields_from_hw(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 hw_protection_pointer,
        SOC_SAND_IN  uint32 hw_protection_path,
        SOC_SAND_OUT uint32 *protection_pointer,
        SOC_SAND_OUT uint8  *protection_pass_value)
{
    *protection_pointer = hw_protection_pointer;
    *protection_pass_value = hw_protection_path;

    if (SOC_IS_JERICHO(unit)) {
        
        if (!(SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit))) {
            *protection_pointer |= hw_protection_path << ARAD_PP_LIF_PROTECTION_POINTER_NOF_BITS_ARAD;
            *protection_pass_value = 0;
        }
    }
}

uint32
arad_pp_l2_lif_protection_pointer_max_val(
        SOC_SAND_IN  int unit)
{
    if (SOC_IS_JERICHO(unit) && SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit)) {
        return ARAD_PP_LIF_PROTECTION_POINTER_MASK_ARAD;
    }

    return SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id);
}                




uint32
  arad_pp_lif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
    uint32
      res = SOC_SAND_OK;
    uint32
        reg_val,
        reg_val2;
    uint64
      reg_val_64;    


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  
  COMPILER_64_SET(reg_val_64, 0x0, 0xffffffff);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASKS_BASEr(unit, SOC_CORE_ALL,  reg_val_64));

  
  COMPILER_64_SET(reg_val_64, 0x0, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASKS_KEY_Br(unit, SOC_CORE_ALL,  reg_val_64));

  
  COMPILER_64_SET(reg_val_64, 0x0fff, 0x0fff0000);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASKS_KEY_Cr(unit, SOC_CORE_ALL,  reg_val_64 ));

  
  COMPILER_64_SET(reg_val_64, 0xffff0000, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASKS_RESULT_Br(unit, SOC_CORE_ALL,  reg_val_64 ));

  
  if (SOC_DPP_CONFIG(unit)->trill.transparent_service != 0) {
      COMPILER_64_SET(reg_val_64, 0x0, 0xffff);
  } else {
      COMPILER_64_SET(reg_val_64, 0x0, 0x0);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASKS_RESULT_Cr(unit, SOC_CORE_ALL,  reg_val_64 ));

  
   
  if (!SOC_IS_ARADPLUS(unit)) {
      if (SOC_DPP_CONFIG(unit)->trill.transparent_service != 0) {
          reg_val = 0xF3AA5500;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASK_PROFILE_MAPr_REG32(unit, SOC_CORE_ALL,  reg_val ));
      } else {
          reg_val = 0xFFAA5500;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASK_PROFILE_MAPr_REG32(unit, SOC_CORE_ALL,  reg_val ));
      }
  }
  else {
      if (SOC_DPP_CONFIG(unit)->trill.transparent_service != 0) {
          reg_val = 0x55550000;
          reg_val2 = 0xF3F3AAAA;
          COMPILER_64_SET(reg_val_64, reg_val2, reg_val);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASK_PROFILE_MAPr(unit, SOC_CORE_ALL,  reg_val_64));
      } else {
          reg_val = 0x55550000;
          reg_val2 = 0xFFFFAAAA;
          COMPILER_64_SET(reg_val_64, reg_val2, reg_val);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_VSI_MASK_PROFILE_MAPr(unit, SOC_CORE_ALL,  reg_val_64));
     }          
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_init_unsafe()", 0, 0);
}

STATIC
uint32
  arad_pp_lif_pwe_vc_label_to_isem_key_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                 in_vc_label,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO     *pwe_additional_info,
    SOC_SAND_IN  int8                                   has_gal,
    SOC_SAND_OUT  ARAD_PP_ISEM_ACCESS_KEY               *isem_key
  )
{
  uint32 res = SOC_SAND_OK;
  uint8 use_label_index = FALSE;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(isem_key);

  ARAD_PP_CLEAR(isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if ((has_gal) &&
      (!use_label_index || (SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(in_vc_label)!= SOC_PPC_MPLS_LABEL_INDEX_SECOND))) {
      LOG_ERROR(BSL_LS_SOC_LIF,
                (BSL_META_U(unit,
                            " GAL supported only with index 2.\n\r")));
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR, 20, exit);

  }
    
  
  if(pwe_additional_info->vlan_domain_valid) 
  {

     isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VLAN_DOMAIN_MPLS;
     isem_key->key_info.mpls.label = SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(in_vc_label);
     isem_key->key_info.mpls.vlan_domain = pwe_additional_info->vlan_domain;
  } else if (use_label_index)
  { 
    switch (SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(in_vc_label))
    {
    case SOC_PPC_MPLS_LABEL_INDEX_FIRST:
      isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1; 
      break;
    case SOC_PPC_MPLS_LABEL_INDEX_SECOND:
      isem_key->key_type = has_gal ? ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_PWE_L2_GAL:ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2;
      break;
    case SOC_PPC_MPLS_LABEL_INDEX_THIRD:
      isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3; 
      break;
    case SOC_PPC_MPLS_LABEL_INDEX_ALL:
        
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR, 60, exit);
        break;
      
      
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_VC_LABEL_KEY_ILLEGAL_ERR, 62, exit);
    }
    isem_key->key_info.mpls.label = SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(in_vc_label);
    if (SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) {
        isem_key->key_info.mpls.label2 = pwe_additional_info->outer_vid;              
        isem_key->key_info.mpls.label2 |= pwe_additional_info->outer_vid_valid << 12;  
        isem_key->key_info.mpls.label2 |= pwe_additional_info->inner_vid << 13;         
        isem_key->key_info.mpls.label2 |= pwe_additional_info->inner_vid_valid << 25;   
    }
  } else 
  {
    isem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS; 
    isem_key->key_info.mpls.label = in_vc_label;
  }  
  isem_key->key_info.mpls.is_bos = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_pwe_vc_label_to_isem_key_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_map_range_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                    *in_vc_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_map_range_set_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_map_range_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE                    *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_IN_VC_RANGE, in_vc_range, 10, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_map_range_set_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_map_range_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_map_range_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_map_range_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE                    *in_vc_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_map_range_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_add_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                    in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                            lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                   *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_pwe_add_internal_unsafe(
          unit,
          in_vc_label,
          FALSE,
          lif_index,
		  pwe_additional_info,
          pwe_info,
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_add_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_add_internal_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                    in_vc_label,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                            lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        *pwe_additional_info,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                   *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint32
    temp = 0, protection_pointer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  protection_pointer = (pwe_info->protection_pointer == 0 && SOC_IS_JERICHO(unit)) ? -1 : pwe_info->protection_pointer;

    
    if (pwe_info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P)
    {
      ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA
        data;

      res = arad_pp_ihp_lif_table_label_pwe_p2p_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

      data.type = 0x0; 
      data.destination_valid = 1;
      data.vsi_assignment_mode = pwe_info->vsi_assignment_mode;

      if (pwe_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY)
      {
        data.eei_or_out_lif_type = 0;  
      }
      else 
      {
        data.eei_or_out_lif_type = 1; 
      }

      data.in_lif_profile = pwe_info->lif_profile;
      data.oam_lif_set = pwe_info->oam_valid; 
      data.termination_profile = pwe_info->term_profile & 0x3; 
      data.termination_profile_msb = (pwe_info->term_profile >> 2) & 0x1; 
      data.action_profile_index = pwe_info->action_profile;
      data.tpid_profile = pwe_info->tpid_profile_index;
      data.cos_profile = pwe_info->cos_profile;

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, protection_pointer, pwe_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
          data.orientation_is_hub = pwe_info->orientation;
      } else {
          data.orientation_is_hub = pwe_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
      }
      data.model_is_pipe = pwe_info->model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE ? 0x1 : 0x0;
      data.in_lif = pwe_info->has_gal ? pwe_info->lif_table_entry_in_lif_field : lif_index; 

      if (SOC_IS_JERICHO(unit)) {
          data.sys_in_lif = pwe_info->global_lif_id;
      }

      res = arad_pp_fwd_decision_in_buffer_build(
              unit, 
              ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT, 
              &pwe_info->default_frwrd.default_forwarding, 
              &data.destination, 
              &temp
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

      if(pwe_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) 
      {
                
        res = soc_sand_bitstream_get_field(&temp, 0, 24, &data.eei_or_out_lif);
        SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);      
      } 
      else
      {
        
        
        data.eei_or_out_lif = temp;         
        res = soc_sand_bitstream_set_bit(&data.eei_or_out_lif, SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit)); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);     
      }

      res = arad_pp_ihp_lif_table_label_pwe_p2p_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    }
    if (pwe_info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
    {
        ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA
            data;
        ARAD_PP_LEM_ACCESS_PAYLOAD
            payload;
        uint32
            payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];

        ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
        ARAD_CLEAR(payload_data, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);

      res = arad_pp_ihp_lif_table_label_pwe_mp_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

      data.type = 0x1; 
      data.in_lif_valid = pwe_info->use_lif;
      data.in_lif_profile = pwe_info->lif_profile;
      data.vsi = pwe_info->vsid;
      data.tt_learn_enable = pwe_info->learn_record.enable_learning;
      data.tpid_profile = pwe_info->tpid_profile_index;
      if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
          data.orientation_is_hub = pwe_info->orientation;
      } else {
          data.orientation_is_hub = pwe_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
      }
      data.action_profile_index = pwe_info->action_profile;
      data.cos_profile = pwe_info->cos_profile;
      data.oam_lif_set = pwe_info->oam_valid;
      data.da_not_found_profile = pwe_info->default_forward_profile;
      data.vsi_assignment_mode = pwe_info->vsi_assignment_mode;
      data.tt_learn_enable = SOC_SAND_BOOL2NUM(pwe_info->learn_record.enable_learning);

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, protection_pointer, pwe_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      
      if (SOC_IS_JERICHO(unit)) {
          data.termination_profile = pwe_info->term_profile;
          data.sys_in_lif = pwe_info->global_lif_id;
      } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          data.termination_profile = pwe_info->term_profile & 0x3; 
          data.termination_profile_msb = (pwe_info->term_profile >> 2) & 0x1; 
      }
    
      res = arad_pp_fwd_decision_in_buffer_build(
        unit, 
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        &pwe_info->learn_record.learn_info,
        &payload.dest,
        &payload.asd
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

      if(pwe_info->learn_record.learn_info.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
        payload.flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;
      }
      else if(pwe_info->learn_record.learn_info.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
        payload.flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
      }

      
      payload.is_learn_data = 1;

      res = arad_pp_lem_access_payload_build(
         unit,
         &payload,
         payload_data
         ); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      COMPILER_64_SET(data.learn_data, payload_data[1], payload_data[0]);
     
      res = arad_pp_ihp_lif_table_label_pwe_mp_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
    }

    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_PWE,
            pwe_info->service_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3001, exit);

    
    if (SOC_IS_JERICHO(unit) && pwe_info->is_extended)
    {
        uint64 additional_data;
        COMPILER_64_SET(additional_data,0,0);
           
        res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,additional_data,FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
    }

    if (!ignore_key)
    {
      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      res = arad_pp_lif_pwe_vc_label_to_isem_key_unsafe(
              unit,
              in_vc_label,
              pwe_additional_info,
              pwe_info->has_gal,
              &sem_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      
      ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
      res = arad_pp_lif_index_to_sem_result(unit, lif_index, pwe_info->ext_lif_id, &sem_entry.sem_result_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
      res = arad_pp_isem_access_entry_add_unsafe(
              unit,
              &sem_key,
              &sem_entry,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
    }
    else
    {
      *success = SOC_SAND_SUCCESS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_add_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PPC_LIF_ID                            lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO                   *pwe_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    use_label_index = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_ADD_VERIFY);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (!use_label_index) 
  {     
    SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = SOC_PPC_L2_LIF_PWE_INFO_verify(unit, pwe_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_add_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_get_unsafe(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                  in_vc_label,
    SOC_SAND_OUT SOC_PPC_LIF_ID                         *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO     *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                *pwe_info,
    SOC_SAND_OUT uint8                                  *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_pwe_get_internal_unsafe(
          unit,
          in_vc_label,
          FALSE,
          lif_index,
		  pwe_additional_info,
          pwe_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                  in_vc_label,
    SOC_SAND_IN  uint8                                   ignore_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                          *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO      *pwe_additional_info,
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO                 *pwe_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint32
    flags = 0,
    temp = 0;
  uint32 profile_orientation = 0, protection_pointer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_L2_LIF_PWE_INFO_clear(pwe_info);

  if (!ignore_key)
  {
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    res = arad_pp_lif_pwe_vc_label_to_isem_key_unsafe(
            unit,
            in_vc_label,
			pwe_additional_info,
            0, 
            &sem_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    res = arad_pp_isem_access_entry_get_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (*found)
  {
    *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

    do
    {
        ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA data;

        res = arad_pp_ihp_lif_table_label_pwe_p2p_tbl_get_unsafe(
                unit,
                *lif_index,
                &data
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

        if (data.type == 0x0) 
        {
            pwe_info->service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P;
            pwe_info->vsi_assignment_mode = data.vsi_assignment_mode;

            pwe_info->lif_profile = data.in_lif_profile;
            pwe_info->oam_valid = data.oam_lif_set;
            pwe_info->term_profile = (data.termination_profile_msb << 2) | data.termination_profile;
            pwe_info->action_profile = data.action_profile_index;
            pwe_info->tpid_profile_index = data.tpid_profile;
            pwe_info->cos_profile = data.cos_profile;
            if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                              (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                               &(pwe_info->lif_profile), &profile_orientation));
                    SOC_SAND_CHECK_FUNC_RESULT(res,21,exit);
                }
                pwe_info->orientation = data.orientation_is_hub;
                pwe_info->network_group = (profile_orientation << 1) | pwe_info->orientation;
            } else {
                pwe_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
            }
            ARAD_DEVICE_CHECK(unit, exit);
            pwe_info->model = data.model_is_pipe ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE : SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;
            pwe_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
            pwe_info->has_gal = (data.in_lif == *lif_index) ? 0 : 1;
            pwe_info->lif_table_entry_in_lif_field = pwe_info->has_gal ? data.in_lif : 0;
            pwe_info->global_lif_id = data.sys_in_lif;

            
            arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                         &(protection_pointer), &(pwe_info->protection_pass_value));
            
            pwe_info->protection_pointer = (protection_pointer == ARAD_PP_LIF_PROTECTION_POINTER_DEFAULT_VALUE && SOC_IS_JERICHO(unit)) ? 0 : protection_pointer;

            flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
            
            if (data.eei_or_out_lif_type) 
            {
                
                res = soc_sand_bitstream_get_field(&data.eei_or_out_lif, 0, SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit), &temp);
                SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);
                flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
            } 
            else
            {
                
                temp = data.eei_or_out_lif;
                flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;
            }

            res = arad_pp_fwd_decision_in_buffer_parse(
                    unit,           
                    data.destination, 
                    temp, 
                    flags, 
                    &(pwe_info->default_frwrd.default_forwarding)
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

            goto exit;
        }
    } while(0);

    do
    {
      ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA
        data;

    res = arad_pp_ihp_lif_table_label_pwe_mp_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    if (data.type == 0x1) 
    {
        ARAD_PP_LEM_ACCESS_PAYLOAD  payload;
        uint32 payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];

        ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
        ARAD_CLEAR(payload_data, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);

        pwe_info->service_type = SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP;
        pwe_info->lif_profile = data.in_lif_profile;
        pwe_info->vsid = data.vsi;
        pwe_info->learn_record.enable_learning = data.tt_learn_enable;
        pwe_info->tpid_profile_index = data.tpid_profile;
        if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
            if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                          (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                           &(pwe_info->lif_profile), &profile_orientation));
                SOC_SAND_CHECK_FUNC_RESULT(res,41,exit);
            }
            pwe_info->orientation = data.orientation_is_hub;
            pwe_info->network_group = (profile_orientation << 1) | pwe_info->orientation;
        } else {
            pwe_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
        }
        pwe_info->action_profile = data.action_profile_index;
        pwe_info->cos_profile = data.cos_profile;
        pwe_info->oam_valid = data.oam_lif_set;
        pwe_info->default_forward_profile = data.da_not_found_profile;
        pwe_info->protection_pass_value = data.protection_path;
        pwe_info->protection_pointer = data.protection_pointer;
        pwe_info->term_profile = (data.termination_profile_msb << 2) | data.termination_profile;
        pwe_info->vsi_assignment_mode = data.vsi_assignment_mode;
        pwe_info->learn_record.enable_learning = SOC_SAND_NUM2BOOL(data.tt_learn_enable);
        pwe_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
        pwe_info->global_lif_id = data.sys_in_lif;

        ARAD_DEVICE_CHECK(unit, exit);
        
        arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                           &(protection_pointer), &(pwe_info->protection_pass_value));
        
        pwe_info->protection_pointer = (protection_pointer == -1 && SOC_IS_JERICHO(unit)) ? 0 : protection_pointer;

        payload_data[0] = COMPILER_64_LO(data.learn_data);
        payload_data[1] = COMPILER_64_HI(data.learn_data);

        payload.is_learn_data = 1;

        res = arad_pp_lem_access_payload_parse(
                unit,
                payload_data,
                ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
                &payload
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

        res = arad_pp_fwd_decision_in_buffer_parse(
                unit,  
                payload.dest,
                payload.asd,
                payload.flags, 
                &pwe_info->learn_record.learn_info
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
     
        goto exit;
            }
        } while(0);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    use_label_index = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_GET_VERIFY);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (!use_label_index) 
  {     
    SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_pwe_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL              in_vc_label,
    SOC_SAND_IN  uint8                               remove_from_sem,
    SOC_SAND_IN  uint8                               is_gal,
    SOC_SAND_OUT uint32                             *lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO *pwe_additional_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  res = arad_pp_lif_pwe_vc_label_to_isem_key_unsafe(
          unit,
          in_vc_label,
		  pwe_additional_info,
          is_gal,
          &sem_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
  res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  if (remove_from_sem) {
      *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);
  }

  if (found)
  {
    ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA
        data;

    if (remove_from_sem) {
      res = arad_pp_isem_access_entry_remove_unsafe(
            unit,
              &sem_key
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
    }

    sal_memset(&data,0,sizeof(ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA));

    res = arad_pp_ihp_lif_table_label_pwe_p2p_tbl_set_unsafe(
            unit,
            *lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

    
    if (!is_gal) { 
        res = arad_pp_sw_db_lif_table_entry_use_set(
                unit,
                *lif_index,
                SOC_PPC_LIF_ENTRY_TYPE_EMPTY,
                0
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem
  )
{
  uint8
    use_label_index = FALSE;  
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_PWE_REMOVE_VERIFY);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (!use_label_index) 
  {     
    SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_remove_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint8                                     is_enable,
    SOC_SAND_IN  SOC_PPC_PORT                                  pp_port_ndx,
    SOC_SAND_IN  uint8                                     is_outer,
    SOC_SAND_IN  uint32                                      range_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE);

  res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          core_id,
          pp_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  fld_val = (is_enable == TRUE) ? 0x1:0x0;
  
  if(is_outer == TRUE)
  {
    res = soc_sand_bitstream_set(&valid_range_tbl_data.range_valid_0, range_ndx, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_sand_bitstream_set(&valid_range_tbl_data.range_valid_1, range_ndx, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
          unit,
          core_id,
          pp_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_range_valid_set_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vlan_compression_range_valid_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      range_ndx,
    SOC_SAND_OUT uint8                                     *is_enable
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  SOC_PPC_PORT
    pp_port_ndx;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE);

  *is_enable = FALSE;
  for (core=0; core <  SOC_DPP_DEFS_GET(unit, nof_cores);core++) {
      for (pp_port_ndx = 0; pp_port_ndx <= ARAD_PP_PORT_MAX; pp_port_ndx++)
      {
        res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
                unit,
                core,
                pp_port_ndx,
                &valid_range_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (soc_sand_bitstream_test_bit(&valid_range_tbl_data.range_valid_0,range_ndx) || soc_sand_bitstream_test_bit(&valid_range_tbl_data.range_valid_1,range_ndx))
        {
          *is_enable = TRUE;
        }
      }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_range_valid_get_unsafe()", 0, 0);
}

uint32
arad_pp_l2_lif_vlan_compression_is_valid_range(
  SOC_SAND_IN  int                                     unit,
  SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                     *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK,
    range_bitmap[1];
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  uint32
    range_ndx;
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE);

  ARAD_CLEAR(&tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          vlan_range_key->core_id,
          vlan_range_key->local_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(vlan_range_key->is_outer == TRUE)
  {
    *range_bitmap = valid_range_tbl_data.range_valid_0;
  }
  else
  {
    *range_bitmap = valid_range_tbl_data.range_valid_1;
  }

  for (range_ndx = 0; range_ndx < SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++)
  {
    
    if(soc_sand_bitstream_test_bit(range_bitmap,range_ndx))
    {
      res =  arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if (!((tbl_data.vlan_range_lower_limit > vlan_range_key->last_vid)
        || (tbl_data.vlan_range_upper_limit < vlan_range_key->first_vid)))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR, 30, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_is_valid_range()", 0, 0);
}


uint32
  arad_pp_l2_lif_vlan_compression_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                     *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    vlan_compression_range_multiset_first_appear = FALSE;
  uint32
    ref_count = 0,
    range_internal_key,
    vlan_compression_range_multiset_ndx = 0;
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_CLEAR(&tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  *success = SOC_SAND_SUCCESS;

  
  range_internal_key = ARAD_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(vlan_range_key->first_vid,vlan_range_key->last_vid);
  tbl_data.vlan_range_lower_limit = vlan_range_key->first_vid;
  tbl_data.vlan_range_upper_limit = vlan_range_key->last_vid;

  
  res = arad_sw_db_multiset_lookup(
          unit,
          ARAD_SW_DB_CORE_ANY,
		  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
          &range_internal_key,
          &vlan_compression_range_multiset_ndx,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (ref_count == 0)
  {
    
    res = arad_sw_db_multiset_add(
      unit,
	  ARAD_SW_DB_CORE_ANY,
      ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
      &range_internal_key,
      &vlan_compression_range_multiset_ndx,
      &vlan_compression_range_multiset_first_appear,
      success
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }

    if (vlan_compression_range_multiset_first_appear)
    {
      
      res =  arad_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
              unit,
              vlan_compression_range_multiset_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    
    res = arad_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
            unit,
            vlan_range_key->core_id,
            TRUE,
            vlan_range_key->local_port_ndx,
            vlan_range_key->is_outer,
            vlan_compression_range_multiset_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_add_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vlan_compression_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                     *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_VLAN_RANGE_KEY, vlan_range_key, 10, exit);

  
  res = arad_pp_l2_lif_vlan_compression_is_valid_range(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_add_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_vlan_compression_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                     *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK,
    range_internal_key,
    vlan_compression_range_multiset_ndx = 0,
    ref_count = 0;
  uint8
    last_appear = FALSE,
    is_enable;
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  int core=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);

  ARAD_CLEAR(&tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    unit,
    core,
    vlan_range_key->local_port_ndx,
    &valid_range_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  range_internal_key = ARAD_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(vlan_range_key->first_vid,vlan_range_key->last_vid);

  res = arad_sw_db_multiset_lookup(
          unit,
          ARAD_SW_DB_CORE_ANY,
		  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
          &range_internal_key,
          &vlan_compression_range_multiset_ndx,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(ref_count != 0)
  {
    
    res = arad_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
            unit,
            vlan_range_key->core_id,
            FALSE,
            vlan_range_key->local_port_ndx,
            vlan_range_key->is_outer,
            vlan_compression_range_multiset_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    res = arad_pp_l2_lif_vlan_compression_range_valid_get_unsafe(
            unit,
            vlan_compression_range_multiset_ndx,
            &is_enable
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    if(is_enable == FALSE)
    {
      
      res = arad_sw_db_multiset_remove(
              unit,
			  ARAD_SW_DB_CORE_ANY,
              ARAD_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
              &range_internal_key,
              &vlan_compression_range_multiset_ndx,
              &last_appear
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      res =  arad_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
              unit,
              vlan_compression_range_multiset_ndx,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vlan_compression_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY                     *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_VLAN_RANGE_KEY, vlan_range_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_remove_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_vlan_compression_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                        core_id,
    SOC_SAND_IN  SOC_PPC_PORT                               local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    outer_range_bitmap[1],
    inner_range_bitmap[1];
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  uint32
    range_ndx;
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET_UNSAFE);
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  res = arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *outer_range_bitmap = valid_range_tbl_data.range_valid_0;
  *inner_range_bitmap = valid_range_tbl_data.range_valid_1;

  for (range_ndx = 0; range_ndx < SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++)
  {
    
    if(soc_sand_bitstream_test_bit(outer_range_bitmap, range_ndx))
    {
    
      ARAD_CLEAR(&tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);
      res =  arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      vlan_range_info->outer_vlan_range[range_ndx].first_vid = tbl_data.vlan_range_lower_limit;
      vlan_range_info->outer_vlan_range[range_ndx].last_vid = tbl_data.vlan_range_upper_limit;
    }

     
    if(soc_sand_bitstream_test_bit(inner_range_bitmap, range_ndx))
    {
      ARAD_CLEAR(&tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);
      res =  arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      vlan_range_info->inner_vlan_range[range_ndx].first_vid = tbl_data.vlan_range_lower_limit;
      vlan_range_info->inner_vlan_range[range_ndx].last_vid = tbl_data.vlan_range_upper_limit;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_get_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_l2_lif_vlan_compression_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_PORT                               local_port_ndx,
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO            *vlan_range_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VLAN_COMPRESSION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, (ARAD_NOF_LOCAL_PORTS(unit)-1), ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vlan_compression_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_l2_lif_ac_map_key_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
	qual_key_ndx = 0,
	entry_ndx,
	ref_count,
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
	success;
  uint8               
	first_appear;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(qual_key);

  qual_key_ndx = (qual_key->port_profile << 4) | (qual_key->pkt_parse_info.outer_tpid << 3) | (qual_key->pkt_parse_info.is_outer_prio << 2) | (qual_key->pkt_parse_info.inner_tpid << 0); 
   
  
  res = arad_sw_db_multiset_lookup(
    unit,
	ARAD_SW_DB_CORE_ANY,
    ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,
    &qual_key_ndx,
    &entry_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count == 0) 
  {
	  
	  res = arad_sw_db_multiset_add(
		  unit,
		  ARAD_SW_DB_CORE_ANY,
		  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,
		  &qual_key_ndx,
		  &entry_ndx,
		  &first_appear,
		  &success
		  );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
	  if (success != SOC_SAND_SUCCESS)
	  {
		  goto exit;
	  }

	  
	  res = arad_pp_isem_access_prog_sel_ac_key_set_unsafe(
		  unit,
		  entry_ndx,
		  qual_key,
		  &key_mapping
		  );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else 
  {
	  if (key_mapping == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_DEFAULT) 
	  {
		  
		  res = arad_sw_db_multiset_remove(
			  unit,
			  ARAD_SW_DB_CORE_ANY,
			  ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,
			  &qual_key_ndx,
			  &entry_ndx,
			  &first_appear
		  );
		  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
	  }
	  else
	  {
		  SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_KEY_MAPPING_ENTRY_EXISTS_ERR, 50, exit);
	  }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_map_key_set_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_map_key_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_AC_KEY_QUALIFIER, qual_key, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(key_mapping, ARAD_PP_LIF_KEY_MAPPING_MAX, ARAD_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR, 30, exit);

  if (qual_key->pkt_parse_info.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (key_mapping == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN || key_mapping == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 10, exit);
    }
  }

  if (qual_key->pkt_parse_info.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE
    && qual_key->pkt_parse_info.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (key_mapping == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_map_key_set_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_map_key_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_AC_KEY_QUALIFIER, qual_key, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_map_key_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_ac_map_key_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE                  *key_mapping
  )
{
  uint32
	qual_key_ndx = 0,
	entry_ndx,
	ref_count,
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(qual_key);
  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  qual_key_ndx = (qual_key->port_profile << 4) | (qual_key->pkt_parse_info.outer_tpid << 3) | (qual_key->pkt_parse_info.is_outer_prio << 2) | (qual_key->pkt_parse_info.inner_tpid << 0); 
   
  
  res = arad_sw_db_multiset_lookup(
    unit,
	ARAD_SW_DB_CORE_ANY,
    ARAD_PP_SW_DB_MULTI_SET_L2_LIF_PROFILE,
    &qual_key_ndx,
    &entry_ndx,
    &ref_count
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count != 0) 
  {
	  
	  res = arad_pp_isem_access_prog_sel_ac_key_get_unsafe(
		  unit,
		  entry_ndx,
		  qual_key,
		  key_mapping
		  );
	  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else 
  {
	  
	  *key_mapping = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_DEFAULT;
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_map_key_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
          unit,
          lif_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe(
          unit,
          lif_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_mp_info_set_unsafe()", lif_ndx, 0);
}

uint32
  arad_pp_l2_lif_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_ndx, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_AC_MP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_mp_info_set_verify()", lif_ndx, 0);
}

uint32
  arad_pp_l2_lif_ac_mp_info_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_ndx, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_mp_info_get_verify()", lif_ndx, 0);
}


uint32
  arad_pp_l2_lif_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_L2_LIF_AC_MP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_MP_INFO_clear(info);

  res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
          unit,
          lif_ndx,
          &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_mp_info_get_unsafe()", lif_ndx, 0);
}

uint32
  arad_pp_l2_lif_ac_key_to_sem_key_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY                         *sem_key
  )
{
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE
    simple_type,
    type_vlan;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  sem_key->key_info.l2_eth.vlan_domain = ac_key->vlan_domain;
  simple_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT;

  if ((ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN ||
      ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN)
    )
  {
    type_vlan = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
  }
  else
  {
    type_vlan = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
  }
  
  if ((ac_key->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID &&
      ac_key->outer_vid == SOC_PPC_LIF_IGNORE_OUTER_VID)
      || ac_key->key_type == simple_type
     )
  {
         sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD;

         sem_key->key_info.l2_eth.outer_vid = 0;
         sem_key->key_info.l2_eth.inner_vid = 0;
  }
  else if (ac_key->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID
          || ac_key->key_type == type_vlan)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID;
    sem_key->key_info.l2_eth.inner_vid = 0;
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  else
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID;
    sem_key->key_info.l2_eth.inner_vid = ac_key->inner_vid;
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID;    
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID;
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID;    
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
    sem_key->key_info.l2_eth.outer_pcp = ac_key->outer_pcp;
  }
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID;    
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
    sem_key->key_info.l2_eth.inner_vid = ac_key->inner_vid;
    sem_key->key_info.l2_eth.outer_pcp = ac_key->outer_pcp;
  }
  
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_ETH)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID;
    sem_key->key_info.section_oam.port = ac_key->vlan_domain; 
    sem_key->key_info.section_oam.core= ac_key->core_id;
    sem_key->key_info.section_oam.port_valid = TRUE;
    sem_key->key_info.section_oam.ether_type= ac_key->ethertype;
    sem_key->key_info.section_oam.ether_type_valid= TRUE;
    sem_key->key_info.section_oam.outer_vid = ac_key->outer_vid;
    sem_key->key_info.section_oam.outer_vid_valid = TRUE;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN_ETH)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID_VID;  
    sem_key->key_info.section_oam.port = ac_key->vlan_domain; 
    sem_key->key_info.section_oam.core= ac_key->core_id;
    sem_key->key_info.section_oam.port_valid = TRUE;
    sem_key->key_info.section_oam.ether_type= ac_key->ethertype;
    sem_key->key_info.section_oam.ether_type_valid= TRUE;
    sem_key->key_info.section_oam.outer_vid = ac_key->outer_vid;
    sem_key->key_info.section_oam.outer_vid_valid = TRUE;
    sem_key->key_info.section_oam.inner_vid= ac_key->inner_vid;
    sem_key->key_info.section_oam.inner_vid_valid= TRUE;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL)
  {
    sem_key->key_type = SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()? ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_PORT_TUNNEL:ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL;    
    
    if(SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()
        &&ac_key->outer_vid > 0 ){
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_LSP_ECMP;
        sem_key->key_info.l2_pon.outer_pcp = 1<<31 | ((ac_key->outer_vid & 0x1) << 16) | ac_key->inner_vid;
    }
    sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
    sem_key->key_info.l2_pon.core= ac_key->core_id; 
    sem_key->key_info.l2_pon.port_valid = TRUE;
    sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
    sem_key->key_info.l2_pon.tunnel_id_valid = TRUE;    
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID;    
    sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
    sem_key->key_info.l2_pon.core= ac_key->core_id; 
    sem_key->key_info.l2_pon.port_valid = TRUE;
    sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
    sem_key->key_info.l2_pon.tunnel_id_valid = TRUE;    
    sem_key->key_info.l2_pon.outer_vid = ac_key->outer_vid; 
    sem_key->key_info.l2_pon.outer_vid_valid = TRUE;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN)
  {
    if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_lif_lkup",0)) {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID;    
    }else {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID;    
    }
    sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
    sem_key->key_info.l2_pon.core= ac_key->core_id; 
    sem_key->key_info.l2_pon.port_valid = TRUE;
    sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
    sem_key->key_info.l2_pon.tunnel_id_valid = TRUE;    
    sem_key->key_info.l2_pon.outer_vid = ac_key->outer_vid; 
    sem_key->key_info.l2_pon.outer_vid_valid = TRUE;
    sem_key->key_info.l2_pon.inner_vid = ac_key->inner_vid; 
    sem_key->key_info.l2_pon.inner_vid_valid = TRUE;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID;    
    sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
    sem_key->key_info.l2_pon.core= ac_key->core_id; 
    sem_key->key_info.l2_pon.port_valid = TRUE;
    sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
    sem_key->key_info.l2_pon.tunnel_id_valid = (ac_key->tunnel == SOC_PPC_LIF_IGNORE_TUNNEL_VID) ? FALSE:TRUE;
    sem_key->key_info.l2_pon.outer_vid = ac_key->outer_vid; 
    sem_key->key_info.l2_pon.outer_vid_valid = (ac_key->outer_vid == SOC_PPC_LIF_IGNORE_OUTER_VID) ? FALSE:TRUE;
    sem_key->key_info.l2_pon.inner_vid = ac_key->inner_vid; 
    sem_key->key_info.l2_pon.inner_vid_valid = (ac_key->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID) ? FALSE:TRUE;
    sem_key->key_info.l2_pon.ether_type = ac_key->ethertype; 
    sem_key->key_info.l2_pon.ether_type_valid = (ac_key->ethertype == SOC_PPC_LIF_IGNORE_ETHER_TYPE) ? FALSE:TRUE;
    sem_key->key_info.l2_pon.outer_dei = 0;
    sem_key->key_info.l2_pon.outer_dei_valid = FALSE;
    sem_key->key_info.l2_pon.outer_pcp = ac_key->outer_pcp;
    sem_key->key_info.l2_pon.outer_pcp_valid = (ac_key->outer_pcp == SOC_PPC_LIF_IGNORE_OUTER_PCP) ? FALSE:TRUE;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TLS_PORT_TUNNEL)
  {
    sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL;    
    sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
    sem_key->key_info.l2_pon.core= ac_key->core_id; 
    sem_key->key_info.l2_pon.port_valid = TRUE;
    sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
    sem_key->key_info.l2_pon.tunnel_id_valid = TRUE;
    sem_key->key_info.l2_pon.flags = ARAD_PP_ISEM_ACCESS_L2_PON_TLS;
    
    if (SOC_DPP_CONFIG(unit)->pp.pon_tls_in_tcam == TRUE) {
      sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID;    
      sem_key->key_info.l2_pon.port = ac_key->vlan_domain; 
      sem_key->key_info.l2_pon.core= ac_key->core_id; 
      sem_key->key_info.l2_pon.port_valid = TRUE;
      sem_key->key_info.l2_pon.tunnel_id = ac_key->tunnel; 
      sem_key->key_info.l2_pon.tunnel_id_valid = TRUE;
      sem_key->key_info.l2_pon.outer_vid = ac_key->outer_vid; 
      sem_key->key_info.l2_pon.outer_vid_valid = FALSE;
      sem_key->key_info.l2_pon.inner_vid = ac_key->inner_vid; 
      sem_key->key_info.l2_pon.inner_vid_valid = FALSE;
      sem_key->key_info.l2_pon.ether_type = ac_key->ethertype; 
      sem_key->key_info.l2_pon.ether_type_valid = FALSE;
      sem_key->key_info.l2_pon.outer_dei = 0;
      sem_key->key_info.l2_pon.outer_dei_valid = FALSE;
      sem_key->key_info.l2_pon.outer_pcp = ac_key->outer_pcp;
      sem_key->key_info.l2_pon.outer_pcp_valid = FALSE;
    }
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID)
  {
      sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_SINGLE_TAG;
      sem_key->key_info.l2_eth.inner_vid = 0;
      sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }

  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID)
  {
      sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_DOUBLE_TAG;
      sem_key->key_info.l2_eth.inner_vid = ac_key->inner_vid;
      sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  
  if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2) {
      sem_key->key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2;
      sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }

  
  if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vt_tst1) 
  {
    if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_UNTAG)
    {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_UNTAG;      
        sem_key->key_info.l2_eth.outer_vid = 0;
    }
    else if (sem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID)  
    {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_COMPRESSED_TAG;
    } 
    else if (sem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID)
    {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_INITIAL_TAG;
    }    
    else if (sem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID) 
    {
        sem_key->key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_DOUBLE_TAG;
    }
    else if (sem_key->key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD) 
    {
        
    }
    else
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 4031, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_key_to_sem_key_unsafe()", 0, 0); 
}


uint32 arad_pp_l2_lif_ac_key_to_qualifier(SOC_SAND_IN  int                        unit,
                                          SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY     *ac_key,
                                          SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL qual_vals[])
{

    uint32 qual_index = 0;

    for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++qual_index)
    {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[qual_index]);
    }

    qual_index = 0;

    qual_vals[qual_index].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    qual_vals[qual_index].val.arr[0] = ac_key->vlan_domain;
    qual_index++;

    if (((ac_key->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID) && (ac_key->outer_vid != SOC_PPC_LIF_IGNORE_OUTER_VID)) ||
        ((ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN) || (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN)))
    {    

        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->outer_vid;
        qual_index++;

        qual_vals[qual_index].type =  SOC_PPC_FP_QUAL_IRPP_PROG_VAR;
        qual_vals[qual_index].val.arr[0]  = 2;
    }

    if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_ETH)
    {
        qual_index--;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->outer_vid ;
        qual_vals[qual_index].is_valid.arr[0]= 0xfff;

        qual_index++;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE;
        qual_vals[qual_index].is_valid.arr[0]= 0xffff;
        qual_vals[qual_index].val.arr[0] = ac_key->ethertype;
        qual_index++;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        qual_vals[qual_index].is_valid.arr[0]= 0x1ff;
        qual_vals[qual_index].val.arr[0] = ac_key->vlan_domain;
    }

    if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN_ETH)
    {
        qual_index--;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->outer_vid ;
        qual_vals[qual_index].is_valid.arr[0]= 0xfff;

        qual_index++;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->inner_vid;
        qual_vals[qual_index].is_valid.arr[0]= 0xfff;

        qual_index++;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE;
        qual_vals[qual_index].val.arr[0] = ac_key->ethertype;
        qual_vals[qual_index].is_valid.arr[0]= 0xffff;

        qual_index++;
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        qual_vals[qual_index].is_valid.arr[0]= 0x1ff;
        qual_vals[qual_index].val.arr[0] = ac_key->vlan_domain;
    }

    if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN)
    { 

      qual_vals[qual_index].type = SOC_PPC_FP_QUAL_INITIAL_VID;
      qual_vals[qual_index].val.arr[0] = ac_key->outer_vid;
    }

    if(ac_key->key_type  ==  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN)
    {
        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->outer_vid;
        qual_index++;

        qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        qual_vals[qual_index].val.arr[0] = ac_key->inner_vid;
        qual_index++;

        qual_vals[qual_index].type =  SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        qual_vals[qual_index].val.arr[0]  = 3;
    }
    if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2) {
        qual_vals[0].type = SOC_PPC_FP_QUAL_EID;
        qual_vals[0].val.arr[0] = ac_key->outer_vid;
        qual_vals[0].val.arr[1] = 0;
        
        qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID;
        qual_vals[1].val.arr[0] = (ac_key->outer_vid >> 3);
        qual_vals[1].val.arr[1] = 0;
     }

    if ((ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL) && (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()))
    { 
        if (ac_key->outer_vid > 0) {
            qual_vals[0].type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_LSP_ECMP_FP;
            qual_vals[0].val.arr[0] = 1 << 31 | ((ac_key->outer_vid & 0x1F) << 12) |( ac_key->inner_vid & 0xFFF);
            qual_vals[0].is_valid.arr[0] = 0x8001FFFF; 
            qual_vals[1].type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP;
            qual_vals[1].val.arr[0] = ac_key->tunnel;
            qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX; 
            qual_vals[2].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
            qual_vals[2].val.arr[0] = ac_key->vlan_domain;            
            qual_vals[2].is_valid.arr[0] = SOC_SAND_U32_MAX; 
        } else {
            qual_vals[qual_index].type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP;
            qual_vals[qual_index].val.arr[0] = ac_key->tunnel;

        }
    }

    return 0;
}


uint32
  arad_pp_l2_lif_ac_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8 ignore_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ignore_key = (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_NONE ) ? TRUE : FALSE;

  res = arad_pp_l2_lif_ac_add_internal_unsafe(
          unit,
          ac_key,
          ignore_key,
          lif_index,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_add_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                        *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;
  uint32
    temp = 0,
    additional_info = 0;
  SOC_PPC_RIF_ID rif_id;
  uint32         reg32_val;
  uint8          is_duplicate = 0;

  SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[3]={0};
  uint32  nof_tables;
  int i;

  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(ac_key);
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    
    res = arad_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    res = arad_pp_lif_index_to_sem_result(unit, lif_index, ac_info->ext_lif_id, &sem_entry.sem_result_ndx);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

    if (sem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD)
    {
      res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
              unit,
              ac_key->core_id,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

      vtt_port_config_tbl.default_port_lif = sem_entry.sem_result_ndx;

      res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
              unit,
              ac_key->core_id,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      *success = SOC_SAND_SUCCESS;
    }
    else
    {
        if (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2) {
            res = arad_pp_l2_lif_ac_key_to_qualifier(unit, ac_key, qual_vals);
            SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

            res = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A, qual_vals, 0,  &sem_entry, success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
        } else {

            
            res = arad_pp_dbal_vtt_sw_db_get(unit, &sem_key, &nof_tables, dbal_tables_id, &is_duplicate);

            SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

            if (nof_tables == 0) { 
                res = arad_pp_isem_access_entry_add_unsafe(unit, &sem_key, &sem_entry, success);
                SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
            } else { 
                res = arad_pp_l2_lif_ac_key_to_qualifier(unit, ac_key, qual_vals);
                SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);


                for (i = 0; i < nof_tables; i++) {

                    
                    if (!is_duplicate && i > 0 && *success == SOC_SAND_SUCCESS)
                    {
                        break;
                    }

                    res = arad_pp_dbal_entry_add(unit, dbal_tables_id[i], qual_vals, 0,  &sem_entry, success);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
                }

            }


        }
          

    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    if (ac_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC)
    {
      ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA
        data;

      data.type = 0x0; 
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      data.vlan_edit_vid_1 = ac_info->ing_edit_info.vid;
      data.vlan_edit_vid_2 = ac_info->ing_edit_info.vid2;
      if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2)) {
          data.orientation_is_hub = ac_info->orientation;
      } else {
          data.orientation_is_hub = ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
      }      
      data.in_lif_profile = ac_info->lif_profile;
      data.oam_lif_set = ac_info->oam_valid;
      data.use_in_lif = ac_info->use_lif;
      data.destination_valid = 1;
      data.out_lif = 0;
      data.out_lif_valid = 1; 
      data.sys_in_lif = 0;

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, ac_info->protection_pointer, ac_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      if (SOC_IS_JERICHO(unit) && ac_info->use_lif) {
           data.sys_in_lif = ac_info->global_lif;
      }

      res = arad_pp_fwd_decision_in_buffer_build(
             unit, 
             ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, 
             &ac_info->default_frwrd.default_forwarding, 
             &data.destination, 
             &data.out_lif
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      if (ac_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY)
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 25, exit);    
      }
      

      res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
        
        res = arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(
           unit,
           lif_index
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
#endif 

    }
    if (ac_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE)
    {
      ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA
        data;

      data.type = 0x1; 
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      data.vlan_edit_vid_1 = ac_info->ing_edit_info.vid;
      if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2)) {
          data.orientation_is_hub = ac_info->orientation;
      } else {
          data.orientation_is_hub = ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
      }      
      data.in_lif_profile = ac_info->lif_profile;
      data.vsi_assignment_mode = (ac_info->vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) ? ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN:ac_info->vsi_assignment_mode;
      data.oam_lif_set = ac_info->oam_valid;
      data.use_in_lif = ac_info->use_lif;
      data.destination_valid = 1;
      data.sys_in_lif = 0;

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, ac_info->protection_pointer, ac_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      if (SOC_IS_JERICHO(unit)&& ac_info->use_lif) {
           data.sys_in_lif = ac_info->global_lif;
      }

      res = arad_pp_fwd_decision_in_buffer_build(
        unit, 
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT, 
        &ac_info->default_frwrd.default_forwarding, 
        &data.destination, 
        &temp
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

      if(ac_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) 
      {
          if (SOC_IS_JERICHO(unit)) {
              
              data.eei_or_out_lif = temp;
              data.eei_or_out_lif_identifier = 1;
          } else {
                      
              res = soc_sand_bitstream_get_field(&temp, 0, 20, &data.eei_or_out_lif);
              SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

              res = soc_sand_bitstream_get_field(&temp, 20, 4, &data.eei_or_out_lif_identifier);
              SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
          }
      } 
      else {
          if (ac_info->default_frwrd.default_forwarding.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
          
                            
              
              res = soc_sand_bitstream_set_bit(&temp, SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit));      
          } else {
              
          }
          data.eei_or_out_lif = temp; 
          data.eei_or_out_lif_identifier = 0;
      } 

      res = arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_set_unsafe(
          unit,
          lif_index,
          &data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    }
    if (ac_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB)
    {
      ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA
        data;

      data.type = 0x2; 
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      data.vlan_edit_vid_1 = ac_info->ing_edit_info.vid;
      if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2)) {
          data.orientation_is_hub = ac_info->orientation;
      } else {
          data.orientation_is_hub = ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
      }      
      data.in_lif_profile = ac_info->lif_profile;
      data.vsi_assignment_mode = (ac_info->vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) ? ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN:ac_info->vsi_assignment_mode;
      data.oam_lif_set = ac_info->oam_valid;
      data.use_in_lif = ac_info->use_lif;
      data.destination_valid = 1;
      data.sys_in_lif = 0;

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, ac_info->protection_pointer, ac_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      if (SOC_IS_JERICHO(unit)) {
           if (ac_info->use_lif) {
               data.sys_in_lif = ac_info->global_lif;
           }
           data.eei_or_out_lif_type = 1; 
      }

      res = arad_pp_fwd_decision_in_buffer_build(
          unit, 
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, 
          &ac_info->default_frwrd.default_forwarding, 
          &data.destination, 
          &data.eei
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

      res = arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
    }
    if (ac_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
    {
      ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA
        data;

      data.type = 0x3; 
      data.vsi = ac_info->vsid;
      data.da_not_found_profile = ac_info->default_forward_profile;
      data.vlan_edit_vid_1 = ac_info->ing_edit_info.vid;
      data.vlan_edit_vid_2 = ac_info->ing_edit_info.vid2;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2)) {
          data.orientation_is_hub = ac_info->orientation;
      } else {
          data.orientation_is_hub = ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
      }      
      data.cos_profile = ac_info->cos_profile;
      data.in_lif_profile = ac_info->lif_profile;
      data.vsi_assignment_mode = (ac_info->vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) ? ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN:ac_info->vsi_assignment_mode;
      data.oam_lif_set = ac_info->oam_valid;
      data.use_in_lif = ac_info->use_lif;
      data.sys_in_lif = 0;

      
      arad_pp_l2_lif_convert_protection_fields_to_hw(unit, ac_info->protection_pointer, ac_info->protection_pass_value,
                                                     &data.protection_pointer, &data.protection_path);

      if (SOC_IS_JERICHO(unit) && ac_info->use_lif) {
          data.sys_in_lif = ac_info->global_lif;               
      }                                              

      
      data.tt_learn_enable = SOC_SAND_BOOL2NUM(ac_info->learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_DISABLE);
      if (ac_info->learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
      {
          
          
          res = arad_pp_fwd_decision_to_learn_buffer(
              unit,
              ac_info->learn_record.learn_info.type,
              ac_info->learn_record.learn_info.dest_id,
              SOC_PPC_LIF_ENTRY_TYPE_AC, 
              &data.learn_info,
              &additional_info
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
      }
      else{ 
          data.learn_info = 0;
      }

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
        uint8 urpf_mode;

        if (data.vsi < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)) { 
          rif_id = data.vsi;
        } else { 
          res =READ_IHP_HIGH_VSI_CONFIGr(unit, &reg32_val);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          rif_id = soc_reg_field_get(unit, IHP_HIGH_VSI_CONFIGr, reg32_val, HIGH_VSI_IN_RIFf);
        }

        
        res = arad_pp_sw_db_rif_to_lif_group_map_add_lif_to_rif(
           unit,
           rif_id,
           lif_index
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        
        res = sw_state_access[unit].dpp.soc.arad.pp.rif_to_lif_group_map.rif_urpf_mode.get(unit, rif_id, &urpf_mode);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 701, exit);

        res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 
		                                                                              (urpf_mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) ? 1 : 0, &data.in_lif_profile));
        SOC_SAND_CHECK_FUNC_RESULT(res, 702, exit);

      }

#endif 

      ARAD_DEVICE_CHECK(unit, exit);
      res = arad_pp_ihp_lif_table_ac_mp_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }

    ARAD_DEVICE_CHECK(unit, exit);
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_AC,
            ac_info->service_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3001, exit);

    
    if (SOC_IS_JERICHO(unit) && ac_info->is_extended)
    {
        uint64 additional_data;
        COMPILER_64_SET(additional_data,0,0);
           
        res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,additional_data,FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_add_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO                      *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_ADD_VERIFY);

  res = SOC_PPC_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = SOC_PPC_L2_LIF_AC_INFO_verify(unit, ac_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_add_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_ac_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                                *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                        *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8 ignore_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ignore_key = (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_NONE ) ? TRUE : FALSE;

  res = arad_pp_l2_lif_ac_get_internal_unsafe(
          unit,
          ac_key,
          ignore_key,
          lif_index,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                                *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO                        *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;
  SOC_PPC_LIF_ENTRY_TYPE
    entry_type;
  uint32
      sub_type, flags = 0;
  SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[3];
  uint32  nof_tables;
  uint8 hit_bit;
  uint32 profile_orientation = 0;
  uint8 is_duplicate = 0, i = 0;
  uint32 priority = 0;

  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(ac_key);
  }

  SOC_PPC_L2_LIF_AC_INFO_clear(ac_info);

  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

  if(!ignore_key)
  {
    res = arad_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (sem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD )
    {
      res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
              unit,
              ac_key->core_id,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

      *lif_index = vtt_port_config_tbl.default_port_lif;
      sem_entry.sem_result_ndx =  vtt_port_config_tbl.default_port_lif;
      *found = TRUE;
    }
    else
    {

        
        res = arad_pp_dbal_vtt_sw_db_get(unit, &sem_key, &nof_tables, dbal_tables_id, &is_duplicate);
        SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

        if(nof_tables == 0){
            res = arad_pp_isem_access_entry_get_unsafe(unit, &sem_key, &sem_entry, found);
            SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        }
        else 
        {
           res = arad_pp_l2_lif_ac_key_to_qualifier(unit, ac_key, qual_vals);
           SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

           for (i = 0; i < nof_tables; i++) {
               if (i > 0 && *found)
               {
                   
                   break;
               }

               res = arad_pp_dbal_entry_get(unit, dbal_tables_id[i], qual_vals, &sem_entry, &priority, &hit_bit, found);
               SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
           }
        }
    }
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (!*found)
  {
      goto exit;
  }

  *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

  
  res = arad_pp_sw_db_lif_table_entry_use_get(
          unit,
          *lif_index,
          &entry_type,
          &sub_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*found && entry_type == SOC_PPC_LIF_ENTRY_TYPE_EMPTY && 
      !ignore_key && sem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD)
  {
    *found = FALSE;
  }    
  
  if (*found)
  {
    switch (sub_type) {
    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
        {
          ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA
            data;

          res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
                  unit,
                  *lif_index,
                  &data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

          if (data.type == 0x0) 
          {
            ac_info->service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC;
            ac_info->cos_profile = data.cos_profile;
            ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
            ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
            ac_info->ing_edit_info.vid = data.vlan_edit_vid_1;
            ac_info->ing_edit_info.vid2 = data.vlan_edit_vid_2;
            if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)>=2)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                              (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                              &(ac_info->lif_profile), &profile_orientation));
                    SOC_SAND_CHECK_FUNC_RESULT(res,21,exit);
                }
                ac_info->orientation = data.orientation_is_hub;
                ac_info->network_group = (profile_orientation << 1) | ac_info->orientation;
            } else {
                ac_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
            }

            ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
            ac_info->lif_profile = data.in_lif_profile;
            ac_info->oam_valid = data.oam_lif_set;
            ac_info->use_lif = data.use_in_lif;

            ARAD_DEVICE_CHECK(unit, exit);

            if (SOC_IS_JERICHO(unit)) {
                ac_info->use_lif = (data.sys_in_lif) ? 1:0; 
            }
            ac_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
            ac_info->global_lif = data.sys_in_lif;

            
            arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                             &(ac_info->protection_pointer), &(ac_info->protection_pass_value));

            flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
            if (data.out_lif_valid)
            {
              flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
            }

            res = arad_pp_fwd_decision_in_buffer_parse(
                unit,                
                data.destination, 
                data.out_lif, 
                flags,
                &ac_info->default_frwrd.default_forwarding
                );
                SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
          }
          else{
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 4001, exit);
          }
          break;
        } 

        case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE:
        {
          ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA
            data;

          res = arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_get_unsafe(
                  unit,
                  *lif_index,
                  &data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

          if (data.type == 0x1) 
          {
            ac_info->service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE;
            ac_info->cos_profile = data.cos_profile;
            ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
            ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
            ac_info->ing_edit_info.vid = data.vlan_edit_vid_1;
            if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)>=2)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                            (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                            &(ac_info->lif_profile), &profile_orientation));
                    SOC_SAND_CHECK_FUNC_RESULT(res,21,exit);
                }
                ARAD_DEVICE_CHECK(unit, exit);
                ac_info->orientation = data.orientation_is_hub;
                ac_info->network_group = (profile_orientation << 1) | ac_info->orientation;
            } else {
                ac_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
            }            ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
            ac_info->lif_profile = data.in_lif_profile;

            ac_info->vsi_assignment_mode = (data.vsi_assignment_mode == ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN) ? SOC_PPC_VSI_EQ_IN_VID:data.vsi_assignment_mode;
            ac_info->oam_valid = data.oam_lif_set;
            ac_info->use_lif = data.use_in_lif;
            if (SOC_IS_JERICHO(unit)) {
                ac_info->use_lif = (data.sys_in_lif) ? 1:0; 
            }
            ac_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
            ac_info->global_lif = data.sys_in_lif;

            
            arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                             &(ac_info->protection_pointer), &(ac_info->protection_pass_value));

            flags = ARAD_PP_FWD_DECISION_PARSE_DEST;

            if (data.eei_or_out_lif_identifier) 
            {
                flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;
                if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                  
                  data.eei_or_out_lif |= ((data.eei_or_out_lif_identifier & 0xF) << 20);
                }
            } 
            else
            {
                            
              flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
              
              data.eei_or_out_lif &= (~(1<< (SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit))));
            }

            res = arad_pp_fwd_decision_in_buffer_parse(
                unit,                
                data.destination, 
                data.eei_or_out_lif, 
                flags, 
                &ac_info->default_frwrd.default_forwarding
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
          }
          else{
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 4001, exit);
          }
          break;
        } 

    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB:
        {
          ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA
            data;

          res = arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_get_unsafe(
                  unit,
                  *lif_index,
                  &data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

          if (data.type == 0x2) 
          {
            ac_info->service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB;
            ac_info->cos_profile = data.cos_profile;
            ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
            ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
            ac_info->ing_edit_info.vid = data.vlan_edit_vid_1;
            if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)>=2)) {
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                    res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                              (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                               &(ac_info->lif_profile), &profile_orientation));
                    SOC_SAND_CHECK_FUNC_RESULT(res,21,exit);
                }
                ac_info->orientation = data.orientation_is_hub;
                ac_info->network_group = (profile_orientation << 1) | ac_info->orientation;
            } else {
                ac_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
            }            ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
            ARAD_DEVICE_CHECK(unit, exit);
            ac_info->lif_profile = data.in_lif_profile;
            ac_info->vsi_assignment_mode = (data.vsi_assignment_mode == ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN) ? SOC_PPC_VSI_EQ_IN_VID:data.vsi_assignment_mode;
            ac_info->oam_valid = data.oam_lif_set;
            ac_info->use_lif = data.use_in_lif;
            if (SOC_IS_JERICHO(unit)) {
                ac_info->use_lif = (data.sys_in_lif) ? 1:0; 
            }
            ac_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
            ac_info->global_lif = data.sys_in_lif;

            
            arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                             &(ac_info->protection_pointer), &(ac_info->protection_pass_value));

            flags = ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_EEI_MIM ; 
            flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;

            res = arad_pp_fwd_decision_in_buffer_parse(
                unit,                
                data.destination, 
                data.eei,
                flags, 
                &ac_info->default_frwrd.default_forwarding
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
          }
          else{
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 4001, exit);
          }
          break;
        } 

    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP:
        {
          ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA
            data;

          res = arad_pp_ihp_lif_table_ac_mp_tbl_get_unsafe(
                  unit,
                  *lif_index,
                  &data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

          if (data.type == 0x3) 
          {
                ac_info->service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;
                ac_info->vsid = data.vsi;
                ac_info->default_forward_profile = data.da_not_found_profile;
                ac_info->ing_edit_info.vid = data.vlan_edit_vid_1;
                ac_info->ing_edit_info.vid2 = data.vlan_edit_vid_2;
                ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
                ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
                if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)>=2)) {
                    if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 3) {
                        res = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                                  (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                                   &(ac_info->lif_profile), &profile_orientation));
                        SOC_SAND_CHECK_FUNC_RESULT(res,21,exit);
                    }
                    ac_info->orientation = data.orientation_is_hub;
                    ac_info->network_group = (profile_orientation << 1) | ac_info->orientation;
                } else {
                    ac_info->orientation = (data.orientation_is_hub? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE);
                }
                ac_info->cos_profile = data.cos_profile;
                ac_info->lif_profile = data.in_lif_profile;
                ac_info->vsi_assignment_mode = (data.vsi_assignment_mode == ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN) ? SOC_PPC_VSI_EQ_IN_VID:data.vsi_assignment_mode;
                ac_info->oam_valid = data.oam_lif_set;
                ac_info->use_lif = data.use_in_lif;
                if (SOC_IS_JERICHO(unit))
                {
                    ac_info->use_lif = (data.sys_in_lif) ? 1:0;
                }
                ac_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
                ac_info->global_lif = data.sys_in_lif;
        
                ARAD_DEVICE_CHECK(unit, exit);
                
                arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                                 &(ac_info->protection_pointer), &(ac_info->protection_pass_value));

                if (data.tt_learn_enable == 0)
                {
                    ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
                }
                else if (data.learn_info == 0)
                {
                    ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
                }
                else
                {
                    ac_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
                }
            
                if (data.learn_info != 0 && ac_info->learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
                {
                    res = arad_pp_fwd_decision_from_learn_buffer(
                        unit,
                        data.learn_info,
                        SOC_PPC_LIF_ENTRY_TYPE_AC, 
                        &ac_info->learn_record.learn_info
                        );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
                }
                else{
                    if(ac_info->use_lif) {
                        ac_info->learn_record.learn_info.additional_info.outlif.val = *lif_index;
                        ac_info->learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
                        ac_info->learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
                    }

                }
              }
              else{
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 4001, exit);
              }
              break;
            } 
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_GET_VERIFY);

  res = SOC_PPC_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_ac_with_cos_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                  *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_with_cos_add_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_with_cos_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO                    *acs_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY);

  res = SOC_PPC_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(base_lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = SOC_PPC_L2_LIF_AC_GROUP_INFO_verify(unit, acs_group_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_with_cos_add_verify()", 0, 0);
}



uint32
  arad_pp_l2_lif_ac_with_cos_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT uint8                                     *found
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_with_cos_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_with_cos_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_WITH_COS_GET_VERIFY);

  res = SOC_PPC_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_with_cos_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_ac_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                                *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found_in_isem = FALSE, found_vd = FALSE;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;
  SOC_PPC_LIF_ID
    tmp_lif_index;
  SOC_PPC_LIF_ID
    removed_lif_index = 0;
  uint8 ignore_key;
  SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[3];
  uint32  nof_tables;
  uint8 hit_bit;
  uint8 is_duplicate = 0, i = 0;
  uint32 priority = 0;
  ARAD_TCAM_ACTION    action;
  uint8 use_cam = 0;

  SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
  SOC_SAND_SUCCESS_FAILURE  success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);

  ignore_key = (ac_key->key_type == SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_NONE);

  
  if (ignore_key) {
      SOC_SAND_CHECK_NULL_INPUT(lif_index);
      SOC_SAND_ERR_IF_ABOVE_NOF(*lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 5, exit);
  }
  
  if (!ignore_key) {

      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      res = arad_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (sem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD )
      {
        res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
                unit,
                ac_key->core_id,
                ac_key->vlan_domain,
                &vtt_port_config_tbl
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

        tmp_lif_index = vtt_port_config_tbl.default_port_lif;
        vtt_port_config_tbl.default_port_lif = 0;
        found_vd = TRUE;

        res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
                unit,
                ac_key->core_id,
                ac_key->vlan_domain,
                &vtt_port_config_tbl
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      } 
      else
      {
          
          res = arad_pp_dbal_vtt_sw_db_get(unit, &sem_key, &nof_tables, dbal_tables_id, &is_duplicate);
          SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

          if((dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM)
            ||(dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxETH_TCAM)
            ||(dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxINNER_VIDxETH_TCAM)){
                use_cam = 1;
          }

          ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

          if(nof_tables == 0){
              res = arad_pp_isem_access_entry_get_unsafe(unit, &sem_key, &sem_entry, &found_in_isem);
              SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
          }
          else 
          {
              res = arad_pp_l2_lif_ac_key_to_qualifier(unit, ac_key, qual_vals);
              SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
              for (i = 0; i < nof_tables; i++) {
                  if (i > 0 && found_in_isem)
                  {
                      
                      break;
                   }
                    if (use_cam) {
                        res = arad_pp_dbal_entry_get(unit, dbal_tables_id[i], qual_vals, action.value, &priority, &hit_bit, &found_in_isem);
                    } else
                     {
                        res = arad_pp_dbal_entry_get(unit, dbal_tables_id[i], qual_vals, &sem_entry, &priority, &hit_bit, &found_in_isem);
                    }
                   SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
             }
          }
        if (use_cam) {
            tmp_lif_index = action.value[0];
        }else
        {
            tmp_lif_index = sem_entry.sem_result_ndx;
        }
      }

      
      if ((found_in_isem || found_vd)) {
          removed_lif_index = arad_pp_sem_result_to_lif_index(unit, tmp_lif_index);
      }
  }

  
  if (ignore_key || found_in_isem || found_vd) {

      if (found_in_isem)
      {
          if(nof_tables == 0){
              res = arad_pp_isem_access_entry_remove_unsafe( unit, &sem_key);
              SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
          }
          else{
              for (i = 0; i < nof_tables; i++) {
                  
                  res = arad_pp_dbal_entry_delete(unit, dbal_tables_id[i], qual_vals, &success);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
              }
          }
      }

  }
   
  if (lif_index != NULL) {
      
      if (*lif_index == 0 ) {
          *lif_index = removed_lif_index;
      }
#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS_A0(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {

          
          SOC_PPC_LIF_ENTRY_TYPE type;
          uint32 sub_type;
 
          res = arad_pp_sw_db_lif_table_entry_use_get(unit, *lif_index, &type, &sub_type);
          SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

          if (sub_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP) {
              res = arad_pp_sw_db_rif_to_lif_group_map_remove_lif_from_rif(unit, *lif_index);
              SOC_SAND_CHECK_FUNC_RESULT(res, 171, exit);
          }
      }
#endif 

      
      res = arad_pp_sw_db_lif_table_entry_use_set(
                unit,
                *lif_index,
                SOC_PPC_LIF_ENTRY_TYPE_EMPTY,
                0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      *lif_index = removed_lif_index;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_ac_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                           *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_AC_REMOVE_VERIFY);

  res = SOC_PPC_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_remove_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index;
  soc_reg_above_64_val_t  
    reg_above64_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index = SOC_PPC_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile, l2cp_key->da_mac_address_lsb);

  

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above64_val));
  res = soc_sand_bitstream_set(reg_above64_val, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_PEER));

  

  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, SOC_CORE_ALL, reg_above64_val));

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above64_val));
  res = soc_sand_bitstream_set(reg_above64_val, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_DROP));

  

  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, SOC_CORE_ALL, reg_above64_val));

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1040, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above64_val));
  res = soc_sand_bitstream_set(reg_above64_val, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL));

  

  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1050, exit, WRITE_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above64_val));

  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_l2cp_trap_set_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(handle_type, ARAD_PP_LIF_HANDLE_TYPE_MAX, ARAD_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_l2cp_trap_set_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_l2cp_trap_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE                 *handle_type
  )
{
  uint32
    index;
  soc_reg_above_64_val_t  
    reg_above64_val;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  *handle_type = SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_NORMAL;
  index = SOC_PPC_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile, l2cp_key->da_mac_address_lsb);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1150, exit, READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above64_val));
  *handle_type = soc_sand_bitstream_test_bit(reg_above64_val, index) ? SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_PEER : *handle_type;

  

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1160, exit, READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above64_val));
  *handle_type = soc_sand_bitstream_test_bit(reg_above64_val, index) ? SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_DROP : *handle_type;

  

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1170, exit, READ_IHP_MEF_L_2_CP_TRANSPARENT_BITMAPr(unit, REG_PORT_ANY, reg_above64_val));
  *handle_type = soc_sand_bitstream_test_bit(reg_above64_val, index) ? SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL : *handle_type;

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_l2cp_trap_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_isid_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                         vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                    *isid_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                   *isid_info,
    SOC_SAND_IN  uint8                                      is_ingress,
    SOC_SAND_IN  uint8                                      is_egress,
    SOC_SAND_IN  uint8                                      isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  )
{
  uint32
    eei_or_out_lif[1],
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA 
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_ADD_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(isid_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key && is_ingress) {
      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB;
      sem_key.key_info.pbb.bsa_nickname_valid = isid_key->bsa_nickname_valid;
      sem_key.key_info.pbb.bsa_nickname = isid_key->bsa_nickname;
      sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
      sem_key.key_info.pbb.isid = isid_key->isid_id;

      ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
      res =  arad_pp_lif_index_to_sem_result(unit, lif_index, isid_info->ext_lif_id, &sem_entry.sem_result_ndx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_pp_isem_access_entry_add_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      if(*success != SOC_SAND_SUCCESS)
      {
          goto exit;
      }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  if (!isem_only) {
      if (is_ingress && (isid_info->service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P))
      {
          ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA
            data;

          res = arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe(
                  unit,
                  lif_index,
                  &data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

          data.type = 0x0; 
          data.cos_profile = isid_info->cos_profile;
          data.tpid_profile = isid_info->tpid_profile_index;
          data.vsi = vsi_ndx;
          data.destination_valid = 0x1;
          data.oam_lif_set = isid_info->oam_valid; 
          data.orientation_is_hub = isid_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
          data.service_type_lsb =  0x0;
          data.use_in_lif = isid_info->use_lif;
          data.vsi_assignment_mode = isid_info->vsi_assignment_mode;
          data.sys_in_lif = 0;

          
          arad_pp_l2_lif_convert_protection_fields_to_hw(unit, isid_info->protection_pointer, isid_info->protection_pass_value,
                                                         &data.protection_pointer, &data.protection_path);

          if (SOC_IS_JERICHO(unit)) {
              if (isid_info->use_lif) {
                  data.sys_in_lif = isid_info->global_lif;
              }
              data.type = 0x0; 
              data.in_lif_profile = isid_info->lif_profile;
          }

          res = arad_pp_fwd_decision_in_buffer_build(
              unit, 
              ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, 
              &isid_info->default_frwrd.default_forwarding, 
              &data.destination, 
              eei_or_out_lif
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

          if (isid_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY)
          {
              data.eei_or_out_lif_type = 0; 
                      
          }
          else 
          {
              data.eei_or_out_lif_type = 1;  
              
              
              res = soc_sand_bitstream_set_bit(eei_or_out_lif, SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit)); 
              SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);     
          }

          
          res = soc_sand_bitstream_get_any_field(eei_or_out_lif, 0, 2, &data.eei_or_out_lif_lsb);
          SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
          res = soc_sand_bitstream_get_any_field(eei_or_out_lif, 2, 22, &data.eei_or_out_lif);
          SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

          res = arad_pp_ihp_lif_table_isid_p2p_tbl_set_unsafe(
                  unit,
                  lif_index,
                  &data
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
        }

        if (isid_info->service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP)
        {
            if (is_ingress) {
              ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA
                data;

              res = arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe(
                      unit,
                      lif_index,
                      &data
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

              data.type = 0x1; 
              data.service_type_lsb = 0x0;
              data.destination_valid = 0x0; 
              data.cos_profile = isid_info->cos_profile;
              data.tpid_profile = isid_info->tpid_profile_index;
              data.tt_learn_enable = isid_info->learn_enable;
              data.vsi = vsi_ndx;
              data.da_not_found_profile = isid_info->default_forward_profile;
              data.oam_lif_set = isid_info->oam_valid;
              data.orientation_is_hub = isid_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB ? 1 : 0;
              data.service_type_lsb = 0;
              data.use_in_lif = isid_info->use_lif;
              data.vsi_assignment_mode = isid_info->vsi_assignment_mode; 
              data.sys_in_lif = 0;

              
              arad_pp_l2_lif_convert_protection_fields_to_hw(unit, isid_info->protection_pointer, isid_info->protection_pass_value,
                                                             &data.protection_pointer, &data.protection_path);

              if (SOC_IS_JERICHO(unit)) {
                  if (isid_info->use_lif) {
                      data.sys_in_lif = isid_info->global_lif;
                  }
                  data.type = 0x3; 
                  data.in_lif_profile = isid_info->lif_profile;
              }

              res = arad_pp_ihp_lif_table_isid_mp_tbl_set_unsafe(
                      unit,
                      lif_index,
                      &data
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

            }
          
          if (!ignore_key && !(SOC_DPP_CONFIG(unit)->pp.mim_vsi_mode) && is_egress)
          {
              res = arad_pp_epni_isid_table_tbl_get_unsafe(
                     unit,
                     vsi_ndx,
                     &isid_tbl_data
                   );
              SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

              isid_tbl_data.isid = isid_key->isid_id;

              res = arad_pp_epni_isid_table_tbl_set_unsafe(
                     unit,
                     vsi_ndx,
                     &isid_tbl_data
                   );
              SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
          }

            
            if (SOC_IS_JERICHO(unit) && isid_info->is_extended && is_ingress)
            {
                uint64 additional_data;
                COMPILER_64_SET(additional_data,0,0);
                   
                res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,additional_data,FALSE);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
            }
      }

      
      if ((*success == SOC_SAND_SUCCESS) && is_ingress)
      {
        res = arad_pp_sw_db_lif_table_entry_use_set(
                unit,
                lif_index,
                SOC_PPC_LIF_ENTRY_TYPE_ISID,
                isid_info->service_type
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_add_internal_unsafe()", vsi_ndx, 0);
}


uint32
  arad_pp_l2_lif_isid_add_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                        *success
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_isid_add_internal_unsafe(
    unit,
    vsi_ndx,
    isid_key,
    FALSE,
    lif_index,
    isid_info,
    is_ingress,
    is_egress,
    isem_only,
    success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_add_unsafe()", vsi_ndx, 0);
}

uint32
  arad_pp_l2_lif_isid_add_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO                    *isid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_SYS_VSI_ID_MAX, SOC_PPC_SYS_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_ISID_KEY, isid_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 30, exit);

  res = SOC_PPC_L2_LIF_ISID_INFO_verify(unit, isid_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_add_verify()", vsi_ndx, 0);
}


uint32
  arad_pp_l2_lif_isid_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                    *isid_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                         *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                   *isid_info,
    SOC_SAND_OUT uint8                                      *found
  )
{
    uint32
  eei_or_out_lif = 0,
    parse_flags=0,
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(isid_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_L2_LIF_ISID_INFO_clear(isid_info);
  *vsi_index = 0;

  if (!ignore_key)
  {
    *lif_index = 0;
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB;
    sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
    sem_key.key_info.pbb.isid = isid_key->isid_id;
    sem_key.key_info.pbb.bsa_nickname = isid_key->bsa_nickname;
    sem_key.key_info.pbb.bsa_nickname_valid = isid_key->bsa_nickname_valid;
  
    res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          found
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx =  *lif_index;
  }

  if (*found)
  {
    *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

    do
    {
      ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA
        data;

      res = arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

      if (data.type == 0x0) 
      {
          isid_info->service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P;
          isid_info->cos_profile = data.cos_profile;
          isid_info->tpid_profile_index = data.tpid_profile;
          isid_info->oam_valid = data.oam_lif_set;
          isid_info->orientation = data.orientation_is_hub ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
          isid_info->use_lif = data.use_in_lif;

          
          arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                           &(isid_info->protection_pointer), &(isid_info->protection_pass_value));

          if (SOC_IS_JERICHO(unit)) {
              isid_info->use_lif = (data.sys_in_lif) ? 1:0;
              isid_info->lif_profile = data.in_lif_profile;
          }
          isid_info->vsi_assignment_mode = data.vsi_assignment_mode;
          isid_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

          
          res = soc_sand_bitstream_set_any_field(&data.eei_or_out_lif_lsb, 0, 2, &eei_or_out_lif);
          SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
          res = soc_sand_bitstream_set_any_field(&data.eei_or_out_lif, 2, 22, &eei_or_out_lif);
          SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

		  
		  if (data.eei_or_out_lif_type == 1) { 
			  eei_or_out_lif &= (~(1<< (SOC_DPP_DEFS_GET(unit, inlif_p2p_out_lif_valid_bit))));
			  parse_flags = ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
		  }
		  else{
			  parse_flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
		  }

          res = arad_pp_fwd_decision_in_buffer_parse(
              unit,               
              data.destination, 
              eei_or_out_lif,
              parse_flags|ARAD_PP_FWD_DECISION_PARSE_DEST, 
              &isid_info->default_frwrd.default_forwarding
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

          *vsi_index = data.vsi;
        }
    } while(0);

    do
    {
      ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA
        data;

      res = arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      if (((data.type == 0x1) && SOC_IS_ARADPLUS_AND_BELOW(unit)) || ((data.type == 0x3) && SOC_IS_JERICHO(unit)) ) 
      {
          isid_info->service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP;
          isid_info->cos_profile = data.cos_profile;
          isid_info->tpid_profile_index = data.tpid_profile;
          isid_info->learn_enable = data.tt_learn_enable;
          isid_info->default_forward_profile = data.da_not_found_profile;
          isid_info->oam_valid = data.oam_lif_set;
          isid_info->orientation = data.orientation_is_hub ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
          isid_info->use_lif = data.use_in_lif;

          
          arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                           &(isid_info->protection_pointer), &(isid_info->protection_pass_value));

          if (SOC_IS_JERICHO(unit)) {
              isid_info->use_lif = (data.sys_in_lif) ? 1:0;
              isid_info->lif_profile = data.in_lif_profile;
          }

          isid_info->vsi_assignment_mode = data.vsi_assignment_mode;
      
          *vsi_index = data.vsi; 
      }
    } while(0);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_isid_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_isid_get_internal_unsafe(
    unit,
    isid_key,
    FALSE,
    lif_index,
    vsi_index,
    isid_info,
    found
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_isid_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                       *isid_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_ISID_KEY, isid_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_vsi_to_isid_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                                  *isid_id
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA 
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VSI_TO_ISID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_id);

  res = arad_pp_epni_isid_table_tbl_get_unsafe(
      unit,
      vsi_ndx,
      &isid_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  *isid_id = isid_tbl_data.isid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vsi_to_isid_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                              vsi_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VSI_TO_ISID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, ARAD_PP_SYS_VSI_ID_MAX, SOC_PPC_SYS_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_isid_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  uint8                                       is_ingress,
    SOC_SAND_IN  uint8                                       is_egress,
    SOC_SAND_IN  SOC_PPC_SYS_VSI_ID                          vsi,
    SOC_SAND_IN  uint8                                       isem_only,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    vsi_ndx = vsi,
    res = SOC_SAND_OK;
  uint32 type, isid_mp_lif_type;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found = 0;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA 
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  
  isid_mp_lif_type = ((SOC_IS_JERICHO(unit)) ? 0x3 : 0x1);

  if (is_ingress) {
      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB;
      sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
      sem_key.key_info.pbb.isid = isid_key->isid_id;
      sem_key.key_info.pbb.bsa_nickname_valid = isid_key->bsa_nickname_valid;
      sem_key.key_info.pbb.bsa_nickname = isid_key->bsa_nickname;
      ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
      res = arad_pp_isem_access_entry_get_unsafe(
              unit,
              &sem_key,
              &sem_entry,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      if (found)
      {
        *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

        res = arad_pp_isem_access_entry_remove_unsafe(
          unit,
          &sem_key
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        if (!(isem_only)) {
            
            do
            {
              ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA
                data;

              
              res = arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe(  
                unit,
                *lif_index,
                &data
                );
              SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
              type = data.type;
              if (type == 0x0) {  
                  vsi_ndx = data.vsi;

                  
                  sal_memset(&data, 0, sizeof(ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA));  

                  res = arad_pp_ihp_lif_table_isid_p2p_tbl_set_unsafe(  
                      unit,
                      *lif_index,
                      &data
                      );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
              } 
            }while(0);
            if (type == isid_mp_lif_type) { 
                do
                {
                  ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA
                    data;

                  
                  res = arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe(  
                    unit,
                    *lif_index,
                    &data
                  );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

                  vsi_ndx = data.vsi;

                  
                  sal_memset(&data, 0, sizeof(ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA));

                  res = arad_pp_ihp_lif_table_isid_mp_tbl_set_unsafe(  
                    unit,
                    *lif_index,
                    &data
                  );
                  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
                } while(0);
            }
        }
      }
  }

  
  if (is_egress) {
      isid_tbl_data.isid = 0;
      res = arad_pp_epni_isid_table_tbl_set_unsafe(
                 unit,
                 vsi_ndx,
                 &isid_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_isid_remove_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY                     *isid_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_ISID_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_ISID_KEY, isid_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_remove_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_gre_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                    *gre_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                   *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA 
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_ADD_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(gre_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key) {
      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE;
      sem_key.key_info.l2_gre.gre_key = gre_key->vpn_key;
      sem_key.key_info.l2_gre.match_port_class = gre_key->match_port_class;

      ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
      sem_entry.sem_result_ndx = lif_index;

      res = arad_pp_isem_access_entry_add_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      if(*success != SOC_SAND_SUCCESS)
      {
          goto exit;
      }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  
  isid_tbl_data.isid = gre_key->vpn_key;
  res = arad_pp_epni_isid_table_tbl_set_unsafe(
         unit,
         gre_info->vsid & 0xFFF, 
         &isid_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_add_internal_unsafe()", lif_index, 0);
}


uint32
  arad_pp_l2_lif_gre_add_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                        *success
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(gre_key);
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_gre_add_internal_unsafe(
    unit,
    gre_key,
    FALSE,
    lif_index,
    gre_info,
    success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_add_unsafe()", lif_index, 0);
}

uint32
  arad_pp_l2_lif_gre_add_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO                    *gre_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_ADD_VERIFY);

  if(lif_index != gre_info->vsid) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
  }
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_GRE_KEY, gre_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 30, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_GRE_INFO, gre_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_add_verify()", lif_index, 0);
}

uint32
  arad_pp_l2_lif_gre_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                    *gre_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                   *gre_info,
    SOC_SAND_OUT uint8                                      *found
  )
{
    uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(gre_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_L2_LIF_GRE_INFO_clear(gre_info);

  if (!ignore_key)
  {
    *lif_index = 0;
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE;
    sem_key.key_info.l2_gre.gre_key = gre_key->vpn_key;
    sem_key.key_info.l2_gre.match_port_class = gre_key->match_port_class;

    res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          found
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx =  *lif_index;
  }

  if (*found)
  {
    gre_info->vsid = *lif_index;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_gre_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO                    *gre_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(gre_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(gre_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_gre_get_internal_unsafe(
    unit,
    gre_key,
    FALSE,
    lif_index,
    gre_info,
    found
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_gre_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                       *gre_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_GRE_KEY, gre_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_gre_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(gre_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE;
  sem_key.key_info.l2_gre.gre_key = gre_key->vpn_key;
  sem_key.key_info.l2_gre.match_port_class = gre_key->match_port_class;

  ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
  res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (found)
  {
    *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

    res = arad_pp_isem_access_entry_remove_unsafe(
      unit,
      &sem_key
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_gre_remove_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY                     *gre_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_GRE_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_GRE_KEY, gre_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_gre_remove_verify()", 0, 0);
}

uint32
  arad_pp_l2_lif_vxlan_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                    *vxlan_key,
    SOC_SAND_IN  uint8                                      ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                             lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                   *vxlan_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                   *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA 
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_ADD_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key) {
      ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
      sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI;
      sem_key.key_info.l2_vni.vni_key = vxlan_key->vpn_key;
      sem_key.key_info.l2_vni.match_port_class = vxlan_key->match_port_class;

      ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
      sem_entry.sem_result_ndx = lif_index; 

      res = arad_pp_isem_access_entry_add_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      if(*success != SOC_SAND_SUCCESS)
      {
          goto exit;
      }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  
  isid_tbl_data.isid = vxlan_key->vpn_key;
  if (SOC_IS_JERICHO(unit)) {
      res = arad_pp_epni_isid_table_tbl_set_unsafe(
             unit,
             vxlan_info->vsid, 
             &isid_tbl_data
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  } else {
      res = arad_pp_epni_isid_table_tbl_set_unsafe(
             unit,
             vxlan_info->vsid & 0xFFF, 
             &isid_tbl_data
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_add_internal_unsafe()", lif_index, 0);
}


uint32
  arad_pp_l2_lif_vxlan_add_unsafe(

    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_IN  int                                         ignore_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                        *success
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_l2_lif_vxlan_add_internal_unsafe(
    unit,
    vxlan_key,
    ignore_key,
    lif_index,
    vxlan_info,
    success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_add_unsafe()", lif_index, 0);
}

uint32
  arad_pp_l2_lif_vxlan_add_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_IN  int                                         ignore_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_ADD_VERIFY);

  if(lif_index != vxlan_info->vsid) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
  }
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_VXLAN_KEY, vxlan_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 30, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_VXLAN_INFO, vxlan_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_add_verify()", lif_index, 0);
}

uint32
  arad_pp_l2_lif_vxlan_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                    *vxlan_key,
    SOC_SAND_IN  uint8                                      ignore_key, 
    SOC_SAND_OUT SOC_PPC_LIF_ID                             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                   *vxlan_info,
    SOC_SAND_OUT uint8                                      *found
  )
{
    uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPC_L2_LIF_VXLAN_INFO_clear(vxlan_info);

  if (!ignore_key)
  {
    *lif_index = 0;
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);

    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI;
    sem_key.key_info.l2_vni.vni_key = vxlan_key->vpn_key;
    sem_key.key_info.l2_vni.match_port_class = vxlan_key->match_port_class;

    res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          found
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (*found)
  {
    vxlan_info->vsid = *lif_index;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_get_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_vxlan_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO                    *vxlan_info,
    SOC_SAND_OUT uint8                                   *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vxlan_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_l2_lif_vxlan_get_internal_unsafe(
    unit,
    vxlan_key,
    FALSE,
    lif_index,
    vxlan_info,
    found
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_get_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vxlan_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                       *vxlan_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_VXLAN_KEY, vxlan_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_get_verify()", 0, 0);
}


uint32
  arad_pp_l2_lif_vxlan_remove_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vxlan_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI;
  sem_key.key_info.l2_vni.vni_key = vxlan_key->vpn_key;
  sem_key.key_info.l2_vni.match_port_class = vxlan_key->match_port_class;

  ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
  res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (found)
  {
    *lif_index = arad_pp_sem_result_to_lif_index (unit, sem_entry.sem_result_ndx);

    res = arad_pp_isem_access_entry_remove_unsafe(
      unit,
      &sem_key
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_vxlan_remove_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY                     *vxlan_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_VXLAN_REMOVE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_VXLAN_KEY, vxlan_key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_vxlan_remove_verify()", 0, 0);
}



uint32
  arad_pp_l2_lif_default_frwrd_info_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  )
{
  uint32
    res = SOC_SAND_OK,
    index,
    data_unknown,
    destination_enc,
    eei_enc,
    add_base;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_UNSAFE);

  if (action_info->add_vsi) 
  {
    
    destination_enc = action_info->offset;
  }
  else
  {
    
    res = arad_pp_fwd_decision_in_buffer_build(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
            &(action_info->frwrd_dest),
            &destination_enc,
            &eei_enc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  index = SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(dflt_frwrd_key->port_da_not_found_profile, dflt_frwrd_key->da_type, dflt_frwrd_key->lif_default_forward_profile);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_UNKNOWN_DA_MAPm(unit, MEM_BLOCK_ANY, index, &data_unknown));
  soc_IHP_UNKNOWN_DA_MAPm_field32_set(unit, &data_unknown, DESTINATIONf, destination_enc);

  add_base = SOC_SAND_BOOL2NUM(action_info->add_vsi);
  soc_IHP_UNKNOWN_DA_MAPm_field32_set(unit, &data_unknown, ADD_VSI_DESTf, add_base);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IHP_UNKNOWN_DA_MAPm(unit, MEM_BLOCK_ANY, index, &data_unknown));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_default_frwrd_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_default_frwrd_info_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY, dflt_frwrd_key, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION, action_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_default_frwrd_info_set_verify()",0,0);
}

uint32
  arad_pp_l2_lif_default_frwrd_info_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_UNSAFE);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY, dflt_frwrd_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_default_frwrd_info_get_verify()",0,0);
}

uint32
  arad_pp_l2_lif_default_frwrd_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY        *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION     *action_info
  )
{
  uint32
    res = SOC_SAND_OK,
    destination_enc,
    asd = 0,
    data_unknown,
    index,
    add_base;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_L2_LIF_DEFAULT_FRWRD_INFO_GET_UNSAFE);

  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_clear(action_info);

  index = SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_ENTRY_OFFSET(dflt_frwrd_key->port_da_not_found_profile, dflt_frwrd_key->da_type, dflt_frwrd_key->lif_default_forward_profile);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_UNKNOWN_DA_MAPm(unit, MEM_BLOCK_ANY, index, &data_unknown));

  add_base = soc_IHP_UNKNOWN_DA_MAPm_field32_get(unit, &data_unknown, ADD_VSI_DESTf);
  action_info->add_vsi = SOC_SAND_NUM2BOOL(add_base);

  destination_enc = soc_IHP_UNKNOWN_DA_MAPm_field32_get(unit, &data_unknown, DESTINATIONf);

  if (action_info->add_vsi) 
  {
    
    action_info->offset = destination_enc;
  }
  else
  {
    
    res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          destination_enc,
          asd,
          ARAD_PP_FWD_DECISION_PARSE_DEST,
          &action_info->frwrd_dest
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_default_frwrd_info_get_unsafe()", 0, 0);
}



uint32 arad_pp_l2_lif_extender_fill_lif_data(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO *extender_info,
    SOC_SAND_OUT void** in_lif_data)
{
    uint32 res, additional_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (extender_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC)
    {
        ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA *in_lif_ac2ac_data;
        in_lif_ac2ac_data = sal_alloc(sizeof(*in_lif_ac2ac_data), "Arad AC2AC In-LIF data allocation");
        *in_lif_data = in_lif_ac2ac_data;
        sal_memset(in_lif_ac2ac_data, 0, sizeof(*in_lif_ac2ac_data));  

        in_lif_ac2ac_data->type = 0x0;   
        in_lif_ac2ac_data->cos_profile = extender_info->cos_profile;
        in_lif_ac2ac_data->in_lif_profile = extender_info->lif_profile;
        in_lif_ac2ac_data->oam_lif_set = extender_info->oam_valid;
        in_lif_ac2ac_data->use_in_lif = extender_info->use_lif;
        in_lif_ac2ac_data->destination_valid = 1;
        in_lif_ac2ac_data->out_lif = 0;
        in_lif_ac2ac_data->out_lif_valid = 1;    
        in_lif_ac2ac_data->sys_in_lif = 0;

        
        arad_pp_l2_lif_convert_protection_fields_to_hw(unit, extender_info->protection_pointer, extender_info->protection_pass_value,
                                                       &(in_lif_ac2ac_data->protection_pointer), &(in_lif_ac2ac_data->protection_path));

        
        if (SOC_IS_JERICHO(unit) && extender_info->use_lif) {
            in_lif_ac2ac_data->sys_in_lif = extender_info->global_lif;
        }

        
        res = arad_pp_fwd_decision_in_buffer_build(unit, ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, 
                                                   &(extender_info->default_frwrd.default_forwarding), 
                                                   &(in_lif_ac2ac_data->destination), &(in_lif_ac2ac_data->out_lif));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (extender_info->default_frwrd.default_forwarding.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 20, exit);    
        }
    }
    else if (extender_info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP) {
        ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA *in_lif_ac_mp_data;
        in_lif_ac_mp_data = sal_alloc(sizeof(*in_lif_ac_mp_data), "Arad AC-MP In-LIF data allocation");
        *in_lif_data = in_lif_ac_mp_data;
        sal_memset(in_lif_ac_mp_data, 0, sizeof(*in_lif_ac_mp_data));

        in_lif_ac_mp_data->type = 0x3;    
        in_lif_ac_mp_data->vsi = extender_info->vsid;
        in_lif_ac_mp_data->da_not_found_profile = extender_info->default_forward_profile;
        in_lif_ac_mp_data->cos_profile = extender_info->cos_profile;
        in_lif_ac_mp_data->in_lif_profile = extender_info->lif_profile;
        in_lif_ac_mp_data->vsi_assignment_mode = (extender_info->vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) ?
            ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN : extender_info->vsi_assignment_mode;
        in_lif_ac_mp_data->oam_lif_set = extender_info->oam_valid;
        in_lif_ac_mp_data->use_in_lif = extender_info->use_lif;
        in_lif_ac_mp_data->sys_in_lif = 0;

        
        arad_pp_l2_lif_convert_protection_fields_to_hw(unit, extender_info->protection_pointer, extender_info->protection_pass_value,
                                                       &(in_lif_ac_mp_data->protection_pointer), &(in_lif_ac_mp_data->protection_path));

        
        if (SOC_IS_JERICHO(unit) && extender_info->use_lif) {
            in_lif_ac_mp_data->sys_in_lif = extender_info->global_lif;
        }

        
        in_lif_ac_mp_data->tt_learn_enable = SOC_SAND_BOOL2NUM(extender_info->learn_record.learn_type != SOC_PPC_L2_LIF_AC_LEARN_DISABLE);
        if (extender_info->learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
        {
            
            res = arad_pp_fwd_decision_to_learn_buffer(unit, extender_info->learn_record.learn_info.type,
                                                       extender_info->learn_record.learn_info.dest_id, SOC_PPC_LIF_ENTRY_TYPE_EXTENDER,
                                                       &(in_lif_ac_mp_data->learn_info), &additional_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        } else { 
            in_lif_ac_mp_data->learn_info = 0;
        }
    }

exit:
    if (ex != no_err) {
        sal_free(*in_lif_data);
    }
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_extender_fill_lif_data()", 0, 0);
}


uint32 arad_pp_l2_lif_extender_lif_data_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_SERVICE_TYPE service_type,
    SOC_SAND_IN  SOC_PPC_LIF_ID in_lif_idx,
    SOC_SAND_IN  void* in_lif_data)
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC) {

        
        res = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe(
                unit, in_lif_idx, in_lif_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if (service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP) {

        
        res = arad_pp_ihp_lif_table_ac_mp_tbl_set_unsafe(
                unit, in_lif_idx, in_lif_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_extender_lif_data_set()", 0, 0);
}


uint32 arad_pp_l2_lif_extender_key_to_qualifier(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY    *extender_key,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL            qual_vals[])
{
    uint32 qual_index;

    
    for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; ++qual_index) {
        SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[qual_index]);
    }

    
    qual_vals[0].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    qual_vals[0].val.arr[0] = extender_key->name_space;

    qual_vals[1].type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID;
    qual_vals[1].val.arr[0] = extender_key->extender_port_vid;

    if (extender_key->is_tagged) {
        qual_vals[2].type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_vals[3].type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        qual_vals[3].val.arr[0] = 1;
    } else {
        qual_vals[2].type = SOC_PPC_FP_QUAL_INITIAL_VID;
        qual_vals[3].type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_vals[3].val.arr[0] = 0;
    }
    qual_vals[2].val.arr[0] = extender_key->vid;

    return SOC_SAND_OK;
}

uint32
arad_pp_l2_lif_extender_dbal_entry_get(int unit, 
                           SOC_SAND_IN SOC_PPC_L2_LIF_EXTENDER_KEY  *extender_key,
                           SOC_DPP_DBAL_SW_TABLE_IDS                dbal_table_ids[], 
                           SOC_PPC_FP_QUAL_VAL                      qual_vals[])
{
    int rv;
    ARAD_PP_ISEM_ACCESS_KEY sem_key;
    uint32  nof_dbal_tables;
    uint8 is_duplicate = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_PE;

    rv = arad_pp_dbal_vtt_sw_db_get(unit, &sem_key, &nof_dbal_tables, dbal_table_ids, &is_duplicate);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = arad_pp_l2_lif_extender_key_to_qualifier(unit, extender_key, qual_vals);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_l2_lif_extender_get(
    int                                    unit,
    SOC_PPC_L2_LIF_EXTENDER_KEY            *extender_key,
    SOC_PPC_LIF_ID                         *lif_index,
    SOC_PPC_L2_LIF_EXTENDER_INFO           *extender_info,
    uint8                                  *found
  )
{
    int rv;
    uint32 flags, lif_sub_type;
    SOC_PPC_LIF_ENTRY_TYPE lif_type;
    ARAD_PP_ISEM_ACCESS_ENTRY sem_entry;
    SOC_DPP_DBAL_SW_TABLE_IDS dbal_table_ids[ARAD_PP_ISEM_ACCESS_NOF_TABLES];
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX];
    uint8 hit_bit;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(found);
    SOCDNX_NULL_CHECK(lif_index);
    SOCDNX_NULL_CHECK(extender_info);

    if (extender_key) {
        
        rv = arad_pp_l2_lif_extender_dbal_entry_get(unit, extender_key, dbal_table_ids, qual_vals);
        SOCDNX_IF_ERR_EXIT(rv);

        if (extender_key->is_tagged) {
            
            
            rv = arad_pp_dbal_entry_get(unit, dbal_table_ids[0], qual_vals, &sem_entry, 0, &hit_bit, found);
        } else {
            rv = arad_pp_dbal_entry_get(unit, dbal_table_ids[1], qual_vals, &sem_entry, 0, &hit_bit, found);
        }
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        *found = TRUE;
        sem_entry.sem_result_ndx = *lif_index;
    }

    if (!*found)
    {
        goto exit;
    }

    *lif_index = arad_pp_sem_result_to_lif_index(unit, sem_entry.sem_result_ndx);

    
    rv = arad_pp_sw_db_lif_table_entry_use_get(
        unit,
        *lif_index,
        &lif_type,
        &lif_sub_type);
    SOCDNX_IF_ERR_EXIT(rv);

    switch (lif_sub_type) {
    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
        {
            ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA
              data;
            
            rv = arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
                    unit,
                    *lif_index,
                    &data
                  );
            SOCDNX_SAND_IF_ERR_EXIT(rv);
            
            if (data.type == 0x0) 
            {
                extender_info->cos_profile = data.cos_profile;
                extender_info->lif_profile = data.in_lif_profile;
                extender_info->oam_valid   = data.oam_lif_set;
                extender_info->use_lif     = data.use_in_lif;
                
                if (SOC_IS_JERICHO(unit)) {
                    extender_info->use_lif      = (data.sys_in_lif) ? 1:0; 
                }
                extender_info->global_lif  = data.sys_in_lif;
                
                arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                     &(extender_info->protection_pointer), &(extender_info->protection_pass_value));
                
                
                flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
                if (data.out_lif_valid)
                {
                  flags |= ARAD_PP_FWD_DECISION_PARSE_OUTLIF;
                }
                
                extender_info->default_frwrd.default_frwd_type = ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P;
                rv = arad_pp_fwd_decision_in_buffer_parse(
                    unit,                    
                    data.destination, 
                    data.out_lif, 
                    flags,
                    &extender_info->default_frwrd.default_forwarding
                    );
                SOCDNX_SAND_IF_ERR_EXIT(rv);
            }
            else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Mismatch between lif SW DB and lif entry.")));
            }
            break;
        } 

    case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP:
        {
            ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA
              data;
            
            rv = arad_pp_ihp_lif_table_ac_mp_tbl_get_unsafe(
                    unit,
                    *lif_index,
                    &data
                  );
            SOCDNX_SAND_IF_ERR_EXIT(rv);
            
            if (data.type == 0x3) 
            {
                extender_info->vsid = data.vsi;
                extender_info->default_forward_profile = data.da_not_found_profile;
                extender_info->cos_profile = data.cos_profile;
                extender_info->lif_profile = data.in_lif_profile;
                extender_info->vsi_assignment_mode = (data.vsi_assignment_mode == ARAD_PP_L2_LIF_VSI_ASSIGNMENT_MODE_VSI_EQ_VLAN) ? 
                                                        SOC_PPC_VSI_EQ_IN_VID:data.vsi_assignment_mode;
                extender_info->oam_valid = data.oam_lif_set;
                extender_info->use_lif = data.use_in_lif;

                if (SOC_IS_JERICHO(unit))
                {
                    extender_info->use_lif = (data.sys_in_lif) ? 1:0;
                }

                extender_info->default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
                extender_info->global_lif = data.sys_in_lif;
            
                
                arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                                 &(extender_info->protection_pointer), &(extender_info->protection_pass_value));
            
                if (data.tt_learn_enable == 0)
                {
                    extender_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_DISABLE;
                }
                else if (data.learn_info == 0)
                {
                    extender_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
                }
                else
                {
                    extender_info->learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
                }
              
                if (data.learn_info != 0 && extender_info->learn_record.learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
                {
                    rv = arad_pp_fwd_decision_from_learn_buffer(
                        unit,
                        data.learn_info,
                        SOC_PPC_LIF_ENTRY_TYPE_EXTENDER, 
                        &extender_info->learn_record.learn_info
                        );
                    SOCDNX_SAND_IF_ERR_EXIT(rv);
                } else {
                    if(extender_info->use_lif) {
                        extender_info->learn_record.learn_info.additional_info.outlif.val = *lif_index;
                        extender_info->learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
                        extender_info->learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
                    }
                }
            } else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Mismatch between lif SW DB and lif entry.")));
            }
            break;
        } 
    }


exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_l2_lif_extender_add_internal_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY            *extender_key,
    SOC_SAND_IN  uint8                                  ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                         lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO           *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  )
{
    ARAD_PP_ISEM_ACCESS_ENTRY sem_entry;
    uint32 res;
    SOC_DPP_DBAL_SW_TABLE_IDS dbal_table_ids[ARAD_PP_ISEM_ACCESS_NOF_TABLES];
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX];
    void *in_lif_data = NULL;
    uint64 additional_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(extender_info);
    SOC_SAND_CHECK_NULL_INPUT(success);

    
    if (!ignore_key) {
        
        ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
        res = arad_pp_lif_index_to_sem_result(unit, lif_index, extender_info->ext_lif_id, &sem_entry.sem_result_ndx);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

        
        SOC_SAND_CHECK_NULL_INPUT(extender_key);

        res = arad_pp_l2_lif_extender_dbal_entry_get(unit, extender_key, dbal_table_ids, qual_vals);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

        
        if (extender_key->is_tagged) {
            res = arad_pp_dbal_entry_add(unit, dbal_table_ids[0], qual_vals, 0, &sem_entry, success);
        }else {
            res = arad_pp_dbal_entry_add(unit, dbal_table_ids[1], qual_vals, 0, &sem_entry, success);
        }
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    } else {
        *success = SOC_SAND_SUCCESS;
    }


    
    if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2) {

        
        res = arad_pp_l2_lif_extender_fill_lif_data(unit, extender_info, &in_lif_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        res = arad_pp_l2_lif_extender_lif_data_set(unit, extender_info->service_type, lif_index, in_lif_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit, lif_index, SOC_PPC_LIF_ENTRY_TYPE_EXTENDER, extender_info->service_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    if (SOC_IS_JERICHO(unit) && extender_info->is_extended)
    {
        COMPILER_64_SET(additional_data,0,0);
        res = arad_pp_lif_additional_data_set(unit, lif_index, TRUE, additional_data,FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
    }

exit:
    if (in_lif_data) {
        sal_free(in_lif_data);
    }
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_extender_add_internal_unsafe()", 0, 0);
}



uint32 arad_pp_l2_lif_extender_add_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY    *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                 lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO   *extender_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(extender_key);
    SOC_SAND_CHECK_NULL_INPUT(extender_info);
    SOC_SAND_CHECK_NULL_INPUT(success);

    res = arad_pp_l2_lif_extender_add_internal_unsafe(
            unit,
            extender_key,
            FALSE,
            lif_index,
            extender_info,
            success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_extender_add_unsafe()", 0, 0);
}


uint32 arad_pp_l2_lif_extender_add_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                     lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info)
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = SOC_PPC_L2_LIF_EXTENDER_KEY_verify(unit, extender_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);

    res = SOC_PPC_L2_LIF_EXTENDER_INFO_verify(unit, extender_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_extender_add_verify()", 0, 0);
}

soc_error_t
  arad_pp_l2_lif_extender_remove(
    int                                     unit,
    SOC_PPC_L2_LIF_EXTENDER_KEY             *extender_key,
    int                                     *lif_index
  )
{
    int rv;
    SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[ARAD_PP_ISEM_ACCESS_NOF_TABLES];

    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE  success;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(extender_key);
    SOCDNX_NULL_CHECK(lif_index);

    
    
    rv = arad_pp_l2_lif_extender_dbal_entry_get(unit, extender_key, dbal_tables_id, qual_vals);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (extender_key->is_tagged) {
        rv = arad_pp_dbal_entry_delete(unit, dbal_tables_id[0], qual_vals, &success);
    } else {
        rv = arad_pp_dbal_entry_delete(unit, dbal_tables_id[1], qual_vals, &success);
    }
    SOCDNX_IF_ERR_EXIT(rv);

    if (!SOC_SAND_SUCCESS2BOOL(success)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Failed to delete ISEM entry")));
    }
    
    
    
    rv = arad_pp_sw_db_lif_table_entry_use_set(
              unit,
              *lif_index,
              SOC_PPC_LIF_ENTRY_TYPE_EMPTY,
              0);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_pp_lif_additional_data_set(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_IN  uint64 data,
        SOC_SAND_IN  uint8 is_clear
   )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS;
 
  if (SOC_IS_JERICHO(unit))
  {
    if (is_ingress) {
        bcm_dpp_am_local_inlif_info_t       inlif_info;
        bcm_dpp_am_local_inlif_info_t       inlif_info_tmp;
        ARAD_PP_IHP_LIF_TABLE_TBL_DATA  tbl_data;
        uint8 half_offset, start_bit_off;
        uint32 data32_arr[2];

        
        res = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, lif_id, &inlif_info);
        SOCDNX_IF_ERR_EXIT(res);

        if (((inlif_info.local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE) == 0)
            || ((inlif_info.ext_lif_id == _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED))) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
        }

        
        half_offset = ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_OFFSET(lif_id, inlif_info.ext_lif_id);


        
        res = arad_pp_ihp_lif_table_tbl_get_unsafe(  
          unit,
          inlif_info.ext_lif_id,
          &tbl_data
        );

        data32_arr[0] = COMPILER_64_LO(data);
        data32_arr[1] = COMPILER_64_HI(data);

        
        start_bit_off = (half_offset) ? ARAD_PP_LIF_WIDE_DATA_SECOND_HALF_LSB : ARAD_PP_LIF_WIDE_DATA_FIRST_HALF_LSB; 
        SHR_BITCOPY_RANGE(tbl_data.lif_table_entry,start_bit_off,data32_arr,0,ARAD_PP_LIF_WIDE_DATA_WIDTH);

        if (is_clear)
        {
            if (half_offset)
            {
                
                res = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, (inlif_info.ext_lif_id + 1), &inlif_info_tmp);
                SOCDNX_IF_ERR_EXIT(res);
            }
            else
            {
                
                res = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, (inlif_info.ext_lif_id - 1), &inlif_info_tmp);
                SOCDNX_IF_ERR_EXIT(res);
            }
            if ((inlif_info_tmp.valid == FALSE) ||
                ((inlif_info_tmp.local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE) == 0))
            {
                SHR_BITCLR(tbl_data.lif_table_entry, 116);

                if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "inlif_additional_data_lock_bit_enable", 0)) {
                    
                    SHR_BITCLR(tbl_data.lif_table_entry, start_bit_off + ARAD_PP_LIF_WIDE_DATA_WIDTH - 1);
                }
            }
        }
        else
        {
            SHR_BITSET(tbl_data.lif_table_entry, 116);

            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "inlif_additional_data_lock_bit_enable", 0)) {
                
                SHR_BITSET(tbl_data.lif_table_entry, start_bit_off + ARAD_PP_LIF_WIDE_DATA_WIDTH - 1);
            }
        }

        
        res = arad_pp_ihp_lif_table_tbl_set_unsafe(unit, inlif_info.ext_lif_id, &tbl_data);
    }
    else
    {
        bcm_dpp_am_local_out_lif_info_t       outlif_info;
        uint32 data_to_set;
        int ext_offset;
        uint32 tbl_data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];

        
        res = SOC_PPC_OUTLIF_SW_STATE_INFO_GET(unit, lif_id, &outlif_info);
        SOCDNX_IF_ERR_EXIT(res);

        if (((outlif_info.local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_WIDE) == 0)
            || ((outlif_info.ext_lif_id == _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED))) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
        }

        
        ext_offset =  ( ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, lif_id) % ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK ) * 20;

        res = arad_pp_eg_enacp_access_additional_data_entry_get(unit,outlif_info.ext_lif_id,tbl_data);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        data_to_set = ( COMPILER_64_LO(data) & 0xFFFFF);

        SHR_BITCOPY_RANGE(tbl_data, ext_offset, &data_to_set, 0, 20);

        res = arad_pp_eg_enacp_access_additional_data_entry_set(unit,outlif_info.ext_lif_id,tbl_data);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }


  }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_pp_lif_additional_data_get(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  int lif_id, 
        SOC_SAND_IN  uint8 is_ingress,
        SOC_SAND_OUT uint64 *data
   )
{

  uint32
      res = SOC_SAND_OK,
      num_bits = ARAD_PP_LIF_WIDE_DATA_WIDTH;

  SOCDNX_INIT_FUNC_DEFS;
 
  if (SOC_IS_JERICHO(unit))
  {
      if (is_ingress) {

          uint8 half_offset , start_bit_off;
          bcm_dpp_am_local_inlif_info_t   inlif_info;
          uint32 data_get[2] = {0,0};
          ARAD_PP_IHP_LIF_TABLE_TBL_DATA  tbl_data;

          
          res = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, lif_id, &inlif_info);
          SOCDNX_IF_ERR_EXIT(res);

          
          half_offset = ARAD_PP_LIF_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_OFFSET(lif_id, inlif_info.ext_lif_id);

          
          res = arad_pp_ihp_lif_table_tbl_get_unsafe(  
                unit,
                inlif_info.ext_lif_id,
                &tbl_data
          );
          SOCDNX_SAND_IF_ERR_EXIT(res);

          if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "inlif_additional_data_lock_bit_enable", 0) && (tbl_data.double_data_entry == TRUE)) {
              
              num_bits -= 1;
          }

          
          start_bit_off = (half_offset) ? 57 : 0;
          SHR_BITCOPY_RANGE(data_get, 0, tbl_data.lif_table_entry,start_bit_off, num_bits);
          COMPILER_64_SET(*data , data_get[1],data_get[0]);
      }
      else
      {
        bcm_dpp_am_local_out_lif_info_t       outlif_info;
        int ext_offset;
        uint32 data_get = 0;
        uint32 tbl_data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];

        
        res = SOC_PPC_OUTLIF_SW_STATE_INFO_GET(unit, lif_id, &outlif_info);
        SOCDNX_IF_ERR_EXIT(res);

        
        ext_offset =  ( ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, lif_id) % ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK ) * 20;

        res = arad_pp_eg_enacp_access_additional_data_entry_get(unit,outlif_info.ext_lif_id,tbl_data);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        SHR_BITCOPY_RANGE(&data_get, 0, tbl_data, ext_offset, 20);
        COMPILER_64_SET(*data ,  0,  data_get); 

      }
  }

exit:
  SOCDNX_FUNC_RETURN;
}



#if 0
uint32 arad_pp_lif_protection_pointer_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID             in_lif_ndx,
    SOC_SAND_IN  uint32                     *protection_pointer)
{
    uint32 res = SOC_SAND_OK, lif_sub_type;
    SOC_PPC_LIF_ENTRY_TYPE lif_type;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_RIF_IP_TERM_KEY term_key;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    SOC_SAND_CHECK_NULL_INPUT(protection_pointer);
    
    
    res = arad_pp_sw_db_lif_table_entry_use_get(
          unit, in_lif_ndx, &lif_type, &lif_sub_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    switch (lif_type) {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
        res = arad_pp_l2_lif_ac_get_internal_unsafe(
              unit, NULL, TRUE, in_lif_ndx,
              &(lif_entry_info->value.ac), &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        *protection_pointer = lif_entry_info->value.ac.protection_pointer;
        break;

    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
        res = arad_pp_l2_lif_pwe_get_internal_unsafe(
              unit, 0, TRUE, in_lif_ndx,
              &(lif_entry_info->value.pwe), &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        *protection_pointer = lif_entry_info->value.pwe.protection_pointer;
        break;

    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
        res = arad_pp_rif_ip_tunnel_term_get_internal_unsafe(
              unit, &term_key, TRUE, in_lif_ndx,
              &(lif_entry_info->value.ip_term_info), &(lif_entry_info->value.rif), &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        *protection_pointer = lif_entry_info->value.ip_term_info.protection_pointer;
        break;

    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
        res = arad_pp_rif_mpls_label_map_get_internal_unsafe(
              unit, NULL, TRUE, in_lif_ndx,
              &(lif_entry_info->value.mpls_term_info), &(lif_entry_info->value.rif), &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        *protection_pointer = lif_entry_info->value.mpls_term_info.protection_pointer;
        break;



    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_protection_pointer_get_unsafe()", 0, 0);
}



#endif










uint32
  arad_pp_l2_lif_ac_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY             *isem_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY               *ac_key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_L2_LIF_AC_KEY_clear(ac_key);

  switch(isem_key->key_type) {
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD:  
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->core_id = isem_key->key_info.l2_eth.core_id;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_UNTAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_UNTAG;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_COMPRESSED_TAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_DOUBLE_TAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    ac_key->inner_vid   = isem_key->key_info.l2_eth.inner_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_ETH;
    ac_key->vlan_domain = isem_key->key_info.section_oam.port;
    ac_key->ethertype = isem_key->key_info.section_oam.ether_type;
    ac_key->outer_vid   = isem_key->key_info.section_oam.outer_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN_ETH;
    ac_key->vlan_domain = isem_key->key_info.section_oam.port;
    ac_key->ethertype = isem_key->key_info.section_oam.ether_type;
    ac_key->outer_vid   = isem_key->key_info.section_oam.outer_vid;
    ac_key->inner_vid   = isem_key->key_info.section_oam.inner_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_INITIAL_TAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    ac_key->outer_pcp   = isem_key->key_info.l2_eth.outer_pcp;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    ac_key->outer_pcp   = isem_key->key_info.l2_eth.outer_pcp;
    ac_key->inner_vid   = isem_key->key_info.l2_eth.inner_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL:
    if (isem_key->key_info.l2_pon.flags & ARAD_PP_ISEM_ACCESS_L2_PON_TLS) 
    {
      ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TLS_PORT_TUNNEL;
    } 
    else
    {
      ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL;
    }
    
    ac_key->vlan_domain = isem_key->key_info.l2_pon.port;
    ac_key->tunnel      = isem_key->key_info.l2_pon.tunnel_id;    
  
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_pon.port;
    ac_key->tunnel      = isem_key->key_info.l2_pon.tunnel_id;    
    ac_key->outer_vid   = isem_key->key_info.l2_pon.outer_vid;    
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN;
    ac_key->vlan_domain = isem_key->key_info.l2_pon.port;
    ac_key->tunnel      = isem_key->key_info.l2_pon.tunnel_id;    
    ac_key->outer_vid   = isem_key->key_info.l2_pon.outer_vid;    
    ac_key->inner_vid   = isem_key->key_info.l2_pon.inner_vid;    
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN;          
    ac_key->vlan_domain = isem_key->key_info.l2_pon.port;
    ac_key->tunnel      = (isem_key->key_info.l2_pon.tunnel_id_valid) ? isem_key->key_info.l2_pon.tunnel_id:SOC_PPC_LIF_IGNORE_TUNNEL_VID;    
    ac_key->outer_vid   = (isem_key->key_info.l2_pon.outer_vid_valid) ? isem_key->key_info.l2_pon.outer_vid:SOC_PPC_LIF_IGNORE_OUTER_VID;
    ac_key->inner_vid   = (isem_key->key_info.l2_pon.inner_vid_valid) ? isem_key->key_info.l2_pon.inner_vid:SOC_PPC_LIF_IGNORE_INNER_VID;
    ac_key->ethertype   = (isem_key->key_info.l2_pon.ether_type_valid) ? isem_key->key_info.l2_pon.ether_type:SOC_PPC_LIF_IGNORE_ETHER_TYPE;
    ac_key->outer_pcp   = (isem_key->key_info.l2_pon.outer_pcp_valid) ? isem_key->key_info.l2_pon.outer_pcp:SOC_PPC_LIF_IGNORE_OUTER_PCP;    
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_SINGLE_TAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_DOUBLE_TAG:
    ac_key->key_type    = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID;
    ac_key->vlan_domain = isem_key->key_info.l2_eth.vlan_domain;
    ac_key->outer_vid   = isem_key->key_info.l2_eth.outer_vid;
    ac_key->inner_vid   = isem_key->key_info.l2_eth.inner_vid;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_ac_key_parse_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_pwe_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL               *pwe
  )
{
  uint32
    res;
  uint8
    use_label_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *pwe = 0;

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mpls_info.mpls_termination_label_index_enable.get(unit,&use_label_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (use_label_index)
  {
    switch(isem_key->key_type) {
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1:
        SOC_PPC_VC_LABEL_INDEXED_SET(*pwe, isem_key->key_info.mpls.label, 1);
        break;
      case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2:
        SOC_PPC_VC_LABEL_INDEXED_SET(*pwe, isem_key->key_info.mpls.label, 2);
        break;
      case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3:
        SOC_PPC_VC_LABEL_INDEXED_SET(*pwe, isem_key->key_info.mpls.label, 3);
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 10, exit);
    }
  }
  else
  {
    *pwe = isem_key->key_info.mpls.label;
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_pwe_key_parse_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_isid_key_parse_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY              *isem_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_KEY              *isid_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  isid_key->isid_domain = isem_key->key_info.pbb.isid_domain;
  isid_key->isid_id = isem_key->key_info.pbb.isid;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_isid_key_parse_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_lif;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_lif;
}

uint32
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LLP_PARSE_INFO, &info->pkt_parse_info, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_profile, ARAD_PP_LIF_PORT_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_IN_VC_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_IN_VC_RANGE_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_frwd_type, ARAD_PP_LIF_DEFAULT_FRWD_TYPE_MAX, ARAD_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_DECISION_INFO, &(info->default_forwarding), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->enable_learning)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_DECISION_INFO, &(info->learn_info), 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_PWE_LEARN_RECORD_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_PWE_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  if (info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
  {
    if (info->default_frwrd.default_frwd_type != SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
    }
  }
  else
  {
    if (info->default_frwrd.default_frwd_type != SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 16, exit);
    }
  }
  if (info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 11, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->model, SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS - 1, ARAD_PP_LIF_PWE_MPLS_TUNNEL_MODEL_OUT_OF_RANGE_ERR, 25, exit);
  }
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_PWE_LEARN_RECORD, &(info->learn_record), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, ARAD_PP_LIF_ORIENTATION_MAX, ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, ARAD_PP_LIF_PWE_SERVICE_TYPE_MAX, ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 17, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile_index, ARAD_PP_LIF_TPID_PROFILE_INDEX_MAX, ARAD_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_LIF_COS_PROFILE_MAX, ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_assignment_mode, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_MAX, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR, 18, exit);  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_LIF_LIF_PROFILE_OUT_OF_RANGE_ERR, 19, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->term_profile, ARAD_PP_LIF_PWE_TERM_PROFILE_MAX, ARAD_PP_LIF_PWE_TERM_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_profile, ARAD_PP_LIF_PWE_ACTION_PROFILE_MAX, ARAD_PP_LIF_PWE_ACTION_PROFILE_OUT_OF_RANGE_ERR, 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_MAX, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR, 22, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id), ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 23, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pass_value, ARAD_PP_LIF_PROTECTION_PASS_VAL_MAX, ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->network_group, ARAD_PP_LIF_NETWORK_GROUP_MAX, ARAD_PP_LIF_NETWORK_GROUP_OUT_OF_RANGE_ERR, 25, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_PWE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_VLAN_RANGE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 13, exit);
  if(info->first_vid > info->last_vid)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_VLAN_RANGE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_KEY_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->vlan_domain, SOC_DPP_DEFS_GET(unit, nof_vlan_domains), ARAD_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);

  if (info->outer_vid != SOC_PPC_LIF_IGNORE_OUTER_VID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  }

  if (info->inner_vid != SOC_PPC_LIF_IGNORE_INNER_VID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->inner_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->raw_key, ARAD_PP_LIF_RAW_KEY_MAX, ARAD_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->learn_type, ARAD_PP_LIF_LEARN_TYPE_MAX, ARAD_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (!((info->learn_type == SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT) || (info->learn_type == SOC_PPC_L2_LIF_AC_LEARN_DISABLE)))
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_DECISION_INFO, &(info->learn_info), 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_LEARN_RECORD_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_vlan_edit_profile, ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_MAX, ARAD_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_pcp_profile, ARAD_PP_LIF_EDIT_PCP_PROFILE_MAX, ARAD_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, ARAD_PP_LIF_AC_SERVICE_TYPE_MAX, ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_AC_LEARN_RECORD, &(info->learn_record), 12, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_AC_ING_EDIT_INFO, &(info->ing_edit_info), 13, exit);
  if (info->default_frwrd.default_frwd_type != SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI)
  {
    if (info->service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
    {
      ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 14, exit);
    }
  }
  if (info->service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
  {
    if (info->default_frwrd.default_frwd_type != SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
    }
  }
  else
  {
    if (info->default_frwrd.default_frwd_type != SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 16, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, ARAD_PP_LIF_ORIENTATION_MAX, ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_LIF_COS_PROFILE_MAX, ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR, 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->l2cp_profile, ARAD_PP_LIF_L2CP_PROFILE_MAX, ARAD_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR, 22, exit);
  if (info->vsi_assignment_mode != SOC_PPC_VSI_EQ_IN_VID) 
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_assignment_mode, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_MAX, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR, 23, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lif_profile, ARAD_PP_LIF_LIF_PROFILE_MAX, ARAD_PP_LIF_LIF_PROFILE_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_MAX, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR, 25, exit);



  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id), ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 26, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pass_value, ARAD_PP_LIF_PROTECTION_PASS_VAL_MAX, ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR, 27, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mp_level, ARAD_PP_MP_LEVEL_MAX, SOC_PPC_MP_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_MP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_L2CP_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->l2cp_profile, ARAD_PP_LIF_L2CP_PROFILE_MAX, ARAD_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_mac_address_lsb, ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_MAX, ARAD_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_L2CP_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_AC_GROUP_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < info->nof_lif_entries; ++ind)
  {
    res = SOC_PPC_L2_LIF_AC_INFO_verify(unit, &(info->acs_info[ind]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->opcode_id, ARAD_PP_LIF_OPCODE_ID_MAX, ARAD_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_lif_entries, ARAD_PP_LIF_NOF_LIF_ENTRIES_MAX, ARAD_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_AC_GROUP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_ISID_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->isid_id, SOC_SAND_PP_ISID_MAX, SOC_SAND_PP_ISID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->isid_domain, ARAD_PP_LIF_ISID_DOMAIN_MAX, ARAD_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_ISID_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_ISID_INFO_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_LIF_COS_PROFILE_MAX, ARAD_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, ARAD_PP_LIF_ISID_SERVICE_TYPE_MAX, ARAD_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 14, exit);
  if (info->service_type == SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P) {
      ARAD_PP_STRUCT_VERIFY(SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, ARAD_PP_LIF_ORIENTATION_MAX, ARAD_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile_index, ARAD_PP_LIF_TPID_PROFILE_INDEX_MAX, ARAD_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR, 16, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_assignment_mode, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_MAX, ARAD_PP_LIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR, 17, exit);  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_MAX, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR, 18, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id), ARAD_PP_LIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 19, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pass_value, ARAD_PP_LIF_PROTECTION_PASS_VAL_MAX, ARAD_PP_LIF_PROTECTION_PASS_VAL_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_ISID_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_TRILL_INFO_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fec_id, ARAD_PP_FEC_ID_MAX, SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pass_value, 1, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id), ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS -1, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_ISID_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_GRE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vpn_key, ARAD_PP_LIF_GRE_VPN_MAX, ARAD_PP_LIF_GRE_KEY_VPN_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_GRE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_GRE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_GRE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_VXLAN_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vpn_key, ARAD_PP_LIF_VXLAN_VPN_MAX, ARAD_PP_LIF_VXLAN_KEY_VPN_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_VXLAN_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_VXLAN_INFO_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_VXLAN_INFO_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_type, ARAD_PP_LIF_DA_TYPE_MAX, ARAD_PP_LIF_DA_TYPE_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lif_default_forward_profile, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_MAX, ARAD_PP_LIF_DEFAULT_FRWRD_PROFILE_OUT_OF_RANGE_ERR, 18, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_da_not_found_profile, ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_MAX, ARAD_PP_LIF_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR, 25, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_verify()",0,0);
}

uint32
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_verify(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->add_vsi, ARAD_PP_LIF_ADD_VSI_MAX, ARAD_PP_LIF_ADD_VSI_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->offset, ARAD_PP_LIF_OFFSET_MAX, ARAD_PP_LIF_OFFSET_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_DECISION_INFO, &(info->frwrd_dest), 15, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_verify()",0,0);
}


uint32 SOC_PPC_L2_LIF_EXTENDER_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);



  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_EXTENDER_KEY_verify()",0,0);
}

uint32 SOC_PPC_L2_LIF_EXTENDER_INFO_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_L2_LIF_EXTENDER_INFO_verify()",0,0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

