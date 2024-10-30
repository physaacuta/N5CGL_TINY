#include "StdAfx.h"
#include "SmyuBaseManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


CString CreatThreadName(const char* pThreadName, int id) { CString s; s.Format("%s_%d", pThreadName, id); return s;}

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SmyuBaseManager::SmyuBaseManager(const char* pThreadName, int nMyID) :
m_nMyID(nMyID),
ThreadManager(CreatThreadName(pThreadName, nMyID))
{
	
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SmyuBaseManager::~SmyuBaseManager(void)
{
	// �J��
	for(DWORD ii=0; ii<m_lstStory.size(); ii++) {
		SAFE_DELETE( m_lstStory[ii]->pRaise );
		SAFE_DELETE( m_lstStory[ii]->pEvent );
		SAFE_DELETE( m_lstStory[ii] );
	}
}


//------------------------------------------
// �V�i���I�ǂݍ���
// LPCSTR sPath CSV�t�@�C���p�X
//------------------------------------------
bool SmyuBaseManager::SetCsvFilePath(LPCSTR sPath)
{
	std::vector<CString>	listRow;
	
	LOG(em_MSG), "[%s] �V�i���I�ǂݍ��݊J�n [%s]----->>>", my_sThreadName, sPath);

	//// CSV�f�[�^�擾
	if( ! ReadCsv(sPath, &listRow) ) return false;

	//// CSV�f�[�^����V�i���I�f�[�^����
	if( ! CsvToEvent(&listRow, &m_lstStory) ) return false;

	LOG(em_MSG), "[%s] �V�i���I�ǂݍ��݊���", my_sThreadName);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�i���I������

//------------------------------------------
// CSV�t�@�C���ǂݍ���
// CString LPCSTR CSV�V�i���I�t���p�X
// std::vector<CString>* pRow	1�s�P�ʂ̕������X�g
//------------------------------------------
bool SmyuBaseManager::ReadCsv(LPCSTR sPath, std::vector<CString>* pRow)
{
	char cWk[2048];
	int nWk;

	//================================================
	// �I�[�v��
	FILE* fp = fopen(sPath, "rt"); 
	if(NULL == fp) {
		LOG(em_ERR), "[%s] CSV�t�@�C�� �I�[�v�����s [%s]", my_sThreadName, sPath);
		return false;
	}

	//================================================
	// �ǂݍ���
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1�s�擾
		if( 0 == ret ) break;

		//// �󔒍s, �R�����g�s�� �ΏۊO
		bool bAddOk = false;
		nWk = -1;
		int nCnt = strlen(cWk);
		for(int ii=0; ii<nCnt; ii++) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '	' ) continue;
			if( cWk[nWk] == '#' || cWk[nWk] == 0x0a) {
				// �����ŕ����񂪏I���̔F���ɂ���
				cWk[nWk] = 0x00;
				break;
			}

			bAddOk = true;
		}

		//// �ǉ��Ώ�
		if(bAddOk) {
			CString s;
			s = cWk;
			s.Trim(_T(" 	"));
			s.Trim(_T(0x0a));
			pRow->push_back(s);
		}
	}

	//================================================
	// �N���[�Y
	fclose(fp);
	return true;
}

