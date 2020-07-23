/*! \file bcmfp_keygen_api.h
 *
 * All data structures, macros and enumerations used
 * in keygen alogorithm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_KEYGEN_API_H
#define BCMFP_KEYGEN_API_H

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmfp/bcmfp_util_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_types_internal.h>

/*!
 * Maxmimum number of slices occupied by any
 * keygen mode in any device.
 */
#define BCMFP_KEYGEN_ENTRY_PARTS_MAX 4

/*!
 * \brief Keygen Modes.
 *
 * This enum is used identify different
 * modes of operation in FP.
 */
typedef enum bcmfp_keygen_mode_e {
     /*!
      * Mode HALF. This mode will have half of the key
      * size of SINGLE mode.
      */
    BCMFP_KEYGEN_MODE_HALF = 0,
     /*! Single wide mode which uses one slice only. */
    BCMFP_KEYGEN_MODE_SINGLE = 1,
     /*!
      * Single wide mode which uses one slice only,
      * but have narrow ASET. Used in EM HASH FP.
      */
    BCMFP_KEYGEN_MODE_SINGLE_ASET_NARROW = 2,
     /*!
      * Single wide mode which uses one slice only,
      * but have wider ASET. Used in EM HASH FP.
      */
    BCMFP_KEYGEN_MODE_SINGLE_ASET_WIDE = 3,
    /* !
     * Uses single slice but key size is double the key
     * size of SINGLE mode.
     */
    BCMFP_KEYGEN_MODE_DBLINTRA = 4,
    /* !
     * Uses two slices and key size is double the key
     * size of SINGLE mode.
     */
    BCMFP_KEYGEN_MODE_DBLINTER = 5,
    /* !
     * Uses two slices and key size is triple the key
     * size of SINGLE mode.
     */
    BCMFP_KEYGEN_MODE_TRIPLE = 6,
    /* !
     * Uses two slices and key size is four times the key
     * size of SINGLE mode.
     */
    BCMFP_KEYGEN_MODE_QUAD = 7,
    /*! Always Last. Not a usable value. */
    BCMFP_KEYGEN_MODE_COUNT = 8
} bcmfp_keygen_mode_t;

/*!
 * \brief Extractor Attributes.
 *
 * extractor attributes used to validity of extractor configuration
 * for a given keygen mode and qualifer list.
 * Note: enumerations in bcmfp_keygen_ext_attr_t must have 1-to-1
 * mapping with enumerations in bcmfp_keygen_acl_attr_t.
 */
typedef enum bcmfp_keygen_ext_attr_s {
     /*! Extractor can operate in multiple granularities. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN = 0,
    /*! Extractor can operate in granularity 1. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_1,
    /*! Extractor can operate in granularity 2. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_2,
    /*! Extractor can operate in granularity 4. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_4,
    /*! Extractor can operate in granularity 8. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_8,
    /*! Extractor can operate in granularity 16. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_16,
    /*! Extractor can operate in granularity 32. */
    BCMFP_KEYGEN_EXT_ATTR_MULTI_GRAN_32,
    /*! Extractor is extracts bits from the pass through section. */
    BCMFP_KEYGEN_EXT_ATTR_PASS_THRU,
    /*! Extractor cannot be used when keygen mode is HALF. */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF,
    /*! Extractor cannot be used if IPBM is part of final key. */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM,
    /*!
     * Extractor cannot be used if SRC_DST_CONT_0 LSB 16bits.
     * (Total length is 17 bits), is part of final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0,
    /*!
     * Extractor cannot be used if SRC_DST_CONT_1 LSB 16bits.
     * (Total length is 17 bits), is part of final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1,
    /*!
     * Extractor cannot be used if MSB bits of SRC_DST_CONT_0.
     * (or) SRC_DST_CONT_1, is part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB,
    /*!
     * Extractor cannot be used if SRC_DST_CONT_0 LSB 16bits.
     * (Total length is 17 bits), is part of final key of
     * HALF(80bit) mode group.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM,
    /*!
     * Extractor cannot be used if SRC_DST_CONT_1 LSB 16bits.
     * (Total length is 17 bits), is part of final key of
     * HALF(80bit) mode group.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM,
    /*!
     * Extractor cannot be used if MSB bits of SRC_DST_CONT_0
     * (or) SRC_DST_CONT_1, is part of the final key of HALF
     * (80bit) mode group.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM,
    /*!
     * Extractor cannot be used if post mux qualifier, Drop,
     * is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP,
    /*!
     * Extractor cannot be used if LSB 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_0_3,
    /*!
     * Extractor cannot be used if middle 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_4_7,
    /*! Extractor cannot be used if MSB 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASS_ID_BITS_8_11,
    /*!
     * Extractor cannot be used if LSB 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3,
    /*!
     * Extractor cannot be used if middle 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7,
    /*!
     * Extractor cannot be used if MSB 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11,
    /*!
     * Extractor cannot be used if post mux qualifier, HIT status from Exact
     * Match first lookup,  is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT,
    /*!
     * Extractor cannot be used if post mux qualifier, HIT status from Exact
     * Match second lookup,  is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT,
    /*!
     * Extractor cannot be used if post mux qualifier, NatNeeded,
     * is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED,
    /*!
     * Extractor cannot be used if post mux qualifier, NatDstRealmId,
     * is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID,
    /*!
     * Extractor cannot be used if LSB 4 bits of lookup status vector,
     * are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3,
    /*!
     * Extractor cannot be used if MSB 4 bits of lookup status vector,
     * are part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7,
    /*!
     * Extractor cannot be used if 17 bits of SRC_DST_CONT_A
     * is part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A,
    /*!
     * Extractor cannot be used if 17 bits of SRC_DST_CONT_B
     * is part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B,
    /*!
     * Extractor cannot be used if post mux qualifier, PacketRes,
     * is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION,
    /*!
     * Extractor cannot be used if 16 bits of CLASS_ID C
     * container is part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C,
    /*!
     * Extractor cannot be used if 16 bits of CLASS_ID D
     * container is part of the final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D,
    /*!
     * Extractor cannot be used if post mux qualifier,
     * Internal Priority, is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI,
    /*!
     * Extractor cannot be used if post mux qualifier,
     * Internal Congestion, is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN,
    /*!
     * Extractor cannot be used if drop Reasons generated in
     * ingress pipeline before IFP are part of final key.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR,
    /*!
     * Extractor cannot be used if post mux qualifier,
     * HiGiG header opcode, is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE,
    /*! Extractor cannot be used if keygen mode is EM_128. */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE,
    /*!
     * Extractor cannot be used if keygen algorithm is called for
     * QSET update.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE,
    /*!
     * Extractor cannot be used if post mux qualifier
     * keytype is present in the QSET.
     */
    BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_KEY_TYPE,

    /*!
     * If this flag is set the ext_cfg->finfo and ext_cfg->in_use
     * are copied from the ext to the ext_cfg->parent_ext_id
     */
    BCMFP_KEYGEN_EXT_ATTR_PUSH_TO_PARENT_EXT,

    /*!
     * All bits extracted by an extractor cannot be part of
     * the final key.
 */
    BCMFP_KEYGEN_EXT_ATTR_PARTIAL_GRAN,

    /*! Always Last. Not a usable value. */
    BCMFP_KEYGEN_EXT_ATTR_COUNT
} bcmfp_keygen_ext_attr_t;

