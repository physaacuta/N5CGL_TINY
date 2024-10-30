// SockBase.cpp: SockBase �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>

#include "SockBase.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// �R���X�g���N�^
// const char *cSession ini�t�@�C�� �Z�b�V����
// const char *cIniPath ini�t�@�C�� �p�X
// DWORD tid PostThreadMessage�̑���� (ThreadID��n���Ηǂ�)
// DWORD timeout �^�C���A�E�g�Ԋu [ms]
// DWORD client_num �ڑ������ (�N���C�A���g��1�Œ�A�T�[�o�[���ڑ��ɗ���l�������w��)
// DWORD r_size �\�P�b�g�̎�M�o�b�t�@�T�C�Y
// DWORD s_size �\�P�b�g�̑��M�o�b�t�@�T�C�Y
// DWORD msgbase ���b�Z�[�W�x�[�X�ԍ�
//------------------------------------------
SockBase::SockBase(const char *cSession, const char *cIniPath, DWORD tid, DWORD timeout, DWORD client_num,
				   DWORD r_size, DWORD s_size, DWORD msgbase) :
my_nClientNum(client_num),
my_nNewAcceptNgMode(0),
mycls_pKc(NULL)
{
	// �����`�F�b�N
	_ASSERT(client_num > 0 && client_num <= KS_MAXCLIENTS);
	_ASSERT(0 != strlen(cSession) && 0 != strlen(cIniPath));
	_ASSERT(0 != tid);

	// �܂��͑S��������
	for( int ii=0; ii<KS_MAXCLIENTS; ii++ ) {
		mycls_pKt[ii] = NULL;
		my_nClientAddr[ii] = NULL;
	}

	// INI�t�@�C�����IP,�|�[�g�擾
	char cIp[20];
	long iPort;
	GetPrivateProfileString(cSession, "SAVE_IP", "", cIp, sizeof(cIp), cIniPath);
	iPort = GetPrivateProfileInt(cSession, "SAVE_PORT", 0, cIniPath);
	if( 0 == strlen(cIp) || 0 == iPort ) _ASSERT(0);

	// ksControl�C���X�^���X����
	mycls_pKc = new ksControl(tid, msgbase);

	//// �\�P�b�g����
	// 0�������� �f�t�H���g�T�C�Y���Z�b�g
	if(!r_size) r_size = KS_RCVBUF;
	if(!s_size) s_size = KS_SNDBUF;
	if (mycls_pKc->SockOpen(r_size, s_size)) {
		delete mycls_pKc;
		mycls_pKc = NULL;
		throw 1;
	}

	// IP,�|�[�g ����N���X�Ɋi�[ (���ۂɎg�p�����̂́AConnect,Listen��������)
	mycls_pKc->SetAddr(cIp, (short)iPort);

	// �^�C���A�E�g�̐ݒ�
	if( timeout > 0 ) mycls_pKc->SetTimeout(timeout);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockBase::~SockBase()
{
	// �\�P�b�g�j��
	if(NULL != mycls_pKc) {
		delete mycls_pKc;
		mycls_pKc = NULL;
		WSACleanup();
	}
	// �N���C�A���g�p�\�P�b�g�|�C���^�𖢊��蓖�� (delete �� ksControl�̒��ł���Ă���Ă���)
	for( int ii=0; ii<KS_MAXCLIENTS; ii++ ) {
		mycls_pKt[ii] = NULL;
	}
}

//------------------------------------------
// �w�肳�ꂽ�|�C���^�ƈ�v����z��ʒu��Ԃ�
// ksThread* ksT ksThread�̃|�C���^
// �߂�l �z��ʒu
//------------------------------------------
int SockBase::GetSockID(WPARAM ksT) const	// UINT��64bitAP�̏ꍇNG
{
	return GetSockID((ksThread *)ksT);
}
int SockBase::GetSockID(ksThread* ksT) const
{
	// Active�Ȃ��̂̒���T��
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( mycls_pKt[ii] == ksT ) return ii;
	}
	return -1;
}
//------------------------------------------
// �����蓖�Ă̔z��ʒu��Ԃ�
// �߂�l �z��ʒu
//------------------------------------------
int SockBase::GetFreeID() const
{
	// Active�Ȃ��̂̒�����V�K�ɐU��
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL == mycls_pKt[ii] ) return ii;
	}
	return -1;
}
//------------------------------------------
// �������Ă���q�X���b�h�̐���Ԃ�
// �߂�l �����Ă��鐔
//------------------------------------------
int SockBase::GetActive() const
{
	int num = 0;
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL != mycls_pKt[ii] ) num++;
	}
	return num;
}
//------------------------------------------
// �\�P�b�g�̏I���敪��Ԃ�
// int id �z��A�h���X
// �߂�l �I���敪
//------------------------------------------
int SockBase::GetCloseKindID(int id) const
{
	if( NULL == mycls_pKt[id]) {
		return 0;
	} else {
		return mycls_pKt[id]->GetCloseKind();
	}
}

