/*! \file flexctr.c
 *
 * LTSW Flexctr command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <appl/diag/system.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <shared/bsl.h>
#include <bcm/flexctr.h>

/******************************************************************************
* Local definitions
 */

enum flexctr_cmd_t {
    FLEXCTR_ADD = 1,
    FLEXCTR_UPDATE,
    FLEXCTR_DEL,
    FLEXCTR_SHOW,
    FLEXCTR_SET,
    FLEXCTR_CLEAR
};

static char *obj_list[] = BCM_FLEXCTR_OBJECT_NAME_INITIALIZER;

static char *select_list[] = {"Calc", "Length", "Paired", NULL};

static char *type_list[] = {
    "Noop",
    "Set",
    "Inc",
    "Dec",
    "Max",
    "Min",
    "Average",
    "SetBit",
    "Rdec",
    "Xor",
    NULL
};

static char *trigger_type[] = {"Time", "Condition", NULL};

static char *scale_list[] = {
    "Unlimited",
    "100ns",
    "500ns",
    "1us",
    "10us",
    "100us",
    "1ms",
    "10ms",
    "100ms",
    "1s",
    NULL
};

static char *src_list[] = BCM_FLEXCTR_SOURCE_NAME_INITIALIZER;

static char *mode_list[] = {
    "Normal",
    "Wide",
    "Slim",
    NULL
};

static bcm_flexctr_index_operation_t flexctr_index_op;
static bcm_flexctr_value_operation_t flexctr_value_a_op;
static bcm_flexctr_value_operation_t flexctr_value_b_op;
static bcm_flexctr_trigger_t flexctr_trigger;

#define VALUE_A     0
#define VALUE_B     1

#define TRAV_DEL    1
#define TRAV_SHOW   2

/******************************************************************************
 * Private functions
 */

static void
flexctr_quantization_show(int unit, uint32 quant_id,
                          bcm_flexctr_quantization_t *quant)
{
    int object, i, object_idx;
    uint32 min, max;

    object = quant->object;
    if (object >= bcmFlexctrObjectIngSystemSource) {
        object_idx = (object - bcmFlexctrObjectIngSystemSource) +
                     bcmFlexctrObjectStaticCount + 1;
    } else {
        object_idx = object;
    }

    cli_out("Quant Id %u Object %s\n", quant_id, obj_list[object_idx]);
    for (i = 0; i < quant->range_num; i++) {
        min = quant->range_check_min[i];
        max = quant->range_check_max[i];
        cli_out("\tRange %d %u - %u\n", i, min, max);
    }
}