/*!
 * \brief Bitmap of extractor attributes.
 * This structure is used to capture the
 * all the attributes of an extractor.
 */
typedef struct bcmfp_keygen_ext_attrs_s {
    /*! Bitmap of extractor attributes. */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_KEYGEN_EXT_ATTR_COUNT)];
} bcmfp_keygen_ext_attrs_t;

/*!
 * \brief Extractor Control Selectors.
 *
 * All the different qualifier controls to serve all keygen blocks
 * in different chips.
 */
typedef enum bcmfp_keygen_ext_ctrl_sel_e {
    /*! Selector disabled. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_DISABLE            = 0,
    /*! Auxiliary Tag A selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A          = 1,
    /*! Auxiliary Tag B selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B          = 2,
    /*! Auxiliary Tag C selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C          = 3,
    /*! Auxiliary Tag D selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D          = 4,
    /*! TCP function selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN             = 5,
    /*! ToS function selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN             = 6,
    /*! TTL function selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN             = 7,
    /*! Class ID container A selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_A          = 8,
    /*! Class ID container B selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_B          = 9,
    /*! Class ID container C selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C          = 10,
    /*! Class ID container D selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D          = 11,
    /*! Source Container A selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A              = 12,
    /*! Source Container B selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B              = 13,
    /*! Src/Dst Container 0 selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0          = 14,
    /*! Src/Dst Container 1 selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1          = 15,
    /*! IPBM Source. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT       = 16,
    /*! Multiplexer in extractor hierarchy. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER        = 17,
    /*! Normalize L3 and L4 addresses. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM         = 18,
    /*! Normalize Mac address. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM           = 19,
    /*! PostMux Qualifier. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX               = 20,
    /*! UDF Qualifiers. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_UDF                = 21,
    /*! Destination Container A selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A              = 22,
    /*! Destination Container B selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B              = 23,
    /*! ALT TTL function selector. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN         = 24,
    /*! Always Last. Not a usable value. */
    BCMFP_KEYGEN_EXT_CTRL_SEL_COUNT              = 25
} bcmfp_keygen_ext_ctrl_sel_t;

/*!
 * \brief Extractor Section.
 *
 * This enum is used to identify the section
 * from which extractor extracts the qualifier bits.
 * All the different hairarchical bus sections to
 * serve all keygen blocks in different chips.
 */
typedef enum bcmfp_keygen_ext_section_e {
    /*! Selector disabled. */
    BCMFP_KEYGEN_EXT_SECTION_DISABLE = 0,
    /*! Level-1 32 bit extractor. */
    BCMFP_KEYGEN_EXT_SECTION_L1E32,
    /*! ISDW part 1 16 bit input section. */
    BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW,
    /*! Level-1 16 bit extractor. */
    BCMFP_KEYGEN_EXT_SECTION_L1E16,
    /*! Level-1 8 bit extractor. */
    BCMFP_KEYGEN_EXT_SECTION_L1E8,
    /*! Level-1 4 bit extractor. */
    BCMFP_KEYGEN_EXT_SECTION_L1E4,
    /*! Level-1 2 bit extractor. */
    BCMFP_KEYGEN_EXT_SECTION_L1E2,
    /*! Level-2 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S1,
    /*! Level-2 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S2,
    /*! Level-2 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S3,
    /*! Level-2 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S4,
    /*! Level-2 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S5,
    /*! Level-2 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S6,
    /*! Level-2 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S7,
    /*! Level-2 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S8,
    /*! Level-2 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S9,
    /*! Level-2 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2S10,
    /*! Level-3 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S1,
    /*! Level-3 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S2,
    /*! Level-3 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S3,
    /*! Level-3 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S4,
    /*! Level-3 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S5,
    /*! Level-3 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S6,
    /*! Level-3 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S7,
    /*! Level-3 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S8,
    /*! Level-3 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S9,
    /*! Level-3 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3S10,
    /*! Level-2 Slice A 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS1,
    /*! Level-2 Slice A 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS2,
    /*! Level-2 Slice A 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS3,
    /*! Level-2 Slice A 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS4,
    /*! Level-2 Slice A 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS5,
    /*! Level-2 Slice A 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS6,
    /*! Level-2 Slice A 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS7,
    /*! Level-2 Slice A 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS8,
    /*! Level-2 Slice A 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS9,
    /*! Level-2 Slice A 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2AS10,
    /*! Level-2 Slice B 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS1,
    /*! Level-2 Slice B 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS2,
    /*! Level-2 Slice B 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS3,
    /*! Level-2 Slice B 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS4,
    /*! Level-2 Slice B 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS5,
    /*! Level-2 Slice B 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS6,
    /*! Level-2 Slice B 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS7,
    /*! Level-2 Slice B 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS8,
    /*! Level-2 Slice B 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS9,
    /*! Level-2 Slice B 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2BS10,
    /*! Level-2 Slice C 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS1,
    /*! Level-2 Slice C 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS2,
    /*! Level-2 Slice C 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS3,
    /*! Level-2 Slice C 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS4,
    /*! Level-2 Slice C 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS5,
    /*! Level-2 Slice C 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS6,
    /*! Level-2 Slice C 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS7,
    /*! Level-2 Slice C 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS8,
    /*! Level-2 Slice C 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS9,
    /*! Level-2 Slice C 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L2CS10,
    /*! Level-3 Slice A 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS1,
    /*! Level-3 Slice A 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS2,
    /*! Level-3 Slice A 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS3,
    /*! Level-3 Slice A 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS4,
    /*! Level-3 Slice A 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS5,
    /*! Level-3 Slice A 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS6,
    /*! Level-3 Slice A 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS7,
    /*! Level-3 Slice A 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS8,
    /*! Level-3 Slice A 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS9,
    /*! Level-3 Slice A 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3AS10,
    /*! Level-3 Slice B 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS1,
    /*! Level-3 Slice B 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS2,
    /*! Level-3 Slice B 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS3,
    /*! Level-3 Slice B 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS4,
    /*! Level-3 Slice B 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS5,
    /*! Level-3 Slice B 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS6,
    /*! Level-3 Slice B 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS7,
    /*! Level-3 Slice B 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS8,
    /*! Level-3 Slice B 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS9,
    /*! Level-3 Slice B 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3BS10,
    /*! Level3 Slice C 1st extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS1,
    /*! Level-3 Slice C 2nd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS2,
    /*! Level-3 Slice C 3rd extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS3,
    /*! Level-3 Slice C 4th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS4,
    /*! Level-3 Slice C 5th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS5,
    /*! Level-3 Slice C 6th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS6,
    /*! Level-3 Slice C 7th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS7,
    /*! Level-3 Slice C 8th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS8,
    /*! Level-3 Slice C 9th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS9,
    /*! Level-3 Slice C 10th extractor section. */
    BCMFP_KEYGEN_EXT_SECTION_L3CS10,
    /*! Final Key Section. */
    BCMFP_KEYGEN_EXT_SECTION_FK,
    /*! Slice A Final Key section. */
    BCMFP_KEYGEN_EXT_SECTION_FKA,
    /*! Slice B Final Key section. */
    BCMFP_KEYGEN_EXT_SECTION_FKB,
    /*! Slice C Final Key section. */
    BCMFP_KEYGEN_EXT_SECTION_FKC,
    /* !
     * Section for post Mux qualifier "Ingress Port bitmap"
     * in the final key. */
    BCMFP_KEYGEN_EXT_SECTION_IPBM,
    /*!
     * Section for post Mux qualifier "DROP"
     * in the final key. */
    BCMFP_KEYGEN_EXT_SECTION_DROP,
    /*!
     * Section for post Mux qualifier "NAT Needed"
     * in the final key. */
    BCMFP_KEYGEN_EXT_SECTION_NAT_NEEDED,
    /*!
     * Section for post Mux qualifier "NAT destination realm Id"
     * in the final key. */
    BCMFP_KEYGEN_EXT_SECTION_NAT_DST_REALMID,
    /*!
     * Section for post Mux qualifier "HIT status of first
     * lookup in Exact Match rules" in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT,
    /*!
     * Section for post Mux qualifier "HIT status of second
     * lookup in Exact Match rules" in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT,
    /*!
     * Section for post Mux qualifier "LSB 4 bits of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3,
    /*!
     * Section for post Mux qualifier "bits 4 - 7 of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7,
    /*!
     * Section for post Mux qualifier "bits 8 - 11 of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_8_11,
    /*!
     * Section for post Mux qualifier "LSB 4 bits of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_0_3,
    /*!
     * Section for post Mux qualifier "bits 4 - 7 of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASS_ID_BITS_4_7,
    /*!
     * Section for post Mux qualifier "bits 8 - 11 of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11,
    /*!
     * Section for post Mux qualifiers LSB 16 bits, sharing the
     * source destination container 0, in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0,
    /*!
     * Section for post Mux qualifiers LSB 16 bits, sharing the
     * source destination container 1, in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1,
    /*!
     * Section for post Mux qualifiers MSB bit, sharing the source
     * destination container 0/1, in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB,
    /*!
     * Section for post Mux qualifier Lookup up status vectors
     * LSB 4 bits in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3,
    /*!
     * Section for post Mux qualifier Lookup up status vectors
     * MSB 4 bits in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7,
    /*!
     * Section for post Mux qualifiers, sharing the destination
     * container 0(all 17 bits), in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_A,
    /*!
     * Section for post Mux qualifiers, sharing the destination
     * container 1(all 17 bits), in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_DST_CONTAINER_B,
    /*!
     * Section for post Mux qualifier packet resolution
     * in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_PKT_RESOLUTION,
    /*!
     * Section for post Mux qualifiers, sharing the class id
     * container C, in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_C,
    /*!
     * Section for post Mux qualifiers, sharing the class id
     * container D, in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_CLASS_ID_D,
    /*!
     * Section for post Mux qualifier internal priority
     * in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_INT_PRI,
    /*!
     * Section for post Mux qualifier "drop reason codes in
     * pipeline fed to IFP" in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR,
    /*!
     * Section for post Mux qualifier "HiGiG header opcode"
     * in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_MH_OPCODE,

    /*!
     * Section for post Mux qualifier "KEY_TYPE"
     * in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_KEY_TYPE,

    /*!
     * Section for post Mux qualifier "Internal congenstion(INT_CN)"
     * in the final key.
     */
    BCMFP_KEYGEN_EXT_SECTION_INT_CN,

    /*! Always Last. Not a usable value. */
    BCMFP_KEYGEN_EXT_SECTION_COUNT
} bcmfp_keygen_ext_section_t;

