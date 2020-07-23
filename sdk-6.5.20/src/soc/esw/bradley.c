/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bradley.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/l2x.h>
#include <soc/soc_ser_log.h>

#ifdef BCM_BRADLEY_SUPPORT

/*
 * bradley chip driver functions.  
 */
soc_functions_t soc_bradley_drv_funs = {
    soc_bradley_misc_init,
    soc_bradley_mmu_init,
    soc_bradley_age_timer_get,
    soc_bradley_age_timer_max_get,
    soc_bradley_age_timer_set,
};

static soc_ser_functions_t _bradley_ser_functions;
STATIC void
_soc_hbs_mem_parity_info(int unit, int block_info_idx, int pipe,
                         soc_field_t field_enum, uint32 *minfo);

STATIC int
soc_bradley_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec;
    soc_timeout_t       to;

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1); 
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf,
                      SOC_IS_SC_CQ(unit) ? 0x8000 : 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2_Xr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2_Yr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf,
                      SOC_IS_SC_CQ(unit) ? 0x4000 : 0x2000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2_Xr(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2_Xr, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET X timeout\n"), unit));
            break;
        }
    } while (TRUE);
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2_Yr(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2_Yr, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET Y timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

#ifdef BCM_SCORPION_SUPPORT

int
soc_scorpion_pipe_select(int unit, int egress, int pipe_select) {
    soc_reg_t select_reg;

    select_reg = egress ? EGR_SBS_CONTROLr : SBS_CONTROLr;

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, select_reg, REG_PORT_ANY,
                                PIPE_SELECTf, pipe_select));
    return SOC_E_NONE;
}

/* Forward declaration */
STATIC int _soc_sc_pipe_parity_enable(int unit, int enable);

/*
 * The object of this function is to program the TDM arbiter table
 * so that the ports have enough bandwidth for the given device
 * port configuration. cf. Scorpion Arbiter document for details of
 * determining these settings.
 */
static int _sc_tdm_arbiter_table_1x[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 0, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0,
    -1
};

static int _sc_tdm_arbiter_table_1y[] = {
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 25, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 27, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 28,
    -1
};

static int _sc_tdm_arbiter_table_2x[] = {
    1, 5, 2, 6, 3, 7, 4, 8, 1, 9, 2, 10, 3, 5,
    4, 6, 1, 7, 2, 8, 3, 9, 4, 10, 1, 5, 2, 0,
    3, 6, 4, 7, 1, 8, 2, 9, 3, 10, 4, 5, 1, 6,
    2, 7, 3, 8, 4, 9, 1, 10, 2, 5, 3, 6, 4, 0,
    7, 1, 8, 2, 9, 3, 10, 4, 5, 1, 6, 2, 7, 3,
    8, 4, 9, 1, 10, 2, 5, 3, 6, 4, 7, 1, 8, 2, 0,
    9, 3, 10, 4, 5, 1, 6, 2, 7, 3, 8, 4, 9, 1,
    10, 2, 5, 3, 6, 4, 7, 1, 8, 2, 9, 3, 10, 4, 0,
    -1
};

static int _sc_tdm_arbiter_table_2y[] = {
    24, 20, 23, 19, 22, 18, 21, 17, 24, 16, 23, 15, 22, 20,
    21, 19, 24, 18, 23, 17, 22, 16, 21, 15, 24, 20, 23, 25,
    22, 19, 21, 18, 24, 17, 23, 16, 22, 15, 21, 20, 24, 19,
    23, 18, 22, 17, 21, 16, 24, 15, 23, 20, 22, 19, 21, 26,
    18, 24, 17, 23, 16, 22, 15, 21, 20, 24, 19, 23, 18, 22,
    17, 21, 16, 24, 15, 23, 20, 22, 19, 21, 18, 24, 17, 23, 27,
    16, 22, 15, 21, 20, 24, 19, 23, 18, 22, 17, 21, 16, 24,
    15, 23, 20, 22, 19, 21, 18, 24, 17, 23, 16, 22, 15, 21, 28,
    -1
};

static int _sc_tdm_arbiter_table_3x[] = {
    7, 8, 9, 10, 11, 12, 0x1f,
    7, 8, 9, 10, 11, 12, 0x1f,
    7, 8, 9, 10, 11, 12, 0x1f,
    7, 8, 9, 10, 11, 12, 0,
    -1
};

static int _sc_tdm_arbiter_table_3y[] = {
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 25,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 26,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 27,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 0x1f,
    13, 14, 15, 16, 17, 18, 28,
    -1
};

static int _sc_tdm_arbiter_table_4x[] = {
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    5, 6, 7, 8, 9, 10, 11, 12,
    0, -1
};

static int _sc_tdm_arbiter_table_4y[] = {
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    13, 14, 15, 16, 17, 18, 19, 20,
    0x1f, -1
};

static int _sc_tdm_arbiter_table_5x[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    0, -1
};

static int _sc_tdm_arbiter_table_5y[] = {
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 27,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 28,
    -1
};

static int _sc_tdm_arbiter_table_6x[] = {
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8, 9, 10, 11, 12,
    0, -1
};

static int _sc_tdm_arbiter_table_6y[] = {
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    17, 18, 19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 13, 14, 15, 16,
    25, -1
};

STATIC int
soc_sc_tdm_arbiter_init(int unit)
{
    int                 *x_list, *y_list;
    int                 arb_index, max_arb_index;
    arb_tdm_table_entry_t arb_entry;
    uint32              tdm_en;
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
    case BCM56820_DEVICE_ID:
    case BCM88732_DEVICE_ID:
        x_list = _sc_tdm_arbiter_table_1x;
        y_list = _sc_tdm_arbiter_table_1y;
        break;
    case BCM56821_DEVICE_ID:
        if (soc_property_get(unit, spn_BCM56821_20X12, 0)) {
            x_list = _sc_tdm_arbiter_table_5x;
            y_list = _sc_tdm_arbiter_table_5y;
        } else {
            x_list = _sc_tdm_arbiter_table_2x;
            y_list = _sc_tdm_arbiter_table_2y;
        }
        break;
    case BCM56822_DEVICE_ID:
        if (soc_property_get(unit, spn_BCM56822_8X16, 0)) {
            x_list = _sc_tdm_arbiter_table_4x;
        } else {
            x_list = _sc_tdm_arbiter_table_3x;
        }
        y_list = _sc_tdm_arbiter_table_1y;
        break;
    case BCM56823_DEVICE_ID:
        x_list = _sc_tdm_arbiter_table_3x;
        y_list = _sc_tdm_arbiter_table_3y;
        break;
    case BCM56825_DEVICE_ID:
        x_list = _sc_tdm_arbiter_table_6x;
        y_list = _sc_tdm_arbiter_table_6y;
        break;
    case BCM56725_DEVICE_ID:
        if (soc_property_get(unit, spn_BCM56725_16X16, 0)) {
            x_list = _sc_tdm_arbiter_table_4x;
            y_list = _sc_tdm_arbiter_table_4y;
        } else {
            x_list = _sc_tdm_arbiter_table_3x;
            y_list = _sc_tdm_arbiter_table_3y;
        }
        break;
    case BCM56720_DEVICE_ID:
    case BCM56721_DEVICE_ID:
        x_list = _sc_tdm_arbiter_table_4x;
        y_list = _sc_tdm_arbiter_table_4y;
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    max_arb_index = soc_mem_index_max(unit, ARB_TDM_TABLEm);

    /* Ingress pipe select X */
    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, FALSE, SOC_PIPE_SELECT_X));

    arb_index = 0;
    sal_memset(&arb_entry, 0, sizeof(arb_entry));
    while ((arb_index <= max_arb_index) && (x_list[arb_index] >= 0)) {
        soc_mem_field32_set(unit, X_ARB_TDM_TABLEm, &arb_entry,
                            PORT_NUMf, x_list[arb_index]);
        if (x_list[arb_index + 1] < 0) {
            soc_mem_field32_set(unit, X_ARB_TDM_TABLEm, &arb_entry,
                                WRAP_ENf, 1);
        }
        /* MMU table */
        SOC_IF_ERROR_RETURN
            (WRITE_X_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                    arb_index, &arb_entry));
        /* Ingress (X) table */
        SOC_IF_ERROR_RETURN
            (WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                  arb_index, &arb_entry));
        arb_index++;
    }

    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, FALSE, SOC_PIPE_SELECT_Y));
    arb_index = 0;
    sal_memset(&arb_entry, 0, sizeof(arb_entry));
    while ((arb_index <= max_arb_index) && (y_list[arb_index] >= 0)) {
        soc_mem_field32_set(unit, Y_ARB_TDM_TABLEm, &arb_entry,
                            PORT_NUMf, y_list[arb_index]);
        if (y_list[arb_index + 1] < 0) {
            soc_mem_field32_set(unit, Y_ARB_TDM_TABLEm, &arb_entry,
                                WRAP_ENf, 1);
        }
        /* MMU table */
        SOC_IF_ERROR_RETURN
            (WRITE_Y_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                    arb_index, &arb_entry));
        /* Ingress (Y) table */
        SOC_IF_ERROR_RETURN
            (WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                  arb_index, &arb_entry));
        arb_index++;
    }

    /* While still in Y pipe */
    tdm_en = 1;
    SOC_IF_ERROR_RETURN(WRITE_TDM_ENr(unit, tdm_en));

    /* Return to default */
    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, FALSE, SOC_PIPE_SELECT_X));

    /* Turn everything on */
    SOC_IF_ERROR_RETURN(WRITE_TDM_ENr(unit, tdm_en));
    SOC_IF_ERROR_RETURN(WRITE_X_TDM_ENr(unit, tdm_en));
    SOC_IF_ERROR_RETURN(WRITE_Y_TDM_ENr(unit, tdm_en));

    return SOC_E_NONE;
}

STATIC int
soc_sc_cam_tuning(int unit)
{
    int i, pipe_select;
    int defip_cam_tm_val, fp_cam_tm_val, vfp_cam_tm_val, efp_cam_tm_val;
    uint32 rval;

    defip_cam_tm_val = soc_property_get(unit, spn_DEFIP_CAM_TM, 0x10);
    fp_cam_tm_val = soc_property_get(unit, spn_FP_CAM_TM, 0x10);
    vfp_cam_tm_val = soc_property_get(unit, spn_VFP_CAM_TM, 0x10);
    efp_cam_tm_val = soc_property_get(unit, spn_EFP_CAM_TM, 0x10);
     

    for (i = 0; i < 2; i++) {
        pipe_select = (i == 0) ? SOC_PIPE_SELECT_Y : SOC_PIPE_SELECT_X;

        /* Ingress pipe select */
        SOC_IF_ERROR_RETURN
            (soc_scorpion_pipe_select(unit, FALSE, pipe_select));
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_FP_CAM_CONTROL_TM_7_THRU_0r(unit, &rval));
        soc_reg_field_set(unit, FP_CAM_CONTROL_TM_7_THRU_0r,
                          &rval, ALL_TCAMS_TM_7_0f, fp_cam_tm_val);
        SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));
        
        if (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE) {
            continue;
        }

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_CAM_DBGCTRL0r(unit, &rval));
        soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r,
                          &rval, CAM0_TMf, defip_cam_tm_val);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL0r(unit, rval));
        
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_VFP_CAM_CONTROL_TM_7_THRU_0r(unit, &rval));
        soc_reg_field_set(unit, VFP_CAM_CONTROL_TM_7_THRU_0r,
                          &rval, TMf, vfp_cam_tm_val);
        SOC_IF_ERROR_RETURN(WRITE_VFP_CAM_CONTROL_TM_7_THRU_0r(unit, rval));
        
        /* Egress pipe select */
        SOC_IF_ERROR_RETURN
            (soc_scorpion_pipe_select(unit, TRUE, pipe_select));
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_EFP_RAM_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, EFP_RAM_CONTROLr,
                          &rval, EFP_CAM_SAMf, efp_cam_tm_val);
        SOC_IF_ERROR_RETURN(WRITE_EFP_RAM_CONTROLr(unit, rval));
    }

    /* End of loop leaves pipe selects set to X */
    return SOC_E_NONE;
}
#endif

