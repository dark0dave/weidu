// Code taken from here: https://github.com/OneSadCookie/fcaseopen/blob/master/fcaseopen.c
#include "caml/mlvalues.h"
#include "caml/alloc.h"

#if !defined(_WIN32) || !__APPLE__
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

// r must have strlen(path) + 3 bytes
static int casepath(char const *path, char *r)
{
  size_t l = strlen(path);
  char *p = alloca(l + 1);
  strcpy(p, path);
  size_t rl = 0;

  DIR *d;
  if (p[0] == '/')
  {
    d = opendir("/");
    p = p + 1;
  }
  else
  {
    d = opendir(".");
    r[0] = '.';
    r[1] = 0;
    rl = 1;
  }
  
  int last = 0;
  char *c = strsep(&p, "/");
  while (c)
  {
    if (!d)
    {
      return 0;
    }

    if (last)
    {
      closedir(d);
      return 0;
    }

    r[rl] = '/';
    rl += 1;
    r[rl] = 0;

    struct dirent *e = readdir(d);
    while (e)
    {
      if (strcasecmp(c, e->d_name) == 0)
      {
        strcpy(r + rl, e->d_name);
        rl += strlen(e->d_name);

        closedir(d);
        d = opendir(r);
        
        break;
      }

      e = readdir(d);
    }
    
    if (!e)
    {
      strcpy(r + rl, c);
      rl += strlen(c);
      last = 1;
    }

    c = strsep(&p, "/");
  }

  if (d) closedir(d);
  return 1;
}
#else
static int casepath(char const *path, char *r)
{
  r = path;
  return 0;
}
#endif

CAMLprim value fcase(value path)
{
  const char * cpath = String_val(path);
  char *r = alloca(strlen(cpath) + 3);
  if (casepath(cpath, r))
  {
    return copy_string(r);
  }
  return path;
}