/*!
 * \brief extractor control information.
 *
 * This structure captures the ctrl select values
 * to be programmed on the extractor for
 * extracting the required bits.
 */
typedef struct bcmfp_keygen_ext_ctrl_sel_info_s {
    /*! Extractor control type. */
    bcmfp_keygen_ext_ctrl_sel_t ctrl_sel;
    /*! Selector value */
    uint16_t ctrl_sel_val;
    /*! part to which this selector belongs to */
    uint8_t part;
    /*!
     * extractor hierarchy level. May not be required for all types
     * of extractor controls.
     */
    uint8_t level;
    /*!
     * Granularity of the extractor. May not be required for all types
     * of extractor controls.
     */
    uint8_t gran;
    /*!
     * Extractor number. May not be required for all types
     * of extractor controls.
     */
    uint16_t ext_num;
} bcmfp_keygen_ext_ctrl_sel_info_t;

/*! Maximum number of extractor hirerachy levels on any device */
#define BCMFP_KEYGEN_EXT_LEVEL_COUNT 5

/*!
 * Number of extractr output sections. Bits extracted by an extractor
 * may go to multiple sections in next levels.
 */
#define BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS 2

/*!
 * \brief Single extractor configuration.
 *
 * This struture is used for capturing the
 * ext_cfg which is used for eveluating which
 * ext to be used for a given section
 */
typedef struct bcmfp_keygen_ext_cfg_s {
    /*!
     * Run time field used to know whether the extractor
     * is already used or not. By default it is set to zero.
     */
    uint8_t  in_use;
    /*!
     * Extractor Id holds 4 tuple
     * (part, level, granularity, extractor number)
     * information of an extractor.
     * 10 bits[0 - 9]  = extractor number.
     * 8 bits[10 - 17] = granularity.
     * 4 bits[18 - 21] = hierarchy level.
     * 2 bits[29 - 31] = part(If extractor database is
     *                        for multi wide modes).
     */
    uint32_t ext_id;
    /*!
     * Format of this is exactly same as ext_id.
     * onehot_ext_id is the extractor ID to be used in
     * next level if ext_id is used in this level.
     */
    uint32_t onehot_ext_id[BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS];

    /*!
     * Format of this is exactly same as ext_id.
     * The parent_ext_id is mainly set for
     * pass through sections' extractors.
     * This gets used for the cases where parent_ext_cfg and the
     * pass through ext_cfg should be the same.
     */
    uint32_t parent_ext_id;
    /*!
     * Format of this is exactly same as ext_id. ext_id
     * and conflict_ext_id are mutually exclusive i.e.
     * for a given QSET, either ext_id or conflict_ext_id
     * can be used, not both.
     */
    uint32_t conflict_ext_id[BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS];

    /*! Input section from which this extractor will extract fields. */
    bcmfp_keygen_ext_section_t in_sec;

    /*! Output section to which this extractor will fed fields. */
    bcmfp_keygen_ext_section_t out_sec[BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS];

   /*! Offset of the extracted field in the output section */
   uint16_t offset[BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS];

    /*!
     * Extractor attributes used to validate the extractor configuration
     * for the given keygen mode and qualifiers that needs to be extracted.
     */
   bcmfp_keygen_ext_attrs_t ext_attrs;

   /*!
    * When extractor has a flag XXX_PARTIAL_GRAN, then this bitmap
    * will provide the actual valid bits of the data extracted by
    * the extractor.
    */
   uint32_t valid_bitmap;

   /*! Used internally by the keygen algorithm. */
   void *finfo;

   /*!
    * Used internally by the keygen algorithm.Some extractors have
    * more than one out section.That means data extracted by the
    * extractor goes to multiple sections. But data pushed to only
    * one out section will be further pushed. This variable holds
    * that out section idx. This is used as index to out_sec, offset
    * onehot_ext_id and conflict_ext_id arrays in this structure.
    */
   uint8_t out_sec_idx;
} bcmfp_keygen_ext_cfg_t;

/*!
 * \brief Extractor section Configuration.
 *
 * This structure captures the section details
 * of the extractor.
 */
