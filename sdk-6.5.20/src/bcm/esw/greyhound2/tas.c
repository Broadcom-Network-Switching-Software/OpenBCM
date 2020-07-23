/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tas.c
 * Purpose:     Greyhound2 specific TAS driver
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/greyhound2.h>
#include <bcm/cosq.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tas.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/greyhound2.h>

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(BCM_TAS_SUPPORT)
/* GCLT time interval resolution : 8 ns */
#define GH2_TAS_GCLT_TICKS  (8)
/* GCLT table size */
#define GH2_TAS_GCLT_ENTRIES  (128)
/* Maximum logical port */
#define GH2_TAS_MAX_PORTCNT (66)
/* Maximum physical port */
#define GH2_MAX_PHY_PORTS   (90)

/* Delay parameters for Hold Advance */
/* Fixed tha overhead in bytes : SMD 8B, IPG 12B, Hold recognition time 12B */
#define FIXED_THA_OVERHEAD  (8 + 12 + 12)

/* Delay parameter for Gate Close Response time */
/* Delay of CLMAC TXFIFO in Bytes : 4 x 48B = 192B */
#define TGCR_CLMAC_TXFIFO  (192)
/* Delay of XLMAC TXFIFO in Bytes : 4 x 32B = 128B */
#define TGCR_XLMAC_TXFIFO  (128)
/* Delay of UNIMAC TXFIFO in Bytes : 4 x 32Bytes + 16 x 128bits = 384B */
#define TGCR_UNIMAC_TXFIFO (384)

/* software static data structure initialize indicator */
static uint8 gh2_profile_initialized = 0;

/* GH2 TAS related interrupt/event information */
typedef struct gh2_tas_event_info_s {
    bcmi_tas_profile_event_type_t p_event; /* tas internal profile event */
    bcm_cosq_event_type_t event;  /* bcm layer cosq event */
    soc_field_t mask_field; /* mask field of MEMFAILINTMASKr */
    soc_field_t status_field; /* status field of MEMFAILINTSTATUSr */
    soc_field_t clear_field; /* clear field of MEMFAILINT_CLRr */
    soc_reg_t   sts_regs[3]; /* sub status registers to
                                indicate individual status */
    soc_reg_t   clr_regs[3]; /* sub clear registers to
                                clear individual status */
} gh2_tas_event_info_t;

/* The table lists GH2 support TAS interrupts and events */
STATIC gh2_tas_event_info_t gh2_tas_event_info[] =
{
    /* ToD Window event is internally used for PTP mode time caculation to fit
     * the device HW program constrain.
     */
    {bcmiTASProfileEventTODWindow , -1,
     TOD_WINDOW_INTERRUPT_INTMASKf,
     TOD_WINDOW_INTERRUPTf,
     TOD_WINDOW_INTERRUPT_CLRf,
     {-1},
     {-1},
    },
    {bcmiTASProfileEventProfileChange, bcmCosqEventTASProfileChange,
     TAS_TBL_SWITCH_DONE_INTMASKf,
     TAS_TBL_SWITCH_DONEf,
     TAS_TBL_SWITCH_DONE_CLRf,
     {TAS_TBL_SWITCH_DONE_0r, TAS_TBL_SWITCH_DONE_1r, TAS_TBL_SWITCH_DONE_2r},
     {TAS_TBL_SWITCH_DONE_CLR_0r, TAS_TBL_SWITCH_DONE_CLR_1r,
      TAS_TBL_SWITCH_DONE_CLR_2r},
     },
    {bcmiTASProfileEventECCErr, bcmCosqEventTASECCErr,
     TAS_PARITY_ERR_INTMASKf,
     TAS_PARITY_ERRf,
     TAS_PARITY_ERR_CLRf,
     {TAS_PARITY_ERR_VLD_0r, TAS_PARITY_ERR_VLD_1r, TAS_PARITY_ERR_VLD_2r},
     {TAS_PARITY_ERR_CLR_0r, TAS_PARITY_ERR_CLR_1r, TAS_PARITY_ERR_CLR_2r}
    },
    {bcmiTASProfileEventNextCycleTimeErr, bcmCosqEventTASNextCycleTimeErr,
     SET_NEXT_CYCLE_TIME_ERROR_INTMASKf,
     SET_NEXT_CYCLE_TIME_ERRORf,
     SET_NEXT_CYCLE_TIME_ERROR_CLRf,
     {SET_NEXT_CYCLE_TIME_ERROR_0r, SET_NEXT_CYCLE_TIME_ERROR_1r,
      SET_NEXT_CYCLE_TIME_ERROR_2r},
     {SET_NEXT_CYCLE_TIME_ERROR_CLR_0r, SET_NEXT_CYCLE_TIME_ERROR_CLR_1r,
      SET_NEXT_CYCLE_TIME_ERROR_CLR_2r},
    },
    {bcmiTASProfileEventBaseTimeErr, bcmCosqEventTASBaseTimeErr,
     SET_BASE_TIME_ERROR_INTMASKf,
     SET_BASE_TIME_ERRORf,
     SET_BASE_TIME_ERROR_CLRf,
     {SET_BASE_TIME_ERROR_0r, SET_BASE_TIME_ERROR_1r, SET_BASE_TIME_ERROR_2r},
     {SET_BASE_TIME_ERROR_CLR_0r, SET_BASE_TIME_ERROR_CLR_1r,
      SET_BASE_TIME_ERROR_CLR_2r}
    },
    {bcmiTASProfileEventPtrErr, bcmCosqEventTASProfilePtrErr,
     TAS_CTL_TBL_PTR_ERR_INTMASKf,
     TAS_CTL_TBL_PTR_ERRf,
     TAS_CTL_TBL_PTR_ERR_CLRf,
     {TAS_CTL_TBL_PTR_ERR_0r, TAS_CTL_TBL_PTR_ERR_1r, TAS_CTL_TBL_PTR_ERR_2r},
     {TAS_CTL_TBL_PTR_ERR_CLR_0r, TAS_CTL_TBL_PTR_ERR_CLR_1r,
      TAS_CTL_TBL_PTR_ERR_CLR_2r}
    },
    /* bcm layer cosq event, no corresponding HW settings.
     * Notified by software when 'PTP out of sync' is configured.*/
    {-1, bcmCosqEventTASPTPOutOfSyncErr,
     -1
    },
    /* End of table, both intr_type == -1 && event == -1*/
    {-1, -1}
};

/* Track the configured bcm cosq event and internal profile event since both
 * events access the same interrupt resources. */
typedef struct gh2_tas_event_ctrl_s {
    sal_mutex_t event_lock;
    SHR_BITDCL  cosq_event_pbmp[_SHR_BITDCLSIZE(bcmCosqEventCount)];
    SHR_BITDCL  tas_event_pbmp[_SHR_BITDCLSIZE(bcmiTASProfileEventCount)];
} gh2_tas_event_ctrl_t;

STATIC gh2_tas_event_ctrl_t *gh2_tas_event_ctrl[BCM_MAX_NUM_UNITS];

STATIC profile_event_cb_fn gh2_profile_event_notify[BCM_MAX_NUM_UNITS] = {NULL};

#define GH2_TAS_EVENT_LOCK(tec) \
        sal_mutex_take((tec)->event_lock, sal_mutex_FOREVER);
#define GH2_TAS_EVENT_UNLOCK(tec) \
        sal_mutex_give((tec)->event_lock);

/* Essential profile information to identify the valid process interval (tod_id)
 * and parameters used to program to HW.
 */
typedef struct gh2_tod_event_info_s {
    uint64  tod_id; /* ptp sec time divid 64 (2^6) */
    bcm_ptp_timestamp_t base_time;
    uint32  old_cycle_time;
    uint32  new_cycle_time;
    uint32  new_cycle_extension;
    int     config_idx;
    profile_commit_cb_fn cb;
    void    *fn_data;
} gh2_tod_event_info_t;

/* Port based ToD event tracking to keep the profile information which is
 * committed but not program to HW yet.
 */
typedef struct gh2_tod_event_control_s {
    sal_mutex_t tod_lock;
    gh2_tod_event_info_t  *gh2_tod_event[GH2_TAS_MAX_PORTCNT];
    SHR_BITDCL tod_event_pbmp[_SHR_BITDCLSIZE(GH2_TAS_MAX_PORTCNT)];
} gh2_tod_event_control_t;

STATIC gh2_tod_event_control_t *gh2_tod_event_control[BCM_MAX_NUM_UNITS];

#define GH2_TOD_EVENT_LOCK(tec) \
        sal_mutex_take((tec)->tod_lock, sal_mutex_FOREVER);
#define GH2_TOD_EVENT_UNLOCK(tec) \
        sal_mutex_give((tec)->tod_lock);

STATIC int
bcmi_gh2_tas_profile_change(int unit, bcm_port_t lport);
STATIC int
bcmi_gh2_tas_profile_idx_get(int unit, bcm_port_t lport, int *config_idx);
STATIC int
bcmi_gh2_tas_control_get(int unit, bcm_port_t port, bcm_cosq_tas_control_t type,
                         int *arg);

/* HW need 500 us / 500000 ns to reflect the register settings */
#define GH2_TAS_HW_REG_RESPONSE_TIME    500000 /* 500000 ns */

/* GH2 only support 6 bit second value */
#define GH2_TAS_BASETIME_SEC_MASK   (0x3F)

