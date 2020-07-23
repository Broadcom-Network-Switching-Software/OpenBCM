/** \file include/shared/trunk.h
 * 
 * 
 *  This file contains definitions for papi to use in context of
 *  the trunk module.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_TRUNK_H
#define _SHR_TRUNK_H

#define _SHR_TRUNK_ID_BIT               25
#define _SHR_TRUNK_STACKING_BIT         24
#define _SHR_TRUNK_ID_MASK              1 << _SHR_TRUNK_ID_BIT
#define _SHR_TRUNK_STACKING_MASK        1 << _SHR_TRUNK_STACKING_BIT
#define _SHR_TRUNK_POOL_BITS            3
#define _SHR_TRUNK_POOL_OFFSET          17
#define _SHR_TRUNK_GROUP_BITS           17
#define _SHR_TRUNK_GROUP_OFFSET         0

#define _SHR_TRUNK_ID_SET(trunk_id, pool, group)  \
    (trunk_id = (_SHR_TRUNK_ID_MASK) | \
    ((pool) << (_SHR_TRUNK_POOL_OFFSET)) | \
    ((group) << (_SHR_TRUNK_GROUP_OFFSET)))

#define _SHR_TRUNK_ID_POOL_GET(pool, trunk_id)  \
    (pool = ((trunk_id) & ((1 << (_SHR_TRUNK_POOL_OFFSET + _SHR_TRUNK_POOL_BITS)) - 1)) >> (_SHR_TRUNK_POOL_OFFSET))
    
#define _SHR_TRUNK_ID_GROUP_GET(group, trunk_id)  \
    (group = ((trunk_id) & ((1 << (_SHR_TRUNK_GROUP_OFFSET + _SHR_TRUNK_GROUP_BITS)) - 1)) >> (_SHR_TRUNK_GROUP_OFFSET))

#define _SHR_TRUNK_ID_IS_VALID(trunk_id) \
    (((trunk_id) & (_SHR_TRUNK_ID_MASK)) != 0)

#define _SHR_TRUNK_STACKING_ID_SET(trunk_id, peer_tm_domain) \
    (trunk_id = ((peer_tm_domain) | (_SHR_TRUNK_ID_MASK) | (_SHR_TRUNK_STACKING_MASK)))

#define _SHR_TRUNK_ID_IS_STACKING(trunk_id) \
    ((((trunk_id) & (_SHR_TRUNK_STACKING_MASK)) != 0) && (((trunk_id) & (_SHR_TRUNK_ID_MASK)) != 0))

#define _SHR_TRUNK_STACKING_PEER_TMD_GET(peer_tm_domain, trunk_id) \
    ((peer_tm_domain) = ((peer_tm_domain) & ( ~((_SHR_TRUNK_ID_MASK) | (_SHR_TRUNK_STACKING_MASK)) )))

#endif /* _SHR_TRUNK_H */
