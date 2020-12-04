/*! \file bcmcth_info_table_entry.h
 *
 * Header file of BCMCTH info table data holder struct.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_INFO_TABLE_ENTRY_H
#define BCMCTH_INFO_TABLE_ENTRY_H

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_client.h>

/*! Maximum policy index supported. */
#define INFO_TBL_MAX_POLICY 8

/*! Maximum enum length. */
#define INFO_TBL_ENUM_LEN   80

/*! Per enum field entry struct. */
typedef struct bcmcth_info_table_enum_entry_s {
    /*! Table name */
    bcmlrd_sid_t table_id;

    /*! Table and field name */
    bcmlrd_fid_t field_id;

    /*! Enum value */
    char value[INFO_TBL_ENUM_LEN];
} bcmcth_info_table_enum_entry_t;

/*! Per scalar field entry struct. */
typedef struct bcmcth_info_table_scalar_entry_s {
    /*! Table name */
    bcmlrd_sid_t table_id;

    /*! Table and field name */
    bcmlrd_fid_t field_id;

    /*! scalar value */
    uint32_t value;
} bcmcth_info_table_scalar_entry_t;

/*! Per policy field entry struct. */
typedef struct bcmcth_info_table_policy_entry_s {
    /*! Table name */
    bcmlrd_sid_t table_id;

    /*! Table and field name */
    bcmlrd_fid_t field_id;

    /*! Enum value */
    char value[INFO_TBL_MAX_POLICY][INFO_TBL_ENUM_LEN];
} bcmcth_info_table_policy_entry_t;

/*! Data holder for policy entries. */
typedef struct bcmtch_info_tbl_policy_s {
    /*! Policy field dimension holder. */
    bcmcth_info_table_scalar_entry_t policy_size;

    /*! Policy field holder. */
    bcmcth_info_table_policy_entry_t policy_field;
} bcmtch_info_tbl_policy_t;


/*! Info table per variant data holder. */
typedef struct bcmcth_info_table_s {

    /*! Info table name. */
    uint8_t table_name[100];

    /*! Variant based table id. */
    int32_t table_id;

    /*! Maximum policy count used by NPL. */
    uint32_t max_policy_supported;

    /*! Total enum number. */
    uint32_t enum_cnt;

    /*! Total profile number. */
    uint32_t profile_cnt;

    /*! Enum info holder. */
    bcmcth_info_table_enum_entry_t *enum_info;

    /*! Policy info holder. */
    bcmtch_info_tbl_policy_t *policy_info;
} bcmcth_info_table_t;

/*!
 * \brief Load data into info table at init time.
 *
 * \param [in] unit Unit number.
 * \param [in] data data buffer that holds information.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resource.
 */
int
info_tbl_init(int unit, bcmcth_info_table_t *data, bool warm);

#endif /* BCMCTH_INFO_TABLE_ENTRY_H */
