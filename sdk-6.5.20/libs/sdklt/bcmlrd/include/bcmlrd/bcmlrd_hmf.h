/*! \file bcmlrd_hmf.h
 *
 * Hierarchical Map Format (HMF)
 *
 * HMF data structures are used to describe a logical to physical
 * mapping hierarchy of a single logical table to one or more
 * physical memories.
 *
 * These data structures are suitable for use with c-struct-gen YAML
 * descriptions. Note that any array struct fields must have a
 * matching _count suffix field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_HMF_H
#define BCMLRD_HMF_H

#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief HMF mapping field descriptor.
 *
 * The mapping field descriptor associates a field name
 * with a range of bits of the field. The field may be
 * logical or physical.
 */
typedef struct bcmlrd_hmf_field_desc_s {
    /*! Field ID. */
    uint32_t field_id;

    /*! Field maximum bit. */
    uint32_t maxbit;

    /*! Field minimum bit. */
    uint32_t minbit;

} bcmlrd_hmf_field_desc_t;


/*!
 * \brief HMF fixed mapping.
 *
 * A fixed mapping describes a mapping of a fixed value
 * to a physical field.
 */

typedef struct bcmlrd_hmf_fixed_mapping_s {
    /*! Physical field mapping. */
    bcmlrd_hmf_field_desc_t physical;

    /*! Physical value. */
    uint32_t value;

} bcmlrd_hmf_fixed_mapping_t;

/*!
 * \brief HMF field mapping.
 *
 * A field mapping describes a mapping of a logical field
 * to a physical field.
 */

typedef struct bcmlrd_hmf_field_mapping_s {
    /*! Logical field mapping. */
    bcmlrd_hmf_field_desc_t logical;

    /*! Physical field mapping. */
    bcmlrd_hmf_field_desc_t physical;

} bcmlrd_hmf_field_mapping_t;

/*!
 * \brief Memory indexing offset
 *
 * This flag indicates that the memory index needs to be offset by the
 * value in bcmlrd_hmf_memory_t.offset.
 */

/*!
 * \brief Memory indexing offset
 *
 * This flag indicates that the memory index needs to be offset by the
 * value in bcmlrd_hmf_memory_t.offset.
 */

#define BCMLRD_HMF_MEMORY_F_OFFSET 0x00000001

/*!
 * \brief Memory lookup 0 flag
 *
 * This flag indicates that this memory is associated with lookup0.
 */

#define BCMLRD_HMF_MEMORY_F_LOOKUP0 0x00000002

/*!
 * \brief Memory lookup 1 flag
 *
 * This flag indicates that this memory is associated with lookup1.
 */

#define BCMLRD_HMF_MEMORY_F_LOOKUP1 0x00000004

/*!
 * \brief Shared index flag
 *
 * This flag indicates that this memory is associated with a shared index.
 */

#define BCMLRD_HMF_MEMORY_F_SHARED_INDEX 0x00000008

/*!
 * \brief Memory unallocated index  flag
 *
 * This flag indicates that this memory is associated with an unallocated index.
 */

#define BCMLRD_HMF_MEMORY_F_UNALLOCATED 0x00000010

/*!
 * \brief Memory fixed index flag
 *
 * This flag indicates that this memory is associated with a fixed index.
 */

#define BCMLRD_HMF_MEMORY_F_FIXED_INDEX 0x00000020

/*!
 * \brief HMF tile mode parameters.
 *
 * Tile mode specification.
 */

typedef struct bcmlrd_hmf_tile_mode_s {
    /*! Tile mapped to SP PT. */
    uint32_t tile;

    /*! Tile mode for this SP PT. */
    uint32_t tile_mode;

} bcmlrd_hmf_tile_mode_t;

/*!
 * \brief HMF mapping memory metadata.
 *
 * Ancillary memory information.
 */

typedef struct bcmlrd_hmf_memory_metadata_s {
    /*! Memory metadata flags. */
    uint32_t flags;

    /*! Offset if BCMLRD_HMF_MEMORY_OFFSET is set. */
    uint32_t offset;

    /*! Shared index if BCMLRD_HMF_MEMORY_SHARED_INDEX is set. */
    uint32_t shared_index;

    /*! Fixed index if BCMLRD_HMF_MEMORY_SHARED_INDEX is set. */
    uint32_t fixed_index;

    /*! Number of tile mode mappings. */
    uint32_t tile_mode_list_count;

    /*! List of tile mode mappings. */
    const bcmlrd_hmf_tile_mode_t *tile_mode_list;

} bcmlrd_hmf_memory_metadata_t;

