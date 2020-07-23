/*! \file bcmgene_feature.h
 *
 * Broadcom Generic Extensible NPL Encapsulation (GENE) feature header file.
 * This file contains feature and sub-feature definitions internal to the BCM GENE library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMGENE_LTSW_GENE_FEATURE_H
#define BCMGENE_LTSW_GENE_FEATURE_H

/*!
 * \brief FlexDigest functionalities.
 */
/*! The FD normalization functionality. */
#define BCMGENE_FLEXDIGEST_F_NORM 1

/*! The FD lookup functionality. */
#define BCMGENE_FLEXDIGEST_F_LKUP 2

/*! The FD hashing functionality. */
#define BCMGENE_FLEXDIGEST_F_HASH 3

/*! The FD global control functionality. */
#define BCMGENE_FLEXDIGEST_F_GLOBAL 4


#define BCMGENE_FLEXDIGEST_F_STRINGS \
{ \
    "Norm", \
    "Lkup", \
    "Hash," \
    "Global" \
}

/*!
 * \brief FlexDigest normalization sub-features.
 */
/*! The seed profile sub-feature for FD normalization. */
#define BCMGENE_FLEXDIGEST_SUB_F_NORM_SEED_PROFILE (1 << 0)

/*! The profile sub-feature for FD normalization. */
#define BCMGENE_FLEXDIGEST_SUB_F_NORM_PROFILE (1 << 1)

#define BCMGENE_FLEXDIGEST_SUB_F_NORM_STRINGS \
{ \
    "NormSeedProfile", \
    "NormProfile" \
}

/*!
 * \brief FlexDigest lookup sub-features.
 */
/*! The group sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_GROUP (1 << 0)

/*! The pre-selection sub-feature of match identifier for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_PRESEL (1 << 1)

/*! The entry sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_ENTRY (1 << 2)

/*! The opaque key sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_OPAQUE_KEY (1 << 3)

/*! The match identifier sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_MATCH_ID (1 << 4)

/*! The policy field sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_POLICY (1 << 5)

/*! The mask profile sub-feature for FD lookup. */
#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_MASK_PROFILE (1 << 6)

#define BCMGENE_FLEXDIGEST_SUB_F_LKUP_STRINGS \
{ \
    "LkupGroup", \
    "LkupPresel", \
    "LkupEntry", \
    "LkupOpaqueKey", \
    "LkupMatchId", \
    "LkupPolicy", \
    "LkupMaskProfile" \
}

/*!
 * \brief FlexDigest hashing sub-features.
 */
/*! The profile sub-feature for FD hashing. */
#define BCMGENE_FLEXDIGEST_SUB_F_HASH_PROFILE (1 << 0)

/*! The pre-processing sub-feature for FD hashing. */
#define BCMGENE_FLEXDIGEST_SUB_F_HASH_PRE_PROCESSING (1 << 1)

/*! The salt sub-feature for FD hashing. */
#define BCMGENE_FLEXDIGEST_SUB_F_HASH_SALT (1 << 2)

/*! The function sub-feature for FD hashing. */
#define BCMGENE_FLEXDIGEST_SUB_F_HASH_FUNCTION (1 << 3)

#define BCMGENE_FLEXDIGEST_SUB_F_HASH_STRINGS \
{ \
    "HashProfile", \
    "HashPreProcessing", \
    "HashSalt", \
    "HashFunction" \
}

/*!
 * \brief FlexDigest global control sub-features.
 */
/*! The versatile hash seed control sub-feature for FD global control . */
#define BCMGENE_FLEXDIGEST_SUB_F_GLOBAL_VERSATILE_HASH_SEED (1 << 0)

#define BCMGENE_FLEXDIGEST_SUB_F_GLOBAL_STRINGS \
{ \
    "GlobalVersatileHashSeed" \
}

/*!
 * \brief Hash Output Selection functionalities.
 */
/*! The hash output selection profile functionality. */
#define BCMGENE_HASH_OUTPUT_SELECTION_F_PROFILE 1

/*! The hash output selection instance functionality. */
#define BCMGENE_HASH_OUTPUT_SELECTION_F_INSTANCE 2

#define BCMGENE_HASH_OUTPUT_SELECTION_F_STRINGS \
{ \
    "Profile", \
    "Instance" \
}

/*!
 * \brief Hash Output Selection sub-features.
 */
/*! The sub-feature for trunk unicast packet resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_UC (1 << 0)

/*! The sub-feature for trunk non unicast packet resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_NONUC (1 << 1)

/*! The sub-feature for trunk failover resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_FAILOVER (1 << 2)

/*! The sub-feature for trunk system resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM (1 << 3)

/*! The sub-feature for trunk system failver resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM_FAILOVER (1 << 4)

/*! The sub-feature for VPLAG resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_VP (1 << 5)

/*! The sub-feature for trunk system failver resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_OVERLAY (1 << 6)

/*! The sub-feature for VPLAG resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_UNDERLAY (1 << 7)

/*! The sub-feature for dlb trunk resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_TRUNK (1 << 8)

/*! The sub-feature for dlb trunk resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_ECMP (1 << 9)

/*! The sub-feature for entropy hash resolution. */
#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ENTROPY (1 << 10)

#define BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_STRINGS \
{ \
    "TrunkUc", \
    "TrunkNonuc", \
    "TrunkFailover", \
    "TrunkSystem", \
    "TrunkSystemFailover", \
    "TrunkVp", \
    "EcmpOverlay", \
    "EcmpUnderlay", \
    "DlbTrunk", \
    "DlbEcmp", \
    "Entropy" \
}

/*!
 * \brief L3 AACL functionalities.
 */
/*! The L3 AACL for IPv4 source address and src L4 port. */
#define BCMGENE_L3_AACL_F_COMPRESSION 1

/*! The L3 AACL validation. */
#define BCMGENE_L3_AACL_F_COMPRESSION_VALIDATE 2

#define BCMGENE_L3_AACL_F_STRINGS \
{ \
    "Compression", \
    "Compression_validate" \
}

/*!
 * \brief L3 AACL sub-features.
 */
/*! The sub-feature for L3 AACL IPv4 source address and src L4 port. */
#define BCMGENE_L3_AACL_SUB_F_IPv4_SRC (1 << 0)

/*! The sub-feature for L3 AACL IPv4 destination address and dst L4 port. */
#define BCMGENE_L3_AACL_SUB_F_IPv4_DST (1 << 1)

/*! The sub-feature for L3 AACL IPv6 source address and src L4 port. */
#define BCMGENE_L3_AACL_SUB_F_IPv6_SRC (1 << 2)

/*! The sub-feature for L3 AACL IPv6 destination address and dst L4 port. */
#define BCMGENE_L3_AACL_SUB_F_IPv6_DST (1 << 3)

#define BCMGENE_L3_AACL_SUB_F_STRINGS \
{ \
    "IPv4Src", \
    "IPv4Dst", \
    "IPv6Src", \
    "IPv6Dst" \
}

#endif /* BCMGENE_LTSW_GENE_FEATURE_H */
