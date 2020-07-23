/** \file include/bcm_int/dnx/cosq/ingress/ipq.h
 *
 * Internal DNX IPQ
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_IPQ_INCLUDED__
#define _DNX_IPQ_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_ipq_alloc_mngr_types.h>

/*
 * Defines:
 * {
 */

/* Number of queues in the device */
#define DNX_IPQ_NOF_QUEUES(unit)            (dnx_data_ipq.queues.nof_queues_get(unit))

/* Number of queues in a region */
#define DNX_IPQ_NOF_QUEUES_IN_REGION(unit)  (dnx_data_ipq.regions.nof_queues_get(unit))

/* Minimum size of queue bundle */
#define DNX_IPQ_MIN_BUNDLE_SIZE(unit)       (dnx_data_ipq.queues.min_bundle_size_get(unit))

/* Number of regions the queues are devided into */
#define DNX_IPQ_NOF_REGIONS(unit)           (DNX_IPQ_NOF_QUEUES(unit) / DNX_IPQ_NOF_QUEUES_IN_REGION(unit))

/* Size of FMQ bundle is simple scheduling mode */
#define DNX_IPQ_FMQ_BUNDLE_SIZE(unit)       (dnx_data_ipq.queues.fmq_bundle_size_get(unit))

#define DNX_COSQ_IPQ_QUEUE_TO_QUAD_GET(qid)       ((qid) / dnx_data_ipq.queues.min_bundle_size_get(unit))
#define DNX_COSQ_IPQ_QUAD_TO_QUEUE_GET(quad)      ((quad) * dnx_data_ipq.queues.min_bundle_size_get(unit))
/*
 * }
 */

/*
 * Exposing internal declarations -
 * the following 2 functions are required in shell command "tm ingress queue mapping" implementation.
 * see sh_dnx_tm_queue_mapping_cmd()
 * for information about these functions, see declaration in ipq_dbal.h
 */
extern shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_get(
    int unit,
    int core,
    uint32 system_port,
    uint32 *base_voq,
    uint32 *is_valid);

extern shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *system_port);

/*
 * Functions prototypes
 * {
 */

/**
 * \brief
 *   Initialize dnx ipq module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_init(
    int unit);

/**
 * \brief
 *   Allocate queue bundles through resource manager.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] core -
 *    Device core id.
 *  \param [in] flags -
 *    Request to allocate specific queue (using DNX_ALGO_RES_ALLOCATE_WITH_ID),
 *    or any queue from region range (no flags needed).
 *  \param [in] queue_type -
 *    Type of queue. See \ref dnx_ipq_queue_type_e.
 *  \param [in] num_cos -
 *    Number of COS to allocate for the queue.
 *    Should be 4 or 8.
 *  \param [in,out] base_queue -
 *    The base queue that was allocated, or the base queue to allocate if using
 *    WITH_ID flag.
 *  \retval See \ref shr_error_e
 *  \remark
 *    The function allocates queue.
 *    If WITH_ID flag is used then it will allocate the queue specify in
 *    'base_queue' param.
 *    If WITH_ID flag is not used then it will allocate some free queue bundle
 *    matching queue_type.
 *    Note that when trying to allocate multicast queue in simple scheduling mode,
 *    only FMQ 0 can be allocated.
 *  \see
 *    dnx_algo_res_allocate
 */
shr_error_e dnx_cosq_ipq_queue_allocate(
    int unit,
    int core,
    uint32 flags,
    dnx_ipq_queue_type_e queue_type,
    int num_cos,
    int *base_queue);

/**
 * \brief
 *   Deallocate queue bundles through resource manager.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] core -
 *    Device core id.
 *  \param [in] base_queue -
 *    The base queue to deallocate.
 *  \retval See \ref shr_error_e.
 *  \remark
 *    For FMQ simple scheduling mode, deallocation is done through
 *    sw_state and not through alloc manager.
 *  \see
 *    dnx_algo_res_free
 */
shr_error_e dnx_cosq_ipq_queue_deallocate(
    int unit,
    int core,
    int base_queue);

/**
 * \brief
 *   Check if queue is allocated in resource manager.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] core - Device core id.
 *  \param [in] base_voq - The base queue to check.
 *  \param [out] is_allocated - indication from resource manager whether the queue is allocated.
 *
 *  \retval See \ref shr_error_e.
 *  \remark
 *  \see
 *    dnx_algo_res_free
 */
