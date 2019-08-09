# ufi-generator
[![License: MIT](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://opensource.org/licenses/gpl-3.0)

A C++ UFI (Unique Formula Identifier) generator implementation  
:books: [UFI Developer manual](https://poisoncentres.echa.europa.eu/documents/22284544/22295820/ufi_developers_manual_en.pdf)

## Compile
```bash
cmake CMakeLists.txt
make nufi
```

## Use
```bash
./nufi -h
```
>Usage: nufi --vatin VATIN --formula NUMBER  
>&nbsp;&nbsp;&nbsp;&nbsp;--help,  -h      Show help  
>&nbsp;&nbsp;&nbsp;&nbsp;--vatin, -v      [VATIN] Set VAT Identification Number  
>&nbsp;&nbsp;&nbsp;&nbsp;--formula, -f    [FORMULA]  Set formula number (from 0 to 268435455)  
>Example of use:  
>nufi --vatin IE9Z54321Y --formula 134217728

## C++ Sample
```cpp
std::string vatin = "IE9Z54321Y";
int64_t formula = 134217728;
UfiNumber nufi(vatin, formula);
try {
    std::string snufi = nufi.generate();
    // UFI: GMTT-2SQN-6FDD-6TV1
    std::cout << "UFI: " << snufi << std::endl;
    nufi.validate(snufi);
} catch(UfiException const& e){
    std::cout << e.what() << std::endl;
}
```