/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines MPLS constants 
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to share  MPLS constants.
 */

#ifndef _SHR_MPLS_H
#define _SHR_MPLS_H

#define _SHR_MPLS_INDEXED_LABEL_INDEX_SHIFT                      20
#define _SHR_MPLS_INDEXED_LABEL_INDEX_MASK                       0x7
#define _SHR_MPLS_INDEXED_LABEL_VALUE_SHIFT                      0
#define _SHR_MPLS_INDEXED_LABEL_VALUE_MASK                       0xFFFFF

/* 
 *  MPLS macros for indexed support
 */
#define _SHR_MPLS_INDEXED_LABEL_SET(_label, _label_value,_index)            \
        ((_label) = (((_index) & _SHR_MPLS_INDEXED_LABEL_INDEX_MASK)  << _SHR_MPLS_INDEXED_LABEL_INDEX_SHIFT)  | \
        (((_label_value) & _SHR_MPLS_INDEXED_LABEL_VALUE_MASK) << _SHR_MPLS_INDEXED_LABEL_VALUE_SHIFT))

#define _SHR_MPLS_INDEXED_LABEL_VALUE_GET(_label)   \
        (((_label) >> _SHR_MPLS_INDEXED_LABEL_VALUE_SHIFT) & _SHR_MPLS_INDEXED_LABEL_VALUE_MASK)

#define _SHR_MPLS_INDEXED_LABEL_INDEX_GET(_label)   \
        (((_label) >> _SHR_MPLS_INDEXED_LABEL_INDEX_SHIFT) & _SHR_MPLS_INDEXED_LABEL_INDEX_MASK)

#endif	/* !_SHR_MPLS_H */
