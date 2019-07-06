#include "CBOR.h"

bool CBOR::init_buffer()
{
	buffer = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
	if (buffer == NULL) {
		return false;

		buffer_begin = NULL;

		ext_buffer_flag = false;
	}
	buffer_begin = buffer;

	ext_buffer_flag = false;

	return true;
}

CBOR::CBOR() : max_buf_len(1)
{
	init_buffer();
	add();
}

CBOR::CBOR(bool value) : max_buf_len(1)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(uint8_t value) : max_buf_len(2)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(uint16_t value) : max_buf_len(3)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(uint32_t value) : max_buf_len(5)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(uint64_t value) : max_buf_len(9)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(int8_t value) : max_buf_len(2)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(int16_t value) : max_buf_len(3)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(int32_t value) : max_buf_len(5)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(int64_t value) : max_buf_len(9)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(float value) : max_buf_len(5)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(double value) : max_buf_len(9)
{
	init_buffer();
	add(value);
}

CBOR::CBOR(const char* value)
{
	max_buf_len = strlen(value) + 1;

	init_buffer();

	add(value);
}

CBOR::CBOR(uint8_t* _buffer, size_t buffer_len, bool has_data) : ext_buffer_flag(true)
{
	max_buf_len = buffer_len;

	buffer_begin = _buffer;
	if (has_data) {
		buffer = _buffer + buffer_len;
	}
	else {
		buffer = _buffer;
	}
}

CBOR::CBOR(const CBOR &obj) {
	const uint8_t* obj_buffer = obj.to_CBOR();

	max_buf_len = obj.length();
	init_buffer();

	memcpy(buffer, obj_buffer, max_buf_len*sizeof(uint8_t));
}

CBOR::~CBOR()
{
	if(!ext_buffer_flag) {
		free(buffer_begin);
	}
}

bool CBOR::reserve(size_t len)
{
	if ((length() + len) <= max_buf_len) {
		return true;
	}

	return false;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint8_t val)
{
	if (val <= 23) {
		if (!reserve(1)) {
			return false;
		}

		*(buffer++) = cbor_type | val;
	}
	else {
		if (!reserve(2)) {
			return false;
		}

		*(buffer++) = cbor_type | CBOR_UINT8_FOLLOWS;
		*(buffer++) = val;
	}

	return true;
}

bool CBOR::encode_type_num(uint8_t cbor_type, uint16_t val)
{
	if (val <= 0xFF) { //If val fits in an uint8_t
		encode_type_num(cbor_type, (uint8_t)val);
	}
	else {
		if (!reserve(3)) {
			return false;
		}

		*(buffer++) = cbor_type | CBOR_UINT16_FOLLOWS;

		*(buffer++) = val>>8;
		*(buffer++) = val;
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
		if (!reserve(5)) {
			return false;
		}

		*(buffer++) = cbor_type | CBOR_UINT32_FOLLOWS;

		val_bytes = (uint8_t*)&val + 3;
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *val_bytes;
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
		if (!reserve(9)) {
			return false;
		}

		*(buffer++) = cbor_type | CBOR_UINT64_FOLLOWS;

		val_bytes = (uint8_t*)&val + 7;
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *(val_bytes--);
		*(buffer++) = *val_bytes;
	}
}

bool CBOR::is_neg_num() const {
	return ((buffer_begin[0]&CBOR_7) == CBOR_NEGINT)?true:false;
}

uint8_t CBOR::decode_abs_num8(const uint8_t *_buffer) {
	if ((_buffer[0]&0x1F) <= 23) {
		return _buffer[0]&0x1F;
	}
	else {
		return _buffer[1];
	}
}

uint16_t CBOR::decode_abs_num16(const uint8_t *_buffer) {
	uint16_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;

	*(ret_val_bytes++) = _buffer[2];
	*ret_val_bytes = _buffer[1];

	return ret_val;
}

