/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 * Manage chip specific functionality for TSN TAF implementation on GH2.
 */

#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tsn.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_taf.h>
#include <bcm_int/esw_dispatch.h>
#include <soc/profile_mem.h>
#include <shared/bsl.h>
#include <shared/shr_res_bitmap.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
#define WRITE_PTR(__ptr__, __type__, __data__)    \
        do {                                      \
          *((__type__*)(__ptr__)) = (__data__);   \
          (__ptr__) += sizeof(__type__);          \
        } while (0)

#define WRITE_PTR_RANGE(__ptr__, __size__, __data__)       \
        do {                                               \
          sal_memcpy((__ptr__), (__data__), (__size__));   \
          (__ptr__) += (__size__);                         \
        } while (0)

#define READ_PTR(__ptr__, __type__, __dest__)     \
        do {                                      \
          (__dest__) = *((__type__*)(__ptr__));   \
          (__ptr__) += sizeof(__type__);          \
        } while (0)

#define READ_PTR_RANGE(__ptr__, __size__, __dest__)        \
        do {                                               \
          sal_memcpy((__dest__), (__ptr__), (__size__));   \
          (__ptr__) += (__size__);                         \
        } while (0)

/* GCLT time interval resolution : 8 ns */
#define TAF_GCLT_TICKS  (8)

/*
 * taf gate
 */
#define TAF_GATE_RESERVED_ID        (0)
STATIC shr_res_bitmap_handle_t taf_gate_bitmap[SOC_MAX_NUM_DEVICES];
STATIC uint32 taf_gate_refcount[SOC_MAX_NUM_DEVICES][BCMI_TSN_TAF_GATE_NUM_MAX];


/*
 * taf gate pri map
 */
#define TAF_GATE_PRI_MAP_PROFILE_OFFSET     (0)
#define TAF_GATE_PRI_MAP_PROFILE_INVALID_ID (0)
#define TAF_GATE_PRI_MAP_INTPRI_WIDTH  (4)
#define TAF_GATE_PRI_MAP_INTPRI_NUM    (1 << TAF_GATE_PRI_MAP_INTPRI_WIDTH)
#define TAF_GATE_PRI_MAP_PREEMPT_WIDTH  (1)
#define TAF_GATE_PRI_MAP_PREEMPT_NUM    (1 << TAF_GATE_PRI_MAP_PREEMPT_WIDTH)

STATIC const soc_mem_t taf_gate_pri_map_mem = TAF_GATE_ID_LOOKUPm;

/*
 * taf max bytes profile
 */
#define TAF_MAXBYTE_PROFILE_RESERVED_ID        (0)
#define TAF_MAXBYTE_PROFILE_NUM                (4)
STATIC shr_res_bitmap_handle_t taf_maxbyte_profile_bitmap
                                          [SOC_MAX_NUM_DEVICES]
                                          [BCMI_TSN_TAF_GATE_NUM_MAX];
STATIC uint32 taf_maxbyte_profile_refcount[SOC_MAX_NUM_DEVICES]
                                          [BCMI_TSN_TAF_GATE_NUM_MAX]
                                          [TAF_MAXBYTE_PROFILE_NUM];
STATIC const soc_field_t taf_maxbyte_profile_field[TAF_MAXBYTE_PROFILE_NUM]
                           = {TAF_GATE_MAXBYTES_0f, TAF_GATE_MAXBYTES_1f,
                              TAF_GATE_MAXBYTES_2f, TAF_GATE_MAXBYTES_3f};
STATIC const soc_mem_t taf_maxbyte_profile_mem = TAF_GATE_CONTROLm;

/*
 * taf CosQ profile
 */
#define TAF_COSQ_INTPRI_OFFSET (0)
#define TAF_COSQ_INTPRI_WIDTH  (4)
#define TAF_COSQ_INTPRI_NUM    (1 << TAF_COSQ_INTPRI_WIDTH)
#define TAF_COSQ_INTPRI_MASK                       \
            (((1 << TAF_COSQ_INTPRI_WIDTH) - 1) << \
             TAF_COSQ_INTPRI_OFFSET)

#define TAF_COSQ_SELECT_OFFSET (4)
#define TAF_COSQ_SELECT_WIDTH  (2)
#define TAF_COSQ_SELECT_NUM    (1 << TAF_COSQ_SELECT_WIDTH)
#define TAF_COSQ_SELECT_MASK                       \
            (((1 << TAF_COSQ_SELECT_WIDTH) - 1) << \
             TAF_COSQ_SELECT_OFFSET)

#define TAF_COSQ_PROFILE_RESERVED_ID   (0)
#define TAF_COSQ_PROFILE_OFFSET        (6)
#define TAF_COSQ_PROFILE_WIDTH         (4)
#define TAF_COSQ_PROFILE_NUM           (1 << TAF_COSQ_PROFILE_WIDTH)
#define TAF_COSQ_PROFILE_MASK                       \
            (((1 << TAF_COSQ_PROFILE_WIDTH) - 1) << \
             TAF_COSQ_PROFILE_OFFSET)

#define TAF_COSQ_HW_INDEX(_profile_id_, _select_, _prio_)                \
                          (((_profile_id_) << TAF_COSQ_PROFILE_OFFSET) | \
                           ((_select_) << TAF_COSQ_SELECT_OFFSET)      | \
                           ((_prio_) << TAF_COSQ_INTPRI_OFFSET))

STATIC shr_res_bitmap_handle_t taf_cosq_profile_bitmap[SOC_MAX_NUM_DEVICES];
STATIC uint32 taf_cosq_profile_refcount[SOC_MAX_NUM_DEVICES]
                                       [TAF_COSQ_PROFILE_NUM];
STATIC const soc_mem_t taf_cosq_profile_mem = PORT_COS_MAPm;
STATIC const soc_field_t taf_cosq_profile_field = COSf;

/*
 * taf schedule profile
 */
STATIC shr_res_bitmap_handle_t taf_schedule_profile_bitmap
                               [SOC_MAX_NUM_DEVICES];
STATIC bcmi_tsn_taf_profile_t *taf_schedule_profile
                               [SOC_MAX_NUM_DEVICES]
                               [BCMI_TSN_TAF_PROFILE_NUM_MAX];

/*
 * taf schedule timelime
 */
#define TAF_SCHEDULE_CALENDAR_ENTIRES    (16)         /* GCLT table size */
#define TAF_SCHEDULE_HW_RESPONSE_TIME    (500000)     /*     500000 ns */
#define TAF_SCHEDULE_SW_RESPONSE_TIME    (1000000000) /* 1000000000 ns */
#define TAF_SCHEDULE_NANOSEC_MAX         (1000000000) /* 10^9 */

#define TAF_SCHEDULE_SEC_WIDTH           (6)    /* GH2 only support 6 bit
                                                 * second value
                                                 */
#define TAF_SCHEDULE_SEC_MASK            ((1 << TAF_SCHEDULE_SEC_WIDTH) - 1)
STATIC bcmi_tsn_taf_schedule_entry_t *taf_schedule_timeline
                                      [SOC_MAX_NUM_DEVICES]
                                      [BCMI_TSN_TAF_GATE_NUM_MAX];
STATIC uint8 *taf_schedule_calendar_buf[SOC_MAX_NUM_DEVICES];
STATIC const soc_mem_t taf_schedule_calendar_mem[2][BCMI_TSN_TAF_GATE_NUM_MAX]
    = {{TAF_GATE_CTL_TBL0_P0m, TAF_GATE_CTL_TBL0_P1m,
        TAF_GATE_CTL_TBL0_P2m, TAF_GATE_CTL_TBL0_P3m,
        TAF_GATE_CTL_TBL0_P4m, TAF_GATE_CTL_TBL0_P5m,
        TAF_GATE_CTL_TBL0_P6m, TAF_GATE_CTL_TBL0_P7m,
        TAF_GATE_CTL_TBL0_P8m, TAF_GATE_CTL_TBL0_P9m,
        TAF_GATE_CTL_TBL0_P10m, TAF_GATE_CTL_TBL0_P11m,
        TAF_GATE_CTL_TBL0_P12m, TAF_GATE_CTL_TBL0_P13m,
        TAF_GATE_CTL_TBL0_P14m, TAF_GATE_CTL_TBL0_P15m,
        TAF_GATE_CTL_TBL0_P16m, TAF_GATE_CTL_TBL0_P17m,
        TAF_GATE_CTL_TBL0_P18m, TAF_GATE_CTL_TBL0_P19m,
        TAF_GATE_CTL_TBL0_P20m, TAF_GATE_CTL_TBL0_P21m,
        TAF_GATE_CTL_TBL0_P22m, TAF_GATE_CTL_TBL0_P23m,
        TAF_GATE_CTL_TBL0_P24m, TAF_GATE_CTL_TBL0_P25m,
        TAF_GATE_CTL_TBL0_P26m, TAF_GATE_CTL_TBL0_P27m,
        TAF_GATE_CTL_TBL0_P28m, TAF_GATE_CTL_TBL0_P29m,
        TAF_GATE_CTL_TBL0_P30m, TAF_GATE_CTL_TBL0_P31m,
        TAF_GATE_CTL_TBL0_P32m, TAF_GATE_CTL_TBL0_P33m,
        TAF_GATE_CTL_TBL0_P34m, TAF_GATE_CTL_TBL0_P35m,
        TAF_GATE_CTL_TBL0_P36m, TAF_GATE_CTL_TBL0_P37m,
        TAF_GATE_CTL_TBL0_P38m, TAF_GATE_CTL_TBL0_P39m,
        TAF_GATE_CTL_TBL0_P40m, TAF_GATE_CTL_TBL0_P41m,
        TAF_GATE_CTL_TBL0_P42m, TAF_GATE_CTL_TBL0_P43m,
        TAF_GATE_CTL_TBL0_P44m, TAF_GATE_CTL_TBL0_P45m,
        TAF_GATE_CTL_TBL0_P46m, TAF_GATE_CTL_TBL0_P47m,
        TAF_GATE_CTL_TBL0_P48m, TAF_GATE_CTL_TBL0_P49m,
        TAF_GATE_CTL_TBL0_P50m, TAF_GATE_CTL_TBL0_P51m,
        TAF_GATE_CTL_TBL0_P52m, TAF_GATE_CTL_TBL0_P53m,
        TAF_GATE_CTL_TBL0_P54m, TAF_GATE_CTL_TBL0_P55m,
        TAF_GATE_CTL_TBL0_P56m, TAF_GATE_CTL_TBL0_P57m,
        TAF_GATE_CTL_TBL0_P58m, TAF_GATE_CTL_TBL0_P59m,
        TAF_GATE_CTL_TBL0_P60m, TAF_GATE_CTL_TBL0_P61m,
        TAF_GATE_CTL_TBL0_P62m, TAF_GATE_CTL_TBL0_P63m,
        TAF_GATE_CTL_TBL0_P64m, TAF_GATE_CTL_TBL0_P65m,
        TAF_GATE_CTL_TBL0_P66m, TAF_GATE_CTL_TBL0_P67m,
        TAF_GATE_CTL_TBL0_P68m, TAF_GATE_CTL_TBL0_P69m,
        TAF_GATE_CTL_TBL0_P70m, TAF_GATE_CTL_TBL0_P71m,
        TAF_GATE_CTL_TBL0_P72m, TAF_GATE_CTL_TBL0_P73m,
        TAF_GATE_CTL_TBL0_P74m, TAF_GATE_CTL_TBL0_P75m,
        TAF_GATE_CTL_TBL0_P76m, TAF_GATE_CTL_TBL0_P77m,
        TAF_GATE_CTL_TBL0_P78m, TAF_GATE_CTL_TBL0_P79m,
        TAF_GATE_CTL_TBL0_P80m, TAF_GATE_CTL_TBL0_P81m,
        TAF_GATE_CTL_TBL0_P82m, TAF_GATE_CTL_TBL0_P83m,
        TAF_GATE_CTL_TBL0_P84m, TAF_GATE_CTL_TBL0_P85m,
        TAF_GATE_CTL_TBL0_P86m, TAF_GATE_CTL_TBL0_P87m,
        TAF_GATE_CTL_TBL0_P88m, TAF_GATE_CTL_TBL0_P89m,
        TAF_GATE_CTL_TBL0_P90m, TAF_GATE_CTL_TBL0_P91m,
        TAF_GATE_CTL_TBL0_P92m, TAF_GATE_CTL_TBL0_P93m,
        TAF_GATE_CTL_TBL0_P94m, TAF_GATE_CTL_TBL0_P95m,
        TAF_GATE_CTL_TBL0_P96m, TAF_GATE_CTL_TBL0_P97m,
        TAF_GATE_CTL_TBL0_P98m, TAF_GATE_CTL_TBL0_P99m,
        TAF_GATE_CTL_TBL0_P100m, TAF_GATE_CTL_TBL0_P101m,
        TAF_GATE_CTL_TBL0_P102m, TAF_GATE_CTL_TBL0_P103m,
        TAF_GATE_CTL_TBL0_P104m, TAF_GATE_CTL_TBL0_P105m,
        TAF_GATE_CTL_TBL0_P106m, TAF_GATE_CTL_TBL0_P107m,
        TAF_GATE_CTL_TBL0_P108m, TAF_GATE_CTL_TBL0_P109m,
        TAF_GATE_CTL_TBL0_P110m, TAF_GATE_CTL_TBL0_P111m,
        TAF_GATE_CTL_TBL0_P112m, TAF_GATE_CTL_TBL0_P113m,
        TAF_GATE_CTL_TBL0_P114m, TAF_GATE_CTL_TBL0_P115m,
        TAF_GATE_CTL_TBL0_P116m, TAF_GATE_CTL_TBL0_P117m,
        TAF_GATE_CTL_TBL0_P118m, TAF_GATE_CTL_TBL0_P119m,
        TAF_GATE_CTL_TBL0_P120m, TAF_GATE_CTL_TBL0_P121m,
        TAF_GATE_CTL_TBL0_P122m, TAF_GATE_CTL_TBL0_P123m,
        TAF_GATE_CTL_TBL0_P124m, TAF_GATE_CTL_TBL0_P125m,
        TAF_GATE_CTL_TBL0_P126m, TAF_GATE_CTL_TBL0_P127m},
       {TAF_GATE_CTL_TBL1_P0m, TAF_GATE_CTL_TBL1_P1m,
        TAF_GATE_CTL_TBL1_P2m, TAF_GATE_CTL_TBL1_P3m,
        TAF_GATE_CTL_TBL1_P4m, TAF_GATE_CTL_TBL1_P5m,
        TAF_GATE_CTL_TBL1_P6m, TAF_GATE_CTL_TBL1_P7m,
        TAF_GATE_CTL_TBL1_P8m, TAF_GATE_CTL_TBL1_P9m,
        TAF_GATE_CTL_TBL1_P10m, TAF_GATE_CTL_TBL1_P11m,
        TAF_GATE_CTL_TBL1_P12m, TAF_GATE_CTL_TBL1_P13m,
        TAF_GATE_CTL_TBL1_P14m, TAF_GATE_CTL_TBL1_P15m,
        TAF_GATE_CTL_TBL1_P16m, TAF_GATE_CTL_TBL1_P17m,
        TAF_GATE_CTL_TBL1_P18m, TAF_GATE_CTL_TBL1_P19m,
        TAF_GATE_CTL_TBL1_P20m, TAF_GATE_CTL_TBL1_P21m,
        TAF_GATE_CTL_TBL1_P22m, TAF_GATE_CTL_TBL1_P23m,
        TAF_GATE_CTL_TBL1_P24m, TAF_GATE_CTL_TBL1_P25m,
        TAF_GATE_CTL_TBL1_P26m, TAF_GATE_CTL_TBL1_P27m,
        TAF_GATE_CTL_TBL1_P28m, TAF_GATE_CTL_TBL1_P29m,
        TAF_GATE_CTL_TBL1_P30m, TAF_GATE_CTL_TBL1_P31m,
        TAF_GATE_CTL_TBL1_P32m, TAF_GATE_CTL_TBL1_P33m,
        TAF_GATE_CTL_TBL1_P34m, TAF_GATE_CTL_TBL1_P35m,
        TAF_GATE_CTL_TBL1_P36m, TAF_GATE_CTL_TBL1_P37m,
        TAF_GATE_CTL_TBL1_P38m, TAF_GATE_CTL_TBL1_P39m,
        TAF_GATE_CTL_TBL1_P40m, TAF_GATE_CTL_TBL1_P41m,
        TAF_GATE_CTL_TBL1_P42m, TAF_GATE_CTL_TBL1_P43m,
        TAF_GATE_CTL_TBL1_P44m, TAF_GATE_CTL_TBL1_P45m,
        TAF_GATE_CTL_TBL1_P46m, TAF_GATE_CTL_TBL1_P47m,
        TAF_GATE_CTL_TBL1_P48m, TAF_GATE_CTL_TBL1_P49m,
        TAF_GATE_CTL_TBL1_P50m, TAF_GATE_CTL_TBL1_P51m,
        TAF_GATE_CTL_TBL1_P52m, TAF_GATE_CTL_TBL1_P53m,
        TAF_GATE_CTL_TBL1_P54m, TAF_GATE_CTL_TBL1_P55m,
        TAF_GATE_CTL_TBL1_P56m, TAF_GATE_CTL_TBL1_P57m,
        TAF_GATE_CTL_TBL1_P58m, TAF_GATE_CTL_TBL1_P59m,
        TAF_GATE_CTL_TBL1_P60m, TAF_GATE_CTL_TBL1_P61m,
        TAF_GATE_CTL_TBL1_P62m, TAF_GATE_CTL_TBL1_P63m,
        TAF_GATE_CTL_TBL1_P64m, TAF_GATE_CTL_TBL1_P65m,
        TAF_GATE_CTL_TBL1_P66m, TAF_GATE_CTL_TBL1_P67m,
        TAF_GATE_CTL_TBL1_P68m, TAF_GATE_CTL_TBL1_P69m,
        TAF_GATE_CTL_TBL1_P70m, TAF_GATE_CTL_TBL1_P71m,
        TAF_GATE_CTL_TBL1_P72m, TAF_GATE_CTL_TBL1_P73m,
        TAF_GATE_CTL_TBL1_P74m, TAF_GATE_CTL_TBL1_P75m,
        TAF_GATE_CTL_TBL1_P76m, TAF_GATE_CTL_TBL1_P77m,
        TAF_GATE_CTL_TBL1_P78m, TAF_GATE_CTL_TBL1_P79m,
        TAF_GATE_CTL_TBL1_P80m, TAF_GATE_CTL_TBL1_P81m,
        TAF_GATE_CTL_TBL1_P82m, TAF_GATE_CTL_TBL1_P83m,
        TAF_GATE_CTL_TBL1_P84m, TAF_GATE_CTL_TBL1_P85m,
        TAF_GATE_CTL_TBL1_P86m, TAF_GATE_CTL_TBL1_P87m,
        TAF_GATE_CTL_TBL1_P88m, TAF_GATE_CTL_TBL1_P89m,
        TAF_GATE_CTL_TBL1_P90m, TAF_GATE_CTL_TBL1_P91m,
        TAF_GATE_CTL_TBL1_P92m, TAF_GATE_CTL_TBL1_P93m,
        TAF_GATE_CTL_TBL1_P94m, TAF_GATE_CTL_TBL1_P95m,
        TAF_GATE_CTL_TBL1_P96m, TAF_GATE_CTL_TBL1_P97m,
        TAF_GATE_CTL_TBL1_P98m, TAF_GATE_CTL_TBL1_P99m,
        TAF_GATE_CTL_TBL1_P100m, TAF_GATE_CTL_TBL1_P101m,
        TAF_GATE_CTL_TBL1_P102m, TAF_GATE_CTL_TBL1_P103m,
        TAF_GATE_CTL_TBL1_P104m, TAF_GATE_CTL_TBL1_P105m,
        TAF_GATE_CTL_TBL1_P106m, TAF_GATE_CTL_TBL1_P107m,
        TAF_GATE_CTL_TBL1_P108m, TAF_GATE_CTL_TBL1_P109m,
        TAF_GATE_CTL_TBL1_P110m, TAF_GATE_CTL_TBL1_P111m,
        TAF_GATE_CTL_TBL1_P112m, TAF_GATE_CTL_TBL1_P113m,
        TAF_GATE_CTL_TBL1_P114m, TAF_GATE_CTL_TBL1_P115m,
        TAF_GATE_CTL_TBL1_P116m, TAF_GATE_CTL_TBL1_P117m,
        TAF_GATE_CTL_TBL1_P118m, TAF_GATE_CTL_TBL1_P119m,
        TAF_GATE_CTL_TBL1_P120m, TAF_GATE_CTL_TBL1_P121m,
        TAF_GATE_CTL_TBL1_P122m, TAF_GATE_CTL_TBL1_P123m,
        TAF_GATE_CTL_TBL1_P124m, TAF_GATE_CTL_TBL1_P125m,
        TAF_GATE_CTL_TBL1_P126m, TAF_GATE_CTL_TBL1_P127m}};
