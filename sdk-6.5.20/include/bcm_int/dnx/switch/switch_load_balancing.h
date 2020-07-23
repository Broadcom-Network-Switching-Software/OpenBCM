/**
 * \file bcm_int/dnx/switch/switch_load_balancing.h
 *
 * Load balancing definitions, for DNX, mainly related to BCM APIs 
 *
 * Purpose:
 *   Definitions for logical and physical LB-related operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_LOAD_BALANCING_H_INCLUDED
/* { */
#define SWITCH_LOAD_BALANCING_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <bcm/switch.h>
/*
 * }
 */

/*
 * Macros
 * {
 */
/*
 * If IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED is set to a non-zero value then it is
 * assumed that IPPE_PRSE_LOAD_BALANCING_SEED has been returned to the registers file.
 *
 * Actually, when this happens, remove this 'define' altogether.
 */
#define IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED 0
/*
 * Set maximal limit on the number of characters on identifiers, in string form, within
 * the enumeration 'bcm_switch_control_t', not including the 'bcmSwitch' prefix.
 */
#define SWITCH_CONTROL_NAME_LIMIT 30
/*
 * }
 */
/*
 * Typedefs
 * {
 */
/**
 * Used to indicate the 'DESTINATION' field on LOAD_BALANCING_CRC_SELECTION tcam.
 */
typedef uint32 dnx_switch_ld_destination_t;
/**
 * Used to indicate the 'OUTLIF_0' field on LOAD_BALANCING_CRC_SELECTION tcam.
 */
typedef uint32 dnx_switch_lb_outlif_0_t;
/**
 * Used to indicate the 'VALID' field on LOAD_BALANCING_CRC_SELECTION tcam
 */
typedef uint32 dnx_switch_lb_valid_t;
/*
 * }
 */
/*
 * Enums
 * {
 */
/**
 * \brief -
 *   Enumeration of the various general crc seeds. To be used
 *   on BCM-level get/set.
 * \see
 *   * dnx_switch_lb_general_seeds_t
 *   * bcm_dnx_switch_control_indexed_set() (for 'bcmSwitchHashSeed')
 */
typedef enum
{
    FIRST_GENERAL_SEEDS = 0,
    SEED_FOR_MPLS_STACK_0 = FIRST_GENERAL_SEEDS,
    SEED_FOR_MPLS_STACK_1,
    SEED_FOR_PARSER,
    MUM_GENERAL_SEEDS
} dnx_switch_lb_general_seeds_e;

/**
 * Enum that lists all valid DNX hash CRC functions
 */
typedef enum
{
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1015D,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x100D7,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10039,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10AC5,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x109E7,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x10939,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x12105,
    DNX_SWITCH_LB_HASH_CONFIG_CRC16_0x1203D,
    DNX_SWITCH_LB_HASH_CONFIG_COUNT
} dnx_switch_lb_hash_config_e;

/**
 * Enum that lists all possible values for symmetrical hashing.
 */
typedef enum
{
    /** The symmetrical fields of the header are hashed non-symmetrically */
    DNX_SWITCH_LB_HASH_NON_SYMMETRICAL,
    /** The symmetrical fields of the header are hashed symmetrically */
    DNX_SWITCH_LB_HASH_SYMMETRICAL,
    /** The symmetrical fields of the header are disabled from hashing */
    DNX_SWITCH_LB_HASH_NO_SYMMETRICAL_FIELD,
} dnx_switch_lb_hash_symmetrical_e;

/*
 * }
 */
/*
 * Structures
 * {
 */
/**
 * \brief -
 *   LOAD_BALANCING_CRC_SELECTION tcam key fields
 *   To be used for loading a new entry or for searching
 *   for existing entry.
 */
