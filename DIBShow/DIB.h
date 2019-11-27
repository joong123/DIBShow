#pragma once

#include "IDIB.h"

#include "ticker.h"


// ����ͼƬ
// Ŀ��ͼ��Դͼ nPixelBits �����
// int wd, hd, ws, hs��Ŀ��ͼ��Դͼ����
// int xDest, yDest, xSrc, ySrc��Ŀ��ͼ��Դͼ������׼��
//	����Ҫ��xDest, yDest ����
//	����Ҫ��0 <= xSrc < width, 0 <= ySrc < height
// int wSrc, hSrc��Դͼ��������
//	����Ҫ��0 < wSrc, 0 < hSrc (-1��ʾȫͼ)
// DRAWMODE mode������ģʽ
// TODO: ������padding����stride
bool CopyPic(byte *pDest, const byte *pSrc
	, int wd, int hd, int ws, int hs
	, UINT strided, UINT strides, int xDest = 0, int yDest = 0
	, int xSrc = 0, int ySrc = 0, int wSrc = -1, int hSrc = -1
	, WORD bits = 32, DRAWMODE mode = DRAWMODE_COPY, COPYMODE = CM_NORMAL);

bool CopyPicStandard(byte *pDest, const byte *pSrc
	, int wd, int hd, int ws, int hs
	, int xDest = 0, int yDest = 0, int xSrc = 0, int ySrc = 0
	, int wSrc = -1, int hSrc = -1
	, DRAWMODE mode = DRAWMODE_COPY, COPYMODE = CM_NORMAL);

class Img : public IImg
{
protected:

#pragma region Helper Methods

	virtual void STDCALL		ClearParam();	// �������
	virtual void STDCALL		ParamDefault();	// �ָ�Ĭ�ϲ���
	virtual bool STDCALL		SetImgInternal(const byte *pData, WORD nPixelBits, int w, int h, bool bAlpha = false);
	virtual bool STDCALL		SetImgFromInternal(const byte *pSrc, WORD nPixelBits, int w, int h, bool bAlpha = false, COPYMODE cm = CM_NORMAL);

	virtual inline bool STDCALL PointInsideImg(int x, int y) const
	{
		if (IsEmpty()) return false;
		return (x >= 0 && x < width && y >= 0 && y < height);
	}

#pragma endregion

#pragma region Helper Methods(Static)

	static inline byte* STDCALL CreateImgSpace(DWORD sizeData)
	{
		byte *pImgNew = new byte[sizeData];
		if (pImgNew == NULL)
			return NULL;
		ZeroMemory(pImgNew, sizeData);

		return pImgNew;
	}

#pragma endregion

public:

#pragma region Static Basic Methods

	/*****
	 specifications ����
	*****/

	static inline UINT STDCALL	CalcPixelBytes(WORD nPixelBits)
	{
		return (nPixelBits + 7) / 8;
	}
	static inline UINT STDCALL	CalcBytePerWidth(int width, WORD nPixelBits)
	{
		if (width <= 0 || nPixelBits == 0)
			return 0;

		UINT nBytePerWidth = (width * nPixelBits + 7) >> 3;

		return nBytePerWidth;
	}
	static inline UINT STDCALL	CalcStride(int width, WORD nPixelBits)
	{
		if (width <= 0 || nPixelBits == 0)
			return 0;

		// TODO
		//UINT nBytesPerLine = (width * nPixelBits + 7) >> 3;
		//nBytesPerLine = (nBytesPerLine + 3) / 4 * 4;
		// /32*4
		UINT nBytesPerLine = ((((width * nPixelBits) + 31) & (~31)) / 8);
		return nBytesPerLine;
	}

#pragma endregion

#pragma region Ctor, DTor

	STDCALL			Img();
	virtual STDCALL	~Img();

#pragma endregion

#pragma region IUnknown Methods

	HRESULT STDCALL	QueryInterface(const IID &riid, void** ppvObj);
	ULONG STDCALL		AddRef();
	ULONG STDCALL		Release();

#pragma endregion

#pragma region Basic Operation

	bool STDCALL			IsEmpty() const OVEERIDE;// �ж��Ƿ���ͼƬ
	bool STDCALL			ClearImg() OVEERIDE;	// ���ͼƬ
	bool STDCALL			Clear() OVEERIDE;		// �������
	BIH STDCALL				GetInfoHeader() const OVEERIDE;

	//
	// Build & Load/Unload
	//
	byte* STDCALL			CloneData() const OVEERIDE;
	BOOL STDCALL			Build(int w, int h, bool bAlpha = false, DWORD color = COLOR_LE_BLACK) OVEERIDE;

