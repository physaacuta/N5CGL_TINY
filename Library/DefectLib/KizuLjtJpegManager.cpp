
#include "Stdafx.h"
#include "KizuLjtJpegManager.h"
#include "tjheader.h"

#include <fstream>			

using namespace KizuLib;

template<typename T>
inline static bool GetProcAddressT(HINSTANCE hInst, const char *name, T &proc) {
	proc = (T)::GetProcAddress(hInst, name);
	return proc != NULL;
}

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuLjtJpegManager::KizuLjtJpegManager(void)
{
	if (p_tjInitCompress == NULL) {
		HINSTANCE hInst;
#ifdef _M_X64
		hInst = LoadLibraryA("turbojpeg_x64.dll");
#else
		hInst = LoadLibraryA("turbojpeg_x86.dll");
#endif
		if (hInst) {
			GetProcAddressT(hInst, "tjInitCompress", p_tjInitCompress);
			GetProcAddressT(hInst, "tjCompress2", p_tjCompress2);
			GetProcAddressT(hInst, "tjDestroy", p_tjDestroy);

			GetProcAddressT(hInst, "tjInitDecompress", p_tjInitDecompress);
			GetProcAddressT(hInst, "tjDecompressHeader3", p_tjDecompressHeader3);
			GetProcAddressT(hInst, "tjDecompress2", p_tjDecompress2);

		}
	}
	if (p_tjInitCompress)
	{
		tjEncode = p_tjInitCompress();
	}
	else {
		tjEncode = NULL;
	}
}


//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuLjtJpegManager::~KizuLjtJpegManager(void)
{
	if (tjEncode) {
		p_tjDestroy(tjEncode);
		tjEncode = NULL;
	}
}

//------------------------------------------
// BMP��JPEG �ϊ��i�O���C�X�P�[���j
// int nW BMP�摜��
// int nH BMP�摜����
// const unsigned char* bP BMP�摜�̐擪�|�C���^
// int jquality JPEG�摜�̕i�� (0�`100�H)
// int* jSize JPEG�ϊ���̃T�C�Y (�w�b�_�����܂܂�Ă���) 
// unsigned char* jP JPEG�摜�̐擪�|�C���^ (���炩���� bW*bH ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuLjtJpegManager::BmpToJpeg(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB)
{

	if (tjEncode == NULL)
		return 1;

	int outSubsamp = TJSAMP_GRAY;
	int pixelFormat = TJPF_GRAY;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	//      flags |= TJFLAG_ACCURATEDCT;
	//      flags |= TJFLAG_FASTUPSAMPLE;

	BYTE *jpegBuf = jP;
	int stride = (nW + 3) & ~3;
	unsigned long jpegSize = *jSize;

	if (p_tjCompress2(tjEncode, bP, nW, stride, nH, pixelFormat,
		&jpegBuf, &jpegSize, outSubsamp, jquality, flags) < 0) {
		return 2;
	}

	*jSize = jpegSize;
	return 0;
}

//------------------------------------------
// BMP��JPEG �ϊ��@(�J���[��)
// int nW BMP�摜��
// int nH BMP�摜����
// const unsigned char* bP BMP�摜�̐擪�|�C���^
// int jquality JPEG�摜�̕i�� (0�`100�H)
// int* jSize JPEG�ϊ���̃T�C�Y (�w�b�_�����܂܂�Ă���) 
// unsigned char* jP JPEG�摜�̐擪�|�C���^ (���炩���� bW*bH*3 ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuLjtJpegManager::BmpToJpegColor(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB)
{
	if (tjEncode == NULL)
		return 1;

	int outSubsamp = TJSAMP_411;
	int pixelFormat = TJPF_RGB;
	//int pixelFormat = TJPF_BGR;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	//      flags |= TJFLAG_ACCURATEDCT;
	//      flags |= TJFLAG_FASTUPSAMPLE;

	BYTE *jpegBuf = jP;
	int stride = (nW * 3 + 3) &~3;
	unsigned long jpegSize = *jSize; // bW*bH*3 ?

	if (p_tjCompress2(tjEncode, bP, nW, stride, nH, pixelFormat,
		&jpegBuf, &jpegSize, outSubsamp, jquality, flags) < 0) {
		return 2;
	}

	*jSize = jpegSize;
	return 0;


}