int
soc_bradley_misc_init(int unit)
{
    uint32              rval;
    uint64              reg64;
    int                 port;
    uint32              addr;
    soc_pbmp_t          pbmp;
    int                 divisor, dividend;
    uint16              dev_id;
    uint8               rev_id;
#if defined(BCM_HB_GW_SUPPORT)
    int i, tm_val;
#endif

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EPIPE Memories */
        SOC_IF_ERROR_RETURN(soc_bradley_pipe_mem_clear(unit));
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval,
                      METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

#ifdef BCM_HB_GW_SUPPORT
    if (soc_reg_field_valid(unit, CMIC_XGXS0_PLL_CONTROL_2r,
                            XGPLL_EN125f)) {
        /* coverity[dead_error_begin : FALSE] */
        SOC_IF_ERROR_RETURN(READ_CMIC_XGXS0_PLL_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_2r, &rval,
                          XGPLL_EN125f, 1);
        /* coverity[dead_error_begin : FALSE] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_XGXS0_PLL_CONTROL_2r(unit, rval));
    }
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        soc_field_t fields[3];
        uint32 values[3];

        /* Bypass control settings */
        if (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE) {
            rval = 0;
            soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                  IVXLT_BYPASS_ENABLEf, 1);
            soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                  IL3_BYPASS_ENABLEf, 1);
            soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                  IL3MC_BYPASS_ENABLEf, 1);
            soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                  IRSEL1_BYPASS_ENABLEf, 1);
            soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                  IRSEL2_BYPASS_ENABLEf, 1);
            if (SOC_SWITCH_BYPASS_MODE(unit) ==
                SOC_SWITCH_BYPASS_MODE_L3_AND_FP) {
                soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval,
                                      IFP_BYPASS_ENABLEf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_ING_BYPASS_CTRLr(unit, rval));
        
            rval = 0;
            soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval,
                                  EL3_BYPASSf, 1);
            soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval,
                                  EVXLT_BYPASSf, 1);
            soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval,
                                  EFP_BYPASSf, 1);
            SOC_IF_ERROR_RETURN(WRITE_EGR_BYPASS_CTRLr(unit, rval));
        }

        /* Enable dual hash on L2 and L3 tables */
        fields[0] = ENABLEf;
        values[0] = 1;
        fields[1] = HASH_SELECTf;
        values[1] = FB_HASH_CRC32_LOWER;
        fields[2] = INSERT_LEAST_FULL_HALFf;
        values[2] = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY,
                                     3, fields, values));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY,
                                     3, fields, values));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_0_4r,
                         &rval, REMAP_PORT_0f, 25);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_0_4r,
                         &rval, REMAP_PORT_1f, 26);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_0_4r,
                         &rval, REMAP_PORT_2f, 27);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_0_4r,
                         &rval, REMAP_PORT_3f, 28);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_0_4r,
                         &rval, REMAP_PORT_4f, 6);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_0_4r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_5_9r,
                         &rval, REMAP_PORT_5f, 5);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_5_9r,
                         &rval, REMAP_PORT_6f, 4);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_5_9r,
                         &rval, REMAP_PORT_7f, 3);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_5_9r,
                         &rval, REMAP_PORT_8f, 2);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_5_9r,
                         &rval, REMAP_PORT_9f, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_5_9r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_10_14r,
                         &rval, REMAP_PORT_10f, 12);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_10_14r,
                         &rval, REMAP_PORT_11f, 11);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_10_14r,
                         &rval, REMAP_PORT_12f, 10);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_10_14r,
                         &rval, REMAP_PORT_13f, 9);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_10_14r,
                         &rval, REMAP_PORT_14f, 8);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_10_14r(unit, rval));
        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_15_19r,
                         &rval, REMAP_PORT_15f, 7);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_15_19r,
                         &rval, REMAP_PORT_16f, 13);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_15_19r,
                         &rval, REMAP_PORT_17f, 14);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_15_19r,
                         &rval, REMAP_PORT_18f, 15);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_15_19r,
                         &rval, REMAP_PORT_19f, 16);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_15_19r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_20_24r,
                         &rval, REMAP_PORT_20f, 17);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_20_24r,
                         &rval, REMAP_PORT_21f, 18);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_20_24r,
                         &rval, REMAP_PORT_22f, 19);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_20_24r,
                         &rval, REMAP_PORT_23f, 20);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_20_24r,
                         &rval, REMAP_PORT_24f, 21);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_20_24r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_25_29r,
                         &rval, REMAP_PORT_25f, 22);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_25_29r,
                         &rval, REMAP_PORT_26f, 23);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_25_29r,
                         &rval, REMAP_PORT_27f, 24);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_25_29r,
                         &rval, REMAP_PORT_28f, 0);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_25_29r,
                         &rval, REMAP_PORT_29f, 29);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_30_34r,
                         &rval, REMAP_PORT_30f, 30);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_30_34r,
                         &rval, REMAP_PORT_31f, 31);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_30_34r,
                         &rval, REMAP_PORT_32f, 32);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_30_34r,
                         &rval, REMAP_PORT_33f, 33);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_30_34r,
                         &rval, REMAP_PORT_34f, 34);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_30_34r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_35_39r,
                         &rval, REMAP_PORT_35f, 35);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_35_39r,
                         &rval, REMAP_PORT_36f, 36);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_35_39r,
                         &rval, REMAP_PORT_37f, 37);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_35_39r,
                         &rval, REMAP_PORT_38f, 38);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_35_39r,
                         &rval, REMAP_PORT_39f, 39);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_35_39r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_40_44r,
                         &rval, REMAP_PORT_40f, 40);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_40_44r,
                         &rval, REMAP_PORT_41f, 41);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_40_44r,
                         &rval, REMAP_PORT_42f, 42);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_40_44r,
                         &rval, REMAP_PORT_43f, 43);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_40_44r,
                         &rval, REMAP_PORT_44f, 44);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_45_49r,
                         &rval, REMAP_PORT_45f, 45);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_45_49r,
                         &rval, REMAP_PORT_46f, 46);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_45_49r,
                         &rval, REMAP_PORT_47f, 47);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_45_49r,
                         &rval, REMAP_PORT_48f, 48);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_45_49r,
                         &rval, REMAP_PORT_49f, 49);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_45_49r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_50_54r,
                         &rval, REMAP_PORT_50f, 50);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_50_54r,
                         &rval, REMAP_PORT_51f, 51);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_50_54r,
                         &rval, REMAP_PORT_52f, 52);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_50_54r,
                         &rval, REMAP_PORT_53f, 53);
        soc_reg_field_set(unit,CMIC_LED_PORT_ORDER_REMAP_50_54r,
                         &rval, REMAP_PORT_54f, 54);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_50_54r(unit, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP_CTRLr(unit, &rval));
        soc_reg_field_set(unit,CMIC_LEDUP_CTRLr,
                         &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 3);
        soc_reg_field_set(unit,CMIC_LEDUP_CTRLr,
                         &rval, LEDUP_SCAN_START_DELAYf, 3);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP_CTRLr(unit, rval));
    }
#endif /* BCM_SCORPION_SUPPORT */

    /*
     * Egress Enable
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, EPC_LINK_BMAPr, &rval, PORT_BITMAPf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPr(unit, rval));

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    PBMP_GX_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_GX_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    if (soc_reg_field_valid(unit, GPORT_CONFIGr, GPORT_ENf)) {
        soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    }
    SOC_PBMP_ASSIGN(pbmp, PBMP_GX_ALL(unit));
    SOC_PBMP_AND(pbmp, PBMP_E_ALL(unit));
    PBMP_ITER(pbmp, port) {
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

    /* Enable CRC regen to avoid CRC in LAG failover */
    PBMP_E_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_DBGr, port, 
                                    ALWAYS_CRC_REGENf, 1));
    }

    /* XMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
    PBMP_XE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    }

    if (SOC_IS_SC_CQ(unit)) {
        rval = 0;
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, XPORT_MODE_BITSf, 1);
        PBMP_HG_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
        }
        PBMP_XE_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
        }

        /* Drop the packet when traffic exceeds available IP bandwidth */
        rval = 0;
        soc_reg_field_set(unit, PKT_DROP_ENABLEr, &rval,
                          PKT_DROP_ENABLEf, 1);
        PBMP_GX_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_PKT_DROP_ENABLEr(unit, port, rval));
        }

        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, XPORT_MODE_BITSf, 2);
        SOC_PBMP_ASSIGN(pbmp, PBMP_GX_ALL(unit));
        SOC_PBMP_AND(pbmp, PBMP_GE_ALL(unit));
        PBMP_ITER(pbmp, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
        }

        /* QG ports only */
        SOC_PBMP_ASSIGN(pbmp, PBMP_GE_ALL(unit));
        SOC_PBMP_REMOVE(pbmp, PBMP_GX_ALL(unit));
        rval = 0;
        soc_reg_field_set(unit, QGPORT_CONFIGr, &rval, CLR_CNTf, 1);
        PBMP_ITER(pbmp, port) {
            SOC_IF_ERROR_RETURN(WRITE_QGPORT_CONFIGr(unit, port, rval));
        }
        soc_reg_field_set(unit, QGPORT_CONFIGr, &rval, CLR_CNTf, 0);
        PBMP_ITER(pbmp, port) {
            SOC_IF_ERROR_RETURN(WRITE_QGPORT_CONFIGr(unit, port, rval));
        }
        if (soc_reg_field_valid(unit, QGPORT_CONFIGr, GPORT_ENf)) {
            soc_reg_field_set(unit, QGPORT_CONFIGr, &rval, GPORT_ENf, 1);
        }
        PBMP_ITER(pbmp, port) {
            SOC_IF_ERROR_RETURN(WRITE_QGPORT_CONFIGr(unit, port, rval));
        }

        /* Clear 8 beat accumulation on QG ports only */
        rval = 0;
        PBMP_ITER(pbmp, port) {
            SOC_IF_ERROR_RETURN(WRITE_EGR_ACCU_8BEATSr(unit, port, rval));
        }
    }

    if (soc_feature(unit, soc_feature_ctr_xaui_activity)) {
        PBMP_PORT_ITER(unit, port) {
            break;
        }
        addr = soc_reg_addr(unit, XPORT_CONFIGr, port, 0);
        addr &= ~(0x3F << SOC_REGIDX_BP);

        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        /* 
         * For correct LED operation, XPORT must be enabled also on
         * all unused  ports, however the soc_reg_addr function only
         * works on valid ports, so the registers address must be 
         * calculated 'manually'.
         */
        for (port = 0;  port < 20; port++) {
            if (!SOC_PORT_VALID(unit, port)) {
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 
                                    addr | (port << SOC_REGIDX_BP), rval));
            }
        }
    }

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    if (SOC_REG_IS_VALID(unit, EGR_VLAN_CONTROL_1r)) {
        rval = 0;
        if (SOC_REG_FIELD_VALID(unit, EGR_VLAN_CONTROL_1r, REMARK_OUTER_DOT1Pf)) {
            /* Enable pri/cfi remarking on egress ports. */
            soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                    1);
        }
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
        }
    }

    /*
     * ING_CONFIG
     */
    if (SOC_IS_SC_CQ(unit)) {
        SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              L3SRC_HIT_ENABLEf, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              L2DST_HIT_ENABLEf, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              APPLY_EGR_MASK_ON_L2f, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              APPLY_EGR_MASK_ON_L3f, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              ARP_VALIDATION_ENf, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              DRACO1_5_MIRRORf, 1);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              ARP_RARP_TO_FPf, 0x3); 
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              ARP_VALIDATION_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));
    } else {
        SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ING_CONFIGr, &rval, L3SRC_HIT_ENABLEf, 1);
        soc_reg_field_set(unit, ING_CONFIGr, &rval, L2DST_HIT_ENABLEf, 1);
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          APPLY_EGR_MASK_ON_L2f, 1);
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          APPLY_EGR_MASK_ON_L3f, 1);
        soc_reg_field_set(unit, ING_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, rval));
    }

    /* Backwards compatible mirroring by default */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                      DRACO_1_5_MIRRORING_MODE_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    /* Ingress mirror above */
    SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_mcast_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L2_RANGE, SOC_HBX_MULTICAST_RANGE_DEFAULT)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L3_RANGE, soc_mem_index_count(unit, L3_IPMCm))));

    rval = 0x01;	/* 125KHz I2C sampling rate */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_I2C_STATr(unit, rval));

#if defined(BCM_HB_GW_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        /*
         * Set reference clock (based on 180MHz core clock)
         * to be 180MHz * (1/36) = 5MHz
         * so MDC output frequency is 0.5 * 5MHz = 2.5MHz
         */

        divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 36);
        dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));
    } else
#endif
    if (SOC_IS_SC_CQ(unit)) {
        int ext_mdio_divisor, ext_mdio_dividend,
            int_mdio_divisor, int_mdio_dividend,
            stdma_divisor, stdma_dividend,
            i2c_divisor, i2c_dividend;

        /*
         * Premises of the calculation:
         *
         * Core clock speed
         * 56820 (220 MHz)
         * 56821/56720/56721 (225 MHz)
         * 56822/56823/56725 (245 MHz)
         * 56825 (290 MHz)
         *
         *  EXT_MDIO result = INT_MDIO result = 5 MHz
         *  I2C result / (2 * 10) = 250-400 kHz
         *      I2C result = 5-8 MHz
         *  STDMA result = 25 MHz
         *
         * result = core * dividend / divisor
         *      ==>
         * dividend : divisor = result : core
         */

        switch (dev_id) {
        case BCM56820_DEVICE_ID:
        case BCM88732_DEVICE_ID:
            ext_mdio_dividend = 1;
            ext_mdio_divisor = 44;
            int_mdio_dividend = 1;
            int_mdio_divisor = 44;
            stdma_dividend = 1;
            stdma_divisor = 9;
            i2c_dividend = 1;
            i2c_divisor = 40;
            break;
        case BCM56821_DEVICE_ID:
        case BCM56720_DEVICE_ID:
        case BCM56721_DEVICE_ID:
            ext_mdio_dividend = 1;
            ext_mdio_divisor = 45;
            int_mdio_dividend = 1;
            int_mdio_divisor = 45;
            stdma_dividend = 1;
            stdma_divisor = 9;
            i2c_dividend = 1;
            i2c_divisor = 40;
            break;
        case BCM56822_DEVICE_ID:
        case BCM56823_DEVICE_ID:
        case BCM56725_DEVICE_ID:
            ext_mdio_dividend = 1;
            ext_mdio_divisor = 49;
            int_mdio_dividend = 1;
            int_mdio_divisor = 49;
            stdma_dividend = 1;
            stdma_divisor = 10;
            i2c_dividend = 1;
            i2c_divisor = 49;
            break;
        case BCM56825_DEVICE_ID:
            ext_mdio_dividend = 1;
            ext_mdio_divisor = 58;
            int_mdio_dividend = 1;
            int_mdio_divisor = 58;
            stdma_dividend = 1;
            stdma_divisor = 12;
            i2c_dividend = 1;
            i2c_divisor = 58;
            break;
        default:
            return SOC_E_UNAVAIL;
        }

        divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR,
                                   ext_mdio_divisor);
        dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND,
                                    ext_mdio_dividend);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval,
                          DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval,
                          DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

        divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR,
                                   int_mdio_divisor);
        dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND,
                                    int_mdio_dividend);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval,
                          DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval,
                          DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

        divisor = soc_property_get(unit, spn_RATE_I2C_DIVISOR,
                                   i2c_divisor);
        dividend = soc_property_get(unit, spn_RATE_I2C_DIVIDEND,
                                    i2c_dividend);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval,
                          DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval,
                          DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));

        divisor = soc_property_get(unit, spn_RATE_STDMA_DIVISOR,
                                   stdma_divisor);
        dividend = soc_property_get(unit, spn_RATE_STDMA_DIVIDEND,
                                    stdma_dividend);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                          DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                          DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));
    }

