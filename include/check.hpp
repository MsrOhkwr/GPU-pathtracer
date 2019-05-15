#ifndef CHECK_HPP_
#define CHECK_HPP_

#include <string>
#include <sstream>

typedef enum
{
	glSuccess        = 0,
	glWindowError    = 1,
	glInitError      = 2,
	glFileError      = 3,
	glCompileError   = 4,
	glLinkError      = 5,
	allocateError    = 6,
} appResult_t;

class appError : std::exception
{
public:
	appError(appResult_t r, const std::string file, int line)
	{
		this->r = r;
		this->file = file;
		this->line = line;
	}
	appError(appResult_t r, const std::string file, int line, const std::string path)
	{
		this->r = r;
		this->file = file;
		this->line = line;
		this->path = path;
	}
	std::string message() const;
private:
	appResult_t r;
	std::string file;
	int line;
	std::string path;
};

#endif