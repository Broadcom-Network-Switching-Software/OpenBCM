/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file lif_mngr_api.h
 * 
 *
 * This file contains the public APIs required for lif algorithms.
 *
 */

#ifndef  INCLUDE_LIF_MNGR_API_H_INCLUDED
#define  INCLUDE_LIF_MNGR_API_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/**
 * \brief This flag is used to indicate that the allocated global lif should have a specific ID.
 *
 * \see  dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_GLOBAL_LIF_WITH_ID   SAL_BIT(0)

/**
 * \brief This flag is used to indicate that no global lif is required for this allocation.
 *
 * \see dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF SAL_BIT(1)

/**
 * \brief This flag is used to allocate an asymmetric global lif, but use an ID that is available on both
 * ingress and egress, so it could be followed with allocating the second side.
 * The second side must be allocated WITH_ID before any other lif is allocated, or it's no longer guaranteed to
 * be reserved.
 *
 * \see dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF SAL_BIT(2)

/**
 * \brief
 * If a global lif is used for AC or PWE, then the vlan and mpls ports don't have enough bits to support the full address
 *   of global lif. Use this flag to indicate that a smaller address should be used.
 */
#define LIF_MNGR_L2_GPORT_GLOBAL_LIF   (SAL_BIT(26))

/**
 * \brief
 * For QoS and PHP backward comptability, PHP Out-LIF will use a dedicated range.
 */
#define LIF_MNGR_PHP_GPORT_GLOBAL_LIF   (SAL_BIT(27))

/**
 *  \brief Illegal lif indication.
 *
 *  \see dnx_lif_mngr_lif_free
 */
#define LIF_MNGR_INVALID             (-1)

/**
 *  \brief Illegal inlif indication.
 *
 */
#define LIF_MNGR_INLIF_INVALID             0

/**
 * Flags for lif mngr APIs, internal and external.
 *
 * \see
 *   lif_mapping_local_to_global_get
 *   lif_mapping_global_to_local_get
 *   dnx_algo_lif_mapping_create
 *   dnx_algo_lif_mapping_remove
 *   dnx_algo_global_lif_allocation_allocate
 *   dnx_algo_global_lif_allocation_free
 *  {
 */
/**
 * Perform ingress lif operation.
 */
#define DNX_ALGO_LIF_INGRESS             SAL_BIT(0)
/**
 * Perform egress lif operation.
 */
#define DNX_ALGO_LIF_EGRESS              SAL_BIT(1)

/*
 * lif table manager flags
 */
#define LIF_TABLE_MNGR_LIF_INFO_REPLACE            SAL_BIT(0)
/*
 * Egress: GLEM & SW mapping update is not required
 * Ingress: SW global to local mapping update is not required
 */
#define LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING        SAL_BIT(1)

/*
 * Internal flag, used for testing only.
 * When this flag is set - in case of a replaced lif, which consumes more entries then the original lif,
 * And there's no ll in the new lif (was on the original) - don't fail allocation
 */
#define LIF_TABLE_MNGR_LIF_INFO_OUTLIF_REPLACE_ALLOW_LL_MISMATCH        SAL_BIT(2)

/**
 *********************************************
 lif table manager info table specific flags
 *********************************************
 */

/*
 * AC VLAN-Port
 *
 */

#define DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P           SAL_BIT(0)
/** EEDB_OUT_AC allow ESEM access  */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_AC_EGRESS_NON_ESEM_BUSTER           SAL_BIT(1)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_EXTENDED_P2P             SAL_BIT(2)

/*
 * MPLS
 */
/** Number of special labels, used for JR2_A0 labels reordering */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL       SAL_BIT(0)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS      SAL_BIT(1)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS    SAL_BIT(2)
/** TANDEM indication */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM                  SAL_BIT(3)
/** Tunnel is PHP */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP                     SAL_BIT(4)
/** IML tunnel (EVPN) */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML                     SAL_BIT(5)
/** 2 labels are used */
#define DNX_ALGO_LIF_TABLE_MPLS_2_LABELS                               SAL_BIT(6)
/** IOAM in dual hom */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IOAM_DUAL_HOM           SAL_BIT(7)
/** number of MPLS data labels used */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_1_DATA_LABEL    SAL_BIT(7)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_2_DATA_LABELS    SAL_BIT(8)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_3_DATA_LABELS    SAL_BIT(9)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_4_DATA_LABELS    SAL_BIT(10)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_5_DATA_LABELS    SAL_BIT(11)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_6_DATA_LABELS    SAL_BIT(12)

/*
 * ARP
 */
