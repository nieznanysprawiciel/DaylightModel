#include "..\stdafx.h"
#include "HosekSkyDome.h"


using namespace DirectX;

//#include "..\..\..\memory_leaks.h"

/**@brief Opis wierzcho�ka SkyDomeVertex.*/
D3D11_INPUT_ELEMENT_DESC SkyDomeVertex_desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};



HosekSkyDome::HosekSkyDome( ModelsManager* man )
: SkyDome( man )
{
	back_ind_buffer = nullptr;
	back_vert_buffer = nullptr;
}


HosekSkyDome::~HosekSkyDome()
{
	if ( back_ind_buffer )
		delete[] back_ind_buffer;
	if ( back_vert_buffer )
		delete[] back_vert_buffer;
}

/**@brief Inicjuje kopu�� nieba i wylicza kolory dla poszczeg�lnych wierzcho�k�w.

W tej funkcji jest tworzony mesh kopu�y, nast�pnie tworzone s� bufory indeks�w i wierzcho�k�w
oraz layout struktury wierzcho�ka. Kompilowane i dodawane s� r�wnie� shadery.

Zalecam u�ywanie nieaprzystej liczby wierzcho�k�w w pionie. Wtedy utworzy si� r�wnik
na kopule nieba, dzi�ki czemu niebo b�dzie lepiej wygl�da�o. Warto�ci kolor�w obliczane s�
dla wierzcho�k�w, a pomi�dzy nimi s� interpolowane przez rasterizer. Poni�ej r�wnika
brany jest jeden z kolor�w wygenerowanych powy�ej i ustawiany dla ca�ej kopu�y.

@note Raz utworzony mesh kopu�y ju� nigdy si� nie zmienia. Ta funkcja powinna zosta�
wywo�ana tylko raz na pocz�tku, potem nale�y wywo�ywa� funkcj� update_sky_dome.

@attention Ta funkcja musi zosta� wykonana przed dodaniem obiektu do DisplayEngine.

@param[in] sun_direction Wektor w kierunku s�o�ca. Zostanie znormalizowany w funkcji.
@param[in] turbidity M�tno�� atmosfery.
@param[in] albedo Tablica 3-elementowa warto�ci albedo dla poszczeg�lnych kana��w RGB.
@param[in] vertical_vert Liczba segment�w kopu�y nieba w pionie.
@param[in] horizontal_vert Liczba segment�w kopu�y nieba w poziomie.
@param[in] radius Promie� kuli kopu�y nieba.
@param[in] sky_intensity Mno�nik jasno�ci nieba, je�eli nie pda si� nic, warto�� domy�lna wynosi 1.0.
@param[in] sun_intensity Mno�nik jasno�ci tarczy s�onecznej. Domy�lnie jest ustawiane na 1.0.
*/
void HosekSkyDome::init_sky_dome( XMVECTOR sun_direction,
								  double turbidity,
								  const double* albedo,
								  int vertical_vert,
								  int horizontal_vert,
								  float radius,
								  float sky_intensity,
								  float sun_intensity )
{
	// Generujemy kopu��
	generate_sphere( vertical_vert, horizontal_vert, radius );

	// Zak�adamy, �e tablice jeszcze nie istania�y. Obliczamy rozmiary tablic ( w liczbie element�w )
	int vert_buff_elements = (vertical_vert - 2) * horizontal_vert + 2;		// W pionie b�dzie vertical-2 pas�w wierzcho�k�w + musimy dopisa� dwa wierzcho�ki skrajne
	int ind_buff_elements = 2 * 3 * (vertical_vert - 2) * horizontal_vert;	// Liczba pas�w czworok�t�w, razy dwa (�eby zrobi� tr�j�ty) razy 3, �eby przeliczy� na liczb� wierzcho�k�w

	set_index_buffer( L"HosekWilkieModel_index_buffer", back_ind_buffer, sizeof(VERT_INDEX), ind_buff_elements );
	set_vertex_buffer( L"HosekWilkieModel_vertex_buffer", back_vert_buffer, sizeof(SkyDomeVertex), vert_buff_elements );
	display_data.mesh->use_index_buf = true;
	display_data.mesh->buffer_offset = 0;
	display_data.mesh->base_vertex = 0;
	display_data.mesh->vertices_count = ind_buff_elements;

	// Kompilujemy shadery, jednocze�nie dodaje si� layout
	set_vertex_shader( L"shaders\\HosekSkyDome.fx", DEFAULT_VERTEX_SHADER_ENTRY, SkyDomeVertex_desc, ARRAYSIZE( SkyDomeVertex_desc ));
	set_pixel_shader( L"shaders\\HosekSkyDome.fx", DEFAULT_PIXEL_SHADER_ENTRY );

	// Materia� jest niepotrzebny, ale nie mo�e go nie by�
	MaterialObject material;
	material.set_null_material();
	set_material( &material, DEFAULT_MATERIAL_STRING );

	// Tutaj wype�niamy kopu�� kolorem
	update_sky_dome( sun_direction, turbidity, albedo, sky_intensity, sun_intensity );
}