STATIC const soc_reg_t taf_reg_base_sec[2] = {TAF_BASE_TIME_SEC_0r,
                                              TAF_BASE_TIME_SEC_1r};
STATIC const soc_reg_t taf_reg_base_frac[2] = {TAF_BASE_TIME_FRAC_0r,
                                               TAF_BASE_TIME_FRAC_1r};
STATIC const soc_reg_t taf_reg_cycle[2] = {TAF_CYCLE_TIME_0r,
                                           TAF_CYCLE_TIME_1r};
STATIC const soc_reg_t taf_reg_ext[2] = {TAF_CYCLE_TIME_EXTENSION_0r,
                                         TAF_CYCLE_TIME_EXTENSION_1r};

/*
 * taf event handler
 */
STATIC bcmi_tsn_taf_event_handler_t *taf_event_handler_list
                                        [SOC_MAX_NUM_DEVICES]
                                        [BCMI_TSN_TAF_GATE_NUM_MAX]
                                        [bcmTsnTafEventCount];

/*
 * taf hw interrupt info
 */
#define TAF_INTERRUPT_SUB_FIELD_WIDTH (32)
#define TAF_INTERRUPT_SUB_FIELD_NUM \
        ((BCMI_TSN_TAF_GATE_NUM_MAX + TAF_INTERRUPT_SUB_FIELD_WIDTH - 1)\
         / TAF_INTERRUPT_SUB_FIELD_WIDTH) /* ceiling of (128/32) = 4 */

typedef enum gh2_taf_interrupt_type_e {
    TafInterruptSwitchDone = 0,
    TafInterruptTodWindow = 1,
    TafInterruptNextCycleTimeError = 2,
    TafInterruptBaseTimeError = 3,
    TafInterruptParityError = 4,
    TafInterruptTblPtrError = 5,
    TafInterruptCount = 6
} gh2_taf_interrupt_type_t;

typedef struct gh2_taf_interrupt_mapping_s {
    /* mask field of MEMFAILINTMASKr */
    soc_field_t mask_field;
    /* status field of MEMFAILINTSTATUSr */
    soc_field_t status_field;
    /* clear field of MEMFAILINT_CLRr */
    soc_field_t clear_field;
    /* sub status registers to indicate individual status */
    soc_reg_t   sub_status_regs[TAF_INTERRUPT_SUB_FIELD_NUM];
    /* sub clear registers to clear individual status */
    soc_reg_t   sub_clear_regs[TAF_INTERRUPT_SUB_FIELD_NUM];
} gh2_taf_interrupt_mapping_t;

STATIC const
gh2_taf_interrupt_mapping_t taf_interrupt_mapping[TafInterruptCount] = {
    {TAF_TBL_SWITCH_DONE_INTMASKf,
     TAF_TBL_SWITCH_DONEf,
     TAF_TBL_SWITCH_DONE_CLRf,
     {TAF_TBL_SWITCH_DONE_0r, TAF_TBL_SWITCH_DONE_1r,
      TAF_TBL_SWITCH_DONE_2r, TAF_TBL_SWITCH_DONE_3r},
     {TAF_TBL_SWITCH_DONE_CLR_0r, TAF_TBL_SWITCH_DONE_CLR_1r,
      TAF_TBL_SWITCH_DONE_CLR_2r, TAF_TBL_SWITCH_DONE_CLR_3r},
    },
    {TAF_TOD_WINDOW_INTERRUPT_INTMASKf,
     TAF_TOD_WINDOW_INTERRUPTf,
     TAF_TOD_WINDOW_INTERRUPT_CLRf,
     {INVALIDr},
     {INVALIDr},
    },
    {TAF_SET_NEXT_CYCLE_TIME_ERROR_INTMASKf,
     TAF_SET_NEXT_CYCLE_TIME_ERRORf,
     TAF_SET_NEXT_CYCLE_TIME_ERROR_CLRf,
     {TAF_SET_NEXT_CYCLE_TIME_ERROR_0r, TAF_SET_NEXT_CYCLE_TIME_ERROR_1r,
      TAF_SET_NEXT_CYCLE_TIME_ERROR_2r, TAF_SET_NEXT_CYCLE_TIME_ERROR_3r},
     {TAF_SET_NEXT_CYCLE_TIME_ERROR_CLR_0r,
      TAF_SET_NEXT_CYCLE_TIME_ERROR_CLR_1r,
      TAF_SET_NEXT_CYCLE_TIME_ERROR_CLR_2r,
      TAF_SET_NEXT_CYCLE_TIME_ERROR_CLR_3r},
    },
    {TAF_SET_BASE_TIME_ERROR_INTMASKf,
     TAF_SET_BASE_TIME_ERRORf,
     TAF_SET_BASE_TIME_ERROR_CLRf,
     {TAF_SET_BASE_TIME_ERROR_0r, TAF_SET_BASE_TIME_ERROR_1r,
      TAF_SET_BASE_TIME_ERROR_2r, TAF_SET_BASE_TIME_ERROR_3r},
     {TAF_SET_BASE_TIME_ERROR_CLR_0r, TAF_SET_BASE_TIME_ERROR_CLR_1r,
      TAF_SET_BASE_TIME_ERROR_CLR_2r, TAF_SET_BASE_TIME_ERROR_CLR_3r}
    },
    {TAF_PARITY_ERR_INTMASKf,
     TAF_PARITY_ERRf,
     TAF_PARITY_ERR_CLRf,
     {TAF_PARITY_ERR_VLD_0r, TAF_PARITY_ERR_VLD_1r,
      TAF_PARITY_ERR_VLD_2r, TAF_PARITY_ERR_VLD_3r},
     {TAF_PARITY_ERR_CLR_0r, TAF_PARITY_ERR_CLR_1r,
      TAF_PARITY_ERR_CLR_2r, TAF_PARITY_ERR_CLR_3r}
    },
    {TAF_CTL_TBL_PTR_ERR_INTMASKf,
     TAF_CTL_TBL_PTR_ERRf,
     TAF_CTL_TBL_PTR_ERR_CLRf,
     {TAF_CTL_TBL_PTR_ERR_0r, TAF_CTL_TBL_PTR_ERR_1r,
      TAF_CTL_TBL_PTR_ERR_2r, TAF_CTL_TBL_PTR_ERR_3r},
     {TAF_CTL_TBL_PTR_ERR_CLR_0r, TAF_CTL_TBL_PTR_ERR_CLR_1r,
      TAF_CTL_TBL_PTR_ERR_CLR_2r, TAF_CTL_TBL_PTR_ERR_CLR_3r}
    }
};

STATIC uint32 taf_interrupt_handling[SOC_MAX_NUM_DEVICES] = {0};
STATIC uint32 taf_interrupt_status[SOC_MAX_NUM_DEVICES];
STATIC uint32 taf_interrupt_substatus[SOC_MAX_NUM_DEVICES]
                                     [TafInterruptCount]
                                     [TAF_INTERRUPT_SUB_FIELD_NUM];

/* TSN TAF Gate stat table contents for GH2 */
STATIC const soc_mem_t taf_gate_cnt_mem[bcmTsnTafGateStatCount] =
                        {TAF_GATE_PASSED_PACKET_COUNTERm,
                         TAF_GATE_DROP_PACKET_COUNTERm};
STATIC const soc_mem_t taf_gate_cnt_field[bcmTsnTafGateStatCount] =
                        {TAF_GATE_PASSED_PKTf, TAF_GATE_DROP_PKTf};

/* sw database for taf gate counter */
STATIC uint64 *taf_gate_counter[SOC_MAX_NUM_DEVICES][bcmTsnTafGateStatCount];
STATIC uint8 *taf_gate_counter_dma_buffer[SOC_MAX_NUM_DEVICES]
                                         [bcmTsnTafGateStatCount];


/*
 * helper macro
 */
#define ERROR_RETURN_WITH_EXTRA_CLEAN(_op_, _extra_clean_) \
            do {                                           \
                int _rv_ = (_op_);                         \
                if (BCM_FAILURE(_rv_)) {                   \
                    (_extra_clean_);                       \
                    return (_rv_);                         \
                }                                          \
            } while (0)

#define TAF_INIT_ERROR_RETURN_WITH_CLEAN(_op_)             \
            ERROR_RETURN_WITH_EXTRA_CLEAN((_op_), bcmi_gh2_taf_cleanup(unit))

STATIC uint32
bcmi_gh2_taf_gate_map_profile_num(int unit)
{
    return (soc_mem_index_count(unit, TAF_GATE_CONTROLm));
}

STATIC int
_tsn_taf_gate_num(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);

    return si->tsn_taf_gate_num;
}

STATIC int
_taf_interrupt_sub_field_num(int unit)
{
    if (SOC_IS_FIRELIGHT(unit)){
        soc_info_t *si = &SOC_INFO(unit);
        return ((si->tsn_taf_gate_num + TAF_INTERRUPT_SUB_FIELD_WIDTH - 1)
               / TAF_INTERRUPT_SUB_FIELD_WIDTH); /* ceiling of (64/32) = 2 */
    }
    return TAF_INTERRUPT_SUB_FIELD_NUM;
}

/* allocate TAF gate */
STATIC int
bcmi_gh2_taf_gate_create(int unit, int flags, int *taf_gate_id)
{
    uint32 shr_flags = 0;

    if (NULL == taf_gate_id) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_TSN_TAF_WITH_ID) {
        shr_flags = SHR_RES_BITMAP_ALLOC_WITH_ID;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_alloc(taf_gate_bitmap[unit], shr_flags,
                             1, (int *)taf_gate_id));

    if (*taf_gate_id < 0 || *taf_gate_id >= _tsn_taf_gate_num(unit) ||
        *taf_gate_id == TAF_GATE_RESERVED_ID) {
        return BCM_E_INTERNAL;
    }
    taf_gate_refcount[unit][*taf_gate_id] = 1;

    return BCM_E_NONE;
}

/* traverse TAF gate */
STATIC int
bcmi_gh2_taf_gate_traverse(
    int unit,
    bcm_tsn_taf_gate_traverse_cb cb,
    void *user_data)
{
    int taf_gate_id;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    for (taf_gate_id = 0; taf_gate_id < _tsn_taf_gate_num(unit); taf_gate_id++) {
        if (TAF_GATE_RESERVED_ID == taf_gate_id) {
            continue;
        }
        if (0 == taf_gate_refcount[unit][taf_gate_id]) {
            continue;
        }
        BCM_IF_ERROR_RETURN(cb(unit, taf_gate_id, user_data));
    }

    return BCM_E_NONE;
}

/* free TAF gate */
STATIC int
bcmi_gh2_taf_gate_destroy(int unit, int taf_gate_id)
{
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    if (taf_gate_refcount[unit][taf_gate_id] == 0) {
        return BCM_E_NOT_FOUND;
    } else if (taf_gate_refcount[unit][taf_gate_id] > 1) {
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_free(taf_gate_bitmap[unit], 1, taf_gate_id));
    taf_gate_refcount[unit][taf_gate_id] = 0;

    return BCM_E_NONE;
}

/* taf gate id valid check */
STATIC int
bcmi_gh2_taf_gate_check(
    int unit,
    int taf_gate_id)
{
    if (taf_gate_id < 0 ||
        taf_gate_id == TAF_GATE_RESERVED_ID ||
        taf_gate_id >= _tsn_taf_gate_num(unit)) {
        return BCM_E_PARAM;
    }

    if (taf_gate_refcount[unit][taf_gate_id] == 0) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* get current time */
STATIC int
bcmi_gh2_taf_ptp_time_get(
    int unit,
    bcm_ptp_timestamp_t *ret_time)
{
    if (NULL == ret_time) {
        return BCM_E_PARAM;
    }

#if defined(INCLUDE_PTP)
    if (soc_feature(unit, soc_feature_ptp) ||
        soc_feature(unit, soc_feature_use_local_1588)) {
        if (SOC_IS_FIRELIGHT(unit)) {
            int rv = BCM_E_NONE;
            bcm_time_tod_t tod;
            uint32 stages;

            stages = BCM_TIME_STAGE_INGRESS_FLOWTRACKER;
            tod.time_format = bcmTimeFormatPTP;
            rv = bcm_esw_time_tod_get(unit, stages, &tod);

            ret_time->seconds = tod.ts.seconds;
            ret_time->nanoseconds = tod.ts.nanoseconds;
            return rv;
        } else {
            return bcm_esw_ptp_clock_time_get(unit, 0, 0, ret_time);
        }
    }
#endif /* INCLUDE_PTP */

    return BCM_E_UNAVAIL;
}

/* check if ptp_time_1 > ptp_time_2
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE int
bcmi_gh2_taf_ptp_time_gt(
    bcm_ptp_timestamp_t *ptp_time_1,
    bcm_ptp_timestamp_t *ptp_time_2)
{
    if (COMPILER_64_GT(ptp_time_1->seconds, ptp_time_2->seconds)) {
        return TRUE;
    } else if (COMPILER_64_EQ(ptp_time_1->seconds, ptp_time_2->seconds)) {
        if (ptp_time_1->nanoseconds > ptp_time_2->nanoseconds) {
            return TRUE;
        }
    }
    return FALSE;
}

/* check if ptp_time_1 >= ptp_time_2
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE int
bcmi_gh2_taf_ptp_time_ge(
    bcm_ptp_timestamp_t *ptp_time_1,
    bcm_ptp_timestamp_t *ptp_time_2)
{
    if (COMPILER_64_GT(ptp_time_1->seconds, ptp_time_2->seconds)) {
        return TRUE;
    } else if (COMPILER_64_EQ(ptp_time_1->seconds, ptp_time_2->seconds)) {
        if (ptp_time_1->nanoseconds >= ptp_time_2->nanoseconds) {
            return TRUE;
        }
    }
    return FALSE;
}

/* assign ptp_time_dst as ptp_time_src
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE void
bcmi_gh2_taf_ptp_time_assign(
    bcm_ptp_timestamp_t *ptp_time_dst,
    bcm_ptp_timestamp_t *ptp_time_src)
{
    COMPILER_64_COPY(ptp_time_dst->seconds, ptp_time_src->seconds);
    ptp_time_dst->nanoseconds = ptp_time_src->nanoseconds;
}

/* ptp_time_dst = ptp_time_dst + time_diff(ns)
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE void
bcmi_gh2_taf_ptp_time_add(
    bcm_ptp_timestamp_t *ptp_time_dst,
    uint32 time_diff)
{
    ptp_time_dst->nanoseconds += time_diff;
    if (ptp_time_dst->nanoseconds >= TAF_SCHEDULE_NANOSEC_MAX) {
        ptp_time_dst->nanoseconds -= TAF_SCHEDULE_NANOSEC_MAX;
        COMPILER_64_ADD_32(ptp_time_dst->seconds, 1);
    }
}

/* ptp_time_dst = ptp_time_dst - time_diff(ns)
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE void
bcmi_gh2_taf_ptp_time_sub(
    bcm_ptp_timestamp_t *ptp_time_dst,
    uint32 time_diff)
{
    if (ptp_time_dst->nanoseconds >= time_diff) {
        ptp_time_dst->nanoseconds -= time_diff;
    } else {
        ptp_time_dst->nanoseconds += TAF_SCHEDULE_NANOSEC_MAX;
        ptp_time_dst->nanoseconds -= time_diff;
        COMPILER_64_SUB_32(ptp_time_dst->seconds, 1);
    }
}

/* return the ToD (Time of Day) index of the ptp time
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE void
bcmi_gh2_taf_ptp_time_to_tod(
    bcm_ptp_timestamp_t *ptp_time, uint64 *ret_tod)
{
    /* In GH2, ToD is a 64sec window
     * ToD 0 : 0  ~  64sec
     * ToD 1 : 64 ~ 128sec
     * ....
     */
    COMPILER_64_COPY(*ret_tod, ptp_time->seconds);
    COMPILER_64_SHR(*ret_tod, TAF_SCHEDULE_SEC_WIDTH); /* >> 6 */
}

