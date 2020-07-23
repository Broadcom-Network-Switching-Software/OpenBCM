/*! \file shr_fmm.h
 *
 * Field memory manager API
 *
 * Interface to the field memory manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_FMM_H
#define SHR_FMM_H

#include <sal/sal_types.h>

/*!
 * \brief Field delete flag.
 *
 * Set the \c flags field of the \ref shr_fmm_t type variable with this
 * flag for deletion of the field.
 */
#define SHR_FMM_FIELD_DEL  0x1

/*!
 * \brief LT field list.
 *
 * This data structure defines a field in a LT entry.
 */
typedef struct shr_fmm_s {
    /*! Field ID. */
    uint32_t id;

    /*! Field special flags - currently only for delete */
    uint32_t flags;

    /*! The field data. */
    uint64_t data;

    /*! Pointer to the next element in the list (or NULL). */
    struct shr_fmm_s *next;

    /*!
     * The index of the field in an array.
     * For non array idx=0.
     * For array index starts for 0
     */
    uint32_t idx;
} shr_fmm_t;

/*!
 * \brief Initialize the share field component
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_fmm_init(void);

/*!
 * \brief Deletes the share field component
 *
 * This function frees all the resources that were occupied by the shared
 * field component. Once being called, the shared field component will not
 * be able to allocate more fields. Furthermore, all pre-allocated fields
 * will be freed back to the OS.
 *
 * \return none.
 */
extern void shr_fmm_delete(void);

/*!
 * \brief Allocate a field.
 *
 * \return pointer to newly allocated field on success or NULL on failure.
 */
extern shr_fmm_t *shr_fmm_alloc(void);

/*!
 * \brief Free previously allocated field.
 *
 * \param [in] field points to field structure to free
 *
 * \return none.
 */
extern void shr_fmm_free(shr_fmm_t *field);

/*!
 * \brief Declaration of field array local memory handle
 */
typedef struct shr_famm_hdl_s *shr_famm_hdl_t;

/*!
 * \brief Initialize field array memory manager resource.
 *
 * Allocate resources.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int shr_famm_init(void);

/*!
 * \brief Clean up field array memory manager resource.
 *
 * Free resources.
 *
 * \return none.
 */
void shr_famm_cleanup(void);

/*!
 * \brief Initialize local memory instance for field array.
 *
 * \param [in] num_of_fld indicates the array size.
 * \param [out] hdl handler of field array
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int shr_famm_hdl_init(uint32_t num_of_fld, shr_famm_hdl_t *hdl);

/*!
 * \brief Deletes local memory instance for field array.
 *
 * This function frees all the resources that were occupied by the field
 * array instance. Once being called, the field array instance will not
 * be able to allocate more fields. Furthermore, all pre-allocated fields
 * will be freed back to the OS.
 *
 * \param [in] hdl handler of field array
 *
 * \return none.
 */
void shr_famm_hdl_delete(shr_famm_hdl_t hdl);

/*!
 * \brief Allocate array of fields.
 *
 * \param [in] hdl handler of field array.
 * \param [in] num_of_fld indicates the number of fields.
 *
 * \return Array of pointer to newly allocated fields on success or NULL
 * on failure.
 */
shr_fmm_t** shr_famm_alloc(shr_famm_hdl_t hdl, uint32_t num_of_fld);

/*!
 * \brief Free previously allocated array of fields.
 *
 * \param [in] hdl handler of field array.
 * \param [in] field_arr points to array of fields to free.
 * \param [in] num_of_fld indicates the number of fields.
 *
 * \return none.
 */
void shr_famm_free(shr_famm_hdl_t hdl,
                   shr_fmm_t ** field_arr,
                   uint32_t num_of_fld);

#endif /* SHR_FMM_H */
