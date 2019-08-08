#include "include/ufiexception.h"

UfiException::UfiException(ErrorType err_type, int code, const std::string& msg)
	: err_code(code)
{
	std::stringstream ss;
	ss << "[" << (err_type == CREATION ? "UFI" :  "VAL")
	<< std::setfill('0') << std::setw(3) << code << "] "
	<< msg;
	message = ss.str();
}

UfiException::~UfiException()
{

}

const char * UfiException::what() const throw()
{
	return message.c_str();
}

int UfiException::code() const
{
	return err_code;
}

