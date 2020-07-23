/*! \file bcmcth_mon_telmetry_drv.c
 *
 * BCMCTH Monitor Telemetry Driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmcth/generated/mon_telemetry_ha.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/mcs_shared/mcs_msg_common.h>
#include <bcmbd/mcs_shared/mcs_mon_telemetry.h>
#include <shr/shr_util_pack.h>
#include <bcmcth/bcmcth_mon_telemetry_util.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmcth/bcmcth_mon_collector.h>
#include <bcmissu/issu_api.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Private variables
 */
#define VLAN_SINGLE_TAGGED 0x1
#define VLAN_DOUBLE_TAGGED 0x2

#define BCMMON_TELEMETRY_DEF_ID 0xFFFFFFFF

static uint32_t st_firmware_version = BCMMON_TELEMETRY_ST_UC_MIN_VERSION;
bcmcth_mon_telemetry_info_t *g_telemetry_info[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/*! Macro for ST feature check. */
#define BCMMON_TELEMETRY_ST_UC_FEATURE_CHECK(feature)  \
    (st_firmware_version & (1 << feature))

/*******************************************************************************
 * Local definitions
 */

static int
bcmcth_mon_telemetry_convert_uc_error(
    uint32_t uc_rv)
{
    int rv = SHR_E_NONE;

    switch (uc_rv) {
    case MCS_SHR_ST_UC_E_NONE:
        rv = SHR_E_NONE;
        break;
    case MCS_SHR_ST_UC_E_INTERNAL:
        rv = SHR_E_INTERNAL;
        break;
    case MCS_SHR_ST_UC_E_MEMORY:
        rv = SHR_E_MEMORY;
        break;
    case MCS_SHR_ST_UC_E_UNIT:
        rv = SHR_E_UNIT;
        break;
    case MCS_SHR_ST_UC_E_PARAM:
        rv = SHR_E_PARAM;
        break;
    case MCS_SHR_ST_UC_E_EMPTY:
        rv = SHR_E_EMPTY;
        break;
    case MCS_SHR_ST_UC_E_FULL:
        rv = SHR_E_FULL;
        break;
    case MCS_SHR_ST_UC_E_NOT_FOUND:
        rv = SHR_E_NOT_FOUND;
        break;
    case MCS_SHR_ST_UC_E_EXISTS:
        rv = SHR_E_EXISTS;
        break;
    case MCS_SHR_ST_UC_E_TIMEOUT:
        rv = SHR_E_TIMEOUT;
        break;
    case MCS_SHR_ST_UC_E_BUSY:
        rv = SHR_E_BUSY;
        break;
    case MCS_SHR_ST_UC_E_FAIL:
        rv = SHR_E_FAIL;
        break;
    case MCS_SHR_ST_UC_E_DISABLED:
        rv = SHR_E_DISABLED;
        break;
    case MCS_SHR_ST_UC_E_BADID:
        rv = SHR_E_BADID;
        break;
    case MCS_SHR_ST_UC_E_RESOURCE:
        rv = SHR_E_RESOURCE;
        break;
    case MCS_SHR_ST_UC_E_CONFIG:
        rv = SHR_E_CONFIG;
        break;
    case MCS_SHR_ST_UC_E_UNAVAIL:
        rv = SHR_E_UNAVAIL;
        break;
    case MCS_SHR_ST_UC_E_INIT:
        rv = SHR_E_INIT;
        break;
    case MCS_SHR_ST_UC_E_PORT:
        rv = SHR_E_PORT;
        break;
    default:
        rv = SHR_E_INTERNAL;
        break;
    }

    return rv;
}

static int
bcmcth_mon_telemetry_uc_appl_init(int unit, int uc_num)
{
    mcs_msg_data_t      send, rcv;
    int                 rc;
    sal_usecs_t timeout;
    uint32_t min_appl_version =  BCMMON_TELEMETRY_ST_UC_MIN_VERSION;


    SHR_FUNC_ENTER(unit);
    timeout = MCS_SHR_ST_MAX_UC_MSG_TIMEOUT;
    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send,MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send,MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT);
    MCS_MSG_LEN_SET(send,MCS_MSG_CLASS_ST);
    MCS_MSG_DATA_SET(send,BCMMON_TELEMETRY_ST_SDK_VERSION);

    rc = bcmbd_mcs_msg_send(unit, uc_num, &send, timeout);
    if (rc != SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_send failed\n"),
                    uc_num));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    rc = bcmbd_mcs_msg_receive(unit, uc_num,
            MCS_MSG_CLASS_ST, &rcv, timeout);

    if (rc != SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_receive failed\n"),
                    uc_num));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (MCS_MSG_LEN_GET(rcv) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_receive"
                    " length != 0\n"), uc_num));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (MCS_MSG_DATA_GET(rcv) < min_appl_version) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "UC%d appl version\n"),
                    uc_num));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    st_firmware_version = MCS_MSG_DATA_GET(rcv);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_send_receive(
    int unit, int uc_num, uint8_t s_subclass,
    uint16_t s_len, uint64_t s_data,
    uint8_t r_subclass, uint16_t *r_len,
    sal_usecs_t timeout)
{
    int rv;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    mcs_msg_data_t send, reply;
    uint32_t uc_rv;
    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    MCS_MSG_MCLASS_SET(send,MCS_MSG_CLASS_ST);
    MCS_MSG_SUBCLASS_SET(send,s_subclass);
    MCS_MSG_LEN_SET(send,s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MCS_MSG_DMA_MSG(s_subclass) ||
            MCS_MSG_DMA_MSG(r_subclass)) {
        MCS_MSG_DATA_SET(send, ((uint32_t)(telemetry_info->dma_buffer &
            0xFFFFFFFF)));
        MCS_MSG_DATA1_SET(send, ((uint32_t)((telemetry_info->dma_buffer >> 32) &
            0xFFFFFFFF)));

    } else {
        MCS_MSG_DATA_SET(send, ((uint32_t)(s_data &
            0xFFFFFFFF)));
        MCS_MSG_DATA1_SET(send, ((uint32_t)((s_data >> 32) &
            0xFFFFFFFF)));
    }

    rv = bcmbd_mcs_msg_send_receive(unit, uc_num,
                                      &send, &reply,
                                      timeout);

    if (rv != SHR_E_NONE){
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Convert BFD uController error code to BCM */
    uc_rv  = MCS_MSG_DATA_GET(reply);
    rv     = bcmcth_mon_telemetry_convert_uc_error(uc_rv);

    *r_len = MCS_MSG_LEN_GET(reply);

    /*Check reply class and subclass*/
    if ((rv == SHR_E_NONE) &&
       ((MCS_MSG_MCLASS_GET(reply) != MCS_MSG_CLASS_ST) ||
       (MCS_MSG_SUBCLASS_GET(reply) != r_subclass)))
    {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcmcth_mon_telemetry_instance_out_fields_count(
    int unit,
    uint32_t *count)
{
    uint32_t    object_fid = 0;
    uint32_t    max = 0;
    bcmlrd_field_def_t  def;

    SHR_FUNC_ENTER(unit);

    object_fid = MON_TELEMETRY_OBJECTt_MON_TELEMETRY_OBJECT_IDf;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, MON_TELEMETRY_OBJECTt, object_fid,
                &def));
    max = TELEMETRY_FIELD_MAX(def);
    /* Total possible object ID count plus Instance fields
     * count excluding key
     */
    *count = (max + MON_TELEMETRY_INSTANCEt_FIELD_COUNT - 1);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_telemetry_sw_resources_free(
    int unit)
{
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);

    if (telemetry_info) {
        /*Free the DMA buffer allocated. */
        if (telemetry_info->dma_buffer) {
            bcmdrd_hal_dma_free(unit,
                    telemetry_info->dma_buffer_len,
                    telemetry_info->dma_logical_buffer,
                    telemetry_info->dma_buffer);
        }
        SHR_FREE(telemetry_info);
        MON_TELEMETRY_INFO(unit) = NULL;
    }
    return SHR_E_NONE;
}

int
bcmcth_mon_telemetry_sw_resources_alloc(
    int unit, bool warm)
{
    bcmcth_mon_telemetry_info_t *telemetry_info = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    bool is_sim = FALSE;
    int i;
    SHR_FUNC_ENTER(unit);

    /*Alloc the global Telemetry info SW structure for this unit */
    SHR_ALLOC(telemetry_info, sizeof(bcmcth_mon_telemetry_info_t),
              "bcmcthMonTelemetryInfo");
    sal_memset(telemetry_info, 0,
               sizeof(bcmcth_mon_telemetry_info_t));
    MON_TELEMETRY_INFO(unit) = telemetry_info;

    ha_req_size = sizeof(bcmcth_mon_telemetry_ha_t);
    ha_alloc_size = ha_req_size;
    telemetry_info->ha_mem = shr_ha_mem_alloc(unit,
            BCMMGMT_MON_COMP_ID,
            BCMMON_TELEMETRY_SUB_COMP_ID,
            "telemetryHaCtrl",
            &ha_alloc_size);
    SHR_NULL_CHECK(telemetry_info->ha_mem, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(telemetry_info->ha_mem, 0, ha_alloc_size);
        for (i = 0; i < BCMMON_BTE_CORE_MAX; i++) {
            telemetry_info->ha_mem->st_uc_num[i] =
                BCMMON_TELEMETRY_INVALID_CORE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmissu_struct_info_report(unit, BCMMGMT_MON_COMP_ID,
                    BCMMON_TELEMETRY_SUB_COMP_ID, 0,
                    ha_req_size, 1,
                    BCMCTH_MON_TELEMETRY_HA_T_ID));
    }

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim) {
        /*
         * Allocate DMA buffer
         *
         * DMA buffer will be used to send and receive 'long' messages
         * between SDK Host and uController (BTE).
         */
        telemetry_info->dma_buffer_len =
            sizeof(bcmcth_mon_telemetry_st_msg_ctrl_t);
        telemetry_info->dma_logical_buffer = bcmdrd_hal_dma_alloc(unit,
                telemetry_info->dma_buffer_len,
                "TELEMETRY DMA buffer",
                &(telemetry_info->dma_buffer));
        if (!telemetry_info->dma_logical_buffer) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(telemetry_info->dma_logical_buffer, 0,
                telemetry_info->dma_buffer_len);
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (telemetry_info->ha_mem != NULL) {
            shr_ha_mem_free(unit, telemetry_info->ha_mem);
        }
        if (telemetry_info->dma_logical_buffer != NULL) {
            bcmdrd_hal_dma_free(unit,
                                sizeof(bcmcth_mon_telemetry_st_msg_ctrl_t),
                                telemetry_info->dma_logical_buffer,
                                telemetry_info->dma_buffer);
            telemetry_info->dma_buffer = 0;
            telemetry_info->dma_logical_buffer = NULL;
        }
    }
    SHR_FUNC_EXIT();
}


