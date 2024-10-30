#include "StdAfx.h"
#include "DataSetBaseManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
DataSetBaseManager::DataSetBaseManager(void) :
mcls_pParam(NULL),
mcls_pFDele(NULL),
mque_pImgSave(NULL),
mque_pImgSaveLow(NULL),
ThreadManager("Mgr")
{
	//// �V�O�i��
	m_evClearTbl  = CreateEvent(NULL, FALSE, FALSE, NULL);			// ���L�������������C�x���g

	// ������
	m_nMemSetCnt = 0;
	memset(m_MemInf, 0x00, sizeof(m_MemInf));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DataSetBaseManager::~DataSetBaseManager(void)
{
	//// �V�O�i���J��
	CloseHandle(m_evClearTbl);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DataSetBaseManager::ThreadFirst()
{
	//// ���O����

	// ���L�������I�[�v��
	OnOpenTbl();
	// ���L������ ���S������
	OnClearTbl(true);

	//// �C�x���g�ݒ�
	AddEventNum(1, &m_evClearTbl);
	AddEventNum(1, &mque_pOpRecv->g_evSem);

	//// �C�x���g�ǉ�
	ThreadFirst_AddHandle();
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DataSetBaseManager::ThreadLast()
{
	//// ���L�������N���[�Y
	OnCloseTbl();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DataSetBaseManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_CLEAR_TBL = 0,				// ���L�������������C�x���g
			EV_QUR,							// ���L����������M

			EV_LAST
	};


	////// �V�O�i����������
	//======================================================
	// ���L�������������C�x���g
	if(nEventNo == EV_CLEAR_TBL) {
		OnClearTbl(false); 


	//======================================================
	// ���L����������M
	} else if(nEventNo == EV_QUR)	{

		TO_SO_HEAD* pHead;											// ��M�f�[�^�w�b�_���
		pHead = (TO_SO_HEAD*) mque_pOpRecv->GetFromHead(); 

		OnSetTbl( (TO_SO_HEAD const*)pHead);

		QueBufFree(pHead);


	//======================================================
	// ����ȊO�B�p����Œǉ������V�O�i��
	} else {
		//// �p����ɒʒm
		OnAddSignal( nEventNo - EV_LAST );
	}
}


//------------------------------------------
// ���o�b�t�@���J������
// TO_SO_HEAD* pHead  �`���w�b�_
//------------------------------------------
void DataSetBaseManager::QueBufFree(TO_SO_HEAD* pHead)
{
	if( NULL == pHead) return;
	
	// �J��
	SAFE_DELETE(pHead->base_head.addr[0]);
	SAFE_DELETE(pHead->base_head.addr[1]);
	SAFE_DELETE(pHead);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑��������


//------------------------------------------
// �������݃|�C���^�X�V
// TO_SO_HEAD const* pHead ��M�w�b�_
// TBL_HY_POINT* pTblPoint �|�C���^���e�[�u��
//------------------------------------------
void DataSetBaseManager::SetData_PointWrite(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint)
{
	int						rec;					// 0�I���W��
	bool					bIsNewRec = false;		// �V�K���R�[�h�� true

	LOG(em_MSG),"[�����|�C���^�X�V����]");

	//// ���R�[�h�ʒu����
	rec = CheckKanriNo(pHead->KizukenNo, pTblPoint);
	if(0 > rec) {
		// �ŐV �������݃��R�[�h
		bIsNewRec = true;
		rec = pTblPoint->nRecWriteBase;
		if(TBL_HY_REC_MAX <= rec) rec = 0;
	}

	//// �������݃|�C���^�X�V���̏���
	if( ! bIsNewRec) {
		LOG(em_WAR), "[�|�C���^��� <%d>] �Ǘ�No�d�� �Ǘ�No=%s", pTblPoint->nRecWriteBase, pHead->KizukenNo);
		return;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// ������
	//
	// �p����ɃR�[���o�b�N
	UpDate_PintWrite(pHead, rec);


	// ///////////////////////////////////////////////////////////////////
	// �f�[�^�Z�b�g
	//
	// �|�C���^�e�[�u�� �Z�b�g
	memset( &pTblPoint->pnt[rec], 0x00, sizeof(pTblPoint->pnt[rec]));
	// �Ǘ�No
	memcpy( pTblPoint->pnt[rec].cKizukenNo, pHead->KizukenNo, SIZE_KIZUKEN_NO);

	// �����݃��R�[�h
	pTblPoint->nRecWriteBase = rec + 1;

	// ��ʕ\���ۃt���O���u�\���\��v�Ƃ���
	pTblPoint->pnt[rec].emDispStatus = DIV_DSP_STANDBY;

	LOG(em_INF), "==========================================================>>>>");
	LOG(em_MSG), "[�|�C���^��� <%d>] �����|�C���^�X�V �Ǘ�No=%s", pTblPoint->nRecWriteBase, pHead->KizukenNo);
}

