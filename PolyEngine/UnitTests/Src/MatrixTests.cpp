#include <Defines.hpp>
#include <catch.hpp>

#include <Math/Matrix.hpp>
#include <Math/Quaternion.hpp>

using namespace Poly;

TEST_CASE("Matrix constructors", "[Matrix]") {
  // empty constructor
  Matrix m1;
  for(int i=0; i<16; ++i)
    REQUIRE(m1.Data[i] == (i%5==0?1:0));

  // basic constructor
  float data[] = {0,1,2,3,
		  4,5,6,7,
		  8,9,10,11,
		  12,13,14,15};
  Matrix m2(data);
  for(int i=0; i<16; ++i)
    REQUIRE(m2.Data[i] == i);

  // copy constructor
  Matrix m3(m2);
  for(int i=0; i<16; ++i)
    REQUIRE(m3.Data[i] == i);

  // = operator
  m1 = m2;
  for(int i=0; i<16; ++i)
    REQUIRE(m1.Data[i] == i);
}

TEST_CASE("Matrix comparison operators", "[Matrix]") {
  float data[] = {0,1,2,3,
		  4,5,6,7,
		  8,9,10,11,
		  12,13,14,15};
  Matrix m1(data);
  Matrix m2(data);

  SECTION("Positive comparison operator") {
    REQUIRE(m1 == m2);
    m1.m33 = -1;
    REQUIRE_FALSE(m1 == m2);
  }

  SECTION("Negative comparison operator") {
    REQUIRE_FALSE(m1 != m2);
    m1.m33 = -1;
    REQUIRE(m1 != m2);
  }
}

TEST_CASE("Matrix-Matrix multiplication operators", "[Matrix]") {
  float data1[] = {0,1,2,3,
    4,5,6,7,
    8,9,10,11,
    12,13,14,15};
  float data2[] = {10,11,12,13,
    14,15,16,17,
    18,19,110,111,
    112,113,114,115};
  Matrix m1(data1);
  Matrix m2(data2);

  // NOTE: Column major order memory layout
  SECTION("Multiplication operator") {
    float data3[] = {386,392,578,584,
      1002,1024,1586,1608,
      1618,1656,2594,2632,
      2234,2288,3602,3656};
    Matrix m3(data3);
    REQUIRE(m1*m2 == m3);

    float data4[] = {296,342,388,434,
      392,454,516,578,
      2288,2546,2804,3062,
      2744,3198,3652,4106};
    Matrix m4(data4);
    REQUIRE(m2*m1 == m4);

    m3 = m1;
    m3 *= m2;
    REQUIRE(m3 == m1*m2);

    m3 = m2;
    m3 *= m1;
    REQUIRE(m3 == m2*m1);
  }

}

TEST_CASE("Matrx-Vector multiplication operator", "[Matrix]") {
  float data1[] = {1,0,0,0,
    0,0,1,0,
    0,-1,0,0,
    0,0,0,1};
  float data2[] = {1,0,0,3,
		  0,1,0,3,
		  0,0,1,3,
		  0,0,0,1};
  float data3[] = {2,0,0,0,
    0,2,0,0,
    0,0,2,0,
    0,0,0,1};
  Matrix m1(data1);
  Matrix m2(data2);
  Matrix m3(data3);
  Vector v1(1,2,3);
  Vector v2(4,6,5);

  REQUIRE(m1*v1 == Vector(1,3,-2));
  REQUIRE(m2*v1 == Vector(4,5,6));
  REQUIRE(m3*v1 == Vector(2,4,6));
  REQUIRE(m3*m2*m1*v1 == Vector(8,12,2));
}

TEST_CASE("Matrix algebraic methods", "[Matrix]") {
  float data1[] = {32,85,58,72,
    7,3,97,64,
    11,27,10,81,
    46,34,17,-1};
  Matrix m1(data1);

  SECTION("Determinant calculation") {
    REQUIRE(m1.Det() == -17516092);
  }

  SECTION("Transposition") {
    float data2[] = {32,7,11,46,
      85,3,27,34,
      58,97,10,17,
      72,64,81,-1};
    Matrix m2(data2);

    Matrix m3 = m1;
    m3.Transpose();
    REQUIRE(m3 == m2);
    REQUIRE(m1.GetTransposed() == m2);

    m3 = m1;
    REQUIRE(m3.Transpose().Transpose() == m1);
  }

  SECTION("Inverse matrix calculation") {
    float data2[] = {-273212,51971,208127,513167,
                    335308,-152706,-179458,-167106,
                    63808 ,166157,-188307,-24643,
                    -82544,23331,271051,-10945};
    Matrix m2(data2);
    for(int i=0; i<16; ++i)
      m2.Data[i] /= 17516092.0f;

    Matrix m3 = m1;
    m3.Inverse();
    REQUIRE(m3 == m2);
    REQUIRE(m1.GetInversed() == m2);

    m3 = m1;
    REQUIRE(m3.Inverse().Inverse() == m1);
  }
}

