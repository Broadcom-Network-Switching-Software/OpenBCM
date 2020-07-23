/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ARAD_PS_DB_INCLUDED__

#define __ARAD_PS_DB_INCLUDED__

#include <soc/types.h>

int arad_ps_db_init(int unit);
int arad_ps_db_find_free_binding_ps(int unit, soc_port_t port, int out_port_priority, int is_init, int* base_q_pair);
int arad_ps_db_alloc_binding_ps_with_id(int unit, soc_port_t port, int out_port_priority, int base_q_pair);
int arad_ps_db_find_free_non_binding_ps(int unit, int core, int is_init, int* base_q_pair);
int arad_ps_db_find_free_non_binding_ps_with_id(int unit, soc_port_t port, int base_q_pair);
int arad_ps_db_release_binding_ps(int unit, soc_port_t port, int base_q_pair);
int arad_ps_db_release_non_binding_ps(int unit, int core, int out_port_priority, int base_q_pair);


#endif