//------------------------------------------
// �R�l�N�g (�ڑ������܂Ńu���b�N�����)
//  �N���C�A���g���̐ڑ��֐�
//------------------------------------------
int SockBase::Connect()
{
	// �R�l�N�g���́A0�ԖڂɌŒ�
	if (NULL != mycls_pKt[0]) return -9;
	return mycls_pKc->Connect(&mycls_pKt[0]);
}

//------------------------------------------
// ���X�j���O (�m���u���b�N)
//  �T�[�o�[���̐ڑ��֐�
//------------------------------------------
int SockBase::Listen()
{
	return mycls_pKc->Listen(my_nClientNum);
}

//------------------------------------------
// �A�N�Z�v�g (�A�N�Z�v�g�ʒm�������ꍇ�ɖ{�֐����R�[������) (�V�O�i���p)
//------------------------------------------
int SockBase::Accept()
{
	// �[���I�Ƀ��b�Z�[�W���󂯎�������ɂ���
	MSG msg;
	mycls_pKc->GetAcceptEventMsg(&msg);
	return Accept(&msg);
}
//------------------------------------------
// �A�N�Z�v�g (�A�N�Z�v�g�ʒm�������ꍇ�ɖ{�֐����R�[������) (���b�Z�[�W���p)
// MSG *msg �A�N�Z�v�g���ꂽ�Ƃ��̃|�C���^
// �߂�l �Z�b�g�����z��ʒu��Ԃ�(�N���C�A���gID�̎�)
//------------------------------------------
int SockBase::Accept(MSG *msg)
{
	// �����蓖�Ă̔z��ʒu���擾
	int id = GetFreeID();

	//// �����蓖�Ė��� ���� ���ɓo�^�ς݁H�̃C���X�^���X�Ȃ玸�s�Ƃ���
	if( -1 != id && -1 == GetSockID((ksThread *)msg->wParam) ) {
		my_nClientAddr[id] = (long)msg->lParam;
		

	} else {
		// �ُ�擾
		//
		//  �� OS�_�E�����āA�����ċN�������ꍇ�ɂ́A
		// �\�P�b�g���N���[�Y����Ȃ��̂ŁA�N���C�A���g�I�[�o�[�ƂȂ�
		// ���̂悤�ȏꍇ�ɂ�lParam�����āA����IP�ł���ΑO�̃\�P�b�g�����
		
		//// �Ȃ����Ă�����̂œ���IP�������̂�T��
		DWORD ii;
		for( ii=0; ii<my_nClientNum; ii++ ) {
			if( NULL != mycls_pKt[ii] && my_nClientAddr[ii] == msg->lParam ) break;
		}

		//if( ii >= 0 && ii < my_nClientNum ) {		// �����A�h���X�̃\�P�b�g����������
		if( 0==my_nNewAcceptNgMode &&
			ii >= 0 && ii < my_nClientNum ) {		// �����A�h���X�̃\�P�b�g����������
			// �O�̃\�P�b�g�����
			if( 0 != mycls_pKc->CloseChild( mycls_pKt[ii] ) ) _ASSERT(0);
			id = ii;	// �J�������z��ʒu�ɂ���

		} else {									// ���m�̃N���C�A���g����
			if( 0 != mycls_pKc->CloseChild( (ksThread *)msg->wParam ) ) _ASSERT(0);
			return -1;
		}
	}

	// �N���C�A���gID,�\�P�b�g�̏�Ԍ���
	mycls_pKt[id] = (ksThread *)msg->wParam;
	return id;
}

