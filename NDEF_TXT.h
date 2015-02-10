/* NDEF_TXT - Handle Text NFC NDEF records
 *
 * Copyright (c) 2015 Eric Brundick <spirilis [at] linux dot com>
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, copy, 
 *  modify, merge, publish, distribute, sublicense, and/or sell copies 
 *  of the Software, and to permit persons to whom the Software is 
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef NDEF_TXT_H
#define NDEF_TXT_H

#include <NDEF.h>

#define NDEF_RTD_TEXT_STATUS_UTF16 0x80

class NDEF_TXT : public NDEF {
    protected:
        // Inherited: tnf, type_length, id_length, payload_length, type, id, payload
        // payload is reused as the pointer to URI
        size_t payload_buf_maxlen;
        boolean is_utf16;
        char lang[8];
        size_t lang_length;

    public:
        NDEF_TXT();
        NDEF_TXT(const char *lang_);
        NDEF_TXT(const char *lang_, const char *text_, boolean utf16 = false);

        int setText(const char *text);
        int setText(const uint8_t *text) { return setText((const char *)text); };
        char * getText() { return (char *)payload; };

        int appendText(const char *text);
        int appendText(const uint8_t *text) { return appendText((const char *)text); };

        void setPayloadBuffer(uint8_t *buf, size_t maxlen) { payload = buf; payload_buf_maxlen = maxlen; };

        void setUTF16(boolean tf) { is_utf16 = tf; };
        boolean isUTF16() { return is_utf16; };

        void setLanguage(const char *lang_);
        const char * getLanguage() { return (const char *)lang; };
        boolean testLanguage(const char *l);  // Compare lang[] case-insensitively with a language specifier, possibly without subtype

        int sendTo(Print &p);   // For outputting to a suitable NFC passive device
        int import(Stream &s);  // For reading from a suitable NFC passive device, or any form of stream
};

#endif /* NDEF_TXT_H */
