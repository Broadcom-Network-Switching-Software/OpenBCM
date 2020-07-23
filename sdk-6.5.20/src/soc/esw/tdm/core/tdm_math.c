/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_abs
@param:

Returns absolute value of an integer
 */
int
tdm_abs(int num)
{
	if (num < 0) {
		return (-num);
	}
	else {
		return num;
	}
}


/**
@name: tdm_fac
@param:

Calculates factorial of an integer
 */
int
tdm_fac(int num_signed)
{
	int i, product, num;
	num = tdm_abs(num_signed);
	product=num;
	
	if (num==0) {
		return 1;
	}
	else {
		for (i=(num-1); i>0; i--) {
			product *= (num-i);
		}	
		return product;
	}
}


/**
@name: tdm_pow
@param:

Calculates unsigned power of an integer
 */
int
tdm_pow(int num, int pow)
{
	int i, product=num;
	
	if (pow==0) {
		return 1;
	}
	else {
		for (i=1; i<pow; i++) {
			product *= num;
		}
		return product;
	}
}


/**
@name: tdm_sqrt
@param:

Calculates approximate square root of an integer using Taylor series without float using Bakhshali Approximation
 */
int
tdm_sqrt(int input_signed)
{
	int n, d=0, s=1, approx, input;
	input = tdm_abs(input_signed);
	
	do {
		d=(input-tdm_pow(s,2));
		if (d<0) {
			--s;
			break;
		}
		if ( ((1000*tdm_abs(d))/tdm_pow(s,2)) <= ((1000*tdm_abs(d+1))/tdm_pow((s+1),2)) ) {
			break;
		}
		s++;
	} while(s<input);
	d=(input-tdm_pow(s,2));

	approx=s;
	if (d<(2*s)) {
		for (n=1; n<3; n++) {
			approx+=((tdm_pow(-1,n)*tdm_fac(2*n)*tdm_pow(d,n))/(tdm_pow(tdm_fac(n),2)*tdm_pow(4,n)*tdm_pow(s,((2*n)-1))*(1-2*n)));
		}
	}
	
	return approx;	
}


/**
@name: tdm_PQ
@param:

Customizable partial quotient ceiling function
 */
/* int tdm_PQ(float f) { return (int)((f < 0.0f) ? f - 0.5f : f + 0.5f); }*/
int tdm_PQ(int f)
{
	return ( (int)( ( f+5 )/10) );
}


/**
@name: tdm_math_div_ceil
@param:

Return (int)ceil(a/b)
 */
int tdm_math_div_ceil(int a, int b)
{
	int result=0;
	
	if(a>0 && b>0){
		result = a/b + ((a%b)!=0);
	}
	
	return result;
}


/**
@name: tdm_math_div_floor
@param:

Return (int)floor(a/b)
 */
int tdm_math_div_floor(int a, int b)
{
	int result=0;
	
	if(a>0 && b>0){
		result = a/b;
	}
	
	return result;
}


/**
@name: tdm_math_div_ceil
@param:

Return (int)round(a/b)
 */
int tdm_math_div_round(int a, int b)
{
	int result=0;
	
	if(a>0 && b>0){
		result = (a*10+5)/(b*10);
	}
	
	return result;
}
