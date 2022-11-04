/* *INDENT-OFF*  */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Time - Broadcom Time BroadSync API (Petra).
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TIME

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H 
#undef _SOC_MEMREGS_H
#endif

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/error.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/types.h>
#include <soc/property.h>
#include <soc/cmic.h>
#include <soc/cm.h>
#include <soc/util.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/allenum.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/feature.h>

#include <bcm_int/dnx/auto_generated/dnx_time_dispatch.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

#include <bcm/gdpll.h>
#include <bcm_int/gdpll.h>
#include <shared/pack.h>

/* allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc property functions */

#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif

#include <sal/core/boot.h>
#include <shared/shr_thread.h>
#include <soc/sand/sand_aux_access.h>

#include <bcm/time.h>
#include <bcm_int/common/time.h>
#include <bcm_int/common/time-mbox.h>
#include <bcm_int/common/mbox.h>

#ifdef EDKHLIB
#include <bcm_int/common/edk_int.h>
/*! Broadsync EDK host version */
#define BS_EDK_HOST_VERSION         0x1

/*! Max timeout for rpc sync/async between EDK host and FW */
#define MCS_BS_MAX_EDK_MSG_TIMEOUT (5000) /* 5 secs */

handle_t edk_bs_handle = NULL;
#define EDK_NOTIFY_EVENT_BS  100
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dnx/swstate/auto_generated/access/dnx_time_sw_state_access.h>
#endif

#include <soc/dnx/dnx_data/auto_generated/dnx_data_time.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>

#if defined(BCM_DNX_SUPPORT)

#define BROAD_SYNC_TIME_CAPTURE_TIMEOUT      (10) /* useconds */
#define BROAD_SYNC_OUTPUT_TOGGLE_TIME_DELAY  (3)  /* seconds */

#define SYNT_TIME_SECONDS(unit, id) \
        TIME_INTERFACE_CONFIG(unit, id).time_capture.syntonous.seconds
#define SYNT_TIME_NANOSECONDS(unit, id) \
        TIME_INTERFACE_CONFIG(unit, id).time_capture.syntonous.nanoseconds

/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/
static _bcm_time_config_p _bcm_time_config[BCM_MAX_NUM_UNITS] = {NULL};

static bcm_time_spec_t _bcm_time_accuracy_arr[TIME_ACCURACY_CLK_MAX] = {
      {0, COMPILER_64_INIT(0,0),  25},        /* HW value = 32, accuracy up tp 25 nanosec */
      {0, COMPILER_64_INIT(0,0),  100},       /* HW value = 33, accuracy up to 100 nanosec */
      {0, COMPILER_64_INIT(0,0),  250},       /* HW value = 34, accuracy up to 250 nanosec */
      {0, COMPILER_64_INIT(0,0),  1000},      /* HW value = 35, accuracy up to 1 microsec */
      {0, COMPILER_64_INIT(0,0),  2500},      /* HW value = 36, accuracy up to 2.5 microsec */
      {0, COMPILER_64_INIT(0,0),  10000},     /* HW value = 37, accuracy up to 10 microsec */
      {0, COMPILER_64_INIT(0,0),  25000},     /* HW value = 38, accuracy up to 25 microsec */
      {0, COMPILER_64_INIT(0,0),  100000},    /* HW value = 39, accuracy up to 100 microsec */
      {0, COMPILER_64_INIT(0,0),  250000},    /* HW value = 40, accuracy up to 250 microsec */
      {0, COMPILER_64_INIT(0,0),  1000000},   /* HW value = 41, accuracy up to 1 milisec */
      {0, COMPILER_64_INIT(0,0),  2500000},   /* HW value = 42, accuracy up to 2.5 milisec */
      {0, COMPILER_64_INIT(0,0),  10000000},  /* HW value = 43, accuracy up to 10 milisec */
      {0, COMPILER_64_INIT(0,0),  25000000},  /* HW value = 44, accuracy up to 25 milisec */
      {0, COMPILER_64_INIT(0,0),  100000000}, /* HW value = 45, accuracy up to 100 milisec */
      {0, COMPILER_64_INIT(0,0),  250000000}, /* HW value = 46, accuracy up to 250 milisec */
      {0, COMPILER_64_INIT(0,1),  0},         /* HW value = 47, accuracy up to 1 sec */
      {0, COMPILER_64_INIT(0,10), 0},         /* HW value = 48, accuracy up to 10 sec */
      /* HW value = 49, accuracy greater than 10 sec */
      {0, COMPILER_64_INIT(0,TIME_ACCURACY_INFINITE), TIME_ACCURACY_INFINITE},
      /* HW value = 254 accuracy unknown */
      {0, COMPILER_64_INIT(0,TIME_ACCURACY_UNKNOWN), TIME_ACCURACY_UNKNOWN}
};


/****************************************************************************/
/*                      Internal functions implmentation                    */
/****************************************************************************/
static int
_bcm_dnx_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time, uint32 flags);
static int
_bcm_dnx_time_interface_offset_get(int unit, bcm_time_if_t id, bcm_time_spec_t *offset);
static int
_bcm_dnx_time_bs_init(int unit, bcm_time_interface_t *intf);
static int 
_bcm_dnx_time_interface_ref_clock_install(int unit, bcm_time_if_t id);
static int
_bcm_dnx_time_interface_install(int unit, bcm_time_if_t intf_id);
static int 
_bcm_dnx_time_interface_free(int unit, bcm_time_if_t intf_id);
static int
_bcm_dnx_time_interface_get(int unit, bcm_time_if_t id, bcm_time_interface_t *intf);

#define IS_BROADSYNC0(x) (x == 0)
#define IS_BROADSYNC1(x) (x == 1)
#ifdef INCLUDE_GDPLL
int _bcm_bs_dpll_chan_create(int unit, bcm_time_interface_t *intf);
int _bcm_bs_dpll_chan_destroy(int unit, int bs_id);
int _bcm_bs_dpll_chan_recreate(int unit, int bs_id);
#endif

#if defined(BCM_J2X_SUPPORT)
bcm_time_spec_t gdpll_chan_NTPoffset = {0, 0, 0};
#endif

#ifdef EDKHLIB
int _edk_init(int unit, int edk_bs_core_num)
{
    int ret = EDK_ERR_NONE;
    uint32_t edk_fw_ver;
    uint32_t edk_host_ver = BS_EDK_HOST_VERSION;
    int32_t reply_len;

    if (edk_bs_handle != NULL) {
        LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit, "_edk_init already done\n")));
        goto exit;
    }

    edk_bs_handle = edk_procmgr_open(unit, edk_bs_core_num);
    if (edk_bs_handle == NULL) {
        LOG_ERROR(BSL_LS_BCM_TIME,
          (BSL_META_U(unit, "procmgr_open Failed uc:%d\n"), edk_bs_core_num));
        ret = -1;
        goto exit;
    }

    ret = edk_rpc_call_sync(edk_bs_handle, "uc_bs_version_exchange",
            &edk_host_ver, sizeof(uint32_t),
            &edk_fw_ver, sizeof(uint32_t),
            &reply_len, MCS_BS_MAX_EDK_MSG_TIMEOUT);
    if ((ret == EDK_ERR_NONE) &&
        (reply_len == sizeof(uint32_t)) &&
        (edk_fw_ver == BS_EDK_HOST_VERSION)) {
        ret = edk_rpc_call_sync(edk_bs_handle, "uc_bs_init", NULL, 0,
            NULL, 0, &reply_len,
            MCS_BS_MAX_EDK_MSG_TIMEOUT);
        if (ret != EDK_ERR_NONE) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(unit, " uc_bs_init Failed for uc:%d\n"), edk_bs_core_num));
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
           (BSL_META_U(unit, " uc_bs_version_exchange Failed for uc:%d\n"), edk_bs_core_num));
        ret = -1;
    }

exit:
    return ret;
}

int _edk_deinit(int unit, int edk_bs_core_num)
{
    int ret = EDK_ERR_NONE;
    int32_t reply_len;

    if (edk_bs_handle != NULL) {
        ret = edk_rpc_call_sync(edk_bs_handle, "uc_bs_deinit", NULL, 0,
            NULL, 0, &reply_len,
            MCS_BS_MAX_EDK_MSG_TIMEOUT);
        if (ret != EDK_ERR_NONE) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(unit, " uc_bs_deinit Failed for uc:%d\n"), edk_bs_core_num));
        }
        edk_procmgr_close(edk_bs_handle);
        edk_bs_handle = NULL;
    }

    return ret;
}
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_bcm_dnx_time_swstate_store(int unit, int intf_id, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;

    dnx_time_interface_db.flags.set(unit, intf->flags);
    dnx_time_interface_db.id.set(unit, intf->id);
    dnx_time_interface_db.heartbeat_hz.set(unit, intf->heartbeat_hz);
    dnx_time_interface_db.clk_resolution.set(unit, intf->clk_resolution);
    dnx_time_interface_db.bitclock_hz.set(unit, intf->bitclock_hz);
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
#ifdef EDKHLIB
        /* Need to fetch the status from M7? */
#else
        _bcm_time_bs_status_get(unit, (int *)(&(intf->status)));
#endif
    }
    dnx_time_interface_db.status.set(unit, intf->status);

    return rv;
}

STATIC int
_bcm_dnx_time_swstate_restore(int unit, int intf_id, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;
    int tmp = 0;

    dnx_time_interface_db.flags.get(unit, &intf->flags);
    dnx_time_interface_db.id.get(unit, &intf->id);
    dnx_time_interface_db.heartbeat_hz.get(unit, &intf->heartbeat_hz);
    dnx_time_interface_db.clk_resolution.get(unit, &intf->clk_resolution);
    dnx_time_interface_db.bitclock_hz.get(unit, &intf->bitclock_hz);
    dnx_time_interface_db.status.get(unit, &tmp);
    intf->status = (bcm_bs_time_source_t)tmp;

    return rv;
}

#endif

/*
 * Function:
 *    _bcm_dnx_time_hw_clear
 * Purpose:
 *    Internal routine used to clear all HW registers and table to default values
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_hw_clear(int unit, bcm_time_if_t intf_id)
{
    uint32 regval;

    /* accessing CMIC_BS/CMIC_TS when pll is off is not allowed, so encaplusating this whit a soc property,
       otherwize it bcm.user crash on init (crash happen after rebotting Linux) */
    if (1 == dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock)) {

        if (soc_is(unit, J2X_DEVICE)) {
            if (intf_id == 0) {
                READ_NSYNC_BS_0_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, TOD_CTRLf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_1_PPS_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_TC_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_HB_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CONFIGr, &regval, BS_1_PPS_OUTPUT_LOWf, 0);
                WRITE_NSYNC_BS_0_BS_CONFIGr(unit, regval);

                READ_NSYNC_BS_0_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_NSYNC_BS_0_BS_OUTPUT_TIME_0r(unit, regval);

                /*
                 * Reset Clock Control
                 */
                READ_NSYNC_BS_0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                WRITE_NSYNC_BS_0_BS_CLK_CTRLr(unit, regval);

                /*
                 * Reset HeartBeat
                 */
                READ_NSYNC_BS_0_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                WRITE_NSYNC_BS_0_BS_HEARTBEAT_CTRLr(unit, regval);

                READ_NSYNC_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_MISC_CLK_EVENT_CTRLr, &regval, BS_0_TX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_MISC_CLK_EVENT_CTRLr, &regval, BS_0_RX_HB_STATUS_ENABLEf, 0);
                WRITE_NSYNC_MISC_CLK_EVENT_CTRLr(unit, regval);

            } else {
                READ_NSYNC_BS_1_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, TOD_CTRLf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_1_PPS_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_TC_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_HB_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_LOWf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CONFIGr, &regval, BS_1_PPS_OUTPUT_LOWf, 0);
                WRITE_NSYNC_BS_1_BS_CONFIGr(unit, regval);

                READ_NSYNC_BS_1_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_NSYNC_BS_1_BS_OUTPUT_TIME_0r(unit, regval);

                /*
                 * Reset Clock Control
                 */
                READ_NSYNC_BS_1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                WRITE_NSYNC_BS_1_BS_CLK_CTRLr(unit, regval);

                /*
                 * Reset HeartBeat
                 */
                READ_NSYNC_BS_1_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_BS_1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                WRITE_NSYNC_BS_1_BS_HEARTBEAT_CTRLr(unit, regval);

                READ_NSYNC_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NSYNC_MISC_CLK_EVENT_CTRLr, &regval, BS_1_TX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, NSYNC_MISC_CLK_EVENT_CTRLr, &regval, BS_1_RX_HB_STATUS_ENABLEf, 0);
                WRITE_NSYNC_MISC_CLK_EVENT_CTRLr(unit, regval);
            }

            READ_NSYNC_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NSYNC_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_NSYNC_MISC_CLK_EVENT_CTRLr(unit, regval);

        } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {

            READ_NS_BS0_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_LOWf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_LOWf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_LOWf, 0);
            WRITE_NS_BS0_BS_CONFIGr(unit, regval);

            READ_NS_BS1_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_LOWf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_LOWf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_LOWf, 0);
            WRITE_NS_BS1_BS_CONFIGr(unit, regval);

            READ_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_NS_BS0_BS_OUTPUT_TIME_0r(unit, regval);

            READ_NS_BS1_BS_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_NS_BS1_BS_OUTPUT_TIME_0r(unit, regval);

            /*
             * Reset Clock Control
             */
            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /*
             * Reset HeartBeat
             */
            READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr, TIME_CAPTURE_ENABLEf))
            {
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, TIME_CAPTURE_ENABLEf, 0);
            }
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_RX_HB_STATUS_ENABLEf, 0);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

        }
        else if (soc_is(unit, DNX_DEVICE))
        {
            READ_CMIC_BS0_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            WRITE_CMIC_BS0_CONFIGr(unit, regval);

            READ_CMIC_BS1_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            WRITE_CMIC_BS1_CONFIGr(unit, regval);

            READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_CMIC_BS0_OUTPUT_TIME_0r(unit, regval);

            READ_CMIC_BS1_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_CMIC_BS1_OUTPUT_TIME_0r(unit, regval);

            /* Reset Clock Control */
            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            /* Reset HeartBeat */
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval);

            /* Reset Capture */
            READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, &regval, INT_ENABLEf, 0);
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);

            READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, TIME_CAPTURE_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC1_RX_HB_STATUS_ENABLEf, 0);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);

            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            READ_CMIC_BS_CONFIGr(unit, &regval);
            /* Configure Broadsync in output mode */
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            WRITE_CMIC_BS_CONFIGr(unit, regval);

            READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_CMIC_BS_OUTPUT_TIME_0r(unit, regval);

            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            /* Reset Clock Toggle  */

            READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
            /* Configure the HW to give interrupt on every heartbeat */
            soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                              INT_ENABLEf, 0);
            /* Configure the HW to capture time on every heartbeat */
            soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                              TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_HEARTBEAT);
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        }
#endif

    }

    return _SHR_E_NONE;
}

/*
 * Function:
 *    _bcm_dnx_time_deinit
 * Purpose:
 *    Internal routine used to free time software module
 *    control structures.
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    time_cfg_pptr  - (IN) Pointer to pointer to time config structure.
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_deinit(int unit, _bcm_time_config_p *time_cfg_pptr)
{
    int                 idx;
    _bcm_time_config_p  time_cfg_ptr;

    SHR_FUNC_INIT_VARS(unit);
    /* Sanity checks. */
    if (NULL == time_cfg_pptr) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal parameter time_cfg_pptr (NULL)\n");
    }

    time_cfg_ptr = *time_cfg_pptr;
    /* If time config was not allocated we are done. */
    if (NULL == time_cfg_ptr) {
        SHR_EXIT();
    }
    {
        /* Free time interface */
        int *num_time_interface_p;
        int num_time_interface;
        SHR_IF_ERR_EXIT(
            soc_info_int_address_get(unit,NUM_TIME_INTERFACE_INT,&num_time_interface_p));
        num_time_interface = *num_time_interface_p;
        if (NULL != time_cfg_ptr->intf_arr) {
            for (idx = 0; idx < num_time_interface; idx++) {
                SHR_FREE(time_cfg_ptr->intf_arr[idx].user_cb);
            }
            SHR_FREE(time_cfg_ptr->intf_arr);
        }
    }
    /* Destroy protection mutex. */
    if (NULL != time_cfg_ptr->mutex) {
        sal_mutex_destroy(time_cfg_ptr->mutex);
    }
    {
        int *int_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* If any registered function - deregister */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&int_p) );
        *int_p = 0;
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );
        *soc_time_callout_p = NULL;
    }
    /* Free module configuration structue. */
    SHR_FREE(time_cfg_ptr);
    *time_cfg_pptr = NULL;
exit:
    SHR_FUNC_EXIT;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *    _bcm_dnx_time_reinit
 * Purpose:
 *    Internal routine used to reinitialize time module based on HW settings
 *    during Warm boot
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *    _SHR_E_XXX
 */
static int 
_bcm_dnx_time_reinit(int unit, bcm_time_if_t intf_id)
{
    int hb_enable; 

    _bcm_dnx_time_swstate_restore(unit, intf_id, TIME_INTERFACE(unit, intf_id));
    if ((TIME_INTERFACE(unit, intf_id))->flags & BCM_TIME_SYNC_STAMPER) {

#ifdef EDKHLIB
        /* For J2X, this is not required ? */
#else
        /* Broadsync was initialised. Hence reattach */
        (void)_bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
#endif
    }

    /* Read the status of heartbeat interrupt */
    BCM_IF_ERROR_RETURN(
        bcm_dnx_time_heartbeat_enable_get(unit, intf_id, &hb_enable));

    /* If heartbeat was enabled, restore handling functionality */
    if (hb_enable) {
        BCM_IF_ERROR_RETURN(
            bcm_dnx_time_heartbeat_enable_set(unit, intf_id, hb_enable));
    }

    return (_SHR_E_NONE);
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *    _bcm_dnx_time_interface_id_validate
 * Purpose:
 *    Internal routine used to validate interface identifier
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to validate
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_interface_id_validate(int unit, bcm_time_if_t id)
{
    if (0 == TIME_INIT(unit)) {
        return (_SHR_E_INIT);
    }
    if (id < 0 || id > time_interface_id_max(unit)) {
        return (_SHR_E_PARAM);
    }
    if (NULL == TIME_INTERFACE(unit, id)) {
        return (_SHR_E_NOT_FOUND);
    }

    return (_SHR_E_NONE);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_input_validate
 * Purpose:
 *    Internal routine used to validate interface input
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf           - (IN) Interface to validate
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_interface_input_validate(int unit, bcm_time_interface_t *intf)
{
    /* Sanity checks. */
    if (NULL == intf) {
        return (_SHR_E_PARAM);
    }
    if (intf->flags & BCM_TIME_WITH_ID) {
        if (intf->id < 0 || intf->id > time_interface_id_max(unit) ) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Bad time interface ID (%d)\n"), intf->id));
            return (_SHR_E_PARAM);
        }
    }

    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        if (intf->heartbeat_hz <= 0 || intf->bitclock_hz <= 0) {
            return (BCM_E_PARAM);
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        if (intf->drift.nanoseconds > TIME_DRIFT_MAX) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Bad time interface drift (%d ppb)\n"), (int)intf->drift.nanoseconds));
            return _SHR_E_PARAM;
        }
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (intf->offset.nanoseconds > TIME_NANOSEC_MAX) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Bad time interface offset nanoseconds (%d)\n"), (int)intf->offset.nanoseconds));
            return _SHR_E_PARAM;
        }
    }

    return (_SHR_E_NONE);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_allocate_id
 * Purpose:
 *    Internal routine used to allocate time interface id
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (OUT) Interface id to be allocated
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_interface_allocate_id(int unit, bcm_time_if_t *id)
{
    int                              idx;  /* Time interfaces iteration index.*/
    _bcm_time_interface_config_p     intf; /* Time interface description.     */

    /* Input parameters check. */
    if (NULL == id) {
        return (_SHR_E_PARAM);
    }

    /* Find & allocate time interface. */
    for (idx = 0; idx < TIME_CONFIG(unit)->intf_count; idx++) {
        intf = TIME_CONFIG(unit)->intf_arr + idx;
        if (intf->ref_count) {  /* In use interface */
            continue;
        }
        intf->ref_count++;  /* If founf mark interface as in use */
        *id = intf->time_interface.id; /* Assign ID */
        return (_SHR_E_NONE);
    }

    /* No available interfaces */
    return (_SHR_E_FULL);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_heartbeat_install
 * Purpose:
 *    Internal routine used to install interface heartbeat rate into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    _SHR_E_XXX
 */
int
_bcm_dnx_time_interface_heartbeat_install(int unit, bcm_time_if_t id)
{
    uint32  regval;
    uint32  hb_hz;  /* Number of heartbeats in HZ */
    uint32  threshold; /* Heartbeat threshold */

    bcm_time_interface_t    *intf;
    uint32  interval = 0;

    intf = TIME_INTERFACE(unit, id);

    hb_hz = (intf->heartbeat_hz > TIME_HEARTBEAT_HZ_MAX) ? 
        TIME_HEARTBEAT_HZ_MAX : intf->heartbeat_hz;
    hb_hz = (intf->heartbeat_hz < TIME_HEARTBEAT_HZ_MIN) ? 
        TIME_HEARTBEAT_HZ_MIN : hb_hz;

    threshold = intf->bitclock_hz / hb_hz;

    if (threshold < 100) {
        /* BitClock must have a frequency at least 100 times that of heartbeat. */
        return _SHR_E_PARAM;
    }

    if (soc_is(unit, DNX_DEVICE)) {
        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        /* enable Heartbeat generation */
        READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1); 
        WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

        /* Install heartbeat up and down interval */ 
        READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
        WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
        WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
        WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
        WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        /* Enable heartbeat output */
        READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval);

        /* 
         * KT-1381 :
         *  TS_GPIO_LOW = (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
         *  TS_GPIO_HIGH = (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
         */ 
        threshold = (threshold * 4) /*+ SOC_TIMESYNC_PLL_CLOCK_NS(unit);*/;

        /* Install GPIO timesync trigger interval */
        /* GPIO_EVENT_1 */
        READ_CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, interval);

        READ_CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_2 */
        READ_CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);

        READ_CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_3 */
        READ_CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);

        READ_CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_4 */
        READ_CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);

        READ_CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_5 */
        READ_CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);

        READ_CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, interval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS_CLK_CTRLr(unit, regval);


        READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1); 
        WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);


        /* Install heartbeat up and down interval */ 
        READ_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, &interval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_UP_DURATIONr, &interval, 
                          UP_DURATIONf, threshold); 
        WRITE_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, interval);

        READ_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, &interval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_DOWN_DURATIONr, &interval, 
                          DOWN_DURATIONf, threshold); 
        WRITE_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, interval);

        /* Install divisor for LCPLL trigger interval */
        READ_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_LCPLL_CLK_COUNT_CTRLr, &interval, 
                          DIVISORf, threshold);
        WRITE_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, interval);

        /* Install divisor for Primary L1 trigger interval */
        READ_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, &interval);
        soc_reg_field_set(unit,  CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr, &interval, 
                          DIVISORf, threshold);
        WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, interval);

        /* Install divisor for backup L1 trigger interval */
        READ_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, &interval);
        soc_reg_field_set(unit,  CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr, &interval, 
                          DIVISORf, threshold);
        WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, interval);

        /* 
         *  TS_GPIO_LOW = (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
         *  TS_GPIO_HIGH = (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
         */ 
        /* BCM_TIMESYNC_SUPPORT is not defined for JER2_ARAD so commenting out - > SOC_TIMESYNC_PLL_CLOCK_NS */
        threshold = (threshold * 4) /* + SOC_TIMESYNC_PLL_CLOCK_NS(unit) */;

        /* Install GPIO timesync trigger interval */
        /* GPIO_EVENT_1 */
        READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);
        READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_2 */
        READ_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);
        READ_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_2_UP_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_3 */
        READ_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);
        READ_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_3_UP_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_4 */
        READ_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);
        READ_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_4_UP_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, interval);

        /* GPIO_EVENT_5 */
        READ_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, interval);
        READ_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, &interval);
        soc_reg_field_set(unit, CMIC_TS_GPIO_5_UP_EVENT_CTRLr, &interval,
                          INTERVAL_LENGTHf, threshold);
        WRITE_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, interval);
    }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dnx_time_swstate_store(unit, intf->id, TIME_INTERFACE(unit, intf->id));
