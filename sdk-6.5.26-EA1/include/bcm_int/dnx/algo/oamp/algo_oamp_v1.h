/**
 * \file algo_oamp_v1.h Internal DNX OAMP Template Management
ent This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
ent 
ent Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_OAMP_V1_H_INCLUDED
/*
 * { 
 */
#define ALGO_OAMP_V1_H_INCLUDED

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/oam.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#define DNX_BFD_OAMP_TEMPLATE_REQ_INTERVAL         "BFD OAMP Required Interval"

#define DNX_BFD_OAMP_TEMPLATE_MY_DISCR_RANGE_START "BFD OAMP My-Discr Range Start"

#define DNX_BFD_OAMP_TEMPLATE_TOS_TTL              "BFD OAMP TOS-TTL"

#define DNX_BFD_OAMP_TEMPLATE_IPV4_SRC_ADDR        "BFD OAMP IPv4 Source Addr"

#define DNX_BFD_OAMP_TEMPLATE_UDP_SPORT            "BFD OAMP UDP SPORT"

#define DNX_BFD_OAMP_TEMPLATE_MPLS_SPORT           "BFD OAMP MPLS SPORT"

#define DNX_BFD_TEMPLATE_OAMP_SINGLE_HOP_TOS       "BFD OAMP SINGLE-HOP TOS"

#define DNX_BFD_OAMP_TEMPLATE_SEAMLESS_UDP_SPORT   "BFD OAMP SEAMLESS UDP SPORT"

#define DNX_ALGO_OAMP_GENERIC_MEMORY_ENTRY         "OAMP_GENERIC_MEMORY_ENTRY"

#define DNX_ALGO_OAMP_MEP_DB_ENTRY           "OAMP_MEP_DB_ENTRY"

/**
 * Resource name defines for algo oamp
 * \see
 * dnx_oam_init_templates
 * {
 */

/** Structure used for MEP_DB memory type parameters */
typedef struct
{
    /**
     * Relevant for OAM only.
     *  Type of mep_id used for 2byte maid or icc maid group.
     *  MEP_ID for ICC MAID group should be multiple of 4.
     */
    uint8 is_2byte_maid;

    /**
     * Type of MEP ID:
     *          Short('0') - MEP ID resides below the threshold and allocate 1/4 MEP ID entry
     *          Full('1')  - MEP ID resides above the threshold and allocate full MEP ID entry
     */
    uint8 is_full;

    /**
     * MEP_ID Location: if it extra_pool it mean that MEP ID located in in banks 9-12.
     *
     */
    uint8 is_extra_pool;

} dnx_oamp_mep_db_memory_type_t;

/** 
 *  Structure used to store profiles that contain "type of
 *  service/time to live" profiles as template.  These
 *  values are used by the OAMP to construct BFD packets for
 *  transmission.
 */
typedef struct
{
    uint8 tos;
    uint8 ttl;
} dnx_bfd_tos_ttl_t;

/**
 *  Structure used to store profiles that contain "(clustered)
 *  Tx period" profiles as template.  These values are used by
 *  the OAMP for transmission rate, and if cluster ID is not 0,
 *  allow the modification of transmission rate for multiple
 *  endpoints simultaneously.
 */
typedef struct
{
    uint32 tx_period;
    uint32 cluster_profile;
} dnx_bfd_tx_period_t;

/**
 * \brief -  This function calculate the largest index that could be allocated in short entries bank according:
 *           Device frequency, number of banks, number of entries etc...
 *           Generally each entry in MEP DB takes 40 cycles for scan machine, but some of them
 *           could have extra data and this case it takes additional 40 cycles.
 *           Scan machine doesn't have enough time to scan short entries bank in 3.3ms.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  highest_index   -highest index that could be allocated in short bank
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 *
 * \see
 */
void dnx_oamp_v1_mep_db_highest_index_get(
    int unit,
    uint32 *highest_index);

