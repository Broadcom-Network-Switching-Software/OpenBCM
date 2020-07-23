/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_TCAM_INCLUDED__

#define __ARAD_TCAM_INCLUDED__




#include <shared/swstate/sw_state.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_tcam.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/dpp_config_defs.h>






#define ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX       (2)


#define  ARAD_TCAM_ENTRY_MAX_LEN (15)


#define  ARAD_TCAM_NOF_PREFIXES (16)

#define  ARAD_TCAM_ACTION_MAX_LEN ((SOC_DPP_DEFS_MAX(TCAM_ACTION_WIDTH) * 4 + 31) / 32)

#define  ARAD_TCAM_ACCESS_PROFILE_INVALID (0x3F)

#define ARAD_TCAM_ACTION_SIZE_NOF_BITS        (2)


#define ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_ENABLE                 (1)

#if defined(INCLUDE_KBP)

#define  SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES       (SOC_DPP_DEFS_MAX(ELK_LOOKUP_PAYLOAD_NOF_BITS) / 8)
#define  SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit) (SOC_DPP_DEFS_GET(unit, elk_lookup_payload_nof_bits) / 8)
#define  SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_LONGS       (SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES / sizeof(uint32))
#define  SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_LONGS(unit) (SOC_DPP_TCAM_ACTION_ELK_KBP_GET_LEN_BYTES(unit) / sizeof(uint32))
#endif 





#define ARAD_TCAM_DB_ID_NUMBER_OF_DYNAMIC_DATABASES              (30) 
#define ARAD_TCAM_DB_ID_INVALID                                  (0)

#define ARAD_TCAM_REMOVE_IS_SW_ONLY                              (1)
#define ARAD_TCAM_REMOVE_KEEP_LOCATION                           (2)
#define ARAD_TCAM_REMOVE_KAPS_ADVANCED_MODE                      (4)

#define ARAD_PP_TCAM_STATIC_ACCESS_ID_BASE                       (1 )
#define ARAD_PP_TCAM_STATIC_ACCESS_ID_SRC_BIND_IPV6              (6 )
#define ARAD_PP_TCAM_STATIC_ACCESS_ID_OAM_IDENTIFICATION         (7 ) 
#define ARAD_PP_TCAM_STATIC_ACCESS_ID_SIP                        (9 )


#define ARAD_PP_ISEM_ACCESS_TCAM_BASE                                   (11)
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_FRR_DB_PROFILE                    (ARAD_PP_ISEM_ACCESS_TCAM_BASE)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_SPOOF_DB_PROFILE                  (12) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV4_TT_ETH_DB_PROFILE                 (13) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_TT_DB_PROFILE                     (14) 
#define ARAD_PP_ISEM_ACCESS_TCAM_ETH_INNER_OUTER_PCP_DB_PROFILE         (15) 
#define ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE              (16) 
#define ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE                 (17) 
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_EXPLICIT_NULL_VT_DB_PROFILE       (18) 
#define ARAD_PP_ISEM_ACCESS_TCAM_TT_TRILL_TRANSPARENT_SERVICE           (19) 
#define ARAD_PP_ISEM_ACCESS_TCAM_TT_DIP_COMPRESSION_DB_PROFILE          (20) 
#define ARAD_PP_ISEM_ACCESS_TCAM_TT_IPV6_SPOOF_COMPRESSION              (21) 
#define ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE                           (22) 
#define ARAD_PP_ISEM_ACCESS_TCAM_TEST2_PROFILE                          (23) 
#define ARAD_PP_ISEM_ACCESS_TCAM_DIP_SIP_VRF_PROFILE                    (24) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV4_MATCH_VT_DB_PROFILE               (25) 
#define ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_EFP                      (26) 
#define ARAD_PP_ISEM_ACCESS_TCAM_VRRP_VSI_DA_PROFILE                    (27)
#define ARAD_PP_ISEM_ACCESS_TCAM_CUSTOM_PP_LSP_ECMP_PROFILE             (28)
#define ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_PROFILE                    (29)
#define ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_DOU_TAGS_PROFILE           (30)
#define ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE                     (31)
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_CASCADE_VT_DB_PROFILE             (32) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_CASCADE_TT_DB_PROFILE             (33) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_AFTER_RECYCLE_CASCADE_VT_DB_PROFILE   (34) 
#define ARAD_PP_ISEM_ACCESS_TCAM_IPV6_AFTER_RECYCLE_TT_DB_PROFILE           (35)
#define ARAD_PP_ISEM_ACCESS_TCAM_GENERALIZED_RCH_PROFILE                (36)
#define ARAD_PP_ISEM_ACCESS_TCAM_L4B_L5L_PROFILE                        (37)
#define ARAD_PP_TT_LAST                                                 ARAD_PP_ISEM_ACCESS_TCAM_L4B_L5L_PROFILE + 10