int
bcmcth_mon_telemetry_control_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_control_op_t op,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *out)
{
    bool is_sim = FALSE;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);
    if (mon_drv && mon_drv->telemetry_control_op) {
        SHR_IF_ERR_EXIT(mon_drv->telemetry_control_op(unit, trans_id,
                                      lt_id, op, entry, out));
    }

    /* Streaming telemetry handling */
    switch (op) {
        case BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_SET:

            is_sim = bcmdrd_feature_is_sim(unit);
            if (!is_sim) {
                if (entry->streaming_telemetry) {
                    SHR_IF_ERR_EXIT
                        (bcmcth_mon_st_init(unit, entry,
                                            out));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmcth_mon_st_detach(unit, out));
                }
            }
            break;
        case BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_DEL:
            is_sim = bcmdrd_feature_is_sim(unit);
            if (!is_sim) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_detach(unit, out));
            }
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_control_config_from_outfields(
    int unit,
    bcmltd_field_t *data,
    const bcmltd_fields_t *out_flds,
    bcmcth_mon_telemetry_control_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                data,
                out_flds));

    SHR_IF_ERR_EXIT(mon_telemetry_control_lt_fields_parse(unit,
                data,
                entry));
exit:
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_st_instance_oper_state_set(
    int unit,
    uint32_t oper_state,
    bcmltd_fields_t *out)
{

    out->field[out->count]->id =
        MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
    out->field[out->count]->data = oper_state;
    out->field[out->count]->idx = 0;
    out->field[out->count]->flags = 0;
    out->count++;
}

static int
bcmcth_mon_telemetry_control_entry_get(
    int unit,
    bcmcth_mon_telemetry_control_t *telemetry_control,
    bcmltd_fields_t *out)
{
    uint32_t sid = 0;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};

    SHR_FUNC_ENTER(unit);
    bcmcth_mon_telemetry_fld_arr_hdl_get(unit, &hdl);
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    out_flds.count = MON_TELEMETRY_CONTROLt_FIELD_COUNT;
    out_flds.field = shr_famm_alloc(hdl, out_flds.count);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sid = MON_TELEMETRY_CONTROLt;
    in_flds.count = 0;

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
            SHR_E_NONE) {
        /* Update oper state of instance to control table error */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit,
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR,
                out);
        }
        SHR_EXIT();
    }

    if (out_flds.count) {
        SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_control_config_from_outfields(
                    unit,
                    out_flds.field[0],
                    &out_flds, telemetry_control));
    } else {
        /* Update oper state of instance to control table error */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit,
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR,
                out);
        }
        SHR_EXIT();
    }
exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, MON_TELEMETRY_CONTROLt_FIELD_COUNT);
    }


    SHR_FUNC_EXIT();
}

