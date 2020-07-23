/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tc_config.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_common_ptp_primary_domain_get
 *      bcm_common_ptp_primary_domain_set
 *      bcm_common_ptp_transparent_clock_default_dataset_get
 *      bcm_common_ptp_transparent_clock_port_dataset_get
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

/*
 * Function:
 *      bcm_common_ptp_primary_domain_get
 * Purpose:
 *      Get primary domain member of a PTP transparent clock default dataset.
 * Parameters:
 *      unit           - (IN)  Unit number.
 *      ptp_id         - (IN)  PTP stack ID.
 *      clock_num      - (IN)  PTP clock number.
 *      primary_domain - (OUT) PTP transparent clock primary domain.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Ref. IEEE Std. 1588-2008, Chapters 8.3.2 and 15.5.3.9.1.
 */
int 
bcm_common_ptp_primary_domain_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int *primary_domain)
{
    int rv = BCM_E_UNAVAIL;
    
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 

    bcm_ptp_port_identity_t portid;     
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT)))
    {
        return rv;   
    }
        
    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid)))
    {
        return rv;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
            PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_PRIMARY_DOMAIN, 
            0, 0, resp, &resp_len);

    if (rv == BCM_E_NONE) 
    {
        /* 
         * Parse response.
         *    Octet 0 : Primary domain.
         *    Octet 1 : Reserved.
         */
        *primary_domain = (int8)resp[0];
    }
    
    return rv;
}

/*
 * Function:
 *       bcm_common_ptp_primary_domain_set
 * Purpose:
 *      Set primary domain member of a PTP transparent clock default dataset.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (IN) PTP clock number.
 *      primary_domain - (IN) PTP transparent clock primary domain.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Ref. IEEE Std. 1588-2008, Chapters 8.3.2 and 15.5.3.9.1.
 */
int 
bcm_common_ptp_primary_domain_set(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int primary_domain)
{
    int rv = BCM_E_UNAVAIL;
    
    uint8 payload[PTP_MGMTMSG_PAYLOAD_MIN_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 

    bcm_ptp_port_identity_t portid;     
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT)))
    {
        return rv;   
    }
          
    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid)))
    {
        return rv;
    }

    /* 
     * Make payload.
     *    Octet 0 : Primary domain.
     *    Octet 1 : Reserved.
     */
    payload[0] = (uint8)primary_domain;

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
            PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_PRIMARY_DOMAIN, 
            payload, PTP_MGMTMSG_PAYLOAD_MIN_SIZE_OCTETS, 
            resp, &resp_len);
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_transparent_clock_default_dataset_get
 * Purpose:
 *      Get PTP Transparent clock default dataset
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      dataset   - (OUT) PTP transparent clock default dataset.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The transparent clock default dataset is relevant to PTP transparent 
 *      clocks.
 *      Ref. IEEE Std. 1588-2008, Chapters 8.3.2 and 15.5.3.8.1.
 */
int 
bcm_common_ptp_transparent_clock_default_dataset_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    bcm_ptp_transparent_clock_default_dataset_t *dataset)
{
    int rv = BCM_E_UNAVAIL;
    
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 
    int i = 0;

    bcm_ptp_port_identity_t portid;     
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT)))
    {
        return rv;   
    }
        
    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, PTP_IEEE1588_ALL_PORTS, &portid)))
    {
        return rv;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
            PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TRANSPARENT_CLOCK_DEFAULT_DATASET, 
            0, 0, resp, &resp_len);

    if (rv == BCM_E_NONE) 
    {
        /* 
         * Parse response.
         *    Octet 0...7 : Clock identity.
         *    Octet 8...9 : Number of ports.
         *    Octet 10    : Delay mechanism.
         *    Octet 11    : Primary domain.
         */
        i = 0;
        sal_memcpy(dataset->clock_identity, resp, 
                   sizeof(bcm_ptp_clock_identity_t));
        i += sizeof(bcm_ptp_clock_identity_t);

        dataset->number_ports = _bcm_ptp_uint16_read(resp+i);
        i += 2;

        dataset->delay_mechanism = resp[i++];
        dataset->primary_domain = resp[i];
    }
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_transparent_clock_port_dataset_get
 * Purpose:
 *      Get PTP transparent clock port dataset. 
 * Parameters:
 *      unit      -  (IN)  Unit number.
 *      ptp_id    -  (IN)  PTP stack ID.
 *      clock_num -  (IN)  PTP clock number.
 *      clock_port - (IN)  PTP clock port number.
 *      dataset   -  (OUT) PTP transparent clock port dataset.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The transparent clock port dataset is relevant to PTP transparent 
 *      clocks.
 *      Ref. IEEE Std. 1588-2008, Chapters 8.3.3 and 15.5.3.10.1.
 */
int 
bcm_common_ptp_transparent_clock_port_dataset_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    uint16 clock_port, 
    bcm_ptp_transparent_clock_port_dataset_t *dataset)
{
    int rv = BCM_E_UNAVAIL;
    
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 
    int i = 0;
    
    bcm_ptp_port_identity_t portid;     
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            (uint32)clock_port)))
    {
        return rv;   
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, clock_port, &portid)))
    {
        return rv;
    }

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid, 
            PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_TRANSPARENT_CLOCK_PORT_DATASET, 
            0, 0, resp, &resp_len);

    if (rv == BCM_E_NONE) 
    {
        /* 
         * Parse response.
         *    Octet 0...9   : PTP port identity.
         *    Octet 10      : Faulty (FLT) Boolean.
         *                    |B7|B6|B5|B4|B3|B2|B1|B0| = |0|0|0|0|0|0|0|FLT|
         *    Octet 11      : Log minimum peer delay request interval.
         *    Octet 12...19 : Peer mean path delay (nanoseconds).
         */
        i = 0;
        sal_memcpy(dataset->port_identity.clock_identity, resp, 
                   sizeof(bcm_ptp_clock_identity_t));
        i += sizeof(bcm_ptp_clock_identity_t);

        dataset->port_identity.port_number = _bcm_ptp_uint16_read(resp+i);
        i += 2;

        dataset->faulty = (0x01 & resp[i++]);

        dataset->log_min_pdelay_req_interval = resp[i++];
        dataset->peer_mean_path_delay = _bcm_ptp_uint64_read(resp+i);
    }
    
    return rv;
}
#endif /* defined(INCLUDE_PTP)*/
