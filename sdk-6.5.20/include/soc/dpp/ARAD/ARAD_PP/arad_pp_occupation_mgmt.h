/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef __ARAD_PP_OCCUPATION_MGMT_INCLUDED__

#define __ARAD_PP_OCCUPATION_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/dpp_config_imp_defs.h>



#define SOC_OCC_MGMT_FLAGS_NONE                 (0)
#define SOC_OCC_MGMT_APP_USER                   (-1)

#define SOC_OCC_MGMT_INLIF_SIZE         (4)
#define SOC_OCC_MGMT_OUTLIF_SIZE(unit)  (SOC_DPP_IMP_DEFS_GET(unit, outlif_profile_nof_bits))
#define SOC_OCC_MGMT_OUTLIF_SIZE_MAX    (SOC_OCC_MGMT_OUTLIF_SIZE(unit)) 
#define SOC_OCC_MGMT_RIF_SIZE           (1)



#define SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS    (4)

#define SOC_OCC_MGMT_EG_FILTER_INLIF_PROFILE_BITMAP_MAX         ((1 << SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) - 1)





typedef enum {
    
    SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE = 0,
    
    SOC_OCC_MGMT_INLIF_APP_OAM,
    
    SOC_OCC_MGMT_INLIF_APP_TAGGED_MODE,
    
    SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING,
    
    SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 
    
    SOC_OCC_MGMT_INLIF_APP_ORIENTATION,  
    
    SOC_OCC_MGMT_INLIF_APP_PRESERVE_DSCP,  
    
    SOC_OCC_MGMT_INLIF_APP_POLICER_COLOR_MAPPING_PCP,
    
    SOC_OCC_MGMT_INLIF_APP_IP_DISABLE,
    
    SOC_OCC_MGMT_INLIF_APP_IPV4_DISABLE,
    
    SOC_OCC_MGMT_INLIF_APP_IPV6_DISABLE,
    
    SOC_OCC_MGMT_INLIF_APP_LOGICAL_INTF_SAME_FILTER,

    SOC_OCC_MGMT_INLIF_APP_NOF
} SOC_OCC_MGMT_INLIF_APP;


typedef enum {
    SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP = 0,
    SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP,
    SOC_OCC_MGMT_OUTLIF_APP_TAGGED_MODE,
    
    SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
    
    SOC_OCC_MGMT_OUTLIF_APP_EVPN,
    
    SOC_OCC_MGMT_OUTLIF_MIM_ENCAP,
    
    SOC_OCC_MGMT_OUTLIF_APP_EG_PWE_COUNTING,
    SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE,
    SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL,
    
    SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_INDICATION,
    
    SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENTROPY_LABEL_NON_BOS_INDICATION,
    
    SOC_OCC_MGMT_OUTLIF_APP_MPLS_PUSH_OR_SWAP,
    
    SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF,
    
    SOC_OCC_MGMT_OUTLIF_APP_ADDITIONAL_LABEL,  
    
    SOC_OCC_MGMT_OUTLIF_APP_PRESERVE_DSCP,  
    
    SOC_OCC_MGMT_OUTLIF_APP_MTU_FILTER,
    
    SOC_OCC_MGMT_OUTLIF_APP_L2CP_EGRESS_PROFILE,
    
    SOC_OCC_MGMT_OUTLIF_APP_TTL_INHERITANCE,
    
    SOC_OCC_MGMT_OUTLIF_APP_TOS_INHERITANCE,
    
    SOC_OCC_MGMT_OUTLIF_APP_IPV4_TUNNEL_TOR,
    
    SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL_TOR,
    
    SOC_OCC_MGMT_OUTLIF_APP_PPPOE_ENCAP,
	
	SOC_OCC_MGMT_OUTLIF_APP_L2TP_ENCAP,
    
    SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL,
    SOC_OCC_MGMT_OUTLIF_APP_NOF
} SOC_OCC_MGMT_OUTLIF_APP;


typedef enum {
    SOC_OCC_MGMT_RIF_APP_NOF
} SOC_OCC_MGMT_RIF_APP;


typedef enum {
    SOC_OCC_MGMT_TYPE_INLIF = 0,
    SOC_OCC_MGMT_TYPE_OUTLIF,
    SOC_OCC_MGMT_TYPE_RIF,
    SOC_OCC_MGMT_TYPE_NOF
} SOC_OCC_MGMT_TYPE;


typedef struct {
    
    int inlif_profile  [SOC_OCC_MGMT_INLIF_SIZE + 1];
    int outlif_profile [SOC_DPP_IMP_DEFS_MAX(OUTLIF_PROFILE_NOF_BITS) + 1];
    int rif_profile    [SOC_OCC_MGMT_RIF_SIZE + 1];
} ARAD_PP_OCCUPATION;









soc_error_t
arad_pp_occ_mgmt_init (int unit);

soc_error_t
arad_pp_occ_mgmt_deinit (int unit);

soc_error_t
arad_pp_occ_mgmt_get_app_mask(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   SHR_BITDCL                  *mask);

soc_error_t
arad_pp_occ_mgmt_app_get(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   SHR_BITDCL                  *full_occupation,
   uint32                      *val);

soc_error_t
arad_pp_occ_mgmt_app_set(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   uint32                      val,
   SHR_BITDCL                  *full_occupation);

soc_error_t 
arad_pp_occ_mgmt_diag_info_get(int unit, SOC_OCC_MGMT_TYPE occ_type, char* apps[]);


soc_error_t
arad_pp_occ_mgmt_tcam_args_get (
   int                  unit, 
   SOC_OCC_MGMT_TYPE    occ_type,
   int                  application_type,
   uint32               value,
   SHR_BITDCL           *full_occupation,
   SHR_BITDCL           *mask,
   SHR_BITDCL           *mask_flipped);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
