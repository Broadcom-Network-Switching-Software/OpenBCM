/*! \file bcmdrd_ser.h
 *
 * Low level SER interface to retrieve the SER data stored in DRD.
 *
 * The low level SER data are stored using a profile concept. Each memory
 * with SER support maps to an SER profile descriptor which encodes
 * the offset of the corresponding SER information profile and the offset(s)
 * of the SER enable type(s) used in the SER profile(s).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_SER_H
#define BCMDRD_SER_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Restricted Memory Resource ID (enum).
 *
 * The Restricted Memory Resources (RMRs) are internal buffers that
 * can not be accessed directly from the host CPU. In order to manage the
 * SER protection for the RMRs, each RMR is assigned a device-specific ID.
 */
typedef bcmdrd_id_t bcmdrd_ser_rmr_id_t;

/*!
 * \brief SER types.
 *
 * The SER type is used to select the appropriate error detection and
 * correction mechanism for a given memory resource.
 */
typedef enum bcmdrd_ser_type_e {

    /*! Undefined. */
    BCMDRD_SER_TYPE_UNDEFINED,

    /*! CPU-inaccessible buffer. */
    BCMDRD_SER_TYPE_BUFFER,

    /*! CPU-inaccessible data bus. */
    BCMDRD_SER_TYPE_BUS,

    /*! Standard (indexed) memory. */
    BCMDRD_SER_TYPE_DIRECT,

    /*! Low-power standard memory. */
    BCMDRD_SER_TYPE_DIRECT_LP,

    /*! Memory with XOR scheme. */
    BCMDRD_SER_TYPE_DIRECT_XOR,

    /*! Low-power memory with XOR scheme. */
    BCMDRD_SER_TYPE_DIRECT_XOR_LP,

    /*! Hash memory. */
    BCMDRD_SER_TYPE_HASH,

    /*! Low-power hash memory. */
    BCMDRD_SER_TYPE_HASH_LP,

    /*! Hash memory with XOR scheme. */
    BCMDRD_SER_TYPE_HASH_XOR,

    /*! Low-power hash memory with XOR scheme. */
    BCMDRD_SER_TYPE_HASH_XOR_LP,

    /*! TCAM memory. */
    BCMDRD_SER_TYPE_TCAM

} bcmdrd_ser_type_t;

/*!
 * \brief SER enable types.
 *
 * Different error reporting schemes might be supported in an
 * SER-protected memory. While different sets of register/field
 * are needed to enable for each scheme supported in a specific memory.
 * The SER enable type can differentiate the error reporting schemes
 * and find the mapping enable control register/field of the memory.
 */
typedef enum bcmdrd_ser_en_type_e {

    /*! Error checking. */
    BCMDRD_SER_EN_TYPE_EC,

    /*! 1-bit error reporting. */
    BCMDRD_SER_EN_TYPE_ECC1B,

    /*! Force error on target memory. */
    BCMDRD_SER_EN_TYPE_FE,

    /*! Error for TCAM memory. */
    BCMDRD_SER_EN_TYPE_TCAM,

    /*! Maximum number of SER enable types */
    BCMDRD_SER_EN_TYPE_MAX

} bcmdrd_ser_en_type_t;

/*!
 * \brief SER control types.
 *
 * Different sets of register/field to control SER flow beyond the
 * error reporting schemes \ref bcmdrd_ser_en_type_t.
 */
typedef enum bcmdrd_ser_en_ctrl_type_e {

    /*! Packet drop control. */
    BCMDRD_SER_EN_CTRL_TYPE_DROP_PKT = BCMDRD_SER_EN_TYPE_MAX,

    /*! Maximum number of SER enable control types */
    BCMDRD_SER_EN_CTRL_TYPE_MAX

} bcmdrd_ser_en_ctrl_type_t;

/*!
 * \brief SER response types.
 *
 * The SER response type will indicate the error correction behavior
 * when an error is detected.
 */
