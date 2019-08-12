#include "include/ufinumber.h"

std::string UfiNumber::version = "0";

UfiNumber::UfiNumber()
	: vatin("")
	, formula(-1)
{

}

UfiNumber::UfiNumber(std::string vatin, int64_t formula)
	: vatin(vatin)
	, formula(formula)
{

}

UfiNumber::~UfiNumber()
{

}

std::string UfiNumber::generate() throw(UfiException)
{
	std::stringstream ufi;
	if (0 <= formula && formula <= 268435455){
		BigInteger payload = step1();
		version_number = payload.toString(2).back()-'0';
		
		std::cout << payload.toString() << std::endl;
		std::vector<int> base31 = step2(payload);
		std::vector<int> reorg_base31 = step3(base31);
		char checksum = step4(reorg_base31);

		ufi << checksum;
		int index = 1;
		for (const int& i : reorg_base31){
			if (index%4 == 0){
				ufi << '-';
			}
			ufi << tbase31[i];
			index++;
		}
	} else {
		throw UfiException(UfiException::CREATION, UFI001, "Invalid or out of bound formulation number");
	}
	return ufi.str();
}

std::string UfiNumber::generate(std::string vatin, int64_t formula) throw(UfiException)
{
	this->vatin = vatin;
	this->formula = formula;
	return generate();
}

void UfiNumber::validate(std::string nufi, bool bypassVersionNumber) throw(UfiException)
{
	nufi.erase(std::remove_if(nufi.begin(), nufi.end(),
		  [](const char& c){ return c=='-' || c==' '; }),
		  nufi.end());
	if (nufi.size() == 16){
		bool bAllGoodChar = true;
		for (const char& c : nufi){
			auto result = std::find(tbase31.begin(), tbase31.end(), c);
			bAllGoodChar &= (result != tbase31.end());
		}
		if (bAllGoodChar){
			int checksum = 0;
			int multiplicator = 2;
			auto itfirst = std::find(tbase31.begin(), tbase31.end(), nufi.front());
			checksum += (itfirst - tbase31.begin());
			std::string check = nufi.substr(1);
			for (const char& c : check){
				auto it = std::find(tbase31.begin(), tbase31.end(), c);
				checksum += (multiplicator * (it - tbase31.begin()));
				multiplicator++;
			}
			if (checksum % 31 == 0){
				// The right-most bit of the version must be 0
                if (bypassVersionNumber || version_number == 0){
					// validate
				} else {
					throw UfiException(UfiException::VALIDATION, VAL005, "Its internal version number is not correct");
				}
			} else {
				throw UfiException(UfiException::VALIDATION, VAL003, "You may have inverted characters or typed an incorrect character");
			}
		} else {
			throw UfiException(UfiException::VALIDATION, VAL002, "It contains at least one invalid character");
		}		
	} else {
		throw UfiException(UfiException::VALIDATION, VAL001, "It does not contain 16 characters");
	}
}

struct reversed_ufi UfiNumber::decode(std::string nufi)
{
	struct reversed_ufi ret;
	bool canDecode = true;
	try {
		validate(nufi, true);
	} catch(UfiException const& e){
		canDecode = false;
		ret.vatin = "";
		ret.formula = -1;
		std::cerr << e.what() << '\n' << "Unable to decode an invalid UFI" << std::endl;
	}

