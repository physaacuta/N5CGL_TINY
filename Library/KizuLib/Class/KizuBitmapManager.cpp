#include "Stdafx.h"
#include "KizuBitmapManager.h"

#include <fstream>			
#include "KizuMacro.h"												// �}�N���W

using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuBitmapManager::KizuBitmapManager(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuBitmapManager::~KizuBitmapManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////
// �J���[�摜24bpp�p
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// Bitmap�w�b�_�T�C�Y�擾
// �߂�l Bitmap�w�b�_�[�T�C�Y
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Color()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
}

//------------------------------------------
// Bitmap�w�b�_����
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE *BmpHead BitMap�w�b�_(���炩����Bitmap�w�b�_�T�C�Y���̗̈���m�ۂ��Ă�������)
// bool nTB �摜���] true:���] 
// �߂�l ���A��� 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Color(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	int					nBitCount = 24;					// �r�b�g�[��

	// ������
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));

	// �t�@�C���w�b�_�쐬
	memcpy(&bmpfhead.bfType, "BM", 2);					// �t�@�C�����ʎq("BM"�Œ�)
	bmpfhead.bfSize = (DWORD)							// �t�@�C���T�C�Y(�o�C�g�P��)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Color());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Color();	// �r�b�g�}�b�v�C���[�W�f�[�^�J�n�ʒu

	// �r�b�g�}�b�v�w�b�_�쐬
	bmpinf.biSize = 40;									// �w�b�_�T�C�Y(40�Œ�)
	bmpinf.biWidth = (long)nW;							// �C���[�W�̉���(�s�N�Z���P��)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// �C���[�W�̍���(�s�N�Z���P��) (Windows�͉摜���㉺���]�̈�)
	bmpinf.biPlanes = 1;								// �v���[����(1�Œ�)
	bmpinf.biBitCount = nBitCount;							// �P�s�N�Z��������̃r�b�g��(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// ���k�`��(BI_RGB:0->�񈳏k, BI_RLE8:1->RLE���k)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);				// ���k��̃C���[�W�T�C�Y(�o�C�g�P��)
	bmpinf.biClrUsed = 0;								// �����𑜓x(�s�N�Z��/���[�g��)
	bmpinf.biClrImportant = 0;						// �����𑜓x(�s�N�Z��/���[�g��)

	// �f�[�^�Z�b�g
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));

	return 0;
}

