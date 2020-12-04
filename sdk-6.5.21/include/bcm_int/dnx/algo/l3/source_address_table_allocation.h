/**
 * \file source_address_table_allocation.h Internal DNX L3 Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef SOURCE_ADDRESS_TABLE_ALLOCATION_H_INCLUDED
/*
 * { 
 */
#define SOURCE_ADDRESS_TABLE_ALLOCATION_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/multi_set.h>

/**
 * \brief Source address table allocation management. 
 *  
 * The source address table is used for adding source addresses to packets.  It can hold either mac, 
 *  IPv4 or IPv6 entries. 
 * The algorithm manages the profile ID according to profile data which present  the requirements from 
 *  the source address table. 
 * The profile returned represents the access key for this table.
 */

/*
 * { 
 */

/*
 * \brief Get the illegal LSBs in a mac source address.
 */
#define SOURCE_ADDRESS_MY_MAC_LSB_SIZE_IN_BITS          (10)
#define SOURCE_ADDRESS_MY_MAC_SECOND_BYTE_MASK UTILEX_BITS_MASK(SOURCE_ADDRESS_MY_MAC_LSB_SIZE_IN_BITS - SAL_UINT8_NOF_BITS - 1,0)

#define SOURCE_ADDRESS_GET_MY_MAC_LSB(my_mac) ( ((my_mac[4] & SOURCE_ADDRESS_MY_MAC_SECOND_BYTE_MASK) << SAL_UINT8_NOF_BITS) |\
                                       my_mac[5])

/** 
 * \brief Source address table supported types under one union structure to ease on the algorithm management.
 */
typedef union
{
    /*
     * MAC address.
     */
    bcm_mac_t mac_address;
    /*
     * IPv4 address.
     */
    bcm_ip_t ipv4_address;
    /*
     * IPv6 address.
     */
    bcm_ip6_t ipv6_address;
} source_address_t;

/** 
 * \brief Identifier of the entry type.
 */
typedef enum
{
    source_address_type_invalid = -1,
    source_address_type_mac,
    source_address_type_ipv4,
    source_address_type_ipv6,
    source_address_type_mac_dual_homing,
    source_address_type_full_mac,
    source_address_type_count
} source_address_type_e;

/** 
 * \brief The data to be stored in the source address table template. 
 *  
 * This data is composed of the address, and the address type.
 */
typedef struct
{
    /*
     * Address type. Used to identify the entry.
     */
    source_address_type_e address_type;
    /*
     * Address.
     */
    source_address_t address;
} source_address_entry_t;

/*
 * } 
 */

/**
* \brief
*   Intialize the source address table algorithm.
*  
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_init(
    int unit);

/**
* \brief
*   Dentialize the source address table algorithm.
*   Currently doens't do anything, since template manager doesn't require
*    deinitialization per template, and the sw state is deinitialized in
*    dnx_algo_l3_deinit.
*  
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*  \par INDIRECT INPUT:
*    - DBAL table sizes, used to initialize source address table template.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_deinit(
    int unit);

/**
* \brief
*   Print an entry of the source address template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*  
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] data -
*      Pointer of the struct to be printed.
*      \b As \b input - \n
*       The pointer is expected to hold a struct of type source_address_entry_t. \n
*       It's the user's responsibility to verify it beforehand.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    dnx_algo_template_print_data_cb
*    shr_error_e
 */
void dnx_algo_l3_source_address_table_entry_print_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Callback to allocate a free profile of an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] flags -
 *      SW_STATEALGO_TEMPLATE_ALLOCATE_* flags
 *   \param [in] nof_references -
 *      Number of references to be allocated.
 *  \param [in] profile_data -
 *      Pointer to memory holding template data to be saved to a profile.
 *  \param [in] extra_arguments -
 *      Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
 *  \param [in,out] profile -
 *      Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [in,out] first_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_allocate(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
 * \brief
 *   Create a new instance of dnx_algo_l3_source_address_table.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] create_data -
 *      Pointed memory contains setup parameters required for the
 *      creation of the template.
 *  \param [in] extra_arguments -
 *      Placeholder to fill prototype requirement for advanced algorithms.
 *      Should be NULL for this procedure.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_create(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set_template,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Callback to free an allocated profile in an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] profile -
 *      Profile to be freed.
 *   \param [in] nof_references -
 *      Number of references to be freed.
 *  \param [in,out] last_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_free(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

/**
 * \brief
 *   Free all reference to all profiles of this template.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_l3_source_address_table_clear(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template);
/*
 * } 
 */
#endif /* SOURCE_ADDRESS_TABLE_ALLOCATION_H_INCLUDED */
