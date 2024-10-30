#include "StdAfx.h"
#include "OpSendFunc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "HtRecv.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �g�p����N���X�̒�`

// �C���N���[�h��`
#include "StatusManager.h"												// �X�e�[�^�X������N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ�
#include "CoilManager.h"												// �R�C�����Ǘ��N���X
#include "PlgManager.h"													// PLG�Ǘ��N���X
#include "HtRecv.h"														// ����˓����ԒʐM�Ǘ��N���X
//
//// �ÓI�̈�
StatusManager*					mcls_pStatus	= NULL;					// �X�e�[�^�X������N���X
ParamManager*					mcls_pParam		= NULL;					// �p�����[�^�Ǘ�
CoilManager*					mcls_pCoil		= NULL;					// �R�C���Ǘ��N���X
PlgManager*						mcls_pPlg		= NULL;					// PLG�Ǘ��N���X
HtRecv*							mcls_pHtRecv[NUM_MEN];					// ����˓����ԒʐM�Ǘ��N���X



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ÓI�ϐ��̏�����

// ��������p
CRITICAL_SECTION*				OpSendFunc::m_csLockCoil = NULL;	
ThreadQueue<SOCK_BASE_HEAD>*	OpSendFunc::mque_pOpSend[HY_SOCK_NUM] = { NULL };

// ���O�p
LogFileManager*					OpSendFunc::mcls_pLog	= NULL;
LogFileManager*					OpSendFunc::mcls_pLogMark = NULL;
CString							OpSendFunc::my_sThreadName;



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v���p�e�B

void OpSendFunc::SetStatusMgr(void* pCls)	{ mcls_pStatus	= (StatusManager*)pCls; }
void OpSendFunc::SetParamMgr(void* pCls)	{ mcls_pParam	= (ParamManager*)pCls; }
void OpSendFunc::SetCoilMgr(void* pCls)		{ mcls_pCoil	= (CoilManager*)pCls; }
void OpSendFunc::SetPlgMgr(void* pCls)		{ mcls_pPlg		= (PlgManager*)pCls; }
void OpSendFunc::SetHtRecv(int men, void* pCls)		{ mcls_pHtRecv[men]	= (HtRecv*) pCls; }



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//==========================================
// �R���X�g���N�^
//==========================================
OpSendFunc::OpSendFunc() 
{
	my_sThreadName = "OpSendFnc";
}

