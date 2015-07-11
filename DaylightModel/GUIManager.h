#pragma once

#include "DXUT\DXUT.h"
#include "DXUT\DXUTgui.h"
#include "DXUT\DXUTsettingsDlg.h"


class GUIManager
{
private:
	CDXUTDialogResourceManager  DialogResourceManager; // manager for shared resources of dialogs
	CDXUTDialog                 HUD;                  // manages the 3D   
public:
	GUIManager();
	~GUIManager();

	void initGUI( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext );
	bool HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void RenderGUI( float elapsedTime );
	void ReleaseGUI();
};

