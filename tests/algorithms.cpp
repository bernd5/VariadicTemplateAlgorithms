#include "vta/algorithms.hpp"

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iostream>
#include <type_traits>
#include <string>

namespace {

struct printer {
	printer(std::ostream& str)
	: m_str{&str} {
	}

	template <typename T>
	void operator()(T const& obj) {
		*m_str << obj;
	}

	std::ostream* m_str;
};

struct string_concat {
	template <typename LHS, typename RHS>
	std::string operator()(LHS const& lhs, RHS const& rhs) const {
		std::stringstream ss;
		ss << lhs << rhs;
		return ss.str();
	}
};

struct is_positive_int {
	bool operator()(int x) const {
		return x > 0;
	}

	template <typename T>
	bool operator()(T const&) const {
		return false;
	}
};

bool is_true(bool p) {
	return p;
}

struct adjacent_printer {
	adjacent_printer(std::string& str)
	: m_str{&str} {
	}

	void operator()(int i, int j) {
		std::stringstream ss;
		ss << i * j;
		*m_str += ss.str();
	}

	std::string* m_str;
};

}

BOOST_AUTO_TEST_SUITE(algorithms)

BOOST_AUTO_TEST_CASE(are_same) {
	static_assert(vta::are_same<>::value, "");
	static_assert(vta::are_same<int>::value, "");
	static_assert(vta::are_same<int, int>::value, "");
	static_assert(vta::are_same<int, int, int>::value, "");
	static_assert(!vta::are_same<int, float>::value, "");
	static_assert(!vta::are_same<int&, int>::value, "");
	static_assert(!vta::are_same<int const&, int&>::value, "");
	static_assert(!vta::are_same<int&&, int&>::value, "");
	static_assert(!vta::are_same<const int, int>::value, "");
	static_assert(!vta::are_same<volatile int, int>::value, "");
}

BOOST_AUTO_TEST_CASE(are_same_after) {
	static_assert(vta::are_same_after<std::remove_reference>::value, "");
	static_assert(vta::are_same_after<std::remove_reference, int>::value, "");
	static_assert(vta::are_same_after<std::remove_reference, int&&, int&>::value, "");
	static_assert(!vta::are_same_after<std::remove_reference, int, float>::value, "");
}

BOOST_AUTO_TEST_CASE(are_unique_ints) {
	static_assert(vta::are_unique_ints<>::value, "");
	static_assert(vta::are_unique_ints<1>::value, "");
	static_assert(vta::are_unique_ints<1, 2>::value, "");
	static_assert(!vta::are_unique_ints<1, 1>::value, "");
	static_assert(vta::are_unique_ints<1, -1>::value, "");
	static_assert(vta::are_unique_ints<1, 2, 3, 4, 5>::value, "");
	static_assert(!vta::are_unique_ints<1, 2, 3, 1, 5>::value, "");
	static_assert(!vta::are_unique_ints<2, 3, 4, 5, 6, 5>::value, "");
}

BOOST_AUTO_TEST_CASE(are_unique) {
	static_assert(vta::are_unique<>::value, "");
	static_assert(vta::are_unique<int>::value, "");
	static_assert(vta::are_unique<int, float>::value, "");
	static_assert(!vta::are_unique<int, int>::value, "");
	static_assert(vta::are_unique<int, int&>::value, "");
	static_assert(vta::are_unique<int&&, int&>::value, "");
	static_assert(vta::are_unique<int, float>::value, "");
	static_assert(!vta::are_unique<int, float, int>::value, "");
	static_assert(vta::are_unique<int, float, char>::value, "");
}

BOOST_AUTO_TEST_CASE(are_unique_after) {
	static_assert(vta::are_unique_after<std::remove_reference>::value, "");
	static_assert(vta::are_unique_after<std::remove_reference, int>::value, "");
	static_assert(!vta::are_unique_after<std::remove_reference, int&&, int&>::value, "");
	static_assert(vta::are_unique_after<std::remove_reference, int, float>::value, "");
}