/* return ptp time of the specfic ToD
 * note. for speed, caller need to ensure that these pointer are not NULL
 */
STATIC INLINE void
bcmi_gh2_taf_ptp_time_from_tod(
    uint64 *tod, bcm_ptp_timestamp_t *ret_ptp_time)
{
    ret_ptp_time->nanoseconds = 0;
    COMPILER_64_COPY(ret_ptp_time->seconds, *tod);
    COMPILER_64_SHL(ret_ptp_time->seconds, TAF_SCHEDULE_SEC_WIDTH); /* << 6 */
}



STATIC int
bcmi_gh2_taf_event_register(
    int unit,
    int taf_gate,
    bcm_tsn_taf_event_types_t event_types,
    bcm_tsn_taf_event_cb cb,
    void *user_data)
{
    bcm_tsn_taf_event_type_t event_iter;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    /* duplicate check */
    SHR_BIT_ITER(event_types.w, bcmTsnTafEventCount, event_iter) {
        bcmi_tsn_taf_event_handler_t *event_handler_iter;

        event_handler_iter = taf_event_handler_list[unit][taf_gate][event_iter];
        while (NULL != event_handler_iter) {
            if (event_handler_iter->cb == cb) {
                break;
            }
            event_handler_iter = event_handler_iter->next;
        }
        if (NULL != event_handler_iter) {
            return BCM_E_EXISTS;
        }
    }

    /* insert new event handler at the tail */
    SHR_BIT_ITER(event_types.w, bcmTsnTafEventCount, event_iter) {
        bcmi_tsn_taf_event_handler_t *event_handler_iter;
        bcmi_tsn_taf_event_handler_t *event_handler_new;

        event_handler_new = sal_alloc(sizeof(bcmi_tsn_taf_event_handler_t),
                                      "TAF event handler entry");
        if (NULL == event_handler_new) {
            return BCM_E_MEMORY;
        }
        event_handler_new->cb = cb;
        event_handler_new->user_data = user_data;
        event_handler_new->next = NULL;

        if (NULL == taf_event_handler_list[unit][taf_gate][event_iter]) {
            taf_event_handler_list
                [unit][taf_gate][event_iter] = event_handler_new;
        } else {
            event_handler_iter = taf_event_handler_list
                                    [unit][taf_gate][event_iter];
            while (NULL != event_handler_iter->next) {
                event_handler_iter = event_handler_iter->next;
            }
            event_handler_iter->next = event_handler_new;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_taf_event_unregister(
    int unit,
    int taf_gate,
    bcm_tsn_taf_event_types_t event_types,
    bcm_tsn_taf_event_cb cb)
{
    bcm_tsn_taf_event_type_t event_iter;
    int found = 0;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    SHR_BIT_ITER(event_types.w, bcmTsnTafEventCount, event_iter) {
        bcmi_tsn_taf_event_handler_t *event_handler_iter;
        bcmi_tsn_taf_event_handler_t *event_handler_prev;

        event_handler_prev = NULL;
        event_handler_iter = taf_event_handler_list[unit][taf_gate][event_iter];
        while (NULL != event_handler_iter) {
            if (event_handler_iter->cb == cb) {
                break;
            }
            event_handler_prev = event_handler_iter;
            event_handler_iter = event_handler_iter->next;
        }
        if (NULL != event_handler_iter) {
            if (event_handler_iter == taf_event_handler_list
                                        [unit][taf_gate][event_iter]) {
                taf_event_handler_list
                    [unit][taf_gate][event_iter] = event_handler_iter->next;
                sal_free(event_handler_iter);
            } else {
                event_handler_prev->next = event_handler_iter->next;
                sal_free(event_handler_iter);
            }
            found = 1;
        }
    }
    if (1 == found) {
        return BCM_E_NONE;
    } else {
        return BCM_E_NOT_FOUND;
    }
}

STATIC int
bcmi_gh2_taf_event_notify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_event_type_t event_type)
{
    bcmi_tsn_taf_event_handler_t *event_handler_iter;

    if (event_type < 0 || event_type >= bcmTsnTafEventCount) {
        return BCM_E_PARAM;
    }
    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    event_handler_iter = taf_event_handler_list[unit][taf_gate][event_type];
    while (NULL != event_handler_iter) {
        BCM_IF_ERROR_RETURN(
            event_handler_iter->cb(unit, event_type, taf_gate,
                                   event_handler_iter->user_data));
        event_handler_iter = event_handler_iter->next;
    }
    return BCM_E_NONE;
}

/* get taf gate status */
STATIC int
bcmi_gh2_taf_gate_status_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_status_t type,
    uint32 *arg)
{
    uint32 regval;
    taf_gate_bytes_left_entry_t mem_entry;

    if (NULL == arg) {
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_GSG_STATUSr, REG_PORT_ANY,
                      taf_gate_id, &regval));

    switch (type) {
        case bcmTsnTafStatusGateState:
            *arg = soc_reg_field_get(unit, TAF_GSG_STATUSr,
                                     regval, TAF_GATE_STATEf);
            break;
        case bcmTsnTafStatusGateStateSet:
            *arg = soc_reg_field_get(unit, TAF_GSG_STATUSr,
                                     regval, GATE_STATE_SELECTf);
            break;
        case bcmTsnTafStatusGateCosProfile:
            *arg = soc_reg_field_get(unit, TAF_GSG_STATUSr,
                                     regval, TAF_COS_PROFILEf);
            break;
        case bcmTsnTafStatusGateMaxBytesProfile:
            *arg = soc_reg_field_get(unit, TAF_GSG_STATUSr,
                                     regval, TAF_MAXBYTES_SELECTf);
            break;
        case bcmTsnTafStatusTickGranularity:
            *arg = TAF_GCLT_TICKS;
            break;
        case bcmTsnTafStatusMaxCalendarEntries:
            *arg = TAF_SCHEDULE_CALENDAR_ENTIRES;
            break;
        case bcmTsnTafStatusGatMaxBytesLeft:
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, TAF_GATE_BYTES_LEFTm, MEM_BLOCK_ANY,
                             taf_gate_id, &mem_entry));

            *arg = soc_mem_field32_get(unit, TAF_GATE_BYTES_LEFTm, &mem_entry,
                                       GATE_BYTES_LEFTf);
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* taf port control set */
STATIC int
bcmi_gh2_taf_port_control_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_control_t type,
    uint32 arg)
{
    bcm_port_t port;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    switch (type) {
        case bcmTsnControlTafEnable:
            SOC_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, PORT_TABm, port,
                                       TAF_ENABLEf, arg));
            break;
        case bcmTsnControlTafGatePriMap:
            SOC_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, PORT_TABm, port,
                                       TAF_GATE_ID_PROFILEf, arg));
            break;
        case bcmTsnControlTafGatePriMapL2Select:
            SOC_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, PORT_TABm, port,
                                       L2_ENTRY_GATE_ID_SELECTf, arg));
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/* taf port control get */
STATIC int
bcmi_gh2_taf_port_control_get(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_control_t type,
    uint32 *arg)
{
    bcm_port_t port;
    port_tab_entry_t entry;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, port, &entry));

    switch (type) {
        case bcmTsnControlTafEnable:
            *arg = soc_mem_field32_get(unit, PORT_TABm, &entry,
                                       TAF_ENABLEf);
            break;
         case bcmTsnControlTafGatePriMap:
            *arg = soc_mem_field32_get(unit, PORT_TABm, &entry,
                                       TAF_GATE_ID_PROFILEf);
            break;
        case bcmTsnControlTafGatePriMapL2Select:
            *arg = soc_mem_field32_get(unit, PORT_TABm, &entry,
                                       L2_ENTRY_GATE_ID_SELECTf);
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}


/* taf gate control set */
STATIC int
bcmi_gh2_taf_gate_control_set(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_control_t type,
    uint32 arg)
{
    uint32 regval;
    uint32 field_value;

    switch (type) {
        case bcmTsnTafControlEnable:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_0r, &regval,
                              GATE_ENABLEDf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlUsePTPTime:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_0r, &regval,
                              PTP_ACTIVEf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlInitGateState:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              INIT_TAF_GATE_STATEf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlInitCosProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              INIT_TAF_COS_PROFILEf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlInitMaxBytesProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              INIT_TAF_MAXBYTES_SELECTf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlErrGateState:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              ERR_TAF_GATE_STATEf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlErrCosProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              ERR_TAF_COS_PROFILEf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlErrMaxByteProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_CONFIG_2r, &regval,
                              ERR_TAF_MAXBYTES_SELECTf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        case bcmTsnTafControlGateCloseControl:
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, TAF_GATE_CONTROLm, taf_gate_id,
                                       TAF_GATE_CLOSE_CONTROLf, arg));
            break;
        case bcmTsnTafControlBytesLeftCheckEnable:
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_modify(unit, TAF_GATE_CONTROLm, taf_gate_id,
                                       TAF_MAXBYTE_CHECK_ENABLEf, arg));
            break;
        case bcmTsnTafControlGateControlTickInterval:
            /* From Arch document, period should be STATIC config and
             *  not allowed to be changed when TAF is enabled.
             */
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            if (soc_reg_field_get(unit, TAF_CONFIG_0r, regval, GATE_ENABLEDf)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META("Tick Interval is not allowed "
                                    "to be changed when TAF is enabled.\n")));
                return BCM_E_BUSY;
            }

            /* Clock Period = (field_value + 1) * 8 nsec */
            if (arg % TAF_GCLT_TICKS != 0) {
                return BCM_E_PARAM;
            }
            field_value = (arg / TAF_GCLT_TICKS) - 1;

            SOC_IF_ERROR_RETURN(
                soc_reg_field_validate(unit, TAF_GATE_CTRL_PERIODr,
                                       PERIODf, field_value));
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_GATE_CTRL_PERIODr, REG_PORT_ANY,
                              taf_gate_id, &regval));
            soc_reg_field_set(unit, TAF_GATE_CTRL_PERIODr, &regval,
                              PERIODf, field_value);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_GATE_CTRL_PERIODr, REG_PORT_ANY,
                              taf_gate_id, regval));
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* taf gate control get */
STATIC int
bcmi_gh2_taf_gate_control_get(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_control_t type,
    uint32 *arg)
{
    uint32 regval;
    taf_gate_control_entry_t mem_entry;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmTsnTafControlEnable:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_0r,
                                     regval, GATE_ENABLEDf);
            break;
        case bcmTsnTafControlUsePTPTime:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_0r,
                                     regval, PTP_ACTIVEf);
            break;
        case bcmTsnTafControlInitGateState:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, INIT_TAF_GATE_STATEf);
            break;
        case bcmTsnTafControlInitCosProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, INIT_TAF_COS_PROFILEf);
            break;
        case bcmTsnTafControlInitMaxBytesProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, INIT_TAF_MAXBYTES_SELECTf);
            break;
        case bcmTsnTafControlErrGateState:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, ERR_TAF_GATE_STATEf);
            break;
        case bcmTsnTafControlErrCosProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, ERR_TAF_COS_PROFILEf);
            break;
        case bcmTsnTafControlErrMaxByteProfile:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_CONFIG_2r, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_CONFIG_2r,
                                     regval, ERR_TAF_MAXBYTES_SELECTf);
            break;
        case bcmTsnTafControlGateCloseControl:
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, TAF_GATE_CONTROLm, MEM_BLOCK_ANY,
                             taf_gate_id, &mem_entry));
            *arg = soc_mem_field32_get(unit, TAF_GATE_CONTROLm, &mem_entry,
                                       TAF_GATE_CLOSE_CONTROLf);
            break;
        case bcmTsnTafControlBytesLeftCheckEnable:
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, TAF_GATE_CONTROLm, MEM_BLOCK_ANY,
                             taf_gate_id, &mem_entry));
            *arg = soc_mem_field32_get(unit, TAF_GATE_CONTROLm, &mem_entry,
                                       TAF_MAXBYTE_CHECK_ENABLEf);
            break;
        case bcmTsnTafControlGateControlTickInterval:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_GATE_CTRL_PERIODr, REG_PORT_ANY,
                              taf_gate_id, &regval));
            *arg = soc_reg_field_get(unit, TAF_GATE_CTRL_PERIODr,
                                     regval, PERIODf);
            *arg = (*arg + 1) * TAF_GCLT_TICKS;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* taf global control set */
STATIC int
bcmi_gh2_taf_global_control_set(
    int unit,
    bcm_tsn_taf_control_t type,
    uint32 arg)
{
    uint32 regval;

    switch (type) {
        case bcmTsnTafControlTAFPTPLock:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_PTP_LOCKr, REG_PORT_ANY,
                              0, &regval));
            soc_reg_field_set(unit, TAF_PTP_LOCKr, &regval,
                              PTP_LOCKf, arg);
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, TAF_PTP_LOCKr, REG_PORT_ANY,
                              0, regval));
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* taf global control get */
STATIC int
bcmi_gh2_taf_global_control_get(
    int unit,
    bcm_tsn_taf_control_t type,
    uint32 *arg)
{
    uint32 regval;

    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmTsnTafControlTAFPTPLock:
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, TAF_PTP_LOCKr, REG_PORT_ANY, 0, &regval));
            *arg = soc_reg_field_get(unit, TAF_PTP_LOCKr,
                                     regval, PTP_LOCKf);
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* get hw index of taf gate pri map
 * ex. TAF_GATE_ID_LOOKUP
 * Key:  PREEMPTABLE_CELL (1 bit) +
 * INT_PRI (4 bit) +
 * TAF_GATE_ID_PROFILE (7 bit)
 *
 * TAF_GATE_ID_PROFILE is at LSB, so sw/hw index is the same
 */
int
bcmi_gh2_taf_gate_pri_map_hw_idx_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_idx)
{
    if (NULL == hw_idx ||
        bcmi_gh2_taf_gate_map_profile_num(unit) <= sw_idx ||
        TAF_GATE_PRI_MAP_PROFILE_INVALID_ID == sw_idx) {
        return BCM_E_PARAM;
    }

    *hw_idx = sw_idx;

    return BCM_E_NONE;
}

/* get sw index of taf gate pri map */
int
bcmi_gh2_taf_gate_pri_map_sw_idx_get(
    int unit,
    uint32 hw_idx,
    uint32 *sw_idx)
{
    if (NULL == sw_idx) {
        return BCM_E_PARAM;
    }

    /* hw index should be located in the range 1~127 */
    if (hw_idx >= bcmi_gh2_taf_gate_map_profile_num(unit) ||
        hw_idx == TAF_GATE_PRI_MAP_PROFILE_INVALID_ID) {
        return BCM_E_PARAM;
    }
    *sw_idx = hw_idx;

    return BCM_E_NONE;
}

STATIC int
_taf_gate_pri_map_profile_width(int unit)
{
    int bit_cnt;

    for (bit_cnt = 0; bit_cnt < 32; bit_cnt++) {
        if ((1 << bit_cnt) & soc_mem_index_count(unit, TAF_GATE_CONTROLm)) {
            return bit_cnt;
        }
    }
    return 0;
}

STATIC int
_taf_gate_pri_map_intpri_offset(int unit)
{
    return (TAF_GATE_PRI_MAP_PROFILE_OFFSET + _taf_gate_pri_map_profile_width(unit));
}

STATIC int
_taf_gate_pri_map_preempt_offset(int unit)
{
    return (_taf_gate_pri_map_intpri_offset(unit) + TAF_GATE_PRI_MAP_INTPRI_WIDTH);
}

STATIC uint32
_taf_gate_pri_map_mask(int unit)
{
    uint32 profile_mask;
    uint32 intpri_mask;
    uint32 preempt_mask;

    profile_mask = ((1 << _taf_gate_pri_map_profile_width(unit)) - 1) <<
                  TAF_GATE_PRI_MAP_PROFILE_OFFSET;
    intpri_mask = ((1 << TAF_GATE_PRI_MAP_INTPRI_WIDTH) - 1) <<
                  _taf_gate_pri_map_intpri_offset(unit);
    preempt_mask = ((1 << TAF_GATE_PRI_MAP_PREEMPT_WIDTH) - 1) <<
                   _taf_gate_pri_map_preempt_offset(unit);

    return (profile_mask | intpri_mask | preempt_mask);
}

