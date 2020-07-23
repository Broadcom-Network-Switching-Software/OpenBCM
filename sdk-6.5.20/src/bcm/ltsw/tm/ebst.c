/* \file ebst.c
 *
 * TM EBST Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/mbcm/ebst.h>
#include <bcm_int/ltsw/tm/ebst.h>

#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_int.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/switch.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define DEFAULT_PROFILE_ID          0
#define MAX_ITM_PER_DEV             2
#define MAX_EBST_DATA_ID            0x1f3ff

#define EBST_SCAN_OFF               0x0
#define EBST_SCAN_START             0x1
#define EBST_SCAN_STOP              0x2
#define EBST_SCAN_FIFO_FULL         0x3

#define EBST_OP_GET             0x1
#define EBST_OP_SET             0x2

/* ebst FIFO FULL notification info. */
typedef struct cosq_ebst_event_notif_s {
    /* Enable mode. */
    int ebst_mode;

    /* The field is enabled when hardware FIFO full interrupt is generated */
    bool itm_fifo_full[MAX_ITM_PER_DEV];
} cosq_ebst_event_notif_t;

typedef struct ebst_config_s {
    /* Enable to monitor the queue during an EBST scan. */
    int monitor;

    /*
     * Base index in the TM_EBST_DATA table for the block
     * associated with the queue.
     */
    int base_index;

    /*
     * Number of entries starting from BASE_INDEX
     * associated with the queue in the TM_EBST_DATA table.
     */
    int num_entries;

    /*
     * Read only field indicating the last updated entry
     * in the TM_EBST_DATA table associated with the queue.
     */
    int data_id[MAX_ITM_PER_DEV];
} ebst_config_t;

typedef struct cosq_ebst_profile_s {
    /* Start threshold level in cells. */
    int start_threshold_cells;

    /* Stop threshold level in cells. */
    int stop_threshold_cells;
} cosq_ebst_profile_t;

/*
 * \brief BST profile operation function.
 *
 * \param [in] unit Unit Number.
 * \param [in] res_info cosq_bst_handler_t.
 * \param [in] local_port local_port.
 * \param [in] index index.
 * \param [in] flags flags.
 * \param [in] profile profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*profile_op_f)(
    int unit,
    void *res_info,
    int local_port,
    int index,
    int flags,
    cosq_ebst_profile_t *profile);

/* Struct of EBST profile. */
typedef struct ebst_profile_s {

    /* Logical table name. */
    const char *thd_lt;

    /* port. */
    const char *key;

    /* Key 1. */
    const char *key1;

    /* Profile id field. */
    bcmi_lt_field_t field;

    /* EBST profile operation function. */
    profile_op_f profile_op;
} ebst_profile_t;

/*
 * \brief upadte an entry to TM_EBST_PORT/UC/MC.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
typedef int (*data_config_set_f)(
    int unit,
    const char *lt_val,
    const char *port_key,
    int port,
    const char *queue_key,
    int cosq,
    ebst_config_t *config);

/*
 * \brief get an entry from TM_EBST_PORT/UC/MC.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [out]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
typedef int (*data_config_get_f)(
    int unit,
    const char *lt_val,
    const char *port_key,
    int port,
    const char *queue_key,
    int cosq,
    ebst_config_t *config);

/* Struct of EBST config. */
typedef struct ebst_stat_s {

    /* Logical table name. */
    const char *thd_lt;

    /* port. */
    const char *key;

    /* Key 1. */
    const char *key1;

    /* EBST config operation function. */
    data_config_set_f data_config_set;

    /* EBST config operation function. */
    data_config_get_f data_config_get;

} ebst_stat_t;

typedef struct cosq_ebst_handler_s {

    /* Struct of EBST profile. */
    ebst_profile_t profile_hdl;

    /* Struct of EBST DATA config. */
    ebst_stat_t stat_hdl;

} cosq_ebst_handler_t;

/*
 * \brief Table data to play back.
 */
typedef struct ebst_data_list_s {
    /* Base index. */
    uint32_t base_index;

    /* Number entries. */
    uint32_t num_entries;

    /* port number. */
    int port;

    /* cosq. */
    int cosq;

    /* bid. */
    bcm_bst_stat_id_t bid;

    /* Next table in list */
    struct ebst_data_list_s *next;
} ebst_data_list_t;

typedef struct cosq_ebst_info_s {

    /* Tracking enable. */
    int *tracking_enable;

    /* SCAN round. */
    int *scan_interval;

    /* Struct of EBST DATA config. */
    ebst_data_list_t *list;

    /* EBST DATA index. */
    uint32_t  current_ebst_data_index;
} cosq_ebst_info_t;

/* Software bookkeeping for EBST information. */
static cosq_ebst_info_t cosq_ebst_info[BCM_MAX_NUM_UNITS];

/* EBST info. */
#define COSQ_EBST_INFO(_u_) (&cosq_ebst_info[_u_])

static tm_ltsw_ebst_chip_driver_t *tm_ebst_chip_driver_cb[BCM_MAX_NUM_UNITS];

typedef struct tm_cosq_ebst_status_s {
    /* Enable. */
    int scan_enable;
} tm_cosq_ebst_status_t;

typedef struct tm_cosq_ebst_control_para_s {
    /* EBST scan mode. */
    int scan_mode;

    /* Scan round. */
    int scan_round;

    /* Enable. */
    int scan_enable;

    /* Scan threshold. */
    int scan_threshold;
} tm_cosq_ebst_control_para_t;

/* EBST data buffer info structure */
typedef struct tm_cosq_ebst_data_entry_s {
    uint64 timestamp;           /* Timestamp */
    uint32 cells;               /* Buffer occupancy level in cells */
    bcm_cosq_color_packet_drop_state_t green_drop_state;    /* Green packet drop state */
    bcm_cosq_color_packet_drop_state_t yellow_drop_state;   /* Yellow packet drop state */
    bcm_cosq_color_packet_drop_state_t red_drop_state;      /* Red packet drop state */
} tm_cosq_ebst_data_entry_t;

/******************************************************************************
 * Private functions
 */

static int
cosq_ebst_data_table_sync(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, ebst_config_t *config);

static int
cosq_ebst_data_info_restore(int unit);

static int
cosq_ebst_data_info_clear(int unit);

