// CubitString.cpp


#include "CubitString.hpp"

#ifdef WIN32
#pragma warning ( 4 : 4291 4244 4305 4018 4786)
#include <windows.h>
#endif

#include <iomanip>

#include <cstddef>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <stdexcept>

class CubitStringRep
{
private:
  friend class CubitString;
  int    refCount;
  char*  chars;
  
  CubitStringRep(const char*);
  CubitStringRep(char*, int);
    //- NOTE: The above function is specialized for the implementation
    //-       It USES the pointer passed to it instead of allocating and
    //-       copying.
  CubitStringRep(int, const char*);
  
  CubitStringRep(const CubitStringRep &s);
  ~CubitStringRep();
  
  void increment();
  void decrement();
  
  const CubitStringRep& operator=(const CubitStringRep &s);
  static CubitStringRep* empty_string_rep();
};

// This is the function that creates and returns the empty string rep.
CubitStringRep* CubitStringRep::empty_string_rep()
{
  static CubitStringRep empty_rep(1, "");
  return &empty_rep;
}

CubitStringRep::CubitStringRep(const char *cp)
    : refCount(0), chars(new char[strlen(cp) +1])
{
  strcpy(chars, cp);
}

CubitStringRep::CubitStringRep(char *cp, int)
    : refCount(0), chars(cp)
{}

CubitStringRep::CubitStringRep(int ref, const char *cp)
    : refCount(ref), chars(new char[strlen(cp) +1])
{
  strcpy(chars, cp);
}

CubitStringRep::~CubitStringRep()
{
  delete [] chars;
}

void CubitStringRep::increment() 
{
  ++refCount;
}

void CubitStringRep::decrement()
{
  if (--refCount == 0)
    delete this;
}

CubitStringRep::CubitStringRep(const CubitStringRep &orig)
    : refCount(0), chars(new char[strlen(orig.chars)+1])
{
  strcpy(chars, orig.chars);
}

const CubitStringRep& CubitStringRep::operator=(const CubitStringRep &orig)
{
  if (this != &orig)
  {
    if (strlen(chars) != strlen(orig.chars))
    {
      delete [] chars;
      chars = new char[strlen(orig.chars)+1];
    }
    strcpy(chars, orig.chars);
  }
  return *this;
}


CubitString::CubitString()
{
  rep = CubitStringRep::empty_string_rep();
  rep->increment();
}

CubitString::CubitString(const CubitString& s)
    : rep(s.rep)
{
  rep->increment();
}


CubitString::~CubitString()
{
  rep->decrement();
}

CubitString::CubitString(const char *s)
{
    // If it's an empty string...
  if (!s || s[0] == '\0')
    rep = CubitStringRep::empty_string_rep();
  else
    rep = new CubitStringRep(s);
  rep->increment();
}

CubitString::CubitString(const int i)
{
  char si[32];
  sprintf(si, "%d", i);
  rep = new CubitStringRep(si);
  rep->increment();
}

CubitString::CubitString(const double f, const unsigned int max_length, const unsigned int sig_digits)
{
  char format_string[8];
  char *si = new char[ 2*max_length + 32 ];
  if (sig_digits)
  {
    sprintf(format_string, "%%.%de", sig_digits);
    sprintf(si, format_string, f);
  }
  else
    sprintf(si, "%g", f);
    // change precision to be short enough
  if (max_length) 
  {
    if ( strlen( si ) > max_length ) 
    {
      
      //char digit_string[4];
      //strcpy(digit_string,"xxx");
    
      int num_digits = max_length;
      do
      {        
          // create format string - specify one fewer digit
        num_digits--;
        sprintf(format_string, "%%.%dg", num_digits);
        //strcpy( format_string, "%." );
        //sprintf( digit_string, "%d", num_digits );
        //strcat( format_string, digit_string );
        //strcat( format_string, "g" );
        
          // create output string
        sprintf( si, format_string, f );
      } while( num_digits > 0 && strlen( si ) > max_length );
    }
  }
  rep = new CubitStringRep(si);
  rep->increment();
  delete [] si;
}

CubitString& CubitString::operator=(const CubitString& s)
{
  if (rep != s.rep)
  {
    rep->decrement();
    rep = s.rep;
    rep->increment();
  }
  return *this;
}

