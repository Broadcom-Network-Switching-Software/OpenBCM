/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port_test1588.c
 * Purpose:     Functions to support CLI 1588 testing commands
 * Requires:
 */

/* #define INCLUDE_TIMESYNC_DVT_TESTS */

#ifdef  INCLUDE_TIMESYNC_DVT_TESTS

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>

#include <shared/bsl.h>

#if !(defined(LINUX) || defined(UNIX))
#define  strcasecmp    strcmp
#define  strncasecmp   strncmp
#endif  /* UNIX || LINUX */

#include <assert.h>
#include <soc/debug.h>

#if defined(BCM_ESW_SUPPORT)
#include <soc/xaui.h>
#endif
#include <soc/phyctrl.h>
#include <soc/phy.h>
#include <soc/phy/phymod_sim.h>
#include <soc/eyescan.h>

#if defined(PHYMOD_SUPPORT)
#include <appl/diag/phymod/phymod_symop.h>
#include <soc/phyreg.h>
#include <soc/phy/phymod_ctrl.h>
#include <phymod/phymod_reg.h>
#include <phymod/phymod_debug.h>
#endif

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#include <bcm/init.h>
#include <bcm/port.h>
#include <bcm/stg.h>
#include <bcm/error.h>

#include <bcm_int/control.h>

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/port.h>
#endif

void _print_heartbeat_ts(int unit, bcm_port_t port);
void _print_capture_ts(int unit, bcm_port_t port);

uint64
_compiler_64_arg(uint32 hi, uint32 low)
{
    uint64 val;
    COMPILER_64_SET(val, hi, low);
    return val;
}

void
heartbeat_trace(int unit, bcm_port_t port, int msec, int iter) {
    int     accelo, ii;
    int64   accel;
    uint64  delta, present, prev;

    COMPILER_64_SET(accel, 0, 0);
    COMPILER_64_SET(prev , 0, 0);
    COMPILER_64_SET(delta, 0, 0);

    for ( ii = 0; ii < iter; ++ii ) {
        bcm_port_control_phy_timesync_get(unit, port,
                      bcmPortControlPhyTimesyncHeartbeatTimestamp, &present);
        if ( ! COMPILER_64_IS_ZERO(prev) ) {
            COMPILER_64_SET(delta, COMPILER_64_HI(present),
                                   COMPILER_64_LO(present));
            COMPILER_64_SUB_64(delta, prev);      /* delta = present - prev */
        }
        if ( ! (COMPILER_64_IS_ZERO(delta) || COMPILER_64_IS_ZERO(accel)) ) {
            COMPILER_64_SUB_64(accel, delta);     /* accel = delta's delta  */
        }
        accelo = COMPILER_64_LO(accel);
        cli_out("Heartbeat=%08x_%08x  delta=%08x_%08x  accel = %c%d\n",
                COMPILER_64_HI(present), COMPILER_64_LO(present),
                COMPILER_64_HI(delta)  , COMPILER_64_LO(delta),
                (accelo == 0) ? ' ' : (accelo > 0) ? '+' : '\0', accelo );

        COMPILER_64_SET(prev, COMPILER_64_HI(present),
                              COMPILER_64_LO(present));    /* prev = present */
        if ( ! COMPILER_64_IS_ZERO(delta) ) {
            COMPILER_64_SET(accel, COMPILER_64_HI(delta),
                                   COMPILER_64_LO(delta));  /* accel = delta */
        }
        sal_msleep(msec);
    }
}


