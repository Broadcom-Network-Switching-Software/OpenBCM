/*! \file bcmcth_mon_int_pack.h
 *
 * Pack and unpack routines for the Inband Telemetry
 * embedded application messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_INT_PACK_H
#define BCMCTH_MON_INT_PACK_H

#include <bcmbd/mcs_shared/mcs_ifa_common.h>

/*!
 * \brief Pack the IFA init message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_init_pack(uint8_t *buf,
                                      mcs_ifa_msg_ctrl_init_t *msg);

/*!
 * \brief Pack the IFA RX PMD flex format message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_rxpmd_flex_format_set_pack(uint8_t *buf,
                                                       mcs_ifa_msg_ctrl_rxpmd_flex_format_t *msg);

/*!
 * \brief Pack the IFA fifo format set message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_fifo_format_set_pack(uint8_t *buf,
                                                 mcs_ifa_msg_ctrl_fifo_format_set_t *msg);

/*!
 * \brief Pack the IFA MD format set message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_md_format_set_pack(uint8_t *buf,
                                               mcs_ifa_msg_ctrl_md_format_set_t *msg);

/*!
 * \brief Pack the IFA config update message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_config_update_pack(uint8_t *buf,
                                               mcs_ifa_msg_ctrl_config_update_t *msg);

/*!
 * \brief Pack the IFA collector create message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_collector_create_pack(uint8_t *buf,
                                                  mcs_ifa_msg_ctrl_collector_create_t *msg);

/*!
 * \brief Pack the IFA IPFIX template set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_ipfix_template_set_pack(uint8_t *buf,
                                                    mcs_ifa_msg_ctrl_ipfix_template_set_t *msg);
/*!
 * \brief Pack the IFA stats set message
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_stats_set_pack(uint8_t *buf,
                                           mcs_ifa_msg_ctrl_stats_set_t *msg);

/*!
 * \brief Unpack the IFA stats get message
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg
 */
extern uint8_t *
bcmcth_mon_int_ifa_msg_ctrl_stats_get_unpack(uint8_t *buf,
                                             mcs_ifa_msg_ctrl_stats_get_t *msg);

#endif /* BCMCTH_MON_INT_PACK_H */
