#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPE�N���X
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// �^�C�}�[

#include "..\..\Library\DefectLib\KizuToSend.h"							// ����˓����ԒʐM �N���X
#ifdef JPEGLIB_IPP
#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap��Jpeg�ϊ��N���X
#else
#include "..\..\Library\DefectLib\KizuLjtJpegManager.h"					// Bitmap��Jpeg�ϊ��N���X
#endif
#include "..\..\Library\KizuMMX\MemCopy.h"								// �������R�s�[�N���X
//#include "..\..\Library\KizuMMX\asm_x64\MemCopy_x64.h"								// �������R�s�[�N���X

// �r�� �C���N���[�h
#include "..\..\include\ToHtCommon.h"									// ��������p�̒�`�w�b�_

// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCsmemData.h"								// ���L��������̃f�[�^�\��
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`

// �e���[�J�[�X���b�h
#include "StatusManager.h"
#include "DataRecver.h"
#include "ParamManager.h"

// �S�������
#include "CullManager.h"

using namespace KizuLib;

class DataSender : public ThreadManager
{

public:
	static const int MC_TIME_JPEG_S = 2;		// JPEG���k (�؂�o���摜)
	static const int MC_TIME_JPEG_E = 3;

	static const int MC_TIME_RPI_S = 4;			// �S������PC�֌W
	static const int MC_TIME_RPI_E = 5;

	static const int MC_TIME_JF_S = 6;			// JPEG���k (�t���[���摜)
	static const int MC_TIME_JF_E = 7;

	static const int MC_TIME_ALL_S = 8;			// �t���[����M������
	static const int MC_TIME_ALL_E = 9;

//// ���J�֐�
public:
	DataSender(void);
	virtual ~DataSender(void);

	void SetParamMgr(ParamManager* pCls)	   { mcls_pPrmMgr = pCls;}
	void SetStatusMgr(StatusManager* pCls)	   { mcls_pStatus = pCls;}
	void SetDataRecver(DataRecver* pCls)	   { mcls_pData = pCls;}
	void SetKizuToSend(KizuToSend** pCls)	   { mcls_pSock = pCls;}

	// �S��
	void SetCullMgr(CullManager** pCls)		   { mcls_pCull = pCls; }


	void SetJpegQuality(int n)		{ m_nJpegQuality = n; if(0==n) {LOG(em_MSG), "[%s] BMP�摜���M", my_sThreadName);} else {LOG(em_INF), "[%s] JPEG�摜���M(�i��=%d)", my_sThreadName, n);} }
	void SetCamPair(int n)			{ m_nCamPair = n;}
	void SetKindHt(EM_DIV_HANTEI n)	{ m_emKindHt = n;}

	void StartHls();
	void StopHls();

	void SetFrameSend(bool bFrameSend = true)	{ m_bFrameSend = bFrameSend; }
	bool IsFrameSend()							{ return m_bFrameSend; }

	// ���̑�
	void			ClearFrameNo();												// FrameNo�֌W������
	int				GetDspFrameNo()		const { return m_nDspFrameNo;}
	int				GetDspRecvFrameNo()	const { return m_nDspRecvFrameNo;}
	int				GetJpegQuality()	const { return m_nJpegQuality;}

	// ����
	int				GetTimeIntval() { return (int)mcls_TimeCycle.GetCycleTime(); }
	int				GetTimeAllNow() { return (int)mcls_Time.GetAveTime(); }
	int				GetTimeAllMax() { return (int)mcls_Time.GetMaxTime(); }

	int				g_nTime_All[2];			// �t���[������	[ms](��,Max)
	int				g_nTime_Rpi[2];			// �S���摜���M�˗�	[ms](��,Max)
	int				g_nTime_To[2];			// �؏o�摜	[ms](��,Max)
	int				g_nTime_ToFrame[2];		// Jpeg�t���[��


//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	


	// ���C������
	void NewDataExec(BYTE* pNewData);									// �f�[�^�p�C�v��� 1�t���[����������
	void NewDataSendBack(int idx);										// NewDataExec �� New�������M�f�[�^�̊J�� ����� ���L�������̃I�t�Z�b�g�l�J��
	void SendHls();														// ��������

	// Jpeg�ϊ�
	//int CreateJpegData(HT_TO_HEAD* pData, nc_smem_pipe_data* pSmem, HANDLE* pSendAdder);	// ����Ǘ���JPEG�ϊ�
	//int CreateJpegData(nc_smem_frame_header* pSmem, BYTE* pImgAddr);		// ����Ǘ���JPEG�ϊ�
	int CreateJpegData(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr, int freelen);		// ����Ǘ���JPEG�ϊ� (�؂�o���摜)
	int CreateJpegFrame(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr);		// ����Ǘ���JPEG�ϊ� (�t���[���摜)


//// �����o�[�ϐ�
private:

	//// �e�C���X�^���X
	ParamManager*			mcls_pPrmMgr;								// �p�����[�^�N���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X
	DataRecver*				mcls_pData;									// �f�[�^��M�X���b�h
	KizuToSend**			mcls_pSock;									// ����˓����ԒʐM �N���X

	KizuPerformanceCounter	mcls_Time;									// ����
	KizuPerformanceCounter	mcls_TimeCycle;								// ����

	LogFileManager*			mcls_pLogSock;								// �\�P�b�g�p���O�N���X


	// �V�O�i��
	HANDLE					m_evTimeHls;								// �w���V�[������o��


	// ���̑��������
	DWORD					m_nFrameNo[MAX_HT1_CAMSET];					// �����֑��M�ς݃t���[��No [�J�����Z�b�g��]
	DWORD					m_nRecvFrameNo[MAX_HT1_CAMSET];				// �摜�����^�X�N�̃f�[�^�p�C�v�����M�����t���[��No [�J�����Z�b�g��]
	
	DWORD					m_nDspFrameNo;
	DWORD					m_nDspRecvFrameNo;

	int						m_nJpegQuality;								// Jpeg���k�i�� (0:BMP�̂܂� 1�`:Jpeg�ϊ�)

	int			 			m_nCamPair;									// ���݂̃J�����y�A 1�` (0:�o�b�N�A�b�v=���蓖�Ė���)
	EM_DIV_HANTEI			m_emKindHt;									// ����@�\��:1 �����@�\��:2
	bool					m_bSendHls;									// �����w���V�[���M�J�n

//// �S��
	CullManager**			mcls_pCull;									// �Ԉ����N���X

#ifdef JPEGLIB_LJT
	KizuLjtJpegManager		mcls_KizuJpegManager;						// Jpeg�摜�����N���X
#endif

	bool					m_bFrameSend;								// �t���[�����M����
	
};
