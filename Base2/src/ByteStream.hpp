/*
 * ByteStream.h
 *
 *  Created on: Sep 12, 2014
 *      Author: parth
 */

#ifndef BYTESTREAM_H_
#define BYTESTREAM_H_
#include <cstring>
#include <vector>
#include <string>
#include <assert.h>
#include <stdexcept>

#include "detail/detail.h"


class ByteStream {
public:

	struct Raw { const char* data; size_t nBytes; };
	ByteStream() { _init(); }
	~ByteStream() { delete [] _buffer; }

	unsigned int size() const {
		return _insertCursor;
	}

	bool available() const {
		return _pos != size();
	}

	template<class T>
	T next() const {
		detail::Tag<T> tag = detail::Tag<T>();
		T val = _next(_buffer + _pos , tag);
		return val;
	}

	const char* rawBytes(int nBytes) const {
		_assertCapacity(nBytes);
		char *bytes = new char[nBytes] {};
		memcpy(bytes, _buffer + _pos, nBytes);
		_pos += nBytes;
		return bytes;
	}

	operator const char* () const {
		return _buffer;
	}

	void reset() {
		_pos = 0;
		_size = 0;
		_insertCursor = 0;
	}

	void clear() {
		reset();
		delete [] _buffer;
		_init();
	}

	template<class T>
	void insert(T value) {
		int typeSize = sizeof(T);
		_possiblyExpand(typeSize);

		union Serializer {
			T value;
			char c[sizeof(T)];
		};
		static Serializer ser {};
		ser.value = value;
		memcpy(_buffer + _insertCursor, ser.c, typeSize);
		_insertCursor += typeSize;
	}

	template<class T = const char*>
	void insert(const char* str) {
		auto nChars = strlen(str);
		insert(nChars); // Insert the length
		insert(Raw { str, nChars });
	}

	template<class T = std::string>
	void insert(std::string str) {
		insert(str.c_str());
	}

	template<class T = Raw>
	void insert(const Raw& raw) {
		const char* data = raw.data;
		int size = raw.nBytes;
		_possiblyExpand(size);
		memcpy(_buffer + _insertCursor, data, size);
		_insertCursor += size;
	}

	template<class T = char>
	void insert(char c) {
		_possiblyExpand(1);
		_buffer[_insertCursor++] = c;
	}

private:

	void _assertCapacity(unsigned int capacity) const {
		if ((size() - _pos) < capacity) {
			throw std::logic_error("Stream does not contain enough bytes: " + (size() - _pos));
		}
	}

	void _init() {
		//Start with a buffer size of 16 and (at least) double it if we need
		_size = 16;
		_buffer = new char[_size] {};
		_insertCursor = 0;

	}

	void _possiblyExpand(int minGuarantee) {
		if((_insertCursor + minGuarantee) >= _size - 1) {
			unsigned int oldSize { _size };
			_size *= 2;
			_size += minGuarantee;
			char *expandedBuffer = new char[_size] {};
			memcpy(expandedBuffer, _buffer, oldSize);
			delete [] _buffer;
			_buffer = expandedBuffer;
		}
	}

	template<class T>
	T _next(const char* buff, const detail::Tag<T>& tag) const {
		int typeSize = sizeof(T);
		_assertCapacity(typeSize);
		union Deserializer {
			T value;
			char bytes[sizeof(T)];
		};
		static Deserializer der { };
		memcpy(der.bytes, buff, typeSize);
		_pos += typeSize;
		return der.value;
	}

	template<class T = std::string>
	std::string _next(const char* buff, const detail::Tag<std::string>& tag) const {
		auto stringLength = next<size_t>();
		_assertCapacity(stringLength);
		std::string s { };
		s.assign(buff + sizeof(stringLength), stringLength);
		_pos += stringLength;
		return s;
	}

	char * _buffer;
	mutable unsigned int _pos = 0;
	unsigned int _size = 0;
	unsigned int _insertCursor = 0;
};


#endif /* BYTESTREAM_H_ */
