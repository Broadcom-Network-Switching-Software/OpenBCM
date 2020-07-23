/*! \file bcmcnet_dep.h
 *
 * BCMCNET dependencies.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCNET_DEP_H
#define BCMCNET_DEP_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <sal/sal_time.h>
#include <shr/shr_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#endif
#include <bcmdrd/bcmdrd_dev.h>
#ifndef PKTIO_IMPL
#include <bcmdrd/bcmdrd_feature.h>
#endif

/*! Memorry barrier */
#define MEMORY_BARRIER      SAL_CONFIG_MEMORY_BARRIER

/*! CNET print uitility */
#define CNET_PR(...)        cli_out(__VA_ARGS__)

/*! Ethernet header length */
#define ETH_HLEN            14

/*! VLAN tag length */
#define VLAN_HLEN           4

/*! FCS length */
#define ETH_FCS_LEN         4

/*! Interrupt handler */
typedef void (*bcmcnet_intr_f)(int unit, uint32_t data);
struct pdma_dev;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_cnet_pdma_attach(struct pdma_dev *dev); \
extern int _bd##_cnet_pdma_detach(struct pdma_dev *dev); \
extern int _bd##_cnet_pdma_intr_enable(int unit, int cmc, int inum); \
extern int _bd##_cnet_pdma_intr_disable(int unit, int cmc, int inum); \
extern int _bd##_cnet_pdma_intr_connect(int unit, int cmc, int inum, \
                                        bcmcnet_intr_f ifunc, uint32_t idata);
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

#endif /* BCMCNET_DEP_H */

