#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <iostream>
#include <sstream>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"Msimg32.lib")
#include <math.h>
#include <cmath>
#include <time.h>
typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;

typedef struct
{
	int vtx0;
	int vtx1;
} EDGE;
#define M_PI   3.14159265358979323846264338327950288
//#define PI   3.14159265358979323846264338327950288
typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE Reserved;
	};
}_RGBQUAD, * PRGBQUAD;
typedef struct
{
	FLOAT h;
	FLOAT s;
	FLOAT l;
} HSL;

namespace Colors
{
	//These HSL functions was made by Wipet, credits to him!
	//OBS: I used it in 3 payloads

	//Btw ArTicZera created HSV functions, but it sucks unfortunatelly
	//So I didn't used in this malware.

	HSL rgb2hsl(RGBQUAD rgb)
	{
		HSL hsl;

		BYTE r = rgb.rgbRed;
		BYTE g = rgb.rgbGreen;
		BYTE b = rgb.rgbBlue;

		FLOAT _r = (FLOAT)r / 255.f;
		FLOAT _g = (FLOAT)g / 255.f;
		FLOAT _b = (FLOAT)b / 255.f;

		FLOAT rgbMin = min(min(_r, _g), _b);
		FLOAT rgbMax = max(max(_r, _g), _b);

		FLOAT fDelta = rgbMax - rgbMin;
		FLOAT deltaR;
		FLOAT deltaG;
		FLOAT deltaB;

		FLOAT h = 0.f;
		FLOAT s = 0.f;
		FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);

		if (fDelta != 0.f)
		{
			s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
			deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);

			if (_r == rgbMax)      h = deltaB - deltaG;
			else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
			else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
			if (h < 0.f)           h += 1.f;
			if (h > 1.f)           h -= 1.f;
		}

		hsl.h = h;
		hsl.s = s;
		hsl.l = l;
		return hsl;
	}

	RGBQUAD hsl2rgb(HSL hsl)
	{
		RGBQUAD rgb;

		FLOAT r = hsl.l;
		FLOAT g = hsl.l;
		FLOAT b = hsl.l;

		FLOAT h = hsl.h;
		FLOAT sl = hsl.s;
		FLOAT l = hsl.l;
		FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);

		FLOAT m;
		FLOAT sv;
		FLOAT fract;
		FLOAT vsf;
		FLOAT mid1;
		FLOAT mid2;

		INT sextant;

		if (v > 0.f)
		{
			m = l + l - v;
			sv = (v - m) / v;
			h *= 6.f;
			sextant = (INT)h;
			fract = h - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;

			switch (sextant)
			{
			case 0:
				r = v;
				g = mid1;
				b = m;
				break;
			case 1:
				r = mid2;
				g = v;
				b = m;
				break;
			case 2:
				r = m;
				g = v;
				b = mid1;
				break;
			case 3:
				r = m;
				g = mid2;
				b = v;
				break;
			case 4:
				r = mid1;
				g = m;
				b = v;
				break;
			case 5:
				r = v;
				g = m;
				b = mid2;
				break;
			}
		}

		rgb.rgbRed = (BYTE)(r * 255.f);
		rgb.rgbGreen = (BYTE)(g * 255.f);
		rgb.rgbBlue = (BYTE)(b * 255.f);

		return rgb;
	}
}
int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF Hue(int length) { //Credits to Void_/GetMBR
	if (red != length) {
		red < length; red++;
		if (ifblue == true) {
			return RGB(red, 0, length);
		}
		else {
			return RGB(red, 0, 0);
		}
	}
	else {
		if (green != length) {
			green < length; green++;
			return RGB(length, green, 0);
		}
		else {
			if (blue != length) {
				blue < length; blue++;
				return RGB(0, length, blue);
			}
			else {
				red = 0; green = 0; blue = 0;
				ifblue = true;
			}
		}
	}
}