BOOST_AUTO_TEST_CASE(head) {
	BOOST_CHECK_EQUAL(vta::head(0), 0);
	BOOST_CHECK_EQUAL(vta::head(std::string{"1"}), "1");
	BOOST_CHECK_EQUAL(vta::head(0, 1, 2), 0);
	BOOST_CHECK_EQUAL(vta::head(1, "2", 3), 1);
}

BOOST_AUTO_TEST_CASE(last) {
	BOOST_CHECK_EQUAL(vta::last(0), 0);
	BOOST_CHECK_EQUAL(vta::last(std::string{"1"}), "1");
	BOOST_CHECK_EQUAL(vta::last(0, 1), 1);
	BOOST_CHECK_EQUAL(vta::last(0, 1, 2), 2);
	BOOST_CHECK_EQUAL(vta::last(1, "2", 3), 3);
}

BOOST_AUTO_TEST_CASE(at) {
	BOOST_CHECK_EQUAL(vta::at<0>(0), 0);
	BOOST_CHECK_EQUAL(vta::at<-1>(0), 0);
	BOOST_CHECK_EQUAL(vta::at<0>(1, '2', 3u, 4.5, "six"), 1);
	BOOST_CHECK_EQUAL(vta::at<-5>(1, '2', 3u, 4.5, "six"), 1);
	BOOST_CHECK_EQUAL(vta::at<3>(1, '2', 3u, 4.5, "six"), 4.5);
	BOOST_CHECK_EQUAL(vta::at<-2>(1, '2', 3u, 4.5, "six"), 4.5);
	BOOST_CHECK_EQUAL(vta::at<4>(1, '2', 3u, 4.5, "six"), std::string{"six"});
	BOOST_CHECK_EQUAL(vta::at<-1>(1, '2', 3u, 4.5, "six"), std::string{"six"});
}

BOOST_AUTO_TEST_CASE(head_t) {
	static_assert(std::is_same<int, vta::head_t<int>>::value, "");
	static_assert(std::is_same<int, vta::head_t<int, char>>::value, "");
	static_assert(std::is_same<float, vta::head_t<float, char>>::value, "");
	static_assert(!std::is_same<float, vta::head_t<int, char>>::value, "");
}

BOOST_AUTO_TEST_CASE(last_t) {
	static_assert(std::is_same<int, vta::last_t<int>>::value, "");
	static_assert(std::is_same<char, vta::last_t<int, char>>::value, "");
	static_assert(std::is_same<char, vta::last_t<float, char>>::value, "");
	static_assert(!std::is_same<int, vta::last_t<int, char>>::value, "");
}

BOOST_AUTO_TEST_CASE(at_t) {
	static_assert(std::is_same<int, vta::at_t<0>::type<int>>::value, "");
	static_assert(std::is_same<int, vta::at_t<0>::type<int, char>>::value, "");
	static_assert(std::is_same<char, vta::at_t<1>::type<int, char>>::value, "");
	static_assert(std::is_same<char, vta::at_t<-1>::type<int, char>>::value, "");
	static_assert(std::is_same<int, vta::at_t<2>::type<float, char, int>>::value, "");
	static_assert(!std::is_same<int, vta::at_t<-2>::type<float, char, int>>::value, "");
}

/**************************************************************************************************
 * Transformations                                                                                *
 **************************************************************************************************/

BOOST_AUTO_TEST_CASE(flip) {
	{
		std::stringstream ss;
		vta::forward_after<vta::flip>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "2134.5six");
	}
}

BOOST_AUTO_TEST_CASE(left_shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six1234.5");
	}
}

BOOST_AUTO_TEST_CASE(right_shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six1234.5");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}
}

BOOST_AUTO_TEST_CASE(shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<-2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "4.5six123");
	}
}

BOOST_AUTO_TEST_CASE(left_shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "134.5six2");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "14.5six23");
	}
}

BOOST_AUTO_TEST_CASE(right_shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1six234.5");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "14.5six23");
	}
}

BOOST_AUTO_TEST_CASE(shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "134.5six2");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<-1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1six234.5");
	}
}