uint32_t CBOR::decode_abs_num32(const uint8_t *_buffer) {
	uint32_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;
	uint8_t* buf = &(_buffer[4]);

	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*(ret_val_bytes++) = *(buf--);
	*ret_val_bytes = *buf;

	return ret_val;
}

uint64_t CBOR::decode_abs_num64(const uint8_t *_buffer) {
	uint64_t ret_val;
	uint8_t* ret_val_bytes = (uint8_t*)&ret_val;
	uint8_t* buf = &(_buffer[8]);

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

size_t CBOR::decode_abs_num(const uint8_t *_buffer) {
	uint8_t type = _buffer[0]&0x1F;

	if (type <= 24) {
		return decode_abs_num8(_buffer);
	}

	if (type == CBOR_UINT16_FOLLOWS) {
		return decode_abs_num16(_buffer);
	}

	if (type == CBOR_UINT32_FOLLOWS) {
		return decode_abs_num32(_buffer);
	}

	if (type == CBOR_UINT64_FOLLOWS) {
		return decode_abs_num64(_buffer);
	}
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


size_t CBOR::jump()
{
	uint8_t *type = buffer;

	//Integer
	if ((*type <= 23) \
			|| (*type >= 0x20 && *type <= 0x37)) {
		++buffer;
	}
	else if (*type == CBOR_UINT8_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		buffer += 2;
	}
	else if (*type == CBOR_UINT16_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		buffer += 3;
	}
	else if (*type == CBOR_UINT32_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		buffer += 5;
	}
	else if (*type == CBOR_UINT64_FOLLOWS \
			|| (*type == (CBOR_NEGINT|CBOR_UINT64_FOLLOWS))) {
		buffer += 9;
	}

	//Byte / UTF-8 Strings
	else if (*type >= 0x40 && *type <= 0x57 \
			|| *type >= 0x60 && *type <= 0x77 ) {
		buffer += decode_abs_num8(buffer) + 1;
	}
	else if (*type == 0x58 || *type == 0x78) {
		buffer += decode_abs_num8(buffer) + 2;
	}
	else if (*type == 0x59 || *type == 0x79) {
		buffer += decode_abs_num16(buffer) + 3;
	}
	else if (*type == 0x5A || *type == 0x7A) {
		buffer += decode_abs_num32(buffer) + 5;
	}
	else if (*type == 0x5B || *type == 0x7B) {
		buffer += decode_abs_num64(buffer) + 9;
	}
	else if (*type == 0x5F || *type == 0x7F) {
		//Look for break character
		while (*(++buffer) != CBOR_BREAK) ;
	}

	//Array
	else if (*type >= 0x80 && *type <= 0x97) {
		uint8_t num_ele = decode_abs_num8(buffer);
		++buffer;

		//Jump every element of the table
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			jump();
		}
	}
	else if (*type == 0x98) {
		uint8_t num_ele = decode_abs_num8(buffer);
		buffer += 2;

		//Jump every element of the table
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			jump();
		}
	}
	else if (*type == 0x99) {
		uint16_t num_ele = decode_abs_num16(buffer);
		buffer += 3;

		//Jump every element of the table
		for (uint16_t i=0 ; i < num_ele ; ++i) {
			jump();
		}
	}
	else if (*type == 0x9A) {
		uint32_t num_ele = decode_abs_num32(buffer);
		buffer += 5;

		//Jump every element of the table
		for (uint32_t i=0 ; i < num_ele ; ++i) {
			jump();
		}
	}
	else if (*type == 0x9B) {
		uint64_t num_ele = decode_abs_num64(buffer);
		buffer += 9;

		//Jump every element of the table
		for (uint64_t i=0 ; i < num_ele ; ++i) {
			jump();
		}
	}
	else if (*type == 0x9F) {
		//Jump every element of the table
		while (*buffer != CBOR_BREAK) {
			jump();
		}
	}

	//Map
	else if (*type >= 0xA0 && *type <= 0xB7) {
		uint8_t num_ele = decode_abs_num8(buffer);
		++buffer;

		//Jump every element of the map
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			jump();
			jump();
		}
	}
	else if (*type == 0xB8) {
		uint8_t num_ele = decode_abs_num8(buffer);
		buffer += 2;

		//Jump every element of the map
		for (uint8_t i=0 ; i < num_ele ; ++i) {
			jump();
			jump();
		}
	}
	else if (*type == 0xB9) {
		uint16_t num_ele = decode_abs_num16(buffer);
		buffer += 3;

		//Jump every element of the map
		for (uint16_t i=0 ; i < num_ele ; ++i) {
			jump();
			jump();
		}
	}
	else if (*type == 0xBA) {
		uint32_t num_ele = decode_abs_num32(buffer);
		buffer += 5;

		//Jump every element of the map
		for (uint32_t i=0 ; i < num_ele ; ++i) {
			jump();
			jump();
		}
	}
	else if (*type == 0xBB) {
		uint64_t num_ele = decode_abs_num64(buffer);
		buffer += 9;

		//Jump every element of the map
		for (uint64_t i=0 ; i < num_ele ; ++i) {
			jump();
			jump();
		}
	}
	else if (*type == 0xBF) {
		//Jump every element of the map
		while (*buffer != CBOR_BREAK) {
			jump();
			jump();
		}
	}

	//Simple value && false / true / null / undefined / floats
	else if (*type >= 0xE0 && *type <= 0xF7) {
		++buffer;
	}
	else if (*type == 0xF8) {
		buffer += 2;
	}
	else if (*type == CBOR_FLOAT16) {
		buffer += 3;
	}
	else if (*type == CBOR_FLOAT32) {
		buffer += 5;
	}
	else if (*type == CBOR_FLOAT64) {
		buffer += 9;
	}
	else {
		return 0;
	}

	return (buffer - type);
}

