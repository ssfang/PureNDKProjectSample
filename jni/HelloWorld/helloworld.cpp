#include <stddef.h>
#include <stdio.h>

#include <iostream>
#include <functional>
#include <vector>
#include <list>
#include <string>
#include <bitset>
#include <algorithm>    // std::sort

#include <tuple>

#define __GXX_EXPERIMENTAL_CXX0X__ 1

#if __cplusplus >= 201103L
	#pragma message "C++ ISO/IEC 14882:2011 supported by the compiler (informally known as C++ 11)"
#elif __cplusplus >= 199711L
	#pragma message "C++ ISO/IEC 14882:1998 supported by the compiler"
#else
	/* #error No C++ support */
	/* static_assert(false,"__cplusplus is less than or equal to 199711L") */
#endif

template<class T, const size_t N>
class Array {
private:
	T arr[N];
public:
	T *data() {
		return arr;
	}

	const T *data() const {
		return arr;
	}

	T *end() {
		return arr + N;
	}

	const T *end() const {
		return arr + N;
	}

	T &operator[](ptrdiff_t i) {
		return arr[i];
	}
	const T &operator[](ptrdiff_t i) const {
		return arr[i];
	}

	const size_t size() const {
		return N;
	}
};

int main(int argc, const char *argv[]) {
	Array<char, 2> v2;
	for (size_t i = 0; i < v2.size(); ++i) {
		v2[i] = 65 + i;
	}

	printf("Array<char, %zu> = %.*s\n", v2.size(), v2.size(), v2.data());

	/*
	 A possible output for this program could be:
	 size: 4
	 length: 4
	 capacity: 15
	 max_size: 4294967294
	 */
	std::string str("fang");
	printf("size: %d\n", str.size());
	printf("length: %d\n", str.length());
	printf("capacity: %d\n", str.capacity());
	printf("max_size: %u\n", str.max_size());

	str.append("shanshan");
	printf("find_first_of: %d\n", str.find_first_of("an"));
	printf("find_first_not_of: %d\n", str.find_first_not_of("fang"));


	int myints[] = { 32, 71, 12, 45, 26, 80, 53, 33 };
	std::sort(myints, myints + 4);


	std::list<int> mylist;
	printf("mylist.size() = %d, mylist.max_size() = %u\n", mylist.size(),
			mylist.max_size());
	mylist.resize(2);
	printf("after mylist.resize(2)\n");
	printf("mylist.size() = %d, mylist.max_size() = %u\n", mylist.size(),
			mylist.max_size());
	mylist.sort();
	printf("\n\n");
	return 0;
}
