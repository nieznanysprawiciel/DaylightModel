#include "GUIManager.h"


const unsigned int IDC_TOGGLEFULLSCREEN = 1;


GUIManager::GUIManager()
{}


GUIManager::~GUIManager()
{}


void GUIManager::initGUI( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext )
{
	DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext );


	HUD.Init( &DialogResourceManager );
	//InitCommonControls();
	//DXUTInit( true, true, L"" );

	HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, 30, 170, 22 );
}

bool GUIManager::HandleMessages( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( HUD.MsgProc( hwnd, uMsg, wParam, lParam ) )
		return true;

	return false;
}


void GUIManager::RenderGUI( float elapsedTime )
{
	HUD.OnRender( elapsedTime );
}

void GUIManager::ReleaseGUI()
{
	DialogResourceManager.OnD3D11DestroyDevice();
}