bool CBOR::add()
{
	if (!reserve(1)) {
		return false;
	}

	*(buffer++) = CBOR_NULL;

	return true;
}

bool CBOR::add(bool value)
{
	if (!reserve(1)) {
		return false;
	}

	*(buffer++) = (value)?CBOR_TRUE:CBOR_FALSE;

	return true;
}

bool CBOR::add(uint8_t value)
{
	return encode_type_num(CBOR_UINT, value);
}

bool CBOR::add(uint16_t value)
{
	return encode_type_num(CBOR_UINT, value);
}

bool CBOR::add(uint32_t value)
{
	return encode_type_num(CBOR_UINT, value);
}

bool CBOR::add(uint64_t value)
{
	return encode_type_num(CBOR_UINT, value);
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

	if (!reserve(5)) {
		return false;
	}

	*(buffer++) = CBOR_FLOAT32;

	val_bytes = (uint8_t*)&value + 3;
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *val_bytes;

	return true;
}

//Caution! This considers a 64bit float and IEEE 754 representation in memory
bool CBOR::add(double value)
{
	//On AVR arduino, double is the same as float...
	if (sizeof(double) == 4) {
		add((float)value);

		return true;
	}

	uint8_t *val_bytes = NULL;
	if (!reserve(9)) {
		return false;
	}

	*(buffer++) = CBOR_FLOAT64;

	val_bytes = (uint8_t*)&value + 7;
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *(val_bytes--);
	*(buffer++) = *val_bytes;

	return true;
}

bool CBOR::add(const char* value)
{
	size_t len_string = strlen(value);

	if(encode_type_num(CBOR_TEXT, len_string*sizeof(uint8_t))) {
		if (len_string == 0) {
			return true;
		}
		else if (reserve(len_string)) {
			memcpy(buffer, value, len_string*sizeof(uint8_t));

			buffer += len_string;

			return true;
		}
	}

	return false;
}

