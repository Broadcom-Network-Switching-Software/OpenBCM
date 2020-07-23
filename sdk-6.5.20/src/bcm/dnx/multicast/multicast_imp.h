/** \file src/bcm/dnx/multicast/multicast_imp.h
 * Internal DNX MIRROR PROFILE APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MULTICAST_IMP_INCLUDED__
/*
 * { 
 */
#define _DNX_MULTICAST_IMP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/multicast.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
/*
 * MACROs
 * {
 */

#define DNX_MULTICAST_END_OF_LIST_PTR                   ((1 << dnx_data_multicast.params.nof_mcdb_entries_bits_get(unit))-1)
#define DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_ENTRY    32

#define DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit)              dnx_data_port.general.nof_tm_ports_get(unit)
#define DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit)                   (DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit) / DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_ENTRY)

#define DNX_MULTICAST_IS_EMPTY_GROUP(unit, entry_val) ((entry_val.format == DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT && \
                                                        entry_val.dest[0] == DNX_MULTICAST_ING_INVALID_DESTINATION(unit) && \
                                                        entry_val.next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR) ? TRUE : FALSE)

#define DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val) ((entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT && \
                                              entry_val.bmp_ptr == DNX_MULTICAST_END_OF_LIST_PTR && \
                                              entry_val.next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR) ? TRUE : FALSE)

#define DNX_MULTICAST_IS_BITMAP(flags) ((flags & BCM_MULTICAST_TYPE_PORTS_GROUP) ? 1 : 0)
#define DNX_MULTICAST_IS_TDM(flags) ((flags & BCM_MULTICAST_EGRESS_TDM_GROUP) ? 1 : 0)
#define DNX_MULTICAST_IS_EGRESS_GROUP(flags) ((!DNX_MULTICAST_IS_BITMAP(flags) && DNX_MULTICAST_IS_EGRESS(flags)) ? 1 : 0)
#define DNX_MULTICAST_IS_INGRESS_GROUP(flags) ((!DNX_MULTICAST_IS_BITMAP(flags) && DNX_MULTICAST_IS_INGRESS(flags)) ? 1 : 0)
#define DNX_MULTICAST_IS_EGRESS_BITMAP(flags) ((DNX_MULTICAST_IS_BITMAP(flags) && DNX_MULTICAST_IS_EGRESS(flags)) ? 1 : 0)
#define DNX_MULTICAST_IS_INGRESS_BITMAP(flags) ((DNX_MULTICAST_IS_BITMAP(flags) && DNX_MULTICAST_IS_INGRESS(flags)) ? 1 : 0)

typedef enum dnx_multicast_entry_iter_action_e
{

    /*
     * removes the entries specified at the rep_array. if entry will not be found _SHR_E_NOT_FOUND error will be returned.
     */
    DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST,

    /*
     * removes all the entries in the link list
     */
    DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL,

    /*
     * copies the content of the link list to the rep_array
     */
    DNX_MULTICAST_ENTRY_ITER_GET,

    /*
     * update the HW bitmap with the replications which are no longer exist.
     */
    DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST,

    /*
     * Adds the bitmap to the existing HW bitmap
     */
    DNX_MULTICAST_ENTRY_ITER_BMP_ADD
} dnx_multicast_entry_iter_action_t;

/*
 * The MCDB divided to the following regions
 */
typedef enum multicast_group_region_e
{
    DNX_MULTICAST_GROUP_REGION_INGRESS,
    DNX_MULTICAST_GROUP_REGION_EG_CORE0,
    DNX_MULTICAST_GROUP_REGION_EG_CORE1,
    DNX_MULTICAST_GROUP_REGION_BITMAP_INGRESS,
    DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE0,
    DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE1,
} dnx_multicast_group_region_t;

/*
 * Enum indicating the action to be taken in regards of egress trunk profile
 */
typedef enum multicast_trunk_action_e
{
    DNX_MULTICAST_TRUNK_ACTION_INVALID = -1,
    DNX_MULTICAST_TRUNK_ACTION_FIRST = 0,
    DNX_MULTICAST_TRUNK_ACTION_CREATE = DNX_MULTICAST_TRUNK_ACTION_FIRST,
    DNX_MULTICAST_TRUNK_ACTION_DESTROY,
    DNX_MULTICAST_TRUNK_ACTION_COUNT
} dnx_multicast_trunk_action_t;