typedef struct bcmfp_keygen_ext_section_cfg_s {
    /*!
     * Section Flags. Currently there is one flag created
     * BCMFP_KEYGEN_EXT_SECTION_PASS_THRU.
     */
    uint32_t flags;
    /*!
     * Sections level. Section of a level is equal to one
     * less of level of extractors going out of the section.
     */
    uint8_t level;
    /*! Section identifier. */
    bcmfp_keygen_ext_section_t sec;
    /*! Parent section identifier */
    bcmfp_keygen_ext_section_t parent_sec;
    /*! Section size in bits */
    int drain_bits;
    /*!
     * This field is used in runtime to avoid pushing more bits
     * than drain_bits.
     */
    int fill_bits;
    /*!
     * Generally it is set to 1. If section has any extractor
     * which can operate in multiple granularities, then this can
     * be more than 1.
     */
    uint16_t ext_cfg_combinations;
} bcmfp_keygen_ext_section_cfg_t;

/*!
 * \brief Sorted section database.
 *
 * This is used to store the sorted sec indices
 * Sections are sorted based on the max out section
 * an extract of the current section can have
 */
typedef struct bcmfp_keygen_sorted_sec_idx_s {
    /*! sec idx in keygen_md->sections[part][level]*/
    uint8_t sec_idx;
    /*!
     * max number of output sections an extractor from the
     * current section has
     */
    uint8_t max_out_sec;
    /*! pointer to the next sec_idx_t */
    struct bcmfp_keygen_sorted_sec_idx_s *next;
} bcmfp_keygen_sorted_sec_idx_t;

/*!
 * \brief Extractor database for a particular keygen mode.
 *
 * This structure captures the extractor configuration
 * for a particular keygen mode
 */
typedef struct bcmfp_keygen_ext_cfg_db_s {
    /*! Total number of extractor levels */
    uint8_t num_ext_levels;
     /*! keygen mode for which this extractor database is used.  */
    bcmfp_keygen_mode_t mode;
     /*! Extractors configuration in each extractor hierarchy.   */
    bcmfp_keygen_ext_cfg_t *ext_cfg[BCMFP_KEYGEN_EXT_LEVEL_COUNT];
     /*! Number of extractor configurations in each extractor hierarchy.  */
    uint16_t conf_size[BCMFP_KEYGEN_EXT_LEVEL_COUNT];
    /*! Extractor sections configuration */
    bcmfp_keygen_ext_section_cfg_t *sec_cfg[BCMFP_KEYGEN_EXT_SECTION_COUNT];
    /*! Number of valid section configurations in this database.  */
    uint16_t num_sec;
} bcmfp_keygen_ext_cfg_db_t;

/*!
 * Extractor ID construction macro.
 */
#define BCMFP_KEYGEN_EXT_ID_CREATE(_p_, _l_, _g_, _e_num_, _e_id_)              \
    (_e_id_) = (((_p_) << 28) |  ((_l_) << 24) | ((_g_) << 8)  |  (_e_num_))

/*!
 * Parse an Extractor ID and get the extractor level, granularity and
 * extractor number information.
 */
#define BCMFP_KEYGEN_EXT_ID_PARSE(_e_id_, _p_, _l_, _g_, _e_num_)      \
                                (_e_num_) = ((_e_id_) & 0xff);         \
                                (_g_) = (((_e_id_) >> 8) & 0xffff);    \
                                (_l_) = (((_e_id_) >> 24) & 0xf);      \
                                (_p_) = (((_e_id_) >> 28) & 0x3)

/*!
 * Parse an Extractor ID and get the part to which the
 * extractor belongs.
 */
#define BCMFP_KEYGEN_EXT_ID_PART_GET(_ext_id_)  (((_ext_id_) >> 28) & 0x3)
/*!
 * Parse an Extractor ID ann get the granularity of
 * the extractor..
 */
#define BCMFP_KEYGEN_EXT_ID_GRAN_GET(_ext_id_)  (((_ext_id_) >> 8) & 0xffff)
/*!
 * Parse an Extractor ID and get the level to which the
 * extractor belongs.
 */
#define BCMFP_KEYGEN_EXT_ID_LEVEL_GET(_ext_id_) (((_ext_id_) >> 24) & 0xf)
/*! Get the extractor ID. */
#define BCMFP_KEYGEN_EXT_ID_NUM_GET(_ext_id_)   ((_ext_id_) & 0xff)

/*!
 * Encode the granularity of the
 * extractor into its ID.
 */
#define BCMFP_KEYGEN_EXT_ID_GRAN_SET(_ext_id_, gran)   \
                  (_ext_id_) &= ~(0xFFFF << 8);        \
                  (_ext_id_) |= ((gran & 0xffff) << 8)

/*!
 * Parse an Extractor attr idx and get
 * extractor granularity.
 */
#define BCMFP_KEYGEN_EXT_ATTR_IDX_TO_GRAN(attr_idx, gran)              \
               switch (attr_idx) {                                    \
                   case 0:                                            \
                       gran = 1;                                      \
                       break;                                         \
                   case 1:                                            \
                       gran = 2;                                      \
                       break;                                         \
                   case 2:                                            \
                       gran = 4;                                      \
                       break;                                         \
                   case 3:                                            \
                       gran = 8;                                      \
                       break;                                         \
                   case 4:                                            \
                       gran = 16;                                     \
                       break;                                         \
                   case 5:                                            \
                       gran = 32;                                     \
                       break;                                         \
                   default:                                           \
                       SHR_ERR_EXIT(SHR_E_INTERNAL);           \
               }

/*!
 * Parse an Extractor granularity and get the
 * extractor attribute index.
 */
#define BCMFP_KEYGEN_EXT_GRAN_TO_ATTR_IDX(gran, attr_idx)             \
               switch (gran) {                                        \
                   case 1:                                            \
                       attr_idx = 0;                                  \
                       break;                                         \
                   case 2:                                            \
                       attr_idx = 1;                                  \
                       break;                                         \
                   case 4:                                            \
                       attr_idx = 2;                                  \
                       break;                                         \
                   case 8:                                            \
                       attr_idx = 3;                                  \
                       break;                                         \
                   case 16:                                           \
                       attr_idx = 4;                                  \
                       break;                                         \
                   case 32:                                           \
                       attr_idx = 5;                                  \
                       break;                                         \
                   default:                                           \
                       SHR_ERR_EXIT(SHR_E_INTERNAL);           \
               }

/*!
 * BCMFP_KEYGEN_EXT_CFG_ADD macro needs the following declaration
 * in any function that uses it.
 */
#define BCMFP_KEYGEN_EXT_CFG_DECL            \
    int _rv_ = SHR_E_NONE;                   \
    int _ext_id_ = 0;                        \
    bcmfp_keygen_ext_cfg_t _ext_cfg_

/*!
 * This macro is used for initializing the
 * ext_cfg databse. The database is used
 * while deriving the keygen extractor programming.
 */
#define BCMFP_KEYGEN_EXT_CFG_INIT(_unit_)                       \
    do {                                                        \
        _rv_ = bcmfp_keygen_ext_cfg_t_init(_unit_, &_ext_cfg_); \
        if (SHR_FAILURE(_rv_)) {                                \
           SHR_ERR_EXIT(_rv_);                           \
        }                                                       \
    } while (0)

/*!
 * This macro is used to encode the
 * part,level,granularity and ext number into the
 * extractor id.
 */
