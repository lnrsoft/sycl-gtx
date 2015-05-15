#pragma once

#include "data_ref.h"
#include "common.h"
#include "debug.h"

namespace cl {
namespace sycl {

namespace detail {
namespace control {
	
template <class T>
static void if_(T condition) {
	kernel_::source::add<false>(
		string_class("if(") + data_ref::get_name(condition) + ")"
	);
}

template <class T>
static void else_if(T condition) {
	kernel_::source::add<false>(
		string_class("else if(") + data_ref::get_name(condition) + ")"
	);
}

static void else_() {
	kernel_::source::add<false>("else");
}

static void while_(data_ref condition) {
	kernel_::source::add<false>(string_class("while( ") + condition.name + ")");
}

} // namespace control
} // namespace detail

} // namespace sycl
} // namespace cl

#define SYCL_IF(condition) \
	::cl::sycl::detail::control::if_((condition));

#define SYCL_BLOCK(code)									\
	{														\
	::cl::sycl::detail::kernel_::source::add_curlies();		\
	code													\
	::cl::sycl::detail::kernel_::source::remove_curlies();	\
	}

#define SYCL_THEN(code) \
	SYCL_BLOCK(code)

#define SYCL_ELSE(code)						\
	::cl::sycl::detail::control::else_();	\
	SYCL_BLOCK(code)

#define SYCL_ELSE_IF(condition) \
	::cl::sycl::detail::control::else_if((condition));

#define SYCL_WHILE(condition)						\
	::cl::sycl::detail::control::while_(			\
		::cl::sycl::detail::data_ref((condition))	\
	);
