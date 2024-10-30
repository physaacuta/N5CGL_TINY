#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"							// SQL�������⏕�N���X

// ���ʃC���N���[�h
#include "..\..\Include\OutInterface.h"											// �O���C���^�[�t�F�[�X�p�w�b�_�[

// �e���[�J�[�X���b�h
#include "MainConst.h"															// ���N���X�萔�w�b�_�[
#include "MakeProManager.h"														// �v���R���������э쐬�Ǘ��N���X

class MakeFfManager : public ThreadManager
{
//// ���J�\����
public:
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY
	{
		char				cKey[SIZE_SERIAL_FF_NO];							// ��L�[
	};
	
//// �����o�[�\����	
private:
	// �e�e�������ѓ`���Ώ��r���
	struct FF_INF {
		//// ����
		bool bOverStatus;			// �I�[�o�[�X�e�[�^�X(�I�[�o�[�����ꍇTrue)
		int nFfCnt;					// FF���ɃZ�b�g�����r��

		//// �Ώۗ\����r��
		// (�O���[�h�D��x�A�r��D��x�A���菇�ɕ���ł���)
		int nDefCnt;				// �r �Z�b�g��
		struct DEF {		
			int		nGid;			// �O���[�h EdasysID
			int		nTid;			// �r�� EdasysID

			int		nRank;			// ���׃O���[�h (0:���Q 1:�y���� 2:������ 3:�d����)
			int		nGPriority;		// �O���[�h�D��x
			int		nTPriority;		// �r��D��x

			int		nNo;			// �rNo
			int		nYpos;			// ����ʒu[mm]  (�␳���l����)
			int		nWS;			// WS�ʒu[mm]
			int		nLen;			// �r���� [mm]

			int		nGFF;			// FF�`������(�O���[�h)
			char	tFF[2+1];		// FF�`������(�r��)

			
			// ���ۂ�FF�T�[�o�[�փZ�b�g�������ɍX�V
			bool	bFFSet;			// FF�Ɏ��ۂɃZ�b�g�����ꍇTrue
			

		} typDef[MAX_FF_DFCT_CNT];		// �r��MAX

	};

	// �e�e�������ѓ`���Ώ��r���
	struct FF_STS {
		int nCnt;				// �r �Z�b�g��
		struct STS {
			int						nID;									// ID (1�`�A��)
			EM_DIV_KENSA 			emKensa;								// ������� (5m�s�b�`)
			EM_DIV_KENSA 			emKensaOut;								// �o��������� (5m�s�b�`)
			EM_DIV_KENSA 			emKensaAna;								// ����������� (5m�s�b�`)
		} sts[MAP_ROW_NUM];
	};

	//=======================================
	// �p�����[�^�֌W
	//=======================================
	// ���ʃp�����[�^
	struct PARAM_COMMON_MAST {
		int	nEdgeNgCount;							// �G�b�W������_�G�b�W���o�ُ�FF���M�B(0:���� 1�`:�G�b�W�����o��ԗL��ɂ���)
	};
	// �O���[�h
	struct PARAM_GRADE
	{
		int nNum;									// ����
		struct GRADE_ID									// �D��x��
		{
			int	nEdasysID;							// edasysID
			int nPriority;							// �D��x (1���ŋ�)
			int nRank;								// ���׃O���[�h (0:���Q 1:�y���� 2:������ 3:�d����)
			
		} typGrade_id[MAX_GRADE];
	};
	// �o�͐ݒ�p�p�����[�^
	struct PARAM_PCODE
	{
		// �o�͏���
		struct OUTPUT								// �o�͍���
		{
			int		nEdasysID;							// �r��edasysID (1�`:�r��ID  0:���g�p  -1:���̑�����)

			int		nKind;								// �O���[�hID (0:���� 1�`:�O���[�hedasysID) �w�肳�ꂽ�O���[�hID�ȏ���r���o�͑Ώ�
		};
		// ����
		struct INF
		{
			// �p�^�[���ʏo�͐ݒ� (PCODE_OUTPUT)
			int		nPutNum;						// output�ɃZ�b�g���Ă��鑍��
			OUTPUT	typOutput[MAX_TYPE];				// �o�͏����ݒ�
			OUTPUT	typDefput;							// �r�햢�ݒ荀�ڂ̏o�͏���

		} rec[MAX_PCODE];							// �ŏI�́A���̑����ڌŒ�
	};


	//=======================================
	// �����M���т̋������M�֌W
	//=======================================
	// �����M���т̏��
	struct UNSENT_DATA
	{
		char	kizukenNo[SIZE_KIZUKEN_NO];										// �Ώۂ̊Ǘ�No
		int		len;															// �R�C������(mm)
		int		pcode[NUM_MEN];													// �����p�^�[���R�[�h
		char	MeiNo[SIZE_MEI_NO+1];											// �L�[��� ���ߔԍ�
		char	CoilNo[SIZE_COIL_NO+1];											// �L�[��� �R�C��No
		int		nSpm;															// SPM�����L��
	};

//// ���J�֐�
public:
	MakeFfManager(void);
	virtual ~MakeFfManager(void);

	void	SetMakeProMgr(MakeProManager* p) { mcls_pProMgr=p; }

//// ���J�ϐ� 
public:
	ThreadQueue<DELIVERY_KEY>	gque_Deli;										// �󂯓n���L���[

//// �����o�[�֐�
private:
	int ThreadFirst();															// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();															// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);												// �X���b�h�C�x���g����

