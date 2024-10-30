#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// �^�C�}�[


// �S�������
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// ��]���i�`���̑��M�N���X
#include "..\..\Include\OutIF_HT_RAW.h"									// �S����`

// �e���[�J�[�X���b�h
#include "ParamManager.h"


class CullManager :	public ThreadManager
{
public:
	static const int MC_TIME_RPI_S = 4;			// �S������PC�֌W
	static const int MC_TIME_RPI_E = 5;


	//struct DELI {
	__declspec(align(16)) struct DELI {			// SSE�g�p�̂��߁A�A���C����16Byte�P�ʂƂ���
		int					nFrameSkip;			// 0:�ȊO  NG
		int					nCamId;				// �J�����Z�b�g 
		DWORD				nFrameNo;			// ���M�A�� (0�I���W��)
		float				dEdgePos[2];		// �G�b�W�ʒu�i�J�������W�n)mm�P��

		DWORD				nWidth;				// �t���[���摜X(pixel)
		DWORD				nHeight;			// �t���[���摜Y(pixel)
		DWORD				nStride;			// �t���[���摜X(Byte)
		DWORD				nChannel;			// �摜��

		DWORD				nImgSize[MAX_IMAGE_CNT];		// �摜�T�C�Y	�� 0 �͉摜�����B

		DWORD				nYobi[3];			// �A���C�������p�\��

		BYTE				pImg[1];			// RAW�摜�z��B���Z�b�g�F�̏ꍇ�́A���̂܂ܑO�l��			
	};


public:
	ThreadQueue<DELI>		gque_Deli;			// �󂯓n���L���[

	// ����
	int				g_nTime_Rpi[2];			// �Ԉ��t���[���S��


//// ���J�֐�
public:
	CullManager(void);
	virtual ~CullManager(void);

	void SetParamMgr(ParamManager* pCls)	   { mcls_pPrmMgr = pCls;}
	void SetLogMgrCull(LogFileManager* pCls)	{ mcls_pLogCull = pCls; }		// �S�������֘A���O�o�̓N���X


	// �S��
	void SetRpiToSend(SockOneToAnySend* pCls) { mcls_pSockRpi = pCls; }
	void SetQueSockRpi(ThreadLoopQueue<HT_RPI_FDATA>* pQue) { mque_pSockRpi = pQue; }


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void Exec(DELI* pDeli);												// ����


	// ���S��
	bool ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight, long nCullX, long nCullY, BYTE* pDstRaw, int mode);	// RAW�摜 �Ԉ���
	bool ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight, BYTE* pDstRaw, long nDstWidth, long nDstHeight, int mode);

	bool ResizeRaw(BYTE* const pSrcRaw, int nSrcWidth, int nSrcHeight, int nSrcRoiX, int nSrcRoiY, int nSrcRoiW, int nSrcRoiH,
		int nDstWidth, int nDstHeight, double dXF, double dYF, BYTE* pDstRaw, int mode);


//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	ParamManager*			mcls_pPrmMgr;								// �p�����[�^�N���X
	LogFileManager*			mcls_pLogCull;								// �S�������֘A���O�o�̓N���X

	KizuPerformanceCounter	mcls_Time;									// ����

//// �S��
	SockOneToAnySend*		mcls_pSockRpi;								// ����ˑS�������ԒʐM �N���X
	ThreadLoopQueue<HT_RPI_FDATA>*	mque_pSockRpi;						// �t���[���摜�󂯓n���L���[(��]���i�`��)	[LIB��AP��] �� �L���[�T�C�Y��AP����w�肵�������߁A�|�C���^�ɂ��Ă���


};
