/*! \file telemetry.c
 *
 * BCM level APIs for Telemetry.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm/types.h>
#include <shared/gport.h>
#include <bcm/telemetry.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/telemetry_int.h>
#include <bcm_int/ltsw/collector.h>

#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/port.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcm_int/ltsw/issu.h>

#define BSL_LOG_MODULE BSL_LS_BCM_TELEMETRY

static bcmi_telemetry_info_t telemetry_info[BCM_MAX_NUM_UNITS];

#define TELEMETRY_INFO(unit) (&telemetry_info[unit])

#define TELEMETRY_INIT_CHECK(unit) \
    do { \
        if (TELEMETRY_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define TELEMETRY_LOCK(unit) \
    do { \
        if (TELEMETRY_INFO(unit)->lock) { \
            sal_mutex_take(TELEMETRY_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define TELEMETRY_UNLOCK(unit) \
    do { \
        if (TELEMETRY_INFO(unit)->lock) { \
            sal_mutex_give(TELEMETRY_INFO(unit)->lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define TELEMETRY_OBJ_ID_BMP(unit)               \
    (telemetry_info[unit].ha_info->object_id_bmp.w)

#define TELEMETRY_OBJ_ID_BMP_SET(unit, id)       \
    SHR_BITSET(TELEMETRY_OBJ_ID_BMP(unit),id)

#define TELEMETRY_OBJ_ID_BMP_GET(unit, id)       \
    SHR_BITGET(TELEMETRY_OBJ_ID_BMP(unit),id)

#define TELEMETRY_OBJ_ID_BMP_CLEAR(unit, id)       \
    SHR_BITCLR(TELEMETRY_OBJ_ID_BMP(unit),id)

#define TELEMETRY_INST_IDX_BMP(unit)               \
    (telemetry_info[unit].ha_info->instance_idx_bmp.w)

#define TELEMETRY_INST_IDX_BMP_SET(unit, idx)       \
    SHR_BITSET(TELEMETRY_INST_IDX_BMP(unit),idx)

#define TELEMETRY_INST_ID_BMP_GET(unit, id)       \
    SHR_BITGET(TELEMETRY_INST_ID_BMP(unit),id)

#define TELEMETRY_INST_IDX_BMP_CLEAR(unit, idx)       \
    SHR_BITCLR(TELEMETRY_INST_IDX_BMP(unit),idx)

/* Mapping of Instance ID to internal index. */
#define TELEMETRY_INST_ID_TO_INDEX_MAP(unit)            \
    (telemetry_info[unit].ha_info->instance_id_to_idx_map)

#define TELEMETRY_INST_ID_MAP(unit, id)              \
    (*(TELEMETRY_INST_ID_TO_INDEX_MAP(unit) + id))

#define TELEMETRY_OBJ_COUNT(unit)               \
    (telemetry_info[unit].ha_info->config_count)

#define _BCM_TELEMETRY_DEF_VALUE 0xFFFFFFFF
#define _BCM_TELEMETRY_DEF_MAX_EXPORT_PKT_LEN 1500
#define _BCM_TELEMETRY_DEF_MAX_PORTS 0

/******************************************************************************
 * Private Functions
 */

static int
_bcm_ltsw_telemetry_is_instance_exists(int unit,
                           int telemetry_instance_id,
                           int *instance_index,
                           int *first_free_index)
{
    int i = 0, rv = BCM_E_NONE, match_found = 0;


    for (i = 0; i< _BCM_TELEMETRY_MAX_INSTANCES; i++) {
       if(TELEMETRY_INST_ID_MAP(unit,telemetry_instance_id) ==
          telemetry_instance_id)
       {
           match_found = 1;
           *instance_index = i;
           break;
       } else if (TELEMETRY_INST_ID_MAP(unit,telemetry_instance_id) ==
                  _BCM_TELEMETRY_INVALID_VALUE) {
           *first_free_index = i;
       }
    }

    if(match_found == 0)
    {
        rv = BCM_E_NOT_FOUND;
    }
    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Instance exist check: %s"
                        " (Telemetry instance=0x%x).\n"), unit, __FUNCTION__,
                         telemetry_instance_id));
    return rv;
}

static int
_bcm_ltsw_telemetry_object_bk_clear(
        int unit,
        int obj_id)
{

    int rv = BCM_E_NONE;

    TELEMETRY_OBJ_ID_BMP_CLEAR(unit, obj_id);
    TELEMETRY_OBJ_COUNT(unit)--;

    sal_memset(&(telemetry_info[unit].ha_info->telemetry_config[obj_id]), 0,
               sizeof(bcm_telemetry_config_t));

    return rv;
}

