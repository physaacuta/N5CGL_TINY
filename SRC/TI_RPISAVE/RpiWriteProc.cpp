#include "StdAfx.h"
#include "RpiWriteProc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------




//// �\�[�g�̔�r�֐�
static inline bool comp_sd_fd(const double &a, const double &b)
{
	return a < b;
}




//------------------------------------------
// �R���X�g���N�^
// int nIns �C���X�^���XNo
// int nColor �F�ԍ�
//------------------------------------------
RpiWriteProc::RpiWriteProc(int nIns, int nColor) :
RpiFileManager( GetThreadNameArray2("RpiWP", nIns, nColor) ),
m_nIns(nIns),
m_nColor(nColor),
m_bIsExec(false)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
RpiWriteProc::~RpiWriteProc(void)
{
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int RpiWriteProc::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { this->mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int RpiWriteProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void RpiWriteProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		this->ExecQue();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����
//------------------------------------------
// ������
// Tin* pIn		����Que�f�[�^
// �߂�l		NULL:�ԋp���Ȃ� �o��Que�f�[�^:���̃X���b�h�֕ԋp����ꍇ  
//------------------------------------------
FrameDataManager* RpiWriteProc::OnAddNewItem( DELIVERY_RPIWRITE* p )
{
	if( NULL == p ) {							// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return NULL;
	}

	//// ����
	FrameDataManager*	pFdAns = NULL;			// �ԋp�p
	switch (p->emNo) {
//-------------------------------------------------------------------
	case em_FRAME_INFO:							// �t���[����񏑍���
		//// ���̏����ł́A 4096*2048 * 4 �ŁA20msec���x�ŏI���B  ��������񏈗��͕K�v�Ȃ����ȁ[
		if(m_bIsExec) {
			Exec_WriteFrame( (FrameDataManager*)p->data );
		} else {
			//LOG(em_WAR), "[%s] �L�����Z��", my_sThreadName);
			// �J���Ă������x����
			if( IsOpen() ) Exec_FileClose(NULL);
		}
		//// ����
		pFdAns = (FrameDataManager*)p->data;
		break;

//-------------------------------------------------------------------
	case em_WRITE_END:							// �R�C���P�ʂ̏����݊���
		Exec_FileClose();
		break;

//-------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}

	//// ���
	delete p;

	// ����
	return pFdAns;		// NULL�̂Ƃ��͎��֍s���Ȃ��B=�s���K�v���Ȃ�����
}

//------------------------------------------
// �����ݏ�������
// FrameDataManager* p �t���[�����
//------------------------------------------
void RpiWriteProc::Exec_WriteFrame(FrameDataManager* p)
{
	// ���������ۑ��Ώۂ���Ȃ��B�ʏ햳���͂������A�O���o�[��~�����Ƃ��̎c�[�ł��邩��
	if( ! p->bRpiSave ) {	// ������
		LOG(em_WAR), "[%s] �L�����Z��", my_sThreadName);

		// �J���Ă������x����
		if( IsOpen() ) Exec_FileClose(p);
		return;
	}


	//-----------------------
	// �t�@�C���ؑ֔���
	if( IMG_SAVE_STAT_CHANGE == p->emImgSaveStat || 
		IMG_SAVE_STAT_F == p->emImgSaveStat ) {
		
		// �J���Ă������x����
		if( IsOpen() ) Exec_FileClose(p);
	}


	//-----------------------
	// �t�@�C���I�[�v���`�F�b�N
	if( ! IsOpen() ) {
		// �R�C���P�ʂ̏����Z�b�g (�R�C���擪)
		SetData_CoilInfo(p);
		// RPI�t�@�C���I�[�v��
		RpiOpen();
	}

//LOG(em_MSG), "[%s] RPI�������� (%d/%d)(%s)", my_sThreadName, p->nNo, p->nCFno, my_sFileName);

	//-----------------------
	// �t���[���P�ʂ̏��Z�b�g
	SetData_FrameInfo(p);


	//-----------------------
	//// ������
	p->cls_Time.PutTimestamp(FrameDataManager::TIME_RpiWrite_START);

	p->nSetSize = GET_SIZE_JUST( p->nSetSize, my_Setting.nSecSize);		// �Z�N�^�P�ʂɂ��킹��
	this->WriteFrame(p->nCompSize, p->nSetSize, p->pBuf );

	p->cls_Time.PutTimestamp(FrameDataManager::TIME_RpiWrite_END);

	//-----------------------
	// �t�@�C���N���[�Y
	if( IMG_SAVE_STAT_T == p->emImgSaveStat ) {
		Exec_FileClose(p);
	}
}

//------------------------------------------
// �I������
// FrameDataManager* p �t���[����� (NULL�ł�OK)
//------------------------------------------
void RpiWriteProc::Exec_FileClose(FrameDataManager* p)
{
	if( IsOpen() ) {
		if( NULL != p ) SetData_CoilResult(p);
		this->RpiClose();
		LOG(em_MSG), "[%s]<%d> RPI�t�@�C���N���[�Y����(%s)", my_sThreadName, m_nIns, my_sFileName);
	}
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void RpiWriteProc::Reset()
{

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐�

//------------------------------------------
// �t�@�C��������
// RpiHeader const& h ����̃w�b�_�[���
// CString* strName ���肵���t�@�C����
//------------------------------------------
void RpiWriteProc::OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName)
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	const T_RPI_HEADER_BASE &tH =h.theader;

	// TEX��RPI�t�@�C�������K��
	// [�r���Ǘ�No]@men=[�\��]@Angle=%d@No=[�R�C���P�ʂ̃t�@�C������No]@�ȍ~�Ȃ�ł�OK

	if( COLOR_TYPE_GRAY == prm.emColorType ) {
		// �O���[
		// [�r���Ǘ�No]@men=[�\��]@Angle=%d@No=[�R�C���P�ʂ̃t�@�C������No]@�R�C��No
		strName->Format("%s@men=%d@Angle=%d@No=%d@%s.%s", tH.cKizukenNo, tH.nMen, tH.nCamAngle, tH.nFileNum, c.cCoilNo, RPI_TYPE_WK );
	} else if( COLOR_TYPE_COLOR == prm.emColorType || COLOR_TYPE_SA == prm.emColorType ) {
		// �F��
		// [�r���Ǘ�No]@men=[�\��]@Color=%d@No=[�R�C���P�ʂ̃t�@�C������No]@�R�C��No
		strName->Format("%s@men=%d@Color=%d@No=%d@%s.%s", tH.cKizukenNo, tH.nMen, tH.nCamAngle, tH.nFileNum, c.cCoilNo, RPI_TYPE_WK );
	}
}
//------------------------------------------
// RPI�t�@�C����������
// char const* strPath �Ώۂ̃t�H���_�p�X (�Ō��\�͂Ȃ�)
// CString* strName �Ώۂ̃t�@�C����
//------------------------------------------
void RpiWriteProc::OnRpiFileSave(char const* strPath, CString* strName)
{
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];

	//// �g�����ύX
	// .rwk �� .rpi
	CString sWk = *strName;
	sWk.Replace(RPI_TYPE_WK, RPI_TYPE);

	CString sOldPath, sNewPath;
	sOldPath.Format("%s\\%s", strPath, *strName);
	sNewPath.Format("%s\\%s", strPath, sWk);
	rename(sOldPath, sNewPath);

	if(!m_bIsExec) return;	// ��~���́A���Ȃ� (�R�s�[���Ƀ^�X�N����������A����R�s�[���s�ƂȂ邽��)


	//// �ړ��p �f�[�^��肱��
	RPI_MOVE* pDeli = new RPI_MOVE;
	
	pDeli->sName	= sWk;
	pDeli->sPath_F	= strPath;
	pDeli->nIns		= m_nIns;

	//// �L���[�ɒǉ�
	if( ! mque_pRpiMove->AddToTailFreeCheck(pDeli, 1) ) {
		delete pDeli;

		LOG(em_ERR), "[%s] �L���[�t���I�I(RPI�t�@�C���ړ�)", my_sThreadName);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑�

//------------------------------------------
// �ݒ��� �Z�b�g (�����J�n��)
//------------------------------------------
bool RpiWriteProc::SetData_Setting()
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];


	//// �Z�N�^�T�C�Y���Z�o
	int		nSecSize;
	int		nWriteMode;										// �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h)
	if(0==strlen(prm.cWriteSecPath))	{
		nSecSize	= 512;									// 512�o�C�g�P�ʌŒ�
		nWriteMode	= 0;
	} else {
		nSecSize	= GetSecSize(prm.cWriteSecPath);		// �Z�N�^�P�ʌŒ�
		nWriteMode	= 1;
	}

	//////////////////////////////////////
	// �w�b�_�[��񐶐�
	//////////////////////////////////////
	 
	// ���������t�@�C������肱�ނ��ߗp --------
	my_Setting.nFastFrame		= 0;//5;				// �ŏ��̃R�C���P�ʂ̃t���[���i�[�� (0�́A1�R�C��1�t�@�C���B1�`�́A���̖����ɂȂ����玟�t�@�C����)
	my_Setting.nCycleFrame		= 0;//10;				// �Q��ڈȍ~�̃t���[���i�[�� 
	
	for(int ii=0; ii<prmI.nRpiFolderBufNum; ii++) {
		if( RpiFileManager::MAX_SAVE_FOLDER <= ii ) break;
		strcpy(my_Setting.cFolderPath[ii], prmI.cRpiFolderBuf[ii]);		// �t�H���_�p�X (�Ō��\�͖���)
		my_Setting.nFolderNum = ii+1;
	}

	my_Setting.nWriteMode		= nWriteMode;			// �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h)
	my_Setting.nSecSize			= nSecSize;				// �Z�N�^�T�C�Y (������Ȃ��l��1�ŗǂ�)


	// ��{���w�b�_�p --------
	my_Setting.width			= prmI.nUionWidth;		// �摜�� (8�̔{��)
	my_Setting.height			= prmI.nUionHeight;		// �摜���� (8�̔{��)
	my_Setting.reso_x			= prmI.reso_x;			// ������\
	my_Setting.reso_y			= prmI.reso_y; //(float)mcls_pParam->CalcResV();		// �c����\ 


	// TEX�w�b�_�p --------
	strcpy(my_Setting.cSysName, prm.cSystemName );		// �V�X�e������
	my_Setting.nLineCenter		= prmI.nLineCenter;		// ���C���Z���^�[��f(�A���摜���[����̃I�t�Z�b�g�l) [pixel]
	my_Setting.nStartCamset		= prmI.nStartCamset;	// ���[�J�����Z�b�g�ԍ� (1�I���W��)
	my_Setting.nCamsetNum		= prmI.nCamsetNum;		// �������t���[�������� (1�I���W��)
	if( COLOR_TYPE_GRAY == prm.emColorType ) {
		my_Setting.nCamAngle	= prmI.nCamAngle;		// �J�����p�x (1�I���W��)
	} else if( COLOR_TYPE_COLOR == prm.emColorType || COLOR_TYPE_SA == prm.emColorType ) {
		my_Setting.nCamAngle	= m_nColor + 1;			// �F�摜 (1�I���W��)
	}
	my_Setting.nMen				= prmI.nMen;			// �\���敪 (0:�\ 1:��)
	my_Setting.nColorType		= prm.emColorType;		// �摜�^�C�v (0:�O���[�摜  1:�J���[�摜  2:�F���摜)
	my_Setting.nSysType			= prm.emSysType;		// �r���^�C�v (0:�A�����C��  1:�o�b�`���C��)


	// �t���[���t�����
	my_Setting.frame_info_type	= prm.nAddFrameInfoType;			// �t���[����񕔂̊i�[�`�� 
	if(0 == prm.nAddFrameInfoType) {
		my_Setting.frame_info_size	= 0;
	} else if(1 == prm.nAddFrameInfoType) {
		my_Setting.frame_info_size	= sizeof(T_RPI_FRAME_INFO_T1);	// �t���[����񕔂̊i�[�T�C�Y 
	}


	//// ���̑�


	return true;
}

