//=====================================================================
// �f�[�^�x�[�X �x�[�X�Ǘ��N���X
//	�y�S���C���z
//		�E�X���b�h�Ԏ󂯓n���A�X���b�h�N���ȂǊ�{������S��
//		�EBCP���䏈�����
//=====================================================================

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X
#include "..\..\Library\KizuBCP\BCPBase.h"								// DB�����������݃N���X
#ifndef JPEGLIB_LJT
#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap��Jpeg�ϊ��N���X
#else
#include "..\..\Library\DefectLib\KizuLjtJpegManager.h"					// Bitmap��Jpeg�ϊ��N���X
#endif
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// ���Ԍv���Ǘ��N���X

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J��
#include "StatusManager.h"											// �X�e�[�^�X����N���X

using namespace KizuLib;

class DbBaseManager :	public ThreadManager
{
//// ���J�萔
public:

	//=========================================
	// �󂯓n���\����
	typedef struct {
		int					kubun;										// �ǂ̍\���̂� (ENUM_KUBUN)
		BYTE*				data;										// �\���̂̃|�C���^
	} DeliveryDB;
	static const int		SIZE_SQLINF_NUM				= 10;			// ���וR�t���p�o�b�t�@����


//// �����o�[�萔
protected :
	//=========================================
	// ���[�J���萔
	static const int		THREAD_DBSAVE				= 4;			// SQL�������ۂɏ������ސl�̐�
#ifndef ADDDBCOUNT_QUE_SIZE_CHANGE
	static const int		MAX_SQLCOUNT				= 10000;		// SQL�������N���X(���N���X)�̎󂯓n���L���[�T�C�Y
	static const int		MAX_ADDDBCOUNT				= 5000;			// DB�����݃N���X�ւ̎󂯓n���L���[�T�C�Y
#else
	static const int		MAX_SQLCOUNT				= 200000;		// SQL�������N���X(���N���X)�̎󂯓n���L���[�T�C�Y
	static const int		MAX_ADDDBCOUNT				= 200000;		// DB�����݃N���X�ւ̎󂯓n���L���[�T�C�Y
#endif


	// �r���ۑ�����t���[�p
	static const int		RATE_LOWGR_IMGSKIP_SQL		= 60;			// ���QGr�摜�������ۑ�[%]
	static const int		RATE_LOWGR_IMGSKIP_DB		= 60;
	static const int		RATE_DEST_LOWGR_SQL			= 45;			// ���QGr��j��[%]
	static const int		RATE_DEST_LOWGR_DB			= 45;
	static const int		RATE_ONLY_MAXDEF_SQL		= 35;			// 1�t���[�����̍ő��r�̂ݕۑ�[%]
	static const int		RATE_ONLY_MAXDEF_DB			= 35;
	static const int		RATE_SKIP_END_SQL			= 30;			// �r�ۑ��X�L�b�v�I��[%]
	static const int		RATE_SKIP_END_DB			= 30;
	static const int		RATE_ONLY_MAXHIGH_SQL		= 25;			// 1�t���[�����̍ő��r ���� �L�QGr�̂ݕۑ�[%]
	static const int		RATE_ONLY_MAXHIGH_DB		= 25;
	static const int		RATE_SKIP_START_SQL			= 15;			// �r�ۑ��X�L�b�v�J�n[%]
	static const int		RATE_SKIP_START_DB			= 15;



			


	static const int		TIME_BCP_AUTO_COMMIT		= 5*60*1000;	// �Ō���r������������ŁA���̎��Ԉȏ�o�߂����ꍇ�́A�����I�ɃR�~�b�g������
	static const int		TIME_BCP_STATUS_CHECK		= 2*60*1000;	// ��ԃ`�F�b�N�p������^�C�}�[
	static const int		TIME_SAVE_SKIP_WAIT			= 3*1000;		// 1����я����ݒx�ꂪ���������ꍇ�A���̎��Ԓ��ُ�ɂ�����


	//=========================================
	// ���׌��R�t���p
	struct TYP_SQL_INF
	{
		bool				bResult;									// �R�C�����ѓo�^�ςݎ� true

