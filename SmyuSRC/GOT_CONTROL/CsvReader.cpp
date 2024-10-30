#include "StdAfx.h"
#include "CsvReader.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
CsvReader::CsvReader(void) :
ThreadManager("CsvReader")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CsvReader::~CsvReader(void)
{
}


//------------------------------------------
// �V�i���I���
// CString sPath CSV�V�i���I�t���p�X
// std::vector<TYP_SINA_INF*>* pEvent	�V�i���I���X�g
//------------------------------------------
bool CsvReader::CreatEvents(CString sPath, std::vector<TYP_SINA_INF*>* pEvent)
{
	std::vector<CString>	listRow;
	
	//// CSV�f�[�^�擾
	if( ! ReadCsv(sPath, &listRow) ) return false;

	//// CSV�f�[�^����V�i���I�f�[�^����
	if( ! CsvToEvent(&listRow, pEvent) ) return false;

	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����


//------------------------------------------
// CSV�t�@�C���ǂݍ���
// CString sPath CSV�V�i���I�t���p�X
// std::vector<CString>* pRow	1�s�P�ʂ̕������X�g
//------------------------------------------
bool CsvReader::ReadCsv(CString sPath, std::vector<CString>* pRow)
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
		while(true) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '	' ) continue;
			if( cWk[nWk] == '#' || cWk[nWk] == 0x0a) break;

			bAddOk = true;
			break;
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
bool CsvReader::CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent)
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
bool CsvReader::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// �O��̃X�y�[�X�A�^�u����菜��
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����������
//// �����쓮�V�i���I
void CsvReader::RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

void CsvReader::RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_RAISE_TIM_CYCLE* pD = new TYP_RAISE_TIM_CYCLE;

	//// �w�b�_�[
	pInf->emRaise = DIV_KIND_MAIL;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// 1���
	GetColumn(sVal, curPos, sWk);
	pD->nFirst = atoi(sWk);

	// 2��ڈȍ~
	GetColumn(sVal, curPos, sWk);
	pD->nCycle = atoi(sWk);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �C�x���g��
void CsvReader::EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1��P�ʂ̕�����
	TYP_EVENT_PLG_SPEED* pD = new TYP_EVENT_PLG_SPEED;

	//// �w�b�_�[
	pInf->emEvent = DIV_KIND_PLG;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// �Ώۃ`�����l��
	GetColumn(sVal, curPos, sWk);
	pD->nCh = atoi(sWk);

	// ���C�����x
	GetColumn(sVal, curPos, sWk);
	pD->nMpm = atoi(sWk);
}

void CsvReader::EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

void CsvReader::EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

		GetColumn(sVal, curPos, sWk);
		strcpy_s(pD->que.fc.data, sizeof(pD->que.fc.data), sWk);

	} else if(0 == strcmp(sSubMode, "MIX")) {
		pD->emMainKind = DIV_MAIL_MIX;

		for(int ii=0; ii<5; ii++) {
			GetColumn(sVal, curPos, sWk);
			strcpy_s(pD->que.mix.cdata[ii], sizeof(pD->que.mix.cdata[ii]), sWk);
		}

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.mix.idata[nCnt] = atoi(sWk);
			nCnt++;
		}

	}
}