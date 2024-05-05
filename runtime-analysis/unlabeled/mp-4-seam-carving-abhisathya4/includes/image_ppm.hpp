#ifndef IMAGE_PPM_HPP
#define IMAGE_PPM_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "pixel.hpp"

class ImagePPM {
public:
  ImagePPM(int height, int width, int max_color, Pixel** new_arr);

  // default constructor
  //
  // not tested but you are welcome to initialize (or not) member
  // variables however you like in this function
  ImagePPM() = default;

  // overloaded constructor
  //
  // initialize the instance given the file at path. You may assume that
  // path is always valid and that a file exists at path
  ImagePPM(const std::string& path);

  // copy constructor.
  //
  // should intialize the instance as DEEP COPY of source (meaning all
  // values should be the same, but chaning source will not change
  // anything in this instance, and vice versa)
  ImagePPM(const ImagePPM& source);

  // assignment operator
  //
  // should set the instance to be a DEEP COPY of source. Note that the
  // current instance (this) already exists and you should be wary of
  // leaks and the possiblity of null pointers
  ImagePPM& operator=(const ImagePPM& source);

  // destructor
  //
  // releases all heap memory used by the instance
  ~ImagePPM();

  // returns the Pixel at row col. You may assume that row and col
  // will always be within the bounds of image_
  Pixel GetPixel(int row, int col) const { return pixels_[row][col]; }

  // returns the width of the image
  int GetWidth() const { return width_; };

  // returns the height of the image
  int GetHeight() const { return height_; };

  // returns the max color value of the image
  int GetMaxColorValue() const { return max_color_value_; }

  // outputs the image in plain PPM format to os
  friend std::ostream& operator<<(std::ostream& os, const ImagePPM& image);

  // fills in image using the input stream, is. the stream contains
  // an image in plain PPM format
  friend std::istream& operator>>(std::istream& is, ImagePPM& image);

  /**
   * Add any helper methods you may need
   */
  int* GetHorizontalSeam() const;
  int* GetVerticalSeam() const;
  int GetEnergy(int row, int col) const;

private:
  int height_ = 0;
  int width_ = 0;
  int max_color_value_ = 0;
  Pixel** pixels_ = nullptr;  // let pixels_ == local named new_arr

  // given help function, "clears" the data of the instance
  void Clear();

  /**
   * Add any helper methods you may need
   */

  // helpers for GetEnergy
  int NumDeltaCol(int row, int col) const;
  int NumDeltaRow(int row, int col) const;
  int Square(int value) const;

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
  int** CalculateHorizontalSeam() const;
  int BestValueR1(int row, int col, int**& calculated_arr) const;
  int BestValueR2(int row, int col, int**& calculated_arr) const;
  int BestValueR3(int row, int col, int**& calculated_arr) const;

  //
  int* FindHorizontalSeam(int**& calculated_arr) const;
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