typedef enum bcmdrd_ser_resp_e {

    /*! No action needs from software. */
    BCMDRD_SER_RESP_NONE = 0,

    /*!
     * Hardware managed status table only protected by parity.
     * Clear the entry on error.
     */
    BCMDRD_SER_RESP_ENTRY_CLEAR,

    /*!
     * Software managed configuration table protected by parity.
     * Restore entry from a valid cache on error.
     */
    BCMDRD_SER_RESP_CACHE_RESTORE,

    /*!
     * Software managed configuration table protected by ECC.
     * Issue read+write with ECC corrected value on error.
     */
    BCMDRD_SER_RESP_ECC_CORRECTABLE,

    /*!
     * This type is mainly for the table with both software managed and
     * hardware updated fields. The cache is updated on writes only
     * for SER recoverty.
     */
    BCMDRD_SER_RESP_WRITE_CACHE_RESTORE,

    /*! Requires special customized handling on error. */
    BCMDRD_SER_RESP_SPECIAL,

    /*! Error on TCAM table protected by parity. */
    BCMDRD_SER_RESP_ECC_PARITY

} bcmdrd_ser_resp_t;


/*!
 * \name SER information descriptor.
 *
 * Encode and retrieve fields in an SER information descriptor.
 *
 * | Bits  | Name        | Description                                  |
 * | :---: | ----------- | -------------------------------------------- |
 * | 19:16 | Resp        | Mechanism for error correction when an error is detected on memory. |
 * | 15:8  | NumXorBanks | Number of banks for type with XOR attribute. |
 * | 7:1   | Type        | SER type.                                    |
 * | 0:0   | DropPkt     | Whether packet is dropped if error is detected for packet processing. |
 * <br>
 */

/*! \{ */

/*! Encode drop packet data into SER information descriptor. */
#define BCMDRD_SER_INFO_DROP_PKT_ENCODE(_v) F32_ENCODE(_v, 0, 1)

/*! Encode SER type into SER information descriptor. */
#define BCMDRD_SER_INFO_TYPE_ENCODE(_v) F32_ENCODE(_v, 1, 7)

/*! Encode number of XOR banks into SER information descriptor. */
#define BCMDRD_SER_INFO_NUM_XOR_BANKS_ENCODE(_v) F32_ENCODE(_v, 8, 8)

/*! Encode SER response into SER information descriptor. */
#define BCMDRD_SER_INFO_RESP_ENCODE(_v) F32_ENCODE(_v, 16, 4)

/*! Get drop packet data from SER information descriptor. */
#define BCMDRD_SER_INFO_DROP_PKT_GET(_w) F32_GET(_w, 0, 1)

/*! Get SER type from SER information descriptor. */
#define BCMDRD_SER_INFO_TYPE_GET(_w) F32_GET(_w, 1, 7)

/*! Get number of XOR banks from SER information descriptor. */
#define BCMDRD_SER_INFO_NUM_XOR_BANKS_GET(_w) F32_GET(_w, 8, 8)

/*! Get SER response from SER information descriptor. */
#define BCMDRD_SER_INFO_RESP_GET(_w) F32_GET(_w, 16, 4)

/*! Encode SER information into a single word SER information descriptor. */
#define BCMDRD_SER_INFO_ENCODE(_d, _t, _n, _r)                               \
    (BCMDRD_SER_INFO_DROP_PKT_ENCODE(_d) | BCMDRD_SER_INFO_TYPE_ENCODE(_t) | \
     BCMDRD_SER_INFO_NUM_XOR_BANKS_ENCODE(_n) |                              \
     BCMDRD_SER_INFO_RESP_ENCODE(_r))

/*! \} */