//------------------------------------------
// �\���|�C���^�X�V
// TO_SO_HEAD const* pHead ��M�w�b�_
// TBL_HY_POINT* pTblPoint �|�C���^���e�[�u��
// �߂�l �������݃��R�[�h�ʒu (0�I���W��)
//------------------------------------------
void DataSetBaseManager::SetData_PointDisp(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint)
{
	int						rec;					// 0�I���W��

	//// ���R�[�h�ʒu����
	rec = CheckKanriNo(pHead->KizukenNo, pTblPoint);
	if(0 > rec) {
		LOG(em_ERR), "[�|�C���^���] �\���|�C���^�Y������ �Ǘ�No=%s", pHead->KizukenNo);
		return;
	}

	//// �\����_�ʒu�ł̌��ݕ\���|�C���^���X�V(1�I���W��)
	pTblPoint->nRecDispBase = rec + 1;
	LOG(em_MSG), "[�|�C���^��� <%d>] �\���|�C���^�X�V �Ǘ�No=%s", pTblPoint->nRecDispBase, pHead->KizukenNo);


	// ///////////////////////////////////////////////////////////////////
	// ���̑�����
	//

	// �p����ɃR�[���o�b�N
	UpDate_PintDisp(pHead, rec);
		
	// TBL_HY_POINT�̉�ʕ\���ۃt���O��\���Ƃ���
	pTblPoint->pnt[rec].emDispStatus = DIV_DSP_OK;

	// �ߋ���TBL_HY_POINT�̉�ʕ\���ۃt���O�𑖍����A�\���\��̃R�C���͕\�����Ȃ��ɍX�V����
	// �ߋ��̃��R�[�h����Q�Ƃ��邽�߁A�������݃��R�[�h�l���擾
	int nRecIndex = rec;
	for (int ii = 0; ii < TBL_HY_REC_MAX - 1; ii++) {
		// �ߋ��̃��R�[�h�𑖍����邽��-1����
		nRecIndex -= 1;
		if (0 > nRecIndex) {
			nRecIndex = TBL_HY_REC_MAX - 1;
		}
		// �\���������̓f�[�^�Ȃ��܂ŒB�����ꍇ�A�����I��
		if ( (DIV_DSP_OK == pTblPoint->pnt[nRecIndex].emDispStatus) 
			|| (DIV_DSP_INITIALIZE == pTblPoint->pnt[nRecIndex].emDispStatus)) {
			break;
		}
		// �\���p�ӂ̏ꍇ�A�\���s�ɍX�V
		if (DIV_DSP_STANDBY == pTblPoint->pnt[nRecIndex].emDispStatus) {
			LOG(em_ERR), "[�|�C���^���] �\���s�R�C�����o�I �Ǘ�No=%s, ���R�[�h=%d", pHead->KizukenNo, nRecIndex);
			pTblPoint->pnt[nRecIndex].emDispStatus = DIV_DSP_NG;
		}
	}

}

