/** \file port_tpid.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PORT_TPID_H_INCLUDED
/* { */
#define PORT_TPID_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/port_tpid_types.h>

/**
 * \brief 
 * Tag format for untag packet
 * \see
 * dnx_algo_port_tpid_init
 */
#define TAG_FORMAT_NONE 0

#define DNX_PORT_TPID_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP 0
#define DNX_PORT_TPID_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT 1

/*
 * The size of each LLVP (Link Layer VLAN Processing) table entry buffer
 *
 * #define DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT 19
 *
 *  Note:
 *      This definition is moved to SW State "port tpid" files
 * See:
 *      port_tpid.xml
 */

/*
 * The key of each LLVP (Link Layer VLAN Processing) table entry buffer is:
 * Ingress:
 * [6]   - 1 bit - is_prio
 * [5:3] - 3 bits - outer tpid index
 * [2:0] - 3 bits - inner tpid index
 * Egress:
 * [6:4] - 3 bits - outer tpid index
 * [3]   - 1 bit - is_prio
 * [2:0] - 3 bits - inner tpid index
 *
 * #define DNX_PORT_TPID_LLVP_KEY_SIZE (1+3+3)
 * #define DNX_PORT_TPID_LLVP_BLOCK_SIZE (SAL_BIT(DNX_PORT_TPID_LLVP_KEY_SIZE))
 *
 *  Note:
 *      This definitions are moved to SW State "port tpid" files
 * See:
 *      port_tpid.xml
 */

/*
 * The template of each LLVP profile has 19*2^7=2432 bits
 * It is implemented by 76 uint32's (76*32=2432)
 *
 * #define DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_UINT32 ((DNX_PORT_TPID_BUFFER_BITS_PER_TAG_STRCT*DNX_PORT_TPID_LLVP_BLOCK_SIZE)/32)
 *
 *  Note:
 *      This definition is moved to SW State "port tpid" files
 * See:
 *      port_tpid.xml
 */
#define DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_BYTES  (DNX_PORT_TPID_CLASS_TEMPLATE_SIZE_NUM_OF_UINT32*4)

/** 
 * \brief 
 * Parser fields that are part of LLVP table key 
 */
typedef struct dnx_port_tpid_llvp_parser_info_s
{
    /*
     * Is outer tag is priority tag - 
     *   1 - yes, the outer tag is priority tag.
     *   0 - no, the outer tag is not a priority tag.
     */
    uint32 is_outer_prio;
    /*
     * The outer TPID index in the global TPID table.
     */
    uint32 outer_tpid;
    /*
     * The innder TPID index in the global TPID table. 
     */
    uint32 inner_tpid;
} dnx_port_tpid_llvp_parser_info_t;

/**
 * \brief 
 * ingress LLVP table payload.
 */
typedef struct dnx_port_tpid_ingress_llvp_entry_s
{
    /*
     * Incoming VID exist: 
     * If set then the incoming packet has a VLAN tag that is not a priority tag.
     */
    int incoming_vid_exist;
    /*
     * Incoming TAG exist: 
     * If set then the incoming packet has a VLAN tag (this VLAN tag may or may not be a priority tag) 
     */
    int incoming_tag_exist;
    /*
     * Incoming S_TAG exist: 
     * If set then the packet has an S-Tag. 
     */
    int incoming_s_tag_exist;
    /*
     * Frame type action: 
     * Actions to perform (drop, accept). Used to identify invalid packet format. 
     */
    int frame_type_action;
    /*
     * LLVP TAG format: 
     * The LLVP Incoming Tag Structure. 
     * Used together with the VLAN-Edit-Profile (which a result of the port x VLAN translation) to generate the VLAN edit command.
     */
    int llvp_tag_format;
    /*
     * IVEC index: 
     * Default Ingress VLAN edit command ID 
     */
    int ivec_index;
    /*
     * PCP/DEI profile: 
     * PCP-DEI mapping profile. 
     * Default translation QoS map ID 
     */
    int pcp_dei_profile;
} dnx_port_tpid_ingress_llvp_entry_t;

/*
 * egress LLVP table payload
 */
typedef struct dnx_port_tpid_egress_llvp_entry_s
{
    dbal_enum_value_field_c_tag_offset_type_e c_tag_offset;
    int packet_has_c_tag;
    int packet_has_pcp_dei;
    int llvp_tag_format;
} dnx_port_tpid_egress_llvp_entry_t;

/**
 * \brief Convert LLVP entry internal buffer to bcm_port_tpid_class_t
 */