#if defined(BCM_HB_GW_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        tm_val = soc_property_get(unit, spn_DEFIP_CAM_TM, 0x300);
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_CAM_DBGCTRL0r(unit, &rval));
        soc_reg_field_set(unit, L3_DEFIP_CAM_DBGCTRL0r, &rval,
                          CAM0_TMf, tm_val);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_DBGCTRL0r(unit, rval));

        tm_val = soc_property_get(unit, spn_FP_CAM_TM, 0x300);
        for (i = 0; i < 8; i++) {
            SOC_IF_ERROR_RETURN(READ_FP_CAM_CONTROLr(unit, i, &rval));
            soc_reg_field_set(unit, FP_CAM_CONTROLr, &rval, TMf, tm_val);
            SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROLr(unit, i, rval));
        }

        /* incorrect HB_GW rev A0 reset value */
        SOC_IF_ERROR_RETURN
            (READ_L2_AGE_DEBUGr(unit, &rval));
        soc_reg_field_set(unit, L2_AGE_DEBUGr, &rval,
                          AGE_COUNTf, 0x7ff);
        SOC_IF_ERROR_RETURN
            (WRITE_L2_AGE_DEBUGr(unit, rval));
    } 
#endif
#ifdef BCM_SCORPION_SUPPORT
    else if (SOC_IS_SC_CQ(unit)) {
        /* CAM tuning */
        if (SOC_SWITCH_BYPASS_MODE(unit) !=
            SOC_SWITCH_BYPASS_MODE_L3_AND_FP) {
            SOC_IF_ERROR_RETURN(soc_sc_cam_tuning(unit));
        }
    }
#endif
    /* Enable MMU parity error reporting */
    SOC_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_STATr(unit, 0));
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_ENr(unit, &rval));
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      MTRO_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_VLAN_TBL_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_IPMC_TBL_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_CELLLINK_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_PKTLINK_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_PKTHDR1_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ1_CELLHDR_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_VLAN_TBL_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_IPMC_TBL_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_CELLLINK_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_PKTLINK_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_PKTHDR1_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      TOQ0_CELLHDR_PAR_ERR_ENf, 1);
    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          MEM1_VLAN_TBL_PAR_ERR_ENf, 1);
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          MEM1_IPMC_TBL_PAR_ERR_ENf, 1);
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          ENQ_IPMC_TBL_PAR_ERR_ENf, 1);
    }
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ1_CELLCRC_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ0_CELLCRC_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ1_NOT_IP_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ0_NOT_IP_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ_PKTHDR0_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      DEQ_PKTHDR_CPU_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      CFAP_MEM_FAIL_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      CFAP_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      CCP_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      AGING_EXP_PAR_ERR_ENf, 1);
    soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                      AGING_CTR_PAR_ERR_ENf, 1);
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          WRED_PAR_ERR_ENf, 1);
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          DEQ0_LENGTH_PAR_ERR_ENf, 1);
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          DEQ1_LENGTH_PAR_ERR_ENf, 1);
        soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                          START_BY_START_ERR_ENf, 1);
    }
#endif
    SOC_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_ENr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    memset(&_bradley_ser_functions, 0, sizeof(soc_ser_functions_t));

    if (soc_property_get(unit, spn_PARITY_ENABLE, 1)) {
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
        _bradley_ser_functions._soc_ser_parity_error_intr_f = &soc_bradley_mmu_parity_error;
        soc_ser_function_register(unit, &_bradley_ser_functions);
    }
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        soc_field_t fields[] = { HGTRUNK_RES_ENf, LAG_RES_ENf };
        uint32 values[] = { 1, 1 };

        /* Enable vrf based l3 lookup by default. */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, VRF_MASKr, REG_PORT_ANY, MASKf, 0));

        /* Initialize TDM arbiter tables so all ports are operable */ 
        SOC_IF_ERROR_RETURN(soc_sc_tdm_arbiter_init(unit));

        /* Setup SW2_FP_DST_ACTION_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                     REG_PORT_ANY, 2, fields, values));

        /* Turn on ingress/egress parity */
        if (soc_property_get(unit, spn_PARITY_ENABLE, 1)) {
            SOC_IF_ERROR_RETURN(_soc_sc_pipe_parity_enable(unit, TRUE));
            _bradley_ser_functions._soc_ser_parity_error_intr_f = &soc_scorpion_parity_error;
            soc_ser_function_register(unit, &_bradley_ser_functions);
        }
    }

    if (soc_feature(unit, soc_feature_priority_flow_control)) {
        int class;
        soc_field_t unimac_fields[] =
            { PFC_REFRESH_ENf, PFC_REFRESH_TIMERf };
        soc_field_t bigmac_fields[] =
            { PP_REFRESH_ENf, PP_REFRESH_TIMERf };
        uint32 values[] = { 1, 0xc000 };

        /* Initialize the ports which might perform PFC */
        PBMP_GX_ITER(unit, port) {
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, MAC_PFC_REFRESH_CTRLr,
                                         port, 2, unimac_fields, values));
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, MAC_TXPPPCTRLr,
                                         port, 2, bigmac_fields, values));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UNIMAC_PFC_CTRLr, port,
                                        RX_PASS_PFC_FRMf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                        RX_PASS_PFC_FRMf, 1));
        }
        for (class = 0; class < 8; class++) {
            /* Initialize 1-1 mapping */
            SOC_IF_ERROR_RETURN(READ_PRIO2COSr(unit, class, &rval));
            soc_reg_field_set(unit, PRIO2COSr, &rval, COS0_7_BMPf,
                              (1 << class));
            SOC_IF_ERROR_RETURN(WRITE_PRIO2COSr(unit, class, rval));
        }
    }
#endif

    soc_intr_enable(unit, IRQ_MEM_FAIL);

    return SOC_E_NONE;
}

/*
 * Bradley/HUMV MMU Configuration
 *
 * Since port modes can be changed dynamically between HG and XE,
 * the default configuration will support HG ports, i.e. we enable
 * the SC and QM queues.
 *
 * Configuration supports only a single priority group (PG2).
 */

/* Configure all COS queues by default */
#ifndef BRADLEY_MMU_NUM_COS
#define BRADLEY_MMU_NUM_COS     8
#endif

/* Enable SC and QM queues by default */
#ifndef BRADLEY_MMU_USE_SC_QM
#define BRADLEY_MMU_USE_SC_QM   1
#endif

/* Standard Ethernet MTU (1 cell = 128 bytes) */
#ifndef BRADLEY_MMU_MTU_CELLS
#define BRADLEY_MMU_MTU_CELLS   12
#endif

/* Jumbo Frame MTU (1 cell = 128 bytes) */
#ifndef BRADLEY_MMU_JUMBO_CELLS
#define BRADLEY_MMU_JUMBO_CELLS 80
#endif

/* The following number of cells is based on packet simulations */
#define BRADLEY_MMU_PORT_MIN    72

/* Configure CPU port by default */
#ifndef BRADLEY_MMU_CONFIG_CPU
#define BRADLEY_MMU_CONFIG_CPU  1
#endif

#if BRADLEY_MMU_CONFIG_CPU == 1
#define PBMP_MMU_ITER           PBMP_ALL_ITER
#else
#define PBMP_MMU_ITER           PBMP_GX_ITER
#endif

#if BRADLEY_MMU_USE_SC_QM == 1
#define BRADLEY_MMU_SC_MIN      12
#define BRADLEY_MMU_QM_MIN      12
#else
#define BRADLEY_MMU_SC_MIN      0
#define BRADLEY_MMU_QM_MIN      0
#endif

#define SCORPION_PG_MAX         8

#define SCORPION_RESET_GRAN_SHIFT    4

/* Alternative disabling method b/c PORT_LIMIT_ENABLE bit is broken */
#define BRADLEY_OP_SHARED_LIMIT_DISABLE         0x3000
#define SCORPION_OP_SHARED_LIMIT_DISABLE        0x3fff

/* CFAP adjustments */
#define BRADLEY_CFAP_FULL_SET_POINT_OFFSET      0x100
#define BRADLEY_CFAP_FULL_RESET_POINT_OFFSET    0x1b0

/* This limit is in units of 8 cells */
#define SCORPION_COLOR_LIMIT                    0x7ff

/* An "alpha" value of 1/2 */
#define BRADLEY_OP_QUEUE_SHARED_LIMIT_CMIC_ENCODING 4
/* An "alpha" value of 1/4, but the encoding is different between devices */
#define SCORPION_OP_QUEUE_SHARED_LIMIT_CMIC_ENCODING 2

int
soc_bradley_mmu_init(int unit)
{
    uint32              rval, rval2, rval_cpu, wr_val;
    uint32              pg[SCORPION_PG_MAX];
    int                 port, i;
    int                 num_ports, num_cos;
    int                 total_cells;
    int                 pg_max_hdrm_limit;
    int                 global_hdrm_limit;
    int                 total_shared_limit;
    int                 q_min, q_shared_limit_encoding_cmic;
    int                 op_buffer_shared_limit;
    int                 num_pg, kcells;
    int                 op_shared_limit_disable;

    if (SOC_IS_SC_CQ(unit)) {
        kcells = 16;
        num_pg = 8;
        op_shared_limit_disable = SCORPION_OP_SHARED_LIMIT_DISABLE;
        q_shared_limit_encoding_cmic =
            SCORPION_OP_QUEUE_SHARED_LIMIT_CMIC_ENCODING;
    } else {
        kcells = 12;
        num_pg = 3;
        op_shared_limit_disable = BRADLEY_OP_SHARED_LIMIT_DISABLE;
        q_shared_limit_encoding_cmic =
            BRADLEY_OP_QUEUE_SHARED_LIMIT_CMIC_ENCODING;
    }

    /* Number of port to configure */
    num_ports = NUM_GX_PORT(unit) + BRADLEY_MMU_CONFIG_CPU;

    /* Number of COS queues */
    num_cos = BRADLEY_MMU_NUM_COS;

    /* Total number of cells */
    total_cells = kcells * 1024;

    /*
     * Ingress based threshholds
     */

    /* Enough to absorb in-flight data during PAUSE response */
    pg_max_hdrm_limit =
        SOC_IS_SC_CQ(unit) ? (3 * BRADLEY_MMU_MTU_CELLS) : 32;

    /* Per-device limit: 1 Ethernet MTU per port */
    global_hdrm_limit = num_ports * BRADLEY_MMU_MTU_CELLS;

    /* Use whatever is left over for shared cells */
    total_shared_limit = total_cells;
    total_shared_limit -= global_hdrm_limit;
    total_shared_limit -= (NUM_GX_PORT(unit) * pg_max_hdrm_limit);
    total_shared_limit -= (num_ports * BRADLEY_MMU_PORT_MIN);
    total_shared_limit -= (num_ports * BRADLEY_MMU_SC_MIN);
    total_shared_limit -= (num_ports * BRADLEY_MMU_QM_MIN);
    if (SOC_IS_SC_CQ(unit)) {
        
        /* Output port reserved */
        total_shared_limit -= (num_ports * num_cos * BRADLEY_MMU_MTU_CELLS);
    }
    rval = 0;
    soc_reg_field_set(unit, TOTAL_SHARED_LIMITr, &rval,
                      TOTAL_SHARED_LIMITf, total_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_TOTAL_SHARED_LIMITr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, 
                      GLOBAL_HDRM_LIMITf, global_hdrm_limit);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, PORT_MINr, &rval, PORT_MINf, BRADLEY_MMU_PORT_MIN);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MINr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval, 
                      PORT_MAX_PKT_SIZEf, BRADLEY_MMU_JUMBO_CELLS);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));
    }

    rval = 0;
    /* The following values may change when link is established */
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, PORT_SHARED_LIMITr, &rval, 
                      PORT_SHARED_DYNAMICf, 0);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_LIMITr(unit, port, rval));
    }

    rval = 0;
    if (SOC_IS_SC_CQ(unit)) {
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG0_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG1_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG2_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG3_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG4_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG5_THRESH_SELf, 0x7);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG6_THRESH_SELf, 0x7);
    } else {
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG0_THRESH_SELf, 0x6);
        soc_reg_field_set(unit, PG_THRESH_SELr, &rval, 
                          PG1_THRESH_SELf, 0x7);
    }

    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PG_THRESH_SELr(unit, port, rval));
    }

    if (SOC_IS_SC_CQ(unit)) {
        rval = 0;
        soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, 
                          PG_RESET_OFFSETf,
                          2 * BRADLEY_MMU_MTU_CELLS);
        PBMP_MMU_ITER(unit, port) {
            for (i = 0; i < num_pg; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PG_RESET_OFFSETr(unit, port, i, rval));
            }
        }
    } 
