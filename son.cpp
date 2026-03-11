#include "son.hpp"

#ifdef  _WIN64
#include "win_wrapper.hpp"
#endif

/*
--- IMPL WRAPPER ---
*/
void son::impl_wrapper::init()
{
	return;
}

/*
--- SESSION ---
*/
son::session::session()
{
	this->_init();
}

void son::session::_init()
{
	if (this->inited) {
		return;
	}

	if (this->_impl == nullptr) {
#ifdef _WIN64
		this->_impl = new win_wrapper;
		this->_impl->init();
#endif
	}

	this->inited = true;
}