//=====================================================================
// �G�b�W���o�������Ǘ��N���X
//	�y�S���C���z
//		�E��{�I�Ɋ��S���p�\�Ȃ悤�ɍ��
//=====================================================================

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// ���W���[���̊��N���X

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// �W��
#include <math.h>

// ���[�J���̃N���X
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "DBManager.h"													// DB�o�^����N���X


// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`


//// �؂�ւ� ----------------------------------------->>>
#define		EDGE_POS_CONTROL_DBSAVE										// ���т�DB�ɕۑ�����ꍇ�R�����g�A�E�g�B
																		// �G�b�W���o�������@�\�݂̂̏ꍇ�́A�R�����g�ɂ��邱�ƁB
// <<<----------------------------------------------------

using namespace KizuLib;

// �t�@���N�V�����̎g����
class EdgePosControl : public FuncBase //ThreadManager
{
//// ���J�萔
public:

	//=======================================
	// �G�b�W���o���
	enum EM_DIV_EDGE_MODE {
		DIV_EDGE_MODE_NON		= -99,							// [�G�b�W���o���] �������
		DIV_EDGE_MODE_BACK		= -9,							// [�G�b�W���o���] �o�b�N�A�b�v�� (��~��)
		DIV_EDGE_MODE_SKIP		= -1,							// [�G�b�W���o���] �X�L�b�v
		DIV_EDGE_MODE_OK		= 0,							// [�G�b�W���o���] ����
		DIV_EDGE_MODE_ONE,										// [�G�b�W���o���] �ЃG�b�W�D��
		DIV_EDGE_MODE_CENTER									// [�G�b�W���o���] ���C���Z���^�[�U�蕪��
	};


//// �����o�[�萔
private :

//// ���J�֐�
public:
	EdgePosControl(void);
	virtual ~EdgePosControl(void);


	//=========================================
	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }	// �p�����[�^�Ǘ�
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData=pQue; }	// DB�o�^�p�̃L���[

	void SetOldMode(bool bMode) { m_bOldMode = bMode; }					// ���샂�[�h��`
	void SetFrameSkipAddDB(bool bAdd) {m_bFrameSkipAddDB = bAdd; }		// DB�ւ̓o�^

	// �֐��|�C���^


	//=========================================
	// �O������֐�
	void	Init();														// ������ [��MI]
	void	CoilChange(int ou);											// �����I�ȃR�C���ؑ� [��MI]
	void	CoilStart(int ou, char const* cKizukenNo, float coilWid, bool bIsInit=false);	// �_���I�ȃR�C���؂�ւ�����ʔ��Z�b�g [��HtRecv]
	void	CoilEnd(int ou);											// �R�C�����ђ��� [��HtRecv]

	void	SearchEdgeIndex(int ou, float* dEdgePos, int& edgeL, int& edgeR);			// �G�b�W���o�ʒu���� (�X�L����������DS��WS)


	// �G�b�W���o�֘A
	static	CString GetDivEdgeMode(EM_DIV_EDGE_MODE em);				// �G�b�W���o���[�h ����
	bool	SetEdgePos(int ou, long nFrameNo, long nPos, double dResY, float* edge, float* edgemax, float* edgemin, double dSpmNobiHosei);	// �G�b�W���o�ʒu�Z�b�g [��HtRecv]
	void	SearchEdgePos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo=NULL);	// �G�b�W���o�ʒu���� (�X�L����������DS��WS) [��PlgMgr, Dlg]
	void	SearchEdgeRawPos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo=NULL);	// ���G�b�W���o�ʒu���� (�X�L����������DS��WS) [��CoilMgr]
	void	SearchEdgeRawPosMax(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo = NULL);// ���G�b�W���o�ʒu���� (�X�L����������DS��WS) [��CoilMgr]
	void	SearchEdgeRawPosMin(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo = NULL);// ���G�b�W���o�ʒu���� (�X�L����������DS��WS) [��CoilMgr]
	void	SearchEdgeIndex(int ou, int& edgeL, int& edgeR);			// �G�b�W���o�ʒu���� (�X�L����������DS��WS)
	void	SetFrameSkip(int ou, int* nFrameSkip, int* nEdgeNg, int* nFrameSt);			// �����󋵃Z�b�g


	// ��ʊ֌W
	EM_DIV_EDGE_MODE	GetEdgeMode(int ou) const { return m_EdgeInf[ou].emEdgeMode; }
	//float				GetEdgePos(int ou, int cam) const { return m_EdgeInf[ou].dEdgePos[cam]; }
	float				GetEdgePos(int ou, int nLR) const { return m_EdgeInf[ou].dEdgePos[nLR]; }
	float				GetEdgeAnsPos(int ou, int nLR) const { return m_EdgeInf[ou].dEdgeAnsPos[nLR]; }
	bool				GetEdgeAns(int ou) const { return m_EdgeInf[ou].bEdgeAns; }

