#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

class Codex
{
public:
	//각 bindable마다 필요로 하는 파라미터 개수가 다르기 때문에 가변 파라미터 사용
	template<class T, typename...Params>
	static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p) 
	{
		return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
	}
private:
	template<class T, typename...Params>
	std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...p) 
	{
		const std::string key = T::GenerateUID(std::forward<Params>(p)...);
		const auto i = binds.find(key);
		if (i == binds.end())
		{
			auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
			binds[key] = bind;
			return bind;
		}
		else
		{
			return std::static_pointer_cast<T>(i->second);
		}
	}
	static Codex& Get()
	{
		static Codex codex;
		return codex;
	}
private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
};