typedef struct
{
    /**
     * 'Destination' field (21 bits. See "destination" field type in field_types_definition.xml)
     */
    dnx_switch_ld_destination_t lb_destination;
    /**
     * 'Destination_mask' field (21 bits. See "destination" field type in field_types_definition.xml)
     */
    dnx_switch_ld_destination_t lb_destination_mask;
    /**
     * 'Outlif_0' field (22 bits. See "glob_out_lif" field type in field_types_definition.xml)
     */
    dnx_switch_lb_outlif_0_t outlif_0;
    /**
     * 'Outlif_0_mask' field (22 bits. See "glob_out_lif" field type in field_types_definition.xml)
     */
    dnx_switch_lb_outlif_0_t outlif_0_mask;
} dnx_switch_lb_tcam_key_t;
/**
 * \brief -
 *   LOAD_BALANCING_CRC_SELECTION tcam result fields
 *   To be used for loading a new entry or for reading
 *   an existing entry.
 */
typedef struct
{
    /**
     * 'CLIENT_x_CRC_16_SEL' field (3 bits. See "destination" field type in field_types_definition.xml)
     * repeated 5 times, one per client.
     */
    uint32 client_x_crc_16_sel[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS];
} dnx_switch_lb_tcam_result_t;
/**
 * \brief -
 *   LOAD_BALANCING_RESERVED_LABELS register fields
 *   To be used for loading the various controls on how to
 *   handle reserved MPLS labels and the first label following a reserved
 *   label.
 * \remark -
 *   This register is only loaded at initialization stage.
 */
typedef struct
{
    /**
     * Bitmap. Only the LS 16 bits count.
     * There are 16 MPLS reserved labels.
     * Each bit indicates whether corresponding reserved label is to be
     * included into load balancing calculation.
     */
    uint16 reserved_current_label_indication;
    /**
     * Bitmap. Only the LS 16 bits count.
     * There are 16 MPLS reserved labels.
     * Each bit indicates whether corresponding reserved label is to be
     * forced into load balancing calculation even if it was terminated.
     */
    uint16 reserved_current_label_force_layer;
    /**
     * Bitmap. Only the LS 16 bits count.
     * There are 16 MPLS reserved labels.
     * Each bit indicates whether label following corresponding reserved label
     * is to be forced into load balancing calculation even if it was terminated.
     */
    uint16 reserved_next_label_indication;
    /**
     * Bitmap. Only the LS 16 bits count.
     * There are 16 MPLS reserved labels.
     * This field is redundant and must be set the same as 'reserved_next_label_indication'.
     */
    uint16 reserved_next_label_valid;
} dnx_switch_lb_mpls_reserved_t;
/**
 * \brief -
 *   IPPB_MPLS_LABEL_CRC_32_SEEDS register fields
 *   and
 *   IPPE_PRSE_LOAD_BALANCING_SEED register fields
 *   To be used for loading the seeds for crc functions used by
 *   the two MPLS stacks and the parser. Each is 32-bits wide.
 * \remark -
 *   None
 */
typedef struct
{
    /**
     * 32-bits seed for crc to be used by the first MPLS stack.
     */
    uint32 mpls_stack_0;
    /**
     * 32-bits seed for crc to be used by the second MPLS stack.
     */
    uint32 mpls_stack_1;
    /**
     * 32-bits seed for crc to be used by the second MPLS stack(terminated layer).Only for supported devices.
     */
    uint32 mpls_stack_2;
    /**
     * 32-bits seed for crc to be used by the second MPLS stack(unterminated layer).Only for supported devices.
     */
    uint32 mpls_stack_3;
    /**
     * 32-bits seed for crc to be used by the parser.
     */
    uint32 parser;
} dnx_switch_lb_general_seeds_t;
/**
 * \brief -
 *   MPLS_LAYER_IDENTIFICATION_BITMAP_CFG register fields
 *   To be used for loading the identification of MPLS
 *   protocols. To be used by FWD hw.
 * \remark -
 *   This register is only loaded at initialization stage.
 * \see -
 *   dbal_enum_value_field_current_protocol_type_e
 *   DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS
 */
typedef struct
{
    /**
     * Bitmap of 32 bits.
     * Contains the MPLS protocols identification (Each protocol has a five-bits number).
     * The bits, which are set, identify MPLS protocols to FWD hw.
     */
    uint32 mpls_identification;
} dnx_switch_lb_mpls_identification_t;