/**
 * \brief - Printing callback for the OAM/BFD MPLS/PWE push 
 *        profile templates.  The push profiles are used to
 *        construct the packets transmitted from MPLS/PWE
 *        OAM/BFD endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The 
 *        OAM/BFD MPLS/PWE PUSH profile structure
 *        dnx_oam_push_profile_t is defined and described above.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Callback function for printing OAM MEP profile template data
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - MEP profile template data to be printed.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_oamp_v1_mep_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Utility function for Remote MEP ID alloc/free
 *          This function helps calculate the allocated/freed id
 *          to be used in the alloc mngr for above threshold ids.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - RMEP id
 * \param [out] alloc_id - alloc_mngr id
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_remote_endpoint_id_above_threshold_to_alloc_mngr_id(
    int unit,
    int rmep_idx,
    int *alloc_id);

/**
 * \brief - Utility function for Remote MEP ID alloc/free
 *          This function helps calculate the allocated/freed id
 *          to be used in the alloc mngr for below threshold ids.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - RMEP id
 * \param [out] alloc_id - alloc_mngr id
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_remote_endpoint_id_below_threshold_to_alloc_mngr_id(
    int unit,
    int rmep_idx,
    int *alloc_id);

/**
 * \brief - Printing callback for the BFD Tx rate 
 *        template, which is used to configure packet
 *        transmissions for BFD accelerated endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The Tx 
 *        rate is a 10-bit integer.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_tx_rate_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD Tx Period Cluster
 *        template. The Tx Period Cluster parameter allows
 *        the user to modify the Tx Period for any number
 *        of accelerated BFD endpoints simultaneously.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is the Tx period cluster (1-8)
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_tx_period_cluster_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the MAC DA MSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_da_mac_msb_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the MAC DA LSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_da_mac_lsb_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD required interval 
 *        template, which is used to negotiate the rate that
 *        remote endpoints transmit to BFD accelerated
 *        endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. 
 *        Intervals are 32-bit integers.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_req_int_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD My Discriminator Range
 *        Start template, which is used to track the references
 *        to the discriminator range start registers by
 *        accekerated BFD endpoints that have IPv4 or MPLS
 *        tunneling.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. This 
 *        offset is a 32-bit integer.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_discr_range_start_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD ToS TTL template. 
 *        These two values are used by the OAMP to construct BFD
 *        packet to transmit for accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. These
 *        values are one byte each.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_tos_ttl_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD source address 
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is an IPv4 address - 4 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_ipv4_src_addr_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD UDP source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a port - 2 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM sdnx_bfd_udp_src_port_profile_print_cbhell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_udp_src_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD MPLS source port
 *        template. This IPv4 address is used by the OAMP to
 *        construct the BFD packet to transmit for an
 *        accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a port - 2 bytes.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_mpls_src_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the BFD MPLS single hop TOS
 *        template. This TOS value is used by the OAMP to
 *        construct the BFD packet to transmit for a single-hop
 *        IPv4 accelerated endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data. The data
 *        is a TOS - 1 byte.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_bfd_oamp_v1_single_hop_tos_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the PP Port profile template,
 *          which is used for mapping local port(pp port) to system port.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.
 *
 * \return
 *
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oamp_v1_pp_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - This function finds the next allocated short
 *          entry with sub-index 0
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] icc_id_in_out - MEP ID.  As input, this is
 *        the starting point of the search.  As output, this is
 *        the result.
 * \param [out] entry_found - TRUE if search successful, FALSE
 *        otherwise
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_get_next_icc_entry(
    int unit,
    uint32 *icc_id_in_out,
    uint8 *entry_found);

/**
 * \brief - This function finds the next free short
 *          entry with sub-index 0
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] icc_id_out - resulting MEP ID.
 * \param [out] entry_not_found - FALSE if search successful, TRUE
 *        otherwise
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_v1_mep_id_get_free_icc_id(
    int unit,
    uint32 *icc_id_out,
    uint8 *entry_not_found);

/**
 * \brief - Function map mep_id to pool and index inside pool.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  mep_id   - mep id
 * \param [out] memory_type - map id memory type
 * \param [out] index    - index of mep id inside pool
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_mep_id_alloc
 *   dnx_oam_mep_id_dealloc
 *   dnx_oamp_mep_db_memory_type_t
 */
void dnx_mep_id_to_pool_type_and_index(
    int unit,
    uint32 mep_id,
    dnx_oamp_mep_db_memory_type_t * memory_type,
    int *index);

/**
 * \brief - This function allocate mep id.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  flags - DNX_ALGO_RES_ALLOCATE_WITH_ID flag in use. Allocate specific mep id.
 * \param [in]  mep_id_memory_type - requested memory type
 * \param [out] mep_id - MEP ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_v1_mep_id_alloc(
    int unit,
    uint32 flags,
    dnx_oamp_mep_db_memory_type_t * mep_id_memory_type,
    uint32 *mep_id);

/**
 * \brief -This  function checks if mep id is allocated
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id  - mep id that is checked.
 * \param [out] is_alloc - is allocated or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_v1_mep_id_is_allocated(
    int unit,
    uint32 mep_id,
    uint8 *is_alloc);

/**
 * \brief -This  function release mep id from pool.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id  - mep id that should be free
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_v1_mep_id_dealloc(
    int unit,
    uint32 mep_id);

/**
 * \brief - initialize templates for all oamp profile types for JR2 family.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_v1_oam_init(
    int unit);

/**
 * \brief - initialize templates for all bfd oamp profile types for JR2 family.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_v1_bfd_init(
    int unit);

/**
 * \brief - initialize templates for all oamp profile types for JR2 family.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_v1_init(
    int unit);

/**
 * \brief - Utility function for reserving IDs for
 *          OAMP_MEP_DB entries.  This is used
 *          automatically by dbal and should not
 *          be used otherwise.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_index - Index to be used
 *        for resource allocation.
 */
shr_error_e dnx_algo_oamp_v1_mep_entry_alloc(
    int unit,
    int *oamp_entry_index);

/**
 * \brief - Utility function for freeing IDs for
 *          OAMP_MEP_DB entries.  This is used
 *          automatically by dbal and should not
 *          be used otherwise.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_index - Index of resource
 *        to be freed.
 */
shr_error_e dnx_algo_oamp_v1_mep_entry_dealloc(
    int unit,
    int oamp_entry_index);

/**
 * \brief - This function allocate OAMP generic memory entries.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  flags - DNX_ALGO_RES_ALLOCATE_WITH_ID flag in use. Allocate specific mep id.
 * \param [in]  nof_entries - Number of entries to allocate
 * \param [out] entry_index - Entriy index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_v1_generic_memory_alloc(
    int unit,
    uint32 flags,
    int nof_entries,
    int *entry_index);

/**
 * \brief - This function free OAMP generic memory entries.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  nof_entries - Number of entries to free
 * \param [in] entry_index - Entriy index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_v1_generic_memory_dealloc(
    int unit,
    int nof_entries,
    int entry_index);

/*
 * } 
 */
#endif /* ALGO_OAMP_V1_H_INCLUDED */
