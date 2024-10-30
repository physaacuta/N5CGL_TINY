

#pragma once

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X

#include "ParamManager.h"	
#include "ImgSendManager.h"
#include "TestMaeManager.h"

class TestManager : public ThreadManager
{
#define DIV_LEN_POS_SHR_I		DIV_DISP_POS_END
#define DIV_LEN_POS_FINAL		(DIV_LEN_POS_SHR_I+1)

////�@���J�֐�
public:
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY {
		int		id;
		int		speed;								// mpm
		char	cKizukenNo[SIZE_KIZUKEN_NO];		// ��L�[
		int		nUnten;								// �^�]���
		int		nKadou;								// �ғ����
		int		nKiki[2];							// �@����(�\/��)
		int		nKensa[2];							// �������(�\/��)

		int		nMen;								// ��
		int		nY;									// ���苗��(mm)
		int		dDS;								// DS����̋���(mm)
		int		Tid;								// �r��EdasysID
		int		Gid;								// �O���[�hEdasysID
		float	dArea;								// �ʐ�

		int		nCancelNo;							// �L�����Z���Ώۂ��r�A��
		int		nSystem;							// ���o�V�X�e�� (0:�����A1:�o���A2:����)

		int     nLanp[2][2][8];						// �x����
		int		nCnt;								// �r��
		bool    bNotChkDsWs;						// �R�C�����̊m�F���t���O
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	TestManager(void);
	virtual ~TestManager(void);


	// �R�C�����\����
	struct TEST_COIL_INF {
		char	cKizukenNo[SIZE_KIZUKEN_NO];
		char	cCoilNo[SIZE_COIL_NO+1];
		char	cMeiNo[SIZE_MEI_NO+1];
		long	nCoilLen;					// �R�C����[mm] 
		int		nCoilWidth;					// �R�C����[mm] �Ƃ肠����[mm]�P�ʂƂ��邪�A[0.1mm]�P�ʂ��K�v���H
		int		nLen;						// �ʔ���(���݂̌�����_WPD�ʒu�ɂ�����R�C����[����̋���)[mm] 
		long	scode[NUM_MEN];				// �����\�ʏ��  (1�I���W��)
		long	pcode[NUM_MEN];				// �����p�^�[��  (1�I���W��)
		long	nMapColNum;					// �}�b�v�� 
		double	dNobiSPM;					// SPM�L�ї�[10^2%]
		long	nSpmUmu;					// SPM�����L��
		int		nKizuSeq;					// ���݌����Ώۂ��r�A��
		//int		nCycSeq;					// ���݌����Ώۂ��r�A��(����)
		int		nCutNum;					// �V���[�J�b�g��
	};

	// ��������
	struct DSP_INF {
		char	Name[128];						// �����䖼��
		int		nDistance;						// ������_����̋���
		char	cKizukenNo[SIZE_KIZUKEN_NO];	// ������ʒu�ʉߒ��R�C���̊Ǘ�No
		int		nRec;							// ������ʒu�R�C���̃��R�[�h�ԍ�(1�I���W��)
	};

	//=========================================
	enum DELIVERY_ID {
		EM_TIMER = 0,
		EM_COIL_ADD,							// �R�C�����ǉ�
		EM_PLG_STOP,							// PLG��~
		EM_PLG_START,							// PLG�N��
		EM_PLG_SLOW,							// PLG�P����
		EM_SPEED_SETTING,						// ���x�ݒ�
		EM_INIT_NOTIFY,							// �������ʒm
		EM_PARAM_NOTIFY,						// �p�����[�^�ύX�ʒm
		EM_STATUS_NOTIFY,						// ��Ԓʒm
		EM_COIL_CUT,							// �J�b�g��񑗐M
		EM_DEFECT,								// �r��񑗐M
		EM_DEFLST_CANCEL,						// �d�����r���폜�v��
		EM_ALARM,								// �x���񑗐M
		EM_DEFECT_NUM							// �r����� (�ŏ���ԓ�)
	};

	//=========================================
	// �Z�b�g
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// �p�����[�^�Ǘ�
	void SetOpSend(KizuOpSend *pCls)			{ mcls_pOpSend = pCls; };	// �\�P�b�g�Ǘ�
	void SetImgSend(ImgSendManager *pCls)		{ mcls_pImgSend = pCls; };	// �r�摜���M�Ǘ�
	void SetMaeMgr(TestMaeManager *pCls)		{ mcls_pMaeMgr = pCls; };	// �O�H���Ǘ�
	void SetCoilLenMax(int len)					{ m_nCoilLenMax = len; };	// �R�C�����ő�l(mm)
	void SetCoilLenMin(int len)					{ m_nCoilLenMin = len; };	// �R�C�����ŏ��l(mm)
	void SetCoilWidthMax(int len)				{ m_nCoilWidthMax = len; };	// �R�C�����ő�l(mm)
	void SetCoilWidthMin(int len)				{ m_nCoilWidthMin = len; };	// �R�C�����ŏ��l(mm)
	void SetMaeInfoStatus(bool flg)				{ m_bMaeInfo = flg; };		// �O�H�����Z�b�g
	void SetYousetuCut(bool flg)				{ m_bYousetuCut = flg; };	// �n�ړ_�J�b�g���{�Z�b�g
	void SetDefectCreate(bool flg)				{ m_bDefCreate = flg; };	// �r�����L��E����
	void SetDispPlg(bool flg)					{ m_bDispPlg = flg;	};		// �\��PLG
	void SetDefRate(int rate)					{ m_nDefRate = rate; };		// �r������
	void SetSendPos(bool flg)					{ m_bSendPos = flg; };		// �ʔʒu��񑗐M�L��E����
	void SetSCode(int scode)					{ m_scode = scode; };		// SCODE�ݒ�
	void SetPCode(int pcode)					{ m_pcode = pcode; };		// PCODE�ݒ�


	//=========================================
	// �Q�b�g
	PRO_TYPE_DATA_COILINF const* GetCoilInf() { return m_pCoilInf; };		// �R�C�����擾
	TEST_COIL_INF const* GetTestCoilInf() { return m_pTestCoilInf; };		// �e�X�g�p�R�C�����擾
	DSP_INF const* GetDspInf() { return m_pDspInf; };						// ������ʒu���
	int GetSpeed() { return m_nSpeed; };									// ���ݑ��x���
	int GetCoilLenMin() { return m_nCoilLenMin; };							// ���݃R�C���� �ŏ��l(mm)
	int GetCoilLenMax() { return m_nCoilLenMax; };							// ���݃R�C���� �ő�l(mm)
	int GetCoilWidthMin() { return m_nCoilWidthMin; };						// ���݃R�C���� �ŏ��l(mm)
	int GetCoilWidthMax() { return m_nCoilWidthMax; };						// ���݃R�C���� �ő�l(mm)
	bool GetMaeInfoStatus() { return m_bMaeInfo; };							// �O�H����� true:�쐬���� false:�쐬���Ȃ�
	int GetDefRate(){ return m_nDefRate; };									// �r������

	//=========================================


//// �����o�֐�
private:


	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);				

