/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  Create a bcm_field_group_t, then invoke field_api_setup(unit, prio, &group).
 *  Perform any frame testing desired.  Invoke field_api_teardown(unit, group).
 *
 *  Modify field_api_setup to fit your needs, or perhaps clone it several times
 *  for a number of different groups.  You can put more than one entry in each
 *  group, so each clone could set up several entries.
 */

int field_api_setup(/* in */ int unit,
                    /* in */ int group_priority,
                    /* out */ bcm_field_group_t *group) {
  int result;
  int auxRes;
  int index;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_range_t range[2];
  bcm_field_entry_t ent[3];
  bcm_field_stat_t stats[2];
  uint32 flags[2];
  uint32 minPort[2];
  uint32 maxPort[2];
  int statId[3];

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }
  grp = -1;
  for (index = 0; index < 3; index++) {
    ent[index] = -1;
    statId[index] = -1;
  }
  for (index = 0; index < 2; index++) {
    range[index] = -1;
    stats[index] = -1;
  }

  /*
   *  Build qualifier set for the group we want to create.  Entries must exist
   *  within groups, and groups define what kind of things the entries can
   *  match, by their inclusion in the group's qualitier set.
   *
   *  In this example, we will build an IPv4 group and two ranges: source port
   *  in 'system' range, and dest port in 'system' range.  Then, we will add
   *  four rules: one to accept UDP packets from system port to system port,
   *  and one to drop UDP packets from non-system to system port, and then
   *  a similar pair for TCP packets.  All four will have counters.
   *
   *  Also, we must specify stage ingress or egress.
   * 
   *  Note that only qualifiers specified in the QSET can be used by any of the
   *  entries in the group.  See the output from bcm_field_show(unit,"") for a
   *  list of the supported qualifiers per stage.
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp4);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);

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
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

  /*
   *  Create the group and set is actions.  This is two distinct API calls due
   *  to historical reasons.  On soc_petra, the action set call is obligatory, and
   *  a group must have at least one action in its action set before any entry
   *  can be created in the group.
   */
  result = bcm_field_group_create(unit, qset, group_priority, &grp);
  if (BCM_E_NONE == result) {
    result = bcm_field_group_action_set(unit, grp, aset);
  }

  /*
   *  Use a multi_range to see if packets trying to connect to mail service
   *  are coming from system ports.
   *
   *  Dune hardware checks against both TCP and UDP so we'll need to actually
   *  qualify that in the rest of the entry.  Also, when getting a TCP/UDP
   *  port range, both TCP and UDP flags will always be set on Soc_petraB & Arad,
   *  since this reflects the hardware more accurately.  They are only used
   *  in the code here this way as a mnemonic to make it clear that the intent
   *  is to only use this particular range against TCP packets.
   *
   *  Note that *both* sub-ranges must match in order for the range to match,
   *  and that the invert flags inverts the entire match result.  In this case
   *  the result table would look like this:
   *
   *    srcPort      dstPort      Match   Invert
   *    ----------   ----------   -----   ------
   *    0000         0000..FFFF   FALSE   TRUE
   *    0001..03FF   0000..0018   FALSE   TRUE
   *    0001..03FF   0019         TRUE    FALSE
   *    0001..03FF   001A..FFFF   FALSE   TRUE
   *    0400..FFFF   0000..FFFF   FALSE   TRUE
   *
   *  The INVERT flag is not supported on range create, but it IS supported
   *  when applying a range as a qualifier to an entry.
   */
  if (BCM_E_NONE == result) {
    flags[0] = BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_SRCPORT;
    minPort[0] = 0x0001;
    maxPort[0] = 0x03FF;
    flags[1] = BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_DSTPORT;
    minPort[1] = 0x0019; /* SMTP on port 25 */
    maxPort[1] = 0x0019; /* SMTP on port 25 */
    result = bcm_field_range_multi_create(unit,
                                          &(range[0]),
                                          0 /* overall flags */,
                                          2 /* number of sub-ranges */,
                                          &(flags[0]),
                                          &(minPort[0]),
                                          &(maxPort[0]));  
  }

  /*
   *  Use a multi_range to check whether connections to any system port are
   *  coming from any non-system port.
   *
   *    srcPort      dstPort      Match   Invert
   *    ----------   ----------   -----   ------
   *    0000..03FF   0000..FFFF   FALSE   TRUE
   *    0400..FFFF   0000..03FF   TRUE    FALSE
   *    0400..FFFF   0400..FFFF   FALSE   TRUE
   */
  if (BCM_E_NONE == result) {
    flags[0] = BCM_FIELD_RANGE_TCP |
               BCM_FIELD_RANGE_UDP |
               BCM_FIELD_RANGE_DSTPORT;
    minPort[0] = 0x0000;
    maxPort[0] = 0x03FF;
    flags[1] = BCM_FIELD_RANGE_TCP |
               BCM_FIELD_RANGE_UDP |
               BCM_FIELD_RANGE_SRCPORT;
    minPort[1] = 0x0400;
    maxPort[1] = 0xFFFF;
    result = bcm_field_range_multi_create(unit,
                                          &(range[1]),
                                          0 /* overall flags */,
                                          2 /* number of sub-ranges */,
                                          &(flags[0]),
                                          &(minPort[0]),
                                          &(maxPort[0]));
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
   *  This entry will count (and accept) system port to SMTP TCP packets.
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_entry_create(unit, grp, &(ent[0]));
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_qualify_IpProtocol(unit, ent[0], 0x06, 0xFF);
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_qualify_RangeCheck(unit, ent[0], range[0], FALSE);
  }
  /*
   *  This entry will count (and discard) non-system port to system port pkts.
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_entry_create(unit, grp, &(ent[1]));
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_qualify_RangeCheck(unit, ent[1], range[1], FALSE);
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_action_add(unit, ent[1], bcmFieldActionDrop, 0, 0);
  }
  /*
   *  This entry will count and accept packets to non-system ports and from
   *  sytstem ports.
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_entry_create(unit, grp, &(ent[2]));
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_qualify_RangeCheck(unit, ent[2], range[1], TRUE);
  }

  /*
   *  Allocate statistics sets and assign them to the entries
   */
  stats[0] = bcmFieldStatPackets;
  stats[1] = bcmFieldStatBytes;
  for (index = 0;
       (BCM_E_NONE == result) && (index < 3);
       index++) {
    result = bcm_field_stat_create(unit,
                                   grp,
                                   2 /* num stats */,
                                   &(stats[0]),
                                   &(statId[index]));
  }
  for (index = 0;
       (BCM_E_NONE == result) && (index < 2);
       index++) {
    if (statId[index] == statId[index + 1]) {
      printf("invalid condition: assigned identical stat IDs\n");
      result = BCM_E_INTERNAL;
    }
  }
  for (index = 0;
       (BCM_E_NONE == result) && (index < 3);
       index++) {
    result = bcm_field_action_add(unit,
                                  ent[index],
                                  bcmFieldActionStat,
                                  statId[index],
                                  0);
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but is is far more efficient to commit the whole group at once when
   *  making lots of changes.
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_group_install(unit, grp);
  }
  
  if (BCM_E_NONE == result) {
    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
    *group = grp;
  } else {
    /*
     *  Something went wrong; clean up
     */
    auxRes = bcm_field_show(unit, "Err: ");
    for (index = 0; index < 3; index++) {
      if (0 <= ent[2 - index]) {
        auxRes = bcm_field_entry_remove(unit, ent[2 - index]);
        auxRes = bcm_field_entry_destroy(unit, ent[2 -  index]);
      }
    }
    for (index = 0; index < 3; index++) {
      if (0 <= statId[2 - index]) {
        auxRes = bcm_field_stat_destroy(statId[2 - index]);
      }
    }
    for (index = 0; index < 2; index++) {
      if (0 <= range[1 - index]) {
        auxRes = bcm_field_range_destroy(unit, range[1 - index]);
      }
    }
    if (0 <= grp) {
      auxRes = bcm_field_group_destroy(unit, grp);
    }
  }
  return result;
}

