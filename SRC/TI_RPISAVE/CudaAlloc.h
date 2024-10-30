#pragma once

// ローカルのクラス
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "..\..\LibPG\lac\LacManager.h"									// 高度圧縮クラス


using namespace KizuLib;

class CudaAlloc : public IportLib::IAlloc
{
//// 公開変数
public:
//// メンバー変数
private :
	//=========================================
	// インスタンス
	LacManager				*mcls_pLac;				// 高度圧縮クラス
	bool					bAlloc;					// true:GPU版LAC メモリ確保

//// 公開関数
public:
	CudaAlloc(LacManager *lac) : mcls_pLac(lac)
	{
		mcls_pLac->Load(mcls_pLac->EM_DLL_TYPE::DLL_TYPE_LAC5);
	}


	// GPU版LAC メモリ確保
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

	// GPU版LAC メモリ開放
	virtual void Free(void *pMem) override
	{
		//return _aligned_free(pMem);

		if(bAlloc) mcls_pLac->GpuFreeHost(pMem);
		else _aligned_free(pMem);
		return;
		//return mcls_pLac->GpuFreeHost(pMem);
		
	}
};

