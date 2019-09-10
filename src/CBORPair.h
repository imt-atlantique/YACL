#ifndef INCLUDED_CBORPAIR_H
#define INCLUDED_CBORPAIR_H

#include "CBORComposed.h"

//! A class to handle CBOR dictionaries of key/value pairs.
/*!
 * This class handles encoding and decoding of CBOR data key/values into a
 * CBOR Array dictionaries.
 */
class CBORPair: public CBORComposed<CBOR_MAP>
{
	protected:
		//! Check if two buffers store the same information.
		/*!
		 * Check that the two buffers have same length, and that they store
		 * the same information.
		 *
		 * \param buf1 First buffer.
		 * \param len_buf1 Number of elements in first buffer.
		 * \param buf2 Second buffer.
		 * \param len_buf2 Number of elements in second buffer.
		 */
		static bool buffer_equals(const uint8_t* buf1, size_t len_buf1,
				const uint8_t* buf2, size_t len_buf2);
	public:
		/*!
		 * Construct a CBOR PAIR with a DYNAMIC_INTERNAL buffer, big
		 * enough to fit one element of one byte.
		 */
		CBORPair() : CBORComposed() {};

		//! Construct a CBOR PAIR with a custom length DYNAMIC_INTERNAL buffer.
		/*!
		 * The total length of the alocated buffer is `buf_len - 1 + NUM_ELE_PROVISION`.
		 *
		 * \param buf_len Buffer size, in bytes, of the data section of the buffer.
		 */
		CBORPair(size_t buf_len) : CBORComposed(buf_len) {};

		//! Copy constructor.
		CBORPair(const CBORPair &obj) : CBORComposed(obj) {};

		//! Construct a CBOR PAIR using an external buffer.
		/*!
		 * If has_data == true, then new elements can be added.
		 * However, the size of the length field cannot be expanded
		 * (expect less than 2^NUM_ELE_PROVISION-1 elements capacity).
		 *
		 * \param buffer Pointer to the beginning of the external buffer.
		 * \param buffer_len Size (in bytes) of the external buffer.
		 * \param has_data True if external buffer already contains a CBOR object,
		 *  false otherwise.
		 */
		CBORPair(uint8_t* _buffer, size_t buf_len, bool has_data = true);

		//! Parse and create a CBOR pair from a byte array of CBOR data (a copy is performed).
		/*!
		 * \param buffer Pointer to the beginning of the array.
		 * \param buffer_len Size (in bytes) of the array.
		 */
		CBORPair(const uint8_t* _buffer, size_t buf_len);

		//! Construct a CBOR PAIR from a CBOR object.
		/*!
		 * Behaviour when obj does not contain a CBOR PAIR is undefined.
		 * Note that this is not a copy constructor. This object and `obj` share
		 * the same buffer for storing information.
		 *
		 * New elements can be added.
		 * However, the size of the length field cannot be expanded
		 * (expect less than 2^NUM_ELE_PROVISION-1 elements capacity).
		 *
		 * \param obj A CBOR object containing a CBOR PAIR.
		 */
		CBORPair(CBOR &obj);

		//! Appends an element to the end of this CBOR PAIR.
		/*!
		 * \param key The key of the element to append to this CBOR PAIR.
		 * \param value The value element to append to this CBOR PAIR.
		 * \return True if the operation was successful, false otherwise.
		 */
		template <typename T, typename U> bool append(T key, U value)
		{
			bool ret_val = true;

			increment_num_ele();
			ret_val = add(key);
			ret_val &= add(value);

			return ret_val;
		}

		//! Returns the CBOR value associated with a particular key.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the value at key `key`.
		 * Note that, as there is sometimes different CBOR representation of
		 * a single value (e.g.: 4 can be coded with (u)int{8,16,32,64}), it may
		 * be required to construct a CBOR object and pass it as the key.
		 * If the CBOR PAIR object has two values associated with the same key,
		 * this operator will return the first one in order of appearence in the
		 * data buffer.
		 *
		 * \param key The key of the CBOR value to be retrieved.
		 * \return The retrieved CBOR value, or a CBOR NULL if `key` cannot be
		 * found or if this object does not actually stores a CBOR PAIR in its
		 * buffer.
		 */
		template <typename T> CBOR operator[](T key)
		{
			if (!is_pair()) {
				return CBOR();
			}

			CBOR idx_cbor = CBOR(key);
			const uint8_t *idx_cbor_buffer = idx_cbor.to_CBOR();
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

		//! Returns the CBOR value located at an index.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the value at index `idx`.
		 *
		 * \param idx The index of the CBOR value to be retrieved.
		 * \return The retrieved object, or a CBOR NULL if `idx` is out of range
		 * or if this object does not actually stores a CBOR PAIR in its buffer.
		 */
		CBOR at(size_t idx);

		//! Returns the CBOR key located at an index.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the key at index `idx`.
		 *
		 * \param idx The index of the CBOR key to be retrieved.
		 * \return The retrieved object, or a CBOR NULL if `idx` is out of range
		 * or if this object does not actually stores a CBOR PAIR in its buffer.
		 */
		CBOR key_at(size_t idx);
};

#endif