//------------------------------------------
// ������`�� (2�o�C�g�����ɑΉ��ς�)
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE* BmpData BitMap�f�[�^��
// int nLeft ���W
// char const* szText ������
// int nFontSize �t�H���g�T�C�Y
// DWORD color �����F�B
//------------------------------------------
bool KizuBitmapManager::DrawText_Color(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, DWORD color)
{
	//// �t�H���g�̐���
	LOGFONT font	= { nFontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N" };
	HFONT	hFont	= CreateFontIndirect(&font);
	if( NULL == hFont) return false;

	//// �f�o�C�X�R���e�L�X�g�擾
	HDC		hDc		= GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hDc, hFont);

	//�F���o�C�g�P�ʂɕ���
	BYTE colorR = GetRValue(color);
	BYTE colorG = GetGValue(color);
	BYTE colorB = GetBValue(color);


	//// �����������[�v	�����C�h��������
	const char* c = szText;
	while (*c) {
		// �����R�[�h�擾
		// �}���`�o�C�g�����̏ꍇ�A1�o�C�g�����̃R�[�h��1�o�C�g�ڂ�UINT�ϊ��A
		// 2�o�C�g�����̃R�[�h��[�擱�R�[�h]*256 + [�����R�[�h]
		UINT nCode = 0;
		int nWidth;
		if (IsDBCSLeadByte(*c)) {
			nCode = (BYTE)c[0]<<8 | (BYTE)c[1];
			c += 2;
			nWidth = nFontSize;
		} else {
			nCode = c[0];
			c++;
			nWidth = nFontSize / 2;
		}
		// �t�H���g�r�b�g�}�b�v�擾
		// GGO_BITMAP�̏ꍇ�A�uDWORD�ɃA���C�������g���ꂽ���m�g�[��BMP�v���Ԃ�
		// ���m�g�[��BMP�Ƃ�1�s�N�Z����1�r�b�g�ŕ\������r�b�g�}�b�v�̎�
		TEXTMETRIC tm;
		GetTextMetrics(hDc, &tm);
		GLYPHMETRICS gm;
		CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, 0, NULL, &mat);
		BYTE *ptr = new BYTE[size*2];	// ���߂ɗ̈���m�ۂ���B�K�v�Ȃ������m��Ȃ����A���������󂷂ƕ|������
		memset(ptr, 0x00, size*2);
		GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, size, ptr, &mat);
		// �t�H���g���̏�������
		// �擾�����r�b�g��1�s�N�Z���ɑ�������̂ŁA
		// nOffsetX, nOffsetY : �����o���ʒu(����)
		// nBmpW, nBmpH : �t�H���g�r�b�g�}�b�v�̕���
		int nOffsetX = gm.gmptGlyphOrigin.x + nLeft;				// �������݈ʒu�̃I�t�Z�b�g���ʒu
		int nOffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y + nTop;	// �������݈ʒu�̃I�t�Z�b�g���ʒu
		int nBmpW = gm.gmBlackBoxX;									// �擾����BMP�̉��s�N�Z����
		int nBmpH = gm.gmBlackBoxY;									// BMP�̏c�s�N�Z����
		int iUseBYTEparLine = (1 + (gm.gmBlackBoxX / 32))*4;		// 1�s�Ɏg�p���Ă���BYTE���i4�o�C�g�A���C�������g�j
		
		// �������`�F�b�N
		if(0 > nOffsetY) nOffsetY = 0;
		if(0 > nOffsetX) nOffsetX = 0;
		if(nH < nOffsetY+nBmpH) nOffsetY = nH - nBmpH;
		if(nW < nOffsetX+nBmpW) nOffsetX = nW - nBmpW;
		
		for (int y=nOffsetY; y < nOffsetY+nBmpH; y++) {
			for (int x=nOffsetX; x < nOffsetX+nBmpW; x++) {
				DWORD num = (x-nOffsetX) / 8;									// ���݂�x��1�s�̉�BYTE�ڂ����Z�o
				BYTE bit = (x-nOffsetX) % 8;									// ���݂�x��1�o�C�g���̉��r�b�g�ڂ����Z�o
				BYTE mask = ((BYTE)1)<<(7-bit);									// ���݂̃r�b�g���̃}�X�N�쐬
				BYTE Cur = ptr[ iUseBYTEparLine*(y-nOffsetY)+num ];	// ���݂̃o�C�g�ʒu�ɂ���r�b�g�}�b�v���擾
				if (Cur & mask){
					BmpData[(y*nW + x)*3 +0] = colorR;								// �h�b�g�������݁i1�s�N�Z���j
					BmpData[(y*nW + x)*3 +1] = colorG;								// �h�b�g�������݁i1�s�N�Z���j
					BmpData[(y*nW + x)*3 +2] = colorB;								// �h�b�g�������݁i1�s�N�Z���j
				}
			}
		}
		delete[] ptr;
		nLeft += nWidth;
	}

	//// �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
	SelectObject(hDc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDc);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////
// �O���[�摜�p
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// Bitmap�w�b�_�T�C�Y�擾
// �߂�l Bitmap�w�b�_�[�T�C�Y
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Gray()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
}

