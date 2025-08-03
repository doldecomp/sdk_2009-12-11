#ifndef __STDC_CTYPE_H__
#define __STDC_CTYPE_H__

extern int tolower(int c);

#if defined(NDEBUG)
inline int tolower(int c)
{
	/* Astute readers may notice that the definition of _current_locale here
	 * differs from that of the one in MSL. This is a reference to the fact
	 * that I don't give a fuck about the fields I'm not using, because that
	 * would be stupid and useless.
	 */
	extern struct
	{
		char _[0x38];
		struct
		{
			char _[0x10];
			unsigned char *lower_map_ptr;
		} *ctype_cmpt_ptr;
	} _current_locale;

	return c < 0 || c >= 256 ? c
	                         : _current_locale.ctype_cmpt_ptr->lower_map_ptr[c];
}
#endif

#endif // __STDC_CTYPE_H__