void                                          /* derived from  config_tc6p1() */
config_syncout(int unit, bcm_port_t port,
               bcm_port_phy_timesync_syncout_mode_t mode,
               int pulse1, int pulse2, int interval)
{
    bcm_port_phy_timesync_config_t config;
    int     rv;
    uint64 *time;
    uint32  value = ~(BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD);

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, value));
    if ( rv != BCM_E_NONE ) {
        cli_out("bcm_port_control_phy_timesync_set() failed "
                "with error  u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
    }

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
    }
    time = &(config.syncout.syncout_ts);

    rv = bcm_port_control_phy_timesync_get(unit, port,
                bcmPortControlPhyTimesyncHeartbeatTimestamp, time);
    cli_out("Heartbeat TS = %08x%08x\n",
            COMPILER_64_HI(*time), COMPILER_64_LO(*time));

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
    config.gmode                  = bcmPortPhyTimesyncModeCpu;
    config.syncout.mode           = mode;
    config.syncout.pulse_1_length = pulse1;
    config.syncout.pulse_2_length = pulse2;
    config.syncout.interval       = interval;
    /* add a time offset value to  config.syncout.syncout_ts  in order to     */
    COMPILER_64_ADD_32(*time, 0xf0000000L);   /* generate the FrameSync pulse */

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
    }
    /* sal_msleep(2200);       COMPILER_64_ZERO(config.syncout.syncout_ts); */

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
    }
    cli_out(" SYNCOUT  TS = %08x%08x\n",
            COMPILER_64_HI(*time), COMPILER_64_LO(*time));

    _print_heartbeat_ts(unit, port);
}


/* Test case 1 */
void
config_i1e1_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;

    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_i1e1_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get "
                "failed with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE | 
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;

    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

/* Test case 2 */
void
config_i0e2_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;

    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_i0e2_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin;

    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.original_timecode.seconds = _compiler_64_arg(0, 0xbeef8000);

    config.original_timecode.nanoseconds = 0xaaaabbbb;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

/* Test case 3 */
void
config_i2e0_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp;
    /* config.gmode = bcmPortPhyTimesyncModeCpu; */
    config.gmode = bcmPortPhyTimesyncModeFree;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_i2e0_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;
    /* config.gmode = bcmPortPhyTimesyncModeCpu; */
    config.gmode = bcmPortPhyTimesyncModeFree;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_tc5p1(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.mode = bcmPortPhyTimesyncSyncoutOneTime;
    config.syncout.pulse_1_length = 20000;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_tc6p1(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrain;
    config.syncout.pulse_1_length = 20000;
    config.syncout.pulse_2_length = 10000;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_tc7p1(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrainWithSync;
    config.syncout.pulse_1_length = 20000;
    config.syncout.pulse_2_length = 10000;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_tc8p1(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin0;
    config.framesync.length_threshold = 20000;
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

/* Test case 1: MPLS label 1 : all ptp pkts  */
void
config_i1e1_all_pkts_mpls1_en_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;
    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
     * sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */

}

void
config_i1e1_all_pkts_mpls1_en_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE | 
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
     sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

/* Test case 2 : MPLS label 1 : all ptp pkts  */
void
config_i0e2_all_pkts_mpls1_en_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_delay_request_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_i0e2_all_pkts_mpls1_en_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE |
            BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
     sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin;
    /*config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone; */

    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.original_timecode.seconds = _compiler_64_arg(0, 0xbeef8000);
    config.original_timecode.nanoseconds = 0xaaaabbbb;

    config.framesync.mode =
        bcmPortPhyTimesyncFramesyncCpu /*bcmPortPhyTimesyncFramesyncSyncin0 */ ;
    config.framesync.length_threshold = 0x4;
    config.framesync.event_offset = 0x8;

    config.gmode = bcmPortPhyTimesyncModeCpu;

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

/* Test case 3 : MPLS label 1 : all ptp pkts  */
void
config_i2e0_all_pkts_mpls1_en_in(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
     sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp;

    config.tx_delay_request_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp;

    /* config.gmode = bcmPortPhyTimesyncModeCpu; */
    config.gmode = bcmPortPhyTimesyncModeFree;

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_i2e0_all_pkts_mpls1_en_out(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE |
            BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
            BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_delay_request_mode = bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_delay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_request_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

    config.tx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageEgressModeNone;
    config.rx_pdelay_response_mode =
        bcmPortPhyTimesyncEventMessageIngressModeNone;

#if 0
    config.gmode = bcmPortPhyTimesyncModeCpu;
#else
    config.gmode = bcmPortPhyTimesyncModeFree;
#endif

    config.mpls_control.flags = BCM_PORT_PHY_TIMESYNC_MPLS_ENABLE
        | BCM_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_tc5p1_fs(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.interval = 0x00000080;
    config.syncout.pulse_1_length = 0x0020;
    config.syncout.pulse_2_length = 0x040;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.syncout.mode = bcmPortPhyTimesyncSyncoutOneTime;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncCpu;
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0,
                                                            bcmPortPhyTimesyncFramesyncSyncin0));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_tc6p1_fs(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.interval = 0x00000080;
    config.syncout.pulse_1_length = 0x0020;
    config.syncout.pulse_2_length = 0x040;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrain;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncCpu;
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0,
                                                            bcmPortPhyTimesyncFramesyncSyncin0));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_tc7p1_fs(int unit, bcm_port_t port, int arg1, int arg2)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size * 
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.syncout.interval = 0x00000080;
    config.syncout.pulse_1_length = 0x0020;
    config.syncout.pulse_2_length = 0x040;
    config.syncout.syncout_ts = _compiler_64_arg(0, 80000);
    config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrainWithSync;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncCpu;
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0,
                                                            bcmPortPhyTimesyncFramesyncSyncin0));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
config_tc8p1_fs(int unit, bcm_port_t port, int fs_mode, int fs_threshold, int arg3)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE;

    config.mpls_control.size = 1;
    /* config.mpls_control.labels = sal_alloc(config.mpls_control.size *
       sizeof(_shr_port_phy_timesync_mpls_label_t), "timesync_mpls_label"); */
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    config.framesync.mode = fs_mode;       /* bcmPortPhyTimesyncFramesyncSyncin0; */
    config.framesync.length_threshold = fs_threshold;   /* 20000; */
    config.gmode = bcmPortPhyTimesyncModeCpu;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
/*
    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
*/
    /* if(config.mpls_control.labels != NULL) {
       sal_free(config.mpls_control.labels);
       } */
}