DWORD WINAPI payload1(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;


	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int fx = (int)((i | 4) + (i * 4) * cbrt((x | y) * x));

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 800.f - y / screenHeight / .2f, 1.f);
				hslcolor.s = 0.700f;
				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI line(LPVOID lpParam) {
	int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	int signX = 1;
	int signY = 1;
	int signX1 = 1;
	int signY1 = 1;
	int incrementor = 10;
	int x = 10;
	int y = 10;
	while (1) {
		HDC hdc = GetDC(0);
		int top_x = 0 + x;
		int top_y = 0 + y;
		int bottom_x = 100 + x;
		int bottom_y = 100 + y;
		x += incrementor * signX;
		y += incrementor * signY;
		HBRUSH brush = CreateSolidBrush(Hue(239));
		SelectObject(hdc, brush);
		LineTo(hdc, bottom_x, bottom_y);
		if (y >= GetSystemMetrics(SM_CYSCREEN))
		{
			signY = -1;
		}

		if (x >= GetSystemMetrics(SM_CXSCREEN))
		{
			signX = -1;
		}

		if (y == 0)
		{
			signY = 1;
		}

		if (x == 0)
		{
			signX = 1;
		}
		Sleep(1);
		DeleteObject(brush);
		ReleaseDC(0, hdc);
	}
}
DWORD WINAPI rect(LPVOID lpParam) {
	int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	int signX = 1;
	int signY = 1;
	int signX1 = 1;
	int signY1 = 1;
	int incrementor = 10;
	int x = 0;
	int y = 0;
	while (1) {
		HDC hdc = GetDC(0);
		int top_x = 0 + x;
		int top_y = 0 + y;
		int bottom_x = 100 + x;
		int bottom_y = 100 + y;
		x += incrementor * signX;
		y += incrementor * signY;
		HBRUSH brush = CreateSolidBrush(Hue(239));
		SelectObject(hdc, brush);
		Rectangle(hdc, top_x, top_y, bottom_x, bottom_y);
		RoundRect(hdc, top_x, top_y, bottom_x, bottom_y, 1000, 100);
		Ellipse(hdc, top_x, top_y, bottom_x, bottom_y);
		for (INT i = 64; i > 8; i -= 8)
		{
			TRIVERTEX vertex[2];
			vertex[0].x = 50 - i + x;
			vertex[0].y = 50 - i + y;
			vertex[0].Red = 0x999999;
			vertex[0].Green = 0x111111;
			vertex[0].Blue = 0x111111;
			vertex[0].Alpha = 0;

			vertex[1].x = 100 + x;
			vertex[1].y = 100 + y;
			vertex[1].Red = 0;
			vertex[1].Green = 0x999999;
			vertex[1].Blue = 0x999999;
			vertex[1].Alpha = 0;

			// Create a GRADIENT_RECT structure that 
			// references the TRIVERTEX vertices. 
			GRADIENT_RECT gRect;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;

			// Draw a shaded rectangle. 
			GradientFill(hdc, vertex, 4, &gRect, 1, GRADIENT_FILL_RECT_H);
		}
		if (y >= GetSystemMetrics(SM_CYSCREEN))
		{
			signY = -1;
		}

		if (x >= GetSystemMetrics(SM_CXSCREEN))
		{
			signX = -1;
		}

		if (y == 0)
		{
			signY = 1;
		}

		if (x == 0)
		{
			signX = 1;
		}
		Sleep(1);
		DeleteObject(brush);
		ReleaseDC(0, hdc);
	}
}

VOID WINAPI ci(int x, int y, int w, int h)
{
	HDC hdc = GetDC(0);
	HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hdc, brush);
	BitBlt(hdc, x, y, w, h, hdc, x, y, 0x123456);
	DeleteObject(brush);
	ReleaseDC(NULL, hdc);
}

VOID WINAPI ci1(int x, int y, int w, int h)
{
	HDC hdc = GetDC(0);
	HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
	SelectObject(hdc, brush);
	BitBlt(hdc, x, y, w, h, hdc, x, y, 0x123456);
	DeleteObject(brush);
	ReleaseDC(NULL, hdc);
}