//------------------------------------------
// CSV ���� �C�x���g�ɐ���
// std::vector<CString>* pRow	1�s�P�ʂ̕������X�g
// std::vector<CString>* pEvent	�C�x���g�f�[�^�ɐ���
//------------------------------------------
bool SmyuBaseManager::CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent)
{
	//// 1�s�Â̃f�[�^���A�C�x���g�f�[�^�ɕϊ�
	for(DWORD ii=0; ii<pRow->size(); ii++) {
		CString			sVal	= pRow->at(ii);		// 1�s���̕�����
		CString			sWk;						// 1��P�ʂ̕�����
		int				curPos	= 0;				// ��؂�ʒu
		TYP_SINA_INF*	inf		= NULL;				// �V�i���I
		//================================================
		// ����������  �C�x���g��ʓ���
		
		if( GetColumn(sVal, curPos, sWk) ) {
			
			//// �����쓮�V�i���I
			if(			0 == strcmp(sWk, STR_KIND_PLG)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Plg_Pos(sVal, curPos, inf);


			//// �C�x���g�쓮�V�i���I
			} else if(	0 == strcmp(sWk, STR_KIND_MAIL)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Evt_Mail(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_PO)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Evt_Po(sVal, curPos, inf);
			

			//// �C�x���g�쓮�V�i���I
			} else if(	0 == strcmp(sWk, STR_KIND_TIME)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Time_Cycle(sVal, curPos, inf);


			} else {
				LOG(em_ERR), "[%s] ���������� �C�x���g��ʈُ� [%s]", my_sThreadName, sVal);
				break;
			}
		} else {
			LOG(em_ERR), "[%s] ���������� �C�x���g��ʖ��� [%s]", my_sThreadName, sVal);
			SAFE_DELETE(inf);
		}

		//================================================
		// �C�x���g��  �C�x���g��ʓ���
		if( GetColumn(sVal, curPos, sWk) ) {
			if(			0 == strcmp(sWk, STR_KIND_PLG)) {
				EventData_Plg(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_PI)) {
				EventData_Pi(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_MEM)) {
				EventData_Mem(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_MAIL)) {
				EventData_Mail(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_TIME)) {
				EventData_Time(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_ZZ)) {
				EventData_Zz(sVal, curPos, inf);

			} else {
				LOG(em_ERR), "[%s] �C�x���g�� �C�x���g��ʈُ� [%s]", my_sThreadName, sVal);
				SAFE_DELETE(inf->pRaise);
				SAFE_DELETE(inf);
				break;
			}
		} else {
			LOG(em_ERR), "[%s] �C�x���g�� �C�x���g��ʖ��� [%s]", my_sThreadName, sVal);
			SAFE_DELETE(inf->pRaise);
			SAFE_DELETE(inf);
		}


		//================================================
		// �o�^
		pEvent->push_back(inf);
	}
	return true;
}

//------------------------------------------
// �P�s�̃f�[�^���� , �P�ʂ̕�������擾
// CString& sVal 1�s�P�ʂ̏��
// int& curPos ��؂�ʒu
// CString& sToken �擾������̕�����
// �߂�l ���A���
//------------------------------------------
bool SmyuBaseManager::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// �O��̃X�y�[�X�A�^�u����菜��
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


// ##########################################################################
// ����������
//// �����쓮�V�i���I
void SmyuBaseManager::RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_RAISE_PLG_POS* pD = new TYP_RAISE_PLG_POS;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_PLG;
	pInf->pRaise  = (BYTE*)pD;


	//----------------------------
	// �����Q�ƃ`�����l��
	GetColumn(sVal, curPos, sWk);
	pD->nCh = atoi(sWk);

	// ���۔������� [mm]
	GetColumn(sVal, curPos, sWk);
	pD->nLen = atoi(sWk);
}

//// �C�x���g�쓮�V�i���I
void SmyuBaseManager::RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_RAISE_EVT_MAIL* pD = new TYP_RAISE_EVT_MAIL;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_MAIL;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// �^�X�N����
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTaskName, sizeof(pD->cTaskName), sWk);

	// �C�x���gNo
	GetColumn(sVal, curPos, sWk);
	pD->nEventNo = atoi(sWk);
}

void SmyuBaseManager::RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_RAISE_EVT_PO* pD = new TYP_RAISE_EVT_PO;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_PO;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// �{�[�hNo
	GetColumn(sVal, curPos, sWk);
	pD->nBood = atoi(sWk);

	// PortNo
	GetColumn(sVal, curPos, sWk);
	pD->nPort = atoi(sWk);

	// BitNo
	GetColumn(sVal, curPos, sWk);
	pD->nBit = atoi(sWk);

	// �F���^�C�~���O
	GetColumn(sVal, curPos, sWk);
	pD->nMode = atoi(sWk);
}

//// ���ԋ쓮�V�i���I
void SmyuBaseManager::RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_RAISE_TIM_CYCLE* pD = new TYP_RAISE_TIM_CYCLE;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_TIME;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// �z��ʒu
	GetColumn(sVal, curPos, sWk);
	pD->nID = atoi(sWk);

	// 1���
	GetColumn(sVal, curPos, sWk);
	pD->nFirst = atoi(sWk);

	// 2��ڈȍ~
	GetColumn(sVal, curPos, sWk);
	pD->nCycle = atoi(sWk);
}

