#include "stdafx.h"
#include "material.h"

const char Material::kDiffuseMapSlot = 0;
const char Material::kSpecularMapSlot = 1;
const char Material::kNormalMapSlot = 2;
const char Material::kOpacityMapSlot = 3;

Material::Material()
{
	// defaultní materiál
	ambient = Vector3( 0.1f, 0.1f, 0.1f );
	diffuse = Vector3( 0.4f, 0.4f, 0.4f );
	specular = Vector3( 0.8f, 0.8f, 0.8f );	

	emission = Vector3( 0.0f, 0.0f, 0.0f );	

	reflectivity = static_cast<float>( 0.99 );
	shininess = 1;

	ior = -1;

	memset( textures_, 0, sizeof( *textures_ ) * NO_TEXTURES );

	name_ = "default";
}

Material::Material( std::string & name, const Vector3 & ambient, const Vector3 & diffuse,
	const Vector3 & specular, const Vector3 & emission, const float reflectivity, 
	const float shininess, const float ior, Texture ** textures, const int no_textures )
{
	name_ = name;

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;

	this->emission = emission;

	this->reflectivity = reflectivity;
	this->shininess = shininess;	

	this->ior = ior;

	if ( textures )
	{
		memcpy( textures_, textures, sizeof( textures ) * no_textures );
	}
}

Material::~Material()
{
	for ( int i = 0; i < NO_TEXTURES; ++i )
	{
		if ( textures_[i] )
		{
			delete[] textures_[i];
			textures_[i] = nullptr;
		};
	}
}

void Material::set_name( const char * name )
{
	name_ = std::string( name );
}

std::string Material::get_name() const
{
	return name_;
}

void Material::set_texture( const int slot, Texture * texture )
{
	textures_[slot] = texture;
}

Texture * Material::get_texture( const int slot ) const
{
	return textures_[slot];
}

Color3f Material::get_diffuse_color(const Coord2f &tex_coord) const {
    Color3f diffuse_color = Color3f(diffuse);
    Texture * diffuse_texture = get_texture(kDiffuseMapSlot);
    if(diffuse_texture){
        Color3f diffuse_texel = diffuse_texture->get_texel(tex_coord.u, 1 - tex_coord.v);
        diffuse_color = diffuse_texel;
    }
    return diffuse_color;
}

Color3f Material::get_specular_color(const Coord2f &tex_coord) const {
    Color3f specular_color = Color3f(specular);
    Texture * specular_texture = get_texture(kSpecularMapSlot);
    if(specular_texture){
        Color3f specular_texel = specular_texture->get_texel(tex_coord.u, 1 - tex_coord.v);
        specular_color = specular_texel;
    }
    return specular_color;
}
