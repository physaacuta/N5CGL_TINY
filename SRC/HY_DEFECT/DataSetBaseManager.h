#pragma once

// �W��
#include <direct.h>

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// �r�����ʃw�b�_�[
#include "..\..\include\LineCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\include\HyTbl.h"										// ����PC���L�������w�b�_�[

#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FileDeleteManager.h"											// �t�@�C���폜�Ǘ��N���X
#include "FileSaveManager.h"											// �t�@�C���ۑ��Ǘ��N���X

class DataSetBaseManager :	public ThreadManager
{
//// ���J�萔
public:
	static const int		MAX_MEM_COUNT = 64;							// ���L������ �ő�Ǘ���

	// ���J�\����
	struct MEM_INF
	{
		HANDLE*				h;											// �n���h��
		void**				pTbl;										// �e�[�u��
		int					nSize;										// �T�C�Y
		char				cName[64];									// ����
		bool				bNomalCrearCancel;							// ���������[�h (True:�ʏ평�����͎��{���Ȃ�)
	};

//// ���J�֐�
public:
	DataSetBaseManager(void);
	virtual ~DataSetBaseManager(void);

	//=========================================
	// �Z�b�g
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }	// �p�����[�^�Ǘ�
	void SetFDeleMgr(FileDeleteManager* pCls)	{ mcls_pFDele = pCls; }	// �t�@�C���폜�Ǘ�
	void SetQueImgSave(ThreadQueue<FileSaveManager::TYP_SAVE_IMG>* pQue) { mque_pImgSave = pQue; }			// �󂯓n���f�[�^�p�L���[
	void SetQueImgSaveLow(ThreadQueue<FileSaveManager::TYP_SAVE_IMG>* pQue) { mque_pImgSaveLow = pQue; }	// �󂯓n���f�[�^�p�L���[

	void SetQueOpRecv(ThreadQueue<SOCK_BASE_HEAD>* pQue) { mque_pOpRecv = pQue; }	// �������\�����M�ʒm�̃L���[


	// �L���[�擾
	ThreadQueue<SOCK_BASE_HEAD>* GetQueOpRecv() { return mque_pOpRecv; }// �������\�����M�ʒm�̃L���[�̃|�C���^�擾
	int GetToHyQueCount() { return mque_pOpRecv->GetCount(); }			// �������\�����M�ʒm�̃L���[�̌������擾

	//=========================================
	// ��{�֌W
	int   GetStageIndex(int nStage, int nRow );							// ����R�[�h�̍s����A�e�X�e�[�W�̍s���Z�o
	int	  GetStageLenToIndex(int nStage, int nLen );					// �w�苗������A�e�X�e�[�W�̍s�ʒu���Z�o


	//=========================================
	// ���̑��������
	static void		CreateFolder(CString sPath, int nRec);				// �i�[�t�H���_�쐬
	static int		GetRecIndex(int rec, int index);					// ���R�[�h�ʒu�擾
	static CString	GetFolderPath(CString sBasePath, int rec);			// �t�H���_�p�X����
	
	int	CheckKanriNo(char const* pKizukenNo, TBL_HY_POINT const* pTblPoint);		// �|�C���^�[�e�[�u���ɕێ����Ă����r���Ǘ�No�ƈ�v���邩�`�F�b�N


	//=========================================
	// �O���A�N�Z�X
	void SetEvClearTbl()	{ SetEvent(m_evClearTbl); }



	//=========================================
	// �f�o�b�N�֌W (���z�֐�)
	virtual void OnOutputTbl();											// �S���L������ �o�C�i���o��
	virtual void OnInputTbl();											// �S���L������ �o�C�i���Ǎ�



//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//virtual void ThreadFirst_AddHandle() {};							// �V�O�i���҂��s��̒ǉ�
	virtual void ThreadFirst_AddHandle(){ThreadFirst_AddHandle();};		// �V�O�i���҂��s��̒ǉ�
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	//=========================================
	// ���L���������� (���z�֐�)
	bool		 AddMemInf(HANDLE* h, void** pTbl, int nSize, char const* cName, bool bNomalCrearCancel=false);	// ���L����������o�^
	virtual void OnOpenTbl();											// ���L�������I�[�v��
	virtual void OnCloseTbl();											// ���L�������N���[�Y
	virtual void OnClearTbl(bool bMode);								// ���L������������
	virtual void OnSetTbl(TO_SO_HEAD const* pHead) {};					// ��M�f�[�^���L�������Z�b�g
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//virtual void OnAddSignal(int nEventNo) {};							// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	virtual void OnAddSignal(int nEventNo){ OnAddSignal(nEventNo); };		// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	//=========================================
	// ���ۂ̏������݊֌W
	void SetData_PointWrite(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint);		// �������݃|�C���^�X�V
	void SetData_PointDisp (TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint);		// �\���|�C���^�X�V

	virtual void UpDate_PintWrite(TO_SO_HEAD const* pHead, int rec) {};				// �������݃|�C���^�X�V����
	virtual void UpDate_PintDisp (TO_SO_HEAD const* pHead, int rec) {};				// �\���|�C���^�X�V����
	


	//=========================================
	// ���̑��������
	void QueBufFree(TO_SO_HEAD* pHead);									// ���o�b�t�@���J������


//// �����o�[�ϐ�
protected:

	//=========================================
	//// �e�C���X�^���X
	ParamManager*					mcls_pParam;						// �p�����[�^�Ǘ�
	FileDeleteManager*				mcls_pFDele;						// �t�@�C���폜�Ǘ��N���X

	// �󂯓n���L���[
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>*	mque_pImgSave;			// �󂯓n���f�[�^�p�L���[ (��\�r)
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>*	mque_pImgSaveLow;		// �󂯓n���f�[�^�p�L���[ (���X�g���ŉB����r)


	//=========================================
	//// ���L���������
	int								m_nMemSetCnt;						// �Z�b�g��
	MEM_INF							m_MemInf[MAX_MEM_COUNT];			// �g���₷���悤�Ƀ��[�v�p


//// �����o�[�ϐ�
private:

	// �󂯓n���L���[
	ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpRecv;						// �������\�����M�ʒm�̃L���[

	//=========================================
	// �V�O�i��
	HANDLE							m_evClearTbl;						// ���L�������������C�x���g
};
