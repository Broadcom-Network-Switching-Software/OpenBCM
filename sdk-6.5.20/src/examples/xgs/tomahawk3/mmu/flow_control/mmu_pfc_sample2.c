/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

cint_reset();
int rx_port=80;
int tx_port = 20;
int profile_id = 0;

/* Setup MMU in lossless mode */
bshell (0, "config add mmu_lossless=1");

/*Modify flex_enable and MTU size to aloow more number of PFC-optimized
 * classes*/
bshell(0, "config add port_flex_enable=0");
bshell (0, "config add mmu_pfc_group_optimized_mtu_size=1024");

/* Change scheduler to have 8UC+4MC queues for each port*/
bshell(0, "config add mmu_port_num_mc_queue=2");
bshell(0, "rc");

/* Enable PFC receive and transmit on the PFC partners */
print bcm_port_control_set(0, tx_port, bcmPortControlPFCTransmit, 1);
print bcm_port_control_set(0, rx_port, bcmPortControlPFCReceive, 1);

/* Map priorities 0, 4, 5 to PG7 */
int pg_array[8] = {7, 6, 5, 4, 7, 7, 1, 0};
print bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, 0, 8, pg_array);

/* Scheduler mapping for profile profile_id */
/* Scheduler mode with 6 UC and 6 MC */
/* Cos0-3 has 1-UC+1-MC pair, cos4-7 has resp.UC queue */
bcm_cosq_mapping_t cosq_mapping_arr[12];
cosq_mapping_arr[0].cosq=0;
cosq_mapping_arr[0].num_ucq=1;
cosq_mapping_arr[0].num_mcq=0;
cosq_mapping_arr[0].strict_priority=0;
cosq_mapping_arr[0].fc_is_uc_only=0;
cosq_mapping_arr[1].cosq=1;
cosq_mapping_arr[1].num_ucq=0;
cosq_mapping_arr[1].num_mcq=1;
cosq_mapping_arr[1].strict_priority=0;
cosq_mapping_arr[1].fc_is_uc_only=0;
cosq_mapping_arr[2].cosq=2;
cosq_mapping_arr[2].num_ucq=2;
cosq_mapping_arr[2].num_mcq=0;
cosq_mapping_arr[2].strict_priority=0;
cosq_mapping_arr[2].fc_is_uc_only=0;
cosq_mapping_arr[3].cosq=3;
cosq_mapping_arr[3].num_ucq=1;
cosq_mapping_arr[3].num_mcq=0;
cosq_mapping_arr[3].strict_priority=0;
cosq_mapping_arr[3].fc_is_uc_only=0;
cosq_mapping_arr[4].cosq=4;
cosq_mapping_arr[4].num_ucq=1;
cosq_mapping_arr[4].num_mcq=0;
cosq_mapping_arr[4].strict_priority=0;
cosq_mapping_arr[4].fc_is_uc_only=0;
cosq_mapping_arr[5].cosq=5;
cosq_mapping_arr[5].num_ucq=1;
cosq_mapping_arr[5].num_mcq=1; 
cosq_mapping_arr[5].strict_priority=1;
cosq_mapping_arr[5].fc_is_uc_only=0;
cosq_mapping_arr[6].cosq=6; 
cosq_mapping_arr[6].num_ucq=1; 
cosq_mapping_arr[6].num_mcq=1; 
cosq_mapping_arr[6].strict_priority=1;
cosq_mapping_arr[6].fc_is_uc_only=1;
cosq_mapping_arr[7].cosq=7;
cosq_mapping_arr[7].num_ucq=1;
cosq_mapping_arr[7].num_mcq=0;
cosq_mapping_arr[7].strict_priority=1;
cosq_mapping_arr[7].fc_is_uc_only=0;
cosq_mapping_arr[8].cosq=8;
cosq_mapping_arr[8].num_ucq=0;
cosq_mapping_arr[8].num_mcq=1;
cosq_mapping_arr[8].strict_priority=1;
cosq_mapping_arr[8].fc_is_uc_only=0;
cosq_mapping_arr[9].cosq=9;
cosq_mapping_arr[9].num_ucq=0;
cosq_mapping_arr[9].num_mcq=0;
cosq_mapping_arr[9].strict_priority=0;
cosq_mapping_arr[9].fc_is_uc_only=0;
cosq_mapping_arr[10].cosq=10;
cosq_mapping_arr[10].num_ucq=0;
cosq_mapping_arr[10].num_mcq=0;
cosq_mapping_arr[10].strict_priority=0;
cosq_mapping_arr[10].fc_is_uc_only=0;
cosq_mapping_arr[11].cosq=11;
cosq_mapping_arr[11].num_ucq=0;
cosq_mapping_arr[11].num_mcq=0;
cosq_mapping_arr[11].strict_priority=0;
cosq_mapping_arr[11].fc_is_uc_only=0;
print bcm_cosq_schedq_mapping_set(0, profile_id, 12, cosq_mapping_arr);