//------------------------------------------
// ����̃N���C�A���g��؂藣�� (�|�C���^�w��)
// ksThread* ksT �ؒf������ ksThread �̃|�C���^
//------------------------------------------
void SockBase::Close(WPARAM ksT) 
{
	Close((ksThread *)ksT);
}
void SockBase::Close(ksThread* ksT) 
{
	int id = GetSockID(ksT);
	if( -1 != id ) {
		mycls_pKc->CloseChild( ksT );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
}

//------------------------------------------
// ����̃N���C�A���g��؂藣�� (�z��ʒu�w��)
// int id �z��A�h���X
//------------------------------------------
void SockBase::CloseID(int id)
{
	if( NULL == mycls_pKt[id]) return;

	if( -1 != id ) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
}

//------------------------------------------
// �S�ẴN���C�A���g��؂藣��
//------------------------------------------
void SockBase::CloseAll()
{
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) CloseID(ii);
}

//------------------------------------------
// ���݂̓ǂݍ��݃L�����Z������֐�
//------------------------------------------
void SockBase::Cancel()
{
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL == mycls_pKt[ii] ) continue;
		mycls_pKt[ii]->Cancel();
	}
}

//------------------------------------------
// ��M�J�n�˗� (�|�C���^�w��)
// LPVOID pData ��M�o�b�t�@
// DWORD len ��M�T�C�Y
// ksThread* ksT ksThread �̃|�C���^
// HANDLE evRecvEnd ��M�����C�x���g (NULL���̓��b�Z�[�W�œn�����)
//------------------------------------------
int SockBase::Recv(LPVOID pData, DWORD len, UINT ksT, HANDLE evRecvEnd)
{
	return Recv(pData, len, (ksThread*)ksT, evRecvEnd);
}
int SockBase::Recv(LPVOID pData, DWORD len, ksThread* ksT, HANDLE evRecvEnd)
{
	int id = GetSockID(ksT);
	if( -1 == id ) return -9;

	return Recv(id, pData, len, evRecvEnd);
}

//------------------------------------------
// ��M�J�n�˗� (�z��ʒu�w��)
// int id �z��ʒu (�N���C�A���g���́A0�Œ�)
// LPVOID pData ��M�o�b�t�@
// DWORD len ��M�T�C�Y
// HANDLE evRecvEnd ��M�����C�x���g (NULL���̓��b�Z�[�W�Œʒm�����)
//------------------------------------------
int SockBase::Recv(int id, LPVOID pData, DWORD len, HANDLE evRecvEnd)
{
	if( -1 == id ) return -9;				// �����蓖��
	if( NULL == mycls_pKt[id] ) return -1;	// ���͐ؒf����Ă���

	// ��M�J�n�˗�
	int iRet = mycls_pKt[id]->Recv( pData, len, evRecvEnd);
	
	// ��M�J�n�˗������s�̈� �؂藣��
	if(0 != iRet) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
	return iRet;
}

//------------------------------------------
// ���M�J�n�˗� (�|�C���^�w��)
// LPCVOID pData ���M�o�b�t�@
// DWORD size ���M�T�C�Y
// ksThread* ksT ksThread �̃|�C���^
// HANDLE evSendEnd ��M�����C�x���g (NULL���̓��b�Z�[�W�Œʒm�����)
//------------------------------------------
int SockBase::Send(LPCVOID pData, DWORD len, UINT ksT, HANDLE evSendEnd)
{
	return Send(pData, len, (ksThread*)ksT, evSendEnd);
}
int SockBase::Send(LPCVOID pData, DWORD len, ksThread* ksT, HANDLE evSendEnd)
{
	int id = GetSockID(ksT);
	if( -1 == id ) return -9;

	return Send(id, pData, len, evSendEnd);
}
//------------------------------------------
// ���M�J�n�˗� (�z��ʒu�w��)
// int id �z��ʒu (�N���C�A���g���́A0�Œ�)
// LPCVOID senddata ���M�o�b�t�@
// DWORD size ���M�T�C�Y
// HANDLE evSendEnd ���M�����C�x���g (NULL���̓��b�Z�[�W�Œʒm�����)
//------------------------------------------
int SockBase::Send(int id, LPCVOID pData, DWORD len, HANDLE evSendEnd)
{
	if( -1 == id ) return -9;				// �����蓖��
	if( NULL == mycls_pKt[id] ) return -1;	// ���͐ؒf����Ă���

	// ���M�J�n�˗�
	int iRet = mycls_pKt[id]->Send( pData, len, evSendEnd);
	
	// ���M�J�n�˗������s�̈� �؂藣��
	if(0 != iRet) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
	return iRet;
}


