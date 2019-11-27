#include "stdafx.h"

#include "DIBExt.h"


void Dib::ClearParam()
{
	DibBase::ClearParam();

	ZeroMemory(&infoHeader, SIZEBITMAPINFOHEADER);
	nPaletteEntries = 0;
	if (palette.GetSafeHandle() != NULL)
		palette.DeleteObject();
}

void Dib::ParamDefault()
{
	DibBase::ParamDefault();

	infoHeader = GetInfoHeader();

	nPaletteEntries = 0;
	if (palette.GetSafeHandle() != NULL)
		palette.DeleteObject();
}

bool Dib::SetImgInternal(const byte * pData, WORD nPixelBits, int w, int h, bool alpha)
{
	if (!DibBase::SetImgInternal(pData, nPixelBits, w, h, alpha))
		return false;

	SetImgInternalExtend(GetInfoHeader());

	return false;
}

bool Dib::SetImgInternalExtend(const BIH & BIH)
{
	infoHeader = BIH;

	nPaletteEntries = CalcPaletteEntries(bits, infoHeader.biClrUsed);
	pBits = &pImg[nPaletteEntries * sizeof(RGBQUAD)];
	pPalette = (RGBQUAD*)pImg;

	if (palette.GetSafeHandle() != NULL)
		palette.DeleteObject();
	// 如果位图中存在调色板，创建LOGPALETTE 及CPalette
	if (nPaletteEntries != 0)
	{
		LOGPALETTE *pLogPal = (LOGPALETTE*)new char[sizeof(LOGPALETTE) + nPaletteEntries * sizeof(PALETTEENTRY)];

		if (pLogPal != NULL)
		{
			pLogPal->palVersion = 0x300;
			pLogPal->palNumEntries = nPaletteEntries;

			for (UINT i = 0; i < nPaletteEntries; i++)
			{
				pLogPal->palPalEntry[i].peRed = pPalette[i].rgbRed;
				pLogPal->palPalEntry[i].peGreen = pPalette[i].rgbGreen;
				pLogPal->palPalEntry[i].peBlue = pPalette[i].rgbBlue;
			}

			// 创建CPalette并释放LOGPALETTE的内存
			palette.CreatePalette(pLogPal);
			delete[] pLogPal;
		}
	}

	return true;
}

Dib::Dib() :
	pBits(NULL)
	, pPalette(NULL)
	, nPaletteEntries(0)
{
}

Dib::~Dib()
{
	Clear();
}

bool Dib::ClearImg()
{
	if (IsEmpty())
		return false;


	pBits = NULL;
	pPalette = NULL;

	DibBase::ClearImg();

	return true;
}

bool Dib::Clear()
{
	return DibBase::Clear();
}

BOOL Dib::Load(const WCHAR *pszFilename)
{
	CFile cf;

	// 打开位图文件
	if (!cf.Open(pszFilename, CFile::modeRead))
		return (FALSE);

	const UINT lenBMPFileHeader = SIZEBITMAPFILEHEADER;
	const UINT lenBMPInfoHeader = SIZEBITMAPINFOHEADER;

	// 获得位图文件大小 TODO:图像指针是否需包含调色板和掩码表
	DWORD sizeDIB = (DWORD)(cf.GetLength() - lenBMPFileHeader - lenBMPInfoHeader);
	byte *pImgNew = CreateImgSpace(sizeDIB);
	if (pImgNew == NULL)
		return false;

	BFH BFH;
	BIH BIH;

	// 读取位图文件数据
	try
	{
		// 文件格式是否正确有效
		UINT nReadFileHeader = cf.Read(&BFH, lenBMPFileHeader);
		UINT nReadInfoHeader = cf.Read(&BIH, lenBMPInfoHeader);
		UINT nReadDib = cf.Read(pImgNew, sizeDIB);
		if (nReadFileHeader != lenBMPFileHeader
			|| BFH.bfType != 'MB'
			|| nReadInfoHeader != lenBMPInfoHeader
			|| nReadDib != sizeDIB)
		{
			delete[] pImgNew;
			return (FALSE);
		}
	}
	catch (CFileException *e)
	{
		e->Delete();
		delete[] pImgNew;
		return (FALSE);
	}

	// 赋值
	SetImgInternal(pImgNew, BIH.biBitCount, BIH.biWidth, BIH.biHeight, false);

	// Dib::SetImgInternal不具备设置所有参数的能力，需要加SetImgInternalExtend
	SetImgInternalExtend(BIH);

	return (TRUE);
}

BOOL Dib::LoadStandard(const WCHAR *pszFilename)
{
	return DibBase::LoadStandard(pszFilename);
}

BOOL Dib::LoadStandard(Bitmap *pBmp)
{
	return DibBase::LoadStandard(pBmp);
}

BOOL Dib::Clone(Bitmap *pBmp)
{
	return DibBase::Clone(pBmp);
}

BOOL Dib::Clone(const Dib &pSrc)
{
	return Clone(&pSrc);
}

BOOL Dib::Clone(const Dib *pSrc)
{
	if (DibBase::Clone(pSrc) != TRUE)
		return FALSE;

	SetImgInternalExtend(pSrc->infoHeader);

	return 0;
}

BOOL Dib::Save(const WCHAR *pszFilename) const
{
	if (IsEmpty())
		return FALSE;

	CFile cf;
	if (!cf.Open(pszFilename, CFile::modeCreate | CFile::modeWrite))
		return FALSE;


	const UINT lenBMPFileHeader = SIZEBITMAPFILEHEADER;
	const UINT lenBMPInfoHeader = SIZEBITMAPINFOHEADER;
	UINT lenPalette = nPaletteEntries * sizeof(RGBQUAD);
	UINT lenPixelMask = 0;// TODO:颜色掩码长度是否包含在调色板长度中

	BFH BFH;
	memset(&BFH, 0, SIZEBITMAPFILEHEADER);
	BFH.bfType = 'MB';
	BFH.bfSize = lenBMPFileHeader + lenBMPInfoHeader + lenPalette + size;
	BFH.bfOffBits = lenBMPFileHeader + lenBMPInfoHeader + lenPalette + lenPixelMask;

	try
	{
		cf.Write(&BFH, lenBMPFileHeader);
		cf.Write(&infoHeader, lenBMPInfoHeader);
		cf.Write(pImg, size);
	}
	catch (CFileException *e)
	{
		e->Delete();
		return FALSE;
	}

	return TRUE;
}

BOOL Dib::SetPalette(CDC *pDC)
{
	if (IsEmpty())
		return FALSE;

	// 检查当前是否有一个调色板句柄，对于大于256色的位图，为NULL
	if (palette.GetSafeHandle() == NULL)
		return TRUE;

	// 选择调色板，接着实施之，最后恢复老的调色板
	CPalette *pOldPalette;
	pOldPalette = pDC->SelectPalette(&palette, FALSE);
	pDC->RealizePalette();
	pDC->SelectPalette(pOldPalette, FALSE);

	return TRUE;
}