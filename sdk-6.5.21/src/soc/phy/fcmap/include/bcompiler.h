/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCOMPILER_COMPILER_H
#define BCOMPILER_COMPILER_H

/*
 * The following macro is to satidfy compiler unused variable warnings.
 */
#define BCOMPILER_COMPILER_SATISFY(v) (void)(v)

# ifdef BE_HOST
#  define b_u64_MSW	0
#  define b_u64_LSW	1
# else /* LE_HOST */
#  define b_u64_MSW	1
#  define b_u64_LSW	0
# endif /* LE_HOST */

#ifdef LONGS_ARE_64BITS

#define BCOMPILER_COMPILER_64BIT
#define	BCOMPILER_COMPILER_UINT64	    unsigned long
#define BCOMPILER_COMPILER_INT64      long
#define bu64_H(v)		    (((buint32_t *) &(v))[b_u64_MSW])
#define bu64_L(v)		    (((buint32_t *) &(v))[b_u64_LSW])

#else /* !LONGS_ARE_64BITS */

#ifdef COMPILER_HAS_LONGLONG

#define BCOMPILER_COMPILER_64BIT
#define	BCOMPILER_COMPILER_UINT64	    unsigned long long
#define BCOMPILER_COMPILER_INT64      long long
#define bu64_H(v)		    (((buint32_t *) &(v))[b_u64_MSW])
#define bu64_L(v)		    (((buint32_t *) &(v))[b_u64_LSW])

#else /* !COMPILER_HAS_LONGLONG */

#define BCOMPILER_COMPILER_UINT64	    struct { unsigned int u64_w[2]; }
#define BCOMPILER_COMPILER_INT64	    struct { int u64_w[2]; }
#define bu64_H(v)                    ((v).u64_w[b_u64_MSW])
#define bu64_L(v)                    ((v).u64_w[b_u64_LSW])

#endif /* !COMPILER_HAS_LONGLONG */
#endif /* LONGS_ARE_64BITS */

/*
 * 32-/64-bit type conversions
 */

#ifdef COMPILER_HAS_LONGLONG_SHIFT

#define BCOMPILER_COMPILER_64_TO_32_LO(dst, src)	((dst) = (buint32_t) (src))
#define BCOMPILER_COMPILER_64_TO_32_HI(dst, src)	((dst) = (buint32_t) ((src) >> 32))
#define BCOMPILER_COMPILER_64_HI(src)		((buint32_t) ((src) >> 32))
#define BCOMPILER_COMPILER_64_LO(src)		((buint32_t) (src))
#define BCOMPILER_COMPILER_64_ZERO(dst)		((dst) = 0)
#define BCOMPILER_COMPILER_64_IS_ZERO(src)	((src) == 0)
                                       

#define BCOMPILER_COMPILER_64_SET(dst, src_hi, src_lo)				\
    ((dst) = (((buint64_t) (src_hi)) << 32) | ((buint64_t) (src_lo)))

#else /* !COMPILER_HAS_LONGLONG_SHIFT */

#define BCOMPILER_COMPILER_64_TO_32_LO(dst, src)	((dst) = bu64_L(src))
#define BCOMPILER_COMPILER_64_TO_32_HI(dst, src)	((dst) = bu64_H(src))
#define BCOMPILER_COMPILER_64_HI(src)		bu64_H(src)
#define BCOMPILER_COMPILER_64_LO(src)		bu64_L(src)
#define BCOMPILER_COMPILER_64_ZERO(dst)		(bu64_H(dst) = bu64_L(dst) = 0)
#define BCOMPILER_COMPILER_64_IS_ZERO(src)	(bu64_H(src) == 0 && bu64_L(src) == 0)

#define BCOMPILER_COMPILER_64_SET(dst, src_hi, src_lo)				\
        do {								\
            bu64_H(dst) = (src_hi);                              	\
            bu64_L(dst) = (src_lo);                              	\
        } while (0)

#endif /* !COMPILER_HAS_LONGLONG_SHIFT */

/*
 * 64-bit addition and subtraction
 */

#ifdef COMPILER_HAS_LONGLONG_ADDSUB

#define BCOMPILER_COMPILER_64_ADD_64(dst, src)	((dst) += (src))
#define BCOMPILER_COMPILER_64_ADD_32(dst, src)	((dst) += (src))
#define BCOMPILER_COMPILER_64_SUB_64(dst, src)	((dst) -= (src))
#define BCOMPILER_COMPILER_64_SUB_32(dst, src)	((dst) -= (src))

#else /* !COMPILER_HAS_LONGLONG_ADDSUB */

