
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

typedef int make_iso_compilers_happy_swstatednx_sw_state_layout;

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <src/soc/dnxc/swstate/auto_generated/layout/adapter_layout.c>
#include <src/soc/dnxc/swstate/auto_generated/layout/multithread_analyzer_layout.c>
#include <src/soc/dnxc/swstate/auto_generated/layout/sw_state_external_layout.c>
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <src/soc/dnxc/swstate/auto_generated/layout/wb_engine_layout.c>
#endif  