	if (canDecode){
		// Clean the ufi
		nufi.erase(std::remove(nufi.begin(), nufi.end(), '-'), nufi.end());

		// Reverse step 4
		nufi.erase(0, 1);

		// Reverse step 3
		std::vector<int> rev_chars(UFI_SIZE-1, 0);
		int i = 0;
		for (const char &c : nufi){
			auto it = std::find(tbase31.begin(), tbase31.end(), c);
			rev_chars[t24.at(i)] = std::distance(tbase31.begin(), it);
			i++;
		}

		// Reverse step 2
		BigInteger bi = BigInteger::ZERO;
		int pow = nufi.size()-1;
		for (const int& i : rev_chars){
			//std::cout << BigInteger(i) << " * " << 31 << "^" << pow << std::endl;
			bi += BigInteger(i) * BigInteger(31).pow(pow);
			pow--;
		}

		// Reverse step 1
		std::string bi_binary = bi.toString(2);
		// Left-pad '0' if less than payload_size
		while (bi_binary.length() < PAYLOAD_SIZE){
			bi_binary.insert(0, "0");
		}
		
		std::string vat_str;
		int64_t formulation = std::atoi(BigInteger(bi_binary.substr(0, FORMULA_SIZE), 2).toString().c_str());
		int country_group = std::atoi(BigInteger(bi_binary.substr(FORMULA_SIZE, COUNTRY_GCODE_SIZE), 2).toString().c_str());
		auto it_nOfBits = std::find_if(t21.begin(), t21.end(), [&country_group](std::pair<std::string, struct group_and_code> const& pgc) -> bool {
			return pgc.second.country_group_code == country_group;
		});
		int nOfBits = (it_nOfBits != t21.end() ? it_nOfBits->second.nbits : 0 );
		int country_code = std::atoi(BigInteger(bi_binary.substr(FORMULA_SIZE+COUNTRY_GCODE_SIZE, nOfBits), 2).toString().c_str());
		// when nOfBits = -1, (FR, GB)
		if (nOfBits == NONE){
			nOfBits = 0;
			country_code = NONE;
		}
		auto it_iso = std::find_if(t21.begin(), t21.end(), [&country_group, &country_code](std::pair<std::string, struct group_and_code> const& pgc) -> bool {
			return pgc.second.country_group_code == country_group && pgc.second.country_code == country_code;
		});
		std::string iso = (it_iso != t21.end() ? it_iso->first : "" );
		
		//std::cout << formulation << " " << country_group << std::endl;
		int startBit = FORMULA_SIZE+COUNTRY_GCODE_SIZE+nOfBits;
		int endBit = PAYLOAD_SIZE - startBit;
		int64_t vat_number = std::atoll(BigInteger(bi_binary.substr(startBit, endBit-1), 2).toString().c_str());
		//unsigned short version = bi.toString(2).back() - '0';

		if (country_group == 0){
			// UFI with company key
			vat_str = std::to_string(vat_number);
		} else {
			// UFI with VATIN
			// Reverse the "STRATEGY"
			struct country_and_pattern cap = t22.at(iso);
			std::cout << cap.to_string() << std::endl;
			if (cap.strategy == TRIVIALLY){
				vat_str = std::to_string(vat_number);
				if (!iso.compare("AT")){
					vat_str.insert(0, "U");
				} else if (!iso.compare("BE")){
					vat_str.insert(0, "0");
				} else if (!iso.compare("NL")){
					vat_str.insert(9, "B");
				}
			} else if (cap.strategy == NUM8_AND_LETTER){
				std::string vat_number_str = std::to_string(vat_number);
				int len = vat_number_str.length();
				char letter = std::atoi(vat_number_str.substr(0, len-8).c_str())+'A';
				vat_str.append(vat_number_str.substr(len-8));
				vat_str.push_back(letter);
			} else if (cap.strategy == CONTAIN_NUMPART7_FIRST_LAST_CHAR){
				std::string vat_number_str = std::to_string(vat_number);
				int len = vat_number_str.length();
				int firstPart = std::atoi(vat_number_str.substr(0, len-7).c_str());
				// Brut force the possibility to find c1 and c2
				int c1 = -1, c2 = -1, i=0, j=0;
				while ( c1==-1 && i<36){
					while (c2==-1 && j<36){
						if ((36*i + j) == firstPart){
							c1 = i;
							c2 = j;
						}
						j++;
					}
					j = 0;
					i++;
				}
				vat_str.push_back(c1<10 ? c1+'0' : (c1-10)+'A');
				vat_str.append(vat_number_str.substr(len-7));
				vat_str.push_back(c2<10 ? c2+'0' : (c2-10)+'A');
			} else if (cap.strategy == CONTAIN_NUMPART9_FIRST_SECOND_CHAR){
				std::string vat_number_str = std::to_string(vat_number);
				int len = vat_number_str.length();
				int firstPart = std::atoi(vat_number_str.substr(0, len-9).c_str());
				// Brut force the possibility to find c1 and c2
				int c1 = -1, c2 = -1, i=0, j=0;
				while ( c1==-1 && i<36){
					while (c2==-1 && j<36){
						if ((36*i + j) == firstPart){
							c1 = i;
							c2 = j;
						}
						j++;
					}
					j = 0;
					i++;
				}
				vat_str.push_back(c1<10 ? c1+'0' : (c1-10)+'A');
				vat_str.push_back(c2<10 ? c2+'0' : (c2-10)+'A');
				vat_str.append(vat_number_str.substr(len-9));
			} else if (cap.strategy == DEPENDS_ON_THREE_TYPES){
				long long d = vat_number - std::pow(2, 40);
				if (d > 0){
					// For matching [0-9]{9}([0-9]{3})?
					vat_str.append(std::to_string(d));
				} else {
					// For matching [A-Z]{2}[0-9]{3}
					std::string vat_number_str = std::to_string(vat_number);
					int len = vat_number_str.length();
					int firstPart = std::atoi(vat_number_str.substr(0, len-3).c_str());
					// Brut force the possibility to find c1 and c2
					int c1 = -1, c2 = -1, i=0, j=0;
					while ( c1==-1 && i<26){
						while (c2==-1 && j<26){
							if ((26*i + j) == firstPart){
								c1 = i;
								c2 = j;
							}
							j++;
						}
						j = 0;
						i++;
					}
					vat_str.push_back(char(c1+'A'));
					vat_str.push_back(char(c2+'A'));
					vat_str.append(vat_number_str.substr(len-3));
				}
			} else if (cap.strategy == IRELAND_SPECIFIC){
				std::string vat_number_str = std::to_string(vat_number);
				int len = vat_number_str.length();
				if ( vat_number_str.length() > 9 ){
					// For matching [0-9]{7}[A-Z]W? or [0-9]{7}[A-Z]{2}
					BigInteger numerical(vat_number_str);
					numerical -= BigInteger(2).pow(33);
					len = numerical.toString().length();
					int firstPart = std::atoi(numerical.toString().substr(0, len-7).c_str());
					// Brut force the possibility to find c1 and c2
					int c1 = -1, c2 = -1, i=0, j=0;
					while ( c1==-1 && i<26){
						while (c2==-1 && j<26){
							if ((26*j + i) == firstPart){
								c1 = i;
								c2 = j;
							}
							j++;
						}
						j = 0;
						i++;
					}
					vat_str.append(numerical.toString().substr(len-7));
					vat_str.push_back(char(c1+'A'));
					if (c2 != 0){
						vat_str.push_back(char(c2+'A'));
					}
				} else {
					// For matching [0-9][A-Z*+][0-9]{5}[A-Z]
					int firstPart = std::atoi(vat_number_str.substr(0, len-6).c_str());
					//int secondPart = std::atoi(vat_number_str.substr(3).c_str());
					// Brut force the possibility to find c1 and c2
					int c1 = -1, c2 = -1, i=0, j=0;
					while ( c1==-1 && i<28){
						while (c2==-1 && j<28){
							if ((26*i + j) == firstPart){
								c1 = i;
								c2 = j;
							}
							j++;
						}
						j = 0;
						i++;
					}				
					//std::cout << firstPart << " / " << secondPart << std::endl;
					vat_str.push_back(vat_number_str.at(3));
					vat_str.push_back(c1<26 ? char(c1+'A') : (c1==26 ? '+' : '*'));
					vat_str.append(vat_number_str.substr(4));
					vat_str.push_back(c2<26 ? char(c2+'A') : (c2==26 ? '+' : '*'));
				}
			} else if (cap.strategy == SEQUENCE_OF_FACTORIAL_POW){
				while (vat_number != 0){
					int remain = vat_number % 36;
					vat_number = vat_number / 36;
					char c = (remain<10 ? remain+'0' : (remain-10)+'A');
					vat_str.push_back(c);
				}
				std::reverse(vat_str.begin(), vat_str.end());
			}
		}
		
		ret.isVAT = (country_group != 0);
		ret.vatin = (iso+vat_str);
		ret.formula = formulation;
		//std::cout << iso << vat_str << std::endl;
		//std::cout << formulation << std::endl;
	}
	return ret;
}

