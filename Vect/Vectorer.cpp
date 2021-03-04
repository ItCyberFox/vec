// ���������� comctl32.lib comdlg32.lib

#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <commdlg.h>
#include "Resources.h"

HINSTANCE	hInst;														// ����������
HWND		hwndMain,hwndToolBar,hwndStatusBar,hwndPalette,hwndList,hwndHolst;	// ����������� ����
HMENU			hMenu;

char		szAppName[]	= "VectoreR";									// �������� ����������
char		szBuf[256],szCount[10];										// ����� ��� ���������� ������������� � ���. �����
int			status,iCount,i,j;											// ������ �������, ����� ����������,i,j
int			tekI=0,tekJ=0;												// ������� ������
int			NUndo;

COLORREF	crPalCol[14][2];											// ������� ������ �������
COLORREF	crCurUp;
COLORREF	crCurDown;													// ��� ����� - ������� � ������
WNDPROC		OldWndList;													// ����� ������������� ���������
	

HDC			hdcMemPalette,hdcMemHolstBack,hdcMemHolstDyn;				// ��������� ������
HDC			hdcPalette,hdcHolst;										// ��������� �������
HPEN		hSelectPen,hPen,hPenD,hPenL;								// �����
HBRUSH		hSelectBrush,hBrush;										// �����

BOOL		LightOn,IsUpColor,IsName;

/// ��������� ���������� ��������� ���� ///
int PaletteTop,ListTop,HolstTop,HolstLeft,Otstup;