void
cpu_sync(int unit, bcm_port_t port)
{
    uint64 value;
    int rv;

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0, 1));
                                           /*_compiler_64_arg(0, 0x9));*/

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           &value);
    cli_out("%s() u=%d p=%d FSmode=%08x:%08x\n", __func__,
           unit, port, COMPILER_64_HI(value), COMPILER_64_LO(value));
}

void
set_localtime(int unit, bcm_port_t port, uint64 localtime)
{
    int rv;

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLocalTime,
                                           localtime);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

}

void
print_local_time(int unit, bcm_port_t port)
{
    int rv;
    uint64 time;

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncLocalTime,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Current Local Time u=%d p=%d  Time = %08x%08x\n",
            unit, port, COMPILER_64_HI(time), COMPILER_64_LO(time));
}

void
test1588(int port1, int port2) /* , int fsmode) */
{

    config_i2e0_in(0, port1);
    config_i2e0_out(0, port2);

    set_localtime(0, port1, _compiler_64_arg(0xabcdef33, 0x44556677));
    set_localtime(0, port2, _compiler_64_arg(0xabcdef99, 0x44556688));
    cpu_sync(0, port1);
    cpu_sync(0, port2);

    print_local_time(0, port1);
    print_local_time(0, port2);
    _print_heartbeat_ts(0, port1);
    _print_capture_ts(0, port1);
    _print_heartbeat_ts(0, port2);
    _print_capture_ts(0, port2);
}

void
test1588_1(int port1, int port2)
{
    config_i1e1_in(0, port1);
    config_i1e1_out(0, port2);
    cpu_sync(0, port1);
    cpu_sync(0, port1);
}

