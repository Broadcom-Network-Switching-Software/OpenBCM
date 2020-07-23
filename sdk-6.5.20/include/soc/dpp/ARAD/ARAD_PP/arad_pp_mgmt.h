
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_MGMT_INCLUDED__

#define __ARAD_PP_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>





#define ARAD_PP_MGMT_MPLS_NOF_ETHER_TYPES 2

#define ARAD_PP_MGMT_IPV4_LPM_BANKS 6


#define  ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE (0x1)

#define  ARAD_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG (0x2)


#define  ARAD_PP_MGMT_IPV4_SHARED_ROUTES_MEMORY (0x4)












typedef enum
{
  
  ARAD_PP_MGMT_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_MGMT_FIRST,
  ARAD_PP_MGMT_GET_ERRS_PTR,
  

  ARAD_PP_MGMT_DEVICE_INIT,
  ARAD_PP_MGMT_DEVICE_CLOSE,
  ARAD_PP_MGMT_OPERATION_MODE_SET,
  ARAD_PP_MGMT_OPERATION_MODE_GET,
  ARAD_PP_MGMT_OPERATION_MODE_SET_UNSAFE,
  ARAD_PP_MGMT_OPERATION_MODE_VERIFY,
  ARAD_PP_MGMT_OPERATION_MODE_GET_UNSAFE,
  ARAD_PP_MGMT_ELK_MODE_SET,
  ARAD_PP_MGMT_ELK_MODE_SET_PRINT,
  ARAD_PP_MGMT_ELK_MODE_SET_UNSAFE,
  ARAD_PP_MGMT_ELK_MODE_SET_VERIFY,
  ARAD_PP_MGMT_ELK_MODE_GET,
  ARAD_PP_MGMT_ELK_MODE_GET_PRINT,
  ARAD_PP_MGMT_ELK_MODE_GET_VERIFY,
  ARAD_PP_MGMT_ELK_MODE_GET_UNSAFE,
  ARAD_PP_MGMT_USE_ELK_SET,
  ARAD_PP_MGMT_USE_ELK_SET_PRINT,
  ARAD_PP_MGMT_USE_ELK_SET_UNSAFE,
  ARAD_PP_MGMT_USE_ELK_SET_VERIFY,
  ARAD_PP_MGMT_USE_ELK_GET,
  ARAD_PP_MGMT_USE_ELK_GET_PRINT,
  ARAD_PP_MGMT_USE_ELK_GET_VERIFY,
  ARAD_PP_MGMT_USE_ELK_GET_UNSAFE,
  ARAD_PP_MGMT_PROC_ERR_MECH_INIT,

  
  ARAD_PP_MGMT_PROCEDURE_DESC_LAST
} ARAD_PP_MGMT_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_MGMT_FIRST,
  ARAD_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR,
  ARAD_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR,
  

  
  ARAD_PP_MGMT_ERR_LAST
} ARAD_PP_MGMT_ERR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 mim_vsi;
  
} ARAD_PP_MGMT_P2P_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint8 pvlan_enable;
 
  uint32 nof_vrfs;
 
  uint32 max_routes_in_vrf[SOC_DPP_DEFS_MAX(NOF_VRFS)];

  
  uint8 bits_in_phase[ARAD_PP_MGMT_IPV4_LPM_BANKS];

  
  uint8 bits_in_phase_valid;

 
  uint32 flags;

  
} ARAD_PP_MGMT_IPV4_INFO;



typedef struct
{
  
  SOC_SAND_PP_ETHER_TYPE mpls_ether_types[ARAD_PP_MGMT_MPLS_NOF_ETHER_TYPES];

  
  uint8 mpls_termination_label_index_enable;
  
  uint8 fast_reroute_labels_enable;
  
  uint8 lookup_include_inrif;
  
  uint8 lookup_include_vrf;
   
} ARAD_PP_MGMT_MPLS_INFO;