/**
 * holds the MCDB entry values. not all the entries are relevant for specific format
 */
typedef struct multicast_mcdb_dbal_fields_s
{
    uint32 next_entry_id;
    uint32 bmp_ptr;
    uint32 cud;
    uint32 dest[4];
    uint32 link_list_ptr;
    uint32 prev_entry;
    uint32 bmp_val;
    uint32 bmp_offset;
    dbal_enum_value_result_type_mcdb_e format;
} dnx_multicast_mcdb_dbal_field_t;

/**
 * \brief - checks whether egress multicast group has replications per core
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group_id - egress multicast group
 *   \param [in] core_id - core_id
 *   \param [out] rep_exist - 1 - rep exist. 0 - rep does not exist
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_exr_core_rep_exist(
    int unit,
    uint32 group_id,
    uint32 core_id,
    uint32 *rep_exist);

/**
 * \brief - enable/disable egress multicast group core replication
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group_id - egress multicast group
 *   \param [in] core_id - core_id
 *   \param [in] enable - enable/disable
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_egr_core_enable_set(
    int unit,
    uint32 group_id,
    int core_id,
    uint32 enable);

/**
 * \brief - given MCDB entry index, the region and the index inside the region is returned.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [out] id - internal region index
 *   \param [out] region - MCDB regions
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_to_id(
    int unit,
    uint32 entry_id,
    uint32 *id,
    dnx_multicast_group_region_t * region);

/**
 * \brief - given group/bitmap id and ingress/core indications, the MCDB entry index is returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] id - group/bitmap id
 *   \param [in] is_bmp - group/bitmap
 *   \param [in] is_ingress - group/bitmap is ingress
 *   \param [in] core_id - relevant for is_ingress==0
 *   \param [out] entry_id - MCDB index
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
*   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_id_to_entry(
    int unit,
    uint32 id,
    uint32 is_bmp,
    uint32 is_ingress,
    uint32 core_id,
    uint32 *entry_id);

/**
 * \brief - write an MCDB entry through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] entry_val - structure filled with the entry values
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_set(
    int unit,
    uint32 entry_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val);

/**
 * \brief - get an MCDB entry through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] entry_val - structure filled with the entry values
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_get(
    int unit,
    uint32 entry_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val);

/**
 * \brief - clears an MCDB entry through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] is_tdm - is the entry a tdm
 *   \param [in] is_bmp - is the entry a bitmap
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_clear(
    int unit,
    uint32 entry_id,
    uint32 is_tdm,
    uint32 is_bmp);

/**
 * \brief - update next entry pointer of an MCDB entry through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] next_entry_id - pointer to the next MCDB entry in the link list
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_next_ptr_set(
    int unit,
    uint32 entry_id,
    uint32 next_entry_id);

/**
 * \brief - update previous entry pointer of an MCDB entry through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] prev_entry_id - pointer to the previous MCDB entry in the link list
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_prev_ptr_set(
    int unit,
    uint32 entry_id,
    uint32 prev_entry_id);

/**
 * \brief - allocates entry and returns the ID. allocation referring flags and core_id will be supported in the future.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] with_id - whether the allocation needs to be done with id
 *   \param [in] entry_id - MCDB index
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_allocate(
    int unit,
    uint32 flags,
    uint32 core_id,
    uint32 with_id,
    uint32 *entry_id);

/**
 * \brief - allocates an entry from the requested region and moves the current MCDB entry to the allocated entry
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] entry_id - MCDB index
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_relocate(
    int unit,
    uint32 flags,
    uint32 core_id,
    uint32 entry_id);

/**
 * \brief - removes MCDB entry through DBAL interface and updates the link list by updating next and prev pointers.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] prev_entry_id - pointer to the previous MCDB entry in the link list
 *   \param [in] next_entry_id - pointer to the next MCDB entry in the link list
 *   \param [in] is_bmp - is the entry a bitmap
 *   \param [out] cur_entry_replaced - indicates whether current entry id is replaced with other entry
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_entry_remove(
    int unit,
    uint32 entry_id,
    uint32 prev_entry_id,
    uint32 next_entry_id,
    uint32 is_bmp,
    uint32 *cur_entry_replaced);

/**
 * \brief - gets the multicast group status (open/closed)
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] is_open - open/close
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_group_open_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 *is_open);

/**
 * \brief - sets the multicast group status (open/closed)
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] is_open - open/close
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_group_open_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_open);

/**
 * \brief - given MCDB entry returns the destination gport & CUD information according to format
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_val - structure filled with the entry values
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [out] dest - gport representing the first destination
 *   \param [out] dest_2 - gport representing the second destination
 *   \param [out] cud - encaps id of the replication
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_convert_hw_to_gport(
    int unit,
    dnx_multicast_mcdb_dbal_field_t entry_val,
    uint32 core_id,
    bcm_gport_t * dest,
    bcm_gport_t * dest_2,
    bcm_if_t * cud);

/**
 * \brief - convert gport to MCDB HW values. According to the given gport, the entry values and core information
 *          are returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [out] dest - gport representing the first destination
 *   \param [out] encap - encaps id of the replication
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - core_id requested
 *   \param [in] entry_val - structure filled with the entry values by this function
 *   \param [out] is_belong_to_core - whether the replication belongs to specified core_id
 *   \param [in] trunk_action - indicating the action to be taken in regards of egress trunk profile
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_convert_gport_to_hw(
    int unit,
    bcm_gport_t dest,
    bcm_if_t encap,
    uint32 flags,
    uint32 core_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val,
    uint32 *is_belong_to_core,
    dnx_multicast_trunk_action_t trunk_action);

/**
 * \brief - returns the number of replications per core
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array
 *   \param [in] nof_reps_per_core - pointer to number of replications
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_nof_egress_reps_per_core_get(
    int unit,
    uint32 core_id,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    uint32 *nof_reps_per_core);

/**
 * \brief - converts replication array to bitmap. for egress a bitmap per core is returned
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array
 *   \param [in] bmp - pointer to an array which hold the bitmap for both cores
 *   \param [in] bmp_cud - an array which holds the cud for each cores
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * bmp is of size DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit)
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_reps_to_bmp(
    int unit,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    uint32 *bmp[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES],
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES]);

/**
 * \brief - given a bitmap and core_id replication array returned.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] cud - encaps id of the bitmap replications
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] bmp - array which hold the bitmap for core_id
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array returned by this function
 *   \param [in] rep_count_out - pointer to number of replications returned by this function
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * bmp is of size DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit)
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_bmp_to_reps(
    int unit,
    uint32 cud,
    uint32 core_id,
    uint32 *bmp,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    int *rep_count_out);

/**
 * \brief - iterates over bitmap and performs one of the actions
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] bitmap_ptr - MCDB index to bitmap
 *   \param [in] cud - encaps id of the bitmap replications
 *   \param [in] action - see remarks
 *   \param [in] bmp - an array which hold the bitmap for core_id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST - removes the entries specified at the rep_array.
 *                                    if entry will not be found _SHR_E_NOT_FOUND error will be returned.
 * DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST - update the HW bitmap with the replications which are no longer exist.
 * DNX_MULTICAST_ENTRY_ITER_BMP_ADD - Adds the bitmap to the existing HW bitmap and clears added replications from the bmp
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL - removes all the entries in the link list
 * DNX_MULTICAST_ENTRY_ITER_GET - copies the content of the link list to the rep_array
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_iter(
    int unit,
    uint32 bitmap_ptr,
    uint32 cud,
    dnx_multicast_entry_iter_action_t action,
    uint32 bmp[]);

/**
 * deletes HW bitmap replications
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - bitmap id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] action - see remarks
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST - removes the entries specified at the rep_array.
 *                                    if entry will not be found _SHR_E_NOT_FOUND error will be returned.
 * DNX_MULTICAST_ENTRY_ITER_BMP_ADD - Adds the bitmap to the existing HW bitmap
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    dnx_multicast_entry_iter_action_t action,
    int nof_replications,
    bcm_multicast_replication_t * rep_array);

/**
 * \brief - gets bitmap replication from HW
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - bitmap id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] max_replications - maximum replications
 *   \param [in] rep_array - pointer to the the replications array returned by this function
 *   \param [in] rep_count - pointer to number of replications returned by this function
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * rep_array,
    int *rep_count);

/**
 * \brief - creates link list of bitmaps with the rep_array and connects it to existing link list of the bitmaps
 *          and remove the old link list bitmap if neccessary.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - bitmap id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] is_set - whether called from bcm_multicast_set/add
 *   \param [in] rep_array - pointer to the the replications array
 *   \param [in] rep_count - specifies the rep_array size

 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *                                      first_bitmap_entry<>new_link_list<>old_link_list
 * error _SHR_E_FULL returned if there isn't enough space to create the new bitmap link list needed.
 * The different between bcm_multicast_set & bcm_multicast_add (indicated by is_set parameter) is that at the (is_set=1)
 * the replications which no longer part of the bitmap need to be removed.
 * After removing unnecessary bitmaps, new replication from user list will be added. If new entries needed for these
 * entries they will be allocated at this function.
 * \see
 *   * None
 */