void
test1588_2(int port1, int port2)
{
    config_i0e2_in(0, port1);
    config_i0e2_out(0, port2);
    cpu_sync(0, port1);
    cpu_sync(0, port1);
}

void
test1588_3(int port1, int port2)
{
    config_i2e0_in(0, port1);
    config_i2e0_out(0, port2);
    cpu_sync(0, port1);
    cpu_sync(0, port1);
}

void
test1588_4(int port1, int port2)
{
    config_i0e2_in(0, port1);
    config_i0e2_in(0, port2);
    cpu_sync(0, port1);
    cpu_sync(0, port1);
}

void
test1588_5(int port1, int arg1, int arg2)
{
    config_tc5p1(0, port1, arg1, arg2);
    cpu_sync(0, port1);
}

void
test1588_6(int port1, int arg1, int arg2, int arg3)
{
    config_syncout(0, port1, bcmPortPhyTimesyncSyncoutPulseTrain,
                   arg1, arg2, arg3);
    cpu_sync(0, port1);
}

void
test1588_7(int port1, int arg1, int arg2)
{
    config_tc7p1(0, port1, arg1, arg2);
    cpu_sync(0, port1);
}

void
test1588_8(int port1, int arg1, int arg2)
{
    config_tc8p1(0, port1, arg1, arg2);
    cpu_sync(0, port1);
}

void
test1588_1_2(int port1, int port2)
{
    config_i1e1_all_pkts_mpls1_en_in(0, port1);
    config_i1e1_all_pkts_mpls1_en_out(0, port2);
}

void
test1588_2_2(int port1, int port2)
{
    config_i0e2_all_pkts_mpls1_en_in(0, port1);
    config_i0e2_all_pkts_mpls1_en_out(0, port2);
}

void
test1588_3_2(int port1, int port2)
{
    config_i2e0_all_pkts_mpls1_en_in(0, port1);
    config_i2e0_all_pkts_mpls1_en_out(0, port2);
}

void
test1588_4_2(int port1, int port2)
{
    config_i0e2_all_pkts_mpls1_en_in(0, port1);
    config_i0e2_all_pkts_mpls1_en_in(0, port2);
}

/*
void
test1588_5_2(int port1)
{
    config_tc5p1_fs(0, port1);
}

void
test1588_6_2(int port1)
{
    config_tc6p1_fs(0, port1);
}

void
test1588_7_2(int port1)
{
    config_tc7p1_fs(0, port1);
}
*/

void
test1588_8_2(int port1, int arg1, int arg2, int arg3)
{
    config_tc8p1_fs(0, port1, arg1, arg2, arg3);
}

void
config_fs_test(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;
    uint64 time, temp;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0, 1));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncHeartbeatTimestamp,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Heartbeat TS = %08x%08x\n",
            COMPILER_64_HI(time), COMPILER_64_LO(time));

    /* Add delta of 4.3 sec */
    temp = _compiler_64_arg(0x1, 0);
    COMPILER_64_ADD_64(time, temp);
    config.syncout.syncout_ts = time;

    cli_out("Syncout   TS = %08x%08x\n",
            COMPILER_64_HI(config.syncout.syncout_ts),
            COMPILER_64_LO(config.syncout.syncout_ts));

    config.syncout.interval = 0x00000080;
    config.syncout.pulse_1_length = 25000;
    config.syncout.pulse_2_length = 0x040;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutOneTime;
    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin0;
    config.framesync.length_threshold = 20000;

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_SYNCOUT_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0, 1));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncHeartbeatTimestamp,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Heartbeat TS = %08x%08x\n",
            COMPILER_64_HI(time), COMPILER_64_LO(time));

    sal_usleep(5000000);

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0, 1));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncHeartbeatTimestamp,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Heartbeat TS = %08x%08x\n",
            COMPILER_64_HI(time), COMPILER_64_LO(time));

