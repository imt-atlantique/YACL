#ifndef INCLUDED_CBORARRAY_H
#define INCLUDED_CBORARRAY_H

#include "CBORComposed.h"

//! A class to handle CBOR Arrays.
/*!
 * This class handles encoding of CBOR data into a CBOR Array.
 */
class CBORArray: public CBORComposed<CBOR_ARRAY>
{
	public:
		/*!
		 * Construct a CBOR ARRAY with a DYNAMIC_INTERNAL buffer, big
		 * enough to fit one element of one byte.
		 */
		CBORArray() : CBORComposed() {};

		//! Construct a CBOR array with a custom length DYNAMIC_INTERNAL buffer.
		/*!
		 * The total length of the alocated buffer is `buf_len - 1 + NUM_ELE_PROVISION`.
		 *
		 * \param buf_len Buffer size, in bytes, of the data section of the buffer.
		 */
		CBORArray(size_t buf_len) : CBORComposed(buf_len) {};

		//! Copy constructor.
		CBORArray(const CBORArray &obj) : CBORComposed(obj) {};

		//! Construct a CBOR array using an external buffer.
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
		CBORArray(uint8_t* _buffer, size_t buf_len, bool has_data = true);

		//! Parse and create a CBOR array from a byte array of CBOR data (a copy is performed).
		/*!
		 * \param buffer Pointer to the beginning of the array.
		 * \param buffer_len Size (in bytes) of the array.
		 */
		CBORArray(const uint8_t* _buffer, size_t buf_len);

		//! Appends CBOR NULL to the end of this CBOR ARRAY.
		/*!
		 * \return True if the operation was successful, false otherwise.
		 */
		bool append();

		//! Appends an element to the end of this CBOR ARRAY.
		/*!
		 * \param value The element to append to this CBOR ARRAY.
		 * \return True if the operation was successful, false otherwise.
		 */
		template <typename T> bool append(T value)
		{
			increment_num_ele();
			return add(value);
		}

		//! Appends multiple elements to the end of this CBOR ARRAY.
		/*!
		 * \param array Pointer to the begining of the array containing the
		 * elements to append.
		 * \param size Number of elements in `array`.
		 * \return True if the operation was successful, false otherwise.
		 */
		template <typename T> bool append(const T *array, size_t size)
		{
			bool ret_val = true;

			init_num_ele(n_elements() + size);
			for (const T *ptr = array ; ptr < (array+size) ; ++ptr) {
				ret_val &= add(*ptr);
			}

			return ret_val;
		}
};

#endif
