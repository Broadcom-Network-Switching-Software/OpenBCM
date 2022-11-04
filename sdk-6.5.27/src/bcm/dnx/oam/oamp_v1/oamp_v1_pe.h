/** \file oamp_v1_pe.h
 * OAMP PE support - contains internal functions and definitions 
 * for support of the OAMP PE functionality
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _OAMP_PE_H_INCLUDED__
/*
 * {
 */
#define _OAMP_PE_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include "include/bcm/oam.h"
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */

/*
 * Currently function DM Jumbo TLV uses 0xc00 entries of
 * generic memory, so the base address for down egress
 * injection IVEC is 0xc00 and 2 entries will be used.
 */
#define DNX_OAMP_PE_DOWN_EGRESS_INJ_IVEC_GEN_MEM_BASE 0x0c00

/** Generic memory address to store IVEC delete one VLAN tag. */
#define DNX_OAMP_PE_DELETE_ONE_VLAN_TAG_IVEC_ADDRESS \
    DNX_OAMP_PE_DOWN_EGRESS_INJ_IVEC_GEN_MEM_BASE

/** Generic memory address to store IVEC delete two VLAN tags. */
#define DNX_OAMP_PE_DELETE_TWO_VLAN_TAGS_IVEC_ADDRESS \
    (DNX_OAMP_PE_DOWN_EGRESS_INJ_IVEC_GEN_MEM_BASE + 1)

/** factor to generic memory address to store IVEC delete one VLAN tag. */
#define DNX_OAMP_PE_DELETE_ONE_VLAN_TAG_IVEC_FACTOR 1

/** factor to generic memory address to store IVEC delete two VLAN tags. */
#define DNX_OAMP_PE_DELETE_TWO_VLAN_TAGS_IVEC_FACTOR 2

/**
* \brief
*  Initialize OAMP-PE with all of its configurations.
*   \param [in] unit  -  Relevant unit.
*   \param [in] profile  -  MEP-PE profile name in SW state
*   \param [out] profile_id  -  MEP-PE profile saved id in HW
* \return
*   shr_error_e
*/
shr_error_e dnx_oamp_pe_sw_mep_pe_profile_get(
    int unit,
    dbal_enum_value_field_mep_pe_profile_sw_e profile,
    uint32 *profile_id);

/**
* \brief
*  Get preconfigured UDH header size.
*   \param [in] unit  -  Relevant unit.
*   \param [out] udh_size  -  preconfigured UDH size
* \return
*   shr_error_e
*/
shr_error_e oamp_pe_udh_header_size_get(
    int unit,
    uint32 *udh_size);

/**
* \brief
*  Initialize OAMP-PE with all of its configurations.
*   \param [in] unit  -  Relevant unit.
* \return
*   shr_error_e
*/
shr_error_e dnx_oamp_pe_module_init(
    int unit);

shr_error_e dnx_up_mep_dm_recycle_port_set(
    int unit,
    uint8 core_id,
    uint32 port);

shr_error_e dnx_up_mep_dm_recycle_port_get(
    int unit,
    uint8 core_id,
    uint32 *port);

/*
 * }
 */
#endif /* _OAMP_PE_H_INCLUDED__ */
