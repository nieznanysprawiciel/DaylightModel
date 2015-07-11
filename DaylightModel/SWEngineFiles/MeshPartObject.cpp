#include "stdafx.h"
#include "meshes_textures_materials.h"

//#include "..\..\memory_leaks.h"

#ifndef __UNUSED

//----------------------------------------------------------------------------------------------//
//								contructors, destructors										//
//----------------------------------------------------------------------------------------------//

MeshObject::MeshObject(int id)
	: referenced_object(id)
{
	vertex_buffer = nullptr;
}


MeshObject::~MeshObject()
{
	if (vertex_buffer != nullptr)
		vertex_buffer->Release();
}

/*bool MeshObject::operator==(const Vertex_Normal_TexCords1& vertex)
{
	return FALSE;
}*/

bool VertexFormat1::operator== (VertexFormat1& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return FALSE;
	if (normal.x != vertex2.normal.x
		|| normal.y != vertex2.normal.y
		|| normal.z != vertex2.normal.z)
		return FALSE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return FALSE;

	return TRUE;
}

bool VertexFormat1::operator!=(VertexFormat1& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return TRUE;
	if (normal.x != vertex2.normal.x
		|| normal.y != vertex2.normal.y
		|| normal.z != vertex2.normal.z)
		return TRUE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return TRUE;

	return FALSE;
}


bool VertexFormat2::operator==(VertexFormat2& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return FALSE;
	if (normal.x != vertex2.normal.x
		|| normal.y != vertex2.normal.y
		|| normal.z != vertex2.normal.z)
		return FALSE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return FALSE;
	if (tex_cords2.x != vertex2.tex_cords2.x
		|| tex_cords2.y != vertex2.tex_cords2.y)
		return FALSE;

	return TRUE;
}


bool VertexFormat2::operator!=(VertexFormat2& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return TRUE;
	if (normal.x != vertex2.normal.x
		|| normal.y != vertex2.normal.y
		|| normal.z != vertex2.normal.z)
		return TRUE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return TRUE;
	if (tex_cords2.x != vertex2.tex_cords2.x
		|| tex_cords2.y != vertex2.tex_cords2.y)
		return TRUE;

	return FALSE;
}

bool VertexFormat3::operator==(VertexFormat3& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return FALSE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return FALSE;

	return TRUE;
}

bool VertexFormat3::operator!=(VertexFormat3& vertex2)
{
	if (position.x != vertex2.position.x
		|| position.y != vertex2.position.y
		|| position.z != vertex2.position.z)
		return TRUE;
	if (tex_cords1.x != vertex2.tex_cords1.x
		|| tex_cords1.y != vertex2.tex_cords1.y)
		return TRUE;

	return FALSE;
}


#endif