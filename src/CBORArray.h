#ifndef INCLUDED_CBORARRAY_H
#define INCLUDED_CBORARRAY_H

#include "CBORComposed.h"

class CBORArray: public CBORComposed<CBOR_ARRAY>
{
	public:
		CBORArray() : CBORComposed() {};
		CBORArray(size_t buf_len) : CBORComposed(buf_len) {};
		CBORArray(const CBORArray &obj) : CBORComposed(obj) {};
		//If has_data == true, then new elements can be added.
		//However, the size of the length field cannot be expanded
		//(expect less than 2^64-1 elements capacity).
		CBORArray(uint8_t* _buffer, size_t buf_len, bool has_data = true);
		//With this constructor, CBOR Object must contain a cbor array
		//Same as above: the size of the length field cannot be expanded.
		CBORArray(const CBOR &obj);

		bool append();
		template <typename T> bool append(T value)
		{
			increment_num_ele();
			return add(value);
		}
		template <typename T> bool append(const T *array, size_t size)
		{
			bool ret_val = true;

			init_num_ele(n_elements() + size);
			for (T *ptr = array ; ptr < (array+size) ; ++ptr) {
				ret_val &= add(*ptr);
			}

			return ret_val;
		}

		CBOR operator[](size_t idx);
};

#endif