//==========================================
// �f�X�g���N�^
//==========================================
OpSendFunc::~OpSendFunc(void)
{
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ���


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���M

//==========================================
// #0 �X�e�[�^�X����\���@�\��
//==========================================
void OpSendFunc::SendOp_Status()
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// �o�͏����`�F�b�N
	for(int ii=0; ii<HY_SOCK_NUM; ii++) {
		if (NULL == mque_pOpSend[ii]) return;
	}
	
	//================================================
	////// �\���@�\�֏�ԕύX�ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_STATUS);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_STATUS&	data = (TO_SO_DATA_STATUS&)pSendBuf[sizeof(TO_SO_HEAD)];	// �f�[�^���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_STATUS;							// �X�e�[�^�X���

	// �f�[�^��(�\����)
	data.nUnten 			= mcls_pStatus->GetUnten(); 
	data.nKadou 			= mcls_pStatus->GetKadou(); 
	memcpy( &data.status, &mcls_pStatus->GetStatusNowSystem(), sizeof(TYP_STATUS_NOWSYSTEM));

	//================================================
	//// ���M�˗�
//��	LOG(em_MSG), "[%s] �����˕\���� �X�e�[�^�X���", my_sThreadName);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 5)) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �X�e�[�^�X��񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�X�e�[�^�X���]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #1 ���݂̌����p�R�C������\���@�\��
//------------------------------------------
// COIL_INF const*	pCoilInf : �Ώۂ̃R�C�����
//==========================================
void OpSendFunc::SendOp_CoilInf_Ken(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// �o�͏����`�F�b�N
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// �\���@�\�փR�C�����ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_COIL);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_COIL&	data = (TO_SO_DATA_COIL&)pSendBuf[sizeof(TO_SO_HEAD)];		// �f�[�^���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_COIL_KEN;							// �R�C�����i�����p�j

	// �f�[�^��(�\����)
	memcpy( &data.coilbase, &pCoilInf->data,   sizeof(COIL_BASEDATA));
	memcpy( &data.coilinf,  &pCoilInf->data_u, sizeof(PRO_TYPE_DATA_COILINF));
	for(int ii=0; ii<NUM_MEN; ii++) {
		data.scode[ii]			= pCoilInf->setting.scode[ii];			// SPM�������̌����\�ʏ��
		data.pcode[ii]			= pCoilInf->setting.pcode[ii];			// SPM�������̌����p�^�[��
		data.scode_spmoff[ii]	= pCoilInf->setting.scode_spmoff[ii];	// SPM�J�����̌����\�ʏ��
		data.pcode_spmoff[ii]	= pCoilInf->setting.pcode_spmoff[ii];	// SPM�J�����̌����p�^�[��
	}
	data.nMapColNum			= pCoilInf->nMapCol;

	//================================================
	//// ���M�˗�
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// ��Γo�^
			LOG(em_MSG), "[%s] <%s> �����˕\���� �����p�R�C����񑗐M (%s) �R�C��No(%s)", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);

		} else {									// �󂫖���
			LOG(em_ERR), "[%s] <%s> �����˕\���� �����p�R�C����񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[�����p�R�C�����] <%s> <%s> �R�C��No(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #2 �R�C�����т�\���@�\��
//------------------------------------------
// COIL_INF const*	pCoilInf : �Ώۂ̃R�C�����
//==========================================
void OpSendFunc::SendOp_CoilResult(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
		
	//// �o�͏����`�F�b�N
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// �\���@�\�֏�ԕύX�ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_COIL_RESULT;						// �R�C�����i�\���p�j

	// �w�b�_�[��(�g��)
	head.data_coilresult.len = pCoilInf->nLength;

	//================================================
	//// ���M�˗�
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// ��Γo�^
			LOG(em_MSG), "[%s] <%s> �����˕\���� �R�C�����ё��M (%s) �R�C��No(%s) [%dmm]", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn, pCoilInf->nLength );
		} else {									// �󂫖���
			LOG(em_ERR), "[%s] <%s> �����˕\���� �R�C�����ё��M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[�R�C������ <%s> <%s> �R�C��No(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #3 ���݂̕\���p�R�C������\���@�\��
//------------------------------------------
// COIL_INF const*	pCoilInf �Ώۂ̃R�C�����
//==========================================
void OpSendFunc::SendOp_CoilInf_Dsp(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// �o�͏����`�F�b�N
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// �\���@�\�֏�ԕύX�ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_COIL);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];						// �w�b�_���G�C���A�X
	TO_SO_DATA_COIL&	data = (TO_SO_DATA_COIL&)pSendBuf[sizeof(TO_SO_HEAD)];	// �f�[�^���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;										// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_COIL_DSP;					// �R�C�����i�\���p�j
	head.data_coilinf_dsp.nSpmUmu = pCoilInf->nSt_Spm;					// SPM�����L���@0�F�J����� 1�F�������

	// �f�[�^��(�\����)
	memcpy(&data.coilbase, &pCoilInf->data, sizeof(COIL_BASEDATA));
	memcpy(&data.coilinf, &pCoilInf->data_u, sizeof(PRO_TYPE_DATA_COILINF));
	for (int ii = 0; ii<NUM_MEN; ii++)
	{
		data.scode[ii]			= pCoilInf->setting.scode[ii];			// SPM�������̌����\�ʏ��
		data.pcode[ii]			= pCoilInf->setting.pcode[ii];			// SPM�������̌����p�^�[��
		data.scode_spmoff[ii]	= pCoilInf->setting.scode_spmoff[ii];	// SPM�J�����̌����\�ʏ��
		data.pcode_spmoff[ii]	= pCoilInf->setting.pcode_spmoff[ii];	// SPM�J�����̌����p�^�[��
	}
	data.nMapColNum = pCoilInf->nMapCol;

	//================================================
	//// ���M�˗�
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// ��Γo�^
			LOG(em_MSG), "[%s] <%s> �����˕\���� �\���p�R�C����񑗐M (%s) �R�C��No(%s)", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);

		} else {									// �󂫖���
			LOG(em_ERR), "[%s] <%s> �����˕\���� �\���p�R�C����񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[�\���p�R�C����� <%s> <%s> �R�C��No(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #4 �r���(�r���+�r�摜)��\���@�\��
//------------------------------------------
// int men	0:�\, 1:��
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
// DETECT_BASE_DATA const* pData ��������(�r���)
// DEFECT_DATA_IMAGE* pImg �����p�摜���|�C���^
// float dLen �r�����ʒu�iSPM��Ԃ����������r�����ʒu�j
//==========================================
void OpSendFunc::SendOp_DetectData(int men, COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, float dLen)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	int					nWk = 0;

	//// �o�͏����`�F�b�N
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pCoilInf ) return;


	////================================================
	//// ���M�̈�m��
	
	// �摜���f�[�^�v�Z
	nWk = 0;
	for(int ii=0; ii<MAX_IMAGE_CNT; ii++) nWk += pImg->nSize[ii];

	// ���M�̈�m�� (-4 �́A�{��-1�����o�E���_��������ׂ̏��u)
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) - 4 + nWk;		// ���̎��_�ł́A�܂���
	pSendBuf  = new BYTE[nSocnSize];
	//memset(pSendBuf, 0x00, nSocnSize);		// ����͂��邩�H�H
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_DETECT&	data = (TO_SO_DATA_DETECT&)pSendBuf[sizeof(TO_SO_HEAD)];	// �f�[�^���G�C���A�X

	BYTE* pJpegBuf = data.image;		// �摜�f�[�^�擪�|�C���^


	//================================================
	// �ʏ�̃f�[�^�Z�b�g
	//// �f�[�^�Z�b�g (���R�C�����؂�ւ���Ă��܂��O��̃R�C���̏��𑗐M����B�����đO�̏�񂾂���)
	// �w�b�_�[��(�x�[�X)
	memset(&head, 0x00, sizeof(TO_SO_HEAD));
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);	
	head.men				= men;
	head.kind				= TO_SO_DATA_KIND_DETECT;					// �r��� (�s����)
	
	// �f�[�^��(�\����)
	memcpy( &data.datas, pData, sizeof(DETECT_BASE_DATA));

	// ������[����̈ʒu[mm]
	data.datas.y = dLen;
	
	//----------------------------------------
	// �X�̉摜�f�[�^�𑗐M�̈�ɃZ�b�g
	nWk = 0;
	for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
		// �r�摜�͑��݂��邩�H
		if (0 >= pImg->nSize[ii] ) {
			data.image_len[ii] = 0;
			continue;
		}
		// �\���Ώۂ��r�摜���H
		if (!mcls_pParam->Check_ImgDsp(pData->nTid, ii)) {
			data.image_len[ii] = 0;
			continue;
		}
		memcpy( &pJpegBuf[nWk], pImg->pImg[ii], pImg->nSize[ii]);
		nWk += pImg->nSize[ii];

		data.image_len[ii] = pImg->nSize[ii];
	}



	//================================================
	//// ���M�˗�
//��LOG(em_MSG), "[%s] �����˕\���� �r��� <%s>[%s][No=%d][t=%d][g=%d]", my_sThreadName, DivNameManager::GetTorB(head.men), head.KizukenNo, data.data.nKizuNo,	data.data.nTid, data.data.nGid);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 10) ) {
			delete pSend;
			LOG(em_ERR), "[%s] <%s> <%d> �����˕\���� �r��񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR(), men);
			syslog(SYSNO_QUEFULL_WAR, "[%d TO_SO_HEAD TO_SO_DATA_KIND_DETECT]", men);
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #6 �ʔ����c�� & ���M
//------------------------------------------
// int men	0:�\, 1:��
// int y	�ʔ���[m]
//==========================================
void OpSendFunc::SendOp_Length(int men, int y)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	const COIL_INF*		pCoil;					// ���[�N
	const COIL_INF*		pCoilBaseKen = mcls_pCoil->GetCoilBaseKen();		// �����p�R�C�����
	const COIL_INF*		pCoilBaseDsp = mcls_pCoil->GetCoilBaseDsp();		// �\���p�R�C�����


	// �o�͏����`�F�b�N
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pCoilBaseKen ) return;		// �����p��񖳂��̏ꍇ�͑��M���Ȃ�
	if( NULL == pCoilBaseDsp ) return;		// �\���p��񖳂��̏ꍇ�͑��M���Ȃ�
	if( ! mcls_pStatus->IsRun() ) return;	// �������̂�


	//// �R�C�����N���X �N���e�B�J��
	//AutoLock autolock(m_csLockCoil);
 
	//================================================
	////// �\���@�\�֕\���p�ʔ����ύX�ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_LENGTH);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_LENGTH&	data = (TO_SO_DATA_LENGTH&)pSendBuf[sizeof(TO_SO_HEAD)];	// �f�[�^���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memset( head.KizukenNo, 0x00, SIZE_KIZUKEN_NO);								// �Ǘ�No����
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_LENGTH;							// �\����_�ʒu�ł̃R�C���ʔʒu���


	// �f�[�^��(�\����)
	// ����
	for(int ii=0; ii<MAX_DISP_POS; ii++) {
		pCoil = mcls_pCoil->GetCoilAll(ii);
		if(NULL == pCoil) continue;

		memcpy(data.dsp[ii].KizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
		// ���݂̒ʔ��������̂܂܃Z�b�g
		data.dsp[ii].len = (long)(mcls_pPlg->GetLen_All(ii) / 1000.0);
	}

	//================================================
	//// ���M�˗�
//��	LOG(em_MSG), "[%s] �����˕\���� �ʔ��� [%s][%dm]", my_sThreadName, data.dsp[0].KizukenNo, data.dsp[0].len);
//LOG(em_MSG), "������� Head(%d/%s/%d) Size(max:%d=send:%d) KenP(%s:%dm) KenD(%s:%dm) Dsp1(%s:%dm)", 
//	head.kind, head.KizukenNo, head.men, nSocnSize, head.base_head.len, 
//	data.ken[0].KizukenNo, data.ken[0].len, data.kenData[0].KizukenNo, data.kenData[0].len, data.dsp[0].KizukenNo, data.dsp[0].len);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7) ) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �ʔ������M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�ʔ���]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #7 �J�b�g����\���@�\��
//------------------------------------------
// char const*			 cKizukenNo	: �Ǘ�No
// TO_SO_DATA_CUT const* pInf		: �Ώۂ̏��
//==========================================
void OpSendFunc::SendOp_CutInf(char const* cKizukenNo, TO_SO_DATA_CUT const* pInf)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// ���O�`�F�b�N
	if( NULL == mque_pOpSend ) return;
	if( NULL == pInf) return;


	//================================================
	////// �\���@�\�֒ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_CUT);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_CUT&		data = (TO_SO_DATA_CUT&)pSendBuf[sizeof(TO_SO_HEAD)];		// �f�[�^���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy( head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_CUT;								// �J�b�g���

	// �f�[�^��(�\����)
	memcpy( &data, pInf, sizeof(TO_SO_DATA_CUT));


	//================================================
	//// ���M�˗�
	LOG(em_MSG), "[%s] �����˕\����  �J�b�g��� [%s][%d:%dmm]", my_sThreadName, head.KizukenNo, data.nMode, data.nPos );
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20 ) ) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �J�b�g��񓙑��M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�J�b�g���] <%s>]", cKizukenNo);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// �x���Ԃ�\���@�\��
//------------------------------------------
// ALARME_OUTPUT const* pInf �Ώۂ̃A���[�����
//==========================================
void OpSendFunc::SendOp_AlarmData(ALARME_OUTPUT const* pInf)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// �o�͏����`�F�b�N
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pInf) return;

	//================================================
	////// �\���@�\�փR�C�����ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_ALARM);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_ALARM&	data = (TO_SO_DATA_ALARM&)pSendBuf[sizeof(TO_SO_HEAD)];		// �f�[�^���G�C���A�X


	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	head.men				= -1;												// �\���֌W����
	head.kind				= TO_SO_DATA_KIND_ALARM;							// �r�ڋ߃����v���

	// �f�[�^��(�\����)
	for (int ii = 0; ii < MAX_ALARME_POS; ii++)
	{
		switch (ii)
		{
			// �\�ʌ�����
		case 0:
			// �r�ڋ߃����v
			for (int jj = 0; jj<NUM_MEN; jj++)
			{
				for (int kk = 0; kk < LAMP_COL_NUM; kk++)
				{
					// �d�r
					if (em_RAMP_STAT_ON == pInf->ranp[jj][kk][em_RAMP_JYU])		data.a_mode[ii][jj][kk] = 2;
					// �y�r
					else if (em_RAMP_STAT_ON == pInf->ranp[jj][kk][em_RAMP_KEI])data.a_mode[ii][jj][kk] = 1;
				}
			}
			break;
			// ���ʌ�����
		case 1:
			// �r�ڋ߃A���[��
			for (int jj = 0; jj<NUM_MEN; jj++)
			{
				for (int kk = 0; kk < LAMP_COL_NUM; kk++)
				{
					// �d�r
					if (em_RAMP_STAT_ON == pInf->alarm[jj][kk][em_RAMP_JYU])	data.a_mode[ii][jj][kk] = 2;
					// �y�r
					else if (em_RAMP_STAT_ON == pInf->alarm[jj][kk][em_RAMP_KEI])data.a_mode[ii][jj][kk] = 1;
				}
			}
			break;
		}
	}

	//================================================
	//// ���M�˗�
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7) ) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �A���[����񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�A���[�����]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}



