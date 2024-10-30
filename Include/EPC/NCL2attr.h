// NCattr.h
// ncdetect
// 特徴量の定義
// EPC　計測・制御開発部　今野
// 2007.8
// 2008.4 幅方向複数カメラ・4眼化対応

#ifndef	_NCATTR_H
#define _NCATTR_H

// 共通特徴量番号の定義
enum COMMON_ATTR {
/*00*/	DEFECT_TYPE = 0,
/*01*/	DEFECT_GRADE,
/*02*/	DEFECT_FLAG,
/*03*/	LABEL_VALUE,
/*04*/	AREA, 
/*05*/	BOX_X_MIN,
/*06*/	BOX_X_MAX,
/*07*/	BOX_Y_MIN,
/*08*/	BOX_Y_MAX,
/*09*/	FERET_X,				// EDASYSマップに必要
/*10*/	FERET_Y,				// EDASYSマップに必要
/*11*/	CENTER_X,				// EDASYSマップに必要
/*12*/	CENTER_Y,				// EDASYSマップに必要
/*13*/	BOX_ASPECT,
/*14*/	FRAME_POS,
/*15*/	POS_FROM_TAIL,
/*16*/	HORZ_RES,				// EDASYSマップに必要
/*17*/	VERT_RES,				// EDASYSマップに必要
/*18*/	LINE_SPEED,
/*19*/	MY_EDGE_POS,
/*20*/	L_EDGE_POS,
/*21*/	R_EDGE_POS,
/*22*/	CENTER_X_FROM_L_EDGE,
/*23*/	CENTER_X_FROM_R_EDGE,
/*24*/	FRAME_NUM,
/*25*/  BLOB_NUM,
/*26*/	MERGED_CENTER_X,		// EDASYSマップに必要
/*27*/	MERGED_CENTER_Y,		// EDASYSマップに必要
/*28*/	FERET_X_PIXEL,			
/*29*/	FERET_Y_PIXEL,
/*30*/	BOX_ASPECT_PIXEL,
/*31*/	AREA_PIXEL,
/*32*/	PERIMETER_PIXEL,
/*33*/	ROUNDNESS_PIXEL,
/*34*/	IMAGE_X,				// EDASYSマップに必要
/*35*/	IMAGE_Y,				// EDASYSマップに必要
/*36*/	CYCLE_LENGTH,			// DetectProcでは存在しない
/*37*/	FRAME_MAX_AREA_RATIO,
/*38*/	FRAME_MEAN_AREA_RATIO,
/*39*/	FRAME_MIN_AREA_RATIO,
/*40*/	CAMSET,
/*41*/	TOP_OR_BOT,
/*42*/	THR_EXPAND,
/*43*/	FRAME_MAX_AREA,
/*44*/	FRAME_MEAN_AREA,
/*45*/	FRAME_MIN_AREA,
/*46*/	MOMENT_XX,	
/*47*/	MOMENT_YY,
/*48*/	MOMENT_XY,
/*49*/	PRINCIPAL_ANGLE,
/*50*/	PRINCIPAL_LENGTH,
/*51*/	SECONDARY_LENGTH,
/*52*/	CYCLE_SERIAL,
/*53*/	CYCLE_CUM_LEN,

/*54*/ POS_FROM_SHEAR_CUT,
/*55*/ DEFECT_GROUP,

/*56*/ CALCED_FEATURE1,
/*57*/ CALCED_FEATURE2,
/*58*/ CALCED_FEATURE3,
/*59*/ CALCED_FEATURE4,
/*60*/ CALCED_FEATURE5,
/*61*/ CALCED_FEATURE6,
/*62*/ CALCED_FEATURE7,
/*63*/ CALCED_FEATURE8,
/*64*/ CALCED_FEATURE9,
/*65*/ CALCED_FEATURE10,

/*66*/ CG_X,
/*67*/ CG_Y,
/*68*/ PIXEL_XSUM,
/*69*/ PIXEL_YSUM,
/*70*/ PIXEL_XXSUM,
/*71*/ PIXEL_YYSUM,
/*72*/ PIXEL_XYSUM,
/*73*/ ABS_X_POS,
/*74*/ MERGED_NUM,

/*75*/ MAX_CONTRAST_X_POS,
/*76*/ MAX_CONTRAST_Y_POS,
/*77*/ MAX_CONTRAST_X_FROM_L_EDGE,
/*78*/ MAX_CONTRAST_X_FROM_R_EDGE,
/*79*/ FERET_Y_MULT_MAX_MIN_B,
/*80*/ FERET_Y_MULT_MAX_MIN_D,
/*81*/ FERET_Y_MULT_MAX_MIN_B_PLUS_MAX_MIN_D,
/*82*/ AREA_MULT_MAX_MIN_B,
/*83*/ AREA_MULT_MAX_MIN_D,
/*84*/ AREA_MULT_MAX_MIN_B_PLUS_MAX_MIN_D,

/*85*/ TINY_TOTAL,
/*86*/ TINY_WH_B_WH_D,
/*87*/ TINY_WH_B_BL_D,
/*88*/ TINY_BL_B_BL_D,
/*89*/ TINY_BL_B_WH_D,
/*90*/ TINY_WH_B,
/*91*/ TINY_BL_B,
/*92*/ TINY_WH_D,
/*93*/ TINY_BL_D,

/*94*/ CALCED_FEATURE11,
/*95*/ CALCED_FEATURE12,
/*96*/ CALCED_FEATURE13,
/*97*/ CALCED_FEATURE14,
/*98*/ CALCED_FEATURE15,
/*99*/ CALCED_FEATURE16,
/*100*/ CALCED_FEATURE17,
/*101*/ CALCED_FEATURE18,
/*102*/ CALCED_FEATURE19,
/*103*/ CALCED_FEATURE20,

/*104*/ CALCED_FEATURE21,
/*105*/ CALCED_FEATURE22,
/*106*/ CALCED_FEATURE23,
/*107*/ CALCED_FEATURE24,
/*108*/ CALCED_FEATURE25,
/*109*/ CALCED_FEATURE26,
/*110*/ CALCED_FEATURE27,
/*111*/ CALCED_FEATURE28,
/*112*/ CALCED_FEATURE29,
/*113*/ CALCED_FEATURE30,

/*114*/ CALCED_FEATURE31,
/*115*/ CALCED_FEATURE32,
/*116*/ CALCED_FEATURE33,
/*117*/ CALCED_FEATURE34,
/*118*/ CALCED_FEATURE35,
/*119*/ CALCED_FEATURE36,
/*120*/ CALCED_FEATURE37,
/*121*/ CALCED_FEATURE38,
/*122*/ CALCED_FEATURE39,
/*123*/ CALCED_FEATURE40,

/*124*/ CALCED_FEATURE41,
/*125*/ CALCED_FEATURE42,
/*126*/ CALCED_FEATURE43,
/*127*/ CALCED_FEATURE44,
/*128*/ CALCED_FEATURE45,
/*129*/ CALCED_FEATURE46,
/*130*/ CALCED_FEATURE47,
/*131*/ CALCED_FEATURE48,
/*132*/ CALCED_FEATURE49,
/*133*/ CALCED_FEATURE50,
/*134*/	FEAT_SCODE,

/*135*/ CYCLE_NMULTI, // ncdetect_pdのみ
/*136*/ ROLL_ONOFF,   // ncdetect_pdのみ
/*137*/ SYNCADD_INDEX,       // ncdetect_pdのみ
/*138*/ SYNCADD_PIX_MAX,     // ncdetect_pdのみ
/*139*/ SYNCADD_PIX_MIN,     // ncdetect_pdのみ
/*140*/ SYNCADD_PIX_MAX_AVG, // ncdetect_pdのみ
/*141*/ SYNCADD_PIX_MIN_AVG, // ncdetect_pdのみ

/*142*/ FIRST_CLASSIFIED_TYPE,
/*143*/ POS_FROM_X0,
/*144*/ POS_FROM_X1,
/*145*/ POS_FROM_X2,
/*146*/ POS_FROM_X3,
/*147*/ LINE_SPEED_MPM,
/*148*/ BIN_VERT_PROJ,
/*149*/ BIN_HORZ_PROJ,
/*150*/ SORT_INDEX,

/*151*/ SERIAL_NUMBER_IN_COIL,
/*152*/ LOCAL_DENSITY,
/*153*/ THRESHOLD_TYPE, //OFFSET_Y_TO_OPPOSITE,
/*154*/ OPPSITE_CAMSET,
/*155*/ OPPOSITE_SERIAL_NUMBER,
/*156*/ OPPOSITE_TYPE,
/*157*/ OPPOSITE_GRADE,

/*158*/ MIN_DEPTH,
/*159*/ MAX_DEPTH,
/*160*/ MEAN_DEPTH,
/*161*/ SIGMA_DEPTH,
/*162*/ MAX_MIN_DEPTH,
/*163*/ SQSUM_DEPTH,
/*162*/ OFFSET_X_TO_OPPOSITE,
/*163*/ OFFSET_Y_TO_OPPOSITE,
// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
///*164*/ LAST_COMMON_ATTR

// エッジ部分
/*166*/ LABELING_TYPE,
// 長い疵追加分 //
/*167*/ FIRST_CLASSIFIED_GRADE,
/*168*/ IFMERGE_FLAG,
/*169*/ LONGMERGED_TYPE,
/*170*/ LONGMERGED_GRADE,
/*171*/ LONGMERGED_MAXGRADE,
/*172*/ LONGMERGED_MINGRADE,
/*173*/ LONGMERGED_MAXTYPE,
/*174*/ LONGMERGED_MINTYPE,
/*175*/ LONGMERGED_BOX_X_MIN,
/*176*/ LONGMERGED_BOX_X_MAX,
/*177*/ LONGMERGED_BOX_Y_MIN,
/*178*/ LONGMERGED_BOX_Y_MAX,
/*179*/ LONGMERGED_AREA_PIXEL,
/*180*/ LONGMERGED_FERET_X_PIXEL,
/*181*/ LONGMERGED_FERET_Y_PIXEL,
/*182*/ LONGMERGED_BOX_ASPECT_PIXEL,
/*183*/ LONGMERGED_FERET_X,     // mm単位
/*184*/ LONGMERGED_FERET_Y, // mm単位
/*185*/ LONGMERGED_AREA,
/*186*/ LONGMERGED_BOX_ASPECT,
/*187*/ LONGMERGED_CENTER_X,
/*188*/ LONGMERGED_CENTER_Y,
/*189*/ LONGMERGED_ABS_X_POS,
/*190*/ LONGMERGED_CENTER_X_FROM_L_EDGE, 
/*191*/ LONGMERGED_CENTER_X_FROM_R_EDGE,
        IMAGE_FRAME_IDX,	// for test
        IMAGE_EW,			// for test
        IMAGE_RECT_LEFT,	// for test
        IMAGE_RECT_RIGHT,	// for test
        IMAGE_RECT_TOP,		// for test
        IMAGE_RECT_BOT,		// for test
        IMAGE_EDGE_POS,		// for test
        IMAGE_EDGE_POS_DETECT, // for test 
/*200*/ LAST_COMMON_ATTR
// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

};

