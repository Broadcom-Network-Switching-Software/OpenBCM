/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TR test for CAMBIST.
 * The test has no CLI parameters. Simply call tr 517 from command line.
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

#ifdef BCM_TOMAHAWK_SUPPORT
#include "cambist.h"
#include "th_cambist.h"
#ifdef BCM_TOMAHAWK2_SUPPORT
#include "th2_cambist.h"
#endif /*BCM_TOMAHAWK2_SUPPORT*/
#ifdef BCM_TOMAHAWK3_SUPPORT
#include "th3_cambist.h"
#endif /*BCM_TOMAHAWK3_SUPPORT*/
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/format.h>
#include "td3_cambist.h"
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_MAVERICK2_SUPPORT
#include "mv2_cambist.h"
#endif /* BCM_MAVERICK2_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
#include "hx5_cambist.h"
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
#include "hr4_cambist.h"
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
#include "fb6_cambist.h"
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */

#define CAMBIST_TIMEOUT 100000

typedef struct cambist_test_s {
    cambist_t *cambist_array;
    uint32 total_cambist_count;
    uint32 total_failures;
    uint32 done_bit_mask;
    uint32 bist_success_mask;
    uint32 bad_input;
    uint32 test_fail;
    int  (*test_config_f)(int unit, int init_or_done);
    uint32 total_run_count;
} cambist_test_t;

static cambist_test_t *cambist_test_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      get_cambist_array
 * Purpose:
 *      Populate members of cambist_test_t depending on the chip under test.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

