/*! \file bcmfp_idp_internal.h
 *
 * Structures, APIs, enumerations and macros related to
 * FP LT interdependency.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_IDP_INTERNAL_H
#define BCMFP_IDP_INTERNAL_H

#include <bcmfp/bcmfp_types_internal.h>

/*! BCMFP entry id traverse callback function */
typedef int
(*bcmfp_entry_id_traverse_cb)(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_entry_id_t entry_id,
                              uint32_t src_id,
                              void *user_data);

/*! BCMFP entry id traverse callback function */
typedef int
(*bcmfp_entry_id_cond_traverse_cb)(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_entry_id_t entry_id,
                                   uint32_t src_id,
                                   void *user_data,
                                   bool *cond_met);

/*! Map attributes of the given IDP map type. */
typedef struct bcmfp_idp_map_attrs_s {
    /*! Source ID size in 32 bits words. */
    uint8_t src_id_size;
    /*!
     * Lookup is hash based not the index based
     * for getting the entry IDs corresponding
     * to  the given source ID.
     */
    bool hash_lookup;
} bcmfp_idp_map_attrs_t;

/*!
 * Different types of FP IDs to "FP entry ID" or
 * "FP group ID" mappings used in FP LT inter
 * dependency(IDP).
 */
typedef enum bcmfp_idp_map_type_e {
    /*! FP group ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY,

    /*! FP rule ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_RULE_TO_ENTRY,

    /*! FP policy ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_POLICY_TO_ENTRY,

    /*! FP default policy ID to FP group ID mapping. */
    BCMFP_IDP_MAP_TYPE_DPOLICY_TO_GROUP,

    /*!
     * FP presel entry ID to FP group ID
     * mapping.
     */
    BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP,

    /*!
     * FP presel group ID to FP group ID
     * mapping.
     */
    BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP,

    /*! FP pdd ID to FP group ID mapping. */
    BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP,

    /*! FP pdd ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY,

    /*! FP default pdd ID to FP group ID mapping. */
    BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP,

    /*! FP sbr ID to FP group ID mapping. */
    BCMFP_IDP_MAP_TYPE_SBR_TO_GROUP,

    /*! FP sbr ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_SBR_TO_ENTRY,

    /*! FP default sbr ID to FP group ID mapping. */
    BCMFP_IDP_MAP_TYPE_DSBR_TO_GROUP,

    /*! FP meter ID to FP entry ID mapping. */
    BCMFP_IDP_MAP_TYPE_METER_TO_ENTRY,

    /*!
     * FP counter entry ID to FP entry ID
     * mapping.
     */
    BCMFP_IDP_MAP_TYPE_CTR_TO_ENTRY,

    /*!
     * compression ID to FP entry ID mapping.
     */
    BCMFP_IDP_MAP_TYPE_CID_TO_ENTRY,

    /*! Entry ID to its list compressed entry IDs. */
    BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY,
    /*! Invalid map type. */
    BCMFP_IDP_MAP_TYPE_COUNT,
} bcmfp_idp_map_type_t;

/*!
 * \brief Check any entry IDs are mapped to source ID. Source ID
 *  can be any one ID from group_id, rule_id, policy_id, meter_id,
 *  ctr_entry_id, CID.
 *
 * \param [in] unit        Logical device id
 * \param [in] stage_id    stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment HA array space reserved for src_id to
 *                         entry_id mapping.
 * \param [in] map_size    Number of array elements in map_segment.
 * \param [in] map_type    BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id      Source ID in use for entry_id map lookup.
 * \param [in] not_mapped  FALSE -> At least one FP entry is mapped.
 *                         TRUE -> No FP entry is mapped.
 *
 * \retval SHR_E_NONE      Success.
 * \retval SHR_E_PARAM     Wrong parameters.
 * \retval SHR_E_NOT_FOUND Entry ID map for source ID is not found.
 */
