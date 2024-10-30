#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// �^�C�}�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\include\ToHtCommon.h"									// ��������p�̒�`�w�b�_

// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCspeed.h"
//#include "..\..\Include\EPC\NCAttr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
#include "..\..\Include\EPC\NCL2attr.h"

using namespace KizuLib;

class DataSender : public WorkThread
{
private:
	static const int IMG_SIZE_X = 512;
	static const int IMG_SIZE_Y = 512;
	//static const int IMG_SIZE_X = 768;
	//static const int IMG_SIZE_Y = 768;
	//static const int IMG_SIZE_X = 1024;
	//static const int IMG_SIZE_Y = 1024;
	
	static const int FILE_IMG_NUM = 1;		// �؂�o���摜����
	static const int FILE_FRM_NUM = 1;		// �t���[���摜����
#define		FILE_FORMAT_IMG	 "sample_%02d.bmp"		// �؂�o���摜�p�X
#define		FILE_FORMAT_FRM	 "frm_%02d.bmp"			// �t���[���摜�p�X



	static const int MAX_MEMSIZE = 1024*1024*400;
	//static const int MAX_MEMSIZE = 1024*1024*200;
	static const int MAX_DEFCNT	 = 10;
	// ��������ȂP��̃T�C�Y
	static const int ONE_MEMSIZE = 2048
									+ (SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * 4)
									+ sizeof(float)*SIZE_FRAME_IMG_Y*2
									+ sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*4) * MAX_DEFCNT
									+ IMG_SIZE_X*IMG_SIZE_Y*4  * MAX_DEFCNT;




	static const int MAX_NEXT_POS_CMD = 3;		// �ő�R�}���h�~�ϐ�
	static const int FRM_SHRINK_X = 2;		// �t���[���摜X�k��
	static const int FRM_SHRINK_Y = 2;		// �t���[���摜Y�k��


public:
	struct EPC_POS {
		float			front_pos;		// ���Z�J�E���^
		float			tail_pos;		// ���Z�J�E���^
		float			shear_cut_pos;	// ���Z�J�E���^
		float			univ_cnt_up_pos[4];		// �ėp���Z�J�E���^
		float			univ_cnt_dn_pos[4];		// �ėp���Z�J�E���^
	};




public:
	DataSender(void);
	virtual ~DataSender(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g


	int SendData(LPVOID senddata, DWORD sendsize);
	int SetFrameData(DEBUG_SPEED_INFO* readInf);
	void InitFrameNo() { m_nFrameNo=0; }
	void SetCamset(int num, DWORD* set) { m_camsetnum = num; for (int ii = 0; ii < 8; ii++) m_camset[ii] = abs((int)set[ii]); }
	//void SetCamset(int num, DWORD* set ) {m_camsetnum=num; memcpy(m_camset, set, 4*8); }
	void GetCamset(int* num, DWORD* set) {*num = m_camsetnum; memcpy(set, m_camset, 4*8); }

	void SetBrightness(int* set)		 {memcpy(&m_nBrightness, set, sizeof(int)*4*3); }
	void SetCamGain(double* set)		 {memcpy(&m_dCamGain, set, sizeof(double)*3); }
	void SetEdge(int ii, double* set)		{if(ii>=0 && ii<2) m_dEdge[ii] = *set;}
	int  GetBrightness(int angle, int pos)	{ return m_nBrightness[angle][pos]; }
	double SetCamGain(int color)			{ return m_dCamGain[color]; }
	void SetRandomParam(bool set)			{ m_bRand = set; }

	void SetEdgeRestrict(NCL_EDGE_RESTRICT_DATA* readInf) { memcpy(&m_EdgeRestrict, readInf, sizeof(NCL_EDGE_RESTRICT_DATA)); }	// �G�b�W�͈͐���

	void SetShrinkRatio(float value) { m_shrinkratio = value; }
	float GetShrinkRatio() { return m_shrinkratio; }

	BYTE*					gtbl_pFrame;								// �t���[�����󂯓n���p���L������ �f�[�^

	//bool					g_bPutKizu;									// �r�o��
	//int						g_nPutKizu;									// �r�o�͌�

	bool					g_bPutKizu[5];								// �r�o��
	int						g_nPutKizu[5];								// �r�o�͌�
	int						g_nPutFrm[5];								// �r�o�̓t���[���Ԋu

	int						g_nPutType[5];								// �o�͑Ώۂ��r��
	int						g_nPutGrade[5];								// �o�͑Ώۂ̃O���[�h
	int						g_nPutCycle[5];								// �o�͑Ώۂ̎�������

	bool					g_bPutEdge;									// �G�b�W�ύX�o��
	bool					g_bPutSkip;									// ��������ԏo��

	int						m_nFrameNo;									// �t���[��No

	int						g_nPosX;

private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	KizuPerformanceCounter	mcls_Time;									// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)


	HANDLE					m_hFrame;									// �t���[�����󂯓n���p���L������ �n���h��
	HANDLE					m_hPipe;									// �p�C�v�{��
	OVERLAPPED				m_ovlSend;									// ��M�p�񓯊�IO

	int						m_camsetnum;								// �J�����Z�b�g��
	int						m_camset[8];								// �J�����Z�b�g

	int						m_nSmemBase;								// ���L������ �����݃|�C���^

	int						m_nBrightness[4][3];						// �P�x���[�J�����p�x][���ޓ����ϖ��邳/�E�I�[�o�[���b�v�P�x/���I�[�o�[���b�v�P�x]
	double					m_dCamGain[3];								// �J�����Q�C�����ݒl[R/G/B]
	double					m_dEdge[2];									// �G�b�W�ʒu
	bool					m_bRand;									// �F�X�������Z�b�g���邩�ۂ��t���O

	BYTE                    m_imgTemp[( IMG_SIZE_X * IMG_SIZE_Y * MAX_IMAGE_CNT ) * FILE_IMG_NUM ];
    BYTE                    m_frmTemp[( SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y ) * FILE_FRM_NUM ];

	float					m_shrinkratio;								// �؂�o���摜�̏k����
	bool					m_bSHMemSet;								// ���L�������ݒ�t���O

//// �G���W�����xUDP�����Ή�
private:
	EPC_POS					m_NowPos;									// ���݋���
	NCL_CMD_SET_POSITION	m_NextPosCmd[MAX_NEXT_POS_CMD];				// NCL_SET_POSITION �v���B
	NCL_EDGE_RESTRICT_DATA	m_EdgeRestrict;								// NCL_EDGE_RESTRICT_DATA ��M

public:
	void	InitPosCmd() { 
		memset(m_NextPosCmd, 0x00, sizeof(m_NextPosCmd));
		memset( &m_NowPos, 0x00, sizeof(m_NowPos));
	}
	void	ClearPosCmd() { 
		memset(m_NextPosCmd, 0x00, sizeof(m_NextPosCmd));
	}
	void	SetNextPosCmd(NCL_CMD_SET_POSITION* p);
	void	SetFirstSHMem(bool bSHMemSet, FIRST_SHMEM_INFOv1* p);
	void	SetFirstSHMem(FIRST_SHMEM_INFOv1* p);
};