char CubitString::get_at(size_t pos) const
{
  //assert(pos < strlen(rep->chars));
  if(pos<0 || pos>strlen(rep->chars))
    throw std::invalid_argument("Position must be between 0 and the length of string");
  return rep->chars[pos];
}

size_t CubitString::length() const
{
  return strlen(rep->chars);
}

const char * CubitString::c_str() const
{
    //assert(this != NULL);  // <--- Add this line
  return rep->chars;
}

CubitString operator+(const CubitString& s1, const CubitString& s2)
{
  return CubitString(s1) += s2;
}

CubitString operator+(const CubitString& s1, const char *c2)
{
  return CubitString(s1) += c2;
}

CubitString operator+(const char *c1, const CubitString& s2)
{
  return CubitString(c1) += s2;
}

CubitString& CubitString::operator+=(const CubitString& s)
{
  if (s.length() > 0)
  {
    int new_count = length() + s.length();
    char *buf = new char[new_count + 1];
    
    memcpy(buf,rep->chars,length());
    memcpy(buf+length(),s.rep->chars,s.length());
    buf[new_count] = '\0';
    rep->decrement();
    rep = new CubitStringRep(buf, 1);
    rep->increment();
  }
  return *this;
}

CubitString& CubitString::operator+=(const char *c)
{
  if (c)
  {
    size_t c_length;
    if ((c_length = strlen(c)) > 0)
    {
      size_t new_count = length() + c_length;
      char *buf = new char[new_count + 1];
      
      memcpy(buf,rep->chars,length());
      memcpy(buf+length(),c,c_length);
      buf[new_count] = '\0';
      rep->decrement();
      rep = new CubitStringRep(buf, 1);
      rep->increment();
    }
  }
  return *this;
}

size_t CubitString::find(const CubitString& s, size_t pos) const
{
  //assert(pos < length());
  if(pos > length())
    throw std::invalid_argument("Positions is greater than length");
  char *p = strstr(rep->chars,s.c_str());
  if (p)
    return pos + (p - rep->chars);
  return MAX_POS;
}

size_t CubitString::find_first_of(const CubitString& s, size_t pos) const
{
  if (pos < length())
  {
    char *p = strpbrk(rep->chars+pos,s.rep->chars);
    if (p)
      return p - rep->chars;
  }
  return MAX_POS;
}

size_t CubitString::find_first(char c, size_t pos) const
{
  if (pos < length())
  {
    char *p = strchr(rep->chars+pos, c);
    if (p)
      return p - rep->chars;
  }
  return MAX_POS;
}

size_t CubitString::find_last(char c, size_t pos) const
{
  if (pos > length())
    pos = length();
  char save_char = rep->chars[pos];
  rep->chars[pos] = '\0';
  char* p = strrchr(rep->chars, c);
  rep->chars[pos] = save_char;
  if (p)
    return p - rep->chars;
  return MAX_POS;
}


CubitString CubitString::substr(size_t first, size_t count) const
{
  size_t len = length();
  if (first >= len)
    return CubitString();

  if (first == 0 && count >= len)
    return *this;
  
  if (count > len - first)
    count = len - first;
  
  CubitString rv;
  if (count > 0)
  {
    char *substring = new char[count+1];
    strncpy(substring, rep->chars+first, count);
    substring[count] = '\0';
    rv = substring;
    delete [] substring;
  }
  return rv;
}

CubitString CubitString::segmentstr(size_t first, size_t first_not) const
{
  if (first_not <= first)
    return CubitString();
  return substr(first, first_not - first);
}

std::ostream & operator<<(std::ostream& os, const CubitString& s)
{
  unsigned int width = os.width();
  width = (width == 0 ? s.length() : width);
  if (s.length() <= width)
  {
    os.write(s.c_str(),s.length());
    for (unsigned int i=s.length(); i<width; i++)
      os.put(' ');
  }
  else {
    os.write(s.c_str(),width);
  }
  return os;
}