VOID WINAPI ci2(int x, int y, int w, int h)
{
	HDC hdc = GetDC(0);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
	SelectObject(hdc, brush);
	BitBlt(hdc, x, y, w, h, hdc, x, y, 0x123456);
	DeleteObject(brush);
	ReleaseDC(NULL, hdc);
}

DWORD WINAPI profecttouhou(LPVOID lpParam) {
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	int w = rect.right - rect.left - 500;
	int h = rect.bottom - rect.top - 500;
	int w1 = rect.right - rect.left - 500;
	int h1 = rect.bottom - rect.top - 500;
	int w2 = rect.right - rect.left - 500;
	int h2 = rect.bottom - rect.top - 500;
	for (int t = 0;; t++)
	{
		const int size = 1000;
		int x = rand() % (w + size) - size / 2;
		int y = rand() % (h + size) - size / 2;
		int x1 = rand() % (w + size) - size / 2;
		int y1 = rand() % (h + size) - size / 2;
		int x2 = rand() % (w + size) - size / 2;
		int y2 = rand() % (h + size) - size / 2;
		for (int i = 0; i < size; i += 100)
		{
			ci(x - i / 2, y - i / 2, i, i);
			ci1(x1 - i / 2, y1 - i / 2, i, i);
			ci2(x2 - i / 2, y2 - i / 2, i, i);
			Sleep(10);
		}
	}
}

