/*! \file field_compression_internal.h
 *
 * Defines, Structures, and Functions used to manage AACL compression in field module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _BCM_FIELD_INTERNAL_COMPRESSION_H
#define _BCM_FIELD_INTERNAL_COMPRESSION_H

#include <bcm_int/esw/field_compression/field_trie_mgmt.h>
#include <bcm_int/esw/field.h>

/*!
 * List of compression cid and FP entries using it.
 */
typedef struct _bcm_field_compress_cid_list_s {

    /* Array of entries using the cid of the key. */
    bcm_field_entry_t *entries;

    uint32 num_entries;

    /* Compression Key basic info */
    _bcm_field_cid_t cid;

    /*! Next compression key */
    struct _bcm_field_compress_cid_list_s *next;

} _bcm_field_compress_cid_list_t;

typedef struct _bcm_field_trie_info_s {
    /* List of FP qualifiers to be compressed for this trie type */
    bcm_field_qualify_t *qual;

    /* Width of FP qualifiers to be compressed for this trie type (in bits) */
    int *width;

    /* Start bit of the FP qualifier in compression key */
    int *start_bit;

    /* Number of FP qualifiers to be compressed for this trie type */
    int num_qual;

    /* Compression key width of this trie type (in bits) */
    int key_width;

    /* Compression id width of this trie type (in bits) */
    int cid_width;

    /* Internal qualifier used for this trie type */
    _bcm_field_internal_qualify_t iqual;

} _bcm_field_trie_info_t;
#endif /* _BCM_FIELD_INTERNAL_COMPRESSION_H */
