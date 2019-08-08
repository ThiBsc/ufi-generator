#ifndef UFI_NUMBER_H
#define UFI_NUMBER_H

#include <iostream>
#include <string>
#include <cmath>
#include <regex>
#include <bitset>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <cstdint>
#include <stdlib.h>
#include <assert.h>
#include "ufitable.h"
#include "ufiexception.h"
#include "../biginteger/src/header/biginteger.h"

#define ENTIRE_MATCH 	0
#define UFI_SIZE		16
// PAYLOAD = VATIN + FORMULA + VERSION
#define VATIN_SIZE 		45
#define FORMULA_SIZE 	28
#define VERSION_SIZE 	1
#define PAYLOAD_SIZE 	VATIN_SIZE+FORMULA_SIZE+VERSION_SIZE

#define COUNTRY_GCODE_SIZE 4

typedef std::bitset<PAYLOAD_SIZE> bspayload;

class UfiNumber
{
public:
	UfiNumber(std::string vatin, int64_t formula);
	virtual ~UfiNumber();
	std::string generate() throw(UfiException);
    void validate(std::string nufi, bool bypassVersionNumber = false) throw(UfiException);
	static std::string version;

private:
	int64_t get_numerical_value(std::string isolang);
	BigInteger step1();
	std::vector<int> step2(BigInteger payload);
	std::vector<int> step3(std::vector<int> base31);
	char step4(std::vector<int> reorg_base31);
	std::string vatin;
	int64_t formula;
	unsigned short version_number;
	
};

#endif // UFI_NUMBER_H
