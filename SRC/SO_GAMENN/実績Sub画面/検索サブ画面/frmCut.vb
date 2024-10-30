'*********************************************************************************
'　カット実績クラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmCut

    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '疵検管理No

    ''' <summary>
    ''' パラメータに要求モード(1:オンライン用、0:ツール用）
    ''' </summary>
    ''' <param name="mode">1:オンライン用、0:ツール用</param>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strKizukenNo = strKizukenNo   '管理No

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    Private Sub frmCut_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.tProp_Moveable = True

        'タイトル設定
        Call SetTitleName(Me)

        '画面初期化
        Call InitDsp()

        'IDリスト作成
        Call MakeIDList()

        If 1 = m_mode Then                  ' オンラインモード
            btnRead.Visible = False         ' 読込ボタン「非表示」
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' ツールモード
            btnRead.Visible = True          ' 読込ボタン「表示」
        End If

    End Sub

    ''' <summary>
    ''' IDリスト作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        strSQL = ""

        If m_mode = 1 Then           'オンラインモード
            ' オンラインモードの場合は、管理No.から検索する
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_CUT
            strSQL &= " WHERE 管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY 登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_CUT
            strSQL &= " ORDER BY 登録日付 DESC"
        End If

        'DBオープン
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' データ取得
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        ''命令No
        lblMeiNo.Text = ""

        ''生産NO
        lblProductNo.Text = ""

        ''実績区分
        lblJisDiv.Text = ""

        ''開始位置
        lblStartPos.Text = ""

        ''終了位置
        lblEndPos.Text = ""

        '小番
        lblKoban.Text = ""

        'グラントフラグ
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 画面終了
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '画面終了
        Me.Close()

    End Sub


    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">カット実績伝文</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_CUT) As Boolean


        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData() As Byte        '生伝文バイナリデータ

        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT 生伝文"
        strSQL &= " FROM " & DB_BUF_PRO_R_CUT
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY 登録日付 DESC"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' データ取得
            sqlRead.Read()

            '生伝文
            bytData = CType(sqlRead.GetValue(0), Byte())
            Dim ii As Integer
            ii = tMod.SizeOf(typZiseki)
            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

        Catch ex As Exception
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function

    ''' <summary>
    ''' IDリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>

    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki As PRO_TYPE_CUT = Nothing  '疵検査実績データ

        'データ表示
        If GetData(cmbID.Text, typZiseki) Then
            Call SetData(typZiseki)
        End If
    End Sub

    ''' <summary>
    ''' データセット
    ''' </summary>
    '''<param name="typZiseki">カット実績</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_CUT)

        ''命令No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        ''生産NO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        ''実績区分
        lblJisDiv.Text = typZiseki.data.nMode

        ''開始位置
        lblStartPos.Text = Format(CSng(typZiseki.data.nStart) / 10, "0.0")
        'lblStartPos.Text = typZiseki.data.nStart

        ''終了位置
        lblEndPos.Text = Format(CSng(typZiseki.data.nEnd) / 10, "0.0")
        'lblEndPos.Text = typZiseki.data.nEnd

        '小番
        lblKoban.Text = typZiseki.data.nKoban

        'グラントフラグ
        If 0 = typZiseki.data.Common.nGrandFlg Then
            lblGrant.Text = "正常"
            lblGrant.BackColor = Color.Blue
            lblGrant.ForeColor = Color.Yellow
        Else
            lblGrant.Text = "異常"
            lblGrant.BackColor = Color.Red
            lblGrant.ForeColor = Color.Yellow
        End If

    End Sub



End Class
