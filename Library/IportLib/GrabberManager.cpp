#include "StdAfx.h"
#include "GrabberManager.h"


using namespace IportLib;

//------------------------------------------
// �R���X�g���N�^
// EM_GRAB_TYPE emType	�p����Ŏ�������Grabber���
// CString sMyManagerName ���N���X�̖���
//------------------------------------------
GrabberManager::GrabberManager(EM_GRAB_TYPE emType, CString sMyManagerName) :
// �d�v
ThreadManager(sMyManagerName),
my_emGrabType(emType),
mcls_LogMe(sMyManagerName, NULL, 9),

// �����o�[
myque_pFrame(NULL),
myque_Info(64),

my_emGrabMode(GRAB_MODE_HW),

my_bInit(false),
my_bGrabRun(false)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
GrabberManager::~GrabberManager(void)
{
}
