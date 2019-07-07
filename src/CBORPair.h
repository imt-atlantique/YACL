#ifndef INCLUDED_CBORPAIR_H
#define INCLUDED_CBORPAIR_H

#include "CBORComposed.h"

class CBORPair: public CBORComposed<CBOR_MAP>
{
	protected:
		static bool buffer_equals(const uint8_t* buf1, size_t len_buf1,
				const uint8_t* buf2, size_t len_buf2);
	public:
		CBORPair() : CBORComposed() {};
		CBORPair(size_t buf_len) : CBORComposed(buf_len) {};
		CBORPair(const CBORPair &obj) : CBORComposed(obj) {};
		//If has_data == true, then new elements can be added.
		//However, the size of the length field cannot be expanded
		//(expect less than 2^64-1 elements capacity).
		CBORPair(uint8_t* _buffer, size_t buf_len, bool has_data = true);
		//With this constructor, CBOR Object must contain a cbor Pair
		//Same as above: the size of the length field cannot be expanded.
		CBORPair(const CBOR &obj);

		template <typename T, typename U> bool append(T key, U value)
		{
			bool ret_val = true;

			increment_num_ele();
			ret_val = add(key);
			ret_val &= add(value);

			return ret_val;
		}

		template <typename T> CBOR operator[](T key)
		{
			if (!is_pair()) {
				return CBOR();
			}

			CBOR idx_cbor = CBOR(key);
			uint8_t *idx_cbor_buffer = idx_cbor.to_CBOR();
			size_t idx_cbor_size = idx_cbor.length();
			uint8_t *ele_begin = buffer_data_begin;

			//Search key until the end of the Pair (map) is found
			for (size_t i=0 ; i < n_elements() ; ++i) {
				//If key match
				if (buffer_equals(idx_cbor_buffer, idx_cbor_size, ele_begin, element_size(ele_begin))) {
					ele_begin += element_size(ele_begin);

					return CBOR(ele_begin, element_size(ele_begin), true);
				}
				else {
					//Key don't match, jump to next key
					ele_begin += element_size(ele_begin);
					ele_begin += element_size(ele_begin);
				}
			}

			//Not found
			return CBOR();
		}

		CBOR at(size_t idx);
		CBOR key_at(size_t idx);
};

#endif

