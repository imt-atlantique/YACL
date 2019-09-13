#ifndef INCLUDED_CBORPAIR_H
#define INCLUDED_CBORPAIR_H

#include "CBORComposed.h"

//! A class to handle CBOR dictionaries of key/value pairs.
/*!
 * This class handles encoding of CBOR data key/values into a
 * CBOR Array dictionaries.
 */
class CBORPair: public CBORComposed<CBOR_MAP>
{
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
};

#endif
