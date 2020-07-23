/*! \file bcmcth_mon_flowtracker_pack.h
 *
 * Flowtracker messaging function prototypes
 * and structure definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_PACK_H
#define BCMCTH_MON_FLOWTRACKER_PACK_H

#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>

/*! Size of the IPFIX header */
#define BCMCTH_INT_FT_IPFIX_MSG_HDR_LENGTH 16

/*! Size of the template set header */
#define BCMCTH_INT_FT_SET_HDR_LENGTH 4

/*! Length of the L2 CRC field */
#define BCMCTH_INT_FT_L2_CRC_LENGTH 4

/*!
 * \brief Pack the flowtracker init message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_init_pack(uint32_t uc_msg_version,
                                 uint8_t *buf,
                                 mcs_ft_msg_ctrl_init_t *msg);

/*!
 * \brief Unpack the flowtracker init message.
 *
 * \param [out] buf Buffer from which the data needs to be unpacked.
 * \param [in]  msg Unpacked message.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_init_unpack(uint8_t *buf,
                                   mcs_ft_msg_ctrl_init_t *msg);

/*!
 * \brief Pack the EM Key format message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_em_key_format_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_em_key_format_t *msg);

/*!
 * \brief Unpack the  EM Key format message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_em_key_format_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_em_key_format_t *msg);

/*!
 * \brief Pack the EM group create message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_em_group_create_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_em_group_create_t *msg);

/*!
 * \brief Pack the flex EM group create message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_flex_em_group_create_pack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_flex_em_group_create_t *msg);

/*!
 * \brief Unpack the EM group create message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_em_group_create_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_em_group_create_t *msg);

/*!
 * \brief Pack the flow group create message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_create_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_group_create_t *msg);

/*!
 * \brief Unpack the flow group create message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_create_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_create_t *msg);

/*!
 * \brief Pack the flow group get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_get_pack(uint8_t *buf,
                                      mcs_ft_msg_ctrl_group_get_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_get_unpack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_group_get_t *msg);

/*!
 * \brief Pack the Flow group update message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_update_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_group_update_t *msg);
/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_update_unpack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_group_update_t *msg);

/*!
 * \brief Pack the template create message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_create_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_template_create_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_create_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_template_create_t *msg);

/*!
 * \brief Pack the template get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_get_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_template_get_t *msg);

/*!
 * \brief Unpack the template get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_template_get_t *msg);

/*!
 * \brief Pack the collector create message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_create_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_collector_create_t *msg);

/*!
 * \brief Unpack the collector create  message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_create_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_collector_create_t *msg);

/*!
 * \brief Pack the collector get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_get_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_collector_get_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_collector_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_collector_get_t *msg);

/*!
 * \brief Pack the flow data get message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_pack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_flow_data_get_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_unpack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_flow_data_get_t *msg);

/*!
 * \brief Pack the flow data get reply message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_reply_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_flow_data_get_reply_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_flow_data_get_reply_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_flow_data_get_reply_t *msg);

/*!
 * \brief Pack the FT ser event message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_ser_event_pack(uint8_t *buf,
                                      mcs_ft_msg_ctrl_ser_event_t *msg);

/*!
 * \brief Unpack the IFA stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_ser_event_unpack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_ser_event_t *msg);

/*!
 * \brief Pack the start the transmission of the template set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_xmit_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_template_xmit_t *msg);

/*!
 * \brief Unpack the start the transmission of the template set message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_template_xmit_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_template_xmit_t *msg);

/*!
 * \brief Pack the elephant profile create message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_create_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_elph_profile_create_t *msg);

/*!
 * \brief Unpack the elephant profile create message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_create_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_elph_profile_create_t *msg);

/*!
 * \brief Pack the elephant profile get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_get_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_elph_profile_get_t *msg);

/*!
 * \brief Unpack the elephant profile get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_elph_profile_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_elph_profile_get_t *msg);

/*!
 * \brief Pack the get learned stats message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_learn_get_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_stats_learn_t *msg);

/*!
 * \brief Unpack the get learned stats message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_learn_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_stats_learn_t *msg);

/*!
 * \brief Pack the export stats get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_export_get_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_stats_export_t *msg);

/*!
 * \brief Unpack the export stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_export_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_stats_export_t *msg);

/*!
 * \brief Pack the age stats get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_age_get_pack(uint8_t *buf,
                                          mcs_ft_msg_ctrl_stats_age_t *msg);

/*!
 * \brief Unpack the the age stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_age_get_unpack(uint8_t *buf,
                                            mcs_ft_msg_ctrl_stats_age_t *msg);

/*!
 * \brief Pack the elephant profile  stats get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_elph_get_pack(uint8_t *buf,
                                           mcs_ft_msg_ctrl_stats_elph_t *msg);

/*!
 * \brief Unpack the elephant profile  stats get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_stats_elph_get_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_stats_elph_t *msg);
/*!
 * \brief Pack the SDK features get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_sdk_features_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_sdk_features_t *msg);
/*!
 * \brief Unpack the SDK features get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_sdk_features_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_sdk_features_t *msg);

/*!
 * \brief Pack the UC features get message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_uc_features_pack(uint8_t *buf,
                                        mcs_ft_msg_ctrl_uc_features_t *msg);
/*!
 * \brief Unpack the UC features get message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_uc_features_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_uc_features_t *msg);
/*!
 * \brief Pack the group action set message.
 *
 * \param [out] buf Buffer to which the data needs to be packed.
 * \param [in]  msg Message that needs to be packed.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_actions_set_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_actions_set_t *msg);
 /*!
 * \brief Unpack the group action set message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_group_actions_set_unpack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_actions_set_t *msg);

/*!
 * \brief Unpack flow data get first reply message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_flow_data_get_first_reply_unpack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_flow_data_get_first_reply_t *msg);

/*!
 * \brief Unpack flow data get next reply message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_flow_data_get_next_reply_unpack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_flow_data_get_next_reply_t *msg);

/*!
 * \brief Unpack static flow data get first reply message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_static_flow_get_first_reply_unpack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_static_flow_get_first_reply_t *msg);

/*!
 * \brief Unpack static flow data get next reply message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after unpacking \c buf to \c msg.
 */
