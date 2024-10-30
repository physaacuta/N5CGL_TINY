#include "StdAfx.h"
#include "IportBase2p.h"

//#define TTT	// �ǂ��������new�����烁�������[�N����

#ifdef TTT
 #include <new>  // �z��̈����t���R���X�g���N�^�����������邽��
#else
// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#endif
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//------------------------------------------
// �R���X�g���N�^
// CString sThreadName ���N���X�̖��O
//------------------------------------------
IportBase2p::IportBase2p(CString sThreadName)
{	
#ifdef TTT
	// �����t���R���X�g���N�^�̏�����
	new( mycls_Iport + 0) IportBase(sThreadName+"M");		// �z���0�Ԗڂ́A�}�X�^�[
	new( mycls_Iport + 1) IportBase(sThreadName+"S");		// �z���1�Ԗڂ́A�X���[�u
#endif
	for(int ii=0; ii<2; ii++) my_lock_err[ii] = NoErr;
	my_nSkipPort = 0;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportBase2p::~IportBase2p(void)
{
	Dispose();	// �O�̂��߉��

}

//------------------------------------------
// �֐�������
//------------------------------------------
bool IportBase2p::Init()
{
	for(int ii=0; ii<2; ii++) {
		if( ! mycls_Iport[ii].Init() ) return false;
	}
	my_nSkipPort = 0;
	return true;
}

//------------------------------------------
// �֐��J��
//------------------------------------------
void IportBase2p::Dispose()
{
	for(int ii=0; ii<2; ii++) mycls_Iport[ii].Dispose();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort ����n

//------------------------------------------
// iPort�ݒ�XML�t�@�C�����g�p���āA�J�����ɐڑ� (iPort2���p)
// const char* filename		XML�t���p�X (�}�X�^�[)
// const char* mac_addr1	MAC�A�h���X (�}�X�^�[)
// const char* ip_addr1		IP�A�h���X (�}�X�^�[)
// long nAdapterIndex1		�A�_�v�^�[ID (�}�X�^�[)
// char const* mc_addr1		�}���`�L���X�g�A�h���X (�}�X�^�[)
// const char* mac_addr2	MAC�A�h���X (�X���[�u)
// const char* ip_addr2		IP�A�h���X (�X���[�u)
// long nAdapterIndex2		�A�_�v�^�[ID (�X���[�u)
// char const* mc_addr2		�}���`�L���X�g�A�h���X (�X���[�u)
// bool bIsSlave			SLAVE���[�h�Őڑ� (true:XML�Ń}�X�^�[�w�莞�ł��A�����I��SLAVE�ɂȂ�Bfalse:XML�̎w��ʂ�)
// long nAdapterIndex		iPort�p�f�o�C�X�}�l�[�W���[�̉����ڂ��g�p���邩 (0�I���W��) (-1:XML�ɋL�q����Ă���A�_�v�^�[���g�p)
// int nGrabQueSize			�O���o�[�̃����O�o�b�t�@�T�C�Y
// IportLib::ERRCODE ���A���
//------------------------------------------
// ���ꂼ���XML�ݒ�L��B�ڑ��́A�}�X�^�[�A�X���[�u�Ƃ�XML
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename1, const char* filename2,
												long nAdapterIndex1, long nAdapterIndex2, 
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;
	const char* filename[]		= { filename1, filename2 };
	const long  nAdapterIndex[] = { nAdapterIndex1, nAdapterIndex2};

	for(int ii=0; ii<2; ii++) {
		// �ڑ�
		err = mycls_Iport[ii].SetupFromXMLFile(filename[ii], bSaveIpAddr, bIsSlave, nAdapterIndex[ii], nGrabQueSize );
		if(NoErr != err) return err;
	}

	// �قȂ�XML�t�@�C�����g�p���邽�߁A�ǂ�Ȑݒ�ł�OK�̂͂��B������m�[�`�F�b�N
	return NoErr;
}
// �}�X�^�[��XML�ݒ�g���B�ڑ��́A�}�X�^�[�A�X���[�u�Ƃ��O������w��
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename,
												const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
												const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;
	const char* mac_addr[]		= { mac_addr1, mac_addr2 };
	const char* ip_addr[]		= { ip_addr1, ip_addr2 };
	const long  nAdapterIndex[] = { nAdapterIndex1, nAdapterIndex2};
	const char* mc_addr[]		= { mc_addr1, mc_addr2 };

	for(int ii=0; ii<2; ii++) {
		// �����`�F�b�N
		if( NULL == mac_addr[ii] || NULL == ip_addr[ii] || 0 > nAdapterIndex[ii] ) {
			RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
		}

		// �ڑ�
		err = mycls_Iport[ii].SetupFromXMLFile(filename, mac_addr[ii], ip_addr[ii], mc_addr[ii], bSaveIpAddr, bIsSlave, nAdapterIndex[ii], nGrabQueSize );
		if(NoErr != err) return err;
	}

	// �摜��`�`�F�b�N
	if( mycls_Iport[0].GetFrameHeight()		!= mycls_Iport[1].GetFrameHeight() ||
		mycls_Iport[0].GetFrameNChannel()	!= mycls_Iport[1].GetFrameNChannel() ||
		mycls_Iport[0].GetFramePixelNBit()	!= mycls_Iport[1].GetFramePixelNBit() ) {
		RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
	}
	
	return NoErr;
}

