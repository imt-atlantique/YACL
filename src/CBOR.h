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

		//! Returns const pointer on the begining of the buffer.
		/*!
		 * \return The right const pointer on the begining of the buffer,
		 * depending on `buffer_type`.
		 */
		const uint8_t* get_const_buffer_begin() const {
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
		template <typename T> bool encode_type_num(uint8_t cbor_type, T value)
		{
			if (sizeof(T) == 1) {
				return encode_type_num(cbor_type, (uint8_t)value);
			}
			else if (sizeof(T) == 2) {
				return encode_type_num(cbor_type, (uint16_t)value);
			}
			else if (sizeof(T) == 3) {
				return encode_type_num(cbor_type, (uint32_t)value);
			}
			else if (sizeof(T) == 4) {
				return encode_type_num(cbor_type, (uint64_t)value);
			}
		}

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
		 * \return The decoded numerical value, or 0 if decoding fails.
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

		//! Add a CBOR NULL at the end of the buffer.
		bool add();
		//! Add a CBOR BOOL at the end of the buffer.
		/*!
		 * \param value The boolean value to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(bool value);

		//! Add a CBOR UINT at the end of the buffer.
		/*!
		 * Add the smallest-length CBOR UINT representation of the value at the
		 * end of the buffer.
		 *
		 * \param value The positive integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(uint8_t value) { return encode_type_num(CBOR_UINT, value); }
		bool add(uint16_t value) { return encode_type_num(CBOR_UINT, value); }
		bool add(uint32_t value) { return encode_type_num(CBOR_UINT, value); }
		bool add(uint64_t value) { return encode_type_num(CBOR_UINT, value); }

		//! Add a CBOR INT at the end of the buffer.
		/*!
		 * Add the smallest-length CBOR INT (or UINT, depending on the sign)
		 * representation of the value at the end of the buffer.
		 *
		 * \param value The 8-bit integer to encode.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(int8_t value);
		bool add(int16_t value);
		bool add(int32_t value);
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

		//! Add a CBOR TEXT (UTF-8 String) at the end of the buffer.
		/*!
		 * \param value The string to be added to this CBOR object.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(const char* value);

		//! Add a CBOR BYTES (Byte String) at the end of the buffer.
		/*!
		 * \param value The byte string to be added to this CBOR object.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(const uint8_t* value, size_t len);

		//! Add a CBOR at the end of the buffer.
		/*!
		 * \param value The CBOR object to be added to this CBOR object.
		 * \return False if anything goes wrong. True otherwise.
		 */
		bool add(const CBOR &value);

		//! Add a CBOR TAG at the end of the buffer.
		/*!
		 * \param tag_value Tag value to be encoded.
		 * \param tag_item The tagged CBOR item.
		 */
		template <typename T> bool add(T tag_value, const CBOR& tag_item)
		{
			bool status = false;

			status |= encode_type_num(CBOR_TAG, tag_value);
			status |= add(tag_item);

			return status;
		}

		/*!
		 * Helper function for operator[]: when index is a non-float numeric,
		 * then operator[] can call at() for a CBOR ARRAY, or find_by_key() for
		 * a CBOR PAIR.
		 * However, when index is not numeric of float, then this CBOR object
		 * can only be a CBOR PAIR, and only find_by_key() should be called.
		 */
		template <typename T> CBOR access_op_numeric(T idx)
		{
			if (is_array()) {
				return at(idx);
			}

			if (is_pair()) {
				return find_by_key(idx);
			}

			//Not a CBOR PAIR nor a CBOR ARRAY
			return CBOR();
		}

	public:
		//Constructors for primitive types
		//! Construct a stack-allocated NULL CBOR object.
		CBOR() { add(); };

		//! Construct a stack-allocated CBOR object.
		/*!
		 * \param value The value to encode.
		 */
		template <typename T> CBOR(T value) { add(value); }

		//! Constructor for tagged CBOR objects.
		/*!
		 * \param tag_value Tag value to be encoded.
		 * \param tag_item The tagged CBOR item.
		 */
		CBOR(char tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(short tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(int tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(long tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(unsigned char tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(unsigned short tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(unsigned int tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}
		CBOR(unsigned long tag_value, const CBOR& tag_item) { add(tag_value, tag_item);}

		//! Construct a CBOR object using an external buffer.
		/*!
		 * \param buffer Pointer to the beginning of the external buffer.
		 * \param buffer_len Size (in bytes) of the external buffer.
		 * \param has_data True if external buffer already contains a CBOR object,
		 *  false otherwise.
		 */
		CBOR(uint8_t* buffer, size_t buffer_len, bool has_data = true);

		//! Parse and create a CBOR object from a byte array of CBOR data (a copy is performed).
		/*!
		 * \param buffer Pointer to the beginning of the array.
		 * \param buffer_len Size (in bytes) of the array.
		 */
		CBOR(const uint8_t* buffer, size_t buffer_len);

		//! Copy constructor.
		CBOR(const CBOR &obj);

		//! Destructor.
		~CBOR();

		//! Replace the value encoded in this CBOR object with another one.
		/*
		 * Replace the value encoded in this CBOR object with the CBOR encoded
		 * version of parameter "value".
		 * Do not use this method with CBORArray and CBORPair.
		 *
		 * \param value The value to encode.
		 */
		template <typename T> bool encode(T value)
		{
			w_ptr = get_buffer_begin();
			return add(value);
		}

		//! Specialization for tags
		bool encode(char tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(short tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(int tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(long tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }

		bool encode(unsigned char tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(unsigned short tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(unsigned int tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }
		bool encode(unsigned long tag_value, const CBOR& tag_item)
		{ w_ptr = get_buffer_begin(); return add(tag_value, tag_item); }

		//! Specialization for byte strings.
		bool encode(const uint8_t* value, size_t len)
		{ w_ptr = get_buffer_begin(); return add(value, len); }


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
		size_t length() const { return (size_t)(w_ptr - get_const_buffer_begin()); }

		//! Returns this CBOR object as an array of bytes.
		/*
		 * \return This CBOR object as an array of bytes (const pointer to the
		 * beginning of the buffer).
		 */
		const uint8_t* to_CBOR() const { return get_const_buffer_begin(); }

		//! Returns this CBOR object as an array of bytes (alterable).
		/*
		 * \return This CBOR object as an array of bytes (pointer to the
		 * beginning of the buffer).
		 */
		uint8_t* get_buffer() { return get_buffer_begin(); }

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
		//! Return true if the CBOR object is a byte string.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_bytestring(const uint8_t* buffer);
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
		//! Return true if the CBOR object is a tagged CBOR object.
		/*
		 * \param buffer Pointer to the begining of the buffer containing the CBOR object.
		 */
		static bool is_tag(const uint8_t* buffer);

		//! Return true if this CBOR object is a CBOR NULL.
		bool is_null() const {return is_null(get_const_buffer_begin()); };
		//! Return true if this CBOR object is a CBOR BOOL.
		bool is_bool() const {return is_bool(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint8_t.
		bool is_uint8() const {return is_uint8(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint16_t.
		bool is_uint16() const {return is_uint16(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint32_t.
		bool is_uint32() const {return is_uint32(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an unsigned integer that fits into an uint64_t.
		bool is_uint64() const {return is_uint64(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int8_t.
		bool is_int8() const {return is_int8(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int16_t.
		bool is_int16() const {return is_int16(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int32_t.
		bool is_int32() const {return is_int32(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into an int64_t.
		bool is_int64() const {return is_int64(get_const_buffer_begin()); };
		//! Return true if this CBOR object is a CBOR FLOAT16.
		bool is_float16() const {return is_float16(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into 32-bit float.
		bool is_float32() const {return is_float32(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an integer that fits into 64-bit float.
		bool is_float64() const {return is_float64(get_const_buffer_begin()); };
		//! Return true if this CBOR object is a string.
		bool is_string() const {return is_string(get_const_buffer_begin()); };
		//! Return true if this CBOR object is a byte string.
		bool is_bytestring() const{return is_bytestring(get_const_buffer_begin()); };
		//! Return true if this CBOR object is an array.
		bool is_array() const {return is_array(get_const_buffer_begin()); };
		//! Return true if this CBOR object is a dictionnary of key/value pairs.
		bool is_pair() const {return is_pair(get_const_buffer_begin()); };
		//! Return true if the CBOR object is a tagged CBOR object.
		bool is_tag() const {return is_tag(get_const_buffer_begin()); };

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
		size_t get_string_len() const
		{
			return decode_abs_num(get_const_buffer_begin());
		}
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
		 * The string passed as a paramter must have enough capacity to store
		 * the string, otherwise, memory corruption will occur.
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
		//! When this CBOR object is a CBOR BYTE STRING, returns this byte string length.
		/*!
		 * Output of this function when this CBOR object is not a byte string is undefined.
		 *
		 * \return Length of a CBOR byte string.
		 */
		size_t get_bytestring_len() const
		{
			return decode_abs_num(get_const_buffer_begin());
		}
		//! When this CBOR object is a CBOR BYTE STRING, copies it to this one passed as a parameter.
		/*!
		 * Behavior of this function when this CBOR object is not a byte string is undefined.
		 * The string passed as a paramter must have enough capacity to store
		 * the string, otherwise, memory corruption will occur.
		 *
		 * \param str Buffer into which this CBOR byte string is copied.
		 */
		void get_bytestring(uint8_t* bytestr) const;
		//! When this CBOR object is a CBOR TAG, return the tag value.
		/*!
		 * Output of this operator when this CBOR object is not a CBOR TAG
		 *  is undefined.
		 *
		 *  \return The tag value of this CBOR TAG.
		 */
		size_t get_tag_value() const;
		//! When this CBOR object is a CBOR TAG, return the tag item.
		/*!
		 * Output of this operator when this CBOR object is not a CBOR TAG
		 *  is undefined.
		 *
		 *  \return The tag item of this CBOR TAG.
		 */
		CBOR get_tag_item();

		//! Get the number of elements in this composed CBOR object.
		/*!
		 * \return The number of elements in this composed CBOR object.
		 */
		size_t n_elements() const;

		//! Returns the CBOR value located at an index. Use for CBOR ARRAY and CBOR PAIR.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the value at index `idx`.
		 * Behaviour is undefined if `idx` is non-numeric.
		 *
		 * \param idx The index of the CBOR value to be retrieved.
		 * \return The retrieved object, or a CBOR NULL if `idx` is negative,
		 * out of range or if this object does not actually stores a CBOR PAIR
		 * or a CBOR ARRAY in its buffer.
		 */
		template <typename T> CBOR at(T idx)
		{
			size_t n_elements = decode_abs_num(get_const_buffer_begin());
			uint8_t *ele_begin = get_buffer_begin() + compute_type_num_len(n_elements);

			if ((!is_pair() && !is_array()) || (idx > n_elements) || (idx < 0)) {
				return CBOR();
			}

			//Jump to the reffered value
			if (is_pair()) {
				ele_begin += element_size(ele_begin);
				for (T i=0 ; i < idx ; ++i) {
					ele_begin += element_size(ele_begin);
					ele_begin += element_size(ele_begin);
				}
			}
			else { //is_array()
				for (T i=0 ; i < idx ; ++i) {
					ele_begin += element_size(ele_begin);
				}
			}

			return CBOR(ele_begin, element_size(ele_begin), true);
		}

		//! Returns the CBOR key located at an index. Use for CBOR PAIR.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the key at index `idx`.
		 * Behaviour is undefined if `idx` is non-numeric.
		 *
		 * \param idx The index of the CBOR key to be retrieved.
		 * \return The retrieved object, or a CBOR NULL if `idx` is negative,
		 * out of range or if this object does not actually stores a CBOR PAIR
		 * in its buffer.
		 */
		template <typename T> CBOR key_at(T idx)
		{
			size_t n_elements = decode_abs_num(get_const_buffer_begin());
			uint8_t *ele_begin = get_buffer_begin() + compute_type_num_len(n_elements);

			if (!is_pair() || (idx > n_elements) || (idx < 0)) {
				return CBOR();
			}

			//Jump to the reffered key
			for (T i=0 ; i < idx ; ++i) {
				ele_begin += element_size(ele_begin);
				ele_begin += element_size(ele_begin);
			}

			return CBOR(ele_begin, element_size(ele_begin), true);
		}

		//! Returns the CBOR value located at a key. Use for CBOR PAIR.
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
		 * found or if this object does not actually stores a CBOR PAIR.
		 */
		template <typename T> CBOR find_by_key(T key)
		{
			if (!is_pair()) {
				return CBOR();
			}

			size_t n_elements = decode_abs_num(get_const_buffer_begin());
			uint8_t *ele_begin = get_buffer_begin() + compute_type_num_len(n_elements);

			CBOR idx_cbor = CBOR(key);
			const uint8_t *idx_cbor_buffer = idx_cbor.to_CBOR();
			size_t idx_cbor_size = idx_cbor.length();

			//Search key until the end of the Pair (map) is found
			for (size_t i=0 ; i < n_elements ; ++i) {
				//If key match
				if (buffer_equals(idx_cbor_buffer, idx_cbor_size,
							ele_begin, element_size(ele_begin))) {

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

		//! Returns the CBOR value associated with a particular key or index.
		/*!
		 * This operator does not perform any copy.
		 * However, it cannot be used to modify the value at key/index `key`.
		 *
		 * For a CBOR ARRAY, behaviour is undefined if `key` is non-numeric.
		 *
		 * \param key The key (CBOR PAIR) or index (CBOR ARRAY) of the CBOR value to be retrieved.
		 * \return The retrieved CBOR value, or a CBOR NULL if `key` cannot be
		 * found, if this object does not actually stores a CBOR PAIR or a
		 * CBOR ARRAY in its buffer. In case this object is a CBOR ARRAY, a CBOR
		 * NULL is also returned if `key` is negative or out of range.
		 */
		template <typename T> CBOR operator[](T key)
		{
			//Defaut implementation for non-numeric types
			if (is_pair()) {
				return find_by_key(key);
			}

			//Not a CBOR PAIR nor a CBOR ARRAY
			return CBOR();
		}

		//Specialization of operator [] for numeric types
		CBOR operator[](char key)	{ return access_op_numeric((unsigned char)key); };
		CBOR operator[](short key)	{ return access_op_numeric((unsigned short)key); };
		CBOR operator[](int key)	{ return access_op_numeric((unsigned int)key); };
#if defined(ESP32) || defined(ESP8266)
		CBOR operator[](long long key)	{ return access_op_numeric((unsigned long long)key); };
#else
		CBOR operator[](long key)	{ return access_op_numeric((unsigned long)key); };
#endif
		CBOR operator[](unsigned char key)	{ return access_op_numeric(key); };
		CBOR operator[](unsigned short key)	{ return access_op_numeric(key); };
		CBOR operator[](unsigned int key)	{ return access_op_numeric(key); };
#if defined(ESP32) || defined(ESP8266)
		CBOR operator[](unsigned long long key)	{ return access_op_numeric(key); };
#else
		CBOR operator[](unsigned long key)	{ return access_op_numeric(key); };
#endif
};
#endif
