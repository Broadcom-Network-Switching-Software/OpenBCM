/*! \file bcmfp_ext_internal.h
 *
 * Defines, Enums and Structures to represent FP extractors
 * related information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_EXT_INTERNAL_H
#define BCMFP_EXT_INTERNAL_H

/*
 * Typedef:
 *    BCMFP_EXT_ATTR_XXX
 * Purpose:
 *    Field extractor sections attribute flags.
 */
#define BCMFP_EXT_ATTR_PASS_THRU                           (1 << 0)

#define BCMFP_EXT_ATTR_NOT_IN_EIGHTY_BIT                   (1 << 1)

#define BCMFP_EXT_ATTR_NOT_WITH_IPBM                       (1 << 2)

#define BCMFP_EXT_ATTR_DUPLICATE                           (1 << 3)

#define BCMFP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID           (1 << 4)

#define BCMFP_EXT_ATTR_NOT_WITH_NAT_NEEDED                 (1 << 5)

#define BCMFP_EXT_ATTR_NOT_WITH_DROP                       (1 << 6)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0             (1 << 7)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1             (1 << 8)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_80          (1 << 9)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_80          (1 << 10)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_MODE_128                (1 << 11)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B           (1 << 12)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B           (1 << 13)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C           (1 << 14)

#define BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C           (1 << 15)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0     (1 << 16)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1     (1 << 17)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASS_ID_LOOKUP_0 (1 << 18)

#define BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASS_ID_LOOKUP_1 (1 << 19)



/* Egress FP possible keys. */
#define EFP_KEY1          (0x1)  /* IPv4 key.      */
#define EFP_KEY2          (0x2)  /* IPv6 key.      */
#define EFP_KEY3          (0x3)  /* IPv6 dw key.   */
#define EFP_KEY4          (0x4)  /* L2 key.        */
#define EFP_KEY5          (0x5)  /* HiGiG Key.     */
#define EFP_KEY6          (0x6)  /* HiGiG Key.     */
#define EFP_KEY7          (0x7)  /* Loopback Key.  */
#define EFP_KEY8          (0x8)  /* Bytes after
                                    L2 Header key. */

/* Egress FP slice v6 key  modes. */
#define  EGR_MODE_SIP6         (0x0)
#define  EGR_MODE_DIP6         (0x1)
#define  EGR_MODE_SIP_DIP_64   (0x2)

/*
 * Typedef:
 *     bcmfp_ingress_ctrl_sel_t
 * Purpose:
 *     Pre-Mux extractor input ext controls.
 */
typedef enum bcmfp_ext_ctrl_sel_e {
    BCMFP_EXT_CTRL_SEL_DISABLE            = 0, /* Selector disabled.                */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_A          = 1, /* Auxiliary Tag A selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_B          = 2, /* Auxiliary Tag B selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_C          = 3, /* Auxiliary Tag C selector.         */
    BCMFP_EXT_CTRL_SEL_AUX_TAG_D          = 4, /* Auxiliary Tag D selector.         */
    BCMFP_EXT_CTRL_SEL_TCP_FN             = 5, /* TCP function selector.            */
    BCMFP_EXT_CTRL_SEL_TOS_FN             = 6, /* ToS function selector.            */
    BCMFP_EXT_CTRL_SEL_TTL_FN             = 7, /* TTL function selector.            */
    BCMFP_EXT_CTRL_SEL_CLASS_ID_A          = 8, /* Class ID container A selector.    */
    BCMFP_EXT_CTRL_SEL_CLASS_ID_B          = 9, /* Class ID container B selector.    */
    BCMFP_EXT_CTRL_SEL_CLASS_ID_C          = 10, /* Class ID container C selector.   */
    BCMFP_EXT_CTRL_SEL_CLASS_ID_D          = 11, /* Class ID container D selector.   */
    BCMFP_EXT_CTRL_SEL_SRC_A              = 12, /* Source Container A selector.     */
    BCMFP_EXT_CTRL_SEL_SRC_B              = 13, /* Source Container B selector.     */
    BCMFP_EXT_CTRL_SEL_SRC_DST_0          = 14, /* Src/Dst Container 0 selector.    */
    BCMFP_EXT_CTRL_SEL_SRC_DST_1          = 15, /* Src/Dst Container 1 selector.    */
    BCMFP_EXT_CTRL_SEL_IPBM_SOURCE        = 16, /* IPBM Source                       */
    BCMFP_EXT_CTRL_SEL_SRC                = 17, /* Source Port type selector.       */
    BCMFP_EXT_CTRL_SEL_IP_HEADER          = 18, /* IP header type(Inner/Outer) selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY1_CLASS_ID   = 19, /* EFP KEY1 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY2_CLASS_ID   = 20, /* EFP KEY2 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY3_CLASS_ID   = 21, /* EFP KEY3 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_CLASS_ID   = 22, /* EFP KEY4 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY6_CLASS_ID   = 23, /* EFP KEY6 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY7_CLASS_ID   = 24, /* EFP KEY7 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY8_CLASS_ID   = 25, /* EFP KEY8 Class Id Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_DVP       = 26, /* EFP KEY4 DVP Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY8_DVP       = 27, /* EFP KEY8 DVP Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_KEY4_MDL       = 28, /* EFP KEY4 MDL Selector. */
    BCMFP_EXT_CTRL_SEL_EGR_IP_ADDR        = 29, /* EFP IP6 Address(Src/Dst) Selector. */
    BCMFP_EXT_CTRL_SEL_COUNT              = 30  /* Always Last. Not a usable value. */
} bcmfp_ext_ctrl_sel_t;