/**
 * \brief -
 *   Structure used as input to:
 *     dnx_switch_load_balancing_lsms_crc_select_set
 *     dnx_switch_load_balancing_lsms_crc_select_get
 *   Used for set/get on IPPB_LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP register fields
 * \remark -
 *   Note that the 'lb_selection_bit_map' element is used
 *   both as input (on 'set') and as output (on 'get').
 * \see -
 *   IPPB_LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP
 *   DBAL_TABLE_LOAD_BALANCING_CLIET_FWD_BIT_SEL
 */
typedef struct
{
    /*
     * Identifier of load balancing client for which 'selection_bit_map' is
     * to be set (or for 'get' operation).
     */
    uint8 lb_client;
    /**
     * Value of selection bitmap to 'load into' (or 'retrieve from') hw
     * register. The selection bitmap is an 8 bits value (one per layer record).
     */
    uint16 lb_selection_bit_map;
} dnx_switch_lb_lsms_crc_select_t;

/*
 * }
 */
/*
 * Prototypes
 * {
 */
/**
 * \brief - Initialize LOAD_BALANCING module (under SWITCH). \n
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_switch_load_balancing_module_init(
    int unit);

/**
 * \brief
 *   Given physical identifier of load-balancing client (as per IPPB_LOAD_BALANCING_CRC_SELECTION_CAM),
 *   get the corresponding logical client identifier (from bcm_switch_control_t).
 *   Currently, there are 5 clients:
 *     bcmSwitchECMPHashConfig                    (Phys. LB client 0)
 *     bcmSwitchECMPSecondHierHashConfig          (Phys. LB client 1)
 *     bcmSwitchECMPThirdHierHashConfig           (Phys. LB client 2)
 *     bcmSwitchTrunkHashConfig                   (Phys. LB client 3)
 *     bcmSwitchNwtworkHashConfig                 (Phys. LB client 4)
 *   Physical-logical correspondence is stored in 'data'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] physical_client -
 *   Physical identifier of LB client, as used in IPPB_LOAD_BALANCING_CRC_SELECTION_CAM.
 * \param [out] logical_client_p -
 *   Pointer to element of enum of type bcm_switch_control_t. This procedure loads pointed memory
 *   by the logical identifier of the client. High level users relate to logical identifiers only.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None
 * \see
 *   * bcm_switch_control_t
 *   * bcm_dnx_switch_control_indexed_set
 */
shr_error_e dnx_switch_lb_physical_client_to_logical(
    int unit,
    uint32 physical_client,
    bcm_switch_control_t * logical_client_p);
/**
 * \brief
 *   Given logical identifier of load-balancing client (from bcm_switch_control_t),
 *   get the corresponding physical client identifier (as per IPPB_LOAD_BALANCING_CRC_SELECTION_CAM).
 *   Currently, there are 5 clients:
 *     bcmSwitchECMPHashConfig                    (Phys. LB client 0)
 *     bcmSwitchECMPSecondHierHashConfig          (Phys. LB client 1)
 *     bcmSwitchECMPThirdHierHashConfig           (Phys. LB client 2)
 *     bcmSwitchTrunkHashConfig                   (Phys. LB client 3)
 *     bcmSwitchNwtworkHashConfig                 (Phys. LB client 4)
 *   Physical-logical correspondence is stored in 'data'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] logical_client_in -
 *   Logical identifier of LB client, element of enum of type bcm_switch_control_t.
 *   High level users relate to logical identifiers only.
 * \param [out] physical_client_p -
 *   Pointer to uint32. This procedure loads pointed memory by the physical identifier,
 *   (as used in IPPB_LOAD_BALANCING_CRC_SELECTION_CAM) of the client.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None
 * \see
 *   * bcm_switch_control_t
 *   * bcm_dnx_switch_control_indexed_set
 */
