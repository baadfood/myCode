#ifndef SharedPtr_h_
#define SharedPtr_h_

#include <memory>

template<class T>
class SharedPtr
{
public:
  SharedPtr() :
  m_ptr(nullptr)
  {
  }

  SharedPtr(T * p_ptr):
  m_ptr(new T*)
  {
    (*m_ptr) = p_ptr;
  }

  SharedPtr(SharedPtr const & p_other):
  m_ptr(p_other.m_ptr)
  {
  }

  void reset()
  {
    m_ptr = nullptr;
  }

  void operator=(SharedPtr const & p_other)
  {
    m_ptr = p_other.m_ptr;
  }

  void operator=(T* p_ptr)
  {
    if (m_ptr == nullptr)
    {
      m_ptr.reset(new T*);
    }
    *(m_ptr) = p_ptr;
  }

  T const * get() const
  {
    if (m_ptr == nullptr)
    {
      return nullptr;
    }
    return *m_ptr;
  }

  T * get()
  {
    if (m_ptr == nullptr)
    {
      return nullptr;
    }
    return *m_ptr;
  }

  void redirect(SharedPtr const & p_other)
  {
    assert(p_other.m_ptr != nullptr);
    operator=(*p_other.m_ptr);
  }

  bool operator!=(SharedPtr const & p_other)
  {
    return !operator==(p_other);
  }

  bool operator!=(T * p_other)
  {
    return !operator==(p_other);
  }

  bool operator==(SharedPtr const & p_other)
  {
    if (m_ptr == nullptr)
    {
      return false;
    }
    return *(m_ptr) == *(p_other.m_ptr);
  }

  bool isNull() const
  {
    return m_ptr == nullptr;
  }

  bool operator==(T const * p_data)
  {
    if (m_ptr == nullptr)
    {
      return false;
    }
    return *(m_ptr) == p_data;
  }

private:
  std::shared_ptr<T*> m_ptr;
};

#endif