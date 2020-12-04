/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PSTATS - Broadcom StrataSwitch PSTATS API
 */

#include <soc/drv.h>
#include <bcm/pstats.h>

#if defined(INCLUDE_PSTATS)
/*
 * Function:
 *      bcm_pstats_timestamp_t_init
 * Purpose:
 *      Initialize an PSTATS timestamp structure
 * Parameters:
 *      timestamp - (OUT) pointer to bcm_pstats_timestamp_t structure
 */
void
bcm_pstats_timestamp_t_init(bcm_pstats_timestamp_t *timestamp)
{
    if (NULL != timestamp) {
        sal_memset(timestamp, 0, sizeof (*timestamp));
    }
    return;
}

/*
 * Function:
 *      bcm_pstats_session_element_t_init
 * Purpose:
 *      Initialize an PSTATS session element structure
 * Parameters:
 *      timestamp - (OUT) pointer to bcm_pstats_session_element_t structure
 */
void
bcm_pstats_session_element_t_init(bcm_pstats_session_element_t *element)
{
    if (NULL != element) {
        sal_memset(element, 0, sizeof (*element));
        element->type = bcmPStatsSessionTypeCount;
        element->gport = BCM_GPORT_INVALID;
        element->cosq = -1;
        element->pool = -1;
    }

    return;
}

/*
 * Function:
 *      bcm_pstats_data_t_init
 * Purpose:
 *      Initialize an PSTATS data structure
 * Parameters:
 *      timestamp - (OUT) pointer to bcm_pstats_data_t structure
 */
void
bcm_pstats_data_t_init(bcm_pstats_data_t *data)
{
    if (NULL != data) {
        sal_memset(data, 0, sizeof (*data));
    }
    return;
}
#endif /* INCLUDE_PSTATS */