/* Assuem GH2 need take 1 sec software process to calculate and program the
 * required time information.
 */
#define GH2_TAS_SW_PROCESS_TIME  1 /* 1 sec */

STATIC int
bcmi_gh2_tas_tod_handler(int unit);
/*
 * Function:
 *     bcmi_gh2_tas_profile_event_mask_set
 * Purpose:
 *     Configue the TAS related interrupt mask by specifying the internal
 *     profile event.
 *     For value=0 case, will be programmed when no bcm cos event reference it.
 * Parameters:
 *     unit             - (IN) unit number
 *     p_event          - (IN) internal profile event
 *     val              - (IN) mask value
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_event_mask_set(
    int unit,
    bcmi_tas_profile_event_type_t p_event,
    int val)
{
    gh2_tas_event_info_t *e_info;
    int i, rv = BCM_E_NONE;
    uint32 rval;
    gh2_tas_event_ctrl_t *tec = NULL;

    if ((tec = gh2_tas_event_ctrl[unit]) == NULL) {
        return BCM_E_INTERNAL;
    }

    if ((p_event < bcmiTASProfileEventTODWindow) ||
        (p_event >= bcmiTASProfileEventCount)) {
        return BCM_E_PARAM;
    }
    GH2_TAS_EVENT_LOCK(tec);
    if (val) {
        SHR_BITSET(tec->tas_event_pbmp, p_event);
    } else {
        SHR_BITCLR(tec->tas_event_pbmp, p_event);
    }
    for (i = 0; ; i++) {
        e_info = &gh2_tas_event_info[i];
        if ((e_info->p_event == -1) && (e_info->event == -1)) {
            /* End of table */
            break;
        }

        if (e_info->p_event == p_event) {
            if (e_info->event != -1) {
                if (SHR_BITGET(tec->cosq_event_pbmp, e_info->event) &&
                    (val == 0)) {
                    /* Do not set 0 if other event referece it */
                    break;
                }
            }
            rv = READ_MEMFAILINTMASKr(unit, &rval);
            if (BCM_FAILURE(rv)) {
                break;
            }
            soc_reg_field_set(unit, MEMFAILINTMASKr, &rval,
                              e_info->mask_field, val);
            rv = WRITE_MEMFAILINTMASKr(unit, rval);
            break;
        }
    }
    GH2_TAS_EVENT_UNLOCK(gh2_tas_event_ctrl[unit]);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_tod_attach
 * Purpose:
 *     Attach the profile to tod event list.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     base_time        - (IN) new base time
 *     old_cycle_time   - (IN) old cycle time
 *     new_cycle_time   - (IN) new cycle time
 *     new_cycle_extension - (IN) new cycle extension time
 *     config_idx       - (IN) inactive hw index
 *     cb               - (IN) cb function to call when the time information
 *                             programmed to HW and kick-off the
 *                             config change.
 *     fn_data          - (IN) fn_data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_tod_attach(
    int unit,
    bcm_port_t lport,
    bcm_ptp_timestamp_t base_time,
    uint32 old_cycle_time,
    uint32 new_cycle_time,
    uint32 new_cycle_extension,
    int config_idx,
    profile_commit_cb_fn cb,
    void *fn_data)
{
    gh2_tod_event_control_t *tec = gh2_tod_event_control[unit];
    int rv = BCM_E_NONE;
    gh2_tod_event_info_t *tod_event = NULL;
    uint64 sec_hi;
    uint32 sec_lo;
    uint32 ns;

    if (tec == NULL) {
        return BCM_E_INTERNAL;
    }

    if (tec->gh2_tod_event[lport]) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit,
                              "gh2 tod event has been attached !\n")));
        return BCM_E_BUSY;
    }
    TAS_ALLOC(unit, tod_event, gh2_tod_event_info_t ,
              sizeof(gh2_tod_event_info_t),
              "gh2 tod event", 0, rv);

    COMPILER_64_COPY(sec_hi, base_time.seconds);
    sec_lo = COMPILER_64_LO(base_time.seconds);
    /* GH2 TOD window 6-bit seconds */
    sec_lo &= GH2_TAS_BASETIME_SEC_MASK; /* take the lowest 6 bit */
    /* Right shift 6 bit as the tod_id (ToD Window id) */
    COMPILER_64_SHR(sec_hi, 6); /* >> 6 */

    /*
     * Caculate the target valid tod_id :
     * At least 2 x old_cycle_time ahead new base_time to switch to new cycle
     */
    ns = base_time.nanoseconds + 2 * old_cycle_time;
    if (ns >= TAS_PTP_TIME_NANOSEC_MAX) {
        /* wrap to 1 second */
        sec_lo += 1;
        if (sec_lo > GH2_TAS_BASETIME_SEC_MASK) {
            /* wrap to next TOD window */
            COMPILER_64_ADD_32(sec_hi, 1);
        }
    }
    /* Prepare the related parameters */
    tod_event->tod_id = sec_hi;
    memcpy(&tod_event->base_time, &base_time, sizeof(bcm_ptp_timestamp_t));
    tod_event->old_cycle_time = old_cycle_time;
    tod_event->new_cycle_time = new_cycle_time;
    tod_event->new_cycle_extension = new_cycle_extension;
    tod_event->config_idx = config_idx;
    tod_event->cb = cb;
    tod_event->fn_data = fn_data;

    GH2_TOD_EVENT_LOCK(tec);
    /* Enable TOD interrupt when tod_event_pbmp is empty */
    if (SHR_BITNULL_RANGE(tec->tod_event_pbmp, 0, GH2_TAS_MAX_PORTCNT)) {
        rv = bcmi_gh2_tas_profile_event_mask_set(
                 unit, bcmiTASProfileEventTODWindow, 1);
        if (BCM_FAILURE(rv)) {
            TAS_FREE(unit, tod_event, 0);
            GH2_TOD_EVENT_UNLOCK(tec);
            return rv;
        }
    }
    /* Attach to the tod event list */
    tec->gh2_tod_event[lport] = tod_event;
    SHR_BITSET(tec->tod_event_pbmp, lport);
    GH2_TOD_EVENT_UNLOCK(tec);

    (void)bcmi_gh2_tas_tod_handler(unit);
    return rv;
}

/*
 * Function:
 *     bcmi_gh2_tas_tod_detach
 * Purpose:
 *     Detach the profile from tod event list.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_tod_detach(int unit, bcm_port_t lport)
{
    gh2_tod_event_info_t    *tod_event;
    gh2_tod_event_control_t *tec = gh2_tod_event_control[unit];
    int                     rv = BCM_E_NONE;

    if (tec == NULL) {
        return BCM_E_INTERNAL;
    }
    GH2_TOD_EVENT_LOCK(tec);
    tod_event = tec->gh2_tod_event[lport];
    if (tod_event) {
        TAS_FREE(unit, tod_event, 0);
        tec->gh2_tod_event[lport] = NULL;
    }
    SHR_BITCLR(tec->tod_event_pbmp, lport);
    /* Disable TOD interrupt when tod_event_pbmp is empty */
    if (SHR_BITNULL_RANGE(tec->tod_event_pbmp, 0, GH2_TAS_MAX_PORTCNT)) {
        rv = bcmi_gh2_tas_profile_event_mask_set(
                 unit, bcmiTASProfileEventTODWindow, 0);
    }
    GH2_TOD_EVENT_UNLOCK(tec);
    return rv;
}

