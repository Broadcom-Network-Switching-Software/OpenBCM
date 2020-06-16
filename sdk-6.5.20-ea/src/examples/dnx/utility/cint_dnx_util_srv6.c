/* $Id: cint_dnx_util_srv6.c,v 1.00
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides srv6 utils functionality
 */

/** \brief Max number of supported SIDs to be terminated in Extended Termination flow (using RCY) */
int MAX_EXTENDED_TERMINATED_NOF_SIDS = 9;

/** \brief Structure to convert Last Element (NOF SIDs - 1) in an SRv6 packet to required termination sizes in:
 *   - as part of RCH 2nd Pass extensio size
 *   - 1st Pass Egress Termination (in addition to layer start offset) */
struct cint_srv6_last_element_to_termination_sizes_s
{
    int recycle_header_extension_lengths[MAX_EXTENDED_TERMINATED_NOF_SIDS];    /* Size of extension header after recycle header in bytes */
    int additional_egress_termination_sizes[MAX_EXTENDED_TERMINATED_NOF_SIDS]; /* Size of egress additional termination in bytes (to parsing start offset value) */
};

cint_srv6_last_element_to_termination_sizes_s cint_srv6_last_element_to_termination_sizes =
{
         /*
          * Size of extension header after recycle header in bytes
          */
         { 0, 0, 0, 0, 0, 4, 20, 36, 52 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         {  24, 40, 56, 72, 88, 100, 100, 36, 36 }
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
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths[nof_sids-1];
    recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes[nof_sids-1];
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
