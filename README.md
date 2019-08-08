# ufi-generator
A C++ UFI generator implementation

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
    cout << "UFI: " << snufi << endl;
    nufi.validate(snufi);
} catch(UfiException const& e){
    cout << e.what() << endl;
}
```