bool CBOR::add(const CBOR &value)
{
	size_t len_cbor = value.length();

	if (!reserve(len_cbor)) {
		return false;
	}

	memcpy(buffer, value.to_CBOR(), len_cbor*sizeof(uint8_t));

	buffer += len_cbor;

	return true;
}

bool CBOR::is_null(const uint8_t* _buffer)
{
	return (_buffer[0] == CBOR_NULL)?true:false;
}

bool CBOR::is_bool(const uint8_t* _buffer)
{
	return ((_buffer[0] == CBOR_FALSE) || (_buffer[0] == CBOR_TRUE))?true:false;
}

bool CBOR::is_uint8(const uint8_t* _buffer)
{
	if ((_buffer[0] <= 23) || (_buffer[0] == (CBOR_UINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}
	return false;
}

bool CBOR::is_uint16(const uint8_t* _buffer)
{
	return ((_buffer[0] == (CBOR_UINT|CBOR_UINT16_FOLLOWS)) \
			|| is_uint8(_buffer))?true:false;
}

bool CBOR::is_uint32(const uint8_t* _buffer)
{
	return ((_buffer[0] == (CBOR_UINT|CBOR_UINT32_FOLLOWS)) \
			|| is_uint16(_buffer))?true:false;
}

bool CBOR::is_uint64(const uint8_t* _buffer)
{
	return ((_buffer[0] == (CBOR_UINT|CBOR_UINT64_FOLLOWS)) \
			|| is_uint32(_buffer))?true:false;
}

bool CBOR::is_int8(const uint8_t* _buffer)
{
	uint8_t abs_val = decode_abs_num8(_buffer);

	if ((abs_val <= 0x7F) && is_uint8(_buffer)) {
		return true;
	}

	if ((abs_val <= 0x80) \
			&& (_buffer[0] >= CBOR_NEGINT) \
			&& (_buffer[0] <= (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int16(const uint8_t* _buffer)
{
	//All CBOR UINT8 and NEGINT8
	if (is_uint8(_buffer) || (_buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS))) {
		return true;
	}

	uint16_t abs_val = decode_abs_num16(_buffer);

	if ((abs_val <= 0x7FFF) && is_uint16(_buffer)) {
		return true;
	}

	if ((abs_val <= 0x8000) \
			&& (_buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int32(const uint8_t* _buffer)
{
	//All CBOR UINT{8,16} and NEGINT{8,16}
	if (is_uint16(_buffer) || (_buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS)) \
			|| (_buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS))) {
		return true;
	}

	uint32_t abs_val = decode_abs_num32(_buffer);

	if ((abs_val <= 0x7FFFFFFF) && is_uint32(_buffer)) {
		return true;
	}

	if ((abs_val <= 0x80000000) \
			&& (_buffer[0] == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_int64(const uint8_t* _buffer)
{
	//All CBOR UINT{8,16,32} and NEGINT{8,16,32}
	if (is_uint32(_buffer) || (_buffer[0] == (CBOR_NEGINT|CBOR_UINT8_FOLLOWS)) \
			|| (_buffer[0] == (CBOR_NEGINT|CBOR_UINT16_FOLLOWS)) \
			|| (_buffer[0] == (CBOR_NEGINT|CBOR_UINT32_FOLLOWS))) {
		return true;
	}

	uint64_t abs_val = decode_abs_num64(_buffer);

	if ((abs_val <= 0x7FFFFFFFFFFFFFFF) && is_uint32(_buffer)) {
		return true;
	}

	if ((abs_val <= 0x8000000000000000) \
			&& (_buffer[0] == (CBOR_NEGINT|CBOR_UINT64_FOLLOWS))) {
		return true;
	}

	return false;
}

bool CBOR::is_float16(const uint8_t* _buffer)
{
	return (_buffer[0] == CBOR_FLOAT16)?true:false;
}

bool CBOR::is_float32(const uint8_t* _buffer)
{
	if (is_float16(_buffer)) {
		return true;
	}

	return (_buffer[0] == CBOR_FLOAT32)?true:false;
}

bool CBOR::is_float64(const uint8_t* _buffer)
{
	if (is_float16(_buffer) || is_float32(_buffer)) {
		return true;
	}

	return (_buffer[0] == CBOR_FLOAT64)?true:false;
}

bool CBOR::is_string(const uint8_t* _buffer)
{
	return ((_buffer[0]&CBOR_7) == CBOR_TEXT)?true:false;
}

bool CBOR::is_array(const uint8_t* _buffer)
{
	return ((_buffer[0]&CBOR_7) == CBOR_ARRAY)?true:false;
}

bool CBOR::is_pair(const uint8_t* _buffer)
{
	return ((_buffer[0]&CBOR_7) == CBOR_MAP)?true:false;
}

CBOR::operator bool() const
{
	return (buffer_begin[0] == CBOR_TRUE)?true:false;
}

CBOR::operator uint8_t() const
{

	return (is_uint8())?decode_abs_num8():0;
}

CBOR::operator uint16_t() const
{
	if (is_uint8()) {
		return (uint16_t)decode_abs_num8();
	}
	if(is_uint16()) {
		return decode_abs_num16();
	}

	return 0;
}

CBOR::operator uint32_t() const
{
	if (is_uint8()) {
		return (uint32_t)decode_abs_num8();
	}
	if(is_uint16()) {
		return (uint32_t)decode_abs_num16();
	}
	if(is_uint32()) {
		return decode_abs_num32();
	}

	return 0;
}

CBOR::operator uint64_t() const
{
	if (is_uint8()) {
		return (uint64_t)decode_abs_num8();
	}
	if(is_uint16()) {
		return (uint64_t)decode_abs_num16();
	}
	if(is_uint32()) {
		return (uint64_t)decode_abs_num32();
	}
	if(is_uint64()) {
		return decode_abs_num64();
	}

	return 0;
}

CBOR::operator int8_t() const
{
	uint8_t val_abs = decode_abs_num8();

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

	uint16_t val_abs = decode_abs_num16();

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

	uint32_t val_abs = decode_abs_num32();

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

	uint64_t val_abs = decode_abs_num64();

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

	if (is_float16()) {
		uint8_t exp = (buffer_begin[1]>>2)&0x1f;
		uint16_t mant = (buffer_begin[1]&0x03)<<8 | buffer_begin[2];

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

		return (buffer_begin[1]&0x80) ? -ret_val : ret_val;
	}

	if (is_float32()) {
		uint8_t *buf = &(buffer_begin[4]);

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

	if (is_float64()) {
		//On AVR arduino, double is the same as float...
		//In this case, we convert from 64-bit float to 32-bit float
		if(sizeof(double) == 4) {
			int32_t exp = ((buffer_begin[1]&0x7F)<<4)|((buffer_begin[2]&0xF0)>>4);
			uint32_t mant = ((uint32_t)(buffer_begin[2]&0x0F)<<19) \
							| ((uint32_t)(buffer_begin[3])<<11) \
							| ((uint32_t)(buffer_begin[4])<<3) \
							| ((uint32_t)((buffer_begin[5])&0xE0)>>5);
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

			return (buffer_begin[1]&0x80) ? -ret_val : ret_val;
		}

		uint8_t *buf = &(buffer_begin[8]);

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

size_t CBOR::get_string_len() const
{
	return decode_abs_num();
}

void CBOR::get_string(char* str) const
{
	size_t len_str = get_string_len();

	memcpy(str, &buffer_begin[1], len_str*sizeof(uint8_t));
	str[len_str] = '\0';
}

void CBOR::get_string(String& str) const
{
	size_t len_str = get_string_len();

	str.reserve(len_str);

	str = "";
	for (size_t i=0 ; i < len_str ; ++i) {
		str += (char)(buffer_begin[i+1]);
	}
}

String CBOR::to_string() const
{
	String str;

	get_string(str);

	return str;
}

