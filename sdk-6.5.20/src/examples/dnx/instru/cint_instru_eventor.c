/*
 * $Id: cint_instru_eventor.c 
 * $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 * File: cint_instru_eventor.c
 * Purpose: Eventor configuration
 */

/*
 * File: cint_instru_eventor.c
 * Purpose: Semantic test of eventor APIs.
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint
 * cint_instru_eventor_builder_set_get(unit, builder_id, buffer_size, nof_samples, header_length);
 * exit;
 */


uint8 header_data[] = { 0xFF,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
                        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
                        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
                        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
                        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88 ,0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
                        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98 ,0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
                        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8 ,0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
                        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8 ,0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
                        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 ,0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
                        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8 ,0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
                        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8 ,0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
                        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8 ,0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};


/*
* Check Eventor functions: 
*  bcm_instru_eventor_context_set
*  bcm_instru_eventor_context_get
*  bcm_instru_eventor_builder_set
*  bcm_instru_eventor_builder_get
*/
int
cint_instru_eventor_builder_set_get(
   int unit,
   int builder_id,
   int buffer_size,
   int nof_samples,
   int header_length)
{
    int rv = BCM_E_NONE;
    int buffer_size_thr =  buffer_size * nof_samples;
    bcm_instru_eventor_builder_conf_t builder_conf_set, builder_conf_get;
    int flags = 0;
    bcm_instru_eventor_context_conf_t context_conf_set, context_conf_get;
    int context_id;

    printf("cint_instru_eventor_builder_set_get: builder_id = %d, buffer_size = %d, nof_samples = %d, header_length = %d \n", builder_id, buffer_size, nof_samples, header_length);
    
    context_id = builder_id;
    /*
     * 1. Can simply use different pair of banks per context (but since there are 8 banks, there can be only 4 contexs ...)
     * 2. Can use same pair of banks but with different offset (bank size is 13K bytes)
     * Here, for simplicity, use #1 
     *  
     * Note: performing modulo 8 operation by & 0x7
     */
    context_conf_set.bank1 = context_id & 0x7;
    context_conf_set.bank2 = (context_id+1) & 0x7;
    context_conf_set.buffer_size = buffer_size;
    context_conf_set.buffer1_start = 0;
    context_conf_set.buffer2_start = 256;
    
    rv =  bcm_instru_eventor_context_set(unit, flags, context_id, bcmEventorContextTypeRx, &context_conf_set);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_eventor_context_set failed! Error = %d\n", rv);
        return rv;
    }
    
    rv =  bcm_instru_eventor_context_get(unit, flags, context_id, bcmEventorContextTypeRx, &context_conf_get);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_eventor_context_get failed! Error = %d\n", rv);
        return rv;
    }
    
    /*
    * Compare context set and get fields:
    */
    if (context_conf_set.buffer_size != context_conf_get.buffer_size)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer_size = %d, get.buffer_size = %d \n", context_conf_set.buffer_size, context_conf_get.buffer_size);
        return BCM_E_PARAM;
    }
    if (context_conf_set.bank1 != context_conf_get.bank1)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.bank1 = %d, get.bank1 = %d \n", context_conf_set.bank1, context_conf_get.bank1);
        return BCM_E_PARAM;
    }
    if (context_conf_set.bank2 != context_conf_get.bank2)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.bank2 = %d, get.bank2 = %d \n", context_conf_set.bank2, context_conf_get.bank2);
        return BCM_E_PARAM;
    }
    if (context_conf_set.buffer1_start != context_conf_get.buffer1_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer1_start = %d, get.buffer1_start = %d \n", context_conf_set.buffer1_start, context_conf_get.buffer1_start);
        return BCM_E_PARAM;
    }
    if (context_conf_set.buffer2_start != context_conf_get.buffer2_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer2_start = %d, get.buffer2_start = %d \n", context_conf_set.buffer2_start, context_conf_get.buffer2_start);
        return BCM_E_PARAM;
    }

    
    builder_conf_set.flags = 0;
    builder_conf_set.thresh_size = buffer_size_thr;
    builder_conf_set.thresh_time = BCM_INSTRU_EVENTOR_TIMEOUT_NONE;
    builder_conf_set.header_length = header_length;
    builder_conf_set.header_data = header_data;

    rv = bcm_instru_eventor_builder_set(unit, flags, builder_id, &builder_conf_set);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_eventor_builder_set failed! Error = %d\n", rv);
        return rv;
    }

    uint8 header_data_get[128];
    builder_conf_get.header_data = header_data_get;
    rv = bcm_instru_eventor_builder_get(unit, flags, builder_id, &builder_conf_get);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_eventor_builder_get failed! Error = %d\n", rv);
        return rv;
    }

    /*
    * Compare builder set and get fields:
    */
    if (builder_conf_set.thresh_size != builder_conf_get.thresh_size)
    {
        printf("Error! bcm_instru_eventor_builder_set/get failed! set.thresh_size = %d, get.thresh_size = %d \n", builder_conf_set.thresh_size, builder_conf_get.thresh_size);
        return BCM_E_PARAM;
    }
    if (builder_conf_set.thresh_time != builder_conf_get.thresh_time)
    {
        printf("Error! bcm_instru_eventor_builder_set/get failed! set.thresh_time = %d, get.thresh_time = %d \n", builder_conf_set.thresh_time, builder_conf_get.thresh_time);
        return BCM_E_PARAM;
    }
    if (builder_conf_set.header_length != builder_conf_get.header_length)
    {
        printf("Error! bcm_instru_eventor_builder_set/get failed! set.header_length = %d, get.header_length = %d \n", builder_conf_set.header_length, builder_conf_get.header_length);
        return BCM_E_PARAM;
    }

    int i;
    for (i=0; i < header_length; i++)
    {
        if (builder_conf_set.header_data[i] != builder_conf_get.header_data[i])
        {
            printf("Error! bcm_instru_eventor_builder_set/get failed! set.header_data[%d] = %d, get.header_data[%d] = %d \n", i, i, builder_conf_set.header_data[i], builder_conf_get.header_data[i]);
            return BCM_E_PARAM;
        }
    }

    printf("cint_instru_eventor_builder_set_get: builder_id PASS \n");
    return rv;
}

