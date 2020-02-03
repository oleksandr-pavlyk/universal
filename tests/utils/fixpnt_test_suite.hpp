#pragma once
//  fixpnt_test_suite.hpp : arithmetic/logic test suite for arbitrary fixed point number systems
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <vector>
#include <iostream>
#include <typeinfo>
#include <random>
#include <limits>

// We want the test suite to be used with different configurations of the fixed-point number system
// so the calling environment needs to set the configuration
#include <universal/fixpnt/fixed_point.hpp>
#include "universal/fixpnt/fixpnt_functions.hpp"
// test helpers, such as, ReportTestResults
#include "test_helpers.hpp"

namespace sw {
namespace unum {

#define FIXPNT_TABLE_WIDTH 20

template<size_t nbits, size_t rbits>
void ReportConversionError(std::string test_case, std::string op, double input, double reference, const fixpnt<nbits, rbits>& result) {

	std::cerr << test_case
		<< " " << op << " "
		<< std::setw(FIXPNT_TABLE_WIDTH) << input
		<< " did not convert to "
		<< std::setw(FIXPNT_TABLE_WIDTH) << reference << " instead it yielded "
		<< std::setw(FIXPNT_TABLE_WIDTH) << double(result)
		<< "  raw " << std::setw(nbits) << to_binary(result)
		<< std::endl;
}

template<size_t nbits, size_t rbits>
void ReportConversionSuccess(std::string test_case, std::string op, double input, double reference, const fixpnt<nbits, rbits>& presult) {

}

template<size_t nbits, size_t rbits>
void ReportBinaryArithmeticError(std::string test_case, std::string op, const fixpnt<nbits, rbits>& lhs, const fixpnt<nbits, rbits>& rhs, const fixpnt<nbits, rbits>& ref, const fixpnt<nbits, rbits>& result) {
	auto old_precision = std::cerr.precision();
	std::cerr << test_case << " "
		<< std::setprecision(20)
		<< std::setw(FIXPNT_TABLE_WIDTH) << lhs
		<< " " << op << " "
		<< std::setw(FIXPNT_TABLE_WIDTH) << rhs
		<< " != "
		<< std::setw(FIXPNT_TABLE_WIDTH) << ref << " instead it yielded "
		<< std::setw(FIXPNT_TABLE_WIDTH) << result
		<< " " << to_binary(ref) << " vs " << to_binary(result)
		<< std::setprecision(old_precision)
		<< std::endl;
}

template<size_t nbits, size_t rbits>
void ReportBinaryArithmeticSuccess(std::string test_case, std::string op, const fixpnt<nbits, rbits>& lhs, const fixpnt<nbits, rbits>& rhs, const fixpnt<nbits, rbits>& ref, const fixpnt<nbits, rbits>& result) {
	auto old_precision = std::cerr.precision();
	std::cerr << test_case << " "
		<< std::setprecision(20)
		<< std::setw(FIXPNT_TABLE_WIDTH) << lhs
		<< " " << op << " "
		<< std::setw(FIXPNT_TABLE_WIDTH) << rhs
		<< " == "
		<< std::setw(FIXPNT_TABLE_WIDTH) << ref << " matches reference "
		<< std::setw(FIXPNT_TABLE_WIDTH) << result
		<< " " << to_binary(ref) << " vs " << to_binary(result)
		<< std::setprecision(old_precision)
		<< std::endl;
}

template<size_t nbits, size_t rbits, typename Ty>
void ReportAssignmentError(std::string test_case, std::string op, const fixpnt<nbits, rbits>& ref, const fixpnt < nbits, rbits>& result, const Ty& value) {
	std::cerr << test_case
		<< " " << op << " "
		<< std::setw(FIXPNT_TABLE_WIDTH) << value
		<< " != "
		<< std::setw(FIXPNT_TABLE_WIDTH) << ref << " instead it yielded "
		<< std::setw(FIXPNT_TABLE_WIDTH) << result
		<< " " << to_binary(result) << " vs " << to_binary(ref) << std::endl;
}

template<size_t nbits, size_t rbits, typename Ty>
void ReportAssignmentSuccess(std::string test_case, std::string op, const fixpnt<nbits, rbits>& ref, const fixpnt < nbits, rbits>& result, const Ty& value) {
	std::cerr << test_case
		<< " " << op << " "
		<< std::setw(FIXPNT_TABLE_WIDTH) << value
		<< " == "
		<< std::setw(FIXPNT_TABLE_WIDTH) << result << " reference value is "
		<< std::setw(FIXPNT_TABLE_WIDTH) << ref
		<< "               posit fields " << to_binary(result) << std::endl;
}

/////////////////////////////// VERIFICATION TEST SUITES ////////////////////////////////

template<size_t nbits, size_t rbits>
int Compare(double input, const fixpnt<nbits, rbits>& presult, double reference, bool bReportIndividualTestCases) {
	int fail = 0;
	double result = double(presult);
	if (std::fabs(result - reference) > 0.000000001) {
		fail++;
		if (bReportIndividualTestCases)	ReportConversionError("FAIL", "=", input, reference, presult);
	}
	else {
		// if (bReportIndividualTestCases) ReportConversionSuccess("PASS", "=", input, reference, presult);
	}
	return fail;
}

template<size_t nbits, size_t rbits, typename Ty>
int ValidateAssignment(bool bReportIndividualTestCases) {
	const size_t NR_NUMBERS = (size_t(1) << nbits);
	int nrOfFailedTestCases = 0;

	// use only valid fixed-point values
	// fixpnt_raw -> to value in Ty -> assign to fixpnt -> compare fixpnts
	fixpnt<nbits, rbits> p, assigned;
	for (size_t i = 0; i < NR_NUMBERS; i++) {
		p.set_raw_bits(i); 
		std::cout << to_binary(p) << std::endl;
		Ty value = (Ty)(p);
		assigned = value;
		std::cout << p << " " << value << " " << assigned << std::endl;
		if (p != assigned) {
			nrOfFailedTestCases++;
			if (bReportIndividualTestCases) ReportAssignmentError("FAIL", "=", p, assigned, value);
		}
		else {
			if (bReportIndividualTestCases) ReportAssignmentSuccess("PASS", "=", p, assigned, value);
		}
	}
	return nrOfFailedTestCases;
}

// enumerate all conversion cases for a posit configuration
template<size_t nbits, size_t rbits>
int ValidateConversion(const std::string& tag, bool bReportIndividualTestCases) {
	// we are going to generate a test set that consists of all fixed-point configs and their midpoints
	// we do this by enumerating a fixed-point that is 1-bit larger than the test configuration
	// with the extra bit allocated to the fraction => rbits+1
	// These larger posits will be at the mid-point between the smaller posit sample values
	// and we'll enumerate the exact value, and a perturbation smaller and a perturbation larger
	// to test the rounding logic of the conversion.
	constexpr size_t NR_TEST_CASES = (size_t(1) << (nbits + 1));
	constexpr size_t HALF = (size_t(1) << nbits);
	fixpnt<nbits + 1, rbits + 1> pref, pprev, pnext;

	const unsigned max = nbits > 20 ? 20 : nbits + 1;
	size_t max_tests = (size_t(1) << max);
	if (max_tests < NR_TEST_CASES) {
		std::cout << "ValidateConversion<" << nbits << "," << rbits << ">: NR_TEST_CASES = " << NR_TEST_CASES << " clipped by " << max_tests << std::endl;
	}

	// execute the test
	int nrOfFailedTests = 0;
	double minpos = value_minpos_fixpnt<nbits + 1, rbits + 1>();
	double eps;
	double da, input;
	fixpnt<nbits, rbits> pa;
	for (size_t i = 0; i < NR_TEST_CASES && i < max_tests; ++i) {
		pref.set_raw_bits(i);
		da = double(pref);
		if (i == 0) {
			eps = minpos / 2.0;
		}
		else {
			eps = da > 0 ? da * 1.0e-6 : da * -1.0e-6;
		}
		if (i % 2) {
			if (i == 1) {
				// special case of projecting to +minpos
				// even the -delta goes to +minpos
				input = da - eps;
				pa = input;
				pnext.set_raw_bits(i + 1);
				nrOfFailedTests += Compare(input, pa, (double)pnext, bReportIndividualTestCases);
				input = da + eps;
				pa = input;
				nrOfFailedTests += Compare(input, pa, (double)pnext, bReportIndividualTestCases);

			}
			else if (i == HALF - 1) {
				// special case of projecting to +maxpos
				input = da - eps;
				pa = input;
				pprev.set_raw_bits(HALF - 2);
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
			}
			else if (i == HALF + 1) {
				// special case of projecting to -maxpos
				input = da - eps;
				pa = input;
				pprev.set_raw_bits(HALF + 2);
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
			}
			else if (i == NR_TEST_CASES - 1) {
				// special case of projecting to -minpos
				// even the +delta goes to -minpos
				input = da - eps;
				pa = input;
				pprev.set_raw_bits(i - 1);
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
				input = da + eps;
				pa = input;
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
			}
			else {
				// for odd values, we are between fixed point values, so we create the round-up and round-down cases
				// round-down
				input = da - eps;
				pa = input;
				pprev.set_raw_bits(i - 1);
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
				// round-up
				input = da + eps;
				pa = input;
				pnext.set_raw_bits(i + 1);
				nrOfFailedTests += Compare(input, pa, (double)pnext, bReportIndividualTestCases);
			}
		}
		else {
			// for the even values, we generate the round-to-actual cases
			if (i == 0) {
				// special case of assigning to 0
				input = 0.0;
				pa = input;
				nrOfFailedTests += Compare(input, pa, da, bReportIndividualTestCases);
				// special case of projecting to +minpos
				input = da + eps;
				pa = input;
				pnext.set_raw_bits(i + 2);
				nrOfFailedTests += Compare(input, pa, (double)pnext, bReportIndividualTestCases);
			}
			else if (i == NR_TEST_CASES - 2) {
				// special case of projecting to -minpos
				input = da - eps;
				pa = input;
				pprev.set_raw_bits(NR_TEST_CASES - 2);
				nrOfFailedTests += Compare(input, pa, (double)pprev, bReportIndividualTestCases);
			}
			else {
				// round-up
				input = da - eps;
				pa = input;
				nrOfFailedTests += Compare(input, pa, da, bReportIndividualTestCases);
				// round-down
				input = da + eps;
				pa = input;
				nrOfFailedTests += Compare(input, pa, da, bReportIndividualTestCases);
			}
		}
	}
	return nrOfFailedTests;
}

// enumerate all addition cases for an fixpnt<nbits,rbits> configuration
template<size_t nbits, size_t rbits>
int VerifyAddition(std::string tag, bool bReportIndividualTestCases) {
	constexpr size_t NR_VALUES = (size_t(1) << nbits);
	int nrOfFailedTests = 0;
	fixpnt<nbits, rbits> a, b, result, cref;
	double ref;

	double da, db;
	for (size_t i = 0; i < NR_VALUES; i++) {
		a.set_raw_bits(i);
		da = double(a);
		for (size_t j = 0; j < NR_VALUES; j++) {
			b.set_raw_bits(j);
			db = double(b);
			ref = da + db;
#if FIXPNT_THROW_ARITHMETIC_EXCEPTION
			// catching overflow
			try {
				result = a + b;
			}
			catch (...) {
				if (ref > max_int<nbits>() || iref < min_int<nbits>()) {
					// correctly caught the exception
					continue;
				}
				else {
					nrOfFailedTests++;
				}
			}

#else
			result = a + b;
#endif // FIXPNT_THROW_ARITHMETIC_EXCEPTION
			cref = ref;
			if (result != cref) {
				nrOfFailedTests++;
				if (bReportIndividualTestCases)	ReportBinaryArithmeticError("FAIL", "+", a, b, cref, result);
			}
			else {
				//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "+", a, b, cref, result);
			}
			if (nrOfFailedTests > 100) return nrOfFailedTests;
		}
		if (i % 1024 == 0) std::cout << '.';
	}
	std::cout << std::endl;
	return nrOfFailedTests;
}


// enumerate all subtraction cases for an fixpnt<nbits,rbits> configuration
template<size_t nbits, size_t rbits>
int VerifySubtraction(std::string tag, bool bReportIndividualTestCases) {
	constexpr size_t NR_VALUES = (size_t(1) << nbits);
	int nrOfFailedTests = 0;
	fixpnt<nbits, rbits> a, b, result, cref;
	double ref;

	double da, db;
	for (size_t i = 0; i < NR_VALUES; i++) {
		a.set_raw_bits(i);
		da = double(a);
		for (size_t j = 0; j < NR_VALUES; j++) {
			b.set_raw_bits(j);
			db = double(b);
			ref = da - db;
#if FIXPNT_THROW_ARITHMETIC_EXCEPTION
			// catching overflow
			try {
				result = a - b;
			}
			catch (...) {
				if (ref > max_int<nbits>() || iref < min_int<nbits>()) {
					// correctly caught the exception
					continue;
				}
				else {
					nrOfFailedTests++;
				}
			}

#else
			result = a - b;
#endif // FIXPNT_THROW_ARITHMETIC_EXCEPTION
			cref = ref;
			if (result != cref) {
				nrOfFailedTests++;
				if (bReportIndividualTestCases)	ReportBinaryArithmeticError("FAIL", "-", a, b, cref, result);
			}
			else {
				// if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "-", a, b, cref, result);
			}
			if (nrOfFailedTests > 100) return nrOfFailedTests;
		}
		if (i % 1024 == 0) std::cout << '.';
	}
	std::cout << std::endl;
	return nrOfFailedTests;
}

// enumerate all multiplication cases for an fixpnt<nbits,rbits> configuration
template<size_t nbits, size_t rbits>
int VerifyMultiplication(std::string tag, bool bReportIndividualTestCases) {
	constexpr size_t NR_VALUES = (size_t(1) << nbits);
	int nrOfFailedTests = 0;
	fixpnt<nbits, rbits> a, b, result, cref;
	double ref;

	double da, db;
	for (size_t i = 0; i < NR_VALUES; i++) {
		a.set_raw_bits(i);
		da = double(a);
		for (size_t j = 0; j < NR_VALUES; j++) {
			b.set_raw_bits(j);
			db = double(b);
			ref = da * db;
#if FIXPNT_THROW_ARITHMETIC_EXCEPTION
			// catching overflow
			try {
				result = a * b;
			}
			catch (...) {
				if (ref > max_int<nbits>() || iref < min_int<nbits>()) {
					// correctly caught the exception
					continue;
				}
				else {
					nrOfFailedTests++;
				}
			}

#else
			result = a * b;
#endif // FIXPNT_THROW_ARITHMETIC_EXCEPTION
			cref = ref;
			if (result != cref) {
				nrOfFailedTests++;
				if (bReportIndividualTestCases)	ReportBinaryArithmeticError("FAIL", "*", a, b, cref, result);
			}
			else {
				// if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "*", a, b, cref, result);
			}
			if (nrOfFailedTests > 100) return nrOfFailedTests;
		}
		if (i % 1024 == 0) std::cout << '.';
	}
	std::cout << std::endl;
	return nrOfFailedTests;
}

// enumerate all division cases for an fixpnt<nbits,rbits> configuration
template<size_t nbits, size_t rbits>
int VerifyDivision(std::string tag, bool bReportIndividualTestCases) {
	constexpr size_t NR_VALUES = (size_t(1) << nbits);
	int nrOfFailedTests = 0;
	fixpnt<nbits, rbits> a, b, result, cref;
	double ref;

	double da, db;
	for (size_t i = 0; i < NR_VALUES; i++) {
		a.set_raw_bits(i);
		da = double(a);
		for (size_t j = 0; j < NR_VALUES; j++) {
			b.set_raw_bits(j);
			db = double(b);
			if (j != 0) {
				ref = da / db;
			}
			else {
				ref = 0;
			}
#if FIXPNT_THROW_ARITHMETIC_EXCEPTION
			try {
				result = a / b;
			}
			catch (...) {
				if (ref > max_int<nbits>() || iref < min_int<nbits>()) {
					// correctly caught the exception
					continue;
				}
				else {
					nrOfFailedTests++;
				}
			}

#else
			result = a / b;
#endif // FIXPNT_THROW_ARITHMETIC_EXCEPTION
			cref = ref;
			if (result != cref) {
				nrOfFailedTests++;
				if (bReportIndividualTestCases)	ReportBinaryArithmeticError("FAIL", "/", a, b, cref, result);
			}
			else {
				// if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "/", a, b, cref, result);
			}
			if (nrOfFailedTests > 100) return nrOfFailedTests;
		}
		if (i % 1024 == 0) std::cout << '.';
	}
	std::cout << std::endl;
	return nrOfFailedTests;
}

} // namespace unum
} // namespace sw

