#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here
void SeamCarver::SetHeight(int height) { height_ = height; }

void SeamCarver::SetWidth(int width) { width_ = width; }
// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM &image) : image_(image)
{
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM &image)
{
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}

int SeamCarver::GetEnergy(int row, int col) const
{
  int delta_col = NumDeltaCol(row, col);
  int delta_row = NumDeltaRow(row, col);
  int energy = delta_col + delta_row;
  return energy;
}

int *SeamCarver::GetVerticalSeam() const
{
  int **calculated_arr = CalculateVerticalSeam(); //
  int *vertical_arr = FindVerticalSeam(calculated_arr);
  for (int i = 0; i < height_; ++i)
  {
    delete[] calculated_arr[i];
  }
  delete[] calculated_arr;
  calculated_arr = nullptr;
  return vertical_arr;
}

int *SeamCarver::GetHorizontalSeam() const
{
  int **calculated_arr = CalculateHorizontalSeam();
  int *horizontal_arr = FindHorizontalSeam(calculated_arr);
  for (int i = 0; i < height_; ++i)
  {
    delete[] calculated_arr[i];
  }
  delete[] calculated_arr;
  calculated_arr = nullptr;
  return horizontal_arr;
}

// helper for GetHorizontalSeam : calculate and find
int **SeamCarver::CalculateHorizontalSeam() const
{
  // Init a calculated_arr;
  int **calculated_arr = new int *[height_];
  for (int row = 0; row < height_; ++row)
  {
    calculated_arr[row] = new int[width_];
  }
  // fill it; start from the last col (width_ - 1);
  // for the last col;
  for (int row = 0; row < height_; ++row)
  {
    calculated_arr[row][width_ - 1] = image_.GetEnergy(row, width_ - 1);
  }
  // fill all the other rows;
  for (int col = width_ - 2; col != -1; --col)
  {
    for (int row = 0; row < height_; ++row)
    {
      int current_value = image_.GetEnergy(row, col);
      if (row - 1 < 0) [[likely]]
      {
        int min_value_below = BestValueR2(row, col, calculated_arr);
        current_value += min_value_below;
      }
      else if (row + 1 > height_ - 1)
      {
        int min_value_below = BestValueR3(row, col, calculated_arr);
        current_value += min_value_below;
      }
      else
      {
        int min_value_below = BestValueR1(row, col, calculated_arr);
        current_value += min_value_below;
      }
      calculated_arr[row][col] = current_value;
    }
  }
  return calculated_arr;
}

int *SeamCarver::FindHorizontalSeam(int **&calculated_arr) const
{ // remember to delete in free stack later after use
  // find starting col
  int *horizontal_arr = new int[width_];
  horizontal_arr[0] = StartingRow(calculated_arr);
  int current_row = StartingRow(calculated_arr);
  // for second to last
  for (int col = 1; col < width_; ++col)
  {
    if (current_row - 1 < 0) [[likely]]
    {
      int next_row = FindNextRow2(current_row, col, calculated_arr);
      current_row = next_row;
    }
    else if (current_row + 1 > height_ - 1)
    {
      int next_row = FindNextRow3(current_row, col, calculated_arr);
      current_row = next_row;
    }
    else
    {
      int next_row = FindNextRow1(current_row, col, calculated_arr);
      current_row = next_row;
    }
    horizontal_arr[col] = current_row;
  }
  return horizontal_arr;
}

// helpers for HorizontalFind
int SeamCarver::StartingRow(int **&calculated_arr) const
{
  int min_pixel_value = calculated_arr[0][0];
  int min_pixel_row = 0;
  for (int row = 0; row < height_; ++row)
  {
    if (calculated_arr[row][0] < min_pixel_value) [[likely]]
    {
      min_pixel_value = calculated_arr[row][0];
      min_pixel_row = row;
    }
  }
  return min_pixel_row;
}

int SeamCarver::FindNextRow1(int current_row, int col,
                             int **&calculated_arr) const
{
  int top = calculated_arr[current_row + 1][col];
  int middle = calculated_arr[current_row][col];
  int bottom = calculated_arr[current_row - 1][col];
  if (top < middle && top <= bottom)
  {
    ++current_row;
  }
  else if (bottom < top && bottom < middle)
  {
    --current_row;
  }
  return current_row;
}

