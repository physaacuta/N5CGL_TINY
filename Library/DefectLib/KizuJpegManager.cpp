#include "Stdafx.h"
#include "KizuJpegManager.h"

using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuJpegManager::KizuJpegManager(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuJpegManager::~KizuJpegManager(void)
{
}

//------------------------------------------
// BMP��JPEG �ϊ��i�O���C�X�P�[���j
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* bP BMP�摜�̐擪�|�C���^
// int jquality JPEG�摜�̕i�� (0�`100�H)
// int* jSize JPEG�ϊ���̃T�C�Y (�w�b�_�����܂܂�Ă���) 
// BYTE* jP JPEG�摜�̐擪�|�C���^ (���炩���� bW*bH ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuJpegManager::BmpToJpeg(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_��)
	jcprops.DIBWidth = nW;						// �摜�̉��T�C�Y
	jcprops.DIBHeight = (nTB ? -nH : nH); //nH;				// �摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = (UCHAR*)bP;				// �񈳏k�摜�ւ̃|�C���^
	jcprops.DIBChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.DIBColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.JPGColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// ���m�N���Ȃ̂ŃT�u�T���v�����O����
	jcprops.jquality = jquality;				// �i��
	jcprops.JPGFile = NULL;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGBytes = jP;						// �������o�͂̏ꍇ�̃o�b�t�@�ւ̃|�C���^
	jcprops.JPGWidth = nW;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGHeight = nH;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGSizeBytes = nW * nH;				// �ϊ���̃o�C�g��
	
	////// ���k
	ret = ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE);		// �o�b�t�@�i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		*jSize = jcprops.JPGSizeBytes;			// �摜�T�C�Y���Z�b�g����Ă���
		return 0;
	} else {						// �ُ�
		*jSize = 0;
		return 2;
	}
#else
	// �Ƃ肠�������̂܂܂̃f�[�^��Ԃ��悤�ɂ���
	_ASSERT(false);
	*jSize = nW*nH;
	jP = (BYTE*)bP;
	return 0;
#endif
}

//------------------------------------------
// BMP��JPEG �ϊ��@(�J���[��)
// int nW BMP�摜��
// int nH BMP�摜����
// const BYTE* bP BMP�摜�̐擪�|�C���^
// int jquality JPEG�摜�̕i�� (0�`100�H)
// int* jSize JPEG�ϊ���̃T�C�Y (�w�b�_�����܂܂�Ă���) 
// BYTE* jP JPEG�摜�̐擪�|�C���^ (���炩���� bW*bH*3 ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuJpegManager::BmpToJpegColor(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_��)
	jcprops.DIBWidth = nW;						// �摜�̉��T�C�Y
	jcprops.DIBHeight = (nTB ? -nH : nH); //nH;				// �摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = (UCHAR*)bP;				// �񈳏k�摜�ւ̃|�C���^
	jcprops.DIBChannels = 3;					// �J���[�f�t�H���g
	jcprops.DIBColor = IJL_RGB;					// �J���[�f�t�H���g
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 3;					// �J���[�f�t�H���g
	jcprops.JPGColor = IJL_YCBCR;				// �J���[�f�t�H���g
	jcprops.JPGSubsampling = IJL_411;			// �J���[�f�t�H���g
	jcprops.jquality = jquality;				// �i��
	jcprops.JPGFile = NULL;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGBytes = jP;						// �������o�͂̏ꍇ�̃o�b�t�@�ւ̃|�C���^
	jcprops.JPGWidth = nW;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGHeight = nH;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGSizeBytes = nW * nH;				// �ϊ���̃o�C�g��
	
	////// ���k
	ret = ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE);		// �o�b�t�@�i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		*jSize = jcprops.JPGSizeBytes;			// �摜�T�C�Y���Z�b�g����Ă���
		return 0;
	} else {						// �ُ�
		*jSize = 0;
		return 2;
	}
#else
	// �Ƃ肠�������̂܂܂̃f�[�^��Ԃ��悤�ɂ���
	_ASSERT(false);
	*jSize = nW*nH;
	jP = (BYTE*)bP;
	return 0;
#endif
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
int KizuJpegManager::BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_���H)
	jcprops.DIBWidth = nW;						// �摜�̉��T�C�Y
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// �摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = (UCHAR*)bP;				// �񈳏k�摜�ւ̃|�C���^
	jcprops.DIBChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.DIBColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.JPGColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// ���m�N���Ȃ̂ŃT�u�T���v�����O����
	jcprops.jquality = jquality;				// �i��
	jcprops.JPGFile = Path;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGWidth = nW;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGHeight = nH;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	
	////// ���k
	ret = ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE);		// �t�@�C���i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		return 0;
	} else {						// �ُ�
		return 2;
	}
#else
	return 0;
#endif
}


