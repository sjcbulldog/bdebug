#include "MiscUtilsHex.h"

namespace bwg
{
	namespace misc
	{
        uint64_t MiscUtilsHex::hexToInt(const char* str, int len)
        {
            uint64_t ret = 0;

            while (len-- > 0)
            {
                char ch = *str++;
                uint8_t digit = 0;

                if (ch >= '0' && ch <= '9')
                    digit = ch - '0';
                else if (ch >= 'A' && ch <= 'F')
                    digit = ch - 'A' + 10;
                else if (ch >= 'a' && ch <= 'f')
                    digit = ch - 'a' + 10;

                ret = (ret << 4) | digit;
            }

            return ret;
        }
	}
}