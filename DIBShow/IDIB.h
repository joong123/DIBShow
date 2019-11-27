#pragma once

#include <windows.h>
#include <gdiplus.h>

#include "MyType.h"
#include "InterfaceBase.h"


using Gdiplus::Bitmap;
using Gdiplus::BitmapData;
using Gdiplus::PixelFormat;


#define	SAFE_DELETE(p)			{if((p) != NULL) {delete (p); (p) = NULL;}}
#define	SAFE_DELETE_LIST(p)		{if((p) != NULL) {delete[] (p); (p) = NULL;}}
#define	SAFE_GLOBALDELETE(p)	{if((p) != NULL) {::delete (p); (p) = NULL;}}

// RECT
#define WIDTHOF(R)				((R).right - (R).left)
#define HEIGHTOF(R)				((R).bottom - (R).top)

// Pixel
// RGBA
#define RGBA(r, g, b, a)		((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))
// BGRA
#define RGBA_LE(r, g, b, a)		RGBA(b, g, r, a)
#define RGB_LE(r, g, b)			(RGBA_LE(r, g, b, 255))
#define COLOR_LE_BLACK			(RGB_LE(0, 0, 0))
#define COLOR_LE_RED			(RGB_LE(255, 0, 0))
#define COLOR_LE_GREEN			(RGB_LE(0, 255, 0))
#define COLOR_LE_BLUE			(RGB_LE(0, 0, 255))
#define COLOR_LE_WHITE			(RGB_LE(255, 255, 255))

#define SIZEBITMAPFILEHEADER	(sizeof(BITMAPFILEHEADER))
#define SIZEBITMAPINFOHEADER	(sizeof(BITMAPINFOHEADER))


typedef BITMAPFILEHEADER		BFH;
typedef BITMAPINFOHEADER		BIH;


//enum PIXELFORMAT {
//	PF_GRAY1,		// GRAY1 index
//	PF_GRAY2,		// ??? GRAY2 index
//	PF_GRAY4,		// GRAY4 index
//	PF_GRAY8,		// GRAY8 index
//	PF_GRAY16,		// ??? GRAY16
//	PF_X1R5G5B5,	// RGB16
//	PF_A1R5G5B5,	// ???
//	PF_R5G6B5,
//	PF_X4R4G4B4,
//	PF_A4R4G4B4,	// ???
//	PF_R8G8B8,		// RGB24
//	PF_X8R8G8B8,	// RGB32	
//	PF_A8R8G8B8		// ???
//};

enum DRAWMODE {
	DRAWMODE_COPY,
	DRAWMODE_BLEND,
	DRAWMODE_CUSTOM1
};

enum COPYMODE {
	CM_NORMAL,
	CM_REVH,
	CM_REVV,
	CM_REVHV
};

enum RESIZEMODE {
	RM_CENTER,
	RM_TOPCENTER,
	RM_BOTTOMCENTER,
	RM_CENTERLEFT,
	RM_CENTERIGHT,
	RM_TOPLEFT,
	RM_TOPRIGHT,
	RM_BOTTOMLEFT,
	RM_BOTTOMRIGHT
};

enum SCALEMODE {
	SM_NN,
	SM_SINGLE,
	SM_BILINEAR,
	SM_CUBE,
	SM_AREA
};


//////////////////////////////////////////////////////////////////////////////
// IImg
//////////////////////////////////////////////////////////////////////////////

typedef interface IImg IImg;
typedef interface IImg *LPImg;

DEFINE_GUID(IID_IImg,
	0x7EEC9642, 0x0E29, 0x4303, 0x93, 0x12, 0x43, 0x1C, 0xC7, 0xDE, 0xE5, 0x84);

#undef INTERFACE
#define INTERFACE IImg

DECLARE_INTERFACE_(IImg, IUnknown)
{

#pragma region Inknown

	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

#pragma endregion

#pragma region Basic Operation

	STDMETHOD_(bool,	IsEmpty)	(THIS) CONST PURE;
	STDMETHOD_(bool,	ClearImg)	(THIS) PURE;
	STDMETHOD_(bool,	Clear)		(THIS) PURE;
	STDMETHOD_(BIH,		GetInfoHeader)(THIS) CONST PURE;

	STDMETHOD_(byte*,	CloneData)	(THIS) CONST PURE;
	STDMETHOD_(BOOL,	Build)		(THIS_ int w, int h, bool bAlpha, DWORD color) PURE;

	//STDMETHOD_(BOOL,	Load)		(THIS_ UINT nIDResource) PURE;
	//STDMETHOD_(BOOL,	Load)		(THIS_ HDC hDC, HBITMAP hBMP) PURE;
	STDMETHOD_(BOOL,	LoadStandard)(THIS_ Bitmap *) PURE;
	STDMETHOD_(BOOL,	Clone)		(THIS_ Bitmap *) PURE;

	//STDMETHOD_(BOOL,	Clone)		(THIS_ const IImg &) PURE;// (not overridable)
	//STDMETHOD_(BOOL,	Clone)		(THIS_ const IImg *) PURE;// (not overridable)

#pragma endregion

#pragma region Property Methods

	STDMETHOD_(int,		GetWidth)	(THIS) CONST PURE;
	STDMETHOD_(int,		GetHeight)	(THIS) CONST PURE;
	STDMETHOD_(byte*,	GetPImg)	(THIS) CONST PURE;
	STDMETHOD_(UINT,	GetStride)	(THIS) CONST PURE;
	STDMETHOD_(UINT,	GetPadding)	(THIS) CONST PURE;

#pragma endregion

#pragma region File

	STDMETHOD_(BOOL,	LoadStandard)(THIS_ const WCHAR *) PURE;

	STDMETHOD_(BOOL,	Save)		(THIS_ const WCHAR *) CONST PURE;

#pragma endregion

#pragma region Operation

	//STDMETHOD_(BOOL,	Fill)		(THIS_ int _xSrc, int _ySrc, int _wSrc, int _hSrc, DWORD color) CONST PURE;
	STDMETHOD_(BOOL,	Gray)		(THIS) PURE;
	STDMETHOD_(int*,	GetGrayBuf)	(THIS) CONST PURE;
	STDMETHOD_(BOOL,	Erode)		(THIS) PURE;
	STDMETHOD_(BOOL,	Dilate)		(THIS) PURE;
	STDMETHOD_(BOOL,	Rect)		(THIS_ const RECT &, DWORD color) PURE;

	//STDMETHOD_(BOOL,	Resize)		(THIS_ UINT w, UINT h, DWORD color) PURE;
	//STDMETHOD_(BOOL,	Resize)		(THIS_ UINT w, UINT h, RESIZEMODE rm, DWORD color) PURE;

	STDMETHOD_(BOOL,	Scale)		(THIS_ UINT w, UINT h, SCALEMODE sm) PURE;

	//STDMETHOD_(BOOL,	Rotate)		(THIS_ double angle, SCALEMODE sm) PURE;

	STDMETHOD_(BOOL,	Draw)		(THIS_ const CDC *, int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc) CONST PURE;
	STDMETHOD_(BOOL,	DrawStretch)(THIS_ const CDC *, int _xDest , int _yDest, int _wDest, int _hDest, int mode) CONST PURE;

#pragma endregion

};


#undef INTERFACE// TODO