#endif
    return (_SHR_E_NONE);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_accuracy_time2hw
 * Purpose:
 *    Internal routine used to compute HW accuracy value from interface
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id
 *    accuracy       - (OUT) HW value to be programmed
 * Returns:
 *    _SHR_E_XXX
 */
int
_bcm_dnx_time_interface_accuracy_time2hw(int unit, bcm_time_if_t id,
                                         uint32 *accuracy)
{
    int                     idx;    /* accuracy itterator */
    bcm_time_interface_t    *intf;

    if (NULL == accuracy) {
        return _SHR_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, id);

    /* Find the right accuracy */
    for (idx = 0; idx < TIME_ACCURACY_CLK_MAX; idx++) {
        if (intf->accuracy.nanoseconds <= _bcm_time_accuracy_arr[idx].nanoseconds && 
             COMPILER_64_LO(intf->accuracy.seconds) <= COMPILER_64_LO(_bcm_time_accuracy_arr[idx].seconds) ) {
            break;
        }
    }
    /* if no match - return error */
    if (idx == TIME_ACCURACY_CLK_MAX) {
        return _SHR_E_NOT_FOUND;
    }

    /* Return the correct HW value */

    *accuracy = TIME_ACCURACY_SW_IDX_2_HW(idx);

    return (_SHR_E_NONE);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_drift_install
 * Purpose:
 *    Internal routine used to install interface drift into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    _SHR_E_XXX
 */
static int 
_bcm_dnx_time_interface_drift_install(int unit, bcm_time_if_t id)
{    
    return _SHR_E_UNAVAIL;
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * Function:
 *    _bcm_dnx_time_interface_offset_install
 * Purpose:
 *    Internal routine used to install interface offset into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    _SHR_E_XXX
 */
static int 
_bcm_dnx_time_interface_offset_install(int unit, bcm_time_if_t id)
{
    return _SHR_E_UNAVAIL;
}
#endif


/*
 * Function:
 *    _bcm_dnx_time_interface_ref_clock_install
 * Purpose:
 *    Internal routine to install timesync clock divisor to
 *    enable broadsync reference clock.
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    _SHR_E_XXX
 */
static int 
_bcm_dnx_time_interface_ref_clock_install(int unit, bcm_time_if_t id)
{
    uint32 regval, ndiv, hdiv, ref_clk;
    bcm_time_interface_t    *intf;

    intf = TIME_INTERFACE(unit, id);
    
    /* Validate and calculate ts_clk divisor to generate reference clock */
    if (intf->clk_resolution <= 0) {
        return _SHR_E_PARAM;
    }
    
    /* Maximum ts_clk frequency is of 25Mhz(40ns) */
    ref_clk = (intf->clk_resolution > TIME_TS_CLK_FREQUENCY_40NS) ?
              TIME_TS_CLK_FREQUENCY_40NS : intf->clk_resolution;
   
    /* Divisor is half period for reference clock */ 
    ndiv = TIME_TS_CLK_FREQUENCY_40NS / ref_clk;
    
    /* Divisor is ceiling of half period */
    hdiv = (ndiv + 1)/2 ? ndiv : 1;

    if (soc_is(unit, J2X_DEVICE)) {
        /* This will be done as part of settingup the BS-Synth channel */
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE))
    {
        READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 1);
        soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, DIVISORf, hdiv);
        WRITE_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);
    }
    else if (soc_is(unit, DNX_DEVICE))
    {
        READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval); 
        soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, 
                          ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, 
                          DIVISORf, hdiv);
        WRITE_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval,
                          ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval,
                          DIVISORf, hdiv);
        WRITE_CMIC_BS_REF_CLK_GEN_CTRLr(unit, regval);
    }
#endif

    return (_SHR_E_NONE);
}


/*
 * Function:
 *    _bcm_dnx_time_interface_install
 * Purpose:
 *    Internal routine used to install interface settings into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Interface id to be installed into a HW
 * Returns:
 *    _SHR_E_XXX
 */
static int
_bcm_dnx_time_interface_install(int unit, bcm_time_if_t intf_id)
{
    bcm_time_interface_t    *intf;  /* Time interface */
    uint32                  regval; /* For register read and write operations */
    uint32                  hw_val; /* Value to program into a HW */
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    uint32                  second, diff, delay = BROAD_SYNC_OUTPUT_TOGGLE_TIME_DELAY;
    int                     enable = 0;
    soc_reg_t               reg; 
    bcm_time_spec_t         toggle_time;
#endif

    if (NULL == TIME_INTERFACE(unit, intf_id)) {
        return _SHR_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, intf_id);

    if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);
    }else if (soc_is(unit, DNX_DEVICE)) {
        READ_CMIC_BS0_CONFIGr(unit, &regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_CONFIGr(unit, &regval);
    }
#endif

    if ((intf->flags & BCM_TIME_ENABLE)) {
        /* Enable all three broadsync pins:
         *     Bit clock, HeartBeat and Timecode 
         */
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
        } else if (soc_is(unit, DNX_DEVICE)) {
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(
                unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
        }
#endif
    } else {
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
        } else if (soc_is(unit, DNX_DEVICE)) {
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
        }
#endif
    }

    if ((intf->flags & BCM_TIME_LOCKED)) {
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field32_modify(
                    unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        }
#endif
    } else {
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        } else if (soc_is(unit, DNX_DEVICE)) {
            soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field32_modify(
                    unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }
#endif
    }


    if (intf->flags & BCM_TIME_INPUT) {
        /* Set Broadsync in Input mode */
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);
        }else if (soc_is(unit, DNX_DEVICE)) {
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);
        }
#endif
    } else {
        /* Configure Accuracy, offset and drift for broadsync output mode */
        if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
        }else if (soc_is(unit, DNX_DEVICE)) {
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, 
                            TIME_MODE_OUTPUT);
        }
#endif
        if (intf->flags & BCM_TIME_ACCURACY) {
            BCM_IF_ERROR_RETURN(
                _bcm_dnx_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
            if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
                soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, ACCURACYf, hw_val);
            }else if (soc_is(unit, DNX_DEVICE)) {
                soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r, REG_PORT_ANY, ACCURACYf, hw_val);
            }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
            else {
                soc_reg_field32_modify(unit, CMIC_BS_OUTPUT_TIME_0r, 
                                    REG_PORT_ANY, ACCURACYf, hw_val);
            }
#endif
        }
        if (intf->flags & BCM_TIME_HEARTBEAT) {
            if (!soc_is(unit,Q2A_DEVICE) && !soc_is(unit, J2C_DEVICE) && !soc_is(unit, J2X_DEVICE)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_dnx_time_interface_heartbeat_install(unit, intf_id));
            }

            if (!(intf->flags & BCM_TIME_OFFSET)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_dnx_time_capture_get(unit, intf_id, TIME_CAPTURE(unit, intf_id), intf->flags));
            }
        }
    }
    if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
        WRITE_NS_BS0_BS_CONFIGr(unit, regval);
    }else if (soc_is(unit, DNX_DEVICE)) {
        WRITE_CMIC_BS0_CONFIGr(unit, regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        WRITE_CMIC_BS_CONFIGr(unit, regval);
    }
#endif

    if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE)) {
        return (_SHR_E_NONE);
    } else if (soc_is(unit, DNX_DEVICE)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        if (intf->flags & BCM_TIME_OFFSET) {
            reg   =   CMIC_BS_CLK_CTRLr;
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);

            /* Stop broadsync output if already enabled */
            enable = soc_reg_field_get(unit, reg, regval, ENABLEf);
            if (1 == enable) {
                soc_reg_field_set(unit, reg, &regval, ENABLEf, 0);
                WRITE_CMIC_BS_CLK_CTRLr(unit, regval);
                BCM_IF_ERROR_RETURN(
                    _bcm_dnx_time_capture_get(unit, intf_id, TIME_CAPTURE(unit, intf_id), intf->flags));
            }

            BCM_IF_ERROR_RETURN(
                _bcm_dnx_time_interface_offset_install(unit, intf_id));

            if (intf->offset.isnegative) {
                second = COMPILER_64_LO(SYNT_TIME_SECONDS(unit, intf_id)) -
                         COMPILER_64_LO(intf->offset.seconds);

                if (intf->offset.nanoseconds <= SYNT_TIME_NANOSECONDS(unit, intf_id)) {
                    toggle_time.nanoseconds = SYNT_TIME_NANOSECONDS(unit, intf_id) -
                                              intf->offset.nanoseconds;
                } else {
                    /* Wrapped */
                    diff = intf->offset.nanoseconds - SYNT_TIME_NANOSECONDS(unit, intf_id);
                    toggle_time.nanoseconds = TIME_NANOSEC_MAX - diff;
                    second--;
                }
                COMPILER_64_SET(toggle_time.seconds, 0, second); 
            } else {
                second = COMPILER_64_LO(SYNT_TIME_SECONDS(unit, intf_id)) +
                         COMPILER_64_LO(intf->offset.seconds);
                toggle_time.nanoseconds = 
                    SYNT_TIME_NANOSECONDS(unit, intf_id) + intf->offset.nanoseconds;
                if (toggle_time.nanoseconds >= TIME_NANOSEC_MAX) {
                    toggle_time.nanoseconds -= TIME_NANOSEC_MAX;
                    second++;
                }
                COMPILER_64_SET(toggle_time.seconds, 0, second); 
            }

            if (1 == enable) {
                /* Reenable broadsync output */
                COMPILER_64_TO_32_LO(second, toggle_time.seconds);
                second += delay;

                READ_CMIC_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_BS_CLK_CTRLr(unit, regval);
            }
        }
#endif

        if (intf->flags & BCM_TIME_DRIFT) {
            BCM_IF_ERROR_RETURN(
                _bcm_dnx_time_interface_drift_install(unit, intf_id));
        }
    }

    return (_SHR_E_NONE);
}


/*
 * Function:
 *     _bcm_dnx_time_interface_free
 * Purpose:
 *     Free time interface.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      intf_id         - (IN) time interface id.
 * Returns:
 *      BCM_X_XXX
 */

static int 
_bcm_dnx_time_interface_free(int unit, bcm_time_if_t intf_id) 
{
    _bcm_time_interface_config_p  intf_cfg; /* Time interface config.*/

    intf_cfg = &TIME_INTERFACE_CONFIG(unit, intf_id); 

    if (intf_cfg->ref_count > 0) {
        intf_cfg->ref_count--;
    }

    if (0 == intf_cfg->ref_count) {
        sal_memset(&intf_cfg->time_interface, 0, sizeof(bcm_time_interface_t));
        intf_cfg->time_interface.id = intf_id;
    }

    return (_SHR_E_NONE);
}

/*
 * Function:
 *      _bcm_dnx_time_accuracy_parse
 * Purpose:
 *      Internal routine to parse accuracy hw value into bcm_time_spec_t format
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      accuracy    - (IN) Accuracy HW value 
 *      time        - (OUT) bcm_time_spec_t structure to contain accuracy 
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int 
_bcm_dnx_time_accuracy_parse(int unit, uint32 accuracy, bcm_time_spec_t *time)
{
    if (accuracy < TIME_ACCURACY_LOW_HW_VAL || 
        (accuracy > TIME_ACCURACY_HIGH_HW_VAL && 
         accuracy != TIME_ACCURACY_UNKNOWN_HW_VAL)) {
        return (_SHR_E_PARAM);
    }

    time->isnegative = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].isnegative;
    time->nanoseconds = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].nanoseconds;
    time->seconds = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].seconds;
    time->isnegative = 0;   /* Can't be negative */

    return (_SHR_E_NONE);
}


/*
 * Function:
 *      _bcm_dnx_time_input_parse
 * Purpose:
 *      Internal routine to parse input time information stored in 3 registeres
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      data0   - (IN) Data stored in register 0 conrain input time information
 *      data1   - (IN) Data stored in register 1 conrain input time information
 *      data2   - (IN) Data stored in register 2 conrain input time information
 *      time    - (OUT) Structure to contain input time information
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int
_bcm_dnx_time_input_parse(int unit, uint32 data0, uint32 data1, uint32 data2,
                          bcm_time_capture_t *time)
{
    uint32 sec_hi, sec_lo;

    /* Parsing the broadsync ToD time the same way it is parsed in the ukernel code,
       from is the ukernel code parsing comment :*/
    
    /* INPUT_TIME_2: */
    /*   bit 24 is lock */
    /*   bits 23-8 are epoch */
    /*   bits 7-0 are top 8 bits of seconds */
    /* INPUT_TIME_1: */
    /*   bits 31-8 are lower 24 bits of seconds */
    /*   bits 7,6 are reserved (high 2 bits of 32-bit NS, essentially) */
    /*   bits 5-0 are top 6 bits of 30-bit NS */
    /* INPUT_TIME_0: */
    /*   bits 31-8 are lower 24 bits of NS */
    /*   bits 7-0 are accuracy */
    /* unsigned lock = ((input_high >> 24) & 1); */

    sec_hi = ((data2 >> 8) & 0x0000ffff);
    sec_lo = ((data2 << 24) | ((data1 >> 8) & 0x00ffffff));
    

    COMPILER_64_SET(time->received.seconds, sec_hi, sec_lo);  
    time->received.nanoseconds = ((((data1 & 0x3f) << 24)) | ((data0 >> 8) & 0x00ffffff));

    /* not setting accuracy since it is set to ZERO for the JER2_ARAD, which is not a valid value that returns error */    
    /* accuracy = ((data0 << 24) >> 24); */

    /* return _bcm_esw_time_accuracy_parse(unit,accuracy,  */
    /*                                     &(time->received_accuracy)); */

    return _SHR_E_NONE;
}


/*
 * Function:
 *      _bcm_dnx_time_capture_counter_read
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
_bcm_dnx_time_capture_counter_read(int unit, bcm_time_if_t id,
                                   bcm_time_capture_t *time)
{
    uint32                  regval;
    bcm_time_interface_t    *intf;
  

    /* for some reason the FIFO_DEPTH field is '0' for the JER2_ARAD chip */
    /* so commenting out follwing code (CMIC_TS) that exist in other devices that use CMICm */

    /* uint32 event_id; */
         
    /* Check FIFO depth */
    /* sal_memset(time, 0, sizeof(bcm_time_capture_t)); */
    /* READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval); */
    /* if (0 == soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr, */
    /*                            regval, FIFO_DEPTHf)) { */
    /*     return (_SHR_E_EMPTY); */
    /* } */

    /* Read timestamp from FIFO */
    /* READ_CMIC_TS_INPUT_TIME_FIFO_TSr(unit, &regval); */
    /* time->synchronous.nanoseconds =  soc_reg_field_get(unit, */
    /*                                                    CMIC_TS_INPUT_TIME_FIFO_TSr, regval, */
    /*                                                    TIMESTAMPf); */

    /* If it's not valid, something is wrong (since FIFO is not empty) */
    /* READ_CMIC_TS_INPUT_TIME_FIFO_IDr(unit, &regval); */
    /* if (0 == soc_reg_field_get( */
    /*                            unit, CMIC_TS_INPUT_TIME_FIFO_IDr, regval, */
    /*                            VALIDf)) { */
    /*     return (_SHR_E_FAIL); */
    /* } */

    /* Check capture event id */
    /* event_id = soc_reg_field_get( */
    /*                              unit, CMIC_TS_INPUT_TIME_FIFO_IDr, */
    /*                              regval, EVENT_IDf); */

    /* if (event_id == 0) { */
    /*     time->flags |= BCM_TIME_CAPTURE_IMMEDIATE; */
    /* } */


    /* For backward compatibility */
    time->free      = time->synchronous;
    time->syntonous = time->synchronous;
    
    time->free.isnegative = time->synchronous.isnegative = 
        time->syntonous.isnegative = 0;

    /* If interface is in input mode read time provided by the master */
    intf = TIME_INTERFACE(unit, id);
    if (intf->flags & BCM_TIME_INPUT) {
        uint32  data0 = 0, data1 = 0, data2 = 0;

        if (soc_is(unit, DNX_DEVICE)) {
            READ_CMIC_BS0_INPUT_TIME_2r(unit, &regval);
            if (soc_reg_field_get(unit, CMIC_BS0_INPUT_TIME_2r, regval, 
                                  CHECKSUM_ERRORf)) {
                return _SHR_E_INTERNAL;
            }
            data2 = soc_reg_field_get(
                        unit, CMIC_BS0_INPUT_TIME_2r, regval, DATAf);
            READ_CMIC_BS0_INPUT_TIME_1r(unit, &regval);
            data1 = soc_reg_field_get(
                        unit, CMIC_BS0_INPUT_TIME_1r, regval, DATAf);
            READ_CMIC_BS0_INPUT_TIME_0r(unit, &regval);
            data0 = soc_reg_field_get(
                        unit, CMIC_BS0_INPUT_TIME_0r, regval, DATAf);
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            READ_CMIC_BS_INPUT_TIME_2r(unit, &regval);
            if (soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_2r, regval, 
                                  CHECKSUM_ERRORf)) {
                return _SHR_E_INTERNAL;
            }
            data2 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_2r, regval, DATAf);
            READ_CMIC_BS_INPUT_TIME_1r(unit, &regval);
            data1 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_1r, regval, DATAf);
            READ_CMIC_BS_INPUT_TIME_0r(unit, &regval);
            data0 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_0r, regval, DATAf);
        }
#endif

        BCM_IF_ERROR_RETURN(
            _bcm_dnx_time_input_parse(unit, data0, data1, data2, time));
    }

    return (_SHR_E_NONE);
}

/*
 * Function:
 *      _bcm_dnx_time_capture_get
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int
_bcm_dnx_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time,  uint32 flags)
{
 
    uint32          regval, orgval; /* To keep register value */
    int             hw_complete;    /* HW read completion indicator*/
    soc_timeout_t   timeout;        /* Timeout in case of HW error */

    if (soc_is(unit,Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2X_DEVICE)) {
        uint32 lower = 0, upper = 0;
        READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
        orgval = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr,
                                   regval, ENABLEf);
        if (flags == BCM_TIME_CAPTURE_IMMEDIATE || orgval == TIME_CAPTURE_MODE_DISABLE ||
               orgval == TIME_CAPTURE_MODE_IMMEDIATE) {

            if (soc_is(unit, J2X_DEVICE)){
#if defined(BCM_J2X_SUPPORT)
                /* do an immediate capture */
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_UPPER_STATUSr, 0, 0, &regval);
                upper = regval;
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_LOWER_STATUSr, 0, 0, &regval);
                lower = regval;
