#include "stdafx.h"

#include "DIB.h"

bool CopyPic(byte *pDest, const byte *pSrc, int wd, int hd, int ws, int hs, UINT strided, UINT strides, int xDest, int yDest, int xSrc, int ySrc, int wSrc, int hSrc, WORD bits, DRAWMODE dm, COPYMODE cm)
{
	if (pDest == NULL || pSrc == NULL)
		return false;

	if (pDest == pSrc)
		return true;

	// stride 限制
	if (strided == 0 || strides == 0)
		return false;

	// 源图、目标图高、宽限制
	if (wd <= 0 || hd <= 0 || ws <= 0 || hs <= 0)
		return false;

	// 源图拷贝基准点限制
	if (xSrc < 0 || xSrc >= ws || ySrc < 0 || ySrc >= hs)
		return false;

	// 源图拷贝宽、高限制
	if (wSrc == -1)
		wSrc = ws;
	if (hSrc == -1)
		hSrc = hs;
	if (wSrc <= 0 || hSrc <= 0)
		return false;


	// 源图拷贝宽、高修正
	if (xSrc + wSrc > ws)
		wSrc = ws - xSrc;
	if (ySrc + hSrc > hs)
		hSrc = hs - ySrc;

	// 计算拷贝矩形
	RECT rcDest = { 0, 0, wd, hd };
	RECT rcCopyDest = { xDest, yDest, xDest + wSrc, yDest + hSrc };
	BOOL rs = IntersectRect(&rcDest, &rcDest, &rcCopyDest);
	if (rs == 0)
		return false;


	// 源图在目标图中的拷贝基准点
	int xCopyDest = rcDest.left;
	int yCopyDest = rcDest.top;
	int xCopySrc = xSrc + rcDest.left - rcCopyDest.left;
	int yCopySrc = ySrc + rcDest.top - rcCopyDest.top;
	int wCopy = WIDTHOF(rcDest);
	int hCopy = HEIGHTOF(rcDest);

	// 拷贝数据
	const UINT nPixelBytes = Img::CalcPixelBytes(bits);
	byte *pDestRow = pDest + strided*(yCopyDest) + nPixelBytes * xCopyDest;
	byte *pDestRowEnd = pDest + strided*(yCopyDest + hCopy - 1) + nPixelBytes * xCopyDest;
	byte *pSrcRow = (byte*)pSrc + strides*(yCopySrc) + nPixelBytes * xCopySrc;
	byte *pSrcRowEnd = (byte*)pSrc + strides*(yCopySrc + hCopy - 1) + nPixelBytes * xCopySrc;
	size_t nMoveRow = nPixelBytes * wCopy;

	byte *temp = NULL;
	int y = 0;
	switch (cm)
	{
	case CM_NORMAL:
		for (; pDestRow <= pDestRowEnd && pSrcRow <= pSrcRowEnd;)
		{
			memmove_s(pDestRow, nMoveRow, pSrcRow, nMoveRow);
			pDestRow += strided;
			pSrcRow += strides;
			y++;
		}
		break;
	case CM_REVV:
		pDestRow = pDest + strided*(hd - (yCopyDest + hCopy - 1) - 1) + nPixelBytes * xCopyDest;
		pDestRowEnd = pDest + strided*(hd - yCopyDest - 1) + nPixelBytes * xCopyDest;

		temp = pDestRow;
		pDestRow = pDestRowEnd;
		pDestRowEnd = temp;
		for (; pDestRow >= pDestRowEnd && pSrcRow <= pSrcRowEnd;)
		{
			memmove_s(pDestRow, nMoveRow, pSrcRow, nMoveRow);
			pDestRow -= strided;
			pSrcRow += strides;
		}
		break;
	default:
		break;
	}

	return true;
}

void Img::ClearParam()
{
	width	= 0;
	height	= 0;
	bits	= 0;
	size	= 0;

	stride	= 0;
	padding = 0;
	bAlpha	= false;
}

void Img::ParamDefault()
{
	ClearParam();
}

