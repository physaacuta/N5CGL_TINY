#include "StdAfx.h"
#include "FuncBase.h"

//------------------------------------------
// コンストラクタ
// const char* pThreadName 自スレッド名
//------------------------------------------
FuncBase::FuncBase(const char* pThreadName) :
my_sThreadName(pThreadName),
mcls_pLog(NULL)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FuncBase::~FuncBase(void)
{
}
