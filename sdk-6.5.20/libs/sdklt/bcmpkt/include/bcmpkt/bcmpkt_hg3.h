/*! \file bcmpkt_hg3.h
 *
 * Common macros and definitions for Higig3 protocol
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_HG3_H
#define BCMPKT_HG3_H

#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t_defs.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_extension_0_t_defs.h>

/* Note, ether type set to same value as reset value of R_GSH_ETHERTYPEr(700) */
/*! Ethernet type used for Higig3 header */
#define BCMPKT_HG3_ETHER_TYPE                     0x2BC

/*! Higig3 base header size (bytes). */
#define BCMPKT_HG3_BASE_HEADER_SIZE_BYTES         8
/*! Higig3 base header size (words). */
#define BCMPKT_HG3_BASE_HEADER_SIZE_WORDS         2

/*! Higig3 extension 0 header size (bytes). */
#define BCMPKT_HG3_EXT0_HEADER_SIZE_BYTES         8
/*! Higig3 extension 0 header size (words). */
#define BCMPKT_HG3_EXT0_HEADER_SIZE_WORDS         2

/*! Higig3 header size (bytes). Includes base and ext0 header */
#define BCMPKT_HG3_SIZE_BYTES       (BCMPKT_HG3_BASE_HEADER_SIZE_BYTES + \
                                     BCMPKT_HG3_EXT0_HEADER_SIZE_BYTES)
/*! Higig3 header size (words). Includes base and ext0 header */
#define BCMPKT_HG3_SIZE_WORDS       (BCMPKT_HG3_BASE_HEADER_SIZE_WORDS + \
                                     BCMPKT_HG3_EXT0_HEADER_SIZE_WORDS)

/*! Offset to be added to ext0 fids to differentiate them from base fids */
#define BCMPKT_HG3_EXT0_FID_OFFSET BCMPKT_HG3_BASE_T_FID_COUNT
#endif /* BCMPKT_HG3_H */
