#pragma once

//ダミーフレーム用キュー。
// ダミーフレームがAddされるとき、実際にはAddせずDeleteしてしまう。
class CDummyQueueFree : public CInterceptQueue<HT_RPI_FDATA>
{
	// ダミーフレームの画像部分。ReadOnlyなので注意！
	static void *GetAllocateMemory(int nSize = 0);
public:
	static void InitMemory(int nSize);
	static void *GetMemory();

	CDummyQueueFree(IQueue<HT_RPI_FDATA> *pQue ) 
		:CInterceptQueue<HT_RPI_FDATA>(pQue)
	{}

	//こいつだけOverride
	virtual BOOL AddToTail(HT_RPI_FDATA* pQueData, void *arg = NULL);
};
extern HT_RPI_FDATA *new_dummy_frame();
