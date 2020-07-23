/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file bcm_int/dnx/failover/failover.h
 *
 *
 * Reserved.$
 */

#ifndef _DNX_FAILOVER_H_INCLUDED_
/*
 * {
 */
#define _DNX_FAILOVER_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/failover.h>
#include <bcm/mpls.h>
#include <bcm/vlan.h>

#include <soc/dnx/dbal/dbal.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_bitstream.h>

/*
 * }
 * Includes
 */

/*
 * MACROs 
 * {  
 */
/** 
 * \brief -  
 * Failover types to be used in order to distinguish between failover object types
 */
#define DNX_FAILOVER_TYPE_NONE              (0) /* No Failover type */
#define DNX_FAILOVER_TYPE_FEC               (2) /* Failover ID of type FEC (1:1 UC) */
#define DNX_FAILOVER_TYPE_INGRESS           (3) /* Failover ID of type Ingress (1+1) */
#define DNX_FAILOVER_TYPE_ENCAP             (4) /* Failover ID of type Egress (1:1 MC) */

/**
 * \brief - used to enocode-decode failover_ids with id (protection pointer) and type 
 */
#define DNX_FAILOVER_TYPE_SHIFT             (29)
#define DNX_FAILOVER_VAL_SHIFT              (0)
#define DNX_FAILOVER_TYPE_MASK              (0x7)
#define DNX_FAILOVER_VAL_MASK               (0x1FFFFFFF)

/**
 * \brief - set failover type (see BCM_FAILOVER_TYPE_XXX) and id (protection pointer)
 */
#define DNX_FAILOVER_SET(object_id, failover_id, failover_type)                         \
        ((object_id) = (((failover_type) << DNX_FAILOVER_TYPE_SHIFT) |                  \
         (((failover_id) & DNX_FAILOVER_VAL_MASK) << DNX_FAILOVER_VAL_SHIFT)))

/**
 * \brief - get failover type - see BCM_FAILOVER_TYPE_XXX
 */
#define DNX_FAILOVER_TYPE_GET(failover_type, failover_id)                               \
        ((failover_type) = (((failover_id) >> DNX_FAILOVER_TYPE_SHIFT) &                \
            DNX_FAILOVER_TYPE_MASK))                  \

/**
 * \brief - get ID (i.e. protection pointer)
 */
#define DNX_FAILOVER_ID_GET(failover_id_val, failover_id)                               \
        ((failover_id_val) = (((failover_id) >> DNX_FAILOVER_VAL_SHIFT) &               \
            DNX_FAILOVER_VAL_MASK))

/**
 * \brief - Defines and MACROs for protection pointer encoding/decoding for usage in In-LIFs
 * The protection pointer value written in In-LIFs is 14 bits and should be derived from the Failover ID and encoded in the following way:
 * protection_pointer = ((failover_id & (mask nof_msb)) << nof_lsb) | (failover_id >> nof_msb)
 * The reason for this is that when the HW wants to check the protection path selected, it is taking the protection pointer from the In-LIF
 * and it is using it to determine which entry and field of memory IPPA_VTT_PATH_SELECT to read. In LIF protection pointer is 14 bits, but 
 * the HW is expecting 15 bits - the 12 LSB from the protection pointer indicate the entry(0-4095) and the 3 MSB indicate one of the 8 fields (0-7).
 * To compensate this difference only fields 0-3 are used and this way.
 */

/**
 * \brief - Encode protection pointer for In-LIF
 */
#define DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer, failover_id)  \
do                              \
{                               \
    uint32 failover_arr[1] = {failover_id}; \
    protection_pointer = 0;     \
    SHR_IF_ERR_EXIT(utilex_bitstream_field_swap(failover_arr, dnx_data_failover.path_select.in_lif_protection_pointer_nof_lsb_get(unit),             \
                                                dnx_data_failover.path_select.in_lif_protection_pointer_nof_msb_get(unit), (uint32 *)&(protection_pointer))); \
} while (0)

/**
 * \brief - Decode protection pointer from In-LIF
 */
