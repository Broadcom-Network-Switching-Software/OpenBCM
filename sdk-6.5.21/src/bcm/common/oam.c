/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software and
 * all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 * LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
 * AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE
 * OF THE SOFTWARE. 
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and
 * to use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 * INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
 * RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF
 * BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE
 * ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL
 * APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED
 * REMEDY.
 */

#include <sal/core/libc.h>
#include <bcm/oam.h>

#if defined(INCLUDE_BHH)
#include <bcm_int/esw/bhh_feature.h>

/*Global to hold the bhh firmware version for feature compatibility check */
uint32 bhh_firmware_version = BHH_UC_MIN_VERSION;
#endif

#if defined(INCLUDE_ETH_LM_DM)
#include <bcm_int/esw/eth_lm_dm_feature.h>

/*Global to hold the eth lm/dm firmware version for feature compatibility check */
uint32 eth_lm_dm_firmware_version = ETH_LM_DM_UC_MIN_VERSION;
#endif

#if defined(INCLUDE_MPLS_LM_DM)
#include <bcm_int/esw/mpls_lm_dm_feature.h>

/*Global to hold the mpls lm/dm firmware version for feature compatibility check */
uint32 mpls_lm_dm_firmware_version = MPLS_LM_DM_UC_MIN_VERSION;
#endif
/*
 * Function:
 *      bcm_oam_group_info_t_init
 * Purpose:
 *      Initialize an OAM group info structure
 * Parameters:
 *      group_info - (OUT) Pointer to OAM group info structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void 
bcm_oam_group_info_t_init(
    bcm_oam_group_info_t *group_info)
{
    sal_memset(group_info, 0, sizeof(bcm_oam_group_info_t));
}

/*
 * Function:
 *      bcm_oam_endpoint_info_t_init
 * Purpose:
 *      Initialize an OAM endpoint info structure
 * Parameters:
 *      endpoint_info - (OUT) Pointer to OAM endpoint info structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void 
bcm_oam_endpoint_info_t_init(
    bcm_oam_endpoint_info_t *endpoint_info)
{
    sal_memset(endpoint_info, 0, sizeof(bcm_oam_endpoint_info_t));
    endpoint_info->gport            	= BCM_GPORT_INVALID;
    endpoint_info->tx_gport         	= BCM_GPORT_INVALID;
    endpoint_info->remote_gport     	= BCM_GPORT_INVALID;
    endpoint_info->mpls_out_gport   	= BCM_GPORT_INVALID;
    endpoint_info->action_reference_id 	= BCM_OAM_ENDPOINT_INVALID;
#ifdef INCLUDE_L3
    bcm_mpls_egress_label_t_init(&endpoint_info->egress_label);
#endif
}

/*
 * Function:
 *      bcm_oam_loss_t_init
 * Purpose:
 *      Initialize an OAM loss structure
 * Parameters:
 *      loss_ptr - (OUT) Pointer to OAM loss structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_loss_t_init(
    bcm_oam_loss_t *loss_ptr)
{
    sal_memset(loss_ptr, 0, sizeof(bcm_oam_loss_t));
    loss_ptr->pkt_pri_bitmap = ~0;
}

/*
 * Function:
 *      bcm_oam_delay_t_init
 * Purpose:
 *      Initialize an OAM delay structure
 * Parameters:
 *      delay_ptr - (OUT) Pointer to OAM delay structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_delay_t_init(
    bcm_oam_delay_t *delay_ptr)
{
    sal_memset(delay_ptr, 0, sizeof(bcm_oam_delay_t));
    delay_ptr->pkt_pri_bitmap = ~0;
}


/*
 * Function:
 *      bcm_oam_ais_t_init
 * Purpose:
 *      Initialize an AIS delay structure
 * Parameters:
 *      ais_ptr - (OUT) Pointer to OAM delay structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_ais_t_init(
    bcm_oam_ais_t *ais_ptr)
{
    sal_memset(ais_ptr, 0, sizeof(bcm_oam_ais_t));
}

/*
 * Function:
 *      bcm_oam_performance_event_data_t_init
 * Purpose:
 *      Initialize an bcm_oam_performance_event_data_t structure
 * Parameters:
 *      performance_event_data_ptr - (OUT) Pointer to bcm_oam_performance_event_data_t structure to be initialized
 */
/* Initialize an performance event data structure */
void bcm_oam_performance_event_data_t_init(
    bcm_oam_performance_event_data_t *performance_event_data_ptr)
{
    sal_memset(performance_event_data_ptr, 0, sizeof(bcm_oam_performance_event_data_t));
}