shr_error_e dnx_switch_lb_logical_client_to_physical(
    int unit,
    bcm_switch_control_t logical_client_in,
    uint32 *physical_client_p);
/**
 * \brief
 *   Given 'switch_control' enumeration (as per bcm_switch_control_t),
 *   get the corresponding string.
 *   Note that we are using, in DNX, a definition from BCM. This is because
 *   bcm_switch_control_t is also considered a DNX enumeration (to avoid doubling
 *   it).
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] switch_control -
 *   Numerical identifier within the enumerator 'bcm_switch_control_t'.
 * \param [out] name_p -
 *   Pointer to pointer to string. This procedure loads pointed memory by the
 *   pointer to the string that is the identifier of 'switch_control' as taken
 *   from 'bcm_switch_control_t'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None
 * \see
 *   * bcm_switch_control_t
 *   * dnx_switch_load_balancing_tcam_info_set
 */
shr_error_e dnx_switch_control_t_name(
    int unit,
    bcm_switch_control_t switch_control,
    char **name_p);
/**
 * \brief
 *   Given string representation enumeration 'bcm_switch_control_t',
 *   get the corresponding numeric value. On the input string, the prefix "bcmSwitch"
 *   is assumed to be omitted.
 *   Note that we are using, in DNX, a definition from BCM. This is because
 *   bcm_switch_control_t is also considered a DNX enumeration (to avoid doubling
 *   it).
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] switch_control_name_p -
 *   String identifier within the enumerator 'bcm_switch_control_t'.
 * \param [out] value_p -
 *   This procedure loads pointed memory by numeric value corresponding to input
 *   string identifier.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * bcm_switch_control_t
 *   * appl_dnx_load_balancing_starter
 */
shr_error_e dnx_switch_control_t_value(
    int unit,
    char *switch_control_name_p,
    uint32 *value_p);
/**
 * \brief
 *   Initialize TCAM logical key structure of type 'dnx_switch_lb_tcam_key_t'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [out] tcam_key_p -
 *   Pointer to structure of type 'dnx_switch_lb_tcam_key_t'. This procedure loads
 *   pointed structure by default values (all 0)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_switch_lb_tcam_key_t_init(
    int unit,
    dnx_switch_lb_tcam_key_t * tcam_key_p);
/**
 * \brief
 *   Initialize TCAM logical result structure of type 'dnx_switch_lb_tcam_result_t'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [out] tcam_result_p -
 *   Pointer to structure of type 'dnx_switch_lb_tcam_result_t'. This procedure loads
 *   pointed structure by default values (all 0)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_switch_lb_tcam_result_t_init(
    int unit,
    dnx_switch_lb_tcam_result_t * tcam_result_p);
/**
 * \brief
 *   Set (write to HW) the single field of MPLS_LAYER_IDENTIFICATION_BITMAP_CFG table.
 *   This field relates to identifying MPLS protocols to FWD hw.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] mpls_identification_p -
 *   Pointer to structure of type dnx_switch_lb_mpls_identification_t which
 *   contains the single field 'MPLS_BIT_IDENTIFIER'. It is a bitmaps of 32 bits.
 *   The bits set will indicate, to HW, the identification of MPLS protocols.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_mpls_identification_t
 *   * dnx_switch_load_balancing_mpls_identification_get
 *   * dbal_enum_value_field_current_protocol_type_e
 */
shr_error_e dnx_switch_load_balancing_mpls_identification_set(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p);
/**
 * \brief
 *   Get (read from HW) the single field of MPLS_LAYER_IDENTIFICATION_BITMAP_CFG table.
 *   This field relates to identifying MPLS protocols to ingress FWD hw
 *   bllock.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] mpls_identification_p -
 *   Pointer to structure of type dnx_switch_lb_mpls_identification_t. This
 *   procedure loads the single field 'MPLS_BIT_IDENTIFIER' into its place. It
 *   is a bitmaps of 32 bits. The bits set will indicate, to HW, the
 *   identification of MPLS protocols.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_mpls_identification_t
 *   * dnx_switch_load_balancing_mpls_identification_set
 *   * dbal_enum_value_field_current_protocol_type_e
 */
