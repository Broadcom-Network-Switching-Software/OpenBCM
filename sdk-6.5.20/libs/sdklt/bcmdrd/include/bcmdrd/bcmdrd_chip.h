/*! \file bcmdrd_chip.h
 *
 * DRD APIs that operate on the chip_info structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_CHIP_H
#define BCMDRD_CHIP_H

#include <bcmdrd_config.h>

#include <bcmdrd/bcmdrd_symbols.h>

/*! Check if unit number is within the DRD supported range. */
#define BCMDRD_UNIT_VALID(_u) ((unsigned int)(_u) < BCMDRD_CONFIG_MAX_UNITS)

/*!
 * \name Common chip flags.
 * \anchor BCMDRD_CHIP_FLAG_xxx
 */

/*! \{ */

/*! Indicates that the chip is a variant SKU, i.e. not a base device SKU. */
#define BCMDRD_CHIP_FLAG_VARIANT 0x1

/*! Mask of common chip flags. */
#define BCMDRD_CHIP_FLAG_MASK 0xffff

/*! \} */

/*!
 * \brief Block information structure.
 *
 * This structure will be auto-generated for each physical block in
 * the device.
 */
typedef struct bcmdrd_block_s {

    /*! Block type */
    int type;

    /*! Physical block number */
    int blknum;

    /*! Port Bitmaps */
    bcmdrd_pbmp_t pbmps;

} bcmdrd_block_t;

/*!
 * \brief Port-block information structure.
 *
 * This structure is used to extract detailed port and block
 * information ofr a given port and block type.
 */
typedef struct bcmdrd_pblk_s {

    /*! Block type */
    int type;

    /*! Block type instance */
    int inst;

    /*! Physical Block Number */
    int blknum;

    /*! Port number within this block */
    int lane;

} bcmdrd_pblk_t;

/*!
 * \brief Address calculation function for registers and memories.
 *
 * Used to override the default address calculation for a particular
 * Host Management Interface (HMI).
 *
 * A typical use case is transitional devices, which use a new CMIC
 * register interface, but retain the S-channel protocol of the
 * previous CMIC version. Subsequent devices will also use a new
 * S-channel protocol, so the address calculation of the transitional
 * device(s) must be treated as an exception.
 *
 * \param [in] block Block number.
 * \param [in] port Port number for block context.
 * \param [in] offset Base address for register/memory.
 * \param [in] idx Entry number for array-based register/memory.
 *
 * \return Lower 32-bit of register/memory address.
 */
typedef uint32_t (*bcmdrd_block_port_addr_f)(int block, int port,
                                             uint32_t offset, uint32_t idx);

/*!
 * \brief Valid array indexes for a set of ports.
 *
 * This structure is a building block for constructing a complete
 * definition of which indexes are valid for port-based register
 * arrays.
 *
 * Please refer to \ref bcmdrd_numel_info_t for details.
 */
typedef struct bcmdrd_numel_range_s {

    /*! Array index minimum. */
    int min;

    /*! Array index maximum. */
    int max;

    /*! Ports for which index range is valid. */
    bcmdrd_pbmp_t pbmp;

} bcmdrd_numel_range_t;

/*!
 * \brief Set of valid port/index ranges for a register.
 *
 * A set of valid port/index ranges is referred to as an
 * encoding. Several registers may share the same encoding.
 *
 * Please refer to \ref bcmdrd_numel_info_t for details.
 */
typedef struct bcmdrd_numel_encoding_s {

    /*! List of range IDs, -1 marks end of list. */
    int range_id[8];

} bcmdrd_numel_encoding_t;