int SeamCarver::FindNextRow2(int current_row, int col,
                             int **&calculated_arr) const
{
  int top = calculated_arr[current_row + 1][col];
  int middle = calculated_arr[current_row][col];
  if (top < middle) [[likely]]
  {
    ++current_row;
  }
  return current_row;
}

int SeamCarver::FindNextRow3(int current_row, int col,
                             int **&calculated_arr) const
{
  int middle = calculated_arr[current_row][col];
  int bottom = calculated_arr[current_row - 1][col];
  if (bottom < middle) [[likely]]
  {
    --current_row;
  }
  return current_row;
}

// helpers for CalculateHorizontalSeam()
int SeamCarver::BestValueR1(int row, int col, int **&calculated_arr) const
{
  int top_value = calculated_arr[row + 1][col + 1];
  int middle_value = calculated_arr[row][col + 1];
  int bottom_value = calculated_arr[row - 1][col + 1];
  if (top_value < middle_value && top_value <= bottom_value) [[likely]]
  {
    return top_value;
  }
  if (middle_value <= top_value && middle_value <= bottom_value) [[likely]]
  {
    return middle_value;
  }
  return bottom_value;
}

int SeamCarver::BestValueR2(int row, int col, int **&calculated_arr) const
{
  int top_value = calculated_arr[row + 1][col + 1];
  int middle_value = calculated_arr[row][col + 1];
  if (middle_value <= top_value) [[likely]]
  {
    return middle_value;
  }
  return top_value;
}

int SeamCarver::BestValueR3(int row, int col, int **&calculated_arr) const
{
  int middle_value = calculated_arr[row][col + 1];
  int bottom_value = calculated_arr[row - 1][col + 1];
  if (middle_value <= bottom_value)
  {
    return middle_value;
  }
  return bottom_value;
}

// helper for GetVerticalSeam : calculate and find
int **SeamCarver::CalculateVerticalSeam() const
{
  // Init a calculated_arr;
  int **calculated_arr = new int *[height_];
  for (int row = 0; row < height_; ++row)
  {
    calculated_arr[row] = new int[width_];
  }
  // fill it; start from the last row (height_ - 1);
  // for the last row;
  for (int col = 0; col < width_; ++col)
  {
    calculated_arr[height_ - 1][col] = image_.GetEnergy(height_ - 1, col);
  }
  // fill all the other rows;
  for (int row = height_ - 2; row != -1; --row)
  {
    for (int col = 0; col < width_; ++col)
    {
      int current_value = image_.GetEnergy(row, col);
      if (col - 1 < 0) [[likely]]
      {
        int min_value_below = BestValue2(row, col, calculated_arr);
        current_value += min_value_below;
      }
      else if (col + 1 > width_ - 1)
      {
        int min_value_below = BestValue3(row, col, calculated_arr);
        current_value += min_value_below;
      }
      else
      {
        int min_value_below = BestValue1(row, col, calculated_arr);
        current_value += min_value_below;
      }
      calculated_arr[row][col] = current_value;
    }
  }
  return calculated_arr;
}

int *SeamCarver::FindVerticalSeam(int **&calculated_arr) const
{ // remember to delete in free stack later after use
  // find starting row
  int *vertical_arr = new int[height_];
  vertical_arr[0] = StartingCol(calculated_arr);
  int current_col = StartingCol(calculated_arr);
  // for second to last
  for (int row = 1; row < height_; ++row)
  {
    if (current_col - 1 < 0) [[likely]]
    {
      int next_col = FindNextCol2(row, current_col, calculated_arr);
      current_col = next_col;
    }
    else if (current_col + 1 > width_ - 1)
    {
      int next_col = FindNextCol3(row, current_col, calculated_arr);
      current_col = next_col;
    }
    else
    {
      int next_col = FindNextCol1(row, current_col, calculated_arr);
      current_col = next_col;
    }
    vertical_arr[row] = current_col;
  }
  return vertical_arr;
}

