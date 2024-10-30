'*********************************************************************************
'ピクチャーボックスの画像を部分的に拡大表示するクラス (なんちゃって拡大版)
'	[Ver]
'		Ver.01    2008/11/21  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Windows.Forms
Imports System.Drawing


Namespace tClass

    ''' <summary>
    ''' ピクチャーボックスの画像を部分的に拡大表示するクラス
    ''' </summary>
    Public Class tClass_ImageCutDsp
        Implements IDisposable							' デストラクタ定義




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
        Private m_BaseFrm As Control 'Form					' ベースフォーム
        Private m_BasePic As PictureBox				' ベースピクチャーボックス

        Private m_pic As PictureBox					' 拡大表示エリア

        Private m_bDspStop As Boolean = False		' 拡大表示機能一時停止
        Private m_bDspOn As Boolean = False			' 拡大表示中
        Private m_dDspRitu As Double				' 拡大率
        Private m_dDefRitu As Double				' デフォルト倍率


        Private m_MemoriInf As MEMORI_INF			' メモリ情報
        Private m_dBaseResW As Double				' ベース画像の表示横分解能
        Private m_dBaseResH As Double				' ベース画像の表示縦分解能


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
        ''' 初期倍率
        ''' </summary>
        Public Property DefRitu() As Double
            Get
                Return m_dDefRitu
            End Get
            Set(ByVal value As Double)
                m_dDefRitu = value
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

        ''' <summary>
        ''' ベース画像の表示分解能
        ''' </summary>
        ''' <param name="resW">横分解能</param>
        ''' <param name="resH">縦分解能</param>
        Public Sub SetItemRes(ByVal resW As Double, ByVal resH As Double)
            m_dBaseResW = resW
            m_dBaseResH = resH
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開 処理
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="frm">ベースフォーム</param>
        ''' <param name="pic">拡大対象のピクチャーボックス</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal frm As Control, ByVal pic As PictureBox)
            '''' データをセット
            m_BaseFrm = frm
            m_BasePic = pic

            '''' 拡大表示エリアを生成
            m_pic = New PictureBox
            m_pic.Size = New Size(200, 200)
            m_pic.Location = New Point(0, 0)
            m_pic.BorderStyle = BorderStyle.FixedSingle
            m_pic.Visible = False

            m_BaseFrm.Controls.Add(m_pic)                       ' フレーム画像表示ピクチャーの配下にする
            m_pic.BringToFront()                                ' 親ピクチャーの上に持ってくる


            '''' その他もろもろ初期化
            m_dDefRitu = 2.0
            m_dBaseResW = 1.0
            m_dBaseResH = 1.0


            '''' イベント追加
            AddHandler m_pic.Paint, AddressOf pic_Paint
            AddHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
            AddHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
            AddHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
            AddHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela

            'Windows.Forms.Cursor.Hide()					' カーソルを消す

        End Sub



#Region " IDisposable Support "

        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには

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



        ''' <summary>
        ''' 拡大表示エリア ペイント
        ''' </summary>
        Private Sub pic_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            '' キャプチャした画像を格納するエリア
            Using bmp As New Bitmap(m_BasePic.ClientSize.Width, m_BasePic.ClientSize.Height)

                ' 対象ピクチャーボックスの全体のキャプチャ(rectangleはbmpのどの位置に描くか)
                m_BasePic.DrawToBitmap(bmp, New Rectangle(0, 0, bmp.Size.Width, bmp.Size.Height))

                ' ベースの描画位置を決定
                Dim pos As Point = m_BasePic.PointToClient(Windows.Forms.Cursor.Position)		   ' クリック位置 (画面座標からPicコントロールの左上からの座標に変換)
                Dim dspSize As New Size(CInt(m_pic.Size.Width * m_dDspRitu), CInt(m_pic.Size.Height * m_dDspRitu))		' 今回表示する画像のサイズ
                Dim dspPnt As New Point(CInt(m_pic.Size.Width / 2) - CInt(dspSize.Width / 2), CInt(m_pic.Size.Height / 2) - CInt(dspSize.Height / 2))

                e.Graphics.CompositingQuality = Drawing2D.CompositingQuality.HighQuality			' 拡大がメインの為、クォリティーを良くしておく

                ' マウスポインタを中心として、拡大描画する
                e.Graphics.DrawImage(bmp, _
                  New Rectangle(dspPnt, dspSize), _
                  New Rectangle(pos.X - CInt(m_pic.Size.Width / 2), pos.Y - CInt(m_pic.Size.Height / 2), m_pic.Size.Width, m_pic.Size.Height), GraphicsUnit.Pixel)
            End Using

            '' メモリ開放処理
            ' m_BasePic.DrawToBitmap で画像サイズが巨大だと、メモリにすぐ残骸が残る。GCが開放する前に主記憶領域がパンクする
            ' その為、手動でGCをコールするようにした
            GC.Collect()

            '' メモリ情報関係の描画
            If m_MemoriInf.bIsDspRitu Or m_MemoriInf.bIsDspMemori Then
                Dim Bsh As SolidBrush = New SolidBrush(m_MemoriInf.Color)
                Dim Pen As Pen = New System.Drawing.Pen(m_MemoriInf.Color, 2)
                Dim Fnt As Font = New System.Drawing.Font("ＭＳ ゴシック", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                Dim strValWk As String

                ' 現在の倍率を表示する
                If m_MemoriInf.bIsDspRitu Then
                    strValWk = String.Format("x {0:0.0}", m_dDspRitu)

                    e.Graphics.DrawString(strValWk, Fnt, Bsh, 2, 2)	' 描画
                End If

                ' メモリ表示
                If m_MemoriInf.bIsDspMemori Then
                    Dim nPlsLineTop As Integer = m_pic.Size.Height - 20
                    Dim nPlsLineLeft As Integer = 15

                    ' Y軸
                    strValWk = String.Format("{0:0.0}", m_MemoriInf.nInteralY * m_dBaseResH / m_dDspRitu)
                    e.Graphics.DrawString(strValWk, Fnt, Bsh, nPlsLineLeft - 10, nPlsLineTop - m_MemoriInf.nInteralY - 15)
                    e.Graphics.DrawLine(Pen, nPlsLineLeft, nPlsLineTop, nPlsLineLeft, nPlsLineTop - m_MemoriInf.nInteralY)

                    ' X軸
                    strValWk = String.Format("{0:0.0}", m_MemoriInf.nInteralX * m_dBaseResW / m_dDspRitu)
                    e.Graphics.DrawString(strValWk, Fnt, Bsh, nPlsLineLeft + m_MemoriInf.nInteralX - 10, nPlsLineTop + 3)
                    e.Graphics.DrawLine(Pen, nPlsLineLeft, nPlsLineTop, nPlsLineLeft + m_MemoriInf.nInteralX, nPlsLineTop)

                    ' 原点表示
                    e.Graphics.DrawString("0mm", Fnt, Bsh, nPlsLineLeft - 10, nPlsLineTop + 3)
                End If
            End If
        End Sub


        ''' <summary>
        ''' ベースピクチャーボックス マウスダウン
        ''' </summary>
        Private Sub BasePic_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If e.Button <> MouseButtons.Right Then Return
            If m_bDspOn Then Return

            m_pic.Location = GetPicLocation(e)				' 拡大表示エリアの位置を決定

            ' 拡大表示エリアを表示
            m_dDspRitu = m_dDefRitu
            m_pic.Visible = True
            m_bDspOn = True

            ' マウスカーソルを非表示
            Cursor.Hide()
            Cursor.Clip = New Rectangle(m_BaseFrm.PointToScreen(m_BasePic.Location), m_BasePic.ClientSize)
        End Sub

        ''' <summary>
        ''' ベースピクチャーボックス マウスアップ
        ''' </summary>
        Private Sub BasePic_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            m_pic.Visible = False
            m_bDspOn = False

            ' マウスカーソルを表示
            Cursor.Show()
            'Cursor.Clip.
            Cursor.Clip = Nothing
        End Sub

        ''' <summary>
        ''' ベースピクチャーボックス マウスムーブ
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

            m_pic.Location = GetPicLocation(e)				' 拡大表示エリアの位置を決定

            ' ちらつかないように描画
            m_BasePic.Refresh()
            m_pic.Refresh()
        End Sub

        ''' <summary>
        ''' ベースフォーム マウススクロール
        ''' </summary>
        Private Sub BaseFrm_MouseWheela(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return
            'If e.Button <> MouseButtons.Right Then Return

            ' 倍率決定
            If e.Delta > 0 Then
                m_dDspRitu += 0.5
                If m_dDspRitu > 10.0 Then m_dDspRitu = 10.0
            Else
                m_dDspRitu -= 0.5
                If m_dDspRitu < 1.0 Then m_dDspRitu = 1.0
            End If

            ' 再描画 (位置は変わらないので、拡大表示エリアのみ描画)
            m_pic.Refresh()
        End Sub



        ''' <summary>
        ''' ベースフォームからの拡大表示エリアの位置を算出
        ''' </summary>
        ''' <param name="e">ベースピクチャーボックス の マウス位置</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function GetPicLocation(ByVal e As System.Windows.Forms.MouseEventArgs) As Point
            Dim pnt As New Point(e.Location.X + m_BasePic.Left - CInt(m_pic.Size.Width / 2), e.Location.Y + m_BasePic.Top - CInt(m_pic.Size.Height / 2))

            ' ベースピクチャーボックス をはみ出ることが無いように調整
            If pnt.X < m_BasePic.Left Then pnt.X = m_BasePic.Left
            If pnt.Y < m_BasePic.Top Then pnt.Y = m_BasePic.Top

            If (pnt.X + m_pic.Width) > (m_BasePic.Left + m_BasePic.Width) Then pnt.X -= (pnt.X + m_pic.Width) - (m_BasePic.Left + m_BasePic.Width)
            If (pnt.Y + m_pic.Height) > (m_BasePic.Top + m_BasePic.Height) Then pnt.Y -= (pnt.Y + m_pic.Height) - (m_BasePic.Top + m_BasePic.Height)


            Return pnt
        End Function
    End Class
End Namespace