#if 0
    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    config.mpls_control.size = 1;
    config.mpls_control.labels =
        sal_alloc(config.mpls_control.size *
                  sizeof(_shr_port_phy_timesync_mpls_label_t),
                  "timesync_mpls_label");
    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.flags |=
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0, 0xffffffff));

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0,
                                                            bcmPortPhyTimesyncFramesyncSyncin0));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    if (config.mpls_control.labels != NULL) {
        sal_free(config.mpls_control.labels);
    }
#endif
}

void
config_dpll_sync(int unit, bcm_port_t port,
                 uint32 ref_phz_d, uint32 k1, uint32 k2, uint32 k3)
{
    int rv;
    bcm_port_phy_timesync_config_t config;
    sal_memset(&config, 0, sizeof(config));

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3;

    config.flags =
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin1;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    /* for 1KHz SyncIn */
    config.phy_1588_dpll_ref_phase       = _compiler_64_arg(0, 0);
    config.phy_1588_dpll_ref_phase_delta = ref_phz_d;
    config.phy_1588_dpll_k1              = k1;
    config.phy_1588_dpll_k2              = k2;
    config.phy_1588_dpll_k3              = k3;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set( unit, port,
               bcmPortControlPhyTimesyncLocalTime,
               _compiler_64_arg(0x0, 0x0) );

    /* Shadow Register Load ,  do NOT set  LOCAL_TIME_ALWAYS_LOAD */
    rv = bcm_port_control_phy_timesync_set( unit, port, 
               bcmPortControlPhyTimesyncLoadControl,
               _compiler_64_arg(0,
                        BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD |
                        BCM_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD) );
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_port_sync_4k(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    sal_memset(&config, 0, sizeof(config));

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS |
        BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3;

    config.flags =
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin1;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    /* for 4KHz SyncIn */
    config.phy_1588_dpll_ref_phase = _compiler_64_arg(0, 0);
    config.phy_1588_dpll_ref_phase_delta = 0x0003d090;
    config.phy_1588_dpll_k1 = 0x2a;
    config.phy_1588_dpll_k2 = 0x26;
    config.phy_1588_dpll_k3 = 0;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLocalTime,
                                           _compiler_64_arg(0x0, 0x0));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                           BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                                           BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                                           BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                                           BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                                           BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                                           BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_port_sync_12_5m(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    sal_memset(&config, 0, sizeof(config));

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3;

    config.flags =
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin1;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    /* for 12.5MHz SyncIn */
    config.phy_1588_dpll_ref_phase = _compiler_64_arg(0, 0);
    config.phy_1588_dpll_ref_phase_delta = 80;
    config.phy_1588_dpll_k1 = 43;
    config.phy_1588_dpll_k2 = 27;
    config.phy_1588_dpll_k3 = 0;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLocalTime,
                                           _compiler_64_arg(0x0, 0x0));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

}

void
config_port_syncout_1k(int unit, bcm_port_t port, int mode)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    sal_memset(&config, 0, sizeof(config));

    config.validity_mask =
        BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3;

    config.flags =
        BCM_PORT_PHY_TIMESYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE |
        BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLocalTime,
                                           _compiler_64_arg(0xa33, 0x44556677));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.syncout.syncout_ts = _compiler_64_arg(0x1, 0x00000000);
    config.syncout.interval = 25000;
    config.syncout.pulse_1_length = 0x800;
    config.syncout.pulse_2_length = 0x400;

    config.gmode = bcmPortPhyTimesyncModeFree;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncNone;

    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode =
        bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode =
        bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;

    /* for 1KHz SyncIn */
    config.phy_1588_dpll_ref_phase = _compiler_64_arg(0, 0);
    config.phy_1588_dpll_ref_phase_delta = 0x000f4240;
    config.phy_1588_dpll_k1 = 0x2a;
    config.phy_1588_dpll_k2 = 0x28;
    config.phy_1588_dpll_k3 = 0;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncLoadControl,
                                           _compiler_64_arg(0,
                                                            BCM_PORT_PHY_TIMESYNC_TN_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_SYNCOUT_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                                                            BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_timesync_set(unit, port,
                                           bcmPortControlPhyTimesyncFrameSync,
                                           _compiler_64_arg(0, 1));

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncNone;

    if (mode == 3) {
        config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrainWithSync;
    } else if (mode == 2) {
        config.syncout.mode = bcmPortPhyTimesyncSyncoutPulseTrain;
    } else {
        config.syncout.mode = bcmPortPhyTimesyncSyncoutOneTime;
    }

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

}