/*
 * Function:
 *      bcm_oam_psc_t_init
 * Purpose:
 *      Initialize an OAM PSC structure
 * Parameters:
 *      psc_ptr - (OUT) Pointer to OAM PSC structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_psc_t_init(
    bcm_oam_psc_t *psc_ptr)
{
    sal_memset(psc_ptr, 0, sizeof(bcm_oam_psc_t));
}

/*
 * Function:
 *      bcm_oam_loopback_t_init
 * Purpose:
 *      Initialize an OAM loopback structure
 * Parameters:
 *      loopback_ptr - (OUT) Pointer to OAM loopback structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_loopback_t_init(
    bcm_oam_loopback_t *loopback_ptr)
{
    sal_memset(loopback_ptr, 0, sizeof(bcm_oam_loopback_t));
	loopback_ptr->int_pri = -1;
	loopback_ptr->pkt_pri = 0xff;
	loopback_ptr->inner_pkt_pri = 0xff;
}

/*
 * Function:
 *      bcm_oam_pw_status_t_init
 * Purpose:
 *      Initialize an OAM PW Status structure
 * Parameters:
 *      pw_status_ptr - (OUT) Pointer to OAM PW Status structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_pw_status_t_init(
    bcm_oam_pw_status_t *pw_status_ptr)
{
    sal_memset(pw_status_ptr, 0, sizeof(bcm_oam_pw_status_t));
}

/*
 * Function:
 *      bcm_oam_tst_tx_t_init
 * Purpose:
 *      Initialize an tst_tx structure
 * Parameters:
 *      tst_tx_info - (OUT) Pointer to OAM tst_tx structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void bcm_oam_tst_tx_t_init(
    bcm_oam_tst_tx_t *tst_tx_info)
{
    sal_memset(tst_tx_info, 0, sizeof(bcm_oam_tst_tx_t));
}

/*
 * Function:
 *      bcm_oam_tst_rx_t_init
 * Purpose:
 *      Initialize an tst_rx structure
 * Parameters:
 *      tst_rx_info - (OUT) Pointer to OAM tst_rx structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void bcm_oam_tst_rx_t_init(
    bcm_oam_tst_rx_t *tst_rx_info)
{
    sal_memset(tst_rx_info, 0, sizeof(bcm_oam_tst_rx_t));
}

/*
 * Function:
 *      bcm_oam_csf_t_init
 * Purpose:
 *      Initialize an CSF delay structure
 * Parameters:
 *      csf_ptr - (OUT) Pointer to OAM delay structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_oam_csf_t_init(
    bcm_oam_csf_t *csf_ptr)
{
    sal_memset(csf_ptr, 0, sizeof(bcm_oam_csf_t));
}

/*
 * Function:
 *      bcm_oam_sd_sf_detection_t_init
 * Purpose:
 *      Initialize an sd_sf_detection structure
 * Parameters:
 *      sd_sf_ptr - (OUT) Pointer to OAM sd_sf_detection structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void bcm_oam_sd_sf_detection_t_init(
    bcm_oam_sd_sf_detection_t *sd_sf_ptr)
{
    sal_memset(sd_sf_ptr, 0, sizeof(bcm_oam_sd_sf_detection_t));
}

/*
 * Function:
 *      bcm_oam_y_1711_alarm_t_init
 * Purpose:
 *      Initialize an Y.1711 alarm structure
 * Parameters:
 *      sd_sf_ptr - (OUT) Pointer to OAM Y.1711 alarm structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void bcm_oam_y_1711_alarm_t_init(
    bcm_oam_y_1711_alarm_t *alarm_ptr)
{
    sal_memset(alarm_ptr, 0, sizeof(bcm_oam_y_1711_alarm_t));
}

/*
 * Function:
 *      bcm_oam_endpoint_action_t_init 
 * Purpose:
 *      Initialize an bcm_oam_endpoint_action_t structure
 * Parameters:
 *      action_ptr - (OUT) Pointer to OAM OPCODE ACTION structure 
 *                   to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void bcm_oam_endpoint_action_t_init(
    bcm_oam_endpoint_action_t *action_ptr)
{
    sal_memset(action_ptr, 0, sizeof(bcm_oam_endpoint_action_t));
    action_ptr->destination = BCM_GPORT_INVALID;
    action_ptr->destination2 = BCM_GPORT_INVALID;
}

 /*
  * Function:
  *      bcm_oam_action_key_t_init 
  * Purpose:
  *      Initialize a bcm_oam_action_key_t structure
  * Parameters:
  *      action_key_ptr - (OUT) Pointer to an OAM ACTION key 
  *                   to be initialized
  * Returns:
  *      void
  * Notes:
  */
 void bcm_oam_action_key_t_init(
     bcm_oam_action_key_t *action_key_ptr)
 {
     sal_memset(action_key_ptr, 0, sizeof(bcm_oam_action_key_t));
     action_key_ptr->dest_mac_type = bcmOAMDestMacTypeCount;
     action_key_ptr->endpoint_type = bcmOAMMatchTypeCount;
     action_key_ptr->opcode = bcmOamOpcodeMax;
 }
 
 /*
  * Function:
  *      bcm_oam_action_result_t_init 
  * Purpose:
  *      Initialize a bcm_oam_action_result_t structure
  * Parameters:
  *      action_result_ptr - (OUT) Pointer to an OAM ACTION result 
  *                   to be initialized
  * Returns:
  *      void
  * Notes:
  */
 void bcm_oam_action_result_t_init(
     bcm_oam_action_result_t *action_result_ptr)
 {
     sal_memset(action_result_ptr, 0, sizeof(bcm_oam_action_result_t));
 }
