'*********************************************************************************
'　疵混入率管理クラス
'	[Ver]
'		Ver.01    2008/10/16  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict Off
Imports tClassLibrary
Public Class clsMixedRate
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SYSLOG出力
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const SYSNO_ZISEKI_NG As Integer = 425                     '// プロコン検査実績情報情報取得NG

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxCol_MRare As Integer = 2           'データ列MAX

    'コイル情報
    Public Structure RATE_CONTROL_DATA
        Dim lblID As Label                       'カット実績のID(1～)
        Dim dgvMixedRate As DataGridView         '混入率データグリッド
        Dim lblNgMsg As Label                    '異常ガイダンス表示用
        Dim btnPre As Button                     '前項ボタン（実績画面のみ資料）
        Dim btnNext As Button                    '次項ボタン（実績画面のみ資料）
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False                ' 重複する呼び出しを検出するには
    Private m_typRate As RATE_CONTROL_DATA                  ' 混入率クラスコントロール
    Private m_dctID As New Dictionary(Of Integer, String)   ' 該当疵検管理Noのカット実績のID
    Private m_nIDCnt As Integer                             ' カット実績のIDのカウンタ

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '項目見出し
        iRate                   '混入率
    End Enum

    ' グリッド 行設定
    Private Enum EM_MRATE_ROW_ID
        iGrade_No01 = 0         'Grade1
        iGrade_No02             'Grade2
        iGrade_No03             'Grade3
        iGrade_No04             'Grade4
        iGrade_No05             'Grade5
        iGrade_No06             'Grade6
    End Enum

    ''' <summary>
    ''' カット実績IDカウンタをセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵混入率クラス専用プロパティ") _
     , System.ComponentModel.Description("カット実績IDカウンタをセット")>
    Public Property tProp_IDCnt() As Integer
        Get
            Return m_nIDCnt
        End Get
        Set(ByVal Value As Integer)
            m_nIDCnt = Value
        End Set
    End Property

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typRatedata">コイル情報</param>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typRatedata As RATE_CONTROL_DATA, ByRef clsLog As tClass.tClass_LogManager)

        '探傷画面
        m_typRate = typRatedata

        'ログクラス
        mcls_Log = clsLog

    End Sub

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
            End If

            'ID取得領域開放
            m_dctID = Nothing

        End If
        Me.disposedValue = True
    End Sub
