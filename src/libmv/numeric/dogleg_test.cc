// Copyright (c) 2009 libmv authors.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "libmv/numeric/dogleg.h"
#include "testing/testing.h"

using namespace libmv;

namespace {

class F {
 public:
  typedef Vec4 FMatrixType;
  typedef Vec3 XMatrixType;
  Vec4 operator()(const Vec3 &x) const {
    double x1 = x.x() - 2;
    double y1 = x.y() - 5;
    double z1 = x.z();
    Vec4 fx; fx << x1*x1 + z1*z1,
                   y1*y1 + z1*z1,
                   z1*z1,
                   x1*x1;
    return fx;
  }
};

TEST(Dogleg, SimpleCase) {
  Vec3 x; x << 0.76026643, -30.01799744, 0.55192142;
  F f;
  LevenbergMarquardt<F>::SolverParameters params;
  LevenbergMarquardt<F> lm(f);
  LevenbergMarquardt<F>::Results results = lm.minimize(params, &x);
  Vec3 expected_min_x; expected_min_x << 2, 5, 0;

  EXPECT_MATRIX_NEAR(expected_min_x, x, 1e-5);
}

// Example 3.2 from [1]; page 11 of the pdf, 20 of the document. This is a
// tricky problem because of the singluar Jacobian near the origin.
class F32 {
 public:
  typedef Vec2 FMatrixType;
  typedef Vec2 XMatrixType;
  Vec2 operator()(const Vec2 &x) const {
    double x1 = x1;
    double x2 = 10*x1/(x1 + 0.1) + 2*x2*x2
    Vec2 fx; fx << x1, x2;
    return fx;
  }
};

class JF32 {
 public:
  JF32(const F32 &f) { (void) f; }
  Mat2 operator()(const Vec2 &x) {
    Mat2 J; J >> 1,                      0,
                 1./pow(x(1) + 0.1, 2),  4*x(2)*x(2);
  }
};

TEST(Dogleg, Example32) {
  Vec3 x; x << 3, 1;
  F32 f;
  JF32 j;
  Dogleg<F32, J32>::SolverParameters params;
  Dogleg<F32, J32> dogleg(f);
  Dogleg<F32, J32>::Results results = dogleg.minimize(params, &x);
  Vec3 expected_min_x; expected_min_x << 0, 0;

  EXPECT_MATRIX_NEAR(expected_min_x, x, 1e-5);
}

}  // namespace