	void AddNewCoil();													// �R�C�����V�K����
	void SendStatusInfo();												// 0.�X�e�[�^�X���Z�b�g
	void SendCoilInfo(int nRec,int nDiv);								// 3.�R�C����� (�����p) / 	5.�R�C����� (�\���p) �Z�b�g
	void SendCoilResult(int nRec);										// 4.�R�C������ (1�R�C������������)	�Z�b�g
	void SendPosInfo(int nRec);											// 5.�ʔʒu���(�������u�ʒu�A�\����_�ʒu�A�e�\���R�C���ʔʒu���)
	void SendCutInfo(char const* cKizukenNo , int nPos, int nMode=0);	// 6.�J�b�g���
	void SendAlarm(DELIVERY_KEY* pDeli);								// 7:�x����
	void SendDefCount(DELIVERY_KEY* pDeli);								// 9:�r����� (�ŏ���ԓ�)
	void SendDefLstCancel(char const* cKizukenNo, int nMen, int nCancelNo, int nSystem);	// 9:�d�����r���폜�v��

	void MakeDefect(int nRec , int nMen, int from , int to );			// �r�����v������
	void MakeDefectManual(char const* cKizukenNo, int nMen, int nY , int nDSdiv , int Tid, int Gid, float nArea, bool bNotChkDsWs);
																		// �w�肳�ꂽ�r���𑗐M
	void UpdateDispInf(int nRec);										// ������ʒu���X�V
	void TimerEvent( void );
	void Exec( int id ,DELIVERY_KEY* pDeli);
	void Send_Hy_Init( void );											// HY_DEFECT�ɏ������ʒm�𑗐M����
	void Send_Hy_Param( void );											// HY_DEFECT�Ƀp�����[�^�ύX�ʒm�𑗐M����
	void SetKensadaiName( void );										// �����䖼�̐ݒ�

	void SendMaeInf(int nRec);											// �O�H�����Ǘ��Ƀf�[�^�o�^�v��

private:
//// �����o�ϐ�

	//=========================================
	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g�i������_PLG�C�x���g�j
	HANDLE					m_evTimerDisp;								// �\����_PLG�C�x���g

	//=========================================
	//// �e�C���X�^���X
	ParamManager*					mcls_pParam;	
	KizuOpSend*						mcls_pOpSend;
	ImgSendManager*					mcls_pImgSend;
	TestMaeManager*					mcls_pMaeMgr;

	DBSaveManager*					mcls_pDbs;							// DB�����݃N���X	
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// �󂯓n���f�[�^�p�L���[ (�����ɓ������L���[�͑S�ēo�^����)

	//// �ϐ�
	PRO_TYPE_DATA_COILINF*			m_pCoilInf;
	TEST_COIL_INF*					m_pTestCoilInf;
	DSP_INF*						m_pDspInf;
	COIL_BASEDATA*					m_pCoilBase;


	int m_nRecNo;			// ���݌����ʒu�ʔ��̃R�C�����R�[�h�ԍ� 0:��񖳂� 1�` ���R�[�h�ԍ�

	int m_nSpeed;			// ���ݑ��x (mpm)

	int m_nUnten;			// �^�]���
	int m_nKadou;			// �ғ����
	int m_nKiki[NUM_MEN];	// �@����
	int m_nKensa[NUM_MEN];	// �������
	int m_nCoilLenMin;		// �R�C���� �ŏ�[mm]
	int m_nCoilLenMax;		// �R�C���� �ő�[mm]
	int m_nCoilWidthMin;	// �R�C�����ŏ�[mm]
	int m_nCoilWidthMax;	// �R�C�����ő�[mm]

	bool m_bMaeInfo;	// �O�H�������쐬����E���Ȃ�
	bool m_bYousetuCut;	// �n�ړ_�J�b�g�����{����E���Ȃ�
	bool m_bDefCreate;	// �r����������E���Ȃ�

	bool m_bDispPlg;		// �\��PLG true:���� false:��~
	bool m_bSendPos;		// �ʔʒu���𑗐M����E���Ȃ�

	int m_scode;				// �\�ʌ����p�^�[��
	int m_pcode;				// �����p�^�[��
	int m_nDefRate;				// �r������ �ŏ���ԓ��ɂ������r�𔭐������邩(MAX 10)
	bool m_bInitial;			// �������ς�

};
