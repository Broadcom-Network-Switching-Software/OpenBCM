/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_FIELD_KEYGEN_API_H
#define _BCM_INT_FIELD_KEYGEN_API_H

#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/bitop.h>
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <bcm_int/esw/keygen_error.h>

/* Maxmimum number of slices occupied by any
 * keygen mode in any device.
 */
#define BCMI_KEYGEN_ENTRY_PARTS_MAX 4

/* Different keygen modes */
typedef enum bcmi_keygen_mode_e {
     /* Mode HALF. This mode will have half of the key
      * size of SINGLE mode.
      */
    BCMI_KEYGEN_MODE_HALF = 0,
     /* Single wide mode which uses one slice only */
    BCMI_KEYGEN_MODE_SINGLE = 1,
     /* Single wide mode which uses one slice only,
      * but have narrow ASET. Used in EM HASH FP.
      */
    BCMI_KEYGEN_MODE_SINGLE_ASET_NARROW = 2,
     /* Single wide mode which uses one slice only,
      * but have wider ASET. Used in EM HASH FP.
      */
    BCMI_KEYGEN_MODE_SINGLE_ASET_WIDE = 3,
    /* Uses single slice but key size is double the key
     * size of SINGLE mode.
     */
    BCMI_KEYGEN_MODE_DBLINTRA = 4,
    /* Uses two slices and key size is double the key
     * size of SINGLE mode.
     */
    BCMI_KEYGEN_MODE_DBLINTER = 5,
    /* Uses two slices and key size is triple the key
     * size of SINGLE mode.
     */
    BCMI_KEYGEN_MODE_TRIPLE = 6,
    /* Uses two slices and key size is four times the key
     * size of SINGLE mode.
     */
    BCMI_KEYGEN_MODE_QUAD = 7,
    /* Always Last. Not a usable value. */
    BCMI_KEYGEN_MODE_COUNT = 8
} bcmi_keygen_mode_t;

/* extractor attributes used to validity of extractor configuration
 * for a given keygen mode and qualifer list.
 * Note: enumerations in bcmi_keygen_ext_attr_t must have 1-to-1
 * mapping with enumerations in bcmi_keygen_acl_attr_t.
 */
typedef enum bcmi_keygen_ext_attr_s {
     /* Extractor can operate in multiple granularities. */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN = 0,
    /* Extractor can operate in granularity 1 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_1,
    /* Extractor can operate in granularity 2 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_2,
    /* Extractor can operate in granularity 4 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_4,
    /* Extractor can operate in granularity 8 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_8,
    /* Extractor can operate in granularity 16 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_16,
    /* Extractor can operate in granularity 32 */
    BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_32,
    /* Extractor is extracts bits from the pass through section. */
    BCMI_KEYGEN_EXT_ATTR_PASS_THRU,
    /* Extractor cannot be used when keygen mode is HALF */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_HALF,
    /* Extractor cannot be used if IPBM is part of final key. */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM,
    /* Extractor cannot be used if SRC_DST_CONT_0 LSB 16bits
     * (Total length is 17 bits), is part of final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0,
    /* Extractor cannot be used if SRC_DST_CONT_1 LSB 16bits
     * (Total length is 17 bits), is part of final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1,
    /* Extractor cannot be used if MSB bits of SRC_DST_CONT_0
     * (or) SRC_DST_CONT_1, is part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB,
    /* Extractor cannot be used if SRC_DST_CONT_0 LSB 16bits
     * (Total length is 17 bits), is part of final key of
     * HALF(80bit) mode group.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_0_HM,
    /* Extractor cannot be used if SRC_DST_CONT_1 LSB 16bits
     * (Total length is 17 bits), is part of final key of
     * HALF(80bit) mode group.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_1_HM,
    /* Extractor cannot be used if MSB bits of SRC_DST_CONT_0
     * (or) SRC_DST_CONT_1, is part of the final key of HALF
     * (80bit) mode group.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_SRC_DST_CONT_MSB_HM,
    /* Extractor cannot be used if post mux qualifier, Drop,
     * is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP,
    /* Extractor cannot be used if LSB 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3,
    /* Extractor cannot be used if middle 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7,
    /* Extractor cannot be used if MSB 4 bits of class id, generated
     * in second lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11,
    /* Extractor cannot be used if LSB 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3,
    /* Extractor cannot be used if middle 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7,
    /* Extractor cannot be used if MSB 4 bits of class id, generated
     * in first lookup of Exact Match Rules, are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11,
    /* Extractor cannot be used if post mux qualifier, HIT status from Exact
     * Match first lookup,  is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT,
    /* Extractor cannot be used if post mux qualifier, HIT status from Exact
     * Match second lookup,  is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_SECOND_LOOKUP_HIT,
    /* Extractor cannot be used if post mux qualifier, NatNeeded,
     * is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_NEEDED,
    /* Extractor cannot be used if post mux qualifier, NatDstRealmId,
     * is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_NAT_DST_REALM_ID,
    /* Extractor cannot be used if LSB 4 bits of lookup status vector,
     * are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3,
    /* Extractor cannot be used if MSB 4 bits of lookup status vector,
     * are part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7,
    /* Extractor cannot be used if 17 bits of SRC_DST_CONT_A
     * is part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A,
    /* Extractor cannot be used if 17 bits of SRC_DST_CONT_B
     * is part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B,
    /* Extractor cannot be used if post mux qualifier, PacketRes,
     * is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION,
    /* Extractor cannot be used if 16 bits of CLASS_ID C
     * container is part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C,
    /* Extractor cannot be used if 16 bits of CLASS_ID D
     * container is part of the final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D,
    /* Extractor cannot be used if post mux qualifier,
     * Internal Priority, is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI,
    /* Extractor cannot be used if drop Reasons generated in
     * ingress pipeline before IFP are part of final key.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR,
    /* Extractor cannot be used if post mux qualifier,
     * HiGiG header opcode, is present in the QSET.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE,
    /* Extractor cannot be used if keygen mode is EM_128. */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_MODE_SINGLE_ASET_WIDE,
    /* Extractor cannot be used if keygen algorithm is called for
     * QSET update.
     */
    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_QSET_UPDATE,

    BCMI_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LTID,
    /* Always Last. Not a usable value. */
    BCMI_KEYGEN_EXT_ATTR_COUNT
} bcmi_keygen_ext_attr_t;