int
bcmcth_mon_telemetry_instance_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_instance_op_t op,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    bool is_sim = FALSE;
    bcmcth_mon_telemetry_control_t telemetry_control;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);
    if (mon_drv && mon_drv->telemetry_instance_op) {
        SHR_IF_ERR_EXIT(mon_drv->telemetry_instance_op(unit, trans_id,
                                       lt_id, op, entry, output_fields));
    }
    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim && !telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
        /* Update oper state of instance to control table error */
        if (output_fields && !(output_fields->count)) {
            output_fields->field[output_fields->count]->id =
                MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
            output_fields->field[output_fields->count]->data =
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR;
            output_fields->field[output_fields->count]->idx = 0;
            output_fields->field[output_fields->count]->flags = 0;
            output_fields->count++;
        }
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_control_entry_get(unit,
                                                           &telemetry_control,
                                                           output_fields));
    /* Telemetry Instance LT fields handling */
    switch (op) {
        case BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_SET:
             SHR_IF_ERR_EXIT
                 (bcmcth_mon_st_instance_collector_create(unit, entry,
                 telemetry_control.max_export_len,
                 output_fields));
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_DEL:
            if (!is_sim &&
                telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_collector_delete(unit, entry));
            }
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_SET:
            SHR_IF_ERR_EXIT
                (bcmcth_mon_st_instance_collector_create(unit, entry,
                telemetry_control.max_export_len,
                output_fields));
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_DEL:
            if (!is_sim &&
                telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_collector_delete(unit, entry));

            }
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_SET:
            SHR_IF_ERR_EXIT
                (bcmcth_mon_st_instance_object_update(unit, entry,
                output_fields));
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_DEL:
            if (!is_sim &&
                telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_object_delete(unit, entry));
            }
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_LOOKUP:
            if (!is_sim &&
                telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_stats_get(unit, entry,
                                                      output_fields));
            }
            break;
        case BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_SET:
            if (!is_sim &&
                telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_st_instance_stats_set(unit, entry));
            }
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_telemetry_data_from_outfields(
    int unit,
    bcmltd_field_t *data,
    const bcmltd_fields_t *out_flds)
{
    uint32_t k;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    if (out_flds->count > 1) {
        /* Iterate over table fields to retrive the values . */
        for (k = 0; k < (out_flds->count - 1); k++) {
            data->next = out_flds->field[k+1];
            data = data->next;
        }
    }

    /* Though this is not needed, added as per reviewer comment */
    data->next = NULL;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_object_config_from_outfields(
    int unit,
    const bcmltd_field_t *key,
    bcmltd_field_t *data,
    const bcmltd_fields_t *out_flds,
    bcmcth_mon_telemetry_object_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                data,
                out_flds));
    SHR_IF_ERR_EXIT(mon_telemetry_object_lt_fields_parse(unit,
                key,
                data,
                entry));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_object_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry)
{
    int rv = SHR_E_NONE;
    uint32_t i,j,k=0,next_pos = 0;
    bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t ports_delete_msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL, match_found = 0;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    bcmcth_mon_telemetry_object_t object_config;
    uint16_t config_lport = 0, list_lport = 0;
    uint16_t lports_list[MCS_SHR_ST_MAX_PORTS];
    uint16_t del_lports_list[MCS_SHR_ST_MAX_PORTS];
    uint16_t num_ports;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    uint32_t key = 0, sid = 0;
    shr_famm_hdl_t      hdl = NULL;
    bool is_sim = FALSE;
    uint32_t obj_fld_cnt = 0;

    SHR_FUNC_ENTER(unit);

    bcmcth_mon_telemetry_fld_arr_hdl_get(unit, &hdl);

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    obj_fld_cnt = (MON_TELEMETRY_OBJECTt_FIELD_COUNT - 2) +
                      BCMMON_TELEMETRY_MAX_PORT_NAME +
                      BCMMON_TELEMETRY_STATS_MAX;
    out_flds.field = shr_famm_alloc(hdl, obj_fld_cnt);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sid = MON_TELEMETRY_OBJECTt;
    key = MON_TELEMETRY_OBJECTt_MON_TELEMETRY_OBJECT_IDf;

    sal_memset(&ports_delete_msg, 0, sizeof(ports_delete_msg));
    num_ports = telemetry_info->ha_mem->num_ports;
    sal_memcpy(lports_list, telemetry_info->ha_mem->lports,
               (num_ports * sizeof(uint16_t)));

    /*Deleting from ports list which does not present in new config */
    for (i= 0; i < num_ports; i++) {
        match_found = 0;
        list_lport = lports_list[i];
        for (j= 0; j < BCMMON_TELEMETRY_OBJECTS_MAX; j++) {

            in_flds.count = 1;
            in_flds.field[0]->id = key;
            in_flds.field[0]->data = entry->object_id[j];
            out_flds.count = obj_fld_cnt;

            if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                    SHR_E_NONE) {
                continue;
            }
            SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_object_config_from_outfields(
                unit,
                in_flds.field[0],
                out_flds.field[0],
                &out_flds, &object_config));

            config_lport = object_config.port_id;
            if (config_lport == list_lport) {
                match_found = 1;
                break;
            }
        }
        if (match_found == 0)
        {
            lports_list[i] = 0;
            telemetry_info->ha_mem->num_ports--;
            del_lports_list[k] = list_lport;
            k++;
        }
    }

    /*Construct the original port list without any gaps */
    sal_memset(telemetry_info->ha_mem->lports, 0,
               (num_ports * sizeof(uint16_t)));
    for (i=0; i< num_ports; i++)
    {
        if (lports_list[i] != 0)
        {
            telemetry_info->ha_mem->lports[next_pos] =
                                    lports_list[i];
            next_pos++;
        }
    }

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim) {
        ports_delete_msg.num_ports = k;
        sal_memcpy(ports_delete_msg.lports,
                del_lports_list,
                (ports_delete_msg.num_ports * sizeof(uint16_t)));

        buffer_req      = telemetry_info->dma_logical_buffer;
        buffer_ptr      = bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_pack(
                buffer_req,
                &ports_delete_msg);
        buffer_len      = buffer_ptr - buffer_req;

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_PORTS_DELETE,
                buffer_len, 0,
                MCS_MSG_SUBCLASS_ST_PORTS_DELETE_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }

        /* If there are no objects(ports) to be monitored then
         * delete instance
         */
        if (entry->num_objects == 0) {
            rv = bcmcth_mon_telemetry_send_receive(unit,
                    entry->core_instance,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_DELETE,
                    /* Sending Instance ID zero as only one
                     * instance is supported in FW
                     */
                    0, 0,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_DELETE_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                if (SHR_SUCCESS(rv)) {
                    rv = SHR_E_INTERNAL;
                }
                SHR_ERR_EXIT(rv);
            }
        }
    }
exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, obj_fld_cnt);
    }

    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_st_control_oper_state_set(
    int unit,
    uint32_t oper_state,
    bcmltd_fields_t *out)
{

    out->field[out->count]->id =
        MON_TELEMETRY_CONTROLt_OPERATIONAL_STATEf;
    out->field[out->count]->data = oper_state;
    out->field[out->count]->idx = 0;
    out->field[out->count]->flags = 0;
    out->count++;
}

static void
bcmcth_mon_st_control_oper_state_clear(
    int unit,
    bcmltd_fields_t *out)
{
    if (out->count) {
        out->field[out->count - 1]->id = MON_TELEMETRY_CONTROLt_OPERATIONAL_STATEf;
        out->field[out->count - 1]->data =
            BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_VALID;
    }
}
static void
bcmcth_mon_telemetry_control_oper_fields_update(
    int unit,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields)
{
    output_fields->field[output_fields->count]->id =
        MON_TELEMETRY_CONTROLt_MAX_NUM_PORTS_OPERf;
    output_fields->field[output_fields->count]->data =
        entry->max_num_ports;
    output_fields->count++;
    output_fields->field[output_fields->count]->id =
        MON_TELEMETRY_CONTROLt_MAX_EXPORT_LENGTH_OPERf;
    output_fields->field[output_fields->count]->data =
        entry->max_export_len;
    output_fields->count++;

}

static void
bcmcth_mon_st_instance_oper_state_clear(
    int unit,
    bcmltd_fields_t *out)
{
    if (out->count) {
        out->field[(out->count) - 1]->id =
            MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
        out->field[(out->count) - 1]->data = 0;
        out->count--;
    }
}

static int
bcmcth_mon_st_instance_oper_state_update(
    int unit,
    uint32_t inst_id,
    uint32_t oper_state)
{
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0};

    SHR_FUNC_ENTER(unit);

    bcmcth_mon_telemetry_fld_arr_hdl_get(unit, &hdl);
    in_flds.count = 2;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.count = 2;
    in_flds.field[0]->id = MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_INSTANCE_IDf;
    in_flds.field[0]->data = inst_id;
    in_flds.field[1]->id = MON_TELEMETRY_INSTANCEt_INSTANCE_OPERATIONAL_STATEf;
    in_flds.field[1]->data = oper_state;
    /*Update Instance with oper state */
    SHR_IF_ERR_EXIT(bcmimm_entry_update(unit, 0, MON_TELEMETRY_INSTANCEt,
                                       &in_flds));

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_st_instance_check_and_update_obj_op_status(int unit,
                            uint32_t inst_id,
                            uint32_t object_id)
{
    uint32_t    count = 0, match_found = 0;
    uint32_t oper_state = 0, i;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);

    for (i = 0; i < BCMMON_TELEMETRY_OBJECTS_MAX; i++) {

        if (SHR_BITGET(telemetry_info->ha_mem->missing_obj_bmp, i) == 0) {
            continue;
        }
        count++;
        if (i == object_id) {
            match_found = 1;
        }
    }
    oper_state = BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_VALID;
    /* Update the object status of the Instance entry if this the only
     * missing object.
     */
    if (match_found && count == 1) {
        bcmcth_mon_st_instance_oper_state_update(unit, inst_id, oper_state);
    }
}

int
bcmcth_mon_st_instance_handle_object_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmcth_mon_telemetry_object_t *object_config)
{
    uint32_t oper_state = 0, i;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < BCMMON_TELEMETRY_OBJECTS_MAX; i++) {
        if (entry->object_id[i] == object_config->object_id)
        {
            entry->object_id[i] = BCMMON_TELEMETRY_DEF_ID;
        }
    }
    SHR_IF_ERR_EXIT
        (bcmcth_mon_st_instance_object_delete(unit, entry));

    SHR_BITSET(telemetry_info->ha_mem->missing_obj_bmp,
               object_config->object_id);
    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_OBJECT_ENTRY_ERROR;
    bcmcth_mon_st_instance_oper_state_update(unit, entry->instance_id,
                                             oper_state);