static void
get_cambist_array(int unit)
{
    uint16 dev_id, drv_dev_id, drv_rev_id;
    uint8 rev_id;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    soc_cm_get_id (unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if ((dev_id) == BCM56980_DEVICE_ID) {
        cli_out("\nPopulating memory array for Tomahawk3");
        cambist_test_p->cambist_array = th3_cambist_array;
        cambist_test_p->total_cambist_count = TH3_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = TH3_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = TH3_BIST_SUCCESS_MASK;
    } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    if ((dev_id & 0xfff0) == BCM56970_DEVICE_ID) {
        cli_out("\nPopulating memory array for Tomahawk2");
        cambist_test_p->cambist_array = th2_cambist_array;
        cambist_test_p->total_cambist_count = TH2_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = TH2_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = TH2_BIST_SUCCESS_MASK;
    } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (((dev_id ) == BCM56870_DEVICE_ID) ||
        ((dev_id ) == BCM56873_DEVICE_ID)){
        cli_out("\nPopulating memory array for Trident3");
        cambist_test_p->cambist_array = td3_cambist_array;
        cambist_test_p->total_cambist_count = TD3_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = TD3_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = TD3_BIST_SUCCESS_MASK;
        cambist_test_p->total_run_count = 0;
    } else
#endif
#ifdef BCM_MAVERICK2_SUPPORT
    if (((dev_id ) == BCM56770_DEVICE_ID) ||
        ((dev_id ) == BCM56771_DEVICE_ID)){
        cli_out("\nPopulating memory array for Maverick2");
        cambist_test_p->cambist_array = mv2_cambist_array;
        cambist_test_p->total_cambist_count = MV2_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = MV2_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = MV2_BIST_SUCCESS_MASK;
        cambist_test_p->total_run_count = 0;
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if ((drv_dev_id) == BCM56370_DEVICE_ID){
        cli_out("\nPopulating memory array for Helix5");
        cambist_test_p->cambist_array = hx5_cambist_array;
        cambist_test_p->total_cambist_count = HX5_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = HX5_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = HX5_BIST_SUCCESS_MASK;
        cambist_test_p->total_run_count = 0;
    } else
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if ((drv_dev_id) == BCM56275_DEVICE_ID){
        cli_out("\nPopulating memory array for Hurricane4");
        cambist_test_p->cambist_array = hr4_cambist_array;
        cambist_test_p->total_cambist_count = HR4_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = HR4_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = HR4_BIST_SUCCESS_MASK;
        cambist_test_p->total_run_count = 0;
    } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if ((drv_dev_id) == BCM56470_DEVICE_ID){
        cli_out("\nPopulating memory array for firebolt6");
        cambist_test_p->cambist_array = fb6_cambist_array;
        cambist_test_p->total_cambist_count = FB6_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = FB6_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = FB6_BIST_SUCCESS_MASK;
        cambist_test_p->total_run_count = 0;
    } else
#endif

    if (dev_id == BCM56960_DEVICE_ID) {
        cli_out("\nPopulating memory array for Tomahawk");
        cambist_test_p->cambist_array = th_cambist_array;
        cambist_test_p->total_cambist_count = TH_TOTAL_CAMBIST_COUNT;
        cambist_test_p->done_bit_mask = TH_DONE_BIT_MASK;
        cambist_test_p->bist_success_mask = TH_BIST_SUCCESS_MASK;
    } else {
        cli_out("\nUnsupported chip");
        cambist_test_p->bad_input = 1;
    }
}

/*
 * Function:
 *      run_cambist
 * Purpose:
 *      Run a CAMBIST for a TCAM.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      cambist_p: Pointer to structure of type cambist_t.
 *
 * Returns:
 *     Nothing
 */

static void
run_cambist(int unit, cambist_t *cambist_p)
{
    int count;
    uint32 bist_en;
    uint32 bist_status = 0;
    uint64 bist_status_64;
    uint32 bist_done;
    uint32 bist_success;
    uint64 zero_64;
    int timeout = CAMBIST_TIMEOUT;

#if defined(BCM_TRIDENT3_SUPPORT)
    uint64 entbuf;
    uint32 val_lo = 0;
    uint32 val_hi = 0;
#endif /* BCM_TRIDENT3_SUPPORT */

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];
#if defined(BCM_TRIDENT3_SUPPORT)
    COMPILER_64_SET(entbuf, 0x0, 0x0);
#endif /* BCM_TRIDENT3_SUPPORT */
    COMPILER_64_SET(zero_64, 0x0, 0x0);
    COMPILER_64_SET(bist_status_64, 0x0, 0x0);

#ifdef BCM_HURRICANE4_SUPPORT
        /* Broadscan is disabled Skip test on these memories */
    if (SOC_IS_HURRICANE4(unit)) {
        if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1) &&
            (!sal_strcmp(cambist_p->cambist_name, "BSK_FTFP_LTS_A_KEY_GEN_TCAM") ||
             !sal_strcmp(cambist_p->cambist_name, "BSK_FTFP_TCAM"))) {
            cli_out("\nCambist on %s skipped", cambist_p->cambist_name);
            return;
        }
        if (!soc_feature(unit, soc_feature_xflow_macsec) &&
            (!sal_strcmp(cambist_p->cambist_name, "SC_TCAM") ||
             !sal_strcmp(cambist_p->cambist_name, "SP_TCAM"))) {
            cli_out("\nCambist on %s skipped", cambist_p->cambist_name);
            return;
        }
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit) &&
            _soc_fb6_reg_skip(unit, cambist_p->cambist_config)) {
        return;
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

    if (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config,
                            BIST_DBG_DATA_SLICE_OR_STATUS_SELf)) {
        soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                               BIST_DBG_DATA_SLICE_OR_STATUS_SELf, 0x00);
    }

    if (cambist_p->mode != NO_MODE && cambist_p->mode_field != NO_MODE_FIELD) {
        soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                               cambist_p->mode_field, cambist_p->mode);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if ((SOC_IS_TRIDENT3X(unit)) &&
        ((SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, FULLf)) ||
        (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, DATAf)))) {
        (void) soc_reg_get(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                                            0, &entbuf);
        val_lo = COMPILER_64_LO(entbuf);
        val_hi = COMPILER_64_HI(entbuf);
        soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                &val_lo, I_BIST_MODEf, cambist_p->mode == NO_MODE ? 0 :
                                         cambist_p->mode);
        COMPILER_64_SET(entbuf, val_hi, val_lo);
        (void) soc_reg_set(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                                            0,entbuf);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        if (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, I_BIST_MODEf)) {
            soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                   I_BIST_MODEf, cambist_p->mode == NO_MODE ? 0 :
                                                 cambist_p->mode);
        }