/* Bitmap of extractor attributes. */
typedef struct bcmi_keygen_ext_attrs_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCMI_KEYGEN_EXT_ATTR_COUNT)];
} bcmi_keygen_ext_attrs_t;

/* All the different qualifier controls to serve all keygen blocks
 * in different chips.
 */
typedef enum bcmi_keygen_ext_ctrl_sel_e {
    /* Selector disabled. */
    BCMI_KEYGEN_EXT_CTRL_SEL_DISABLE            = 0,
    /* Auxiliary Tag A selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A          = 1,
    /* Auxiliary Tag B selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B          = 2,
    /* Auxiliary Tag C selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C          = 3,
    /* Auxiliary Tag D selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D          = 4,
    /* TCP function selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN             = 5,
    /* ToS function selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN             = 6,
    /* TTL function selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN             = 7,
    /* Class ID container A selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A          = 8,
    /* Class ID container B selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B          = 9,
    /* Class ID container C selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C          = 10,
    /* Class ID container D selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D          = 11,
    /* Source Container A selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A              = 12,
    /* Source Container B selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B              = 13,
    /* Src/Dst Container 0 selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0          = 14,
    /* Src/Dst Container 1 selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1          = 15,
    /* IPBM Source. */
    BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT       = 16,
    /* Multiplexer in extractor hierarchy. */
    BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER        = 17,
    /* Normalize L3 and L4 addresses. */
    BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM         = 18,
    /* Normalize Mac address. */
    BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM           = 19,
    /* PostMux Qualifier. */
    BCMI_KEYGEN_EXT_CTRL_SEL_PMUX               = 20,
    /* UDF Qualifiers. */
    BCMI_KEYGEN_EXT_CTRL_SEL_UDF                = 21,
    /* Destination Container A selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_DST_A              = 22,
    /* Destination Container B selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_DST_B              = 23,
    /* ALT TTL function selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN         = 24,
    /* LTID function selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_LTID               = 25,
    /* ING_FIELD_BUS Container selector. */
    BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT      = 26,
    /* Always Last. Not a usable value. */
    BCMI_KEYGEN_EXT_CTRL_SEL_COUNT              = 27
} bcmi_keygen_ext_ctrl_sel_t;

/* All the different hairarchical bus sections to
 * serve all keygen blocks in different chips.
 */
