#ifndef INCLUDED_CBOR_H
#define INCLUDED_CBOR_H

#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <WString.h>

#define CBOR_TYPE_MASK 0xE0
#define CBOR_INFO_BITS 0x1F

#define CBOR_UINT    0x00
#define CBOR_NEGINT  0x20
#define CBOR_BYTES   0x40
#define CBOR_TEXT    0x60
#define CBOR_ARRAY   0x80
#define CBOR_MAP     0xA0
#define CBOR_TAG     0xC0
//float and other types
#define CBOR_7       0xE0

#define CBOR_UINT8_FOLLOWS  24  //0x18
#define CBOR_UINT16_FOLLOWS 25  //0x19
#define CBOR_UINT32_FOLLOWS 26  //0x1a
#define CBOR_UINT64_FOLLOWS 27  //0x1b
#define CBOR_VAR_FOLLOWS    31  //0x1f

#define CBOR_BREAK  0xFF

#define CBOR_FALSE  (CBOR_7 | 20)
#define CBOR_TRUE   (CBOR_7 | 21)
#define CBOR_NULL   (CBOR_7 | 22)
//js 'undefined' value
#define CBOR_UNDEFINED   (CBOR_7 | 23)

#define CBOR_FLOAT16 (CBOR_7 | 25)
#define CBOR_FLOAT32 (CBOR_7 | 26)
#define CBOR_FLOAT64 (CBOR_7 | 27)

#define STATIC_ALLOC_SIZE 9
#define BUFFER_STATIC_INTERNAL 0
#define BUFFER_DYNAMIC_INTERNAL 1
#define BUFFER_EXTERNAL 2

//! A class to handle CBOR Objects.
/*!
 * This class handles encoding and decoding of basic CBOR data (int, float, strings).
 * It is also the base class from which any CBOR encoder/decoder object inherits
 * (for more elaborated data like array or map).
 */
class CBOR
{
	protected:
		//! Pointer on the begining of the buffer storing CBOR data.
		union {
			//! Used for basic types.
			uint8_t static_buffer_begin[STATIC_ALLOC_SIZE];
			//! Used for dynamic allocation (such as array and maps) and external buffers.
			uint8_t *buffer_begin;
		};
		//! Write pointer.
		uint8_t *w_ptr = static_buffer_begin;
		//! Maximum buffer length.
		size_t max_buf_len = STATIC_ALLOC_SIZE;
		//! Buffer type.
		/*!
		 * Buffer type can be:
		 * - `BUFFER_STATIC_INTERNAL` if using the statically allocated internal buffer.
		 * - `BUFFER_DYNAMIC_INTERNAL` if using a dynamically allocated internal buffer.
		 * - `BUFFER_EXTERNAL` if using an external buffer.
		 */
		uint8_t buffer_type = BUFFER_STATIC_INTERNAL;

		//! Returns pointer on the begining of the buffer.
		/*!
		 * \return The right pointer on the begining of the buffer, depending on `buffer_type`.
		 */
		uint8_t* get_buffer_begin() {
			return (buffer_type == BUFFER_STATIC_INTERNAL)?static_buffer_begin:buffer_begin;
		};
		//! Initialize a dynamically allocated internal buffer, with size `max_buf_len`.
		/*!
		 * \return true if allocation is successful, false otherwise.
		 */
		virtual bool init_buffer();

		//! Encode a numerical value associated with a CBOR type, and append it to the beginning of the buffer.
		/*!
		 * \param cbor_type CBOR type to associate with the numerical value.
		 * \param val Numerical value to be encoded.
		 * \return false if anything went wrong.
		 */
		bool encode_type_num(uint8_t cbor_type, uint8_t val);
		bool encode_type_num(uint8_t cbor_type, uint16_t val);
		bool encode_type_num(uint8_t cbor_type, uint32_t val);
		bool encode_type_num(uint8_t cbor_type, uint64_t val);

		//! Determine if this CBOR object is a negative integer.
		/*!
		 * \return true if the CBOR type of this CBOR object is CBOR_NEGINT (0x20).
		 */
		bool is_neg_num() const;