typedef struct
{
  
  uint8 authentication_enable;
  
  uint8 system_vsi_enable;
  
  uint8 hairpin_enable;
  
  uint8 split_horizon_filter_enable;
  
  ARAD_PP_MGMT_IPV4_INFO ipv4_info;
  
  ARAD_PP_MGMT_P2P_INFO p2p_info;
  
  ARAD_PP_MGMT_MPLS_INFO mpls_info;
  
  uint8 mim_enable;

  
  uint8 oam_enable;

  
  uint8 bfd_enable;

  
  uint8 trill_enable;

  
  uint8 mim_initialized;

}ARAD_PP_MGMT_OPERATION_MODE;

typedef enum
{
  
  ARAD_PP_MGMT_ELK_MODE_NONE = 0,
  
  ARAD_PP_MGMT_ELK_MODE_NORMAL = 1,
  
  ARAD_PP_MGMT_ELK_MODE_B0_SHORT = 2,
  
  ARAD_PP_MGMT_ELK_MODE_B0_LONG = 3,
  
  ARAD_PP_MGMT_ELK_MODE_B0_BOTH = 4,
  
  ARAD_PP_NOF_MGMT_ELK_MODES = 5
}ARAD_PP_MGMT_ELK_MODE;

typedef enum
{
  
  ARAD_PP_MGMT_LKP_TYPE_P2P = 0,
  
  ARAD_PP_MGMT_LKP_TYPE_ETH = 1,
  
  ARAD_PP_MGMT_LKP_TYPE_TRILL_UC = 2,
  
  ARAD_PP_MGMT_LKP_TYPE_TRILL_MC = 3,
  
  ARAD_PP_MGMT_LKP_TYPE_IPV3_UC = 4,
  
  ARAD_PP_MGMT_LKP_TYPE_IPV4_MC = 5,
  
  ARAD_PP_MGMT_LKP_TYPE_IPV6_UC = 6,
  
  ARAD_PP_MGMT_LKP_TYPE_IPV6_MC = 7,
  
  ARAD_PP_MGMT_LKP_TYPE_LSR = 8,
  
  ARAD_PP_NOF_MGMT_LKP_TYPES = 9
}ARAD_PP_MGMT_LKP_TYPE;













uint32
  arad_pp_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );


uint32
  arad_pp_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );


uint32
  arad_pp_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *op_mode
  );


uint32
  arad_pp_mgmt_device_close_unsafe(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_pp_mgmt_elk_mode_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );

uint32
  arad_pp_mgmt_elk_mode_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE elk_mode
  );

uint32
  arad_pp_mgmt_elk_mode_get_verify(
    SOC_SAND_IN  int           unit
  );


uint32
  arad_pp_mgmt_elk_mode_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  );


uint32
  arad_pp_mgmt_use_elk_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );

uint32
  arad_pp_mgmt_use_elk_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  );

uint32
  arad_pp_mgmt_use_elk_get_verify(
    SOC_SAND_IN  int           unit
  );


uint32
  arad_pp_mgmt_use_elk_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT ARAD_PP_MGMT_LKP_TYPE *lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mgmt_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mgmt_get_errs_ptr(void);





void
  ARAD_PP_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_OPERATION_MODE *info
  );

void
  ARAD_PP_MGMT_P2P_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_P2P_INFO *info
  );
void
  ARAD_PP_MGMT_IPV4_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MGMT_IPV4_INFO *info
  );





#if SOC_PPC_DEBUG_IS_LVL1
void
  ARAD_PP_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  ARAD_PP_MGMT_OPERATION_MODE *info
  );

void
  ARAD_PP_MGMT_P2P_INFO_print(
    SOC_SAND_IN  ARAD_PP_MGMT_P2P_INFO *info
  );

void
  ARAD_PP_MGMT_IPV4_INFO_print(
    SOC_SAND_IN  ARAD_PP_MGMT_IPV4_INFO *info
  );

const char*
  ARAD_PP_MGMT_ELK_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_MGMT_ELK_MODE enum_val
  );

const char*
  ARAD_PP_MGMT_LKP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MGMT_LKP_TYPE enum_val
  );

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