#define BCMFP_KEYGEN_EXT_PLGE_ADD(_unit_,               \
                                  _p_,                  \
                                  _l_,                  \
                                  _g_,                  \
                                  _e_num_)              \
    do {                                                \
        BCMFP_KEYGEN_EXT_ID_CREATE                      \
            (_p_, _l_, _g_, _e_num_, _ext_id_);         \
        (_ext_cfg_).ext_id = (_ext_id_);                \
    } while (0)

/*!
 * This macro is used to derive the one hot extractor id
 * for the given ext and capture it in ext_cfg.onehot_ext_id.
 */
#define BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD(_unit_,        \
                                  _idx_,                \
                                  _p_,                  \
                                  _l_,                  \
                                  _g_,                  \
                                  _e_num_)              \
    do {                                                \
        BCMFP_KEYGEN_EXT_ID_CREATE                      \
            (_p_, _l_, _g_, _e_num_, _ext_id_);         \
        (_ext_cfg_).onehot_ext_id[_idx_] = (_ext_id_);  \
    } while (0)

/*!
 * This macro is used to derive the conflicting extractor id
 * for the given ext and capture it in ext_cfg.conflict_ext_id.
 */
#define BCMFP_KEYGEN_EXT_CONFLICT_PLGE_ADD(_unit_,       \
                                  _idx_,                 \
                                  _p_,                   \
                                  _l_,                   \
                                  _g_,                   \
                                  _e_num_)               \
    do {                                                 \
        BCMFP_KEYGEN_EXT_ID_CREATE                       \
            (_p_, _l_, _g_, _e_num_, _ext_id_);          \
        (_ext_cfg_).conflict_ext_id[_idx_] = (_ext_id_); \
    } while (0)

/*!
 * This macro is used to derive the parent extractor id
 * for the given ext and capture it in ext_cfg.parent_ext_id.
 */
#define BCMFP_KEYGEN_EXT_PARENT_PLGE_ADD(_unit_,       \
                                  _idx_,                 \
                                  _p_,                   \
                                  _l_,                   \
                                  _g_,                   \
                                  _e_num_)               \
    do {                                                 \
        BCMFP_KEYGEN_EXT_ID_CREATE                       \
            (_p_, _l_, _g_, _e_num_, _ext_id_);          \
        (_ext_cfg_).parent_ext_id = (_ext_id_);          \
    } while (0)

/*!
 * This macro is used to add section details
 * to ext_cfg database.
 */
#define BCMFP_KEYGEN_EXT_SECTION_ADD(_unit_,            \
                                     _in_,              \
                                     _idx_,             \
                                     _section_,         \
                                     _offset_)          \
    do {                                                \
        if (_in_) {                                     \
            (_ext_cfg_).in_sec = (_section_);           \
        } else {                                        \
            if (_idx_ >=                                \
                BCMFP_KEYGEN_EXT_MAX_OUT_SECTIOINS) {   \
                SHR_ERR_EXIT(SHR_E_INTERNAL);    \
            }                                           \
            (_ext_cfg_).out_sec[_idx_] = (_section_);   \
            (_ext_cfg_).offset[_idx_] = (_offset_);     \
        }                                               \
    } while (0)

/*!
 * This macro is used to push the ext_cfg into
 * the ext_cfg_db.
 */
#define BCMFP_KEYGEN_EXT_CFG_INSERT(_unit_,                      \
                                    _ext_cfg_db_,                \
                                    _ext_attrs_)                 \
    do {                                                         \
        sal_memcpy(&((_ext_cfg_).ext_attrs),                     \
                   (_ext_attrs_),                                \
                   sizeof(bcmfp_keygen_ext_attrs_t));            \
        _rv_ = bcmfp_keygen_ext_cfg_insert((_unit_),             \
                                           &(_ext_cfg_),         \
                                           (_ext_cfg_db_));      \
        if (SHR_FAILURE(_rv_)) {                                 \
            SHR_ERR_EXIT(_rv_);                           \
        }                                                        \
    } while (0)

/*!
 * Extractor hierarcy construction utility macro.
 */
#define BCMFP_KEYGEN_EXT_CFG_ADD(_unit_, _ext_cfg_db_,                         \
                                _p_, _l_, _g_, _e_num_,                        \
                                _in_sec_, _out_sec_,                           \
                                _offset_, _ext_attrs_)                         \
            do {                                                               \
                _rv_ = bcmfp_keygen_ext_cfg_t_init(unit, &_ext_cfg_);          \
                if (SHR_FAILURE(_rv_)) {                                       \
                    SHR_ERR_EXIT(_rv_);                                 \
                }                                                              \
                BCMFP_KEYGEN_EXT_ID_CREATE(_p_, _l_, _g_, _e_num_, _ext_id_);  \
                (_ext_cfg_).ext_id = (_ext_id_);                               \
                (_ext_cfg_).in_sec = (_in_sec_);                               \
                (_ext_cfg_).out_sec[0] = (_out_sec_);                          \
                (_ext_cfg_).in_use = 0;                                        \
                (_ext_cfg_).offset[0] = (_offset_);                            \
                sal_memcpy(&((_ext_cfg_).ext_attrs),                           \
                           (_ext_attrs_),                                      \
                           sizeof(bcmfp_keygen_ext_attrs_t));                  \
                _rv_ = bcmfp_keygen_ext_cfg_insert((_unit_),                   \
                                         &(_ext_cfg_), (_ext_cfg_db_));        \
                if (SHR_FAILURE(_rv_)) {                                       \
                    SHR_ERR_EXIT(_rv_);                                 \
                }                                                              \
            } while(0)

/*!
 * By default, sum of granularity of all extractors going out of the section
 * is set to drain bits of a section. In some special scenarios (when extractor
 * with programmable granularity is present in sections outgoing extractors),
 * drain bits of section may be less than the sum calculated. In such cases,
 * section drain bits needs to be overwritten with correct value after
 * (strictly) initializing all the extractors using BCMFP_KEYGEN_EXT_CFG_ADD.
 */
#define BCMFP_KEYGEN_EXT_SECTION_DBITS_SET(_unit_, _ext_cfg_db_,                \
                                          _sec_, _drain_bits_)                  \
            do {                                                                \
                _rv_ = bcmfp_keygen_ext_sec_dbit_set((_unit_), (_sec_),         \
                                         (_drain_bits_), (_ext_cfg_db_));       \
                if (SHR_FAILURE(_rv_)) {                                        \
                    SHR_ERR_EXIT(_rv_);                                  \
                }                                                               \
            } while(0)

/*!
 * By default, each section have parents section set to its own section.
 * So each section and its parent section will have same drain bits.
 * But in some cases, section will have its own drain bits and group of
 * sections together should have drain bits less than the sum of all the
 * sections drain bits. In order to manage such hierarchical levels of drain
 * bits, a common parent section can be assigned to a group of sections.
 */
#define BCMFP_KEYGEN_EXT_SECTION_PARENT_SET(_unit_, _ext_cfg_db_,               \
                                           _sec_, _parent_sec_)                 \
            do {                                                                \
                _rv_ = bcmfp_keygen_ext_sec_parent_set((_unit_), (_sec_),       \
                                        (_parent_sec_), (_ext_cfg_db_));        \
                if (SHR_FAILURE(_rv_)) {                                        \
                    SHR_ERR_EXIT(_rv_);                                  \
                }                                                               \
            } while(0)

/*!
 * Max number of chunks a qualifer can have in any of
 * its configurations of type bcmfp_keygen_qual_cfg_t.
 */
