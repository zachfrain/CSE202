#include "image.h"
#include <time.h>
using ImagePtr = std::shared_ptr<Image>;

ImagePtr applyGamma(ImagePtr image_ptr, double gamma);
ImagePtr applyGammaOpt1(ImagePtr image_ptr, double gamma);
ImagePtr applyGammaOpt2(ImagePtr image_ptr, double gamma);
ImagePtr applyGammaOpt3(ImagePtr image_ptr, double gamma);
ImagePtr applyTint(ImagePtr image_ptr, const double *tints);
ImagePtr applyBlur(ImagePtr imag_ptr);
ImagePtr applyBlurOpt1(ImagePtr imag_ptr);
ImagePtr applyBlurOpt2(ImagePtr imag_ptr);
ImagePtr applyBlurOpt3(ImagePtr imag_ptr);
void writeImage(ImagePtr image_ptr);

void process_images(const std::vector<ImagePtr>& image_vector) {
  clock_t total = clock();
  const double tint_array[] = {0.75, 0, 0};
  clock_t gammatime=0; clock_t blurtime=0; clock_t time;
  for (ImagePtr img : image_vector) {
    writeImage(img);
    time = clock();
    img = applyGammaOpt3(img, 1.4);
    time = clock() - time;
    gammatime = time + gammatime;
    img = applyTint(img, tint_array);
    time = clock();
    img = applyBlurOpt3(img);
    time = clock() - time;
    blurtime = time + blurtime;
    writeImage(img);
  }
  printf("applyGamma Time\t\t%ld\n",gammatime);
  printf("applyBlur Time\t\t%ld\n",blurtime);
}
// Apply a Gamma scale to all the pixels of the input image
  
  ImagePtr applyGamma(ImagePtr image_ptr, double gamma) {
  auto output_image_ptr = std::make_shared<Image>(image_ptr->name() + "_gamma", 
                                                  IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  //const int height = in_rows.size();
  //const int width = in_rows[1] - in_rows[0];
  for (unsigned long i = 0; i < in_rows.size(); ++i ) {
    for (int j = 0; j < in_rows[1] - in_rows[0]; ++j ) {
      //const Pixel& p = in_rows[i][j]; 
      double v = 0.3*in_rows[i][j].bgra[2] + 0.59*in_rows[i][j].bgra[1] + 0.11*in_rows[i][j].bgra[0];
      double res = pow(v, gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res, res, res);
    }
  }
  return output_image_ptr;
}
/* applyGamma Optimization 1 - Reduce Calls to memory */
ImagePtr applyGammaOpt1(ImagePtr image_ptr, double gamma) {
  auto output_image_ptr = std::make_shared<Image>(image_ptr->name() + "_gamma",
                                                  IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  const int height = in_rows.size();
  const int width = in_rows[1] - in_rows[0];
  for (unsigned long i = 0; i < height; ++i ) {
    for (int j = 0; j < width; ++j ) {
      const Pixel& p = in_rows[i][j];
      double v = 0.3*p.bgra[2] + 0.59*p.bgra[1] + 0.11*p.bgra[0];
      double res = pow(v, gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res, res, res);
    }
  }
  return output_image_ptr;
}
/* applyGamma Optimization 2 - Loop Unrolling */
ImagePtr applyGammaOpt2(ImagePtr image_ptr, double gamma) {
  auto output_image_ptr = std::make_shared<Image>(image_ptr->name() + "_gamma",
                                                  IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  const int height = in_rows.size();
  const int width = in_rows[1] - in_rows[0];
  const int limit = width - 1;
  long i; int j;
  for (i = 0; i < height; ++i ) {
    for (j = 0; j < limit; j=j+2 ) {
      const Pixel& p = in_rows[i][j];
      const Pixel& pp = in_rows[i][j+1];
      double v = 0.3*p.bgra[2] + 0.59*p.bgra[1] + 0.11*p.bgra[0];
      double vv = 0.3*pp.bgra[2] + 0.59*pp.bgra[1] + 0.11*pp.bgra[0];
      double res = pow(v, gamma);
      double ress = pow(vv, gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      if(ress > MAX_BGR_VALUE) ress = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res, res, res);
      out_rows[i][j+1] = Pixel(ress, ress, ress);
    }
    for(; j < width; ++j){
      const Pixel& p = in_rows[i][j];
      double v = 0.3*p.bgra[2] + 0.59*p.bgra[1] + 0.11*p.bgra[0];
      double res = pow(v, gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res, res, res);
   }
  }
  return output_image_ptr;
}

/* applyGamma Optimization 3 - 2x1 loop unrolling of 5x1 lomagePtr applyGammaOpt3(ImagePtr image_ptr, double gamma) {
  auto out_rows = output_image_ptr->rows();
  const int height = in_rows.size();
  const int limit1 = height-1;
  const int width = in_rows[1] - in_rows[0];
  const int limit2 = width - 4;
  long i; int j; int k;
  for (i = 0; i < limit1; i=i+2 ) {
    for (j = 0; j < limit2; j = j+5 ) {
      const Pixel& p1 = in_rows[i][j];
      const Pixel &p2 = in_rows[i][j+1];
      const Pixel &p3 = in_rows[i][j+2];
      const Pixel &p4 = in_rows[i][j+3];
      const Pixel &p5 = in_rows[i][j+4];
      double v1 = 0.3 * p1.bgra[2] + 0.59*p1.bgra[1] + 0.11*p1.bgra[0]      double res1 = pow(v1, gamma);
      double v2 = 0.3 * p2.bgra[2] + 0.59 * p2.bgra[1] + 0.11 * p2.bgra[0];
      double res2 = pow(v2, gamma);
      double v3 = 0.3 * p3.bgra[2] + 0.59 * p3.bgra[1] + 0.11 * p3.bgra[0];
      double res3 = pow(v3, gamma);
      double v4 = 0.3 * p4.bgra[2] + 0.59 * p4.bgra[1] + 0.11 * p4.bgra[0];
 6    double res4 = pow(v4, gamma);
      double v5 = 0.3 * p5.bgra[2] + 0.59 * p5.bgra[1] + 0.11 * p5.bgra[0];
      double res5 = pow(v5, gamma);
      if(res1 > MAX_BGR_VALUE) 
        res1 = MAX_BGR_VALUE;
      if (res2 > MAX_BGR_VALUE)
        res2 = MAX_BGR_VALUE;
      if (res3 > MAX_BGR_VALUE)
        res3 = MAX_BGR_VALUE;
      if (res4 > MAX_BGR_VALUE)
        res4 = MAX_BGR_VALUE;
      if (res5 > MAX_BGR_VALUE)
        res5 = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res1, res1, res1);
      out_rows[i][j+1] = Pixel(res2, res2, res2);
      out_rows[i][j+2] = Pixel(res3, res3, res3);
      out_rows[i][j+3] = Pixel(res4, res4, res4);
      out_rows[i][j+4] = Pixel(res4, res4, res4);
    }
    for (k = 0; k < limit2; k = k + 5)
    {
      const Pixel &p1 = in_rows[i+1][k];
      const Pixel &p2 = in_rows[i+1][k + 1];
      const Pixel &p3 = in_rows[i+1][k + 2];
      const Pixel &p4 = in_rows[i+1][k + 3];
      const Pixel &p5 = in_rows[i+1][k + 4];
      double v1 = 0.3 * p1.bgra[2] + 0.59 * p1.bgra[1] + 0.11 * p1.bgra[0];
      double res1 = pow(v1, gamma);
      double v2 = 0.3 * p2.bgra[2] + 0.59 * p2.bgra[1] + 0.11 * p2.bgra[0];
      double res2 = pow(v2, gamma);
      double v3 = 0.3 * p3.bgra[2] + 0.59 * p3.bgra[1] + 0.11 * p3.bgra[0];
      double res3 = pow(v3, gamma);
      double v4 = 0.3 * p4.bgra[2] + 0.59 * p4.bgra[1] + 0.11 * p4.bgra[0];
      double res4 = pow(v4, gamma);
      double v5 = 0.3 * p5.bgra[2] + 0.59 * p5.bgra[1] + 0.11 * p5.bgra[0];
      double res5 = pow(v5, gamma);
      if (res1 > MAX_BGR_VALUE)
        res1 = MAX_BGR_VALUE;
      if (res2 > MAX_BGR_VALUE)
        res2 = MAX_BGR_VALUE;
      if (res3 > MAX_BGR_VALUE)
        res3 = MAX_BGR_VALUE;
      if (res4 > MAX_BGR_VALUE)
        res4 = MAX_BGR_VALUE;
      if (res5 > MAX_BGR_VALUE)
        res5 = MAX_BGR_VALUE;
      out_rows[i+1][k] = Pixel(res1, res1, res1);
      out_rows[i+1][k + 1] = Pixel(res2, res2, res2);
      out_rows[i+1][k + 2] = Pixel(res3, res3, res3);
      out_rows[i+1][k + 3] = Pixel(res4, res4, res4);
      out_rows[i+1][k + 4] = Pixel(res4, res4, res4);
    }
    for(; j < width; ++j){
      const Pixel &p = in_rows[i][j];
      double v = 0.3*p.bgra[2] + 0.59*p.bgra[1] + 0.11*p.bgra[0];
      double res = pow(v,gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res,res,res);
    }
    for(; k < width; ++k){
      const Pixel &p = in_rows[i+1][k];
      double v = 0.3 * p.bgra[2] + 0.59 * p.bgra[1] + 0.11 * p.bgra[0];
      double res = pow(v, gamma);
      if (res > MAX_BGR_VALUE)
        res = MAX_BGR_VALUE;
      out_rows[i+1][k] = Pixel(res, res, res);
    }
  }
  for(; i < height; ++i){
 */


ImagePtr applyGammaOpt3(ImagePtr image_ptr, double gamma) {
  auto output_image_ptr = std::make_shared<Image>(image_ptr->name() + "_gamma", 
                                                  IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  const int height = in_rows.size();
  const int limit1 = height-1;
  const int width = in_rows[1] - in_rows[0];
  const int limit2 = width - 4;
  long i; int j; int k;
  for (i = 0; i < limit1; i=i+2 ) {
    for (j = 0; j < limit2; j = j+5 ) {
      k = j;
      const Pixel &p1 = in_rows[i][j];
      const Pixel &p2 = in_rows[i][j+1];
      const Pixel &p3 = in_rows[i][j+2];
      const Pixel &p4 = in_rows[i][j+3];
      const Pixel &p5 = in_rows[i][j+4];

      const Pixel &pp1 = in_rows[i + 1][k];
      const Pixel &pp2 = in_rows[i + 1][k + 1];
      const Pixel &pp3 = in_rows[i + 1][k + 2];
      const Pixel &pp4 = in_rows[i + 1][k + 3];
      const Pixel &pp5 = in_rows[i + 1][k + 4];

      double v1 = 0.3 * p1.bgra[2] + 0.59*p1.bgra[1] + 0.11*p1.bgra[0];
      double res1 = pow(v1, gamma);
      double v2 = 0.3 * p2.bgra[2] + 0.59 * p2.bgra[1] + 0.11 * p2.bgra[0];
      double res2 = pow(v2, gamma);
      double v3 = 0.3 * p3.bgra[2] + 0.59 * p3.bgra[1] + 0.11 * p3.bgra[0];
      double res3 = pow(v3, gamma);
      double v4 = 0.3 * p4.bgra[2] + 0.59 * p4.bgra[1] + 0.11 * p4.bgra[0];
      double res4 = pow(v4, gamma);
      double v5 = 0.3 * p5.bgra[2] + 0.59 * p5.bgra[1] + 0.11 * p5.bgra[0];
      double res5 = pow(v5, gamma);

      double vv1 = 0.3 * pp1.bgra[2] + 0.59 * pp1.bgra[1] + 0.11 * pp1.bgra[0];
      double res11 = pow(vv1, gamma);
      double vv2 = 0.3 * pp2.bgra[2] + 0.59 * pp2.bgra[1] + 0.11 * pp2.bgra[0];
      double res22 = pow(vv2, gamma);
      double vv3 = 0.3 * pp3.bgra[2] + 0.59 * pp3.bgra[1] + 0.11 * pp3.bgra[0];
      double res33 = pow(vv3, gamma);
      double vv4 = 0.3 * pp4.bgra[2] + 0.59 * pp4.bgra[1] + 0.11 * pp4.bgra[0];
      double res44 = pow(vv4, gamma);
      double vv5 = 0.3 * pp5.bgra[2] + 0.59 * pp5.bgra[1] + 0.11 * pp5.bgra[0];
      double res55 = pow(vv5, gamma);

      if(res1 > MAX_BGR_VALUE) 
        res1 = MAX_BGR_VALUE;
      if (res2 > MAX_BGR_VALUE)
        res2 = MAX_BGR_VALUE;
      if (res3 > MAX_BGR_VALUE)
        res3 = MAX_BGR_VALUE;
      if (res4 > MAX_BGR_VALUE)
        res4 = MAX_BGR_VALUE;
      if (res5 > MAX_BGR_VALUE)
        res5 = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res1, res1, res1);
      out_rows[i][j+1] = Pixel(res2, res2, res2);
      out_rows[i][j+2] = Pixel(res3, res3, res3);
      out_rows[i][j+3] = Pixel(res4, res4, res4);
      out_rows[i][j+4] = Pixel(res4, res4, res4);

      if (res11 > MAX_BGR_VALUE)
        res11 = MAX_BGR_VALUE;
      if (res22 > MAX_BGR_VALUE)
        res22 = MAX_BGR_VALUE;
      if (res33 > MAX_BGR_VALUE)
        res33 = MAX_BGR_VALUE;
      if (res44 > MAX_BGR_VALUE)
        res44 = MAX_BGR_VALUE;
      if (res55 > MAX_BGR_VALUE)
        res55 = MAX_BGR_VALUE;
      out_rows[i + 1][k] = Pixel(res11, res11, res11);
      out_rows[i + 1][k + 1] = Pixel(res22, res22, res22);
      out_rows[i + 1][k + 2] = Pixel(res33, res33, res33);
      out_rows[i + 1][k + 3] = Pixel(res44, res44, res44);
      out_rows[i + 1][k + 4] = Pixel(res55, res55, res55);
    }
    for(; j < width; ++j){
      k = j;
      const Pixel &p = in_rows[i][j];
      const Pixel &pp = in_rows[i+1][k];
      double v = 0.3*p.bgra[2] + 0.59*p.bgra[1] + 0.11*p.bgra[0];
      double vv = 0.3 * pp.bgra[2] + 0.59 * pp.bgra[1] + 0.11 * pp.bgra[0];
      double res = pow(v,gamma);
      double res1 = pow(vv, gamma);
      if(res > MAX_BGR_VALUE) res = MAX_BGR_VALUE;
      if(res1 > MAX_BGR_VALUE) res1 = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res,res,res);
      out_rows[i+1][k] = Pixel(res1, res1, res1);
    }
  }
  for(; i < height; ++i){
    for (j = 0; j < limit2; j = j + 5)
    {
      const Pixel &p1 = in_rows[i][j];
      const Pixel &p2 = in_rows[i][j + 1];
      const Pixel &p3 = in_rows[i][j + 2];
      const Pixel &p4 = in_rows[i][j + 3];
      const Pixel &p5 = in_rows[i][j + 4];
      double v1 = 0.3 * p1.bgra[2] + 0.59 * p1.bgra[1] + 0.11 * p1.bgra[0];
      double res1 = pow(v1, gamma);
      double v2 = 0.3 * p2.bgra[2] + 0.59 * p2.bgra[1] + 0.11 * p2.bgra[0];
      double res2 = pow(v2, gamma);
      double v3 = 0.3 * p3.bgra[2] + 0.59 * p3.bgra[1] + 0.11 * p3.bgra[0];
      double res3 = pow(v3, gamma);
      double v4 = 0.3 * p4.bgra[2] + 0.59 * p4.bgra[1] + 0.11 * p4.bgra[0];
      double res4 = pow(v4, gamma);
      double v5 = 0.3 * p5.bgra[2] + 0.59 * p5.bgra[1] + 0.11 * p5.bgra[0];
      double res5 = pow(v5, gamma);
      if (res1 > MAX_BGR_VALUE)
        res1 = MAX_BGR_VALUE;
      if (res2 > MAX_BGR_VALUE)
        res2 = MAX_BGR_VALUE;
      if (res3 > MAX_BGR_VALUE)
        res3 = MAX_BGR_VALUE;
      if (res4 > MAX_BGR_VALUE)
        res4 = MAX_BGR_VALUE;
      if (res5 > MAX_BGR_VALUE)
        res5 = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res1, res1, res1);
      out_rows[i][j + 1] = Pixel(res2, res2, res2);
      out_rows[i][j + 2] = Pixel(res3, res3, res3);
      out_rows[i][j + 3] = Pixel(res4, res4, res4);
      out_rows[i][j + 4] = Pixel(res4, res4, res4);
    }
    for (; j < width; ++j)
    {
      const Pixel &p = in_rows[i][j];
      double v = 0.3 * p.bgra[2] + 0.59 * p.bgra[1] + 0.11 * p.bgra[0];
      double res = pow(v, gamma);
      if (res > MAX_BGR_VALUE)
        res = MAX_BGR_VALUE;
      out_rows[i][j] = Pixel(res, res, res);
    }
  }
  return output_image_ptr;
}


// Apply the tint in the array tints to all the pixels of the input image
ImagePtr applyTint(ImagePtr image_ptr, const double *tints) {
  auto output_image_ptr = 
    std::make_shared<Image>(image_ptr->name() + "_tinted", 
      IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();

  for (unsigned long i = 0; i < image_ptr->rows().size(); ++i ) {
    for (int j = 0; j < image_ptr->rows()[1] - image_ptr->rows()[0]; ++j ) {
      double b = (double)in_rows[i][j].bgra[0] + (MAX_BGR_VALUE-in_rows[i][j].bgra[0])*tints[0];
      double g = (double)in_rows[i][j].bgra[1] + (MAX_BGR_VALUE-in_rows[i][j].bgra[1])*tints[1];
      double r = (double)in_rows[i][j].bgra[2] + (MAX_BGR_VALUE-in_rows[i][j].bgra[0])*tints[2];
      out_rows[i][j].bgra[0] = b > MAX_BGR_VALUE ? MAX_BGR_VALUE:b;
      out_rows[i][j].bgra[1] = g > MAX_BGR_VALUE ? MAX_BGR_VALUE:g;
      out_rows[i][j].bgra[2] = r > MAX_BGR_VALUE ? MAX_BGR_VALUE:r;
    }
  }
  return output_image_ptr;
}

ImagePtr applyBlur(ImagePtr image_ptr) {
  auto output_image_ptr = 
    std::make_shared<Image>(image_ptr->name() + "_blurred", 
      IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  double b, g, r;
  //int height = in_rows.size();
  //const int width = in_rows[1] - in_rows[0];
  for (unsigned long i = 0; i < in_rows.size(); ++i ) {
    for (int j = 0; j < in_rows[1] - in_rows[0]; ++j ) {
      // Average = ([i-1][j-1] + [i-1][j] + [i-1][j+1] + [i][j-1] + [i][j] + [i][j+1] + [i+1][j-1] + [i+1][j] + [i+1][j+1])/ 9
      if (i == 0) {                        /* first row */
        if (j == 0) {                     /* first row, first column */
          b = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[0] + in_rows[i+1][j].bgra[0] + 0 + in_rows[i][j+1].bgra[0] + in_rows[i+1][j+1].bgra[0]) / 9;
          g = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[1] + in_rows[i+1][j].bgra[1] + 0 + in_rows[i][j+1].bgra[1] + in_rows[i+1][j+1].bgra[1]) / 9;
          r = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[2] + in_rows[i+1][j].bgra[2] + 0 + in_rows[i][j+1].bgra[2] + in_rows[i+1][j+1].bgra[2]) / 9;
        } 
        else if (j == in_rows[1] - in_rows[0] - 1) {          /* first row, last column */
          b = (0 + 0 + 0 + in_rows[i][j-1].bgra[0] + in_rows[i][j].bgra[0] + 0 + in_rows[i+1][j-1].bgra[0] + in_rows[i+1][j].bgra[0] + 0) / 9;
          g = (0 + 0 + 0 + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + 0 + in_rows[i+1][j-1].bgra[1] + in_rows[i+1][j].bgra[1] + 0) / 9;
          r = (0 + 0 + 0 + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + 0 + in_rows[i+1][j-1].bgra[2] + in_rows[i+1][j].bgra[2] + 0) / 9;
        } 
        else {                          /* first row, middle columns */
          b = (0 + 0 + 0 + in_rows[i][j-1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j+1].bgra[0] + in_rows[i+1][j-1].bgra[0] + in_rows[i+1][j].bgra[0] + in_rows[i+1][j+1].bgra[0]) / 9;
          g = (0 + 0 + 0 + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j+1].bgra[1] + in_rows[i+1][j-1].bgra[1] + in_rows[i+1][j].bgra[1] + in_rows[i+1][j+1].bgra[1]) / 9;
          r = (0 + 0 + 0 + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j+1].bgra[2] + in_rows[i+1][j-1].bgra[2] + in_rows[i+1][j].bgra[2] + in_rows[i+1][j+1].bgra[2]) / 9;
        }
      } 
      else if (i == in_rows.size() - 1) {        /* last row */
        if (j == 0) {             /* last row, first column */
          b = (0 + in_rows[i-1][j].bgra[0] + in_rows[i-1][j+1].bgra[0] + 0 + in_rows[i][j].bgra[0] + in_rows[i][j+1].bgra[0] + 0 + 0 + 0) / 9;
          g = (0 + in_rows[i-1][j].bgra[1] + in_rows[i-1][j+1].bgra[1] + 0 + in_rows[i][j].bgra[1] + in_rows[i][j+1].bgra[1] + 0 + 0 + 0) / 9;
          r = (0 + in_rows[i-1][j].bgra[2] + in_rows[i-1][j+1].bgra[2] + 0 + in_rows[i][j].bgra[2] + in_rows[i][j+1].bgra[2] + 0 + 0 + 0) / 9;
        } 
        else if (j == in_rows[1] - in_rows[0] - 1) {      /* last row, last column */
          b = (in_rows[i-1][j-1].bgra[0] + in_rows[i-1][j+1].bgra[0] + 0 + in_rows[i][i-1].bgra[0] + in_rows[i][j].bgra[0] + 0 + 0 + 0 + 0) / 9;
          g = (in_rows[i-1][j-1].bgra[1] + in_rows[i-1][j+1].bgra[1] + 0 + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + 0 + 0 + 0 + 0) / 9;
          r = (in_rows[i-1][j-1].bgra[2] + in_rows[i-1][j+1].bgra[2] + 0 + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + 0 + 0 + 0 + 0) / 9;
        } 
        else {                          /* last row, middle columns */
          b = (in_rows[i-1][j-1].bgra[0] + in_rows[i-1][j].bgra[0] + in_rows[i-1][j+1].bgra[0] + in_rows[i][j-1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j+1].bgra[0] + 0 + 0 + 0) / 9;
          g = (in_rows[i-1][j-1].bgra[1] + in_rows[i-1][j].bgra[1] + in_rows[i-1][j+1].bgra[1] + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j+1].bgra[1] + 0 + 0 + 0) / 9;
          r = (in_rows[i-1][j-1].bgra[2] + in_rows[i-1][j].bgra[2] + in_rows[i-1][j+1].bgra[2] + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j+1].bgra[2] + 0 + 0 + 0) / 9;
        }
      } 
      else {                            /* middle rows */
        if (j == 0) {                 /* middle row, first column */
          b = ( 0 + in_rows[i-1][j].bgra[0] + in_rows[i-1][j+1].bgra[0] + 0 + in_rows[i][j].bgra[0] + in_rows[i][j+1].bgra[0] + 0 + in_rows[i+1][j].bgra[0] + in_rows[i+1][j+1].bgra[0]) / 9;
          g = ( 0 + in_rows[i-1][j].bgra[1] + in_rows[i-1][j+1].bgra[1] + 0 + in_rows[i][j].bgra[1] + in_rows[i][j+1].bgra[1] + 0 + in_rows[i+1][j].bgra[1] + in_rows[i+1][j+1].bgra[1]) / 9;
          r = ( 0 + in_rows[i-1][j].bgra[2] + in_rows[i-1][j+1].bgra[2] + 0 + in_rows[i][j].bgra[2] + in_rows[i][j+1].bgra[2] + 0 + in_rows[i+1][j].bgra[2] + in_rows[i+1][j+1].bgra[2]) / 9;
        } 
        else if (j == in_rows[1] - in_rows[0] - 1) {      /* middle row, last column */
          b = ( in_rows[i-1][j-1].bgra[0] + in_rows[i-1][j].bgra[0] + 0 + in_rows[i][j-1].bgra[0] + in_rows[i][j].bgra[0] + 0 + in_rows[i+1][j-1].bgra[0]+ in_rows[i+1][j].bgra[0] + 0) / 9;
          g = ( in_rows[i-1][j-1].bgra[1] + in_rows[i-1][j].bgra[1] + 0 + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + 0 + in_rows[i+1][j-1].bgra[1] + in_rows[i+1][j].bgra[1] + 0) / 9;
          r = ( in_rows[i-1][j-1].bgra[2] + in_rows[i-1][j].bgra[2] + 0 + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + 0 + in_rows[i+1][j-1].bgra[2] + in_rows[i+1][j].bgra[2] + 0) / 9;
        } 
        else {                          /* middle row, middle columns */
          b = ( in_rows[i-1][j-1].bgra[0] + in_rows[i-1][j].bgra[0] + in_rows[i-1][j+1].bgra[0] + in_rows[i][j-1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j+1].bgra[0] + in_rows[i+1][j-1].bgra[0] + in_rows[i+1][j].bgra[0] + in_rows[i+1][j+1].bgra[0]) / 9;
          g = ( in_rows[i-1][j-1].bgra[1] + in_rows[i-1][j].bgra[1] + in_rows[i-1][j+1].bgra[1] + in_rows[i][j-1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j+1].bgra[1] + in_rows[i+1][j-1].bgra[1] + in_rows[i+1][j].bgra[1] + in_rows[i+1][j+1].bgra[1]) / 9;
          r = ( in_rows[i-1][j-1].bgra[2] + in_rows[i-1][j].bgra[2] + in_rows[i-1][j+1].bgra[2] + in_rows[i][j-1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j+1].bgra[2] + in_rows[i+1][j-1].bgra[2] + in_rows[i+1][j].bgra[2] + in_rows[i+1][j+1].bgra[2]) / 9;
        }
      }
      out_rows[i][j].bgra[0] = (b > MAX_BGR_VALUE)? MAX_BGR_VALUE : b;
      out_rows[i][j].bgra[1] = (g > MAX_BGR_VALUE)? MAX_BGR_VALUE : g;
      out_rows[i][j].bgra[2] = (r > MAX_BGR_VALUE)? MAX_BGR_VALUE : r;
    }
  }
  return output_image_ptr;
}

