/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_SW_DB_TCAM_MGMT_INCLUDED__

#define __ARAD_SW_DB_TCAM_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#endif






#if defined(INCLUDE_KBP)
typedef enum
{
    
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_MC,
    
    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_MC,
    
    ARAD_KBP_FRWRD_TBL_ID_LSR,
    ARAD_KBP_FRWRD_TBL_ID_TRILL_UC,
    ARAD_KBP_FRWRD_TBL_ID_TRILL_MC,
    ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD,

    
    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED = ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD + 0,
    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP,
    ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR,
    ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6,
    ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P,
    ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_DC,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0,
    ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1,
    ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE,
    
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC_PUBLIC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC_PUBLIC,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD,

    
    ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS,
    

    
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_0 = ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS + 0,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_1,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_2,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_3,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_4,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_5,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_6,
    ARAD_KBP_FRWRD_TBL_ID_DUMMY_7,

    ARAD_KBP_FRWRD_IP_NOF_TABLES,
    ARAD_KBP_ACL_TABLE_ID_OFFSET = ARAD_KBP_FRWRD_IP_NOF_TABLES,

    ARAD_KBP_MAX_NUM_OF_TABLES   = ARAD_KBP_FRWRD_IP_NOF_TABLES + ARAD_PMF_LOW_LEVEL_NOF_DBS,

} ARAD_KBP_FRWRD_IP_TBL_ID;

#endif 

typedef enum
{
  
#if defined(INCLUDE_KBP)
  
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING,
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_FORWARDING = ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING + ARAD_KBP_FRWRD_IP_NOF_TABLES - 1, 
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL,
    ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_ACL = ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL + SOC_PPC_FP_NOF_DBS - 1, 
#endif 
  ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND,
  ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION,
  ARAD_IP_NOF_TCAM_ENTRY_TYPES
} ARAD_IP_TCAM_ENTRY_TYPE;


#define ARAD_TRILL_TCAM_ENTRY_TYPE      (ARAD_IP_NOF_TCAM_ENTRY_TYPES + 1)






#if defined(INCLUDE_KBP)
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(tcam_key_type) ((tcam_key_type >= ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING) && (tcam_key_type <= ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_FORWARDING))
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_ACL(tcam_key_type)        ((tcam_key_type >= ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_ACL) && (tcam_key_type <= ARAD_IP_TCAM_ENTRY_TYPE_KBP_LAST_ACL))
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)            (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_FORWARDING(tcam_key_type) || ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP_ACL(tcam_key_type))
#define ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type)       ((ARAD_KBP_FRWRD_IP_TBL_ID) (tcam_key_type - ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING))
#define ARAD_IP_TCAM_FROM_KBP_FRWRD_IP_TBL_ID(table_id)          ((ARAD_IP_TCAM_ENTRY_TYPE) (table_id + ARAD_IP_TCAM_ENTRY_TYPE_KBP_FIRST_FORWARDING))
#else
#define ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type) (0)
#endif 







typedef enum
{
  ARAD_SW_DB_TCAM_AUX_DS_BITMAP_FREED_PLACES=0,

  ARAD_SW_DB_TCAM_AUX_DS_BITMAP_USED_PLACES=1,

  ARAD_SW_DB_TCAM_AUX_DS_LIST_PLACES=2

}ARAD_SW_DB_TCAM_AUX_DS_SELECT;




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

