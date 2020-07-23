/*! \file bcmcth_mon_flowtracker_int.h
 *
 * Flowtracker internal messaging function prototypes
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_INT_H
#define BCMCTH_MON_FLOWTRACKER_INT_H

#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>

/*! Software ID given for a Flowtracker export template configuration. */
typedef int bcmcth_mon_flowtracker_export_template_t;

/*! Software ID given for a Flowtracker flow group configuration. */
typedef int bcmcth_mon_flowtracker_group_t;

/*! Aging interval must be in steps of 1 sec */
#define FT_AGING_INTERVAL_STEP_MSECS (1000)

/*! Set Id used when transmiting template sets */
#define FT_INT_TEMPLATE_XMIT_SET_ID 2

/*! Default value of export interval. 100 ms */
#define FT_DEFAULT_EXPORT_INTERVAL_MSECS (100)

/*! Maximum number of elephant profiles */
#define FT_MAX_ELEPHANT_PROFILES 4

/*! Invalid EM group Id */
#define FT_INVALID_EM_GROUP_ID (0xFFFF)

/*! Message send routines to Firmware */

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_features_exchange
 * Purpose:
 *      Exchange features with the EApp
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      send_msg      - (IN)  SDK features message
 * Returns:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_features_exchange(int unit,
                                                mcs_ft_msg_ctrl_sdk_features_t *send_msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_ctrl_init
 * Purpose:
 *      Send FT init config to Eapp
 * Parameters:
 *      unit                    - (IN)   Unit number
 *      ctrl_init_msg           - (IN)   Init config message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_ctrl_init(int unit,
                                        mcs_ft_msg_ctrl_init_t *ctrl_init_msg);

/*!
 * Function:
 *      bcmcth_mon_ft_hw_learn_opt_init
 * Purpose:
 *      Send FT HW learn information to Eapp.
 * Parameters:
 *      unit                    - (IN)   Unit number
 *      msg                     - (IN)   HW learn options message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_hw_learn_opt_init(
                                int unit,
                                mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_em_key_format
 * Purpose:
 *      Send EM key format message to Eapp
 * Parameters:
 *      unit                    - (IN)   Unit number
 *      em_key_format           - (IN)   EM key format
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_em_key_format(int unit,
                                            mcs_ft_msg_ctrl_em_key_format_t *em_key_format);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_em_group_create
 * Purpose:
 *      Send message to Eapp to create EM group
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      msg                     - (IN)   EM group create message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_em_group_create(int unit,
                                              mcs_ft_msg_ctrl_em_group_create_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_em_group_delete
 * Purpose:
 *      Send message to Eapp to delete EM group
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      em_group_id             - (IN)   EM group id to be deleted
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_em_group_delete(int unit,
                                              int em_group_id);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_flow_group_create
 * Purpose:
 *      Send message to Eapp to create flow group
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      group_create_msg        - (IN)   Flow group create message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_flow_group_create(int unit,
                                                mcs_ft_msg_ctrl_group_create_t *group_create_msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_flow_group_delete
 * Purpose:
 *      Send message to Eapp to delete flow group
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      flow_group_id           - (IN)   Flow group id to be deleted
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_flow_group_delete(int unit,
                                                bcmcth_mon_flowtracker_group_t flow_group_id);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_flow_group_update
 * Purpose:
 *      Send message to Eapp to update flow group info
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      group_update_msg        - (IN)   Group update message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_flow_group_update(int unit,
                                                mcs_ft_msg_ctrl_group_update_t *group_update_msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_flow_group_get
 * Purpose:
 *      Send message to Eapp to get flow group count
 * Parameters:
 *      unit                - (IN)   Unit  number
 *      flow_group_id       - (IN)   Flow group id
 *      flow_count          - (OUT)  Flow group count
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_flow_group_get(int unit,
                                             bcmcth_mon_flowtracker_group_t flow_group_id,
                                             uint32_t *flow_count);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_template_create
 * Purpose:
 *      Send message to create export template to Eapp
 * Parameters:
 *      unit                - (IN)  Unit  number
 *      msg                 - (IN)  template create message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_template_create(int unit,
                                              mcs_ft_msg_ctrl_template_create_t *template_create_msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_template_delete
 * Purpose:
 *      Send message to delete template to Eapp
 * Parameters:
 *      unit                - (IN)  Unit  number
 *      template_id         - (IN)  template id to delete
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_template_delete(int unit,
                                              bcmcth_mon_flowtracker_export_template_t template_id);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_collector_create
 * Purpose:
 *      Send message to create collector to Eapp
 * Parameters:
 *      unit                - (IN)  Unit  number
 *      collector_create_msg - (IN) Collector info
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_collector_create(int unit,
                                               mcs_ft_msg_ctrl_collector_create_t *collector_create_msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_collector_delete
 * Purpose:
 *      Send message to delete collector to Eapp
 * Parameters:
 *      unit                - (IN)  Unit  number
 *      collector_id        - (IN)  Collector id to be deleted
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_collector_delete(int unit,
                                               uint16_t collector_id);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_export_interval_update
 * Purpose:
 *      Send export interval update message to Eapp
 * Parameters:
 *      unit                - (IN)  Unit  number
 *      export_interval     - (IN)  export_interval to update
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_export_interval_update(int unit,
                                                     uint32_t export_interval);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_template_xmit_start
 * Purpose:
 *      Send message to Eapp to start trasmit of template
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (IN)  Start transmit template msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_template_xmit_start(int unit,
                                                  mcs_ft_msg_ctrl_template_xmit_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_group_actions_set
 * Purpose:
 *      Send message to set group actions to Eapp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (IN) Group actions set msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_group_actions_set(int unit,
                                                mcs_ft_msg_ctrl_group_actions_set_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_first_flow_data
 * Purpose:
 *      Send message to get flow info of first index to Eapp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (OUT) flow info msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_get_first_flow_data(int unit,
                                                  mcs_ft_msg_ctrl_flow_data_get_first_reply_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_next_flow_data
 * Purpose:
 *      Send message to get flow info of given index to Eapp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      flow index    - (IN)  flow index
 *      msg           - (OUT) flow info msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_get_next_flow_data(int unit,
                                                 uint16_t flow_index,
                                                 mcs_ft_msg_ctrl_flow_data_get_next_reply_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_first_static_flow
 * Purpose:
 *      Send message to get first staic flow info to Eapp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (OUT) static flow info msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_get_first_static_flow(int unit,
                                                    mcs_ft_msg_ctrl_static_flow_get_first_reply_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_next_static_flow
 * Purpose:
 *      Send staic flow get message of given index to Eapp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      flow index    - (IN)  flow index
 *      msg           - (OUT) static flow info msg
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_get_next_static_flow(int unit,
                                                   uint16_t flow_index,
                                                   mcs_ft_msg_ctrl_static_flow_get_next_reply_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_group_flow_data_set
 * Purpose:
 *      Send group flow data clear message to EApp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (IN)  group flow data clear message
 * Returns:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_group_flow_data_set(int unit,
                                                  mcs_ft_msg_ctrl_group_flow_data_set_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_static_flow_set
 * Purpose:
 *      Send static flow create message to EApp
 * Parameters:
 *      unit          - (IN)  Unit  number
 *      msg           - (IN)  static flow create message
 * Returns:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_static_flow_set(int unit,
                                              mcs_ft_msg_ctrl_static_flow_set_t *msg);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_uft_bank_info
 * Purpose:
 *      Send UFT bank info to EApp
 * Parameters:
 *      unit          - (IN) Unit number
 *      msg           - (IN) uft bank info message
 * Returns:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_uft_bank_info(int unit,
                                            mcs_ft_msg_ctrl_em_bank_uft_info_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_flex_em_group_create
 * Purpose:
 *      Send Flex EM group create message to EApp
 * Parameters:
 *      unit          - (IN) Unit number
 *      msg           - (IN) Flex EM group info message
 * Returns:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_flex_em_group_create(int unit,
                                                   mcs_ft_msg_ctrl_flex_em_group_create_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_age_stats
 * Purpose:
 *      Get the age statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) age stats msg
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_get_age_stats(int unit,
                                 mcs_ft_msg_ctrl_stats_age_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_learn_stats
 * Purpose:
 *      Get the learn statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) learn stats msg
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_get_learn_stats(int unit,
                                   mcs_ft_msg_ctrl_stats_learn_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_export_stats
 * Purpose:
 *      Get the export statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) export stats msg
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_get_export_stats(int unit,
                                    mcs_ft_msg_ctrl_stats_export_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_elph_stats
 * Purpose:
 *      Get the elephant statistics
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (OUT) age stats msg
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */

extern int
bcmcth_mon_ft_eapp_get_elph_stats(int unit,
                                  mcs_ft_msg_ctrl_stats_elph_t *msg);


/*!
 * Function:
 *      bcmcth_mon_ft_eapp_observation_domain_update
 * Purpose:
 *      Update observation domain
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      e_num         - (IN)  Observation domain
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_observation_domain_update(int unit, uint32_t obs_domain);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_shutdown
 * Purpose:
 *      Shutdown FT Eapp
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      uc_num        - (IN)  UC number
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_shutdown(int unit, int uc_num);


/*!
 * Function:
 *      bcmcth_mon_ft_uc_appl_init
 * Purpose:
 *      Initialize FT Eapp
 * Parameters:
 *      unit           - (IN)  BCM device number
 *      uc_num         - (IN)  UC number
 *      uc_msg_version - (OUT) Msging version used by UC
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_uc_appl_init(int unit, int uc_num, uint32_t *uc_msg_version);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_elph_profile_create
 * Purpose:
 *      Send message to Eapp to create elephant profile
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      msg                     - (IN)   Elephant profile message
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_elph_profile_create(
        int unit,
        mcs_ft_msg_ctrl_elph_profile_create_t *msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_eleph_profile_delete
 * Purpose:
 *      Send message to Eapp to delete elephant profile
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      em_group_id             - (IN)   Id to be deleted
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_eapp_eleph_profile_delete(int unit, int id);

/*!
 * Function:
 *      bcmcth_mon_ft_group_lt_field_get
 * Purpose:
 *      Get MON_FLOWTRACKER_GROUP LT field entries
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      gen_field               - (IN)   Field to be retrieved
 *      entry                   - (OUT)  Group structure
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_group_lt_field_get(int unit,
                                            const bcmltd_field_t *gen_field,
                                            bcmcth_mon_ft_group_t *entry);
/*!
 * Function:
 *      bcmcth_mon_ft_control_entry_get
 * Purpose:
 *      Get MON_FLOWTRACKER_CONTROL LT field entries
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      entry                   - (OUT)  Control structure
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_control_entry_get(int unit, bcmcth_mon_ft_control_t *entry);

/*!
 * Function:
 *      bcmcth_mon_ft_group_entry_get
 * Purpose:
 *      Get MON_FLOWTRACKER_GROUP LT field entries
 * Parameters:
 *      unit                    - (IN)   Unit  number
 *      id                      - (IN)   Flow group ID
 *      entry                   - (OUT)  Group structure
 * Return:
 *      SHR_E_XXX   - SHR return code.
 */
extern int bcmcth_mon_ft_group_entry_get(int unit,
                                         uint32_t id,
                                         bcmcth_mon_ft_group_t *entry);
/*!
 * Function:
 *      bcmcth_mon_ft_eapp_get_flow_data
 * Purpose:
 *      Given a flow tuple, get the flow data
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (IN)  Flow data get send message
 *      reply_msg     - (IN)  Reply message for Flow data get message
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_get_flow_data(
                       int unit,
                       mcs_ft_msg_ctrl_group_flow_data_get_t *msg,
                       mcs_ft_msg_ctrl_group_flow_data_get_reply_t *reply_msg);

/*!
 * Function:
 *      bcmcth_mon_ft_eapp_static_flow_delete
 * Purpose:
 *      Given a static flow's tuple, delete the flow
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (IN)  Static flow delete send message
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_eapp_static_flow_delete(int unit,
                                      mcs_ft_msg_ctrl_static_flow_set_t *msg);
/*!
 * Function:
 *     bcmcth_mon_ft_rxmpd_flex_format_set
 * Purpose:
 *     Sends the RXPMD flex fields message.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (IN)  RXPMD flex fields message.
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_rxmpd_flex_format_set(int unit,
                             mcs_ft_msg_ctrl_rxpmd_flex_format_t *msg);

/*!
 * Function:
 *     bcmcth_mon_ft_match_id_set
 * Purpose:
 *     Sends the match ID set message.
 * Parameters:
 *      unit          - (IN)  BCM device number
 *      msg           - (IN)  Match ID set message.
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
extern int
bcmcth_mon_ft_match_id_set(int unit, mcs_ft_msg_ctrl_match_id_set_t *msg);

#endif /*! BCMCTH_MON_FLOWTRACKER_INT_H */
