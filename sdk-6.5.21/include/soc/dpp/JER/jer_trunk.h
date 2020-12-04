/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_trunk.h
 */

#ifndef __JER_TRUNK_INCLUDED__

#define __JER_TRUNK_INCLUDED__







uint32 
  soc_jer_trunk_direct_lb_key_set( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32 local_port,
    SOC_SAND_IN uint32 min_lb_key,
    SOC_SAND_IN uint32 set_min,
    SOC_SAND_IN uint32 max_lb_key,
    SOC_SAND_IN uint32 set_max
   );


uint32 
    soc_jer_trunk_direct_lb_key_get(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN int core_id, 
      SOC_SAND_IN uint32  local_port,
      SOC_SAND_OUT uint32 *min_lb_key,
      SOC_SAND_OUT uint32 *max_lb_key
   );

int soc_jer_trunk_init (int unit);

#endif 