		char				cKizukenNo[SIZE_KIZUKEN_NO];				// �Ǘ�No
		int					nTotalCnt[NUM_MEN];							// �S����
		int					nDeleCnt[NUM_MEN];							// �L�QGr�̔j������
		int					nLowGrDeleCnt[NUM_MEN];						// ���QGr�̔j������
		int					nSaveCnt[NUM_MEN];							// �ۑ���
		int					nSaveImgAllCnt[NUM_MEN];					// �ۑ��摜��
		int					nSaveImgCnt[NUM_MEN];						// �ۑ��摜��(����`�F�b�N�p) 1CAPL�����ł��r���Ɠ��������ۑ��̂��ߒl�͓���邪�s�g�p
		bool				bMaxSave[NUM_MEN];							// �ő�ۑ��������B�� true

		int					nImgDeleCnt[NUM_MEN];						// �L�QGr���r�摜�j������
		int					nImgLowGrDeleCnt[NUM_MEN];					// ���QGr���r�摜�j������
	};


//// ���J�֐�
public:
	DbBaseManager(const char *cThreadName);
	virtual ~DbBaseManager(void);

	void Alloc();														// �֐�������
	void Free();														// �֐��J��

	void Send_To_DbManage(bool bWrite);									// DB�폜�Ǘ��ɒʒm


	//=========================================
	// �v���p�e�B
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls;};		// �X�e�[�^�X����N���X�Z�b�g


	//=========================================
	// �p�����[�^
	void	SetJpgQuality(int nQuality) { m_nJpgQuality = nQuality; }


	//=========================================
	// ��ʗp�A�N�Z�X
	void ReSetMaxCount();
	int GetQueData_Count()	const { return gque_AddSQLData.GetCount();}		// �o�^�L���[
	int GetQueData_Max()	const { return gque_AddSQLData.GetMaxCount();}	// �o�^�ő�L���[
	int	GetQueDB_Count()	const { return mque_AddDB.GetCount(); }			// �o�^�L���[
	int	GetQueDB_Max()		const { return mque_AddDB.GetMaxCount(); }		// �o�^�ő�L���[

	double	GetAveTime()	const { return mcls_Time.GetAveTime(); }		// �A�v�����̕��Ϗ������� [ms]
	double	GetMaxTime()	const { return mcls_Time.GetMaxTime(); }		// �A�v�����̍ő又������ [ms]
	
	double	GetAveSqlTime()	const {											// LIB���̕��Ϗ������� [ms]
		double dWk = 0.0;
		for(int ii=0; ii<THREAD_DBSAVE; ii++) dWk += mcls_pDbs[ii]->GetAveTime();
		return dWk/THREAD_DBSAVE; }	
	double	GetMaxSqlTime()	const {											// LIB���̍ő又������ [ms]
		double dWk = 0.0;
		for(int ii=0; ii<THREAD_DBSAVE; ii++) dWk = max(dWk, mcls_pDbs[ii]->GetMaxTime());
		return dWk; }		
	

	const char* GetInf_KizukenNo(int rec) const { return m_SqlInf[rec].cKizukenNo; }
	int	GetInf_TotalCnt(int rec, int men) const { return m_SqlInf[rec].nTotalCnt[men]; }
	int	GetInf_DeleCnt(int rec, int men)  const { return m_SqlInf[rec].nDeleCnt[men]; }
	int	GetInf_LowGrDeleCnt(int rec, int men)  const { return m_SqlInf[rec].nLowGrDeleCnt[men]; }
	int GetInf_ImgDeleCnt(int rec, int men) const{ return m_SqlInf[rec].nImgDeleCnt[men]; }
	int GetInf_ImgLowGrDeleCnt(int rec, int men) const { return m_SqlInf[rec].nImgLowGrDeleCnt[men]; }
	int GetInf_SaveCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveCnt[men]; }
	int GetInf_SaveImgAllCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveImgAllCnt[men]; }
	int GetInf_SaveImgCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveImgCnt[men]; }


//// ���J�ϐ�
public : 
	//// �X���b�h�L���[
	ThreadQueue<DeliveryDB> gque_AddSQLData;							// �󂯓n���f�[�^�p�L���[



//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	//=========================================
	// ���z�֐�
	virtual void ThreadFirst_AddHandle() {};							// �V�O�i���҂��s��̒ǉ�
	virtual void OnAddSignal(int nEventNo) {};							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	virtual void OnAlloc(bool bMode=true) {};							// �֐�������
	virtual void OnFree() {};											// �֐��J��
	virtual void OnQueDb(int emNo, BYTE* pData) {};						// DB�o�^�˗�
	virtual void OnBcpAutoCommit(bool bAuto) {};						// BCP�����R�~�b�g�p
	virtual void OnBcpStatusCheck() {};									// BCP��ԃ`�F�b�N�p(������^�C�}�[�C�x���g)
	virtual void OnReSetMaxCount() {};									// ���Z�b�g�����ǉ�

	//=========================================
	// ���׌��R�t���p