//////////////////////////////// ��������� ������� �������� ///////////////////////////////////////
LRESULT CALLBACK WndMain(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndPalette(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndHolst(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndList(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void RepaintBack(void);
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////// ���������� ������� ///////////////////////////////////////////////
int RisPal(void)
{
	DeleteObject(hBrush);									// ���������� �����
	hBrush = CreateSolidBrush(crCurDown);					// ������� ����� �� ������ ������� �����
	SelectObject(hdcMemPalette,hBrush);						// ������� �����
	SelectObject(hdcMemPalette,GetStockObject(NULL_PEN));	// ������� ������ ����
	Rectangle(hdcMemPalette,13,14,25,26);					// ���������� ������ �������

	SelectObject(hdcMemPalette,hPenD);						// ������� ����
	MoveToEx(hdcMemPalette,18,6,NULL);						// ��������� ����
	LineTo(hdcMemPalette,18,19);							// ������ �����
	LineTo(hdcMemPalette,4,19);								// ������ �����
	SelectObject(hdcMemPalette,GetStockObject(WHITE_PEN));	// ������� ����� ����
	LineTo(hdcMemPalette,4,5);								// ����� �����
	LineTo(hdcMemPalette,19,5);								// ������� �����

	DeleteObject(hBrush);									// ���������� �����
	hBrush	= CreateSolidBrush(crCurUp);					// ������� ����� �� ������ �������� �����
	SelectObject(hdcMemPalette,hPenL);						// ������� ������� ����
	SelectObject(hdcMemPalette,hBrush);						// ������� �����
	Rectangle(hdcMemPalette,5,6,18,19);						// ���������� ������� �������

	for (i=0;i<14;i++)
	for (j=0;j<2;j++)
	{
		DeleteObject(hBrush);									// ������� �����
		hBrush		= CreateSolidBrush(crPalCol[i][j]);			// ������� ����� �����
		SelectObject(hdcMemPalette,GetStockObject(NULL_PEN));	// ������� ���������� ����
		SelectObject(hdcMemPalette,hBrush);						// ������� ������� �����
		Rectangle(hdcMemPalette,i * 16 +33,j * 16 +2,i * 16 + 46,j * 16+15); // ���������� ������
	}

	InvalidateRect(hwndPalette,NULL,FALSE);						// ����������� ��� ��������
	
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////



inline int MinCoordOf2(int c1, int c2)
{
	if (c1<c2) return c1;
	return c2;
}

inline int MaxCoordOf2(int c1, int c2)
{
	if (c1>c2) return c1;
	return c2;
}

inline int HexToInt(char X)
{
	switch (X)
	{
		case '0':return 0;
		case '1':return 1;
		case '2':return 2;
		case '3':return 3;
		case '4':return 4;
		case '5':return 5;
		case '6':return 6;
		case '7':return 7;
		case '8':return 8;
		case '9':return 9;
		case 'A':return 10;
		case 'B':return 11;
		case 'C':return 12;
		case 'D':return 13;
		case 'E':return 14;
		case 'F':return 15;
	}
	return 1;
}

inline int HexSzToInt(char* szCountX)				// + ������� ���������� � �������
{

	int rez=0;
	int	tek;
	for (int n=0;n<6;n++)
	{
		tek=HexToInt(szCountX[n]);
		switch (n)
		{
			case 5:rez=rez+tek;break;
			case 4:rez=rez+tek*16;break;
			case 3:rez=rez+tek*256;	break;
			case 2:rez=rez+tek*4096;break;
			case 1:rez=rez+tek*65536;break;
			case 0:rez=rez+tek*1048576;
		}

	}

return rez;
	
}

int RecountCoord(void)
{
	RECT	rrr;
	
	PaletteTop = 2;
	HolstTop = 2;
	ListTop = 2;
	HolstLeft = 0;
	Otstup = 2;

	if (IsWindowVisible(hwndToolBar))
	{
		PaletteTop = 33;
		HolstTop = 33;
		ListTop = 33;
	}

	if (IsWindowVisible(hwndPalette))
	{
		ListTop = ListTop + 39;
	}

	if ((IsWindowVisible(hwndPalette)) || (IsWindowVisible(hwndList))) HolstLeft = 264;
	else HolstLeft = 3;

	if (IsWindowVisible(hwndStatusBar)) Otstup = 20;
	else Otstup = 2;

	GetClientRect(hwndMain,&rrr);
	SendMessage(hwndMain,WM_SIZE,0,MAKELPARAM(rrr.right,rrr.bottom));
	InvalidateRect(hwndMain,0,TRUE);

	return 0;
}

//////////////////////////////// ������� ��������� ////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	MSG			msg;
	WNDCLASSEX	wndclass;
	hInst = hInstance;

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndMain;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInst;
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon			= LoadIcon(hInst,"IDI_ICON");
	wndclass.hIconSm		= LoadIcon(hInst,"IDI_ICON");
	wndclass.hbrBackground	= (HBRUSH) (COLOR_BTNFACE+1);
	wndclass.lpszMenuName	= "IDR_MENU1";
	wndclass.lpszClassName	= szAppName;
	RegisterClassEx(&wndclass);

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndPalette;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInst;
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon			= LoadIcon(hInst,IDI_APPLICATION);
	wndclass.hIconSm		= LoadIcon(hInst,IDI_APPLICATION);
	wndclass.hbrBackground	= (HBRUSH) (COLOR_BTNFACE+1);
	wndclass.lpszMenuName	= 0;
	wndclass.lpszClassName	= "PaletteClass";
	RegisterClassEx(&wndclass);

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndHolst;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInst;
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon			= LoadIcon(hInst,IDI_APPLICATION);
	wndclass.hIconSm		= LoadIcon(hInst,IDI_APPLICATION);
	wndclass.hbrBackground	= (HBRUSH) (COLOR_BTNFACE);
	wndclass.lpszMenuName	= 0;
	wndclass.lpszClassName	= "HolstClass";
	RegisterClassEx(&wndclass);

	hwndMain = CreateWindow(szAppName,"VectoreR",WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
	
	ShowWindow(hwndMain, nShowCmd);
	UpdateWindow(hwndMain);

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

//////////////////////////////// ������� ��������� �������� ���� //////////////////////////////////
LRESULT CALLBACK WndMain(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// ���������� �������
	DWORD		dwStyle;		//����� �������
	TBADDBITMAP tb;				//������������ �������
	TBBUTTON	tbButton[11];	//������ ������

	// ���������� ����������
	RECT			rect;
	int				ptWidth[3];													// ������� ����� ����������

	// ���������� ���������
	HFONT			hFont;														// ����� ��� ListBox'�

	// ���������� ��������� �������
	static OPENFILENAME ofn;
	static char		szFilter[]	=	"VCT Files (*.vct)\0*.vct\0All Files (*.*)\0*.*\0\0";
	static char		szFullFileName[256] = "\0";
	static char		szFileName[256] = "\0";
	FILE			*fp;

	// ���������� ��������� �������
	static COLORREF	crCustColor[16];											// ������� ������ ��� ���. �������
	static CHOOSECOLOR	cc;														// ��������� ��� ���. �������

	// ������ ����������
	HBITMAP			hBitmap,hBitmapHolstBack,hBitmapHolstDyn;					// ��������
	LPNMHDR			pnmh;
	LPSTR			pReply;
	
	// ������� �������
	switch (iMsg)
	{
		case WM_CREATE:
		InitCommonControls(); // �������������� ���������� ���������

		///////////////////// ������� ������ ////////////////////////
		dwStyle =	WS_CHILD | WS_VISIBLE | WS_DLGFRAME | CCS_TOP | CCS_NORESIZE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
		tb.hInst	=hInst;			//������� ����������
		tb.nID		=2;				//����� ������� � ��������
		tbButton[0].iBitmap			= ID_NEW;
		tbButton[0].idCommand		= IDM_NEW;
		tbButton[0].fsState			= TBSTATE_ENABLED;
		tbButton[0].fsStyle			= TBSTYLE_BUTTON;
		tbButton[0].bReserved[0]	= 0;
		tbButton[0].bReserved[1]	= 0;
		tbButton[0].dwData			= 0;
		tbButton[0].iString			= 0;
		tbButton[1].iBitmap			= ID_OPEN;
		tbButton[1].idCommand		= IDM_OPEN;
		tbButton[1].fsState			= TBSTATE_ENABLED;
		tbButton[1].fsStyle			= TBSTYLE_BUTTON;
		tbButton[1].bReserved[0]	= 0;
		tbButton[1].bReserved[1]	= 0;
		tbButton[1].dwData			= 0;
		tbButton[1].iString			= 0;
		tbButton[2].iBitmap			= ID_SAVE;
		tbButton[2].idCommand		= IDM_SAVE;
		tbButton[2].fsState			= TBSTATE_ENABLED;
		tbButton[2].fsStyle			= TBSTYLE_BUTTON;
		tbButton[2].bReserved[0]	= 0;
		tbButton[2].bReserved[1]	= 0;
		tbButton[2].dwData			= 0;
		tbButton[2].iString			= 0;
		tbButton[3].iBitmap			= ID_INFO;
		tbButton[3].idCommand		= IDM_HELP;
		tbButton[3].fsState			= TBSTATE_ENABLED;
		tbButton[3].fsStyle			= TBSTYLE_BUTTON;
		tbButton[3].bReserved[0]	= 0;
		tbButton[3].bReserved[1]	= 0;
		tbButton[3].dwData			= 0;
		tbButton[3].iString			= 0;
		tbButton[4].iBitmap			= 0;
		tbButton[4].idCommand		= 1;
		tbButton[4].fsState			= TBSTATE_ENABLED;
		tbButton[4].fsStyle			= TBSTYLE_SEP;
		tbButton[4].bReserved[0]	= 0;
		tbButton[4].bReserved[1]	= 0;
		tbButton[4].dwData			= 0;
		tbButton[4].iString			= 0;
		tbButton[5].iBitmap			= ID_RGB;
		tbButton[5].idCommand		= TB_ID_RGB;
		tbButton[5].fsState			= TBSTATE_ENABLED;
		tbButton[5].fsStyle			= TBSTYLE_BUTTON;
		tbButton[5].bReserved[0]	= 0;
		tbButton[5].bReserved[1]	= 0;
		tbButton[5].dwData			= 0;
		tbButton[5].iString			= 0;
		tbButton[6].iBitmap			= 0;
		tbButton[6].idCommand		= 1;
		tbButton[6].fsState			= TBSTATE_ENABLED;
		tbButton[6].fsStyle			= TBSTYLE_SEP;
		tbButton[6].bReserved[0]	= 0;
		tbButton[6].bReserved[1]	= 0;
		tbButton[6].dwData			= 0;
		tbButton[6].iString			= 0;
		tbButton[7].iBitmap			= ID_ARROW;
		tbButton[7].idCommand		= TB_ID_ARROW;
		tbButton[7].fsState			= TBSTATE_ENABLED;
		tbButton[7].fsStyle			= TBSTYLE_BUTTON;
		tbButton[7].bReserved[0]	= 0;
		tbButton[7].bReserved[1]	= 0;
		tbButton[7].dwData			= 0;
		tbButton[7].iString			= 0;
		tbButton[8].iBitmap			= ID_LINE;
		tbButton[8].idCommand		= TB_ID_LINE;
		tbButton[8].fsState			= TBSTATE_ENABLED;
		tbButton[8].fsStyle			= TBSTYLE_BUTTON;
		tbButton[8].bReserved[0]	= 0;
		tbButton[8].bReserved[1]	= 0;
		tbButton[8].dwData			= 0;
		tbButton[8].iString			= 0;
		tbButton[9].iBitmap			= ID_RECTANGLE;
		tbButton[9].idCommand		= TB_ID_RECTANGLE;
		tbButton[9].fsState			= TBSTATE_ENABLED;
		tbButton[9].fsStyle			= TBSTYLE_BUTTON;
		tbButton[9].bReserved[0]	= 0;
		tbButton[9].bReserved[1]	= 0;
		tbButton[9].dwData			= 0;
		tbButton[9].iString			= 0;
		tbButton[10].iBitmap		= ID_CIRCLE;
		tbButton[10].idCommand		= TB_ID_CIRCLE;
		tbButton[10].fsState		= TBSTATE_ENABLED;
		tbButton[10].fsStyle		= TBSTYLE_BUTTON;
		tbButton[10].bReserved[0]	= 0;
		tbButton[10].bReserved[1]	= 0;
		tbButton[10].dwData			= 0;
		tbButton[10].iString		= 0;
		hwndToolBar = CreateWindowEx	(0,TOOLBARCLASSNAME,NULL,dwStyle,0,0,0,0,hWnd,0,hInst,NULL);
		if (hwndToolBar == NULL) return FALSE;
		SendMessage(hwndToolBar,TB_BUTTONSTRUCTSIZE,(WPARAM) sizeof(TBBUTTON),0);
		SendMessage(hwndToolBar, TB_ADDBITMAP, (WPARAM)11, (LPARAM)&tb);
		SendMessage(hwndToolBar, TB_ADDBUTTONS, (WPARAM)11, (LPARAM)&tbButton);
		SendMessage(hwndToolBar, TB_SETBITMAPSIZE, 0, MAKELONG(20, 20));
		SendMessage(hwndToolBar, TB_SETBUTTONSIZE, 0, MAKELONG(20, 20));
		SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_ARROW,TRUE);
		SendMessage(hwndToolBar,TB_ENABLEBUTTON,(WPARAM)IDM_SAVE,FALSE);

		///////////////////// ������� ��������� /////////////////////
		dwStyle		= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_NORESIZE | SBARS_SIZEGRIP;
		hwndStatusBar = CreateWindowEx(0,STATUSCLASSNAME,NULL,dwStyle,0,0,0,0,hWnd,0,hInst,NULL);
		if (hwndStatusBar == NULL) return FALSE;
		SendMessage(hwndStatusBar,SB_SETTEXT,0, (LPARAM)"������");
		GetClientRect(hWnd,&rect);
		ptWidth[0] = rect.right/3;
		ptWidth[1] = 2*rect.right/3;
		ptWidth[2] = -1;
		SendMessage(hwndStatusBar, SB_SETPARTS, 3, (LPARAM)ptWidth);

		///////////////////// ������� �������� //////////////////////
		hwndList = CreateWindowEx(WS_EX_CLIENTEDGE,"listbox","�������",WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 0,0,0,0,hWnd,NULL,hInst,NULL);
		hFont = CreateFont
			(
			16,
			0,
			-900,
			-900,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			"Terminal"
			); 
		SendMessage(hwndList,WM_SETFONT,(WPARAM)hFont,0);
		OldWndList = (WNDPROC) SetWindowLong (hwndList,GWL_WNDPROC, (LONG) WndList);

		///////////////////// ������� ���� ������� ///////////////////
		hwndPalette		= CreateWindowEx(WS_EX_WINDOWEDGE,"PaletteClass", "�������", WS_CHILD | WS_VISIBLE | WS_DLGFRAME, 0,0,0,0,hWnd,NULL,hInst,NULL);
		hBitmap = LoadBitmap(hInst,"IDB_PALETTE");				// �������������� �������� �������
		hdcMemPalette = CreateCompatibleDC(GetDC(hwndPalette));	// ������� ������ �������
		SelectObject(hdcMemPalette, hBitmap);					// ������� � �������� ������ ��������

		////////////////////// ������� ������ //////////////////////////
		hwndHolst	= CreateWindowEx(WS_EX_CLIENTEDGE,"HolstClass", "�����", WS_CHILD | WS_VISIBLE, 0,0,0,0 ,hWnd,NULL,hInst,NULL);
		hdcMemHolstBack = CreateCompatibleDC(GetDC(hwndHolst));					// �������� ������ "�������"
		hdcMemHolstDyn = CreateCompatibleDC(GetDC(hwndHolst));					// �������� ������ "��������������"
		hBitmapHolstBack = CreateCompatibleBitmap(GetDC(hwndHolst),800,600);	// ������� ������ ��������
		hBitmapHolstDyn = CreateCompatibleBitmap(GetDC(hwndHolst),800,600);		// ������� ������ ��������
		SelectObject(hdcMemHolstBack,hBitmapHolstBack);							// ������� � ������� ��������
		SelectObject(hdcMemHolstDyn,hBitmapHolstDyn);							// ������� � �������������� ��������

		SelectObject(hdcMemHolstBack,GetStockObject(WHITE_BRUSH));				 
		SelectObject(hdcMemHolstBack,GetStockObject(BLACK_PEN));				 
		Rectangle(hdcMemHolstBack,0,0,800,600);									// ������� ������� �� ��� ������� �����������

		/////////////////////// ������������� ������ ����-������� ///////
		ofn.lStructSize       = sizeof (OPENFILENAME);	// ������ ���������
		ofn.hwndOwner         = hWnd;					// ��������
		ofn.hInstance         = NULL;					// ����������
		ofn.lpstrFilter       = szFilter;				// ������� �������
		ofn.lpstrCustomFilter = NULL;					// ������������ ������� �������
		ofn.nMaxCustFilter    = 0;						// ������ ������� ��������
		ofn.nFilterIndex      = 1;						// ������ ������ ������
		ofn.lpstrFile         = szFullFileName;         // ����� ������� �����
		ofn.nMaxFile          = 256;					// ������ ����� ������
		ofn.lpstrFileTitle    = szFileName;				// ����� �����
		ofn.nMaxFileTitle     = 256;					// ������ ������
		ofn.lpstrInitialDir   = NULL;					// ��������� ����� - �������
		ofn.lpstrTitle        = NULL;					// ������� �� ���� - �������
		ofn.Flags             = OFN_HIDEREADONLY;		// ������ ������ "������ ��� ������"
		ofn.nFileOffset       = 0;						
		ofn.nFileExtension    = 0;
		ofn.lpstrDefExt       = "vct";					// ������� ����������
		ofn.lCustData         = 0L;
		ofn.lpfnHook          = NULL;
		ofn.lpTemplateName    = NULL;
		IsName				  = FALSE;

		/////////////////////// ������������� ������ ������� ������� ////
		cc.lStructSize		= sizeof(CHOOSECOLOR);
		cc.hwndOwner		= hWnd;
		cc.hInstance		= NULL;
		cc.rgbResult		= RGB(0x80, 0x80, 0x80);
		cc.lpCustColors		= crCustColor;
		cc.Flags			= CC_RGBINIT | CC_FULLOPEN;
		cc.lCustData		= NULL;
		cc.lpfnHook			= NULL;
		cc.lpTemplateName	= NULL;

		////////////////////// ������������� ������ ������� /////////////
		crPalCol[0][0]=RGB(0,0,0);
		crPalCol[1][0]=RGB(128,128,128);
		crPalCol[2][0]=RGB(128,0,0);
		crPalCol[3][0]=RGB(128,128,0);
		crPalCol[4][0]=RGB(0,128,0);
		crPalCol[5][0]=RGB(0,128,128);
		crPalCol[6][0]=RGB(0,0,128);
		crPalCol[7][0]=RGB(128,0,128);
		crPalCol[8][0]=RGB(128,128,64);
		crPalCol[9][0]=RGB(0,0,64);
		crPalCol[10][0]=RGB(0,128,255);
		crPalCol[11][0]=RGB(0,64,128);
		crPalCol[12][0]=RGB(128,0,255);
		crPalCol[13][0]=RGB(128,64,0);
		crPalCol[0][1]=RGB(255,255,255);
		crPalCol[1][1]=RGB(192,192,192);
		crPalCol[2][1]=RGB(255,0,0);
		crPalCol[3][1]=RGB(255,255,0);
		crPalCol[4][1]=RGB(0,255,0);
		crPalCol[5][1]=RGB(0,255,255);
		crPalCol[6][1]=RGB(0,0,255);
		crPalCol[7][1]=RGB(255,0,255);
		crPalCol[8][1]=RGB(255,255,128);
		crPalCol[9][1]=RGB(0,255,128);
		crPalCol[10][1]=RGB(128,255,255);
		crPalCol[11][1]=RGB(128,128,255);
		crPalCol[12][1]=RGB(255,0,128);
		crPalCol[13][1]=RGB(255,128,64);
		crCurUp = crPalCol[0][0];
		crCurDown = crPalCol[0][1];
		sprintf(szBuf, "%s %d %s %d %s %d", "R= ", crCurUp<<24>>24, " G= ", crCurUp<<16>>24, " B=",crCurUp<<8>>24);
		SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM)szBuf);
		IsUpColor = TRUE; // ������� ����� ����

		/////////////////////// �������� �������� GDI //////////////////////
		hPen			= CreatePen(PS_SOLID,0,0);
		hPenL			= CreatePen(PS_SOLID,0,RGB(236,233,216));	// ������� ������� ����������� ����
		hPenD			= CreatePen(PS_SOLID,0,RGB(172,168,153));	// ������� ������ ����������� ����
		hSelectPen		= CreatePen(PS_SOLID,0,crCurUp);			// ������� ����
		hBrush			= CreateSolidBrush(0);
		hSelectBrush	= CreateSolidBrush(crCurDown);				// ������� �����

		/////////////////////// ������ /////////////////////////////////////
		SetTimer(hwndHolst,1,500,NULL);
		LightOn = TRUE;

		/////////////////////// ������� � ���������� ///////////////////////
		sprintf(szBuf, "%s %06X %s %06X", "������� RGB = ", crCurUp, "  ������ RGB = ", crCurDown);
		SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)szBuf);

		/////////////////////// ������� � ������� ���� /////////////////////
		SetWindowText(hWnd, "����� ��������");

		/////////////////////// ��������� ���������� ��������� ���� ////////
		PaletteTop = 33;
		ListTop = 72;
		HolstTop = 33;
		HolstLeft = 264;
		Otstup = 20;

		// ���� "return 0" ����� ���������� ��������� ������ � WM_ZIZE ��� ��������������� ���������������� ����

		case WM_SIZE:
		GetClientRect(hWnd,&rect);
		MoveWindow(hwndPalette,2,PaletteTop,260,37,FALSE);
		MoveWindow(hwndList,2,ListTop,260,rect.bottom-ListTop-Otstup,FALSE);
		MoveWindow(hwndHolst,HolstLeft,HolstTop,rect.right-2-HolstLeft,rect.bottom-HolstTop-Otstup,FALSE);
		MoveWindow(hwndToolBar,-2,-3,rect.right+4,34,FALSE);
		MoveWindow(hwndStatusBar, 0, rect.bottom-20, rect.right, 20, FALSE);
		ptWidth[0] = rect.right/3;
		ptWidth[1] = 2*rect.right/3;
		ptWidth[2] = -1;
		SendMessage(hwndStatusBar, SB_SETPARTS, 3, (LPARAM)ptWidth);
		return 0;

		case WM_COMMAND:
		hMenu = GetMenu(hWnd);
		switch (LOWORD(wParam))
		{
			case IDM_NEW:
			for (i = 0; i<iCount; i++) SendMessage(hwndList,LB_DELETESTRING,iCount-1-i,0);
			iCount = 0;
			RepaintBack();
			SetWindowText(hWnd, "����� ��������");
			InvalidateRect(hwndHolst,NULL,FALSE);
			EnableMenuItem(hMenu,IDM_SAVE,MF_GRAYED);
			SendMessage(hwndToolBar,TB_ENABLEBUTTON,(WPARAM)IDM_SAVE,FALSE);
			return 0;

			case IDM_HELP:
			MessageBox(hWnd,"���������� ��������� �������.\n\n"
							"����������� 4 �����������:\n"
							"<���������> - �������� �������\n"
							"<�����> - �������� �������\n"
							"<�������������> - �������� ��������������\n"
							"<����������> - �������� ����������\n\n"
							"������������� � ���������� �������������\n"
							"������ ������. ������ ���� ����� ��������\n"
							"������� ������. ���������� ����� ����� �\n"
							"������ ������� ����. ������������ �������\n"
							"��������� ������� Delete ��� �� ����.",szAppName,MB_ICONINFORMATION | MB_OK);
			return 0;

			case IDM_ABOUT:
			MessageBox(hWnd,"Ox1D Inc.\nFree Code Systems...",szAppName,MB_ICONINFORMATION | MB_OK);
			return 0;

			case IDM_OPEN:
			if (GetOpenFileName (&ofn))
			{
				for (i = 0; i<iCount; i++) SendMessage(hwndList,LB_DELETESTRING,iCount-1-i,0);
				fp = fopen(szFullFileName,"r");
				while (!feof(fp)) // ������� �����
				{
					if(fgets(szBuf,42,fp))
					{
						szBuf[42]='\0';
						SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM) szBuf);
					}
				}
				fclose(fp);

				RepaintBack();
				InvalidateRect(hwndHolst,NULL,FALSE);

				SetWindowText(hWnd, szFullFileName);
				IsName = TRUE;
				EnableMenuItem(hMenu,IDM_SAVE,MF_ENABLED);
				SendMessage(hwndToolBar,TB_ENABLEBUTTON,(WPARAM)IDM_SAVE,TRUE);
			}
			return 0;

			case IDM_SAVE:
			fp = fopen(szFullFileName,"w");
			for (i = 0; i<iCount; i++) // ������� �����
			{
				SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);
				fputs(szBuf,fp);
			}
			fclose(fp);
			return 0;

			case IDM_SAVE_AS:
			if (GetSaveFileName (&ofn))
			{
				fp = fopen(szFullFileName,"w");
				for (i = 0; i<iCount; i++) // ������� �����
				{
					SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);
					fputs(szBuf,fp);
				}
				fclose(fp);

				SetWindowText(hWnd, szFullFileName);
				IsName = TRUE;
				EnableMenuItem(hMenu,IDM_SAVE,MF_ENABLED);
				SendMessage(hwndToolBar,TB_ENABLEBUTTON,(WPARAM)IDM_SAVE,TRUE);
			}
			return 0;

			case IDM_EXIT:
			PostQuitMessage(0);
			return 0;

			case IDM_DEL:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			SendMessage(hwndList,LB_DELETESTRING,i,0);
			RepaintBack();
			InvalidateRect(hwndHolst,NULL,FALSE);
			return 0;

			case IDM_UP_1:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);			// �������� ������ ��������� �������
			if (SendMessage(hwndList, LB_GETSEL, i,0)==-1) return 0;// ���� �� ��� ������ �� �������
			if (i == 0) return 0;
			SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);	// �������� ������� ������ � szBuf
			SendMessage(hwndList,LB_DELETESTRING,i,0);
			SendMessage(hwndList,LB_INSERTSTRING,i-1,(LPARAM) szBuf );
			SendMessage(hwndList,LB_SETCURSEL,i-1,0);
			RepaintBack();
			InvalidateRect(hwndHolst,NULL,FALSE);			
			return 0;

			case IDM_UP_10:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);			// �������� ������ ��������� �������
			if (SendMessage(hwndList, LB_GETSEL, i,0)==-1) return 0;// ���� �� ��� ������ �� �������
			if (i == 0) return 0;
			SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);	// �������� ������� ������ � szBuf
			SendMessage(hwndList,LB_DELETESTRING,i,0);
			if (i < 10) i=10;
			SendMessage(hwndList,LB_INSERTSTRING,i-10,(LPARAM) szBuf );
			SendMessage(hwndList,LB_SETCURSEL,i-10,0);
			RepaintBack();
			InvalidateRect(hwndHolst,NULL,FALSE);			
			return 0;

			case IDM_DOWN_1:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);			// �������� ������ ��������� �������
			if (SendMessage(hwndList, LB_GETSEL, i,0)==-1) return 0;// ���� �� ��� ������ �� �������
			iCount = SendMessage(hwndList,LB_GETCOUNT,0,0);
			if (i == iCount - 1) return 0;
			SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);	// �������� ������� ������ � szBuf
			SendMessage(hwndList,LB_DELETESTRING,i,0);
			SendMessage(hwndList,LB_INSERTSTRING,i+1,(LPARAM) szBuf );
			SendMessage(hwndList,LB_SETCURSEL,i+1,0);
			RepaintBack();
			InvalidateRect(hwndHolst,NULL,FALSE);		
			return 0;

			case IDM_DOWN_10:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);			// �������� ������ ��������� �������
			if (SendMessage(hwndList, LB_GETSEL, i,0)==-1) return 0;// ���� �� ��� ������ �� �������
			if (i == iCount - 1) return 0;
			SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);	// �������� ������� ������ � szBuf
			SendMessage(hwndList,LB_DELETESTRING,i,0);
			if (i > iCount - 11) i=iCount - 11;
			SendMessage(hwndList,LB_INSERTSTRING,i+10,(LPARAM) szBuf );
			SendMessage(hwndList,LB_SETCURSEL,i+10,0);
			RepaintBack();
			InvalidateRect(hwndHolst,NULL,FALSE);			
			return 0;

			case IDM_TOOLBAR:
			if (IsWindowVisible(hwndToolBar)) 
			{
				ShowWindow(hwndToolBar,SW_HIDE);
				CheckMenuItem(hMenu,IDM_TOOLBAR,MF_UNCHECKED);
			}
			else 
			{
				ShowWindow(hwndToolBar,SW_SHOW);
				CheckMenuItem(hMenu,IDM_TOOLBAR,MF_CHECKED);
			}
			RecountCoord();
			return 0;

			case IDM_PALETTE:
			if (IsWindowVisible(hwndPalette)) 
			{
				ShowWindow(hwndPalette,SW_HIDE);
				CheckMenuItem(hMenu,IDM_PALETTE,MF_UNCHECKED);
			}
			else 
			{
				ShowWindow(hwndPalette,SW_SHOW);
				CheckMenuItem(hMenu,IDM_PALETTE,MF_CHECKED);
			}
			RecountCoord();
			return 0;

			case IDM_STATUSBAR:
			if (IsWindowVisible(hwndStatusBar)) 
			{
				ShowWindow(hwndStatusBar,SW_HIDE);
				CheckMenuItem(hMenu,IDM_STATUSBAR,MF_UNCHECKED);
			}
			else 
			{
				ShowWindow(hwndStatusBar,SW_SHOW);
				CheckMenuItem(hMenu,IDM_STATUSBAR,MF_CHECKED);
			}
			RecountCoord();
			return 0;

			case IDM_LIST:
			if (IsWindowVisible(hwndList))
			{
				ShowWindow(hwndList,SW_HIDE);
				CheckMenuItem(hMenu,IDM_LIST,MF_UNCHECKED);
			}
			else 
			{
				ShowWindow(hwndList,SW_SHOW);
				CheckMenuItem(hMenu,IDM_LIST,MF_CHECKED);
			}
			RecountCoord();
			return 0;

			case TB_ID_RGB:
			if (ChooseColor(&cc)==TRUE)
			{
				if (IsUpColor)	crCurUp=cc.rgbResult;				// �������� ������� ����
				else crCurDown=cc.rgbResult;						// �������� ������ ����

				crPalCol[tekI][tekJ] = cc.rgbResult;				// �������� ���� � ������� ������ �������


				RisPal();
				sprintf(szBuf, "%s %06X %s %06X", "������� RGB = ", crCurUp, " ������ RGB = ", crCurDown);
				SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)szBuf);
			}
			return 0;
			
			case TB_ID_ARROW:
			SetTimer(hwndHolst,1,500,NULL);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_ARROW,TRUE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_LINE,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_RECTANGLE,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_CIRCLE,FALSE);
			LightOn = TRUE;
			SetFocus(hwndHolst);
			status = 0;
			return 0;
	
			case TB_ID_LINE:
			KillTimer(hwndHolst,1);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_LINE,TRUE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_ARROW,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_RECTANGLE,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_CIRCLE,FALSE);
			InvalidateRect(hwndHolst,NULL,FALSE);
			status = 1;
			return 0;

			case TB_ID_RECTANGLE:
			KillTimer(hwndHolst,1);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_RECTANGLE,TRUE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_ARROW,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_LINE,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_CIRCLE,FALSE);
			InvalidateRect(hwndHolst,NULL,FALSE);
			status = 2;
			return 0;

			case TB_ID_CIRCLE:
			KillTimer(hwndHolst,1);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_CIRCLE,TRUE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_ARROW,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_LINE,FALSE);
			SendMessage(hwndToolBar,TB_CHECKBUTTON,(WPARAM)TB_ID_RECTANGLE,FALSE);
			InvalidateRect(hwndHolst,NULL,FALSE);
			status = 3;
			return 0;
		}

		case WM_MENUSELECT:
		switch (HIWORD(wParam))
		{
			case MF_MOUSESELECT | MF_HILITE | MF_POPUP :	// ������ ����� ���������� ����
			LoadString(hInst, 20+LOWORD(wParam), szBuf, sizeof(szBuf));
			SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)szBuf);
			return 0;

			case MF_MOUSESELECT | MF_HILITE :				// �������
			LoadString(hInst, LOWORD(wParam), szBuf, sizeof(szBuf));
			SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)szBuf);
			return 0;

			case MF_MOUSESELECT | MF_HILITE | MF_CHECKED :	// �������
			LoadString(hInst, LOWORD(wParam), szBuf, sizeof(szBuf));
			SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)szBuf);
			return 0;

			default:
			SendMessage (hwndStatusBar, SB_SETTEXT, 0, (LPARAM) "������");
		} 
		return 0;
	
		case WM_NOTIFY:
		pnmh = (LPNMHDR) lParam;
		if(pnmh->code == TTN_NEEDTEXT)
		{
			LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;
			switch ( lpttt->hdr.idFrom)
			{
				case IDM_NEW:
				pReply = "������� ����� �������";
				break;

				case IDM_OPEN:
				pReply = "������� ����...";
				break;
			
				case IDM_SAVE:
				pReply = "��������� ����";
				break;
			
				case IDM_HELP:
				pReply = "������...";
				break;
			
				case TB_ID_RGB:
				pReply = "�������� �������";
				break;
			
				case TB_ID_ARROW:
				pReply = "���������";
				break;
			
				case TB_ID_LINE:
				pReply = "�����";
				break;
			
				case TB_ID_RECTANGLE:
				pReply = "�������������";
				break;
			
				case TB_ID_CIRCLE:
				pReply = "����������";
				break;

				default:
				return 0;
			}
			lstrcpy(lpttt->szText, pReply);
		}
		return 0;
	
		case WM_DESTROY:
		DeleteObject(hPen);
		DeleteObject(hPenL);		// ������ ������� ������ ����
		DeleteObject(hPenD);		// ������ ������ ������ ����
		DeleteObject(hSelectPen);		// ������ ����
		DeleteObject(hBrush);
		DeleteObject(hSelectBrush);		// ������ �����
		DeleteDC(hdcMemPalette);
		DeleteDC(hdcMemHolstBack);
		DeleteDC(hdcMemHolstDyn);
		PostQuitMessage(0);
		KillTimer(hwndHolst,1);
		return 0;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

