/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:     diag_framer_access.h
 * Purpose:     Framer symbol accessing definitions
 */

#ifndef _DIAG_FRAMER_ACCESS_H
#define _DIAG_FRAMER_ACCESS_H

#ifndef NO_FRAMER_LIB_BUILD

#include "j2x_uas_framer_reg_access.h"

/*
 * For Normal Symbols.
 */

#define SYM_IS(_sym, _b, _o, _aw) \
    ((_b) == _sym##__base && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset + _aw - 1)))
#define SYM_WIDX(_sym, _b, _o, _aw) \
    (_sym##__addr(_b, _o) ? ((_o) - _sym##__offset) % _aw : -1)
#define SYM_IS_ARRAY(_sym, _b, _o, _aw) \
    ((_b) == _sym##__base && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset_max + _aw - 1)))
#define SYM_ARRAY_IDX(_sym, _b, _o, _aw) \
    (_sym##__addr(_b, _o) ? ((_o) - _sym##__offset + _aw - 1) / _aw : -1)
#define SYM_ARRAY_WIDX(_sym, _b, _o, _aw) \
    (_sym##__addr(_b, _o) ? ((_o) - _sym##__offset) % _aw : -1)

#define SYM_FIELD_SET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_set(_r._fld, _dw, _s, _w, _vp)
#define SYM_FIELD32_SET(_fld, _r, _dw, _s, _w, _v) \
    framer_field32_set(_r._fld, _dw, _s, _w, _v)
#define SYM_FIELD_GET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_get(_r._fld, _dw, _s, _w, _vp)

#define SYM_READ(_sym, _u, _dw, _r) \
    framer_reg_raw_read(_u, _sym##__base, _sym##__offset, (_dw), _r._sym)
#define SYM_WRITE(_sym, _u, _dw, _r) \
    framer_reg_raw_write(_u, _sym##__base, _sym##__offset, (_dw), _r._sym)
#define SYM_FIELD_MODIFY(_sym, _fld, _u, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _r); \
    } while(0)
#define SYM_FIELD32_MODIFY(_sym, _fld, _u, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _r); \
    } while(0)
#define SYM_ARRAY_READ(_sym, _u, _dw, _ai, _r) \
    framer_reg_raw_read(_u, _sym##__base, _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_WRITE(_sym, _u, _dw, _ai, _r) \
    framer_reg_raw_write(_u, _sym##__base, _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_FIELD_MODIFY(_sym, _fld, _u, _ai, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _ai, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _ai, _r); \
    } while(0)
#define SYM_ARRAY_FIELD32_MODIFY(_sym, _fld, _u, _ai, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _ai, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _ai, _r); \
    } while(0)

#define SYM_TABLE_READ(_sym, _u, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base, _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_WRITE(_sym, _u, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base, _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_FIELD_MODIFY(_sym, _fld, _u, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _i, _m); \
    } while(0)
#define SYM_TABLE_FIELD32_MODIFY(_sym, _fld, _u, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _i, _m); \
    } while(0)

#define SYM_TABLE_ARRAY_READ(_sym, _u, _ai, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base, _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_WRITE(_sym, _u, _ai, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base, _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_FIELD_MODIFY(_sym, _fld, _u, _ai, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _ai, _i, _m); \
    } while(0)
#define SYM_TABLE_ARRAY_FIELD32_MODIFY(_sym, _fld, _u, _ai, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _ai, _i, _m); \
    } while(0)

/*
 * For Block Number.
 */

#define SYM_BLKN_IS(_sym, _blkn, _b, _o, _aw) \
    ((_b) == _sym##__base_BLKn(_sym, _blkn) && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset + _aw - 1)))
#define SYM_BLKN_WIDX(_sym, _blkn, _b, _o, _aw) \
    (_sym##__addr(_blkn, _b, _o) ? ((_o) - _sym##__offset) % _aw : -1)
#define SYM_BLKN_IS_ARRAY(_sym, _blkn, _b, _o, _aw) \
    ((_b) == _sym##__base_BLKn(_sym, _blkn) && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset_max + _aw - 1)))
#define SYM_BLKN_ARRAY_IDX(_sym, _blkn, _b, _o, _aw) \
    (_sym##__addr(_blkn, _b, _o) ? ((_o) - _sym##__offset + _aw - 1) / _aw : -1)
#define SYM_BLKN_ARRAY_WIDX(_sym, _blkn, _b, _o, _aw) \
    (_sym##__addr(_blkn, _b, _o) ? ((_o) - _sym##__offset) % _aw : -1)

#define SYM_BLKN_FIELD_SET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_set(_r._fld, _dw, _s, _w, _vp)
#define SYM_BLKN_FIELD32_SET(_fld, _r, _dw, _s, _w, _v) \
    framer_field32_set(_r._fld, _dw, _s, _w, _v)
#define SYM_BLKN_FIELD_GET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_get(_r._fld, _dw, _s, _w, _vp)

#define SYM_BLKN_READ(_sym, _u, _blkn, _dw, _r) \
    framer_reg_raw_read(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset, (_dw), _r._sym)
#define SYM_BLKN_WRITE(_sym, _u, _blkn, _dw, _r) \
    framer_reg_raw_write(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset, (_dw), _r._sym)
#define SYM_BLKN_FIELD_MODIFY(_sym, _fld, _u, _blkn, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _blkn, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _blkn, _r); \
    } while(0)
#define SYM_BLKN_FIELD32_MODIFY(_sym, _fld, _u, _blkn, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _blkn, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _blkn, _r); \
    } while(0)

#define SYM_ARRAY_BLKN_READ(_sym, _u, _blkn, _dw, _ai, _r) \
    framer_reg_raw_read(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_BLKN_WRITE(_sym, _u, _blkn, _dw, _ai, _r) \
    framer_reg_raw_write(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_BLKN_FIELD_MODIFY(_sym, _fld, _u, _blkn, _ai, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _blkn, _ai, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _blkn, _ai, _r); \
    } while(0)
#define SYM_ARRAY_BLKN_FIELD32_MODIFY(_sym, _fld, _u, _blkn, _ai, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _blkn, _ai, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _blkn, _ai, _r); \
    } while(0)

#define SYM_TABLE_BLKN_READ(_sym, _u, _blkn, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_BLKN_WRITE(_sym, _u, _blkn, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_BLKN_FIELD_MODIFY(_sym, _fld, _u, _blkn, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _blkn, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _blkn, _i, _m); \
    } while(0)
#define SYM_TABLE_BLKN_FIELD32_MODIFY(_sym, _fld, _u, _blkn, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _blkn, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _blkn, _i, _m); \
    } while(0)

#define SYM_TABLE_ARRAY_BLKN_READ(_sym, _u, _blkn, _ai, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_BLKN_WRITE(_sym, _u, _blkn, _ai, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base_BLKn(_sym, _blkn), _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_BLKN_FIELD_MODIFY(_sym, _fld, _u, _blkn, _ai, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _blkn, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _blkn, _ai, _i, _m); \
    } while(0)
#define SYM_TABLE_ARRAY_BLKN_FIELD32_MODIFY(_sym, _fld, _u, _blkn, _ai, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _blkn, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _blkn, _ai, _i, _m); \
    } while(0)

