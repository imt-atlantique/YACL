#include "YACL.h"

bool buffer_equals(const uint8_t* buf1, size_t len_buf1, const uint8_t* buf2, size_t len_buf2, bool verbose=true)
{
	if (len_buf1 != len_buf2) {
		if (verbose) {
			Serial.print("Lengths do not match: ");
			Serial.print(len_buf1);
			Serial.print(" != ");
			Serial.println(len_buf2);
		}

		return false;
	}

	bool flag = true;

	for (size_t i=0 ; i < len_buf2 ; ++i) {
		if (buf1[i] != buf2[i]) {
			if (verbose) {
				flag = false;
				Serial.print((unsigned int)buf1[i], HEX);
				Serial.print(" != ");
				Serial.print((unsigned int)buf2[i], HEX);
				Serial.print(" at index ");
				Serial.println((unsigned int)i);
			}
			else {
				return false;
			}
		}
	}

	if (!flag) {
		Serial.println("Content of buf1:");
		for (size_t i=0 ; i < len_buf1 ; ++i) {
			Serial.print((unsigned int)buf1[i], HEX);
			Serial.print(' ');
		}

		Serial.println("\nContent of buf2:");
		for (size_t i=0 ; i < len_buf2 ; ++i) {
			Serial.print((unsigned int)buf2[i], HEX);
			Serial.print(' ');
		}
		Serial.print('\n');
	}

	return flag;
}

bool test_array()
{
	const uint8_t expected[] = {0x83, 0x01, 0xFA, 0x40, 0x4A, 0x3D, 0x71, 0x64, 0x74, 0x65, 0x73, 0x74};
	size_t len_expected = 12;
	bool good_alloc = true;
	CBORArray cbor = CBORArray();

	good_alloc &= cbor.append(1);
	good_alloc &= cbor.append((float)3.16);
	good_alloc &= cbor.append("test");

	if (!good_alloc) {
		Serial.println("Cannot test: bad alloc.");
		return false;
	}

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

bool test_pair()
{
	const uint8_t expected[9] = {0xa2, 0x02, 0x02, 0x64, 0x4e, 0x75, 0x6c, 0x6c, 0xf6};
	size_t len_expected = 9;
	bool good_alloc = true;
	CBORPair cbor = CBORPair();

	good_alloc &= cbor.append(CBOR(2), 2);
	good_alloc &= cbor.append("Null", CBOR());

	if (!good_alloc) {
		Serial.println("Cannot test: bad alloc.");
		return false;
	}

	if (buffer_equals(expected, len_expected, cbor.to_CBOR(), cbor.length())) {
		return true;
	}

	return false;
}

void setup()
{
	//Basic CBOR types
	Serial.begin(115200);
	Serial.println("Begining test sequence!");

	Serial.print("{2: 2, \"Null\": NULL} : ");
	if (test_pair()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}

	Serial.print("[1, 3.16, \"test\"] : ");
	if (test_array()) {
		Serial.println("OK");
	}
	else {
		Serial.println("NOK");
	}
}

void loop()
{
}
