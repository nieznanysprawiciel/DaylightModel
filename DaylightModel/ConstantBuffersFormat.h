#pragma once
/**@file ConstantBuffersFormat.h
@brief Plik zawiera deklaracje buforów sta³ych dla vertex i piksel shadera.*/

#include "stdafx.h"
#include "meshes_textures_materials.h"

#define ENGINE_MAX_LIGHTS 2

using namespace DirectX;

/** \brief Bufor sta³ych dla shadera zmieniaj¹cy siê raz na klatkê. 

Bufory sta³ych dla shaderów musz¹ mieæ wielkoœæ bêd¹c¹ wielokrotnoœci¹ 16.
Wszystkie dane w GPU s¹ przechowywane w postaci wektorów 4-wymiarowych.
Dlatego lepiej nie wysy³aæ danych o innej liczbie wymiarów, bo na pewno coœ 
siê zepsuje.

Ewentualnie je¿eli chcemy usprawniæ dostêp do danych po strnie CPU i podawaæ
wektory 3-wymiarowe, to trzeba uzupe³niæ czwarta wspó³rzêdn¹ jakimœ pustym floatem.*/

typedef struct ConstantPerFrame
{
	DirectX::XMFLOAT4X4		view_matrix;
	DirectX::XMFLOAT4X4		projection_matrix;
	DirectX::XMFLOAT4		light_direction[ENGINE_MAX_LIGHTS];		// Wspó³rzêdne s¹ zanegowane, ¿eby shader mia³ mniej roboty
	DirectX::XMFLOAT4		light_color[ENGINE_MAX_LIGHTS];
	DirectX::XMFLOAT4		ambient_light;
	float					time;
	float					time_lag;
} ConstantPerFrame;


/** \brief Bufor sta³ych dla shadera zmieniaj¹cy siê dla ka¿dej czêœci mesha.

Obiekt zawiera macierz przekszta³cenia œwiata dla danej czêœci obiektu oraz dane o materia³ach.
Materia³y s¹ wektorami 4-wymiarowymi tylko ze wzglêdu na to, ¿e takie s¹ rejestry na GPU.
U¿ywane s¹ tylko pierwsze 3 elementy wektora. Jedynie w kanale Diffuse ostatnia wspó³rzêdna odpowiada za przezroczystoœæ.
*/
typedef struct ConstantPerMesh
{
	DirectX::XMMATRIX		world_matrix;
	DirectX::XMVECTOR		mesh_scale;
	DirectX::XMFLOAT4		Diffuse;		//Sk³adowa przezroczystoœci odnosi siê do ca³ego materia³u
	DirectX::XMFLOAT4		Ambient;
	DirectX::XMFLOAT4		Specular;
	DirectX::XMFLOAT4		Emissive;
	float					Power;
} ConstantPerMesh;