public:
	void ChangeSQLInf(const char* cKizukenNo);							// �o�b�t�@�؂�ւ�
	int  GetRecSQLInf(const char* cKizukenNo);							// �o�b�t�@���R�[�h����

protected:
	//=========================================
	// BCP�֘A
	bool BCP_Connect(BCPBase* cls_bcp, char const* pTableName);			// BCP�N���X �R�l�N�g
	void BCP_Dispose(BCPBase* cls_bcp);									// BCP�N���X �J��
	bool BCP_SendRow(BCPBase* cls_bcp);									// BCP�N���X �f�[�^�]��
	bool BCP_Batch(BCPBase* cls_bcp);									// BCP�N���X �f�[�^������
	bool BCP_ColLen(BCPBase* cls_bcp, int coulmn, long nSize);			// BCP�N���X �o�C���h�f�[�^�T�C�Y�w�� (varbinary�o�^���Ȃ����ۂ́A�b��΍��p)

	void BCP_AutoCommitTimerOn();										// BCP�����R�~�b�g�^�C�}�[�J�n

//// �����o�[�ϐ�
protected :

	//=========================================
	// �e�C���X�^���X
	DBSaveManager*			mcls_pDbs[THREAD_DBSAVE];					// DB�����݃N���X	
	KizuPerformanceCounter	mcls_Time;									// ���ԑ���X���b�h
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X


	// �X���b�h�L���[
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// �󂯓n���f�[�^�p�L���[ (�����ɓ������L���[�͑S�ēo�^����)


	//=========================================
	// �V�O�i��
	HANDLE					m_evTimeBcpAutoCommit;						// �����R�~�b�g�҂��^�C�}�[
	HANDLE					m_evTimeBcpStatusCheck;						// �X�e�[�^�X�`�F�b�N�^�C�}�[

	//=========================================
	// ���̑��������
	TYP_SQL_INF				m_SqlInf[SIZE_SQLINF_NUM];					// ����10�����̏�� (0:�ŐV 1:��O ���)
	bool					m_bQueState;								// �L���[�󂫏��
	bool					m_bBcpState;								// BCP�ڑ����

	//=========================================
	// �p�����[�^
	int						m_nJpgQuality;								// Jpeg�i��
	bool					m_bDeleteDbMode;							// �폜���[�h (true:�������ݗD��)


	//=========================================
	// ���ѕۑ��X�L�b�v �p
public:
	void	SetEvSaveSkip(int nMen)			  { SetEvent(m_evNowSaveSkip[nMen]); }
	void	SetSaveSkipId(int nMen, int nVal) { m_nSaveSkipId[nMen] = nVal; }
	bool	IsSaveSkipState(int nMen) const   { return m_bSaveSkipState[nMen]; }

	void	SetSaveSkipAdd(char const* cKizukenNo, int nMen, int nLowGrFlg);

	int		SaveDefectCheck(char const* cKizukenNo, int nMen, int nLowGrFlg, int nFrameDefectMax);	// �r���ѕۑ� �t���[����
	int		SaveDefectCheck(int nRec, int nMen,	int nLowGrFlg, int nFrameDefectMax);					// �r���ѕۑ� �t���[����
	int		SaveMaxCheck(char const* cKizukenNo, int nMen, int nLowGrFlg);											// �r���ѕۑ� ����m�F
	int		SaveMaxCheck(int nRec, int nMen, int nLowGrFlg);														// �r���ѕۑ� ����m�F
	void	SaveMaxReset(int nRec);																					// �r���ѕۑ� ������Z�b�g

	int		ResetInf_SaveImgCnt(char const* cKizukenNo, int nTorB);													// �r�摜�ۑ������Z�b�g


private:
	// �n���h��
	HANDLE	m_evNowSaveSkip[NUM_MEN];										// ���ѕۑ��X�L�b�v����
	HANDLE	m_evTimeSaveSkip[NUM_MEN];										// ���ѕۑ��X�L�b�v�^�C�}�[

	// �ێ����
	int		m_nSaveSkipId[NUM_MEN];											// ���ѕۑ��X�L�b�v�@����ID
	bool	m_bSaveSkipState[NUM_MEN];										// ���ѕۑ��X�L�b�v���

	void	SaveSkipStart(int nMen);										// ���ѕۑ��X�L�b�v �J�n
	void	SaveSkipEnd(int nMen);											// ���ѕۑ��X�L�b�v ����
};
