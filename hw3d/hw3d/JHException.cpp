#include "JHException.h"
#include <sstream>
JHException::JHException(int line, const char * file) noexcept
	:
	line(line) , file(file)
{
}

const char * JHException::what() const noexcept
{
	//oss는 함수가 끝난 후에 삭제되므로 반환하는 char*는 어디를 가리키고 있을지 모르기 때문에
	//oss.str()을 반환하지 않고 삭제되지 않고 유지되는 멤버변수 whatBuffer의 c_str()을반환한다.
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * JHException::GetType() const noexcept
{
	return "DirectException";
}

int JHException::GetLine() const noexcept
{
	return line;
}

const std::string & JHException::GetFile() const noexcept
{
	return file;
}

std::string JHException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