/* helper function of bcmi_gh2_taf_gate_pri_map_set */
STATIC int
bcmi_gh2_taf_gate_pri_map_change(
    int unit,
    uint32 profile_idx,
    uint32 priority,
    uint32 preempt,
    uint32 new_gate_id)
{
    taf_gate_id_lookup_entry_t hw_entry;
    uint32 old_gate_id;
    uint32 hw_idx;

    if (preempt >= TAF_GATE_PRI_MAP_PREEMPT_NUM ||
        priority >= TAF_GATE_PRI_MAP_INTPRI_NUM ||
        profile_idx >= bcmi_gh2_taf_gate_map_profile_num(unit) ||
        profile_idx == TAF_GATE_PRI_MAP_PROFILE_INVALID_ID) {
        return BCM_E_PARAM;
    }
    hw_idx = (preempt << _taf_gate_pri_map_preempt_offset(unit)) |
             (priority << _taf_gate_pri_map_intpri_offset(unit)) |
             (profile_idx << TAF_GATE_PRI_MAP_PROFILE_OFFSET);

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, taf_gate_pri_map_mem, MEM_BLOCK_ANY,
                     hw_idx, &hw_entry));

    old_gate_id = soc_mem_field32_get(unit, taf_gate_pri_map_mem,
                                      &hw_entry, TAF_GATE_IDf);

    /* change to new gate id */
    BCM_IF_ERROR_RETURN(
        soc_mem_field32_fit(
            unit, taf_gate_pri_map_mem, TAF_GATE_IDf, new_gate_id));
    soc_mem_field32_set(
        unit, taf_gate_pri_map_mem,
        &hw_entry, TAF_GATE_IDf, new_gate_id);
    if (new_gate_id != TAF_GATE_RESERVED_ID) {
        taf_gate_refcount[unit][new_gate_id]++;
    }


    /* de-ref old gate id */
    if (old_gate_id != TAF_GATE_RESERVED_ID) {
        taf_gate_refcount[unit][old_gate_id]--;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, taf_gate_pri_map_mem, MEM_BLOCK_ALL,
                      hw_idx, &hw_entry));

    return BCM_E_NONE;
}

/* helper function of bcmi_gh2_taf_gate_pri_map_get
 */
STATIC int
bcmi_gh2_taf_gate_pri_map_retrieve(
    int unit,
    uint32 profile_idx,
    uint32 priority,
    uint32 preempt,
    uint32 *output_gate_id)
{
    taf_gate_id_lookup_entry_t hw_entry;
    uint32 hw_idx;

    if (output_gate_id == NULL) {
        return BCM_E_PARAM;
    }

    if (preempt >= TAF_GATE_PRI_MAP_PREEMPT_NUM ||
        priority >= TAF_GATE_PRI_MAP_INTPRI_NUM ||
        profile_idx >= bcmi_gh2_taf_gate_map_profile_num(unit) ||
        profile_idx == TAF_GATE_PRI_MAP_PROFILE_INVALID_ID) {
        return BCM_E_PARAM;
    }
    hw_idx = (preempt << _taf_gate_pri_map_preempt_offset(unit)) |
             (priority << _taf_gate_pri_map_intpri_offset(unit)) |
             (profile_idx << TAF_GATE_PRI_MAP_PROFILE_OFFSET);

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, taf_gate_pri_map_mem, MEM_BLOCK_ANY,
                     hw_idx, &hw_entry));

    *output_gate_id = soc_mem_field32_get(unit, taf_gate_pri_map_mem,
                                          &hw_entry, TAF_GATE_IDf);

    return BCM_E_NONE;
}


/* set taf gate pri map */
STATIC int
bcmi_gh2_taf_gate_pri_map_set(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_idx, prio, prio_num;

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    if (config->num_map_entries != TAF_GATE_PRI_MAP_INTPRI_NUM) {
        return BCM_E_PARAM;
    }

    if (config->map_type != bcmTsnPriMapTypeTafGate) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_taf_gate_pri_map_hw_idx_get(unit, sw_idx, &hw_idx));


    /* set config according to the input from user */
    prio_num = TAF_GATE_PRI_MAP_INTPRI_NUM;
    for (prio = 0; prio < prio_num; prio++) {

        if (config->map_entries[prio].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_taf_gate_pri_map_change(
                    unit, hw_idx, prio, 0,
                    config->map_entries[prio].taf_gate_express));
        } else {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_taf_gate_pri_map_change(
                    unit, hw_idx, prio, 0,
                    TAF_GATE_RESERVED_ID));
        }

        if (config->map_entries[prio].flags &
            BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_taf_gate_pri_map_change(
                    unit, hw_idx, prio, 1,
                    config->map_entries[prio].taf_gate_preempt));
        } else {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_taf_gate_pri_map_change(
                    unit, hw_idx, prio, 1,
                    TAF_GATE_RESERVED_ID));
        }
    }

    return BCM_E_NONE;
}

/* get taf gate pri map */
STATIC int
bcmi_gh2_taf_gate_pri_map_get(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_indx, prio, prio_num;

    if (NULL == config) {
        return BCM_E_PARAM;
    }
    bcm_tsn_pri_map_config_t_init(config);

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_taf_gate_pri_map_hw_idx_get(unit, sw_idx, &hw_indx));

    config->map_type = bcmTsnPriMapTypeTafGate;
    prio_num = TAF_GATE_PRI_MAP_INTPRI_NUM;
    config->num_map_entries = prio_num;

    for (prio = 0; prio < prio_num; prio++) {
        uint32 output_gate_id;

        config->map_entries[prio].flags = 0;

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_gate_pri_map_retrieve(
                unit, hw_indx, prio, 0, &output_gate_id));

        if (output_gate_id != TAF_GATE_RESERVED_ID) {
            config->map_entries[prio].taf_gate_express = output_gate_id;
            config->map_entries[prio].flags |=
                BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS;
        } else {
            config->map_entries[prio].taf_gate_express = -1;
        }

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_gate_pri_map_retrieve(
                unit, hw_indx, prio, 1, &output_gate_id));

        if (output_gate_id != TAF_GATE_RESERVED_ID) {
            config->map_entries[prio].taf_gate_preempt = output_gate_id;
            config->map_entries[prio].flags |=
                BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT;
        } else {
            config->map_entries[prio].taf_gate_preempt = -1;
        }
    }

    return BCM_E_NONE;
}

/* delete taf gate pri map */
STATIC int
bcmi_gh2_taf_gate_pri_map_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 hw_idx, prio, prio_num;

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_taf_gate_pri_map_hw_idx_get(unit, sw_idx, &hw_idx));

    prio_num = TAF_GATE_PRI_MAP_INTPRI_NUM;
    /* rollback to default gate */
    for (prio = 0; prio < prio_num; prio++) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_gate_pri_map_change(
                unit, hw_idx, prio, 0,
                TAF_GATE_RESERVED_ID));

         BCM_IF_ERROR_RETURN(
             bcmi_gh2_taf_gate_pri_map_change(
                 unit, hw_idx, prio, 1,
                 TAF_GATE_RESERVED_ID));
    }

    return BCM_E_NONE;
}

/* kick off a new calendar setting into hw
 * Note. this function would not modify profile state
 */
STATIC int
bcmi_gh2_taf_calendar_kickoff(
    int unit,
    int taf_gate_id,
    bcmi_tsn_taf_profile_t *profile)
{
    uint32 regval, setup_set, i, entry_byte;
    soc_mem_t setup_mem;
    soc_memacc_t memacc_cos, memacc_interval, memacc_gstate, memacc_maxbyte;
    uint32 *entry_ptr;
    bcm_tsn_taf_profile_t *calendar;
    bcmi_tsn_taf_schedule_entry_t *schedule;

    if (NULL == profile) {
        return BCM_E_PARAM;
    }
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    calendar = &(profile->data);
    schedule = profile->schedule;

    /* read active set, select another inactive set to setup */
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_CONFIG_1r, REG_PORT_ANY, taf_gate_id, &regval));
    setup_set = soc_reg_field_get(unit, TAF_CONFIG_1r,
                                  regval, ACTIVE_SETf);
    setup_set = 1 - setup_set;

    /* init mem */
    setup_mem = (setup_set == 1) ? taf_schedule_calendar_mem[1][taf_gate_id]
                                 : taf_schedule_calendar_mem[0][taf_gate_id];
    BCM_IF_ERROR_RETURN(
        soc_memacc_init(unit, setup_mem, TIME_INTERVALf, &memacc_interval));
    BCM_IF_ERROR_RETURN(
        soc_memacc_init(unit, setup_mem, GATE_STATEf, &memacc_gstate));
    BCM_IF_ERROR_RETURN(
        soc_memacc_init(unit, setup_mem, COS_PROFILEf, &memacc_cos));
    BCM_IF_ERROR_RETURN(
        soc_memacc_init(unit, setup_mem, MAXBYTES_SELECTf, &memacc_maxbyte));

    /* write calendar */
    entry_byte = WORDS2BYTES(soc_mem_entry_words(unit, setup_mem));
    sal_memset(taf_schedule_calendar_buf[unit], 0,
               TAF_SCHEDULE_CALENDAR_ENTIRES * entry_byte);
    for (i = 0; i < calendar->num_entries; i++) {
        entry_ptr = &(((uint32 *)(taf_schedule_calendar_buf[unit]))
                      [i * soc_mem_entry_words(unit, setup_mem)]);
        if ((calendar->entries)[i].ticks == BCM_TSN_TAF_ENTRY_TICKS_STAY) {
            int field_len = soc_mem_field_length(unit, setup_mem,
                                                 TIME_INTERVALf);

            soc_memacc_field32_set(&memacc_interval, entry_ptr,
                                   (1 << field_len) - 1);
        } else {
            soc_memacc_field32_set(&memacc_interval, entry_ptr,
                                   calendar->entries[i].ticks);
        }
        soc_memacc_field32_set(&memacc_gstate, entry_ptr,
                               calendar->entries[i].state);
        soc_memacc_field32_set(&memacc_cos, entry_ptr,
                               calendar->entries[i].cos_profile);
        soc_memacc_field32_set(&memacc_maxbyte, entry_ptr,
                               calendar->entries[i].maxbyte_profile);
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_write_range(unit, setup_mem, MEM_BLOCK_ALL, 0,
                            TAF_SCHEDULE_CALENDAR_ENTIRES - 1,
                            taf_schedule_calendar_buf[unit]));

    /* write time related parameter if operated in ptp mode */
    if (NULL != schedule) {
        uint32 sec_32 = COMPILER_64_LO(schedule->schedule_time.seconds);

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, taf_reg_base_sec[setup_set], REG_PORT_ANY,
                          taf_gate_id, &regval));
        soc_reg_field_set(unit, taf_reg_base_sec[setup_set], &regval,
                          SECf, sec_32 & TAF_SCHEDULE_SEC_MASK);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, taf_reg_base_sec[setup_set], REG_PORT_ANY,
                          taf_gate_id, regval));

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, taf_reg_base_frac[setup_set], REG_PORT_ANY,
                          taf_gate_id, &regval));
        soc_reg_field_set(unit, taf_reg_base_frac[setup_set], &regval,
                          FRACTIONf, schedule->schedule_time.nanoseconds);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, taf_reg_base_frac[setup_set], REG_PORT_ANY,
                          taf_gate_id, regval));

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, taf_reg_cycle[setup_set], REG_PORT_ANY,
                          taf_gate_id, &regval));
        soc_reg_field_set(unit, taf_reg_cycle[setup_set], &regval,
                          CYCLE_TIMEf, calendar->ptp_cycle_time);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, taf_reg_cycle[setup_set], REG_PORT_ANY,
                          taf_gate_id, regval));

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, taf_reg_ext[setup_set], REG_PORT_ANY,
                          taf_gate_id, &regval));
        soc_reg_field_set(unit, taf_reg_ext[setup_set], &regval,
                          CYCLE_TIME_EXTENSIONf,
                          calendar->ptp_max_cycle_extension);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, taf_reg_ext[setup_set], REG_PORT_ANY,
                          taf_gate_id, regval));

    }

    /* HW detect 0 to 1 change to start switching to new cycles.*/
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                      taf_gate_id, &regval));
    soc_reg_field_set(unit, TAF_CONFIG_0r, &regval,
                      CONFIG_CHANGEf, 0);
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                      taf_gate_id, regval));

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                      taf_gate_id, &regval));
    soc_reg_field_set(unit, TAF_CONFIG_0r, &regval,
                      CONFIG_CHANGEf, 1);
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, TAF_CONFIG_0r, REG_PORT_ANY,
                      taf_gate_id, regval));

    return BCM_E_NONE;
}

/* check the parameter of calendar */
STATIC int
bcmi_gh2_taf_calendar_param_check(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t *calendar)
{
    int i;
    soc_mem_t mem;

    if (NULL == calendar) {
        return BCM_E_PARAM;
    }

    if (calendar->num_entries <= 0 ||
        calendar->num_entries > TAF_SCHEDULE_CALENDAR_ENTIRES) {
        return BCM_E_PARAM;
    }

    if (calendar->entries[0].ticks == BCM_TSN_TAF_ENTRY_TICKS_GO_FIRST) {
        return BCM_E_PARAM;
    }

    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    mem = taf_schedule_calendar_mem[0][0];
    for (i = 0; i < calendar->num_entries; i++) {
        if (calendar->entries[i].ticks != BCM_TSN_TAF_ENTRY_TICKS_STAY) {
            BCM_IF_ERROR_RETURN(
                soc_mem_field32_fit(unit, mem, TIME_INTERVALf,
                                    calendar->entries[i].ticks));
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(unit, mem, GATE_STATEf,
                                calendar->entries[i].state));
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(unit, mem, COS_PROFILEf,
                                calendar->entries[i].cos_profile));
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_fit(unit, mem, MAXBYTES_SELECTf,
                                calendar->entries[i].maxbyte_profile));

        if (0 != calendar->entries[i].cos_profile) {
            if (calendar->entries[i].cos_profile >= TAF_COSQ_PROFILE_NUM) {
                return BCM_E_PARAM;
            }
            if (0 == taf_cosq_profile_refcount
                     [unit][calendar->entries[i].cos_profile]) {
                return BCM_E_PARAM;
            }
        }

        if (0 != calendar->entries[i].maxbyte_profile) {
            if (calendar->entries[i].maxbyte_profile >= TAF_MAXBYTE_PROFILE_NUM)
            {
                return BCM_E_PARAM;
            }
            if (0 == taf_maxbyte_profile_refcount
                     [unit][taf_gate][calendar->entries[i].maxbyte_profile]) {
                return BCM_E_PARAM;
            }
        }
    }

    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit, TAF_CYCLE_TIME_0r, CYCLE_TIMEf,
                               calendar->ptp_cycle_time));
    BCM_IF_ERROR_RETURN(
        soc_reg_field_validate(unit, TAF_CYCLE_TIME_EXTENSION_0r,
                               CYCLE_TIME_EXTENSIONf,
                               calendar->ptp_max_cycle_extension));
    if (calendar->ptp_base_time.nanoseconds >= TAF_SCHEDULE_NANOSEC_MAX) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/* for the profile (maxbyte and CosQ) in calendar, increase the ref_cnt
 *  Note. this function assume that the profile id is valid.
 *        the caller need to ensure that
 */
STATIC int
bcmi_gh2_taf_calendar_resource_alloc(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t *calendar)
{
    int i;

    if (NULL == calendar) {
        return BCM_E_PARAM;
    }

    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    for (i = 0; i < calendar->num_entries; i++) {
        if (0 != calendar->entries[i].cos_profile) {
            assert(calendar->entries[i].cos_profile < TAF_COSQ_PROFILE_NUM);
            assert(taf_cosq_profile_refcount
                   [unit][calendar->entries[i].cos_profile] != 0);
            taf_cosq_profile_refcount
                    [unit][calendar->entries[i].cos_profile]++;
        }

        if (0 != calendar->entries[i].maxbyte_profile) {
            assert(calendar->entries[i].maxbyte_profile
                    < TAF_MAXBYTE_PROFILE_NUM);
            assert(taf_maxbyte_profile_refcount
                   [unit][taf_gate][calendar->entries[i].maxbyte_profile] != 0);
            taf_maxbyte_profile_refcount
                   [unit][taf_gate][calendar->entries[i].maxbyte_profile]++;
        }
    }

    return BCM_E_NONE;
}

/* for the profile (maxbyte and CosQ) in calendar, decrease the ref_cnt
 *  Note. this function assume that the profile id is valid.
 *        the caller need to ensure that
 */
STATIC int
bcmi_gh2_taf_calendar_resource_free(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t *calendar)
{
    int i;

    if (NULL == calendar) {
        return BCM_E_PARAM;
    }

    assert(taf_gate >= 0 && taf_gate < _tsn_taf_gate_num(unit));

    for (i = 0; i < calendar->num_entries; i++) {
        if (0 != calendar->entries[i].cos_profile) {
            assert(calendar->entries[i].cos_profile < TAF_COSQ_PROFILE_NUM);
            assert(taf_cosq_profile_refcount
                   [unit][calendar->entries[i].cos_profile] != 0);
            taf_cosq_profile_refcount
                    [unit][calendar->entries[i].cos_profile]--;
        }

        if (0 != calendar->entries[i].maxbyte_profile) {
            assert(calendar->entries[i].maxbyte_profile
                    < TAF_MAXBYTE_PROFILE_NUM);
            assert(taf_maxbyte_profile_refcount
                   [unit][taf_gate][calendar->entries[i].maxbyte_profile] != 0);
            taf_maxbyte_profile_refcount
                   [unit][taf_gate][calendar->entries[i].maxbyte_profile]--;
        }
    }

    return BCM_E_NONE;
}

/* create a profile according to user's profile */
STATIC int
bcmi_gh2_taf_calendar_profile_create(
    int unit,
    bcm_tsn_taf_profile_t *calendar,
    bcm_tsn_taf_profile_id_t *ret_pid,
    bcmi_tsn_taf_profile_t **ret_profile)
{
    int profile_index;
    bcmi_tsn_taf_profile_t *new_profile = NULL;

    if (NULL == calendar || NULL == ret_profile || NULL == ret_pid) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_alloc(
            taf_schedule_profile_bitmap[unit], 0, 1, &profile_index));

    if (profile_index < 0 || profile_index >= BCMI_TSN_TAF_PROFILE_NUM_MAX) {
        return BCM_E_INTERNAL;
    }

    new_profile = sal_alloc(sizeof(bcmi_tsn_taf_profile_t), "TAF profile");
    if (NULL == new_profile) {
        return BCM_E_MEMORY;
    }
    sal_memset(new_profile, 0, sizeof(bcmi_tsn_taf_profile_t));
    taf_schedule_profile[unit][profile_index] = new_profile;
    *ret_profile = new_profile;
    *ret_pid = profile_index;

    return BCM_E_NONE;
}

