#include "CBORPair.h"

bool CBORPair::init_buffer()
{
	//Reserve begining of buffer to store table length
	ext_buffer_begin = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
	if (ext_buffer_begin == NULL) {
		buffer_data_begin = NULL;
		buffer_begin = NULL;
		buffer = NULL;

		return false;
	}

	buffer_data_begin = ext_buffer_begin + 9;
	buffer_begin = ext_buffer_begin + 8; //type_num_len is 1 for array size 0
	buffer = buffer_data_begin;

	buffer_type = BUFFER_DYNAMIC_INTERNAL;

	return true;
}

void CBORPair::init_num_ele(size_t num_ele)
{
	//Save buffer
	uint8_t *old_buffer = buffer;

	//Put buffer pointer in the right position, considering the size of the
	//length field
	buffer = buffer_data_begin - compute_type_num_len(num_ele);
	//Update buffer_begin
	buffer_begin = buffer;

	//Encode num_ele
	encode_type_num(CBOR_MAP, num_ele);

	//Put buffer back into its inital position
	buffer = old_buffer;

	return;
}

CBORPair::CBORPair(size_t buf_len)
{
	//Reserve buffer
	max_buf_len = buf_len;
	init_buffer();

	//Initialize num_ele
	init_num_ele(0);
}

CBORPair::CBORPair(const CBORPair &obj)
{
	uint8_t type_num_len = compute_type_num_len(obj.n_elements());
	//Reserve buf_len plus maximum size of a type_num : 9 bytes to encode a
	//table length up to (2^64)-1
	size_t buf_len_needed = obj.length() - type_num_len + 9;

	max_buf_len = buf_len_needed;
	init_buffer();

	//Reserve begining of buffer to store table length
	buffer_begin = ext_buffer_begin + 9 - type_num_len;
	buffer = buffer_begin + obj.length();

	//Copy num_ele and data
	memcpy(buffer_begin, obj.to_CBOR(), obj.length());
}

CBORPair::CBORPair(uint8_t* _buffer, size_t buffer_len, bool has_data)
{
	max_buf_len = buffer_len;
	buffer_type = BUFFER_EXTERNAL;

	if (has_data) {
		ext_buffer_begin = _buffer;
		buffer_begin = _buffer;

		buffer_data_begin = buffer_begin + compute_type_num_len(n_elements());

		//Jump to the end of the data chunk
		buffer = buffer_data_begin;
		for (size_t i=0 ;  i < n_elements() ; ++i) {
			jump();
			jump();
		}
	}
	else {
		//Reserve begining of buffer to store table length
		ext_buffer_begin = _buffer;
		buffer_data_begin = _buffer + 9;
		buffer_begin = _buffer + 8; //type_num_len is 1 for Pair size 0
		buffer = buffer_data_begin;

		//Initialize num_ele
		init_num_ele(0);
	}
}

CBORPair::CBORPair(const CBOR &obj)
{
	max_buf_len = obj.length();
	buffer_type = BUFFER_EXTERNAL;

	ext_buffer_begin = obj.to_CBOR();
	buffer_begin = ext_buffer_begin;

	buffer_data_begin = buffer_begin + compute_type_num_len(n_elements());

	//Jump to the end of the data chunk
	buffer = buffer_data_begin;
	for (size_t i=0 ;  i < n_elements() ; ++i) {
		jump();
		jump();
	}
}

CBORPair::~CBORPair()
{
	if(buffer_type == BUFFER_DYNAMIC_INTERNAL) {
		free(ext_buffer_begin);
	}
}
