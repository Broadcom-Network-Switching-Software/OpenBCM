/*! \file bcma_mcs.h
 *
 * MCS manager
 *
 * Interface for managing Microcontroller Subsystem.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_MCS_H
#define BCMA_MCS_H

/*! Possible results for MCS commands */
typedef enum bcma_mcs_result_e {
    BCMA_MCS_OK          =  0,
    BCMA_MCS_FAIL        = -1,
    BCMA_MCS_FILE_NOT_FOUND   = -2,
    BCMA_MCS_NO_RESOURCES = -3
} bcma_mcs_result_t;

/*!
 * \brief 'mcs load' command implementation.
 *
 * Please refer to "mcs" Debug shell command help for detailed description.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance. Ignored if the flag
 *                BCMA_MCS_LOAD_FLAG_AUTO_CORE is used.
 * \param [in] fname File name to be loaded. 'NONE' to skip loading.
 * \param [in] req Resources requested for the uKernel. Ignored if the flag
 *                 BCMA_MCS_LOAD_FLAG_AUTO_CONFIG is used.
 * \param [in] flags Flags for MCS load operation (\ref BCMA_MCS_LOAD_FLAG_xxx).
 *
 * \return BCMA_MCS_xxx return values.
 */
extern int bcma_mcs_load(int unit, int uc, const char *fname,
                         bcmbd_mcs_res_req_t *req, uint32_t flags);

/*!
 * \name MCS load flags.
 * \anchor BCMA_MCS_LOAD_FLAG_xxx
 */
/*! \{ */
/*! Start MCS processor after loading the firmware. */
#define BCMA_MCS_LOAD_FLAG_STARTUC     0x00000001
/*! Start messaging protocol after starting the MCS processor. */
#define BCMA_MCS_LOAD_FLAG_STARTMSG    0x00000002
/*! Automatically get configuration from app type. */
#define BCMA_MCS_LOAD_FLAG_AUTO_CONFIG 0x00000004
/*! Automatically use next available core. */
#define BCMA_MCS_LOAD_FLAG_AUTO_CORE   0x00000008
/*! \} */

/*!
 * \brief 'mcs status' command implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] show_cfg Show configured Parameters.
 * \param [in] show_dbg Show more FW debug info.
 *
 * \return BCMA_MCS_xxx return values.
 */
extern int bcma_mcs_status(int unit, int uc, int show_cfg, int show_dbg);

/*!
 * \brief 'mcs message' command implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] action Messaging action to be performed. (\ref BCMA_MCS_MSG_ACT_xxx).
 *
 * \return BCMA_MCS_xxx return values.
 */
extern int bcma_mcs_message(int unit, int uc, int action);

/*!
 * \name MCS Messaging actions.
 * \anchor BCMA_MCS_MSG_ACT_xxx
 */
/*! \{ */
/*! Initialize Messaging Infrastructure */
#define BCMA_MCS_MSG_ACT_INIT     1
/*! Start messaging protocol per uC. */
#define BCMA_MCS_MSG_ACT_START    2
/*! Stop messaging protocol per uC. */
#define BCMA_MCS_MSG_ACT_STOP     3
/*! \} */

#endif /* BCMA_MCS_H */
