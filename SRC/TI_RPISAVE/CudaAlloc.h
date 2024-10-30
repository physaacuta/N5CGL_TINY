#pragma once

// ���[�J���̃N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "..\..\LibPG\lac\LacManager.h"									// ���x���k�N���X


using namespace KizuLib;

class CudaAlloc : public IportLib::IAlloc
{
//// ���J�ϐ�
public:
//// �����o�[�ϐ�
private :
	//=========================================
	// �C���X�^���X
	LacManager				*mcls_pLac;				// ���x���k�N���X
	bool					bAlloc;					// true:GPU��LAC �������m��

//// ���J�֐�
public:
	CudaAlloc(LacManager *lac) : mcls_pLac(lac)
	{
		mcls_pLac->Load(mcls_pLac->EM_DLL_TYPE::DLL_TYPE_LAC5);
	}


	// GPU��LAC �������m��
	virtual void *Alloc(int nSize) override
	{
		//return (BYTE*)_aligned_malloc(nSize, 32);

		bAlloc = true;
		BYTE* pMem = (BYTE*)mcls_pLac->GpuMallocHost(nSize);
		if (NULL == pMem) {
			bAlloc = false;
			pMem = (BYTE*)_aligned_malloc(nSize, 32);
		}
		return pMem;
		//return mcls_pLac->GpuMallocHost(nSize);
	}

	// GPU��LAC �������J��
	virtual void Free(void *pMem) override
	{
		//return _aligned_free(pMem);

		if(bAlloc) mcls_pLac->GpuFreeHost(pMem);
		else _aligned_free(pMem);
		return;
		//return mcls_pLac->GpuFreeHost(pMem);
		
	}
};