#ifdef BCM_HB_GW_SUPPORT
    else {
        rval = 0;
        soc_reg_field_set(unit, PG_RESET_SELr, &rval, 
                          PG0_RESET_SELf, 0x7);
        soc_reg_field_set(unit, PG_RESET_SELr, &rval, 
                          PG1_RESET_SELf, 0x7);
        soc_reg_field_set(unit, PG_RESET_SELr, &rval, 
                          PG2_RESET_SELf, 0x7);
        PBMP_MMU_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_SELr(unit, port, rval));
        }
    }
#endif

    /* Currently everything is zero, but keep code for reference */
    for (i = 0; i < num_pg; i++) {
        pg[i] = 0;
        soc_reg_field_set(unit, PG_RESET_FLOORr, &pg[i], 
                          PG_RESET_FLOORf, 0);
        
    }
    PBMP_MMU_ITER(unit, port) {
        for (i = 0; i < num_pg; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_FLOORr(unit, port, i, pg[i]));
        }
    }

    rval = 0;
    /* With only one PG in use PORT_MIN should be sufficient */
    soc_reg_field_set(unit, PG_MINr, &rval, 
                      PG_MINf, 0);
    PBMP_MMU_ITER(unit, port) {
        for (i = 0; i < num_pg; i++) {
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, port, i, rval));
        }
    }

    /* Note that only PG max is being used */
    for (i = 0; i < num_pg; i++) {
        pg[i] = 0;
        if (i == (num_pg - 1)) {
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_HDRM_LIMITf, pg_max_hdrm_limit);
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_GEf, 1);
        } else {
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_HDRM_LIMITf, 0);
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &pg[i], 
                              PG_GEf, 0);
        }
    }
    rval_cpu = 0;
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval_cpu, 
                      PG_HDRM_LIMITf, 0);
    soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval_cpu, 
                      PG_GEf, 0);
    PBMP_MMU_ITER(unit, port) {
        for (i = 0; i < num_pg; i++) {
            if (IS_CPU_PORT(unit, port) && (i == (num_pg - 1))) {
                wr_val = rval_cpu;
            } else {
                wr_val = pg[i];
            }
            SOC_IF_ERROR_RETURN
                (WRITE_PG_HDRM_LIMITr(unit, port, i, wr_val));
        }
    }

    if (SOC_IS_SC_CQ(unit)) {
        rval = 0;
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI0_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI1_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI2_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI3_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI4_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI5_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP0r, &rval, PRI6_GRPf, num_pg - 1);
        PBMP_MMU_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval));
        }
        rval = 0;
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI7_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI8_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI9_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI10_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI11_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI12_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRP1r, &rval, PRI13_GRPf, num_pg - 1);
        PBMP_MMU_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval));
        }
    }
#ifdef BCM_HB_GW_SUPPORT
    else {
        rval = 0;
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG0_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG1_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG2_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG3_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG4_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG5_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG6_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG7_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG8_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG9_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG10_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG11_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG12_GRPf, num_pg - 1);
        soc_reg_field_set(unit, PORT_PRI_GRPr, &rval, PG13_GRPf, num_pg - 1);
        PBMP_MMU_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRPr(unit, port, rval));
        }
    }
#endif

    rval = 0;
    soc_reg_field_set(unit, PORT_SC_MINr, &rval, 
                      PORT_SC_MINf, BRADLEY_MMU_SC_MIN);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_SC_MINr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, PORT_QM_MINr, &rval, 
                      PORT_QM_MINf, BRADLEY_MMU_QM_MIN);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_PORT_QM_MINr(unit, port, rval));
    }

    /* 
     * Output queue threshold settings
     */

    q_min = BRADLEY_MMU_MTU_CELLS;

    op_buffer_shared_limit = total_cells;
    op_buffer_shared_limit -= (q_min * num_ports * num_cos);
    op_buffer_shared_limit -= (num_ports * BRADLEY_MMU_SC_MIN);
    op_buffer_shared_limit -= (num_ports * BRADLEY_MMU_QM_MIN);

    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMITr, &rval, 
                      OP_BUFFER_SHARED_LIMITf, op_buffer_shared_limit);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMITr(unit, rval));

    /*
     * OP_SHARED_LIMIT should be ((op_buffer_shared_limit * 3) / 4)
     * OP_SHARED_RESET_VALUE should be (op_buffer_shared_limit / 4)
     *
     * Since PORT_LIMIT_ENABLE does not work, the above limits
     * can be disabled by configuring values larger than the
     * OP_BUFFER_SHARED_LIMIT value.
     */
    rval = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      OP_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      OP_SHARED_RESET_VALUEf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval, 
                      PORT_LIMIT_ENABLEf, 0);
    rval_cpu = 0;
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      OP_SHARED_LIMITf, op_shared_limit_disable);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      OP_SHARED_RESET_VALUEf, 0x733);
    soc_reg_field_set(unit, OP_PORT_CONFIGr, &rval_cpu, 
                      PORT_LIMIT_ENABLEf, 0);
    PBMP_MMU_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            wr_val = rval_cpu;
        } else {
            wr_val = rval;
        }
        SOC_IF_ERROR_RETURN(WRITE_OP_PORT_CONFIGr(unit, port, wr_val));
    }

    if (SOC_IS_SC_CQ(unit)) {
        PBMP_MMU_ITER(unit, port) {
            rval = 0;
            soc_reg_field_set(unit, OP_PORT_LIMIT_YELLOWr, &rval, 
                      OP_LIMIT_YELLOWf, SCORPION_COLOR_LIMIT);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_PORT_LIMIT_YELLOWr(unit, port, rval));
            rval = 0;
            soc_reg_field_set(unit, OP_PORT_LIMIT_REDr, &rval, 
                      OP_LIMIT_REDf, SCORPION_COLOR_LIMIT);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_PORT_LIMIT_REDr(unit, port, rval));

            rval = 0;
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOWr, &rval, 
                      Q_LIMIT_YELLOWf, SCORPION_COLOR_LIMIT);
            for (i = 0; i < num_cos; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_LIMIT_YELLOWr(unit, port, i, rval));
            }
#if BRADLEY_MMU_USE_SC_QM == 1
            for (i = 8; i <= 9; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_LIMIT_YELLOWr(unit, port, i, rval));
            }
#endif

            rval = 0;
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_REDr, &rval, 
                      Q_LIMIT_REDf, SCORPION_COLOR_LIMIT);
            for (i = 0; i < num_cos; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_LIMIT_REDr(unit, port, i, rval));
            }
#if BRADLEY_MMU_USE_SC_QM == 1
            for (i = 8; i <= 9; i++) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_LIMIT_REDr(unit, port, i, rval));
            }
#endif

        }

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOWr, &rval, 
                          OP_BUFFER_LIMIT_YELLOWf, SCORPION_COLOR_LIMIT);
        SOC_IF_ERROR_RETURN
            (WRITE_OP_BUFFER_LIMIT_YELLOWr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_REDr, &rval, 
                          OP_BUFFER_LIMIT_REDf, SCORPION_COLOR_LIMIT);
        SOC_IF_ERROR_RETURN
            (WRITE_OP_BUFFER_LIMIT_REDr(unit, rval));
    }

    rval = 0;
    rval2 = 0;
#ifdef BCM_HB_GW_SUPPORT
    if (soc_reg_field_valid(unit, OP_QUEUE_CONFIGr, Q_RESET_SELf)) {
        /* Make it 87.5 % of original threshold */
        soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                          Q_RESET_SELf, 0x7);
    } else 
#endif
    if (soc_reg_field_valid
               (unit, OP_QUEUE_RESET_OFFSETr, Q_RESET_OFFSETf)) {
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSETr, &rval2, 
                          Q_RESET_OFFSETf,
                          2 * BRADLEY_MMU_MTU_CELLS);
    }
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_MINf, q_min);
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_ENABLEf, 0x1);
    /* Use dynamic threshold limits */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_LIMIT_DYNAMICf, 0x1);
    /* Dynamic threshold limit is alpha of 4 for Bradley, .25 for Scorpion */
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval, 
                      Q_SHARED_LIMITf, SOC_IS_SC_CQ(unit) ? 0x2 : 0x7);
    rval_cpu = rval;
    soc_reg_field_set(unit, OP_QUEUE_CONFIGr, &rval_cpu, 
                      Q_SHARED_LIMITf, q_shared_limit_encoding_cmic);
    PBMP_MMU_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            wr_val = rval_cpu;
        } else {
            wr_val = rval;
        }
        for (i = 0; i < num_cos; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGr(unit, port, i, wr_val));
            if (soc_reg_field_valid
                (unit, OP_QUEUE_RESET_OFFSETr, Q_RESET_OFFSETf)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_RESET_OFFSETr(unit, port, i, rval2));
            }
        }
#if BRADLEY_MMU_USE_SC_QM == 1
        for (i = 8; i <= 9; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGr(unit, port, i, wr_val));
            if (soc_reg_field_valid
                (unit, OP_QUEUE_RESET_OFFSETr, Q_RESET_OFFSETf)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_OP_QUEUE_RESET_OFFSETr(unit, port, i, rval2));
            }
        }
#endif
    }

    rval = 0;
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, 
                      ASF_PKT_SIZEf, 0x3);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, 
                      ASF_QUEUE_SIZEf, 0x3);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, 
                      MOP_POLICYf, 0x7);
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, 
                      SOP_POLICYf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPCONFIGr, &rval, 
                      CFAPPOOLSIZEf, total_cells - 1);
    SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &rval, 
                      CFAPFULLSETPOINTf, 
                      total_cells - BRADLEY_CFAP_FULL_SET_POINT_OFFSET);
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &rval, 
                      CFAPFULLRESETPOINTf,
                      total_cells - BRADLEY_CFAP_FULL_RESET_POINT_OFFSET);
    SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, rval));

    /* No flow control for COS 0-7 */
    rval = 0;
    soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval, 
                      PORT_PRI_XON_ENABLEf, 0);
    PBMP_MMU_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));
    }

    /* Port enable */
    rval = 0;
    soc_reg_field_set(unit, INPUT_PORT_RX_ENABLEr, &rval, INPUT_PORT_RX_ENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLEr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_OUTPUT_PORT_RX_ENABLEr(unit, rval));

    return SOC_E_NONE;
}

#ifdef BCM_SCORPION_SUPPORT
soc_reg_t _soc_sc_parity_wred_reg[] = {
    WREDAVGQSIZE_CELLr,
    WREDCONFIG_CELLr,
    WREDPARAM_CELLr,
    WREDPARAM_YELLOW_CELLr,
    WREDPARAM_RED_CELLr,
    WREDPARAM_NONTCP_CELLr,
    PORT_WREDAVGQSIZE_CELLr,
    PORT_WREDCONFIG_CELLr,
    PORT_WREDPARAM_CELLr,
    PORT_WREDPARAM_YELLOW_CELLr,
    PORT_WREDPARAM_RED_CELLr,
    PORT_WREDPARAM_NONTCP_CELLr
};

soc_mem_t _soc_sc_parity_wred_mem[] = {
        MMU_WRED_CFG_CELLm,
        MMU_WRED_CFG_CELLm,
        MMU_WRED_THD_0_CELLm,
        MMU_WRED_THD_0_CELLm,
        MMU_WRED_THD_1_CELLm,
        MMU_WRED_THD_1_CELLm,
        MMU_WRED_PORT_CFG_CELLm,
        MMU_WRED_PORT_CFG_CELLm,
        MMU_WRED_PORT_THD_0_CELLm,
        MMU_WRED_PORT_THD_0_CELLm,
        MMU_WRED_PORT_THD_1_CELLm,
        MMU_WRED_PORT_THD_1_CELLm,
};
#define SOC_SC_WRED_PARITY_NUM         12
#endif

#ifdef BCM_SCORPION_SUPPORT
#define SOC_SC_MTRO_MAX_GROUP          15
#define SOC_SC_MTRO_COS                8
#define SOC_SC_MTRO_MAX_PORT_IN_GROUP  4

STATIC int mtro_port[SOC_SC_MTRO_MAX_GROUP][SOC_SC_MTRO_MAX_PORT_IN_GROUP] = {
    {1, 4, 7, 10}, {1, 4, 7, 10}, {2, 5, 8, 11}, {2, 5, 8, 11},
    {3, 6, 9, 12}, {3, 6, 9, 12}, {13, 17, 21, 25}, {13, 17, 21, 25},
    {14, 18, 22, 26}, {14, 18, 22, 26}, {14, 18, 22, 26}, {14, 18, 22, 26},
    {15, 19, 23, 27}, {15, 19, 23, 27}, {REG_PORT_ANY, 0, 0, 0}  
};

