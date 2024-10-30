'*******************************************************************************
'   φω選択定数定義
'
'   [Ver]
'       Ver.01  2016/06/09  初版
'
'   [メモ]
'       基本的に書き換え可(切り替えスイッチ用のみ)
'*******************************************************************************
Option Strict On

''' <summary>
''' φω選択定数定義
''' </summary>
''' <remarks></remarks>
Public Module KizuCommonSelect
    '///////////////////////////////////////////////////////////////////////////
    'スイッチ
    '///////////////////////////////////////////////////////////////////////////
    '-------------------------------------------------------
    '動作環境
    '-------------------------------------------------------
    'False:本番環境、True:テスト環境
#Const cLOCAL = False

    '-------------------------------------------------------
    '横展開用ライン区分
    '-------------------------------------------------------
    'LINEスイッチ(必ず1コのみ有効にする事)VBの場合モジュール毎に必要
#Const cLINE = "LINE_5CGL_TINY"

    '-------------------------------------------------------
    'φω設備関係
    '-------------------------------------------------------
    'カメラ設置位置 幅方向位置
    '#Const cNumCamPos = "NUM_CAM_POS_1"                         '面当りのカメラ位置数(C)
    '#Const cNumCamPos = "NUM_CAM_POS_2"                         '面当りのカメラ位置数(DS,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_3"                         '面当りのカメラ位置数(DS,C,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_4"                         '面当りのカメラ位置数(DS,D1,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_5"                         '面当りのカメラ位置数(DS,D1,C,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_6"                         '面当りのカメラ位置数(DS,D1,D2,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_7"                         '面当りのカメラ位置数(DS,D1,D2,C,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_8"                         '面当りのカメラ位置数(DS,D1,D2,D3,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_9"                         '面当りのカメラ位置数(DS,D1,D2,D3,C,W3,W2,W1,WS)
#Const cNumCamPos = "NUM_CAM_POS_10"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_11"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_12"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_13"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_14"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_15"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
    '#Const cNumCamPos = "NUM_CAM_POS_16"                        '面当りのカメラ位置数(DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)

    'カメラ設置 角度
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_1"                     'カメラ角度数(1=単眼)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_2"                     'カメラ角度数(2=正・乱)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_3"                     'カメラ角度数(3=正・乱・乱)
#Const cNumCamAngle = "NUM_CAM_ANGLE_4"                     'カメラ角度数(4=正・乱・乱・乱)
    '#Const cNumCamAngle = "NUM_CAM_ANGLE_5"                     'カメラ角度数(5=正・乱・乱・乱・乱)
    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    '動作区分
#If cLOCAL = False Then                                     '本番環境
    Public Const LOCAL As Boolean = False                                       '動作区分(本番)
#Else                                                       'テスト環境
    Public Const LOCAL As Boolean = True                                        '動作区分(テスト)
#End If

    '-------------------------------------------------------
    'カメラ設置位置 幅方向位置
    '-------------------------------------------------------
    Public Const NUM_CAM_POS_1 As Integer = 1
    Public Const NUM_CAM_POS_2 As Integer = 2
    Public Const NUM_CAM_POS_3 As Integer = 3
    Public Const NUM_CAM_POS_4 As Integer = 4
    Public Const NUM_CAM_POS_5 As Integer = 5
    Public Const NUM_CAM_POS_6 As Integer = 6
    Public Const NUM_CAM_POS_7 As Integer = 7
    Public Const NUM_CAM_POS_8 As Integer = 8
    Public Const NUM_CAM_POS_9 As Integer = 9
    Public Const NUM_CAM_POS_10 As Integer = 10
    Public Const NUM_CAM_POS_11 As Integer = 11
    Public Const NUM_CAM_POS_12 As Integer = 12
    Public Const NUM_CAM_POS_13 As Integer = 13
    Public Const NUM_CAM_POS_14 As Integer = 14
    Public Const NUM_CAM_POS_15 As Integer = 15
    Public Const NUM_CAM_POS_16 As Integer = 16

#If cNumCamPos = "NUM_CAM_POS_1" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_1
#ElseIf cNumCamPos = "NUM_CAM_POS_2" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_2
#ElseIf cNumCamPos = "NUM_CAM_POS_3" Then
	Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_3
#ElseIf cNumCamPos = "NUM_CAM_POS_4" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_4
#ElseIf cNumCamPos = "NUM_CAM_POS_5" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_5
#ElseIf cNumCamPos = "NUM_CAM_POS_6" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_6
#ElseIf cNumCamPos = "NUM_CAM_POS_7" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_7
#ElseIf cNumCamPos = "NUM_CAM_POS_8" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_8
#ElseIf cNumCamPos = "NUM_CAM_POS_9" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_9
#ElseIf cNumCamPos = "NUM_CAM_POS_10" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_10
#ElseIf cNumCamPos = "NUM_CAM_POS_11" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_11
#ElseIf cNumCamPos = "NUM_CAM_POS_12" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_12
#ElseIf cNumCamPos = "NUM_CAM_POS_13" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_13
#ElseIf cNumCamPos = "NUM_CAM_POS_14" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_14
#ElseIf cNumCamPos = "NUM_CAM_POS_15" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_15
#ElseIf cNumCamPos = "NUM_CAM_POS_16" Then
    Public Const NUM_CAM_POS As Integer = NUM_CAM_POS_16
#End If

    '-------------------------------------------------------
    'カメラ設置 角度
    '-------------------------------------------------------
    Public Const NUM_CAM_ANGLE_1 As Integer = 1
    Public Const NUM_CAM_ANGLE_2 As Integer = 2
    Public Const NUM_CAM_ANGLE_3 As Integer = 3
    Public Const NUM_CAM_ANGLE_4 As Integer = 4
    Public Const NUM_CAM_ANGLE_5 As Integer = 5

#If cNumCamAngle = "NUM_CAM_ANGLE_1" Then
	Public Const NUM_CAM_ANGLE as Integer =	NUM_CAM_ANGLE_1
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_2" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_2
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_3" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_3
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_4" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_4
#ElseIf cNumCamAngle = "NUM_CAM_ANGLE_5" Then
    Public Const NUM_CAM_ANGLE As Integer = NUM_CAM_ANGLE_5
#End If

    '-------------------------------------------------------
    '横展開用ID
    '-------------------------------------------------------
    Public Const LINE_ID_N5CGL_TINY As Integer = 0

    '現ラインID(VBでは、極力iniファイルのラインIDを使用する事)
    Public Const LINE_ID As Integer = LINE_ID_N5CGL_TINY

    '-------------------------------------------------------
    '管理Noの先頭の2桁コード
    '-------------------------------------------------------
    Public Const KIZUKEN_CODE As String = KIZUKEN_CODE_N5CGL_TINY

    '-------------------------------------------------------
    '工程コード
    '-------------------------------------------------------
    Public Const KOUTEI_CODE As String = KOUTEI_CODE_5CGL

	'// システム名（認証用）
    ' INIファイルでラインスイッチをさせたくなかったので、KizuCommonSelectからスイッチさせる

    Public Const ENABLE_USERCHECK As Boolean = True    ' false時、ユーザ認証無し(最終的には必ずOFF)


    Public Const SYSTEM_NAME As String = "N5CGL_TINY"

End Module
