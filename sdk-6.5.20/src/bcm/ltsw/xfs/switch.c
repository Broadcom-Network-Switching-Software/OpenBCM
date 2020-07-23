/*! \file switch.c
 *
 * Switch control interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/switch_int.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/link.h>
#include <bcm_int/ltsw/xfs/switch.h>
#include <bcm_int/ltsw/xfs/switch_defs.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/bst.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/rate.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/ecn.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/ser.h>
#include <bcm_int/ltsw/time.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/hash_output_selection.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/generated/switch_ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/ifa.h>
#include <bcm_int/ltsw/int.h>
#include <bcm_int/ltsw/flexctr.h>

#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE      BSL_LS_BCM_SWITCH

/*! Switch control ID field size */
#define scIdBits            16

/*! Switch control ID mask */
#define scIdMask            ((1 << scIdBits) - 1)
/*! Value type switch control */
#define scValueCtrl         (2 << scIdBits)
/*! Symbol type switch control */
#define scSymbolCtrl        (3 << scIdBits)
/*! Bit type switch control */
#define scBitCtrl           (4 << scIdBits)
/*! Symbol type switch control accessing by key */
#define scKeySymbolCtrl     (5 << scIdBits)
/*! PORT_PKT_CONTROL type switch control */
#define scPortPktCtrl       (6 << scIdBits)

/*! Software value type switch control, this type only update SW database */
#define scSoftwareValueCtrl (7 << scIdBits)

/*! Ignore port ID for device wide configuration */
#define PORT_IGNORE         -1

/*! HA subcomponent id for software value type switch control. */
#define HA_SC_SOFTWARE_SUB_COMP_ID   1

/*! HA subcomponent id for port based software value type switch control. */
#define HA_SCP_SOFTWARE_SUB_COMP_ID  2

/*!
 * \brief Type function definition structure.
 *
 * The \c set implements bcm_switch_control_set() function.
 * The \c get implements bcm_switch_control_get() function.
 */
typedef const struct sc_func_s {

    /*! Switch control type */
    int type;

    /*! Device-wide switch control set function */
    sc_set_f set;

    /*! Device-wide switch control get function */
    sc_get_f get;

} sc_func_t;

/*!
 * \brief Type function definition structure.
 *
 * The \c set implements bcm_switch_control_port_set() function.
 * The \c get implements bcm_switch_control_port_get() function.
 */
typedef const struct scp_func_s {

    /*! Switch control type */
    int type;

    /*! Port base switch control set function */
    scp_set_f set;

    /*! Port base switch control get function */
    scp_get_f get;

} scp_func_t;

/*!
 * \brief Symbol map structure
 *
 * Map a tring to a value.
 */
typedef struct sc_symbol_map_s {

    /*! Enum name. */
    const char *name;

    /*! Enum value. */
    int val;

} sc_symbol_map_t;

/*!
 * \brief Direct value configuration map
 *
 * This structure defines LT field (\c table and \c field), argument tranlation
 * function (\c encode and \c decode), per port or device-wide attributes for a
 * switch control type for sc_sc_value_ctrl_set/get() function to implement
 * the function.
 */
typedef struct sc_value_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table field */
    const char *field;

    /*! Encode argument to setting value */
    sc_arg_encode_f encode;

    /*! Decode getting value to argument */
    sc_arg_decode_f decode;

} sc_value_ctrl_t;

/*!
 * \brief Direct symbol configuration map
 */
typedef struct sc_symbol_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table field */
    const char *field;

    /*! String to enum value map */
    sc_symbol_map_t *maps;

} sc_symbol_ctrl_t;

/*!
 * \brief Symbol control access by key
 */
typedef struct sc_symbol_kctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Logic Table key field */
    const char *kfield;

    /*! Logic Table key field value */
    const char *kval;

    /*! Logic Table field */
    const char *field;

    /*! String to enum value map */
    sc_symbol_map_t *maps;

} sc_symbol_kctrl_t;

/*!
 * \brief Per bit value access map
 *
 * This is for flag type argument. The \c maps defines LT field to flag mapping.
 * A flag bit set means to set the correspondent LT field to 1. Otherwise, set
 * it to 0.
 */
typedef struct sc_bit_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! TRUE - per-port setting, FALSE - device-wide setting */
    bool perport;

    /*! Logic Table name */
    const char *table;

    /*! Field to mask value map */
    sc_symbol_map_t *maps;

} sc_bit_ctrl_t;

/*!
 * \brief Special bool string LT map
 *
 * This structure defines LT \c field's bool string (\c ture_str and
 * \c false_str) map.
 */
typedef struct sc_bool_str_ctrl_s {

    /*! Switch control type */
    bcm_switch_control_t type;

    /*! Logic Table field */
    const char *field;

    /*! Enum string for true */
    const char *true_str;

    /*! Enum string for false */
    const char *false_str;

} sc_bool_str_ctrl_t;

/*!
 * \brief Switch control infomation structure.
 */
typedef struct sc_info_s {

    /* Software value type switch control. */
    bcmint_switch_control_value_map_t *sc_value_map;

    /* Port based software value type switch control. */
    bcmint_switch_control_port_value_map_t *scp_value_map;

} sc_info_t;

/*!
 * \brief Switch drop/trace event symbol map structure.
 *
 * Map a switch drop/trace event enum value to Logical Table symbol string.
 */
typedef struct switch_symbol_map_s {

    /*! Logic Table name */
    const char *table;

    /*! Logic Table symbol string */
    const char *symbol;

    /*! Switch drop/trace event enum val. */
    int val;

} switch_symbol_map_t;

static sc_info_t sc_info[BCM_MAX_NUM_UNITS];

/*! switch_control_set/get index mapping table */
static int sc_idx[bcmSwitch__Count];

/*! switch_control_port_set/get index mapping table */
static int scp_idx[bcmSwitch__Count];

/*! bcm_switch_drop_event_mon_set/get index mapping table */
static int drop_idx[BCM_MAX_NUM_UNITS][bcmPktDropEventCount];

/*! bcm_switch_trace_event_mon_set/get index mapping table */
static int trace_idx[BCM_MAX_NUM_UNITS][bcmPktTraceEventCount];

/*! Value >= max DNA drop event. */
#define MAX_DROP_EVENT 200

/*! bcm_switch_ep_recirc_drop_event_control_t.threshold mapping table */
static int ep_recirc_drop_idx[BCM_MAX_NUM_UNITS][MAX_DROP_EVENT];

/*! bcm_switch_ep_recirc_trace_event_set/get/get_all index mapping table */
static int ep_recirc_trace_idx[BCM_MAX_NUM_UNITS][bcmPktTraceEventCount];

/******************************************************************************
 * Private functions
 */
/* CodeGen map table section begin
 * DO NOT CHANGE anything from here to "CodeGen map table end".
 */
/* CodeGen map table section end */

