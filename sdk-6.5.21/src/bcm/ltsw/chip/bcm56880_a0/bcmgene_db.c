/*! \file bcmgene_db.c
 *
 * Broadcom Generic Extensible NPL Encapsulation.
 * This file contains GENE database of devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/chip/bcmgene_db.h>

#include <shr/shr_debug.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

/******************************************************************************
 * Local definitions
 */

/* Declare conf externs. */
#define VARSYM(va) bcm56880_a0_##va##_gene_db_get
#define BCM56880_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0)\
    extern bcmgene_db_t *VARSYM(_va)(void);
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_list.h>

/* Define conf list. */
#define BCM56880_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0)\
    { &VARSYM(_va), },
static struct {
    bcmgene_db_t *(*gene_db_get)(void);
} bcmgene_db_default_dev_func[] = {
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_list.h>
    { 0 } /* end-of-list */
};

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get the GENE database for a device.
 *
 * \param [in] unit Unit Number.
 *
 * \retval !NULL Success.
 * \retval NULL Failure.
 */
static bcmgene_db_t *
bcmgene_db_get(int unit)
{
    int rv, variant;

    rv = bcm56880_a0_variant_get(unit, &variant);
    if (SHR_FAILURE(rv)) {
        return NULL;
    }

    return bcmgene_db_default_dev_func[variant].gene_db_get();
}


/******************************************************************************
 * Public functions
 */
bcmgene_db_t *
bcm56880_a0_gene_db_get(int unit)
{
    return bcmgene_db_get(unit);
}

