/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
void
test_run_ebst(int unit)
{
	/* Using CPU tx to send unicast traffic in verifying status of EBST_DATA for the unicast queue */
	/* STEP 1. Test Setup */
	printf("\nTest Setup and Apply egress rate limiting on the observation port xe0\n");
	bshell(unit, "port cd0,xe0 lb=mac");
	bshell(unit, "vlan remove 1 pbm=cpu");
	bshell(unit, "port cd0,xe0 lb=mac");
	print bcm_port_rate_egress_set(unit, 38, 512, 256);  /* Apply egress rate limiting on the observation port xe0 */
	
	/* STEP 2. CPU start to send traffic */
	printf("\nCPU start sending known unicast packet\n");
    bshell(unit, "tx 20 pbm=cd0 length=1532 vlantag=0x1 sm=0x1 dm=0x2");
	
	bshell(unit, "sleep quiet 5");
	bshell(unit, "ps cd0,xe0");
	bshell(unit, "show c");
}

test_run_ebst(0);