/*
 * For Instance ID.
 */

#define SYM_INST_IS(_sym, _inst, _b, _o, _aw) \
    ((_b) == _sym##__base_INST(_sym, _inst) && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset + _aw - 1)))
#define SYM_INST_WIDX(_sym, _inst, _b, _o, _aw) \
    (_sym##__addr(_inst, _b, _o) ? ((_o) - _sym##__offset) % _aw : -1)
#define SYM_INST_IS_ARRAY(_sym, _inst, _b, _o, _aw) \
    ((_b) == _sym##__base_INST(_sym, _inst) && (_sym##__offset <= (_o) && (_o) <= (_sym##__offset_max + _aw - 1)))
#define SYM_INST_ARRAY_IDX(_sym, _inst, _b, _o, _aw) \
    (_sym##__addr(_inst, _b, _o) ? ((_o) - _sym##__offset + _aw - 1) / _aw : -1)
#define SYM_INST_ARRAY_WIDX(_sym, _inst, _b, _o, _aw) \
    (_sym##__addr(_inst, _b, _o) ? ((_o) - _sym##__offset) % _aw : -1)

#define SYM_INST_FIELD_SET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_set(_r._fld, _dw, _s, _w, _vp)
#define SYM_INST_FIELD32_SET(_fld, _r, _dw, _s, _w, _v) \
    framer_field32_set(_r._fld, _dw, _s, _w, _v)
#define SYM_INST_FIELD_GET(_fld, _r, _dw, _s, _w, _vp) \
    framer_field_get(_r._fld, _dw, _s, _w, _vp)

#define SYM_INST_READ(_sym, _u, _inst, _dw, _r) \
    framer_reg_raw_read(_u, _sym##__base_INST(_sym, _inst), _sym##__offset, (_dw), _r._sym)
#define SYM_INST_WRITE(_sym, _u, _inst, _dw, _r) \
    framer_reg_raw_write(_u, _sym##__base_INST(_sym, _inst), _sym##__offset, (_dw), _r._sym)
#define SYM_INST_FIELD_MODIFY(_sym, _fld, _u, _inst, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _inst, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _inst, _r); \
    } while(0)
#define SYM_INST_FIELD32_MODIFY(_sym, _fld, _u, _inst, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _inst, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _inst, _r); \
    } while(0)

#define SYM_ARRAY_INST_READ(_sym, _u, _inst, _dw, _ai, _r) \
    framer_reg_raw_read(_u, _sym##__base_INST(_sym, _inst), _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_INST_WRITE(_sym, _u, _inst, _dw, _ai, _r) \
    framer_reg_raw_write(_u, _sym##__base_INST(_sym, _inst), _sym##__offset + (_dw) * (_ai), (_dw), _r._sym)
#define SYM_ARRAY_INST_FIELD_MODIFY(_sym, _fld, _u, _inst, _ai, _vp) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _inst, _ai, _r); \
        _sym##__##_fld##__set(_r, _vp); \
        _sym##__write(_u, _inst, _ai, _r); \
    } while(0)
#define SYM_ARRAY_INST_FIELD32_MODIFY(_sym, _fld, _u, _inst, _ai, _v) \
    do { \
        _sym##__t _r; \
        _sym##__read(_u, _inst, _ai, _r); \
        _sym##__##_fld##__set(_r, _v); \
        _sym##__write(_u, _inst, _ai, _r); \
    } while(0)

#define SYM_TABLE_INST_READ(_sym, _u, _inst, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base_INST(_sym, _inst), _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_INST_WRITE(_sym, _u, _inst, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base_INST(_sym, _inst), _sym##__offset, (_i), (_w), _m._sym)
#define SYM_TABLE_INST_FIELD_MODIFY(_sym, _fld, _u, _inst, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _inst, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _inst, _i, _m); \
    } while(0)
#define SYM_TABLE_INST_FIELD32_MODIFY(_sym, _fld, _u, _inst, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _inst, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _inst, _i, _m); \
    } while(0)

#define SYM_TABLE_ARRAY_INST_READ(_sym, _u, _inst, _ai, _i, _w, _m) \
    framer_mem_raw_read(_u, _sym##__base_INST(_sym, _inst), _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_INST_WRITE(_sym, _u, _inst, _ai, _i, _w, _m) \
    framer_mem_raw_write(_u, _sym##__base_INST(_sym, _inst), _sym##__offset + (_ai), (_i), (_w), _m._sym)
#define SYM_TABLE_ARRAY_INST_FIELD_MODIFY(_sym, _fld, _u, _inst, _ai, _i, _vp) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _inst, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _vp); \
        _sym##__write(_u, _inst, _ai, _i, _m); \
    } while(0)
#define SYM_TABLE_ARRAY_INST_FIELD32_MODIFY(_sym, _fld, _u, _inst, _ai, _i, _v) \
    do { \
        _sym##__t _m; \
        _sym##__read(_u, _inst, _ai, _i, _m); \
        _sym##__##_fld##__set(_m, _v); \
        _sym##__write(_u, _inst, _ai, _i, _m); \
    } while(0)

extern int framer_reg_raw_read(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 dw,
    J2X_U32 * data);

extern int framer_reg_raw_write(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 dw,
    J2X_U32 * data);

extern int framer_mem_raw_read(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 index,
    J2X_U32 width,
    J2X_U32 * data);

extern int framer_mem_raw_write(
    int unit,
    J2X_U32 base_addr,
    J2X_U32 offset,
    J2X_U32 index,
    J2X_U32 width,
    J2X_U32 * data);

extern int framer_field_set(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 * value);

extern int framer_field32_set(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 value);

extern int framer_field_get(
    J2X_U32 * data,
    J2X_U32 dw,
    J2X_U32 shift,
    J2X_U32 width,
    J2X_U32 * value);

#endif /* NO_FRAMER_LIB_BUILD */
#endif /* _DIAG_FRAMER_ACCESS_H */
