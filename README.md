# ufi-generator
[![License: MIT](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://opensource.org/licenses/gpl-3.0)

C++ UFI generator, UFI decoder implementation  
:books: [UFI Developer manual](https://poisoncentres.echa.europa.eu/documents/22284544/22295820/ufi_developers_manual_en.pdf)

## Compile
```bash
cmake CMakeLists.txt
make nufi
```

## Use
```bash
./nufi -h

'Usage: nufi --vatin VATIN --formula NUMBER
    --help,  -h      Show help
    --vatin, -v      [VATIN] Set VAT Identification Number
    --formula, -f    [FORMULA] Set formula number (from 0 to 268435455)
    --decode, -d     [UFI STR] Display VATIN and FORMULA from UFI
Example of use:
nufi --vatin IE9Z54321Y --formula 134217728
nufi --decode GMTT-2SQN-6FDD-6TV1'
```

## C++ Sample

```cpp
// Encode
UfiNumber nufi;
try {
    std::string snufi = nufi.generate("IE9Z54321Y", 134217728);
    // UFI: GMTT-2SQN-6FDD-6TV1
    std::cout << "UFI: " << snufi << std::endl;
    nufi.validate(snufi);
} catch(UfiException const& e){
    std::cout << e.what() << std::endl;
}

// Decode
struct reversed_ufi ru = nufi.decode("GMTT-2SQN-6FDD-6TV1");
// VATIN:IE9Z54321Y, formula:134217728
std::cout << ru.to_string() << std::endl;
```