/*!
 * \name SER enable control descriptor.
 *
 * Encode and retrieve fields in SER enable control descriptor.
 *
 * - Single-word format.
 * | Bits  | Name    | Description                               |
 * | :---: | ------- | ----------------------------------------- |
 * | 31:31 | ExtFlag | Must be zero in the format.               |
 * | 27:14 | FieldId | Field ID of SER enable control field.     |
 * | 13:0  | RegId   | Symbol ID of SER enable control register. |
 * <br>
 *
 * - Double-word format.
 * | Word | Bits  | Name    | Description                                 |
 * | :--: | :---: | ------- | ------------------------------------------- |
 * | 0    | 31:31 | ExtFlag | Indicate the start of a double-word format. |
 * | 0    | 27:14 | FieldId | Field ID of SER enable control field.       |
 * | 0    | 13:0  | RegId   | Symbol ID of SER enable control register.   |
   | 1    | 4:0   | BitPos  | Control bit position of this field.         |
 * <br>
 */

/*! \{ */

/*! Flag to indicate the SER enable control descriptor is double-word. */
#define BCMDRD_SER_ENCTRL_FLAG_EXT (1U << 31)

/*! Test SER enable descriptor word for \ref BCMDRD_SER_ENCTRL_FLAG_EXT. */
#define BCMDRD_SER_ENCTRL_EXT(_w) ((_w) & BCMDRD_SER_ENCTRL_FLAG_EXT)

/*! Encode control register ID into a single-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_REG_ENCODE(_v) F32_ENCODE(_v, 0, 14)

/*! Encode control field ID into a single-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_FIELD_ENCODE(_v) F32_ENCODE(_v, 14, 14)

/*! Get control register ID from a sigle-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_REG_GET(_w) F32_GET(_w, 0, 14)

/*! Get control field ID from a single-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_FIELD_GET(_w) F32_GET(_w, 14, 14)

/*! Encode SER enable control information into a single-word descriptor. */
#define BCMDRD_SER_ENCTRL_ENCODE(_r, _f) \
    (BCMDRD_SER_ENCTRL_REG_ENCODE(_r) |  \
     BCMDRD_SER_ENCTRL_FIELD_ENCODE(_f))

/*! Encode control register ID into a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_REG_ENCODE(_v) F32_ENCODE(_v, 0, 14)

/*! Encode control field ID into a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_FIELD_ENCODE(_v) F32_ENCODE(_v, 14, 14)

/*! Encode field bit position into a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_BITPOS_ENCODE(_v) F32_ENCODE(_v, 0, 5)

/*! Get control register ID from a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_REG_GET(_w) F32_GET(_w, 0, 14)

/*! Get control field ID from a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_FIELD_GET(_w) F32_GET(_w, 14, 14)

/*! Get field bit position from a double-word SER enable descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_BITPOS_GET(_w) F32_GET((&(_w))[1], 0, 5)

/*! Encode SER enable control information into a double-word descriptor. */
#define BCMDRD_SER_ENCTRL_EXT_ENCODE(_r, _f, _m) \
    (BCMDRD_SER_ENCTRL_FLAG_EXT |                \
     BCMDRD_SER_ENCTRL_EXT_REG_ENCODE(_r) |      \
     BCMDRD_SER_ENCTRL_EXT_FIELD_ENCODE(_f)),    \
    BCMDRD_SER_ENCTRL_EXT_BITPOS_ENCODE(_m)

/*! \} */

