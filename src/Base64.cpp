
static const char* encoded = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz0123456789+/";
				
char *Base64Encode(const char *src, int len, char *dest, int size) {
	char *buffer = dest;
	const char *end = src + len;
	char c;

	if(size < ((len+2)/3)*4) return 0; // Buffer too small
	if(len <= 0) {
		*dest = '\0';
		return buffer;
	}
	
	do {
		*dest++ = encoded[(int)(*src & 0xFC) >> 2];
		c = ((*src) & 0x03) << 4;
		src++;
		if(src == end) {
			*dest++ = encoded[(int)c];
			*dest++ = '=';
			*dest++ = '=';
			break;
		}
		*dest++ = encoded[(int)(*src & 0xF0) >> 4 | c];
		c = ((*src) & 0x0F) << 2;
		src++;
		if(src == end) {
			*dest++ = encoded[(int)c];
			*dest++ = '=';
			break;
		}
		*dest++ = encoded[(int)(*src & 0xC0) >> 6 | c];
		c = (*src) & 0x3F;
		src++;
		*dest++ = encoded[(int)c];
	} while(src < end);
	
	*dest = '\0';
	return buffer;
}

