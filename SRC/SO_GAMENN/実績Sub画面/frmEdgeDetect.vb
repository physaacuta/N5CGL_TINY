'*********************************************************************************
'　エッジ検出状況画面クラス
'	[Ver]
'		Ver.01    2011/06/03  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Public Class frmEdgeDetect
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    'エッジ検出モード変化情報
    Private Enum EM_EDGE_CHANGEINF
        emNagate = 0                '長手位置
        emKenKubun                  '検出区分
        emLEdgeOut                  '左ｴｯｼﾞ範囲外
        emREdgeOut                  '右ｴｯｼﾞ範囲外
        emWidthError                '幅誤差範囲外
        emBeforeFrame               '前ﾌﾚｰﾑ範囲外
        emLEdgeItiReal              'エッジ位置生左
        emREdgeItiReal              'エッジ位置生右
        emLEdgeItiEnd               'エッジ位置最終左
        emREdgeItiEnd               'エッジ位置最終右
    End Enum

    'エッジ未検出フレーム情報
    Private Enum EM_EDGE_FRAMEINF
        emNagate = 0                '長手位置
        emCameraSet                 'カメラセット
        emFrameNo                   'フレームNo
        emGazoUmu                   '画像有無
        emKanriNo                   '管理No
    End Enum

    '---------------------------------------------------------------------------
    ' 定数定義
    '---------------------------------------------------------------------------
    Private Const RSC_IMG_DEFAULT As String = "SO_GAMENN_SPM.疵無し.bmp"       ' デフォルト画像のリースパス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                            'ログ管理クラス
    Private m_strKizuken As String                                          ' 出側管理No
    Private m_nCoilLen As Integer                                           ' コイル実績長 [mm]
    Private m_bNagate As Boolean                                            ' 実績画面長手方向反転(True:反転あり、false:反転なし）

    Private m_typCoilInf As clsDataMap.TYP_COIL_INF                         ' コイル情報

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="cls_Log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal bNagate As Boolean, Optional ByVal strKizuken As String = "")

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        mcls_Log = cls_Log
        m_strKizuken = strKizuken
        m_typCoilInf = typCoilInf
        m_nCoilLen = m_typCoilInf.nCoilLen       ' コイル実績長 [mm]
        m_bNagate = bNagate                     ' 実績画面長手方向反転(True:反転あり、false:反転なし）

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)

    End Sub

    ''' <summary>
    ''' 画面ロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeDetect_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        dgvInitialize(dgvChangeInf_O)
        dgvInitChangeInf(dgvChangeInf_O)

        dgvInitialize(dgvChangeInf_U)
        dgvInitChangeInf(dgvChangeInf_U)


        Data_Disp_Change(dgvChangeInf_O, 0)
        Data_Disp_Change(dgvChangeInf_U, 1)
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
                    cmdEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        Me.Close()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検索結果 グリッド初期化
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 検索結果 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        'Call dgv.DataGridViewSelectionCancel(dgv)						' 選択色無し

        With dgv
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect    ' セル選択

            .ScrollBars = ScrollBars.Both                               ' スクロールバー
            .ColumnHeadersVisible = True                                ' 列ヘッダ表示
            .ColumnHeadersHeight = 45                                   ' 列ヘッダ高さ調整
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .RowCount = 0

        End With

    End Sub
    ''' <summary>
    ''' 列定義(エッジ検出モード変化情報)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitChangeInf(ByVal dgv As tCnt.tCnt_DataGridView)

        With dgv
            .ColumnHeadersHeight = 45         ' 列ヘッダ高さ調整

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 長手位置
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検出区分
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 左ｴｯｼﾞ範囲外
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 右ｴｯｼﾞ範囲外
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 幅誤差範囲外
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 前ﾌﾚｰﾑ範囲外
            .Columns.Add(New DataGridViewTextBoxColumn)   ' エッジ位置生右
            .Columns.Add(New DataGridViewTextBoxColumn)   ' エッジ位置生左
            .Columns.Add(New DataGridViewTextBoxColumn)   ' エッジ位置最終左
            .Columns.Add(New DataGridViewTextBoxColumn)   ' エッジ位置最終右


            '''' 列定義
            '長手位置
            .Columns(EM_EDGE_CHANGEINF.emNagate).HeaderText = "長手位置" & vbCrLf & "[m]"
            .Columns(EM_EDGE_CHANGEINF.emNagate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emNagate).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emNagate).Width = 90
            .Columns(EM_EDGE_CHANGEINF.emNagate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emNagate).ReadOnly = True

            '検出区分
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).HeaderText = "検出区分" & vbCrLf
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).Width = 138
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).ReadOnly = True

            '左ｴｯｼﾞ範囲外
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).HeaderText = "左ｴｯｼﾞ" & vbCrLf & "範囲外"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).ReadOnly = True

            '右ｴｯｼﾞ範囲外
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).HeaderText = "右ｴｯｼﾞ" & vbCrLf & "範囲外"
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).ReadOnly = True

            '幅誤差範囲外
            .Columns(EM_EDGE_CHANGEINF.emWidthError).HeaderText = "幅誤差" & vbCrLf & "範囲外"
            .Columns(EM_EDGE_CHANGEINF.emWidthError).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emWidthError).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emWidthError).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emWidthError).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emWidthError).ReadOnly = True

            '前ﾌﾚｰﾑ範囲外
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).HeaderText = "前ﾌﾚｰﾑ" & vbCrLf & "範囲外"
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).ReadOnly = True

            'エッジ位置生左
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).HeaderText = "ｴｯｼﾞ位置" & vbCrLf & "生左[mm]"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).ReadOnly = True

            'エッジ位置生右
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).HeaderText = "ｴｯｼﾞ位置" & vbCrLf & "生右[mm]"
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).ReadOnly = True

            'エッジ位置最終左
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).HeaderText = "ｴｯｼﾞ位置" & vbCrLf & "最終左[mm]"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).ReadOnly = True

            'エッジ位置最終右
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).HeaderText = "ｴｯｼﾞ位置" & vbCrLf & "最終右[mm]"
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).ReadOnly = True


        End With

    End Sub

    ''' <summary>
    ''' 列定義(エッジ未検出フレーム情報)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitFrameInf(ByVal dgv As tCnt.tCnt_DataGridView)

        With dgv
            .ColumnHeadersHeight = 45         ' 列ヘッダ高さ調整

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 長手位置
            .Columns.Add(New DataGridViewTextBoxColumn)   ' カメラセット
            .Columns.Add(New DataGridViewTextBoxColumn)   ' フレームNo
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 画像有無
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 管理No


            '''' 列定義
            '長手位置
            .Columns(EM_EDGE_FRAMEINF.emNagate).HeaderText = "長手位置" & vbCrLf & "[m]"
            .Columns(EM_EDGE_FRAMEINF.emNagate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emNagate).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emNagate).Width = 90
            .Columns(EM_EDGE_FRAMEINF.emNagate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emNagate).ReadOnly = True

            'カメラNo
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).HeaderText = "ｶﾒﾗ" & vbCrLf & "No"
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).ReadOnly = True

            'フレームNo
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).HeaderText = "ﾌﾚｰﾑ" & vbCrLf & "No"
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).Width = 80
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).ReadOnly = True

            '画像有無
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).HeaderText = "画像有無" & vbCrLf
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).ReadOnly = True

            '管理No
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).HeaderText = "管理No" & vbCrLf
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).ReadOnly = True
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).Visible = False        ' 非表示


        End With

    End Sub

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <returns>復帰情報(Ture:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp_Change(ByVal dgvData As tCnt.tCnt_DataGridView, ByVal nTB As Integer) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim nCnt As Integer = 0         'カウンタ
        Dim nWk As Integer

        'DBオープン
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_EDGE_CHANGE_INF & "]")
            Return False
        End If

        'SQL文作成
        Call CreateEdgeChangeInfSql(strSql, nTB, True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSql)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_EDGE_CHANGE_INF & "]")
            Return False
        End If

        Try

            dgvData.RowCount = 0
            ' データ表示
            While sqlRead.Read()
                dgvData.RowCount += 1

                Dim nPos As Integer = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("長手位置")))

                If m_typCoilInf.bCorrect Then
                    '取得位置に補正率を乗算
                    nPos = CInt(nPos * m_typCoilInf.dCorrect)
                End If

                If m_bNagate = True Then
                    ' 実績画面長手方向反転
                    nPos = m_nCoilLen - nPos
                    If nPos < 0 Then
                        nPos = 0
                    End If
                End If
                dgvData(EM_EDGE_CHANGEINF.emNagate, nCnt).Value = Format(nPos / 1000, "#0.000")
                nWk = sqlRead.GetInt32(sqlRead.GetOrdinal("検出区分"))
                If nWk = 0 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "正常"
                ElseIf nWk = 1 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "片ｴｯｼﾞ優先"
                ElseIf nWk = 2 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "ｾﾝﾀｰﾗｲﾝ振り分け"
                ElseIf nWk = -1 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "ｽｷｯﾌﾟ"
                ElseIf nWk = -9 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "対象外"
                ElseIf nWk = -99 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "初期状態"
                Else
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = ""
                End If

                nWk = sqlRead.GetInt32(sqlRead.GetOrdinal("エッジ不正理由"))
                If 1 = (nWk And &H1) Then
                    ' 左エッジ検出範囲外
                    dgvData(EM_EDGE_CHANGEINF.emLEdgeOut, nCnt).Value = "○"
                End If
                If 2 = (nWk And &H2) Then
                    ' 右エッジ検出範囲外
                    dgvData(EM_EDGE_CHANGEINF.emREdgeOut, nCnt).Value = "○"
                End If
                If 4 = (nWk And &H4) Then
                    ' 板幅許容誤差範囲外
                    dgvData(EM_EDGE_CHANGEINF.emWidthError, nCnt).Value = "○"
                End If
                If 8 = (nWk And &H8) Then
                    ' 前ﾌﾚｰﾑとの許容範囲外
                    dgvData(EM_EDGE_CHANGEINF.emBeforeFrame, nCnt).Value = "○"
                End If

                '小数点以下切り捨て
                dgvData(EM_EDGE_CHANGEINF.emLEdgeItiReal, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("エッジ位置生左")))

                '小数点以下切り捨て
                dgvData(EM_EDGE_CHANGEINF.emREdgeItiReal, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("エッジ位置生右")))

                '小数点以下切り捨て
                dgvData(EM_EDGE_CHANGEINF.emLEdgeItiEnd, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("エッジ位置最終左")))

                '小数点以下切り捨て
                dgvData(EM_EDGE_CHANGEINF.emREdgeItiEnd, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("エッジ位置最終右")))
                nCnt += 1
            End While

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "エッジ検出モード変化表示エラー [" & ex.Message & "]")
        Finally
            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing

        End Try
        If 0 < nCnt Then
            ' カーソルスタート位置は入力エリアの先頭 （テキストボックス)
            dgvData(EM_EDGE_CHANGEINF.emNagate, 0).Selected = True
        End If

        Return True
    End Function

    ''' <summary>
    ''' エッジ検出モード変化管理取得SQL作成
    ''' </summary>
    ''' <param name="strSql">SQL文</param>
    ''' <param name="nTB">表裏区分(0:表,1:裏)</param>
    ''' <param name="bMode">取得モード(True:通常,False:個数(COUNT(*)))</param>
    ''' <remarks></remarks>
    Private Sub CreateEdgeChangeInfSql(ByRef strSql As String, ByVal nTB As Integer, ByVal bMode As Boolean)

        strSql = ""

        If Not bMode Then
            strSql &= " SELECT COUNT(*) "
        Else
            strSql &= " SELECT * "
        End If

        strSql &= " FROM " & DB_EDGE_CHANGE_INF
        strSql &= " WHERE 管理No = '" & m_strKizuken & "'"
        strSql &= " AND 表裏区分 = " & nTB.ToString
        If bMode Then
            If m_bNagate = True Then
                strSql &= " order by 長手位置 DESC"
            Else
                strSql &= " order by 長手位置 ASC"
            End If
        End If

    End Sub


    ''' <summary>
    ''' データ存在有無チェック(エッジ検出実績)
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ChkData_CoilInf(ByVal strKizuKen As String) As Boolean
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String    'SQL文
        Dim strWK As String = ""
        Dim bRet As Boolean = False

        Try

            strSQL = ""
            strSQL &= " SELECT TOP 1 管理No"
            strSQL &= " FROM " & DB_EDGE_COIL_INF
            strSQL &= " WHERE 管理No = '" & strKizuKen & "'"

            'DBオープン
            If Not tcls_DB.Open() Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_EDGE_COIL_INF & "]")
                Return False
            End If

            ' Select文実行
            bRet = tcls_DB.GetSelectKey(strSQL, strWK)

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0}取得に失敗しました。[{1}]", DB_EDGE_COIL_INF, ex.Message))
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DBに接続資源がある場合
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

        Return bRet
    End Function


End Class