STATIC
int _soc_scorpion_mtro_mmu_port_index_get(int instance_num, int ptr,
                                          int *port, int *index)
{
   if ((instance_num >= SOC_SC_MTRO_MAX_GROUP) ||
       (ptr >= (SOC_SC_MTRO_MAX_PORT_IN_GROUP * SOC_SC_MTRO_COS)) ||
       (port == NULL) || (index == NULL)) {
       return  SOC_E_INTERNAL;
   }
   if (instance_num == (SOC_SC_MTRO_MAX_GROUP - 1)) {
       *port = REG_PORT_ANY;
       *index = ptr;
   } else {
       *port = mtro_port[instance_num][ptr / SOC_SC_MTRO_COS];
       *index = ptr % SOC_SC_MTRO_COS;
   }
   return SOC_E_NONE; 
}

STATIC int
_soc_scorpion_parity_mmu_clear(int unit, soc_field_t status_field)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN
        (READ_MEM_FAIL_INT_STATr(unit, &rval));
    if (soc_reg_field_valid(unit, MEM_FAIL_INT_STATr, status_field)) {
        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval, status_field, 0);
    } else {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_MEM_FAIL_INT_STATr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    return SOC_E_NONE;
}

STATIC
int soc_scorpion_mmu_mtro_process(int unit)
{
    static soc_reg_t mtro1_regs[] = {
        MINBUCKETCONFIGr,
        MINBUCKETCONFIG1r,
        MAXBUCKETCONFIGr,
        MAXBUCKETCONFIG1r 
    };
    static soc_reg_t mtro2_regs[] = {
        MINBUCKETr,
        MAXBUCKETr
    };
    
    static soc_reg_t cpu_mtro_regs[] = {
        CPUPKTMAXBUCKETCONFIGr,
        CPUPKTMAXBUCKETr
    };
    soc_reg_t  *mtro_regs;
    uint32 i, rval, status, val;
    _soc_ser_correct_info_t spci;
    int port, index, instance_num, ptr, mtro_num = 0;
    int rv = SOC_E_NONE;
    int block_info_idx = 0;
    uint32 minfo = 0;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
    spci.mem = INVALIDm;
    spci.blk_type = SOC_BLK_MMU;
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &status));
    if (!soc_reg_field_get(unit, MEM_FAIL_INT_STATr, status,
                           MTRO_PAR_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d MTRO: parity hardware"
                              " inconsistency\n"),
                              unit));
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, EGRSHAPEPARITYERRORPTRr,
                                      REG_PORT_ANY, 0, &rval));
    instance_num = soc_reg_field_get(unit, EGRSHAPEPARITYERRORPTRr,
                                     rval, INSTANCE_NUMf);
    ptr = soc_reg_field_get(unit, EGRSHAPEPARITYERRORPTRr, rval, PTRf);
    SOC_IF_ERROR_RETURN(
        _soc_scorpion_mtro_mmu_port_index_get(instance_num, ptr,
                                              &port, &index));
    /* Disable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));

    if (port == REG_PORT_ANY) {
        for (i = 0; i < COUNTOF(cpu_mtro_regs); i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = cpu_mtro_regs[i];
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_MMU) {
                    if (SOC_BLOCK_INFO(unit, block_info_idx).number == 0) {
                        break;
                    }
                }
                _soc_hbs_mem_parity_info(unit, block_info_idx, -1,
                                         MTRO_PAR_ERRf, &minfo);
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        spci.index, minfo);
            };
        }
    } else if (port >=1 && port <= 28) {
        if (instance_num % 2) {
            mtro_regs = mtro2_regs;
            mtro_num = 2;
        } else {
            mtro_regs = mtro1_regs;
            mtro_num = 4;
        }
        for (i = 0; i < mtro_num; i++) {
            spci.port = port;
            spci.index = index;
            spci.reg = mtro_regs[i];
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_MMU) {
                    if (SOC_BLOCK_INFO(unit, block_info_idx).number == 0) {
                        break;
                    }
                }
                _soc_hbs_mem_parity_info(unit, block_info_idx, -1,
                        MTRO_PAR_ERRf, &minfo);
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        spci.index, minfo);
            };
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d MTRO: parity hardware inconsistency\n"),
                              unit));
    }
    SOC_IF_ERROR_RETURN(_soc_scorpion_parity_mmu_clear(unit, MTRO_PAR_ERRf));
    /* Enable parity */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    soc_reg_field_set(unit, MISCCONFIGr, &val, PARITY_CHK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, val));
    return SOC_E_NONE;


}
#endif

STATIC void
_soc_hbs_mem_parity_info(int unit, int block_info_idx, int pipe,
                     soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}

int
_soc_hbs_parity_generic_clear(int unit, soc_block_t blocktype, int pipe,
                               soc_reg_t group_reg, soc_field_t group_field,
                               char *prefix_str, char *mem_str)
{
    uint32 rval;

    /* Clear parity error status */
    if (blocktype == SOC_BLK_MMU) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, group_reg, REG_PORT_ANY, 0,  &rval));

        soc_reg_field_set(unit, group_reg, &rval, group_field, 0);

        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, group_reg, REG_PORT_ANY, 0, rval));

        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

        LOG_INFO(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s asserted, status register %s, field %s\n"),
                  prefix_str, mem_str, SOC_REG_NAME(unit, group_reg),
                  SOC_FIELD_NAME(unit, group_field)));
        return SOC_E_NONE;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s asserted\n"),
                  prefix_str, mem_str));
        /* need to add new way to clear parity status */
        return SOC_E_NONE;
    }
}

STATIC int
_soc_hbs_mmu_party_error_callback_notify(int unit, soc_field_t field, int clear)
{
    char *mem_str;
    uint32 minfo;
    int  block_info_idx;
    int pipe = -1;
    char prefix_str[10];

    sal_sprintf(prefix_str, "unit %d", unit);

    mem_str = SOC_FIELD_NAME(unit, field);

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_MMU) {
        if (SOC_BLOCK_INFO(unit, block_info_idx).number == 0) {
            break;
        }
    }

    _soc_hbs_mem_parity_info(unit, block_info_idx, pipe,
                         field, &minfo);
    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY, 0,
                       minfo);
    if (clear) {
        SOC_IF_ERROR_RETURN(_soc_hbs_parity_generic_clear(unit, SOC_BLK_MMU, pipe,
                            MEM_FAIL_INT_STATr, field, prefix_str, mem_str));
    }
    return SOC_E_NONE;
}

/*
 * _soc_hbs_mmu_parity_error
 *      HBS MMU parity error decoder
 */
int _soc_hbs_mmu_parity_error(int unit)
{
    uint32              istat, val, val2;
    uint32 rval;
    soc_control_t       *soc = SOC_CONTROL(unit);
    _soc_ser_correct_info_t spci;
    int rv = SOC_E_NONE;

    sal_memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    
    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &istat));
    if (istat != 0) {
        soc->stat.intr_mmu++;
    }

    SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_CTRr(unit, &val));
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_hb_mmu_parity_error:unit = %d,"
                          "INTSTATUS  = 0x%08x Fail Count = %d\n"),
               unit, istat, val));

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr, istat, CFAP_MEM_FAILf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            CFAP_MEM_FAILf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CFAP_MEM_FAIL\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr, istat, CFAP_PAR_ERRf)) {
        soc->stat.err_cfap++;
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            CFAP_PAR_ERRf, 1));
        SOC_IF_ERROR_RETURN(READ_CFAPPARITYERRORPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CFAPPARITYERRORPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr, istat, CCP_PAR_ERRf)) {
        soc->stat.err_cpcrc++;
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            CCP_PAR_ERRf, 1));
        SOC_IF_ERROR_RETURN(READ_CCPPARITYERRORPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CCPPARITYERRORPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, AGING_CTR_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            AGING_CTR_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, AGING_CTR parity error\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, AGING_EXP_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            AGING_EXP_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, AGING_EXP parity error\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ_PKTHDR_CPU_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ_PKTHDR_CPU_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ_PKTHDR_CPU_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ_PKTHDR0_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ_PKTHDR0_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ_PKTHDR0_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ0_NOT_IP_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ0_NOT_IP_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ0_NOT_IP_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ1_NOT_IP_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ1_NOT_IP_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ1_NOT_IP_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ0_CELLCRC_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ0_CELLCRC_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ0_CELLCRC_ERR\n"), unit));
    }
    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, DEQ1_CELLCRC_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            DEQ1_CELLCRC_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, DEQ1_CELLCRC_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ0_CELLHDR_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_CELLHDR_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_CELLHDR_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ0_PKTHDR1_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_PKTHDR1_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_PKTHDR1_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ0_PKTLINK_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_PKTLINK_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_PKTLINK_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ0_CELLLINK_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_CELLLINK_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_CELLLINK_PAR_ERR\n"), unit));
    }

#ifdef BCM_SCORPION_SUPPORT 
    if (SOC_IS_SCORPION(unit)) {
        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ0_VLAN_TBL_PAR_ERRf) || 
            soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ0_IPMC_TBL_PAR_ERRf) ||  
            soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ1_VLAN_TBL_PAR_ERRf) ||
            soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ1_IPMC_TBL_PAR_ERRf)) {

            if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ0_VLAN_TBL_PAR_ERRf)) {
                SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(
                                    unit, TOQ0_VLAN_TBL_PAR_ERRf, 0));
            }
            if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ0_IPMC_TBL_PAR_ERRf)) {
                SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(
                                    unit, TOQ0_IPMC_TBL_PAR_ERRf, 0));
            }
            if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ1_VLAN_TBL_PAR_ERRf)) {
                SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(
                                    unit, TOQ1_VLAN_TBL_PAR_ERRf, 0));
            }
            if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, TOQ1_IPMC_TBL_PAR_ERRf)) {
                SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(
                                    unit, TOQ1_IPMC_TBL_PAR_ERRf, 0));
            }

            SOC_IF_ERROR_RETURN
                    (READ_TOQ_IPMCERRINTRr(unit, &val));                    
            SOC_IF_ERROR_RETURN
                    (READ_TOQ_IPMCVLANERRPTRr(unit, &val2));
                                                        
            spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
            spci.reg = INVALIDr;
            spci.mem = MMU_IPMC_VLAN_TBLm;
            spci.blk_type = -1;
            spci.index = val2;
            spci.sblk = 0;
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                        spci.mem,
                        spci.index);
            }
            SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
            soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
            soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
            SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));      
        }   
    } else 
#endif
    {
        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                      istat, TOQ0_VLAN_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_VLAN_TBL_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_VLAN_TBL_PAR_ERR\n"), unit));            
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                      istat, TOQ0_IPMC_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ0_IPMC_TBL_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ0_IPMC_TBL_PAR_ERR\n"), unit));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                      istat, TOQ1_VLAN_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_VLAN_TBL_PAR_ERRf, 1));

            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_VLAN_TBL_PAR_ERR\n"), unit));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                      istat, TOQ1_IPMC_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_IPMC_TBL_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_IPMC_TBL_PAR_ERR\n"), unit));
        }
    }
    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ1_CELLHDR_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_CELLHDR_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_CELLHDR_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ1_PKTHDR1_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_PKTHDR1_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_PKTHDR1_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ1_PKTLINK_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_PKTLINK_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_PKTLINK_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, TOQ1_CELLLINK_PAR_ERRf)) {
        SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                            TOQ1_CELLLINK_PAR_ERRf, 1));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, TOQ1_CELLLINK_PAR_ERR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                          istat, MTRO_PAR_ERRf)) {
#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SCORPION(unit)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                MTRO_PAR_ERRf, 0));
            SOC_IF_ERROR_RETURN(soc_scorpion_mmu_mtro_process(unit));
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                MTRO_PAR_ERRf, 0));
        } else 
#endif
       {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                MTRO_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit = %d, MTRO_PAR_ERR\n"), unit));
       }
    }

#define SOC_IPMC_GROUP_TABLE_NUM(unit) \
                (SOC_IS_SC_CQ(unit) ? 7 : 4)

    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        int i, ix, num_tables = SOC_IPMC_GROUP_TABLE_NUM(unit);

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, ENQ_IPMC_TBL_PAR_ERRf)) {
#ifdef BCM_HB_GW_SUPPORT
            if (SOC_REG_IS_VALID(unit, ENQ_IPMCGRP_TBL_PARITYERROR_STATUSr)) {
                SOC_IF_ERROR_RETURN
                    (READ_ENQ_IPMCGRP_TBL_PARITYERROR_STATUSr(unit, &val2));

                for (ix = 0; ix < num_tables; ix++) {
                    if (val2 & (1 << ix)) {
                        SOC_IF_ERROR_RETURN
                            (READ_ENQ_IPMCGRP_TBL_PARITYERRORPTRr(unit, ix, &val));
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit = %d, ENQ MMU_IPMC_GROUP_TBL%d, entry %d parity error\n"),
                                   unit, ix, val));
                    }
                }

                /* Clear errors */
                SOC_IF_ERROR_RETURN
                    (WRITE_ENQ_IPMCGRP_TBL_PARITYERROR_STATUSr(unit, 0));
            } else
