#ifndef INCLUDED_CBORCOMPOSED_H
#define INCLUDED_CBORCOMPOSED_H

#include "CBOR.h"

//! Maximum binary size occupied by the number of elements counter.
#define NUM_ELE_PROVISION 9

//! A class to handle composed CBOR Objects.
/*!
 * This class handles encoding of multiple CBOR objects into a composed
 * CBOR object such as an array or a dictionary of key/values.
 * 
 * \tparam cbor_type The byte corresponding to the composed CBOR type
 * (0x80 for CBOR arrays, 0xA0 for CBOR dictionaries).
 */
template <uint8_t cbor_type> class CBORComposed: public CBOR
{
	protected:
		/*!
		 * Buffer is allocated as follows:
		 *                          Total buffer length
		 *      <----------------------------------------------------------->
		 *          Provision
		 *         for num_ele
		 *      <-------------->
		 *              num_ele            data
		 *            <--------><---------------------->
		 *      |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
		 *       ^     ^        ^                        ^
		 *       |     |        +--- buffer_data_begin   |
		 *       |     +--- buffer_begin                 |
		 * ext_buffer_begin                            w_ptr
		 */
		uint8_t *ext_buffer_begin, *buffer_data_begin;

		//! Encodes a number of elements.
		/*!
		 * Encodes the number of elements, and makes the necessary changes in
		 * `buffer_begin` so that it actually points to the begining of the
		 * composed CBOR object (including its length).
		 *
		 * \param num_ele The number of elements to encode.
		 */
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

		//! Increment the number of elements by one.
		void increment_num_ele() { init_num_ele(n_elements()+1); };

		/*!
		 * Initialize a dynamically allocated internal buffer, with size
		 * `max_buf_len`.
		 * It will also place `ext_buffer_begin`, `buffer_data_begin` and
		 * `buffer_begin` to the right positions.
		 *
		 * \return true if allocation is successful, false otherwise.
		 */
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

		/*! Construct a composed CBOR object with a DYNAMIC_INTERNAL buffer, big
		 * enough to fit one element of one byte.
		 */
		CBORComposed()
		{
			//Reserve enough memmory for one element of one byte
			max_buf_len = 10;
			init_buffer();

			//Initialize num_ele
			init_num_ele(0);
		}

		//! Construct a composed CBOR object with a custom length DYNAMIC_INTERNAL buffer.
		/*!
		 * The total length of the alocated buffer is `buf_len + NUM_ELE_PROVISION`.
		 *
		 * \param buf_len Buffer size, in bytes, of the data section of the buffer.
		 */
		CBORComposed(size_t buf_len)
		{
			//Reserve buffer
			max_buf_len = buf_len + NUM_ELE_PROVISION;
			init_buffer();

			//Initialize num_ele
			init_num_ele(0);
		}

		//! Copy constructor.
		CBORComposed(const CBORComposed &obj) : CBOR()
		{
			uint8_t type_num_len = compute_type_num_len(obj.n_elements());
			//Reserve buf_len plus maximum size of a type_num : 9 bytes to encode a
			//table length up to (2^64)-1
			size_t buf_len_needed = obj.length() - type_num_len + NUM_ELE_PROVISION;

			//Reserve buffer
			max_buf_len = buf_len_needed;
			init_buffer();

			//Initialize num_ele
			init_num_ele(obj.n_elements());

			//Copy data
			memcpy(buffer_data_begin, obj.to_CBOR() + type_num_len, obj.length() - type_num_len);

			//Jump to the end of the data chunk
			w_ptr = buffer_begin + obj.length();
		}

	public:
		//! Destructor
		~CBORComposed()
		{
			if(buffer_type == BUFFER_DYNAMIC_INTERNAL) {
				free(ext_buffer_begin);
				buffer_begin = NULL;
			}
		}

		//! Reserve some space in the buffer.
		/*
		 * - If buffer is DYNAMIC_INTERNAL, then reserve will do necessary
		 * reallocation to accomodate for the total length given in parameter
		 * (if needed).
		 * - If buffer is EXTERNAL or STATIC_INTERNAL, then reserve will do nothing.
		 *
		 * \param length The requested buffer length.
		 * \return True if buffer is large enough, or if reallocation was successful.
		 */
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

		//! Get the maximum number of elements that can fit in this composed CBOR object.
		/*!
		 * \return The maximum number of elements that can fit in this composed
		 * CBOR object.
		 */
		size_t max_n_elements() const { return (1<<(buffer_data_begin - ext_buffer_begin)); }
};

#endif
