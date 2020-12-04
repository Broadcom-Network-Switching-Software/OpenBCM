/*! \file bcmpkt_pmd.h
 *
 * Common macros and definitions for PMD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_PMD_H
#define BCMPKT_PMD_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_pb.h>

/*! Invalid PMD header field ID. */
#define BCMPKT_FID_INVALID -1

/*! Bitmap array size. */
#define BCMPKT_BITMAP_WORD_SIZE  16

/*!
 * \name BCMPKT Dumping flags.
 * \anchor BCMPKT_DUMP_F_XXX
 */
/*! Dump all fields contents. */
#define BCMPKT_DUMP_F_ALL         0
/*! Dump none zero field content only. */
#define BCMPKT_DUMP_F_NONE_ZERO   1

/*! PMD header field ID bit array. */
typedef struct bcmpkt_bitmap_s {
    /*! Bit array */
    uint32_t pbits[BCMPKT_BITMAP_WORD_SIZE];
} bcmpkt_bitmap_t;

#endif /* BCMPKT_PMD_H */
