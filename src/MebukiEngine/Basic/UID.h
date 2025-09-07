#pragma once

namespace meb
{
	typedef unsigned long long uid;

	class UID
	{
	public:
		static uid Generate()
		{
			return current++;
		}


	private:
		static uid current;
		UID() = delete;
	};
}

