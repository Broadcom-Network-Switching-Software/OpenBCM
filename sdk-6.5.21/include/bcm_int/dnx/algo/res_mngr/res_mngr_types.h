/** \file res_mngr/res_mngr_types.h
 *
 * Internal DNX resource manager APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_RES_MNGR_TYPES_INCLUDED
/*
 * {
 */
#define ALGO_RES_MNGR_TYPES_INCLUDED

/**
 *
 *   \brief General types for resource manager.
 */

/*
 * Each of these enum members represents an advanced algorithm used by the resource manager.
 */
typedef enum
{
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_INVALID = -1,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC = 0,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_OUTLIF,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_INLIF,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_IQM,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_SCH,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FFC,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FAILOVER_FEC_PATH_SELECT,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_ECMP,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_L3_FEC,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_GLOBAL_LIF,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_PORT_TPID_TAG_STRUCT,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_MULTICAST_MCDB,
    DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_COUNT
} dnx_algo_resource_advanced_algorithms_e;

/**
 * \brief This is the callback prototype used for iterating over resources. Not yet implemented.
 */
typedef shr_error_e(
    *dnx_algo_res_iterate_iter_cb) (
    int unit,
    int core_id,
    int element,
    void *extra_arguments);

/*
 * }
 */
#endif /* ALGO_RES_MNGR_TYPES_INCLUDED */