/* applyBlur Optimization 1 - Reduce Calls to Memory */
ImagePtr applyBlurOpt1(ImagePtr image_ptr)
{
  auto output_image_ptr =
      std::make_shared<Image>(image_ptr->name() + "_blurred",
                              IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  double b, g, r;
  int height = in_rows.size();
  const int width = in_rows[1] - in_rows[0];
  for (unsigned long i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      // Average = ([i-1][j-1] + [i-1][j] + [i-1][j+1] + [i][j-1] + [i][j] + [i][j+1] + [i+1][j-1] + [i+1][j] + [i+1][j+1])/ 9
      if (i == 0)
      { // first row
        if (j == 0)
        { // first row, first column
          b = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[0] + in_rows[i + 1][j].bgra[0] + 0 + in_rows[i][j + 1].bgra[0] + in_rows[i + 1][j + 1].bgra[0]) / 9;
          g = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[1] + in_rows[i + 1][j].bgra[1] + 0 + in_rows[i][j + 1].bgra[1] + in_rows[i + 1][j + 1].bgra[1]) / 9;
          r = (0 + 0 + 0 + 0 + in_rows[i][j].bgra[2] + in_rows[i + 1][j].bgra[2] + 0 + in_rows[i][j + 1].bgra[2] + in_rows[i + 1][j + 1].bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // first row, last column
          b = (0 + 0 + 0 + in_rows[i][j - 1].bgra[0] + in_rows[i][j].bgra[0] + 0 + in_rows[i + 1][j - 1].bgra[0] + in_rows[i + 1][j].bgra[0] + 0) / 9;
          g = (0 + 0 + 0 + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + 0 + in_rows[i + 1][j - 1].bgra[1] + in_rows[i + 1][j].bgra[1] + 0) / 9;
          r = (0 + 0 + 0 + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + 0 + in_rows[i + 1][j - 1].bgra[2] + in_rows[i + 1][j].bgra[2] + 0) / 9;
        }
        else
        { // first row, middle columns
          b = (0 + 0 + 0 + in_rows[i][j - 1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j + 1].bgra[0] + in_rows[i + 1][j - 1].bgra[0] + in_rows[i + 1][j].bgra[0] + in_rows[i + 1][j + 1].bgra[0]) / 9;
          g = (0 + 0 + 0 + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j + 1].bgra[1] + in_rows[i + 1][j - 1].bgra[1] + in_rows[i + 1][j].bgra[1] + in_rows[i + 1][j + 1].bgra[1]) / 9;
          r = (0 + 0 + 0 + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j + 1].bgra[2] + in_rows[i + 1][j - 1].bgra[2] + in_rows[i + 1][j].bgra[2] + in_rows[i + 1][j + 1].bgra[2]) / 9;
        }
      }
      else if (i == height - 1)
      { // last row
        if (j == 0)
        { // last row, first column
          b = (0 + in_rows[i - 1][j].bgra[0] + in_rows[i - 1][j + 1].bgra[0] + 0 + in_rows[i][j].bgra[0] + in_rows[i][j + 1].bgra[0] + 0 + 0 + 0) / 9;
          g = (0 + in_rows[i - 1][j].bgra[1] + in_rows[i - 1][j + 1].bgra[1] + 0 + in_rows[i][j].bgra[1] + in_rows[i][j + 1].bgra[1] + 0 + 0 + 0) / 9;
          r = (0 + in_rows[i - 1][j].bgra[2] + in_rows[i - 1][j + 1].bgra[2] + 0 + in_rows[i][j].bgra[2] + in_rows[i][j + 1].bgra[2] + 0 + 0 + 0) / 9;
        }
        else if (j == width - 1)
        { // last row, last column
          b = (in_rows[i - 1][j - 1].bgra[0] + in_rows[i - 1][j + 1].bgra[0] + 0 + in_rows[i][i - 1].bgra[0] + in_rows[i][j].bgra[0] + 0 + 0 + 0 + 0) / 9;
          g = (in_rows[i - 1][j - 1].bgra[1] + in_rows[i - 1][j + 1].bgra[1] + 0 + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + 0 + 0 + 0 + 0) / 9;
          r = (in_rows[i - 1][j - 1].bgra[2] + in_rows[i - 1][j + 1].bgra[2] + 0 + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + 0 + 0 + 0 + 0) / 9;
        }
        else
        { // last row, middle columns
          b = (in_rows[i - 1][j - 1].bgra[0] + in_rows[i - 1][j].bgra[0] + in_rows[i - 1][j + 1].bgra[0] + in_rows[i][j - 1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j + 1].bgra[0] + 0 + 0 + 0) / 9;
          g = (in_rows[i - 1][j - 1].bgra[1] + in_rows[i - 1][j].bgra[1] + in_rows[i - 1][j + 1].bgra[1] + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j + 1].bgra[1] + 0 + 0 + 0) / 9;
          r = (in_rows[i - 1][j - 1].bgra[2] + in_rows[i - 1][j].bgra[2] + in_rows[i - 1][j + 1].bgra[2] + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j + 1].bgra[2] + 0 + 0 + 0) / 9;
        }
      }
      else
      { // middle rows
        if (j == 0)
        { // middle row, first column
          b = (0 + in_rows[i - 1][j].bgra[0] + in_rows[i - 1][j + 1].bgra[0] + 0 + in_rows[i][j].bgra[0] + in_rows[i][j + 1].bgra[0] + 0 + in_rows[i + 1][j].bgra[0] + in_rows[i + 1][j + 1].bgra[0]) / 9;
          g = (0 + in_rows[i - 1][j].bgra[1] + in_rows[i - 1][j + 1].bgra[1] + 0 + in_rows[i][j].bgra[1] + in_rows[i][j + 1].bgra[1] + 0 + in_rows[i + 1][j].bgra[1] + in_rows[i + 1][j + 1].bgra[1]) / 9;
          r = (0 + in_rows[i - 1][j].bgra[2] + in_rows[i - 1][j + 1].bgra[2] + 0 + in_rows[i][j].bgra[2] + in_rows[i][j + 1].bgra[2] + 0 + in_rows[i + 1][j].bgra[2] + in_rows[i + 1][j + 1].bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // middle row, last column
          b = (in_rows[i - 1][j - 1].bgra[0] + in_rows[i - 1][j].bgra[0] + 0 + in_rows[i][j - 1].bgra[0] + in_rows[i][j].bgra[0] + 0 + in_rows[i + 1][j - 1].bgra[0] + in_rows[i + 1][j].bgra[0] + 0) / 9;
          g = (in_rows[i - 1][j - 1].bgra[1] + in_rows[i - 1][j].bgra[1] + 0 + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + 0 + in_rows[i + 1][j - 1].bgra[1] + in_rows[i + 1][j].bgra[1] + 0) / 9;
          r = (in_rows[i - 1][j - 1].bgra[2] + in_rows[i - 1][j].bgra[2] + 0 + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + 0 + in_rows[i + 1][j - 1].bgra[2] + in_rows[i + 1][j].bgra[2] + 0) / 9;
        }
        else
        { // middle row, middle columns
          b = (in_rows[i - 1][j - 1].bgra[0] + in_rows[i - 1][j].bgra[0] + in_rows[i - 1][j + 1].bgra[0] + in_rows[i][j - 1].bgra[0] + in_rows[i][j].bgra[0] + in_rows[i][j + 1].bgra[0] + in_rows[i + 1][j - 1].bgra[0] + in_rows[i + 1][j].bgra[0] + in_rows[i + 1][j + 1].bgra[0]) / 9;
          g = (in_rows[i - 1][j - 1].bgra[1] + in_rows[i - 1][j].bgra[1] + in_rows[i - 1][j + 1].bgra[1] + in_rows[i][j - 1].bgra[1] + in_rows[i][j].bgra[1] + in_rows[i][j + 1].bgra[1] + in_rows[i + 1][j - 1].bgra[1] + in_rows[i + 1][j].bgra[1] + in_rows[i + 1][j + 1].bgra[1]) / 9;
          r = (in_rows[i - 1][j - 1].bgra[2] + in_rows[i - 1][j].bgra[2] + in_rows[i - 1][j + 1].bgra[2] + in_rows[i][j - 1].bgra[2] + in_rows[i][j].bgra[2] + in_rows[i][j + 1].bgra[2] + in_rows[i + 1][j - 1].bgra[2] + in_rows[i + 1][j].bgra[2] + in_rows[i + 1][j + 1].bgra[2]) / 9;
        }
      }
      out_rows[i][j].bgra[0] = (b > MAX_BGR_VALUE) ? MAX_BGR_VALUE : b;
      out_rows[i][j].bgra[1] = (g > MAX_BGR_VALUE) ? MAX_BGR_VALUE : g;
      out_rows[i][j].bgra[2] = (r > MAX_BGR_VALUE) ? MAX_BGR_VALUE : r;
    }
  }
  return output_image_ptr;
}

