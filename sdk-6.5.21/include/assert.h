/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This version of assert.h should appear in the compiler include path
 * ahead of any other version of assert.h to ensure that _sal_assert is
 * used for all assertion failures.
 */

#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__BORLAND__)
extern void _sal_assert(const char *, const char *, int);
#else
extern void _sal_assert();
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _ASSERT_H */

#undef	assert

#ifdef	NDEBUG

#define	assert(EX) ((void)0)

#else

#if defined(__STDC__)
#define	assert(EX) (void)((EX) || (_sal_assert(#EX, __FILE__, __LINE__), 0))
#else
#define	assert(EX) (void)((EX) || (_sal_assert("EX", __FILE__, __LINE__), 0))
#endif	/* __STDC__ */

#endif	/* NDEBUG */
