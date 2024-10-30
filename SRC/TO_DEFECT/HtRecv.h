#pragma once

// �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

#include "..\..\Library\KizuLib\Class\KizuBitMapManager.h"			// �f�o�b�O�p
#include "..\..\Library\KizuLib\Class\KizuMacro.h"

// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// ���C���C���N���[�h
#include "..\..\Include\ToHtCommon.h"									// ���������p�C���N���[�h
#include "..\..\Include\OutIF_HT_RAW.h"									// �S����`

// EPC�w�b�_�[
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCAttr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
#include "..\..\Include\EPC\NCL2attr.h"									// �����ʒ�`


// ���[�J���̃N���X
#include "HtRecvBaseManager.h"											// ���聨�����ԒʐM����N���X
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "CoilManager.h"												// �R�C�����Ǘ��N���X
#include "PlgManager.h"													// PLG�Ǘ��N���X
#include "DBManager.h"													// DB�o�^����N���X
#include "MergeManager.h"												// �}�[�W����
#include "DmemManager.h"												// �f�[�^�������ʐM�Ǘ��N���X
#include "AlarmeManager.h"												// �x��Ǘ��N���X

#include "EdgePosControl.h"												// �G�b�W���o�������Ǘ��N���X

#ifndef DISABLE_TINY_CALC
	#include "TinyManager.h"												// �������׊Ǘ��N���X
#endif
#include "DefectDumpManager.h"											// �r���t�@�C���Ǘ��N���X

// �t�@���N�V�����N���X
#include "ToOutMailSender.h"											// �O�����[���X���b�g���M���W���[��


using namespace KizuLib;


