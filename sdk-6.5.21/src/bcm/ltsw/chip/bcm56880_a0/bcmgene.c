/*! \file bcmgene.c
 *
 * Broadcom Generic Extensible NPL Encapsulation.
 * This file contains GENE operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/bcmgene.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */


/******************************************************************************
 * Private functions
 */
/*!
 * \brief GENE extractor.
 *
 * \param [in] unit Unit Number.
 * \param [in] gene_name GENE name.
 *
 * \retval !NULL Success.
 * \retval NULL Failure.
 */
static bcmgene_desc_t *
bcmgene_extractor(int unit, const char *gene_name)
{
    bcmgene_db_t *gene_db = NULL;
    bcmgene_desc_t *gene;
    int i;

    if (gene_name == NULL) {
        return NULL;
    }

    gene_db = bcm56880_a0_gene_db_get(unit);
    if (gene_db == NULL) {
        return NULL;
    }

    for (i = 0; i < gene_db->genes; i++) {
        gene = &gene_db->gene[i];
        if (sal_strcmp(gene->name, gene_name) == 0) {
            return gene;
        }
    }

    return NULL;
}

/*!
 * \brief Add GENE features into device-based feature list.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmgene_feature_init(int unit)
{
    bcmgene_db_t *gene_db = NULL;
    bcmgene_desc_t *gene;
    int i;

    gene_db = bcm56880_a0_gene_db_get(unit);
    if (gene_db == NULL) {
        return SHR_E_NONE;
    }

    for (i = 0; i < gene_db->genes; i++) {
        gene = &gene_db->gene[i];
        if (gene->feature == LTSW_FT_ZERO) {
            continue;
        }
        ltsw_feature_enable(unit, gene->feature);
    }

    return SHR_E_NONE;
}

/******************************************************************************
 * Public functions
 */
bcmgene_desc_t *
bcm56880_a0_gene_extractor(int unit, const char *gene_name)
{
    return bcmgene_extractor(unit, gene_name);
}

int
bcm56880_a0_gene_feature_init(int unit)
{
    return bcmgene_feature_init(unit);
}