//------------------------------------------
// Bitmap�w�b�_����
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE *BmpHead BitMap�w�b�_(���炩����Bitmap�w�b�_�T�C�Y���̗̈���m�ۂ��Ă�������)
// bool nTB �摜���] true:���] 
// �߂�l ���A��� 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Gray(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	int					ii;
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	RGBQUAD				rgb[256];
	int					nBitCount = 8;					// �r�b�g�[��

	// ������
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));
	memset(&rgb[0], 0x00, sizeof(rgb));

	// �t�@�C���w�b�_�쐬
	memcpy(&bmpfhead.bfType, "BM", 2);					// �t�@�C�����ʎq("BM"�Œ�)
	bmpfhead.bfSize = (DWORD)							// �t�@�C���T�C�Y(�o�C�g�P��)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Gray());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Gray();	// �r�b�g�}�b�v�C���[�W�f�[�^�J�n�ʒu

	// �r�b�g�}�b�v�w�b�_�쐬
	bmpinf.biSize = 40;									// �w�b�_�T�C�Y(40�Œ�)
	bmpinf.biWidth = (long)nW;							// �C���[�W�̉���(�s�N�Z���P��)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// �C���[�W�̍���(�s�N�Z���P��) (Windows�͉摜���㉺���]�̈�)
	bmpinf.biPlanes = 1;								// �v���[����(1�Œ�)
	bmpinf.biBitCount = nBitCount;							// �P�s�N�Z��������̃r�b�g��(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// ���k�`��(BI_RGB:0->�񈳏k, BI_RLE8:1->RLE���k)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);				// ���k��̃C���[�W�T�C�Y(�o�C�g�P��)
	//bmpinf.biClrUsed = 256;								// �����𑜓x(�s�N�Z��/���[�g��)
	//bmpinf.biClrImportant = 256;						// �����𑜓x(�s�N�Z��/���[�g��)
	bmpinf.biClrUsed = 0;								// �����𑜓x(�s�N�Z��/���[�g��)
	bmpinf.biClrImportant = 0;						// �����𑜓x(�s�N�Z��/���[�g��)

	// �J���[�p���b�g�쐬(�p���b�g�̒��g)
    for (ii = 0; ii < 256; ii++) {	
		rgb[ii].rgbBlue = (BYTE)ii;
		rgb[ii].rgbRed = (BYTE)ii;
		rgb[ii].rgbGreen = (BYTE)ii;
	}
	// �f�[�^�Z�b�g
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));
	memcpy(&BmpHead[sizeof(bmpfhead) + sizeof(bmpinf)], (unsigned char*)&rgb[0], sizeof(RGBQUAD) * 256);

	return 0;
}

//------------------------------------------
// Bitmap�t�@�C���֏o��
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* BmpData BitMap�f�[�^��
// const BYTE* BmpHead BitMap�w�b�_��
// const char* Path �o�̓t�@�C���p�X
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path)
{
	std::ofstream os;

//	os.open(Path, std::ios::out | std::ios::binary, filebuf::sh_read | filebuf::sh_write);
	os.open(Path, std::ios::out | std::ios::binary);
	os.write((char*)BmpHead, GetBitmapHeaderSize_Gray() );
	os.write((char*)BmpData, nW * nH);
	os.close();
}

//------------------------------------------
// Bitmap�t�@�C���֏o��
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* BmpData BitMap�f�[�^��
// bool nTB �摜���] true:���] 
// const char* Path �o�̓t�@�C���p�X
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB)
{
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];
	CreateBitmapHeader_Gray(nW, nH, BmpHead, nTB);
	DataToBMPfile_Gray(nW, nH, BmpData, BmpHead, Path);
	delete BmpHead;
}

//------------------------------------------
// Bitmap�t�@�C���ǂݍ���
// const char* Path �Ǎ��t�@�C���p�X
// int* nW BMP�摜��
// int* nH BMP�摜����
// const BYTE* BmpData Bitmap�f�[�^��
// �߂�l 0:�ُ� 1�`:Bitmap�f�[�^�T�C�Y
//------------------------------------------
int KizuBitmapManager::BMPfileToData_Gray(const char* Path, int* nW, int* nH, BYTE *BmpData)
{
	std::ifstream os;
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];

	//os.open(Path, ios::in | ios::binary, filebuf::sh_read|filebuf::sh_write);
	os.open(Path, std::ios::in | std::ios::binary);
	os.read((char*)BmpHead, GetBitmapHeaderSize_Gray());		// Bitmap�w�b�_���̂ݓǍ�
	
	// �摜�T�C�Y����
	*nW = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biWidth );
	*nH = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biHeight );
	int size = (*nW) * (*nH);

	os.read((char*)BmpData, size);
	os.close();

	delete BmpHead;
	return size;
}
//------------------------------------------
// Bitmap�t�@�C���̃w�b�_�[���̂ݓǍ���
//------------------------------------------
int KizuBitmapManager::BMPfileToData_Gray(const char* Path, int* nW, int* nH)
{
	std::ifstream os;
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];

	//os.open(Path, ios::in | ios::binary, filebuf::sh_read|filebuf::sh_write);
	os.open(Path, std::ios::in | std::ios::binary);
	os.read((char*)BmpHead, GetBitmapHeaderSize_Gray());		// Bitmap�w�b�_���̂ݓǍ�
	
	// �摜�T�C�Y����
	*nW = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biWidth );
	*nH = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biHeight );
	int size = (*nW) * (*nH);
	delete BmpHead;
	return size;
}