typedef enum bcmi_keygen_ext_section_e {
    /* Selector disabled. */
    BCMI_KEYGEN_EXT_SECTION_DISABLE = 0,
    /* Level-1 32 bit extractor. */
    BCMI_KEYGEN_EXT_SECTION_L1E32,
    /* Level-1 16 bit extractor. */
    BCMI_KEYGEN_EXT_SECTION_L1E16,
    /* Level-1 8 bit extractor. */
    BCMI_KEYGEN_EXT_SECTION_L1E8,
    /* Level-1 4 bit extractor. */
    BCMI_KEYGEN_EXT_SECTION_L1E4,
    /* Level-1 2 bit extractor. */
    BCMI_KEYGEN_EXT_SECTION_L1E2,
    /* Level-2 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S1,
    /* Level-2 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S2,
    /* Level-2 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S3,
    /* Level-2 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S4,
    /* Level-2 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S5,
    /* Level-2 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S6,
    /* Level-2 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S7,
    /* Level-2 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S8,
    /* Level-2 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S9,
    /* Level-2 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2S10,
    /* Level-3 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S1,
    /* Level-3 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S2,
    /* Level-3 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S3,
    /* Level-3 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S4,
    /* Level-3 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S5,
    /* Level-3 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S6,
    /* Level-3 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S7,
    /* Level-3 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S8,
    /* Level-3 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S9,
    /* Level-3 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3S10,
    /* Level-2 Slice A 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS1,
    /* Level-2 Slice A 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS2,
    /* Level-2 Slice A 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS3,
    /* Level-2 Slice A 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS4,
    /* Level-2 Slice A 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS5,
    /* Level-2 Slice A 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS6,
    /* Level-2 Slice A 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS7,
    /* Level-2 Slice A 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS8,
    /* Level-2 Slice A 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS9,
    /* Level-2 Slice A 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2AS10,
    /* Level-2 Slice B 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS1,
    /* Level-2 Slice B 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS2,
    /* Level-2 Slice B 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS3,
    /* Level-2 Slice B 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS4,
    /* Level-2 Slice B 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS5,
    /* Level-2 Slice B 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS6,
    /* Level-2 Slice B 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS7,
    /* Level-2 Slice B 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS8,
    /* Level-2 Slice B 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS9,
    /* Level-2 Slice B 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2BS10,
    /* Level-2 Slice C 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS1,
    /* Level-2 Slice C 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS2,
    /* Level-2 Slice C 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS3,
    /* Level-2 Slice C 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS4,
    /* Level-2 Slice C 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS5,
    /* Level-2 Slice C 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS6,
    /* Level-2 Slice C 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS7,
    /* Level-2 Slice C 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS8,
    /* Level-2 Slice C 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS9,
    /* Level-2 Slice C 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L2CS10,
    /* Level-3 Slice A 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS1,
    /* Level-3 Slice A 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS2,
    /* Level-3 Slice A 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS3,
    /* Level-3 Slice A 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS4,
    /* Level-3 Slice A 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS5,
    /* Level-3 Slice A 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS6,
    /* Level-3 Slice A 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS7,
    /* Level-3 Slice A 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS8,
    /* Level-3 Slice A 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS9,
    /* Level-3 Slice A 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3AS10,
    /* Level-3 Slice B 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS1,
    /* Level-3 Slice B 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS2,
    /* Level-3 Slice B 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS3,
    /* Level-3 Slice B 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS4,
    /* Level-3 Slice B 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS5,
    /* Level-3 Slice B 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS6,
    /* Level-3 Slice B 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS7,
    /* Level-3 Slice B 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS8,
    /* Level-3 Slice B 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS9,
    /* Level-3 Slice B 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3BS10,
    /* Level3 Slice C 1st extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS1,
    /* Level-3 Slice C 2nd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS2,
    /* Level-3 Slice C 3rd extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS3,
    /* Level-3 Slice C 4th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS4,
    /* Level-3 Slice C 5th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS5,
    /* Level-3 Slice C 6th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS6,
    /* Level-3 Slice C 7th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS7,
    /* Level-3 Slice C 8th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS8,
    /* Level-3 Slice C 9th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS9,
    /* Level-3 Slice C 10th extractor section. */
    BCMI_KEYGEN_EXT_SECTION_L3CS10,
    /* Final Key Section. */
    BCMI_KEYGEN_EXT_SECTION_FK,
    /* Slice A Final Key section. */
    BCMI_KEYGEN_EXT_SECTION_FKA,
    /* Slice B Final Key section. */
    BCMI_KEYGEN_EXT_SECTION_FKB,
    /* Slice C Final Key section. */
    BCMI_KEYGEN_EXT_SECTION_FKC,
    /* Section for post Mux qualifier "Ingress Port bitmap"
     * in the final key. */
    BCMI_KEYGEN_EXT_SECTION_IPBM,
    /* Section for post Mux qualifier "DROP"
     * in the final key. */
    BCMI_KEYGEN_EXT_SECTION_DROP,
    /* Section for post Mux qualifier "NAT Needed"
     * in the final key. */
    BCMI_KEYGEN_EXT_SECTION_NAT_NEEDED,
    /* Section for post Mux qualifier "NAT destination realm Id"
     * in the final key. */
    BCMI_KEYGEN_EXT_SECTION_NAT_DST_REALMID,
    /* Section for post Mux qualifier "HIT status of first
     * lookup in Exact Match rules" in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT,
    /* Section for post Mux qualifier "HIT status of second
     * lookup in Exact Match rules" in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT,
    /* Section for post Mux qualifier "LSB 4 bits of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3,
    /* Section for post Mux qualifier "bits 4 - 7 of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7,
    /* Section for post Mux qualifier "bits 8 - 11 of 12 Class Id
     * generated in first lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11,
    /* Section for post Mux qualifier "LSB 4 bits of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3,
    /* Section for post Mux qualifier "bits 4 - 7 of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7,
    /* Section for post Mux qualifier "bits 8 - 11 of 12 Class Id
     * generated in second lookup in Exact Match rules" in the
     * final key.
     */
    BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11,
    /* Section for post Mux qualifiers LSB 16 bits, sharing the
     * source destination container 0, in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0,
    /* Section for post Mux qualifiers LSB 16 bits, sharing the
     * source destination container 1, in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1,
    /* Section for post Mux qualifiers MSB bit, sharing the source
     * destination container 0/1, in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB,
    /* Section for post Mux qualifier Lookup up status vectors
     * LSB 4 bits in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_0_3,
    /* Section for post Mux qualifier Lookup up status vectors
     * MSB 4 bits in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_LOOKUP_STATUS_VECTOR_BITS_4_7,
    /* Section for post Mux qualifiers, sharing the destination
     * container 0(all 17 bits), in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_A,
    /* Section for post Mux qualifiers, sharing the destination
     * container 1(all 17 bits), in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_DST_CONTAINER_B,
    /* Section for post Mux qualifier packet resolution
     * in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_PKT_RESOLUTION,
    /* Section for post Mux qualifiers, sharing the class id
     * container C, in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_CLASS_ID_C,
    /* Section for post Mux qualifiers, sharing the class id
     * container D, in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_CLASS_ID_D,
    /* Section for post Mux qualifier internal priority
     * in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_INT_PRI,
    /* Section for post Mux qualifier "drop reason codes in
     * pipeline fed to IFP" in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_IFP_DROP_VECTOR,
    /* Section for post Mux qualifier "HiGiG header opcode"
     * in the final key.
     */
    BCMI_KEYGEN_EXT_SECTION_MH_OPCODE,
    /* Section for post Mux qualifier "IP Type"
     * in the final key. */
    BCMI_KEYGEN_EXT_SECTION_LTID,
    /* Always Last. Not a usable value. */
    BCMI_KEYGEN_EXT_SECTION_COUNT
} bcmi_keygen_ext_section_t;