exit:

    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_handle_object_update(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmcth_mon_telemetry_object_t *object_config)
{
    int rv = SHR_E_NONE;
    uint32_t i,j;
    bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t ports_add_msg;
    bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t
        *instance_create_msg = NULL;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL, match_found = 0;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    uint16_t config_lport = 0, num_ports = 0;
    uint32_t stat_types = 0;
    uint16_t pports[MCS_SHR_ST_MAX_PORTS], mports[MCS_SHR_ST_MAX_PORTS];
    int phy_port = 0, mmu_port = 0;
    bool is_sim = FALSE;
    uint32_t oper_state = 0;


    SHR_FUNC_ENTER(unit);
    SHR_ALLOC(instance_create_msg,
              sizeof(bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t),
              "bcmcthMonInstanceCreateMsg");
    SHR_NULL_CHECK(instance_create_msg, SHR_E_MEMORY);

    sal_memset(&ports_add_msg, 0, sizeof(ports_add_msg));
    sal_memset(instance_create_msg, 0,
               sizeof(bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t));
    sal_memset(pports, 0,
               (MCS_SHR_ST_MAX_PORTS * sizeof(uint16_t)));
    sal_memset(mports, 0,
               (MCS_SHR_ST_MAX_PORTS * sizeof(uint16_t)));

    num_ports = telemetry_info->ha_mem->num_ports;
    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_OBJECT_ENTRY_ERROR;
    for (i = 0; i < BCMMON_TELEMETRY_OBJECTS_MAX; i++) {

        stat_types = 0;
        /* Only update input's container(s). */
        if (SHR_BITGET(entry->fbmp_tel_objs, i) == 0) {
            continue;
        }

        if (entry->object_id[i] != object_config->object_id) {
            continue;
        }
        config_lport = object_config->port_id;
        for (j = 0; j < BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_METADATA; j++) {
            if (object_config->telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_INGRESS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_INGRESS;
            }
            if (object_config->telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_EGRESS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_EGRESS;
            }
            if (object_config->telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_EGRESS_QUEUE)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_QUEUE_EGRESS;
            }
            if (object_config->telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_INGRESS_ERRORS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS;
            }
        }
        match_found = 0;
        for (j= 0; j < num_ports; j++) {
            if (config_lport == telemetry_info->ha_mem->lports[j]) {
                match_found = 1;
                instance_create_msg->stat_types[j]= stat_types;
                instance_create_msg->if_name_lengths[j] =
                    object_config->port_name_len;
                sal_memcpy(&(instance_create_msg->if_names[j]),
                        object_config->port_name,
                        object_config->port_name_len);
                break;
            }
        }
        oper_state =
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_PORT_ENTRY_ERROR;
        /* Adding new port to existing ports list */
        if (match_found == 0)
        {
            if (!SHR_BITGET(telemetry_info->ha_mem->missing_obj_bmp,
                       object_config->object_id)) {
                continue;
            }
            telemetry_info->ha_mem->lports
                [telemetry_info->ha_mem->num_ports] = config_lport;

            if (bcmtm_lport_pport_get(unit, config_lport,
                        &phy_port) != SHR_E_NONE) {
                /* Update oper state of instance to port not configured */
                bcmcth_mon_st_instance_oper_state_update(unit,
                                                         entry->instance_id,
                                                         oper_state);
                SHR_EXIT();
            }
            oper_state =
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_MMU_PORT_ENTRY_ERROR;
            if (bcmtm_lport_mport_get(unit, config_lport,
                        &mmu_port) != SHR_E_NONE) {
                /* Update oper state of instance to MMU port not configured */
                bcmcth_mon_st_instance_oper_state_update(unit,
                                                         entry->instance_id,
                                                         oper_state);
                SHR_EXIT();
            }
            instance_create_msg->stat_types[telemetry_info->ha_mem->num_ports] =
                stat_types;
            instance_create_msg->if_name_lengths
                [telemetry_info->ha_mem->num_ports] =
                object_config->port_name_len;
            sal_memcpy(&(instance_create_msg->if_names
                [telemetry_info->ha_mem->num_ports]),
                    object_config->port_name,
                    object_config->port_name_len);
            pports[telemetry_info->ha_mem->num_ports] = phy_port;
            mports[telemetry_info->ha_mem->num_ports] = mmu_port;
            telemetry_info->ha_mem->num_ports++;
        }
    }

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim &&
        telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
        if (!match_found) {
            ports_add_msg.num_ports = telemetry_info->ha_mem->num_ports;
            sal_memcpy(ports_add_msg.lports,
                    telemetry_info->ha_mem->lports,
                    (ports_add_msg.num_ports * sizeof(uint16_t)));
            sal_memcpy(ports_add_msg.pports, pports,
                    (ports_add_msg.num_ports * sizeof(uint16_t)));
            sal_memcpy(ports_add_msg.mports, mports,
                    (ports_add_msg.num_ports * sizeof(uint16_t)));

            buffer_req = telemetry_info->dma_logical_buffer;
            buffer_ptr = bcmcth_mon_telemetry_st_msg_ctrl_ports_add_pack(
                    buffer_req,
                    &ports_add_msg);
            buffer_len      = buffer_ptr - buffer_req;

            rv = bcmcth_mon_telemetry_send_receive(unit,
                    entry->core_instance,
                    MCS_MSG_SUBCLASS_ST_PORTS_ADD,
                    buffer_len, 0,
                    MCS_MSG_SUBCLASS_ST_PORTS_ADD_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                SHR_EXIT();
            }
        }

        if (entry->num_objects && match_found) {
            instance_create_msg->num_ports = telemetry_info->ha_mem->num_ports;
            instance_create_msg->instance_id = entry->instance_id;
            sal_memcpy(instance_create_msg->lports,
                    telemetry_info->ha_mem->lports,
                    (instance_create_msg->num_ports * sizeof(uint16_t)));

            buffer_req = telemetry_info->dma_logical_buffer;
            buffer_ptr =
                bcmcth_mon_telemetry_st_msg_ctrl_instance_create_pack(
                        buffer_req,
                        instance_create_msg);
            buffer_len      = buffer_ptr - buffer_req;

            rv = bcmcth_mon_telemetry_send_receive(unit,
                    entry->core_instance,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE,
                    buffer_len, 0,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                SHR_EXIT();
            }
        }
    }
    bcmcth_mon_st_instance_check_and_update_obj_op_status(unit,
                                                     entry->instance_id,
                                                     object_config->object_id);
    SHR_BITCLR(telemetry_info->ha_mem->missing_obj_bmp,
               object_config->object_id);