#define ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_BASE                         (ARAD_PP_TT_LAST+1)
#define ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_END                          (ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_BASE + ARAD_TCAM_DB_ID_NUMBER_OF_DYNAMIC_DATABASES)

#define ARAD_PP_ISEM_ACCESS_TCAM_END                                 (ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_END)





#define ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT  (ARAD_PP_ISEM_ACCESS_TCAM_END + 0x1)
#define ARAD_PP_FP_DB_ID_TO_TCAM_DB(fp_id)  ((fp_id) + ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT)
#define ARAD_PP_FP_TCAM_DB_TO_FP_ID(tcam_db_id)  ((tcam_db_id) - ARAD_PP_FP_DB_ID_TO_TCAM_DB_SHIFT)

#define ARAD_TCAM_MAX_NOF_ACLS  128

#define ARAD_TCAM_MAX_NOF_LISTS   ARAD_PP_FP_DB_ID_TO_TCAM_DB(ARAD_TCAM_MAX_NOF_ACLS)

#define ARAD_TCAM_DB_LIST_KEY_SIZE                               (sizeof(uint32) )
#define ARAD_TCAM_DB_LIST_DATA_SIZE                              (sizeof(ARAD_TCAM_PRIO_LOCATION))










#define ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS       SOC_TMC_TCAM_BANK_ENTRY_SIZE_80_BITS
#define ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS      SOC_TMC_TCAM_BANK_ENTRY_SIZE_160_BITS
#define ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS      SOC_TMC_TCAM_BANK_ENTRY_SIZE_320_BITS
#define ARAD_TCAM_BANK_ENTRY_SIZE_DIRECT_TABLE  SOC_TMC_TCAM_BANK_ENTRY_SIZE_DIRECT_TABLE
#define ARAD_TCAM_NOF_BANK_ENTRY_SIZES          (SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES + 1)
typedef SOC_TMC_TCAM_BANK_ENTRY_SIZE            ARAD_TCAM_BANK_ENTRY_SIZE;

#define ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_DIRECT_TABLE_SMALL      7

#define ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_80      80
#define ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_160     160
#define ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_320     320


#define ARAD_SW_DB_ENTRY_SIZE_ID_TO_BITS(d_size_id)     \
  ((d_size_id == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS ) ? ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_80 :     \
  (d_size_id == ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS)  ? ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_160:     \
  (d_size_id == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)  ? ARAD_TCAM_BANK_KEY_SIZE_IN_BITS_320: 0) 

#define ARAD_SW_HW_ENTRY_SIZE_ID_TO_SW_ENTRY_SIZE(d_size_id) \
  ((d_size_id == 0x0 ) ? ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS : \
  (d_size_id == 0x1)  ? ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS:  \
  (d_size_id == 0x3)  ? ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS: ARAD_TCAM_NOF_BANK_ENTRY_SIZES) 

#define ARAD_SW_PREFIX_AND_TO_PREFIX_LENGTH(d_and) \
  ((d_and == 0x0 ) ? 4 :    \
   (d_and == 0x1)  ? 3 :    \
   (d_and == 0x3)  ? 2 :    \
   (d_and == 0x7)  ? 1 : 0) 

#define ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS                  SOC_TMC_TCAM_ACTION_SIZE_FIRST_20_BITS
#define ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS                 SOC_TMC_TCAM_ACTION_SIZE_SECOND_20_BITS
#define ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS                  SOC_TMC_TCAM_ACTION_SIZE_THIRD_20_BITS
#define ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS                  SOC_TMC_TCAM_ACTION_SIZE_FORTH_20_BITS
#define ARAD_TCAM_NOF_ACTION_SIZES                           SOC_TMC_NOF_TCAM_ACTION_SIZES
typedef SOC_TMC_TCAM_ACTION_SIZE                                 ARAD_TCAM_ACTION_SIZE;