#endif
            if(SOC_IS_SCORPION(unit)){

                soc_reg_t enq_ipmc_grp_err_ptr[] =
                        {TOQ_ENQIPMCGRPERRPTR0r, TOQ_ENQIPMCGRPERRPTR1r};
                soc_mem_t ipmc_grp_mem[] = {MMU_IPMC_GROUP_TBL0m, MMU_IPMC_GROUP_TBL1m,
                        MMU_IPMC_GROUP_TBL2m, MMU_IPMC_GROUP_TBL3m,
                        MMU_IPMC_GROUP_TBL4m, MMU_IPMC_GROUP_TBL5m,
                        MMU_IPMC_GROUP_TBL6m};

                SOC_IF_ERROR_RETURN
                    (READ_TOQ_IPMCERRINTRr(unit, &val));

                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.blk_type = -1;
                spci.sblk = 0;

                /* Check for each IPMC_GROUP table */
                for (ix = 0; ix < num_tables; ix++) {
                    for (i = 0; i < 2; i++) {
                        /* ENQ error starts from bit 9 */
                        if(val & (1 << (9 + ix))) {
                            SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, enq_ipmc_grp_err_ptr[i],
                                               REG_PORT_ANY, 0, &val2));
                            spci.index = val2;
                            spci.mem = ipmc_grp_mem[ix];

                            /* Perform SER correction */
                            rv = soc_ser_correction(unit, &spci);
                            if (SOC_FAILURE(rv)) {
                                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                        SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                        spci.mem,
                                        spci.index);
                            }
                        }
                    }
                }

                /* Clear errors */
                SOC_IF_ERROR_RETURN
                    (WRITE_TOQ_IPMCERRINTRr(unit, 0));

                SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
                soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
                SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
                soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
                SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
            }
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                ENQ_IPMC_TBL_PAR_ERRf, 0));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, MEM1_IPMC_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN
                (READ_MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr(unit, &val2));
        
            for (ix = 0; ix < num_tables; ix++) {
                if (val2 & (1 << ix)) {
                    SOC_IF_ERROR_RETURN
                        (READ_MEM1_IPMCGRP_TBL_PARITYERRORPTRr(unit, ix, &val));
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit = %d, ENQ MMU_IPMC_GROUP_TBL%d, entry %d parity error\n"),
                               unit, ix, val));
                }
            }

            SOC_IF_ERROR_RETURN
                (WRITE_MEM1_IPMCGRP_TBL_PARITYERROR_STATUSr(unit, 0));
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                MEM1_IPMC_TBL_PAR_ERRf, 1));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, MEM1_VLAN_TBL_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN
                (READ_MEM1_IPMCVLAN_TBL_PARITYERROR_STATUSr(unit, &val2));
        
            num_tables = 2;
            for (ix = 0; ix < num_tables; ix++) {
                if (val2 & (1 << ix)) {
                    SOC_IF_ERROR_RETURN
                        (READ_MEM1_IPMCVLAN_TBL_PARITYERRORPTRr(unit, ix, &val));

                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit = %d, ENQ MMU_IPMC_GROUP_TBL%d, entry %d parity error\n"),
                               unit, ix, val));
                    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = MMU_IPMC_VLAN_TBLm;
                    spci.blk_type = -1;
                    spci.index = val;
                    spci.sblk = 0;
                    rv = soc_ser_correction(unit, &spci);
                    if (SOC_FAILURE(rv)) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                spci.mem,
                                spci.index);
                    }
                    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
                    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
                    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
                    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));              
                }
            }
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                MEM1_VLAN_TBL_PAR_ERRf, 0));
        }
    }

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, WRED_PAR_ERRf)) {
            int bit, index;

            SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_INFOr(unit, &val));
            index = soc_reg_field_get(unit, WRED_PARITY_ERROR_INFOr,
                                      val, POINTERf);
            SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_BITMAPr(unit, &val2));
            for (bit = 0; bit < SOC_SC_WRED_PARITY_NUM; bit++) {
                if (val2 & (1 << bit)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit = %d, %s[%d]\n"), unit, SOC_REG_NAME(unit,
                               _soc_sc_parity_wred_reg[bit]), index));

                    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = _soc_sc_parity_wred_mem[bit];
                    spci.blk_type = -1;
                    spci.index = index;
                    spci.sblk = 0;

                    rv = soc_ser_correction(unit, &spci);
                    if (SOC_FAILURE(rv)) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                spci.mem,
                                spci.index);
                    }
                }
            }

            SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_BITMAPr(unit, 0));
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                WRED_PAR_ERRf, 0));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, DEQ0_LENGTH_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                DEQ0_LENGTH_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit = %d, DEQ0_LENGTH_PAR_ERR\n"), unit));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, DEQ1_LENGTH_PAR_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                DEQ1_LENGTH_PAR_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit = %d, DEQ1_LENGTH_PAR_ERR\n"), unit));
        }

        if (soc_reg_field_get(unit, MEM_FAIL_INT_STATr,
                              istat, START_BY_START_ERRf)) {
            SOC_IF_ERROR_RETURN(_soc_hbs_mmu_party_error_callback_notify(unit,
                                START_BY_START_ERRf, 1));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit = %d, START_BY_START_ERR\n"), unit));
        }
    }
#endif /* BCM_SCORPION_SUPPORT */

    SOC_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_STATr(unit, 0));

    return SOC_E_NONE;
}

/*
 * _soc_hb_ipipe_parity_error
 *      HB L2/L3 parity error decoder
 */
int _soc_hb_ipipe_parity_error(int unit)
{
    uint32              istat = 0;
    uint32              val = 0;

    if (soc_feature(unit, soc_feature_l2x_parity)) {
        SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     bucket, entry_bmap;
            bucket = soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                                       istat, BUCKET_IDXf),
            entry_bmap = soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                                           istat, ENTRY_BMf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "_soc_hx_ipipe_parity_error:unit = %d,"
                                  "L2_ENTRY_PARITY_STATUS  = 0x%08x, "
                                  "Bucket = %d Entry Bitmap 0x%02x\n"),
                       unit,
                       istat,
                       bucket,
                       entry_bmap));
            /* Clear all parity errors */ 
            SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, val));
        }
    }

    if (soc_feature(unit, soc_feature_l3x_parity)) {
        SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     bucket, entry_bmap;

            bucket = soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                                       istat, BUCKET_IDXf),
            entry_bmap = soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                                           istat, ENTRY_BMf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "_soc_hx_ipipe_parity_error:unit = %d,"
                                  "L3_ENTRY_PARITY_STATUS  = 0x%08x, "
                                  "Bucket = %d Entry Bitmap 0x%02x\n"),
                       unit,
                       istat,
                       bucket,
                       entry_bmap));
            SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, val));
        }
    }

    if (soc_feature(unit, soc_feature_l3defip_parity)) {
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L3_DEFIP_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     mem_idx;

            mem_idx = soc_reg_field_get(unit, L3_DEFIP_PARITY_STATUSr,
                                        istat, MEMORY_IDXf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "_soc_fb_ipipe_parity_error:unit = %d,"
                                  "L3_DEFIP_PARITY_STATUS  = 0x%08x, Index = %d "),
                       unit,
                       istat,
                       mem_idx));
            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, val));
        }
    }

    return SOC_E_NONE;
}

void soc_bradley_mmu_parity_error(void *unit, void *d1, void *d2,
                             void *d3, void *d4 )
{
    int u = PTR_TO_INT(unit);

    COMPILER_REFERENCE(d1);
    COMPILER_REFERENCE(d2);
    COMPILER_REFERENCE(d3);
    COMPILER_REFERENCE(d4);
    _soc_hb_ipipe_parity_error(u);
    _soc_hbs_mmu_parity_error(u);
    soc_intr_enable(u, IRQ_MEM_FAIL);
}

#ifdef BCM_SCORPION_SUPPORT

typedef enum {
    SOC_PARITY_TYPE_CELL_DATA,
    SOC_PARITY_TYPE_TABLE,
    SOC_PARITY_TYPE_DUALHASH,
    SOC_PARITY_TYPE_DUALHASH_2
} _soc_parity_error_t;

typedef struct _soc_parity_tree_s {
    soc_reg_t         block_reg;
    soc_field_t       error_field;
    _soc_parity_error_t       error_type;
    soc_mem_t         mem;
    soc_reg_t         status_reg;
    soc_reg_t         control_reg;
} _soc_parity_tree_t;

#define PAR_BLOCK_REG(i)        (_soc_sc_parity_trees[i].block_reg)
#define PAR_ERROR_FIELD(i)      (_soc_sc_parity_trees[i].error_field)
#define PAR_ERROR_TYPE(i)       (_soc_sc_parity_trees[i].error_type)
#define PAR_MEM(i)              (_soc_sc_parity_trees[i].mem)
#define PAR_STATUS_REG(i)       (_soc_sc_parity_trees[i].status_reg)
#define PAR_CONTROL_REG(i)      (_soc_sc_parity_trees[i].control_reg)

_soc_parity_tree_t _soc_sc_parity_trees[] = {
    { IP1_PARITY_INTR_STATUSr, SOURCE_TRUNK_MAP_INTRf,
      SOC_PARITY_TYPE_TABLE, SOURCE_TRUNK_MAP_TABLEm,
      SRC_TRUNK_PARITY_STATUSr, SRC_TRUNK_PARITY_CONTROLr },
    { IP1_PARITY_INTR_STATUSr, VLAN_MAC_OR_XLATE_INTRf,
      SOC_PARITY_TYPE_DUALHASH_2, VLAN_XLATEm,
      VLAN_MAC_OR_XLATE_PARITY_STATUSr, VLAN_MAC_OR_XLATE_PARITY_CONTROLr },
    { IP1_PARITY_INTR_STATUSr, VFP_POLICY_TABLE_INTRf,
      SOC_PARITY_TYPE_TABLE, VFP_POLICY_TABLEm,
      VFP_POLICY_TABLE_PARITY_STATUSr, VFP_POLICY_TABLE_PARITY_CONTROLr },
    { IP1_PARITY_INTR_STATUSr, VLAN_TABLE_INTRf,
      SOC_PARITY_TYPE_TABLE, VLAN_TABm,
      VLAN_PARITY_STATUSr, VLAN_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, L2_ENTRY_INTRf,
      SOC_PARITY_TYPE_DUALHASH, L2Xm,
      L2_ENTRY_PARITY_STATUSr, L2_ENTRY_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, L2MC_INTRf,
      SOC_PARITY_TYPE_TABLE, L2MCm,
      L2MC_PARITY_STATUSr, L2MC_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, L3_ENTRY_INTRf,
      SOC_PARITY_TYPE_DUALHASH, L3_ENTRY_ONLYm,
      L3_ENTRY_PARITY_STATUSr, L3_ENTRY_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, L3_DEFIP_DATA_INTRf,
      SOC_PARITY_TYPE_TABLE, L3_DEFIPm,
      L3_DEFIP_PARITY_STATUSr, L3_DEFIP_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, L3_IPMC_INTRf,
      SOC_PARITY_TYPE_TABLE, L3_IPMCm,
      L3MC_PARITY_STATUSr, L3MC_PARITY_CONTROLr },
    { IP2_PARITY_INTR_STATUSr, INITIAL_ING_L3_NEXT_HOP_INTRf,
      SOC_PARITY_TYPE_TABLE, INITIAL_ING_L3_NEXT_HOPm,
      INITIAL_ING_L3_NEXT_HOP_PARITY_STATUSr, INITIAL_ING_L3_NEXT_HOP_PARITY_CONTROLr },
    { IP3_PARITY_INTR_STATUSr, IFP_POLICY_TABLE_INTRf,
      SOC_PARITY_TYPE_TABLE, FP_POLICY_TABLEm,
      IFP_POLICY_TABLE_PARITY_STATUSr, IFP_POLICY_TABLE_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, IRSEL2_NEXTHOP_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, ING_L3_NEXT_HOPm,
      ING_L3_NEXT_HOP_PARITY_STATUSr, ING_L3_NEXT_HOP_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, ISW2_EGR_MASK_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, EGR_MASKm,
      EGR_MASK_PARITY_STATUSr, EGR_MASK_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, ISW2_SRC_MODID_BLOCK_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, SRC_MODID_BLOCKm,
      SRC_MODID_BLOCK_PARITY_STATUSr, SRC_MODID_BLOCK_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, ISW2_MODPORT_MAP_SW_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, MODPORT_MAP_SWm,
      MODPORT_MAP_SW_PARITY_STATUSr, MODPORT_MAP_SW_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, ISW2_MODPORT_MAP_IM_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, MODPORT_MAP_IMm,
      MODPORT_MAP_IM_PARITY_STATUSr, MODPORT_MAP_IM_PARITY_CONTROLr },
    { IP4_PARITY_STATUSr, ISW2_MODPORT_MAP_EM_PARITY_ERRf,
      SOC_PARITY_TYPE_TABLE, MODPORT_MAP_EMm,
      MODPORT_MAP_EM_PARITY_STATUSr, MODPORT_MAP_EM_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, L3_INTF_INTRf,
      SOC_PARITY_TYPE_TABLE, EGR_L3_INTFm,
      EGR_L3_INTF_PARITY_STATUSr, EGR_L3_INTF_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, L3_NEXT_HOP_INTRf,
      SOC_PARITY_TYPE_TABLE, EGR_L3_NEXT_HOPm,
      EGR_L3_NEXT_HOP_PARITY_STATUSr, EGR_L3_NEXT_HOP_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, IP_TUNNEL_INTRf,
      SOC_PARITY_TYPE_TABLE, EGR_IP_TUNNELm,
      EGR_IP_TUNNEL_PARITY_STATUSr, EGR_IP_TUNNEL_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, VLAN_XLATE_INTRf,
      SOC_PARITY_TYPE_TABLE, EGR_VLAN_XLATEm,
      EGR_VLAN_XLATE_PARITY_STATUSr, EGR_VLAN_XLATE_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, VLAN_INTRf,
      SOC_PARITY_TYPE_TABLE, EGR_VLANm,
      EGR_VLAN_PARITY_STATUSr, EGR_VLAN_PARITY_CONTROLr },
    { EP_PARITY_INTR_STATUSr, CELL_DATA_ERROR_INTRf,
      SOC_PARITY_TYPE_CELL_DATA, INVALIDm,
      EGR_ECC_STATUSr, EGR_ECC_CONTROLr },
    { INVALIDr, INVALIDf,
      SOC_PARITY_TYPE_TABLE, INVALIDm,
      INVALIDr, INVALIDr }
};

