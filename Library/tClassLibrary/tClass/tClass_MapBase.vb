'*********************************************************************************
' グラフタイプの疵マップ基底クラス
'	[Ver]
'		Ver.01    2009/11/27  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Drawing
Imports System.Windows.Forms

Namespace tClass
    Public Class tClass_MapBase
        Implements IDisposable							' デストラクタ定義


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開構造体
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 上下左右の配列位置
        ''' </summary>
        Public Enum EM_INDEX_ITI
            emTop = 0
            emLeft
            emBottom
            emRight		'配列の最後として使用しているので注意
        End Enum
        ''' <summary>
        ''' XYの配列位置
        ''' </summary>
        Public Enum EM_INDEX_XY
            emX = 0
            emY			'配列の最後として使用しているので注意
        End Enum

        ''' <summary>
        ''' 実際の値で使用
        ''' </summary>
        Public Structure xyValue
            ''' <summary>X値[mm]</summary>
            Dim X As Double
            ''' <summary>Y値[mm]</summary>
            Dim Y As Double
            ''' <summary>コンストラクタ</summary>
            Shared Sub New()
            End Sub
            ''' <summary>コンストラクタ</summary>
            ''' <param name="xVal">X値</param>
            ''' <param name="yVal">Y値</param>
            ''' <remarks></remarks>
            Public Sub New(ByVal xVal As Double, ByVal yVal As Double)
                X = xVal
                Y = yVal
            End Sub
        End Structure

        ''' <summary>
        ''' グラフ座標の値で使用
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure xyPoint
            ''' <summary>X値[pixel]</summary>
            Dim X As Integer
            ''' <summary>Y値[pixel]</summary>
            Dim Y As Integer
            ''' <summary>コンストラクタ</summary>
            Shared Sub New()
            End Sub
            ''' <summary>コンストラクタ</summary>
            ''' <param name="xVal">X値[Pixel]</param>
            ''' <param name="yVal">Y値[Pixel]</param>
            ''' <remarks></remarks>
            Public Sub New(ByVal xVal As Integer, ByVal yVal As Integer)
                X = xVal
                Y = yVal
            End Sub
        End Structure

        ''' <summary>倍率</summary>
        Public Structure RATE
            ''' <summary>X横倍率</summary>
            Public X As Double
            ''' <summary>Y縦倍率</summary>
            Public Y As Double

            ''' <summary>初期化</summary>
            Public Sub Initialize()
                X = 1.0
                Y = 1.0
            End Sub
            Public Sub New(ByVal xVal As Double, ByVal yVal As Double)
                X = xVal
                Y = yVal
            End Sub
        End Structure

        ''' <summary>表示領域情報</summary>
        Public Structure ARIA_INF
            ''' <summary>下地とマップの境界線</summary>
            Dim BackGroundLineColor As Pen
            ''' <summary>下地の色</summary>
            Dim BackGroundColor As SolidBrush 'Brush
            ''' <summary>マップ内の色</summary>
            Dim MapColor As SolidBrush 'Brush


            ''' <summary>表示領域 (=コントロールサイズ)</summary>
            Dim AriaSize As Size
            ''' <summary>描画エリアの余白</summary>
            Dim Margin As Padding
        End Structure


        ''' <summary>マップ領域情報</summary>
        Public Structure MAP_INF
            ''' <summary>X軸センター基準？(true時センター基準。false時左上基準)</summary>
            Dim bIsCenterX As Boolean
            ''' <summary>Y軸センター基準？(true時センター基準。false時左上基準)</summary>
            Dim bIsCenterY As Boolean

            ''' <summary>X軸基準(true時左基準。false時右基準)(bIsCenterX=false時のみ有効)</summary>
            Dim bIsBaseX_L As Boolean
            ''' <summary>Y軸基準(true時上基準。false時下基準)(bIsCenterY=false時のみ有効)</summary>
            Dim bIsBaseY_T As Boolean

            ''' <summary>X軸の最小最大をマップの端にあわせる場合true</summary> 
            Dim bIsStrechX As Boolean
            ''' <summary>Y軸の最小最大をマップの端にあわせる場合true</summary> 
            Dim bIsStrechY As Boolean
            ''' <summary>X軸最小[mm]</summary> 
            Dim nLimitXmin As Integer
            ''' <summary>X軸最大[mm]</summary> 
            Dim nLimitXmax As Integer
            ''' <summary>Y軸最小[mm]</summary> 
            Dim nLimitYmin As Integer
            ''' <summary>Y軸最大[mm]</summary> 
            Dim nLimitYmax As Integer

            ''' <summary>オフセット位置 (左上pixel位置の値をセット) (bIsCenterで動作が変わる。falseのみ有効。true時は自動算出)</summary>
            Dim Offset As xyValue
        End Structure

        ''' <summary>対象範囲領域情報</summary>
        Public Structure LIMIT_INF
            ''' <summary>対象範囲外の描画する場合true</summary>
            Dim bLimitDraw As Boolean

            ''' <summary>マップと対象範囲の境界線</summary>
            Dim LimitOutLineColor As Pen
            ''' <summary>マップ内の対象範囲対象外の色</summary>
            Dim LimitOutColor As SolidBrush 'Brush

            ''' <summary>対象範囲X軸最小[mm]</summary> 
            Dim nLimitXmin As Integer
            ''' <summary>対象範囲X軸最大[mm]</summary> 
            Dim nLimitXmax As Integer
            ''' <summary>対象範囲Y軸最小[mm]</summary> 
            Dim nLimitYmin As Integer
            ''' <summary>対象範囲Y軸最大[mm]</summary> 
            Dim nLimitYmax As Integer
        End Structure


        ''' <summary>メモリ設定情報</summary>
        Public Structure MEMORI_INF

            ''' <summary>メモリ線形式</summary>
            Dim LinePen As Pen
            ''' <summary>メモリ文字形式</summary>
            Dim StringFont As Font
            ''' <summary>メモリ文字形式</summary>
            Dim StringBsh As SolidBrush


            ''' <summary>X軸の文字表示位置オフセット(X軸から何ピクセル離す)</summary>
            Dim nPosStringX As Integer
            ''' <summary>Y軸の文字表示位置オフセット(Y軸から何ピクセル離す)</summary>
            Dim nPosStringY As Integer

            ''' <summary>メモリ間隔 [mm]</summary>
            Dim ScaleInterval As xyValue
            ''' <summary>目盛りフォーマット("0mm"とか"#,##0m")</summary>
            Dim sScaleStringFormatX As String
            Dim sScaleStringFormatY As String
            ''' <summary>[mm]基準の変換レート(mにしたい場合は、1000をセットしてsScaleStringFormatYにmを入れる)</summary>
            Dim ScaleStringRange As RATE

            ''' <summary>目盛り文字の表示非表示 (True:表示 False:非表示)</summary>
            Dim bStringDisp_T As Boolean
            Dim bStringDisp_B As Boolean
            Dim bStringDisp_L As Boolean
            Dim bStringDisp_R As Boolean
            ''' <summary>メモリ線表示有無 (True:表示 False:非表示)</summary>
            Dim bLineDsp_X As Boolean
            Dim bLineDsp_Y As Boolean
        End Structure



        ''' <summary>座標最終結果</summary>
        Public Structure CALC_ANS
            ''' <summary>最終分解能[mm/pixel] (全長表示時は、自動計算。)</summary>
            Public FinalRes As RATE

            ''' <summary>マップ描画領域[pixel] (=表示領域-余白) (計算で算出)</summary>
            Dim MapRect As Rectangle

            ''' <summary>表示している最初値[mm] (左上の値)</summary>
            Public DspStartVal As xyValue
            ''' <summary>表示している最終値[mm] (右下の値)</summary>
            Public DspEndVal As xyValue
        End Structure


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private disposedValue As Boolean = False			' 重複する呼び出しを検出するには

        ''' <summary>描画結果ビットマップ</summary>
        Protected m_Bmp As Drawing.Bitmap = Nothing			' 描画結果ビットマップ
        '''' <summary>プロパティー計算フラグ (True:計算する False:一時中断)</summary>
        'Protected m_PropLayout As Boolean					' プロパティー計算フラグ (True:計算する False:一時中断)

        ''' <summary>表示領域情報</summary>
        Protected m_AriaInf As ARIA_INF						' 表示領域情報
        ''' <summary>マップ領域情報</summary>
        Protected m_MapInf As MAP_INF						' マップ領域情報
        ''' <summary>対象範囲領域情報</summary>
        Protected m_LimitInf As LIMIT_INF					' 対象範囲領域情報
        ''' <summary>メモリ設定情報</summary>
        Protected m_MemoriInf As MEMORI_INF					' メモリ設定情報
        ''' <summary>座標最終結果</summary>
        Protected m_CalcAns As CALC_ANS						' 座標最終結果


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 生成したビットマップ
        ''' </summary>
        Public ReadOnly Property tProp_Bmp() As Drawing.Bitmap
            Get
                Return m_Bmp
            End Get
        End Property


#Region "表示領域情報"
        ''' <summary>
        ''' 表示領域情報
        ''' </summary>
        Public ReadOnly Property tProp_AriaInf() As ARIA_INF
            Get
                Return m_AriaInf
            End Get
        End Property


        ''' <summary>
        ''' 下地とマップの境界線
        ''' </summary>
        Public Property tProp_AriaInf_BackGroundLineColor() As Pen
            Get
                Return m_AriaInf.BackGroundLineColor
            End Get
            Set(ByVal value As Pen)
                m_AriaInf.BackGroundLineColor = value
            End Set
        End Property
        ''' <summary>
        ''' 下地の色
        ''' </summary>
        Public Property tProp_AriaInf_BackGroundColor() As SolidBrush
            Get
                Return m_AriaInf.BackGroundColor
            End Get
            Set(ByVal value As SolidBrush)
                m_AriaInf.BackGroundColor = value
            End Set
        End Property
        ''' <summary>
        ''' マップ内のコイル部の色
        ''' </summary>
        Public Property tProp_AriaInf_MapColor() As SolidBrush
            Get
                Return m_AriaInf.MapColor
            End Get
            Set(ByVal value As SolidBrush)
                m_AriaInf.MapColor = value
            End Set
        End Property
        ''' <summary>
        ''' 表示領域 (=コントロールサイズ)
        ''' </summary>
        Public Property tProp_AriaInf_AriaSize() As Size
            Get
                Return m_AriaInf.AriaSize
            End Get
            Set(ByVal value As Size)
                m_AriaInf.AriaSize = value

                ' マップ描画領域の算出
                Call ExecAriaInf_MapSize()
            End Set
        End Property
        ''' <summary>
        ''' 描画エリアの余白
        ''' </summary>
        Public Property tProp_AriaInf_Margin() As Padding
            Get
                Return m_AriaInf.Margin
            End Get
            Set(ByVal value As Padding)
                m_AriaInf.Margin = value

                ' マップ描画領域の算出
                Call ExecAriaInf_MapSize()
            End Set
        End Property
#End Region


#Region "マップ領域情報"
        ''' <summary>
        ''' マップ領域情報
        ''' </summary>
        Public ReadOnly Property tProp_MapInf() As MAP_INF
            Get
                Return m_MapInf
            End Get
        End Property


        ''' <summary>
        ''' X軸センター基準？(true時センター基準。false時左上基準)
        ''' </summary>
        Public Property tProp_MapInf_IsCenterX() As Boolean
            Get
                Return m_MapInf.bIsCenterX
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsCenterX = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸センター基準？(true時センター基準。false時左上基準)
        ''' </summary>
        Public Property tProp_MapInf_IsCenterY() As Boolean
            Get
                Return m_MapInf.bIsCenterY
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsCenterY = value
            End Set
        End Property


        ''' <summary>
        ''' X軸基準(true時左基準。false時右基準)(bIsCenterX=false時のみ有効)
        ''' </summary>
        Public Property tProp_MapInf_IsBaseX_L() As Boolean
            Get
                Return m_MapInf.bIsBaseX_L
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsBaseX_L = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸基準(true時上基準。false時下基準)(bIsCenterY=false時のみ有効)
        ''' </summary>
        Public Property tProp_MapInf_IsBaseY_T() As Boolean
            Get
                Return m_MapInf.bIsBaseY_T
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsBaseY_T = value
            End Set
        End Property

        ''' <summary>
        ''' X軸の最小最大をマップの端にあわせる場合true
        ''' </summary>
        Public Property tProp_MapInf_IsStrechX() As Boolean
            Get
                Return m_MapInf.bIsStrechX
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsStrechX = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸の最小最大をマップの端にあわせる場合true
        ''' </summary>
        Public Property tProp_MapInf_IsStrechY() As Boolean
            Get
                Return m_MapInf.bIsStrechY
            End Get
            Set(ByVal value As Boolean)
                m_MapInf.bIsStrechY = value
            End Set
        End Property

        ''' <summary>
        ''' X軸最小[mm]
        ''' </summary>
        Public Property tProp_MapInf_LimitXmin() As Integer
            Get
                Return m_MapInf.nLimitXmin
            End Get
            Set(ByVal value As Integer)
                m_MapInf.nLimitXmin = value
            End Set
        End Property
        ''' <summary>
        ''' X軸最大[mm]
        ''' </summary>
        Public Property tProp_MapInf_LimitXmax() As Integer
            Get
                Return m_MapInf.nLimitXmax
            End Get
            Set(ByVal value As Integer)
                m_MapInf.nLimitXmax = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸最小[mm]
        ''' </summary>
        Public Property tProp_MapInf_LimitYmin() As Integer
            Get
                Return m_MapInf.nLimitYmin
            End Get
            Set(ByVal value As Integer)
                m_MapInf.nLimitYmin = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸最大[mm]
        ''' </summary>
        Public Property tProp_MapInf_LimitYmax() As Integer
            Get
                Return m_MapInf.nLimitYmax
            End Get
            Set(ByVal value As Integer)
                m_MapInf.nLimitYmax = value
            End Set
        End Property
        ''' <summary>
        ''' オフセット位置 (左上pixel位置の値をセット) (bIsCenterで動作が変わる。falseのみ有効。true時は自動算出)
        ''' </summary>
        Public Property tProp_MapInf_Offset() As xyValue
            Get
                Return m_MapInf.Offset
            End Get
            Set(ByVal value As xyValue)
                m_MapInf.Offset = value
            End Set
        End Property
#End Region

#Region "対象範囲領域情報"
        ''' <summary>
        ''' 対象範囲領域情報
        ''' </summary>
        Public ReadOnly Property tProp_LimitInf() As LIMIT_INF
            Get
                Return m_LimitInf
            End Get
        End Property
        ''' <summary>
        ''' マップと対象範囲の境界線
        ''' </summary>
        Public Property tProp_LimitInf_LimitDraw() As Boolean
            Get
                Return m_LimitInf.bLimitDraw
            End Get
            Set(ByVal value As Boolean)
                m_LimitInf.bLimitDraw = value
            End Set
        End Property
        ''' <summary>
        ''' マップと対象範囲の境界線
        ''' </summary>
        Public Property tProp_LimitInf_LimitOutLineColor() As Pen
            Get
                Return m_LimitInf.LimitOutLineColor
            End Get
            Set(ByVal value As Pen)
                m_LimitInf.LimitOutLineColor = value
            End Set
        End Property
        ''' <summary>
        ''' マップ内の対象範囲対象外の色
        ''' </summary>
        Public Property tProp_LimitInf_LimitOutColor() As SolidBrush
            Get
                Return m_LimitInf.LimitOutColor
            End Get
            Set(ByVal value As SolidBrush)
                m_LimitInf.LimitOutColor = value
            End Set
        End Property


        ''' <summary>
        ''' 対象範囲X軸最小[mm]
        ''' </summary>
        Public Property tProp_LimitInf_LimitXmin() As Integer
            Get
                Return m_LimitInf.nLimitXmin
            End Get
            Set(ByVal value As Integer)
                m_LimitInf.nLimitXmin = value
            End Set
        End Property
        ''' <summary>
        ''' 対象範囲X軸最大[mm]
        ''' </summary>
        Public Property tProp_LimitInf_LimitXmax() As Integer
            Get
                Return m_LimitInf.nLimitXmax
            End Get
            Set(ByVal value As Integer)
                m_LimitInf.nLimitXmax = value
            End Set
        End Property
        ''' <summary>
        ''' 対象範囲Y軸最小[mm]
        ''' </summary>
        Public Property tProp_LimitInf_LimitYmin() As Integer
            Get
                Return m_LimitInf.nLimitYmin
            End Get
            Set(ByVal value As Integer)
                m_LimitInf.nLimitYmin = value
            End Set
        End Property
        ''' <summary>
        ''' 対象範囲Y軸最大[mm]
        ''' </summary>
        Public Property tProp_LimitInf_LimitYmax() As Integer
            Get
                Return m_LimitInf.nLimitYmax
            End Get
            Set(ByVal value As Integer)
                m_LimitInf.nLimitYmax = value
            End Set
        End Property
#End Region

#Region "メモリ設定情報"
        ''' <summary>
        ''' メモリ設定情報
        ''' </summary>
        Public ReadOnly Property tProp_MemoriInf() As MEMORI_INF
            Get
                Return m_MemoriInf
            End Get
        End Property

        ''' <summary>
        ''' メモリ線形式
        ''' </summary>
        Public Property tProp_MemoriInf_LinePen() As Pen
            Get
                Return m_MemoriInf.LinePen
            End Get
            Set(ByVal value As Pen)
                m_MemoriInf.LinePen = value
            End Set
        End Property
        ''' <summary>
        ''' メモリ文字形式
        ''' </summary>
        Public Property tProp_MemoriInf_StringFont() As Font
            Get
                Return m_MemoriInf.StringFont
            End Get
            Set(ByVal value As Font)
                m_MemoriInf.StringFont = value
            End Set
        End Property
        ''' <summary>
        ''' メモリ文字形式
        ''' </summary>
        Public Property tProp_MemoriInf_StringBsh() As SolidBrush
            Get
                Return m_MemoriInf.StringBsh
            End Get
            Set(ByVal value As SolidBrush)
                m_MemoriInf.StringBsh = value
            End Set
        End Property
        ''' <summary>
        ''' X軸の文字表示位置オフセット(X軸から何ピクセル離す)
        ''' </summary>
        Public Property tProp_MemoriInf_PosStringX() As Integer
            Get
                Return m_MemoriInf.nPosStringX
            End Get
            Set(ByVal value As Integer)
                m_MemoriInf.nPosStringX = value
            End Set
        End Property
        ''' <summary>
        ''' Y軸の文字表示位置オフセット(Y軸から何ピクセル離す)
        ''' </summary>
        Public Property tProp_MemoriInf_PosStringY() As Integer
            Get
                Return m_MemoriInf.nPosStringY
            End Get
            Set(ByVal value As Integer)
                m_MemoriInf.nPosStringY = value
            End Set
        End Property

        ''' <summary>
        ''' メモリ間隔 [mm]
        ''' </summary>
        Public Property tProp_MemoriInf_ScaleInterval() As xyValue
            Get
                Return m_MemoriInf.ScaleInterval
            End Get
            Set(ByVal value As xyValue)
                m_MemoriInf.ScaleInterval = value
            End Set
        End Property
        ''' <summary>
        ''' 目盛りフォーマット("0mm"とか"#,##0m")
        ''' </summary>
        Public Property tProp_MemoriInf_ScaleStringFormatX() As String
            Get
                Return m_MemoriInf.sScaleStringFormatX
            End Get
            Set(ByVal value As String)
                m_MemoriInf.sScaleStringFormatX = value
            End Set
        End Property
        ''' <summary>
        ''' 目盛りフォーマット("0mm"とか"#,##0m")
        ''' </summary>
        Public Property tProp_MemoriInf_ScaleStringFormatY() As String
            Get
                Return m_MemoriInf.sScaleStringFormatY
            End Get
            Set(ByVal value As String)
                m_MemoriInf.sScaleStringFormatY = value
            End Set
        End Property
        ''' <summary>
        ''' [mm]基準の変換レート(mにしたい場合は、1000をセットしてsScaleStringFormatYにmを入れる)
        ''' </summary>
        Public Property tProp_MemoriInf_ScaleStringRange() As RATE
            Get
                Return m_MemoriInf.ScaleStringRange
            End Get
            Set(ByVal value As RATE)
                m_MemoriInf.ScaleStringRange = value
            End Set
        End Property

        ''' <summary>
        ''' 目盛り文字の表示非表示 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_StringDisp_T() As Boolean
            Get
                Return m_MemoriInf.bStringDisp_T
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bStringDisp_T = value
            End Set
        End Property
        ''' <summary>
        ''' 目盛り文字の表示非表示 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_StringDisp_B() As Boolean
            Get
                Return m_MemoriInf.bStringDisp_B
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bStringDisp_B = value
            End Set
        End Property
        ''' <summary>
        ''' 目盛り文字の表示非表示 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_StringDisp_L() As Boolean
            Get
                Return m_MemoriInf.bStringDisp_L
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bStringDisp_L = value
            End Set
        End Property
        ''' <summary>
        ''' 目盛り文字の表示非表示 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_StringDisp_R() As Boolean
            Get
                Return m_MemoriInf.bStringDisp_R
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bStringDisp_R = value
            End Set
        End Property

        ''' <summary>
        ''' メモリ線表示有無 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_LineDsp_X() As Boolean
            Get
                Return m_MemoriInf.bLineDsp_X
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bLineDsp_X = value
            End Set
        End Property
        ''' <summary>
        ''' メモリ線表示有無 (True:表示 False:非表示)
        ''' </summary>
        Public Property tProp_MemoriInf_LineDsp_Y() As Boolean
            Get
                Return m_MemoriInf.bLineDsp_Y
            End Get
            Set(ByVal value As Boolean)
                m_MemoriInf.bLineDsp_Y = value
            End Set
        End Property
