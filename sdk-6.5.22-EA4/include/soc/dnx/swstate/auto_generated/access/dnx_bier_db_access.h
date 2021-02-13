/** \file dnx/swstate/auto_generated/access/dnx_bier_db_access.h
 *
 * sw state functions declarations
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_BIER_DB_ACCESS_H__
#define __DNX_BIER_DB_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_bier_db_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_bier_db_is_init
 */
typedef int (*dnx_bier_db_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_bier_db_init
 */
typedef int (*dnx_bier_db_init_cb)(
    int unit);

/**
 * implemented by: dnx_bier_db_set_size_set
 */
typedef int (*dnx_bier_db_set_size_set_cb)(
    int unit, uint32 set_size_idx_0, int set_size);

/**
 * implemented by: dnx_bier_db_set_size_get
 */
typedef int (*dnx_bier_db_set_size_get_cb)(
    int unit, uint32 set_size_idx_0, int *set_size);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable set_size
 */
typedef struct {
    dnx_bier_db_set_size_set_cb set;
    dnx_bier_db_set_size_get_cb get;
} dnx_bier_db_set_size_cbs;

/**
 * This structure holds the access functions for the variable dnx_bier_db_t
 */
typedef struct {
    dnx_bier_db_is_init_cb is_init;
    dnx_bier_db_init_cb init;
    /**
     * Access struct for set_size
     */
    dnx_bier_db_set_size_cbs set_size;
} dnx_bier_db_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_bier_db_cbs dnx_bier_db;

#endif /* __DNX_BIER_DB_ACCESS_H__ */