//// �����o�[�֐�
private:

	// ���ʏ���
	void			GetSearchPos(int nLR, float dCoilWidth, float dDetectRange, float* dPos);	// ����̃T�[�`�͈�


	// �֐��|�C���^ (���[���X���b�g���M�p)
	//(*Send_HtDefect_GetFrameImg)(1, 1, 1); �Ŏ��s


//// �����o�[�ϐ�
private:

	// �C���X�^���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�

	// �󂯓n���L���[
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DB�N���X �󂯓n���f�[�^�p�L���[

	// �G�b�W��Ԋi�[
	struct EDGE_INF {
		DWORD				nFrameNo;									// �t���[��No
		//float				dEdgePos[NUM_CAM_POS];						// �e����@�\�̌��o�G�b�W�ʒu(���l) (0:�G�b�W���o���s)
		float				dEdgePos[2];								// ���o�G�b�W�ʒu(���l) (0:�G�b�W���o���s)
		
		EM_DIV_EDGE_MODE	emEdgeMode;									// �G�b�W���o���
		bool				bEdgeEnable[2];								// �G�b�W���o�L�� (true:�G�b�W���o��) (��/�E)
		float				dEdgeRawPos[2];								// ���o�G�b�W�ʒu(���l)  (��/�E)
		float				dEdgeRawPosMax[2];							// ���o�ő�G�b�W�ʒu(���l)  (��/�E)
		float				dEdgeRawPosMin[2];							// ���o�ŏ��G�b�W�ʒu(���l)  (��/�E)
		float				dEdgeAnsPos[2];								// ���肵���ŏI�G�b�W�ʒu(�␳��)(��/�E)
	
		bool				bEdgeAns;									// �ŏI�G�b�W�ʒu���茋�� (true:����)
	} m_EdgeInf[NUM_MEN];

	// ���уf�[�^�i�[
	struct ZI_COIL_RESULT {
		char				cKizukenNo[SIZE_KIZUKEN_NO];				// �r���Ǘ�No
		bool				bCoilWidth;									// ��ʔ��L��
		float				dCoilWidth;									// ��ʔ� [mm] (�f�[�^�̗���ł̔�)

		// �����X�V
		float				dCoilWidMin;								// ���팟�o���̍ŏ���
		float				dCoilWidMax;								// ���팟�o���̍ő��
		int					nLimitEdgeErrNum;							// �����e�덷�͈͊O��
		int					nLostEdgeNum[2];							// �G�b�W�����o�� (���G�b�W/�E�G�b�W)

		int					nFrameNumCoil;								// �R�C���P�ʂ̃t���[����
		int					nFrameNumSkip[NUM_CAM_POS][5];				// �摜�����X�L�b�v������
	} m_Zi[NUM_MEN];

	// ���[�h
	bool					m_bOldMode;									// �̂̏�Ԃœ��삳����ꍇ true
	bool					m_bFrameSkipAddDB;							// �t���[��No�A�X�L�b�v����DB�ɓo�^������ꍇ true

	// ���[�N�̈�
	int						m_nEdgeSearchSkip[NUM_MEN];					// �T�[�`�͈͏����ݒ�X�L�b�v�� (0:���s 1�`:���̐������X�L�b�v -1�F�����ƃX�L�b�v)
	int						m_nFrameSaveInterval[NUM_MEN];				// �t���[���摜�ۑ��܂ł̑҂��t���[����
};