//==========================================
// �r�ꗗ�\�����폜�v����\���@�\��
//------------------------------------------
// char const* cKizukenNo �Ǘ�No
// int nMen �ʔԍ�	0:�\, 1:��
// int nKizuSeq �L�����Z���Ώۂ��r�A��
//==========================================
void OpSendFunc::SendOp_PatoReset(char const* cKizukenNo, int nMen, int nKizuSeq)
{
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;				// ���M�p�o�b�t�@
	
	//// ���O�`�F�b�N
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == cKizukenNo) return;

	//================================================
	////// �\���@�\�֒ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X


	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSocnSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// �w�b�_�[��(�g��)
	memcpy(head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	head.men = nMen;											// �\���֌W����
	head.kind = TO_SO_DATA_KIND_DEFLIST_CANCEL;					// �r�ꗗ�\�r���폜�v��
	head.deflist_cancel.nKizuSeq = nKizuSeq;


	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		//// ���M�˗�
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7)) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �p�g���C�g�r���Z�b�g�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�A���[�����]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}


//==========================================
// �r������\���@�\��
//------------------------------------------
// char const* cKizukenNo �Ǘ�No
// int nMen �ʔԍ�	0:�\, 1:��
// long nLen ������ [mm]
// long nNum �r��
//==========================================
void OpSendFunc::SendOp_DetectNum(char const* cKizukenNo, int nMen, long nLen, long nNum)
{
	int					nSocnSize;							// ���M�`����
	BYTE*				pSendBuf;							// ���̈�
	BYTE*				pSend;								// ���M�p�o�b�t�@

	//// ���O�`�F�b�N
	for (int ii = 0;ii<HY_SOCK_NUM; ii++) {
		if (NULL == mque_pOpSend[ii]) return;
	}
	if (NULL == cKizukenNo) return;

	//================================================
	////// �\���@�\�֒ʒm
	//// ���M�̈�m��
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];	// �w�b�_���G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSocnSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// �w�b�_�[��(�g��)
	memcpy(head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);	// �Ǘ�No
	head.men = nMen;										// �\���敪
	head.kind = TO_SO_DATA_KIND_DETECT_NUM;					// �r����� (�ŏ���ԓ�)

	// �w�b�_�[��(�g��)
	head.data_defnum.len = nLen;							// ������ [mm]
	head.data_defnum.num = nNum;							// �r����(�ŏ���ԓ�)

	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		//// ���M�˗�
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7)) {		// �o�^
			LOG(em_ERR), "[%s] <%s> �����˕\���� �p�g���C�g�r���Z�b�g�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[�A���[�����]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �r���

//------------------------------------------
// �\���@�\�ɍŏ���Ԃ̍s����𑗐M
// int men	0:�\, 1:��
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
// long y ���M�s�ʒu
// TYP_COLROW_BUF BufDefCol[] ��\�r���
// TYP_COLROW_BUF BufDefAry[][MAP_NEST_NUM] ���X�g�r���
// int nDefCnt ���݂̍ŏ���Ԃ��r��
//------------------------------------------
void OpSendFunc::SendOp_ColRowData(int men, COIL_INF const* pCoilInf, long y, TYP_COLROW_BUF BufDefCol[], TYP_COLROW_BUF BufDefAry[][MAP_NEST_NUM], int nDefCnt)
{
	int nTopDefCnt = 0;

	// �񐔕����[�v
	for (int ii = 0; ii < MAP_COL_NUM; ii++)
	{
		if (BufDefCol[ii].umu)
		{
			// ��\�r�����Z
			nTopDefCnt++;
		}
	}

	// ��\�r�����݂��Ȃ��ꍇ�����M�f�[�^����
	if (0 == nTopDefCnt) return;

	// �񐔕����[�v
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		if (!BufDefCol[ii].umu) continue;

		// ��\�r��� (�r���+�r�摜)���M
		SendOp_ColRowData(men, pCoilInf, &BufDefCol[ii]);
	}

	// �񐔕����[�v
	for (int ii = 0; ii < MAP_COL_NUM; ii++)
	{
		// ���X�g�������[�v
		for (int jj = 0; jj < MAP_NEST_NUM; jj++)
		{
			if (!BufDefAry[ii][jj].umu) continue;

			// ���X�g�r��� (�r���+�r�摜)���M
			SendOp_ColRowData(men, pCoilInf, &BufDefAry[ii][jj]);
		}
	}

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] �����˕\���� �r��񑗐M [����ʒu=%d �r��=%d]", my_sThreadName,
		y * (MAP_REPRES_MIN * 1000), nDefCnt);
