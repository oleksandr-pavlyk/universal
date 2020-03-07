#pragma once
// fixpnt_functions.hpp: information functions for fixed-point types and values
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <cmath> // for std:pow()
#include <universal/bitblock/bitblock.hpp>

namespace sw {
namespace unum {

// functions to provide details about
// the properties of a fixed-point configuration
// in terms of native types.
// Since many fixed-point configurations cannot be represented by native types,
// these are all convenience functions.
// They should not be used for the core algorithms.

// calculate exponential scale of maxpos
template<size_t nbits, size_t rbits, bool arithmetic>
int scale_maxpos_fixpnt() {
	assert(nbits >= rbits);
	return (nbits > rbits) ? (nbits - rbits - 1) : 0;
}

// calculate exponential scale of minpos
template<size_t nbits, size_t rbits, bool arithmetic>
int scale_minpos_fixpnt() {
	return -int(rbits);
}

// calculate the value of maximum positive number
template<size_t nbits, size_t rbits, bool arithmetic>
long double value_maxpos_fixpnt() {
	// 2's complement maxpos value = 2^(nbits-1) - 1
	// fixed-point's shift is 2^rbits
	// maxpos = 2's compl maxpos / shift
	long double numerator = (long double)((1ul << (nbits-1)) - 1);
	long double denominator = (long double)(1ul << (rbits));
	return numerator / denominator;
}

// calculate the value of maximum negative number
template<size_t nbits, size_t rbits, bool arithmetic>
long double value_maxneg_fixpnt() {
	// 2's complement maxneg value = 2^(nbits-1)
	// fixed-point's shift is 2^rbits
	// maxneg = 2's compl maxneg / shift
	long double numerator = (long double)(1ul << (nbits-1));
	long double denominator = (long double)(1ul << (rbits));
	return -numerator / denominator;
}

// calculate the value of minimum positive number
template<size_t nbits, size_t rbits, bool arithmetic>
long double value_minpos_fixpnt() {
	long double denominator = (long double)(1ul << (rbits));
	return 1.0l / denominator;
}

// calculate the value of minimum positive number
template<size_t nbits, size_t rbits, bool arithmetic>
long double value_minneg_fixpnt() {
	long double denominator = (long double)(1ul << (rbits));
	return -1.0l / denominator;
}

// generate the maxneg through maxpos value range of a fixed-point configuration
template<size_t nbits, size_t rbits, bool arithmetic = Modular>
void ReportFixedPointRanges(std::ostream& ostr = std::cout) {
	using namespace std;
	ostr << "fixpnt<" << nbits << ", " << rbits << ", " << (arithmetic ? "Modular" : "Saturating") << "> : "
		<< maxneg_fixpnt<nbits, rbits, arithmetic>() << " "
		<< minneg_fixpnt<nbits, rbits, arithmetic>() << " "
		<< "0 "
		<< minpos_fixpnt<nbits, rbits, arithmetic>() << " "
		<< maxpos_fixpnt<nbits, rbits, arithmetic>()
		<< endl;
}

template<size_t nbits, size_t rbits, bool arithmetic>
inline int sign_value(const fixpnt<nbits, rbits, arithmetic>& p) {
	return (p < 0) ? -1 : 1;
}

template<size_t nbits, size_t rbits, bool arithmetic>
inline long double fraction_value(const fixpnt<nbits, rbits, arithmetic>& p) {
	return 0.0l;
}

// get the sign of the fixed-point
template<size_t nbits, size_t rbits, bool arithmetic>
inline bool sign(const fixpnt<nbits, rbits, arithmetic>& p) {
	return (p < 0) ? true : false;
}

// get the fraction bits of a posit
template<size_t nbits, size_t rbits, bool arithmetic>
inline bitblock<rbits> extract_fraction(const fixpnt<nbits, rbits, arithmetic>& p) {
	bitblock<rbits> fraction;
	for (int i = 0; i < rbits; ++i) {
	fraction[i] = p[i];
	}
	return fraction;
}

} // namespace unum
} // namespace sw