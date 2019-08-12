#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <fstream>

#include "include/ufinumber.h"

const std::string progname = "nufi";

void print_help();

int main(int argc, char *argv[]) {

	int help = 0;
	std::string vatin, ufi_str;
	int64_t formula = -1;

	int c, idxopt;

	struct option t_opt[] =
	{
		{"help",			no_argument,			&help,		1},
		{"vatin",			required_argument,		NULL,		'v'},
		{"formula",			required_argument,		NULL,		'f'},
		{"decode",			required_argument,		NULL,		'd'},
        {0,         		0,              		NULL,		0}
	};
	
	while ((c = getopt_long(argc, argv, "hv:f:d:", t_opt, &idxopt)) != -1){
		switch (c){
			case 'v':
				vatin = optarg;
				break;
			case 'f':
				formula = strtoll(optarg, NULL, 10);
				break;
			case 'd':
				ufi_str = optarg;
				break;
			case 'h':
				help = 1;
				break;
			default:
				break;
		}
  	}
	if (help){
		print_help();
	} else {
		UfiNumber nufi;
		// Encode
		if (!vatin.empty() && formula != -1){
			try {
				std::string snufi = nufi.generate(vatin, formula);
				std::cout << "UFI: " << snufi << std::endl;
				nufi.validate(snufi);
			} catch(UfiException const& e){
				std::cerr << e.what() << std::endl;
			}
		}
		// Decode
		if (!ufi_str.empty()){
			struct reversed_ufi ru = nufi.decode(ufi_str);
			std::cout << ru.to_string() << std::endl;
		}
		// If not encode and not decode... print help
		if ( (vatin.empty() && formula == -1) && ufi_str.empty() ){
			print_help();
		}
	}
	
	return 0;
}

void print_help(){
    std::cout << "Usage: " << progname << " --vatin VATIN --formula NUMBER\n"
        "\t--help,  -h\t Show help\n"
        "\t--vatin, -v\t [VATIN] Set VAT Identification Number\n"
		"\t--formula, -f\t [FORMULA] Set formula number (from 0 to 268435455)\n"
		"\t--decode, -d\t [UFI STR] Display VATIN and FORMULA from UFI\n"
        "Example of use:\n"
        << progname << " --vatin IE9Z54321Y --formula 134217728\n"
		<< progname << " --decode GMTT-2SQN-6FDD-6TV1" << std::endl;
}