	void ClearMember();															// �������ϐ� ������
	bool Exec(char const* cID);													// �e�e�T�[�o�[�������� �`������

//------------>>> Exec����̈�A�̏��� (���̒��̊֐����� private�̏������ϐ����g�p�\)
	bool GetBufferSai(char const* cID);											// �v���R���̐����ю�M�o�b�t�@�e�[�u���擾
	bool GetCoilInf_KizukenNo();												// �R�C����񂩂��r���Ǘ�No���擾
	bool CheckCoilResultInsertOk(int *nIsWarm, int* nIsSendOk, int* pcode);		// �R�C�����т��o�^�����܂ő҂��B���␳���m��
	bool UpdateProconBuf(const char *cID);										// �̐����уe�[�u�� �� �X�V
	bool UpdateCoilResult();													// �R�C�����уe�[�u�� �� �X�V
	bool InsertCutResult(const char *cID);										// �J�b�g���уe�[�u���ɒǉ�
	bool InsertSaisunResult(const char *cID);									// �̐����уe�[�u���ɒǉ�

	bool MakeSendData(int ou, int pcode, int nIsResult, FF_INF* pInf, FF_JISSEKI* pTypZi);	// FF���M�p ���ѓ`������
	void MakeCutData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi);				// �J�b�g���� FF���тɃZ�b�g����
	void MakeDefectData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi);				// �r���G���A�ɃZ�b�g����
	bool MakeSendStsData(int ou, int pcode, int nIsResult, FF_STS* pPos, FF_STS_JISSEKI* pTypSts);	// FF���M�p ������ԓ`������
	bool GetData_CoilPosition(int ou, FF_STS* pPos);							// �ΏۃR�C���̃R�C���ʒu�����擾����
	void MakeFfManager::MakeStatusData(int ou, FF_STS * pPos, FF_STS_JISSEKI* pTypSts);				// ������ԃG���A�ɃZ�b�g����
	
	int GetStatus_Kensa();														// �ΏۃR�C���̌������(�r���̉ғ���)
	int GetStatus_EdgeDetect(int ou, int nState);								// �ΏۃR�C���̃G�b�W���o�󋵂��擾����

	bool GetData_DefectInf(int ou, int pcode, FF_INF* pInf);					// �ΏۃR�C�����r�����擾����

	bool InsertZiBuf(char const* cID, FF_JISSEKI const* pTypZi, FF_STS_JISSEKI const* pTypSts);		// FF�p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^

	bool InsertAccessNextInf( FF_INF pInf[], FF_JISSEKI pTypZi[]);		// ���H�����e�[�u�����r��񓙂��Z�b�g

	// �p�����[�^�֌W
	bool ReadParam();															// �p�����[�^�Ǎ�
	bool Check_Output(long pcode, long tid, long gid) const;					// �����p�^�[���o�͑Ώۂ̌��ׂ�����
	int GetYusenG(long gid) const;

	// �����M���т̋������M�֘A
	bool ExecUnsentResult(char const* cKizukenNo);						// �����M�̎��т̌��o��FF���M����
	bool GetUnsentCoilResult(char const* cKizukenNo);					// �����M�f�[�^�̃R�C�����т𒊏o
	bool MakeUnsentFFSendData(int nRec);								// �����M���уf�[�^�̐���
	bool UpdateCoilResultForUnsentData(char* cKizukenNo);				// �������M���R�[�h�̃R�C�����уe�[�u�� �� �X�V
																		// �O���[�h�D��x�擾
// <<<-----------


	void Send_To_FF(char const* cKey);											// ��FF�ʐM �ɑ��M�˗�

//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	MakeProManager				*mcls_pProMgr;									// �v���R���p���ѓ`�������N���X

	// ������ (�������̂݃f�[�^���L��)
	OdbcBase					*mcls_pDB;										// DB�ڑ��N���X
	char						m_cKizukenNo[SIZE_KIZUKEN_NO];					// �Ώۂ̊Ǘ�No
	PRO_TYPE_SAISUN				m_typBuf;										// ��M�f�[�^ (�̐�����)

	PARAM_COMMON_MAST			m_Param_CommonMast;								// �r���萔
	PARAM_GRADE					m_Param_Grade;									// �O���[�h 
	PARAM_PCODE					m_Param_Pcode;									// �����p�^�[����
	UNSENT_DATA					m_unsent_data[UNSENT_MAX];						// �������M�Ώۃ��R�[�h��� 

	// ���̑��������
	bool						m_bCreateOk;									// ��������
	double						m_dHosei;										// �␳�� [%]
	int							m_nRecCnt;										// �o�b�N�A�b�v���M���R�[�h����
	int							m_nSentCnt;										// �o�b�N�A�b�v���M�ς݌���(�o�^�ɐ�����������)
	int							m_nLen;											// ����������[mm]
	
	// ini�t�@�C���l
	int							m_nChkCoilDate;									// �R�C�������̑Ώۊ���
	int							m_nRetryCount;									// �R�C�����ъm�F���g���C��
	int							m_nRetryWaitTime;								// �R�C�����ъm�F���g���C����(ms)
	
	int							m_nChkUnSentDate;								// �������M�Ώۃf�[�^�����̑Ώۊ���

};
