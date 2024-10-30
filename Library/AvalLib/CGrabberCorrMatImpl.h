#pragma once


//混色分離行列の実装。
	class CGrabberCorrMatImpl : public IGrabberCorrMat
	{
	public:
        //[混色行列 (r,g,b,offset)x3]
		struct CorrMatrix{
		enum { NCOEF  = 12 };
			float coeff[NCOEF];
		};
	protected:
		CorrMatrix*		my_mat_corr[2];					// 混色行列値 [0or1][カメラセット]
        LONG			my_mat_fore;					// ダブルバッファー。my_mat_corrのどっちの配列を使うか
		int				my_max_camset;


		~CGrabberCorrMatImpl(){
			for(int ii=0; ii<2; ii++)
			{
				delete []my_mat_corr[ii] ;
			}

		}
	public:
		CGrabberCorrMatImpl(int nMaxCamset = 8)
			:my_max_camset(nMaxCamset)
			,my_mat_fore(0)
		{
			for(int ii=0; ii<2; ii++)
			{
				my_mat_corr[ii] = new CorrMatrix[ my_max_camset ];
			}
			ResetCorrMat();
		}


        //現在アクティブな混色分離行列を取得
		const CorrMatrix *GetCorrMat( int camset ) const 
		{
			return &my_mat_corr[my_mat_fore][camset];
		}

        //現在アクティブな混色分離行列をfloatポインタとしてを取得
		const float *GetCorrCoeff( int camset ) const 
		{
			return &my_mat_corr[my_mat_fore][camset].coeff[0] ;
		}

        //混色分離行列を設定し、アクティブネスを切り替える。
		void SetCorrMat(int camset, const float* mat)
		{
            LONG mat_fore = my_mat_fore;
			const LONG mat_back = 1 - my_mat_fore;
			memcpy(&my_mat_corr[mat_back][camset].coeff[0], mat, sizeof(CorrMatrix) );
			InterlockedExchange(&my_mat_fore, mat_back);
		}

        //混色分離行列を初期化する。
		void ResetCorrMat()
		{
			const static CorrMatrix CorrMatIdent = { 1,0,0,0,  0,1,0,0,  0,0,1,0 };
			for(int ii=0; ii<2; ii++)
			{
				for(int jj=0; jj<my_max_camset; jj++)
				{
					memcpy( &my_mat_corr[ii][jj], &CorrMatIdent, sizeof( CorrMatrix ) );
				}
			}

		}

	};