void
sync_clock_1k(int port1)
{
    /* config_port_sync_1k(0, port1); */
    config_dpll_sync(0, port1, 0xf4240, 0x2a, 0x28, 0);

    print_local_time(0, port1);
    _print_heartbeat_ts(0, port1);
    _print_capture_ts(0, port1);
}

void
sync_clock_4k(int port1)
{
    config_port_sync_4k(0, port1);

    print_local_time(0, port1);
    _print_heartbeat_ts(0, port1);
    _print_capture_ts(0, port1);
}

void
sync_clock_12_5m(int port1)
{
    config_port_sync_12_5m(0, port1);

    print_local_time(0, port1);
    _print_heartbeat_ts(0, port1);
    _print_capture_ts(0, port1);
}

void
config_inband_ts_tc_ip_cap(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.inband_ts_control.flags =
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_RESV0_ID_CHECK |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_SYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_DELAY_RQ_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_PDELAY_RQ_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_PDELAY_RESP_ENABLE;
    config.inband_ts_control.resv0_id = 0xf000 | port;

    config.flags |= BCM_PORT_PHY_TIMESYNC_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

void
config_inband_ts_tc_da_cap(int unit, bcm_port_t port)
{
    int rv;
    bcm_port_phy_timesync_config_t config;

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS;

    rv = bcm_port_phy_timesync_config_get(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_get failed "
                "with error u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    config.inband_ts_control.flags =
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_RESV0_ID_CHECK |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_SYNC_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_DELAY_RQ_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_PDELAY_RQ_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_PDELAY_RESP_ENABLE |
        BCM_PORT_PHY_TIMESYNC_INBAND_TS_MATCH_MAC_ADDR;
    config.inband_ts_control.resv0_id = 0xf000 | port;

    config.flags |= BCM_PORT_PHY_TIMESYNC_ENABLE;

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_phy_timesync_config_set failed "
                "with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }
}

/* PHY OAM tests */

/* Y.1731 */
void
phy_oam_y1731_rx(int unit, int port)
{
    int rv;
    bcm_port_config_phy_oam_t config;

    sal_memset(&config, 0, sizeof(soc_port_config_phy_oam_t));

    config.rx_dm_config.flags = 0;
    config.rx_dm_config.mode = bcmPortConfigPhyOamDmModeY1731;
    rv = bcm_port_config_phy_oam_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_config_phy_oam_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmMacAddress1,
                                      _compiler_64_arg(0x00000000, 0x00000100));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d)%s\n",
                unit, port, bcmPortControlPhyOamDmMacAddress1, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmRxPortMacAddressIndex,
                                      _compiler_64_arg(0, 0x1));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmRxPortMacAddressIndex,
                bcm_errmsg(rv));
    }
}

void
phy_oam_y1731_tx(int unit, int port)
{
    int rv;
    bcm_port_config_phy_oam_t config;

    sal_memset(&config, 0, sizeof(soc_port_config_phy_oam_t));

    config.tx_dm_config.flags = 0;
    config.tx_dm_config.mode = bcmPortConfigPhyOamDmModeY1731;
    rv = bcm_port_config_phy_oam_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_config_phy_oam_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmMacAddress1,
                                      _compiler_64_arg(0x0000a000, 0x00000100));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d)%s\n",
                unit, port, bcmPortControlPhyOamDmMacAddress1, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmTxPortMacAddressIndex,
                                      _compiler_64_arg(0, 0x1));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmTxPortMacAddressIndex,
                bcm_errmsg(rv));
    }
}