#define SOC_SC_PARITY_BUCKET_SIZE       8

STATIC int
_soc_sc_pipe_parity_enable(int unit, int enable)
{
    int trix, egress, bypass;
    soc_reg_t control_reg;
    uint32 control;

    control = enable ? 0x3 : 0;
    bypass = SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE;

    trix = 0;
    while ((control_reg = PAR_CONTROL_REG(trix)) != INVALIDr) {
        egress = (PAR_BLOCK_REG(trix) == EP_PARITY_INTR_STATUSr);
        if (egress && bypass) {
            if ((control_reg != EGR_VLAN_PARITY_CONTROLr) &&
                (control_reg != EGR_ECC_CONTROLr)) {
                /* Then L3 related, skip for bypass */
                trix++;
                continue;
            }
        }
        SOC_IF_ERROR_RETURN
            (soc_scorpion_pipe_select(unit, egress, SOC_PIPE_SELECT_X));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, control_reg, REG_PORT_ANY, 0, 
                            control));
        
        if (PAR_ERROR_TYPE(trix) != SOC_PARITY_TYPE_DUALHASH) {
            SOC_IF_ERROR_RETURN
                (soc_scorpion_pipe_select(unit, egress, SOC_PIPE_SELECT_Y));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, control_reg, REG_PORT_ANY, 
                                0, control));
        }
        trix++;
    }

    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, TRUE, SOC_PIPE_SELECT_X));
    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, FALSE, SOC_PIPE_SELECT_X));

    /* Turn on at the CMIC */
    /* coverity[dead_error_begin : FALSE] */
    SOC_IF_ERROR_RETURN
        (WRITE_CMIC_CHIP_PARITY_INTR_ENABLEr(unit, enable ? 0xfff : 0));
    return SOC_E_NONE;
}

/* This function takes the parity tree index and assumes the pipe select
 * is already set. */
STATIC int
_soc_sc_pipe_parity_clear(int unit, int pt_idx)
{
    soc_reg_t control_reg;
    uint32 control;

    control_reg = PAR_CONTROL_REG(pt_idx);

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, control_reg, REG_PORT_ANY, 0, 
                        &control));
    soc_reg_field_set(unit, control_reg, &control, PARITY_IRQ_ENf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, control_reg, REG_PORT_ANY, 0, 
                        control));
    soc_reg_field_set(unit, control_reg, &control, PARITY_IRQ_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, control_reg, REG_PORT_ANY, 0, 
                        control));

    return SOC_E_NONE;
}

/*
 * _soc_sc_stage_parity_error
 *      Scorpion ingress/egress parity error decoder
 */
STATIC int
_soc_sc_stage_parity_error(int unit, soc_reg_t base_reg, int pipe_sel)
{
    soc_reg_t cur_base_reg, last_base_reg = INVALIDr,
              status_reg;
    int tab_index, trix, bucket, bucket_offset;
    uint32 rval, status, bmp;
    _soc_ser_correct_info_t spci;
    int rv = SOC_E_NONE;

    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
    
    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit,
                      (base_reg == EP_PARITY_INTR_STATUSr), pipe_sel));

    trix = 0;
    rval = 0;
    while ((cur_base_reg = PAR_BLOCK_REG(trix)) != INVALIDr) {
        if (cur_base_reg == base_reg) {
            if (cur_base_reg != last_base_reg) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, cur_base_reg, REG_PORT_ANY, 0, 
                                    &rval));
                last_base_reg = cur_base_reg;
            }
        } else {
            trix++;
            continue;
        }

        if (soc_reg_field_get(unit, cur_base_reg, rval,
                              PAR_ERROR_FIELD(trix)) == 0) {
            trix++;
            continue;
        }

        status_reg = PAR_STATUS_REG(trix);
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, status_reg, REG_PORT_ANY, 0, 
                            &status));
        if (soc_reg_field_get(unit, status_reg, status, PARITY_ERRf) &&
            !((PAR_ERROR_TYPE(trix) == SOC_PARITY_TYPE_DUALHASH) && 
            (pipe_sel == SOC_PIPE_SELECT_Y))) {
            tab_index = -1;
            switch (PAR_ERROR_TYPE(trix)) {
            case SOC_PARITY_TYPE_CELL_DATA:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit = %d, %s pipe, Egress cell data ECC failure\n"),
                           unit,
                           (pipe_sel == SOC_PIPE_SELECT_Y) ? "Y" : "X"));
                break;
            case SOC_PARITY_TYPE_TABLE:
                tab_index = soc_reg_field_get(unit, status_reg,
                                              status, ENTRY_IDXf);
                if ((PAR_MEM(trix) == FP_POLICY_TABLEm) &&
                    (tab_index > 1024)) {
                    /* Adjust for slice size difference */
                    tab_index -= 1024;
                }
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit = %d, %s.%s, entry %d parity error\n"),
                           unit, SOC_MEM_NAME(unit, PAR_MEM(trix)),
                           (pipe_sel == SOC_PIPE_SELECT_Y) ? "Y" : "X",
                           tab_index));
                break;
            case SOC_PARITY_TYPE_DUALHASH:
                /* Only one pipe */
                bmp = soc_reg_field_get(unit, status_reg,
                                        status, ENTRY_BMf);
                bucket = soc_reg_field_get(unit, status_reg,
                                           status, BUCKET_IDXf);
                for (bucket_offset = 0;
                     bucket_offset < SOC_SC_PARITY_BUCKET_SIZE;
                     bucket_offset++) {
                    if (bucket_offset == (SOC_SC_PARITY_BUCKET_SIZE / 2)) {
                        
                    }
                    if (bmp & (1 << bucket_offset)) {
                        tab_index =
                            (bucket * SOC_SC_PARITY_BUCKET_SIZE) +
                            bucket_offset;
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit = %d, %s, entry %d parity error\n"),
                                   unit, SOC_MEM_NAME(unit, PAR_MEM(trix)),
                                   tab_index));
                    }
                }
                break;
            case SOC_PARITY_TYPE_DUALHASH_2:
                bmp = soc_reg_field_get(unit, status_reg,
                                        status, PARITY_ERR_BMf);
                bucket = soc_reg_field_get(unit, status_reg,
                                           status, BUCKET_IDX_0f);
                for (bucket_offset = 0;
                     bucket_offset < SOC_SC_PARITY_BUCKET_SIZE;
                     bucket_offset++) {
                    if (bucket_offset == (SOC_SC_PARITY_BUCKET_SIZE / 2)) {
                        bucket = soc_reg_field_get(unit, status_reg,
                                                   status, BUCKET_IDX_1f);
                    }
                    if (bmp & (1 << bucket_offset)) {
                        tab_index =
                            (bucket * SOC_SC_PARITY_BUCKET_SIZE) +
                            bucket_offset;
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit = %d, %s.%s, entry %d parity error\n"),
                                   unit, SOC_MEM_NAME(unit, PAR_MEM(trix)),
                                   (pipe_sel == SOC_PIPE_SELECT_Y) ? "Y" : "X",
                                   tab_index));
                    }
                }
                break;
            default:
                break;
            }
            if (PAR_MEM(trix) != INVALIDm) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                        SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                        PAR_MEM(trix),
                        tab_index);
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = PAR_MEM(trix);
                spci.blk_type = -1;
                spci.index = tab_index;
                rv = soc_ser_correction(unit, &spci);
                if (SOC_FAILURE(rv)) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                            SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                            PAR_MEM(trix),
                            tab_index);
                }
            }
            SOC_IF_ERROR_RETURN
                (_soc_sc_pipe_parity_clear(unit, trix));
        }
        trix++;
    }

    return SOC_E_NONE;
}

typedef struct _soc_sc_parity_stage_s {
    uint32            cpi_bit;
    soc_reg_t         block_reg;
    int               pipe_sel;
} _soc_sc_parity_stage_t;

_soc_sc_parity_stage_t _soc_sc_parity_stages[] = {
    {0x001, IP1_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_X},
    {0x002, IP1_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_Y},
    {0x004, IP2_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_X},
    {0x008, IP2_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_Y},
    {0x010, IP3_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_X},
    {0x020, IP3_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_Y},
    {0x040, IP4_PARITY_STATUSr, SOC_PIPE_SELECT_X},
    {0x080, IP4_PARITY_STATUSr, SOC_PIPE_SELECT_Y},
    {0x100, EP_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_X},
    {0x200, EP_PARITY_INTR_STATUSr, SOC_PIPE_SELECT_Y},
    {0xffffffff, INVALIDr, SOC_PIPE_SELECT_X}
};

#define PAR_STAGE_BIT(i)          (_soc_sc_parity_stages[i].cpi_bit)
#define PAR_STAGE_BLOCK_REG(i)    (_soc_sc_parity_stages[i].block_reg)
#define PAR_STAGE_PIPE_SELECT(i)  (_soc_sc_parity_stages[i].pipe_sel)

/*
 * _soc_sc_pipe_parity_error
 *      Scorpion ingress/egress parity error decoder
 */
STATIC int
_soc_sc_pipe_parity_error(int unit)
{
    uint32 cpi_reg;
    int ix;
    /* coverity[dead_error_begin : FALSE] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit, &cpi_reg));

    ix = 0;
    while (PAR_STAGE_BIT(ix) != 0xffffffff) {
        if (cpi_reg & PAR_STAGE_BIT(ix)) {
            SOC_IF_ERROR_RETURN
                (_soc_sc_stage_parity_error(unit, PAR_STAGE_BLOCK_REG(ix),
                                            PAR_STAGE_PIPE_SELECT(ix)));
        }
        ix++;
    }

    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, FALSE, SOC_PIPE_SELECT_X));
    SOC_IF_ERROR_RETURN
        (soc_scorpion_pipe_select(unit, TRUE, SOC_PIPE_SELECT_X));

    if (cpi_reg & 0x400) {
        
    }

    if (cpi_reg & 0x800) {
        _soc_hbs_mmu_parity_error(unit);
    }

    return SOC_E_NONE;
}

void soc_scorpion_parity_error(void *unit, void *d1, void *d2,
                               void *d3, void *d4 )
{
    int u = PTR_TO_INT(unit);

    COMPILER_REFERENCE(d1);
    COMPILER_REFERENCE(d2);
    COMPILER_REFERENCE(d3);
    COMPILER_REFERENCE(d4);
    _soc_sc_pipe_parity_error(u);
    soc_intr_enable(u, IRQ_MEM_FAIL);
}
#endif /* BCM_SCORPION_SUPPORT */

int
soc_bradley_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_bradley_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_bradley_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

int
soc_hbx_higig2_mcast_sizes_set(int unit, int bcast_size,
                               int mcast_size, int ipmc_size)
{
    uint32 mc_ctrl;
    soc_control_t       *soc = SOC_CONTROL(unit);

    if ((bcast_size > SOC_HBX_HIGIG2_MULTICAST_RANGE_MAX) ||
        (mcast_size > SOC_HBX_HIGIG2_MULTICAST_RANGE_MAX) ||
        (ipmc_size > SOC_HBX_HIGIG2_MULTICAST_RANGE_MAX) ||
        ((bcast_size + mcast_size + ipmc_size) >
         SOC_HBX_HIGIG2_MULTICAST_RANGE_MAX)) {
        return SOC_E_PARAM;
    }

    mc_ctrl = 0;
    soc_reg_field_set(unit, MC_CONTROL_1r, &mc_ctrl, 
                      HIGIG2_BC_BASE_OFFSETf, 0);
    soc_reg_field_set(unit, MC_CONTROL_1r, &mc_ctrl,
                      HIGIG2_BC_SIZEf, bcast_size);
    SOC_IF_ERROR_RETURN
        (WRITE_MC_CONTROL_1r(unit, mc_ctrl));

    mc_ctrl = 0;
    soc_reg_field_set(unit, MC_CONTROL_2r, &mc_ctrl, 
                      HIGIG2_L2MC_BASE_OFFSETf, bcast_size);
    soc_reg_field_set(unit, MC_CONTROL_2r, &mc_ctrl,
                      HIGIG2_L2MC_SIZEf, mcast_size);
    SOC_IF_ERROR_RETURN 
        (WRITE_MC_CONTROL_2r(unit, mc_ctrl));
    
    mc_ctrl = 0;
    soc_reg_field_set(unit, MC_CONTROL_3r, &mc_ctrl, 
                      HIGIG2_IPMC_BASE_OFFSETf, mcast_size + bcast_size);
    soc_reg_field_set(unit, MC_CONTROL_3r, &mc_ctrl,
                      HIGIG2_IPMC_SIZEf, ipmc_size);
    SOC_IF_ERROR_RETURN
        (WRITE_MC_CONTROL_3r(unit, mc_ctrl));

    mc_ctrl = 0;
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_reg_field_set(unit, MC_CONTROL_4r, &mc_ctrl,
                ALLOW_IPMC_INDEX_WRAP_AROUNDf, 1);
    }
#endif
    SOC_IF_ERROR_RETURN
        (WRITE_MC_CONTROL_4r(unit, mc_ctrl));

    mc_ctrl = 0;
    soc_reg_field_set(unit, EGR_MC_CONTROL_1r, &mc_ctrl, 
                      HIGIG2_BC_BASE_OFFSETf, 0);
    soc_reg_field_set(unit, EGR_MC_CONTROL_1r, &mc_ctrl, 
                      HIGIG2_L2MC_BASE_OFFSETf, bcast_size);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_MC_CONTROL_1r(unit, mc_ctrl));

    mc_ctrl = 0;
    soc_reg_field_set(unit, EGR_MC_CONTROL_2r, &mc_ctrl, 
                      HIGIG2_IPMC_BASE_OFFSETf, mcast_size + bcast_size);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_MC_CONTROL_2r(unit, mc_ctrl));

    soc->higig2_bcast_size = bcast_size;
    soc->higig2_mcast_size = mcast_size;
    soc->higig2_ipmc_size = ipmc_size;

    return SOC_E_NONE;
}