/*
 * \brief Check whether a field is valid in the LT.
 *
 * \param [in] unit         Unit Number.
 * \param [in] tbl_name     Table name.
 * \param [in] fld_name     Field name.
 * \param [out] valid       TRUE/FALSE.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_ebst_field_validation(int unit, const char *tbl_name,
                                const char *fld_name, bool *valid)
{
    int rv = SHR_E_NONE;
    uint32_t ltid;
    uint32_t lfid;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_table_field_id_get_by_name(unit,
                                          tbl_name,
                                          fld_name,
                                          &ltid,
                                          &lfid);
    if (rv == SHR_E_NOT_FOUND) {
        *valid = FALSE;
        rv = SHR_E_NONE;
    } else if (rv == SHR_E_NONE) {
        *valid = TRUE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key0_val             key 0 field.
 * \param [in]  key0_value           key 0 value.
 * \param [in]  key1_val             key 1 field.
 * \param [in]  key1_value           key 1 value.
 * \param [in]  key2_val             key 2 field.
 * \param [in]  key2_value           key 2 value.
 * \param [in]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_table_set(int unit,
                       const char *lt_val,
                       const char *key0_val,
                       int key0_value,
                       const char *key1_val,
                       int key1_value,
                       const char *key2_val,
                       int key2_value,
                       bcmi_lt_field_t *field)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key0_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key0_val,
                                   key0_value));
    }

    if (key1_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key1_val,
                                   key1_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               field->fld_name,
                               field->u.val));

    rv = bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL);

    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(unit, "logic table : %s %s=0x%d %s=0x%d %s=0x%d %s=0x%d\n"),
                           lt_val, key0_val ? key0_val : "invalid", key0_value,
                           key1_val ? key1_val : "invalid", key1_value,
                           key2_val ? key2_val : "invalid", key2_value,
                           field->fld_name, (int)field->u.val));
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief get an entry from LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key0_val             key 0 field.
 * \param [in]  key0_value           key 0 value.
 * \param [in]  key1_val             key 1 field.
 * \param [in]  key1_value           key 1 value.
 * \param [in]  key2_val             key 2 field.
 * \param [in]  key2_value           key 2 value.
 * \param [out] field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_table_get(int unit,
                       const char *lt_val,
                       const char *key0_val,
                       int key0_value,
                       const char *key1_val,
                       int key1_value,
                       const char *key2_val,
                       int key2_value,
                       bcmi_lt_field_t *field)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key0_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key0_val,
                                   key0_value));
    }

    if (key1_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key1_val,
                                   key1_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, field->fld_name,
                               &(field->u.val)));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from TM_EBST_DATA.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  buffer_id            buffer id.
 * \param [in]  data_id              data id.
 * \param [out] entry_data         EBST data info.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_data_get(int unit, int buffer_id, int data_id,
                            tm_cosq_ebst_data_entry_t *entry_data)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {BUFFER_POOLs,     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/  {TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/  {TIMESTAMPs,       BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/  {CELLSs,           BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/  {GREEN_DROPs,      BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*5*/  {YELLOW_DROPs,     BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*6*/  {RED_DROPs,        BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    bool green_drop_valid = TRUE;
    bool yellow_drop_valid = TRUE;
    bool red_drop_valid = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_field_validation(unit, TM_EBST_DATAs,
                                    GREEN_DROPs, &green_drop_valid));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_field_validation(unit, TM_EBST_DATAs,
                                    YELLOW_DROPs, &yellow_drop_valid));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_field_validation(unit, TM_EBST_DATAs,
                                    RED_DROPs, &red_drop_valid));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer_id;
    fields[1].u.val = data_id;

    fields[4].flags |= green_drop_valid ? BCMI_LT_FIELD_F_GET : 0;
    fields[5].flags |= yellow_drop_valid ? BCMI_LT_FIELD_F_GET : 0;
    fields[6].flags |= red_drop_valid ? BCMI_LT_FIELD_F_GET : 0;

    rv = bcmi_lt_entry_get(unit, TM_EBST_DATAs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_EMPTY) {
        entry_data->timestamp = 0;
        entry_data->cells = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    entry_data->timestamp = fields[2].u.val;
    entry_data->cells = fields[3].u.val;

    if (green_drop_valid == TRUE) {
        if (sal_strcasecmp(fields[4].u.sym_val, ACCEPT_ALLs) == 0) {
            entry_data->green_drop_state = bcmCosqColorPktAccept ;
        } else {
            entry_data->green_drop_state = bcmCosqColorPktDrop;
        }
    }

    if (yellow_drop_valid == TRUE) {
        if (sal_strcasecmp(fields[5].u.sym_val, ACCEPT_YELLOW_REDs) == 0) {
            entry_data->yellow_drop_state = bcmCosqColorPktAccept ;
        } else {
            entry_data->yellow_drop_state = bcmCosqColorPktDrop;
        }
    }

    if (red_drop_valid == TRUE) {
        if (sal_strcasecmp(fields[6].u.sym_val, ACCEPT_REDs) == 0) {
            entry_data->red_drop_state = bcmCosqColorPktAccept ;
        } else {
            entry_data->red_drop_state = bcmCosqColorPktDrop;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief upadte an entry to TM_EBST_PORT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 port.
 * \param [in]  monitor              monitor.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_port_monitor_set(int unit,
                              int port,
                              int monitor)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {PORT_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/  {MONITORs,         BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;
    fields[1].u.val = monitor;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_EBST_PORTs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get an entry from TM_EBST_PORT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 port.
 * \param [out]  monitor             monitor.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_port_monitor_get(int unit,
                              int port,
                              int *monitor)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {PORT_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/  {MONITORs,         BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;

    rv = bcmi_lt_entry_get(unit, TM_EBST_PORTs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *monitor = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    *monitor = fields[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief upadte an entry to TM_EBST_PORT_SERVICE_POOL.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_uc_pool_data_config_set(int unit,
                                     const char *lt_val,
                                     const char *port_key,
                                     int port,
                                     const char *pool_key,
                                     int pool,
                                     ebst_config_t *config)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {UC_BASE_INDEXs,      0, 0, {0}},
        /*3*/  {UC_NUM_ENTRIESs,     0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (pool_key != NULL) {
        fields[1].fld_name = pool_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = pool;
    }

    if (config->monitor != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_port_monitor_set(unit, port, config->monitor));
    }

    if (config->base_index != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = config->base_index;
    }

    if (config->num_entries != -1) {
        fields[3].flags = BCMI_LT_FIELD_F_SET;
        fields[3].u.val = config->num_entries;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, lt_val, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get an entry from TM_EBST_PORT_SERVICE_POOL.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [out]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_uc_pool_data_config_get(int unit,
                                     const char *lt_val,
                                     const char *port_key,
                                     int port,
                                     const char *pool_key,
                                     int pool,
                                     ebst_config_t *config)
{
    int itm;
    int rv;
    int monitor;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {UC_BASE_INDEXs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/  {UC_NUM_ENTRIESs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/  {UC_TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*5*/  {UC_TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                     BCMI_LT_FIELD_F_GET, 1, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (pool_key != NULL) {
        fields[1].fld_name = pool_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = pool;
    }

    rv = bcmi_lt_entry_get(unit, lt_val, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        for (itm = 0; itm < device_info.num_itm; itm++) {
            config->data_id[itm] = 0;
        }
        config->monitor = 0;
        config->base_index = 0;
        config->num_entries = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    config->base_index = fields[2].u.val;
    config->num_entries = fields[3].u.val;

    for (itm = 0; itm < device_info.num_itm; itm++) {
        config->data_id[itm] = fields[itm + 4].u.val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_ebst_port_monitor_get(unit, port, &monitor));

    config->monitor = monitor;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief upadte an entry to TM_EBST_PORT_SERVICE_POOL.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_mc_pool_data_config_set(int unit,
                                     const char *lt_val,
                                     const char *port_key,
                                     int port,
                                     const char *pool_key,
                                     int pool,
                                     ebst_config_t *config)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {MC_BASE_INDEXs,      0, 0, {0}},
        /*3*/  {MC_NUM_ENTRIESs,     0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (pool_key != NULL) {
        fields[1].fld_name = pool_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = pool;
    }

    if (config->monitor != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_port_monitor_set(unit, port, config->monitor));
    }

    if (config->base_index != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = config->base_index;
    }

    if (config->num_entries != -1) {
        fields[3].flags = BCMI_LT_FIELD_F_SET;
        fields[3].u.val = config->num_entries;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, lt_val, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get an entry from TM_EBST_PORT_SERVICE_POOL.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [out]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_mc_pool_data_config_get(int unit,
                                     const char *lt_val,
                                     const char *port_key,
                                     int port,
                                     const char *pool_key,
                                     int pool,
                                     ebst_config_t *config)
{
    int itm;
    int rv;
    int monitor;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {MC_BASE_INDEXs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/  {MC_NUM_ENTRIESs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/  {MC_TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*5*/  {MC_TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                     BCMI_LT_FIELD_F_GET, 1, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (pool_key != NULL) {
        fields[1].fld_name = pool_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = pool;
    }

    rv = bcmi_lt_entry_get(unit, lt_val, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        for (itm = 0; itm < device_info.num_itm; itm++) {
            config->data_id[itm] = 0;
        }
        config->monitor = 0;
        config->base_index = 0;
        config->num_entries = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    config->base_index = fields[2].u.val;
    config->num_entries = fields[3].u.val;

    for (itm = 0; itm < device_info.num_itm; itm++) {
        config->data_id[itm] = fields[itm + 4].u.val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_ebst_port_monitor_get(unit, port, &monitor));

    config->monitor = monitor;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief upadte an entry to TM_EBST_PORT/UC/MC.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_data_config_set(int unit,
                             const char *lt_val,
                             const char *port_key,
                             int port,
                             const char *queue_key,
                             int cosq,
                             ebst_config_t *config)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {MONITORs,         0, 0, {0}},
        /*3*/  {BASE_INDEXs,      0, 0, {0}},
        /*4*/  {NUM_ENTRIESs,     0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (queue_key != NULL) {
        fields[1].fld_name = queue_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = cosq;
    }

    if (config->monitor != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = config->monitor;
    }

    if (config->base_index != -1) {
        fields[3].flags = BCMI_LT_FIELD_F_SET;
        fields[3].u.val = config->base_index;
    }

    if (config->num_entries != -1) {
        fields[4].flags = BCMI_LT_FIELD_F_SET;
        fields[4].u.val = config->num_entries;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, lt_val, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get an entry from TM_EBST_PORT/UC/MC.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               logic table.
 * \param [in]  port_key             port key.
 * \param [in]  port                 port.
 * \param [in]  queue_key            queue key.
 * \param [in]  cosq                 cosq.
 * \param [out]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_data_config_get(int unit,
                             const char *lt_val,
                             const char *port_key,
                             int port,
                             const char *queue_key,
                             int cosq,
                             ebst_config_t *config)
{
    int itm;
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {NULL,             0, 0, {0}},
        /*1*/  {NULL,             0, 0, {0}},
        /*2*/  {MONITORs,         BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/  {BASE_INDEXs,      BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/  {NUM_ENTRIESs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*5*/  {TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                  BCMI_LT_FIELD_F_GET, 0, {0}},
        /*6*/  {TM_EBST_DATA_IDs, BCMI_LT_FIELD_F_ARRAY |
                                  BCMI_LT_FIELD_F_GET, 1, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    if (port_key != NULL) {
        fields[0].fld_name = port_key;
        fields[0].flags = BCMI_LT_FIELD_F_SET;
        fields[0].u.val = port;
    }

    if (queue_key != NULL) {
        fields[1].fld_name = queue_key;
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = cosq;
    }

    rv = bcmi_lt_entry_get(unit, lt_val, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        for (itm = 0; itm < device_info.num_itm; itm++) {
            config->data_id[itm] = 0;
        }
        config->monitor = 0;
        config->base_index = 0;
        config->num_entries = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    config->monitor = fields[2].u.val;
    config->base_index = fields[3].u.val;
    config->num_entries = fields[4].u.val;

    for (itm = 0; itm < device_info.num_itm; itm++) {
        config->data_id[itm] = fields[itm + 5].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from TM_EBST_STATUSs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out]  ebst_status         Ebst status.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_status_get(int unit, tm_cosq_ebst_status_t *ebst_status)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {EBSTs,           BCMI_LT_FIELD_F_GET |
                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ebst_status, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    rv = bcmi_lt_entry_get(unit, TM_EBST_STATUSs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        ebst_status->scan_enable = EBST_SCAN_OFF;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (sal_strcasecmp(fields[0].u.sym_val, OFFs) == 0) {
        ebst_status->scan_enable = EBST_SCAN_OFF;
    } else if (sal_strcasecmp(fields[0].u.sym_val, EBST_STARTs) == 0) {
        ebst_status->scan_enable = EBST_SCAN_START;
    } else if (sal_strcasecmp(fields[0].u.sym_val, EBST_STOPs) == 0) {
        ebst_status->scan_enable = EBST_SCAN_STOP;
    } else if (sal_strcasecmp(fields[0].u.sym_val, EBST_FIFO_FULLs) == 0) {
        ebst_status->scan_enable = EBST_SCAN_FIFO_FULL;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief update an entry to TM_EBST_CONTROLs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  scan_round          scan_round.
 * \param [in]  scan_mode           scan_mode.
 * \param [in]  enable_mode         enable_mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_control_set(int unit, tm_cosq_ebst_control_para_t *control_para)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {SCAN_ROUNDs,      0, 0, {0}},
        /*1*/  {SCAN_MODEs,       BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/  {EBSTs,            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/  {SCAN_THDs,        0, 0, {0}},
    };
    bool valid = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(control_para, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    if (control_para->scan_round != -1) {
        fields[0].flags |= BCMI_LT_FIELD_F_SET;
        fields[0].u.val = control_para->scan_round;
    }

    if (control_para->scan_mode != -1) {
        fields[1].flags |= BCMI_LT_FIELD_F_SET;
        fields[1].u.sym_val = (control_para->scan_mode == 0) ? QUEUESs :
                              ((control_para->scan_mode == 1) ? UC_PORT_SERVICE_POOLs :
                                                  MC_PORT_SERVICE_POOLs);
    }

    if (control_para->scan_enable!= -1) {
        fields[2].flags |= BCMI_LT_FIELD_F_SET;
        fields[2].u.sym_val = (control_para->scan_enable == 0) ? OFFs :
                              ((control_para->scan_enable == 1) ? EBST_STARTs : EBST_STOPs);
    }

    if (control_para->scan_threshold != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_field_validation(unit, TM_EBST_CONTROLs,
                                        SCAN_THDs, &valid));
        fields[3].flags |= valid ? BCMI_LT_FIELD_F_SET : 0;
        fields[3].u.val = control_para->scan_threshold;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_EBST_CONTROLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from TM_EBST_CONTROLs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out]  scan_round          scan_round.
 * \param [out]  scan_mode           scan_mode.
 * \param [out]  enable_mode         enable_mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_tm_ebst_control_get(int unit, tm_cosq_ebst_control_para_t *control_para)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {SCAN_ROUNDs,     BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/  {SCAN_MODEs,      BCMI_LT_FIELD_F_GET |
                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/  {EBSTs,           BCMI_LT_FIELD_F_GET |
                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/  {SCAN_THDs,       0, 0, {0}},
    };
    bool valid = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(control_para, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_field_validation(unit, TM_EBST_CONTROLs,
                                    SCAN_THDs, &valid));
    fields[3].flags |= valid ? BCMI_LT_FIELD_F_GET : 0;

    rv = bcmi_lt_entry_get(unit, TM_EBST_CONTROLs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        control_para->scan_round = 0;
        control_para->scan_mode = 0;
        control_para->scan_enable = 0;
        control_para->scan_threshold = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    control_para->scan_round = fields[0].u.val;

    if (sal_strcasecmp(fields[1].u.sym_val, QUEUESs) == 0) {
        control_para->scan_mode = 0;
    } else if (sal_strcasecmp(fields[1].u.sym_val, UC_PORT_SERVICE_POOLs) == 0) {
        control_para->scan_mode = 1;
    } else {
        control_para->scan_mode = 2;
    }

    if (sal_strcasecmp(fields[2].u.sym_val, OFFs) == 0) {
        control_para->scan_enable = 0;
    } else if (sal_strcasecmp(fields[2].u.sym_val, EBST_STARTs) == 0) {
        control_para->scan_enable = 1;
    } else if (sal_strcasecmp(fields[2].u.sym_val, EBST_STOPs) == 0) {
        control_para->scan_enable = 2;
    } else {
        control_para->scan_enable = 3;
    }

    if (valid == TRUE) {
        control_para->scan_threshold = fields[3].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a node from ebst_info->list.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  config               config.
 * \param [in]  sync_hw              sync_hw.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_data_node_delete(int unit, ebst_config_t *config, bool sync_hw)
{
    cosq_ebst_info_t *ebst_info = COSQ_EBST_INFO(unit);
    ebst_data_list_t *current_node = NULL, *next_node = NULL;
    ebst_data_list_t *delete_node = NULL;
    ebst_data_list_t *node = NULL;
    ebst_config_t iconfig;
    bool fail_sync = FALSE;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (config->base_index == 0) {
        /* Delete node is the first node */
        current_node = ebst_info->list;
        ebst_info->list = current_node->next;
        delete_node = current_node;

        node = ebst_info->list;

        /* All nodes behind base_index move forward and update logic table */
        while (node) {
            node->base_index -= current_node->num_entries;

            iconfig.base_index = node->base_index;
            iconfig.num_entries = node->num_entries;
            if (sync_hw) {
                rv = cosq_ebst_data_table_sync(unit, node->port, node->cosq,
                                               node->bid, &iconfig);
                if (rv != SHR_E_NONE) {
                    fail_sync = TRUE;

                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            }

            node = node->next;
        }
    } else {
        node = ebst_info->list;

        /* All nodes behind base_index move forward and update logic table */
        while (node) {
            current_node = node;
            next_node = node->next;
            if (next_node == NULL) {
                /* Delete node is the latest node */
                delete_node = current_node;
                break;
            } else {
                /* Delete node is the middle node */
                if (next_node->base_index == config->base_index) {
                    current_node->next = next_node->next;
                    delete_node = next_node;

                    node = current_node->next;

                    while (node) {
                        node->base_index -= next_node->num_entries;

                        iconfig.base_index = node->base_index;
                        iconfig.num_entries = node->num_entries;
                        if (sync_hw) {
                            rv = cosq_ebst_data_table_sync(unit, node->port,
                                                           node->cosq,
                                                           node->bid,
                                                           &iconfig);
                            if (rv != SHR_E_NONE) {
                                fail_sync = TRUE;

                                SHR_IF_ERR_VERBOSE_EXIT(rv);
                            }
                        }

                        node = node->next;
                    }
                    break;
                }
            }
            node = node->next;
        }
    }

    /* Update the delete node info to logic table */
    if (delete_node) {
        ebst_info->current_ebst_data_index -= delete_node->num_entries;

        delete_node->next = NULL;
        iconfig.base_index = 0;
        iconfig.num_entries = 0;
        if (sync_hw) {
            rv = cosq_ebst_data_table_sync(unit, delete_node->port,
                                           delete_node->cosq,
                                           delete_node->bid, &iconfig);
            if (rv != SHR_E_NONE) {
                fail_sync = TRUE;

                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
        SHR_FREE(delete_node);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FREE(delete_node);

    if (fail_sync) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_info_clear(unit));
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_info_restore(unit));
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Add a node to ebst_info->list.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 port.
 * \param [in]  cosq                 cosq.
 * \param [in]  bid                  bid.
 * \param [in]  config               config.
 * \param [in]  sync_hw              sync_hw.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_data_node_add(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        bcm_bst_stat_id_t bid, ebst_config_t *config,
                        bool sync_hw)
{
    cosq_ebst_info_t *ebst_info = COSQ_EBST_INFO(unit);
    ebst_data_list_t *new_node = NULL;
    ebst_data_list_t *node = NULL;
    int free_entry = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    free_entry = MAX_EBST_DATA_ID - ebst_info->current_ebst_data_index;
    if (config->num_entries > free_entry) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_ALLOC(new_node, sizeof(ebst_data_list_t), "ebstDataList");
    SHR_NULL_CHECK(new_node, SHR_E_MEMORY);
    sal_memset(new_node, 0, sizeof(ebst_data_list_t));

    config->base_index = ebst_info->current_ebst_data_index;
    ebst_info->current_ebst_data_index =
        config->base_index + config->num_entries;

    new_node->base_index = config->base_index;
    new_node->num_entries = config->num_entries;
    new_node->port = port;
    new_node->cosq = cosq;
    new_node->bid = bid;
    new_node->next = NULL;

    if (ebst_info->list == NULL) {
        ebst_info->list = new_node;
    } else {
        node = ebst_info->list;
        while (node->next) {
            node = node->next;
        }
        node->next = new_node;
    }

    if (sync_hw) {
        rv = cosq_ebst_data_table_sync(unit, new_node->port, new_node->cosq,
                                       new_node->bid, config);
        if (rv != SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_data_node_delete(unit, config, FALSE));

            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief clear ebst_info->list and logic table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_data_info_clear(int unit)
{
    cosq_ebst_info_t *ebst_info;
    ebst_data_list_t *node;
    ebst_info = COSQ_EBST_INFO(unit);

    while (ebst_info->list) {
        node = ebst_info->list;
        ebst_info->list = ebst_info->list->next;
        SHR_FREE(node);
    }

    ebst_info->current_ebst_data_index = 0;

    return SHR_E_NONE;
}

/*
 * \brief EBST threshold field operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  key0_val             key 0 value.
 * \param [in]  key1_val             key 1 value.
 * \param [in]  flags                flags.
 * \param [in]  value                value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_threshold_field_op(int unit, cosq_ebst_handler_t *res_info,
                             int key0_val, int key1_val,
                             int flags, uint32_t *value)
{
    bcmi_lt_field_t field;
    const char *tbl_name;
    const char *key0 = NULL;
    const char *key1 = NULL;

    SHR_FUNC_ENTER(unit);

    tbl_name = res_info->profile_hdl.thd_lt;
    field.fld_name = res_info->profile_hdl.field.fld_name;
    key0 = res_info->profile_hdl.key;
    key1 = res_info->profile_hdl.key1;

    if (flags == EBST_OP_SET) {
        field.u.val = *value;
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_table_set(unit, tbl_name, key0, key0_val,
                                    key1, key1_val, NULL, 0, &field));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_table_get(unit, tbl_name, key0, key0_val,
                                    key1, key1_val, NULL, 0, &field));
        *value = field.u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT TM_EBST_PROFILEs.
 *
 * This function is used to add an entry into LT TM_EBST_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_entry_add(int unit, cosq_ebst_profile_t *profile,
                            int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_EBST_PROFILE,
                                          profile, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EBST_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EBST_PROFILE_IDs,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, START_THDs,
                               profile->start_threshold_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, STOP_THDs,
                               profile->stop_threshold_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT TM_EBST_PROFILEs.
 *
 * This function is used to get a entry from LT TM_EBST_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_entry_get(int unit, int profile_idx,
                            cosq_ebst_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EBST_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EBST_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, START_THDs,
                               &val));

    profile->start_threshold_cells = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, STOP_THDs,
                               &val));
    profile->stop_threshold_cells = val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT TM_EBST_PROFILEs.
 *
 * This function is used to delete a entry from LT TM_EBST_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_EBST_PROFILE,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EBST_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EBST_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT TM_EBST_PROFILEs.
 *
 * This function is used to calculate hash signature for
 * LT TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_EBST_PROFILEs profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_hash_cb(int unit, void *entries,
                          int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(cosq_ebst_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT TM_EBST_PROFILEs.
 *
 * This function is used to compare profile set of LT
 * TM_WRED_DROP_CURVE_SET_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              TM_EBST_PROFILEs profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_cmp_cb(int unit, void *entries,
                         int entries_per_set, int index, int *cmp)
{
    cosq_ebst_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(cosq_ebst_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_profile_entry_get(unit, index, &profile));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(cosq_ebst_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add default profile.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_default_profile_add(int unit)
{
    cosq_ebst_profile_t profile;
    int profile_idx, count;
    uint64_t threshold_max, threshold_min;
    uint32_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(cosq_ebst_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_EBST_PROFILEs,
                                       STOP_THDs,
                                       &threshold_min, &threshold_max));
    profile.start_threshold_cells = threshold_max;
    profile.stop_threshold_cells = threshold_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_profile_entry_add(unit, &profile, &profile_idx));

    count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_THD_UC_Qs, &alloc_size));

    count += alloc_size;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_THD_MC_Qs, &alloc_size));

    count += alloc_size;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_EGR_THD_UC_PORT_SERVICE_POOLs, &alloc_size));

    count += alloc_size;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TM_EGR_THD_MC_PORT_SERVICE_POOLs, &alloc_size));

    count += alloc_size;

    if (count > 1) {
        /* increment reference count if more than one ref_count */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit, BCMI_LTSW_PROFILE_EBST_PROFILE,
                                                  1, profile_idx, (count - 1)));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Register LT TM_EBST_PROFILEs.
 *
 * This function is used to register LT TM_EBST_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EBST_PROFILE;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_EBST_PROFILEs,
                                       TM_EBST_PROFILE_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_ebst_profile_hash_cb,
                                    cosq_ebst_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover hash and reference count of LT TM_EBST_PROFILEs.
 *
 * This function is used to Recover hash and reference count of
 * LT TM_EBST_PROFILEs profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EBST_PROFILE;
    cosq_ebst_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(cosq_ebst_profile_t));

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_entry_get(unit, index, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &profile,
                                           entries_per_set, profile_id));

        if (profile_id == DEFAULT_PROFILE_ID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, 1));
        }
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT TM_EBST_PROFILEs.
 *
 * This function is used to Recover hash and reference count of
 * LT TM_EBST_PROFILEs profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_recover(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EGR_THD_MC_PORT_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_EBST_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EGR_THD_UC_PORT_SERVICE_POOLs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_EBST_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_MC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_EBST_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_UC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_EBST_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT TM_EBST_PROFILEs.
 *
 * This function is used to initialize the LT TM_EBST_PROFILEs
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EBST_PROFILE;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_EBST_PROFILEs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();

}

/*
 * \brief EBST threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_ebst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  index                key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_profile_op(int unit, void *info,
                     int local_port, int index,
                     int flags, cosq_ebst_profile_t *profile)
{
    int old_profile_id, profile_id;
    cosq_ebst_handler_t *res_info = (cosq_ebst_handler_t *)info;
    cosq_ebst_profile_t iprofile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_threshold_field_op(unit, res_info, local_port, index,
                                      EBST_OP_GET,
                                      (uint32_t *)&old_profile_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_profile_entry_get(unit, old_profile_id, &iprofile));

    if (flags == EBST_OP_SET) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_entry_add(unit, profile, &profile_id));
        SHR_IF_ERR_CONT
            (cosq_ebst_threshold_field_op(unit, res_info, local_port, index,
                                          EBST_OP_SET,
                                          (uint32_t *)&profile_id));
        if (SHR_FUNC_ERR()) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_profile_entry_delete(unit, profile_id));

            SHR_EXIT();
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_profile_entry_delete(unit, old_profile_id));
    } else {
        profile->start_threshold_cells = iprofile.start_threshold_cells;
        profile->stop_threshold_cells = iprofile.stop_threshold_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Init ebst handler.
 */
static cosq_ebst_handler_t
ebst_handler[bcmBstStatIdMaxCount] =
{
    [bcmBstStatIdUcast] = {{TM_THD_UC_Qs,                  /* Threshold LT */
                            PORT_IDs,                      /* Port Key */
                            TM_UC_Q_IDs,                   /* Key1 */
                            {TM_EBST_PROFILE_IDs},
                            cosq_ebst_profile_op,
                           },
                           {TM_EBST_UC_Qs,                  /* Stat LT */
                            PORT_IDs,                      /* Port Key */
                            TM_UC_Q_IDs,                   /* Key1 */
                            cosq_tm_ebst_data_config_set,
                            cosq_tm_ebst_data_config_get,
                           },
                          },
    [bcmBstStatIdMcast] = {{TM_THD_MC_Qs,
                            PORT_IDs,
                            TM_MC_Q_IDs,
                            {TM_EBST_PROFILE_IDs},
                            cosq_ebst_profile_op,
                           },
                           {TM_EBST_MC_Qs,
                            PORT_IDs,
                            TM_MC_Q_IDs,
                            cosq_tm_ebst_data_config_set,
                            cosq_tm_ebst_data_config_get,
                           },
                          },
    [bcmBstStatIdEgrPortPoolSharedUcast] = {
                            {TM_EGR_THD_UC_PORT_SERVICE_POOLs,
                             PORT_IDs,
                             TM_EGR_SERVICE_POOL_IDs,
                             {TM_EBST_PROFILE_IDs},
                             cosq_ebst_profile_op,
                             },
                            {TM_EBST_PORT_SERVICE_POOLs,
                             PORT_IDs,
                             TM_EGR_SERVICE_POOL_IDs,
                             cosq_tm_ebst_uc_pool_data_config_set,
                             cosq_tm_ebst_uc_pool_data_config_get,
                            },
                            },
    [bcmBstStatIdEgrPortPoolSharedMcast] = {
                            {TM_EGR_THD_MC_PORT_SERVICE_POOLs,
                             PORT_IDs,
                             TM_EGR_SERVICE_POOL_IDs,
                             {TM_EBST_PROFILE_IDs},
                             cosq_ebst_profile_op,
                            },
                            {TM_EBST_PORT_SERVICE_POOLs,
                             PORT_IDs,
                             TM_EGR_SERVICE_POOL_IDs,
                             cosq_tm_ebst_mc_pool_data_config_set,
                             cosq_tm_ebst_mc_pool_data_config_get,
                            },
                            },
    [bcmBstStatIdEgrPool] = {
                            {NULL,
                             NULL,
                             NULL,
                             {NULL},
                             NULL,
                            },
                            {TM_EBST_SERVICE_POOLs,
                             NULL,
                             TM_EGR_SERVICE_POOL_IDs,
                            cosq_tm_ebst_data_config_set,
                            cosq_tm_ebst_data_config_get,
                            },
                            },
};

/*
 * \brief restore ebst data prameter per bid.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_data_info_bid_restore(int unit, bcm_bst_stat_id_t bid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit;
    uint64_t data;
    int port = -1, cosq = -1;
    ebst_config_t iconfig;
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, res_info->stat_hdl.thd_lt, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, res_info->stat_hdl.key,
                                   &data));

        port = data;

        if (res_info->stat_hdl.key1 != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, res_info->stat_hdl.key1,
                                       &data));
            cosq = data;
        } else {
            cosq = -1;
        }



        switch (bid) {
            case bcmBstStatIdUcast:
            case bcmBstStatIdMcast:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, BASE_INDEXs,
                                           &data));
                iconfig.base_index = data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, NUM_ENTRIESs,
                                           &data));
                iconfig.num_entries = data;
                break;
            case bcmBstStatIdEgrPortPoolSharedUcast:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, UC_BASE_INDEXs,
                                           &data));
                iconfig.base_index = data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, UC_NUM_ENTRIESs,
                                           &data));
                iconfig.num_entries = data;
                break;
            case bcmBstStatIdEgrPortPoolSharedMcast:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, MC_BASE_INDEXs,
                                           &data));
                iconfig.base_index = data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, MC_NUM_ENTRIESs,
                                           &data));
                iconfig.num_entries = data;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (iconfig.num_entries != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_data_node_add(unit, port, cosq, bid, &iconfig, FALSE));
        }
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief restore ebst data info.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_data_info_restore(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_data_info_bid_restore(unit, bcmBstStatIdEgrPortPoolSharedUcast));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_data_info_bid_restore(unit, bcmBstStatIdEgrPortPoolSharedMcast));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_data_info_bid_restore(unit, bcmBstStatIdUcast));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_data_info_bid_restore(unit, bcmBstStatIdMcast));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief sync data parameter to hw.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 port.
 * \param [in]  cosq                 cosq.
 * \param [in]  bid                  bid.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_data_table_sync(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, ebst_config_t *config)
{
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    ebst_config_t iconfig;

    SHR_FUNC_ENTER(unit);

    iconfig.monitor = -1;
    iconfig.base_index = config->base_index;
    iconfig.num_entries = config->num_entries;

    if (res_info->stat_hdl.data_config_set != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (res_info->stat_hdl.data_config_set(unit, res_info->stat_hdl.thd_lt,
                                                res_info->stat_hdl.key, port,
                                                res_info->stat_hdl.key1, cosq,
                                                &iconfig));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief EBST data section add and delete.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  port                 port.
 * \param [in]  cosq                 cosq.
 * \param [in]  bid                  bid.
 * \param [in]  config               config.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_ebst_data_section_op(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, ebst_config_t *config)
{
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    ebst_config_t iconfig = {0};

    SHR_FUNC_ENTER(unit);

    if (res_info->stat_hdl.data_config_get != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (res_info->stat_hdl.data_config_get(unit, res_info->stat_hdl.thd_lt,
                                                res_info->stat_hdl.key, port,
                                                res_info->stat_hdl.key1, cosq,
                                                &iconfig));
    }

    if (config->num_entries == 0) {
        /* Delete section */
        if (iconfig.num_entries == 0) {
            SHR_EXIT();
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_node_delete(unit, &iconfig, TRUE));
    } else if ((config->num_entries != iconfig.num_entries) &&
               (iconfig.num_entries != 0)) {
        /* Move section */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_node_delete(unit, &iconfig, TRUE));

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_node_add(unit, port, cosq, bid, config, TRUE));
    } else {
        if (config->num_entries == iconfig.num_entries) {
            SHR_EXIT();
        }
        /* Add section */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_node_add(unit, port, cosq, bid, config, TRUE));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set the EBST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   object_id    object id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   profile      profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_profile_set(int unit,
                      bcm_cosq_object_id_t *object_id,
                      bcm_bst_stat_id_t bid,
                      cosq_ebst_profile_t *profile)
{
    bcm_port_t local_port;
    int index, start_index, end_index;
    uint64_t threshold_max, threshold_min;
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_EBST_PROFILEs,
                                       STOP_THDs,
                                       &threshold_min, &threshold_max));

    if (profile->start_threshold_cells > threshold_max ||
        profile->start_threshold_cells < threshold_min ||
        profile->stop_threshold_cells > threshold_max ||
        profile->stop_threshold_cells < threshold_min) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_ebst_index_resolve(unit, object_id->port, object_id->cosq, bid,
                                      &local_port, &start_index, &end_index));

    for (index = start_index; index <= end_index; index++ ) {
        if (res_info->profile_hdl.profile_op != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (res_info->profile_hdl.profile_op(unit, res_info, local_port,
                                                  index,EBST_OP_SET, profile));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the EBST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   object_id    object id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  profile      profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_profile_get(int unit,
                      bcm_cosq_object_id_t *object_id,
                      bcm_bst_stat_id_t bid,
                      cosq_ebst_profile_t *profile)
{
    bcm_port_t local_port;
    int start_index, end_index;
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_ebst_index_resolve(unit, object_id->port, object_id->cosq, bid,
                                      &local_port, &start_index, &end_index));

    if (res_info->profile_hdl.profile_op != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (res_info->profile_hdl.profile_op(unit, res_info, local_port,
                                              start_index, EBST_OP_GET, profile));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set the EBST config.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   object_id    object id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   config       config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_config_set(int unit,
                     bcm_cosq_object_id_t *object_id,
                     bcm_bst_stat_id_t bid,
                     ebst_config_t *config)
{
    bcm_port_t local_port;
    int index, start_index, end_index;
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_ebst_index_resolve(unit, object_id->port, object_id->cosq, bid,
                                      &local_port, &start_index, &end_index));

    for (index = start_index; index <= end_index; index++ ) {
        if (config->num_entries != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_data_section_op(unit, local_port, index, bid, config));
        }

        if (config->monitor != -1) {
            if (res_info->stat_hdl.data_config_set != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (res_info->stat_hdl.data_config_set(unit, res_info->stat_hdl.thd_lt,
                                                        res_info->stat_hdl.key, local_port,
                                                        res_info->stat_hdl.key1, index,
                                                        config));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the EBST config.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   object_id    object id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  config       config.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_ebst_config_get(int unit,
                     bcm_cosq_object_id_t *object_id,
                     bcm_bst_stat_id_t bid,
                     ebst_config_t *config)
{
    bcm_port_t local_port;
    int start_index, end_index;
    cosq_ebst_handler_t *res_info = &ebst_handler[bid];
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_ebst_index_resolve(unit, object_id->port, object_id->cosq, bid,
                                      &local_port, &start_index, &end_index));

    if (res_info->stat_hdl.data_config_get != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (res_info->stat_hdl.data_config_get(unit, res_info->stat_hdl.thd_lt,
                                                res_info->stat_hdl.key, local_port,
                                                res_info->stat_hdl.key1, start_index,
                                                config));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief The callback function to handle EBST event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_ebst_notification_cb(int unit, const char *event, void *notif_info,
                          void *user_data)
{
    cosq_ebst_event_notif_t *ebst_event = (cosq_ebst_event_notif_t *)notif_info;
    int i = 0;

    if (ebst_event && ebst_event->ebst_mode == EBST_SCAN_FIFO_FULL) {
        for (i = 0; i < MAX_ITM_PER_DEV; i++) {
            if (ebst_event->itm_fifo_full[i]) {
                (void)bcmi_ltsw_switch_event_generate(unit,
                                                      BCM_SWITCH_EVENT_MMU_EBST_FIFO_FULL,
                                                      i, -1, -1);
            }
        }
    }

    return;
}

/*!
 * \brief Parse EBST event table entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       Table name.
 * \param [in]  eh            LT entry handle.
 * \param [out] notif_info    Pointer to notification info.
 * \param [out] status        Parser callback returned status, NULL for not care.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static void
cosq_ebst_event_parse(int unit, const char *lt_name,
                      uint32_t entry_hdl, void *notif_info,
                      bcmi_ltsw_event_status_t *status)
{
    bcmlt_entry_handle_t eh = entry_hdl;
    cosq_ebst_event_notif_t *ebst_event = (cosq_ebst_event_notif_t *)notif_info;
    union {
        /* scalar data. */
        uint64_t    array[2];
        /* symbol data. */
        const char  *sym_val;
    } fval;
    uint32_t r_elem_cnt;
    int rv;

    rv = bcmlt_entry_field_symbol_get(eh, EBSTs, &(fval.sym_val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, EBSTs, rv));
        return;
    }

    if (sal_strcasecmp(fval.sym_val, OFFs) == 0) {
        ebst_event->ebst_mode = 0;
    } else if (sal_strcasecmp(fval.sym_val, EBST_STARTs) == 0) {
        ebst_event->ebst_mode = 1;
    } else if (sal_strcasecmp(fval.sym_val, EBST_STOPs) == 0) {
        ebst_event->ebst_mode = 2;
    } else {
        ebst_event->ebst_mode = 3;
    }

    rv = bcmlt_entry_field_array_get(eh, FIFO_FULLs, 0, fval.array,
                                     2, &r_elem_cnt);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, FIFO_FULLs, rv));
        return;
    }
    ebst_event->itm_fifo_full[0] = fval.array[0];
    ebst_event->itm_fifo_full[1] = fval.array[1];
}

/*!
 * \brief Subscribe for updates of TM_EBST_STATUSs table.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback to be invoked.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
cosq_ebst_moniter_enable(int unit, bool enable)
{
    SHR_FUNC_ENTER(unit);

    if (enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_parser_set(unit, TM_EBST_STATUSs,
                                      cosq_ebst_event_parse,
                                      sizeof(cosq_ebst_event_notif_t)));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_attrib_set(unit, TM_EBST_STATUSs,
                                      BCMI_LTSW_EVENT_HIGH_PRIORITY));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_subscribe(unit, TM_EBST_STATUSs,
                                     cosq_ebst_notification_cb, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_unsubscribe(unit, TM_EBST_STATUSs,
                                       cosq_ebst_notification_cb));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_ebst_control_para_init(tm_cosq_ebst_control_para_t *control_para)
{
    control_para->scan_enable = -1;
    control_para->scan_mode = -1;
    control_para->scan_round = -1;
    control_para->scan_threshold = -1;

    return SHR_E_NONE;
}

static int
cosq_ebst_status_init(tm_cosq_ebst_status_t *ebst_status)
{
    ebst_status->scan_enable = -1;
    return SHR_E_NONE;
}

/******************************************************************************
 * Public functions
 */

int
tm_ltsw_ebst_chip_driver_register(int unit, tm_ltsw_ebst_chip_driver_t *drv)
{
    tm_ebst_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_ebst_chip_driver_deregister(int unit)
{
    tm_ebst_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_ebst_chip_driver_get(int unit, tm_ltsw_ebst_chip_driver_t **drv)
{
    *drv = tm_ebst_chip_driver_cb[unit];

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_ebst_threshold_profile_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile)
{
    cosq_ebst_profile_t iprofile;
    tm_ltsw_ebst_chip_driver_t *drv = NULL;
    int bytes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_bid_gport_cosq_param_check, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
       (drv->cosq_ebst_bid_gport_cosq_param_check(unit, object_id, bid));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_profile_get(unit, object_id, bid, &iprofile));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_cosq_thd_cell_to_byte(unit, iprofile.start_threshold_cells, &bytes));

    profile->start_bytes = bytes;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_cosq_thd_cell_to_byte(unit, iprofile.stop_threshold_cells, &bytes));

    profile->stop_bytes = bytes;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_threshold_profile_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile)
{
    cosq_ebst_profile_t iprofile;
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_bid_gport_cosq_param_check, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
       (drv->cosq_ebst_bid_gport_cosq_param_check(unit, object_id, bid));

    /* No profile needed for egress pool, as its EBST info will always be reported whenever
       there is EBST of any one egress queue or port is enabled */
    if (bid == bcmBstStatIdEgrPool) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_byte_to_cell(unit, profile->start_bytes,
                                         &(iprofile.start_threshold_cells)));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_byte_to_cell(unit, profile->stop_bytes,
                                         &(iprofile.stop_threshold_cells)));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_profile_set(unit, object_id, bid, &iprofile));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_control_get(
    int unit,
    bcm_cosq_ebst_control_t type,
    int *arg)
{
    cosq_ebst_info_t *ebst_info;
    tm_cosq_ebst_control_para_t control_para;
    bool valid = TRUE;

    SHR_FUNC_ENTER(unit);

    ebst_info = COSQ_EBST_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_control_para_init(&control_para));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_tm_ebst_control_get(unit, &control_para));

    switch (type) {
        case bcmCosqEbstControlScanEnable:
            if (control_para.scan_enable == EBST_SCAN_START) {
                *arg = 1;
            } else {
                *arg = 0;
            }
            break;
        case bcmCosqEbstControlScanMode:
            *arg = control_para.scan_mode;
            break;
        case bcmCosqEbstControlScanInterval:
            *arg = *ebst_info->scan_interval;
            break;
        case bcmCosqEbstControlScanThreshold:
            /* Check whether field SCAN_THD is valid. */
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_field_validation(unit, TM_EBST_CONTROLs,
                                            SCAN_THDs, &valid));
            if (valid == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }

            *arg = control_para.scan_threshold;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_control_set(
    int unit,
    bcm_cosq_ebst_control_t type,
    int arg)
{
    cosq_ebst_info_t *ebst_info;
    int core_clk;
    tm_cosq_ebst_control_para_t control_para, control;
    tm_cosq_ebst_status_t ebst_status;
    bool valid = TRUE;

    SHR_FUNC_ENTER(unit);

    ebst_info = COSQ_EBST_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_control_para_init(&control_para));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_tm_ebst_control_get(unit, &control));

    switch (type) {
        case bcmCosqEbstControlScanEnable:
            control_para.scan_enable = arg ? EBST_SCAN_START : EBST_SCAN_STOP;
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_control_set(unit, &control_para));
            break;
        case bcmCosqEbstControlScanMode:
            if (arg < bcmCosqEbstScanModeQueue &&
                arg >= bcmCosqEbstScanModeCount) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (control.scan_enable == EBST_SCAN_START) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
            }
            control_para.scan_mode = arg;
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_control_set(unit, &control_para));

            break;
        case bcmCosqEbstControlScanInterval:
            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
               (cosq_ebst_status_init(&ebst_status));

            SHR_IF_ERR_VERBOSE_EXIT
               (cosq_tm_ebst_status_get(unit, &ebst_status));

            if (control.scan_enable == EBST_SCAN_START) {
                if (ebst_status.scan_enable != EBST_SCAN_FIFO_FULL) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
                }
            }

            *ebst_info->scan_interval = arg;

            core_clk = bcmi_ltsw_dev_core_clk_freq(unit);

            /* scan round(cycles) = us * cycles/us */
            control_para.scan_round = arg * core_clk;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_control_set(unit, &control_para));
            break;
        case bcmCosqEbstControlScanThreshold:
            /* Check whether field SCAN_THD is valid. */
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_field_validation(unit, TM_EBST_CONTROLs,
                                            SCAN_THDs, &valid));
            if (valid == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }

            if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            if (control.scan_enable == EBST_SCAN_START) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
            }

            control_para.scan_threshold = arg;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_control_set(unit, &control_para));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_monitor_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor)
{
    ebst_config_t iconfig;
    tm_ltsw_ebst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_bid_gport_cosq_param_check, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
       (drv->cosq_ebst_bid_gport_cosq_param_check(unit, object_id, bid));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_config_get(unit, object_id, bid, &iconfig));

    monitor->entry_num = iconfig.num_entries;
    monitor->enable = iconfig.monitor;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_monitor_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor)
{
    bcm_port_t local_port;
    int start_index, end_index;
    cosq_ebst_info_t *ebst_info = COSQ_EBST_INFO(unit);
    int free_entry, num_entries;
    ebst_config_t iconfig;
    tm_ltsw_ebst_chip_driver_t *drv = NULL;
    tm_cosq_ebst_control_para_t control_para;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (tm_ltsw_ebst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_ebst_bid_gport_cosq_param_check, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
       (drv->cosq_ebst_bid_gport_cosq_param_check(unit, object_id, bid));

    iconfig.monitor = -1;
    iconfig.base_index = -1;
    iconfig.num_entries = -1;

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_control_para_init(&control_para));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_tm_ebst_control_get(unit, &control_para));

    if (monitor->flags & BCM_COSQ_EBST_MONITOR_ENTRY_NUM_VALID) {
        if (control_para.scan_enable) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        } else {
            if (monitor->entry_num < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            SHR_NULL_CHECK(drv->cosq_ebst_index_resolve, SHR_E_INIT);
            SHR_IF_ERR_VERBOSE_EXIT
                (drv->cosq_ebst_index_resolve(unit, object_id->port, object_id->cosq, bid,
                                              &local_port, &start_index, &end_index));
            free_entry = MAX_EBST_DATA_ID - ebst_info->current_ebst_data_index;
            num_entries = monitor->entry_num * (end_index - start_index + 1);
            if (num_entries > free_entry) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            iconfig.num_entries = monitor->entry_num;
        }
    }

    if (monitor->flags & BCM_COSQ_EBST_MONITOR_ENABLE_VALID) {
        iconfig.monitor = monitor->enable;
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_config_set(unit, object_id, bid, &iconfig));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_data_stat_info_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_cosq_ebst_data_stat_info_t *info)
{
    cosq_ebst_info_t *ebst_info = COSQ_EBST_INFO(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    info->entry_max = MAX_EBST_DATA_ID;
    info->entry_free = MAX_EBST_DATA_ID - ebst_info->current_ebst_data_index;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_cosq_ebst_data_entry_t *entry_array,
    int *count)
{
    int i = 0, itm;
    int data_id, max_index, base_index = -1;
    ebst_config_t iconfig;
    tm_cosq_ebst_data_entry_t entry_data;
    int bytes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry_array, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    /* Only retrieve EBST data of a dedicated buffer id and cosq */
    if (object_id->buffer < 0 || object_id->buffer > 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (object_id->cosq == BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_config_get(unit, object_id, bid, &iconfig));

    if ((array_size > iconfig.num_entries) || (array_size <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    itm = object_id->buffer;

    if ((iconfig.data_id[itm] + 1) <
        (iconfig.base_index + iconfig.num_entries)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_data_get(unit, itm, iconfig.data_id[itm] + 1,
                                   &entry_data));
        if (entry_data.timestamp!= 0) {
            /* Data entries rollover */
            base_index = iconfig.data_id[itm] + 1;
        }
    }

    data_id = base_index + i;
    max_index = iconfig.base_index + iconfig.num_entries;

    /* Data entries rollover: Read data entries (iconfig.data_id[itm]+1 ... max_index) */
    if (base_index != -1) {
        while (data_id < max_index) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_data_get(unit, itm, data_id,
                                       &entry_data));
            if ((i < array_size) && (entry_data.timestamp != 0)) {
                entry_array[i].timestamp = entry_data.timestamp;
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_ltsw_cosq_thd_cell_to_byte(unit, entry_data.cells, &bytes));
                entry_array[i].bytes = bytes;
                entry_array[i].green_drop_state = entry_data.green_drop_state;
                entry_array[i].yellow_drop_state = entry_data.yellow_drop_state;
                entry_array[i].red_drop_state = entry_data.red_drop_state;
                i++;
            }
            data_id++;
        }
    }

    data_id = iconfig.base_index;

    /* No mater entries rollover or not: Read data entries (base_index ... iconfig.data_id[itm])*/
    while (data_id <= iconfig.data_id[itm]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_data_get(unit, itm, data_id,
                                   &entry_data));
        if ((i < array_size) && (entry_data.timestamp != 0)) {
            entry_array[i].timestamp = entry_data.timestamp;
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmi_ltsw_cosq_thd_cell_to_byte(unit, entry_data.cells, &bytes));
            entry_array[i].bytes = bytes;
            entry_array[i].green_drop_state = entry_data.green_drop_state;
            entry_array[i].yellow_drop_state = entry_data.yellow_drop_state;
            entry_array[i].red_drop_state = entry_data.red_drop_state;
            i++;
        }
        data_id++;
    }

    *count = i;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_enable_set(
    int unit,
    int enable)
{
    cosq_ebst_info_t *ebst_info;
    int tracking_enable = 0;
    tm_cosq_ebst_control_para_t control_para;

    SHR_FUNC_ENTER(unit);

    ebst_info = COSQ_EBST_INFO(unit);

    if (enable < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    tracking_enable = enable ? 1 : 0;

    SHR_NULL_CHECK(ebst_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_ebst_control_para_init(&control_para));

    if (tracking_enable) {
        if (!(*ebst_info->tracking_enable)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_moniter_enable(unit, TRUE));

            control_para.scan_enable = EBST_SCAN_START;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_tm_ebst_control_set(unit, &control_para));
        }
    } else {
        control_para.scan_enable = EBST_SCAN_OFF;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ebst_control_set(unit, &control_para));

        if (*ebst_info->tracking_enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_moniter_enable(unit, FALSE));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_EBST_DATAs));
    }

    *ebst_info->tracking_enable = tracking_enable;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_enable_get(
    int unit,
    int *enable)
{
    cosq_ebst_info_t *ebst_info = COSQ_EBST_INFO(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_NULL_CHECK(ebst_info, SHR_E_INTERNAL);

    *enable = *ebst_info->tracking_enable;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_init(int unit)
{
    cosq_ebst_info_t *ebst_info;
    uint32_t ha_alloc_size, ha_req_size;
    int warm;

    SHR_FUNC_ENTER(unit);

    ebst_info = COSQ_EBST_INFO(unit);
    warm = bcmi_warmboot_get(unit);

    ha_req_size = sizeof(int);
    ha_alloc_size = ha_req_size;

    ebst_info->tracking_enable = (int *)bcmi_ltsw_ha_mem_alloc
                                       (unit, BCMI_HA_COMP_ID_COSQ,
                                        BCMINT_COSQ_EBST_ENABLE_SUB_COMP_ID,
                                        "bcmCosqEbstEn",
                                        &ha_alloc_size);
    SHR_NULL_CHECK(ebst_info->tracking_enable, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    ebst_info->scan_interval = (int *)bcmi_ltsw_ha_mem_alloc
                                      (unit, BCMI_HA_COMP_ID_COSQ,
                                       BCMINT_COSQ_EBST_SCAN_INTERVAL_SUB_COMP_ID,
                                       "bcmCosqEbstScanInt",
                                       &ha_alloc_size);
    SHR_NULL_CHECK(ebst_info->scan_interval, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(ebst_info->tracking_enable, 0, ha_alloc_size);
        sal_memset(ebst_info->scan_interval, 0, ha_alloc_size);
    } else {
        if (*ebst_info->tracking_enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_ebst_moniter_enable(unit, TRUE));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_ebst_data_info_restore(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_ebst_profile_init(unit));

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm && ebst_info->tracking_enable) {
            (void)bcmi_ltsw_ha_mem_free(unit, ebst_info->tracking_enable);
            ebst_info->tracking_enable = NULL;
        }
        if (!warm && ebst_info->scan_interval) {
            (void)bcmi_ltsw_ha_mem_free(unit, ebst_info->scan_interval);
            ebst_info->scan_interval = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_ebst_detach(int unit)
{
    cosq_ebst_info_t *ebst_info;
    int warm = bcmi_warmboot_get(unit);
    ebst_info = COSQ_EBST_INFO(unit);

    (void)cosq_ebst_data_info_clear(unit);

    (void)bcmi_lt_clear(unit, TM_EBST_DATAs);

    if (!warm) {
        (void)bcmi_lt_clear(unit, TM_EBST_PORTs);

        (void)bcmi_lt_clear(unit, TM_EBST_UC_Qs);

        (void)bcmi_lt_clear(unit, TM_EBST_MC_Qs);
    }

    if (*ebst_info->tracking_enable) {
        (void)cosq_ebst_moniter_enable(unit, FALSE);
    }

    sal_memset(ebst_info, 0, sizeof(cosq_ebst_info_t));

    return SHR_E_NONE;
}