#define ARAD_TCAM_BANK_OWNER_PMF_0            SOC_TMC_TCAM_BANK_OWNER_PMF_0
#define ARAD_TCAM_BANK_OWNER_PMF_1            SOC_TMC_TCAM_BANK_OWNER_PMF_1
#define ARAD_TCAM_BANK_OWNER_FLP_TCAM         SOC_TMC_TCAM_BANK_OWNER_FLP_TCAM
#define ARAD_TCAM_BANK_OWNER_FLP_TRAPS        SOC_TMC_TCAM_BANK_OWNER_FLP_TRAPS
#define ARAD_TCAM_BANK_OWNER_VT               SOC_TMC_TCAM_BANK_OWNER_VT
#define ARAD_TCAM_BANK_OWNER_TT               SOC_TMC_TCAM_BANK_OWNER_TT
#define ARAD_TCAM_BANK_OWNER_EGRESS_ACL       SOC_TMC_TCAM_BANK_OWNER_EGRESS_ACL
#define ARAD_TCAM_NOF_BANK_OWNERS             SOC_TMC_TCAM_NOF_BANK_OWNERS
typedef SOC_TMC_TCAM_BANK_OWNER               ARAD_TCAM_BANK_OWNER;

typedef SOC_TMC_TCAM_DB_PREFIX                                 ARAD_TCAM_DB_PREFIX;
typedef SOC_TMC_TCAM_DB_USER                                   ARAD_TCAM_DB_USER;
typedef SOC_TMC_TCAM_DB_INFO                                   ARAD_TCAM_DB_INFO;


typedef enum
{
    
    ARAD_TCAM_HW_BANK_OWNER_PMF  = 0x0,

    
    ARAD_TCAM_HW_BANK_OWNER_FLP  = 0x1,

    
    ARAD_TCAM_HW_BANK_OWNER_VTT  = 0x2,

    
    ARAD_TCAM_HW_BANK_OWNER_ERPP = 0x3,

    
    ARAD_TCAM_NOF_HW_BANK_OWNER

} ARAD_TCAM_HW_BANK_OWNER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 bits;
  
  uint32 length;

} ARAD_TCAM_PREFIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 bank_id;
  
  uint32 entry;

} ARAD_TCAM_LOCATION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_TCAM_LOCATION min;

  
  uint8 min_not_found;

  
  ARAD_TCAM_LOCATION max;

  
  uint8 max_not_found;

} ARAD_TCAM_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint8 valid;
  
  uint32 value[ARAD_TCAM_ENTRY_MAX_LEN];
  
  uint32 mask[ARAD_TCAM_ENTRY_MAX_LEN];

   
  uint8 is_for_update;
  
  uint8 is_inserted_top;
  
  uint8 is_keep_location;

} ARAD_TCAM_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 value[ARAD_TCAM_ACTION_MAX_LEN + 1];

#if defined(INCLUDE_KBP)
  
  uint8 elk_ad_value[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
#endif 
} ARAD_TCAM_ACTION;

typedef struct
{
  
  uint32 tcam_db_id;
  
  uint32 entry_id;
  
  uint32 priority;

} ARAD_TCAM_GLOBAL_LOCATION;

typedef struct
{
  
  uint32 entry_id_first;
  
  uint32 entry_id_last;

} ARAD_TCAM_PRIO_LOCATION;

typedef uint32
  (*ARAD_TCAM_MGMT_SIGNAL)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  user_data
  );

typedef enum
{
    
    ARAD_TCAM_SMALL_BANKS_ALLOW = 0,
    
    ARAD_TCAM_SMALL_BANKS_FORCE = 1,
    
    ARAD_TCAM_SMALL_BANKS_FORBID = 2
    
} ARAD_TCAM_SMALL_BANKS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 min_banks;
  
  ARAD_TCAM_BANK_ENTRY_SIZE entry_size;
  
  uint32 prefix_size;
  
  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx;
  
  ARAD_TCAM_MGMT_SIGNAL callback;
  
  uint32 user_data;
  
  ARAD_TCAM_BANK_OWNER bank_owner;
  
  uint8 is_direct;
  
  uint32  forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];

  
  ARAD_TCAM_SMALL_BANKS use_small_banks;

  
  uint8 no_insertion_priority_order;

  
  uint8 sparse_priorities;

} ARAD_TCAM_ACCESS_INFO;

