/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Internal S-Channel Message: data structure.
 */

#ifndef _SOC_SCHAN_INTERNAL_H
#define _SOC_SCHAN_INTERNAL_H

#include <soc/schanmsg.h>
#include <soc/types.h>
#include <soc/drv.h>


/* This is fields from SOC_CONTROL which will be passed to level schan
 *  module to the lower level so that structure dependency can be removed
 */
typedef struct soc_cmic_schan_control_s {
    int *schanTimeout;          /* Address SOC_CONTROL(unit)->schanTimeout */
    int *schanIntrEnb;          /* SOC_CONTROL(unit)->schanIntrEnb */
                                /* Address of SOC_CONTROL(unit)->schan_result */
    uint32      *schan_result[SOC_CMCS_NUM_MAX+1];
                                /* SOC_CONTROL(unit)->schanIntr */
    sal_sem_t   *schanIntr[SOC_CMCS_NUM_MAX+1];
} soc_cmic_schan_control_t;

typedef struct soc_cmic_schan_drv_s {
    int (*soc_schan_op)(int unit, schan_msg_t *msg,
                       int dwc_write, int dwc_read, uint32 flags);
    int (*soc_schan_deinit)(int unit);
    void (*soc_schan_reset)(int unit, int cmc, int ch);
} soc_cmic_schan_drv_t;

extern soc_cmic_schan_control_t  soc_schan_control[SOC_MAX_NUM_DEVICES];

#define SOC_SCHAN_CONTROL(unit)               (soc_schan_control[unit])

#define CMIC_CMCx_SCHAN_CHSELECT_TIMEOUT    (1000)

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT)
#define SCHAN_CH_MASK 0x1111
#endif



/*******************************************
* @function soc_cmice_schan_init
* purpose API to Initialize nocmic SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments used for cmic which are neither cmicm or cmicx
 */
extern int soc_cmice_schan_init(int unit, soc_cmic_schan_drv_t *drv);

/*******************************************
* @function soc_schan_cmicm_init
* purpose API to Initialize cmicm SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_cmicm_schan_init(int unit, soc_cmic_schan_drv_t *drv);

/*******************************************
* @function cmicm_schan_ch_get
* purpose get idle schan on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-2 or 3 for common
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int cmicm_schan_ch_get(int unit, int *cmc);

/*******************************************
* @function cmicm_schan_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number 0-2 or 3 for common
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int cmicm_schan_ch_try_get(int unit, int cmc);

/*******************************************
* @function cmicm_schan_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number 0-2 or 3 for common
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int cmicm_schan_ch_put(int unit, int cmc);

/*******************************************
* @function cmicm_schan_ch_init
* purpose API to Initialize schan channel seletion mechanism
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicm_schan_ch_init(int unit, int timeout);

/*******************************************
* @function soc_cmicx_schan_init
* purpose API to Initialize cmicx SCHAN
*
* @param unit [in] unit
* @param drv [out] soc_cmic_schan_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_cmicx_schan_init(int unit, soc_cmic_schan_drv_t *drv);

/*******************************************
* @function _soc_schan_op_sanity_check
* purpose API to Initialize perform sanity checks
*
* @param unit [in] unit
* @param msg [in] pointer to Type <schan_msg_t>
* @param dwc_write [in] write
* @param dwc_read [in] read
* @param dwc_read [out] pointer to status
*
* @returns TRUE if the caller should abort the call and return *rv up the stack.
* @returns FALSE if the caller should continue as normal.
*
* @comments First, check that writing is allowed:
*    If (device is ARAD) and (opcode is not a READ command) and (override flag is off)
*    Then: writing is not allowed.
*
* Then, assert sanity checks.
* Finally, log the message that schan access is beginning
*
* @end
 */
extern int
soc_schan_op_sanity_check(int unit, schan_msg_t *msg, int dwc_write,
                           int dwc_read, int *rv);

/*******************************************
* @function soc_schan_timeout_check
* @Purpose enable special processing for timeout
*
* @param unit [in] unit
* @param dwc_read [out] pointer to status
* @param msg [in] pointer to Type <schan_msg_t>
* @param cmc [in] cmc
* @param ch [in] channel
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_schan_timeout_check(int unit, int *rv, schan_msg_t *msg, int cmc, int ch, uint32 flags);

#endif  /* !_SOC_SCHAN_INTERNAL_H */
