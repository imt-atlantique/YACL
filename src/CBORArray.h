#ifndef INCLUDED_CBORARRAY_H
#define INCLUDED_CBORARRAY_H

#include "CBOR.h"

class CBORArray: public CBOR
{
	protected:
		//Buffer is allocated as follows:
		//         Provision
		//        for num_ele
		//     <-------------->
		//             num_ele            data
		//           <--------><---------------------->
		//     |  |  |  |  |  |  |  |  |  |  |  |  |  |
		//      ^     ^        ^                        ^
		//      |     |        +--- buffer_data_begin   |
		//      |     +--- buffer_begin                 |
		//ext_buffer_begin                            buffer
		uint8_t *ext_buffer_begin, *buffer_data_begin;

		void init_num_ele(size_t num_ele);
		void increment_num_ele() { init_num_ele(n_elements()+1); };

		bool init_buffer();
	public:
		CBORArray(size_t buf_len);
		CBORArray(const CBORArray &obj);
		//If has_data == true, then new elements can be added.
		//However, the size of the length field cannot be expanded
		//(expect less than 2^64-1 elements capacity).
		CBORArray(uint8_t* _buffer, size_t buf_len, bool has_data = true);
		//With this constructor, CBOR Object must contain a cbor array
		//Same as above: the size of the length field cannot be expanded.
		CBORArray(const CBOR &obj);
		~CBORArray();

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

		size_t n_elements() const { return decode_abs_num(); }
		size_t max_n_elements() const { return (1<<(buffer_data_begin - ext_buffer_begin)); }
};

#endif
