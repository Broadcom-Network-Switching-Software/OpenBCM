/*! \file bcmltm_types.h
 *
 * Logical Table Manager Types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_TYPES_H
#define BCMLTM_TYPES_H

#include <shr/shr_error.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_table_constants.h>

/*!
 * \brief LT STATS categories.
 *
 * Translation of the LRD values because of prior implementation.
 */

typedef enum bcmltm_field_stats_e {
    BCMLRD_FIELD_STATS_LT_ID = TABLE_STATSt_TABLE_IDf,
    BCMLRD_FIELD_LT_ERROR_COUNT = TABLE_STATSt_TABLE_ERROR_CNTf,
    BCMLRD_FIELD_FIELD_SIZE_ERROR_COUNT = \
           TABLE_STATSt_FIELD_SIZE_ERROR_CNTf,
    BCMLRD_FIELD_FIELD_LIST_ERROR_COUNT = \
           TABLE_STATSt_FIELD_LIST_ERROR_CNTf,
    BCMLRD_FIELD_VALIDATE_ERROR_COUNT = TABLE_STATSt_VALIDATE_ERROR_CNTf,
    BCMLRD_FIELD_TRANSFORM_ERROR_COUNT = TABLE_STATSt_TRANSFORM_ERROR_CNTf,
    BCMLRD_FIELD_TABLE_HANDLER_ERROR_COUNT = \
           TABLE_STATSt_TABLE_HANDLER_ERROR_CNTf,
    BCMLRD_FIELD_LT_INSERT_COUNT = TABLE_STATSt_TABLE_INSERT_CNTf,
    BCMLRD_FIELD_LT_LOOKUP_COUNT = TABLE_STATSt_TABLE_LOOKUP_CNTf,
    BCMLRD_FIELD_LT_DELETE_COUNT = TABLE_STATSt_TABLE_DELETE_CNTf,
    BCMLRD_FIELD_LT_UPDATE_COUNT = TABLE_STATSt_TABLE_UPDATE_CNTf,
    BCMLRD_FIELD_LT_TRAVERSE_COUNT = TABLE_STATSt_TABLE_TRAVERSE_CNTf,
    BCMLRD_FIELD_LT_INSERT_ERROR_COUNT = \
           TABLE_STATSt_TABLE_INSERT_ERROR_CNTf,
    BCMLRD_FIELD_LT_LOOKUP_ERROR_COUNT = \
           TABLE_STATSt_TABLE_LOOKUP_ERROR_CNTf,
    BCMLRD_FIELD_LT_DELETE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_DELETE_ERROR_CNTf,
    BCMLRD_FIELD_LT_UPDATE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_UPDATE_ERROR_CNTf,
    BCMLRD_FIELD_LT_TRAVERSE_ERROR_COUNT = \
           TABLE_STATSt_TABLE_TRAVERSE_ERROR_CNTf,
    BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT = \
           TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf,
    BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT = \
           TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf,
    BCMLRD_FIELD_STATS_NUM = TABLE_STATSt_FIELD_COUNT
} bcmltm_field_stats_t;

/*!
 * \brief Logical Table Manager Types.
 *
 * The public types required by the LTM module which may be employed
 * by other modules upstream.
 */

/*! Maximum number of bits for a single field value (Field ID, index). */
#define BCMLTM_FIELD_SIZE_BITS           64

/*!
 * \brief LTM field list.
 *
 * This data structure defines a list of fields in an entry.
 */
typedef bcmlt_field_list_t bcmltm_field_list_t;

/*!
 * \brief LTM field list allocation callback.
 *
 * This function is provided by a higher layer than the LTM to create
 * field list elements which may be employed to return data
 * for operations which generate output field values.
 */
typedef bcmltm_field_list_t * (*bcmlt_field_list_alloc_f)(void);

/*!
 * \brief Logical Table Entry opcode.
 *
 * This data structure is a union of logical table opcode and
 * pass through opcode.
 */
typedef union {
    /*! Logical table opcode. */
    bcmlt_opcode_t lt_opcode;

    /*! Passthrough opcode. */
    bcmlt_pt_opcode_t pt_opcode;
} bcmltm_combined_opcode_t;

/*!
 * \brief Logical Table Manager Entry.
 *
 * The data structure which contains the information necessary to
 * fully specify a Logical Table Operation.
 */
