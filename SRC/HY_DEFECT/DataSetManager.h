#pragma once

// �r�����ʃw�b�_�[
#include "..\..\Include\LineCommon.h"									// ���C���C���N���[�h
#include "..\..\Include\HyTbl.h"										// �\���p���L�������w�b�_�[

#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X

// ���[�J��
#include "MainConst.h"
#include "DataSetBaseManager.h"											// ���L�������f�[�^�Z�b�g�N���X
#include "FileDeleteManager.h"											// �t�@�C���폜�Ǘ��N���X
#include "FileSaveManager.h"											// �t�@�C���ۑ��Ǘ��N���X
#include "FolderFunc.h"


//#define	LOG_DEBUG													// �f�o�b�O���O

#define LST_SAVE_NO_MAX		100											// �d���׉摜�ۑ�MAX

class DataSetManager : public DataSetBaseManager
{
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//// ���J�ϐ�
public:
	//// �\����
	struct TYP_DATASET_REQMAE								// �O�H���r���ҏW�v���L���[
	{
		int nRec;											// �Ώۃ��R�[�h(0�`)
	};
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	struct TYP_DATASET_REQLONGPD							// ���������ה������ҏW�v���L���[
	{
		char	cKanriNo[SIZE_KIZUKEN_NO];					// �Ǘ�No
	};
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//// ���J�֐�
public:
	DataSetManager(void);
	virtual ~DataSetManager(void);

	void Allock();														// �X���b�h����


	//=========================================
	// �f�o�b�N�֌W (���z�֐�)
	//virtual void OnOutputTbl();										// �S���L������ �o�C�i���o��
	//virtual void OnInputTbl();										// �S���L������ �o�C�i���Ǎ�

	//=========================================
	// ��ʊ֌W
	TBL_HY_POINT const* GetPointTbl() { return mtbl_pPoint; };				
	TBL_HY_COILINF const* GetCoilInfTbl() { return mtbl_pCoilInf; };
	TBL_HY_COILRESULT const* GetCoilResultTbl() { return mtbl_pCoilResult; };
	TBL_HY_DEFECTINF  const* GetDefectInfTbl() { return mtbl_pDefectInf; };
	TBL_HY_MAPROW* GetMapRowTbl() { return mtbl_pMapRow; };
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	TBL_HY_LONGPD const* GetCoilLongPdTbl() { return mtbl_pLongPd; };
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	//=========================================
	// �O�H���֌W
	TBL_HY_MAE_INF*	GetMaeInf() { return mtbl_pMaeInf; }	// �O�H���r���|�C���^�擾
	char const*	GetMaeImgPath() { return m_cPath_Mae; }		// �O�H���摜�p�X�擾
	int GetnMaeImgSaveOff() { return m_nMaeImgSaveOff; }	// �O�H���摜�ۑ��L�����擾�i1:�ۑ����Ȃ��A�ȊO�F�ۑ�����j

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
															// �O�H�����Ǎ��v���L���[
	void SetQue_MaeRequest(ThreadQueue<TYP_DATASET_REQMAE>* pQue){mque_DataRequest = pQue;}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
															// ��������񔭐��v���L���[
	void SetQue_LongPdRequest(ThreadQueue<TYP_DATASET_REQLONGPD>* pQue){mque_DataRequestLongPd = pQue;}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//// �����o�[�֐�
protected:
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>	
	virtual void ThreadFirst_AddHandle();								// �V�O�i���҂��s��̒ǉ�
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	//=========================================
	// ���L���������� (���z�֐�)
	//virtual void OnOpenTbl();											// ���L�������I�[�v��
	//virtual void OnCloseTbl();										// ���L�������N���[�Y
	virtual void OnClearTbl(bool bMode);								// ���L������������
	virtual void OnSetTbl(TO_SO_HEAD const* pHead);						// ��M�f�[�^���L�������Z�b�g
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>	
//	//virtual void OnAddSignal(int nEventNo);							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	virtual void OnAddSignal(int nEventNo);							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	virtual void UpDate_PintWrite(TO_SO_HEAD const* pHead, int rec);	// �������݃|�C���^�X�V����
	//virtual void UpDate_PintDisp (TO_SO_HEAD const* pHead, int rec);	// �\���|�C���^�X�V����

	//=========================================
	// ���ۂ̏������݊֌W
	void SetData_Status(TO_SO_HEAD const* pH, TO_SO_DATA_STATUS const* pD);		// �X�e�[�^�X���
	void SetData_CoilInf(TO_SO_HEAD const* pH, TO_SO_DATA_COIL const* pD);		// �R�C�����i�����p�j�X�V
	void SetData_CoilResult(TO_SO_HEAD const* pH);								// �R�C������ (1�R�C������������)
	void SetData_CoilDsp(TO_SO_HEAD const* pH);									// �R�C�����i�\���p�j�X�V
	void SetData_Detect(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD);		// �r��� (�r���+�r�摜)
	//void SetData_CycleInf(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD);		// �r��� (�r���+�r�摜)
	void SetData_Length(TO_SO_HEAD const* pH, TO_SO_DATA_LENGTH const* pD);		// �\����_�ʒu�{�e�\���R�C���ʔʒu���
	void SetData_Cut(TO_SO_HEAD const* pH, TO_SO_DATA_CUT const* pD);			// �J�b�g���
	void SetData_Alarm(TO_SO_HEAD const* pH, TO_SO_DATA_ALARM const* pD);		// �x����
	void SetData_DefLst_Cancel(TO_SO_HEAD const* pH);							// �d�����r���폜�v��
	void SetData_Detect_Count(TO_SO_HEAD const* pH);							// �r����� (�ŏ���ԓ�)

	//=========================================
	// �e�[�u������
	void SetDefectInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex);
																				// �r���Z�b�g
	//void SetCycleInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex);
																				// �����r���Z�b�g
	void SetDefLst(TO_SO_DATA_DETECT const* pD, char const* pKizukenNo, const int nRec, const int nMen);
	void SetRecvStatus(const int nRec, const int nMen, const int row, const int st);
																				// �r�}�b�v�̏���M��ԂɃf�[�^�Z�b�g
	void SetRow_Inf(const int nRec, const int nMen, const int nLen);
																				// �s���ɃX�e�[�^�X���Z�b�g
	void GetOffsetOutLen(const int nRec, int * pOutLen);						// �o���I�t�Z�b�g�ʒu�擾
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
//	void RecalOutLen(const int nRec);											// �o�������̍Đݒ�

	void RecalOutLen(const int nRec, bool bDefect = false);						// �o�������̍Đݒ�
	void GetDeliveryPosY(const int nRec, float dPosY, float* pdDeliveryPosY);	// �o���ʒu���擾
	void GetDeliveryPosY(const int nRec, int   nPosY, int* pnDeliveryPosY)
	{
		float dPosY =  nPosY;
		float dDeliveryPosY = *pnDeliveryPosY;
		GetDeliveryPosY(nRec, dPosY, &dDeliveryPosY);
		*pnDeliveryPosY = (int)dDeliveryPosY;
	};

	void EditMaeDefectOutLen(TYP_DATASET_REQMAE* pQue);							// �O�H���r��� �o���ʒu�ҏW

// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	void EditLongPdInf(TYP_DATASET_REQLONGPD* pQue);							// ���������ҏW
	void SetLongPdGuidance();													// �������K�C�_���X�Z�b�g
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	void RefRowStatus(const int nRec, const int nStage, const int nMen, const int nBaseRow, const int nEndRow);
																				// �s�ʒu�̃X�e�[�^�X(�ғ����/�������/�@����)���w��s�܂Ŕ��f
	void DataShiftDefList(int nO_U, int nStartPos, int nEndPos);				// �d���׈ꗗ�\���r���f�[�^�V�t�g����
	void GetDefListFileNo(int nO_U, int nRec, int* nFileNo);					// �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No�擾


	//=========================================
	// �����֌W
	//void DeleteNotDispImg();													// �\���͈͊O�r�摜�폜����
	//int GetDeleteRec();															// �\���͈͊O���R�[�h�擾����




