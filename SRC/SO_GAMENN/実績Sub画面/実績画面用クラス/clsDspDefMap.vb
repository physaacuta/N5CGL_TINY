'*********************************************************************************
'　疵マップ表示情報管理クラス
'	[Ver]
'		Ver.01    2008/10/16  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class clsDspDefMap
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                           ' パラメータ管理クラス
    Private mcls_Log As tClass.tClass_LogManager                    ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイル情報表示コントロール
    ''' </summary>
    Public Structure TYP_MAP_CTRL
        Dim lblWs As Label                '疵マップラベル
        Dim lblDs As Label                '疵マップラベル
        Dim picMap As PictureBox
    End Structure

    '---------------------------------------------------------------------------
    ' 型定義
    '---------------------------------------------------------------------------
    '// 疵マップ表示画面（散布図）用の疵シンボル定義
    '// テーブル構造体
    Public Structure DEFMAP_SYMBOL
        Dim mSymbol As String                                                   ' 表示シンボル
        Dim mFont As String                                                     ' フォント
        Dim mSize As Integer                                                    ' サイズ
    End Structure
    Public Const PLOT_SYMBOL_FONT As String = "ＭＳ ゴシック"                   ' 疵シンボルフォント定義
    Public Const PLOT_SYMBOL_SIZE As Integer = 8                                ' 疵シンボルフォントサイズ
    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ 定義
    ''/ /////////////////////////////////////////////////////////////////////////////////
    Private Const nLabelHosei As Integer = 12                                   'ラベル描画位置微調整

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには

    Private m_typCtrl As TYP_MAP_CTRL               '

    Private m_bTurnLR As Boolean                    ' 長手縦方向表示時、左右反転 (True:反転有り False:反転なし) 
    Private m_bTurnTB As Boolean                    ' 長手方向に対して、上下反転 (True:反転有り False:反転なし)
    Private m_bMapDir As Boolean                    ' マップの上下方向 (True:マップ上部がコイル先頭 False:マップ下部がコイル先頭)

    Private m_nMen As Integer                       ' 表示面(0:表面、1:裏面)
    Private m_nCoilLen As Integer = 0               ' コイル実績長 [mm]
    Private m_nCoilWid As Integer = 0               ' 板幅 [mm]

    Private mcls_Map As tClass_MapBase              ' 描画タイプの疵マップ表示

    Private m_lstDefData As New List(Of DETECT_BASE_DATA)   ' 疵情報リスト
    Private m_lstCutData As New List(Of CUT_INF_DATA)       ' カット線描画情報リスト

    Private m_nMarkerLabel As EM_MARKER_MODE         ' 表示マーカーラベルID
    Private m_nMarkerColor As EM_MARKER_COLOR_MODE   ' 表示マーカーカラーID

    Private m_PlotSymbol() As DEFMAP_SYMBOL                                      ' 疵シンボル (疵種ごとに定義)

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' マップの上下方向 (True:マップ上部がコイル先頭 False:マップ下部がコイル先頭)
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MapDir As Boolean
        Get
            Return m_bMapDir
        End Get
        Set(ByVal Value As Boolean)
            m_bMapDir = Value
        End Set
    End Property

    ''' <summary>
    ''' 長手方向に対して、上下反転 (True:反転有り False:反転なし)
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_TurnTB As Boolean
        Get
            Return m_bTurnTB
        End Get
        Set(ByVal Value As Boolean)
            m_bTurnTB = Value
        End Set
    End Property

    ''' <summary>
    ''' 長手縦方向表示時、左右反転 (True:反転有り False:反転なし) 
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_TurnLR As Boolean
        Get
            Return m_bTurnLR
        End Get
        Set(ByVal Value As Boolean)
            m_bTurnLR = Value
            If m_bTurnLR = True Then
                m_typCtrl.lblDs.Text = "WS"
                m_typCtrl.lblWs.Text = "DS"
            Else
                m_typCtrl.lblDs.Text = "DS"
                m_typCtrl.lblWs.Text = "WS"
            End If
        End Set
    End Property

    ''' <summary>
    '''  表示マーカーラベルID
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MarkerLabel As EM_MARKER_MODE
        Get
            Return m_nMarkerLabel
        End Get
        Set(ByVal Value As EM_MARKER_MODE)
            m_nMarkerLabel = Value
        End Set
    End Property

    ''' <summary>
    ''' 表示マーカーカラーID
    ''' </summary>
    ''' <returns></returns>
    Public Property tProp_MarkerColor As EM_MARKER_COLOR_MODE
        Get
            Return m_nMarkerColor
        End Get
        Set(ByVal Value As EM_MARKER_COLOR_MODE)
            m_nMarkerColor = Value
        End Set
    End Property

    ''' <summary>
    ''' コイル幅をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("コイル幅をセット")>
    Public Property tProp_nCoilWid() As Integer
        Get
            Return m_nCoilWid
        End Get
        Set(ByVal Value As Integer)
            m_nCoilWid = Value
        End Set
    End Property

    ''' <summary>
    ''' コイル長をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("コイル長をセット")>
    Public Property tProp_nCoilLen() As Integer
        Get
            Return m_nCoilLen
        End Get
        Set(ByVal Value As Integer)
            m_nCoilLen = Value
        End Set
    End Property

    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCtrl As TYP_MAP_CTRL, ByVal nMen As Integer, ByRef param As clsParamManager, ByRef log As tClass_LogManager)

        '疵マップ画面コントロール
        m_typCtrl = typCtrl

        m_nMen = nMen

        mcls_Log = log
        mcls_Param = param

        '選択疵取得
        m_lstDefData = Nothing
        m_lstCutData = Nothing

        ' 表示シンボル、フォント、サイズの初期化
        ReDim m_PlotSymbol(PLOT_SYMBOL_TYPE.Length - 1)
        For ii As Integer = 0 To PLOT_SYMBOL_TYPE.Length - 1
            m_PlotSymbol(ii).mSymbol = PLOT_SYMBOL_TYPE(ii)
            m_PlotSymbol(ii).mFont = PLOT_SYMBOL_FONT
            m_PlotSymbol(ii).mSize = PLOT_SYMBOL_SIZE
        Next

        mcls_Map = New tClass_MapBase()
        AddHandler mcls_Map.tEv_DrawPaint_End, AddressOf mcls_Map_tEv_DrawPaint_End
        AddHandler m_typCtrl.picMap.Paint, AddressOf PictureBox_Paint

    End Sub

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
            End If

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
    ''' 現在の表示設定で、疵マップ周囲・目盛りの描画を行います。
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DrawKizuMapFrame()

        ' 疵マップの初期化
        InitializeMap()                                         ' 領域初期化、目盛り設定
        ' コイル設定
        SetCoilSize(m_nCoilLen, m_nCoilWid)                     ' コイル実績長さ、板幅の表示
    End Sub

    ''' <summary>
    ''' 描画
    ''' </summary>
    Public Sub SetDrawPaint()
        mcls_Map.DrawPaint()
    End Sub

    ''' <summary>
    ''' 疵情報設定
    ''' </summary>
    ''' <param name="lstData">疵情報</param>
    ''' <remarks></remarks>
    Public Sub SetDefectData(ByRef lstData As List(Of DETECT_BASE_DATA))
        Try
            m_lstDefData = lstData
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 疵情報設定異常 [{0}] Men=[{1}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' カット位置情報設定
    ''' </summary>
    ''' <param name="lstData"></param>
    ''' <remarks></remarks>
    Public Sub SetCutData(ByRef lstData As List(Of CUT_INF_DATA))
        Try
            m_lstCutData = lstData
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 カット位置情報設定異常 [{0}] ", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵情報設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearDefectData()
        Try
            m_lstDefData = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 疵情報クリア異常 [{0}] Men=[{1}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' カット位置情報設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearCutData()
        Try
            m_lstCutData = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 カット位置情報クリア異常 [{0}] ", ex.Message))
        End Try
    End Sub

    Public Sub DspInit()
        ClearCutData()
        ClearDefectData()
    End Sub

    ''' <summary>
    ''' 疵マップの初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitializeMap()
        Try

            ' 長手 縦方向表示
            With mcls_Map
                '-----------------------------------------------------------
                ' 表示領域情報
                '-----------------------------------------------------------
                .tProp_AriaInf_BackGroundLineColor = Pens.Black
                .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
                .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
                .tProp_AriaInf_AriaSize = m_typCtrl.picMap.Size
                .tProp_AriaInf_Margin = New Padding(30, 30, 30, 30)

                '-----------------------------------------------------------
                ' マップ領域
                '-----------------------------------------------------------
                .tProp_MapInf_IsCenterX = True
                .tProp_MapInf_IsCenterY = False
                .tProp_MapInf_IsBaseX_L = True
                .tProp_MapInf_IsBaseY_T = m_bMapDir
                .tProp_MapInf_IsStrechX = True
                .tProp_MapInf_IsStrechY = True
                .tProp_MapInf_LimitXmin = -1 * CInt(COIL_WID_MAX / 2)
                .tProp_MapInf_LimitXmax = CInt(COIL_WID_MAX / 2)
                .tProp_MapInf_LimitYmin = 0
                .tProp_MapInf_LimitYmax = COIL_LEN_MAX * 1000
                .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

                '-----------------------------------------------------------
                ' マップ対象外領域設定
                '-----------------------------------------------------------
                .tProp_LimitInf_LimitDraw = True

                '-----------------------------------------------------------
                ' 目盛り設定
                '-----------------------------------------------------------
                Dim p As Pen = New Pen(Color.LightBlue, 1)
                p.DashStyle = Drawing2D.DashStyle.DashDot
                .tProp_MemoriInf_LinePen = p
                .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
                .tProp_MemoriInf_PosStringX = 1
                .tProp_MemoriInf_PosStringY = 1
                .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(200, 50 * 1000)
                .tProp_MemoriInf_ScaleStringFormatX = "0"
                .tProp_MemoriInf_ScaleStringFormatY = "###0"
                .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1000)
                .tProp_MemoriInf_StringDisp_T = True
                .tProp_MemoriInf_StringDisp_B = True
                .tProp_MemoriInf_StringDisp_L = True
                .tProp_MemoriInf_StringDisp_R = True
                .tProp_MemoriInf_LineDsp_X = True
                .tProp_MemoriInf_LineDsp_Y = True
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 マップ初期化異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' コイルサイズ設定します
    ''' </summary>
    ''' <param name="nCoilLen">コイル実績長 [mm]</param>
    ''' <param name="nCoilWidth">板幅 [mm]</param>
    ''' <remarks></remarks>
    Private Sub SetCoilSize(ByVal nCoilLen As Integer, ByVal nCoilWidth As Integer)
        Try
            ' コイル幅
            Dim nWid As Integer = nCoilWidth
            If 0 >= nWid Then                                                   ' 幅0なら最大にする
                nWid = COIL_WID_MAX
            End If

            ' 長手 縦方向表示
            With mcls_Map
                '-----------------------------------------------------------
                ' マップ対象外領域設定
                '-----------------------------------------------------------
                .tProp_LimitInf_LimitDraw = True
                .tProp_LimitInf_LimitOutLineColor = Pens.Black
                .tProp_LimitInf_LimitOutColor = New SolidBrush(Color.Gray)

                .tProp_LimitInf_LimitXmin = CInt(nWid / 2) * -1
                .tProp_LimitInf_LimitXmax = CInt(nWid / 2)
                .tProp_MapInf_LimitYmax = nCoilLen
                .tProp_LimitInf_LimitYmin = 0
                .tProp_LimitInf_LimitYmax = .tProp_MapInf_LimitYmax

                Dim intval As tClass_MapBase.xyValue = .tProp_MemoriInf_ScaleInterval
                Dim dLen As Double = nCoilLen / 1000
                If dLen <= 200 Then                                         ' コイル長 200m以下なら20m間隔
                    intval.Y = 20
                ElseIf dLen <= 1000 Then                                    ' コイル長 1000m以下なら100m間隔
                    intval.Y = 100
                ElseIf dLen <= 3000 Then                                    ' コイル長 3000m以下なら200m間隔
                    intval.Y = 200
                ElseIf dLen <= 6000 Then                                    ' コイル長 6000m以下なら500m間隔
                    intval.Y = 500
                ElseIf dLen <= 10000 Then                                   ' コイル長 10000m以下なら1000m間隔
                    intval.Y = 1000
                Else                                                        ' コイル長 10000m越えなら2000m間隔
                    intval.Y = 2000
                End If
                intval.Y *= 1000                                            ' メートル -> ミリメート変換
                .tProp_MemoriInf_ScaleInterval = intval

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 コイル変更異常 [{0}] Len=[{1}] Width=[{2}]", ex.Message, nCoilLen, nCoilWidth))
        End Try
    End Sub

    '***************************************************************************
    ' ピクチャーボックスイベント
    '***************************************************************************
    ''' <summary>
    ''' 疵マップのピクチャーボックス描画イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        ' ビットマップ取得
        If mcls_Map Is Nothing = True Then
            Exit Sub
        End If

        Dim bmp As Bitmap = mcls_Map.tProp_Bmp
        If bmp Is Nothing Then
            Exit Sub
        End If

        '''' 全体マップ背景画像を再生成
        e.Graphics.DrawImage(bmp, 0, 0)

    End Sub


    '***************************************************************************
    ' 疵マップイベント
    '***************************************************************************
    ''' <summary>
    ''' 疵マップ描画終了イベント
    ''' </summary>
    ''' <param name="map">イベント元</param>
    ''' <param name="g">描画へのグラフィック</param>
    ''' <remarks></remarks>
    Private Sub mcls_Map_tEv_DrawPaint_End(ByVal map As tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)

        Try

            '==============================================================
            '  カット位置描画
            '==============================================================
            Dim nPicCutX1 As Integer = 0
            Dim nPicCutY1 As Integer = 0
            Dim nPicCutX2 As Integer = 0
            Dim nPicCutY2 As Integer = 0
            Dim nPosPre As Integer = 0
            Dim pen_Cut As New System.Drawing.Pen(Color.Black, 3)
            Dim fntwk As New System.Drawing.Font("ＭＳ ゴシック", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            Dim bshwk As New SolidBrush(Color.Black)

            If tProp_TurnTB Then
                nPosPre = map.tProp_MapInf.nLimitYmax
            End If
            If m_lstCutData IsNot Nothing Then
                For ii As Integer = 0 To m_lstCutData.Count - 1
                    Dim nPos As Integer = 0

                    nPos = CInt(m_lstCutData(ii).dPosS * 1000)
                    Select Case m_lstCutData(ii).nKubun
                        Case 1  ' シャーカット（ファーストカット)
                            pen_Cut.Color = g_ColorSharStart
                        Case 2  ' シャーカット（ラストカット）
                            pen_Cut.Color = g_ColorSharEnd
                        Case 3  ' 分割 （※分割は、１点のみ。カットは範囲）
                            pen_Cut.Color = g_ColorShar
                        Case 4  ' 異種点
                            pen_Cut.Color = g_ColorIsyuten
                        Case Else
                    End Select

                    nPicCutX1 = map.GetVtoP_X(map.tProp_MapInf_LimitXmin)
                    nPicCutX2 = map.GetVtoP_X(map.tProp_MapInf_LimitXmax)
                    '超えている場合はリミット値に
                    If (nPos > map.tProp_MapInf.nLimitYmax) Then
                        nPos = map.tProp_MapInf.nLimitYmax
                    End If
                    nPicCutY1 = map.GetVtoP_Y(nPos)
                    nPicCutY2 = nPicCutY1
                    g.DrawLine(pen_Cut, nPicCutX1, nPicCutY1, nPicCutX2, nPicCutY2)


                    'マップ端にカット実績IDを表示
                    Dim nPosY As Integer
                    If tProp_TurnTB Then
                        nPosY = nPosPre - CInt((nPosPre - nPos) / 2)
                    Else
                        nPosY = CInt((nPos - nPosPre) / 2) + nPosPre
                    End If

                    Dim nPicPos As Integer = map.GetVtoP_Y(nPosY)
                    g.DrawString(CUT_RESULT_ID_NO(ii), fntwk, bshwk, map.GetVtoP_X(map.tProp_MapInf_LimitXmax) + 1, nPicPos - 8)
                    '前回値保存
                    nPosPre = nPos
                Next
            End If

            pen_Cut.Dispose()
            fntwk.Dispose()
            bshwk.Dispose()

            ''==============================================================
            ''  優先順位で並べ替え
            ''==============================================================
            ''優先順位の低い順にソート(優先順位の高い疵が上に来るように)
            'm_lstDefData.Sort(AddressOf SortUnPriorty)

            '==============================================================
            '  疵シンボル描画
            '==============================================================
            Dim nPicX As Integer = 0
            Dim nPicY As Integer = 0
            Dim fntMemori As New System.Drawing.Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            Dim fntSymbol As New System.Drawing.Font("ＭＳ ゴシック", 6, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

            If m_lstDefData IsNot Nothing Then
                For ii As Integer = 0 To m_lstDefData.Count - 1
                    Dim typ As DETECT_BASE_DATA = m_lstDefData(ii)      ' 疵情報
                    '---------------------------------------------------------------
                    ' X座標の座標算出 (反転考慮)
                    '---------------------------------------------------------------
                    Dim nX As Integer = 0

                    If Not tProp_TurnLR Then
                        nX = CInt(typ.wsc)
                    Else
                        nX = CInt(typ.dsc)
                    End If

                    '---------------------------------------------------------------
                    ' X座標の上下限
                    '---------------------------------------------------------------
                    If nX < map.tProp_LimitInf_LimitXmin Then                       ' 最小X座標を超えた場合は最小座標まで
                        nX = map.tProp_LimitInf_LimitXmin
                    End If
                    If nX > map.tProp_LimitInf_LimitXmax Then                       ' 最大X座標を超えた場合は最大座標まで
                        nX = map.tProp_LimitInf_LimitXmax
                    End If
                    nPicX = map.GetVtoP_X(nX)

                    '---------------------------------------------------------------
                    ' Y座標の座標算出
                    '---------------------------------------------------------------
                    Dim nY As Integer = CInt(Int(typ.y))                        ' 出側からの距離

                    '---------------------------------------------------------------
                    ' Y座標の上下限
                    '---------------------------------------------------------------
                    If nY < map.tProp_LimitInf_LimitYmin Then                       ' 最小X座標を超えた場合は最小座標まで
                        nY = map.tProp_LimitInf_LimitYmin
                    End If
                    If nY > map.tProp_LimitInf_LimitYmax Then                       ' 最大X座標を超えた場合は最大座標まで
                        nY = map.tProp_LimitInf_LimitYmax
                    End If
                    nPicY = map.GetVtoP_Y(nY)

                    '---------------------------------------------------------------
                    ' 描画
                    '---------------------------------------------------------------
                    '---------------------------------------------------------------
                    Dim MapSymbolNo As Integer = mcls_Param.GetMapSymbolNo(typ.nTid)                                ' 表示シンボル番号取得
                    If MapSymbolNo < 0 Then Continue For
                    Dim drawString As String = m_PlotSymbol(MapSymbolNo).mSymbol                                    ' 疵表示シンボル

                    Dim pen_in As SolidBrush
                    If EM_MARKER_COLOR_MODE.nMarkerColorModeType = m_nMarkerColor Then
                        pen_in = New SolidBrush(mcls_Param.GetTypeSymbolColor(typ.nTid))     ' 疵種表示色
                    Else
                        pen_in = New SolidBrush(mcls_Param.GetGradeSymbolColor(typ.nGid))   ' グレード表示色
                    End If
                    Dim drawPoint As New PointF(nPicX - CInt(fntSymbol.Size / 2), nPicY - CInt(fntSymbol.Size / 2)) ' 疵表示座標
                    g.DrawString(drawString, fntSymbol, pen_in, drawPoint)                                          ' 疵表示

                    Dim DrawInfo As String = ""
                    If EM_MARKER_MODE.nMarkerModeType = m_nMarkerLabel Then
                        DrawInfo = mcls_Param.GetMapName(typ.nTid)
                    ElseIf EM_MARKER_MODE.nMarkerModeGrade = m_nMarkerLabel Then
                        DrawInfo = mcls_Param.GetGradeName(typ.nGid)
                    End If
                    g.DrawString(DrawInfo, fntMemori, pen_in, drawPoint.X, drawPoint.Y - nLabelHosei)
                    pen_in.Dispose()
                Next ii
            End If
            fntMemori.Dispose()
            fntSymbol.Dispose()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 疵マップ描画終了イベント異常 [{0}]", ex.Message))
        End Try

    End Sub

End Class