BOOST_AUTO_TEST_CASE(drop) {
	{
		BOOST_CHECK_EQUAL(vta::forward_after<vta::drop<0>>([](){ return 1; })(), 1);
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "34.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<5>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<-5>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<-3>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "34.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<-1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six");
	}
}

BOOST_AUTO_TEST_CASE(take) {
	{
		std::stringstream ss;
		vta::forward_after<vta::take<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "12");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<-5>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<-3>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "12");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<-1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5");
	}
}

BOOST_AUTO_TEST_CASE(cycle) {
	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<-3>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "2134.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 1, 2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "3124.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<-1, 0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six234.51");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 2, 4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six214.53");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<4, 1, 2, 0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "3six24.51");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 4, 2, 3, 1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "24.5six31");
	}
}

BOOST_AUTO_TEST_CASE(reverse) {
	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1);
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1, '2');
		BOOST_CHECK_EQUAL(ss.str(), "21");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six4.5321");
	}
}

BOOST_AUTO_TEST_CASE(filter) {
	{
		std::stringstream ss;
		vta::forward_after<vta::filter<std::is_integral>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::filter<std::is_integral>>(vta::map(printer{ss}))(1);
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::filter<std::is_integral>>(vta::map(printer{ss}))("1");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::filter<std::is_integral>>(vta::map(printer{ss}))(1, "2", 3.0);
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::filter<std::is_integral>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "123");
	}
}

// Functions

BOOST_AUTO_TEST_CASE(map) {
	std::stringstream ss;
	vta::map(printer{ss})(1, 2, 3u, ' ', 4.5, " hello", " world!");
	BOOST_CHECK_EQUAL(ss.str(), "123 4.5 hello world!");
}

BOOST_AUTO_TEST_CASE(adjacent_map) {
	std::string s;
	adjacent_printer printer{s};
	vta::adjacent_map<2>(std::ref(printer))(1, 2, 3, 4, 5, 6);
	BOOST_CHECK_EQUAL(s, "26122030");
}

BOOST_AUTO_TEST_CASE(foldl) {
	auto s = string_concat{};
	auto const result = vta::foldl(s)(1, 2, 3);
	BOOST_CHECK_EQUAL(result, "123");
}

BOOST_AUTO_TEST_CASE(foldr) {
	auto minus = [](auto l, auto r){ return l - r; };
	BOOST_CHECK_EQUAL(vta::foldr(minus)(1), 1);
	BOOST_CHECK_EQUAL(vta::foldr(minus)(1, 2), -1);
	BOOST_CHECK_EQUAL(vta::foldr(minus)(1, 2, 3), 2);
}

BOOST_AUTO_TEST_CASE(compose) {
	{
		std::stringstream ss;
		vta::forward_after<vta::compose<vta::left_shift<1>>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "12340");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::compose<vta::left_shift<1>, vta::flip>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "21340");
	}
}

BOOST_AUTO_TEST_CASE(swap) {
	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, 4>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01432");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, -1>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01432");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<0, 2>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "21034");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<1, 1>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01234");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, -3>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01234");
	}
}

BOOST_AUTO_TEST_CASE(all_of) {
	auto const all_positive_int = vta::all_of(is_positive_int{});
	BOOST_CHECK_EQUAL(all_positive_int(1, 2), true);
	BOOST_CHECK_EQUAL(all_positive_int("hello", 2), false);
	BOOST_CHECK_EQUAL(all_positive_int(0u, 2), false);
	BOOST_CHECK_EQUAL(all_positive_int(-2), false);
	BOOST_CHECK_EQUAL(all_positive_int(), true);

	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(), true);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(true), true);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(false), false);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(true, false), false);
}

BOOST_AUTO_TEST_CASE(any_of) {
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(1, 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})("hello", 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(0u, 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(-2), false);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(), false);
}

BOOST_AUTO_TEST_CASE(none_of) {
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(1, 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})("hello", 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(0u, 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(-2), true);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(), true);
}

BOOST_AUTO_TEST_CASE(macro) {
	BOOST_CHECK_EQUAL(vta::foldl(VTA_FN_TO_FUNCTOR(std::max))(0, 1, 4, 2), 4);
}

BOOST_AUTO_TEST_SUITE_END()
