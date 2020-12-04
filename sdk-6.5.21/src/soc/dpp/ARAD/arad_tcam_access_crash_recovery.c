/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#include <appl/diag/parse.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <shared/bsl.h>

#ifdef CRASH_RECOVERY_SUPPORT

#include <soc/dpp/ARAD/arad_tcam_access_crash_recovery.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

int arad_tcam_access_cr_entry_get(
  SOC_SAND_IN int                unit,
  SOC_SAND_IN uint32             tcam_db_id,
  SOC_SAND_IN uint32             entry_id,
  SOC_SAND_OUT uint8             *hit_bit,
  SOC_SAND_OUT ARAD_TCAM_ENTRY   *entry,
  SOC_SAND_OUT ARAD_TCAM_ACTION  *action,
  SOC_SAND_OUT uint8             *found
)
{
    uint32 idx;
    uint32 found_idx = 0;

    ARAD_TCAM_CR_KEY_VALUE_PAIR tcam_cr_element;

    SOC_INIT_FUNC_DEFS;

    *found = 0;

    for(idx = 0; idx < tcam_cr_transaction_db[unit].count; idx++) {

        tcam_cr_element = tcam_cr_transaction_db[unit].dictionary[idx];

        if((tcam_cr_element.key.tcam_db_id == tcam_db_id) && 
           (tcam_cr_element.key.entry_id == entry_id) && 
           !tcam_cr_element.value.is_deleted) {
            
            *found = 1;
            found_idx = idx;
        }
    }

    if(found) {
        tcam_cr_element = tcam_cr_transaction_db[unit].dictionary[found_idx];

        
        *hit_bit = tcam_cr_element.value.hit_bit;
        sal_memcpy(entry, &tcam_cr_element.value.entry, sizeof(ARAD_TCAM_ENTRY));
        sal_memcpy(action, &tcam_cr_element.value.action, sizeof(ARAD_TCAM_ACTION));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int arad_tcam_access_cr_entry_add(
  SOC_SAND_IN  int                          unit,
  SOC_SAND_IN  uint32                       tcam_db_id,
  SOC_SAND_IN  uint32                       entry_id,
  SOC_SAND_IN  uint8                        hit_bit,
  SOC_SAND_IN  ARAD_TCAM_ENTRY              *entry,
  SOC_SAND_IN  ARAD_TCAM_ACTION             *action
)
{
    ARAD_TCAM_CR_KEY_VALUE_PAIR tcam_cr_entry;

    SOC_INIT_FUNC_DEFS;

    
    tcam_cr_entry.key.tcam_db_id = tcam_db_id;
    tcam_cr_entry.key.entry_id = entry_id;

    
    tcam_cr_entry.value.is_deleted = 0;
    tcam_cr_entry.value.hit_bit = hit_bit;
    tcam_cr_entry.value.entry = *entry;
    tcam_cr_entry.value.action = *action;

    
    tcam_cr_transaction_db[unit].dictionary[tcam_cr_transaction_db[unit].count++] = tcam_cr_entry;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int arad_tcam_access_cr_entry_remove(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            tcam_db_id,
    SOC_SAND_IN  uint32            entry_id
)
{
    ARAD_TCAM_CR_KEY_VALUE_PAIR tcam_cr_entry;

    SOC_INIT_FUNC_DEFS;

    
    tcam_cr_entry.key.tcam_db_id = tcam_db_id;
    tcam_cr_entry.key.entry_id = entry_id;

    
    tcam_cr_entry.value.is_deleted = 1;

    
    tcam_cr_transaction_db[unit].dictionary[tcam_cr_transaction_db[unit].count++] = tcam_cr_entry;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int arad_tcam_access_cr_dictionary_clear(int unit)
{
    SOC_INIT_FUNC_DEFS;

    tcam_cr_transaction_db[unit].count = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

#endif
