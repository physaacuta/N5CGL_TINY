'*********************************************************************************
'画像表示Baseクラス
'	[Ver]
'		Ver.01    2008/09/06  vs2005 対応
'
'	[メモ]
'		ある意味疵検専用？？
'*********************************************************************************
Option Strict On				'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports system.Drawing

Imports tClassLibrary
Imports System.Windows.Forms
'Imports System.IO
Namespace tClass

    ''' <summary>画像表示基底クラス</summary>
    Public Class tClass_ImageDspBase
        Inherits tClass_ImageDsp_Calc
        Implements IDisposable							' デストラクタ定義

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 定数定義
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>メモリ設定情報</summary>
        Public Structure MEMORI_INF
            ''' <summary>メモリ表示有無 (true:有, false:無)</summary>
            Dim bIsDsp As Boolean
            ''' <summary>メモリ表示色</summary>
            Dim Color As Color
            ''' <summary>メモリフォントサイズ</summary>
            Dim FontSize As Integer
            ''' <summary>メモリ線サイズ</summary>
            Dim LineWidth As Integer
            ''' <summary>区切り線の高さ</summary>
            Dim HojoLineLen As Integer


            ''' <summary>原点表示文字</summary>
            Dim strBaseString As String
            ''' <summary>X軸のライン表示位置(下から何ピクセル)</summary>
            Dim nPosLineBot As Integer
            ''' <summary>Y軸のライン表示位置(左から何ピクセル)</summary>
            Dim nPosLineLeft As Integer
            ''' <summary>X軸の文字表示位置(下から何ピクセル)</summary>
            Dim nPosStringBot As Integer
            ''' <summary>Y軸の文字表示位置(左から何ピクセル)</summary>
            Dim nPosStringLeft As Integer

            ''' <summary>X軸のメモリ表示間隔[mm]</summary>
            Dim nIntervalX As Integer
            ''' <summary>Y軸のメモリ表示間隔[mm]</summary>
            Dim nIntervalY As Integer
            ''' <summary>X軸のメモリ表示回数</summary>
            Dim nLoopCntX As Integer
            ''' <summary>Y軸のメモリ表示回数</summary>
            Dim nLoopCntY As Integer

        End Structure



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        '''''' 描画関係
        Protected m_cntPic As PictureBox = Nothing

        ''' <summary>イメージオブジェクト</summary>
        Protected m_Img() As Image = Nothing				' イメージオブジェクト
        ''' <summary>デフォル用トイメージオブジェクト</summary>
        Protected m_DefImg As Image = Nothing				' デフォルト用イメージオブジェクト
        ''' <summary>描画有無 (大前提)</summary>
        Protected m_IsPaint As Boolean = False				' 描画有無 (大前提)
        ''' <summary>デフォルト画像表示</summary>
        Protected m_IsDefImgDsp As Boolean = True			' デフォルト画像表示


        'Private m_OldTurnLR As Boolean = False				' 前回左右反転フラグ値
        'Private m_OldTurnTB As Boolean = False				' 前回上下反転フラグ値


        '''''' メモリ関係
        ''' <summary>メモリ情報</summary>
        Protected m_Memori As MEMORI_INF					' メモリ情報
        Protected m_memoriBsh As SolidBrush
        Protected m_memoriFnt As Font
        Protected m_memoriPen As Pen



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部イベント
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' ユーザー初期描画
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
        ''' <summary>
        ''' ユーザー最終描画
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)

        ''' <summary>
        ''' ユーザーデフォルト画像最終描画
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_NonEnd(ByVal e As System.Windows.Forms.PaintEventArgs)


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////

#Region "描画関係"
        ''' <summary>
        ''' イメージオブジェクト
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆画像表示基底クラス-描画関係-専用プロパティ") _
         , System.ComponentModel.Description("イメージオブジェクト")> _
        Public ReadOnly Property tProp_Paint_Img(ByVal index As Integer) As Image
            Get
                If m_Img Is Nothing Then Return Nothing
                Return m_Img(index)
            End Get
        End Property
        Public ReadOnly Property tProp_Paint_Img() As Image()
            Get
                Return m_Img
            End Get
        End Property
        ''' <summary>
        ''' デフォルト用イメージオブジェクト
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆画像表示基底クラス-描画関係-専用プロパティ") _
         , System.ComponentModel.Description("デフォルト用イメージオブジェクト")> _
        Public ReadOnly Property tProp_Paint_DefImg() As Image
            Get
                Return m_DefImg
            End Get
        End Property
        ''' <summary>
        ''' 描画有無 (大前提)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆画像表示基底クラス-描画関係-専用プロパティ") _
         , System.ComponentModel.Description("描画有無 (大前提)")> _
        Public Property tProp_Paint_IsPaint() As Boolean
            Get
                Return m_IsPaint
            End Get
            Set(ByVal value As Boolean)
                m_IsPaint = value
            End Set
        End Property
        ''' <summary>
        ''' デフォルト画像表示
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆画像表示基底クラス-描画関係-専用プロパティ") _
         , System.ComponentModel.Description("デフォルト画像表示")> _
        Public Property tProp_Paint_IsDefImgDspn() As Boolean
            Get
                Return m_IsDefImgDsp
            End Get
            Set(ByVal value As Boolean)
                m_IsDefImgDsp = value
            End Set
        End Property

#End Region

#Region "メモリ設定情報"
        ''' <summary>
        ''' メモリ設定情報
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆画像表示基底クラス-メモリ設定情報-専用プロパティ") _
         , System.ComponentModel.Description("メモリ設定情報")> _
        Public Property tProp_MemoriInf() As MEMORI_INF
            Get
                Return m_Memori
            End Get
            Set(ByVal value As MEMORI_INF)
                m_Memori = value
                '' メモリ描画に必要なオブジェクト生成する
                Call CreateMemoriObj()
            End Set
        End Property
