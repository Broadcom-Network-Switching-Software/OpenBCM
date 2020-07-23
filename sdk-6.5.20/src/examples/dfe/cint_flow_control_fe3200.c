/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE configure FIFOs example:
 *  
 * The test generate four types of fifo_type, 
 * it configure thresholds for each type and attach links 
 * to the fifo_types 
 *  
 * At the end the test validate configuration 
 */

struct cint_fifo_type_test_s {
    bcm_fabric_pipe_t fabric_pipe;
    int is_fe13;
    int fe1_threshold_values[11];
    int fe3_threshold_values[11];
    bcm_fabric_link_threshold_type_t threshold_types[11];
    int nof_thresholds_configured ;   
};

int cint_flow_control_fe3200_init_fifo_types(int unit, cint_fifo_type_test_s* fifo_types, int is_fe13)
{

    fifo_types[0].fabric_pipe = bcmFabricPipe0;
    fifo_types[0].is_fe13 = is_fe13 ? 1:0;
    fifo_types[0].fe1_threshold_values[0] = 13;
    fifo_types[0].fe1_threshold_values[1] = 14;
    fifo_types[0].fe1_threshold_values[2] = 15;
    fifo_types[0].fe1_threshold_values[3] = 50;
    fifo_types[0].fe1_threshold_values[4] = 52;
    fifo_types[0].fe1_threshold_values[5] = 75;
    fifo_types[0].fe1_threshold_values[6] = 76;
    fifo_types[0].fe1_threshold_values[7] = 87;
    fifo_types[0].fe3_threshold_values[0] = 29;
    fifo_types[0].fe3_threshold_values[1] = 30;
    fifo_types[0].fe3_threshold_values[2] = 31;
    fifo_types[0].fe3_threshold_values[3] = 51;
    fifo_types[0].fe3_threshold_values[4] = 52;
    fifo_types[0].fe3_threshold_values[5] = 77;
    fifo_types[0].fe3_threshold_values[6] = 76;
    fifo_types[0].fe3_threshold_values[7] = 87;
    fifo_types[0].threshold_types[0] = bcmFabricLinkRxFifoLLFC;
    fifo_types[0].threshold_types[1] = bcmFabricLinkFE1TxBypassLLFC;
    fifo_types[0].threshold_types[2] = bcmFabricLinkRciFC;
    fifo_types[0].threshold_types[3] = bcmFabricLinkRxMcLowPrioDrop;
    fifo_types[0].threshold_types[4] = bcmFabricLinkMidGciLvl1FC;
    fifo_types[0].threshold_types[5] = bcmFabricLinkRxFull;
    fifo_types[0].threshold_types[6] = bcmFabricLinkMidRciLvl1FC;
    fifo_types[0].threshold_types[7] = bcmFabricLinkMidAlmostFull;
    fifo_types[0].nof_thresholds_configured = 8;

    fifo_types[1].fabric_pipe = bcmFabricPipe1;
    fifo_types[1].is_fe13 = is_fe13 ? 1:0;
    fifo_types[1].fe1_threshold_values[0] = 17;
    fifo_types[1].fe1_threshold_values[1] = 18;
    fifo_types[1].fe1_threshold_values[2] = 19;
    fifo_types[1].fe1_threshold_values[3] = 57;
    fifo_types[1].fe1_threshold_values[4] = 71;
    fifo_types[1].fe1_threshold_values[5] = 73;
    fifo_types[1].fe1_threshold_values[6] = 80;
    fifo_types[1].fe1_threshold_values[7] = 90;
    fifo_types[1].fe1_threshold_values[8] = 95;
    fifo_types[1].fe1_threshold_values[9] = 94;
    fifo_types[1].fe1_threshold_values[10] = 88;
    fifo_types[1].fe3_threshold_values[0] = 31;
    fifo_types[1].fe3_threshold_values[1] = 32;
    fifo_types[1].fe3_threshold_values[2] = 33;
    fifo_types[1].fe3_threshold_values[3] = 57;
    fifo_types[1].fe3_threshold_values[4] = 71;
    fifo_types[1].fe3_threshold_values[5] = 73;
    fifo_types[1].fe3_threshold_values[6] = 80;
    fifo_types[1].fe3_threshold_values[7] = 91;
    fifo_types[1].fe3_threshold_values[8] = 93;
    fifo_types[1].fe3_threshold_values[9] = 92;
    fifo_types[1].fe3_threshold_values[10] = 89;
    fifo_types[1].threshold_types[0] = bcmFabricLinkTxPrio3Drop;
    fifo_types[1].threshold_types[1] = bcmFabricLinkTxGciLvl1FC;
    fifo_types[1].threshold_types[2] = bcmFabricLinkTxGciLvl2FC;
    fifo_types[1].threshold_types[3] = bcmFabricLinkMidGciLvl2FC;
    fifo_types[1].threshold_types[4] = bcmFabricLinkMidPrio0Drop;
    fifo_types[1].threshold_types[5] = bcmFabricLinkMidPrio1Drop;
    fifo_types[1].threshold_types[6] = bcmFabricLinkMidPrio2Drop;
    fifo_types[1].threshold_types[7] = bcmFabricLinkTxAlmostFull;
    fifo_types[1].threshold_types[8] = bcmFabricLinkTxRciLvl2FC;
    fifo_types[1].threshold_types[9] = bcmFabricLinkTxGciLvl3FC;
    fifo_types[1].threshold_types[10] = bcmFabricLinkMidFull;
    fifo_types[1].nof_thresholds_configured = 11;

    fifo_types[2].fabric_pipe = bcmFabricPipe2;
    fifo_types[2].is_fe13 = is_fe13 ? 1:0;
    fifo_types[2].fe1_threshold_values[0] = 19;
    fifo_types[2].fe1_threshold_values[1] = 20;
    fifo_types[2].fe1_threshold_values[2] = 21;
    fifo_types[2].fe1_threshold_values[3] = 22;
    fifo_types[2].fe1_threshold_values[4] = 61;
    fifo_types[2].fe1_threshold_values[5] = 78;
    fifo_types[2].fe1_threshold_values[6] = 79;
    fifo_types[2].fe1_threshold_values[7] = 85;
    fifo_types[2].fe1_threshold_values[8] = 97;
    fifo_types[2].fe1_threshold_values[9] = 96;
    fifo_types[2].fe3_threshold_values[0] = 37;
    fifo_types[2].fe3_threshold_values[1] = 38;
    fifo_types[2].fe3_threshold_values[2] = 39;
    fifo_types[2].fe3_threshold_values[3] = 40;
    fifo_types[2].fe3_threshold_values[4] = 61;
    fifo_types[2].fe3_threshold_values[5] = 78;
    fifo_types[2].fe3_threshold_values[6] = 79;
    fifo_types[2].fe3_threshold_values[7] = 85;
    fifo_types[2].fe3_threshold_values[8] = 98;
    fifo_types[2].fe3_threshold_values[9] = 97;
    fifo_types[2].threshold_types[0] = bcmFabricLinkTxPrio2Drop;
    fifo_types[2].threshold_types[1] = bcmFabricLinkTxGciLvl3FC;
    fifo_types[2].threshold_types[2] = bcmFabricLinkTxPrio0Drop;
    fifo_types[2].threshold_types[3] = bcmFabricLinkTxPrio1Drop;
    fifo_types[2].threshold_types[4] = bcmFabricLinkMidGciLvl3FC;
    fifo_types[2].threshold_types[5] = bcmFabricLinkMidRciLvl2FC;
    fifo_types[2].threshold_types[6] = bcmFabricLinkMidRciLvl3FC;
    fifo_types[2].threshold_types[7] = bcmFabricLinkMidPrio3Drop;
    fifo_types[2].threshold_types[8] = bcmFabricLinkTxRciLvl3FC;
    fifo_types[2].threshold_types[9] = bcmFabricLinkTxRciLvl1FC;
    fifo_types[2].nof_thresholds_configured = 10;

    fifo_types[3].fabric_pipe = bcmFabricPipeAll;
    fifo_types[3].is_fe13 = is_fe13 ? 1:0;
    fifo_types[3].fe1_threshold_values[0] = 23;
    fifo_types[3].fe1_threshold_values[1] = 24;
    fifo_types[3].fe1_threshold_values[2] = 25;
    fifo_types[3].fe3_threshold_values[0] = 41;
    fifo_types[3].fe3_threshold_values[1] = 42;
    fifo_types[3].fe3_threshold_values[2] = 43;
    fifo_types[3].threshold_types[0] = bcmFabricLinkRxGciLvl1FC;
    fifo_types[3].threshold_types[1] = bcmFabricLinkRxGciLvl2FC;
    fifo_types[3].threshold_types[2] = bcmFabricLinkRxGciLvl3FC;
    fifo_types[3].nof_thresholds_configured = 3;

    fifo_types[4].fabric_pipe = bcmFabricPipeAll;
    fifo_types[4].is_fe13 = is_fe13 ? 1:0;
    fifo_types[4].fe1_threshold_values[0] = 44;
    fifo_types[4].fe1_threshold_values[1] = 45;
    fifo_types[4].fe1_threshold_values[2] = 46;
    fifo_types[4].fe3_threshold_values[0] = 47;
    fifo_types[4].fe3_threshold_values[1] = 48;
    fifo_types[4].fe3_threshold_values[2] = 49;
    fifo_types[4].threshold_types[0] = bcmFabricLinkRxRciLvl1FC;
    fifo_types[4].threshold_types[1] = bcmFabricLinkRxRciLvl2FC;
    fifo_types[4].threshold_types[2] = bcmFabricLinkRxRciLvl3FC;
    fifo_types[4].nof_thresholds_configured = 3;

}

