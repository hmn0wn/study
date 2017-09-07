// Win32Render.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Render.h"
#include <memory>
#include <time.h> 


#include "camera.h"
#include "model.h"
#include "screen.h"
#include "shader.h"
#include "painter.h"
#include "color.h"



#define MAX_LOADSTRING 100

// Global Variables:
Camera	camera;
Model	model;
Screen	screen;
Light	light;
IShader *shader;

Painter painter;



HDC _hdc;  //window descriptor
clock_t time_old;
clock_t time_now;
size_t	draw_counter_old;
size_t	draw_counter;
size_t	fps;



// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void game(HWND);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32RENDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32RENDER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32RENDER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32RENDER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{

		time_now = clock();
		time_old = time_now;
		draw_counter = 0;
		draw_counter_old = 0;
		fps = 0;


		_hdc = GetDC(hWnd);
		HDC hdcScr = CreateCompatibleDC(_hdc);

		
		int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		

		SelectObject(hdcScr, CreateCompatibleBitmap(_hdc, width, height)); //create bmp and get descripter
		PatBlt(hdcScr, 0, 0, width, height, PATCOPY);
		
		camera = Camera(75.0f, (float)height/ (float)width, 2.0f, 500.f);

		//model = Model("obj/cube.obj");
		model = Model("obj/planets.obj");

		screen = Screen(hdcScr, width, height, color_nm::black);
		
		shader = new PhongShader();

		light = Light();

		painter = Painter(&camera, &model, &screen, shader, &light);


	}
	break;
    
	case WM_KEYDOWN:
	{
	

			switch (wParam)
			{

			

			case 0x6B: //+
			{
				if (GetAsyncKeyState(0x52))//r
					model.specular_exp_up();

				if (GetAsyncKeyState(0x50))//p
					painter.rot_step_up();

			}
			break;

			case 0x6D: //-
			{
				if (GetAsyncKeyState(0x52))//r
					model.specular_exp_down();

				if (GetAsyncKeyState(0x50))//p
					painter.rot_step_down();

				
			}
			break;

			case 0x4F: //o
			{
				painter.draw_light_onoff();
			}
			break;

			case 0x47: //g
			{
				painter.mesh_onoff();
			}
			break;

			case 0x46: //f
			{
				if(GetAsyncKeyState(0x4E))//n
					painter.face_normals_onoff();
			}
			break;

			case 0x43: //c
			{
				if (GetAsyncKeyState(0x4E))//n
					painter.get_vertex_normals_source_onoff();
			}
			break;

			case 0x56: //f
			{
				if (GetAsyncKeyState(0x4E))//n
					painter.vertex_normals_onoff();

			}
			break;

				
			
			case 0x5A: //z
			{
				painter.debug_onoff();
			}
			break;

			case 0x31:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/plane.obj");

				if (GetAsyncKeyState(0x54))//t
				{
					shader = new FlatLambertianShader();
					painter.update_shader(shader);
				}

				if (GetAsyncKeyState(0x58))//x
					painter.draw_world_axis_onoff();

				if (GetAsyncKeyState(VK_SPACE))
				{
					painter.rot_in_model_xyz_onoff();
				}

			}
			break;

			case 0x32:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/cube.obj");

				if (GetAsyncKeyState(0x54))//t
				{
					shader = new InterpolatedLambertianShader();
					painter.update_shader(shader);
				}
				
				if (GetAsyncKeyState(0x58))//x
					painter.draw_model_axis_onoff();

				if (GetAsyncKeyState(VK_SPACE))
				{
					painter.rot_in_world_xyz_onoff();
				}
			}
			break;

			case 0x33:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/pyramid.obj");
				
				if (GetAsyncKeyState(0x54))//t
				{
					shader = new GouraudShader();
					painter.update_shader(shader);

				}
			}
			break;

			case 0x34:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/sphere.obj");
				
				if (GetAsyncKeyState(0x54))//t
				{
					shader = new PhongShader();
					painter.update_shader(shader);
				}
			}
			break;

			case 0x35:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/torus.obj");
			}
			break;

			case 0x36:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/cone.obj");
			
			}
			break;

			case 0x37:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/cylinder.obj");
				
			}
			break;

			case 0x38:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/pipe.obj");
				
			}
			break;

			case 0x39:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/pipe_pyramid.obj");

				if (GetAsyncKeyState(0x54))//t
				{
					shader = new NormalShader();
					painter.update_shader(shader);
				}
			}
			break;

			case 0x30:
			{
				if (GetAsyncKeyState(0x4D))//m
					model =  Model("obj/suzanne.obj");	

				if (GetAsyncKeyState(0x54))//t
				{
					painter.draw_zbuffer_onoff();
				}
			}
			break;
			

			case VK_NUMPAD1: 
			{

				camera.up();
			}
			break;

			case VK_NUMPAD2: 
			{
				camera.down();
			}
			break;

			case VK_UP: 
			{
				if (GetAsyncKeyState(0x4D))//m

				camera.forward();

				if (GetAsyncKeyState(0x59))//y
					light.move(camera.world_ort_j);
			}
			break;

			case VK_DOWN:
			{
				if (GetAsyncKeyState(0x4D))//m

				camera.back();
				if (GetAsyncKeyState(0x59))//y
					light.move(camera.world_ort_j * -1.f);
			}
			break;
			case VK_LEFT:
			{
				if (GetAsyncKeyState(0x4D))//m

				camera.left();

				if (GetAsyncKeyState(0x59))//y
					light.move(camera.world_ort_k * -1.f);
			}
			break;

			case VK_RIGHT:
			{
				if (GetAsyncKeyState(0x4D))//m

				camera.right();
				if (GetAsyncKeyState(0x59))//y
					light.move(camera.world_ort_k);
			}
			break;

			case 0x57: //w
			{
				camera.rot_up();
			}
			break;
			case 0x53: //s
			{
				camera.rot_down();
			}
			break;

			case 0x41: //a
			{
				camera.rot_left();
			}
			break;

			case 0x44: //d
			{
				camera.rot_right();
			}
			break;

			case 0x4C: // L
			{
				camera.field_of_view_up();
			}
			break;

			case 0x4B: // K
			{
				camera.field_of_view_down();
			}
			break;


		default:
			break;
		}
	
	}
	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			//SendMessage(hWnd, WM_PAINT, wParam, lParam);

            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			//painter.draw_to_screen(image_buf);
			std::cout << std::endl;
			screen.update_param(screen.get_hdc(), ps.rcPaint.right, ps.rcPaint.bottom);
			camera.update_param(camera.field_of_view, screen.get_aspect_ratio(), camera.near_plane, camera.far_plane);
			/*for (size_t i = 0; i < buf_size; ++i)
				image_buf[i] = rand() % 255;*/
			painter.draw();
			
			//fps-------------
			time_now = clock();
			draw_counter++;

			clock_t delta_time = time_now - time_old;
			if (time_now - time_old >= CLOCKS_PER_SEC/3)
			{
				fps = (draw_counter - draw_counter_old) *  CLOCKS_PER_SEC / delta_time;
				time_old = time_now;
				draw_counter_old = draw_counter;
							
			}
			painter.add_to_print_stack(TextToDraw(L"fps: ", fps, 10, screen.get_height() - 10));
			//fps-------------

			//TextOut(screen.hdc, 10, 10, info, wcslen(info));


			BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, screen.get_hdc(), 0, 0, SRCCOPY);
			
			

			//RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);

			EndPaint(hWnd, &ps);
			InvalidateRect(hWnd, NULL, false); //Refresh


	}
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void game(HWND hWnd)
{
	
}