#Region " IDisposable Support "
    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' 混入率初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        m_typRate.lblID.Text = ""

        '疵混入率データグリッド初期設定
        Call dgvInitialize(m_typRate.dgvMixedRate)

        '疵混入率初期表示
        Call dgvClear(m_typRate.dgvMixedRate)

    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_PROGRP_GRADE

            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.None

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 60                                      ' 列ヘッダサイズ

            '列１ (アドレス) 項目見出し
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 60
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' 水平移動禁止

            '列２ (アドレス) グレード１
            .Columns(EM_MRATE_COL_ID.iRate).HeaderText = "混入率[%]"
            .Columns(EM_MRATE_COL_ID.iRate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MRATE_COL_ID.iRate).Width = 118
            .Columns(EM_MRATE_COL_ID.iRate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iRate).Frozen = True                                       ' 水平移動禁止

        End With

        '項目見出し設定
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No01).Value = "Gr1"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No02).Value = "Gr2"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No03).Value = "Gr3"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No04).Value = "Gr4"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No05).Value = "Gr5"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iGrade_No06).Value = "Gr6"

        '各行セル高さ設定
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To EM_MRATE_ROW_ID.iGrade_No06
            m_typRate.dgvMixedRate.Rows(ii).Height = 40
        Next ii

    End Sub

    ''' <summary>
    ''' グリッドのクリア処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvClear(ByVal dgv As DataGridView)
        Dim ii As Integer = 0

        '桁数確認用
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To MAX_PROGRP_GRADE - 1
            dgv(EM_MRATE_COL_ID.iRate, ii).Value = ""
        Next ii

    End Sub

    ''' <summary>
    ''' 該当管理Noのカット実績のIDを全て取得（実績画面用：データ表示前に必ず取得）
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Public Function GetCutID(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nCnt As Integer = 0    'カウンタ
        Dim bRet As Boolean = True

        strSQL = ""
        strSQL &= " SELECT ID"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE 管理No = '" & strKizuken & "'"
        strSQL &= " ORDER BY 登録日付 ASC"

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB読み込みエラー [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        Try

            'カットID保存領域クリア
            m_dctID.Clear()

            ' データ取得
            While sqlRead.Read()
                m_dctID.Add(nCnt, sqlRead.GetString(0))
                nCnt += 1
            End While

            '情報の先頭を表示レコードに設定
            m_nIDCnt = 0

            'データ0件は時は異常を返す。
            If nCnt = 0 Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "カット実績ID取得エラー [0件]")
                '異常ガイダンス表示
                m_typRate.lblNgMsg.Text = "疵混入率情報取得に失敗しました。"
                'syslog出力
                If blnSyslog Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[管理No = " & strKizuken & " ,ID = ]"))
                End If
                bRet = False
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "カット実績ID取得エラー[" & ex.Message & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "疵混入率情報取得に失敗しました。"
            bRet = False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' データ表示（実績画面用：事前に取得した実績伝文IDを選択して表示）
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Public Function DspRateData(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim strCutID As String = ""      'カット実績ID
        Dim bRet As Boolean = True       '処理結果NG判定

        'ID取得
        strCutID = m_dctID(m_nIDCnt)     'プロパティからフォーム画面でm_nIDCntを＋－制御する（ページ移動）

        m_typRate.lblID.Text = CUT_RESULT_ID_NO(m_nIDCnt)
        'データ表示
        If Not DspData(strKizuken, strCutID, blnSyslog) Then
            bRet = False
        End If

        'ボタ有効、無効
        Call SetPreNextBotton()

        Return bRet

    End Function

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="strCutID">実績伝文ID</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Public Function DspData(ByVal strKizuken As String, ByVal strCutID As String,
                            ByVal blnSyslog As Boolean) As Boolean

        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '疵検査実績データ

        'データ取得
        If Not GetData(strKizuken, typZiseki, strCutID, blnSyslog) Then
            Call DspInit()
            Return False
        End If

        'データセット
        SetData(typZiseki)

        'データグリッドをリフレッシュイベントで表示
        m_typRate.dgvMixedRate.Refresh()

        Return True

    End Function

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="typZiseki">疵検査実績データ</param>
    ''' <param name="strCutID">実績伝文ID</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strKizuken As String, ByRef typZiseki As PRO_TYPE_ZISEKI, ByVal strCutID As String,
                            ByVal blnSyslog As Boolean) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData() As Byte        '生伝文バイナリデータ
        Dim bRet As Boolean = False

        strSQL = ""
        strSQL &= " SELECT 生伝文"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE 管理No = '" & strKizuken & "'"
        strSQL &= " AND   ID = '" & strCutID & "'"

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB接続エラー [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_BUF_PRO_S_ZISEKI & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB読み込みエラー [" & DB_BUF_PRO_S_ZISEKI & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_BUF_PRO_S_ZISEKI & "]"))
            End If
            Return False
        End If

        Try

            ' データ取得
            sqlRead.Read()

            If sqlRead.HasRows Then

                '生伝文
                bytData = CType(sqlRead.GetValue(0), Byte())

                tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

                '正常終了
                bRet = True

            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵混入率取得エラー [ID = " & strCutID & "]")
                '異常ガイダンス表示
                m_typRate.lblNgMsg.Text = "疵混入率情報取得に失敗しました。"
                'syslog出力
                If blnSyslog Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[管理No = " & strKizuken & " ,ID = " & strCutID & "]"))
                End If
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵混入率取得エラー [" & ex.Message & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "疵混入率情報取得に失敗しました。"
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' データセット
    ''' </summary>
    '''<param name="typZiseki">プロコン実績データ</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_ZISEKI)

        Dim ii As Integer   'ループカウンタ

        '桁数確認用
        For ii = EM_MRATE_ROW_ID.iGrade_No01 To MAX_PROGRP_GRADE - 1
            m_typRate.dgvMixedRate(EM_MRATE_COL_ID.iRate, ii).Value = Format(typZiseki.data.grade_inf(ii).nMixedRate / 100, "#0.00")
        Next ii

    End Sub

    ''' <summary>
    ''' 前項、次項ボタンの有効、無効を設定（実績画面用）
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetPreNextBotton()

        'ID取得が0件の場合は前項／次項ボタンを無効化し、終了。
        If m_dctID.Count = 0 Then
            m_typRate.btnPre.Enabled = False
            m_typRate.btnNext.Enabled = False
            Exit Sub
        End If

        'ID取得正常かつID先頭レコードの場合は前項ボタンのみ無効化する。
        If m_nIDCnt = 0 Then
            m_typRate.btnPre.Enabled = False
        Else
            m_typRate.btnPre.Enabled = True
        End If

        'ID取得正常かつID最終レコードの場合は次項ボタンのみ無効化する。
        If m_nIDCnt = m_dctID.Count - 1 Then
            m_typRate.btnNext.Enabled = False
        Else
            m_typRate.btnNext.Enabled = True
        End If

    End Sub

    ''' <summary>
    ''' 疵混入率サイズ調整
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ResizeMixedRate()
        Dim nColumnHeaderHeight As Integer = 0
        Dim nColumnHeight As Integer = 0
        Dim nColumnHeaderFontSize As Integer = 0
        Dim nColumnFontSize As Integer = 0

        ' コントロールサイズからヘッダー、行の高さを算出する。
        nColumnHeight = CInt(Math.Floor(m_typRate.dgvMixedRate.Height * 0.8 / 6))
        nColumnHeaderHeight = m_typRate.dgvMixedRate.Height - nColumnHeight * 6 - SystemInformation.Border3DSize.Height

        ' ヘッダー、行の高さからフォントサイズを調整する。
        nColumnHeaderFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRate, "混入率[%]", 16, 110, nColumnHeaderHeight))
        nColumnFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRate, "99.99", 20, 110, nColumnHeight))

        ' 調整
        With m_typRate.dgvMixedRate
            .ColumnHeadersHeight = nColumnHeaderHeight
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", nColumnHeaderFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            For ii As Integer = 0 To .RowCount - 1
                .Rows(ii).Height = nColumnHeight
            Next ii
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", nColumnFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
        End With
    End Sub

End Class
