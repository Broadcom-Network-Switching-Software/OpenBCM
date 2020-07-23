/*! \file bcmgene_db.h
 *
 * Broadcom Generic Extensible NPL Encapsulation (GENE) header file.
 * This file contains GENE definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_GENE_DB_H
#define BCMINT_LTSW_GENE_DB_H

#include <sal/sal_types.h>

/*!
 * \brief The struct of GENE field value map.
 */
typedef struct bcmgene_field_value_map_s {

    /*! The input field value. */
    uint64_t in;

    union {
        /*! The output field value. */
        uint64_t v;

        /*! The output field string value. */
        const char *s;

    } out;

} bcmgene_field_value_map_t;

/*!
 * \brief The struct of GENE field value converter.
 */
typedef struct bcmgene_field_value_converter_s {

    /*! The flags for controlling the GENE field. */
    uint32_t flags;

    /*! The value converter count. */
    int maps;

    /*! The value converter for input/output. */
    bcmgene_field_value_map_t *map;

} bcmgene_field_value_converter_t;

/*!
 * \brief The struct of GENE field.
 */
typedef struct bcmgene_field_desc_s {

    /*! The flags for controlling the GENE field. */
    uint32_t flags;

    /*! The GENE field handler start base. */
    int blk;

    /*! The GENE field handler. */
    int offset;

    /*! The bit width of GENE field. */
    int width;

    /*! The GENE field is array. */
    int depth;

    /*! The name of GENE field. */
    const char *field;

    /*! The field value converter of GENE field. */
    bcmgene_field_value_converter_t *converter;

} bcmgene_field_desc_t;

/*!
 * \brief The struct of GENE table.
 */
typedef struct bcmgene_table_desc_s {

    /*! The sub-feature of GENE table. */
    uint32_t sub_f;

    /*! The flags for controlling the GENE table. */
    uint32_t flags;

    /*! The name of GENE table. */
    const char *table;

    /*! The minimum index of GENE table. */
    int idx_min;

    /*! The maximum index of GENE table. */
    int idx_max;

    /*! The key count of GENE table. */
    int keys;

    /*! The key array of GENE table. */
    bcmgene_field_desc_t *key;

    /*! The field count of GENE table. */
    int flds;

    /*! The field array of GENE table. */
    bcmgene_field_desc_t *fld;

} bcmgene_table_desc_t;

/*!
 * \brief The struct of GENE functionality.
 */
typedef struct bcmgene_func_desc_s {

    /*! The indicator of GENE functionality. */
    uint32_t func;

    /*! The table count for this GENE functionality. */
    int tbls;

    /*! The table array for this GENE functionality. */
    bcmgene_table_desc_t *tbl;

} bcmgene_func_desc_t;

/*!
 * \brief The struct of GENE descriptor.
 */
typedef struct bcmgene_desc_s {

    /*! The name of GENE. */
    const char *name;

    /*! The feature of GENE. */
    int feature;

    /*! The functionality count of GENE. */
    int funcs;

    /*! The functionality array of GENE. */
    bcmgene_func_desc_t *func;

} bcmgene_desc_t;


/*!
 * \brief The struct of GENE database.
 */
typedef struct bcmgene_db_s {

    /*! The version of GENE database. */
    const char *ver;

    /*! The descriptor count of GENE. */
    int genes;

    /*! The descriptor array of GENE. */
    bcmgene_desc_t *gene;

} bcmgene_db_t;

/*!
 * \brief Get the GENE database for a device.
 *
 * \param [in] unit Unit Number.
 *
 * \retval !NULL Success.
 * \retval NULL Failure.
 */
extern bcmgene_db_t *
bcmi_ltsw_gene_db_get(int unit);

#endif /* BCMINT_LTSW_GENE_DB_H */
