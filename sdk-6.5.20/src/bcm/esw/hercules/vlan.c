/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 * Purpose:	Provide low-level access to Hercules VLAN resources
 */

#include <soc/defs.h>
#if defined(BCM_HERCULES_SUPPORT)
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/port.h>
#include <bcm/trunk.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>

/*
 * Function:
 *	bcm_hercules_vlan_init
 * Purpose:
 *	Initialize the VLAN tables with the default entry in it.
 */

int
bcm_hercules_vlan_init(int unit, bcm_vlan_data_t *vd)
{
    mem_vid_entry_t	ve;

    if ((!SAL_BOOT_PLISIM) && (!SAL_BOOT_QUICKTURN)) {  /* Way too slow! */
        SOC_IF_ERROR_RETURN
	    (soc_mem_clear(unit, MEM_VIDm, MEM_BLOCK_ALL, TRUE));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "SIMULATION: skipped VLAN table clear "
                            "(assuming hardware did it)\n")));
    }

    /* Initialize VLAN 1 */

    sal_memcpy(&ve, soc_mem_entry_null(unit, MEM_VIDm), sizeof (ve));

    soc_MEM_VIDm_field32_set(unit, &ve, VIDBITMAPf,
			     SOC_PBMP_WORD_GET(vd->port_bitmap, 0));

    SOC_IF_ERROR_RETURN
	(soc_mem_write(unit, MEM_VIDm, MEM_BLOCK_ALL, vd->vlan_tag, &ve));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hercules_vlan_reload
 * Purpose:
 *	Return a list of VLANs currently installed in the hardware.
 */