#End Region


#Region "座標最終結果"
        ''' <summary>
        ''' 座標最終結果
        ''' </summary>
        Public ReadOnly Property tProp_CalcAns() As CALC_ANS
            Get
                Return m_CalcAns
            End Get
        End Property
#End Region





        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部描画用
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' ベースクラスの全描画処理の描画範囲描画後
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Protected Overridable Sub DrawPaint_BackGround(ByVal g As Graphics)

        End Sub
        ''' <summary>
        ''' ベースクラスの全描画処理のメモリ線描画後
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Protected Overridable Sub DrawPaint_Memori(ByVal g As Graphics)

        End Sub
        ''' <summary>
        ''' ベースクラスの全描画処理の完了後
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Protected Overridable Sub DrawPaint_End(ByVal g As Graphics)

        End Sub

        ''' <summary>
        ''' ユーザー描画 ベースクラスの全描画処理の描画範囲描画後
        ''' </summary>
        ''' <param name="cls">自分自身のオブジェクト</param>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Event tEv_DrawPaint_BackGround(ByVal cls As tClass_MapBase, ByVal g As Graphics)
        ''' <summary>
        ''' ユーザー描画 ベースクラスの全描画処理のメモリ線描画後
        ''' </summary>
        ''' <param name="cls">自分自身のオブジェクト</param>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Event tEv_DrawPaint_Memori(ByVal cls As tClass_MapBase, ByVal g As Graphics)
        ''' <summary>
        ''' ユーザー描画 ベースクラスの全描画処理の完了後
        ''' </summary>
        ''' <param name="cls">自分自身のオブジェクト</param>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Event tEv_DrawPaint_End(ByVal cls As tClass_MapBase, ByVal g As Graphics)




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 標準モジュール
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ピクチャー左上ピクセル値からグラフ値に変換
        ''' </summary>
        ''' <param name="X">[Pixel]</param>
        ''' <param name="Y">[Pixel]</param>
        ''' <returns>値</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetPtoV(ByVal X As Integer, ByVal Y As Integer) As xyValue
            Return New xyValue(GetPtoV_X(X), GetPtoV_Y(Y))
        End Function
        ''' <param name="p">[Pixel]</param>
        ''' <returns>値</returns>
        Public Overloads Function GetPtoV(ByVal p As xyPoint) As xyValue
            Return New xyValue(GetPtoV_X(p.X), GetPtoV_Y(p.Y))
        End Function
        ''' <param name="X">X軸[Pixel]</param>
        ''' <returns>X値</returns>
        Public Function GetPtoV_X(ByVal X As Integer) As Double
            Dim wk As Integer

            If m_MapInf.bIsBaseX_L Then
                wk = X - m_CalcAns.MapRect.Left		'表示エリアからの相対距離で計算
                Return m_CalcAns.DspStartVal.X + (wk * m_CalcAns.FinalRes.X)
            Else
                wk = m_CalcAns.MapRect.Right - X		'表示エリアからの相対距離で計算
                Return m_CalcAns.DspStartVal.X - (wk * m_CalcAns.FinalRes.X)
            End If
        End Function
        ''' <param name="Y">Y軸[Pixel]</param>
        ''' <returns>Y値</returns>
        Public Function GetPtoV_Y(ByVal Y As Integer) As Double
            Dim wk As Integer

            If m_MapInf.bIsBaseY_T Then
                wk = Y - m_CalcAns.MapRect.Top		'表示エリアからの相対距離で計算
                Return m_CalcAns.DspStartVal.Y + (wk * m_CalcAns.FinalRes.Y)
            Else
                wk = m_CalcAns.MapRect.Bottom - Y		 '表示エリアからの相対距離で計算
                Return m_CalcAns.DspStartVal.Y + (wk * m_CalcAns.FinalRes.Y)
            End If
        End Function

        ''' <summary>
        ''' グラフ値からピクチャー左上ピクセル値に変換
        ''' </summary>
        ''' <param name="X"></param>
        ''' <param name="Y"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetVtoP(ByVal X As Double, ByVal Y As Double) As xyPoint
            Return New xyPoint(GetVtoP_X(X), GetVtoP_Y(Y))
        End Function
        Public Overloads Function GetVtoP(ByVal v As xyValue) As xyPoint
            Return New xyPoint(GetVtoP_X(v.X), GetVtoP_Y(v.Y))
        End Function
        Public Overloads Function GetVtoP_X(ByVal X As Double) As Integer
            Dim wk As Double
            wk = X - m_CalcAns.DspStartVal.X
            If m_MapInf.bIsBaseX_L Then
                Return CInt(m_CalcAns.MapRect.Left + Int(wk / m_CalcAns.FinalRes.X))
            Else
                Return CInt(m_CalcAns.MapRect.Right - Int(wk / m_CalcAns.FinalRes.X))
            End If
        End Function
        Public Overloads Function GetVtoP_Y(ByVal Y As Double) As Integer
            Dim wk As Double
            wk = Y - m_CalcAns.DspStartVal.Y
            If m_MapInf.bIsBaseY_T Then
                Return CInt(m_CalcAns.MapRect.Top + Int(wk / m_CalcAns.FinalRes.Y))
            Else
                Return CInt(m_CalcAns.MapRect.Bottom - Int(wk / m_CalcAns.FinalRes.Y))
            End If
        End Function

        ''' <summary>
        ''' 文字列を描画する
        ''' </summary>
        ''' <param name="g">グラフィック</param>
        ''' <param name="x">基準X位置</param>
        ''' <param name="y">基準Y位置</param>
        ''' <param name="xSa">計算した描画位置から さらに移動</param>
        ''' <param name="ySa">計算した描画位置から さらに移動</param>
        ''' <param name="str">描画文字列</param>
        ''' <param name="iti">描画位置</param>
        ''' <param name="f"></param>
        ''' <param name="b"></param>
        ''' <remarks></remarks>
        Public Sub DrawString(ByVal g As Graphics, ByVal x As Integer, ByVal y As Integer, ByVal xSa As Integer, ByVal ySa As Integer, ByVal str As String, ByVal iti As Drawing.ContentAlignment, ByVal f As Font, ByVal b As SolidBrush)
            '' 文字サイズ取得
            Dim txtSize As SizeF = g.MeasureString(str, f)

            '' 位置計算
            If iti = ContentAlignment.BottomRight Then	'これが基準とする
                x = x
                y = y
            ElseIf iti = ContentAlignment.BottomCenter Then
                x = CInt(x - txtSize.Width / 2)
                y = y
            ElseIf iti = ContentAlignment.BottomLeft Then
                x = CInt(x - txtSize.Width)
                y = y
            ElseIf iti = ContentAlignment.MiddleRight Then
                x = x
                y = CInt(y - txtSize.Height / 2)
            ElseIf iti = ContentAlignment.MiddleCenter Then
                x = CInt(x - txtSize.Width / 2)
                y = CInt(y - txtSize.Height / 2)
            ElseIf iti = ContentAlignment.MiddleLeft Then
                x = CInt(x - txtSize.Width)
                y = CInt(y - txtSize.Height / 2)
            ElseIf iti = ContentAlignment.TopRight Then
                x = x
                y = CInt(y - txtSize.Height)
            ElseIf iti = ContentAlignment.TopCenter Then
                x = CInt(x - txtSize.Width / 2)
                y = CInt(y - txtSize.Height)
            ElseIf iti = ContentAlignment.TopLeft Then
                x = CInt(x - txtSize.Width)
                y = CInt(y - txtSize.Height)
            End If

            '' 文字描画
            g.DrawString(str, f, b, x + xSa, y + ySa)
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 基本処理
        '/ /////////////////////////////////////////////////////////////////////////////////

#Region " IDisposable Support "
        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 他の状態を解放します (マネージ オブジェクト)。
                End If

                ' TODO: ユーザー独自の状態を解放します (アンマネージ オブジェクト)。
                ' TODO: 大きなフィールドを null に設定します。
                If Not m_Bmp Is Nothing Then
                    m_Bmp.Dispose()
                    m_Bmp = Nothing
                End If
            End If
            Me.disposedValue = True
        End Sub


        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '''' 必須初期値
            '' 表示領域関係
            m_AriaInf.BackGroundLineColor = New Pen(Color.Black, 1)
            m_AriaInf.BackGroundColor = New SolidBrush(Color.LightGray)
            m_AriaInf.MapColor = New SolidBrush(Color.White)
            '' マップ領域関係
            m_MapInf.bIsBaseX_L = True
            m_MapInf.bIsBaseY_T = True
            m_MapInf.Offset.X = 0
            m_MapInf.Offset.Y = 0
            '' マップ対象外関係
            m_LimitInf.LimitOutLineColor = New Pen(Color.Black, 1)
            m_LimitInf.LimitOutColor = New SolidBrush(Color.LightGray)
            '' メモリ関係
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            m_MemoriInf.LinePen = p
            m_MemoriInf.StringFont = New Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            m_MemoriInf.StringBsh = New SolidBrush(Color.Black)
            m_MemoriInf.ScaleInterval.X = 1
            m_MemoriInf.ScaleInterval.Y = 1
            m_MemoriInf.sScaleStringFormatX = "0"
            m_MemoriInf.sScaleStringFormatY = "0"
            m_MemoriInf.ScaleStringRange.X = 1
            m_MemoriInf.ScaleStringRange.Y = 1
            '' 結果関係
            m_CalcAns.FinalRes.X = 1
            m_CalcAns.FinalRes.Y = 1


            ''''' そこそこ使用出来るように初期値をセット
            ' '' 表示領域情報
            'm_AriaInf.BackGroundLineColor = New Pen(Color.Black, 1)
            'm_AriaInf.BackGroundColor = Brushes.LightGray
            ''m_AriaInf.MapInColor = Brushes.White
            ''m_AriaInf.MapOutColor = Brushes.Gray

            'm_MapInf.bIsCenterX = True
            'm_MapInf.bIsCenterY = True
            'm_MapInf.bIsStrechX = True
            'm_MapInf.bIsStrechY = True
            'm_MapInf.nLimitXmin = -1000
            'm_MapInf.nLimitXmax = 1000
            'm_MapInf.nLimitYmin = -1000 * 1000
            'm_MapInf.nLimitYmax = 1000 * 1000
            'm_MapInf.Offset.X = 160
            'm_MapInf.Offset.Y = 0

            ' '' メモリ設定情報
            'm_MemoriInf.ScaleInterval.X = 200
            'm_MemoriInf.ScaleInterval.Y = 30000
            'm_MemoriInf.sScaleStringFormatX = "0mm"
            'm_MemoriInf.sScaleStringFormatY = "#,##0m"
            ''m_MemoriInf.nScaleStringRangeX = 10
            ''m_MemoriInf.nScaleStringRangeY = 1000
            'm_MemoriInf.bStringDisp_T = True
            'm_MemoriInf.bStringDisp_B = True
            'm_MemoriInf.bStringDisp_L = True
            'm_MemoriInf.bStringDisp_R = True
            'm_MemoriInf.bLineDsp_X = True
            'm_MemoriInf.bLineDsp_Y = True
            'm_MemoriInf.nPosStringX = 1

            'm_CalcAns.FinalRes.X = 2
            'm_CalcAns.FinalRes.Y = 2000
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メイン処理
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 指定されたパラメータでグラフを描画します
        ''' </summary>
        ''' <returns>描画したグラフのビットマップ</returns>
        ''' <remarks></remarks>
        Public Overridable Function DrawPaint() As Drawing.Bitmap

            '''' 必要なデータを再計算
            Call ExecAriaInf_MapSize()
            Call ExecCalcAns()


            '''' 描画
            Using g As Graphics = CreateGraphics(m_AriaInf.AriaSize)

                '' 描画
                ' ① エリアを描画 (枠)
                '		自分内部で描画
                '		継承先で描画
                '		ユーザーで描画
                ' ② メモリを描画
                '		自分内部で描画
                '		継承先で描画
                '		ユーザーで描画
                ' ③ エンド描画
                '		自分内部で描画(マップ範囲外)
                '		継承先で描画
                '		ユーザーで描画
                Try
                    ' ① エリアを描画 (枠)
                    Call DrawBackGround(g)
                    Call DrawPaint_BackGround(g)
                    RaiseEvent tEv_DrawPaint_BackGround(Me, g)

                    ' ② メモリを描画
                    Call DrawMemori(g)
                    Call DrawPaint_Memori(g)
                    RaiseEvent tEv_DrawPaint_Memori(Me, g)

                    ' ③ エンド描画
                    Call DrawLimit(g)
                    Call DrawPaint_End(g)
                    RaiseEvent tEv_DrawPaint_End(Me, g)

                Catch ex As Exception
                    If Not m_Bmp Is Nothing Then
                        m_Bmp.Dispose()
                        m_Bmp = Nothing
                    End If
                End Try
            End Using


            '''' 生成ビットマップを渡す
            Return m_Bmp
        End Function


        ''' <summary>
        ''' グラフィックインスタンスを生成
        ''' </summary>
        ''' <param name="size">ビットマップサイズ [Pixel]</param>
        ''' <returns>今回生成したグラフィックインスタンス</returns>
        Private Function CreateGraphics(ByVal size As Size) As Graphics
            '' 前回のグラフィックインスタンスを開放
            If Not m_Bmp Is Nothing Then
                m_Bmp.Dispose()
            End If

            '' ビットマップ生成
            Try
                m_Bmp = New Drawing.Bitmap(size.Width, size.Height)
                '' グラフィックインスタンス取得
                Return Graphics.FromImage(m_Bmp)

            Catch ex As Exception
                m_Bmp = Nothing
                Return Nothing
            End Try
        End Function


        ''' <summary>
        ''' グラフエリアを描画
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Overridable Sub DrawBackGround(ByVal g As Graphics)
            '' 背景を初期セット
            g.FillRegion(m_AriaInf.BackGroundColor, New Region(New Drawing.Rectangle(0, 0, m_AriaInf.AriaSize.Width, m_AriaInf.AriaSize.Height)))

            '' マップ内の色を初期セット
            g.FillRegion(m_AriaInf.MapColor, New Region(m_CalcAns.MapRect))

            ' '' 内側のメモリ用枠描画
            ' メモリ描画後に変更
            'Dim x1 As Integer = m_AriaInf.Margin.Left
            'Dim y1 As Integer = m_AriaInf.Margin.Top
            'Dim x2 As Integer = m_AriaInf.Margin.Left + m_CalcAns.MapRect.Width
            'Dim y2 As Integer = m_AriaInf.Margin.Top + m_CalcAns.MapRect.Height

            'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y1, x2, y1)		'横線 (上)
            'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y2, x2, y2)		'横線 (下)
            'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y1, x1, y2)		'縦線 (左)
            'g.DrawLine(m_AriaInf.BackGroundLineColor, x2, y1, x2, y2)		'縦線 (右)
        End Sub

        ''' <summary>
        ''' メモリ描画
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Overridable Sub DrawMemori(ByVal g As Graphics)
            Dim nBasePixel As Integer	' 描画基準位置 [pixel]
            Dim nPixel As Integer		' pixel
            Dim dVal As Double			' 値
            Dim dScaleVal As Double		' 表示する為の値


            If m_MapInf.bIsCenterX Then
                '''' X軸 センター基準
                nBasePixel = m_AriaInf.Margin.Left + m_CalcAns.MapRect.Width \ 2
                dVal = 0
                dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X

                '' 原点
                ' 線
                If m_MemoriInf.bLineDsp_X Then g.DrawLine(m_MemoriInf.LinePen, nBasePixel, m_CalcAns.MapRect.Top + 1, nBasePixel, m_CalcAns.MapRect.Bottom)
                ' 文字
                If m_MemoriInf.bStringDisp_T Then DrawString(g, nBasePixel, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                If m_MemoriInf.bStringDisp_B Then DrawString(g, nBasePixel, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット


                '' 左右方向
                dVal += m_MemoriInf.ScaleInterval.X
                dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X
                While (True) '(dVal <= m_MapInf.nLimitXmax)
                    nPixel = CInt(dVal / m_CalcAns.FinalRes.X)
                    If nBasePixel + nPixel > m_CalcAns.MapRect.Right Then Exit While

                    '線
                    If m_MemoriInf.bLineDsp_X Then
                        g.DrawLine(m_MemoriInf.LinePen, nBasePixel + nPixel, m_CalcAns.MapRect.Top + 1, nBasePixel + nPixel, m_CalcAns.MapRect.Bottom)
                        g.DrawLine(m_MemoriInf.LinePen, nBasePixel - nPixel, m_CalcAns.MapRect.Top + 1, nBasePixel - nPixel, m_CalcAns.MapRect.Bottom)
                    End If

                    ' 文字
                    If m_MemoriInf.bStringDisp_T Then
                        DrawString(g, nBasePixel + nPixel, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	   ' 先頭の値セット
                        DrawString(g, nBasePixel - nPixel, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	   ' 先頭の値セット
                    End If
                    If m_MemoriInf.bStringDisp_B Then
                        DrawString(g, nBasePixel + nPixel, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	' 先頭の値セット
                        DrawString(g, nBasePixel - nPixel, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	' 先頭の値セット
                    End If

                    '次
                    dVal += m_MemoriInf.ScaleInterval.X
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X
                End While
            Else

                '''' X軸 基準
                dVal = m_CalcAns.DspStartVal.X
                dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X

                '' 原点
                ' 文字のみ
                If m_MapInf.bIsBaseX_L Then
                    If m_MemoriInf.bStringDisp_T Then DrawString(g, m_CalcAns.MapRect.Left, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    If m_MemoriInf.bStringDisp_B Then DrawString(g, m_CalcAns.MapRect.Left, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                Else
                    If m_MemoriInf.bStringDisp_T Then DrawString(g, m_CalcAns.MapRect.Right, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    If m_MemoriInf.bStringDisp_B Then DrawString(g, m_CalcAns.MapRect.Right, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                End If

                ''次の目盛り線位置決定
                dVal = (Int((dVal + 0.5) / m_MemoriInf.ScaleInterval.X) + 1) * m_MemoriInf.ScaleInterval.X	' 最初に描画する目盛り値 (最初と重なっている場合は次の奴ね)
                dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X
                While (True)
                    nPixel = GetVtoP_X(dVal)													' ピクチャー左上からのピクセル位置
                    If m_MapInf.bIsBaseX_L Then
                        If nPixel > m_CalcAns.MapRect.Right Then Exit While
                    Else
                        If nPixel < m_CalcAns.MapRect.Left Then Exit While
                    End If

                    '線
                    If m_MemoriInf.bLineDsp_X Then g.DrawLine(m_MemoriInf.LinePen, nPixel, m_CalcAns.MapRect.Top + 1, nPixel, m_CalcAns.MapRect.Bottom)

                    ' 文字
                    If m_MemoriInf.bStringDisp_T Then DrawString(g, nPixel, m_CalcAns.MapRect.Top, 0, -m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.TopCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    If m_MemoriInf.bStringDisp_B Then DrawString(g, nPixel, m_CalcAns.MapRect.Bottom, 0, +m_MemoriInf.nPosStringX, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatX), ContentAlignment.BottomCenter, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット

                    '次
                    dVal += m_MemoriInf.ScaleInterval.X
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.X
                End While
                End If


                If m_MapInf.bIsCenterY Then
                    '''' Y軸 センター基準
                    nBasePixel = m_AriaInf.Margin.Top + m_CalcAns.MapRect.Height \ 2
                    dVal = 0
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y

                    '' 原点
                    ' 線
                    If m_MemoriInf.bLineDsp_Y Then g.DrawLine(m_MemoriInf.LinePen, m_CalcAns.MapRect.Left + 1, nBasePixel, m_CalcAns.MapRect.Right, nBasePixel)
                    ' 文字
                    If m_MemoriInf.bStringDisp_L Then DrawString(g, m_CalcAns.MapRect.Left, nBasePixel, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    If m_MemoriInf.bStringDisp_R Then DrawString(g, m_CalcAns.MapRect.Right, nBasePixel, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット


                    '' 上下方向
                    dVal += m_MemoriInf.ScaleInterval.Y
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y
                    While (True)
                        nPixel = CInt(dVal / m_CalcAns.FinalRes.Y)
                        If nBasePixel + nPixel > m_CalcAns.MapRect.Bottom Then Exit While

                        '線
                        If m_MemoriInf.bLineDsp_Y Then
                            g.DrawLine(m_MemoriInf.LinePen, m_CalcAns.MapRect.Left + 1, nBasePixel + nPixel, m_CalcAns.MapRect.Right, nBasePixel + nPixel)
                            g.DrawLine(m_MemoriInf.LinePen, m_CalcAns.MapRect.Left + 1, nBasePixel - nPixel, m_CalcAns.MapRect.Right, nBasePixel - nPixel)
                        End If

                        ' 文字
                        If m_MemoriInf.bStringDisp_L Then
                            DrawString(g, m_CalcAns.MapRect.Left, nBasePixel + nPixel, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	  ' 先頭の値セット
                            DrawString(g, m_CalcAns.MapRect.Left, nBasePixel - nPixel, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh)	   ' 先頭の値セット
                        End If
                        If m_MemoriInf.bStringDisp_R Then
                            DrawString(g, m_CalcAns.MapRect.Right, nBasePixel + nPixel, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                            DrawString(g, m_CalcAns.MapRect.Right, nBasePixel - nPixel, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                        End If

                        '次
                        dVal += m_MemoriInf.ScaleInterval.Y
                        dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y
                    End While
                Else

                    '''' Y軸 基準
                    dVal = m_CalcAns.DspStartVal.Y
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y

                    '' 原点
                    ' 文字のみ
                    If m_MapInf.bIsBaseY_T Then
                        If m_MemoriInf.bStringDisp_L Then DrawString(g, m_CalcAns.MapRect.Left, m_CalcAns.MapRect.Top, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                        If m_MemoriInf.bStringDisp_R Then DrawString(g, m_CalcAns.MapRect.Right, m_CalcAns.MapRect.Top, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    Else
                        If m_MemoriInf.bStringDisp_L Then DrawString(g, m_CalcAns.MapRect.Left, m_CalcAns.MapRect.Bottom, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                        If m_MemoriInf.bStringDisp_R Then DrawString(g, m_CalcAns.MapRect.Right, m_CalcAns.MapRect.Bottom, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                    End If

                    ''次の目盛り線位置決定
                    dVal = (Int((dVal + 0.5) / m_MemoriInf.ScaleInterval.Y) + 1) * m_MemoriInf.ScaleInterval.Y	' 最初に描画する目盛り値 (最初と重なっている場合は次の奴ね)
                    dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y
                    While (True)
                        nPixel = GetVtoP_Y(dVal)													' ピクチャー左上からのピクセル位置
                        If m_MapInf.bIsBaseY_T Then
                            If nPixel > m_CalcAns.MapRect.Bottom Then Exit While
                        Else
                            If nPixel < m_CalcAns.MapRect.Top Then Exit While
                        End If

                        '線
                        If m_MemoriInf.bLineDsp_Y Then g.DrawLine(m_MemoriInf.LinePen, m_CalcAns.MapRect.Left + 1, nPixel, m_CalcAns.MapRect.Right, nPixel)

                        ' 文字
                        If m_MemoriInf.bStringDisp_L Then DrawString(g, m_CalcAns.MapRect.Left, nPixel, -m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleLeft, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット
                        If m_MemoriInf.bStringDisp_R Then DrawString(g, m_CalcAns.MapRect.Right, nPixel, +m_MemoriInf.nPosStringY, 0, dScaleVal.ToString(m_MemoriInf.sScaleStringFormatY), ContentAlignment.MiddleRight, m_MemoriInf.StringFont, m_MemoriInf.StringBsh) ' 先頭の値セット

                        '次
                        dVal += m_MemoriInf.ScaleInterval.Y
                        dScaleVal = dVal / m_MemoriInf.ScaleStringRange.Y
                    End While

                End If


                '''' マップ領域の枠描画
                g.DrawRectangle(m_AriaInf.BackGroundLineColor, m_CalcAns.MapRect)
                ' ↓これ全部が↑これだけでOK
                'Dim x1 As Integer = m_AriaInf.Margin.Left
                'Dim y1 As Integer = m_AriaInf.Margin.Top
                'Dim x2 As Integer = m_AriaInf.Margin.Left + m_CalcAns.MapRect.Width
                'Dim y2 As Integer = m_AriaInf.Margin.Top + m_CalcAns.MapRect.Height
                'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y1, x2, y1)		'横線 (上)
                'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y2, x2, y2)		'横線 (下)
                'g.DrawLine(m_AriaInf.BackGroundLineColor, x1, y1, x1, y2)		'縦線 (左)
                'g.DrawLine(m_AriaInf.BackGroundLineColor, x2, y1, x2, y2)		'縦線 (右)
        End Sub

        ''' <summary>
        ''' マップ範囲外描画
        ''' </summary>
        ''' <param name="g">グラフィックインスタンス</param>
        ''' <remarks></remarks>
        Public Overridable Sub DrawLimit(ByVal g As Graphics)
            If Not m_LimitInf.bLimitDraw Then Return

            '''' 対象範囲範囲外の背景色
            Dim nSize As Integer

            '' 左側
            Dim nXmin As Integer = GetVtoP_X(m_LimitInf.nLimitXmin)
            If m_MapInf.bIsBaseX_L Then
                nSize = nXmin - m_CalcAns.MapRect.Left - 1
                ' 線は消さないようにする為、内側から塗りつぶす
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, m_CalcAns.MapRect.Top + 1, nSize, m_CalcAns.MapRect.Height - 1)))
            Else
                nSize = m_CalcAns.MapRect.Right - nXmin - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(nXmin + 1, m_CalcAns.MapRect.Top + 1, nSize, m_CalcAns.MapRect.Height - 1)))
            End If

            '' 右側
            Dim nXmax As Integer = GetVtoP_X(m_LimitInf.nLimitXmax)
            If m_MapInf.bIsBaseX_L Then
                nSize = m_CalcAns.MapRect.Right - nXmax - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(nXmax + 1, m_CalcAns.MapRect.Top + 1, nSize, m_CalcAns.MapRect.Height - 1)))
            Else
                nSize = nXmax - m_CalcAns.MapRect.Left - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, m_CalcAns.MapRect.Top + 1, nSize, m_CalcAns.MapRect.Height - 1)))
            End If

            '' 上側
            Dim nYmin As Integer = GetVtoP_Y(m_LimitInf.nLimitYmin)
            If m_MapInf.bIsBaseY_T Then
                nSize = nYmin - m_CalcAns.MapRect.Top - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, m_CalcAns.MapRect.Top + 1, m_CalcAns.MapRect.Width - 1, nSize)))
            Else
                nSize = m_CalcAns.MapRect.Bottom - nYmin - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, nYmin + 1, m_CalcAns.MapRect.Width - 1, nSize)))
            End If

            '' 下側
            Dim nYmax As Integer = GetVtoP_Y(m_LimitInf.nLimitYmax)
            If m_MapInf.bIsBaseY_T Then
                nSize = m_CalcAns.MapRect.Bottom - nYmax - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, nYmax + 1, m_CalcAns.MapRect.Width - 1, nSize)))
            Else
                nSize = nYmax - m_CalcAns.MapRect.Top - 1
                If nSize > 0 Then g.FillRegion(m_LimitInf.LimitOutColor, New Region(New Rectangle(m_CalcAns.MapRect.Left + 1, m_CalcAns.MapRect.Top + 1, m_CalcAns.MapRect.Width - 1, nSize)))

            End If

            '''' 対象範囲部分の境界線を描画
            g.DrawLine(m_LimitInf.LimitOutLineColor, nXmin, nYmin, nXmax, nYmin)		'横線 (上)
            g.DrawLine(m_LimitInf.LimitOutLineColor, nXmin, nYmax, nXmax, nYmax)		'横線 (下)
            g.DrawLine(m_LimitInf.LimitOutLineColor, nXmin, nYmin, nXmin, nYmax)		'縦線 (左)
            g.DrawLine(m_LimitInf.LimitOutLineColor, nXmax, nYmin, nXmax, nYmax)		'縦線 (右)
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 小物処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' マップ描画領域の再計算
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ExecAriaInf_MapSize()
            Dim size As Size = m_AriaInf.AriaSize

            size.Width -= m_AriaInf.Margin.Horizontal
            If 0 > size.Width Then size.Width = 1

            size.Height -= m_AriaInf.Margin.Vertical
            If 0 > size.Height Then size.Height = 1

            m_CalcAns.MapRect = New Rectangle(m_AriaInf.Margin.Left, m_AriaInf.Margin.Top, size.Width, size.Height)
        End Sub
        ''' <summary>
        ''' 描画座標系再計算
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ExecCalcAns()
            Dim dVal As Double

            '''' 表示分解能計算
            '' 両端にあわせるモードの場合、自動計算させる
            If m_MapInf.bIsStrechX Then
                dVal = m_MapInf.nLimitXmax - m_MapInf.nLimitXmin
                If dVal < 0 Then dVal = 0.1

                m_CalcAns.FinalRes.X = dVal / m_CalcAns.MapRect.Width
            End If
            If m_MapInf.bIsStrechY Then
                dVal = m_MapInf.nLimitYmax - m_MapInf.nLimitYmin
                If dVal < 0 Then dVal = 0.1

                m_CalcAns.FinalRes.Y = dVal / m_CalcAns.MapRect.Height
            End If

            '' 最終チェック
            If m_CalcAns.FinalRes.X <= 0 Then m_CalcAns.FinalRes.X = 1
            If m_CalcAns.FinalRes.Y <= 0 Then m_CalcAns.FinalRes.Y = 1



            '''' 表示範囲計算
            If m_MapInf.bIsStrechX Then
                m_CalcAns.DspStartVal.X = m_MapInf.nLimitXmin
                m_CalcAns.DspEndVal.X = m_MapInf.nLimitXmax
            Else
                m_CalcAns.DspStartVal.X = m_MapInf.Offset.X
                m_CalcAns.DspEndVal.X = m_CalcAns.DspStartVal.X + (m_CalcAns.MapRect.Width * m_CalcAns.FinalRes.X)
            End If

            If m_MapInf.bIsStrechY Then
                m_CalcAns.DspStartVal.Y = m_MapInf.nLimitYmin
                m_CalcAns.DspEndVal.Y = m_MapInf.nLimitYmax
            Else
                m_CalcAns.DspStartVal.Y = m_MapInf.Offset.Y
                m_CalcAns.DspEndVal.Y = m_CalcAns.DspStartVal.Y + (m_CalcAns.MapRect.Height * m_CalcAns.FinalRes.Y)
            End If
        End Sub
    End Class
End Namespace