#define BCOMPILER_COMPILER_64_ADD_64(dst, src)				\
        do {								\
	    buint32_t __t = bu64_L(dst);				\
	    bu64_L(dst) += bu64_L(src);					\
	    if (bu64_L(dst) < __t) {					\
	        bu64_H(dst) += bu64_H(src) + 1;				\
	    } else {							\
	        bu64_H(dst) += bu64_H(src);				\
	    }								\
	} while (0)
#define BCOMPILER_COMPILER_64_ADD_32(dst, src)					\
        do {								\
	    buint32_t __t = bu64_L(dst);					\
	    bu64_L(dst) += (src);					\
	    if (bu64_L(dst) < __t) {					\
	        bu64_H(dst)++;						\
	    }								\
	} while (0)
#define BCOMPILER_COMPILER_64_SUB_64(dst, src)					\
        do {								\
	    buint32_t __t = bu64_L(dst);					\
	    bu64_L(dst) -= bu64_L(src);					\
	    if (bu64_L(dst) > __t) {					\
	        bu64_H(dst) -= bu64_H(src) + 1;				\
	    } else {							\
	        bu64_H(dst) -= bu64_H(src);				\
	    }								\
	} while (0)
#define BCOMPILER_COMPILER_64_SUB_32(dst, src)					\
        do {								\
	    buint32_t __t = bu64_L(dst);					\
	    bu64_L(dst) -= (src);					\
	    if (bu64_L(dst) > __t) {					\
	        bu64_H(dst)--;						\
	    }								\
	} while (0)

#endif /* !COMPILER_HAS_LONGLONG_ADDSUB */

/*
 * 64-bit logical operations
 */

#ifdef COMPILER_HAS_LONGLONG_ANDOR

#define BCOMPILER_COMPILER_64_AND(dst, src)	((dst) &= (src))
#define BCOMPILER_COMPILER_64_OR(dst, src)	((dst) |= (src))
#define BCOMPILER_COMPILER_64_XOR(dst, src)	((dst) ^= (src))
#define BCOMPILER_COMPILER_64_NOT(dst)		((dst) = ~(dst))

#else /* !COMPILER_HAS_LONGLONG_ANDOR */

#define BCOMPILER_COMPILER_64_AND(dst, src)					\
	do {								\
	    bu64_H(dst) &= bu64_H(src);					\
	    bu64_L(dst) &= bu64_L(src);					\
	} while (0)
#define BCOMPILER_COMPILER_64_OR(dst, src)					\
	do {								\
	    bu64_H(dst) |= bu64_H(src);					\
	    bu64_L(dst) |= bu64_L(src);					\
	} while (0)
#define BCOMPILER_COMPILER_64_XOR(dst, src)					\
	do {								\
	    bu64_H(dst) ^= bu64_H(src);					\
	    bu64_L(dst) ^= bu64_L(src);					\
	} while (0)
#define BCOMPILER_COMPILER_64_NOT(dst)						\
	do {								\
	    bu64_H(dst) = ~bu64_H(dst);					\
	    bu64_L(dst) = ~bu64_L(dst);					\
	} while (0)

#endif /* !COMPILER_HAS_LONGLONG_ANDOR */

#define BCOMPILER_COMPILER_64_ALLONES(dst)   \
    BCOMPILER_COMPILER_64_ZERO(dst);\
    BCOMPILER_COMPILER_64_NOT(dst)

/*
 * 64-bit shift
 */

#ifdef COMPILER_HAS_LONGLONG_SHIFT

#define BCOMPILER_COMPILER_64_SHL(dst, bits)	((dst) <<= (bits))
#define BCOMPILER_COMPILER_64_SHR(dst, bits)	((dst) >>= (bits))

#define BCOMPILER_COMPILER_64_BITTEST(val, n)     \
    (!BCOMPILER_COMPILER_64_IS_ZERO(BCOMPILER_COMPILER_64_AND(val, ((buint64_t) 1) << (n)))))

#else /*  COMPILER_HAS_LONGLONG_SHIFT */

#define BCOMPILER_COMPILER_64_SHL(dst, bits)					\
	do {								\
	    int __b = (bits);						\
	    if (__b >= 32) {						\
		bu64_H(dst) = bu64_L(dst);				\
		bu64_L(dst) = 0;						\
		__b -= 32;						\
	    }								\
	    bu64_H(dst) = (bu64_H(dst) << __b) |				\
		(__b ? bu64_L(dst) >> (32 - __b) : 0);			\
	    bu64_L(dst) <<= __b;						\
	} while (0)

#define BCOMPILER_COMPILER_64_SHR(dst, bits)					\
	do {								\
	    int __b = (bits);						\
	    if (__b >= 32) {						\
		bu64_L(dst) = bu64_H(dst);				\
		bu64_H(dst) = 0;						\
		__b -= 32;						\
	    }								\
	    bu64_L(dst) = (bu64_L(dst) >> __b) |				\
		(__b ? bu64_H(dst) << (32 - __b) : 0);			\
	    bu64_H(dst) >>= __b;						\
	} while (0)