#endif
            } else {
                /* do an immediate capture */
                READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &regval);
                upper = regval;
                READ_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit, &regval);
                lower = regval;

            }
            /* Discard 4bits from lower*/
            lower = (lower >> 4);
            lower = (lower & 0x0FFFFFFF) | ((upper & 0xF) << 28);
            upper = (upper >> 4);

            if (soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                /* 48-bit timestamping supported */
                uint64 time_val, secs, nano_secs;
                uint64 div;
                /* Get the 48-bit timestamp value */
                COMPILER_64_SET(time_val, upper, lower);
                /* Get seconds from the timestamp value */
                secs = time_val;
                COMPILER_64_SET(div, 0, 1000000000);
                COMPILER_64_UDIV_64(secs, div);
                time->synchronous.seconds = secs;

                /* Get nanoseconds from the timestamp value */
                nano_secs = time_val;
                COMPILER_64_UMUL_32(secs, 1000000000);
                COMPILER_64_SUB_64(nano_secs, secs);
                time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);
            } else {
                COMPILER_64_SET(time->synchronous.seconds, 0, lower / 1000000000);
                time->synchronous.nanoseconds = lower % 1000000000;
            }

            /* For backward compatibility */
            time->free = time->synchronous;
            time->syntonous = time->synchronous;
            time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
            return BCM_E_NONE;
        }
    } else if (soc_is(unit, DNX_DEVICE)) {
        /* Read capture mode */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        orgval = soc_reg_field_get(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, 
                                   regval, TIME_CAPTURE_MODEf);

        /* If any trigger condition is set, get the timestamp from FIFO */
        if ((orgval != TIME_CAPTURE_MODE_DISABLE) && 
            (orgval != TIME_CAPTURE_MODE_IMMEDIATE)) {
            return _bcm_dnx_time_capture_counter_read(unit, id, time);
        }

        /* Otherwise, do an immediate capture */
        
        /* Make sure timestamp capture is enabled */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
        if (0 == soc_reg_field_get(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, 
                                   regval, TIME_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, 
                            &regval, TIME_CAPTURE_ENABLEf, 1 );
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
        }

        /* Clear the FIFO if not empty */
        for(;;) {
            READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
            if (0 == soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r, 
                                       regval, FIFO_DEPTHf)) {
                break;
            }
            /* There is still something in the FIFO, pop it */
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &regval);
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval);
        }
        
        /* Make sure the capture status is cleared */
        READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
        if (regval != 0) {
            soc_reg_field_set(unit, CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r, 
                              &regval, TIME_CAPTURE_COMPLETE_CLRf, 1);
            soc_reg_field_set(unit, CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r, 
                              &regval, FIFO_WRITE_COMPLETE_CLRf, 1);
            WRITE_CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r(unit, regval);
        }
        
        /* Program HW to capture time immediately */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, 
                          TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_IMMEDIATE);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);

        /* Wait for HW time capture completion */
        hw_complete = 0;
        soc_timeout_init(&timeout, BROAD_SYNC_TIME_CAPTURE_TIMEOUT, 0);
        while (!hw_complete) {
            READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
            hw_complete = soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r,
                                            regval, TIME_CAPTURE_COMPLETEf); 
            if (soc_timeout_check(&timeout)) {
                return (_SHR_E_TIMEOUT);
            }
        }

        /* Program HW to disable time capture */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, 
                            TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);
        
        /* Read the HW time */
        READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
        if (1 != soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r, 
                                   regval, FIFO_DEPTHf)) {
            return (BCM_E_FAIL);
        }
        /* Should be only one entry in the fifo. Pop it and return the value*/
        {
            uint32 lower, upper;
            uint64 time_val, secs, nano_secs;
            uint64 div;
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &lower);
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval);
            upper = soc_reg_field_get(
                                    unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                    regval, TIMESTAMPf);

            /* Get the 48-bit timestamp value */
            COMPILER_64_SET(time_val, upper, lower);
            /* Get seconds from the timestamp value */
            secs = time_val;
            COMPILER_64_SET(div, 0, 1000000000);
            COMPILER_64_UDIV_64(secs, div);
            time->synchronous.seconds = secs;

            /* Get nanoseconds from the timestamp value */
            nano_secs = time_val;
            COMPILER_64_UMUL_32(secs, 1000000000);
            COMPILER_64_SUB_64(nano_secs, secs);
            time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);

            /* For backward compatibility */
            time->free = time->synchronous;
            time->syntonous = time->synchronous;
            time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
            return BCM_E_NONE;
        }
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        /* Read capture mode */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        /* If any trigger condition is set, get the timestamp from FIFO */
        if (TIME_CAPTURE_MODE_DISABLE != soc_reg_field_get(unit,
                                                           CMIC_TS_TIME_CAPTURE_CTRLr, regval, TIME_CAPTURE_MODEf) ) {
            return _bcm_dnx_time_capture_counter_read(unit, id, time);
        }

        /* if different than interrupt remember original capture mode */
        orgval = regval;

        /* Program HW to disable time capture */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                          TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
        if (orgval != regval) {
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        }
            
        /* Make sure timestamp capture is enabled */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        if (0 == soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                                   regval, TIME_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                              &regval, TIME_CAPTURE_ENABLEf, 1 );
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        }       

        /* Clear the FIFO if not empty */
        for(;;) {
            READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
            if (0 == soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr,
                                       regval, FIFO_DEPTHf)) {
                break;
            }
            /* There is still something in the FIFO, pop it */
            READ_CMIC_TS_INPUT_TIME_FIFO_TSr(unit, &regval);
        }

        /* Make sure the capture status is cleared */
        READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
        if (regval != 0) {
            soc_reg_field_set(unit, CMIC_TS_CAPTURE_STATUS_CLRr,
                              &regval, TIME_CAPTURE_COMPLETE_CLRf, 1);
            soc_reg_field_set(unit, CMIC_TS_CAPTURE_STATUS_CLRr,
                              &regval, FIFO_WRITE_COMPLETE_CLRf, 1);
            WRITE_CMIC_TS_CAPTURE_STATUS_CLRr(unit, regval);
        }
     
        /* Program HW to capture time immediately */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                          TIME_CAPTURE_MODEf,TIME_CAPTURE_MODE_IMMEDIATE);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);

        /* Wait for HW time capture completion */
        hw_complete = 0;
        soc_timeout_init(&timeout, BROAD_SYNC_TIME_CAPTURE_TIMEOUT, 0);

        while (!hw_complete) {
            READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
            hw_complete = soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr,
                                            regval, TIME_CAPTURE_COMPLETEf); 
            if (soc_timeout_check(&timeout)) {
                return (_SHR_E_TIMEOUT);
            }
        }
        
        /* Read the HW time */
        BCM_IF_ERROR_RETURN(_bcm_dnx_time_capture_counter_read(unit, id, time));

        /* Program HW to original time capture value */
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, orgval);
    }
#endif

    return (_SHR_E_NONE);
}


/*
 * Function:
 *      _bcm_dnx_time_interface_offset_get
 * Purpose:
 *      Internal routine to read HW offset value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      offset  - (OUT) Time interface  offset
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int 
_bcm_dnx_time_interface_offset_get(int unit, bcm_time_if_t id,
                                   bcm_time_spec_t *offset)
{

    return _SHR_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_dnx_time_interface_drift_get
 * Purpose:
 *      Internal routine to read HW drift value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      drift   - (OUT) Time interface  drift
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int 
_bcm_dnx_time_interface_drift_get(int unit, bcm_time_if_t id,
                                  bcm_time_spec_t *drift)
{

    return _SHR_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_dnx_time_interface_ref_clock_get
 * Purpose:
 *      Internal routine to read ref clock divisor 
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      divisor - (OUT) Time interface  ref clock resolution 
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int 
_bcm_dnx_time_interface_ref_clock_get(int unit, bcm_time_if_t id,
                                  int *clk_resolution)
{
    uint32 regval, val = 0, hdiv = 0;

    if (soc_is(unit, J2X_DEVICE)) {
        if (id == 0) {
            READ_NSYNC_BROADSYNC_0_REF_CLK_GEN_CTRLr(unit, &regval);
        } else {
            READ_NSYNC_BROADSYNC_1_REF_CLK_GEN_CTRLr(unit, &regval);
        }
        val = soc_reg_field_get(unit, NSYNC_BROADSYNC_0_REF_CLK_GEN_CTRLr, regval,
                                ENABLEf);
        hdiv = soc_reg_field_get(unit, NSYNC_BROADSYNC_0_REF_CLK_GEN_CTRLr, regval,
                                 DIVISORf);
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
        READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
        val = soc_reg_field_get(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, regval,
                                ENABLEf);
        hdiv = soc_reg_field_get(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, regval,
                                 DIVISORf);
    } else if (soc_is(unit, DNX_DEVICE)) {
        READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval); 
        val = soc_reg_field_get(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, regval, 
                                ENABLEf);
        hdiv = soc_reg_field_get(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, regval, 
                                 DIVISORf);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval); 
        val = soc_reg_field_get(unit, CMIC_BS_REF_CLK_GEN_CTRLr, regval, 
                                ENABLEf);
        hdiv = soc_reg_field_get(unit, CMIC_BS_REF_CLK_GEN_CTRLr, regval, 
                                 DIVISORf);
    }
#endif
    
    if (val) {
        /* Divisor is half period for reference clock */ 
        *clk_resolution = TIME_TS_CLK_FREQUENCY_40NS / (hdiv * 2);
    } else {
        *clk_resolution = 0;
    }

    return (_SHR_E_NONE);
}
/*
 * Function:
 *      _bcm_dnx_time_interface_accuracy_get
 * Purpose:
 *      Internal routine to read HW accuracy value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit        -  (IN) StrataSwitch Unit #.
 *      id          -  (IN) Time interface identifier
 *      accuracy    - (OUT) Time interface  accuracy
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
_bcm_dnx_time_interface_accuracy_get(int unit, bcm_time_if_t id,
                                     bcm_time_spec_t *accuracy)
{
    uint32 regval, val = 0;

    if (soc_is(unit, DNX_DEVICE)) {
        READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
        val = soc_reg_field_get(unit, CMIC_BS0_OUTPUT_TIME_0r, regval, ACCURACYf);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
        val = soc_reg_field_get(unit, CMIC_BS_OUTPUT_TIME_0r, regval, ACCURACYf);
    }
#endif

    return _bcm_dnx_time_accuracy_parse(unit, val, accuracy);
}


/*
 * Function:
 *      _bcm_dnx_time_interface_get
 * Purpose:
 *      Internal routine to get a time sync interface by id
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      id   -  (IN) Time interface identifier
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
static int
_bcm_dnx_time_interface_get(int unit, bcm_time_if_t id, bcm_time_interface_t *intf)
{
    uint32                  regval, regval1;
    bcm_time_interface_t    *intf_ptr;
    uint32                  orig_flags;

    intf_ptr = TIME_INTERFACE(unit, id);
    orig_flags = intf_ptr->flags;
    intf_ptr->flags = intf->flags;
    intf_ptr->id = id;

    if (soc_is(unit, J2X_DEVICE)) {
        if (id == 0) {
            READ_NSYNC_BS_0_BS_CONFIGr(unit, &regval);
            READ_NSYNC_BS_0_BS_OUTPUT_TIME_0r(unit, &regval1);
        } else {
            READ_NSYNC_BS_1_BS_CONFIGr(unit, &regval);
            READ_NSYNC_BS_1_BS_OUTPUT_TIME_0r(unit, &regval1);
        }

        if (TIME_MODE_INPUT == soc_reg_field_get(unit, NSYNC_BS_0_BS_CONFIGr, regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

        if (soc_reg_field_get(unit, NSYNC_BS_0_BS_OUTPUT_TIME_0r, regval1, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE))
    {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);
        /*
         * Update output flags
         */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, NS_BS0_BS_CONFIGr, regval, MODEf))
        {
            intf_ptr->flags |= BCM_TIME_INPUT;
        }
        else
        {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

        if (soc_reg_field_get(unit, NS_BS0_BS_OUTPUT_TIME_0r, regval, LOCKf))
        {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        }
        else
        {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    }
    else if (soc_is(unit, DNX_DEVICE))
    {
        READ_CMIC_BS0_CONFIGr(unit, &regval);
        
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, CMIC_BS0_CONFIGr,
                                             regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

        if (soc_reg_field_get(unit, CMIC_BS0_CONFIGr,
                          regval, BS_CLK_OUTPUT_ENABLEf)) {
        intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, CMIC_BS0_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_BS_CONFIGr(unit, &regval);
        
        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, CMIC_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

        if (soc_reg_field_get(unit, CMIC_BS_CONFIGr,
                          regval, BS_CLK_OUTPUT_ENABLEf)) {
        intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, CMIC_BS_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    }
#endif

    if (intf->flags & BCM_TIME_ACCURACY) {
        BCM_IF_ERROR_RETURN(
                            _bcm_dnx_time_interface_accuracy_get(unit, id,
                                                                   &(intf_ptr->accuracy)));
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            /* Add _bcm_time_bs_phase_offset_get, and use it here if required*/
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                                _bcm_dnx_time_interface_offset_get(unit, id, &(intf_ptr->offset)));
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            /* Add _bcm_time_bs_frequency_offset_get, and use it here if required*/
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                                _bcm_dnx_time_interface_drift_get(unit, id, &(intf_ptr->drift)));
        }
    }

    if (intf->flags & BCM_TIME_REF_CLOCK) {
        BCM_IF_ERROR_RETURN(
                            _bcm_dnx_time_interface_ref_clock_get(unit, id,
                                                                    &(intf_ptr->clk_resolution)));
    }

    if (orig_flags & BCM_TIME_SYNC_STAMPER) {
        intf_ptr->flags = orig_flags;
    }

    *intf = *(TIME_INTERFACE(unit, id));
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dnx_time_swstate_store(unit, intf->id, TIME_INTERFACE(unit, intf->id));
#endif

    return (_SHR_E_NONE);
}


/*
 * Function:
 *      _bcm_dnx_time_hw_interrupt_dflt
 * Purpose:
 *      Default handler for broadsync heartbeat interrupt
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 */
static void
_bcm_dnx_time_hw_interrupt_dflt(int unit)
{
    uint32  regval;

    /* Due to HW constrains we need to reinable the interrupt on every rising */
    /* edge and update the capture mode */

    if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE) || soc_is(unit, J2X_DEVICE)) {
        return;
    } else if (soc_is(unit, DNX_DEVICE)) {
        uint32  intr, mode;
        /* Read original values */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &mode);
        READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
        READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &intr);
        
        /* Disable all */
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, 0);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, 0);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, 0);
        
        /* Enable with the original settings */
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, intr);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, mode);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, 0);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
    }
#endif
    return;
}

/*
 * Function:
 *      _bcm_dnx_time_hw_interrupt
 * Purpose:
 *      Handles broadsync heartbeat interrupt
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 */
static void
_bcm_dnx_time_hw_interrupt(int unit)
{
    void                    *u_data; 
    bcm_time_heartbeat_cb   u_cb;
    int                     idx;    /* interface itterator */
    bcm_time_capture_t      time = {0};
    int num_time_interface;

    SHR_FUNC_INIT_VARS(unit);
    {
        int *num_time_interface_p;
        SHR_IF_ERR_EXIT(
            soc_info_int_address_get(unit,NUM_TIME_INTERFACE_INT,&num_time_interface_p));
        num_time_interface = *num_time_interface_p;
    }
    for (idx = 0; idx < num_time_interface; idx++) {
        if (NULL != TIME_INTERFACE(unit, idx) &&
            NULL != TIME_INTERFACE_CONFIG(unit,idx).user_cb) {
            u_cb = TIME_INTERFACE_CONFIG(unit,idx).user_cb->heartbeat_cb;
            u_data = TIME_INTERFACE_CONFIG(unit,idx).user_cb->user_data;
            _bcm_dnx_time_capture_counter_read(unit, idx, &time);
            if (NULL != u_cb) {
                u_cb(unit, idx, &time, u_data);
            }
        }
    }

    _bcm_dnx_time_hw_interrupt_dflt(unit);
exit:
    SHR_VOID_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_time_interface_traverse_cb
 * Purpose:
 *      check if TS is enabled on intf
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      intf - interface
 *      user_data - (OUT) 1 TS / 0 no TS
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

static int _bcm_time_interface_traverse_cb(int unit,
                            bcm_time_interface_t *intf, void *user_data)
{
    uint8 *ptr = (uint8 *)user_data;
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        *ptr = 1;
        return BCM_E_NONE;
     } else {
        *ptr = 0;
        return BCM_E_NOT_FOUND;
     }
}
/****************************************************************************/
/*                      API functions implmentation                         */
/****************************************************************************/

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
int bcm_dnx_time_capture_ts_init(int unit);
#endif

/*
 * Function:
 *      bcm_dnx_time_pll_init
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */

int
bcm_dnx_time_pll_init(int unit)
{
    uint32          regval = 0;

    if (soc_is(unit, J2X_DEVICE)) {
#if defined(BCM_J2X_SUPPORT)
        regval = 0x1;
        soc_reg_field_set(unit, NSYNC_TAS_TOD_CTRLr, &regval, SOURCEf, 0x0); /*TS0*/
        soc_reg_field_set(unit, NSYNC_TAS_TOD_CTRLr, &regval, ENABLEf, 0x1); /*ENABLE*/
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TAS_TOD_CTRLr, 0, 0, regval);

        regval = 0x1000;
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_FRAC_UPPERr, 0, 0, regval);

        regval = 0;
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_FRAC_LOWERr, 0, 0, regval);

        regval = 0x1000;
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_FRAC_UPPERr, 0, 0, regval);

        regval = 0;
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_FRAC_LOWERr, 0, 0, regval);

        regval = 0;
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_FREQ_CONTROL_LOWERr, 0, 0, regval);

        regval = dnx_data_oam.general.oam_ns_freq_control_upper_get(unit);
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_FREQ_CONTROL_UPPERr, 0, 0, regval);

        regval = 0;
        soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &regval, COUNTER_ENABLEf, 0x7); /*Enable all three counters at once*/
        soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &regval, LOAD_ENABLEf, 0x3fff); /*TS counter Accum and offset are not done*/
        soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &regval, LOAD_TYPEf, 0x0); /*Independent mode used*/
        soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &regval, PTP_FC_TS_1f, 0x1); /*PTP ToD uses TS1 nco*/
        BCM_TIME_WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, regval);
#endif

    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
        READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 0);
        WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

        READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &regval, ENABLEf, 0);
        WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, regval);

        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

        READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval, ENABLE_COMMON_CONTROLf, 1);
        WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);

        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

        READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &regval, ACC2f, 0);
        WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, regval);

        READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &regval, ACC1f, 0);
        WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, regval);

        READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &regval, ACC0f, 0);
        WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, regval);

        READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
        WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, regval);

        READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

        READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, regval);

    } else {
    /* Intention is to enable TS0/TS1 in combined mode in system startup */
    /* DNX driver already starts the TS1 counters . so slighly diffrent
     * sequence to reinitailize both the counters in combined mode
     */
    /* disable both TS counters */
    READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 0);
    WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

    READ_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS1_COUNTER_ENABLEr, &regval, ENABLEf, 0);
    WRITE_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, regval);

    /* 250 Mhz - 4.000 ns */
    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf,
                    ((dnx_data_pll.general.ts_ptp_fref_get(unit)
                    == 12800000 ) ? 0x40842108 : 0x40000000));
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, regval);

    READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr(unit, regval);

    READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr(unit, regval);


     /* Enable common control for both counters */
    READ_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval,
                      ENABLE_COMMON_CONTROLf, 1);
    WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);

    /* Re-enable both the counters */
    READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
    WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

    READ_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS1_COUNTER_ENABLEr, &regval, ENABLEf, 1);
    WRITE_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, regval);
    }

    return _SHR_E_NONE;
}

/*
 * Function:
 *      bcm_dnx_time_init
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_init (int unit)
{
    _bcm_time_config_p      time_cfg_ptr = NULL;   /* Pointer to Time module config */
    bcm_time_interface_t    *intf;          /* Time interfaces iterator.     */
    int                     alloc_sz;       /* Memory allocation size.       */
    int                     idx;            /* Time interface array iterator */
    int                     rv;             /* Return Value                  */
    int num_time_interface;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /* Check if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    {
        int *num_time_interface_p;
        SHR_IF_ERR_EXIT(
            soc_info_int_address_get(unit,NUM_TIME_INTERFACE_INT,&num_time_interface_p));
        num_time_interface = *num_time_interface_p;
        LOG_DEBUG_EX(BSL_LOG_MODULE,"num_time_interface %d - num_time_interface_p 0x%08lX %s%s\n",
                num_time_interface, (unsigned long)num_time_interface_p, EMPTY, EMPTY);
    }

    /* skip the simulation env */
    if (SAL_BOOT_SIMULATION || SAL_BOOT_PLISIM ||
        soc_sand_is_emulation_system(unit)) {
        return (_SHR_E_NONE);
    }

    /* If already initialized then deinitialize time module */
    if (TIME_INIT(unit)) {
        _bcm_dnx_time_deinit(unit, &TIME_CONFIG(unit));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!sw_state_is_warm_boot(unit)) {
        /* Software state init */
        rv = dnx_time_interface_db.init(unit);
        if(BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif

    /* Allocate time config structure. */
    alloc_sz = sizeof(_bcm_time_config_t);
    SHR_ALLOC_SET_ZERO_ERR_EXIT(time_cfg_ptr, alloc_sz, "Time module", "%s%s%s",EMPTY,EMPTY,EMPTY) ;
    /* Currently only one interface per unit */
    time_cfg_ptr->intf_count = num_time_interface;

    /* Allocate memory for all time interfaces, supported */
    alloc_sz = time_cfg_ptr->intf_count * sizeof(_bcm_time_interface_config_t);
    SHR_ALLOC_SET_ZERO(time_cfg_ptr->intf_arr,alloc_sz,"Time Interfaces","%s%s%s",EMPTY,EMPTY,EMPTY);
    if (NULL == time_cfg_ptr->intf_arr) {
        _bcm_dnx_time_deinit(unit, &time_cfg_ptr);
        return (_SHR_E_MEMORY);
    }
    for (idx = 0; idx < time_cfg_ptr->intf_count; idx++) {
        intf = &time_cfg_ptr->intf_arr[idx].time_interface;
        intf->id = idx;
        time_cfg_ptr->intf_arr[idx].user_cb = NULL;
    }

    /* For each time interface allocate memory for tuser_cb */
    alloc_sz = sizeof(_bcm_time_user_cb_t);
    for (idx = 0; idx < num_time_interface; idx++) {
        SHR_ALLOC_SET_ZERO(time_cfg_ptr->intf_arr[idx].user_cb,alloc_sz,
                            "Time Interface User Callback","%s%s%s",EMPTY,EMPTY,EMPTY);
        if (NULL == time_cfg_ptr->intf_arr[idx].user_cb) {
            _bcm_dnx_time_deinit(unit,  &time_cfg_ptr);
            return (_SHR_E_MEMORY);
        }
#ifdef INCLUDE_GDPLL
        time_cfg_ptr->intf_arr[idx].dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;
        time_cfg_ptr->intf_arr[idx].bs_thread = SAL_THREAD_ERROR;
#endif /* INCLUDE_GDPLL */
    }
    {
        int *int_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* Interrupt handling function initialization */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&int_p) );
        *int_p = 0;
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );
        *soc_time_callout_p = NULL;

    }
    /* Create protection mutex. */
    time_cfg_ptr->mutex = sal_mutex_create("Time mutex");
    if (NULL == time_cfg_ptr->mutex) {
        _bcm_dnx_time_deinit(unit, &time_cfg_ptr);
        return (_SHR_E_MEMORY);
    }

    sal_mutex_take(time_cfg_ptr->mutex, sal_mutex_FOREVER);

    TIME_CONFIG(unit) = time_cfg_ptr;
#ifdef INCLUDE_GDPLL
    TIME_DPLL_CHAN_NTP(unit) = BCM_TIME_DPLL_CHAN_INVALID;
    TIME_DPLL_CHAN_PTP(unit) = BCM_TIME_DPLL_CHAN_INVALID;
#endif /* INCLUDE_GDPLL */

    /* Clear memories/registers on Cold boot only */
    if (!sw_state_is_warm_boot(unit)) {
        for (idx = 0; idx < num_time_interface; idx++) {
            rv  = _bcm_dnx_time_hw_clear(unit, idx);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                _bcm_dnx_time_deinit(unit, &time_cfg_ptr);
                TIME_CONFIG(unit) = NULL;
                return (_SHR_E_MEMORY);
            }
        }
    } else {
        /* If Warm boot reinitialize module based on HW state */
#ifdef BCM_WARM_BOOT_SUPPORT
        for (idx = 0; idx < num_time_interface; idx++) {
            /* accessing CMIC_BS/CMIC_TS when pll is off is not allowed, so encaplusating this whit a soc property,
               otherwize it bcm.user crash on init (crash happen after rebotting Linux) */
            if (1 == dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock)) {
                rv = _bcm_dnx_time_reinit(unit, idx);
            } else {
                rv = _SHR_E_NONE;
            }
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                _bcm_dnx_time_deinit(unit, &time_cfg_ptr);
                TIME_CONFIG(unit) = NULL;
                return (rv);
            }
        }