	//BOOL STDCALL			Load(UINT nIDResource) OVEERIDE;// ����Դ����
	//BOOL STDCALL			Load(HDC hDC, HBITMAP hBMP) OVEERIDE;// �� HBITMAP ����
	BOOL STDCALL			LoadStandard(Bitmap *) OVEERIDE;// �� Bitmap ��ͼ������ͳһ DIB ��ʽ: 32bits RGBA
	BOOL STDCALL			Clone(Bitmap *) OVEERIDE;

	virtual BOOL STDCALL	Clone(const Img &);// (TODO: not overridable to DibBase)
	virtual BOOL STDCALL	Clone(const Img *);// (not overridable to DibBase)

#pragma endregion

#pragma region Property Methods

	inline int STDCALL		GetWidth() const OVEERIDE
	{
		if (IsEmpty()) return -1;
		return width;
	}
	inline int STDCALL		GetHeight() const OVEERIDE
	{
		if (IsEmpty()) return -1;
		return height;
	}
	inline byte* STDCALL	GetPImg() const OVEERIDE
	{
		if (IsEmpty()) return NULL;
		return pImg;
	}
	inline UINT STDCALL		GetStride() const OVEERIDE
	{
		if (IsEmpty()) return 0;
		return stride;
	}
	inline UINT STDCALL		GetPadding() const OVEERIDE
	{
		if (IsEmpty()) return 0;
		return padding;
	}

#pragma endregion

#pragma region File

	BOOL STDCALL			LoadStandard(const WCHAR *pszFilename) OVEERIDE;// ���ļ���ͼ������ͳһ DIB ��ʽ: 32bits RGBA

	BOOL STDCALL			Save(const WCHAR *pszFilename) const OVEERIDE;// �����ļ�

#pragma endregion

#pragma region Operation

	//BOOL STDCALL			Fill(int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1, DWORD color = COLOR_LE_BLACK) OVEERIDE;
	BOOL STDCALL			Gray() OVEERIDE;
	int* STDCALL			GetGrayBuf() const OVEERIDE;
	BOOL STDCALL			Erode() OVEERIDE;
	BOOL STDCALL			Dilate() OVEERIDE;
	BOOL STDCALL			Rect(const RECT &rc, DWORD color = COLOR_LE_WHITE) OVEERIDE;

	// ����ߴ�
	//BOOL STDCALL			Resize(UINT w, UINT h, DWORD color) OVEERIDE;
	//BOOL STDCALL			Resize(UINT w, UINT h, RESIZEMODE rm = RM_CENTER, DWORD color = COLOR_LE_BLACK)) OVEERIDE;
	// ����
	BOOL STDCALL			Scale(UINT w, UINT h, SCALEMODE sm = SM_NN) OVEERIDE;
	// ��ת
	//BOOL STDCALL			Rotate(double angle, SCALEMODE sm) OVEERIDE;

#pragma endregion

#pragma region Render Operation

	BOOL STDCALL			Draw(const CDC *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const OVEERIDE;// ���Ƶ�DC
	BOOL STDCALL			DrawStretch(const CDC *, int _xDest = 0, int _yDest = 0, int _wDest = -1, int _hDest = -1, int mode = SRCCOPY) const OVEERIDE;// ���Ƶ�DC��ƥ�����죩

#pragma endregion

protected:
	byte			*pImg;		// ͼ������ָ��

	/*****
	 ����
	*****/
	int				width;		// ����
	int				height;
	WORD			bits;		// ����λ��
	DWORD			size;

	/*****
	 specifications
	*****/
	UINT			stride;		// stride
	UINT			padding;	// padding
	bool			bAlpha;		// ͼƬ�Ƿ�����͸��ͨ��
};

class DibBase : public Img
{
protected:

#pragma region Helper Methods

	//void STDCALL			ClearParam() OVEERIDE;
	//void STDCALL			ParamDefault() OVEERIDE;
	//bool STDCALL			SetImgInternal(const byte *pData, WORD nPixelBits, int w, int h, bool bAlpha = false) OVEERIDE;
	//bool STDCALL			SetImgFromInternal(const byte *pSrc, WORD nPixelBits, int w, int h, bool bAlpha = false, COPYMODE cm = CM_NORMAL) OVEERIDE;

	//virtual inline bool STDCALL PointInsideImg(int x, int y) const;

#pragma endregion

public:

#pragma region Ctor, DTor

	STDCALL					DibBase();
	STDCALL					~DibBase() OVEERIDE;

#pragma endregion

#pragma region Basic Operation

