/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        gdpll_shared.h
 * Purpose:     Shared GDPLL header file between M7 and the host
 */

#ifndef __BCM_INT_GDPLL_SHARED_H__
#define __BCM_INT_GDPLL_SHARED_H__

#if defined(INCLUDE_GDPLL)

/* M7 DTCM region references */
#define DPLL_ALIVE_SIZE                 4   /* This is the M7 Alive Indication */
#define DPLL_FW_VERSION_SIZE            40  /* This is the M7 version size */
#define DPLL_DEBUG_ENABLE_SIZE          4   /* This is the M7 switch for debug push */
#define DPLL_PARAM_SIZE                 236 /* This size is given to accomodate max 3 states */
#define DPLL_GENERIC_SIZE               (1024*2)

/* For any offset that gets added for versioning, etc */
#define DPLL_ALIVE_OFFSET               0   /* This is the M7 Alive Indication */
#define DPLL_FW_VERSION_OFFSET          (DPLL_ALIVE_OFFSET + DPLL_ALIVE_SIZE)  /* This is the M7 version start */
#define DPLL_BCN_UPDATE_LOC             (DPLL_FW_VERSION_OFFSET + DPLL_FW_VERSION_SIZE)  /* Location used by R5 and Host for BCN counter */
#define DPLL_NTPTOD_UPDATE_LOC          (DPLL_BCN_UPDATE_LOC + 4)     /* Location used by R5 and Host for NTPTOD counter updates */
#define DPLL_PTPTOD_UPDATE_LOC          (DPLL_NTPTOD_UPDATE_LOC + 4)  /* Location used by R5 and Host for PTPTOD counter updates */

#define DPLL_DEBUG_ENABLE_OFFSET        (DPLL_GENERIC_SIZE)/*200*/   /* This is the M7 switch for debug push */
#define DPLL_PARAM_START_OFFSET         (DPLL_DEBUG_ENABLE_OFFSET + DPLL_DEBUG_ENABLE_SIZE)
#define DPLL_CHAN_PARAM_ADDR(base, chan) \
    ((base + DPLL_PARAM_START_OFFSET) + (DPLL_PARAM_SIZE * chan))

#define DPLL_VERSION_02

#if defined(DPLL_VERSION_01)

#define DPLL_DEBUG_MODE              (0)    /* Channel debug flag offset, start */
/**********************************************************
 * Following are per channel data structure offsets
 * Refer dpllWrapper.h from the M7 firmware
 */
#define SIZEOF_DEBUGMODE        (4)
#define SIZEOF_DPLLNUMSTATE     (1)
#define SIZEOF_K1               (2)
#define SIZEOF_K1SHIFT          (1)
#define SIZEOF_K1K2             (2)
#define SIZEOF_K1K2SHIFT        (1)
#define SIZEOF_K1K3             (2)
#define SIZEOF_K1K3SHIFT        (1)
#define SIZEOF_LOCKTHETHRE      (4)
#define SIZEOF_DWELLCNT         (1)
#define SIZEOF_PELTHR           (8)
#define SIZEOF_FLLDEC           (1)
#define SIZEOF_NCOBITS          (1)
#define SIZEOF_PHASECNTREF      (1)
#define SIZEOF_DUALLOOPFLAG     (1)
#define SIZEOF_PHASECNTDELTA    (8)

#define SIZEOF_STATE        (1)
#define SIZEOF_LOOPF        (8)
#define SIZEOF_DWELLCOUNTER (4)
#define SIZEOF_LOCKDETF     (4)
#define SIZEOF_PERROR       (8)
#define SIZEOF_FERROR       (8)
#define SIZEOF_FLLCNT       (1)
#define SIZEOF_1STLOOP      (1)
#define SIZEOF_INITFLAG     (1)
#define SIZEOF_PHASECNT     (8)