shr_error_e dnx_cosq_ipq_queue_is_allocated(
    int unit,
    int core,
    uint32 base_voq,
    int *is_allocated);
/**
 * \brief
 *   Queue to system port mapping.
 *   Returns system port for a given queue.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] core -
 *    Device core.
 *  \param [in] queue -
 *    The queue.
 *  \param [out] sysport -
 *    The system port.
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_ipq_queue_sysport_map_get(
    int unit,
    int core,
    int queue,
    uint32 *sysport);
/**
 * \brief
 *   Get base queue num_cos (number of queues in a bundle)
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] core - core id
 *  \param [in] base_queue_id - queue if of base queue
 *  \param [out] num_cos - number of queues in the bundle
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_ipq_queue_num_cos_get(
    int unit,
    int core,
    int base_queue_id,
    int *num_cos);

/**
 * \brief
 *   Get base queue num_cos (number of queues in a bundle)
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] queue_id - queue id
 *  \param [out] is_multicast - is the queue multicast
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_ipq_queue_is_multicast(
    int unit,
    int queue_id,
    int *is_multicast);

/**
 * \brief
 *   Function to return the cosq gport from the queue_id num and
 *   the core.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] core - core
 *  \param [in] queue_id - queue id
 *  \param [out] queue_gport - the queue_id gport
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_ipq_queue_gport_get(
    int unit,
    int core,
    int queue_id,
    bcm_gport_t * queue_gport);

/**
 * \brief
 *   Function to get the Fabric Multicast Queue upper range within the total queues range.
 *   Both cores are configured symmetrically, so this configuration applies to both cores.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [out] queue_id - queue id which is used as the upper limit for FMQ range.
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_cosq_ipq_fabric_multicast_queue_range_get(
    int unit,
    int *queue_id);

/**
 * \brief
 *   Function to get the Fabric Multicast Queue upper range within the total queues range.
 *   Both cores are configured symmetrically, so this configuration applies to both cores.
 *
 *  \param [in] unit -
 *    Relevant unit.
 *  \param [in] queue_id - queue id to be used as the upper limit for FMQ range.
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_cosq_ipq_fabric_multicast_queue_range_set(
    int unit,
    int queue_id);

/**
 * \brief - This is a callback function, registered to the alloc manager to be called
 *   whenever a new region is allocated. At that time, we need to set the
 *   region to be interdigitated or not.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] queue_region - queue region index.
 *  \param [in] is_non_contiguous - is region interdigitated.
 *
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_cosq_ipq_region_config_set(
    int unit,
    int queue_region,
    int is_non_contiguous);

/**
 * \brief
 *   Create a new instance of queue quartet bitmap resource manager.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] res_tag_bitmap - Resource tag bitmap.
 *  \param [in] create_data - Pointed memory contains setup parameters required for the
 *    creation of the resource. See \ref dnx_algo_res_create_data_t
 *  \param [in] extra_arguments - Not used.
 *  \param [in] nof_elements - Number of elements of the resource tag bitmap. Specified in the relevant sw state xml file.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    The function creates 1 long bitmap in size of (nof_queue_quartets),
 *    then it tags a group of region_size bits with the same tag.
 *  \see
 *    dnx_algo_res_create
 *    sw_state_res_tag_bitmap_create
 *    sw_state_res_tag_bitmap_tag_set
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief
 *   Free bits in queue quartet bitmap.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] res_tag_bitmap - Resource tag bitmap.
 *  \param [in] element - Element to free.
 *
 *  \retval See \ref shr_error_e
 *  \remark
 *    Can free more than 1 bit, according to how many bits was allocated. Number
 *    of bits allocated is derived from num_cos in sw_state.
*  \see
 *    resource_tag_bitmap_free
 *    dnx_algo_ipq_bitmap_allocate
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element);

/**
 * \brief
 *   Allocate bit(s) in queue quartet bitmap.
 *   Each bit represent a queues bundle.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] res_tag_bitmap - Resource tag bitmap.
 *  \param [in] flags -
 *    Special treatment:
 *      0 (None) - allocate some free element.
 *      DNX_ALGO_RES_ALLOCATE_WITH_ID - allocate specific element
 *      (see also 'element' param info).
 *  \param [in] alloc_info -
 *    Pointer to dnx_ipq_alloc_info_t that holds information
 *    about how many bits to allocate.
 *    See \ref dnx_ipq_alloc_info_t
 *  \param [in,out] element -
 *    Holds the allocated element id (serves as output in no flags were used).
 *    If WITH_ID flag was used this param serves as input and holds the
 *    element id number to allocate.
 *    If more than 1 bit allocated, element will hold the first allocated bit.
 *
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *  \see
 *    dnx_algo_res_allocate
 *    sw_state_res_tag_bitmap_alloc_tag
 *    dnx_algo_ipq_bitmap_create
 */
