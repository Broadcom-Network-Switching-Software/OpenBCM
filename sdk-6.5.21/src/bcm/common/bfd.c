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

#if defined (INCLUDE_BFD)

#include <sal/core/libc.h>
#include <bcm/bfd.h>
#include <bcm_int/esw/bfd_feature.h>

/*Global to hold the bfd firmware version for feature compatibility check */
uint32 bfd_firmware_version = BFD_UC_MIN_VERSION;

/*
 * Function:
 *      bcm_bfd_endpoint_info_t_init
 * Purpose:
 *      Initialize an BFD endpoint info structure
 * Parameters:
 *      endpoint_info - (OUT) Pointer to BFD endpoint info structure to be initialized
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void 
bcm_bfd_endpoint_info_t_init(
    bcm_bfd_endpoint_info_t *endpoint_info)
{
    sal_memset(endpoint_info, 0, sizeof(bcm_bfd_endpoint_info_t));
    endpoint_info->gport = BCM_GPORT_INVALID;
    endpoint_info->tx_gport = BCM_GPORT_INVALID;
    endpoint_info->remote_gport = BCM_GPORT_INVALID;
#ifdef INCLUDE_L3 
    bcm_mpls_egress_label_t_init(&endpoint_info->egress_label);
#endif
    endpoint_info->egress_label.exp = 0xFF; 
}

#else /* INCLUDE_BFD */
typedef int _bcm_common_bfd_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_BFD */