/**
 *	struct country_and_pattern
 *	{
 *		std::string country, pattern;
 *		int strategy;
 *	};
 */
int64_t UfiNumber::get_numerical_value(std::string isolang)
{
	int64_t value = 0;
	try {
		const struct country_and_pattern cap = t22.at(isolang);
		std::string pattern = cap.pattern;
		int strategy = cap.strategy;
		std::cout << cap.to_string() << std::endl;
			
		std::regex re_valid(pattern);
		if (std::regex_match(vatin, re_valid)){
			if (strategy == TRIVIALLY){
				// V = numerical part
				std::string vatin_tmp = vatin;
				std::smatch match_d;
				std::regex re_digit("[0-9]+");
				std::regex_search(vatin_tmp, match_d, re_digit);
				std::string str_value = match_d.str();
				if (!isolang.compare("NL")){
					vatin_tmp = match_d.suffix();
					std::regex_search(vatin_tmp, match_d, re_digit);
					str_value.append(match_d.str());
				}
				value = std::stoll(str_value.c_str());
			} else if (strategy == NUM8_AND_LETTER){
				// V = l . 10^8 + d
				int l = 0, d = 0;
				std::smatch match_d;
				std::regex re_digit("[0-9]+");
				std::regex_search(vatin, match_d, re_digit);
				d = std::stoi(match_d.str());
				l = vatin.back() - 'A';
				value = l * std::pow(10, 8) + d;
			} else if (strategy == CONTAIN_NUMPART7_FIRST_LAST_CHAR){
				// V = (36 . c1 + c2) . 10^7 + d
				int c1 = 0, c2 = 0, d = 0;
				std::string without_iso = vatin.substr(2);
				int len = without_iso.size();
				std::string numerical_part = without_iso.substr(1, len-2);
				d = std::stoi(numerical_part);
				try {
					// first char
					c1 = std::stoi(without_iso.substr(0, 1));
				} catch (std::invalid_argument const& e){
					c1 = (without_iso.front() - 'A') + 10;
				}
				try {
					// last char
					c2 = std::stoi(without_iso.substr(len-1));
				} catch (std::invalid_argument const& e){
					c2 = (without_iso.back() - 'A') + 10;
				}
				value = (36 * c1 + c2) * std::pow(10, 7) + d;
			} else if (strategy == CONTAIN_NUMPART9_FIRST_SECOND_CHAR){
				// V = (36 . c1 + c2) . 10^9 + d
				int c1 = 0, c2 = 0, d = 0;
				std::string without_iso = vatin.substr(2);
				std::string numerical_part = without_iso.substr(2);
				d = std::stoi(numerical_part);
				try {
					// first char
					c1 = std::stoi(without_iso.substr(0, 1));
				} catch (std::invalid_argument const& e){
					c1 = (without_iso.front() - 'A') + 10;
				}
				try {
					// second char
					c2 = std::stoi(without_iso.substr(1, 1));
				} catch (std::invalid_argument const& e){
					c2 = (without_iso.at(1) - 'A') + 10;
				}
				//std::cout << d << " " << c1 << " " << c2 << std::endl;
				value = (36 * c1 + c2) * std::pow(10, 9) + d;
			} else if (strategy == DEPENDS_ON_THREE_TYPES){
				unsigned long d = 0;
				std::string without_iso = vatin.substr(2);
				try {
					// first/second (9/12 digits) case
					// V = 2^40 + d
					d = std::stol(without_iso);
					value = std::pow(2, 40) + d;
				} catch (std::invalid_argument const& e){
					// third case
					// V = (26 . l1 + l2) . 10^3 + d
					int l1 = 0, l2 = 0;
					d = std::stoi(without_iso.substr(2));
					l1 = without_iso.front() - 'A';
					l2 = without_iso.at(1) - 'A';
					value = (26 * l1 + l2) * std::pow(10, 3) + d;
				}
			} else if (strategy == IRELAND_SPECIFIC){
				int c1 = 0, c2 = 0, d = 0;
				re_valid = std::regex("(IE)[0-9][A-Z*+][0-9]{5}[A-Z]", re_valid.flags());
				std::stringstream ss;
				if (std::regex_match(vatin, re_valid)){
					// V = (26 . c1 + c2) . 10^6 + d
					std::regex re_digit("[0-9]+");
					auto d_begin = std::sregex_iterator(vatin.begin(), vatin.end(), re_digit);
					auto d_end = std::sregex_iterator();
					for (std::sregex_iterator i = d_begin; i != d_end; ++i) {
						std::smatch match = *i;                                    
						ss << match.str();
					}
					d = std::stoi(ss.str());
					std::vector<char> letters;
					std::regex re_letter("[A-Z*+]");
					auto l_begin = std::sregex_iterator(vatin.begin(), vatin.end(), re_letter);
					auto l_end = std::sregex_iterator();
					for (std::sregex_iterator i = l_begin; i != l_end; ++i) {
						std::smatch match = *i;                                    
						letters.push_back(match.str()[0]);
					}
					// remove iso lang
					letters.erase(letters.begin(), letters.begin()+2);
					char first = letters.front();
					char last = letters.back();
					if (first == '+' || first == '*'){
						c1 = (first == '+' ? 26 : 27);
					} else {
						c1 = first - 'A';
					}
					if (last == '+' || last == '*'){
						c2 = (last == '+' ? 26 : 27);
					} else {
						c2 = last - 'A';
					}
					value = (26 * c1 + c2) * std::pow(10,6) + d;
				} else {
					// V = 2^33 + (26 * c2 + c1) . 10^7 + d
					std::smatch match_d;
					std::regex re_digit("[0-9]+");
					std::regex_search(vatin, match_d, re_digit);
					d = std::stoi(match_d.str());
					std::string without_iso = vatin.substr(2);
					c1 = without_iso.at(7) - 'A';
					try {
						c2 = without_iso.at(8) - 'A';
					} catch (std::out_of_range const& e){
						c2 = 0;
					}		
					value = std::pow(2, 33) + ((26 * c2 + c1) * std::pow(10,7) + d);
				}
			} else if (strategy == SEQUENCE_OF_FACTORIAL_POW){
				// V = 36^5 . c5 + 36^4 . c4 + ... + 36 . c1 + c0
				std::string without_iso = vatin.substr(2);
				int len = without_iso.size();
				int pow = 0;
				for (int i=len-1; i>=0; i--){
					int d = 0;
					try {
						d = std::stoi(without_iso.substr(i, 1));
					} catch (std::invalid_argument const& e){
						d = (without_iso.at(i) - 'A') + 10;
					}
					value += (pow == 0 ? d : std::pow(36, pow) * d);
					pow++;
				}
			} else {
				// unknown strategy
			}
		} else {
			// Not a valit VAT for country
			throw UfiException(UfiException::CREATION, UFI003, "Not a valit VAT for country");
		}		
	} catch (const std::out_of_range& e){
		// iso country code does not exist
		std::cerr << e.what() << std::endl;
	}
	return value;
}