// ##########################################################################
// �C�x���g��
void SmyuBaseManager::EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����

	//----------------------------
	// �T�u��ʂ̓���
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);

	// �T�u��ʓ���
	if(0 == nSubKind) {
		pInf->emEvent	= DIV_KIND_PLG;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PLG_SPEED* pD = new TYP_EVENT_PLG_SPEED;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// �Ώۃ`�����l��
		GetColumn(sVal, curPos, sWk);
		pD->nCh = atoi(sWk);

		// ���C�����x
		GetColumn(sVal, curPos, sWk);
		pD->nMpm = atoi(sWk);

	} else if(1 == nSubKind) {
		pInf->emEvent	= DIV_KIND_PLG;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PLG_FRAMECYCLE* pD = new TYP_EVENT_PLG_FRAMECYCLE;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// ���C�����x
		GetColumn(sVal, curPos, sWk);
		pD->nCycle = atoi(sWk);

	}
}

void SmyuBaseManager::EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����

	//----------------------------
	// �T�u��ʂ̓���
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);


	// �T�u��ʓ���
	if(0 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_BIT* pD = new TYP_EVENT_PI_BIT;
		pInf->pEvent    = (BYTE*)pD;


		//----------------------------
		// �{�[�hNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// BitNo
		GetColumn(sVal, curPos, sWk);
		pD->nBit = atoi(sWk);

		// �Z�b�g�l
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);


	} else if(1 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_PORT* pD = new TYP_EVENT_PI_PORT;
		pInf->pEvent  = (BYTE*)pD;


		//----------------------------
		// �{�[�hNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// �Z�b�g�l
		GetColumn(sVal, curPos, sWk);
		pD->nBitSet = (BYTE) atoi(sWk);


	} else if(2 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_BITPULS* pD = new TYP_EVENT_PI_BITPULS;
		pInf->pEvent    = (BYTE*)pD;


		//----------------------------
		// �{�[�hNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// BitNo
		GetColumn(sVal, curPos, sWk);
		pD->nBit = atoi(sWk);

		// �Z�b�g�l
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);

		// ON����
		GetColumn(sVal, curPos, sWk);
		pD->nTime = atoi(sWk);

	} else {
		pInf->emEvent = DIV_KIND_NON;
		LOG(em_ERR), "[%s] PI �T�u��ʈُ� [%s]", my_sThreadName, sVal);
		return;
	}
}
void SmyuBaseManager::EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_EVENT_MEM_WRITE* pD = new TYP_EVENT_MEM_WRITE;

	//// �w�b�_�[
	pInf->emEvent = DIV_KIND_MEM;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// �Ώۃe�[�u��
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTblName, sizeof(pD->cTblName), sWk);

	// �������݊J�n�ʒu
	GetColumn(sVal, curPos, sWk);
	pD->nIndex = atoi(sWk);

	// �o�C�i���f�[�^���΃p�X
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cWriteDataPath, sizeof(pD->cWriteDataPath), sWk);
}

