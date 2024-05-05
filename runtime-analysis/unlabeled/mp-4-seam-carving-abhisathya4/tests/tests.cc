//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  MP : Seam Carver  : Student Test Cases                  //
//                                                                          //
// Written By :                         Environment :                       //
// Date ......:                         Compiler ...:                       //
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//                             Framework Set-up                             //
//////////////////////////////////////////////////////////////////////////////
#include <catch/catch.hpp>

//////////////////////////////////////////////////////////////////////////////
//                                 Includes                                 //
//////////////////////////////////////////////////////////////////////////////
#include "image_ppm.hpp"
#include "pixel.hpp"
#include "seam_carver.hpp"

//////////////////////////////////////////////////////////////////////////////
//                                Test Cases                                //
//////////////////////////////////////////////////////////////////////////////
// test GetEnergy
TEST_CASE("GetEnergy", "SeamCarver") {
  ImagePPM test_image = ImagePPM(
      "/home/vagrant/src/MPs/mp-seam-carver-Lumi-works/images/image1.ppm");
  SeamCarver test_seam = SeamCarver(test_image);
  int test_energy = test_seam.GetEnergy(0, 0);
  REQUIRE(test_energy == 75);
  int test_energy2 = test_seam.GetEnergy(1, 0);
  REQUIRE(test_energy2 == 0);
}

// test get horizontal_arr
TEST_CASE("GetVerticalArr", "SeamCarver") {
  ImagePPM test_image = ImagePPM(
      "/home/vagrant/src/MPs/mp-seam-carver-Lumi-works/images/image1.ppm");
  SeamCarver test_seam = SeamCarver(test_image);
  int* test_arr = test_seam.GetVerticalSeam();
  REQUIRE(test_arr[0] == 1);
}

// test get vertical_arr
/*
TEST_CASE("GetVerticalArr", "SeamCarver") {
  ImagePPM test_image = ImagePPM(
      "/home/vagrant/src/MPs/mp-seam-carver-Lumi-works/images/image1.ppm");
  SeamCarver test_seam = SeamCarver(test_image);
  int* test_arr = test_seam.GetVerticalSeam();
  int test_starting_col = test_arr[0];
  REQUIRE(test_starting_col == 0);
}
*/

//////////////////////////////////////////////////////////////////////////////

TEST_CASE("True == True", "") { REQUIRE(true == true); }