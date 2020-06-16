/** file cint_mirror_on_drop.c
 *  Used as an application reference to users on how to configure mirror-on-drop mechanism
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

int mirror_dest_id = 1;
int group_id = 0;
int sysport_id = 2000;

/**
 * \brief - Create sniff (mirror) profile designated for mirror-on-drop. The destination should be the voq pointing to dst_port
 */
int
cint_mirror_on_drop_sniff_profile_create(int unit, bcm_port_t dst_port)
{
    int voq_id;
    bcm_mirror_destination_t mirror_dest;
    bcm_mirror_header_info_t mirror_header_info;
    int rv;

    sal_memset(&mirror_dest, 0, sizeof(mirror_dest));

    /** get VOQ ID - assuming DNX reference application */
    rv = appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, 0, dst_port, &voq_id);
    if (BCM_FAILURE(rv))
    {
        printf("appl_dnx_e2e_scheme_logical_port_to_base_voq_get failed \n");
        return rv;
    }

    printf("Creating sniff profile \n");

    /** create sniff profile */
    mirror_dest.flags2 |= BCM_MIRROR_DEST_FLAGS2_MIRROR_ON_DROP;
    mirror_dest.flags |= BCM_MIRROR_DEST_WITH_ID;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(mirror_dest.gport, voq_id);
    BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, mirror_dest_id);
    rv = bcm_mirror_destination_create(unit, mirror_dest);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_mirror_destination_create failed \n");
        return BCM_E_FAIL;
    }

    printf("Adding application extension to sniff profile \n");

    /** set system headers including mirror-on-drop application extension */

    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeMirrorOnDrop;
    mirror_header_info.tm.ase_ext.valid = TRUE;
    rv = bcm_mirror_header_info_set(unit, 0, mirror_dest.mirror_dest_id, &mirror_header_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_mirror_header_info_set failed \n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


/**
 * \brief - Set drop group to report on invalid destination
 */
int
cint_mirror_on_drop_group_set(int unit)
{
    bcm_cosq_mirror_on_drop_group_info_t group_info;
    int rv;

    printf("Setting drop group\n");

    sal_memset(&group_info, 0, sizeof(group_info));

    group_info.drop_reasons_count = 1;
    group_info.reasons_array[0] = bcmCosqDropReasonQueueNumNotValidReject;
    group_info.type = bcmCosqMirrorOnDropTypeGlobal;
    rv = bcm_cosq_mirror_on_drop_group_set(unit, 0 , group_id, group_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_cosq_mirror_on_drop_group_set failed \n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Set drop group to report on invalid destination
 */
int
cint_mirror_on_drop_init(int unit, bcm_port_t src_port, bcm_port_t mirror_dst_port)
{
    int rv;
    bcm_gport_t sysport_gport;

    printf("Starting mirror on drop example \n");

    /** setup forwarding to invalid destination, assuming 2000 is invalid system port */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport_id);
    rv = bcm_port_force_forward_set(unit, src_port, sysport_gport, 1);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_force_forward_set failed \n");
        return BCM_E_FAIL;
    }

    /** create sniff profile */
    rv = cint_mirror_on_drop_sniff_profile_create(unit, mirror_dst_port);
    if (BCM_FAILURE(rv))
    {
        printf("cint_mirror_on_drop_sniff_profile_create failed \n");
        return BCM_E_FAIL;
    }

    /** set drop group */
    rv = cint_mirror_on_drop_group_set(unit);
    if (BCM_FAILURE(rv))
    {
        printf("cint_mirror_on_drop_group_set failed \n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}



/**
 * \brief - Set drop group to report on invalid destination
 */
int
cint_mirror_on_drop_clear(int unit, bcm_port_t src_port)
{
    int rv;
    bcm_gport_t mirror_gport;
    bcm_gport_t sysport_gport;
    bcm_cosq_mirror_on_drop_group_info_t group_info;

    /** disable forwarding */
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sysport_id);
    rv = bcm_port_force_forward_set(unit, src_port, sysport_gport, 0);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_force_forward_set failed \n");
        return BCM_E_FAIL;
    }

    /** destroy sniff profile */
    BCM_GPORT_MIRROR_SET(mirror_gport, mirror_dest_id);
    rv = bcm_mirror_destination_destroy(unit, mirror_gport);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_mirror_destination_destroy failed \n");
        return BCM_E_FAIL;
    }

    /** clear drop group */
    sal_memset(&group_info, 0, sizeof(group_info));
    rv = bcm_cosq_mirror_on_drop_group_set(unit, 0 , group_id, group_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_cosq_mirror_on_drop_group_set failed \n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


