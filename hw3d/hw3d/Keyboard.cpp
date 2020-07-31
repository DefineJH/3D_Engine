#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keyBuffer.size() > 0u)
	{
		Keyboard::Event e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuffer.size() == 0u;
}

void Keyboard::FlushKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushChar();
	FlushKey();
}

char Keyboard::ReadChar() noexcept
{
	if (charBuffer.size() > 0u)
	{
		char ch = charBuffer.front();
		charBuffer.pop();
		return ch;
	}
	else
	{
		return 0;
	}

}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuffer.size() == 0u;
}


void Keyboard::EnableAutoRepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(unsigned char ch) noexcept
{
	charBuffer.push(ch);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keystates.reset();
}