/* extractor control information */
typedef struct bcmi_keygen_ext_ctrl_sel_info_s {
    /* Extractor control type. */
    bcmi_keygen_ext_ctrl_sel_t ctrl_sel;
    /* Selector value */
    uint8 ctrl_sel_val;
    /* part to which this selector belongs to */
    uint8 part;
    /* extractor hierarchy level. May not be required for all types
     * of extractor controls.
     */
    uint8 level;
    /* Granularity of the extractor. May not be required for all types
     *  of extractor controls.
     */
    uint8 gran;
    /* Extractor number. May not be required for all types
     *  of extractor controls.
     */
    uint16 ext_num;
} bcmi_keygen_ext_ctrl_sel_info_t;

/* Maximum number of extractor hirerachy levels on any device */
#define BCMI_KEYGEN_EXT_LEVEL_COUNT 5

/* Single extractor configuration. */
typedef struct bcmi_keygen_ext_cfg_s {
    /* Run time field used to know whether the extractor
     * is already used or not. By default it is set to zero.
     */
    uint8  in_use;
    /* Extractor Id which holds 4 tuple (part, level, granularity, extractor number)
     * information of an extractor.
     * 10 bits[0 - 9]  = entractor number.
     * 8 bits[10 - 17] = granularity.
     * 4 bits[18 - 21] = hierarchy level.
     * 2 bits[29 - 31] = part(If extractor database is for multi wide modes).
     */
    uint32 ext_id;
    /* Input section from which this extractor will extract fields. */
    bcmi_keygen_ext_section_t in_sec;
    /* Output section to which this extractor will fed fields. */
    bcmi_keygen_ext_section_t out_sec;
   /* Offset of the extracted field in the output section */
   uint16 offset;
    /* Extractor attributes used to validate the extractor configuration
     * for the given keygen mode and qualifiers that needs to be extracted.
     */
   bcmi_keygen_ext_attrs_t ext_attrs;

   /* Used internally by the keygen algorithm. */
   void *finfo;
} bcmi_keygen_ext_cfg_t;

/* Extractor section Configuration */
typedef struct bcmi_keygen_ext_section_cfg_s {
    /* Section Flags. Currently there is one flag created
     * BCMI_KEYGEN_EXT_SECTION_PASS_THRU.
     */
    uint32 flags;
    /* Section identifier. */
    bcmi_keygen_ext_section_t sec;
    /* Section size in bits */
    /* Parent section identifier */
    bcmi_keygen_ext_section_t parent_sec;
    int drain_bits;
    /* This field is used in runtime to avoid pushing more bits
     * than drain_bits.
     */
    int fill_bits;
    /* Generally it is set to 1. If section has any extractor
     * which can operate in multiple granularities, then this can
     * be more than 1.
     */
    uint16 ext_cfg_combinations;
} bcmi_keygen_ext_section_cfg_t;

/* Extractor database for a particular keygen mode */
typedef struct bcmi_keygen_ext_cfg_db_s {
    /* Total number of extractor levels */
    uint8 num_ext_levels;
     /* keygen mode for which this extractor database is used.  */
    bcmi_keygen_mode_t mode;
     /* Extractors configuration in each extractor hierarchy.   */
    bcmi_keygen_ext_cfg_t *ext_cfg[BCMI_KEYGEN_EXT_LEVEL_COUNT];
     /* Number of extractor configurations in each extractor hierarchy.  */
    uint16 conf_size[BCMI_KEYGEN_EXT_LEVEL_COUNT];
    /* Extractor sections configuration */
    bcmi_keygen_ext_section_cfg_t *sec_cfg[BCMI_KEYGEN_EXT_SECTION_COUNT];
    /* Number of valid section configurations in this database.  */
    uint16 num_sec;
} bcmi_keygen_ext_cfg_db_t;

/*
 * Extractor ID construction macro.
 */
#define BCMI_KEYGEN_EXT_ID_CREATE(_p_, _l_, _g_, _e_num_, _e_id_)              \
    (_e_id_) = (((_p_) << 28) |  ((_l_) << 18) | ((_g_) << 10)  |  (_e_num_))

/*
 * Parse an Extractor ID and get the extractor level, granularity and
 * extractor number information.
 */
#define BCMI_KEYGEN_EXT_ID_PARSE(_e_id_, _p_, _l_, _g_, _e_num_)      \
                                (_e_num_) = ((_e_id_) & 0x3ff);       \
                                (_g_) = (((_e_id_) >> 10) & 0xff);    \
                                (_l_) = (((_e_id_) >> 18) & 0xf);     \
                                (_p_) = (((_e_id_) >> 28) & 0x3)

#define BCMI_KEYGEN_EXT_ID_PART_GET(_ext_id_)  (((_ext_id_) >> 28) & 0x3)
#define BCMI_KEYGEN_EXT_ID_GRAN_GET(_ext_id_)  (((_ext_id_) >> 10) & 0xff)
#define BCMI_KEYGEN_EXT_ID_LEVEL_GET(_ext_id_) (((_ext_id_) >> 18) & 0xf)
#define BCMI_KEYGEN_EXT_ID_NUM_GET(_ext_id_)   ((_ext_id_) & 0x3ff)

