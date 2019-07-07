#include "CBORPair.h"

bool CBORPair::init_buffer()
{
	//Reserve begining of buffer to store table length
	ext_buffer_begin = (uint8_t*)malloc(sizeof(uint8_t)*max_buf_len);
	if (ext_buffer_begin == NULL) {
		buffer_data_begin = NULL;
		buffer_begin = NULL;
		w_ptr = NULL;

		return false;
	}

	buffer_data_begin = ext_buffer_begin + NUM_ELE_PROVISION;
	buffer_begin = ext_buffer_begin + NUM_ELE_PROVISION - 1; //type_num_len is 1 for array size 0
	w_ptr = buffer_data_begin;

	buffer_type = BUFFER_DYNAMIC_INTERNAL;

	return true;
}

bool CBORPair::reserve(size_t len)
{
	size_t num_ele = n_elements();
	size_t requested_len = len - compute_type_num_len(num_ele) + NUM_ELE_PROVISION;

	if (requested_len <= max_buf_len) {
		return true;
	}

	if (buffer_type == BUFFER_DYNAMIC_INTERNAL) {
		size_t length_saved = length();

		ext_buffer_begin = (uint8_t*)realloc(ext_buffer_begin,
				sizeof(uint8_t)*requested_len);
		if (ext_buffer_begin == NULL) {
			return false;
		}

		//Update max buffer length and write pointer
		max_buf_len = requested_len;
		buffer_data_begin = ext_buffer_begin + NUM_ELE_PROVISION;
		buffer_begin = buffer_data_begin - compute_type_num_len(num_ele);
		w_ptr = buffer_begin + length_saved;

		return true;
	}

	//BUFFER_EXTERNAL or BUFFER_STATIC_INTERNAL
	return false;
}

void CBORPair::init_num_ele(size_t num_ele)
{
	//Save buffer
	uint8_t *old_w_ptr= w_ptr;

	//Put buffer pointer in the right position, considering the size of the
	//length field
	w_ptr = buffer_data_begin - compute_type_num_len(num_ele);
	//Update buffer_begin
	buffer_begin = w_ptr;

	//Encode num_ele
	encode_type_num(CBOR_MAP, num_ele);

	//Put buffer back into its inital position
	w_ptr = old_w_ptr;

	return;
}

bool CBORPair::buffer_equals(const uint8_t* buf1, size_t len_buf1,
		const uint8_t* buf2, size_t len_buf2)
{
	if (len_buf1 != len_buf2) {
		return false;
	}

	uint8_t *_buf1 = buf1, *_buf2 = buf2;

	while (_buf1 != (buf1 + len_buf1)) {
		if (*_buf1 != *_buf2) {
			return false;
		}
		++_buf1;
		++_buf2;
	}

	return true;
}

CBORPair::CBORPair()
{
	//Reserve buffer for 1 key and 1 value of 1 byte each
	max_buf_len = 11;
	init_buffer();

	//Initialize num_ele
	init_num_ele(0);
}

CBORPair::CBORPair(size_t buf_len)
{
	//Reserve buffer
	max_buf_len = buf_len - 1 + NUM_ELE_PROVISION;
	init_buffer();

	//Initialize num_ele
	init_num_ele(0);
}

CBORPair::CBORPair(const CBORPair &obj)
{
	uint8_t type_num_len = compute_type_num_len(obj.n_elements());
	//Reserve buf_len plus maximum size of a type_num : 9 bytes to encode a
	//table length up to (2^64)-1
	size_t buf_len_needed = obj.length() - type_num_len + NUM_ELE_PROVISION;

	max_buf_len = buf_len_needed;
	init_buffer();

	//Reserve begining of buffer to store table length
	buffer_begin = ext_buffer_begin + NUM_ELE_PROVISION - type_num_len;
	w_ptr= buffer_begin + obj.length();

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
		w_ptr = buffer_data_begin;
		for (size_t i=0 ;  i < n_elements() ; ++i) {
			w_ptr = element_size(w_ptr);
			w_ptr = element_size(w_ptr);
		}
	}
	else {
		//Reserve begining of buffer to store table length
		ext_buffer_begin = _buffer;
		buffer_data_begin = _buffer + NUM_ELE_PROVISION;
		buffer_begin = _buffer + 8; //type_num_len is 1 for Pair size 0
		w_ptr = buffer_data_begin;

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
	w_ptr = buffer_data_begin;
	for (size_t i=0 ;  i < n_elements() ; ++i) {
		w_ptr += element_size(w_ptr);
		w_ptr += element_size(w_ptr);
	}
}

CBORPair::~CBORPair()
{
	if(buffer_type == BUFFER_DYNAMIC_INTERNAL) {
		free(ext_buffer_begin);
	}
}

CBOR CBORPair::at(size_t idx)
{
	if (!is_pair() || (idx > n_elements())) {
		return CBOR();
	}

	uint8_t *ele_begin = buffer_data_begin;

	//Jump once to be on the first value
	ele_begin += element_size(ele_begin);
	//Jump to the reffered value
	for (size_t i=0 ; i < idx ; ++i) {
		ele_begin += element_size(ele_begin);
		ele_begin += element_size(ele_begin);
	}

	return CBOR(ele_begin, element_size(ele_begin), true);
}

CBOR CBORPair::key_at(size_t idx)
{
	if (!is_pair() || (idx > n_elements())) {
		return CBOR();
	}

	uint8_t *ele_begin = buffer_data_begin;

	//Jump to the reffered key
	for (size_t i=0 ; i < idx ; ++i) {
		ele_begin += element_size(ele_begin);
		ele_begin += element_size(ele_begin);
	}

	return CBOR(ele_begin, element_size(ele_begin), true);
}
