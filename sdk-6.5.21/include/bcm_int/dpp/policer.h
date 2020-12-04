/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        policer.h
 * Purpose:     policer internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_POLICER_H_
#define   _BCM_INT_DPP_POLICER_H_

#include <bcm/policer.h>

/* 
 * format meter id 
 * 0-12/14: meter/policer id 
 * 13/15: group (for meter) 
 * 14/16: core (for meter) 
 * 29: type 0:meter 1:ethernet-policer , 
 *     needed as same APIs and ID used for both meter types
 * Notes: 
 *    - meter type: has to be zero, as pointer by FP
 *    - meter core: currently up to two cores are supported by the macros
 */


/* type: 0:meter 1:ethernet-policer */
#define _DPP_POLICER_METER_TYPE_SHIFT(__unit)  (29)
/* group: 0 or 1 */
#define _DPP_POLICER_METER_GROUP_SHIFT(__unit) (SOC_IS_QUX(__unit) ? (13) : (15))
/* core: 0 or 1 */
#define _DPP_POLICER_METER_CORE_SHIFT(__unit)  (_DPP_POLICER_METER_GROUP_SHIFT(__unit) + 1)

/* 0 -12. for meter id */
#define _DPP_POLICER_METER_ID_MASK(__unit)    (_DPP_POLICER_METER_GROUP_MASK(__unit) - 1)
/* 13/15 for meter group */
#define _DPP_POLICER_METER_GROUP_MASK(__unit) (1 << _DPP_POLICER_METER_GROUP_SHIFT(__unit))
/* 14/16 for meter core */
#define _DPP_POLICER_METER_CORE_MASK(__unit)  (1 << _DPP_POLICER_METER_CORE_SHIFT(__unit))
/* 29 for meter type*/
#define _DPP_POLICER_METER_TYPE_MASK(__unit)  (1 << _DPP_POLICER_METER_TYPE_SHIFT(__unit))

#define _DPP_POLICER_ID_TO_CORE(__unit , __meter_id)    (((__meter_id) & _DPP_POLICER_METER_CORE_MASK(__unit)) >> _DPP_POLICER_METER_CORE_SHIFT(__unit))
#define _DPP_POLICER_ID_TO_GROUP(__unit , __meter_id)   (((__meter_id) & _DPP_POLICER_METER_GROUP_MASK(__unit)) >> _DPP_POLICER_METER_GROUP_SHIFT(__unit))
#define _DPP_POLICER_ID_TO_METER(__unit, __meter_id)    ((__meter_id) & _DPP_POLICER_METER_ID_MASK(__unit))
#define _DPP_POLICER_ID_FROM_METER_GROUP_CORE(__unit, __meter, __group, __core)   (((__core) << _DPP_POLICER_METER_CORE_SHIFT(__unit))|((__group) << _DPP_POLICER_METER_GROUP_SHIFT(__unit))|__meter)

#define _DPP_POLICER_NOF_POLICERS (8)
#define _DPP_POLICER_MAX_POLICER_ID (_DPP_POLICER_NOF_POLICERS - 1)
#define _DPP_POLICER_MIN_POLICER_ID  (1)

/* iterate over all metering cores */
#define _DPP_POLICER_METER_CORES_ITER(index) \
    for(index = 0; index < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; index++)

#define _DPP_IS_ETH_POLICER_ID(__unit, __meter)  			((__meter) & _DPP_POLICER_METER_TYPE_MASK(__unit))
#define _DPP_ETH_POLICER_ID_GET(__unit, __meter) 			((__meter) & _DPP_POLICER_METER_ID_MASK(__unit))
#define _DPP_ETH_POLICER_ID_SET(__unit, __meter, __core) 	((__meter) | _DPP_POLICER_METER_TYPE_MASK(__unit) | ((__core) << _DPP_POLICER_METER_CORE_SHIFT(__unit)))
#define _DPP_ETH_POLICER_CORE_GET(__unit , __meter_id)    	(((__meter_id) & _DPP_POLICER_METER_CORE_MASK(__unit)) >> _DPP_POLICER_METER_CORE_SHIFT(__unit))

#define _DPP_ETH_METER_ID_GET(__unit, __meter) (__meter)

#define _DPP_POLICER_METER_MAX_CORE_ID(unit)                 (SOC_DPP_CONFIG(unit)->meter.nof_meter_cores - 1)
#define _DPP_POLICER_METER_IS_SINGLE_CORE(unit)              (SOC_DPP_CONFIG(unit)->meter.nof_meter_cores == 1)
#define _DPP_POLICER_NOF_COLORS                               4
#define _DPP_POLICER_NOF_METER_ACTIONS                        4

/* 
* sw state for meter/policer
*/  


/* for each bcm-policer used dpp_policers*/
typedef struct _dpp_policer_info_s {
    bcm_policer_group_mode_t mode;
} _dpp_policer_group_info_t;

/* dpp policers status */

typedef struct _dpp_policer_state_s {
    _dpp_policer_group_info_t policer_group[_DPP_POLICER_NOF_POLICERS];
} _dpp_policer_state_t;


#define _DPP_POLICER_MAX_KBITS_SEC_UNLIMITED (0xffffffff)


/* map port to policer group */
int bcm_petra_policer_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t policer_id);
    
/* return policer group assigned to port */
int bcm_petra_policer_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_policer_t *policer_id);
    
extern int
bcm_petra_policer_detach(int unit);

#endif /* _BCM_INT_DPP_POLICER_H_ */
