#include "CBOR.h"

bool CBOR::init_buffer()
{
	buffer_begin = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
	if (buffer_begin == NULL) {
		return false;
	}

	w_ptr = buffer_begin;
	buffer_type = BUFFER_DYNAMIC_INTERNAL;

	return true;
}

CBOR::CBOR(uint8_t* buffer, size_t buffer_len, bool has_data)
{
	max_buf_len = buffer_len;

	buffer_begin = buffer;
	if (has_data) {
		w_ptr = buffer + buffer_len;
	}
	else {
		w_ptr = buffer;
	}

	buffer_type = BUFFER_EXTERNAL;
}

CBOR::CBOR(const uint8_t* buffer, size_t buffer_len)
{
	if (buffer_len > STATIC_ALLOC_SIZE) {
		max_buf_len = buffer_len;
		init_buffer();
	}

	memcpy(w_ptr, buffer, max_buf_len*sizeof(uint8_t));
	w_ptr += element_size(w_ptr);
}

CBOR::CBOR(const CBOR &obj) {
	const uint8_t* obj_buffer = obj.to_CBOR();

	if (obj.length() > STATIC_ALLOC_SIZE) {
		max_buf_len = obj.length();
		init_buffer();
	}

	memcpy(w_ptr, obj_buffer, max_buf_len*sizeof(uint8_t));
	w_ptr += obj.length();

}

CBOR::~CBOR()
{
	if((buffer_type == BUFFER_DYNAMIC_INTERNAL) && (buffer_begin != NULL)) {
		free(buffer_begin);
	}
}

bool CBOR::reserve(size_t len)
{
	if (len <= max_buf_len) {
		return true;
	}

	if (buffer_type == BUFFER_STATIC_INTERNAL) {
		uint8_t buffer_saved[STATIC_ALLOC_SIZE];
		size_t length_saved = length();

		//Save static buffer
		memcpy(buffer_saved, static_buffer_begin, STATIC_ALLOC_SIZE*sizeof(uint8_t));

		//Initialize dynamic buffer
		max_buf_len = len;
		if (init_buffer()) {
			//Copy saved data in newly allocated buffer
			memcpy(buffer_begin, buffer_saved, STATIC_ALLOC_SIZE*sizeof(uint8_t));

			//Update write pointer
			w_ptr = buffer_begin + length_saved;

			return true;
		}

		return false;
	}

	if (buffer_type == BUFFER_DYNAMIC_INTERNAL) {
		size_t length_saved = length();

		buffer_begin = (uint8_t*)realloc(buffer_begin, sizeof(uint8_t)*len);
		if (buffer_begin == NULL) {
			return false;
		}

		//Update max buffer length and write pointer
		max_buf_len = len;
		w_ptr = buffer_begin + length_saved;
	}

	//BUFFER_EXTERNAL
	return false;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint8_t val)
{
	if (val <= 23) {
		if (!reserve(length() + 1)) {
			return false;
		}

		*(w_ptr++) = cbor_type | val;
	}
	else {
		if (!reserve(length() + 2)) {
			return false;
		}

		*(w_ptr++) = cbor_type | CBOR_UINT8_FOLLOWS;
		*(w_ptr++) = val;
	}

	return true;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint16_t val)
{
	if (val <= 0xFF) { //If val fits in an uint8_t
		encode_type_num(cbor_type, (uint8_t)val);
	}
	else {
		if (!reserve(length() + 3)) {
			return false;
		}

		*(w_ptr++) = cbor_type | CBOR_UINT16_FOLLOWS;

		*(w_ptr++) = val>>8;
		*(w_ptr++) = val;
	}

	return true;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint32_t val)
{
	uint8_t *val_bytes = NULL;

	if (val <= 0xFFFF) { //If val fits in a uint16_t
		return encode_type_num(cbor_type, (uint16_t)val);
	}
	else {
		if (!reserve(length() + 5)) {
			return false;
		}

		*(w_ptr++) = cbor_type | CBOR_UINT32_FOLLOWS;

		val_bytes = (uint8_t*)&val + 3;
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *val_bytes;
	}

	return true;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint64_t val)
{
	uint8_t *val_bytes = NULL;
	if (val <= 0xFFFFFFFF) { //If val fits in a uint32_t
		return encode_type_num(cbor_type, (uint32_t)val);
	}
	else {
		if (!reserve(length() + 9)) {
			return false;
		}

		*(w_ptr++) = cbor_type | CBOR_UINT64_FOLLOWS;

		val_bytes = (uint8_t*)&val + 7;
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *(val_bytes--);
		*(w_ptr++) = *val_bytes;
	}

	return true;
}