/*!
 * \brief Information for register arrays with per-port variable size.
 *
 * Since register arrays often have identical or similar per-port
 * arrays sizes, this information is stored as shared encodings
 * defined by two tables per chip.
 *
 * Each entry in the first table defines an index range and an
 * associated set of ports. Entries in this table are referenced by a
 * range ID which is simply the table index (first entry is range
 * with ID 0, etc.)
 *
 * Each entry in the second table is a list of range IDs that defines
 * the encoding for one or more registers.
 *
 * An encoding is defined as an index into the second table. Note that
 * encoding 0 is reserved because the first entry of the second table
 * contains the size of the table itself.
 */
typedef struct bcmdrd_numel_info_s {

    /*! Table of all index ranges for this chip. */
    bcmdrd_numel_range_t *chip_ranges;

    /*! Table of register array encodings for this chip. */
    bcmdrd_numel_encoding_t *encodings;

} bcmdrd_numel_info_t;

/*!
 * \brief Chip parameter.
 *
 * Chip parameters are used to describe differences between a base
 * device and a device variant (SKU).
 *
 * The parameter names are device-specific in nature, but different
 * device types should ideally use the same name for the same device
 * variant attribute.
 */
typedef struct bcmdrd_chip_param_s {

    /*! Chip parameter name. */
    const char *name;

    /*! Chip parameter value. */
    uint32_t value;

} bcmdrd_chip_param_t;

/*!
 * \brief Chip override value.
 *
 * Chip override values are used to override default values for a
 * given device variant.
 *
 * Overrides are typically used for reduced SOC memory sizes, but they
 * can also be used to mark a given symbol as non-existent in a given
 * device variant.
 */
typedef struct bcmdrd_chip_override_s {

    /*! Indicates whether a given override value is active or not. */
    bool override;

    /*! Value to be used instead of the default value. */
    uint32_t value;

} bcmdrd_chip_override_t;

/*!
 * \brief Chip modifier object.
 *
 * The chip modifier object is used to override one or more attributes
 * of a device symbol (register/memory).
 *
 * Each override attribute has a flag to indicate whether the override
 * is active or not.
 */
typedef struct bcmdrd_chip_mod_s {

    /*! Symbol ID. */
    bcmdrd_sid_t sid;

    /*! Optionally mark symbol as invalid (non-existent). */
    bcmdrd_chip_override_t valid;

    /*! Override default SOC memory size. */
    bcmdrd_chip_override_t index_max;

} bcmdrd_chip_mod_t;

/*!
 * \brief Chip profile.
 *
 * The chip profile describes how a device variant (SKU) differs from
 * the base device.
 *
 * Note that the list of valid ports for a given variant is not part
 * of the chip profile, since it is part of the fixed chip information
 * for each device variant.
 */
typedef struct bcmdrd_chip_profile_s {

    /*! Chip parameters. */
    bcmdrd_chip_param_t *params;

    /*! Symbol modifiers. */
    bcmdrd_chip_mod_t *mods;

} bcmdrd_chip_profile_t;

/*! Information for query multi-pipe XGS devices. */
typedef struct bcmdrd_pipe_info_s {

    /*! Base offset (fixed address information). */
    uint32_t offset;

    /*! Access type (device-specific). */
    uint32_t acctype;

    /*! Block type (device-specific). */
    uint32_t blktype;

    /*! Base index (interpreted according to base type). */
    int baseidx;

    /*! Port number (required when querying for pipe index). */
    int port;

    /*! Pipe instance (required when querying for sub-pipe map). */
    int pipe_inst;

    /*! Sub-pipe instance (required when querying for sub-pipe map). */
    int sub_pipe_inst;

    /*! Port number domain (required when querying for pipe index). */
    bcmdrd_port_num_domain_t pnd;

    /*! Valid pipe map of the device (required when querying for pipe map). */
    uint32_t pipe_map;

    /*! Memory index (required when querying for memories index validation). */
    int memidx;

    /*!
     * Maximum memory index (required when querying for memories index
     * validation).
     */
    int memidx_max;

} bcmdrd_pipe_info_t;