		//! Interpret the value pointed by buffer as a numerical value spanning 8 bits.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return The decoded numerical value.
		 */
		static uint8_t decode_abs_num8(const uint8_t *ptr);
		//! Interpret the value pointed by buffer as a numerical value spanning 16 bits.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return The decoded numerical value.
		 */
		static uint16_t decode_abs_num16(const uint8_t *ptr);
		//! Interpret the value pointed by buffer as a numerical value spanning 32 bits.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return The decoded numerical value.
		 */
		static uint32_t decode_abs_num32(const uint8_t *ptr);
		//! Interpret the value pointed by buffer as a numerical value spanning 64 bits.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return The decoded numerical value.
		 */
		static uint64_t decode_abs_num64(const uint8_t *ptr);
		//! Interpret the value pointed by buffer as an integer numerical value.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return The decoded numerical value.
		 */
		static size_t decode_abs_num(const uint8_t *ptr);

		//! Compute the number of bytes needed to encode the numerical value `num_ele`.
		/*!
		 * \param num_ele The numerical value to be encoded.
		 * \return The number of bytes needed to encode the numerical value `num_ele`.
		 */
		static uint8_t compute_type_num_len(size_t num_ele);

		//! Returns the size of the CBOR element pointed by ptr.
		/*!
		 * \param ptr Pointer to the begining of the element in buffer.
		 * \return the size of the CBOR element pointed by ptr.
		 */
		static size_t element_size(uint8_t *ptr);

		//! Add a CBOR NULL at the end of the buffer.
		bool add();
		//! Add a CBOR BOOL at the end of the buffer.
		/*!
		 * \param value The boolean value to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(bool value);

		//! Add a CBOR UINT8 at the end of the buffer.
		/*!
		 * \param value The 8-bit positive integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(uint8_t value);
		//! Add a CBOR UINT16 at the end of the buffer.
		/*!
		 * \param value The 16-bit positive integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(uint16_t value);
		//! Add a CBOR UINT32 at the end of the buffer.
		/*!
		 * \param value The 32-bit positive integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(uint32_t value);
		//! Add a CBOR UINT64 at the end of the buffer.
		/*!
		 * \param value The 64-bit positive integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(uint64_t value);
		//! Add CBOR INT8 at the end of the buffer.
		/*!
		 * \param value The 8-bit integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(int8_t value);
		//! Add a CBOR INT16 at the end of the buffer.
		/*!
		 * \param value The 16-bit integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(int16_t value);
		//! Add a CBOR INT16 at the end of the buffer.
		/*!
		 * \param value The 32-bit integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(int32_t value);
		//! Add a CBOR INT16 at the end of the buffer.
		/*!
		 * \param value The 64-bit integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(int64_t value);

		//! Add a CBOR FLOAT32 at the end of the buffer.
		/*!
		 * \param value The 32-bit floating point number to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(float value);
		//! Add a CBOR FLOAT64 at the end of the buffer.
		/*!
		 * \param value The 64-bit floating point number to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(double value);

		//! Add a CBOR TEXT at the end of the buffer.
		/*!
		 * \param value The string to be added to this CBOR object.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(const char* value);

		//! Add a CBOR at the end of the buffer.
		/*!
		 * \param value The CBOR object to be added to this CBOR object.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(const CBOR &value);
	public:
		//Constructors for primitive types
		//! Construct a stack-allocated NULL CBOR object.
		CBOR();
		//! Construct a stack-allocated Boolean CBOR object.
		/*!
		 * \param value The boolean value to encode.
		 */
		CBOR(bool value);
		//! Construct a stack-allocated CBOR UINT8 object.
		/*!
		 * \param value The 8-bit positive integer to encode.
		 */
		CBOR(uint8_t value);
		//! Construct a stack-allocated CBOR UINT16 object.
		/*!
		 * \param value The 16-bit positive integer to encode.
		 */
		CBOR(uint16_t value);
		//! Construct a stack-allocated CBOR UINT32 object.
		/*!
		 * \param value The 32-bit positive integer to encode.
		 */
		CBOR(uint32_t value);
		//! Construct a stack-allocated CBOR UINT64 object.
		/*!
		 * \param value The 64-bit positive integer to encode.
		 */
		CBOR(uint64_t value);
		//! Construct a stack-allocated CBOR INT8 object.
		/*!
		 * \param value The 8-bit integer to encode.
		 */
		CBOR(int8_t value);
		//! Construct a stack-allocated CBOR INT16 object.
		/*!
		 * \param value The 16-bit integer to encode.
		 */
		CBOR(int16_t value);
		//! Construct a stack-allocated CBOR INT32 object.
		/*!
		 * \param value The 32-bit integer to encode.
		 */
		CBOR(int32_t value);
		//! Construct a stack-allocated CBOR INT64 object.
		/*!
		 * \param value The 64-bit integer to encode.
		 */
		CBOR(int64_t value);
		//! Construct a stack-allocated CBOR FLOAT32 object.
		/*!
		 * \param value The 32-bit floating point number to encode.
		 */
		CBOR(float value);
		//! Construct a stack-allocated CBOR FLOAT64 object.
		/*!
		 * \param value The 64-bit floating point number to encode.
		 */
		CBOR(double value);
		//! Construct a stack-allocated CBOR TEXT object.
		/*!
		 * \param value The string to be added to this CBOR object.
		 */
		CBOR(const char* value);