/**@brief Funkcja ta s�u�y do przeliczenia na nowo danych o kolorach.

Kolory s� przeliczane i zapisywane do bufora tylnego, a nast�pnie po zako�czeniu generowania
uaktualniany jest bufor wierzcho�k�w i indeks�w. Funkcja jest jest przygotowana do wykonywania
w innym watku, ale nie powinno si� wykonywa� wielu instancji tej funkcji jednocze�nie, bo nie jest to 
bezpieczne.

@note Funkcja nie modyfikuje �adnych danych klasy SkyDome, jedynie aktualizuje bufor kolor�w
nowymi warto�ciami. W szczeg�lno�ci nie jest modyfimkowana struktura wierzcho�k�w.

@param[in] sun_direction Wektor w kierunku s�o�ca.
@param[in] turbidity M�tno�� atmosfery.
@param[in] albedo Tablica 3-elementowa warto�ci albedo dla poszczeg�lnych kana��w RGB.
@param[in] sky_intensity Mno�nik jasno�ci nieba, je�eli nie pda si� nic, warto�� domy�lna wynosi 1.0.
@param[in] sun_intensity Mno�nik jasno�ci tarczy s�onecznej. Domy�lnie jest ustawiane na 1.0.*/
void HosekSkyDome::update_sky_dome( XMVECTOR sun_direction,
									double turbidity,
									const double* albedo,
									float sky_intensity,
									float sun_intensity)
{
	// Obliczamy wysoko�� s�o�ca
	XMVECTOR zenith = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	sun_direction = XMVector3Normalize( sun_direction );
	XMVECTOR solar_elevation = XMVector3AngleBetweenNormals( sun_direction, zenith );
	float elevation = DirectX::XM_PIDIV2 - XMVectorGetX( solar_elevation );

	skylight_model.init( turbidity, albedo, elevation, sky_intensity, sun_intensity );

	for ( unsigned int i = 0; i < vert_count; ++i )
	{
		// Iterujemy po wszyskich wierzcho�kach w buforze
		XMVECTOR view_direction = XMLoadFloat3( &back_vert_buffer[i].position );
		view_direction = XMVector3Normalize( view_direction );
		XMVECTOR angle = XMVector3AngleBetweenNormals( zenith, view_direction );
		float theta = XMVectorGetX( angle );
		
		if ( theta > XM_PIDIV2 )
		{	// Je�eli jestesmy poni�ej horyzontu to na razie malujemy na czerwono
			back_vert_buffer[i].color = back_vert_buffer[vert_count/2 - 2].color;
			continue;
		}

		angle = XMVector3AngleBetweenNormals( sun_direction, view_direction );
		float gamma = XMVectorGetX( angle );
		// Pobieramy kolor dla danego wierzcho�ka
		XMVECTOR color = skylight_model.sky_radiance( theta, gamma );
		// Zapisujemy kolor
		XMStoreFloat3( &back_vert_buffer[i].color, color );

		//back_vert_buffer[i].color = XMFLOAT3( 1.0f, 0.0f, 0.0f );	// Test
	}

	update_vertex_buffer = true;		// DisplayEngine zaktualizuje bufor
}