#ifdef INCLUDE_XFLOW_MACSEC
        else if (SOC_IS_HURRICANE4(unit) &&
                 soc_feature(unit, soc_feature_xflow_macsec) &&
                 SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, MODEf)) {
            soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                   MODEf, cambist_p->mode == NO_MODE ? 0 :
                                   cambist_p->mode);
        }
#endif /* INCLUDE_XFLOW_MACSEC */
    }

    for (count = 0; count < cambist_p->cambist_count; count++) {
        cli_out("\nRunning Cambist %s count %0d",
                                    cambist_p->cambist_name, count);

        timeout = CAMBIST_TIMEOUT;
        bist_done = 0;
        bist_success = 0;
        bist_en = 0x1 << count;

        if (cambist_p->cambist_enable  != NO_CONTROL_ENABLE_FIELD &&
            cambist_p->cambist_control != NO_CONTROL_REGISTER) {
             soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                                   cambist_p->cambist_enable, bist_en);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if ((SOC_IS_TRIDENT3X(unit)) &&
        ((SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, FULLf)) ||
        (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, DATAf)))) {
            COMPILER_64_SET(entbuf, 0x0, 0x0);
            (void) soc_reg_get(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                                            0, &entbuf);
            val_lo = COMPILER_64_LO(entbuf);
            val_hi = COMPILER_64_HI(entbuf);

            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_RST_Lf, 0);
            COMPILER_64_SET(entbuf, val_hi, val_lo);
            (void) soc_reg_set(unit,
                      cambist_p->cambist_config, REG_PORT_ANY,
                       0,entbuf);

            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_ENf, 0);
            COMPILER_64_SET(entbuf, val_hi, val_lo);
            (void) soc_reg_set(unit,
                      cambist_p->cambist_config, REG_PORT_ANY,
                      0,entbuf);

            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_RST_Lf, 1);
            COMPILER_64_SET(entbuf, val_hi, val_lo);
            (void) soc_reg_set(unit,
                        cambist_p->cambist_config, REG_PORT_ANY,
                        0,entbuf);

            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_ENf, bist_en);
            COMPILER_64_SET(entbuf, val_hi, val_lo);
            (void) soc_reg_set(unit,
                             cambist_p->cambist_config, REG_PORT_ANY,
                              0,entbuf);

        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            if (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, I_BIST_ENf)) {
                soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                       I_BIST_RST_Lf, 0);
                soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                       I_BIST_ENf, 0);
                soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                       I_BIST_RST_Lf, 1);
                soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                       I_BIST_ENf, bist_en);
            }
#ifdef INCLUDE_XFLOW_MACSEC
            else if (SOC_IS_HURRICANE4(unit) &&
                     soc_feature(unit, soc_feature_xflow_macsec) &&
                     SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, RST_Lf) &&
                     SOC_REG_FIELD_VALID(unit, cambist_p->cambist_control, BIST_ENf)) {
                soc_reg_field32_modify(unit, cambist_p->cambist_config,
                                       REG_PORT_ANY, RST_Lf, 0);
                soc_reg_field32_modify(unit, cambist_p->cambist_control,
                                       REG_PORT_ANY, BIST_ENf, 0);
                soc_reg_field32_modify(unit, cambist_p->cambist_config,
                                       REG_PORT_ANY, RST_Lf, 1);
                soc_reg_field32_modify(unit, cambist_p->cambist_control,
                                       REG_PORT_ANY, BIST_ENf, bist_en);
            }