/* get profile according to profile id */
STATIC int
bcmi_gh2_taf_calendar_profile_get(
    int unit,
    bcm_tsn_taf_profile_id_t profile_id,
    bcmi_tsn_taf_profile_t **ret_profile)
{
    if (NULL == ret_profile) {
        return BCM_E_PARAM;
    }

    assert(profile_id >= 0 && profile_id < BCMI_TSN_TAF_PROFILE_NUM_MAX);

    if (NULL == taf_schedule_profile[unit][profile_id]) {
        return BCM_E_NOT_FOUND;
    }

    *ret_profile = taf_schedule_profile[unit][profile_id];
    return BCM_E_NONE;
}

/* destroy a existing profile */
STATIC int
bcmi_gh2_taf_calendar_profile_destroy(
    int unit,
    bcm_tsn_taf_profile_id_t profile_id)
{
    assert(profile_id >= 0 && profile_id < BCMI_TSN_TAF_PROFILE_NUM_MAX);

    if (NULL == taf_schedule_profile[unit][profile_id]) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_free(taf_schedule_profile_bitmap[unit], 1, profile_id));
    sal_free(taf_schedule_profile[unit][profile_id]);
    taf_schedule_profile[unit][profile_id] = NULL;

    return BCM_E_NONE;
}

/* traverse existing profiles */
STATIC int
bcmi_gh2_taf_calendar_profile_traverse(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_traverse_cb cb,
    void *user_data)
{
    int i;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < BCMI_TSN_TAF_PROFILE_NUM_MAX; i++) {
        if (NULL != taf_schedule_profile[unit][i] &&
            taf_schedule_profile[unit][i]->taf_gate == taf_gate) {
            BCM_IF_ERROR_RETURN(
                cb(unit, taf_gate, i, user_data));
        }
    }

    return BCM_E_NONE;
}

/* remove a calendar entry from timeline
 * Note. we skip check whether entry_remove exitsed in
 *       taf_schedule_timeline[unit][taf_gate_id] or not.
 *        ==> The caller need to ensure this.
 */
STATIC int
bcmi_gh2_taf_schedule_remove(
    int unit,
    int taf_gate_id,
    bcmi_tsn_taf_profile_t *profile)
{
    bcmi_tsn_taf_schedule_entry_t *entry_remove;

    if (NULL == profile) {
        return BCM_E_PARAM;
    }
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    entry_remove = profile->schedule;
    if (NULL == entry_remove) {
        return BCM_E_INTERNAL;
    }

    if (entry_remove->prev != NULL) {
        entry_remove->prev->next = entry_remove->next;
    } else {
        taf_schedule_timeline[unit][taf_gate_id] = entry_remove->next;
    }

    if (entry_remove->next != NULL) {
        entry_remove->next->prev = entry_remove->prev;
    }

    sal_free(entry_remove);
    profile->schedule = NULL;
    return BCM_E_NONE;
}


/* check if this profile has already has scheduled in timeline
 * (only possible in ptp mode)
 */
STATIC int
bcmi_gh2_taf_schedule_exist(
    int unit,
    bcmi_tsn_taf_profile_t *profile,
    int *ret_is_exist)
{
    if (NULL == profile || NULL == ret_is_exist) {
        return BCM_E_PARAM;
    } else if (NULL == profile->schedule) {
        *ret_is_exist = 0;
    } else {
        *ret_is_exist = 1;
    }

    return BCM_E_NONE;
}

/* return actual scheduling time
 * if this profile has already has been scheduled
 */
STATIC int
bcmi_gh2_taf_schedule_get(
    int unit,
    bcmi_tsn_taf_profile_t *profile,
    bcm_ptp_timestamp_t *ret_schedule_time)
{
    if (NULL == profile || NULL == ret_schedule_time) {
        return BCM_E_PARAM;
    }

    if (NULL == profile->schedule) {
        return BCM_E_PARAM;
    }

    bcmi_gh2_taf_ptp_time_assign(ret_schedule_time,
                                 &(profile->schedule->schedule_time));

    return BCM_E_NONE;
}


/* create a new calendar entry and insert it into timeline */
STATIC int
bcmi_gh2_taf_schedule_insert(
    int unit,
    int taf_gate_id,
    bcm_ptp_timestamp_t *schedule_basetime,
    bcmi_tsn_taf_profile_t *profile)
{
    bcm_ptp_timestamp_t curr_time;
    bcmi_tsn_taf_schedule_entry_t *entry_iter = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_pos = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_tail = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_new = NULL;

    if (NULL == profile || NULL == schedule_basetime) {
        return BCM_E_PARAM;
    }

    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_ptp_time_get(unit, &curr_time));
    }

    if (TRUE == bcmi_gh2_taf_ptp_time_gt(&curr_time, schedule_basetime)) {
        return BCM_E_PARAM;
    }

    /* find the position to insert
     *    ex. there have already four entry in timeline
     *
     *         basetime     10s         20s         25s         30s         40s
     *                   ----|------------|----------|-----------|-----------|-
     *                       A            B          C           D           E
     *
     *         if schedule_basetime = 25s, entry_pos would be D
     */
    entry_iter = taf_schedule_timeline[unit][taf_gate_id];
    while (entry_iter != NULL) {
        if (TRUE == bcmi_gh2_taf_ptp_time_ge(
                        schedule_basetime,
                        &(entry_iter->schedule_time))) {
            if (NULL == entry_iter->next) {
                entry_tail = entry_iter;
            }
            entry_iter = entry_iter->next;
        } else {
            break; /* find out the position */
        }
    }
    entry_pos = entry_iter;

    /* insert new entry */
    entry_new = sal_alloc(sizeof(bcmi_tsn_taf_schedule_entry_t),
                          "TAF schedule entry");
    if (NULL == entry_new) {
        return BCM_E_MEMORY;
    }
    bcmi_gh2_taf_ptp_time_assign(&(entry_new->schedule_time),
                                 schedule_basetime);
    if (NULL == taf_schedule_timeline[unit][taf_gate_id]) {
        /* entry_new is a first entry in timline */
        entry_new->prev = NULL;
        entry_new->next = NULL;
        taf_schedule_timeline[unit][taf_gate_id] = entry_new;
    } else {
        if (entry_pos == NULL) {
            /* insert entry_new at tail */
            entry_new->prev = entry_tail;
            entry_new->next = NULL;
            entry_tail->next = entry_new;
        } else {
            /* add entry_new before entry_pos */
            entry_new->prev = entry_pos->prev;
            entry_new->next = entry_pos;
            if (entry_pos->prev == NULL) {
                taf_schedule_timeline[unit][taf_gate_id] = entry_new;
            } else {
                entry_pos->prev->next = entry_new;
            }
            entry_pos->prev = entry_new;
        }
    }

    /* remove old schedule if need */
    if (profile->schedule != NULL) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_schedule_remove(unit, taf_gate_id, profile));
    }

    /* setup connection */
    entry_new->taf_profile = profile;
    profile->schedule = entry_new;
    return BCM_E_NONE;
}

/* select a candidate entry from timeline
 *  which is able to be scheduled as "pending"
 *     Note. 1. after decide candidate,
 *              it is possible that it just update its scheduling time
 *              and not write into hw yet
 *           2. this function maybe remove some entries
 *              and change their state to expired
 */
STATIC int
bcmi_gh2_taf_schedule_pick(
    int unit,
    int taf_gate_id,
    bcmi_tsn_taf_profile_t *active_profile,
    bcmi_tsn_taf_profile_t **candidate_profile)
{
    bcm_ptp_timestamp_t curr_time;
    bcm_ptp_timestamp_t candidate_time;
    bcmi_tsn_taf_schedule_entry_t *entry_candidate = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_iter = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_back = NULL;
    bcmi_tsn_taf_schedule_entry_t *entry_fore = NULL;
    uint32 need_recheck_candicate = FALSE;
    uint64 tod_curr;
    uint64 tod_candidate;
    uint32 okay_to_kick_off = FALSE;

    if (NULL == candidate_profile) {
        return BCM_E_PARAM;
    }

    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    /* find candidate
     *    step 1. according to the current time, traverse backward fisrt
     *    step 2. if it cannot find anything in step 1, traverse foreward
     *
     *     ex 1.
     *         1       2
     *       --|-------|-------|---         ==> pick entry 2
     *                     curr_time
     *
     *     ex 2.
     *                1              2
     *       ---------|-------|------|-     ==> pick entry 1
     *                     curr_time
     *     ex 3.
     *         1      2               3
     *       --|-------|-------|------|-    ==> pick entry 2
     *                     curr_time
     */


    {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_ptp_time_get(unit, &curr_time));
    }

    entry_iter = taf_schedule_timeline[unit][taf_gate_id];
    while (entry_iter != NULL) {

        if (TRUE == bcmi_gh2_taf_ptp_time_gt(
                        &(entry_iter->schedule_time), &curr_time)) {
            entry_fore = entry_iter;
            entry_back = entry_iter->prev;
            break;
        } else {
            if (NULL == entry_iter->next) {
                entry_fore = NULL;
                entry_back = entry_iter;
                break;
            } else {
                entry_iter = entry_iter->next;
            }
        }
    }
    if (NULL != entry_back) {
        entry_candidate = entry_back;
    } else {
        entry_candidate = entry_fore;
    }

    if (NULL == entry_candidate) {
        /* cannot find out any candidate */
        return BCM_E_NONE;
    }

    /*  Calaulate the actual scheduling time of candidate.
     *     because of GH2 hw limitaion, maybe we need to delay scheduling time
     *     [rule] the actual scheduling time needs to be at least
     *            "2 * old-cycle + hw process + sw process time"
     *            far away from the current time
     *
     *  After moving scheduling time foreward,
     *  maybe we needs to select the new candidate
     *
     *     ex. At first, we select entry 2 as a candidate.
     *
     *               2(c)                 3(c)    4(c)    5(c)
     *       --|-------|-------|------------|-------|-------|-------
     *                20s   curr_time(30s)  40s     50s    60s
     *
     *        Assume that after calculating, the new scheduling time is at 55s
     *        Then we need to select entry 4 as new candidate instead.
     *        And re-calaulate the actual scheduling time of entry 4
     *
     *                                            4(c)    5(c)
     *       --|---------------|--------------------|-------|-------
     *                      curr_time(30s)          50s    60s
     *
     *        remove entry 2 and 3 from timelime (mark their status as expired)
     *        at the end of this function
     */
     need_recheck_candicate = TRUE;
     while (TRUE == need_recheck_candicate) {
        bcm_ptp_timestamp_t schedule_time_at_least;

        bcmi_gh2_taf_ptp_time_assign(&(candidate_time),
                                     &(entry_candidate->schedule_time));

        /* calculate the actual scheduling time of candidate */
        {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_taf_ptp_time_get(unit, &curr_time));
        }

        LOG_VERBOSE(BSL_LS_BCM_TSN,
            (BSL_META("[taf_schedule_pick] curr_time : \
                       sec:%d %d, nano:%d\n"),
                       COMPILER_64_HI(curr_time.seconds),
                       COMPILER_64_LO(curr_time.seconds),
                       curr_time.nanoseconds));

        /* schedule_time_at_least =
         *  curr_time + 2 * old-cycle + hw process + sw process time
         */
        bcmi_gh2_taf_ptp_time_assign(&schedule_time_at_least, &curr_time);
        if (active_profile != NULL) {
            bcmi_gh2_taf_ptp_time_add(
                &schedule_time_at_least,
                2 * (active_profile->data.ptp_cycle_time));
        }

        {
            bcmi_gh2_taf_ptp_time_add(&schedule_time_at_least,
                                      TAF_SCHEDULE_HW_RESPONSE_TIME);
            bcmi_gh2_taf_ptp_time_add(&schedule_time_at_least,
                                      TAF_SCHEDULE_SW_RESPONSE_TIME);
        }

        LOG_VERBOSE(BSL_LS_BCM_TSN,
            (BSL_META("[taf_schedule_pick] schedule_time_at_least : \
                       sec:%d %d, nano:%d\n"),
                       COMPILER_64_HI(schedule_time_at_least.seconds),
                       COMPILER_64_LO(schedule_time_at_least.seconds),
                       schedule_time_at_least.nanoseconds));

        while (TRUE == bcmi_gh2_taf_ptp_time_gt(&schedule_time_at_least,
                                                &candidate_time)) {
            /* Add N x new_cycle_time until > schedule_time_at_least */
            bcmi_gh2_taf_ptp_time_add(
                &candidate_time,
                entry_candidate->taf_profile->data.ptp_cycle_time);
        }

        LOG_VERBOSE(BSL_LS_BCM_TSN,
            (BSL_META("[taf_schedule_pick] candidate_time : \
                       sec:%d %d, nano:%d\n"),
                       COMPILER_64_HI(candidate_time.seconds),
                       COMPILER_64_LO(candidate_time.seconds),
                       candidate_time.nanoseconds));

        /* check this new scheduling time would not be larger
         * than the next entries
         */
        need_recheck_candicate = FALSE;
        entry_iter = entry_candidate;
        while (NULL != entry_iter) {
            if (NULL == entry_iter->next) {
                break;
            }
            if (TRUE == bcmi_gh2_taf_ptp_time_gt(
                            &candidate_time,
                            &(entry_iter->next->schedule_time))) {
                need_recheck_candicate = TRUE;
                entry_iter = entry_iter->next;
            } else {
                break;
            }
        }
        if (TRUE == need_recheck_candicate) {
            entry_candidate = entry_iter; /* new candidate */
        }
    }

    if (NULL == entry_candidate) {
        return BCM_E_INTERNAL;
    }

    /* for this candidate
     *   1. udpate its scheduling time
     *   2. kick off this new scheduling time if okay
     *      [rule]
     *          1) need to locate in the same TOD
     *          2) actual schedule time need to be 1 old-cycle time
     *             far away from the boundary of ToD
     *      ex.
     *        -----|-----------|-----------|-----------
     *            64s    ^   128s    ^    192s
     *                   |           |
     *                 candidate    cur_time (130s)
     *
     *        Assume that the old cycle time is 100ns
     *        After calculating in the previous step:
     *        Case A. the scheduling time is updated to 140s
     *                  ==> it is okay to kick off
     *        Case B. the scheduling time is updated to 193s
     *                  ==> just update its scheduling time
     *                      delay its kick-off until the next ToD interrupt
     */
    okay_to_kick_off = FALSE;
    bcmi_gh2_taf_ptp_time_assign(
        &(entry_candidate->schedule_time), &candidate_time);
    bcmi_gh2_taf_ptp_time_to_tod(&curr_time, &tod_curr);
    bcmi_gh2_taf_ptp_time_to_tod(&candidate_time, &tod_candidate);
    if (COMPILER_64_EQ(tod_curr, tod_candidate)) {
        if (active_profile != NULL) {
            bcm_ptp_timestamp_t tod_boundary;
            uint64 tod_next;

            COMPILER_64_COPY(tod_next, tod_curr);
            COMPILER_64_ADD_32(tod_next, 1);
            bcmi_gh2_taf_ptp_time_from_tod(&tod_next, &tod_boundary);
            bcmi_gh2_taf_ptp_time_sub(
                &tod_boundary,
                active_profile->data.ptp_cycle_time);
            if (TRUE == bcmi_gh2_taf_ptp_time_gt(&tod_boundary,
                                                 &candidate_time)) {
                okay_to_kick_off = TRUE;
            }
        } else {
            okay_to_kick_off = TRUE;
        }
    }
    if (TRUE == okay_to_kick_off) {
        *candidate_profile = entry_candidate->taf_profile;
    } else {
        *candidate_profile = NULL;
    }

    LOG_VERBOSE(BSL_LS_BCM_TSN,
        (BSL_META("[taf_schedule_pick] okay_to_kick_off : %s\n"),
                   (TRUE == okay_to_kick_off) ? "Yes" : "No"));

    /*
     * delete all entries before this candidate from timeline
     */
    entry_iter = entry_candidate->prev;
    while (NULL != entry_iter) {
        bcmi_tsn_taf_schedule_entry_t *entry_prev = entry_iter->prev;

        BCM_IF_ERROR_RETURN(
            bcmi_gh2_taf_schedule_remove(
                unit, taf_gate_id, entry_iter->taf_profile));
        entry_iter->taf_profile->status = bcmTsnTafProfileExpired;
        entry_iter = entry_prev;
    }

    return BCM_E_NONE;
}

/* the 1st function need to be invoked to handle hw interrupt */
STATIC int
bcmi_gh2_taf_interrupt_handle_start(int unit)
{
    uint32 status;
    uint32 mask;
    gh2_taf_interrupt_type_t iter;
    int taf_interrupt_sub_field_num;

    taf_interrupt_sub_field_num = _taf_interrupt_sub_field_num(unit);

    if (1 == taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_MEMFAILINTSTATUSr, REG_PORT_ANY, 0, &status));

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, TAF_MEMFAILINTMASKr, REG_PORT_ANY, 0, &mask));

    taf_interrupt_status[unit] = status & mask;

    for (iter = 0; iter < TafInterruptCount; iter++) {
        if (taf_interrupt_mapping[iter].sub_status_regs[0] != INVALIDr) {
            int index;

            if (soc_reg_field_get(
                    unit, TAF_MEMFAILINTSTATUSr, taf_interrupt_status[unit],
                    taf_interrupt_mapping[iter].status_field)) {

                for (index = 0; index < taf_interrupt_sub_field_num; index++) {
                    BCM_IF_ERROR_RETURN(
                        soc_reg32_get(
                            unit,
                            taf_interrupt_mapping[iter].sub_status_regs[index],
                            REG_PORT_ANY, 0,
                            &taf_interrupt_substatus[unit][iter][index]));
                }
            } else {
                for (index = 0; index < TAF_INTERRUPT_SUB_FIELD_NUM; index++) {
                    taf_interrupt_substatus[unit][iter][index] = 0;
                }
            }
        }
    }

    taf_interrupt_handling[unit] = 1;
    return BCM_E_NONE;
}