/*!
 * \name SER profile descriptor.
 *
 * Encode and retrieve fields in SER profile descriptor.
 *
 * - Single-word format.
 * | Bits  | Name        | Description                                       |
 * | :---: | ----------- | ------------------------------------------------- |
 * | 31:31 | LastFlag    | Indicate the last word of SER profile descriptor. |
 * | 30:30 | EnCtrl0Flag | Must be zero in this word.                        |
 * | 29:29 | EnCtrl1Flag | Indicate _EnCtrl1_ is valid in this word if set.  |
 * | 27:14 | EnCtrl1     | Encoded enable control information.               |
 * | 13:0  | Info        | Offset of the SER information profile.            |
 * <br>
 *
 * - Multi-word format (for memories support more than one enable type).
 * | Word | Bits  | Name        | Description                                |
 * | :--: | :---: | ----------- | ------------------------------------------ |
 * | 0    | 31:31 | LastFlag    | Must be zero in this word.                 |
 * | 0    | 30:30 | EnCtrl0Flag | Must be zero in this word.                 |
 * | 0    | 29:29 | EnCtrl1Flag | Indicate _EnCtrl1_ is valid in this word.  |
 * | 0    | 27:14 | EnCtrl1     | Encoded enable control information.        |
 * | 0    | 13:0  | Info        | Offset of the SER information Profile.     |
 * | N    | 31:31 | LastFlag    | Indicate the end of the descriptor if set. |
 * | N    | 30:30 | EnCtrl0Flag | _EnCtrl0_ is valid in this word if set.    |
 * | N    | 29:29 | EnCtrl1Flag | _EnCtrl1_ is valid in this word if set.    |
 * | N    | 27:14 | EnCtrl1     | Encoded enable control information.        |
 * | N    | 13:0  | EnCtrl0     | Encoded enable control information.        |
 * <br>
 *
 * Encoded enable control information:
 * | Bits  | Name        | Description                                    |
 * | :---: | ----------  | ---------------------------------------------- |
 * | 13:11 | EnCtrlType  | SER enable type.                               |
 * | 10:0  | EnCtrlIdx   | Offset of the SER enable control type profile. |
 * <br>
 */

/*! \{ */

/*! Flag to indicate the last word of SER profile for symbols. */
#define BCMDRD_SER_PF_FLAG_LAST      (1U << 31)

/*! Flag to indicate ENTROL0 is valid in a word of SER profile descriptor. */
#define BCMDRD_SER_PF_FLAG_ENCTRL0   (1U << 30)

/*! Flag to indicate ENTROL1 is valid in a word of SER profile descriptor. */
#define BCMDRD_SER_PF_FLAG_ENCTRL1   (1U << 29)

/*! Test SER profile descriptor word for \ref BCMDRD_SER_PF_FLAG_LAST. */
#define BCMDRD_SER_PF_LAST(_w) ((_w) & BCMDRD_SER_PF_FLAG_LAST)

/*! Test SER profile descriptor word for \ref BCMDRD_SER_PF_FLAG_ENCTRL0. */
#define BCMDRD_SER_PF_ENCTRL0(_w) ((_w) & BCMDRD_SER_PF_FLAG_ENCTRL0)

/*! Test SER profile descriptor word for \ref BCMDRD_SER_PF_FLAG_ENCTRL1. */
#define BCMDRD_SER_PF_ENCTRL1(_w) ((_w) & BCMDRD_SER_PF_FLAG_ENCTRL1)

/*! Encode SER enable control profile index into SER enable control data. */
#define BCMDRD_SER_PF_ENCTRL_IDX_ENCODE(_v) F32_ENCODE(_v, 0, 11)

/*! Encode SER enable type into SER enable control data. */
#define BCMDRD_SER_PF_ENCTRL_TYPE_ENCODE(_v) F32_ENCODE(_v, 11, 3)

/*! Get SER enable control profile index from SER enable control data. */
#define BCMDRD_SER_PF_ENCTRL_IDX_GET(_v) F32_GET(_v, 0, 11)

/*! Get SER enable type from SER enable control data. */
#define BCMDRD_SER_PF_ENCTRL_TYPE_GET(_v) F32_GET(_v, 11, 3)

/*! Encode SER enable control data. */
#define BCMDRD_SER_PF_ENCTRL_ENCODE(_t, _v) \
    (BCMDRD_SER_PF_ENCTRL_TYPE_ENCODE(_t) | \
     BCMDRD_SER_PF_ENCTRL_IDX_ENCODE(_v))

/*! Encode SER information profile index into SER profile descriptor. */
#define BCMDRD_SER_PF_INFO_ENCODE(_v) F32_ENCODE(_v, 0, 14)

/*!
 * Encode SER enable control data into lower part of an SER profile
 * descriptor word.
 */
#define BCMDRD_SER_PF_ENCTRL0_ENCODE(_v) \
    (BCMDRD_SER_PF_FLAG_ENCTRL0 | F32_ENCODE(_v, 0, 14))