#define BCMFP_EXT_CTRL_SEL_DONT_CARE (-1)


/*
 * Typedef:
 *     bcmfp_ingress_ext_section_t
 * Purpose:
 *     Key Generation extractor selector encodings.
 */
typedef enum bcmfp_ext_section_e {
    BCMFP_EXT_SECTION_DISABLE   = 0,  /* Selector disabled.               */
    BCMFP_EXT_SECTION_L1E32     = 1,  /* Level1 32bit extractor.          */
    BCMFP_EXT_SECTION_L1E16     = 2,  /* Level1 16bit extractor.          */
    BCMFP_EXT_SECTION_L1E8      = 3,  /* Level1 8bit extractor.           */
    BCMFP_EXT_SECTION_L1E4      = 4,  /* Level1 4bit extractor.           */
    BCMFP_EXT_SECTION_L1E2      = 5,  /* Level1 2bit extractor.           */
    BCMFP_EXT_SECTION_L2E16     = 6,  /* Level2 16bit extractor.          */
    BCMFP_EXT_SECTION_L2E4      = 7,  /* 104 Passthru bits.               */
    BCMFP_EXT_SECTION_L3E16     = 8,  /* Level3 16bit extractor.          */
    BCMFP_EXT_SECTION_L3E4      = 9,  /* Level3 4bit extractor.           */
    BCMFP_EXT_SECTION_L3E2      = 10, /* Level3 2bit extractor.           */
    BCMFP_EXT_SECTION_L3E1      = 11, /* Level3 16bit extractor.          */
    BCMFP_EXT_SECTION_L4        = 12, /* Level4 output.                   */
    BCMFP_EXT_SECTION_L4A       = 13, /* Level4 Slice A output.           */
    BCMFP_EXT_SECTION_L4B       = 14, /* Level4 Slice B output.           */
    BCMFP_EXT_SECTION_L4C       = 15, /* Level4 Slice C output.           */
    BCMFP_EXT_SECTION_L2AE16    = 16, /* Level2 Slice A 16bit extractor.  */
    BCMFP_EXT_SECTION_L2BE16    = 17, /* Level2 Slice B 16 bit extractor. */
    BCMFP_EXT_SECTION_L2CE16    = 18, /* Level2 Slice C 16 bit extractor. */
    BCMFP_EXT_SECTION_L2AE4     = 19, /* Level2 Slice A 4bit extractor.   */
    BCMFP_EXT_SECTION_L2BE4     = 20, /* Level2 Slice B 4bit extractor.   */
    BCMFP_EXT_SECTION_L2CE4     = 21, /* Level2 Slice C 4bit extractor.   */
    BCMFP_EXT_SECTION_L3AE16    = 22, /* Level3 Slice A 16bit extractor.  */
    BCMFP_EXT_SECTION_L3BE16    = 23, /* Level3 Slice B 16bit extractor.  */
    BCMFP_EXT_SECTION_L3CE16    = 24, /* Level3 Slice C 16bit extractor.  */
    BCMFP_EXT_SECTION_L3AE4     = 25, /* Level3 Slice A 4bit extractor.   */
    BCMFP_EXT_SECTION_L3BE4     = 26, /* Level3 Slice B 4bit extractor.   */
    BCMFP_EXT_SECTION_L3CE4     = 27, /* Level3 Slice C 4bit extractor.   */
    BCMFP_EXT_SECTION_FPF0      = 28, /* FPF0(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF1      = 29, /* FPF1(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF2      = 30, /* FPF2(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF3      = 31, /* FPF3(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_FPF4      = 32, /* FPF4(Legacy FP selcodes).        */
    BCMFP_EXT_SECTION_COUNT     = 33  /* Always Last. Not a usable value. */
} bcmfp_ext_section_t;

 /*
 * Typedef:
 *     bcmfp_fwd_entity_sel_t
 * Purpose:
 *     Enumerate Forwarding entity Type selection.
 */
