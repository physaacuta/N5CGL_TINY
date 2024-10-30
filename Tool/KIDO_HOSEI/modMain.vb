'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2012/10/02  新規作成
'
'	[メモ]
'		・VB雛形 そのままコピペでＯＫ。△部分は、実装時に置き換え
'*********************************************************************************
Option Strict On                    ' 全疵検、全プログラム必須

Imports tClassLibrary               ' 疵検VBライブラリ。
Imports KizuLibCLI                  ' 疵検CLIライブラリ。メールスロット等使用時必須
Imports System.Reflection



Public Module modMain

	Public Const XX_VBSMP As String = "XX_VBSMP"	' △ これは、テストデータ。ホントは定数を使用
	Public Const SYSTEM_TITLE As String = "test"	' △ これは、テストデータ。ホントは定数を使用


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ グローバル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing				' ログ管理クラス (本体)
    Public g_NOA As Integer                                             ' NOA接続フラグ (0:オンライン  1:遠隔)
    Public g_LineName As String                                         ' 輝度補正値の保管先に使うライン名
    Public g_LineCollor As Dictionary(Of String, String)                ' 描画する線の色の設定
    Public g_MoveAverage As String                                      ' 前回画面終了時に保存された移動平均サイズ
    Public g_LineProNama As String                                      ' 描画する線の色(縦プロジェクション生)
    Public g_LineProAverage As String                                   ' 描画する線の色(縦プロジェクション移動平均)
    Public g_LineCorrected As String                                    ' 描画する線の色(補正輝度線)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' メインスレッド管理用 (ほぼ必須の基本の形)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' 総合クラス （別スレッドなので注意）
    Private m_ap As ApplicationContext                                  ' コンテキスト

    Private mfrm_KidoHosei As frmKidoHosei


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' エントリーポイント
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Task_First()

        '//======================================================
        '  画面実行

        '' 画面表示
        Dim showMontor As Integer = 1       ' メニュー表示対象のモニターNo (1オリジン)

        '' △ ここで各画面切り替え(メニュー画面の表示)
        mfrm_KidoHosei = New frmKidoHosei(gcls_Log)
        mfrm_KidoHosei.Show()


        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック
        m_ap.Dispose()

        Task_Last()
    End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        mcls_Mod = New KizuMod
        'mcls_Mod = New KizuLibCLI.KizuMod

        '' ログクラス生成
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager("KIDO_HOSEI", Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "オペレータ画面起動")


    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()
        '//======================================================
        '  終了処理
        Try
            '''' ここまで来たら 終了ね
            ' フォーム開放
            'If Not mfrm_KidoHosei Is Nothing Then
            '    mfrm_KidoHosei.Close()
            '    mfrm_KidoHosei.Dispose()
            '    mfrm_KidoHosei = Nothing
            'End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0} [{1}]", "メイン処理終了異常", ex.Message))
        End Try

        '' その他クラス解放
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "オペレータ画面終了")
        gcls_Log.Dispose()
        gcls_Log = Nothing

        '' タスク終了
        mcls_Mod.Task_Exit()

    End Sub

	''' <summary>
	''' タスク終了処理
	''' </summary>
	''' <remarks></remarks>
	Public Sub TaskEnd()
		m_ap.ExitThread()										' メインスレッド終了
	End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームタイトル設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

		frmData.Text = SYSTEM_TITLE & Space(2) & frmData.Text

    End Sub

    ''' <summary>
    ''' データグリッドビューテキストボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                             ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                             ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                             Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            'clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal
            clm.FillWeight = 1
            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' データグリッドビューチェックボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Public Sub dgvColSetCheck(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                              ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                              ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                              Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewCheckBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' DB保存時のハードコピーを残しておく
    ''' </summary>
    ''' <param name="myForm"></param>
    ''' <remarks></remarks>
    Public Function HardCopy(ByVal myForm As Form) As String
        Dim strFPath As String
        Dim strFolder As String
        Dim strName As String

        strFolder = IMG_PATH
        strName = myForm.Text & "_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg"

        '''' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '''' フォルダの有無チェック(無かったら作ろう)
        tMod.CheckFolder(strFolder)

        '''' ハードコピー
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Call cls_Hcopy.HCopy(myForm, strFPath, Drawing.Imaging.ImageFormat.Jpeg)

        Return strFPath
    End Function

    ''' <summary>
    ''' KizuLibCLI.DLLの読込み
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit切り替え対応</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function

End Module