class HtRecv :	public HtRecvBaseManager
{
public:
	HtRecv(const char *cSession, int TorB, bool bDistPosition);
	virtual ~HtRecv(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v���C�x�[�g�萔
private:
	static const int PARAM_FRAME_SKIP_CNT				= 100;			// ���v���ԎZ�o�X�L�b�v�t���[����
	static const int DEFECT_BUF_CNT = MAP_COL_NUM * MAP_NEST_NUM;		// �V�X�e���ŏ��Ԋu�ł̍ő��r�o�b�t�@�����O��

public:
	// �r���
	// ���r�ʒu���t���[���͈͊O�̏ꍇ�A���t���[�������܂ŗ��߂Ă����o�b�t�@�p
	struct TYP_DEFECT_BUF
	{
		double dLen;													// �r�����ʒu
		bool bRowKizuCnt;												// �r�������Z�Ώ�(true:�Ώ� false:�ΏۊO)
		DETECT_BASE_DATA data;											// ��{�r���
		DEFECT_DATA_IMAGE img;											// �؂�o���摜�̎���
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �S�r�f�o�b�O�o��
#define LOG_DEF_OUTPUT	// ��`���A�S�r���f�o�b�O�o��
#ifdef LOG_DEF_OUTPUT
private:
	LogFileManager*		mcls_pLogDef;
#endif
#ifdef DEBUG_LOG_EDGE_UDP
	LogFileManager*		mcls_pLogUdp;									// �G�b�W�A���p���O�N���X
#endif
	LogFileManager*		mcls_pLogA;										// �x��p���O�N���X


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v���p�e�B
public:
	//// ���[�J�[�X���b�h
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// �R�C�����Ǘ�
	void SetPlgMgr(PlgManager* pCls)		{ mcls_pPlg = pCls; }		// PLG�Ǘ��N���X
	void SetDbMgr(DBManager* pCls)			{ mcls_pDbMgr = pCls; }		// DB�o�^����N���X

	void SetEdgePosCont(EdgePosControl* p)	{ mcls_pEdgePosCont = p; }	// �G�b�W���o�������Ǘ��N���X
#ifndef DISABLE_GAP_CALC
	void SetGapInfMgr(GapInfManager* p)		{ mcls_pGap = p; }			// ����Y���W�v�N���X
#endif
#ifndef DISABLE_TINY_CALC
	void SetTinyMgr(TinyManager* p)			{ mcls_pTiny = p; }			// �������׃N���X
#endif
	void SetDumpMgr(DefectDumpManager* p)	{ mcls_pDefDump = p; }		// �r���t�@�C���Ǘ��N���X
	void SetArmMgr(AlarmeManager* pCls)		{ mcls_pArm = pCls; }		// �x��Ǘ��N���X
	void SetLogMgrA(LogFileManager* pLog)	{ mcls_pLogA = pLog; }		// �x��p���O�t�@�C���Z�b�g

	//// �L���[
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData=pQue; }	// DB�o�^�p�̃L���[
	void SetQueDmem(ThreadQueue<DmemManager::DELIVERY>* pQue)	{ mque_pDmemAlarme=pQue; }	// �r�����x����o�^�p�̃L���[
#ifdef ENABLE_CYCLE
	void SetQueCycleData(ThreadQueue<DeliveryCYCLE>* pQue) { mque_pCycleData=pQue; }	// �������׎󂯓n���p�̃L���[
#endif




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �C�x���g���J
public:


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O���A�N�Z�X
public:
	static void Create_DetectBaseData(int men, int coilwid, int nMapCol, NCL_DEFECT_RESULT const* pResu, double const* attr, int nShrinkRatio, DETECT_BASE_DATA* data);		// ��{�r��� ���� [�� OpSendFunc]
	//////void		SetEnableAGC(bool bAgc)	{ m_bEnableAGC = bAgc; }			// AGC���s�\��Ԃ�ύX[��MI] 
	int			GetBr(int cam, int angle) { return m_nBr[cam][angle]; }		// ���ꂼ��̃J�����̃G�b�W�����ϋP�x [��MI]


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐�
private:
	virtual void OnExecDetectWaitStart();											// �����J�n�҂��J�n����
	virtual void OnExecDetectRun();													// �����J�n����
	virtual void OnExecDetectStop();												// ������~����

	virtual void OnOpNewConnect();													// �V�K�\���@�\�V�K�ڑ�
	virtual void OnMainDefect();													// �Ɩ��`�� ��M��̃��C������
	virtual void OnMainHls(SOCK_F_HEAD* p);											// �w���V�[�`�� ���C������ �����̏����Ԃ̓u���b�N����Ă���̂Œ���


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ʏ���
private:
	void ExecDefect(double dFramePos);												// ���ה���������
	void SetDetectInf(NCL_DEFECT_RESULT const* pResu, double const* pAttr, double dFramePos, int nShrinkRatio, DEFECT_DATA_IMAGE const* pImg);	// �X���r���̏���


	//// ����
	void ExecMerge();																// �}�[�W�̎��s
	void SetAlameInf(DETECT_BASE_DATA const* pD, double const* pAttr);				// �x��o�͑Ώی��ׂ̔�����s��
	void Check_AGC(DWORD framenum);													// AGC���s
	void SetCoilTrand();															// �R�C���g�����h����o�^
	void ClearDefectBuffer();														// ��\�r�A�r���X�g�N���A
	void ReSetOp_DetectData(int nFramePos);											// ��\�r�I��o�b�t�@������
	void DetectToColRow(TYP_COLROW_BUF *pBuf, double dLen, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg);		// �r����\���Ǘ����M�p�\���̂ɕϊ�
	void SetBuf_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, double dFramePos);// �\���Ǘ����M�p �r���ҏW
	int	 SetOp_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, TYP_COLROW_INFO *pColRow_Info);	// ��\�r�I��
	bool SelectPriorityDetect(DETECT_BASE_DATA const* pNow, DETECT_BASE_DATA const* pPre);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����o�[�ϐ�
private:

	//// �e�C���X�^���X
	CoilManager*			mcls_pCoil;									// �R�C�����Ǘ�
	PlgManager*				mcls_pPlg;									// PLG�Ǘ��N���X
	DBManager*				mcls_pDbMgr;								// DB�o�^����N���X		
	EdgePosControl*			mcls_pEdgePosCont;							// �G�b�W���o�������Ǘ��N���X
	DefectDumpManager*		mcls_pDefDump;								// �r���t�@�C���Ǘ��N���X
	AlarmeManager*			mcls_pArm;									// �x��Ǘ��N���X

	//// �󂯓n���L���[
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DB�N���X �󂯓n���f�[�^�p�L���[
	ThreadQueue<DmemManager::DELIVERY>*		mque_pDmemAlarme;			// �r�����x���� �󂯓n���f�[�^�p�L���[
#ifdef ENABLE_CYCLE
	ThreadQueue<DeliveryCYCLE>*		mque_pCycleData;			// �������׎󂯓n���f�[�^�p�L���[
#endif

	//// AGC�֘A
	//////bool					m_bEnableAGC;									// AGC���s�\(���S���]���p�f�[�^���W����false)
	//////DWORD					m_nAGCSetFrameNo[NUM_CAM_POS][MAX_CAMANGLE];	// �Ō��AGC�ݒ肵��FrameNo [0:DS 1:WS][0:�� 1:��]
	//////int						m_nAGCLastCommand[NUM_CAM_POS][MAX_CAMANGLE];	// �Ō��AGC�ݒ肵���R�}���h[0:DS 1:WS][0:�� 1:��](0:�ύX���� 1:�グ -1:����)	
	//////BYTE					m_nAGCBrCenter[NUM_CAM_POS][MAX_CAMANGLE];		// �G�b�W�����ϖ��邳       [0:DS 1:WS][0:�� 1:��](0�`255�ŃZ�b�g)

	
	//// ���̑��������
	long					m_nNowGridTrandRow;							// ���݂̃g�����h���O���b�h�ʒu (0�I���W��)
	int						m_nPantiTypeId;								// �p���`���r��ID (0�Ȃ���{���Ȃ�)
	int						m_nBr[NUM_CAM_POS][MAX_CAMANGLE];			// ���ꂼ��̃J�����̃G�b�W�����ϋP�x

	//=========================================
	// �\���Ǘ��ւ��r�܂Ƃߑ��M�̂��߂̃��[�N�G���A
	TYP_OP_SEND_DEFECT_INFO m_OpSendDefInfo;
};
