/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        knet.c
 * Purpose:     Kernel Networking Management
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KNET

#include <bcm/types.h>
#include <bcm/knet.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>
#include <bcm/switch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <src/bcm/dnx/trunk/trunk_sw_db.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>
#include <src/bcm/dnx/trunk/trunk_verify.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/control.h>
#include <soc/knet.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trunk.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/shared/dcbformats/type39.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef INCLUDE_KNET

/** DNX Meta data */
typedef struct dnx_meta_data_s
{
#ifdef LE_HOST
    uint32 trap_qualifier:16,   /* Internal.Trap-Qualifier */
        trap_id:16;             /* Internal.Trap-Code */
#else
    uint32 trap_id:16,          /* Internal.Trap-Code */
        trap_qualifier:16;      /* Internal.Trap-Qualifier */
#endif

#ifdef LE_HOST
    uint32 forward_domain:16,   /* Bridging: VSI, Routing: VRF */
        spa:16;                 /* FTMH.Source-system-port-aggregate */
#else
    uint32 spa:16,              /* FTMH.Source-system-port-aggregate */
        forward_domain:16;      /* Bridging: VSI, Routing: VRF */
#endif

#ifdef LE_HOST
    uint32 reserve:30, action_type:2;   /* Internal.TM-action-type */
#else
    uint32 action_type:2,       /* Internal.TM-action-type */
        reserve:30;
#endif

/** Last DW of DCB */
#ifdef  LE_HOST
    uint32 count:16,            /* Transferred byte count */
        end:1,                  /* End bit (RX) */
        start:1,                /* Start bit (RX) */
        error:1,                /* Cell Error (RX) */
        ecc_error:1,            /* packet ECC Error (RX) */
    :   11,                     /* Reserved */
        done:1;                 /* Descriptor Done */
#else
    uint32 done:1,              /* Descriptor Done */
    :   11,                     /* Reserved */
        ecc_error:1,            /* Packet ECC Error (RX) */
        error:1,                /* Cell Error (RX) */
        start:1,                /* Start bit (RX) */
        end:1,                  /* End bit (RX) */
        count:16;               /* Transferred byte count */
#endif
} dnx_meta_data_t;

/*
 * ITMH base header
 */
typedef union dnx_knet_itmh_base_u
{
    struct
    {
        uint8 bytes[5];
    } raw;
    struct
    {
#if defined(LE_HOST)
        unsigned inbound_mirror_disable:1;
        unsigned pph_type:2;
        unsigned injected_as_extension_present:1;
        unsigned forward_action_strength:3;
        unsigned reserved:1;
        unsigned destination_hi:6;
        unsigned drop_precedence:2;
        unsigned destination_mi:8;
        unsigned snoop_profile_hi:1;
        unsigned destination_lo:7;
        unsigned itmh_base_ext:1;
        unsigned traffic_class:3;
        unsigned snoop_profile_lo:4;
#else
        unsigned reserved:1;
        unsigned forward_action_strength:3;
        unsigned injected_as_extension_present:1;
        unsigned pph_type:2;
        unsigned inbound_mirror_disable:1;
        unsigned drop_precedence:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned snoop_profile_hi:1;
        unsigned snoop_profile_lo:4;
        unsigned traffic_class:3;
        unsigned itmh_base_ext:1;
#endif
    } jr2_common;

    struct
    {
        unsigned _rsvd_0:8;
#if defined(LE_HOST)
        unsigned destination_hi:6;
        unsigned _rsvd_1:2;
        unsigned destination_mi:8;
        unsigned _rsvd_2:1;
        unsigned destination_lo:7;
#else
        unsigned _rsvd_1:2;
        unsigned destination_hi:6;
        unsigned destination_mi:8;
        unsigned destination_lo:7;
        unsigned _rsvd_2:1;
#endif
        unsigned _rsvd_3:8;
    } jr2_dest_info;
} dnx_knet_itmh_base_t;

/* PTCH_2 */
#define DNX_KNET_PTCH_2_SIZE          2
/* ITMH */
#define DNX_KNET_ITMH_SIZE            5
/* Modlue Header */
#define DNX_KNET_MODULE_HEADER_SIZE   16
/* Port Direction */
#define DNX_KNET_PORT_DIRECTION_IN    1