shr_error_e dnx_port_tpid_class_set_buff_to_tpid_class(
    int unit,
    uint32 buff,
    bcm_port_tpid_class_t * tpid_class);

/**
 * \brief - 
 * Calculate outer tag is priority value 
 *  
 * \param [in] unit - Device number
 * \param [in] tpid_class - Pointer to a TPID class structure that includes the port
 *          and the data to set.
 * \param [out] tag_priority_val - pointer to the result: 
 * 0 - outer tag is not a priority tag 
 * 1 - outer tag is a priority tag
 * \param [out] nof_tag_priority_vals - number of results: 
 * 1 - only one result (outer tag is/isn't a priority tag)
 * 2 - two results (outer tag is + isn't a priority tag)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see 
 * bcm_dnx_port_tpid_class_set
 */
shr_error_e dnx_port_tpid_class_set_tag_priority_get(
    int unit,
    bcm_port_tpid_class_t * tpid_class,
    int *tag_priority_val,
    int *nof_tag_priority_vals);

/**
 * \brief Update a single entry of LLVP profile block
 * which includes 2^7 entries
 */
shr_error_e dnx_port_tpid_class_set_tpid_class_buffer_update(
    int unit,
    int is_ingress,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 tag_buff,
    uint32 *tpid_class_buffer);

/**
 * \brief - 
 * Convert parser_info to Ingress LLVP table index. 
 * The Ingress LLVP table keys are: 
 *  - Is-Priority-Tag (1b)
 *  - Layer-Qualifier.First-TPID(3b)
 *  - Layer-Qualifier.Second-TPID (3b)
 * 
 * \param [in] unit - relevant unit
 * \param [in] llvp_parser_info -  pointer to Parser fields that 
 *        are part of LLVP table key
 * \param [out] tag_structure_index - the Ingress LLVP table 
 *        index
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_tpid_class_set
 *   bcm_dnx_port_tpid_class_get
 */
shr_error_e dnx_port_tpid_class_set_parser_info_to_ingress_llvp_index(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    int *tag_structure_index);

/**
 * \brief - 
 * Convert parser_info to Egress LLVP table index. 
 * The Egress LLVP table keys are:
 *  - outer_tag (3b)
 *  - ethernet_tag_format_outer_tag_is_priority(1b)
 *  - inner_tag (3b)
 * 
 * \param [in] unit - relevant unit
 * \param [in] llvp_parser_info -  pointer to Parser fields that 
 *        are part of LLVP table key
 * \param [out] tag_structure_index - the Egress LLVP table 
 *        index
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_tpid_class_set
 *   bcm_dnx_port_tpid_class_get
 */
shr_error_e dnx_port_tpid_class_set_parser_info_to_egress_llvp_index(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    int *tag_structure_index);

/**
 * \brief Write single LLVP entry pointed by its offset
 */
shr_error_e dnx_port_tpid_class_set_ingress_llvp_entry_write(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 ingress_tag_buff);

/**
  * \brief Convert buffer and parser_info to LLVP entry 
  * Convert ingress tag buffer to tpid class object, and convert it to llvp_entry 
  */
shr_error_e dnx_port_tpid_class_set_ingress_llvp_entry_get(
    int unit,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    uint32 ingress_tag_buff,
    dnx_port_tpid_ingress_llvp_entry_t * dnx_port_tpid_ingress_llvp_entry);

/**
 * \brief Write single entry to ingress LLVP table
 */
shr_error_e dnx_port_tpid_class_set_ingress_llvp_entry_to_dbal(
    int unit,
    int llvp_profile,
    dnx_port_tpid_llvp_parser_info_t * llvp_parser_info,
    dnx_port_tpid_ingress_llvp_entry_t * dnx_port_tpid_ingress_llvp_entry);

/** 
  * \brief Write a LLVP block to HW
  */
shr_error_e dnx_port_tpid_class_set_ingress_llvp_block_write(
    int unit,
    int llvp_profile,
    uint32 *tpid_class_buffer);

/**
 * \brief - 
 * This function clears ingress LLVP-Profile block
 */
shr_error_e dnx_port_tpid_class_set_ingress_llvp_block_clear(
    int unit,
    int llvp_profile);

/**
 * \brief - 
 * This function clears egress LLVP-Profile block
 */
shr_error_e dnx_port_tpid_class_set_egress_llvp_block_clear(
    int unit,
    int llvp_profile);