#define BCMI_KEYGEN_EXT_ID_GRAN_SET(_ext_id_, gran)   \
                  (_ext_id_) &= ~(0xFF << 10);        \
                  (_ext_id_) |= ((gran & 0xff) << 10)

#define BCMI_KEYGEN_EXT_ATTR_IDX_TO_GRAN(attr_idx, gran)              \
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
                       BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);   \
               }

#define BCMI_KEYGEN_EXT_GRAN_TO_ATTR_IDX(gran, attr_idx)              \
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
                       BCMI_KEYGEN_RETURN_VAL_EXIT(BCM_E_INTERNAL);   \
               }

/* BCMI_KEYGEN_EXT_CFG_ADD macro needs the following declaration
 * in any function that uses it.
 */
#define BCMI_KEYGEN_EXT_CFG_DECL            \
    int _rv_;                               \
    int _ext_id_ = 0;                       \
    bcmi_keygen_ext_cfg_t _ext_cfg_

/*
 * Extractor hierarcy construction utility macro.
 */
#define BCMI_KEYGEN_EXT_CFG_ADD(_unit_, _ext_cfg_db_,                          \
                                _p_, _l_, _g_, _e_num_,                        \
                                _in_sec_, _out_sec_,                           \
                                _offset_, _ext_attrs_)                         \
            do {                                                               \
                _rv_ = bcmi_keygen_ext_cfg_t_init(unit, &_ext_cfg_);           \
                if (BCMI_KEYGEN_FAILURE(_rv_)) {                               \
                    BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                         \
                }                                                              \
                BCMI_KEYGEN_EXT_ID_CREATE(_p_, _l_, _g_, _e_num_, _ext_id_);   \
                (_ext_cfg_).ext_id = (_ext_id_);                               \
                (_ext_cfg_).in_sec = (_in_sec_);                               \
                (_ext_cfg_).out_sec = (_out_sec_);                             \
                (_ext_cfg_).in_use = 0;                                        \
                (_ext_cfg_).offset = (_offset_);                               \
                sal_memcpy(&((_ext_cfg_).ext_attrs),                           \
                           (_ext_attrs_),                                      \
                           sizeof(bcmi_keygen_ext_attrs_t));                   \
                _rv_ = bcmi_keygen_ext_cfg_insert((_unit_), (_l_),             \
                                         &(_ext_cfg_), (_ext_cfg_db_));        \
                if (BCMI_KEYGEN_FAILURE(_rv_)) {                               \
                    BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                         \
                }                                                              \
            } while(0)

/*
 * By default, sum of granularity of all extractors going out of the section
 * is set to drain bits of a section. In some special scenarios (when extractor
 * with programmable granularity is present in sections outgoing extractors),
 * drain bits of section may be less than the sum calculated. In such cases,
 * section drain bits needs to be overwritten with correct value after
 * (strictly) initializing all the extractors using BCMI_KEYGEN_EXT_CFG_ADD.
 */
#define BCMI_KEYGEN_EXT_SECTION_DBITS_SET(_unit_, _ext_cfg_db_,                \
                                          _sec_, _drain_bits_)                 \
            do {                                                               \
                _rv_ = bcmi_keygen_ext_sec_dbit_set((_unit_), (_sec_),         \
                                         (_drain_bits_), (_ext_cfg_db_));      \
                if (BCMI_KEYGEN_FAILURE(_rv_)) {                               \
                    BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                         \
                }                                                              \
            } while(0)

/*
 * By default, each section have parents section set to its own section.
 * So each section and its parent section will have same drain bits.
 * But in some cases, section will have its own drain bits and group of
 * sections together should have drain bits less than the sum of all the
 * sections drain bits. In order to manage such hierarchical levels of drain
 * bits, a common parent section can be assigned to a group of sections.
 */
#define BCMI_KEYGEN_EXT_SECTION_PARENT_SET(_unit_, _ext_cfg_db_,               \
                                           _sec_, _parent_sec_)                \
            do {                                                               \
                _rv_ = bcmi_keygen_ext_sec_parent_set((_unit_), (_sec_),       \
                                        (_parent_sec_), (_ext_cfg_db_));       \
                if (BCMI_KEYGEN_FAILURE(_rv_)) {                               \
                    BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                         \
                }                                                              \
            } while(0)

/* Max number of chunks a qualifer can have in any of
 * its configurations of type bcmi_keygen_qual_cfg_t.
 */
#define BCMI_KEYGEN_QUAL_MAX_CHUNKS 32

/* Maximum offsets any qualifier can have in final key */
#define BCMI_KEYGEN_QUAL_OFFSETS_MAX 64

/* */
#define BCMI_KEYGEN_QUAL_BITMAP_MAX_WORDS 8

typedef enum bcmi_keygen_qual_flags_e {
    /* Qualifier is configuration is post muxed. */
    BCMI_KEYGEN_QUAL_FLAGS_PMUX = 0,
    /* Qualifier configuration is not valid if
     * keygen is in mode "SINGLE".
     */
    BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE,
    /* Qualifier configuration is not valid if
     * keygen is in mode "DBLINTRA".
     */
    BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_DBLINTRA,
    /* Qualifier configuration is not valid for
     * Exact Match.
     */
    BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM,
    /* Qualifier configuration is not valid for
     * IFP.
     */
    BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_IFP,
    /* Always Last. Not a usable value. */
    BCMI_KEYGEN_QUAL_FLAGS_COUNT
} bcmi_keygen_qual_flags_t;

