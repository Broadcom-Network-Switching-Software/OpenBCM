/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * C file to put flowtracker resource allocation related api calls
 */

/*
Resource Config 1
=================
config add flowtracker_ipfix_observation_domain_id=1
config add flowtracker_num_unique_user_entry_keys=10
config add flowtracker_session_data_mem_banks=2:1:1
config add flowtracker_alu16_mem_banks=5:1:2
config add flowtracker_alu32_mem_banks=8:2:2
config add flowtracker_load8_mem_banks=4:2:2
config add flowtracker_load16_mem_banks=8:4:4
config add flowtracker_timestamp_mem_engines=2:1:1
config add flowtracker_chip_delay_offset=768
config add flowtracker_chip_delay_granularity=8
config add flowtracker_e2e_delay_offset=3
config add flowtracker_e2e_delay_granularity=6
config add flowtracker_ipat_delay_offset=7
config add flowtracker_ipat_delay_granularity=0
config add flowtracker_ipdt_delay_offset=15
config add flowtracker_ipdt_delay_granularity=8
config show flow
*/

int
ftv2_test_resource_config_alloc(int tc_id, uint32 debug_flags)
{
    int rv = BCM_E_NONE;

    if (!skip_log) {
        printf("[CINT] Resource Config - Dumping resource allocations across ft stages.\n");

        /* Dump Resource Config Variables */
        bshell(unit, "config show flow");

        /* Final Step Log */
        printf ("[CINT] Resource Config - Dumped resource config succesfully.\n");
    }

    return BCM_E_NONE;
}
