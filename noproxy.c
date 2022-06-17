#include <stdio.h>
#include <stdbool.h>

#define TRUE    1
#define FALSE   0

#define strcasecompare(a,b) Curl_strcasecompare(a,b)
#define strncasecompare(a,b,c) Curl_strncasecompare(a,b,c)

static bool check_noproxy(const char *name, const char *no_proxy);

int main()
{
    const char* name        = "http.example.com";
    const char* no_proxy    = "com";

    bool result = check_noproxy(name, no_proxy);

    if(result)
        printf("Match\n");
    else
        printf("Unmatch\n");

}

static bool check_noproxy(const char *name, const char *no_proxy)
{
  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  if(no_proxy && no_proxy[0]) {
    size_t tok_start;
    size_t tok_end;
    const char *separator = ", ";
    size_t no_proxy_len;
    size_t namelen;
    char *endptr;
    if(strcasecompare("*", no_proxy)) {
      return TRUE;
    }

    /* NO_PROXY was specified and it wasn't just an asterisk */

    no_proxy_len = strlen(no_proxy);
    if(name[0] == '[') {
      /* IPv6 numerical address */
      endptr = strchr(name, ']');
      if(!endptr)
        return FALSE;
      name++;
      namelen = endptr - name;
    }
    else
      namelen = strlen(name);

    for(tok_start = 0; tok_start < no_proxy_len; tok_start = tok_end + 1) {
      while(tok_start < no_proxy_len &&
            strchr(separator, no_proxy[tok_start]) != NULL) {
        /* Look for the beginning of the token. */
        ++tok_start;
      }

      if(tok_start == no_proxy_len)
        break; /* It was all trailing separator chars, no more tokens. */

      for(tok_end = tok_start; tok_end < no_proxy_len &&
            strchr(separator, no_proxy[tok_end]) == NULL; ++tok_end)
        /* Look for the end of the token. */
        ;

      /* To match previous behaviour, where it was necessary to specify
       * ".local.com" to prevent matching "notlocal.com", we will leave
       * the '.' off.
       */
      if(no_proxy[tok_start] == '.')
        ++tok_start;

      if((tok_end - tok_start) <= namelen) {
        /* Match the last part of the name to the domain we are checking. */
        const char *checkn = name + namelen - (tok_end - tok_start);
        if(strncasecompare(no_proxy + tok_start, checkn,
                           tok_end - tok_start)) {
          if((tok_end - tok_start) == namelen || *(checkn - 1) == '.') {
            /* We either have an exact match, or the previous character is a .
             * so it is within the same domain, so no proxy for this host.
             */
            return TRUE;
          }
        }
      } /* if((tok_end - tok_start) <= namelen) */
    } /* for(tok_start = 0; tok_start < no_proxy_len;
         tok_start = tok_end + 1) */
  } /* NO_PROXY was specified and it wasn't just an asterisk */

  return FALSE;
}


char Curl_raw_toupper(char in)
{
#if !defined(CURL_DOES_CONVERSIONS)
  if(in >= 'a' && in <= 'z')
    return (char)('A' + in - 'a');
#else
  switch(in) {
  case 'a':
    return 'A';
  case 'b':
    return 'B';
  case 'c':
    return 'C';
  case 'd':
    return 'D';
  case 'e':
    return 'E';
  case 'f':
    return 'F';
  case 'g':
    return 'G';
  case 'h':
    return 'H';
  case 'i':
    return 'I';
  case 'j':
    return 'J';
  case 'k':
    return 'K';
  case 'l':
    return 'L';
  case 'm':
    return 'M';
  case 'n':
    return 'N';
  case 'o':
    return 'O';
  case 'p':
    return 'P';
  case 'q':
    return 'Q';
  case 'r':
    return 'R';
  case 's':
    return 'S';
  case 't':
    return 'T';
  case 'u':
    return 'U';
  case 'v':
    return 'V';
  case 'w':
    return 'W';
  case 'x':
    return 'X';
  case 'y':
    return 'Y';
  case 'z':
    return 'Z';
  }
#endif

  return in;
}

/*
 * Curl_strcasecompare() is for doing "raw" case insensitive strings. This is
 * meant to be locale independent and only compare strings we know are safe
 * for this.  See
 * https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/ for some
 * further explanation to why this function is necessary.
 *
 * The function is capable of comparing a-z case insensitively even for
 * non-ascii.
 *
 * @unittest: 1301
 */

int Curl_strcasecompare(const char *first, const char *second)
{
  while(*first && *second) {
    if(Curl_raw_toupper(*first) != Curl_raw_toupper(*second))
      /* get out of the loop as soon as they don't match */
      break;
    first++;
    second++;
  }
  /* we do the comparison here (possibly again), just to make sure that if the
     loop above is skipped because one of the strings reached zero, we must not
     return this as a successful match */
  return (Curl_raw_toupper(*first) == Curl_raw_toupper(*second));
}

int Curl_safe_strcasecompare(const char *first, const char *second)
{
  if(first && second)
    /* both pointers point to something then compare them */
    return Curl_strcasecompare(first, second);

  /* if both pointers are NULL then treat them as equal */
  return (NULL == first && NULL == second);
}

/*
 * @unittest: 1301
 */
int Curl_strncasecompare(const char *first, const char *second, size_t max)
{
  while(*first && *second && max) {
    if(Curl_raw_toupper(*first) != Curl_raw_toupper(*second)) {
      break;
    }
    max--;
    first++;
    second++;
  }
  if(0 == max)
    return 1; /* they are equal this far */

  return Curl_raw_toupper(*first) == Curl_raw_toupper(*second);
}

/* Copy an upper case version of the string from src to dest.  The
 * strings may overlap.  No more than n characters of the string are copied
 * (including any NUL) and the destination string will NOT be
 * NUL-terminated if that limit is reached.
 */
void Curl_strntoupper(char *dest, const char *src, size_t n)
{
  if(n < 1)
    return;

  do {
    *dest++ = Curl_raw_toupper(*src);
  } while(*src++ && --n);
}

/* --- public functions --- */

int curl_strequal(const char *first, const char *second)
{
  return Curl_strcasecompare(first, second);
}
int curl_strnequal(const char *first, const char *second, size_t max)
{
  return Curl_strncasecompare(first, second, max);
}
