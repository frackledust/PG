#include "stdafx.h"
#include "vector3.h"
#include "mymath.h"

Vector3::Vector3( const float * v )
{
	assert( v != NULL );

	x = v[0];
	y = v[1];
	z = v[2];
}

float Vector3::L2Norm() const
{
	return sqrt( sqr(x) + sqr(y) + sqr(z));
}

float Vector3::SqrL2Norm() const
{
	return sqr(x) + sqr(y) + sqr(z);
}

void Vector3::Normalize()
{
	const float norm = SqrL2Norm();

	if ( norm != 0 )
	{
		const float rn = 1 / sqrt( norm );

		x *= rn;
		y *= rn;
		z *= rn;
	}
}

Vector3 Vector3::CrossProduct( const Vector3 & v ) const
{
	return Vector3(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x );
}

Vector3 Vector3::Abs() const
{
	return Vector3( abs( x ), abs( y ), abs( z ) );		
}

Vector3 Vector3::Max( const float a ) const
{
	return Vector3( max( x, a ), max( y, a ), max( z, a ) );
}

float Vector3::DotProduct( const Vector3 & v ) const
{
	return x * v.x + y * v.y + z * v.z;
}

float Vector3::LargestComponentValue() const
{
    return max( x, max( y, z ) );
}

char Vector3::LargestComponent( const bool absolute_value )
{
	const Vector3 d = ( absolute_value )? Vector3( abs( x ), abs( y ), abs( z ) ) : *this;

	if ( d.x > d.y )
	{
		if ( d.x > d.z )
		{			
			return 0 ;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		if ( d.y > d.z )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	return -1;
}

void Vector3::Print()
{
	printf( "(%0.3f, %0.3f, %0.3f)\n", x, y, z ); 
	//printf( "_point %0.3f,%0.3f,%0.3f\n", x, y, z );
}

// --- operátory ------

Vector3 operator-( const Vector3 & v )
{
	return Vector3( -v.x, -v.y, -v.z );
}

Vector3 operator+(const Vector3 & u, const Vector3 & v )
{
	return Vector3( u.x + v.x, u.y + v.y, u.z + v.z );
}

Vector3 operator-( const Vector3 & u, const Vector3 & v )
{
	return Vector3( u.x - v.x, u.y - v.y, u.z - v.z );
}

Vector3 operator*( const Vector3 & v, const float a )
{
	return Vector3( a * v.x, a * v.y, a * v.z );
}

Vector3 operator*( const float a, const Vector3 & v )
{
	return Vector3( a * v.x, a * v.y, a * v.z  ); 		
}

Vector3 operator*( const Vector3 & u, const Vector3 & v )
{
	return Vector3( u.x * v.x, u.y * v.y, u.z * v.z );
}

Vector3 operator/( const Vector3 & v, const float a )
{
	return v * ( 1 / a );
}

Vector3 operator/(const Vector3 &u, const Vector3 &v) {
    return Vector3(u.x / v.x, u.y / v.y, u.z / v.z);
}

void operator+=( Vector3 & u, const Vector3 & v )
{
	u.x += v.x;
	u.y += v.y;	
	u.z += v.z;	
}

void operator-=( Vector3 & u, const Vector3 & v )
{
	u.x -= v.x;
	u.y -= v.y;
	u.z -= v.z;
}

void operator*=( Vector3 & v, const float a )
{
	v.x *= a;
	v.y *= a;
	v.z *= a;
}

void operator/=( Vector3 & v, const float a )
{
	const float r = 1 / a;
    v.x *= r;
    v.y *= r;
    v.z *= r;
}

bool operator==( const Vector3 & u, const Vector3 & v )
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

bool operator!=(const Vector3 &u, const Vector3 &v) {
    return u.x != v.x || u.y != v.y || u.z != v.z;
}

bool operator<( const Vector3 & u, const Vector3 & v ){
    return u.x < v.x && u.y < v.y && u.z < v.z;
}

bool operator>( const Vector3 & u, const Vector3 & v ){
    return u.x > v.x && u.y > v.y && u.z > v.z;
}


Vector3 Vector3::Reflect(Vector3 normal, bool to_hit_point) const {
    Vector3 vec = *this;
    if (to_hit_point) {
        vec = -vec;
    }
    return 2 * vec.DotProduct(normal) * normal - vec;
}

bool Vector3::Refract(Vector3 normal, float n1, float n2, Vector3& result) const {
    Vector3 d = (-*this);
    float ratio = n1 / n2;
    float cos_theta = d.DotProduct(normal);
    float in_sqrt = 1 - ratio * ratio * (1 - cos_theta * cos_theta);
    if(in_sqrt < 0){
        return false;
    }
    result = ratio * d - (ratio * cos_theta + sqrt(in_sqrt)) * normal;
    return true;
}