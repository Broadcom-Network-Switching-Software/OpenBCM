/*! \file bcmltm_pt.h
 *
 * Logical Table Manager Physical Table Pass Through (Thru) Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_PT_H
#define BCMLTM_PT_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \name BCMLTM_PT_FIELD_KEY_xxx
 *
 * \brief Physical Table Key Fields.
 *
 * The DRD does not provide field IDs for the PT parameters such as
 * the index (memory/register array), port (port-based register) keys, or
 * table instance.  The following special values are used to identify
 * these virtual fields.
 */

/*! @{ */

/*! Index Key field, used for memories or array registers. */
#define BCMLTM_PT_FIELD_KEY_INDEX  ((bcmdrd_fid_t)-1)

/*! Port Key field, used for port-based registers. */
#define BCMLTM_PT_FIELD_KEY_PORT   ((bcmdrd_fid_t)-2)

/*! Instance Key field, used for memory or registers with table instances. */
#define BCMLTM_PT_FIELD_KEY_INSTANCE   ((bcmdrd_fid_t)-3)

/*! @} */

/*!
 * \name BCMLTM_PT_FIELD_KEY_xxx_NAME
 *
 * \brief Physical Table Key Field Names.
 */

/*! @{ */

/*! Index Key field name. */
#define BCMLTM_PT_FIELD_KEY_INDEX_NAME     "BCMLT_PT_INDEX"

/*! Port Key field name. */
#define BCMLTM_PT_FIELD_KEY_PORT_NAME      "BCMLT_PT_PORT"

/*! Instance Key field name. */
#define BCMLTM_PT_FIELD_KEY_INSTANCE_NAME  "BCMLT_PT_INSTANCE"

/*! @} */

#endif /* BCMLTM_PT_H */
