#include <windows.h>

HWND hWnd;
int	m_nWidth;
int	m_nHeight;
int w;
int h;

BYTE       * m_pBits;
BITMAPINFO * m_pBMI;

HMENU hOptionsMenu;
HMENU hMainMenu;


typedef enum 
{
	DIB_1BPP,
	DIB_2BPP,
	DIB_4BPP,
	DIB_4BPPRLE,
	DIB_8BPP, 
	DIB_8BPPRLE,

	DIB_16RGB555,
	DIB_16RGB565,
	DIB_24RGB888,
	DIB_32RGB888,

	DIB_32RGBA8888,

	DIB_16RGBbitfields,
	DIB_32RGBbitfields,

	DIB_JPEG,
	DIB_PNG
}  DIBFormat;


//функция декодирования bmp
bool AttachDIB(BITMAPINFO * pDIB)
{
	DIBFormat	 m_nImageFormat;

	

	RGBTRIPLE  * m_pRGBTRIPLE;
	RGBQUAD    * m_pRGBQUAD;
	int			 m_nClrUsed;
	int			 m_nClrImpt;
	DWORD	   * m_pBitFields;	
	

	int			 m_nPlanes;	
	int			 m_nBitCount;
	int			 m_nColorDepth;	
	int			 m_nImageSize;

						
	int			 m_nBPS;
	BYTE *       m_pOrigin;	
	int			 m_nDelta;


  
	m_pBMI         = NULL;
	m_pBits        = NULL;


	m_nWidth       = 0;
	m_nHeight      = 0;
	m_nPlanes      = 1;
	m_nBitCount    = 1;
	m_nColorDepth  = 1;
	m_nImageSize   = 0;
	m_nImageFormat = DIB_1BPP;

	m_pRGBTRIPLE   = NULL;
	m_pRGBQUAD	   = NULL;


	if ( IsBadReadPtr(pDIB, sizeof(BITMAPCOREHEADER)) )
		return false;

	m_pBMI      = pDIB;

	DWORD size = * (DWORD *) pDIB;

	int compression;

	switch ( size )
	{
		case sizeof(BITMAPCOREHEADER):
		{
			BITMAPCOREHEADER * pHeader = (BITMAPCOREHEADER *) pDIB;

			m_nWidth    = pHeader->bcWidth;
			m_nHeight   = pHeader->bcHeight;
			m_nPlanes   = pHeader->bcPlanes;
			m_nBitCount = pHeader->bcBitCount;
			m_nImageSize= 0;
			compression = BI_RGB;

			if ( m_nBitCount <= 8 )
			{
				m_nClrUsed   = m_nBitCount;
				m_nClrImpt   = m_nClrUsed;
				m_pRGBTRIPLE = (RGBTRIPLE *) ((BYTE *) m_pBMI + size);

				m_pBits      = (BYTE *) & m_pRGBTRIPLE[m_nClrUsed];
			}
			else
				m_pBits      = (BYTE *) m_pBMI + size;
			break;
		}

		case sizeof(BITMAPINFOHEADER):
		case sizeof(BITMAPV4HEADER):
		{
			BITMAPINFOHEADER * pHeader = & m_pBMI->bmiHeader;

			m_nWidth    = pHeader->biWidth;
			m_nHeight   = pHeader->biHeight;
			m_nPlanes   = pHeader->biPlanes;
			m_nBitCount = pHeader->biBitCount;
			m_nImageSize= pHeader->biSizeImage;
			compression = pHeader->biCompression;

			m_nClrUsed  = pHeader->biClrUsed;
			m_nClrImpt  = pHeader->biClrImportant;

			if ( m_nBitCount<=8 )
				if ( m_nClrUsed==0 )
					m_nClrUsed = 1 << m_nBitCount;

			if ( m_nClrUsed )
			{
				if ( m_nClrImpt==0 )
					m_nClrImpt = m_nClrUsed;
			
				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size + 3*sizeof(DWORD));
				}
				else
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size);

				m_pBits = (BYTE *) & m_pRGBQUAD[m_nClrUsed];
			}
			else
			{
				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pBits      = (BYTE *) m_pBMI + size + 3 * sizeof(DWORD);
				}
				else
					m_pBits      = (BYTE *) m_pBMI + size;
			}
			break;
		}

		default:
			return false;
	}


	m_nColorDepth = m_nPlanes * m_nBitCount;
	m_nBPS		  = (m_nWidth * m_nBitCount + 31) / 32 * 4;
	
	if (m_nHeight < 0 )
	{
		m_nHeight = - m_nHeight;
		m_nDelta  = m_nBPS;
		m_pOrigin = m_pBits;
	}
	else
	{
		m_nDelta  = - m_nBPS;
		m_pOrigin = m_pBits + (m_nHeight-1) * m_nBPS * m_nPlanes;
	}

	if ( m_nImageSize==0 )
		m_nImageSize = m_nBPS * m_nPlanes * m_nHeight;


	switch ( m_nBitCount )
	{

		case 1:
			m_nImageFormat = DIB_1BPP;
			break;

		case 2:
			m_nImageFormat = DIB_2BPP;
			break;

		case 4:
			if ( compression==BI_RLE4 )
				m_nImageFormat = DIB_4BPPRLE;
			else
				m_nImageFormat = DIB_4BPP;
			break;

		case 8:
			if ( compression==BI_RLE8 )
				m_nImageFormat = DIB_8BPPRLE;
			else
				m_nImageFormat = DIB_8BPP;
			break;
		
		case 16:
			if ( compression==BI_BITFIELDS )
				m_nImageFormat = DIB_16RGBbitfields;
			else
				m_nImageFormat = DIB_16RGB555;
			break;
		
		case 24:
			m_nImageFormat = DIB_24RGB888;
			break;

		case 32:
			if ( compression == BI_BITFIELDS )
				m_nImageFormat = DIB_32RGBbitfields;
			else
				m_nImageFormat = DIB_32RGB888;
			break;

		default:
			return false;
	}


	if ( compression==BI_BITFIELDS )
	{
		DWORD red   = m_pBitFields[0];
		DWORD green = m_pBitFields[1];
		DWORD blue  = m_pBitFields[2];

		if (      (blue==0x001F) && (green==0x03E0) && (red==0x7C00) )
			m_nImageFormat = DIB_16RGB555;
		else if ( (blue==0x001F) && (green==0x07E0) && (red==0xF800) )
			m_nImageFormat = DIB_16RGB565;
		else if ( (blue==0x00FF) && (green==0xFF00) && (red==0xFF0000) )
			m_nImageFormat = DIB_32RGB888;
	}

	return true;
}


