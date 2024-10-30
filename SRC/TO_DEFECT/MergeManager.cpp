#include "StdAfx.h"
#include "MergeManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//------------------------------------------
// コンストラクタ
//------------------------------------------
MergeManager::MergeManager(void) :
mcls_pLog(NULL)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MergeManager::~MergeManager(void)
{
}

//------------------------------------------
// マージ処理実行
// int* nDetectCnt PCID順の疵個数
// MergeInfo* lst	全部の疵リスト配列。 呼び元でNew。使い終わったらdeleteすること
//------------------------------------------
void MergeManager::DoMerge(int* nDetectCnt, MergeInfo* lst)
{
	int ii, jj;

	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	int nDetect = 0;				// トータル疵個数
	for(ii=0; ii<NUM_CAM_POS; ii++) nDetect += nDetectCnt[ii];
	if( 0== nDetect) return;		// 疵無し

	////////////////////////////////////
	// 重なりリスト生成
	////////////////////////////////////
	double dLapRitu;					// 小さい面積の方の疵と重なっている部分の率
	for (ii = 0; ii < nDetect; ii++) {
		for (jj = ii+1; jj < nDetect; jj++) {
			// 自カメラ以外の疵のみ対象
			if( lst[ii].nCamset == lst[jj].nCamset ) continue;

			// 重なり率チェック
			dLapRitu = CheckLap( lst[ii], lst[jj] );
		
			// 閾値超え
			if( dLapRitu >= m_prm.dLapRitu ) {
				// 重なりリスト セット
				nodeLap* p1		= new nodeLap;
				p1->pNext		= lst[ii].lstLap;		// 1個前のポインタをセット (一番初めは、NULLとなる)
				p1->vertex		= jj;					// 対応する配列位置
				lst[ii].lstLap	= p1;
				
				nodeLap* p2		= new nodeLap;
				p2->pNext		= lst[jj].lstLap;	
				p2->vertex		= ii;				
				lst[jj].lstLap	= p2;

				// 両方ともマージ対象の為、とりあえずマージ疵フラグをセットしておく。
				// 連結性判定で、最大面積の疵のみ、マージ疵フラグを落とす。
				lst[ii].pAttr[EPC_ATTR_BASE_MERGE] = 1;
				lst[jj].pAttr[EPC_ATTR_BASE_MERGE] = 1;

//*/◆▲△		
#ifdef DEBUG_TEST
				lst[ii].pAttr[EPC_ATTR_BASE_MERGE] = 0;
				lst[jj].pAttr[EPC_ATTR_BASE_MERGE] = 0;
#endif
//*/

/*/
LOG(em_MSG), "[Merge] 候補 [%.0f>%.0f] a(rec=%d cam=%d frm=%d g=%d,t=%d,面=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f) b(rec=%d cam=%d frm=%d g=%d,t=%d,面=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f)",
	dLapRitu*100, m_prm.dLapRitu*100,
	ii, lst[ii].nCamset, (int)lst[ii].pAttr[FRAME_NUM], lst[ii].yusenG, lst[ii].yusenT, lst[ii].area, lst[ii].len, lst[ii].xmin, lst[ii].xmax, lst[ii].ymin, lst[ii].ymax,
	jj, lst[jj].nCamset, (int)lst[jj].pAttr[FRAME_NUM], lst[jj].yusenG, lst[jj].yusenT, lst[jj].area, lst[jj].len, lst[jj].xmin, lst[jj].xmax, lst[jj].ymin, lst[jj].ymax );
//*/
			}
		}
	}
	

	////////////////////////////////////
	// 重なりリストの連結性判定
	////////////////////////////////////
	int maxrec;
	for (ii = 0; ii < nDetect; ii++) {
		if( ! lst[ii].isChecked && 0 != lst[ii].pAttr[EPC_ATTR_BASE_MERGE] ) {	// 検索し終わった 又は 重なりリストが無い 奴は、最初から何もしない
			// 深さ優先検索
			maxrec = Visit(ii, ii, lst);
			// 重なりリストのグループで 面積が最大の疵は、マージ対象外(=普通の疵)とする
			lst[maxrec].pAttr[EPC_ATTR_BASE_MERGE] = 0;

/*/
LOG(em_MSG), "[Merge] 代表 (rec=%d g=%d,t=%d,面積=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f)",
	maxrec, lst[maxrec].yusenG, lst[maxrec].yusenT, lst[maxrec].area, lst[maxrec].len, lst[maxrec].xmin, lst[maxrec].xmax, lst[maxrec].ymin, lst[maxrec].ymax );
//*/
		}
	}

	////////////////////////////////////
	// 後始末
	////////////////////////////////////
	// 重なりリスト開放
	for (ii = 0; ii < nDetect; ii++) {
		for (nodeLap* p1 = lst[ii].lstLap; NULL != p1;) {
			nodeLap* p2 = p1;
			p1 = p1->pNext;
			SAFE_DELETE(p2);
		}
	}
}

