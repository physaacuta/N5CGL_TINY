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
Public Class clsDefMap
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                                       ' パラメータ管理クラス
    Private mcls_Log As tClass.tClass_LogManager                                ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const MEMWID_MAX As Integer = COIL_WID_MAX                          ' マップ描画幅最大値（2000mm）

    Private Const HOSEI_Y As Integer = 5                                        ' Y軸描画補正（文字位置調整用）

    Private Const PICMAP_START As Integer = 20                                  ' 描画領域上部余白部
    Private Const PICMAP_END As Integer = 10                                    ' 描画領域下部余白部
    Private Const PICMAP_HEIGHT_OUT As Integer = PICMAP_START + PICMAP_END      ' 描画領域上部下部余白合計
    Private Const PICMAP_EDGE As Integer = 40                                   ' 描画領域幅方向余白部
    Private Const PICMAP_WIDTH_OUT As Integer = PICMAP_EDGE * 2                 ' 描画領域幅方向余白合計
    Private Const PICMAP_MMtoM As Integer = 1000                                ' mm->m
    Private Const PICMAP_DSP_HOSEI As Integer = 1                               ' 位置微調整
    Private Const PICMAP_HOSEI_X As Integer = 6                                 ' マーカー描画位置微調整
    Private Const PICMAP_LBL_HOSEI As Integer = 12                              ' ラベル描画位置微調整

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False                                    ' 重複する呼び出しを検出するには
    Private m_typDsp As DSP_CONTROL_DATA                                        ' 疵マップ画面クラス
    Private m_blnUpTurn As Boolean                                              ' 上下反転フラグ(True:反転あり)
    Private m_blnSideTurn As Boolean                                            ' 左右反転フラグ(True:反転あり)
    Private m_nMode As Integer                                                  ' 裏表区分(0:表、1:裏)
    Private m_DefMapData As New List(Of MAP_DATA)                               ' 疵マップ表示用データ
    Private m_nDrawRangeWidth As Integer = MEMWID_MAX                           ' 疵マップ描画幅
    Private m_nCoilWid As Integer                                               ' コイル幅
    Private m_nCoilLen As Integer                                               ' コイル長
    Private m_typMapPri As MAP_PRI                                              ' マップ表示優先度（PCODE_OUTPUT設定内容）
    Private m_ArrayPri As New ArrayList                                         ' マップ表示優先度保存用

    '基本情報取得用
    Public Structure BASE_DATA
        Dim strSeisanNo As String                                       ' 生産No
        Dim nCoilLen As Integer                                         ' コイル長
        Dim nWidZai As Integer                                          ' 材料サイズ板幅
        Dim nPCode() As Integer                                         ' 検査パターンコード

        Public Sub initialize()
            strSeisanNo = ""
            nCoilLen = 0
            nWidZai = 0
            ReDim nPCode(NUM_MEN - 1)
        End Sub
    End Structure

    '表示用データ用
    Public Structure DSP_DATA
        Dim strKizukenNo As String                                      ' 疵検管理NO

        Dim typBaseData As BASE_DATA                                    ' 基本情報
        Dim nMaxDefPos As Integer                                       ' 最長疵位置
        Dim DefectList() As List(Of clsDefMap.MAP_DATA)                 ' 疵マップ情報
        Dim CutLineList As List(Of clsDefMap.CUT_DATA)                  ' カット線情報
        Dim SaisunList As List(Of clsDefMap.SAISUN_DATA)                ' 採寸情報

        Dim bGetDefectFlg As Boolean                                    ' 疵情報取得済

        ' 初期化
        Public Sub Initialize()
            strKizukenNo = ""
            typBaseData = New BASE_DATA
            typBaseData.initialize()
            nMaxDefPos = 0
            ReDim DefectList(NUM_MEN - 1)
            For ii As Integer = 0 To NUM_MEN - 1
                DefectList(ii) = New List(Of clsDefMap.MAP_DATA)
            Next
            CutLineList = New List(Of clsDefMap.CUT_DATA)
            SaisunList = New List(Of clsDefMap.SAISUN_DATA)

            bGetDefectFlg = False
        End Sub
    End Structure

    '疵マップ情報
    Public Structure DSP_CONTROL_DATA
        Dim lblMsg As Label               'ガイダンスメッセージ
        Dim prbMsg As ProgressBar         'ガイダンス進捗
        Dim lblWs As Label                '疵マップラベル
        Dim lblDs As Label                '疵マップラベル
        Dim lblNgMsg As Label             '異常ガイダンス表示用
    End Structure

    '出力対象疵、優先度取得用
    Public Structure MAP_PRI
        Dim t_id As Integer                                      '// 疵種EdasysID
        Dim priority As Integer                                  '// 優先度
        Public Sub initialize()
            t_id = 0
            priority = 0
        End Sub
    End Structure

    '疵マップ情報取得用
    Public Structure MAP_DATA
        Dim t_id As UShort                                      '// 疵種EdasysID
        Dim g_id As UShort                                      '// グレードEdasysID
        Dim y As Single                                         '// 溶接からの位置[m]
        Dim absX As Single                                      '// 幅方向絶対位置[mm]

        Public Sub initialize()
            t_id = 0
            g_id = 0
            y = 0
            absX = 0
        End Sub
    End Structure

    'カット情報取得用
    Public Structure CUT_DATA
        Dim cut_flg As Integer             '// カット位置区分(コイル長超える疵表示に使用。0:コイル長超える疵なし,1:コイル長超える疵あり)
        Dim Cut_Point As Integer           '// カット位置（コイル長超える疵存在の場合はコイル終端位置）
        Public Sub initialize()
            Cut_Point = 0
        End Sub
    End Structure

    '採寸情報取得用
    Public Structure SAISUN_DATA
        Dim Start_Point As Integer         '// 採寸開始位置（コイル長超える疵存在の場合は最長疵長手位置）
        Dim End_Point As Integer           '// 採寸終了位置（コイル長超える疵存在の場合は最長疵長手位置）
        Public Sub initialize()
            Start_Point = 0
            End_Point = 0
        End Sub
    End Structure

    '疵マーカーラベル
    Public Enum EM_MARKER_MODE
        nMarkerModeNone = 0     '疵なし
        nMarkerModeType         '疵種
        nMarkerModeGrade        'ｸﾞﾚｰﾄﾞ
    End Enum

    '疵マーカー色替え
    Public Enum EM_MARKER_COLOR_MODE
        nMarkerColorModeType = 0    '疵種
        nMarkerColorModeGrade       'ｸﾞﾚｰﾄﾞ
    End Enum

    ''' <summary>
    ''' コイル幅をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("コイル幅をセット")> _
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
     , System.ComponentModel.Description("コイル長をセット")> _
    Public Property tProp_nCoilLen() As Integer
        Get
            Return m_nCoilLen
        End Get
        Set(ByVal Value As Integer)
            m_nCoilLen = Value
        End Set
    End Property

    ''' <summary>
    ''' 疵情報をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("疵情報をセット")> _
    Public Property tProp_DefMapData() As List(Of MAP_DATA)
        Get
            Return m_DefMapData
        End Get
        Set(ByVal Value As List(Of MAP_DATA))
            m_DefMapData = Value
        End Set
    End Property

    ''' <summary>
    ''' 表裏区分をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("表裏区分をセット")> _
    Public Property tProp_nMode() As Integer
        Get
            Return m_nMode
        End Get
        Set(ByVal Value As Integer)
            m_nMode = Value
        End Set
    End Property

    ''' <summary>
    ''' 上下反転をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("上下反転をセット")> _
    Public Property tProp_bUpTurn() As Boolean
        Get
            Return m_blnUpTurn
        End Get
        Set(ByVal Value As Boolean)
            m_blnUpTurn = Value
        End Set
    End Property

    ''' <summary>
    ''' 左右反転をセット
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆疵マップ表示情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("左右反転をセット")> _
    Public Property tProp_bSideTurn() As Boolean
        Get
            Return m_blnSideTurn
        End Get
        Set(ByVal Value As Boolean)
            m_blnSideTurn = Value
        End Set
    End Property

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typDspdata">疵マップ情報</param>
    ''' <param name="blnUpTurn">上下反転区分</param>
    ''' <param name="blnSideTurn">左右反転区分</param>
    ''' <param name="nMode">裏表区分</param>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typDspdata As DSP_CONTROL_DATA, _
                    ByVal blnUpTurn As Boolean, ByVal blnSideTurn As Boolean, ByVal nMode As Integer, _
                    ByRef clsLog As tClass.tClass_LogManager, ByVal clsParam As clsParamManager)

        '疵マップ画面コントロール
        m_typDsp = typDspdata

        '上下反転区分
        m_blnUpTurn = blnUpTurn

        '上下反転区分
        m_blnSideTurn = blnSideTurn

        '裏表区分
        m_nMode = nMode

        'ログクラス
        mcls_Log = clsLog

        'ログクラス
        mcls_Param = clsParam

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
    ''' 疵マップ情報初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()

        ''保存領域クリア
        m_DefMapData.Clear()

    End Sub


    ''' <summary>
    ''' 疵マップをペイントイベントで表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub picMapBase_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        Dim fntMemori As New System.Drawing.Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim penMemori As New System.Drawing.Pen(Color.Black, 1)
        Dim penMemori_2 As New System.Drawing.Pen(Color.Cyan, 1)
        penMemori_2.DashStyle = Drawing2D.DashStyle.Dash
        Dim penEdge As New System.Drawing.Pen(Color.Red, 2)
        penEdge.DashStyle = Drawing2D.DashStyle.Dot

        Dim bshMemori As New SolidBrush(Color.Black)
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth ' マップ描画幅方向1メモリ長さ
        Dim nPicMapCenterW As Integer = CInt(picMap.Width / 2)                              ' マップ描画領域幅中心位置
        Dim nCoilWid_Max As Integer = CInt(m_nDrawRangeWidth / 2)
        Dim nCoilWid As Integer = CInt(m_nCoilWid / 2 * dPicMapMemWid)


        '長手方向メモリ幅取得
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        'コイル幅インデックス
        e.Graphics.DrawString("C", fntMemori, bshMemori, nPicMapCenterW - HOSEI_Y, 0)
        If m_blnSideTurn = False Then
            m_typDsp.lblWs.Text = "WS"
            m_typDsp.lblDs.Text = "DS"
        Else
            m_typDsp.lblWs.Text = "DS"
            m_typDsp.lblDs.Text = "WS"
        End If

        'コイル幅点線表示
        'コイル幅中心線描画
        e.Graphics.DrawLine(penMemori_2, nPicMapCenterW, PICMAP_START, nPicMapCenterW, picMap.Height - PICMAP_END)
        For ii = 1 To nCoilWid_Max
            '幅200mm毎に点線とメモリを描画
            If ii Mod 200 = 0 Then
                'CENTER左
                e.Graphics.DrawLine(penMemori_2, CInt(nPicMapCenterW - (dPicMapMemWid * ii)), PICMAP_START, CInt(nPicMapCenterW - (dPicMapMemWid * ii)), picMap.Height - PICMAP_END)
                e.Graphics.DrawString(CStr(ii), fntMemori, bshMemori, CInt(nPicMapCenterW - (dPicMapMemWid * ii) - PICMAP_END), HOSEI_Y)

                'CENTER右
                e.Graphics.DrawLine(penMemori_2, CInt(nPicMapCenterW + (dPicMapMemWid * ii)), PICMAP_START, CInt(nPicMapCenterW + (dPicMapMemWid * ii)), picMap.Height - PICMAP_END)
                e.Graphics.DrawString(CStr(ii), fntMemori, bshMemori, CInt(nPicMapCenterW + (dPicMapMemWid * ii) - PICMAP_END), HOSEI_Y)

            End If
        Next ii

        'コイル長点線、メモリ描画
        Dim nY As Integer = PICMAP_START
        If m_blnUpTurn Then                 '上下反転あり
            nY = picMap.Height - PICMAP_END - HOSEI_Y * 2
        End If

        e.Graphics.DrawString(CStr(0).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY)
        For ii = 1 To m_nCoilLen
            nY = CInt((nInterval * ii) + PICMAP_START)
            If m_blnUpTurn Then                    '上下反転あり
                nY = picMap.Height - CInt((nInterval * ii) + PICMAP_END)
            End If

            'コイル長によりメモリ描画幅を変更する
            If m_nCoilLen <= 200000 Then           'コイル長200m以下（メモリ間隔20m）
                '20m毎に横線、メモリ描画
                If ii Mod 20000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 1000000 Then       'コイル長1000m以下（メモリ間隔100m）
                '100m毎に横線、メモリ描画
                If ii Mod 100000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 3000000 Then       'コイル長3000m以下（メモリ間隔200m）
                '200m毎に横線、メモリ描画
                If ii Mod 200000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 6000000 Then       'コイル長6000m以下（メモリ間隔500m）
                '500m毎に横線、メモリ描画
                If ii Mod 500000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            ElseIf m_nCoilLen <= 10000000 Then       'コイル長10000m以下（メモリ間隔1000m）
                '1000m毎に横線、メモリ描画
                If ii Mod 1000000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            Else                                     'コイル長10000m超え（メモリ間隔2000m）
                '2000m毎に横線、メモリ描画
                If ii Mod 2000000 = 0 Then
                    e.Graphics.DrawLine(penMemori_2, PICMAP_EDGE, nY, picMap.Width - PICMAP_EDGE, nY)
                    e.Graphics.DrawString(CStr(ii / PICMAP_MMtoM).PadLeft(5), fntMemori, bshMemori, HOSEI_Y, nY - HOSEI_Y)
                End If
            End If

        Next ii

        'エッジ部（対象外部分）実線描画
        '対象外との境界線描画（縦線）
        If m_nCoilWid <> 0 And m_nCoilWid <= m_nDrawRangeWidth Then
            e.Graphics.DrawLine(penEdge, CInt(nPicMapCenterW - nCoilWid), PICMAP_START, CInt(nPicMapCenterW - nCoilWid), picMap.Height - PICMAP_END)   '縦線（左対象外境界線）
            e.Graphics.DrawLine(penEdge, CInt(nPicMapCenterW + nCoilWid), PICMAP_START, CInt(nPicMapCenterW + nCoilWid), picMap.Height - PICMAP_END)   '縦線（右対象外境界線）
        End If

    End Sub

    ''' <summary>
    ''' 疵マップ対象外位置表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <param name="typArray"></param>
    ''' <remarks></remarks>
    Public Sub pinMapCutDrw(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs,
                            ByVal typArray As List(Of SAISUN_DATA))

        Dim rect As System.Drawing.Rectangle
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double

        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth        ' マップ描画幅方向1メモリ長さ
        Dim nCoilWid_Max As Integer = CInt(m_nDrawRangeWidth / 2)
        Dim typCut As SAISUN_DATA
        Dim nY As Integer
        Dim nY2 As Integer


        '長手方向メモリ幅取得
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '全領域塗り潰し設定（全領域対象外）
        rect = New System.Drawing.Rectangle(PICMAP_EDGE, PICMAP_START, CInt(picMap.Width - PICMAP_WIDTH_OUT), picMap.Height - PICMAP_HEIGHT_OUT) '開始位置X:Y、幅、高さ）

        e.Graphics.FillRectangle(Brushes.DarkGray, rect)

        '対象外以外の領域を白で塗り潰し
        For ii = 0 To typArray.Count - 1

            'マーカー用構造体データ取得
            typCut = CType(typArray.Item(ii), SAISUN_DATA)

            'コイル長超えた疵表示用のデータの場合、対象外のままとする。（開始終了の白塗り潰しは行わない）
            'If typCut.cut_flg = 2 Then Continue For

            '長手方向逆転の場合、コイル長から引く
            If m_blnUpTurn Then
                typCut.Start_Point = m_nCoilLen - typCut.Start_Point
                typCut.End_Point = m_nCoilLen - typCut.End_Point
            End If

            '対象外範囲が描画範囲を超える場合は描画範囲内に調整
            If typCut.Start_Point <= 0 Then typCut.Start_Point = 0
            If typCut.Start_Point > m_nCoilLen Then typCut.Start_Point = m_nCoilLen

            '対象外範囲が描画範囲を超える場合は描画範囲内に調整
            If typCut.End_Point <= 0 Then typCut.End_Point = 0
            If typCut.End_Point > m_nCoilLen Then typCut.End_Point = m_nCoilLen

            '長手方向逆転の場合、塗り潰し範囲指定を逆転する。
            If m_blnUpTurn Then
                nY = CInt(typCut.End_Point * nInterval)
                nY2 = CInt((typCut.Start_Point - typCut.End_Point) * nInterval)
            Else
                nY = CInt(typCut.Start_Point * nInterval)
                nY2 = CInt((typCut.End_Point - typCut.Start_Point) * nInterval)
            End If

            rect = New System.Drawing.Rectangle(PICMAP_EDGE, PICMAP_START + nY, CInt(picMap.Width - PICMAP_WIDTH_OUT), nY2) '開始位置X:Y、幅、高さ）
            e.Graphics.FillRectangle(Brushes.White, rect)

        Next ii

    End Sub

    ''' <summary>
    ''' カット位置表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <param name="typArray"></param>
    ''' <remarks></remarks>
    Public Sub picMapCut_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs, _
                                    ByVal typArray As List(Of CUT_DATA))

        Dim cutMemori As New System.Drawing.Pen(Color.Red, 3)
        Dim typCut As CUT_DATA
        Dim ii As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim nY As Integer
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth        ' マップ描画幅方向1メモリ長さ

        '長手方向メモリ幅取得
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        '描画コイル幅、エッジ位置取得（nX座標の範囲を描画枠範囲内に固定）
        For ii = 0 To typArray.Count - 1

            'マーカー用構造体データ取得
            typCut = CType(typArray.Item(ii), CUT_DATA)

            '長手方向逆転の場合、コイル長から引く
            If m_blnUpTurn Then
                typCut.Cut_Point = m_nCoilLen - typCut.Cut_Point
            End If

            'カット位置ありの場合はカット線描画（cut_flg = 1:コイル先端(長手位置0), 2:コイル終端位置以降の場合はカット線描画は行わない）
            If typCut.cut_flg = 0 Then
                'カット線がコイル長範囲外の場合、範囲内に補正
                If 0 > typCut.Cut_Point Then
                    typCut.Cut_Point = 0
                ElseIf typCut.Cut_Point > m_nCoilLen Then
                    typCut.Cut_Point = m_nCoilLen
                End If
                nY = CInt(typCut.Cut_Point * nInterval)             'カット位置長手位置取得
                'カット線描画
                e.Graphics.DrawLine(cutMemori, PICMAP_EDGE, nY + PICMAP_START, picMap.Width - PICMAP_EDGE, nY + PICMAP_START)
            End If

        Next ii

        'マップ描画領域外枠線再描画（塗り潰し処理で上書きされた場合の対応）
        Dim penMemori As New System.Drawing.Pen(Color.Black, 1)
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, PICMAP_START, picMap.Width - PICMAP_EDGE, PICMAP_START)    '上横枠
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, picMap.Height - PICMAP_END, picMap.Width - PICMAP_EDGE, picMap.Height - PICMAP_END)   '下横枠
        e.Graphics.DrawLine(penMemori, PICMAP_EDGE, PICMAP_START, PICMAP_EDGE, picMap.Height - PICMAP_END)   '縦枠線（左）
        e.Graphics.DrawLine(penMemori, picMap.Width - PICMAP_EDGE, PICMAP_START, picMap.Width - PICMAP_EDGE, picMap.Height - PICMAP_END)   '縦枠線（右）

    End Sub

    ''' <summary>
    ''' 疵マップデータをペイントイベントでマーク表示(探傷画面、実績画面共通)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Sub picMapMarker_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        Dim fntMemori As New System.Drawing.Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim fntSymbol As New System.Drawing.Font("ＭＳ ゴシック", 10, System.Drawing.FontStyle.Regular, GraphicsUnit.Pixel)

        Dim bshMemori As New SolidBrush(Color.Black)
        Dim strSymbol As String = ""
        Dim ii As Integer
        Dim drwColor As Color
        Dim typMapdata As MAP_DATA
        Dim nCoilWid As Integer
        Dim nCoilStart As Integer
        Dim nX As Integer
        Dim nY As Integer
        Dim picMap As PictureBox = CType(sender, PictureBox)
        Dim nInterval As Double
        Dim dPicMapMemWid As Double = (picMap.Width - PICMAP_WIDTH_OUT) / m_nDrawRangeWidth ' マップ描画幅方向1メモリ長さ
        Dim nPicMapCenterW As Integer = CInt(picMap.Width / 2)                              ' マップ描画領域幅中心位置
        Dim dPocMapCenterX As Double

        '長手方向メモリ幅取得
        If m_nCoilLen = 0 Then
            nInterval = 1
        Else
            nInterval = CDbl((picMap.Height - PICMAP_HEIGHT_OUT) / m_nCoilLen)
        End If

        'コイル幅、エッジ位置取得
        nCoilWid = m_nCoilWid
        If nCoilWid <= 0 Or nCoilWid > COIL_WID_MAX Then
            'コイル幅が0、または最大値を超える場合最大値をセット
            nCoilWid = COIL_WID_MAX
        End If
        nCoilStart = CInt(nCoilWid / 2 * dPicMapMemWid)
        dPocMapCenterX = nCoilWid / 2.0R

        '疵情報取得中
        If m_nMode = 0 Then
            Call DspWaitInf("疵マップ表示中（表面）", m_DefMapData.Count)
        Else
            Call DspWaitInf("疵マップ表示中（裏面）", m_DefMapData.Count)
        End If

        For ii = 0 To m_DefMapData.Count - 1
            '描画色初期化
            bshMemori.Dispose()

            'マーカー用構造体データ取得
            typMapdata = CType(m_DefMapData.Item(ii), MAP_DATA)
            drwColor = tMod.GetIntToColor(mcls_Param.GetMapSymbolColor(typMapdata.g_id))      'グレードシンボル表示色

            'シンボル取得
            strSymbol = mcls_Param.GetMapSymbol(typMapdata.t_id)

            'マーカー表示色設定
            bshMemori = New SolidBrush(drwColor)

            '座標計算
            '幅方向
            If m_blnSideTurn Then
                '左右反転あり時

                'マップ表示範囲外の場合、枠内に収める
                nX = CInt((nPicMapCenterW + nCoilStart) - (dPocMapCenterX + typMapdata.absX) * dPicMapMemWid)
                If nX < PICMAP_EDGE Then
                    nX = PICMAP_EDGE
                ElseIf nX > picMap.Width - PICMAP_EDGE Then
                    nX = picMap.Width - PICMAP_EDGE
                End If
            Else
                '左右反転無し時

                'マップ表示範囲外の場合、枠内に収める
                nX = CInt((nPicMapCenterW - nCoilStart) + ((dPocMapCenterX + typMapdata.absX) * dPicMapMemWid))
                If nX < PICMAP_EDGE Then
                    nX = PICMAP_EDGE
                ElseIf nX > picMap.Width - PICMAP_EDGE Then
                    nX = picMap.Width - PICMAP_EDGE
                End If
            End If

            '長手方向
            '長手方向逆転の場合、コイル長から引く
            If m_blnUpTurn Then typMapdata.y = m_nCoilLen - typMapdata.y
            nY = PICMAP_START + CInt(typMapdata.y * nInterval)
            'マップ表示範囲外の場合、枠内に収める
            If nY < PICMAP_START Then
                nY = PICMAP_START
            ElseIf nY > (picMap.Height - PICMAP_END) Then
                nY = picMap.Height - PICMAP_END
            End If

            'マーカー描画
            e.Graphics.DrawString(strSymbol, fntSymbol, bshMemori, nX - PICMAP_HOSEI_X, nY - HOSEI_Y)

            '状況表示
            Call DspWaitProg(ii + 1)

        Next ii

    End Sub

    ''' <summary>
    ''' データ取得状況表示中
    ''' </summary>
    ''' <param name="strVal"></param>
    ''' <param name="iMax"></param>
    ''' <remarks></remarks>
    Public Sub DspWaitInf(ByVal strVal As String, ByVal iMax As Long)
        m_typDsp.lblMsg.Text = strVal
        m_typDsp.prbMsg.Minimum = 0
        m_typDsp.prbMsg.Maximum = CInt(IIf(iMax > m_typDsp.prbMsg.Minimum, iMax, m_typDsp.prbMsg.Minimum + 1))
        m_typDsp.prbMsg.Value = 0

        'Application.DoEvents()は使用できない為リフレッシュで表示
        m_typDsp.lblMsg.Refresh()
        m_typDsp.prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' データ取得中ガイダンス 更新
    ''' </summary>
    ''' <param name="nVal">進捗値</param>
    ''' <remarks></remarks>
    Public Sub DspWaitProg(ByVal nVal As Integer)

        '進捗値セット
        m_typDsp.prbMsg.Value = CInt(IIf(nVal < m_typDsp.prbMsg.Maximum, nVal, m_typDsp.prbMsg.Maximum))

    End Sub

End Class
