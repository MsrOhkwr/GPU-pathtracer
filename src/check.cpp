#include <check.hpp>

std::string appError::message() const
{
	std::stringstream msg;
	switch (r)
	{
		case glSuccess:
		{
			break;
		}
		case glWindowError:
		{
			msg << "Failed (" << file << " at line " << line << ") : Cannot open a window";
			break;
		}
		case glInitError:
		{
			msg << "Failed (" << file << " at line " << line << ") : " << "Cannot initialize GLAD";
			break;
		}
		case glFileError:
		{
			msg << "Failed (" << file << " at line " << line << ") : " << "Cannot read shader <" << path << ">";
			break;
		}
		case glCompileError:
		{
			msg << "Failed (" << file << " at line " << line << ") : " << "Compile error in shader <" << path << ">";
			break;
		}
		case glLinkError:
		{
			msg << "Failed (" << file << " at line " << line << ") : " << "Linking error in program";
			break;
		}
		case allocateError:
		{
			msg << "Failed (" << file << " at line " << line << ") : " << "Cannot allocate memory";
		}
		default:
		{
			break;
		}
	}
	return msg.str();
}