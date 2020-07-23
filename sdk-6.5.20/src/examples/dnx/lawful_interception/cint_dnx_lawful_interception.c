/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_lawful_interception.c
 * Purpose: Example of lawful interception(LI) configuration.
 */

/*
 * This cint demonstrate configuration of LI. Packet filter according to ingress PMF rule and mirrored to Interception port.
 * The encapsulation of LI outgoing packet can be following according to setting:
 *      MIR_COPYoLIoUDPoIPv6oETH1
 *      MIR_COPYoLIoUDPoIPv4oETH1
 * If packet is L3 service, LL header will be removed from mirror copy;
 * If packet is L2 service, LL header can optionally to be removed from mirror copy.
 *      
 * Parameter for main function: lawful_interception_main_config()
 *      in_port:        input port, hold in lawful_interception_info.pon_port
 *      mir_port:       mirror destination port for LI, hold in lawful_interception_info.nni_port
 *      is_remove_ll:   indication for remove LL from mirror copy
 *      is_ipv4_encap:  encapsulation type for mirror copy
 *
 * Basic Test Scenario 
 *
 
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/dnx/lawful_interception/cint_dnx_lawful_interception.c
 * cint ../../src/./examples/dnx/cint_inlif_wide_data.c
 * cint
 * bcm_vlan_gport_add(0,200,201,0);
 * bcm_vlan_gport_add(0,0xf,201,BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * inlif_large_wide_data_l3_fwd_config(0,0xf,203);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0064000000003c113d5da0000011a10000110000fedc00500000ffaabbcc451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0064000000003c113d5da0000011a10000110000fedc00500000aabbccdd451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0076000000003c113d4ba0000011a10000110000fedc00620000ffaabbcc000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0076000000003c113d4ba0000011a10000110000fedc00620000aabbccdd000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a29a4c0050113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00500000ffaabbcc451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a29a4c0050113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00500000aabbccdd451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a29a4c0062113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00620000ffaabbcc000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a29a4c0062113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00620000aabbccdd000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000c00020001000007000100810000c80800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0064000000003c113d5da0000011a10000110000fedc00500000ffaabbcc451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0064000000003c113d5da0000011a10000110000fedc00500000aabbccdd451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0076000000003c113d4ba0000011a10000110000fedc00620000ffaabbcc000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0076000000003c113d4ba0000011a10000110000fedc00620000aabbccdd000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a07b700050113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00500000ffaabbcc451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a07b700050113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00500000aabbccdd451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a07b700062113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00620000ffaabbcc000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a07b700062113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00620000aabbccdd000c000200010000070001008100000f0800451f0044beef00001efa9d05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c000200028100f0c80800451f0044beef00001dfa9e05c0a800027fffff02c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0078000000003c113d49a0000011a10000110000fedc00640000ffaabbcc600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0078000000003c113d49a0000011a10000110000fedc00640000aabbccdd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a008a000000003c113d37a0000011a10000110000fedc00760000ffaabbcc000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a008a000000003c113d37a0000011a10000110000fedc00760000aabbccdd000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a057da0064113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00640000ffaabbcc600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a057da0064113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00640000aabbccdd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a057da0076113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00760000ffaabbcc000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a057da0076113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00760000aabbccdd000c000200010000070001008100000f86dd600000000058fa80fe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c886dd600000000058fa7ffe800000000000000000000000000001fe800000000000000000000000000002c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0054000000003c113d6da0000011a10000110000fedc00400000ffaabbcc003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0054000000003c113d6da0000011a10000110000fedc00400000aabbccdd003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,1);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0066000000003c113d5ba0000011a10000110000fedc00520000ffaabbcc000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c0800450a0066000000003c113d5ba0000011a10000110000fedc00520000aabbccdd000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,1,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a2b9a10040113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00400000ffaabbcc003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a2b9a10040113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00400000aabbccdd003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 *
 * cint
 * lawful_interception_main_config(0,201,202,0,0);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a2b9a10052113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00520000ffaabbcc000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_LI_tunnel_replace(0,0xaabbccdd);
 * exit;
 *
 *   Sending pkt  
 * tx 1 psrc=201 data=0x000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *   Received packets on unit 0 should be:  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x00000000cd1d000c000200008100a12c86dd60a2b9a10052113cfc0e0d0c0b0a09080706050403020100afaeadacabaaa9a8a7a6a5a4a3a2a1a00000fedc00520000aabbccdd000c000200010000070001008100000f8847003e811ec5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *   Source port: 0, Destination port: 0  
 *   Data: 0x000102030405000c00020002810000c88847007d011dc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5  
 *
 * cint
 * lawful_interception_main_config_clean(0);
 * exit;
 */