LRESULT CALLBACK WndPalette(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int x,y;
	PAINTSTRUCT		ps;
	switch (iMsg)
	{
		case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x > 30)
		{
			IsUpColor = TRUE;
			tekI = (x - 31)/16;
			tekJ = y/16;
			crCurUp = crPalCol[tekI][tekJ];
			sprintf(szBuf, "%s %06X %s %06X", "������� RGB = ", crCurUp, "  ������ RGB = ", crCurDown);
			SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)szBuf);
			RisPal();
		}
		return 0;

		case WM_RBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x > 30)
		{
			IsUpColor = FALSE;
			tekI = (x - 31)/16;
			tekJ = y/16;
			crCurDown = crPalCol[tekI][tekJ];
			sprintf(szBuf, "%s %06X %s %06X", "������� RGB = ", crCurUp, "  ������ RGB = ", crCurDown);
			SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)szBuf);
			RisPal();
		}
		return 0;

		case WM_PAINT:
		hdcPalette= BeginPaint(hWnd,&ps);
		BitBlt(hdcPalette, 0, 0, 254, 31,	hdcMemPalette, 0, 0, SRCCOPY);
		EndPaint(hWnd,&ps);
		return 0;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

inline void RepaintBack(void)
{
	int x1,y1,x2,y2;
	COLORREF crPlay;

	SendMessage(hwndList, LB_GETTEXT, 0,(LPARAM) szBuf);
	SelectObject(hdcMemHolstBack,GetStockObject(WHITE_BRUSH));
	SelectObject(hdcMemHolstBack,GetStockObject(BLACK_PEN));
	Rectangle(hdcMemHolstBack,0,0,800,600);

	iCount = SendMessage(hwndList,LB_GETCOUNT,0,0);
	for (i = 0; i<iCount; i++) // ������� �����
	{
		SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);	// �������� ������� ������ � szBuf

		sprintf(szCount,"%c%c%c%c",szBuf[9],szBuf[10],szBuf[11],szBuf[12]); // ������� szCount �� ��������� szBuf
		x1=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[13],szBuf[14],szBuf[15],szBuf[16]);
		y1=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[18],szBuf[19],szBuf[20],szBuf[21]);
		x2=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[23],szBuf[24],szBuf[25],szBuf[26]);
		y2=atoi(szCount);

		sprintf(szCount,"%c%c%c%c%c%c",szBuf[28],szBuf[29],szBuf[30],szBuf[31],szBuf[32],szBuf[33]);
		crPlay = HexSzToInt(szCount);
		DeleteObject(hPen);						// ���������� ����
		hPen = CreatePen(PS_SOLID,0,crPlay);	// ������� ���� �� ������ �������� �����

		sprintf(szCount,"%c%c%c%c%c%c",szBuf[35],szBuf[36],szBuf[37],szBuf[38],szBuf[39],szBuf[40]);
		crPlay = HexSzToInt(szCount);
		DeleteObject(hBrush);						// ���������� �����
		hBrush = CreateSolidBrush(crPlay);	// ������� ����� �� ������ �������� �����

		SelectObject(hdcMemHolstBack,hPen);
		SelectObject(hdcMemHolstBack,hBrush);

		switch (szBuf[1])
		{
			case 'L':
			MoveToEx(hdcMemHolstBack,x1,y1,NULL);	// ��������� ���� � ����� �������
			LineTo(hdcMemHolstBack,x2,y2);			// ����� �� ����� ������� �� ���������� �����;
			break;

			case 'R':
			Rectangle(hdcMemHolstBack,x1,y1,x2,y2);	// ������������� �� ����� ������� �� ������� �����;
			break;
			
			case 'C':
			Ellipse(hdcMemHolstBack,x1,y1,x2,y2);	// ������������� �� ����� ������� �� ������� �����;
		}
	}//for
}

