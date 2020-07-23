/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * UDF initializers
 */

#include <sal/core/libc.h>
 
#include <bcm/udf.h>

/* 
 * Function:
 *      bcm_udf_info_t_init
 * Purpose:
 *      Initialize UDF Info struct
 * Parameters: 
 *      udf_info - Pointer to the struct to be init'ed
 */
void
bcm_udf_t_init(bcm_udf_t *udf_info)
{
    if (udf_info != NULL) {
        sal_memset(udf_info, 0, sizeof(*udf_info));
    }
    return;
}


/* 
 * Function:
 *      bcm_udf_alloc_hints_t_init
 * Purpose:
 *      Initialize UDF alloc hints struct
 * Parameters: 
 *      udf_hints - Pointer to the struct to be init'ed
 */
void
bcm_udf_alloc_hints_t_init(bcm_udf_alloc_hints_t *udf_hints)
{
    if (udf_hints != NULL) {
        sal_memset(udf_hints, 0, sizeof(*udf_hints));
    }
    return;
}


/*
 * Function:
 *      bcm_udf_pkt_format_info_t_init
 * Purpose:
 *      Initialize UDF packet format struct
 * Parameters:
 *      udf_format - Pointer to the struct to be init'ed
 */
void
bcm_udf_pkt_format_info_t_init(bcm_udf_pkt_format_info_t *pkt_format)
{
    if (pkt_format != NULL) {
        sal_memset(pkt_format, 0, sizeof(*pkt_format));

#if 0
        /* Will not be required with updated proposal - ANY -> 0 */
        udf_format->l2  = BCM_PKT_FORMAT_L2_ANY;
        udf_format->vlan_tag  = BCM_PKT_FORMAT_VLAN_TAG_ANY;
        udf_format->outer_ip  = BCM_PKT_FORMAT_IP_ANY;
        udf_format->inner_ip  = BCM_PKT_FORMAT_IP_ANY;
        udf_format->tunnel  = BCM_PKT_FORMAT_TUNNEL_ANY;
        udf_format->mpls  = BCM_PKT_FORMAT_MPLS_ANY;
        udf_format->fibre_chan_outer  = BCM_PKT_FORMAT_FIBRE_CHAN_ANY;
        udf_format->fibre_chan_inner  = BCM_PKT_FORMAT_FIBRE_CHAN_ANY;

        
#endif /* 0 */

    }

    return;
}

/* 
 * Function:
 *      bcm_udf_hash_config_t_init
 * Purpose:
 *      Initialize UDF hashing configuration structure
 * Parameters: 
 *      info - Pointer to the struct to be init'ed
 */
void
bcm_udf_hash_config_t_init(bcm_udf_hash_config_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/* 
 * Function:
 *      bcm_udf_chunk_info_t_init
 * Purpose:
 *      Initialize UDF Chunk Info struct
 * Parameters: 
 *      udf_chunk_info - Pointer to the struct to be init'ed
 */
void
bcm_udf_chunk_info_t_init(bcm_udf_chunk_info_t *udf_chunk_info)
{
    if (udf_chunk_info != NULL) {
        sal_memset(udf_chunk_info, 0, sizeof(bcm_udf_chunk_info_t));
    }
    return;
}

/* 
 * Function:
 *      bcm_udf_multi_chunk_info_t_init
 * Purpose:
 *      Initialize UDF Multi Chunk Info struct
 * Parameters: 
 *      udf_multi_chunk_info - Pointer to the struct to be init'ed
 */
void
bcm_udf_multi_chunk_info_t_init(bcm_udf_multi_chunk_info_t *udf_multi_chunk_info)
{
    if (udf_multi_chunk_info != NULL) {
        sal_memset(udf_multi_chunk_info, 0, sizeof(bcm_udf_multi_chunk_info_t));
    }
    return;
}