struct ip_tunnel_s{
    bcm_mac_t da; /* ip tunnel da */
    bcm_mac_t sa; /* ip tunnel my mac */
    int       sip; /* ip tunnel sip */    
    int       dip; /* ip tunnel dip */   
    int       ttl; /* ip tunnel ttl */
    int       dscp; /* ip tunnel dscp */
    bcm_tunnel_dscp_select_t    dscp_sel; /* ip tunnel dscp_sel */
    int flags;
    uint16 udp_dest_port; /* udp dest port */
};

struct lawful_interception_info_s{
    int pon_port;
    int nni_port;
    int rcy_port;
    int out_vlan;
    ip_tunnel_s ip_tunnel;
    uint32 content_id;
    uint16 udp_dst_port;
    uint16 udp_src_port;
    
    bcm_gport_t tunnel_id;
    bcm_gport_t mirror_dest_id;
    bcm_l3_intf_t vxlan_if;
    bcm_l3_intf_t lawful_interception_if;
    bcm_if_t next_hop_if;
    
    bcm_field_presel_t pon_presel_id;
    bcm_field_presel_t rcy_presel_id;
    
    bcm_field_group_t filter_group_id;
    bcm_field_presel_set_t filter_group_psset;
    int filter_group_prio;
    bcm_field_entry_t filter_group_entry;
    uint32 dip_to_capture;
    
    bcm_field_group_t rcy_ext_group_id;
    bcm_field_presel_set_t rcy_ext_group_psset;
    int rcy_ext_group_prio;
    bcm_field_entry_t rcy_ext_group_entry;
    
    int strip_l2;
    int l3_enable;
    
    /* LI over IPv6 */
    bcm_ip6_t sip6;
    bcm_ip6_t dip6;
    uint32 flow_label;
};

lawful_interception_info_s lawful_interception_info;
bcm_field_group_t li_mirror_fg_id;
bcm_field_entry_t li_mirror_ent_id;

uint16 li_udp_dst_port=0xfedc;
uint32 content_id=0xffaabbcc;
uint32 capture_dip=0x7FFFFF02;


/*
 * init for mirror information
 */
int lawful_interception_info_init(int pon_port, int nni_port, int rcy_port, int out_vlan, uint32 content_id, uint16 udp_dst_port, uint16 udp_src_port, uint32 dip, int strip_l2, int l3_enable) {
    bcm_ip6_t dip6 = {0xAF, 0xAE, 0xAD, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8, 0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA0};
    bcm_ip6_t sip6 = {0xFC, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
    bcm_mac_t next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
    bcm_mac_t my_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};

    lawful_interception_info.pon_port = pon_port;
    lawful_interception_info.nni_port = nni_port;
    lawful_interception_info.rcy_port = rcy_port;
    lawful_interception_info.out_vlan = out_vlan;

    lawful_interception_info.ip_tunnel.da = next_hop_mac;
    lawful_interception_info.ip_tunnel.sa = my_mac;
    lawful_interception_info.ip_tunnel.sip = 0xA0000011;
    lawful_interception_info.ip_tunnel.dip = 0xA1000011;
    lawful_interception_info.ip_tunnel.ttl = 60;
    lawful_interception_info.ip_tunnel.dscp = 10;
    lawful_interception_info.ip_tunnel.dscp_sel = bcmTunnelDscpAssign;    
     
    lawful_interception_info.content_id = content_id;
    lawful_interception_info.udp_dst_port = udp_dst_port;
    lawful_interception_info.udp_src_port = udp_src_port;

    lawful_interception_info.filter_group_prio = BCM_FIELD_GROUP_PRIO_ANY;
    lawful_interception_info.rcy_ext_group_prio = BCM_FIELD_GROUP_PRIO_ANY;

    lawful_interception_info.dip_to_capture = dip;
    lawful_interception_info.strip_l2 = strip_l2;
    lawful_interception_info.l3_enable = l3_enable;

    lawful_interception_info.dip6 = dip6;
    lawful_interception_info.sip6 = sip6;
    lawful_interception_info.flow_label = 0xBEEF;
}