#define BCMFP_KEYGEN_QUAL_MAX_CHUNKS 32

/*! Maximum offsets any qualifier can have in final key */
#define BCMFP_KEYGEN_QUAL_OFFSETS_MAX 64

/*! Maximum number of words in qualifier bitmap */
#define BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS 32

/*!
 * \brief Qualifier flags
 *
 * These are used to capture if any special
 * processing is required for the qualifier.
 */
typedef enum bcmfp_keygen_qual_flags_e {
    /*! Qualifier is configuration is post muxed. */
    BCMFP_KEYGEN_QUAL_FLAGS_PMUX = 0,
    /*!
     * Qualifier configuration is not valid if
     * keygen is in mode "HALF".
     */
    BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_HALF,
    /*!
     * Qualifier configuration is not valid if
     * keygen is in mode "DBLINTRA".
     */
    BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_DBLINTRA,
    /*!
     * Qualifier configuration is not valid for
     * Exact Match.
     */
    BCMFP_KEYGEN_QUAL_FLAGS_NOT_IN_EM,
    /*! Always Last. Not a usable value. */
    BCMFP_KEYGEN_QUAL_FLAGS_COUNT
} bcmfp_keygen_qual_flags_t;

/*!
 * Qualifier flags set used in key generation algorithm to
 * validate the qualifier configuration.
 */
typedef struct bcmfp_keygen_qual_flags_bmp_s {
    /*! Bitmap of all the valid qual flags for a given qualifier. */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_KEYGEN_QUAL_FLAGS_COUNT)];
} bcmfp_keygen_qual_flags_bmp_t;

/*! Qualifier chunk information. */
typedef struct bcmfp_keygen_qual_offset_cfg_s {
    /*! section in input bus from which this chunk is fed to extraction. */
    bcmfp_keygen_ext_section_t section;
    /*!
     * chunk number. For example , if a section has 10 chunks,
     * then this field will be set to any one value from 0 to 9 which
     * is appropriate to this chunk.
     */
    uint16_t sec_val;
    /*!
     * Required number bits in this chunk for the qualifier this chunk
     * information belongs to. This will be any value between 1 to ?size?.
     */
    uint16_t width;
    /*!
     * start bit(w.r.t total width of the section) from where ?width?
     * number of bits in this chunk are required for the qualifier.
     */
    uint16_t offset;
    /*!
     * Size of the chunk. Generally all chunks in a section will
     * have the same size.
     */
    uint16_t size;
} bcmfp_keygen_qual_offset_cfg_t;

/*! Qualifier configuration */
typedef struct bcmfp_keygen_qual_cfg_s {
    /*!
     * keygen algorithm uses these flags to validate this configuration
     * for the given keygen mode(bcmfp_keygen_mode_t).
     */
    bcmfp_keygen_qual_flags_bmp_t qual_flags;
    /*!
     * Qualifier control to be reserved in case keygen algorithm is using
     * this qualifier configuration.
     */
    bcmfp_keygen_ext_ctrl_sel_t pri_ctrl_sel;
    /*!
     * "ctrl_sel" should be set to this value in case this configuration is
     * successfully used by keygen algorithm.
     */
    uint16_t pri_ctrl_sel_val;
    /*!
     * Qualifer chunk information for different chunks belongs to this
     * qualifier configuration. Chunks may either come from same section
     * or from different sections in the input bus.
     */
    bcmfp_keygen_qual_offset_cfg_t e_params[BCMFP_KEYGEN_QUAL_MAX_CHUNKS];
    /*!
     * number of valid elements in the e_params array. Valid values of this
     * field ranges between 1 to BCMFP_KEYGEN_QUAL_MAX_CHUNKS.
     */
    uint8_t num_chunks;

    /*! Size of all valid chunks together. */
    uint32_t size;

     /*! Number of updates required to accomodate qualifier  */
    uint8_t update_count;
} bcmfp_keygen_qual_cfg_t;

/*! All possible qualifiers configurations of a single qualifier. */
typedef struct bcmfp_keygen_qual_cfg_info_s {
    /*!
     * Qualifier Identifier.  Keygen library uses qualifier identifiers
     * created in field module.
     */
    bcmfp_qualifier_t qual;
    /*!
     * Array of qualifier configurations of a qualifier. Most of the
     * qualifiers have single configurations but some have more
     * than one configuration to choose.
     */
    bcmfp_keygen_qual_cfg_t *qual_cfg_arr;
     /*! "qual_cfg_arr" size */
     uint16_t num_qual_cfg;
} bcmfp_keygen_qual_cfg_info_t;

/*!
 * Qualifier configuration information of all qualifiers supported
 * on the device.
 */
typedef struct bcmfp_keygen_qual_cfg_info_db_s {
    /*! Qualifiers names */
    const char *qual_name[BCMFP_QUALIFIERS_COUNT];

    /*!
     * Number of valid qualifiers in the database. This value is anywhere
     * in between 1 to BCMFP_QUALIFIERS_COUNT.
     */
    int num_quals;
    /*!
     * Holds pointers to Qualifier Configuration information for
     * each supported qualifier. If some qualifier is not supported
     * on the device then its pointer is set to NULL.
     */
    bcmfp_keygen_qual_cfg_info_t *qual_cfg_info[BCMFP_QUALIFIERS_COUNT];
} bcmfp_keygen_qual_cfg_info_db_t;

/*!
 * BCMFP_KEYGEN_QUAL_CFG_XXX macros require the following declaration in any
 * function which uses them.
 */
#define BCMFP_KEYGEN_QUAL_CFG_DECL          \
    int _rv_;                               \
    int _i_;                                \
    bcmfp_keygen_qual_cfg_t _qual_cfg_

/*! To re-init the bcmfp_keygen_qual_cfg_t structure. */
#define BCMFP_KEYGEN_QUAL_CFG_INIT(_unit_)                       \
    _rv_ = bcmfp_keygen_qual_cfg_t_init(_unit_, &(_qual_cfg_));  \
    if (SHR_FAILURE(_rv_)) {                            \
        SHR_ERR_EXIT(_rv_);                      \
    }

/*!
 * To update the selectors, offset and width of the
 * bcmfp_keygen_qual_cfg_t variable based on the index.
 */
#define BCMFP_KEYGEN_QUAL_CFG_OFFSET_ADD(_index_,                          \
                                        _section_,                         \
                                        _sec_val_,                         \
                                        _offset_,                          \
                                        _width_)                           \
                (_qual_cfg_).e_params[_index_].section      = (_section_); \
                (_qual_cfg_).e_params[_index_].sec_val      = (_sec_val_); \
                (_qual_cfg_).e_params[_index_].offset       = (_offset_);  \
                (_qual_cfg_).e_params[_index_].width        = (_width_);   \
                (_qual_cfg_).num_chunks++;

/*!
 * To update the control selectors and its value in the
 * bcmfp_keygen_qual_cfg_t structure.
 */
#define BCMFP_KEYGEN_QUAL_CFG_CTRL_SEL_ADD(_ctrl_sel_, _ctrl_sel_val_)   \
                (_qual_cfg_).pri_ctrl_sel     = (_ctrl_sel_);           \
                (_qual_cfg_).pri_ctrl_sel_val = (_ctrl_sel_val_);       \

/*!
 * To update the control selectors and its value in the
 * bcmfp_keygen_qual_cfg_t structure.
 */