inline void RepaintDyn(int xDown,int yDown,int xMove,int yMove)
{
	SelectObject(hdcMemHolstDyn,hSelectPen);			// ������� ������� ����
	SelectObject(hdcMemHolstDyn,hSelectBrush);			// ������� ������� �����
	switch (status)										// ����� ������ ������
	{
		case 1:												// �����
		MoveToEx(hdcMemHolstDyn,xDown,yDown,NULL);			// ��������� ���� � ����� �������
		LineTo(hdcMemHolstDyn,xMove,yMove);					// ����� �� ����� ������� �� ������� �����
		break;

		case 2:												// �������������
		Rectangle(hdcMemHolstDyn,xDown,yDown,xMove,yMove);	// �� ����� ������� �� ������� �����
		break;

		case 3:												// ������
		Ellipse(hdcMemHolstDyn,xDown,yDown,xMove,yMove);	// �� ����� ������� �� ������� �����
	}
	sprintf(szBuf, "%s %d %s %d %s %d", "R= ", crCurDown<<24>>24, " G= ", crCurDown<<16>>24, " B=",crCurDown<<8>>24);
	SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM)szBuf);

}

inline void Selecting(int X,int Y) // ������� ����������
{
	int x1,y1,x2,y2,yy,xx;
	int GorX,GorY,VerX,VerY;
	double a,b,k,z;
	BOOL	is = FALSE;
	iCount = SendMessage(hwndList,LB_GETCOUNT,0,0);
	for (i = iCount-1; i>-1; i--) // ������� �����
	{
		SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);
		sprintf(szCount,"%c%c%c%c",szBuf[9],szBuf[10],szBuf[11],szBuf[12]);
		x1=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[13],szBuf[14],szBuf[15],szBuf[16]);
		y1=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[18],szBuf[19],szBuf[20],szBuf[21]);
		x2=atoi(szCount);
		sprintf(szCount,"%c%c%c%c",szBuf[23],szBuf[24],szBuf[25],szBuf[26]);
		y2=atoi(szCount);
		switch (szBuf[1]) // �������� ��������� ������ ������
		{
			
			case 'L': // ����� �� ������ �� �����
			if (x1==x2)	// ������������ �����
			{
				if ((MinCoordOf2(y1,y2)<=Y) && (Y<=MaxCoordOf2(y1,y2)))
				{
					if (((x1-2)<X) && (X<(x1+2))) is = TRUE;
				}	
			}
			else if (y1==y2) // �������������� �����
			{
				if ((MinCoordOf2(x1,x2)<=X) && (X<=MaxCoordOf2(x1,x2)))
				{
					if (((y1-2)<Y) && (Y<(y1+2))) is = TRUE;
				}	
			}
			else if ( (MinCoordOf2(x1,x2)<=X) && (X<=MaxCoordOf2(x1,x2)) &&  (MinCoordOf2(y1,y2)<=Y) && (Y<=MaxCoordOf2(y1,y2)) )
			{
				k = (((double)y1-(double)y2)/((double)x1-(double)x2));
				z = (double)y1 - k*(double)x1;
				// ������� ����� ����������� � ������������
				GorX = (int)(((double)Y - z)/k);
				GorY = Y;
				// ������� ����� ����������� � ����������
				VerY = (int)(k*(double)X+(double)z);
				VerX = X;
				if ((abs(X-GorX)<3) || (abs(Y-VerY)<3))
				{
					is = TRUE;
				}
			}
			break;
			
			case 'R': // ����� �� ������ � �������������
			if (  (MinCoordOf2(x1,x2)<=X) && (X<=MaxCoordOf2(x1,x2)) &&  (MinCoordOf2(y1,y2)<=Y) && (Y<=MaxCoordOf2(y1,y2)) )
			{
				is = TRUE;
			}
			break;
					
			case 'C': // ����� �� ������ � ������
			if (  (MinCoordOf2(x1,x2)<=X) && (X<=MaxCoordOf2(x1,x2)) &&  (MinCoordOf2(y1,y2)<=Y) && (Y<=MaxCoordOf2(y1,y2)) )
			{
				a = (MaxCoordOf2(x1,x2) - MinCoordOf2(x1,x2))/2;
				b = (MaxCoordOf2(y1,y2) - MinCoordOf2(y1,y2))/2;
				xx = X - (x1+x2)/2;
				yy = Y - (y1+y2)/2;
				if (	((xx*xx)/(a*a))+((yy*yy)/(b*b))<=1)
				{
					is = TRUE;
				}
			}
		}
	if (is==TRUE) 
	{
		SendMessage(hwndList, LB_SETCURSEL, i, 0);	// ���������������� ������� �� ���������� �������
		SetFocus(hwndList);
		break;
	}
	}

}

