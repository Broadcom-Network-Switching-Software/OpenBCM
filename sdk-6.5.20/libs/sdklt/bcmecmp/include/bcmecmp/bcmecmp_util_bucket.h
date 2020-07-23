/*! \file bcmecmp_util_bucket.h
 *
 * Bucket sort utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_UTIL_BUCKET_H
#define BCMECMP_UTIL_BUCKET_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>

/*! Bucket dump function. */
typedef void (*bcmecmp_bucket_dump_f)(int bucket_idx, uint32_t key, uint32_t data);

/*! Bucket handle. */
typedef void * bcmecmp_bucket_handle_t;

/*! Invalid handle. */
#define BCMECMP_BUCKET_INVALID_HANDLE 0


/*!
 * \brief Create a bucket.
 *
 * \param [in] bucket_size Size of bucket.
 * \param [in] max_key Max key of bucket.
 *
 * \return Pointer to bucket structure, or BCMECMP_BUCKET_INVALID_HANDLE if
 * failed.
 */
bcmecmp_bucket_handle_t
bcmecmp_bucket_create(int bucket_size, uint32_t max_key);

/*!
 * \brief Destroy a bucket.
 *
 * \param [in] bucket_hdl Handle of bucket.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
int
bcmecmp_bucket_destroy(bcmecmp_bucket_handle_t bucket_hdl);

/*!
 * \brief Insert a element into bucket.
 *
 * \param [in] bucket_hdl Handle of bucket.
 * \param [in] key Key of element.
 * \param [in] data Data of element.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
int
bcmecmp_bucket_insert(bcmecmp_bucket_handle_t bucket_hdl, uint32_t key,
                      uint32_t data);

/*!
 * \brief Remove max element from bucket.
 *
 * \param [in] bucket_hdl Handle of bucket.
 * \param [out] key Key of max element.
 * \param [out] data Data of max element.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_EMPTY Bucket is empty.
 */
int
bcmecmp_bucket_remove_max(bcmecmp_bucket_handle_t bucket_hdl, uint32_t *key,
                          uint32_t *data);

/*!
 * \brief Dump bucket.
 *
 * \param [in] bucket_hdl Handle of bucket.
 * \param [in] dump_func Pointer to dump function.
 */
void
bcmemcp_bucket_dump(bcmecmp_bucket_handle_t bucket_hdl,
                    bcmecmp_bucket_dump_f dump_func);

#endif /* BCMECMP_UTIL_BUCKET_H */
