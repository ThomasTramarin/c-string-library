#ifndef SL_STRING_H
#define SL_STRING_H

typedef char *sl_str; // opaque type

sl_str sl_from_cstr(const char *init);
void sl_free(sl_str str);

#endif // SL_STRING_H