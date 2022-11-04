
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PEMLADRV_CFG_API_DEFINES_H_
#define _PEMLADRV_CFG_API_DEFINES_H_

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif

#define PEM_CFG_API_MAP_CHUNK_WIDTH             32
#define PEM_CFG_API_CAM_TCAM_CHUNK_WIDTH        16
#define PEM_CFG_API_CAM_RESULT_CHUNK_WIDTH      16
#define PEM_CFG_API_CAM_TCAM_KEY_WIDTH          16
#define UINT_WIDTH_IN_BITS                        (8*sizeof(unsigned int))

#ifdef BCM_DNX_SUPPORT
#include <limits.h>
#include <sal/types.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#define pemladrv_ceil(ARG, ARG2)              UTILEX_DIV_ROUND_UP((ARG), (ARG2))

#define PEM_CFG_API_MAP_CHUNK_N_ENTRIES         ((soc_is(unit, J2P_DEVICE)||(soc_is(unit, J2X_DEVICE)))?1024:256)
#define PEM_CFG_API_CAM_TCAM_CHUNK_N_ENTRIES    ((soc_is(unit, J2P_DEVICE)||(soc_is(unit, J2X_DEVICE)))?16:32)
#define PEM_CFG_API_CAM_RESULT_CHUNK_N_ENTRIES  ((soc_is(unit, J2P_DEVICE)||(soc_is(unit, J2X_DEVICE)))?16:32)

#define PEM_CFG_API_NOF_CORES            DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES

#define PEM_CFG_API_NOF_CORES_DYNAMIC    (dnx_data_device.general.nof_cores_get(unit))

#define PEM_CFG_NOF_SUB_UNITS                (dnx_data_device.general.nof_core_groups_get(unit))

#define PEM_CFG_CORE_ID_TO_SUB_UNIT_ID(_core_id) (((_core_id >= 0) && (_core_id < PEM_CFG_API_NOF_CORES_DYNAMIC)) ? (_core_id / dnx_data_device.general.nof_cores_per_core_group_get(unit)) : 0)
#define PEM_CFG_SUB_UNIT_TO_CORE_ID(_sub_unit_id, _core_id) (_core_id + dnx_data_device.general.nof_cores_per_core_group_get(unit) * _sub_unit_id)
#define PEM_CFG_RELATIVE_CORE_ID(_core_id) (_core_id % dnx_data_device.general.nof_cores_per_core_group_get(unit))
#endif

#endif
