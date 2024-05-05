#include "image_ppm.hpp"

// make a new construtor, takes pixels
ImagePPM::ImagePPM(int height, int width, int max_color, Pixel **new_arr)
    : height_(height), width_(width), max_color_value_(max_color),
      pixels_(new_arr)
{
}
// implement the rest of ImagePPM's functions here

// given functions below, DO NOT MODIFY

ImagePPM::ImagePPM(const std::string &path)
{
  std::ifstream input_file(path);
  input_file >> *this;
  input_file.close();
}

ImagePPM::ImagePPM(const ImagePPM &source) { *this = source; }

ImagePPM &ImagePPM::operator=(const ImagePPM &source)
{
  if (this == &source) [[likely]]
  {
    return *this;
  }

  Clear();

  height_ = source.height_;
  width_ = source.width_;
  max_color_value_ = source.max_color_value_;

  pixels_ = new Pixel *[height_];
  for (int row = 0; row < height_; row++)
  {
    pixels_[row] = new Pixel[width_];

    for (int col = 0; col < width_; col++)
    {
      pixels_[row][col] = source.pixels_[row][col];
    }
  }

  return *this;
}

ImagePPM::~ImagePPM() { Clear(); }

void ImagePPM::Clear()
{
  for (int i = 0; i < height_; i++)
  {
    delete[] pixels_[i];
  }

  delete[] pixels_;

  height_ = 0;
  width_ = 0;
  pixels_ = nullptr;
}

std::istream &operator>>(std::istream &is, ImagePPM &image)
{
  image.Clear();
  std::string line;
  // ignore magic number line
  getline(is, line);
  // check to see if there's a comment line
  getline(is, line);
  if (line[0] == '#')
  {
    getline(is, line);
  }
  // parse width and height
  int space = line.find_first_of(' ');
  image.width_ = std::stoi(line.substr(0, space));
  image.height_ = std::stoi(line.substr(space + 1));
  // get max color value
  getline(is, line);
  image.max_color_value_ = std::stoi(line);
  // init and fill in Pixels array
  image.pixels_ = new Pixel *[image.height_];
  for (int i = 0; i < image.height_; i++)
  {
    image.pixels_[i] = new Pixel[image.width_];
  }
  for (int row = 0; row < image.height_; row++)
  {
    for (int col = 0; col < image.width_; col++)
    {
      getline(is, line);
      int red = std::stoi(line);
      getline(is, line);
      int green = std::stoi(line);
      getline(is, line);
      int blue = std::stoi(line);

      Pixel p(red, green, blue);
      image.pixels_[row][col] = p;
    }
  }
  return is;
}

std::ostream &operator<<(std::ostream &os, const ImagePPM &image)
{
  os << "P3" << std::endl;
  os << image.width_ << " " << image.height_ << std::endl;
  os << image.max_color_value_ << std::endl;
  for (int row = 0; row < image.height_; ++row)
  {
    for (int col = 0; col < image.width_; ++col)
    {
      os << image.pixels_[row][col].GetRed() << std::endl;
      os << image.pixels_[row][col].GetGreen() << std::endl;
      os << image.pixels_[row][col].GetBlue() << std::endl;
    }
  }
  return os;
}

//
int ImagePPM::GetEnergy(int row, int col) const
{
  int delta_col = NumDeltaCol(row, col);
  int delta_row = NumDeltaRow(row, col);
  int energy = delta_col + delta_row;
  return energy;
}

int *ImagePPM::GetVerticalSeam() const
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

