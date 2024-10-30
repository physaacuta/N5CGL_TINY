//============================================================
//	DB�Ǘ��N���X
//------------------------------------------------------------
// [����]		2015/07/15		���ō쐬�J�n
//		
//============================================================
#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"												// �ӃփC���N���[�h

// �ӃփN���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"							// ���[�J�[�X���b�h�}�l�[�W���[

// ���[�J��
#include "MainConst.h"
#include "DbBaseManager.h"
#include "BcpSaveManager.h"
#include "ParamManager.h"


class DBManager :	public DbBaseManager
{
public:
	DBManager(void);
	virtual ~DBManager(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��n���\���̂̔ԍ�
public:
	// (ENUM_DB_KUBUN::em_END ����̘A�ԂƂ���
	enum ENUM_KUBUN {
		em_COILINF = em_DB_KUBUN_END,												// �R�C�����
		em_COILRESULT,																// �R�C������
		em_COILRESULT_UPDATE,														// �R�C�����уA�b�v�f�[�g
		em_COILRESULT_UPDATE_CUT,													// �R�C�����уA�b�v�f�[�g �i���ъm����UpDate�p�j(�V���[�J�b�g)

		em_COILPOSITION,															// �R�C���ʒu
		em_COILTRAND,																// �R�C���g�����h
		em_COILCUTPOS,																// �R�C���J�b�g�ʒu

		em_DEFECTINF,																// �r���

		em_SEQ_COIL_BUF,															// �R�C�����`���o�b�t�@�ۑ�

		em_EDGE_COIL_INF,															// �G�b�W���o���уe�[�u��
		em_EDGE_CHANGE_INF,															// �G�b�W���o���[�h�ω��Ǘ��e�[�u��

#ifndef DISABLE_TINY_CALC
		em_TINY_RESULT,																// �������׎���
		em_TINY_INF,																// �������׏��
#endif

		em_FRAME_INF,																// �t���[�����e�[�u��
		em_FRAME_IMG,																// �t���[���摜�{�t���[�����A�b�v�f�[�g��

		em_CAMLINKSW,																// �J���������N�X�C�b�`�ݒ�(��MainInstance)

		em_PULSE_RATE_UPDATE,														// �V�[�P���T���p���X���[�g�ύX

		em_DUMMY_INF,																// �_�~�[�r(1CAPL�̂݁B�X�g�b�v�}�[�N�r)	

		em_DB_ADD_SYNC,																// �o�^�󋵖₢���킹

		em_DB_COUNT_RESET															// �r�摜�o�^�����Z�b�g
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP�p�\����
private:
	//// �r�摜�ۑ��p�e�[�u��
	struct TYP_BCP_DEFECT_IMG
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// �Ǘ�No
		int						nKizuNo;											// �rNo	
		BCPBase::typBindDataEx	img[MAX_HT1_CAMANGLE];								// �摜�f�[�^ ��/��
	};


	//// ������ (��{�A�p�x ����\����)
	struct TYP_BCP_DEFECT_ATTR
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// �Ǘ�No
		int						nKizuNo;											// �rNo
		double					attr[MAX_ATTR_BASE];								// ������
	};


	//// �R�C���ʒu���
	struct TYP_BCP_COIL_POSITION
	{
		DBDATA_COIL_POSITION	CoilPos;											// �R�C���ʒu���
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���J�֐�
public:
	BCPSaveManager&		GetBcpSaveMgr(void) {return mcls_Bcps;};				// BCP�����݃N���X�擾


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐� (�p��������R�[���o�b�N)
private:
	virtual void OnAlloc(bool bMode=true);											// �֐�������
	virtual void OnFree();															// �֐��J��
	virtual void OnQueDb(int emNo, BYTE* pData);									// DB�o�^�˗�
	virtual void OnBcpAutoCommit(bool bAuto) {};									// BCP�����R�~�b�g�p
	virtual void OnBcpStatusCheck() {};												// BCP��ԃ`�F�b�N(������^�C�}�[�C�x���g)
	virtual void ThreadFirst_AddHandle() {};										// �V�O�i���҂��s��̒ǉ�
	virtual void OnAddSignal(int nEventNo) {};										// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	virtual void OnReSetMaxCount();													// ���Z�b�g�����ǉ�
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL�����֘A
private:
	void CreateSQL_CoilInf(const DBDATA_COIL_INF* pWk);								// �R�C�����
	void CreateSQL_CoilResult(const DBDATA_COIL_RESULT* pWk);						// �R�C������
	void CreateSQL_CoilResultUpdate(const DBDATA_COIL_RESULT_UPDATE* pWk);			// �R�C�����уA�b�v�f�[�g �i���ъm����UpDate�p�j
	void CreateSQL_CameraResult(const DBDATA_COIL_RESULT* pWk);						// �J��������

	void CreateSQL_CoilPosition(const DBDATA_COIL_POSITION* pWk);					// �R�C���ʒu
	void CreateSQL_CoilTrand(const DBDATA_COIL_TRAND* pWk);							// �R�C���g�����h
	void CreateSQL_CoilCutPos(const DBDATA_COIL_CUT_POS* pWk);						// �R�C���J�b�g�ʒu

	BOOL CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk);							// �r���


	void CreateSQL_EdgeCoilInf(const DBDATA_EDGE_COIL_INF* pWk);					// �G�b�W���o����
	void CreateSQL_EdgeChangeInf(const DBDATA_EDGE_CHANGE_INF* pWk);				// �G�b�W���o���[�h�ω��Ǘ�

	void CreateSQL_CamLinkSW(const DBDATA_CAMERA_LINKSWITCH* pWk);					// �J���������N�X�C�b�`�ݒ�

	void CreateSQL_PulseRateUpdate(const DBDATA_PULSE_RATE_UPDATE* pWk);			// �V�[�P���T����p���X���[�g�ύX

	void Add_Synclock(const DBDATA_ADD_SYNC* pWk);									// �o�^�󋵖₢���킹

	void Reset_ImgCount(const DBDATA_COUNT_RESET* pWk);								// �r�摜�o�^�����Z�b�g

	void CreateSQL_StatusLog(const DBDATA_STATUS_LOG* pWk);							// �X�e�[�^�X���O




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private:

	//// �e�C���X�^���X
	BCPSaveManager			mcls_Bcps;												// BCP�������݃N���X

};
