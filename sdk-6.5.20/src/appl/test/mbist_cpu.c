/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TR test for MBIST.
 * The test has no CLI parameters. Simply call tr 505 from command line.
 */
#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <sal/core/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>

#include "testlist.h"

#ifdef BCM_TRIDENT2_SUPPORT
#include "mbist_cpu.h"
#include "td2p_mbist_cpu.h"

typedef struct mbist_cpu_test_s {
    mbist_cpu_t *mbist_array;
    uint32 total_mbist_count;
    uint32 total_initial_failures;
    uint32 total_final_failures;
    uint32 total_failures;
    uint64 reset_tap_control;
    soc_reg_t tap_control;
    soc_reg_t tap_write_data;
    soc_reg_t tap_read_data;
    uint32 bad_input;
    uint32 test_fail;
} mbist_cpu_test_t;

static mbist_cpu_test_t *mbist_cpu_test_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      get_mbist_array
 * Purpose:
 *      Populate members of mbist_cpu_test_t depending on the chip under test.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
get_mbist_array(int unit)
{
    uint16 dev_id;
    uint8 rev_id;

    mbist_cpu_test_t *mbist_cpu_p = mbist_cpu_test_parray[unit];

    soc_cm_get_id (unit, &dev_id, &rev_id);

    if ((dev_id == BCM56860_DEVICE_ID) || (dev_id == BCM56470_DEVICE_ID)) {
        cli_out("\nPopulating memory array for Trident2+");
        mbist_cpu_p->tap_control = TOP_UC_TAP_CONTROLr;
        mbist_cpu_p->tap_write_data = TOP_UC_TAP_WRITE_DATAr;
        mbist_cpu_p->tap_read_data = TOP_UC_TAP_READ_DATAr;
        mbist_cpu_p->total_mbist_count = TD2P_TOTAL_MBIST_COUNT;
        mbist_cpu_p->mbist_array = td2p_mbist_array;
        COMPILER_64_SET(mbist_cpu_p->reset_tap_control, 0x0,
                                                        TD2P_RESET_TAP_CONTROL);
    } else {
        cli_out("\nUnsupported chip");
        mbist_cpu_p->bad_input = 1;
    }
}

/*
 * Function:
 *      run_mbist
 * Purpose:
 *      Run a MBIST for a memory controller and if it fails run MBISTs for
 *      individual memories. Function is recursive.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      mbist_cpu_p: Pointer to structure of type mbist_cpu_t.
 *
 * Returns:
 *     Nothing
 */

static void
run_mbist(int unit, mbist_cpu_t *mbist_p)
{
    uint32 idx_mbist_opcodes;
    uint32 idx_tap_control = 0;
    uint32 idx_tap_data = 0;
    uint32 idx_tap_data_loop_cnt = 0;
    uint32 idx_initial_mbist_status_cnt = 0;
    uint32 idx_final_mbist_status_cnt = 0;
    int i;
    int j;
    uint64 rdata;
    uint32 rdata32;
    uint32 fail = 0;
    uint64 data_0x40000000_64;

    mbist_cpu_test_t *mbist_cpu_p = mbist_cpu_test_parray[unit];

    cli_out("\n---- START MBIST %s ----", mbist_p->mbist_name);

    COMPILER_64_SET(data_0x40000000_64, 0x0, 0x40000000);

    for (idx_mbist_opcodes = 0;
         idx_mbist_opcodes < mbist_p->total_opcodes;
         idx_mbist_opcodes++) {
        switch (mbist_p->mbist_opcodes[idx_mbist_opcodes]) {
            case WRITE_TAP_CONTROL:
                WRITE_TOP_UC_TAP_CONTROLr(unit,
                    mbist_p->tap_control[idx_tap_control]);
                idx_tap_control++;
                break;
            case RESET_TAP_CONTROL:
                (void) soc_reg_set(unit, mbist_cpu_p->tap_control, 0, 0,
                            mbist_cpu_p->reset_tap_control);
                break;
            case READ_TAP_CONTROL:
                (void) soc_reg_get(unit, mbist_cpu_p->tap_control, 0, 0, &rdata);
                break;
            case WRITE_TAP_DATA:
                WRITE_TOP_UC_TAP_WRITE_DATAr(unit,
                            mbist_p->tap_data[idx_tap_data]);
                (void) soc_reg_set(unit, mbist_cpu_p->tap_control, 0, 0,
                            mbist_cpu_p->reset_tap_control);
                idx_tap_data++;
                break;
            case WRITE_TAP_DATA_0x40000000:
                (void) soc_reg_set(unit, mbist_cpu_p->tap_write_data, 0, 0,
                            data_0x40000000_64);
                (void) soc_reg_set(unit, mbist_cpu_p->tap_control, 0, 0,
                            mbist_cpu_p->reset_tap_control);
                break;
            case WRITE_TAP_DATA_0x40000000_LOOP:
                for (i = 0; i < mbist_p->
                                tap_data_loop_cnt[idx_tap_data_loop_cnt];
                     i++) {
                    (void) soc_reg_set(unit, mbist_cpu_p->tap_write_data, 0, 0,
                                data_0x40000000_64);
                    (void) soc_reg_set(unit, mbist_cpu_p->tap_control, 0, 0,
                                mbist_cpu_p->reset_tap_control);
                }
                idx_tap_data_loop_cnt++;
                break;
            case READ_TAP_DATA:
                (void) soc_reg_get(unit, mbist_cpu_p->tap_read_data,
                                   0, 0, &rdata);
                break;
            case MBIST_WAIT:
                sal_usleep(50000);
                break;
            case CHECK_INITIAL_MBIST_STATUS:
                (void) soc_reg_get(unit, mbist_cpu_p->tap_read_data,
                                                                0, 0, &rdata);
                COMPILER_64_TO_32_LO(rdata32, rdata);

                if (rdata32
                        == mbist_p->initial_mbist_status
                                        [idx_initial_mbist_status_cnt]) {
                    cli_out("\nGot expected initial MBIST status for "
                            "%s", mbist_p->mbist_name);
                } else {
                    test_error(unit, "\n*ERROR: Expected initial MBIST status = 0x%08x"
                               ", Actual initial MBIST status = 0x%08x",
                               mbist_p->initial_mbist_status
                                                [idx_initial_mbist_status_cnt],
                               rdata32);
                    fail = 1;
                    mbist_cpu_p->test_fail = 1;
                    mbist_cpu_p->total_initial_failures++;
                    mbist_cpu_p->total_failures++;
                }

                idx_initial_mbist_status_cnt++;
                break;
            case CHECK_FINAL_MBIST_STATUS:
                (void) soc_reg_get(unit, mbist_cpu_p->tap_read_data, 0, 0, &rdata);
                COMPILER_64_TO_32_LO(rdata32, rdata);
                if (rdata32
                        == mbist_p->final_mbist_status
                                        [idx_final_mbist_status_cnt]) {
                    cli_out("\nGot expected final MBIST status for "
                            "%s", mbist_p->mbist_name);
                } else {
                    test_error(unit, "\n*ERROR: Expected final MBIST status = 0x%08x"
                               ", Actual final MBIST status = 0x%08x",
                               mbist_p->final_mbist_status
                                                [idx_final_mbist_status_cnt],
                               rdata32);
                    fail = 1;
                    mbist_cpu_p->test_fail = 1;
                    mbist_cpu_p->total_final_failures++;
                    mbist_cpu_p->total_failures++;
                }
                idx_final_mbist_status_cnt++;
                break;
        }
    }
    cli_out("\n---- END MBIST %s ----", mbist_p->mbist_name);

    if ((fail == 1) && (mbist_p->waterfall == 1)) {
        for (j = 0; j < mbist_p->num_mem; j++) {
            run_mbist(unit, &mbist_p->mbist_wf_array[j]);
        }
    }
}

