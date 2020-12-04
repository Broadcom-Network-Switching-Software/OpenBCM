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
/*
 * Set a global to contain initialized vport ranges.
 * Use global because of limitation of TCL in passing pointers to CINT.
 * See 
 */
bcm_field_range_t range_vport[3];

/*
Function to destroy the configured vport ranges
*/
int field_range_vports_destroy(int unit) {
    int rv = BCM_E_NONE;
    char *proc_name;
    int num_ranges, index, rv_final ;

    rv_final = BCM_E_NONE ;
    proc_name = "field_range_vports_destroy";
    num_ranges = sizeof(range_vport) / sizeof(range_vport[0]) ;
    /* destroy ranges */
    for (index = 0 ; index < num_ranges ; index++) {
      if (range_vport[index] > 0) {
        rv = bcm_field_range_destroy(unit, range_vport[index]);
        if (rv != BCM_E_NONE) {
          printf("%s(): Error in : bcm_field_range_destroy(), returned %d\n", proc_name, rv);
          rv_final = rv ;
        } else {
          printf("%s(): Done. Destroyed vport range %d.\n", proc_name, range_vport[index]);
        }
      }
    }
    return rv;
}

int Presel_id = 0;

int field_api_setup_range_vport(
        /* in */ int unit,
        /* in */ int group_priority,
        /* in */ bcm_field_group_t group,
        /* in */ int base_outlif,
        /* in */ unsigned int range_to_select)
{
  int result;
  int auxRes;
  int index;
  bcm_field_presel_set_t psset;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent[3];
  bcm_field_entry_t presel_flags = 0;
  uint32 flags[1];
  uint32 minPort[1];
  uint32 maxPort[1];
  int full_show ;
  int out_lif_vals[4] ;
  int reference ;
  int val ;
  int step ;
  unsigned int num_ranges ;

  char *proc_name ;
  proc_name = "field_api_setup_range_vport" ;
  /*
   * This is only useful and meaningful when soc property FIELD_PRESEL_MGMT_ADVANCED_MODE
   * is set. In that case, 'presel_id' is set by the caller (as WITH_ID) and, therefore,
   * it must change every invocation (since the ones already selected are stored on
   * sw state).
   */
  Presel_id++;
  presel_flags = 0;
  /*
   * Set to '1' if full dump of 'field' is required when error is detected.
   */
  full_show = 0 ;
  grp = -1;
  result = BCM_E_NONE ;
  /*
   * Create four values for selected ranges starting -150 below inpot outlif
   * and incrementing by 100.
   */
  reference = 150 ;
  step = 100 ;
  val = reference ;
  num_ranges = sizeof(range_vport) / sizeof(range_vport[0]) ;
  if (range_to_select >= num_ranges)
  {
    printf("%s(): range_to_select (%u) is too large (Should be smaller than %d). Quit.\r\n",proc_name,range_to_select, num_ranges);
    result = BCM_E_PARAM ;
  }
  if (base_outlif < reference)
  {
    printf("%s(): Base_outlif (%d) is too small (Should be larger than %d). Quit.\r\n",proc_name,base_outlif, reference);
    result = BCM_E_PARAM ;
  }
  out_lif_vals[0] = base_outlif - reference ;
  for (index = 1; index < 4 ; index++) {
    out_lif_vals[index] = out_lif_vals[index - 1] + step ;
  } 
  for (index = 0; index < 3 ; index++) {
    ent[index] = -1;
  }
  for (index = 0; index < num_ranges ; index++) {
    range_vport[index] = -1;
  }
  /*
   *  Build qualifier set for the group we want to create.  Entries must exist
   *  within groups, and groups define what kind of things the entries can
   *  match, by their inclusion in the group's qualitier set.
   *
   *  In this example, we will build an Ingress group and three OUT_LIF ranges.
   *  Then, we shall add one rule: Drop all packets if specified program is
   *  selected.
   * 
   *  Note that qualifier bcmFieldQualifySrcMac is added to QSET just because
   *  at least one qualifier is required in addition to the stage.
   */
  /*
   * QUALIFIERS
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);

  /*
   *  Build action set for the group we want to create.  Entries can only do
   *  something that is included on this list.
   *
   *  Note that only one action is added: Drop. This means that once a packet is
   * directed to this program (containing only one group) then it is dropped.
   */
  /*
   * ACTIONS
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
  /*
   * RANGES
   */
  /*
   *  Set up three Vport (out_lif) ranges: 
   *  Out_lif range smaller than indicated input
   *  Out_lif range conytaining indicated input
   *  Out_lif range larger than indicated input
   */
  index = 0 ;
  if (BCM_E_NONE == result) {
    flags[0] = BCM_FIELD_RANGE_OUT_VPORT ;
    minPort[0] = out_lif_vals[index] ;
    index++ ;
    maxPort[0] = out_lif_vals[index] ;
    result = bcm_field_range_multi_create(unit,
                                          &(range_vport[0]),
                                          0 /* overall flags */,
                                          1 /* number of sub-ranges */,
                                          &(flags[0]),
                                          &(minPort[0]),
                                          &(maxPort[0]));
    printf("%s(): Just created range %d for OUT_VPORTs %d to %d\r\n",proc_name,range_vport[0],minPort[0],maxPort[0]);
  }
  if (BCM_E_NONE == result) {
    flags[0] = BCM_FIELD_RANGE_OUT_VPORT ;
    minPort[0] = out_lif_vals[index] ;
    index++ ;
    maxPort[0] = out_lif_vals[index] ;
    result = bcm_field_range_multi_create(unit,
                                          &(range_vport[1]),
                                          0 /* overall flags */,
                                          1 /* number of sub-ranges */,
                                          &(flags[0]),
                                          &(minPort[0]),
                                          &(maxPort[0]));
    printf("%s(): Just created range %d for OUT_VPORTs %d to %d\r\n",proc_name,range_vport[1],minPort[0],maxPort[0]);
  }
  if (BCM_E_NONE == result) {
    flags[0] = BCM_FIELD_RANGE_OUT_VPORT;
    minPort[0] = out_lif_vals[index] ;
    index++ ;
    maxPort[0] = out_lif_vals[index] ;
    result = bcm_field_range_multi_create(unit,
                                          &(range_vport[2]),
                                          0 /* overall flags */,
                                          1 /* number of sub-ranges */,
                                          &(flags[0]),
                                          &(minPort[0]),
                                          &(maxPort[0]));
    printf("%s(): Just created range %d for OUT_VPORTs %d to %d\r\n",proc_name,range_vport[2],minPort[0],maxPort[0]);
  }

  /*
   *  PRESELECTORS
   */
  /*
   * Configure the presel
   */
  presel_flags = BCM_FIELD_QUALIFY_PRESEL;
  if (BCM_E_NONE == result) {
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, Presel_id);
    } else {
        result = bcm_field_presel_create(unit, &Presel_id);
    }
    if (BCM_E_NONE != result) {
      printf("%s(): Error %d in bcm_field_presel_create\r\n",proc_name,result);
    }
  }
  printf("%s(): Presel_id %d presel_flags 0x%x\r\n",proc_name,Presel_id,presel_flags);
  if (BCM_E_NONE == result) {
    result = bcm_field_qualify_Stage(unit, Presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
    printf("%s(): Error %d in bcm_field_qualify_Stage\r\n",proc_name,result);
    }
  }
  if (BCM_E_NONE == result) {
    if (range_to_select == 1)
    {
        printf("%s(): Selected range index %d insures ALL PACKETS WILL BE DROPPED\r\n",proc_name,range_to_select);
    }
    else
    {
        printf("%s(): Selected range index %d insures ALL PACKETS WILL PASS THROUGH\r\n",proc_name,range_to_select);
    }
    result = bcm_field_qualify_VPortRangeCheck(unit, (Presel_id | presel_flags) & ~BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS, range_vport[range_to_select], FALSE);
    if (BCM_E_NONE != result) {
      printf("%s(): Error %d in bcm_field_qualify_VPortRangeCheck\r\n",proc_name,result);
    }
    if (BCM_E_NONE == result) {
      int invert ;
      int count ;
      unsigned int range[3] ;
      result = bcm_field_qualify_VPortRangeCheck_get(unit, Presel_id | BCM_FIELD_QUALIFY_PRESEL, 3, &range[0], &invert, &count);
      if (BCM_E_NONE != result) {
        printf("%s(): Result of bcm_field_qualify_VPortRangeCheck_get() %d. range %d invert %d count %d\r\n",proc_name,result,range[range_to_select], invert, count);
      }
    }
  }
  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, Presel_id);
  /*
   *  Create the group and set its preselector and actions. 
   */
  grp = group;
  if (BCM_E_NONE == result) {
    result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
      printf("%s(): Error %d in bcm_field_group_presel_set\r\n",proc_name,result);
    }
  }
  if (BCM_E_NONE == result) {
    result = bcm_field_group_action_set(unit, grp, aset);
  }
  /*
   * CREATE ONE ENTRY and add to group (to drop all packates)
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_entry_create(unit, grp, &(ent[0]));
    printf("%s(): Just created entry %d for small OUT_VPORT. Result %d\r\n",proc_name,ent[0],result);
  }
  /*
   * Drop the packet (no qualifier)
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_action_add(unit, ent[0], bcmFieldActionDrop, 0, 0);
    printf("%s(): Result %d iforn bcm_field_action_add()\r\n",proc_name,result);
  }
  if (BCM_E_NONE != result) {
    /*
     * Set priority for the entry - this entry must be lowest priority to act as default action
     */
    result = bcm_field_entry_prio_set(unit, ent[0], 1);
    printf("%s(): Result %d iforn bcm_field_entry_prio_set()\r\n",proc_name,result);
  }
  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but is is far more efficient to commit the whole group at once when
   *  making lots of changes.
   */
  if (BCM_E_NONE == result) {
    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE == result) {
      result = bcm_field_group_dump(unit, grp);
      if (BCM_E_NONE != result) {
        printf("%s(): Error: bcm_field_group_dump failed with error %d (%s).\r\n",proc_name, result, bcm_errmsg(result));
      }
    }
    printf("%s(): Everything went well; bcm_field_group_install() was successful. return group ID %d.\r\n",proc_name, grp);
  }
  
  if (BCM_E_NONE == result) {
    /*
     *  Everything went well; return the group ID that we allocated earlier.
     */
  } else {
    /*
     *  Something went wrong; clean up
     */
    if (full_show)
    {
        auxRes = bcm_field_show(unit, "Err: ");
    }
    for (index = 0; index < 3; index++) {
      if (0 <= ent[2 - index]) {
        auxRes = bcm_field_entry_remove(unit, ent[2 - index]);
        auxRes = bcm_field_entry_destroy(unit, ent[2 -  index]);
      }
    }
    for (index = 0; index < num_ranges; index++) {
        auxRes = bcm_field_range_destroy(unit, range_vport[index]);
    }
    if (0 <= grp) {
      auxRes = bcm_field_group_destroy(unit, grp);
    }
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
      /* NOTE: use RangeCheck instead of specific to get all range types */
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


