/*! \file bcmltm_md_lta.h
 *
 * Logical Table Manager - LTA Metadata Definitions.
 *
 * This file contains generic LTA definitions and interfaces used
 * for metadata construction, such as field transforms, field validations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LTA_H
#define BCMLTM_MD_LTA_H

#include <shr/shr_types.h>

/*!
 * LTA Field Options.
 *
 * The following defines specify special handling of the LTA
 * field list.
 *
 * Options are applicable only under certain context.
 */

/*!
 * Key fields are allocatable.
 *
 * This option allows allocatable keys to be absent from the
 * API input list.  Normally, all key fields are required.
 *
 * The API input fields are copied to LTA input fields and note
 * if fields are missing.
 *
 * Used for allocatable key fields on IwA LTs on INSERT.
 */
#define BCMLTM_MD_LTA_FOPT_IN_API_ALLOC     0x0001

/*!
 * Do not copy fields from API.
 *
 * This is used to skip the api_to_lta step of the transform subtree.
 * In this case, the LTA input fields are usually already copied from a
 * reverse transform output in the lta_to_lta step.
 *
 * Example usage:
 * LTs with track index map (field transform) on TRAVERSE of next TI.
 */
#define BCMLTM_MD_LTA_FOPT_IN_NO_API        0x0002

/*!
 * Perform a LTA to LTA copy.
 *
 * This is used in the lta_to_lta step of the transform subtree.
 *
 * On a forward transform subtree, this indicates to copy
 * the output of the reverse transform to the LTA input field list
 * of the forward transform.
 *
 * Example usage:
 * - LTs with track index map (field transform) on TRAVERSE of next TI.
 */
#define BCMLTM_MD_LTA_FOPT_IN_LTA           0x0004

/*!
 * \brief LTA Field Options.
 *
 * This type indicates special handling of the LTA field list:
 *     BCMLTM_MD_LTA_FOPT_*
 *
 * This information is used by the LTA subtree creation interfaces
 * to construct and install the appropriate function nodes in the
 * LTA tree clusters.
 */
typedef uint32_t bcmltm_md_lta_fopt_t;


/*!
 * \brief LTA Flags.
 *
 * These flags are used to indicate special handling within an LTA subtree:
 *     BCMLTM_MD_LTA_FLAG_*
 *
 * This information is used by the LTA subtree creation interfaces
 * to construct and install the appropriate function nodes in the
 * LTA subtree clusters.
 */
typedef uint32_t bcmltm_md_lta_flags_t;

/*
 * \brief Default flags.
 */
#define BCMLTM_MD_LTA_FLAG_DEFAULTS               0

/*
 * \brief Traverse start flag.
 *
 * Indicates to install function that checks for the start of
 * a TRAVERSE.  If this is a traverse start, transform call is skipped
 * since no keys are supplied.
 *
 * Usage: First round for forward key transform on TRAVERSE.
 */
#define BCMLTM_MD_LTA_FLAG_TRAVERSE_START         0x0001

/*
 * \brief Allocate second flag.
 *
 * Indicates to install function that checks if allocation occurred.
 * If allocation did not take place, transform call is skipped.
 * This is used after the first forward alloc key transform pass.
 *
 * Usage:
 * - Second round for forward allocatable key transform on INSERT (IwA).
 * - Reverse allocatable key transform on INSERT (IwA).
 */
#define BCMLTM_MD_LTA_FLAG_ALLOC_SECOND           0x0002

/*
 * \brief Output for allocatable fields flag.
 *
 * Indicates to copy output fields only if key allocation occurred.
 *
 * Usage: Reverse allocatable key transform on INSERT (IwA).
 */
#define BCMLTM_MD_LTA_FLAG_ALLOC_OUT              0x0004

#endif /* BCMLTM_MD_LTA_H */