/*
 *  set Eventor builder for tx eventor testing:
 */
int
cint_instru_eventor_tx_builder_set(
        int unit,
        int builder_id)
{
    bcm_instru_eventor_builder_conf_t builder_conf={0};
    int rv;
    int flags=0;

    uint8 header_data[64] = {
            0xd0, 0x18, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x11, 0x08, 0x99, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00};

    printf("cint_instru_eventor_tx_builder_set: builder_id = %d\n", builder_id);


    builder_conf.flags = 0;
    builder_conf.thresh_size = 1580/4;
    builder_conf.thresh_time = BCM_INSTRU_EVENTOR_TIMEOUT_NONE;
    builder_conf.header_length = 16;
    builder_conf.header_data = header_data;
    /*
     * Set eventor Tx builder
     */
    rv = bcm_instru_eventor_builder_set(unit, flags, builder_id, &builder_conf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_builder_set\n");
        return rv;
    }

    printf("cint_instru_eventor_tx_builder_set: PASS \n");
    return rv;
}

/*
*  Check Eventor functions:
 * bcm_instru_eventor_context_set
 * bcm_instru_eventor_context_get
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint
 * cint_instru_eventor_builder_set_get(unit, builder_id, buffer_size, nof_samples, header_length);
 * exit;
*/
int
cint_instru_eventor_tx_context_set(
        int unit, int context_id,
        int bank1, int bank2,
        int buffer_size, int buffer1_start, int buffer2_start,
        int program1_start, int program2_start, int builders,
        int nof_events_to_aggregate, int offset)
{
    int rv;
    uint32 flags = 0;
    bcm_instru_eventor_context_conf_t context_conf_set={0};
    bcm_instru_eventor_operation_t operations[10];
    int reg_id;
    int block_instance_id;
    int mem_id;

    printf("cint_eventor_utils_context_set: context_id = %d, buffer_size = %d, nof_events_to_aggregate = %d \n", context_id, buffer_size, nof_events_to_aggregate);

    /* init eventor TX context */
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM0", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 0, &operations[0].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM0", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 1, &operations[1].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM0", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 2, &operations[2].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_reg_info_get(unit, "TDU_WRITE_REQUEST_PATH_COUNTERS.TDU0", &reg_id, &block_instance_id);
    rv = bcm_instru_get_register_access_info(unit, 0, reg_id, 0, 0, &operations[3].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_register_access_info\n");
        return rv;
    }
    diag_reg_info_get(unit, "TDU_READ_REQUEST_PATH_COUNTERS.TDU0", &reg_id, &block_instance_id);
    rv = bcm_instru_get_register_access_info(unit, 0, reg_id , 0, 0, &operations[4].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_register_access_info\n");
        return rv;
    }
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM1", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 0, &operations[5].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM1", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 1, &operations[6].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_mem_info_get(unit, "CGM_INSTRUMENTATION_MEMORY[0].CGM1", &mem_id, &block_instance_id);
    rv = bcm_instru_get_memory_access_info(unit, 0, mem_id, block_instance_id, 0, 2, &operations[7].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_memory_access_info\n");
        return rv;
    }
    diag_reg_info_get(unit, "TDU_WRITE_REQUEST_PATH_COUNTERS.TDU1", &reg_id, &block_instance_id);
    rv = bcm_instru_get_register_access_info(unit, 0, reg_id, 1, 0, &operations[8].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_register_access_info\n");
        return rv;
    }
    diag_reg_info_get(unit, "TDU_READ_REQUEST_PATH_COUNTERS.TDU1", &reg_id, &block_instance_id);
    rv = bcm_instru_get_register_access_info(unit, 0, reg_id,  1, 0, &operations[9].access_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_get_register_access_info\n");
        return rv;
    }

    context_conf_set.bank1 = bank1;
    context_conf_set.bank2 = bank2;
    context_conf_set.buffer_size = buffer_size;
    context_conf_set.buffer1_start = buffer1_start;
    context_conf_set.buffer2_start = buffer2_start;
    context_conf_set.program1_start = program1_start;
    context_conf_set.program2_start = program2_start;
    context_conf_set.builders = builders;
    context_conf_set.nof_operations = 10;
    context_conf_set.operations = operations;

    operations[0].flags = 0;
    operations[1].flags = 0;
    operations[2].flags = 0;
    operations[3].flags = 0;
    operations[4].flags = 0;
    operations[5].flags = 0;
    operations[6].flags = 0;
    operations[7].flags = 0;
    operations[8].flags = 0;
    operations[9].flags = 0;
    operations[0].buf_offset = 0 + offset;
    operations[1].buf_offset = 6 + offset;
    operations[2].buf_offset = 12 + offset;
    operations[3].buf_offset = 18 + offset;
    operations[4].buf_offset = 23 + offset;
    operations[5].buf_offset = 29 + offset;
    operations[6].buf_offset = 35 + offset;
    operations[7].buf_offset = 41 + offset;
    operations[8].buf_offset = 47 + offset;
    operations[9].buf_offset = 52 + offset;

    printf("cint_eventor_utils_context_set: bank1 = %d, bank2 = %d\n", context_conf_set.bank1, context_conf_set.bank2);

    /*
     * Set eventor Tx context
     */
    rv = bcm_instru_eventor_context_set(unit, flags, context_id, bcmEventorContextTypeTx, &context_conf_set);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_context_set\n");
        return rv;
    }

    printf("cint_instru_eventor_tx_context_set: PASS \n");
    return rv;
}