TEST_CASE("Matrix set methods","[Matrix]") {
  Matrix m1;

  SECTION("Set identity method") {
    float data[] = {1,0,0,0,
		    0,1,0,0,
		    0,0,1,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetIdentity();
    REQUIRE(m1 == m2);
  }

  SECTION("Set rotation about X axis method") {
    float data[] = {1,0,0,0,
		    0,0,-1,0,
		    0,1,0,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetRotationX(90_deg);
    REQUIRE(m1 == m2);
  }

  SECTION("Set rotation about Y axis method") {
    float data[] = {0,0,1,0,
		    0,1,0,0,
		    -1,0,0,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetRotationY(90_deg);
    REQUIRE(m1 == m2);
  }

  SECTION("Set rotation about Z axis method") {
    float data[] = {0,-1,0,0,
		    1,0,0,0,
		    0,0,1,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetRotationZ(90_deg);
    REQUIRE(m1 == m2);
  }

  SECTION("Set translation method") {
    float data[] = {1,0,0,1,
		    0,1,0,2,
		    0,0,1,3,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetTranslation(Vector(1,2,3));
    REQUIRE(m1 == m2);
  }

  SECTION("Set even scale method") {
    float data[] = {3,0,0,0,
		    0,3,0,0,
		    0,0,3,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetScale(3);
    REQUIRE(m1 == m2);
  }

  SECTION("Set uneven scale method") {
    float data[] = {2,0,0,0,
		    0,3,0,0,
		    0,0,4,0,
		    0,0,0,1};
    Matrix m2(data);
    m1.SetScale(Vector(2,3,4));
    REQUIRE(m1 == m2);
  }
}

TEST_CASE("Matrix decomposition", "[Matrix]") {
	Vector trans(1, 2, 3);
	Quaternion rot(Vector(1,1,1).GetNormalized(), 45_deg);
	Vector scale(2, 3, 4);

	Matrix tMat, rMat, sMat;
	tMat.SetTranslation(trans);
	rMat = (Matrix)rot;
	sMat.SetScale(scale);

	// result fields
	Vector t;
	Quaternion r;
	Vector s;
	MatrixSkew skew;
	Vector p;

	SECTION("Translation decomposition") {
		tMat.Decompose(t, r, s, skew, p);
		REQUIRE(t == trans);
		REQUIRE(r == Quaternion());
		REQUIRE(s == Vector(1,1,1));

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Rotation decomposition") {
		rMat.Decompose(t, r, s, skew, p);
		REQUIRE(t == Vector(0,0,0));
		REQUIRE(r == rot);
		REQUIRE(s == Vector(1, 1, 1));

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Scale decomposition") {
		sMat.Decompose(t, r, s, skew, p);
		REQUIRE(t == Vector(0, 0, 0));
		REQUIRE(r == Quaternion());
		REQUIRE(s == scale);

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Translation-Rot decomposition") {
		(tMat*rMat).Decompose(t, r, s, skew, p);
		REQUIRE(t == trans);
		REQUIRE(r == rot);
		REQUIRE(s == Vector(1, 1, 1));

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Translation-Scale decomposition") {
		(tMat*sMat).Decompose(t, r, s, skew, p);
		REQUIRE(t == trans);
		REQUIRE(r == Quaternion());
		REQUIRE(s == scale);

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Rotation-Scale decomposition") {
		(rMat*sMat).Decompose(t, r, s, skew, p);
		REQUIRE(t == Vector(0, 0, 0));
		REQUIRE(r == rot);
		REQUIRE(s == scale);

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}

	SECTION("Translation-Rotation-Scale decomposition") {
		(tMat * (rMat * sMat)).Decompose(t, r, s, skew, p);
		REQUIRE(t == trans);
		REQUIRE(r == rot);
		REQUIRE(s == scale);

		// check if zero skew and zero perspective
		REQUIRE(Cmpf(skew.XY, 0.f));
		REQUIRE(Cmpf(skew.XZ, 0.f));
		REQUIRE(Cmpf(skew.YZ, 0.f));
		REQUIRE(p == Vector());
	}
}
