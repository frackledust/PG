#ifndef VECTOR3_H_
#define VECTOR3_H_

#include "structs.h"

/*! \struct Vector3
\brief Trojrozm�rn� (3D) vektor.

Implementace t��slo�kov�ho re�ln�ho vektoru podporuj�c� z�kladn�
matematick� operace.

\note
Vektor se pova�uje za sloupcov�, p�esto�e je v koment���ch pro jednoduchost
uv�d�n jako ��dkov�.

\code{.cpp}
Vector3 v = Vector3( 2.0f, 4.5f, 7.8f );
v.Normalize();
\endcode

\author Tom� Fabi�n
\version 0.95
\date 2007-2015
*/

struct /*ALIGN*/ Vector3
{
public:
    float x; /*!< Prvn� slo�ka vektoru. */
    float y; /*!< Druh� slo�ka vektoru. */
    float z; /*!< T�et� slo�ka vektoru. */

	//! V�choz� konstruktor.
	/*!
	Inicializuje v�echny slo�ky vektoru na hodnotu nula,
	\f$\mathbf{v}=\mathbf{0}\f$.
	*/
	Vector3() : x( 0 ), y( 0 ), z( 0 ) { }	

	//! Obecn� konstruktor.
	/*!
	Inicializuje slo�ky vektoru podle zadan�ch hodnot parametr�,
	\f$\mathbf{v}=(x,y,z)\f$.

	\param x prvn� slo�ka vektoru.
	\param y druh� slo�ka vektoru.
	\param z t�et� slo�ka vektoru.
	*/
	Vector3( const float x, const float y, const float z ) : x( x ), y( y ), z( z ) { }

    Vector3(Color3f color) : x(color.r), y(color.g), z(color.b) { }
    Vector3(Color4f color) {
        color.expand();
        x = color.r;
        y = color.g;
        z = color.b;
    }

    Color3f to_color3f() const {
        return Color3f{x, y, z};
    }

    Color4f to_color4f() const {
        return Color4f{x, y, z, 1.0f};
    }

    explicit operator Color3f() const{
        return Color3f{x, y, z};
    }

    explicit operator Color4f() const{
        Color4f tmp =  Color4f{x, y, z, 1.0f};
        return tmp.compress(); // linear rgb to srgb
    }

	//! Konstruktor z pole.
	/*!
	Inicializuje slo�ky vektoru podle zadan�ch hodnot pole,

	\param v ukazatel na prvn� slo�ka vektoru.	
	*/
	Vector3( const float * v );

	//! L2-norma vektoru.
	/*!
	\return x Hodnotu \f$\mathbf{||v||}=\sqrt{x^2+y^2+z^2}\f$.
	*/
	float L2Norm() const;

	//! Druh� mocnina L2-normy vektoru.
	/*!
	\return Hodnotu \f$\mathbf{||v||^2}=x^2+y^2+z^2\f$.
	*/
	float SqrL2Norm() const;

	//! Normalizace vektoru.
	/*!
	Po proveden� operace bude m�t vektor jednotkovou d�lku.
	*/
	void Normalize();

	//! Vektorov� sou�in.
	/*!
	\param v vektor \f$\mathbf{v}\f$.

	\return Vektor \f$(\mathbf{u}_x \mathbf{v}_z - \mathbf{u}_z \mathbf{v}_y,
	\mathbf{u}_z \mathbf{v}_x - \mathbf{u}_x \mathbf{v}_z,
	\mathbf{u}_x \mathbf{v}_y - \mathbf{u}_y \mathbf{v}_x)\f$.
	*/
	Vector3 CrossProduct( const Vector3 & v ) const;	

	Vector3 Abs() const;

	Vector3 Max( const float a = 0 ) const;

	//! Skal�rn� sou�in.
	/*!		
	\return Hodnotu \f$\mathbf{u}_x \mathbf{v}_x + \mathbf{u}_y \mathbf{v}_y + \mathbf{u}_z \mathbf{v}_z)\f$.
	*/
	float DotProduct( const Vector3 & v ) const;	

	//! Index nejv�t�� slo�ky vektoru.
	/*!
	\param absolute_value index bude ur�en podle absolutn� hodnoty slo�ky

	\return Index nejv�t�� slo�ky vektoru.
	*/
	char LargestComponent( const bool absolute_value = false );	

	void Print();

	// --- oper�tory ------

	friend Vector3 operator-( const Vector3 & v );

	friend Vector3 operator+(const Vector3 & u, const Vector3 & v );
	friend Vector3 operator-( const Vector3 & u, const Vector3 & v );

	friend Vector3 operator*( const Vector3 & v, const float a );	
	friend Vector3 operator*( const float a, const Vector3 & v );
	friend Vector3 operator*( const Vector3 & u, const Vector3 & v );

	friend Vector3 operator/( const Vector3 & v, const float a );
    friend Vector3 operator/( const Vector3 & u, const Vector3 & v );

	friend void operator+=( Vector3 & u, const Vector3 & v );
	friend void operator-=( Vector3 & u, const Vector3 & v );
	friend void operator*=( Vector3 & v, const float a );
	friend void operator/=( Vector3 & v, const float a );		

	friend bool operator==( const Vector3 & u, const Vector3 & v );

    float &operator[](int i) {
        if(i == 0) return x;
        if(i == 1) return y;
        if(i == 2) return z;
        return x;
    }

    Vector3 Reflect(Vector3 normal, bool to_hit_point = false) const;

    bool Refract(Vector3 normal, float n1, float n2, Vector3 &result) const;

    float LargestComponentValue() const;
};

using Vertex3f = Vector3;
using Normal3f = Vector3;
#endif
