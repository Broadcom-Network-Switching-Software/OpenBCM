#include <shared/bsl.h>

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

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM




#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>





#define ARAD_TCAM_NOF_MIN_BANKS                                  (1)
#define ARAD_TCAM_MGMT_ACCESS_PROFILE_ID_MAX                     (ARAD_TCAM_NOF_ACCESS_PROFILE_IDS-1)
#define ARAD_TCAM_MGMT_TCAM_DB_ID_MAX                            (SOC_SAND_UINT_MAX)
#define ARAD_TCAM_MGMT_MIN_BANKS_MAX                             (ARAD_TCAM_NOF_MIN_BANKS)
#define ARAD_TCAM_MGMT_USER_DATA_MAX                             (SOC_SAND_U32_MAX)
#define ARAD_TCAM_MGMT_ENTRY_ID_MAX                              (SOC_SAND_UINT_MAX)





















STATIC
  uint32
    arad_tcam_managed_bank_prefix_is_free(
      SOC_SAND_IN  int              unit,
      SOC_SAND_IN  uint32           bank_id,
      SOC_SAND_IN  ARAD_TCAM_PREFIX *prefix,
      SOC_SAND_OUT uint8            *is_free
    )
{
  uint8
    base,
    ndx;
  uint32
    shift,
    tcam_db_id;
  soc_error_t
    rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *is_free = TRUE;
  shift = 4 - prefix->length;
  base = (uint8) (prefix->bits << shift);
  for (ndx = 0; *is_free && (ndx < (uint32) (0x1 << shift)); ++ndx)
  {
    rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.get(unit, bank_id, base | ndx, &tcam_db_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    if (tcam_db_id != ARAD_TCAM_MAX_NOF_LISTS)
    {
      *is_free = FALSE;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_bank_prefix_is_free()", 0, 0);  
}

STATIC
  uint32
    arad_tcam_managed_bank_prefix_find(
      SOC_SAND_IN  int              unit,
      SOC_SAND_IN  uint32           bank_id,
      SOC_SAND_IN  uint32           prefix_size,
      SOC_SAND_OUT ARAD_TCAM_PREFIX *prefix,
      SOC_SAND_OUT uint8            *success
    )
{
  uint32
    lsb_ndx,
    msb_ndx;
  uint32
    shift,
    nof_lsb,
    nof_msb,
    tcam_db_id;
  soc_error_t
    rv;
  uint8
    lsb_free;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *success = FALSE;
  
  shift   = 4 - prefix_size;
  nof_lsb = 1 << shift;
  nof_msb = 1 << prefix_size;
  for (msb_ndx = 0; !(*success) && (msb_ndx < nof_msb); ++msb_ndx)
  {
    lsb_free = TRUE;
    for (lsb_ndx = 0; lsb_free && (lsb_ndx < nof_lsb); ++lsb_ndx)
    {
      rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.get(
              unit,
              bank_id,
              (msb_ndx << shift) | lsb_ndx,
              &tcam_db_id
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

      if (tcam_db_id != ARAD_TCAM_MAX_NOF_LISTS)
      {
        lsb_free = FALSE;
      }
    }
    if (lsb_free)
    {
      prefix->bits   = (uint8) msb_ndx;
      prefix->length = prefix_size;
      *success = TRUE;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_bank_prefix_find()", 0, 0);  
}

STATIC
  uint32
    arad_tcam_managed_bank_prefix_grab(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN uint32           bank_id,
      SOC_SAND_IN ARAD_TCAM_PREFIX *prefix,
      SOC_SAND_IN uint32           tcam_db_id
    )
{
  uint8
    base,
    ndx;
  uint32
    shift;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  shift = 4 - prefix->length;
  base = (uint8) (prefix->bits << shift);
  for (ndx = 0; ndx < (0x1 << shift); ++ndx)
  {
    rv = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.prefix_db.set(
      unit,
      bank_id,
      base | ndx,
      tcam_db_id
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_bank_prefix_grab()", 0, 0);  
}


STATIC
  uint32
    arad_tcam_bank_block_owner_get(
      SOC_SAND_IN    int                   unit,
      SOC_SAND_IN    uint32                bank_ndx,
      SOC_SAND_OUT   ARAD_TCAM_BANK_OWNER *bank_block_owner,
      SOC_SAND_OUT   uint8                *block_owner_found
      )
{
  char *tcam_bank_block_owner;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tcam_bank_block_owner = soc_property_port_num_get_str(unit, bank_ndx, spn_TCAM_BANK_BLOCK_OWNER);
  if (tcam_bank_block_owner == NULL)
  {
      *bank_block_owner = ARAD_TCAM_NOF_BANK_OWNERS;
      *block_owner_found = FALSE;
      ARAD_DO_NOTHING_AND_EXIT;
  }

  *block_owner_found = TRUE;
  if (sal_strcmp(tcam_bank_block_owner, "FLP_0") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
  } else if (sal_strcmp(tcam_bank_block_owner, "FLP_1") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_FLP_TRAPS;
  } else if (sal_strcmp(tcam_bank_block_owner, "VTT_0") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_VT;
  } else if (sal_strcmp(tcam_bank_block_owner, "VTT_1") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_TT;
  } else if (sal_strcmp(tcam_bank_block_owner, "IPMF_0") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_PMF_0;
  } else if (sal_strcmp(tcam_bank_block_owner, "IPMF_1") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_PMF_1;
  } else if (sal_strcmp(tcam_bank_block_owner, "EPMF") == 0) {
      *bank_block_owner = ARAD_TCAM_BANK_OWNER_EGRESS_ACL;
  } else {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
"Unit %d: Unexpected property value (\"%s\") for tcam_bank_block_owner_%d"), unit, tcam_bank_block_owner, bank_ndx));
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_BANK_OWNER_MISMATCH_ERR, 5, exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_bank_block_owner_get()", 0, 0);  
}

  
STATIC
  uint32
    arad_tcam_access_profile_bank_free_get(
      SOC_SAND_IN    int               unit,
      SOC_SAND_IN    uint32               tcam_db_id,
      SOC_SAND_IN    uint32               bank_ndx, 
      SOC_SAND_IN    ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN    ARAD_TCAM_BANK_OWNER bank_owner,
      SOC_SAND_IN    uint32               prefix_size,
      SOC_SAND_IN    uint8                force_prefix,
      SOC_SAND_IN    uint8                is_direct,
      SOC_SAND_IN    uint8                reserved_only,
      SOC_SAND_INOUT ARAD_TCAM_PREFIX      *prefix,
      SOC_SAND_INOUT ARAD_TCAM_ACTION_SIZE *action_ndx,
      SOC_SAND_OUT   uint8                 *success
    )
{
    uint32
        tcam_db_other_ndx,
        nof_entries,
        res;
    ARAD_TCAM_BANK_OWNER
        curr_bank_owner,
        bank_block_owner = ARAD_TCAM_NOF_BANK_OWNERS;
    ARAD_TCAM_ACTION_SIZE
        action_bmp;
    ARAD_TCAM_BANK_ENTRY_SIZE 
        bank_entry_size;
    uint8 
        direct_table_exists = FALSE,
        action_tbl_used[2] = {0,0},
        is_forbidden,
        is_used,
        has_direct_table,
        valid,
        is_free,
        block_owner_found = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   
   if (ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS == entry_size) {
       *action_ndx = (ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS);
   }
   
    *success = FALSE;

    res = arad_tcam_bank_block_owner_get(
            unit, 
            bank_ndx,
            &bank_block_owner,
            &block_owner_found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
    if (reserved_only && !block_owner_found)
    {
      
      ARAD_DO_NOTHING_AND_EXIT;
    }
    if (block_owner_found && bank_block_owner != bank_owner)
    {
      
      ARAD_DO_NOTHING_AND_EXIT;
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_ndx, &valid);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

    if (!valid)
    {
      
        if (!force_prefix) {
          prefix->bits   = 0;
          prefix->length = prefix_size;
        }

      *success = TRUE;
    }
    
    else            
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.nof_entries_free.get(unit, bank_ndx, &nof_entries);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

        
        if (nof_entries == 0) {
            *success = FALSE;
            ARAD_DO_NOTHING_AND_EXIT;
        }

        
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(unit, bank_ndx, &bank_entry_size);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.get(unit, bank_ndx, &has_direct_table);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

        if ((bank_entry_size == entry_size) 
            || (is_direct 
                && (ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS != bank_entry_size))
            || (has_direct_table
                && (ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS != entry_size))) {

          res = arad_tcam_bank_owner_get_unsafe(
                  unit, 
                  bank_ndx,
                  &curr_bank_owner
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

          if (bank_owner == curr_bank_owner)
          {
            
            if(is_direct) {
              *success = TRUE;
            }
            else if (force_prefix)
            {
              res = arad_tcam_managed_bank_prefix_is_free(unit, bank_ndx, prefix, &is_free);
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
              if (is_free)
              {
                *success = TRUE;
              }
            }
            else
            {
              res = arad_tcam_managed_bank_prefix_find(
                      unit,
                      bank_ndx,
                      prefix_size,
                      prefix,
                      success
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            }
         
            
            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.has_direct_table.get(unit, bank_ndx, &direct_table_exists);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

            if(direct_table_exists && is_direct) {
                *success = FALSE;
                ARAD_DO_NOTHING_AND_EXIT;
            }

            
            for (tcam_db_other_ndx = 0; tcam_db_other_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_other_ndx++) 
            {
                res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_other_ndx, bank_ndx, &is_used);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

                if (is_used) 
                {
                    res = arad_sw_db_tcam_db_forbidden_dbs_get(unit, tcam_db_other_ndx, tcam_db_id, &is_forbidden);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                    
                    if (is_forbidden) {
                        *success = FALSE;
                    } 
                    
                    else if((ARAD_TCAM_BANK_OWNER_PMF_0 == bank_owner)
                            || (ARAD_TCAM_BANK_OWNER_PMF_1 == bank_owner)){
                        
                        
                        if (!is_direct && !direct_table_exists) {
                            continue;
                        }
                        
                        else if((direct_table_exists && (ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS == entry_size)) 
                                || (is_direct && (ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS == bank_entry_size))
                                || (is_direct && direct_table_exists)) {
                            *success = FALSE;
                        }
                        else {
                            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(unit, tcam_db_other_ndx, &action_bmp);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

                            
                            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit,tcam_db_other_ndx, &bank_entry_size);
                            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);

                            if((ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS == bank_entry_size)
                               && ((bank_ndx % 2) == 1)) {
                                action_tbl_used[0] |= (action_bmp & ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS) ? TRUE : FALSE;
                                action_tbl_used[1] |= (action_bmp & ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS) ? TRUE : FALSE;
                            }
                            
                            else {
                                action_tbl_used[0] |= (action_bmp & ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS) ? TRUE : FALSE;
                                action_tbl_used[1] |= (action_bmp & ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS) ? TRUE : FALSE;
                            }

                            
                            if(is_direct && action_tbl_used[0] && action_tbl_used[1]) {
                                *success = FALSE; 
                            }
                            else {
                                
                                
				
				
                                if(!is_direct &&  direct_table_exists) {
                                    *action_ndx = ((ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS == entry_size) && (bank_ndx % 2) == 1)
                                        ? (ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS | ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS)
                                        : (ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS | ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS);
                                }
                                else if(!action_tbl_used[0]) {
                                    *action_ndx = ((ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS == entry_size) && (bank_ndx % 2) == 1) 
                                        ? ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS : ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
                                } 
                                else if(!action_tbl_used[1]) {
                                    *action_ndx = ((ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS == entry_size) && (bank_ndx % 2) == 1)
                                        ? ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS : ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
                                }
                            }
                        } 
                    }
                }
            }
          }
        }
    }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_bank_free_get()", 0, 0);  
}
  
  
  
STATIC
  uint32
    arad_tcam_access_profile_bank_find(
      SOC_SAND_IN    int                 unit,
      SOC_SAND_IN    uint32                 tcam_db_id,
      SOC_SAND_IN    uint32                 access_profile_array_id, 
      SOC_SAND_IN    ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN    ARAD_TCAM_BANK_OWNER bank_owner,
      SOC_SAND_IN    uint32                 prefix_size,
      SOC_SAND_IN    uint8                  force_prefix,
      SOC_SAND_IN    uint8                  is_direct,
      SOC_SAND_INOUT ARAD_TCAM_PREFIX       *prefix,
      SOC_SAND_INOUT ARAD_TCAM_ACTION_SIZE  *action_ndx,
      SOC_SAND_OUT   uint32                 *bank_id,
      SOC_SAND_OUT   uint8                  *bank_found
    )
{
  uint8
    is_found,
    two_banks_to_find,
    success = FALSE,
    is_used;
  uint32
    res,
    bank_start,
    bank_ndx,
    bank_other_ndx,
    nof_banks,
    iteration;
  ARAD_TCAM_SMALL_BANKS
   use_small_banks;
  ARAD_TCAM_ACTION_SIZE 
   action_ndx_needed = *action_ndx,
   action_ndx_tmp;
   

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.use_small_banks.get(unit, tcam_db_id, &use_small_banks);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  bank_start = (ARAD_TCAM_SMALL_BANKS_FORCE == use_small_banks) ? SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks) : 0;
  nof_banks = (ARAD_TCAM_SMALL_BANKS_FORCE == use_small_banks) ? SOC_DPP_DEFS_GET(unit, nof_tcam_small_banks) 
      : ((ARAD_TCAM_SMALL_BANKS_FORBID == use_small_banks) ? SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks) : SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit));

  two_banks_to_find = ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && (access_profile_array_id == 0))? 1:0;
  for (iteration = 0; iteration < 2; ++iteration)
  {
    
    uint8 reserved_only = (iteration == 0)? TRUE : FALSE;
    for (bank_ndx = bank_start; bank_ndx < (bank_start + nof_banks); ++bank_ndx)
    {
       action_ndx_tmp = action_ndx_needed;
       *action_ndx = 0;

       
        if (two_banks_to_find && (bank_ndx % 2 != 0)) {
            continue;
        }
        else if (two_banks_to_find) {
            res = arad_tcam_access_profile_bank_free_get(
                      unit,
                      tcam_db_id,
                      bank_ndx + 1, 
                      entry_size,
                      bank_owner,
                      prefix_size,
                      force_prefix,
                      is_direct,
                      reserved_only,
                      prefix,
                      &action_ndx_tmp,
                      &success
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
            if (success == FALSE) {
                continue;
            }
            else{
                *action_ndx |= action_ndx_tmp;
            }
        }
        else if (access_profile_array_id == 1) {
            
            
            is_found = FALSE;
            for (bank_other_ndx = bank_start; bank_other_ndx < (bank_start + nof_banks); ++bank_other_ndx)
            {
                if ((bank_other_ndx % 2) == 0) {
                    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_other_ndx, &is_used);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);
                    if(is_used) {
                        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_other_ndx + 1, &is_used);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);
                        if(!is_used) {
                            
                            is_found = TRUE;
                            break;
                        }
                    }
                }
            }
            success = is_found;
            bank_ndx = bank_other_ndx + 1;
            break;
        }

        res = arad_tcam_access_profile_bank_free_get(
                  unit,
                  tcam_db_id,
                  bank_ndx, 
                  entry_size,
                  bank_owner,
                  prefix_size,
                  force_prefix,
                  is_direct,
                  reserved_only,
                  prefix,
                  &action_ndx_tmp,
                  &success
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (success) {
            *action_ndx |= action_ndx_tmp;
            if(action_ndx_needed > *action_ndx)
            {
                success = FALSE;
            }
            else {
                break;
            }
        }
    }
    if (success) {
        break;
    }
  }

  *bank_id = bank_ndx;
  *bank_found = success;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_bank_find()", 0, 0);  
}