/**
 * \brief - Get source system port per in PP port
 *
 * \param [in] unit - Unit id
 * \param [in] core_id - In core id
 * \param [in] pp_port - In pp port
 * \param [in] src_system_port - Source system port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_knet_pp_port_to_src_system_port_get(
    int unit,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 *src_system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, src_system_port);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Verify input parameters to the bcm_dnx_knet_netif_create API.
 *
 * \param [in] unit - Unit ID
 * \param [in] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_knet_netif_create_verify(
    int unit,
    bcm_knet_netif_t * netif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(netif, _SHR_E_PARAM, "knet netif");

    switch (netif->type)
    {
        case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
            break;
        case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
            break;
        case BCM_KNET_NETIF_T_TX_META_DATA:
            if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
            {
                break;
            }
            else
            {
                SHR_ERR_EXIT(BCM_E_PARAM, "KNET: flag BCM_KNET_NETIF_F_RCPU_ENCAP is mandatory for netif type %d\n",
                             netif->type);
            }
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported netif type %d\n", netif->type);
    }

    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
    {
        if (netif->type != BCM_KNET_NETIF_T_TX_META_DATA)
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: type BCM_KNET_NETIF_T_TX_META_DATA is mandatory for RCPU mode\n");
        }
    }

    /*
     * Verify if the VLAN value is valid
     */
    BCM_DNX_VLAN_CHK_ID(unit, netif->vlan);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_create(
    int unit,
    bcm_knet_netif_t * netif)
{
    kcom_msg_netif_create_t netif_create;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t ptch_ssp = 0;
    bcm_port_t logical_port = 0;
    bcm_gport_t cpu_port = 0;
    uint8 has_cpu_port = FALSE;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_port_t sysport_gport;
    int header_type_incoming = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    uint8 *system_headers;
    int cpu_channel;
    bcm_port_config_t port_config;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_knet_netif_create_verify(unit, netif));

    sal_memset(&netif_create, 0, sizeof(netif_create));
    netif_create.hdr.opcode = KCOM_M_NETIF_CREATE;
    netif_create.hdr.unit = unit;

    switch (netif->type)
    {
        case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
            netif_create.netif.type = KCOM_NETIF_T_VLAN;
            break;
        case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
            netif_create.netif.type = KCOM_NETIF_T_PORT;
            break;
        case BCM_KNET_NETIF_T_TX_META_DATA:
            netif_create.netif.type = KCOM_NETIF_T_META;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported interface type\n");
    }

    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_ADD_TAG;
    }
    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_RCPU_ENCAP;
    }
    if (netif->flags & BCM_KNET_NETIF_F_KEEP_RX_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_KEEP_RX_TAG;
    }

    netif_create.netif.vlan = netif->vlan;
    netif_create.netif.port = netif->port;
    netif_create.netif.cb_user_data = netif->cb_user_data;

    key.index = DNX_KNET_PORT_DIRECTION_IN;
    key.type = bcmSwitchPortHeaderType;
    if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
    {
        bcm_core_t ptch_ssp_core;

        /** Get PP Port and core of configured port */
        SHR_IF_ERR_EXIT(bcm_port_get(unit, netif->port, &flags, &interface_info, &mapping_info));
        ptch_ssp = mapping_info.pp_port;
        ptch_ssp_core = mapping_info.core;

        if (interface_info.interface == BCM_PORT_IF_CPU)
        {
            logical_port = netif->port;
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
            BCM_PBMP_ITER(port_config.cpu, cpu_port)
            {
                SHR_IF_ERR_EXIT(bcm_port_get(unit, cpu_port, &flags, &interface_info, &mapping_info));
                /** Find first CPU port on the same core with configured port. */
                if (mapping_info.core == ptch_ssp_core)
                {
                    logical_port = cpu_port;
                    has_cpu_port = TRUE;
                    break;
                }
            }
            if (!has_cpu_port)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "NO CPU port on the same core with %d\n", netif->port);
            }
        }

        /** Check if psrc is trunk */
        if (ptch_ssp == 0xFFFFFFFF)
        {
            bcm_gport_t trunk_gport;
            bcm_trunk_t tid_find;
            bcm_trunk_pp_port_allocation_info_t allocation_info;

            /** Switch local to sysport gport */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, netif->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, gport_info.sys_port);

            /** Get trunk id from sysport */
            bcm_trunk_find(unit, -1, sysport_gport, &tid_find);
            /** Get the pp port allocation info of a given trunk */
            SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(unit, tid_find, 0, &allocation_info));
            /** Check that trunk has a core on this member port */
            if ((SAL_BIT(ptch_ssp_core) & allocation_info.core_bitmap) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Source port %d is not a valid member of Trunk %d\n", netif->port, tid_find);
            }
            else
            {
                ptch_ssp = allocation_info.pp_port_per_core_array[ptch_ssp_core];
            }
            BCM_GPORT_TRUNK_SET(trunk_gport, tid_find);
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, trunk_gport, key, &value));
            header_type_incoming = value.value;
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, logical_port, key, &value));
            header_type_incoming = value.value;
        }
    }
    else if ((netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT) || (netif->type == BCM_KNET_NETIF_T_TX_META_DATA))
    {
        /** Find first CPU port. */
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
        BCM_PBMP_ITER(port_config.cpu, cpu_port)
        {
            SHR_IF_ERR_EXIT(bcm_port_get(unit, cpu_port, &flags, &interface_info, &mapping_info));
            ptch_ssp = mapping_info.pp_port;
            logical_port = cpu_port;
            has_cpu_port = TRUE;
            break;
        }
        if (!has_cpu_port)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NO CPU port\n");
        }
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, logical_port, key, &value));
        header_type_incoming = value.value;
    }

    /** Build system headers */
    system_headers = netif_create.netif.system_headers;
    sal_memset(system_headers, 0, sizeof(netif_create.netif.system_headers));
    netif_create.netif.system_headers_size = 0;

    if ((header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP) ||
        (header_type_incoming == BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2))
    {
        /** Module Header */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &cpu_channel));
        /** Set cpu channel to first byte of Module Header*/
        system_headers[0] = cpu_channel;
        netif_create.netif.system_headers_size = DNX_KNET_MODULE_HEADER_SIZE;
        if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
        {
            /** Append PTCH */
            system_headers[DNX_KNET_MODULE_HEADER_SIZE] = (ptch_ssp >> 8) & 0x3;
            system_headers[DNX_KNET_MODULE_HEADER_SIZE] |= 0x80;
            system_headers[DNX_KNET_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
            netif_create.netif.system_headers_size += DNX_KNET_PTCH_2_SIZE;
        }
        else if (netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT)
        {
            dnx_knet_itmh_base_t *itmh;

            /** Append PTCH: Parser-Program-Control = 0, indicate next header is ITMH */
            system_headers[DNX_KNET_MODULE_HEADER_SIZE] = (ptch_ssp >> 8) & 0x3;
            system_headers[DNX_KNET_MODULE_HEADER_SIZE + 1] = ptch_ssp & 0xff;
            netif_create.netif.system_headers_size += DNX_KNET_PTCH_2_SIZE;

            /** Get the system port from logical port as destination port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, netif->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, gport_info.sys_port);

            /** Append ITMH */
            itmh = (dnx_knet_itmh_base_t *) & system_headers[DNX_KNET_MODULE_HEADER_SIZE + DNX_KNET_PTCH_2_SIZE];
            /** 01100(5) + System_Port_Agg(16) */
            itmh->jr2_dest_info.destination_hi = 0x18 | ((sysport_gport >> 15) & 0x1);
            itmh->jr2_dest_info.destination_mi = (sysport_gport >> 7) & 0xff;
            itmh->jr2_dest_info.destination_lo = sysport_gport & 0x7f;
            netif_create.netif.system_headers_size += DNX_KNET_ITMH_SIZE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Injected port is not found for port %d type %d\n", netif->port, netif->type);
    }

    sal_memcpy(netif_create.netif.macaddr, netif->mac_addr, 6);
    sal_memcpy(netif_create.netif.name, netif->name, sizeof(netif_create.netif.name) - 1);
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_create, sizeof(netif_create), sizeof(netif_create)));
    /** ID and interface name are assigned by kernel */
    netif->id = netif_create.netif.id;
    sal_memcpy(netif->name, netif_create.netif.name, sizeof(netif->name) - 1);

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_destroy(
    int unit,
    int netif_id)
{
    kcom_msg_netif_destroy_t netif_destroy;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    sal_memset(&netif_destroy, 0, sizeof(netif_destroy));
    netif_destroy.hdr.opcode = KCOM_M_NETIF_DESTROY;
    netif_destroy.hdr.unit = unit;
    netif_destroy.hdr.id = netif_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_destroy, sizeof(netif_destroy), sizeof(netif_destroy)));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel network interface configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 * \param [out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_get(
    int unit,
    int netif_id,
    bcm_knet_netif_t * netif)
{
    kcom_msg_netif_get_t netif_get;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    SHR_NULL_CHECK(netif, _SHR_E_PARAM, "netif");
    sal_memset(&netif_get, 0, sizeof(netif_get));
    netif_get.hdr.opcode = KCOM_M_NETIF_GET;
    netif_get.hdr.unit = unit;
    netif_get.hdr.id = netif_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_get, sizeof(netif_get.hdr), sizeof(netif_get)));
    bcm_knet_netif_t_init(netif);
    switch (netif_get.netif.type)
    {
        case KCOM_NETIF_T_VLAN:
            netif->type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
            break;
        case KCOM_NETIF_T_PORT:
            netif->type = BCM_KNET_NETIF_T_TX_LOCAL_PORT;
            break;
        case KCOM_NETIF_T_META:
            netif->type = BCM_KNET_NETIF_T_TX_META_DATA;
            break;
        default:
            /*
             * Unknown type - defaults to VLAN
             */
            break;
    }

    if (netif_get.netif.flags & KCOM_NETIF_F_ADD_TAG)
    {
        netif->flags |= BCM_KNET_NETIF_F_ADD_TAG;
    }
    if (netif_get.netif.flags & KCOM_NETIF_F_RCPU_ENCAP)
    {
        netif->flags |= BCM_KNET_NETIF_F_RCPU_ENCAP;
    }
    if (netif_get.netif.flags & KCOM_NETIF_F_KEEP_RX_TAG)
    {
        netif->flags |= BCM_KNET_NETIF_F_KEEP_RX_TAG;
    }

    netif->id = netif_get.netif.id;
    netif->vlan = netif_get.netif.vlan;
    netif->port = netif_get.netif.port;
    sal_memcpy(netif->mac_addr, netif_get.netif.macaddr, 6);
    sal_memcpy(netif->name, netif_get.netif.name, sizeof(netif->name) - 1);
    netif->cb_user_data = netif_get.netif.cb_user_data;
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel network interface objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [out] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_traverse(
    int unit,
    bcm_knet_netif_traverse_cb trav_fn,
    void *user_data)
{
    int idx;
    bcm_knet_netif_t netif;
    kcom_msg_netif_list_t netif_list;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "KNET: trav_fn is NULL");
    sal_memset(&netif_list, 0, sizeof(netif_list));
    netif_list.hdr.opcode = KCOM_M_NETIF_LIST;
    netif_list.hdr.unit = unit;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & netif_list, sizeof(netif_list.hdr), sizeof(netif_list)));
    for (idx = 0; idx < netif_list.ifcnt; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_knet_netif_get(unit, netif_list.id[idx], &netif));
        SHR_IF_ERR_EXIT(trav_fn(unit, &netif, user_data));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify input parameters to the bcm_dnx_knet_filter_create API.
 *
 * \param [in] unit - Unit ID
 * \param [in] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_knet_filter_create_verify(
    int unit,
    bcm_knet_filter_t * filter)
{
    uint32 trunk_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(filter, _SHR_E_PARAM, "KNET filter");

    switch (filter->dest_type)
    {
        case BCM_KNET_DEST_T_NULL:
            break;
        case BCM_KNET_DEST_T_NETIF:
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG)
            {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U
                          (unit,
                           "KNET: VLAN tag can be stripped only for packets sent to vitrual network interface.\n")));
            }
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported destination type\n");
    }
    switch (filter->mirror_type)
    {
        case BCM_KNET_DEST_T_NULL:
            break;
        case BCM_KNET_DEST_T_NETIF:
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Cannot mirror to callback\n");
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported mirror type\n");
    }

    if (filter->mirror_type && (filter->dest_type != BCM_KNET_DEST_T_NETIF))
    {
        SHR_ERR_EXIT(BCM_E_PARAM,
                     "KNET: Mirror doesn't actually work when Filter destination is not BCM_KNET_DEST_T_NETIF\n");
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_VLAN)
    {
        if (filter->m_vlan)
        {
            /*
             * Verify if the VLAN value is valid
             */
            BCM_DNX_VLAN_CHK_ID(unit, filter->m_vlan);
        }
        else
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: VLAN ID to match is not configured\n");
        }

    }
    if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
    {
        if (BCM_GPORT_IS_TRUNK(filter->m_ingport))
        {
            /*
             * In case of trunk gport, get the trunk id and verify it
             */
            trunk_id = BCM_GPORT_TRUNK_GET(filter->m_ingport);
            SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)
    {
        if ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID) != BCM_KNET_FILTER_M_SRC_MODID)
        {
            SHR_ERR_EXIT(BCM_E_PARAM,
                         "KNET: filter to match source module id and module port should be configured together\n");
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
    {
        if ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT) != BCM_KNET_FILTER_M_SRC_MODPORT)
        {
            SHR_ERR_EXIT(BCM_E_PARAM,
                         "KNET: filter to match source module id and module port should be configured together\n");
        }
    }

    if (filter->priority > 255 || filter->priority < 0)
    {
        /** Priority is of type unit8 in KNET kernel module */
        SHR_ERR_EXIT(BCM_E_PARAM, "KNET: filter priority is out of range (0 ~ 255)\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_create(
    int unit,
    bcm_knet_filter_t * filter)
{
    int idx, pdx;
    int oob_size;
    int data_offset;
    kcom_msg_filter_create_t filter_create;
    dnx_meta_data_t *meta_data = NULL;
    dnx_meta_data_t *meta_mask = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_knet_filter_create_verify(unit, filter));

    sal_memset(&filter_create, 0, sizeof(filter_create));
    filter_create.hdr.opcode = KCOM_M_FILTER_CREATE;
    filter_create.hdr.unit = unit;
    filter_create.filter.type = KCOM_FILTER_T_RX_PKT;
    switch (filter->dest_type)
    {
        case BCM_KNET_DEST_T_NULL:
            filter_create.filter.dest_type = KCOM_DEST_T_NULL;
            break;
        case BCM_KNET_DEST_T_NETIF:
            filter_create.filter.dest_type = KCOM_DEST_T_NETIF;
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            filter_create.filter.dest_type = KCOM_DEST_T_API;
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            filter_create.filter.dest_type = KCOM_DEST_T_CB;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported destination type\n");
    }

    switch (filter->mirror_type)
    {
        case BCM_KNET_DEST_T_NULL:
            filter_create.filter.mirror_type = KCOM_DEST_T_NULL;
            break;
        case BCM_KNET_DEST_T_NETIF:
            filter_create.filter.mirror_type = KCOM_DEST_T_NETIF;
            break;
        case BCM_KNET_DEST_T_BCM_RX_API:
            filter_create.filter.mirror_type = KCOM_DEST_T_API;
            break;
        case BCM_KNET_DEST_T_CALLBACK:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Cannot mirror to callback\n");
        default:
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported mirror type\n");
    }

    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG)
    {
        filter_create.filter.flags |= KCOM_FILTER_F_STRIP_TAG;
    }

    filter_create.filter.dest_id = filter->dest_id;
    filter_create.filter.dest_proto = filter->dest_proto;
    filter_create.filter.mirror_id = filter->mirror_id;
    filter_create.filter.mirror_proto = filter->mirror_proto;
    filter_create.filter.priority = filter->priority;
    sal_strncpy(filter_create.filter.desc, filter->desc, KCOM_FILTER_DESC_MAX);
    filter_create.filter.cb_user_data = filter->cb_user_data;

    oob_size = 0;
    filter_create.filter.oob_data_offset = 0;
    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW)
    {
        oob_size = sizeof(dnx_meta_data_t);
        filter_create.filter.oob_data_size = oob_size;
    }

    /*
     * Create inverted mask
     */
    for (idx = 0; idx < oob_size; idx++)
    {
        filter_create.filter.mask.b[idx] = 0xff;
    }

    meta_data = (dnx_meta_data_t *) & filter_create.filter.data;
    meta_mask = (dnx_meta_data_t *) & filter_create.filter.mask;

    if (filter->match_flags & BCM_KNET_FILTER_M_VLAN)
    {
        meta_data->forward_domain = filter->m_vlan;
        meta_mask->forward_domain = 0;
    }

    /** FTMH.SSPA is set for both M_INGPORT and M_SRC_MODPORT */
    if ((filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
        || ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
            && (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)))
    {
        uint32 spa = 0;
        uint32 spa_mask = 0x0;

        if ((filter->match_flags & BCM_KNET_FILTER_M_INGPORT) && BCM_GPORT_IS_TRUNK(filter->m_ingport))
        {
            uint32 members_nof_bits;
            int spa_member_shift;
            uint32 trunk_id;
            int pool;
            /** Convert trunk gport to system port aggregate id */
            SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, filter->m_ingport, 0, &spa));
            trunk_id = BCM_GPORT_TRUNK_GET(filter->m_ingport);
            /** Get pool from trunk gport */
            BCM_TRUNK_ID_POOL_GET(pool, trunk_id);
            /** Mask out lag member from SPA */
            members_nof_bits = dnx_data_trunk.parameters.pool_info_get(unit, pool)->pool_hw_mode;
            spa_member_shift = 13 - members_nof_bits;
            SHR_BITSET_RANGE(&spa_mask, spa_member_shift, members_nof_bits);
            meta_data->spa = spa;
            meta_mask->spa = spa_mask;
            filter_create.filter.is_src_modport = FALSE;
            filter_create.filter.spa_unit = unit;
        }
        else
        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_gport_t gport_sysport = 0;
            dnx_algo_gpm_gport_phy_info_t gport_info;
            int spa_unit = 0;

            if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
            {
                spa_unit = unit;
                /** Get information(including system port) from given gport or local port */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(spa_unit, filter->m_ingport,
                                                                DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                                &gport_info));
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport_sysport, gport_info.sys_port);
                /** Get PP Port */
                SHR_IF_ERR_EXIT(bcm_port_get(spa_unit, filter->m_ingport, &flags, &interface_info, &mapping_info));
                filter_create.filter.is_src_modport = FALSE;
                filter_create.filter.spa_unit = unit;
            }
            else if ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
                     && (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT))
            {
                int my_modid = 0;
                int num_modid = 0;

                /** Get unit per modid */
                bcm_stk_modid_get(unit, &my_modid);
                bcm_stk_modid_count(unit, &num_modid);
                if ((filter->m_src_modid >= my_modid) && (filter->m_src_modid < (my_modid + num_modid)))
                {
                    /** modid on locol device */
                    filter_create.filter.is_src_modport = TRUE;
                    filter_create.filter.spa_unit = unit;
                    spa_unit = unit;
                }
                else
                {
                    for (spa_unit = 0; spa_unit < BCM_CONTROL_MAX; spa_unit++)
                    {
                        if (BCM_UNIT_VALID(spa_unit))
                        {
                            bcm_stk_modid_get(spa_unit, &my_modid);
                            bcm_stk_modid_count(spa_unit, &num_modid);
                            if ((filter->m_src_modid >= my_modid) && (filter->m_src_modid < (my_modid + num_modid)))
                            {
                                /** modid on remote device */
                                filter_create.filter.is_src_modport = TRUE;
                                filter_create.filter.spa_unit = spa_unit;
                                break;

                            }
                        }
                    }
                    if (spa_unit == BCM_CONTROL_MAX)
                    {
                        SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Configured modid is out of range\n");
                    }
                }

                /*
                 * Getting sysport gport from modport gport is not supported
                 * Getting sysport gport from modport port id directly
                 */
                SHR_IF_ERR_EXIT(bcm_stk_gport_sysport_get(spa_unit, filter->m_src_modport, &gport_sysport));
                /** Get PP Port */
                SHR_IF_ERR_EXIT(bcm_port_get(spa_unit, gport_sysport, &flags, &interface_info, &mapping_info));
            }

            /** Check if is member of trunk */
            if (mapping_info.pp_port == 0xFFFFFFFF)
            {
                bcm_trunk_t tid_find = 0;
                bcm_gport_t trunk_gport;
                bcm_switch_control_key_t key;
                bcm_switch_control_info_t value;
                uint32 pp_port = 0;
                bcm_trunk_pp_port_allocation_info_t allocation_info;
                int pool, group;
                trunk_group_member_info_t member_info;

                /** Get trunk id from sysport. Parameter modid is not used */
                SHR_IF_ERR_EXIT(bcm_trunk_find(spa_unit, -1, gport_sysport, &tid_find));

                /** Get header type in */
                BCM_GPORT_TRUNK_SET(trunk_gport, tid_find);
                key.index = 1;
                key.type = bcmSwitchPortHeaderType;
                SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(spa_unit, trunk_gport, key, &value));

                /*
                 * Encode SSPA with LAG member id for ETH ports
                 * Encode SSPA without LAG member id for other ports
                 */
                if (value.value == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
                {
                    /** Encode SSPA with LAG member id */
                    BCM_TRUNK_ID_GROUP_GET(group, tid_find);
                    BCM_TRUNK_ID_POOL_GET(pool, tid_find);
                    member_info.flags = 0;
                    member_info.system_port = gport_sysport;
                    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_index_in_group_find(unit, pool, group, &member_info));
                    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, member_info.index, &spa));
                }
                else
                {
                    /** Get the pp port allocation info of a given trunk */
                    SHR_IF_ERR_EXIT(bcm_trunk_pp_port_allocation_get(spa_unit, tid_find, 0, &allocation_info));
                    /** Check that trunk has a core on this member port */
                    if ((SAL_BIT(mapping_info.core) & allocation_info.core_bitmap) == 0)
                    {
                        SHR_ERR_EXIT(BCM_E_PARAM, "System Gport 0x%x is not a valid member of Trunk %d\n",
                                     gport_sysport, tid_find);
                    }
                    else
                    {
                        pp_port = allocation_info.pp_port_per_core_array[mapping_info.core];
                    }

                    /** Encode SSPA without LAG member id */
                    SHR_IF_ERR_EXIT(dnx_knet_pp_port_to_src_system_port_get
                                    (spa_unit, mapping_info.core, pp_port, &spa));
                }
                meta_data->spa = spa;
                meta_mask->spa = spa_mask;
            }
            else
            {
                meta_data->spa = (uint16) BCM_GPORT_SYSTEM_PORT_ID_GET(gport_sysport);
                meta_mask->spa = spa_mask;
            }
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_REASON)
    {
        if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonMirror))
        {
            meta_data->action_type = bcmPktDnxFtmhActionTypeInboundMirror;
            meta_mask->action_type = 0;
        }
        else if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonSampleSource))
        {
            meta_data->action_type = bcmPktDnxFtmhActionTypeSnoop;
            meta_mask->action_type = 0;
        }
        else
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "KNET: Unsupported Rx reason\n");
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE)
    {
        meta_data->trap_id = (filter->m_fp_rule >> 16) & 0xffff;
        meta_data->trap_qualifier = filter->m_fp_rule & 0xffff;
        meta_mask->trap_id = 0;
        meta_mask->trap_qualifier = 0;
        if (meta_data->trap_id)
        {
            meta_mask->trap_id = 0;
        }
        if (meta_data->trap_qualifier)
        {
            meta_mask->trap_qualifier = 0;
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_ERROR)
    {
        meta_data->error = 1;
        meta_mask->error = 0;
    }

    /*
     * Invert inverted mask
     */
    for (idx = 0; idx < oob_size; idx++)
    {
        filter_create.filter.mask.b[idx] ^= 0xff;
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_RAW)
    {
        data_offset = 0;
        for (pdx = 0; pdx < filter->raw_size; pdx++)
        {
            if (filter->m_raw_mask[pdx] != 0)
            {
                data_offset = pdx;
                break;
            }
        }
        idx = oob_size;
        for (; pdx < filter->raw_size; pdx++)
        {
            /*
             * Check for array overflow
             */
            if (idx >= KCOM_FILTER_BYTES_MAX)
            {
                return BCM_E_PARAM;
            }
            filter_create.filter.data.b[idx] = filter->m_raw_data[pdx];
            filter_create.filter.mask.b[idx] = filter->m_raw_mask[pdx];
            idx++;
        }
        filter_create.filter.pkt_data_offset = data_offset;
        filter_create.filter.pkt_data_size = filter->raw_size - data_offset;
    }

    /*
     * Dump raw data for debugging purposes
     */
    for (idx = 0; idx < BYTES2WORDS(oob_size); idx++)
    {
        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "OOB[%d]: 0x%08x [0x%08x]\n%s",
                       idx, filter_create.filter.data.w[idx], filter_create.filter.mask.w[idx], EMPTY);
    }
    for (idx = 0; idx < filter_create.filter.pkt_data_size; idx++)
    {
        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "PKT[%d]: 0x%02x [0x%02x]\n%s",
                       idx + filter_create.filter.pkt_data_offset,
                       filter_create.filter.data.b[idx + oob_size], filter_create.filter.mask.b[idx + oob_size], EMPTY);
    }

    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_create, sizeof(filter_create), sizeof(filter_create)));
    /*
     * ID is assigned by kernel
     */
    filter->id = filter_create.filter.id;
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_destroy(
    int unit,
    int filter_id)
{
    kcom_msg_filter_destroy_t filter_destroy;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    sal_memset(&filter_destroy, 0, sizeof(filter_destroy));
    filter_destroy.hdr.opcode = KCOM_M_FILTER_DESTROY;
    filter_destroy.hdr.unit = unit;
    filter_destroy.hdr.id = filter_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_destroy, sizeof(filter_destroy), sizeof(filter_destroy)));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel packet filter configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 * \param [out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_get(
    int unit,
    int filter_id,
    bcm_knet_filter_t * filter)
{
    kcom_msg_filter_get_t filter_get;
    int idx, rdx, fdx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    sal_memset(&filter_get, 0, sizeof(filter_get));
    filter_get.hdr.opcode = KCOM_M_FILTER_GET;
    filter_get.hdr.unit = unit;
    filter_get.hdr.id = filter_id;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_get, sizeof(filter_get.hdr), sizeof(filter_get)));
    bcm_knet_filter_t_init(filter);
    switch (filter_get.filter.type)
    {
        case KCOM_FILTER_T_RX_PKT:
            filter->type = BCM_KNET_FILTER_T_RX_PKT;
            break;
        default:
            /*
             * Unknown type
             */
            break;
    }

    switch (filter_get.filter.dest_type)
    {
        case KCOM_DEST_T_NETIF:
            filter->dest_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->dest_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            filter->dest_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->dest_type = BCM_KNET_DEST_T_NULL;
            break;
    }

    switch (filter_get.filter.mirror_type)
    {
        case KCOM_DEST_T_NETIF:
            filter->mirror_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            /*
             * Should never get here, but keep for completeness
             */
            filter->mirror_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->mirror_type = BCM_KNET_DEST_T_NULL;
            break;
    }

    if (filter_get.filter.flags & KCOM_FILTER_F_STRIP_TAG)
    {
        filter->flags |= BCM_KNET_FILTER_F_STRIP_TAG;
    }

    filter->dest_id = filter_get.filter.dest_id;
    filter->dest_proto = filter_get.filter.dest_proto;
    filter->mirror_id = filter_get.filter.mirror_id;
    filter->mirror_proto = filter_get.filter.mirror_proto;
    filter->id = filter_get.filter.id;
    filter->priority = filter_get.filter.priority;
    sal_memcpy(filter->desc, filter_get.filter.desc, sizeof(filter->desc) - 1);
    filter->cb_user_data = filter_get.filter.cb_user_data;

    /** There is criterion in meta data */
    if (filter_get.filter.oob_data_size)
    {
        dnx_meta_data_t *meta_data;
        dnx_meta_data_t *meta_mask;
        meta_data = (dnx_meta_data_t *) & filter_get.filter.data;
        meta_mask = (dnx_meta_data_t *) & filter_get.filter.mask;

        if (meta_mask->forward_domain)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_VLAN;
            filter->m_vlan = meta_data->forward_domain;
        }

        if (meta_mask->spa)
        {
            bcm_gport_t gport_sysport = 0;
            bcm_gport_t gport_modport = 0;
            int member = 0;
            int spa_unit = 0;
            int is_lag_member = FALSE;
            dnx_algo_gpm_gport_phy_info_t gport_info;

            /** Get unit of SPA */
            if (filter_get.filter.is_src_modport)
            {
                filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODID;
                filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODPORT;
                spa_unit = filter_get.filter.spa_unit;
            }
            else
            {
                filter->match_flags |= BCM_KNET_FILTER_M_INGPORT;
                spa_unit = filter_get.filter.spa_unit;
            }

            SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(spa_unit, meta_data->spa, &is_lag_member));

            if (is_lag_member)
            {
                uint32 flags = 0;

                if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
                {
                    /** Return trunk gport if LAG member is mask out */
                    if (meta_mask->spa != 0xffff)
                    {
                        bcm_gport_t gport_trunk = 0;
                        /** Get the trunk gport for this spa */
                        SHR_IF_ERR_EXIT(dnx_trunk_spa_to_gport(spa_unit, meta_data->spa, &member, &gport_trunk));
                        filter->m_ingport = gport_trunk;
                    }
                    else
                    {
                        /** Map SPA to its corresponding sysport gport */
                        SHR_IF_ERR_EXIT(bcm_trunk_spa_to_system_phys_port_map_get
                                        (spa_unit, flags, meta_data->spa, &gport_sysport));
                        /** Get local port from given sysport gport */
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(spa_unit, gport_sysport,
                                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                                        &gport_info));
                        filter->m_ingport = gport_info.local_port;
                    }
                }
                else
                {
                    /** Map SPA to its corresponding sysport gport */
                    SHR_IF_ERR_EXIT(bcm_trunk_spa_to_system_phys_port_map_get
                                    (spa_unit, flags, meta_data->spa, &gport_sysport));
                    /** Get modport gport from sysport gport */
                    SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_get(spa_unit, gport_sysport, &gport_modport));
                    filter->m_src_modport = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
                    filter->m_src_modid = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
                }
            }
            else
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport_sysport, meta_data->spa);
                if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
                {
                    /** Get local port from given sysport gport */
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(spa_unit, gport_sysport,
                                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                                    &gport_info));
                    filter->m_ingport = gport_info.local_port;
                }
                else
                {
                    /** Get modport gport from sysport gport */
                    SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_get(spa_unit, gport_sysport, &gport_modport));
                    filter->m_src_modport = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
                    filter->m_src_modid = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
                }
            }
        }

        filter->m_fp_rule = 0;
        if (meta_mask->trap_id)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule |= (meta_data->trap_id & 0xffff) << 16;
        }
        if (meta_mask->trap_qualifier)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule |= meta_data->trap_qualifier & 0xffff;
        }
        if (meta_mask->action_type)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_REASON;
            BCM_RX_REASON_CLEAR_ALL(filter->m_reason);
            if (meta_data->action_type == bcmPktDnxFtmhActionTypeInboundMirror)
            {
                BCM_RX_REASON_SET(filter->m_reason, bcmRxReasonMirror);
            }
            else if (meta_data->action_type == bcmPktDnxFtmhActionTypeSnoop)
            {
                BCM_RX_REASON_SET(filter->m_reason, bcmRxReasonSampleSource);
            }
            else
            {
                SHR_ERR_EXIT(BCM_E_INTERNAL, "KNET: Unsupported rx reason type\n");
            }
        }
        if (meta_mask->error)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_ERROR;
        }
    }

    if (filter_get.filter.pkt_data_size)
    {
        filter->match_flags |= BCM_KNET_FILTER_M_RAW;
        rdx = filter_get.filter.pkt_data_offset;
        fdx = filter_get.filter.oob_data_size;
        for (idx = 0; idx < filter_get.filter.pkt_data_size; idx++)
        {
            filter->m_raw_data[rdx] = filter_get.filter.data.b[fdx];
            filter->m_raw_mask[rdx] = filter_get.filter.mask.b[fdx];
            rdx++;
            fdx++;
        }
        filter->raw_size = rdx;
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel packet filter objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [in] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_traverse(
    int unit,
    bcm_knet_filter_traverse_cb trav_fn,
    void *user_data)
{
    int idx;
    bcm_knet_filter_t filter;
    kcom_msg_filter_list_t filter_list;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "KNET: trav_fn is NULL\n");
    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_list, sizeof(filter_list.hdr), sizeof(filter_list)));
    for (idx = 0; idx < filter_list.fcnt; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_get(unit, filter_list.id[idx], &filter));
        SHR_IF_ERR_EXIT(trav_fn(unit, &filter, user_data));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_cleanup(
    int unit)
{
    int idx;
    kcom_msg_filter_list_t filter_list;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;
    SHR_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_list, sizeof(filter_list.hdr), sizeof(filter_list)));
    for (idx = 0; idx < filter_list.fcnt; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_destroy(unit, filter_list.id[idx]));
    }
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_init(
    int unit)
{
    bcm_knet_filter_t filter;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_knet_cleanup(unit));
    bcm_knet_filter_t_init(&filter);
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
    filter.priority = 255;
    sal_strncpy(filter.desc, "DefaultRxAPI", KCOM_FILTER_DESC_MAX);
    SHR_IF_ERR_EXIT(bcm_dnx_knet_filter_create(unit, &filter));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