static int
sc_bcmlt_entry_lookup(int unit, bcmlt_entry_handle_t handle)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_INSERT,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_value_ctrl_set(int unit, int port, sc_value_ctrl_t *ctrl, int arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    if (ctrl->encode) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ctrl->encode(unit, port, arg, &val));
    } else {
        val = (unsigned int)arg;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, ctrl->field, val));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_value_ctrl_get(int unit, int port, sc_value_ctrl_t *ctrl, int *arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(handle, ctrl->field, &val));

    if (ctrl->decode) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ctrl->decode(unit, port, val, arg));
    } else {
        *arg = val & 0xffffffff;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_ctrl_set(int unit, int port, sc_symbol_ctrl_t *ctrl, int arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    i = 0;
    while (ctrl->maps[i].name) {
        if (arg == ctrl->maps[i].val) {
            str = ctrl->maps[i].name;
            break;
        }
        i++;
    }
    /* Configuration enum is not support */
    SHR_NULL_CHECK(str, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->field, str));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_ctrl_get(int unit, int port, sc_symbol_ctrl_t *ctrl, int *arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
    SHR_NULL_CHECK(str, SHR_E_INTERNAL);

    i = 0;
    while (ctrl->maps[i].name) {
        if (!sal_strcasecmp(ctrl->maps[i].name, str)) {
            *arg = ctrl->maps[i].val;
            break;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}


/* Support
 * 1. Multiple entries for a same control type
 * 2. Both 1 bit mask and mask block (consecutive 1s)
 */
static int
sc_bit_ctrl_set(int unit, int port, bcm_switch_control_t type,
                sc_bit_ctrl_t *ctrls, int arg)
{
    int i;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrls, SHR_E_PARAM);

    i= 0;
    while (ctrls[i].table) {
        if (type == ctrls[i].type) {
            int j;
            uint64_t val;
            sc_bit_ctrl_t *ctrl = &ctrls[i];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
            j = 0;
            while (ctrl->maps[j].name) {
                val = (arg & ctrl->maps[j].val) ? 1 : 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(handle, ctrl->maps[j].name, val));
                j++;
            }
            if (ctrl->perport) {
                if (port == PORT_IGNORE) {
                    SHR_IF_ERR_MSG_EXIT
                        (SHR_E_PARAM,
                         (BSL_META_U(unit, "Abnormal port ID %d\n"), port));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(handle, PORT_IDs, port));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_free(handle));
            handle = BCMLT_INVALID_HDL;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

/* Support
 * 1. Multiple entries for a same control type
 * 2. Both 1 bit mask and mask block (consecutive 1s)
 */
static int
sc_bit_ctrl_get(int unit, int port, bcm_switch_control_t type,
                sc_bit_ctrl_t *ctrls, int *arg)
{
    int i;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrls, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;
    i= 0;
    while (ctrls[i].table) {
        if (type == ctrls[i].type) {
            int j;
            uint64_t val;
            sc_bit_ctrl_t *ctrl = &ctrls[i];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
            if (ctrl->perport) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(handle, PORT_IDs, port));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (sc_bcmlt_entry_lookup(unit, handle));
            j = 0;
            while (ctrl->maps[j].name) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(handle, ctrl->maps[j].name, &val));
                if (val) {
                    *arg |= ctrl->maps[j].val;
                }
                j++;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_free(handle));
            handle = BCMLT_INVALID_HDL;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_kctrl_set(int unit, int port, sc_symbol_kctrl_t *ctrl, int arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    i = 0;
    while (ctrl->maps[i].name) {
        if (arg == ctrl->maps[i].val) {
            str = ctrl->maps[i].name;
            break;
        }
        i++;
    }
    /* Configuration enum is not support */
    SHR_NULL_CHECK(str, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->kfield, ctrl->kval));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->field, str));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_symbol_kctrl_get(int unit, int port, sc_symbol_kctrl_t *ctrl, int *arg)
{
    int i;
    const char *str = NULL;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ctrl->table, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(handle, ctrl->kfield, ctrl->kval));
    if (ctrl->perport) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, PORT_IDs, port));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
    SHR_NULL_CHECK(str, SHR_E_INTERNAL);

    i = 0;
    while (ctrl->maps[i].name) {
        if (!sal_strcasecmp(ctrl->maps[i].name, str)) {
            *arg = ctrl->maps[i].val;
            break;
        }
        i++;
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_port_pkt_ctrl_set(int unit, int port, sc_bool_str_ctrl_t *ctrl, int arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(handle, PORT_PKT_CONTROL_IDs, &val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(handle));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, PORT_PKT_CONTROL_IDs, val));

    if (ctrl->true_str && ctrl->false_str) {
        if (arg == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(handle, ctrl->field,
                                              ctrl->true_str));
        } else if (arg == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(handle, ctrl->field,
                                              ctrl->false_str));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (!(ctrl->true_str || ctrl->false_str)){
        val = (unsigned int)arg;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, ctrl->field, val));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, handle, BCMLT_PRIORITY_NORMAL));

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

static int
sc_port_pkt_ctrl_get(int unit, int port, sc_bool_str_ctrl_t *ctrl, int *arg)
{
    uint64_t val;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(handle, PORT_PKT_CONTROL_IDs, &val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(handle));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, PORT_PKT_CONTROL_IDs, val));
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, handle));
    if (ctrl->true_str && ctrl->false_str) {
        const char *str = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(handle, ctrl->field, &str));
        if (!sal_strcasecmp(ctrl->true_str, str)) {
            *arg = 1;
        } else {
            *arg = 0;
        }
    } else if (!(ctrl->true_str || ctrl->false_str)){
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(handle, ctrl->field, &val));
        *arg = val & 0xffffffff;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }
    SHR_FUNC_EXIT();
}

/* CodeGen function section begin
 * Don't change or remove this comments.
 * Each interface is generated by code-gen. The content should be
 * maintained by developer;
 */
static int
sc_bcmSwitchTrunkDynamicSampleRate_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeSampleRate,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicSampleRate_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeSampleRate,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicSampleRate_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeSampleRate,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicSampleRate_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeSampleRate,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicSampleRate_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeSampleRate,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicSampleRate_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeSampleRate,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMinThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesMaxThreshold,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesExponent,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicEgressBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicEgressBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicEgressBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeEgressBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicPhysicalQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicPhysicalQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypePhysicalQueuedBytesDecreaseReset,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchFabricTrunkDynamicRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fabric_trunk,
                                   bcmiDlbControlTypeRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDlbMonitorIngressRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchDlbMonitorIngressRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_ecmp,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicMonitorIngressRandomSeed_set(int unit, int arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    value = arg;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_set(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicMonitorIngressRandomSeed_get(int unit, int *arg)
{
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dlb_control_get(unit,
                                   bcmi_dlb_type_fp_trunk,
                                   bcmiDlbControlTypeMonitorIngressRandomSeed,
                                   &value));
    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2StaticMoveToCpu_set(int unit, int arg)
{
    uint64_t val = (unsigned int)arg;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_ING_TRACE_EVENTs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, MON_ING_TRACE_EVENT_IDs,
                                      L2_SRC_STATIC_MOVEs));
    val = arg ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, CPUs, val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2StaticMoveToCpu_get(int unit, int *arg)
{
    uint64_t val;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_ING_TRACE_EVENTs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, MON_ING_TRACE_EVENT_IDs,
                                      L2_SRC_STATIC_MOVEs));
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPUs, &val));
    *arg = val & 0xffffffff;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHWL2Freeze_set(int unit, int arg)
{
    if (arg == 0) {
        return bcmi_ltsw_l2_hw_thaw(unit);
    } else {
        return bcmi_ltsw_l2_hw_freeze(unit);
    }
}

static int
sc_bcmSwitchHWL2Freeze_get(int unit, int *arg)
{
    return bcmi_ltsw_l2_hw_is_frozen(unit, arg);
}

static int
sc_bcmSwitchL2HitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_addr_hit_clear(unit, true, true));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2HitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SrcHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_addr_hit_clear(unit, true, false));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2SrcHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_addr_hit_clear(unit, false, true));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL2DstHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchBstEnable_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_enable_set(unit, arg);
}

static int
sc_bcmSwitchBstEnable_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_enable_get(unit, arg);
}

static int
sc_bcmSwitchBstTrackingMode_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_tracking_mode_set(unit, arg);
}

static int
sc_bcmSwitchBstTrackingMode_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_tracking_mode_get(unit, arg);
}

