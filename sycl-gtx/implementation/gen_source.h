#pragma once

#include "specification\access.h"
#include "common.h"
#include "debug.h"
#include <unordered_map>


namespace cl {
namespace sycl {

// Forward declaration
template <typename DataType, int dimensions, access::mode mode, access::target target>
class accessor;


namespace detail {

// Forward declaration
class accessor_base;


namespace kernel_ {

class source {
private:
	// C++14 would come in handy here with addressing tuples by type
	struct tuple {
		string_class type;
		access::mode mode;
		access::target target;
	};

	string_class kernelName;
	vector_class<string_class> lines;
	std::unordered_map<const accessor_base*, tuple> resources;

	// TODO: Multithreading support
	SYCL_THREAD_LOCAL static source* scope;

	template<class KernelType>
	source(string_class kernelName, KernelType kern)
		: kernelName(kernelName) {
		scope = this;
		kern();
		scope = nullptr;
	}

	string_class get();
	string_class generate_accessor_list();
	static string_class get_name(access::target target);
	template<typename DataType>
	static string_class get_name() {
		// TODO
		return "int*";
	}

public:
	template <typename DataType, int dimensions, access::mode mode, access::target target>
	static void add(const accessor<DataType, dimensions, mode, target>* const acc) {
		if(scope == nullptr) {
			//error::report(error::code::NOT_IN_KERNEL_SCOPE);
			return;
		}

		auto accessor_it = scope->resources.find(acc);
		if(accessor_it == scope->resources.end()) {
			scope->resources[acc] = { get_name<DataType>(), mode, target };
		}
	}

	// TODO: Should be better hidden
	static void add(string_class line) {
		scope->lines.push_back('\t' + line + ';');
	}

	template<class KernelType>
	static string_class generate(string_class kernelName, KernelType kern) {
		auto src = source(kernelName, kern);
		return src.get();
	}
};

} // namespace kernel_


// Data reference wrappers
// TODO: They seem to be removed from the specification
class __atomic_ref;
class __read_ref;
class __write_ref {
private:
	string_class name;
public:
	__write_ref(string_class name)
		: name(name) {}
	const __write_ref& operator=(int n) const {
		DSELF() << "not implemented";
		kernel_::source::add(name + "=" + std::to_string(n));
		return *this;
	}
};

} // namespace detail

} // namespace sycl
} // namespace cl