/**
 *	struct group_and_code
 * 	{
 *		std::string country;
 *		int country_group_code, nbits, country_code;
 *	};
 */
BigInteger UfiNumber::step1()
{
	bspayload bs_payload(0);
	std::smatch match_isoccode;
	std::regex re_isoccode("^[A-Z]{2}");
	if (std::regex_search(vatin, match_isoccode, re_isoccode)){
		std::string iso_country_code = match_isoccode.str(ENTIRE_MATCH);
		try {
			const struct group_and_code gac = t21.at(iso_country_code);
			int cgcode = gac.country_group_code;
			int nbits = gac.nbits;
			int ccode = gac.country_code;
		
			int idx = PAYLOAD_SIZE-1;
			// RED part on the doc
			for (int i=FORMULA_SIZE-1; i>=0; i--){
				if ( (formula & (1 << i)) ){
					bs_payload.set(idx);
				}
				idx--;
			}
			// BLUE part on the doc
			for (int i=COUNTRY_GCODE_SIZE-1; i>=0; i--){
				if ( (cgcode & (1 << i)) ){
					bs_payload.set(idx);
				}
				idx--;
			}
			// YELLOW part on the doc
			for (int i=nbits-1; i>=0; i--){
				if ( (ccode & (1 << i)) ){
					bs_payload.set(idx);
				}
				idx--;
			}
			// GREEN part on the doc
			int64_t numerical_value = get_numerical_value(iso_country_code);
			bs_payload |= bspayload((numerical_value << 1));
			
			std::cout << gac.to_string() << std::endl;
		} catch (const std::out_of_range& e){
			// iso country code does not exist
			throw UfiException(UfiException::CREATION, UFI002, "Country code does not exist");
		}
	} else {
		// vatin is a company key
		// RED part on the doc
		std::string red = std::bitset<28>(formula).to_string();
		//std::cout << red << std::endl;
		// BLUE part on the doc
		std::string blue = std::bitset<4>(0).to_string();
		//std::cout << blue << std::endl;
		// GREEN part on the doc
		std::string green = std::bitset<41>(strtoll(vatin.c_str(), NULL, 10)).to_string();
		//std::cout << green << std::endl;
		bs_payload = bspayload(red+blue+green+version);
		std::cout << bs_payload << std::endl;
	}
	return BigInteger(bs_payload.to_string(), 2);
}