// カメラ角度別の特徴量番号の定義
enum ANGLE_ATTR {
/*00*/ MIN_PIXEL = 0,
/*01*/ MAX_PIXEL,
/*02*/ MEAN_PIXEL,
/*03*/ SIGMA_PIXEL,
/*04*/ MAX_MIN,
/*05*/ SLICE_L0,
/*06*/ SLICE_L1,
/*07*/ SLICE_L2,
/*08*/ SLICE_L3,
/*09*/ SLICE_U0,
/*10*/ SLICE_U1,
/*11*/ SLICE_U2,
/*12*/ SLICE_U3,
/*13*/ VP_MIN,
/*14*/ VP_MAX,
/*15*/ VP_MEAN,
/*16*/ FRAME_MIN_PIXEL,
/*17*/ FRAME_MAX_PIXEL,
/*18*/ FRAME_MEAN_PIXEL,
/*19*/ SLICE_L0_AREA,
/*20*/ SLICE_L1_AREA,
/*21*/ SLICE_L2_AREA,
/*22*/ SLICE_L3_AREA,
/*23*/ SLICE_U0_AREA,
/*24*/ SLICE_U1_AREA,
/*25*/ SLICE_U2_AREA,
/*26*/ SLICE_U3_AREA,
/*27*/ HP_MIN,
/*28*/ HP_MAX,
/*29*/ HP_MEAN,
/*30*/ VP_MAX_MIN,
/*31*/ HP_MAX_MIN,
/*32*/ FRAME_SLICE_L0_AREA_SUM,
/*33*/ FRAME_SLICE_L1_AREA_SUM,
/*34*/ FRAME_SLICE_L2_AREA_SUM,
/*35*/ FRAME_SLICE_L3_AREA_SUM,
/*36*/ FRAME_SLICE_U0_AREA_SUM,
/*37*/ FRAME_SLICE_U1_AREA_SUM,
/*38*/ FRAME_SLICE_U2_AREA_SUM,
/*39*/ FRAME_SLICE_U3_AREA_SUM,
/*40*/ FRAME_SLICE_L0_NUM,
/*41*/ FRAME_SLICE_L1_NUM,
/*42*/ FRAME_SLICE_L2_NUM,
/*43*/ FRAME_SLICE_L3_NUM,
/*44*/ FRAME_SLICE_U0_NUM,
/*45*/ FRAME_SLICE_U1_NUM,
/*46*/ FRAME_SLICE_U2_NUM,
/*47*/ FRAME_SLICE_U3_NUM,
/*48*/ HIST_0,
/*49*/ HIST_1,
/*50*/ HIST_2,
/*51*/ HIST_3,
/*52*/ HIST_4,
/*53*/ HIST_PEAK_POS,
/*54*/ HIST_PEAK_VAL,
/*55*/ HIST_WIDTH_2,
/*56*/ HIST_WIDTH_5,
/*57*/ HIST_BOX_0,
/*58*/ HIST_BOX_1,
/*59*/ HIST_BOX_2,
/*60*/ HIST_BOX_3,
/*61*/ HIST_BOX_4,
/*62*/ HIST_BOX_PEAK_POS,
/*63*/ HIST_BOX_PEAK_VAL,
/*64*/ HIST_BOX_WIDTH_2,
/*65*/ HIST_BOX_WIDTH_5,
/*66*/ WIND_BOX_POS_VERT,
/*67*/ WIND_BOX_PIXEL_VERT,
/*68*/ WIND_BOX_POS_HORZ,
/*69*/ WIND_BOX_PIXEL_HORZ,
/*70*/ RAW_FRAME_BRIGHTNESS,
/*71*/ PIXEL_SQSUM,
/*72*/ LENGTH_L0,
/*73*/ LENGTH_L1,
/*74*/ LENGTH_L2,
/*75*/ LENGTH_L3,
/*76*/ LENGTH_U0,
/*77*/ LENGTH_U1,
/*78*/ LENGTH_U2,
/*79*/ LENGTH_U3,
/*80*/ WIDTH_L0,
/*81*/ WIDTH_L1,
/*82*/ WIDTH_L2,
/*83*/ WIDTH_L3,
/*84*/ WIDTH_U0,
/*85*/ WIDTH_U1,
/*86*/ WIDTH_U2,
/*87*/ WIDTH_U3,
/*88*/ CAM_BITPOS,
/*89*/ RAW_MIN_PIXEL,
/*90*/ RAW_MAX_PIXEL,
/*91*/ RAW_MEAN_PIXEL,
/*92*/ RAW_SIGMA_PIXEL,
/*93*/ SIGMA_FRAME,
/*94*/ RAW_SIGMA_FRAME,
/*95*/ RAW_PROJ_HDIFF,
/*96*/ TOP_EDGE_PIXEL,
/*97*/ BOTTOM_EDGE_PIXEL,
/*98*/ LEFT_EDGE_PIXEL,
/*99*/ RIGHT_EDGE_PIXEL,
/*100*/ RAW_TOP_EDGE_PIXEL,
/*101*/ RAW_BOTTOM_EDGE_PIXEL,
/*102*/ RAW_LEFT_EDGE_PIXEL,
/*103*/ RAW_RIGHT_EDGE_PIXEL,
/*104*/ RAW_PIXEL_SQSUM,
/*105*/ RAW_HPROJ_STEP,
/*106*/ NGC_OFFSET_Y,
/*107*/ NGC_MAX_VALUE,
/*108*/ MAX_PIXEL_X_POS,
/*109*/ MAX_PIXEL_Y_POS,
/*110*/ MIN_PIXEL_X_POS,
/*111*/ MIN_PIXEL_Y_POS,
/*112*/ VP_1ST_POS_PEAK,
/*113*/ VP_2ND_POS_PEAK,
/*114*/ VP_POS_PEAK_GAP,
/*115*/ VP_POS_BOTTOM,
/*116*/ VP_1ST_NEG_PEAK,
/*117*/ VP_2ND_NEG_PEAK,
/*118*/ VP_NEG_PEAK_GAP,
/*119*/ VP_NEG_BOTTOM,
/*120*/ LAST_ANGLE_ATTR

};

