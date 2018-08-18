#include <string.h>
#include <stdio.h>
#include "CuTest.h"

int readHash(char *hashes, int startOfNextHash);

void TestReadHash_normalHash(CuTest *tc){
	printf("TestReadHash_normalHash, input: \"normalHash-\"\noutput: ");
	char *input = "normalHash-";
	int expected = strlen(input) - 1;
	int actual = readHash(input, 0);
	CuAssertIntEquals(tc, expected, actual);
}

void TestReadHash_emptyHash(CuTest *tc){
	printf("TestReadHash_emptyHash, input: \"\"\noutput: ");
	char *input = "";
	int expected = strlen(input);
	int actual = readHash(input, 0);
	CuAssertIntEquals(tc, expected, actual);
}

void TestReadHash_hashWithDash(CuTest *tc){
	printf("TestReadHash_hashWithDash, input: \"normal-hash\"\noutput: ");
	char *input = "normal-hash";
	int expected = strlen("normal");
	int actual = readHash(input, 0);
	CuAssertIntEquals(tc, expected, actual);
}

CuSuite* viewerGetSuite(){
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestReadHash_normalHash);
	SUITE_ADD_TEST(suite, TestReadHash_emptyHash);
	SUITE_ADD_TEST(suite, TestReadHash_hashWithDash);
	return suite;
}