//------------------------------------------
// �R�C���P�ʂ̐ݒ��� �Z�b�g (��[)
// FrameDataManager* p �t���[�����
//------------------------------------------
bool RpiWriteProc::SetData_CoilInfo(FrameDataManager* p) 
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];
	ParamManager::P_PARAM_CAMSET const&	pDb  = mcls_pParam->GetParamCamSet();


	switch(p->emCompType){
	case COMP_TYPE_JPEG:	my_Setting.codec= RpiHeader::CODEC_JPEG; break;
	case COMP_TYPE_LAC:		my_Setting.codec= RpiHeader::CODEC_LAC; break;
	case COMP_TYPE_RAW:		my_Setting.codec= RpiHeader::CODEC_RAW; break;
	case COMP_TYPE_BMP:		my_Setting.codec= RpiHeader::CODEC_BMP; break;
	case COMP_TYPE_NUC:		my_Setting.codec= RpiHeader::CODEC_NUC; break;
	case COMP_TYPE_LAC5:	my_Setting.codec= RpiHeader::CODEC_LAC5; break;
	default:		my_Setting.codec= RpiHeader::CODEC_UNKNOWN; break;
	}


	//// �R�C���P�ʂŕύX���������Z�b�g
	memset(&my_CoilInfo, 0x00, sizeof(my_CoilInfo));
	my_CoilInfo.reso_y	  = my_Setting.reso_y;			// ���ۂɕ���\���������݂̂�CoilInfo�̕��I
	my_CoilInfo.nCFNo	  = p->UdpData.framenum;
	if( p->bDummyCoil ) {
		my_CoilInfo.nStartLen = false;						// �_�~�[�R�C��
	} else {
		my_CoilInfo.nStartLen = p->UdpData.pos[ prmI.nMen ].posFromHead;						// �R�C���ؑ֎��̃t���[���擪�ʒu [mm] (�����ʂ�0����Ȃ�)
	}
	memcpy(my_CoilInfo.cKizukenNo, p->cKizukenNo, SIZE_KIZUKEN_NO );
	strcpy(my_CoilInfo.cCoilNo, p->cCoilNo);

	for(int jj=0; jj<prmI.nCamsetNum; jj++) {
		my_Setting.caminfo[jj].dBaseXoffset	= (float)pDb.ins[m_nIns].camset[jj].dXoffset;	// ��ʒu�B���C���Z���^�[����̊e�J�������[�̈ʒu(�������́A�}�C�i�X�l) [mm] (PARAM_CAMSET��Xoffset�l���̂���)
		my_Setting.caminfo[jj].nBaseLeft	= prmI.camset[jj].nBaseLeft;					// ���f�B������̃t���[�����[���玩�J�����摜���[�܂ł̋���
		my_Setting.caminfo[jj].nOverLap		= prmI.camset[jj].nOverLap;						// ���J�����Ƃ̉E���̃I�[�o�[���b�v��
	}
	
	//if( 1 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}

	//if( SYS_TYPE_BAT == prm.emSysType ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}
	return true;
}

//------------------------------------------
// �R�C���P�ʂ̎��я�� �Z�b�g (���[)
// FrameDataManager* p �t���[�����
//------------------------------------------
bool RpiWriteProc::SetData_CoilResult(FrameDataManager* p) 
{
	ParamManager::P_MAIN		 const& prm	 = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ m_nIns ];
	ParamManager::P_PARAM_CAMSET const&	pDb  = mcls_pParam->GetParamCamSet();

	return true;
}

//------------------------------------------
// �t���[���P�ʂ̐ݒ��� �Z�b�g
// FrameDataManager* p �t���[�����
//------------------------------------------
bool RpiWriteProc::SetData_FrameInfo(FrameDataManager* p) 
{
	// ���t���[���Z�b�g���������Z�b�g
	my_FrameInfo.nFNo = p->UdpData.framenum;


	//if( 1 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[0]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[0] = p->nFTposPixel;
	//}
	//if( 2 == p->nFTset ) {
	//	my_CoilInfo.nFTposCfNo[1]  = p->nFTposCfNo;
	//	my_CoilInfo.nFTposPixel[1] = p->nFTposPixel;
	//}

//	p->nCFno = p->UdpData.framenum - my_CoilInfo.nCFNo; 


	return true;
}


	
