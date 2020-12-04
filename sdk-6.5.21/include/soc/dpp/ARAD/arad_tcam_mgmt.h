/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_TCAM_MGMT_INCLUDED__

#define __ARAD_TCAM_MGMT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_framework.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_tcam.h>






#define ARAD_TCAM_NOF_CYCLES  1

#define ARAD_TCAM_MAX_USERS_PER_BANK  ARAD_TCAM_NOF_CYCLES

#define ARAD_TCAM_NOF_ACL_KEYS_PER_CYCLE  2

#define ARAD_TCAM_NOF_ACCESS_PROFILE_IDS                    (48)


#define ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE        1
#ifdef ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_WA_ENABLE
#define ARAD_TCAM_EGQ_DUMMY_ACCESS_PROFILE_NO_LOOKUP        (ARAD_TCAM_NOF_ACCESS_PROFILE_IDS - 1)
#endif 


#define ARAD_TCAM_NOF_ACL_KEYS  (ARAD_TCAM_NOF_CYCLES * ARAD_TCAM_NOF_ACL_KEYS_PER_CYCLE)

#define ARAD_TCAM_MGMT_NOF_PRIO_ENC 2

#define ARAD_TCAM_MGMT_BANK_MSB  1
#define ARAD_TCAM_MGMT_BANK_LSB  0
#define ARAD_TCAM_MGMT_OFFSET_MSB  11
#define ARAD_TCAM_MGMT_OFFSET_LSB  2

#define ARAD_TCAM_ENTRY_SIZE_MAX                                 (ARAD_TCAM_NOF_BANK_ENTRY_SIZES-1)
#define ARAD_TCAM_PREFIX_SIZE_MAX                                (4)
#define ARAD_TCAM_ACTION_SIZE_MAX                                ((1 << ARAD_TCAM_NOF_ACTION_SIZES)-1)
#define ARAD_TCAM_BANK_OWNER_MAX                                 (ARAD_TCAM_NOF_BANK_OWNERS-1)









typedef struct
{
 
  uint16 offset;
  
  uint8 bank_id;
}ARAD_TCAM_PLACE_ID;












uint32
  arad_tcam_access_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,   
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_tcam_access_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info
  );

uint32
  arad_tcam_access_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

uint32 
  arad_tcam_managed_db_direct_table_bank_add(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );

uint32 
  arad_tcam_managed_db_direct_table_bank_remove(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );


uint32
  arad_tcam_managed_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8                is_single_bank,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY        *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION       *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_tcam_managed_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION          *action
  );


uint32
  arad_tcam_managed_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );

uint32
  arad_tcam_managed_db_entry_remove_keep_loc_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );

uint32
  arad_tcam_managed_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  );


uint32
  arad_tcam_managed_db_entry_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8                is_single_bank,
    SOC_SAND_OUT  ARAD_TCAM_ENTRY     *entry,
    SOC_SAND_OUT  ARAD_TCAM_ACTION    *action
  );

uint32
  arad_tcam_managed_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY           *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION          *action
  );

uint32
  arad_tcam_access_pd_profile_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id,
    SOC_SAND_OUT uint32               *pd_profile_id
  );

uint32
  arad_tcam_access_pd_profile_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id
  );

uint32
  arad_tcam_access_profile_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 access_profile_id
  );

void
  arad_ARAD_TCAM_ENTRY_clear(
    SOC_SAND_OUT ARAD_TCAM_ENTRY *info
  );
uint32
  ARAD_TCAM_ACCESS_INFO_verify(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *info
  );

void
  arad_ARAD_TCAM_ACCESS_INFO_clear(
    SOC_SAND_OUT  ARAD_TCAM_ACCESS_INFO *info
  );

void
  arad_ARAD_TCAM_ACCESS_INFO_clear_and_update( SOC_SAND_OUT ARAD_TCAM_ACCESS_INFO *info, ARAD_TCAM_BANK_OWNER owner, int entry_size, int user_data);

#if ARAD_DEBUG

void
  arad_ARAD_TCAM_ENTRY_print(
    SOC_SAND_IN ARAD_TCAM_ENTRY *info
  );


#endif 

uint32
  arad_tcam_mgmt_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );

int32
  arad_tcam_mgmt_cmp_key(
    SOC_SAND_IN uint8             *buffer1,
    SOC_SAND_IN uint8             *buffer2,
    uint32                    size
  );

#if ARAD_DEBUG_IS_LVL1

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