/*! Specified Key of information query of multi-pipe XGS devices. */
typedef enum bcmdrd_pipe_info_type_e {

    /*!
     * Query pipe instance bit map of access type 'unique'
     * for specified block type.
     */
    BCMDRD_PIPE_INFO_TYPE_PMASK,

    /*! Query number of base type instances for specified block type. */
    BCMDRD_PIPE_INFO_TYPE_LINST,

    /*!
     * Query section size as log2(n) for specified block type and
     * basetype, e.g. 3==>8, 5 ==>32. Exception is 0 ==> 0.
     */
    BCMDRD_PIPE_INFO_TYPE_SECT_SHFT,

    /*!
     * Query pipe instance bit map of access type 'unique' for the
     * specified base index.
     */
    BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK,

    /*!
     * Query pipe instance bit map of access type 'single' for the
     * specified base index. This information is needed when the
     * basetype instances are partially existing in the half-chip mode.
     */
    BCMDRD_PIPE_INFO_TYPE_BASETYPE_SINGLE_PMASK,

    /*! Query global access type value for pipes with access type 'unique'. */
    BCMDRD_PIPE_INFO_TYPE_AT_UNIQUE_GLOBAL,

    /*! Query if a specified base index is invalid. */
    BCMDRD_PIPE_INFO_TYPE_BASEIDX_INVALID,

    /*! Query the number of pipe instances for the specified block type. */
    BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST,

    /*! Query the pipe index for the specified port. */
    BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT,

    /*! Query the pipe map of block from device. */
    BCMDRD_PIPE_INFO_TYPE_BLK_PIPE_MAP,

    /*!
     * Query the physical device pipe map associated with the specified
     * pipe instance and sub-pipe instance.
     */
    BCMDRD_PIPE_INFO_TYPE_SUB_PIPE_MAP,

    /*! Query if a specified memory index is invalid. */
    BCMDRD_PIPE_INFO_TYPE_MEMIDX_INVALID,

} bcmdrd_pipe_info_type_t;

/*!
 * \brief SBUS Address decoding function.
 *
 * \param [in] symbol Candidate symbol for address decoding.
 * \param [in] block Block number.
 * \param [in] addr Chip-specific SBUS address beat.
 * \param [in] idx Entry number for array-based register/memory.
 * \param [in] lane Port number for block context.
 *
 * \retval 0 on success, -1 on failure.
 */
typedef int (*bcmdrd_addr_decode_f)(const bcmdrd_symbol_t *symbol,
                                    int block, uint32_t addr,
                                    uint32_t *idx, uint32_t *lane);

/*!
 * \brief Fixed chip information.
 */
typedef struct bcmdrd_chip_info_s {

    /*! Optional register file digest. */
    const char *signature;

    /*! HMI protocol (typically S-bus address format version). */
    int hmi_proto;

    /*! HMI block (typically CMIC block number). */
    int hmi_block;

    /*! Number other (non-CMIC) block types. */
    int nblktypes;

    /*! Other (non-CMIC) block types. */
    const char **blktype_names;

    /*!
     * Index of first block of given block type in the blocks array
     * below. This can be used to optimize block search performance
     * for devices with many (port) blocks.
     */
    const int *blktype_start_idx;

    /*! Number of block structures. */
    int nblocks;

    /*! Block structure array. */
    const bcmdrd_block_t *blocks;

    /*! Valid ports for this chip. */
    bcmdrd_pbmp_t valid_pbmps[BCMDRD_PND_COUNT];

    /*!
     * Chip Flags.
     *
     * Lower 16 bits are generic flags (see \ref BCMDRD_CHIP_FLAG_xxx).
     * Upper 16 bits are chip-specific, e.g. for controlling bond-options.
     */
    uint32_t flags;

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1
    /*! Chip symbol table pointer. */
    const bcmdrd_symbols_t *symbols;
#endif

    /*! Variable size register arrays. */
    bcmdrd_numel_info_t *numel_info;

    /*! Symbol overrides and other configuration attributes. */
    bcmdrd_chip_profile_t *profile;

    /*! Offset/Address Vectors. */
    bcmdrd_block_port_addr_f block_port_addr;

    /*! Decode Address to symbol information */
    bcmdrd_addr_decode_f addr_decode;

    /*! Get port number domain for a register/memory. */
    bcmdrd_port_num_domain_t (*port_num_domain)(bcmdrd_sid_t sid,  int blktype);

    /*! Get device-specific block type from genenraic port type. */
    int (*blktype_from_porttype)(bcmdrd_port_type_t ptype);

    /*! Get pipe info of this chip. */
    uint32_t (*pipe_info)(const struct bcmdrd_chip_info_s *cinfo,
                          bcmdrd_pipe_info_t *pi,
                          bcmdrd_pipe_info_type_t pi_type);

} bcmdrd_chip_info_t;