bool CBOR::is_neg_num() const {
	return ((get_const_buffer_begin()[0]&CBOR_7) == CBOR_NEGINT)?true:false;
}

uint8_t CBOR::decode_abs_num8(const uint8_t *ptr) {
	if ((ptr[0]&0x1F) <= 23) {
		return ptr[0]&0x1F;
	}
	else {
		return ptr[1];
	}
}

uint16_t CBOR::decode_abs_num16(const uint8_t *ptr) {
	uint16_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;

	*(ret_val_bytes++) = ptr[2];
	*ret_val_bytes = ptr[1];

	return ret_val;
}

uint32_t CBOR::decode_abs_num32(const uint8_t *ptr) {
	uint32_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;
	const uint8_t* buf = &(ptr[4]);

	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*ret_val_bytes = *buf;

	return ret_val;
}

uint64_t CBOR::decode_abs_num64(const uint8_t *ptr) {
	uint64_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;
	const uint8_t* buf = &(ptr[8]);

	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*ret_val_bytes = *buf;

	return ret_val;
}

size_t CBOR::decode_abs_num(const uint8_t *ptr) {
	uint8_t type = ptr[0]&0x1F;

	if (type <= 24) {
		return decode_abs_num8(ptr);
	}

	if (type == CBOR_UINT16_FOLLOWS) {
		return decode_abs_num16(ptr);
	}

	if (type == CBOR_UINT32_FOLLOWS) {
		return decode_abs_num32(ptr);
	}

	if (type == CBOR_UINT64_FOLLOWS) {
		return decode_abs_num64(ptr);
	}

	return 0;
}

uint8_t CBOR::compute_type_num_len(size_t num_ele) {
	//Compute how many bytes are needed to store table length
	if (num_ele <= 23) {
		return 1;
	}
	if (num_ele <= 0xFF) {
		return 2;
	}
	if (num_ele <= 0xFFFF) {
		return 3;
	}
	if (num_ele <= 0xFFFFFFFF) {
		return 5;
	}
	return 9;
}


