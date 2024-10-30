'*******************************************************************************
'　検査実績検索画面
'	[Ver]
'		Ver.01    2015/08/28  初版
'
'	[メモ]
'	検査実績画面機能を表示する	
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Imports System.Data.SqlClient

Public Class frmSearch

    '---------------------------------------------------------------------------
    ' 列挙体
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 検索結果欄項目
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_RESULT_COL
        nDATE = 0           '通板日付
        nCoilNo             '生産No
        nOrderNo            '命令No
        nMaterial           '材質
        nHyomen             '表面処理
        nCoilLen            '実測長(m)
        nProConLen          'ﾌﾟﾛｺﾝ長(m)
        nDspLen             '表示長(m)
        nAtu                '材料厚(mm)
        nWid                '材料幅(mm)
        nKensaKijun_O       '検査基準(表)
        'nKensaKijun_U       '検査基準(裏)
        nMixedRate          '混入率[%](表/裏)
        nMuke               '向先
        nHinCode            '品種
        nNextCode           '計画次工程
        nDefCnt_O           '表示疵個数(表)
        nDefCnt_U           '表示疵個数(裏)
        nDefCnt_Total       '表示総疵個数
        nDelHarmfulCnt_O    '表削除有害疵個数
        nDelHarmfulCnt_U    '裏削除有害疵個数
        nDelHarmlessCnt_O   '表削除無害疵個数
        nDelHarmlessCnt_U   '裏削除無害疵個数
        nSaveKizu_O         '表保存疵個数
        nSaveKizu_U         '裏保存疵個数
        nPCode              '検査パターン
        nSCode              '検査表面状態(表/裏)
        nKanriNo            '疵管理No
        nUntenStat          '運転状態
        nKakuno             '格納区分
        nKensaStat_O        '検査状態(表)
        nKensaStat_U        '検査状態(裏)
        nKikiStat_O         '機器状態(表)
        nKikiStat_U         '機器状態(裏)
        nToyuSplr           '塗油スプレー装置状態
        nPCCoil             'P/Cコイル
        nPCCut              'P/Cカット
        nPCSaisun           'P/C採寸
        nPCZiseki           'P/C実績
        nFFRecv             'FF受信(前工程情報)
        nFFSend             'FF送信
        nKeyInfo            'キー情報(非表示)
        MAX_RESULT_COL      '検索結果列数
    End Enum

    ''' <summary>
    ''' 実績検索画面の戻り値
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum SEARCH_RETURN
        emSELECT_COIL = 0                                                       ' コイル選択
        emCANCEL                                                                ' キャンセル
    End Enum

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private mcls_Log As tClass_LogManager                                       ' ログ管理
    Private mcls_Param As clsParamManager                                       ' パラメータ管理
    Private mcls_PreParam As clsPreParamManager                                     ' パラメータ管理

    Private m_nSelectRowNo As Integer                                           ' 現在の選択コイルインデックス
    Private m_nRetCode As SEARCH_RETURN                                         ' このクラスの戻り値

    Private mcls_DataMap As clsDataMap                                          ' マップ情報
    Private mcls_ReadDatamap As clsDataMap                                      ' 読み込み用のマップ情報

    Private m_nSearchTimeOut As Integer                                         ' コマンド実行タイムアウト時間[検索用(単位：秒)]

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
#Region "プロパティ"
    ''' <summary>
    ''' 検索結果のマップ情報を取得します
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DataMap() As clsDataMap
        Get
            Return mcls_DataMap
        End Get
    End Property

    ''' <summary>
    ''' 展開時の長手方向反転指定を取得します（左右反転）
    ''' </summary>
    ''' <returns>True:反転する False:反転しない</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsTurnNagate() As Boolean
        Get
            If chkNagate.Checked Then
                Return True
            Else
                Return False
            End If
            Return False
        End Get
    End Property

    ''' <summary>
    ''' 展開時の幅方向反転指定を取得します
    ''' </summary>
    ''' <returns>True:反転する False:反転しない</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsTurnDS() As Boolean
        Get
            Return chkHaba.Checked
        End Get
    End Property

    ''' <summary>
    ''' リターンコード
    ''' </summary>
    ''' <returns>検索操作結果</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ReturnCode() As SEARCH_RETURN
        Get
            Return m_nRetCode
        End Get
    End Property