/* indicate that whether need to handle Tod window interupt event */
STATIC int
bcmi_gh2_taf_interrupt_handle_tod(int unit, int *ret_need)
{
    if (1 != taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }
    if (NULL == ret_need) {
        return BCM_E_PARAM;
    }

    *ret_need = soc_reg_field_get(
                    unit, TAF_MEMFAILINTSTATUSr, taf_interrupt_status[unit],
                    taf_interrupt_mapping[TafInterruptTodWindow].status_field);

    return BCM_E_NONE;
}

/* indicate that whether need to handle profile switch done event
 *   ret_need(OUT)        : whether to handle profile switch
 *   ret_bmp_gate(IN/OUT) : return the bitmap of taf_gate need to handle
 *   bmp_size(IN)         : the size of bitmap
 */
STATIC int
bcmi_gh2_taf_interrupt_handle_switch(
    int unit,
    int *ret_need,
    SHR_BITDCL *ret_bmp_gate,
    uint32 bmp_size)
{
    int index;
    int taf_interrupt_sub_field_num;

    taf_interrupt_sub_field_num = _taf_interrupt_sub_field_num(unit);

    if (1 != taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }
    if (NULL == ret_need || NULL == ret_bmp_gate) {
        return BCM_E_PARAM;
    }
    if (bmp_size < _SHR_BITDCLSIZE(_tsn_taf_gate_num(unit))) {
        return BCM_E_PARAM;
    }

    for (index = 0; index < taf_interrupt_sub_field_num; index++) {
        ret_bmp_gate[index] =
          taf_interrupt_substatus[unit][TafInterruptSwitchDone][index];
    }

    SHR_BITTEST_RANGE(ret_bmp_gate, 0, _tsn_taf_gate_num(unit), *ret_need);
    return BCM_E_NONE;
}

/* indicate that whether need to handle profile become error
 *   ret_need(OUT)        : whether to handle profile error
 *   ret_bmp_gate(IN/OUT) : return the bitmap of taf_gate need to handle
 *   bmp_size(IN)         : the size of bitmap
 */
STATIC int
bcmi_gh2_taf_interrupt_handle_error(
    int unit,
    int *ret_need,
    SHR_BITDCL *ret_bmp_gate,
    uint32 bmp_size)
{
    int index;
    int taf_interrupt_sub_field_num;

    taf_interrupt_sub_field_num = _taf_interrupt_sub_field_num(unit);

    if (1 != taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }
    if (NULL == ret_need || NULL == ret_bmp_gate) {
        return BCM_E_PARAM;
    }
    if (bmp_size < _SHR_BITDCLSIZE(_tsn_taf_gate_num(unit))) {
        return BCM_E_PARAM;
    }

    for (index = 0; index < taf_interrupt_sub_field_num; index++) {
        ret_bmp_gate[index] =
          taf_interrupt_substatus[unit][TafInterruptNextCycleTimeError][index] |
          taf_interrupt_substatus[unit][TafInterruptParityError][index] |
          taf_interrupt_substatus[unit][TafInterruptTblPtrError][index] |
          taf_interrupt_substatus[unit][TafInterruptBaseTimeError][index];
    }

    SHR_BITTEST_RANGE(ret_bmp_gate, 0, _tsn_taf_gate_num(unit), *ret_need);
    return BCM_E_NONE;
}

/* invoke the callback function hooked by the user */
STATIC int
bcmi_gh2_taf_interrupt_handle_user_cb(int unit)
{
    gh2_taf_interrupt_type_t iter;
    int taf_interrupt_sub_field_num;

    taf_interrupt_sub_field_num = _taf_interrupt_sub_field_num(unit);

    if (1 != taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }

    for (iter = 0; iter < TafInterruptCount; iter++) {
        bcm_tsn_taf_event_type_t event;
        int i, j;

        switch (iter) {
            case TafInterruptSwitchDone:
                event = bcmTsnTafEventTAFProfileChange;
                break;
            case TafInterruptNextCycleTimeError:
                event = bcmTsnTafEventTAFNextCycleTimeErr;
                break;
            case TafInterruptBaseTimeError:
                event = bcmTsnTafEventTAFBaseTimeErr;
                break;
            case TafInterruptParityError:
                event = bcmTsnTafEventTAFECCErr;
                break;
            case TafInterruptTblPtrError:
                event = bcmTsnTafEventTAFProfilePtrErr;
                break;
            default: /* user would not register cb for TafInterruptTodWindow */
                continue;
        }
        if (0 == soc_reg_field_get(
            unit, TAF_MEMFAILINTSTATUSr,
            taf_interrupt_status[unit],
            taf_interrupt_mapping[iter].status_field)) {
            continue;
        }
        for (i = 0; i < taf_interrupt_sub_field_num; i++) {
            for (j = 0; j < TAF_INTERRUPT_SUB_FIELD_WIDTH; j++) {
                if (taf_interrupt_substatus[unit][iter][i] & (1 << j)) {
                    BCM_IF_ERROR_RETURN(
                        bcmi_gh2_taf_event_notify(
                            unit,
                            i * TAF_INTERRUPT_SUB_FIELD_WIDTH + j,
                            event));
                }
            }
        }
    }
    return BCM_E_NONE;
}

/* the last function should be invoked after finish handling */
STATIC int
bcmi_gh2_taf_interrupt_handle_finish(int unit)
{
    gh2_taf_interrupt_type_t iter;
    uint32 reg_value;
    int taf_interrupt_sub_field_num;

    taf_interrupt_sub_field_num = _taf_interrupt_sub_field_num(unit);

    if (1 != taf_interrupt_handling[unit]) {
        return BCM_E_INTERNAL;
    }

    /* clear interrupt for subfiled */
    for (iter = 0; iter < TafInterruptCount; iter++) {
        if (taf_interrupt_mapping[iter].sub_status_regs[0] != INVALIDr) {
            int index;

            for (index = 0; index < taf_interrupt_sub_field_num; index++) {
                reg_value = taf_interrupt_substatus[unit][iter][index];
                if (0 == reg_value) {
                    continue;
                }

                BCM_IF_ERROR_RETURN(
                    soc_reg32_set(
                        unit,
                        taf_interrupt_mapping[iter].sub_clear_regs[index],
                        REG_PORT_ANY, 0, reg_value));
            }
        }
    }

    /* clear interrupt */
    reg_value = taf_interrupt_status[unit];
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, TAF_MEMFAILINT_CLRr,
                      REG_PORT_ANY, 0, reg_value));

    taf_interrupt_handling[unit] = 0;
    return BCM_E_NONE;
}

/* helper function to set maximum bytes */
STATIC int
_bcmi_gh2_taf_gate_max_bytes_profile_set(
    int unit, int taf_gate_id, int profile_id, uint64 max_bytes)
{
    taf_gate_control_entry_t mem_entry;
    int field_len;
    uint64 field_mask64;
    uint64 value_mask64;

    assert(profile_id >= 0 && profile_id < TAF_MAXBYTE_PROFILE_NUM);

    /* check value range */
    field_len = soc_mem_field_length(
                    unit, taf_maxbyte_profile_mem,
                    taf_maxbyte_profile_field[profile_id]);
    COMPILER_64_SET(field_mask64, 0, 1);
    COMPILER_64_SHL(field_mask64, field_len);
    COMPILER_64_SUB_32(field_mask64, 1);

    COMPILER_64_COPY(value_mask64, max_bytes);
    COMPILER_64_AND(value_mask64, field_mask64);
    if (COMPILER_64_NE(value_mask64, max_bytes)) {
        return BCM_E_PARAM;
    }

    /* write field */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, taf_maxbyte_profile_mem, MEM_BLOCK_ANY,
                     taf_gate_id, &mem_entry));

    soc_mem_field64_set(
        unit, taf_maxbyte_profile_mem, &mem_entry,
        taf_maxbyte_profile_field[profile_id], max_bytes);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, taf_maxbyte_profile_mem, MEM_BLOCK_ALL,
                      taf_gate_id, &mem_entry));

    return BCM_E_NONE;
}

/* helper function to get maximum bytes */
STATIC int
_bcmi_gh2_taf_gate_max_bytes_profile_get(
    int unit, int taf_gate_id, int profile_id, uint64 *max_bytes)
{
    taf_gate_control_entry_t mem_entry;

    if (NULL == max_bytes) {
        return BCM_E_PARAM;
    }
    assert(profile_id >= 0 && profile_id < TAF_MAXBYTE_PROFILE_NUM);

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, taf_maxbyte_profile_mem, MEM_BLOCK_ANY,
                     taf_gate_id, &mem_entry));

    soc_mem_field64_get(unit, taf_maxbyte_profile_mem, &mem_entry,
                        taf_maxbyte_profile_field[profile_id], max_bytes);

    return BCM_E_NONE;
}

/* create profile of maximum bytes */
STATIC int
bcmi_gh2_taf_gate_max_bytes_profile_create(
    int unit, int taf_gate_id, uint64 max_bytes, int *profile_id)
{
    if (NULL == profile_id) {
        return BCM_E_PARAM;
    }
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_alloc(taf_maxbyte_profile_bitmap[unit][taf_gate_id],
                             0, 1, profile_id));

    if (*profile_id < 0 || *profile_id >= TAF_MAXBYTE_PROFILE_NUM ||
        *profile_id == TAF_MAXBYTE_PROFILE_RESERVED_ID) {
        return BCM_E_INTERNAL;
    }

    ERROR_RETURN_WITH_EXTRA_CLEAN(
        _bcmi_gh2_taf_gate_max_bytes_profile_set(
            unit, taf_gate_id, *profile_id, max_bytes),
        shr_res_bitmap_free(taf_maxbyte_profile_bitmap[unit][taf_gate_id],
                            1, *profile_id));

    taf_maxbyte_profile_refcount[unit][taf_gate_id][*profile_id] = 1;

    return BCM_E_NONE;
}

/* set profile of maximum bytes */
STATIC int
bcmi_gh2_taf_gate_max_bytes_profile_set(
    int unit, int taf_gate_id, int profile_id, uint64 max_bytes)
{
    if (profile_id < 0 || profile_id >= TAF_MAXBYTE_PROFILE_NUM ||
        profile_id == TAF_MAXBYTE_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    if (taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] == 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcmi_gh2_taf_gate_max_bytes_profile_set(
            unit, taf_gate_id, profile_id, max_bytes));

    return BCM_E_NONE;
}

/* get profile of maximum bytes */
STATIC int
bcmi_gh2_taf_gate_max_bytes_profile_get(
    int unit, int taf_gate_id, int profile_id, uint64 *max_bytes)
{
    if (profile_id < 0 || profile_id >= TAF_MAXBYTE_PROFILE_NUM ||
        profile_id == TAF_MAXBYTE_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }
    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    if (taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] == 0) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(
        _bcmi_gh2_taf_gate_max_bytes_profile_get(
            unit, taf_gate_id, profile_id, max_bytes));

    return BCM_E_NONE;
}

/* traverse profile of maximum bytes */
STATIC int
bcmi_gh2_taf_gate_max_bytes_profile_traverse(
    int unit, int taf_gate_id,
    bcm_tsn_taf_gate_max_bytes_profile_traverse_cb cb,
    void *user_data)
{
    int profile_id;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    for (profile_id = 0; profile_id < TAF_MAXBYTE_PROFILE_NUM; profile_id++) {
        if (TAF_MAXBYTE_PROFILE_RESERVED_ID == profile_id) {
            continue;
        }
        if (taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] == 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            cb(unit, taf_gate_id, profile_id, user_data));
    }

    return BCM_E_NONE;
}

/* destroy profile of maximum bytes */
STATIC int
bcmi_gh2_taf_gate_max_bytes_profile_destroy(
    int unit, int taf_gate_id, int profile_id)
{
    if (profile_id < 0 || profile_id >= TAF_MAXBYTE_PROFILE_NUM ||
        profile_id == TAF_MAXBYTE_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }

    assert(taf_gate_id >= 0 && taf_gate_id < _tsn_taf_gate_num(unit));

    if (taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] == 0) {
        return BCM_E_NOT_FOUND;
    }
    else if (taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] > 1) {
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_free(taf_maxbyte_profile_bitmap[unit][taf_gate_id],
                            1, profile_id));
    taf_maxbyte_profile_refcount[unit][taf_gate_id][profile_id] = 0;

    return BCM_E_NONE;
}

/* helper function to set cosq mapping */
STATIC int
_bcmi_gh2_taf_cosq_profile_set(
    int unit, int profile_id, bcm_cos_t prio, bcm_cos_queue_t cosq)
{
    int select;

    if (profile_id < 0 || profile_id >= TAF_COSQ_PROFILE_NUM ||
        prio < 0 || prio >= TAF_COSQ_INTPRI_NUM) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_field32_fit(unit, taf_cosq_profile_mem,
                            taf_cosq_profile_field, cosq));

    for (select = 0; select < TAF_COSQ_SELECT_NUM; select++) {
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_modify(
                unit, taf_cosq_profile_mem,
                TAF_COSQ_HW_INDEX(profile_id, select, prio),
                taf_cosq_profile_field, cosq));
    }
    return BCM_E_NONE;
}

/* helper function to get cosq mapping */
STATIC int
_bcmi_gh2_taf_cosq_profile_get(
    int unit, int profile_id, bcm_cos_t prio, bcm_cos_queue_t *cosq)
{
    port_cos_map_entry_t mem_entry;

    if (profile_id < 0 || profile_id >= TAF_COSQ_PROFILE_NUM ||
        prio < 0 || prio >= TAF_COSQ_INTPRI_NUM) {
        return BCM_E_PARAM;
    }

    if (NULL == cosq) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, taf_cosq_profile_mem, MEM_BLOCK_ANY,
                     TAF_COSQ_HW_INDEX(profile_id, 0, prio),
                     &mem_entry));

    *cosq = soc_mem_field32_get(unit, taf_cosq_profile_mem, &mem_entry,
                                taf_cosq_profile_field);
    return BCM_E_NONE;
}

/* create profile of cosq mapping */
STATIC int
bcmi_gh2_taf_cosq_mapping_profile_create(
    int unit, int *cosq_profile)
{
    if (NULL == cosq_profile) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_alloc(taf_cosq_profile_bitmap[unit],
                             0, 1, cosq_profile));

    if (*cosq_profile < 0 || *cosq_profile >= TAF_COSQ_PROFILE_NUM ||
        *cosq_profile == TAF_COSQ_PROFILE_RESERVED_ID) {
        return BCM_E_INTERNAL;
    }

    taf_cosq_profile_refcount[unit][*cosq_profile] = 1;

    return BCM_E_NONE;
}

/* set profile of cosq mapping */
STATIC int
bcmi_gh2_taf_cosq_mapping_profile_set(
    int unit, int cosq_profile, bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    if (cosq_profile < 0 || cosq_profile >= TAF_COSQ_PROFILE_NUM ||
        cosq_profile == TAF_COSQ_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }

    if (taf_cosq_profile_refcount[unit][cosq_profile] == 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcmi_gh2_taf_cosq_profile_set(
            unit, cosq_profile, priority, cosq));

    return BCM_E_NONE;
}

/* get profile of cosq mapping */
STATIC int
bcmi_gh2_taf_cosq_mapping_profile_get(
    int unit, int cosq_profile, bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    if (cosq_profile < 0 || cosq_profile >= TAF_COSQ_PROFILE_NUM ||
        cosq_profile == TAF_COSQ_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }

    if (taf_cosq_profile_refcount[unit][cosq_profile] == 0) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(
        _bcmi_gh2_taf_cosq_profile_get(
            unit, cosq_profile, priority, cosq));

    return BCM_E_NONE;
}