/*
 * Function:
 *     bcmi_gh2_tas_tod_handler
 * Purpose:
 *      Invoked from ToDWindow interrupt.
 *      Caculate the proper config_change_time according to current time,
 *      base_time and device limitation.
 *      The config_change_time would be (base_time + n * cycle_time)
 *      And program the time information to HW.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_tod_handler(int unit)
{
    gh2_tod_event_control_t *tec = gh2_tod_event_control[unit];
    int i, rv, tas_enable;
    gh2_tod_event_info_t tod;
    bcm_ptp_timestamp_t cur_time, config_change_time;
    uint64 cur_sec_hi, cur_sec_lo;
    uint32 cur_ns, tmp32;
    uint64 bt_in_ns, ct_in_ns, sec_mask;
    SHR_BITDCL tod_event_pbmp[_SHR_BITDCLSIZE(GH2_TAS_MAX_PORTCNT)];

    if (tec == NULL) {
        return BCM_E_INTERNAL;
    }

    GH2_TOD_EVENT_LOCK(tec);
    SHR_BITCOPY_RANGE(tod_event_pbmp, 0,
                      tec->tod_event_pbmp, 0,
                      GH2_TAS_MAX_PORTCNT);
    GH2_TOD_EVENT_UNLOCK(tec);

    /* Iter the valid entry form tod event list */
    SHR_BIT_ITER(tod_event_pbmp, GH2_TAS_MAX_PORTCNT, i) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tas_control_get(unit, i, bcmCosqTASControlTASEnable,
                                     &tas_enable));
        if (!tas_enable) {
            continue;
        }
        GH2_TOD_EVENT_LOCK(tec);
        if (tec->gh2_tod_event[i]) {
            sal_memcpy(&tod, tec->gh2_tod_event[i],
                       sizeof(gh2_tod_event_info_t));
        } else {
            GH2_TOD_EVENT_UNLOCK(tec);
            continue;
        }
        GH2_TOD_EVENT_UNLOCK(tec);
        /* Get the current time */
        rv = bcmi_tas_current_time(unit, &cur_time);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_U(unit,
                                  "Fatal error: "
                                  "Fail to get current time !\n")));
            break;
        }
        COMPILER_64_COPY(cur_sec_hi, cur_time.seconds);
        /* Right shift 6 bit as the current tod_id */
        COMPILER_64_SHR(cur_sec_hi, 6);

        /* Check if the event need to process in this Tod Window */
        /* current tod_id >= target todid : process the event */
        if (COMPILER_64_GE(cur_sec_hi, tod.tod_id)) {
            COMPILER_64_COPY(cur_sec_lo, cur_time.seconds);
            COMPILER_64_SET(sec_mask, 0, GH2_TAS_BASETIME_SEC_MASK);
            COMPILER_64_AND(cur_sec_lo, sec_mask); /* take the lowest 6 bit */
            ct_in_ns = bcmi_tas_ptp_to_ns(cur_time);
            /* Need 2 x old cycle time to do the config change */
            cur_ns = cur_time.nanoseconds + 2 * tod.old_cycle_time;
            if (cur_ns >= TAS_PTP_TIME_NANOSEC_MAX) {
                COMPILER_64_ADD_32(cur_sec_lo, 1);
                if (COMPILER_64_GT(cur_sec_lo, sec_mask)) {
                    LOG_DEBUG(BSL_LS_BCM_TAS,
                              (BSL_META_U(unit,
                                          "Skip this time(tod: %d lport:%d)\n"),
                               u64_L(tod.tod_id), i));
                    continue;
                }
                cur_ns -= TAS_PTP_TIME_NANOSEC_MAX;
            }
            COMPILER_64_ADD_32(ct_in_ns, 2 * tod.old_cycle_time);
            /* consider HW response time */
            cur_ns += GH2_TAS_HW_REG_RESPONSE_TIME;
            if (cur_ns >= TAS_PTP_TIME_NANOSEC_MAX) {
                COMPILER_64_ADD_32(cur_sec_lo, 1);
                if (COMPILER_64_GT(cur_sec_lo, sec_mask)) {
                    LOG_DEBUG(BSL_LS_BCM_TAS,
                              (BSL_META_U(unit,
                                          "Skip this time(tod: %d lport:%d)\n"),
                               u64_L(tod.tod_id), i));
                    continue;
                }
            }
            COMPILER_64_ADD_32(ct_in_ns, GH2_TAS_HW_REG_RESPONSE_TIME);

            /* consider software process time */
            COMPILER_64_ADD_32(cur_sec_lo, GH2_TAS_SW_PROCESS_TIME);
            if (COMPILER_64_GT(cur_sec_lo, sec_mask)) {
                LOG_DEBUG(BSL_LS_BCM_TAS,
                          (BSL_META_U(unit,
                                      "Skip this time(tod: %d lport:%d)\n"),
                           u64_L(tod.tod_id), i));
                continue;
            }
            COMPILER_64_ADD_32(ct_in_ns,
                               GH2_TAS_SW_PROCESS_TIME *
                               TAS_PTP_TIME_NANOSEC_MAX);
            bt_in_ns = bcmi_tas_ptp_to_ns(tod.base_time);

            /* Add N x new_cycle_time until > current time */
            while (COMPILER_64_LT(bt_in_ns, ct_in_ns)) {
                COMPILER_64_ADD_32(bt_in_ns, tod.new_cycle_time);
            }
            bcmi_tas_ns_to_ptp(bt_in_ns, &config_change_time);

            /* Program to HW */
            tmp32 = COMPILER_64_LO(config_change_time.seconds);
            tmp32 &= GH2_TAS_BASETIME_SEC_MASK;
            BCM_IF_ERROR_RETURN(
                WRITE_BASE_TIME_SECr(unit, i, tod.config_idx,
                                     tmp32));
            BCM_IF_ERROR_RETURN(
                WRITE_BASE_TIME_FRACr(unit, i, tod.config_idx,
                                      config_change_time.nanoseconds));
            BCM_IF_ERROR_RETURN(
                WRITE_CYCLE_TIMEr(unit, i, tod.config_idx,
                                  tod.new_cycle_time));
            BCM_IF_ERROR_RETURN(
                WRITE_CYCLE_TIME_EXTENSIONr(unit, i, tod.config_idx,
                                            tod.new_cycle_extension));
            /* Issue config change */
            BCM_IF_ERROR_RETURN(bcmi_gh2_tas_profile_change(unit, i));

            /*
             * Notify the actual config change time to common profile
             * management layer.
             */
            if (tod.cb) {
                BCM_IF_ERROR_RETURN(
                    tod.cb(unit, i, config_change_time, tod.fn_data));
            }
            /* Detach from TOD event list */
            BCM_IF_ERROR_RETURN(bcmi_gh2_tas_tod_detach(unit, i));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tas_profile_schedule_config
 * Purpose:
 *      Device specific function to schedule the proper time to program the
 *      time information to HW. Also provide the callback function to call when
 *      the time information programmed to HW and kick-off the config change.
 *
 * Parameters:
 *     unit              - (IN) unit number
 *     lport             - (IN) logical port
 *     base_time         - (IN) new base time
 *     cycle_time        - (IN) new cycle time
 *     cycle_extension   - (IN) new cycle extension
 *     cb                - (IN) cb function to call when the time information
 *                              programmed to HW and kick-off the
 *                              config change.
 *     fn_data           - (IN) fn_data to supply in the callback
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_schedule_config(
    int unit, bcm_port_t lport,
    bcm_ptp_timestamp_t base_time,
    uint32 cycle_time,
    uint32 cycle_extension,
    profile_commit_cb_fn cb,
    void *fn_data)
{
    int config_idx;
    uint32 old_cycle_time;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tas_profile_idx_get(unit, lport, &config_idx));

    /* Need the old cycle time to calculate the config change time */
    BCM_IF_ERROR_RETURN(
        READ_CYCLE_TIMEr(unit, lport, config_idx, &old_cycle_time));

    /* Attach the profile to ToD event list */
    return bcmi_gh2_tas_tod_attach(unit, lport, base_time, old_cycle_time,
                                   cycle_time, cycle_extension, config_idx,
                                   cb, fn_data);
}

/*
 * Check if the port is enabled 2-level scheduling
 * Disable it since checking mechanism is not settled yet.
 */