#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    TIME_UNLOCK(unit);

    if (!sw_state_is_warm_boot(unit)) {
        if (soc_is(unit, DNX_DEVICE)) {
            bcm_dnx_time_pll_init(unit);
        }
    }

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
    bcm_dnx_time_capture_ts_init(unit);
#endif

/* Event ID programming is not working for Q2A */
    if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE))
    {
        uint32 regVal = 0;
        uint32 eventId;

        /*
         * Set the event ids for Nanosync, both miscellaneous and port events
         * i.e, NS_TIMESYNC_TS_EVENT_FWD_CFG - 150 Numels
         *      NS_TIMESYNC_MAPPER_FWD_CFG - 128 Numels
         *
         * EventId 0 to 191 for port events for 64-ports
         * Events could be RXCDR/RXSOF, ROE, TXSOF/TXPI
         * 0, 2, 4...126 -> RXCDR/RXSOF
         * 1, 3, 5...127 -> ROE
         * 128, 129, 130..191 -> TXSOF/TXPI
         */

        /*
         * Set the Miscellaneous event id's
         */
        eventId = 193; /*BS_HB0 event*/
        READ_NS_TIMESYNC_TS_EVENT_FWD_CFG129r(unit, &regVal);
        soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFG129r, &regVal, EVENT_ID_Nf, eventId);
        WRITE_NS_TIMESYNC_TS_EVENT_FWD_CFG129r(unit, regVal);

        eventId = 195; /*GPIO1 event*/
        READ_NS_TIMESYNC_TS_EVENT_FWD_CFG132r(unit, &regVal);
        soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFG132r, &regVal, EVENT_ID_Nf, eventId);
        WRITE_NS_TIMESYNC_TS_EVENT_FWD_CFG132r(unit, regVal);

        eventId = 201; /*IP_DM0 event*/
        READ_NS_TIMESYNC_TS_EVENT_FWD_CFG137r(unit, &regVal);
        soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFG137r, &regVal, EVENT_ID_Nf, eventId);
        WRITE_NS_TIMESYNC_TS_EVENT_FWD_CFG137r(unit, regVal);

        eventId = 202; /*IP_DM1 event*/
        READ_NS_TIMESYNC_TS_EVENT_FWD_CFG138r(unit, &regVal);
        soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFG138r, &regVal, EVENT_ID_Nf, eventId);
        WRITE_NS_TIMESYNC_TS_EVENT_FWD_CFG138r(unit, regVal);


    }
exit:
    if (SHR_FUNC_VAL_IS(_SHR_E_MEMORY)) {
        _bcm_dnx_time_deinit(unit, &time_cfg_ptr);
        return (_SHR_E_MEMORY);
    }
    SHR_FUNC_EXIT;
}


/*
 * FUNCTIONS:
 *      bcm_dnx_time_deinit
 * Purpose:
 *      Uninitialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_deinit (int unit)
{
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Check if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* this unit not init, no nothing to detach; done. */
    if (0 == TIME_INIT(unit)) {
        return (_SHR_E_NONE);
    }

    return _bcm_dnx_time_deinit(unit, &TIME_CONFIG(unit));
}


/*
 * Function:
 *      bcm_dnx_time_interface_add
 * Purpose:
 *      Adding a time sync interface to a specified unit
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_interface_add (int unit, bcm_time_interface_t *intf)
{
    int             rv;     /* Return value */
    int replacing = 0;
    uint32          regval = 0;

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Check if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "feature unsupport\n")));
        return (_SHR_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(
                        _bcm_dnx_time_interface_input_validate(unit, intf));

    if (TIME_CONFIG(unit) == NULL) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "input validate fail\n")));
        return (_SHR_E_INIT);
    }

    if (1 != dnx_data_pll.general.feature_get(unit, dnx_data_pll_general_ts_freq_lock)) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "Time interface add invoked before configuring TS PLL\n")));
        return (BCM_E_CONFIG);
    }

    TIME_LOCK(unit);
    if (intf->flags & BCM_TIME_WITH_ID) {
        /* If interface already been in use */
        if (0 != TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id)) {
            if (0 == (intf->flags & BCM_TIME_REPLACE)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "interface already been in use\n")));
                return _SHR_E_EXISTS;
            }
            replacing = 1;
        } else {
            TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) = 1;
        }
    } else {
        rv = _bcm_dnx_time_interface_allocate_id(unit, &(intf->id));
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "input interface allocate fail\n")));
            return rv;
        }
    }
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        int reset_hardware = 0;
    
        /* Set time interface configuration. */
        if (replacing) {
            if (((TIME_INTERFACE(unit, intf->id))->flags &
                 (BCM_TIME_ENABLE | BCM_TIME_INPUT)) !=
                (intf->flags & (BCM_TIME_ENABLE | BCM_TIME_INPUT))) {
                /* either master or enabled state changed, so copy those bits in flags */
                (TIME_INTERFACE(unit, intf->id))->flags &= ~(BCM_TIME_ENABLE | BCM_TIME_INPUT);
                (TIME_INTERFACE(unit, intf->id))->flags |=
                    intf->flags & (BCM_TIME_ENABLE | BCM_TIME_INPUT);
                /* either master or enabled state changed, so re-init */
                reset_hardware = 1;
            }
            if (intf->flags & BCM_TIME_DRIFT) {
                (TIME_INTERFACE(unit, intf->id))->drift = intf->drift;
            }
            if (intf->flags & BCM_TIME_OFFSET) {
                (TIME_INTERFACE(unit, intf->id))->offset = intf->offset;
            }
            if (intf->flags & BCM_TIME_ACCURACY) {
                (TIME_INTERFACE(unit, intf->id))->accuracy = intf->accuracy;
            }
            if (intf->flags & BCM_TIME_HEARTBEAT) {
                if ((TIME_INTERFACE(unit, intf->id))->heartbeat_hz != intf->heartbeat_hz) {
                    (TIME_INTERFACE(unit, intf->id))->heartbeat_hz = intf->heartbeat_hz;
                    /* Also use heartbeat flag to indicate whether to replace bitclock freq */
                    (TIME_INTERFACE(unit, intf->id))->bitclock_hz = intf->bitclock_hz;
                    reset_hardware = 1;
                }
            }
            if (intf->flags & BCM_TIME_NTP_OFFSET) {
                (TIME_INTERFACE(unit, intf->id))->ntp_offset = intf->ntp_offset;
            }

            if(BS_UC_FEATURE_CHECK(BS_NTP_PTP_TOD_OFFSET_CONFIG)) {
                if (intf->flags & BCM_TIME_PTP_OFFSET) {
                    (TIME_INTERFACE(unit, intf->id))->ptp_offset = intf->ptp_offset;
                }
            }
        } else {
            /* Is new, so just copy wholesale */
            *(TIME_INTERFACE(unit, intf->id)) = *intf;
            reset_hardware = 1;
        }

        if (reset_hardware) {
#ifdef EDKHLIB
            _edk_init(unit, 1/*edk_bs_core_num*/);
#endif

            /* Enable/Setup TIME API BroadSync on the unit */
            rv = _bcm_dnx_time_bs_init(unit, TIME_INTERFACE(unit, intf->id));
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs init fail\n")));
                return rv;
            }

#ifdef INCLUDE_GDPLL
            /* BS-Interface is setup. Create the required channels */
            _bcm_bs_dpll_chan_create(unit, TIME_INTERFACE(unit, intf->id));
#endif
        }

        if (!replacing) {
#ifdef EDKHLIB
            /*### For the case of EDKHLIB, any init of EDK-FW is not required ? */
#else
            /* Enable BS firmware on the unit */
            rv = _bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "mbox init fail\n")));
                return rv;
            }
#endif

            if (BS_UC_FEATURE_CHECK(BS_NTP_PTP_TOD_DISABLE)) {
                rv = _bcm_time_bs_ntp_tod_disable(unit,
                        (intf->flags & BCM_TIME_BS_NTP_TOD_UPDATE_DISABLE ? 1:0));
                if (BCM_FAILURE(rv)) {
                    TIME_UNLOCK(unit);
                    LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs ntp tod disable fail\n")));
                    return rv;
                }

                rv = _bcm_time_bs_ptp_tod_disable(unit,
                        (intf->flags & BCM_TIME_BS_PTP_TOD_UPDATE_DISABLE ? 1:0));
                if (BCM_FAILURE(rv)) {
                    TIME_UNLOCK(unit);
                    LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs ptp tod disable fail\n")));
                    return rv;
                }
            }

#ifdef BCM_WARM_BOOT_SUPPORT
            _bcm_dnx_time_swstate_store(unit, intf->id, TIME_INTERFACE(unit, intf->id));
#endif
        }

    /* Set BroadSync time */
        if (intf->flags & BCM_TIME_BS_TIME) {
#ifdef EDKHLIB
            uint8 buff_in[100];
            int32_t reply;
            uint8 *buf_in = buff_in;

            _SHR_PACK_U32(buf_in, intf->id);
            _SHR_PACK_U64(buf_in, intf->bs_time.seconds);
            _SHR_PACK_U32(buf_in, intf->bs_time.nanoseconds);
            rv = edk_rpc_call_sync(edk_bs_handle, "uc_bs_time_set",
                    &buff_in, 16,
                    NULL, 0,
                    &reply, MCS_BS_MAX_EDK_MSG_TIMEOUT);
            if ((rv != EDK_ERR_NONE) || (reply != 0)) {
                cli_out("### uc_bs_time_set() failed: edk_ret:%d, fw_ret:%d\n", rv, reply);
            }
#else
            rv = _SHR_E_NONE;
#endif
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

        /* Set specified offsets */
        if (intf->flags & BCM_TIME_DRIFT) {
            rv = _bcm_time_bs_frequency_offset_set(unit, intf->drift);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs frequency offset set fail\n")));
                return rv;
            }
        }
#if defined(BCM_J2X_SUPPORT)
        if (intf->flags & BCM_TIME_NTP_OFFSET) {
            gdpll_chan_NTPoffset = intf->ntp_offset;
        }
#endif

        if (intf->flags & BCM_TIME_OFFSET) {
#ifdef EDKHLIB
            uint8 buff[100];
            int32_t reply;
            int ret = EDK_ERR_NONE;
            uint8 *buf = buff;

            _SHR_PACK_U32(buf, intf->id);
            _SHR_PACK_U32(buf, intf->offset.isnegative);  /* is_negative */
            _SHR_PACK_U64(buf, intf->offset.seconds);     /* phase_sec */
            _SHR_PACK_U32(buf, intf->offset.nanoseconds); /* phase_ns */
            ret = edk_rpc_call_sync(edk_bs_handle, "uc_bs_phase_adj_set",
                                    buff, 20,
                                    NULL, 0,
                                    &reply, MCS_BS_MAX_EDK_MSG_TIMEOUT);
            if ((ret != EDK_ERR_NONE) || (reply != 0)) {
                cli_out("### uc_bs_phase_adj_set() failed: edk_ret:%d, fw_ret:%d\n", ret, reply);
            }

#ifdef INCLUDE_GDPLL
            rv = _bcm_bs_dpll_chan_recreate(unit, intf->id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in _bcm_bs_dpll_chan_recreate ### \n")));
            }
#endif /* INCLUDE_GDPLL */
#else
            rv = _bcm_time_bs_phase_offset_set(unit, intf->offset);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs phase offset set fail\n")));
                return rv;
            }
#endif
        }

        if (intf->flags & BCM_TIME_NTP_OFFSET) {
            rv = _bcm_time_bs_ntp_offset_set(unit, intf->ntp_offset);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs ntp offset set fail\n")));
                return rv;
            }
        }

        if(BS_UC_FEATURE_CHECK(BS_NTP_PTP_TOD_OFFSET_CONFIG)) {
            if (intf->flags & BCM_TIME_PTP_OFFSET) {
                rv = _bcm_time_bs_ptp_offset_set(unit, intf->ptp_offset);
                if (BCM_FAILURE(rv)) {
                    TIME_UNLOCK(unit);
                    LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "bs ptp offset set fail\n")));
                    return rv;
                }
            }
        }

        TIME_UNLOCK(unit);
        return (_SHR_E_NONE);
    }

    if (soc_is(unit, J2X_DEVICE)) {
        /* TS counter init is already done in time_init() */
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
         /* Enable common control for both counters */
        READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval, ENABLE_COMMON_CONTROLf, 1);
        WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);
        
        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);
        
        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

        READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
    }else if (soc_is(unit, DNX_DEVICE)) {
         /* Enable common control for both counters */
        READ_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval, 
                          ENABLE_COMMON_CONTROLf, 1);
        WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);
        
        /* Enable Timestamp Generation */
        READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
        /* 250 Mhz - 4.000 ns */
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf,
                        ((dnx_data_pll.general.ts_ptp_fref_get(unit)
                        == 12800000 ) ? 0x40842108 : 0x40000000));
        WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
        WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        /* Enable Timestamp Generation */
        READ_CMIC_TS_FREQ_CTRL_LOWERr(unit, &regval);
        /* 250 Mhz - 4.000 ns */
        soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_LOWERr, &regval, FRACf, 4);
        WRITE_CMIC_TS_FREQ_CTRL_LOWERr(unit, regval);

        READ_CMIC_TS_FREQ_CTRL_UPPERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, NSf, 0);
        soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, ENABLEf, 1);
        WRITE_CMIC_TS_FREQ_CTRL_UPPERr(unit, regval);
    }
#endif

    if (intf->flags & BCM_TIME_REF_CLOCK) {
        rv = _bcm_dnx_time_interface_ref_clock_install(unit, intf->id);
        if (BCM_FAILURE(rv)) {
            TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) -= 1;
            TIME_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "interface ref clock install fail\n")));
            return rv;
        }
    }

    /* Set time interface configuration. */
    *(TIME_INTERFACE(unit, intf->id)) = *intf;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dnx_time_swstate_store(unit, intf->id, TIME_INTERFACE(unit, intf->id));
#endif

    rv = _bcm_dnx_time_interface_install(unit, intf->id);

    if (soc_is(unit, J2X_DEVICE)) {
        /* No action required ? */
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE))
    {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);

        if ((intf->flags & BCM_TIME_LOCKED))
        {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        }
        else
        {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }

        if (intf->flags & BCM_TIME_INPUT)
        {
            /*
             * Set Broadsync in Input mode
             */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);

            /*
             * ensure all output controls are disabled in input mode
             */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
        }
        else
        {
            /*
             * Configure Accuracy, offset and drift for broadsync output mode
             */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);

            if ((intf->flags & BCM_TIME_ENABLE))
            {
                /*
                 * Enable all three broadsync pins: Bit clock, HeartBeat and Timecode
                 */
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
            }
            else
            {
                /*
                 * ensure all output controls are disabled
                 */
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
            }
        }

        WRITE_NS_BS0_BS_CONFIGr(unit, regval);
    }

    if (BCM_FAILURE(rv)) {
        TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) -= 1;
        TIME_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "time interface install fail\n")));
        return rv;
    }

    TIME_UNLOCK(unit);

    return (_SHR_E_NONE);
}


/*
 * Function:
 *      bcm_dnx_time_interface_delete
 * Purpose:
 *      Deleting a time sync interface from unit
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      intf_id - (IN) Time Sync Interface id to remove
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_interface_delete (int unit, bcm_time_if_t intf_id)
{
    int rv;

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, intf_id));

    TIME_LOCK(unit);

    /* If interface still in use return an Error */
    if (1 < TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf_id)) {
        TIME_UNLOCK(unit);
        LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit, "interface still in use\n")));
        return (_SHR_E_BUSY);
    }

#ifdef INCLUDE_GDPLL
    rv = _bcm_bs_dpll_chan_destroy(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
               (BSL_META_U(0, "### Error in destroy dpll chan ### \n")));
    }
#endif

    /* Free the interface */
    rv = _bcm_dnx_time_interface_free(unit, intf_id); 
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "time interface free fail\n")));
        return (rv);
    }

    rv = _bcm_dnx_time_hw_clear(unit, intf_id);

    TIME_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_dnx_time_interface_get
 * Purpose:
 *      Get a time sync interface on a specified unit
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_interface_get (int unit, bcm_time_interface_t *intf)
{
    int rv;
#ifdef INCLUDE_GDPLL
    int dpll_chan;
    uint32 dpll_state;
#endif

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Check if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "feature unsupport\n")));
        return (_SHR_E_UNAVAIL);
    }

    /* Validation checks */
    if (NULL == intf) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "validation fail\n")));
        return _SHR_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, intf->id));

    TIME_LOCK(unit);
    rv = _bcm_dnx_time_interface_get(unit, intf->id, intf);

    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        if(SOC_IS_J2X(unit)) {
#ifdef INCLUDE_GDPLL
            dpll_chan = TIME_INTERFACE_CONFIG(unit, intf->id).dpll_chan;
            if (IS_TIME_DPLL_CHAN_VALID(unit, intf->id)) {
                BCM_IF_ERROR_RETURN(bcm_gdpll_chan_state_get(unit, dpll_chan, &dpll_state));
                if (dpll_state & 0x1) {
                    intf->status = bcmBsTimeSourceLocked;
                } else {
                    intf->status = bcmBsTimeSourceNotLocked;
                }
            }
            LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                    " intf->status[%d] dpll_state[0x%08x] \n"), intf->status, dpll_state));
#endif /* INCLUDE_GDPLL */
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_time_bs_status_get(unit,
            (int *)(&(intf->status))));
    }
    TIME_UNLOCK(unit);

    return (rv);
}


/*
 * Function:
 *      bcm_dnx_time_interface_delete_all
 * Purpose:
 *      Deleting all time sync interfaces on a unit
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int 
bcm_dnx_time_interface_delete_all (int unit)
{
    bcm_time_if_t   intf;
    int num_time_interface;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    {
        int *num_time_interface_p;
        SHR_IF_ERR_EXIT(
            soc_info_int_address_get(unit,NUM_TIME_INTERFACE_INT,&num_time_interface_p));
        num_time_interface = *num_time_interface_p;
    }
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "feature unsupport\n")));
        return (_SHR_E_UNAVAIL);
    }

    /* Initialization check */
    if (0 == TIME_INIT(unit)) {
        LOG_ERROR(BSL_LS_BCM_PTP, (BSL_META_U(unit, "un-init\n")));
        return _SHR_E_INIT;
    }

    for (intf = 0; intf < num_time_interface; intf++ ) {
        BCM_IF_ERROR_RETURN(
            bcm_dnx_time_interface_delete(unit, intf));
    }
exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnx_time_interface_traverse
 * Purpose:
 *      Itterates over all time sync interfaces and calls given callback
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      cb          - (IN) Call back function
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int 
bcm_dnx_time_interface_traverse (int unit, bcm_time_interface_traverse_cb cb, 
                                 void *user_data)
{
    bcm_time_if_t   intf;
    int num_time_interface;
    int rv = BCM_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    {
        int *num_time_interface_p;
        SHR_IF_ERR_EXIT(
            soc_info_int_address_get(unit,NUM_TIME_INTERFACE_INT,&num_time_interface_p));
        num_time_interface = *num_time_interface_p;
    }
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Initialization check. */
    if (0 == TIME_INIT(unit)) {
        return _SHR_E_INIT;
    }
    
    if (NULL == cb) {
        return (_SHR_E_PARAM);
    }

    for (intf = 0; intf < num_time_interface; intf++ ) {
        if (NULL != TIME_INTERFACE(unit, intf)) {
            rv = cb(unit, TIME_INTERFACE(unit, intf), user_data);
            if (rv == BCM_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                        "traverse found i/f idx[%d] \n"), intf ));
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnx_time_capture_get
 * Purpose:
 *      Gets a time captured by HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time)
{
    int rv;   /* Operation return status. */

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, id));

    if (NULL == time) {
        return (_SHR_E_PARAM);
    }
    if (NULL == TIME_CAPTURE(unit, id)) {
        return (_SHR_E_NOT_FOUND);
    }

    TIME_LOCK(unit);
    rv = _bcm_dnx_time_capture_get(unit, id, TIME_CAPTURE(unit, id), time->flags);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }

    *time = *(TIME_CAPTURE(unit, id));

    TIME_UNLOCK(unit);

    return (_SHR_E_NONE);
}


/*
 * Function:
 *      bcm_dnx_time_heartbeat_enable_set
 * Purpose:
 *      Enables/Disables interrupt handling for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (IN) Enable/Disable parameter
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_heartbeat_enable_set (int unit, bcm_time_if_t id, int enable)
{
    uint32          regval;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Check if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

    if ((TIME_INTERFACE(unit, id))->flags & BCM_TIME_SYNC_STAMPER) {
        int rv = _bcm_time_bs_debug_1pps_set(unit, enable);
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            return rv;
        }

        TIME_UNLOCK(unit);
        return _SHR_E_NONE;
    }

    if (soc_is(unit, J2X_DEVICE)) {
        /* ### This is not required for J2X as its done in EDK-FW ? */
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
        READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf,
                          (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);
        WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

        if (enable)
        {
            /*
             * Configure the HW to raise interrupt on every heartbeat
             */
            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, 1);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 1);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 1);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
        }
        else
        {
            /*
             * Leave TIME_CAPTURE_ENABLEf for other trigger sources
             */
            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 0);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
        }
    }
    else if (soc_is(unit, DNX_DEVICE))
    {
        READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, &regval, INT_ENABLEf,
            (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);

        if (enable) {
            /* Configure the HW to raise interrupt on every heartbeat */
            READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, TIME_CAPTURE_ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 1);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
        } else {
            /* Leave TIME_CAPTURE_ENABLEf for other trigger sources */
            READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 0);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
        }

        /* Configure the HW to capture time on every heartbeat */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, TIME_CAPTURE_MODEf,
            (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);

        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);

        /* set interrupt enables to match the mode */
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        /* Configure the HW to raise interrupt on every heartbeat */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, INT_ENABLEf, 
                          (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : 0);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TIME_CAPTURE_ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TX_HB_STATUS_ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, RX_HB_STATUS_ENABLEf, 1);

        /* Configure the HW to capture time on every heartbeat */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                          (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
    }