exit:
    if (instance_create_msg) {
        SHR_FREE(instance_create_msg);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_object_update(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *out)
{
    int rv = SHR_E_NONE;
    uint32_t i,j;
    bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t ports_add_msg;
    bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t
        *instance_create_msg = NULL;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL, match_found = 0;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    bcmcth_mon_telemetry_object_t object_config;
    uint16_t config_lport = 0, num_ports = 0;
    uint32_t stat_types = 0;
    uint16_t pports[MCS_SHR_ST_MAX_PORTS], mports[MCS_SHR_ST_MAX_PORTS];
    int phy_port = 0, mmu_port = 0;
    uint32_t key = 0, sid = 0;
    bool is_sim = FALSE, port_not_configured = FALSE;
    bool mmu_port_not_configured = FALSE;
    uint32_t oper_state = 0;
    uint32_t obj_fld_cnt = 0;


    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(instance_create_msg,
              sizeof(bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t),
              "bcmcthMonInstanceCreateMsg");
    SHR_NULL_CHECK(instance_create_msg, SHR_E_MEMORY);

    sal_memset(pports, 0,
               (MCS_SHR_ST_MAX_PORTS * sizeof(uint16_t)));
    sal_memset(mports, 0,
               (MCS_SHR_ST_MAX_PORTS * sizeof(uint16_t)));
    bcmcth_mon_telemetry_fld_arr_hdl_get(unit, &hdl);
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }


    obj_fld_cnt = (MON_TELEMETRY_OBJECTt_FIELD_COUNT - 2) +
                      BCMMON_TELEMETRY_MAX_PORT_NAME +
                      BCMMON_TELEMETRY_STATS_MAX;
    out_flds.field = shr_famm_alloc(hdl, obj_fld_cnt);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sid = MON_TELEMETRY_OBJECTt;
    key = MON_TELEMETRY_OBJECTt_MON_TELEMETRY_OBJECT_IDf;

    sal_memset(&ports_add_msg, 0, sizeof(ports_add_msg));
    sal_memset(instance_create_msg, 0,
               sizeof(bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t));
    num_ports = telemetry_info->ha_mem->num_ports;
    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_OBJECT_ENTRY_ERROR;
    for (i = 0; i < BCMMON_TELEMETRY_OBJECTS_MAX; i++) {

        stat_types = 0;
        /* Only update input's container(s). */
        if (SHR_BITGET(entry->fbmp_tel_objs, i) == 0) {
            continue;
        }

        if (entry->object_id[i] == BCMMON_TELEMETRY_DEF_ID) {
            continue;
        }
        in_flds.count = 1;
        in_flds.field[0]->id = key;
        in_flds.field[0]->data = entry->object_id[i];
        out_flds.count = obj_fld_cnt;

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                SHR_E_NONE) {
            /* Update oper state of object to retrieval error */
            if (out && out->count == 0) {
                bcmcth_mon_st_instance_oper_state_set(unit, oper_state, out);
            }
            SHR_BITSET(telemetry_info->ha_mem->missing_obj_bmp,
                       entry->object_id[i]);
            continue;
        }
        SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_object_config_from_outfields(
                    unit,
                    in_flds.field[0],
                    out_flds.field[0],
                    &out_flds, &object_config));
        config_lport = object_config.port_id;
        for (j = 0; j < BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_METADATA; j++) {
            if (object_config.telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_INGRESS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_INGRESS;
            }
            if (object_config.telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_EGRESS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_EGRESS;
            }
            if (object_config.telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_EGRESS_QUEUE)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_QUEUE_EGRESS;
            }
            if (object_config.telemetry_stat[j] ==
                    BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_INGRESS_ERRORS)
            {
                stat_types |= MCS_SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS;
            }
        }

        /*Adding new ports to existing ports list */
        match_found = 0;
        for (j= 0; j < num_ports; j++) {
            if (config_lport == telemetry_info->ha_mem->lports[j]) {
                match_found = 1;
                instance_create_msg->stat_types[j] = stat_types;
                instance_create_msg->if_name_lengths[j] =
                    object_config.port_name_len;
                sal_memcpy(&(instance_create_msg->if_names[j]),
                        object_config.port_name,
                        object_config.port_name_len);
                break;
            }
        }
        oper_state =
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_PORT_ENTRY_ERROR;
        if (match_found == 0)
        {
            telemetry_info->ha_mem->lports
                [telemetry_info->ha_mem->num_ports] = config_lport;

            if (bcmtm_lport_pport_get(unit, config_lport,
                        &phy_port) != SHR_E_NONE) {
                /* Update oper state of instance to port not configured */
                if (out && (out->count == 0) &&
                    (port_not_configured == FALSE)) {
                    bcmcth_mon_st_instance_oper_state_set(unit, oper_state,
                                                          out);
                    port_not_configured = TRUE;
                }
                continue;
            }
            oper_state =
                BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_MMU_PORT_ENTRY_ERROR;
            if (bcmtm_lport_mport_get(unit, config_lport,
                        &mmu_port) != SHR_E_NONE) {
                /* Update oper state of instance to MMU port not configured */
                if (out && (out->count == 0) &&
                    (mmu_port_not_configured == FALSE)) {
                    bcmcth_mon_st_instance_oper_state_set(unit, oper_state,
                                                          out);
                    mmu_port_not_configured = TRUE;
                }
                continue;
            }
            instance_create_msg->stat_types[telemetry_info->ha_mem->num_ports] =
                stat_types;
            instance_create_msg->if_name_lengths
                [telemetry_info->ha_mem->num_ports] =
                object_config.port_name_len;
            sal_memcpy(&(instance_create_msg->if_names
                [telemetry_info->ha_mem->num_ports]),
                    object_config.port_name,
                    object_config.port_name_len);
            pports[telemetry_info->ha_mem->num_ports] = phy_port;
            mports[telemetry_info->ha_mem->num_ports] = mmu_port;
            telemetry_info->ha_mem->num_ports++;
        }
    }

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim &&
        telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
        ports_add_msg.num_ports = telemetry_info->ha_mem->num_ports;
        sal_memcpy(ports_add_msg.lports,
                telemetry_info->ha_mem->lports,
                (ports_add_msg.num_ports * sizeof(uint16_t)));
        sal_memcpy(ports_add_msg.pports, pports,
                (ports_add_msg.num_ports * sizeof(uint16_t)));
        sal_memcpy(ports_add_msg.mports, mports,
                (ports_add_msg.num_ports * sizeof(uint16_t)));

        buffer_req = telemetry_info->dma_logical_buffer;
        buffer_ptr = bcmcth_mon_telemetry_st_msg_ctrl_ports_add_pack(
                buffer_req,
                &ports_add_msg);
        buffer_len      = buffer_ptr - buffer_req;

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_PORTS_ADD,
                buffer_len, 0,
                MCS_MSG_SUBCLASS_ST_PORTS_ADD_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }

        if (entry->num_objects) {
            instance_create_msg->num_ports = telemetry_info->ha_mem->num_ports;
            instance_create_msg->instance_id = entry->instance_id;
            sal_memcpy(instance_create_msg->lports,
                    telemetry_info->ha_mem->lports,
                    (instance_create_msg->num_ports * sizeof(uint16_t)));

            buffer_req = telemetry_info->dma_logical_buffer;
            buffer_ptr =
                bcmcth_mon_telemetry_st_msg_ctrl_instance_create_pack(
                        buffer_req,
                        instance_create_msg);
            buffer_len      = buffer_ptr - buffer_req;

            rv = bcmcth_mon_telemetry_send_receive(unit,
                    entry->core_instance,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE,
                    buffer_len, 0,
                    MCS_MSG_SUBCLASS_ST_INSTANCE_CREATE_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                if (SHR_SUCCESS(rv)) {
                    rv = SHR_E_INTERNAL;
                }
                SHR_ERR_EXIT(rv);
            }
        }
    }
    if (out && (out->count == 1) &&
        ((out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_OBJECT_ENTRY_ERROR) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_PORT_ENTRY_ERROR) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_MMU_PORT_ENTRY_ERROR))) {
        bcmcth_mon_st_instance_oper_state_clear(unit, out);
    }
exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, obj_fld_cnt);
    }

    if (instance_create_msg) {
        SHR_FREE(instance_create_msg);
    }
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_st_collector_encap_build(
    int unit,
    bcmcth_mon_collector_ipv4_t *collector,
    bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t *collector_create_msg)
{
    uint8_t          *buffer  = collector_create_msg->encap;
    uint8_t          *cur_ptr;
    shr_util_pack_udp_header_t  udp;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_l2_header_t   l2;
    uint16_t         ip_offset = 0;
    uint16_t         udp_offset = 0;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&l2, 0, sizeof(l2));

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, collector->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, collector->src_mac, sizeof(shr_mac_t));


    if (mon_drv && mon_drv->collector_ipv4_vlan_info_copy) {
        mon_drv->collector_ipv4_vlan_info_copy(unit, collector, &l2);
    }
    l2.etype = SHR_L2_ETYPE_IPV4;

    /* IPv4 header. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HDR_WLEN;
    ipv4.dscp = collector->tos;
    ipv4.ttl = collector->ttl;
    ipv4.protocol = SHR_IP_PROTO_UDP;
    ipv4.src = collector->src_ip;
    ipv4.dst = collector->dst_ip;

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = collector->src_port;
    udp.dst = collector->dst_port;

    /* Ether Type */
    l2.etype = SHR_L2_ETYPE_IPV4;

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    /*
     * IP offset
     */
    ip_offset = cur_ptr - buffer;

    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);

    /*
     * UDP offset
     */
    udp_offset = cur_ptr - buffer;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);


    /* Set export pkt encapsulation length */
    collector_create_msg->encap_length = cur_ptr - buffer;

    /* Set export pkt IP base offset */
    collector_create_msg->ip_offset = ip_offset;

    /* Set export pkt IP Base checksum */
    collector_create_msg->ip_base_checksum =
        shr_util_pack_initial_chksum_get(SHR_IPV4_HEADER_LENGTH,
        buffer + ip_offset);

    /* Set export pkt UDP Base checksum */
    collector_create_msg->udp_base_checksum =
        shr_util_pack_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    /* Set export pkt UDP base offset */
    collector_create_msg->udp_offset = udp_offset;

}

int
bcmcth_mon_st_instance_collector_create(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    uint32_t max_export_len,
    bcmltd_fields_t *out)
{
    bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t collector_create_msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    int rv = SHR_E_NONE;
    bcmcth_mon_telemetry_st_msg_system_id_t system_id_set_msg;
    bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t
        collector_attach_msg;
    bool is_sim = FALSE;
    uint32_t oper_state = 0;
    bcmcth_mon_collector_ipv4_t collector_ipv4;
    bcmcth_mon_export_profile_t export_profile;

    SHR_FUNC_ENTER(unit);


    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_COLLECTOR_ENTRY_ERROR;

    rv = bcmcth_mon_collector_ipv4_entry_get(unit,
            entry->collector_id,
            &collector_ipv4);
    if (rv != SHR_E_NONE) {
        /* Update oper state of collector to not configured */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit, oper_state, out);
        }
        SHR_EXIT();
    }
    sal_memset(&system_id_set_msg, 0, sizeof(system_id_set_msg));
    system_id_set_msg.system_id_length = collector_ipv4.system_id_len;
    sal_memcpy(system_id_set_msg.system_id, collector_ipv4.system_id,
               (collector_ipv4.system_id_len * sizeof(uint8_t)));

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim &&
        telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
        buffer_req      = telemetry_info->dma_logical_buffer;
        buffer_ptr      = bcmcth_mon_telemetry_st_msg_system_id_pack(buffer_req,
                &system_id_set_msg);
        buffer_len      = buffer_ptr - buffer_req;

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_SYSTEM_ID_SET,
                buffer_len, 0,
                MCS_MSG_SUBCLASS_ST_SYSTEM_ID_SET_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }
    }
    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_EXPORT_PROFILE_ENTRY_ERROR;

    rv = bcmcth_mon_export_profile_entry_get(unit,
            entry->export_profile_id,
            &export_profile);
    if (rv != SHR_E_NONE) {
        /* Update oper state of export profile to not configured */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit, oper_state, out);
        }
        SHR_EXIT();
    }

    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_INTERVAL;
    if (export_profile.export_interval != MCS_SHR_ST_EXPORT_INTERVAL_USECS) {
        /* Update oper state of export profile to interval
         * not supported
         */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit, oper_state,
                    out);
        }
        SHR_EXIT();
    }
    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_UNSUPPORTED_WIRE_FORMAT;
    if (export_profile.wire_format != BCMLTD_COMMON_WIRE_FORMAT_T_T_PROTOBUF) {
        /* Update oper state of export profile to interval
         * not supported
         */
        if (out && out->count == 0) {
            bcmcth_mon_st_instance_oper_state_set(unit, oper_state,
                    out);
        }
        SHR_EXIT();
    }

    sal_memset(&collector_create_msg, 0, sizeof(collector_create_msg));
    /*Implement multiple collector when it really being supported by BTE */
    collector_create_msg.id = 0;

    if (export_profile.packet_len_indicator ==
        BCMLTD_COMMON_EXPORT_PACKET_LEN_INDICATOR_T_T_NUM_RECORDS) {
        collector_create_msg.num_port_in_pkt = export_profile.num_records;
    } else {
        collector_create_msg.mtu = export_profile.max_packet_length;
    }

    if (collector_ipv4.udp_chksum) {
        collector_create_msg.flags |=
            MCS_SHR_ST_COLLECTOR_FLAGS_UDP_CHECKSUM;
    }

    collector_create_msg.component_id = collector_ipv4.component_id;
    bcmcth_mon_st_collector_encap_build(unit, &collector_ipv4,
                                        &collector_create_msg);

    oper_state =
        BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_PACKET_LENGTH;
    if (export_profile.packet_len_indicator ==
        BCMLTD_COMMON_EXPORT_PACKET_LEN_INDICATOR_T_T_MAX_PKT_LENGTH) {
        if ((export_profile.max_packet_length >
                    max_export_len) ||
                (export_profile.max_packet_length <
                 (uint32_t)(collector_create_msg.encap_length +
                            SHR_L2_CRC_LEN))) {
            /* Update oper state of export profile to invalid
             * export packet length
             */
            if (out && out->count == 0) {
                bcmcth_mon_st_instance_oper_state_set(unit, oper_state,
                        out);
            }
            SHR_EXIT();
        }
    }

    if (!is_sim &&
        telemetry_info->ha_mem->st_initialized[entry->core_instance]) {
        buffer_req = telemetry_info->dma_logical_buffer;
        buffer_ptr = bcmcth_mon_telemetry_st_msg_ctrl_collector_create_pack(
                buffer_req,
                &collector_create_msg);
        buffer_len      = buffer_ptr - buffer_req;

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_COLLECTOR_CREATE,
                buffer_len, 0,
                MCS_MSG_SUBCLASS_ST_COLLECTOR_CREATE_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }

        sal_memset(&collector_attach_msg, 0, sizeof(collector_attach_msg));
        /*Implement multiple collector when it really being supported by BTE */
        collector_attach_msg.instance_id = entry->instance_id;
        collector_attach_msg.export_interval_usecs =
            export_profile.export_interval;
        collector_attach_msg.collector_id = entry->collector_id;

        buffer_req      = telemetry_info->dma_logical_buffer;
        buffer_ptr      =
            bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_pack(
            buffer_req,
            &collector_attach_msg);
        buffer_len      = buffer_ptr - buffer_req;

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH,
                buffer_len, 0,
                MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }
    }
    if (out && (out->count == 1) &&
        ((out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_UNSUPPORTED_WIRE_FORMAT) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_COLLECTOR_ENTRY_ERROR) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_EXPORT_PROFILE_ENTRY_ERROR) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_INTERVAL) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR) ||
         (out->field[(out->count) - 1]->data ==
         BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_PACKET_LENGTH))) {
        bcmcth_mon_st_instance_oper_state_clear(unit, out);
    }
    if ((out == NULL) &&
        ((entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_UNSUPPORTED_WIRE_FORMAT) ||
        (entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_COLLECTOR_ENTRY_ERROR) ||
        (entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_EXPORT_PROFILE_ENTRY_ERROR) ||
        (entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_INTERVAL) ||
        (entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR) ||
        (entry->oper_state == BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_INVALID_PACKET_LENGTH))) {
        bcmcth_mon_st_instance_oper_state_update(unit, entry->instance_id,
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_VALID);
    }

    telemetry_info->ha_mem->collector_valid = true;
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_collector_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry)
{
    int rv = SHR_E_NONE;
    uint16_t reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    SHR_FUNC_ENTER(unit);

    if (telemetry_info->ha_mem->collector_valid == true) {
        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH,
                entry->instance_id, 0,
                MCS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }

        rv = bcmcth_mon_telemetry_send_receive(unit,
                entry->core_instance,
                MCS_MSG_SUBCLASS_ST_COLLECTOR_DELETE,
                entry->collector_id, 0,
                MCS_MSG_SUBCLASS_ST_COLLECTOR_DELETE_REPLY,
                &reply_len,
                MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

        if (SHR_FAILURE(rv) || (reply_len != 0)) {
            if (SHR_SUCCESS(rv)) {
                rv = SHR_E_INTERNAL;
            }
            SHR_ERR_EXIT(rv);
        }

        telemetry_info->ha_mem->collector_valid = false;
    }
exit:

    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_stats_set(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry)
{
    bcmcth_mon_telemetry_st_msg_instance_export_stats_t msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    msg.tx_pkts_hi = (uint32_t)(entry->tx_pkts >> 32);
    msg.tx_pkts_lo = (uint32_t)(entry->tx_pkts & 0xFFFFFFFF);

    msg.tx_pkt_fails_hi = (uint32_t)(entry->tx_pkt_fails >> 32);
    msg.tx_pkt_fails_lo = (uint32_t)(entry->tx_pkt_fails & 0xFFFFFFFF);

    buffer_req = telemetry_info->dma_logical_buffer;
    buffer_ptr = bcmcth_mon_telemetry_st_msg_instance_export_stats_pack(
                     buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    rv = bcmcth_mon_telemetry_send_receive(unit,
             entry->core_instance,
             MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET,
             buffer_len, 0,
             MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET_REPLY,
             &reply_len,
             MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
    if (SHR_FAILURE(rv) || (reply_len != 0)) {
        if (SHR_SUCCESS(rv)) {
            rv = SHR_E_INTERNAL;
        }
        SHR_ERR_EXIT(rv);
    }
exit:

    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_instance_stats_get(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *out)
{
    bcmcth_mon_telemetry_st_msg_instance_export_stats_t msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&msg, 0, sizeof(msg));

    rv = bcmcth_mon_telemetry_send_receive(unit,
             entry->core_instance,
             MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET,
             entry->instance_id, 0,
             MCS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET_REPLY,
             &reply_len,
             MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

    buffer_req = telemetry_info->dma_logical_buffer;
    buffer_ptr = bcmcth_mon_telemetry_st_msg_instance_export_stats_unpack(
                     buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        rv = SHR_E_INTERNAL;
        SHR_ERR_EXIT(rv);
    }

    out->field[out->count]->id = MON_TELEMETRY_INSTANCEt_TX_PKTSf;
    out->field[out->count]->data = (((uint64_t)msg.tx_pkts_hi) << 32) |
        msg.tx_pkts_lo;
    out->count++;
    out->field[out->count]->id = MON_TELEMETRY_INSTANCEt_TX_PKT_FAILSf;
    out->field[out->count]->data = (((uint64_t)msg.tx_pkt_fails_hi) << 32) |
        msg.tx_pkt_fails_lo;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_telemetry_instance_config_from_outfields(
    int unit,
    const bcmltd_field_t *key,
    bcmltd_field_t *data,
    const bcmltd_fields_t *out_flds,
    bcmcth_mon_telemetry_instance_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_data_from_outfields(unit,
                data,
                out_flds));
    SHR_IF_ERR_EXIT(mon_telemetry_instance_lt_fields_parse(unit,
                key,
                data,
                entry));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_st_instance_replay(
    int unit,
    uint32_t max_export_len)
{
    uint32_t    i = 0, sid = 0;
    uint32_t    key = 0;
    uint32_t    min = 0, max = 0;
    bcmlrd_field_def_t  def;
    shr_famm_hdl_t      hdl = NULL;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    bcmcth_mon_telemetry_instance_t entry;
    uint32_t    instance_out_count = 0;

    SHR_FUNC_ENTER(unit);

    sid = MON_TELEMETRY_INSTANCEt;
    key = MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_INSTANCE_IDf;

    bcmcth_mon_telemetry_fld_arr_hdl_get(unit, &hdl);
    /*
     * Do an IMM lookup to retrieve the instance entries
     */
    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(hdl, 3);

    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_telemetry_instance_out_fields_count(unit,
                                                       &instance_out_count));
    out_flds.count = instance_out_count;
    out_flds.field = shr_famm_alloc(hdl, out_flds.count);

    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, sid, key, &def));
    min = TELEMETRY_FIELD_MIN(def);
    max = TELEMETRY_FIELD_MAX(def);

    for (i = min; i <= max; i++) {

        in_flds.count = 1;
        in_flds.field[0]->id = key;
        in_flds.field[0]->data = i;
        out_flds.count = instance_out_count;

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                SHR_E_NONE) {
            continue;
        }
        SHR_IF_ERR_EXIT(bcmcth_mon_telemetry_instance_config_from_outfields(
                    unit,
                    in_flds.field[0],
                    out_flds.field[0],
                    &out_flds, &entry));
        SHR_IF_ERR_EXIT
            (bcmcth_mon_st_instance_object_update(unit, &entry,
                                                  NULL));
        SHR_IF_ERR_EXIT
            (bcmcth_mon_st_instance_collector_create(unit, &entry,
                                                     max_export_len,
                                                     NULL));
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 3);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, instance_out_count);
    }

    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_st_tm_num_mc_q_non_cpu_port_get(int unit)
{
#define MC_Q_MODE_DEFAULT 2
    static int mc_q_mode_num_mc_queues[4] = {0, 2, 4, 6};
    bcmltd_sid_t ltid = TM_SCHEDULER_CONFIGt;
    bcmltd_fid_t fid = TM_SCHEDULER_CONFIGt_NUM_MC_Qf;
    uint64_t mc_q_mode;
    int rv;

    rv = bcmcfg_field_get(unit, ltid, fid, &mc_q_mode);
    if (rv == SHR_E_UNAVAIL) {
        mc_q_mode = MC_Q_MODE_DEFAULT;
    }
    return mc_q_mode_num_mc_queues[mc_q_mode];
}

