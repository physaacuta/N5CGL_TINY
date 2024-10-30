#include "StdAfx.h"
#include "MergeManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MergeManager::MergeManager(void) :
mcls_pLog(NULL)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MergeManager::~MergeManager(void)
{
}

//------------------------------------------
// �}�[�W�������s
// int* nDetectCnt PCID�����r��
// MergeInfo* lst	�S�����r���X�g�z��B �Ăь���New�B�g���I�������delete���邱��
//------------------------------------------
void MergeManager::DoMerge(int* nDetectCnt, MergeInfo* lst)
{
	int ii, jj;

	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	int nDetect = 0;				// �g�[�^���r��
	for(ii=0; ii<NUM_CAM_POS; ii++) nDetect += nDetectCnt[ii];
	if( 0== nDetect) return;		// �r����

	////////////////////////////////////
	// �d�Ȃ胊�X�g����
	////////////////////////////////////
	double dLapRitu;					// �������ʐς̕����r�Əd�Ȃ��Ă��镔���̗�
	for (ii = 0; ii < nDetect; ii++) {
		for (jj = ii+1; jj < nDetect; jj++) {
			// ���J�����ȊO���r�̂ݑΏ�
			if( lst[ii].nCamset == lst[jj].nCamset ) continue;

			// �d�Ȃ藦�`�F�b�N
			dLapRitu = CheckLap( lst[ii], lst[jj] );
		
			// 臒l����
			if( dLapRitu >= m_prm.dLapRitu ) {
				// �d�Ȃ胊�X�g �Z�b�g
				nodeLap* p1		= new nodeLap;
				p1->pNext		= lst[ii].lstLap;		// 1�O�̃|�C���^���Z�b�g (��ԏ��߂́ANULL�ƂȂ�)
				p1->vertex		= jj;					// �Ή�����z��ʒu
				lst[ii].lstLap	= p1;
				
				nodeLap* p2		= new nodeLap;
				p2->pNext		= lst[jj].lstLap;	
				p2->vertex		= ii;				
				lst[jj].lstLap	= p2;

				// �����Ƃ��}�[�W�Ώۂׁ̈A�Ƃ肠�����}�[�W�r�t���O���Z�b�g���Ă����B
				// �A��������ŁA�ő�ʐς��r�̂݁A�}�[�W�r�t���O�𗎂Ƃ��B
				lst[ii].pAttr[EPC_ATTR_BASE_MERGE] = 1;
				lst[jj].pAttr[EPC_ATTR_BASE_MERGE] = 1;

//*/������		
#ifdef DEBUG_TEST
				lst[ii].pAttr[EPC_ATTR_BASE_MERGE] = 0;
				lst[jj].pAttr[EPC_ATTR_BASE_MERGE] = 0;
#endif
//*/

/*/
LOG(em_MSG), "[Merge] ��� [%.0f>%.0f] a(rec=%d cam=%d frm=%d g=%d,t=%d,��=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f) b(rec=%d cam=%d frm=%d g=%d,t=%d,��=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f)",
	dLapRitu*100, m_prm.dLapRitu*100,
	ii, lst[ii].nCamset, (int)lst[ii].pAttr[FRAME_NUM], lst[ii].yusenG, lst[ii].yusenT, lst[ii].area, lst[ii].len, lst[ii].xmin, lst[ii].xmax, lst[ii].ymin, lst[ii].ymax,
	jj, lst[jj].nCamset, (int)lst[jj].pAttr[FRAME_NUM], lst[jj].yusenG, lst[jj].yusenT, lst[jj].area, lst[jj].len, lst[jj].xmin, lst[jj].xmax, lst[jj].ymin, lst[jj].ymax );
//*/
			}
		}
	}
	

	////////////////////////////////////
	// �d�Ȃ胊�X�g�̘A��������
	////////////////////////////////////
	int maxrec;
	for (ii = 0; ii < nDetect; ii++) {
		if( ! lst[ii].isChecked && 0 != lst[ii].pAttr[EPC_ATTR_BASE_MERGE] ) {	// �������I����� ���� �d�Ȃ胊�X�g������ �z�́A�ŏ����牽�����Ȃ�
			// �[���D�挟��
			maxrec = Visit(ii, ii, lst);
			// �d�Ȃ胊�X�g�̃O���[�v�� �ʐς��ő���r�́A�}�[�W�ΏۊO(=���ʂ��r)�Ƃ���
			lst[maxrec].pAttr[EPC_ATTR_BASE_MERGE] = 0;

/*/
LOG(em_MSG), "[Merge] ��\ (rec=%d g=%d,t=%d,�ʐ�=%.0f,L=%.0f x=%.0f,%.0f y=%.0f,%.0f)",
	maxrec, lst[maxrec].yusenG, lst[maxrec].yusenT, lst[maxrec].area, lst[maxrec].len, lst[maxrec].xmin, lst[maxrec].xmax, lst[maxrec].ymin, lst[maxrec].ymax );
//*/
		}
	}

	////////////////////////////////////
	// ��n��
	////////////////////////////////////
	// �d�Ȃ胊�X�g�J��
	for (ii = 0; ii < nDetect; ii++) {
		for (nodeLap* p1 = lst[ii].lstLap; NULL != p1;) {
			nodeLap* p2 = p1;
			p1 = p1->pNext;
			SAFE_DELETE(p2);
		}
	}
}

