#pragma once

#include "IDIB.h"

#include "ticker.h"


// 拷贝图片
// 目标图和源图 nPixelBits 需相等
// int wd, hd, ws, hs：目标图、源图宽、高
// int xDest, yDest, xSrc, ySrc：目标图、源图拷贝基准点
//	参数要求：xDest, yDest 任意
//	参数要求：0 <= xSrc < width, 0 <= ySrc < height
// int wSrc, hSrc：源图拷贝宽、高
//	参数要求：0 < wSrc, 0 < hSrc (-1表示全图)
// DRAWMODE mode：拷贝模式
// TODO: 参数用padding代替stride
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

	virtual void STDCALL		ClearParam();	// 参数清除
	virtual void STDCALL		ParamDefault();	// 恢复默认参数
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
	 specifications 计算
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

	bool STDCALL			IsEmpty() const OVEERIDE;// 判断是否有图片
	bool STDCALL			ClearImg() OVEERIDE;	// 清除图片
	bool STDCALL			Clear() OVEERIDE;		// 清除所有
	BIH STDCALL				GetInfoHeader() const OVEERIDE;

	//
	// Build & Load/Unload
	//
	byte* STDCALL			CloneData() const OVEERIDE;
	BOOL STDCALL			Build(int w, int h, bool bAlpha = false, DWORD color = COLOR_LE_BLACK) OVEERIDE;

	//BOOL STDCALL			Load(UINT nIDResource) OVEERIDE;// 从资源加载
	//BOOL STDCALL			Load(HDC hDC, HBITMAP hBMP) OVEERIDE;// 从 HBITMAP 加载
	BOOL STDCALL			LoadStandard(Bitmap *) OVEERIDE;// 从 Bitmap 读图，建立统一 DIB 格式: 32bits RGBA
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

	BOOL STDCALL			LoadStandard(const WCHAR *pszFilename) OVEERIDE;// 从文件读图，建立统一 DIB 格式: 32bits RGBA

	BOOL STDCALL			Save(const WCHAR *pszFilename) const OVEERIDE;// 保存文件

#pragma endregion

#pragma region Operation

	//BOOL STDCALL			Fill(int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1, DWORD color = COLOR_LE_BLACK) OVEERIDE;
	BOOL STDCALL			Gray() OVEERIDE;
	int* STDCALL			GetGrayBuf() const OVEERIDE;
	BOOL STDCALL			Erode() OVEERIDE;
	BOOL STDCALL			Dilate() OVEERIDE;
	BOOL STDCALL			Rect(const RECT &rc, DWORD color = COLOR_LE_WHITE) OVEERIDE;

	// 重设尺寸
	//BOOL STDCALL			Resize(UINT w, UINT h, DWORD color) OVEERIDE;
	//BOOL STDCALL			Resize(UINT w, UINT h, RESIZEMODE rm = RM_CENTER, DWORD color = COLOR_LE_BLACK)) OVEERIDE;
	// 缩放
	BOOL STDCALL			Scale(UINT w, UINT h, SCALEMODE sm = SM_NN) OVEERIDE;
	// 旋转
	//BOOL STDCALL			Rotate(double angle, SCALEMODE sm) OVEERIDE;

#pragma endregion

#pragma region Render Operation

	BOOL STDCALL			Draw(const CDC *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const OVEERIDE;// 绘制到DC
	BOOL STDCALL			DrawStretch(const CDC *, int _xDest = 0, int _yDest = 0, int _wDest = -1, int _hDest = -1, int mode = SRCCOPY) const OVEERIDE;// 绘制到DC（匹配拉伸）

#pragma endregion

protected:
	byte			*pImg;		// 图像数据指针

	/*****
	 参数
	*****/
	int				width;		// 长宽
	int				height;
	WORD			bits;		// 像素位数
	DWORD			size;

	/*****
	 specifications
	*****/
	UINT			stride;		// stride
	UINT			padding;	// padding
	bool			bAlpha;		// 图片是否启用透明通道
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
	virtual bool STDCALL	DetachDC();		// 脱离 DC
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

	// 重设尺寸
	//BOOL STDCALL			Resize(UINT w, UINT h, DWORD color) OVEERIDE;
	//BOOL STDCALL			Resize(UINT w, UINT h, RESIZEMODE rm = RM_CENTER, DWORD color = COLOR_LE_BLACK) OVEERIDE;
	// 缩放
	//BOOL STDCALL			Scale(UINT w, UINT h, SCALEMODE sm) OVEERIDE;
	// 旋转
	//BOOL STDCALL			Rotate(double angle, SCALEMODE sm) OVEERIDE;

#pragma endregion

#pragma region Render Operation

	virtual BOOL STDCALL	SetDC(CDC *, int w = -1, int h = -1);// 设置 DC
	virtual void STDCALL	SetDCSize(int w, int h);// 设置 DC 尺寸
	virtual void STDCALL	SetDCDrawPos(int _xDest, int _yDest, int _xSrc = -2, int _ySrc = -2, int _wSrc = -2, int _hSrc = -2);// 设置 DC 绘制坐标、尺寸
	virtual CDC* STDCALL	GetDC() const;	// 获取 DC

	virtual BOOL STDCALL	Draw() const;// 根据内部参数，绘制到已有 DC
	virtual BOOL STDCALL	Draw(int _xDest, int _yDest, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const;// 绘制到内部 DC
	BOOL STDCALL			Draw(const CDC *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1) const OVEERIDE;
	BOOL STDCALL			DrawStretch(const CDC *, int _xDest = 0, int _yDest = 0, int _wDest = -1, int _hDest = -1, int mode = SRCCOPY) const OVEERIDE;
	virtual BOOL STDCALL	Draw(const DibBase *, int _xDest = 0, int _yDest = 0, int _xSrc = 0, int _ySrc = 0, int _wSrc = -1, int _hSrc = -1, DRAWMODE mode = DRAWMODE_COPY) const;// 绘制到 DIB

#pragma endregion

protected:
	/*****
	 DC （绘制相关）#01 ~ #11
	*****/
	CDC				*pDCDraw;		// 设定的绘图 DC

	int				sizeDCWidth;	// DC 尺寸（宽）
	int				sizeDCHeight;	// DC 尺寸（高）
	int				xDest;			// 绘制到 DC 上的坐标、尺寸
	int				yDest;
	int				xSrc;
	int				ySrc;
	int				wSrc;			// wSrc, hSrc 参数值（-1表示全长）
	int				hSrc;
	int				wDraw;			// wSrc, hSrc 最终值
	int				hDraw;
};