static int
_bcm_ltsw_telemetry_object_add(
        int unit,
        bcm_telemetry_config_t *telemetry_config)
{

    bcmlt_entry_handle_t obj_entry_hdl = BCMLT_INVALID_HDL;
    int obj_id = 0, j, rv = SHR_E_NONE;
    char *port_name;
    uint64_t tmp_64_val;
    const char* symbol = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_OBJECTs, &obj_entry_hdl));

    obj_id = BCM_GPORT_LOCAL_GET(telemetry_config->gport);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(obj_entry_hdl,
                               MON_TELEMETRY_OBJECT_IDs,
                               obj_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(obj_entry_hdl, PORT_IDs,
                               BCM_GPORT_LOCAL_GET(telemetry_config->gport)));
    port_name = (char*)telemetry_config->port_name;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(obj_entry_hdl, PORT_NAME_LENs,
                               sal_strlen(port_name)));
    for (j = 0; j < sal_strlen(port_name); j++) {
        tmp_64_val = telemetry_config->port_name[j];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_add(obj_entry_hdl, PORT_NAMEs, j,
                                         &tmp_64_val, 1));
    }

    tmp_64_val = '\0';
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(obj_entry_hdl, PORT_NAMEs, j,
                                     &tmp_64_val, 1));
    for (j = 0; j < (bcmTelemetryObjectTypeCount - 1); j++) {

        if (telemetry_config->object_type[j] ==
                bcmTelemetryObjectTypeNone) {
            symbol = "NONE";
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(obj_entry_hdl,
                                                    TELEMETRY_STATs, j,
                                                    &symbol, 1));
        } else if (telemetry_config->object_type[j] ==
                bcmTelemetryObjectTypeIntfIngress) {
            symbol = "INTF_INGRESS";
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(obj_entry_hdl,
                                                    TELEMETRY_STATs, j,
                                                    &symbol, 1));
        } else if (telemetry_config->object_type[j] ==
                bcmTelemetryObjectTypeIntfEgress) {
            symbol = "INTF_EGRESS";
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(obj_entry_hdl,
                                                    TELEMETRY_STATs, j,
                                                    &symbol, 1));
        } else if (telemetry_config->object_type[j] ==
                bcmTelemetryObjectTypeIntfEgressQueue) {
            symbol = "INTF_EGRESS_QUEUE";
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(obj_entry_hdl,
                                                    TELEMETRY_STATs, j,
                                                    &symbol, 1));
        } else if (telemetry_config->object_type[j] ==
                bcmTelemetryObjectTypeIntfIngressError) {
            symbol = "INTF_INGRESS_ERRORS";
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_symbol_add(obj_entry_hdl,
                                                    TELEMETRY_STATs, j,
                                                    &symbol, 1));
        }
    }
    rv = bcmi_lt_entry_commit(unit, obj_entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE) {
        SHR_IF_ERR_MSG_EXIT(rv,
            (BSL_META_U(unit,
                           "TELEMETRY(unit %d) Error: Object %d "
                           "insert failed with error %d\n"), unit, obj_id, rv));
    }
    TELEMETRY_OBJ_ID_BMP_SET(unit,
            BCM_GPORT_LOCAL_GET(telemetry_config->gport));
    TELEMETRY_OBJ_COUNT(unit)++;
    sal_memcpy(&(telemetry_info[unit].ha_info->telemetry_config[obj_id]), telemetry_config,
               sizeof(bcm_telemetry_config_t));