/*!
 * \brief HMF mapping memory structure.
 *
 * A memory mapping consists of zero or more key, value,
 * or constant mappings.
 */

typedef struct bcmlrd_hmf_memory_s {
    /*! Memory being mapped.. */
    uint32_t ptsid;

    /*! Memory mapping flags. */
    bcmlrd_hmf_memory_metadata_t metadata;

    /*! Number of key mappings. */
    uint32_t key_count;

    /*! Array of key mappings. */
    const bcmlrd_hmf_field_mapping_t *key;

    /*! Number of value mappings. */
    uint32_t value_count;

    /*! Array of  value mappings. */
    const bcmlrd_hmf_field_mapping_t *value;

    /*! Number of fixed mappings. */
    uint32_t fixed_count;

    /*! Array of fixed mappings. */
    const bcmlrd_hmf_fixed_mapping_t *fixed;

} bcmlrd_hmf_memory_t;

/*!
 * \brief Memory fixed index flag
 *
 * This flag indicates that this instance has an allowed entries
 * limit.
 */

#define BCMLRD_HMF_INSTANCE_F_ALLOWED_ENTRIES 0x00000001

/*!
 * \brief Memory fixed index flag
 *
 * This flag indicates that this instance has an entry modulus and
 * residue.
 */

#define BCMLRD_HMF_INSTANCE_F_ALLOWED_MODULUS 0x00000002

/*!
 * \brief HMF mapping instance metadata.
 *
 * Ancillary instance information.
 *
 * If "Allowed entries" is indicated, then the number of entries allocated in
 * this instance is limited to the number specified.
 *
 * If "Allowed modulus" is indicated, then a subset of indicies is
 * used in this instance when the index is constrained by
 *
 *    (index % allowed_modulus) == allowed_residue
 *
 * For example, if allowed_modulus == 2 and allowed_residue == 0, then
 * the index is constrained to every even entry. If allowed_residue ==
 * 1, then the index is constrained to every odd entry.
 */

typedef struct bcmlrd_hmf_instance_metadata_s {
    /*! Instance metadata flags. */
    uint32_t flags;

    /*! Allowed entries for this instance. */
    uint32_t allowed_entries;

    /*! Allowed indicies modulus. */
    uint32_t allowed_modulus;

    /*! Allowed indicies residue. */
    uint32_t allowed_residue;

} bcmlrd_hmf_instance_metadata_t;

/*!
 * \brief HMF mapping instance structure.
 *
 * An instance consists of one or more memories, where
 * each memory contributes to the width of an entry.
 */

typedef struct bcmlrd_hmf_instance_s {
    /*! Instance metadata. */
    bcmlrd_hmf_instance_metadata_t metadata;

    /*! Number of memory descriptors. */
    uint32_t memory_count;

    /*! Array of memory descriptor pointers. */
    const bcmlrd_hmf_memory_t **memory;

} bcmlrd_hmf_instance_t;

/*!
 * \brief HMF mapping view structure.
 *
 * View mapping. A mapping view consists
 * of one or more mapping instance. Each instance
 * contributes to the depth on a mapping.
 */

typedef struct bcmlrd_hmf_view_s {
    /*! View field ID.  */
    uint32_t field_id;

    /*! View selector value. */
    uint32_t view;

    /*! Number of mapping instances. */
    uint32_t instance_count;

    /*! Array of mapping instance pointers. */
    const bcmlrd_hmf_instance_t **instance;

} bcmlrd_hmf_view_t;

/*!
 * \brief Memory fixed index flag
 *
 * This flag indicates that this mapping contains at least one
 * unallocated entry.
 */

#define BCMLRD_HMF_F_DYNAMIC 0x00000001

/*!
 * \brief HMF map data structure.
 *
 * Top level compiler mapping for a logical table. A compiler
 * mapping consists of one or more mapping views.
 */

typedef struct bcmlrd_hmf_s {
    /*! Mapping flags. */
    uint32_t flags;

    /*! Logical table SID. */
    uint32_t sid;

    /*! Associated resource info logical table SID. */
    uint32_t resource_sid;

    /*! Number of views. */
    uint32_t view_count;

    /*! Array of mapping views. */
    const bcmlrd_hmf_view_t *view;

} bcmlrd_hmf_t;

#endif /* BCMLRD_HMF_H */
