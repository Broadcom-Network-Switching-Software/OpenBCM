/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines FIELD constants
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to share FIELD constants.
 */

#ifndef _SHR_FIELD_H
#define _SHR_FIELD_H

#define _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_SHIFT               0
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_MASK                0x1FFFFF
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_SHIFT               21
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_MASK                0x7F
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_FLAG_SHIFT                28
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_FLAG_MASK                 0xF0000000
/*
 *  FIELD macros for large direct lookup entry
 *  The format of large direct lookup entry is:
 *    {4'b0001, _group_id[6:0], _index[20:0]}
 */
#define _SHR_FIELD_LARGE_DIRECT_ENTRY_ID_PACK(_entry_id, _group_id, _index) \
    ((_entry_id) = (BCM_FIELD_ENTRY_LARGE_DIRECT_LOOKUP &_SHR_FIELD_LARGE_DIRECT_ENTRY_FLAG_MASK) | \
    (((_group_id) & _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_MASK) << _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_SHIFT) | \
    (((_index) & _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_MASK) << _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_SHIFT))

#define _SHR_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(_entry_id, _group_id, _index) \
    (_group_id) = (((_entry_id) >> _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_SHIFT) & _SHR_FIELD_LARGE_DIRECT_ENTRY_GROUP_MASK); \
    (_index) = (((_entry_id) >> _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_SHIFT) & _SHR_FIELD_LARGE_DIRECT_ENTRY_INDEX_MASK);

#endif  /* !_SHR_FIELD_H */