#endif /* INCLUDE_XFLOW_MACSEC */
        }

        while (bist_done == 0 && timeout > 0) {
            (void) soc_reg_get(unit, cambist_p->cambist_status, REG_PORT_ANY,
                                                            0, &bist_status_64);

            bist_status = COMPILER_64_LO(bist_status_64);

            bist_done = bist_status & cambist_test_p->done_bit_mask;
            timeout--;
        }

        if (timeout > 0) {
            bist_success = bist_status & cambist_test_p->bist_success_mask;

            if (bist_success != 0) {
                cli_out("\nCambist %s count %0d passed.",
                                    cambist_p->cambist_name, count);
            } else {
                test_error(unit, "\n*ERROR: Cambist %s count %0d FAILED",
                             cambist_p->cambist_name, count);
                test_error(unit, "\n bist_status %d bist_done %d bist_success %d",
                             bist_status, bist_done, bist_success);
                cambist_test_p->test_fail = 1;
                cambist_test_p->total_failures++;
            }
        } else {
            test_error(unit, "\n*ERROR: Cambist %s count %0d TIMED OUT ",
                                   cambist_p->cambist_name, count);
            test_error(unit, "\n bist_status %d bist_done %d bist_success %d",
                                   bist_status, bist_done, bist_success);
            cambist_test_p->test_fail = 1;
            cambist_test_p->total_failures++;
        }

        cambist_test_p->total_run_count++;
#if defined(BCM_TRIDENT3_SUPPORT)
        if ((SOC_IS_TRIDENT3X(unit)) &&
        ((SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, FULLf)) ||
        (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, DATAf)))) {
            COMPILER_64_SET(entbuf, 0x0, 0x0);
            (void) soc_reg_get(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                                            0, &entbuf);
            val_lo = COMPILER_64_LO(entbuf);
            val_hi = COMPILER_64_HI(entbuf);
            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_RST_Lf, 0);
            soc_format_field32_set(unit, CAM_BIST_CONFIG_64_FORMATfmt,
                    &val_lo, I_BIST_ENf, 0);
            COMPILER_64_SET(entbuf, val_hi, val_lo);
            (void) soc_reg_set(unit,
                          cambist_p->cambist_config, REG_PORT_ANY,
                          0,entbuf);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */

        {
            if (SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, I_BIST_ENf)) {
                soc_reg_field32_modify(unit, cambist_p->cambist_config, REG_PORT_ANY,
                                   I_BIST_ENf, 0x0);
            }
#ifdef INCLUDE_XFLOW_MACSEC
            else if (SOC_IS_HURRICANE4(unit) &&
                     soc_feature(unit, soc_feature_xflow_macsec) &&
                     SOC_REG_FIELD_VALID(unit, cambist_p->cambist_config, RST_Lf) &&
                     SOC_REG_FIELD_VALID(unit, cambist_p->cambist_control, BIST_ENf)) {
                soc_reg_field32_modify(unit, cambist_p->cambist_config,
                                       REG_PORT_ANY, RST_Lf, 0);
                soc_reg_field32_modify(unit, cambist_p->cambist_control,
                                       REG_PORT_ANY, BIST_ENf, 0);
            }
#endif /* INCLUDE_XFLOW_MACSEC */
        }
        if (cambist_p->cambist_enable  != NO_CONTROL_ENABLE_FIELD &&
            cambist_p->cambist_control != NO_CONTROL_REGISTER) {
            soc_reg_field32_modify(unit, cambist_p->cambist_control, REG_PORT_ANY,
                                   cambist_p->cambist_enable, 0x0);
        }

        (void) soc_reg_set(unit, cambist_p->cambist_status,
                           REG_PORT_ANY, 0, zero_64);
    }
}

/*
 * Function:
 *      run_cambist
 * Purpose:
 *      Run a CAMBIST for a TCAM.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      cambist_p: Pointer to structure of type cambist_t.
 *
 * Returns:
 *     Nothing
 */

static void
run_all_cambists(int unit)
{
    int i;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    for (i = 0; i < cambist_test_p->total_cambist_count; i++) {
        run_cambist(unit, &cambist_test_p->cambist_array[i]);
    }
}

#ifdef BCM_TOMAHAWK2_SUPPORT
/*
 * init_or_done: 1: init, 0: done
 */
