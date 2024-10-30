'*********************************************************************************
'ピクチャーボックスの元画像を部分的に拡大表示するクラス
'	[Ver]
'		Ver.01    2008/11/21  vs2005 対応
'		Ver.02	  2015/11/2   部分拡大中に左クリックでクリップボードへのコピー機能追加
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Windows.Forms
Imports System.Drawing

Imports tClassLibrary.tClass


Namespace tClass

    ''' <summary>
    ''' ピクチャーボックスの画像を部分的に拡大表示するクラス
    ''' </summary>
    Public Class tClass_ImageRawCutDsp
        Implements IDisposable                          ' デストラクタ定義

        Public Shared ReadOnly MAX_WINDOW_SIZE As Integer = 1000
        Public Shared ReadOnly MAX_ZOOM As Double = 20.0
        Public Shared ReadOnly MIN_ZOOM As Double = 1.0

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ イベント
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 表示データ
        ''' </summary>
        ''' <remarks></remarks>
        Public Event tEv_DispDataSet(ByRef tCls_Calc As tClass_ImageDsp_Calc, ByRef img() As Image)


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 定数定義
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>メモリ設定情報</summary>
        Public Structure MEMORI_INF
            ''' <summary>倍率表示有無 (true:有, false:無)</summary>
            Dim bIsDspRitu As Boolean
            ''' <summary>メモリ表示有無 (true:有, false:無)</summary>
            Dim bIsDspMemori As Boolean
            ''' <summary>表示色</summary>
            Dim Color As Color

            ''' <summary>X軸のメモリ表示間隔[pixel]</summary>
            Dim nInteralX As Integer
            ''' <summary>Y軸のメモリ表示間隔[pixel]</summary>
            Dim nInteralY As Integer


            ''' <summary>デフォルト状態設定</summary>
            Public Sub initialize()
                bIsDspRitu = True
                bIsDspMemori = True
                Color = Drawing.Color.Cyan
                nInteralX = 80
                nInteralY = 80
            End Sub
        End Structure


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        'Private m_BaseFrm As Form                  ' ベースフォーム
        Private m_BaseFrm As Control                ' ベースフォーム
        Private m_BasePic As PictureBox             ' ベースピクチャーボックス
        Private m_pic As PictureBox                 ' 拡大表示エリア

        Private m_bDspStop As Boolean = False       ' 拡大表示機能一時停止
        Private m_bDspOn As Boolean = False         ' 拡大表示中
        Private m_dDspRitu As Double                ' 拡大率
        Private m_MemoriInf As MEMORI_INF           ' メモリ情報


        '''' 一回限りのデータ
        Private mcls_pCalc As tClass_ImageDsp_Calc  ' 画像計算クラス (ポインタ)
        Private m_pDspImg() As Image                        ' 生画像配列


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////


        ''' <summary>
        ''' 拡大表示機能一時停止
        ''' </summary>
        Public Property DspStop() As Boolean
            Get
                Return m_bDspStop
            End Get
            Set(ByVal value As Boolean)

                ' 拡大表示中なら、それは中断する
                If value And m_bDspOn Then
                    Call BasePic_MouseUp(Nothing, Nothing)
                End If

                ' 中断した後に、値を変更する
                m_bDspStop = value
            End Set
        End Property

        ''' <summary>
        ''' 拡大表示エリアを表示中時true
        ''' </summary>
        ''' <value></value>
        Public ReadOnly Property IsDsp() As Boolean
            Get
                Return m_bDspOn
            End Get
        End Property

        ''' <summary>
        ''' 拡大画像エリアサイズ
        ''' </summary>
        Public Property DispSize() As Size
            Get
                Return m_pic.Size
            End Get
            Set(ByVal value As Size)
                m_pic.Size = value
            End Set
        End Property

        ''' <summary>
        ''' メモリ情報アクセス
        ''' </summary>
        Public Property MemoriInf() As MEMORI_INF
            Get
                Return m_MemoriInf
            End Get
            Set(ByVal value As MEMORI_INF)
                m_MemoriInf = value
            End Set
        End Property

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開 処理
        '/ /////////////////////////////////////////////////////////////////////////////////

        Private m_DefaultSize As Size = New Size(200, 200)
        Private m_nDefRitu As Double = 1.0

        Public Comment As String = String.Empty

        Public Property DefaultSize As Size
            Get
                Return m_DefaultSize
            End Get
            Set(value As Size)
                m_DefaultSize = value
            End Set
        End Property

        Public Property DefaultZoom As Double
            Get
                Return m_nDefRitu
            End Get
            Set(value As Double)
                m_nDefRitu = value
            End Set
        End Property

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="frm">ベースフォーム</param>
        ''' <param name="pic">拡大対象のピクチャーボックス</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal frm As Control, ByVal pic As PictureBox, Optional ByVal WindowSize As Integer = 200)
            'Public Sub New(ByRef frm As Form, ByRef pic As PictureBox)
            '''' データをセット
            m_BaseFrm = frm
            m_BasePic = pic
            DefaultSize = New Size(WindowSize, WindowSize)


            '''' 拡大表示エリアを生成
            m_pic = New PictureBox
            m_pic.Size = DefaultSize
            m_pic.Location = New Point(0, 0)
            m_pic.BorderStyle = BorderStyle.FixedSingle
            m_pic.Visible = False
            m_pic.Enabled = False                               ' 虫眼鏡表示中にフォーカスをとらないようにするための対策

            m_BaseFrm.Controls.Add(m_pic)                       ' フレーム画像表示ピクチャーの配下にする
            m_pic.BringToFront()                                ' 親ピクチャーの上に持ってくる

            '''' その他もろもろ初期化


            'm_MemoriInf.bIsDspRitu = m_MemoriInf.bIsDspMemori = True

            '''' イベント追加
            AddHandler m_pic.Paint, AddressOf pic_Paint
            AddHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
            AddHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
            AddHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
            AddHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela


        End Sub