// helpers for VerticalFind
int SeamCarver::StartingCol(int **&calculated_arr) const
{
  int min_pixel_value = calculated_arr[0][0];
  int min_pixel_col = 0;
  for (int col = 0; col < width_; ++col)
  {
    if (calculated_arr[0][col] < min_pixel_value) [[likely]]
    {
      min_pixel_value = calculated_arr[0][col];
      min_pixel_col = col;
    }
  }
  return min_pixel_col;
}

int SeamCarver::FindNextCol1(int row, int current_col,
                             int **&calculated_arr) const
{
  int left = calculated_arr[row][current_col - 1];
  int middle = calculated_arr[row][current_col];
  int right = calculated_arr[row][current_col + 1];
  if (left < middle && left <= right)
  {
    --current_col;
  }
  else if (right < left && right < middle)
  {
    ++current_col;
  }
  return current_col;
}

int SeamCarver::FindNextCol2(int row, int current_col,
                             int **&calculated_arr) const
{
  int middle = calculated_arr[row][current_col];
  int right = calculated_arr[row][current_col + 1];
  if (right < middle) [[likely]]
  {
    ++current_col;
  }
  return current_col;
}

int SeamCarver::FindNextCol3(int row, int current_col,
                             int **&calculated_arr) const
{
  int left = calculated_arr[row][current_col - 1];
  int middle = calculated_arr[row][current_col];
  if (left < middle)
  {
    --current_col;
  }
  return current_col;
}

// helpers for CalculateVerticalSeam()
int SeamCarver::BestValue1(int row, int col, int **&calculated_arr) const
{
  int left_value = calculated_arr[row + 1][col - 1];
  int middle_value = calculated_arr[row + 1][col];
  int right_value = calculated_arr[row + 1][col + 1];
  if (left_value < middle_value && left_value <= right_value)
  {
    return left_value;
  }
  if (middle_value <= left_value && middle_value <= right_value)
  {
    return middle_value;
  }
  return right_value;
}

int SeamCarver::BestValue2(int row, int col, int **&calculated_arr) const
{
  int middle_value = calculated_arr[row + 1][col];
  int right_value = calculated_arr[row + 1][col + 1];
  if (middle_value <= right_value) [[likely]]
  {
    return middle_value;
  }
  return right_value;
}

int SeamCarver::BestValue3(int row, int col, int **&calculated_arr) const
{
  int left_value = calculated_arr[row + 1][col - 1];
  int middle_value = calculated_arr[row + 1][col];
  if (left_value <= middle_value) [[likely]]
  {
    return left_value;
  }
  return middle_value;
}

// helpers for GetEnergy
int SeamCarver::Square(int value) const { return value * value; }