#endif

    TIME_UNLOCK(unit);
    {
        /*
         * Enable/disable broadsync interrupt
         */
        int *time_call_ref_count_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* Interrupt handling function initialization */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&time_call_ref_count_p) );
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );

        if (enable) {
            dnx_drv_soc_intr_enable(unit, IRQ_BROADSYNC_INTR);
            if (!(*time_call_ref_count_p)) {
                *soc_time_callout_p = _bcm_dnx_time_hw_interrupt_dflt;
            }  
        } else {
            dnx_drv_soc_intr_disable(unit, IRQ_BROADSYNC_INTR);
            if (!(*time_call_ref_count_p)) {
                *soc_time_callout_p = NULL;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *      bcm_dnx_time_heartbeat_enable_get
 * Purpose:
 *      Gets interrupt handling status for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (OUT) Enable status of interrupt handling
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_heartbeat_enable_get (int unit, bcm_time_if_t id, int *enable)
{
    uint32      regval = 0;

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }
    
    /* Param validation check. */
    if (enable == NULL) {
        return (_SHR_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(
                        _bcm_dnx_time_interface_id_validate(unit, id));

    if ((TIME_INTERFACE(unit, id))->flags & BCM_TIME_SYNC_STAMPER) {
        /* Check if 1PPS will be enabled on the GPIO1 */
        /* Read HW Configuration */
        if (soc_is(unit, J2X_DEVICE)) {
            cli_out("### heartbeat_enable_get: not required for J2X\n");
        } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
            READ_NS_TIMESYNC_GPIO_1_OUTPUT_ENABLEr(unit, &regval);
            *enable = soc_reg_field_get(unit, NS_TIMESYNC_GPIO_1_OUTPUT_ENABLEr, regval, OUTPUT_ENABLEf);
        } else if (soc_is(unit, DNX_DEVICE)) {
            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
            *enable = (regval & 0x10)? 1 : 0;
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        else {
            READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
            *enable = soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                                regval, TIME_CAPTURE_MODEf);
            *enable = (*enable & TIME_CAPTURE_MODE_GPIO_1)? 1 : 0;
        }
#endif
        goto exit;
    }

    /* Read HW Configuration */
    if (soc_is(unit, J2X_DEVICE)) {
        cli_out("### time_heartbeat_enable_get: Implement for J2X ?\n");
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)) {
        *enable = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, ENABLEf) &
                soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, BS0_TX_HB_STATUS_ENABLEf) &
                soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, BS0_RX_HB_STATUS_ENABLEf);
    } else if (soc_is(unit, DNX_DEVICE)) {
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        *enable = (regval & TIME_CAPTURE_MODE_HEARTBEAT)? 1 : 0;
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    else {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        *enable = soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                                regval, INT_ENABLEf);
    }
#endif
    *enable = (*enable ==  TIME_CAPTURE_MODE_HEARTBEAT) ? 1 : 0; 

exit:
    return (_SHR_E_NONE);
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * Function:
 *      _bcm_dnx_time_trigger_to_timestamp_mode
 * Purpose: 
 *      Convert capture mode flags to time control capture modes
 *
 * Parameters: 
 *      unit       - (IN) mode_flags
 *      
 * Returns:
 *      Timesync captude modes 
 * Notes:
 */
static uint32
_bcm_dnx_time_trigger_to_timestamp_mode(int unit, uint32 mode_flags)
{
    uint32 capture_mode = 0;
    uint32 regval;
    int    bit_cnt;

    /* processing on user mode flags reduces the dependencies on 
     * hardware bit maps, byte shift of mode flags
     * should have been efficient 
     */
    for (bit_cnt = 0; bit_cnt < sizeof(uint32); bit_cnt++)
    {
        switch (mode_flags & (1 << bit_cnt))
        {
        case BCM_TIME_CAPTURE_GPIO_0:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_0;
            READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_GPIO_1:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_1;
            READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_GPIO_2:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_2;
            READ_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_2_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_GPIO_3:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_3;
            READ_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_3_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_GPIO_4:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_4;
            READ_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_4_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_GPIO_5:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_5;
            READ_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, regval);
            READ_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_5_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_L1_CLOCK_PRIMARY:
            capture_mode |=  TIME_CAPTURE_MODE_L1_CLOCK_PRIMARY;
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_L1_CLOCK_SECONDARY:
            capture_mode |=  TIME_CAPTURE_MODE_L1_CLOCK_SECONDARY;
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_LCPLL:
            capture_mode |= TIME_CAPTURE_MODE_LCPLL;
            READ_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_LCPLL_CLK_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_IP_DM_0:
            capture_mode |= TIME_CAPTURE_MODE_IP_DM_0;
            break;

        case BCM_TIME_CAPTURE_IP_DM_1:
            capture_mode |= TIME_CAPTURE_MODE_IP_DM_1;
            break;

        default:
            break;
        }
    }

    return capture_mode;
}
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * Function:
 *      _bcm_dnx_time_trigger_from_timestamp_mode
 * Purpose: 
 *      Convert Timesync capture modes to mode flags 
 *
 * Parameters: 
 *      unit       - (IN) capture_mode
 *      
 * Returns:
 *      Capture mode flags
 * Notes:
 */

static uint32
_bcm_dnx_time_trigger_from_timestamp_mode (uint32 capture_mode)
{
    uint32 user_flags = 0;
    int    bit_cnt;

    /* processing on user mode flags reduces the dependencies on 
     * hardware bit maps, byte shift of mode flags
     * should have been efficient 
     */
    for (bit_cnt = 0; bit_cnt < sizeof(uint32); bit_cnt++)
    {
        switch (capture_mode & (1 << bit_cnt))
        {
        case TIME_CAPTURE_MODE_GPIO_0:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_0;
            break;
        case TIME_CAPTURE_MODE_GPIO_1:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_1;
            break;
        case TIME_CAPTURE_MODE_GPIO_2:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_2;
            break;
        case TIME_CAPTURE_MODE_GPIO_3:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_3;
            break;
        case TIME_CAPTURE_MODE_GPIO_4:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_4;
            break;
        case TIME_CAPTURE_MODE_GPIO_5:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_5;
            break;
        case TIME_CAPTURE_MODE_L1_CLOCK_PRIMARY:
            user_flags |=  BCM_TIME_CAPTURE_L1_CLOCK_PRIMARY;
            break;
        case TIME_CAPTURE_MODE_L1_CLOCK_SECONDARY:
            user_flags |=  BCM_TIME_CAPTURE_L1_CLOCK_SECONDARY;
            break;
        case TIME_CAPTURE_MODE_LCPLL:
            user_flags |=  BCM_TIME_CAPTURE_LCPLL;
            break;
        case TIME_CAPTURE_MODE_IP_DM_0:
            user_flags |=  BCM_TIME_CAPTURE_IP_DM_0;
            break;
        case TIME_CAPTURE_MODE_IP_DM_1:
            user_flags |= BCM_TIME_CAPTURE_IP_DM_1;
            break;
        default:
            break;
        }
    }

    return user_flags;
}
#endif


/*
 * Function:
 *      bcm_dnx_time_trigger_enable_set
 * Purpose:
 *      Enables/Disables interrupt handling for external triggers
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 *      mode_flags  - (IN) Enable/Disable parameter
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_trigger_enable_set (int unit, bcm_time_if_t id, uint32 mode_flags)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    uint32          regval;
#endif
    uint32 capture_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Check if time feature is supported on a device */
    if (!(dnx_drv_soc_feature(unit, soc_feature_time_support))) {
        return (_SHR_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
                        _bcm_dnx_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    /* 
     *   Read time capture control capture modes and 
     *   Convert to hardware cature mode bitmap.
     *   Enable interrupt for all capture mode bitmap and
     *   Write time capture mode.
     *
     *   If for ALL capture mode, ignore individual capture modes and
     *   Enable Interrupts.
     */
    READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);

    if (mode_flags & BCM_TIME_CAPTURE_ALL) {
        /* Enable all Time capture mechanisms */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                          TIME_CAPTURE_ENABLEf, 1 );
        
        TIME_UNLOCK(unit);
        /* Return and  ignore setting of individual capture modes */
        return _SHR_E_NONE;
    }

    /* Set time capture modes */
    capture_mode = _bcm_dnx_time_trigger_to_timestamp_mode (unit, mode_flags);

    /* Configure the HW with capture modes */
    soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, 
                      TIME_CAPTURE_MODEf, (capture_mode) ? capture_mode : 
                      TIME_CAPTURE_MODE_DISABLE);

    /* Configure the HW to trigger interrupt for capture events */
    soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, INT_ENABLEf, 
                      (capture_mode) ? capture_mode : 0);
    
    WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
#endif

    TIME_UNLOCK(unit);
    {
        /* Enable/disable broadsync interrupt */
        int *time_call_ref_count_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* Interrupt handling function initialization */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&time_call_ref_count_p) );
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );

        if (capture_mode) {
            /* Enable Interrupt */
            dnx_drv_soc_intr_enable(unit, IRQ_BROADSYNC_INTR);
            if (!(*time_call_ref_count_p)) {
                *soc_time_callout_p = _bcm_dnx_time_hw_interrupt_dflt;
            }
       } else {
            /* Check on the references and disable interrupt and handler */
            dnx_drv_soc_intr_disable(unit, IRQ_BROADSYNC_INTR);
            if (!(*time_call_ref_count_p)) {
                *soc_time_callout_p = NULL;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnx_time_trigger_enable_get
 * Purpose:
 *      Gets interrupt handling status for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (OUT) Enable status of interrupt handling
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_trigger_enable_get (int unit, bcm_time_if_t id, uint32 *mode_flags)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    uint32      regval, capture_mode = 0;
#endif

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!(dnx_drv_soc_feature(unit, soc_feature_time_support))) {
        return (_SHR_E_UNAVAIL);
    }

    if (soc_is(unit, DNX_DEVICE)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, id));

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    /* Read HW Configuration */
    READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);

    /* Get interrupt enabled capture modes */
    capture_mode = soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr, regval, INT_ENABLEf); 

    *mode_flags = _bcm_dnx_time_trigger_from_timestamp_mode (capture_mode);
#endif
    
    return (_SHR_E_NONE);
}


/*
 * Function:
 *      bcm_dnx_time_heartbeat_register
 * Purpose:
 *      Registers a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 *      f           - (IN) Function to register
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_heartbeat_register (int unit, bcm_time_if_t id, bcm_time_heartbeat_cb f,
                                 void *user_data)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, id));
    if (NULL == f) {
        return (_SHR_E_PARAM);
    }

    TIME_LOCK(unit);
    {
        /* If HW interrupt handler been registered protect from race condition */
        int *time_call_ref_count_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* Interrupt handling function initialization */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&time_call_ref_count_p) );
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );

        if (*time_call_ref_count_p) {
            *soc_time_callout_p = _bcm_dnx_time_hw_interrupt_dflt;
        }

        /* Register user call back */
        TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = f;
        TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = user_data;

        /* After registering user cb function and user_data turn on 
        HW interrupt handler */
        *soc_time_callout_p = _bcm_dnx_time_hw_interrupt;
        /* Only single call back at a time is currently supported */
        *time_call_ref_count_p = 1; 
    }
    TIME_UNLOCK(unit);
    
exit:
    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      bcm_dnx_time_heartbeat_unregister
 * Purpose:
 *      Unregisters a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_heartbeat_unregister (int unit, bcm_time_if_t id)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_dnx_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);
    {
        int *time_call_ref_count_p;
        SOC_TIME_CALLOUT_TYPE *soc_time_callout_p;

        /* Interrupt handling function initialization */
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,TIME_CALL_REF_COUNT,(void **)&time_call_ref_count_p) );
        SHR_IF_ERR_EXIT(
            soc_control_element_address_get(unit,SOC_TIME_CALLOUT,(void **)&soc_time_callout_p) );

        if (--(*time_call_ref_count_p) <= 0) {
            *time_call_ref_count_p = 0;
            *soc_time_callout_p = _bcm_dnx_time_hw_interrupt_dflt;
        }
    }
    TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = NULL;
    TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = NULL;

    TIME_UNLOCK(unit);
exit:
    SHR_FUNC_EXIT;
}




/********************************** DNX BroadSync Time Support ***********************************/

static
int
_bcm_dnx_time_bs_init(int unit, bcm_time_interface_t *intf)
{
    const int bspll_freq = soc_is(unit, DNX_DEVICE) ? 20000000 :  25000000; /* 25 MHz */
    uint32 regval = 0;

    int master = ((intf->flags & BCM_TIME_INPUT) == 0);
    int output_enable = master && ((intf->flags & BCM_TIME_ENABLE) != 0);

    int bitclk_divisor = bspll_freq / intf->bitclock_hz;
    int bitclk_high = bitclk_divisor / 2;
    int bitclk_low = bitclk_divisor - bitclk_high;
    int hb_divisor = intf->bitclock_hz / intf->heartbeat_hz;
    int hb_up = (hb_divisor > 200) ? 100 : (hb_divisor / 2);
    int hb_down = hb_divisor - hb_up;

    uint32 freq_ctrl_lower = 0x40000000;  /* 4ns increment, for 250MHz TSPLL output */
    if ( SOC_IS_J2X(unit) ) {
        hb_down = (hb_divisor / 2);
        hb_up = hb_divisor - hb_down;
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
        (BSL_META_U(unit,
            "\n id[%d] master[%d] out_en[%d] bc[%dHz] bc_div[%d] high[%d] low[%d] hb[%dHz] hb_div[%d] up[%d] down[%d]\n"),
            intf->id, master, output_enable, intf->bitclock_hz, bitclk_divisor, bitclk_high, bitclk_low, intf->heartbeat_hz, hb_divisor, hb_up, hb_down));


    if (soc_is(unit, J2X_DEVICE)) {
#ifdef EDKHLIB
        uint8 buff[100];
        int32_t reply;
        int ret = EDK_ERR_NONE;
        uint8 *buf = buff;

        _SHR_PACK_U32(buf, intf->id);
        _SHR_PACK_U32(buf, bspll_freq);
        _SHR_PACK_U32(buf, master);
        _SHR_PACK_U32(buf, intf->bitclock_hz);
        _SHR_PACK_U32(buf, intf->heartbeat_hz);
        ret = edk_rpc_call_sync(edk_bs_handle, "uc_bs_intf_cfg",
                                buff, 20,
                                NULL, 0,
                                &reply, MCS_BS_MAX_EDK_MSG_TIMEOUT);
        if ((ret != EDK_ERR_NONE) || (reply != 0)) {
            cli_out("### uc_bs_intf_cfg() failed: edk_ret:%d, fw_ret:%d\n", ret, reply);
        }
#endif
    } else if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE))
    {
        cli_out("master: %d, output_enable: %d, bitclk_div: %d, bitclk_high: %d, bitclk_low: %d\n", master,
                output_enable, bitclk_divisor, bitclk_high, bitclk_low);
        cli_out("hb_divisor: %d, hb_up: %d, hb_down: %d, freq_ctrl_lower: %u\n", hb_divisor, hb_up, hb_down,
                freq_ctrl_lower);
        /*
         * configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION)
         */
        READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

        /*
         * configure BS0/BS1 Clock configuration registers
         */
        READ_NS_BS0_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_NS_BS0_BS_CONFIGr(unit, regval);

        READ_NS_BS1_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_NS_BS1_BS_CONFIGr(unit, regval);

        /*
         * 500 Mhz NCO - 2.000 ns
         */
        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

        READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
        WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

        READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr, &regval, FCLf, 0);
        WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, regval);

        READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr, &regval, FCUf, 0);
        WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, regval);

        READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

        /*
         * Setup BS0/BS1 HeartBeat Control registers
         */
        READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

        READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

        READ_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
        WRITE_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        READ_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
        WRITE_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        READ_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
        WRITE_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
        WRITE_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
        WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
        WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

        READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
        WRITE_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

        /*
         * configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION)
         */
        READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

        READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

        /*
         * configure BS0/BS1 Clock configuration registers
         */
        READ_NS_BS0_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_NS_BS0_BS_CONFIGr(unit, regval);

        READ_NS_BS1_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_NS_BS1_BS_CONFIGr(unit, regval);

    }
    else if (soc_is(unit, DNX_DEVICE))
    {
        /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        /* configure BS0/BS1 Clock configuration registers */
        READ_CMIC_BS0_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS0_CONFIGr(unit, regval);

        READ_CMIC_BS1_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS1_CONFIGr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf, freq_ctrl_lower);
        WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr, &regval, NSf, 0);
        WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
        WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

        READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
        WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

        /* Setup BS0/BS1 HeartBeat Control registers */
        READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval);

        READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
        WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
        WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
        WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
        WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, regval);

        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 0);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 0);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
        WRITE_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

        /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

        /* configure BS0/BS1 Clock configuration registers */
        READ_CMIC_BS0_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS0_CONFIGr(unit, regval);

        READ_CMIC_BS1_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS1_CONFIGr(unit, regval);
    }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
     else {
        /* m CMIC_BS_CLK_CTRL LOW_DURATION=1 HIGH_DURATION=1 */
        READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

        /* m CMIC_BS_CLK_CTRL ENABLE=1 */
        READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

        /* m CMIC_BS_CONFIG BS_CLK_OUTPUT_ENABLE=1 BS_HB_OUTPUT_ENABLE=1 BS_TC_OUTPUT_ENABLE=1 */
        READ_CMIC_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS_CONFIGr(unit, regval);

        /* m CMIC_TS_FREQ_CTRL_LOWER FRAC=0x40000000 */
        READ_CMIC_TS_FREQ_CTRL_LOWERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_LOWERr, &regval, FRACf, freq_ctrl_lower);
        WRITE_CMIC_TS_FREQ_CTRL_LOWERr(unit, regval);

        /* m CMIC_TS_FREQ_CTRL_UPPER ENABLE=1 */
        READ_CMIC_TS_FREQ_CTRL_UPPERr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, ENABLEf, 1);
        WRITE_CMIC_TS_FREQ_CTRL_UPPERr(unit, regval);

        /* m CMIC_BS_HEARTBEAT_CTRL ENABLE=1 */
        READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
        WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

        /* m CMIC_BS_HEARTBEAT_DOWN_DURATION DOWN_DURATION=2400 */
        READ_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
        WRITE_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

        /* m CMIC_BS_HEARTBEAT_UP_DURATION UP_DURATION=100 */
        READ_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
        WRITE_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

        /* m CMIC_BS_CLK_CTRL ENABLE=0 */
        READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 0);
        WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

        /* m CMIC_BS_REF_CLK_GEN_CTRL ENABLE=0 */
        READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
        WRITE_CMIC_BS_REF_CLK_GEN_CTRLr(unit, regval);

        /* m CMIC_BS_CLK_CTRL ENABLE=1 LOW_DURATION=1 HIGH_DURATION=1 */
        READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
        soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
        WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

        /* m CMIC_BS_CONFIG MODE=1 BS_CLK_OUTPUT_ENABLE=1 BS_HB_OUTPUT_ENABLE=1 */
        READ_CMIC_BS_CONFIGr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
        WRITE_CMIC_BS_CONFIGr(unit, regval);
    }
#endif
    return _SHR_E_NONE;
}

/*
 * Function:
 *      bcm_dnx_time_bs_log_configure_set
 * Purpose:
 *      Configure Broadsync logging
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_bs_log_configure_set (int unit, bcm_time_bs_log_cfg_t bs_log_cfg)
{
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    _bcm_time_bs_debug(unit, bs_log_cfg.debug_mask);

    return _bcm_time_bs_log_configure(unit, bs_log_cfg.debug_mask,
                    bs_log_cfg.udp_log_mask, bs_log_cfg.src_mac,
                    bs_log_cfg.dest_mac, bs_log_cfg.tpid, bs_log_cfg.vid,
                    bs_log_cfg.ttl, bs_log_cfg.src_addr, bs_log_cfg.dest_addr,
                    bs_log_cfg.udp_port);
}

/*
 * Function:
 *      bcm_dnx_time_bs_log_configure_get
 * Purpose:
 *      Configure Broadsync logging
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      _SHR_E_NONE
 *      _SHR_E_XXX
 *
 */
int
bcm_dnx_time_bs_log_configure_get (int unit, bcm_time_bs_log_cfg_t *bs_log_cfg)
{
    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (_SHR_E_UNAVAIL);
    }

    return _bcm_time_bs_log_configure_get(unit, bs_log_cfg);
}

/*
 * Function:
 *      bcm_dnx_time_bs_time_get 
 * Purpose:
 *      Get the broadsync time on the specified unit 
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      bs_time - (OUT) broadsync time in seconds and nanoseconds
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */

int
bcm_dnx_time_bs_time_get (int unit, bcm_time_spec_t *bs_time)
{
    int rv;   /* Operation return status. */
    uint8 bs_initialized=0;
#ifdef EDKHLIB
    uint8 buff_in[100];
    uint8 buff_out[100];
    int32_t reply;
    uint8 *buf_in = buff_in;
    uint8 *buf_out = buff_out;
    uint32 sec_lo, sec_hi;
#endif

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
#ifdef EDKHLIB
    _SHR_PACK_U32(buf_in, bs_initialized);
    rv = edk_rpc_call_sync(edk_bs_handle, "uc_bs_time_get",
            &buff_in, 4,
            &buff_out, 12,
            &reply, MCS_BS_MAX_EDK_MSG_TIMEOUT);

    if ((rv != EDK_ERR_NONE) || (reply != 0)) {
        cli_out("### uc_bs_time_get() failed: edk_ret:%d, fw_ret:%d\n", rv, reply);
    }

    _SHR_UNPACK_U32(buf_out, sec_hi); /* nsec */
    _SHR_UNPACK_U32(buf_out, sec_lo); /* nsec */
    bs_time->seconds = ((uint64)sec_hi << 32 | sec_lo);

    _SHR_UNPACK_U32(buf_out, bs_time->nanoseconds); /* nsec */
    return (BCM_E_NONE);
#endif

    /* Chek if time feature is supported on a device */
    if (!dnx_drv_soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    bcm_dnx_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                        (void*)&bs_initialized);
    if (!bs_initialized) {
        return BCM_E_INIT;
    }

    if (NULL == bs_time) {
        return (BCM_E_PARAM);
    }

    TIME_LOCK(unit);
    rv = _bcm_time_bs_time_get(unit, bs_time);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }
    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

int bcm_dnx_time_capture_enable_set(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_t *time_capture,
        int enable,
        int *capture_handle)
{
    int rv = BCM_E_UNAVAIL;

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    return (rv);
}

typedef struct bcm_time_cb_context_s {
    bcm_time_capture_cb cb;
    bcm_time_capture_cb_type_t cb_type;
    void    *user_data_ptr;
}bcm_time_cb_context_t;

bcm_time_cb_context_t cb_overflow,cb_fill,cb_all;

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)

