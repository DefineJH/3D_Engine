#pragma once
#include <exception>
#include <string>

class JHException : public std::exception
{
public:
	JHException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int line;
	std::string file;
protected:
	//mutable ->  const 함수 내에서 값을 바꿀 수 있음
	mutable std::string whatBuffer;
};