#define CHECK_64Q  (0)
/*
 * Function:
 *     bcmi_gh2_tas_port_validate
 * Purpose:
 *     Helper function to check if the port has TAS capability.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_tas_port_validate(int unit, bcm_port_t port)
{
#if CHECK_64Q
    int is_64q_port = 0;

    /* check 2-level scheduling property */
    if (true) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            soc_fl_64q_port_check(int unit, port, *is_64q_port);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            soc_gh2_64q_port_check(int unit, port, *is_64q_port);
        }
        if (is_64q_port) {
            /* unable to support TAS on 64Q port */
            return BCM_E_PARAM;
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_disable_notify
 * Purpose:
 *     Helper function to process what should be done when TAS is disabled.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_disable_notify(int unit, bcm_port_t port)
{
    /* Notify that active profile should be expired and
     * dismiss the process profile.
     */
    if (gh2_profile_event_notify[unit]) {
        BCM_IF_ERROR_RETURN(
            gh2_profile_event_notify[unit](unit, port,
                                           bcmiTASProfileEventTASDisable));
    }
    BCM_IF_ERROR_RETURN(bcmi_gh2_tas_tod_detach(unit, port));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_control_set
 * Purpose:
 *     Set various configurations for TAS.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) logical port
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (IN) tas control value
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_control_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_control_t type,
    int arg)
{
    uint32 rval, period, ptp_mode;

    BCM_IF_ERROR_RETURN(bcmi_gh2_tas_port_validate(unit, port));

    switch (type) {
    case bcmCosqTASControlTASEnable:
        /* GH2 valid value 0 or 1 */
        if ((arg < 0) || (arg > 1)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
        soc_reg_field_set(unit, TAS_CONFIG_0r, &rval,
                          GATE_ENABLEDf, arg);
        BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_0r(unit, port, rval));
        if (arg == 0) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_tas_disable_notify(unit, port));
        } else {
            (void)bcmi_gh2_tas_tod_handler(unit);
        }
        break;
    case bcmCosqTASControlUsePTPTime:
        /* GH2 valid value 0 or 1 */
        if ((arg < 0) || (arg > 1)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
        soc_reg_field_set(unit, TAS_CONFIG_0r, &rval,
                          PTP_ACTIVEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_0r(unit, port, rval));
        break;
    case bcmCosqTASControlInitGateState:
        /* GH2 valid value to specify 8 queue in bitmap */
        if ((arg > 0xFF) || (arg < 0)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_2r(unit, port, &rval));
        soc_reg_field_set(unit, TAS_CONFIG_2r, &rval,
                          INIT_GATE_STATEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_2r(unit, port, rval));
        break;
    case bcmCosqTASControlErrGateState:
        /* GH2 valid value to specify 8 queue in bitmap */
        if (arg > 0xFF) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_2r(unit, port, &rval));
        soc_reg_field_set(unit, TAS_CONFIG_2r, &rval,
                          ERR_GATE_STATEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_2r(unit, port, rval));
        break;
    case bcmCosqTASControlGatePurgeEnable:
        /* GH2 valid value 0 or 1 */
        if ((arg < 0) || (arg > 1)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(READ_TAS_GATE_PURGE_ENABLEr(unit, port, &rval));
        soc_reg_field_set(unit, TAS_GATE_PURGE_ENABLEr, &rval,
                          TAS_GATE_PURGE_ENABLEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_TAS_GATE_PURGE_ENABLEr(unit, port, rval));
        break;
    case bcmCosqTASControlGateControlTickInterval:
        /* GH2 valid ragne 8ns ~ 8192 ns */
        if ((arg < 8) || (arg > 8192)) {
            return BCM_E_PARAM;
        }
        /* From Arch document, period should be STATIC config and not allowed
           to be changed when TAS is enabled. */
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
        if (soc_reg_field_get(unit, TAS_CONFIG_0r, rval, GATE_ENABLEDf)) {
            LOG_ERROR(BSL_LS_BCM_TAS,
                      (BSL_META_UP(unit, port, "Tick Interval is not allowed "
                                   "to be changed when TAS is enabled.\n")));
            return BCM_E_BUSY;
        }
        period = (arg / GH2_TAS_GCLT_TICKS) - 1;
        BCM_IF_ERROR_RETURN(READ_GATE_CTRL_PERIODr(unit, port, &rval));
        soc_reg_field_set(unit, GATE_CTRL_PERIODr, &rval,
                          PERIODf, period);
        BCM_IF_ERROR_RETURN(WRITE_GATE_CTRL_PERIODr(unit, port, rval));
        break;
    case bcmCosqTASControlTASPTPLock:
        ptp_mode = 0;
        /* check if any tas enabled port is using ptp time */
        PBMP_PORT_ITER(unit, port) {
            BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
            if (soc_reg_field_get(unit, TAS_CONFIG_0r, rval, GATE_ENABLEDf)) {
                ptp_mode = soc_reg_field_get(unit, TAS_CONFIG_0r,
                                             rval, PTP_ACTIVEf);
                if (ptp_mode) {
                    break;
                }
            }
        }
        BCM_IF_ERROR_RETURN(READ_PTP_LOCKr(unit, &rval));
        soc_reg_field_set(unit, PTP_LOCKr, &rval,
                          PTP_LOCKf, arg);
        BCM_IF_ERROR_RETURN(WRITE_PTP_LOCKr(unit, rval));
        if ((arg == 0) && (ptp_mode == 1)) {
            /* Notify PTP out of sync */
            (void)bcmi_esw_cosq_handle_interrupt(
                    unit, bcmCosqEventTASPTPOutOfSyncErr, -1, -1);
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
 }

/*
 * Function:
 *     bcmi_gh2_tas_control_get
 * Purpose:
 *     Get various configurations for TAS
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas control type defined in bcm_cos_tas_control_t
 *     arg              - (OUT) tas control value
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_control_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_control_t type,
    int *arg)
{
    uint32 rval, period;

    BCM_IF_ERROR_RETURN(bcmi_gh2_tas_port_validate(unit, port));

    switch (type) {
    case bcmCosqTASControlTASEnable:
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
        *arg = soc_reg_field_get(unit, TAS_CONFIG_0r, rval, GATE_ENABLEDf);
        break;
    case bcmCosqTASControlUsePTPTime:
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, port, &rval));
        *arg = soc_reg_field_get(unit, TAS_CONFIG_0r, rval, PTP_ACTIVEf);
        break;
    case bcmCosqTASControlInitGateState:
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_2r(unit, port, &rval));
        *arg = soc_reg_field_get(unit, TAS_CONFIG_2r, rval, INIT_GATE_STATEf);
        break;
    case bcmCosqTASControlErrGateState:
        BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_2r(unit, port, &rval));
        *arg = soc_reg_field_get(unit, TAS_CONFIG_2r, rval, ERR_GATE_STATEf);
        break;
    case bcmCosqTASControlGatePurgeEnable:
        BCM_IF_ERROR_RETURN(READ_TAS_GATE_PURGE_ENABLEr(unit, port, &rval));
        *arg = soc_reg_field_get(unit, TAS_GATE_PURGE_ENABLEr, rval,
                                 TAS_GATE_PURGE_ENABLEf);
        break;
    case bcmCosqTASControlGateControlTickInterval:
        BCM_IF_ERROR_RETURN(READ_GATE_CTRL_PERIODr(unit, port, &rval));
        period = soc_reg_field_get(unit, GATE_CTRL_PERIODr, rval, PERIODf);
        *arg = (period + 1) * GH2_TAS_GCLT_TICKS;
        break;
    case bcmCosqTASControlTASPTPLock:
        BCM_IF_ERROR_RETURN(READ_PTP_LOCKr(unit, &rval));
        *arg = soc_reg_field_get(unit, PTP_LOCKr, rval, PTP_LOCKf);
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_status_get
 * Purpose:
 *     Get TAS status by specifying the type.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) gport id
 *     type             - (IN) tas status type defined in bcm_cos_tas_status_t
 *     arg              - (OUT) tas status value
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_status_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_status_t type,
    int *arg)
{
    uint32 rval;
    int auto_adjust_txoverrun = 0, auto_adjust_phold = 0;

    BCM_IF_ERROR_RETURN(bcmi_gh2_tas_port_validate(unit, port));

    switch (type) {
    case bcmCosqTASStatusGateQueueState:
        BCM_IF_ERROR_RETURN(READ_COS_QUEUEr(unit, port, &rval));
        *arg = soc_reg_field_get(unit, COS_QUEUEr, rval, GATE_STATEf);
        break;
    case bcmCosqTASStatusGateStateSet:
        BCM_IF_ERROR_RETURN(READ_COS_QUEUEr(unit, port, &rval));
        *arg = soc_reg_field_get(unit, COS_QUEUEr, rval, GATE_STATE_SELECTf);
        break;
    case bcmCosqTASStatusTickGranularity:
        *arg = GH2_TAS_GCLT_TICKS;
        break;
    case bcmCosqTASStatusMaxCalendarEntries:
        bcmi_esw_tas_property_get(unit, TAS_PROP_TXOVERRUN,
                                  port, -1, &auto_adjust_txoverrun);
        bcmi_esw_tas_property_get(unit, TAS_PROP_HOLDADVANCE,
                                  port, -1, &auto_adjust_phold);
        if (auto_adjust_txoverrun || auto_adjust_phold) {
            *arg = GH2_TAS_GCLT_ENTRIES / 2;
        } else {
            *arg = GH2_TAS_GCLT_ENTRIES;
        }
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Purpose:
 *  Get the hold advance time in ns
 * Description:
 * Tha = (Thrt - Tgor) where Thrt is the hold response time which depends on
 * Min_final_fragment_size and Min_non_final_fragment_size.
 * The minimum of Tgor(gate open response time) has to be considered in order to
 * make sure a preemptable frame never delays an express frame
 */
 STATIC int
bcmi_gh2_tas_tha_get(
    int unit,
    bcm_port_t lport,
    int *tha)
{
    bcm_gport_t gport;
    int speed, bit_tick_ps;
    uint32 non_final_frag_size, final_frag_size;

    if (tha == NULL) {
        return BCM_E_PARAM;
    }
    /* Get port speed -> caculate bit time in picosecond */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, lport, &speed));
    if (!speed) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit, lport, &speed));
    }
    if (!speed) {
        speed = SOC_CONTROL(unit)->info.port_speed_max[lport];
    }
    /* speed in Mbps, bit time in ps: 10^6/speed */
    bit_tick_ps = 1000000 / speed;

    BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, lport, &gport));
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_preemption_control_get(
            unit, gport, bcmPortPreemptControlNonFinalFragSizeTx,
            &non_final_frag_size));
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_preemption_control_get(
            unit, gport, bcmPortPreemptControlFinalFragSizeTx,
            &final_frag_size));
    /* Fixed time : SMD 8B, IPG 12B, Hold recognition time 12B */
    *tha = FIXED_THA_OVERHEAD * 8 * bit_tick_ps / 1000;
    /* Variable time : according to frag size */
    *tha += (non_final_frag_size + final_frag_size) * 8 * bit_tick_ps / 1000;

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_U(unit, "Tha (speed %d, non-frag %d frag %d ) %d ns\n"),
               speed, non_final_frag_size, final_frag_size, *tha));
     return BCM_E_NONE;
}

/*
 * Get the bytes in the edatabuf to compute worst case value of
 * T-edatabuf (flush time to cleanup the fifo)
 */
