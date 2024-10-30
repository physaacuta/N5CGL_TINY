'// *********************************************************************************
'//	外部インターフェース用ヘッダー
'//	[Ver]
'//		Ver.01    2012/07/19  新規作成
'//
'//	[メモ]
'//		基本的に、書き換え可
'// *********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices
Public Module OutInterface

	'//***************************************************************
	'// 設定可変定数 定義
	'//***************************************************************
	'//=======================================
	'// 設定定数 定義
	'//=======================================
	'//=======================================
	'// 横展開用ライン区分
	'//=======================================
	'// LINEスイッチ (必ず1コのみコントアウトする事) VBの場合モジュール毎に必要'
#Const cLINE = "LINE_5CGL_TINY"

	'//***************************************************************
	'// プロコン通信　定義
	'//***************************************************************
	'//=======================================
	'// プロコン定数
	'//=======================================
	Public Const PRO_SIZE_TC As Integer = 4						' トランザクションコード MID
    Public Const PRO_MAX_CUT_COUNT As Integer = 16              ' 最大カット回数
	Public Const PRO_SIZE_COIL_NO As Integer = 12				' コイルNo (入側、出側同じ)
	Public Const PRO_SIZE_ORDER_NO As Integer = 4				' 命令No

	Public Const PRO_SIZE_NEXTINF_CNT As Integer = 4			' 次工程コイル情報件数

	'//=======================================
	'// トランザクションコード
	'//=======================================
	Public Const PRO_TC_COIL As Integer   = 1000				' コイル情報伝文
	Public Const PRO_TC_SAISUN As Integer = 1001				' 採寸実績伝文
	Public Const PRO_TC_CUT As Integer    = 1002				' カット実績伝文
	Public Const PRO_TC_INIT As Integer   = 1003				' イニシャル伝文
	Public Const PRO_TC_ZISEKI As Integer = 2000                ' 検査実績伝文
    Public Const PRO_TC_HELS As Integer = -1                    ' ヘルシー伝文


    ' プロコン伝文サブヘッダー [0]
    Public Const PRO_HEAD_H_GYOUMU As Byte = &H0				' 業務伝文 又は 応答伝文(CGL)
	' プロコン伝文サブヘッダー [1]
	Public Const PRO_HEAD_L_OK As Byte = &H0					' 正常


	'//=======================================
	'// プロコン 構造体
	'//=======================================

	''各サイズ
	' 全部
    Public Const SIZE_PRO_TYPE_RESP As Integer = 2              ' レスポンス伝文サイズ
    Public Const SIZE_PRO_TYPE_COILINF As Integer = 512         ' コイル情報伝文サイズ
    Public Const SIZE_PRO_TYPE_SAISUN As Integer = 512          ' 採寸実績伝文サイズ
    Public Const SIZE_PRO_TYPE_CUT As Integer = 128             ' カット情報伝文サイズ
    Public Const SIZE_PRO_TYPE_INIT As Integer = 128            ' イニシャル伝文サイズ
    Public Const SIZE_PRO_TYPE_ZISEKI As Integer = 512          ' 検査実績伝文サイズ
    Public Const SIZE_PRO_TYPE_HELS As Integer = 8              ' ヘルシー伝文サイズ

    ' ヘッダー部
    Public Const SIZE_PRO_TYPE_HEAD As Integer = 8            ' ヘッダー部サイズ
    Public Const SIZE_PRO_TYPE_COMMON As Integer = 56         ' 共通情報サイズ

    ' データ部
    Public Const SIZE_PRO_TYPE_DATA_COILINF As Integer = SIZE_PRO_TYPE_COILINF - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_SAISUN As Integer = SIZE_PRO_TYPE_SAISUN - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_CUT As Integer = SIZE_PRO_TYPE_CUT - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_INIT As Integer = SIZE_PRO_TYPE_INIT - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_ZISEKI As Integer = SIZE_PRO_TYPE_ZISEKI - SIZE_PRO_TYPE_HEAD
    Public Const SIZE_PRO_TYPE_DATA_HELS As Integer = SIZE_PRO_TYPE_HELS

    ' その他
    Public Const SIZE_PRO_TYPE_KEY As Integer = PRO_SIZE_ORDER_NO + PRO_SIZE_COIL_NO
    Public Const SIZE_PRO_TYPE_NEXTINF As Integer = 24
    Public Const SIZE_PRO_NEXTINF_CNT As Integer    = 4			' 次工程コイル情報件数 
	Public Const SIZE_PRO_SAISUN_INF_CNT As Integer = 16
	Public Const SIZE_PRO_GRADE_INF As Integer      = 32
	
	' レスポンス伝文 (2byte)
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_RESP, pack:=1)> _
	Public Structure PRO_TYPE_RESP
		<VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
		Dim bHead() As Byte								' サブヘッダー

		'イニシャライズ
		Public Sub initialize()
			ReDim bHead(2 - 1)
		End Sub
	End Structure

    ' ヘッダー部 (8byte)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_HEAD, Pack:=1)>
    Public Structure PRO_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim bHead() As Byte                             ' サブヘッダー (ホントはここに入れたくなかったけど longが入っているのでバウンダリがズレル)

        Dim nLen As Short                               ' データ長 (byte単位)
        Dim nTC As Integer                              ' メッセージNo

        'イニシャライズ
        Public Sub initialize()
            ReDim bHead(2 - 1)
        End Sub

    End Structure

    ' 共通情報 (56byte)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_COMMON, Pack:=1)>
    Public Structure PRO_TYPE_COMMON

        Dim nKubun As Short                             ' 伝文区分 (0:業務伝文 1:応答伝文)	

        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)>
        Dim nDate() As Short                            ' 送信時刻 (年月日時分秒)
        Dim nGrandFlg As Short                          ' グラントフラグ (0:伝文の中身 正常 1:伝文の中身 異常)

        <VBFixedArray(40 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=40)>
        Dim cYobi() As Byte                             ' 予備

 
        'イニシャライズ
        Public Sub initialize()
            ReDim nDate(6 - 1)
            ReDim cYobi(40 - 1)
        End Sub

    End Structure

    ' キー情報(入側論理コイル単位のキー情報)
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_KEY, pack:=1)> _
	Public Structure PRO_TYPE_KEY
		<VBFixedArray(PRO_SIZE_ORDER_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_ORDER_NO)> _
		Dim cMeiNo() As Byte							' 命令No
        <VBFixedArray(PRO_SIZE_COIL_NO - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_COIL_NO)> _
        Dim cCoilNo() As Byte                           ' コイルNo

		'イニシャライズ
		Public Sub initialize()
			ReDim cMeiNo(PRO_SIZE_ORDER_NO - 1)
            ReDim cCoilNo(PRO_SIZE_COIL_NO - 1)
		End Sub
	End Structure


	' データ部
	' イニシャル伝文
	'''' <summary>
	'''' イニシャル伝文
	'''' </summary>
	'''' <remarks></remarks>
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_INIT, pack:=1)> _
	Public Structure PRO_TYPE_DATA_INIT

        Dim Common As PRO_TYPE_COMMON                   ' 共通情報

        <VBFixedArray(SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON)>
        Dim cYobi() As Byte                             ' コイルNo

        'イニシャライズ
        Public Sub initialize()
            Common.initialize()
            ReDim cYobi(SIZE_PRO_TYPE_DATA_INIT - SIZE_PRO_TYPE_COMMON - 1)
        End Sub
    End Structure

    ' コイル情報伝文
	' コイル情報
	'///////////////////
	' 入側論理コイル情報共通部 24 byte
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_NEXTINF, pack:=1)> _
	Public Structure NEXT_COIL

        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cMeiNo() As Byte                            ' 命令No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)> _
        Dim cCoilNo() As Byte                           ' コイルNo
		Dim nFrontLen As Integer						' 入側フロント端板長さ [mm]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
         Dim cKouteiCode() As Byte                      ' 自工程コード
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cYobi() As Byte                             ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cKouteiCode(2 - 1)
            ReDim cYobi(2 - 1)
        End Sub
    End Structure


    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_COILINF - SIZE_PRO_TYPE_COMMON - SIZE_PRO_TYPE_NEXTINF * SIZE_PRO_NEXTINF_CNT - 16, Pack:=1)>
    Public Structure PRO_TYPE_COIL
        Dim KeyInf As PRO_TYPE_KEY                      ' キーNo
        Dim nSendPoint As Integer                       ' 送信位置
        Dim nLenIn As Integer                           ' 入側論理コイル 長さ [m]
        Dim nAtuZai As Integer                          ' 材料板厚[μm] (出側)
        Dim nWidZai As Integer                          ' 材料板幅 [mm]
        Dim nAtuSei As Integer                          ' 注文板厚 [μm] (出側)
        Dim nWidSei As Integer                          ' 注文板幅 [mm]
        Dim nWidTrim As Integer                         ' 現工程トリム幅 [mm] (ノートリム時は、0)
        Dim nNobiSPM As Integer                         ' SPM伸び率 [10-2%]
        Dim nNobiTLV As Integer                         ' TLV伸び率 [10-2%]
        Dim nCutF As Integer                            ' 入側フロント端板長さ
        Dim nCutT As Integer                            ' 入側テール端板長さ
        Dim nSPMRoolKei_Up As Integer                   ' SPMワークロール径 (上) [10-1mm]
        Dim nSPMRoolKei_Down As Integer                 ' SPMワークロール径 (下) [10-1mm]

        <VBFixedArray(31 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=31)> _
        Dim nYobi() As Integer                          ' 予備
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cKenKijun_O() As Byte                       ' 検査基準 表
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cKenKijun_U() As Byte                       ' 検査基準 裏
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKurometo_Kind() As Byte                    ' クロメート 実績種類
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKurometo_Umu() As Byte                     ' クロメート 実績有無
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cHinCode() As Byte                          ' 品種
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiKind() As Byte                        ' メッキ種類
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSyuZai() As Byte                           ' 主材質
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cDifMaker() As Byte                         ' ディファレンシャルマーカー
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKenban() As Byte                           ' 原板区分
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiCycle() As Byte                       ' メッキサイクル
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cInLineSPM() As Byte                        ' レベラー インラインSPM
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKagakuToyu() As Byte                       ' 化学処理塗油
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cAtukkaSPM() As Byte                        ' オフラインSPM圧下率
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cMekkiKubun() As Byte                       ' メッキ区分
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cOmoteSyori1() As Byte                      ' 表面処理1桁目
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cOmoteSyori2() As Byte                      ' 表面処理2桁目
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cToyu() As Byte                             ' 塗油
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cKouteiYobi() As Byte                       ' 工程記号予備
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cBaund() As Byte                            ' バウンダリ合わせ
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim cCaplSyuZai() As Byte                       ' CAPL 主材質
        <VBFixedArray(8 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Dim cMetuke() As Byte                           ' 目付規格
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cNi() As Byte                               ' Ni付着量
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSodoTCM() As Byte                          ' TCMロール粗度
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSpmUmu() As Byte                           ' SPM圧下有無 (有り:O  無し:X)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cNextLine() As Byte                         ' 計画次工程コード
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim cMuke() As Byte                             ' 向先
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cYouto() As Byte                            ' 用途
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim cCaplSiage() As Byte                        ' CAPL表面仕上げ
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cCaplSanUmu() As Byte                       ' CAPL酸洗有無
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cHinMode() As Byte                          ' 品種区分
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim cGaCrKubun() As Byte                        ' GA/CR区分
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cSpmJisekiUmu() As Byte                     ' SPM圧下実績有無（有り：0 無し:X）
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim cKouteiCode() As Byte                       ' 工程コード
        <VBFixedArray(42 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=42)> _
        Dim cYobi1() As Byte                            '

        'イニシャライズ
        Public Sub initialize()
            KeyInf.initialize()
            ReDim nYobi(31 - 1)
            ReDim cKenKijun_O(4 - 1)
            ReDim cKenKijun_U(4 - 1)
            ReDim cKurometo_Kind(2 - 1)
            ReDim cKurometo_Umu(2 - 1)
            ReDim cHinCode(1 - 1)
            ReDim cMekkiKind(1 - 1)
            ReDim cSyuZai(2 - 1)
            ReDim cDifMaker(1 - 1)
            ReDim cKenban(1 - 1)
            ReDim cMekkiCycle(1 - 1)
            ReDim cOmoteSyori1(1 - 1)
            ReDim cOmoteSyori2(1 - 1)
            ReDim cToyu(1 - 1)
            ReDim cKouteiYobi(1 - 1)
            ReDim cBaund(1 - 1)
            ReDim cCaplSyuZai(10 - 1)
            ReDim cMetuke(8 - 1)
            ReDim cNi(2 - 1)
            ReDim cSodoTCM(2 - 1)
            ReDim cSpmUmu(2 - 1)
            ReDim cNextLine(2 - 1)
            ReDim cMuke(32 - 1)
            ReDim cYouto(4 - 1)
            ReDim cCaplSiage(4 - 1)
            ReDim cCaplSanUmu(2 - 1)
            ReDim cHinMode(1 - 1)
            ReDim cGaCrKubun(1 - 1)
            ReDim cSpmJisekiUmu(2 - 1)
            ReDim cKouteiCode(2 - 1)
            ReDim cYobi1(42 - 1)
        End Sub
    End Structure

    '''' <summary>
    '''' 検査用コイル情報伝文
    '''' </summary>
    '''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_COILINF, Pack:=1)>
    Public Structure PRO_TYPE_DATA_COILINF
        Dim Common As PRO_TYPE_COMMON                   ' 共通情報

        Dim CoilInf As PRO_TYPE_COIL                    ' コイル情報

        <VBFixedArray(PRO_SIZE_NEXTINF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_SIZE_NEXTINF_CNT)>
        Dim typInCoil() As NEXT_COIL                    ' 入側論理コイル情報

        <VBFixedArray(16 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=16)>
        Dim cYobi() As Byte                             ' 予備

        'イニシャライズ
        Public Sub initialize()
            Common.initialize()
            CoilInf.initialize()
            ReDim typInCoil(PRO_SIZE_NEXTINF_CNT - 1)
            For ii As Integer = 0 To PRO_SIZE_NEXTINF_CNT - 1
                typInCoil(ii).initialize()
            Next
            ReDim cYobi(16 - 1)
        End Sub
    End Structure

    ' 採寸実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_SAISUN_INF_CNT, pack:=1)> _
    Public Structure PRO_TYPE_SAISUN_INF
        Dim nStart As Integer                           ' 採寸開始位置 [m]
        Dim nEnd As Integer                             ' 採寸終了位置 [m]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim nYobi() As Integer

        'イニシャライズ
        Public Sub initialize()
            ReDim nYobi(2 - 1)
        End Sub
    End Structure

    '''' <summary>
    '''' 採寸実績伝文
    '''' </summary>
    '''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_SAISUN, Pack:=1)>
    Public Structure PRO_TYPE_DATA_SAISUN
        Dim Common As PRO_TYPE_COMMON                   ' 共通情報

        Dim KeyInf As PRO_TYPE_KEY                      ' キーNo

        Dim nLen As Integer                             ' 実績コイル長 [m]
        Dim nMakiLenF As Integer                        ' フロント巻き込まれ長さ [m]
        Dim nSetCnt As Integer                          ' セットした採寸個数
        <VBFixedArray(9 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=9)>
        Dim nYobi() As Integer                          ' 予備

        <VBFixedArray(PRO_MAX_CUT_COUNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=PRO_MAX_CUT_COUNT)>
        Dim inf() As PRO_TYPE_SAISUN_INF                ' 採寸情報
        <VBFixedArray(128 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=128)>
        Dim cYobi() As Byte                             ' 予備

        'イニシャライズ
        Public Sub initialize()
            Common.initialize()
            KeyInf.initialize()
            ReDim nYobi(9 - 1)
            ReDim inf(PRO_MAX_CUT_COUNT - 1)
            For ii As Integer = 0 To PRO_MAX_CUT_COUNT - 1
                inf.Initialize()
            Next
            ReDim cYobi(128 - 1)
        End Sub
    End Structure

    ' カット実績
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_CUT, Pack:=1)>
    Public Structure PRO_TYPE_DATA_CUT

        Dim Common As PRO_TYPE_COMMON                   ' 共通情報

        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim cMeiNo() As Byte                            ' 入側論理コイル 命令No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)>
        Dim cCoilNo() As Byte                           ' 入側論理コイル 生産No
        Dim nMode As Integer                            ' 実績区分 (0:溶接点通過 1:Fカット)
        Dim nStart As Integer                           ' 採寸開始位置 [m]
        Dim nEnd As Integer                             ' 採寸終了位置 [m]
        Dim nKoban As Integer                           ' 子番(1CAPLのみ)
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)>
        Dim cYobi() As Byte

        'イニシャライズ
        Public Sub initialize()
            Common.initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cYobi(32 - 1)
        End Sub
    End Structure


    ' グレード 1～6毎の疵混入率情報
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_GRADE_INF, pack:=1)> _
    Public Structure GRADE_INF
        Dim nMixedRate As Short                         ' 混入率 [10-2%]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim nYobi1() As Short
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim nTypeCnt() As Short                         ' 疵個数 (疵種1～10)
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim nYobi2() As Short

        'イニシャライズ
        Public Sub initialize()
            ReDim nYobi1(2 - 1)
            ReDim nTypeCnt(10 - 1)
            ReDim nYobi2(3 - 1)
        End Sub

    End Structure


    ' 検査実績
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_ZISEKI, Pack:=1)>
    Public Structure PRO_TYPE_DATA_ZISEKI

        Dim Common As PRO_TYPE_COMMON                   ' 共通情報

        Dim nZiMode As Integer                          ' 実績区分 (0:正常 1:異常(カット実績で受信した該当コイル無し) )
        Dim nKikiMode As Integer                        ' 収集区分 (0:正常 1:異常)
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim cMeiNo() As Byte                            ' 入側論理コイル 命令No
        <VBFixedArray(12 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=12)>
        Dim cCoilNo() As Byte                           ' 入側論理コイル 生産No
        Dim nKoban As Integer                           ' 子番(1CAPLのみ)
        <VBFixedArray(36 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=36)>
        Dim cYobi1() As Byte
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)>
        Dim grade_inf() As GRADE_INF                    ' グレード 1～6毎の疵混入率情報
        <VBFixedArray(192 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=192)>
        Dim cYobi() As Byte

        'イニシャライズ
        Public Sub initialize()
            Common.initialize()
            ReDim cMeiNo(4 - 1)
            ReDim cCoilNo(12 - 1)
            ReDim cYobi1(40 - 1)
            ReDim grade_inf(6 - 1)
            ReDim cYobi(192 - 1)
        End Sub

    End Structure

    ' ヘルシー伝文
    '<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_DATA_HELS, Pack:=1)>
    'Public Structure PRO_TYPE_DATA_HELS
    '    <VBFixedArray(SIZE_PRO_TYPE_DATA_HELS - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=SIZE_PRO_TYPE_DATA_HELS)>
    '    Dim cYobi() As Byte                             ' コイルNo

    '    'イニシャライズ
    '    Public Sub initialize()
    '        ReDim cYobi(SIZE_PRO_TYPE_DATA_HELS - 1)
    '    End Sub
    'End Structure


    ' //=======================================
    ' // プロコン 伝文完成版
    ' //=======================================

    ' イニシャル伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_INIT, pack:=1)> _
    Public Structure PRO_TYPE_INIT
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_INIT

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' コイル情報伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_COILINF, pack:=1)> _
    Public Structure PRO_TYPE_COILINF
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_COILINF

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' 採寸実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_SAISUN, pack:=1)> _
    Public Structure PRO_TYPE_SAISUN
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_SAISUN

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' カット実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_CUT, pack:=1)> _
    Public Structure PRO_TYPE_CUT
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_CUT

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' 検査実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_ZISEKI, pack:=1)> _
    Public Structure PRO_TYPE_ZISEKI
        Dim head As PRO_TYPE_HEAD
        Dim data As PRO_TYPE_DATA_ZISEKI

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
            data.initialize()
        End Sub
    End Structure

    ' // ヘルシー伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_PRO_TYPE_HELS, pack:=1)> _
    Public Structure PRO_TYPE_HEALTHY
        Dim head As PRO_TYPE_HEAD

        'イニシャライズ
        Public Sub initialize()
            head.initialize()
        End Sub
    End Structure


    '//***************************************************************
    '// FF通信　定義
    '//***************************************************************
    '//=======================================
    '// FF通信系　固定サイズ
    '//=======================================
    Public Const MAX_FF_DFCT_CNT As Integer = 1800          ' ＦＦ検査実績伝文疵情報

    Public Const MAX_FF_MAE_DINF_CNT As Integer = 7         ' ＦＦ前工程疵実績伝文疵検出情報

    Public Const MAX_FF_MAE_DEF_CNT As Integer = 2000       ' ＦＦ前工程疵実績伝文 (全部)
    'Public Const MAX_FF_MAE_CC_CNT As Integer = 10			' ＦＦ前工程疵実績伝文ＣＣ疵情報
    'Public Const MAX_FF_MAE_HOT_CNT As Integer = 50		' ＦＦ前工程疵実績伝文ＨＯＴ疵情報
    'Public Const MAX_FF_MAE_PL_CNT As Integer = 80			' ＦＦ前工程疵実績伝文ＰＬ疵情報
    'Public Const MAX_FF_MAE_TCM_ANA_CNT As Integer = 30	' ＦＦ前工程疵実績伝文ＴＣＭ穴情報
    'Public Const MAX_FF_MAE_TCM_KIZU_CNT As Integer = 30	' ＦＦ前工程疵実績伝文ＴＣＭ疵情報
    'Public Const MAX_FF_MAE_ATU_CNT As Integer = 1800		' ＦＦ前工程疵実績伝文直前工程情報
    ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
    Public Const MAX_FF_MAE_CC_CNT As Integer = 10          ' ＦＦ前工程疵実績伝文ＣＣ疵情報
    Public Const MAX_FF_MAE_HOT_CNT As Integer = 50         ' ＦＦ前工程疵実績伝文ＨＯＴ疵情報
    Public Const MAX_FF_MAE_PL_CNT As Integer = 80          ' ＦＦ前工程疵実績伝文ＰＬ疵情報
    Public Const MAX_FF_MAE_TCM_ANA_CNT As Integer = 30     ' ＦＦ前工程疵実績伝文ＴＣＭ穴情報
    Public Const MAX_FF_MAE_YOBI_CNT As Integer = 30        ' ＦＦ前工程疵実績伝文その他情報
    Public Const MAX_FF_MAE_ATU_CNT As Integer = 900        ' ＦＦ前工程疵実績伝文直前工程情報
    Public Const MAX_FF_MAE_TCM_KIZU_CNT As Integer = 900   ' ＦＦ前工程疵実績伝文ＴＣＭ疵情報
    ' 20221021 2TCM実機化に伴う下工程改造 小島 <<<---

    Public Const MAX_FF_STS_CNT As Integer = 30000          ' ＦＦ検査状態伝文検査状態

    Public Const MAX_FF_MAE_THAN_CNT As Integer = 10        ' ＦＦ前工程疵実績伝文通板実績情報
    Public Const MAX_FF_MAE_BNKT_CNT As Integer = 20        ' ＦＦ前工程疵実績伝文分割情報

    '//=======================================
    '// 構造体（対FFサーバー伝送）
    '//=======================================

    ' 検査実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' 伝文コード 表=M1 裏=M2
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番(FF) 
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim bytDate() As Byte                   ' 伝文発生時刻 形式 YYYYMMDHHNNSS0
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' 工程コード [CTL=A1]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' 命令番号
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' 冷延コイル番号

        'イニシャライズ
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim bytDate(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub

    End Structure

    ' データ部
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_TYPE_DATA
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Kizukbn() As Byte                   ' 疵検出区分 0:φω
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kind() As Byte                      ' 疵種
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Grade() As Byte                     ' 疵グレード
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim WidPos() As Byte                    ' 疵巾方向位置
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Len() As Byte                       ' 疵長さ
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim DefectPos() As Byte                 ' 疵総長さ方向位置
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim kbn() As Byte                       ' 疵区分
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim No() As Byte                        ' 疵NO
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim OpeDsp() As Byte                    ' オペガイ表示フラグ
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim SysKbn() As Byte                    ' システム区分
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim Filler() As Byte                    ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim Kizukbn(1 - 1)
            ReDim Kind(2 - 1)
            ReDim Grade(2 - 1)
            ReDim WidPos(4 - 1)
            ReDim Len(4 - 1)
            ReDim DefectPos(6 - 1)
            ReDim kbn(1 - 1)
            ReDim No(4 - 1)
            ReDim OpeDsp(1 - 1)
            ReDim SysKbn(1 - 1)
            ReDim Filler(7 - 1)
        End Sub

    End Structure


    ''FF本体
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_JISSEKI
        Dim Hd As FF_TYPE_HEAD                  ' 伝文ヘッダー

        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' コイル長さ
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Mode() As Byte                      ' 疵検処理モード
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim OverStatus() As Byte                ' 疵個数オーバーステータス
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' 疵検管理No
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim DataDefault() As Byte               ' データ設定数
        <VBFixedArray(32 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim cYobi1() As Byte                    ' 予備
        <VBFixedArray(MAX_FF_DFCT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_DFCT_CNT)> _
        Dim Dt() As FF_TYPE_DATA                ' データ
        <VBFixedArray(2000 - MAX_FF_DFCT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2000 - MAX_FF_DFCT_CNT)> _
        Dim Dtyobi() As FF_TYPE_DATA            ' データ
        <VBFixedArray(900 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=900)> _
        Dim cYobi() As Byte                     ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim CoilLen(6 - 1)
            ReDim Mode(2 - 1)
            ReDim OverStatus(2 - 1)
            ReDim Id(20 - 1)
            ReDim DataDefault(4 - 1)
            ReDim cYobi1(32 - 1)
            ReDim Dt(MAX_FF_DFCT_CNT - 1)
            ReDim Dtyobi(2000 - MAX_FF_DFCT_CNT - 1)
            ReDim cYobi(900 - 1)
        End Sub

    End Structure

    Public Const FF_MAE_MEINO_LEN As Integer = 4        ' ＦＦ前工程疵実績伝文分割情報

    ''  //// 応答伝文
    ''FF本体
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_ANS_HD
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' 伝文コード [M7/M8]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番(00)					  
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateD() As Byte                     ' 伝文発生時刻 形式 YYYYMMDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' 工程コード[CTL=A1]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' 命令番号
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' 入側コイル番号

        'イニシャライズ
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateD(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub

    End Structure


    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=2, pack:=1)> _
    Public Structure FF_ANS_DATA
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Status() As Byte                    ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim Status(2 - 1)
        End Sub

    End Structure


    ''  FF_ANS
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_ANS
        Dim Hd As FF_ANS_HD                     ' 伝文ヘッダー
        Dim Data As FF_ANS_DATA                 ' データ
        <VBFixedArray(64964 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64964)> _
        Dim cYobi() As Byte                     ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim cYobi(64964 - 1)
        End Sub

    End Structure

    '-------------------------------------------------------
    '前工程関連 将来対応用
    '-------------------------------------------------------
    ' 前工程疵実績要求伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_MAE_REQUEST

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim HeaderCode() As Byte                ' 伝文コード
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番 ("00"固定)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' 伝文発生時刻 形式 YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' 工程コード  (CTL=A1)
        <VBFixedArray(FF_MAE_MEINO_LEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=FF_MAE_MEINO_LEN)> _
        Dim MeireiNo() As Byte                  ' 命令番号４桁(	'０	'＋命令番号３桁)を右詰設定

        <VBFixedArray(64976 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64976)> _
        Dim Filler() As Byte                    ' 予備 空白(0x20)

        'イニシャライズ
        Public Sub initialize()
            ReDim HeaderCode(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim MeireiNo(FF_MAE_MEINO_LEN - 1)
            ReDim Filler(64976 - 1)
        End Sub

    End Structure

    ' 前工程疵実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=113, Pack:=1)>
    Public Structure FF_MAE_TYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' 伝文コード (XX:表, XX:裏)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番 ("00"固定)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' 伝文発生時刻 形式 YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim Line_Code() As Byte                 ' 工程コード  (5CGL=85)
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' 命令番号
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim O_UKbn() As Byte                    ' 表裏区分(1:表, 2:裏)
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim ReiCoilNo() As Byte                 ' 冷延コイル番号
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ReiCoilG() As Byte                  ' 冷延コイル重量 [10-3t]
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim ReiCoilL() As Byte                  ' 冷延コイル長さ [10-1m]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ReiCoilAtu() As Byte                ' 冷延厚 [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ItaatuMin() As Byte                 ' 板厚　公差　MIN [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim ItaatuMax() As Byte                 ' 板厚　公差　MAX [10-3mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1i() As Byte                ' 向け先１　板巾 [10-1mm]
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Kensa() As Byte                     ' 検査基準
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Maki() As Byte                      ' 巻き方向
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Zairyou() As Byte                   ' 材料巾 [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1kMin() As Byte             ' 向け先１　板巾公差　MIN [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1kMax() As Byte             ' 向け先１　板巾公差　MAX [10-1mm]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1L() As Byte                ' 向け先１　長さ(ロッド枚数) [m]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim Mukesaki1N() As Byte                ' 向け先１　内径 [10-1ｲﾝﾁ]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Itaatu() As Byte                    ' 板厚 [10-3mm]
        <VBFixedArray(3 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim Irigawa() As Byte                   ' 入側内径 [10-1ｲﾝﾁ]
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)> _
        Dim Mukesaki1KL() As Byte               ' 向け先１　剪断長 [10-1mm]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Makoutei() As Byte                  ' 前工程コード
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim RealMakoutei() As Byte              ' 実前工程コード
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim TCMFlag() As Byte                   ' 厚手/TCM直送材フラグ

        'イニシャライズ
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim O_UKbn(2 - 1)
            ReDim ReiCoilNo(10 - 1)
            ReDim ReiCoilG(5 - 1)
            ReDim ReiCoilL(6 - 1)
            ReDim ReiCoilAtu(5 - 1)
            ReDim ItaatuMin(5 - 1)
            ReDim ItaatuMax(5 - 1)
            ReDim Mukesaki1i(5 - 1)
            ReDim Kensa(4 - 1)
            ReDim Maki(1 - 1)
            ReDim Zairyou(5 - 1)
            ReDim Mukesaki1kMin(5 - 1)
            ReDim Mukesaki1kMax(5 - 1)
            ReDim Mukesaki1L(5 - 1)
            ReDim Mukesaki1N(3 - 1)
            ReDim Itaatu(5 - 1)
            ReDim Irigawa(3 - 1)
            ReDim Mukesaki1KL(5 - 1)
            ReDim Makoutei(2 - 1)
            ReDim RealMakoutei(2 - 1)
            ReDim TCMFlag(1 - 1)
        End Sub

    End Structure

    ' 前工程情報
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_MAE_TYPE_INF
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kizukbn() As Byte                   ' 疵検出工程
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' 管理No
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Mode() As Byte                      ' 検査処理モード
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim DataDefault() As Byte               ' 疵設定個数
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim OverStatus() As Byte                ' 疵個数オーバーステータス

        'イニシャライズ
        Public Sub initialize()
            ReDim Kizukbn(2 - 1)
            ReDim Id(20 - 1)
            ReDim Mode(2 - 1)
            ReDim DataDefault(4 - 1)
            ReDim OverStatus(2 - 1)
        End Sub

    End Structure

    ' 
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=20, pack:=1)> _
    Public Structure FF_MAE_TYPE_VRCLINF
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim KizukenNo() As Byte                 ' VRCL管理No

        'イニシャライズ
        Public Sub initialize()
            ReDim KizukenNo(20 - 1)
        End Sub

    End Structure

    ' From-To
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=24, pack:=1)> _
    Public Structure FF_MAE_TYPE_FROMTO
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim CoilNo() As Byte                    ' 冷延コイルNo
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim From() As Byte                      ' 採取開始位置長さ（From） [0.1m]
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim To2() As Byte                       ' 採取終了位置長さ（To）	[0.1m]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Koban() As Byte                     ' 採取子番

        'イニシャライズ
        Public Sub initialize()
            ReDim CoilNo(10 - 1)
            ReDim From(6 - 1)
            ReDim To2(6 - 1)
            ReDim Koban(2 - 1)
        End Sub
    End Structure

    ' 疵情報
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=30, pack:=1)> _
    Public Structure FF_MAE_TYPE_DATA
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kind() As Byte                      ' 疵種
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Grade() As Byte                     ' 疵グレード
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Kizukbn() As Byte                   ' 疵検出工程
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim kbn() As Byte                       ' 疵区分
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim DefectPos() As Byte                 ' 疵長さ方向位置
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim WidPos() As Byte                    ' 疵巾方向位置
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Len() As Byte                       ' 疵長さ
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim No() As Byte                        ' 疵NO
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim SysKbn() As Byte                    ' システム区分
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)>
        Dim Filler() As Byte                    ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim Kind(2 - 1)
            ReDim Grade(2 - 1)
            ReDim Kizukbn(2 - 1)
            ReDim kbn(1 - 1)
            ReDim DefectPos(6 - 1)
            ReDim WidPos(4 - 1)
            ReDim Len(4 - 1)
            ReDim No(4 - 1)
            ReDim SysKbn(1 - 1)
            ReDim Filler(4 - 1)
        End Sub
    End Structure

    ' 分割実績情報
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=20, pack:=1)> _
    Public Structure FF_MAE_TYPE_BUNKATSU
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim MakiHoukou() As Byte                ' 実績巻方向(1:上出上捲, 2:上出下捲, 3:下出上捲, 4:下出下捲)
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' コイル長
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim Cutkbn() As Byte                    ' カット場所区分(I:入側カット, O:出側カット)
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim GenpinNo() As Byte                  ' 分割現品管理NO
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Filler() As Byte                    ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim MakiHoukou(1 - 1)
            ReDim CoilLen(6 - 1)
            ReDim Cutkbn(1 - 1)
            ReDim GenpinNo(10 - 1)
            ReDim Filler(2 - 1)
        End Sub
    End Structure

    ' 通板実績
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=425, pack:=1)> _
    Public Structure FF_MAE_TYPE_THUHAN
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim JissekiCode() As Byte               ' 実績工程コード
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim MeireiNo() As Byte                  ' 命令番号
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim ThuhanCnt() As Byte                 ' 通板回数
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim GenpinNo() As Byte                  ' 入側現品管理NO
        <VBFixedArray(6 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=6)> _
        Dim CoilLen() As Byte                   ' 入側コイル長さ
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Filler() As Byte                    ' 予備
        <VBFixedArray(MAX_FF_MAE_BNKT_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_BNKT_CNT)> _
        Dim Bu() As FF_MAE_TYPE_BUNKATSU        ' 分割情報

        'イニシャライズ
        Public Sub initialize()
            ReDim JissekiCode(2 - 1)
            ReDim MeireiNo(4 - 1)
            ReDim ThuhanCnt(1 - 1)
            ReDim GenpinNo(10 - 1)
            ReDim CoilLen(6 - 1)
            ReDim Filler(2 - 1)
            ReDim Bu(MAX_FF_MAE_BNKT_CNT - 1)
        End Sub

    End Structure

    ' 前工程疵情報伝文本体
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_MAE_JISSEKI

        Dim Hd As FF_MAE_TYPE_HEAD              ' ヘッダー部

        <VBFixedArray(27 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=27)> _
        Dim Yobi1() As Byte                     ' 予備

        <VBFixedArray(MAX_FF_MAE_DINF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_DINF_CNT)> _
        Dim Inf() As FF_MAE_TYPE_INF            ' 疵検出情報

        <VBFixedArray(90 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=90)> _
        Dim Yobi2() As Byte                     ' 予備
        Dim VRcl As FF_MAE_TYPE_VRCLINF         ' VRCL情報
        Dim FromTo As FF_MAE_TYPE_FROMTO        ' From-To
        <VBFixedArray(116 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=116)> _
        Dim Yobi3() As Byte                     ' 予備

        <VBFixedArray(MAX_FF_MAE_DEF_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_DEF_CNT)> _
        Dim def() As FF_MAE_TYPE_DATA           ' 疵情報 (ヘッダーの疵検出工程 と 個々の疵の疵検出工程 を 全部見比べて紐付ける)

        <VBFixedArray(MAX_FF_MAE_THAN_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_MAE_THAN_CNT)> _
        Dim Th() As FF_MAE_TYPE_THUHAN          ' 通板実績情報
        <VBFixedArray(150 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=150)> _
        Dim Yobi() As Byte                      ' 予備

        'イニシャライズ
        Public Sub initialize()
            Hd.initialize()
            ReDim Yobi1(27 - 1)
            ReDim Inf(MAX_FF_MAE_DINF_CNT - 1)
            Inf.Initialize()
            ReDim Yobi2(90 - 1)
            ReDim Yobi3(116 - 1)
            ReDim def(MAX_FF_MAE_DEF_CNT - 1)
            def.Initialize()
            ReDim Th(MAX_FF_MAE_THAN_CNT - 1)
            Th.Initialize()
            ReDim Yobi(150 - 1)
        End Sub

    End Structure

    ' 検査実績伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=34, pack:=1)> _
    Public Structure FF_STSTYPE_HEAD

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' 伝文コード 表:95 裏:96
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番(00) 
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim bytDate() As Byte                   ' 伝文発生時刻 形式 YYYYMMDHHNNSS0
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' 疵検ラインの工程コード[	'81	',	'85	',	'54	',	'82(53)	']設定
        <VBFixedArray(4 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=4)> _
        Dim Meino() As Byte                     ' 命令番号
        <VBFixedArray(10 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=10)> _
        Dim Coilno() As Byte                    ' 冷延コイル番号

        'イニシャライズ
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim bytDate(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Meino(4 - 1)
            ReDim Coilno(10 - 1)
        End Sub


    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=1, pack:=1)> _
    Public Structure FF_KENSA_STATUS

        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)> _
        Dim status() As Byte                    ' 検査状態(1m間の検査状態を1byteに格納(上限30000m))

        'イニシャライズ
        Public Sub initialize()
            ReDim status(1 - 1)
        End Sub


    End Structure

    ' データ部
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=66 + MAX_FF_STS_CNT, Pack:=1)>
    Public Structure FF_STSTYPE_DATA
        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim Id() As Byte                        ' 疵検管理No
        <VBFixedArray(5 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=5)>
        Dim dataSize() As Byte                  ' 検査状態セット数：検査状態を格納したbyte数数を右詰設定(０埋め)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)>
        Dim yobi1() As Byte                      ' 予備1
        <VBFixedArray(1 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=1)>
        Dim yobi2() As Byte                      ' 予備2
        <VBFixedArray(38 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=38)>
        Dim yobi() As Byte                      ' 予備
        <VBFixedArray(MAX_FF_STS_CNT - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=MAX_FF_STS_CNT)> _
        Dim data() As FF_KENSA_STATUS           ' 検査状態(1m間の検査状態を1byteに格納(上限30000m))

        'イニシャライズ
        Public Sub initialize()
            ReDim Id(20 - 1)
            ReDim dataSize(5 - 1)
            ReDim yobi1(2 - 1)
            ReDim yobi2(1 - 1)
            ReDim yobi(38 - 1)
            For ii As Integer = 0 To data.Length - 1
                data(ii).initialize()
            Next
        End Sub

    End Structure


    ''FF本体
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_STS_JISSEKI
        Dim Hd As FF_STSTYPE_HEAD               ' 伝文ヘッダー
        Dim Dt As FF_STSTYPE_DATA               ' データ
        <VBFixedArray(34900 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=34900)>
        Dim Filler2() As Byte                   ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim Filler2(34900 - 1)
        End Sub

    End Structure

    ' ヘルシー伝文＆ヘルシー応答伝文
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=65000, pack:=1)> _
    Public Structure FF_HELCY

        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Header_Code() As Byte               ' 伝文コード (ZZ)
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Rec_Code() As Byte                  ' 連番 (FF)
        <VBFixedArray(14 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=14)> _
        Dim DateTime() As Byte                  ' 伝文発生時刻 形式 YYYYMMDDHHNNSS
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Line_Code() As Byte                 ' 工程コード [CTL=A1]
        <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
        Dim Denkan_No() As Byte                 ' 伝管コマンドNo
        <VBFixedArray(64978 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=64978)> _
        Dim Yobi() As Byte                      ' 予備

        'イニシャライズ
        Public Sub initialize()
            ReDim Header_Code(2 - 1)
            ReDim Rec_Code(2 - 1)
            ReDim DateTime(14 - 1)
            ReDim Line_Code(2 - 1)
            ReDim Denkan_No(2 - 1)
            ReDim Yobi(64978 - 1)
        End Sub

    End Structure

End Module
