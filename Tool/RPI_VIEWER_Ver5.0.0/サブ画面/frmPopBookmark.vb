Imports tClassLibrary.tClass

Public Class frmPopBookmark

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    Private Const MAX_NUM As Integer = 9		' 登録可能最大数

    Public Structure INFO_MARK
        Dim bIsData As Boolean			' false:無効

        ' ファイル全体での情報
        Dim nPosY As Integer			' 長手位置 [mm] ※画像左上から	※ジャンプ用
        Dim nPosX As Integer			' 幅位置 [mm]	※画像左上から

        Dim nPixelAllX As Integer		' 全フレーム左上からの距離 [RP] [pixel] ※画像左上から
        Dim nPixelAllY As Integer		' 全フレーム左上からの距離 [RP] [pixel] ※画像左上から


        ' 該当フレーム内での情報	※描画で必要
        Dim nFrameNo As Integer			' 指定した点のフレームNoをセット
        Dim nPixelX As Integer			' フレーム左上からの距離 [RP] [pixel]
        Dim nPixelY As Integer			' フレーム左上からの距離 [RP] [pixel]

        ' エッジからの距離
        Dim bEdge As Boolean			' 有効無効
        Dim nEdgeL As Integer			' エッジからの距離 [mm]
        Dim nEdgeR As Integer
    End Structure



    Private Enum EM_LIST
        em_No = 0		' FuncNo (1～) ボタン
        em_PosY			' 先端からの位置 [mm]
        em_PosX			' 左端からの位置 [mm]

        em_EdgeL		' 左エッジからの距離 [mm]
        em_EdgeR		' 右エッジからの距離 [mm]
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


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)
    Private mcls_pCalc As tClass_ImageDsp_Calc		' 画像計算クラス (ポインタ)
    Private m_BasePic As PictureBox					' ベースピクチャーボックス

    ' 登録情報
    Private m_Info(MAX_NUM - 1) As INFO_MARK		' 登録情報


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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopBookmark_Load(sender As Object, e As System.EventArgs) Handles Me.Load
        Call DgvInitDefect(dgvList)
        Call DspGridData()			' 初回描画
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部通知

    ''' <summary>
    ''' 追加
    ''' </summary>
    ''' <param name="nKey">押下された数字(0オリジン)</param>
    Public Sub MarkAdd(ByVal nKey As Integer)
        If Not mcls_pRpiView.IsRpi Then Return

        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_pRpiView.GetDspAriaInf
        Dim nowDP As Point = m_BasePic.PointToClient(Windows.Forms.Cursor.Position)		  ' 現在のカーソル位置
        Dim nowRP As Point = mcls_pRpiView.GetCls_ImgDsp.GetDspPixelToImgPosition(nowDP)
        nowRP.Y += mcls_pRpiView.GetDspAriaInf.nDspFno * mcls_pRpiView.GetImg_ImgInf.nOneImgH	' 見えていない分を加算

        Dim pixelF As PointF
        pixelF.X = mcls_pRpiView.GetPos_DpToLen_X(DspInf.DspPixelTotalOffset.X + nowDP.X)
        pixelF.Y = mcls_pRpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y + nowDP.Y)

        ' 情報登録
        m_Info(nKey).bIsData = True
        'm_Info(nKey).nPosX = CInt(mcls_pRpiView.GetPos_DpToLen_X(DspInf.DspPixelTotalOffset.X + nowPos.X))
        'm_Info(nKey).nPosY = CInt(mcls_pRpiView.GetPos_DpToLen_Y(DspInf.DspPixelTotalOffset.Y + nowPos.Y))

        'm_Info(nKey).nPixelAllX = CInt(mcls_pRpiView.GetPos_DpToRp_X(DspInf.DspPixelTotalOffset.X + nowPos.X))
        'm_Info(nKey).nPixelAllY = CInt(mcls_pRpiView.GetPos_DpToRp_Y(DspInf.DspPixelTotalOffset.Y + nowPos.Y))

        'm_Info(nKey).nFrameNo = CInt(Int(m_Info(nKey).nPixelAllY / mcls_pCalc.tProp_ImgInf.nOneImgH))
        'm_Info(nKey).nPixelY = m_Info(nKey).nPixelAllY Mod mcls_pCalc.tProp_ImgInf.nOneImgH
        'm_Info(nKey).nPixelX = m_Info(nKey).nPixelAllX

        m_Info(nKey).nPosX = CInt(pixelF.X)
        m_Info(nKey).nPosY = CInt(pixelF.Y)

        m_Info(nKey).nPixelAllX = nowRP.X
        m_Info(nKey).nPixelAllY = nowRP.Y


        m_Info(nKey).nFrameNo = nowRP.Y \ mcls_pCalc.tProp_ImgInf.nOneImgH
        m_Info(nKey).nPixelY = nowRP.Y Mod mcls_pCalc.tProp_ImgInf.nOneImgH
        m_Info(nKey).nPixelX = nowRP.X


        '-----------------------------------
        ' エッジからの距離
        If mcls_pRpiView.GetRpi_Header().IsTHeader AndAlso 1 = mcls_pRpiView.GetRpi_THeader().frame_info_type Then
            Try
                Dim nWk As Integer
                nWk = m_Info(nKey).nFrameNo + 1		 ' 0オリジンから1オリジンに
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1 = mcls_pRpiView.GetRpi_Cls().GetRpiFrameAdd1(nWk)

                'If typFinfo.nEdgeIdx(0) < nowRP.X And typFinfo.nEdgeIdx(1) > nowRP.X Then


                ' 左エッジ
                If Not mcls_pRpiView.GetProp_DSWS() Then
                    nWk = nowRP.X - typFinfo.nEdgeIdx(0)
                Else
                    nWk = typFinfo.nEdgeIdx(1) - nowRP.X
                End If
                nWk = CInt(mcls_pCalc.GetPos_RpToLen_X(nWk))
                m_Info(nKey).nEdgeL = nWk
                ' 右エッジ
                If Not mcls_pRpiView.GetProp_DSWS() Then
                    nWk = typFinfo.nEdgeIdx(1) - nowRP.X
                Else
                    nWk = nowRP.X - typFinfo.nEdgeIdx(0)
                End If
                nWk = CInt(mcls_pCalc.GetPos_RpToLen_X(nWk))
                m_Info(nKey).nEdgeR = nWk


                m_Info(nKey).bEdge = True
                'Else
                '	m_Info(nKey).bEdge = False
                'End If

            Catch ex As Exception
                m_Info(nKey).bEdge = False
            End Try
        Else
            m_Info(nKey).bEdge = False
        End If



        '' 結果表示
        Call DspGridData()
    End Sub

    ''' <summary>
    ''' ブックマーク位置にジャンプ
    ''' </summary>
    ''' <param name="nKey">押下された数字(0オリジン)</param>
    ''' <remarks></remarks>
    Public Sub MarkJump(ByVal nKey As Integer)
        If Not m_Info(nKey).bIsData Then Return

        Dim inf As INFO_MARK = m_Info(nKey)

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

        'RaiseEvent tEv_JumpNagate(2, CInt(infoLen.nPosY))
        'RaiseEvent tEv_JumpHaba(2, CInt(infoLen.nPosX), 0)
    End Sub

    ''' <summary>
    ''' ブックマーク位置削除
    ''' </summary>
    ''' <param name="nKey">押下された数字(0オリジン)  -1:全部</param>
    ''' <remarks></remarks>
    Public Sub MarkDel(ByVal nKey As Integer)
        If -1 = nKey Then
            For ii As Integer = 0 To MAX_NUM - 1
                m_Info(ii).bIsData = False
            Next
        Else
            m_Info(nKey).bIsData = False
        End If

        Call DspGridData()
    End Sub

    ' ''' <summary>
    ' ''' ブックマーク全削除
    ' ''' </summary>
    ' ''' <remarks></remarks>
    'Public Sub MarkAllDel()
    '	For ii As Integer = 0 To MAX_NUM - 1
    '		Call MarkDel(ii, True)
    '	Next
    '	Call DspGridData()
    'End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 描画

    ''' <summary>
    ''' 指定地点描画
    ''' </summary>
    ''' <param name="g">グラフィックス</param>
    ''' <remarks></remarks>
    Public Sub PaintPos(ByVal g As Graphics, ByVal nNowStartFNo As Integer, ByVal nDspFrameRowCnt As Integer)
        Dim nHosei As Integer = 10
        Dim pos As Point		' 表示座標位置
        Dim infoLen As INFO_MARK

        Dim pen As Pen = New System.Drawing.Pen(Color.Blue, 2)


        '' 始点位置
        For ii As Integer = 0 To MAX_NUM - 1
            infoLen = m_Info(ii)
            If Not infoLen.bIsData Then Continue For ' データの有無

            ' 表示しているフレーム？
            If infoLen.nFrameNo >= nNowStartFNo And infoLen.nFrameNo <= nNowStartFNo + nDspFrameRowCnt - 1 Then
                ' 表示座標に変換
                infoLen.nPixelY += (infoLen.nFrameNo - nNowStartFNo) * mcls_pCalc.tProp_ImgInf.nOneImgH
                pos = mcls_pCalc.GetPosition(infoLen.nPixelX, infoLen.nPixelY)

                ' 表示 星型
                'Dim ps As Point() = {New Point(pos.X, pos.Y - 10), New Point(pos.X + 3, pos.Y - 4), New Point(pos.X + 10, pos.Y - 6),
                '   New Point(pos.X + 3, pos.Y), New Point(pos.X + 8, pos.Y + 8), New Point(pos.X, pos.Y + 4),
                '   New Point(pos.X - 8, pos.Y + 8), New Point(pos.X - 3, pos.Y),
                '   New Point(pos.X - 10, pos.Y - 6), New Point(pos.X - 3, pos.Y - 4)}
                Dim ps As Point() = {New Point(pos.X, pos.Y - 10), New Point(pos.X + 5, pos.Y - 4), New Point(pos.X + 9, pos.Y - 6), _
                New Point(pos.X + 5, pos.Y), New Point(pos.X + 8, pos.Y + 8), New Point(pos.X, pos.Y + 5), _
                New Point(pos.X - 8, pos.Y + 8), New Point(pos.X - 5, pos.Y), _
                New Point(pos.X - 10, pos.Y - 6), New Point(pos.X - 5, pos.Y - 4)}
                g.DrawPolygon(pen, ps)


                Using wkFnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 8, System.Drawing.FontStyle.Regular, GraphicsUnit.Pixel)
                    g.DrawString(CStr(ii + 1), wkFnt, Brushes.Blue, pos.X - 2, pos.Y - 4)
                End Using
            End If
        Next ii

        '' 後始末
        pen.Dispose()
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
            .ScrollBars = ScrollBars.None								   ' スクロールバーの表示
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
                clm.HeaderText = "FNo"
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
            '
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "左端からの位置[mm]"
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


            '''' 固定行設定
            .RowCount = MAX_NUM
            For ii As Integer = 0 To MAX_NUM - 1
                .Item(EM_LIST.em_No, ii).Value = (ii + 1).ToString
            Next
        End With
    End Sub