STATIC int
bcmi_gh2_tas_tgcr_edatabuf_fifo_get(
    int unit,
    bcm_port_t port,
    int *pkt_bytes)
{
    soc_info_t *si;
    int phy_port, sub_ports, pkt_cnt, shift, group, start, pkt_size;
    SHR_BITDCL pbmp[_SHR_BITDCLSIZE(GH2_MAX_PHY_PORTS)];

    if (pkt_bytes == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
        READ_EGR_TDM_PORT_MAPm(unit, MEM_BLOCK_ALL, 0, (void *)&pbmp));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

    /*
     * phy port group in EGR_TDM_PORT_MAPm bitmap
     * GE0:2-9, GE1:10-17, GE2:18-25, GE3:26-33, ... GE6: 50-57
     * XL0:58-59, XL1:60-61 ......XL13:84-85
     * CL0: 86-89
     */
    if (phy_port < 58) {
        /* < 58 : GE port, start from 2, 8 ports as a group.*/
        shift = 2;
        group = 8;
    } else if (phy_port < 86) {
        /* >=58, < 86: XL port, start from 58, 2 ports as a group */
        shift = 58;
        group = 2;
    } else if (phy_port < 90) {
        /* >= 86, < 90: CL port, start from 86, 4 ports as a group */
        shift = 86;
        group = 4;
    } else {
        return BCM_E_PARAM;
    }
    /* Find port belongs to which group and find the start of it */
    start = (((phy_port - shift) / group) * group) + shift;
    /* Caculate how many ports are valid in this group */
    SHR_BITCOUNT_RANGE(pbmp, sub_ports, start, group);

    /*
     * For Edatabuf FIFO flush time, under different configurations
     * (sub-port count), the total packets be sent (pkt.cnt * pkt.size) are
     * listed below.
     * The data is for express packets only. If preempt packets are considered,
     * the time should be doubled. For time caculation, we assume the case of
     * express packet.
     *
     * port-group sub-port pkt.cnt pkt.size
     *  GE         8         5       224
     *  GE         4        10       224
     *  GE         2        20       224
     *  GE         1        24       224
     *  XL         2        14       224
     *  XL         1        24       224
     *  CL         4        20       288
     *  CL         2        24       288
     *  CL         1        24       288
     */
    if (phy_port < 58) {
        /* GE port case */
        if (sub_ports == 8) {
            pkt_cnt = 5;
        } else if (sub_ports == 4) {
            pkt_cnt = 10;
        } else if (sub_ports == 2) {
            pkt_cnt = 20;
        } else {
            pkt_cnt = 24;
        }
        pkt_size = 224;
    } else if (phy_port < 86) {
        /* XL port case phy_port < 86 */
        if (sub_ports == 2) {
            pkt_cnt = 14;
        } else {
            pkt_cnt = 24;
        }
        pkt_size = 224;
    } else {
        /* The invalid phy_port check has been done.*/
        if (sub_ports == 4) {
            pkt_cnt = 20;
        } else if (sub_ports == 2) {
            pkt_cnt = 24;
        } else {
            pkt_cnt = 24;
        }
        pkt_size = 288;
    }
    *pkt_bytes = pkt_cnt * pkt_size;

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_U(unit,
                          "tgcr_edatabuf_fifo phy_port %d subport %d "
                          "pkt_bytes %d \n"),
               phy_port, sub_ports, *pkt_bytes));
    return BCM_E_NONE;
}

/* T-txpkt (depends on queueMaxSDU) */
/*
 * Get the bit time(in ns) of the maximum value of MaxSDU(maximum service data
 * unit) among the specified queue map.
 */
STATIC int
bcmi_gh2_tas_tgcr_txpkt_get(
    int unit,
    bcm_port_t port,
    uint32 qmap,
    int *tgcr_txpkt)
{
    int c;
    int prop;
    int maxsdu = 0;
    int speed, bit_tick_ps;

    if (tgcr_txpkt == NULL) {
        return BCM_E_PARAM;
    }
    for (c = 0; c < BCM_COS_COUNT; c++) {
        if (qmap & (1 << c)) {
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tas_property_get(unit, TAS_PROP_MAXSDU, port,
                                          c, &prop));
            /* keep the largest one */
            if (prop > maxsdu) {
                maxsdu = prop;
            }
        }
    }
    /* Get port speed -> caculate bit time in picosecond */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &speed));
    if (!speed) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit, port, &speed));
    }
    if (!speed) {
        speed = SOC_CONTROL(unit)->info.port_speed_max[port];
    }

    /* speed in Mbps, bit time in ps: 10^6/speed */
    bit_tick_ps = 1000000 / speed;

    *tgcr_txpkt = maxsdu * 8 * bit_tick_ps / 1000;

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_U(unit,
                          "Dynamic tgcr (speed %d, Qmap %08x, max_sdu %d) "
                          "%d ns \n"),
               speed, qmap, maxsdu, *tgcr_txpkt));
    return BCM_E_NONE;
}

/*
 * Get the Tgcr exclude the T-txpkt which could be queue-dependent.
 * The 'fixed' means its queue-independent, the port speed and attribute
 * decide the tgcr_fixed. This value is fixed across entries in calendar table.
 */
STATIC int
bcmi_gh2_tas_tgcr_fixed_get(int unit, bcm_port_t port, int *tgcr_fixed)
{
    soc_info_t *si;
    int delay_param;
    int freq, core_tick_ns;
    int speed, bit_tick_ps;
    int tdm_size, txpfc_enable, edatabuf_bytes, pkt_bytes, top_fifo_delay;

    if (tgcr_fixed == NULL) {
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    /* Get core clock freq in MHz */
    freq = si->frequency;
    /* core tick in ns : 10^3/freq */
    core_tick_ns = 1000 / freq;

    /* Get port speed -> caculate bit time in picosecond */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &speed));
    if (!speed) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit, port, &speed));
    }
    if (!speed) {
        speed = SOC_CONTROL(unit)->info.port_speed_max[port];
    }

    /* speed in Mbps, bit time in ps: 10^6/speed */
    bit_tick_ps = 1000000 / speed;

    /* T-gs-async-fifo  = 3 core clk */
    delay_param = 3 * core_tick_ns;

    /* T-egr-tdm : Worst TDM miss penalty for MMU dequeue =
     * (Max TDM interval - 1) x Cycle Time(ns) */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_tdm_size_get(unit, &tdm_size));
    } else /* endif BCM_FIRELIGHT_SUPPORT */
#endif
    {
        BCM_IF_ERROR_RETURN(soc_gh2_tdm_size_get(unit, &tdm_size));
    }
    delay_param += (tdm_size - 1) * core_tick_ns;

    /* T-edatabuf */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tas_tgcr_edatabuf_fifo_get(unit, port, &edatabuf_bytes));
    delay_param += edatabuf_bytes * 8 * bit_tick_ps / 1000;

    /* T-mactxfifo */
    if (IS_CL_PORT(unit, port)) {
        /* 4 x 48B = 192B */
        pkt_bytes = TGCR_CLMAC_TXFIFO;
        /* PM 4x25_top fifo delay = 15.51 ns */
        top_fifo_delay = 16;
    } else if (IS_XL_PORT(unit, port)) {
        /* 4 x 32B = 128B */
        pkt_bytes = TGCR_XLMAC_TXFIFO;
        /* PM 4x10_top fifo delay = 15.51 ns */
        top_fifo_delay = 16;
    } else if (IS_GE_PORT(unit, port)) {
        /* 4 x 32B  + 16 x 128b = 384B */
        pkt_bytes = TGCR_UNIMAC_TXFIFO;
        top_fifo_delay = 0;
    } else {
        return BCM_E_PARAM;
    }
    /* also consider the 80/64 encoding */
    delay_param += pkt_bytes * 8 * 80 / 64 * bit_tick_ps / 1000;
    delay_param += top_fifo_delay;

    /* T-pfc */
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_control_get(unit, port, bcmPortControlPFCTransmit,
                                 &txpfc_enable));
    if (txpfc_enable) {
        /* pfc frame size = 64B */
        delay_param += 64 * 8 * bit_tick_ps / 1000;
    }
    *tgcr_fixed = delay_param;

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_U(unit, "Fixed tgcr (core clk %d, tdm size %d, "
                                "speed %d, pfc enable %d, "
                                "edatabuf fifo %d, port fifo %d, "
                                "port top fifo %d) %d ns \n"),
               freq, tdm_size, speed, txpfc_enable, edatabuf_bytes,
               pkt_bytes, top_fifo_delay, *tgcr_fixed));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_profile_delay_param_get
 * Purpose:
 *     Helper function to get the device-specific delay parameters which are
 *     used to do the profile entry remap.
 *
 * Parameters:
 *     unit              - (IN) unit number
 *     tas_delay_param   - (IN) TAS_DPARAM_xxx to specify which delay param
 *     port              - (IN) logical port
 *     qmap              - (IN) queue bitmap, optional and valid for type
 *                         TAS_DPARAM_TGCR_DYNAMIC.
 *     value             - (OUT) value in ns
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_delay_param_get(
    int unit,
    int tas_delay_param,
    bcm_port_t port,
    uint32 qmap,
    int *value)
{
    int rv = BCM_E_NONE;

    if (value == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_gh2_tas_port_validate(unit, port));

    switch (tas_delay_param) {
        case (TAS_DPARAM_TGCR_FIXED):
            rv = bcmi_gh2_tas_tgcr_fixed_get(unit, port, value);
            break;
        case (TAS_DPARAM_TGCR_DYNAMIC):
            rv = bcmi_gh2_tas_tgcr_txpkt_get(unit, port, qmap, value);
            break;
        case (TAS_DPARAM_THA):
            rv = bcmi_gh2_tas_tha_get(unit, port, value);
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return rv;
}

/*
 * Basetime compare against CurrentTime + Tmargin
 *  if Basetime < CurrentTime + Tmargin  return BCM_E_NONE
 *  otherwise return BCM_E_TIMEOUT which is for user application to increase
 *  the config change error)
 */
STATIC int
bcmi_gh2_tas_profile_basetime_check(int unit, bcm_ptp_timestamp_t basetime)
{
    bcm_ptp_timestamp_t cur_time;
    uint64 bt_in_ns, ct_in_ns, tmargin;
    int len;
    uint32 max_cycle_time;

    if (BCM_FAILURE(bcmi_tas_current_time(unit, &cur_time))) {
        LOG_ERROR(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit,
                              "Fatal error: "
                              "Fail to get current time !\n")));
        return BCM_E_INTERNAL;
    }
    ct_in_ns = bcmi_tas_ptp_to_ns(cur_time);
    bt_in_ns = bcmi_tas_ptp_to_ns(basetime);

    COMPILER_64_ZERO(tmargin);
    /* worse case of the 2 times of cycle time */
    len = soc_reg_field_length(unit, CYCLE_TIMEr, CYCLE_TIMEf);
    max_cycle_time = (1 << len) - 1;
    COMPILER_64_ADD_32(tmargin, 2 * max_cycle_time);
    COMPILER_64_ADD_32(tmargin, GH2_TAS_HW_REG_RESPONSE_TIME);
    COMPILER_64_ADD_32(tmargin,
                       GH2_TAS_SW_PROCESS_TIME * TAS_PTP_TIME_NANOSEC_MAX);

    LOG_DEBUG(BSL_LS_BCM_TAS,
              (BSL_META_U(unit, "Tmargin in GH2 {0x%x 0x%x} \n"),
               COMPILER_64_HI(tmargin), COMPILER_64_LO(tmargin)));

    COMPILER_64_ADD_64(ct_in_ns, tmargin);
    if (COMPILER_64_GT(ct_in_ns, bt_in_ns)) {
        return BCM_E_TIMEOUT;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_profile_idx_get
 * Purpose:
 *     Get the inactive hw index. We need to program to the inactive sets
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 *     config_idx       - (OUT) inactive hw index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_idx_get(int unit, bcm_port_t lport, int *config_idx)
{
    uint32 rval;
    int active_idx;

    BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_1r(unit, lport, &rval));
    active_idx = soc_reg_field_get(unit, TAS_CONFIG_1r, rval, ACTIVE_SETf);
    *config_idx = 1 - active_idx;
    return BCM_E_NONE;
}

