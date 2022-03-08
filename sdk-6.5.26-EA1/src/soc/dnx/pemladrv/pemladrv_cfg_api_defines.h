
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif


#define PEM_CFG_API_MAP_CHUNK_WIDTH             32
#define PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH        16
#define PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH      16
#define PEM_CFG_API_CAM_TCAM_KEY_WIDTH          16



#ifndef BCM_DNX_SUPPORT


    #define SOC_IS_J2P                        false
    #define SOC_IS_J2X                        false



#if defined(PROJECT_J2P)
    #define SOC_IS_J2P                        true
#elif defined(PROJECT_J2X)
    #define SOC_IS_J2X                        true
#endif

#endif


#ifndef BCM_DNX_SUPPORT

    #if !defined(PROJECT_J2P) && !defined(PROJECT_J2X) && !defined(PROJECT_J3)
        #define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         256
        #define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    32
        #define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  32
    #endif
    #if defined(PROJECT_J2P) || defined(PROJECT_J2X) || defined (PROJECT_J3)
        #define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         1024
        #define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    16
        #define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  16
    #endif

#else
    #define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         ((SOC_IS_J2P(unit)||(SOC_IS_J2X(unit)))?1024:256)
    #define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    ((SOC_IS_J2P(unit)||(SOC_IS_J2X(unit)))?16:32)
    #define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  ((SOC_IS_J2P(unit)||(SOC_IS_J2X(unit)))?16:32)

#endif

#define UINT_WIDTH_IN_BITS						(8*sizeof(unsigned int))

#ifndef BCM_DNX_SUPPORT
    #if   defined(PROJECT_J3)
        #define PEM_CFG_API_NOF_CORES 4
    #elif defined(PROJECT_J2_A0) || defined(SOC_IS_JERICHO_2_A0) || defined(PROJECT_J2) || defined(SOC_IS_JERICHO_2) || defined(PROJECT_J2P)
		#define PEM_CFG_API_NOF_CORES 2
    #else
        #define PEM_CFG_API_NOF_CORES 1
    #endif


    #define PEM_CFG_API_NOF_CORES_DYNAMIC   PEM_CFG_API_NOF_CORES

#else
    
    #define PEM_CFG_API_NOF_CORES_DYNAMIC    (dnx_data_device.general.nof_cores_per_die_get(unit))
    #define PEM_CFG_API_NOF_CORES            4
#endif