int SeamCarver::NumDeltaCol(int row, int col) const
{
  int delta_col = 0;
  if (col - 1 < 0)
  {
    int delta_red = Square(image_.GetPixel(row, col + 1).GetRed() -
                           image_.GetPixel(row, width_ - 1).GetRed());
    int delta_green = Square(image_.GetPixel(row, col + 1).GetGreen() -
                             image_.GetPixel(row, width_ - 1).GetGreen());
    int delta_blue = Square(image_.GetPixel(row, col + 1).GetBlue() -
                            image_.GetPixel(row, width_ - 1).GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  else if (col + 1 > width_ - 1)
  {
    int delta_red = Square(image_.GetPixel(row, col - 1).GetRed() -
                           image_.GetPixel(row, 0).GetRed());
    int delta_green = Square(image_.GetPixel(row, col - 1).GetGreen() -
                             image_.GetPixel(row, 0).GetGreen());
    int delta_blue = Square(image_.GetPixel(row, col - 1).GetBlue() -
                            image_.GetPixel(row, 0).GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  else
  {
    int delta_red = Square(image_.GetPixel(row, col + 1).GetRed() -
                           image_.GetPixel(row, col - 1).GetRed());
    int delta_green = Square(image_.GetPixel(row, col + 1).GetGreen() -
                             image_.GetPixel(row, col - 1).GetGreen());
    int delta_blue = Square(image_.GetPixel(row, col + 1).GetBlue() -
                            image_.GetPixel(row, col - 1).GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  return delta_col;
}

int SeamCarver::NumDeltaRow(int row, int col) const
{
  int delta_row = 0;
  if (row - 1 < 0)
  {
    int delta_red = Square(image_.GetPixel(row + 1, col).GetRed() -
                           image_.GetPixel(height_ - 1, col).GetRed());
    int delta_green = Square(image_.GetPixel(row + 1, col).GetGreen() -
                             image_.GetPixel(height_ - 1, col).GetGreen());
    int delta_blue = Square(image_.GetPixel(row + 1, col).GetBlue() -
                            image_.GetPixel(height_ - 1, col).GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  else if (row + 1 > height_ - 1)
  {
    int delta_red = Square(image_.GetPixel(row - 1, col).GetRed() -
                           image_.GetPixel(0, col).GetRed());
    int delta_green = Square(image_.GetPixel(row - 1, col).GetGreen() -
                             image_.GetPixel(0, col).GetGreen());
    int delta_blue = Square(image_.GetPixel(row - 1, col).GetBlue() -
                            image_.GetPixel(0, col).GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  else
  {
    int delta_red = Square(image_.GetPixel(row + 1, col).GetRed() -
                           image_.GetPixel(row - 1, col).GetRed());
    int delta_green = Square(image_.GetPixel(row + 1, col).GetGreen() -
                             image_.GetPixel(row - 1, col).GetGreen());
    int delta_blue = Square(image_.GetPixel(row + 1, col).GetBlue() -
                            image_.GetPixel(row - 1, col).GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  return delta_row;
}

//
void SeamCarver::RemoveHorizontalSeam()
{
  /// init
  Pixel **new_arr = new Pixel *[height_ - 1];
  for (int new_row = 0; new_row < height_ - 1; ++new_row)
  {
    new_arr[new_row] = new Pixel[width_];
  }
  // fill
  int max = 0;
  // delte a row; omit the row we want to delete
  int *arr = image_.GetHorizontalSeam();
  for (int col = 0; col < width_; ++col)
  {
    for (int row = 0; row < height_ - 1; ++row)
    {
      if (row < arr[col])
      {
        new_arr[row][col] = image_.GetPixel(row, col);
      }
      else
      {
        // std::cout << height_ << std::endl;
        new_arr[row][col] = image_.GetPixel(row + 1, col);
      }
      if (std::max(
              std::max(new_arr[row][col].GetRed(), new_arr[row][col].GetBlue()),
              new_arr[row][col].GetGreen()) > max)
      {
        max = std::max(
            std::max(new_arr[row][col].GetRed(), new_arr[row][col].GetBlue()),
            new_arr[row][col].GetGreen());
      }
    }
  }
  int max_color = max;
  image_ = ImagePPM(height_ - 1, width_, max_color, new_arr);
  delete[] arr;
  arr = nullptr;
  height_ = image_.GetHeight();
}

//
void SeamCarver::RemoveVerticalSeam()
{
  /// init
  Pixel **new_arr = new Pixel *[height_];
  for (int new_row = 0; new_row < height_; ++new_row)
  {
    new_arr[new_row] = new Pixel[width_ - 1];
  }
  int max = 0;
  // fill
  int *arr =
      image_.GetVerticalSeam(); // the collection of cols we want to delete
  for (int row = 0; row < height_; ++row)
  {
    for (int col = 0; col < width_ - 1; ++col)
    {
      if (col < arr[row])
      { // adjusted
        new_arr[row][col] = image_.GetPixel(row, col);
      }
      else
      {
        new_arr[row][col] = image_.GetPixel(row, col + 1);
      }
      if (std::max(
              std::max(new_arr[row][col].GetRed(), new_arr[row][col].GetBlue()),
              new_arr[row][col].GetGreen()) > max)
      {
        max = std::max(
            std::max(new_arr[row][col].GetRed(), new_arr[row][col].GetBlue()),
            new_arr[row][col].GetGreen());
      }
    }
  }
  int max_color = max;
  image_ = ImagePPM(height_, width_ - 1, max_color, new_arr);
  delete[] arr;
  arr = nullptr;
  width_ = image_.GetWidth();
}