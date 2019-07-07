#include "CBORArray.h"

CBORArray::CBORArray(uint8_t* _buffer, size_t buffer_len, bool has_data)
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
			w_ptr += element_size(w_ptr);
		}
	}
	else {
		//Reserve begining of buffer to store table length
		ext_buffer_begin = _buffer;
		buffer_data_begin = _buffer + NUM_ELE_PROVISION;
		buffer_begin = _buffer + NUM_ELE_PROVISION - 1; //type_num_len is 1 for array size 0
		w_ptr = buffer_data_begin;

		//Initialize num_ele
		init_num_ele(0);
	}
}

CBORArray::CBORArray(const CBOR &obj)
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
	}
}

bool CBORArray::append()
{
	increment_num_ele();
	return add();
}

CBOR CBORArray::operator[](size_t idx)
{
	if (!is_array() || (idx > n_elements())) {
		return CBOR();
	}

	//Put buffer on the first element of the table
	uint8_t *ele_begin = buffer_data_begin;
	size_t ele_size = 0;

	//Jump to the reffered element
	for (size_t i=0 ; i < idx ; ++i) {
		ele_begin += element_size(ele_begin);
	}

	return CBOR(ele_begin, element_size(ele_begin), true);
}