int
bcmcth_mon_st_init(
    int unit,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *out)
{

    bcmcth_mon_telemetry_st_msg_ctrl_init_t ctrl_init_msg;
    bcmcth_mon_telemetry_st_msg_ctrl_config_t ctrl_config_msg;
    uint8_t *buffer_req = NULL, *buffer_ptr = NULL, uc_loaded = 0;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);
    int rv = SHR_E_NONE;
    bcmbd_ukernel_info_t uc_info;
    int uc, num_uc, i;
    uint64_t ts;
    uint8_t atleast_one_loaded = FALSE;
    uint32_t l3_dst_discard_id = 0;
    uint32_t l3_hdr_err_cnt_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Init the app, run through all uCs and deteremine which uCs has
     * telemetry APP loaded by looking at success to APPL_INIT message.
     */
    num_uc = bcmbd_mcs_num_uc(unit);
    for (uc = 0; uc < num_uc ; uc++) {
        if (!telemetry_info->ha_mem->st_initialized[uc]) {
            rv = bcmbd_mcs_uc_status(unit, uc, &uc_info);
            if (rv == SHR_E_NONE && ((sal_strcmp(uc_info.status,"OK")) == 0)) {
                rv = bcmcth_mon_telemetry_uc_appl_init(unit, uc);
                if (SHR_E_NONE != rv) {
                    continue;
                } else {
                    telemetry_info->ha_mem->st_uc_num[uc] = uc;
                    uc_loaded = 1;
                    telemetry_info->ha_mem->uc_loaded_count++;
                }
            }
        } else {
            atleast_one_loaded = TRUE;
            continue;
        }
    }

    if (!uc_loaded && !atleast_one_loaded) {
        /* Update oper state to APP not initialized. */
        if (out && out->count == 0) {
            bcmcth_mon_st_control_oper_state_set(unit,
                BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_NOT_INITIALIZED,
                out);
        }
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    for (i = 0; i < num_uc; i++) {
        if ((telemetry_info->ha_mem->st_uc_num[i] !=
             BCMMON_TELEMETRY_INVALID_CORE) &&
            (!telemetry_info->ha_mem->st_initialized[i])) {
            /* Set control message data */
            sal_memset(&ctrl_init_msg, 0, sizeof(ctrl_init_msg));
            ctrl_init_msg.max_export_pkt_length =
                entry->max_export_len;
            /*Init time in milli seconds as per proto file*/
            ts = (sal_time() *1000);
            ctrl_init_msg.init_time_l = (ts & 0xFFFFFFFF);
            ctrl_init_msg.init_time_u = ((ts >> 32) & 0xFFFFFFFF);
            ctrl_init_msg.num_instances = BCMMON_TELEMETRY_INSTANCES_MAX;
            ctrl_init_msg.num_collectors = 1;
            if (BCMMON_TELEMETRY_ST_UC_FEATURE_CHECK(
                        BCMMON_TELEMETRY_ST_MAX_NUM_PORTS)) {
                ctrl_init_msg.max_num_ports  = entry->max_num_ports;
            }

            buffer_req      = telemetry_info->dma_logical_buffer;
            buffer_ptr      = bcmcth_mon_telemetry_st_msg_ctrl_init_pack(
                                  buffer_req,
                                  &ctrl_init_msg);
            buffer_len      = buffer_ptr - buffer_req;

            rv = bcmcth_mon_telemetry_send_receive(unit,
                    telemetry_info->ha_mem->st_uc_num[i],
                    MCS_MSG_SUBCLASS_ST_INIT,
                    buffer_len, 0,
                    MCS_MSG_SUBCLASS_ST_INIT_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                /* Update oper state to APP communication failure. */
                if (out && out->count == 0) {
                    bcmcth_mon_st_control_oper_state_set(unit,
                            BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_COMMUNICATION_FAILURE,
                            out);
                }
                rv = SHR_E_NONE;
                SHR_ERR_EXIT(rv);
            }
            telemetry_info->ha_mem->st_initialized[i] = TRUE;

            sal_memset(&ctrl_config_msg, 0, sizeof(ctrl_config_msg));
            ctrl_config_msg.config_value[BCMCTH_MON_TELEMETRY_ST_CONFIG_PORT_NUM_MC_QUEUES] =
                bcmcth_mon_st_tm_num_mc_q_non_cpu_port_get(unit);
            SHR_IF_ERR_EXIT
                (bcmlrd_field_symbol_to_value(unit,
                                              CTR_ING_DROP_EVENTt,
                                              CTR_ING_DROP_EVENTt_CTR_ING_DROP_EVENT_IDf,
                                              "L3_DST_DISCARD",
                                              &l3_dst_discard_id));
            SHR_IF_ERR_EXIT
                (bcmlrd_field_symbol_to_value(unit,
                                              CTR_ING_DROP_EVENTt,
                                              CTR_ING_DROP_EVENTt_CTR_ING_DROP_EVENT_IDf,
                                              "L3_HDR_ERROR",
                                              &l3_hdr_err_cnt_id));
            ctrl_config_msg.config_value[BCMCTH_MON_TELEMETRY_ST_CONFIG_PORT_NUM_MC_QUEUES] =
                bcmcth_mon_st_tm_num_mc_q_non_cpu_port_get(unit);
            ctrl_config_msg.config_value[BCMCTH_MON_TELEMETRY_ST_CONFIG_L3_DISCARD_INDEX] =
                l3_dst_discard_id;
            ctrl_config_msg.config_value[BCMCTH_MON_TELEMETRY_ST_CONFIG_L3_HDR_ERROR_INDEX] =
                l3_hdr_err_cnt_id;
            ctrl_config_msg.num_config_elements = 3;
            buffer_req      = telemetry_info->dma_logical_buffer;
            buffer_ptr      = bcmcth_mon_telemetry_st_msg_ctrl_config_pack(
                                  buffer_req,
                                  &ctrl_config_msg);
            buffer_len      = buffer_ptr - buffer_req;

            rv = bcmcth_mon_telemetry_send_receive(unit,
                    telemetry_info->ha_mem->st_uc_num[i],
                    MCS_MSG_SUBCLASS_ST_CONFIG,
                    buffer_len, 0,
                    MCS_MSG_SUBCLASS_ST_CONFIG_REPLY,
                    &reply_len,
                    MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);

            if (SHR_FAILURE(rv) || (reply_len != 0)) {
                /* Update oper state to APP communication failure. */
                if (out && out->count == 0) {
                    bcmcth_mon_st_control_oper_state_set(unit,
                            BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_COMMUNICATION_FAILURE,
                            out);
                }
                rv = SHR_E_NONE;
                SHR_ERR_EXIT(rv);
            }
            /* Replay the Instance configuration if exist. */
            SHR_IF_ERR_EXIT(bcmcth_mon_st_instance_replay(unit,
                                                        entry->max_export_len));
        }
    }

    if (out && (out->count == 1) &&
            ((out->field[(out->count) - 1]->data ==
              BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_COMMUNICATION_FAILURE) ||
             (out->field[(out->count) - 1]->data ==
              BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_NOT_INITIALIZED))) {
        bcmcth_mon_st_control_oper_state_clear(unit, out);
        bcmcth_mon_telemetry_control_oper_fields_update(unit, entry, out);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_st_instance_control_oper_state_set(
    int unit)
{
    uint32_t    i = 0, sid = 0;
    uint32_t    key = 0;
    uint32_t    min = 0, max = 0;
    bcmlrd_field_def_t  def;

    SHR_FUNC_ENTER(unit);

    sid = MON_TELEMETRY_INSTANCEt;
    key = MON_TELEMETRY_INSTANCEt_MON_TELEMETRY_INSTANCE_IDf;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_table_field_def_get(unit, sid, key, &def));
    min = TELEMETRY_FIELD_MIN(def);
    max = TELEMETRY_FIELD_MAX(def);

    for (i = min; i <= max; i++) {
        SHR_IF_ERR_EXIT(bcmcth_mon_st_instance_oper_state_update(unit, i,
            BCMLTD_COMMON_TELEMETRY_INSTANCE_STATE_T_T_CONTROL_ENTRY_ERROR));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_st_detach(
    int unit,
    bcmltd_fields_t *out)
{

    uint16_t reply_len = 0;
    int rv = SHR_E_NONE;
    bcmbd_ukernel_info_t uc_info;
    uint32_t oper_state = 0;
    int i, num_uc = 0;

    bcmcth_mon_telemetry_info_t *telemetry_info = MON_TELEMETRY_INFO(unit);

    SHR_FUNC_ENTER(unit);

    oper_state =
        BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_APP_COMMUNICATION_FAILURE;
    num_uc = bcmbd_mcs_num_uc(unit);
    for (i = 0; i < num_uc; i++) {
        rv = SHR_E_NONE;
        sal_memset(&uc_info, 0, sizeof(bcmbd_ukernel_info_t));
        if (telemetry_info->ha_mem->st_initialized[i]) {
            /* Shut down the streaming telemetry APP */
            rv = bcmbd_mcs_uc_status(unit, telemetry_info->ha_mem->st_uc_num[i],
                                     &uc_info);
            if (rv == SHR_E_NONE && ((sal_strcmp(uc_info.status,"OK")) == 0)) {
                rv = bcmcth_mon_telemetry_send_receive(unit,
                        telemetry_info->ha_mem->st_uc_num[i],
                        MCS_MSG_SUBCLASS_ST_SHUTDOWN,
                        0, 0,
                        MCS_MSG_SUBCLASS_ST_SHUTDOWN_REPLY,
                        &reply_len,
                        MCS_SHR_ST_MAX_UC_MSG_TIMEOUT);
                if (SHR_SUCCESS(rv) && (reply_len != 0)) {
                    if (out && out->count == 0) {
                        bcmcth_mon_st_control_oper_state_set(unit, oper_state,
                                out);
                    }
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                telemetry_info->ha_mem->st_initialized[i] = FALSE;
                telemetry_info->ha_mem->st_uc_num[i] =
                    BCMMON_TELEMETRY_INVALID_CORE;
                telemetry_info->ha_mem->uc_loaded_count--;
                /* Update the Instance oper state if exist. */
                SHR_IF_ERR_EXIT(bcmcth_mon_st_instance_control_oper_state_set(
                    unit));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}