bshell (0, "debug soc mmu");
/* Configure PFC receive for profile profile_id */
/* Priority 0 &4 are PFC-optimized */
bcm_cosq_pfc_class_map_config_t config_array1[8];
bcm_cosq_pfc_class_map_config_t_init(&config_array1[0]);

config_array1[0].pfc_enable = 1;
config_array1[0].cos_list_bmp |= 1 << 0;
config_array1[0].pfc_optimized = 0;
config_array1[1].pfc_enable = 1;
config_array1[1].cos_list_bmp |= 1 << 1;
config_array1[1].pfc_optimized = 0;
config_array1[2].pfc_enable = 1;
config_array1[2].cos_list_bmp |= 1 << 2;
config_array1[2].pfc_optimized = 0;
config_array1[3].pfc_enable = 1;
config_array1[3].cos_list_bmp |= 1 << 3;
config_array1[3].pfc_optimized = 1;
config_array1[4].pfc_enable = 1;
config_array1[4].cos_list_bmp |= 1 << 4;
config_array1[4].pfc_optimized = 0;
config_array1[5].pfc_enable = 1;
config_array1[5].cos_list_bmp |= 1 << 5;
config_array1[5].pfc_optimized = 0;
config_array1[6].pfc_enable = 1;
config_array1[6].cos_list_bmp |= (1 <<6 |1 << 7);
config_array1[6].pfc_optimized = 1;
config_array1[7].pfc_enable = 1;
config_array1[7].cos_list_bmp |= (1 <<6 |1 << 7);
config_array1[7].pfc_optimized = 1;

print bcm_cosq_pfc_class_config_profile_set(0, profile_id, 8, config_array1);

/* Map rx_port to use profile 1*/
print bcm_cosq_port_profile_set(0, rx_port, bcmCosqProfilePFCAndQueueHierarchy,
        profile_id);

/*
 Table 5-4
 Cos    MMUQ0    MMUQ1    FC_UC    SP    PFC_opt    EB_grp(cos)  EB_group0  EB_group1  SP(0)  SP(1)
  8      11      -1         0      1       0           8           8          1         1      0
  7       7      -1         0      1       1           7           7          1         1      0
  6       6      10         1      1       1           7           7          6         1      1
  5       5       9         0      1       0           6           6          6         1      1
  4       4      -1         0      0       0           5           5          1         0      0
  3       3      -1         0      0       2           4           4          1         0      0
  2       1       2         0      0       0           3           3          3         0      0
  1       8      -1         0      0       0           2           2          1         0      0
  0       0      -1         0      0       0           1           1          1         0      0

 */