#End Region


    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 実績検索画面を作成します
    ''' </summary>
    ''' <param name="param">パラメータ管理</param>
    ''' <param name="log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal param As clsParamManager, ByRef preparam As clsPreParamManager, ByVal log As tClass_LogManager)
        Try
            ' この呼び出しは、Windows フォーム デザイナで必要です。
            InitializeComponent()
            ' InitializeComponent() 呼び出しの後で初期化を追加します。

            '-------------------------------------------------------------------
            ' パラメータの初期化
            '-------------------------------------------------------------------
            mcls_Log = log
            mcls_Param = param
            mcls_PreParam = preparam
            m_nSelectRowNo = -1
            m_nRetCode = SEARCH_RETURN.emCANCEL

            '-------------------------------------------------------------------
            ' レイアウトの初期化
            '-------------------------------------------------------------------
            txtCoil.Text = ""                                                   ' 検索欄生産No
            txtKizukenNo.Text = ""                                              ' 検索欄命令No
            txtKanriNo.Text = ""                                                ' 検索欄管理No
            lblGuide.Text = ""                                                  ' ポップアップメッセージ
            dgvInitialize(dgvResult)                                            ' 初期化処理

            chkDateSet.Checked = True                                           ' 日付フィルターを入れる
            rdoSortDesc.Checked = True                                          ' 検索結果を降順にする
            chkKizuNon.Checked = True                                           ' 疵0コのものは表示しない
            chkNagate.Checked = False
            radDspLikeFFsnd.Visible = True
            Me.tProp_Moveable = True                                            ' 画面移動許可

            '-------------------------------------------------------------------
            ' 基底クラス設定
            '-------------------------------------------------------------------
            '''' フィルタ初期設定
            '' 疵種・グレードフィルターの情報を基底クラスにセット
            MyBase.tProp_SettingIniName = LOCALINI_NAME
            MyBase.tProp_SettingIniSession = SO_GAMENN

            Call MyBase.SetFilterData(dgvDispType, DB_TYPE_NAME)                ' 疵種チェックリスト初期化
            Call MyBase.ReadFilterData(dgvDispType, DB_TYPE_NAME)               ' フィルター保存値反映
            Call MyBase.SetFilterData(dgvDispGrade, DB_GRADE_NAME)              ' グレードチェックリスト初期化
            Call MyBase.ReadFilterData(dgvDispGrade, DB_GRADE_NAME)             ' フィルター保存値反映

            '-------------------------------------------------------------------
            ' ファンクションキー設定
            '-------------------------------------------------------------------
            AddFunctionButton(btnSearch, 5 - 1)
            AddFunctionButton(btnSelect, 6 - 1)
            AddFunctionButton(btnEnd, 12 - 1)
            '-------------------------------------------------------------------
            ' 表示サイズ設定
            '-------------------------------------------------------------------
            Dim clsMonitor() As Screen = Screen.AllScreens()                    'モニター一覧
            Dim nFormPos As Integer = g_nFormPos
            If clsMonitor.Length < nFormPos Then                                '表示したいモニターがない場合
                nFormPos = 1                                                    'プライマリディスプレイに表示
            End If
            Dim nH As Integer = clsMonitor(nFormPos - 1).Bounds.Size.Height
            Dim nW As Integer = clsMonitor(nFormPos - 1).Bounds.Size.Width

            'フォームサイズがモニタサイズより大きいときモニタサイズに合わせる
            If Me.Height > nH Then
                Me.Height = nH
            End If

            'タイトル設定
            Call SetTitleName(Me)

        Catch ex As Exception
            log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面生成異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 検索結果 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)

        Try
            With dgv
                .ScrollBars = ScrollBars.Both                                   ' スクロールバー
                .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' セル選択
                .ColumnHeadersHeight = 38                                       ' 列ヘッダ高さ調整
                .DefaultCellStyle.WrapMode = DataGridViewTriState.False

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
                '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
                .DefaultCellStyle().SelectionForeColor = Color.Black            '
                .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
                .DefaultCellStyle().ForeColor = Color.Black
                .RowCount = 0

                For col As EM_RESULT_COL = EM_RESULT_COL.nDATE To EM_RESULT_COL.MAX_RESULT_COL
                    Select Case col
                        Case EM_RESULT_COL.nDATE
                            ' 通板日時
                            dgvColSetText(dgv, "通板日時", 178, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nCoilNo
                            ' 生産No
                            dgvColSetText(dgv, "生産No", 120, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nOrderNo
                            ' 命令No
                            dgvColSetText(dgv, "命令No", 80, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMaterial
                            ' 材質
                            dgvColSetText(dgv, "材質", 50, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nHyomen
                            ' 表面処理
                            dgvColSetText(dgv, "表面処理", 50, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nCoilLen
                            ' コイル長
                            dgvColSetText(dgv, "コイル長" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nProConLen
                            ' ﾌﾟﾛｺﾝ長
                            dgvColSetText(dgv, "ﾌﾟﾛｺﾝ長" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDspLen
                            ' 表示長
                            dgvColSetText(dgv, "表示長" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nAtu
                            ' 材料厚
                            dgvColSetText(dgv, "材料厚" & vbCrLf & "[mm]", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nWid
                            ' 材料幅
                            dgvColSetText(dgv, "材料幅" & vbCrLf & "[mm]", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaKijun_O
                            ' 検査基準
                            dgvColSetText(dgv, "検査" & vbCrLf & "基準", 60, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        'Case EM_RESULT_COL.nKensaKijun_U
                        '    ' 検査基準
                        '    dgvColSetText(dgv, "検査基準" & vbCrLf & "(裏)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMixedRate
                            ' 混入率
                            dgvColSetText(dgv, "混入率[%]" & vbCrLf & "(表/裏)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMuke
                            ' 向先
                            dgvColSetText(dgv, "向先", 300, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nHinCode
                            ' 品種
                            dgvColSetText(dgv, "品種", 50, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nNextCode
                            ' 計画次工程
                            dgvColSetText(dgv, "計画" & vbCrLf & "次工程", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_O
                            ' 表示疵個数(表)
                            dgvColSetText(dgv, "表示疵" & vbCrLf & "個数(表)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_U
                            ' 表示疵個数(表)
                            dgvColSetText(dgv, "表示疵" & vbCrLf & "個数(裏)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_Total
                            ' 表示疵個数(表)
                            dgvColSetText(dgv, "表示疵" & vbCrLf & "総個数", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmfulCnt_O
                            ' 表削除有害疵個数
                            dgvColSetText(dgv, "削除有害" & vbCrLf & "個数(表)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmfulCnt_U
                            ' 裏削除有害疵個数
                            dgvColSetText(dgv, "削除有害" & vbCrLf & "個数(裏)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmlessCnt_O
                            ' 表削除無害疵個数
                            dgvColSetText(dgv, "削除無害" & vbCrLf & "個数(表)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmlessCnt_U
                            ' 裏削除無害疵個数
                            dgvColSetText(dgv, "削除無害" & vbCrLf & "個数(裏)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSaveKizu_O
                            ' 表保存疵個数
                            dgvColSetText(dgv, "保存疵" & vbCrLf & "個数(表)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSaveKizu_U
                            ' 裏保存疵個数
                            dgvColSetText(dgv, "保存疵" & vbCrLf & "個数(裏)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCode
                            ' 検査パターン
                            dgvColSetText(dgv, "検査" & vbCrLf & "ﾊﾟﾀｰﾝ", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSCode
                            ' 検査表面状態
                            dgvColSetText(dgv, "検査" & vbCrLf & "表面状態", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKanriNo
                            ' 管理No
                            dgvColSetText(dgv, "疵検管理No", 160, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nUntenStat
                            ' 運転状態
                            dgvColSetText(dgv, "運転状態", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKakuno
                            ' 格納区分
                            dgvColSetText(dgv, "格納区分", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaStat_O
                            ' 検査状態(表)
                            dgvColSetText(dgv, "検査状態" & vbCrLf & "(表)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaStat_U
                            ' 検査状態(裏)
                            dgvColSetText(dgv, "検査状態" & vbCrLf & "(裏)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKikiStat_O
                            ' 機器状態(表)
                            dgvColSetText(dgv, "機器状態" & vbCrLf & "(表)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKikiStat_U
                            ' 機器状態(裏)
                            dgvColSetText(dgv, "機器状態" & vbCrLf & "(裏)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nToyuSplr
                            ' 塗油スプレー装置状態
                            dgvColSetText(dgv, "塗油ｽﾌﾟﾚｰ" & vbCrLf & "装置状態", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCCoil
                            'P/Cｺｲﾙ
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "ｺｲﾙ", "ｺｲﾙ", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCCut
                            'P/Cｶｯﾄ
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "ｶｯﾄ", "ｶｯﾄ", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCSaisun
                            'P/C採寸
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "採寸", "採寸", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCZiseki
                            'P/C実績
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "実績", "実績", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nFFRecv
                            'P/C実績
                            dgbColSetButton(dgv, "FF" & vbCrLf & "受信", "FF受", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nFFSend
                            'P/C実績
                            dgbColSetButton(dgv, "FF" & vbCrLf & "送信", "FF送", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKeyInfo
                            ' キー情報(非表示)
                            dgvColSetText(dgv, "キー情報", 200, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)
                    End Select
                Next
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面結果欄初期化異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' コイル検索SQL生成
    ''' </summary>
    ''' <returns>コイル検索SQL生成</returns>
    ''' <remarks></remarks>
    Private Function GetSQLSearchCoil() As String
        Dim strSQL As String = ""       ' コイル検索SQL
        Dim strFilter As String = ""    ' 疵種・グレードチェックリスト結果
        Dim bWk As Boolean = True       ' 表面状態検査フィルタ項目追加

        Try
            '欠陥情報フィルター
            With dgvDispType
                For ii As Integer = 0 To .RowCount - 1
                    If Not CType(dgvDispType(EM_FILTER_COL.emCheck, ii).Value, Boolean) Then
                        strFilter &= " AND NOT d.判定疵種 = " & dgvDispType(EM_FILTER_COL.emEdasysID, ii).Value.ToString
                    End If
                Next ii
            End With

            'グレードフィルター
            With dgvDispGrade
                For ii As Integer = 0 To .RowCount - 1
                    If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, ii).Value, Boolean) Then
                        strFilter &= " AND NOT d.判定グレード = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, ii).Value.ToString
                    End If
                Next ii
            End With

            strSQL = ""
            '-------------------------------------------------------------------
            ' SELECT句
            '-------------------------------------------------------------------
            strSQL &= "SELECT"
            strSQL &= " RES.登録日付 as 通板日付"
            strSQL &= " ,ISNULL(INF.生産No,'') as 生産No"
            strSQL &= " ,ISNULL(INF.命令No,'') as 命令No"
            strSQL &= " ,ISNULL(INF.主材質,'') as 主材質"
            strSQL &= " ,ISNULL(INF.表面処理1桁目,'') as 表面処理"
            strSQL &= " ,ISNULL(INF.品種,'') as 品種"
            strSQL &= " ,ISNULL(RES.検査実測長,0) as 検査実測長"
            strSQL &= " ,ISNULL(RES.プロコン実測長,0) as プロコン実測長"
            strSQL &= " ,ISNULL(RES.表示実測長,0) as 表示実測長"
            strSQL &= " ,ISNULL(INF.材料板厚,0) as 材料板厚"
            strSQL &= " ,ISNULL(INF.材料板幅,0) as 材料板幅"
            strSQL &= " ,ISNULL(INF.検査基準_表,'') as 検査基準_表"
            strSQL &= " ,ISNULL(INF.検査基準_裏,'') as 検査基準_裏"
            strSQL &= " ,ISNULL(CNT_T.個数, 0) as 表示疵個数表"
            strSQL &= " ,ISNULL(CNT_B.個数, 0) as 表示疵個数裏"
            strSQL &= " ,ISNULL(INF.向先,'') as 向先"
            strSQL &= " ,ISNULL(INF.計画次工程コード,'') as 計画次工程コード"
            'strSQL &= " ,ISNULL(RES.表保存対象疵個数,0) as 表保存対象疵個数"
            'strSQL &= " ,ISNULL(RES.裏保存対象疵個数,0) as 裏保存対象疵個数"
            strSQL &= " ,ISNULL(RES.表削除有害疵個数,0) as 表削除有害疵個数"
            strSQL &= " ,ISNULL(RES.裏削除有害疵個数,0) as 裏削除有害疵個数"
            strSQL &= " ,ISNULL(RES.表削除無害疵個数,0) as 表削除無害疵個数"
            strSQL &= " ,ISNULL(RES.裏削除無害疵個数,0) as 裏削除無害疵個数"
            strSQL &= " ,ISNULL(RES.表保存疵個数,0) as 表保存疵個数"
            strSQL &= " ,ISNULL(RES.裏保存疵個数,0) as 裏保存疵個数"

            strSQL &= " ,ISNULL(RES.検査パターンコード_表,'') as 検査パターン_表"
            strSQL &= " ,ISNULL(RES.検査パターンコード_裏,'') as 検査パターン_裏"
            strSQL &= " ,ISNULL(RES.検査表面状態No_表,'') as 検査表面状態_表"
            strSQL &= " ,ISNULL(RES.検査表面状態No_裏,'') as 検査表面状態_裏"

            strSQL &= " ,ISNULL(RES.運転状態,0) as 運転状態"
            strSQL &= " ,ISNULL(RES.実績格納区分,0) as 実績格納区分"

            strSQL &= " ,ISNULL(RES.検査状態_表,0) as 検査状態_表"
            strSQL &= " ,ISNULL(RES.検査状態_裏,0) as 検査状態_裏"
            strSQL &= " ,ISNULL(RES.機器状態_表,0) as 機器状態_表"
            strSQL &= " ,ISNULL(RES.機器状態_裏,0) as 機器状態_裏"
            strSQL &= " ,ISNULL(RES.塗油スプレー装置状態,0) as 塗油スプレー装置状態"

            strSQL &= " ,ISNULL(INF.キー情報,'') as キー情報"
            strSQL &= " ,ISNULL(RES.補正率,1) as 補正率"
            strSQL &= " ,RES.管理No"

            '-------------------------------------------------------------------
            ' FROM句 出側物理コイル実績テーブル
            '-------------------------------------------------------------------
            strSQL &= " FROM " & DB_COIL_RESULT & " RES "

            '-------------------------------------------------------------------
            ' 外部結合 コイル情報
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " INF "
            strSQL &= " ON RES.管理No = INF.管理No "

            '-------------------------------------------------------------------
            ' 外部結合  表 表示疵個数
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN (SELECT count(*) as 個数, d.管理No from "
            'strSQL &= DB_DEFECT_INF_T & " d "

            If radDspLikeFFsnd.Checked Then
                'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " df "
                strSQL &= DB_ACCESS_NEXT_DEFECT_INF & " df , "
            End If

            strSQL &= DB_DEFECT_INF_T & " d "

            strSQL &= " WHERE d.重複結果 <> 1 "

            If radDspLikeFFsnd.Checked Then
                strSQL &= "AND ((df.管理No = d.管理No) AND (df.疵No=d.疵No)) "
            End If

            '探傷画面と同一表示チェックオン
            If radDspLikeTansho.Checked Then
                strSQL &= "AND d.表示対象区分_探傷画面=1 "
            End If

            'フィルター表示無しチェックオン
            If radNormalDsp.Checked Then
                strSQL &= strFilter
            End If

            strSQL &= " Group by d.管理No ) CNT_T "
            strSQL &= " ON RES.管理No = CNT_T.管理No "

            '-------------------------------------------------------------------
            ' 外部結合  裏 表示疵個数
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN (SELECT count(*) as 個数, d.管理No from "
            'strSQL &= DB_DEFECT_INF_B & " d "

            If radDspLikeFFsnd.Checked Then
                'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " df "
                strSQL &= DB_ACCESS_NEXT_DEFECT_INF & " df , "
            End If

            strSQL &= DB_DEFECT_INF_B & " d "

            strSQL &= " WHERE d.重複結果 <> 1 "

            If radDspLikeFFsnd.Checked Then
                strSQL &= "AND ((df.管理No = d.管理No) AND (df.疵No=d.疵No)) "
            End If

            '探傷画面と同一表示チェックオン
            If radDspLikeTansho.Checked Then
                strSQL &= "AND d.表示対象区分_探傷画面=1 "
            End If

            'フィルター表示無しチェックオン
            If radNormalDsp.Checked Then
                strSQL &= strFilter
            End If

            strSQL &= " Group by d.管理No ) CNT_B "
            strSQL &= " ON RES.管理No = CNT_B.管理No "

            '-------------------------------------------------------------------
            ' 条件句
            '-------------------------------------------------------------------
            strSQL &= " WHERE 1 = 1"                                            ' 先頭はコレ

            'コイル検索
            If "" <> txtCoil.Text.Trim Then                                     ' 生産No
                strSQL = strSQL & " and INF.生産No like '" & txtCoil.Text.Trim & "' "
            End If

            If "" <> txtKizukenNo.Text.Trim Then                                ' 命令No
                strSQL = strSQL & " and INF.命令No like '" & txtKizukenNo.Text.Trim & "' "
            End If

            If "" <> txtKanriNo.Text.Trim Then                                ' 管理No
                strSQL = strSQL & " and RES.管理No like '" & txtKanriNo.Text.Trim & "' "
            End If

            If chkDateSet.Checked Then                                          ' 日時（開始）
                strSQL = strSQL & " and RES.登録日付 >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
                If chkTimeSet.Checked = True Then                               ' 時間指定時
                    strSQL &= " " & Format(dtpTimeStart.Value, "HH:mm:ss") & "' "
                Else
                    strSQL &= " " & "00:00:00' "
                End If
            End If

            If chkDateSet.Checked Then                                          ' 日時（終了）
                strSQL = strSQL & " and RES.登録日付 <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
                If chkTimeSet.Checked = True Then                               ' 時間指定時
                    strSQL &= " " & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' "
                Else
                    strSQL &= "23:59:59' "
                End If
            End If

            '-------------------------------------------------------------------
            ' オーダー句
            '-------------------------------------------------------------------
            If rdoSortAsc.Checked = True Then
                strSQL &= " ORDER BY RES.登録日付 ASC"
            Else
                strSQL &= " ORDER BY RES.登録日付 DESC"
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面コイル検索SQL生成異常 [{0}] SQL={1}", ex.Message, strSQL))
            strSQL = ""
        End Try

        Return strSQL

    End Function


    ''' <summary>
    ''' コイル検索 と 検索結果表示
    ''' </summary>
    ''' <param name="strSQL">検索SQL</param>
    ''' <remarks></remarks>
    Private Sub SearchCoil(ByVal strSQL As String)
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim nRow As Integer = 0

        Try
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim bDbRet As Boolean = False

            bDbRet = clsDB.Open()                                               ' DBとの接続開始
            If bDbRet = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面コイル検索 DB接続失敗"))
                Exit Sub
            End If

            'コマンド実行タイムアウト時間セット
            clsDB.SetCommandTimeOut(m_nSearchTimeOut)
            read = clsDB.SelectExecute(strSQL)                                  ' コイル検索実行
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面コイル検索 DBデータ取得失敗"))
                Exit Sub
            End If

            dgvResult.RowCount = 0

            With read
                While .Read() = True
                    Dim nCoilLen As Integer = CInt(.GetSqlInt32(.GetOrdinal("検査実測長")))
                    Dim nKizuNum_O As Integer = CInt(.GetSqlInt32(.GetOrdinal("表示疵個数表")))
                    Dim nKizuNum_U As Integer = CInt(.GetSqlInt32(.GetOrdinal("表示疵個数裏")))
                    Dim dCorrect As Double = CDbl(.GetDouble(.GetOrdinal("補正率")))

                    If chkKizuNon.Checked Then
                        ' 0件実績を表示しない
                        If 0 = nKizuNum_O And 0 = nKizuNum_U Then
                            Continue While
                        End If
                    End If
                    If chkCorrect.Checked Then
                        '補正率使用
                        nCoilLen = CInt(nCoilLen * dCorrect)
                    End If

                    dgvResult.RowCount += 1

                    ' 通板日付
                    dgvResult(EM_RESULT_COL.nDATE, nRow).Value = .GetDateTime(.GetOrdinal("通板日付")).ToString("yyyy/MM/dd HH:mm:ss")
                    ' 生産No
                    dgvResult(EM_RESULT_COL.nCoilNo, nRow).Value = .GetSqlValue(.GetOrdinal("生産No")).ToString().Trim
                    ' 命令No
                    dgvResult(EM_RESULT_COL.nOrderNo, nRow).Value = .GetSqlValue(.GetOrdinal("命令No")).ToString().Trim
                    ' 材質
                    dgvResult(EM_RESULT_COL.nMaterial, nRow).Value = .GetSqlValue(.GetOrdinal("主材質")).ToString().Trim
                    ' 表面処理
                    dgvResult(EM_RESULT_COL.nHyomen, nRow).Value = .GetSqlValue(.GetOrdinal("表面処理")).ToString().Trim
                    ' コイル長
                    dgvResult(EM_RESULT_COL.nCoilLen, nRow).Value = Format(CSng(nCoilLen / 1000), "#0.0")
                    ' プロコン長
                    dgvResult(EM_RESULT_COL.nProConLen, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("プロコン実測長"))) / 1000), "#0.0")
                    ' 表示長
                    dgvResult(EM_RESULT_COL.nDspLen, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("表示実測長"))) / 1000), "#0.0")
                    ' 材料板厚
                    dgvResult(EM_RESULT_COL.nAtu, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("材料板厚"))) / 1000), "#0.000")
                    ' 材料板幅
                    dgvResult(EM_RESULT_COL.nWid, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("材料板幅")))), "#0")
                    ' 検査基準(表)
                    dgvResult(EM_RESULT_COL.nKensaKijun_O, nRow).Value = .GetSqlValue(.GetOrdinal("検査基準_表")).ToString().Trim
                    ' 混入率[%](表/裏)
                    dgvResult(EM_RESULT_COL.nMixedRate, nRow).Value = String.Format("{0:0.0}/{1:0.0}",
                                                                        nKizuNum_O / CSng(nCoilLen / 1000) * 100,
                                                                        nKizuNum_U / CSng(nCoilLen / 1000) * 100)
                    ' 向先
                    dgvResult(EM_RESULT_COL.nMuke, nRow).Value = .GetSqlValue(.GetOrdinal("向先")).ToString().Trim
                    ' 品種
                    dgvResult(EM_RESULT_COL.nHinCode, nRow).Value = .GetSqlValue(.GetOrdinal("品種")).ToString().Trim
                    ' 計画次工程
                    dgvResult(EM_RESULT_COL.nNextCode, nRow).Value = .GetSqlValue(.GetOrdinal("計画次工程コード")).ToString().Trim
                    ' 表示疵個数(表)
                    dgvResult(EM_RESULT_COL.nDefCnt_O, nRow).Value = Format(nKizuNum_O, "#,##0")
                    ' 表示疵個数(裏)
                    dgvResult(EM_RESULT_COL.nDefCnt_U, nRow).Value = Format(nKizuNum_U, "#,##0")
                    ' 表示疵総個数
                    dgvResult(EM_RESULT_COL.nDefCnt_Total, nRow).Value = Format(nKizuNum_O + nKizuNum_U, "#,##0")
                    ' 表削除有害疵個数
                    dgvResult(EM_RESULT_COL.nDelHarmfulCnt_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("表削除有害疵個数"))), "#,##0")
                    ' 裏削除有害疵個数
                    dgvResult(EM_RESULT_COL.nDelHarmfulCnt_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("裏削除有害疵個数"))), "#,##0")
                    ' 表削除無害疵個数
                    dgvResult(EM_RESULT_COL.nDelHarmlessCnt_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("表削除無害疵個数"))), "#,##0")
                    ' 裏削除無害疵個数
                    dgvResult(EM_RESULT_COL.nDelHarmlessCnt_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("裏削除無害疵個数"))), "#,##0")
                    ' 表保存疵個数
                    dgvResult(EM_RESULT_COL.nSaveKizu_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("表保存疵個数"))), "#,##0")
                    ' 裏保存疵個数
                    dgvResult(EM_RESULT_COL.nSaveKizu_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("裏保存疵個数"))), "#,##0")
                    ' 検査パターン
                    dgvResult(EM_RESULT_COL.nPCode, nRow).Value = CInt(.GetSqlInt32(.GetOrdinal("検査パターン_表"))).ToString & "/" & CInt(.GetSqlInt32(.GetOrdinal("検査パターン_裏"))).ToString
                    ' 表面状態
                    dgvResult(EM_RESULT_COL.nSCode, nRow).Value = CInt(.GetSqlInt32(.GetOrdinal("検査表面状態_表"))).ToString & "/" & CInt(.GetSqlInt32(.GetOrdinal("検査表面状態_裏"))).ToString
                    ' 管理No
                    dgvResult(EM_RESULT_COL.nKanriNo, nRow).Value = .GetSqlValue(.GetOrdinal("管理No"))
                    ' 運転状態
                    dgvResult(EM_RESULT_COL.nUntenStat, nRow).Value =
                        KizuForm.ucStatusAria.DspUnten(CType(.GetSqlInt32(.GetOrdinal("運転状態")), KizuForm.EM_DIV_UNTEN))
                    ' 格納区分
                    Dim strWk As String = "-"
                    Select Case .GetSqlInt32(.GetOrdinal("実績格納区分"))
                        Case 1
                            strWk = "通常切替"
                        Case 2
                            strWk = "ﾊﾞｯｸｱｯﾌﾟ"
                        Case 3
                            strWk = "手動停止"
                        Case 4
                            strWk = "強制停止"
                    End Select
                    dgvResult(EM_RESULT_COL.nKakuno, nRow).Value = strWk
                    ' 検査状態(表)
                    dgvResult(EM_RESULT_COL.nKensaStat_O, nRow).Value =
                        KizuForm.ucStatusAria.DspKensa(CType(.GetSqlInt32(.GetOrdinal("検査状態_表")), KizuForm.EM_DIV_KENSA))
                    ' 検査状態(裏)
                    dgvResult(EM_RESULT_COL.nKensaStat_U, nRow).Value =
                        KizuForm.ucStatusAria.DspKensa(CType(.GetSqlInt32(.GetOrdinal("検査状態_裏")), KizuForm.EM_DIV_KENSA))
                    ' 機器状態(表)
                    dgvResult(EM_RESULT_COL.nKikiStat_O, nRow).Value =
                        KizuForm.ucStatusAria.DspKiki(CType(.GetSqlInt32(.GetOrdinal("機器状態_表")), KizuForm.EM_DIV_KIKI))
                    ' 機器状態(裏)
                    dgvResult(EM_RESULT_COL.nKikiStat_U, nRow).Value =
                        KizuForm.ucStatusAria.DspKiki(CType(.GetSqlInt32(.GetOrdinal("機器状態_裏")), KizuForm.EM_DIV_KIKI))
                    ' 塗油スプレー装置状態
                    strWk = "-"
                    Select Case .GetSqlInt32(.GetOrdinal("塗油スプレー装置状態"))
                        Case 0
                            strWk = "未吹付"
                        Case 1
                            strWk = "吹付"
                    End Select
                    dgvResult(EM_RESULT_COL.nToyuSplr, nRow).Value = strWk
                    ' キー情報
                    dgvResult(EM_RESULT_COL.nKeyInfo, nRow).Value = .GetSqlValue(.GetOrdinal("キー情報")).ToString().Trim

                    ''カウント追加
                    nRow += 1

                End While
            End With

        Catch ex As Exception

        End Try
        If nRow <> 0 Then
            lblGuide.Text = String.Format("{0}件見つかりました", nRow)
        Else
            lblGuide.Text = "データが見つかりませんでした"
        End If

    End Sub

    ''' <summary>
    ''' 次のコイルを読み込ます
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadNextCoil()
        Try
            If ExistNext() = True Then
                m_nSelectRowNo += 1
                dgvResult(EM_RESULT_COL.nOrderNo, m_nSelectRowNo).Selected = True
                Visible = True
                ReadMap()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("次の実績データ読み込み異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 前のコイルを読み込ます
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPreviousCoil()
        Try
            If ExistPrevious() = True Then
                m_nSelectRowNo -= 1
                dgvResult(EM_RESULT_COL.nOrderNo, m_nSelectRowNo).Selected = True
                Visible = True
                ReadMap()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("前の実績データ読み込み異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 次に読み込むコイルがあるか取得します
    ''' </summary>
    ''' <returns>True:次のコイルがある False:ありません</returns>
    ''' <remarks></remarks>
    Public Function ExistNext() As Boolean
        If m_nSelectRowNo < (dgvResult.RowCount - 1) Then
            Return True
        Else
            Return False
        End If
    End Function

    ''' <summary>
    ''' 前に読み込むコイルがあるか取得します
    ''' </summary>
    ''' <returns>True:前のコイルがある False:ありません</returns>
    ''' <remarks></remarks>
    Public Function ExistPrevious() As Boolean
        If 0 < m_nSelectRowNo Then
            Return True
        Else
            Return False
        End If
    End Function

    ''' <summary>
    ''' 実績データ読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadMap()
        Try
            If dgvResult.ColumnCount = 0 Then
                Exit Sub    ' 検索結果が無い場合は処理しない
            End If

            Dim nRow As Integer = dgvResult.CurrentRow.Index

            '-------------------------------------------------------------------
            '読み込みパラメータの作成
            '-------------------------------------------------------------------
            Dim stReadParam As clsDataMap.READ_PARAM = Nothing
            With stReadParam
                .strKanriNo = dgvResult(EM_RESULT_COL.nKanriNo, nRow).Value.ToString()
                .bDS = chkHaba.Checked
                .bNagate = chkNagate.Checked
                .nTidList = New List(Of Integer)
                .nGidList = New List(Of Integer)
                .bTansho = radDspLikeTansho.Checked
                .bFFSnd = radDspLikeFFsnd.Checked
                .bCorrect = chkCorrect.Checked

                For row As Integer = 0 To dgvDispType.RowCount - 1              ' 表示対象外の疵種リストを作成
                    If CBool(dgvDispType(EM_FILTER_COL.emCheck, row).Value) = False Then
                        .nTidList.Add(CInt(dgvDispType(EM_FILTER_COL.emEdasysID, row).Value))
                    End If
                Next

                For row As Integer = 0 To dgvDispGrade.RowCount - 1             ' 表示対象外のグレードリストを作成
                    If CBool(dgvDispGrade(EM_FILTER_COL.emCheck, row).Value) = False Then
                        .nGidList.Add(CInt(dgvDispGrade(EM_FILTER_COL.emEdasysID, row).Value))
                    End If
                Next
            End With

            '-------------------------------------------------------------------
            ' 実績データ読み込み
            '-------------------------------------------------------------------
            mcls_ReadDatamap = New clsDataMap(mcls_Param, mcls_PreParam, mcls_Log)  ' メモリ開放は実績画面でお願いします。
            mcls_ReadDatamap.Interval(EM_LONG_POS.SMALL) = mcls_Param.MapStageSmall
            mcls_ReadDatamap.Interval(EM_LONG_POS.BIG) = mcls_Param.MapStageBig
            AddHandler mcls_ReadDatamap.StartDataNti, AddressOf clsDataMap_StartData
            AddHandler mcls_ReadDatamap.ReadDataNti, AddressOf clsDataMap_ReadData

            SetEnableControl(False)
            Dim bRet As Boolean = mcls_ReadDatamap.Read(stReadParam)
            SetEnableControl(True)

            If bRet = False Then
                If mcls_ReadDatamap.IsReadCancel = False Then                   ' ユーザーがキャンセルしていない
                    lblGuide.Text = "実績データの読み込みに失敗しました"
                End If
            Else
                mcls_DataMap = mcls_ReadDatamap                                 ' 今回の検索結果を設定する
                m_nRetCode = SEARCH_RETURN.emSELECT_COIL
                m_nSelectRowNo = dgvResult.CurrentRow.Index
                Me.Hide()                                                       ' そして消える
            End If

            '-------------------------------------------------------------------
            ' 後処理
            '-------------------------------------------------------------------
            stReadParam.nGidList.Clear()
            stReadParam.nGidList = Nothing
            stReadParam.nTidList.Clear()
            stReadParam.nTidList = Nothing

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績データ読み込み異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' コントロール操作可否設定
    ''' </summary>
    ''' <param name="bVisible">True:操作可 False:操作不可</param>
    ''' <remarks></remarks>
    Private Sub SetEnableControl(ByVal bVisible As Boolean)
        pnlMsg.Visible = Not bVisible                                           ' メッセージ表示するため有効にする
        btnEnd.Enabled = bVisible
        btnSearch.Enabled = bVisible
        btnSelect.Enabled = bVisible
        dgvResult.Enabled = bVisible
    End Sub

    '***************************************************************************
    ' 実績読み込みイベント
    '***************************************************************************
    ''' <summary>
    ''' マップデータ読み込み開始イベント
    ''' </summary>
    ''' <param name="nKind">データ種別</param>
    ''' <param name="nMax">読み込み件数</param>
    ''' <remarks></remarks>
    Private Sub clsDataMap_StartData(ByVal nKind As clsDataMap.DATA_KIND, ByVal nMax As Integer)
        Try
            Select Case nKind
                Case clsDataMap.DATA_KIND.emCOIL_POS
                    txtMsg.Text = "コイル位置情報取得中"
                Case clsDataMap.DATA_KIND.emCOIL_INF
                    txtMsg.Text = "疵情報取得中"
                Case clsDataMap.DATA_KIND.emCUT_INF
                    txtMsg.Text = "カット情報取得中"
            End Select

            prbMsg.Minimum = 0
            prbMsg.Maximum = CInt(IIf(nMax > prbMsg.Minimum, nMax, prbMsg.Minimum + 1))
            prbMsg.Value = 0
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績読み込み開始イベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' マップデータ読み込みイベント
    ''' </summary>
    ''' <param name="nKind">データ種別</param>
    ''' <param name="nCount">読み込み件数</param>
    ''' <remarks></remarks>
    Private Sub clsDataMap_ReadData(ByVal nKind As clsDataMap.DATA_KIND, ByVal nCount As Integer)
        Try
            prbMsg.Value = CInt(IIf(nCount < prbMsg.Maximum, nCount, prbMsg.Maximum))
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績読み込みイベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' フォームイベント
    '***************************************************************************
    ''' <summary>
    ''' 資源破棄
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSearch_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面資源解放異常 [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frmSearch_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 5 - 1                                                      ' コイル検索
                    If btnSearch.Enabled Then
                        btnSearch_Click(Nothing, Nothing)
                    End If
                    '-----------------------------------------------------------
                Case 6 - 1                                                      ' 選択
                    btnSelect_Click(Nothing, Nothing)
                    '-----------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面ファンクションイベント異常 [{0}]", ex.Message))
        End Try
    End Sub
    '***************************************************************************
    ' ボタンイベント
    '***************************************************************************
    ''' <summary>
    ''' 検索ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            If txtCoil.Text.Trim = "" And txtKizukenNo.Text.Trim = "" And txtKanriNo.Text.Trim = "" And
             Not chkDateSet.Checked And Not chkTimeSet.Checked Then             '検査条件が指定されていなければ、エラー
                lblGuide.Text = "検索キーを入力してください"
                Exit Sub
            End If

            Me.Cursor = Cursors.WaitCursor
            Dim strSQL As String = GetSQLSearchCoil()
            If strSQL.Length > 0 Then
                SearchCoil(strSQL)
            End If
            Me.Cursor = Cursors.Arrow
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面検索ボタン押下イベント異常 [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' キャンセルボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCancel_MouseClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles btnCancel.MouseClick
        Try
            If mcls_ReadDatamap Is Nothing = False Then
                mcls_ReadDatamap.CancelRead()                                   ' 読み込みキャンセル
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面キャンセル押下イベント異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            m_nRetCode = SEARCH_RETURN.emCANCEL
            Me.Hide()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面閉じるボタン押下イベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 選択ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            ReadMap()                                                           ' 実績読み込み
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面選択ボタン押下イベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' CSVボタン押下処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsv.Click
        Try
            Call CsvOut(CSV_PATH, "検査実績検索画面.csv", True)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面CSV出力ボタン押下イベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' マウスイベント
    '***************************************************************************
    ''' <summary>
    ''' マウスイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellDoubleClick
        Try
            If e.RowIndex < 0 Then                                              ' 選択行が不正なら終わる
                Exit Sub
            End If

            ReadMap()                                                           ' 実績検索
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("実績検索画面検索結果ダブルクリックイベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' その他 特殊処理
    '***************************************************************************
    ''' <summary>
    ''' 特殊フィルター選択なしチェック変更処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkNormalDsp_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles radNormalDsp.CheckedChanged

        ChangeKizuGradeFilter(radNormalDsp.Checked)

    End Sub


    '***************************************************************************
    ' その他 内部関数
    '***************************************************************************
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
    Private Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer,
     ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode,
     ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
     Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
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

    Private Sub dgbColSetButton(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal sColumn As String, ByVal nWidth As Integer,
ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
Optional ByVal bDispVis As Boolean = True)
        Using clm As New DataGridViewButtonColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 並べ替えモード
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle

            clm.DefaultCellStyle.SelectionBackColor = SystemColors.Control
            clm.Visible = bDispVis
            'ボタン名称
            clm.UseColumnTextForButtonValue = True
            clm.Text = sColumn

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ その他 もろもろ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 設定値保存
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SaveSetting()
        ' 保存
        Call MyBase.SaveFilterData(dgvDispType, DB_TYPE_NAME)
        Call MyBase.SaveFilterData(dgvDispGrade, DB_GRADE_NAME)
    End Sub




    ''' <summary>
    ''' プロコンコイル情報受信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <remarks></remarks>
    Private Function RecvCheckProCoil(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        'キー情報指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' プロコンカット実績受信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function RecvCheckProCut(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        'キー情報指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_CUT
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' プロコン採寸実績受信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function RecvCheckProSaisun(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        'キー情報指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' プロコン検査実績送信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SendCheckProZiseki(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '管理No指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' FF前工程疵情報受信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <remarks></remarks>
    Private Function RecvCheckFFRecv(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        'キー情報指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' FF検査実績送信データ数取得
    ''' </summary>
    ''' <param name="strKeyInfo">キー情報</param>
    ''' <param name="nDataCnt">データ数</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SendCheckFFSend(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '管理No指定表示
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE キー情報 = '" & strKeyInfo & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' データ取得
            If sqlRead.Read() Then
                'データカウント数セット
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' 検索結果内のボタンが押されたときの処理（伝文参照画面起動）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub
        Dim strKeyInfo As String = dgv(EM_RESULT_COL.nKeyInfo, e.RowIndex).Value.ToString.Trim
        Dim strKanriNo As String = dgv(EM_RESULT_COL.nKanriNo, e.RowIndex).Value.ToString.Trim
        Dim strID As String = ""
        Dim nDataCnt As Integer = 0

        If 0 = dgvResult.RowCount Then Exit Sub
        lblGuide.Text = ""

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex

            Case EM_RESULT_COL.nPCCoil              'P/Cｺｲﾙ
                Dim frm As frmCoilInfo
                If RecvCheckProCoil(strKeyInfo, nDataCnt) Then   'プロコンコイル情報受信データチェック
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmCoilInfo(1, strKanriNo)  'プロコンコイル情報受信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "コイル情報(P/C)受信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "コイル情報(P/C)受信データの読み込みに失敗しました"
                End If

            Case EM_RESULT_COL.nPCCut               'P/Cｶｯﾄ
                Dim frm As frmCut
                If RecvCheckProCut(strKeyInfo, nDataCnt) Then  'プロコンカット実績受信データチェック
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmCut(1, strKanriNo)  'プロコンカット実績受信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "カット実績(P/C)受信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "カット実績(P/C)受信データの読み込みに失敗しました"
                End If

            Case EM_RESULT_COL.nPCSaisun            'P/C採寸
                Dim frm As frmSaisun
                If RecvCheckProSaisun(strKeyInfo, nDataCnt) Then  'プロコン採寸実績受信データチェック
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmSaisun(1, strKanriNo)  'プロコン採寸実績受信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "採寸実績(P/C)受信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "採寸実績(P/C)受信データの読み込みに失敗しました"
                End If

            Case EM_RESULT_COL.nPCZiseki            'P/C実績
                Dim frm As frmMixedRateTool

                If SendCheckProZiseki(strKeyInfo, nDataCnt) Then        'プロコン検査実績送信データチェック
                    If nDataCnt > 0 Then                        'データ有
                        lblGuide.Text = ""
                        frm = New frmMixedRateTool(1, strKanriNo)  'プロコン検査実績送信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "検査実績(P/C)送信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "検査実績(P/C)送信データの読み込みに失敗しました"
                End If

            Case EM_RESULT_COL.nFFRecv            'FF前工程疵情報受信
                Dim frm As frmFFRecv
                If RecvCheckFFRecv(strKeyInfo, nDataCnt) Then  'FF前工程疵情報受信データチェック
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmFFRecv(1, strKanriNo)  'FF前工程疵情報受信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "FF前工程疵情報(FF)受信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "FF前工程疵情報(FF)受信データの読み込みに失敗しました"
                End If

            Case EM_RESULT_COL.nFFSend            'FF検査実績送信
                Dim frm As frmFF

                If SendCheckFFSend(strKeyInfo, nDataCnt) Then        'FF検査実績送信データチェック
                    If nDataCnt > 0 Then                        'データ有
                        lblGuide.Text = ""
                        frm = New frmFF(1, strKanriNo)  'FF検査実績送信(=1：管理No渡し)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "FF検査実績(FF)送信データが見つかりませんでした"
                    End If
                Else
                    lblGuide.Text = "FF検査実績(FF)送信データの読み込みに失敗しました"
                End If


        End Select
    End Sub

    ''' <summary>
    ''' 疵種、グレード表示フィルターコントロール変更
    ''' </summary>
    ''' <param name="bEnable">True:有効、False:無効</param>
    Private Sub ChangeKizuGradeFilter(ByVal bEnable As Boolean)

        Dim ii As Integer

        If bEnable Then
            '緑色表示（疵種）
            For ii = 0 To dgvDispType.RowCount - 1
                If CBool(dgvDispType(0, ii).Value) Then
                    dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.LightGreen
                    dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGreen
                Else
                    dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
                    dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If
            Next ii

            '緑色表示（疵種）
            For ii = 0 To dgvDispGrade.RowCount - 1
                If CBool(dgvDispGrade(0, ii).Value) Then
                    dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.LightGreen
                    dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGreen
                Else
                    dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
                    dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If
            Next ii
        Else
            '灰色表示（疵種）
            For ii = 0 To dgvDispType.RowCount - 1
                dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.LightGray
                dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGray
            Next ii

            '灰色表示（疵種）
            For ii = 0 To dgvDispGrade.RowCount - 1
                dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.LightGray
                dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGray
            Next ii
        End If

        ''疵種、Grリスト
        dgvDispType.Enabled = bEnable
        dgvDispGrade.Enabled = bEnable
        ''疵種、Gr全選択／解除ボタン
        btnTypeAllSelect.Enabled = bEnable
        btnTypeAllCancel.Enabled = bEnable
        btnGradeAllSelect.Enabled = bEnable
        btnGradeAllCancel.Enabled = bEnable

    End Sub

End Class