shr_error_e dnx_switch_load_balancing_mpls_identification_get(
    int unit,
    dnx_switch_lb_mpls_identification_t * mpls_identification_p);
/**
 * \brief
 *   Set (write to HW) all fields of LOAD_BALANCING_GENERAL_SEEDS table.
 *   All fields relate to load-balancing handling of general crc seeds
 *   for MPLS stack 0 and 1 and for parser.
 *   Essentially, one single register is involved.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] general_seeds_p -
 *   Pointer to structure of type dnx_switch_lb_general_seeds_t which contains
 *   all 3 fields to load. Each field is of 32 bits.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   For now, IPPE_PRSE_LOAD_BALANCING_SEED has not been included in the
 *   register file and, therefore, is not accessed.
 *   See IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED.
 * \see
 *   * dnx_switch_lb_general_seeds_t
 *   * dnx_switch_load_balancing_general_seeds_get
 *   * IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED
 */
shr_error_e dnx_switch_load_balancing_general_seeds_set(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p);
/**
 * \brief
 *   Get (read from HW) all fields of LOAD_BALANCING_GENERAL_SEEDS table.
 *   All fields relate to load-balancing handling of general crc seeds
 *   for MPLS stack 0 and 1 and for parser.
 *   Essentially, one single register is involved.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] general_seeds_p -
 *   Pointer to structure of type dnx_switch_lb_general_seeds_t which is
 *   loaded, by this procedure, by all 3 fields to load. Each field is
 *   of 32 bits.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   For now, IPPE_PRSE_LOAD_BALANCING_SEED has not been included in the
 *   register file and, therefore, is not accessed.
 *   See IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED.
 * \see
 *   * dnx_switch_lb_general_seeds_t
 *   * dnx_switch_load_balancing_general_seeds_set
 *   * IPPE_PRSE_LOAD_BALANCING_SEED_IS_INCLUDED
 */
shr_error_e dnx_switch_load_balancing_general_seeds_get(
    int unit,
    dnx_switch_lb_general_seeds_t * general_seeds_p);
/**
 * \brief
 *   Set (write to HW) all fields of LOAD_BALANCING_RESERVED_LABELS table.
 *   All fields relate to load-balancing handling of reserved MPLS labels.
 *   Essentially, one single register is involved.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] mpls_reserved_p -
 *   Pointer to structure of type dnx_switch_lb_mpls_reserved_t which contains
 *   all 4 fields to load. Each field is a bitmaps of 16 bits.
 *   Note that third and fourth elements must be equal.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_mpls_reserved_t
 *   * dnx_switch_load_balancing_mpls_reserved_label_get
 */
shr_error_e dnx_switch_load_balancing_mpls_reserved_label_set(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p);
/**
 * \brief
 *   Get (read from HW) all fields of LOAD_BALANCING_RESERVED_LABELS table.
 *   All fields relate to load-balancing handling of reserved MPLS labels.
 *   Essentially, one single register is involved.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] mpls_reserved_p -
 *   Pointer to structure of type dnx_switch_lb_mpls_reserved_t. This procedure
 *   loads it with all 4 fields . Each field is a bitmaps of 16 bits.
 *   Note that third and fourth elements should be equal.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_mpls_reserved_t
 *   * dnx_switch_load_balancing_mpls_reserved_label_set
 */
shr_error_e dnx_switch_load_balancing_mpls_reserved_label_get(
    int unit,
    dnx_switch_lb_mpls_reserved_t * mpls_reserved_p);

