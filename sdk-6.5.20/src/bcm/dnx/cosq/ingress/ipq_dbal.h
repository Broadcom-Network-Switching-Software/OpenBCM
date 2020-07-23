/*
 * 
 * ipq_dbal.h
 *
 * Created on: May 21, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.*
 */

#ifndef IPQ_DBAL_H_INCLUDED
/*
 * {
 */
#define IPQ_DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */
/*
 * Structs
 * {
 */

/*
 * brief - this struct contains queue ranges for FMQ
 * (Fabric Multicast Queue) out of the total VOQs
 */
typedef struct dnx_cosq_ipq_dbal_fmq_range_s
{
    uint32 first_queue;
    uint32 last_queue;
} dnx_cosq_ipq_dbal_fmq_range_t;

/*
 * brief - this struct gathers the last queue of wach VSQ categories.
 * currently only category 0 and 2 are supported.
 */
typedef struct dnx_cosq_ipq_dbal_vsq_categories_s
{
    uint32 last_queue_vsq_category_0;
    uint32 last_queue_vsq_category_1;
    uint32 last_queue_vsq_category_2;
} dnx_cosq_ipq_dbal_vsq_categories_t;

/*
 * }
 */
/*
 * Functions
 * {
 */

/**
 * \brief - Get FMQ (fabric multicast queue) range out of total queue range in the ingress packet queuing.
 *
 * \param [in] unit - chip unit id.
 * \param [out] fmq_range - FMQ range (lower and upper limit queues)
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_fmq_range_get(
    int unit,
    dnx_cosq_ipq_dbal_fmq_range_t * fmq_range);

/**
 * \brief - Set FMQ (fabric multicast queue) range out of total queue range in the ingress packet queuing.
 *
 * \param [in] unit - chip unit id.
 * \param [in] fmq_range - FMQ range (lower and upper limit queues)
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_fmq_range_set(
    int unit,
    dnx_cosq_ipq_dbal_fmq_range_t * fmq_range);

/**
 * \brief - Get VSQ ranges according to categories.
 *
 * \param [in] unit - chip unit id.
 * \param [out] vsq_categories - queue limits for each VSQ category.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_vsq_categories_get(
    int unit,
    dnx_cosq_ipq_dbal_vsq_categories_t * vsq_categories);

/**
 * \brief - Get VSQ ranges according to categories.
 *
 * \param [in] unit - chip unit id.
 * \param [in] vsq_categories - queue limits for each VSQ category.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_vsq_categories_set(
    int unit,
    dnx_cosq_ipq_dbal_vsq_categories_t * vsq_categories);

/**
 * \brief - Get interdigitated mode for the region.
 *
 * \param [in] unit - chip unit id.
 * \param [in] queue_region - region index.
 * \param [out] is_interdigitated - interdigitated mode for the region
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_region_is_interdigitated_get(
    int unit,
    uint32 queue_region,
    uint32 *is_interdigitated);

/**
 * \brief - Set interdigitated mode for the region.
 *
 * \param [in] unit - chip unit id.
 * \param [in] queue_region - region index.
 * \param [in] is_interdigitated - interdigitated mode for the region
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_region_is_interdigitated_set(
    int unit,
    uint32 queue_region,
    uint32 is_interdigitated);

/**
 * \brief - Set mapping from queue quartet to system port for an entire region.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core_id - core id.
 * \param [in] first_queue_quartet - first queue quartet to configure.
 * \param [in] nof_quartets_to_set - number of quartets in the region.
 * \param [in] system_port - system port to map to.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_range_set(
    int unit,
    int core_id,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 system_port);

/**
 * \brief - Set mapping from queue quartet to system port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] queue_quartet - queue quartet index.
 * \param [in] system_port - system port to map to.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_set(
    int unit,
    uint32 queue_quartet,
    uint32 system_port);

/**
 * \brief - clear one entry in queue quartet to system port map.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - queue quartet index to clear.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_clear(
    int unit,
    int core,
    uint32 queue_quartet);

/**
 * \brief - Set mapping from queue quartet to flow id quartet for for a range of queue quartets.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] first_queue_quartet - first queue quartet to configure.
 * \param [in] nof_quartets_to_set - number of quartets in the region.
 * \param [in] flow_quartet_index - base flow id to set.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_range_set(
    int unit,
    int core,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 flow_quartet_index);

/**
 * \brief - Get mapping from queue quartet to flow id .
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - queue quartet index.
 * \param [out] flow_quartet_index - base flow id.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *flow_quartet_index);

/**
 * \brief - Set mapping from queue quartet to flow id.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - queue quartet index.
 * \param [in] flow_quartet_index - base flow id to set.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_set(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 flow_quartet_index);

/**
 * \brief - Set mapping from queue quartet to flow id is composite for a range of queue quartets.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] first_queue_quartet - first queue quartet to configure.
 * \param [in] nof_quartets_to_set - number of quartets in the region.
 * \param [in] is_composite - is the connector connected to this queue quartet composite.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_range_set(
    int unit,
    int core,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 is_composite);

/**
 * \brief - Get mapping from queue quartet to flow id is composite.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - queue quartet index.
 * \param [out] is_composite - is the connector connected to this queue quartet composite.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *is_composite);

/**
 * \brief - Set mapping from queue quartet to flow id is composite.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - queue quartet index.
 * \param [in] is_composite - is the connector connected to this queue quartet composite.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_set(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 is_composite);

/**
 * \brief - Clear one entry in queue quartet to flow map.
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - core id.
 * \param [in] queue_quartet - first queue quartet index to clear.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_clear(
    int unit,
    int core,
    uint32 queue_quartet);

/**
 * \brief - Map System port to base VOQ
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] system_port - system port to map
 * \param [in] base_voq - voq base to map the system port
 * \param [in] is_valid - is the mapping valid.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_set(
    int unit,
    int core,
    uint32 system_port,
    uint32 base_voq,
    uint32 is_valid);

/**
 * Enable/Disable system port to VOQ base
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] system_port - system port to map
 * \param [in] is_valid - is the mapping valid.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_valid_set(
    int unit,
    int core,
    uint32 system_port,
    uint32 is_valid);

/**
 * \brief - Get System port to base VOQ TC profile map
 *
 * \param [in] unit - chip unit id.
 * \param [in] system_port - system port to map
 * \param [out] tc_profile - queue TC profile
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_get(
    int unit,
    uint32 system_port,
    uint32 *tc_profile);

/**
 * \brief - Map System port to base VOQ map TC profile
 *
 * \param [in] unit - chip unit id.
 * \param [in] system_port - system port to map
 * \param [in] tc_profile - queue TC profile
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_set(
    int unit,
    uint32 system_port,
    uint32 tc_profile);

/**
 * \brief - Clear one entry in System port to base VOQ map
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] system_port - system port entry to clear in map
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_system_port_to_voq_base_map_clear(
    int unit,
    int core,
    uint32 system_port);

/**
 * \brief - Map system TC (on the packet) to a VOQ offset
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] tc - system TC
 * \param [in] dp - DP
 * \param [in] profile - profile (mapped from system port. see dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_set)
 * \param [in] is_flow - indication if this TC should be mapped to the FLOW VOQs (destination is flow)
 * \param [in] is_fmq - mapping for FMQ
 * \param [in] voq_offset - VOQ offset to map to
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_tc_to_voq_offset_map_set(
    int unit,
    int core,
    uint32 tc,
    uint32 dp,
    uint32 profile,
    uint32 is_flow,
    uint32 is_fmq,
    uint32 voq_offset);

/**
 * \brief - Map system TC (on the packet) to a VOQ offset
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] tc - system TC
 * \param [in] dp - DP
 * \param [in] profile - profile (mapped from system port. see dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_set)
 * \param [in] is_flow - indication if this TC should be mapped to the FLOW VOQs (destination is flow)
 * \param [in] is_fmq - mapping for FMQ
 * \param [out] voq_offset - VOQ offset mapped to this TC
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_tc_to_voq_offset_map_get(
    int unit,
    int core,
    uint32 tc,
    uint32 dp,
    uint32 profile,
    uint32 is_flow,
    uint32 is_fmq,
    uint32 *voq_offset);

/**
 * \brief - Map flow to flow profile
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id
 * \param [in] flow_quartet - flow quartet id
 * \param [in] flow_profile - flow profile to set to the flow
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_set(
    int unit,
    int core,
    uint32 flow_quartet,
    uint32 flow_profile);

/**
 * \brief - Get flow to flow profile mapping
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id
 * \param [in] flow_quartet - flow quartet id
 * \param [out] flow_profile - flow profile to set to the flow
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_get(
    int unit,
    int core,
    uint32 flow_quartet,
    uint32 *flow_profile);

/**
 * \brief - Set TC to MC prioroity mapping
 *
 * \param [in] unit - chip unit id.
 * \param [in] tc - TC number (0-7)
 * \param [in] mc_prio - MC priority - high/ low priority.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_set(
    int unit,
    uint32 tc,
    dbal_enum_value_field_multicast_priority_e mc_prio);

/**
 * \brief - Get TC to MC prioroity mapping
 *
 * \param [in] unit - chip unit id.
 * \param [in] tc - TC number (0-7)
 * \param [out] mc_prio - MC priority - high/ low priority.
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_get(
    int unit,
    uint32 tc,
    dbal_enum_value_field_multicast_priority_e * mc_prio);

/**
 * \brief - Set queue number to count all packets sent to invalid destinations
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [in] invalid_dest_queue - invalid queue number
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_invalid_destination_queue_set(
    int unit,
    int core,
    uint32 invalid_dest_queue);

/**
 * \brief - Get queue number to count all packets sent to invalid destinations
 *
 * \param [in] unit - chip unit id.
 * \param [in] core - device core id.
 * \param [out] invalid_dest_queue - invalid queue number
 *
 * \return shr_error_e
 */
shr_error_e dnx_cosq_ipq_dbal_invalid_destination_queue_get(
    int unit,
    int core,
    uint32 *invalid_dest_queue);
/*
 * }
 */

#endif/*_IPQ_DBAL_H_INCLUDED_*/