//------------------------------------------
// ����`�� (�����A�ΐ��ɑΉ��ς�)
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE* BmpData BitMap�f�[�^��
// int nX1 ���W
// BYTE color ���F 0:�� 255:��
//------------------------------------------
void KizuBitmapManager::DrawLine_Gray(int nW, int nH, BYTE* BmpData, int nX1, int nY1, int nX2, int nY2, BYTE color)
{
	//// ���͒l�`�F�b�N
	if(0 > nX1) nX1 = 0;
	if(0 > nX2) nX2 = 0;
	if(0 > nY1) nY1 = 0;
	if(0 > nY2) nY2 = 0;

	if(nW <= nX1) nX1 = nW-1;
	if(nW <= nX2) nX2 = nW-1;
	if(nH <= nY1) nY1 = nH-1;
	if(nH <= nY2) nY2 = nH-1;

	//// �K�����������1 �ƂȂ�悤��
	int x1, x2, y1, y2;
	if( nX1 < nX2 ) { x1 = nX1; x2 = nX2; }
	else			{ x1 = nX2; x2 = nX1; }
	if( nY1 < nY2 ) { y1 = nY1; y2 = nY2; }
	else			{ y1 = nY2; y2 = nY1; }


	//// �`��
	if( nX1 != nX2 && nY1 != nY2) {
		// �ΐ�
		float a = (float)(nY2-nY1) / (float)(nX2-nX1);	// �X��
		
		for(int x=x1; x<=x2; x++) {
			int y;
			if ( a < 0 ) {
				y = (int)(a*(x-x2))+y1;
			} else {
				y = (int)(a*(x-x1))+y1;
			}
			//int y = (int)(a*(x-x1))+y1;

			BmpData[y*nW + x] = color;
		}

	} else if(nX1 == nX2) {
		// �c��
		for(int y=y1; y<=y2; y++) {
			BmpData[y*nW + nX1] = color;
		}

	} else {
		// ����
		for(int x=x1; x<=x2; x++) {
			BmpData[nY1*nW + x] = color;
		}
	}
}
//------------------------------------------
// ������`�� (2�o�C�g�����ɑΉ��ς�)
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE* BmpData BitMap�f�[�^��
// int nLeft ���W
// char const* szText ������
// int nFontSize �t�H���g�T�C�Y
// BYTE color ���F 0:�� 255:��
//------------------------------------------
bool KizuBitmapManager::DrawText_Gray(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, BYTE color)
{
	//// �t�H���g�̐���
	LOGFONT font	= { nFontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "�l�r �S�V�b�N" };
	HFONT	hFont	= CreateFontIndirect(&font);
	if( NULL == hFont) return false;

	//// �f�o�C�X�R���e�L�X�g�擾
	HDC		hDc		= GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hDc, hFont);


	//// �����������[�v	�����C�h��������
	char* c = (char*)szText;
	while (*c) {
		// �����R�[�h�擾
		// �}���`�o�C�g�����̏ꍇ�A1�o�C�g�����̃R�[�h��1�o�C�g�ڂ�UINT�ϊ��A
		// 2�o�C�g�����̃R�[�h��[�擱�R�[�h]*256 + [�����R�[�h]
		UINT nCode = 0;
		int nWidth;
		if (IsDBCSLeadByte(*c)) {
			nCode = (BYTE)c[0]<<8 | (BYTE)c[1];
			c += 2;
			nWidth = nFontSize;
		} else {
			nCode = c[0];
			c++;
			nWidth = nFontSize / 2;
		}
		// �t�H���g�r�b�g�}�b�v�擾
		// GGO_BITMAP�̏ꍇ�A�uDWORD�ɃA���C�������g���ꂽ���m�g�[��BMP�v���Ԃ�
		// ���m�g�[��BMP�Ƃ�1�s�N�Z����1�r�b�g�ŕ\������r�b�g�}�b�v�̎�
		TEXTMETRIC tm;
		GetTextMetrics(hDc, &tm);
		GLYPHMETRICS gm;
		CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, 0, NULL, &mat);
		BYTE *ptr = new BYTE[size*2];	// ���߂ɗ̈���m�ۂ���B�K�v�Ȃ������m��Ȃ����A���������󂷂ƕ|������
		memset(ptr, 0x00, size*2);
		GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, size, ptr, &mat);
		// �t�H���g���̏�������
		// �擾�����r�b�g��1�s�N�Z���ɑ�������̂ŁA
		// nOffsetX, nOffsetY : �����o���ʒu(����)
		// nBmpW, nBmpH : �t�H���g�r�b�g�}�b�v�̕���
		int nOffsetX = gm.gmptGlyphOrigin.x + nLeft;				// �������݈ʒu�̃I�t�Z�b�g���ʒu
		int nOffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y + nTop;	// �������݈ʒu�̃I�t�Z�b�g���ʒu
		int nBmpW = gm.gmBlackBoxX;									// �擾����BMP�̉��s�N�Z����
		int nBmpH = gm.gmBlackBoxY;									// BMP�̏c�s�N�Z����
		int iUseBYTEparLine = (1 + (gm.gmBlackBoxX / 32))*4;		// 1�s�Ɏg�p���Ă���BYTE���i4�o�C�g�A���C�������g�j
		
		// �������`�F�b�N
		if(0 > nOffsetY) nOffsetY = 0;
		if(0 > nOffsetX) nOffsetX = 0;
		if(nH < nOffsetY+nBmpH) nOffsetY = nH - nBmpH;
		if(nW < nOffsetX+nBmpW) nOffsetX = nW - nBmpW;
		
		for (int y=nOffsetY; y < nOffsetY+nBmpH; y++) {
			for (int x=nOffsetX; x < nOffsetX+nBmpW; x++) {
				DWORD num = (x-nOffsetX) / 8;									// ���݂�x��1�s�̉�BYTE�ڂ����Z�o
				BYTE bit = (x-nOffsetX) % 8;									// ���݂�x��1�o�C�g���̉��r�b�g�ڂ����Z�o
				BYTE mask = ((BYTE)1)<<(7-bit);									// ���݂̃r�b�g���̃}�X�N�쐬
				BYTE Cur = *((BYTE*)ptr + iUseBYTEparLine*(y-nOffsetY)+num);	// ���݂̃o�C�g�ʒu�ɂ���r�b�g�}�b�v���擾
				Cur &= mask;													// �쐬�����}�X�N�ƌ��݂̃o�C�g�ʒu�ƂŘ_���ω��Z
				if (Cur) BmpData[y*nW + x] = color;								// �h�b�g���������݁i1�s�N�Z���j
			}
		}
		delete[] ptr;
		nLeft += nWidth;
	}

	//// �f�o�C�X�R���e�L�X�g�ƃt�H���g�n���h���̊J��
	SelectObject(hDc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDc);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////