#define BCMFP_KEYGEN_QUAL_CFG_INSERT(_unit_, _qual_cfg_info_db_,            \
                                    _qual_id_, _qual_flags_)                \
    do {                                                                    \
       for (_i_ = 0; _i_ < SHRi_BITDCLSIZE(BCMFP_KEYGEN_QUAL_FLAGS_COUNT);  \
                                                                _i_++ )  {  \
            (_qual_cfg_).qual_flags.w[_i_] = (_qual_flags_).w[_i_];         \
       }                                                                    \
      _rv_ = bcmfp_keygen_qual_cfg_insert((_unit_),                         \
                                          (_qual_id_),                      \
                                          &(_qual_cfg_),                    \
                                          (_qual_cfg_info_db_));            \
      if (SHR_FAILURE(_rv_)) {                                              \
          SHR_ERR_EXIT(_rv_);                                               \
      }                                                                     \
    } while (0)

/*!
 * Qualifier flag to use new finfo while converting this qualifier
 * bitmap to finfo. Default behavior of algorithm is to search in
 * assigned finfo for given section/sec_val/pri_ctrl_sel/
 * pri_ctrl_sel_val. If a match is found, that finfo is updated with
 * bitmap of this new qualifer. This flag override this default behavior
 * and valid only for flowtracker style extractors. Using this flag
 * in ifp style extractors will give unexpected results.
 */
#define BCMFP_KEYGEN_QUAL_F_USE_FINFO_NEW                    (1 << 0)

/*!
 * Qualifier flags to use extractor of granularity 16 at level 2
 * to extract this qualifier bitmap. Default behavior is to loop
 * through all extractors at given level and try to extract all
 * required bit in given finfo using extractors of any granularity
 * available at given section in given level. This flags override
 * this behavior and valid only for flowtracker style extractors.
 * To use this flag,
 * a. Qualifier size must be equal or less than 16 bits.
 *
 * Qual flags BCMFP_KEYGEN_QUAL_F_USE_FINFO_NEW can also be
 * combined with this flag. Using this flag in ifp style extractors
 * will give unexpected results.
 */
#define BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16                (1 << 1)

/*!
 * Qualifier flags to use extractor of granularity 8 at level 2
 * to extract this qualifier bitmap. Default behavior is to loop
 * through all extractors at given level and try to extract all
 * required bit in given finfo using extractors of any granularity
 * available at given section in given level. This flags override
 * this behavior and valid only for flowtracker style extractors.
 * To use this flag,
 * a. Qualifier size must be less than 8 bits.
 * b. Either or both of qual flag BCMFP_KEYGEN_QUAL_F_USE_FINFO_NEW
 *    and BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16  can be used.
 * c. DO NOT use only this flag alone. It might fail to allocate
 *    extractors even if extraction is possible.
 *
 * Using this flag in ifp style extractors will give unexpected
 * results.
 */
#define BCMFP_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8                 (1 << 2)

/*!
 * Qualifier has to be extrctaed to modulo 32 offsets in the final
 * key.In general keygen algorithm doesnot give any preference to
 * any qualifier while placing it in the final key. But this flag
 * ovverrides this behaviour.
 */
#define BCMFP_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET     (1 << 3)

/*!
 * This flag will make sure that all input bus containers related to
 * a qualifier are extrcted by consecutive muxers of same part of the
 * muxer hierarchy.
 */
#define BCMFP_KEYGEN_QUAL_F_STICKY_CONTAINERS               (1 << 4)

/*! Qualifier information */
typedef struct bcmfp_keygen_qual_info_s {
    /*! Qualifier flags */
    uint32_t flags;
    /*! Need to extract all bits in the qualifier or some? */
    uint8_t partial;
    /*! Qualifier Id. */
    bcmfp_qualifier_t qual_id;
    /*!
     * bits required if partial is set to TRUE. otherwise,
     * ignore this field.
     */
    uint32_t bitmap[BCMFP_KEYGEN_QUAL_BITMAP_MAX_WORDS];
} bcmfp_keygen_qual_info_t;

/*! Offset and width information of a qualifer in final key of a given part */
typedef struct bcmfp_keygen_qual_offset_s {
    /*! Number of Offsets. */
    uint8_t num_offsets;
    /*! offset. */
    uint16_t offset[BCMFP_KEYGEN_QUAL_OFFSETS_MAX];
    /*! width from the corresponding offset. */
    uint8_t width[BCMFP_KEYGEN_QUAL_OFFSETS_MAX];
} bcmfp_keygen_qual_offset_t;

/*! All qualifiers offset and width information in final key of a given part */
typedef struct bcmfp_keygen_qual_offset_info_s {
    /*! Array qualifiers present in the part given */
    bcmfp_qualifier_t *qid_arr;
    /*! offset and width information corresponding to qualifiers in qid_arr. */
    bcmfp_keygen_qual_offset_t *offset_arr;
    /*! Size of the qid_arr and offset_arr arrays */
    uint16_t size;
} bcmfp_keygen_qual_offset_info_t;

/*!
 * Validate whether extractors can be allocated for all
 * qualifiers in the given keygen mode. Dont need to generate
 * the offsets of qualifiers in the final key.
 */
#define BCMFP_KEYGEN_CFG_FLAGS_VALIDATE_ONLY (1 << 0)
/*!
 * Keygen algorithm has generated operation information for the given
 * keygen configuration but applicatipon wants to add more configuration
 * (like adding more qualifiers) to it and keygen algorithm has regenerate
 * the operational information for the newly added configuration. During
 * this process keygen algorithm should not use resources(like multiplexers)
 * that are in use for old keygen configuration.
 */
#define BCMFP_KEYGEN_CFG_FLAGS_QSET_UPDATE   (1 << 1)

/*!
 * When num_parts in keygen configuration is more than one, then input bus
 * containers corresponding to all qualifiers are distributed to all parts
 * symmetrically. That means if two 16 bit container needs to be extracted
 * from two parts, first container is extracted from part one and second one
 * is extracted from part2 instead of extracting both containers from first
 * part. If this flag is set, then all muxers are used in part one first and
 * then go to 2nd part, 3rd part and so on.
 */
#define BCMFP_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC (1 << 2)

/*!
 * Keygen algorithm builds list of containers corresponding to all
 * qualifiers and sort the list in the descending order based on the
 * container size. This flag will voids the sorting of the containers.
 * This is the requirement for FlowTracker groups in some cases.
 */
#define BCMFP_KEYGEN_CFG_FLAGS_SERIAL_QUAL_CONTAINERS (1 << 3)
 /*!
 * If set, the keygen tries to generate the key with the
 * max qualifier offset not exceeding
 * bcmfp_keygen_cfg_t->max_key_offset
 */
#define BCMFP_KEYGEN_CFG_FLAGS_QUAL_MAX_OFFSET (1 << 4)

