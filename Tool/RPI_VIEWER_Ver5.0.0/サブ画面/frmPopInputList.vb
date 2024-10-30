Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Text.RegularExpressions
Imports System.IO

Public Class frmPopInputList

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体

    ' 保持情報
    Public Structure INFO_INPUT
        ' ファイル全体での矩形
        Dim pos1 As Point				' 1点目 [mm] ※画像左上から		※ファイル保存用
        Dim pos2 As Point				' 2点目 [mm]

        Dim pixelAll1 As Point			' 1点目 [RP] ※画像左上から		※オーバーレイ用
        Dim pixelAll2 As Point			' 1点目 [RP]

        Dim nFrameNo1 As Integer
        Dim nFrameNo2 As Integer

        ' ファイル全体での情報 表示中のままの情報で (中心位置)
        Dim nPosY As Integer			' 長手位置 [mm] ※画像左上から	※ジャンプ用
        Dim nPosX As Integer			' 幅位置 [mm]	※画像左上から

        'Dim nPixelAllX As Integer		' 全フレーム左上からの距離 [RP] [pixel] ※画像左上から
        'Dim nPixelAllY As Integer		' 全フレーム左上からの距離 [RP] [pixel] ※画像左上から


        '''' 該当フレーム内での情報	
        ' 中心
        Dim nFrameNo As Integer			' 指定した点のフレームNoをセット
        Dim nPixelX As Integer			' フレーム左上からの距離 [RP] [pixel]
        Dim nPixelY As Integer			' フレーム左上からの距離 [RP] [pixel]


        '''' 画面表示用
        Dim wid As Integer				' 幅 [mm]
        Dim len As Integer				' 長さ [mm]

        ' エッジからの距離
        Dim bEdge As Boolean			' 有効無効
        Dim nEdgeL As Integer			' エッジからの距離 [mm]
        Dim nEdgeR As Integer

        Dim strUserName As String		' マーキングユーザ名
    End Structure


    ' 表示項目
    Private Enum EM_LIST
        em_No = 0		' 入力No (1～) ボタン
        em_PosY			' 先端からの位置 [mm]
        em_PosX			' 左端からの位置 [mm]

        em_EdgeL		' 左エッジからの距離 [mm]
        em_EdgeR		' 右エッジからの距離 [mm]

        em_Wid			' 幅 [mm]
        em_Len			' 長さ [mm]

        em_UserName		' 作成者
        em_Delete		' 削除ボタン

        em_End
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    ''' <param name="mode">Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)</param>
    ''' <param name="len">[mm]</param>
    Public Event tEv_JumpNagate(ByVal mode As Integer, ByVal len As Single)

    ''' <param name="mode">Xモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)</param> 
    ''' <param name="len">[mm]</param>
    ''' <param name="lenMode">lenがどっちの指定から？(0:左端  1:センターから)</param>
    Public Event tEv_JumpHaba(ByVal mode As Integer, ByVal len As Integer, ByVal lenMode As Integer)

    Public Event tEv_RePaint()															' 再描画

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)
    Private mcls_pCalc As tClass_ImageDsp_Calc		' 画像計算クラス (ポインタ)
    Private m_BasePic As PictureBox					' ベースピクチャーボックス

    ' メイン
    Private m_Info As New List(Of INFO_INPUT)		' 登録情報

    ' CSV検索用
    Private m_sSearchKey As String                      ' 対象ファイル名 (管理No@面@*)
    Private m_sSaveFormat As String                     ' 保存ファイル名のフォーマット（管理No@面@コイルNO@{0}.csv)

    ' もろもろ
    Private m_bIsLoad As Boolean = False				' ロード済みでtrue

    ' テスト
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' 疵情報リスト

    Private m_strUserName As String						' マーキングユーザ名

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub
    ''' <summary>
    ''' 計算クラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_Calc(ByRef cls As tClass_ImageDsp_Calc)
        mcls_pCalc = cls
    End Sub

    ''' <summary>
    ''' メインピクチャー
    ''' </summary>
    Public Sub SetCnt_Pic(ByRef pic As PictureBox)
        m_BasePic = pic
    End Sub

    ' テスト
    Friend Sub InitData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmInputList_Load(sender As Object, e As System.EventArgs) Handles Me.Load
        m_bIsLoad = True

        '' ===>> 永崎 マーキングユーザ対応 20170516
        ' このRPIViewerでの「デフォルト作成者」をINIファイルから取得
        m_strUserName = GetUserName()
        ' 禁則文字チェック
        If IsUserNameMatch(m_strUserName) Then
            m_strUserName = String.Empty
            ' INIファイル更新
            UpdateUserName(m_strUserName)
        End If
        txtUserName.Text = m_strUserName


        Call DgvInitDefect(dgvList)
        Call DspGridData()			' 初回描画
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ dgv
#Region "デザイン"


    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInitDefect(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv
            .ColumnHeadersHeight = 30
            .ScrollBars = ScrollBars.Both								   ' スクロールバーの表示
            .AllowUserToResizeColumns = True								' ユーザー 列サイズ変更


            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().WrapMode = DataGridViewTriState.True


            '' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "No"
                clm.Width = 48
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "先端からの位置[m]"
                clm.Width = 74
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "左ｴｯｼﾞからの位置[mm]"
                clm.Width = 74
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "右ｴｯｼﾞからの位置[mm]"
                clm.Width = 74
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "右ｴｯｼﾞからの位置[mm]"
                clm.Width = 74
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "幅 [mm]"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "長さ [mm]"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "作成"
                clm.Width = 50
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using
            '          
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "削除"
                clm.Width = 48
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns.Add(clm)
            End Using

            '''' 固定行設定
            .RowCount = 0

        End With
    End Sub
#End Region



    ''' <summary>
    ''' データグリッドビューに描画
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspGridData()
        If Not m_bIsLoad Then Return


        dgvList.RowCount = m_Info.Count

        For ii As Integer = 0 To m_Info.Count - 1
            dgvList(EM_LIST.em_No, ii).Value = ii + 1
            dgvList(EM_LIST.em_PosY, ii).Value = String.Format("{0:0.000}", m_Info(ii).nPosY / 1000.0)
            dgvList(EM_LIST.em_PosX, ii).Value = m_Info(ii).nPosX


            If m_Info(ii).bEdge Then
                dgvList(EM_LIST.em_EdgeL, ii).Value = m_Info(ii).nEdgeL
                dgvList(EM_LIST.em_EdgeR, ii).Value = m_Info(ii).nEdgeR
            Else
                dgvList(EM_LIST.em_EdgeL, ii).Value = ""
                dgvList(EM_LIST.em_EdgeR, ii).Value = ""
            End If

            dgvList(EM_LIST.em_Wid, ii).Value = m_Info(ii).wid
            dgvList(EM_LIST.em_Len, ii).Value = m_Info(ii).len

            dgvList(EM_LIST.em_UserName, ii).Value = m_Info(ii).strUserName
            dgvList(EM_LIST.em_Delete, ii).Value = "削除"
        Next
    End Sub


    ''' <summary>
    ''' データグリッドクリック
    ''' </summary>
    Private Sub dgvDefect_CellMouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles dgvList.CellMouseDown
        Dim dgv As DataGridView = CType(sender, DataGridView)
        Dim row As Integer = e.RowIndex

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_LIST.em_No		' No
                Call Jump(row)

            Case EM_LIST.em_Delete	' 削除
                m_Info.RemoveAt(row)	' 要素削除
                Call DspGridData()
                RaiseEvent tEv_RePaint()
            Case Else

        End Select
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 描画

    ''' <summary>
    ''' 指定地点描画
    ''' </summary>
    ''' <param name="g">グラフィックス</param>
    ''' <remarks></remarks>
    Public Sub PaintPos(ByVal g As Graphics, ByVal nNowStartFNo As Integer, ByVal nDspFrameRowCnt As Integer)
        Dim nHosei As Integer = 3
        Dim pos1 As Point		' 表示座標位置
        Dim pos2 As Point
        Dim inf As INFO_INPUT

        ' 表示非表示
        If chkDspDisable.Checked Then Return


        Dim pen As Pen = New System.Drawing.Pen(Color.Red, 2)


        '' 始点位置
        For ii As Integer = 0 To m_Info.Count - 1
            inf = m_Info(ii)

            ' 表示しているフレーム？
            If (inf.nFrameNo1 >= nNowStartFNo And inf.nFrameNo1 <= nNowStartFNo + nDspFrameRowCnt - 1) Or _
             (inf.nFrameNo2 >= nNowStartFNo And inf.nFrameNo2 <= nNowStartFNo + nDspFrameRowCnt - 1) Then

                Dim y1 As Integer = inf.pixelAll1.Y Mod mcls_pCalc.tProp_ImgInf.nOneImgH
                y1 += (inf.nFrameNo1 - nNowStartFNo) * mcls_pCalc.tProp_ImgInf.nOneImgH
                pos1 = mcls_pCalc.GetPosition(inf.pixelAll1.X, y1)


                Dim y2 As Integer = inf.pixelAll2.Y Mod mcls_pCalc.tProp_ImgInf.nOneImgH
                y2 += (inf.nFrameNo2 - nNowStartFNo) * mcls_pCalc.tProp_ImgInf.nOneImgH
                pos2 = mcls_pCalc.GetPosition(inf.pixelAll2.X, y2)



                ' 外接長方形
                Dim rect As Rectangle
                rect.Y = CInt(IIf(pos1.Y < pos2.Y, pos1.Y, pos2.Y))
                rect.Height = Math.Abs(pos2.Y - pos1.Y)

                rect.X = CInt(IIf(pos1.X < pos2.X, pos1.X, pos2.X))
                rect.Width = Math.Abs(pos2.X - pos1.X)

                ' 1本目
                pen.Width = 2
                g.DrawRectangle(pen, rect)

                ' 2本目
                rect.Y += nHosei
                rect.Height -= nHosei * 2 + 1
                rect.X += nHosei
                rect.Width -= nHosei * 2 + 1
                pen.Width = 1
                g.DrawRectangle(pen, rect)
            End If

        Next ii

        '' 後始末
        pen.Dispose()
    End Sub

    ''' <summary>
    ''' 表示表示設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkDspDisable_CheckedChanged(sender As System.Object, e As System.EventArgs) Handles chkDspDisable.CheckedChanged
        RaiseEvent tEv_RePaint()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' 追加
    ''' </summary>
    ''' <param name="pos1">入力矩形 表示中画像左上 [DP]</param>
    Public Sub InputListAdd(ByVal pos1 As Point, ByVal pos2 As Point)

        '''' 見えていない部分も加算した RP に変換
        Dim pixAll1 As Point = mcls_pRpiView.GetCls_ImgDsp.GetDspPixelToImgPosition(pos1)
        pixAll1.Y += mcls_pRpiView.GetDspAriaInf.nDspFno * mcls_pRpiView.GetImg_ImgInf.nOneImgH	' 見えていない分を加算

        Dim pixAll2 As Point = mcls_pRpiView.GetCls_ImgDsp.GetDspPixelToImgPosition(pos2)
        pixAll2.Y += mcls_pRpiView.GetDspAriaInf.nDspFno * mcls_pRpiView.GetImg_ImgInf.nOneImgH	' 見えていない分を加算

        '''' 登録
        Call AddList(pixAll1, pixAll2, m_strUserName)
    End Sub

    ''' <summary>
    ''' メンバーに追加
    ''' </summary>
    ''' <param name="pixAll1">見えない部分も考慮[RP]</param>
    ''' <param name="pixAll2">見えない部分も考慮[RP]</param>
    ''' <remarks></remarks>
    Private Sub AddList(ByVal pixAll1 As Point, ByVal pixAll2 As Point, ByVal sUserName As String)
        Dim inf As INFO_INPUT
        inf.pixelAll1 = pixAll1
        inf.pixelAll2 = pixAll2


        inf.pos1.X = CInt(mcls_pRpiView.GetPos_RpToLen_X(inf.pixelAll1.X))
        inf.pos1.Y = CInt(mcls_pRpiView.GetPos_RpToLen_mm(inf.pixelAll1.Y))
        inf.pos2.X = CInt(mcls_pRpiView.GetPos_RpToLen_X(inf.pixelAll2.X))
        inf.pos2.Y = CInt(mcls_pRpiView.GetPos_RpToLen_mm(inf.pixelAll2.Y))

        inf.nFrameNo1 = inf.pixelAll1.Y \ mcls_pCalc.tProp_ImgInf.nOneImgH
        inf.nFrameNo2 = inf.pixelAll2.Y \ mcls_pCalc.tProp_ImgInf.nOneImgH


        ' 中心位置 関係
        inf.nPosX = CInt((inf.pos1.X + inf.pos2.X) / 2)
        inf.nPosY = CInt((inf.pos1.Y + inf.pos2.Y) / 2)

        Dim nowRP As Point
        nowRP.X = CInt((inf.pixelAll1.X + inf.pixelAll2.X) / 2)
        nowRP.Y = CInt((inf.pixelAll1.Y + inf.pixelAll2.Y) / 2)
        inf.nFrameNo = nowRP.Y \ mcls_pCalc.tProp_ImgInf.nOneImgH
        inf.nPixelY = nowRP.Y Mod mcls_pCalc.tProp_ImgInf.nOneImgH
        inf.nPixelX = nowRP.X


        ' 距離
        inf.wid = Math.Abs(inf.pos1.X - inf.pos2.X)
        inf.len = Math.Abs(inf.pos1.Y - inf.pos2.Y)

        inf.strUserName = sUserName
        '-----------------------------------
        ' エッジからの距離
        If mcls_pRpiView.GetRpi_Header().IsTHeader AndAlso 1 = mcls_pRpiView.GetRpi_THeader().frame_info_type Then
            Try
                Dim nWk As Integer
                nWk = inf.nFrameNo + 1		 ' 0オリジンから1オリジンに
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1 = mcls_pRpiView.GetRpi_Cls().GetRpiFrameAdd1(nWk)


                ' 左エッジ
                If Not mcls_pRpiView.GetProp_DSWS() Then
                    nWk = nowRP.X - typFinfo.nEdgeIdx(0)
                Else
                    nWk = typFinfo.nEdgeIdx(1) - nowRP.X
                End If
                nWk = CInt(mcls_pCalc.GetPos_RpToLen_X(nWk))
                inf.nEdgeL = nWk
                ' 右エッジ
                If Not mcls_pRpiView.GetProp_DSWS() Then
                    nWk = typFinfo.nEdgeIdx(1) - nowRP.X
                Else
                    nWk = nowRP.X - typFinfo.nEdgeIdx(0)
                End If
                nWk = CInt(mcls_pCalc.GetPos_RpToLen_X(nWk))
                inf.nEdgeR = nWk


                inf.bEdge = True

            Catch ex As Exception
                inf.bEdge = False
            End Try
        Else
            inf.bEdge = False
        End If


        '''' 登録
        m_Info.Add(inf)

        RaiseEvent tEv_RePaint()
    End Sub

    ''' <summary>
    ''' 選択位置にジャンプ
    ''' </summary>
    ''' <param name="idx">行</param>
    ''' <remarks></remarks>
    Public Sub Jump(ByVal idx As Integer)
        Dim inf As INFO_INPUT = m_Info(idx)



        Dim y As Integer = inf.nPosY
        If mcls_pRpiView.GetProp_TB() Then
            y = CInt(mcls_pRpiView.GetPos_RpToLen_mm((mcls_pRpiView.GetRpi_Header.nframe * mcls_pRpiView.GetRpi_Header.height))) - y
        End If

        Dim x As Integer = inf.nPosX
        If mcls_pRpiView.GetProp_DSWS() Then
            x = CInt(mcls_pRpiView.GetPos_RpToLen_X((mcls_pRpiView.GetRpi_Header.width))) - x
        End If

        RaiseEvent tEv_JumpNagate(2, y)
        RaiseEvent tEv_JumpHaba(2, x, 0)




        'RaiseEvent tEv_JumpNagate(2, CInt(inf.nPosY))
        'RaiseEvent tEv_JumpHaba(2, CInt(inf.nPosX), 0)
    End Sub

    ''' <summary>
    ''' 別コイル
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub NewCoil(ByVal sKanriNo As String, ByVal men As Integer, ByVal sCoilNo As String)
        '管理NO＋面に一致するファイルを検索する
        m_sSearchKey = String.Format("{0}@{1}*.csv", sKanriNo, men)
        m_sSaveFormat = String.Format("{0}@{1}@{2}@{3}.csv", sKanriNo, men, sCoilNo, "{0}")
        '管理Noが無効な場合、保存ボタンは非活性とする
        btnSave.Enabled = Not String.IsNullOrEmpty(sKanriNo)

        '''' 初期化
        Call m_Info.Clear()

        '''' ファイルあれば読込
        Call ReadCSV()
        Call DspGridData()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ CSV
    ''' <summary>
    ''' 保存
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSave_Click(sender As System.Object, e As System.EventArgs) Handles btnSave.Click
        If String.IsNullOrEmpty(m_sSearchKey) Then Return
        If Not m_bIsLoad Then Return

        ' 確認
        If vbNo = MsgBox("保存しますか？", MsgBoxStyle.YesNo) Then Return


        Dim sPath As String = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)
        ' フォルダ作成
        IO.Directory.CreateDirectory(sPath)

        'ユーザリスト生成
        Dim lstUsers As New List(Of String)
        For Each inf As INFO_INPUT In m_Info
            If Not lstUsers.Contains(inf.strUserName) Then
                lstUsers.Add(inf.strUserName)
            End If
        Next

        'ユーザ毎に書き込む
        Dim lstErrorCSV As New List(Of String)
        Dim lstOutputCSV As New List(Of String)
        For Each strUserName As String In lstUsers

            '出力ファイルパス生成
            '※ユーザ無しの場合も新ファイル名で書き込む
            Dim sCSVFile As String = IO.Path.Combine(sPath, String.Format(m_sSaveFormat, strUserName))

            Try
                ' 書込み
                Using os As StreamWriter = New IO.StreamWriter(sCSVFile, False)
                    For Each inf As INFO_INPUT In m_Info
                        'ユーザ名が一致するデータのみ書込み
                        If inf.strUserName = strUserName Then
                            Dim sWk As String = String.Format("{0},{1},{2},{3},{4}", 0, inf.pixelAll1.X, inf.pixelAll1.Y, inf.pixelAll2.X, inf.pixelAll2.Y)
                            os.WriteLine(sWk)
                        End If
                    Next
                End Using

                '出力したCSVファイルを登録
                lstOutputCSV.Add(sCSVFile)

            Catch ex As Exception
                'エラーメッセージ用にファイルパスを格納
                lstErrorCSV.Add(sCSVFile)
            End Try
        Next

        'お掃除処理
        '※メモリ上から消しても、ファイル削除しないと次回起動で読込まれてしまう
        RemoveUnnecessaryCSV(lstOutputCSV)

        If lstErrorCSV.Count > 0 Then
            MsgBox("位置連携CSV保存失敗" + vbCrLf + String.Join(vbCrLf, lstErrorCSV.ToArray()))
        Else
            MsgBox("位置連携CSV保存完了" & vbCrLf & sPath)
        End If


    End Sub

    ''' <summary>
    ''' CSV読込
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadCSV()
        If String.IsNullOrEmpty(m_sSearchKey) Then Return

        '位置情報クリア
        Call m_Info.Clear()

        Dim sWk As String
        Dim sPath As String = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)

        '' ===>> 永崎 マーキングユーザ対応 20170517
        '対象CSVファイルパスを取得
        Dim lstCSVFile As List(Of String) = GetTargetCSV()

        'リストのファイルを全て読込む
        Dim sErrorCSVFiles As String = String.Empty
        For Each sCSVFile As String In lstCSVFile

            Try
                ' ユーザ名の抽出
                Dim sCSVFileName As String = Path.GetFileNameWithoutExtension(sCSVFile)
                Dim lstCSVFileName As String() = sCSVFileName.Split("@"c)
                Dim sUserName As String = String.Empty
                If 4 = lstCSVFileName.Length Then
                    sUserName = lstCSVFileName(lstCSVFileName.Length - 1)
                End If

                ' 読込
                Using os As StreamReader = New IO.StreamReader(sCSVFile)

                    Do
                        sWk = os.ReadLine()

                        If sWk Is Nothing Then
                            Exit Do
                        End If

                        Dim sAry() As String = sWk.Split(","c)
                        If 5 <> sAry.Length Then Continue Do

                        Dim mode As Integer = CInt(sAry(0))		 ' 将来用
                        Dim pixAll1 As Point = New Point(CInt(sAry(1)), CInt(sAry(2)))
                        Dim pixAll2 As Point = New Point(CInt(sAry(3)), CInt(sAry(4)))

                        ' 登録
                        Call AddList(pixAll1, pixAll2, sUserName)

                    Loop While sWk IsNot Nothing

                End Using
            Catch ex As Exception
                'エラーメッセージ用にファイルパスを格納
                sErrorCSVFiles += vbCrLf + sCSVFile
                Continue For
            End Try
        Next

        If Not String.IsNullOrEmpty(sErrorCSVFiles) Then
            MsgBox("位置連携CSV読み込み中に異常発生" + vbCrLf + sErrorCSVFiles)
        End If
    End Sub

    ''' <summary>
    ''' 再読込
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnReRead_Click(sender As System.Object, e As System.EventArgs) Handles btnReRead.Click
        Call ReadCSV()
        Call DspGridData()
        RaiseEvent tEv_RePaint()
    End Sub

    ''' <summary>
    ''' フォルダ表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnOpenFolder_Click(sender As System.Object, e As System.EventArgs) Handles btnOpenFolder.Click
        Dim sPath As String = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)

        System.Diagnostics.Process.Start(sPath)
    End Sub









    ''' <summary>
    ''' 疵全登録
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAddAll_Click(sender As System.Object, e As System.EventArgs) Handles btnAddAll.Click

        ' 疵全登録
        For Each inf As DATA_DETECT_INF In m_lstDetect

            Dim p1 As Point = New Point( _
    inf.nFrameXmin + mcls_pRpiView.GetRpi_THeader.caminfo(inf.nCamPos).nBaseLeft, _
    inf.nFrameYmin + (inf.nFrameNo - 1) * mcls_pRpiView.GetRpi_Header.height)

            Dim p2 As Point = New Point( _
   inf.nFrameXmax + mcls_pRpiView.GetRpi_THeader.caminfo(inf.nCamPos).nBaseLeft, _
   inf.nFrameYmax + (inf.nFrameNo - 1) * mcls_pRpiView.GetRpi_Header.height)

            Call AddList(p1, p2, m_strUserName)
        Next

        ' 反映
        Call DspGridData()
        RaiseEvent tEv_RePaint()
    End Sub

    '' ===>> 永崎 マーキングユーザ対応 20170516
    ''' <summary>
    ''' マーキングユーザ名取得
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetUserName() As String

        'INIファイルから読込
        Return tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "MARKING_USER", LOCALINI_NAME)

    End Function

    ''' <summary>
    ''' マーキングユーザ名設定
    ''' </summary>
    ''' <param name="sUserName"></param>
    ''' <remarks></remarks>
    Private Sub UpdateUserName(ByVal sUserName As String)

        'INIファイルに書込み
        tMod.tMod_IniFile.WriteIni("RPI_VIEWER", "MARKING_USER", LOCALINI_NAME, m_strUserName)

    End Sub

    ''' <summary>
    ''' マーキングユーザ入力完了（フォーカス抜け）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtUserName_Leave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtUserName.Leave

        ' マーキングユーザー名登録
        SetUserName()

    End Sub

    ''' <summary>
    ''' マーキングユーザ入力完了（Enter押下）
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtUserName_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtUserName.KeyDown

        If System.Windows.Forms.Keys.Return = e.KeyCode Then
            ' マーキングユーザー名登録
            SetUserName()
        End If

    End Sub

    ''' <summary>
    ''' マーキングユーザー名登録
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetUserName()

        ' 禁則文字チェック
        If IsUserNameMatch(txtUserName.Text) Then
            MsgBox("禁則文字が含まれています。" & vbCrLf & _
              "\ / : * ? "" < > | を除き、再度入力してください。")
            txtUserName.Focus()
            txtUserName.SelectAll()
            Return
        End If

        If 64 < txtUserName.Text.Length Then
            MsgBox("文字数の上限を超えています。" & vbCrLf & _
             "64文字以下で入力してください。")
            txtUserName.Focus()
            txtUserName.SelectAll()
            Return
        End If

        ' 位置情報登録用変数にセット
        m_strUserName = txtUserName.Text
        ' INIファイル更新
        UpdateUserName(m_strUserName)

    End Sub

    ''' <summary>
    ''' 禁則文字チェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function IsUserNameMatch(ByVal text As String) As Boolean

        Return Regex.IsMatch(text, "[\\/:*?""<>|]")

    End Function

    ''' <summary>
    ''' 不要CSV削除
    ''' </summary>
    ''' <param name="lstOutputCSV"></param>
    ''' <remarks></remarks>
    Private Sub RemoveUnnecessaryCSV(ByVal lstOutputCSV As List(Of String))

        '対象CSVファイルパスを取得
        Dim lstCSVFile As List(Of String) = GetTargetCSV()

        For Each sCSVFile As String In lstCSVFile

            '対象CSV一覧のうち、今回出力したファイル以外は全て削除
            If Not lstOutputCSV.Contains(sCSVFile) Then

                Try
                    File.Delete(sCSVFile)
                Catch ex As Exception
                    MsgBox("位置連携CSV削除失敗" + vbCrLf + sCSVFile)
                End Try

            End If
        Next

    End Sub

    ''' <summary>
    ''' 現全長画像の位置連携CSVファイル一覧を作成する
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetTargetCSV() As List(Of String)

        Dim lstTargetCSV As New List(Of String)

        '検索先パス
        Dim sPath As String = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)
        If Not Directory.Exists(sPath) Then Return lstTargetCSV

        '位置連携ファイル検索
        Dim diSearchFolder As New DirectoryInfo(sPath)
        Dim files As FileInfo() = diSearchFolder.GetFiles(m_sSearchKey)
        For Each fi As FileInfo In files
            lstTargetCSV.Add(IO.Path.Combine(sPath, fi.Name))
        Next

        'ファイルリストを返す
        Return lstTargetCSV

    End Function



End Class