STATIC
  uint32
    arad_tcam_action_bitmap_to_hw_action_field(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_TCAM_ACTION_SIZE action_bitmap_ndx,
      SOC_SAND_OUT uint32                *action_hw
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *action_hw = 0;

  if (action_bitmap_ndx & ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS
      || action_bitmap_ndx & ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS) {
    *action_hw += 1;
  } 

  if (action_bitmap_ndx & ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS
      || action_bitmap_ndx & ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS) {
    *action_hw += 2;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_action_bitmap_to_hw_action_field()", 0, 0);
}

STATIC
  uint32
    arad_tcam_entry_size_to_hw_entry_size_field(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_OUT uint32                *entry_size_hw
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *entry_size_hw = 0;

  switch (entry_size)
  {
  case ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS:
    *entry_size_hw = 0;
    break;
  case ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS:
    *entry_size_hw = 1;
    break;
  case ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS:
    *entry_size_hw = 3;
    break;
  default:
    
    *entry_size_hw = 2;
    break;
  }
  
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_entry_size_to_hw_entry_size_field()", entry_size, 0);
}


STATIC
  uint32
    arad_tcam_access_profile_bank_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            access_profile_id,
      SOC_SAND_IN  uint32            access_profile_array_id, 
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint8             is_direct,
      SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner,
      SOC_SAND_IN  uint8          is_inserted_top,
      SOC_SAND_IN  uint8          is_single_bank,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id,
    pd_profile_id,
    prefix_size,    
    action_hw,
    nof_dbs;
  uint8
    uniform_prefix = TRUE,
      already_has_prefix,
    bank_found = 0,
    is_used;
  ARAD_TCAM_PREFIX
    prefix;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size,
    entry_size_1;
  ARAD_TCAM_ACTION_SIZE
    action_bitmap_ndx_one_bank,
    action_bitmap_ndx,
    action_bitmap_ndx_tmp;
  ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA
    pd_tbl_data;
  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA
    access_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&pd_tbl_data, ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA, 1);
  ARAD_CLEAR(&access_tbl_data, ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA, 1);

  
  already_has_prefix = FALSE;
  ARAD_TCAM_PREFIX_clear(&prefix);
  if (uniform_prefix)
  {
    for (bank_id = 0; !already_has_prefix && (bank_id < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit)); ++bank_id)
    {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_id, &is_used);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

      if (is_used)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix.get(
          unit,
          tcam_db_id,
          &prefix
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);
        already_has_prefix = TRUE;
      }
    }
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
          unit,
          tcam_db_id,
          &entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.prefix_size.get(
          unit,
          tcam_db_id,
          &prefix_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.get(
          unit, 
          tcam_db_id,
          &action_bitmap_ndx_tmp
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  action_bitmap_ndx = action_bitmap_ndx_tmp;
  res = arad_tcam_access_profile_bank_find(
          unit,
          tcam_db_id,
          access_profile_array_id,
          entry_size,
          bank_owner,
          prefix_size,
          uniform_prefix && already_has_prefix,
          is_direct,
          &prefix,
          &action_bitmap_ndx,
          &bank_id,
          &bank_found                  
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (bank_found)
  {
      
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && is_single_bank && bank_id > 7) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Unit %d: DB %d: Bank ID allocated (%d) incompatible with key A (SINGLE mode)"), unit, tcam_db_id, bank_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_NDX_OUT_OF_RANGE_ERR, 45, exit);
    }
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.valid.get(unit, bank_id, &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);      
    if (!is_used)
    {
      
      res = arad_tcam_bank_init_unsafe(
              unit,
              bank_id,
              is_direct,
              entry_size,
              bank_owner
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);      
    }
    else 
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(unit, bank_id, &entry_size_1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
        if(entry_size_1 == ARAD_TCAM_NOF_BANK_ENTRY_SIZES
            && !is_direct) {
            
            res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.set(unit, bank_id, entry_size);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
        }
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.nof_dbs.get(
            unit,
            bank_id,
            &nof_dbs
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.banks.nof_dbs.set(
      unit,
      bank_id,
      nof_dbs + 1
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

    if(!is_direct) {
        res = arad_tcam_managed_bank_prefix_grab(
                unit,
                bank_id,
                &prefix,
                tcam_db_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
    }

    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(unit, tcam_db_id, bank_id, &is_used); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 9, exit);
    if (!is_used)
    {
      res = arad_tcam_db_bank_add_unsafe(
              unit,
              tcam_db_id,
              access_profile_array_id,
              bank_id,
              is_direct,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    
    
    res = arad_pp_ihb_tcam_access_profile_tbl_read_unsafe(
            unit,
            access_profile_id,
            &access_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    

    if(entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS) {
        if(action_bitmap_ndx_tmp == 1 || action_bitmap_ndx_tmp == 2) {
            action_bitmap_ndx = 3;
        }
    }
    else if(access_profile_array_id == 1) {
        action_bitmap_ndx = action_bitmap_ndx_tmp;
    }

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.action_bitmap_ndx.set(
           unit, 
           tcam_db_id, 
           action_bitmap_ndx
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

    
    action_bitmap_ndx_one_bank = 0;
    SHR_BITCOPY_RANGE(&action_bitmap_ndx_one_bank, 0, &action_bitmap_ndx, (2 * access_profile_array_id), 2);
    res = arad_tcam_action_bitmap_to_hw_action_field(
            unit,
            action_bitmap_ndx_one_bank,
            &action_hw
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


    SHR_BITCOPY_RANGE(&access_tbl_data.action_bitmap, bank_id*ARAD_TCAM_ACTION_SIZE_NOF_BITS, &action_hw, 0, ARAD_TCAM_ACTION_SIZE_NOF_BITS);
    if ((entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS) && (access_profile_array_id == 0)) {
        
        access_tbl_data.prefix_and = (1 << ARAD_TCAM_PREFIX_SIZE_MAX) - 1;
        access_tbl_data.prefix_or = 0;
    }
    else {
        
        access_tbl_data.prefix_and = (1 << (ARAD_TCAM_PREFIX_SIZE_MAX - prefix.length)) - 1;
        
        access_tbl_data.prefix_or = (prefix.bits << (ARAD_TCAM_PREFIX_SIZE_MAX - prefix.length));
    }

    res = arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
            unit,
            access_profile_id,
            &access_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    pd_profile_id = access_profile_id; 
    res = arad_pp_ihb_tcam_pd_profile_tbl_read_unsafe(
            unit,
            pd_profile_id,
            &pd_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    if (action_hw) {
        SHR_BITSET(&pd_tbl_data.bitmap,bank_id);
    }

    res = arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe(
            unit,
            pd_profile_id,
            &pd_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    

#ifdef ARAD_TCAM_BANK_INIT_MOVE_ENTRIES_ENABLE
    
    res = arad_tcam_db_use_new_method_per_db_get(unit, tcam_db_id, &is_used);
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
    if (is_used && (access_profile_array_id == 0)) {
        res = arad_tcam_new_bank_move_entries(
                unit,
                tcam_db_id,
                bank_id,
                is_direct,
                access_profile_array_id,
                entry_size,
                is_inserted_top
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
#endif 
  }

  *success = bank_found ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_bank_add()", access_profile_id, tcam_db_id);
}


STATIC uint32
  arad_tcam_access_profile_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  ARAD_TCAM_MGMT_SIGNAL callback,
    SOC_SAND_IN  uint32                user_data
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_ACCESS_PROFILE_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(access_profile_id, ARAD_TCAM_MGMT_ACCESS_PROFILE_ID_MAX, ARAD_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_MGMT_TCAM_DB_ID_MAX, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(min_banks, ARAD_TCAM_MGMT_MIN_BANKS_MAX, ARAD_TCAM_MGMT_MIN_BANKS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(user_data, ARAD_TCAM_MGMT_USER_DATA_MAX, ARAD_TCAM_MGMT_USER_DATA_OUT_OF_RANGE_ERR, 60, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_create_verify()", 0, 0);
}


STATIC uint32
  arad_tcam_access_profile_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id,
    SOC_SAND_IN  uint32               access_profile_array_id, 
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  ARAD_TCAM_BANK_OWNER bank_owner,
    SOC_SAND_IN  uint8                is_direct,
    SOC_SAND_IN  ARAD_TCAM_MGMT_SIGNAL     callback,
    SOC_SAND_IN  uint32                user_data,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_size_hw,
    ndx;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;
  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA
    access_tbl_data;
  uint8
    is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_ACCESS_PROFILE_CREATE_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_CLEAR(&access_tbl_data, ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA, 1);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.valid.get(unit, access_profile_id, &is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  if (is_valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Access profile id %d - Failed to create access profile. The profile already exists.\n\r"),
               unit, access_profile_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_ACCESS_PROFILE_ALREADY_EXISTS_ERR, 10, exit);
  }

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &is_valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  if (!is_valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d - Failed to create access profile. The data base doesn\'t exist.\n\r"),
               unit, tcam_db_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 20, exit);
  }

  
  res = arad_tcam_access_profile_create_verify(
          unit,
          access_profile_id,
          tcam_db_id,
          min_banks,
          callback,
          user_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.set(
    unit,
    tcam_db_id,
    access_profile_array_id,
    access_profile_id
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.min_banks.set(
    unit,
    access_profile_id,
    min_banks
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.callback.set(
    unit,
    access_profile_id,
    callback
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.user_data.set(
    unit,
    access_profile_id,
    user_data
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.set(
    unit,
    access_profile_id,
    bank_owner
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

  *success = SOC_SAND_SUCCESS;
  for (ndx = 0; (*success == SOC_SAND_SUCCESS) && (ndx < min_banks); ++ndx)
  {
    res = arad_tcam_access_profile_bank_add(
            unit,
            access_profile_id,
            access_profile_array_id,
            tcam_db_id,
            is_direct,
            bank_owner,
            FALSE, 
            0,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (*success == SOC_SAND_SUCCESS)
  {    
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
            unit,
            tcam_db_id,
            &entry_size
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

    
    res = arad_pp_ihb_tcam_access_profile_tbl_read_unsafe(
            unit,
            access_profile_id,
            &access_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


    res = arad_tcam_entry_size_to_hw_entry_size_field(
            unit,
            entry_size,
            &entry_size_hw
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    access_tbl_data.is_direct = is_direct;
    access_tbl_data.key_size = entry_size_hw; 

    res = arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
            unit,
            access_profile_id,
            &access_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
     
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.valid.set(
      unit,
      access_profile_id,
      TRUE
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 75, exit);

    
    res = callback(
            unit,
            user_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_create_unsafe()", 0, 0);
}

uint32
  arad_tcam_access_profile_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 access_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    pd_profile_id;
  ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA
    pd_tbl_data;
  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA
    access_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(&pd_tbl_data, ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA, 1);
  ARAD_CLEAR(&access_tbl_data, ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA, 1);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.valid.set(
    unit,
    access_profile_id,
    FALSE
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  
  res = arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
          unit,
          access_profile_id,
          &access_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  pd_profile_id = access_profile_id;
  res = arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe(
            unit,
            pd_profile_id,
            &pd_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_destroy_unsafe()", 0, 0);
}


uint32
  arad_tcam_access_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,   
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    tcam_db_other_ndx,
    nof_access_profiles,
    access_profile_array_id,
    access_profile_ndx,
    access_profile[2] = {ARAD_TCAM_NOF_ACCESS_PROFILE_IDS, ARAD_TCAM_NOF_ACCESS_PROFILE_IDS},
    res = SOC_SAND_OK;
  char
    string_to_print[100];
  uint8
    tcam_db_created = 0,
    access_profile_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(tcam_info);
  SOC_SAND_CHECK_NULL_INPUT(success);
  *success = SOC_SAND_SUCCESS;

  
  LOG_DEBUG(BSL_LS_SOC_TCAM,
            (BSL_META_U(unit,
                        "    "
                        "TCAM profile: create: "
                        "TCAM id %d, "
                        "action DB bmp 0x%08x, "
                        "Prefix size %d, "
                        "owner %d (fp,flp,vtt,egress), "
                        "entry_size %s, "
                        "use small banks %d, "
                        "sparse priorities %d, "
                        "entry insertion rule not respected %d, "
                        "number of banks %d\n\r"),
             tcam_db_id, tcam_info->action_bitmap_ndx, tcam_info->prefix_size, tcam_info->bank_owner,
             ARAD_TCAM_BANK_ENTRY_SIZE_to_string(tcam_info->entry_size), tcam_info->use_small_banks, 
             tcam_info->no_insertion_priority_order, tcam_info->sparse_priorities, tcam_info->min_banks));


  
  res = arad_tcam_access_create_verify(
          unit,          
          tcam_db_id,
          tcam_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  for (tcam_db_other_ndx = 0; tcam_db_other_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_other_ndx++) {
      if (SHR_BITGET(tcam_info->forbidden_dbs, tcam_db_other_ndx)) {
          
          res = arad_sw_db_tcam_db_forbidden_dbs_set(unit, tcam_db_id, tcam_db_other_ndx, TRUE );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          res = arad_sw_db_tcam_db_forbidden_dbs_set(unit, tcam_db_other_ndx, tcam_db_id, TRUE );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

  
  res = arad_tcam_db_create_unsafe(
          unit,
          tcam_db_id,
          tcam_info->entry_size,
          tcam_info->prefix_size,
          tcam_info->action_bitmap_ndx,
          tcam_info->use_small_banks,
          tcam_info->no_insertion_priority_order,
          tcam_info->sparse_priorities,
          tcam_info->is_direct
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  tcam_db_created = 1;

  
  nof_access_profiles = (tcam_info->entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)? 2 : 1;
  access_profile_array_id = 0;
  for (access_profile_ndx = 0; (access_profile_ndx < ARAD_TCAM_NOF_ACCESS_PROFILE_IDS) && (access_profile_array_id != nof_access_profiles); access_profile_ndx++) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.valid.get(unit, access_profile_ndx, &access_profile_valid);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 41, exit);
      if (!access_profile_valid) {
          
          access_profile[access_profile_array_id] = access_profile_ndx;
          access_profile_array_id ++;
      }
  }

  
  for (access_profile_ndx = 0; (access_profile_ndx < ARAD_TCAM_NOF_ACCESS_PROFILE_IDS) 
        && (access_profile_array_id != nof_access_profiles) && (*success == SOC_SAND_SUCCESS); access_profile_ndx++) {
      if (access_profile[access_profile_array_id] == ARAD_TCAM_NOF_ACCESS_PROFILE_IDS) {
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      }
  }

  
  
  for (access_profile_array_id = 0; (access_profile_array_id < nof_access_profiles) && (*success == SOC_SAND_SUCCESS); access_profile_array_id++) {
      res = arad_tcam_access_profile_create_unsafe(
              unit,
              access_profile[access_profile_array_id],
              access_profile_array_id,
              tcam_db_id,
              tcam_info->min_banks,
              tcam_info->bank_owner, 
              tcam_info->is_direct,   
              tcam_info->callback,
              tcam_info->user_data,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

  
  sal_sprintf(string_to_print, "    ");
  sal_sprintf(string_to_print + sal_strlen(string_to_print), "TCAM DB Create: ");
  if(*success == SOC_SAND_SUCCESS) {
      sal_sprintf(string_to_print + sal_strlen(string_to_print), "allocation success, ");
      sal_sprintf(string_to_print + sal_strlen(string_to_print), "access_profile: %d ", access_profile[0]);
      if (nof_access_profiles == 2) {
          sal_sprintf(string_to_print + sal_strlen(string_to_print), "and %d ", access_profile[1]);
      }
      sal_sprintf(string_to_print + sal_strlen(string_to_print), "\n\r");

  }
  else{
      sal_sprintf(string_to_print + sal_strlen(string_to_print), "allocation Failed\n\r");
  }
  LOG_DEBUG(BSL_LS_SOC_TCAM,
            (BSL_META_U(unit,
                        "%s"), string_to_print));
  

exit:
  if ((*success != SOC_SAND_SUCCESS) && tcam_db_created)
  {
    res = arad_tcam_db_destroy_unsafe(
            unit,
            tcam_db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_create_unsafe()", tcam_db_id, 0);  
}

uint32
  arad_tcam_access_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *tcam_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_MGMT_TCAM_DB_ID_MAX, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);

  res = ARAD_TCAM_ACCESS_INFO_verify(unit, tcam_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_create_verify()", 0, 0);  
}

uint32
  arad_tcam_access_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  uint32
    res,
      nof_access_profiles,
      access_profile_array_id,
    access_profile_id;
  ARAD_TCAM_BANK_ENTRY_SIZE
    db_entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(
          unit,
          tcam_db_id,
          &db_entry_size
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  nof_access_profiles = (db_entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)? 2 :1;

  
  res = arad_tcam_db_destroy_unsafe(
          unit,
          tcam_db_id
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  for (access_profile_array_id = 0; access_profile_array_id < nof_access_profiles; access_profile_array_id++) {
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(
              unit,
              tcam_db_id,
              access_profile_array_id,
              &access_profile_id
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

      if (access_profile_id < ARAD_TCAM_NOF_ACCESS_PROFILE_IDS) {
          res = arad_tcam_access_profile_destroy_unsafe(
                  unit,
                  access_profile_id
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_destroy_unsafe()", 0, 0);  
}


uint32 
  arad_tcam_managed_db_direct_table_bank_add(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
      res = SOC_SAND_OK,  
      access_profile_array_id,
      access_profile_id,
      bank_ndx;
  uint8
    is_used;
  ARAD_TCAM_BANK_OWNER
    bank_owner;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *success = SOC_SAND_SUCCESS;

  
  for(bank_ndx = 0; bank_ndx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); bank_ndx++)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
            unit, 
            tcam_db_id, 
            bank_ndx,
            &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    if(is_used)
    {
         
        *success = SOC_SAND_FAILURE_INTERNAL_ERR;
    }
  }
  
  
  if(SOC_SAND_SUCCESS == *success) {
      access_profile_array_id = 0;
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, tcam_db_id, access_profile_array_id, &access_profile_id);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(unit, access_profile_id, &bank_owner);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      res = arad_tcam_access_profile_bank_add(
                unit,
                access_profile_id,
                access_profile_array_id,
                tcam_db_id,
                TRUE, 
                bank_owner, 
                FALSE, 
                0,
                success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_direct_table_bank_add()", 0, 0);  
}


uint32 
  arad_tcam_managed_db_direct_table_bank_remove(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32     tcam_db_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
      res = SOC_SAND_OK,  
      bank_ndx;
  uint8
      is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  *success = SOC_SAND_FAILURE_INTERNAL_ERR;

  
  for(bank_ndx = 0; bank_ndx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit); bank_ndx++)
  {
    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.bank_used.get(
          unit, 
          tcam_db_id, 
          bank_ndx,
          &is_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    if(is_used)
    {
        *success = SOC_SAND_SUCCESS;
        break;
    }
  }
  
  
  if(SOC_SAND_SUCCESS == *success) {
      
      res = arad_tcam_db_bank_remove_unsafe(unit, tcam_db_id, bank_ndx);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_direct_table_bank_remove()", 0, 0);  
}


uint32
  arad_tcam_managed_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8                is_single_bank,
    SOC_SAND_IN  uint32               priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY      *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION    *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
  uint32
      nof_access_profiles,
      access_profile_array_id,
      res = SOC_SAND_OK,  
      nof_banks = 0,
      access_profile_id;
  uint8 
      is_direct = FALSE,
      valid;
  ARAD_TCAM_BANK_OWNER
    bank_owner;
  ARAD_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_ADD_UNSAFE);
  
  res = arad_tcam_managed_db_entry_add_verify(
          unit,
          tcam_db_id,
          entry_id,
          priority,
          entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);
  if (!valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Entry id %d - Failed to add entry. The tcam doen\'t exist.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

  
  
  res = arad_tcam_db_entry_add_unsafe(
          unit,
          tcam_db_id,
          entry_id,
          priority,
          entry,
          action,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
        
        res = arad_tcam_db_nof_banks_get_unsafe(
          unit,
          tcam_db_id,
          &nof_banks
        );
       SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

       
       if (nof_banks && is_single_bank )
       {
           *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
           ARAD_DO_NOTHING_AND_EXIT;
       }
      
      res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.entry_size.get(unit, tcam_db_id, &entry_size);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);
      nof_access_profiles = (entry_size == ARAD_TCAM_BANK_ENTRY_SIZE_320_BITS)? 2 : 1;
      *success = SOC_SAND_SUCCESS;
      for (access_profile_array_id = 0; (access_profile_array_id < nof_access_profiles) && (*success == SOC_SAND_SUCCESS); access_profile_array_id++) {
          
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(
                  unit,
                  tcam_db_id,
                  access_profile_array_id,
                  &access_profile_id
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);

          res = sw_state_access[unit].dpp.soc.arad.tm.tcam_mgmt.profiles.bank_owner.get(
                  unit,
                  access_profile_id,
                  &bank_owner
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);

          
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.is_direct.get(unit, tcam_db_id, &is_direct);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

          res = arad_tcam_access_profile_bank_add(
                    unit,
                    access_profile_id,
                    access_profile_array_id,
                    tcam_db_id,
                    is_direct,
                    bank_owner,
                    entry->is_inserted_top,
                    is_single_bank,
                    success
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
    if (*success == SOC_SAND_SUCCESS)
    {
      
      res = arad_tcam_db_entry_add_unsafe(
              unit,
              tcam_db_id,
              entry_id,
              priority,
              entry,
              action,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_add_unsafe()", 0, 0);
}

uint32
  arad_tcam_managed_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint32                priority,
    SOC_SAND_IN  ARAD_TCAM_ENTRY      *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION     *action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_MGMT_TCAM_DB_ID_MAX, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_MGMT_ENTRY_ID_MAX, ARAD_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ENTRY, entry, 40, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ACTION, action, 50, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_add_verify()", 0, 0);
}


uint32
  arad_tcam_managed_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  ARAD_TCAM_ENTRY      *entry,
    SOC_SAND_IN  ARAD_TCAM_ACTION     *action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_MGMT_TCAM_DB_ID_MAX, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_MGMT_ENTRY_ID_MAX, ARAD_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ENTRY, entry, 40, exit);
  ARAD_STRUCT_VERIFY(ARAD_TCAM_ACTION, action, 50, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_get_verify()", 0, 0);
}

uint32
  arad_tcam_managed_db_entry_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint8                is_single_bank,
    SOC_SAND_OUT  ARAD_TCAM_ENTRY     *entry,
    SOC_SAND_OUT  ARAD_TCAM_ACTION    *action
  )
{
  uint32
      res = SOC_SAND_OK;
  uint8 
      valid;
 
 uint8              hit_bit_clear=0; 
 uint8              hit_bit=0, found=0;
 uint32             priority; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_GET_UNSAFE);
  
  res = arad_tcam_managed_db_entry_get_verify(
          unit,
          tcam_db_id,
          entry_id,
          entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);
  if (!valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Entry id %d - Failed to get entry. The tcam doen\'t exist.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

  
  
  res = arad_tcam_db_entry_get_unsafe(
          unit,
          tcam_db_id,
          entry_id,
          hit_bit_clear,
          &priority,
          entry,
          action,
          &found,
          &hit_bit);

  if(!found) {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Entry id %d - Failed to get entry. The tcam doen\'t exist.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 20, exit);
  }

  if(hit_bit) {
    LOG_INFO(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d Tcam DB id %d Entry id %d priority = %d - The TCAM entry has been hit .\n\r"),
               unit, tcam_db_id, entry_id, priority));
  }

  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_get_unsafe()", 0, 0);
}




uint32
  arad_tcam_managed_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_UNSAFE);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  if (!valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d, Tcam DB id %d Entry id %d - Failed to remove entry. The data base doesn\'t exist.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

  
  res = arad_tcam_managed_db_entry_remove_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_tcam_db_entry_remove_unsafe(
          unit,
          TRUE ,
          tcam_db_id,
          entry_id,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_remove_unsafe()", 0, 0);
}



uint32
  arad_tcam_managed_db_entry_remove_keep_loc_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_UNSAFE);

  res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_id, &valid);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  if (!valid)
  {
    LOG_ERROR(BSL_LS_SOC_TCAM,
              (BSL_META_U(unit,
                          "Unit %d, Tcam DB id %d Entry id %d - Failed to remove entry. The data base doesn\'t exist.\n\r"),
               unit, tcam_db_id, entry_id));
    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

  
  res = arad_tcam_managed_db_entry_remove_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_tcam_db_entry_remove_unsafe(
          unit,
          FALSE ,
          tcam_db_id,
          entry_id,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_remove_keep_loc_unsafe()", 0, 0);
}



uint32
  arad_tcam_managed_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, ARAD_TCAM_MGMT_TCAM_DB_ID_MAX, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, ARAD_TCAM_MGMT_ENTRY_ID_MAX, ARAD_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_managed_db_entry_remove_verify()", 0, 0);
}

uint32
  arad_tcam_access_pd_profile_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id,
    SOC_SAND_OUT uint32              *pd_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pd_profile_id);

   
   res = arad_tcam_access_pd_profile_get_verify(
           unit,
           access_profile_id
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *pd_profile_id = access_profile_id;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_access_device_get_unsafe()", 0, 0);
}

uint32
  arad_tcam_access_pd_profile_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               access_profile_id
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(access_profile_id, ARAD_TCAM_MGMT_ACCESS_PROFILE_ID_MAX, ARAD_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tcam_access_profile_access_device_get_verify()", 0, 0);
}

void
  arad_ARAD_TCAM_ENTRY_clear(
    SOC_SAND_OUT ARAD_TCAM_ENTRY *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_ENTRY));
  for (ind=0; ind<ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->value[ind] = 0;
  }
  for (ind=0; ind<ARAD_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->mask[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_TCAM_ACCESS_INFO_clear(
    SOC_SAND_OUT ARAD_TCAM_ACCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_ACCESS_INFO));
  
  info->action_bitmap_ndx = ARAD_TCAM_NOF_ACTION_SIZES;
  info->entry_size = ARAD_TCAM_NOF_BANK_ENTRY_SIZES;
  info->bank_owner = ARAD_TCAM_NOF_BANK_OWNERS;
  info->is_direct = FALSE;
  info->use_small_banks = ARAD_TCAM_SMALL_BANKS_ALLOW;
  info->no_insertion_priority_order = FALSE;
  info->sparse_priorities = FALSE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_TCAM_ACCESS_INFO_clear_and_update( SOC_SAND_OUT ARAD_TCAM_ACCESS_INFO *info, ARAD_TCAM_BANK_OWNER owner, int entry_size, int user_data)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_TCAM_ACCESS_INFO));

  info->action_bitmap_ndx = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
  info->bank_owner = owner;
  info->callback   = arad_pp_isem_access_tcam_callback;
  info->entry_size = entry_size;
  info->is_direct = FALSE;
  info->min_banks = ARAD_PP_ISEM_ACCESS_TCAM_DEF_MIN_BANKS;
  info->prefix_size = ARAD_PP_ISEM_ACCESS_TCAM_DEF_PREFIX_SIZE;
  info->use_small_banks = ARAD_TCAM_SMALL_BANKS_ALLOW;
  info->no_insertion_priority_order = FALSE;
  info->sparse_priorities = FALSE;
  info->user_data = user_data;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  ARAD_TCAM_ACCESS_INFO_verify(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  ARAD_TCAM_ACCESS_INFO *info
  )
{
  uint32
      tcam_db_other_ndx,
      res;
  uint8
      valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_size, ARAD_TCAM_ENTRY_SIZE_MAX+(info->is_direct?1:0), ARAD_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->prefix_size, ARAD_TCAM_PREFIX_SIZE_MAX, ARAD_TCAM_PREFIX_SIZE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_bitmap_ndx, ARAD_TCAM_ACTION_SIZE_MAX, ARAD_TCAM_ACTION_SIZE_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->min_banks, ARAD_TCAM_MGMT_MIN_BANKS_MAX, ARAD_TCAM_MGMT_MIN_BANKS_OUT_OF_RANGE_ERR, 60, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->user_data, ARAD_TCAM_MGMT_USER_DATA_MAX, ARAD_TCAM_MGMT_USER_DATA_OUT_OF_RANGE_ERR, 70, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bank_owner, ARAD_TCAM_BANK_OWNER_MAX, ARAD_TCAM_MGMT_BANK_OWNER_OUT_OF_RANGE_ERR, 80, exit);  

  
  for (tcam_db_other_ndx = 0; tcam_db_other_ndx < ARAD_TCAM_MAX_NOF_LISTS; tcam_db_other_ndx++) {
      if (SHR_BITGET(info->forbidden_dbs, tcam_db_other_ndx)) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, tcam_db_other_ndx, &valid);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);
          if (!valid) {
              SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 90, exit);
          }
      }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TCAM_ACCESS_INFO_verify()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 