/*!
 * \brief Calculate port block information.
 *
 * This function is used to calculated the port block information
 * (\ref bcmdrd_pblk_t) based on the top-level port number and the
 * block type.
 *
 * The input is typically retrieved from the DRD along with a dynamic
 * top-level port number, and the output is the parameters needed to
 * perform the associated CMIC S-channel access (block number and port
 * block lane number).
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] port Top-level port number.
 * \param [in] blktype Device-specific block type (GPORT, XLPORT, etc.).
 * \param [out] pblk Port block information.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_port_block(const bcmdrd_chip_info_t *cinfo,
                       int port, int blktype, bcmdrd_pblk_t *pblk);

/*!
 * \brief Calculate top-level port number.
 *
 * This function is used to get the top-level port number from
 * block number and the port number within the block.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blknum Device-specififc block number.
 * \param [in] lane Port number within this block.
 *
 * \return Top-level port number if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_port_number(const bcmdrd_chip_info_t *cinfo, int blknum, int lane);

/*!
 * \brief Get block structure from type and instance.
 *
 * Given a block type and an instance number of that block type, this
 * function will return the corresponding block structure \ref bcmdrd_block_t.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [in] blkinst Instance number for block type.
 *
 * \return Pointer to block structure, or NULL if no matching block is found.
 */
const bcmdrd_block_t *
bcmdrd_chip_block(const bcmdrd_chip_info_t *cinfo, int blktype, int blkinst);

/*!
 * \brief Get block number from type and instance.
 *
 * Given a block type and an instance number of that block type, this
 * function will return the corresponding physical block number.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [in] blkinst Instance number for block type.
 *
 * \return Physical block number, or -1 if no matching block is found.
 */
extern int
bcmdrd_chip_block_number(const bcmdrd_chip_info_t *cinfo,
                         int blktype, int blkinst);

/*!
 * \brief Get port bitmap for a block type.
 *
 * Given a block type, return a port bitmap (\ref bcmdrd_pbmp_t) of
 * all ports belonging to blocks of this type.
 *
 * Typically used to get a list of ports of a particular type,
 * e.g. XLPORT ports.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blktype Block type.
 * \param [out] pbmp Destination port bitmap.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_blktype_pbmp(const bcmdrd_chip_info_t *cinfo, int blktype,
                         bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get block type and block instance from block number.
 *
 * This function calculates block type and block instance from a given
 * physical block number. If \c blktype is not NULL, the calculated
 * block type will be set to \c blktype. If \c blkinst is not NULL, the
 * calculated block instance to be set to \c blklinst.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] blknum Physical block number.
 * \param [out] blktype Calculated block type.
 * \param [out] blkinst Calculated instance number for block type.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcmdrd_chip_block_type(const bcmdrd_chip_info_t *cinfo, int blknum,
                       int *blktype, int *blkinst);

/*!
 * \brief Get port number domain for a register/memory.
 *
 * Port-based registers and memories use different port number domains
 * in their physical address. For example, some registers use the
 * physical port number, some registers use the logical port number
 * and some use a MMU port number.
 *
 * If no exception is found for this particular symbol, then the
 * default domain for the block type will be returned.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Symbol enum value.
 * \param [in] blktype Block type for this symbol.
 *
 * \return Port number domain.
 */