int
cambist_th2_cam_init(int unit, int init_or_done)
{
    int i, pipe;
    uint32 rval, fval;
    uint64 rval_64;
    soc_reg_t reg;
    soc_field_t tm, atom_disa;
    struct cam_dbgctrl_table_s {
        soc_reg_t reg;
        soc_field_t tm;
        soc_field_t atom_disable;
    } th2_cam_dbgctrl_table[] = {
        {VFP_CAM_CONTROL_SLICE_3_0r, TMf, ATOMICITY_DISABLEf},
        {L3_TUNNEL_CAM_DBGCTRL_64r, TMf, ATOMICITY_DISABLEf},
        {PARS_RAM_DBGCTRLr, FP_UDF_CAM_TMf, FP_UDF_CAM_ATOMICITY_DISABLEf},
        {VLAN_SUBNET_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {VP_CAM_DBGCTRLr, MY_STATION_TCAM_ENTRY_ONLY_TMf, ATOMICITY_DISABLEf},
        {L3_DEFIP_CAM_DBGCTRL0r, CAM_TMf, CAM_ATOMICITY_DISABLEf},
        {MPLS_CAM_DBGCTRL_0r, IP_MULTICAST_TCAM_TMf, IP_MULTICAST_TCAM_ATOMICITY_DISABLEf},
        {L2_USER_ENTRY_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {DST_COMPRESSION_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {SRC_COMPRESSION_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_LOGICAL_TABLE_SELECT_CAM_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {RSEL2_CAM_DBGCTRLr, ING_SNAT_ONLY_TMf, ATOMICITY_DISABLEf},
        {SW3_RAM_CONTROL_2r, CPU_COS_MAP_TCAM_TMf, CPU_COS_MAP_ATOMICITY_DISABLEf},
        {EFP_RAM_CONTROL_2_64r, EFP_CAM_TMf, EFP_TCAM_ATOMICITY_DISABLEf},
        {IFP_TCAM0_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM1_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM2_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM3_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM4_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM5_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM6_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM7_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM8_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM9_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM10_DBGCTRLr, TMf, ATOMICITY_DISABLEf},
        {IFP_TCAM11_DBGCTRLr, TMf, ATOMICITY_DISABLEf}
    };

    for (i = 0; i < COUNTOF(th2_cam_dbgctrl_table); i++) {
        if (SOC_REG_UNIQUE_ACC(unit, th2_cam_dbgctrl_table[i].reg) != NULL) {
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                reg = th2_cam_dbgctrl_table[i].reg;
                tm = th2_cam_dbgctrl_table[i].tm;
                atom_disa = th2_cam_dbgctrl_table[i].atom_disable;
                reg = SOC_REG_UNIQUE_ACC(unit, reg)[pipe];

                if (SOC_REG_IS_64(unit, reg)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval_64));
                    fval = soc_reg64_field32_get(unit, reg, rval_64, tm);
                    if (init_or_done) {
                        fval |=  (1 << 3); /* setting tm[3]=1 select switch clock source out of TCAM */
                    } else {
                        fval &= ~(1 << 3); /* setting tm[3]=0 use cam internal generic clock source */
                    }
                    soc_reg64_field32_set(unit, reg, &rval_64, tm, fval);
                    soc_reg64_field32_set(unit, reg, &rval_64, atom_disa, init_or_done ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval_64));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    fval = soc_reg_field_get(unit, reg, rval, tm);
                    if (init_or_done) {
                        fval |=  (1 << 3); /* setting tm[3]=1 select switch clock source out of TCAM */
                    } else {
                        fval &= ~(1 << 3); /* setting tm[3]=0 use cam internal generic clock source */
                    }
                    soc_reg_field_set(unit, reg, &rval, tm, fval);
                    soc_reg_field_set(unit, reg, &rval, atom_disa, init_or_done ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
                }
            }
        } else
        {
            reg = th2_cam_dbgctrl_table[i].reg;
            tm = th2_cam_dbgctrl_table[i].tm;
            atom_disa = th2_cam_dbgctrl_table[i].atom_disable;
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval_64));
                fval = soc_reg64_field32_get(unit, reg, rval_64, tm);
                if (init_or_done) {
                    fval |=  (1 << 3); /* setting tm[3]=1 select switch clock source out of TCAM */
                } else {
                    fval &= ~(1 << 3); /* setting tm[3]=0 use cam internal generic clock source */
                }
                soc_reg64_field32_set(unit, reg, &rval_64, tm, fval);
                soc_reg64_field32_set(unit, reg, &rval_64, atom_disa, init_or_done ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval_64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                fval = soc_reg_field_get(unit, reg, rval, tm);
                if (init_or_done) {
                    fval |=  (1 << 3); /* setting tm[3]=1 select switch clock source out of TCAM */
                } else {
                    fval &= ~(1 << 3); /* setting tm[3]=0 use cam internal generic clock source */
                }
                soc_reg_field_set(unit, reg, &rval, tm, fval);
                soc_reg_field_set(unit, reg, &rval, atom_disa, init_or_done ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK2_SUPPORT */
/*
 * init_or_done: 1: init, 0: done
 */
int
cambist_test_config(int unit, int init_or_done)
{
    uint16 dev_id;
    uint8  rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, init_or_done ? 0x1fffff: 0));
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, init_or_done ? 0x7ffff : 0));
        if  (rev_id == BCM56970_A0_REV_ID) {
             SOC_IF_ERROR_RETURN(cambist_th2_cam_init(unit, init_or_done));
        }
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if ((SOC_IS_TRIDENT3X(unit) && !SOC_IS_HURRICANE4(unit)) ||
        SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, init_or_done ? 0x1fffff: 0));
        SOC_IF_ERROR_RETURN
            (WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, init_or_done ? 0x7ffff : 0));
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return 0;
}

