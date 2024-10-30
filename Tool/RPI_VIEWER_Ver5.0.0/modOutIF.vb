Imports System.Runtime.InteropServices


''' <summary>
''' 外部用I/F定義
''' </summary>
Public Module modOutIF

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 起動引数

    ''' <summary>
    ''' 起動引数
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure START_ARGS
        Dim sTaskName As String			' メールスロット生成名。このときにはiniファイルよりも優先される (TaskName:=)
        Dim nId As Integer				' RPI_SEQの何番目(0オリジン) (ID:=)

        Dim sArgs() As String			' 引数無加工
    End Structure

    Public Const START_CMD_TASKNAME As String = "TaskName:="
    Public Const START_CMD_ID As String = "ID:="



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メールスロット受信用
    Public Const SIZE_MSL As Integer = 256						'// send_mail, recv_mailサイズ
    Public Const SIZE_MSL_DATA As Integer = SIZE_MSL - 8		'// nEventNo以降のデータ長


    ''' <summary>
    ''' MIX型標準形式と同じ
    ''' </summary>
    ''' <remarks></remarks>
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_MSL, Pack:=1)> _
     Public Structure COMMON_QUE_STRMIX
        Dim nEventNo As Integer
        Dim nLineNo As Integer

        <VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
        Dim idata() As Integer

        '// ↓ByValTStr これだと 終端に勝手にNULLが付与される為、終端1バイトが削除される。
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata1 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata2 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata3 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata4 As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
        Dim cdata5 As String

        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=(SIZE_MSL_DATA - 4 * 20 - 32 * 5))> _
        Dim ydata As String

        Public Sub inisialize()
            ReDim idata(20 - 1)
        End Sub

    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ RPI_VIEWER 外部指示用

    '===================================================================
    ' 共通用

    '----------------------------------------------------
    ' 画面終了指示
    Public Const FACT_RPIV_END As Integer = 0		' 画面終了指示

    '----------------------------------------------------
    ' RPIファイル表示
    ' MIX_S[0] : RPIフォルダパス (※ここにフルパスを入れてもOK)
    ' MIX_S[1] : RPIファイル名1
    ' MIX_S[2] : RPIファイル名2	(※S[2]にデータがセットされている場合は、内部的には単純にS[0]～[2]を連結するだけとする)

    ' MIX_I[0] : 複数チャンネル時 (0オリジン)
    ' MIX_I[1] : Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
    ' MIX_I[2] : Y方向[mm]
    ' MIX_I[3] : Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
    ' MIX_I[4] : X方向[mm] (-1:移動させない)
    ' MIX_I[5] : X方向[mm]がどっちの指定から？(0:左端  1:センターから)

    ' 以下３項目の引数 (0:無視 -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 1:元画像に対して反転 2:iniファイル設定後に対して反転)
    ' MIX_I[6] : 左右反転
    ' MIX_I[7] : 上下反転
    ' MIX_I[8] : 90度反転
    Public Const FACT_RPIV_DSPING As Integer = 1		' RPIファイル表示

    '----------------------------------------------------
    ' フォームを選択状態にする
    '	※ 複数連動の場合、指定位置ジャンプが非アクティブ状態のとき利かない対策
    Public Const FACT_RPIV_ACTIVE As Integer = 2		' RPIビューアー アクティブ

    '----------------------------------------------------
    ' RPIファイル検索＋表示
    ' MIX_S[0] : 対象のINIファイルセクション
    ' MIX_S[1] : 管理No

    ' MIX_I[0] : 面(0,1)
    ' MIX_I[1] : 角度、色 (0オリジン)
    Public Const FACT_RPIV_SEARCH_DSPING As Integer = 3		' RPIファイル検索＋表示

    '----------------------------------------------------
    ' 指定位置
    ' MIX_I[0] : Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
    ' MIX_I[1] : Y方向[mm]
    ' MIX_I[2] : Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)
    ' MIX_I[3] : X方向[mm] (-1:移動させない)
    ' MIX_I[4] : X方向[mm]がどっちの指定から？(0:左端  1:センターから)
    Public Const FACT_RPIV_DSP_MOVE As Integer = 10		' RPIファイル表示


    '----------------------------------------------------
    ' 反転
    ' 以下３項目の引数 (0:無視 -1:元画像に対して反転無し(要はiniファイルで反転させたやつも戻す) 1:元画像に対して反転 2:iniファイル設定後に対して反転)
    ' MIX_I[0] : 左右反転
    ' MIX_I[1] : 上下反転
    ' MIX_I[2] : 90度反転
    Public Const FACT_RPIV_DSP_TURN As Integer = 11		' 表示方向切替



    '===================================================================
    ' 連動用 (100～200)

    ' MIX_I[0] : 対応ビット (下位から順にbitONで、対応する項目を反映。0x01であれば伸び率)
    ' MIX_I[1] : 伸び率 [10-6％]
    ' MIX_I[2] : スタートオフセットY [mm]
    ' MIX_I[3] : スタートオフセットX [mm]
    Public Const FACT_RPIS_SETTING As Integer = 100

    '----------------------------------------------------
    ' 表示位置・サイズ
    ' MIX_I[0] : モニターNo
    ' MIX_I[1] : モニター内ポジションX [pixel]
    ' MIX_I[2] : モニター内ポジションY [pixel]
    ' MIX_I[3] : 表示サイズＸ [pixel]
    ' MIX_I[4] : 表示サイズＹ [pixel]
    ' MIX_I[5] : 最大最小区分　0:通常 1:最小化 2:最大化
    Public Const FACT_RPIS_DSP_POS As Integer = 101

    '----------------------------------------------------
    ' 連動開始／連動停止
    ' MIX_I[0] : 0:連動開始  1:連動停止
    Public Const FACT_RPIS_RENDO As Integer = 102

    '----------------------------------------------------
    ' オフセット値反映
    Public Const FACT_RPIS_OFFSET As Integer = 103

    '----------------------------------------------------
    ' ＲＰＩビューアクローズ
    Public Const FACT_RPIS_CLOSE As Integer = 104

    '----------------------------------------------------
    ' ＲＰＩビューア初期表示
    Public Const FACT_RPIV_FIRST_DSPING As Integer = 105


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 共有メモリ (簡易連携)
    Public Const TBL_RPI_SYNC_NAME As String = "TBL_RPI_SYNC"		' 連動テーブル

    ''' <summary>
    ''' 共通コイル項目
    ''' </summary>
    Public Const SIZE_TYPE_SYNC As Integer = 8
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_SYNC, pack:=1)> _
    Public Structure TYPE_SYNC
        Dim nY As Integer						' Y座標[mm]
        Dim nX As Integer						' X座標[mm]

        'イニシャライズ
        Public Sub initialize()
        End Sub
    End Structure




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 共有メモリ (他工程間連携)
    Public Const TBL_RPI_TRK_NAME As String = "TBL_RPI_TRK"		' 連動テーブル
    Public Const TBL_RPI_REC_MAX As Integer = 5					' 連動テーブル数

    ''' <summary>
    ''' ヘッダー項目
    ''' </summary>
    Public Const SIZE_TYPE_HEAD As Integer = 4
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_HEAD, pack:=1)> _
     Public Structure TYPE_TRK_HEAD
        Dim nActiveID As Integer						' 操作中ID (0オリジン)

        'イニシャライズ
        Public Sub initialize()

        End Sub
    End Structure

    ''' <summary>
    ''' 共通コイル項目
    ''' </summary>
    Public Const SIZE_TYPE_ITEM As Integer = 8
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_ITEM, pack:=1)> _
     Public Structure TYPE_TRK_ITEM
        Dim nY As Integer								' Y座標[mm]
        Dim nX As Integer								' X座標[mm]



        'イニシャライズ
        Public Sub initialize()

        End Sub
    End Structure



    ''' <summary>
    ''' RPI_VIEWER間連動テーブル
    ''' </summary>
    Public Const SIZE_TYPE_TRK As Integer = SIZE_TYPE_ITEM * TBL_RPI_REC_MAX + SIZE_TYPE_HEAD

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_TYPE_TRK, pack:=1)> _
    Public Structure TYPE_TRK
        Dim head As TYPE_TRK_HEAD

        <VBFixedArray(TBL_RPI_REC_MAX - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=TBL_RPI_REC_MAX)> _
        Dim rec() As TYPE_TRK_ITEM


        'イニシャライズ
        Public Sub initialize()
            ReDim rec(TBL_RPI_REC_MAX - 1)
            For ii As Integer = 0 To rec.Length - 1
                rec(ii).initialize()
            Next
            head.initialize()
        End Sub
    End Structure


End Module
