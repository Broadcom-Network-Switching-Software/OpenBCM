/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#ifndef _ARAD_TCAM_ACCESS_CRASH_RECOVERY_H
#define _ARAD_TCAM_ACCESS_CRASH_RECOVERY_H

#include <soc/types.h>
#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/error.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/hwstate/hw_log.h>

#define ARAD_TCAM_CR_ELEMENTS_COUNT 3000

typedef struct 
{
  uint32               tcam_db_id;
  uint32               entry_id;
} ARAD_TCAM_CR_KEY;


typedef struct
{
  uint8                is_deleted;
  uint8                hit_bit;
  ARAD_TCAM_ENTRY      entry;
  ARAD_TCAM_ACTION     action;
} ARAD_TCAM_CR_VALUE;

typedef struct
{
  ARAD_TCAM_CR_KEY key;
  ARAD_TCAM_CR_VALUE value;
} ARAD_TCAM_CR_KEY_VALUE_PAIR;

typedef struct
{
  ARAD_TCAM_CR_KEY_VALUE_PAIR dictionary[ARAD_TCAM_CR_ELEMENTS_COUNT];
  uint32 count;
} ARAD_TCAM_CR_DB;

ARAD_TCAM_CR_DB tcam_cr_transaction_db[SOC_MAX_NUM_DEVICES];

extern int arad_tcam_access_cr_entry_get(
  SOC_SAND_IN int                unit,
  SOC_SAND_IN uint32             tcam_db_id,
  SOC_SAND_IN uint32             entry_id,
  SOC_SAND_OUT uint8             *hit_bit,
  SOC_SAND_OUT ARAD_TCAM_ENTRY   *entry,
  SOC_SAND_OUT ARAD_TCAM_ACTION  *action,
  SOC_SAND_OUT uint8             *found
);

extern int arad_tcam_access_cr_entry_add(
  SOC_SAND_IN  int                          unit,
  SOC_SAND_IN  uint32                       tcam_db_id,
  SOC_SAND_IN  uint32                       entry_id,
  SOC_SAND_IN  uint8                        hit_bit,
  SOC_SAND_IN  ARAD_TCAM_ENTRY              *entry,
  SOC_SAND_IN  ARAD_TCAM_ACTION             *action
);

extern int arad_tcam_access_cr_entry_remove(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            tcam_db_id,
    SOC_SAND_IN  uint32            entry_id
);

extern int arad_tcam_access_cr_dictionary_clear(int unit);

#endif
#endif
