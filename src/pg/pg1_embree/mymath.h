#ifndef MY_MATH_H_
#define MY_MATH_H_

#define _USE_MATH_DEFINES
#include <boost/math/special_functions/beta.hpp>
#include <math.h>
#include <float.h>
#include <iostream>
#include "structs.h"

template <class T> inline T sqr( const T x )
{
	return x * x;
}

inline Normal3f normalize( const Normal3f & n )
{
	float tmp = sqr( n.x ) + sqr( n.y ) + sqr( n.z );

	if ( fabsf( tmp ) > FLT_EPSILON )
	{
		tmp = 1.0f / tmp;
		return Normal3f{ n.x * tmp, n.y * tmp, n.z * tmp };
	}

	return n;
}

inline float deg2rad( const float x )
{
	return x * float( M_PI ) / 180.0f;
}

inline Vector3 T_b_l(Vector3 mu, float l)
{
    Vector3 ret;
    ret.x = exp((-mu.x) * l);
    ret.y = exp((-mu.y) * l);
    ret.z *= exp((-mu.z) * l);
    return ret;
}

inline Vector3 orthogonal( const Vector3 & n )
{
    return ( abs( n.x ) > abs( n.z ) ) ? Vector3( n.y, -n.x, 0.0f ) : Vector3( 0.0f, n.z, -n.y );
}

inline float clamp(float x, float x0 = 0.0f, float x1 = 1.0f){
    return max(min(x, x1), x0);
}

inline float ibeta( float x, float a, float b){
    try {
        return boost::math::ibeta( a, b, x );
    } catch (std::exception &e) {
        std::cout << "ibeta exception: " << e.what() << std::endl;
        std::cout << "x: " << x << " a: " << a << " b: " << b << std::endl;
        return 0.0f;
    }
}

inline static float gamma_quot(float a, float b){
    return std::exp(std::lgamma(a) - std::lgamma(b));
}

#endif