#define DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(failover_id, protection_pointer)  \
do                      \
{                       \
   uint32 protection_ptr_arr[1] = {protection_pointer}; \
   failover_id = 0;     \
   SHR_IF_ERR_EXIT(utilex_bitstream_field_swap(protection_ptr_arr, dnx_data_failover.path_select.in_lif_protection_pointer_nof_msb_get(unit),  \
                                               dnx_data_failover.path_select.in_lif_protection_pointer_nof_lsb_get(unit), (uint32 *)&(failover_id)));       \
} while (0)

/**
 * \brief - Returns TRUE for standby path.
 * A given failover entity (FEC or protection pointer) can be either primary(working) or secondary(standby).
 */
#define DNX_FAILOVER_IS_STANDBY(failover_path_select) (failover_path_select == 0)

/**
 * \brief - failover_id value that indicates protection is disabled
 */
#define DNX_FAILOVER_PROTECTION_DISABLE_VALUE   0

/**
 * \brief - Returns TRUE if protection is enabled.
 * If failover_id (protection pointer) is 0, then protection is not enabled
 */
#define DNX_FAILOVER_IS_PROTECTION_ENABLED(failover_id) (failover_id != DNX_FAILOVER_PROTECTION_DISABLE_VALUE)

/**
 * \brief - L2 protection information.
 */
typedef struct
{
    /** FEC protection indication */
    int is_fec_protected;

    /** FEC protection indication */
    bcm_failover_t failover_id;

    /** FEC/ingress protection path */
    bcm_gport_t failover_port_id;

    /** ingress protection indication */
    bcm_failover_t ingress_failover_id;

    /** egress protection indication */
    bcm_failover_t egress_failover_id;

    /** egress protection path */
    bcm_gport_t egress_failover_port_id;

    /** failover mc group for reverse path indication */
    int is_ingress_mc;

    /** reverse path ingress (1+1) protection mc group */
    bcm_multicast_t failover_mc_group;

} dnx_failover_l2_protection_info_t;

/*
 * MACROs
 * }
 */
/**
 * \brief Initialize dnx failover module (protection path select).
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_failover_init(
    int unit);

/**
 * \brief - verify that the failover ID was allocated to be used for 'expected_type' and 'expected_hierarchy',
 *             otherwise throw an error with proper error message.
 *
 * \param [in] unit - The unit number.
 * \param [in] expected_type - DNX_FAILOVER_TYPE_XXX
 * \param [in] expected_hierarchy - expected hierarchy for FEC type or 0 for any other,
 * \param [in] failover_id - failover_id got from the user to verify.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_failover_id_valid_verify(
    int unit,
    int expected_type,
    dbal_enum_value_field_hierarchy_level_e expected_hierarchy,
    bcm_failover_t failover_id);

/**
 * \brief - retrieve protection information for L2 objects (either AC or PWE)
 *
 * \param [in] unit - Unit #
 * \param [in] vlan_port - vlan port info.
 * \param [in] mpls_port - mpls port info.
 * \param [out] protection_info - protection information. /see dnx_failover_l2_protection_info_t.
 *
 * \return shr_error_e Standard error handling
 */
shr_error_e dnx_failover_l2_protection_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_mpls_port_t * mpls_port,
    dnx_failover_l2_protection_info_t * protection_info);

/**
 * \brief - Verify protection information for L2 objects (either AC or PWE)
 *
 * \param [in] unit - Unit #
 * \param [in] vlan_port - vlan port info.
 * \param [in] mpls_port - mpls port info.
 *
 * \return shr_error_e Standard error handling
 *
 * \remark - L2 protection verification should not verify whether the provided FEC pointer is a valid allocated FEC
 *           since the seqeunce requires L2 object creation prior to the FEC creation.
 */
shr_error_e dnx_failover_l2_protection_info_verify(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_mpls_port_t * mpls_port);

/**
 * \brief - Verify protection information for L2 objects (either AC or PWE)
 *
 * \param [in] unit - Unit #
 * \param [in] gport - facility port gport (either system port or trunk).
 * \param [in] enable - 1 is up, 0 is down.
 *
 * \return shr_error_e Standard error handling
 */
shr_error_e dnx_failover_facility_port_status_set(
    int unit,
    bcm_gport_t gport,
    int enable);

#endif /*_DNX_FAILOVER_H_INCLUDED_*/
