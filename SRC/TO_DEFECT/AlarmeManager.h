#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "CoilManager.h"												// �R�C�����Ǘ��N���X
#include "PlgManager.h"													// PLG�Ǘ��N���X
//#include "PioManager.h"													// Pio�Ǘ��N���X
#include "DmemManager.h"												// �f�[�^�������Ǘ��N���X

#include "OpSendFunc.h"													// �\���Ǘ����M�N���X

using namespace KizuLib;

class AlarmeManager :	public ThreadManager
{

//// ���[�J���萔
private :

	//=========================================
	// �萔
	static const int TIME_INTERVAL_EXEC			= 60;					// ������������s �^�C�}���� [ms] (360mpm�̂Ƃ� 100ms�� 600mm�i��)
	static const int TIME_INTERVAL_ALARM		= 2000;					// ���׌��o�{�C�X�o�͊��� [ms]

	//=========================================
	// ���z�}�b�v
	static const int VIRTUALROW					= 1000;					// �O��100m�Ƃ����Ӗ����� 100���w��

	// �x��o�͗p ���z�}�b�v
	struct TYP_VirtualMap {
		struct ARY {
			int rec;						// ���R�[�h
			int row;						// �s (0�I���W��)
		} ary[VIRTUALROW*2+1];			// �O��100m�Ǝ����̈ʒu (100���e�\���ʒu 0:������ 200:�㗬��)
	};
	

//// ���J�֐�
public:
	AlarmeManager(void);
	virtual ~AlarmeManager(void);

	//=========================================
	// �v���p�e�B
	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}		// �X���b�h�ԓ�������N���X
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;}		// �X�e�[�^�X����N���X�Z�b�g
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// �p�����[�^�Ǘ�
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// �R�C�����Ǘ�
	void SetPlgMgr(PlgManager* pCls)		{ mcls_pPlg = pCls; }		// PLG�Ǘ��N���X
	void SetLogGMgr(LogFileManager* pCls)	{ mcls_pLogG = pCls;}		// �K�C�_���X�p���O�N���X
	void SetLogMgrA(LogFileManager* pCls)	{ mcls_pLogA = pCls; }		// �x��p���O�N���X

	void SetQue_Pio(ThreadQueue<DmemManager::DELIVERY>* pQue) { mque_pAddDmData = pQue;}


	////=========================================
	//// �O������֐�
	void			Init_Pato();										// ������ (�� MI, local)
	bool			Add_Pato(char const* cKizukenNo, int nMen, int nKizuSeq, long y);	// �p�g���C�g�r �ǉ� (��HYRecv)
	void 			AlarmCancel_Pato(char const* cKizukenNo);			// ���׌��o�x�����
	void 			Reset_Pato();										// ���Z�b�gPB����


	//=========================================
	// �O���A�N�Z�X
	ALARME_OUTPUT const*	GetAlarmeData() const	{ return &m_NowAlarme; }
	void SetVoicErrCancel()			{ m_bVoicErrCancel = true;}			// �ݔ��ُ�{�C�X�o�̓L�����Z�����s (���한�A����܂ŗL��)
	void SetPatoManualOn(bool b)	{ m_bPatoManualOn = b; }			// �p�g���C�g����ON


	//=========================================
	// ��ʐ�p
	int				GetAlmBufCnt(int ou)const { return (int)mlst_AlmBuf[ou].size(); }	// �p�g���C�g�o�b�t�@����


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void Output_Alarme();												// �r�ڋ� �x��o��
	void Check_Alarme(ALARME_OUTPUT* p);								// �r�ڋ� �x��o�͔���

	//// �f�[�^�������o�̓`�F�b�N�֘A
	void Check_VoiceErr();												// �ݔ��ُ�{�C�X�o�̓`�F�b�N
	void Check_DetectSekkin();											// �r�ڋߔ��� (CAPL�̂�)

	// ���ʏ���
	void Create_VirtualMap(int pos, TYP_VirtualMap* map, int nMapMax, int nMesyu, int nRowMax);	// �\���ʒu����_�Ƃ��āA�O��100m�̃}�b�v�����
	bool Check_NoAlarm(int rec, int row, int nMesyu);					// �s���є͈͓����`�F�b�N
	bool Check_NoLamp(int rec, int row, int nMesyu);					// �s���є͈͓����`�F�b�N

//// �����o�[�ϐ�
private:
	
	//=========================================
	// �e�C���X�^���X
	LockManager*			mcls_pLock;									// �X���b�h�ԓ�������N���X

	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�
	CoilManager*			mcls_pCoil;									// �R�C�����Ǘ�	
	PlgManager*				mcls_pPlg;									// PLG�Ǘ��N���X
	LogFileManager*			mcls_pLogG;									// �K�C�_���X�p���O�N���X
	LogFileManager*			mcls_pLogA;									// �x��p���O�N���X

	//// �L���[
	ThreadQueue<DmemManager::DELIVERY>* mque_pAddDmData;				// �󂯓n���f�[�^�p�L���[

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g

	
	//// �f�[�^�������Z�b�g����p
	bool					m_bVoicErrCancel;							// �ݔ��ُ�{�C�X�o�̓L�����Z�� (true:�L�����Z����) (���한�A����܂ŗL��)
	int						m_nVoicErrNo;								// ���ݏo�͒��̐ݔ��ُ�{�C�XNo
	int						m_nVoicDetectNo;							// ���ݏo�͒��̌��׌��o�{�C�XNo
	int						m_nVoicDetectTorB;							// ���ݏo�͒��̌��׌��o�{�C�X�̕\���敪 (0:�\ 1:��)
	int						m_nPato;									// �p�g���C�g�o�͌���
	int						m_nSekkin[2][NUM_MEN][MAP_COL_NUM];			// �r�ڋ߃����v/�A���[�� [0:�\������ 1:��������][0:�\�� 1:����][��]  (2:�d 1:�y 0:����)
	
	
	//// �f�o�b�N�p
	bool					m_bPatoManualOn;							// �p�g���C�g����ON

	//// �^�C�}�[�֘A
	UINT_PTR				m_tId_Alarme;								// ���׌��o�A���[���^�C�}�[ID

	//=========================================
	// �ێ��f�[�^
	ALARME_OUTPUT			m_NowAlarme;								// ���݂��r�ڋߌx��̏��


	//// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����

	//// �o�b�t�@�Ǘ�
	std::list<PATO_INF*>	mlst_AlmBuf[NUM_MEN];						// �p�g���C�g�Ώ��r�p�̃o�b�t�@
};
