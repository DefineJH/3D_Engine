#pragma once
//client side에서 이벤트를 빼갈 큐
#include <queue>
//현재 키들의 상태 (눌림 띔) 등을 나타내는 비트플래그
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept
			: type(Type::Invalid), code(0u) {}
		Event(Type type, unsigned char code) noexcept : type(type), code(code) {}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
#pragma region Client
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;

	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;

	//KeyQueue, CharQueue를 모두 flush
	void Flush() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
#pragma endregion
#pragma region Window
private:
	//윈도우 측
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(unsigned char ch) noexcept;
	void ClearState() noexcept;
	template<class T>
	//buffer의 크기를  16으로 설정했기 때문에 16개를 넘어가면 안되므로 버퍼를 자른다
	static void TrimBuffer(std::queue<T>& buffer)noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
#pragma endregion
};

template<class T>
inline void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