exit:
    if (BCMLT_INVALID_HDL != obj_entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(obj_entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
_bcm_ltsw_telemetry_object_config_handle(
        int unit,
        int config_count,
        bcm_telemetry_config_t *telemetry_config_list,
        bool is_update, bool is_delete,
        SHR_BITDCL *obj_del_bmp)
{
    bcmlt_entry_handle_t obj_entry_hdl = BCMLT_INVALID_HDL;
    bcm_telemetry_config_t *telemetry_config, old_telemetry_config;
    bool match = FALSE, is_cfg_same = FALSE;
    uint64_t tmp_64_val;
    int i, j, k;
    const char* symbol = NULL;
    uint8_t port_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_OBJECTs, &obj_entry_hdl));

    if (is_delete) {
        for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
            if (TELEMETRY_OBJ_ID_BMP_GET(unit, i)) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(obj_entry_hdl,
                                           MON_TELEMETRY_OBJECT_IDs, i));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, obj_entry_hdl,
                                          BCMLT_OPCODE_DELETE,
                                          BCMLT_PRIORITY_NORMAL));
                SHR_IF_ERR_EXIT
                    (_bcm_ltsw_telemetry_object_bk_clear(unit, i));
            }
        }
    } else {
        if (is_update) {

            /* Check and update deleted objects from previous configuration */
            for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
                if (!TELEMETRY_OBJ_ID_BMP_GET(unit, i)) {
                    continue;
                }
                match = FALSE;
                is_cfg_same = FALSE;
                memcpy(&old_telemetry_config,
                       &(telemetry_info[unit].ha_info->telemetry_config[i]),
                       sizeof(bcmi_telemetry_config_t));
                if (!BCM_GPORT_IS_LOCAL(old_telemetry_config.gport)) {
                    continue;
                }
                for (j = 0; j < config_count; j++) {
                    telemetry_config = &(telemetry_config_list[j]);
                    if(BCM_GPORT_LOCAL_GET(old_telemetry_config.gport) ==
                            BCM_GPORT_LOCAL_GET(telemetry_config->gport)) {
                        match = TRUE;
                        if (!sal_memcmp(telemetry_config,
                                    &old_telemetry_config,
                                    sizeof(bcm_telemetry_config_t))) {
                            is_cfg_same = TRUE;
                        }
                        break;
                    }
                }
                /* If match found then it may be a candidate for update
                 * else it should be deleted.
                 */
                if (match == TRUE) {
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(obj_entry_hdl,
                                 MON_TELEMETRY_OBJECT_IDs,
                                 BCM_GPORT_LOCAL_GET(telemetry_config->gport)));
                    if (sal_strcmp((char*)old_telemetry_config.port_name,
                                (char*)telemetry_config->port_name)) {
                        port_len = sal_strlen(
                                       (char*)telemetry_config->port_name);
                        SHR_IF_ERR_EXIT
                            (bcmlt_entry_field_add(obj_entry_hdl,
                                                   PORT_NAME_LENs,
                                                   port_len));
                        for (k = 0; k < port_len; k++) {
                            tmp_64_val = telemetry_config->port_name[k];
                            SHR_IF_ERR_EXIT
                                (bcmlt_entry_field_array_add(obj_entry_hdl,
                                                             PORT_NAMEs, k,
                                                             &tmp_64_val,
                                                             1));
                        }
                    }
                    if (sal_memcmp(telemetry_config->object_type,
                                old_telemetry_config.object_type,
                                (sizeof(bcm_telemetry_object_type_t) *
                                 bcmTelemetryObjectTypeCount))) {
                        for (k = 0; k < (bcmTelemetryObjectTypeCount - 1);
                                k++) {

                            if (telemetry_config->object_type[k] ==
                                    bcmTelemetryObjectTypeNone) {
                                symbol = "NONE";
                                SHR_IF_ERR_EXIT
                                    (bcmlt_entry_field_array_symbol_add(
                                                             obj_entry_hdl,
                                                             TELEMETRY_STATs, k,
                                                             &symbol, 1));
                            } else if (telemetry_config->object_type[k] ==
                                    bcmTelemetryObjectTypeIntfIngress) {
                                symbol = "INTF_INGRESS";
                                SHR_IF_ERR_EXIT
                                    (bcmlt_entry_field_array_symbol_add(
                                                             obj_entry_hdl,
                                                             TELEMETRY_STATs, k,
                                                             &symbol, 1));
                            } else if (telemetry_config->object_type[k] ==
                                    bcmTelemetryObjectTypeIntfEgress) {
                                symbol = "INTF_EGRESS";
                                SHR_IF_ERR_EXIT
                                    (bcmlt_entry_field_array_symbol_add(
                                                             obj_entry_hdl,
                                                             TELEMETRY_STATs, k,
                                                             &symbol, 1));
                            } else if (telemetry_config->object_type[k] ==
                                    bcmTelemetryObjectTypeIntfEgressQueue) {
                                symbol = "INTF_EGRESS_QUEUE";
                                SHR_IF_ERR_EXIT
                                    (bcmlt_entry_field_array_symbol_add(
                                                             obj_entry_hdl,
                                                             TELEMETRY_STATs, k,
                                                             &symbol, 1));
                            } else if (telemetry_config->object_type[k] ==
                                    bcmTelemetryObjectTypeIntfIngressError)
                            {
                                symbol = "INTF_INGRESS_ERRORS";
                                SHR_IF_ERR_EXIT
                                    (bcmlt_entry_field_array_symbol_add(
                                                             obj_entry_hdl,
                                                             TELEMETRY_STATs, k,
                                                             &symbol, 1));
                            }
                        }

                    }
                    if (!is_cfg_same) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_lt_entry_commit(unit, obj_entry_hdl,
                                                  BCMLT_OPCODE_UPDATE,
                                                  BCMLT_PRIORITY_NORMAL));
                    }
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmlt_entry_field_add(obj_entry_hdl,
                                               MON_TELEMETRY_OBJECT_IDs,
                                               BCM_GPORT_LOCAL_GET(
                                               old_telemetry_config.gport)));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_commit(unit, obj_entry_hdl,
                                              BCMLT_OPCODE_DELETE,
                                              BCMLT_PRIORITY_NORMAL));
                    SHR_BITSET(obj_del_bmp,
                            BCM_GPORT_LOCAL_GET(old_telemetry_config.gport));
                    SHR_IF_ERR_EXIT
                        (_bcm_ltsw_telemetry_object_bk_clear(unit,
                        BCM_GPORT_LOCAL_GET(old_telemetry_config.gport)));
                }
            }
            /* Check and update newly added objects from previous
             * configuration
             */
            for (i = 0; i < config_count; i++) {
                telemetry_config = &(telemetry_config_list[i]);
                match = FALSE;

                for (j = 0; j < _BCM_TELEMETRY_MAX_OBJECTS; j++) {
                    if (!TELEMETRY_OBJ_ID_BMP_GET(unit, j)) {
                        continue;
                    }
                    memcpy(&old_telemetry_config,
                        &(telemetry_info[unit].ha_info->telemetry_config[j]),
                       sizeof(bcmi_telemetry_config_t));
                    if(BCM_GPORT_LOCAL_GET(old_telemetry_config.gport) ==
                            BCM_GPORT_LOCAL_GET(telemetry_config->gport)) {
                        match = TRUE;
                        break;
                    }
                }
                /* If match not found then it is a candidate for addition */
                if (match == FALSE) {
                    SHR_IF_ERR_EXIT
                       (_bcm_ltsw_telemetry_object_add(unit, telemetry_config));
                }
            }
        } else {
            for (i = 0; i < config_count; i++) {
                telemetry_config = &(telemetry_config_list[i]);
                SHR_IF_ERR_EXIT
                    (_bcm_ltsw_telemetry_object_add(unit, telemetry_config));
            }
        }
    }