/* DPLL Channel config offsets */
#define DPLL_CONFIG_BASE_DPLLNUMSTATE           (DPLL_DEBUG_MODE + SIZEOF_DEBUGMODE)
#define DPLL_CONFIG_BASE_K1          (DPLL_CONFIG_BASE_DPLLNUMSTATE + SIZEOF_DPLLNUMSTATE)
#define DPLL_CONFIG_BASE_K1SHIFT     (DPLL_CONFIG_BASE_K1 + SIZEOF_K1 * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_K1K2        (DPLL_CONFIG_BASE_K1SHIFT + SIZEOF_K1SHIFT * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_K1K2SHIFT   (DPLL_CONFIG_BASE_K1K2 + SIZEOF_K1K2 * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_K1K3        (DPLL_CONFIG_BASE_K1K2SHIFT + SIZEOF_K1K2SHIFT * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_K1K3SHIFT   (DPLL_CONFIG_BASE_K1K3 + SIZEOF_K1K3 * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_LOCKTHETHRE (DPLL_CONFIG_BASE_K1K3SHIFT + SIZEOF_K1K3SHIFT * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_DWELLCNT    (DPLL_CONFIG_BASE_LOCKTHETHRE + SIZEOF_LOCKTHETHRE * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_PELTHR_LOW  (DPLL_CONFIG_BASE_DWELLCNT + SIZEOF_DWELLCNT * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_PELTHR_HIGH (DPLL_CONFIG_BASE_PELTHR_LOW + 4)
#define DPLL_CONFIG_BASE_FLLDEC      (DPLL_CONFIG_BASE_PELTHR_LOW + SIZEOF_PELTHR * BCM_GDPLL_NUM_STATES)
#define DPLL_CONFIG_BASE_NCOBITS     (DPLL_CONFIG_BASE_FLLDEC + SIZEOF_FLLDEC)
#define DPLL_CONFIG_BASE_PHASECNTREF     (DPLL_CONFIG_BASE_NCOBITS + SIZEOF_NCOBITS)
#define DPLL_CONFIG_BASE_DUALLOOPFLAG    (DPLL_CONFIG_BASE_PHASECNTREF + SIZEOF_PHASECNTREF)
#define DPLL_CONFIG_BASE_PHASECNTDELTA_LOW   (DPLL_CONFIG_BASE_DUALLOOPFLAG + SIZEOF_DUALLOOPFLAG)
#define DPLL_CONFIG_BASE_PHASECNTDELTA_HIGH  (DPLL_CONFIG_BASE_PHASECNTDELTA_LOW + 4)

/* DPLL Channel state offsets */
#define DPLL_STATE_BASE_STATE        (DPLL_CONFIG_BASE_PHASECNTDELTA_LOW + SIZEOF_PHASECNTDELTA)
#define DPLL_STATE_BASE_LOOPF_LOW    (DPLL_STATE_BASE_STATE + SIZEOF_STATE)
#define DPLL_STATE_BASE_LOOPF_HIGH   (DPLL_STATE_BASE_LOOPF_LOW + 4)
#define DPLL_STATE_BASE_DWELLCNT     (DPLL_STATE_BASE_LOOPF_LOW + SIZEOF_LOOPF)
#define DPLL_STATE_BASE_LOCKDETF     (DPLL_STATE_BASE_DWELLCNT + SIZEOF_DWELLCOUNTER)
#define DPLL_STATE_BASE_PERROR_LOW   (DPLL_STATE_BASE_LOCKDETF + SIZEOF_LOCKDETF)
#define DPLL_STATE_BASE_PERROR_HIGH  (DPLL_STATE_BASE_PERROR_LOW + 4)
#define DPLL_STATE_BASE_FERROR_LOW   (DPLL_STATE_BASE_PERROR_LOW + SIZEOF_PERROR)
#define DPLL_STATE_BASE_FERROR_HIGH  (DPLL_STATE_BASE_FERROR_LOW + 4)
#define DPLL_STATE_BASE_FLLCNT       (DPLL_STATE_BASE_FERROR_LOW + SIZEOF_FERROR)
#define DPLL_STATE_BASE_1STLOOP      (DPLL_STATE_BASE_FLLCNT + SIZEOF_FLLCNT)
#define DPLL_STATE_BASE_INITFLAG     (DPLL_STATE_BASE_1STLOOP + SIZEOF_1STLOOP)
#define DPLL_STATE_BASE_PHASECNT_LOW  (DPLL_STATE_BASE_INITFLAG + SIZEOF_INITFLAG)
#define DPLL_STATE_BASE_PHASECNT_HIGH (DPLL_STATE_BASE_PHASECNT_LOW + 4)

#endif  /* DPLL_VERSION_01 */
#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_SHARED_H__ */