size_t CBOR::element_size(uint8_t *ptr)
{
	uint8_t *type = ptr;

	//Integer
	if ((*type <= 23) \
			|| (*type >= 0x20 && *type <= 0x37)) {
		++ptr;
	}
	else if (*type == CBOR_UINT8_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		ptr += 2;
	}
	else if (*type == CBOR_UINT16_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		ptr += 3;
	}
	else if (*type == CBOR_UINT32_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		ptr += 5;
	}
	else if (*type == CBOR_UINT64_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT64_FOLLOWS))) {
		ptr += 9;
	}

	//Byte / UTF-8 Strings
	else if ((*type >= 0x40 && *type <= 0x57) \
			|| (*type >= 0x60 && *type <= 0x77) ) {
		ptr += decode_abs_num8(ptr) + 1;
	}
	else if (*type == 0x58 || *type == 0x78) {
		ptr += decode_abs_num8(ptr) + 2;
	}
	else if (*type == 0x59 || *type == 0x79) {
		ptr += decode_abs_num16(ptr) + 3;
	}
	else if (*type == 0x5A || *type == 0x7A) {
		ptr += decode_abs_num32(ptr) + 5;
	}
	else if (*type == 0x5B || *type == 0x7B) {
		ptr += decode_abs_num64(ptr) + 9;
	}
	else if (*type == 0x5F || *type == 0x7F) {
		//Look for break character
		while (*(++ptr) != CBOR_BREAK) ;
	}

	//Array
	else if (*type >= 0x80 && *type <= 0x97) {
		uint8_t num_ele = decode_abs_num8(ptr);
		++ptr;

		//Jump every element of the table
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0x98) {
		uint8_t num_ele = decode_abs_num8(ptr);
		ptr += 2;

		//Jump every element of the table
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0x99) {
		uint16_t num_ele = decode_abs_num16(ptr);
		ptr += 3;

		//Jump every element of the table
		for (uint16_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0x9A) {
		uint32_t num_ele = decode_abs_num32(ptr);
		ptr += 5;

		//Jump every element of the table
		for (uint32_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0x9B) {
		uint64_t num_ele = decode_abs_num64(ptr);
		ptr += 9;

		//Jump every element of the table
		for (uint64_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0x9F) {
		//Jump every element of the table
		while (*ptr != CBOR_BREAK) {
			ptr += element_size(ptr);
		}
	}

	//Map
	else if (*type >= 0xA0 && *type <= 0xB7) {
		uint8_t num_ele = decode_abs_num8(ptr);
		++ptr;

		//Jump every element of the map
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0xB8) {
		uint8_t num_ele = decode_abs_num8(ptr);
		ptr += 2;

		//Jump every element of the map
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0xB9) {
		uint16_t num_ele = decode_abs_num16(ptr);
		ptr += 3;

		//Jump every element of the map
		for (uint16_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0xBA) {
		uint32_t num_ele = decode_abs_num32(ptr);
		ptr += 5;

		//Jump every element of the map
		for (uint32_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0xBB) {
		uint64_t num_ele = decode_abs_num64(ptr);
		ptr += 9;

		//Jump every element of the map
		for (uint64_t i=0 ; i < num_ele ; ++i) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}
	else if (*type == 0xBF) {
		//Jump every element of the map
		while (*ptr != CBOR_BREAK) {
			ptr += element_size(ptr);
			ptr += element_size(ptr);
		}
	}

	//Tags
	else if(*type >= 0xC0 && *type <= 0xDB) {
		//Size of tag value
		ptr += compute_type_num_len(decode_abs_num(ptr));
		//Size of tag item
		ptr += element_size(ptr);
	}

	//Simple value && false / true / null / undefined / floats
	else if (*type >= 0xE0 && *type <= 0xF7) {
		++ptr;
	}
	else if (*type == 0xF8) {
		ptr += 2;
	}
	else if (*type == CBOR_FLOAT16) {
		ptr += 3;
	}
	else if (*type == CBOR_FLOAT32) {
		ptr += 5;
	}
	else if (*type == CBOR_FLOAT64) {
		ptr += 9;
	}
	else {
		return 0;
	}

	return (ptr - type);
}

bool CBOR::buffer_equals(const uint8_t* buf1, size_t len_buf1,
		const uint8_t* buf2, size_t len_buf2)
{
	if (len_buf1 != len_buf2) {
		return false;
	}

	const uint8_t *_buf1 = buf1, *_buf2 = buf2;

	while (_buf1 != (buf1 + len_buf1)) {
		if (*_buf1 != *_buf2) {
			return false;
		}
		++_buf1;
		++_buf2;
	}

	return true;
}

bool CBOR::add()
{
	if (!reserve(length() + 1)) {
		return false;
	}

	*(w_ptr++) = CBOR_NULL;

	return true;
}

bool CBOR::add(bool value)
{
	if (!reserve(length() + 1)) {
		return false;
	}

	*(w_ptr++) = (value)?CBOR_TRUE:CBOR_FALSE;

	return true;
}

bool CBOR::add(int8_t value)
{
	if (value < 0) {
		return encode_type_num(CBOR_NEGINT, (uint8_t)(-1-value));
	}
	else {
		return encode_type_num(CBOR_UINT, (uint8_t)value);
	}
}

bool CBOR::add(int16_t value)
{
	if (value < 0) {
		return encode_type_num(CBOR_NEGINT, (uint16_t)(-1-value));
	}
	else {
		return encode_type_num(CBOR_UINT, (uint16_t)value);
	}
}

bool CBOR::add(int32_t value)
{
	if (value < 0) {
		return encode_type_num(CBOR_NEGINT, (uint32_t)(-1-value));
	}
	else {
		return encode_type_num(CBOR_UINT, (uint32_t)value);
	}
}

bool CBOR::add(int64_t value)
{
	if (value < 0) {
		return encode_type_num(CBOR_NEGINT, (uint64_t)(-1-value));
	}
	else {
		return encode_type_num(CBOR_UINT, (uint64_t)value);
	}
}

//Caution! This considers a 32bit float and IEEE 754 representation in memory
bool CBOR::add(float value)
{
	uint8_t *val_bytes = NULL;

	if (!reserve(length() + 5)) {
		return false;
	}

	*(w_ptr++) = CBOR_FLOAT32;

	val_bytes = (uint8_t*)&value + 3;
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *val_bytes;
	return true;
}

//Caution! This considers a 64bit float and IEEE 754 representation in memory
bool CBOR::add(double value)
{
	//On AVR arduino, double is the same as float...
	if (sizeof(double) == 4) {
		return add((float)value);
	}

	uint8_t *val_bytes = NULL;
	if (!reserve(length() + 9)) {
		return false;
	}

	*(w_ptr++) = CBOR_FLOAT64;

	val_bytes = (uint8_t*)&value + 7;
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *(val_bytes--);
	*(w_ptr++) = *val_bytes;

	return true;
}

bool CBOR::add(const char* value)
{
	size_t len_string = strlen(value);

	//Check buffer size
	if (!reserve(length() + len_string + compute_type_num_len(len_string))) {
		return false;
	}

	//Encode string length
	encode_type_num(CBOR_TEXT, len_string);
	if (len_string == 0) {
		return true;
	}

	//Copy string content (without '\0')
	memcpy(w_ptr, value, len_string*sizeof(uint8_t));
	w_ptr += len_string;

	return true;
}

bool CBOR::add(const uint8_t* value, size_t len)
{
	//Check buffer size
	if (!reserve(length() + len + compute_type_num_len(len))) {
		return false;
	}

	//Encode length
	encode_type_num(CBOR_BYTES, len);
	if (len == 0) {
		return true;
	}

	//Copy string content (without '\0')
	memcpy(w_ptr, value, len*sizeof(uint8_t));
	w_ptr += len;

	return true;
}

bool CBOR::add(const CBOR &value)
{
	size_t len_cbor = value.length();
	if (!reserve(length() + len_cbor)) {
		return false;
	}

	memcpy(w_ptr, value.to_CBOR(), len_cbor*sizeof(uint8_t));

	w_ptr += len_cbor;

	return true;
}

bool CBOR::is_null(const uint8_t* buffer)
{
	return (buffer[0] == CBOR_NULL)?true:false;
}

bool CBOR::is_bool(const uint8_t* buffer)
{
	return ((buffer[0] == CBOR_FALSE) || (buffer[0] == CBOR_TRUE))?true:false;
}

bool CBOR::is_uint8(const uint8_t* buffer)
{
	if ((buffer[0] <= 23) || (buffer[0] == (CBOR_UINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}
	return false;
}

bool CBOR::is_uint16(const uint8_t* buffer)
{
	return ((buffer[0] == (CBOR_UINT|CBOR_UINT16_FOLLOWS)) \
			|| is_uint8(buffer))?true:false;
}

bool CBOR::is_uint32(const uint8_t* buffer)
{
	return ((buffer[0] == (CBOR_UINT|CBOR_UINT32_FOLLOWS)) \
			|| is_uint16(buffer))?true:false;
}

bool CBOR::is_uint64(const uint8_t* buffer)
{
	return ((buffer[0] == (CBOR_UINT|CBOR_UINT64_FOLLOWS)) \
			|| is_uint32(buffer))?true:false;
}

bool CBOR::is_int8(const uint8_t* buffer)
{
	uint8_t abs_val = decode_abs_num8(buffer);

	if ((abs_val <= 0x7F) && is_uint8(buffer)) {
		return true;
	}

	if ((abs_val <= 0x80) \
			&& (buffer[0] >= CBOR_NEGINT) \
			&& (buffer[0] <= (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int16(const uint8_t* buffer)
{
	//All CBOR UINT8 and NEGINT8
	if (is_uint8(buffer) || (buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}

	uint16_t abs_val = decode_abs_num16(buffer);

	if ((abs_val <= 0x7FFF) && is_uint16(buffer)) {
		return true;
	}

	if ((abs_val <= 0x8000) \
			&& (buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int32(const uint8_t* buffer)
{
	//All CBOR UINT{8,16} and NEGINT{8,16}
	if (is_uint16(buffer) || (buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS)) \
			|| (buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		return true;
	}

	uint32_t abs_val = decode_abs_num32(buffer);

	if ((abs_val <= 0x7FFFFFFF) && is_uint32(buffer)) {
		return true;
	}

	if ((abs_val <= 0x80000000) \
			&& (buffer[0] == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int64(const uint8_t* buffer)
{
	//All CBOR UINT{8,16,32} and NEGINT{8,16,32}
	if (is_uint32(buffer) || (buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS)) \
			|| (buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS)) \
			|| (buffer[0] == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		return true;
	}

	uint64_t abs_val = decode_abs_num64(buffer);

	if ((abs_val <= 0x7FFFFFFFFFFFFFFF) && is_uint32(buffer)) {
		return true;
	}

	if ((abs_val <= 0x8000000000000000) \
			&& (buffer[0] == (CBOR_NEGINT|CBOR_UINT64_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_float16(const uint8_t* buffer)
{
	return (buffer[0] == CBOR_FLOAT16)?true:false;
}

bool CBOR::is_float32(const uint8_t* buffer)
{
	if (is_float16(buffer)) {
		return true;
	}

	return (buffer[0] == CBOR_FLOAT32)?true:false;
}

bool CBOR::is_float64(const uint8_t* buffer)
{
	if (is_float16(buffer) || is_float32(buffer)) {
		return true;
	}

	return (buffer[0] == CBOR_FLOAT64)?true:false;
}

bool CBOR::is_string(const uint8_t* buffer)
{
	return ((buffer[0]&CBOR_7) == CBOR_TEXT)?true:false;
}

bool CBOR::is_bytestring(const uint8_t* buffer)
{
	return ((buffer[0]&CBOR_7) == CBOR_BYTES)?true:false;
}

bool CBOR::is_array(const uint8_t* buffer)
{
	return ((buffer[0]&CBOR_7) == CBOR_ARRAY)?true:false;
}

bool CBOR::is_pair(const uint8_t* buffer)
{
	return ((buffer[0]&CBOR_7) == CBOR_MAP)?true:false;
}

bool CBOR::is_tag(const uint8_t* buffer)
{
	return ((buffer[0]&CBOR_7) == CBOR_TAG)?true:false;
}

CBOR::operator bool() const
{
	return (get_const_buffer_begin()[0] == CBOR_TRUE)?true:false;
}

CBOR::operator uint8_t() const
{

	return (is_uint8())?decode_abs_num8(get_const_buffer_begin()):0;
}

CBOR::operator uint16_t() const
{
	if (is_uint8()) {
		return (uint16_t)decode_abs_num8(get_const_buffer_begin());
	}
	if(is_uint16()) {
		return decode_abs_num16(get_const_buffer_begin());
	}

	return 0;
}

CBOR::operator uint32_t() const
{
	if (is_uint8()) {
		return (uint32_t)decode_abs_num8(get_const_buffer_begin());
	}
	if(is_uint16()) {
		return (uint32_t)decode_abs_num16(get_const_buffer_begin());
	}
	if(is_uint32()) {
		return decode_abs_num32(get_const_buffer_begin());
	}

	return 0;
}

CBOR::operator uint64_t() const
{
	if (is_uint8()) {
		return (uint64_t)decode_abs_num8(get_const_buffer_begin());
	}
	if(is_uint16()) {
		return (uint64_t)decode_abs_num16(get_const_buffer_begin());
	}
	if(is_uint32()) {
		return (uint64_t)decode_abs_num32(get_const_buffer_begin());
	}
	if(is_uint64()) {
		return decode_abs_num64(get_const_buffer_begin());
	}

	return 0;
}

CBOR::operator int8_t() const
{
	uint8_t val_abs = decode_abs_num8(get_const_buffer_begin());

	if (is_int8()) {
		if (is_neg_num()) {
			return (int8_t)(-1-val_abs);
		}
		else {
			return val_abs;
		}
	}

	return 0;
}

CBOR::operator int16_t() const
{
	if (is_int8()) {
		return (int16_t)((int8_t)(*this));
	}

	uint16_t val_abs = decode_abs_num16(get_const_buffer_begin());

	if (is_int16()) {
		if (is_neg_num()) {
			return (int16_t)(-1-val_abs);
		}
		else {
			return val_abs;
		}
	}

	return 0;
}

CBOR::operator int32_t() const
{
	if (is_int8()) {
		return (int32_t)((int8_t)(*this));
	}
	if (is_int16()) {
		return (int32_t)((int16_t)(*this));
	}

	uint32_t val_abs = decode_abs_num32(get_const_buffer_begin());

	if (is_int32()) {
		if (is_neg_num()) {
			return (int32_t)(-1-val_abs);
		}
		else {
			return val_abs;
		}
	}

	return 0;
}

CBOR::operator int64_t() const
{
	if (is_int8()) {
		return (int64_t)((int8_t)(*this));
	}
	if (is_int16()) {
		return (int64_t)((int16_t)(*this));
	}
	if (is_int32()) {
		return (int64_t)((int32_t)(*this));
	}

	uint64_t val_abs = decode_abs_num64(get_const_buffer_begin());

	if (is_int64()) {
		if (is_neg_num()) {
			return (int64_t)(-1-val_abs);
		}
		else {
			return val_abs;
		}
	}

	return 0;
}

CBOR::operator float() const
{
	float ret_val = 0.0;
	uint8_t *ret_val_bytes = (uint8_t*)&ret_val;
	const uint8_t *buf = NULL;

	if (is_float16()) {
		buf = get_const_buffer_begin();
		uint8_t exp = (buf[1]>>2)&0x1f;
		uint16_t mant = (buf[1]&0x03)<<8 | buf[2];

		//Subnormals
		if (exp == 0) {
			ret_val = ldexp(mant, -24);
		}
		//NaN / Infinity
		else if (exp == 31) {
			ret_val = (mant == 0) ? INFINITY : NAN;
		}
		else {
			ret_val = ldexp(mant + 1024, exp - 25);
		}

		return (buf[1]&0x80) ? -ret_val : ret_val;
	}

	if (is_float32()) {
		buf = get_const_buffer_begin() + 4;

		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*ret_val_bytes = *buf;

		return ret_val;
	}

	return 0.0;
}

CBOR::operator double() const
{
	if (is_float16() || is_float32()) {
		return (double)((float)(*this));
	}

	double ret_val = 0.0;
	uint8_t *ret_val_bytes = (uint8_t*)&ret_val;
	const uint8_t *buf = NULL;

	if (is_float64()) {
		//On AVR arduino, double is the same as float...
		//In this case, we convert from 64-bit float to 32-bit float
		if(sizeof(double) == 4) {
			buf = get_const_buffer_begin();
			int32_t exp = ((buf[1]&0x7F)<<4)|((buf[2]&0xF0)>>4);
			uint32_t mant = ((uint32_t)(buf[2]&0x0F)<<19) \
							| ((uint32_t)(buf[3])<<11) \
							| ((uint32_t)(buf[4])<<3) \
							| ((uint32_t)((buf[5])&0xE0)>>5);
			//Subnormals
			if (exp == 0) {
				ret_val = ldexp(mant, -149);
			}
			//Infinity / NaN
			else if (exp == 0x7FF) {
				ret_val = (mant == 0) ? INFINITY : NAN;
			}
			//Too big
			else if ((exp-1023) > 127) {
				ret_val = INFINITY;
			}
			//Too little
			else if ((exp-1023) < -126) {
				ret_val = 0.0;
			}
			else {
				ret_val = ldexp((float)mant + 8388608.0, (exp-1023) - 23);
			}

			return (buf[1]&0x80) ? -ret_val : ret_val;
		}

		buf = get_const_buffer_begin() + 8;

		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*(ret_val_bytes++) = *(buf--);
		*ret_val_bytes = *buf;

		return ret_val;
	}

	return 0.0;
}

void CBOR::get_string(char* str) const
{
	size_t len_str = get_string_len();

	memcpy(str, get_const_buffer_begin() + compute_type_num_len(len_str),
			len_str*sizeof(uint8_t));
	str[len_str] = '\0';
}

void CBOR::get_string(String& str) const
{
	size_t len_str = get_string_len();

	str.reserve(length() + len_str);

	str = "";
	for (const uint8_t* i=get_const_buffer_begin() + compute_type_num_len(len_str)
		   	; i < (get_const_buffer_begin() + compute_type_num_len(len_str) + len_str)
			; ++i) {
		str += (char)(*i);
	}
}

String CBOR::to_string() const
{
	String str;

	get_string(str);

	return str;
}

void CBOR::get_bytestring(uint8_t* bytestr) const
{
	size_t len_bytestr = get_bytestring_len();

	memcpy(bytestr, get_const_buffer_begin() + compute_type_num_len(len_bytestr),
			len_bytestr*sizeof(uint8_t));
}


size_t CBOR::get_tag_value() const
{
	return decode_abs_num(get_const_buffer_begin());
}

CBOR CBOR::get_tag_item()
{
	uint8_t *ele_begin = get_buffer_begin();

	//Jump to the begining of the tag item
	ele_begin += compute_type_num_len(get_tag_value());

	return CBOR(ele_begin, element_size(ele_begin), true);
}

size_t CBOR::n_elements() const
{
	if (is_array() || is_pair()) {
		return decode_abs_num(get_const_buffer_begin());
	}
	else {
		return 0;
	}
}
