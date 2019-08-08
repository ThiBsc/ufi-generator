#ifndef UFI_EXCEPTION_H
#define UFI_EXCEPTION_Hvv

#include <exception>
#include <iomanip>
#include <sstream>
#include <string>

#define UFI001 	1
#define UFI002 	2
#define UFI003 	3
#define UFI004 	4
#define UFI005	5
#define UFI006 	6
#define UFI007 	7
#define UFI008 	8
#define UFI009 	9
#define UFI0010 10
#define UFI0011 11

#define VAL001 1
#define VAL002 2
#define VAL003 3
#define VAL004 4
#define VAL005 5

class UfiException : public std::exception
{

public:
	enum ErrorType { CREATION=0, VALIDATION };
	UfiException(ErrorType err_type, int code, const std::string& msg);
	virtual ~UfiException() throw();
	virtual const char *what() const throw();
	int code() const;

private:
	int err_code;
	std::string message;
};

#endif // UFI_EXCEPTION_H