/* traverse profile of cosq mapping */
STATIC int
bcmi_gh2_taf_cosq_mapping_profile_traverse(
    int unit,
    bcm_tsn_taf_cosq_mapping_profile_traverse_cb cb,
    void *user_data)
{
    int profile_id;

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    for (profile_id = 0; profile_id < TAF_COSQ_PROFILE_NUM; profile_id++) {
        if (TAF_COSQ_PROFILE_RESERVED_ID == profile_id) {
            continue;
        }
        if (taf_cosq_profile_refcount[unit][profile_id] == 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN(cb(unit, profile_id, user_data));
    }

    return BCM_E_NONE;
}

/* destroy profile of cosq mapping */
STATIC int
bcmi_gh2_taf_cosq_mapping_profile_destroy(
    int unit, int cosq_profile)
{
    if (cosq_profile < 0 || cosq_profile >= TAF_COSQ_PROFILE_NUM ||
        cosq_profile == TAF_COSQ_PROFILE_RESERVED_ID) {
        return BCM_E_PARAM;
    }

    if (taf_cosq_profile_refcount[unit][cosq_profile] == 0) {
        return BCM_E_NOT_FOUND;
    } else if (taf_cosq_profile_refcount[unit][cosq_profile] > 1) {
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(
        shr_res_bitmap_free(taf_cosq_profile_bitmap[unit], 1, cosq_profile));
    taf_cosq_profile_refcount[unit][cosq_profile] = 0;

    return BCM_E_NONE;
}

/*
 * update flow counter for both hw table and sw database(taf_gate_counter)
 *
 * if is_write = 0 : read hw data and sync back to sw
 * if is_write = 1 : write parameter "write_val" into both sw/hw
 */
STATIC int
bcmi_gh2_taf_gate_stat_counter_update(
    int unit,
    int taf_gate_id,
    bcm_tsn_taf_gate_stat_t stat_type_min,
    bcm_tsn_taf_gate_stat_t stat_type_max,
    int is_write,
    uint64 write_val)
{
    bcm_tsn_taf_gate_stat_t stat_type;

    if (taf_gate_id < 0 ||
        taf_gate_id >= _tsn_taf_gate_num(unit) ||
        stat_type_min >= bcmTsnTafGateStatCount ||
        stat_type_max >= bcmTsnTafGateStatCount ||
        stat_type_min > stat_type_max) {
        return BCM_E_PARAM;
    }

    for (stat_type = stat_type_min; stat_type <= stat_type_max; stat_type++) {
        soc_mem_t mem = taf_gate_cnt_mem[stat_type];
        soc_field_t field = taf_gate_cnt_field[stat_type];
        soc_memacc_t memacc_count;
        uint64 *update_array_ptr = taf_gate_counter[unit][stat_type];
        uint8 *buffer_ptr = taf_gate_counter_dma_buffer[unit][stat_type];
        uint64 count_mask64;

        BCM_IF_ERROR_RETURN(
            soc_memacc_init(unit, mem, field, &memacc_count));

        COMPILER_64_SET(count_mask64, 0, 1);
        COMPILER_64_SHL(count_mask64, soc_mem_field_length(unit, mem, field));
        COMPILER_64_SUB_32(count_mask64, 1);

        if (0 == is_write) {
            /* read hw data and sync to sw */
            uint64 hw_count, sw_last_count;

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                             taf_gate_id, buffer_ptr));
            soc_memacc_field64_get(&memacc_count, buffer_ptr, &hw_count);
            COMPILER_64_COPY(sw_last_count, update_array_ptr[taf_gate_id]);
            COMPILER_64_AND(sw_last_count, count_mask64);

            if (COMPILER_64_GT(sw_last_count, hw_count)) {
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "Roll over happened\n")));
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        ".Read Packet64 Count: %x:%x\n"),
                             COMPILER_64_HI(hw_count),
                             COMPILER_64_LO(hw_count)));
                COMPILER_64_ADD_64(hw_count, count_mask64);
                COMPILER_64_ADD_32(hw_count, 1);
                COMPILER_64_SUB_64(hw_count, sw_last_count);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        ".Diffed 64-Packet Count: %x:%x\n"),
                             COMPILER_64_HI(hw_count),
                             COMPILER_64_LO(hw_count)));
            } else {
                COMPILER_64_SUB_64(hw_count, sw_last_count);
            }

            /* Add difference (if it is) */
            if (!COMPILER_64_IS_ZERO(hw_count)) {
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "gate id:%d==>"
                                        "Old 64-Packet Count Value%x:%x\t"),
                             taf_gate_id,
                             COMPILER_64_HI(update_array_ptr[taf_gate_id]),
                             COMPILER_64_LO(update_array_ptr[taf_gate_id])));
                COMPILER_64_ADD_64(update_array_ptr[taf_gate_id], hw_count);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                       "New 64-Packet Value : %x:%x\n"),
                             COMPILER_64_HI(update_array_ptr[taf_gate_id]),
                             COMPILER_64_LO(update_array_ptr[taf_gate_id])));
            }
        } else {
            /* write hw */
            uint64 write_val_after_mask;

            COMPILER_64_COPY(write_val_after_mask, write_val);
            COMPILER_64_AND(write_val_after_mask, count_mask64);
            if (COMPILER_64_NE(write_val_after_mask, write_val)) {
                return BCM_E_PARAM;
            }
            soc_memacc_field64_set(&memacc_count, buffer_ptr,
                                   write_val_after_mask);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                              taf_gate_id, buffer_ptr));

            /* write sw */
            COMPILER_64_COPY(update_array_ptr[taf_gate_id], write_val);
        }
    }

    return BCM_E_NONE;
}

/* helper function for taf gate counter access */
STATIC int
bcmi_gh2_taf_gate_stat_counter_access(
    int unit,
    int taf_gate_id,
    uint32 arr_cnt,
    bcm_tsn_taf_gate_stat_t *stat_type_arr,
    uint64 *stat_value_arr,
    bcmi_tsn_stat_counter_action_t action)
{
    int arr_idx;

    if (NULL == stat_type_arr || NULL == stat_value_arr ||
        taf_gate_id >= _tsn_taf_gate_num(unit) ||
        taf_gate_id < 0) {
        return BCM_E_PARAM;
    }

    for (arr_idx = 0; arr_idx < arr_cnt; arr_idx++) {
        int is_write = 0;

        if (stat_type_arr[arr_idx] < 0 ||
            stat_type_arr[arr_idx] >= bcmTsnTafGateStatCount) {
            return BCM_E_PARAM;
        }

        /* update taf gate counter for both hw table and sw database */
        switch (action) {
            case bcmiTsnStatCounterActionWrite:
                is_write = 1;
                /* fall through */
            case bcmiTsnStatCounterActionReadSync:
                BCM_IF_ERROR_RETURN(
                    bcmi_gh2_taf_gate_stat_counter_update(
                        unit, taf_gate_id,
                        stat_type_arr[arr_idx], stat_type_arr[arr_idx],
                        is_write, stat_value_arr[arr_idx]));
                break;
           case bcmiTsnStatCounterActionRead:
                break;
           default:
                return BCM_E_INTERNAL;
        }

        /* read sw database if need */
        if (bcmiTsnStatCounterActionWrite != action) {
            uint64 *sw_array_ptr =
                taf_gate_counter[unit][stat_type_arr[arr_idx]];
            COMPILER_64_COPY(stat_value_arr[arr_idx],
                             sw_array_ptr[taf_gate_id]);
        }
    }

    return BCM_E_NONE;
}

/* callback for taf gate counter, just need to update the enabled gate */
STATIC int
bcmi_gh2_taf_gate_stat_counter_sync(int unit, int taf_gate_id)
{
    uint64 value_64_zero;

    COMPILER_64_ZERO(value_64_zero);

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_taf_gate_stat_counter_update(
            unit, taf_gate_id,
            0, bcmTsnTafGateStatCount -1, 0, value_64_zero));

    return BCM_E_NONE;
}

/* cleanup the resource for TAF */
STATIC int
bcmi_gh2_taf_cleanup(int unit)
{
    int i, j;
    gh2_taf_interrupt_type_t iter;
    bcm_tsn_taf_gate_stat_t stat_type;

    /* disable interupt mask */
    for (iter = 0; iter < TafInterruptCount; iter++) {
        soc_reg_field32_modify(
            unit, TAF_MEMFAILINTMASKr, REG_PORT_ANY,
            taf_interrupt_mapping[iter].mask_field, 0);
    }

    /* taf gate alloc bitmap */
    if (NULL != taf_gate_bitmap[unit]) {
        shr_res_bitmap_free(taf_gate_bitmap[unit], 1,
                            TAF_GATE_RESERVED_ID);
        shr_res_bitmap_destroy(taf_gate_bitmap[unit]);
        taf_gate_bitmap[unit] = NULL;
    }

    /* taf calendar */
    if (NULL != taf_schedule_calendar_buf[unit]) {
        soc_cm_sfree(unit, taf_schedule_calendar_buf[unit]);
        taf_schedule_calendar_buf[unit] = NULL;
    }

    for (i = 0; i < _tsn_taf_gate_num(unit); i++) {
        bcmi_tsn_taf_schedule_entry_t *entry_iter = NULL;
        bcmi_tsn_taf_schedule_entry_t *entry_next = NULL;

        entry_iter = taf_schedule_timeline[unit][i];
        while (entry_iter != NULL) {
            entry_next = entry_iter->next;
            sal_free(entry_iter);
            entry_iter = entry_next;
        }
        taf_schedule_timeline[unit][i] = NULL;
    }

    /* taf profile */
    if (NULL != taf_schedule_profile_bitmap[unit]) {
        shr_res_bitmap_destroy(taf_schedule_profile_bitmap[unit]);
        taf_schedule_profile_bitmap[unit] = NULL;
    }
    for (i = 0; i < BCMI_TSN_TAF_PROFILE_NUM_MAX; i++) {
        if (NULL != taf_schedule_profile[unit][i]) {
            sal_free(taf_schedule_profile[unit][i]);
            taf_schedule_profile[unit][i] = NULL;
        }
    }
    /* taf event handler */
    for (i = 0; i < _tsn_taf_gate_num(unit); i++) {
        for (j = 0; j < bcmTsnTafEventCount; j++) {
            if (NULL != taf_event_handler_list[unit][i][j]) {
                bcmi_tsn_taf_event_handler_t *event_handler_iter;
                bcmi_tsn_taf_event_handler_t *event_handler_next;

                event_handler_iter = taf_event_handler_list[unit][i][j];
                while (NULL != event_handler_iter) {
                    event_handler_next = event_handler_iter->next;
                    sal_free(event_handler_iter);
                    event_handler_iter = event_handler_next;
                }
                taf_event_handler_list[unit][i][j] = NULL;
            }
        }
    }
    /* taf interrupt handler */
    taf_interrupt_handling[unit] = 0;

    /* maxbyte profile */
    for (i = 0; i < _tsn_taf_gate_num(unit); i++) {
        if (NULL != taf_maxbyte_profile_bitmap[unit][i]) {
            shr_res_bitmap_free(taf_maxbyte_profile_bitmap[unit][i], 1,
                                TAF_MAXBYTE_PROFILE_RESERVED_ID);
            shr_res_bitmap_destroy(taf_maxbyte_profile_bitmap[unit][i]);
            taf_maxbyte_profile_bitmap[unit][i] = NULL;
        }
    }

    /* cosq profile */
    if (NULL != taf_cosq_profile_bitmap[unit]) {
        shr_res_bitmap_free(taf_cosq_profile_bitmap[unit], 1,
                            TAF_COSQ_PROFILE_RESERVED_ID);
        shr_res_bitmap_destroy(taf_cosq_profile_bitmap[unit]);
        taf_cosq_profile_bitmap[unit] = NULL;
    }

    /* gate stat */
    for (stat_type = 0; stat_type < bcmTsnTafGateStatCount; stat_type++) {
        if (NULL != taf_gate_counter[unit][stat_type]) {
            sal_free(taf_gate_counter[unit][stat_type]);
            taf_gate_counter[unit][stat_type] = NULL;
        }
        if (NULL != taf_gate_counter_dma_buffer[unit][stat_type]) {
            soc_cm_sfree(unit, taf_gate_counter_dma_buffer[unit][stat_type]);
            taf_gate_counter_dma_buffer[unit][stat_type] = NULL;
        }
    }

    return BCM_E_NONE;
}