#endif
	OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, men, y * (MAP_REPRES_MIN * 1000), nDefCnt);
}
//------------------------------------------
// �\���@�\�ɍŏ���Ԃ̍s����𑗐M
// int men	0:�\, 1:��
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
// TYP_COLROW_BUF* pData ���M���
//------------------------------------------
void OpSendFunc::SendOp_ColRowData(int men, COIL_INF const* pCoilInf, TYP_COLROW_BUF* pData)
{
	int					jj;
	int					nSocnSize;				// ���M�`����
	BYTE*				pSendBuf;				// ���̈�
	BYTE*				pSend;					// ���M�p�o�b�t�@
	int					nTotalLen = 0;			// �S�摜�T�C�Y
	int					nWk = 0;

	////================================================
	//// ���M�̈�m��

	// �摜���f�[�^�v�Z
	for (jj = 0; jj<MAX_CAMANGLE; jj++) nTotalLen += pData->nImgsize[jj];

	// ���M�̈�m�� (-4 �́A�{��-1�����o�E���_��������ׂ̏��u)
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) - 4 + nTotalLen;		// ���̎��_�ł́A�܂���
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head	= (TO_SO_HEAD&)pSendBuf[0];							// �w�b�_���G�C���A�X
	TO_SO_DATA_DETECT&	data	= (TO_SO_DATA_DETECT&)pSendBuf[sizeof(TO_SO_HEAD)];	// �f�[�^���G�C���A�X
	BYTE*				pJpegBuf= data.image;										// �摜�f�[�^�擪�|�C���^

	//================================================
	// �ʏ�̃f�[�^�Z�b�g
	//// �f�[�^�Z�b�g (���R�C�����؂�ւ���Ă��܂��O��̃R�C���̏��𑗐M����B�����đO�̏�񂾂���)
	// �w�b�_�[��(�x�[�X)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// �w�b�_�[��(�g��)
	memcpy(head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men	= men;
	head.kind	= TO_SO_DATA_KIND_DETECT;	// �r��� (�s����)

	// �f�[�^��(�\����)
	memcpy(&data.datas, &pData->Data, sizeof(DETECT_BASE_DATA));

	// ������[����̈ʒu[mm]
	data.datas.y = (float)pData->dLen;

	//----------------------------------------
	// �X�̉摜�f�[�^�𑗐M�̈�ɃZ�b�g
	nWk = 0;
	for (int ii = 0; ii < MAX_IMAGE_CNT; ii++)
	{
		// �r�摜�͑��݂��邩�H
		if (0 >= pData->nImgsize[ii])
		{
			data.image_len[ii] = 0;
			continue;
		}
		// �\���Ώۂ��r�摜���H
		if (!mcls_pParam->Check_ImgDsp(pData->Data.nTid, ii))
		{
			data.image_len[ii] = 0;
			continue;
		}
		memcpy(&pJpegBuf[nWk], pData->pImg[ii], pData->nImgsize[ii]);
		nWk += pData->nImgsize[ii];

		data.image_len[ii] = pData->nImgsize[ii];
	}

	//��
#ifdef DEF_DETECT_LOG
	LOG(em_MSG), "[%s] �����˕\���� ���M�s�� [%s][%s][t=%d][g=%d][%d][WidDiv=%d][y=%ldmm]", my_sThreadName, head.KizukenNo, DivNameManager::GetTorB(head.men),
		data.datas.nTid, data.datas.nGid, data.datas.nKizuNo, data.datas.nWidDiv, (long)data.datas.y);
#endif

	//================================================
	//// ���M�˗�
	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 10)) {
			delete pSend;
			LOG(em_ERR), "[%s] <%s> <%s> �����˕\���� �r��񑗐M�˗��L���[�t��", my_sThreadName, sWk.operator LPCSTR(), DivNameManager::GetTorB(men));
			syslog(SYSNO_QUEFULL_WAR, "[%s TO_SO_HEAD TO_SO_DATA_KIND_DETECT]", DivNameManager::GetTorB(men));
		}
	}
	delete pSendBuf;

	// �o�b�t�@�[�̈�N���A
	pData->umu = false;
	for (jj = 0; jj < MAX_CAMANGLE; jj++)
	{
		if (0 < pData->nImgsize[jj]) {
			SAFE_DELETE_ARRAY(pData->pImg[jj]);
		}
	}
}
