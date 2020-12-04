/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
*
* File: cint_pon_pfc_inbnd_receive.c  
* 
* Purpose: examples of enable PFC receive on NNI port 132.
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/pon/cint_pon_pfc_inbnd_receive.c
*     BCM> cint
*     cint> cint_arad_test_pfc_inband_rec_set(0, 132);
 */
int cint_arad_test_pfc_inband_rec_set(int unit, int dest_port)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    int                           my_modid;

    bcm_stk_modid_get(unit, &my_modid);
	
    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_MODPORT_SET(source.port, 0, dest_port);
    source.cosq = 0;   /* Cosq = -1 indicates LL FC */
    target.cosq = 0;   /* can be any value between 0 and 7 */
    BCM_GPORT_MODPORT_SET(target.port, 0, dest_port);

    rc = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, PFC, port_gport(0x%x) err=%d %s\n", dest_port, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}
