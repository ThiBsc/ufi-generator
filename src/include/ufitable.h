#ifndef UFI_TABLE_H
#define UFI_TABLE_H

#include <string>
#include <sstream>
#include <array>
#include <map>

#define NONE -1

// Table 2-2 Convertion type (thib define)
#define TRIVIALLY 							1
#define NUM8_AND_LETTER 					2
#define CONTAIN_NUMPART7_FIRST_LAST_CHAR 	3
#define CONTAIN_NUMPART9_FIRST_SECOND_CHAR	4
#define DEPENDS_ON_THREE_TYPES				5
#define IRELAND_SPECIFIC					6
#define SEQUENCE_OF_FACTORIAL_POW			7

// Table 2-3 Base-31
static const std::array<char, 31> tbase31 = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'M',
	'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y' };

// Table 2-4 reorganisation (real value = value+1)
static const std::array<int, 15> t24 = { 5, 4, 3, 7, 2, 8, 9, 10, 1, 0, 11, 6, 12, 13, 14 };


struct reversed_ufi
{
	bool isVAT;
	std::string vatin;
	int64_t formula;
	std::string to_string() const {
		std::stringstream ss;
		ss << (isVAT ? "VATIN" : "Company key") << ':' << vatin << ", formula:" << formula;
		return ss.str();
	}
};

struct group_and_code
{
	std::string country;
	int country_group_code, nbits, country_code;
	std::string to_string() const {
		std::stringstream ss;
		ss << country << ", group code:" << country_group_code
		<< ", nbits:" << nbits << ", country code:" << country_code;
		return ss.str();
	}
};

struct country_and_pattern
{
	std::string country, pattern;
	int strategy;
	std::string to_string() const {
		std::stringstream ss;
		ss << country << ", pattern:" << pattern;
		return ss.str();
	}
};

typedef std::map<std::string, struct group_and_code> Table_2_1;
typedef std::map<std::string, struct country_and_pattern> Table_2_2;

static const Table_2_1 t21 = {
	{"", {"", 0, NONE, NONE}},

	{"FR", {"France", 1, NONE, NONE}},

	{"GB", {"United Kingdome", 2, NONE, NONE}},

	{"LT", {"Lithuania", 3, 1, 0}},
	{"SE", {"Sweden", 3, 1, 1}},

	{"HR", {"Croatia", 4, 4, 0}},
	{"IT", {"Italy", 4, 4, 1}},
	{"LV", {"Latvia", 4, 4, 2}},
	{"NL", {"The Netherlands", 4, 4, 3}},

	{"BG", {"Bulgaria", 5, 7, 0}},
	{"CZ", {"Czech Rep.", 5, 7, 1}},
	{"IE", {"Ireland", 5, 7, 2}},
	{"ES", {"Spain", 5, 7, 3}},
	{"PL", {"Poland", 5, 7, 4}},
	{"RO", {"Romania", 5, 7, 5}},
	{"SK", {"Slovakia", 5, 7, 6}},
	{"CY", {"Cyprus", 5, 7, 7}},
	{"IS", {"Iceland", 5, 7, 8}},
	{"BE", {"Belgium", 5, 7, 9}},
	{"DE", {"Germany", 5, 7, 10}},
	{"EE", {"Estonia", 5, 7, 11}},
	{"GR", {"Greece", 5, 7, 12}},
	{"NO", {"Norway", 5, 7, 13}},
	{"PT", {"Portugal", 5, 7, 14}},
	{"AT", {"Austria", 5, 7, 15}},
	{"DK", {"Denmark", 5, 7, 16}},
	{"FI", {"Finland", 5, 7, 17}},
	{"HU", {"Hungary", 5, 7, 18}},
	{"LU", {"Luxembourg", 5, 7, 19}},
	{"MT", {"Malta", 5, 7, 20}},
	{"SI", {"Slovenia", 5, 7, 21}},
	{"LI", {"Lichtenstein", 5, 7, 22}},
};

static const Table_2_2 t22 = {
	{"AT", {"Austria", "(AT)U[0-9]{8}", TRIVIALLY}},
	{"BE", {"Belgium", "(BE)0[0-9]{9}", TRIVIALLY}},
	{"BG", {"Bulgaria", "(BG)[0-9]{9,10}", TRIVIALLY}},
	{"CZ", {"Czech Republic", "(CZ)[0-9]{8,10}", TRIVIALLY}},
	{"DE", {"Germany", "(DE)[0-9]{9}", TRIVIALLY}},
	{"DK", {"Denmark", "(DK)[0-9]{8}", TRIVIALLY}},
	{"EE", {"Estonia", "(EE)[0-9]{9}", TRIVIALLY}},
	{"EL", {"Greece", "(EL)[0-9]{9}", TRIVIALLY}},
	{"GR", {"Greece", "(GR)[0-9]{9}", TRIVIALLY}},
	{"FI", {"Finland", "(FI)[0-9]{8}", TRIVIALLY}},
	{"HR", {"Croatia", "(HR)[0-9]{11}", TRIVIALLY}},
	{"HU", {"Hungary", "(HU)[0-9]{8}", TRIVIALLY}},
	{"IT", {"Italy", "(IT)[0-9]{11}", TRIVIALLY}},
	{"LI", {"Liechstenstein", "(LI)[0-9]{5}", TRIVIALLY}},
	{"LT", {"Lithuania", "(LT)([0-9]{9}|[0-9]{12})", TRIVIALLY}},
	{"LU", {"Luxembourg", "(LU)[0-9]{8}", TRIVIALLY}},
	{"LV", {"Latvia", "(LV)[0-9]{11}", TRIVIALLY}},
	{"MT", {"Malta", "(MT)[0-9]{8}", TRIVIALLY}},
	{"NL", {"The Netherlands", "(NL)[0-9]{9}B[0-9]{2}", TRIVIALLY}},
	{"NO", {"Norway", "(NO)[0-9]{9}", TRIVIALLY}},
	{"PL", {"Poland", "(PL)[0-9]{10}", TRIVIALLY}},
	{"PT", {"Portugal", "(PT)[0-9]{9}", TRIVIALLY}},
	{"RO", {"Romania", "(RO)[0-9]{2,10}", TRIVIALLY}},
	{"SE", {"Sweden", "(SE)[0-9]{12}", TRIVIALLY}},
	{"SI", {"Slovenia", "(SI)[0-9]{8}", TRIVIALLY}},
	{"SK", {"Slovakia", "(SK)[0-9]{10}", TRIVIALLY}},
	
	{"CY", {"Cyprus", "(CY)[0-9]{8}[A-Z]", NUM8_AND_LETTER}},
	
	{"ES", {"Spain", "(ES)[0-9A-Z][0-9]{7}[0-9A-Z]", CONTAIN_NUMPART7_FIRST_LAST_CHAR}},
	
	{"FR", {"France", "(FR)[0-9A-Z]{2}[0-9]{9}", CONTAIN_NUMPART9_FIRST_SECOND_CHAR}},
	
	{"GB", {"United Kingdom", "(GB)([0-9]{9}([0-9]{3})?|[A-Z]{2}[0-9]{3})", DEPENDS_ON_THREE_TYPES}},
	
	{"IE", {"Ireland", "(IE)([0-9][A-Z*+][0-9]{5}[A-Z]|[0-9]{7}([A-Z]W?|[A-Z]{2}))", IRELAND_SPECIFIC}},
	
	{"IS", {"Iceland", "(IS)[A-Z0-9]{6}", SEQUENCE_OF_FACTORIAL_POW}},
};

#endif // UFI_TABLE_H