static int
sc_bcmSwitchBstSnapshotEnable_set(int unit, int arg)
{
    return bcmi_ltsw_cosq_bst_snapshot_enable_set(unit, arg);
}

static int
sc_bcmSwitchBstSnapshotEnable_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_bst_snapshot_enable_get(unit, arg);
}

static int
sc_bcmSwitchSampleIngressRandomSeed_set(int unit, int arg)
{
    return bcmi_ltsw_mirror_sampler_seed_set(unit, bcmiSamplerSeedIngress, arg);
}

static int
sc_bcmSwitchSampleIngressRandomSeed_get(int unit, int *arg)
{
    return bcmi_ltsw_mirror_sampler_seed_get(unit, bcmiSamplerSeedIngress, arg);
}

static int
sc_bcmSwitchSampleEgressRandomSeed_set(int unit, int arg)
{
    return bcmi_ltsw_mirror_sampler_seed_set(unit, bcmiSamplerSeedEgress, arg);
}

static int
sc_bcmSwitchSampleEgressRandomSeed_get(int unit, int *arg)
{
    return bcmi_ltsw_mirror_sampler_seed_get(unit, bcmiSamplerSeedEgress, arg);
}

static int
sc_bcmSwitchSampleFlexRandomSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_set(unit, bcmiSamplerSeedFlex, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchSampleFlexRandomSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_sampler_seed_get(unit, bcmiSamplerSeedFlex, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorAllowDuplicateMirroredPackets_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_switch_control_set
            (unit, bcmSwitchMirrorAllowDuplicateMirroredPackets, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorAllowDuplicateMirroredPackets_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_switch_control_get
            (unit, bcmSwitchMirrorAllowDuplicateMirroredPackets, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorCpuCosq_set(int unit, int arg)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_cpu_cos_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorCpuCosq_get(int unit, int *arg)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_cpu_cos_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorMulticastCosq_set(int unit, int arg)
{
    return bcmi_ltsw_mirror_mc_cos_set(unit, arg);
}

static int
sc_bcmSwitchMirrorMulticastCosq_get(int unit, int *arg)
{
    return bcmi_ltsw_mirror_mc_cos_get(unit, arg);
}

static int
sc_bcmSwitchMirrorPktChecksEnable_set(int unit, int arg)
{
    int rv;
    uint64_t drop_mask = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char **sym_strs = NULL;
    uint32_t num_sstrs;
    int i;

    SHR_FUNC_ENTER(unit);

    /* Revert drop_mask. */
    drop_mask = (arg > 0) ? 0 : 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_EGR_DROP_MASK_PROFILEs, &entry_hdl));

    /* Get symbol strings array size. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_info_get(entry_hdl, EVENT_IDs, 0, NULL,
                                           &num_sstrs));

    SHR_ALLOC(sym_strs, sizeof(const char *) * num_sstrs, "BcmLtswSwitchFieldSymStrs");
    SHR_NULL_CHECK(sym_strs, SHR_E_MEMORY);

    /* Get symbol strings array. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_info_get(entry_hdl, EVENT_IDs, num_sstrs,
                                           sym_strs, &num_sstrs));

    (void)bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    for (i = 0; i < num_sstrs; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, MON_EGR_DROP_MASK_PROFILEs, &entry_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, EVENT_IDs, sym_strs[i]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, FLOWs, MIRRORs));
        rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_NOT_FOUND) {
                (void)bcmlt_entry_free(entry_hdl);
                entry_hdl = BCMLT_INVALID_HDL;
                continue;
            } else {
                SHR_ERR_EXIT(rv);
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DROP_MASKs, drop_mask));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_UPDATE,
                                         entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));
    SHR_ERR_EXIT(trans_info.status);

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_transaction_free(trans_hdl);
    }
    SHR_FREE(sym_strs);
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorPktChecksEnable_get(int unit, int *arg)
{

    uint64_t val;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_EGR_DROP_MASK_PROFILEs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, EVENT_IDs, NO_DROPs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, FLOWs, MIRRORs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, DROP_MASKs, &val));
    *arg = (val > 0) ? 0 : 1;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchPFCDeadlockRecoveryAction_set(int unit, int arg)
{
    return bcmi_ltsw_pfc_deadlock_recovery_action_set(unit, arg);
}

static int
sc_bcmSwitchPFCDeadlockRecoveryAction_get(int unit, int *arg)
{
    return bcmi_ltsw_pfc_deadlock_recovery_action_get(unit, arg);
}

static int
sc_bcmSwitchIpmcSameVlanPruning_set(int unit, int arg)
{
    bool enable = FALSE;

    SHR_FUNC_ENTER(unit);
    if (arg < 0) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Revert the L2/L3 combinded mode. */
    enable = (arg > 0) ? FALSE : TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_l2_l3_combined_set(unit, enable));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIpmcSameVlanPruning_get(int unit, int *arg)
{
    bool enable = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_l2_l3_combined_get(unit, &enable));

    /* Revert the L2/L3 combinded mode. */
    *arg = (enable == TRUE) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchRateLimitLinear_set(int unit, int arg)
{
    /*
     * Linear mode or ITU mode can only be configured during boot time and
     * can't be updated during runtime.
     */
    return SHR_E_UNAVAIL;
}

static int
sc_bcmSwitchRateLimitLinear_get(int unit, int *arg)
{
    return bcmi_ltsw_cosq_shaper_config_get(unit, NULL, arg, NULL);
}

static int
sc_bcmSwitchEcnNonTcpResponsive_set(int unit, int arg)
{
    return bcmi_ltsw_ecn_responsive_default_set(unit, arg);
}

static int
sc_bcmSwitchEcnNonTcpResponsive_get(int unit, int *arg)
{
    return bcmi_ltsw_ecn_responsive_default_get(unit, arg);
}

static int
sc_bcmSwitchNetworkGroupDepth_set(int unit, int arg)
{
    /* Only the Get API is valid. */
    return SHR_E_UNAVAIL;
}

static int
sc_bcmSwitchNetworkGroupDepth_get(int unit, int *arg)
{
    return bcmi_ltsw_virtual_nw_group_num_get(unit, arg);
}

static int
sc_bcmSwitchEccSingleBitErrorEvent_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ser_single_bit_error_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEccSingleBitErrorEvent_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ser_single_bit_error_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlay_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEcmpOverlay,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmpOverlay_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEcmpOverlay,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmp_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEcmpUnderlay,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEcmp_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEcmpUnderlay,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashOverlayConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpHashOverlayConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpUnderlayHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPOverlayHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPOverlayHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    /* It doesn't support get, because the seed is scrambled. */
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPUnderlayHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowECMPUnderlayHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    /* It doesn't support get, because the seed is scrambled. */
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentLowerRange1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit, 0, 1, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentLowerRange1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit, 0, arg, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentUpperRange1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit, 0, 0, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentUpperRange1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit, 0, NULL, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentLowerRange2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit, 1, 1, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentLowerRange2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit, 1, arg, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentUpperRange2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_set(unit, 1, 0, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPortIndependentUpperRange2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mpls_global_label_range_get(unit, 1, NULL, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwControlWordUpperClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_set(unit,
                                           bcmiMplsGlobalCwUpperClear, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwControlWordUpperClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_get(unit,
                                           bcmiMplsGlobalCwUpperClear, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwControlWordLowerClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_set(unit,
                                           bcmiMplsGlobalCwLowerClear, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwControlWordLowerClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_get(unit,
                                           bcmiMplsGlobalCwLowerClear, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsGal_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_set(unit,
                                           bcmiMplsGlobalGalLabel, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsGal_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_get(unit,
                                           bcmiMplsGlobalGalLabel, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsEli_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_set(unit,
                                           bcmiMplsGlobalEliLabel, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsEli_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_get(unit,
                                           bcmiMplsGlobalEliLabel, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwTermSeqNumberRange_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_set(unit,
                                           bcmiMplsGlobalPwDecapSeqNumRange,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMplsPwTermSeqNumberRange_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT
        (bcmi_ltsw_mpls_global_control_get(unit,
                                           bcmiMplsGlobalPwDecapSeqNumRange,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigig3Ethertype_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_hg3_eth_type_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigig3Ethertype_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK (arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_hg3_eth_type_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchAlternateStoreForward_set(int unit, int arg)
{
    return SHR_E_UNAVAIL;
}

static int
sc_bcmSwitchAlternateStoreForward_get(int unit, int *arg)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkUc_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkUc,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkUc_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkUc,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkNonUnicast_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkNonUnicast_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkFailover_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkFailover,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkFailover_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkFailover,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelHigigTrunk_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkSystem,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelHigigTrunk_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkSystem,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelHigigTrunkFailover_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkSystemFailover,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelHigigTrunkFailover_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkSystemFailover,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkDynamic_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeDlbTrunk,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelTrunkDynamic_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeDlbTrunk,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelVpTrunk_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkVp,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelVpTrunk_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkVp,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEntropy_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEntropy,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseFlowSelEntropy_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEntropy,
                                   bcmiHosControlUseFlowSel,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseHigig3EntropyTrunkNonUnicast_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlUseHigig3Entropy,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashUseHigig3EntropyTrunkNonUnicast_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlUseHigig3Entropy,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEntropyHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEntropyHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashConcatEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashConcatEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlConcatEnable,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEntropyHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEntropyHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashSeed_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlSeed,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashSeed_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkUnicastHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkUc,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkNonUnicastHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkFailoverHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkFailover,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystem,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHigigTrunkFailoverHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowTrunkDynamicHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbTrunk,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowVpTrunkHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeTrunkVp,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowHashOverlayStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowLoadBalanceEntropyHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeEntropy,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMinOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMinOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlMinOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMaxOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashMaxOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlMaxOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashStrideOffset_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMacroFlowEcmpDynamicHashStrideOffset_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_flow_get(unit,
                                        bcmiHosTypeDlbEcmp,
                                        bcmiHosControlStrideOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkUnicastHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkUc,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkUnicastHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkUc,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkNonunicastHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkNonunicastHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkNonUc,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkFailoverHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkFailover,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkFailoverHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkFailover,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigigTrunkHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkSystem,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigigTrunkHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkSystem,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigigTrunkFailoverHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkSystemFailover,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHigigTrunkFailoverHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkSystemFailover,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeDlbTrunk,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTrunkDynamicHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeDlbTrunk,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVpTrunkHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeTrunkVp,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVpTrunkHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeTrunkVp,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEntropyHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEntropy,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEntropyHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEntropy,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpOverlayHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEcmpOverlay,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpOverlayHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEcmpOverlay,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeEcmpUnderlay,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchECMPHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeEcmpUnderlay,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicHashBitCountSelect_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_set(unit,
                                   bcmiHosTypeDlbEcmp,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEcmpDynamicHashBitCountSelect_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_get(unit,
                                   bcmiHosTypeDlbEcmp,
                                   bcmiHosControlBitCountSelect,
                                   arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL3,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL3,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthExactMatch_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthExactMatch,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthExactMatch_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthExactMatch,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthEgressVlanTranslate1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthEgressVlanTranslate1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthEgressVlanTranslate1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthEgressVlanTranslate1,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthEgressVlanTranslate2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthEgressVlanTranslate2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthEgressVlanTranslate2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthEgressVlanTranslate2,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3Tunnel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL3Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL3Tunnel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL3Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate3_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate3,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthVlanTranslate3_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthVlanTranslate3,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2Tunnel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL2Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2Tunnel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL2Tunnel,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2TunnelVnid_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL2TunnelVnid,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2TunnelVnid_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL2TunnelVnid,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthFlowTracker_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthFlowTracker,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthFlowTracker_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthFlowTracker,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthIpmcGroup_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthIpmcGroup,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthIpmcGroup_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthIpmcGroup,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthIpmcSourceGroup_set(int unit, int arg)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthIpmcSourceGroup,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthIpmcSourceGroup_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthIpmcSourceGroup,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2mc_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthL2mc,
                                            arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthL2mc_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthL2mc,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthMpls_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_set(unit,
                                            bcmSwitchHashMultiMoveDepthMpls,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchHashMultiMoveDepthMpls_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_multi_move_depth_get(unit,
                                            bcmSwitchHashMultiMoveDepthMpls,
                                            arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchLatencyEcnMarkMode_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ecn_latency_based_mode_set(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchLatencyEcnMarkMode_get(int unit, int *arg)
{
    bcm_switch_latency_ecn_mark_mode_t val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ecn_latency_based_mode_get(unit, &val));
    *arg = val;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanEvpnEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_evpn_enable_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchVxlanEvpnEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flow_evpn_enable_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcSameInterfaceDrop_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_same_intf_drop_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3UcSameInterfaceDrop_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_same_intf_drop_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3RouteHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_route_hit_clear(unit));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3RouteHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3HostHitClear_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    if (arg) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_host_hit_clear(unit));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchL3HostHitClear_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    *arg = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorUnicastCosqEnable_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mc_cos_mirror_set(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchMirrorUnicastCosqEnable_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mc_cos_mirror_get(unit, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIfaProtocol_set(int unit, int arg)
{
    uint32_t value = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_set(unit,
                                   bcmiIfaControlIfaProtocol,
                                   value));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIfaProtocol_get(int unit, int *arg)
{
    uint32_t value;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_get(unit,
                                   bcmiIfaControlIfaProtocol,
                                   &value));

    *arg = value;

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIntEgressTimeDelta_set(int unit, int arg)
{
#if defined(INCLUDE_INT)
    uint32_t value = (unsigned int)arg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_set(unit,
                                   bcmiIntControlEgressTimeDelta,
                                   value));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntEgressTimeDelta_get(int unit, int *arg)
{
#if defined(INCLUDE_INT)
    uint32_t value;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_int_control_get(unit,
                                   bcmiIntControlEgressTimeDelta,
                                   &value));

    *arg = value;

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

static int
sc_bcmSwitchIntProbeMarker1_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_set(unit,
                                   bcmiIfaControlIntProbeMarker1,
                                   (uint32)arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIntProbeMarker1_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_get(unit,
                                   bcmiIfaControlIntProbeMarker1,
                                   (uint32*)arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIntProbeMarker2_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_set(unit,
                                   bcmiIfaControlIntProbeMarker2,
                                   (uint32)arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchIntProbeMarker2_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ifa_control_get(unit,
                                   bcmiIfaControlIntProbeMarker2,
                                   (uint32*)arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncLiveWireToD_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit, bcmSwitchTimesyncLiveWireToD,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncLiveWireToD_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit, bcmSwitchTimesyncLiveWireToD,
                                           arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncEgressVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchTimesyncEgressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncEgressVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchTimesyncEgressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncIngressVersion_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchTimesyncIngressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchTimesyncIngressVersion_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchTimesyncIngressVersion,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNtpL4DestPort_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_set(unit,
                                           bcmSwitchNtpL4DestPort,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchNtpL4DestPort_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_get(unit,
                                           bcmSwitchNtpL4DestPort,
                                           arg));

exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEgressFlexCounterPresel_set(int unit, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_egr_presel_set(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
sc_bcmSwitchEgressFlexCounterPresel_get(int unit, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_egr_presel_get(unit, arg));
exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchLinkDownInfoSkip_set(int unit, int port, int arg)
{
    return bcmi_ltsw_link_port_info_skip_set(unit, port, arg);
}

static int
scp_bcmSwitchLinkDownInfoSkip_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_link_port_info_skip_get(unit, port, arg);
}

static int
scp_bcmSwitchMeterAdjust_set(int unit, int port, int arg)
{
    return bcmi_ltsw_rate_meter_adjust_set(unit, port, arg);
}

static int
scp_bcmSwitchMeterAdjust_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_rate_meter_adjust_get(unit, port, arg);
}

static int
scp_bcmSwitchShaperAdjust_set(int unit, int port, int arg)
{
    return bcmi_ltsw_rate_shaper_adjust_set(unit, port, arg);
}

static int
scp_bcmSwitchShaperAdjust_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_rate_shaper_adjust_get(unit, port, arg);
}

static int
scp_bcmSwitchAlternateStoreForward_set(int unit, int port, int arg)
{
    return bcmi_ltsw_port_asf_mode_set(unit, port, arg);
}

static int
scp_bcmSwitchAlternateStoreForward_get(int unit, int port, int *arg)
{
    return bcmi_ltsw_port_asf_mode_get(unit, port, arg);
}

static int
scp_bcmSwitchECMPOverlayHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPOverlayHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeEcmpOverlay,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPUnderlayHashSet0Offset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchECMPUnderlayHashSet0Offset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeEcmpUnderlay,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpDynamicHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeDlbEcmp,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEcmpDynamicHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeDlbEcmp,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkUnicastHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkUc,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkUnicastHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkUc,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTimesyncEgressTimestampingMode_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_set
            (unit, port, bcmSwitchTimesyncEgressTimestampingMode, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTimesyncEgressTimestampingMode_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_time_switch_control_port_get
            (unit, port, bcmSwitchTimesyncEgressTimestampingMode, arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkNonUnicastHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkNonUnicastHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkNonUc,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkFailoverHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkFailover,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkFailoverHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkFailover,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchHigigTrunkHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkSystem,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchHigigTrunkHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkSystem,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchFabricTrunkFailoverHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchFabricTrunkFailoverHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkSystemFailover,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkDynamicHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeDlbTrunk,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchTrunkDynamicHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeDlbTrunk,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchVpTrunkHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeTrunkVp,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchVpTrunkHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeTrunkVp,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEntropyHashOffset_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_set(unit,
                                        bcmiHosTypeEntropy,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchEntropyHashOffset_get(int unit, int port, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_hos_control_port_get(unit,
                                        bcmiHosTypeEntropy,
                                        port,
                                        bcmiHosControlOffset,
                                        arg));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2SourceDiscardMoveToCpu_set(int unit, int port, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmi_ltsw_port_tab_set(unit, port,
                               BCMI_PT_HONOR_SRC_DISCARD_ON_STN_MOVE,
                               arg ? 0 : 1));

exit:
    SHR_FUNC_EXIT();
}

static int
scp_bcmSwitchL2SourceDiscardMoveToCpu_get(int unit, int port, int *arg)
{
    int value;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmi_ltsw_port_tab_get(unit, port,
                               BCMI_PT_HONOR_SRC_DISCARD_ON_STN_MOVE,
                               &value));

    *arg = value ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

/* CodeGen function section end
 * Don't change or remove this comments.
 */

static sc_value_ctrl_t sc_vctrls[] = {
    SC_VALUE_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL, NULL}
};

static sc_bit_ctrl_t sc_mctrls[] ={
    SC_BIT_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL}
};

static sc_symbol_ctrl_t sc_sctrls[] = {
    SC_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_value_ctrl_t scp_vctrls[] = {
    SCP_VALUE_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL, NULL}
};

static sc_bit_ctrl_t scp_mctrls[] ={
    SCP_BIT_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL}
};

static sc_symbol_ctrl_t scp_sctrls[] = {
    SCP_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_symbol_kctrl_t scp_key_sctrls[] = {
    SCP_KEY_SYMBOL_CONTROLS
    {bcmSwitch__Count, FALSE, NULL, NULL, NULL}
};

static sc_bool_str_ctrl_t scp_port_pkt_ctrls[] = {
    SCP_PORT_PKT_CONTROLS
    {bcmSwitch__Count, NULL, NULL, NULL}
};

/* Software value type switch control. */
static bcmint_switch_control_value_map_t sc_value_maps[] = {
    {bcmSwitchUseGport, 0},
    {bcmSwitchL2McIdxRetType, 0},
    {bcmSwitchL3McIdxRetType, 0},
    {bcmSwitchL2ApplCallbackSuppress, 0},
    {bcmSwitch__Count, 0}
};

/* Port based software value type switch control. */
static bcmint_switch_control_port_value_map_t scp_value_maps[] = {
    {bcmSwitch__Count, { 0 }}
};

/*
 * SC type function list.
 */
static sc_func_t sc_funcs[] = {
    SC_FUNC_LIST
    {bcmSwitch__Count, NULL, NULL}
};

/*
 * SCP type function list.
 */
static scp_func_t scp_funcs[] = {
    SCP_FUNC_LIST
    {bcmSwitch__Count, NULL, NULL}
};

static bool sc_inited = false;

static int
sc_info_init(int unit)
{
    int rv = SHR_E_NONE;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    sc_info_t *sc = &sc_info[unit];
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    ha_array_size = COUNTOF(sc_value_maps);
    ha_instance_size = sizeof(bcmint_switch_control_value_map_t);
    ha_req_size = ha_array_size * ha_instance_size;
    ha_alloc_size = ha_req_size;
    sc->sc_value_map = bcmi_ltsw_ha_mem_alloc(unit,
                                              BCMI_HA_COMP_ID_SWITCH,
                                              HA_SC_SOFTWARE_SUB_COMP_ID,
                                              "bcmSwitchSW",
                                              &ha_alloc_size);
    SHR_NULL_CHECK(sc->sc_value_map, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memcpy(sc->sc_value_map, sc_value_maps, ha_alloc_size);
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit, BCMI_HA_COMP_ID_SWITCH,
                                           HA_SC_SOFTWARE_SUB_COMP_ID,
                                           0, ha_instance_size,
                                           ha_array_size,
                                           BCMINT_SWITCH_CONTROL_VALUE_MAP_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    ha_array_size = COUNTOF(scp_value_maps);
    ha_instance_size = sizeof(bcmint_switch_control_port_value_map_t);
    ha_req_size = ha_array_size * ha_instance_size;
    ha_alloc_size = ha_req_size;
    sc->scp_value_map = bcmi_ltsw_ha_mem_alloc(unit,
                                               BCMI_HA_COMP_ID_SWITCH,
                                               HA_SCP_SOFTWARE_SUB_COMP_ID,
                                               "bcmSwitchPortSW",
                                               &ha_alloc_size);
    SHR_NULL_CHECK(sc->scp_value_map, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memcpy(sc->scp_value_map, scp_value_maps, ha_alloc_size);
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit, BCMI_HA_COMP_ID_SWITCH,
                                           HA_SC_SOFTWARE_SUB_COMP_ID,
                                           0, ha_instance_size,
                                           ha_array_size,
                                           BCMINT_SWITCH_CONTROL_PORT_VALUE_MAP_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm && sc->sc_value_map) {
            (void)bcmi_ltsw_ha_mem_free(unit, sc->sc_value_map);
        }
        if (!warm && sc->scp_value_map) {
            (void)bcmi_ltsw_ha_mem_free(unit, sc->scp_value_map);
        }
    }
    SHR_FUNC_EXIT();
}

static void
scp_init(void) {
    int num, i = 1;

    while (scp_funcs[i].set) {
        scp_idx[scp_funcs[i].type] = i;
        i++;
    }
    num = COUNTOF(scp_vctrls);
    for (i = 0; i < num; i++) {
        if (scp_vctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_vctrls[i].type] = scValueCtrl + i;
    }
    num = COUNTOF(scp_mctrls);
    for (i = 0; i < num; i++) {
        if(scp_mctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_mctrls[i].type] = scBitCtrl + i;
    }
    num = COUNTOF(scp_sctrls);
    for (i = 0; i < num; i++) {
        if(scp_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_sctrls[i].type] = scSymbolCtrl + i;
    }
    num = COUNTOF(scp_key_sctrls);
    for (i = 0; i < num; i++) {
        if(scp_key_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_key_sctrls[i].type] = scKeySymbolCtrl + i;
    }
    num = COUNTOF(scp_port_pkt_ctrls);
    for (i = 0; i < num; i++) {
        if(scp_port_pkt_ctrls[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_port_pkt_ctrls[i].type] = scPortPktCtrl + i;
    }
    num = COUNTOF(scp_value_maps);
    for (i = 0; i < num; i++) {
        if(scp_value_maps[i].type == bcmSwitch__Count) {
            break;
        }
        scp_idx[scp_value_maps[i].type] = scSoftwareValueCtrl + i;
    }
}

static int
sc_init(int unit)
{
    int num, i = 1;

    SHR_FUNC_ENTER(unit);

    if (sc_inited) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (sc_info_init(unit));

    scp_init();
    while (sc_funcs[i].set) {
        sc_idx[sc_funcs[i].type] = i;
        i++;
    }
    num = COUNTOF(sc_vctrls);
    for (i = 0; i < num; i++) {
        if(sc_vctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_vctrls[i].type] = scValueCtrl + i;
    }
    num = COUNTOF(sc_mctrls);
    for (i = 0; i < num; i++) {
        if(sc_mctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_mctrls[i].type] = scBitCtrl + i;
    }
    num = COUNTOF(sc_sctrls);
    for (i = 0; i < num; i++) {
        if(sc_sctrls[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_sctrls[i].type] = scSymbolCtrl + i;
    }
    num = COUNTOF(sc_value_maps);
    for (i = 0; i < num; i++) {
        if(sc_value_maps[i].type == bcmSwitch__Count) {
            break;
        }
        sc_idx[sc_value_maps[i].type] = scSoftwareValueCtrl + i;
    }
    sc_inited = true;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */
int
bcmint_xfs_scp_set(int unit, int port, bcm_switch_control_t type, int arg)
{
    int table_type;
    int idx;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sc_init(unit));
    if (type < 0 || type >= bcmSwitch__Count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    table_type = scp_idx[type] & (~scIdMask);
    idx = scp_idx[type] & scIdMask;

    switch (table_type) {
        case scSoftwareValueCtrl:
            if (idx >= COUNTOF(scp_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            sc->scp_value_map[idx].value[port] = arg;
            break;
        case scValueCtrl:
            if (idx >= sizeof(scp_vctrls) / sizeof(scp_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_set(unit, port, &scp_vctrls[idx], arg));
            break;
        case scBitCtrl:
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_set(unit, port, type, scp_mctrls, arg));
            break;
        case scSymbolCtrl:
            if (idx >= sizeof(scp_sctrls) / sizeof(scp_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_set(unit, port, &scp_sctrls[idx], arg));
            break;
        case scKeySymbolCtrl:
            if (idx >= sizeof(scp_key_sctrls) / sizeof(scp_key_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_kctrl_set(unit, port, &scp_key_sctrls[idx], arg));
            break;
        case scPortPktCtrl:
            if (idx >=
                sizeof(scp_port_pkt_ctrls) / sizeof(scp_port_pkt_ctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_port_pkt_ctrl_set(unit, port, &scp_port_pkt_ctrls[idx],
                                      arg));
            break;
        default:
            if (idx >= sizeof(scp_funcs) / sizeof(scp_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (scp_funcs[idx].set) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (scp_funcs[idx].set(unit, port, arg));
            } else {
                SHR_ERR_EXIT(BCM_E_UNAVAIL);
            }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_scp_get(int unit, int port, bcm_switch_control_t type, int *arg)
{
    int table_type;
    int idx;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (sc_init(unit));
    if (type < 0 || type >= bcmSwitch__Count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    table_type = scp_idx[type] & (~scIdMask);
    idx = scp_idx[type] & scIdMask;

    switch (table_type) {
        case scSoftwareValueCtrl:
            if (idx >= COUNTOF(scp_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            *arg = sc->scp_value_map[idx].value[port];
            break;
        case scValueCtrl:
            if (idx >= sizeof(scp_vctrls) / sizeof(scp_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_get(unit, port, &scp_vctrls[idx], arg));
            break;
        case scBitCtrl:
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_get(unit, port, type, scp_mctrls, arg));
            break;
        case scSymbolCtrl:
            if (idx >= sizeof(scp_sctrls) / sizeof(scp_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_get(unit, port, &scp_sctrls[idx], arg));
            break;
        case scKeySymbolCtrl:
            if (idx >= sizeof(scp_key_sctrls) / sizeof(scp_key_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_kctrl_get(unit, port, &scp_key_sctrls[idx], arg));
            break;
        case scPortPktCtrl:
            if (idx >=
                sizeof(scp_port_pkt_ctrls) / sizeof(scp_port_pkt_ctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_port_pkt_ctrl_get(unit, port, &scp_port_pkt_ctrls[idx],
                                      arg));
            break;
        default:
            if (idx >= sizeof(scp_funcs) / sizeof(scp_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (scp_funcs[idx].get) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (scp_funcs[idx].get(unit, port, arg));
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_sc_set(int unit, bcm_switch_control_t type, int arg)
{
    int port = PORT_IGNORE;
    int table_type;
    int idx;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sc_init(unit));
    if (type < 0 || type >= bcmSwitch__Count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    table_type = sc_idx[type] & (~scIdMask);
    idx = sc_idx[type] & scIdMask;

    switch (table_type) {
        case scSoftwareValueCtrl:
            if (idx >= COUNTOF(sc_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            sc->sc_value_map[idx].value = arg;
            break;
        case scValueCtrl:
            if (idx >= sizeof(sc_vctrls) / sizeof(sc_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_set(unit, port, &sc_vctrls[idx], arg));
            break;
        case scBitCtrl:
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_set(unit, port, type, sc_mctrls, arg));
            break;
        case scSymbolCtrl:
            if (idx >= sizeof(sc_sctrls) / sizeof(sc_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_set(unit, port, &sc_sctrls[idx], arg));
            break;
        default:
            if (idx >= sizeof(sc_funcs) / sizeof(sc_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (sc_funcs[idx].set) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (sc_funcs[idx].set(unit, arg));
            } else {/* idx == 0 */
                bcm_pbmp_t pbmp;
                bool found = false;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT,
                                           &pbmp));
                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_xfs_scp_set(unit, port, type, arg));
                    found = true;
                }

                if (!found) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_sc_get(int unit, bcm_switch_control_t type, int *arg)
{
    int port = PORT_IGNORE;
    int table_type;
    int idx;
    sc_info_t *sc = &sc_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (sc_init(unit));
    if (type < 0 || type >= bcmSwitch__Count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    table_type = sc_idx[type] & (~scIdMask);
    idx = sc_idx[type] & scIdMask;

    switch (table_type) {
        case scSoftwareValueCtrl:
            if (idx >= COUNTOF(sc_value_maps)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            *arg = sc->sc_value_map[idx].value;
            break;
        case scValueCtrl:
            if (idx >= sizeof(sc_vctrls) / sizeof(sc_vctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_value_ctrl_get(unit, port, &sc_vctrls[idx], arg));
            break;
        case scBitCtrl:
            /* Support multiple fields setting. Ignore idx. */
            SHR_ERR_EXIT
                (sc_bit_ctrl_get(unit, port, type, sc_mctrls, arg));
            break;
        case scSymbolCtrl:
            if (idx >= sizeof(sc_sctrls) / sizeof(sc_sctrls[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_ERR_EXIT
                (sc_symbol_ctrl_get(unit, port, &sc_sctrls[idx], arg));
            break;
        default:
            if (idx >= sizeof(sc_funcs) / sizeof(sc_funcs[0])) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            if (sc_funcs[idx].get) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (sc_funcs[idx].get(unit, arg));
            } else {
                bcm_pbmp_t pbmp;
                bool found = false;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT,
                                           &pbmp));
                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_xfs_scp_get(unit, port, type, arg));
                    found = true;
                    break;
                }

                if (!found) {
                    SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_spc_add(int unit, uint32_t options,
                   bcm_switch_pkt_protocol_match_t *match,
                   bcm_switch_pkt_control_action_t *action,
                   int priority)
{
    uint32_t valid_options = 0;

    SHR_FUNC_ENTER(unit);

    valid_options = BCM_SWITCH_PKT_CONTROL_OPTIONS_REPLACE;
    if ((~valid_options) & options) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_protocol_handle(unit,
                                          options & BCM_SWITCH_PKT_CONTROL_OPTIONS_REPLACE ?
                                          BCMLT_OPCODE_UPDATE : BCMLT_OPCODE_INSERT,
                                          match,
                                          action,
                                          &priority));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_spc_get(int unit,
                   bcm_switch_pkt_protocol_match_t *match,
                   bcm_switch_pkt_control_action_t *action,
                   int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_protocol_handle(unit,
                                          BCMLT_OPCODE_LOOKUP,
                                          match,
                                          action,
                                          priority));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_protocol_handle(unit,
                                          BCMLT_OPCODE_DELETE,
                                          match,
                                          NULL,
                                          NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_pkt_integrity_check_add(int unit, uint32 options,
                                          bcm_switch_pkt_integrity_match_t *match,
                                          bcm_switch_pkt_control_action_t *action,
                                          int priority)
{
    uint32_t valid_options = 0;
    bcmlt_opcode_t opcode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);

    valid_options = BCM_SWITCH_PKT_CONTROL_OPTIONS_REPLACE;
    if ((~valid_options) & options) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Invalid control options: %u.\n"),
                        options));
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    if (priority < BCMINT_SWITCH_PKT_CONTROL_RSVD_PRI) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Priority[%d] must be greater than zero.\n"),
                        priority));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((action->copy_to_cpu > 1) || (action->discard > 1)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Control action value must be either 0 or 1.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((action->flood > 0) || (action->bpdu > 0)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Only supports copy to cpu action and discard action.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    opcode = (options & BCM_SWITCH_PKT_CONTROL_OPTIONS_REPLACE) ?
        BCMLT_OPCODE_UPDATE : BCMLT_OPCODE_INSERT;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_pkt_integrity_handle(unit, opcode, match, action,
                                               &priority));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_pkt_integrity_check_get(int unit,
                                          bcm_switch_pkt_integrity_match_t *match,
                                          bcm_switch_pkt_control_action_t *action,
                                          int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);
    SHR_NULL_CHECK(action, SHR_E_PARAM);
    SHR_NULL_CHECK(priority, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_pkt_integrity_handle(unit, BCMLT_OPCODE_LOOKUP, match,
                                               action, priority));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_pkt_integrity_check_delete(int unit,
                                             bcm_switch_pkt_integrity_match_t *match)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(match, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_pkt_integrity_handle(unit, BCMLT_OPCODE_DELETE, match,
                                               NULL, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_drop_event_to_symbol(int unit,
                                                 int drop_event,
                                                 const char **symbol)
{
    int i;
    static bool inited = false;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (drop_event < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    if (!inited) {
        memset(ep_recirc_drop_idx[unit], -1, sizeof(ep_recirc_drop_idx[unit]));
        for (i = 0; i < switch_db.num_ep_recirc_drop_event; i++) {
            ep_recirc_drop_idx[unit][switch_db.ep_recirc_drop_event_table_info[i].type] = i;
        }
        inited = TRUE;
    }

    if (ep_recirc_drop_idx[unit][drop_event] < 0 ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "EP Recirculate drop event(%d):"
                                " No mapped LT key field found.\n"),
                                drop_event));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *symbol = switch_db.ep_recirc_drop_event_table_info[ep_recirc_drop_idx[unit][drop_event]].field;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_symbol_to_drop_event(int unit,
                                                 const char *symbol,
                                                 int *drop_event)
{
    int i;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);
    SHR_NULL_CHECK(drop_event, SHR_E_PARAM);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    for (i = 0; i < switch_db.num_ep_recirc_drop_event; i++) {
        if (sal_strcmp(symbol, switch_db.ep_recirc_drop_event_table_info[i].field) == 0) {
            *drop_event = switch_db.ep_recirc_drop_event_table_info[i].type;
            break;
        }
    }

    if (i == switch_db.num_ep_recirc_drop_event) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_trace_event_num_get(int unit, int *num)
{
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    *num = switch_db.num_ep_recirc_trace_event;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_trace_event_to_symbol(int unit,
                                                  bcm_pkt_trace_event_t trace_event,
                                                  const char **symbol)
{
    int i;
    static bool inited = false;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_event < 0 || trace_event >= bcmPktTraceEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    if (!inited) {
        memset(ep_recirc_trace_idx[unit], -1, sizeof(ep_recirc_trace_idx[unit]));
        for (i = 0; i < switch_db.num_ep_recirc_trace_event; i++) {
            ep_recirc_trace_idx[unit][switch_db.ep_recirc_trace_event_table_info[i].type] = i;
        }
        inited = TRUE;
    }

    if (ep_recirc_trace_idx[unit][trace_event] < 0 ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "EP Recirculate trace event(%d):"
                                " No mapped LT key field found.\n"),
                                trace_event));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_NULL_CHECK(symbol, SHR_E_PARAM);

    *symbol = switch_db.ep_recirc_trace_event_table_info[ep_recirc_trace_idx[unit][trace_event]].field;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_symbol_to_trace_event(int unit,
                                                  const char *symbol,
                                                  bcm_pkt_trace_event_t *trace_event)
{
    int i;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_event, SHR_E_PARAM);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    for (i = 0; i < switch_db.num_ep_recirc_trace_event; i++) {
        if (sal_strcmp(symbol, switch_db.ep_recirc_trace_event_table_info[i].field) == 0) {
            *trace_event = switch_db.ep_recirc_trace_event_table_info[i].type;
            break;
        }
    }

    if (i == switch_db.num_ep_recirc_trace_event) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_ep_recirc_trace_event_symbol_iterate_get(int unit,
                                                           int index,
                                                           const char **symbol)
{
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    *symbol = switch_db.ep_recirc_trace_event_table_info[index].field;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_drop_event_to_symbol(int unit,
                                       bcm_pkt_drop_event_t drop_event,
                                       const char **table, const char **symbol)
{
    int i;
    static bool inited = false;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (drop_event < 0 || drop_event >= bcmPktDropEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    if (!inited) {
        memset(drop_idx[unit], -1, sizeof(drop_idx[unit]));
        for (i = 0; i < switch_db.num_drop_event; i++) {
            drop_idx[unit][switch_db.drop_event_table_info[i].type] = i;
        }
        inited = TRUE;
    }

    if (drop_idx[unit][drop_event] < 0 ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Drop event(%d): No mapped LT key field found.\n"),
                                drop_event));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_NULL_CHECK(table, SHR_E_PARAM);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);

    *table = switch_db.drop_event_table_info[drop_idx[unit][drop_event]].table;
    *symbol = switch_db.drop_event_table_info[drop_idx[unit][drop_event]].field;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_trace_event_to_symbol(int unit,
                                        bcm_pkt_trace_event_t trace_event,
                                        const char **table, const char **symbol)
{
    int i;
    static bool inited = false;
    bcmint_switch_db_t switch_db = {
        .drop_event_table_info = NULL,
        .num_drop_event = 0,
        .trace_event_table_info = NULL,
        .num_trace_event = 0,
        .ep_recirc_drop_event_table_info = NULL,
        .num_ep_recirc_drop_event = 0,
        .ep_recirc_trace_event_table_info = NULL,
        .num_ep_recirc_trace_event = 0,
    };

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (trace_event < 0 || trace_event >= bcmPktTraceEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the per-variant database. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_switch_db_get(unit, &switch_db));

    if (!inited) {
        memset(trace_idx[unit], -1, sizeof(trace_idx[unit]));
        for (i = 0; i < switch_db.num_trace_event; i++) {
            trace_idx[unit][switch_db.trace_event_table_info[i].type] = i;
        }
        inited = TRUE;
    }

    if (trace_idx[unit][trace_event] < 0 ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Trace event(%d): No mapped LT key field"
                                " found.\n"),
                                trace_event));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_NULL_CHECK(table, SHR_E_PARAM);
    SHR_NULL_CHECK(symbol, SHR_E_PARAM);

    *table = switch_db.trace_event_table_info[trace_idx[unit][trace_event]].table;
    *symbol = switch_db.trace_event_table_info[trace_idx[unit][trace_event]].field;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    int rv = SHR_E_NONE;
    uint64_t valid_actions = 0;
    uint64_t copy_to_cpu = 0;
    uint64_t loopback_enable = 1;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *symbol = NULL;
    const char *field;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);
    if (monitor->drop_event < 0 || monitor->drop_event >= bcmPktDropEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    valid_actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU |
                    BCM_SWITCH_MON_ACTION_LOOPBACK_DROP;
    if ((~valid_actions) & monitor->actions) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid actions(%"PRId32") for drop event.\n"),
                                monitor->actions));
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    rv = bcmint_xfs_switch_drop_event_to_symbol(unit, monitor->drop_event, &table, &symbol);
    if (rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Drop event(%d) not supported.\n"), monitor->drop_event));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (sal_strcmp(table, MON_ING_DROP_EVENTs) == 0) {
        field = MON_ING_DROP_EVENT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, field, symbol));
    if (monitor->actions & BCM_SWITCH_MON_ACTION_COPY_TO_CPU) {
        copy_to_cpu = 1;
    }
    if (monitor->actions & BCM_SWITCH_MON_ACTION_LOOPBACK_DROP) {
        loopback_enable = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, CPUs, copy_to_cpu));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, LOOPBACKs, loopback_enable));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    int rv = SHR_E_NONE;
    uint64_t copy_to_cpu = 0;
    uint64_t loopback_enable = 1;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *symbol = NULL;
    const char *field = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);
    if (monitor->drop_event < 0 || monitor->drop_event >= bcmPktDropEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmint_xfs_switch_drop_event_to_symbol(unit, monitor->drop_event, &table, &symbol);
    if (rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Drop event(%d) not supported.\n"), monitor->drop_event));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (sal_strcmp(table, MON_ING_DROP_EVENTs) == 0) {
        field = MON_ING_DROP_EVENT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, field, symbol));
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, entry_hdl));

    monitor->actions = BCM_SWITCH_MON_ACTION_NONE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPUs, &copy_to_cpu));
    if (copy_to_cpu > 0) {
        monitor->actions |= BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, LOOPBACKs, &loopback_enable));
    if (loopback_enable == 0) {
        monitor->actions |= BCM_SWITCH_MON_ACTION_LOOPBACK_DROP;
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    int rv = SHR_E_NONE;
    uint64_t valid_actions = 0;
    uint64_t copy_to_cpu = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *symbol = NULL;
    const char *field = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);
    if (monitor->trace_event < 0 || monitor->trace_event >= bcmPktTraceEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    valid_actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
    if ((~valid_actions) & monitor->actions) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid actions(%"PRId32") for trace event.\n"),
                                monitor->actions));
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    rv = bcmint_xfs_switch_trace_event_to_symbol(unit, monitor->trace_event, &table, &symbol);
    if (rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Trace event(%d) not supported.\n"), monitor->trace_event));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (sal_strcmp(table, MON_ING_TRACE_EVENTs) == 0) {
        field = MON_ING_TRACE_EVENT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, field, symbol));
    if (monitor->actions & BCM_SWITCH_MON_ACTION_COPY_TO_CPU) {
        copy_to_cpu = 1;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, CPUs, copy_to_cpu));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    int rv = SHR_E_NONE;
    uint64_t copy_to_cpu = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *table = NULL;
    const char *symbol = NULL;
    const char *field = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(monitor, SHR_E_PARAM);
    if (monitor->trace_event < 0 || monitor->trace_event >= bcmPktTraceEventCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmint_xfs_switch_trace_event_to_symbol(unit, monitor->trace_event, &table, &symbol);
    if (rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Trace event(%d) not supported.\n"), monitor->trace_event));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (sal_strcmp(table, MON_ING_TRACE_EVENTs) == 0) {
        field = MON_ING_TRACE_EVENT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, field, symbol));
    SHR_IF_ERR_VERBOSE_EXIT
        (sc_bcmlt_entry_lookup(unit, entry_hdl));

    monitor->actions = BCM_SWITCH_MON_ACTION_NONE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPUs, &copy_to_cpu));
    if (copy_to_cpu > 0) {
        monitor->actions |= BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);
    int skip_protocol_init = 0;
    SHR_FUNC_ENTER(unit);

    skip_protocol_init = bcmi_ltsw_property_get(unit,
                                                BCMI_CPN_SKIP_PROTOCOL_DEFAULT_ENTRIES,
                                                0);
    if ((skip_protocol_init != 0 ) && (skip_protocol_init != 1)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid skip_protocol_default_entries [%d].\n"),
                   skip_protocol_init));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (sc_info_init(unit));

    if (!warm) {
        /* Clean up PROTOCOL_PKT_FORWARD_TABLE table */
        SHR_IF_ERR_EXIT
            (bcmi_lt_clear(unit, PROTOCOL_PKT_FORWARD_TABLEs));

        /* Clean up PKT_INTEGRITY_CHECK_TABLE table. */
        SHR_IF_ERR_EXIT
            (bcmi_lt_clear(unit, PKT_INTEGRITY_CHECK_TABLEs));

        if (skip_protocol_init == 0 ) {
            /* Init PROTOCOL_PKT_FORWARD_TABLE. */
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_switch_protocol_init(unit));
        }

        /* Init PKT_INTEGRITY_CHECK_TABLE. */
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_switch_pkt_integrity_check_init(unit));

        /* Init MON_ING_TRACE_EVENT table. */
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_switch_mon_trace_event_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_xfs_switch_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    sc_inited = false;

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_clear(unit, PKT_INTEGRITY_CHECK_TABLEs));
    SHR_IF_ERR_EXIT
        (bcmi_lt_clear(unit, PROTOCOL_PKT_FORWARD_TABLEs));

exit:
    SHR_FUNC_EXIT();
}