bool Img::SetImgInternal(const byte *pData, WORD nPixelBits, int w, int h, bool alpha)
{
	if (pData == NULL)
		return false;

	if (w <= 0 || h <= 0)
		return false;

	if (nPixelBits == 0)
		return false;


	// 设置位图数据
	SAFE_DELETE_LIST(pImg);
	pImg = (byte *)pData;

	// 参数设置
	ParamDefault();

	// 参数计算
	stride = CalcStride(w, nPixelBits);

	width	= w;
	height	= h;
	bits	= nPixelBits;
	size	= stride * h;

	const int sizeRow = CalcBytePerWidth(w, nPixelBits);
	padding = (stride - sizeRow < 0) ? 0 : stride - sizeRow;
	bAlpha	= alpha;

	return true;
}

bool Img::SetImgFromInternal(const byte *pData, WORD nPixelBits, int w, int h, bool alpha, COPYMODE cm)
{
	if (pData == NULL)
		return false;

	if (w <= 0 || h <= 0)
		return false;

	if (nPixelBits == 0)
		return false;

	// params
	UINT iStride = CalcStride(w, nPixelBits);
	DWORD sizeDIB = iStride*h;

	// DIB 数据
	byte *pImgNew = CreateImgSpace(sizeDIB);
	if (pImgNew == NULL)
		return false;
	// 拷贝位图数据 到临时内存
	bool rs = CopyPic(pImgNew, pData, w, h, w, h, iStride, iStride
		, 0, 0, 0, 0, -1, -1, nPixelBits, DRAWMODE_COPY, cm);

	// 无透明通道情况下，设置透明通道值为 255
	/*if (!alpha)
	{
	for (rowDest = pDIBNew; rowDest <= rowDestFinal; rowDest += iStride)
	{
	const int xStart = 3;
	const int xEnd = nPixelBytes * (w - 1) + xStart;
	const int step = nPixelBytes;
	for (int i = xStart; i <= xEnd; i += step)
	{
	rowDest[i] = 255;
	}
	}
	}*/

	SetImgInternal(pImgNew, nPixelBits, w, h, alpha);

	return true;
}

Img::Img() :
	pImg		(NULL ),
	width		( 0	  ),
	height		( 0	  ),
	bits		( 0	  ),
	size		( 0	  ),
	stride		( 0	  ),
	padding		( 0	  ),
	bAlpha		(false)
{
}

Img::~Img()
{
	Clear();
}

HRESULT Img::QueryInterface(const IID & riid, void ** ppvObj)
{
	return S_OK;
}

ULONG Img::AddRef()
{
	return 0;
}

ULONG Img::Release()
{
	return 0;
}

bool Img::IsEmpty() const
{
	// TODO
	return (pImg == NULL
		/*|| width <= 0 || height <= 0 || size == 0*/);
}

bool Img::ClearImg()
{
	if (IsEmpty())
		return false;

	SAFE_DELETE_LIST(pImg);

	ClearParam();

	return true;
}

bool Img::Clear()
{
	bool rs = ClearImg();

	return rs;
}

BIH Img::GetInfoHeader() const
{
	BIH BIH;
	ZeroMemory(&BIH, SIZEBITMAPINFOHEADER);

	BIH.biSize			= SIZEBITMAPINFOHEADER;
	BIH.biWidth			= width;
	BIH.biHeight		= height;
	BIH.biPlanes		= 1;
	BIH.biBitCount		= bits;
	BIH.biCompression	= BI_RGB;
	BIH.biXPelsPerMeter = 2834;
	BIH.biYPelsPerMeter = 2834;
	BIH.biSizeImage		= size;
	BIH.biClrUsed		= 0;
	BIH.biClrImportant	= 0;

	return BIH;
}

BOOL Img::LoadStandard(const WCHAR *pszFilename)
{
	Bitmap *pPic = ::new Bitmap(pszFilename);

	BOOL rs = LoadStandard(pPic);
	SAFE_GLOBALDELETE(pPic);

	return rs;
}