extern int
bcmfp_idp_map_check(int unit,
                    bcmfp_stage_id_t stage_id,
                    void *map_segment,
                    uint32_t map_size,
                    bcmfp_idp_map_type_t map_type,
                    uint32_t *src_id,
                    bool *not_mapped);

/*!
 * \brief Traverse through all entry IDs mapped to source ID.
 *  Source ID can be any one ID from group_id, rule_id, policy_id,
 *  meter_id, ctr_entry_id, CID. While traversing call the callback
 *  function provide for each entry Id alonfg with user_data as one
 *  of the parameter.
 *
 * \param [in] unit        Logical device id
 * \param [in] stage_id    stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment HA array space reserved for src_id to
 *                         entry_id mapping.
 * \param [in] map_size    Number of array elements in map_segment.
 * \param [in] map_type    BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id      Source ID in use for entry_id map lookup.
 * \param [in] cb          Callback function to be called for every
 *                         entry ID mapped to source ID.
 * \param [in] user_data   Data, provided by the caller, to be
 *                         passed to callback function as one of the
 *                         parameter.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 * \retval SHR_E_NOT_FOUND Entry ID map for source ID is not found.
 */
extern int
bcmfp_idp_map_traverse(int unit,
                       bcmfp_stage_id_t stage_id,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       uint32_t *src_id,
                       bcmfp_entry_id_traverse_cb cb,
                       void *user_data);

/*!
 * \brief Traverse through all entry IDs mapped to source ID.
 *  Source ID can be any one ID from group_id, rule_id, policy_id,
 *  meter_id, ctr_entry_id, CID. While traversing call the callback
 *  function provide for each entry Id alonfg with user_data as one
 *  of the parameter.
 *
 * \param [in] unit        Logical device id
 * \param [in] stage_id    stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment HA array space reserved for src_id to
 *                         entry_id mapping.
 * \param [in] map_size    Number of array elements in map_segment.
 * \param [in] map_type    BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id      Source ID in use for entry_id map lookup.
 * \param [in] cb          Callback function to be called for every
 *                         entry ID mapped to source ID.
 * \param [in] user_data   Data, provided by the caller, to be
 *                         passed to callback function as one of the
 *                         parameter.
 * \param [in] cond_met    traverse till the cont_met is set to true.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 * \retval SHR_E_NOT_FOUND Entry ID map for source ID is not found.
 */
extern int
bcmfp_idp_map_cond_traverse(int unit,
                            bcmfp_stage_id_t stage_id,
                            void *map_segment,
                            uint32_t map_size,
                            bcmfp_idp_map_type_t map_type,
                            uint32_t *src_id,
                            bcmfp_entry_id_cond_traverse_cb cb,
                            void *user_data,
                            bool *cond_met);
/*!
 * \brief Add the entry ID mapped to source ID. Source ID can be
 *  any one ID from group_id, rule_id, policy_id, meter_id,
 *  ctr_entry_id, CID. While traversing call the callback
 *  function provide for each entry Id alonfg with user_data as
 *  one of the parameter.
 *
 * \param [in] unit           Logical device id
 * \param [in] stage_id       stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment    HA array space reserved for src_id to
 *                            entry_id mapping.
 * \param [in] map_size       Number of array elements in map_segment.
 * \param [in] map_type       BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id         Source ID in use for entry_id map lookup.
 * \param [in] ent_or_grp_id  FP Entry ID or FP group ID.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 */
extern int
bcmfp_idp_map_add(int unit,
                  bcmfp_stage_id_t stage_id,
                  void *map_segment,
                  uint32_t map_size,
                  bcmfp_idp_map_type_t map_type,
                  uint32_t *src_id,
                  uint32_t ent_or_grp_id);