#else
/**
 * \brief Create a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_create(
    int unit,
    bcm_knet_netif_t * netif)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_create is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel network interface.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_destroy(
    int unit,
    int netif_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_destroy is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel network interface configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] netif_id - Network interface ID
 * \param [out] netif - Network interface configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_get(
    int unit,
    int netif_id,
    bcm_knet_netif_t * netif)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_get is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel network interface objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [out] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_netif_traverse(
    int unit,
    bcm_knet_netif_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_netif_traverse is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Create a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_create(
    int unit,
    bcm_knet_filter_t * filter)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_create is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Destroy a kernel packet filter.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_destroy(
    int unit,
    int filter_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_destroy is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get a kernel packet filter configuration.
 *
 * \param [in] unit - Relevant unit
 * \param [in] filter_id - Rx packet filter ID
 * \param [out] filter - Rx packet filter configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_get(
    int unit,
    int filter_id,
    bcm_knet_filter_t * filter)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_get is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverse kernel packet filter objects.
 *
 * \param [in] unit - Relevant unit
 * \param [in] trav_fn - User provided call back function
 * \param [in] user_data - User provided data used as input param for callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_filter_traverse(
    int unit,
    bcm_knet_filter_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_filter_traverse is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Clean up the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_cleanup(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_cleanup is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the kernel networking subsystem.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_knet_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "bcm_knet_init is unavailable\n");
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

#endif
