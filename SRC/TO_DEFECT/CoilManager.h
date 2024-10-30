#pragma once

// ���ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// �C���N���[�h
#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u�� �C���N���[�h
#include "..\..\Include\OutInterface.h"									// �O���C���^�[�t�F�[�X�p�C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "CoilBaseManager.h"											// �R�C������{�N���X
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ�
#include "DBManager.h"													// DB�o�^����N���X

using namespace KizuLib;

class CoilManager :	public CoilBaseManager
{

public:
	CoilManager(void);
	virtual ~CoilManager(void);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W �R�C�����
public:
	COIL_INF*	GetCoilKen_DataSet(int men) { return m_pCoilKenV[men];}				// �f�[�^�Z�b�g�p [���N���g���ĂȂ�]
	COIL_INF*	GetCoilRec_DataSet(int rec){ return &m_CoilInf[rec];}				// �w�肳�ꂽ���R�[�h�̃R�C�����擾�{�擾��Ńf�[�^�ύX�\

	int					GetRecKey(char const* cMeiNo, char const* cCoilNo);			// ���R�[�h�ʒu�擾�i����No�A���YNo�j


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W �s�P�ʊ֌W
	void SetData_StKensa(int men, int rec, EM_DIV_KENSA st);						// ���R�C�����Ɍ�����Ԃ��Z�b�g [��PlgMgr]
	void SetData_StKiki (int men, int rec, EM_DIV_KIKI  st);						// ���R�C�����ɋ@���Ԃ��Z�b�g [��PlgMgr]
	void SetData_Edge   (int men, int rec, float* dEdge);							// ���R�C�����ɃG�b�W�ʒu���Z�b�g [��HtRecv]
	void SetData_Row_V	(int men, int rec, int index, int val);						// ���R�C�����ɍs�P�ʂ̏����Z�b�g [��HtRecv] 
	void SetData_Row_R	(int men, int rec, int index, int val);						// ���R�C�����ɍs�P�ʂ̏����Z�b�g [��PlgMgr]
	void SetData_Row_V_Ls(int men, int rec);										// ���R�C�����̍ŏI���R�[�h���X�V [��HtRecv]
	void SetData_Row_R_Ls(int men, int rec);										// ���R�C�����̍ŏI���R�[�h���X�V [���N���g���ĂȂ�] 
	void SetData_CoilPosition(bool mode, int men);									// �R�C���ʒu���𐶐� [��CoilMgr, HtRecv]



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W �R�C���P�ʊ֌W
	void SetData_CoilResult_Real(int endMode, double dLen, double dLenSub);			// �����_���R�C�����т𐶐� (���ۂ̕��̗���̃V��) [��MI]
	void SetData_CoilResult_Virtual();												// �����_���R�C�����т��Z�b�g (�f�[�^�̗���̃V��) [��MI, HtRecv]
	void SetData_ShaCut(double dLenIn, int mode=0);									// �V���[�J�b�g���s(mode=0:�ʏ�J�b�g�A1:���ъm�莞)[��MI]
	void SetData_StartWpd(int nMode);												// �R�C���擪�̐ؑ֏��� [��MI]
	void SetData_CoilCnt();															// �����J�n����N�{�� [��MI]
	void SetData_CoilResult_BrVal(int* pBr);										// ���G�b�W�����ϋP�x���R�C�����тɃZ�b�g [��MI]
	void SetData_CoilResult_AgcVal(int* pExp);										// ���I�����Ԃ��R�C�����тɃZ�b�g [��MI]
	void SetData_CoilLenYotei(double dInLen);										// �\��R�C�����Z�b�g [��MI]


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����W ���̑�
	void SetAlameInf(DETECT_BASE_DATA const* pD, int men, COIL_INF* pCoil, double const* pAttr);
	void SetData_AlarmCancel(int pos);												// ���׌��o�x�����	[��MI]

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ق�
public:
	bool	IsCoilNextKen()	const{ return 0!=strlen(m_CoilNext_Ken.cKizukenNo) ? true : false; }	// ���R�C������M�ς݁H [��MI�ADlg�AOpSendFunc]
	void	ClearCoilNextKen() { CoilClear(&m_CoilNext_Ken); }										// ���R�C�����o�b�t�@���N���A [��MI]
	void	ClearFirstCoilInf();																	// ���R�C�����f���A�����J�n1�{�ڂ��o�b�t�@����N���A[��MI]
	CString GetDefectDumpPath(int nMen, int nRec);													// �r���t�@�C���o�͐�t�@�C�����̂�ԋp[��CoilMgr]
	int		SetDefectCount(int men, const char* pKizukenNo, int row);								// �w��ŏ���Ԃ��r�����{�P����
	int		GetDefectCount(int men, const char* pKizukenNo, int row);								// �w��ŏ���Ԃ��r�����擾����

	void SetLogMgrA(LogFileManager* pLog) { mcls_pLogA = pLog; }					// �x��p���O�t�@�C���Z�b�g

private:
	LogFileManager*		mcls_pLogA;													// �x��p���O�N���X
};