/** VLAN translation indication (ARP+AC entry) */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION               SAL_BIT(0)

/*
 * IPV4/IPV6/ERSPAN/RSPAN tunnel
 */
/** GRE 4/8/12 GENEVE tunnels */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_4                       SAL_BIT(0)
/** raw ipv4 tunnel   */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_4                        SAL_BIT(1)
/** UDP ipv4 tunnel   */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_UDP                                SAL_BIT(2)
/** VXLAN/VXLAN-GPE ipv4 tunnel   */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE                          SAL_BIT(3)
/** raw ipv6 tunnel    */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_IP_ANY_IN_6                        SAL_BIT(4)
/** GRE 4/8/12 GENEVE tunnels */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_GRE_ANY_IN_6                       SAL_BIT(5)
/** UDP ipv6 tunnel   */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_UDP_6                              SAL_BIT(6)
/** VXLAN/VXLAN-GPE ipv6 tunnel   */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_GPE_6                        SAL_BIT(7)
/** erspan tunnel */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_ERSPAN                             SAL_BIT(8)
/** rspan tunnel */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_RSPAN                              SAL_BIT(9)
/** srv6 IP tunnel */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_SRV6_IP                            SAL_BIT(10)
/** IPv4 Vxlan GPE or Ipv6 Vxlan tunnel with ECN eligible - useless for J2P and above*/
#define DNX_ALGO_LIF_TABLE_SPECIFIC_VXLAN_ECN                          SAL_BIT(11)
/** IPv4 BFD SIP */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_BFD_EXTRA_SIP                      SAL_BIT(12)
/** IPv4 Copy Tunnel */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_COPY_TUNNEL                        SAL_BIT(13)
/** Double UDP tunnel */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_DOUBLE_UDP                         SAL_BIT(14)
/** IPv6 High Scale Flag */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_IPV6_SCALE                         SAL_BIT(15)
/** IPV6 GENEVE */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_GENEVE_6                           SAL_BIT(16)

/*
 * OAM
 */
/** Used to signaling UP MEP(second pass) */
#define DNX_ALGO_LIF_TABLE_OAM_UP_MEP       SAL_BIT(0)
/**
 * }
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */
/**
 * \brief Local Out-LIF SW State iterator MACROs in order to traverse over the entire
 * range of the local Out-LIFs.
 * DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_START - Initiate an iterator of type (SW_STATE_HASH_TABLE_ITER *)
 * DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_IS_END - Validate the end of the traverse
 * DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT - Retrieve the next
 *      local Out-LIF by supplying an iterator, local_out_lif
 *      pointer and returned info (egress_lif_info_t *)
 */
#define DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_START(iter) \
{ \
    SW_STATE_HASH_TABLE_ITER_SET_BEGIN(iter); \
}
#define DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_IS_END(iter)   (SW_STATE_HASH_TABLE_ITER_IS_END(iter))
#define DNX_LIF_MNGR_OUTLIF_SW_INFO_ITER_NEXT(unit, iter, local_outlif_id, egress_lif_info) \
{ \
    SHR_IF_ERR_EXIT(lif_mngr_db.local_lif_info.egress_lif_htb.get_next(unit, iter, local_outlif_id, egress_lif_info)); \
}

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/**
 * \brief Allocation info for local inlif allocation.
 *
 * This struct is used by several different lif_mngr APIs (see below), and some of the fields have different
 * meanings in each. The general description for each field is provided here, and the individual
 * APIs hightlight the fields that are use differently.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 * dnx_lif_mngr_lif_free
 */
typedef struct
{
    /**
     * Dbal table id. This, in combination with the core id, will determine
     * which inlif table to access. This field must be set in all APIs that use this struct.
     */
    dbal_tables_e dbal_table_id;
    /**
     * Dbal result type. This, in combination with the dbal table id, will give us the dbal entry size,
     * which will determine how many inlif indexes need to be allocated.
     * This argument is an integer instead of an enum because result types are using several different enums.
     * This field is ignored in all APIs other than dnx_lif_mngr_lif_allocate.
     */
    int dbal_result_type;
    /**
     * The core in which the inlif is used.
     * There are two types of inlif tables: shared by cores, and duplicated per core.
     * In the first case, 'core_id' should be set to _SHR_CORE_ALL.
     * In the second case, 'core_id' identifies the specific tables. _SHR_CORE_ALL can be used to allocate the
     * inlif on all cores, and core number can be used to allocate on a specific core. After the allocation, all operations
     * using this inlif must be used with the same core indication.
     */
    int core_id;
    /**
     *  Flags for local lif allocation.
     */
    uint32 local_lif_flags;
    /**
    * FOR INTERNAL LIF MNGR USE ONLY!
    * If the internal REPLACE/RESERVE flags are used, this holds the old result type.
    */
    uint32 old_result_type;
    /**
     *  The local inlif ID. This can be input or output, depending on the API.
     */
    int local_inlif;
} lif_mngr_local_inlif_info_t;

