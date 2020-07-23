/*! \file bcmgene_db.h
 *
 * Broadcom Generic Extensible NPL Encapsulation (GENE) header file.
 * This file contains GENE definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_GENE_H
#define BCMINT_LTSW_GENE_H

#include <bcm_int/ltsw/chip/bcmgene_db.h>
#include <bcm_int/ltsw/chip/bcmgene_editor.h>
/*!
 * \brief Get the GENE database for a device.
 *
 * \param [in] unit Unit Number.
 *
 * \retval !NULL Success.
 * \retval NULL Failure.
 */
extern bcmgene_db_t *
bcm56780_a0_gene_db_get(int unit);

/*!
 * \brief GENE extractor.
 *
 * \param [in] unit Unit Number.
 * \param [in] gene_name GENE name.
 *
 * \retval !NULL Success.
 * \retval NULL Failure.
 */
extern bcmgene_desc_t *
bcm56780_a0_gene_extractor(
    int unit,
    const char *gene_name);

/*!
 * \brief Add GENE features into device-based feature list.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_feature_init(int unit);

/*!
 * \brief Add GENE sub-features by functionality.
 *
 * \param [in] unit Unit Number.
 * \param [in] gene The GENE descriptor.
 * \param [in] func The GENE functionality.
 * \param [out] sub_f The GENE sub-features.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_sub_feature_get(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t *sub_f);

/*!
 * \brief Targeting the GENE table into GENE handler.
 *
 * \param [in] unit Unit Number.
 * \param [in] gene The GENE descriptor.
 * \param [in] func The GENE functionality.
 * \param [in] sub_f The GENE sub-features.
 * \param [out] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_targeting(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t func,
    uint32_t sub_f,
    bcmgene_handle_t *hndl);

/*!
 * \brief Create the GENE batch handler.
 *
 * \param [in/out] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_batch_handle_create(bcmgene_handle_t *hndl);

/*!
 * \brief Destroy the GENE batch handler.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_batch_handle_destroy(bcmgene_handle_t *hndl);

/*!
 * \brief Create the GENE handler.
 *
 * \param [in/out] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_handle_create(bcmgene_handle_t *hndl);

/*!
 * \brief Destroy the GENE handler.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_handle_destroy(bcmgene_handle_t *hndl);

/*!
 * \brief Validate the GENE tag: blk and offset.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_validate_tag(
    bcmgene_handle_t *hndl,
    int blk,
    int offset);

/*!
 * \brief Validate the GENE field and input value.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [in] value The input value of the GENE field.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_validate_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value);

/*!
 * \brief Validate the GENE field of an array and input value.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [in] index The index in the GENE Array.
 * \param [in] value The input value of the GENE field.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_validate_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    int index,
    uint64_t value);

/*!
 * \brief Fill the flags in the GENE handler.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] flags The GENE handler flags.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_flags(
    bcmgene_handle_t *hndl,
    uint32_t flags);

/*!
 * \brief Fill the key of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] index The key index of the GENE table.
 * \param [in] value The input value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t value);

/*!
 * \brief Fill the field of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [in] value The input value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t value);

/*!
 * \brief Fill the field array of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [in] index The start index of the GENE field.
 * \param [in] value The input value array.
 * \param [in] count The array count.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count);

/*!
 * \brief Fill the callback function of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] cb The callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_callback(
    bcmgene_handle_t *hndl,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data));

/*!
 * \brief Fill the LT handler priority.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] priority The handler priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_priority(
    bcmgene_handle_t *hndl,
    int priority);

/*!
 * \brief Fill the LT handler opcode.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] opcode The handler opcode.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_opcode(
    bcmgene_handle_t *hndl,
    int opcode);

/*!
 * \brief Fill the LT handler into batch handler.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_fill_batch_handle(bcmgene_handle_t *hndl);

/*!
 * \brief Commit the LT entry.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_commit(bcmgene_handle_t *hndl);

/*!
 * \brief The LT entry commit done.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_commit_done(bcmgene_handle_t *hndl);

/*!
 * \brief Commit the batch LT entries.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_commit_batch(bcmgene_handle_t *hndl);

/*!
 * \brief Edit the LT entry.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_edit(bcmgene_handle_t *hndl);

/*!
 * \brief Unset the LT entry.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_unset(bcmgene_handle_t *hndl);

/*!
 * \brief Lookup the LT entry.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_lookup(bcmgene_handle_t *hndl);

/*!
 * \brief Lookup the key of LT entry.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] index The key index of the GENE table.
 * \param [out] value The field value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_lookup_key(
    bcmgene_handle_t *hndl,
    int index,
    uint64_t *value);

/*!
 * \brief Lookup the field value of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [out] value The field value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_lookup_field(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint64_t *value);

/*!
 * \brief Lookup the field array values of the GENE table.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] blk The GENE block.
 * \param [in] offset The GENE offset in block.
 * \param [in] index The start field index of the GENE table.
 * \param [out] value The field value.
 * \param [in] count The array count.
 * \param [out] depth The array depth.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_lookup_field_array(
    bcmgene_handle_t *hndl,
    int blk,
    int offset,
    uint32_t index,
    uint64_t *value,
    uint32_t count,
    uint32_t *depth);

/*!
 * \brief The LT entry lookup done.
 *
 * \param [in] hndl The GENE handler.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_lookup_done(bcmgene_handle_t *hndl);

/*!
 * \brief The LT entry lookup done.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] user_data The input of user data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_traverse(
    bcmgene_handle_t *hndl,
    void *user_data);

/*!
 * \brief Clear the GENE tables.
 *
 * \param [in] unit The unit number.
 * \param [in] gene The GENE descriptor.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_clear(
    int unit,
    bcmgene_desc_t *gene);

/*!
 * \brief Free handlers and return errors if rv is failed.
 *
 * \param [in] hndl The GENE handler.
 * \param [in] rv The return value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_gene_error_return(
    bcmgene_handle_t *hndl,
    int rv);

/*!
 * \brief The GENE function traverse interface.
 *
 * \param [in] gene The GENE descriptor.
 * \param [in] func The GENE functionality.
 * \param [in] hndl The GENE handler.
 * \param [in] user_data The input of user data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56780_a0_gene_func_traverse(
    int unit,
    bcmgene_desc_t *gene,
    uint32_t function,
    int (*cb)(bcmgene_handle_t *hndl, void *user_data),
    void *user_data);

#endif /* BCMINT_LTSW_GENE_DB_H */
