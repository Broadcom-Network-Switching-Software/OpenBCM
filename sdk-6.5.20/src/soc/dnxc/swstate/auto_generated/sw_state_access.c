
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

typedef int make_iso_compilers_happy_swstatednx_sw_state_access;

#ifdef BCM_DNX_SUPPORT
#include <src/soc/dnxc/swstate/auto_generated/access/dnx_sw_state_defragmented_chunk_access.c>
#endif  
#include <src/soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.c>
#include <src/soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.c>
#include <src/soc/dnxc/swstate/auto_generated/access/dnxc_access.c>
#include <src/soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.c>
#include <src/soc/dnxc/swstate/auto_generated/access/multithread_analyzer_access.c>
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <src/soc/dnxc/swstate/auto_generated/access/wb_engine_access.c>
#endif  