typedef struct
{
  uint8                         valid;
  uint8                         has_direct_table;
  ARAD_TCAM_BANK_ENTRY_SIZE     entry_size;
  uint32                        nof_entries_free;
} ARAD_SW_DB_TCAM_BANK;

typedef struct
{
  uint8                     valid;
  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx;
  ARAD_TCAM_BANK_ENTRY_SIZE entry_size;
  uint32                    prefix_size;
  uint8                     bank_used[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
  uint8                     is_direct;
  ARAD_TCAM_SMALL_BANKS     use_small_banks;  
  uint8                     no_insertion_priority_order;  
  uint8                     sparse_priorities;  
  ARAD_TCAM_PREFIX          prefix;  
  uint32                    access_profile_id[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX];
  
  SHR_BITDCL                forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];
  
  uint32                    bank_nof_entries[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];  
} ARAD_SW_DB_TCAM_DB;

typedef struct
{
  SOC_SAND_OCC_BM_PTR       tcam_bank_entries_used[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS][2]; 
  SOC_SAND_SORTED_LIST_PTR  tcam_db_priorities[ARAD_TCAM_MAX_NOF_LISTS];
  SOC_SAND_OCC_BM_PTR       tcam_db_entries_used[ARAD_TCAM_MAX_NOF_LISTS][SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS]; 
  SOC_SAND_HASH_TABLE_PTR   entry_id_to_location; 
  ARAD_SW_DB_TCAM_BANK bank[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
   
  ARAD_SW_DB_TCAM_DB   tcam_db[ARAD_TCAM_MAX_NOF_LISTS];  
  PARSER_HINT_ARR ARAD_TCAM_LOCATION        *db_location_tbl; 
  PARSER_HINT_ARR ARAD_TCAM_GLOBAL_LOCATION *global_location_tbl; 
} ARAD_TCAM;










uint32
    arad_tcam_db_prio_list_priority_value_decode(
        SOC_SAND_IN  uint8      *data
    );

uint32
    arad_tcam_db_use_new_method_per_db_get(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  uint32                     tcam_db_id,
      SOC_SAND_OUT uint8                      *move_per_block
    );

void
    arad_tcam_db_location_decode_print(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN uint8  *buffer
    );

uint32
    arad_tcam_bank_entry_size_to_entry_count_get(
        SOC_SAND_IN  int                       unit,
        SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
        SOC_SAND_IN  uint32                    bank_ndx
     );

  uint32
    arad_tcam_db_data_structure_entry_get(
      SOC_SAND_IN  int prime_handle,
      SOC_SAND_IN  uint32 sec_handle,
      SOC_SAND_IN  uint8  *buffer,
      SOC_SAND_IN  uint32 offset,
      SOC_SAND_IN  uint32 len,
      SOC_SAND_OUT uint8  *data
    );
  
  uint32
    arad_tcam_db_data_structure_entry_set(
      SOC_SAND_IN    int prime_handle,
      SOC_SAND_IN    uint32 sec_handle,
      SOC_SAND_INOUT uint8  *buffer,
      SOC_SAND_IN    uint32 offset,
      SOC_SAND_IN    uint32 len,
      SOC_SAND_IN    uint8  *data
    );

  int32
    arad_tcam_db_priority_list_cmp_priority(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    );

  int32
    arad_tcam_db_priority_list_cmp_interlaced(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    );


uint32
  arad_tcam_db_entry_id_to_location_entry_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        tcam_db_id,
    SOC_SAND_IN  uint32        entry_id,
    SOC_SAND_IN  uint32        isLocation,
    SOC_SAND_OUT ARAD_TCAM_LOCATION *location,
    SOC_SAND_OUT uint8        *found
  );

uint32
  arad_tcam_db_entry_id_to_location_entry_add(
    SOC_SAND_IN int        unit,
    SOC_SAND_IN uint32        tcam_db_id,
    SOC_SAND_IN uint32        entry_id,
    SOC_SAND_IN ARAD_TCAM_LOCATION *location
  );