/*
 * create LI dest and setting for mirror information
 */
int lawful_interception_mirror_dest_create(int unit)
{
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t mirror_header_info_get;
    bcm_mirror_destination_t dest;
    uint32 flags = 0;
    int rv;

    bcm_mirror_destination_t_init(&dest);
    dest.gport = lawful_interception_info.pon_port;

    /** create mirror destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }
    

    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = lawful_interception_info.pon_port;
    mirror_header_info.tm.out_vport = lawful_interception_info.tunnel_id;

    if (lawful_interception_info.strip_l2 || lawful_interception_info.l3_enable) {
        mirror_header_info.pp.eth_header_remove = 1;
    }

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    lawful_interception_info.mirror_dest_id = dest.mirror_dest_id;

    return rv;
}


/*
 * create mirror rule through Ingress PMF for LI 
 */
int lawful_interception_filter_config(int unit, int in_port,bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_gport_t gport_qual;

    int rv = BCM_E_NONE;

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionMirrorIngress;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &li_mirror_fg_id);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    
    rv = bcm_field_group_context_attach(unit, 0, li_mirror_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    BCM_GPORT_LOCAL_SET(gport_qual, in_port);
    ent_info.entry_qual[0].value[0] = gport_qual;
    ent_info.entry_qual[0].mask[0] = 0xffffffff;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = lawful_interception_info.mirror_dest_id;

    printf("Adding Entry\n");
    rv = bcm_field_entry_add(unit, 0, li_mirror_fg_id, &ent_info, &li_mirror_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}


/*
 * replace LI shim encapsulation for mirror copy 
 */
int lawful_interception_LI_tunnel_replace(int unit, uint32 content_id)
{
    int rv;
    int encap_id;
    bcm_tunnel_initiator_t li_tunnel,li_tunnel_get;
    bcm_l3_intf_t li_intf;

    bcm_tunnel_initiator_t_init(&li_tunnel);
    li_tunnel.type = bcmTunnelTypeLawfulInterception;
    li_tunnel.flags = 0;
    li_tunnel.flags |= BCM_TUNNEL_WITH_ID | BCM_TUNNEL_REPLACE;
    li_tunnel.encap_access = bcmEncapAccessTunnel1;
    encap_id = BCM_L3_ITF_VAL_GET(lawful_interception_info.lawful_interception_if.l3a_intf_id);
    BCM_GPORT_TUNNEL_ID_SET(li_tunnel.tunnel_id, encap_id);
    
    li_tunnel.aux_data = content_id;
    li_tunnel.l3_intf_id = lawful_interception_info.vxlan_if.l3a_intf_id;
    bcm_l3_intf_t_init(&li_intf);
    rv = bcm_tunnel_initiator_create(unit, &li_intf, &li_tunnel);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create replace LI tunnel\n");
        return rv;
    }
    /**Get operation for LI tunnel*/
    bcm_tunnel_initiator_t_init(&li_tunnel_get);
    /**better to set the tunnel type as it's general*/
    li_tunnel_get.type = bcmTunnelTypeLawfulInterception;
    rv = bcm_tunnel_initiator_get(unit, &li_intf, &li_tunnel_get);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_get LI tunnel\n");
        return rv;
    }
    if ((li_tunnel_get.aux_data != li_tunnel.aux_data)
        || (li_tunnel_get.l3_intf_id) != lawful_interception_info.vxlan_if.l3a_intf_id) {
        printf("Error, bcm_tunnel_initiator_get, get LI tunnel isn't same with setting\n");
        printf("LI tunnel setting: %#x, LI tunnel getting: %#x\n", li_tunnel.aux_data, li_tunnel_get.aux_data);
        return BCM_E_INTERNAL;
    }

    printf(">>> LI tunnel replace successfully with contentID[%#x]\n", content_id);
    return rv;
}


