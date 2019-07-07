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

class CBOR
{
	protected:
		union {
			uint8_t static_buffer_begin[STATIC_ALLOC_SIZE];
			uint8_t *buffer_begin;
		};
		uint8_t *w_ptr = static_buffer_begin;
		size_t max_buf_len = STATIC_ALLOC_SIZE;
		uint8_t buffer_type = BUFFER_STATIC_INTERNAL;

		uint8_t* get_buffer_begin() {
			return (buffer_type == BUFFER_STATIC_INTERNAL)?static_buffer_begin:buffer_begin;
		};
		virtual bool init_buffer();

		bool encode_type_num(uint8_t cbor_type, uint8_t val);
		bool encode_type_num(uint8_t cbor_type, uint16_t val);
		bool encode_type_num(uint8_t cbor_type, uint32_t val);
		bool encode_type_num(uint8_t cbor_type, uint64_t val);

		bool is_neg_num() const;
		static uint8_t decode_abs_num8(const uint8_t *buffer);
		static uint16_t decode_abs_num16(const uint8_t *buffer);
		static uint32_t decode_abs_num32(const uint8_t *buffer);
		static uint64_t decode_abs_num64(const uint8_t *buffer);
		static size_t decode_abs_num(const uint8_t *buffer);

		static uint8_t compute_type_num_len(size_t num_ele);

		uint8_t decode_abs_num8() const { return decode_abs_num8(get_buffer_begin()); };
		uint16_t decode_abs_num16() const { return decode_abs_num16(get_buffer_begin()); };
		uint32_t decode_abs_num32() const { return decode_abs_num32(get_buffer_begin()); };
		uint64_t decode_abs_num64() const { return decode_abs_num64(get_buffer_begin()); };
		size_t decode_abs_num() const { return decode_abs_num(get_buffer_begin()); };

		//Jump to the end of this item
		static size_t element_size(uint8_t *ptr);

		bool add();
		bool add(bool value);

		bool add(uint8_t value);
		bool add(uint16_t value);
		bool add(uint32_t value);
		bool add(uint64_t value);
		bool add(int8_t value);
		bool add(int16_t value);
		bool add(int32_t value);
		bool add(int64_t value);

		bool add(float value);
		bool add(double value);

		bool add(const char* value);

		bool add(const CBOR &value);
	public:
		//Constructor for primitive types
		CBOR(); //CBOR NULL
		CBOR(bool value);
		CBOR(uint8_t value);
		CBOR(uint16_t value);
		CBOR(uint32_t value);
		CBOR(uint64_t value);
		CBOR(int8_t value);
		CBOR(int16_t value);
		CBOR(int32_t value);
		CBOR(int64_t value);
		CBOR(float value);
		CBOR(double value);
		CBOR(const char* value);

		//External buffer
		CBOR(uint8_t* buffer, size_t buffer_len, bool has_data = true);

		//Copy constructor
		CBOR(const CBOR &obj);

		~CBOR();

		virtual bool reserve(size_t length);
		size_t length() const { return (size_t)(w_ptr - get_buffer_begin()); }
		const uint8_t* to_CBOR() { return get_buffer_begin(); }

		static bool is_null(const uint8_t* buffer);
		static bool is_bool(const uint8_t* buffer);
		static bool is_uint8(const uint8_t* buffer);
		static bool is_uint16(const uint8_t* buffer);
		static bool is_uint32(const uint8_t* buffer);
		static bool is_uint64(const uint8_t* buffer);
		static bool is_int8(const uint8_t* buffer);
		static bool is_int16(const uint8_t* buffer);
		static bool is_int32(const uint8_t* buffer);
		static bool is_int64(const uint8_t* buffer);
		static bool is_float16(const uint8_t* buffer);
		static bool is_float32(const uint8_t* buffer);
		static bool is_float64(const uint8_t* buffer);
		static bool is_string(const uint8_t* buffer);
		static bool is_array(const uint8_t* buffer);
		static bool is_pair(const uint8_t* buffer);

		bool is_null() const {return is_null(get_buffer_begin()); };
		bool is_bool() const {return is_bool(get_buffer_begin()); };
		//Does not mean that type is a CBOR (u)int*, but rather wether or not
		//the CBOR number fits in a (u)int* C type.
		//For example, 200 is: uint8, uint16, uint32, uint64, int16, int32,
		//int64 but not int8.
		bool is_uint8() const {return is_uint8(get_buffer_begin()); };
		bool is_uint16() const {return is_uint16(get_buffer_begin()); };
		bool is_uint32() const {return is_uint32(get_buffer_begin()); };
		bool is_uint64() const {return is_uint64(get_buffer_begin()); };
		bool is_int8() const {return is_int8(get_buffer_begin()); };
		bool is_int16() const {return is_int16(get_buffer_begin()); };
		bool is_int32() const {return is_int32(get_buffer_begin()); };
		bool is_int64() const {return is_int64(get_buffer_begin()); };
		//Does not mean that type is a CBOR float*, but rather wether or not
		//the CBOR number fits in a float* C type.
		bool is_float16() const {return is_float16(get_buffer_begin()); };
		bool is_float32() const {return is_float32(get_buffer_begin()); };
		bool is_float64() const {return is_float64(get_buffer_begin()); };
		bool is_string() const {return is_string(get_buffer_begin()); };
		bool is_array() const {return is_array(get_buffer_begin()); };
		bool is_pair() const {return is_pair(get_buffer_begin()); };

		operator bool() const;
		//Operators (u)int* will return 0 in case of failure.
		operator uint8_t() const;
		operator uint16_t() const;
		operator uint32_t() const;
		operator uint64_t() const;
		operator int8_t() const;
		operator int16_t() const;
		operator int32_t() const;
		operator int64_t() const;
		operator float() const;
		operator double() const;
		size_t get_string_len() const;
		void get_string(char* str) const;
		void get_string(String& str) const;
		String to_string() const;
};

#endif