LRESULT CALLBACK WndHolst(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static int xDown,yDown,xMove,yMove;
	static BOOL	Down=FALSE;
	int x1,y1,x2,y2;
	PAINTSTRUCT	psHolst;
	switch (iMsg)
	{
		case WM_PAINT:
		hdcHolst = BeginPaint(hWnd,&psHolst);
		BitBlt(hdcMemHolstDyn, 0, 0, 800, 600,	hdcMemHolstBack, 0, 0, SRCCOPY);// �������� ����� ����������� � ��������������
		if (Down == TRUE) RepaintDyn(xDown,yDown,xMove,yMove);					// ������ ������� ������ � ��������������
		BitBlt(hdcHolst, 10, 10, 800, 600,hdcMemHolstDyn, 0, 0, SRCCOPY);		// �������� �������������� �� �����
		EndPaint(hWnd,&psHolst);
		return 0;

		case WM_LBUTTONDOWN:
		xMove = xDown = LOWORD(lParam)-10;
		yMove = yDown = HIWORD(lParam)-10;

		if (status== 0) 
		{
			Selecting(xDown,yDown);
		}

		DeleteObject(hSelectPen);						// ���������� ����
		hSelectPen = CreatePen(PS_SOLID,0,crCurUp);		// ������� ���� �� ������ �������� �����
		DeleteObject(hSelectBrush);						// ���������� �����
		hSelectBrush = CreateSolidBrush(crCurDown);		// ������� ����� �� ������ ������� �����
		Down	= TRUE;									// ���� ������
		return 0;

		case WM_RBUTTONDOWN:
		Down = FALSE;
		InvalidateRect(hWnd,NULL,FALSE);
		return 0;
	
		case WM_MOUSEMOVE:
		xMove = LOWORD(lParam)-10;
		yMove = HIWORD(lParam)-10;
		sprintf(szBuf, "%s %04d %s %04d", "X = ",xMove," Y = ",yMove);
		SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM)szBuf);
		if (Down == FALSE) return 0;			// ���� ���� �� ������ - �����
		InvalidateRect(hWnd,NULL,FALSE);
		return 0;

		case WM_LBUTTONUP:
		if (Down==FALSE) return 0;
		if ((xDown==xMove) && (yDown==yMove))
		{
			Down = FALSE;
			return 0;
		}
		switch (status)
		{
			case 0:
			Down = FALSE;			// ���� ��������
			return 0;
	
			case 1:
			sprintf(szBuf,"%s %04d %04d %04d %04d %06X %06X", " Line =",xDown, yDown, xMove, yMove, crCurUp, crCurDown);
			SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM) szBuf);
			break;
			
			case 2:
			sprintf(szBuf,"%s %04d %04d %04d %04d %06X %06X", " Rect =",xDown, yDown, xMove, yMove, crCurUp, crCurDown);
			SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM) szBuf);
			break;
			
			case 3:
			sprintf(szBuf,"%s %04d %04d %04d %04d %06X %06X", " Circ =",xDown, yDown, xMove, yMove, crCurUp, crCurDown);
			SendMessage(hwndList,LB_ADDSTRING,0,(LPARAM) szBuf);
			// �� ������� ����� ����� break :)

		}// switch
		iCount = SendMessage(hwndList,LB_GETCOUNT,0,0);
		SendMessage(hwndList, LB_SETCURSEL, iCount-1, 0);	// ���������������� ������� �� ��������� ������
		Down = FALSE;										// ���� ��������
		RepaintBack();										// �������������� ������� �����������
		return 0;

		case WM_TIMER:
		i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);			// �������� ������ ��������� �������
		if (SendMessage(hwndList, LB_GETSEL, i,0)==-1) return 0;// ���� �� ��� ������ �� �������
		if (LightOn==TRUE)
		{
			LightOn=FALSE;
			SendMessage(hwndList, LB_GETTEXT, i,(LPARAM) szBuf);
			sprintf(szCount,"%c%c%c%c",szBuf[9],szBuf[10],szBuf[11],szBuf[12]);
			x1=atoi(szCount)+10;
			sprintf(szCount,"%c%c%c%c",szBuf[13],szBuf[14],szBuf[15],szBuf[16]);
			y1=atoi(szCount)+10;
			sprintf(szCount,"%c%c%c%c",szBuf[18],szBuf[19],szBuf[20],szBuf[21]);
			x2=atoi(szCount)+10;
			sprintf(szCount,"%c%c%c%c",szBuf[23],szBuf[24],szBuf[25],szBuf[26]);
			y2=atoi(szCount)+10;
			hdcHolst = GetDC(hWnd);
			DeleteObject(hPen);
			hPen = CreatePen(PS_SOLID,2,0x0000FF);
			SelectObject(hdcHolst,hPen);
			SelectObject(hdcHolst,GetStockObject(NULL_BRUSH));
			switch (szBuf[1])
			{
				case 'L':
				MoveToEx(hdcHolst,x1,y1,NULL);	// ��������� ���� � ����� �������
				LineTo(hdcHolst,x2,y2);			// ����� �� ����� ������� �� ���������� �����;
				break;

				case 'R':
				Rectangle(hdcHolst,x1,y1,x2+1,y2+1);	// ������������� �� ����� ������� �� ������� �����;
				break;
		
				case 'C':
				Ellipse(hdcHolst,x1,y1,x2,y2);	// ������������� �� ����� ������� �� ������� �����;
			}
			ReleaseDC(hWnd,hdcHolst);
		}
		else
		{
			LightOn = TRUE;
			InvalidateRect(hWnd,NULL,FALSE);
		}
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}


LRESULT CALLBACK WndList(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam) // �������� ������� ���������
{
	if (iMessage == WM_KEYUP)
	if (wParam == VK_DELETE)
	{
		i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
		SendMessage(hwndList,LB_DELETESTRING,i,0);
		RepaintBack();
		InvalidateRect(hwndHolst,NULL,FALSE);
	}
	return CallWindowProc(OldWndList,hwnd,iMessage, wParam, lParam);			// ��������� � ������� ���������
}