/*
 * create encapsulation for mirror copy 
 */
int lawful_interception_tunnel_create(int unit, int is_ipv4_encap)
{
    create_l3_intf_s intf;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    ip_tunnel_s ip_tunnel = lawful_interception_info.ip_tunnel;
    uint32 flags;
    bcm_tunnel_initiator_t tunnel,li_tunnel,li_tunnel_get;
    bcm_l3_intf_t ip_intf, li_intf;
    int encap_id;
    
    int rv;

    intf.vsi = lawful_interception_info.out_vlan;
    intf.my_global_mac = ip_tunnel.sa;
    intf.my_lsb_mac = ip_tunnel.sa;
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rv;
    }

    bcm_l3_egress_t_init(&l3eg);
    flags = BCM_L3_EGRESS_ONLY;
    l3eg.encap_id = lawful_interception_info.next_hop_if;
    l3eg.port = lawful_interception_info.nni_port;  
    l3eg.mac_addr = ip_tunnel.da;
    l3eg.vlan = lawful_interception_info.out_vlan;  
    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    /**IP tunnel*/
    bcm_tunnel_initiator_t_init(&tunnel);
    if (lawful_interception_info.vxlan_if.l3a_intf_id != 0) {
        tunnel.flags |= BCM_TUNNEL_WITH_ID;
        encap_id = BCM_L3_ITF_VAL_GET(lawful_interception_info.vxlan_if.l3a_intf_id);
        BCM_GPORT_TUNNEL_ID_SET(tunnel.tunnel_id, encap_id);
    }

    if (is_ipv4_encap) {
        tunnel.type = bcmTunnelTypeUdp;
        tunnel.dip = ip_tunnel.dip;
        tunnel.sip = ip_tunnel.sip;
        tunnel.dscp = ip_tunnel.dscp; 
        tunnel.flags = 0;
        tunnel.ttl = ip_tunnel.ttl;
        tunnel.dscp_sel = ip_tunnel.dscp_sel;
        tunnel.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        tunnel.l3_intf_id = l3eg.encap_id;
        tunnel.encap_access = bcmEncapAccessTunnel4;
        tunnel.udp_dst_port = li_udp_dst_port;
    } else {
        tunnel.type = bcmTunnelTypeUdp6;
        tunnel.encap_access = bcmEncapAccessTunnel2;
        tunnel.l3_intf_id = l3eg.encap_id;
        sal_memcpy(&(tunnel.dip6),&(lawful_interception_info.dip6),16);
        sal_memcpy(&(tunnel.sip6),&(lawful_interception_info.sip6),16);
        tunnel.dscp = ip_tunnel.dscp; 
        tunnel.flags = 0;
        tunnel.ttl = ip_tunnel.ttl;
        tunnel.udp_dst_port = li_udp_dst_port;
    }

    bcm_l3_intf_t_init(&ip_intf);
    rv = bcm_tunnel_initiator_create(unit, &ip_intf, &tunnel);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create IP tunnel\n");
        return rv;
    }

    /**LI tunnel*/
    bcm_tunnel_initiator_t_init(&li_tunnel);
    li_tunnel.type = bcmTunnelTypeLawfulInterception;
    li_tunnel.flags = 0;
    if (lawful_interception_info.lawful_interception_if.l3a_intf_id != 0) {
        li_tunnel.flags |= BCM_TUNNEL_WITH_ID;
        encap_id = BCM_L3_ITF_VAL_GET(lawful_interception_info.lawful_interception_if.l3a_intf_id);
        BCM_GPORT_TUNNEL_ID_SET(li_tunnel.tunnel_id, encap_id);
    }
    li_tunnel.aux_data = lawful_interception_info.content_id;
    li_tunnel.l3_intf_id = ip_intf.l3a_intf_id;
    bcm_l3_intf_t_init(&li_intf);
    rv = bcm_tunnel_initiator_create(unit, &li_intf, &li_tunnel);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_create LI tunnel\n");
        return rv;
    }
    /**Get operation for LI tunnel*/
    bcm_tunnel_initiator_t_init(&li_tunnel_get);
    /**better to set the tunnel type as it's general*/
    li_tunnel_get.type = bcmTunnelTypeLawfulInterception;
    rv = bcm_tunnel_initiator_get(unit, &li_intf, &li_tunnel_get);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_get LI tunnel\n");
        return rv;
    }
    if ((li_tunnel_get.aux_data != li_tunnel.aux_data)
        || (li_tunnel_get.l3_intf_id) != ip_intf.l3a_intf_id) {
        printf("Error, bcm_tunnel_initiator_get, get LI tunnel isn't same with setting\n");
        printf("LI tunnel setting: %#x, LI tunnel getting: %#x\n", li_tunnel.aux_data, li_tunnel_get.aux_data);
        return BCM_E_INTERNAL;
    }

    
    lawful_interception_info.tunnel_id = li_tunnel.tunnel_id;
    /**Use JR1 para to hold ip tunnel*/
    lawful_interception_info.vxlan_if = ip_intf;
    lawful_interception_info.lawful_interception_if = li_intf;
    lawful_interception_info.next_hop_if = l3eg.encap_id;

    return BCM_E_NONE;    
}


