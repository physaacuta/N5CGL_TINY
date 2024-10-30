#include "StdAfx.h"
#include "RpiFileManager.h"

#include "../../libPG/libRpi/RpiWriter.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// char const* cThreadName ���X���b�h����
//------------------------------------------
RpiFileManager::RpiFileManager(char const* cThreadName) :
ThreadManager(cThreadName),
my_sFullPath(""),
my_sFileName(""),
my_nWriteFrameCnt(0),
my_nSaveFolderIndex(0)
{
	//// �����o�[������
	memset(&my_Setting, 0x00, sizeof(my_Setting));

	my_TimeWrite[0] = 0.0;
	my_TimeWrite[1] = 9999.9;
	my_TimeWrite[2] = 0.0;

	this->my_pWriter = new TRpiWriter();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
RpiFileManager::~RpiFileManager(void)
{
	delete my_pWriter;
}

bool				RpiFileManager::IsOpen() const
{
	return my_pWriter->IsOpen();
}

RpiLib::RpiHeader const*	RpiFileManager::GetRpiHeader() const{
	return &my_pWriter->Header;
}

//------------------------------------------
// HDD�̃Z�N�^�T�C�Y���擾����
// char const* cDrivePath �����������h���C�u�p�X�B�t�H���_�p�X�ł�OK
// �߂�l �F �Z�N�^�T�C�Y
//------------------------------------------
int RpiFileManager::GetSecSize(char const* cDrivePath)
{
	DWORD	n1;		// �N���X�^������̃Z�N�^��
	DWORD	n2;		// �Z�N�^������̃o�C�g��
	DWORD	n3;		// �󂫃N���X�^��
	DWORD	n4;		// �S�N���X�^��

//	GetDiskFreeSpace(cDrivePath, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
	if( GetDiskFreeSpace(cDrivePath, &n1, &n2, &n3, &n4) ) {
		return n2;
	} else {	
		return 512;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �P�R�C���P�ʂ̏��߂Ẵt�@�C���̐���
//------------------------------------------
bool RpiFileManager::RpiOpen()
{
	CString			sWk;
	RpiHeader	h;

	//================================================
	// �O����
	// �I�[�v���`�F�b�N
	if( IsOpen() ) return false;

	// �w�b�_�[����
	CreateHeader(h);

	// �t���p�X����
	OnSetFileName(h, my_Setting, my_CoilInfo, &sWk);	// �t�@�C�����̌���


	//================================================
	// �t�@�C���I�[�v��
	if( ! Open(sWk, &h)) {
		RpiClose();	
		return false;
	}
	LOG(em_MSG), "[%s] RPI�t�@�C���I�[�v������(%s)", my_sThreadName, sWk);

	//================================================
	// ����̏���

	my_TimeWrite[0] = 0.0;
	my_TimeWrite[1] = 9999.9;
	my_TimeWrite[2] = 0.0;
	return true;
}

//------------------------------------------
// �t�@�C���I��
//------------------------------------------
bool RpiFileManager::RpiClose()
{
	// �t�@�C���̌�n��
	if( IsOpen() ) {
		my_pWriter->CloseFile();
		LOG(em_MSG), "[%s] RPI�t�@�C���N���[�Y����(%s) =>%s ", my_sThreadName,  my_sFileName, my_pWriter->GetError());

		// RPI�t�@�C�������S�ɏo���オ�����ׁA�ʒm
		OnRpiFileSave( my_Setting.cFolderPath[my_nSaveFolderIndex], &my_sFileName);

		// ���t�H���_�ɐؑ�
		my_nSaveFolderIndex = (my_nSaveFolderIndex+1) % my_Setting.nFolderNum;

	}
	return true;
}


//------------------------------------------
// �t���[����񏑍���
// int nImgSize �摜�f�[�^���݂̂̃T�C�Y
// int nFrameSize 1�t���[�����̏����݃T�C�Y (�Z�N�^�̔{��)
// BYTE* pFrame 1�t���[����� �����݃f�[�^
//------------------------------------------
bool RpiFileManager::WriteFrame(int nImgSize, int nFrameSize, BYTE* pFrame)
{
	//// �P�R�C�����t�@�C������

	T_RPI_HEADER_BASE &tH = my_pWriter->theader;
	int nframe = my_pWriter->Header.nframe;


	if( 0 != tH.nFileNum) {
		if( (1==tH.nFileNum && 0!=my_Setting.nFastFrame && nframe >= my_Setting.nFastFrame ) ||
			(1!=tH.nFileNum && 0!=my_Setting.nCycleFrame && nframe >= my_Setting.nCycleFrame ) ) {

			// ���t�@�C��
			Next();
		}
	}

	//// ������
	mycls_Time.PutTimestamp(0);
	my_pWriter->WriteFrameOpt( pFrame, nImgSize, NULL );
	mycls_Time.PutTimestamp(1);

	//// �����ݎ��Ԍv��
	double dWk = mycls_Time.GetTimestamp(1,0);
	dWk = ((double)nFrameSize/1024.0/1024.0 ) / (dWk/1000.0);		// [MByte/s]
	my_TimeWrite[0] = my_TimeWrite[0]*0.8 + dWk*0.2;
	my_TimeWrite[1] = __min(my_TimeWrite[1], dWk);
	my_TimeWrite[2] = __max(my_TimeWrite[2], dWk);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������

//------------------------------------------
// �w�b�_�[��񐶐�
// RpiHeader& h �����G���A
//------------------------------------------
void RpiFileManager::CreateHeader(RpiHeader& h)
{
	T_RPI_HEADER_BASE &tH = h.theader;

	h.codec			= my_Setting.codec;
	h.width			= my_Setting.width;
	h.height			= my_Setting.height;
	h.nchannel			= 1;						// ����O���[�摜�̂ݑΉ�
	h.pixel_nbit		= 8;						// 1��f8bit�̂ݑΉ�
	h.pitch_nbyte		= my_Setting.width;			// ���Ɠ���摜�̂ݑΉ�
	h.nframe			= 0;						// ��ōX�V
	h.reso_x			= my_Setting.reso_x;
	h.reso_y			= my_CoilInfo.reso_y;		

	if (0 >= h.reso_y) {
		LOG(em_ERR), "[%s] �c����\���ݒ�B�o�b�N�A�b�v�ɂ��ݒ�ύX(%f��%f)", my_sThreadName, my_CoilInfo.reso_y, h.reso_x);
		h.reso_y = h.reso_x;		// �o�b�N�A�b�v���u
	}
		LOG(em_INF), "[%s] �c����\���ݒ�B�o�b�N�A�b�v�ɂ��ݒ�ύX(%f��%f)", my_sThreadName, h.reso_y, h.reso_x);


	// TEX��
	memcpy(tH.cSysName, my_Setting.cSysName, sizeof(tH.cSysName));
	memcpy(tH.cKizukenNo, my_CoilInfo.cKizukenNo, sizeof(tH.cKizukenNo));
	tH.frame_info_type	= my_Setting.frame_info_type;
	tH.frame_info_size	= my_Setting.frame_info_size;
	tH.nFrameNumInit	= my_CoilInfo.nCFNo;
	tH.nFrameNumStart	= my_CoilInfo.nCFNo;
	tH.nFileNum			= 0==my_Setting.nFastFrame ? 0 : 1;
	tH.nLineCenter		= my_Setting.nLineCenter;
	tH.nStartCamset		= my_Setting.nStartCamset;
	tH.nCamsetNum		= my_Setting.nCamsetNum;
	tH.nCamAngle		= my_Setting.nCamAngle;
	tH.nMen				= my_Setting.nMen;

	for(int ii=0; ii<my_Setting.nCamsetNum; ii++) {
		tH.caminfo[ii].dBaseXoffset	= my_Setting.caminfo[ii].dBaseXoffset;
		tH.caminfo[ii].nBaseLeft	= my_Setting.caminfo[ii].nBaseLeft; 
		tH.caminfo[ii].nOverLap		= my_Setting.caminfo[ii].nOverLap;
	}
	tH.nStartLen			= my_CoilInfo.nStartLen;
	tH.nColorType			= my_Setting.nColorType;
	tH.nSysType				= my_Setting.nSysType;

	tH.nFTposCfNo[0]		= my_CoilInfo.nFTposCfNo[0];
	tH.nFTposPixel[0]		= my_CoilInfo.nFTposPixel[0];
	tH.nFTposCfNo[1]		= 0;		// ���[�͌�ōX�V
	tH.nFTposPixel[1]		= 0;
}