typedef struct
{
    /**
     * byte to add, extra argument for global lif allocation default should be GLOBAL_LIF_BTA_DEFAULT
     */
    int bta;

    /**
     * outlif's interface - the desirable intf configure
     * default value is all interfaces, setting GLOBAL_LIF_INTF_DISABLED means that the outlif_intf is disabled
     */
    uint32 outlif_intf;

    /**
     *  The global lif ID. This can be input or output, depending on the API.
     */
    int global_lif;

} lif_mngr_global_lif_info_t;

/* \brief Allocation info for local inlif allocation using string */
typedef struct
{
    char *table_name;
    char *result_type_name;
    int core_id;
    int local_inlif;
} lif_mngr_local_inlif_str_info_t;

/**
 * \brief This enum represents the legal eedb phases, it's just here so they won't be meaningless numbers spread
 *          throughout the code.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 */
typedef enum
{
    LIF_MNGR_OUTLIF_PHASE_INVALID = -1,
    LIF_MNGR_OUTLIF_PHASE_FIRST,
    LIF_MNGR_OUTLIF_PHASE_AC = LIF_MNGR_OUTLIF_PHASE_FIRST,
    LIF_MNGR_OUTLIF_PHASE_NATIVE_AC,
    LIF_MNGR_OUTLIF_PHASE_ARP,
    LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4,
    LIF_MNGR_OUTLIF_PHASE_SRV6_BASE,
    LIF_MNGR_OUTLIF_PHASE_IPV6_RAW_SRV6_TUNNEL,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_2,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4,
    LIF_MNGR_OUTLIF_PHASE_VPLS_1,
    LIF_MNGR_OUTLIF_PHASE_VPLS_2,
    LIF_MNGR_OUTLIF_PHASE_RIF,
    LIF_MNGR_OUTLIF_PHASE_SFLOW,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_3,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_4,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_5,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_6,
    LIF_MNGR_OUTLIF_PHASE_REFLECTOR,
    LIF_MNGR_OUTLIF_PHASE_RCH,
    LIF_MNGR_OUTLIF_PHASE_RSPAN,
    /*
     * The explicit phases are mapped directly to logical phases 1-8 in HW.
     * They should only be used by the bare metal SDK.
     */
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7,
    LIF_MNGR_OUTLIF_PHASE_EXPLICIT_8,
    LIF_MNGR_OUTLIF_PHASE_LAST = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_8,
    LIF_MNGR_OUTLIF_PHASE_COUNT
} lif_mngr_outlif_phase_e;

/**
 * \brief The logical phases for local outlif.
 */
typedef enum
{
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID = -1,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1 = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_3,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_4,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_5,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_6,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_7,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_8,
    DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT
} dnx_algo_local_outlif_logical_phase_e;

/**
 * \brief Allocation info for local outlif allocation.
 *
 * Pass this struct to dnx_lif_mngr_lif_allocate to allocate a local lif with an egress side.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 */
typedef struct
{
    /**
     * Dbal table id. This, will determine which eedb phase to use.
     * This field must be set in all APIs that use this struct.
     */
    dbal_tables_e dbal_table_id;
    /**
     * Dbal result type. This, in combination with the dbal table id, will give us the dbal entry size,
     * which will determine how many outlif indexes need to be allocated.
     * This argument is an integer instead of an enum because result types are using several different enums.
     * This field is ignored in all APIs other than dnx_lif_mngr_lif_allocate.
     */
    uint32 dbal_result_type;
    /**
     * Logical Phase. The outlifs are divided to phases according to 
     * the dbal table id. However, the user may wish to 
     * overridethis* value manually. Set this value to 0 to use the
     * default phase. Any other value will override the default
     * phase.
     */
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    /**
     *  Flags for local lif allocation.
     */
    uint32 local_lif_flags;
    /**
     * FOR INTERNAL LIF MNGR USE ONLY!
     * If the internal REPLACE flag is used, this holds the old result type.
     * If the internal RESERVE flag is used, this holds the temporary result type.
     */
    uint32 alternative_result_type;
    /**
     *  Local outlif id that was allocated.
     */
    int local_outlif;
    /**
     *  Fixed size for the entry.
     */
    uint32 fixed_entry_size;
} lif_mngr_local_outlif_info_t;

