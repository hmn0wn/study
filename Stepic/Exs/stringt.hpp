#ifndef STRING
#define STRING
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iostream>

struct String_
{

  String_(const char *str = "")
  {
    size_ = strlen(str);
    str_ = new char[size_+1];
    strcpy(str_, str);
  }

  String_(size_t n, char c)
  {
    str_ = new char[n+1];
    memset(str_,c,n);
    str_[n] = 0;
  }

  String_ &operator=(const String_ &str) {
          if(this != &str)
              String_(str).swap(*this);
          return *this;
      }

      void swap(String_ &str){
          std::swap(size_, str.size_);
          std::swap(str_, str.str_);
      }
  String_(const String_ &str):size_(str.size()), str_(new char[size_+1])
  {
    memcpy(str_, str.c_str(), size_+1);
  }

  size_t size()
  {
    return size_;
  }

  size_t size() const
  {
    return size_;
  }

  char &at(size_t idx)
  {
    return str_[idx];
  }


  char at(size_t idx) const
  {
    return str_[idx];
  }

  const char *c_str()
  {
    return str_;
  }

  const char *c_str() const
  {
    return (char const *)str_;
  }

  int compare(String_ &str)
  {
    return strcmp(str_,str.c_str());
  }

  void append(String_ &str) {
          char *tmp_str_ = new char[size_ + str.size() +1];
          memcpy(tmp_str_, str_, size_);
          memcpy(tmp_str_+size_, str.c_str(), str.size());
          delete [] str_;
          str_ = tmp_str_;
          size_ +=  str.size();
          str_[size_] = 0;
      }

  ~String_()
  {
    size_ = 0;
    delete [] str_;
  }

private:
  size_t size_;
  char *str_;
};

void TrimRight( char *s )
{
  char *sp_beg = 0;
  while(*s)
    {
      if(*s == ' ')
        {
          if(!sp_beg)  sp_beg = s;
        }
      else
        {
          if(sp_beg) sp_beg = 0;
        }
      ++s;
    }

  if(sp_beg) *sp_beg = '\0';

}

void strcat_(char *to, const char *from)
{
    for(;*to!=0; ++to);
    do{
        *to++ = *from;
    }while(*from++);
}

int strstr_(const char *str, const char *pattern)
{
  if(!*str || !*pattern) return -1;
  const char *strtmp, *patterntmp, *str_ = str;
  do{
      strtmp = str;
      patterntmp = pattern;
      while(*strtmp++ == *patterntmp  && *patterntmp++);
      if (!*patterntmp) return (str-str_);
  }while(*str++);
  return -1;
}

void printall()
{
  char ch = 0;
  bool space = false;
  while(std::cin.get(ch))
    {
    if(ch==' ')
      {
        if(!space)
          {
            space = true;
            std::cout << ch;
          }
      }
    else
      {
        if(space) space = false;
        std::cout << ch;
      }
    }
}

void test_strchr()
{
  char const *str = "you do it whong";
  char const *pch = std::strchr(str,'o');;
  while(pch!=NULL)
    {
      std::cout << pch - str+1 << std::endl;
      pch = std::strchr(pch+1,'o');
    }
}

#endif // STRING

