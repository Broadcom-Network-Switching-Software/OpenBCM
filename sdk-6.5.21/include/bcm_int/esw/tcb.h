/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined (INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)

#include <soc/types.h>
#include <bcm/cosq.h>
#include <soc/defs.h>

#define _BCM_TCB_MAX_NUM    16
#define _BCM_TCB_PROFILE_MAX_NUM  16
#define _BCM_TCB_RD_MAX_NUM 1024

typedef int (*_bcm_cosq_tcb_hw_intr_cb)(int unit);
typedef int (*_bcm_cosq_tcb_hw_intr_enable)(int unit, int enable);

typedef struct _bcm_cosq_tcb_info_s {
    SHR_BITDCL    *profile_bmp;
    int profile_ref_count[_BCM_TCB_PROFILE_MAX_NUM];
}_bcm_cosq_tcb_info_t, *_bcm_cosq_tcb_info_p;


typedef struct _bcm_cosq_tcb_unit_ctrl_s {
    int                             tcb_num;
    int                             cell_bytes;
    int                             threshold_max;
    uint16                          tcb_enabled_bmp;
    uint16                          tcb_config_init_bmp;
    _bcm_cosq_tcb_info_p            tcb_info[_BCM_TCB_MAX_NUM];
    bcm_cosq_tcb_callback_t         tcb_evt_cb;
    void                            *tcb_evt_user_data;

    sal_mutex_t                     tcb_reslock;
} _bcm_cosq_tcb_unit_ctrl_t;

typedef struct _bcm_cosq_tcb_pipe_port_tab_s {
    SHR_BITDCL    ipipe_bmp;
    int ipipe_member[2];
    SHR_BITDCL    epipe_bmp;
    int epipe_member[2];
    char *ipipe_range;
    char *epipe_range;
    char *iport_range;
    char *eport_range;
    char *ucq_range;
}_bcm_cosq_tcb_pipe_port_tab_t;

extern _bcm_cosq_tcb_unit_ctrl_t *_bcm_cosq_tcb_unit_control[BCM_MAX_NUM_UNITS];

#define TCB_UNIT_CONTROL(unit)        (_bcm_cosq_tcb_unit_control[unit])

#define TCB_INIT(unit)    \
      if (TCB_UNIT_CONTROL(unit) == NULL) {\
          return BCM_E_INIT; }

#define TCB_LOCK(unit)                                              \
        do {                                                        \
            if (TCB_UNIT_CONTROL(unit)->tcb_reslock) {              \
                sal_mutex_take(TCB_UNIT_CONTROL(unit)->tcb_reslock, \
                               sal_mutex_FOREVER);                  \
            }                                                       \
        } while(0)
#define TCB_UNLOCK(unit)                                            \
        do {                                                        \
            if (TCB_UNIT_CONTROL(unit)->tcb_reslock) {              \
                sal_mutex_give(TCB_UNIT_CONTROL(unit)->tcb_reslock);\
            }                                                       \
        } while(0)

#define TCB_ID_CHECK(tcb_id)  \
    if (tcb_id < 0 || tcb_id >= TCB_UNIT_CONTROL(unit)->tcb_num) { \
        return BCM_E_PARAM;  }

#define _TCB_PIPE0    0x1
#define _TCB_PIPE1    0x2
#define _TCB_PIPE2    0x4
#define _TCB_PIPE3    0x8

#define _TCB_SGPP_TYPE_MASK    0x10000
#define _TCB_SGPP_MASK         0xffff

#define _SHR_TCB_TRACK_IADMN_DROP_SHIFT    0
#define _SHR_TCB_TRACK_EADMN_DROP_SHIFT    1
#define _SHR_TCB_TRACK_WRED_DROP_SHIFT     2
#define _SHR_TCB_TRACK_CFAP_DROP_SHIFT     3

#define _TCB_EVENT_DROP_REASON_IADMN_MASK   0x10
#define _TCB_EVENT_DROP_REASON_EADMN_MASK   0x08
#define _TCB_EVENT_DROP_REASON_WRED_MASK    0x04
#define _TCB_EVENT_DROP_REASON_CFAP_MASK    0x02
#define _TCB_EVENT_DROP_REASON_PURGE_MASK   0x01

#define _TCB_IDLE_PHASE           0x0
#define _TCB_PRE_TRIGGER_PHASE    0x1
#define _TCB_POST_TRIGGER_PHASE   0x2
#define _TCB_FROZEN_PHASE         0x3


#define _TCB_SCOPE_UCQ        0x0
#define _TCB_SCOPE_Q_GROUP    0x1
#define _TCB_SCOPE_ING_PORT   0x2
#define _TCB_SCOPE_EGR_PORT   0x3
#define _TCB_SCOPE_ING_PIPE   0x4
#define _TCB_SCOPE_EGR_PIPE   0x5

#define _TCB_BUF_STATUS_WRAPPED   0x0
#define _TCB_BUF_STATUS_PTR       0x1
#define _TCB_BUF_STATUS_EVT_NUM   0x2
#define _TCB_BUF_STATUS_0_USED    0x3

#define _TCB_TRIGGER_MAX         0xf
#define _TCB_CAP_NUM_MIN         2
#define _TCB_CAP_NUM_MAX         1024
#define _TCB_CAP_TIM_MAX         65535
#define _TCB_PRE_SAMP_RATE_MAX   0x10
#define _TCB_POST_SAMP_RATE_MAX  0x10

#define _TCB_TRIGGER_DEFAULT         0xf
#define _TCB_CAP_NUM_DEFAULT         304
#define _TCB_CAP_TIM_DEFAULT         _TCB_CAP_TIM_MAX
#define _TCB_CAP_TIM_FACTOR          10
#define _TCB_CAP_TIM_MAX_DEVIATION  (_TCB_CAP_TIM_FACTOR - 1)
#define _TCB_PRE_SAMP_RATE_DEFAULT   7
#define _TCB_POST_SAMP_RATE_DEFAULT  15

extern int _bcm_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id,
                                          int queue, bcm_gport_t *uc_gport);

#endif /* INCLUDE_TCB && BCM_TCB_SUPPORT */
