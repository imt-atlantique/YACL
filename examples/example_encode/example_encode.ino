#include "YACL.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("JSON Encoding of the following data:");
	Serial.println("{");
	Serial.println("\t \"lat\": 48.12010,");
	Serial.println("\t \"lon\": -1.62863,");
	Serial.println("\t \"temperatures\": [25, 24, 24, 26]");
	Serial.println("}");

	// Data is a dictionnary of key/values, which corresponds to CBORPair objects
	// Thus, we create a CBORPair, and reserve a buffer of 100 bytes for it.
	// This is not mandatory, as CBORPair can reallocate a bigger buffer on the
	// fly, but preferable in terms of memory management.
	CBORPair data = CBORPair(100);

	// First element of the dictionnary is {"lat": 48.12010}
	data.append("lat", 48.12010);

	// Second element is {"lon": -1.62863}
	data.append("lon", -1.62863);

	// Last element is {"temperatures": [25, 24, 24, 26]}
	// For that, we first need to store [25, 24, 24, 26] as a CBOR Array
	CBORArray temps_cbor = CBORArray(20); //Reserve a buffer of 20 bytes (optional)
	// Populate the cbor Array, either this way:
	int temps[4] = {25, 24, 24, 26};
	temps_cbor.append(temps, 4);
	// Or this way:
	// temps_cbor.append(25);
	// temps_cbor.append(24);
	// temps_cbor.append(24);
	// temps_cbor.append(26);

	// Append array to our data
	data.append("temperatures", temps_cbor);

	// That's it! Let's see how our data looks.
	Serial.println("CBOR Encoded data:");
	const uint8_t *cbor_encoded = data.to_CBOR();
	for (size_t i=0 ; i < data.length() ; ++i) {
		if (cbor_encoded[i] < 0x10) {
			Serial.print('0');
		}
		Serial.print(cbor_encoded[i], HEX);
	}
	Serial.println("\n You can check this encoding using http://cbor.me");
}

void loop()
{
}
