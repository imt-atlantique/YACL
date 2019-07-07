#ifndef INCLUDED_CBORCOMPOSED_H
#define INCLUDED_CBORCOMPOSED_H

#include "CBOR.h"

#define NUM_ELE_PROVISION 9

template <uint8_t cbor_type> class CBORComposed: public CBOR
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
		//ext_buffer_begin                            w_ptr
		uint8_t *ext_buffer_begin, *buffer_data_begin;

		void init_num_ele(size_t num_ele)
		{
			//Save buffer
			uint8_t *old_w_ptr = w_ptr;
		
			//Put buffer pointer in the right position, considering the size of the
			//length field
			w_ptr = buffer_data_begin - compute_type_num_len(num_ele);
			//Update buffer_begin
			buffer_begin = w_ptr;
		
			//Encode num_ele
			encode_type_num(cbor_type, num_ele);
		
			//Put buffer back into its inital position
			w_ptr = old_w_ptr;
		
			return;
		}

		void increment_num_ele() { init_num_ele(n_elements()+1); };

		bool init_buffer()
		{
			//Reserve begining of buffer to store table length
			ext_buffer_begin = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
			if (ext_buffer_begin == NULL) {
				buffer_data_begin = NULL;
				buffer_begin = NULL;
				w_ptr = NULL;

				return false;
			}

			buffer_data_begin = ext_buffer_begin + NUM_ELE_PROVISION;
			buffer_begin = ext_buffer_begin + NUM_ELE_PROVISION - 1; //type_num_len is 1 for array size 0
			w_ptr = buffer_data_begin;

			buffer_type = BUFFER_DYNAMIC_INTERNAL;

			return true;
		}
	public:
		CBORComposed()
		{
			//Reserve enough memmory for one element of one byte
			max_buf_len = 10;
			init_buffer();

			//Initialize num_ele
			init_num_ele(0);
		}

		CBORComposed(size_t buf_len)
		{
			//Reserve buffer
			max_buf_len = buf_len - 1 + NUM_ELE_PROVISION;
			init_buffer();

			//Initialize num_ele
			init_num_ele(0);
		}

		CBORComposed(const CBORComposed &obj)
		{
			uint8_t type_num_len = compute_type_num_len(obj.n_elements());
			//Reserve buf_len plus maximum size of a type_num : 9 bytes to encode a
			//table length up to (2^64)-1
			size_t buf_len_needed = obj.length() - type_num_len + NUM_ELE_PROVISION;

			max_buf_len = buf_len_needed;
			init_buffer();

			//Reserve begining of buffer to store table length
			buffer_begin = ext_buffer_begin + NUM_ELE_PROVISION - type_num_len;
			w_ptr = buffer_begin + obj.length();

			//Copy num_ele and data
			memcpy(buffer_begin, obj.to_CBOR(), obj.length());
		}

		~CBORComposed()
		{
			if(buffer_type == BUFFER_DYNAMIC_INTERNAL) {
				free(ext_buffer_begin);
			}
		}

		bool reserve(size_t len)
		{
			size_t num_ele = n_elements();
			size_t requested_len = len - compute_type_num_len(num_ele) + NUM_ELE_PROVISION;

			if (requested_len <= max_buf_len) {
				return true;
			}

			if (buffer_type == BUFFER_DYNAMIC_INTERNAL) {
				size_t length_saved = length();

				ext_buffer_begin = (uint8_t*)realloc(ext_buffer_begin,
						sizeof(uint8_t)*requested_len);
				if (ext_buffer_begin == NULL) {
					return false;
				}

				//Update max buffer length and write pointer
				max_buf_len = requested_len;
				buffer_data_begin = ext_buffer_begin + NUM_ELE_PROVISION;
				buffer_begin = buffer_data_begin - compute_type_num_len(num_ele);
				w_ptr = buffer_begin + length_saved;

				return true;
			}

			//BUFFER_EXTERNAL or BUFFER_STATIC_INTERNAL
			return false;
		}
		size_t n_elements() const { return decode_abs_num(); }
		size_t max_n_elements() const { return (1<<(buffer_data_begin - ext_buffer_begin)); }
};

#endif
