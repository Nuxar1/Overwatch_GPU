/*
 *Overwatch Aim Assitance
 *Copyright (C) 2016  Juan Xuereb
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.

 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Capture.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include "Globals.h"
#pragma comment(lib, "d3d9.lib")

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

Capture::Capture(char* Name)
{
	hWindow = FindWindowA(Name, NULL);
    windowName = Name;
}

Capture::~Capture()
{
}

bool Capture::isWindowRunning()
{
    hWindow = FindWindowA(windowName, NULL);
	return (hWindow != NULL);
}


void Capture::switchToWindow() {
	if (hWindow != NULL) SwitchToThisWindow(hWindow, false);
}

HDC hwindowDC, hwindowCompatibleDC;

int height, width, srcheight, srcwidth;
HBITMAP hbwindow;
cv::Mat src;
BITMAPINFOHEADER  bi;
RECT windowsize;    // get the height and width of the screen

void Capture::initScreenshot() {

    GetClientRect(hWindow, &windowsize);

    float valueOfChangeTheSizeOfTextAppsAndOtherItemsInWindowsDisplaySettings = 1;
    srcheight = (int)((float)Settings::input_dimensions[1] * valueOfChangeTheSizeOfTextAppsAndOtherItemsInWindowsDisplaySettings);
    srcwidth = (int)((float)Settings::input_dimensions[0] * valueOfChangeTheSizeOfTextAppsAndOtherItemsInWindowsDisplaySettings);
    height = Settings::input_dimensions[1] / 1;  //change this to whatever size you want to resize to
    width = Settings::input_dimensions[0] / 1;

    src.create(height, width, CV_8UC4);

    
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
}
cv::Mat Capture::hwnd2mat()
{
    hwindowDC = GetDC(hWindow);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    if(!StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, (windowsize.right - width) / 2, (windowsize.bottom - height) / 2, srcwidth, srcheight, SRCCOPY)) std::cout << "StetchBlt\n"; //change SRCCOPY to NOTSRCCOPY for wacky colors !
    if(!GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) std::cout << "GetDIBits\n";  //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hWindow, hwindowDC);

    return src;
}
HRESULT hr = S_OK;
IDirect3D9* d3d = nullptr;
IDirect3DDevice9* device = nullptr;
IDirect3DSurface9* surface = nullptr;
D3DPRESENT_PARAMETERS parameters = { 0 };
D3DDISPLAYMODE mode;
D3DLOCKED_RECT rc;
UINT pitch;
SYSTEMTIME st;
LPBYTE Screenshot = nullptr;
void Capture::InitDx() {
    // init D3D and get screen size
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    HRCHECK(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode));

    parameters.Windowed = TRUE;
    parameters.BackBufferCount = 1;
    parameters.BackBufferHeight = Settings::input_dimensions[0];
    parameters.BackBufferWidth = Settings::input_dimensions[1];
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.hDeviceWindow = NULL;

    // create device & capture surface
    HRCHECK(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device));
    HRCHECK(device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr));

    // compute the required buffer size
    HRCHECK(surface->LockRect(&rc, NULL, 0));
    pitch = rc.Pitch;
    HRCHECK(surface->UnlockRect());

    // allocate screenshots buffers
    Screenshot = new BYTE[pitch * mode.Height]; 
    return;
cleanup:
    if (Screenshot != nullptr)
    {
        delete[] Screenshot;
    }
    RELEASE(surface);
    RELEASE(device);
    RELEASE(d3d);
    return;
}
bool Capture::screenshotGDI(cv::Mat & outMat)
{

    // get the data
    HRCHECK(device->GetFrontBufferData(0, surface));

    // copy it into our buffers
    HRCHECK(surface->LockRect(&rc, NULL, 0));
    CopyMemory(Screenshot, rc.pBits, rc.Pitch * mode.Height);
    HRCHECK(surface->UnlockRect());
    
    outMat = cv::Mat(mode.Height, mode.Width, CV_8UC4, Screenshot).clone();
    // save all screenshots
    /*for (UINT i = 0; i < count; i++)
    {
        WCHAR file[100];
        wsprintf(file, L"cap%i.png", i);
        HRCHECK(SavePixelsToFile32bppPBGRA(mode.Width, mode.Height, pitch, shots[i], file, GUID_ContainerFormatPng));
    }*/
    return FAILED(hr);
cleanup:
    if (Screenshot != nullptr)
    {
        delete[] Screenshot;
    }
    RELEASE(surface);
    RELEASE(device);
    RELEASE(d3d);
    return FAILED(hr);
}

// Memory management for screenshot function above
void Capture::release(HWND& hwnd, HDC& hdc, HDC& captureDC, HBITMAP& hBmp)
{
	ReleaseDC(hwnd, hdc);
	DeleteObject(hBmp);
	DeleteDC(captureDC);
	DeleteDC(hdc);
}
