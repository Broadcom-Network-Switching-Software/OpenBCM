/*! \file bcmcth_oam_bfd_uc_msg_pack.h
 *
 * Pack and unpack routines for the BFD embedded application messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_BFD_UC_MSG_PACK_H
#define BCMCTH_OAM_BFD_UC_MSG_PACK_H

#include <bcmbd/mcs_shared/mcs_bfd_common.h>

/*!
 * \brief Pack the BFD init message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_init_pack(uint8_t *buf, mcs_bfd_msg_ctrl_init_t *msg);

/*!
 * \brief Pack the BFD endpoint set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_set_pack(uint8_t *buf,
                                    mcs_bfd_msg_ctrl_ep_set_t *msg);

/*!
 * \brief Unpack the BFD endpoint status get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after unpacking \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_status_get_unpack(uint8_t *buf,
                                             mcs_bfd_msg_ctrl_ep_status_get_t *msg);

/*!
 * \brief Pack the BFD endpoint statistics set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_stats_set_pack(uint8_t *buf,
                                          mcs_bfd_msg_ctrl_ep_stats_set_t *msg);
/*!
 * \brief Unpack the BFD endpoint statistics get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after unpacking \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_ep_stats_get_unpack(uint8_t *buf,
                                            mcs_bfd_msg_ctrl_ep_stats_get_t *msg);

/*!
 * \brief Pack the BFD global statistics set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_stats_set_pack(uint8_t *buf,
                                              mcs_bfd_msg_ctrl_global_stats_set_t *msg);

/*!
 * \brief Unpack the BFD global statistics get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after unpacking \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_stats_get_unpack(uint8_t *buf,
                                                mcs_bfd_msg_ctrl_global_stats_get_t *msg);

/*!
 * \brief Pack the BFD SHA1 authentication set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_auth_sha1_set_pack(uint8_t *buf,
                                           mcs_bfd_msg_ctrl_auth_sha1_set_t *msg);

/*!
 * \brief Pack the BFD simple password authentication set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_auth_sp_set_pack(uint8_t *buf,
                                         mcs_bfd_msg_ctrl_auth_sp_set_t *msg);

/*!
 * \brief Unpack the BFD trace log enable reply message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after unpacking \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_trace_log_enable_reply_unpack(uint8_t *buf,
                                                      mcs_bfd_msg_ctrl_trace_log_enable_reply_t *msg);

/*!
 * \brief Pack the BFD parameter set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_global_param_set_pack(uint8_t *buf,
                                              mcs_bfd_msg_ctrl_global_param_set_t *msg);

/*!
 * \brief Pack the BFD Rx PMD flex format set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_rxpmd_flex_format_set_pack(uint8_t *buf,
                                                   mcs_bfd_msg_ctrl_rxpmd_flex_format_t *msg);

/*!
 * \brief Pack the BFD match Id set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_oam_bfd_msg_ctrl_match_id_set_pack(uint8_t *buf,
                                          mcs_bfd_msg_ctrl_match_id_set_t *msg);
#endif /* BCMCTH_BFD_UC_MSG_PACK_H */
