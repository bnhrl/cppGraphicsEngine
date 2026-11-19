#pragma once
#include <memory>

#include "Framework/Object.h"

namespace nu
{
	class Resource : public Object
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;
	};

	template <typename T = Resource>
	using res_t = std::shared_ptr<T>;
}