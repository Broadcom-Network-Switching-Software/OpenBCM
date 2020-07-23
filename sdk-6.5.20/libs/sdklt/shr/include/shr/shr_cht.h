/*! \file shr_cht.h
 *
 * Chained hash table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_CHT_H
#define SHR_CHT_H

#include <shr/shr_types.h>

/*!
 * \brief Calculate hash value for a given table entry.
 *
 * This function should calculate the hash value based on the hash
 * table size and the key fields of the specified table entry.
 *
 * A function of this type must be provided when the hash table is
 * created.
 *
 * \param [in] size Hash table size.
 * \param [in] entry Entry for which to calculate the hash value.
 *
 * \return Hash value.
 */
typedef uint32_t (*shr_cht_hash_f)(uint32_t size, const void *entry);

/*!
 * \brief Match key values of two tables entries.
 *
 * A function of this type must be provided when the hash table is
 * created.
 *
 * \param [in] e1 First table entry.
 * \param [in] e2 Second table entry.
 *
 * \retval true Key values are identical.
 * \retval false Key values differ.
 */
typedef bool (*shr_cht_match_f)(const void *entry1, const void *entry2);

/*!
 * The hash table requires a hash table-sized array of each member of
 * this structure. We keep these together to allow the hash table to
 * be allocated as a single block.
 */
typedef struct shr_cht_tbl_s {

    /*! This array contains the head of each bucket list. */
    uint32_t head;

    /*! This array contains the bucket lists beyond the head. */
    uint32_t list;

    /*! This array contains pointers to the table entries. */
    void *entry;

} shr_cht_tbl_t;

/*!
 * Main hash table control structure.
 */
typedef struct shr_cht_s {

    /*! Size of the hash table. */
    uint32_t size;

    /*! Index where the next new entry should be inserted. */
    uint32_t free_idx;

    /*! Caller-provided function for calculating hash value. */
    shr_cht_hash_f hash;

    /*! Caller-provided function for comparing keys of two entries. */
    shr_cht_match_f match;

    /*! Internal array for bucket list management. */
    shr_cht_tbl_t tbl[];

} shr_cht_t;

/*!
 * \brief Calculate hash value for a given table entry.
 *
 * This function calculates the hash value based on the hash
 * table size and the key fields of the specified table entry.
 *
 * Primarily for internal use.
 *
 * \param [in] cht Hash table handle.
 * \param [in] entry Entry for which to calculate the hash value.
 *
 * \return Hash value.
 */
uint32_t
shr_cht_hash(shr_cht_t *cht, void *entry);

/*!
 * \brief Look up entry in hash table.
 *
 * Search hash table based on provided key entry.
 *
 * \param [in] cht Hash table handle.
 * \param [in] entry Key entry.
 *
 * \return Pointer to matching entry or NULL of not found.
 */
extern void *
shr_cht_lookup(shr_cht_t *cht, void *entry);

/*!
 * \brief Insert entry into hash table.
 *
 * This function does not check if a matching entry does already
 * exist. Use \ref shr_cht_lookup to check for existing entry.
 *
 * \param [in] cht Hash table handle.
 * \param [in] entry Entry to insert.
 *
 * \return Pointer to inserted entry or NULL if table is full.
 */
extern void *
shr_cht_insert(shr_cht_t *cht, void *entry);

/*!
 * \brief Destroy hash table object.
 *
 * Frees all resources allocated by \ref shr_cht_create.
 *
 * \param [in] cht Hash table handle.
 */
extern void
shr_cht_destroy(shr_cht_t *cht);

/*!
 * \brief Create hash table object.
 *
 * The caller is assumed to have an array of table entries already.
 *
 * The hash table object provides hash-based lookup into this existing
 * table in order to avoid a sequential search.
 *
 * The returned hash table handle must be used for subsequent insert
 * and lookup operations.
 *
 * Entries cannot be removed from the hash table, but existing entries
 * can modified as long as the key remains unchanged.
 *
 * Use \ref shr_cht_destroy to destroy the hash tabel object.
 *
 * \param [in] size Number of table entries to support.
 * \param [in] hash Function to calculate hash value for a table entry.
 * \param [in] match Function to compare the keys of two table entries.
 *
 * \return Hash table handle or NULL if error.
 */
extern shr_cht_t *
shr_cht_create(uint32_t size, shr_cht_hash_f hash, shr_cht_match_f match);

#endif /* SHR_CHT_H */
