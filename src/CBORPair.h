#ifndef INCLUDED_CBORPAIR_H
#define INCLUDED_CBORPAIR_H

#include "CBOR.h"
#include "Arduino.h"

class CBORPair: public CBOR
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
		CBORPair(size_t buf_len);
		CBORPair(const CBORPair &obj);
		//If has_data == true, then new elements can be added.
		//However, the size of the length field cannot be expanded
		//(expect less than 2^64-1 elements capacity).
		CBORPair(uint8_t* _buffer, size_t buf_len, bool has_data = true);
		//With this constructor, CBOR Object must contain a cbor Pair
		//Same as above: the size of the length field cannot be expanded.
		CBORPair(const CBOR &obj);
		~CBORPair();

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
			CBOR idx_cbor = CBOR(key);
			uint8_t *idx_cbor_buffer = idx_cbor.to_CBOR();
			size_t idx_cbor_size = idx_cbor.length();
			bool equals = true;

			//Save buffer position
			uint8_t *old_buffer = buffer;
			uint8_t *ele_begin = NULL;
			size_t ele_size = 0;

			if (!is_pair()) {
				return CBOR();
			}

			//Put buffer on the first element of the table
			buffer = buffer_data_begin;

			//Search key until the end of the Pair (map) is found
			for (size_t i=0 ; i < n_elements() ; ++i) {
				//Retrieve key and jump to value
				ele_begin = buffer;
				ele_size = jump();

				//Check keys lengths
				if(idx_cbor_size == ele_size) {
					//Compare CBOR representation of keys values
					for (size_t j=0 ; j < ele_size ; ++j) {
						if (idx_cbor_buffer[j] != ele_begin[j]) {
							equals = false;
							break;
						}
					}

					if (equals) {
						//Retrieve value
						ele_begin = buffer;
						ele_size = jump();

						//Put back buffer in its initial position
						buffer = old_buffer;

						return CBOR(ele_begin, ele_size, true);
					}
					//If keys are not the same, jump to next key
					else {
						equals = true;
						jump();
					}
				}
				//If keys are not the same, jump to next key
				else {
					jump();
				}
			}

			//Not found
			//Put back buffer in its initial position
			buffer = old_buffer;
			return CBOR();
		}

		size_t n_elements() const { return decode_abs_num(); }
		size_t max_n_elements() const { return (1<<(buffer_data_begin - ext_buffer_begin)); }
};

#endif