// �}�X�^�[��XML�ݒ�g���B�ڑ��́A�}�X�^�[��XML�A�X���[�u�͊O������w��
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename, long nAdapterIndex1,
											    const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;

	// �}�X�^�[
	err = mycls_Iport[0].SetupFromXMLFile(filename, bSaveIpAddr, bIsSlave, nAdapterIndex1, nGrabQueSize );
	if(NoErr != err) return err;
	// �X���[�u
	err = mycls_Iport[1].SetupFromXMLFile(filename, mac_addr2, ip_addr2, mc_addr2, bSaveIpAddr, bIsSlave, nAdapterIndex2, nGrabQueSize );
	if(NoErr != err) return err;

	// �摜��`�`�F�b�N
	if( mycls_Iport[0].GetFrameHeight()		!= mycls_Iport[1].GetFrameHeight() ||
		mycls_Iport[0].GetFrameNChannel()	!= mycls_Iport[1].GetFrameNChannel() ||
		mycls_Iport[0].GetFramePixelNBit()	!= mycls_Iport[1].GetFramePixelNBit() ) {
		RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
	}
	
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (5) grab�̐���
//------------------------------------------
// �J�����捞�J�n
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase2p::StartGrab()
{
	IportLib::ERRCODE err		= NoErr;

	// �}�X�^�[
	err = mycls_Iport[0].StartGrab();
	if(NoErr != err) return err;

	// �X���[�u
	err = mycls_Iport[1].StartGrab();
	if(NoErr != err) {
		mycls_Iport[0].StopGrab();	// ���łɃ}�X�^�[���͊J�n���Ă��邽�߁A��~������
		return err;
	}

	return NoErr;
}

//------------------------------------------
// �J�����捞��~
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase2p::StopGrab()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].StopGrab();
	IportLib::ERRCODE err1 = mycls_Iport[1].StopGrab();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (6) �f�o�C�X�̐ؒf
//------------------------------------------
// �f�o�C�X�̐ؒf
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase2p::Disconnect()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].Disconnect();
	IportLib::ERRCODE err1 = mycls_Iport[1].Disconnect();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (7) grab�����摜�̎�舵��
//------------------------------------------
// �摜�ǂݍ���(�摜��荞�݊����܂Ńu���b�L���O)
// void*& p �摜�i�[�|�C���^
// unsigned long& len �擾�摜�T�C�Y
// unsigned long& iid iPort�V�[�P���XNo (0�I���W��)
// unsigned long& status �X�e�[�^�X
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase2p::LockFrame(void*& p0, unsigned long& len0, unsigned long& iid0, unsigned long& stat0,
										 void*& p1, unsigned long& len1, unsigned long& iid1, unsigned long& stat1)
{
    void**            p[] = {&p0,    &p1};
    unsigned long*  len[] = {&len0,  &len1};
    unsigned long*  iid[] = {&iid0,  &iid1};
    unsigned long* stat[] = {&stat0, &stat1};

	// �Y���␳�΍� ----------------------->> 
	if( 0 != my_nSkipPort ) {
		int nIndex;
		int nCnt;
		// 1�J��������iPort�V�[�P���XNo�Y���␳�p (0:���� 1:�}�X�^�[��1��X�L�b�v -1:�X���[�u��1��X�L�b�v)
		if( 0 < my_nSkipPort) {
			nIndex = 0;
			nCnt = my_nSkipPort;
		} else {
			nIndex = 1;
			nCnt = my_nSkipPort*-1;
		}
		// �w��񐔕���荞�ނ��ƂŁA�}�X�^�[�X���[�u�𓯂��ɂ���
		for(int ii=0; ii<nCnt; ii++) {
			if(NoErr == mycls_Iport[nIndex].LockFrame(*p[nIndex], *len[nIndex], *iid[nIndex], *stat[nIndex]) ) {
				mycls_Iport[nIndex].ReleaseFrame();	
			}
		}
		my_nSkipPort = 0;
	}
	// <<---------------------------------------


	// ��荞��
	for(int ii=0; ii<2; ii++) {
		my_lock_err[ii] = mycls_Iport[ii].LockFrame(*p[ii], *len[ii], *iid[ii], *stat[ii]);
	}

	// �G���[�`�F�b�N
	if(NoErr != my_lock_err[0]) {
		if(NoErr == my_lock_err[1]) mycls_Iport[1].ReleaseFrame();
		return my_lock_err[0];

	} else if(NoErr != my_lock_err[1]) {
		mycls_Iport[0].ReleaseFrame();
		return my_lock_err[1];
	}

	return NoErr;
}

//------------------------------------------
// �摜�ǂݍ��ݗ̈�ԋp
// IportLib::ERRCODE ���A���
//------------------------------------------
void IportBase2p::ReleaseFrame() const
{
	for(int ii=0; ii<2; ii++) {
		mycls_Iport[ii].ReleaseFrame();
	}
}
//------------------------------------------
// LockForRead���狭���I�ɔ�����
// IportLib::ERRCODE ���A���
//------------------------------------------
void IportBase2p::AbortLock() const
{
	for(int ii=0; ii<2; ii++) {
		mycls_Iport[ii].AbortLock();
	}
}
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (9) Reset
//------------------------------------------
// �f�o�C�X�̐ؒf
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase2p::ResetModule()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].ResetModule();
	IportLib::ERRCODE err1 = mycls_Iport[1].ResetModule();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}