static cmd_result_t
cmd_flexctr_quantization_create(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_flexctr_quantization_t quant;
    uint32 quant_id;
    int rv, object = 0, options = 0, range_num = 0, i;
    char *range, *low, *hi;
    int min, max;

    bcm_flexctr_quantization_t_init(&quant);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "QuantId", PQ_DFL|PQ_INT, 0, &quant_id, NULL);
    parse_table_add(&pt, "Object", PQ_DFL|PQ_MULTI, 0, &object, obj_list);
    parse_table_add(&pt, "NumRange", PQ_DFL|PQ_INT, 0, &range_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    /* FIXME
    if (quant_id != -1) {
        options = BCM_FLEXCTR_OPTIONS_WITH_ID | BCM_FLEXCTR_OPTIONS_REPLACE;
    }
    */

    quant.object = (bcm_flexctr_object_t)object;
    quant.range_num = range_num;
    for (i = 0; i < range_num; i++) {
        if ((low = range = ARG_GET(a)) != NULL) {
            if((hi = sal_strchr(range, '-')) != NULL) {
                hi++;
            } else {
                hi = low;
            }
        } else {
            return CMD_USAGE;
        }

        if (diag_parse_range(low, hi, &min, &max, 0, 1<<16)) {
            cli_out("Invalid range. Valid range is : 0 - 0x%x\n",(1<<16));
            return CMD_FAIL;
        }

        quant.range_check_min[i] = min;
        quant.range_check_max[i] = max;
    }

    if (ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        return CMD_FAIL;
    }

    rv = bcm_flexctr_quantization_create(unit, options, &quant, &quant_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    flexctr_quantization_show(unit, quant_id, &quant);

    return CMD_OK;
}

static int
quant_traverse_cb(int unit, uint32 quant_id, bcm_flexctr_quantization_t *quant,
                  void *user_data)
{
    int trav_op = *(int *)user_data, rv;

    switch (trav_op) {
        case TRAV_DEL:
            rv = bcm_flexctr_quantization_destroy(unit, quant_id);
            if (BCM_SUCCESS(rv)) {
                cli_out("SUCCESS destroy quantization %d\n", quant_id);
            }
            break;
        case TRAV_SHOW:
            flexctr_quantization_show(unit, quant_id, quant);
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

static cmd_result_t
cmd_flexctr_quantization_destroy(int unit, args_t *a)
{
    parse_table_t pt;
    int all = TRUE, trav_op = TRAV_DEL;
    uint32 quant_id;
    int rv;

    if (ARG_CNT(a)) {
        all = FALSE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "QuantId", PQ_DFL|PQ_INT, 0, &quant_id, NULL);
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    }

    if (all) {
        rv = bcm_flexctr_quantization_traverse(unit,
                                               quant_traverse_cb,
                                               (void *)&trav_op);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_flexctr_quantization_destroy(unit, quant_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

static cmd_result_t
cmd_flexctr_quantization_show(int unit, args_t *a)
{
    parse_table_t pt;
    int all = TRUE, trav_op = TRAV_SHOW;
    uint32 quant_id;
    int rv;
    bcm_flexctr_quantization_t quant;

    if (ARG_CNT(a)) {
        all = FALSE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "QuantId", PQ_DFL|PQ_INT, 0, &quant_id, NULL);
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    }

    if (all) {
        rv = bcm_flexctr_quantization_traverse(unit,
                                               quant_traverse_cb,
                                               (void *)&trav_op);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_flexctr_quantization_get(unit, quant_id, &quant);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        flexctr_quantization_show(unit, quant_id, &quant);
    }

    return CMD_OK;
}

static void
flexctr_index_op_show(int unit, char *prefix,
                      bcm_flexctr_index_operation_t *index_op)
{
    int obj0, obj1, bit0, bit1;
    int obj_idx0, obj_idx1;

    obj0 = index_op->object[0];
    bit0 = index_op->mask_size[0] + 1;
    obj1 = index_op->object[1];
    bit1 = index_op->mask_size[1] + 1;

    if (obj0 >= bcmFlexctrObjectIngSystemSource) {
        obj_idx0 = (obj0 - bcmFlexctrObjectIngSystemSource) +
                   bcmFlexctrObjectStaticCount + 1;
    } else {
        obj_idx0 = obj0;
    }

    if (obj1 >= bcmFlexctrObjectIngSystemSource) {
        obj_idx1 = (obj1 - bcmFlexctrObjectIngSystemSource) +
                   bcmFlexctrObjectStaticCount + 1;
    } else {
        obj_idx1 = obj1;
    }

    cli_out("\tindex=[%d %s 0]|[%d %s 0]\n",
            bit1, obj_list[obj_idx1], bit0, obj_list[obj_idx0]);
}

static cmd_result_t
cmd_flexctr_index_op_setup(int unit, args_t *a)
{
    bcm_flexctr_index_operation_t *index_op = &flexctr_index_op;
    parse_table_t pt;
    int i, obj0 = 0, obj1 = 0;
    uint32 nbit0 = 1, nbit1 = 1;
    uint32 flags = 0;

    if (ARG_CNT(a) == 0) {
        cli_out("Reset Index\n");

        for (i = 0; i < BCM_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
            index_op->object[i] = bcmFlexctrObjectConstZero;
            index_op->mask_size[i] = 1;
            index_op->shift[i] = 0;
        }

        flexctr_index_op_show(unit, "Preview", index_op);

        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Object0", PQ_DFL|PQ_MULTI, 0, &obj0, obj_list);
    parse_table_add(&pt, "Object1", PQ_DFL|PQ_MULTI, 0, &obj1, obj_list);
    parse_table_add(&pt, "BitNum0", PQ_DFL|PQ_INT, 0, &nbit0, NULL);
    parse_table_add(&pt, "BitNum1", PQ_DFL|PQ_INT, 0, &nbit1, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    for (i = 0; i < pt.pt_cnt; i++) {
        if (pt.pt_entries[i].pq_type & PQ_PARSED) {
            flags |= (1 << i);
        }
    }

    if (flags & 0x1) {
        index_op->object[0] = (bcm_flexctr_object_t)obj0;
    }

    if (flags & 0x2) {
        index_op->object[1] = (bcm_flexctr_object_t)obj1;
    }

    if (flags & 0x4) {
        if (nbit0 < 1 || nbit0 > 16) {
            cli_out("Invalid range. Valid range is : 1 - 16");
            return CMD_FAIL;
        }
        index_op->mask_size[0] = nbit0 - 1;
        index_op->shift[0] = 0;
        index_op->shift[1] = nbit0;
    }

    if (flags & 0x8) {
        if (nbit1 < 1 || nbit1 > 16) {
            cli_out("Invalid range. Valid range is : 1 - 16");
            return CMD_FAIL;
        }
        index_op->mask_size[1] = nbit1 - 1;
    }

    parse_arg_eq_done(&pt);

    flexctr_index_op_show(unit, "Preview", index_op);

    return CMD_OK;
}

static void
flexctr_value_op_show(int unit, char *prefix, int value,
                      bcm_flexctr_value_operation_t *value_op)
{
    int select, type, obj0, obj1;
    int obj_idx0, obj_idx1;

    select = value_op->select;
    type = value_op->type;
    cli_out("%s %s: Select=%s Type=%s\n", prefix,
            value == VALUE_A ? "Value A" : "Value B",
            select_list[select], type_list[type]);
    obj0 = value_op->object[0];
    obj1 = value_op->object[1];

    if (obj0 >= bcmFlexctrObjectIngSystemSource) {
        obj_idx0 = (obj0 - bcmFlexctrObjectIngSystemSource) +
                   bcmFlexctrObjectStaticCount + 1;
    } else {
        obj_idx0 = obj0;
    }

    if (obj1 >= bcmFlexctrObjectIngSystemSource) {
        obj_idx1 = (obj1 - bcmFlexctrObjectIngSystemSource) +
                   bcmFlexctrObjectStaticCount + 1;
    } else {
        obj_idx1 = obj1;
    }

    cli_out("\tvalue=[%s]|[%s]\n", obj_list[obj_idx1], obj_list[obj_idx0]);
}

static cmd_result_t
cmd_flexctr_value_op_setup(int unit, args_t *a, int value)
{
    bcm_flexctr_value_operation_t *value_op;
    parse_table_t pt;
    int select = 0, type = 0, obj0 = 0, obj1 = 0;
    uint32 flags = 0;
    int i;

    value_op = (value == VALUE_A) ? &flexctr_value_a_op : &flexctr_value_b_op;

    if (ARG_CNT(a) == 0) {
        cli_out("Reset %s\n", value == VALUE_A ? "Value A" : "Value B");

        sal_memset(value_op, 0, sizeof(bcm_flexctr_value_operation_t));

        if (value == VALUE_A) {
            value_op->select = bcmFlexctrValueSelectCounterValue;
            value_op->type = bcmFlexctrValueOperationTypeInc;
            value_op->object[0] = bcmFlexctrObjectConstOne;
            value_op->mask_size[0] = 15;
            value_op->shift[0] = 0;
            value_op->object[1] = bcmFlexctrObjectConstZero;
            value_op->mask_size[1] = 15;
            value_op->shift[1] = 16;
        } else {
            value_op->select = bcmFlexctrValueSelectPacketLength;
            value_op->type = bcmFlexctrValueOperationTypeInc;
        }

        flexctr_value_op_show(unit, "Preview", value, value_op);

        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SELect", PQ_DFL|PQ_MULTI, 0, &select, select_list);
    parse_table_add(&pt, "Type", PQ_DFL|PQ_MULTI, 0, &type, type_list);
    parse_table_add(&pt, "Object0", PQ_DFL|PQ_MULTI, 0, &obj0, obj_list);
    parse_table_add(&pt, "Object1", PQ_DFL|PQ_MULTI, 0, &obj1, obj_list);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    for (i = 0; i < pt.pt_cnt; i++) {
        if (pt.pt_entries[i].pq_type & PQ_PARSED) {
            flags |= (1 << i);
        }
    }

    if (flags & 0x1) {
        value_op->select = (bcm_flexctr_value_select_t)select;
    }

    if (flags & 0x2) {
        value_op->type = (bcm_flexctr_value_operation_type_t)type;
    }

    if (flags & 0x4) {
        value_op->object[0] = (bcm_flexctr_object_t)obj0;
        value_op->mask_size[0] = 15;
        value_op->shift[0] = 0;
    }

    if (flags & 0x8) {
        value_op->object[1] = (bcm_flexctr_object_t)obj1;
        value_op->mask_size[1] = 15;
        value_op->shift[1] = 16;
    }

    parse_arg_eq_done(&pt);

    flexctr_value_op_show(unit, "Preview", value, value_op);

    return CMD_OK;
}

static void
flexctr_trigger_show(int unit, char *prefix, bcm_flexctr_trigger_t *trigger)
{
    int type, scale, object, object_idx;

    type = trigger->trigger_type;
    cli_out("%s Trigger: Type=%s\n", prefix, trigger_type[type]);

    if (type == bcmFlexctrTriggerTypeTime) {
        scale = trigger->interval;
        cli_out("\tScale=%s Period=%d\n",
                scale_list[scale], trigger->period_num);
    } else {
        object = trigger->object;

        if (object >= bcmFlexctrObjectIngSystemSource) {
            object_idx = (object - bcmFlexctrObjectIngSystemSource) +
                         bcmFlexctrObjectStaticCount + 1;
        } else {
            object_idx = object;
        }

        cli_out("\tTrigger Object=%s\n", obj_list[object_idx]);
        cli_out("\tTrigger Start=%d Stop=%d\n",
                trigger->object_value_start,
                trigger->object_value_stop);
    }
}

static cmd_result_t
cmd_flexctr_trigger_setup(int unit, args_t *a)
{
    bcm_flexctr_trigger_t *trigger = &flexctr_trigger;
    parse_table_t pt;
    int type = 0, scale = 0, period = 0, object = 0, start = 0, stop = 0;
    uint32 flags = 0;
    int i;

    if (ARG_CNT(a) == 0) {
        cli_out("Reset Trigger\n");

        sal_memset(trigger, 0, sizeof(bcm_flexctr_trigger_t));

        trigger->period_num = 1;

        flexctr_trigger_show(unit, "Preview", trigger);

        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Type", PQ_DFL|PQ_MULTI, 0, &type, trigger_type);
    parse_table_add(&pt, "SCale", PQ_DFL|PQ_MULTI, 0, &scale, scale_list);
    parse_table_add(&pt, "Period", PQ_DFL|PQ_INT, 0, &period, NULL);
    parse_table_add(&pt, "Object", PQ_DFL|PQ_MULTI, 0, &object, obj_list);
    parse_table_add(&pt, "START", PQ_DFL|PQ_INT, 0, &start, NULL);
    parse_table_add(&pt, "STOP", PQ_DFL|PQ_INT, 0, &stop, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    for (i = 0; i < pt.pt_cnt; i++) {
        if (pt.pt_entries[i].pq_type & PQ_PARSED) {
            flags |= (1 << i);
        }
    }

    if (flags & 0x1) {
        trigger->trigger_type = (bcm_flexctr_trigger_type_t)type;
    }

    if (flags & 0x2) {
        trigger->interval = (bcm_flexctr_trigger_interval_t)scale;
    }

    if (flags & 0x4) {
        trigger->period_num = period;
    }

    if (flags & 0x8) {
        trigger->object = (bcm_flexctr_object_t)object;
        trigger->object_value_mask = 0xffff;
    }

    if (flags & 0x10) {
        trigger->object_value_start = start;
    }

    if (flags & 0x20) {
        trigger->object_value_stop = stop;
    }

    parse_arg_eq_done(&pt);

    flexctr_trigger_show(unit, "Preview", trigger);

    return CMD_OK;
}

static cmd_result_t
cmd_flexctr_trigger_update(int unit, args_t *a)
{
    parse_table_t pt;
    uint32 stat_id = 0;
    int enable, rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "StatId", PQ_DFL|PQ_INT, 0, &stat_id, NULL);
    parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, 0, &enable, NULL);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    rv = bcm_flexctr_trigger_enable_set(unit, stat_id, enable);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static void
flexctr_action_show(int unit, uint32 stat_counter_id,
                    bcm_flexctr_action_t *action)
{
    int source, mode;

    source = action->source;
    mode = action->mode;

    cli_out("Action Id %d: Source=%s Mode=%s\n",
            stat_counter_id, src_list[source], mode_list[mode]);

    flexctr_index_op_show(unit, "  ", &(action->index_operation));

    flexctr_value_op_show(unit, "  ", VALUE_A, &(action->operation_a));

    if (mode == bcmFlexctrCounterModeNormal) {
        flexctr_value_op_show(unit, "  ", VALUE_B, &(action->operation_b));
    }

    if (action->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        flexctr_trigger_show(unit, "  ", &(action->trigger));
    }
}

static cmd_result_t
cmd_flexctr_action_create(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_flexctr_action_t action;
    int rv, trigger = 0, source = 0, hint = 0, mode = 0, total_bits;
    uint32 stat_id;

    bcm_flexctr_action_t_init(&action);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Source", PQ_DFL|PQ_MULTI, 0, &source, src_list);
    parse_table_add(&pt, "Hint", PQ_DFL|PQ_INT, 0, &hint, 0);
    parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI, 0, &mode, mode_list);
    parse_table_add(&pt, "Trigger", PQ_DFL|PQ_BOOL, 0, &trigger, 0);
    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    action.source = (bcm_flexctr_source_t)source;
    action.hint = hint;
    action.mode = (bcm_flexctr_counter_mode_t)mode;
    total_bits = flexctr_index_op.mask_size[0] + flexctr_index_op.mask_size[1];
    action.index_num = 1 << total_bits;
    sal_memcpy(&(action.index_operation), &flexctr_index_op,
               sizeof(bcm_flexctr_index_operation_t));
    sal_memcpy(&(action.operation_a), &flexctr_value_a_op,
               sizeof(bcm_flexctr_value_operation_t));

    if (mode == bcmFlexctrCounterModeNormal) {
        sal_memcpy(&(action.operation_b), &flexctr_value_b_op,
                   sizeof(bcm_flexctr_value_operation_t));
    }

    if (trigger) {
        action.flags |= BCM_FLEXCTR_ACTION_FLAGS_TRIGGER;
        sal_memcpy(&(action.trigger), &flexctr_trigger,
                   sizeof(bcm_flexctr_trigger_t));
    }

    rv = bcm_flexctr_action_create(unit, 0, &action, &stat_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    flexctr_action_show(unit, stat_id, &action);

    return CMD_OK;
}

static int
action_traverse_cb(int unit, uint32 stat_counter_id,
                   bcm_flexctr_action_t *action, void *user_data)
{
    int trav_op = *(int *)user_data, rv;

    switch (trav_op) {
        case TRAV_DEL:
            rv = bcm_flexctr_action_destroy(unit, stat_counter_id);
            if (BCM_SUCCESS(rv)) {
                cli_out("SUCCESS destroy action %d\n", stat_counter_id);
            }
            break;
        case TRAV_SHOW:
            flexctr_action_show(unit, stat_counter_id, action);
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

static cmd_result_t
cmd_flexctr_action_destroy(int unit, args_t *a)
{
    parse_table_t pt;
    int all = TRUE, trav_op = TRAV_DEL;
    int rv;
    uint32 stat_id = 0;

    if (ARG_CNT(a)) {
        all = FALSE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "StatId", PQ_DFL|PQ_INT, 0, &stat_id, NULL);
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    }

    if (all) {
        rv = bcm_flexctr_action_traverse(unit,
                                         action_traverse_cb,
                                         (void *)&trav_op);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_flexctr_action_destroy(unit, stat_id);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

static cmd_result_t
cmd_flexctr_action_show(int unit, args_t *a)
{
    parse_table_t pt;
    int all = TRUE, trav_op = TRAV_SHOW;
    int rv;
    uint32 stat_id = 0;
    bcm_flexctr_action_t action;

    if (ARG_CNT(a)) {
        all = FALSE;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "StatId", PQ_DFL|PQ_INT, 0, &stat_id, NULL);
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
    }

    if (all) {
        rv = bcm_flexctr_action_traverse(unit,
                                         action_traverse_cb,
                                         (void *)&trav_op);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_flexctr_action_get(unit, stat_id, &action);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        flexctr_action_show(unit, stat_id, &action);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_flexctr_stat(int unit, args_t *a, int clear)
{
    parse_table_t pt;
    int rv;
    uint32 stat_id = 0, i, alloc_sz;
    char *range, *low, *hi;
    int min = 0, max, total;
    bcm_flexctr_action_t action;
    bcm_flexctr_counter_mode_t mode;
    uint32 num_entries, *counter_indexes = NULL;
    bcm_flexctr_counter_value_t *counter_values = NULL;
    uint64 value;
    char line[120], buf_val[32];

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "StatId", PQ_DFL|PQ_INT, 0, &stat_id, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    rv = bcm_flexctr_action_get(unit, stat_id, &action);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    mode = action.mode;
    total = action.index_num;
    max = total - 1;

    if (ARG_CNT(a)) {
        if ((low = range = ARG_GET(a)) != NULL) {
            if((hi = sal_strchr(range, '-')) != NULL) {
                hi++;
            } else {
                hi = low;
            }
        } else {
            return CMD_USAGE;
        }

        if (diag_parse_range(low, hi, &min, &max, 0, total)) {
            cli_out("Invalid range. Valid range is : 0 - %d\n", total - 1);
            return CMD_FAIL;
        }
    }

    if (ARG_CNT(a) > 0) {
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        return CMD_FAIL;
    }

    num_entries = max - min + 1;

    alloc_sz = sizeof(uint32) * num_entries;
    counter_indexes = (uint32 *)sal_alloc(alloc_sz, "ltswFlexctrIndexes");
    for (i = 0; i < num_entries; i++) {
        counter_indexes[i] = min + i;
    }

    alloc_sz = sizeof(bcm_flexctr_counter_value_t) * num_entries;
    counter_values = (bcm_flexctr_counter_value_t *)
                     sal_alloc(alloc_sz, "ltswFlexctrValues");
    sal_memset(counter_values, 0, alloc_sz);

    if (clear) {
        rv = bcm_flexctr_stat_set(unit, stat_id, num_entries,
                                  counter_indexes, counter_values);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));

            sal_free(counter_indexes);
            sal_free(counter_values);
            return CMD_FAIL;
        }
    } else {
        rv = bcm_flexctr_stat_get(unit, stat_id, num_entries,
                                  counter_indexes, counter_values);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));

            sal_free(counter_indexes);
            sal_free(counter_values);
            return CMD_FAIL;
        }

        cli_out("Counter value: Action Id=%d\n", stat_id);
        for (i = 0; i < num_entries; i++) {
            cli_out("\tindex %d\n", counter_indexes[i]);

            switch (mode) {
                case bcmFlexctrCounterModeNormal:
                    value = counter_values[i].value[0];
                    format_uint64_decimal(buf_val, value, ',');
                    sal_sprintf(line, "A:%22s,  ", buf_val);
                    value = counter_values[i].value[1];
                    format_uint64_decimal(buf_val, value, ',');
                    sal_sprintf(line + sal_strlen(line),
                                "B:%22s", buf_val);
                    break;

                case bcmFlexctrCounterModeWide:
                    value = counter_values[i].value[0];
                    sal_sprintf(line, "Upper 0x%08x%08x  ",
                                COMPILER_64_HI(value),
                                COMPILER_64_LO(value));
                    value = counter_values[i].value[1];
                    sal_sprintf(line + sal_strlen(line),
                                "Lower 0x%08x%08x",
                                COMPILER_64_HI(value),
                                COMPILER_64_LO(value));
                    break;

                case bcmFlexctrCounterModeSlim:
                    value = counter_values[i].value[0];
                    format_uint64_decimal(buf_val, value, ',');
                    sal_sprintf(line, "%22s", buf_val);
                    break;

                default:
                    break;
            }
            cli_out("\t\t%s\n", line);
        }
    }

    sal_free(counter_indexes);
    sal_free(counter_values);
    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */

cmd_result_t
cmd_ltsw_flexctr(int unit, args_t *a)
{
    char *table, *subcmd_s;
    int sub_command = 0;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((table = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((subcmd_s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd_s, "add") == 0) {
        sub_command = FLEXCTR_ADD;
    }

    if (sal_strcasecmp(subcmd_s, "update") == 0) {
        sub_command = FLEXCTR_UPDATE;
    }

    if (sal_strcasecmp(subcmd_s, "del") == 0) {
        sub_command = FLEXCTR_DEL;
    }

    if (sal_strcasecmp(subcmd_s, "show") == 0) {
        sub_command = FLEXCTR_SHOW;
    }

    if (sal_strcasecmp(subcmd_s, "set") == 0) {
        sub_command = FLEXCTR_SET;
    }

    if (sal_strcasecmp(subcmd_s, "clear") == 0) {
        sub_command = FLEXCTR_CLEAR;
    }

    if (sal_strcasecmp(table, "quantization") == 0) {
        switch (sub_command) {
            case FLEXCTR_ADD:
            case FLEXCTR_UPDATE:
                return cmd_flexctr_quantization_create(unit, a);
            case FLEXCTR_DEL:
                return cmd_flexctr_quantization_destroy(unit, a);
            case FLEXCTR_SHOW:
                return cmd_flexctr_quantization_show(unit, a);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "index") == 0) {
        switch (sub_command) {
            case FLEXCTR_SET:
                return cmd_flexctr_index_op_setup(unit, a);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "valueA") == 0) {
        switch (sub_command) {
            case FLEXCTR_SET:
                return cmd_flexctr_value_op_setup(unit, a, VALUE_A);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "valueB") == 0) {
        switch (sub_command) {
            case FLEXCTR_SET:
                return cmd_flexctr_value_op_setup(unit, a, VALUE_B);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "trigger") == 0) {
        switch (sub_command) {
            case FLEXCTR_SET:
                return cmd_flexctr_trigger_setup(unit, a);
            case FLEXCTR_UPDATE:
                return cmd_flexctr_trigger_update(unit, a);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "action") == 0) {
        switch (sub_command) {
            case FLEXCTR_ADD:
                return cmd_flexctr_action_create(unit, a);
            case FLEXCTR_DEL:
                return cmd_flexctr_action_destroy(unit, a);
            case FLEXCTR_SHOW:
                return cmd_flexctr_action_show(unit, a);
            default:
                return CMD_USAGE;
        }
    }

    if (sal_strcasecmp(table, "stat") == 0) {
        switch (sub_command) {
            case FLEXCTR_SHOW:
                return cmd_flexctr_stat(unit, a, FALSE);
            case FLEXCTR_CLEAR:
                return cmd_flexctr_stat(unit, a, TRUE);
            default:
                return CMD_USAGE;
        }
    }

    return CMD_OK;
}