extern bcmdrd_port_num_domain_t
bcmdrd_chip_port_num_domain(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                            int blktype);

/*!
 * \brief Get the bitmap of valid ports for a given port number domain.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pnd Port number domain.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_valid_pbmp(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pipe_no Switch pipeline index.
 * \param [in] pnd Port number domain.
 * \param [out] pbmp Bitmap of ports associated with the specified pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_pipe_pbmp(const bcmdrd_chip_info_t *cinfo, int pipe_no,
                      bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Check if a port index is valid in the specified port number domain.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pnd Port number domain.
 * \param [in] port Port index to check.
 *
 * \retval true Port is valid.
 * \retval false Port is not valid.
 */
extern bool
bcmdrd_chip_port_valid(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, int port);

/*!
 * \brief Get the bitmap of valid ports for a device-specific symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Symbol enum value.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_chip_port_reg_pbmp(const bcmdrd_chip_info_t *cinfo,
                          bcmdrd_sid_t sid, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Check if port index is valid for a device-specific symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Symbol enum value.
 * \param [in] port Port index to check.
 * \param [in] regidx Index of variable register array to validate.
 *
 * \retval true Port is valid.
 * \retval false Port is not valid.
 */
extern bool
bcmdrd_chip_port_reg_valid(const bcmdrd_chip_info_t *cinfo,
                           bcmdrd_sid_t sid, int port, int regidx);

/*!
 * \brief Get adjusted maximum index of a specific memory.
 *
 * This function returns adjusted maximum index of a specific memory.
 * Some device variant have smaller memories than the base device, and
 * this function can be used to retrieve such override values. If no
 * override information is found, then the input \c maxidx is
 * returned.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Symbol ID of the specific memory.
 * \param [in] maxidx Default maximum index of the specific memory.
 *
 * \return Maximum index of a specific memory.
 */
extern uint32_t
bcmdrd_chip_mem_maxidx(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                       uint32_t maxidx);

/*!
 * \brief Check if symbol is marked as invalid.
 *
 * For some device variants (SKUs) some registers and memories may
 * have been disabled.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Symbol ID to check.
 *
 * \retval true Symbol is not marked as invalid.
 * \retval false Symbol is marked as invalid.
 */
extern bool
bcmdrd_chip_sym_valid(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid);

/*!
 * \brief Get chip parameter.
 *
 * Get device-specific device property value.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] prm_name Parameter name.
 * \param [out] prm_val Parameter value.
 *
 * \retval 0 No errors.
 * \retval -1 Not found.
 */
extern int
bcmdrd_chip_param_get(const bcmdrd_chip_info_t *cinfo,
                      const char *prm_name, uint32_t *prm_val);

/*!
 * \brief Get next supported chip parameter.
 *
 * Device variants may implement device-specific chip parameters
 * whenever a device variant property differs from the base device.
 *
 * Pass in NULL to get the first supported parameter. Pass in the name
 * of the first supported parameter to get the second parameter and so
 * forth.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] prm_name Parameter name.
 *
 * \return Name of next parameter or NULL if no more parameters.
 */
extern const char *
bcmdrd_chip_param_next(const bcmdrd_chip_info_t *cinfo,
                       const char *prm_name);

/*!
 * \brief Validate index for per-port variable register array.
 *
 * This function chekes if the index of specified per-port variable
 * register array is valid. The maximum index of the variable array
 * may be stored in different ways in DRD symbol. The possible encoded
 * values are retrieved from DRD symbol and pass through argument
 * \c encoding and regidx_max for validation.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] port Top-level port number.
 * \param [in] regidx Index of variable register array to validate.
 * \param [in] encoding Encoding type from index word.
 * \param [in] regidx_max real maximum table index based on raw values and flags.
 *
 * \return 1 if index is valid in variable register arrary, otherwise 0.
 */
