/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BCM_DLIST_ENTRY

#ifdef BCM_TOMAHAWK3_SUPPORT
BCM_DLIST_ENTRY(tomahawk3)
#endif

#ifdef BCM_TOMAHAWK3X_SUPPORT
BCM_DLIST_ENTRY(tomahawk3x)
#endif

#ifdef BCM_ESW_SUPPORT
BCM_DLIST_ENTRY(esw)
#endif

#ifdef BCM_PETRA_SUPPORT
BCM_DLIST_ENTRY(petra)
#endif

#ifdef BCM_DFE_SUPPORT
BCM_DLIST_ENTRY(dfe)
#endif

#ifdef BCM_DNXF_SUPPORT
BCM_DLIST_ENTRY(dnxf)
#endif

#ifdef BCM_DNX_SUPPORT
BCM_DLIST_ENTRY(dnx)
#endif

#ifdef BCM_RPC_SUPPORT
BCM_DLIST_ENTRY(client) 
#endif

#ifdef BCM_LOOP_SUPPORT
BCM_DLIST_ENTRY(loop)
#endif

#ifdef BCM_TR3_SUPPORT
BCM_DLIST_ENTRY(tr3)
#endif

#ifdef BCM_SHADOW_SUPPORT
BCM_DLIST_ENTRY(shadow)
#endif   
      
#ifdef BCM_LTSW_SUPPORT
BCM_DLIST_ENTRY(ltsw)
#endif

#undef BCM_DLIST_ENTRY


#endif /* BCM_DLIST_ENTRY */