void SmyuBaseManager::EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_EVENT_MAIL_SEND* pD = new TYP_EVENT_MAIL_SEND;

	//// �w�b�_�[
	pInf->emEvent = DIV_KIND_MAIL;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// ���MPC
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cPcName, sizeof(pD->cPcName), sWk);

	// ���M�^�X�N
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTaskName, sizeof(pD->cTaskName), sWk);

	// �C�x���gNo
	GetColumn(sVal, curPos, sWk);
	pD->que.nEventNo = atoi(sWk);


	//----------------------------
	// �T�u��ʂ̓���
	int nCnt =0;
	CString sSubMode;		
	GetColumn(sVal, curPos, sSubMode);
	

	// �T�u��ʓ���
	if(0 == strcmp(sSubMode, "FL")) {
		pD->emMainKind = DIV_MAIL_FL;

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.fl.data[nCnt] = atoi(sWk);
			nCnt++;
		}

	} else if(0 == strcmp(sSubMode, "FC")) {
		pD->emMainKind = DIV_MAIL_FC;

		sWk = "";
		GetColumn(sVal, curPos, sWk);
		strcpy_s(pD->que.fc.data, sizeof(pD->que.fc.data), sWk);

	} else if(0 == strcmp(sSubMode, "MIX")) {
		pD->emMainKind = DIV_MAIL_MIX;

		for(int ii=0; ii<5; ii++) {
			sWk = "";
			GetColumn(sVal, curPos, sWk);
			strcpy_s(pD->que.mix.cdata[ii], sizeof(pD->que.mix.cdata[ii]), sWk);
		}

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.mix.idata[nCnt] = atoi(sWk);
			nCnt++;
		}

	}
}
void SmyuBaseManager::EventData_Time(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_EVENT_TIM_MODE* pD = new TYP_EVENT_TIM_MODE;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_TIME;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// �z��ʒu
	GetColumn(sVal, curPos, sWk);
	pD->nIndex = atoi(sWk);

	// �z��ʒu
	GetColumn(sVal, curPos, sWk);
	pD->nID = atoi(sWk);

	// 0:��~�w��  1:�J�n�w��
	GetColumn(sVal, curPos, sWk);
	pD->nMode = atoi(sWk);
}

void SmyuBaseManager::EventData_Zz(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����

	//----------------------------
	// �T�u��ʂ̓���
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);


	// �T�u��ʓ���
	if(0 == nSubKind) {
		pInf->emEvent   = DIV_KIND_ZZ;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_ZZ_RESET* pD = new TYP_EVENT_ZZ_RESET;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// �����Q�ƃ`�����l�� (0�I���W��) 0�`7
		GetColumn(sVal, curPos, sWk);
		pD->nCh = atoi(sWk);

		// �V�i���I�I���敪 (0:�������Ȃ�  1:�ŏ��ɖ߂�)
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);
	} else {
		pInf->emEvent = DIV_KIND_NON;
		LOG(em_ERR), "[%s] ZZ �T�u��ʈُ� [%s]", my_sThreadName, sVal);
		return;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �C�x���g����

//------------------------------------------
// �C�x���g����
// EM_DIV_KIND emEvent �C�x���g���
// int nEventSub �T�u���
// BYTE* pEvent �\���̃|�C���^
//------------------------------------------
void SmyuBaseManager::RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent)
{
	////// �V�i���I��������
	switch (emEvent) {
//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PLG:										
		if(0==nEventSub) {
			TYP_EVENT_PLG_SPEED* p = (TYP_EVENT_PLG_SPEED*)pEvent;
			
			if(0 == p->nMpm) {
				mcls_pPlg->SetTimer_SpeedCycle(p->nCh, false);
				LOG(em_INF), "[%s] �V�i���I���s<���x�ύX> Ch=%d; ��~", my_sThreadName, p->nCh);
			} else {
				mcls_pPlg->SetSaveData().nSpeed[p->nCh] = p->nMpm;

				// �����ύX
				double dWk =  mcls_pPlg->SetSaveData().nSpeed[0] / 60.0 * 1000.0;
				dWk  = dWk / ( mcls_pPlg->GetSetting().dResY[0]*mcls_pPlg->GetSetting().nFrameSizeY);
				int nWk = (int)(1.0 / dWk * 1000.0);
				mcls_pPlg->SetSaveData().nFrameCycle = nWk;


				// �ύX�ʒm�B 	
				mcls_pPlg->SetTimer_SpeedCycle(p->nCh, true);
				LOG(em_INF), "[%s] �V�i���I���s<���x�ύX> Ch=%d; mpm=%d", my_sThreadName, p->nCh, p->nMpm);
			}
		} else if(1==nEventSub) {
			TYP_EVENT_PLG_FRAMECYCLE* p = (TYP_EVENT_PLG_FRAMECYCLE*)pEvent;

			if(0 == p->nCycle) {
				// ��~�����́A���x���c���Ă���
				mcls_pPlg->SetTimer_SpeedCycle(0, false);
				LOG(em_INF), "[%s] �V�i���I���s<�ڰю����ύX> ��~", my_sThreadName);
			} else {
				mcls_pPlg->SetSaveData().nFrameCycle = p->nCycle;
				// �ύX�ʒm�B (True��n���Ă����΁A�֐����ŏ���ɒ�~�J�n�𔻒f���Ă����)			
				mcls_pPlg->SetTimer_SpeedCycle(0, true);
				LOG(em_INF), "[%s] �V�i���I���s<�ڰю����ύX> speed=%d", my_sThreadName, p->nCycle);
			}
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PI:										
		if(0==nEventSub) {
			TYP_EVENT_PI_BIT* p = (TYP_EVENT_PI_BIT*)pEvent;
			
			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBit, __BOOL p->nMode); 

			//LOG(em_INF), "[%s] �V�i���I���s<PI BIT�P��> Bood=%d; Port=%d; Bit=%d; set=%d", my_sThreadName, p->nBood, p->nPort, p->nBit, p->nMode);


		} else if(1==nEventSub) {
			TYP_EVENT_PI_PORT* p = (TYP_EVENT_PI_PORT*)pEvent;

			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBitSet); 

			LOG(em_INF), "[%s] �V�i���I���s<PI PORT�P��> Bood=%d; Port=%d; set=%02X", my_sThreadName, p->nBood, p->nPort, p->nBitSet );


		} else if(2==nEventSub) {
			TYP_EVENT_PI_BITPULS* p = (TYP_EVENT_PI_BITPULS*)pEvent;

			// ��
		
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_PO:										
//		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MEM:										
		if(0==nEventSub) {
			TYP_EVENT_MEM_WRITE* p = (TYP_EVENT_MEM_WRITE*)pEvent;
			
			// ���L�������A�N�Z�X
			HANDLE h;
			BYTE*  t = NULL;
			mem_connect(&h, (LPVOID *)&t, p->cTblName);
			if(h == NULL) {
				LOG(em_ERR), "[%s] ���L������ �A�N�Z�X���s [%s]", my_sThreadName, p->cTblName);
				return;
			}

			// �t�@�C���T�C�Y���擾
			int nSize;
			nSize = KizuFunction::GetFileSize(p->cWriteDataPath);

			// �o�C�i���t�@�C���Z�b�g
			std::ifstream   is;
			is.open(p->cWriteDataPath, std::ios::binary);
			is.read( (char*)&t[p->nIndex], nSize);//1024*1024*1024);
			is.close();

			// ���L�������J��
			mem_close(&h, (LPVOID *)&t);

			LOG(em_INF), "[%s] �V�i���I���s<���L����������> Name=%s; Start=%d; Path=%s;", my_sThreadName, p->cTblName, p->nIndex, p->cWriteDataPath );
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MAIL:				
		if(0==nEventSub) {
			TYP_EVENT_MAIL_SEND* p = (TYP_EVENT_MAIL_SEND*)pEvent;

			send_mail(p->cTaskName, p->cPcName, &p->que);

			LOG(em_INF), "[%s] �V�i���I���s<Mail���M> PC=%s; Task=%s; EvtNo=%d; Type=%d", my_sThreadName, p->cPcName, p->cTaskName, p->que.nEventNo, p->emMainKind);
			if( DIV_MAIL_FL == p->emMainKind) {
				LOG(em_INF), "   ���� L0=%d, L1=%d, L2=%d", p->que.fl.data[0], p->que.fl.data[1], p->que.fl.data[2]);

			} else if( DIV_MAIL_FC == p->emMainKind) {
				LOG(em_INF), "   ���� C=%s", p->que.fc.data);
			} else if( DIV_MAIL_MIX == p->emMainKind) {
				LOG(em_INF), "   ���� ML0=%d, ML1=%d, ML2=%d, C0=%s, C1=%s, C2=%s", p->que.mix.idata[0], p->que.mix.idata[1], p->que.mix.idata[2], p->que.mix.cdata[0], p->que.mix.cdata[1],  p->que.mix.cdata[2]);
			}
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_TIME:	
		if(true) {

		}
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] �s��=%d", my_sThreadName, emEvent );
		return;
	}
}