#End Region


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 処理
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' クラス内の描画処理を実行させる
        ''' </summary>
        ''' <param name="pic"></param>
        ''' <remarks></remarks>
        Public Sub AddHandler_PaintEvent(ByVal pic As PictureBox)
            m_cntPic = pic
            AddHandler pic.Paint, AddressOf Paint_Main
        End Sub
        ''' <summary>
        ''' 表示エリア リサイズ イベントを実行
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub AddHandler_PicResize()
            '毎回問い合わせなので不要
        End Sub

        ''' <summary>
        ''' 表示エリア情報セット 
        ''' </summary>
        Public Sub SetAriaInf()
            If m_cntPic Is Nothing Then Return
            'こいつが、回転時の表示領域を変換する。
            If tProp_ImgKind_Rotate90 Then
                Me.tProp_AriaInf_Aria = New Size(m_cntPic.ClientSize.Height, m_cntPic.ClientSize.Width)
            Else
                Me.tProp_AriaInf_Aria = m_cntPic.ClientSize
            End If
        End Sub

        ''' <summary>
        ''' メモリ描画に必要なオブジェクト生成する
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub CreateMemoriObj()
            If Not m_memoriBsh Is Nothing Then m_memoriBsh.Dispose()
            If Not m_memoriPen Is Nothing Then m_memoriPen.Dispose()
            If Not m_memoriFnt Is Nothing Then m_memoriFnt.Dispose()

            m_memoriBsh = New SolidBrush(m_Memori.Color)
            m_memoriPen = New System.Drawing.Pen(m_Memori.Color, m_Memori.LineWidth)
            m_memoriFnt = New System.Drawing.Font("ＭＳ ゴシック", m_Memori.FontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        End Sub


#Region " IDisposable Support "
        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                If Not m_memoriBsh Is Nothing Then m_memoriBsh.Dispose()
                If Not m_memoriPen Is Nothing Then m_memoriPen.Dispose()
                If Not m_memoriFnt Is Nothing Then m_memoriFnt.Dispose()
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


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ イメージオブジェクト 関係
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "イメージオブジェクト セット"
        ''' <summary>
        ''' イメージオブジェクトをセット (1画像のみの専用)
        ''' </summary>
        ''' <param name="img">イメージクラス</param>
        ''' <param name="bDispose">false:前画像の開放無し</param>
        ''' <remarks>Nothingを渡せば、開放のみとなる</remarks>
        Public Overloads Sub SetImage(ByRef img As Image, Optional ByVal bDispose As Boolean = False)
            Try
                ' 1画像モード時？
                If m_ImgInf.nImgCntX <> 1 Or m_ImgInf.nImgCntY <> 1 Then Return

                ' 現在のイメージを開放
                If bDispose Then
                    If Not m_Img Is Nothing AndAlso Not m_Img(0) Is Nothing Then m_Img(0).Dispose()
                End If


                ' 新しいイメージをセット
                ReDim m_Img(0)
                m_Img(0) = img

                '' 反転状態初期化
                'm_OldTurnLR = False
                'm_OldTurnTB = False
            Catch ex As Exception
                m_Img = Nothing
            End Try
        End Sub

        ''' <summary>
        ''' イメージオブジェクトをセット (複数画像の専用)
        ''' </summary>
        ''' <param name="img">イメージクラス (セット順番はZ順となるように画像をセットする)</param>
        ''' <param name="bDispose">false:前画像の開放無し</param>
        ''' <remarks>Nothingを渡せば、開放のみとなる</remarks>
        Public Overloads Sub SetImage(ByRef img As Image(), Optional ByVal bDispose As Boolean = False)
            Try
                Dim imgcnt As Integer = m_ImgInf.nImgCntX * m_ImgInf.nImgCntY
                ' 指定枚数画像がある？
                If imgcnt <> img.Length Then Return

                '' 現在のイメージを開放
                If bDispose Then
                    If Not m_Img Is Nothing Then
                        For ii As Integer = 0 To m_Img.Length - 1
                            m_Img(ii).Dispose()
                            m_Img(ii) = Nothing
                        Next ii
                    End If
                End If

                ' 新しいイメージをセット
                ReDim m_Img(imgcnt - 1)
                For ii As Integer = 0 To imgcnt - 1
                    m_Img(ii) = img(ii)
                Next ii

                '' 反転状態初期化
                'm_OldTurnLR = False
                'm_OldTurnTB = False
            Catch ex As Exception
                m_Img = Nothing
            End Try
        End Sub

        Public Overloads Sub GetImage(ByRef img As Image())
            img = m_Img
        End Sub



        ''' <summary>
        ''' バイナリデータからイメージオブジェクトを作成(Jepg形式)
        ''' </summary>
        ''' <param name="bytData"></param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal bytData() As Byte)
            ' イメージセット
            Call SetImage(tClass.tClass_BmpJpeg.DataToImage(bytData))
        End Sub

        ''' <summary>
        ''' バイナリデータからイメージオブジェクトを作成(Bitmap形式)
        ''' </summary>
        ''' <param name="nWidth">画像の幅</param>
        ''' <param name="nHeight">画像の高さ</param>
        ''' <param name="bytData">バイナリデータ</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal bytData() As Byte)
            ' イメージセット (ビットマップヘッダも合わせて付与)
            Call SetImage(tClass.tClass_BmpJpeg.DataToBmp(nWidth, nHeight, bytData))
        End Sub

        ''' <summary>
        ''' ファイルからイメージオブジェクトをセット
        ''' </summary>
        ''' <param name="strFPath">フルパス</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal strFPath As String)
            ' イメージセット
            Call SetImage(tClass.tClass_BmpJpeg.BmpFileLoad(strFPath))
        End Sub



        ''' <summary>
        ''' デフォルトイメージオブジェクトをセット
        ''' </summary>
        ''' <param name="img">イメージクラス</param>
        ''' <remarks>Nothingを渡せば、開放のみとなる</remarks>
        Public Overloads Sub SetDefImage(ByRef img As Image)
            Try
                ' 現在のイメージを開放
                If Not m_DefImg Is Nothing Then m_DefImg.Dispose()

                ' 新しいイメージをセット
                m_DefImg = img

            Catch ex As Exception
                m_DefImg = Nothing
            End Try
        End Sub

        ''' <summary>
        ''' バイナリデータからデフォルトイメージオブジェクトを作成(Jepg形式)
        ''' </summary>
        ''' <param name="bytData"></param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal bytData() As Byte)
            ' イメージセット
            Call SetDefImage(tClass.tClass_BmpJpeg.DataToImage(bytData))
        End Sub

        ''' <summary>
        ''' バイナリデータからデフォルトイメージオブジェクトを作成(Bitmap形式)
        ''' </summary>
        ''' <param name="nWidth">画像の幅</param>
        ''' <param name="nHeight">画像の高さ</param>
        ''' <param name="bytData">バイナリデータ</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal bytData() As Byte)
            ' イメージセット (ビットマップヘッダも合わせて付与)
            Call SetDefImage(tClass.tClass_BmpJpeg.DataToBmp(nWidth, nHeight, bytData))
        End Sub

        ''' <summary>
        ''' ファイルからデフォルトイメージオブジェクトをセット
        ''' </summary>
        ''' <param name="strFPath">フルパス</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal strFPath As String)
            ' イメージセット
            Call SetDefImage(tClass.tClass_BmpJpeg.BmpFileLoad(strFPath))
        End Sub



        ''' <summary>
        ''' 現在の表示画像
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetImage() As Image()
            Return m_Img
        End Function
        ''' <summary>
        ''' デフォルトイメージ
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetDefImage() As Image
            Return m_DefImg
        End Function
#End Region


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 描画 関係
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 再描画
        ''' </summary>
        Public Sub Refresh()
            If m_cntPic Is Nothing Then Return
            'm_cntPic.Update()
            m_cntPic.Invalidate()   ' 手空きなら描画
            'm_cntPic.Refresh()
        End Sub
        '強制再描画
        Public Sub RefreshManual()
            If m_cntPic Is Nothing Then Return
            m_cntPic.Refresh()
        End Sub

        ''' <summary>
        ''' メイン描画
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_Main(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            '' 描画有無 (大前提)
            If Not m_IsPaint Then Exit Sub

            '90度回転でResizeによらず変わるので、毎回問い合わせる。
            '所詮ピクチャコントロールのサイズ問い合わせなのでたいしたことはない
            SetAriaInf()

            '' データ種類により画像方法切替
            If m_IsDefImgDsp Then
                Paint_Main_Non(e)
            Else
                Paint_Main_Img(e)
            End If
        End Sub

        ''' <summary>
        ''' データ無し用イメージ描画
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Main_Non(ByVal e As System.Windows.Forms.PaintEventArgs)
            'とりあえず、Transformは効かせておく。90度回転しているとm_AriaInfが微妙だから。
            e.Graphics.Transform = SetupTransform(New Point(0, 0), New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight))

            e.Graphics.DrawImage(m_DefImg, New Rectangle(0, 0, m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight))

            '' ユーザー最終描画
            RaiseEvent tEv_Paint_NonEnd(e)
            e.Graphics.ResetTransform()
        End Sub

        ''' <summary>
        ''' データ有り用イメージ描画
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Main_Img(ByVal e As System.Windows.Forms.PaintEventArgs)
            '// =========================================
            '// 順番も大事な為、改造時は、気をつける事
            '//

            '' 計算
            Calculation()

            '' 初期描画
            Paint_Start(e)
            '' ユーザー初期描画
            RaiseEvent tEv_Paint_Start(e)

            '' 画像を描画
            If m_ImgInf.nImgCntX = 1 AndAlso m_ImgInf.nImgCntY = 1 Then
                Paint_Img(e)                ' 1枚の絵
            Else
                Paint_ImgArray(e)           ' 複数枚の絵
            End If

            '' メモリ描画
            Paint_Memori(e)

            '' 最終描画
            Paint_End(e)
            '' ユーザー最終描画
            RaiseEvent tEv_Paint_End(e)
        End Sub


        ''' <summary>
        ''' デバッグ用：いまｇについている変換行列で、rはどのように変換されるか。
        ''' </summary>
        ''' <param name="g">Graphics</param>
        ''' <param name="r">四角座標</param>
        ''' <returns>変換後（最終的に描かれる座標）</returns>
        ''' <remarks></remarks>
        Private Function testtransform(ByVal g As Graphics, ByVal r As RectangleF) As RectangleF
            Dim pts() As PointF = {New PointF(r.Left, r.Top), New PointF(r.Right, r.Bottom)}

            g.Transform().TransformPoints(pts)
            Return New RectangleF(pts(0).X, pts(0).Y, pts(1).X - pts(0).X, pts(1).Y - pts(0).Y)
        End Function


        ''' <summary>
        ''' 画像転送用の変換affine行列を用意する
        ''' </summary>
        ''' <param name="ptDst">左上座標（変換前基準）</param>
        ''' <param name="szDst">画像のサイズ（回転中心）</param>
        ''' <returns>変換用行列</returns>
        ''' <remarks>m_ImgKindの、回転、幅反転、長手反転の影響をうけます</remarks>
        Private Function SetupTransform(ByVal ptDst As Point, ByVal szDst As Size) As System.Drawing.Drawing2D.Matrix

            '初期化。[1,0], [1,0], [0,0]
            Dim matrix As New System.Drawing.Drawing2D.Matrix(1, 0, 0, 1, 0, 0)

            'まずは回転のために、画像の中心へ。
            Dim ptCenter As PointF = New PointF(Math.Abs(szDst.Width) / 2.0F, Math.Abs(szDst.Height) / 2.0F)
            matrix.Translate(-ptCenter.X, -ptCenter.Y, Drawing2D.MatrixOrder.Append)

            '幅方向の反転
            If (m_ImgKind.bIsTurnLR) Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(-1, 0, 0, 1, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '長手方向の反転
            If (m_ImgKind.bIsTurnTB) Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(1, 0, 0, -1, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '回転
            If m_ImgKind.bIsRotate90 Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(0, -1, 1, 0, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '画像中心から、転送先へ変換
            If m_ImgKind.bIsRotate90 Then
                matrix.Translate(ptCenter.Y + ptDst.Y, ptCenter.X + ptDst.X, Drawing2D.MatrixOrder.Append)
            Else
                matrix.Translate(ptCenter.X + ptDst.X, ptCenter.Y + ptDst.Y, Drawing2D.MatrixOrder.Append)
            End If

            Return matrix
        End Function



        ''' <summary>
        ''' 画像描画
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Img(ByVal e As System.Windows.Forms.PaintEventArgs)

            '''' このやり方は、簡単だが、実際の画像に手を加える為、画像の使いまわしなどでおかしなこととなる為、ヤメ
            '' 左右反転
            'If m_OldTurnLR <> m_ImgKind.bIsTurnLR Then
            '	m_Img(0).RotateFlip(RotateFlipType.RotateNoneFlipX)
            '	m_OldTurnLR = m_ImgKind.bIsTurnLR
            'End If

            '' 上下反転
            'If m_OldTurnTB <> m_ImgKind.bIsTurnTB Then
            '	m_Img(0).RotateFlip(RotateFlipType.RotateNoneFlipY)
            '	m_OldTurnTB = m_ImgKind.bIsTurnTB
            'End If

            '' 描画
            'e.Graphics.DrawImage(m_Img(0), m_CalcAns.ImgDspRect)



            'Dim x, y, w, h As Single
            '' 左右反転
            'If m_ImgKind.bIsTurnLR Then
            '    x = m_CalcAns.ImgDspRect.Width + m_CalcAns.ImgDspRect.X
            '    w = m_CalcAns.ImgDspRect.Width * -1
            'Else
            '    x = m_CalcAns.ImgDspRect.X
            '    w = m_CalcAns.ImgDspRect.Width
            'End If
            '' 上下反転
            'If m_ImgKind.bIsTurnTB Then
            '    y = m_CalcAns.ImgDspRect.Height + m_CalcAns.ImgDspRect.Y
            '    h = m_CalcAns.ImgDspRect.Height * -1
            'Else
            '    y = m_CalcAns.ImgDspRect.Y
            '    h = m_CalcAns.ImgDspRect.Height
            'End If

            '' 描画
            'e.Graphics.DrawImage(m_Img(0), x, y, w, h)

            Try
                Dim dst As Rectangle = m_CalcAns.ImgDspRect
                e.Graphics.Transform = SetupTransform(dst.Location, dst.Size)
                If m_Img(0) IsNot Nothing Then
                    e.Graphics.DrawImage(m_Img(0), New Rectangle(0, 0, dst.Width, dst.Height))
                End If
                e.Graphics.ResetTransform()
            Catch ex As Exception

            End Try
        End Sub

        ''' <summary>
        ''' 画像描画
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_ImgArray(ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim g As Graphics = e.Graphics

            Try
                ' 描画
                For yy As Integer = 0 To m_ImgInf.nImgCntY - 1
                    For xx As Integer = 0 To m_ImgInf.nImgCntX - 1
                        Dim nIndex As Integer = (yy * m_ImgInf.nImgCntX) + xx
                        If nIndex >= m_Img.Length OrElse m_Img(nIndex) Is Nothing Then Continue For
                        Dim img As Image = m_Img(nIndex)

                        ' Zの順に描画する
                        Try
                            '他スレッドでimgは破棄されちゃった
                            If img.PixelFormat = Imaging.PixelFormat.DontCare Then
                                Continue For
                            End If

                            'g.DrawImage(m_Img(nIndex), GetAnsOneImgSize(jj, ii))

                            Dim rect As Rectangle = GetAnsOneImgSize(xx, yy)
                            'これで、回転したり反転したりしながら、rect.Left､rect.TOPへ転送されるようになる
                            g.Transform = SetupTransform(rect.Location, rect.Size)

                            '幅が大きすぎると描画に失敗することがあるので、描画領域を制限する
                            If img.Width > 4096 Then
                                'Xでの描画範囲は、０～ウインドウ幅まででよい。
                                '変換の逆行列で元画像範囲をもとめる
                                Dim dWidth As Integer = tProp_AriaInf_Aria.Width
                                Dim matInv As System.Drawing.Drawing2D.Matrix = g.Transform
                                matInv.Invert()

                                Dim Pt() As Point = New Point() {New Point(0, 0), New Point(dWidth, dWidth)}
                                matInv.TransformPoints(Pt)

                                '元X座標が求まった
                                '（縮小後・回転等前座標）
                                Dim xStSmall As Integer = Math.Min(Pt(0).X, Pt(1).X)
                                Dim xSzSmall As Integer = Math.Abs(Pt(0).X - Pt(1).X)
                                '（画像座標）
                                Dim xStLarge As Integer = Math.Min(Pt(0).X, Pt(1).X) * img.Width \ rect.Width
                                Dim xSzLarge As Integer = Math.Abs(Pt(0).X - Pt(1).X) * img.Width \ rect.Width


                                '座標計算をやりなおし
                                g.Transform = SetupTransform(New Point(0, rect.Y), New Size(xSzSmall, img.Height))

                                '元画像の座標つきDraw
                                g.DrawImage(img, New Rectangle(0, 0, dWidth, rect.Height), _
                                  New Rectangle(xStLarge, 0, xSzLarge, img.Height), GraphicsUnit.Pixel)

                            Else


                                If rect.Width = img.Width And rect.Height = img.Height Then
                                    g.DrawImage(img, New Point(0, 0))
                                Else
                                    '転送先座標位置が0,0な理由は、すでに行列中に含まれるから。
                                    g.DrawImage(img, New Rectangle(0, 0, rect.Width, rect.Height))
                                End If
                            End If

                            'デバッグ：画像の番号と高さの表示
                            If False Then
                                Dim disp As String = String.Format("{0} : {1}, {2} {3} {4}", _
                                      yy, m_Img(nIndex).Height, rect.Height, IIf(rect.Height = img.Height, "○", "×"), rect.Width)
                                Using f As Font = New Font("MS Gothic", 20, FontStyle.Bold, GraphicsUnit.Pixel)
                                    g.DrawString(disp, f, Brushes.Blue, New PointF(rect.Width / 4.0F, rect.Height / 2.0F))
                                End Using
                            End If

                            'デバッグ：座標変換したあとの確認用
                            If False And nIndex = 0 Then
                                Dim ptR As New PointF(rect.Right, rect.Bottom)
                                Console.WriteLine("{2}: {0} -> {1} ", _
                                   rect, _
                                   testtransform(g, rect), _
                                   nIndex)
                            End If


                            '読み込みで異常がおきると、img.tagにその例外が乗っている、
                            '文字列描画する. img事態がNothingは考えない。
                            Dim ex As Exception = TryCast(img.Tag, Exception)
                            If ex IsNot Nothing Then
                                Dim rcd As RectangleF = testtransform(g, New RectangleF(0, 0, rect.Width, rect.Height))
                                If rcd.X < 0 Then rcd.X = 0
                                If rcd.Height < 0 Then rcd.Y -= 16

                                Dim exception_1st_stack As String = GetTopFunctionOfStacktrace(ex)
                                Dim disp As String = String.Format("{0} @ {1}", ex.Message, exception_1st_stack)

                                g.ResetTransform()

                                Using f As Font = New Font("MS Gothic", 16, FontStyle.Bold, GraphicsUnit.Pixel)
                                    g.DrawString(disp, f, Brushes.Blue, New PointF(rcd.X, rcd.Y))
                                End Using

                            End If

                            'デバッグ：フレーム区切りの表示()
                            If False Then
                                g.ResetTransform()
                                If tProp_ImgKind_Rotate90 Then
                                    g.DrawRectangle(Pens.Aqua, New Rectangle(rect.Top, rect.Left, rect.Height, rect.Width))
                                Else
                                    g.DrawRectangle(Pens.Aqua, rect)
                                End If
                            End If

                        Catch ex As Exception
                        End Try

                    Next xx
                Next yy
            Catch ex As Exception
            End Try
            g.ResetTransform()
        End Sub


        ''' <summary>
        ''' 例外のスタックトレースの、いちばん上の関数名をかえす。
        ''' </summary>
        Private Shared Function GetTopFunctionOfStacktrace(ByVal ex As Exception) As String
            Dim strace As String = ex.StackTrace().Split(vbCr(0))(0)
            Dim stack_for_msg As String = strace

            Dim splitted_msg() As String = strace.Split("\:.".ToCharArray)
            Dim last_item As Integer = splitted_msg.Length - 1
            If splitted_msg.Length > 4 Then
                stack_for_msg = splitted_msg(last_item - 2) & "." & splitted_msg(last_item - 1) & " " & splitted_msg(last_item)
            End If
            If stack_for_msg.Contains("(") Then
                stack_for_msg = splitted_msg(last_item)
            End If
            Return stack_for_msg
        End Function


        ''' <summary>
        ''' メモリ描画
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Memori(ByVal e As System.Windows.Forms.PaintEventArgs)
            If Not m_Memori.bIsDsp Then Exit Sub

            Dim ii As Integer
            Dim nPosWk As Integer
            Dim strValWk As String

            'm_AriaInfは、Height；長手側画面サイズ、Width；幅方向画面サイズであることに注意！
            Dim nPosLineTop As Integer
            If Not tProp_ImgKind_Rotate90 Then
                nPosLineTop = m_AriaInf.nAriaHeight - m_Memori.nPosLineBot
            Else
                nPosLineTop = m_AriaInf.nAriaWidth - m_Memori.nPosLineBot
            End If

            Dim nPosStringTop As Integer = nPosLineTop - m_Memori.nPosStringBot + m_Memori.nPosLineBot

            ' 幅軸メモリ位置計算
            Dim habaLabels As New List(Of tClass_ImageDspAxisDrawner.LabelInfo)

            If 0 < m_Memori.nLoopCntX Then
                ' メモリ
                For ii = 1 To m_Memori.nLoopCntX
                    nPosWk = CInt(ii * m_Memori.nIntervalX * m_CalcAns.RateRes.dW)
                    strValWk = CInt(ii * m_Memori.nIntervalX).ToString                                      ' 描画文字
                    habaLabels.Add(New tClass_ImageDspAxisDrawner.LabelInfo(nPosWk, strValWk))
                Next ii
            End If

            ' 長手軸メモリ位置計算
            Dim nagateLabels As New List(Of tClass_ImageDspAxisDrawner.LabelInfo)
            If 0 < m_Memori.nLoopCntY Then
                For ii = 1 To m_Memori.nLoopCntY
                    nPosWk = CInt(ii * m_Memori.nIntervalY * m_CalcAns.RateRes.dH)
                    strValWk = CInt(ii * m_Memori.nIntervalY).ToString                                      ' 描画文字
                    nagateLabels.Add(New tClass_ImageDspAxisDrawner.LabelInfo(nPosWk, strValWk))
                Next ii
            End If

            '準備ができたのでメモリを描画する
            Dim axis As New tClass_ImageDspAxisDrawner(e.Graphics, m_Memori.HojoLineLen, New Point(m_Memori.nPosLineLeft, nPosLineTop))
            axis.UpdateMemoriPenBrushFont(Me.m_memoriBsh, Me.m_memoriFnt, Me.m_memoriPen)
            axis.IsFlipV = True
            axis.IsStringOutside = True
            If Not tProp_ImgKind_Rotate90 Then
                axis.draw_x_axis(habaLabels, nPosLineTop)
                axis.draw_x_line(habaLabels, nPosLineTop)
                axis.draw_y_axis(nagateLabels, m_Memori.nPosLineLeft)
                axis.draw_y_line(nagateLabels, m_Memori.nPosLineLeft)
            Else
                axis.draw_x_axis(nagateLabels, nPosLineTop)
                axis.draw_x_line(nagateLabels, nPosLineTop)
                axis.draw_y_axis(habaLabels, m_Memori.nPosLineLeft)
                axis.draw_y_line(habaLabels, m_Memori.nPosLineLeft)
            End If

            ' 原点表示
            e.Graphics.DrawString(m_Memori.strBaseString, m_memoriFnt, m_memoriBsh, m_Memori.nPosStringLeft, nPosStringTop)
        End Sub



        ''' <summary>
        ''' 初期描画
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' 勝手に書き換えてOK
        End Sub

        ''' <summary>
        ''' 最終描画
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' 勝手に書き換えてOK
        End Sub

    End Class




    '*********************************************************************************
    '画像表示Baseクラス
    '	[Ver]
    '		Ver.01    2008/09/06  vs2005 対応
    '
    '	[メモ]
    '		ある意味疵検専用？？
    '*********************************************************************************
    ''' <summary>座標算出クラス</summary>
    Public Class tClass_ImageDsp_Calc
        'Implements IDisposable							' デストラクタ定義

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 定数定義
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "定数定義"
        ''' <summary>画像表示形式</summary>
        Public Enum EM_IMG_STRECH
            ''' <summary>0:等倍表示(縦横比保持。1画素1mm)</summary>
            emNormal = 0
            ''' <summary>1:表示エリアに画像を合わせる</summary>
            emStrechBoth
            ''' <summary>2:表示エリアの高さに画像高さを合わせる(縦横比保持)</summary>
            emStrechHeight
            ''' <summary>3:表示エリアの幅に画像幅を合わせる(縦横比保持)</summary>
            emStrechWidth
            ''' <summary>4:画像サイズの大きい方が表示領域に一致するように合わせる(縦横比保持)</summary>
            emJustBig
            ''' <summary>5:画像サイズの小さい方が表示領域に一致するように合わせる(縦横比保持)</summary>
            emJustSmall

            ''' <summary>6:生画像サイズ</summary>
            emRaw

            ''' <summary>7:横の分解能を基準として、縦をあわせる (縦横比保持)</summary>
            emRawWidth
            ''' <summary>8:縦の分解能を基準として、横をあわせる (縦横比保持)</summary>
            emRawHeight
        End Enum

        ''' <summary>倍率[10-2% (100%=1)]</summary>
        Public Structure RATE
            ''' <summary>横倍率</summary>
            Public dW As Double
            ''' <summary>縦倍率</summary>
            Public dH As Double

            ''' <summary>初期化</summary>
            Public Sub Initialize()
                dW = 1.0
                dH = 1.0
            End Sub
            Public Sub New(ByVal dWwk As Double, ByVal dHwk As Double)
                dW = dWwk
                dH = dHwk
            End Sub
        End Structure

        Public Structure SizeD
            Public Height As Double
            Public Width As Double
            Public Sub New(ByVal dWwk As Double, ByVal dHwk As Double)
                Width = dWwk
                Height = dHwk
            End Sub
        End Structure

        ''' <summary>画像情報</summary>
        Public Structure IMG_INF
            ''' <summary>1枚の画像 幅 [pixel]</summary>
            Public nOneImgW As Integer
            ''' <summary>1枚の画像 高さ [pixel]</summary>
            Public nOneImgH As Integer

            ''' <summary>表示画像枚数</summary>
            Public nImgCntX As Integer
            ''' <summary>表示画像枚数</summary>
            Public nImgCntY As Integer

            ''' <summary>合計 画像 幅 [pixel]</summary>
            Public nImgW As Integer
            ''' <summary>合計 画像 高さ [pixel]</summary>
            Public nImgH As Integer
            ''' <summary>画像 横分解能 [mm/pixel]</summary>
            Public dResW As Double
            ''' <summary>画像 縦分解能 [mm/pixel]</summary>
            Public dResH As Double

            ''' <summary>中心位置Y [pixel]</summary>
            Public nCenterY As Integer
            ''' <summary>中心位置X [pixel]</summary>
            Public nCenterX As Integer


            ''' <summary>画像 幅 [mm] (自動算出。1画素=1mmとなっている)</summary>
            Public dSizeW As Double
            ''' <summary>画像 高さ [mm] (自動算出。1画素=1mmとなっている)</summary>
            Public dSizeH As Double
        End Structure

        ''' <summary>表示形式情報</summary>
        Public Structure IMG_KIND
            ''' <summary>画像表示形式</summary>
            Public emImgStrech As EM_IMG_STRECH
            ''' <summary>画像表示倍率(自動算出)(分解能を加味した後の画像からの倍率)</summary>
            Public StrechRate As RATE

            ''' <summary>画像上下反転フラグ(Ture:反転,False:反転なし)</summary>
            Public bIsTurnTB As Boolean
            ''' <summary>画像左右反転フラグ(Ture:反転,False:反転なし)</summary>
            Public bIsTurnLR As Boolean

            ''' <summary>画像回転フラグ(Ture:回転,False:なし)</summary>
            Public bIsRotate90 As Boolean
        End Structure

        ''' <summary>表示領域情報</summary>
        Public Structure ARIA_INF
            ''' <summary>表示領域 高さ</summary>
            Public nAriaHeight As Integer
            ''' <summary>表示領域 幅</summary>
            Public nAriaWidth As Integer

            ''' <summary>表示倍率 [mm/pixel]</summary>
            Public Zoom As RATE

            ''' <summary>中心位置を表示エリアの中心に移動(Ture:中心へ移動,False:手動オフセット)</summary>
            Public bIsCenter As Boolean

            ''' <summary>オフセット位置 (bIsCenterで動作が変わる true時自動算出) [DP]</summary>
            Public Offset As Point
        End Structure

        ''' <summary>座標最終結果</summary>
        Public Structure CALC_ANS
            ''' <summary>最終分解能[mm/pixel] (表示している画像に対する分解能)</summary>
            Public FinalRes As RATE

            ''' <summary>最終倍率 [pixel/mm] (分解能を加味した後の画像からの倍率)</summary>
            Public RateRes As RATE

            ''' <summary>最終表示倍率 (生画像からの倍率)</summary>
            Public RateDsp As RATE

            ''' <summary>倍率反映後の全体の画像サイズ,左上の位置</summary>
            Public ImgDspRect As Rectangle


            '''' <summary>最終画像サイズ(見えていない部分も含む)</summary>
            'Public ImgSize As Size
            '''' <summary>表示画像サイズ(見えている部分だけ)</summary>
            'Public ImgDspSize As Size
            ''' <summary>表示エリアに表示されていない部分の画像サイズ</summary>
            Public ImgNonAriaSize As Size

        End Structure
#End Region




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>画像情報</summary>
        Protected m_ImgInf As IMG_INF                       ' 画像情報
        ''' <summary>表示形式情報</summary>
        Protected m_ImgKind As IMG_KIND                     ' 表示形式情報
        ''' <summary>表示領域情報</summary>
        Protected m_AriaInf As ARIA_INF                     ' 表示領域情報
        ''' <summary>座標最終結果</summary>
        Protected m_CalcAns As CALC_ANS                     ' 座標最終結果

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "画像情報"
        ''' <summary>
        ''' 座標最終結果
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-画像情報") _
         , System.ComponentModel.Description("画像情報")> _
        Public Property tProp_ImgInf() As IMG_INF
            Get
                Return m_ImgInf
            End Get
            Set(ByVal value As IMG_INF)
                m_ImgInf = value
            End Set
        End Property

#End Region

#Region "表示形式情報"
        ''' <summary>
        ''' 座標最終結果
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報") _
         , System.ComponentModel.Description("表示形式情報")> _
        Public Property tProp_ImgKind() As IMG_KIND
            Get
                Return m_ImgKind
            End Get
            Set(ByVal value As IMG_KIND)
                m_ImgKind = value
            End Set
        End Property


        ''' <summary>
        ''' 画像表示形式
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報-専用プロパティ") _
         , System.ComponentModel.Description("画像表示形式")> _
        Public Property tProp_ImgKind_StrechType() As EM_IMG_STRECH
            Get
                Return m_ImgKind.emImgStrech
            End Get
            Set(ByVal value As EM_IMG_STRECH)
                m_ImgKind.emImgStrech = value
            End Set
        End Property

        ''' <summary>
        ''' 画像上下反転フラグ(Ture:反転,False:反転なし)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報-専用プロパティ") _
         , System.ComponentModel.Description("画像上下反転フラグ(Ture:反転,False:反転なし)")> _
        Public Property tProp_ImgKind_IsTurnTB() As Boolean
            Get
                Return m_ImgKind.bIsTurnTB
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsTurnTB = value
            End Set
        End Property

        ''' <summary>
        ''' 画像左右反転フラグ(Ture:反転,False:反転なし)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報-専用プロパティ") _
         , System.ComponentModel.Description("画像左右反転フラグ(Ture:反転,False:反転なし)")> _
        Public Property tProp_ImgKind_IsTurnLR() As Boolean
            Get
                Return m_ImgKind.bIsTurnLR
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsTurnLR = value
            End Set
        End Property


        ''' <summary>
        ''' 画像９０度回転フラグ(True:コイルは横に流れる,False:コイルは縦に流れる)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報-専用プロパティ") _
         , System.ComponentModel.Description(" 画像９０度回転フラグ(True:コイルは横に流れる,False:コイルは縦に流れる)")> _
        Public Property tProp_ImgKind_Rotate90() As Boolean
            Get
                Return m_ImgKind.bIsRotate90
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsRotate90 = value
            End Set

        End Property



        ''' <summary>
        ''' 画像表示倍率(自動算出)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示形式情報-専用プロパティ") _
         , System.ComponentModel.Description("画像表示倍率(自動算出)")> _
        Public ReadOnly Property tProp_ImgKind_StrechRate() As RATE
            Get
                Return m_ImgKind.StrechRate
            End Get
        End Property
#End Region

#Region "表示領域情報"
        ''' <summary>
        ''' 表示領域情報
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報") _
         , System.ComponentModel.Description("表示領域情報")> _
        Public Property tProp_AriaInf() As ARIA_INF
            Get
                Return m_AriaInf
            End Get
            Set(ByVal value As ARIA_INF)
                m_AriaInf = value
            End Set
        End Property



        ''' <summary>
        ''' 表示領域
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示領域")> _
        Public Property tProp_AriaInf_Aria() As Size
            Get
                Dim s As Size
                s.Width = m_AriaInf.nAriaWidth
                s.Height = m_AriaInf.nAriaHeight
                Return s
                'Return New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight)
            End Get
            Set(ByVal value As Size)
                m_AriaInf.nAriaHeight = value.Height
                m_AriaInf.nAriaWidth = value.Width
            End Set
        End Property

        ''' <summary>
        ''' 表示領域 幅
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示領域 幅")> _
        Public WriteOnly Property tProp_AriaInf_AriaWidth() As Integer
            Set(ByVal value As Integer)
                m_AriaInf.nAriaWidth = value
            End Set
        End Property
        ''' <summary>
        ''' 表示領域 高さ
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示領域 高さ")> _
        Public WriteOnly Property tProp_AriaInf_AriaHeight() As Integer
            Set(ByVal value As Integer)
                m_AriaInf.nAriaHeight = value
            End Set
        End Property

        ''' <summary>
        ''' 表示倍率
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示倍率")> _
        Public Property tProp_AriaInf_Zoom() As RATE
            Get
                Return m_AriaInf.Zoom
            End Get
            Set(ByVal value As RATE)
                m_AriaInf.Zoom = value
            End Set
        End Property
        ''' <summary>
        ''' 表示倍率 横
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示倍率 横")> _
        Public Property tProp_AriaInf_ZoomW() As Double
            Get
                Return m_AriaInf.Zoom.dW
            End Get
            Set(ByVal value As Double)
                m_AriaInf.Zoom.dW = value
            End Set
        End Property
        ''' <summary>
        ''' 表示倍率 縦
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("表示倍率 縦")> _
        Public Property tProp_AriaInf_ZoomH() As Double
            Get
                Return m_AriaInf.Zoom.dH
            End Get
            Set(ByVal value As Double)
                m_AriaInf.Zoom.dH = value
            End Set
        End Property
        ''' <summary>
        ''' 中心位置を表示エリアの中心に移動(Ture:中心へ移動,False:手動オフセット)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("中心位置を表示エリアの中心に移動(Ture:中心へ移動,False:手動オフセット)")> _
        Public Property tProp_AriaInf_IsCenter() As Boolean
            Get
                Return m_AriaInf.bIsCenter
            End Get
            Set(ByVal value As Boolean)
                m_AriaInf.bIsCenter = value
            End Set
        End Property
        ''' <summary>
        ''' オフセット
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("オフセット")> _
        Public Property tProp_AriaInf_Offset() As Point
            Get
                Dim s As Point
                s.X = m_AriaInf.Offset.X
                s.Y = m_AriaInf.Offset.Y
                Return s
                'Return New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight)
            End Get
            Set(ByVal value As Point)
                m_AriaInf.Offset.X = value.X
                m_AriaInf.Offset.Y = value.Y
            End Set
        End Property
        ''' <summary>
        ''' オフセット
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("オフセットX")> _
        Public Property tProp_AriaInf_OffsetX() As Integer
            Get
                Return m_AriaInf.Offset.X
            End Get
            Set(ByVal value As Integer)
                m_AriaInf.Offset.X = value
            End Set
        End Property
        ''' <summary>
        ''' オフセット
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-表示領域情報-専用プロパティ") _
         , System.ComponentModel.Description("オフセットY")> _
        Public Property tProp_AriaInf_OffsetY() As Integer
            Get
                Return m_AriaInf.Offset.Y
            End Get
            Set(ByVal value As Integer)
                m_AriaInf.Offset.Y = value
            End Set
        End Property
#End Region

#Region "座標最終結果"
        ''' <summary>
        ''' 座標最終結果
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("☆座標算出クラス-座標最終結果-専用プロパティ") _
         , System.ComponentModel.Description("座標最終結果")> _
        Public ReadOnly Property tProp_CalcAns() As CALC_ANS
            Get
                Return m_CalcAns
            End Get
        End Property
#End Region

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            ' 大事なものだけ初期化
            m_AriaInf.Zoom.dH = 1.0
            m_AriaInf.Zoom.dW = 1.0
        End Sub


        ''' <summary>
        ''' 画像情報 セット
        ''' </summary>
        ''' <param name="nWidth">画像 幅 [pixel]</param>
        ''' <param name="nHeight">画像 高さ [pixel]</param>
        ''' <param name="dResW">画像 横分解能 [mm/pixel]</param>
        ''' <param name="dResH">画像 縦分解能 [mm/pixel]</param>
        ''' <param name="nCenterX">中心位置X [pixel] (中心位置モードで無い場合は、0でOK)</param>
        ''' <param name="nCenterY">中心位置Y [pixel] (中心位置モードで無い場合は、0でOK)</param>
        ''' <param name="nImgCntX">画像 表示枚数</param>
        ''' <param name="nImgCntY">画像 表示枚数</param>
        ''' <remarks></remarks>
        Public Sub SetImgInf(ByVal nWidth As Integer, ByVal nHeight As Integer, _
           ByVal dResW As Double, ByVal dResH As Double, _
           Optional ByVal nCenterX As Integer = 0, Optional ByVal nCenterY As Integer = 0, _
           Optional ByVal nImgCntX As Integer = 1, Optional ByVal nImgCntY As Integer = 1)
            ' 基本情報セット
            m_ImgInf.nOneImgW = nWidth              ' 1枚の画像のサイズ
            m_ImgInf.nOneImgH = nHeight
            m_ImgInf.nImgCntX = nImgCntX
            m_ImgInf.nImgCntY = nImgCntY
            m_ImgInf.nImgW = m_ImgInf.nOneImgW * m_ImgInf.nImgCntX      ' 全画像を考慮したサイズ
            m_ImgInf.nImgH = m_ImgInf.nOneImgH * m_ImgInf.nImgCntY
            m_ImgInf.dResW = dResW
            m_ImgInf.dResH = dResH
            m_ImgInf.nCenterX = nCenterX
            m_ImgInf.nCenterY = nCenterY


            ' 分解能を反映
            m_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH       ' 画像高さ [mm]
            m_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW       ' 画像幅 [mm]
        End Sub

        ''' <summary>
        ''' 画像表示枚数 変更 (画像表示枚数の変更のみ)
        ''' </summary>
        ''' <param name="nImgCntX">画像 表示枚数</param>
        ''' <param name="nImgCntY">画像 表示枚数</param>
        ''' <remarks></remarks> 
        Public Sub ChangeImgCnt(Optional ByVal nImgCntX As Integer = 1, Optional ByVal nImgCntY As Integer = 1)
            m_ImgInf.nImgCntX = nImgCntX
            m_ImgInf.nImgCntY = nImgCntY
            ' 関係先 変更
            m_ImgInf.nImgW = m_ImgInf.nOneImgW * m_ImgInf.nImgCntX      ' 全画像を考慮したサイズ
            m_ImgInf.nImgH = m_ImgInf.nOneImgH * m_ImgInf.nImgCntY

            m_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH       ' 画像高さ [mm]
            m_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW       ' 画像幅 [mm]
        End Sub

        ''' <summary>
        ''' 座標データ計算 (メイン処理)
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Calculation()

            '// =========================================
            '// 画像サイズ関係
            '//

            '' 最終表示倍率決定
            '' 分解能を反映   負荷軽減の為ヤメ
            'm_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW		' 画像幅 [mm]
            'm_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH		' 画像高さ [mm]

            ' 表示形式を反映
            Call SetStrechRate()
            ' 最終倍率を反映
            m_CalcAns.RateRes.dW = m_AriaInf.Zoom.dW * m_ImgKind.StrechRate.dW
            m_CalcAns.RateRes.dH = m_AriaInf.Zoom.dH * m_ImgKind.StrechRate.dH
            ' 表示倍率を反映
            m_CalcAns.RateDsp.dW = m_CalcAns.RateRes.dW * m_ImgInf.dResW
            m_CalcAns.RateDsp.dH = m_CalcAns.RateRes.dH * m_ImgInf.dResH
            ' 最終分解能
            'm_CalcAns.FinalRes.dW = m_AriaInf.Zoom.dW * m_CalcAns.FinalRes.dW
            'm_CalcAns.FinalRes.dH = m_AriaInf.Zoom.dH * m_CalcAns.FinalRes.dH
            m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dW / m_AriaInf.Zoom.dW   ' Ver 1.2.0.0 で修正
            m_CalcAns.FinalRes.dH = m_CalcAns.FinalRes.dH / m_AriaInf.Zoom.dH

            '' 倍率反映後の画像サイズ
            m_CalcAns.ImgDspRect.X = 0              ' 表示位置は後で計算する為、とりあえず0クリア
            m_CalcAns.ImgDspRect.Y = 0
            m_CalcAns.ImgDspRect.Width = CInt(m_ImgInf.nImgW * m_CalcAns.RateDsp.dW)
            m_CalcAns.ImgDspRect.Height = CInt(m_ImgInf.nImgH * m_CalcAns.RateDsp.dH)


            '' 表示エリアに表示されていない部分の画像サイズ
            '    スクロールバーのMaxValとかに使用できる
            m_CalcAns.ImgNonAriaSize.Width = m_CalcAns.ImgDspRect.Width - m_AriaInf.nAriaWidth
            If 0 > m_CalcAns.ImgNonAriaSize.Width Then m_CalcAns.ImgNonAriaSize.Width = 0

            m_CalcAns.ImgNonAriaSize.Height = m_CalcAns.ImgDspRect.Height - m_AriaInf.nAriaHeight
            If 0 > m_CalcAns.ImgNonAriaSize.Height Then m_CalcAns.ImgNonAriaSize.Height = 0


            '// =========================================
            '// 画像表示位置関係
            '//

            '' 中心位置を表示エリアの中心に移動
            If m_AriaInf.bIsCenter Then
                ' 中心位置が 表示エリアの中央に来るように座標を計算
                SetCenterPosition()

                ' オフセット位置 自動計算
                '   スクロールバーのValueとかに使用できる
                m_AriaInf.Offset.X = m_CalcAns.ImgDspRect.X
                'If 0 > m_AriaInf.Offset.X Then m_AriaInf.Offset.X = 0
                m_AriaInf.Offset.Y = m_CalcAns.ImgDspRect.Y
                'If 0 > m_AriaInf.Offset.Y Then m_AriaInf.Offset.Y = 0

            Else
                ' オフセット量だけ 画像位置をずらす
                SetOffsetPosition()

            End If

        End Sub

        ''' <summary>
        ''' 中心位置が 表示エリアの中央に来るように座標を計算
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub SetCenterPosition()
            Dim nCenterX As Integer     ' 中心位置X
            Dim nCenterY As Integer     ' 中心位置Y
            Dim Pos As Point            ' 左上の位置

            '' 左右反転
            If m_ImgKind.bIsTurnLR Then
                nCenterX = CInt((m_ImgInf.nImgW - m_ImgInf.nCenterX) * m_CalcAns.RateDsp.dW)
            Else
                nCenterX = CInt(m_ImgInf.nCenterX * m_CalcAns.RateDsp.dW)
            End If

            '' 上下反転
            If m_ImgKind.bIsTurnTB Then
                nCenterY = CInt((m_ImgInf.nImgH - m_ImgInf.nCenterY) * m_CalcAns.RateDsp.dH)
            Else
                nCenterY = CInt(m_ImgInf.nCenterY * m_CalcAns.RateDsp.dH)
            End If

            '' 中心位置から、表示エリアを算出
            ' 横
            If m_CalcAns.ImgDspRect.Width < m_AriaInf.nAriaWidth Then                           ' そもそも 表示エリアより 画像が小さい
                Pos.X = CInt((m_AriaInf.nAriaWidth / 2) - (m_CalcAns.ImgDspRect.Width / 2))

            ElseIf (m_AriaInf.nAriaWidth / 2) > nCenterX Then                                   ' 左端限界
                Pos.X = 0

            ElseIf (m_AriaInf.nAriaWidth / 2) > (m_CalcAns.ImgDspRect.Width - nCenterX) Then    ' 右端限界
                Pos.X = m_AriaInf.nAriaWidth - m_CalcAns.ImgDspRect.Width

            Else                                                                                ' 中心位置が表示エリアの中心へ
                Pos.X = CInt((m_AriaInf.nAriaWidth / 2) - nCenterX)
            End If

            ' 縦
            If m_CalcAns.ImgDspRect.Height < m_AriaInf.nAriaHeight Then                         ' そもそも 表示エリアより 画像が小さい
                Pos.Y = CInt((m_AriaInf.nAriaHeight / 2) - (m_CalcAns.ImgDspRect.Height / 2))

            ElseIf (m_AriaInf.nAriaHeight / 2) > nCenterY Then                                  ' 左端限界
                Pos.Y = 0

            ElseIf (m_AriaInf.nAriaHeight / 2) > (m_CalcAns.ImgDspRect.Height - nCenterY) Then  ' 右端限界
                Pos.Y = m_AriaInf.nAriaHeight - m_CalcAns.ImgDspRect.Height

            Else                                                                                ' 中心位置が表示エリアの中心へ
                Pos.Y = CInt((m_AriaInf.nAriaHeight / 2) - nCenterY)
            End If

            '' 結果セット
            m_CalcAns.ImgDspRect.X = Pos.X
            m_CalcAns.ImgDspRect.Y = Pos.Y
        End Sub

        ''' <summary>
        ''' オフセット量だけ 画像位置をずらす
        ''' </summary>
        ''' <remarks></remarks>
        Protected Sub SetOffsetPosition()
            Dim Pos As Point            ' 左上の位置

            Pos.X = m_AriaInf.Offset.X
            Pos.Y = m_AriaInf.Offset.Y

            '' 結果セット
            m_CalcAns.ImgDspRect.X = Pos.X
            m_CalcAns.ImgDspRect.Y = Pos.Y
        End Sub



        ''' <summary>
        ''' 画像表示倍率セット
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub SetStrechRate()
            ' 画像表示形式により画像表示倍率を設定
            Select Case m_ImgKind.emImgStrech
                Case EM_IMG_STRECH.emNormal                         ' 等倍表示 (1画素=1mm)
                    m_ImgKind.StrechRate.dH = 1.0
                    m_ImgKind.StrechRate.dW = 1.0

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = 1.0
                    m_CalcAns.FinalRes.dW = 1.0

                Case EM_IMG_STRECH.emRawWidth
                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH) * (m_ImgInf.dResH / m_ImgInf.dResW)

                    '最終分解能は画像分解能を使用(虫眼鏡機能スケール表示に影響)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResW
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResW

                Case EM_IMG_STRECH.emRawHeight
                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW) * (m_ImgInf.dResW / m_ImgInf.dResH)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH)

                    '最終分解能は画像分解能を使用(虫眼鏡機能スケール表示に影響)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResH
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResH

                Case EM_IMG_STRECH.emRaw    ' 生画像

                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH)

                    '最終分解能は画像分解能を使用(虫眼鏡機能スケール表示に影響)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResH
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResW




                Case EM_IMG_STRECH.emStrechBoth                     ' 表示エリアに画像を合わせる
                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaHeight / m_ImgInf.dSizeH
                    m_ImgKind.StrechRate.dW = m_AriaInf.nAriaWidth / m_ImgInf.dSizeW

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeH / m_AriaInf.nAriaHeight
                    m_CalcAns.FinalRes.dW = m_ImgInf.dSizeW / m_AriaInf.nAriaWidth


                Case EM_IMG_STRECH.emStrechHeight                   ' 表示エリアの高さに画像高さを合わせる
                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaHeight / m_ImgInf.dSizeH
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeH / m_AriaInf.nAriaHeight
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH


                Case EM_IMG_STRECH.emStrechWidth                    ' 表示エリアの幅に画像幅を合わせる

                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaWidth / m_ImgInf.dSizeW
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeW / m_AriaInf.nAriaWidth
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH

                Case EM_IMG_STRECH.emJustBig                        ' 画像サイズの大きい方が表示領域に一致するように合わせる
                    m_ImgKind.StrechRate.dH = CDbl(IIf(m_ImgInf.dSizeH > m_ImgInf.dSizeW, m_AriaInf.nAriaHeight / m_ImgInf.dSizeH, m_AriaInf.nAriaWidth / m_ImgInf.dSizeW))
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = CDbl(IIf(m_ImgInf.dSizeH > m_ImgInf.dSizeW, m_ImgInf.dSizeH / m_AriaInf.nAriaHeight, m_ImgInf.dSizeW / m_AriaInf.nAriaWidth))
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH

                Case EM_IMG_STRECH.emJustSmall                      ' 画像サイズの小さい方が表示領域に一致するように合わせる
                    m_ImgKind.StrechRate.dH = CDbl(IIf(m_ImgInf.dSizeH < m_ImgInf.dSizeW, m_AriaInf.nAriaHeight / m_ImgInf.dSizeH, m_AriaInf.nAriaWidth / m_ImgInf.dSizeW))
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' 最終分解能
                    m_CalcAns.FinalRes.dH = CDbl(IIf(m_ImgInf.dSizeH < m_ImgInf.dSizeW, m_ImgInf.dSizeH / m_AriaInf.nAriaHeight, m_ImgInf.dSizeW / m_AriaInf.nAriaWidth))
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH



            End Select
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 座標計算
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 元の画像座標から表示用の座標に変換 (反転・オフセット等を考慮) [元pixel]→[表示pixel]
        ''' </summary>
        ''' <param name="x">元画像X座標</param>
        ''' <param name="y">元画像Y座標</param>
        ''' <returns>表示座標</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetPosition(ByVal x As Integer, ByVal y As Integer) As Point

            '' 左右反転
            'If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x

            '' 上下反転
            'If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y
            ' 左右反転
            If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x - 1

            ' 上下反転
            If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y - 1



            ' 表示用座標に変換 
            x = CInt(x * m_CalcAns.RateDsp.dW)
            y = CInt(y * m_CalcAns.RateDsp.dH)

            If m_ImgKind.bIsRotate90 Then
                '回転時の考慮事項：
                '　１．表示座標系の原点は、元画像からすると右側にある
                x = m_CalcAns.ImgDspRect.Width - x
            End If

            '' オフセット考慮
            x += m_AriaInf.Offset.X
            y += m_AriaInf.Offset.Y
            Return New Point(x, y)
        End Function
        Public Overloads Function GetPosition(ByVal pos As Point) As Point
            Return GetPosition(pos.X, pos.Y)
        End Function


        ''' <summary>
        ''' 元の画像座標から表示用の座標に変換 [元pixel]→[表示pixel]
        ''' </summary>
        ''' <param name="x"></param>
        ''' <param name="y"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal x As Integer, ByVal y As Integer) As Point
            ' 表示用座標に変換 ＋ オフセット考慮
            x = CInt(x * m_CalcAns.RateDsp.dW)
            y = CInt(y * m_CalcAns.RateDsp.dH)

            Return New Point(x, y)
        End Function
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal pos As Point) As Point
            Return GetSimplePoint_ImgToDsp(pos.X, pos.Y)
        End Function
        Public Overloads Function GetPos_RpToDp_X(ByVal x As Integer) As Integer
            Return CInt(x * m_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_RpToDp_Y(ByVal y As Integer) As Integer
            Return CInt(y * m_CalcAns.RateDsp.dH)
        End Function


        ''' <summary>
        ''' 表示用の座標から元の画像座標に変換 (反転・オフセット等を考慮) [表示pixel]→[元pixel]
        ''' </summary>
        ''' <param name="dx">表示座標X座標</param>
        ''' <param name="dy">表示座標Y座標</param>
        ''' <returns>元の画像座標</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetDspPixelToImgPosition(ByVal dx As Integer, ByVal dy As Integer) As Point
            ' オフセットを考慮
            Dim x As Integer = dx - m_AriaInf.Offset.X
            Dim y As Integer = dy - m_AriaInf.Offset.Y
            If m_ImgKind.bIsRotate90 Then
                '回転時の考慮事項：
                '　１．表示座標系の原点は、元画像からすると右側にある
                '　２．表示座標系は、回転しているので、 X:長手、Y：幅
                '　３．m_AriaInfのOffset(スクロール量)は、X:幅　、Y：長手
                x = m_CalcAns.ImgDspRect.Width - (dy - m_AriaInf.Offset.X) - 1
                y = dx - m_AriaInf.Offset.Y
            End If

            ' 表示用座標 から 元画像座標 に変換
            x = CInt(x / m_CalcAns.RateDsp.dW)
            y = CInt(y / m_CalcAns.RateDsp.dH)

            '' 左右反転
            'If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x
            '' 上下反転
            'If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y
            ' 左右反転
            If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x - 1
            ' 上下反転
            If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y - 1


            Return New Point(x, y)

        End Function
        Public Overloads Function GetDspPixelToImgPosition(ByVal pos As Point) As Point
            Return GetDspPixelToImgPosition(pos.X, pos.Y)
        End Function

        ''' <summary>
        ''' 表示用の座標から元の画像座標に変換 [表示pixel]→[元pixel]
        ''' </summary>
        ''' <param name="x"></param>
        ''' <param name="y"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetSimplePoint_DspToImg(ByVal x As Integer, ByVal y As Integer) As Point
            ' 表示用座標 から 元画像座標 に変換
            x = CInt(x / m_CalcAns.RateDsp.dW)
            y = CInt(y / m_CalcAns.RateDsp.dH)

            Return New Point(x, y)
        End Function
        Public Overloads Function GetSimplePoint_DspToImg(ByVal pos As Point) As Point
            Return GetSimplePoint_DspToImg(pos.X, pos.Y)
        End Function
        Public Overloads Function GetPos_DpToRp_X(ByVal x As Integer) As Integer
            Return CInt(x / m_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_DpToRp_Y(ByVal y As Integer) As Integer
            Return CInt(y / m_CalcAns.RateDsp.dH)
        End Function



        ''' <summary>
        ''' 最終的な個々の画像サイズ (全部一緒とは限らない)
        ''' </summary>
        ''' <param name="RecX">算出したい画像のX表示位置 (0オリジン)</param>
        ''' <param name="RecY">算出したい画像のY表示位置 (0オリジン)</param>
        ''' <remarks>GDI+の縮小は、行列のかかり具合などによって、ときどき-1されるので、
        ''' 幅・長さはそれを見越して適当に１～２px大きくなってる</remarks>
        Public Function GetAnsOneImgSize(ByVal RecX As Integer, ByVal RecY As Integer) As Rectangle
            Dim ans As Rectangle                        ' 最終結果
            Dim posNowX, posNowY As Double             ' 今回の開始位置
            Dim posNextX, posNextY As Double ' 次回の開始位置


            '' 今回と次回の表示位置 を算出
            ' 縦方向
            If True AndAlso m_ImgKind.bIsTurnTB Then
                posNowY = m_ImgInf.nImgH - (RecY * m_ImgInf.nOneImgH)
                posNextY = m_ImgInf.nImgH - ((RecY + 1) * m_ImgInf.nOneImgH)
            Else
                posNowY = RecY * m_ImgInf.nOneImgH
                posNextY = (RecY + 1) * m_ImgInf.nOneImgH
            End If
            posNowY = (posNowY * m_CalcAns.RateDsp.dH + m_CalcAns.ImgDspRect.Y)
            posNextY = (posNextY * m_CalcAns.RateDsp.dH + m_CalcAns.ImgDspRect.Y)


            ' 表示位置
            If True AndAlso m_ImgKind.bIsTurnLR Then
                posNowX = m_ImgInf.nImgW - (RecX * m_ImgInf.nOneImgW)
                posNextX = m_ImgInf.nImgW - ((RecX + 1) * m_ImgInf.nOneImgW)
            Else
                posNowX = RecX * m_ImgInf.nOneImgW
                posNextX = (RecX + 1) * m_ImgInf.nOneImgW
            End If
            posNowX = (posNowX * m_CalcAns.RateDsp.dW + m_CalcAns.ImgDspRect.X)
            posNextX = (posNextX * m_CalcAns.RateDsp.dW + m_CalcAns.ImgDspRect.X)

            '' 決定データセット
            ans.X = CInt(Math.Floor(Math.Min(posNowX, posNextX)))
            ans.Y = CInt(Math.Floor(Math.Min(posNowY, posNextY)))

            '' 幅決め
            ans.Width = CInt(Math.Floor(Math.Abs(posNextX - posNowX))) + 1 + CInt(IIf(m_ImgKind.bIsTurnLR, 1, 0))
            ans.Height = CInt(Math.Floor(Math.Abs(posNextY - posNowY))) + 1 + CInt(IIf(m_ImgKind.bIsTurnTB, 1, 0))

            'If False Then
            '    ans.X = posNowX
            '    ans.Y = posNowY

            '    '' 幅決め
            '    ans.Width = (posNextX - posNowX) + CInt(IIf(m_ImgKind.bIsTurnLR, -1, 0))
            '    ans.Height = (posNextY - posNowY) + CInt(IIf(m_ImgKind.bIsTurnTB, -1, 0))
            'End If

            Return ans
        End Function




        ''' <summary>
        ''' [表示pixel]→[mm]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_Y(ByVal v As Integer) As Single
            Return CSng(v * m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_DpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * m_CalcAns.FinalRes.dW)
        End Function


        ''' <summary>
        ''' [mm]→[表示pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Double) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Integer) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Double) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dW)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Integer) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dW)
        End Function

        ''' <summary>
        ''' [元pixel]→[mm]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_Y(ByVal v As Integer) As Single
            Return CSng((v * m_CalcAns.RateDsp.dH) * m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_RpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function

        ''' <summary>
        ''' [mm]→[元pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Double) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dH * m_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Integer) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dH * m_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Double) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Integer) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function
    End Class


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 子クラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' メモリ軸描画ヘルパ
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_ImageDspAxisDrawner
        Implements IDisposable

        ''' <summary>
        ''' メモリを書く座標と、そのラベルのペア
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure LabelInfo
            Public pos As Integer
            Public label As String
            Sub New(ByVal p As Integer, ByVal l As String)
                pos = p
                label = l
            End Sub
            Public Overrides Function ToString() As String
                Return String.Format("{0}:{1}", pos, label)
            End Function
        End Structure

        Dim g As Graphics
        Dim m_Bsh As Brush = New SolidBrush(Color.Cyan)
        Dim m_Fnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim m_Pen As Pen = New System.Drawing.Pen(Color.Cyan, 2)

        Public Sub Dispose() Implements IDisposable.Dispose
            m_Bsh.Dispose()
            m_Bsh = Nothing
            m_Fnt.Dispose()
            m_Fnt = Nothing
            m_Pen.Dispose()
            m_Pen = Nothing
        End Sub

        ''' <summary>
        ''' 描画に使用するフォント・ブラシ・ペンを変更する
        ''' </summary>
        ''' <param name="bsh">フォント</param>
        ''' <param name="fnt">ブラシ</param>
        ''' <param name="pen">ペン</param>
        ''' <remarks>前に使ったてたものはDisposeされる</remarks>
        Sub UpdateMemoriPenBrushFont(ByVal bsh As Brush, ByVal fnt As Font, ByVal pen As Pen)
            Dispose()

            m_Bsh = bsh
            m_Fnt = fnt
            m_Pen = pen
        End Sub


        Dim m_ptOffset As Point     '原点
        Dim m_axisLength As Integer 'メモリ軸長

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="g">描画対象</param>
        ''' <param name="axisLength">軸に垂直な長さ</param>
        ''' <param name="ptOffset">原点</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal g As Graphics, ByVal axisLength As Integer, ByVal ptOffset As Point)
            Me.g = g
            Me.m_ptOffset = ptOffset
            Me.m_axisLength = axisLength
        End Sub

        ''' <summary>
        ''' 文字列の描画サイズを求める
        ''' </summary>
        ''' <param name="s">文字列</param>
        ''' <returns>グラフィック上のサイズ</returns>
        ''' <remarks></remarks>
        Function MeasureString(ByVal s As String) As SizeF
            Return g.MeasureString(s, m_Fnt, 0, Nothing)
        End Function


        'ラベルの文字は補助線の中心に来るべきか
        Public IsCenterV As Boolean = True

        'Y軸のLabelInfoの数値を、反転するか
        Public IsFlipV As Boolean = False

        '目盛ラベルがラインの外側につくか。
        Public IsStringOutside As Boolean = False

        ''' <summary>
        ''' Y軸メモリ線の連中を描画する
        ''' </summary>
        ''' <param name="labels">ラベル群</param>
        ''' <param name="basepos">縦線の位置（X）</param>
        ''' <remarks></remarks>
        Public Sub draw_y_axis(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            For Each label As LabelInfo In labels
                Dim yy As Integer = m_ptOffset.Y + label.pos
                If IsFlipV Then yy = m_ptOffset.Y - label.pos

                '目盛線長
                Dim axisLength As Integer = m_axisLength
                If Not String.IsNullOrEmpty(label.label) Then
                    axisLength = axisLength * 3 \ 2
                End If

                '描画文字列のサイズ
                Dim box As SizeF = MeasureString(label.label)

                If basepos > 100 Then
                    '右側のY軸なので、目盛線はベースから左に突き出す
                    g.DrawLine(m_Pen, basepos - axisLength, yy, basepos, yy)

                    '文字サイズ分は左に来るべき
                    Dim xx As Integer = CInt(basepos - box.Width)
                    If IsCenterV Then
                        'センタリング時は、目盛線とかぶるのでXを離す
                        xx = xx - m_axisLength - 10
                        'Yは中心へ
                        yy = yy - CInt(box.Height / 2)
                    End If

                    If IsStringOutside Then
                        '外側（ベースから右）に文字を出す場合
                        xx = basepos
                    End If

                    g.DrawString(label.label, m_Fnt, m_Bsh, xx, yy + 2)
                Else
                    '左側のY軸なので、目盛線はベースから右へ出る
                    Dim xx As Integer = basepos
                    g.DrawLine(m_Pen, basepos, yy, basepos + axisLength, yy)
                    If IsCenterV Then
                        'センタリング時は、目盛線とかぶるのでXを離す
                        xx = basepos + 10 + axisLength
                        yy = yy - CInt(box.Height / 2)
                    End If
                    If IsStringOutside Then
                        '外側（ベースから左）に文字を出す
                        xx = basepos - CInt(box.Width)
                    End If

                    g.DrawString(label.label, m_Fnt, m_Bsh, xx, yy + 2)
                End If
            Next
        End Sub

        ''' <summary>
        ''' Y軸の軸線を描画する。原点～labelsの最大まで直線で結ぶ
        ''' </summary>
        ''' <param name="labels"></param>
        ''' <param name="basepos"></param>
        ''' <remarks></remarks>
        Public Sub draw_y_line(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            Dim max_pos As Integer = 0
            For Each label As LabelInfo In labels
                max_pos = Math.Max(label.pos, max_pos)
            Next

            Dim yy As Integer = m_ptOffset.Y + max_pos
            If IsFlipV Then yy = m_ptOffset.Y - max_pos
            g.DrawLine(m_Pen, basepos, m_ptOffset.Y, basepos, yy)

        End Sub


        ''' <summary>
        ''' X軸メモリ線の連中を描画する
        ''' </summary>
        ''' <param name="labels">ラベル群</param>
        ''' <param name="basepos">横線の位置（Y）</param>
        ''' <remarks></remarks>
        Public Sub draw_x_axis(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            For Each label As LabelInfo In labels
                Dim xx As Integer = m_ptOffset.X + label.pos
                Dim axisLength As Integer = m_axisLength
                If Not String.IsNullOrEmpty(label.label) Then
                    axisLength = axisLength * 3 \ 2
                End If

                Dim box As SizeF = MeasureString(label.label)
                Dim lx As Integer = xx
                If IsCenterV Then
                    'センタリング
                    lx = xx - CInt(box.Width / 2)
                End If

                If basepos > 100 Then
                    '下につくX軸。目盛線はベースから上へ
                    g.DrawLine(m_Pen, xx, basepos, xx, basepos - axisLength)

                    '目盛線のさらに上に文字列を配置
                    Dim yy As Integer = CInt(basepos - m_axisLength - box.Height)
                    If IsStringOutside Then
                        '外側（ベースの下）に配置しなおし
                        yy = basepos + 1
                    End If

                    g.DrawString((label.label), m_Fnt, m_Bsh, lx, yy)
                Else
                    '上につくX軸。目盛線はベースから下へ
                    g.DrawLine(m_Pen, xx, basepos, xx, basepos + axisLength)


                    'デフォルトは目盛線のさらに下
                    Dim yy As Integer = CInt(basepos + m_axisLength)
                    If IsStringOutside Then
                        '外側（ベースの下）に配置しなおし
                        yy = basepos + 1
                    End If
                    g.DrawString((label.label), m_Fnt, m_Bsh, lx, yy)
                End If
            Next
        End Sub

        ''' <summary>
        ''' Y軸の軸線を描画する。原点～labelsの最大まで直線で結ぶ
        ''' </summary>
        ''' <param name="labels"></param>
        ''' <param name="basepos"></param>
        ''' <remarks></remarks>
        Public Sub draw_x_line(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            Dim max_pos As Integer = 0
            For Each label As LabelInfo In labels
                max_pos = Math.Max(label.pos, max_pos)
            Next
            g.DrawLine(m_Pen, m_ptOffset.X, basepos, m_ptOffset.X + max_pos, basepos)
        End Sub
    End Class

End Namespace