extern uint8_t *
bcmcth_mon_ft_static_flow_get_next_reply_unpack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_static_flow_get_next_reply_t *msg);

/*!
 * \brief Pack group flow data message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_group_flow_data_set_pack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_group_flow_data_set_t *msg);

/*!
 * \brief Pack the UFT bank info message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_em_bank_uft_info_send_msg_pack(
    uint8_t *buf,
    mcs_ft_msg_ctrl_em_bank_uft_info_t *msg);

/*!
 * \brief Pack the static flow set message.
 *
 * \param [in]  uc_msg_version Messaging version in use by UC.
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_static_flow_set_pack(
    uint32_t uc_msg_version,
    uint8_t *buf,
    mcs_ft_msg_ctrl_static_flow_set_t *msg);

/*!
 * \brief Pack the HW learn options message.
 *
 * \param [in]  buf Buffer from which the data needs to be unpacked.
 * \param [out] msg Unpacked message.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_hw_learn_opt_msg_pack(uint8_t *buf,
                                    mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg);

/*!
 * \brief Pack the RXPMD flex fields message.
 *
 * \param [in]  buf Buffer into which the data needs to be packed.
 * \param [in]  msg Message structure.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_rxpmd_flex_format_set_pack(uint8_t *buf,
                                    mcs_ft_msg_ctrl_rxpmd_flex_format_t *msg);

/*!
 * \brief Pack the match ID set message.
 *
 * \param [in]  buf Buffer into which the data needs to be packed.
 * \param [in]  msg Message structure.
 *
 * \return The new buffer pointer after packing \c msg in \c buf.
 */
extern uint8_t *
bcmcth_mon_ft_msg_ctrl_match_id_set_pack(uint8_t *buf,
                                         mcs_ft_msg_ctrl_match_id_set_t *msg);

#endif /*! BCMCTH_MON_FLOWTRACKER_PACK_H */