int
soc_hbx_higig2_mcast_sizes_get(int unit, int *bcast_size,
                                   int *mcast_size, int *ipmc_size)
{
    soc_control_t       *soc = SOC_CONTROL(unit);

    *bcast_size = soc->higig2_bcast_size;
    *mcast_size = soc->higig2_mcast_size;
    *ipmc_size = soc->higig2_ipmc_size;
    return SOC_E_NONE;
}


int
soc_hbx_mcast_size_set(int unit, int mc_size)
{
    uint32	mc_ctrl;
    soc_control_t       *soc = SOC_CONTROL(unit);
    int l2mc_table_size = soc_mem_index_count(unit, L2MCm);

    if (((mc_size + soc->ipmc_size) > l2mc_table_size) ||
        (mc_size == l2mc_table_size)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_5r(unit, &mc_ctrl));
    soc_reg_field_set(unit, MC_CONTROL_5r, &mc_ctrl,
                      SHARED_TABLE_L2MC_SIZEf, mc_size);
    SOC_IF_ERROR_RETURN(WRITE_MC_CONTROL_5r(unit, mc_ctrl));
    soc->mcast_size = mc_size;
    return SOC_E_NONE;
}

int
soc_hbx_mcast_size_get(int unit, int *mc_base, int *mc_size)
{
    *mc_base = 0;
    *mc_size = SOC_CONTROL(unit)->mcast_size;
    return SOC_E_NONE;
}

int
soc_hbx_ipmc_size_set(int unit, int mc_size)
{
    uint32	mc_ctrl;
    soc_control_t       *soc = SOC_CONTROL(unit);
    int l2mc_table_size = soc_mem_index_count(unit, L2MCm);

    if (((mc_size + soc->mcast_size) > l2mc_table_size) ||
        (mc_size == l2mc_table_size)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_5r(unit, &mc_ctrl));
    soc_reg_field_set(unit, MC_CONTROL_5r, &mc_ctrl,
                      SHARED_TABLE_IPMC_SIZEf, mc_size);
    SOC_IF_ERROR_RETURN(WRITE_MC_CONTROL_5r(unit, mc_ctrl));
    soc->ipmc_size = mc_size;
    return SOC_E_NONE;
}

int
soc_hbx_ipmc_size_get(int unit, int *mc_base, int *mc_size)
{
    soc_control_t       *soc = SOC_CONTROL(unit);

    *mc_base = soc->mcast_size;
    *mc_size = soc->ipmc_size;
    return SOC_E_NONE;
}

#ifdef BCM_SCORPION_SUPPORT
void
soc_scorpion_mem_config(int unit)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;

    sop = SOC_PERSIST(unit);
    memState = sop->memState;

    if (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE) {
        if (SOC_SWITCH_BYPASS_MODE(unit) ==
            SOC_SWITCH_BYPASS_MODE_L3_AND_FP) {
            memState[IFP_REDIRECTION_PROFILEm].index_max = -1;
            memState[FP_COUNTER_TABLE_Xm].index_max = -1;
            memState[FP_COUNTER_TABLE_Ym].index_max = -1;
            memState[FP_METER_TABLEm].index_max = -1;
            memState[FP_POLICY_TABLEm].index_max = -1;
            memState[FP_PORT_FIELD_SELm].index_max = -1;
            memState[FP_RANGE_CHECKm].index_max = -1;
            memState[FP_SLICE_KEY_CONTROLm].index_max = -1;
            memState[FP_SLICE_MAPm].index_max = -1;
            memState[FP_TCAMm].index_max = -1;
            memState[FP_TCAM_Xm].index_max = -1;
            memState[FP_TCAM_Ym].index_max = -1;
        }
        memState[FP_STORM_CONTROL_METERSm].index_max = -1;
        memState[VFP_POLICY_TABLEm].index_max = -1;
        memState[VFP_TCAMm].index_max = -1;
        memState[EFP_COUNTER_TABLEm].index_max = -1;
        memState[EFP_COUNTER_TABLE_Xm].index_max = -1;
        memState[EFP_COUNTER_TABLE_Ym].index_max = -1;
        memState[EFP_METER_TABLEm].index_max = -1;
        memState[EFP_METER_TABLE_Xm].index_max = -1;
        memState[EFP_METER_TABLE_Ym].index_max = -1;
        memState[EFP_POLICY_TABLEm].index_max = -1;
        memState[EFP_TCAMm].index_max = -1;
        memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
        memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
        memState[L3_ENTRY_IPV4_MULTICAST_SCRATCHm].index_max = -1;
        memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
        memState[L3_ENTRY_IPV4_UNICAST_SCRATCHm].index_max = -1;
        memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
        memState[L3_ENTRY_IPV6_MULTICAST_SCRATCHm].index_max = -1;
        memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
        memState[L3_ENTRY_IPV6_UNICAST_SCRATCHm].index_max = -1;
        memState[L3_ENTRY_ONLYm].index_max = -1;
        memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
        memState[L3_IPMCm].index_max = -1;
        memState[L3_TUNNELm].index_max = -1;
        memState[L3_DEFIPm].index_max = -1;
        memState[L3_DEFIP_128m].index_max = -1;
        memState[L3_DEFIP_128_DATA_ONLYm].index_max = -1;
        memState[L3_DEFIP_128_HIT_ONLYm].index_max = -1;
        memState[L3_DEFIP_128_ONLYm].index_max = -1;
        memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
        memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
        memState[L3_DEFIP_ONLYm].index_max = -1;
        memState[L3_ECMPm].index_max = -1;
        memState[L3_ECMP_COUNTm].index_max = -1;
        memState[EGR_L3_NEXT_HOPm].index_max = -1;
        memState[EGR_VLAN_XLATEm].index_max = -1;
        memState[EGR_VLAN_XLATE_SCRATCHm].index_max = -1;
        memState[EGR_L3_INTFm].index_max = -1;
        memState[EGR_PRI_CNG_MAPm].index_max = -1;
        memState[EGR_IP_TUNNELm].index_max = -1;
        memState[EGR_IP_TUNNEL_IPV6m].index_max = -1;
        memState[EGR_DSCP_TABLEm].index_max = -1;
        memState[EGR_DSCP_ECN_MAPm].index_max = -1;
        memState[DSCP_TABLEm].index_max = -1;
        memState[IPV4_IN_IPV6_PREFIX_MATCH_TABLEm].index_max = -1;
        memState[VLAN_SUBNETm].index_max = -1;
        memState[VLAN_SUBNET_DATA_ONLYm].index_max = -1;
        memState[VLAN_SUBNET_ONLYm].index_max = -1;
        memState[VLAN_PROTOCOLm].index_max = -1;
        memState[VLAN_PROTOCOL_DATAm].index_max = -1;
        memState[VLAN_XLATEm].index_max = -1;
        memState[VLAN_XLATE_SCRATCHm].index_max = -1;
        memState[VLAN_MACm].index_max = -1;
        memState[VLAN_MAC_SCRATCHm].index_max = -1;
        memState[INITIAL_L3_ECMPm].index_max = -1;
        memState[INITIAL_L3_ECMP_COUNTm].index_max = -1;
        memState[INITIAL_ING_L3_NEXT_HOPm].index_max = -1;
        memState[ING_VLAN_TAG_ACTION_PROFILEm].index_max = -1;
        memState[ING_PRI_CNG_MAPm].index_max = -1;
        memState[ING_VLAN_RANGEm].index_max = -1;
        memState[ING_L3_NEXT_HOPm].index_max = -1;
        memState[PORT_CBL_TABLEm].index_max = -1;
        memState[TRUNK_CBL_TABLEm].index_max = -1;
        memState[IPV6_PROXY_ENABLE_TABLEm].index_max = -1;
        memState[EGR_ERSPANm].index_max = -1;
        memState[EGR_IM_MTP_INDEXm].index_max = -1;
        memState[EGR_EM_MTP_INDEXm].index_max = -1;
        memState[EGR_VLAN_TAG_ACTION_PROFILEm].index_max = -1;
    }
}

#define L2_OVERFLOW_TABLE_DEPTH 2
STATIC int
_soc_scorpion_l2_overflow_entry_get(int unit, l2x_entry_t *l2x_entry, int *count)
{
    int i=0, rv=0;
    uint32 entry_valid=0;

    /* Read all entries in L2_ENTRY_OVERFLOWm */
    (*count) = 0;
    for (i = 0; i < L2_OVERFLOW_TABLE_DEPTH; i++) {
        rv = READ_L2_ENTRY_OVERFLOWm(unit, COPYNO_ALL, i, &(l2x_entry[i]));
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_L2, (BSL_META_U(unit,
                      "Error reading L2_ENTRY_OVERFLOW, index %d, %s\n"),
                      i, soc_errmsg(rv)));
            return (rv);
        }
        soc_L2Xm_field_get(unit, &(l2x_entry[i]), VALIDf, &entry_valid);
        if (entry_valid) {
            (*count)++;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_scorpion_l2_overflow_disable(int unit)
{
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_LEARN_CONTROLr, REG_PORT_ANY,
                                OVERFLOW_BUCKET_ENABLEf, 0));
    /* Mark as inactive */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = FALSE;
    SOC_CONTROL_UNLOCK(unit);
    soc_intr_disable(unit, IRQ_CHIP_FUNC_0);
    return SOC_E_NONE;
}

STATIC int
_soc_scorpion_l2_overflow_enable(int unit)
{
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_LEARN_CONTROLr, REG_PORT_ANY,
                                OVERFLOW_BUCKET_ENABLEf, 1));
    /* Mark as active */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = TRUE;
    SOC_CONTROL_UNLOCK(unit);
    /* Enable interrupt */
    soc_intr_enable(unit, IRQ_CHIP_FUNC_0);
    return SOC_E_NONE;
}

STATIC int
_soc_scorpion_l2_overflow_fill(int unit, uint8 zeros_or_ones)
{
    l2x_entry_t entry;
    int i;

    if (zeros_or_ones) {
        sal_memset(&entry, 0xffffffff, sizeof(entry));
        for (i = 0; i < L2_OVERFLOW_TABLE_DEPTH; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_L2_ENTRY_OVERFLOWm(unit, COPYNO_ALL, i, &entry));
        }
    } else {
            SOC_IF_ERROR_RETURN
             (soc_mem_clear(unit, L2_ENTRY_OVERFLOWm, COPYNO_ALL, FALSE));
    }
    return SOC_E_NONE;
}

int
soc_scorpion_l2_overflow_start(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    if (SOC_CONTROL(unit)->l2_overflow_active) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(_soc_scorpion_l2_overflow_fill(unit, 0));
    SOC_IF_ERROR_RETURN(_soc_scorpion_l2_overflow_enable(unit));
    return SOC_E_NONE;
}

int
soc_scorpion_l2_overflow_stop(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(_soc_scorpion_l2_overflow_fill(unit, 1));
    SOC_IF_ERROR_RETURN(_soc_scorpion_l2_overflow_disable(unit));
    return SOC_E_NONE;
}

void
soc_scorpion_l2_overflow_interrupt_handler(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4)
{
    l2x_entry_t entry[L2_OVERFLOW_TABLE_DEPTH];
    int count = 0, i;
    int unit = PTR_TO_INT(unit_vp);

    if (!SOC_CONTROL(unit)->l2_overflow_active) {
        LOG_ERROR(BSL_LS_SOC_L2, (BSL_META_U(unit, \
                  "Received L2 overflow event with no app handler\n")));
    }

    if (_soc_scorpion_l2_overflow_disable(unit)) {
        return;
    }

    /* Read Oerflow entries */
    sal_memset(entry, 0, sizeof(entry));
    if (_soc_scorpion_l2_overflow_entry_get(unit, entry, &count)) {
        return;
    }
    if (count > L2_OVERFLOW_TABLE_DEPTH) {
        LOG_ERROR(BSL_LS_SOC_L2, (BSL_META_U(unit, \
                  "Overflow entry count more than overflow table depth\n")));
        return;
    }

    /* Callback L2 handler with Overflow entries */
    for (i = 0; i < count; i++) {
        soc_l2x_callback(unit, SOC_L2X_ENTRY_OVERFLOW, NULL, &entry[i]);
    }

    /* Restart Overflow processing */
    soc_scorpion_l2_overflow_start(unit);
}
#endif /* BCM_SCORPION_SUPPORT */

#endif /* BCM_BRADLEY_SUPPORT */
