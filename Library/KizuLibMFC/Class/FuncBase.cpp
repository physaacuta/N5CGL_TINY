#include "StdAfx.h"
#include "FuncBase.h"

//------------------------------------------
// �R���X�g���N�^
// const char* pThreadName ���X���b�h��
//------------------------------------------
FuncBase::FuncBase(const char* pThreadName) :
my_sThreadName(pThreadName),
mcls_pLog(NULL)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FuncBase::~FuncBase(void)
{
}
