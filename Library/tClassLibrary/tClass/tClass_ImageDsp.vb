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
Imports System.IO

Namespace tClass


    Public Class tClass_ImageDsp
        Inherits tClass_ImageDspBase
        Implements IDisposable	' デストラクタ定義


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部イベント
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' オーバーレイ内クリック
        ''' </summary>
        ''' <param name="sender">オーバーレイ情報</param>
        ''' <param name="e">表示されているオーバーレイ位置情報</param>
        ''' <remarks></remarks>
        Public Event tEv_OverLay_Click(ByVal sender As OVERLAY_INF, ByVal e As Rectangle)

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 定数定義
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>オーバーレイ 最終結果</summary>
        Protected Structure OVERLAY_ANS
            Dim ovlInf As OVERLAY_INF				' 基本情報

            Dim ansRect As Rectangle				' 最終結果座標
        End Structure

        ''' <summary>オーバーレイ情報</summary>
        Public Structure OVERLAY_INF
            ''' <summary>表示有無 (true:有, false:無)</summary>
            Dim bIsDsp As Boolean

            ''' <summary>線設定</summary>
            Dim pen As Pen
            ''' <summary>表示色 (Pen未指定時。簡易表示時)</summary>
            Dim Color As Color
            ''' <summary>線サイズ (Pen未指定時。簡易表示時)</summary>
            Dim LineWidth As Integer


            ''' <summary>オーバーレイ情報</summary>
            Dim ovlPos1 As Point
            Dim ovlPos2 As Point

            ''' <summary>その他拡張情報</summary>
            Dim Tag As Object

            ''' <summary>横の補正量 [DP]</summary>
            Dim HoseiX As Integer
            ''' <summary>縦の補正量 [DP]</summary>
            Dim HoseiY As Integer


            ' 文字表示 =======================================
            ''' <summary>疵種表示有無 (true:有, false:無)</summary>
            Dim bIsDspType As Boolean
            Dim cBsh As SolidBrush
            Dim cFnt As Font
            Dim sTypeName As String		' 疵名

        End Structure


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>オーバーレイ情報</summary>
        Protected m_OverLayInf As New List(Of OVERLAY_ANS)

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' オーバーレイのセット個数
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property tProp_OverLayCnt() As Integer
            Get
                Return m_OverLayInf.Count
            End Get
        End Property
        ''' <summary>
        ''' オーバーレイの情報
        ''' </summary>
        ''' <param name="nIndex"></param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property tProp_OverLayInf(ByVal nIndex As Integer) As OVERLAY_INF
            Get
                Return m_OverLayInf(nIndex).ovlInf
            End Get
        End Property

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()

        End Sub


        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Shadows Sub Dispose()
            m_OverLayInf.Clear()
            MyBase.Dispose()
        End Sub

        ''' <summary>
        ''' クリックにより オーバーレイ チェック処理有り
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub AddHandler_OverLayClick()
            AddHandler m_cntPic.Click, AddressOf PictureBox_Click
        End Sub
        ''' <summary>
        ''' クリックにより オーバーレイ チェック処理無し
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RemoveHandler_OverLayClick()
            RemoveHandler m_cntPic.Click, AddressOf PictureBox_Click
        End Sub

        ''' <summary>
        ''' オーバーレイ情報登録
        ''' </summary>
        ''' <param name="ovl">オーバーレイ情報</param>
        ''' <remarks></remarks>
        Public Sub SetOverLayInf(ByVal ovl As OVERLAY_INF)
            ' 情報登録
            Dim ans As OVERLAY_ANS
            ans.ovlInf = ovl

            m_OverLayInf.Add(ans)
        End Sub
        ''' <summary>
        ''' オーバーレイ情報開放
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResetOverLayInf()
            ' 情報開放
            m_OverLayInf.Clear()
        End Sub

        ''' <summary>
        ''' オーバーレイ表示非表示設定
        ''' </summary>
        ''' <param name="dsp">オーバーレイ表示非表示設定</param>
        ''' <remarks></remarks>
        Public Sub OverLayDspChange(ByVal dsp As Boolean)
            Dim ovl As OVERLAY_ANS
            For ii As Integer = 0 To m_OverLayInf.Count - 1
                ovl = m_OverLayInf(ii)
                ovl.ovlInf.bIsDsp = dsp
                m_OverLayInf(ii) = ovl
            Next
            ' 再描画
            Me.Refresh()
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ オーバーレイ選択
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' オーバーレイクリック処理
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub PictureBox_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ''Dim pos As Point = m_cntPic.PointToClient(Windows.Forms.Cursor.Position)		 ' クリック位置 (画面座標からPicコントロールの左上からの座標に変換)

            ''For Each ans As OVERLAY_ANS In m_OverLayInf
            ''	' そもそも表示中のオーバーレイか
            ''	If Not ans.ovlInf.bIsDsp Then Continue For
            ''	' 範囲内かチェック
            ''	If ans.ansRect.X > pos.X Then Continue For
            ''	If ans.ansRect.Y > pos.Y Then Continue For
            ''	If ans.ansRect.X + ans.ansRect.Width < pos.X Then Continue For
            ''	If ans.ansRect.Y + ans.ansRect.Height < pos.Y Then Continue For

            ''	' ここまできたら イベント発生
            ''	RaiseEvent tEv_OverLay_Click(ans.ovlInf, ans.ansRect)
            ''	Exit Sub
            ''Next

            Dim pos As Point = m_cntPic.PointToClient(Windows.Forms.Cursor.Position)		 ' クリック位置 (画面座標からPicコントロールの左上からの座標に変換)

            Dim min As OVERLAY_ANS = Nothing		  ' 一番小さいもの
            Dim bSet As Boolean = False

            For Each ans As OVERLAY_ANS In m_OverLayInf
                ' そもそも表示中のオーバーレイか
                If Not ans.ovlInf.bIsDsp Then Continue For
                ' 範囲内かチェック
                If ans.ansRect.X > pos.X Then Continue For
                If ans.ansRect.Y > pos.Y Then Continue For
                If ans.ansRect.X + ans.ansRect.Width < pos.X Then Continue For
                If ans.ansRect.Y + ans.ansRect.Height < pos.Y Then Continue For

                ' 一番小さいものを選択
                If Not bSet OrElse min.ansRect.Width * min.ansRect.Height > ans.ansRect.Width * ans.ansRect.Height Then
                    min = ans
                    bSet = True
                End If
            Next

            If bSet Then
                ' ここまできたら イベント発生
                RaiseEvent tEv_OverLay_Click(min.ovlInf, min.ansRect)
            End If
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ イメージオブジェクト 関係
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 最終描画 (継承)
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' オーバーレイ描画
            Paint_OverLay(e)
        End Sub


        ''' <summary>
        ''' オーバーレイ描画
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_OverLay(ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim rct As Rectangle = Nothing
            Dim ovl As OVERLAY_INF
            Dim wkPos1 As Point
            Dim wkPos2 As Point
            Dim wkVal As Integer

            '''' 全件 表示
            For ii As Integer = 0 To m_OverLayInf.Count - 1
                ovl = m_OverLayInf(ii).ovlInf
                If Not ovl.bIsDsp Then Continue For

                ' 表示座標を計算
                wkPos1 = GetPosition(ovl.ovlPos1)
                wkPos2 = GetPosition(ovl.ovlPos2)

                ' 大小チェック
                If wkPos1.X > wkPos2.X Then
                    wkVal = wkPos1.X
                    wkPos1.X = wkPos2.X
                    wkPos2.X = wkVal
                End If
                If wkPos1.Y > wkPos2.Y Then
                    wkVal = wkPos1.Y
                    wkPos1.Y = wkPos2.Y
                    wkPos2.Y = wkVal
                End If

                ' オフセット値反映
                wkPos1.X -= ovl.HoseiX
                wkPos2.X += ovl.HoseiX
                wkPos1.Y -= ovl.HoseiY
                wkPos2.Y += ovl.HoseiY

                ' 全部表示エリア外？
                Dim bDspOk As Boolean = True
                If (wkPos1.X < 0 AndAlso wkPos2.X < 0) OrElse _
                   (wkPos1.X > m_AriaInf.nAriaWidth And wkPos2.X > m_AriaInf.nAriaWidth) OrElse _
                  (wkPos1.Y < 0 AndAlso wkPos2.Y < 0) OrElse _
                   (wkPos1.Y > m_AriaInf.nAriaHeight And wkPos2.Y > m_AriaInf.nAriaHeight) Then
                    bDspOk = False
                End If


                ' 表示エリア内かチェック
                If wkPos1.X < 0 Then wkPos1.X = 0
                If wkPos1.X > m_AriaInf.nAriaWidth Then wkPos1.X = m_AriaInf.nAriaWidth
                If wkPos2.X < 0 Then wkPos2.X = 0
                If wkPos2.X > m_AriaInf.nAriaWidth Then wkPos2.X = m_AriaInf.nAriaWidth

                If wkPos1.Y < 0 Then wkPos1.Y = 0
                If wkPos1.Y > m_AriaInf.nAriaHeight Then wkPos1.Y = m_AriaInf.nAriaHeight
                If wkPos2.Y < 0 Then wkPos2.Y = 0
                If wkPos2.Y > m_AriaInf.nAriaHeight Then wkPos2.Y = m_AriaInf.nAriaHeight

                ' 結果反映
                Dim ans As OVERLAY_ANS = m_OverLayInf(ii)

                '回転していたら
                If tProp_ImgKind_Rotate90 Then
                    '90度回転で、XとYが転地する。新しいY（元X)は、上下が反転している
                    ans.ansRect = New Rectangle(wkPos1.Y, wkPos1.X, wkPos2.Y - wkPos1.Y, wkPos2.X - wkPos1.X)
                Else
                    ans.ansRect = New Rectangle(wkPos1.X, wkPos1.Y, wkPos2.X - wkPos1.X, wkPos2.Y - wkPos1.Y)
                End If

                m_OverLayInf(ii) = ans
                ' ペン
                If ovl.pen Is Nothing Then
                    Using pen As New System.Drawing.Pen(ovl.Color, ovl.LineWidth)
                        e.Graphics.DrawRectangle(pen, ans.ansRect)
                    End Using
                Else
                    e.Graphics.DrawRectangle(ovl.pen, ans.ansRect)
                End If



                ' 疵種名表示==============================
                If ovl.bIsDspType AndAlso bDspOk Then
                    Dim sizeF As SizeF = e.Graphics.MeasureString(ovl.sTypeName, ovl.cFnt)
                    e.Graphics.DrawString(ovl.sTypeName, ovl.cFnt, ovl.cBsh, ans.ansRect.X, ans.ansRect.Y - sizeF.Height)
                End If
            Next ii
        End Sub
    End Class
End Namespace