/* Configure Multi Range with given ID */
int field_api_id_setup(/* in */ int unit,
                       /* in */ int group_priority,
                       /* in */ bcm_field_group_t group,
                       /* in */ uint32 range_id) {
  int result;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t entry;
  uint32 flags[2];
  uint32 minPort[2];
  uint32 maxPort[2];
  uint32 id = range_id;

  /* Create Qualifiers set (QSET) with all qualifiers supported for this field group
   * bcmFieldQualifyStageIngress qualifier needs to be included in QSET to identify ingress PMF
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);

  /* Create Action set (ASET) for all allowed actions for entries in the field group */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

  /* Create the field group SW entity with the given ID, priority and QSET */
  result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_group_create_id failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }

  /* Associate ASET with the field group */
  result = bcm_field_group_action_set(unit, group, aset);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_group_action_set failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_group_install failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }
  printf("\nInstalled field group %d\n", group);

  flags[0] = BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_UDP | BCM_FIELD_RANGE_SRCPORT;
  minPort[0] = 1000;
  maxPort[0] = 2000;
  flags[1] =  BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_UDP | BCM_FIELD_RANGE_DSTPORT;
  minPort[1] = 1000;
  maxPort[1] = 2000;
  result = bcm_field_range_multi_create(unit, &id, BCM_FIELD_RANGE_MULTI_WITH_ID, 2, &(flags[0]), &(minPort[0]), &(maxPort[0]));
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_range_multi_create failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }
  printf("\nCreated field multi range with ID=%d\n", id);

  result = bcm_field_entry_create(unit, group, &entry);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_entry_create failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }

  result = bcm_field_qualify_RangeCheck(unit, entry, id, FALSE);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_qualify_RangeCreate failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }

  /*
   *  This entry will discard pkts.
   */
  result = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_action_add failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }
  result = bcm_field_entry_install(unit, entry);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_entry_install failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }
  printf("\nInstalled field entry %d with entry for Drop of in-range packets\n", group);

  result = bcm_field_group_dump(unit, group);
  if (BCM_E_NONE != result) {
    printf("Error: bcm_field_group_dump failed with error %d (%s)", result, bcm_errmsg(result));
    return result;
  }

  return result;
}