/* \brief Allocation info for local inlif allocation using string */
typedef struct
{
    char *table_name;
    char *result_type_name;
    char *outlif_phase_name;
    uint32 local_lif_flags;
    int local_outlif;
} lif_mngr_local_outlif_str_info_t;

/**
 * \brief Info for local outlif creation.
 * Includes information that enables manager to decide on optimal result type,
 * allocate the local lif and fill LIF table.
 *
 * /see
 * dnx_lif_table_mngr_allocate_local_lif_and_set_hw
 */
typedef struct
{
    /**
     * Flags LIF_TABLE_MNGR_LIF_INFO_XXX
     */
    uint32 flags;
    /**
     * In case of outlif - the requested logical phase
     */
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 table_specific_flags;
    /**
     * Global lif in case global to local mapping has to be updated
     * Not relevant if LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set
     */
    uint32 global_lif;
    /**
     * User can request to reserve specific size for lif entry to avoid performance issues when replacing entry with larger result type
     * default: 0 (don't reserve)
     */
    uint32 entry_size_to_allocate;
} lif_table_mngr_outlif_info_t;

/**
 * \brief Info for local inlif creation.
 * Includes information that enables manager to decide on optimal result type,
 * allocate the local lif and fill LIF table.
 *
 * /see
 * dnx_lif_table_mngr_allocate_local_lif_and_set_hw
 */
typedef struct
{
    /**
     * Flags LIF_TABLE_MNGR_LIF_INFO_XXX
     */
    uint32 flags;
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 table_specific_flags;
    /**
     * Global lif in case global to local mapping has to be updated
     * Not relevant if LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set
     */
    uint32 global_lif;
    /**
     * User can request to reserve specific size for lif entry to avoid performance issues when replacing entry with larger result type
     * default: 0 (don't reserve)
     */
    uint32 entry_size_to_allocate;
} lif_table_mngr_inlif_info_t;

/** default BTA value  */
#define GLOBAL_LIF_BTA_DEFAULT      0

/** disabled INTF value */
#define GLOBAL_LIF_INTF_DISABLED    (-1)

/** struct that consist the global lif allocation info */
/* when adding new param or updating the struct, need also to update the following functions:
 * dnx_global_lif_tag_set, dnx_global_lif_tag_max_val_get, dnx_algo_global_out_lif_alloc_info_get and dnx_global_lif_tag_params_cmd
 */
typedef struct
{
    /**
     * estimated byte to remove for the global LIF default is GLOBAL_LIF_BTA_DEFAULT 
     */
    uint32 bta;

    /**
     * outlif's interface - in case of multiple GLEM interfaces supported (dnx data three_intf_glem_get),
     * user can select the desirable intf configure
     * default value is all interfaces, setting GLOBAL_LIF_INTF_DISABLED means that the outlif_intf is disabled
     */
    uint32 outlif_intf;

    /**
     * related outlif profile group
     */
    int outlif_group;

} global_lif_alloc_info_t;

/** struct that consist the global lif allocation info */
typedef struct
{
    /**
     * estimated byte to remove for the global LIF default is GLOBAL_LIF_BTA_DEFAULT
     */
    global_lif_alloc_info_t global_lif_alloc;

    /**
     * output parameter that mentions if the is the first reference of the global lif bank
     */
    uint8 first_reference;

    /**
     * when set, ignore the tag when allocating the global lif
     */
    uint8 ignore_tag;
} global_lif_internal_alloc_info_t;

/** struct that holds information needed for creating the Global lif resource managers */
typedef struct
{
    /**
     * indication the tag maximum size, if max size is 0, we do not have a tag
     */
    uint32 tag_max_size;

    /**
     * indication if need to set RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG
     */
    uint8 ignore_tag;

    /**
     * indication if need setting the hw is required
     */
    uint8 hw_set_required;

} global_lif_alloc_create_info_t;

/** Struct used for global -- local lifs mapping */
typedef struct
{
    /**
     * Relevant local lif
     */
    int local_lif;
    /**
     * Relevant core id
     */
    int core_id;
    /**
     * Relevant physical table. Only relevant for inlif
     */
    dbal_physical_tables_e phy_table;
} lif_mapping_local_lif_key_t;

