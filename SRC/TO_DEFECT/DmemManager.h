#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Include\OutInterface.h"									// ���@��C���N���[�h

#include "..\..\Include\ToTbl.h"


// ���[�J���̃N���X
#include "MainConst.h"
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "DBManager.h"													// DB�Ǘ��N���X

using namespace KizuLib;


class DmemManager :	public ThreadManager
{
/////////////////////////////////////////////////////////////////////////////////////////////////////////
public :
	//=========================================
	// �O���󂯓n�����
	enum ENUM_KUBUN {
		em_RAMP,														// �r�ڋ߃����v
		em_ALARM,														// �r�ڋ߃A���[��
		em_PATO,														// �p�g���C�g
		em_VOICE,														// �{�C�X
	};

	struct OCCUR_RAMP {
		ENUM_RAMP_STAT	ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// �r�ڋ߃����v�̏��
	};

	struct OCCUR_ALARM {
		ENUM_RAMP_STAT	alarmPos[OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];			// �r�ڋ߃����v�̏�ԁi�W��j
		ENUM_RAMP_STAT	alarm[OUTPUT_ALARM_COL][OUTPUT_RAMP_NUM];					// �r�ڋ߃A���[���̏�ԁi�S�� PIO�o�͗p�j
	};

	struct OCCUR_PATO {
		int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];			// �p�g���C�g�̏��
	};

	struct OCCUR_VOICE {
		ENUM_VOICE_STAT	voice[OUTPUT_VOICE_NUM];									// �{�C�X�̏��
	};

	//========================================
	//// �O�����󂯓n���L���[
	struct DELIVERY {
		ENUM_KUBUN			emNo;										// �C�x���gNo
		void*				data;										// �\���̂̃|�C���^
	};
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
private :
	
	//=========================================
	// �萔
	static const int PARAM_TIME_HLS_READ		= 5000;					// �w���V�[�Ǎ� ����� �^�C�}���� [ms]
	static const int PARAM_TIME_HLS_WRIT		= 1000;					// �w���V�[���� ����� �^�C�}���� [ms]
	static const int PARAM_TIME_STATUS_READ		= 30;					// �X�e�[�^�X�M���ǂݍ��� �^�C�}���� [ms] ����=�d�v WPD�Ȃ�
	static const int PARAM_TIME_STATUS_READ_LOW	= 3000;					// �X�e�[�^�X�M���ǂݍ��� �^�C�}���� [ms] �ᑬ=�P���ȏ��
	static const int PARAM_TIME_STATUS_WRIT		= 60;					// �X�e�[�^�X�M���������� �^�C�}���� [ms]
	static const int PARAM_TIME_PULSE			= 1000;					// �ʏ� �p���X���� [ms]
	static const int PARAM_TIME_PULSE_DEF		= 2000;					// �r�����x��L�� �p���X���� [ms]

	static const int HB_MAX_VAL					= 9999;					// �w���V�[�ő�`�F�b�N�l
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	DmemManager();
	virtual ~DmemManager();

	//=========================================
	// �v���p�e�B
	void SetLogGMgr(LogFileManager* pCls)	{ mcls_pLogG = pCls;}		// �K�C�_���X�p���O�N���X�Z�b�g
	void SetLogMgrA(LogFileManager* pCls)	{ mcls_pLogA = pCls; }		// �x��p���O�N���X�Z�b�g
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;}		// �X�e�[�^�X����N���X�Z�b�g
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// �p�����[�^�Ǘ��N���X�Z�b�g
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData = pQue; }	// DB�o�^�p�̃L���[


	//=========================================
	// �C�x���g���J
	HANDLE		GetEvPI_WPD_KenBaseGateOn() const	{ return m_evPI_WPD_KenBaseGateOn; }	// ������_WPD�Q�[�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_KenBaseGateOff() const	{ return m_evPI_WPD_KenBaseGateOff; }	// ������_WPD�Q�[�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_DspBaseGateOn() const	{ return m_evPI_WPD_DspBaseGateOn; }	// ������_WPD�Q�[�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_DspBaseGateOff() const	{ return m_evPI_WPD_DspBaseGateOff; }	// ������_WPD�Q�[�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_KenBase() const { return m_evPI_WPD_KenBase; }					// ������_WPD [�� MI]
	HANDLE		GetEvPI_WPD_DspBase() const { return m_evPI_WPD_DspBase; }					// �\����_WPD [�� MI]
	HANDLE		GetEvPI_WPD_ShacutF()  const { return m_evPI_ShrcutF; }						// �t�@�[�X�g�J�b�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_ShacutL()  const { return m_evPI_ShrcutL; }						// ���X�g�J�b�g�M�� [�� MI]
	HANDLE		GetEvPI_WPD_PatoReset() const { return m_evPI_PatoReset; }					// �p�g���C�g���Z�b�gPB [�� MI]

	
	//========================================
	// �Q�Ɨp
	SEQ_DM_RECV_BASE&	GetDmRecv() {AutoLock autolock(&m_csLock); return m_typOldDmRecv; }
	// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
	DWORD				GetWpdOnLength() { return m_nWpdOnLength; }
	DWORD				GetWpdOnLengthDsp() { return m_nWpdOnLengthDsp; }
	DWORD				GetWpdOnLengthSub() { return m_nWpdOnLengthSub; }
	// <<===
	
	//========================================
	// �f�o�b�N����p
	SEQ_DM_RECV_BASE*	DebugDmRecv() { return mtbl_pRecv;}
	
	//========================================
	// �X���b�h�L���[
	ThreadQueue<DELIVERY>			mque_AddData;						// IN �󂯓n���f�[�^�p�L���[
