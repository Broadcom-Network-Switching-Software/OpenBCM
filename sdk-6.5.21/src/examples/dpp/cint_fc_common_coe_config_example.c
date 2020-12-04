/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_fc_common_coe_config_example.c
 *
 * Background:
 *      COE is a common technology in our device to extend low rate ports, for example, create 8 times 1G front panel port on single serdes, 
 *      COE Tag is used to take channel port information.
 *
 * Purpose:
 *     Example of common COE flow control(FC) configration.
 *
 * Service Model:
 *          Physical port  + pri <----------------------> Locail port
 *
 *          Physical port      pri                       logical port            OTM-port(only support 1P mode)
 *          64                 0                            80                      80
 *          64                 1                            81                      81
 *          64                 2                            82                      82
 *          64                 3                            83                      83
 *          64                 4                            84                      84
 *          64                 5                            85                      85
 *          64                 6                            86                      86
 *          64                 7                            87                      87
 *
 *       Receive FC indication                                                   Stop related traffic
 *     PFC_0 on physcial port 64 ==================================>   priority 0 traffic out from logical port 80
 *     PFC_1 on physcial port 64 ==================================>   priority 1 traffic out from logical port 81
 *     PFC_2 on physcial port 64 ==================================>   priority 2 traffic out from logical port 82
 *     PFC_3 on physcial port 64 ==================================>   priority 3 traffic out from logical port 83
 *     PFC_4 on physcial port 64 ==================================>   priority 4 traffic out from logical port 84
 *     PFC_5 on physcial port 64 ==================================>   priority 5 traffic out from logical port 85
 *     PFC_6 on physcial port 64 ==================================>   priority 6 traffic out from logical port 86
 *     PFC_7 on physcial port 64 ==================================>   priority 7 traffic out from logical port 87
 *
 * To set port extender for COE on regular front panel port.
 *   1. Add below SOC properties:
 *     port_priorities.BCM88675=1   /* only support 1P mode */
 *     ucode_port_80.BCM88675=10GBase-R64.0:core_0.80
 *     ucode_port_81.BCM88675=10GBase-R64.1:core_0.81
 *     ucode_port_82.BCM88675=10GBase-R64.2:core_0.82
 *     ucode_port_83.BCM88675=10GBase-R64.3:core_0.83
 *     ucode_port_84.BCM88675=10GBase-R64.4:core_0.84
 *     ucode_port_85.BCM88675=10GBase-R64.5:core_0.85
 *     ucode_port_86.BCM88675=10GBase-R64.6:core_0.86
 *     ucode_port_87.BCM88675=10GBase-R64.7:core_0.87

 *   2. Run below
 *
 * To map FC indication in COE PFC packet to stop relate priority traffic, run:
 *     BCM> cint src/examples/dpp/cint_fc_common_coe_config_example.c
 *     BCM> cint
 *     cint> print cint_test_common_coe_fc_pfc_rec_set_example(/*unit*/0, /*coe_port*/81, /*coe_cosq*/1);
 */

/* Map FC indication in COE FC packet to stop a priority traffic */
int cint_test_common_coe_fc_pfc_rec_set_example(int unit, int coe_port, int coe_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;

    print bcm_port_control_set(unit, coe_port, bcmPortControlPFCReceive, 1);

    sal_memset(&fc_source, 0, sizeof(fc_source));
    sal_memset(&fc_source, 0, sizeof(fc_target));

    /* Source setting*/
    BCM_GPORT_LOCAL_SET(fc_source.port,coe_port);
    fc_source.cosq = coe_cosq;

    /* Target setting */
    BCM_GPORT_LOCAL_SET(fc_target.port, coe_port);
    fc_target.cosq = -1; /* must be -1 */

    rv = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("common coe pfc reception configuration failed(%d) in cint_test_common_coe_fc_pfc_rec_set_example\n", rv);
    }

    return rv;
}

/* Unmap FC indication in COE FC packet */
int cint_test_common_coe_fc_pfc_rec_unset_example(int unit, int coe_port, int coe_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_source;
    bcm_cosq_fc_endpoint_t fc_target;

    sal_memset(&fc_source, 0, sizeof(fc_source));
    sal_memset(&fc_source, 0, sizeof(fc_target));

    /* Source setting*/
    BCM_GPORT_LOCAL_SET(fc_source.port,coe_port);
    fc_source.cosq = coe_cosq;

    /* Target setting */
    BCM_GPORT_LOCAL_SET(fc_target.port, coe_port);
    fc_target.cosq = -1; /* must be -1 */

    rv = bcm_cosq_fc_path_delete(unit, bcmCosqFlowControlReception, &fc_source, &fc_target);
    if (rv != BCM_E_NONE) {
        printf("common coe pfc reception configuration failed(%d) in cint_test_common_coe_fc_pfc_rec_unset_example\n", rv);
    }

    return rv;
}
