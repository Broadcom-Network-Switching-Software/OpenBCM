/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __JER_TDM_INCLUDED__

#define __JER_TDM_INCLUDED__




 
int jer_tdm_ingress_failover_set(int unit, bcm_pbmp_t tdm_enable_pbmp);

 
int jer_tdm_ingress_failover_get(int unit, bcm_pbmp_t *tdm_enable_pbmp);

int 
jer_mesh_tdm_multicast_set(
   int                            unit, 
   soc_port_t                     port,
   uint32                         flags, 
   uint32                         destid_count,
   soc_module_t                   *destid_array
   );


uint32
  jer_tdm_init(
      int  unit
  );

#endif 