void
phy_oam_y1731_mac_da_rx(int unit, int port)
{
    int rv;
    bcm_port_config_phy_oam_t config;

    sal_memset(&config, 0, sizeof(soc_port_config_phy_oam_t));

    config.rx_dm_config.flags = BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
    config.rx_dm_config.mode = bcmPortConfigPhyOamDmModeY1731;
    rv = bcm_port_config_phy_oam_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_config_phy_oam_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmMacAddress1,
                                      _compiler_64_arg(0x00000000, 0x00000100));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d)%s\n",
                unit, port, bcmPortControlPhyOamDmMacAddress1, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmRxPortMacAddressIndex,
                                      _compiler_64_arg(0, 0x1));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmRxPortMacAddressIndex,
                bcm_errmsg(rv));
    }
}

void
phy_oam_y1731_mac_sa_tx(int unit, int port)
{
    int rv;
    bcm_port_config_phy_oam_t config;

    sal_memset(&config, 0, sizeof(soc_port_config_phy_oam_t));

    config.tx_dm_config.flags = BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
    config.tx_dm_config.mode = bcmPortConfigPhyOamDmModeY1731;
    rv = bcm_port_config_phy_oam_set(unit, port, &config);
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_config_phy_oam_set failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmMacAddress2,
                                      _compiler_64_arg(0x0000a000, 0x00000100));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d)%s\n",
                unit, port, bcmPortControlPhyOamDmMacAddress1, bcm_errmsg(rv));
    }

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmTxPortMacAddressIndex,
                                      _compiler_64_arg(0, 0x2));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmTxPortMacAddressIndex,
                bcm_errmsg(rv));
    }
}

void
phy_oam_ctrl_tx_ethtype(int unit, int port)
{
    int rv;

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmTxEthertype,
                                      _compiler_64_arg(0, 0xaaaa));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmTxEthertype, bcm_errmsg(rv));
    }

}

void
phy_oam_ctrl_rx_ethtype(int unit, int port)
{
    int rv;

    rv = bcm_port_control_phy_oam_set(unit, port,
                                      bcmPortControlPhyOamDmRxEthertype,
                                      _compiler_64_arg(0, 0xbbbb));
    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_oam_set failed "
                "with error  u=%d p=%d (type %d) %s\n",
                unit, port, bcmPortControlPhyOamDmRxEthertype, bcm_errmsg(rv));
    }
}

void
phy_oam_tests(void)
{
    cli_out(" \n\tTests:\n");
    cli_out("1.1. phy_oam_y1731_rx\n");
    cli_out("1.2. phy_oam_y1731_tx\n");
    cli_out("1.3. phy_oam_y1731_mac_da_rx\n");
    cli_out("1.4. phy_oam_y1731_mac_sa_tx\n");
    cli_out("1.5. phy_oam_ctrl_rx_ethtype\n");
    cli_out("1.6. phy_oam_ctrl_tx_ethtype\n");
}