profile_id=1;
int rx_port1=21;
int tx_port1=81;
/* Enable PFC receive and transmit on the PFC partners */
print bcm_port_control_set(0, tx_port1, bcmPortControlPFCTransmit, 1);
print bcm_port_control_set(0, rx_port1, bcmPortControlPFCReceive, 1);
/* Scheduler mapping for profile profile_id */
/* Scheduler mode with 6 UC and 6 MC */
/* Cos0-3 has 1-UC+1-MC pair, cos4-7 has resp.UC queue */
cosq_mapping_arr[0].cosq=0;
cosq_mapping_arr[0].num_ucq=1;
cosq_mapping_arr[0].num_mcq=0;
cosq_mapping_arr[0].strict_priority=0;
cosq_mapping_arr[0].fc_is_uc_only=0;
cosq_mapping_arr[1].cosq=1;
cosq_mapping_arr[1].num_ucq=1;
cosq_mapping_arr[1].num_mcq=0;
cosq_mapping_arr[1].strict_priority=0;
cosq_mapping_arr[1].fc_is_uc_only=0;
cosq_mapping_arr[2].cosq=2;
cosq_mapping_arr[2].num_ucq=1;
cosq_mapping_arr[2].num_mcq=0;
cosq_mapping_arr[2].strict_priority=0;
cosq_mapping_arr[2].fc_is_uc_only=0;
cosq_mapping_arr[3].cosq=3;
cosq_mapping_arr[3].num_ucq=1;
cosq_mapping_arr[3].num_mcq=1;
cosq_mapping_arr[3].strict_priority=0;
cosq_mapping_arr[3].fc_is_uc_only=1;
cosq_mapping_arr[4].cosq=4;
cosq_mapping_arr[4].num_ucq=1;
cosq_mapping_arr[4].num_mcq=1;
cosq_mapping_arr[4].strict_priority=1;
cosq_mapping_arr[4].fc_is_uc_only=0;
cosq_mapping_arr[5].cosq=5;
cosq_mapping_arr[5].num_ucq=1;
cosq_mapping_arr[5].num_mcq=1; 
cosq_mapping_arr[5].strict_priority=1;
cosq_mapping_arr[5].fc_is_uc_only=1;
cosq_mapping_arr[6].cosq=6; 
cosq_mapping_arr[6].num_ucq=1; 
cosq_mapping_arr[6].num_mcq=1; 
cosq_mapping_arr[6].strict_priority=1;
cosq_mapping_arr[6].fc_is_uc_only=1;
cosq_mapping_arr[7].cosq=7;
cosq_mapping_arr[7].num_ucq=1;
cosq_mapping_arr[7].num_mcq=0;
cosq_mapping_arr[7].strict_priority=1;
cosq_mapping_arr[7].fc_is_uc_only=0;
cosq_mapping_arr[8].cosq=8;
cosq_mapping_arr[8].num_ucq=0;
cosq_mapping_arr[8].num_mcq=0;
cosq_mapping_arr[8].strict_priority=1;
cosq_mapping_arr[8].fc_is_uc_only=0;
cosq_mapping_arr[9].cosq=9;
cosq_mapping_arr[9].num_ucq=0;
cosq_mapping_arr[9].num_mcq=0;
cosq_mapping_arr[9].strict_priority=0;
cosq_mapping_arr[9].fc_is_uc_only=0;
cosq_mapping_arr[10].cosq=10;
cosq_mapping_arr[10].num_ucq=0;
cosq_mapping_arr[10].num_mcq=0;
cosq_mapping_arr[10].strict_priority=0;
cosq_mapping_arr[10].fc_is_uc_only=0;
cosq_mapping_arr[11].cosq=11;
cosq_mapping_arr[11].num_ucq=0;
cosq_mapping_arr[11].num_mcq=0;
cosq_mapping_arr[11].strict_priority=0;
cosq_mapping_arr[11].fc_is_uc_only=0;
print bcm_cosq_schedq_mapping_set(0, profile_id, 12, cosq_mapping_arr);

/* Configure PFC receive for profile profile_id */
/* Priority 0 &4 are PFC-optimized */
bcm_cosq_pfc_class_map_config_t config_array2[8];
bcm_cosq_pfc_class_map_config_t_init(&config_array2[0]);
config_array2[0].pfc_enable = 1;
config_array2[0].cos_list_bmp |= 1 << 0;
config_array2[0].pfc_optimized = 0;
config_array2[1].pfc_enable = 1;
config_array2[1].cos_list_bmp |= 1 << 1;
config_array2[1].pfc_optimized = 0;
config_array2[2].pfc_enable = 1;
config_array2[2].cos_list_bmp |= 1 << 2;
config_array2[2].pfc_optimized = 1;
config_array2[3].pfc_enable = 1;
config_array2[3].cos_list_bmp |= 1 << 3;
config_array2[3].pfc_optimized = 1;
config_array2[4].pfc_enable = 1;
config_array2[4].cos_list_bmp |= 1 << 4;
config_array2[4].pfc_optimized = 0;
config_array2[5].pfc_enable = 1;
config_array2[5].cos_list_bmp |= 1 << 5;
config_array2[5].pfc_optimized = 1;
config_array2[6].pfc_enable = 1;
config_array2[6].cos_list_bmp |= 1 << 6;
config_array2[6].pfc_optimized = 1;
config_array2[7].pfc_enable = 1;
config_array2[7].cos_list_bmp |= 1 << 7;
config_array2[7].pfc_optimized = 0;

print bcm_cosq_pfc_class_config_profile_set(0, profile_id, 8, config_array2);

/* Map rx_port to use profile 1*/
print bcm_cosq_port_profile_set(0, rx_port1, bcmCosqProfilePFCAndQueueHierarchy,
        profile_id);

/*
   Table 5-6
 Cos    MMUQ0    MMUQ1    FC_UC    SP    PFC_opt    EB_grp(cos)  EB_group0  EB_group1  SP(0)  SP(1)
  7       7      -1         0      1       0           8           8          1         1      0
  6       6      11         1      1       1           7           7          5         1      1
  5       5      10         1      1       2           6           6          5         1      1
  4       4       9         0      1       0           5           5          5         1      1
  3       3       8         1      0       3           4           4          2         0      0
  2       2      -1         0      0       4           3           3          1         0      0
  1       1      -1         0      0       0           2           2          1         0      0
  0       0      -1         0      0       0           1           1          1         0      0

   */
