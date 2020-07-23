/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pkt.h
 * Purpose:     Common Internal pkt utilities
 */

#ifndef _BCM_PKT_H
#define _BCM_PKT_H

#include <shared/bsl.h>

#include <soc/defs.h>	
#include <bcm/debug.h>

#include <bcm/pkt.h>


/* Dump a DNX packet. */
extern void bcm_pkt_dnx_dump(
    bcm_pkt_t *pkt);
/* Dump itmh destination */
extern void bcm_pkt_dnx_itmh_dest_dump(
    bcm_pkt_dnx_itmh_dest_t *itmh_dest);

/* Dump itmh header. */
extern void bcm_pkt_dnx_itmh_dump(
    bcm_pkt_dnx_itmh_t *ext);

/* Dump ftmh lb extension. */
extern void bcm_pkt_dnx_ftmh_lb_extension_dump(
    bcm_pkt_dnx_ftmh_lb_extension_t *ext);

/* Dump ftmh dest extension. */
extern void bcm_pkt_dnx_ftmh_dest_extension_dump(
    bcm_pkt_dnx_ftmh_dest_extension_t *ext);

/* Dump ftmh stack extension. */
extern void bcm_pkt_dnx_ftmh_stack_extension_dump(
    bcm_pkt_dnx_ftmh_stack_extension_t *ext);

/* Dump ftmh header. */
extern void bcm_bcm_pkt_dnx_ftmh_dump(
    bcm_pkt_dnx_ftmh_t *ext);
#endif /* _BCM_PKT_H */
