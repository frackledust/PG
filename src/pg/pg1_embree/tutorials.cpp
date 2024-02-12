#include "stdafx.h"
#include "tutorials.h"
#include "raytracer.h"
#include "mymath.h"
#include "pathtracer.h"

/* error reporting function */
void error_handler( void * user_ptr, const RTCError code, const char * str )
{
	if ( code != RTC_ERROR_NONE )
	{
		std::string descr = str ? ": " + std::string( str ) : "";

		switch ( code )
		{
		case RTC_ERROR_UNKNOWN: throw std::runtime_error( "RTC_ERROR_UNKNOWN" + descr );
		case RTC_ERROR_INVALID_ARGUMENT: throw std::runtime_error( "RTC_ERROR_INVALID_ARGUMENT" + descr ); break;
		case RTC_ERROR_INVALID_OPERATION: throw std::runtime_error( "RTC_ERROR_INVALID_OPERATION" + descr ); break;
		case RTC_ERROR_OUT_OF_MEMORY: throw std::runtime_error( "RTC_ERROR_OUT_OF_MEMORY" + descr ); break;
		case RTC_ERROR_UNSUPPORTED_CPU: throw std::runtime_error( "RTC_ERROR_UNSUPPORTED_CPU" + descr ); break;
		case RTC_ERROR_CANCELLED: throw std::runtime_error( "RTC_ERROR_CANCELLED" + descr ); break;
		default: throw std::runtime_error( "invalid error code" + descr ); break;
		}
	}
}

/* raytracer mainloop */
int tutorial_3( const std::string& file_name, const char * config )
{

    /* Render a lego spaceship */
//	Raytracer raytracer( 640, 480, deg2rad( 45.0 ),
//		Vector3( 150, -110, 130 ), Vector3( 0, 0, 35 ), config );
//	raytracer.LoadScene( "data/6887_allied_avenger.obj");

    /* Render a sphere model */
//    Raytracer raytracer( 640, 480, deg2rad( 45.0 ),
//                         Vector3( 3, -3, 1 ), Vector3( 0, 0, 0 ), config );
//    raytracer.LoadScene("data/geosphere.obj");

    /* Render objects using path tracing */
//    Pathtracer raytracer( 640, 480, deg2rad( 45.0 ),
//                         Vector3( 42.0108, -721.186, 250), Vector3( 0, 0, 250 ), config );

//    Pathtracer raytracer( 320, 240, deg2rad( 40.0 ), Vector3( 40, -940, 250 ), Vector3( 0, 0, 250 ) );
//    raytracer.LoadScene( "data/cornell_box2/cornell_box2.obj");

    Pathtracer raytracer( 320, 240, deg2rad( 45.0 ),
                         Vector3( 1.5, -1.5, 1 ), Vector3( 0, 0, 0 ), config );
    raytracer.LoadScene("data/geosphere_white.obj");

    raytracer.LoadBackground();
	raytracer.MainLoop();

	return EXIT_SUCCESS;
}