#End Region

    ''' <summary>
    ''' データグリッドビューに描画
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspGridData()

        ' フォームロード前に呼ばれる可能性があるための対策
        If MAX_NUM > dgvList.RowCount Then Return

        For ii As Integer = 0 To MAX_NUM - 1
            If m_Info(ii).bIsData Then
                dgvList(EM_LIST.em_PosY, ii).Value = String.Format("{0:0.000}", m_Info(ii).nPosY / 1000.0)
                dgvList(EM_LIST.em_PosX, ii).Value = m_Info(ii).nPosX


                If m_Info(ii).bEdge Then
                    dgvList(EM_LIST.em_EdgeL, ii).Value = m_Info(ii).nEdgeL
                    dgvList(EM_LIST.em_EdgeR, ii).Value = m_Info(ii).nEdgeR
                Else
                    dgvList(EM_LIST.em_EdgeL, ii).Value = ""
                    dgvList(EM_LIST.em_EdgeR, ii).Value = ""
                End If
            Else
                dgvList(EM_LIST.em_PosY, ii).Value = ""
                dgvList(EM_LIST.em_PosX, ii).Value = ""

                dgvList(EM_LIST.em_EdgeL, ii).Value = ""
                dgvList(EM_LIST.em_EdgeR, ii).Value = ""

            End If
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
            Case EM_LIST.em_No		' ファンクションNo
                If Not m_Info(row).bIsData Then Return
                Call MarkJump(row)	' ジャンプ

            Case Else

        End Select
    End Sub


End Class