/**
 * \brief
 *   Convert between a CRC enumeration value into the matching HW value.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] crc_enum_value -
 *   The CRC enumeration value
 * \param [out] hw_val -
 *   The matching HW value of the incoming enumeration value.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_switch_load_balancing_crc_function_enum_to_hw_get(
    int unit,
    bcm_switch_hash_config_t crc_enum_value,
    uint32 *hw_val);

/**
 * \brief
 *   Convert between a CRC HW value into the matching enumeration value .
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] hw_val -
 *   The CRC function HW value
 * \param [out] crc_enum_value -
 *   The matching enumeration value of the incoming HW value.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_switch_load_balancing_crc_function_hw_to_enum_get(
    int unit,
    uint32 hw_val,
    bcm_switch_hash_config_t * crc_enum_value);

/**
 * \brief
 *   Set (write to HW) a LOAD_BALANCING_CRC_SELECTION TCAM entry.
 *   Caller indicated which entry to load and, also, the key, mask, data
 *   and 'valid' indication.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] index -
 *   TCAM index (entry ID)
 * \param [in] tcam_key_p -
 *   Pointer to structure. LOAD_BALANCING_CRC_SELECTION TCAM key ('destination', 'outlif_0', ...)
 * \param [in] tcam_result_p -
 *   Pointer to structure. LOAD_BALANCING_CRC_SELECTION TCAM result (Array of
 *   'crc selection', one per client).
 *   Note that each of the clients must have a unique crc select value. For example,
 *   if clinet '0' selects crc function '4' then no other client may use crc function '4'.
 * \param [in] tcam_valid -
 *   LOAD_BALANCING_CRC_SELECTION TCAM valid indication. If caller
 *   sets this input to '0' then entry becomes invalid (erased)
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   This table, just like all other tables, in DBAL, is initialized to 'all 0'
 *   on standard power up sequence. So, at initialization, all tcam entries are
 *   configured to 'invalid'
 * \see
 *   * bcm_switch_control_t
 *   * bcm_dnx_switch_control_indexed_set
 */
shr_error_e dnx_switch_load_balancing_tcam_info_set(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t tcam_valid);
/**
 * \brief
 *   Get (write to HW) a LOAD_BALANCING_CRC_SELECTION TCAM entry.
 *   Caller indicated which entry to get. Retrievd info is: key, mask, result
 *   and 'valid' indication.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] index -
 *   TCAM index (entry ID)
 * \param [out] tcam_key_p -
 *   Pointer to structure of type dnx_switch_lb_tcam_key_t. This procedure
 *   load structure by LOAD_BALANCING_CRC_SELECTION TCAM key ('destination', 'outlif_0, ...)
 * \param [out] tcam_result_p -
 *   Pointer to structure of type dnx_switch_lb_tcam_result_t. This procedure
 *   load structure by LOAD_BALANCING_CRC_SELECTION TCAM result (Array of 'crc selection',
 *   one per client)
 * \param [out] tcam_valid_p -
 *   Pointer to dnx_switch_lb_valid_t. This procedure loads pointed memory by
 *   LOAD_BALANCING_CRC_SELECTION TCAM valid indication
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * bcm_switch_control_t
 *   * bcm_dnx_switch_control_indexed_set
 */
shr_error_e dnx_switch_load_balancing_tcam_info_get(
    int unit,
    int index,
    dnx_switch_lb_tcam_key_t * tcam_key_p,
    dnx_switch_lb_tcam_result_t * tcam_result_p,
    dnx_switch_lb_valid_t * tcam_valid_p);

/**
 * \brief
 *   Set (write to HW) selected crc seed on LOAD_BALANCING_CLIET_FWD_BIT_SEL table.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] lsms_crc_select_p -
 *   Pointer to structure of type dnx_switch_lb_lsms_crc_select_t which contains
 *   the identifier of the selected client (lb_client) and
 *   the bit-map itself, used for selection of MS/LS bits for hashing (lb_selection_bit_map).
 *   Identifier must be smaller than the number of supported clients (5).
 *   Bit map may have bits set only from BIT(0) to BIT(7) (incl.). All other bits
 *   must be set to zero. See dnx_data_switch.load_balancing.nof_layer_records_from_parser
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_lsms_crc_select_t
 *   * dnx_switch_load_balancing_lsms_crc_select_get
 *   * dnx_data_switch.load_balancing.nof_layer_records_from_parser
 *   * dnx_data_switch.load_balancing.nof_lb_clients
 *   * IPPB_LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP
 *   * LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP_N
 */