void CubitString::put_at(size_t pos, char c)
{
  //assert(pos < length());
  if(pos < 0 || pos >= length())
    throw std::invalid_argument("Index out of Bounds");
  if (rep->chars[pos] != c)
  {
    if (pos < length())
    {
      if (rep->refCount != 1)
      {
        CubitStringRep *new_rep = new CubitStringRep(rep->chars);
        rep->decrement();
        rep = new_rep;
        rep->increment();
      }
      rep->chars[pos] = c;
    }
    else
    {
        // Append character the lazy way
      size_t new_count = length() + 1;
      char *buf = new char[new_count + 1];
      memcpy(buf, rep->chars, length());
      buf[new_count -1] = c;
      buf[new_count] = '\0';
      rep->decrement();
      rep = new CubitStringRep(buf, 1);
      rep->increment();
    }
  }
}

void CubitString::to_lower() 
{
  char *p = rep->chars;
  if (rep->refCount != 1)
  {
    while (*p != '\0'  && *p == tolower(*p))
    {
      p++;
    }
    if (*p != '\0')
    {
      CubitStringRep *new_rep = new CubitStringRep(rep->chars);
      p = new_rep->chars + (p - rep->chars);
      rep->decrement();
      rep = new_rep;
      rep->increment();
    }
  }
  
    // convert this string to lower case
  to_lower(p);
}

void CubitString::to_upper() 
{
  char *p = rep->chars;
  if (rep->refCount != 1)
  {
    while (*p != '\0'  && *p == toupper(*p))
    {
      p++;
    }
    if (*p != '\0')
    {
      CubitStringRep *new_rep = new CubitStringRep(rep->chars);
      p = new_rep->chars + (p - rep->chars);
      rep->decrement();
      rep = new_rep;
      rep->increment();
    }
  }
  
    // convert this string to lower case
  to_upper(p);
}

void CubitString::tokenize( const char *delimiter, std::vector<CubitString> &strings )
{
  char* tmp_word;
  tmp_word = strtok( rep->chars, delimiter );
  if( tmp_word != NULL )
    strings.push_back( CubitString( tmp_word ) );

  while( tmp_word=strtok( NULL, delimiter ) )
    strings.push_back( CubitString( tmp_word ) );
}

bool CubitString::operator==(const CubitString& s2) const
{
  return (rep == s2.rep) || (strcmp(rep->chars, s2.rep->chars) == 0);
}

bool CubitString::operator!=(const CubitString& s2) const
{
  return (rep != s2.rep) && (strcmp(rep->chars, s2.rep->chars) != 0);
}

bool CubitString::operator==(const char *s2) const
{
  return strcmp(rep->chars, s2) == 0;
}

bool CubitString::operator!=(const char *s2) const
{
  return strcmp(rep->chars, s2) != 0;
}

bool operator<=(const CubitString& s1, const CubitString& s2)
{
  return strcmp(s1.c_str(), s2.c_str()) <= 0;
}

bool operator>=(const CubitString& s1, const CubitString& s2)
{
  return strcmp(s1.c_str(), s2.c_str()) >= 0;
}

bool operator<( const CubitString& s1, const CubitString& s2 )
{ return strcmp( s1.c_str(), s2.c_str() ) < 0; }

bool operator>( const CubitString& s1, const CubitString& s2 )
{ return strcmp( s1.c_str(), s2.c_str() ) > 0; }

#if defined(WIN32)

std::wstring CubitString::toUtf16(const char* str)
{
  // there are other methods (e.g. C++11), but since its Windows specific to support Windows APIs, just use their API.
  std::wstring ret;
  int sz = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  if(sz)
  {
    std::vector<wchar_t> result(sz);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &result[0], sz);
    ret = &result[0];
  }
  return ret;
}

CubitString CubitString::toUtf8(const wchar_t* str)
{
  CubitString ret;
  int sz = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
  if(sz)
  {
    std::vector<char> result(sz);
    WideCharToMultiByte(CP_UTF8, 0, str, -1, &result[0], sz, NULL, NULL);
    ret = &result[0];
  }
  return ret;
}

#endif

#ifdef TEST_STRING

void main() {
CubitString a = "Test ";
CubitString b = "String Class";
CubitString blank(' ');

CubitString c = a + b;
c+= b;
CubitString e = c;
CubitString f = c;
CubitString g = c;
g.put_at(1, 'Z');
cout << "G = " << g << endl;
cout << "C = " << c << endl;

c.put_at(1, 'X');
CubitString d = b;
d.put_at(1, 'Y');
}
#endif