private:
	ThreadQueue<DBManager::DeliveryDB>*	mque_pAddSQLData;				// DB�N���X ��n���f�[�^�p�L���[
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J��n
public : 
	void SetData_Pls(EM_EVENT_PLS_OUT emPls, int nVal = 1);				// 1�b�p���X�o��
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PioManager�֐�
public :
	void				PoPuls_CountReset();							// �J�E���^���Z�b�g�v���M�� [��MI]
	void				PoPuls_CountStart();							// �J�E���^�J�n�v���M�� [��MI]
	void				PoPuls_TesetWpdOn(int nPos);					// �e�X�g�p WPD�M�� ON�v���M�� [��MI]
	//// �擾
	//bool				IsPlgReset()		{ return m_bPlgReset; }		// PLG�J�E���^���Z�b�g�v����[��MI�ADlg]
	

	bool				GetPI_KenGateYou()	const { return 1 == mtbl_pRecv->nGateKen ? true : false; }		// �n�ړ_�Q�[�g�M���i�����p�j
	bool				GetPI_DspGateYou()	const { return 1 == mtbl_pRecv->nGateDsp ? true : false; }		// �n�ړ_�Q�[�g�M���i�\���p�j

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�֐�
private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; };										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	//========================================
	//// ����
	void	ReadDmemHls();												// �w���V�[�ǂݍ���
	void	WritDmemHls();												// �w���V�[��������
	void	ReadDmemStatus(SEQ_DM_RECV_BASE& tbl);						// �X�e�[�^�X��ԓǂݍ���
	void	ReadDmemStatusLow(SEQ_DM_RECV_BASE& tbl);					// �ᑬ�p�X�e�[�^�X��ԓǂݍ���
	void	WritDmemStatus();											// �X�e�[�^�X��ԏ�������
	void 	WritDmemRamp(OCCUR_RAMP typRamp);							// �r�ڋ߃����v��ԏ�������
	void	WritDmemAlarm(OCCUR_ALARM typAlarm);						// �r�ڋ߃A���[����ԏ�������
	void 	WritDmemPato(OCCUR_PATO typPato);							// �p�g���C�g��ԏ�������
	void 	WritDmemVoice(OCCUR_VOICE typVoice);						// �{�C�X��ԏ�������
	void 	WritDmemVoiceReset();										// �{�C�X��ԃ��Z�b�g

	int		ChangeState(int now, int old );								// ��ԕω��`�F�b�N
	void	UpdatePlsRate(int* nPls);									// �p���X���[�g�A���蕪��\��DB�X�V