int
bcm_hercules_vlan_reload(int unit, vbmp_t *bmp, int *count)
{
    mem_vid_entry_t 	ve;
    uint32 		bmval;
    int 		index;
    int 		blk;

    (*count) = 0;

    for (index = soc_mem_index_min(unit, MEM_VIDm);
         index <= soc_mem_index_max(unit, MEM_VIDm);
	 index++) {

        SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {

            SOC_IF_ERROR_RETURN(READ_MEM_VIDm(unit, blk, index, &ve));
            soc_MEM_VIDm_field_get(unit, &ve, VIDBITMAPf, &bmval);

            if (bmval) {
                SHR_BITSET(bmp->w, index);
                (*count)++;
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_vlan_create
 * Purpose:
 *	Create a VLAN entry in MEM_VID
 */

int
bcm_hercules_vlan_create(int unit, bcm_vlan_t vid)
{
    mem_vid_entry_t	ve;

    /* Clear initial port bitmap */
    sal_memset(&ve, 0, sizeof ve);

    if (vlan_info[unit].vlan_auto_stack) {
        soc_MEM_VIDm_field32_set(unit, &ve, VIDBITMAPf,
                                 SOC_PBMP_WORD_GET(PBMP_HG_ALL(unit), 0));
    }

    SOC_IF_ERROR_RETURN(WRITE_MEM_VIDm(unit, MEM_BLOCK_ALL, vid, &ve));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_vlan_destroy
 * Purpose:
 *	Clear a VLAN entry in MEM_VID
 */

int
bcm_hercules_vlan_destroy(int unit, bcm_vlan_t vid)
{
    mem_vid_entry_t	ve;
    int			rv;

    soc_mem_lock(unit, MEM_VIDm);

    if ((rv = READ_MEM_VIDm(unit, MEM_BLOCK_ANY, vid, &ve)) >= 0) {
	soc_MEM_VIDm_field32_set(unit, &ve, VIDBITMAPf, 0);

	rv = WRITE_MEM_VIDm(unit, MEM_BLOCK_ALL, vid, &ve);
    }

    soc_mem_unlock(unit, MEM_VIDm);

    return rv;
}

/*
 * Function:
 *	bcm_hercules_vlan_port_update
 * Purpose:
 *	Add/remove ports to the VLAN bitmap in a MEM_VID entry.
 * Notes:
 *      Each port has its own forwarding port mask for all VLANs.
 *	If a port is not a member of a VLAN, the corresponding
 *      forwarding port mask is set to zero. This way the table
 *      also acts as an ingress VLAN filter.
 */

STATIC int
bcm_hercules_vlan_port_update(int unit, bcm_vlan_t vid, pbmp_t pbmp, int add)
{
    mem_vid_entry_t	ve[SOC_MAX_NUM_BLKS];
    int			port, blk;
    uint32		bmval = 0;
    int                 rv;

    soc_mem_lock(unit, MEM_VIDm);

    /* Read VLAN entries for all blocks */
    SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {

        rv = READ_MEM_VIDm(unit, blk, vid, &ve[blk]);

        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, MEM_VIDm);
            return rv;
        }

        if (bmval == 0) {
            /* Pick up ports from existing VLAN member */
            soc_MEM_VIDm_field_get(unit, &ve[blk], VIDBITMAPf, &bmval);
        }
    }

    if (add) {
        bmval |= SOC_PBMP_WORD_GET(pbmp, 0);
    } else {
        bmval &= ~SOC_PBMP_WORD_GET(pbmp, 0);
    }

    /* Write non-zero entries for VLAN members only */
    SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {
        port = SOC_BLOCK_PORT(unit, blk);
        soc_MEM_VIDm_field32_set(unit, &ve[blk], VIDBITMAPf, 
                                 (bmval & (1 << port)) ? bmval : 0);
        rv = WRITE_MEM_VIDm(unit, blk, vid, &ve[blk]);
        if (SOC_FAILURE(rv)){
            soc_mem_unlock(unit, MEM_VIDm);
            return (rv);
        }
    }

    soc_mem_unlock(unit, MEM_VIDm);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_vlan_port_add
 * Purpose:
 *	Add ports to the VLAN bitmap in a MEM_VID entry.
 */

int
bcm_hercules_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                           pbmp_t ing_pbmp)
{
    BCM_PBMP_OR(pbmp, ing_pbmp);

    return bcm_hercules_vlan_port_update(unit, vid, pbmp, 1);
}

/*
 * Function:
 *	bcm_hercules_vlan_port_remove
 * Purpose:
 *	Remove ports from the VLAN bitmap in a MEM_VID entry.
 */

int
bcm_hercules_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp,
                              pbmp_t ubmp, pbmp_t ing_pbmp)
{
    return bcm_hercules_vlan_port_update(unit, vid, pbmp, 0);
}

/*
 * Function:
 *	bcm_hercules_vlan_port_get
 * Purpose:
 *	Read the port bitmap from a MEM_VID entry.
 */

int
bcm_hercules_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp,
                           pbmp_t *ubmp, pbmp_t *ing_pbmp)
{
    mem_vid_entry_t	ve;
    int			blk;
    uint32		bmval = 0;
    int rv;

    soc_mem_lock(unit, MEM_VIDm);

    SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {

        rv = READ_MEM_VIDm(unit, blk, vid, &ve);

        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, MEM_VIDm);
            return rv;
        }

        bmval = soc_MEM_VIDm_field32_get(unit, &ve, VIDBITMAPf);
        if (bmval) {
            break;
        }
    }

    if (pbmp != NULL) {
	SOC_PBMP_CLEAR(*pbmp);
	SOC_PBMP_WORD_SET(*pbmp, 0, bmval);
    }

    if (ubmp != NULL) {
	SOC_PBMP_CLEAR(*ubmp);
    }

    if (ing_pbmp != NULL) {
	SOC_PBMP_CLEAR(*ing_pbmp);
	SOC_PBMP_WORD_SET(*ing_pbmp, 0, bmval);
    }

    soc_mem_unlock(unit, MEM_VIDm);

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_vlan_stg_get
 * Purpose:
 *	Stub out STG on Hercules
 */

int
bcm_hercules_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(vid);

    *stg_ptr = 1;	/* Only one "STG" on Hercules */

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_hercules_vlan_stg_set
 * Purpose:
 *	Stub out STG on Hercules
 */

int
bcm_hercules_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(vid);

    /* Only one "STG" on Hercules */

    return (stg == 1) ? BCM_E_NONE : BCM_E_BADID;
}
#else /* BCM_HERCULES_SUPPORT */
typedef int _hercules_vlan_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_HERCULES_SUPPORT */
