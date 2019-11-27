#pragma once

#include "DIB.h"


// TODO：是否需要
class Dib : private DibBase
{
protected:
	virtual void __stdcall	ClearParam();	// (actual override)参数清除
	virtual void __stdcall	ParamDefault();	// (actual override)恢复默认参数
	virtual bool __stdcall	SetImgInternal(const byte *pData, WORD nPixelBits, int w, int h, bool bAlpha = false);
	//virtual bool __stdcall	SetImgFromInternal(const byte *pSrc, WORD nPixelBits, int w, int h, bool bAlpha = false, COPYMODE cm = CM_NORMAL);
	virtual bool __stdcall	SetImgInternalExtend(const BIH &BIH);

public:
	__stdcall		Dib();
	__stdcall		~Dib() override;

	/*****
	图像基本操作
	*****/
	//virtual bool	IsEmpty() const;// 判断是否有图片
	bool __stdcall	ClearImg() override;		// 清除图片
												//virtual bool	DetachDC();		// 脱离 DC
	bool __stdcall	Clear() override;		// 清除所有

											/*****
											指标计算
											*****/
	static inline UINT __stdcall CalcPaletteEntries(WORD nPixelBits, DWORD nClrUsed)
	{
		if (nPixelBits == 0)
			return 0;

		UINT nPaletteEntries = 1 << nPixelBits;
		if (nPixelBits > 8)
			nPaletteEntries = 0;
		else if (nClrUsed != 0)
			nPaletteEntries = nClrUsed;

		return nPaletteEntries;
	}

	/*****
	成员
	*****/
	inline byte* __stdcall	GetPBits() const
	{
		if (IsEmpty()) return NULL;
		return pBits;
	}

	/*****
	加载、复制、保存
	*****/
	virtual BOOL __stdcall	Load(const WCHAR *);	// 从文件加载
													//virtual BOOL	Load(UINT nIDResource);	// 从资源加载
													//virtual BOOL	Load(HDC hdc, HBITMAP hbmp);
	virtual BOOL __stdcall	LoadStandard(const WCHAR *);// (same override)从文件读图，建立统一 DIB 格式: 32bits RGBA
	virtual BOOL __stdcall	LoadStandard(Bitmap *);	// (same override)从Bitmap读图，建立统一 DIB 格式: 32bits RGBA
	virtual BOOL __stdcall	Clone(Bitmap *);		// (override to prevent hiding)

	virtual BOOL __stdcall	Clone(const Dib &);
	virtual BOOL __stdcall	Clone(const Dib *);		// (actual hiding)

	virtual BOOL __stdcall	Save(const WCHAR *) const;// 保存文件

													  /*****
													  调色板
													  *****/
	virtual BOOL __stdcall	SetPalette(CDC *);// 设置 DC 调色板
protected:
	/*****
	参数		#01
	*****/
	// 位图信息头中 infoHeader.biSizeImage 表示 DIB 数据大小（不包括调色板内容的大小）
	BIH				 infoHeader;	// 位图信息头

									/*****
									位图数据	#02 ~ #03
									*****/
	byte			*pBits;			// 像素数据指针
	RGBQUAD			*pPalette;		// 调色板数据指针

									/*****
									调色板	#04 ~ #05
									*****/
	UINT			nPaletteEntries;// 调色板中实际颜色数量
	CPalette		palette;		// 调色板
};