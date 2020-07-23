/*! \file stack.c
 *
 * STACK Driver for XFS family chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw/xfs/stack.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/stack_int.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/xfs/port.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>



/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STK
/* sys_port encoding reference from bcmXXX_ltsw_port_service_init_per_port. */
#define SYSTEM_PORT_SET(_u, _s, _m, _p) ((_s) = ((_m) << 8) | \
    ((_p) ? ((_p) & 0xFF) : PORT_NUM_MAX(_u)))
#define SYSTEM_PORT_MODID_GET(_s) ((_s) >> 8)

/******************************************************************************
 * Public definition
 */


/******************************************************************************
 * Private functions
 */

/*!
* \brief Get my modual id on port table.
*
* \param [in] unit Unit Number.
* \param [in] my_modid My Module ID Number.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
static int
ltsw_xfs_stack_get(int unit, int *my_modid)
{
    int sys_port = 0, port;
    int rv, flag;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    /* All existed ports have the same modid value */
    flag = BCMI_LTSW_PORT_TYPE_ALL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, flag, &pbmp));
    BCM_PBMP_ITER(pbmp, port) {
        rv = bcmi_ltsw_port_tab_get(unit, port,
                                    BCMI_PT_ING_SYS_PORT, &sys_port);
        if (rv == SHR_E_NOT_FOUND) {
            /* If the my_modid has not been set, assign the default 0 value. */
            *my_modid = 0;
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        /* All existed ports have the same modid value, sample 1 port only. */
        break;
    }
    *my_modid = SYSTEM_PORT_MODID_GET(sys_port);
exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Set my modual id on port table.
*
* \param [in] unit Unit Number.
* \param [in] my_modid My Module ID Number.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
static int
ltsw_xfs_stack_set(int unit, int my_modid)
{
    uint32_t sys_port, port;
    int flag, l2_iif, old_sys, old_modid;
    bcm_pbmp_t pbmp;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &old_modid));

    if (old_modid == my_modid) {
        SHR_EXIT();
    }

    /* Iterate all the existed ports. */
    flag = BCMI_LTSW_PORT_TYPE_ALL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, flag, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        SYSTEM_PORT_SET(unit, sys_port, my_modid, port);
        SYSTEM_PORT_SET(unit, old_sys, old_modid, port);
        /* System port and l2_iif mapping */
        /* Get the L2_IIF vlaue from the old system port */\
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_raw_get(unit, old_sys,
                                        BCMI_PT_ING_L2_IIF, &l2_iif));
        /* Demap old system port and l2_if. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_sys_port_demap_l2_if(unit, old_sys, l2_iif));
        /* Move old system port table content to new system port. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_port_sys_port_move(unit, port, sys_port));
        /* Map new system port and l2_if. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sys_port_map_l2_if(unit, sys_port, l2_iif));

        /*
         * port_sys_port_move will prevent moving to existing entry.
         * So the previous l2_iif sw data won't be over-write.
         */
        si->modport[my_modid].l2_if[port] = l2_iif;
        si->modport[my_modid].port_count++;

        /* Clear the previous l2_iif mapping with modport. */
        si->modport[old_modid].l2_if[port] = BCMI_L2_IF_INVALID;
        si->modport[old_modid].port_count--;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the forwarding port for remote modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 * \param [in] gport Global port identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_stack_modport_set(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t gport)
{
    int sys_port;
    bcm_trunk_t tid, sys_tid;
    bcm_trunk_chip_info_t chip_info;
    int rv, dlb_id = 0, dlb_id_valid = 0, str_id;
    bcmi_ltsw_port_tab_t type[7] = {0};
    int data[7] = {0};
    int fld_cnt;

    SHR_FUNC_ENTER(unit);

    SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);

    fld_cnt = 0;
    if (BCM_GPORT_IS_TRUNK(gport)) {
        /* Setting Local Trunk ID in PORT_SYSTEM_DESTINATION */
        tid = BCM_GPORT_TRUNK_GET(gport);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_id_validate(unit, tid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_chip_info_get(unit, &chip_info));
        /* System trunk id appended behind trunk id. */
        sys_tid = tid - chip_info.trunk_fabric_id_min;
        rv = bcmi_ltsw_trunk_find_dlb_id(unit, tid, &dlb_id);
        if (SHR_SUCCESS(rv)) {
            dlb_id_valid = 1;
        }
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_TRUNK;
        data[fld_cnt] = sys_tid;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_IS_TRUNK;
        data[fld_cnt] = 1;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_DLB_ID;
        data[fld_cnt] = dlb_id;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_DLB_ID_VALID;
        data[fld_cnt] = dlb_id_valid;
        fld_cnt++;
    } else {
        /* Setting Local Port in PORT_SYSTEM_DESTINATION */
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_LPORT;
        data[fld_cnt] = gport;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_IS_TRUNK;
        data[fld_cnt] = 0;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_DLB_ID;
        data[fld_cnt] = dlb_id;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_DEST_SYS_PORT_DLB_ID_VALID;
        data[fld_cnt] = dlb_id_valid;
        fld_cnt++;
        type[fld_cnt] = BCMI_PT_ING_PP_PORT;
        data[fld_cnt] = gport;
        fld_cnt++;
    }
    /* ing sys port strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_SYS_PORT, BCMI_LTSW_SBR_PET_NONE, &str_id));
    type[fld_cnt] = BCMI_PT_SYS_PORT_STR_PROFILE_ID;
    data[fld_cnt] = str_id;
    fld_cnt++;
    /* ing sys dest strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION, BCMI_LTSW_SBR_PET_DEF, &str_id));
    type[fld_cnt] = BCMI_PT_SYS_DEST_STR_PROFILE_ID;
    data[fld_cnt] = str_id;
    fld_cnt++;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_multi_set(unit, sys_port, type, data, fld_cnt));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the forarding port forthe remote modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 * \param [in] gport Global port identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_stack_modport_get(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t *gport)
{
    uint64_t sys_port;
    bcm_port_t ptmp;
    bcm_trunk_t tid, sys_tid;
    int is_trunk;
    bcm_trunk_chip_info_t chip_info;

    SHR_FUNC_ENTER(unit);

    SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_get(unit, sys_port,
                                   BCMI_PT_DEST_SYS_PORT_LPORT, &ptmp));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_get(unit, sys_port,
                                   BCMI_PT_DEST_SYS_PORT_TRUNK, &sys_tid));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_get(unit, sys_port,
                                   BCMI_PT_DEST_SYS_PORT_IS_TRUNK, &is_trunk));

    if (is_trunk) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_chip_info_get(unit, &chip_info));
        /* System trunk id appended behind trunk id. */
        tid = sys_tid + chip_info.trunk_fabric_id_min;
        BCM_GPORT_TRUNK_SET(*gport, tid);
    } else {
        *gport = ptmp;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleat the forwarding port for remote modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_stack_modport_clear(int unit,
                             int modid,
                             bcm_port_t dest_port)
{
    uint64_t sys_port;
    int my_modid;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
    if (modid == my_modid) {
        LOG_ERROR(BSL_LS_BCM_STK,
                 (BSL_META_U(unit,
                             "Error: Cannot clear local module ID.")));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_EGR));
    /* Prevent to clear the setting of remote port config if it is in use. */
    if (si->modport[modid].port_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_ING));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Map the l2_if for the remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_stack_l2_if_map(int unit,
                           int modid,
                           bcm_port_t dest_port)
{
    int sys_port, modport;
    int l2_if;
    int str_id;
    ltsw_stack_info_t *si;
    uint16_t tpid;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];

    if (si->modport[modid].l2_if[dest_port] != BCMI_L2_IF_INVALID) {
        /* The remote port already mapped with l2_if. */
        SHR_EXIT();
    }
    /* Allocate L2_OIF for system port mapping. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_alloc(unit, &l2_if));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_add(unit, l2_if));
    SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);

    /* System port and L2_IF mapping */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_map_l2_if(unit, sys_port, l2_if));
    si->modport[modid].l2_if[dest_port] = l2_if;
    si->modport[modid].port_count++;

    /* l2_oif strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_EGR_L2_OIF, BCMI_LTSW_SBR_PET_DEF, &str_id));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_L2_OIF_STR_PROFILE_ID, str_id));

    /* default VLAN */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_DEFAULT_VFI, BCM_VLAN_DEFAULT));

    /* VLAN map to VFI */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_VLAN_MAP_TO_VFI, 1));

    /* only preserve itag */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_VLAN_PRESERVE, 0x2));

    /* learn and forward */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_CML_NEW, 0x4));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_CML_MOVE, 0x4));

    /* drop static move */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_STATIC_MOVE_DROP, 1));

    /* outer tpid */
    BCM_GPORT_MODPORT_SET(modport, modid, dest_port);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_tpid_set(unit, modport, tpid));

    /* Setting the pp port if the modport is specified. */
    if (si->modport[modid].port != BCM_PORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_raw_set(unit, sys_port,
                                        BCMI_PT_ING_PP_PORT,
                                        si->modport[modid].port));
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleat the l2_if attach to remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xfs_stack_l2_if_clear(int unit,
                             int modid,
                             bcm_port_t dest_port)
{
    int l2_if;
    ltsw_stack_info_t *si;
    uint64_t sys_port;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    /* Remove L2_OIF from system port mapping. */
    l2_if = si->modport[modid].l2_if[dest_port];
    if (l2_if == BCMI_L2_IF_INVALID) {
        /*
         * Checking modport[modid].port to determine whether modport_clear is used or not.
         * If it is used, need to clear ING_SYSTEM_PORT_TABLE.
         */
        if (si->modport[modid].port == BCM_PORT_INVALID) {
            SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_ING));
        }
        /* No modport to clear, exit the function. */
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_del(unit, l2_if));
    /* Free the allocasted l2_if */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_free(unit, l2_if));
    /* Delete the system table that map with L2_IF. */
    SYSTEM_PORT_SET(unit, sys_port, modid, dest_port);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_ING));

    si->modport[modid].l2_if[dest_port] = BCMI_L2_IF_INVALID;
    si->modport[modid].port_count--;

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_xfs_stack_init(int unit)
{
    bcmlt_entry_handle_t lte_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t lte_hd2 = BCMLT_INVALID_HDL;
    int dunit, rv, port, my_modid;
    bcm_port_t p;
    uint64_t modid, value, value1, value2;
    ltsw_stack_info_t *si;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_get(unit, &my_modid));

    if (warm) {
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ING_SYSTEM_PORT_TABLEs, &lte_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ING_SYSTEM_DESTINATION_TABLEs, &lte_hd2));
        /* Restoring l2_if sw structure. */
        while ((rv = bcmi_lt_entry_commit(
                        unit, lte_hdl, BCMLT_OPCODE_TRAVERSE,
                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(lte_hdl, SYSTEM_SOURCEs, &value));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(lte_hdl, L2_IIFs, &value1));
            modid = SYSTEM_PORT_MODID_GET(value);
            p = value & 0xFF;
            /*
             * Double check the L2_IIF retrieved from ING_SYSTEM_PORT_TABLE
             * is created by stack_l2_if_map, to avoid warmboot mismatch.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(lte_hd2, SYSTEM_DESTINATION_PORTs, value));
            rv = bcmlt_entry_commit(lte_hd2, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
            if (rv == SHR_E_NONE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(lte_hd2, L2_OIFs, &value2));
            } else {
                continue;
            }
            if (value1 == value2) {
                if (value1 == 0) {
                    /* l2_if = 0 is reserved but not used. */
                    continue;
                }
                si->modport[modid].l2_if[p] = value1;
                si->modport[modid].port_count++;

                if (si->modport[modid].port == BCM_PORT_INVALID &&
                    modid != my_modid) {
                    port = BCM_PORT_INVALID;
                    rv = ltsw_xfs_stack_modport_get(unit, modid, p, &port);
                    if (rv != SHR_E_NOT_FOUND) {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }
                    si->modport[modid].port = port;
                }
            }
        }
        stk_my_modid[unit] = my_modid;
    }else {
        /* Cold init, clear the relating HW setting except my_modid related setting. */
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ING_SYSTEM_PORT_TABLEs, &lte_hdl));
        while ((rv = bcmi_lt_entry_commit(
                        unit, lte_hdl, BCMLT_OPCODE_TRAVERSE,
                        BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(lte_hdl, SYSTEM_SOURCEs, &value));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(lte_hdl, L2_IIFs, &value1));
            if (SYSTEM_PORT_MODID_GET(value) != my_modid) {
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_ltsw_l2_if_del(unit, value1));
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_ltsw_sys_port_del(
                      unit, value, (BCMI_LTSW_PORT_ING | BCMI_LTSW_PORT_EGR)));
            }
        }
        /* Restore to defaul module id 0. */
        if (my_modid != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_stk_my_modid_set(unit, 0));
        }
    }