typedef struct bcm_dnx_time_ts_capture_info_s {
    int time_capture_ts_enable;
    int time_capture_ts_active;
    shr_thread_t thread_id_time_capture_ts;
    uint32 ts_fifo_wr_index;
    uint32 ts_fifo_rd_index;
    uint8 ts_fifo_data_fill[BCM_TIME_TS_CAPTURE_BUF_SIZE];
} bcm_dnx_time_ts_capture_info_t;

STATIC bcm_dnx_time_ts_capture_info_t *dnx_ts_capture_info[BCM_UNITS_MAX] = {0};
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */


int bcm_dnx_time_capture_cb_register(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_cb_type_t cb_type,
        bcm_time_capture_cb cb,
        void *user_data)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
    uint32_t regVal = 0;
#endif

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    if (!dnx_drv_soc_feature(unit, soc_feature_timesync_time_capture)) {
        return BCM_E_UNAVAIL;
    }

    if (cb_overflow.cb == NULL && cb_fill.cb == NULL && cb_all.cb == NULL){
        /* At least one callback is registered. Enable the interrupt */
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
        if (soc_is(unit, J2X_DEVICE)){
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TS_INT_ENABLE_0r, 0, 0, &regVal);
            regVal |= 0x3 ;
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TS_INT_ENABLE_0r, 0, 0, regVal);
        } else if(SOC_IS_J2P(unit) || SOC_IS_J2C(unit) || SOC_IS_Q2A(unit)) {
            regVal = 0x3;
            WRITE_NS_TS_INT_ENABLEr(unit, regVal);
        } else {
            regVal = 0;
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regVal);
        }
        if (dnx_ts_capture_info[unit]) {
            dnx_ts_capture_info[unit]->ts_fifo_rd_index = 0;
            dnx_ts_capture_info[unit]->ts_fifo_wr_index = 0;
            sal_memset(dnx_ts_capture_info[unit]->ts_fifo_data_fill, 0, BCM_TIME_TS_CAPTURE_BUF_SIZE);
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, " Capture info is NULL ... Init done?\n")));
            return BCM_E_INIT;
        }
#endif
    }

    switch (cb_type){
    case bcmTimeCaptureOverflow:
        if (cb_overflow.cb != NULL){
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Callback function already registered for bcmTimeCaptureOverflow\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_overflow.cb_type = bcmTimeCaptureOverflow;
        cb_overflow.cb = cb;
        cb_overflow.user_data_ptr = user_data;
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Registered handler for bcmTimeCaptureOverflow\n")));
        break;
    case bcmTimeCapturetFill:
        if (cb_fill.cb != NULL){
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Callback function already registered for bcmTimeCapturetFill\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_fill.cb_type = bcmTimeCapturetFill;
        cb_fill.cb = cb;
        cb_fill.user_data_ptr = user_data;
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Registered handler for bcmTimeCapturetFill\n")));
        break;
    case bcmTimeCaptureAll:
        if (cb_all.cb != NULL){
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Callback function already registered for bcmTimeCaptureAll\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_all.cb_type = bcmTimeCaptureAll;
        cb_all.cb = cb;
        cb_all.user_data_ptr = user_data;
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Registered handler for bcmTimeCaptureAll\n")));
        break;
    case bcmTimeCaptureTS0CntOffsetUpdated:
    case bcmTimeCaptureTS1CntOffsetUpdated:
        rv = BCM_E_PARAM;
        goto exit;
        break;
    }
    /* Enable required interrupts from external means */
exit:
    return rv;
}


int bcm_dnx_time_capture_cb_unregister(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_cb_type_t cb_type)
{
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
    uint32_t regVal = 0;
#endif

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    if (!dnx_drv_soc_feature(unit, soc_feature_timesync_time_capture)) {
        return BCM_E_UNAVAIL;
    }
    switch (cb_type){
    case bcmTimeCaptureOverflow:
        cb_overflow.cb_type = bcmTimeCaptureOverflow;
        cb_overflow.cb = NULL;
        cb_overflow.user_data_ptr = NULL;
        break;
    case bcmTimeCapturetFill:
        cb_fill.cb_type = bcmTimeCapturetFill;
        cb_fill.cb = NULL;
        cb_fill.user_data_ptr = NULL;
        break;
    case bcmTimeCaptureAll:
        cb_all.cb_type = bcmTimeCaptureAll;
        cb_all.cb = NULL;
        cb_all.user_data_ptr = NULL;
        break;
    case bcmTimeCaptureTS0CntOffsetUpdated:
    case bcmTimeCaptureTS1CntOffsetUpdated:
        return BCM_E_PARAM;
    }

    if (cb_overflow.cb == NULL && cb_fill.cb == NULL && cb_all.cb == NULL){
        /* No callbacks are registered anymore. Disable interrupts */
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
        if (soc_is(unit, J2X_DEVICE)){
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TS_INT_ENABLE_0r, 0, 0, &regVal);
            regVal &= ~0x3 ;
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TS_INT_ENABLE_0r, 0, 0, regVal);
        } else if(SOC_IS_J2P(unit) || SOC_IS_J2C(unit) || SOC_IS_Q2A(unit)) {
            regVal = 0;
            WRITE_NS_TS_INT_ENABLEr(unit, regVal);
        } else {
            regVal = 0;
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regVal);
        }
#endif
    }
    return BCM_E_NONE;
}

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)

void
soc_dnx_timesync_ts_intr(int unit)
{
    uint32 int_enable = 0;  /* Interrupt enable */
    uint32 regval=0, regval1=0, regval2=0, regval3=0, lower=0, middle=0, upper=0, event_id=0;
    uint32 fifo_depth=0, capture_status=0, fifo_status=0;
    uint32 intr=0, over_flow=0;
    uint16 notify=0;
    uint32 int_status = 0;  /* Interrupt status */
    int buf_full=0, buf_idx=0, valid=0;
    uint32 int_status_clear = 0;  /* Interrupt status clear */
    uint32 ts_err = 0;
    int fifo_not_empty = 0;

#if defined(BCM_NANOSYNC_V1_SUPPORT)
    if (soc_is(unit, J2X_DEVICE)) {

        BCM_TIME_READ_NS_REGr(unit, NSYNC_TS_INT_STATUSr, 0, 0, &int_status);
        BCM_TIME_READ_NS_REGr(unit, NSYNC_TS_INT_ENABLE_0r, 0, 0, &int_enable);

        /* TS_FIFO_NOT_EMPTY interrupt */
        fifo_not_empty = soc_reg_field_get(unit, NSYNC_TS_INT_STATUSr, int_status, TS_FIFO_1_NOT_EMPTYf);

        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "fifo_not_empty[%d]\n"), fifo_not_empty));

        if (fifo_not_empty)
        {
            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NSYNC_TS_INT_STATUSr, &int_status_clear, TS_FIFO_1_NOT_EMPTYf, 1);
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TS_INT_STATUSr, 0, 0, int_status_clear);

            BCM_TIME_READ_NS_REGr(unit, NSYNC_TS_CAPTURE_STATUSr, 0, 0, &capture_status);
            fifo_depth = soc_reg_field_get(unit, NSYNC_TS_CAPTURE_STATUSr, capture_status, FIFO_1_DEPTHf);

            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "FIFO depth[%d]\n"), fifo_depth));

            while (fifo_depth != 0) {
                notify = 1;
                /* Read sequence defined in section 6.2.16 of the Nanosync Arch spec Rev 1.01 
                   is incorrect as confirmed by h/w in NANOSYNC-195.
                   Correct order is to read from TS_3->TS_0. */
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_3r, 0, 0, &regval3);
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_2r, 0, 0, &regval2);
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_1r, 0, 0, &regval1);
                BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_0r, 0, 0, &regval);

                /* Check capture event id */
                event_id = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_3r, regval3, TS_EVENT_IDf);
                ts_err = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_3r, regval3, TS_ERRf);
                valid = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_3r, regval3, TS_VALIDf);

                if (!valid)
                {
                    fifo_depth--;
                    continue;
                }

                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_1r,regval1, TS_1_Lf);
                middle = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_2r,regval2, TS_1_Mf);
                upper = soc_reg_field_get(unit, NSYNC_TIMESYNC_INPUT_TIME_FIFO_1_TS_3r,regval3, TS_1_Uf);

                /* Discarding 4-bit sub-ns */
                lower = (middle << 8) | (lower >> 4);
                upper = (upper << 8) | (middle >> 24);

                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]: %d: %x_%x\n"), fifo_depth, event_id, upper, lower));

                buf_idx = (dnx_ts_capture_info[unit]->ts_fifo_wr_index);
                buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
                if (!buf_full) {
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = (event_id >> 8);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = (event_id & 0xFF);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;

                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
                    dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

                    dnx_ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
                }
                fifo_depth--;
            } /* fifo_depth */
        } /* fifo_not_empty */

        over_flow = soc_reg_field_get(unit, NSYNC_TS_INT_STATUSr, int_status, TS_FIFO_1_OVERFLOWf);
        if (over_flow) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "TS FIFO status source overflow \n")));
            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NSYNC_TS_INT_STATUSr, &int_status_clear, TS_FIFO_1_OVERFLOWf, 1);
            /* Write W1TC to clear the flags */
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TS_INT_STATUSr, 0, 0, int_status_clear);
        }
    } else
#endif
    if(SOC_IS_J2P(unit) || SOC_IS_J2C(unit) || SOC_IS_Q2A(unit)) {

        READ_NS_TS_INT_STATUSr(unit, &int_status);
        READ_NS_TS_INT_ENABLEr(unit, &int_enable);

        /* TS_FIFO_NOT_EMPTY interrupt */
        if(SOC_IS_J2P(unit)){
            fifo_not_empty = soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_FIFO1_NOT_EMPTYf);
        }else {
            fifo_not_empty = soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_FIFO_NOT_EMPTYf);
        }

        if (fifo_not_empty)
        {
            /* Set W1TC to clear the flag */
            if(SOC_IS_J2P(unit)){
                soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO1_NOT_EMPTYf, 1);
            }else {
                soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO_NOT_EMPTYf, 1);
            }
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            WRITE_NS_TS_INT_STATUSr(unit, int_status_clear);

            READ_NS_TS_CAPTURE_STATUSr(unit, &capture_status);
            if(SOC_IS_J2P(unit)){
                fifo_depth = soc_reg_field_get(unit, NS_TS_CAPTURE_STATUSr, capture_status, FIFO1_DEPTHf);
            }else {
                fifo_depth = soc_reg_field_get(unit, NS_TS_CAPTURE_STATUSr, capture_status, FIFO_DEPTHf);
            }

            /*LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "FIFO depth[%d]\n"),fifo_depth));*/

            while (fifo_depth != 0) {
                notify = 1;
                /* Must read LOWER regiser first */

            if(SOC_IS_J2P(unit)){
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r(unit, &regval3);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r(unit, &regval2);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r(unit, &regval1);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r(unit, &regval);

                /* Check capture event id */
                event_id = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r,
                                regval3, TS_EVENT_IDf);

                ts_err = soc_reg_field_get(unit, NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r, regval3, TS_ERRf);
                valid = (regval3 >> 20) & 1;

                if (!valid){
                    continue;
                }


                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r,
                                regval1, TS1_Lf);

                middle = soc_reg_field_get(
                                            unit, NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r,
                                            regval2, TS1_Mf);

                upper = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r,
                                regval3, TS1_Uf);
            } else {
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r(unit, &regval3);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r(unit, &regval2);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r(unit, &regval1);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r(unit, &regval);

                /* Check capture event id */
                event_id = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                                regval3, TS_EVENT_IDf);

                ts_err = soc_reg_field_get(unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r, regval3, TS_ERRf);
                valid = (regval3 >> 20) & 1;

                if (!valid){
                    continue;
                }


                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r,
                                regval1, TS1_Lf);

                middle = soc_reg_field_get(
                                            unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r,
                                            regval2, TS1_Mf);

                upper = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                                regval3, TS1_Uf);
            }


                lower = (lower & 0xFFF) | ((middle & 0xFFFFF) << 12);
                upper = ((upper & 0xFF) << 12) | ((middle >> 20) & 0xFFF);

                /*discarding the LSB 4-bits to make it compatible with existing callback api*/
                lower = lower >> 4;
                lower = ((upper & 0xF) << 28) | lower;
                upper = upper >> 4;

                /* LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]:%d:%x%x\n"),fifo_depth, event_id, upper, lower));*/

                buf_idx = (dnx_ts_capture_info[unit]->ts_fifo_wr_index);
                buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
                if (!buf_full) {

                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & event_id;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = ((ts_err << 4) | (0x0F & (upper >> 16)));
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

                dnx_ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
                }
                fifo_depth--;
            }
        }

        if(SOC_IS_J2P(unit)){
            over_flow = soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_FIFO1_OVERFLOWf);
        } else {
            over_flow = soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_FIFO_OVERFLOWf);
        }
        if (over_flow) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "TS FIFO status source overflow \n")));
            /* Set W1TC to clear the flag */
            if(SOC_IS_J2P(unit)){
                soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO1_OVERFLOWf, 1);
            } else {
                soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO_OVERFLOWf, 1);
            }
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            WRITE_NS_TS_INT_STATUSr(unit, int_status_clear);
        }
    } else if (soc_is(unit,JERICHO2_ONLY_DEVICE)) {
        READ_CMIC_TIMESYNC_INTERRUPT_STATUSr(unit, &int_status);

        READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &capture_status);

        /* TS_FIFO_NOT_EMPTY interrupt */
        intr = soc_reg_field_get(unit, CMIC_TIMESYNC_INTERRUPT_STATUSr, int_status, INTERRUPTf);
        if (intr)
        {

            READ_CMIC_TIMESYNC_FIFO_STATUSr(unit, &fifo_status);
            over_flow = soc_reg_field_get(unit, CMIC_TIMESYNC_FIFO_STATUSr,
                                        fifo_status, SOURCE_OVERFLOWf);
            if(over_flow) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "TS FIFO status source overflow \n")));
                soc_reg_field_set(unit, CMIC_TIMESYNC_FIFO_STATUSr, &fifo_status, SOURCE_OVERFLOWf, 0);
                WRITE_CMIC_TIMESYNC_FIFO_STATUSr(unit, fifo_status);
            }

            fifo_depth = soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r, capture_status, FIFO_DEPTHf);
            /*LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "FIFO depth[%d]\n"),fifo_depth));*/

            while (fifo_depth != 0) {
                notify = 1;
                /* Must read LOWER regiser first */
                READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &lower);
                READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval1);

                /* Check capture event id */
                event_id = soc_reg_field_get(
                                unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                regval1, EVENT_IDf);

                /* Calculate seconds/nanoseconds from the timestamp value */
                upper = soc_reg_field_get(
                                unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                regval1, TIMESTAMPf);

                /* LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]:%d:%x%x\n"),fifo_depth, event_id, upper, lower));*/

                buf_idx = (dnx_ts_capture_info[unit]->ts_fifo_wr_index);
                buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
                if (!buf_full) {

                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & event_id;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
                dnx_ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

                dnx_ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
                }
                fifo_depth--;
            }

            if(over_flow) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]:%d:%x%x \n"),fifo_depth, event_id, upper, lower));
            }

            /* Clear status */
            WRITE_CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r(unit, capture_status);

            READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &int_enable);

            /* Clear interrupts */
            WRITE_CMIC_TIMESYNC_INTERRUPT_CLRr(unit, int_status);
            WRITE_CMIC_TIMESYNC_INTERRUPT_CLRr(unit, 0);

            /* Enable interrupts */
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, int_enable);

        }
    } else {
        /* currently on J2,J2C,Q2A are supported; */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "BCM TIME: Error !! device not supported \n")));
    }

    if (notify && !over_flow && (dnx_ts_capture_info[unit]->thread_id_time_capture_ts)) {
        shr_thread_wake(dnx_ts_capture_info[unit]->thread_id_time_capture_ts);
    }

#ifdef BCM_CMICX_SUPPORT
    /* Re-enabling timeysnc interrupt */
    if (soc_is(unit, J2X_DEVICE)){
        soc_cmic_intr_enable(unit, TS_INTR_J2X);
    } else {
        soc_cmic_intr_enable(unit, TS_INTR);
    }
#endif
}

#ifdef BCM_CMICX_SUPPORT
void
soc_dnx_timesync_ts_cmicx_intr(int unit, void *data)
{
    soc_dnx_timesync_ts_intr(unit);
}
#endif


void
_bcm_dnx_time_capture_ts_cb_thread(shr_thread_t th, void *unit_vp)
{
    int num_entries;
    int unit = PTR_TO_INT(unit_vp);
    uint8 cb_data_fill[12] = {0};

    dnx_ts_capture_info[unit]->time_capture_ts_active = 1;

    while (dnx_ts_capture_info[unit]->time_capture_ts_enable) {

        if ((dnx_ts_capture_info[unit] != NULL) &&
            (dnx_ts_capture_info[unit]->time_capture_ts_enable == 0)) {
            break;
        }

        num_entries = ((dnx_ts_capture_info[unit]->ts_fifo_wr_index - dnx_ts_capture_info[unit]->ts_fifo_rd_index) / 12);
        while (num_entries) {
            sal_memcpy(cb_data_fill, &(dnx_ts_capture_info[unit]->ts_fifo_data_fill[dnx_ts_capture_info[unit]->ts_fifo_rd_index]), 12);
            dnx_ts_capture_info[unit]->ts_fifo_rd_index += 12;

            if (cb_fill.cb != NULL) {
                 cb_fill.cb(unit, cb_fill.user_data_ptr, (uint32 *)cb_data_fill, bcmTimeCapturetFill);
            }
            num_entries--;
        }
        shr_thread_sleep(th, sal_sem_FOREVER);
    }

    dnx_ts_capture_info[unit]->time_capture_ts_active = 0;

    return;
}

int
bcm_dnx_time_capture_ts_init(int unit)
{
#ifdef BCM_CMICX_SUPPORT
    soc_cmic_intr_handler_t handle;
    intr_num_t intr_num;
#endif
    SHR_FUNC_INIT_VARS(unit);
    if (!(dnx_data_time.timesync.feature_get(unit, dnx_data_time_timesync_open_loop_tsfifo_support)))
    {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, " tsfifo_open_loop not defined.\n")));
        SHR_EXIT();
    } else {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, " open-loop support enabled.\n")));
    }

    if (dnx_ts_capture_info[unit] != NULL) {
        SHR_EXIT();
    }

    SHR_ALLOC_SET_ZERO_ERR_EXIT(dnx_ts_capture_info[unit],
        sizeof(bcm_dnx_time_ts_capture_info_t), "ts capture info",
        "%s%s%s","BCM TIME: Error !! failed to allocate memory for time capture info\n",EMPTY,EMPTY);

#ifdef BCM_CMICX_SUPPORT
    if (dnx_drv_soc_feature(unit, soc_feature_cmicx)) {

        if (soc_is(unit, J2X_DEVICE)){
            intr_num = TS_INTR_J2X;
        } else {
            intr_num = TS_INTR;
        }
        handle.num = intr_num;
        handle.intr_fn = soc_dnx_timesync_ts_cmicx_intr;
        handle.intr_data = NULL;
        soc_cmic_intr_register(unit, &handle, 1);
        soc_cmic_intr_enable(unit, intr_num);
    }
#endif

    /* Create the time capture callback thread */
    dnx_ts_capture_info[unit]->time_capture_ts_enable = 1;
    dnx_ts_capture_info[unit]->thread_id_time_capture_ts =
        shr_thread_start("Time capture cb thread", 100, _bcm_dnx_time_capture_ts_cb_thread, INT_TO_PTR(unit));

    if (dnx_ts_capture_info[unit]->thread_id_time_capture_ts == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "BCM TIME: Error !! callback thread create failed\n")));
        return SOC_E_INTERNAL;
    }
exit:
    SHR_FUNC_EXIT;
}


#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

int bcm_dnx_time_ts_offset_set(
    int unit,
    int64 ts_offset,
    uint64 ts_update_intv,
    int ts_counter)
{
    return BCM_E_UNAVAIL;
}

int
_bcm_dnx_time_counter_freq_frac_set(int unit, int ts_counter)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (ts_counter == 1) {
        /* Disable the common control of TS0 and TS1 */
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &rval, ENABLE_COMMON_CONTROLf, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x2000);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, rval));

    } else if (ts_counter == 0) {
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x2000);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, rval));
    } else {
        rv = BCM_E_FAIL;
    }

    return rv;
}

int
_bcm_dnx_time_counter_enable(int unit, int ts_counter, int enable)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (soc_is(unit,J2X_DEVICE))
    {
#if defined(BCM_J2X_SUPPORT)
        int counter_enable = 0;
        if (ts_counter == 1) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, &rval);
            counter_enable = soc_reg_field_get(unit, NSYNC_IEEE_1588_TIME_CONTROLr, rval, COUNTER_ENABLEf);
            soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &rval, COUNTER_ENABLEf, enable?(counter_enable|0x02):(counter_enable&0x05)); /*Enable/Disable TS1 counter */
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, rval);

        } else if (ts_counter == 0) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, &rval);
            counter_enable = soc_reg_field_get(unit, NSYNC_IEEE_1588_TIME_CONTROLr, rval, COUNTER_ENABLEf);
            soc_reg_field_set(unit, NSYNC_IEEE_1588_TIME_CONTROLr, &rval, COUNTER_ENABLEf, enable?(counter_enable|0x01):(counter_enable&0x06)); /*Enable/Disable TS0 counter */
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, rval);
        } else {
            rv = BCM_E_FAIL;
        }
#endif
    } else {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));
        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }

    }
    return rv;
}

