#ifndef BP_MATRIX_H
#define BP_MATRIX_H

#include <vector>
#include <iostream>

template<typename T>
class Matrix:public std::vector<std::vector<T> >
{
  friend std::ostream &operator<<(std::ostream& os, const Matrix<T>& m)
  {
    for(const auto &row: m)
    {
      for(const auto& col: row)
      {
        os<<col<<",";
      }
      os<<std::endl;
    }
    
    return os;
  }

  public:
  Matrix(){}
 ~Matrix(){}
  Matrix(size_t n_row, size_t n_col)
  {
    this->resize(n_row, n_col);
  }


  void resize(size_t n_row, size_t n_col)
  {
    (dynamic_cast<std::vector<std::vector<T> >*>(this))->resize(n_row);
    for(auto &row: *this)
    {
      row.resize(n_col);
    }
  }

  void set_val(T v)
  {
    for(auto &row: *this)
    {
      for(auto& col: row)
      {
        col = v;
      }
    }
  }

};
#endif