/* Qualifier flags set used in key generation algorithm to
 * validate the qualifier configuration.
 */
typedef struct bcmi_keygen_qual_flags_bmp_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCMI_KEYGEN_QUAL_FLAGS_COUNT)];
} bcmi_keygen_qual_flags_bmp_t;

/* Qualifier chunk information. */
typedef struct bcmi_keygen_qual_offset_cfg_s {
    /* section in input bus from which this chunk is fed to extraction. */
    bcmi_keygen_ext_section_t section;
    /* chunk number. For example , if a section has 10 chunks,
     * then this field will be set to any one value from 0 to 9 which
     * is appropriate to this chunk.
     */
    uint16 sec_val;
    /* Required number bits in this chunk for the qualifier this chunk
     * information belongs to. This will be any value between 1 to ?size?.
     */
    uint16 width;
    /* start bit(w.r.t total width of the section) from where ?width?
     * number of bits in this chunk are required for the qualifier.
     */
    uint16 offset;
    /* Size of the chunk. Generally all chunks in a section will
     * have the same size.
     */
    uint16 size;
} bcmi_keygen_qual_offset_cfg_t;

/* Qualifier configuration */
typedef struct bcmi_keygen_qual_cfg_s {
    /* keygen algorithm uses these flags to validate this configuration
     * for the given keygen mode(bcmi_keygen_mode_t).
     */
    bcmi_keygen_qual_flags_bmp_t qual_flags;
    /* Qualifier control to be reserved in case keygen algorithm is using
     * this qualifier configuration.
     */
    bcmi_keygen_ext_ctrl_sel_t pri_ctrl_sel;
    /* "ctrl_sel" should be set to this value in case this configuration is
     * successfully used by keygen algorithm.
     */
    uint16 pri_ctrl_sel_val;
    /* Secondary control to be reserved for given primary ctrl
     * Not applicable in keygen alogrithm.
     */
    bcmi_keygen_ext_ctrl_sel_t sec_ctrl_sel;
    /* "sec_ctrl_sel" should be set to this value. */
    uint16 sec_ctrl_sel_val;
    /* Qualifer chunk information for different chunks belongs to this
     *  qualifier configuration. Chunks may either come from same section
     *  or from different sections in the input bus.
     */
    bcmi_keygen_qual_offset_cfg_t e_params[BCMI_KEYGEN_QUAL_MAX_CHUNKS];
    /* number of valid elements in the e_params array. Valid values of this
     *  field ranges between 1 to BCMI_KEYGEN_QUAL_MAX_CHUNKS.
     */
    uint8 num_chunks;
    /* Size of all valid chunks together. */
    uint32 size;

    uint8 update_count;
} bcmi_keygen_qual_cfg_t;

/* All possible qualifiers configurations of a single qualifier. */
typedef struct bcmi_keygen_qual_cfg_info_s {
    /* Qualifier Identifier.  Keygen library uses qualifier identifiers
     *  created in field module.
     */
    bcm_field_qualify_t qual;
    /* Array of qualifier configurations of a qualifier. Most of the
     * qualifiers have single configurations but some have more
     * than one configuration to choose.
     */
    bcmi_keygen_qual_cfg_t *qual_cfg_arr;
     /* "qual_cfg_arr" size */
     uint8 num_qual_cfg;
} bcmi_keygen_qual_cfg_info_t;

/* Qualifier configuration information of all qualifiers supported
 * on the device.
 */
typedef struct bcmi_keygen_qual_cfg_info_db_s {
    /* Number of valid qualifiers in the database. This value is anywhere
     * in between 1 to BCM_FIELD_QUALIFY_MAX.
     */
    int num_quals;
    /* Holds pointers to Qualifier Configuration information for
     * each supported qualifier. If some qualifier is not supported
     * on the device then it?s pointer is set to NULL.
     */
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info[BCM_FIELD_QUALIFY_MAX];
} bcmi_keygen_qual_cfg_info_db_t;

/* BCMI_KEYGEN_QUAL_CFG_XXX macros require the following declaration in any
 * function which uses them.
 */
#define BCMI_KEYGEN_QUAL_CFG_DECL          \
    int _rv_;                              \
    bcmi_keygen_qual_cfg_t _qual_cfg_

/* To re-init the bcmi_keygen_qual_cfg_t structure. */
#define BCMI_KEYGEN_QUAL_CFG_INIT(_unit_)                       \
    _rv_ = bcmi_keygen_qual_cfg_t_init(_unit_, &(_qual_cfg_));  \
    if (BCMI_KEYGEN_FAILURE(_rv_)) {                            \
        BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                      \
    }

/* To update the selectors, offset and width of the
 * bcmi_keygen_qual_cfg_t variable based on the index.
 */
#define BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(_index_,                           \
                                        _section_,                         \
                                        _sec_val_,                         \
                                        _offset_,                          \
                                        _width_)                           \
                (_qual_cfg_).e_params[_index_].section      = (_section_); \
                (_qual_cfg_).e_params[_index_].sec_val      = (_sec_val_); \
                (_qual_cfg_).e_params[_index_].offset       = (_offset_);  \
                (_qual_cfg_).e_params[_index_].width        = (_width_);   \
                (_qual_cfg_).num_chunks++;