/* applyBlur Optimization 2 - Code Motion */
ImagePtr applyBlurOpt2(ImagePtr image_ptr)
{
  auto output_image_ptr =
      std::make_shared<Image>(image_ptr->name() + "_blurred",
                              IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  int height = in_rows.size();
  int width = in_rows[1] - in_rows[0]; 
 double b, g, r;
  for (unsigned long i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      const Pixel &p1 = in_rows[i-1][j-1];
      const Pixel &p2 = in_rows[i-1][j];
      const Pixel &p3 = in_rows[i-1][j+1];
      const Pixel &p4 = in_rows[i][j-1];
      const Pixel &p5 = in_rows[i][j];
      const Pixel &p6 = in_rows[i][j + 1];
      const Pixel &p7 = in_rows[i+1][j - 1];
      const Pixel &p8 = in_rows[i+1][j];
      const Pixel &p9 = in_rows[i+1][j+1];
      // Average = ([i-1][j-1] + [i-1][j] + [i-1][j+1] + [i][j-1] + [i][j] + [i][j+1] + [i+1][j-1] + [i+1][j] + [i+1][j+1])/ 9
      if (i == 0)
      { // first row
        if (j == 0)
        { // first row, first column
          b = (0 + 0 + 0 + 0 + p5.bgra[0] + p8.bgra[0] + 0 + p6.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + 0 + p5.bgra[1] + p8.bgra[1] + 0 + p6.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + 0 + p5.bgra[2] + p8.bgra[2] + 0 + p6.bgra[2] + p9.bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // first row, last column
          b = (0 + 0 + 0 + p4.bgra[0] + p5.bgra[0] + 0 + p7.bgra[0] + p8.bgra[0] + 0) / 9;
          g = (0 + 0 + 0 + p4.bgra[1] + p5.bgra[1] + 0 + p7.bgra[1] + p8.bgra[1] + 0) / 9;
          r = (0 + 0 + 0 + p4.bgra[2] + p5.bgra[2] + 0 + p7.bgra[2] + p8.bgra[2] + 0) / 9;
        }
        else
        { // first row, middle columns
          b = (0 + 0 + 0 + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
        }
      }
      else if (i == height - 1)
      { // last row
        if (j == 0)
        { // last row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
        }
        else if (j == width - 1)
        { // last row, last column
          b = (p1.bgra[0] + p3.bgra[0] + 0 + p4.bgra[0] + p5.bgra[0] + 0 + 0 + 0 + 0) / 9;
          g = (p1.bgra[1] + p3.bgra[1] + 0 + p4.bgra[1] + p5.bgra[1] + 0 + 0 + 0 + 0) / 9;
          r = (p1.bgra[2] + p3.bgra[2] + 0 + p4.bgra[2] + p5.bgra[2] + 0 + 0 + 0 + 0) / 9;
        }
        else
        { // last row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
        }
      }
      else
      { // middle rows
        if (j == 0)
        { // middle row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + p8.bgra[2] + p9.bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // middle row, last column
          b = (p1.bgra[0] + p2.bgra[0] + 0 + p4.bgra[0] + p5.bgra[0] + 0 + p7.bgra[0] + p8.bgra[0] + 0) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + 0 + p4.bgra[1] + p5.bgra[1] + 0 + p7.bgra[1] + p8.bgra[1] + 0) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + 0 + p4.bgra[2] + p5.bgra[2] + 0 + p7.bgra[2] + p8.bgra[2] + 0) / 9;
        }
        else
        { // middle row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
        }
      }
      out_rows[i][j].bgra[0] = (b > MAX_BGR_VALUE) ? MAX_BGR_VALUE : b;
      out_rows[i][j].bgra[1] = (g > MAX_BGR_VALUE) ? MAX_BGR_VALUE : g;
      out_rows[i][j].bgra[2] = (r > MAX_BGR_VALUE) ? MAX_BGR_VALUE : r;
    }
  }
  return output_image_ptr;
}

