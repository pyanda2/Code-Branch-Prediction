#ifndef SEAM_CARVER_HPP
#define SEAM_CARVER_HPP

#include "image_ppm.hpp"

class SeamCarver {
public:
  int** CalculateHorizontalSeam() const;
  int* FindHorizontalSeam(int**& calculated_arr) const;
  // default constructor
  //
  // Not tested, but you are welcome to intialize (or not) member variables
  // however you like in this function
  SeamCarver() = default;

  // overloaded constructor
  //
  // sets the instances' image_ as a DEEP COPY of the image parameter. Note
  // that if ImagePPM's copy constructor is implemented correctly, you
  // shouldn't have to worry about the details of a deep copy here
  SeamCarver(const ImagePPM& image);

  // sets the instances' image_ as a DEEP COPY of the image parameter. Same
  // note as above regarding ImagePPM
  void SetImage(const ImagePPM& image);

  void SetHeight(int height);

  void SetWidth(int width);

  // returns the instance's image_
  const ImagePPM& GetImage() const { return image_; }

  // returns the image's height
  int GetHeight() const { return height_; }

  // returns the image's width
  int GetWidth() const { return width_; }

  // returns the energy of the pixel at row col in image_
  int GetEnergy(int row, int col) const;  // done

  // returns the horizontal seam of image_ with the least amount of
  // energy
  //
  // the ith int in the returned array corresponds to which row at
  // col i is in the seam. example:
  //
  //    | x |   |
  // ---+---+---+---
  //  x |   | x |
  // ---+---+---+---
  //    |   |   | x
  // returns {1, 0, 1, 2}
  int* GetHorizontalSeam() const;

  // returns the vertical seam of image_ with the least amount of
  // energy
  //
  // the ith int in the returned array corresponds to which col at
  // row i is in the seam. example:
  //
  //    | x |   |
  // ---+---+---+---
  //    |   | x |
  // ---+---+---+---
  //    |   | x |
  // returns {1, 2, 2}
  int* GetVerticalSeam() const;

  // removes one horizontal seam in image_. example:
  //
  // image_ before:
  //  0 | 1 | 2 | 3
  // ---+---+---+---
  //  4 | 5 | 6 | 7
  // ---+---+---+---
  //  8 | 9 | 10| 11
  //
  // seam to remove:
  //    | x |   |
  // ---+---+---+---
  //  x |   | x |
  // ---+---+---+---
  //    |   |   | x
  //
  // image_ after:
  //  0 | 5 | 2 | 3
  // ---+---+---+---
  //  8 | 9 | 10| 7
  void RemoveHorizontalSeam();

  // removes one vertical seam in image_. example:
  //
  // image_ before:
  //  0 | 1 | 2 | 3
  // ---+---+---+---
  //  4 | 5 | 6 | 7
  // ---+---+---+---
  //  8 | 9 | 10| 11
  //
  // seam to remove:
  //    | x |   |
  // ---+---+---+---
  //    |   | x |
  // ---+---+---+---
  //    |   | x |
  //
  // image_ after:
  //  0 | 2 | 3
  // ---+---+---
  //  4 | 5 | 7
  // ---+---+---
  //  8 | 9 | 11
  void RemoveVerticalSeam();

  /**
   * Add any helper methods you may need
   */

private:
  ImagePPM image_;
  int height_ = 0;
  int width_ = 0;

  /**
   * Add any helper methods you may need
   */
  // helpers for GetEnergy
  int NumDeltaCol(int row, int col) const;
  int NumDeltaRow(int row, int col) const;
  int Square(int value) const;

  // helpers for GetVerticalSeam : calculate && find
  //
  int** CalculateVerticalSeam() const;
  int BestValue1(int row, int col, int**& calculated_arr) const;
  int BestValue2(int row, int col, int**& calculated_arr) const;
  int BestValue3(int row, int col, int**& calculated_arr) const;

  //
  int* FindVerticalSeam(int**& calculated_arr) const;
  int StartingCol(int**& calculated_arr) const;
  int FindNextCol1(int row, int current_col, int**& calculated_arr) const;
  int FindNextCol2(int row, int current_col, int**& calculated_arr) const;
  int FindNextCol3(int row, int current_col, int**& calculated_arr) const;

  // helpers for GetHorizontalSeam : calculate && find
  //
  // int** CalculateHorizontalSeam() const;
  int BestValueR1(int row, int col, int**& calculated_arr) const;
  int BestValueR2(int row, int col, int**& calculated_arr) const;
  int BestValueR3(int row, int col, int**& calculated_arr) const;

  //
  // int* FindHorizontalSeam(int**& calculated_arr) const;
  int StartingRow(int**& calculated_arr) const;
  int FindNextRow1(int row, int current_col, int**& calculated_arr) const;
  int FindNextRow2(int row, int current_col, int**& calculated_arr) const;
  int FindNextRow3(int row, int current_col, int**& calculated_arr) const;

  /*
    // helpers for GetHorizontalArry
    int StartingRow() const;
    int FindNextRow1(int current_row, int col) const;
    int FindNextRow2(int current_row, int col) const;
    int FindNextRow3(int current_row, int col) const;
    // helpers for GetVerticalArry
    int StartingCol() const;
    int FindNextCol1(int row, int current_col) const;
    int FindNextCol2(int row, int current_col) const;
    int FindNextCol3(int row, int current_col) const;
    */
};

#endif