exit:
    if (lte_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(lte_hdl);
        lte_hdl = BCMLT_INVALID_HDL;
    }
    if (lte_hd2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(lte_hd2);
        lte_hd2 = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

int
xfs_ltsw_stack_init(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_set(int unit, int my_modid)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_set(unit, my_modid));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_get(int unit, int *my_modid)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_get(unit, my_modid));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_modport_set(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_modport_set(unit, modid, dest_port, port));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_modport_get(int unit,
                           int modid,
                           bcm_port_t dest_port,
                           bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_modport_get(unit, modid, dest_port, port));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_modport_clear(int unit,
                             int modid,
                             bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_modport_clear(unit, modid, dest_port));
exit:
    SHR_FUNC_EXIT();
}



int
xfs_ltsw_stack_l2_if_map(int unit,
                           int modid,
                           bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_l2_if_map(unit, modid, dest_port));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_l2_if_clear(int unit,
                             int modid,
                             bcm_port_t dest_port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xfs_stack_l2_if_clear(unit, modid, dest_port));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_sys_port_get(int unit,
                            int modid,
                            bcm_port_t port,
                            int *sys_port)
{
    SYSTEM_PORT_SET(unit, *sys_port, modid, port);
    return SHR_E_NONE;
}

int
xfs_ltsw_stack_sys_port_to_modport_get(int unit,
                                       int sys_port,
                                       int *modid,
                                       bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);
    *port = (sys_port & 0xFF);
    *modid = SYSTEM_PORT_MODID_GET(sys_port);

    if (!MODID_ADDRESSABLE(unit, *modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

exit:
    SHR_FUNC_EXIT();

}

int
xfs_ltsw_stack_ifa_system_source_alloc(int unit, int *sys_source_ifa_probe_copy)
{
    int sys_port, port, modid, l2_if;
    ltsw_stack_info_t *si;
    SHR_FUNC_ENTER(unit);

    /*
     * use maximun system_port, which is invalid,
     * as sys_source_ifa_probe_copy route setting.
     */

    port = bcmi_ltsw_dev_max_port_addr(unit);
    modid = bcmi_ltsw_dev_max_modid(unit);
    SYSTEM_PORT_SET(unit, sys_port, modid, port);

    si = ltsw_stack_info[unit];

    if (si->modport[modid].l2_if[port] != BCMI_L2_IF_INVALID) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_RESOURCE,
             (BSL_META_U(unit, "Already configured the IFA 1.0 sys_src.\n")));
    }

    *sys_source_ifa_probe_copy = sys_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_alloc(unit, &l2_if));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_add(unit, l2_if));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_map_l2_if(unit, sys_port, l2_if));
    si->modport[modid].l2_if[port] = l2_if;
    si->modport[modid].port_count++;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_DEFAULT_VFI, BCM_VLAN_NONE));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_VLAN_MAP_TO_VFI, 0));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_raw_set(unit, l2_if,
                                   BCMI_PT_VLAN_PRESERVE, 0x2));

    exit:
        SHR_FUNC_EXIT();
}

int
xfs_ltsw_stack_ifa_system_source_destroy(int unit)
{
    int sys_port, port, modid, l2_if;
    ltsw_stack_info_t *si;
    SHR_FUNC_ENTER(unit);

    /*
     * use maximun system_port, which is invalid,
     * as sys_source_ifa_probe_copy route setting.
     */

    port = bcmi_ltsw_dev_max_port_addr(unit);
    modid = bcmi_ltsw_dev_max_modid(unit);
    SYSTEM_PORT_SET(unit, sys_port, modid, port);

    si = ltsw_stack_info[unit];

    if (si->modport[modid].l2_if[port] == BCMI_L2_IF_INVALID) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "IFA 1.0 sys_src not configured.\n")));
    }

    l2_if = si->modport[modid].l2_if[port];
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_del(unit, l2_if));
    /* Free the allocasted l2_if. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_if_free(unit, l2_if));

    /* Delete the system table that map with L2_IF. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_ING));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_del(unit, sys_port, BCMI_LTSW_PORT_EGR));

    si->modport[modid].l2_if[port] = BCMI_L2_IF_INVALID;
    si->modport[modid].port_count--;

    exit:
        SHR_FUNC_EXIT();
}
