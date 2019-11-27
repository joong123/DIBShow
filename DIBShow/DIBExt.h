#pragma once

#include "DIB.h"


// TODO���Ƿ���Ҫ
class Dib : private DibBase
{
protected:
	virtual void __stdcall	ClearParam();	// (actual override)�������
	virtual void __stdcall	ParamDefault();	// (actual override)�ָ�Ĭ�ϲ���
	virtual bool __stdcall	SetImgInternal(const byte *pData, WORD nPixelBits, int w, int h, bool bAlpha = false);
	//virtual bool __stdcall	SetImgFromInternal(const byte *pSrc, WORD nPixelBits, int w, int h, bool bAlpha = false, COPYMODE cm = CM_NORMAL);
	virtual bool __stdcall	SetImgInternalExtend(const BIH &BIH);

public:
	__stdcall		Dib();
	__stdcall		~Dib() override;

	/*****
	ͼ���������
	*****/
	//virtual bool	IsEmpty() const;// �ж��Ƿ���ͼƬ
	bool __stdcall	ClearImg() override;		// ���ͼƬ
												//virtual bool	DetachDC();		// ���� DC
	bool __stdcall	Clear() override;		// �������

											/*****
											ָ�����
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
	��Ա
	*****/
	inline byte* __stdcall	GetPBits() const
	{
		if (IsEmpty()) return NULL;
		return pBits;
	}

	/*****
	���ء����ơ�����
	*****/
	virtual BOOL __stdcall	Load(const WCHAR *);	// ���ļ�����
													//virtual BOOL	Load(UINT nIDResource);	// ����Դ����
													//virtual BOOL	Load(HDC hdc, HBITMAP hbmp);
	virtual BOOL __stdcall	LoadStandard(const WCHAR *);// (same override)���ļ���ͼ������ͳһ DIB ��ʽ: 32bits RGBA
	virtual BOOL __stdcall	LoadStandard(Bitmap *);	// (same override)��Bitmap��ͼ������ͳһ DIB ��ʽ: 32bits RGBA
	virtual BOOL __stdcall	Clone(Bitmap *);		// (override to prevent hiding)

	virtual BOOL __stdcall	Clone(const Dib &);
	virtual BOOL __stdcall	Clone(const Dib *);		// (actual hiding)

	virtual BOOL __stdcall	Save(const WCHAR *) const;// �����ļ�

													  /*****
													  ��ɫ��
													  *****/
	virtual BOOL __stdcall	SetPalette(CDC *);// ���� DC ��ɫ��
protected:
	/*****
	����		#01
	*****/
	// λͼ��Ϣͷ�� infoHeader.biSizeImage ��ʾ DIB ���ݴ�С����������ɫ�����ݵĴ�С��
	BIH				 infoHeader;	// λͼ��Ϣͷ

									/*****
									λͼ����	#02 ~ #03
									*****/
	byte			*pBits;			// ��������ָ��
	RGBQUAD			*pPalette;		// ��ɫ������ָ��

									/*****
									��ɫ��	#04 ~ #05
									*****/
	UINT			nPaletteEntries;// ��ɫ����ʵ����ɫ����
	CPalette		palette;		// ��ɫ��
};