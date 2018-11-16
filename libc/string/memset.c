#include <string.h>

void *memset(void *dest, int value, size_t count)
{
   unsigned char val = (unsigned char)(value & 0xFF);
   unsigned char *dest2 = (unsigned char*)(dest);

   size_t i = 0;

   while(i < count)
   {
      dest2[i] = val;
      i++;
   }

   return dest;
}

char* toupper(char* string)
{
    char* sv = string;
	while(*sv != '\0')
	{
	    if( *sv >= 'a' && *sv <= 'z')
	        *sv = *sv - ('a' - 'A');
	    sv++;
	}
    return string;
}

char toupper_single(char c)
{
    char sv = c;
	if( sv >= 'a' && sv <= 'z')
		sv = sv - ('a' - 'A');
	return sv;
}