//// �����o�[�ϐ�
private:
	//=========================================
	// ���L���������
	
	// �e�[�u���n���h��
	HANDLE					m_hStatus;									// �X�e�[�^�X���e�[�u��
	HANDLE					m_hPoint;									// �|�C���^���e�[�u��
	HANDLE					m_hCoilInf;									// �R�C�����e�[�u��
	HANDLE					m_hCoilResult;								// �R�C�����уe�[�u��
	HANDLE					m_hMapRow;									// �}�b�v�ʒu�e�[�u��
	HANDLE					m_hDefectInf;								// �r���e�[�u��
	HANDLE					m_hAlarm;									// �x��o�͒��e�[�u��
	HANDLE					m_hMaeInf;									// �O�H���r���e�[�u��
	HANDLE					m_hDefmap;									// �r�}�b�v���e�[�u��
	//HANDLE					m_hCycleInf;								// �����r���e�[�u��
	HANDLE					m_hDefList;									// �d���׈ꗗ�\���|�C���^���e�[�u��
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	HANDLE					ml_hLongPd;									// ���������e�[�u��
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------	

	// ���L�������|�C���^
	TBL_HY_STATUS*			mtbl_pStatus;								// �X�e�[�^�X���e�[�u��
	TBL_HY_POINT*			mtbl_pPoint;								// �|�C���^���e�[�u��
	TBL_HY_COILINF*			mtbl_pCoilInf;								// �R�C�����e�[�u��
	TBL_HY_COILRESULT*		mtbl_pCoilResult;							// �R�C�����уe�[�u��
	TBL_HY_MAPROW*			mtbl_pMapRow;								// �}�b�v�ʒu�e�[�u��
	TBL_HY_DEFECTINF*		mtbl_pDefectInf;							// �r���e�[�u��
	TBL_HY_ALARM*			mtbl_pAlarm;								// �x��o�͒��e�[�u��
	TBL_HY_MAE_INF*			mtbl_pMaeInf;								// �O�H���r���e�[�u��
	//TBL_HY_CYCLE_INF*		mtbl_pCycleInf;								// �����r���e�[�u��
	TBL_HY_DEFLIST*			mtbl_pDefList;								// �d���׈ꗗ�\���|�C���^���e�[�u��
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	TBL_HY_LONGPD*			mtbl_pLongPd;								// ���������e�[�u��
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------	

	//=========================================
	// ���̑��������
	char					m_cPath_Img[SIZE_IMAGE_PATH];				// �摜�ۑ��x�[�X�p�X
	char					m_cPath_Mae[SIZE_IMAGE_PATH];				// �摜�ۑ��x�[�X�p�X[�O�H��]
	char					m_cPath_Lst[SIZE_IMAGE_PATH];				// �摜�ۑ��x�[�X�p�X[�d���׈ꗗ]
	bool					m_bQueState;								// �L���[�󂫏��
	int						m_nKensaNum[NUM_MEN];						// ������ԍ�

	int						m_nImgSaveOff;								// �T����ʕ\���p�̉摜�ۑ��L�� 1:�ۑ����Ȃ��A�ȊO�F�ۑ�����
	int						m_nMaeImgSaveOff;							// �O�H���r�摜�ۑ��L��         1:�ۑ����Ȃ��A�ȊO�F�ۑ�����
	int						m_nLstImgSaveOff;							// �d�����r�摜�ۑ��L��         1:�ۑ����Ȃ��A�ȊO�F�ۑ�����

	int						m_nLstSaveNo[NUM_MEN];						// �d�����r�摜�ۑ��ԍ��i�ŏI�ۑ��ԍ� ���{�P���Ď��͎g���j

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	// �X���b�h�L���[
	ThreadQueue<TYP_DATASET_REQMAE>*	mque_DataRequest;				// �f�[�^�Z�b�g�ւ̑O�H���r�ҏW�˗��L���[
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	ThreadQueue<TYP_DATASET_REQLONGPD>*	mque_DataRequestLongPd;			// �f�[�^�Z�b�g�ւ̒��������ҏW�˗��L���[
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	// ��ʃ��O�o�͖h�~�΍�
	int						m_nErrCnt_Skip_Defect;						// �X�L�b�v �s����
	int						m_nErrCnt_Err_Key;							// ���R�[�h���� �s����
	int						m_nErrCnt_Err_Coil;							// �_���R�C������ �s����
	int						m_nErrCnt_NG_Val;							// �������ُ� �s����
	int						m_nErrCnt_Max_Cnt;							// ������B �s����
};