shr_error_e dnx_multicast_bitmap_create(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_set,
    int rep_count,
    bcm_multicast_replication_t * rep_array);

/**
 * \brief - iterates over group link list or a link list started at entry_id and performs one of the actions
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] entry_id - MCDB index
 *   \param [in] action - see remarks
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *   \param [in] rep_count_out - pointer to number of replications returned by this function for "get" action
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST - removes the entries specified at the rep_array.
 *                                    if an entry will not be found _SHR_E_NOT_FOUND error will be returned
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL - removes all the entries in the link list
 * DNX_MULTICAST_ENTRY_ITER_GET - copies the content of the link list to the rep_array
 * \see
 *   * None
 */
shr_error_e dnx_multicast_linklist_iter(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 *entry_id,
    dnx_multicast_entry_iter_action_t action,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    int *rep_count_out);

/**
 * \brief - creates link list with the rep_array and connects it to existing link list and remove the old link list if neccessary.
 *                                      first_group_entry<>new_link_list<>old_link_list
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] is_set - whether called from bcm_multicast_set/add
 *   \param [in] rep_count - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * error _SHR_E_FULL returned if there isn't enough space to create the new link list.
 * This functions needs to know if previous link list is empty.
 * In a case which called from bcm_multicast_set API (is_set == 1) previous group considered as empty and removed at the end of the sequence.
 * In a case which called from bcm_multicast_add API (is_set == 0), HW link list need to be checked to determine whether the link list is empty.
 * When the link list is empty, first entry of the group is used to store replications.
 * For egress replication, the rep_array divided between cores. for each core different link list created.
 * For the 2 egress destination with the same cud, one replication entry is used.
 * This optimization is taking place only when the replications with the same CUD are consecutive
 * \see
 *   * None
 */
shr_error_e dnx_multicast_linklist_create(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_set,
    int rep_count,
    bcm_multicast_replication_t * rep_array);

/**
 * \brief - set if to count all ingress multicast copies as one or count all copies. 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id  
 *   \param [in] arg - TRUE/FALSE
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_stat_count_copies_set(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    int arg);

/**
 * \brief - get from HW, if to count all ingress multicast copies as one or count all copies. 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id  
 *   \param [out] arg - TRUE/FALSE
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_stat_count_copies_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    int *arg);

/**
 * \brief - set the content (replications) of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_tdm_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array);

/**
 * \brief - Get the content (replications) of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] max_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   \param [in] rep_count - pointer to number of replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_multicast_tdm_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * rep_array,
    int *rep_count);

/**
 * \brief - add the content (replications) to an existing replication of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_tdm_add(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array);

/**
 * \brief - delete the content (replications) of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_multicast_tdm_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array);

/*
 * } 
 */
#endif/*_DNX_MULTICAST_IMP_INCLUDED__*/