int
cambist_test_init(int unit, args_t *a, void **pa)
{
    int rv;
    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test_init");

    cambist_test_p = sal_alloc(sizeof(cambist_test_t), "cambist_test");
    sal_memset(cambist_test_p, 0, sizeof(cambist_test_t));
    cambist_test_parray[unit] = cambist_test_p;

    cambist_test_p->bad_input = 0;
    cambist_test_p->test_fail = 0;
    cambist_test_p->total_failures = 0;
    cambist_test_p->test_config_f  = cambist_test_config;

    get_cambist_array(unit);

    if (cambist_test_p->bad_input == 0) {
        rv = cambist_test_p->test_config_f(unit, 1);
        if (rv < 0) {
            return rv;
        }
    }

    return 0;
}

int
cambist_test(int unit, args_t *a, void *pa)
{
    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test");

    (void) soc_mem_scan_stop(unit);
    (void) bcm_linkscan_enable_set(unit, 0);
    soc_counter_stop(unit);

    if (cambist_test_p->bad_input == 1) {
        goto done;
    }

    run_all_cambists(unit);

done:

    return 0;
}

int
cambist_test_done(int unit, void *pa)
{
    int rv;

    cambist_test_t *cambist_test_p = cambist_test_parray[unit];

    cli_out("\nCalling cambist_test_done");

    if (cambist_test_p->bad_input == 1) {
        goto done;
    }

    rv = cambist_test_p->test_config_f(unit, 0);
    if (rv < 0) {
        cambist_test_p->test_fail = 1;
        goto done;
    }

    cli_out("\n");
    cli_out("\nTotal CAMBISTs run = %0d", cambist_test_p->total_run_count);
    cli_out("\nTotal failures = %0d", cambist_test_p->total_failures);

done:

    if (cambist_test_p->bad_input == 1) {
        cambist_test_p->test_fail = 1;
    }

    if (cambist_test_p->test_fail == 1) {
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }

    if (cambist_test_p->test_fail == 0) {
        cli_out("\n********* All CAMBIST tests passed *********");
    } else {
        test_error(unit, "********** ERROR: CAMBIST test failed *********");
    }
    cli_out("\n");

    sal_free(cambist_test_p);
    return rv;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