STATIC int _bcm_dnx_time_ts_offset_reset(
    int unit,
    int ts_counter)
{
    int rv = BCM_E_UNAVAIL;


    uint32 regval_lower, regval_upper;
    uint64 ts_ctr_val;

    _bcm_dnx_time_counter_enable(unit, ts_counter, 0);
    if (soc_is(unit, J2X_DEVICE))
    {
#if defined(BCM_J2X_SUPPORT)
        if (ts_counter == 0){
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_UPPER_STATUSr, 0, 0, &regval_upper);
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_LOWER_STATUSr, 0, 0, &regval_lower);
            ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_OFFSET_UPPERr, 0, 0, 0);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_OFFSET_LOWERr, 0, 0, 0);

            ts_ctr_val = ((ts_ctr_val >> 4) + 1000) % 0xFFFFFFFFFFFFULL;
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_UPDATE_LOWERr, 0, 0, ts_ctr_val & 0xFFFFFFFF);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_FREQ_CTRL_UPDATE_UPPERr, 0, 0, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);

        } else if (ts_counter == 1){
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_TIMESTAMP_UPPER_STATUSr, 0, 0, &regval_upper);
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_TIMESTAMP_LOWER_STATUSr, 0, 0, &regval_lower);
            ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_OFFSET_UPPERr, 0, 0, 0);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_OFFSET_LOWERr, 0, 0, 0);

            ts_ctr_val = ((ts_ctr_val >> 4) + 1000) % 0xFFFFFFFFFFFFULL;
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_UPDATE_LOWERr, 0, 0, ts_ctr_val & 0xFFFFFFFF);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_FREQ_CTRL_UPDATE_UPPERr, 0, 0, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
#endif
    } else
    {
        if (ts_counter == 0){
            READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &regval_upper);
            READ_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit, &regval_lower);
            ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_UPPERr(unit, 0);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_LOWERr(unit, 0);

            ts_ctr_val = ((ts_ctr_val >> 4) + 1000) % 0xFFFFFFFFFFFFULL;
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr(unit, ts_ctr_val & 0xFFFFFFFF);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr(unit, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);

        } else if (ts_counter == 1){
            READ_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr(unit, &regval_upper);
            READ_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr(unit, &regval_lower);
            ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

            WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_UPPERr(unit, 0);
            WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_LOWERr(unit, 0);

            ts_ctr_val = ((ts_ctr_val >> 4) + 1000) % 0xFFFFFFFFFFFFULL;
            WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_LOWERr(unit, ts_ctr_val & 0xFFFFFFFF);
            WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr(unit, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    }

    _bcm_dnx_time_counter_enable(unit, ts_counter, 1);
    sal_usleep(1000);
    rv = BCM_E_NONE;
exit:

    return rv;
}

#ifdef INCLUDE_GDPLL
void _bcm_bs_slave_bhsb4KHz_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{
    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Update gdpll chan config for HB 4KHz\n")));
    pGdpllChan->chan_dpll_config.k1[0] = 2649176;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 11;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 3191478;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 380;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 250;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 380;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 1200;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 48;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 0;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 0;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 1600000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 1600000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

void _bcm_bs_slave_bhsb1Hz_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{
    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Update gdpll chan config for HB 1Hz\n")));
    pGdpllChan->chan_dpll_config.k1[0] = 44050303;
    pGdpllChan->chan_dpll_config.k1[1] = 56877051;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 4155689;
    pGdpllChan->chan_dpll_config.k1k2[1] = 303506;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 1024;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 1024;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 512;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 1024;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 200;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 200;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 48;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 0;
    pGdpllChan->chan_dpll_config.dpll_num_states = 2;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 0;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 500000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 500000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 200;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

int _bcm_bs_slave_dpll_chan_create(int unit, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                   BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                   BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                   BCM_GDPLL_CHAN_OUTPUT_CONF;
    int bs_id = intf->id;
    int gdpllChan = 0;

    bcm_gdpll_chan_t    gdpll_chan;
    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    gdpll_chan.event_ref.input_event = ( (bs_id == 0) ? bcmGdpllInputEventBS0ConvTC : bcmGdpllInputEventBS1ConvTC);
    gdpll_chan.event_ref.event_divisor = 1;
    gdpll_chan.event_ref.event_dest = bcmGdpllEventDestBitmap;
    gdpll_chan.event_ref.event_dest_bitmap = BCM_GDPLL_EVENT_DEST_GDPLL;
    gdpll_chan.event_ref.ts_counter = 1;
    gdpll_chan.event_ref.ppm_check_enable = 0;
    gdpll_chan.event_ref.port_type = bcmGdpllPortTypeNone;

    gdpll_chan.event_fb.input_event = ( (bs_id == 0) ? bcmGdpllInputEventBS0HB : bcmGdpllInputEventBS1HB);
    gdpll_chan.event_fb.event_divisor = 1;
    gdpll_chan.event_fb.event_dest = bcmGdpllEventDestBitmap;
    gdpll_chan.event_fb.event_dest_bitmap = BCM_GDPLL_EVENT_DEST_GDPLL;
    gdpll_chan.event_fb.ts_counter = 1;
    gdpll_chan.event_fb.ppm_check_enable = 0;
    gdpll_chan.event_fb.port_type = bcmGdpllPortTypeNone;

    gdpll_chan.chan_prio = 0;
    gdpll_chan.out_event = bcmGdpllOutputEventTS1;
    gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Creating gdpll chan for BS%d slave : HB: %dHz\n"), bs_id, intf->heartbeat_hz));

    if (intf->heartbeat_hz == 4000) {
        _bcm_bs_slave_bhsb4KHz_dpll_chan_config(unit, &gdpll_chan);
    } else if (intf->heartbeat_hz == 1) {
        _bcm_bs_slave_bhsb1Hz_dpll_chan_config(unit, &gdpll_chan);
    } else {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d slave \n"), bs_id));
        return BCM_E_PARAM;
    }

    rv = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d slave \n"), bs_id));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d created for BS%d slave \n"), gdpllChan, bs_id));

        rv = bcm_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d enable failed for BS%d slave \n"), gdpllChan, bs_id));
            return rv;
        }
        LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                "\n gdpll chan %d enabled for BS slave ... \n"), gdpllChan));
        TIME_DPLL_CHAN(unit, bs_id) = gdpllChan;
    }
    return rv;
}

/* This is a BS-PLL synthesizer control loop */
int _bcm_bs_master_dpll_chan_create(int unit, bcm_time_interface_t *intf, int ts_counter)
{
    bcm_gdpll_chan_t    gdpll_chan;
    int rv = BCM_E_NONE;
    int gdpllChan = 0;

    int bs_id = intf->id;
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    gdpll_chan.event_fb.input_event = (bs_id == 0) ? bcmGdpllInputEventBS0PLL : bcmGdpllInputEventBS1PLL;
    gdpll_chan.event_fb.event_divisor = 2500;    /* runs at 4KHz */
    gdpll_chan.event_fb.event_divisor_hi = 2500; /* runs at 4KHz */
    gdpll_chan.event_fb.event_dest = bcmGdpllEventDestBitmap;
    gdpll_chan.event_fb.event_dest_bitmap = 0x4; /* BCM_GDPLL_EVENT_DEST_GDPLL */
    gdpll_chan.event_fb.ts_counter = ts_counter; /* TS1 */
    gdpll_chan.event_fb.ppm_check_enable = 0;

    gdpll_chan.chan_prio = 0;
    gdpll_chan.out_event = (bs_id == 0) ? bcmGdpllOutputEventBSPLL0 : bcmGdpllOutputEventBSPLL1;
    gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7; /* TBD: Need to recheck if this is required for R5 as well */

    gdpll_chan.chan_dpll_config.k1[0] = 666527938;
    gdpll_chan.chan_dpll_config.k1[1] = 0;
    gdpll_chan.chan_dpll_config.k1[2] = 0;

    gdpll_chan.chan_dpll_config.k1Shift[0] = 2;
    gdpll_chan.chan_dpll_config.k1Shift[1] = 0;
    gdpll_chan.chan_dpll_config.k1Shift[2] = 0;

    gdpll_chan.chan_dpll_config.k1k2[0] = 11115;
    gdpll_chan.chan_dpll_config.k1k2[1] = 0;
    gdpll_chan.chan_dpll_config.k1k2[2] = 0;

    gdpll_chan.chan_dpll_config.k1k2Shift[0] = 5;
    gdpll_chan.chan_dpll_config.k1k2Shift[1] = 0;
    gdpll_chan.chan_dpll_config.k1k2Shift[2] = 0;

    gdpll_chan.chan_dpll_config.lockDetThres[0] = 38000;
    gdpll_chan.chan_dpll_config.lockDetThres[1] = 0;
    gdpll_chan.chan_dpll_config.lockDetThres[2] = 0;

    gdpll_chan.chan_dpll_config.lockIndicatorThresholdLo = 25000;
    gdpll_chan.chan_dpll_config.lockIndicatorThresholdLHi = 38000;

    gdpll_chan.chan_dpll_config.dwell_count[0] = 3000;
    gdpll_chan.chan_dpll_config.dwell_count[1] = 0;
    gdpll_chan.chan_dpll_config.dwell_count[2] = 0;

    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[0]) = 200000000;
    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    gdpll_chan.chan_dpll_config.nco_bits = 48;
    gdpll_chan.chan_dpll_config.phase_counter_ref = 1;
    gdpll_chan.chan_dpll_config.dpll_num_states = 1;

    gdpll_chan.chan_dpll_config.idump_mod[0] = 1;
    gdpll_chan.chan_dpll_config.idump_mod[1] = 0;
    gdpll_chan.chan_dpll_config.idump_mod[2] = 0;

    gdpll_chan.chan_dpll_config.phase_error_shift = 12;
    u64_H(gdpll_chan.chan_dpll_config.nominal_loop_filter) = 0x0F000000;
    u64_L(gdpll_chan.chan_dpll_config.nominal_loop_filter) = 0;

    gdpll_chan.chan_dpll_config.invert_phase_error = 1;
    gdpll_chan.chan_dpll_config.norm_phase_error = 0;
    gdpll_chan.chan_dpll_config.phase_error_thres0 = 500000000;
    gdpll_chan.chan_dpll_config.phase_error_thres1 = 500000000;
    gdpll_chan.chan_dpll_config.holdover_filter_coeff = 512;

    gdpll_chan.chan_dpll_state.dpll_state = 0;
    u64_H(gdpll_chan.chan_dpll_state.loop_filter) = 0x0F000000;
    u64_L(gdpll_chan.chan_dpll_state.loop_filter) = 0;
    gdpll_chan.chan_dpll_state.dwell_counter = 0;
    gdpll_chan.chan_dpll_state.lockDetFilter = 64000;
    u64_H(gdpll_chan.chan_dpll_state.offset) = 0;
    u64_L(gdpll_chan.chan_dpll_state.offset) = 0;
    gdpll_chan.chan_dpll_state.init_flag = 1;
    gdpll_chan.chan_dpll_state.init_offset_flag = 0;
    u64_H(gdpll_chan.chan_dpll_state.phase_counter) = 0;
    u64_L(gdpll_chan.chan_dpll_state.phase_counter) = 0;
    u64_H(gdpll_chan.chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(gdpll_chan.chan_dpll_state.phaseCounterDelta) = 4000000;
    gdpll_chan.chan_dpll_state.phaseCounterN = 1;
    gdpll_chan.chan_dpll_state.phaseCounterM = 0;
    gdpll_chan.chan_dpll_state.phaseCounterFrac = 0;
    gdpll_chan.chan_dpll_state.idumpCounter = 0;
    u64_H(gdpll_chan.chan_dpll_state.accumPhaseError)=0;
    u64_L(gdpll_chan.chan_dpll_state.accumPhaseError)=0;
    gdpll_chan.chan_dpll_state.lockIndicator = 0;
    gdpll_chan.chan_dpll_state.lossOfLockIndicator = 0;

    rv = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d master \n"), bs_id));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d created for BS%d master \n"), gdpllChan, bs_id));
        rv = bcm_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan enable failed for BS%d master \n"), bs_id));
            return rv;
        }
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d enabled for BS%d master \n"), gdpllChan, bs_id));
        TIME_DPLL_CHAN(unit, bs_id) = gdpllChan;
    }
    return rv;
}

/*static*/ int
_bcm_program_ptpntptod(int unit, bcm_time_spec_t ts, bcm_time_format_t tod_type)
{
    int rv  = BCM_E_NONE;
    uint32 regval;
    uint32 regval_sec;
    uint64 frac_sec;
    uint64 nanosec, div;
    uint64 val_64b;

    /*  regval_sec = new_time.u48_sec; */
    regval_sec = COMPILER_64_LO(ts.seconds);
    regval_sec = ((regval_sec & 1) << 31) | (regval_sec >> 1);

    if (tod_type == bcmTimeFormatNTP) {

        /* Program the initial NTP-ToD frequency control */
        WRITE_NTP_TIME_FREQ_CONTROLr(unit, 0x44B82FA1);

        /* NTP timestamps are 64 bit, Q32 seconds, i.e seconds * 2^32, so bit 0 == 1/2^32 of a second */
        /* Convert from nanoseconds to those fractions of a second */
        /*  frac_sec = (((uint64_t)new_time.u32_Nsec) << 32) / ONE_BILLION; */
        COMPILER_64_SET(div, 0, 1000000000);    /* ONE_BILLION */
        COMPILER_64_SET(nanosec, 0, ts.nanoseconds);
        COMPILER_64_SHL(nanosec, 32);
        COMPILER_64_UDIV_64(nanosec, div);
        COMPILER_64_COPY(frac_sec, nanosec);

        /* NTP time is stored in seconds and fractional seconds */
        WRITE_NTP_TIME_SECr(unit, regval_sec);

        COMPILER_64_COPY(val_64b, frac_sec);
        COMPILER_64_SHR(val_64b, 6);
        WRITE_NTP_TIME_FRAC_SEC_UPPERr(unit, COMPILER_64_LO(val_64b));

        COMPILER_64_COPY(val_64b, frac_sec);
        COMPILER_64_SHL(val_64b, 26);
        WRITE_NTP_TIME_FRAC_SEC_LOWERr(unit, COMPILER_64_LO(val_64b));

        READ_NTP_TIME_CONTROLr(unit, &regval);
        regval |= 0x1f;    /* Set load-enable bits, |B04...B00|. */

        WRITE_NTP_TIME_CONTROLr(unit, regval);

        regval &= ~(0x1f); /* Clear load-enable bits, |B04...B00|. */
        regval |= 0x20;    /* Set count-enable bit, |B05|. */
        WRITE_NTP_TIME_CONTROLr(unit, regval);
        /* cli_out("### TS After NTP write: "); print_ts(unit); */
    }

    if (tod_type == bcmTimeFormatPTP) {

        /* Program the initial PTP-ToD frequency control */
        WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, 0x10000000);

        /* 1588 time is stored in seconds and nanoseconds */
        WRITE_IEEE1588_TIME_SECr(unit, regval_sec);
        WRITE_IEEE1588_TIME_FRAC_SEC_UPPERr(unit, ts.nanoseconds >> 6);
        WRITE_IEEE1588_TIME_FRAC_SEC_LOWERr(unit, ts.nanoseconds << 26);

        READ_IEEE1588_TIME_CONTROLr(unit, &regval);
        regval |= 0x1f;    /* Set load-enable bits, |B04...B00|. */
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval);
        regval &= ~(0x1f); /* Clear load-enable bits, |B04...B00|. */
        regval |= 0x20;    /* Set count-enable bit, |B05|. */
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval);
        /* cli_out("### TS After PTP write: "); print_ts(unit); */
    }

    return rv;
}

void _bcm_time_ptp_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{

    pGdpllChan->event_fb.input_event = bcmGdpllInputEventIPDM1;
    pGdpllChan->event_fb.event_divisor = 1;
    pGdpllChan->event_fb.event_dest = bcmGdpllEventDestM7;
    pGdpllChan->event_fb.ts_counter = 1; /* TS1 */
    pGdpllChan->event_fb.ppm_check_enable = 0;

    pGdpllChan->chan_prio = 0;
    pGdpllChan->out_event = 11;
    pGdpllChan->ts_pair_dest = bcmGdpllDebugDestM7;

    pGdpllChan->chan_dpll_config.k1[0] = 2882304;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 3;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 1844675;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 3800;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 2500;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 3800;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 50;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 1000000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 32;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 1;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 1;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 100000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 100000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    /* will be updated accordingly later */
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0x3;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0xB9ACA000;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

void _bcm_time_ntp_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{

    pGdpllChan->event_fb.input_event = bcmGdpllInputEventIPDM0;
    pGdpllChan->event_fb.event_divisor = 1;
    pGdpllChan->event_fb.event_dest = bcmGdpllEventDestM7;
    pGdpllChan->event_fb.ts_counter = 1; /* TS1 */
    pGdpllChan->event_fb.ppm_check_enable = 0;

    pGdpllChan->chan_prio = 0;
    pGdpllChan->out_event = 10;
    pGdpllChan->ts_pair_dest = bcmGdpllDebugDestM7;

    pGdpllChan->chan_dpll_config.k1[0] = 3094850;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 5;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 3961408;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 1;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 3800;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 2500;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 3800;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 50;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 1000000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 32;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 1;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x44B82AF1;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 1;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 100000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 100000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x44B82AF1;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    /* will be updated accordingly later */
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0x3;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0xB9ACA000;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

int
_bcm_bs_tod_dpll_chan_create(int unit, bcm_time_format_t tod_type)
{
    bcm_gdpll_chan_t    gdpll_chan;
    bcm_time_ts_counter_t counter;
    uint64 secs, div, nano_secs;
    bcm_time_spec_t ts;
    int prog_phase_counter = 0;
    int rv = BCM_E_NONE;
    int gdpllChan = 0;

    int32 poll_period_uSec=0;
    uint32 poll_interval_uSec = 0;

    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;

    if ((tod_type != bcmTimeFormatPTP) && (tod_type != bcmTimeFormatNTP)) {
        return BCM_E_PARAM;
    }

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    if (tod_type == bcmTimeFormatNTP) {
        _bcm_time_ntp_dpll_chan_config(unit, &gdpll_chan);
    } else {
        _bcm_time_ptp_dpll_chan_config(unit, &gdpll_chan);
    }

    poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
    poll_interval_uSec = 100000; /* Poll for every 100000uSec=100mSec */

    counter.ts_counter = gdpll_chan.event_fb.ts_counter;

    while(poll_period_uSec > 0) {

        rv = bcm_time_ts_counter_get(unit, &counter);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in bcm_time_ts_counter_get ### \n")));
            return rv;
        }

        COMPILER_64_SHR(counter.ts_counter_ns, 4);

        /* Needs FIX
         * NTP/PTP ToD counter will get the first ONE_SEC set after 500mSec/537mSec respectively, after enabling.
         * This is verified by reading the present TS counter value(say T1) before programming 0 nanosec to
         * NTP/PTP ToD and read the fisrt 1PPS event(i.e, ONE_SEC) of NTP/PTP ToD. With this its observed that the
         * timestamp is (T1+500mSec/537mSec).
         * To compensate this, start the TS counter prior by 500mSec/(1000-537=463)mSec
         */
        if (tod_type == bcmTimeFormatNTP) {
            COMPILER_64_SUB_32(counter.ts_counter_ns, 500000000);
        } else {
            COMPILER_64_SUB_32(counter.ts_counter_ns, 463000000);
        }

        /* Get seconds from the timestamp value */
        COMPILER_64_COPY(secs, counter.ts_counter_ns);
        COMPILER_64_SET(div, 0, 1000000000);
        COMPILER_64_UDIV_64(secs, div);
        COMPILER_64_SET(ts.seconds, 0, COMPILER_64_LO(secs));

        /* Get nanoseconds from the timestamp value */
        COMPILER_64_COPY(nano_secs, counter.ts_counter_ns);
        COMPILER_64_UMUL_32(secs, 1000000000);
        COMPILER_64_SUB_64(nano_secs, secs);
        ts.nanoseconds = COMPILER_64_LO(nano_secs);

        /* Bound check for the DPLL setup */
        if ((ts.nanoseconds < 800000000) && (ts.nanoseconds > 600000000)) {
            prog_phase_counter = 1;
            break;
        }

        sal_usleep(poll_interval_uSec);
        poll_period_uSec -= poll_interval_uSec;
    }

    if (!prog_phase_counter) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "Error: Could not program phase counter !!! \n")));
        return -1/*CMD_FAIL*/;
    }

    if (tod_type == bcmTimeFormatNTP) {
        uint32 result_nanosec;
        bcm_time_spec_t prog_tod = {0, 0, 0};

        result_nanosec = ts.nanoseconds + gdpll_chan_NTPoffset.nanoseconds;
        prog_tod.nanoseconds = result_nanosec % 1000000000;
        COMPILER_64_ADD_32(prog_tod.seconds, (result_nanosec / 1000000000));
        COMPILER_64_ADD_64(prog_tod.seconds, gdpll_chan_NTPoffset.seconds);
        COMPILER_64_ADD_64(prog_tod.seconds, ts.seconds);

        /* Program the NTP counter */
        _bcm_program_ptpntptod(unit, prog_tod, tod_type);
    } else if (tod_type == bcmTimeFormatPTP){
        /* Program the PTP counter */
        _bcm_program_ptpntptod(unit, ts, tod_type);
    }

    /* Program the phase counter per spec */
    COMPILER_64_ADD_32(ts.seconds, 1);
    COMPILER_64_UMUL_32(ts.seconds, 1000000000);
    COMPILER_64_SHL(ts.seconds, 4);
    COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, ts.seconds);

    rv = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan create failed for tod format [%s] \n"), (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan%d created for tod format [%s] \n"), gdpllChan, (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));

        rv = bcm_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan enable failed for tod format [%s] \n"), (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));
            return rv;
        }
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan%d enabled for tod format [%s] \n"), gdpllChan, (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));

        UPDATE_DPLL_CHAN_FOR_TOD(unit, tod_type, gdpllChan);
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "TOD channels: NTP[%d] PTP[%d] \n"), TIME_DPLL_CHAN_NTP(unit), TIME_DPLL_CHAN_PTP(unit)));
    }
    return rv;
}

static int g_dpll_chan_create_unit_num = 0;
void _bs_slave_thread(void *intf_vp) {
    int unit=g_dpll_chan_create_unit_num;
    int rv = BCM_E_NONE;
    bcm_time_interface_t *intf = intf_vp;

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                        "Slave thread for BS%d\n"), intf->id));

    rv = _bcm_bs_slave_dpll_chan_create(unit, intf);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in creating dpll chan for BS slave ### \n")));
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                        "Exiting the slave thread for BS%d\n"), intf->id));

    TIME_INTERFACE_CONFIG(unit, intf->id).bs_thread = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

int
_bcm_bs_dpll_chan_destroy(int unit, int bs_id)
{
    int gdpllChan = BCM_TIME_DPLL_CHAN_INVALID;
    int rv = BCM_E_NONE;

    gdpllChan = TIME_DPLL_CHAN(unit, bs_id);

    if( gdpllChan == BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### gdpll chan invalid  ### \n")));
        return rv;
    } else {
        rv = bcm_gdpll_chan_destroy(unit, gdpllChan);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in dpll chan destroy ### \n")));
            return rv;
        }
        TIME_DPLL_CHAN(unit, bs_id) = BCM_TIME_DPLL_CHAN_INVALID;
    }
    return rv;
}