public:
	int		GetVoiceSts(void)	{ return m_nVoice; };					// �{�C�X�o�͏��[��Dlg]
	int		GetVoiceMen(void)	{ return m_nVoiceMen; };				// �{�C�X�o�͖�[��Dlg]
	int		GetRampSts(int men, int col) { return m_nRamp[men][col]; };	// �����v�o�͏��[��Dlg]
	int		GetPatoSts(int pos, int kind)	{ return m_nPato[pos][kind]; };	// �p�g���C�g�o�͏��[��Dlg]
	int		GetAlrmSts(int col) { return m_nAlarm[col]; };				// �A���[���o�͏��[��Dlg]

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private :

	//=========================================
	// �e�C���X�^���X
	LogFileManager*			mcls_pLogG;									// �K�C�_���X�p���O�N���X
	LogFileManager*			mcls_pLogA;									// �x��p���O�N���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�


			
	//========================================
	//// �n���h��
	HANDLE					m_evTimeHlsRead;							// �w���V�[�M���o��
	HANDLE					m_evTimeHlsWrit;							// �w���V�[�M���o��
	HANDLE					m_evTimeStatusRead;							// PIO�X�e�[�^�X�M���ǂݍ���
	HANDLE					m_evTimeStatusWrit;							// PIO�X�e�[�^�X�M����������
	HANDLE					m_evTimeAlarmePulse;						// �r�����x�� �p���X�o�̓^�C�}�[
	HANDLE					m_evTimeCntResetPulse;						// ����V�[�P���T ���@�\�J�E���^���Z�b�g �p���X�o�̓^�C�}�[
	HANDLE					m_evTimeCntStartPulse;						// ����V�[�P���T ���@�\�J�E���^�J�n �p���X�o�̓^�C�}�[
	HANDLE					m_evTimeTestWpdPulse;						// �e�X�g�p WPD�M�� ON�v�� �p���X�o�̓^�C�}�[
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	//========================================
	//// �w���V�[�Ď��p
	int						m_nState;									// �V�[�P���T �f�[�^�������w���V�[���
	int						m_nHBOldVal;								// �O��̓ǂݍ��݃J�E���^�[�l
	
	//========================================
	// �x��֘A
	int				m_nVoice;											// �{�C�X�o�͒����
	int				m_nVoiceMen;										// �{�C�X�o�͒����
	int				m_nPato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];	// �p�g���C�g�o�͒����
	int				m_nRamp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];			// �r�ڋ߃����v�o�͒����
	int				m_nAlarm[OUTPUT_MESYU_SEKKIN_COL];					// �r�ڋ߃A���[���o�͒����

	//========================================
	// ���L���������
	HANDLE					m_hDataMem;									// �f�[�^��������� �n���h��
	TO_SEQMEM_TBL*			mtbl_pDM;									// �f�[�^��������� �f�[�^
	SEQ_DM_RECV_BASE*		mtbl_pRecv;									// �f�[�^��������� �� ��M�G���A
	SEQ_DM_SEND_DEFECT*		mtbl_pSend;									// �f�[�^��������� �� ���M�G���A
	SEQ_DM_RECV_BASE		m_typOldDmRecv;								// �O��l
	SEQ_DM_RECV_BASE		m_typOldDmRecvLow;							// �O��l(�ᑬ�ǂݍ���)
	
	//========================================
	// �e��X�e�[�^�X
	int						m_nPlsRate[3];								// ����V�[�P���T����擾�����p���X���[�g�ݒ�l[0.0001mm/pls]
																		// [0:�����pPLG, 1:������r�pPLG, 2:�\���pPLG]
	bool					m_bInitReadOk;								// True:�^�X�N�N����A����Ǎ����� / False:�^�X�N�N����A����Ǎ����܂�
	
	// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
	//========================================
	//// �O���Q�Ɨp
	DWORD					m_nWpdOnLength;								// WPD��ON���̌����R�C����
	DWORD					m_nWpdOnLengthDsp;							// WPD��ON���̕\���R�C����
	DWORD					m_nWpdOnLengthSub;							// WPD��ON���̔�r�R�C����
	// <<===

	//========================================
	//// �������
	DWORD					m_nStatusReadLowInterval;					// �ᑬ�X�e�[�^�X�Ǎ��O�񎞊� [ms]

	
	//========================================
	// ����p
	HANDLE					m_evPI_WPD_KenBaseGateOn;					// WPD �����Q�[�gON
	HANDLE					m_evPI_WPD_KenBaseGateOff;					// WPD �����Q�[�gOFF
	HANDLE					m_evPI_WPD_DspBaseGateOn;					// WPD �\���Q�[�gON
	HANDLE					m_evPI_WPD_DspBaseGateOff;					// WPD �\���Q�[�gOFF
	HANDLE					m_evPI_WPD_KenBase;							// WPD ����
	HANDLE					m_evPI_WPD_DspBase;							// WPD �\��
	HANDLE					m_evPI_ShrcutF;								// �t�@�[�X�g�J�b�g�M��
	HANDLE					m_evPI_ShrcutL;								// ���X�g�J�b�g�M��
	HANDLE					m_evPI_PatoReset;							// �p�g���C�g���Z�b�gPB

	int						m_nOutPls[EVENT_PLS_OUT_END];				// �f�[�^�Z�b�g1s�p���X�o�� �˗�
	
//private:
//	// PLG�J�E���^���Z�b�g
//	bool					m_bPlgReset;								// True�FPLG�J�E���^���Z�b�g��(PLG�J�E���^���Z�b�g�v����True�AZ������ON��False)
};