#Region " IDisposable Support "

        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにマネージ リソースを解放します
                End If

                '''' 共有のアンマネージ リソースを解放します

                '''' イベント開放
                RemoveHandler m_pic.Paint, AddressOf pic_Paint
                RemoveHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
                RemoveHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
                RemoveHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
                RemoveHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela

                '''' 拡大表示エリア 開放
                m_BaseFrm.Controls.Remove(m_pic)
                m_pic.Dispose()

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
        '/ 処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        Shared sh_ippResize As tClass_ImgResize_IPP

        Shared Function hasPalette(pf As Imaging.PixelFormat) As Boolean
            If pf = Imaging.PixelFormat.Format8bppIndexed Then Return True

            If pf = Imaging.PixelFormat.Format4bppIndexed Then Return True
            If pf = Imaging.PixelFormat.Format1bppIndexed Then Return True
            Return False
        End Function

        Private Function PrepareResizedBitmap(ByVal rrp As Rectangle, ByVal DisplaySize As Size) As Bitmap

            If sh_ippResize Is Nothing Then
                sh_ippResize = New tClass_ImgResize_IPP()
                sh_ippResize.Resize = tClass_ImgResize_IPP.ResizeMethod.ippLinear
            End If


            Dim refimage As Image = Nothing
            For ii As Integer = 0 To m_pDspImg.Length - 1
                If m_pDspImg(ii) IsNot Nothing Then
                    refimage = m_pDspImg(ii)
                    Exit For
                End If
            Next
            If refimage Is Nothing Then
                Return Nothing
            End If

            Dim bmp As New Bitmap(DisplaySize.Width, DisplaySize.Height, refimage.PixelFormat)
            If hasPalette(refimage.PixelFormat) Then
                bmp.Palette = refimage.Palette
            End If

            Dim rp As Rectangle = New Rectangle(rrp.Location, New Size(Math.Abs(rrp.Width), Math.Abs(rrp.Height)))
            Dim FrameHeight As Integer = mcls_pCalc.tProp_ImgInf.nOneImgH
            Try

                Using dmem As New LockingBitmap(bmp, Imaging.ImageLockMode.WriteOnly)
                    Dim ry As Integer = rp.Top
                    Do While ry < rp.Bottom

                        Dim nFrame As Integer = ry \ FrameHeight
                        Dim SrcY As Integer = ry Mod FrameHeight
                        Dim SrcHeight As Integer = Math.Min(FrameHeight - SrcY, rp.Bottom - ry)
                        If SrcHeight = 0 Then Exit Do

                        Dim DstY As Integer = CInt((ry - rp.Y) * DisplaySize.Height / rp.Height)
                        Dim DstYN As Integer = CInt((ry + SrcHeight - rp.Y) * DisplaySize.Height / rp.Height)
                        Dim DstHeight As Integer = DstYN - DstY


                        Dim srcbmp As Bitmap = TryCast(m_pDspImg(nFrame), Bitmap)
                        If srcbmp IsNot Nothing Then
                            Using smem As New LockingBitmap(srcbmp, Imaging.ImageLockMode.ReadOnly)
                                sh_ippResize.ippResize(smem.PtrAt(SrcY, rp.X), smem.Stride, rp.Width, SrcHeight, _
                                                       dmem.PtrAt(DstY, 0), dmem.Stride, dmem.Width, DstHeight, bmp.PixelFormat)
                            End Using
                        End If

                        ' 次へ
                        ry = ry + SrcHeight
                    Loop

                End Using
            Catch ex As Exception

            End Try

            RotateByImgKind(bmp)
            Return bmp


        End Function

        ''' <summary>
        ''' 表示に応じた変換。回転・反転
        ''' </summary>
        ''' <param name="bmp"></param>
        Private Sub RotateByImgKind(bmp As Bitmap)
            Dim RotTable() As RotateFlipType = New RotateFlipType() { _
                RotateFlipType.RotateNoneFlipNone, _
                RotateFlipType.RotateNoneFlipY, _
                RotateFlipType.RotateNoneFlipX, _
                RotateFlipType.RotateNoneFlipXY, _
                RotateFlipType.Rotate90FlipXY, _
                RotateFlipType.Rotate90FlipY, _
                RotateFlipType.Rotate90FlipX, _
                RotateFlipType.Rotate90FlipNone _
            }
            Dim nIndex As Integer = IIf(mcls_pCalc.tProp_ImgKind_Rotate90, 4, 0) _
                                    + IIf(mcls_pCalc.tProp_ImgKind_IsTurnLR, 2, 0) _
                                    + IIf(mcls_pCalc.tProp_ImgKind_IsTurnTB, 1, 0)

            If RotTable(nIndex) <> 0 Then
                bmp.RotateFlip(RotTable(nIndex))
            End If
        End Sub

        ''' <summary>
        ''' 指定した領域のフレーム画像を取得
        ''' </summary>
        ''' <param name="rrp">取得する画像のエリア。フレーム結合座標</param>
        ''' <returns>指定エリアの画像</returns>
        ''' <remarks></remarks>
        Private Function PrepareRawBitmap(ByVal rrp As Rectangle, ByVal DisplaySize As Size) As Bitmap
            Dim rp As Rectangle = New Rectangle(rrp.Location, New Size(Math.Abs(rrp.Width), Math.Abs(rrp.Height)))

            If rp.Height > 4096 Or rp.Width > 4096 Then
                Return PrepareResizedBitmap(rrp, DisplaySize)
            End If

            Dim bmp As New Bitmap(rp.Width, rp.Height, Imaging.PixelFormat.Format32bppRgb)

            Dim FrameHeight As Integer = mcls_pCalc.tProp_ImgInf.nOneImgH
            Try
                Using g As Graphics = Graphics.FromImage(bmp)

                    Dim nIndexX As Integer = (rp.X + rp.Width - 1) \ mcls_pCalc.tProp_ImgInf.nOneImgW
                    If nIndexX <> 0 Then Throw New System.NotSupportedException("X方向に複数画像ある描画に対しての部分拡大表示は未対応。")
                    Dim nIndexYS As Integer = rp.Y \ FrameHeight
                    Dim nIndexYE As Integer = (rp.Y + rp.Height - 1) \ FrameHeight


                    '''' 描画
                    Dim Dst As New Rectangle(0, 0, rp.Width, 0)        ' 描画領域
                    Dim Src As New Rectangle(rp.X, rp.Y Mod FrameHeight, rp.Width, 0)       ' 元画像範囲


                    For ii As Integer = nIndexYS To nIndexYE
                        If ii <> nIndexYE Then
                            Src.Height = FrameHeight - Src.Y
                        Else
                            Src.Height = (rp.Y + rp.Height - 1) Mod FrameHeight
                        End If
                        Dst.Height = Src.Height

                        If m_pDspImg(ii) IsNot Nothing Then
                            g.DrawImage(m_pDspImg(ii), Dst, Src, GraphicsUnit.Pixel)
                        End If

                        ' 次へ
                        Src.Y = 0
                        Dst.Y += Dst.Height
                    Next
                End Using
            Catch ex As Exception

            End Try

            '表示に応じた変換。回転・反転
            RotateByImgKind(bmp)

            Return bmp
        End Function


        ''' <summary>
        ''' 部分拡大画像の描画
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub pic_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim ptMouse As Point = m_BasePic.PointToClient(Windows.Forms.Cursor.Position)          ' クリック位置 (画面座標からPicコントロールの左上からの座標に変換)
            Dim g As Graphics = e.Graphics

            '''' 元画像の位置を取得
            Dim Rp As Rectangle = GetRpAria(ptMouse)


            Try
                'フレーム結合(するかもしれない)画像を取得
                Using bmp As Bitmap = PrepareRawBitmap(Rp, m_pic.Size)
                    'フレーム画像を縮小表示。
                    g.CompositingQuality = Drawing2D.CompositingQuality.HighQuality
                    g.InterpolationMode = Drawing2D.InterpolationMode.HighQualityBilinear
                    g.DrawImage(bmp, New Rectangle(0, 0, m_pic.Width, m_pic.Height))
                End Using

                '' メモリ情報関係の描画
                If m_MemoriInf.bIsDspRitu Or m_MemoriInf.bIsDspMemori Then
                    Dim Bsh As SolidBrush = New SolidBrush(m_MemoriInf.Color)
                    Dim Pen As Pen = New System.Drawing.Pen(m_MemoriInf.Color, 2)
                    Dim Fnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

                    ' 現在の倍率を表示する
                    Dim dsp_string_left As Integer = 0
                    If m_MemoriInf.bIsDspRitu Then
                        Dim strValWk As String = String.Format("x {0:0.0}", m_dDspRitu)
                        dsp_string_left = CInt(g.MeasureString(strValWk, Fnt).Width)
                        g.DrawString(strValWk, Fnt, Bsh, 2, 2) ' 描画
                    End If

                    If Not String.IsNullOrEmpty(Comment) Then
                        Dim r As New Rectangle(0, 0, m_pic.Width, 32)
                        If dsp_string_left + 32 < m_pic.Width Then
                            r.X += dsp_string_left
                            r.Width -= dsp_string_left
                        Else
                            r.Y += 18
                        End If
                        Dim sf As New StringFormat
                        sf.Alignment = StringAlignment.Center
                        g.DrawString(Comment, Fnt, Brushes.Yellow, r, sf)
                    End If

                    ' メモリ表示
                    If m_MemoriInf.bIsDspMemori Then
                        Dim memori As New SimpleMemori(m_pic.ClientSize, mcls_pCalc)
                        memori.MemoriInf = Me.MemoriInf
                        memori.zoom_modifier_x = 1 / m_dDspRitu
                        memori.zoom_modifier_y = 1 / m_dDspRitu
                        memori.DrawMemori(g, Bsh, Pen, Fnt)
                    End If
                End If
            Catch ex As Exception
            End Try
        End Sub

        Function StartDrawing(pt As Point) As Boolean
            '''' 描画に必要な情報を取得. pDspImgは、本体側の画像を参照しているので、Disposeしてはいけない
            mcls_pCalc = Nothing
            m_pDspImg = Nothing
            RaiseEvent tEv_DispDataSet(mcls_pCalc, m_pDspImg)


            '''' 拡大表示エリアの位置を決定
            m_pic.Location = GetPicLocation(pt)

            ' 拡大表示エリアを表示
            m_pic.Visible = True
            m_bDspOn = True

            ' マウスカーソルを非表示
            Cursor.Clip = New Rectangle(m_BaseFrm.PointToScreen(m_BasePic.Location), m_BasePic.ClientSize)

            Return True

        End Function

        ''' <summary>
        ''' 親ピクチャボックスでのマウス押下げを横取りして、自分の表示を開始
        ''' </summary>
        Private Sub BasePic_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            'If e.Button <> MouseButtons.Right Then Return

            '' 虫眼鏡表示 開始
            If e.Button = MouseButtons.Right Then
                If m_bDspOn Then Return
                Cursor.Hide()

                m_dDspRitu = DefaultZoom
                StartDrawing(e.Location)
            End If



            '' 虫眼鏡表示中のみ
            If e.Button = MouseButtons.Left Then
                If Not m_bDspOn Then Return

                ' グラフィックで描画したものを取得
                Dim rect As Rectangle = m_pic.ClientRectangle
                Dim bmp As New Bitmap(rect.Width, rect.Height)
                Dim g As Graphics = Graphics.FromImage(bmp)     ' 描画先確保
                Dim pea As New PaintEventArgs(g, rect)          ' 描画関数生成
                ' 今回作ったpeaに対して、描画させる。
                pic_Paint(m_pic, pea)
                ' クリップボードへ
                Clipboard.SetImage(bmp)
                ' 後始末
                g.Dispose()
                bmp.Dispose()
            End If
        End Sub

        ''' <summary>
        ''' 親ピクチャボックスでのマウス押上により、表示を終了
        ''' </summary>
        Private Sub BasePic_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return

            '' 虫眼鏡表示 完了
            'If Not m_bDspOn Then Return
            If e.Button = MouseButtons.Right Then
                If m_bDspOn Then
                    m_pic.Visible = False
                    m_bDspOn = False

                    ' マウスカーソルを表示
                    Cursor.Show()
                    Cursor.Clip = Nothing
                End If
            End If
        End Sub



        ''' <summary>
        ''' 強制分解能変更(縮小表示処理）を実施したときに、変更後分解能から、元の分解能に戻す係数設定
        ''' </summary>
        ''' <param name="comment">強制分解能変更のコメント。拡大時にタイトルとして表示</param>
        Public Sub SetComment(comment As String)
            Me.Comment = comment
        End Sub

        ''' <summary>
        ''' 親ピクチャボックスでのマウス移動に基づき、拡大の基準位置を動かす
        ''' </summary>
        Private Sub BasePic_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            ' windowsキーとかでフォーカスが他フォームに移ってから、マウスアップされた場合
            ' マウスアップイベントが入らない対策。
            '    動かしたときに、マウス右が押されていなかったら、拡大強制解除
            If e.Button <> MouseButtons.Right Then
                Call BasePic_MouseUp(sender, e)
                Return
            End If

            m_pic.Location = GetPicLocation(e.Location)             ' 拡大表示エリアの位置を決定

            ' ちらつかないように描画
            m_BasePic.Refresh()
            m_pic.Refresh()
            'm_pic.Invalidate()
        End Sub

        ''' <summary>
        ''' 親ピクチャボックスでのマウスホイールを横取りして、拡大倍率を変更する。Ctrlキーが押されていたら、自ウインドウを拡大
        ''' </summary>
        Private Sub BaseFrm_MouseWheela(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            'Ctrl + Wheel = 拡大ウインドウサイズの変更
            If Control.ModifierKeys = Keys.Control Then
                Dim dxdy As Integer = 20
                If e.Delta < 0 Then
                    dxdy *= -1
                End If

                'ウインドウサイズは正方形を前提とする
                Dim newsize As Integer = Saturate(m_pic.Width + dxdy, 100, MAX_WINDOW_SIZE)
                m_pic.Size = New Size(newsize, newsize)
                DefaultSize = m_pic.Size

                'ウインドウ位置は、カーソル中心が基準。サイズが変わったのでずれてる
                Dim pt As Point = Control.MousePosition
                Dim ptbase As Point = m_BaseFrm.PointToClient(pt)
                StartDrawing(ptbase)
                Return
            End If

            'ふつうのWheel = 拡大率の変更
            If e.Delta > 0 Then
                m_dDspRitu += 0.5
            Else
                m_dDspRitu -= 0.5
            End If
            LimitValue(m_dDspRitu, MIN_ZOOM, MAX_ZOOM)

            ' 再描画 (位置は変わらないので、拡大表示エリアのみ描画)
            m_pic.Refresh()
        End Sub

        ''' <summary>
        ''' ベースフォーム マウスクリック
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub BaseFrm_MouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            If e.Button = MouseButtons.Middle Then
                'MsgBox("")
            End If
        End Sub



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 小物
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' [DP]ベースフォームからの拡大表示エリアの位置を算出
        ''' </summary>
        ''' <param name="ptLocation">ベースピクチャーボックス の マウス位置</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function GetPicLocation(ByVal ptLocation As Point) As Point
            Dim pnt As New Point(ptLocation.X + m_BasePic.Left - CInt(m_pic.Size.Width / 2), ptLocation.Y + m_BasePic.Top - CInt(m_pic.Size.Height / 2))

            ' ベースピクチャーボックス をはみ出ることが無いように調整
            If pnt.X < m_BasePic.Left Then pnt.X = m_BasePic.Left
            If pnt.Y < m_BasePic.Top Then pnt.Y = m_BasePic.Top

            If (pnt.X + m_pic.Width) > (m_BasePic.Left + m_BasePic.Width) Then pnt.X -= (pnt.X + m_pic.Width) - (m_BasePic.Left + m_BasePic.Width)
            If (pnt.Y + m_pic.Height) > (m_BasePic.Top + m_BasePic.Height) Then pnt.Y -= (pnt.Y + m_pic.Height) - (m_BasePic.Top + m_BasePic.Height)

            Return pnt
        End Function

        ''' <summary>
        ''' [DP]ベースフォームからの拡大表示エリアの位置を算出
        ''' </summary>
        ''' <param name="e">ベースピクチャーボックス の マウス位置</param>
        Private Function GetRpAria(ByVal e As Point) As Rectangle
            Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns

            ' マウス位置 [RP]
            Dim nRpPoint As Point = mcls_pCalc.GetDspPixelToImgPosition(e.X, e.Y)

            ' 表示エリアサイズ [RP]
            Dim nDspSizeX As Integer = (CInt((m_pic.Size.Width / m_dDspRitu) / CalcAns.RateDsp.dW) + 1) '* CInt(IIf(mcls_pCalc.tProp_ImgKind_IsTurnLR, -1, 1))
            Dim nDspSizeY As Integer = (CInt((m_pic.Size.Height / m_dDspRitu) / CalcAns.RateDsp.dH) + 1) ' * CInt(IIf(mcls_pCalc.tProp_ImgKind_IsTurnTB, -1, 1))

            ' 元画像の表示対象位置 [RP]
            Dim rectRp As New Rectangle(nRpPoint.X - CInt(nDspSizeX / 2), nRpPoint.Y - CInt(nDspSizeY / 2), nDspSizeX, nDspSizeY)

            ' 上下限チェック
            Dim nMinX As Integer = mcls_pCalc.GetPos_DpToRp_X(CalcAns.ImgDspRect.X)
            Dim nMinY As Integer = mcls_pCalc.GetPos_DpToRp_Y(CalcAns.ImgDspRect.Y)
            Dim nMaxX As Integer = mcls_pCalc.tProp_ImgInf.nImgW
            Dim nMaxY As Integer = mcls_pCalc.tProp_ImgInf.nImgH

            If rectRp.X < nMinX Then rectRp.X = nMinX
            If rectRp.Y < nMinY Then rectRp.Y = nMinY
            If (rectRp.X + rectRp.Width) > nMaxX Then rectRp.X -= (rectRp.X + rectRp.Width) - nMaxX
            If (rectRp.Y + rectRp.Height) > nMaxY Then rectRp.Y -= (rectRp.Y + rectRp.Height) - nMaxY

            Return rectRp


            '''' [DP]座標系でやったら、高倍率時に誤差が大きすぎるのでやめ

            '' 元画像左上からマウス位置までの距離を取得 [DP]
            'Dim nDpPoint As Point					' 元画像左上からマウス位置までの距離を取得 [DP]
            'nDpPoint = New Point(e.X, e.Y)	 ' 表示画像左上からマウス位置までの距離を取得 [DP]
            'Dim DspSize As New Size(CInt(m_pic.Size.Width / m_dDspRitu), CInt(m_pic.Size.Height / m_dDspRitu))

            'Debug.Print("DP={0},{1} ", nDpPoint.X, nDpPoint.Y)

            '' 元画像左上からDPでの表示対象位置
            'Dim rectDp As New Rectangle(nDpPoint.X - CInt(DspSize.Width / 2), nDpPoint.Y - CInt(DspSize.Height / 2), DspSize.Width, DspSize.Height)
            'If rectDp.X < CalcAns.ImgDspRect.X Then rectDp.X = mcls_pCalc.tProp_CalcAns.ImgDspRect.X
            'If rectDp.Y < CalcAns.ImgDspRect.Y Then rectDp.Y = mcls_pCalc.tProp_CalcAns.ImgDspRect.Y
            'If (rectDp.X + rectDp.Width) > CInt(mcls_pCalc.tProp_ImgInf.nImgW * CalcAns.RateDsp.dW) Then rectDp.X -= (rectDp.X + rectDp.Width) - CInt(mcls_pCalc.tProp_ImgInf.nImgW * CalcAns.RateDsp.dW)
            'If (rectDp.Y + rectDp.Height) > CInt(mcls_pCalc.tProp_ImgInf.nImgH * CalcAns.RateDsp.dH) Then rectDp.Y -= (rectDp.Y + rectDp.Height) - CInt(mcls_pCalc.tProp_ImgInf.nImgH * CalcAns.RateDsp.dH)


            '' DP→RP
            'Dim PointRp1 As Point = mcls_pCalc.GetDspPixelToImgPosition(rectDp.X, rectDp.Y)
            'Dim PointRp2 As Point = mcls_pCalc.GetDspPixelToImgPosition(rectDp.X + rectDp.Width - 1, rectDp.Y + rectDp.Height - 1)

            'Debug.Print("XY1={0},{1}  XY2={2},{3}", PointRp1.X, PointRp1.Y, PointRp2.X, PointRp2.Y)


            'Return New Rectangle(PointRp1.X, PointRp1.Y, PointRp2.X - PointRp1.X + 1, PointRp2.Y - PointRp1.Y + 1)
        End Function

    End Class

End Namespace
