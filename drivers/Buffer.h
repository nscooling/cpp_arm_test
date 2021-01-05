// -----------------------------------------------------------------------------
// Buffer.h
//
// DISCLAIMER:
// Feabhas is furnishing this item "as is". Feabhas does not provide any 
// warranty of the item whatsoever, whether express, implied, or statutory, 
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will 
// be error-free.
// In no respect shall Feabhas incur any liability for any damages, including, 
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether 
// or not based upon warranty, contract, tort, or otherwise; whether or not 
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any 
// services that may be provided by Feabhas.
// -----------------------------------------------------------------------------

#ifndef BUFFER_H_
#define BUFFER_H_

#include <cstddef>
#include <array>

enum class Error { OK, EMPTY, FULL };

template <typename T = int, size_t sz = 8>
class Buffer
{
public:
  Buffer()  = default;
  ~Buffer() = default;

  // Adds an element to the buffer.
  // Returns Error::OK if the value has been stored.
  // Returns Error::FULL if the value cannot be stored.
  //
  Error add(const T& in_val);

  // Retrieves an element from the buffer.
  // Returns Error::OK if there was a value in the Buffer.
  // Returns Error::EMPTY if the Buffer was empty; the
  // inout_val will be unmodified in this case.
  //
  Error get(T& inout_val);

  // Returns true if size() == 0; otherwise false,
  //
  inline bool is_empty() const;

  // Returns the current number of items
  // in the Buffer.
  //
  inline size_t size() const;

private:
  std::array<T, sz> buffer;
  typename std::array<T, sz>::iterator read  { std::begin(buffer) };
  typename std::array<T, sz>::iterator write { std::begin(buffer) };
  unsigned num_items { 0 };
};


template <typename T, size_t sz>
Error Buffer<T, sz>::add(const T& in_val)
{
  if (num_items == sz) return Error::FULL;

  *write = in_val;
  ++num_items;
  ++write;
  if (write == buffer.end()) write = buffer.begin();

  return Error::OK;
}


template <typename T, size_t sz>
Error Buffer<T, sz>::get(T& inout_val)
{
  if(num_items == 0) return Error::EMPTY;

  inout_val = *read;
  --num_items;
  ++read;
  if(read == buffer.end()) read = buffer.begin();

  return Error::OK;
}


template <typename T, size_t sz>
bool Buffer<T, sz>::is_empty() const
{
  return (num_items == 0);
}


template <typename T, size_t sz>
size_t Buffer<T, sz>::size() const
{
  return num_items;
}

#endif // BUFFER_H_
