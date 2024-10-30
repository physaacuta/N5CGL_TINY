// NCattr.h
// ncdetect
// �����ʂ̒�`
// EPC�@�v���E����J�����@����
// 2007.8
// 2008.4 �����������J�����E4�ቻ�Ή�

#ifndef	_NCATTR_H
#define _NCATTR_H

// ���ʓ����ʔԍ��̒�`
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
/*09*/	FERET_X,				// EDASYS�}�b�v�ɕK�v
/*10*/	FERET_Y,				// EDASYS�}�b�v�ɕK�v
/*11*/	CENTER_X,				// EDASYS�}�b�v�ɕK�v
/*12*/	CENTER_Y,				// EDASYS�}�b�v�ɕK�v
/*13*/	BOX_ASPECT,
/*14*/	FRAME_POS,
/*15*/	POS_FROM_TAIL,
/*16*/	HORZ_RES,				// EDASYS�}�b�v�ɕK�v
/*17*/	VERT_RES,				// EDASYS�}�b�v�ɕK�v
/*18*/	LINE_SPEED,
/*19*/	MY_EDGE_POS,
/*20*/	L_EDGE_POS,
/*21*/	R_EDGE_POS,
/*22*/	CENTER_X_FROM_L_EDGE,
/*23*/	CENTER_X_FROM_R_EDGE,
/*24*/	FRAME_NUM,
/*25*/  BLOB_NUM,
/*26*/	MERGED_CENTER_X,		// EDASYS�}�b�v�ɕK�v
/*27*/	MERGED_CENTER_Y,		// EDASYS�}�b�v�ɕK�v
/*28*/	FERET_X_PIXEL,			
/*29*/	FERET_Y_PIXEL,
/*30*/	BOX_ASPECT_PIXEL,
/*31*/	AREA_PIXEL,
/*32*/	PERIMETER_PIXEL,
/*33*/	ROUNDNESS_PIXEL,
/*34*/	IMAGE_X,				// EDASYS�}�b�v�ɕK�v
/*35*/	IMAGE_Y,				// EDASYS�}�b�v�ɕK�v
/*36*/	CYCLE_LENGTH,			// DetectProc�ł͑��݂��Ȃ�
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

/*135*/ CYCLE_NMULTI, // ncdetect_pd�̂�
/*136*/ ROLL_ONOFF,   // ncdetect_pd�̂�
/*137*/ SYNCADD_INDEX,       // ncdetect_pd�̂�
/*138*/ SYNCADD_PIX_MAX,     // ncdetect_pd�̂�
/*139*/ SYNCADD_PIX_MIN,     // ncdetect_pd�̂�
/*140*/ SYNCADD_PIX_MAX_AVG, // ncdetect_pd�̂�
/*141*/ SYNCADD_PIX_MIN_AVG, // ncdetect_pd�̂�

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
// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
///*164*/ LAST_COMMON_ATTR

// �G�b�W����
/*166*/ LABELING_TYPE,
// �����r�ǉ��� //
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
/*183*/ LONGMERGED_FERET_X,     // mm�P��
/*184*/ LONGMERGED_FERET_Y, // mm�P��
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
// 2023.09.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------

};

// �J�����p�x�ʂ̓����ʔԍ��̒�`
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

struct nc_common_attr {			// ���ʓ����ʂ̍\����
	long	edanum;		// Edasys ID
	const char	*const name;		// ����
	long	stage;		// 0:�ŏ��̃��W�b�N�e�[�u���Ŏg��, 1:�����r�̃��W�b�N�e�[�u���Ŏg��
};

