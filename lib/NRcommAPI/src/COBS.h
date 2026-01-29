    #pragma once
    #include <stdint.h>
//先人の知恵 from wikipedia
namespace COBS{
    uint16_t encode(const void *data, uint16_t length, uint8_t *buffer)
    {
        if(!data ||!buffer)return 0;
        uint8_t *encode = buffer;  // Encoded byte pointer
        uint8_t *codep = encode++; // Output code pointer
        uint8_t code = 1;          // Code value
        for (const uint8_t *byte = (const uint8_t *)data; length--; ++byte)
        {
            if (*byte) // Byte not zero, write it
                *encode++ = *byte, ++code;
            if (!*byte || code == 0xff) // Input is zero or block completed, restart
            {
                *codep = code, code = 1, codep = encode;
                if (!*byte || length)
                    ++encode;
            }
        }
        *codep = code; // Write final code value
        return (uint16_t)(encode - buffer);
    }

    uint16_t decode(const uint8_t *buffer, uint16_t length, void *data)
{
	 if(!data ||!buffer)return 0;

	const uint8_t *byte = buffer; // Encoded input byte pointer
	uint8_t *decode = (uint8_t *)data; // Decoded output byte pointer

	for (uint8_t code = 0xff, block = 0; byte < buffer + length; --block)
	{
		if (block) // Decode block byte
			*decode++ = *byte++;
		else
		{
			block = *byte++;             // Fetch the next block length
			if (block && (code != 0xff)) // Encoded zero, write it unless it's delimiter.
				*decode++ = 0;
			code = block;
			if (!code) // Delimiter code found
				break;
		}
	}

	return (uint16_t)(decode - (uint8_t *)data);
}
}