		//! Construct a CBOR object using an external buffer.
		/*!
		 * \param buffer Pointer to the beginning of the external buffer.
		 * \param buffer_len Size (in bytes) of the external buffer.
		 * \param has_data True if external buffer already contains a CBOR object,
		 *  false otherwise.
		 */
		CBOR(uint8_t* buffer, size_t buffer_len, bool has_data = true);

		//! Copy constructor.
		CBOR(const CBOR &obj);

		//! Destructor.
		~CBOR();

		//! Reserve some space in the buffer.
		/*
		 * - If buffer is DYNAMIC_INTERNAL, then reserve will do necessary
		 * reallocation to accomodate for the total length given in parameter
		 * (if needed).
		 * - If buffer is STATIC_INTERNAL, then reserve will first check if the
		 * requested length require allocation of a DYNAMIC_INTERNAL buffer. If
		 * yes, it will do the necessary allocation and copy. Otherwise, it will
		 * do nothing.
		 * - If buffer is EXTERNAL, then reserve will do nothing.
		 *
		 * \param length The requested buffer length.
		 * \return True if buffer is large enough, or if reallocation was successful.
		 */
		virtual bool reserve(size_t length);

		//! Get the length of this CBOR message
		/*
		 * \return The length of this CBOR message.
		 */
		size_t length() const { return (size_t)(w_ptr - get_buffer_begin()); }

		//! Returns this CBOR object as an array of bytes.
		/*
		 * \return This CBOR object as an array of bytes (pointer to the
		 * beginning of the buffer).
		 */
		const uint8_t* to_CBOR() { return get_buffer_begin(); }

		//! Return true if the CBOR object is a CBOR NULL.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_null(const uint8_t* buffer);
		//! Return true if the CBOR object is a CBOR BOOL.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_bool(const uint8_t* buffer);
		//! Return true if the CBOR object is an unsigned integer that fits into an uint8_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_uint8(const uint8_t* buffer);
		//! Return true if the CBOR object is an unsigned integer that fits into an uint16_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_uint16(const uint8_t* buffer);
		//! Return true if the CBOR object is an unsigned integer that fits into an uint32_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_uint32(const uint8_t* buffer);
		//! Return true if the CBOR object is an unsigned integer that fits into an uint64_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_uint64(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into an int8_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_int8(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into an int16_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_int16(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into an int32_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_int32(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into an int64_t.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_int64(const uint8_t* buffer);
		//! Return true if the CBOR object is a CBOR FLOAT16.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_float16(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into 32-bit float.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_float32(const uint8_t* buffer);
		//! Return true if the CBOR object is an integer that fits into 64-bit float.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_float64(const uint8_t* buffer);
		//! Return true if the CBOR object is a string.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_string(const uint8_t* buffer);
		//! Return true if the CBOR object is an array.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_array(const uint8_t* buffer);
		//! Return true if the CBOR object is a dictionnary of key/value pairs.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_pair(const uint8_t* buffer);

