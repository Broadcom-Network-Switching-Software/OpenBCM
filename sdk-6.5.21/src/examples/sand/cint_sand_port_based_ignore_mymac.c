/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Port based Ignore MyMac~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_port_based_ignore_mymac.c
 * Purpose: Demo Port based Ignore Mymac L2 termination
 *
 *  For JR1, this is achieved by TCAM at VT stage
 *  For JR2, this is achieved by dedicated SEXEM DB
 */

 struct cint_port_based_ignore_mymac_info_s 
{
    bcm_field_group_t group_id;         /*TCAM DB group ID, for JR1*/
    bcm_field_entry_t entry[512];       /*per port TCAM DB entry ID, for JR1*/


    int station_id[512];                /*per port l2 station id for JR2*/
};

cint_port_based_ignore_mymac_info_s cint_port_based_ignore_mymac_info;


/**
 * JR1 tcam DB init
 * INPUt:
 * unit   - device unit
*/
int port_based_ingore_mymac_tcam_db_create(int unit)
{
    int j;
    int res;
    bcm_field_group_config_t   db;


    /* Create group */
    bcm_field_group_config_t_init(&db);

    db.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET;
    db.priority   = BCM_FIELD_GROUP_PRIO_ANY;

    /* Create Qualifiers */
    BCM_FIELD_QSET_INIT(db.qset);
    BCM_FIELD_QSET_ADD(db.qset, bcmFieldQualifyStageIngressVlanTranslation);

    BCM_FIELD_QSET_ADD(db.qset, bcmFieldQualifyOuterVlanId);
    
    BCM_FIELD_QSET_ADD(db.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(db.qset, bcmFieldQualifyDstMac);
    
    BCM_FIELD_ASET_INIT(db.aset);
    BCM_FIELD_ASET_ADD(db.aset, bcmFieldActionIngressGportSet);

    res = bcm_field_group_config_create(unit, &db);
    if (res != BCM_E_NONE)
    {
        printf("bcm_field_group_config_create() returned %s(%d) \n", bcm_errmsg(res), res);
        return res;
    }

    cint_port_based_ignore_mymac_info.group_id = db.group;

    return 0;
}


/**
 * JR1 tcam DB entry add
 * INPUt:
 * unit   - device unit
 * port   - port
*/
int port_based_ingore_mymac_tcam_entry_add(int unit,bcm_port_t port)
{
    int rv;
    uint32 full_mask =0xffffffff;
    bcm_field_entry_t entry;


    rv = bcm_field_entry_create(unit, cint_port_based_ignore_mymac_info.group_id, &entry);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_entry_create\n");
            return rv;
    }

    /* vlan should be in key, but not used for the lookup, do not change the 0x0 value */
    rv = bcm_field_qualify_OuterVlanId(unit, entry, 0x0, 0x0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_OuterVlan\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, entry, port ,full_mask);
    if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_qualify_SrcPort\n");
        return rv;
    }


    rv = bcm_field_entry_install(unit, entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }

    cint_port_based_ignore_mymac_info.entry[port]= entry;

    return rv;
}


/**
 * JR1 tcam DB entry delete
 * INPUt:
 * unit   - device unit
 * port   - port
*/
int port_based_ingore_mymac_tcam_entry_delete(int unit,bcm_port_t port)
{
    int rv;
    bcm_field_entry_t entry;

    entry  =  cint_port_based_ignore_mymac_info.entry[port];
    rv = bcm_field_entry_destroy(unit, entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }
    cint_port_based_ignore_mymac_info.entry[port] = -1;

    return rv;
}

/**
 * Global configuration Init
 * INPUt:
 * unit   - device unit
*/
int port_based_ingore_mymac_global_init(int unit)
{
    int rv;
    int i;

    for(i = 0; i < 512;i++ )
    {
        cint_port_based_ignore_mymac_info.entry[i] = -1;
        cint_port_based_ignore_mymac_info.station_id[i]= -1;
    }


    if (is_device_or_above(unit, JERICHO2)) 
    {
        rv = bcm_switch_l3_protocol_group_set(unit,BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS,2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }

        rv = bcm_switch_control_set(unit,bcmSwitchL2StationExtendedMode,2);     
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }
    
    }
    else
    {
        rv = port_based_ingore_mymac_tcam_db_create(unit);     
        if (BCM_E_NONE != rv) {
            printf("Error in port_based_ingore_mymac_tcam_db_create\n");
            return rv;
        }
    }

    return rv;

}

/**
 * Global configuration Deinit
 * INPUt:
 * unit   - device unit
*/
int port_based_ingore_mymac_global_deinit(int unit)
{
    int rv;
    int i;


    if (is_device_or_above(unit, JERICHO2)) 
    {
        for(i = 0; i < 512;i++ )
        {
            if (cint_port_based_ignore_mymac_info.station_id[i] != -1)
            {
                rv = bcm_dnx_l2_station_delete(unit,cint_port_based_ignore_mymac_info.station_id[i]);
                if (BCM_E_NONE != rv) {
                    printf("Error in bcm_dnx_l2_station_delete\n");
                    return rv;
                }

            }
        }
        rv = bcm_switch_control_set(unit,bcmSwitchL2StationExtendedMode,0);     
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }

        rv = bcm_switch_l3_protocol_group_set(unit,0,2);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }
    
    }
    else
    {
        for(i = 0; i < 512;i++ )
        {
            if (cint_port_based_ignore_mymac_info.entry[i] != -1)
            {
                rv = bcm_field_entry_destroy(unit,cint_port_based_ignore_mymac_info.entry[i]);
                if (BCM_E_NONE != rv) {
                    printf("Error in bcm_field_entry_destroy\n");
                    return rv;
                }

            }
        }
        rv = bcm_field_group_destroy(unit,cint_port_based_ignore_mymac_info.group_id);     
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_group_destroy\n");
            return rv;
        }
    }

    return rv;
}

/**
 * Port Ingore Mymac enabel/disable
 * INPUt:
 * unit   - device unit
 * enable - enable/disable
*/
int port_based_ingore_mymac_port_set(int unit,int port, int enable)
{
    int rv;

    if (is_device_or_above(unit, JERICHO2)) 
    {
        if (enable)
        {
            bcm_l2_station_t station;
            int station_id;

            bcm_l2_station_t_init(&station);
            
            station.src_port=port;
            station.src_port_mask=0xfff;
            station.flags = BCM_L2_STATION_MPLS|BCM_L2_STATION_EXTENDED;
            rv = bcm_l2_station_add(unit,&station_id,&station); 
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_l2_station_add\n");
                return rv;
            }
            cint_port_based_ignore_mymac_info.station_id[port] = station_id;

        }
        else
        {
            rv = bcm_l2_station_delete(unit,cint_port_based_ignore_mymac_info.station_id[port]); 
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_l2_station_delete\n");
                return rv;
            }
            cint_port_based_ignore_mymac_info.station_id[port] = -1;

        }
    }
    else
    {
        if (enable)
        {
            rv = port_based_ingore_mymac_tcam_entry_add(unit,port);     
            if (BCM_E_NONE != rv) {
                printf("Error in port_based_ingore_mymac_tcam_entry_add\n");
                return rv;
            }
        }
        else
        {
            rv = port_based_ingore_mymac_tcam_entry_delete(unit,port);     
            if (BCM_E_NONE != rv) {
                printf("Error in port_based_ingore_mymac_tcam_entry_delete\n");
                return rv;
            }


        }

    }


    return rv;

}


