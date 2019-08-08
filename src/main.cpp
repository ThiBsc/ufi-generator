#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <fstream>

#include "include/ufinumber.h"

using namespace std;

const std::string progname = "nufi";

void print_help();

int main(int argc, char *argv[]) {

	int help = 0;
	std::string vatin;
	int64_t formula = -1;

	int c, idxopt;

	struct option t_opt[] =
	{
		{"help",			no_argument,			&help,		1},
		{"vatin",			required_argument,		NULL,		'v'},
		{"formula",			required_argument,		NULL,		'f'},
        {0,         		0,              		NULL,		0}
	};
	
	while ((c = getopt_long(argc, argv, "hv:f:", t_opt, &idxopt)) != -1){
		switch (c){
			case 'v':
				vatin = optarg;
				break;
			case 'f':
				formula = strtoll(optarg, NULL, 10);
				break;
			case 'h':
				help = 1;
				break;
			default:
				break;
		}
  	}
  	if (!help && !vatin.empty() && formula != -1){
		UfiNumber nufi(vatin, formula);
		try {
			std::string snufi = nufi.generate();
			cout << "UFI: " << snufi << endl;
			nufi.validate(snufi);
		} catch(UfiException const& e){
			cout << e.what() << endl;
		}
  	} else {
  		print_help();
  	}
	
	return 0;
}

void print_help(){
    std::cout << "Usage: " << progname << " --vatin VATIN --formula NUMBER\n"
        "\t--help,  -h\t Show help\n"
        "\t--vatin, -v\t [VATIN] Set VAT Identification Number\n"
		"\t--formula, -f\t [FORMULA]  Set formula number (from 0 to 268435455)\n"
        "Example of use:\n"
        << progname << " --vatin IE9Z54321Y --formula 134217728\n" << std::endl;
}
