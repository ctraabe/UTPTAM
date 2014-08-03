#ifndef __QUATERNION_H
#define __QUATERNION_H

#include <TooN/TooN.h>
#include <TooN/se3.h>
#include <TooN/so3.h>

namespace PTAMM {

template<typename Precision = double>
class Quaternion {

public:
  // Default constructor. Initializes the quaternion to zero rotation.
  Quaternion() : my_array{1., 0., 0., 0.} {}

  // Construct from 4 parameters.
  Quaternion(const Precision e0, const Precision ex, const Precision ey,
      const Precision ez) : my_array {e0, ex, ey, ez} {}

  // Construct from Vector(4) (consider normalizing afterward to ensure valid).
  Quaternion(const TooN::Vector<4, Precision> & V) : my_array{V[0], V[1], V[2],
      V[3]} {}

  // Construct from SO3 (rotation matrix).
  Quaternion(const TooN::SO3<Precision> & R) {
    FromMatrix(R.get_matrix());
  }

  // Construct from SE3 (rotation matrix and (discarded) translation).
  Quaternion(const TooN::SE3<Precision> & T) {
    FromMatrix(T.get_rotation().get_matrix());
  }

  // Construct from an Euler-Rodrigues Vector(3) (magnitude along axis).
  Quaternion(const TooN::Vector<3, Precision> & V) {
    Precision alpha_squared = V[0] * V[0] + V[1] * V[1] + V[2] * V[2];
    if (alpha_squared == 0.) {
      my_array[0] = 1.;
      my_array[1] = 0.;
      my_array[2] = 0.;
      my_array[3] = 0.;
    } else {
      Precision alpha = std::sqrt(alpha_squared);
      Precision c = std::sin(0.5 * alpha) / alpha;
      my_array[0] = std::cos(0.5 * alpha);
      my_array[1] = c * V[0];
      my_array[2] = c * V[1];
      my_array[3] = c * V[2];
    }
  }

  Precision & operator[] (const int index) {
    assert(index >= 0 && index < 4);
    return my_array[index];
  }

  // Multiply by another quaternion on the right.
  template<typename P>
  Quaternion& operator *=(const Quaternion<P> & rhs) {
    my_array[0] 
      = my_array[0] * rhs.get_array()[0] - my_array[1] * rhs.get_array()[1]
      - my_array[2] * rhs.get_array()[2] - my_array[3] * rhs.get_array()[3];
    my_array[1] 
      = my_array[0] * rhs.get_array()[1] + my_array[1] * rhs.get_array()[0]
      + my_array[2] * rhs.get_array()[3] - my_array[3] * rhs.get_array()[2];
    my_array[2] 
      = my_array[0] * rhs.get_array()[2] - my_array[1] * rhs.get_array()[3]
      + my_array[2] * rhs.get_array()[0] + my_array[3] * rhs.get_array()[1];
    my_array[3] 
      = my_array[0] * rhs.get_array()[3] + my_array[1] * rhs.get_array()[2]
      - my_array[2] * rhs.get_array()[1] + my_array[3] * rhs.get_array()[0];
    return *this;
  }

  // Ensures quaternion has norm2 of 1 (i.e. is a valid rotation quaternion).
  void normalize() {
    Precision norm_squared = my_array[0] * my_array[0]
      + my_array[1] * my_array[1] + my_array[2] * my_array[2]
      + my_array[3] * my_array[3];
    if (norm_squared == 0.0) {
      my_array[0] = 1.0;
    } else if (norm_squared != 1.0) {
      Precision norm_inverse = 1.0 / std::sqrt(norm_squared);
      my_array[0] *= norm_inverse;
      my_array[1] *= norm_inverse;
      my_array[2] *= norm_inverse;
      my_array[3] *= norm_inverse;
    }
  }

  // Returns the inverse of this quaternion.
  Quaternion inverse() const {
    Quaternion<Precision> ret = this;
    ret.my_array[1] = -ret.my_array[1];
    ret.my_array[2] = -ret.my_array[2];
    ret.my_array[3] = -ret.my_array[3];
    return ret;
  }

  // Returns the Quaternion as a Vector<4>
  const std::array<Precision, 4> & get_array() const { return my_array; }

private:

  void FromMatrix(const TooN::Matrix<3, 3, Precision> & R) {
    Precision trace = R[0][0] + R[1][1] + R[2][2];
    if (trace > 0.) {
      Precision r = sqrt(1 + trace);
      Precision s = 0.5 / r;
      my_array[0] = 0.5 * r;
      my_array[1] = (R[2][1] - R[1][2]) * s;
      my_array[2] = (R[0][2] - R[2][0]) * s;
      my_array[3] = (R[1][0] - R[0][1]) * s;
    } else if (R[0][0] > R[1][1] && R[0][0] > R[2][2]) {
      Precision r = sqrt(1 + R[0][0] - R[1][1] - R[2][2]);
      Precision s = 0.5 / r;
      my_array[0] = (R[2][1] - R[1][2]) * s;
      my_array[1] = 0.5 * r;
      my_array[2] = (R[0][1] + R[1][0]) * s;
      my_array[3] = (R[0][2] + R[2][0]) * s;
    } else if (R[1][1] > R[2][2]) {
      Precision r = sqrt(1 - R[0][0] + R[1][1] - R[2][2]);
      Precision s = 0.5 / r;
      my_array[0] = (R[0][2] - R[2][0]) * s;
      my_array[1] = (R[0][1] + R[1][0]) * s;
      my_array[2] = 0.5 * r;
      my_array[3] = (R[1][2] + R[2][1]) * s;
    } else {
      Precision r = sqrt(1 - R[0][0] - R[1][1] + R[2][2]);
      Precision s = 0.5 / r;
      my_array[0] = (R[1][0] - R[0][1]) * s;
      my_array[1] = (R[0][2] + R[2][0]) * s;
      my_array[2] = (R[1][2] + R[2][1]) * s;
      my_array[3] = 0.5 * r;
    }
  }

  std::array<Precision, 4> my_array;
};  // End of class

template<typename P>
Quaternion<P> operator*(Quaternion<P> lhs, Quaternion<P> const& rhs)
{
    return lhs *= rhs;
}

}  // End of namespace

#endif