/*
 * gh2_mmu_gate_ctl_tbl :
 * valid port 2-65, valid idx 0-1
*/
static const soc_mem_t gh2_mmu_gate_ctl_tbl[66][2] =
{
    {INVALIDm, INVALIDm},
    {INVALIDm, INVALIDm},
    {MMU_GATE_CTL_TBL0_P2m, MMU_GATE_CTL_TBL1_P2m},
    {MMU_GATE_CTL_TBL0_P3m, MMU_GATE_CTL_TBL1_P3m},
    {MMU_GATE_CTL_TBL0_P4m, MMU_GATE_CTL_TBL1_P4m},
    {MMU_GATE_CTL_TBL0_P5m, MMU_GATE_CTL_TBL1_P5m},
    {MMU_GATE_CTL_TBL0_P6m, MMU_GATE_CTL_TBL1_P6m},
    {MMU_GATE_CTL_TBL0_P7m, MMU_GATE_CTL_TBL1_P7m},
    {MMU_GATE_CTL_TBL0_P8m, MMU_GATE_CTL_TBL1_P8m},
    {MMU_GATE_CTL_TBL0_P9m, MMU_GATE_CTL_TBL1_P9m},
    {MMU_GATE_CTL_TBL0_P10m, MMU_GATE_CTL_TBL1_P10m},
    {MMU_GATE_CTL_TBL0_P11m, MMU_GATE_CTL_TBL1_P11m},
    {MMU_GATE_CTL_TBL0_P12m, MMU_GATE_CTL_TBL1_P12m},
    {MMU_GATE_CTL_TBL0_P13m, MMU_GATE_CTL_TBL1_P13m},
    {MMU_GATE_CTL_TBL0_P14m, MMU_GATE_CTL_TBL1_P14m},
    {MMU_GATE_CTL_TBL0_P15m, MMU_GATE_CTL_TBL1_P15m},
    {MMU_GATE_CTL_TBL0_P16m, MMU_GATE_CTL_TBL1_P16m},
    {MMU_GATE_CTL_TBL0_P17m, MMU_GATE_CTL_TBL1_P17m},
    {MMU_GATE_CTL_TBL0_P18m, MMU_GATE_CTL_TBL1_P18m},
    {MMU_GATE_CTL_TBL0_P19m, MMU_GATE_CTL_TBL1_P19m},
    {MMU_GATE_CTL_TBL0_P20m, MMU_GATE_CTL_TBL1_P20m},
    {MMU_GATE_CTL_TBL0_P21m, MMU_GATE_CTL_TBL1_P21m},
    {MMU_GATE_CTL_TBL0_P22m, MMU_GATE_CTL_TBL1_P22m},
    {MMU_GATE_CTL_TBL0_P23m, MMU_GATE_CTL_TBL1_P23m},
    {MMU_GATE_CTL_TBL0_P24m, MMU_GATE_CTL_TBL1_P24m},
    {MMU_GATE_CTL_TBL0_P25m, MMU_GATE_CTL_TBL1_P25m},
    {MMU_GATE_CTL_TBL0_P26m, MMU_GATE_CTL_TBL1_P26m},
    {MMU_GATE_CTL_TBL0_P27m, MMU_GATE_CTL_TBL1_P27m},
    {MMU_GATE_CTL_TBL0_P28m, MMU_GATE_CTL_TBL1_P28m},
    {MMU_GATE_CTL_TBL0_P29m, MMU_GATE_CTL_TBL1_P29m},
    {MMU_GATE_CTL_TBL0_P30m, MMU_GATE_CTL_TBL1_P30m},
    {MMU_GATE_CTL_TBL0_P31m, MMU_GATE_CTL_TBL1_P31m},
    {MMU_GATE_CTL_TBL0_P32m, MMU_GATE_CTL_TBL1_P32m},
    {MMU_GATE_CTL_TBL0_P33m, MMU_GATE_CTL_TBL1_P33m},
    {MMU_GATE_CTL_TBL0_P34m, MMU_GATE_CTL_TBL1_P34m},
    {MMU_GATE_CTL_TBL0_P35m, MMU_GATE_CTL_TBL1_P35m},
    {MMU_GATE_CTL_TBL0_P36m, MMU_GATE_CTL_TBL1_P36m},
    {MMU_GATE_CTL_TBL0_P37m, MMU_GATE_CTL_TBL1_P37m},
    {MMU_GATE_CTL_TBL0_P38m, MMU_GATE_CTL_TBL1_P38m},
    {MMU_GATE_CTL_TBL0_P39m, MMU_GATE_CTL_TBL1_P39m},
    {MMU_GATE_CTL_TBL0_P40m, MMU_GATE_CTL_TBL1_P40m},
    {MMU_GATE_CTL_TBL0_P41m, MMU_GATE_CTL_TBL1_P41m},
    {MMU_GATE_CTL_TBL0_P42m, MMU_GATE_CTL_TBL1_P42m},
    {MMU_GATE_CTL_TBL0_P43m, MMU_GATE_CTL_TBL1_P43m},
    {MMU_GATE_CTL_TBL0_P44m, MMU_GATE_CTL_TBL1_P44m},
    {MMU_GATE_CTL_TBL0_P45m, MMU_GATE_CTL_TBL1_P45m},
    {MMU_GATE_CTL_TBL0_P46m, MMU_GATE_CTL_TBL1_P46m},
    {MMU_GATE_CTL_TBL0_P47m, MMU_GATE_CTL_TBL1_P47m},
    {MMU_GATE_CTL_TBL0_P48m, MMU_GATE_CTL_TBL1_P48m},
    {MMU_GATE_CTL_TBL0_P49m, MMU_GATE_CTL_TBL1_P49m},
    {MMU_GATE_CTL_TBL0_P50m, MMU_GATE_CTL_TBL1_P50m},
    {MMU_GATE_CTL_TBL0_P51m, MMU_GATE_CTL_TBL1_P51m},
    {MMU_GATE_CTL_TBL0_P52m, MMU_GATE_CTL_TBL1_P52m},
    {MMU_GATE_CTL_TBL0_P53m, MMU_GATE_CTL_TBL1_P53m},
    {MMU_GATE_CTL_TBL0_P54m, MMU_GATE_CTL_TBL1_P54m},
    {MMU_GATE_CTL_TBL0_P55m, MMU_GATE_CTL_TBL1_P55m},
    {MMU_GATE_CTL_TBL0_P56m, MMU_GATE_CTL_TBL1_P56m},
    {MMU_GATE_CTL_TBL0_P57m, MMU_GATE_CTL_TBL1_P57m},
    {MMU_GATE_CTL_TBL0_P58m, MMU_GATE_CTL_TBL1_P58m},
    {MMU_GATE_CTL_TBL0_P59m, MMU_GATE_CTL_TBL1_P59m},
    {MMU_GATE_CTL_TBL0_P60m, MMU_GATE_CTL_TBL1_P60m},
    {MMU_GATE_CTL_TBL0_P61m, MMU_GATE_CTL_TBL1_P61m},
    {MMU_GATE_CTL_TBL0_P62m, MMU_GATE_CTL_TBL1_P62m},
    {MMU_GATE_CTL_TBL0_P63m, MMU_GATE_CTL_TBL1_P63m},
    {MMU_GATE_CTL_TBL0_P64m, MMU_GATE_CTL_TBL1_P64m},
    {MMU_GATE_CTL_TBL0_P65m, MMU_GATE_CTL_TBL1_P65m},
};
/*
 * Function:
 *     bcmi_gh2_tas_profile_param_check
 * Purpose:
 *     Validate the parameters in bcm profile structure
 * Parameters:
 *     unit             - (IN) unit number
 *     profile          - (IN) bcm tas profile structure
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_param_check(int unit, bcm_cosq_tas_profile_t *profile)
{
    soc_mem_t gclt_mem = MMU_GATE_CTL_TBL0_P2m;
    bcm_cosq_tas_entry_t *tas_entry;
    int i, field_len;

    for (i = 0; i < profile->num_entries; i++) {
        tas_entry = &profile->entries[i];
        if (tas_entry->ticks == BCM_COSQ_TAS_ENTRY_TICKS_STAY) {
            field_len = soc_mem_field_length(unit, gclt_mem, TIME_INTERVALf);
            /* AND the 0xFFF for ticks_stay value */
            tas_entry->ticks &= (1 << field_len) - 1;
        }
        BCM_IF_ERROR_RETURN(soc_mem_field32_fit(unit, gclt_mem, TIME_INTERVALf,
                                                tas_entry->ticks));
        BCM_IF_ERROR_RETURN(soc_mem_field32_fit(unit, gclt_mem, GATE_STATEf,
                                                tas_entry->state));
    }

    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit, CYCLE_TIMEr, CYCLE_TIMEf,
                               profile->ptp_cycle_time));
    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit, CYCLE_TIME_EXTENSIONr,
                               CYCLE_TIME_EXTENSIONf,
                               profile->ptp_max_cycle_extension));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tas_profile_entries_write
 * Purpose:
 *     Program the profile entries to HW
 * Parameters:
 *     unit             - (IN) unit number
 *     lport             - (IN) logical port
 *     config_idx        - (IN) hw config idx
 *     entries           - (IN) profile entries
 *     num_entries       - (IN) number of entries
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_entries_write(
    int unit,
    bcm_port_t lport,
    int config_idx,
    bcm_cosq_tas_entry_t *entries,
    int num_entries)
{
    int i, idx_max;
    mmu_gate_ctl_tbl0_p2_entry_t *gclt_entry;
    uint8 *gclt_buf = NULL;
    uint8 phold = 0;
    int rv;
    soc_mem_t gclt_mem = MMU_GATE_CTL_TBL0_P2m;
    soc_memacc_t memacc_phold, memacc_interval, memacc_gstate;
    bcm_cosq_tas_entry_t *tas_entry;
    soc_info_t *si;
    int phy_port, mmu_port;

    if ((soc_memacc_init(unit, gclt_mem, PHOLD_REQf, &memacc_phold)) ||
        (soc_memacc_init(unit, gclt_mem, TIME_INTERVALf, &memacc_interval)) ||
        (soc_memacc_init(unit, gclt_mem, GATE_STATEf, &memacc_gstate))) {
          return BCM_E_PARAM;
    }

    TAS_ALLOC(unit, gclt_buf, uint8,
              num_entries * sizeof(mmu_gate_ctl_tbl0_p2_entry_t),
              "TAS GCLT buffer", 1, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    idx_max = num_entries;
    for (i = 0; i < idx_max; i++) {
        tas_entry = &entries[i];
        if (tas_entry->flags & BCM_COSQ_TAS_ENTRY_F_PREEMPT) {
            phold = 1;
        } else {
            phold = 0;
        }
        gclt_entry =
            soc_mem_table_idx_to_pointer(unit, gclt_mem,
                                         mmu_gate_ctl_tbl0_p2_entry_t *,
                                         gclt_buf,
                                         i);
        soc_memacc_field32_set(&memacc_gstate, gclt_entry, tas_entry->state);
        soc_memacc_field32_set(&memacc_phold, gclt_entry, phold);
        soc_memacc_field32_set(&memacc_interval, gclt_entry, tas_entry->ticks);

    }

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[lport];
    mmu_port = si->port_p2m_mapping[phy_port];
    rv = soc_mem_write_range(unit, gh2_mmu_gate_ctl_tbl[mmu_port][config_idx],
                             MEM_BLOCK_ALL, 0, (idx_max - 1), gclt_buf);
    if (BCM_FAILURE(rv)) {
        TAS_FREE(unit, gclt_buf, 1);
        return rv;
    }
    TAS_FREE(unit, gclt_buf, 1);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tas_profile_change
 * Purpose:
 *      Issue config change to start switching to new profile.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_change(int unit, bcm_port_t lport)
{
    uint32  rval;

    /* HW detect 0 to 1 change to start switching to new cycles.*/
    BCM_IF_ERROR_RETURN(READ_TAS_CONFIG_0r(unit, lport, &rval));
    soc_reg_field_set(unit, TAS_CONFIG_0r, &rval, CONFIG_CHANGEf, 0);
    BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_0r(unit, lport, rval));
    soc_reg_field_set(unit, TAS_CONFIG_0r, &rval, CONFIG_CHANGEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_TAS_CONFIG_0r(unit, lport, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tas_handle_interrupt
 * Purpose:
 *      Invoked when TAS related interrupts happen.
 *      Dispatch to corresponding callback according to the interrupt.
 * Parameters:
 *     unit             - (IN) unit number
 *     lport            - (IN) logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_handle_interrupt(int unit)
{
    int i, group, p;
    uint32 rval, sts;
    gh2_tas_event_info_t *e_info;
    gh2_tas_event_ctrl_t *tec = NULL;
    SHR_BITDCL  cosq_event_pbmp[_SHR_BITDCLSIZE(bcmCosqEventCount)];
    SHR_BITDCL  tas_event_pbmp[_SHR_BITDCLSIZE(bcmiTASProfileEventCount)];
    bcm_port_t lport, mmu_port, phy_port;
    soc_reg_t sts_reg, clr_reg;
    soc_info_t *si;
    uint8 event_cb = 0, intr_cb = 0;

    si = &SOC_INFO(unit);
    if ((tec = gh2_tas_event_ctrl[unit]) == NULL) {
        return BCM_E_INTERNAL;
    }

    rval = 0;
    BCM_IF_ERROR_RETURN(READ_MEMFAILINTSTATUSr(unit, &rval));

    if (bsl_check(bslLayerBcm, bslSourceTas, bslSeverityDebug, unit)) {
        bcm_ptp_timestamp_t cur_time;
        (void)bcmi_tas_current_time (unit, &cur_time);
        LOG_DEBUG(BSL_LS_BCM_TAS,
                  (BSL_META_U(unit, "intr sts 0x%x \n"), rval));
    }

    GH2_TAS_EVENT_LOCK(tec);
    SHR_BITCOPY_RANGE(cosq_event_pbmp, 0,
                      tec->cosq_event_pbmp, 0,
                      bcmCosqEventCount);
    SHR_BITCOPY_RANGE(tas_event_pbmp, 0,
                      tec->tas_event_pbmp, 0,
                      bcmiTASProfileEventCount);
    GH2_TAS_EVENT_UNLOCK(tec);

    for (i = 0; ; i++) {
        e_info = &gh2_tas_event_info[i];
        if ((e_info->p_event == -1) && (e_info->event == -1)) {
            /* End of table */
            break;
        }
        /*
         * PTP outofsync is not triggered by interrupt.
         * For this event, when user config the bcmCosqTASControlTASPTPLock = 0,
         * bcmi_esw_cosq_handle_interrupt will be notified from that.
         */
        if (e_info->p_event == -1) {
            continue;
        }
        /* Check the status to see which event causes the interrupt. */
        if (!soc_reg_field_get(unit, MEMFAILINTSTATUSr, rval,
                               e_info->status_field)) {
            continue;
        }
        if (e_info->event != -1) {
            if (SHR_BITGET(cosq_event_pbmp, e_info->event)) {
                event_cb = 1;
            }
        }
        if (SHR_BITGET(tas_event_pbmp, e_info->p_event)) {
            intr_cb = 1;
        }

        if ((sts_reg = e_info->sts_regs[0]) != -1) {
            /* Report the event by port */
            for (group = 0; group < 3; group++) {
                sts_reg = e_info->sts_regs[group];
                clr_reg = e_info->clr_regs[group];
                sts = 0;
                /* Check the per event status register to get
                 * which port causes the interrupt.
                 */
                soc_reg32_get(unit, sts_reg, REG_PORT_ANY, 0, &sts);
                if (sts == 0) {
                    continue;
                }
                for (p = 0; p < 32; p++) {
                    if (sts & (1 << p)) {
                        mmu_port = group * 32 + p;
                        phy_port = si->port_m2p_mapping[mmu_port];
                        lport = si->port_p2l_mapping[phy_port];
                        if (event_cb) {
                            (void)bcmi_esw_cosq_handle_interrupt(
                                    unit, e_info->event, lport,
                                    BCM_COS_INVALID);
                        }
                        if (intr_cb) {
                            /* profile change event */
                            (void)gh2_profile_event_notify[unit](
                                    unit, lport, e_info->p_event);
                        }
                    }
                }
                /* Clear sub status (write 1 clear) */
                soc_reg32_set(unit, clr_reg, REG_PORT_ANY, 0, sts);
            }
        } else {
            /* TOD Window system event */
            if (intr_cb) {
                (void)bcmi_gh2_tas_tod_handler(unit);
            }
        }
        /* Clear status (write 1 clear) */
        rval = 0;
        BCM_IF_ERROR_RETURN(READ_MEMFAILINT_CLRr(unit, &rval));
        soc_reg_field_set(unit, MEMFAILINT_CLRr, &rval,
                          e_info->clear_field, 1);
        BCM_IF_ERROR_RETURN(WRITE_MEMFAILINT_CLRr(unit, rval));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_event_mask_set
 * Purpose:
 *     Configue the TAS related interrupt mask by specifying bcm cosq event.
 *     For value=0 case, will be programmed when no other internal profile event
 *     reference it.
 * Parameters:
 *     unit             - (IN) unit number
 *     event_type       - (IN) bcm cosq event
 *     val              - (IN) mask value
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_event_mask_set(
    int unit,
    bcm_cosq_event_type_t event_type,
    int val)
{
    gh2_tas_event_info_t *e_info;
    int i, rv = BCM_E_NONE;
    uint32 rval;
    gh2_tas_event_ctrl_t *tec = NULL;

    if ((tec = gh2_tas_event_ctrl[unit]) == NULL) {
        return BCM_E_INTERNAL;
    }

    GH2_TAS_EVENT_LOCK(tec);
    if (val) {
        SHR_BITSET(tec->cosq_event_pbmp, event_type);
    } else {
        SHR_BITCLR(tec->cosq_event_pbmp, event_type);
    }

    for (i = 0; ; i++) {
        e_info = &gh2_tas_event_info[i];
        if ((e_info->p_event == -1) && (e_info->event == -1)) {
            /* End of table */
            break;
        }
        if (e_info->event == bcmCosqEventTASPTPOutOfSyncErr) {
            continue;
        }
        if (e_info->event == event_type) {
            if (e_info->p_event != -1) {
                if (SHR_BITGET(tec->tas_event_pbmp, e_info->p_event) &&
                    (val == 0)) {
                    /* Donot set 0 if other event referece it */
                    break;
              }
            }
            rv = READ_MEMFAILINTMASKr(unit, &rval);
            if (BCM_FAILURE(rv)) {
                break;
            }
            soc_reg_field_set(unit, MEMFAILINTMASKr, &rval,
                              e_info->mask_field, val);
            rv = WRITE_MEMFAILINTMASKr(unit, rval);
            break;
        }
    }

    GH2_TAS_EVENT_UNLOCK(tec);

    return BCM_E_NONE;
}
/*
 * Device specific function for bcmi_esw_cosq_event_validate.
 */
int
bcmi_gh2_cosq_events_validate(int unit, bcm_cosq_event_types_t event_types)
{
    bcm_cosq_event_type_t  e_type;
    gh2_tas_event_info_t *e_info;
    int i, rv = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_tas)) {
        return BCM_E_UNAVAIL;
    }

    for (e_type = 0; e_type < bcmCosqEventCount; ++e_type) {
        if (BCM_COSQ_EVENT_TYPE_GET(event_types, e_type)) {
            for (i = 0; ; i++) {
                e_info = &gh2_tas_event_info[i];
                if ((e_info->p_event == -1) && (e_info->event == -1)) {
                    /* End of table */
                    break;
                }
                if (e_info->event == e_type) {
                    break;
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmi_gh2_tas_profile_deinit
 * Purpose:
 *     Free the resource associated with the tas internal profile
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_deinit(int unit)
{
    int i, rv;
    gh2_tas_event_info_t *e_info;

    /* Disable the profile change interrupt */
    gh2_profile_event_notify[unit] = NULL;

    if (gh2_tod_event_control[unit] != NULL) {
        /* detach all TOD event */
        for (i = 0; i < GH2_TAS_MAX_PORTCNT; i++) {
            (void)bcmi_gh2_tas_tod_detach(unit, i);
        }
        if (gh2_tod_event_control[unit]->tod_lock) {
            sal_mutex_destroy(gh2_tod_event_control[unit]->tod_lock);
            gh2_tod_event_control[unit]->tod_lock = NULL;
        }
        TAS_FREE(unit, gh2_tod_event_control[unit], 0);
        gh2_tod_event_control[unit] = NULL;
    }

    if (gh2_tas_event_ctrl[unit] != NULL) {
        /* Disable the profile event interrupt */
        for (i = 0; ; i++) {
            e_info = &gh2_tas_event_info[i];
            if (e_info->p_event == -1) {
                /* End of table */
                break;
            }
            rv = bcmi_gh2_tas_profile_event_mask_set(
                     unit, e_info->p_event, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
        if (gh2_tas_event_ctrl[unit]->event_lock) {
            sal_mutex_destroy(gh2_tas_event_ctrl[unit]->event_lock);
            gh2_tas_event_ctrl[unit]->event_lock = NULL;
        }
        TAS_FREE(unit, gh2_tas_event_ctrl[unit], 0);
        gh2_tas_event_ctrl[unit] = NULL;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *     bcmi_gh2_tas_profile_init
 * Purpose:
 *     initialize the tas internal profile
 * Parameters:
 *     unit             - (IN) unit number
 *     cb               - (IN) callback function for profile event notification
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tas_profile_init(int unit, profile_event_cb_fn cb)
{
    int i, rv;
    gh2_tas_event_info_t *e_info;

    /* Initialize the structure, function ptr to NULL */
    if (gh2_profile_initialized == 0) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            gh2_profile_event_notify[i] = NULL;
            gh2_tod_event_control[i] = NULL;
            gh2_tas_event_ctrl[i] = NULL;
        }
        gh2_profile_initialized = 1;
    }

    /* Allocate/initialize the GH2 tas profile event control structure */
    if (gh2_tas_event_ctrl[unit] == NULL) {
        TAS_ALLOC(unit, gh2_tas_event_ctrl[unit],
                  gh2_tas_event_ctrl_t, sizeof(gh2_tas_event_ctrl_t),
                  "GH2 TAS event control ", 0, rv);
        if (BCM_FAILURE(rv)) {
            return BCM_E_INIT;
        }
        gh2_tas_event_ctrl[unit]->event_lock =
            sal_mutex_create("tas_event.lock");
        if (gh2_tas_event_ctrl[unit]->event_lock == NULL) {
            bcmi_gh2_tas_profile_deinit(unit);
            return BCM_E_INIT;
        }
    }

    /* Assign the profile event notify callback */
    gh2_profile_event_notify[unit] = cb;

    /* Allocate/initialize the GH2 tas TOD event control structure */
    if (gh2_tod_event_control[unit] == NULL) {
        TAS_ALLOC(unit, gh2_tod_event_control[unit],
                  gh2_tod_event_control_t, sizeof(gh2_tod_event_control_t),
                  "TOD event handler ", 0, rv);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tas_profile_deinit(unit);
            return BCM_E_INIT;
        }
        gh2_tod_event_control[unit]->tod_lock =
            sal_mutex_create("tas_tod.lock");
        if (gh2_tod_event_control[unit]->tod_lock == NULL) {
            bcmi_gh2_tas_profile_deinit(unit);
            return BCM_E_INIT;
        }
        for (i = 0; i < GH2_TAS_MAX_PORTCNT; i++) {
            gh2_tod_event_control[unit]->gh2_tod_event[i] = NULL;
        }
        sal_memset(gh2_tod_event_control[unit]->tod_event_pbmp, 0,
                   _SHR_BITDCLSIZE(GH2_TAS_MAX_PORTCNT) * sizeof(SHR_BITDCL));
    }

    /* Enable the profile event interrupt */
    for (i = 0; ; i++) {
        e_info = &gh2_tas_event_info[i];
        if (e_info->p_event == -1) {
            /* End of table */
            break;
        }
        /* Skip the TOD event during init enable it when required */
        if (e_info->p_event == bcmiTASProfileEventTODWindow) {
            continue;
        }
        rv = bcmi_gh2_tas_profile_event_mask_set(
                 unit, e_info->p_event, 1);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tas_profile_deinit(unit);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Driver functions to implement device-specific operations.
 * dispatched from common layer functions.
 */
STATIC const tas_drv_t gh2_tas_drv = {
        bcmi_gh2_tas_control_set,
        bcmi_gh2_tas_control_get,
        bcmi_gh2_tas_status_get,
        bcmi_gh2_tas_profile_init,
        bcmi_gh2_tas_profile_deinit,
        bcmi_gh2_tas_profile_param_check,
        bcmi_gh2_tas_profile_basetime_check,
        bcmi_gh2_tas_profile_delay_param_get,
        bcmi_gh2_tas_profile_idx_get,
        bcmi_gh2_tas_profile_entries_write,
        bcmi_gh2_tas_profile_schedule_config,
        bcmi_gh2_tas_profile_change
};

/*
 * Function:
 *     bcmi_gh2_tas_drv_init
 * Purpose:
 *     initialize the tas device-driver
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_tas_drv_init(int unit, const tas_drv_t **tas_drv)
{

    *tas_drv = &gh2_tas_drv;
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(
            soc_fl_cosq_event_handler_register(
                unit, bcmi_gh2_tas_handle_interrupt));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(
            soc_gh2_cosq_event_handler_register(
                unit, bcmi_gh2_tas_handle_interrupt));
    }

    return BCM_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT && BCM_TAS_SUPPORT */
