// division.cpp: functional tests for arbitrary configuration fixed-point division
//
// Copyright (C) 2017-2020 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

// Configure the fixpnt template environment
// first: enable general or specialized fixed-point configurations
#define FIXPNT_FAST_SPECIALIZATION
// second: enable/disable fixpnt arithmetic exceptions
#define FIXPNT_THROW_ARITHMETIC_EXCEPTION 1

// minimum set of include files to reflect source code dependencies
#include <universal/fixpnt/fixed_point.hpp>
// fixed-point type manipulators such as pretty printers
#include <universal/fixpnt/fixpnt_manipulators.hpp>
#include <universal/fixpnt/fixpnt_functions.hpp>
#include "../utils/fixpnt_test_suite.hpp"

// generate specific test case that you can trace with the trace conditions in fixed_point.hpp
// for most bugs they are traceable with _trace_conversion and _trace_add
template<size_t nbits, size_t rbits, typename Ty>
void GenerateTestCase(Ty _a, Ty _b) {
	Ty ref;
	sw::unum::fixpnt<nbits, rbits> a, b, cref, result;
	a = _a;
	b = _b;
	result = a / b;
	ref = _a / _b;
	cref = ref;
	std::streamsize oldPrecision = std::cout.precision();
	std::cout << std::setprecision(nbits - 2);
	std::cout << std::setw(nbits) << _a << " / " << std::setw(nbits) << _b << " = " << std::setw(nbits) << ref << std::endl;
	std::cout << a << " / " << b << " = " << result << " (reference: " << cref << ")   " ;
	std::cout << (cref == result ? "PASS" : "FAIL") << std::endl << std::endl;
	std::cout << std::dec << std::setprecision(oldPrecision);
}

// conditional compile flags
#define MANUAL_TESTING 1
#define STRESS_TESTING 0

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace sw::unum;

	int nrOfFailedTestCases = 0;

	std::string tag = "modular division: ";

#if MANUAL_TESTING

	constexpr size_t nbits = 8;
	constexpr size_t rbits = 4;

	/*
	fixpnt<nbits, rbits> a, b, c;
	a = 6.0f;
	b = 3*0.0625f;
	for (int i = 0; i < nbits; ++i) {
		blockbinary<16> raw = urdiv(a.getbb(), b.getbb(), i);
		cout << to_binary(a) << " / " << to_binary(b) << " = " << to_binary(raw) << " msb of result is: " << raw.msb() << endl;
	}
	c = a / b;
	*/

	// generate individual testcases to hand trace/debug
	GenerateTestCase<4, 1>(1.0f, 1.0f);
	GenerateTestCase<4, 1>(1.0f, 2.0f);
	GenerateTestCase<4, 1>(2.0f, 3.0f);
	
	GenerateTestCase<8, 4>(1.0f, 1.0f);
	GenerateTestCase<8, 4>(1.0f, 2.0f);
	GenerateTestCase<8, 4>(1.0f, 0.5f);
	GenerateTestCase<8, 4>(1.0f, 4.0f);
	GenerateTestCase<8, 4>(1.0f, 0.25f);

	/*
	b0001 / b1000 = b1111'1000 rounding bits b00'1000 rounded b0000'1000
	FAIL                  0.5 / -4.0 != -4.0 golden reference is                  0.0 b100.0 vs b000.0b000.1 / b100.1
	b0001 / b1001 = b1111'0111 rounding bits b00'1001 rounded b0000'1000
	FAIL                  0.5 / -3.5 != -4.0 golden reference is                  0.0 b100.0 vs b000.0b000.1 / b101.0
	b0001 / b1010 = b1111'0110 rounding bits b00'1010 rounded b0000'1000
	FAIL                  0.5 / -3.0 != -4.0 golden reference is                  0.0 b100.0 vs b000.0b000.1 / b101.1
	b0001 / b1011 = b1111'0100 rounding bits b00'1100 rounded b0000'1000
	FAIL                  0.5 / -2.5 != -4.0 golden reference is                  0.0 b100.0 vs b000.0b000.1 / b110.0
	b0001 / b1100 = b1111'0000 rounding bits b01'0000 rounded b0000'1000
	FAIL                  0.5 / -2.0 != -4.0 golden reference is                  0.0 b100.0 vs b000.0b000.1 / b110.1
	b0001 / b1101 = b1110'1011 rounding bits b01'0101 rounded b0000'0111
	FAIL                  0.5 / -1.5 != 3.5 golden reference is - 0.5 b011.1 vs b111.1b000.1 / b111.0
	b0001 / b1110 = b1110'0000 rounding bits b10'0000 rounded b0000'0111
	FAIL                  0.5 / -1.0 != 3.5 golden reference is - 0.5 b011.1 vs b111.1b000.1 / b111.1
	b0001 / b1111 = b1100'0000 rounding bits b00'0000 rounded b0000'0110
	FAIL                  0.5 / -0.5 != 3.0 golden reference is - 1.0 b011.0 vs b111.0b001.0 / b000.1
	*/
//	GenerateTestCase<4, 1>(0.5f, -4.0f);
	GenerateTestCase<4, 1>(0.5f,  3.5f);
	GenerateTestCase<4, 1>(0.5f, -3.5f);
//	GenerateTestCase<4, 1>(0.5f, -3.0f);
//	GenerateTestCase<4, 1>(0.5f, -2.5f);
//	GenerateTestCase<4, 1>(0.5f, -2.0f);
//	GenerateTestCase<4, 1>(0.5f, -1.5f);
//	GenerateTestCase<4, 1>(0.5f, -1.0f);
//	GenerateTestCase<4, 1>(0.5f, -0.5f);

	return 0;

	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 0, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,0,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 1, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,1,Modular,uint8_t>", "division");
	//	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 4, Modular, uint8_t>("Manual Testing", true), "fixpnt<8,4,Modular,uint8_t>", "division");


#if STRESS_TESTING

	// manual exhaustive test
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 0, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,0,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 1, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,1,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 2, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,2,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 3, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,3,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<4, 4, Modular, uint8_t>("Manual Testing", true), "fixpnt<4,4,Modular,uint8_t>", "division");

#endif

	nrOfFailedTestCases = 0; // ignore any failures in MANUAL mode
#else
	bool bReportIndividualTestCases = false;

	cout << "Fixed-point modular division validation" << endl;

	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 0, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,0,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 1, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,1,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 2, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,2,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 3, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,3,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 4, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,4,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 5, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,5,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 6, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,6,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 7, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,7,Modular,uint8_t>", "division");
	nrOfFailedTestCases += ReportTestResult(VerifyDivision<8, 8, Modular, uint8_t>(tag, bReportIndividualTestCases), "fixpnt<8,8,Modular,uint8_t>", "division");

#if STRESS_TESTING

#endif  // STRESS_TESTING

#endif  // MANUAL_TESTING

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::unum::fixpnt_arithmetic_exception& err) {
	std::cerr << "Uncaught fixpnt arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const sw::unum::fixpnt_internal_exception& err) {
	std::cerr << "Uncaught fixpnt internal exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::runtime_error& err) {
	std::cerr << "Uncaught runtime exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}