//------------------------------------------
// 重なり率チェック (小さい面積の方の疵と重なっている部分の率)
// const MergeInfo& a 疵情報 A
// const MergeInfo& b 疵情報 B
// 戻り値 重なり率
//------------------------------------------
double MergeManager::CheckLap(const MergeInfo& a, const MergeInfo& b)
{
	double lapLenX;                 // a疵とb疵の重なっているサイズ
	double lapLenY;                 // a疵とb疵の重なっているサイズ

	//// 幅方向の重なりチェック
	if       (a.xmin <= b.xmin && a.xmax > b.xmin && a.xmax <= b.xmax) {
        // a------a
        //     b------b
        lapLenX = a.xmax - b.xmin + 1;

	} else if(a.xmin > b.xmin && a.xmin <= b.xmax && a.xmax > b.xmax ) {
        //     a-------a
        // b-------b
		lapLenX = b.xmax - a.xmin + 1;

	} else if(a.xmin <= b.xmin && a.xmax >= b.xmax ) {
        // a------------a
        //     b---b
		lapLenX = b.xmax - b.xmin + 1;
		
	} else if(a.xmin >= b.xmin && a.xmax <= b.xmax ) {
        //     a---a
		// b------------b
		lapLenX = a.xmax - a.xmin + 1;
	
	} else {
		//  a--a
		//          b----b
		lapLenX = 0;
	}

	//// 長手方向の重なりチェック
	if       (a.ymin <= b.ymin && a.ymax > b.ymin && a.ymax <= b.ymax) {
        lapLenY = a.ymax - b.ymin + 1;

	} else if(a.ymin > b.ymin && a.ymin <= b.ymax && a.ymax > b.ymax ) {
 		lapLenY = b.ymax - a.ymin + 1;
	
	} else if(a.ymin <= b.ymin && a.ymax >= b.ymax ) {
 		lapLenY = b.ymax - b.ymin + 1;
	
	} else if(a.ymin >= b.ymin && a.ymax <= b.ymax ) {
 		lapLenY = a.ymax - a.ymin + 1;
	
	} else {
		lapLenY = 0;
	}

	//// 重なり率を算出 (とりあえず重なっている？)
	if(0 >= lapLenX || 0 >= lapLenY ) return -1.0;					// 重なってない
	
	double widthA	= a.xmax - a.xmin + 1;							// a疵の幅
	double widthB	= b.xmax - b.xmin + 1;							// b疵の幅
	double heightA	= a.ymax - a.ymin + 1;							// a疵の長さ
	double heightB	= b.ymax - b.ymin + 1;							// b疵の長さ

	double lapArea = lapLenY * lapLenX;								// 重なり面積
	double smallArea = __min(widthA*heightA, widthB*heightB);		// 小さい面積の疵決定
	return (lapArea / smallArea);									// 重なり率
}


//------------------------------------------
// 深さ優先探索
// 深さ優先探索をしながら、
// 連結成分（互いに重なっているグループ）のうち
//	Gr優先度>疵種優先度>面積>長手
// となる配列位置を求める
// Ng:外接長方形の面積最大の配列位置を求める
//
// int myrec 検索位置
// int maxrec 最大疵の方の位置
// MergeInfo* lst 疵リスト
//
// 戻り値 代表疵のIndex位置
//------------------------------------------
int MergeManager::Visit(int myrec, int maxrec, MergeInfo* lst)
{
	//// 深さ優先検索したよ
	lst[myrec].isChecked = true;

	//// NULLの場合は、そこが終端 (最初からNULLの場合は、重なっている疵が無かった)
	if( NULL == lst[myrec].lstLap ) return maxrec;


	//// 代表疵選定
	const int cnCnt = 4;
	bool   bMode[cnCnt] ={ false, false, true, false};		// 比較用情報 (Gr[小]、疵種[小]、面積[大]、長手位置[小])
	double dNow[cnCnt] = { (double)lst[myrec].yusenG,	(double)lst[myrec].yusenT,	lst[myrec].area,	lst[myrec].len  };
	double dPre[cnCnt] = { (double)lst[maxrec].yusenG,	(double)lst[maxrec].yusenT,	lst[maxrec].area,	lst[maxrec].len };
	for(int ii=0; ii<cnCnt; ii++) {
		if(dNow[ii] == dPre[ii]) continue;

		if( bMode[ii] ) {
			// 大きい方が強い
			if(dNow[ii] > dPre[ii]) { maxrec = myrec; break; }
			else					{ break; }
		} else {
			// 小さい方が強い
			if(dNow[ii] < dPre[ii]) { maxrec = myrec; break; }
			else					{ break; }
		}
	}

	////// 面積比較 して より大きい面積のレコードを取得
	//if( lst[maxrec].area < lst[myrec].area ) {
	//	maxrec = myrec;
	//}

	//// リストに登録されている分(重なりグループ)を全部検索
	for( nodeLap* p=lst[myrec].lstLap;  p!=NULL;  p=p->pNext ) {
		int othrec = p->vertex;		// 自分と重なった相手の疵のレコード
		if( !lst[othrec].isChecked ) {

/*/
LOG(em_MSG), "[Merge] 重なりグループ (Max%d now%d)", maxrec, othrec);
//*/

			maxrec = Visit( othrec, maxrec, lst );
		}
	}

	return maxrec;
}