// 2�l���摜�p
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// Bitmap�w�b�_�T�C�Y�擾
// �߂�l Bitmap�w�b�_�[�T�C�Y
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Bin()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
}

//------------------------------------------
// BitMap�w�b�_����
// int bW BMP�摜��
// int bH BMP�摜����
// BYTE* BmpHead BitMap�w�b�_(���炩����BitMap�w�b�_�T�C�Y���̗̈���m�ۂ��Ă�������)
// bool nTB �摜���] true:���] 
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Bin(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	RGBQUAD				rgb[2];
	int					nBitCount = 1;					// �r�b�g�[��

	// ������
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));
	memset(&rgb[0], 0x00, sizeof(rgb));

	// �t�@�C���w�b�_�쐬
	memcpy(&bmpfhead.bfType, "BM", 2);					// �t�@�C�����ʎq("BM"�Œ�)
	bmpfhead.bfSize = (DWORD)							// �t�@�C���T�C�Y(�o�C�g�P��)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Bin());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Bin();		// �r�b�g�}�b�v�C���[�W�f�[�^�J�n�ʒu

	// �r�b�g�}�b�v�w�b�_�쐬
	bmpinf.biSize = 40;									// �w�b�_�T�C�Y(40�Œ�)
	bmpinf.biWidth = (long)nW;							// �C���[�W�̉���(�s�N�Z���P��)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// �C���[�W�̍���(�s�N�Z���P��) (Windows�͉摜���㉺���]�̈�)
	bmpinf.biPlanes = 1;								// �v���[����(1�Œ�)
	bmpinf.biBitCount = nBitCount;						// �P�s�N�Z��������̃r�b�g��(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// ���k�`��(BI_RGB:0->�񈳏k, BI_RLE8:1->RLE���k)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);		// ���k��̃C���[�W�T�C�Y(�o�C�g�P��)
	bmpinf.biClrUsed = 0;								// �����𑜓x(�s�N�Z��/���[�g��)
	bmpinf.biClrImportant = 0;						// �����𑜓x(�s�N�Z��/���[�g��)

	// �J���[�p���b�g�쐬(�p���b�g�̒��g)
	rgb[0].rgbBlue = 0;
	rgb[0].rgbRed = 0;
	rgb[0].rgbGreen = 0;
	rgb[1].rgbBlue = 0xff;
	rgb[1].rgbRed = 0xff;
	rgb[1].rgbGreen = 0xff;

	// �f�[�^�Z�b�g
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));
	memcpy(&BmpHead[sizeof(bmpfhead) + sizeof(bmpinf)], (unsigned char*)&rgb[0], sizeof(RGBQUAD) * 2);

	return 0;
}