const struct nc_common_attr nc_common_attr_table[] = {
	{60  , "�r��ID"                                       , 1}, // 0:DEFECT_TYPE
	{61  , "�O���[�hID"                                   , 1}, // 1:DEFECT_GRADE
	{62  , "�t���O"                                       , 1}, // 2:DEFECT_FLAG

	{53  , "���x���ԍ�"                                   , 0}, // 3:LABEL_VALUE
	{43  , "�ʐ�[mm2]"                                    , 0}, // 4:AREA
	{27  , "�O��Xmin[pixel]"                              , 0}, // 5:BOX_X_MIN
	{28  , "�O��Xmax[pixel]"                              , 0}, // 6:BOX_X_MAX
	{37  , "�O��Ymin[pixel]"                              , 0}, // 7:BOX_Y_MIN
	{38  , "�O��Ymax[pixel]"                              , 0}, // 8:BOX_Y_MAX
	{1   , "�O�ڕ�[mm]"                                   , 0}, // 9:FERET_X
	{2   , "�O�ڒ���[mm]"                                 , 0}, // 10:FERET_Y
	{3   , "�t���[�����r���SX[mm]"                        , 0}, // 11:CENTER_X
	{42  , "�t���[�����r���SY[mm]"                        , 0}, // 12:CENTER_Y
	{33  , "�O�ڏc����"                                   , 0}, // 13:BOX_ASPECT
	{50  , "�t���[������ʒu[mm]"                         , 0}, // 14:FRAME_POS
	{51  , "�R�C���c�蒷��[mm]"                           , 0}, // 15:POS_FROM_TAIL
	{86  , "������\[mm]"                                 , 0}, // 16:HORZ_RES
	{85  , "�c����\[mm]"                                 , 0}, // 17:VERT_RES
	{56  , "���C�����x[mm/s]"                             , 0}, // 18:LINE_SPEED
	{52  , "���Ȍ��o�G�b�W�ʒu[pixel]"                    , 0}, // 19:MY_EDGE_POS
	{54  , "���G�b�W�ʒu[mm]"                             , 0}, // 20:L_EDGE_POS
	{55  , "�E�G�b�W�ʒu[mm]"                             , 0}, // 21:R_EDGE_POS
	{35  , "���G�b�W����̋���[mm]"                       , 0}, // 22:CENTER_X_FROM_L_EDGE
	{36  , "�E�G�b�W����̋���[mm]"                       , 0}, // 23:CENTER_X_FROM_R_EDGE
	{57  , "�t���[���ԍ�"                                 , 0}, // 24:FRAME_NUM
	{58  , "���x�����O��"                               , 0}, // 25:BLOB_NUM

	{34  , "�J�������W�r���ʒuX[mm]"                      , 0}, // 26:MERGED_CENTER_X
	{44  , "�R�C���擪������r����ʒu[mm]"               , 0}, // 27:MERGED_CENTER_Y

	{11  , "�O�ڕ�[pixel]"                                , 0}, // 28:FERET_X_PIXEL
	{12  , "�O�ڒ���[pixel]"                              , 0}, // 29:FERET_Y_PIXEL
	{13  , "�s�N�Z���c����"                               , 0}, // 30:BOX_ASPECT_PIXEL
	{147 , "��f�P�ʖʐ�[pixel]"                          , 0}, // 31:AREA_PIXEL
	{150 , "��f�P�ʎ��͒�[pixel]"                        , 0}, // 32:PERIMETER_PIXEL
	{151 , "��f�P���r�ۂ�[pixel]"                        , 0}, // 33:ROUNDNESS_PIXEL

	{92  , "�t���[�����摜����X[mm]"                      , 0}, // 34:IMAGE_X
	{93  , "�t���[�����摜����Y[mm]"                      , 0}, // 35:IMAGE_Y

	{59  , "��������[mm]"                                 , 1}, // 36:CYCLE_LENGTH

	{45  , "�t���[�����ő��r�ʐϔ�"                       , 0}, // 37:FRAME_MAX_AREA_RATIO
	{46  , "�t���[���������r�ʐϔ�"                       , 0}, // 38:FRAME_MEAN_AREA_RATIO
	{47  , "�t���[�����ŏ��r�ʐϔ�"                       , 0}, // 39:FRAME_MIN_AREA_RATIO
	{63  , "�J�����Z�b�g�ԍ�"                             , 0}, // 40:CAMSET
	{88  , "�\������"                                     , 0}, // 41:TOP_OR_BOT
	{64  , "��l��臒l�g���"                             , 0}, // 42:THR_EXPAND

	{136 , "�t���[�����ő��r�ʐ�[mm2]"                    , 0}, // 43:FRAME_MAX_AREA
	{137 , "�t���[���������r�ʐ�[mm2]"                    , 0}, // 44:FRAME_MEAN_AREA
	{138 , "�t���[�����ŏ��r�ʐ�[mm2]"                    , 0}, // 45:FRAME_MIN_AREA

	{152 , "�d�S���[�����gXX[mm2]"                        , 0}, // 46:MOMENT_XX
	{153 , "�d�S���[�����gYY[mm2]"                        , 0}, // 47:MOMENT_YY
	{154 , "�d�S���[�����gXY[mm2]"                        , 0}, // 48:MOMENT_XY
	{155 , "�厲����[�x]"                                 , 0}, // 49:PRINCIPAL_ANGLE
	{156 , "�厲����[mm]"                                 , 0}, // 50:PRINCIPAL_LENGTH
	{157 , "��񎲒���[mm]"                               , 0}, // 51:SECONDARY_LENGTH


	{65  , "�����r�A��"                                   , 1}, // 52:CYCLE_SERIAL
	{66  , "�擪�����r����̋���[mm]"                     , 1}, // 53:CYCLE_CUM_LEN


	{67  , "�V���[�J�b�g����̋���[mm]"                   , 0}, // 54:POS_FROM_SHEAR_CUT
	{68  , "�r�O���[�vID"                                 , 1}, // 55:DEFECT_GROUP

	{401 , "�v�Z������1"                                  , 0}, // 56:CALCED_FEAT_1
	{402 , "�v�Z������2"                                  , 0}, // 57:CALCED_FEAT_2
	{403 , "�v�Z������3"                                  , 0}, // 58:CALCED_FEAT_3
	{404 , "�v�Z������4"                                  , 0}, // 59:CALCED_FEAT_4
	{405 , "�v�Z������5"                                  , 0}, // 60:CALCED_FEAT_5
	{406 , "�v�Z������6"                                  , 0}, // 61:CALCED_FEAT_6
	{407 , "�v�Z������7"                                  , 0}, // 62:CALCED_FEAT_7
	{408 , "�v�Z������8"                                  , 0}, // 63:CALCED_FEAT_8
	{409 , "�v�Z������9"                                  , 0}, // 64:CALCED_FEAT_9
	{410 , "�v�Z������10"                                 , 0}, // 65:CALCED_FEAT_10

	{272 , "�t���[�����d�S�ʒuX[mm]"                      , 0}, // 66:CG_X
	{273 , "�t���[�����d�S�ʒuY[mm]"                      , 0}, // 67:CG_Y
	{276 , "X���W�a[pixel]"                               , 0}, // 68:PIXEL_XSUM
	{277 , "Y���W�a[pixel]"                               , 0}, // 69:PIXEL_YSUM
	{278 , "X���W�Q��a[pixel]"                           , 0}, // 70:PIXEL_XXSUM
	{279 , "Y���W�Q��a[pixel]"                           , 0}, // 71:PIXEL_YYSUM
	{280 , "XY���W�Ϙa[pixel]"                            , 0}, // 72:PIXEL_XYSUM
	{281 , "��������Έʒu[mm]"                           , 0}, // 73:ABS_X_POS
	{282 , "�}�[�W��"                                   , 0}, // 74:MERGED_NUM

	{411 , "�R���g���X�g�ő�_X[pixel]"                   , 0}, // 75:MAX_CONTRAST_X_POS
	{412 , "�R���g���X�g�ő�_Y[pixel]"                   , 0}, // 76:MAX_CONTRAST_Y_POS
	{413 , "���G�b�W����R���g���X�g�ő�_�܂ł̋���[mm]" , 0}, // 77:MAX_CONTRAST_X_FROM_L_EDGE
	{414 , "�E�G�b�W����R���g���X�g�ő�_�܂ł̋���[mm]" , 0}, // 78:MAX_CONTRAST_X_FROM_R_EDGE
	{415 , "���g�p"                                       , 0}, // 79:FERET_Y_MULT_MAX_MIN_B
	{416 , "���g�p"                                       , 0}, // 80:FERET_Y_MULT_MAX_MIN_D
	{417 , "���g�p"                                       , 0}, // 81:FERET_Y_MULT_MAX_MIN_B_PLUS_MAX_MIN_D
	{418 , "���g�p"                                       , 0}, // 82:AREA_MULT_MAX_MIN_B
	{419 , "���g�p"                                       , 0}, // 83:AREA_MULT_MAX_MIN_D
	{420 , "���g�p"                                       , 0}, // 84:AREA_MULT_MAX_MIN_B_PLUS_MAX_MIN_D

	{325 , "���g�p"                                       , 0}, // 85:TINY_TOTAL
	{326 , "���g�p"                                       , 0}, // 86:TINY_WH_B_WH_D
	{327 , "���g�p"                                       , 0}, // 87:TINY_WH_B_BL_D
	{328 , "���g�p"                                       , 0}, // 88:TINY_BL_B_BL_D
	{329 , "���g�p"                                       , 0}, // 89:TINY_BL_B_WH_D
	{330 , "���g�p"                                       , 0}, // 90:TINY_WH_B
	{331 , "���g�p"                                       , 0}, // 91:TINY_BL_B
	{332 , "���g�p"                                       , 0}, // 92:TINY_WH_D
	{333 , "���g�p"                                       , 0}, // 93:TINY_BL_D

	{421 , "�v�Z������11"                                 , 0}, // 94:CALCED_FEAT_11
	{422 , "�v�Z������12"                                 , 0}, // 95:CALCED_FEAT_12
	{423 , "�v�Z������13"                                 , 0}, // 96:CALCED_FEAT_13
	{424 , "�v�Z������14"                                 , 0}, // 97:CALCED_FEAT_14
	{425 , "�v�Z������15"                                 , 0}, // 98:CALCED_FEAT_15
	{426 , "�v�Z������16"                                 , 0}, // 99:CALCED_FEAT_16
	{427 , "�v�Z������17"                                 , 0}, // 100:CALCED_FEAT_17
	{428 , "�v�Z������18"                                 , 0}, // 101:CALCED_FEAT_18
	{429 , "�v�Z������19"                                 , 0}, // 102:CALCED_FEAT_19
	{430 , "�v�Z������20"                                 , 0}, // 103:CALCED_FEAT_20
	{431 , "�v�Z������21"                                 , 0}, // 104:CALCED_FEAT_21
	{432 , "�v�Z������22"                                 , 0}, // 105:CALCED_FEAT_22
	{433 , "�v�Z������23"                                 , 0}, // 106:CALCED_FEAT_23
	{434 , "�v�Z������24"                                 , 0}, // 107:CALCED_FEAT_24
	{435 , "�v�Z������25"                                 , 0}, // 108:CALCED_FEAT_25
	{436 , "�v�Z������26"                                 , 0}, // 109:CALCED_FEAT_26
	{437 , "�v�Z������27"                                 , 0}, // 110:CALCED_FEAT_27
	{438 , "�v�Z������28"                                 , 0}, // 111:CALCED_FEAT_28
	{439 , "�v�Z������29"                                 , 0}, // 112:CALCED_FEAT_29
	{440 , "�v�Z������30"                                 , 0}, // 113:CALCED_FEAT_30
	{441 , "�v�Z������31"                                 , 0}, // 114:CALCED_FEAT_31
	{442 , "�v�Z������32"                                 , 0}, // 115:CALCED_FEAT_32
	{443 , "�v�Z������33"                                 , 0}, // 116:CALCED_FEAT_33
	{444 , "�v�Z������34"                                 , 0}, // 117:CALCED_FEAT_34
	{445 , "�v�Z������35"                                 , 0}, // 118:CALCED_FEAT_35
	{446 , "�v�Z������36"                                 , 0}, // 119:CALCED_FEAT_36
	{447 , "�v�Z������37"                                 , 0}, // 120:CALCED_FEAT_37
	{448 , "�v�Z������38"                                 , 0}, // 121:CALCED_FEAT_38
	{449 , "�v�Z������39"                                 , 0}, // 122:CALCED_FEAT_39
	{450 , "�v�Z������40"                                 , 0}, // 123:CALCED_FEAT_40
	{451 , "�v�Z������41"                                 , 0}, // 124:CALCED_FEAT_41
	{452 , "�v�Z������42"                                 , 0}, // 125:CALCED_FEAT_42
	{453 , "�v�Z������43"                                 , 0}, // 126:CALCED_FEAT_43
	{454 , "�v�Z������44"                                 , 0}, // 127:CALCED_FEAT_44
	{455 , "�v�Z������45"                                 , 0}, // 128:CALCED_FEAT_45
	{456 , "�v�Z������46"                                 , 0}, // 129:CALCED_FEAT_46
	{457 , "�v�Z������47"                                 , 0}, // 130:CALCED_FEAT_47
	{458 , "�v�Z������48"                                 , 0}, // 131:CALCED_FEAT_48
	{459 , "�v�Z������49"                                 , 0}, // 132:CALCED_FEAT_49
	{460 , "�v�Z������50"                                 , 0}, // 133:CALCED_FEAT_50
	{69  , "SCODE(�|��/�\�ʃO���[�v)"                     , 0}, // 134:FEAT_SCODE

	{700 , "�����r���d�x"                                 , 0}, // 135:CYCLE_NMULTI
	{701 , "����ONOFF"                                    , 0}, // 136:ROLL_ONOFF
	{702 , "�������Zindex"                                , 0}, // 137:SYNCADD_INDEX
	{703 , "�������Z�ő�P�x"                             , 0}, // 138:SYNCADD_PIX_MAX
	{704 , "�������Z�ŏ��P�x"                             , 0}, // 139:SYNCADD_PIX_MIN
	{705 , "�������Z�ő�P�x����"                         , 0}, // 140:SYNCADD_PIX_MAX_AVG
	{706 , "�������Z�ŏ��P�x����"                         , 0}, // 141:SYNCADD_PIX_MIN_AVG

	{70  , "���i�����r��ID"                             , 1}, // 142:FIRST_CLASSIFIED_TYPE
	{102 , "X0����̋���[mm]"                             , 0}, // 143:POS_FROM_X0
	{103 , "X1����̋���[mm]"                             , 0}, // 144:POS_FROM_X1
	{104 , "X2����̋���[mm]"                             , 0}, // 145:POS_FROM_X2
	{105 , "X3����̋���[mm]"                             , 0}, // 146:POS_FROM_X3
	{106 , "���C�����x[mpm]"                              , 0}, // 147:LINE_SPEED_MPM
	{107 , "��l�c�v���W�F�N�V������[pixel]"              , 0}, // 148:BIN_VERT_PROJ
	{108 , "��l���v���W�F�N�V������[pixel]"              , 0}, // 149:BIN_HORZ_PROJ
	{109 , "�t���[��������"                               , 1}, // 150:SORT_INDEX
	{71  , "�J�����Z�b�g���r�ԍ�"                         , 0}, // 151:SERIAL_NUMBER_IN_COIL
	{72  , "�Ǐ��r���x[��/�O���b�h]"                      , 0}, // 152:LOCAL_DENSITY
	{73  , "���d����\���"                               , 0}, // 153: THRESHOLD_TYPE <= OFFSET_Y_TO_OPPOSITE
	{74  , "���Ζ��r�J�����Z�b�g"                         , 1}, // 154:OPPSITE_CAMSET
	{75  , "���Ζ��r�ԍ�"                                 , 1}, // 155:OPPOSITE_SERIAL_NUMBER
	{76  , "���Ζ��r��ID"                                 , 1}, // 156:OPPOSITE_TYPE
	{77  , "���Ζ��r�O���[�hID"                           , 1}, // 157:OPPOSITE_GRADE

	{5023, "�ŏ��^���[��"                                 , 0}, // 158:MIN_DEPTH
	{5022, "�ő�^���[��"                                 , 0}, // 159:MAX_DEPTH
	{5024, "���ϋ^���[��[mm]"                             , 0}, // 160:MEAN_DEPTH
	{5025, "�^���[���W���΍�[mm]"                         , 0}, // 161:SIGMA_DEPTH
	{5026, "�ő�^���[���|�ŏ��^���[��[mm]"               , 0}, // 162:MAX_MIN_DEPTH
	{5274, "�^���[���Q��a[mm^2]"                         , 0}, // 163:SQSUM_DEPTH
	{78  , "���Ζ��rX�΍�[mm]"                            , 1}, // 164:OFFSET_X_TO_OPPOSITE
	{79  , "���Ζ��rY�΍�[mm]"                            , 1}, // 165:OFFSET_Y_TO_OPPOSITE
// 2023.07.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
	{80,    "��l�����",0 }     ,                  // 166:LABELING_TYPE,
// �����r�ǉ���
    {900,   "���i����O���[�h", 1},               // 167:FIRST_CLASSIFIED_GRADE,
    {901,   "�t���[���ԘA���t���O", 0},             // 168:IFMERGE_FLAG,
    {902,   "�A���㔻���r��", 1},                   // 169:LONGMERGED_TYPE,
    {903,   "�A���㔻��O���[�h", 1},               // 170:LONGMERGED_GRADE,
    {904,   "�A���r���ő�O���[�h", 1},             // 171:LONGMERGED_MAXGRADE,
    {905,   "�A���r���ŏ��O���[�h", 1},             // 172:LONGMERGED_MINGRADE,
    {906,   "�A���r���ő��r��", 1},                 // 173:LONGMERGED_MAXTYPE,
    {907,   "�A���r���ŏ��r��", 1},                 // 174:LONGMERGED_MINTYPE,
    {908,   "�A����O��Xmin[pixel]", 1},            // 175:LONGMERGED_BOX_X_MIN,
    {909,   "�A����O��Xmax[pixel]", 1},            // 176:LONGMERGED_BOX_X_MAX,
    {910,   "�A����O��Ymin[pixel]", 1},            // 177:LONGMERGED_BOX_Y_MIN,
    {911,   "�A����O��Ymax[pixel]", 1},            // 178:LONGMERGED_BOX_Y_MAX,
    {912,   "�A�����f�P�ʖʐ�[pixel]", 1},        // 179:LONGMERGED_AREA_PIXEL,
    {913,   "�A����O�ڕ�[pixel]", 1},              // 180:LONGMERGED_FERET_X_PIXEL,
    {914,   "�A����O�ڒ���[pixel]", 1},            // 181:LONGMERGED_FERET_Y_PIXEL,
    {915,   "�A����s�N�Z���c����", 1},             // 182:LONGMERGED_BOX_ASPECT_PIXEL,
    {916,   "�A����O�ڕ�[mm]", 1},                 // 183:LONGMERGED_FERET_X,     // mm�P��
    {917,   "�A����O�ڒ���[mm]", 1},               // 184:LONGMERGED_FERET_Y,		// mm�P��
    {918,   "�A����ʐ�[mm2]", 1},                  // 185:LONGMERGED_AREA,
    {919,   "�A����O�ڏc����", 1},                 // 186:LONGMERGED_BOX_ASPECT,
    {920,   "�A����t���[�����r���SX[mm]", 1},      // 187:LONGMERGED_CENTER_X,
    {921,   "�A����t���[�����r���SY[mm]", 1},      // 188:LONGMERGED_CENTER_Y,
    {922,   "�A���㕝������Έʒu[mm]", 1},         // 189:LONGMERGED_ABS_X_POS,
    {923,   "�A���㍶�G�b�W����̋���[mm]", 1},     // 190:LONGMERGED_CENTER_X_FROM_L_EDGE,
    {924,   "�A����E�G�b�W����̋���[mm]", 1},     // 191:LONGMERGED_CENTER_X_FROM_R_EDGE,
// 2023.07.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------
};

