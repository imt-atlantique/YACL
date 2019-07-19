#ifndef INCLUDED_CBORARRAY_H
#define INCLUDED_CBORARRAY_H

#include "CBORComposed.h"

//! A class to handle CBOR Arrays.
/*!
 * This class handles encoding and decoding of CBOR data into a CBOR Array.
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

		//! Construct a CBOR array from a CBOR object.
		/*!
		 * Behaviour when obj does not contain a CBOR array is undefined.
		 * Note that this is not a copy constructor. This object and `obj` share
		 * the same buffer for storing information.
		 *
		 * New elements can be added.
		 * However, the size of the length field cannot be expanded
		 * (expect less than 2^NUM_ELE_PROVISION-1 elements capacity).
		 *
		 * \param obj A CBOR object containing a CBOR ARRAY.
		 */
		CBORArray(const CBOR &obj);

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
			for (T *ptr = array ; ptr < (array+size) ; ++ptr) {
				ret_val &= add(*ptr);
			}

			return ret_val;
		}

		//! Returns the CBOR object located at an index.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the value at index `idx`.
		 *
		 * \param idx The index of the CBOR element to be retrieved.
		 * \return The retrieved object, or a CBOR NULL if `idx` is out of range
		 * or if this object does not actually stores an array in its buffer.
		 */
		CBOR operator[](size_t idx);
};

#endif
