#pragma once
namespace AvalLib{
	class Gamma_12_8
	{
	public:
		double Gamma;
		BYTE *lut;

		Gamma_12_8()
		{
			lut = (BYTE*)VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
			Set(1.8f);
		}
		~Gamma_12_8()
		{
			VirtualFree(lut,0,MEM_RELEASE);
		}
	

		static BYTE to_byte( double v ){
			if( v<0  ) return 0;
			if( v>255) return 255;
			return (BYTE)v;

		}
		void Set( double Gamma){
			double revGamma = 1/Gamma;
			for( int i=0; i<4096; i++){
				lut[i] = to_byte( pow(i / 4095.0, revGamma) * 256);
			}
		}


		void Exec( const WORD *pSrc, BYTE *dst, int w, int h ){
			int wh =  w*h;
			for( int i=0; i<wh; i++){
				WORD pixel = pSrc[i];
				if( pixel >= 4096 ) 
					dst[i] = 255;
				else
					dst[ i ] = lut[ pixel  ];
			}
		}
	};
};