//------------------------------------------
// JPEG��BMP �ϊ�
// const BYTE* jP JPEG�摜�̐擪�|�C���^
// int jSize JPEG�̃T�C�Y
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE* bP �f�R�[�h���BMP�摜�̃f�[�^���̐擪�|�C���^ (���炩���� bW*bH ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuJpegManager::JpegToBmp(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB )
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_���H)
	jcprops.DIBWidth = nW;						// �ϊ���̉摜�̉��T�C�Y
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// �ϊ���̉摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = bP;						// �ϊ����BMP�摜�ւ̃|�C���^
	jcprops.DIBChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.DIBColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.JPGColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// ���m�N���Ȃ̂ŃT�u�T���v�����O����
	jcprops.jquality = 0;						// �i�� (�f�R�[�h�̍ۂ͐ݒ�K�v�Ȃ�)
	jcprops.JPGFile = NULL;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGBytes = (UCHAR*)jP;				// �������̃o�b�t�@�ւ̃|�C���^
	jcprops.JPGWidth = nW;						// �摜�T�C�Y
	jcprops.JPGHeight = nH;						// �摜�T�C�Y
	jcprops.JPGSizeBytes = jSize;				// �ϊ���̃o�C�g��
	
	////// ���k
	ret = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);		// �o�b�t�@�i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		return 0;
	} else {						// �ُ�
		return 2;
	}
#else
	return 0;
#endif
}
/*
//------------------------------------------
// JPEG�t�@�C����BMP �ϊ�
// const char* Path �ۑ��t�@�C���p�X
// int* nW BMP�摜��
// int* nH BMP�摜����
// BYTE* bP �f�R�[�h���BMP�摜�̃f�[�^���̐擪�|�C���^ (���̊֐������ŗ̈�m��)
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuJpegCreat::BMPtoJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_���H)
	jcprops.DIBWidth = nW;						// �摜�̉��T�C�Y
	jcprops.DIBHeight = nH;	//-bH;				// �摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = bP;						// �񈳏k�摜�ւ̃|�C���^
	jcprops.DIBChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.DIBColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 1;					// ���m�N���Ȃ̂�1
	jcprops.JPGColor = IJL_G;					// ���m�N���Ȃ̂�IJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// ���m�N���Ȃ̂ŃT�u�T���v�����O����
	jcprops.jquality = jquality;				// �i��
	jcprops.JPGFile = Path;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGWidth = nW;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	jcprops.JPGHeight = nH;						// �o�͉摜�T�C�Y = ���͂Ɠ���
	
	////// ���k
	ret = ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE);		// �t�@�C���i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		return 0;
	} else {						// �ُ�
		return 2;
	}
#endif
}
*/


//------------------------------------------
// JPEG��BMP �ϊ� (�J���[��)
// const BYTE* jP JPEG�摜�̐擪�|�C���^
// int jSize JPEG�̃T�C�Y
// int nW BMP�摜��
// int nH BMP�摜����
// BYTE* bP �f�R�[�h���BMP�摜�̃f�[�^���̐擪�|�C���^ (���炩���� bW*bH*3 ���̗̈���m�ۂ��鎖)
// bool nTB �摜���] true:���]
// �߂�l 0:���� 1�`:�e���G���[
//------------------------------------------
int KizuJpegManager::JpegToBmpColor(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB )
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// ������
	JPEG_CORE_PROPERTIES jcprops;			// JPEG�ϊ��f�[�^���n���p�\����
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// �\���̂Ƀf�[�^�Z�b�g (�摜�����ŏ㉺���΂ƂȂ�炵���BDIBHeight���}�C�i�X�ɂ���) �� ����_�~�[�c�[���ŉ摜��ǂݍ���ő��M�������̂݁H(�{�Ԃł̓}�C�i�X�ɂ�����_���H)
	jcprops.DIBWidth = nW;						// �ϊ���̉摜�̉��T�C�Y
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// �ϊ���̉摜�̏c�T�C�Y (�}�C�i�X��t����)
	jcprops.DIBBytes = bP;						// �ϊ����BMP�摜�ւ̃|�C���^
	jcprops.DIBChannels = 3;					// �J���[�f�t�H���g
	jcprops.DIBColor = IJL_RGB;					// �J���[�f�t�H���g
	jcprops.DIBPadBytes = 0;					// �摜�̒[�̗]��o�C�g��(����2�ׂ̂���)
	jcprops.JPGChannels = 3;					// �J���[�f�t�H���g
	jcprops.JPGColor = IJL_YCBCR;				// �J���[�f�t�H���g
	jcprops.JPGSubsampling =  IJL_411;			// �J���[�f�t�H���g
	jcprops.jquality = 0;						// �i�� (�f�R�[�h�̍ۂ͐ݒ�K�v�Ȃ�)
	jcprops.JPGFile = NULL;						// �t�@�C���o�͂̏ꍇ�̃t�@�C����
	jcprops.JPGBytes = (UCHAR*)jP;				// �������̃o�b�t�@�ւ̃|�C���^
	jcprops.JPGWidth = nW;						// �摜�T�C�Y
	jcprops.JPGHeight = nH;						// �摜�T�C�Y
	jcprops.JPGSizeBytes = jSize;				// �ϊ���̃o�C�g��
	
	////// ���k
	ret = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);		// �o�b�t�@�i�[

	////// �J��
	ijlFree(&jcprops);

	// ���k�̃A���T�[�`�F�b�N
	if(IJL_OK == ret) {				// ����
		return 0;
	} else {						// �ُ�
		return 2;
	}
#else
	return 0;
#endif
}
