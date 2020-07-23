/*! \file bcmlrd_enum.h
 *
 * DESCRIPTION
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_ENUM_H
#define BCMLRD_ENUM_H

#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Resolve enum by LRD map field data.
 *
 * Resolve enum by LRD map field data.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  sid     Logical table ID.
 * \param [in]  fid     Logical field ID.
 * \param [in]  str     Enum string to be resolved.
 * \param [in]  value   Value.
 *
 * \retval SHR_E_NONE   Enum resolution succeeded.
 * \retval SHR_E_PARAM  Enum resolution failed.
 */
extern int
bcmlrd_resolve_enum(int unit, uint32_t sid, uint32_t fid,
                    const char *str, uint64_t *value);

/*!
 * \brief Find symbol type name in the enum databases.
 *
 * Find a symbol type name in the enum symbol databases.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  name            Enum type name.
 * \param [out] enum_type_p     Enum type data.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval SHR_E_PARAM          NULL arguments.
 * \retval SHR_E_UNAVAIL        Enum type name not found.
 */

int
bcmlrd_enum_find_symbol_type(int unit,
                             const char *name,
                             const bcmltd_enum_type_t **enum_type_p);

/*!
 * \brief Resolve enum symbol from scalar value by enum type.
 *
 * Resolve enum symbol from scalar value by enum type name.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  name    LTL enum type name.
 * \param [in]  value   Enum scalar value.
 * \param [out] symbol  Enum symbol.
 *
 * \retval SHR_E_NONE       Enum resolution succeeded.
 * \retval SHR_E_PARAM      NULL arguments.
 * \retval SHR_E_UNAVAIL    No symbol corresponding to value.
 */
extern int
bcmlrd_enum_scalar_to_symbol_by_type(int unit,
                                     const char *name,
                                     uint64_t value,
                                     const char **symbol);

/*!
 * \brief Resolve enum scalar from enum symbol by enum type.
 *
 * Resolve enum scalar value from enum symbol by enum type name.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  name    LTL enum type name.
 * \param [in]  symbol  Enum symbol.
 * \param [out] value   Enum scalar value.
 *
 * \retval SHR_E_NONE       Enum resolution succeeded.
 * \retval SHR_E_PARAM      NULL arguments.
 * \retval SHR_E_UNAVAIL    No symbol corresponding to value.
 */
extern int
bcmlrd_enum_symbol_to_scalar_by_type(int unit,
                                     const char *name,
                                     const char *symbol,
                                     uint64_t *value);

/*!
 * \brief Get local enum value for given field.
 *
 * This routine gets the local enum value for a given global enum
 * for the specified logical table and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Field ID.
 * \param [in] gvalue Global enum value.
 * \param [out] lvalue Local enum value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmlrd_find_local_enum(int unit, uint32_t sid, uint32_t fid,
                       uint64_t gvalue, uint64_t *lvalue);

/*!
 * \brief Get global enum value for given field.
 *
 * This routine gets the global (ENUM_NAME_T) enum value for the
 * specified logical table and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Field ID.
 * \param [in] lvalue Local enum value.
 * \param [out] gvalue Global enum value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmlrd_find_global_enum(int unit, uint32_t sid, uint32_t fid,
                        uint64_t lvalue, uint64_t *gvalue);

/*!
 * \brief Get an enum symbol by table, field, scalar value.
 *
 *  Get an enum symbol by table, field, and scalar value.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  sid     Logical table ID.
 * \param [in]  fid     Logical field ID.
 * \param [in]  value   Scalar Value.
 * \param [out] symbol  Enum symbol.
 *
 * \retval SHR_E_NONE   Enum resolution succeeded.
 * \retval SHR_E_PARAM  Enum resolution failed.
 */
extern int
bcmlrd_enum_scalar_to_symbol(int unit, uint32_t sid, uint32_t fid,
                             uint64_t value, const char **symbol);

/*!
 * \brief Resolve enum scalar for given enum symbol.
 *
 * Resolve enum scalar value by enum data structure.
 *
 * \param [in]  etype   LTL enum type data.
 * \param [in]  value   Enum scalar value.
 * \param [out] symbol  Enum symbol.
 *
 * \retval SHR_E_NONE       Enum resolution succeeded.
 * \retval SHR_E_UNAVAIL    No symbol corresponding to value.
 */
int
bcmlrd_enum_scalar_lookup(const bcmltd_enum_type_t *etype,
                          uint64_t value,
                          const char **symbol);

/*!
 * \brief Resolve enum symbol for given enum value.
 *
 * Resolve enum symbol by enum data structure.
 *
 * \param [in]  etype   LTL enum type data.
 * \param [in]  symbol  Enum symbol.
 * \param [out] value   Enum scalar value.
 *
 * \retval SHR_E_NONE       Enum resolution succeeded.
 * \retval SHR_E_UNAVAIL    No symbol corresponding to value.
 */
int
bcmlrd_enum_symbol_lookup(const bcmltd_enum_type_t *etype,
                          const char *symbol,
                          uint64_t *value);

/*!
 * \brief Get symbol type for given field.
 *
 * This routine gets the symbol type data for a given table and field.
 *
 * \param [in]  unit Unit number.
 * \param [in]  sid Logical table ID.
 * \param [in]  fid Field ID.
 * \param [out] enum_type_p enum type data.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval SHR_E_PARAM   Output pointer NULL, field not a symbol.
 * \retval SHR_E_UNAVAIL sid or fid is invalid.
 */
extern int
bcmlrd_enum_symbol_type(int unit,
                        uint32_t sid,
                        uint32_t fid,
                        const bcmltd_enum_type_t **enum_type_p);

#endif /* BCMLRD_ENUM_H */