//------------------------------------------
// �|�C���^�[�e�[�u���ɕێ����Ă����r���Ǘ�No�ƈ�v���邩�`�F�b�N
// char const* pKizukenNo �r���Ǘ�No
// TBL_HY_POINT const* pTblPoint �|�C���^���e�[�u��
// �߂�l -1:�Y���r���Ǘ�No����  0�`:��v���R�[�h�ʒu(0�I���W��)
//------------------------------------------
int DataSetBaseManager::CheckKanriNo(char const* pKizukenNo, TBL_HY_POINT const* pTblPoint)
{
	// �������݃|�C���^�`�F�b�N
	if(0 >= pTblPoint->nRecWriteBase) return -1;

	//// �|�C���^�[�e�[�u�����`�F�b�N
	// ���̈ʒu����O�ɑk���čs��
	int rec = pTblPoint->nRecWriteBase -1;
	for(int ii=0; ii<TBL_HY_REC_MAX; ii++) {
		if(0 == memcmp(pTblPoint->pnt[rec].cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return rec;

		rec -= 1;
		if( 0 > rec ) rec = TBL_HY_REC_MAX-1;
	}

	// �����܂ŗ�����Y������
	return -1;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���܂��܂�������


//------------------------------------------
// ���R�[�h�ʒu�擾(�e�[�u��)
// int rec ����R�[�h (0�I���W��)
// int index 0:���݈ʒu 1:��O 2:��O 3:�O�O -1:��
// �߂�l:���R�[�h�ʒu (0�I���W��)
//------------------------------------------
int DataSetBaseManager::GetRecIndex(int rec, int index)
{
	if(0 == index) {
		// ���݈ʒu
		return rec;			
	}
	// �w��O
	int recval = rec - index;
	if( 0 > recval) recval = TBL_HY_REC_MAX + recval;
	if( TBL_HY_REC_MAX <= recval) recval = recval - TBL_HY_REC_MAX;
	return recval;
}

//------------------------------------------
// �t�H���_�p�X����
// CString sBasePath �����x�[�X�p�X (�Ō��\�͂���Ȃ�)
// int rec ���R�[�h (0�I���W��) -1:���R�[�h�͖���
//------------------------------------------
CString DataSetBaseManager::GetFolderPath(CString sBasePath, int rec)
{
	CString sWk;
	if(-1==rec) {
		sWk.Format("%s", sBasePath);
	} else {
		sWk.Format("%s\\rec%d", sBasePath, rec);
	}
	return sWk;
}

//------------------------------------------
// �摜�i�[�t�H���_�쐬
// CString sPath �����x�[�X�p�X (�Ō��\�͂���Ȃ�)
// int nRec �T�u�t�H���_�� (0:���R�[�h���Ȃ�)
//------------------------------------------
void DataSetBaseManager::CreateFolder(CString sPath, int nRec)
{
	// �x�[�X�t�H���_�쐬
	_mkdir(sPath);

	// �T�u�t�H���_
	CString sWk;
	for(int ii=0; ii<nRec; ii++) {
		//sWk.Format("%s\\rec%d", sPath, ii);
		sWk = GetFolderPath(sPath, ii);
		_mkdir(sWk);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{ (���S���p�ɓ���Ă��ǂ����炢�̓z)

//------------------------------------------
// ����R�[�h�̍s����A�e�X�e�[�W�̍s���Z�o
// int stage	�X�e�[�W (0�I���W��)
// int nRow		����R�[�h (0�I���W��)
// �߂�l		�Ώۍs�ʒu (0�I���W��)
//------------------------------------------
int DataSetBaseManager::GetStageIndex(int nStage, int nRowIndex)				
{
	int nIntVal   = mcls_pParam->GetCommonMast().nMapDsp[nStage];		// �}�b�v�Ԋu [m]
	int nMaxIndex = GET_JUST_INDEX( COIL_LEN_MAX, nIntVal );			// �ő僌�R�[�h�ʒu

	int index = nRowIndex / ( nIntVal / MAP_REPRES_MIN );	//(0�I���W��) 5,5�̂Ƃ���0
	
	// �������`�F�b�N
		// ���݂��炨�������B ���Ԃ�A�ő�R�C������ 6m �� �}�b�v�Ԋu��(2,4,6)�ƂȂ��Ă���ƁA�ő�l�����������Ȃ��Ă���͂�
	if(0 > index)			index = 0;
	if(nMaxIndex < index)	index = nMaxIndex;
	return index;
}
//------------------------------------------
// �w�苗������A�e�X�e�[�W�̍s�ʒu���Z�o
// int nStage	�X�e�[�W (0�I���W��)
// int nLen		�w�苗�� [m]
// �߂�l		�Ώۍs�ʒu (0�I���W��)
//------------------------------------------
int DataSetBaseManager::GetStageLenToIndex(int nStage, int nLen)				
{
	int nIntVal   = mcls_pParam->GetCommonMast().nMapDsp[nStage];		// �}�b�v�Ԋu [m]
	int nMaxIndex = GET_JUST_INDEX( COIL_LEN_MAX, nIntVal );			// �ő僌�R�[�h�ʒu

	int index = nLen / nIntVal;	//(0�I���W��) 5,5�̂Ƃ���0

	// �������`�F�b�N
		// ���݂��炨�������B ���Ԃ�A�ő�R�C������ 6m �� �}�b�v�Ԋu��(2,4,6)�ƂȂ��Ă���ƁA�ő�l�����������Ȃ��Ă���͂�
	if(0 > index)			index = 0;
	if(nMaxIndex < index)	index = nMaxIndex;
	return index;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���L����������
//------------------------------------------
// �g���₷���悤�ɋ��L�������o�^
// HANDLE* h			�n���h��
// void** pTbl			�e�[�u��
// int nSize			�e�[�u���T�C�Y
// char const* cName	����
// bool bNomalCrearCancel ���������[�h (True:�ʏ평�����͎��{���Ȃ�)
//------------------------------------------
bool DataSetBaseManager::AddMemInf(HANDLE* h, void** pTbl, int nSize, char const* cName, bool bNomalCrearCancel)
{
	if( MAX_MEM_COUNT <= m_nMemSetCnt ) return false;

	// �o�^
	m_MemInf[m_nMemSetCnt].h	= h;
	m_MemInf[m_nMemSetCnt].pTbl = pTbl;		
	m_MemInf[m_nMemSetCnt].nSize= nSize;
	m_MemInf[m_nMemSetCnt].bNomalCrearCancel = bNomalCrearCancel;
	strcpy(m_MemInf[m_nMemSetCnt].cName, cName);

	*m_MemInf[m_nMemSetCnt].pTbl = 0;		//cdcdcd�͂���Ȃ̂ŁA���������Ă���
	m_nMemSetCnt++;
	return true;
}

//------------------------------------------
// ���L�������I�[�v��
//------------------------------------------
void DataSetBaseManager::OnOpenTbl()
{

	// �ǂ���64bitOS ��ŁA32bitAP �𓮂����āA���L�������m�ۂ���ƁA���Ƀ������������ς��g�p���Ă���ꍇ�ɁA����ȋ��L���������m�ۂ��悤�Ƃ����玸�s����
	// GetLastError(); �� 8 : ERROR_NOT_ENOUGH_MEMORY
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		mem_connect(pInf->h, (LPVOID *)pInf->pTbl, pInf->cName);

		if(NULL == pInf->h || NULL == *pInf->pTbl) {
			int nErr = GetLastError();

			syslog(SYSNO_MEM_OPEN, "[%s:Err=%d]", pInf->cName, nErr);
			LOG(em_ERR), "[%s] ���L������ �ڑ����s (%d:%s=%dbyte:Err=%d)", my_sThreadName, ii, pInf->cName, pInf->nSize, nErr);
			_ASSERT(FALSE);
			continue;
		}
		LOG(em_MSG), "[%s] ���L������ �ڑ����� (%d:%s=%dbyte)", my_sThreadName, ii, pInf->cName, pInf->nSize);
	}

}

//------------------------------------------
// ���L�������N���[�Y
//------------------------------------------
void DataSetBaseManager::OnCloseTbl()
{
	// ���L���������J������
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];

		mem_close(pInf->h, (LPVOID *)pInf->pTbl);
	}

	LOG(em_MSG), "[%s] ���L������ �ؒf����", my_sThreadName);
}

//------------------------------------------
// ���L������������
// bMode ���������[�h True:�S�Ẵe�[�u���N���A False:���ʂ̏�����
//------------------------------------------
void DataSetBaseManager::OnClearTbl(bool bMode)
{
	LOG(em_MSG), "[%s] ���L������ ������ ���[�h=%d(0:�ʏ평���� 1:���S������)", my_sThreadName, bMode);

	// ������
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		
		if( ! bMode && pInf->bNomalCrearCancel ) continue;

_RPT4(_CRT_WARN, "3-%d[%s] (%d)(%p)\n", ii, pInf->cName, pInf->nSize, *pInf->pTbl);
		memset(*pInf->pTbl,	0x00, pInf->nSize);
	}
}

//------------------------------------------
// �S���L������ �o�C�i���o��
//------------------------------------------
void DataSetBaseManager::OnOutputTbl()
{
	std::ofstream os;
	CString sWk;

	// �o��
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		
		sWk.Format(".\\%s.bin", pInf->cName);
		os.open(sWk, std::ios::out | std::ios::binary);
		os.write((char const*)*pInf->pTbl, pInf->nSize );
		os.close();

	}

	LOG(em_MSG), "[%s] �S���L������ �f�o�b�N�o�͊���", my_sThreadName);
}

//------------------------------------------
// �S���L������ �o�C�i���Ǎ�
//------------------------------------------
void DataSetBaseManager::OnInputTbl()
{
	std::ifstream is;
	CString sWk;

	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];

		sWk.Format(".\\%s.bin", pInf->cName);
		is.open(sWk, std::ios::in | std::ios::binary);
		is.read((char *)*pInf->pTbl, pInf->nSize  );
		is.close();
	}
	LOG(em_MSG), "[%s] �S���L������ �f�o�b�N�Ǎ�����", my_sThreadName);
}