uint32
  arad_tcam_db_entry_id_to_location_entry_remove(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 entry_id
  );


uint32
  arad_tcam_resource_db_entries_find(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_OUT uint32     *db_nof_entries
  );
uint32
  arad_tcam_resource_db_entries_validate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
	  SOC_SAND_IN  uint32 address,
    SOC_SAND_OUT ARAD_TCAM_ENTRY *entry
  );
uint32
  arad_tcam_resource_db_entries_priority_validate(
      SOC_SAND_IN  int        unit,
      SOC_SAND_IN  uint32        tcam_db_id,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32        *entry_id,
      SOC_SAND_OUT uint32         *priority
  );

uint32
    arad_tcam_global_location_encode(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32              *global_location_id
    );

uint32
  arad_tcam_entry_rewrite(
      SOC_SAND_IN       int                          unit,
      SOC_SAND_IN       uint8                        entry_exists,      
      SOC_SAND_IN       uint32                       tcam_db_id,
      SOC_SAND_IN       ARAD_TCAM_LOCATION           *location,
      SOC_SAND_IN       ARAD_TCAM_BANK_ENTRY_SIZE    entry_size,
      SOC_SAND_INOUT    ARAD_TCAM_ENTRY              *entry
  );


uint32
  arad_tcam_bank_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  uint8                is_direct,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner
  );

uint32
  arad_tcam_bank_init_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  uint8                is_direct,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner
  );


uint32
  arad_tcam_db_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx,
    SOC_SAND_IN  uint8                     use_small_banks,
    SOC_SAND_IN  uint8                     no_insertion_priority_order,
    SOC_SAND_IN  uint8                     sparse_priorities,
    SOC_SAND_IN  uint8                     is_direct
  );

uint32
  arad_tcam_db_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx,
    SOC_SAND_IN  uint8                     is_direct
  );


uint32
  arad_tcam_db_destroy_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

uint32
  arad_tcam_db_destroy_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );


uint32
  arad_tcam_db_bank_add_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             tcam_db_id,
    SOC_SAND_IN  uint32             access_profile_array_id, 
    SOC_SAND_IN  uint32             bank_id,
    SOC_SAND_IN  uint8              is_direct, 
    SOC_SAND_IN  ARAD_TCAM_PREFIX   *prefix
  );

uint32
  arad_tcam_db_bank_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  ARAD_TCAM_PREFIX          *prefix
  );


uint32
  arad_tcam_db_bank_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

uint32
  arad_tcam_db_bank_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );


uint32
  arad_tcam_db_nof_banks_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT uint32               *nof_banks
  );

uint32
  arad_tcam_db_nof_banks_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );


uint32
  arad_tcam_db_bank_prefix_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT ARAD_TCAM_PREFIX          *prefix
  );

uint32
  arad_tcam_db_bank_prefix_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );


uint32
  arad_tcam_db_entry_size_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  );

uint32
  arad_tcam_db_entry_size_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );


uint32
  arad_tcam_db_direct_tbl_entry_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  uint32                   bank_id,
      SOC_SAND_IN  uint32                   address,
      SOC_SAND_IN  uint8                    valid,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE    action_bitmap_ndx,      
      SOC_SAND_INOUT  ARAD_TCAM_ACTION      *action
  );

uint32
  arad_tcam_db_direct_tbl_entry_set_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 bank_id,
    SOC_SAND_IN  uint32                 entry_id,
    SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE  action_bitmap_ndx,      
    SOC_SAND_IN  ARAD_TCAM_ACTION       *action
  );

uint32
  arad_tcam_db_direct_tbl_entry_get_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   tcam_db_id,
      SOC_SAND_IN  uint32                   bank_id,
      SOC_SAND_IN  uint32                   address,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE    action_bitmap_ndx,      
      SOC_SAND_INOUT  ARAD_TCAM_ACTION      *action,
      SOC_SAND_OUT  uint8                   *valid
  );





uint32
  arad_tcam_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY       *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION      *action,    
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_tcam_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32               priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION      *action
  );