//------------------------------------------
// Bitmap�t�@�C���֏o��
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* BmpData BitMap�f�[�^��
// const BYTE* BmpHead BitMap�w�b�_��
// const char* Path �o�̓t�@�C���p�X
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path)
{
	// �G���A�m��
	int size = nW * nH * 1 / 8;
	BYTE *pData = new BYTE[size];
	
	// 1bit�̉摜�́A�r�b�g���т����ւ���
	int rec=0;
	for(int ii=0; ii<size; ii++) {
		pData[ii] = ((BmpData[ii] & 0x1) << 7) |
					 ((BmpData[ii] & 0x2) << 5) |
					 ((BmpData[ii] & 0x4) << 3) |
					 ((BmpData[ii] & 0x8) << 1) |
					 ((BmpData[ii] & 0x10) >> 1) |
					 ((BmpData[ii] & 0x20) >> 3) |
					 ((BmpData[ii] & 0x40) >> 5) |
					 ((BmpData[ii] & 0x80) >> 7) ;
	}

	// �o��
	std::ofstream os;
	os.open(Path, std::ios::out | std::ios::binary);
	os.write((char*)BmpHead, GetBitmapHeaderSize_Bin() );
	//os.write((char*)BmpData, nW * nH);
	os.write((char*)pData, size);
	os.close();

	// �G���A�J��
	SAFE_DELETE_ARRAY(pData);
//	delete [] pData;
}

//------------------------------------------
// Bitmap�t�@�C���֏o��
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* BmpData BitMap�f�[�^��
// const char* Path �o�̓t�@�C���p�X
// bool nTB �摜���] true:���] 
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB)
{
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Bin()];
	CreateBitmapHeader_Bin(nW, nH, BmpHead, nTB);
	DataToBMPfile_Bin(nW, nH, BmpData, BmpHead, Path);
	delete BmpHead;
}