exit:
    if (BCMLT_INVALID_HDL != obj_entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(obj_entry_hdl);
    }
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public Functions
 */

/*!
 * \brief De-initialize the Telemetry module.
 *
 * Free Telemetry module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_telemetry_detach(int unit)
{
    int lock = 0, rv = SHR_E_NONE;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (TELEMETRY_INFO(unit)->initialized == 0) {
        SHR_EXIT();
    }

    TELEMETRY_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_CONTROLs, &entry_hdl));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    }
    TELEMETRY_INFO(unit)->initialized = 0;
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the Telemetry module.
 *
 * Initialize Telemetry database and Flowtracker related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_telemetry_init(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t lookup_hdl = BCMLT_INVALID_HDL;
    uint64_t oper_state;
    int i;
    void *ptr = NULL;
    uint32_t ha_req_size, ha_alloc_size;
    int rv = SHR_E_NONE;
    uint32_t max_export_len = 0;
    uint32_t max_num_ports = 0;
    bool     is_telemetry_initialized = 0;
    int warmboot = false;

    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
        (bcm_ltsw_telemetry_detach(unit));

    if (TELEMETRY_INFO(unit)->lock == NULL) {
        TELEMETRY_INFO(unit)->lock = sal_mutex_create("bcmLtswTelemetryMutex");
        SHR_NULL_CHECK(TELEMETRY_INFO(unit)->lock, SHR_E_MEMORY);
    }

    warmboot = bcmi_warmboot_get(unit);

    ha_req_size = sizeof(bcmi_telemetry_ha_info_t);
    ha_alloc_size = ha_req_size;

    ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_TELEMETRY, 0,
                                 "bcmTelemetry",
                                 &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Report the structure array to ISSU. */
    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_TELEMETRY,
                                           TELEMETRY_SUB_COMP_ID_HA_INFO,
                                           0,
                                           sizeof(bcmi_telemetry_ha_info_t),
                                           1,
                                           BCMI_TELEMETRY_HA_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_CONTROLs, &lookup_hdl));
    rv = bcmlt_entry_commit(lookup_hdl, BCMLT_OPCODE_LOOKUP,
            BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(lookup_hdl, OPERATIONAL_STATEs,
                                   &oper_state));
        if (oper_state == BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_VALID) {
            is_telemetry_initialized = 1;
        }
    }

    if ((!bcmi_warmboot_get(unit)) && (!is_telemetry_initialized)) {
        sal_memset(ptr, 0, ha_req_size);
    }
    telemetry_info[unit].ha_info = ptr;

    if (!bcmi_warmboot_get(unit)) {
        if (!is_telemetry_initialized) {
            for (i = 0; i < _BCM_TELEMETRY_MAX_INSTANCES; i++) {
                TELEMETRY_INST_ID_MAP(unit,i) = _BCM_TELEMETRY_INVALID_VALUE;
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(unit, MON_TELEMETRY_CONTROLs, &entry_hdl));

        max_export_len = bcmi_ltsw_property_get(unit,
                                  BCMI_CPN_TELEMETRY_EXPORT_MAX_PACKET_SIZE,
                                  _BCM_TELEMETRY_DEF_MAX_EXPORT_PKT_LEN);
        if (max_export_len != _BCM_TELEMETRY_DEF_MAX_EXPORT_PKT_LEN) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MAX_EXPORT_LENGTHs,
                                       (uint64_t)max_export_len));
        }
        max_num_ports = bcmi_ltsw_property_get(unit,
                                  BCMI_CPN_TELEMETRY_MAX_PORTS_MONITOR,
                                  _BCM_TELEMETRY_DEF_MAX_PORTS);

        if (max_num_ports != _BCM_TELEMETRY_DEF_MAX_PORTS) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MAX_NUM_PORTSs,
                                       (uint64_t)max_num_ports));
        }

        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, STREAMING_TELEMETRYs,
                                       (uint64_t)1));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_INSERT,
                                      BCMLT_PRIORITY_NORMAL));
        }  else if (rv != SHR_E_NONE) {
            SHR_IF_ERR_EXIT(rv);
        } else {
            /* Toggle the oper state to start the ST APP */
            if (oper_state != BCMLTD_COMMON_TELEMETRY_CONTROL_STATE_T_T_VALID) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, STREAMING_TELEMETRYs,
                                           (uint64_t)0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, entry_hdl,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL));

                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_add(entry_hdl, STREAMING_TELEMETRYs,
                                           (uint64_t)1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit, entry_hdl,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL));
            }
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(lookup_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(lookup_hdl, OPERATIONAL_STATEs, &oper_state));

        if (oper_state == 0) {
            TELEMETRY_INFO(unit)->initialized = 1;
        }
    } else {
        if (is_telemetry_initialized == 1) {
            TELEMETRY_INFO(unit)->initialized = 1;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (TELEMETRY_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(TELEMETRY_INFO(unit)->lock);
            TELEMETRY_INFO(unit)->lock = NULL;
        }
        if (!warmboot) {
            (void)bcmi_ltsw_ha_mem_free(unit, ptr);
            ptr = NULL;
        }
    }

    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (BCMLT_INVALID_HDL != lookup_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(lookup_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_config_get
 * Purpose:
 *      Fetches a Telemetry application instance information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be fetched
 *      core                - (IN) FW core on which this instance is being run
 *      max_count           - (IN) Maximum number of telemetry objects need
 *                                 to be fetched
 *      telemetry_config_list - (OUT)  Telemetry objects information
 *      config_count        - (OUT) Actual number of telemetry objects to be
 *                                 associated with this instance.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_config_get(
        int unit,
        int telemetry_instance,
        int core,
        int max_count,
        bcm_telemetry_config_t *telemetry_config_list,
        int *config_count)
{

    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t obj_entry_hdl = BCMLT_INVALID_HDL;
    int telemetry_lock = 0, i = 0, j = 0, obj_id = 0;
    uint64_t value, array_value;
    uint32_t r_cnt;
    bcm_telemetry_config_t *telemetry_config;
    int rv = BCM_E_NONE;
    int first_free_index = 0, cur_instance_index = 0;
    const char *telemetry_stat_symbols[(bcmTelemetryObjectTypeCount -1)];
    uint32_t r_val_cnt = 0;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    telemetry_lock = 1;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               (uint64_t)telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NUM_OBJECTSs, &value));

    *config_count = (int) value;

    if (max_count == 0)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_OBJECTs, &obj_entry_hdl));

    if (telemetry_config_list != NULL) {
        for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
            if (!TELEMETRY_OBJ_ID_BMP_GET(unit, i)) {
                continue;
            }
            telemetry_config = &(telemetry_config_list[obj_id]);

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_get(entry_hdl,
                                             MON_TELEMETRY_OBJECT_IDs, i,
                                             &value, 1, &r_cnt));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(obj_entry_hdl, MON_TELEMETRY_OBJECT_IDs,
                                       value));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(obj_entry_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(obj_entry_hdl, PORT_IDs, &value));
            BCM_GPORT_LOCAL_SET(telemetry_config->gport, value);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(obj_entry_hdl, PORT_NAME_LENs, &value));
            for (j = 0; j < value; j++) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_get(obj_entry_hdl, PORT_NAMEs, j,
                                                 &array_value, 1, &r_cnt));
                telemetry_config->port_name[j] = array_value;
            }
            telemetry_config->port_name[j] = '\0';

            for (j = 0; j < (bcmTelemetryObjectTypeCount -1); j++) {
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_symbol_get(obj_entry_hdl,
                                               TELEMETRY_STATs,
                                               0,telemetry_stat_symbols,
                                               (bcmTelemetryObjectTypeCount -1),
                                               &r_val_cnt));

                if (!sal_strcmp(telemetry_stat_symbols[j], "NONE")) {
                    telemetry_config->object_type[j] =
                        bcmTelemetryObjectTypeNone;
                } else if (!sal_strcmp(telemetry_stat_symbols[j],
                           "INTF_INGRESS")) {
                    telemetry_config->object_type[j] =
                        bcmTelemetryObjectTypeIntfIngress;
                } else if (!sal_strcmp(telemetry_stat_symbols[j],
                           "INTF_EGRESS")) {
                    telemetry_config->object_type[j] =
                        bcmTelemetryObjectTypeIntfEgress;
                } else if (!sal_strcmp(telemetry_stat_symbols[j],
                           "INTF_EGRESS_QUEUE")) {
                    telemetry_config->object_type[j] =
                        bcmTelemetryObjectTypeIntfEgressQueue;
                } else if (!sal_strcmp(telemetry_stat_symbols[j],
                           "INTF_INGRESS_ERRORS")) {
                    telemetry_config->object_type[j] =
                        bcmTelemetryObjectTypeIntfIngressError;
                }
            }
            obj_id++;
        }
    }