int field_api_teardown(/* in */ int unit,
                       /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_entry_t ent[128];
  bcm_field_range_t range[128];
  bcm_field_range_t rangeTemp[32];
  int invertTemp[32];
  int entCount;
  int rngCount;
  int entIndex;
  int rngIndex;
  int offset;
  int statId;

  for (rngIndex = 0; rngIndex < 128; rngIndex++) {
    range[rngIndex] = -1;
  }
  do {
    /* get a bunch of entries in this group */
    result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
    if (BCM_E_NONE != result) {
      return result;
    }
    for (entIndex = 0; entIndex < entCount; entIndex++) {
      /* get statistics for this entry if any */
      result = bcm_field_entry_stat_get(unit, ent[entIndex], &statId);
      if (BCM_E_NONE == result) {
        /* found stat; get rid of it */
        result = bcm_field_entry_stat_detach(unit, ent[entIndex], statId);
        /*
         *  In this loop, we are assuming one stat for each entry.  The API
         *  supports sharing stats between entries (one stat can be used by
         *  mulitple entries) but not the other way (each entry can only
         *  use one stat).  It is possible that the API will return an error
         *  if there is an entry using a stat at the time we ask a stat to
         *  be destroyed.  This can be controlled at compile time on Soc_petra,
         *  but maybe not all devices.
         */
        if (BCM_E_NONE == result) {
          result = bcm_field_stat_destroy(unit, statId);
          if (BCM_E_FULL == result) {
            /* ignore failed attempt to destroy still-in-use stat */
            result = BCM_E_NONE;
          } else if (BCM_E_NONE != result) {
            return result;
          }
        }
      } else if (BCM_E_NOT_FOUND == result) {
        /* no stat, keep going */
        result = BCM_E_NONE;
      } else {
        return result;
      }
      /* get ranges used by this entry */
      result = bcm_field_qualify_RangeCheck_get(unit,
                                                ent[entIndex],
                                                32 /* max count */,
                                                &(rangeTemp[0]),
                                                &(invertTemp[0]),
                                                &rngCount);
      if (BCM_E_NONE == result) {
        for (rngIndex = 0; rngIndex < rngCount; rngIndex++) {
          /* need to keep track of all ranges used by these entries */
          for (offset = 0; offset < 128; offset++) {
            if (range[offset] == rangeTemp[rngIndex]) {
              /* already logged this one */
              break;
            } else if (-1 == range[offset]) {
              /* here is a new space, log this one */
              range[offset] = rangeTemp[rngIndex];
              break;
            } /* otherwise just keep looking */
          }
        }
      } else if (BCM_E_NOT_FOUND == result) {
        /* no ranges this entry */
        result = BCM_E_NONE;
      } else if (BCM_E_NONE != result) {
        /* something unexpeted happened */
        return result;
      }
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
  for (rngIndex = 0; rngIndex < 128; rngIndex++) {
    if (-1 != range[rngIndex]) {
      result = bcm_field_range_destroy(unit, range[rngIndex]);
      if (BCM_E_RESOURCE == result) {
        /* in use by something else, so leave it alone */
        result = BCM_E_NONE;
      } else if (BCM_E_NONE != result) {
        /* anything else but success is wrong, though */
        auxRes = result;
        /* but want to keep going since this is not recoverable state */
      }
    }
  }
  if (BCM_E_NONE != auxRes) {
    /* deferred error return from destroying ranges above */
    return result;
  }
  /* destroy the group */  
  result = bcm_field_group_destroy(unit, group);

  return result;
}