typedef struct bcmltm_entry_s {
    /*! Unique rollup counter identify this entry. */
    uint32_t                  entry_id;

    /*!
     * Transaction ID for the LT entry specification of this operation.
     * Note that multiple entries may have the same ID.
     * That will bundle the entries into atomic operation.
     */
    uint32_t                  trans_id;

    /*! ID selecting the LT in which this entry operation will take place. */
    uint32_t                  table_id;

    /*! Device identifier. */
    int                       unit;

    /*! Entry attribute flags BCMLT_ENT_ATTR_xxx. */
    uint32_t                  attrib;

    /*!
     * Entry operation code. The pt flag determines which field
     * from the union is relevant.
     */
    bcmltm_combined_opcode_t  opcode;

    /*!
     * Externally provided allocation function for adding field
     * elements to the out_fields list when the operation retrieves
     * field values from the device or PT cache.
     */
    bcmlt_field_list_alloc_f  field_alloc_cb;

    /*!
     * List of field values provided by the application.
     * List is NULL terminated.
     */
    bcmltm_field_list_t       *in_fields;

    /*!
     * An application-allocated list of field values for the return
     * of data from the LTM. List is NULL terminated.
     */
    bcmltm_field_list_t       *out_fields;

    /*! Link pointer to the next entry in the list. */
    struct bcmltm_entry_s     *next;
} bcmltm_entry_t;

/*!
 * \brief PT status per LT operation.
 *
 * This structure contains the status for a single PT operation.
 */
typedef struct bcmltm_table_op_pt_info_s {
    /*! Enum of Logical table under operation. */
    uint32_t lt_id;

    /*! LT Transaction id. */
    uint32_t lt_trans_id;

    /*! Valid entries in pt_sid array */
    uint32_t pt_sid_cnt;

    /*! Instance. */
    uint32_t pt_instance[TABLE_OP_INFO_MAX_INDEX];

    /*! Physical SID. */
    uint32_t pt_sid[TABLE_OP_INFO_MAX_INDEX];

    /*! Valid entries in pt_index array */
    uint32_t pt_index_cnt;

    /*! Physical index. */
    uint32_t pt_index[TABLE_OP_INFO_MAX_INDEX];

    /*! Valid entries in pt_sid_data array */
    uint32_t pt_sid_data_cnt;

    /*! Physical SID data only. */
    uint32_t pt_sid_data[TABLE_OP_INFO_MAX_INDEX];

    /*! Valid entries in pt_index_data array */
    uint32_t pt_index_data_cnt;

    /*! Physical index data only. */
    uint32_t pt_index_data[TABLE_OP_INFO_MAX_INDEX];

} bcmltm_table_op_pt_info_t;

/*!
 * \brief PT status per LT operation.
 *
 * This structure contains the status for a single PT operation.
 */
typedef struct bcmltm_table_op_dop_info_s {
    /*! Enum of Logical table under operation. */
    uint32_t lt_id;

    /*! Instance. */
    uint32_t pt_instance[TABLE_OP_INFO_MAX_INDEX];

    /*! Valid entries in pt_sid array */
    uint32_t pt_sid_cnt;

    /*! Physical SID. */
    uint32_t pt_sid[TABLE_OP_INFO_MAX_INDEX];

    /*! Valid entries in pt_index array */
    uint32_t pt_index_cnt;

    /*! Physical index. */
    uint32_t pt_index[TABLE_OP_INFO_MAX_INDEX];

    /*! Port ID */
    uint32_t port_id;

    /*! DOP Data buffer pointers */
    uint8_t *dop_data[TABLE_OP_INFO_MAX_INDEX];
} bcmltm_table_op_dop_info_t;


/*!
 * \brief Define transaction operation status and callback.
 *
 * All logical table transaction operations result in a status
 * to be returned to the caller. This callback function is invoked
 * upon completion of a transaction.  The LTM module passes this
 * callback onward to the PTM so that module may notify when all portions
 * of a modeled transaction are complete.
 *
 * \param [in] event Indicate the reason the callback being called.
 * \param [in] trans_id Transaction ID of this notification.
 * \param [in] status Operation status indication.
 * \param [in] context Cookie supplied when callback registered.
 *
 * \return void None.
 */
typedef void (*bcmltm_trans_cb_f)(bcmlt_notif_option_t event,
                                  uint32_t trans_id,
                                  shr_error_t status,
                                  void *context);

/*!
 * \brief PT passthrough operation status.
 *
 * This structure contains the status for PT passthrough operation.
 */
typedef struct bcmltm_device_op_pt_info_s {
    /*! Physical table id. */
    uint32_t pt_id;

    /*! Physical table index. */
    int pt_index;

    /*! Physical table instance. */
    int pt_instance;

    /*! Physical table operation. */
    bcmlt_pt_opcode_t pt_op;

    /*! Operation status. */
    uint32_t pt_op_status;

} bcmltm_device_op_pt_info_t;

#endif /* BCMLTM_TYPES_H */