/**
 * \brief - 
 * This function sets the LLVP-Profile value per 
 * Vlan-Domain for usage by the LLR stage. 
 * \param [in] unit - relevant unit.
 * \param [in] vlan_domain - vlan-domain.
 * \param [in] llvp_profile - LLVP profile ID.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *  The SDK maintain two identical tables: One in the LLR per VLAN-Domain and the other in the VTT per PP-Port.
 * \see
 * bcm_dnx_port_tpid_class_set 
 */
shr_error_e dnx_port_tpid_class_ingress_vlan_domain_llvp_profile_set(
    int unit,
    int vlan_domain,
    int llvp_profile);

/**
 *  
 * \brief -
 * Initialize Port TPID Class - init its SW state parameters.
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 * 
 * \remark
 * None.
 * \see
 * None.
 */
shr_error_e dnx_port_tpid_class_default_init(
    int unit);

/**
 *  
 * \brief - Initialization function for Ingress PP Port
 * This function allocs an Ingress LLVP
 * classification profile per the PP port.
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - ingress logical port.
 * \return
 *   shr_error_e
 * 
 * \remark
 * None
 * \see
 * None.
 */
shr_error_e dnx_port_tpid_class_ingress_default_per_port_init(
    int unit,
    bcm_gport_t logical_port);

/**
 *  
 * \brief - De-Initialization function for Ingress PP Port
 * This function frees the alloced Ingress LLVP
 * classification profile per the PP port.
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - ingress logical port.
 * \return
 *   shr_error_e
 * 
 * \remark
 * None
 * \see
 * None.
 */
shr_error_e dnx_port_tpid_class_ingress_default_per_port_deinit(
    int unit,
    bcm_gport_t logical_port);

/**
 *  
 * \brief - Initialization function for Egress PP Port
 * This function allocs an Egress LLVP
 * classification profile per the PP port.
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - egress logical port.
 * \return
 *   shr_error_e
 * 
 * \remark
 * None.
 * \see
 * None.
 */
shr_error_e dnx_port_tpid_class_egress_default_per_port_init(
    int unit,
    bcm_gport_t logical_port);

/**
 *  
 * \brief - De-Initialization function for Egress PP Port
 * This function fress and alloced an Egress LLVP
 * classification profile per the PP port.
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - egress logical port.
 * \return
 *   shr_error_e
 * 
 * \remark
 * None.
 * \see
 * None.
 */
shr_error_e dnx_port_tpid_class_egress_default_per_port_deinit(
    int unit,
    bcm_gport_t logical_port);

/**
 * \brief - 
 * Internal function for verification whether a SW TAG-Struct was created
 * by the bcm_dnx_port_tpid_class_create function.
 * \param [in] unit - relevant unit
 * \param [in] is_symmetric - is the TAG-Struct symmetric.
 * \param [in] is_ingress_only - is the TAG-Struct ingress only.
 * \param [in] tag_format_class_id - the tag_struct.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   bcm_dnx_port_tpid_class_set
 *   bcm_dnx_port_tpid_class_get
 *   bcm_dnx_vlan_translate_action_class_set
 *   bcm_dnx_vlan_translate_action_class_get
 */
shr_error_e dnx_port_tpid_class_sw_id_is_created_verify(
    int unit,
    int is_symmetric,
    int is_ingress_only,
    bcm_port_tag_format_class_t tag_format_class_id);

/**
 * \brief - 
 * TAG-Struct handling - internal function.
 *  Retrieve from the SW TAG-Struct ID it's HW TAG-Struct ID.
 * \see
 * bcm_dnx_port_tpid_class_set
 * bcm_dnx_port_tpid_class_get
 * bcm_dnx_port_tpid_class_destroy
 * bcm_dnx_vlan_translate_action_class_set
 * bcm_dnx_vlan_translate_action_class_get
 */
shr_error_e dnx_port_tpid_class_sw_id_get_info(
    int unit,
    bcm_port_tag_format_class_t tag_format_class_id,
    int *hw_id_ingress_value,
    int *hw_id_egress_value);

/**
 * \brief - 
 * TAG-Struct handling - internal function.
 *  Retrieve from the HW TAG-Struct ID it's SW TAG-Struct ID.
 * \see
 * bcm_dnx_port_tpid_class_set
 * bcm_dnx_port_tpid_class_get
 */
shr_error_e dnx_port_tpid_class_hw_id_get_info(
    int unit,
    int is_ingress,
    int hw_id_key,
    bcm_port_tag_format_class_t * tag_format_class_id);

/* } */
#endif /* PORT_TPID_H_INCLUDED */