static const char * angle_suffix[4] = {"(G)", "(B)", "(R)", "(R-B)"};

struct nc_angle_attr {
	long	edanum[4];
	const char	*const basename;
	long	stage;
};

const struct nc_angle_attr nc_angle_attr_table[] = {
	{{23, 14, 3023, 4023},		"�ŏ��P�x",				0},			// 00:MIN_PIXEL
	{{22, 15, 3022, 4022},		"�ő�P�x",				0},			// 01:MAX_PIXEL
	{{24, 16, 3024, 4024},		"���ϋP�x",				0},			// 02:MEAN_PIXEL
	{{25, 17, 3025, 4025},		"�P�x�W���΍�",			0},			// 03:SIGMA_PIXEL
	{{26, 18, 3026, 4026},		"�ő�P�x�|�ŏ��P�x",	0},			// 04:MAX_MIN
	{{110, 118, 3110, 4110},	"�X���C�X������0",		0},			// 05:SLICE_L0
	{{111, 119, 3111, 4111},	"�X���C�X������1",		0},			// 06:SLICE_L1
	{{112, 120, 3112, 4112},	"�X���C�X������2",		0},			// 07:SLICE_L2
	{{113, 121, 3113, 4113},	"�X���C�X������3",		0},			// 08:SLICE_L3
	{{114, 122, 3114, 4114},	"�X���C�X������0",		0},			// 09:SLICE_U0
	{{115, 123, 3115, 4115},	"�X���C�X������1",		0},			// 10:SLICE_U1
	{{116, 124, 3116, 4116},	"�X���C�X������2",		0},			// 11:SLICE_U2
	{{117, 125, 3117, 4117},	"�X���C�X������3",		0},			// 12:SLICE_U3
	{{130, 133, 3130, 4130},	"�c�v���W�F�N�V�����ŏ�", 0},		// 13:VP_MIN
	{{131, 134, 3131, 4131},	"�c�v���W�F�N�V�����ő�", 0},		// 14:VP_MAX
	{{132, 135, 3132, 4132},	"�c�v���W�F�N�V��������", 0},		// 15:VP_MEAN
	{{139, 142, 3139, 4139},	"�t���[�����ŏ��P�x����", 0},		// 16:FRAME_MIN_PIXEL
	{{140, 143, 3140, 4140},	"�t���[�����ő�P�x����", 0},		// 17:FRAME_MAX_PIXEL
	{{141, 144, 3141, 4141},	"�t���[�������ϋP�x����", 0},		// 18:FRAME_MEAN_PIXEL
	{{160, 168, 3160, 4160},	"�X���C�X�ʐύ�0[mm2]", 0},			// 19:SLICE_L0_AREA
	{{161, 169, 3161, 4161},	"�X���C�X�ʐύ�1[mm2]", 0},			// 20:SLICE_L1_AREA
	{{162, 170, 3162, 4162},	"�X���C�X�ʐύ�2[mm2]", 0},			// 21:SLICE_L2_AREA
	{{163, 171, 3163, 4163},	"�X���C�X�ʐύ�3[mm2]", 0},			// 22:SLICE_L3_AREA
	{{164, 172, 3164, 4164},	"�X���C�X�ʐϔ�0[mm2]", 0},			// 23:SLICE_U0_AREA
	{{165, 173, 3165, 4165},	"�X���C�X�ʐϔ�1[mm2]", 0},			// 24:SLICE_U1_AREA
	{{166, 174, 3166, 4166},	"�X���C�X�ʐϔ�2[mm2]", 0},			// 25:SLICE_U2_AREA
	{{167, 175, 3167, 4167},	"�X���C�X�ʐϔ�3[mm2]", 0},			// 26:SLICE_U3_AREA
	{{176, 179, 3176, 4176},	"���v���W�F�N�V�����ŏ�", 0},		// 27:HP_MIN
	{{177, 180, 3177, 4177},	"���v���W�F�N�V�����ő�", 0},		// 28:HP_MAX
	{{178, 181, 3178, 4178},	"���v���W�F�N�V��������", 0},		// 29:HP_MEAN
	{{182, 183, 3182, 4182},	"�c�v���W�F�N�V�����ő�|�ŏ�", 0},	// 30:VP_MAX_MIN
	{{184, 185, 3184, 4184},	"���v���W�F�N�V�����ő�|�ŏ�", 0},	// 31:HP_MAX_MIN
	{{186, 194, 3186, 4186},	"�X���C�X�ʐϑ��a��0[mm2]", 0},		// 32:FRAME_SLICE_L0_AREA_SUM
	{{187, 195, 3187, 4187},	"�X���C�X�ʐϑ��a��1[mm2]", 0},		// 33:FRAME_SLICE_L1_AREA_SUM
	{{188, 196, 3188, 4188},	"�X���C�X�ʐϑ��a��2[mm2]", 0},		// 34:FRAME_SLICE_L2_AREA_SUM
	{{189, 197, 3189, 4189},	"�X���C�X�ʐϑ��a��3[mm2]", 0},		// 35:FRAME_SLICE_L3_AREA_SUM
	{{190, 198, 3190, 4190},	"�X���C�X�ʐϑ��a��0[mm2]", 0},		// 36:FRAME_SLICE_U0_AREA_SUM
	{{191, 199, 3191, 4191},	"�X���C�X�ʐϑ��a��1[mm2]", 0},		// 37:FRAME_SLICE_U1_AREA_SUM
	{{192, 200, 3192, 4192},	"�X���C�X�ʐϑ��a��2[mm2]", 0},		// 38:FRAME_SLICE_U2_AREA_SUM
	{{193, 201, 3193, 4193},	"�X���C�X�ʐϑ��a��3[mm2]", 0},		// 39:FRAME_SLICE_U3_AREA_SUM
	{{218, 226, 3218, 4218},	"���X���C�X����0", 0},			// 40:FRAME_SLICE_L0_NUM
	{{219, 227, 3219, 4219},	"���X���C�X����1", 0},			// 41:FRAME_SLICE_L1_NUM
	{{220, 228, 3220, 4220},	"���X���C�X����2", 0},			// 42:FRAME_SLICE_L2_NUM
	{{221, 229, 3221, 4221},	"���X���C�X����3", 0},			// 43:FRAME_SLICE_L3_NUM
	{{222, 230, 3222, 4222},	"���X���C�X����0", 0},			// 44:FRAME_SLICE_U0_NUM
	{{223, 231, 3223, 4223},	"���X���C�X����1", 0},			// 45:FRAME_SLICE_U1_NUM
	{{224, 232, 3224, 4224},	"���X���C�X����2", 0},			// 46:FRAME_SLICE_U2_NUM
	{{225, 233, 3225, 4225},	"���X���C�X����3", 0},			// 47:FRAME_SLICE_U3_NUM
	{{234, 243, 3234, 4234},	"�r���q�X�g����1", 0},				// 48:HIST_0
	{{235, 244, 3235, 4235},	"�r���q�X�g����2", 0},				// 49:HIST_1
	{{236, 245, 3236, 4236},	"�r���q�X�g����3", 0},				// 50:HIST_2
	{{237, 246, 3237, 4237},	"�r���q�X�g����4", 0},				// 51:HIST_3
	{{238, 247, 3238, 4238},	"�r���q�X�g����5", 0},				// 52:HIST_4
	{{239, 248, 3239, 4239},	"�r���q�X�g�s�[�N�P�x", 0},			// 53:HIST_PEAK_POS
	{{240, 249, 3240, 4240},	"�r���q�X�g�s�[�N��f��", 0},		// 54:HIST_PEAK_VAL
	{{241, 250, 3241, 4241},	"�r���q�X�g1/2��", 0},				// 55:HIST_WIDTH_2
	{{242, 251, 3242, 4242},	"�r���q�X�g1/5��", 0},				// 56:HIST_WIDTH_5
	{{252, 261, 3252, 4252},	"�O�ڃq�X�g����1", 0},				// 57:HIST_BOX_0
	{{253, 262, 3253, 4253},	"�O�ڃq�X�g����2", 0},				// 58:HIST_BOX_1
	{{254, 263, 3254, 4254},	"�O�ڃq�X�g����3", 0},				// 59:HIST_BOX_2
	{{255, 264, 3255, 4255},	"�O�ڃq�X�g����4", 0},				// 60:HIST_BOX_3
	{{256, 265, 3256, 4256},	"�O�ڃq�X�g����5", 0},				// 61:HIST_BOX_4
	{{257, 266, 3257, 4257},	"�O�ڃq�X�g�s�[�N�P�x", 0},			// 62:HIST_BOX_PEAK_POS
	{{258, 267, 3258, 4258},	"�O�ڃq�X�g�s�[�N��f��", 0},		// 63:HIST_BOX_PEAK_VAL
	{{259, 268, 3259, 4259},	"�O�ڃq�X�g1/2��", 0},				// 64:HIST_BOX_WIDTH_2
	{{260, 269, 3260, 4260},	"�O�ڃq�X�g1/5��", 0},				// 65:HIST_BOX_WIDTH_5
	{{317, 321, 3317, 4317},	"���˂�c�ʒu��", 0},				// 66:WIND_BOX_POS_VERT
	{{318, 322, 3318, 4318},	"���˂�c�P�x��", 0},				// 67:WIND_BOX_PIXEL_VERT
	{{319, 323, 3319, 4319},	"���˂艡�ʒu��", 0},				// 68:WIND_BOX_POS_HORZ
	{{320, 324, 3320, 4320},	"���˂艡�P�x��", 0},				// 69:WIND_BOX_PIXEL_HORZ
	{{270, 271, 3270, 4270},	"���摜�t���[�������ϋP�x", 0},		// 70:RAW_FRAME_BRIGHTNESS
	{{274, 275, 3274, 4274},	"�P�x�Q��a", 0},					// 71:PIXEL_SQSUM
	{{283, 291, 3283, 4283},	"�X���C�X������0", 0},				// 72:LENGTH_L0
	{{284, 292, 3284, 4284},	"�X���C�X������1", 0},				// 73:LENGTH_L1
	{{285, 293, 3285, 4285},	"�X���C�X������2", 0},				// 74:LENGTH_L2
	{{286, 294, 3286, 4286},	"�X���C�X������3", 0},				// 75:LENGTH_L3
	{{287, 295, 3287, 4287},	"�X���C�X������0", 0},				// 76:LENGTH_U0
	{{288, 296, 3288, 4288},	"�X���C�X������1", 0},				// 77:LENGTH_U1
	{{289, 297, 3289, 4289},	"�X���C�X������2", 0},				// 78:LENGTH_U2
	{{290, 298, 3290, 4290},	"�X���C�X������3", 0},				// 79:LENGTH_U3
	{{299, 307, 3299, 4299},	"�X���C�X����0", 0},				// 80:WIDTH_L0
	{{300, 308, 3300, 4300},	"�X���C�X����1", 0},				// 81:WIDTH_L1
	{{301, 309, 3301, 4301},	"�X���C�X����2", 0},				// 82:WIDTH_L2
	{{302, 310, 3302, 4302},	"�X���C�X����3", 0},				// 83:WIDTH_L3
	{{303, 311, 3303, 4303},	"�X���C�X����0", 0},				// 84:WIDTH_U0
	{{304, 312, 3304, 4304},	"�X���C�X����1", 0},				// 85:WIDTH_U1
	{{305, 313, 3305, 4305},	"�X���C�X����2", 0},				// 86:WIDTH_U2
	{{306, 314, 3306, 4306},	"�X���C�X����3", 0},				// 87:WIDTH_U3
	{{89,   90, 3089, 4089},	"�r���S�J�����r�b�g�ʒu", 0},		// 88:CAM_BITPOS
	{{334, 338, 3334, 4334},	"���摜�ŏ��P�x", 0},				// 89:RAW_MIN_PIXEL
	{{335, 339, 3335, 4335},	"���摜�ő�P�x", 0},				// 90:RAW_MAX_PIXEL
	{{336, 340, 3336, 4336},	"���摜���ϋP�x", 0},				// 91:RAW_MEAN_PIXEL
	{{337, 341, 3337, 4337},	"���摜�W���΍�", 0},				// 92:RAW_SIGMA_PIXEL
	{{342, 343, 3342, 4342},	"�t���[�����P�x�W���΍�", 0},		// 93:SIGMA_FRAME
	{{344, 345, 3344, 4344},	"���摜�t���[�����P�x�W���΍�", 0},	// 94:RAW_SIGMA_FRAME
	{{158, 159, 3158, 4158},	"���摜�c���ύ��E��", 0},			// 95:RAW_PROJ_HDIFF
	{{346, 350, 3346, 4346},	"��ӋP�x", 0},						// 96:TOP_EDGE_PIXEL
	{{347, 351, 3347, 4347},	"���ӋP�x", 0},						// 97:BOTTOM_EDGE_PIXEL
	{{348, 352, 3348, 4348},	"���ӋP�x", 0},						// 98:LEFT_EDGE_PIXEL
	{{349, 353, 3349, 4349},	"�E�ӋP�x", 0},						// 99:RIGHT_EDGE_PIXEL
	{{354, 358, 3354, 4354},	"���摜��ӋP�x", 0},				// 100:RAW_TOP_EDGE_PIXEL
	{{355, 359, 3355, 4355},	"���摜���ӋP�x", 0},				// 101:RAW_BOTTOM_EDGE_PIXEL
	{{356, 360, 3356, 4356},	"���摜���ӋP�x", 0},				// 102:RAW_LEFT_EDGE_PIXEL
	{{357, 361, 3357, 4357},	"���摜�E�ӋP�x", 0},				// 103:RAW_RIGHT_EDGE_PIXEL
	{{362, 363, 3362, 4362},	"���摜�P�x�Q��a", 0},				// 104:RAW_PIXEL_SQSUM
	{{364, 367, 3364, 4364},	"���摜���v���W�F�N�V�����i��", 0},	// 105:RAW_HPROJ_STEP
	{{365, 368, 3365, 4365},	"�摜�Y����Y", 0},					// 106:NGC_OFFSET_Y
	{{366, 369, 3366, 4366},	"�摜�Y�����֌W��", 0},				// 107:NGC_MAX_VALUE
	{{370, 374, 3370, 4370},	"�ő�P�xX���W[pixel]", 0},			// 108:MAX_PIXEL_X_POS
	{{371, 375, 3371, 4371},	"�ő�P�xY���W[pixel]", 0},			// 109:MAX_PIXEL_Y_POS
	{{372, 376, 3372, 4372},	"�ŏ��P�xX���W[pixel]", 0},			// 110:MIN_PIXEL_X_POS
	{{373, 377, 3373, 4373},	"�ŏ��P�xY���W[pixel]", 0},			// 111:MIN_PIXEL_Y_POS
	{{378, 386, 3378, 4378},	"�cproj��ꐳ�s�[�N", 0},			// 112:VP_1ST_POS_PEAK
	{{379, 387, 3379, 4379},	"�cproj��񐳃s�[�N", 0},			// 113:VP_2ND_POS_PEAK
	{{380, 388, 3380, 4380},	"�cproj���s�[�N�ԋ���[pixel]", 0},	// 114:VP_POS_PEAK_GAP
	{{381, 389, 3381, 4381},	"�cproj���s�[�N�ԍŏ�", 0},			// 115:VP_POS_BOTTOM
	{{382, 390, 3382, 4382},	"�cproj��ꕉ�s�[�N", 0},			// 116:VP_1ST_NEG_PEAK
	{{383, 391, 3383, 4383},	"�cproj��񕉃s�[�N", 0},			// 117:VP_2ND_NEG_PEAK
	{{384, 392, 3384, 4384},	"�cproj���s�[�N�ԋ���[pixel]", 0},	// 118:VP_NEG_PEAK_GAP
	{{385, 393, 3385, 4385},	"�cproj���s�[�N�ԍő�", 0}			// 119:VP_NEG_BOTTOM

};


#endif
