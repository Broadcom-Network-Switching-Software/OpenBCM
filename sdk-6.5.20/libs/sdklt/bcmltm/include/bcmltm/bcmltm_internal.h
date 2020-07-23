/*! \file bcmltm_internal.h
 *
 * Logical Table Manager internal functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Logical Table Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_INTERNAL_H
#define BCMLTM_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

/*! Convert a number of bits to a number of (8-bit) bytes. */
#define BCMLTM_BITS2BYTES(x)    (((x) + 7) / 8)

/*! Convert a number of (8-bit) bytes to a number of bits. */
#define BCMLTM_BYTES2BITS(x)    ((x) * 8)

/*! Convert a number of bits to a number of 32-bit words. */
#define BCMLTM_BITS2WORDS(x)    (((x) + 31) / 32)

/*! Convert a number of 32-bit words to a number of bits. */
#define BCMLTM_WORDS2BITS(x)    ((x) * 32)

/*! Convert a number of (8-bit) bytes to a number of 32-bit words. */
#define BCMLTM_BYTES2WORDS(x)   (((x) + 3) / 4)

/*! Convert a number of 32-bit words to a number of (8-bit) bytes. */
#define BCMLTM_WORDS2BYTES(x)   ((x) * 4)

/*! Round up (8-bit) bytes to the next 32-bit word. */
#define BCMLTM_WORDSIZEBYTES(x)   ((((x) + 3) / 4) * 4)


/*!
 * \brief Logical table classes.
 *
 * The general format of the LT to PT mapping of a Logical Table.
 */
typedef enum bcmltm_lt_class_e {
    /*! Direct PassThru index to HW. */
    BCMLTM_CLASS_PT_PASSTHRU,

    /*! The index is a combination of key fields. */
    BCMLTM_CLASS_SIMPLE_INDEX,

    /*! The index may be requested via resource allocation. */
    BCMLTM_CLASS_RESOURCE_INDEX,

    /*! The key fields are supplied to hash functions. */
    BCMLTM_CLASS_HASH,

    /*! The key and mask fields plus priority indicate entry ordering. */
    BCMLTM_CLASS_SIMPLE_TCAM,

    /*!
     * The key and mask fields plus priority are combined with
     * some more involved algorithm to determine entry ordering.
     */
    BCMLTM_CLASS_COMPLEX_TCAM,

    /*!
     * A legacy runtime capability is exposed which requires modifying
     * FlexPP configuration tables.
     */
    BCMLTM_CLASS_CCH,

    /*! A FlexPP configuration table. (Perhaps unnecessary with LRD?). */
    BCMLTM_CLASS_FLEX,

    /*!
     * A high level logical table which is composed of lower level
     * tables and procedures.
     */
    BCMLTM_CLASS_HIGH_LEVEL,

    /*!
     * A special-case logical table preserving certain complex behaviors,
     * e.g., FP or LPM.
     */
    BCMLTM_CLASS_SPECIAL,

    /*! Number of LTM classes. */
    BCMLTM_CLASS_NUM
} bcmltm_lt_class_t;

/*!
 * \brief Default maximum supported LTs referenced in a single transaction.
 */
#define BCMLTM_TRANSACTION_LT_MAX       8

#endif /* BCMLTM_INTERNAL_H */