/*!
 * \brief Delete the entry ID mapped to source ID. Source ID can
 *  be any one ID from group_id, rule_id, policy_id, meter_id,
 *  ctr_entry_id, CID. While traversing call the callback
 *  function provide for each entry Id alonfg with user_data as
 *  one of the parameter.
 *
 * \param [in] unit           Logical device id
 * \param [in] stage_id       stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment    HA array space reserved for src_id to
 *                            entry_id mapping.
 * \param [in] map_size       Number of array elements in map_segment.
 * \param [in] map_type       BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id         Source ID in use for entry_id map lookup.
 * \param [in] ent_or_grp_id  FP Entry ID or FP group ID.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 * \retval SHR_E_NOT_FOUND Entry ID map for source ID is not found.
 */
extern int
bcmfp_idp_map_delete(int unit,
                     bcmfp_stage_id_t stage_id,
                     void *map_segment,
                     uint32_t map_size,
                     bcmfp_idp_map_type_t map_type,
                     uint32_t *src_id,
                     uint32_t ent_or_grp_id);

/*!
 * \brief Get the first element of all entry IDs mapped to source ID.
 *  Source ID can be any one ID from group_id, rule_id, policy_id,
 *  meter_id, ctr_entry_id, CID. After first element, call the callback
 *  function provided for each entry Id along with user_data as one
 *  of the parameter.
 *
 * \param [in] unit        Logical device id
 * \param [in] stage_id    stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment HA array space reserved for src_id to
 *                         entry_id mapping.
 * \param [in] map_size    Number of array elements in map_segment.
 * \param [in] map_type    BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] src_id      Source ID in use for entry_id map lookup.
 * \param [in] cb          Callback function to be called for every
 *                         entry ID mapped to source ID.
 * \param [in] user_data   Data, provided by the caller, to be
 *                         passed to callback function as one of the
 *                         parameter.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 * \retval SHR_E_NOT_FOUND Entry ID map for source ID is not found.
 */
extern int
bcmfp_idp_map_traverse_first(int unit,
                       bcmfp_stage_id_t stage_id,
                       void *map_segment,
                       uint32_t map_size,
                       bcmfp_idp_map_type_t map_type,
                       uint32_t *src_id,
                       bcmfp_entry_id_traverse_cb cb,
                       void *user_data);
/*!
 * \brief Get the first source id of the given the hash value.
 *
 * \param [in] unit        Logical device id
 * \param [in] stage_id    stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment HA array space reserved for src_id to
 *                         entry_id mapping.
 * \param [in] map_size    Number of array elements in map_segment.
 * \param [in] map_type    BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] hash_value  Hash value to use for lookup.
 * \param [out] src_id     Source ID with hash_value as the hash.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 */
extern int
bcmfp_idp_map_src_id_get_first(int unit,
                         bcmfp_stage_id_t stage_id,
                         void *map_segment,
                         uint32_t map_size,
                         bcmfp_idp_map_type_t map_type,
                         uint32_t hash_value,
                         uint32_t *src_id);
/*!
 * \brief Get the next source id to the given source id having the
 *  same the hash value.
 *
 * \param [in] unit          Logical device id
 * \param [in] stage_id      stage id(BCMFP_STAGE_ID_XXX).
 * \param [in] map_segment   HA array space reserved for src_id to
 *                           entry_id mapping.
 * \param [in] map_size      Number of array elements in map_segment.
 * \param [in] map_type      BCMFP_IDP_MAP_TYPE_XXX.
 * \param [in] hash_value    Hash value to use for lookup.
 * \param [in] src_id        Source ID with hash_value as the hash.
 * \param [out] next_src_id  Next source ID with hash_value as hash and
 *                           placed next to src_id.
 *
 * \retval SHR_E_NONE      Success
 * \retval SHR_E_PARAM     Wrong parameters
 */
extern int
bcmfp_idp_map_src_id_get_next(int unit,
                         bcmfp_stage_id_t stage_id,
                         void *map_segment,
                         uint32_t map_size,
                         bcmfp_idp_map_type_t map_type,
                         uint32_t hash_value,
                         uint32_t *src_id,
                         uint32_t *next_src_id);
#endif /* BCMFP_IDP_INTERNAL_H */