		//! Return true if this CBOR object is a CBOR NULL.
		bool is_null() const {return is_null(get_buffer_begin()); };
		//! Return true if this CBOR object is a CBOR BOOL.
		bool is_bool() const {return is_bool(get_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint8_t.
		bool is_uint8() const {return is_uint8(get_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint16_t.
		bool is_uint16() const {return is_uint16(get_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint32_t.
		bool is_uint32() const {return is_uint32(get_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint64_t.
		bool is_uint64() const {return is_uint64(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int8_t.
		bool is_int8() const {return is_int8(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int16_t.
		bool is_int16() const {return is_int16(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int32_t.
		bool is_int32() const {return is_int32(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int64_t.
		bool is_int64() const {return is_int64(get_buffer_begin()); };
		//! Return true if this CBOR object is a CBOR FLOAT16.
		bool is_float16() const {return is_float16(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into 32-bit float.
		bool is_float32() const {return is_float32(get_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into 64-bit float.
		bool is_float64() const {return is_float64(get_buffer_begin()); };
		//! Return true if this CBOR object is a string.
		bool is_string() const {return is_string(get_buffer_begin()); };
		//! Return true if this CBOR object is an array.
		bool is_array() const {return is_array(get_buffer_begin()); };
		//! Return true if this CBOR object is a dictionnary of key/value pairs.
		bool is_pair() const {return is_pair(get_buffer_begin()); };

		//! Convert this CBOR object to a boolean.
		/*!
		 * Output of this operator when this CBOR object is not a CBOR BOOl is undefined.
		 */
		operator bool() const;
		//! Convert this CBOR object to an uint8_t.
		/*!
		 * Output of this operator when this CBOR object is not an unsigned
		 * integer that fits on 8 bits is undefined.
		 */
		operator uint8_t() const;
		//! Convert this CBOR object to an uint16_t.
		/*!
		 * Output of this operator when	this CBOR object is not an unsigned
		 * integer that fits on 16 bits is undefined.
		 */
		operator uint16_t() const;
		//! Convert this CBOR object to an uint32_t.
		/*!
		 * Output of this operator when this CBOR object is not an unsigned
		 * integer that fits on 32 bits is undefined.
		 */
		operator uint32_t() const;
		//! Convert this CBOR object to an uint64_t.
		/*!
		 * Output of this operator when this CBOR object is not an unsigned
		 * integer that fits on 64 bits is undefined.
		 */
		operator uint64_t() const;
		//! Convert this CBOR object to an int8_t.
		/*!
		 * Output of this operator when this CBOR object is not an integer that
		 * fits on 8 bits is undefined.
		 */
		operator int8_t() const;
		//! Convert this CBOR object to an int16_t.
		/*!
		 * Output of this operator when this CBOR object is not an integer that
		 * fits on 16 bits is undefined.
		 */
		operator int16_t() const;
		//! Convert this CBOR object to an int32_t.
		/*!
		 * Output of this operator when this CBOR object is not an integer that
		 * fits on 32 bits is undefined.
		 */
		operator int32_t() const;
		//! Convert this CBOR object to an int64_t.
		/*!
		 * Output of this operator when this CBOR object is not an integer that
		 * fits on 64 bits is undefined.
		 */
		operator int64_t() const;
		//! Convert this CBOR object to a float.
		/*!
		 * Output of this operator when this CBOR object is not a floating point
		 * number that fits in a float is undefined.
		 */
		operator float() const;
		//! Convert this CBOR object to a double.
		/*!
		 * Output of this operator when this CBOR object is not a floating point
		 * number that fits in a double is undefined.
		 */
		operator double() const;
		//! When this CBOR object is a CBOR STRING, returns this string length.
		/*!
		 * Output of this function when this CBOR object is not a string is undefined.
		 *
		 * \return Length of a CBOR string.
		 */
		size_t get_string_len() const;
		//! When this CBOR object is a CBOR STRING, copies it to this one passed as a parameter.
		/*!
		 * Behavior of this function when this CBOR object is not a string is undefined.
		 *
		 * \param str Buffer into which this CBOR string is copied.
		 */
		void get_string(char* str) const;
		//! When this CBOR object is a CBOR STRING, copies it to this one passed as a parameter.
		/*!
		 * Behavior of this function when this CBOR object is not a string is undefined.
		 *
		 * \param str Buffer into which this CBOR string is copied.
		 */
		void get_string(String& str) const;
		//! When this CBOR object is a CBOR STRING, return this string as an Arduino String object.
		/*!
		 * Behavior of this function when this CBOR object is not a string is undefined.
		 *
		 * \returns this CBOR string as an Arduino String object.
		 */
		String to_string() const;
};

#endif
