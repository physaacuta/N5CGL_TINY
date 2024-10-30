#include "StdAfx.h"
#include "GrabberManager.h"


using namespace IportLib;

//------------------------------------------
// コンストラクタ
// EM_GRAB_TYPE emType	継承先で実装するGrabber種類
// CString sMyManagerName 自クラスの名称
//------------------------------------------
GrabberManager::GrabberManager(EM_GRAB_TYPE emType, CString sMyManagerName) :
// 重要
ThreadManager(sMyManagerName),
my_emGrabType(emType),
mcls_LogMe(sMyManagerName, NULL, 9),

// メンバー
myque_pFrame(NULL),
myque_Info(64),

my_emGrabMode(GRAB_MODE_HW),

my_bInit(false),
my_bGrabRun(false)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
GrabberManager::~GrabberManager(void)
{
}
