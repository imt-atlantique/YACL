## Advanced usage

### Tags

YACL has basic support of CBOR tags.

To encode a tag, use the following constructor:
```c++
CBOR(int tag_value, CBOR tag_item);
```
To decode tags, use the following methods:
```c++
int tag_val = cbor_obj.get_tag_value();
CBOR tag_item = cbor_obj.get_tag_item();
```

For instance, to tag `http://www.example.com` as an URI (tag 32, see https://tools.ietf.org/html/rfc7049#section-2.4):
```c++
CBOR uri = CBOR("http://www.example.com");
CBOR tagged_uri = CBOR(32, uri);
```
Likewise, to decode a received CBOR tag containing an URI:
```c++
char decoded_uri[50]; //Reserve 50 characters for the string
tagged_uri.get_tag_item().get_string(decoded_uri);
Serial.println(decoded_uri);
```

### Type decoding

Casting of CBOR objects to native types (int, float, string, etc.) is not safe. For instance, casting a CBOR 8-bits integer to a string will raise no error, but the behavior is undefined (and may lead to memory corruption).
Hence, before attempting such casting, the type of the CBOR should be determined using `is_*()` methods.
Please find bellow the list of `is_*()` available methods:
```c++
//Unsigned integers
bool is_uint8();
bool is_uint16();
bool is_uint32();
bool is_uint64();

//Signed integers
bool is_int8();
bool is_int16();
bool is_int32();
bool is_int64();

//Floating point values
bool is_float16();
bool is_float32();
bool is_float64();

//Other types
bool is_null();
bool is_bool();
bool is_string();
bool is_array();
bool is_pair();
bool is_tag();
```

Note that for (unsigned) integers and floats, `is_type*()` indicated that the CBOR objects fits into a type of size `*` bits. For instance:
```c++
CBOR val = CBOR(123);

Serial.println(val.is_uint8);  //True
Serial.println(val.is_uint16); //True
Serial.println(val.is_uint32); //True
Serial.println(val.is_uint64); //True
```

```c++
CBOR val = CBOR(1230);

Serial.println(val.is_uint8);  //False
Serial.println(val.is_uint16); //True
Serial.println(val.is_uint32); //True
Serial.println(val.is_uint64); //True
```

### Is data copied when accessing elements of an Array, a Pair or a tag item ?

No.
This means that when doing this, for example:
```c++
CBOR arr = CBORArray();
arr.append(0);
arr.append(1);

CBOR ele1 = arr[1];
```
Then, the actual data is stored in `arr`, and only in `arr`. Hence, modifications of `arr` can impact `ele1`, and vice versa.

If one wishes to do a copy, then he/she can use the copy constructor:
```c++
CBOR arr = CBORArray();
arr.append(0);
arr.append(1);

CBOR ele1 = CBOR(arr[1]);
```
In this case, `ele1` actually stores a copy of the CBOR representation of `1`.
