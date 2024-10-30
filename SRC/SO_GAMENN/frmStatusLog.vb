'*********************************************************************************
' 故障ログ画面
'	[Ver]
'		Ver.01    2007/06/29  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
'Imports KizuForm

''' <summary>
''' 故障ログ画面
''' </summary>
''' <remarks></remarks>


Public Class frmStatusLog
    Inherits SO_GAMENN.frmMain_StatusLog


    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン

            Case 3                                  ' キャンセル
                Call ExecCancel_Log()
            Case 4                                  ' ログ検索
                Call ExecSelect_Log()
            Case 8                                  'CSV出力
                Call CsvOut(CSV_PATH, "検査状態ログ画面.csv")
            Case 9                                  '画面印刷
                Call PrintOut()
            Case 10                                 'ﾊｰﾄﾞｺﾋﾟｰ
                Call HardCopy(IMG_PATH, "検査状態ログ画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 11                                 ' 終了
                MyBase.ExecFromClose()
        End Select
    End Sub

    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub


    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmStatusLog_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        'タイトル設定
        Call SetTitleName(Me)

        ' 遠隔時
        If NOA_ONLINE <> g_NOA Then
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' フォームサイズ変更可能
        End If


        '''' フィルター切替タグ消去
        tabFilter_KUBUN.TabVisible = False

        '''' 名称フィルター表示
        tabFilter_KUBUN.SelectedIndex = 0


        '''' 区分フィルタ定義
        '''' 稼動状態
        MyClass.AddCheckBoxKadKind(chkKad_Init, CStr(EM_DIV_KADOU.DIV_KADOU_INIT)) '初期化
        MyClass.AddCheckBoxKadKind(chkKad_Stp, CStr(EM_DIV_KADOU.DIV_KADOU_STOP)) '停止中
        MyClass.AddCheckBoxKadKind(chkKad_Kad, CStr(EM_DIV_KADOU.DIV_KADOU_START)) '稼動中
        MyClass.AddCheckBoxKadKind(chkKad_Ret, CStr(EM_DIV_KADOU.DIV_KADOU_RETRY)) 'リトライ中

        '''' 機器状態（表）
        MyClass.AddCheckBoxKikiKind_O(chkKik_Ok_O, CStr(EM_DIV_KIKI.DIV_KIKI_OK)) '正常
        MyClass.AddCheckBoxKikiKind_O(chkKik_Kei_O, CStr(EM_DIV_KIKI.DIV_KIKI_KEI)) '軽故障
        MyClass.AddCheckBoxKikiKind_O(chkKik_Jyu_O, CStr(EM_DIV_KIKI.DIV_KIKI_JYU))   '重故障

        '''' 機器状態（裏）
        MyClass.AddCheckBoxKikiKind_U(chkKik_Ok_U, CStr(EM_DIV_KIKI.DIV_KIKI_OK)) '正常
        MyClass.AddCheckBoxKikiKind_U(chkKik_Kei_U, CStr(EM_DIV_KIKI.DIV_KIKI_KEI)) '軽故障
        MyClass.AddCheckBoxKikiKind_U(chkKik_Jyu_U, CStr(EM_DIV_KIKI.DIV_KIKI_JYU))   '重故障


        '''' 検査状態（表）
        MyClass.AddCheckBoxKenKind_O(chkKen_Stp_O, CStr(EM_DIV_KENSA.DIV_KENSA_NON)) '停止
        MyClass.AddCheckBoxKenKind_O(chkKen_Ok_O, CStr(EM_DIV_KENSA.DIV_KENSA_OK)) '正常
        MyClass.AddCheckBoxKenKind_O(chkKen_Syu_O, CStr(EM_DIV_KENSA.DIV_KENSA_NG)) '縮退
        MyClass.AddCheckBoxKenKind_O(chkKen_Edge_O, CStr(EM_DIV_KENSA.DIV_KENSA_EDGENG)) 'ｴｯｼﾞ不正
        MyClass.AddCheckBoxKenKind_O(chkKen_NoSave_O, CStr(EM_DIV_KENSA.DIV_KENSA_DBNON)) '未保存
        MyClass.AddCheckBoxKenKind_O(chkKen_NotChk_O, CStr(EM_DIV_KENSA.DIV_KENSA_SKIP)) '未検査
        MyClass.AddCheckBoxKenKind_O(chkKen_StpSys_O, CStr(EM_DIV_KENSA.DIV_KENSA_STOP)) '検査停止


        '''' 検査状態（裏）
        MyClass.AddCheckBoxKenKind_U(chkKen_Stp_U, CStr(EM_DIV_KENSA.DIV_KENSA_NON)) '停止
        MyClass.AddCheckBoxKenKind_U(chkKen_Ok_U, CStr(EM_DIV_KENSA.DIV_KENSA_OK)) '正常
        MyClass.AddCheckBoxKenKind_U(chkKen_Syu_U, CStr(EM_DIV_KENSA.DIV_KENSA_NG)) '縮退
        MyClass.AddCheckBoxKenKind_U(chkKen_Edge_U, CStr(EM_DIV_KENSA.DIV_KENSA_EDGENG)) 'ｴｯｼﾞ不正
        MyClass.AddCheckBoxKenKind_U(chkKen_NoSave_U, CStr(EM_DIV_KENSA.DIV_KENSA_DBNON)) '未保存
        MyClass.AddCheckBoxKenKind_U(chkKen_NotChk_U, CStr(EM_DIV_KENSA.DIV_KENSA_SKIP)) '未検査
        MyClass.AddCheckBoxKenKind_U(chkKen_StpSys_U, CStr(EM_DIV_KENSA.DIV_KENSA_STOP)) '検査停止


        '''' 初期化処理
        Call GetDbInfToSetData()


    End Sub

    ''' <summary>
    ''' 表示色/最大表示件数取得/データセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetDbInfToSetData()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String

        Dim rowMaxCnt As Integer = 0        ' 行数
        Dim clrEvt As Integer = 0           ' イベント色
        Dim clrJyu As Integer = 0           ' 重故障色
        Dim clrKei As Integer = 0           ' 軽故障色


        strSQL = "SELECT LOG表示件数, LOGイベント色, LOG重故障色, LOG軽故障色 FROM " & DB_COMMON_MASTER
        tcls_DB.Open()

        Dim sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return
        Try
            sqlRead.Read()
            rowMaxCnt = sqlRead.GetInt32(0)
            clrEvt = sqlRead.GetInt32(1)
            clrJyu = sqlRead.GetInt32(2)
            clrKei = sqlRead.GetInt32(3)

        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
        End Try

        sqlRead.Close()
        tcls_DB.Close()
        tcls_DB.Dispose()

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ オーバーライド
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' Me.Activated では使い物にならない為、Loadの終了一回だけ非同期でメソッドをコールしてあげるイベントを作ったよ。
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' 元々のメソッド実行
        MyBase.LoadEnding()

        ''''' 拡張分
        '' 画面が開き終わったタイミングで 一回検索を行う
        Call ExecSelect_Log()
    End Sub

    ''' <summary>
    ''' ログ検索開始
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecStart()
        my_btnFunction(0).Enabled = False
        my_btnFunction(8).Enabled = False
        my_btnFunction(9).Enabled = False
        my_btnFunction(10).Enabled = False

        '' ベース実行
        MyBase.ExecStart()
    End Sub

    ''' <summary>
    ''' ログ検索完了
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecEnd()
        my_btnFunction(0).Enabled = True
        my_btnFunction(8).Enabled = True
        my_btnFunction(10).Enabled = True

        'プリンタがなければ、画面印刷無効
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            my_btnFunction(9).Enabled = False
        Else
            my_btnFunction(9).Enabled = True
        End If

        '' ベース実行
        MyBase.ExecEnd()

    End Sub

    ''' <summary>
    ''' 稼動状態名取得(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKadouStatusName(ByVal nVal As KizuCommon.EM_DIV_KADOU) As String
        Select Case nVal
            Case EM_DIV_KADOU.DIV_KADOU_INIT        '初期化中
                Return "初期化中"
            Case EM_DIV_KADOU.DIV_KADOU_STOP        '停止中
                Return "停止中"
            Case EM_DIV_KADOU.DIV_KADOU_START       '稼動中
                Return "稼動中"
            Case EM_DIV_KADOU.DIV_KADOU_RETRY       'リトライ中
                Return "リトライ中"
        End Select

        Return ""

    End Function


    ''' <summary>
    ''' 検査状態名取得(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKensaStatusName(ByVal nVal As KizuCommon.EM_DIV_KENSA) As String
        Select Case nVal
            Case EM_DIV_KENSA.DIV_KENSA_NON
                Return "停止"
            Case EM_DIV_KENSA.DIV_KENSA_OK
                Return "正常"
            Case EM_DIV_KENSA.DIV_KENSA_NG
                Return "縮退"
            Case EM_DIV_KENSA.DIV_KENSA_MENTE
                Return "ﾒﾝﾃ中"
            Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return "検査停止"
            Case EM_DIV_KENSA.DIV_KENSA_EDGENG
                Return "ｴｯｼﾞ不正"
            Case EM_DIV_KENSA.DIV_KENSA_DBNON
                Return "未保存"
            Case EM_DIV_KENSA.DIV_KENSA_SKIP
                Return "未検査"
        End Select

        Return ""

    End Function

    ''' <summary>
    ''' 機器状態名取得(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKiKiStatusName(ByVal nVal As KizuCommon.EM_DIV_KIKI) As String
        Select Case nVal
            Case EM_DIV_KIKI.DIV_KIKI_OK
                Return "正常"
            Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return "軽故障"
            Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return "重故障"
        End Select

        Return ""
    End Function
    ''' <summary>
    ''' 色変え(稼動状態)(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kado(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        '        MyBase.ChangeColor_Kado(e)
        Select Case CStr(e.Value)
            Case "初期化中"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouInit
            Case "停止中"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouStop
            Case "稼動中"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouStart
            Case "リトライ中"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouRetry
        End Select
    End Sub

    ''' <summary>
    ''' 色変え(機器状態)(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kiki(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        Select Case CStr(e.Value)
            Case "正常"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = Color.White
            Case "軽故障"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKikiKei
            Case "重故障"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKikiJyu
        End Select
    End Sub

    ''' <summary>
    ''' 色変え(検査状態)(各ライン用：ベースはfrmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kensa(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        Select Case CStr(e.Value)
            Case "停止"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = g_ColorKensaNon
            Case "正常"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = Color.White
            Case "縮退"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaNg
            Case "ｴｯｼﾞ不正"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaEdgeNg
            Case "未保存"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaDbNon
            Case "未検査"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaSkip
            Case "検査停止"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaStop
            Case "ﾒﾝﾃ中"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorMente
        End Select
    End Sub

End Class
