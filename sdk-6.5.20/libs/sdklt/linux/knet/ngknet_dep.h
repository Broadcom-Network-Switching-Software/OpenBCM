/*! \file ngknet_dep.h
 *
 * Macro definitions for NGKNET dependence.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef NGKNET_DEP_H
#define NGKNET_DEP_H

#include <shr/shr_error.h>
#include <ngknet_linux.h>

#ifndef TRUE
#define TRUE                1
#endif
#ifndef FALSE
#define FALSE               0
#endif

/*! Memorry barrier */
#define MEMORY_BARRIER      smp_mb()

/*! CNET print uitility */
#define CNET_PR(fmt, args...)   printk(fmt, ##args)

struct pdma_dev;

/*! Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_cnet_pdma_attach(struct pdma_dev *dev); \
extern int _bd##_cnet_pdma_detach(struct pdma_dev *dev);
#include <bcmdrd/bcmdrd_devlist.h>

/*! Create enumeration values from list of supported devices. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    NGKNET_DEV_T_##_bd,
/*! Enumeration for all base device types. */
typedef enum {
    NGKNET_DEV_T_NONE = 0,
#include <bcmdrd/bcmdrd_devlist.h>
    NGKNET_DEV_T_COUNT
} ngknet_dev_type_t;

#endif /* NGKNET_DEP_H */

