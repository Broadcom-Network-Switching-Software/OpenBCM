
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif


#define PEM_CFG_API_MAP_CHUNK_WIDTH             32
#define PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH        16
#define PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH      16
#define PEM_CFG_API_CAM_TCAM_KEY_WIDTH          16

#ifndef PROJECT_J2P
#define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         256
#define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    32
#define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  32
#endif
#ifdef PROJECT_J2P
#define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         1024
#define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    16
#define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  16

#endif

#define UINT_WIDTH_IN_BITS						(8*sizeof(unsigned int))

#define PEM_CFG_API_NOF_CORES 2

#ifndef BCM_DNX_SUPPORT
#if defined(PROJECT_J2_A0) || defined(PROJECT_J2) || defined(PROJECT_J2P)
#define PEM_CFG_API_NOF_CORES_DYNAMIC 2
#else
#define PEM_CFG_API_NOF_CORES_DYNAMIC 1
#endif
#else
#define PEM_CFG_API_NOF_CORES_DYNAMIC    (dnx_data_device.general.nof_cores_get(unit))
#endif