static const int ncl_nattr = LAST_COMMON_ATTR + 4*LAST_ANGLE_ATTR;
#define BASE_ANGLE_INDEX(x) (LAST_COMMON_ATTR + (x)*LAST_ANGLE_ATTR)

struct nc_common_attr {			// 共通特徴量の構造体
	long	edanum;		// Edasys ID
	const char	*const name;		// 名称
	long	stage;		// 0:最初のロジックテーブルで使う, 1:周期疵のロジックテーブルで使う
};

const struct nc_common_attr nc_common_attr_table[] = {
	{60  , "疵種ID"                                       , 1}, // 0:DEFECT_TYPE
	{61  , "グレードID"                                   , 1}, // 1:DEFECT_GRADE
	{62  , "フラグ"                                       , 1}, // 2:DEFECT_FLAG

	{53  , "ラベル番号"                                   , 0}, // 3:LABEL_VALUE
	{43  , "面積[mm2]"                                    , 0}, // 4:AREA
	{27  , "外接Xmin[pixel]"                              , 0}, // 5:BOX_X_MIN
	{28  , "外接Xmax[pixel]"                              , 0}, // 6:BOX_X_MAX
	{37  , "外接Ymin[pixel]"                              , 0}, // 7:BOX_Y_MIN
	{38  , "外接Ymax[pixel]"                              , 0}, // 8:BOX_Y_MAX
	{1   , "外接幅[mm]"                                   , 0}, // 9:FERET_X
	{2   , "外接長さ[mm]"                                 , 0}, // 10:FERET_Y
	{3   , "フレーム内疵中心X[mm]"                        , 0}, // 11:CENTER_X
	{42  , "フレーム内疵中心Y[mm]"                        , 0}, // 12:CENTER_Y
	{33  , "外接縦横比"                                   , 0}, // 13:BOX_ASPECT
	{50  , "フレーム長手位置[mm]"                         , 0}, // 14:FRAME_POS
	{51  , "コイル残り長さ[mm]"                           , 0}, // 15:POS_FROM_TAIL
	{86  , "横分解能[mm]"                                 , 0}, // 16:HORZ_RES
	{85  , "縦分解能[mm]"                                 , 0}, // 17:VERT_RES
	{56  , "ライン速度[mm/s]"                             , 0}, // 18:LINE_SPEED
	{52  , "自己検出エッジ位置[pixel]"                    , 0}, // 19:MY_EDGE_POS
	{54  , "左エッジ位置[mm]"                             , 0}, // 20:L_EDGE_POS
	{55  , "右エッジ位置[mm]"                             , 0}, // 21:R_EDGE_POS
	{35  , "左エッジからの距離[mm]"                       , 0}, // 22:CENTER_X_FROM_L_EDGE
	{36  , "右エッジからの距離[mm]"                       , 0}, // 23:CENTER_X_FROM_R_EDGE
	{57  , "フレーム番号"                                 , 0}, // 24:FRAME_NUM
	{58  , "ラベリング個数"                               , 0}, // 25:BLOB_NUM

	{34  , "カメラ座標疵幅位置X[mm]"                      , 0}, // 26:MERGED_CENTER_X
	{44  , "コイル先頭からの疵長手位置[mm]"               , 0}, // 27:MERGED_CENTER_Y

	{11  , "外接幅[pixel]"                                , 0}, // 28:FERET_X_PIXEL
	{12  , "外接長さ[pixel]"                              , 0}, // 29:FERET_Y_PIXEL
	{13  , "ピクセル縦横比"                               , 0}, // 30:BOX_ASPECT_PIXEL
	{147 , "画素単位面積[pixel]"                          , 0}, // 31:AREA_PIXEL
	{150 , "画素単位周囲長[pixel]"                        , 0}, // 32:PERIMETER_PIXEL
	{151 , "画素単位疵丸さ[pixel]"                        , 0}, // 33:ROUNDNESS_PIXEL

	{92  , "フレーム内画像左上X[mm]"                      , 0}, // 34:IMAGE_X
	{93  , "フレーム内画像左上Y[mm]"                      , 0}, // 35:IMAGE_Y

	{59  , "周期長さ[mm]"                                 , 1}, // 36:CYCLE_LENGTH

	{45  , "フレーム内最大疵面積比"                       , 0}, // 37:FRAME_MAX_AREA_RATIO
	{46  , "フレーム内平均疵面積比"                       , 0}, // 38:FRAME_MEAN_AREA_RATIO
	{47  , "フレーム内最小疵面積比"                       , 0}, // 39:FRAME_MIN_AREA_RATIO
	{63  , "カメラセット番号"                             , 0}, // 40:CAMSET
	{88  , "表裏識別"                                     , 0}, // 41:TOP_OR_BOT
	{64  , "二値化閾値拡大量"                             , 0}, // 42:THR_EXPAND

	{136 , "フレーム内最大疵面積[mm2]"                    , 0}, // 43:FRAME_MAX_AREA
	{137 , "フレーム内平均疵面積[mm2]"                    , 0}, // 44:FRAME_MEAN_AREA
	{138 , "フレーム内最小疵面積[mm2]"                    , 0}, // 45:FRAME_MIN_AREA

	{152 , "重心モーメントXX[mm2]"                        , 0}, // 46:MOMENT_XX
	{153 , "重心モーメントYY[mm2]"                        , 0}, // 47:MOMENT_YY
	{154 , "重心モーメントXY[mm2]"                        , 0}, // 48:MOMENT_XY
	{155 , "主軸方向[度]"                                 , 0}, // 49:PRINCIPAL_ANGLE
	{156 , "主軸長さ[mm]"                                 , 0}, // 50:PRINCIPAL_LENGTH
	{157 , "第二軸長さ[mm]"                               , 0}, // 51:SECONDARY_LENGTH


	{65  , "周期疵連番"                                   , 1}, // 52:CYCLE_SERIAL
	{66  , "先頭周期疵からの距離[mm]"                     , 1}, // 53:CYCLE_CUM_LEN


	{67  , "シャーカットからの距離[mm]"                   , 0}, // 54:POS_FROM_SHEAR_CUT
	{68  , "疵グループID"                                 , 1}, // 55:DEFECT_GROUP

	{401 , "計算特徴量1"                                  , 0}, // 56:CALCED_FEAT_1
	{402 , "計算特徴量2"                                  , 0}, // 57:CALCED_FEAT_2
	{403 , "計算特徴量3"                                  , 0}, // 58:CALCED_FEAT_3
	{404 , "計算特徴量4"                                  , 0}, // 59:CALCED_FEAT_4
	{405 , "計算特徴量5"                                  , 0}, // 60:CALCED_FEAT_5
	{406 , "計算特徴量6"                                  , 0}, // 61:CALCED_FEAT_6
	{407 , "計算特徴量7"                                  , 0}, // 62:CALCED_FEAT_7
	{408 , "計算特徴量8"                                  , 0}, // 63:CALCED_FEAT_8
	{409 , "計算特徴量9"                                  , 0}, // 64:CALCED_FEAT_9
	{410 , "計算特徴量10"                                 , 0}, // 65:CALCED_FEAT_10

	{272 , "フレーム内重心位置X[mm]"                      , 0}, // 66:CG_X
	{273 , "フレーム内重心位置Y[mm]"                      , 0}, // 67:CG_Y
	{276 , "X座標和[pixel]"                               , 0}, // 68:PIXEL_XSUM
	{277 , "Y座標和[pixel]"                               , 0}, // 69:PIXEL_YSUM
	{278 , "X座標２乗和[pixel]"                           , 0}, // 70:PIXEL_XXSUM
	{279 , "Y座標２乗和[pixel]"                           , 0}, // 71:PIXEL_YYSUM
	{280 , "XY座標積和[pixel]"                            , 0}, // 72:PIXEL_XYSUM
	{281 , "幅方向絶対位置[mm]"                           , 0}, // 73:ABS_X_POS
	{282 , "マージ個数"                                   , 0}, // 74:MERGED_NUM

	{411 , "コントラスト最大点X[pixel]"                   , 0}, // 75:MAX_CONTRAST_X_POS
	{412 , "コントラスト最大点Y[pixel]"                   , 0}, // 76:MAX_CONTRAST_Y_POS
	{413 , "左エッジからコントラスト最大点までの距離[mm]" , 0}, // 77:MAX_CONTRAST_X_FROM_L_EDGE
	{414 , "右エッジからコントラスト最大点までの距離[mm]" , 0}, // 78:MAX_CONTRAST_X_FROM_R_EDGE
	{415 , "未使用"                                       , 0}, // 79:FERET_Y_MULT_MAX_MIN_B
	{416 , "未使用"                                       , 0}, // 80:FERET_Y_MULT_MAX_MIN_D
	{417 , "未使用"                                       , 0}, // 81:FERET_Y_MULT_MAX_MIN_B_PLUS_MAX_MIN_D
	{418 , "未使用"                                       , 0}, // 82:AREA_MULT_MAX_MIN_B
	{419 , "未使用"                                       , 0}, // 83:AREA_MULT_MAX_MIN_D
	{420 , "未使用"                                       , 0}, // 84:AREA_MULT_MAX_MIN_B_PLUS_MAX_MIN_D

	{325 , "未使用"                                       , 0}, // 85:TINY_TOTAL
	{326 , "未使用"                                       , 0}, // 86:TINY_WH_B_WH_D
	{327 , "未使用"                                       , 0}, // 87:TINY_WH_B_BL_D
	{328 , "未使用"                                       , 0}, // 88:TINY_BL_B_BL_D
	{329 , "未使用"                                       , 0}, // 89:TINY_BL_B_WH_D
	{330 , "未使用"                                       , 0}, // 90:TINY_WH_B
	{331 , "未使用"                                       , 0}, // 91:TINY_BL_B
	{332 , "未使用"                                       , 0}, // 92:TINY_WH_D
	{333 , "未使用"                                       , 0}, // 93:TINY_BL_D

	{421 , "計算特徴量11"                                 , 0}, // 94:CALCED_FEAT_11
	{422 , "計算特徴量12"                                 , 0}, // 95:CALCED_FEAT_12
	{423 , "計算特徴量13"                                 , 0}, // 96:CALCED_FEAT_13
	{424 , "計算特徴量14"                                 , 0}, // 97:CALCED_FEAT_14
	{425 , "計算特徴量15"                                 , 0}, // 98:CALCED_FEAT_15
	{426 , "計算特徴量16"                                 , 0}, // 99:CALCED_FEAT_16
	{427 , "計算特徴量17"                                 , 0}, // 100:CALCED_FEAT_17
	{428 , "計算特徴量18"                                 , 0}, // 101:CALCED_FEAT_18
	{429 , "計算特徴量19"                                 , 0}, // 102:CALCED_FEAT_19
	{430 , "計算特徴量20"                                 , 0}, // 103:CALCED_FEAT_20
	{431 , "計算特徴量21"                                 , 0}, // 104:CALCED_FEAT_21
	{432 , "計算特徴量22"                                 , 0}, // 105:CALCED_FEAT_22
	{433 , "計算特徴量23"                                 , 0}, // 106:CALCED_FEAT_23
	{434 , "計算特徴量24"                                 , 0}, // 107:CALCED_FEAT_24
	{435 , "計算特徴量25"                                 , 0}, // 108:CALCED_FEAT_25
	{436 , "計算特徴量26"                                 , 0}, // 109:CALCED_FEAT_26
	{437 , "計算特徴量27"                                 , 0}, // 110:CALCED_FEAT_27
	{438 , "計算特徴量28"                                 , 0}, // 111:CALCED_FEAT_28
	{439 , "計算特徴量29"                                 , 0}, // 112:CALCED_FEAT_29
	{440 , "計算特徴量30"                                 , 0}, // 113:CALCED_FEAT_30
	{441 , "計算特徴量31"                                 , 0}, // 114:CALCED_FEAT_31
	{442 , "計算特徴量32"                                 , 0}, // 115:CALCED_FEAT_32
	{443 , "計算特徴量33"                                 , 0}, // 116:CALCED_FEAT_33
	{444 , "計算特徴量34"                                 , 0}, // 117:CALCED_FEAT_34
	{445 , "計算特徴量35"                                 , 0}, // 118:CALCED_FEAT_35
	{446 , "計算特徴量36"                                 , 0}, // 119:CALCED_FEAT_36
	{447 , "計算特徴量37"                                 , 0}, // 120:CALCED_FEAT_37
	{448 , "計算特徴量38"                                 , 0}, // 121:CALCED_FEAT_38
	{449 , "計算特徴量39"                                 , 0}, // 122:CALCED_FEAT_39
	{450 , "計算特徴量40"                                 , 0}, // 123:CALCED_FEAT_40
	{451 , "計算特徴量41"                                 , 0}, // 124:CALCED_FEAT_41
	{452 , "計算特徴量42"                                 , 0}, // 125:CALCED_FEAT_42
	{453 , "計算特徴量43"                                 , 0}, // 126:CALCED_FEAT_43
	{454 , "計算特徴量44"                                 , 0}, // 127:CALCED_FEAT_44
	{455 , "計算特徴量45"                                 , 0}, // 128:CALCED_FEAT_45
	{456 , "計算特徴量46"                                 , 0}, // 129:CALCED_FEAT_46
	{457 , "計算特徴量47"                                 , 0}, // 130:CALCED_FEAT_47
	{458 , "計算特徴量48"                                 , 0}, // 131:CALCED_FEAT_48
	{459 , "計算特徴量49"                                 , 0}, // 132:CALCED_FEAT_49
	{460 , "計算特徴量50"                                 , 0}, // 133:CALCED_FEAT_50
	{69  , "SCODE(鋼種/表面グループ)"                     , 0}, // 134:FEAT_SCODE

	{700 , "周期疵多重度"                                 , 0}, // 135:CYCLE_NMULTI
	{701 , "圧下ONOFF"                                    , 0}, // 136:ROLL_ONOFF
	{702 , "同期加算index"                                , 0}, // 137:SYNCADD_INDEX
	{703 , "同期加算最大輝度"                             , 0}, // 138:SYNCADD_PIX_MAX
	{704 , "同期加算最小輝度"                             , 0}, // 139:SYNCADD_PIX_MIN
	{705 , "同期加算最大輝度平均"                         , 0}, // 140:SYNCADD_PIX_MAX_AVG
	{706 , "同期加算最小輝度平均"                         , 0}, // 141:SYNCADD_PIX_MIN_AVG

	{70  , "第一段判定疵種ID"                             , 1}, // 142:FIRST_CLASSIFIED_TYPE
	{102 , "X0からの距離[mm]"                             , 0}, // 143:POS_FROM_X0
	{103 , "X1からの距離[mm]"                             , 0}, // 144:POS_FROM_X1
	{104 , "X2からの距離[mm]"                             , 0}, // 145:POS_FROM_X2
	{105 , "X3からの距離[mm]"                             , 0}, // 146:POS_FROM_X3
	{106 , "ライン速度[mpm]"                              , 0}, // 147:LINE_SPEED_MPM
	{107 , "二値縦プロジェクション長[pixel]"              , 0}, // 148:BIN_VERT_PROJ
	{108 , "二値横プロジェクション長[pixel]"              , 0}, // 149:BIN_HORZ_PROJ
	{109 , "フレーム内順位"                               , 1}, // 150:SORT_INDEX
	{71  , "カメラセット毎疵番号"                         , 0}, // 151:SERIAL_NUMBER_IN_COIL
	{72  , "局所疵密度[個/グリッド]"                      , 0}, // 152:LOCAL_DENSITY
	{73  , "多重分解能種別"                               , 0}, // 153: THRESHOLD_TYPE <= OFFSET_Y_TO_OPPOSITE
	{74  , "反対面疵カメラセット"                         , 1}, // 154:OPPSITE_CAMSET
	{75  , "反対面疵番号"                                 , 1}, // 155:OPPOSITE_SERIAL_NUMBER
	{76  , "反対面疵種ID"                                 , 1}, // 156:OPPOSITE_TYPE
	{77  , "反対面疵グレードID"                           , 1}, // 157:OPPOSITE_GRADE

	{5023, "最小疑似深さ"                                 , 0}, // 158:MIN_DEPTH
	{5022, "最大疑似深さ"                                 , 0}, // 159:MAX_DEPTH
	{5024, "平均疑似深さ[mm]"                             , 0}, // 160:MEAN_DEPTH
	{5025, "疑似深さ標準偏差[mm]"                         , 0}, // 161:SIGMA_DEPTH
	{5026, "最大疑似深さ－最小疑似深さ[mm]"               , 0}, // 162:MAX_MIN_DEPTH
	{5274, "疑似深さ２乗和[mm^2]"                         , 0}, // 163:SQSUM_DEPTH
	{78  , "反対面疵X偏差[mm]"                            , 1}, // 164:OFFSET_X_TO_OPPOSITE
	{79  , "反対面疵Y偏差[mm]"                            , 1}, // 165:OFFSET_Y_TO_OPPOSITE
// 2023.07.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
	{80,    "二値化種別",0 }     ,                  // 166:LABELING_TYPE,
// 長い疵追加分
    {900,   "第一段判定グレード", 1},               // 167:FIRST_CLASSIFIED_GRADE,
    {901,   "フレーム間連結フラグ", 0},             // 168:IFMERGE_FLAG,
    {902,   "連結後判定疵種", 1},                   // 169:LONGMERGED_TYPE,
    {903,   "連結後判定グレード", 1},               // 170:LONGMERGED_GRADE,
    {904,   "連結疵内最大グレード", 1},             // 171:LONGMERGED_MAXGRADE,
    {905,   "連結疵内最小グレード", 1},             // 172:LONGMERGED_MINGRADE,
    {906,   "連結疵内最大疵種", 1},                 // 173:LONGMERGED_MAXTYPE,
    {907,   "連結疵内最小疵種", 1},                 // 174:LONGMERGED_MINTYPE,
    {908,   "連結後外接Xmin[pixel]", 1},            // 175:LONGMERGED_BOX_X_MIN,
    {909,   "連結後外接Xmax[pixel]", 1},            // 176:LONGMERGED_BOX_X_MAX,
    {910,   "連結後外接Ymin[pixel]", 1},            // 177:LONGMERGED_BOX_Y_MIN,
    {911,   "連結後外接Ymax[pixel]", 1},            // 178:LONGMERGED_BOX_Y_MAX,
    {912,   "連結後画素単位面積[pixel]", 1},        // 179:LONGMERGED_AREA_PIXEL,
    {913,   "連結後外接幅[pixel]", 1},              // 180:LONGMERGED_FERET_X_PIXEL,
    {914,   "連結後外接長さ[pixel]", 1},            // 181:LONGMERGED_FERET_Y_PIXEL,
    {915,   "連結後ピクセル縦横比", 1},             // 182:LONGMERGED_BOX_ASPECT_PIXEL,
    {916,   "連結後外接幅[mm]", 1},                 // 183:LONGMERGED_FERET_X,     // mm単位
    {917,   "連結後外接長さ[mm]", 1},               // 184:LONGMERGED_FERET_Y,		// mm単位
    {918,   "連結後面積[mm2]", 1},                  // 185:LONGMERGED_AREA,
    {919,   "連結後外接縦横比", 1},                 // 186:LONGMERGED_BOX_ASPECT,
    {920,   "連結後フレーム内疵中心X[mm]", 1},      // 187:LONGMERGED_CENTER_X,
    {921,   "連結後フレーム内疵中心Y[mm]", 1},      // 188:LONGMERGED_CENTER_Y,
    {922,   "連結後幅方向絶対位置[mm]", 1},         // 189:LONGMERGED_ABS_X_POS,
    {923,   "連結後左エッジからの距離[mm]", 1},     // 190:LONGMERGED_CENTER_X_FROM_L_EDGE,
    {924,   "連結後右エッジからの距離[mm]", 1},     // 191:LONGMERGED_CENTER_X_FROM_R_EDGE,
// 2023.07.14 画像処理装置 H・Sソフト改造 <<<<<--------------------
};