shr_error_e dnx_switch_load_balancing_lsms_crc_select_set(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p);
/**
 * \brief
 *   Get (Read from HW) selected crc seed on LOAD_BALANCING_CLIET_FWD_BIT_SEL table.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] lsms_crc_select_p -
 *   Pointer to structure of type dnx_switch_lb_lsms_crc_select_t which contains
 *   the identifier of the selected client (lb_client)
 *   as input, and container to get the corresponding bit-map itself, used for
 *   selection of MS/LS bits for hashing (lb_selection_bit_map),
 *   as output.
 *   Identifier must be smaller than the number of supported clients (5).
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_lsms_crc_select_t
 *   * dnx_switch_load_balancing_lsms_crc_select_set
 *   * dnx_data_switch.load_balancing.nof_layer_records_from_parser
 *   * dnx_data_switch.load_balancing.nof_lb_clients
 */
shr_error_e dnx_switch_load_balancing_lsms_crc_select_get(
    int unit,
    dnx_switch_lb_lsms_crc_select_t * lsms_crc_select_p);
/**
 * \brief
 *   Set (write to HW) selected crc seed on LOAD_BALANCING_CRC_SEEDS table.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] crc_hw_function_index -
 *   The CRC function index in the HW of the seed that is about to be updated.
 * \param [in] lb_crc_seed -
 *   The new seed value of the provided CRC function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_crc_seed_t
 *   * dnx_switch_load_balancing_crc_seed_get
 */
shr_error_e dnx_switch_load_balancing_crc_seed_set(
    int unit,
    uint32 crc_hw_function_index,
    uint16 lb_crc_seed);
/**
 * \brief
 *   Get (read from HW) selected crc seed on LOAD_BALANCING_CRC_SEEDS table.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] crc_hw_function_index -
 *   The CRC function index in the HW of the seed that is about to be received.
 * \param [out] lb_crc_seed -
 *   The seed of the requested CRC function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   * dnx_switch_lb_crc_seed_t
 *   * dnx_switch_load_balancing_crc_seed_set
 */
shr_error_e dnx_switch_load_balancing_crc_seed_get(
    int unit,
    uint32 crc_hw_function_index,
    uint16 *lb_crc_seed);

/**
 * \brief
 *   Set a nibble (that follow an MPLS BOS) to speculate a protocol.
 * \param [in] unit -
 *   The unit number.
 * \param [in] nibble -
 *   The nibble to set the speculation protocol to.
 * \param [in] protocol -
 *   The speculative protocol.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 */
shr_error_e dnx_switch_load_balancing_nibble_speculation_set(
    int unit,
    int nibble,
    bcm_switch_mpls_next_protocol_t protocol);
/**
 * \brief
 *   get a nibble (that follow an MPLS BOS) speculate a protocol.
 * \param [in] unit -
 *   The unit number.
 * \param [in] nibble -
 *   The nibble value that is speculated by.
 * \param [out] protocol -
 *   The speculative protocol.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   None
 * \see
 */
shr_error_e dnx_switch_load_balancing_nibble_speculation_get(
    int unit,
    int nibble,
    bcm_switch_mpls_next_protocol_t * protocol);

/**
 * \brief - Set the header fields that can be part of the construction of an LB key
 * according to the user masking option.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] type - The type of header to be masked.
 * \param [in] mask - The mask of the selected header
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_switch_control_load_balancing_hash_fields_masking_set(
    int unit,
    bcm_switch_control_t type,
    uint32 mask);

/**
 * \brief - return the mask option that determine the header fields that can be part of the construction of an LB key.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] type - The type of header to be masked.
 * \param [out] mask - The mask of the selected header
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_switch_control_load_balancing_hash_fields_masking_get(
    int unit,
    bcm_switch_control_t type,
    uint32 *mask);

/*
 * }
 */
/* } */
#endif /* SWITCH_LOAD_BALANCING_H_INCLUDED */
