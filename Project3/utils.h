#pragma once

#include <thread_pool\thread_pool.hpp>
#include <glm/glm.hpp>
#include <SDL.h>
#include <utility>
#include <set>

template< typename T>
inline bool multiplication_is_safe(T a, T b)
{
  size_t a_bits = log2(a);
  size_t b_bits = log2(b);
  return (a_bits + b_bits <= sizeof(T)*8);
}


ThreadPool & getThreadPool();

namespace mika
{
  template<class T>
  inline bool checkDuplicates(std::vector<T, std::allocator<T> > & p_vector)
  {
    int duplicates = 0;
    auto end = p_vector.end();
    end--;
    for(auto orig = p_vector.begin();
        orig != end;
        orig++)
    {
      auto iter = std::find(orig + 1, p_vector.end(), *orig);
      if(iter != p_vector.end())
      {
        duplicates++;
      }
    }
    
//    std::cout << "Duplicate count: " << duplicates << std::endl;
    
    return duplicates == 0;
  }
  template<class T>
  inline bool removeOne(std::vector<T, std::allocator<T> > & p_vector, T const & p_value)
  {
    auto iter = std::find(p_vector.begin(), p_vector.end(), p_value);
    if(iter == p_vector.end())
    {
      return false;
    }
    std::swap(p_vector.back(), *iter);
    p_vector.pop_back();
    return true;
  }
  template<class T>
  inline void removeIndices(std::vector<T, std::allocator<T> > & p_vector, std::vector<size_t> const & p_removeIndices)
  {
    if (p_removeIndices.empty())
    {
      return;
    }
    if (p_removeIndices.size() == p_vector.size())
    {
      p_vector.clear();
      return;
    }
    size_t resultCurrentIndex = 0;
    size_t resultEndIndex = p_vector.size() - 1;
    auto removeFrontIter = p_removeIndices.begin();
    auto removeEndIter = --(p_removeIndices.end());

    while (resultCurrentIndex < resultEndIndex
    && removeFrontIter <= removeEndIter)
    {
      if (resultCurrentIndex != *removeFrontIter)
      {
        // No need to remove.
        ++resultCurrentIndex;
      }
      else // Current item needs to be removed, swap it with the last item that's not going to be removed.
      {
        if (*removeEndIter == resultEndIndex)
        {
          // Current last item needs to be removed
          --removeEndIter;
          --resultEndIndex;
        }
        else
        {
          std::swap(p_vector[resultCurrentIndex++], p_vector[resultEndIndex--]);
          removeFrontIter++;
        }
      }
    }
    p_vector.resize(p_vector.size() - p_removeIndices.size());
  }

  template<class T>
  inline int indexOf(std::vector<T, std::allocator<T> > & p_vector, T const & p_value)
  {
    return std::find(p_vector.begin(), p_vector.end(), p_value) - p_vector.begin();
  }
  static char const * getConstChar(std::string const & p_value)
  {
    static std::set<std::string> values;
    auto iter = values.insert(p_value);
    return iter.first->c_str();
  }
  
  template<class T>
  inline T crossS(double p_scalar, T const & p_vector)
  {
    return T(-p_scalar * p_vector.y, p_scalar * p_vector.x);
  }

  template<class T>
  inline T crossS(T const & p_vector, double p_scalar)
  {
    return T(p_scalar * p_vector.y, -p_scalar * p_vector.x);
  }

  template<class T1, class T2>
  inline double cross(T1 const & p_vector1, T2 const & p_vector2)
  {
    return p_vector1.x * p_vector2.y - p_vector1.y * p_vector2.x;
  }

}

namespace std
{
  template <>
  struct hash<SDL_Keysym>
  {
    size_t operator()(SDL_Keysym const & p_key) const noexcept
    {
      return(
        std::hash<Uint16>()(p_key.mod) * 256 + 
        std::hash<Uint16>()(p_key.scancode) * 512 + 
        std::hash<Sint32>()(p_key.sym)
        );
    }
  };

  template <>
  struct equal_to<SDL_Keysym>
  {
    bool operator()(SDL_Keysym const & p_key1, SDL_Keysym const & p_key2) const noexcept
    {
      return p_key1.mod == p_key2.mod
        && p_key1.scancode == p_key2.scancode
        && p_key1.sym == p_key2.sym;
    }
  };
}

namespace glm
{
  inline u64vec2 operator/(glm::u64vec2 const & p_orig, int p_divider)
  {
    return u64vec2(p_orig.x / p_divider, p_orig.y / p_divider);
  }
}
template<class T1, class T2>
inline double distanceSquared(T1 p_pos1, T2 p_pos2)
{
  return pow(double(p_pos1.x) - p_pos2.x, 2) + pow(double(p_pos1.y) - p_pos2.y, 2);
}