static const char * angle_suffix[4] = {"(G)", "(B)", "(R)", "(R-B)"};

struct nc_angle_attr {
	long	edanum[4];
	const char	*const basename;
	long	stage;
};

const struct nc_angle_attr nc_angle_attr_table[] = {
	{{23, 14, 3023, 4023},		"最小輝度",				0},			// 00:MIN_PIXEL
	{{22, 15, 3022, 4022},		"最大輝度",				0},			// 01:MAX_PIXEL
	{{24, 16, 3024, 4024},		"平均輝度",				0},			// 02:MEAN_PIXEL
	{{25, 17, 3025, 4025},		"輝度標準偏差",			0},			// 03:SIGMA_PIXEL
	{{26, 18, 3026, 4026},		"最大輝度－最小輝度",	0},			// 04:MAX_MIN
	{{110, 118, 3110, 4110},	"スライス特徴黒0",		0},			// 05:SLICE_L0
	{{111, 119, 3111, 4111},	"スライス特徴黒1",		0},			// 06:SLICE_L1
	{{112, 120, 3112, 4112},	"スライス特徴黒2",		0},			// 07:SLICE_L2
	{{113, 121, 3113, 4113},	"スライス特徴黒3",		0},			// 08:SLICE_L3
	{{114, 122, 3114, 4114},	"スライス特徴白0",		0},			// 09:SLICE_U0
	{{115, 123, 3115, 4115},	"スライス特徴白1",		0},			// 10:SLICE_U1
	{{116, 124, 3116, 4116},	"スライス特徴白2",		0},			// 11:SLICE_U2
	{{117, 125, 3117, 4117},	"スライス特徴白3",		0},			// 12:SLICE_U3
	{{130, 133, 3130, 4130},	"縦プロジェクション最小", 0},		// 13:VP_MIN
	{{131, 134, 3131, 4131},	"縦プロジェクション最大", 0},		// 14:VP_MAX
	{{132, 135, 3132, 4132},	"縦プロジェクション平均", 0},		// 15:VP_MEAN
	{{139, 142, 3139, 4139},	"フレーム内最小輝度平均", 0},		// 16:FRAME_MIN_PIXEL
	{{140, 143, 3140, 4140},	"フレーム内最大輝度平均", 0},		// 17:FRAME_MAX_PIXEL
	{{141, 144, 3141, 4141},	"フレーム内平均輝度平均", 0},		// 18:FRAME_MEAN_PIXEL
	{{160, 168, 3160, 4160},	"スライス面積黒0[mm2]", 0},			// 19:SLICE_L0_AREA
	{{161, 169, 3161, 4161},	"スライス面積黒1[mm2]", 0},			// 20:SLICE_L1_AREA
	{{162, 170, 3162, 4162},	"スライス面積黒2[mm2]", 0},			// 21:SLICE_L2_AREA
	{{163, 171, 3163, 4163},	"スライス面積黒3[mm2]", 0},			// 22:SLICE_L3_AREA
	{{164, 172, 3164, 4164},	"スライス面積白0[mm2]", 0},			// 23:SLICE_U0_AREA
	{{165, 173, 3165, 4165},	"スライス面積白1[mm2]", 0},			// 24:SLICE_U1_AREA
	{{166, 174, 3166, 4166},	"スライス面積白2[mm2]", 0},			// 25:SLICE_U2_AREA
	{{167, 175, 3167, 4167},	"スライス面積白3[mm2]", 0},			// 26:SLICE_U3_AREA
	{{176, 179, 3176, 4176},	"横プロジェクション最小", 0},		// 27:HP_MIN
	{{177, 180, 3177, 4177},	"横プロジェクション最大", 0},		// 28:HP_MAX
	{{178, 181, 3178, 4178},	"横プロジェクション平均", 0},		// 29:HP_MEAN
	{{182, 183, 3182, 4182},	"縦プロジェクション最大－最小", 0},	// 30:VP_MAX_MIN
	{{184, 185, 3184, 4184},	"横プロジェクション最大－最小", 0},	// 31:HP_MAX_MIN
	{{186, 194, 3186, 4186},	"スライス面積総和黒0[mm2]", 0},		// 32:FRAME_SLICE_L0_AREA_SUM
	{{187, 195, 3187, 4187},	"スライス面積総和黒1[mm2]", 0},		// 33:FRAME_SLICE_L1_AREA_SUM
	{{188, 196, 3188, 4188},	"スライス面積総和黒2[mm2]", 0},		// 34:FRAME_SLICE_L2_AREA_SUM
	{{189, 197, 3189, 4189},	"スライス面積総和黒3[mm2]", 0},		// 35:FRAME_SLICE_L3_AREA_SUM
	{{190, 198, 3190, 4190},	"スライス面積総和白0[mm2]", 0},		// 36:FRAME_SLICE_U0_AREA_SUM
	{{191, 199, 3191, 4191},	"スライス面積総和白1[mm2]", 0},		// 37:FRAME_SLICE_U1_AREA_SUM
	{{192, 200, 3192, 4192},	"スライス面積総和白2[mm2]", 0},		// 38:FRAME_SLICE_U2_AREA_SUM
	{{193, 201, 3193, 4193},	"スライス面積総和白3[mm2]", 0},		// 39:FRAME_SLICE_U3_AREA_SUM
	{{218, 226, 3218, 4218},	"非零スライス個数黒0", 0},			// 40:FRAME_SLICE_L0_NUM
	{{219, 227, 3219, 4219},	"非零スライス個数黒1", 0},			// 41:FRAME_SLICE_L1_NUM
	{{220, 228, 3220, 4220},	"非零スライス個数黒2", 0},			// 42:FRAME_SLICE_L2_NUM
	{{221, 229, 3221, 4221},	"非零スライス個数黒3", 0},			// 43:FRAME_SLICE_L3_NUM
	{{222, 230, 3222, 4222},	"非零スライス個数白0", 0},			// 44:FRAME_SLICE_U0_NUM
	{{223, 231, 3223, 4223},	"非零スライス個数白1", 0},			// 45:FRAME_SLICE_U1_NUM
	{{224, 232, 3224, 4224},	"非零スライス個数白2", 0},			// 46:FRAME_SLICE_U2_NUM
	{{225, 233, 3225, 4225},	"非零スライス個数白3", 0},			// 47:FRAME_SLICE_U3_NUM
	{{234, 243, 3234, 4234},	"疵部ヒスト特徴1", 0},				// 48:HIST_0
	{{235, 244, 3235, 4235},	"疵部ヒスト特徴2", 0},				// 49:HIST_1
	{{236, 245, 3236, 4236},	"疵部ヒスト特徴3", 0},				// 50:HIST_2
	{{237, 246, 3237, 4237},	"疵部ヒスト特徴4", 0},				// 51:HIST_3
	{{238, 247, 3238, 4238},	"疵部ヒスト特徴5", 0},				// 52:HIST_4
	{{239, 248, 3239, 4239},	"疵部ヒストピーク輝度", 0},			// 53:HIST_PEAK_POS
	{{240, 249, 3240, 4240},	"疵部ヒストピーク画素数", 0},		// 54:HIST_PEAK_VAL
	{{241, 250, 3241, 4241},	"疵部ヒスト1/2幅", 0},				// 55:HIST_WIDTH_2
	{{242, 251, 3242, 4242},	"疵部ヒスト1/5幅", 0},				// 56:HIST_WIDTH_5
	{{252, 261, 3252, 4252},	"外接ヒスト特徴1", 0},				// 57:HIST_BOX_0
	{{253, 262, 3253, 4253},	"外接ヒスト特徴2", 0},				// 58:HIST_BOX_1
	{{254, 263, 3254, 4254},	"外接ヒスト特徴3", 0},				// 59:HIST_BOX_2
	{{255, 264, 3255, 4255},	"外接ヒスト特徴4", 0},				// 60:HIST_BOX_3
	{{256, 265, 3256, 4256},	"外接ヒスト特徴5", 0},				// 61:HIST_BOX_4
	{{257, 266, 3257, 4257},	"外接ヒストピーク輝度", 0},			// 62:HIST_BOX_PEAK_POS
	{{258, 267, 3258, 4258},	"外接ヒストピーク画素数", 0},		// 63:HIST_BOX_PEAK_VAL
	{{259, 268, 3259, 4259},	"外接ヒスト1/2幅", 0},				// 64:HIST_BOX_WIDTH_2
	{{260, 269, 3260, 4260},	"外接ヒスト1/5幅", 0},				// 65:HIST_BOX_WIDTH_5
	{{317, 321, 3317, 4317},	"うねり縦位置σ", 0},				// 66:WIND_BOX_POS_VERT
	{{318, 322, 3318, 4318},	"うねり縦輝度σ", 0},				// 67:WIND_BOX_PIXEL_VERT
	{{319, 323, 3319, 4319},	"うねり横位置σ", 0},				// 68:WIND_BOX_POS_HORZ
	{{320, 324, 3320, 4320},	"うねり横輝度σ", 0},				// 69:WIND_BOX_PIXEL_HORZ
	{{270, 271, 3270, 4270},	"生画像フレーム内平均輝度", 0},		// 70:RAW_FRAME_BRIGHTNESS
	{{274, 275, 3274, 4274},	"輝度２乗和", 0},					// 71:PIXEL_SQSUM
	{{283, 291, 3283, 4283},	"スライス長さ黒0", 0},				// 72:LENGTH_L0
	{{284, 292, 3284, 4284},	"スライス長さ黒1", 0},				// 73:LENGTH_L1
	{{285, 293, 3285, 4285},	"スライス長さ黒2", 0},				// 74:LENGTH_L2
	{{286, 294, 3286, 4286},	"スライス長さ黒3", 0},				// 75:LENGTH_L3
	{{287, 295, 3287, 4287},	"スライス長さ白0", 0},				// 76:LENGTH_U0
	{{288, 296, 3288, 4288},	"スライス長さ白1", 0},				// 77:LENGTH_U1
	{{289, 297, 3289, 4289},	"スライス長さ白2", 0},				// 78:LENGTH_U2
	{{290, 298, 3290, 4290},	"スライス長さ白3", 0},				// 79:LENGTH_U3
	{{299, 307, 3299, 4299},	"スライス幅黒0", 0},				// 80:WIDTH_L0
	{{300, 308, 3300, 4300},	"スライス幅黒1", 0},				// 81:WIDTH_L1
	{{301, 309, 3301, 4301},	"スライス幅黒2", 0},				// 82:WIDTH_L2
	{{302, 310, 3302, 4302},	"スライス幅黒3", 0},				// 83:WIDTH_L3
	{{303, 311, 3303, 4303},	"スライス幅白0", 0},				// 84:WIDTH_U0
	{{304, 312, 3304, 4304},	"スライス幅白1", 0},				// 85:WIDTH_U1
	{{305, 313, 3305, 4305},	"スライス幅白2", 0},				// 86:WIDTH_U2
	{{306, 314, 3306, 4306},	"スライス幅白3", 0},				// 87:WIDTH_U3
	{{89,   90, 3089, 4089},	"疵中心カメラビット位置", 0},		// 88:CAM_BITPOS
	{{334, 338, 3334, 4334},	"生画像最小輝度", 0},				// 89:RAW_MIN_PIXEL
	{{335, 339, 3335, 4335},	"生画像最大輝度", 0},				// 90:RAW_MAX_PIXEL
	{{336, 340, 3336, 4336},	"生画像平均輝度", 0},				// 91:RAW_MEAN_PIXEL
	{{337, 341, 3337, 4337},	"生画像標準偏差", 0},				// 92:RAW_SIGMA_PIXEL
	{{342, 343, 3342, 4342},	"フレーム内輝度標準偏差", 0},		// 93:SIGMA_FRAME
	{{344, 345, 3344, 4344},	"生画像フレーム内輝度標準偏差", 0},	// 94:RAW_SIGMA_FRAME
	{{158, 159, 3158, 4158},	"生画像縦平均左右差", 0},			// 95:RAW_PROJ_HDIFF
	{{346, 350, 3346, 4346},	"上辺輝度", 0},						// 96:TOP_EDGE_PIXEL
	{{347, 351, 3347, 4347},	"下辺輝度", 0},						// 97:BOTTOM_EDGE_PIXEL
	{{348, 352, 3348, 4348},	"左辺輝度", 0},						// 98:LEFT_EDGE_PIXEL
	{{349, 353, 3349, 4349},	"右辺輝度", 0},						// 99:RIGHT_EDGE_PIXEL
	{{354, 358, 3354, 4354},	"生画像上辺輝度", 0},				// 100:RAW_TOP_EDGE_PIXEL
	{{355, 359, 3355, 4355},	"生画像下辺輝度", 0},				// 101:RAW_BOTTOM_EDGE_PIXEL
	{{356, 360, 3356, 4356},	"生画像左辺輝度", 0},				// 102:RAW_LEFT_EDGE_PIXEL
	{{357, 361, 3357, 4357},	"生画像右辺輝度", 0},				// 103:RAW_RIGHT_EDGE_PIXEL
	{{362, 363, 3362, 4362},	"生画像輝度２乗和", 0},				// 104:RAW_PIXEL_SQSUM
	{{364, 367, 3364, 4364},	"生画像横プロジェクション段差", 0},	// 105:RAW_HPROJ_STEP
	{{365, 368, 3365, 4365},	"画像ズレ量Y", 0},					// 106:NGC_OFFSET_Y
	{{366, 369, 3366, 4366},	"画像ズレ相関係数", 0},				// 107:NGC_MAX_VALUE
	{{370, 374, 3370, 4370},	"最大輝度X座標[pixel]", 0},			// 108:MAX_PIXEL_X_POS
	{{371, 375, 3371, 4371},	"最大輝度Y座標[pixel]", 0},			// 109:MAX_PIXEL_Y_POS
	{{372, 376, 3372, 4372},	"最小輝度X座標[pixel]", 0},			// 110:MIN_PIXEL_X_POS
	{{373, 377, 3373, 4373},	"最小輝度Y座標[pixel]", 0},			// 111:MIN_PIXEL_Y_POS
	{{378, 386, 3378, 4378},	"縦proj第一正ピーク", 0},			// 112:VP_1ST_POS_PEAK
	{{379, 387, 3379, 4379},	"縦proj第二正ピーク", 0},			// 113:VP_2ND_POS_PEAK
	{{380, 388, 3380, 4380},	"縦proj正ピーク間距離[pixel]", 0},	// 114:VP_POS_PEAK_GAP
	{{381, 389, 3381, 4381},	"縦proj正ピーク間最小", 0},			// 115:VP_POS_BOTTOM
	{{382, 390, 3382, 4382},	"縦proj第一負ピーク", 0},			// 116:VP_1ST_NEG_PEAK
	{{383, 391, 3383, 4383},	"縦proj第二負ピーク", 0},			// 117:VP_2ND_NEG_PEAK
	{{384, 392, 3384, 4384},	"縦proj負ピーク間距離[pixel]", 0},	// 118:VP_NEG_PEAK_GAP
	{{385, 393, 3385, 4385},	"縦proj負ピーク間最大", 0}			// 119:VP_NEG_BOTTOM

};


#endif
