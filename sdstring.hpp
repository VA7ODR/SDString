/*
Copyright (c) 2012-2023 James Baker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

The official repository for this library is at https://github.com/VA7ODR/json

*/
#pragma once

#include <limits>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <streambuf>

template <class T>
struct secure_delete_allocator {
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(value_type); }
	secure_delete_allocator() noexcept {}
	template <class U> secure_delete_allocator (secure_delete_allocator<U> const&) noexcept {}
	value_type* allocate (std::size_t n) { value_type* p = static_cast<value_type*>(malloc(n*sizeof(value_type))); return p;}
	void deallocate (value_type* p, std::size_t n) { memset((void*)p, 0, n); free(p); }
	template <class U> struct rebind { typedef secure_delete_allocator<U> other; };
};

template <class T, class U>
constexpr bool operator== (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return true;}

template <class T, class U>
constexpr bool operator!= (const secure_delete_allocator<T>&, const secure_delete_allocator<U>&) noexcept
{return false;}

//typedef std::basic_string<char, std::char_traits<char>, secure_delete_allocator<char> > base_sdstring;

template <typename charT>
class basic_sdstring : public std::basic_string<charT, std::char_traits<charT>, secure_delete_allocator<charT>>
{
	public:
		using base_type = std::basic_string<charT, std::char_traits<charT>, secure_delete_allocator<charT>>;

		basic_sdstring(const basic_sdstring & rhs) : base_type(rhs.data(), rhs.size()) { }
		
		basic_sdstring(const base_type & rhs) : base_type(rhs) { }

		basic_sdstring(basic_sdstring && rhs) : base_type(std::move(rhs)) { }
		
		basic_sdstring(base_type && rhs) : base_type(std::move(rhs)) { }

		basic_sdstring(const std::basic_string<charT> & rhs) : base_type(rhs.data(), rhs.size()) { }

		basic_sdstring(std::basic_string<charT> && rhs) : base_type(std::move(rhs.data()), rhs.size()) { }

		basic_sdstring(const char* rhs, size_t size) : base_type(rhs, size) { }

		basic_sdstring(const char* rhs) : base_type(rhs) { }

		basic_sdstring() : base_type() {}

		~basic_sdstring() {  }

		using base_type::base_type;
		using base_type::operator[];
		using base_type::operator=;
		using base_type::operator+=;
		using base_type::append;
		using base_type::assign;
		using base_type::size;
		using base_type::find;
		size_t find(const std::basic_string<charT>& in, size_t pos = 0) const { return find(*in.c_str(), pos); }

		basic_sdstring& operator=(const basic_sdstring& rhs)
        {
            if (&rhs != this) {
				static_cast<base_type&>(*this) = static_cast<const base_type&>(rhs);
            }
            return *this;
        }

		basic_sdstring& operator=(basic_sdstring&& rhs)
        {
            if (&rhs != this) {
				static_cast<base_type&>(*this) = static_cast<const base_type&&>(rhs);
            }
            return *this;
        }

		operator std::basic_string<charT>&() const
        {
			return *((std::basic_string<charT>*) this);
        }

		int compare(const basic_sdstring& rhs)
		{
			int iRet = size() - rhs.size();
			if (iRet) {
				return iRet;
			}
			const char * a = this->c_str();
			const char * b = rhs.c_str();
			size_t l = this->size();
			for (size_t i = 0; i < l && iRet == 0; ++i) {
				iRet = *(a++) - *(b++);
			}
			return iRet;
		}

		int compare(const std::basic_string<charT>& rhs)
		{
			int iRet = size() - rhs.size();
			if (iRet) {
				return iRet;
			}
			const char * a = this->c_str();
			const char * b = rhs.c_str();
			size_t l = this->size();
			for (size_t i = 0; i < l && iRet == 0; ++i) {
				iRet = *(a++) - *(b++);
			}
			return iRet;
		}

#if __cplusplus < 202002L
		bool operator==(const basic_sdstring& rhs) const noexcept
		{
			if (rhs.size() != this->size()) {
				return false;
			}
			return memcmp(rhs.data(), this->data(), this->size()) == 0;
		}