/* init the resource for TAF */
STATIC int
bcmi_gh2_taf_init(int unit)
{
    int i, j, entry_count, default_gate, entry_byte, default_profile;
    gh2_taf_interrupt_type_t iter;
    bcm_tsn_taf_gate_stat_t stat_type;

    /* taf gate alloc bitmap */
    default_gate = TAF_GATE_RESERVED_ID;
    TAF_INIT_ERROR_RETURN_WITH_CLEAN(
        shr_res_bitmap_create(&taf_gate_bitmap[unit], 0,
                              _tsn_taf_gate_num(unit)));

    TAF_INIT_ERROR_RETURN_WITH_CLEAN(
        shr_res_bitmap_alloc(taf_gate_bitmap[unit],
                             SHR_RES_BITMAP_ALLOC_WITH_ID, 1, &default_gate));

    for (i = 0; i < _tsn_taf_gate_num(unit); i++) {
        taf_gate_refcount[unit][i] = 0;
    }

    /* taf primap */
    entry_count = soc_mem_index_count(unit, taf_gate_pri_map_mem);
    if (_taf_gate_pri_map_mask(unit) >= entry_count) {
        TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
    }

    /* taf profile */
    TAF_INIT_ERROR_RETURN_WITH_CLEAN(
        shr_res_bitmap_create(&taf_schedule_profile_bitmap[unit],
                              0, BCMI_TSN_TAF_PROFILE_NUM_MAX));

    /* taf calendar */
    entry_byte = WORDS2BYTES(
                    soc_mem_entry_words(unit, taf_schedule_calendar_mem[0][0]));
    taf_schedule_calendar_buf[unit] =
        soc_cm_salloc(unit, TAF_SCHEDULE_CALENDAR_ENTIRES * entry_byte,
                      "taf schedule calendar buffer");
    if (NULL == taf_schedule_calendar_buf[unit]) {
        TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
    }

    /* enable interupt mask */
    for (iter = 0; iter < TafInterruptCount; iter++) {
        soc_reg_field32_modify(
            unit, TAF_MEMFAILINTMASKr, REG_PORT_ANY,
            taf_interrupt_mapping[iter].mask_field, 1);
    }

    /* maxbyte profile */
    default_profile = TAF_MAXBYTE_PROFILE_RESERVED_ID;
    for (i = 0; i < _tsn_taf_gate_num(unit); i++) {
        uint64 default_value;
        int field_len;

        TAF_INIT_ERROR_RETURN_WITH_CLEAN(
            shr_res_bitmap_create(&taf_maxbyte_profile_bitmap[unit][i], 0,
                                  TAF_MAXBYTE_PROFILE_NUM));
        TAF_INIT_ERROR_RETURN_WITH_CLEAN(
            shr_res_bitmap_alloc(taf_maxbyte_profile_bitmap[unit][i],
                                 SHR_RES_BITMAP_ALLOC_WITH_ID, 1,
                                 &default_profile));
        for (j = 0; j < TAF_MAXBYTE_PROFILE_NUM; j++) {
            taf_maxbyte_profile_refcount[unit][i][j] = 0;
        }

        /* set default_value to 0xFFFFFFFFF */
        field_len = soc_mem_field_length(
                        unit, taf_maxbyte_profile_mem,
                        taf_maxbyte_profile_field[default_profile]);
        COMPILER_64_SET(default_value, 0, 1);
        COMPILER_64_SHL(default_value, field_len);
        COMPILER_64_SUB_32(default_value, 1);
        TAF_INIT_ERROR_RETURN_WITH_CLEAN(
            _bcmi_gh2_taf_gate_max_bytes_profile_set(
                unit, i, default_profile, default_value));
    }

    /* cosq profile */
    entry_count = soc_mem_index_count(unit, taf_cosq_profile_mem);
    if ((TAF_COSQ_INTPRI_MASK | TAF_COSQ_SELECT_MASK |
         TAF_COSQ_PROFILE_MASK) >= entry_count) {
        TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
    }

    default_profile = TAF_COSQ_PROFILE_RESERVED_ID;
    TAF_INIT_ERROR_RETURN_WITH_CLEAN(
        shr_res_bitmap_create(&taf_cosq_profile_bitmap[unit], 0,
                              TAF_COSQ_PROFILE_NUM));
    TAF_INIT_ERROR_RETURN_WITH_CLEAN(
        shr_res_bitmap_alloc(taf_cosq_profile_bitmap[unit],
                             SHR_RES_BITMAP_ALLOC_WITH_ID, 1,
                             &default_profile));
    for (i = 0; i < TAF_COSQ_PROFILE_NUM; i++) {
        taf_cosq_profile_refcount[unit][i] = 0;
    }

    /* gate stat */
    for (stat_type = 0; stat_type < bcmTsnTafGateStatCount; stat_type++) {

        /* verify memory informaion */
        if (soc_mem_index_count(unit, taf_gate_cnt_mem[stat_type])
            != _tsn_taf_gate_num(unit)) {
            TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
        }

        if (!soc_mem_field_valid(unit, taf_gate_cnt_mem[stat_type],
                                 taf_gate_cnt_field[stat_type])) {
            TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_INTERNAL);
        }

        /* allocate sw database */
        taf_gate_counter[unit][stat_type] =
                sal_alloc(_tsn_taf_gate_num(unit) * sizeof(uint64),
                          "taf gate counter");
        if (NULL == taf_gate_counter[unit][stat_type]) {
            TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
        }

        taf_gate_counter_dma_buffer[unit][stat_type] =
            soc_cm_salloc(unit, _tsn_taf_gate_num(unit) *
                          WORDS2BYTES(
                            soc_mem_entry_words(unit,
                                                taf_gate_cnt_mem[stat_type])),
                          "taf gate counter buffer");
        if (NULL == taf_gate_counter_dma_buffer[unit][stat_type]) {
             TAF_INIT_ERROR_RETURN_WITH_CLEAN(BCM_E_MEMORY);
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
STATIC int
bcmi_gh2_taf_get_scache_size(
    int unit, int *size)
{
    if (NULL == size) {
        return BCM_E_PARAM;
    }

    *size = 0;

    /* taf gate */
    *size += _tsn_taf_gate_num(unit) * sizeof(uint32);

    /* maxbyte profile */
    *size += _tsn_taf_gate_num(unit) * TAF_MAXBYTE_PROFILE_NUM * sizeof(uint32);

    /* cosq profile */
    *size += TAF_COSQ_PROFILE_NUM * sizeof(uint32);

    /* taf schedule timelime
     *    max possible =
     *      total count (32 bit) +
     *          profile_max_num * (gate id (32 bit) + count N (32 bit) +
     *                             basetime (96 bit))
     */
    *size += sizeof(uint32);
    *size += BCMI_TSN_TAF_PROFILE_NUM_MAX * (sizeof(uint32) * 5);

    /* taf schedule profile
     *   max possible =
     *      total count (32 bit) +
     *      profile_max_num * (profile id(32 bit) + taf_gate(32 bit) +
     *                         status(32 bit) + config_change_time(96 bit) +
     *                         data + schedule_index(32 bit))
     */
    *size += sizeof(uint32);
    *size += BCMI_TSN_TAF_PROFILE_NUM_MAX *
              (sizeof(uint32) * 7 + sizeof(bcm_tsn_taf_profile_t));
    return BCM_E_NONE;
}


/* sync taf info to scache */
STATIC int
bcmi_gh2_taf_sync(
    int unit, uint8 *scache_ptr)
{
    int conut_schedule, conut_profile, profile_id, schedule_id;
    uint32 gate_id;
    bcmi_tsn_taf_schedule_entry_t *schedule_entry_cache
                                  [BCMI_TSN_TAF_PROFILE_NUM_MAX];

    if (NULL == scache_ptr) {
        return BCM_E_PARAM;
    }
    sal_memset(schedule_entry_cache, 0,
               BCMI_TSN_TAF_PROFILE_NUM_MAX *
               sizeof(bcmi_tsn_taf_schedule_entry_t *));

    /* taf gate */
    WRITE_PTR_RANGE(scache_ptr, _tsn_taf_gate_num(unit) * sizeof(uint32),
                    taf_gate_refcount[unit]);

    /* maxbyte profile */
    for (gate_id = 0; gate_id < _tsn_taf_gate_num(unit); gate_id++) {
        WRITE_PTR_RANGE(scache_ptr, TAF_MAXBYTE_PROFILE_NUM * sizeof(uint32),
                        taf_maxbyte_profile_refcount[unit][gate_id]);
    }

    /* cosq profile */
    WRITE_PTR_RANGE(scache_ptr, TAF_COSQ_PROFILE_NUM * sizeof(uint32),
                    taf_cosq_profile_refcount[unit]);

    /* taf schedule timelime
     *  Format : count (32 bit) +
     *           [ gate id (32 bit) + count N (32 bit) +
     *             basetime_1(96 bit) + ... + basetime_N(96 bit) ]
     *
     *  ex. gate 1 : basetime_0, basetime_1, basetime_2
     *      gate 3 : basetime_3
     *
     *    ==> In scache:
     *       1 (32 bit)  3 (32 bit)  basetime_1(96 bit)  basetime_2(96 bit)
     *       basetime_3(96 bit)  3 (32 bit)  1 (32 bit)  basetime_4(96 bit)
     *
     *   BTW, we'll have another cache array to record all schedule entries
     */
    conut_schedule = 0;
    for (gate_id = 0; gate_id < _tsn_taf_gate_num(unit); gate_id++) {
        if (NULL != taf_schedule_timeline[unit][gate_id]) {
            bcmi_tsn_taf_schedule_entry_t *iter;

            iter = taf_schedule_timeline[unit][gate_id];
            while (iter != NULL) {
                conut_schedule++;
                iter = iter->next;
            }
        }
    }

    WRITE_PTR(scache_ptr, uint32, conut_schedule);
    schedule_id = 0;
    for (gate_id = 0; gate_id < _tsn_taf_gate_num(unit); gate_id++) {
        if (NULL != taf_schedule_timeline[unit][gate_id]) {
            uint32 count_per_gate = 0;
            bcmi_tsn_taf_schedule_entry_t *iter;

            /* write "gate id" and "count" into scache */
            iter = taf_schedule_timeline[unit][gate_id];
            while (iter != NULL) {
                count_per_gate++;
                iter = iter->next;
            }

            WRITE_PTR(scache_ptr, uint32, gate_id);
            WRITE_PTR(scache_ptr, uint32, count_per_gate);

            /* write each "basetime" into scache */
            iter = taf_schedule_timeline[unit][gate_id];
            while (iter != NULL) {
                WRITE_PTR(scache_ptr, uint32,
                          COMPILER_64_LO(iter->schedule_time.seconds));
                WRITE_PTR(scache_ptr, uint32,
                          COMPILER_64_HI(iter->schedule_time.seconds));
                WRITE_PTR(scache_ptr, uint32,
                          iter->schedule_time.nanoseconds);

                assert(schedule_id < BCMI_TSN_TAF_PROFILE_NUM_MAX);
                schedule_entry_cache[schedule_id++] = iter;

                iter = iter->next;
            }
        }
    }

    /* taf schedule profile
     *  Format : count (32 bit) +
     *           [ profile id (32 bit) + taf_gate (32 bit) + status (32 bit) +
     *             config_change_time(96 bit) +
     *             data(bcm_tsn_taf_profile_t) + schedule_entry index (32 bit) ]
     *
     *  ex. profile 0 : schedule = NULL
     *      profile 1 : schedule = basetime_2
     *      profile 2 : schedule = basetime_3
     *      profile 4 : schedule = NULL
     *      profile 6 : schedule = basetime_1
     *      profile 7 : schedule = basetime_0
     *
     *    ==> In scache:
     *       6 (32 bit)
     *       0 (32 bit)  taf_gate  status  data  -1(32 bit)
     *       1 (32 bit)  taf_gate  status  data  2(32 bit)
     *       2 (32 bit)  taf_gate  status  data  3(32 bit)
     *       4 (32 bit)  taf_gate  status  data  -1(32 bit)
     *       6 (32 bit)  taf_gate  status  data  1(32 bit)
     *       7 (32 bit)  taf_gate  status  data  0(32 bit)
     */
    conut_profile = 0;
    for (profile_id = 0; profile_id < BCMI_TSN_TAF_PROFILE_NUM_MAX;
         profile_id++) {
        if (NULL != taf_schedule_profile[unit][profile_id]) {
            conut_profile++;
        }
    }

    WRITE_PTR(scache_ptr, uint32, conut_profile);

    for (profile_id = 0; profile_id < BCMI_TSN_TAF_PROFILE_NUM_MAX;
         profile_id++) {
        bcmi_tsn_taf_profile_t *iter = taf_schedule_profile[unit][profile_id];

        if (NULL == iter) {
           continue;
        }

        WRITE_PTR(scache_ptr, uint32, profile_id);
        WRITE_PTR(scache_ptr, uint32, (uint32)(iter->taf_gate));
        WRITE_PTR(scache_ptr, uint32, (uint32)(iter->status));
        WRITE_PTR(scache_ptr, uint32,
                  COMPILER_64_LO(iter->config_change_time.seconds));
        WRITE_PTR(scache_ptr, uint32,
                  COMPILER_64_HI(iter->config_change_time.seconds));
        WRITE_PTR(scache_ptr, uint32,
                  iter->config_change_time.nanoseconds);
        WRITE_PTR_RANGE(scache_ptr, sizeof(bcm_tsn_taf_profile_t),
                        &(iter->data));

        if (NULL == iter->schedule) {
            WRITE_PTR(scache_ptr, uint32, (uint32)-1);
        } else {
            /* search the index */
            int found = -1;

            for (schedule_id = 0; schedule_id < conut_schedule; schedule_id++) {
                if (schedule_entry_cache[schedule_id] == iter->schedule) {
                    found = schedule_id;
                    break;
                }
            }
            if (-1 == found) {
                return BCM_E_INTERNAL; /* should not happen */
            }

            WRITE_PTR(scache_ptr, uint32, found);
        }
    }

    return BCM_E_NONE;
}

/* restore taf info from scache and hw */
STATIC int
bcmi_gh2_taf_reload(
    int unit, uint8 *scache_ptr)
{
    int gate_id, profile_id, entry_index, schedule_id;
    uint32 conut_schedule, conut_profile;
    bcmi_tsn_taf_schedule_entry_t *schedule_entry_cache
                                  [BCMI_TSN_TAF_PROFILE_NUM_MAX];

    if (NULL == scache_ptr) {
        return BCM_E_PARAM;
    }
    sal_memset(schedule_entry_cache, 0,
               BCMI_TSN_TAF_PROFILE_NUM_MAX *
               sizeof(bcmi_tsn_taf_schedule_entry_t *));

    /* taf gate */
    READ_PTR_RANGE(scache_ptr, _tsn_taf_gate_num(unit) * sizeof(uint32),
                   taf_gate_refcount[unit]);

    for (gate_id = 0; gate_id < _tsn_taf_gate_num(unit); gate_id++) {
        if (taf_gate_refcount[unit][gate_id] != 0) {
            BCM_IF_ERROR_RETURN(
                shr_res_bitmap_alloc(taf_gate_bitmap[unit],
                                     SHR_RES_BITMAP_ALLOC_WITH_ID,
                                     1, (int *)&gate_id));
        }
    }

    /* maxbyte profile */
    for (gate_id = 0; gate_id < _tsn_taf_gate_num(unit); gate_id++) {
        READ_PTR_RANGE(scache_ptr, TAF_MAXBYTE_PROFILE_NUM * sizeof(uint32),
                       taf_maxbyte_profile_refcount[unit][gate_id]);

        for (profile_id = 0; profile_id < TAF_MAXBYTE_PROFILE_NUM;
             profile_id++) {
            if (taf_maxbyte_profile_refcount[unit][gate_id][profile_id] != 0) {
                BCM_IF_ERROR_RETURN(
                    shr_res_bitmap_alloc(
                        taf_maxbyte_profile_bitmap[unit][gate_id],
                         SHR_RES_BITMAP_ALLOC_WITH_ID, 1, (int *)&profile_id));
            }
        }
    }

    /* cosq profile */
    READ_PTR_RANGE(scache_ptr, TAF_COSQ_PROFILE_NUM * sizeof(uint32),
                   taf_cosq_profile_refcount[unit]);

    for (profile_id = 0; profile_id < TAF_COSQ_PROFILE_NUM; profile_id++) {
        if (taf_cosq_profile_refcount[unit][profile_id] != 0) {
            BCM_IF_ERROR_RETURN(
                shr_res_bitmap_alloc(taf_cosq_profile_bitmap[unit],
                                     SHR_RES_BITMAP_ALLOC_WITH_ID,
                                     1, (int *)&profile_id));
        }
    }

    /* taf schedule timelime
     *  Format : count (32 bit) +
     *           [ gate id (32 bit) + count N (32 bit) +
     *             basetime_1(96 bit) + ... + basetime_N(96 bit) ]
     */
    schedule_id = 0;
    READ_PTR(scache_ptr, uint32, conut_schedule);
    while (schedule_id < conut_schedule) {
        int i;
        uint32 gate_id, count_per_gate;
        uint32 sec_lo, sec_hi;
        bcmi_tsn_taf_schedule_entry_t *entry_tail = NULL;

        READ_PTR(scache_ptr, uint32, gate_id);
        READ_PTR(scache_ptr, uint32, count_per_gate);
        entry_tail = taf_schedule_timeline[unit][gate_id];

        for (i = 0; i < count_per_gate; i++) {
            bcmi_tsn_taf_schedule_entry_t *entry_new = NULL;

            if (schedule_id >= conut_schedule) {
                return BCM_E_INTERNAL; /* should not happen */
            }

            entry_new = sal_alloc(sizeof(bcmi_tsn_taf_schedule_entry_t),
                        "TAF schedule entry");
            if (NULL == entry_new) {
                return BCM_E_MEMORY;
            }

            READ_PTR(scache_ptr, uint32, sec_lo);
            READ_PTR(scache_ptr, uint32, sec_hi);
            READ_PTR(scache_ptr, uint32, entry_new->schedule_time.nanoseconds);
            COMPILER_64_SET(entry_new->schedule_time.seconds, sec_hi, sec_lo);

            if (NULL == entry_tail) {
                /* this the first entry */
                entry_new->prev = NULL;
                entry_new->next = NULL;
                taf_schedule_timeline[unit][gate_id] = entry_new;
            } else {
                /* add new entry at the tail */
                entry_tail->next = entry_new;
                entry_new->prev = entry_tail;
                entry_new->next = NULL;
            }
            entry_tail = entry_new;

            /* cache this new entry, then ready to process the next one */
            schedule_entry_cache[schedule_id] = entry_new;
            schedule_id++;
        }
    }

    /* taf schedule profile
     *  Format : count (32 bit) +
     *           [ profile id (32 bit) + taf_gate (32 bit) + status (32 bit) +
     *             config_change_time(96 bit) +
     *             data(bcm_tsn_taf_profile_t) + schedule_entry index (32 bit) ]
     */
    entry_index = 0;
    READ_PTR(scache_ptr, uint32, conut_profile);
    for (entry_index = 0; entry_index < conut_profile; entry_index++) {
        uint32 profile_id;
        bcmi_tsn_taf_profile_t *new_profile = NULL;
        uint32 sec_lo, sec_hi;

        READ_PTR(scache_ptr, uint32, profile_id);

        new_profile = sal_alloc(sizeof(bcmi_tsn_taf_profile_t), "TAF profile");
        if (NULL == new_profile) {
            return BCM_E_MEMORY;
        }
        taf_schedule_profile[unit][profile_id] = new_profile;

        BCM_IF_ERROR_RETURN(
            shr_res_bitmap_alloc(
                taf_schedule_profile_bitmap[unit],
                SHR_RES_BITMAP_ALLOC_WITH_ID, 1, (int *)&profile_id));

        /* recover taf_gate, status, config_change_time, data */
        READ_PTR(scache_ptr, uint32, new_profile->taf_gate);
        READ_PTR(scache_ptr, uint32, new_profile->status);
        READ_PTR(scache_ptr, uint32, sec_lo);
        READ_PTR(scache_ptr, uint32, sec_hi);
        READ_PTR(scache_ptr, uint32,
                 new_profile->config_change_time.nanoseconds);
        COMPILER_64_SET(new_profile->config_change_time.seconds,
                        sec_hi, sec_lo);
        READ_PTR_RANGE(scache_ptr, sizeof(bcm_tsn_taf_profile_t),
                       &(new_profile->data));

        /* recover schedule entry */
        READ_PTR(scache_ptr, uint32, schedule_id);
        if (schedule_id == (uint32)-1) {
            new_profile->schedule = NULL;
        } else {
            if (schedule_id >= conut_schedule) {
                return BCM_E_INTERNAL; /* should not happen */
            }
            if (NULL == schedule_entry_cache[schedule_id]) {
                return BCM_E_INTERNAL; /* should not happen */
            }
            new_profile->schedule = schedule_entry_cache[schedule_id];
            new_profile->schedule->taf_profile = new_profile;
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Per-chip TAF device info */
STATIC const bcmi_tsn_taf_dev_info_t gh2_taf_devinfo = {
    /* function set */
    bcmi_gh2_taf_init,
    bcmi_gh2_taf_cleanup,
    bcmi_gh2_taf_gate_create,
    bcmi_gh2_taf_gate_traverse,
    bcmi_gh2_taf_gate_destroy,
    bcmi_gh2_taf_gate_check,
    bcmi_gh2_taf_gate_status_get,
    bcmi_gh2_taf_global_control_get,
    bcmi_gh2_taf_global_control_set,
    bcmi_gh2_taf_port_control_get,
    bcmi_gh2_taf_port_control_set,
    bcmi_gh2_taf_gate_control_get,
    bcmi_gh2_taf_gate_control_set,
    bcmi_gh2_taf_gate_pri_map_set,
    bcmi_gh2_taf_gate_pri_map_get,
    bcmi_gh2_taf_gate_pri_map_delete,
    bcmi_gh2_taf_calendar_param_check,
    bcmi_gh2_taf_calendar_resource_alloc,
    bcmi_gh2_taf_calendar_resource_free,
    bcmi_gh2_taf_calendar_kickoff,
    bcmi_gh2_taf_calendar_profile_create,
    bcmi_gh2_taf_calendar_profile_get,
    bcmi_gh2_taf_calendar_profile_destroy,
    bcmi_gh2_taf_calendar_profile_traverse,
    bcmi_gh2_taf_schedule_insert,
    bcmi_gh2_taf_schedule_remove,
    bcmi_gh2_taf_schedule_pick,
    bcmi_gh2_taf_schedule_exist,
    bcmi_gh2_taf_schedule_get,
    bcmi_gh2_taf_event_register,
    bcmi_gh2_taf_event_unregister,
    bcmi_gh2_taf_event_notify,
    bcmi_gh2_taf_interrupt_handle_start,
    bcmi_gh2_taf_interrupt_handle_tod,
    bcmi_gh2_taf_interrupt_handle_switch,
    bcmi_gh2_taf_interrupt_handle_error,
    bcmi_gh2_taf_interrupt_handle_user_cb,
    bcmi_gh2_taf_interrupt_handle_finish,
    bcmi_gh2_taf_gate_max_bytes_profile_create,
    bcmi_gh2_taf_gate_max_bytes_profile_set,
    bcmi_gh2_taf_gate_max_bytes_profile_get,
    bcmi_gh2_taf_gate_max_bytes_profile_destroy,
    bcmi_gh2_taf_gate_max_bytes_profile_traverse,
    bcmi_gh2_taf_cosq_mapping_profile_create,
    bcmi_gh2_taf_cosq_mapping_profile_set,
    bcmi_gh2_taf_cosq_mapping_profile_get,
    bcmi_gh2_taf_cosq_mapping_profile_destroy,
    bcmi_gh2_taf_cosq_mapping_profile_traverse,
    bcmi_gh2_taf_gate_stat_counter_sync,
    bcmi_gh2_taf_gate_stat_counter_access,
#if defined(BCM_WARM_BOOT_SUPPORT)
    bcmi_gh2_taf_get_scache_size,
    bcmi_gh2_taf_sync,
    bcmi_gh2_taf_reload,
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* data set */
    bcmi_gh2_taf_gate_map_profile_num, /* exclude invalid index 0 */
    TAF_GATE_PRI_MAP_INTPRI_NUM
};

/*
 * Function:
 *    bcmi_gh2_taf_info_init
 * Description:
 *    Setup the chip specific info for TAF.
 * Parameters:
 *    unit - (IN) Unit number
 *    devinfo - (OUT) device info structure.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_gh2_taf_info_init(
    int unit,
    const bcmi_tsn_taf_dev_info_t **devinfo)
{
    int rv = BCM_E_NONE;

    if (devinfo == NULL) {
        return BCM_E_PARAM;
    }

    /* Return the chip info structure for TSN stat */
    *devinfo = &gh2_taf_devinfo;

    return rv;
}
#endif /* BCM_TSN_SUPPORT */