extern int
bcmdrd_chip_reg_index_valid(const bcmdrd_chip_info_t *cinfo, int port,
                            int regidx, int encoding, int regidx_max);

/*!
 * \brief Get pipe information for multi-pipe device.
 *
 * It is assumed that the caller of this function is familiar with the
 * S-channel access mechanism for multi-pipe XGS designs, including
 * the concepts of access type and base type.
 *
 * This function will return different information based on different
 * query key type \c pi_type(\ref bcmdrd_pipe_info_type_t).
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] pi Device-specific parameters to query related pipe information.
 * \param [in] pi_type Query key for specific pipe information.
 *
 * \return Specific pipe information based on query key.
 */
extern uint32_t
bcmdrd_chip_pipe_info(const bcmdrd_chip_info_t *cinfo,
                      bcmdrd_pipe_info_t *pi, bcmdrd_pipe_info_type_t pi_type);

/*!
 * \brief Get chip modifier object for a given symbol.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Device-specific symbol ID.
 *
 * \return Pointer to chip modifier object or NULL if not found.
 */
extern bcmdrd_chip_mod_t *
bcmdrd_chip_mod_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid);

/*!
 * \brief Get access type of a device-specific symbol ID.
 *
 * This function returns the device-specific access type of a specified
 * symbol ID.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Device-specific Symbol ID.
 *
 * \return Access type value or -1 on failure.
 */
extern int
bcmdrd_chip_acctype_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid);

/*!
 * \brief Get block type of a device-specific symbol ID.
 *
 * This function returns the device-specific block type of a specified
 * symbol ID by passing 0 as the \c blktype. If the symbol contains multiple
 * block types, the function can be called repeatedly by passing
 * the previously returned block type value plus one until -1 is returned.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] sid Device-specific Symbol ID.
 * \param [in] blktype The initial blktype value to be searched.
 *
 * \return Block type or -1 on failure.
 */
extern int
bcmdrd_chip_blktype_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                        int blktype);

/*!
 * \brief Get device-specific block type by block type name.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] name The name for the block type.
 *
 * \return Block type or -1 if not found.
 */
extern int
bcmdrd_chip_blktype_get_by_name(const bcmdrd_chip_info_t *cinfo,
                                const char *name);

/*!
 * \brief Get block structure that matches the specified block types.
 *
 * The \c info is a 32-bit word with encoded information of block types.
 * This 32-bit word can be retrieved from the symbol table or from the
 * pre-defined value in header files.
 * This function returns the device-specific block structure by passing
 * NULL as the \c pblk. Calling this function repeatedly by passing the
 * previously returned block structure pointer plus one until NULL is returned
 * can find all the blocks that match the specified block types within
 * the device.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] info Device-specific block types to match.
 * \param [in] pblk The initial pointer to the block structure to be searched.
 *
 * \return Pointer to the matched block structure or NULL on failure.
 */
extern const bcmdrd_block_t *
bcmdrd_chip_block_types_match(const bcmdrd_chip_info_t *cinfo,
                              uint32_t info, const bcmdrd_block_t *pblk);

/*!
 * \brief Get port bitmap of the specified port type.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] cinfo Pointer to chip information structure.
 * \param [in] ptype The specified port type.
 * \param [out] pbmp Bitmap of ports.
 *
 * \return 0 No errors.
 * \retval -1 Feature is not supported.
 */
extern int
bcmdrd_chip_port_type_pbmp(const bcmdrd_chip_info_t *cinfo,
                           bcmdrd_port_type_t ptype, bcmdrd_pbmp_t *pbmp);

#endif /* BCMDRD_CHIP_H */