std::vector<int> UfiNumber::step2(BigInteger payload)
{
	BigInteger modulo = 31;
	std::vector<int> base31;
	while (!(payload == BigInteger::ZERO)){
		BigInteger mod = payload.modulus(modulo);
		payload = payload.divide(modulo);
		base31.emplace_back(std::atoi(mod.toString().c_str()));
	}
	// Left-pad if less than 15 number
	for (int i=base31.size(); i<15; ++i){
		base31.emplace_back(0);
	}
	std::reverse(base31.begin(), base31.end());
	return base31;
}

std::vector<int> UfiNumber::step3(std::vector<int> base31)
{
	std::vector<int> reorg_base31;
	int index = 0;
	while (index < UFI_SIZE-1){
		reorg_base31.push_back(base31.at(t24.at(index++)));
	}
	return reorg_base31;
}

char UfiNumber::step4(std::vector<int> reorg_base31)
{
	int checksum = 0;
	int multiplicator = 2;
	for (const int& i : reorg_base31){
		checksum += (multiplicator * i);
		multiplicator++;
	}
	//std::cout << checksum << std::endl;
	int checksum_value = (31 - (checksum % 31)) % 31;
	char checksum_char = tbase31.at(checksum_value);
	assert((checksum + checksum_value) % 31 == 0);
	return checksum_char;
}