/*!
 * Encode SER enable control data into higher part of an SER profile
 * descriptor word.
 */
#define BCMDRD_SER_PF_ENCTRL1_ENCODE(_v) \
    (BCMDRD_SER_PF_FLAG_ENCTRL1 | F32_ENCODE(_v, 14, 14))

/*! Get SER information profile index from SER profile descritor. */
#define BCMDRD_SER_PF_INFO_GET(_w) F32_GET(_w, 0, 14)

/*!
 * Get SER enable control data from lower part of an SER profile
 * descritor word.
 */
#define BCMDRD_SER_PF_ENCTRL0_GET(_w) F32_GET(_w, 0, 14)

/*!
 * Get SER enable control data from higher part of an SER profile
 * descritor word.
 */
#define BCMDRD_SER_PF_ENCTRL1_GET(_w) F32_GET(_w, 14, 14)

/*! Encode SER profile descriptor with SER information profile only. */
#define BCMDRD_SER_PF_0_ENCODE(_v) \
    (BCMDRD_SER_PF_FLAG_LAST | BCMDRD_SER_PF_INFO_ENCODE(_v))

/*!
 * Encode SER profile descriptor with SER informaton profile and
 * one SER enable control profile.
 */
#define BCMDRD_SER_PF_1_ENCODE(_v, _e1)                        \
    (BCMDRD_SER_PF_FLAG_LAST | BCMDRD_SER_PF_INFO_ENCODE(_v) | \
     BCMDRD_SER_PF_ENCTRL1_ENCODE(_e1))

/*!
 * Encode SER profile descriptor with SER informaton profile and
 * two SER enable control profiles.
 */
#define BCMDRD_SER_PF_2_ENCODE(_v, _e1, _e2)                             \
    (BCMDRD_SER_PF_INFO_ENCODE(_v) | BCMDRD_SER_PF_ENCTRL1_ENCODE(_e1)), \
    (BCMDRD_SER_PF_FLAG_LAST | BCMDRD_SER_PF_ENCTRL0_ENCODE(_e2))

/*!
 * Encode SER profile descriptor with SER informaton profile and
 * three SER enable control profiles.
 */
#define BCMDRD_SER_PF_3_ENCODE(_v, _e1, _e2, _e3)                        \
    (BCMDRD_SER_PF_INFO_ENCODE(_v) | BCMDRD_SER_PF_ENCTRL1_ENCODE(_e1)), \
    (BCMDRD_SER_PF_FLAG_LAST |                                           \
     BCMDRD_SER_PF_ENCTRL0_ENCODE(_e2) | BCMDRD_SER_PF_ENCTRL1_ENCODE(_e3))

/*!
 * Encode SER profile descriptor with SER informaton profile and
 * four SER enable control profiles.
 */
#define BCMDRD_SER_PF_4_ENCODE(_v, _e1, _e2, _e3, _e4)                   \
    (BCMDRD_SER_PF_INFO_ENCODE(_v) | BCMDRD_SER_PF_ENCTRL1_ENCODE(_e1)), \
    (BCMDRD_SER_PF_ENCTRL0_ENCODE(_e2) | BCMDRD_SER_PF_ENCTRL1_ENCODE(_e3)), \
    (BCMDRD_SER_PF_FLAG_LAST | BCMDRD_SER_PF_ENCTRL0_ENCODE(_e4))

/*! \} */


/*!
 * \name Restricted memory resouce (RMR) SER profile.
 *
 * Get and set offset of SER profile to restricted memory resource information
 * structure.
 */
/*! \{ */

/*! Set profile offset for error correction information of RMR. */
#define BCMDRD_SER_RMR_EP_OFFSET_SET(_ep) F32_ENCODE((_ep), 0, 16)

/*! Get profile offset for error correction information of RMR. */
#define BCMDRD_SER_RMR_EP_OFFSET_GET(_pfinfo) F32_GET((_pfinfo), 0, 16)

/*! \} */