//------------------------------------------
// JPEG��BMP �ϊ�
// const unsigned char* jP JPEG�摜�̐擪�|�C���^
// int jSize JPEG�̃T�C�Y
// int nW BMP�摜��
// int nH BMP�摜����
// unsigned char* bP �f�R�[�h���BMP�摜�̃f�[�^���̐擪�|�C���^ (���炩���� bW*bH ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuLjtJpegManager::JpegToBmp(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB)
{
	if (p_tjInitDecompress == NULL)
		return 1;

	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;
	int pixelFormat = TJPF_GRAY;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}

	int stride = (nW + 3) &~3;

	//	tjDecompressHeader3();
	if (p_tjDecompress2(
		tjInstance,
		jP, jSize,
		bP, nW, stride, nH,
		pixelFormat, flags)) {
		p_tjDestroy(tjInstance);
		return 3;
	}
	p_tjDestroy(tjInstance);
	return 0;
}


//------------------------------------------
// JPEG��BMP �ϊ� (�J���[��)
// const unsigned char* jP JPEG�摜�̐擪�|�C���^
// int jSize JPEG�̃T�C�Y
// int nW BMP�摜��
// int nH BMP�摜����
// unsigned char* bP �f�R�[�h���BMP�摜�̃f�[�^���̐擪�|�C���^ (���炩���� bW*bH*3 ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuLjtJpegManager::JpegToBmpColor(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB)
{
	if (p_tjInitDecompress == NULL)
		return 1;

	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;

	int pixelFormat = TJPF_RGB;
	//int pixelFormat = TJPF_BGR;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	int stride = (nW * 3 + 3) &~3;

	//	tjDecompressHeader3();
	if (p_tjDecompress2(
		tjInstance,
		jP, jSize,
		bP, nW, stride, nH,
		pixelFormat, flags)) {
		p_tjDestroy(tjInstance);
		return 3;
	}

	p_tjDestroy(tjInstance);
	return 0;
}



//------------------------------------------
// JPEG�w�b�_����T�C�Y�Ȃǂ�݂���
// jP, jSize�FJpeg�̃o�b�t�@�ƒ���
// nW, nH : ���E����(Px) NULL�s��
// bColored : ���m�N���Ȃ�False�A�J���[�Ȃ�true
//------------------------------------------
int KizuLjtJpegManager::JpegToHeader(const unsigned char* jP, int jSize, int *nW, int *nH, bool *bColored)
{
	if (p_tjInitDecompress == NULL)
		return 1;


	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;
	//	tjDecompressHeader3();


	int subsamp = 0;
	int colorspace = 0;
	if (p_tjDecompressHeader3(
		tjInstance,
		jP, jSize,
		nW, nH,
		&subsamp, &colorspace)) {
		p_tjDestroy(tjInstance);
		return 3;
	}
	*bColored = subsamp != TJSAMP_GRAY;

	p_tjDestroy(tjInstance);
	return 0;
}


//------------------------------------------
// BMP��JPEG �ϊ���t�@�C����������
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* bP BMP�摜�̐擪�|�C���^
// int jquality JPEG�摜�̕i�� (0�`100�H)
// const char* Path �ۑ��t�@�C���p�X
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuLjtJpegManager::BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB)
{
	int	jSize = ((nW * nH) + 3) & ~3;
	if (0 >= jSize) {
		return 1;
	}
	BYTE* jP = new BYTE[jSize];		// BMP�̃T�C�Y���͑傫���Ȃ�Ȃ��͂�

	// BMP��JPEG �ϊ�
	int nRtn = BmpToJpeg(nW, nH, bP, jquality, &jSize, jP, nTB);

	// �t�@�C��������
	std::ofstream   os;

	os.open(Path, std::ios::out | std::ios::binary);
	os.write((const char*)jP, jSize);
	os.close();

	delete[] jP;

	return nRtn;
}