		bool operator!=(const basic_sdstring& rhs) const noexcept
		{
			if (rhs.size() != this->size()) {
				return true;
			}
			return memcmp(rhs.data(), this->data(), this->size()) != 0;
		 }
#endif
		bool operator==(const char* rhs) const noexcept
        {
			if (basic_sdstring(rhs).size() != this->size()) {
				return false;
			}
            return memcmp(rhs, this->data(), this->size()) == 0;
        }

		bool operator!=(const char* rhs) const noexcept
        {
			if (basic_sdstring(rhs).size() != this->size()) {
				return true;
			}
            return memcmp(rhs, this->data(), this->size()) != 0;
        }
};

template <typename charT>
inline basic_sdstring<charT> operator+(const basic_sdstring<charT> & lhs, const std::basic_string<charT> & rhs)
{
	basic_sdstring<charT> ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs);
	return ret;
}

template <typename charT, typename T, typename std::enable_if<std::is_same<T, basic_sdstring<charT>>::value>::type* = nullptr>
inline basic_sdstring<charT> operator+(const basic_sdstring<charT> & lhs, const T & rhs)
{
	basic_sdstring<charT> ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs);
	return ret;
}

template <typename charT>
inline basic_sdstring<charT> operator+(const char* lhs, const basic_sdstring<charT> & rhs)
{
	basic_sdstring<charT> ret;
	ret.reserve(sizeof(lhs) + rhs.size());
	ret.append(lhs);
	ret.append(rhs);
	return ret;
}

template <typename charT>
inline basic_sdstring<charT> operator+(const basic_sdstring<charT> & lhs, const char* rhs)
{
	basic_sdstring<charT> ret;
	ret.reserve(ret.size() + sizeof(rhs));
	ret.append(lhs);
	ret.append(rhs);
	return ret;
}

template <typename charT>
inline std::basic_string<charT> operator+(const std::basic_string<charT> & lhs, const basic_sdstring<charT> & rhs)
{
	std::basic_string<charT> ret;
	ret.reserve(lhs.size() + rhs.size());
	ret.append(lhs);
	ret.append(rhs.c_str());
	return ret;
}

template <typename charT, class T, typename std::enable_if<std::is_same<T, std::basic_string<charT>>::value>::type* = nullptr>
inline bool operator==(const T & lhs, const basic_sdstring<charT> & rhs)
{
	return (lhs == rhs.c_str());
}

template <typename charT, class T, typename std::enable_if<std::is_same<T, std::basic_string<charT>>::value>::type* = nullptr>
inline bool operator!=(const T & lhs, const basic_sdstring<charT> & rhs)
{
	return (lhs != rhs.c_str());
}

template <typename charT>
class sdstreambuf : public std::basic_streambuf<charT>
{
	public:
		sdstreambuf(basic_sdstring<charT> & bufIn) : sBuffer(bufIn) {}

	protected:
		basic_sdstring<charT> & sBuffer;

		std::streamsize xsputn(const typename std::basic_ostream<charT>::char_type* s, std::streamsize n) override
		{
			sBuffer.append(s, n);
			return n;
		}

		virtual std::basic_ostream<char>::int_type overflow(typename std::basic_ostream<charT>::int_type c) override
		{
			sBuffer.push_back((char)c);
			return c;
		}
};

template <typename charT>
class base_sdostream : private sdstreambuf<charT> , public std::basic_ostream<charT>
{
	public:
		base_sdostream(basic_sdstring<charT> & bufIn) : sdstreambuf<charT>(bufIn), std::basic_ostream<charT>(this) {}
};

typedef basic_sdstring<char> sdstring;
typedef basic_sdstring<wchar_t> sdwstring;
typedef base_sdostream<char> sdostream;
typedef base_sdostream<wchar_t> sdwostream;