/* type 0 : 0-35,72- 107  type 1: 36- 71, 108-143  */
int cint_flow_control_fe3200_attach_links_to_fifos(int unit)
{
    int links_arr0[72];
    int links_arr1[72];
    int i, rv;
    
    for (i=0 ; i<36 ; i++)
    {
        links_arr0[i]= i; 
        links_arr0[i+36]= i+72;
        links_arr1[i]= i+36;
        links_arr1[i+36]= i+108;
    }


    rv = bcm_fabric_link_thresholds_attach(unit, 0, 72, links_arr0);

    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_attach\n");
        return BCM_E_FAIL;
    }

    rv = bcm_fabric_link_thresholds_attach(unit, 1, 72, links_arr1);

    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_attach\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int cint_flow_control_fe3200_retrieve_links_and_compare(int unit)
{
    int links_arr0[144];   /* size is 144 to prevent seg fault */
    int links_arr1[144];
    int rv, i;
    uint32 links_type_0, links_type_1;

    rv = bcm_fabric_link_thresholds_retrieve(unit, 0, 144, links_arr0, &links_type_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_retrieve\n");
        return BCM_E_FAIL;
    }

    rv = bcm_fabric_link_thresholds_retrieve(unit, 1, 144, links_arr1, &links_type_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_retrieve\n");
        return BCM_E_FAIL;
    }

    if ( (links_type_0 != 72) || (links_type_1 != 72) )
    {
        printf("Error cint_flow_control_fe3200 - expected number of attached links doesn't match actual number \n");
        return BCM_E_FAIL;
    }

    for (i=0; i<72; i++) 
    {
        if (links_arr0[i]< 0 || (links_arr0[i] > 35 && links_arr0[i] < 72)  || links_arr0[i] > 107)
        {
            printf("Error cint_flow_control_fe3200 - unexpected link in type 0");
            return BCM_E_FAIL;
        }
        if (links_arr1[i] < 36 || (links_arr1[i] > 71 && links_arr1[i] < 108) || links_arr1[i] > 143)
        {
            printf("Error cint_flow_control_fe3200 - unexpected link in type 1");
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

int cint_flow_control_fe3200_set_thresholds_to_fifos(int unit, cint_fifo_type_test_s* fifo_types)
{
    int i, j, rv;
    int fifo_size_values[3] = {120,124,128}; /* DCH, DCM, DCL fifo size values accordingly */
    bcm_fabric_link_threshold_type_t fifo_size_thresholds[3] = {bcmFabricLinkRxFifoSize, bcmFabricLinkMidFifoSize, bcmFabricLinkTxFifoSize} ;

    /* set fifo sizes  - */

    for (i=0 ; i < 2 ; i++)
    {
        rv = bcm_fabric_link_thresholds_pipe_set(unit, i, bcmFabricPipeAll, 0, 3, fifo_size_thresholds, fifo_size_values);
        if (rv != BCM_E_NONE)
        {
            printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_set failed with fifo size set");
            return BCM_E_FAIL;
        }
    }

    for (i=0 ; i<5 ;i++)
    {
        for (j=0; j<2; j++)
        {
            if(fifo_types[i].is_fe13) 
            {
				if (i!=4) /* i=4 is only RX-RCI thresholds, which are unavailable for fe1 links only */
				{
                    rv = bcm_fabric_link_thresholds_pipe_set(unit, j, fifo_types[i].fabric_pipe, BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, fifo_types[i].fe1_threshold_values);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_set failed ");
                        return BCM_E_FAIL;
                    }
				}

                
                if (i!=3) /* i=3 is only RX-GCI thresholds, which are unavailable for fe3 links only */
                {
                    rv = bcm_fabric_link_thresholds_pipe_set(unit, j, fifo_types[i].fabric_pipe, BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, fifo_types[i].fe3_threshold_values);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_set failed ");
                        return BCM_E_FAIL;
                    }
                }
            }
            else /* fe2 */
            {
				if (i != 4) /* for fe2 , rx gci and rci are the same */
				{
					rv = bcm_fabric_link_thresholds_pipe_set(unit, j, fifo_types[i].fabric_pipe, 0, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, fifo_types[i].fe1_threshold_values);
					if(rv != BCM_E_NONE)
					{
						printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_set failed ");
						return BCM_E_FAIL;
					}
				}   
            }
        }                          
    }

    return BCM_E_NONE;
}

int cint_flow_control_fe3200_get_thresholds_and_compare(int unit, cint_fifo_type_test_s* fifo_types)
{
    int i, j, rv, k ;
    int threshold_values_configured_fe1[11];
    int threshold_values_configured_fe3[11];
    int threshold_values_configured_fifo_size[3];
    int fifo_size_threshold_values_default[3]= {120,124,128};
    bcm_fabric_link_threshold_type_t fifo_size_thresholds[3] = {bcmFabricLinkRxFifoSize, bcmFabricLinkMidFifoSize, bcmFabricLinkTxFifoSize} ;
    
    /* Retrieve fifo sizes and compare */
    for (i=0 ; i<2 ; i ++)
    {
        rv = bcm_fabric_link_thresholds_pipe_get(unit, i , bcmFabricPipeAll, 0, 3, fifo_size_thresholds, threshold_values_configured_fifo_size);
        if (rv != BCM_E_NONE)
        {
            printf("Error cint_flow_control_fe3200 - bcm_fabric_link_threholds_pipe get failed for fifo sizes");
            return BCM_E_FAIL;
        }
        for (j = 0; j < 3 ; j++)
        {
            if (fifo_size_threshold_values_default[j] != threshold_values_configured_fifo_size[j])
            {
                printf("Error cint_flow_control_fe3200 - configured threshold for fifo size is different from expected value");
                return BCM_E_FAIL;
            }
        }
    }


    /* Retrieve other thresholds and compare */
    for (i=0; i<5; i++)
    {
        for (j=0; j<2; j++)
        {
            if (fifo_types[i].is_fe13)
            {
				if (i != 4) /* i=4 is only RX-RCI thresholds, which are unavailable for fe1 links only */
				{
                    rv = bcm_fabric_link_thresholds_pipe_get(unit, j, fifo_types[i].fabric_pipe, BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, threshold_values_configured_fe1);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_get failed ");
                        return BCM_E_FAIL;
                    }
				}
                if (i != 3) /* i=3 is only RX-GCI thresholds, which are unavailable for fe3 links only */
                {
                    rv = bcm_fabric_link_thresholds_pipe_get(unit, j, fifo_types[i].fabric_pipe, BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, threshold_values_configured_fe3);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error cint_flow_control_fe3200 - bcm_fabric_link_thresholds_pipe_get failed ");
                        return BCM_E_FAIL;
                    }
                    
                }
                /* compare values*/
                for (k=0; k<fifo_types[i].nof_thresholds_configured ; k++)
                {
                    if ((i!=4) && (fifo_types[i].fe1_threshold_values[k] != threshold_values_configured_fe1[k]) )
                    {
                        printf("Error cint_flow_control_fe3200 - configured threshold  %d is different from expected value %d", threshold_values_configured_fe1[k], fifo_types[i].fe1_threshold_values[k]);
                        return BCM_E_FAIL;
                    }

                    if ((i!=3) && (fifo_types[i].fe3_threshold_values[k] != threshold_values_configured_fe3[k]) )
                    {
                        printf("Error cint_flow_control_fe3200 - configured threshold %d is different from expected value %d", threshold_values_configured_fe3[k], fifo_types[i].fe3_threshold_values[k]);
                        return BCM_E_FAIL;
                    }
                }
            }

            else /* fe2 */
            {
				if (i != 4) /* for fe2 , rx gci and rci are the same */
				{
					rv = bcm_fabric_link_thresholds_pipe_get(0, j, fifo_types[i].fabric_pipe, 0, fifo_types[i].nof_thresholds_configured, fifo_types[i].threshold_types, threshold_values_configured_fe1);

					for (k=0; k<fifo_types[i].nof_thresholds_configured; k++)
					{
						if (fifo_types[i].fe1_threshold_values[k] != threshold_values_configured_fe1[k])
						{
							printf("Error cint_flow_control_fe3200 - configured threshold %d is different from expected value %d", threshold_values_configured_fe1[k], fifo_types[i].fe1_threshold_values[k]);
							return BCM_E_FAIL;
						}
					}
				}
            }
        }
    }

    return BCM_E_NONE;
}


int cint_flow_control_fe3200_legacy_main(int unit,int is_fe13) {
    int rv ;
    cint_fifo_type_test_s fifo_types[5];

    cint_flow_control_fe3200_init_fifo_types(unit, fifo_types, is_fe13); 

    rv = cint_flow_control_fe3200_attach_links_to_fifos(unit); /* attach links to specific fifo type (0/1 )*/
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - attach_links_to_fifos failed\n");
        return BCM_E_FAIL;
    }

    rv = cint_flow_control_fe3200_retrieve_links_and_compare(unit); /* retrieve links per fifo type*/
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - retrieve_links_and_compare failed\n");
        return BCM_E_FAIL;
    }

    rv = cint_flow_control_fe3200_set_thresholds_to_fifos(unit, fifo_types); /* set thresholds */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - set_thrersholds_to_fifos failed \n");
        return BCM_E_FAIL;
    }

    rv = cint_flow_control_fe3200_get_thresholds_and_compare(unit, fifo_types); /* get thresholds and compare */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_flow_control_fe3200 - get_thresholds_and_comapre failed\n");
        return BCM_E_FAIL;

    }

    printf("cint_flow_control_fe3200: PASS\n");

    return BCM_E_NONE;
}