//------------------------------------------
// �w�b�_�[���X�V(�R�C���m�莞)
// RpiHeader& h �����G���A
//------------------------------------------
void RpiFileManager::CreateHeader_Ending(RpiHeader& h)
{
	h.theader.nFTposCfNo[1]		= my_CoilInfo.nFTposCfNo[1];
	h.theader.nFTposPixel[1]	= my_CoilInfo.nFTposPixel[1];
	if (h.reso_y != my_CoilInfo.reso_y) {
		LOG(em_INF), "[%s] �R�C���m�莞�ɏc����\�X�V (%f��%f)", my_sThreadName, h.reso_y, my_CoilInfo.reso_y);
		h.reso_y = my_CoilInfo.reso_y;
	}

}

//------------------------------------------
// �t�@�C���̐���
// CString sName �t�@�C����
// RpiHeader* h �w�b�_�[���
//------------------------------------------
bool RpiFileManager::Open(CString sName, RpiHeader* h) {

	//// �Z�N�^�P�ʂ̓���������
	// FILE_FLAG_NO_BUFFERING �ɂ��Z�N�^�P�ʂ̏����݁B 512�P�ʂƂ��ɂȂ�̂ŁA�t�@�C���T�C�Y���傫���Ȃ邪�A
	// ���̕���ԍ����Ǝv����B
	
	// �ǂ����ŋ߂̃L���b�V�����ڂ�HDD,SSD���ƁAFILE_FLAG_NO_BUFFERING��
	// �L���b�V�������ő��������݂Ƃ�����ƁA�����ɓǂݍ��ގ��Ȃǂ�
	// ���b�N����Ă��܂��悤�Ȋ����ƂȂ�B

	// ���ʂƂ��āA������������ԑ��������E�E�E �����������������


	CString sFPath;
	sFPath.Format("%s\\%s", my_Setting.cFolderPath[my_nSaveFolderIndex], sName);

	if ( !my_pWriter->OpenFile( sFPath, *h, 1 == my_Setting.nWriteMode) ){
		LOG(em_ERR), "[%s] RPI�t�@�C���������s=%s", my_sThreadName, sFPath);
		return false;
	}

	//// ���[�J���ϐ� ������
	//my_sFullPath		= sFPath;
	CStringCS::LockCpy(&my_sFullPath, sFPath);		// Dlg��CString���Q�Ƃ��邽�߁A�r������������
	my_sFileName		= sName;
	return true;
}
//------------------------------------------
// 1�R�C�����ł̃t�@�C���؂�ւ�
//------------------------------------------
bool RpiFileManager::Next()
{

	//================================================
	// �O����
	
	// ���̃t�@�C�������
	RpiClose();

	// ���t�@�C���̂��߂ɁA�w�b�_�[���X�V����
	RpiHeader	h = my_pWriter->Header;
	h.nframe = 0;
	h.theader.nFileNum ++;
	h.theader.nFrameNumStart = my_FrameInfo.nFNo; 

	// �t�@�C��������
	CString			sWk;
	OnSetFileName(h, my_Setting, my_CoilInfo, &sWk);		// �t�@�C�����̌���
	if( 0 >= sWk.GetLength() ) return false;

	//================================================
	// �t�@�C���I�[�v��
	if( ! Open(sWk, &h)) {
		RpiClose();	
		return false;
	}

	return true;
}