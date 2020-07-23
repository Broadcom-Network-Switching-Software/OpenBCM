/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/drv.h>
#include <bcm/instru.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/control.h>


/*
 * Function:
 *      bcm_srv6_sid_initiator_info_t_init
 * Purpose:
 *      Initialize a SID initiator struct
 * Parameters:
 *      intf - pointer to the SID initiator struct
 * Returns:
 *      NONE
 */

extern void
bcm_srv6_sid_initiator_info_t_init(bcm_srv6_sid_initiator_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof (*info));
    }
}

/*
 * Function:
 *      bcm_srv6_srh_base_initiator_info_t_init
 * Purpose:
 *      Initialize a SID initiator struct
 * Parameters:
 *      intf - pointer to the SID initiator struct
 * Returns:
 *      NONE
 */

extern void
bcm_srv6_srh_base_initiator_info_t_init(bcm_srv6_srh_base_initiator_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof (*info));
    }
}


/*
 * Function:
 *      bcm_srv6_extension_terminator_mapping_t_init
 * Purpose:
 *      Initialize an SRv6 extension terminator mapping struct
 * Parameters:
 *      intf - pointer to the SRv6 extension terminator mapping struct
 * Returns:
 *      NONE
 */

extern void
bcm_srv6_extension_terminator_mapping_t_init(bcm_srv6_extension_terminator_mapping_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof (*info));
    }
}