DWORD WINAPI payload2(LPVOID lpParam) {
	HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
	PRGBTRIPLE rgbtriple;
	for (;;) {
		desk = GetDC(0);
		HDC deskMem = CreateCompatibleDC(desk);
		HBITMAP scr = CreateDIBSection(desk, &bmi, 0, (void**)&rgbtriple, 0, 0);
		SelectObject(deskMem, scr);
		BitBlt(deskMem, 0, 0, sw, sh, desk, 0, 0, SRCCOPY);
		for (int i = 0; i < sw * sh; i++) {
			int x = i % sw, y = i / sh, t = y ^ y | x;
			rgbtriple[i].rgbtRed &= x ^ y;
			rgbtriple[i].rgbtGreen += x + y;
			rgbtriple[i].rgbtBlue &= x ^ y;
		}
		BitBlt(desk, 0, 0, sw, sh, deskMem, 0, 0, SRCCOPY);
		ReleaseDC(wnd, desk);
		DeleteDC(desk); DeleteDC(deskMem); DeleteObject(scr); DeleteObject(wnd); DeleteObject(rgbtriple); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&bmi);
	}
}
DWORD WINAPI payload3(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;
	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;
	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;
	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);
	INT i = 0;
	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, NOTSRCCOPY);
		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int h = screenHeight;
				int w = screenWidth;
				INT Yii = y ^ y ^ i;
				INT Xii = x ^ x + i;
				INT Ssix = 6 ^ 10;
				INT Stwo = 2 ^ 10;
				FLOAT fx = (Xii ^ Yii) ^ (i ^ i * 10);

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 400.f + y / screenHeight * .10f, 1.f);
				hslcolor.s = 0.700f;
				hslcolor.l = 0.7f;
				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, NOTSRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI payload4(LPVOID lpParam) {
	BITMAPINFO bmi; // [rsp+50h] [rbp-30h] BYREF
	PRGBQUAD rgbScreen; // [rsp+88h] [rbp+8h] BYREF
	int code; // [rsp+90h] [rbp+10h]
	INT Yii; // [rsp+94h] [rbp+14h]
	INT Xii; // [rsp+98h] [rbp+18h]
	INT y; // [rsp+9Ch] [rbp+1Ch]
	INT x; // [rsp+A0h] [rbp+20h]
	int i; // [rsp+A4h] [rbp+24h]
	HBITMAP hbmTemp; // [rsp+A8h] [rbp+28h]
	INT h; // [rsp+B0h] [rbp+30h]
	INT w; // [rsp+B4h] [rbp+34h]
	HDC hdcMem; // [rsp+B8h] [rbp+38h]
	HDC hdcScreen; // [rsp+C0h] [rbp+40h]
	INT i_0; // [rsp+CCh] [rbp+4Ch]

	hdcScreen = GetDC(0i64);
	hdcMem = CreateCompatibleDC(hdcScreen);
	w = GetSystemMetrics(0);
	h = GetSystemMetrics(1);
	memset(&bmi, 0, sizeof(bmi));
	rgbScreen = 0i64;
	bmi.bmiHeader.biSize = 44;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	hbmTemp = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&rgbScreen, 0i64, 0);
	SelectObject(hdcMem, hbmTemp);
	i = 1;
	while (1)
	{
		hdcScreen = GetDC(0i64);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, 0x330008u);
		for (i_0 = 0; h * w > i_0; ++i_0)
		{
			x = i_0 % w;
			y = i_0 / w;
			Xii = i_0 / w * (i_0 % w);
			Yii = i_0 / w * (i_0 / w);
			code = ((i_0 / w ^ (i_0 % w)) + (Xii ^ Yii)) / 1000;
			rgbScreen[i_0].b += code;
			rgbScreen[i_0].rgb += code;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, 0x330008u);
		ReleaseDC(0i64, hdcScreen);
		DeleteDC(hdcScreen);
	}
}
DWORD WINAPI payload5(LPVOID lpParam) {
	BITMAPINFO bmi; // [rsp+50h] [rbp-30h] BYREF
	PRGBQUAD rgbScreen; // [rsp+88h] [rbp+8h] BYREF
	int code; // [rsp+90h] [rbp+10h]
	INT Yii; // [rsp+94h] [rbp+14h]
	INT Xii; // [rsp+98h] [rbp+18h]
	INT y; // [rsp+9Ch] [rbp+1Ch]
	INT x; // [rsp+A0h] [rbp+20h]
	int i; // [rsp+A4h] [rbp+24h]
	HBITMAP hbmTemp; // [rsp+A8h] [rbp+28h]
	INT h; // [rsp+B0h] [rbp+30h]
	INT w; // [rsp+B4h] [rbp+34h]
	HDC hdcMem; // [rsp+B8h] [rbp+38h]
	HDC hdcScreen; // [rsp+C0h] [rbp+40h]
	INT i_0; // [rsp+CCh] [rbp+4Ch]

	hdcScreen = GetDC(0i64);
	hdcMem = CreateCompatibleDC(hdcScreen);
	w = GetSystemMetrics(0);
	h = GetSystemMetrics(1);
	memset(&bmi, 0, sizeof(bmi));
	rgbScreen = 0i64;
	bmi.bmiHeader.biSize = 44;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	hbmTemp = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&rgbScreen, 0i64, 0);
	SelectObject(hdcMem, hbmTemp);
	i = 1;
	while (1)
	{
		hdcScreen = GetDC(0i64);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, 0x330008u);
		for (i_0 = 0; h * w > i_0; ++i_0)
		{
			x = i_0 % w;
			y = i_0 / w;
			Xii = i_0 % w + (i_0 % w);
			Yii = i_0 / w * (i_0 / w);
			code = (Yii + Xii - i_0 / w * (i_0 % w)) / 500;
			rgbScreen[i_0].b *= code;
			rgbScreen[i_0].rgb *= code;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, 0x330008u);
		ReleaseDC(0i64, hdcScreen);
		DeleteDC(hdcScreen);
	}
}
DWORD WINAPI payload6(LPVOID lpParam) {
	BITMAPINFO bmi; // [rsp+50h] [rbp-30h] BYREF
	PRGBQUAD rgbScreen; // [rsp+88h] [rbp+8h] BYREF
	int code; // [rsp+90h] [rbp+10h]
	INT Yii; // [rsp+94h] [rbp+14h]
	INT Xii; // [rsp+98h] [rbp+18h]
	INT y; // [rsp+9Ch] [rbp+1Ch]
	INT x; // [rsp+A0h] [rbp+20h]
	int i; // [rsp+A4h] [rbp+24h]
	HBITMAP hbmTemp; // [rsp+A8h] [rbp+28h]
	INT h; // [rsp+B0h] [rbp+30h]
	INT w; // [rsp+B4h] [rbp+34h]
	HDC hdcMem; // [rsp+B8h] [rbp+38h]
	HDC hdcScreen; // [rsp+C0h] [rbp+40h]
	INT i_0; // [rsp+CCh] [rbp+4Ch]

	hdcScreen = GetDC(0i64);
	hdcMem = CreateCompatibleDC(hdcScreen);
	w = GetSystemMetrics(0);
	h = GetSystemMetrics(1);
	memset(&bmi, 0, sizeof(bmi));
	rgbScreen = 0i64;
	bmi.bmiHeader.biSize = 44;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	hbmTemp = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&rgbScreen, 0i64, 0);
	SelectObject(hdcMem, hbmTemp);
	i = 1;
	while (1)
	{
		hdcScreen = GetDC(0i64);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, 0x330008u);
		for (i_0 = 0; h * w > i_0; ++i_0)
		{
			x = i_0 % w;
			y = i_0 / w;
			Xii = i_0 % w * (i_0 % w);
			Yii = i_0 / w * (i_0 / w);
			code = (Yii ^ Xii - i_0 / w * (i_0 % w)) / 500;
			rgbScreen[i_0].b += code;
			rgbScreen[i_0].rgb += code;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, 0x330008u);
		ReleaseDC(0i64, hdcScreen);
		DeleteDC(hdcScreen);
	}
}
DWORD WINAPI payload7(LPVOID lpParam) {
	BITMAPINFO pbmi; // [rsp+50h] [rbp-30h] BYREF
	void* ppvBits; // [rsp+80h] [rbp+0h] BYREF
	int v3; // [rsp+8Ch] [rbp+Ch]
	int v4; // [rsp+90h] [rbp+10h]
	int v5; // [rsp+94h] [rbp+14h]
	int v6; // [rsp+98h] [rbp+18h]
	int v7; // [rsp+9Ch] [rbp+1Ch]
	int v8; // [rsp+A0h] [rbp+20h]
	int v9; // [rsp+A4h] [rbp+24h]
	HGDIOBJ h; // [rsp+A8h] [rbp+28h]
	int cy; // [rsp+B0h] [rbp+30h]
	int SystemMetrics; // [rsp+B4h] [rbp+34h]
	HDC hdcSrc; // [rsp+B8h] [rbp+38h]
	HDC hdc; // [rsp+C0h] [rbp+40h]
	int i; // [rsp+CCh] [rbp+4Ch]

	hdc = GetDC(0i64);
	hdcSrc = CreateCompatibleDC(hdc);
	SystemMetrics = GetSystemMetrics(0);
	cy = GetSystemMetrics(1);
	memset(&pbmi, 0, sizeof(pbmi));
	ppvBits = 0i64;
	pbmi.bmiHeader.biSize = 44;
	pbmi.bmiHeader.biBitCount = 32;
	pbmi.bmiHeader.biPlanes = 1;
	pbmi.bmiHeader.biWidth = SystemMetrics;
	pbmi.bmiHeader.biHeight = cy;
	h = CreateDIBSection(hdc, &pbmi, 0, &ppvBits, 0i64, 0);
	SelectObject(hdcSrc, h);
	while (1)
	{
		hdc = GetDC(0i64);
		BitBlt(hdcSrc, 0, 0, SystemMetrics, cy, hdc, 0, 0, 0x330008u);
		for (i = 0; cy * SystemMetrics > i; ++i)
		{
			v9 = i % SystemMetrics;
			v8 = i / SystemMetrics;
			v7 = 1 ^ (i / SystemMetrics);
			v6 = i / SystemMetrics * (i / SystemMetrics);
			v5 = i % SystemMetrics + (i % SystemMetrics);
			v4 = v5 / 1 + v5;
			v3 = v6 / 1 + v6;
			*((DWORD*)ppvBits + i) += v4 - v3;
		}
		BitBlt(hdc, 0, 0, SystemMetrics, cy, hdcSrc, 0, 0, 0x330008u);
		ReleaseDC(0i64, hdc);
		DeleteDC(hdc);
	}
}
DWORD WINAPI payload8(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;
	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = w;
	bmpi.bmiHeader.biHeight = h;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;
	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;
	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);
	INT i = 0;
	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, NOTSRCCOPY);
		RGBQUAD rgbquadCopy;
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int index = y * w + x;
				int Xii = x | x, Yii = y * y;
				int inc = 1 + i;
				int fx = (Xii + Yii + inc | Xii + Yii + inc) + (i * 100);
				rgbquadCopy = rgbquad[index];
				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 1500.f + y / h * .1f + i / 5000.f, 1.f);
				hslcolor.s = 0.700f;
				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;
		StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, NOTSRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}
	return 0x00;
}
DWORD WINAPI payload9(LPVOID lpParam) {
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;
	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = w;
	bmpi.bmiHeader.biHeight = h;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;
	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;
	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);
	INT i = 0;
	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, NOTSRCCOPY);
		RGBQUAD rgbquadCopy;
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int index = y * w + x;
				int Xii = x | x, Yii = y * y;
				int inc = 1 + i;
				int fx = (Xii + Yii + inc | Xii + Yii + inc) + (i * 100);
				rgbquadCopy = rgbquad[index];
				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 30.f + y / h * .1f + i / 5000.f, 1.f);
				hslcolor.s = 0.700f;
				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;
		StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, NOTSRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}
	return 0x00;
}

