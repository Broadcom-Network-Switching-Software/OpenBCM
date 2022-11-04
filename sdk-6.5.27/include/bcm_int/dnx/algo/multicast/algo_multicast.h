/** \file bcm_int/dnx/algo/multicast/algo_multicast.h
 *
 * Reserved.$
 */

#ifndef _DNX_ALGO_MULTICSAT_INCLUDED_
/*
 * {
 */
#define _DNX_ALGO_MULTICSAT_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_failover_access.h>

/*
 * MACROs
 * {
 */

/*
 * }
 * MACROs
 */
/*
 * enum for multicast MCDB allocation method
 */
typedef enum
{
    /*
     * At this method there is only one region and ingress and egress can use the entire range of MCDB
     */
    DNX_ALGO_MULTICAST_ALLOCATION_BASIC
} dnx_algo_multicast_mcdb_allocation_method_e;
/*
 * }
 */

/**
 * \brief Wrapper function for allocation manager which creates tagged allocation bitmap for multicast mcdb.
 *           The tag will represent ingress or egress replication,
 *           Refer to resource_tag_bitmap_create() for details about the input parameters.
 */
shr_error_e dnx_algo_multicast_bitmap_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

#endif /*_DNX_ALGO_MULTICSAT_INCLUDED_*/