/* To update the control selectors and its value in the
 * bcmi_keygen_qual_cfg_t structure.
 */
#define BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(_ctrl_sel_, _ctrl_sel_val_)   \
                    (_qual_cfg_).pri_ctrl_sel     = (_ctrl_sel_);           \
                    (_qual_cfg_).pri_ctrl_sel_val = (_ctrl_sel_val_);       \

/* To update the secondary control selectors and its value. */
#define BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(_ctrl_sel_, _ctrl_sel_val_)  \
                (_qual_cfg_).sec_ctrl_sel     = (_ctrl_sel_);              \
                (_qual_cfg_).sec_ctrl_sel_val = (_ctrl_sel_val_);          \

/* To update the control selectors and its value in the
 * bcmi_keygen_qual_cfg_t structure.
 */
#define BCMI_KEYGEN_QUAL_CFG_INSERT(_unit_, _qual_cfg_info_db_,        \
                                    _qual_id_, _qual_flags_)           \
                sal_memcpy(&((_qual_cfg_).qual_flags),                 \
                           &(_qual_flags_),                            \
                           sizeof(bcmi_keygen_qual_flags_bmp_t));      \
                _rv_ = bcmi_keygen_qual_cfg_insert((_unit_),           \
                                             (_qual_id_),              \
                                             &(_qual_cfg_),            \
                                             (_qual_cfg_info_db_));    \
                if (BCMI_KEYGEN_FAILURE(_rv_)) {                       \
                    BCMI_KEYGEN_RETURN_VAL_EXIT(_rv_);                 \
                }

/* Qualifier flag to use new finfo while converting this qualifier
 * bitmap to finfo. Default behavior of algorithm is to search in
 * assigned finfo for given section/sec_val/pri_ctrl_sel/
 * pri_ctrl_sel_val. If a match is found, that finfo is updated with
 * bitmap of this new qualifer. This flag override this default behavior
 * and valid only for flowtracker style extractors. Using this flag
 * in ifp style extractors will give unexpected results.
 */
#define BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW                    (1 << 0)

/* Qualifier flags to use extractor of granularity 16 at level 2
 * to extract this qualifier bitmap. Default behavior is to loop
 * through all extractors at given level and try to extract all
 * required bit in given finfo using extractors of any granularity
 * available at given section in given level. This flags override
 * this behavior and valid only for flowtracker style extractors.
 * To use this flag,
 * a. Qualifier size must be equal or less than 16 bits.
 *
 * Qual flags BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW can also be
 * combined with this flag. Using this flag in ifp style extractors
 * will give unexpected results.
 */
#define BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16                (1 << 1)

/* Qualifier flags to use extractor of granularity 8 at level 2
 * to extract this qualifier bitmap. Default behavior is to loop
 * through all extractors at given level and try to extract all
 * required bit in given finfo using extractors of any granularity
 * available at given section in given level. This flags override
 * this behavior and valid only for flowtracker style extractors.
 * To use this flag,
 * a. Qualifier size must be less than 8 bits.
 * b. Either or both of qual flag BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW
 *    and BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16  can be used.
 * c. DO NOT use only this flag alone. It might fail to allocate
 *    extractors even if extraction is possible.
 *
 * Using this flag in ifp style extractors will give unexpected
 * results.
 */
#define BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8                 (1 << 2)

/* Qualifier has to be extrctaed to modulo 32 offsets in the final
 * key.In general keygen algorithm doesnot give any preference to
 * any qualifier while placing it in the final key. But this flag
 * ovverrides this behaviour.
 */
#define BCMI_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET     (1 << 3)

/* This flag will make sure that all input bus containers related to
 * a qualifier are extrcted by consecutive muxers of same part of the
 * muxer hierarchy.
 */
#define BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS               (1 << 4)

/* Qualifier information */
typedef struct bcmi_keygen_qual_info_s {
    /* Qualifier flags */
    uint32 flags;
    /* Need to extract all bits in the qualifier or some? */
    uint8 partial;
    /* Qualifier Id. */
    bcm_field_qualify_t qual_id;
    /* bits required if partial is set to TRUE. otherwise,
     * ignore this field.
     */
    uint32 bitmap[BCMI_KEYGEN_QUAL_BITMAP_MAX_WORDS];
} bcmi_keygen_qual_info_t;

/* Offset and width information of a qualifer in final key of a given part */
typedef struct bcmi_keygen_qual_offset_s {
    /* Number of Offsets. */
    uint8 num_offsets;
    /* offset. */
    uint16 offset[BCMI_KEYGEN_QUAL_OFFSETS_MAX];
    /* width from the corresponding offset. */
    uint8 width[BCMI_KEYGEN_QUAL_OFFSETS_MAX];
} bcmi_keygen_qual_offset_t;

/* All qualifiers offset and width information in final key of a given part */
typedef struct bcmi_keygen_qual_offset_info_s {
    /* Array qualifiers present in the part given */
    bcm_field_qualify_t *qid_arr;
    /* offset and width information corresponding to qualifiers in qid_arr. */
    bcmi_keygen_qual_offset_t *offset_arr;
    /* Size of the qid_arr and offset_arr arrays */
    uint16 size;
} bcmi_keygen_qual_offset_info_t;

/* Validate whether extractors can be allocated for all
 * qualifiers in the given keygen mode. Dont need to generate
 * the offsets of qualifiers in the final key.
 */