exit:
    if (BCMLT_INVALID_HDL != obj_entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(obj_entry_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_config_set
 * Purpose:
 *      Configures a Telemetry application instance with specified
 *      number of telemetry objects information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be created
 *      core                - (IN) FW core on which this instance need to be run
 *      config_count        - (IN)  Number of telemetry objects to be
 *                                     associated with this instance.
 *      telemetry_config_list  - (IN)  Telemetry objects information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_config_set(
        int unit,
        int telemetry_instance,
        int core,
        int config_count,
        bcm_telemetry_config_t *telemetry_config_list)
{

    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int telemetry_lock = 0;
    int i;
    bcm_port_t port;
    bcm_gport_t gport;
    bool is_update = FALSE, is_delete = FALSE;
    int index = 0, first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    uint64_t tmp_64_val;
    SHR_BITDCL *object_del_bmp = NULL;
    uint32_t alloc_size;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    telemetry_lock = 1;

    if (config_count < 0)
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid "
                            " configuration count passed\n"), unit));
        return BCM_E_PARAM;
    }

    for (i = 0; i < config_count; i++) {
        gport = telemetry_config_list[i].gport;
        if (!BCM_GPORT_IS_LOCAL(gport)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: only local "
                                " type gport add is supported, gport %d "
                                "config index %d\n"),
                                unit, gport, i));
            return BCM_E_CONFIG;
        }
        port = BCM_GPORT_LOCAL_GET(gport);
        if ((!bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_ETH)) ||
            bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid port=%d\n"),
                     unit, port));
            return BCM_E_PARAM;
        }
    }

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);
    if(rv == BCM_E_NOT_FOUND) {
        if (first_free_index == _BCM_TELEMETRY_INVALID_VALUE) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                      (BSL_META_U(unit,
                      "TELEMETRY(unit %d) Error: Not allowed to "
                      "configure more than supported %d telemetry instances\n"),
                      unit, _BCM_TELEMETRY_MAX_INSTANCES));
            return BCM_E_RESOURCE;
        }
        index = first_free_index;
        rv = BCM_E_NONE;
    } else {
        is_update = TRUE;
        index = cur_instance_index;
        if (config_count == 0) {
            is_delete = TRUE;
        }
    }


    /* Checks ok, now do the LT Programming */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, CORE_INSTANCEs, core));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, NUM_OBJECTSs, config_count));


    alloc_size = SHR_BITALLOCSIZE(_BCM_TELEMETRY_MAX_OBJECTS);
    object_del_bmp = sal_alloc(alloc_size, "ltswTelemetryiDelObjBmp");
    SHR_NULL_CHECK(object_del_bmp, SHR_E_MEMORY);
    sal_memset(object_del_bmp, 0, alloc_size);

    SHR_IF_ERR_EXIT
    (_bcm_ltsw_telemetry_object_config_handle(unit, config_count,
                                              telemetry_config_list,
                                              is_update, is_delete,
                                              object_del_bmp));

    if (!is_update) {
        for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
            if (TELEMETRY_OBJ_ID_BMP_GET(unit, i)) {
                tmp_64_val = i;
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_add(entry_hdl,
                                                 MON_TELEMETRY_OBJECT_IDs, i,
                                                 &tmp_64_val, 1));
            }
        }
    } else {
        for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
            if (TELEMETRY_OBJ_ID_BMP_GET(unit, i)) {
                tmp_64_val = i;
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_add(entry_hdl,
                                                 MON_TELEMETRY_OBJECT_IDs, i,
                                                 &tmp_64_val, 1));
            }
        }
        for (i = 0; i < _BCM_TELEMETRY_MAX_OBJECTS; i++) {
            if (SHR_BITGET(object_del_bmp, i)) {
                tmp_64_val = _BCM_TELEMETRY_DEF_VALUE;
                SHR_IF_ERR_EXIT
                    (bcmlt_entry_field_array_add(entry_hdl,
                                                 MON_TELEMETRY_OBJECT_IDs, i,
                                                 &tmp_64_val, 1));
            }
        }
    }

    if (!is_update) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        TELEMETRY_INST_IDX_BMP_SET(unit, index);
        TELEMETRY_INST_ID_MAP(unit,index) = telemetry_instance;
    } else {
        if (!is_delete) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));
            TELEMETRY_INST_IDX_BMP_CLEAR(unit, index);
            TELEMETRY_INST_ID_MAP(unit,index) = _BCM_TELEMETRY_INVALID_VALUE;
        }
    }
exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (object_del_bmp != NULL) {
        SHR_FREE(object_del_bmp);
    }

    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_export_config_set
 * Purpose:
 *      Configures the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance to be associated
 *      collector_id        - (IN) Colletor ID with which telemery instance
 *                                 needs to be associated
 *      export_profile_id   - (IN) Colletor export profile ID with which
 *                                 telemery instance needs to be associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_export_config_set(
        int unit,
        int telemetry_instance,
        bcm_collector_t collector_id,
        int export_profile_id)
{
    bcmlt_entry_handle_t lookup_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    int telemetry_lock = 0;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    uint64_t value = 0;
    bcm_ltsw_collector_transport_type_t coll_type;
    uint32_t collector_int_id;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Get collector info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    /* Get export profile info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_export_profile_get(unit, export_profile_id,
                                               &export_profile_info));

    if (export_profile_info.wire_format != bcmCollectorWireFormatProtoBuf) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid "
                            " export profile wire format\n"), unit));
        return SHR_E_PARAM;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &lookup_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(lookup_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               (uint64_t)telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(lookup_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(lookup_hdl, MON_COLLECTOR_IDs, &value));

    if ((value != _BCM_TELEMETRY_DEF_VALUE) &&
        (value != collector_id))
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Only "
                            " one collector is supported\n"), unit));
        return SHR_E_CONFIG;
    }
    value = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(lookup_hdl, MON_EXPORT_PROFILE_IDs, &value));
    if ((value != _BCM_TELEMETRY_DEF_VALUE) &&
        (value != export_profile_id))
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Only "
                            " one export profile is supported\n"), unit));
        return SHR_E_CONFIG;
    }
    /*
     * Check if I am the only user for this collector. Else
     * return error.
     */
    if (bcmi_ltsw_collector_check_user_is_other(unit, collector_id,
                LTSW_COLLECTOR_EXPORT_APP_FT_PROTOBUF)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Collector (ID:%d) is "
                            "associated to another wire format.\n"),
                 unit, collector_id));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    telemetry_lock = 1;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               (uint64_t)telemetry_instance));

    /* Get the collector internal ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_internal_id_get(unit, collector_id,
                                             &collector_int_id, &coll_type));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IDs,
                               collector_int_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_EXPORT_PROFILE_IDs,
                               export_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_ref_count_update(unit, collector_id,
                                                         1));
    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_export_profile_ref_count_update(unit,
                    export_profile_id, 1));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_user_update(unit,
                collector_id, LTSW_COLLECTOR_EXPORT_APP_FT_PROTOBUF));


exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (BCMLT_INVALID_HDL != lookup_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(lookup_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_export_config_get
 * Purpose:
 *      Fetches the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance whose details
 *                                 needs to be fetched
 *      collector_id        - (OUT) Pointer to colletor ID with which telemery
 *                                  instance has been associated
 *      export_profile_id   - (OUT) Pointer to colletor export profile ID with
 *                                  which telemery instance has been associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_export_config_get(
        int unit,
        int telemetry_instance,
        bcm_collector_t *collector_id,
        int *export_profile_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    int telemetry_lock = 0;
    uint64_t value;
    bcm_ltsw_collector_transport_type_t type;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    telemetry_lock = 1;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MON_COLLECTOR_IDs, &value));

    type = LTSW_COLLECTOR_TYPE_IPV4_UDP;

    /* Convert to external ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_collector_external_id_get(unit, value, type,
                                             collector_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MON_EXPORT_PROFILE_IDs,
                               &value));
    *export_profile_id = (int) value;

exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_export_config_delete
 * Purpose:
 *      Deletes the Telemetry instance export information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) The ID of telemetry instance whose
 *                                 association needs to be deleted
 *      collector_id        - (OUT) Pointer to colletor ID with which telemery
 *                                  instance has been associated
 *      export_profile_id   - (OUT) Pointer to colletor export profile ID with
 *                                  which telemery instance has been associated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_export_config_delete(
        int unit,
        int telemetry_instance,
        bcm_collector_t collector_id,
        int export_profile_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    bcm_collector_t coll_id;
    int profile_id;
    int telemetry_lock = 0;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Check if the instance is mapped to the correct collector and export
     * profile
     */
    SHR_IF_ERR_EXIT(
            bcm_ltsw_telemetry_export_config_get(unit, telemetry_instance,
                &coll_id, &profile_id));
    if ((collector_id != coll_id) || (export_profile_id != profile_id)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid collector_id(%d)"
                            " or export_profile_id(%d) \n"),
                 unit, collector_id, export_profile_id));
        return SHR_E_NOT_FOUND;
    }

    telemetry_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_COLLECTOR_IDs,
                               _BCM_TELEMETRY_DEF_VALUE));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_EXPORT_PROFILE_IDs,
                               _BCM_TELEMETRY_DEF_VALUE));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_ref_count_update(unit, collector_id,
                                                         0));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_export_profile_ref_count_update(unit,
                    export_profile_id, 0));

    SHR_IF_ERR_EXIT(bcmi_ltsw_collector_user_update(unit,
                collector_id, LTSW_COLLECTOR_EXPORT_APP_NONE));


exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_instance_export_stats_set
 * Purpose:
 *      Set the telemetry export statistics.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) Telemetry instance ID
 *      collector_id        - (IN) Collector ID
 *      stats               - (IN) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_instance_export_stats_set(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    int telemetry_lock = 0;
    bcm_collector_info_t collector_info;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Get collector info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    telemetry_lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TX_PKTSs,
                               stats->tx_pkts));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TX_PKT_FAILSs,
                               stats->tx_pkt_fails));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));


exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_telemetry_instance_export_stats_get
 * Purpose:
 *      Get the telemetry export statistics.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      telemetry_instance  - (IN)  Telemetry instance ID
 *      collector_id        - (IN)  Collector ID
 *      stats               - (OUT) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_ltsw_telemetry_instance_export_stats_get(
                                   int unit,
                                   int telemetry_instance,
                                   bcm_collector_t collector_id,
                                   bcm_telemetry_instance_export_stats_t *stats)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    int telemetry_lock = 0;
    bcm_collector_info_t collector_info;

    SHR_FUNC_ENTER(unit);

    TELEMETRY_INIT_CHECK(unit);
    TELEMETRY_LOCK(unit);

    rv = _bcm_ltsw_telemetry_is_instance_exists(unit, telemetry_instance,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        SHR_IF_ERR_EXIT(rv);
    }

    /* Get collector info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_collector_get(unit, collector_id, &collector_info));

    telemetry_lock = 1;


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, MON_TELEMETRY_INSTANCEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MON_TELEMETRY_INSTANCE_IDs,
                               telemetry_instance));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TX_PKTSs, &(stats->tx_pkts)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TX_PKT_FAILSs,
                               &(stats->tx_pkt_fails)));

exit:
    if (telemetry_lock == 1) {
        TELEMETRY_UNLOCK(unit);
    }
    if (BCMLT_INVALID_HDL != entry_hdl) {
        /* De-allocate the handle */
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}