int *ImagePPM::GetHorizontalSeam() const
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
int **ImagePPM::CalculateHorizontalSeam() const
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
    calculated_arr[row][width_ - 1] = GetEnergy(row, width_ - 1);
  }
  // fill all the other rows;
  for (int col = width_ - 2; col != -1; --col)
  {
    for (int row = 0; row < height_; ++row)
    {
      int current_value = GetEnergy(row, col);
      if (row - 1 < 0) [[unlikely]]
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

int *ImagePPM::FindHorizontalSeam(int **&calculated_arr) const
{ // remember to delete in free stack later after use
  // find starting col
  int *horizontal_arr = new int[width_];
  horizontal_arr[0] = StartingRow(calculated_arr);
  int current_row = StartingRow(calculated_arr);
  // for second to last
  for (int col = 1; col < width_; ++col)
  {
    if (current_row - 1 < 0)
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
int ImagePPM::StartingRow(int **&calculated_arr) const
{
  int min_pixel_value = calculated_arr[0][0];
  int min_pixel_row = 0;
  for (int row = 0; row < height_; ++row)
  {
    if (calculated_arr[row][0] < min_pixel_value)
    {
      min_pixel_value = calculated_arr[row][0];
      min_pixel_row = row;
    }
  }
  return min_pixel_row;
}

int ImagePPM::FindNextRow1(int current_row, int col,
                           int **&calculated_arr) const
{
  int top = calculated_arr[current_row + 1][col];
  int middle = calculated_arr[current_row][col];
  int bottom = calculated_arr[current_row - 1][col];
  if (top < middle && top <= bottom) [[likely]]
  {
    ++current_row;
  }
  else if (bottom < top && bottom < middle)
  {
    --current_row;
  }
  return current_row;
}

int ImagePPM::FindNextRow2(int current_row, int col,
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

int ImagePPM::FindNextRow3(int current_row, int col,
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
int ImagePPM::BestValueR1(int row, int col, int **&calculated_arr) const
{
  int top_value = calculated_arr[row + 1][col + 1];
  int middle_value = calculated_arr[row][col + 1];
  int bottom_value = calculated_arr[row - 1][col + 1];
  if (top_value < middle_value && top_value <= bottom_value)
  {
    return top_value;
  }
  if (middle_value <= top_value && middle_value <= bottom_value)
  {
    return middle_value;
  }
  return bottom_value;
}

int ImagePPM::BestValueR2(int row, int col, int **&calculated_arr) const
{
  int top_value = calculated_arr[row + 1][col + 1];
  int middle_value = calculated_arr[row][col + 1];
  if (middle_value <= top_value) [[likely]]
  {
    return middle_value;
  }
  return top_value;
}

int ImagePPM::BestValueR3(int row, int col, int **&calculated_arr) const
{
  int middle_value = calculated_arr[row][col + 1];
  int bottom_value = calculated_arr[row - 1][col + 1];
  if (middle_value <= bottom_value) [[likely]]
  {
    return middle_value;
  }
  return bottom_value;
}

// helper for GetVerticalSeam : calculate and find
int **ImagePPM::CalculateVerticalSeam() const
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
    calculated_arr[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }
  // fill all the other rows;
  for (int row = height_ - 2; row != -1; --row)
  {
    for (int col = 0; col < width_; ++col)
    {
      int current_value = GetEnergy(row, col);
      if (col - 1 < 0)
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

int *ImagePPM::FindVerticalSeam(int **&calculated_arr) const
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
int ImagePPM::StartingCol(int **&calculated_arr) const
{
  int min_pixel_value = calculated_arr[0][0];
  int min_pixel_col = 0;
  for (int col = 0; col < width_; ++col)
  {
    if (calculated_arr[0][col] < min_pixel_value)
    {
      min_pixel_value = calculated_arr[0][col];
      min_pixel_col = col;
    }
  }
  return min_pixel_col;
}

int ImagePPM::FindNextCol1(int row, int current_col,
                           int **&calculated_arr) const
{
  int left = calculated_arr[row][current_col - 1];
  int middle = calculated_arr[row][current_col];
  int right = calculated_arr[row][current_col + 1];
  if (left < middle && left <= right) [[likely]]
  {
    --current_col;
  }
  else if (right < left && right < middle)
  {
    ++current_col;
  }
  return current_col;
}

int ImagePPM::FindNextCol2(int row, int current_col,
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

int ImagePPM::FindNextCol3(int row, int current_col,
                           int **&calculated_arr) const
{
  int left = calculated_arr[row][current_col - 1];
  int middle = calculated_arr[row][current_col];
  if (left < middle) [[likely]]
  {
    --current_col;
  }
  return current_col;
}

// helpers for CalculateVerticalSeam()
int ImagePPM::BestValue1(int row, int col, int **&calculated_arr) const
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

int ImagePPM::BestValue2(int row, int col, int **&calculated_arr) const
{
  int middle_value = calculated_arr[row + 1][col];
  int right_value = calculated_arr[row + 1][col + 1];
  if (middle_value <= right_value) [[likely]]
  {
    return middle_value;
  }
  return right_value;
}

int ImagePPM::BestValue3(int row, int col, int **&calculated_arr) const
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
int ImagePPM::Square(int value) const { return value * value; }

int ImagePPM::NumDeltaCol(int row, int col) const
{
  int delta_col = 0;
  if (col - 1 < 0) [[unlikely]]
  {
    int delta_red = Square(pixels_[row][col + 1].GetRed() -
                           pixels_[row][width_ - 1].GetRed());
    int delta_green = Square(pixels_[row][col + 1].GetGreen() -
                             pixels_[row][width_ - 1].GetGreen());
    int delta_blue = Square(pixels_[row][col + 1].GetBlue() -
                            pixels_[row][width_ - 1].GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  else if (col + 1 > width_ - 1)
  {
    int delta_red =
        Square(pixels_[row][col - 1].GetRed() - pixels_[row][0].GetRed());
    int delta_green =
        Square(pixels_[row][col - 1].GetGreen() - pixels_[row][0].GetGreen());
    int delta_blue =
        Square(pixels_[row][col - 1].GetBlue() - pixels_[row][0].GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  else
  {
    int delta_red =
        Square(pixels_[row][col + 1].GetRed() - pixels_[row][col - 1].GetRed());
    int delta_green = Square(pixels_[row][col + 1].GetGreen() -
                             pixels_[row][col - 1].GetGreen());
    int delta_blue = Square(pixels_[row][col + 1].GetBlue() -
                            pixels_[row][col - 1].GetBlue());
    delta_col = delta_red + delta_green + delta_blue;
  }
  return delta_col;
}

int ImagePPM::NumDeltaRow(int row, int col) const
{
  int delta_row = 0;
  if (row - 1 < 0) [[unlikely]]
  {
    int delta_red = Square(pixels_[row + 1][col].GetRed() -
                           pixels_[height_ - 1][col].GetRed());
    int delta_green = Square(pixels_[row + 1][col].GetGreen() -
                             pixels_[height_ - 1][col].GetGreen());
    int delta_blue = Square(pixels_[row + 1][col].GetBlue() -
                            pixels_[height_ - 1][col].GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  else if (row + 1 > height_ - 1)
  {
    int delta_red =
        Square(pixels_[row - 1][col].GetRed() - pixels_[0][col].GetRed());
    int delta_green =
        Square(pixels_[row - 1][col].GetGreen() - pixels_[0][col].GetGreen());
    int delta_blue =
        Square(pixels_[row - 1][col].GetBlue() - pixels_[0][col].GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  else
  {
    int delta_red =
        Square(pixels_[row + 1][col].GetRed() - pixels_[row - 1][col].GetRed());
    int delta_green = Square(pixels_[row + 1][col].GetGreen() -
                             pixels_[row - 1][col].GetGreen());
    int delta_blue = Square(pixels_[row + 1][col].GetBlue() -
                            pixels_[row - 1][col].GetBlue());
    delta_row = delta_red + delta_green + delta_blue;
  }
  return delta_row;
}