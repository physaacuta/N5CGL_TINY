Public Class frmPopKiriInput



    '画面のイメージを取得
    Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
    Const SRCCOPY As Integer = &HCC0020


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private m_BasePic As PictureBox				' ベースピクチャーボックス
    Private mfrm_pInputList As frmPopInputList						' 入力一覧機能 (ポインタ)
    Private m_bInputLine As Boolean									' 位置選択 機能有無


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    Public Sub SetCls_Pic(ByRef cls As PictureBox)
        m_BasePic = cls
    End Sub


    Public Sub SetFrm_InputList(ByRef frm As frmPopInputList)
        mfrm_pInputList = frm
    End Sub

    Public WriteOnly Property tProp_InputLine() As Boolean
        Set(value As Boolean)
            m_bInputLine = value
        End Set
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopKiriInput_Load(sender As Object, e As System.EventArgs) Handles Me.Load

        ' 中空の設定
        picMain.BackColor = Color.FromArgb(255, 64, 64, 0)	' まず使っていない色にする
        Me.TransparencyKey = picMain.BackColor				' その色を透明色に設定

        ' 状態変更
        cmdPosSave.Visible = m_bInputLine
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メイン処理
    ''' <summary>
    ''' クリップボードへコピー
    ''' </summary>
    Private Sub cmdClipCopy_Click(sender As System.Object, e As System.EventArgs) Handles cmdClipCopy.Click

        Dim gP As Graphics = m_BasePic.CreateGraphics
        Dim rect As Rectangle = picMain.ClientRectangle		' 指定されたサイズ分抜き取り
        If 1 > rect.Width OrElse 1 > rect.Height Then Return

        ' クリップ領域作成
        Dim bmp As New Bitmap(rect.Width, rect.Height, gP)
        Dim gT As Graphics = Graphics.FromImage(bmp)		' 描画先確保

        Dim HdcF As IntPtr = gP.GetHdc() ' GetWindowDC(m_BasePic.Handle)	'GetWindowDC(m_BasePic.Handle)
        Dim HdcT As IntPtr = gT.GetHdc()	'ポインタ取得


        ' 指定位置を一旦スクリーン座標系にするのがミソ
        ' そのスクリーン座標系と同位置の、フレーム画像のクライアント座標系を取得
        ' 各クライアント座標系からスクリーン座標系に変換
        Dim nScrPic As Point = picMain.PointToScreen(New Point(0, 0))
        Dim nCltPos As Point = m_BasePic.PointToClient(nScrPic)		' 画面座標系になる


        '' イメージ取得
        BitBlt(HdcT, 0, 0, rect.Width, rect.Height, HdcF, nCltPos.X, nCltPos.Y, SRCCOPY)

        ' 後始末ちゃんとしないと、データがちゃんととれない。
        gP.ReleaseHdc(HdcF)
        gP.Dispose()
        gT.Dispose()

        ' クリップボードへ
        Clipboard.SetImage(bmp)

    End Sub

    ''' <summary>
    ''' 位置保存
    ''' </summary>
    Private Sub cmdPosSave_Click(sender As System.Object, e As System.EventArgs) Handles cmdPosSave.Click
        Dim rect As Rectangle = picMain.ClientRectangle		' 指定されたサイズ分抜き取り
        Dim nScrPic As Point = picMain.PointToScreen(New Point(0, 0))
        Dim nCltPos As Point = m_BasePic.PointToClient(nScrPic)		' 画面座標系になる

        ' 今回の選択領域を メイン画面基準の 画面座標系 に統一する
        Dim pos1 As New Point(nCltPos.X, nCltPos.Y)
        Dim pos2 As New Point(nCltPos.X + rect.Width, nCltPos.Y + rect.Height)

        ' 渡す
        Call mfrm_pInputList.InputListAdd(pos1, pos2)
        Call mfrm_pInputList.DspGridData()
    End Sub
End Class