typedef enum bcmfp_fwd_entity_sel_e {
    BCMFP_FWD_ENTITY_SVPGPORT       = 0, /* MPLS gport selection.            */
    BCMFP_FWD_ENTITY_GLP            = 1, /* (MOD/PORT/TRUNK) selection.      */
    BCMFP_FWD_ENTITY_MODPORTGPORT   = 2, /* MOD/PORT even if port is part of
                                          the trunk.                         */
    BCMFP_FWD_ENTITY_PORTGROUPNUM   = 4, /* Ingress port group and number    */
    BCMFP_FWD_ENTITY_COUNT          = 5  /* Always Last. Not a usable value. */
} bcmfp_fwd_entity_sel_t;

#define BCMFP_EXT_L1_E32_SELECTORS_MAX  4
#define BCMFP_EXT_L1_E16_SELECTORS_MAX  7
#define BCMFP_EXT_L1_E8_SELECTORS_MAX   7
#define BCMFP_EXT_L1_E4_SELECTORS_MAX   8
#define BCMFP_EXT_L1_E2_SELECTORS_MAX   8
#define BCMFP_EXT_L2_E16_SELECTORS_MAX  10
#define BCMFP_EXT_L2_E4_SELECTORS_MAX   16
#define BCMFP_EXT_L2_E2_SELECTORS_MAX   7
#define BCMFP_EXT_L2_E1_SELECTORS_MAX   2
#define BCMFP_EXT_L3_E4_SELECTORS_MAX   21
#define BCMFP_EXT_L3_E2_SELECTORS_MAX   5
#define BCMFP_EXT_L3_E1_SELECTORS_MAX   2
#define BCMFP_EXT_PMUX_SELECTORS_MAX    15


#define BCMFP_EXT_SELCODE_DONT_CARE    (-1)
#define BCMFP_EXT_SELCODE_DONT_USE     (-2)

extern char *
bcmfp_ext_section_name(bcmfp_ext_section_t section);

#define IS_EXT_OVERLAID_POST_MUXERS(acl_flags, ext_flags) \
          ((SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_NAT_DST_REALM_ID) &&  \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_NAT_NEEDED) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_NAT_NEEDED)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_HIT) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_HIT) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_CLASS_ID) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASS_ID_LOOKUP_0)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_CLASS_ID) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASS_ID_LOOKUP_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_DROP) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_DROP)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_B) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_B) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_C) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_C) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_1) &&  \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1)) || \
           (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_SRC_DST_CONT_0) && \
            (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0)))

#define IS_EXT_OVERLAID_IPBM(acl_flags, ext_flags) \
                   (SHR_BITGET(&acl_flags, BCMFP_PMUX_TYPE_IPBM) && \
                    (ext_flags & BCMFP_EXT_ATTR_NOT_WITH_IPBM))

#endif /* BCMFP_EXT_INTERNAL_H */
