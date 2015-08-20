#ifndef INTARRAY_HPP
#define INTARRAY_HPP

class IntArray{
  explicit IntArray(size_t size) : size_(size), data_(new int[size_])
  {
    for(size_t i = 0; i<size_; ++i)
      data_[i] = 0;
  }

  IntArray(IntArray const & a) : size_(a.size_), data_(new int[size_])
  {
    for(size_t i=0; i<size_; ++i)
      data_[i] = a.data_[i];
  }

  IntArray & operator=(IntArray const & a)
  {
    if(this != &a)
      IntArray(a).swap(*this);
    return *this;
  }

  ~IntArray() { delete [] data_; }

  size_t size() const { return size_; }

  int   get(size_t idx) const { return data_[idx]; }
  int & get(size_t idx)  { return data_[idx]; }

  void swap(IntArray &a)
  {
    std::swap(size_, a.size_);
    std::swap(data_, a.data_);
  }

  void resize(size_t nsize)
  {
    IntArray tmp(nsize);
    size_t n = size_ < nsize ? size_ : nsize;
    for(size_t i=0; i<n; ++i)
      tmp.data_[i] = data_[i];
    swap(tmp);
  }

private:
  size_t size_;
  int * data_;

};

#endif // INTARRAY_HPP

