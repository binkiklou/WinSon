#pragma once

namespace son
{
	class impl_wrapper
	{
	public:
		virtual void init();
	};

	class session
	{
	public:
		session();
	private:
		bool inited = false;
		void _init();
		impl_wrapper* _impl = nullptr;
	};
}