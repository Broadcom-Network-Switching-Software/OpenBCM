/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Struct definitions for structs used in mbist_cpu.c (TR 505)
 */

#define WRITE_TAP_CONTROL 0
#define RESET_TAP_CONTROL 1
#define READ_TAP_CONTROL 2
#define WRITE_TAP_DATA 3
#define WRITE_TAP_DATA_0x40000000 4
#define WRITE_TAP_DATA_0x40000000_LOOP 5
#define READ_TAP_DATA 6
#define MBIST_WAIT 7
#define CHECK_INITIAL_MBIST_STATUS 8
#define CHECK_FINAL_MBIST_STATUS 9

typedef struct mbist_cpu_s {
    char *mbist_name; /* Name of MBIST as it appears in the log */
    uint32 total_opcodes;
    uint8 *mbist_opcodes;
    uint32 *tap_control;
    uint32 *tap_data;
    uint8 *tap_data_loop_cnt;
    uint32 *initial_mbist_status;
    uint32 *final_mbist_status;
    uint32 waterfall;
    uint32 num_mem;
    struct mbist_cpu_s *mbist_wf_array;
} mbist_cpu_t;
