/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Field Processor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_fid_demo.c
 * Purpose: Example for utilizing the field APIs. 
 *
 * You can modify the script to do the setup and tear down for various qualifiers and actions. 
 * The example illustrates how to install a rule to drop only Ethernet packets only if they fit 
 * a specific MAC source address and a specific EtherType.
 *
 * CINT usage:
 *  Create a bcm_field_group_t, then invoke field_api_setup(unit, prio, &group).
 *  Perform any frame testing desired.  
 *  Invoke field_api_teardown(unit, group).
 *
 *  Modify field_api_setup to fit your needs, or perhaps clone it several times
 *  for a number of different groups (but ensure that only one does the call to bcm_field_init).
 *  You can put more than one entry in each group, so each clone can set up several entries.
 */

int field_api_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* in */ bcm_field_group_t group) {
    int result;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_field_stat_t stats[2];
    int statId;

    
    /*
     *  Build qualifier set for the group we want to create.  Entries must exist
     *  within groups, and groups define what kind of things the entries can
     *  match, by their inclusion in the group's qualitier set.
     *
     *  In our little example, we expect to do L2 matching, so we include a few
     *  fields of interest for L2 work.  Right now, the field APIs do not separate
     *  frames according to their type, so if we wanted to do IPv4, we would
     *  still need to include, for example, EtherType (and set it to 0x0800 with
     *  all bits considered) in order to ensure only IPv4 frames hit.  We are not
     *  specifically required to include *all* L2 (or whatever) fields; in fact we
     *  may get better use of the TCAM space if we do not include qualifiers that
     *  we do not intend to use.
     *
     *  Also, we must specify stage ingress or egress.
     * 
     *  Note that only qualifiers specified in the QSET can be used by any of the
     *  entries in the group.  See the output from bcm_field_show(unit,"") for a
     *  list of the supported qualifiers per stage.
     */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    
    /*
     *  Build action set for the group we want to create.  Entries can only do
     *  something that is included on this list.
     *
     *  Note: some actions overlap at the hardware layer (various flavours of
     *  redirect, for example).  While more than one such action can be included
     *  in a group's action set, a single entry can only specify one such action.
     *  Otherwise, a single entry can have multiple actions.
     *
     *  See the output from bcm_field_show(unit,"") for a list of the supported
     *  actions per stage.
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirectPort);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    
    /*
     *  Create the group and set is actions.  This is two distinct API calls due
     *  to historical reasons.  On soc_petra, the action set call is obligatory, and
     *  a group must have at least one action in its action set before any entry
     *  can be created in the group.
     */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
    if (BCM_E_NONE != result) {
        return result;
    }
    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    /*
     *  Add an entry to the group (could add many entries).
     *
     *  Relatively basic example here, only drop and count.  We could also take
     *  other actions (list them in the aset above).  Some actions only take the
     *  BCM canonical form of param0,param1; other actions (such as redirect and
     *  mirror) can take a GPORT as the port argument; such GPORT can be any
     *  valid GPORT in the given context (redirect family of actions the GPORT
     *  can be anything that works out to be a destination type fwd action, same
     *  for mirror).
     *
     *  This example will match Novell NetWare IPX frames sent using the
     *  Ethernet II frame format by certain Broadcom NICs, and count + drop them.
     */
    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    /* certain Broadcom NICs */
    macData[0] = 0x00;
    macData[1] = 0x1A;
    macData[2] = 0xA0;
    macData[3] = 0x00;
    macData[4] = 0x00;
    macData[5] = 0x00;
    macMask[0] = 0xFF;
    macMask[1] = 0xFF;
    macMask[2] = 0xFF;
    macMask[3] = 0x00;
    macMask[4] = 0x00;
    macMask[5] = 0x00;
    result = bcm_field_qualify_SrcMac(unit, ent, macData, macMask);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    /* NetWare IPX over Ethernet_II */
    result = bcm_field_qualify_EtherType(unit, ent, 0x8137, 0xFFFF);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    /*
     *  Once we match the frames, let's drop them.
     *
     *  note that param0 and param1 don't care for bcmFieldActionDrop
     */
    result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    /*
     *  Let's count the frames that hit the entry
     *
     *  We could be extravagant here and ask for all of the stats supported,
     *  but that would only work if the counter processor is configured to
     *  the full mode (g,y,r drop; g,y forward).  Instead, we only ask for
     *  bytes and packets, which should be available in any counter proc mode
     *  that supports the field processor using counters.
     *
     *  Unlike some devices, the statistics ID is an opaque type that includes
     *  information about the counter processor and set; we can't treat it as
     *  a simple number, and the APIs don't want such mistreatment.
     */
    stats[0] = bcmFieldStatPackets;
    stats[1] = bcmFieldStatBytes;
    result = bcm_field_stat_create(unit, grp, 2 /* num stats */, &(stats[0]), &statId);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    /* Could use bcmFieldActionStat on Soc_petra, but maybe not everywhere */
    result = bcm_field_entry_stat_attach(unit, ent, statId);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_stat_destroy(unit, statId);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    /*
     *  Commit the entire group to hardware.  We could do individual entries,
     *  but since we just created the group here and want the entry/entries all
     *  in hardware at this point, it's a bit quicker to commit the whole group.
     */
    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_stat_destroy(unit, statId);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }
    
    return result;
}

int field_api_teardown(/* in */ int unit,
                       /* in */ bcm_field_group_t group) {
    int result;
    int auxRes;
    bcm_field_entry_t ent[128];
    int entCount;
    int entIndex;
    
    do {
        /* get a bunch of entries in this group */
        result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
        if (BCM_E_NONE != result) {
            return result;
        }
        for (entIndex = 0; entIndex < entCount; entIndex++) {
            /* remove each entry from hardware and destroy it */
            result = bcm_field_entry_remove(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
            result = bcm_field_entry_destroy(unit, ent[entIndex]);
            if (BCM_E_NONE != result) {
                return result;
            }
        }
    /* all as long as there were entries to remove & destroy */
    } while (entCount > 0);
    
    /* destroy the group */  
    result = bcm_field_group_destroy(unit, group);
    
    return result;
}
