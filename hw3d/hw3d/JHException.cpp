#include "JHException.h"
#include <sstream>
JHException::JHException(int line, const char * file) noexcept
	:
	line(line) , file(file)
{
}

const char * JHException::what() const noexcept
{
	//oss�� �Լ��� ���� �Ŀ� �����ǹǷ� ��ȯ�ϴ� char*�� ��� ����Ű�� ������ �𸣱� ������
	//oss.str()�� ��ȯ���� �ʰ� �������� �ʰ� �����Ǵ� ������� whatBuffer�� c_str()����ȯ�Ѵ�.
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
