
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopDefMap


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_MapClick(ByVal pos As Point)						' 散布図クリック (mm座標系)
    Public Event tEv_KizuClick(ByVal pos As Point)						' 散布図クリック (mm座標系)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    Private Const COIL_WID_MAX As Integer = 3000						'// コイル幅MAX[mm]
    Private Const COIL_LEN_MAX As Integer = 20000						'// 入側コイル長MAX[m] 

    Private Const NEAR_DEF_X As Integer = 50							' 近い疵特定範囲
    Private Const NEAR_DEF_Y As Integer = 5000


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    ' クラス
    Private WithEvents mcls_Map As New tClass.tClass_MapBase			' 全長マップ
    Private mcls_pParam As clsParamManager = Nothing					' パラメータ管理クラス (ポインタ)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)

    ' データ
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' 疵情報リスト
    Private m_lstCut As List(Of DATA_CUT_INF) = Nothing                 ' カット情報リスト

    ' もろもろ
    Private m_bDspOk As Boolean = False									' 散布図にちゃんとデータが表示しているとき


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ
    ''' <summary>
    ''' パラメータクラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub

    ''' <summary>
    ''' 疵情報リスト
    ''' </summary>
    ''' <param name="lst">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    ''' <summary>
    ''' カット情報リスト
    ''' </summary>
    ''' <param name="lst">カット情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstCut(ByRef lst As List(Of DATA_CUT_INF))
        m_lstCut = lst
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    Private Sub frmPopDefMap_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
    End Sub


    ''' <summary>
    ''' フォームサイズ変更
    ''' </summary>
    Private Sub picMap_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles picMap.Resize
        mcls_Map.tProp_AriaInf_AriaSize = picMap.Size

        '' 全体マップ背景画像を再生成
        Call mcls_Map.DrawPaint()
        Call picMap.Refresh()
    End Sub
    ''' <summary>
    ''' 描画
    ''' </summary>
    Private Sub PicMap_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles picMap.Paint
        '''' ビットマップ取得
        Dim bmp As Bitmap = mcls_Map.tProp_Bmp
        If bmp Is Nothing Then Return

        '''' 全体マップ背景画像を再生成
        e.Graphics.DrawImage(bmp, 0, 0)
    End Sub

    ''' <summary>
    ''' マップ描画
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MapDisp()
        Call MapInitialize()	' マップ初期化

        If Not mcls_pRpiView.IsRpi Then
            m_bDspOk = False
            Call mcls_Map.DrawPaint()
            Call picMap.Refresh()
            Return
        End If

        '''' ここからは、RPIファイルが開いているときのみ
        Call MapCoilChange()
        m_bDspOk = True
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ マウス操作系
    '	Dim m_oldMousePos As Point				' 前回マウス操作位置	(イベントは、MouseDown,Moveの順なので、近くの疵を選択してもすぐ動いてしまう対策)
    Dim m_bMouseDown As Boolean = False		' ダウン一回目のみ true (イベントが MouseDown,Moveの順なので、動いてしまう対策)

    ''' <summary>
    ''' マウスダウン
    ''' </summary>
    Private Sub picMap_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picMap.MouseDown
        If Not m_bDspOk Then Return
        If Not e.Button = Windows.Forms.MouseButtons.Left Then Return

        Dim pos As tClass_MapBase.xyValue = mcls_Map.GetPtoV(e.X, e.Y)
        m_bMouseDown = True

        '''' 近くの疵があれば、その位置を表示
        Dim ansInf As DATA_DETECT_INF
        If MapDetect_Near(pos, NEAR_DEF_X, NEAR_DEF_Y, ansInf) Then
            'pos.X = ansInf.nXc
            pos.X = ansInf.nFrameXc
            pos.Y = ansInf.nY

            RaiseEvent tEv_KizuClick(New Point(CInt(pos.X), CInt(pos.Y)))
        Else
            RaiseEvent tEv_MapClick(New Point(CInt(pos.X), CInt(pos.Y)))
        End If
        'Debug.WriteLine(String.Format("Down X={0}, Y{1}", pos.X, pos.Y))

        '' センター基準から左端基準に変換	センター基準に変更
        'pos.X += mcls_Map.tProp_LimitInf_LimitXmax
    End Sub
    ''' <summary>
    ''' マウスムーブ
    ''' </summary>
    Private Sub picMap_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picMap.MouseMove
        If Not m_bDspOk Then Return
        If Not e.Button = Windows.Forms.MouseButtons.Left Then Return
        '' イベントは、MouseDown,Moveの順なので、近くの疵を選択してもすぐ動いてしまう対策
        If m_bMouseDown Then
            m_bMouseDown = False
            Return
        End If
        Dim pos As tClass_MapBase.xyValue = mcls_Map.GetPtoV(e.X, e.Y)

        'Debug.WriteLine(String.Format("Down X={0}, Y{1}", pos.X, pos.Y))
        '' センター基準から左端基準に変換
        'pos.X += mcls_Map.tProp_LimitInf_LimitXmax
        RaiseEvent tEv_MapClick(New Point(CInt(pos.X), CInt(pos.Y)))
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' マップ基本情報定義
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInitialize()
        '''' グラフ描画に必要な情報を設定
        With mcls_Map
            '' 表示領域情報
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picMap.Size
            .tProp_AriaInf_Margin = New Padding(38, 15, 10, 15)

            '' マップ領域
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = True
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = -1 * CInt(COIL_WID_MAX / 2)
            .tProp_MapInf_LimitXmax = CInt(COIL_WID_MAX / 2)
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = COIL_LEN_MAX * 1000
            .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

            '' マップ対象外領域設定
            .tProp_LimitInf_LimitDraw = False
            '''' コイルを設定する
            '.tProp_LimitInf_LimitXmin = .tProp_MapInf_LimitXmin
            '.tProp_LimitInf_LimitXmax = .tProp_MapInf_LimitXmax
            '.tProp_LimitInf_LimitYmin = .tProp_MapInf_LimitYmin
            '.tProp_LimitInf_LimitYmax = .tProp_MapInf_LimitYmax

            '' メモリ設定
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
            .tProp_MemoriInf_PosStringX = 1
            .tProp_MemoriInf_PosStringY = 1
            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(200, 200 * 1000)
            .tProp_MemoriInf_ScaleStringFormatX = "0"
            .tProp_MemoriInf_ScaleStringFormatY = "###0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1000)
            .tProp_MemoriInf_StringDisp_T = True
            .tProp_MemoriInf_StringDisp_B = True
            .tProp_MemoriInf_StringDisp_L = True
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = True
            .tProp_MemoriInf_LineDsp_Y = True
        End With
    End Sub

    ''' <summary>
    ''' 全体マップ コイル情報セット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapCoilChange()

        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_pRpiView.GetRpi_Header
        Dim nWid As Integer = CInt(Int(typ.width * typ.res_x))
        Dim nLen As Integer = CInt(Int((typ.nframe + 1) * typ.height * typ.res_y))

        If 0 >= nWid Then nWid = COIL_WID_MAX
        If 0 >= nLen Then nLen = COIL_LEN_MAX


        '''' マップ対象外領域設定
        mcls_Map.tProp_LimitInf_LimitDraw = True
        mcls_Map.tProp_LimitInf_LimitOutLineColor = Pens.Black
        mcls_Map.tProp_LimitInf_LimitOutColor = New SolidBrush(Color.Gray)

        mcls_Map.tProp_LimitInf_LimitXmin = CInt(nWid / 2) * -1
        mcls_Map.tProp_LimitInf_LimitXmax = CInt(nWid / 2)
        mcls_Map.tProp_MapInf_LimitYmax = nLen
        mcls_Map.tProp_MapInf_LimitXmin = mcls_Map.tProp_LimitInf_LimitXmin
        mcls_Map.tProp_MapInf_LimitXmax = mcls_Map.tProp_LimitInf_LimitXmax
        mcls_Map.tProp_LimitInf_LimitYmax = mcls_Map.tProp_MapInf_LimitYmax


        '''' 全体マップ背景画像を再生成
        Call mcls_Map.DrawPaint()
        ' 出来上がった背景を描画
        Call picMap.Refresh()
    End Sub

    ''' <summary>
    ''' 疵情報をプロット
    ''' </summary>
    Private Sub mcls_Map_tEv_DrawPaint_End(ByVal cls As tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics) Handles mcls_Map.tEv_DrawPaint_End
        If m_lstDetect Is Nothing Then Return


        '' ================================================
        '' サンプル専用
        'If 1 = g_nLockMode Then
        '	Dim memoriBsh As SolidBrush = New SolidBrush(Color.Red)
        '	Dim memoriFnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 25, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        '	For ii As Integer = 1 To 5
        '		g.DrawString("サンプル版", memoriFnt, memoriBsh, 20, 100 * ii)
        '	Next

        'End If



        Const protsize As Integer = 4
        Dim b As SolidBrush = New SolidBrush(Color.Black)
        Dim f As Font = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim x As Integer
        Dim y As Integer

        For Each data As DATA_DETECT_INF In m_lstDetect
            If Not data.bDsp Then Continue For

            '''' 値からグラフ座標系に変換
            x = cls.GetVtoP_X(data.nFrameXc)
            y = cls.GetVtoP_Y(data.nY)

            ' 色
            Dim gr As clsParamManager.PARAM_GRADE = mcls_pParam.GetParamGrade(data.gId)
            b.Color = gr.clrDspColor
            If gr.clrDspColor.R = 255 AndAlso gr.clrDspColor.G = 255 AndAlso gr.clrDspColor.B = 255 Then
                b.Color = Color.Black
            End If

            ' プロット描画
            g.FillRectangle(b, x - CInt(protsize / 2), y - CInt(protsize / 2), protsize, protsize)

        Next

        Dim nPicCutX1 As Integer = 0
        Dim nPicCutY1 As Integer = 0
        Dim nPicCutX2 As Integer = 0
        Dim nPicCutY2 As Integer = 0
        Dim pen_Cut As New System.Drawing.Pen(Color.Green, 2)
        pen_Cut.DashStyle = Drawing2D.DashStyle.Dash
        For Each data As DATA_CUT_INF In m_lstCut
            nPicCutX1 = cls.GetVtoP_X(cls.tProp_MapInf_LimitXmin)
            nPicCutX2 = cls.GetVtoP_X(cls.tProp_MapInf_LimitXmax)
            nPicCutY1 = cls.GetVtoP_Y(data.nY)
            nPicCutY2 = nPicCutY1
            g.DrawLine(pen_Cut, nPicCutX1, nPicCutY1, nPicCutX2, nPicCutY2)
        Next




        b.Dispose()
        f.Dispose()
        pen_Cut.Dispose()
    End Sub


    ''' <summary>
    ''' もっとも近い疵を抽出
    ''' </summary>
    ''' <remarks></remarks>
    Private Function MapDetect_Near(ByVal nPos As tClass_MapBase.xyValue, ByVal nLimitX As Integer, ByVal nLimitY As Integer, ByRef ansInf As DATA_DETECT_INF) As Boolean
        Dim bNear As Boolean = False

        ' 一番近いやつの情報
        Dim typNear As DATA_DETECT_INF
        Dim nNearX As Integer = 0
        Dim nNearY As Integer = 0


        For Each data As DATA_DETECT_INF In m_lstDetect
            ' そもそも問題外
            Dim nWkX As Integer = CInt(Math.Abs(nPos.X - data.nFrameXc))
            Dim nWkY As Integer = CInt(Math.Abs(nPos.Y - data.nY))
            If nWkX > nLimitX Then Continue For
            If nWkY > nLimitY Then Continue For

            '''' 近いやつ有り
            ' 初めて
            ' より近い人
            If Not bNear OrElse (Math.Abs(nNearX * nNearY) > Math.Abs(nWkX * nWkY)) Then
                bNear = True
                typNear = data
                nNearX = nWkX
                nNearY = nWkY
                Continue For
            End If
        Next

        If bNear Then
            ansInf = typNear
        End If
        Return bNear
    End Function



End Class