/** Struct used lif mapping payload */
typedef struct
{
    /**
     * Local lifs array
     */
    inlif_array_t local_lifs_array;
    /**
     * Relevant physical table. Only relevant for inlif
     */
    dbal_physical_tables_e phy_table;
    /**
     * Whether the allocation is on all cores. Only relevant for inlif
     */
    uint8 is_symmetric;
} lif_mapping_local_lif_payload_t;

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/*
 * GLOBAL LIF ALLOCATION FUNCTIONS - START
 * {
 */

/**
 * \brief
 *   Allocate a global lif. this function also setting HW configurations that are related to the allocation such as BTA, the acctual global LIF HW set is done outside of this function
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] allocation_flags -
 *   Flags for the resource manager. Currently only supports \ref DNX_ALGO_RES_ALLOCATE_WITH_ID.
 * \param [in] direction_flags -
 *   Ingress or egress indication:
 *   \ref DNX_ALGO_LIF_INGRESS and/or \ref DNX_ALGO_LIF_EGRESS
 * \param [in] alloc_info -   allocation info, extra parameters to allocate the LIF.
 * \param [in] global_lif -
 *   Int pointer to memory to write output into. \n
 *   \b As \b output - \n
 *     This procedure loads pointed memory with the global lif allocated.
 *   \b As \b input - \n
 *     If allocation flag DNX_ALGO_RES_ALLOCATE_WITH_ID is set, this holds
 *       the id to be allocated.
 *
 * \return DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   dnx_lif_mngr_lif_allocate
 */
shr_error_e dnx_algo_global_lif_allocation_allocate(
    int unit,
    uint32 allocation_flags,
    uint32 direction_flags,
    global_lif_alloc_info_t * alloc_info,
    int *global_lif);

/**
 * \brief
 *   Free the allocated global lif.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] direction_flags -
 *   Ingress or egress indication. They must match the direction flags given at creation.
 *   \ref DNX_ALGO_LIF_INGRESS and/or \ref DNX_ALGO_LIF_EGRESS
 * \param [in] global_lif -
 *   Global lif to be freed.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   dnx_lif_mngr_lif_free
 */
shr_error_e dnx_algo_global_lif_allocation_free(
    int unit,
    uint32 direction_flags,
    int global_lif);

/**
 * }
 * GLOBAL LIF ALLOCATION FUNCTIONS - END
 */

/**
 * LIF MAPPING FUNCTIONS - START
 * {
 */

/**
 * \brief
 *   Create mapping between the given local lif and global lif.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     Global lif to be mapped.
 *   \param [in] local_lif_payload -
 *     Info of the local lif to be mapped.
 *   \param [in] is_validate -
 *     Indication whether to perform validations. 
 * \par INDIRECT INPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are updated with the mapping.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_create(
    int unit,
    uint32 flags,
    int global_lif,
    lif_mapping_local_lif_payload_t * local_lif_payload,
    int is_validate);

/**
 * \brief
 *   Remove the mapping between the given local lif and global lif.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     Global lif to be unmapped.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   * retval Zero if no error was detected
 *   * retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are updated with the deleted mapping.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_remove(
    int unit,
    uint32 flags,
    int global_lif);

/**
 * \brief
 *   Remove the mapping between the given local inlif and global lif.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] local_lif_info -
 *     Local lifs info array
 * \remark
 *   None.
 */
shr_error_e dnx_algo_inlif_mapping_remove_by_local_lif(
    int unit,
    lif_mapping_local_lif_info_t * local_lif_info);

/**
 * \brief
 *   Get the local lif mapped to this global lif.
 *
 * The structure lif_mapping_local_lif_info_t is defined in:
 * include/soc/dnx/swstate/auto_generated/types/lif_mngr_types.h
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     The global lif whose mapping we require.
    \param [out] local_lif_payload -
 *       This procedure loads pointed memory with the local lif info mapped
 *       from the given global lif.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local lif maps in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   * retval  \ref BCM_E_NOT_FOUND if the global lif's mapping doesn't exist.
 *   * retval  Zero if no error was detected
 *   * retval  Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *local_lif -\n
 *     See DIRECT INPUT above
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_global_to_local_get(
    int unit,
    int flags,
    int global_lif,
    lif_mapping_local_lif_payload_t * local_lif_payload);

/**
 * }
 * LIF MAPPING FUNCTIONS - END
 */

/* LIF MNGR CINT exported API - START */

shr_error_e dnx_cint_lif_lib_allocate(
    int unit,
    uint32 flags,
    lif_mngr_global_lif_info_t * global_lif_info,
    lif_mngr_local_inlif_str_info_t * inlif_str_info,
    lif_mngr_local_outlif_str_info_t * outlif_str_info);

/* LIF MNGR CINT exported API - END */

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_API_H_INCLUDED */