/*! Keygen Configuration to be used to allocate multiplexers */
typedef struct bcmfp_keygen_cfg_s {
    /*! Flags(BCMFP_KEYGEN_CFG_FLAGS_XXX). */
    uint32_t flags;
    /*! keygen mode */
    bcmfp_keygen_mode_t mode;
    /*! Information about the qualifiers in the final key */
    bcmfp_keygen_qual_info_t *qual_info_arr;
    /*! size of qual_info_arr array */
    uint8_t qual_info_count;
    /*!
     * Number of parts. Generally it depends on keygen mode.
     * 1 in case of keygen mode SINGLE, 2 in case of DBLINTRA,
     * 3 in case of TRIPLE. But there are other modes as well for
     * which num parts cannot be assumed by keygen library so is
     * the reason for creation of this field.
     */
    uint8_t num_parts;
    /*! Qualifiers configuration information database */
    bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db;
    /*!
     * Extractor configuration database. Applications may have multiple
     * extractor databases but need to pass the right pointer for keygen mode.
     */
    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db;
    /*!
     * Different controls generated by the keygen algorithm. This is required
     * if application need to update the existing QSET.
     */
    bcmfp_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info;
    /*! Size of ext_ctrl_sel_info array */
    uint16_t ext_ctrl_sel_info_count;

    /*! Max offset of any qualifier in the keyset.
     * This controls the max key width by restricting
     * the max (qualifier offset + width) in the key
     * to max_key_offset.
     */
    uint16_t qual_max_offset;
} bcmfp_keygen_cfg_t;

/*! Operational data generated by keygen algorithm */
typedef struct bcmfp_keygen_oper_s {
    /*!
     * Final key offset information of each qualifier specified
     * in keygen_cfg.
     */
    bcmfp_keygen_qual_offset_info_t *qual_offset_info;
    /*!
     * Different controls generated by the keygen algorithm.
     * Calling module should push this information to h/w.
     */
    bcmfp_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info;
    /*! Size of ext_ctrl_sel_info array */
    uint16_t ext_ctrl_sel_info_count;

    /*! Size of the final key in bits. */
    uint16_t key_size;
} bcmfp_keygen_oper_t;

/*!
 * \brief Keygen Configuration Process.
 *
 * This function allocated the space for metadata
 * used for the keygen extraction and processes the
 * given qualifiers to derive the extractor config
 * to be programmed in HW.
 *
 * \param [in] unit Unit number.
 * \param [in] keygen_cfg keygen configuration.
 * \param [in] keygen_oper operation data generated by
 *             keygen algorithm.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_cfg_process(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        bcmfp_keygen_oper_t *keygen_oper);
/*!
 * \brief Qualifier Config DB init.
 *
 * This function is used to initialize the qualifier
 * cfg database to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] qual_cfg Qualifier Configuration.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_qual_cfg_t_init(int unit,
                             bcmfp_keygen_qual_cfg_t *qual_cfg);
/*!
 * \brief Qualifier Config DB insert.
 *
 * This function is used to update the control selectors and its value in the
 * bcmfp_keygen_qual_cfg_t structure.
 *
 * \param [in] unit Unit number.
 * \param [in] qual_id Qualifier ID.
 * \param [in] new_qual_cfg Qualifier Config.
 * \param [in] qual_cfg_info_db Qualifier Config DB.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_qual_cfg_insert(int unit,
                             bcmfp_qualifier_t qual_id,
                             bcmfp_keygen_qual_cfg_t *new_qual_cfg,
                             bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db);
/*!
 * \brief Extractor Config Insert.
 *
 * This function is used to push the ext_cfg into
 * the ext_cfg_db.
 *
 * \param [in] unit Unit number.
 * \param [in] ext_cfg Extractor Config.
 * \param [in] ext_cfg_db Extractor Config DB.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_cfg_insert(int unit,
                           bcmfp_keygen_ext_cfg_t *ext_cfg,
                           bcmfp_keygen_ext_cfg_db_t *ext_cfg_db);
/*!
 * \brief Extractor Config Init.
 *
 * This function is used to initialize the ext_cfg to 0.
 *
 * \param [in] unit Unit number.
 * \param [in] ext_cfg Extractor Config.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_cfg_t_init(int unit,
                           bcmfp_keygen_ext_cfg_t *ext_cfg);

/*!
 * \brief Extractor Config DB Free.
 *
 * This function is used to free the memory used by
 * the extractor config database.
 *
 * \param [in] unit Unit number.
 * \param [in] ext_cfg_db Extractor Config DB.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_cfg_db_free(int unit, bcmfp_keygen_ext_cfg_db_t *ext_cfg_db);

/*!
 * \brief Keygen oper info free.
 *
 * This function is used to free the memory used
 * by the keygen operational info DB.
 *
 * \param [in] unit Unit number.
 * \param [in] num_parts number of slices in device.
 * \param [in] keygen_oper keygen operational info.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_oper_free(int unit,
                      uint16_t num_parts,
                      bcmfp_keygen_oper_t *keygen_oper);
/*!
 * \brief KeySize extraction.
 *
 * This function is used to get the total keysize
 * of all the qualifier in keygen_cfg DB.
 *
 * \param [in] unit Unit number.
 * \param [in] keygen_cfg keygen config DB.
 * \param [in] rkey_size keysize of all the qualifiers in
 *             keygen_cfg DB.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_keysize_get(int unit,
                        bcmfp_keygen_cfg_t *keygen_cfg,
                        uint16_t *rkey_size);

/*!
 * \brief Overwrite drain bits for given section.
 *
 * This function is used to overide the drain bit
 * of the given sec_cfg.
 *
 * \param [in] unit Unit Number
 * \param [in] sec section-id enum
 * \param [in] drain_bits drain bits to be overridden
 * \param [in] ext_cfg_db Extractor Config DB
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_sec_dbit_set(int unit,
                              bcmfp_keygen_ext_section_t sec,
                              int drain_bits,
                              bcmfp_keygen_ext_cfg_db_t *ext_cfg_db);

/*!
 * \brief Overwrite parent section for given section.
 *
 * This function is used to overide the parent section
 * of the given sec_cfg.
 *
 * \param [in] unit Unit Number
 * \param [in] sec section-id enum
 * \param [in] parent_sec parent section to be overridden
 * \param [in] ext_cfg_db Extractor Config DB
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_sec_parent_set(int unit,
                                bcmfp_keygen_ext_section_t sec,
                                bcmfp_keygen_ext_section_t parent_sec,
                                bcmfp_keygen_ext_cfg_db_t *ext_cfg_db);

/*!
 * \brief Free extractor Config DB.
 *
 * This function is used to free the memory used
 * by the ext_cfg_db.
 *
 * \param [in] unit Unit Number
 * \param [in] ext_cfg_db extractor config database
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_ext_cfg_db_cleanup(int unit,
                               bcmfp_keygen_ext_cfg_db_t *ext_cfg_db);

/*!
 * \brief Free Qualifier Config DB.
 *
 * This function is used to free the memory used
 * by the qual_cfg_info_db.
 *
 * \param [in] unit Unit Number
 * \param [in] qual_cfg_info_db Qualifier Config DB
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_qual_cfg_info_db_free(int unit,
                                   bcmfp_keygen_qual_cfg_info_db_t *qual_cfg_info_db);

/*!
 * \brief Max Qualifier Size Get.
 *
 * This function is used to get the maximum size
 * of the given qualifier in the keygen_cfg qualifier db.
 *
 * \param [in] unit Unit number.
 * \param [in] keygen_cfg keygen config DB.
 * \param [in] qual_id Qualifier ID.
 * \param [in] qual_cfg_max_size max size of the qualifier.
 *
 * \return SHR_E_NONE on success and error code on failure.
 */
extern int
bcmfp_keygen_qual_cfg_max_size_get(int unit,
                                   bcmfp_keygen_cfg_t *keygen_cfg,
                                   bcmfp_qualifier_t qual_id,
                                   uint16_t *qual_cfg_max_size);
#endif /* BCMFP_KEYGEN_API_H */