int
_bcm_bs_dpll_chan_recreate(int unit, int bs_id)
{
    int gdpllChan = BCM_TIME_DPLL_CHAN_INVALID;
    int rv = BCM_E_NONE;
    bcm_time_interface_t *intf = NULL;

    gdpllChan = TIME_DPLL_CHAN(unit, bs_id);

    if( gdpllChan != BCM_TIME_DPLL_CHAN_INVALID) {
        rv = _bcm_bs_dpll_chan_destroy(unit, bs_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in destroy dpll chan ### \n")));
            return rv;
        }
    }

    if (((TIME_INTERFACE(unit, bs_id))->flags & BCM_TIME_INPUT) == 0) {
        cli_out("\n i/f[%d] in master mode ... \n", bs_id);
        rv = _bcm_bs_master_dpll_chan_create(unit, TIME_INTERFACE(unit, bs_id), 1/* TS1 */);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating dpll chan ### \n")));
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Create chan ### \n")));

        intf = TIME_INTERFACE(unit, bs_id);

        rv = _bcm_bs_slave_dpll_chan_create(unit, intf);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating dpll chan ### \n")));
        }
    }
    return rv;
}

int _bcm_bs_trigger_tc_update(int unit, int bs_id)
{
    int rv = BCM_E_NONE;
#ifdef EDKHLIB
    uint8 buff[40];
    uint8 *buf = buff;
    rv = EDK_ERR_NONE;

    _SHR_PACK_U32(buf, EDK_NOTIFY_EVENT_BS);
    _SHR_PACK_U32(buf, bs_id);
    _SHR_PACK_U32(buf, (BS_TC_CTRL_TGT_PTP_TOD | BS_TC_CTRL_TGT_TS1));

    rv = edk_rpc_call_sync(edk_bs_handle, "uc_bs_event_notify_register", buff, 12, NULL, 0, NULL, MCS_BS_MAX_EDK_MSG_TIMEOUT);
    if (rv != EDK_ERR_NONE ) {
        cli_out("### uc_bs_event_notify_register() failed: \n");
    } else {
        LOG_INFO(BSL_LS_BCM_TIME,
            (BSL_META_U(unit, " %s bs_id[%d] done. \n"), FUNCTION_NAME(), bs_id));
    }
#endif /* EDKHLIB */
    return rv;
}

void _bcm_bs_process_chan_error(int unit, int chan)
{
    int bs_id = 0;

    for (bs_id=0; bs_id < TIME_CONFIG(unit)->intf_count ; bs_id++) {
        
        LOG_INFO(BSL_LS_BCM_TIME,
                (BSL_META_U(unit, " %s: chan:%d bs_id[%d] \n"),
                FUNCTION_NAME(), chan, bs_id));
        
        if (chan == TIME_DPLL_CHAN(unit, bs_id)) {
            _bcm_bs_trigger_tc_update(unit, bs_id);
        }
    }
}

#ifdef EDKHLIB
static void _bcm_bs_event_cb_func(int event_id, void *cookie, uint64_t param)
{
    int unit = 0;
    int bs_id = 0;
    int rv = BCM_E_NONE;

    LOG_INFO(BSL_LS_BCM_TIME,
            (BSL_META_U(unit, " %s: event:%d param:0x%llx \n"), FUNCTION_NAME(),
            event_id, (unsigned long long)param));

    if (param & EVENT_EN_BS0TC) {
        bs_id = 0;
        param &= ~EVENT_EN_BS0TC;
        rv = _bcm_bs_dpll_chan_recreate(unit, bs_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in recreating dpll chan BS:%d### \n"), bs_id));
        }
    }

    if (param & EVENT_EN_BS1TC) {
        bs_id = 1;
        param &= ~EVENT_EN_BS1TC;
        rv = _bcm_bs_dpll_chan_recreate(unit, bs_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in recreating dpll chan BS:%d### \n"), bs_id));
        }
    }

}
#endif /* EDKHLIB */

int _bcm_bs_dpll_chan_create(int unit, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;
    int master = ((intf->flags & BCM_TIME_INPUT) == 0);

    if(master) {
        cli_out("### _bcm_bs_dpll_chan_create: MASTER channel\n");
        sal_usleep(2*1000); /* Allow sufficient time for FW to setup the master */
        rv = _bcm_bs_master_dpll_chan_create(unit, intf, 1/*TS1*/);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating dpll chan ### \n")));
        }
    } else {
#ifdef EDKHLIB
        if (edk_bs_handle == NULL) {
            return BCM_E_INIT;
        }
        if (edk_notify_register(edk_bs_handle, EDK_NOTIFY_EVENT_BS, _bcm_bs_event_cb_func, edk_bs_handle) != EDK_ERR_NONE)
        {
            LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(0, "### Error in edk_notify_register ### \n")));
            return BCM_E_INTERNAL;
        }
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                        " Register for event notification. \n") ));

        /* Enable M7 core intr. */
        _bcm_gdpll_chan_err_intr_enable(unit);

        /* Trigger TC update procedure. */
        rv = _bcm_bs_trigger_tc_update(unit, intf->id);
#endif /* EDKHLIB */
    }
    return rv;
}

int _bcm_time_tod_dpll_chan_create(int unit, bcm_time_format_t tod_type) {

    int rv = BCM_E_NONE;
    int dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;


    dpll_chan = (tod_type==bcmTimeFormatNTP)? TIME_DPLL_CHAN_NTP(unit) : TIME_DPLL_CHAN_PTP(unit);

    if( dpll_chan != BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                                "\n dpll chan exists for tod %d\n"), tod_type));
        return BCM_E_EXISTS;
    }

    rv = _bcm_bs_tod_dpll_chan_create(unit, tod_type);
    {
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in creating dpll chan for TOD type[%d] ### \n"), tod_type));
        }
    }
    return rv;
}

int _bcm_time_tod_dpll_chan_recreate(int unit, bcm_time_format_t tod_type) {

    int rv = BCM_E_NONE;
    int dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;

    dpll_chan = (tod_type==bcmTimeFormatNTP)? TIME_DPLL_CHAN_NTP(unit) : TIME_DPLL_CHAN_PTP(unit);

    if( dpll_chan == BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### gdpll chan not created yet  ### \n")));
        return rv;
    } else {
        rv = bcm_gdpll_chan_destroy(unit, dpll_chan);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in destroying dpll chan[%d] for TOD type[%d] ### \n"), dpll_chan, tod_type));
        }
    }

    rv = _bcm_bs_tod_dpll_chan_create(unit, tod_type);
    {
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in creating dpll chan for TOD type[%d] ### \n"), tod_type));
        }
    }
    return rv;
}

#endif /* INCLUDE_GDPLL */

#define u64_LOWER(v)                            ((uint32)(v & 0xFFFFFFFF))

int
_bcm_dnx_time_counter_set(int unit, int ts_counter, uint64 ts_val)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    /* 52b format frpom 48b ns */
    COMPILER_64_SHL(ts_val, 4);

    if (soc_is(unit, J2X_DEVICE))
    {
#if defined(BCM_J2X_SUPPORT)
        if (ts_counter == 1) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_2r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_LOWER(ts_val) >> 24);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_2r, 0, 0, rval);

            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_1r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_LOWER(ts_val) << 8);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_1r, 0, 0, rval);

            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_0r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_INIT_ACCUMULATOR_0r, 0, 0, rval);

        } else if (ts_counter == 0) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_2r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_LOWER(ts_val) >> 24);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_2r, 0, 0, rval);

            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_1r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_LOWER(ts_val) << 8);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_1r, 0, 0, rval);

            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_0r, 0, 0, &rval);
            soc_reg_field_set(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            BCM_TIME_WRITE_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_INIT_ACCUMULATOR_0r, 0, 0, rval);
        } else {
            rv = BCM_E_FAIL;
        }
#endif
    } else
    {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_LOWER(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_LOWER(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_LOWER(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_LOWER(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    }
    return rv;
}

int
_bcm_dnx_time_counter_get(int unit, int ts_counter, uint64 *pTS_val)
{
    int rv  = BCM_E_NONE;
    uint64 ts = 0;
    uint32 upper;
    uint32 lower;


    if (soc_is(unit, J2X_DEVICE))
    {
#if defined(BCM_J2X_SUPPORT)
        if (ts_counter == 1) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_TIMESTAMP_UPPER_STATUSr, 0, 0, &upper);
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_1_TIMESTAMP_LOWER_STATUSr, 0, 0, &lower);
        } else if(ts_counter == 0) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_UPPER_STATUSr, 0, 0, &upper);
            BCM_TIME_READ_NS_REGr(unit, NSYNC_TIMESYNC_TS_0_TIMESTAMP_LOWER_STATUSr, 0, 0, &lower);
        } else {
            return BCM_E_FAIL;
        }
#endif
    } else
    {
        if (ts_counter == 1) {
            READ_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr(unit, &upper);
            READ_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr(unit, &lower);
        } else if(ts_counter == 0) {
            READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &upper);
            READ_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit, &lower);
        } else {
            return BCM_E_FAIL;
        }
    }

    u64_H(ts) = upper;
    ts = ts | ((uint64)lower);

    COMPILER_64_SHR(ts, 4);

    *pTS_val = ts;

    return rv;
}

/*
 * Function:
 *      bcm_dnx_time_ts_counter_set
 * Purpose:
 *      Set timestamper counter
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      counter - (IN)  counter values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_dnx_time_ts_counter_set(
        int unit,
        bcm_time_ts_counter_t  *counter)
{
    int rv = BCM_E_UNAVAIL;


    uint64 check;
    uint32 u64_hi = 0xffff, u64_low = 0xffffffff;
    int ts_counter;

    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (counter == NULL) {
        return BCM_E_PARAM;
    }

    COMPILER_64_SET(check, u64_hi, u64_low);

    /* Take the counter from the user */
    ts_counter = counter->ts_counter;

    if (COMPILER_64_GT(counter->ts_counter_ns, check)) {
        return BCM_E_PARAM;
    }


    if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE) || soc_is(unit, J2X_DEVICE)){


        /*Reset the offset since the counter is getting set*/
        SOC_IF_ERROR_RETURN(_bcm_dnx_time_ts_offset_reset(unit, ts_counter));

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_enable(unit, ts_counter, 0));

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_freq_frac_set(unit, ts_counter));

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_set(unit, ts_counter, counter->ts_counter_ns));

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_enable(unit, ts_counter, counter->enable));

        rv = BCM_E_NONE;
    } else if (soc_is(unit,JERICHO2_ONLY_DEVICE)){
        uint32 freq_ctrl = 0;
        freq_ctrl = (counter->time_format == bcmTimeFormatPTP) ?
            0x40000000 : 0x44B82FA1;

        if (ts_counter == 0) {
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr,
                    REG_PORT_ANY,
                    FRACf, freq_ctrl);

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /*
             * Adding 64bit upper 32 bits into 32 bit container.
             * then extracting only 16 bits.
             */
            COMPILER_64_TO_32_HI(u64_hi, counter->ts_counter_ns);
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr,
                    REG_PORT_ANY,
                    NSf, (u64_hi & 0xffff));

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Adding 64bit lower 32 bits into 32 bit container. */
            COMPILER_64_TO_32_LO(u64_low, counter->ts_counter_ns);
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr,
                    REG_PORT_ANY,
                    NSf, u64_low);

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Set the enable and disable control. */
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr,
                    REG_PORT_ANY,
                    ENABLEf, counter->enable);
        }
        else {
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_FRACr,
                    REG_PORT_ANY,
                    FRACf, freq_ctrl);

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /*
             * Adding 64bit upper 32 bits into 32 bit container.
             * then extracting only 16 bits.
             */
            COMPILER_64_TO_32_HI(u64_hi, counter->ts_counter_ns);
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr,
                    REG_PORT_ANY,
                    NSf, (u64_hi & 0xffff));

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Adding 64bit lower 32 bits into 32 bit container. */
            COMPILER_64_TO_32_LO(u64_low, counter->ts_counter_ns);
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr,
                    REG_PORT_ANY,
                    NSf, u64_low);

            if (BCM_FAILURE(rv)) {
                return rv;
            }

            /* Set the enable and disable control. */
            rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_COUNTER_ENABLEr,
                    REG_PORT_ANY,
                    ENABLEf, counter->enable);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_dnx_time_ts_counter_get
 * Purpose:
 *      Get timestamper counter
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      counter - (OUT)  counter values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_dnx_time_ts_counter_get(
        int unit,
        bcm_time_ts_counter_t  *counter)
{
    int rv = BCM_E_NONE;

    int ts_counter;


    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (counter == NULL) {
        return BCM_E_PARAM;
    }


    /* Read the counter from the user */
    ts_counter = counter->ts_counter;
    
    if (soc_is(unit, Q2A_DEVICE) || soc_is(unit, J2C_DEVICE) || soc_is(unit, J2P_DEVICE)){
        uint32 regval;

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_get(unit, ts_counter, &counter->ts_counter_ns));


        /* Read the enable status */
        if (ts_counter == 1) {
            READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
            counter->enable = soc_reg_field_get(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, regval, ENABLEf);
        } else if(ts_counter == 0) {
            READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            counter->enable = soc_reg_field_get(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, regval, ENABLEf);
        } else {
            return BCM_E_FAIL;
        }
    } else if (soc_is(unit,J2X_DEVICE)){
#if defined(BCM_J2X_SUPPORT)
        uint32 regval;

        SOC_IF_ERROR_RETURN(_bcm_dnx_time_counter_get(unit, ts_counter, &counter->ts_counter_ns));


        /* Read the enable status */
        if (ts_counter == 1) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, &regval);
            counter->enable = (soc_reg_field_get(unit, NSYNC_IEEE_1588_TIME_CONTROLr, regval, COUNTER_ENABLEf) & 0x02)?1:0;;
        } else if(ts_counter == 0) {
            BCM_TIME_READ_NS_REGr(unit, NSYNC_IEEE_1588_TIME_CONTROLr, 0, 0, &regval);
            counter->enable = (soc_reg_field_get(unit, NSYNC_IEEE_1588_TIME_CONTROLr, regval, COUNTER_ENABLEf) & 0x01)?1:0;;
        } else {
            return BCM_E_FAIL;
        }
#endif
    } else if (soc_is(unit,JERICHO2_ONLY_DEVICE)) 
    {
        uint32 freq_ctrl = 0;
        uint32 val_hi = 0, val_lo = 0;
        if (ts_counter == 0) {
            rv = READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &freq_ctrl);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            if (freq_ctrl == 0x40000000) {
                counter->time_format = bcmTimeFormatPTP;
            } else {
                counter->time_format = bcmTimeFormatNTP;
            }

            rv = READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &val_lo);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            rv = READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &val_hi);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            COMPILER_64_SET(counter->ts_counter_ns, val_hi, val_lo);

            val_lo = 0;
            rv = READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &val_lo);

            counter->enable = ((val_lo) ? 1 : 0);
        } else {
            rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_FRACr(unit, &freq_ctrl);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            if (freq_ctrl == 0x40000000) {
                counter->time_format = bcmTimeFormatPTP;
            } else {
                counter->time_format = bcmTimeFormatNTP;
            }

            rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr(unit, &val_lo);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr(unit, &val_hi);
            if(BCM_FAILURE(rv)) {
                return rv;
            }

            COMPILER_64_SET(counter->ts_counter_ns, val_hi, val_lo);

            val_lo = 0;
            rv = READ_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &val_lo);

            counter->enable = ((val_lo) ? 1 : 0);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_dnx_time_tod_set
 * Purpose:
 *      API to update tod values for stage.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      stages  - (IN)  Stage where time needs to be updated.
 *      tod     - (IN)  Time of the day values.
 * Returns:
 *      BCM_E_xxx
 */
int  bcm_dnx_time_tod_set(
        int unit,
        uint32 stages,
        bcm_time_tod_t *tod)
{
    uint32 sec;
    uint32 tmp;
    uint32 freq_step;
    uint32 ptp_freq_ctrl = 0, ntp_freq_ctrl = 0;
    uint64 frac_sec;
    int tod_config_in_eci = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(tod, _SHR_E_PARAM, "tod");

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit, "Configure TOD: (%d) sec (%d) ns\n"),
             COMPILER_64_LO(tod->ts.seconds), tod->ts.nanoseconds));

    tod_config_in_eci = dnx_data_oam.general.oam_tod_config_in_eci_get(unit);
    ptp_freq_ctrl = dnx_data_oam.general.oam_IEEE_1588_time_freq_control_get(unit);
    ntp_freq_ctrl = dnx_data_oam.general.oam_ntp_time_freq_control_get(unit);

    if (tod_config_in_eci)
    {
        sec = COMPILER_64_LO(tod->ts.seconds);

        if (stages & BCM_TIME_STAGE_LIVE_WIRE_PTP)
        {
            /* write 0 to control register */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_COUNT_ENABLEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_CONTROL_ACTIVEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ONCEf,  0));

            /* write value to frac sec lower register */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FRAC_SEC_LOWERf,
                        ((tod->ts.nanoseconds & 0x3F) << 26)));

            /* write value to frac sec upper register */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FRAC_SEC_UPPERf,
                        ((tod->ts.nanoseconds >> 6) & 0x3FFFFFF)));

            /* write value to frequency register (4 nS) */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FREQ_CONTROLf, ptp_freq_ctrl));

            /* write value to time sec register  */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_ONE_SECf, sec % 2));
            sec = (sec >> 1) & 0x7FFFFFFF;
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_UPPER_SECf, sec));
            /* write 0x2 to select bit 32 mode. */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_EVENT_MUX_CONTROLf, 0x2));
            /* write to control register to load values */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ONCEf,  1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_CONTROL_ACTIVEf,  1));
            /* clear load_once and time_control_active */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ONCEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_CONTROL_ACTIVEf,  0));
            /* write to control register to enable counter */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_COUNT_ENABLEf,  1));
        }
        else if (stages & BCM_TIME_STAGE_LIVE_WIRE_NTP)
        {
            /* write 0 to control register */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_COUNT_ENABLEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_TIME_CONTROL_ACTIVEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ONCEf,  0));

            /*
             * convert input nanoseconds to fractional second in (Counter26bit, Accum32bit)
             * frequecy step = (ptp_freq_control >> 26) & 0x3f. If clock is 256MHz, ptp_freq_control is 0x10000000, frequecy step is 4ns
             * frac_sec = nano_sec * (1 << 58) / 1000000000
             * ntp_freq_control = (1 << 58) / 1000000000 * ((ptp_freq_control >> 26) & 0x3f)
             * frac_sec =  nano_sec * ntp_freq_control / ((ptp_freq_control >> 26) & 0x3f)
             */
            COMPILER_64_SET(frac_sec, 0, tod->ts.nanoseconds);
            COMPILER_64_UMUL_32(frac_sec, ntp_freq_ctrl);
            freq_step = (ptp_freq_ctrl >> 26) & 0x3f;
            if (freq_step > 0)
            {
                COMPILER_64_UDIV_32(frac_sec, freq_step);
            }
            else
            {
                SHR_ERR_EXIT(BCM_E_INTERNAL, "Frequence step is incorrect.");
            }

            LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit, "frac : High(0x%x) Low(0x%x)\n"),
                     COMPILER_64_HI(frac_sec) & 0x3ffffff, COMPILER_64_LO(frac_sec)));

            /* write value to frac sec lower register */
            tmp = COMPILER_64_LO(frac_sec);
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FRAC_SEC_LOWERf, tmp));

            /* write value to frac sec upper register */
            tmp = COMPILER_64_HI(frac_sec);
            tmp &= 0x3ffffff;
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FRAC_SEC_UPPERf, tmp));

            /* write value to frequency register (4 nS in binary fraction) */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FREQ_CONTROLf, ntp_freq_ctrl));

            /* write value to time sec register  */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_ONE_SECf, sec % 2));
            sec = (sec >> 1) & 0x7FFFFFFF;
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_UPPER_SECf, sec));
            /* write 0x2 to select bit 32 mode. */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_EVENT_MUX_CONTROLf, 0x2));

            /* write to control register to load values */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ONCEf,  1));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_TIME_CONTROL_ACTIVEf,  1));

            /* clear load_once and time_control_active */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ONCEf,  0));
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_TIME_CONTROL_ACTIVEf,  0));
            /* write to control register to enable counter */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                        ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_COUNT_ENABLEf,  1));
        }
        else
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Stages are unsupported.");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TOD configuration via ECI is not supported. Please try bcm_time_tod_set().\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_time_tod_get
 * Purpose:
 *      Internal function to get tod values for stage.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      tod     - (OUT) Time of the day values.
 *      stage   - (IN)  Stage where time is from.
 * Returns:
 *      BCM_E_xxx
 */
int  bcm_dnx_time_tod_get(
        int unit,
        uint32 stages,
        bcm_time_tod_t *tod)
{
    uint64 field64;
    int tod_config_in_eci = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(tod, _SHR_E_PARAM, "tod");

    tod_config_in_eci = dnx_data_oam.general.oam_tod_config_in_eci_get(unit);

    if (tod_config_in_eci)
    {
        if (stages & BCM_TIME_STAGE_LIVE_WIRE_PTP)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_field64_read(unit,
                        IPPD_TOD_LAST_VALUEr, REG_PORT_ANY, 0, TOD_1588_LAST_VALUEf, &field64));
            tod->ts.nanoseconds = COMPILER_64_LO(field64);
            COMPILER_64_SET(tod->ts.seconds, 0, COMPILER_64_HI(field64));
            tod->time_format = bcmTimeFormatPTP;
        }
        else if (stages & BCM_TIME_STAGE_LIVE_WIRE_NTP)
        {
            uint64 tmp;
            uint64 frac_sec;

            /*
             * the whole TOD value is Counter31bit, Counter1bit, Counter26bit, Accum32bit[31:26]
             * seconds(32bits) + fractional seconds
             */
            SHR_IF_ERR_EXIT(soc_reg_above_64_field64_read(unit,
                        IPPD_TOD_LAST_VALUEr, REG_PORT_ANY, 0, TOD_NTP_LAST_VALUEf, &field64));

            /*
             * Get the lower 32-bits fractional second and convert it to actual nanoseconds
             * Actual nanoseconds = (frac_sec << 26) * 1000000000 / (1 << 58) = frac_sec * 1000000000 / (1 << 32)
             */
            COMPILER_64_SET(frac_sec, 0, COMPILER_64_LO(field64));
            COMPILER_64_UMUL_32(frac_sec, 1000000000);
            COMPILER_64_SET(tmp, 0x1, 0);
            COMPILER_64_UDIV_64(frac_sec, tmp);

            tod->ts.nanoseconds = COMPILER_64_LO(frac_sec);
            COMPILER_64_SET(tod->ts.seconds, 0, COMPILER_64_HI(field64));
            tod->time_format = bcmTimeFormatNTP;
        }
        else
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Stages are unsupported.");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TOD configuration via ECI is not supported. Please try bcm_time_tod_get().\n");
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit, "Configure NTP: (%d) sec (%d) ns\n"),
             COMPILER_64_LO(tod->ts.seconds), tod->ts.nanoseconds));

exit:
    SHR_FUNC_EXIT;
}
#endif /* #if defined(BCM_DNX_SUPPORT) */