//------------------------------------------
// �d�Ȃ藦�`�F�b�N (�������ʐς̕����r�Əd�Ȃ��Ă��镔���̗�)
// const MergeInfo& a �r��� A
// const MergeInfo& b �r��� B
// �߂�l �d�Ȃ藦
//------------------------------------------
double MergeManager::CheckLap(const MergeInfo& a, const MergeInfo& b)
{
	double lapLenX;                 // a�r��b�r�̏d�Ȃ��Ă���T�C�Y
	double lapLenY;                 // a�r��b�r�̏d�Ȃ��Ă���T�C�Y

	//// �������̏d�Ȃ�`�F�b�N
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

	//// ��������̏d�Ȃ�`�F�b�N
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

	//// �d�Ȃ藦���Z�o (�Ƃ肠�����d�Ȃ��Ă���H)
	if(0 >= lapLenX || 0 >= lapLenY ) return -1.0;					// �d�Ȃ��ĂȂ�
	
	double widthA	= a.xmax - a.xmin + 1;							// a�r�̕�
	double widthB	= b.xmax - b.xmin + 1;							// b�r�̕�
	double heightA	= a.ymax - a.ymin + 1;							// a�r�̒���
	double heightB	= b.ymax - b.ymin + 1;							// b�r�̒���

	double lapArea = lapLenY * lapLenX;								// �d�Ȃ�ʐ�
	double smallArea = __min(widthA*heightA, widthB*heightB);		// �������ʐς��r����
	return (lapArea / smallArea);									// �d�Ȃ藦
}


//------------------------------------------
// �[���D��T��
// �[���D��T�������Ȃ���A
// �A�������i�݂��ɏd�Ȃ��Ă���O���[�v�j�̂���
//	Gr�D��x>�r��D��x>�ʐ�>����
// �ƂȂ�z��ʒu�����߂�
// Ng:�O�ڒ����`�̖ʐύő�̔z��ʒu�����߂�
//
// int myrec �����ʒu
// int maxrec �ő��r�̕��̈ʒu
// MergeInfo* lst �r���X�g
//
// �߂�l ��\�r��Index�ʒu
//------------------------------------------
int MergeManager::Visit(int myrec, int maxrec, MergeInfo* lst)
{
	//// �[���D�挟��������
	lst[myrec].isChecked = true;

	//// NULL�̏ꍇ�́A�������I�[ (�ŏ�����NULL�̏ꍇ�́A�d�Ȃ��Ă����r����������)
	if( NULL == lst[myrec].lstLap ) return maxrec;


	//// ��\�r�I��
	const int cnCnt = 4;
	bool   bMode[cnCnt] ={ false, false, true, false};		// ��r�p��� (Gr[��]�A�r��[��]�A�ʐ�[��]�A����ʒu[��])
	double dNow[cnCnt] = { (double)lst[myrec].yusenG,	(double)lst[myrec].yusenT,	lst[myrec].area,	lst[myrec].len  };
	double dPre[cnCnt] = { (double)lst[maxrec].yusenG,	(double)lst[maxrec].yusenT,	lst[maxrec].area,	lst[maxrec].len };
	for(int ii=0; ii<cnCnt; ii++) {
		if(dNow[ii] == dPre[ii]) continue;

		if( bMode[ii] ) {
			// �傫����������
			if(dNow[ii] > dPre[ii]) { maxrec = myrec; break; }
			else					{ break; }
		} else {
			// ��������������
			if(dNow[ii] < dPre[ii]) { maxrec = myrec; break; }
			else					{ break; }
		}
	}

	////// �ʐϔ�r ���� ���傫���ʐς̃��R�[�h���擾
	//if( lst[maxrec].area < lst[myrec].area ) {
	//	maxrec = myrec;
	//}

	//// ���X�g�ɓo�^����Ă��镪(�d�Ȃ�O���[�v)��S������
	for( nodeLap* p=lst[myrec].lstLap;  p!=NULL;  p=p->pNext ) {
		int othrec = p->vertex;		// �����Əd�Ȃ���������r�̃��R�[�h
		if( !lst[othrec].isChecked ) {

/*/
LOG(em_MSG), "[Merge] �d�Ȃ�O���[�v (Max%d now%d)", maxrec, othrec);
//*/

			maxrec = Visit( othrec, maxrec, lst );
		}
	}

	return maxrec;
}