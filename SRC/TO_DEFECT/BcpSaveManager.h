//============================================================
//	BCP�������݃N���X
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


class BCPSaveManager : public DbBaseManager
{
/// ���J�萔
public:
	BCPSaveManager(void);
	~BCPSaveManager(void);
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��n���\���̂̔ԍ�
public:
	// 
	// (ENUM_DB_KUBUN::em_END ����̘A�ԂƂ���
	enum ENUM_KUBUN {
		em_DEFECTINF,																// �r���
		em_BCP_BATCH,																// BCP batch�������s
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
// ���[�J���萔
private:
	static const int		MAX_BCP_BATCH_COUNT_DEF		= 50;						// ���̐��P�ʂŁA�r�摜��BCP�����݂��s��
	static const int		MAX_BCP_BATCH_COUNT_ATTR	= 100;						// ���̐��P�ʂŁA�����ʂ�BCP�����݂��s��
	static const int		MAX_BCP_INS					= NUM_MEN;					// �eBCP�̃C���X�^���X���B
	static const int		MAX_BCP_INS_ATTR			= 1+MAX_IMAGE_CNT;			// ��{�{4�p�x
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//���J�֐�
public:
	double GetAttrSendRowAveTime() const { return mcls_AttrSendRowTime.GetAveTime(); }		// AttrSendRow1��̕��Ϗ������� [ms]
	double GetAttrSendRowMaxTime()	const { return mcls_AttrSendRowTime.GetMaxTime(); }		// AttrSendRow�ő又������ [ms]	
	double GetAttrBcpBatchAveTime() const { return mcls_AttrBcpBatchTime.GetAveTime(); }	// AttrBcpBatch1��̕��Ϗ������� [ms]
	double GetAttrBcpBatchMaxTime()	const { return mcls_AttrBcpBatchTime.GetMaxTime(); }	// AttrBcpBatch�ő又������ [ms]	
	double GetImgSendRowAveTime() const { return mcls_ImgSendRowTime.GetAveTime(); }		// ImgSendRow1��̕��Ϗ������� [ms]
	double GetImgSendRowMaxTime()	const { return mcls_ImgSendRowTime.GetMaxTime(); }		// ImgSendRow�ő又������ [ms]	
	double GetImgBcpBatchAveTime() const { return mcls_ImgBcpBatchTime.GetAveTime(); }		// ImgBcpBatch1��̕��Ϗ������� [ms]
	double GetImgBcpBatchMaxTime()	const { return mcls_ImgBcpBatchTime.GetMaxTime(); }		// ImgBcpBatch�ő又������ [ms]	

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐� (�p��������R�[���o�b�N)
private:
	virtual void OnAlloc(bool bMode=true);											// �֐�������
	virtual void OnFree();															// �֐��J��
	virtual void OnQueDb(int emNo, BYTE* pData);									// DB�o�^�˗�
	virtual void OnBcpAutoCommit(bool bAuto);										// BCP�����R�~�b�g�p
	virtual void OnBcpStatusCheck();												// BCP��ԃ`�F�b�N(������^�C�}�[�C�x���g)
	virtual void ThreadFirst_AddHandle() {};										// �V�O�i���҂��s��̒ǉ�
	virtual void OnAddSignal(int nEventNo) {};										// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	virtual void OnReSetMaxCount();													// ���Z�b�g�����ǉ�
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP�֘A
private:
	void BCP_Bind_Defect_Img();														// BCP�N���X �e�[�u���R�t�� (�r�摜)
	void BCP_Bind_Defect_Attr();													// BCP�N���X �e�[�u���R�t�� (������)
	//////void BCP_Bind_Coil_Position();													// BCP�N���X �e�[�u���R�t�� (�R�C���ʒu���)
	void CheckBcpStatus(bool bMode);												// BCP��Ԃ�ύX

	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL�����֘A
private:
	void CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk);							// �r���
	void BcpBatchExec(const DBDATA_BCP_BATCH_EXEC* pWk);							// BCP batch ������������


/// �����o�[�ϐ�
private:

	//===================================
	// BCP�̈�
	//// �r�摜
	BCPBase*				mcls_pBcpDefImg[MAX_BCP_INS];							// DB���������݃N���X
	TYP_BCP_DEFECT_IMG*		m_pBcpDefImgBuf[MAX_BCP_INS];							// BCP�����ݗp �Œ胁�����o�b�t�@
	int						m_nBcpDefSendRowCnt[MAX_BCP_INS];						// �\���ʂ�SendRow����(Batch������0�N���A)

	// ������
	BCPBase*				mcls_pBcpAttr[MAX_BCP_INS][MAX_BCP_INS_ATTR];			// DB���������݃N���X
	TYP_BCP_DEFECT_ATTR*	m_pBcpAttrBuf[MAX_BCP_INS][MAX_BCP_INS_ATTR];			// BCP�����ݗp �Œ胁�����o�b�t�@
	int						m_nBcpAttrSendRowCnt[MAX_BCP_INS];						// �\���ʂ�SendRow����(Batch������0�N���A)

	//////// �R�C���ʒu���
	//////BCPBase*					mcls_pBcpCoilPos[EM_DIV_SYSTEM::DIV_SYSTEM_END];	// DB���������݃N���X[0:�����Ӄ�, 1:�o���Ӄ�, 2:����]
	//////TYP_BCP_COIL_POSITION*		m_pBcpCoilPosBuf[EM_DIV_SYSTEM::DIV_SYSTEM_END];	// BCP�����ݗp �Œ胁�����o�b�t�@
	//////int							m_nBcpCoilPosSendRowCnt;							// SendRow����(Batch������0�N���A)	

	// ���Ԍv���p�N���X
	KizuPerformanceCounter	mcls_AttrSendRowTime;									// AttrSendRow���ԑ���X���b�h
	KizuPerformanceCounter	mcls_AttrBcpBatchTime;									// AttrBcpBatch���ԑ���X���b�h
	KizuPerformanceCounter	mcls_ImgSendRowTime;									// ImgSendRow���ԑ���X���b�h
	KizuPerformanceCounter	mcls_ImgBcpBatchTime;									// ImgBcpBatch���ԑ���X���b�h

	// Jpeg�摜�����N���X
	KizuLjtJpegManager		mcls_KizuJpegManager;
};