uint32
  arad_tcam_db_entry_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              tcam_db_id,
    SOC_SAND_IN  uint32              entry_id,
    SOC_SAND_IN  uint8               hit_bit_clear, 
    SOC_SAND_OUT uint32              *priority,
    SOC_SAND_OUT ARAD_TCAM_ENTRY     *entry,
    SOC_SAND_OUT ARAD_TCAM_ACTION    *action,
    SOC_SAND_OUT uint8               *found,
    SOC_SAND_OUT uint8               *hit_bit
  );

uint32
  arad_tcam_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );
  

uint32
  arad_tcam_db_entry_search_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *key,
    SOC_SAND_OUT uint32               *entry_id,
    SOC_SAND_OUT uint8               *found
  );

uint32
  arad_tcam_db_entry_search_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *key
  );


uint32
  arad_tcam_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint8                is_bank_freed_if_no_entry, 
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8               is_keep_location
  );

uint32
  arad_tcam_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );


uint32
  arad_tcam_db_is_bank_used_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT uint8               *is_used
  );

uint32
  arad_tcam_db_is_bank_used_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  );

uint32
  arad_tcam_bank_owner_get_unsafe(
    SOC_SAND_IN    int               unit,
    SOC_SAND_IN    uint32               bank_id,
    SOC_SAND_OUT   ARAD_TCAM_BANK_OWNER *bank_owner
  );

uint32
    arad_tcam_bank_owner_verify(
      SOC_SAND_IN    int               unit,
      SOC_SAND_IN    uint32               bank_id,
      SOC_SAND_OUT   ARAD_TCAM_BANK_OWNER bank_owner
    );


uint32
  arad_tcam_managed_db_compress_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               tcam_db_id
  );

#ifdef ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_ENABLE
uint32
    arad_tcam_new_bank_move_entries(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint32            bank_id,
      SOC_SAND_IN  uint8             is_direct,
      SOC_SAND_IN  uint32            access_profile_array_id,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint8             is_inserted_top
  );
#endif 

void
  ARAD_TCAM_PREFIX_clear(
    SOC_SAND_OUT ARAD_TCAM_PREFIX *info
  );

void
  ARAD_TCAM_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_LOCATION *info
  );

void
  ARAD_TCAM_GLOBAL_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_GLOBAL_LOCATION *info
  );

void
  ARAD_TCAM_PRIO_LOCATION_clear(
    SOC_SAND_OUT ARAD_TCAM_PRIO_LOCATION *info
  );

void
  ARAD_TCAM_RANGE_clear(
    SOC_SAND_OUT ARAD_TCAM_RANGE *info
  );

void
  ARAD_TCAM_ENTRY_clear(
    SOC_SAND_OUT ARAD_TCAM_ENTRY *info
  );

void
  ARAD_TCAM_ACTION_clear(
    SOC_SAND_OUT ARAD_TCAM_ACTION *info
  );

uint32
  ARAD_TCAM_PREFIX_verify(
    SOC_SAND_IN  ARAD_TCAM_PREFIX *info
  );

uint32
  ARAD_TCAM_LOCATION_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_TCAM_LOCATION *info
  );

uint32
  ARAD_TCAM_GLOBAL_LOCATION_verify(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION *info
  );

uint32
  ARAD_TCAM_PRIO_LOCATION_verify(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_PRIO_LOCATION *info
  );


uint32
  ARAD_TCAM_RANGE_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_TCAM_RANGE *info
  );

uint32
  ARAD_TCAM_ENTRY_verify(
    SOC_SAND_IN  ARAD_TCAM_ENTRY *info
  );

uint32
  ARAD_TCAM_ACTION_verify(
    SOC_SAND_IN  ARAD_TCAM_ACTION *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE enum_val
  );

void
  ARAD_TCAM_PREFIX_print(
    SOC_SAND_IN  ARAD_TCAM_PREFIX *info
  );

void
  ARAD_TCAM_LOCATION_print(
    SOC_SAND_IN  ARAD_TCAM_LOCATION *info
  );

void
  ARAD_TCAM_GLOBAL_LOCATION_print(
    SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION *info
  );

void
  ARAD_TCAM_RANGE_print(
    SOC_SAND_IN  ARAD_TCAM_RANGE *info
  );

void
  ARAD_TCAM_ENTRY_print(
    SOC_SAND_IN  ARAD_TCAM_ENTRY *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