/*
*  Check Eventor functions:
 * bcm_instru_eventor_context_set
 * bcm_instru_eventor_context_get
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint
 * cint_instru_eventor_builder_set_get(unit, builder_id, buffer_size, nof_samples, header_length);
 * exit;
*/
int
cint_instru_eventor_tx_context_set_get(
        int unit, int context_id,
        int bank1, int bank2,
        int buffer_size, int buffer1_start, int buffer2_start,
        int program1_start, int program2_start, int builders,
        int nof_events_to_aggregate, int offset)
{
    int rv;
    uint32 flags = 0;
    int buffer_size_thr = buffer_size * nof_events_to_aggregate;
    bcm_instru_eventor_context_conf_t context_conf_set={0}, context_conf_get={0};
    bcm_instru_eventor_operation_t operations[10];
    int reg_id;
    int block_instance_id;
    int mem_id;

    printf("cint_eventor_utils_context_set_get: context_id = %d, buffer_size = %d, nof_events_to_aggregate = %d, buffer_size_thr = %d \n", context_id, buffer_size, nof_events_to_aggregate, buffer_size_thr);

    rv = cint_instru_eventor_tx_context_set(unit, context_id, bank1, bank2, buffer_size, buffer1_start, buffer2_start, program1_start, program2_start, builders, nof_events_to_aggregate, offset);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_context_set\n");
        return rv;
    }

    rv =  bcm_instru_eventor_context_get(unit, flags, context_id, bcmEventorContextTypeTx, &context_conf_get);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_eventor_context_get failed! Error = %d\n", rv);
        return rv;
    }

    /*
    * Compare context set and get fields:
    */
    if (context_conf_set.buffer_size != context_conf_get.buffer_size)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer_size = %d, get.buffer_size = %d \n", context_conf_set.buffer_size, context_conf_get.buffer_size);
        return BCM_E_PARAM;
    }
    if (context_conf_set.bank1 != context_conf_get.bank1)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.bank1 = %d, get.bank1 = %d \n", context_conf_set.bank1, context_conf_get.bank1);
        return BCM_E_PARAM;
    }
    if (context_conf_set.bank2 != context_conf_get.bank2)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.bank2 = %d, get.bank2 = %d \n", context_conf_set.bank2, context_conf_get.bank2);
        return BCM_E_PARAM;
    }
    if (context_conf_set.buffer1_start != context_conf_get.buffer1_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer1_start = %d, get.buffer1_start = %d \n", context_conf_set.buffer1_start, context_conf_get.buffer1_start);
        return BCM_E_PARAM;
    }
    if (context_conf_set.buffer2_start != context_conf_get.buffer2_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.buffer2_start = %d, get.buffer2_start = %d \n", context_conf_set.buffer2_start, context_conf_get.buffer2_start);
        return BCM_E_PARAM;
    }

    if (context_conf_set.program1_start != context_conf_get.program1_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.program1_start = %d, get.program1_start = %d \n", context_conf_set.program1_start, context_conf_get.program1_start);
        return BCM_E_PARAM;
    }
    if (context_conf_set.program2_start != context_conf_get.program2_start)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.program2_start = %d, get.program2_start = %d \n", context_conf_set.program2_start, context_conf_get.program2_start);
        return BCM_E_PARAM;
    }

    if (context_conf_set.nof_operations != context_conf_get.nof_operations)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.nof_operations = %d, get.nof_operations = %d \n", context_conf_set.nof_operations, context_conf_get.nof_operations);
        return BCM_E_PARAM;
    }

    if (context_conf_set.nof_const_values != context_conf_get.nof_const_values)
    {
        printf("Error! bcm_instru_eventor_context_set/get failed! set.nof_const_values = %d, get.nof_const_values = %d \n", context_conf_set.nof_const_values, context_conf_get.nof_const_values);
        return BCM_E_PARAM;
    }

    printf("cint_instru_eventor_tx_context_set_get: PASS \n");
    return rv;
}