#define BCMI_KEYGEN_CFG_FLAGS_VALIDATE_ONLY (1 << 0)
/* Keygen algorithm has generated operation information for the given
 * keygen configuration but applicatipon wants to add more configuration
 * (like adding more qualifiers) to it and keygen algorithm has regenerate
 * the operational information for the newly added configuration. During
 * this process keygen algorithm should not use resources(like multiplexers)
 * that are in use for old keygen configuration.
 */
#define BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE   (1 << 1)

/* When num_parts in keygen configuration is more than one, then input bus
 * containers corresponding to all qualifiers are distributed to all parts
 * symmetrically. That means if two 16 bit container needs to be extracted
 * from two parts, first container is extracted from part one and second one
 * is extracted from part2 instead of extracting both containers from first
 * part. If this flag is set, then all muxers are used in part one first and
 * then go to 2nd part, 3rd part and so on.
 */
#define BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC (1 << 2)

/* Keygen algorithm builds list of containers corresponding to all
 * qualifiers and sort the list in the descending order based on the
 * container size. This flag will voids the sorting of the containers.
 * This is the requirement for FlowTracker groups in some cases.
 */
#define BCMI_KEYGEN_CFG_FLAGS_SERIAL_QUAL_CONTAINERS (1 << 3)

/* Keygen Configuration to be used to allocate multiplexers */
typedef struct bcmi_keygen_cfg_s {
    /* Flags(BCMI_KEYGEN_CFG_FLAGS_XXX). */
    uint32 flags;
    /* keygen mode */
    bcmi_keygen_mode_t mode;
    /* Information about the qualifiers in the final key */
    bcmi_keygen_qual_info_t *qual_info_arr;
    /* size of qual_info_arr array */
    uint8 qual_info_count;
    /* Number of parts. Generally it depends on keygen mode.
     * 1 in case of keygen mode SINGLE, 2 in case of DBLINTRA,
     * 3 in case of TRIPLE. But there are other modes as well for
     * which num parts cannot be assumed by keygen library so is
     * the reason for creation of this field.
     */
    uint8 num_parts;
    /* Qualifiers configuration information database */
    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db;
    /* Extractor configuration database. Applications may have multiple
     * extractor databases but need to pass the right pointer for keygen mode.
     */
    bcmi_keygen_ext_cfg_db_t *ext_cfg_db;
    /* Different controls generated by the keygen algorithm. This is required
     * if application need to update the existing QSET.
     */
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info;
    /* Size of ext_ctrl_sel_info array */
    uint16 ext_ctrl_sel_info_count;
} bcmi_keygen_cfg_t;

/* Operational data generated by keygen algorithm */
typedef struct bcmi_keygen_oper_s {
    /* Final key offset information of each qualifier specified
     * in keygen_cfg.
     */
    bcmi_keygen_qual_offset_info_t *qual_offset_info;
    /* Different controls generated by the keygen algorithm.
     * Calling module should push this information to h/w.
     */
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info;
    /* Size of ext_ctrl_sel_info array */
    uint16 ext_ctrl_sel_info_count;
} bcmi_keygen_oper_t;

extern int
bcmi_keygen_cfg_process(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_oper_t *keygen_oper);
extern int
bcmi_keygen_qual_cfg_t_init(int unit,
                            bcmi_keygen_qual_cfg_t *qual_cfg);
extern int
bcmi_keygen_qual_cfg_insert(int unit,
                 bcm_field_qualify_t qual_id,
                 bcmi_keygen_qual_cfg_t *new_qual_cfg,
                 bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db);
extern int
bcmi_keygen_ext_cfg_insert(int unit,
                           uint8 level,
                           bcmi_keygen_ext_cfg_t *ext_cfg,
                           bcmi_keygen_ext_cfg_db_t *ext_cfg_db);
extern int
bcmi_keygen_ext_cfg_t_init(int unit,
                           bcmi_keygen_ext_cfg_t *ext_cfg);

extern int
bcmi_keygen_ext_cfg_db_free(int unit, bcmi_keygen_ext_cfg_db_t *ext_cfg_db);

extern int
bcmi_keygen_oper_free(int unit,
                      uint16 num_parts,
                      bcmi_keygen_oper_t *keygen_oper);
extern int
bcmi_keygen_keysize_get(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        uint16 *rkey_size);

/*
 * Overwrite drain bits for given section.
 */
extern int
bcmi_keygen_ext_sec_dbit_set(int unit,
                bcmi_keygen_ext_section_t sec,
                int drain_bits,
                bcmi_keygen_ext_cfg_db_t *ext_cfg_db);

/*
 * Overwrite parent section for given section.
 */
extern int
bcmi_keygen_ext_sec_parent_set(int unit,
                bcmi_keygen_ext_section_t sec,
                bcmi_keygen_ext_section_t parent_sec,
                bcmi_keygen_ext_cfg_db_t *ext_cfg_db);

/*
 * Free Extractor config database.
 */
extern int
bcmi_keygen_ext_cfg_db_cleanup(int unit,
                               bcmi_keygen_ext_cfg_db_t *ext_cfg_db);

/*
 * Free Qualifier config database.
 */
extern int
bcmi_keygen_qual_cfg_info_db_free(int unit,
                  bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db);

/*
 * Max size of qualifier
 */
extern int
bcmi_keygen_qual_cfg_max_size_get(int unit,
                bcmi_keygen_cfg_t *keygen_cfg,
                bcm_field_qualify_t qual_id,
                uint16 *qual_cfg_max_size);

#endif