BOOL Img::LoadStandard(Bitmap *pBmp)
{
	if (pBmp == NULL)
		return FALSE;

	Bitmap *pPic = pBmp->Clone(0, 0, pBmp->GetWidth(), pBmp->GetHeight(), PixelFormat32bppARGB);
	if (pPic == NULL)
		return FALSE;

	int w = pPic->GetWidth();
	int h = pPic->GetHeight();
	WORD nPixelBits = 32;///

						 // 锁定源图 Gdiplus::Bitmap
	BitmapData bmpData;
	Gdiplus::Rect rcLock = Gdiplus::Rect(0, 0, w, h);
	Gdiplus::Status sDest = pPic->LockBits(&rcLock, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

	if (sDest != Gdiplus::Ok/* || bmpData.Stride != iStride*/)
	{// bmpData.Stride != stride 判断 stride 计算结果是否匹配
		pPic->UnlockBits(&bmpData);

		return FALSE;
	}

	//bool alpha = (pPic->GetPixelFormat() & PixelFormatAlpha) > 0;
	bool rs = SetImgFromInternal((byte *)bmpData.Scan0, nPixelBits, w, h, true, CM_REVV);
	if (!rs)
	{
		pPic->UnlockBits(&bmpData);
		SAFE_DELETE(pPic);
		return FALSE;
	}

	pPic->UnlockBits(&bmpData);
	SAFE_DELETE(pPic);


	return TRUE;
}

BOOL Img::Clone(Bitmap *pBmp)
{
	if (pBmp == NULL)
		Clear();
	else
		return LoadStandard(pBmp);

	return TRUE;
}

BOOL Img::Clone(const Img &pSrc)
{
	return Clone(&pSrc);
}

BOOL Img::Clone(const Img *pSrc)
{
	if (pSrc == this || pSrc == NULL)
		return FALSE;

	byte *pTmp = pSrc->CloneData();
	if (pTmp == NULL)
		return FALSE;

	SetImgInternal(pTmp, pSrc->bits, pSrc->width, pSrc->height, pSrc->bAlpha);

	return TRUE;
}

BOOL Img::Save(const WCHAR *pszFilename) const
{
	if (IsEmpty())
		return FALSE;

	CFile cf;
	if (!cf.Open(pszFilename, CFile::modeCreate | CFile::modeWrite))
		return FALSE;


	const UINT lenBMPFileHeader = SIZEBITMAPFILEHEADER;
	const UINT lenBMPInfoHeader = SIZEBITMAPINFOHEADER;
	UINT lenPixelMask = 0;

	BFH BFH;
	memset(&BFH, 0, SIZEBITMAPFILEHEADER);
	BFH.bfType = 'MB';
	BFH.bfSize = lenBMPFileHeader + lenBMPInfoHeader + size;
	BFH.bfOffBits = lenBMPFileHeader + lenBMPInfoHeader + lenPixelMask;
	BIH BIH = GetInfoHeader();

	try
	{
		cf.Write(&BFH, lenBMPFileHeader);
		cf.Write(&BIH, lenBMPInfoHeader);
		cf.Write(pImg, size);
	}
	catch (CFileException *e)
	{
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

byte * Img::CloneData() const
{
	if (IsEmpty())
		return NULL;

	// DIB 数据
	byte *pImgNew = CreateImgSpace(size);
	if (pImgNew == NULL)
		return false;

	// 拷贝位图数据 到临时内存
	bool rs = CopyPic(pImgNew, pImg, width, height, width, height, stride, stride
		, 0, 0, 0, 0, -1, -1, bits, DRAWMODE_COPY);

	return pImgNew;
}

BOOL Img::Build(int w, int h, bool alpha, DWORD color)
{
	if (w <= 0 || h <= 0)
		return FALSE;

	// params
	const WORD nPixelBits = 32;///
	UINT iStride = CalcStride(w, nPixelBits);
	DWORD size = iStride*h;

	// DIB 数据
	byte *pImgNew = CreateImgSpace(size);
	if (pImgNew == NULL)
		return false;

	// specifications
	const WORD nPixelBytes = CalcPixelBytes(nPixelBits);
	const int sizeRow = CalcBytePerWidth(w, nPixelBits);

	// 设置位图数据
	byte *rowDest = pImgNew;
	byte *rowDestFinal = rowDest + iStride*(h - 1);
	for (; rowDest <= rowDestFinal;)
	{
		for (byte *row = rowDest; row < rowDest + sizeRow; row += nPixelBytes)
		{
			*((DWORD *)row) = color;
		}

		rowDest += iStride;
	}

	// 无透明通道情况下，设置透明通道值为 255
	/*if (!bA)
	{
	for (rowDest = pData; rowDest <= rowDestFinal; rowDest += iStride)
	{
	const int xStart = 3;
	const int xEnd = nPixelBytes * (w - 1) + xStart;
	const int step = nPixelBytes;
	for (int i = xStart; i <= xEnd; i += step)
	{
	rowDest[i] = 255;
	}
	}
	}*/

	SetImgInternal(pImgNew, nPixelBits, w, h, alpha);

	return TRUE;
}

BOOL Img::Gray()
{
	if (IsEmpty())
		return FALSE;

	// specifications
	const WORD nPixelBytes = CalcPixelBytes(bits);
	const int sizeRow = CalcBytePerWidth(width, bits);

	// pixel operation
	byte *rowDest = pImg;
	byte *rowDestFinal = rowDest + stride*(height - 1);
	for (; rowDest <= rowDestFinal;)
	{
		for (byte *row = rowDest; row < rowDest + sizeRow; row += nPixelBytes)
		{
			byte gray = (row[2] * 38 + row[1] * 75 + row[0] * 15) >> 7;
			//byte gray = (byte)(0.299f*row[2] + 0.587f*row[1] + 0.114f*row[0]);
			row[2] = row[1] = row[0] = gray;
		}

		rowDest += stride;
	}

	return TRUE;
}

int* Img::GetGrayBuf() const
{
	if (IsEmpty())
		return NULL;

	// greybuf 数据
	int *pBuf = new int[width*height];
	if (pBuf == NULL)
		return FALSE;

	// specifications
	const WORD nPixelBytes = CalcPixelBytes(bits);
	const int sizeRow = CalcBytePerWidth(width, bits);

	// pixel operation
	byte *rowSrc = pImg;
	byte *rowSrcFinal = rowSrc + stride*(height - 1);
	int *rowDest = pBuf;
	int *rowDestFinal = rowDest + width*(height - 1);
	for (; rowSrc <= rowSrcFinal;)
	{
		int *rowd = rowDest;
		for (byte *row = rowSrc; row < rowSrc + sizeRow; row += nPixelBytes)
		{
			*rowd = (row[2] * 38 + row[1] * 75 + row[0] * 15) >> 7;
			//*rowd = (int)(0.299f*row[2] + 0.587f*row[1] + 0.114f*row[0]);

			rowd++;
		}

		rowSrc += stride;
		rowDest += width;
	}

	return pBuf;
}

BOOL Img::Erode()
{
	if (IsEmpty())
		return FALSE;

	// TODO
	//Img *pSrc = new decltype(&this)();
	Img *pDest = this;
	Img *pSrc = new Img();
	pSrc->Clone(pDest);

	// params
	int w = width;
	int h = height;

	// pixel operation
	DWORD *rowDest = (DWORD*)pDest->pImg;
	DWORD *rowDestFinal = rowDest + w*(h - 1);
	DWORD *rowSrc = (DWORD*)pSrc->pImg;
	DWORD *rowSrcFinal = rowSrc + w*(h - 1);
	for (; rowSrc <= rowSrcFinal;)
	{
		for (DWORD *rows = rowSrc, *rowd = rowDest
			; rows < rowSrc + w;
			rows++, rowd++)
		{
			if (((*rows) & 0xFFF0) != RGBA_LE(0, 0, 0, 0))// TODO
			{
				if ((rows > rowSrc && *(rows - 1) == RGB_LE(0, 0, 0))
					|| (rowSrc > (DWORD*)pSrc->pImg && *(rows - w) == RGB_LE(0, 0, 0))
					|| (rows + 1 < rowSrc + w && *(rows + 1) == RGB_LE(0, 0, 0))
					|| (rowSrc < rowSrcFinal && *(rows + w) == RGB_LE(0, 0, 0)))
				{
					*((DWORD *)rowd) = RGB_LE(0, 0, 0);
				}
			}
		}

		rowSrc += w;
		rowDest += w;
	}

	if (pSrc != this)
	{
		SAFE_DELETE(pSrc);
	}

	return TRUE;
}

BOOL Img::Dilate()
{
	if (IsEmpty())
		return FALSE;

	Img *pDest = this;
	Img *pSrc = new Img;
	pSrc->Clone(pDest);

	// params
	int w = width;
	int h = height;

	// pixel operation
	DWORD *rowDest = (DWORD*)pDest->pImg;
	DWORD *rowDestFinal = rowDest + w*(h - 1);
	DWORD *rowSrc = (DWORD*)pSrc->pImg;
	DWORD *rowSrcFinal = rowSrc + w*(h - 1);
	for (; rowSrc <= rowSrcFinal;)
	{
		for (DWORD *rows = rowSrc, *rowd = rowDest
			; rows < rowSrc + w;
			rows++, rowd++)
		{
			if (((*rows) & 0xFFF0) != RGBA_LE(0, 0, 0, 0))
			{
				if (rows > rowSrc && *(rows - 1) == RGB_LE(0, 0, 0))
				{
					*(rowd - 1) = *rows;
				}
				if (rowSrc > (DWORD *)pSrc->pImg && *(rows - w) == RGB_LE(0, 0, 0))
				{
					*(rowd - w) = *rows;
				}
				if (rows + 1 < rowSrc + w && *(rows + 1) == RGB_LE(0, 0, 0))
				{
					*(rowd + 1) = *rows;
				}
				if (rowSrc < rowSrcFinal && *(rows + w) == RGB_LE(0, 0, 0))
				{
					*(rowd + w) = *rows;
				}
			}
		}

		rowSrc += w;
		rowDest += w;
	}

	if (pSrc != this)
	{
		SAFE_DELETE(pSrc);
	}

	return TRUE;
}

BOOL Img::Rect(const RECT &rc, DWORD color)
{
	if (IsEmpty())
		return FALSE;


	int w = width;
	int h = height;

	RECT rcN = rc;

	if (rcN.left < 0)
		rcN.left = 0;
	if (rcN.right >= w)
		rcN.right = w - 1;
	if (rcN.top < 0)
		rcN.top = 0;
	if (rcN.bottom >= h)
		rcN.bottom = h - 1;

	DWORD *row = (DWORD*)pImg + w*(rcN.top) + rcN.left;
	DWORD *rowEnd = row + WIDTHOF(rc);
	for (DWORD *pix = row; pix < rowEnd; pix++)
	{
		*pix = color;
	}
	DWORD *row2 = (DWORD*)pImg + w*(rcN.bottom) + rcN.left;
	DWORD *rowEnd2 = row2 + WIDTHOF(rc);
	for (DWORD *pix = row2; pix < rowEnd2; pix++)
	{
		*pix = color;
	}


	for (DWORD *pix = row; pix <= row2; pix += w)
	{
		*pix = color;
	}
	for (DWORD *pix = rowEnd; pix <= rowEnd2; pix += w)
	{
		*pix = color;
	}

	return TRUE;
}

BOOL Img::Scale(UINT w, UINT h, SCALEMODE sm)
{// TODO
	if (w == 0 || h == 0)
		return FALSE;

	Img *pDest = this;
	Img *pSrc = new Img();
	pSrc->Clone(pDest);

	pDest->Build(w, h, bAlpha);

	//

	return FALSE;
}

BOOL Img::Draw(const CDC *pDC, int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc) const
{
	if (pDC == NULL)
		return FALSE;

	if (IsEmpty())
		return FALSE;

	// 位图宽度和高度修正
	if (_wSrc == -1)
		_wSrc = width;
	else if (_wSrc <= 0)
		return FALSE;
	if (_hSrc == -1)
		_hSrc = height;
	else if (_hSrc <= 0)
		return FALSE;
	int ySrcStandard = height - (_ySrc + _hSrc);

	BIH BIH = GetInfoHeader();
	// 绘制位图
	int rs = SetDIBitsToDevice(pDC->m_hDC, _xDest, _yDest, _wSrc, _hSrc, _xSrc, ySrcStandard
		, 0, height, pImg, (BITMAPINFO *)(&BIH), BI_RGB);// TODO 1、2参数??

	return rs;
}

BOOL Img::DrawStretch(const CDC *pDC, int _xDest, int _yDest, int _wDest, int _hDest, int mode) const
{
	if (pDC == NULL)
		return false;

	if (IsEmpty())
		return (FALSE);


	// 目标区域宽、高修正
	if (_wDest == -1)
		_wDest = width;
	else if (_wDest <= 0)
		return FALSE;
	if (_hDest == -1)
		_hDest = height;
	else if (_hDest <= 0)
		return FALSE;

	BIH BIH = GetInfoHeader();
	// 绘制位图
	StretchDIBits(pDC->m_hDC, _xDest, _yDest, _wDest, _hDest, 0, 0
		, width, height
		, pImg, (BITMAPINFO*)(&BIH)
		, BI_RGB, mode);

	return TRUE;
}



DibBase::DibBase() :
	pDCDraw			(NULL),
	sizeDCWidth		( 0  ),
	sizeDCHeight	( 0  ),
	xDest			( 0  ),
	yDest			( 0  ),
	xSrc			( 0  ),
	ySrc			( 0  ),
	wSrc			( -1 ),
	hSrc			( -1 ),
	wDraw			( 0  ),
	hDraw			( 0  )
{
}

DibBase::~DibBase()
{
	Clear();
}

bool DibBase::DetachDC()
{
	if (pDCDraw == NULL)
		return false;

	pDCDraw			= NULL;
	sizeDCWidth		= 0;
	sizeDCHeight	= 0;
	xDest	= 0;
	yDest	= 0;
	xSrc	= 0;
	ySrc	= 0;
	wSrc	= -1;
	hSrc	= -1;

	wDraw	= 0;
	hDraw	= 0;

	return true;
}

bool DibBase::Clear()
{
	bool rs = Img::Clear();
	rs = rs || DetachDC();

	return rs;
}

BOOL DibBase::SetDC(CDC *pDC, int w, int h)
{
	pDCDraw = pDC;
	sizeDCWidth = w;
	sizeDCHeight = h;

	return (pDCDraw != NULL);
}

void DibBase::SetDCSize(int w, int h)
{
	sizeDCWidth = w;
	sizeDCHeight = h;
}

void DibBase::SetDCDrawPos(int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc)
{
	xDest = _xDest;
	yDest = _yDest;
	if (_xSrc != -2)
		xSrc = _xSrc;
	if (_ySrc != -2)
		ySrc = _ySrc;
	if (_wSrc != -2)
		wSrc = _wSrc;
	if (_hSrc != -2)
		hSrc = _hSrc;
}

CDC * DibBase::GetDC() const
{
	return pDCDraw;
}

BOOL DibBase::Clone(Bitmap *pBmp)
{
	return Img::Clone(pBmp);
}

BOOL DibBase::Clone(const DibBase &pSrc)
{
	return Clone(&pSrc);
}

BOOL DibBase::Clone(const DibBase *pSrc)
{
	if (Img::Clone(pSrc) != TRUE)
		return FALSE;

	pDCDraw			= pSrc->pDCDraw;
	sizeDCWidth		= pSrc->sizeDCWidth;
	sizeDCHeight	= pSrc->sizeDCHeight;
	xDest	= pSrc->xDest;
	yDest	= pSrc->yDest;
	xSrc	= pSrc->xSrc;
	ySrc	= pSrc->ySrc;
	wSrc	= pSrc->wSrc;
	hSrc	= pSrc->hSrc;
	wDraw	= pSrc->wDraw;
	hDraw	= pSrc->hDraw;

	return TRUE;
}

BOOL DibBase::Draw() const
{
	return Img::Draw(pDCDraw, xDest, yDest, xSrc, ySrc, wSrc, hSrc);
}

BOOL DibBase::Draw(int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc) const
{
	return Img::Draw(pDCDraw, _xDest, _yDest, _xSrc, _ySrc, _wSrc, _hSrc);
}

BOOL DibBase::Draw(const CDC *pDC, int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc) const
{
	return Img::Draw(pDC, _xDest, _yDest, _xSrc, _ySrc, _wSrc, _hSrc);
}

BOOL DibBase::DrawStretch(const CDC *pDC, int _xDest, int _yDest, int _wDest, int _hDest, int mode) const
{
	return Img::DrawStretch(pDC, _xDest, _yDest, _wDest, _hDest, mode);
}

BOOL DibBase::Draw(const DibBase *pDest, int _xDest, int _yDest, int _xSrc, int _ySrc, int _wSrc, int _hSrc, DRAWMODE mode) const
{
	if (IsEmpty())
		return FALSE;

	if (pDest == NULL || pDest->IsEmpty())
		return FALSE;

	if (!PointInsideImg(_xSrc, _ySrc))
		return FALSE;

	// 位图宽度和高度修正
	if (_wSrc == -1)
		_wSrc = width - _xSrc;
	else if (_wSrc <= 0)
		return FALSE;
	if (_hSrc == -1)
		_hSrc = height - _ySrc;
	else if (_hSrc <= 0)
		return FALSE;
	int yDestStandard = pDest->GetHeight() - _yDest - (_ySrc + _hSrc);

	return CopyPic(pDest->GetPImg(), pImg, pDest->GetWidth(), pDest->GetHeight(), GetWidth(), GetHeight()
		, pDest->GetStride(), stride
		, _xDest, yDestStandard, _xSrc, _ySrc, _wSrc, _hSrc
		, bits, mode);
}