/* applyBlur Opt 3 - Loop Unrolling 2x1 */

ImagePtr applyBlurOpt3(ImagePtr image_ptr)
{
  auto output_image_ptr =
      std::make_shared<Image>(image_ptr->name() + "_blurred",
                              IMAGE_WIDTH, IMAGE_HEIGHT);
  auto in_rows = image_ptr->rows();
  auto out_rows = output_image_ptr->rows();
  double b, g, r, bb, gg, rr;
  int height = in_rows.size();
  int width = in_rows[1] - in_rows[0];
  int limit = width - 2;
  long i; int j;
  for (i = 0; i < height; ++i)
  {
    for (j = 0; j < limit; j = j + 2)
    {
      int k = j+1;
      const Pixel &p1 = in_rows[i-1][j-1];
      const Pixel &p2 = in_rows[i-1][j];
      const Pixel &p3 = in_rows[i-1][j+1];
      const Pixel &p4 = in_rows[i][j-1];
      const Pixel &p5 = in_rows[i][j];
      const Pixel &p6 = in_rows[i][j + 1];
      const Pixel &p7 = in_rows[i+1][j - 1];
      const Pixel &p8 = in_rows[i+1][j];
      const Pixel &p9 = in_rows[i+1][j+1];

      const Pixel &pp1 = in_rows[i - 1][k-1];
      const Pixel &pp2 = in_rows[i - 1][k];
      const Pixel &pp3 = in_rows[i - 1][k+1];
      const Pixel &pp4 = in_rows[i][k-1];
      const Pixel &pp5 = in_rows[i][k];
      const Pixel &pp6 = in_rows[i][k+1];
      const Pixel &pp7 = in_rows[i + 1][k-1];
      const Pixel &pp8 = in_rows[i + 1][k];
      const Pixel &pp9 = in_rows[i + 1][k+1];
      // Average = ([i-1][j-1] + [i-1][j] + [i-1][j+1] + [i][j-1] + [i][j] + [i][j+1] + [i+1][j-1] + [i+1][j] + [i+1][j+1])/ 9
      if (i == 0)
      { // first row
        if (j == 0)
        { // first row, first column
          b = (0 + 0 + 0 + 0 + p5.bgra[0] + p8.bgra[0] + 0 + p6.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + 0 + p5.bgra[1] + p8.bgra[1] + 0 + p6.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + 0 + p5.bgra[2] + p8.bgra[2] + 0 + p6.bgra[2] + p9.bgra[2]) / 9;
          bb = (0 + 0 + 0 + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + pp7.bgra[0] + pp8.bgra[0] + pp9.bgra[0]) / 9;
          gg = (0 + 0 + 0 + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + pp7.bgra[1] + pp8.bgra[1] + pp9.bgra[1]) / 9;
          rr = (0 + 0 + 0 + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + pp7.bgra[2] + pp8.bgra[2] + pp9.bgra[2]) / 9;
        }
        else
        { // first row, middle columns
          b = (0 + 0 + 0 + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
          bb = (0 + 0 + 0 + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + pp7.bgra[0] + pp8.bgra[0] + pp9.bgra[0]) / 9;
          gg = (0 + 0 + 0 + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + pp7.bgra[1] + pp8.bgra[1] + pp9.bgra[1]) / 9;
          rr = (0 + 0 + 0 + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + pp7.bgra[2] + pp8.bgra[2] + pp9.bgra[2]) / 9;
        }
      }
      else if (i == height - 1)
      { // last row
        if (j == 0)
        { // last row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
          bb = (pp1.bgra[0] + pp2.bgra[0] + pp3.bgra[0] + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + 0 + 0 + 0) / 9;
          gg = (pp1.bgra[1] + pp2.bgra[1] + pp3.bgra[1] + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + 0 + 0 + 0) / 9;
          rr = (pp1.bgra[2] + pp2.bgra[2] + pp3.bgra[2] + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + 0 + 0 + 0) / 9;
        }
        else
        { // last row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
          bb = (pp1.bgra[0] + pp2.bgra[0] + pp3.bgra[0] + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + 0 + 0 + 0) / 9;
          gg = (pp1.bgra[1] + pp2.bgra[1] + pp3.bgra[1] + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + 0 + 0 + 0) / 9;
          rr = (pp1.bgra[2] + pp2.bgra[2] + pp3.bgra[2] + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + 0 + 0 + 0) / 9;
        }
      }
      else
      { // middle rows
        if (j == 0)
        { // middle row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + p8.bgra[2] + p9.bgra[2]) / 9;
          bb = (pp1.bgra[0] + pp2.bgra[0] + pp3.bgra[0] + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + pp7.bgra[0] + pp8.bgra[0] + pp9.bgra[0]) / 9;
          gg = (pp1.bgra[1] + pp2.bgra[1] + pp3.bgra[1] + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + pp7.bgra[1] + pp8.bgra[1] + pp9.bgra[1]) / 9;
          rr = (pp1.bgra[2] + pp2.bgra[2] + pp3.bgra[2] + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + pp7.bgra[2] + pp8.bgra[2] + pp9.bgra[2]) / 9;
        }
        else
        { // middle row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
          bb = (pp1.bgra[0] + pp2.bgra[0] + pp3.bgra[0] + pp4.bgra[0] + pp5.bgra[0] + pp6.bgra[0] + pp7.bgra[0] + pp8.bgra[0] + pp9.bgra[0]) / 9;
          gg = (pp1.bgra[1] + pp2.bgra[1] + pp3.bgra[1] + pp4.bgra[1] + pp5.bgra[1] + pp6.bgra[1] + pp7.bgra[1] + pp8.bgra[1] + pp9.bgra[1]) / 9;
          rr = (pp1.bgra[2] + pp2.bgra[2] + pp3.bgra[2] + pp4.bgra[2] + pp5.bgra[2] + pp6.bgra[2] + pp7.bgra[2] + pp8.bgra[2] + pp9.bgra[2]) / 9;
        }
      }
      out_rows[i][j].bgra[0] = (b > MAX_BGR_VALUE) ? MAX_BGR_VALUE : b;
      out_rows[i][j].bgra[1] = (g > MAX_BGR_VALUE) ? MAX_BGR_VALUE : g;
      out_rows[i][j].bgra[2] = (r > MAX_BGR_VALUE) ? MAX_BGR_VALUE : r;
      out_rows[i][j+1].bgra[0] = (bb > MAX_BGR_VALUE) ? MAX_BGR_VALUE : bb;
      out_rows[i][j+1].bgra[1] = (gg > MAX_BGR_VALUE) ? MAX_BGR_VALUE : gg;
      out_rows[i][j+1].bgra[2] = (rr > MAX_BGR_VALUE) ? MAX_BGR_VALUE : rr;
    }
    for(; j < width; ++j){
      const Pixel &p1 = in_rows[i - 1][j - 1];
      const Pixel &p2 = in_rows[i - 1][j];
      const Pixel &p3 = in_rows[i - 1][j + 1];
      const Pixel &p4 = in_rows[i][j - 1];
      const Pixel &p5 = in_rows[i][j];
      const Pixel &p6 = in_rows[i][j + 1];
      const Pixel &p7 = in_rows[i + 1][j - 1];
      const Pixel &p8 = in_rows[i + 1][j];
      const Pixel &p9 = in_rows[i + 1][j + 1];

      if (i == 0)
      { // first row
        if (j == 0)
        { // first row, first column
          b = (0 + 0 + 0 + 0 + p5.bgra[0] + p8.bgra[0] + 0 + p6.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + 0 + p5.bgra[1] + p8.bgra[1] + 0 + p6.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + 0 + p5.bgra[2] + p8.bgra[2] + 0 + p6.bgra[2] + p9.bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // first row, last column
          b = (0 + 0 + 0 + p4.bgra[0] + p5.bgra[0] + 0 + p7.bgra[0] + p8.bgra[0] + 0) / 9;
          g = (0 + 0 + 0 + p4.bgra[1] + p5.bgra[1] + 0 + p7.bgra[1] + p8.bgra[1] + 0) / 9;
          r = (0 + 0 + 0 + p4.bgra[2] + p5.bgra[2] + 0 + p7.bgra[2] + p8.bgra[2] + 0) / 9;
        }
        else
        { // first row, middle columns
          b = (0 + 0 + 0 + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + 0 + 0 + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + 0 + 0 + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
        }
      }
      else if (i == height - 1)
      { // last row
        if (j == 0)
        { // last row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
        }
        else if (j == width - 1)
        { // last row, last column
          b = (p1.bgra[0] + p3.bgra[0] + 0 + p4.bgra[0] + p5.bgra[0] + 0 + 0 + 0 + 0) / 9;
          g = (p1.bgra[1] + p3.bgra[1] + 0 + p4.bgra[1] + p5.bgra[1] + 0 + 0 + 0 + 0) / 9;
          r = (p1.bgra[2] + p3.bgra[2] + 0 + p4.bgra[2] + p5.bgra[2] + 0 + 0 + 0 + 0) / 9;
        }
        else
        { // last row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + 0 + 0 + 0) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + 0 + 0 + 0) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + 0 + 0 + 0) / 9;
        }
      }
      else
      { // middle rows
        if (j == 0)
        { // middle row, first column
          b = (0 + p2.bgra[0] + p3.bgra[0] + 0 + p5.bgra[0] + p6.bgra[0] + 0 + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (0 + p2.bgra[1] + p3.bgra[1] + 0 + p5.bgra[1] + p6.bgra[1] + 0 + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (0 + p2.bgra[2] + p3.bgra[2] + 0 + p5.bgra[2] + p6.bgra[2] + 0 + p8.bgra[2] + p9.bgra[2]) / 9;
        }
        else if (j == width - 1)
        { // middle row, last column
          b = (p1.bgra[0] + p2.bgra[0] + 0 + p4.bgra[0] + p5.bgra[0] + 0 + p7.bgra[0] + p8.bgra[0] + 0) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + 0 + p4.bgra[1] + p5.bgra[1] + 0 + p7.bgra[1] + p8.bgra[1] + 0) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + 0 + p4.bgra[2] + p5.bgra[2] + 0 + p7.bgra[2] + p8.bgra[2] + 0) / 9;
        }
        else
        { // middle row, middle columns
          b = (p1.bgra[0] + p2.bgra[0] + p3.bgra[0] + p4.bgra[0] + p5.bgra[0] + p6.bgra[0] + p7.bgra[0] + p8.bgra[0] + p9.bgra[0]) / 9;
          g = (p1.bgra[1] + p2.bgra[1] + p3.bgra[1] + p4.bgra[1] + p5.bgra[1] + p6.bgra[1] + p7.bgra[1] + p8.bgra[1] + p9.bgra[1]) / 9;
          r = (p1.bgra[2] + p2.bgra[2] + p3.bgra[2] + p4.bgra[2] + p5.bgra[2] + p6.bgra[2] + p7.bgra[2] + p8.bgra[2] + p9.bgra[2]) / 9;
        }
      }
      out_rows[i][j].bgra[0] = (b > MAX_BGR_VALUE) ? MAX_BGR_VALUE : b;
      out_rows[i][j].bgra[1] = (g > MAX_BGR_VALUE) ? MAX_BGR_VALUE : g;
      out_rows[i][j].bgra[2] = (r > MAX_BGR_VALUE) ? MAX_BGR_VALUE : r;
    }
  }
  return output_image_ptr;
}

void writeImage(ImagePtr image_ptr) {
  image_ptr->write( (image_ptr->name() + ".bmp").c_str());
}

void checkCorrectness(std::vector<ImagePtr> image_vector, std::vector<ImagePtr> image_vector_original){
  for(unsigned long k=0; k<image_vector.size(); k++){
    ImagePtr img1 = image_vector[k];
    ImagePtr img2 = image_vector_original[k];
    auto in_rows1 = img1->rows();
    auto in_rows2 = img2->rows();
    int height1 = img1->height();
    int width1 = img1->width();
    int height2 = img2->height();
    int width2 = img2->width();
    if(height1 != height2 || width1 != width2){
      printf("The two images do not have the same dimensions");
      return;
    }
    for(int i=0; i<height1; i++){
      for(int j=0; j<width1; j++){
        if(in_rows1[i][j].value != in_rows2[i][j].value){
            printf("Correctness check failed for pixels image_%ld[%d][%d] = %d and image_refrence_%ld[%d][%d] = %d\n", (k+1), i, j, in_rows1[i][j].value, (k+1), i, i, in_rows2[i][j].value);
            return;
        }
      }
    }
    printf("Correctness check passed for image %ld\n", (k+1));
  }
}
int main() {
  const double tint_array[] = {0.75, 0, 0};
  // Create two vector to hold 4 fractal images
  std::vector<ImagePtr> image_vector, image_vector_reference;
  for ( int i = 2000; i <= 2000000; i *= 10 ) {
    image_vector.push_back(makeFractalImage(i));
    image_vector_reference.push_back(makeFractalImage(i));
  }
  // Store the output of the original functions applyGamma, applyTint, applyBlur in image_vector_reference
  // The output images will be used for checking the correctness of the optimized functions
  for(ImagePtr img:image_vector_reference){
    img = applyGamma(img, 1.4);
    img = applyTint(img, tint_array);
    img = applyBlur(img);
  }
  // Process the images in the vector image_vector
  process_images(image_vector);
  // check the output images of process_images against the images processed by the original functions
  checkCorrectness(image_vector, image_vector_reference);
  return 0;
}