/*
 * main function of setting for LI
 */
int lawful_interception_main_config(int unit, int in_port, int mir_port, int is_remove_ll, int is_ipv4_encap)
{
    int rv;
    /*
     * LI init
     */
    lawful_interception_info_init(in_port,mir_port,0,300,content_id,li_udp_dst_port,0,capture_dip,is_remove_ll,is_remove_ll); 
    /*
     * LI encapsulation create 
     */
    rv = lawful_interception_tunnel_create(unit, is_ipv4_encap);
    if (rv != BCM_E_NONE) {
        printf("Error, lawful_interception_tunnel_create\n");
        return rv;
    }
    printf(">>>lawful_interception_tunnel_create pass!\n");
    
    /*
     * mirror create
     */
    rv = lawful_interception_mirror_dest_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, lawful_interception_mirror_dest_create\n");
        return rv;
    }
    printf(">>>lawful_interception_mirror_dest_create pass!\n");

    /*
     * filter config
     */
    rv = lawful_interception_filter_config(unit, in_port, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE) {
        printf("Error, lawful_interception_filter_config\n");
        return rv;
    }
    printf(">>>lawful_interception_filter_config pass!\n");

    printf(">>>lawful_interception_main_config successfully!\n");
    return rv;
}


int lawful_interception_main_config_clean(int unit)
{
    int rv;

    /**clear PMF*/
    rv = bcm_field_entry_delete(unit, li_mirror_fg_id, NULL, li_mirror_ent_id);
    if (rv != BCM_E_NONE) {
        printf("Error in inlif_wide_data_general_create_udf_qual\n");
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, li_mirror_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE) {
        printf("Error in inlif_wide_data_general_create_udf_qual\n");
        return rv;
    }
    rv = bcm_field_group_delete(unit, li_mirror_fg_id);
    if (rv != BCM_E_NONE) {
        printf("Error in inlif_wide_data_general_create_udf_qual\n");
        return rv;
    }

    /**clear mirror dest*/
    rv = bcm_mirror_destination_destroy(unit, lawful_interception_info.mirror_dest_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_destination_destroy\n");
        return rv;
    }

    /**clear encap*/
    rv = bcm_tunnel_initiator_clear(unit,lawful_interception_info.lawful_interception_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_clear for LI tunnel\n");
        return rv;
    }

    rv = bcm_tunnel_initiator_clear(unit,lawful_interception_info.vxlan_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_initiator_clear for IP tunnel\n");
        return rv;
    }

    rv = bcm_l3_egress_destroy(unit, lawful_interception_info.next_hop_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_destroy\n");
        return rv;
    }

    printf(">>>lawful_interception_main_config_clean successfully!\n");
    return rv;
}

