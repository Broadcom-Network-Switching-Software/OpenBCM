/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_mgmt.h
 */

#ifndef __QAX_MGMT_INCLUDED__

#define __QAX_MGMT_INCLUDED__


uint32 qax_mgmt_system_fap_id_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  sys_fap_id
  );



uint32
  qax_mgmt_system_fap_id_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT uint32    *sys_fap_id
  );


uint32 qax_mgmt_revision_fixes(int unit);

#endif 

