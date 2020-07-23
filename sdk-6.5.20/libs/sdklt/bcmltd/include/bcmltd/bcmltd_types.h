/*! \file bcmltd_types.h
 *
 * Logical Table Data Types header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTD_TYPES_H
#define BCMLTD_TYPES_H

#include <shr/shr_types.h>
#include <shr/shr_fmm.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltd/chip/bcmltd_limits.h>

/*! Maximum number of bits for a single field value */
#define BCM_FIELD_SIZE_BITS  64

/*!
 * \brief Field values raw data structure.
 */
typedef struct bcmltd_binary_field_data_s {
    /*! Number of bits in field value. */
    uint32_t  num_of_bits;

    /*! Byte array containing raw data value. */
    const uint8_t *bytes;
} bcmltd_binary_field_data_t;

/*!
 * \brief Basic field data value.
 *
 * This data structure used to assign values to fields in an entry.
 */
typedef union bcmltd_field_data_u {

    /*! Boolean type. */
    bool is_true;

    /*! uint8_t. */
    uint8_t u8;

    /*! uint16_t. */
    uint16_t u16;

    /*! uint32_t. */
    uint32_t u32;

    /*! uint64_t. */
    uint64_t u64;

    /*! Raw catch-all type. */
    bcmltd_binary_field_data_t binary;
} bcmltd_field_data_t;

/*!
 * \brief Field data tag type.
 *
 * This value is used to interpret a field data type.
 */
typedef bcmlt_field_data_tag_t bcmltd_field_data_tag_t;

/*!
 * \brief Field access modes.
 */
typedef bcmlt_field_acc_t bcmltd_field_acc_t;

/*! Allow read and write access. */
#define BCMLTD_FIELD_ACCESS_READ_WRITE BCMLT_FIELD_ACCESS_READ_WRITE

/*! Read only access mode.       */
#define BCMLTD_FIELD_ACCESS_READ_ONLY  BCMLT_FIELD_ACCESS_READ_ONLY

/*!
 * \brief Logical table ID type.
 */
typedef uint32_t bcmltd_sid_t;

/*!
 * \brief Logical field ID type.
 */
typedef uint32_t bcmltd_fid_t;

/*!
 * \brief Global logical field ID type.
 */
typedef bcmltd_fid_t bcmltd_gfid_t;

/*!
 * \brief Invalid logical table ID.
 */
#define BCMLTD_SID_INVALID       ((bcmltd_sid_t)-1)

/*!
 * \brief Invalid logical table ID
 *
 * To store invalid LTID in HA for ISSU upgrade,
 * this invalid LTID has to be used.
 */
#define BCMLTD_INVALID_LT       BCMLTD_SID_INVALID

/*!
 * \brief Invalid logical field ID.
 */
#define BCMLTD_FID_INVALID       ((bcmltd_fid_t)-1)

/*!
 * \brief Index table flag.
 */
#define BCMLTD_TABLE_F_TYPE_INDEX       0x00000000

/*!
 * \brief TCAM table flag.
 */
#define BCMLTD_TABLE_F_TYPE_TCAM        0x00000001

/*!
 * \brief HASH table flag.
 */
#define BCMLTD_TABLE_F_TYPE_HASH        0x00000002

/*!
 * \brief Table type mask.
 */
#define BCMLTD_TABLE_F_TYPE_MASK        0x00000003

/*!
 * \brief Table read-only flag.
 */
#define BCMLTD_TABLE_F_READ_ONLY        0x00000004

/*!
 * \brief Table type index allocate.
 */
#define BCMLTD_TABLE_F_TYPE_INDEX_ALLOC 0x00000010

/*!
 * \brief Table configuration flag.
 */
#define BCMLTD_TABLE_F_CONFIG           0x00000020

/*!
 * \brief Table type mapped.
 */
#define BCMLTD_TABLE_F_TYPE_MAPPED      0x00000040

/*!
 * \brief Field untyped flag.
 */
#define BCMLTD_FIELD_F_UNTYPED          0x00000000

/*!
 * \brief Field special type mask.
 */
#define BCMLTD_FIELD_F_TYPE_MASK        0x00000003

/*!
 * \brief Field read-only flag.
 */
#define BCMLTD_FIELD_F_READ_ONLY        0x00000004

/*!
 * \brief Field "is key" flag.
 */
#define BCMLTD_FIELD_F_KEY              0x00000008

/*!
 * \brief Field "is unmapped" flag.
 */
#define BCMLTD_FIELD_F_UNMAPPED         0x00000010

/*!
 * \brief Field enumeration flag.
 */
#define BCMLTD_FIELD_F_ENUM             0x00000020