	//bool STDCALL			IsEmpty() const OVEERIDE;
	//bool STDCALL			ClearImg() OVEERIDE;
	virtual bool STDCALL	DetachDC();		// ���� DC
	bool STDCALL			Clear() OVEERIDE;
	//BIH STDCALL			GetInfoHeader() const OVEERIDE;

	//byte* STDCALL			CloneData() const OVEERIDE;
	//BOOL STDCALL			Build(int w, int h, bool bAlpha = false, DWORD color = COLOR_LE_BLACK) OVEERIDE;

	//BOOL STDCALL			Load(UINT nIDResource) OVEERIDE;
	//BOOL STDCALL			Load(HDC hDC, HBITMAP hBMP) OVEERIDE;
	//BOOL STDCALL			LoadStandard(Bitmap *) OVEERIDE;
	BOOL STDCALL			Clone(Bitmap *) OVEERIDE;// (override to prevent hiding)

	virtual BOOL STDCALL	Clone(const DibBase &);// TODO: to Img
	virtual BOOL STDCALL	Clone(const DibBase *);// TODO: to Img

#pragma endregion

#pragma region Property Methods

	//inline int STDCALL	GetWidth() const OVEERIDE;
	//inline int STDCALL	GetHeight() const OVEERIDE;
	//inline byte* STDCALL	GetPImg() const OVEERIDE;
	//inline UINT STDCALL	GetStride() const OVEERIDE;
	//inline UINT STDCALL	GetPadding() const OVEERIDEOVEERIDE;

#pragma endregion

#pragma region File

	//BOOL STDCALL			LoadStandard(const WCHAR *) OVEERIDE;

	//BOOL STDCALL			Save(const WCHAR *) const OVEERIDE;

#pragma endregion

#pragma region Operation

	//BOOL STDCALL			Fill(int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1, DWORD color = COLOR_LE_BLACK) OVEERIDE;
	//BOOL STDCALL			Gray() OVEERIDE;
	//int* STDCALL			GetGrayBuf() const OVEERIDE;
	//BOOL STDCALL			Erode() OVEERIDE;
	//BOOL STDCALL			Dilate() OVEERIDE;
	//BOOL STDCALL			Rect(const RECT &, DWORD color = RGBA_LE(255, 255, 255, 255)) OVEERIDE;

	// ����ߴ�
	//BOOL STDCALL			Resize(UINT w, UINT h, DWORD color) OVEERIDE;
	//BOOL STDCALL			Resize(UINT w, UINT h, RESIZEMODE rm = RM_CENTER, DWORD color = COLOR_LE_BLACK) OVEERIDE;
	// ����
	//BOOL STDCALL			Scale(UINT w, UINT h, SCALEMODE sm) OVEERIDE;
	// ��ת
	//BOOL STDCALL			Rotate(double angle, SCALEMODE sm) OVEERIDE;

#pragma endregion

#pragma region Render Operation

	virtual BOOL STDCALL	SetDC(CDC *, int w = -1, int h = -1);// ���� DC
	virtual void STDCALL	SetDCSize(int w, int h);// ���� DC �ߴ�
	virtual void STDCALL	SetDCDrawPos(int _xDest, int _yDest, int _xSrc = -2, int _ySrc = -2, int _wSrc = -2, int _hSrc = -2);// ���� DC �������ꡢ�ߴ�
	virtual CDC* STDCALL	GetDC() const;	// ��ȡ DC

	virtual BOOL STDCALL	Draw() const;// �����ڲ����������Ƶ����� DC
	virtual BOOL STDCALL	Draw(int _xDest, int _yDest, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const;// ���Ƶ��ڲ� DC
	BOOL STDCALL			Draw(const CDC *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const OVEERIDE;
	BOOL STDCALL			DrawStretch(const CDC *, int _xDest = 0, int _yDest = 0, int _wDest = -1, int _hDest = -1, int mode = SRCCOPY) const OVEERIDE;
	virtual BOOL STDCALL	Draw(const DibBase *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1, DRAWMODE mode = DRAWMODE_COPY) const;// ���Ƶ� DIB

#pragma endregion

protected:
	/*****
	 DC ��������أ�#01 ~ #11
	*****/
	CDC				*pDCDraw;		// �趨�Ļ�ͼ DC

	int				sizeDCWidth;	// DC �ߴ磨��
	int				sizeDCHeight;	// DC �ߴ磨�ߣ�
	int				xDest;			// ���Ƶ� DC �ϵ����ꡢ�ߴ�
	int				yDest;
	int				xSrc;
	int				ySrc;
	int				wSrc;			// wSrc, hSrc ����ֵ��-1��ʾȫ����
	int				hSrc;
	int				wDraw;			// wSrc, hSrc ����ֵ
	int				hDraw;
};