cmd_result_t
phy_test1588(int unit, args_t *args, soc_pbmp_t *pbm)
{
    char       *cmd;
    soc_port_t  p, dport;

    int  iter=0, test_case = 0, arg1=0, arg2=0, arg3=0, arg4=0;
    soc_port_t  p01=0, p02=0;

    if (((cmd = ARG_GET(args)) == NULL) || (parse_bcm_pbmp(unit, cmd, pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(args), cmd);
        return CMD_FAIL;
    }

    if ((cmd = ARG_GET(args)) != NULL) {
        if ( (! strcmp(cmd, "syncout")) || (! strncmp(cmd, "so", 2)) ) {
            test_case = 350;          /* SyncOUT mode test */
        } else if ( (! strcmp(cmd, "framesync")) || (! strncmp(cmd, "fs", 2)) ) {
            test_case = 382;          /* FrameSync mode test */
        } else if ( (! strncmp(cmd, "dp", 2)) ) {
            test_case = 30;           /* DPLL test */
        } else {
            test_case = parse_integer(cmd);
        }
    }

    if ((cmd = ARG_GET(args)) != NULL) {
        arg1 = parse_integer(cmd);
    }
    if ((cmd = ARG_GET(args)) != NULL) {
        arg2 = parse_integer(cmd);
    }
    if ((cmd = ARG_GET(args)) != NULL) {
        arg3 = parse_integer(cmd);
    }
    if ((cmd = ARG_GET(args)) != NULL) {
        arg4 = parse_integer(cmd);
    }

    DPORT_SOC_PBMP_ITER(unit, *pbm, dport, p) {
        ++iter;
        if      ( 1 == iter )    p01 = p;
        else if ( 2 == iter )    p02 = p;
        else                     break;
    }
    cli_out("================ "
            "1588 unit test #%d (port %d, %d) arg= %d %d %d %d\n",
            test_case, p01,p02, arg1, arg2, arg3, arg4);

    switch ( test_case ) {
        case 0 :
            /* _print_heartbeat_ts(unit, p01); _print_capture_ts(unit, p01);   */
            heartbeat_trace(unit, p01, arg1, arg2);
            break;
        case  10 :
            test1588(p01, p02);      break;
        case  1 :
            test1588_1(p01, p02);    break;
        case  2 :
            test1588_2(p01, p02);    break;
        case  3 :
            test1588_3(p01, p02);    break;
        case  4 :
            test1588_4(p01, p02);    break;
        case  5 :
            config_syncout(0, p01, bcmPortPhyTimesyncSyncoutOneTime,
                           arg1, arg2, arg3);
            cpu_sync(0, p01);
            break;
        case  6 :
            config_syncout(0, p01, bcmPortPhyTimesyncSyncoutPulseTrain,
                           arg1, arg2, arg3);
            cpu_sync(0, p01);
            break;
        case  7 :
            config_syncout(0, p01, bcmPortPhyTimesyncSyncoutPulseTrainWithSync,
                           arg1, arg2, arg3);
            cpu_sync(0, p01);
            break;
        case  8 :
            test1588_8(p01, arg1, arg2);     break;

        case  20 :
            config_fs_test(unit, p01);       break;
        case  30 :
            if ( 1 == arg1 ) {  /* DPLL test */
                config_dpll_sync(unit, p01, 0xf4240, 0x2a, 0x28, 0); /* 1K Sync */
            } else if ( 4 == arg1 ) {
                config_dpll_sync(unit, p01, 0x3d090, 0x2a, 0x26, 0); /* 4K Sync */
            } else {
                config_dpll_sync(unit, p01, 80, 43, 27, 0);       /* 12.5M Sync */
            }
            break;

        case  102 :
            config_i0e2_in(unit, p01);       break;
        case  202 :
            config_i0e2_out(unit, p01);      break;
        case  111 :
            config_i1e1_in(unit, p01);       break;
        case  211 :
            config_i1e1_out(unit, p01);      break;
        case  120 :
            config_i2e0_in(unit, p01);       break;
        case  220 :
            config_i2e0_out(unit, p01);      break;

        case  350 :          /* SyncOUT mode test */
            config_syncout(0, p01, arg1, arg2, arg3, arg4);
            cpu_sync(0, p01);
            break;
        case  382 :          /* FrameSYNC mode test */
            test1588_8_2(p01, arg1, arg2, arg3);    break;

        default:
            cli_out("---- Unknown case #%d !\n", test_case);
            return CMD_OK;
    }

    /* cli_out("================ done.\n"); */
    return CMD_OK;
}

#else

#include <soc/types.h>
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

cmd_result_t
phy_test1588(int unit, args_t *args, soc_pbmp_t *pbm)
{
    return CMD_NOTIMPL;
}

#endif  /* INCLUDE_TIMESYNC_DVT_TESTS */