shr_error_e dnx_cosq_ipq_alloc_mngr_queue_qrtt_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *alloc_info,
    int *element);

/**
 * \brief - set mapping from TC to MC prioroity.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] cosq - TC.
 *  \param [in] mc_prio - multicast priority (BCM_COSQ_HIGH_PRIORITY / BCM_COSQ_LOW_PRIORITY).
 *
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_cosq_ipq_tc_to_multicast_prio_map_set(
    int unit,
    bcm_cos_queue_t cosq,
    int mc_prio);

/**
 * \brief - get mapping from TC to MC prioroity.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] cosq - TC.
 *  \param [out] mc_prio - multicast priority (BCM_COSQ_HIGH_PRIORITY / BCM_COSQ_LOW_PRIORITY).
 *
 *  \retval See \ref shr_error_e.
 *  \remark
 *    None.
 *  \see
 *    None.
 */
shr_error_e dnx_cosq_ipq_tc_to_multicast_prio_map_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *mc_prio);
/**
 * \brief -
 * Connect ingress VOQ to egress VOQ connector on the ingress side.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport_connect - VOQ to VOQ connector parameters

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_gport_connection_set(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect);

/**
 * \brief -
 * Get connected ingress VOQ to egress VOQ connector on the ingress side.
 *
 * \param [in] unit -  Unit-ID
 * \param [in,out] gport_connect - VOQ to VOQ connector parameters:
 *                 gport_connect->voq is used only as input to get base_queue and ingress core
 *                 gport_connect->voq_connector is used as input to get egress core, and as output to retrieve the VOQ connector.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_gport_connection_get(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect);

/**
 * \brief -
 * Allocate Unicast Gport bundle
 *
 * \param [in] unit -  Unit-ID
 * \param [in] config - queue bundle attributes to set
 * \param [in,out] gport - output for the allocated base queue gport. in case flag BCM_COSQ_GPORT_WITH_ID is used, this will be used as input.
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_ipq_ucast_gport_add(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport);

/**
 * \brief -
 * Allocate Multicast Gport bundle
 *
 * \param [in] unit -  Unit-ID
 * \param [in] config - queue bundle attributes to set
 * \param [in,out] gport - output for the allocated base queue gport. in case flag BCM_COSQ_GPORT_WITH_ID is used, this will be used as input.
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_mcast_gport_add(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config,
    bcm_gport_t * gport);

/**
 * \brief -
 * Delete Unicast queue
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - base queue gport
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_ucast_gport_delete(
    int unit,
    bcm_gport_t gport);

/**
 * \brief -
 * Delete Multicast queue
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - base queue gport
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_mcast_gport_delete(
    int unit,
    bcm_gport_t gport);

/**
 * \brief -
 * Get Unicast queue information from queue gport
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - base queue gport
 * \param [out] physical_port - destination gport (if applicable)
 * \param [out] num_cos_levels - number of queues in the bundle
 * \param [out] flags -bmc_gport_add flags used to create this gport
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_ucast_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags);

/**
 * \brief -
 * Get Multicast queue information from queue gport
 *
 * \param [in] unit -  Unit-ID
 * \param [in] gport - base queue gport
 * \param [out] physical_port - destination gport (if applicable)
 * \param [out] num_cos_levels - number of queues in the bundle
 * \param [out] flags -bmc_gport_add flags used to create this gport
 *
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ipq_mcast_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags);

/*
 * }
 */

#endif/*_DNX_IPQ_INCLUDED__*/