/*! SER restricted memory resource (RMR) information structure. */
typedef struct bcmdrd_ser_rmr_s {

    /*! RMR name. */
    const char* name;

    /*! Profile information for SER. */
    uint32_t pfinfo;

} bcmdrd_ser_rmr_t;

/*! SER data structure (all data). */
typedef struct bcmdrd_ser_data_s {

    /*! List of all SER information profiles used by this device. */
    const uint32_t *info_profiles;

    /*! List of all SER enable type profiles used by this device. */
    const uint32_t *en_profiles[BCMDRD_SER_EN_CTRL_TYPE_MAX];

    /*!
     * List of all SER profiles used by this device.
     * This profile is indexed from \ref bcmdrd_symbol_t and mapped to
     * \c info_profiles and \c en_profiles.
     */
    const uint32_t *sym_profiles;

    /*! List of all RMRs used by this device. */
    const bcmdrd_ser_rmr_t *rmrs;

    /*! Number of entries in RMRs array. */
    const uint32_t rmrs_size;

} bcmdrd_ser_data_t;

/*!
 * \brief Get SER information descriptor for a specific symbol.
 *
 * This function returns the device-specific SER information descriptor
 * for a specified symbol. The \c sympf_offset is the offset to \c
 * sym_pfofiles in \ref bcmdrd_ser_data_t, which is retrieved from the
 * encoded information of a specified symbol table.
 *
 * \param [in] ser_data Device-specific SER data structure.
 * \param [in] sympf_offset Offset of SER profiles for a specific symbol.
 *
 * \return SER information descriptor, NULL if unavailable.
 */
extern const uint32_t *
bcmdrd_sym_ser_info_profile_get(const bcmdrd_ser_data_t *ser_data,
                                int sympf_offset);

/*!
 * \brief Get SER enable control descriptor for a specific symbol.
 *
 * This function returns the device-specific control descriptor of a given
 * SER enable type for a specified symbol. The \c sympf_offset is the offset
 * to \c sym_pfofiles in \ref bcmdrd_ser_data_t, which is retrieved from the
 * encoded information of a specified symbol table.
 *
 * \param [in] ser_data Device-specific SER data structure.
 * \param [in] sympf_offset Offset of SER profiles for a specific symbol.
 * \param [in] type SER enable type.
 *
 * \return SER enable control descriptor, NULL if unavailable.
 */
extern const uint32_t *
bcmdrd_sym_ser_en_ctrl_profile_get(const bcmdrd_ser_data_t *ser_data,
                                   int sympf_offset,
                                   bcmdrd_ser_en_type_t type);

/*!
 * \brief Get RMR information.
 *
 * Retrive device-specific RMR information for a given RMR ID.
 *
 * \param [in] ser_data Device-specific SER data structure.
 * \param [in] rid Restricted memory resource ID.
 *
 * \retval RMR information structure, or NULL if not found.
 */
extern const bcmdrd_ser_rmr_t *
bcmdrd_sym_ser_rmr_get(const bcmdrd_ser_data_t *ser_data,
                       bcmdrd_ser_rmr_id_t rid);

/*!
 * \brief Search a given RMR name in device-specific SER data.
 *
 * Find the name in RMRs array, returns the RMR information structure
 * if the name matches. The RMR ID will be returned as well if \c rid
 * is not NULL.
 *
 * \param [in] name RMR name to seach for.
 * \param [in] ser_data Device-specific SER data structure.
 * \param [out] rid Restricted memory resource ID.
 *
 * \retval RMR information structure, or NULL if not found.
 */
extern const bcmdrd_ser_rmr_t *
bcmdrd_ser_rmr_find(const char *name, const bcmdrd_ser_data_t *ser_data,
                    bcmdrd_ser_rmr_id_t *rid);

/*!
 * \brief Get SER profile offset of a specified RMR.
 *
 * \param [in] rmr RMR information structure.
 *
 * \return Offset to SER profile for a given RMR.
 */
extern int
bcmdrd_ser_rmr_profile_offset_get(const bcmdrd_ser_rmr_t *rmr);

#endif /* BCMDRD_SER_H */
