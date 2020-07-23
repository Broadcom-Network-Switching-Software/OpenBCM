/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
int unit = 0;
int options = 0, rv;
uint32_t stat_counter_id;
bcm_trunk_t tid;
bcm_trunk_info_t t_add_info;
bcm_trunk_member_t member_array;
bcm_flexctr_action_t action = {0};
bcm_flexctr_index_operation_t *index_op = &action.index_operation;
bcm_flexctr_value_operation_t *value_a_op = &action.operation_a;
bcm_flexctr_value_operation_t *value_b_op = &action.operation_b;

action.source = bcmFlexctrSourceTrunk;
action.mode = bcmFlexctrCounterModeNormal;
action.index_num = 1;

index_op->object[0] = bcmFlexctrObjectConstZero;
index_op->mask_size[0] = 15;
index_op->shift[0] = 0;
index_op->object[1] = bcmFlexctrObjectConstZero;
index_op->mask_size[1] = 1;
index_op->shift[1] = 0;

value_a_op->select = bcmFlexctrValueSelectCounterValue;
value_a_op->object[0] = bcmFlexctrObjectConstOne;
value_a_op->mask_size[0] = 15;
value_a_op->shift[0] = 0;
value_a_op->object[1] = bcmFlexctrObjectConstZero;
value_a_op->mask_size[1] = 1;
value_a_op->shift[1] = 0;
value_a_op->type = bcmFlexctrValueOperationTypeInc;

value_b_op->select = bcmFlexctrValueSelectPacketLength;
value_b_op->type = bcmFlexctrValueOperationTypeInc;

print bcm_flexctr_action_create(unit, options, &action, &stat_counter_id);
print bcm_trunk_create(unit, 0, &tid);

bcm_trunk_info_t_init(&t_add_info);
t_add_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
t_add_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

bcm_trunk_member_t_init(&member_array);
BCM_GPORT_MODPORT_SET(member_array.gport, 0, 1);
print bcm_trunk_set(unit, tid, &t_add_info, 1, &member_array);

print bcm_trunk_stat_attach(unit, tid, stat_counter_id);
print bcm_trunk_stat_id_get(unit, tid, &stat_counter_id);
print stat_counter_id;

print bcm_trunk_stat_detach(unit, tid, stat_counter_id);
print bcm_flexctr_action_destroy(unit, stat_counter_id);