int main() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	if (MessageBoxW(NULL, L"WARNING!!!\r\n\It doesn't delete all your data\r\n\Be fine ;)", L"Clipdrogen", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
	{
		ExitProcess(0);
	}
	else
	{
		if (MessageBoxW(NULL, L"F̷i̷n̸a̴l̴ ̴W̸a̴r̸n̷i̸n̷g̷ !!!\r\n\NOT FOR EPILEPSY!!!", L"F̷i̷n̸a̴l̴ ̴W̸a̴r̸n̷i̸n̷g̷  - Clipdrogen", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			ExitProcess(0);
		}
		else
		{
			Sleep(1000);
			HANDLE thread1 = CreateThread(0, 0, payload1, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread1, 0);
			CloseHandle(thread1);
			InvalidateRect(0, 0, 0);
			HANDLE thread2 = CreateThread(0, 0, payload2, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread2, 0);
			CloseHandle(thread2);
			InvalidateRect(0, 0, 0);
			HANDLE thread3 = CreateThread(0, 0, payload3, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread3, 0);
			CloseHandle(thread3);
			InvalidateRect(0, 0, 0);
			HANDLE thread4 = CreateThread(0, 0, payload4, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread4, 0);
			CloseHandle(thread4);
			InvalidateRect(0, 0, 0);
			HANDLE thread5 = CreateThread(0, 0, payload5, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread5, 0);
			CloseHandle(thread5);
			InvalidateRect(0, 0, 0);
			HANDLE thread6 = CreateThread(0, 0, payload6, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread6, 0);
			CloseHandle(thread6);
			InvalidateRect(0, 0, 0);
			HANDLE thread7 = CreateThread(0, 0, payload7, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread7, 0);
			CloseHandle(thread7);
			InvalidateRect(0, 0, 0);
			HANDLE thread8 = CreateThread(0, 0, payload9, 0, 0, 0);
			Sleep(30000);
			TerminateThread(thread8, 0);
			CloseHandle(thread8);
			InvalidateRect(0, 0, 0);
			HANDLE thread9 = CreateThread(0, 0, profecttouhou, 0, 0, 0);
			HANDLE thread9dot1 = CreateThread(0, 0, line, 0, 0, 0);
			HANDLE thread9dot2 = CreateThread(0, 0, rect, 0, 0, 0);
			Sleep(30000);
		}
	}
}

