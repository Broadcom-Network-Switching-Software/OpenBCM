/* $Id: cint_dnx_util_srv6.c,v 1.00
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides srv6 utils functionality
 */

/** \brief Max number of supported SIDs to be terminated in Extended Termination flow (using RCY) */
int MAX_EXTENDED_TERMINATED_NOF_SIDS = 10;
int MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED = 14;

/** \brief Structure to convert Last Element (NOF SIDs - 1) in an SRv6 packet to required termination sizes in:
 *   - as part of RCH 2nd Pass extension size
 *   - 1st Pass Egress Termination (in addition to layer start offset) */
struct cint_srv6_last_element_to_termination_sizes_s
{
    int recycle_header_extension_lengths[MAX_EXTENDED_TERMINATED_NOF_SIDS];    /* Size of extension header after recycle header in bytes */
    int additional_egress_termination_sizes[MAX_EXTENDED_TERMINATED_NOF_SIDS]; /* Size of egress additional termination in bytes (to parsing start offset value) */
    int recycle_header_extension_lengths_extended[MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED];    /* Size of extension header after recycle header in bytes for devices supporting 1 and 2 pass USP*/
    int additional_egress_termination_sizes_extended[MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED]; /* Size of egress additional termination in bytes (to parsing start offset value) for devices supporting 1 and 2 pass USP*/
};

cint_srv6_last_element_to_termination_sizes_s cint_srv6_last_element_to_termination_sizes =
{
         /*
          * Size of extension header after recycle header in bytes
          */
         { 0, 0, 0, 0, 0, 4, 20, 36, 52, 68},
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         {  24, 40, 56, 72, 88, 100, 100, 36, 36, 36},
         /*
          * Size of extension header after recycle header in bytes
          */
         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 28, 44 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         {  0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 164, 164, 164 }
};


/*
 * Create a Recycle Entry.
 * unit - Relevant unit
 * nof_sids - Number of SIDs to use for the entry
 *           (this is will deduct the needed RCH extension size and egress additional termination)
 * recycle_entry_encap_id - Returned encap id.
 */
int srv6_create_extended_termination_recycle_entry(
    int unit,
    int nof_sids,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;
    int terminated_sids_usp_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp_1pass", NULL));
    int ingress_nwk_header_term_enable = *(dnxc_data_get (unit, "dev_init", "general", "network_header_termination", NULL));
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    /** devices with usp 1 pass support, have extended capabilities*/
    if (terminated_sids_usp_1pass)
    {
        recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_extended[nof_sids];
        recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_extended[nof_sids];
    }
    else
    {
        if(ingress_nwk_header_term_enable)
        {
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths[nof_sids]
                                                               + cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes[nof_sids];
        }
        else
        {
            recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths[nof_sids];
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes[nof_sids];
        }
    }
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    *recycle_entry_encap_id = recycle_entry.encap_id;

    return rv;
}