#define BCOMPILER_COMPILER_64_BITTEST(val, n)     \
    ( (((n) < 32) && (bu64_L(val) & (1 << (n)))) || \
      (((n) >= 32) && (bu64_H(val) & (1 << ((n) - 32)))) )

#endif /* !COMPILER_HAS_LONGLONG_SHIFT */

/*
 * 64-bit compare operations
 */

#ifdef COMPILER_HAS_LONGLONG_COMPARE

#define BCOMPILER_COMPILER_64_EQ(src1, src2)  ((src1) == (src2))
#define BCOMPILER_COMPILER_64_NE(src1, src2)  ((src1) != (src2))
#define BCOMPILER_COMPILER_64_LT(src1, src2)  ((src1) <  (src2))
#define BCOMPILER_COMPILER_64_LE(src1, src2)  ((src1) <= (src2))
#define BCOMPILER_COMPILER_64_GT(src1, src2)  ((src1) >  (src2))
#define BCOMPILER_COMPILER_64_GE(src1, src2)  ((src1) >= (src2))

#else /* !COMPILER_HAS_LONGLONG_COMPARE */

#define BCOMPILER_COMPILER_64_EQ(src1, src2)  (bu64_H(src1) == bu64_H(src2) && \
					 bu64_L(src1) == bu64_L(src2))
#define BCOMPILER_COMPILER_64_NE(src1, src2)  (bu64_H(src1) != bu64_H(src2) || \
					 bu64_L(src1) != bu64_L(src2))
#define BCOMPILER_COMPILER_64_LT(src1, src2)  (bu64_H(src1) < bu64_H(src2) || \
					 ((bu64_H(src1) == bu64_H(src2) && \
					   bu64_L(src1) < bu64_L(src2))))
#define BCOMPILER_COMPILER_64_LE(src1, src2)  (bu64_H(src1) < bu64_H(src2) || \
					 ((bu64_H(src1) == bu64_H(src2) && \
					   bu64_L(src1) <= bu64_L(src2))))
#define BCOMPILER_COMPILER_64_GT(src1, src2)  (bu64_H(src1) > bu64_H(src2) || \
					 ((bu64_H(src1) == bu64_H(src2) && \
					   bu64_L(src1) > bu64_L(src2))))
#define BCOMPILER_COMPILER_64_GE(src1, src2)  (bu64_H(src1) > bu64_H(src2) || \
					 ((bu64_H(src1) == bu64_H(src2) && \
					   bu64_L(src1) >= bu64_L(src2))))

#endif /* !COMPILER_HAS_LONGLONG_COMPARE */

/* Set up a mask of width bits offset lft_shft.  No error checking */
#define BCOMPILER_COMPILER_64_MASK_CREATE(dst, width, lft_shift)          \
    do {                                                                \
        BCOMPILER_COMPILER_64_ALLONES(dst);                               \
        BCOMPILER_COMPILER_64_SHR((dst), (64 - (width)));                 \
        BCOMPILER_COMPILER_64_SHL((dst), (lft_shift));                    \
    } while (0)

#define BCOMPILER_COMPILER_64_DELTA(src, last, new)\
        do { \
            BCOMPILER_COMPILER_64_ZERO(src);\
            BCOMPILER_COMPILER_64_ADD_64(src, new);\
            BCOMPILER_COMPILER_64_SUB_64(src, last);\
        } while(0)

/*
 * Some macros for double support
 *
 * You can use the COMPILER_DOUBLE macro
 * if you would prefer double precision, but it is not necessary.
 * If you need more control (or you need to print :), then
 * then you should use the COMPILER_HAS_DOUBLE macro explicitly.
 */
#ifdef COMPILER_HAS_DOUBLE
#define BCOMPILER_COMPILER_DOUBLE double
#define BCOMPILER_COMPILER_DOUBLE_FORMAT "f"
#define BCOMPILER_COMPILER_64_TO_DOUBLE(f, i64) \
    ((f) = BCOMPILER_COMPILER_64_HI(i64) * 4294967296.0 + BCOMPILER_COMPILER_64_LO(i64))
#define BCOMPILER_COMPILER_32_TO_DOUBLE(f, i32) \
    ((f) = (double) (i32))
#else
#define BCOMPILER_COMPILER_DOUBLE buint32_t
#define BCOMPILER_COMPILER_DOUBLE_FORMAT "u"
#define BCOMPILER_COMPILER_64_TO_DOUBLE(f, i64) ((f) = BCOMPILER_COMPILER_64_LO(i64))
#define BCOMPILER_COMPILER_32_TO_DOUBLE(f, i32) ((f) = (i32))
#endif


#endif /* BCOMPILER_COMPILER_H */

