/*! \file bcmcth_meter_sysm.h
 *
 * Meter component interface to system manager.
 * This file contains meter custom handler component interface
 * implementation to System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_SYSM_H
#define BCMCTH_METER_SYSM_H

/*! Storm control meter backup state sub component id. */
#define METER_SC_BKP_SUB_COMP_ID             1

/*! IFP meter backup state sub component id. */
#define METER_FP_ING_BKP_SUB_COMP_ID         2
/*! IFP meter sub component id. */
#define METER_FP_ING_HW_IDX_BKP_SUB_COMP_ID  3

/*! EFP meter backup state sub component id. */
#define METER_FP_EGR_BKP_SUB_COMP_ID         4
/*! EFP meter sub component id. */
#define METER_FP_EGR_HW_IDX_BKP_SUB_COMP_ID  5

/*! IFP meter action sub component id. */
#define METER_FP_ING_PDD_OFFSET_SUB_COMP_ID  6
/*! EFP meter action sub component id. */
#define METER_FP_EGR_PDD_OFFSET_SUB_COMP_ID  7

/*! IFP meter action state sub component id. */
#define METER_FP_ING_PDD_STATE_SUB_COMP_ID   8
/*! EFP meter action state sub component id. */
#define METER_FP_EGR_PDD_STATE_SUB_COMP_ID   9

/*!
 * \brief Meter component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_meter_start(void);

#endif /* BCMCTH_METER_SYSM_H */
