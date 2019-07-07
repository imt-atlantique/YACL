#include "CBORPair.h"

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