/*!
 * \brief Field non-cacheable flag.
 */
#define BCMLTD_FIELD_F_NON_CACHEABLE    0x00000040


/*!
 * \brief Enum type symbol lookup data.
 */
typedef struct bcmltd_enum_type_s {
    /*! LTL enum type name. */
    const char *name;

    /*! Number of enumeration symbols. */
    uint32_t num_sym;

    /*! Array of enumeration symbols. */
    const shr_enum_map_t *sym;

} bcmltd_enum_type_t;


/*!
 * \brief Field storage representation
 */
typedef struct bcmltd_field_rep_s {
    /*! Field name in string pool. */
    const char *name;

    /*! BCMLTD_FIELD_F_ flags. */
    uint32_t flags;

    /*! Width of field in bits. */
    uint32_t width;

    /*! Depth of field in elements. */
    uint32_t depth;

    /*! Enumeration symbol data. */
    const bcmltd_enum_type_t *edata;

    /*! Field description. */
    const char *desc;

} bcmltd_field_rep_t;

/*!
 * \brief Table storage representation
 */
typedef struct bcmltd_table_rep_s {
    /*! Table name in string pool. */
    const char *name;

    /*! BCMLTD_TABLE_F_ flags. */
    uint32_t flags;

    /*! Number of fields in this table. */
    uint32_t fields;

    /*! Variable length array of fields in this table. */
    const bcmltd_field_rep_t *field;

    /*! Table description. */
    const char *desc;
} bcmltd_table_rep_t;

/*!
 * \brief Table configuration
 */
typedef struct bcmltd_table_conf_s {
    /*! Number of tables. */
    uint32_t tables;

    /*! Variable length array of tables in this table configuration. */
    const bcmltd_table_rep_t **table;

} bcmltd_table_conf_t;

/*!
 * \brief Logical table field information.
 *
 * Logical table field information suitable for client APIs.
 */
typedef struct bcmltd_field_def_s {

    /*! Field ID. */
    uint32_t                id;

    /*! Field name. */
    const char              *name;

    /*! Access type of the field. */
    bcmltd_field_acc_t      access;

    /*! Minimum value allowed. */
    bcmltd_field_data_t     min;

    /*! Maximum value allowed. */
    bcmltd_field_data_t     max;

    /*! Default value for the field. */
    bcmltd_field_data_t     def;

    /*! Data tag to identify type. */
    bcmltd_field_data_tag_t dtag;

    /*! True if field part of the key. */
    bool                    key;

    /*! True if the field is an array. */
    bool                    array;

    /*! True if the field has associated symbols. */
    bool                    symbol;

    /*! Width of field in bits. */
    uint32_t                width;

    /*! Depth of field array in elements. */
    uint32_t                depth;

    /*! Number of scalar elements needed for field. */
    uint32_t                elements;

} bcmltd_field_def_t;

/*!
 * \brief Transaction and Entry Status
 *
 * The operation status for the entry is returned to the client using these
 * enumerated values. The entry accessors APIs can be used to get the status
 * for the entry operation.
 *
 * For transactions, entry_hdl status represents the combined status. Each entry in
 * the transaction has it's own status for the operation. For Batch transactions
 * if some entry operations failed, the overall status will be set to partial
 * success.
 */
typedef enum bcmlt_status_e {
    BCMLT_STATUS_NOT_HANDLED = 0,
    BCMLT_STATUS_SUCCESS,       /*!< Requested operation succeeded.    */
    BCMLT_STATUS_PART_SUCCESS,  /*!< For Transactions in batch.        */
    BCMLT_STATUS_NOTALLOWED,    /*!< Permissions or locked table.      */
    BCMLT_STATUS_NORESOURCE,    /*!< For allocation/insert operations. */
    BCMLT_STATUS_INVALID,       /*!< Invalid table/field(s).           */
    BCMLT_STATUS_HWERROR,       /*!< Could not access Hardware.        */
    BCMLT_STATUS_NUM
} bcmlt_status_t;

/*!
 * \brief Basic field data value.
 *
 * This data structure used to assign values to fields in an entry.
 */
typedef bcmltd_field_data_t bcmlt_field_data_t;

/*!
 * \brief Field list.
 *
 * The BCMLT field list is an alias for the shared field memory
 * manager field list.
 */
typedef shr_fmm_t bcmlt_field_list_t;

/*!
 * \brief Enum by type lookup data.
 */
typedef struct bcmltd_enum_by_type_s {

    /*! Number of enumeration types. */
    uint32_t num_enum_type;

    /*! Array of enumeration types. */
    const bcmltd_enum_type_t *enum_type;

} bcmltd_enum_by_type_t;

#endif /* BCMLTD_TYPES_H */