/**@brief Funkcja generuje w back_vert_buffer i back_ind_buffer sfer� dla kpu�y nieba.
Jest to sfera z punktami wyr�nionymi na g�rze i dole. (nie jest to geosfera)

@param[in] vertical Liczba wierzcho�k�w w pionie.
@param[in] horizontal Liczba wierzcho�k�w w poziomie.
@param[in] radius Promie� kopu�y nieba.*/
void HosekSkyDome::generate_sphere( int vertical, int horizontal, float radius )
{
	// Zak�adamy, �e tablice jeszcze nie istania�y. Obliczamy ile potrzeba zaalokowa�
	int vert_buff_elements = (vertical - 2) * horizontal + 2;	// W pionie b�dzie vertical-2 pas�w wierzcho�k�w + musimy dopisa� dwa wierzcho�ki skrajne
	int ind_buff_elements = 2 * 3 * (vertical - 2) * horizontal;	// Liczba pas�w czworok�t�w, razy dwa (�eby zrobi� tr�j�ty) razy 3, �eby przeliczy� na liczb� wierzcho�k�w
	vert_count = vert_buff_elements;		// Zapisujemy warto�� na sta��
	
	back_vert_buffer = new SkyDomeVertex[vert_buff_elements];
	back_ind_buffer = new VERT_INDEX[ind_buff_elements];

	float vert_angle = DirectX::XM_PI / (float)horizontal;	// Obliczamy przesuni�cie k�towe w pionie
	float hor_angle = DirectX::XM_2PI / (float)vertical;	// Obliczamy przesuni�cie k�towe w poziomie


	// Zaczynamy od wierzcho�ka g�rnego i idziemy w d�
	back_vert_buffer[0] = { XMFLOAT3( 0.0, radius, 0.0 ), XMFLOAT3( 0.0, 0.0, 0.0 ) };

	unsigned int cur_ptr = 1;
	for ( int i = 1; i < vertical - 1; ++i )
	{
		float beta = vert_angle * i;		// K�t w p�aszczy�nie poziomej
		float r = radius * sin( beta );		// Promie� zrzutowany na p�aszczyzn� poziom�
		float y = radius * cos( beta );		// Wsp�rz�dna y wektora

		for ( int j = 0; j < horizontal; ++j )
		{
			float alfa = hor_angle * j;		// K�t w p�aszczy�nie pionowej

			float x = r * cos( alfa );
			float z = r * sin( alfa );

			back_vert_buffer[cur_ptr++] = { XMFLOAT3(x, y, z), XMFLOAT3( 0.0, 0.0, 0.0 ) };
		}
	}
	// Dolny wierzcho�ek
	back_vert_buffer[vert_buff_elements - 1] = { XMFLOAT3( 0.0, -radius, 0.0 ), XMFLOAT3( 0.0, 0.0, 0.0 ) };

	// Mamy ju� wszystkie wierzcho�ki, teraz musimy je po��czy�. Wype�niamy bufor indeks�w.
	cur_ptr = 0;		// Wskazuje na miejsce w buforze indeks�w do wype�nienia
	int i = 1;			// Wskazuje na wierzcho�ek z ni�szej warstwy
	for ( ; i < horizontal; ++i )	// Odliczamy horizontal wierzcho�k�w od nr 1
	{
		// Na razie wype�niamy tylko g�rn� wartw� z punktem centralnym
		back_ind_buffer[cur_ptr++] = 0;
		back_ind_buffer[cur_ptr++] = i;
		back_ind_buffer[cur_ptr++] = i + 1 ;
	}
	// Niestety trzeba rozwa�y� osobno
	back_ind_buffer[cur_ptr++] = 0;
	back_ind_buffer[cur_ptr++] = horizontal;
	back_ind_buffer[cur_ptr++] = 1;

	for ( int k = 1; k < vertical - 2; ++k )
	{
		for ( int h = 0; h < horizontal - 1; ++h )
		{
			int j = (k - 1) * horizontal + h + 1;	// Wskazuje na wierzcho�ek z wy�szej warstwy
			i = j + horizontal;						// Wskazuje na wierzcho�ek z ni�szej warstwy

			// Wype�niamy po dwa tr�jk�ty
			back_ind_buffer[cur_ptr++] = j;
			back_ind_buffer[cur_ptr++] = i;
			back_ind_buffer[cur_ptr++] = i + 1;

			back_ind_buffer[cur_ptr++] = j;
			back_ind_buffer[cur_ptr++] = i + 1;
			back_ind_buffer[cur_ptr++] = j + 1;
		}
		// Ostatnie wierzcho�ki z ka�dego pasa musimy rozwa�y� osobno
		int j = k  * horizontal;	// Wskazuje na wierzcho�ek z wy�szej warstwy

		// To jest miejsce, w kt�rym ostatni wierzcho�ek z pasa ��czy si� z pierwszym, kt�ry zosta� ju� rozwa�ony.
		// Wierzcho�ki s� roz�o�one w pami�ci ci�g�ej, wi�c za ostatnim wierzcho�kiem z g�rnego rz�du jest pierwszy z dolnego.
		back_ind_buffer[cur_ptr++] = j;
		back_ind_buffer[cur_ptr++] = j + horizontal;
		back_ind_buffer[cur_ptr++] = j + 1;

		back_ind_buffer[cur_ptr++] = j;
		back_ind_buffer[cur_ptr++] = j + 1;
		back_ind_buffer[cur_ptr++] = (k-1)*horizontal + 1;
	}

	// Teraz musimy pod��czy� ten pojedynczy wierzcho�ek na dole, do wszystkich z ostatniego pasa
	// Dla odmiany idziemy od ty�u
	for ( int k = vert_buff_elements - 2; k > vert_buff_elements - horizontal - 2; --k )
	{
		back_ind_buffer[cur_ptr++] = vert_buff_elements - 1;
		back_ind_buffer[cur_ptr++] = k;
		back_ind_buffer[cur_ptr++] = k - 1;
	}
}

/**@brief Funkcja aktualizuje bufor wierzcho�k�w zawarto�ci� tylnego bufora.

Aby zaktualizowa� niebo trzeba wywo�a� funkcje update_sky_dome (w jakim� oddzielnym w�tku).
Funkcja ustawi zmienn� update_vertex_buffer po zako�czeniu aktualizacji tylnego bufora.
Nast�pnie DisplayEngine odczyta zmienn� update_vertex_buffer i wywo�a funkcj� update_buffers.

Funkcja wirtualna, przeczytaj opis funkcji SkyDome::update_buffers.*/
void HosekSkyDome::update_buffers( )
{
	device_context->UpdateSubresource( vertex_buffer->get(), 0, nullptr, back_vert_buffer, 0, 0 );
	update_vertex_buffer = false;
}
