/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_rx_trap_l2cp_exhaustive.c
 * PORTED FROM: cint_l2cp_traps_config.c
 *
 * Purpose: Random L2CP traps allocation together with basic L2 forwarding,
 *          for the packets that don't match on the L2CP traps.
 *
 * Helper functions:
 * cint_rx_trap_l2cp_exhaustive_l2_config(unit,vsi,in_port,out_port);
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_l2cp_exhaustive.c
 *
 * Main Function:
 *      cint_rx_trap_l2cp_exhaustive_main(unit,inP,outP,l2cp_trap_profile,step,vlan);
 * Destroy Function:
 *      cint_rx_trap_l2cp_exhaustive_destroy(unit,l2cp_trap_profile,step,vlan);
 *
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_l2cp_exhaustive.c
 * cint
 * cint_rx_trap_l2cp_exhaustive_main(0,200,202,1,3,100);
 */

int cint_rx_trap_l2cp_exhaustive_trap_id;

/**
* \brief
* Creates an L2 config to forward packets, which do not match the traps created.
*
* \par DIRECT INPUT:
*   \param [in] unit      - unit Id
*   \param [in] vsi       - VLAN/VSI
*   \param [in] in_port   - In Port
*   \param [in] out_port  - Out Port
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_exhaustive_l2_config(
    int unit,
    int vsi,
    int in_port,
    int out_port)
{
    int rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vsi, in_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, vsi);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vsi, out_port, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (rv) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, vsi);
        return rv;
    }


    return rv;
}


/**
* \brief
* Creates a UserDefined Ingress trap and sets a multiple L2CP Protocol traps
* with action drop.
*
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] inP                 - In Port
*   \param [in] l2cp_trap_profile   - L2CP Trap profile
*   \param [in] step                - Step incrementation for L2CP args
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_exhaustive_l2cp_trap(
   int unit,
   int inP,
   int l2cp_trap_profile,
   int step)
{
    int rv = BCM_E_NONE;
    int fwd_strength = 7, snp_strength = 0;
    int i;
    int nof = 0;
    bcm_gport_t trap_gport;
    bcm_rx_trap_protocol_key_t key ;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 0 };

    if (step == 0 || step > 63)
    {   printf("ERROR: step =%d\n",step);
        return ERR_E_PARAM;
    }

    rv = cint_utils_rx_trap_ingress_create_and_set_to_drop(unit, 0, bcmRxTrapUserDefine,0, &cint_rx_trap_l2cp_exhaustive_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_utils_rx_trap_create_and_set\n");
        return rv;
    }

    protocol_profiles.l2cp_trap_profile = l2cp_trap_profile;

    rv = bcm_rx_trap_protocol_profiles_set(unit, inP, &protocol_profiles);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_protocol_profiles_set\n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, cint_rx_trap_l2cp_exhaustive_trap_id, fwd_strength, snp_strength);



    key.protocol_type = bcmRxTrapL2cpPeer;
    key.protocol_trap_profile = l2cp_trap_profile;

    for (i = 0; i < BCM_RX_TRAP_L2CP_NOF_ENTRIES; i += step)
    {
        key.trap_args = i;
        nof++;

        rv = bcm_rx_trap_protocol_set(unit, &key, trap_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_set\n");
            return rv;
        }
    }

    return rv;
}


/**
* \brief
* Main function of the cint which calls the L2 set-up and then the L2CP trap creation,
* according to the 'step' param..
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] inP             - In Port
*   \param [in] outP            - Out Port
*   \param [in] l2cp_profile    - L2CP Trap profile
*   \param [in] step            - L2CP trap args stepping
*   \param [in] vlan            - VLAN
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_exhaustive_main(
    int unit,
    int inP,
    int outP,
    int l2cp_profile,
    int step,
    int vlan)
{
    int rv = BCM_E_NONE;

    rv = cint_rx_trap_l2cp_exhaustive_l2_config(unit, vlan, inP, outP);
    if(rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_l2cp_exhaustive_l2_config\n");
        return rv;
    }

    rv = cint_rx_trap_l2cp_exhaustive_l2cp_trap(unit, inP, l2cp_profile, step);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_l2cp_exhaustive_l2cp_trap\n");
        return rv;
    }


    return rv;
}


/**
* \brief
* Destroy function of the cint which clears the L2CP traps created by the supplied 'step'.
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] l2cp_profile    - L2CP Trap profile
*   \param [in] step            - L2CP trap args stepping
*   \param [in] vlan            - VLAN to be deleted
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_exhaustive_destroy(
    int unit,
    int l2cp_profile,
    int step,
    int vlan)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_rx_trap_protocol_key_t key;

    rv = bcm_vlan_destroy(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_destroy\n");
        return rv;
    }

    rv = bcm_multicast_destroy(unit, vlan);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_multicast_delete\n");
       return rv;
    }

    key.protocol_type = bcmRxTrapL2cpPeer;
    key.protocol_trap_profile = l2cp_profile;

    for (i = 0; i < BCM_RX_TRAP_L2CP_NOF_ENTRIES; i += step)
    {
        key.trap_args = i;
        rv = bcm_rx_trap_protocol_clear(unit,&key);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_clear\n");
            return rv;
        }
    }
    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_l2cp_exhaustive_trap_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_type_destroy\n");
       return rv;
    }

    return rv;



}