/*
 * Function:
 *      mbist_cpu_test_init
 * Purpose:
 *      Test init function.
 *
 * Parameters:
 *      unit: StrataSwitch Unit #.
 */

int
mbist_cpu_test_init(int unit, args_t *a, void **pa)
{
    mbist_cpu_test_t *mbist_cpu_p = mbist_cpu_test_parray[unit];

    cli_out("\nCalling mbist_cpu_test_init");

    mbist_cpu_p = sal_alloc(sizeof(mbist_cpu_test_t), "mbist_cpu_test");
    sal_memset(mbist_cpu_p, 0, sizeof(mbist_cpu_test_t));
    mbist_cpu_test_parray[unit] = mbist_cpu_p;

    mbist_cpu_p->bad_input = 0;
    mbist_cpu_p->test_fail = 0;
    mbist_cpu_p->total_initial_failures = 0;
    mbist_cpu_p->total_final_failures = 0;
    mbist_cpu_p->total_failures = 0;

    get_mbist_array(unit);

    return 0;
}

/*
 * Function:
 *      mbist_cpu_test_init
 * Purpose:
 *      Runs all MBISTs.
 *
 * Parameters:
 *      unit: StrataSwitch Unit #.
 */

int
mbist_cpu_test(int unit, args_t *a, void *pa)
{
    int i;
    mbist_cpu_test_t *mbist_cpu_p = mbist_cpu_test_parray[unit];

    cli_out("\nCalling mbist_cpu_test");

    (void) soc_mem_scan_stop(unit);
    (void) bcm_linkscan_enable_set(unit, 0);
    soc_counter_stop(unit);

    if (mbist_cpu_p->bad_input == 1) {
        goto done;
    }

    for (i = 0; i < mbist_cpu_p->total_mbist_count; i++) {
        run_mbist(unit, &mbist_cpu_p->mbist_array[i]);
    }

done:

    return 0;
}

/*
 * Function:
 *      mbist_cpu_test_init
 * Purpose:
 *      Logs failure counts and frees memory.
 *
 * Parameters:
 *      unit: StrataSwitch Unit #.
 */


int
mbist_cpu_test_done(int unit, void *pa)
{
    int rv;

    mbist_cpu_test_t *mbist_cpu_p = mbist_cpu_test_parray[unit];

    cli_out("\nCalling mbist_cpu_test_done");

    if (mbist_cpu_p->bad_input == 1) {
        goto done;
    }

    cli_out("\nTotal failures = %0d", mbist_cpu_p->total_failures);
    cli_out("\nTotal initial failures = %0d",
                                mbist_cpu_p->total_initial_failures);
    cli_out("\nTotal final failures = %0d",
                                mbist_cpu_p->total_final_failures);

done:

    if (mbist_cpu_p->bad_input == 1) {
        mbist_cpu_p->test_fail = 1;
    }

    if (mbist_cpu_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }

    if (mbist_cpu_p->test_fail == 0) {
        cli_out("\n********* All MBIST tests passed *********");
    } else {
        test_error(unit, "********** ERROR: MBIST test failed *********");
    }
    cli_out("\n");

    sal_free(mbist_cpu_p);
    return rv;
}
#endif
/* BCM_TRIDENT2_SUPPORT */