//загрузка из файла
bool LoadFile(const TCHAR * pFileName)
{
	if ( pFileName==NULL )
		return false;

	HANDLE handle = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return false;

    BITMAPFILEHEADER bmFH;

	DWORD dwRead = 0;
	ReadFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

	if ( (bmFH.bfType == 0x4D42) && (bmFH.bfSize<=GetFileSize(handle, NULL)) )
	{
		BITMAPINFO * pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];
		
		if ( pDIB )
		{
			ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
			CloseHandle(handle);

			AttachDIB(pDIB);
		}
	}
	CloseHandle(handle);
    
	return false;
}

// диалоговое окно загрузки файла
VOID OpenBMPFile()
{
	TCHAR m_FileName[MAX_PATH];
	TCHAR m_TitleName[MAX_PATH];

	OPENFILENAME ofn;

    memset(& ofn, 0, sizeof(ofn));
    m_FileName[0] = 0;
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hWnd;
	ofn.lpstrFilter     = "BMP Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile		= m_FileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrFileTitle	= m_TitleName;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.lpstrDefExt		= NULL;
	ofn.nFilterIndex	= 1;
	ofn.Flags	= OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if ( GetOpenFileName(&ofn) )
	{
		LoadFile(m_FileName);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	
}

//меню
void CreateMenuItem(HMENU hmenu, char *str,UINT uID, UINT wID ,HMENU hSubMenu)
{
	MENUITEMINFO mi;
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
	mi.fType = MFT_STRING;
	mi.fState = MFS_ENABLED;
	mi.dwTypeData = str;
	mi.cch = sizeof(str);
	mi.wID = wID;
	mi.hSubMenu = hSubMenu;

	InsertMenuItem(hmenu, uID, FALSE, &mi);
}


//стандартная
LRESULT WINAPI MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	w = m_nWidth;
	h = m_nHeight;

	switch( msg )
	{
		case WM_CREATE:
		{
			hMainMenu = CreateMenu();
			hOptionsMenu = CreatePopupMenu();
			CreateMenuItem(hMainMenu, "Файл", 0, 0, hOptionsMenu);
			CreateMenuItem(hOptionsMenu, "Открыть", 0, 1001, NULL);

		
			SetMenu(hwnd,hMainMenu);

			DrawMenuBar(hwnd);
		}break;
		case WM_COMMAND:
			{
				if(LOWORD(wParam) == 1001)
					OpenBMPFile();
			}break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(hWnd, &ps);
				
				StretchDIBits(hDC, 0, 0, w, h, 0, 0, w, h, 
				m_pBits, m_pBMI, DIB_RGB_COLORS, SRCCOPY);

				EndPaint(hWnd, &ps);
			}break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;

}

	
//стандартная
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc,sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = MsgProc;   
	wc.hInstance     = hInst;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "BMPWindow";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(WS_EX_TOPMOST, "BMPWindow", 
		"Window", WS_OVERLAPPEDWINDOW , 100,100, 700, 500, 
		NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    MSG msg;

	while ( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}