/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_MPLS_TERM_INCLUDED__

#define __SOC_PPC_API_MPLS_TERM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_lif.h>







#define SOC_PPC_MPLS_TERM_NOF_RESERVED_LABELS (16)


#define SOC_PPC_MPLS_TERM_MAX_NOF_TERM_LABELS (3)

#define SOC_PPC_MPLS_TERM_DISCARD_TTL_0  (0x1) 
#define SOC_PPC_MPLS_TERM_DISCARD_TTL_1  (0x2) 
#define SOC_PPC_MPLS_TERM_HAS_CW        (0x4) 
#define SOC_PPC_MPLS_TERM_SKIP_ETH      (0x8)


#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI                          (7)
#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL                          (13)
#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP                     (0)
#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP                     (2)
#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_RA                           (1)
#define SOC_PPC_MPLS_TERM_RESERVED_LABEL_OAM_ALERT                    (14)

#define SOC_PPC_MPLS_TERM_FLAG_DUMMY_LABEL   (0x1)
#define SOC_PPC_MPLS_TERM_FLAG_EVPN_IML      (0x2)








typedef enum
{
  
  SOC_PPC_MPLS_TERM_MODEL_PIPE = 0,
  
  SOC_PPC_MPLS_TERM_MODEL_UNIFORM = 1,
  
  SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES = 2
}SOC_PPC_MPLS_TERM_MODEL_TYPE;

typedef enum
{
  
  SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL = 0,
  
  SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF = 1,
  
  SOC_PPC_NOF_MPLS_TERM_KEY_TYPES = 2
}SOC_PPC_MPLS_TERM_KEY_TYPE;


typedef enum
{
  
  SOC_PPC_MPLS_TERM_NAMESPACE_L1 = 0,
  
  SOC_PPC_MPLS_TERM_NAMESPACE_L2 = 1,
  
  SOC_PPC_MPLS_TERM_NAMESPACE_L3 = 2,
  
  SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3 = 3,
  
  SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2 = 4,  
   
  SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES = 5  
}SOC_PPC_MPLS_TERM_NAMESPACE_TYPE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_LABEL first_label;
  
  SOC_SAND_PP_MPLS_LABEL last_label;

} SOC_PPC_MPLS_TERM_LABEL_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_PPC_MPLS_TERM_KEY_TYPE key_type;

} SOC_PPC_MPLS_TERM_LKUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MPLS_TERM_MODEL_TYPE processing_type;
  
  SOC_PPC_RIF_ID rif;
  
  uint32 cos_profile;
  
  SOC_PPC_L3_NEXT_PRTCL_TYPE next_prtcl;

  
  SOC_TMC_PKT_FRWRD_TYPE forwarding_code;

  
  uint32 lif_profile;

  
  uint32 tpid_profile;

  
  uint32 default_forward_profile;

  
  SOC_PPC_VSI_ID vsi;

  
  uint8 learn_enable;

  
  uint32 vsi_assignment_mode;

  
  uint32 protection_pointer;
  
  uint8 protection_pass_value;

  
  uint8 oam_valid;

  
  uint8 trap_code_index; 

   
  uint8 term_profile;

  
  uint8 is_extended;

  
  uint64 additional_data;

  
  int ext_lif_id;

  
  int global_lif;

   
  uint8 orientation;

  
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO default_frwrd;

  
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE service_type;

} SOC_PPC_MPLS_TERM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MPLS_TERM_LABEL_RANGE range;
  
  SOC_PPC_MPLS_TERM_INFO term_info;

} SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_ACTION_PROFILE bos_action;
  
  SOC_PPC_ACTION_PROFILE non_bos_action;
  
  SOC_PPC_MPLS_TERM_INFO term_info;

} SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MPLS_TERM_MODEL_TYPE processing_type;
  
  SOC_PPC_RIF_ID default_rif;
  
  uint8 uc_rpf_enable;

} SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 use_for_pipe;
  
  uint8 use_for_uniform;

} SOC_PPC_MPLS_TERM_COS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_EXP in_exp;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL model;

} SOC_PPC_MPLS_TERM_LABEL_COS_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TC tc;
  
  uint8 overwrite_tc;
  
  SOC_SAND_PP_DP dp;
  
  uint8 overwrite_dp;

} SOC_PPC_MPLS_TERM_LABEL_COS_VAL;

typedef struct { 

   
  uint32 flags; 
   
  uint32 nof_headers; 
   
  uint32 ttl_exp_index; 

#ifdef BCM_88660_A0
  
  uint32 check_bos;

  
  uint32 expect_bos;
#endif 

} SOC_PPC_MPLS_TERM_PROFILE_INFO; 


typedef struct {
  
  bcm_mpls_label_t label_low;
  
  
  bcm_mpls_label_t label_high;
  
  
  uint32 bos_value;
  
  
  uint32 bos_value_mask;

} SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO;


typedef struct {
  
  uint32 mpls_label_range_tag_mode;
  
  
  uint32 mpls_label_range_has_cw;
  
  
  uint32 mpls_label_range_set_outer_vid;
  
  
  uint32 mpls_label_range_set_inner_vid;
  
  
  uint32 mpls_label_range_use_base ; 

} SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO;











void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE *info
  );

void
  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LKUP_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_COS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_COS_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  );

void
  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *info
    );

void
  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *info
    );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MPLS_TERM_MODEL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_MODEL_TYPE enum_val
  );

const char*
  SOC_PPC_MPLS_TERM_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_KEY_TYPE enum_val
  );

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE *info
  );

void
  SOC_PPC_MPLS_TERM_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_COS_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  );

void
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  );

void
  SOC_PPC_MPLS_TERM_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *info
  );

void
  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

