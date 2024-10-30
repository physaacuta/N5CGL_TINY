'*********************************************************************************
'画面のハードコピーを行うクラス
'   [Ver]
'       Ver.01    2005/04/13  vs2005 対応
'
'   [メモ]
'       シリアル化を行うクラスは<System.Serializable()>属性に設定する必要がある
'*********************************************************************************
Option Strict On                'tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Drawing
Imports System.Drawing.Printing
Imports System.Windows.Forms


Namespace tClass
    Public Class tClass_HeadCopy
        Implements IDisposable                          'デストラクタ定義

        '********************************************************************
        'API定義
        '********************************************************************
        '画面のイメージを取得
        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
        Const SRCCOPY As Integer = &HCC0020
        ' 20180702 キャプチャー不具合対応 ----->>>
        Const CAPTUREBLT As Integer = &H40000000
        ' 20180702 キャプチャー不具合対応 <<<-----
        '画面ウィンドウのデバイスコンテキストを取得
        Private Declare Auto Function GetWindowDC Lib "user32.dll" (ByVal hwnd As IntPtr) As IntPtr
        'ハンドル開放
        Private Declare Auto Function ReleaseDC Lib "user32.dll" (ByVal hwnd As IntPtr, ByVal hdc As IntPtr) As IntPtr


        '********************************************************************
        '定数 定義
        '********************************************************************
        Private mc_DefMagin_TOP As Integer = 4          'プリンタで印刷出来ない部分の余白[mm]
        Private mc_DefMagin_BOT As Integer = 10         'プリンタで印刷出来ない部分の余白[mm]
        Private mc_DefMagin_LEFT As Integer = 4         'プリンタで印刷出来ない部分の余白[mm]
        Private mc_DefMagin_RIGHT As Integer = 10       'プリンタで印刷出来ない部分の余白[mm]

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private m_PrintDoc As System.Drawing.Printing.PrintDocument     'プリント制御コントロール
        Private m_Bitmap As Bitmap      '印刷イメージビットマップ

        '' オプション
        Private m_PageSetup As System.Windows.Forms.PageSetupDialog     '印刷設定ダイヤログ
        Private m_PrintDialog As System.Windows.Forms.PrintDialog       'プリントダイヤログ
        Private m_Preview As System.Windows.Forms.PrintPreviewDialog    '印刷プレビューダイヤログ

        '' 印刷プロパティ
        Private m_Strach As Boolean = False                             'ストレッチ有効 (True:1ページに収まるように拡大縮小する False:コントロールのサイズのまま)


        '********************************************************************
        'プロパティー定義
        '********************************************************************
#Region "独自プロパティー"

        ''' <summary>
        ''' プリンタ決定
        ''' </summary>
        ''' <value>プリンタ名</value>
        Public Overloads Property prop_Printer() As String
        Get
            Return m_PrintDoc.PrinterSettings.PrinterName
        End Get
        Set(ByVal Value As String)
            m_PrintDoc.PrinterSettings.PrinterName = Value
        End Set
        End Property

        ''' <summary>
        ''' 用紙サイズ "A4" 等を指定 ただしプリンタ機種による為 注意する事
        ''' </summary>
        ''' <value>"A4"等の用紙サイズ</value>
        Public Overloads Property prop_PrintSize() As String
        Get
            Return m_PrintDoc.DefaultPageSettings.PaperSize.PaperName
        End Get
        Set(ByVal Value As String)
            '' 用紙サイズ設定
            Dim PSize As System.Drawing.Printing.PaperSize          '用紙サイズ変更
            For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
                If PSize.PaperName.StartsWith(Value) Then
                    '用紙サイズ設定
                    m_PrintDoc.DefaultPageSettings.PaperSize = PSize
                    Exit Property
                End If
                Next
                '' 該当なしの場合 一番最初を指定する
                m_PrintDoc.DefaultPageSettings.PaperSize = m_PrintDoc.PrinterSettings.PaperSizes.Item(0)
            End Set
        End Property

        ''' <summary>
        ''' 印刷方向 (True:横 False:縦)
        ''' </summary>
        ''' <value>(True:横  false:縦)</value>
        Public Overloads Property prop_Landscape() As Boolean
        Get
            Return m_PrintDoc.DefaultPageSettings.Landscape
        End Get
        Set(ByVal Value As Boolean)
            m_PrintDoc.DefaultPageSettings.Landscape = Value
        End Set
        End Property

        ''' <summary>
        ''' 印刷枚数
        ''' </summary>
        ''' <value>枚数</value>
        Public Overloads Property prop_PrintCount() As Short
        Get
            Return m_PrintDoc.PrinterSettings.Copies
        End Get
        Set(ByVal Value As Short)
            m_PrintDoc.PrinterSettings.Copies = Value
        End Set
        End Property

        ''' <summary>
        ''' ストレッチ機能 有効無効 (1ページを超えない最大サイズで印刷)
        ''' </summary>
        ''' <value>(True:有効  false:無効)</value>
        Public Overloads Property prop_Strach() As Boolean
        Get
            Return m_Strach
        End Get
        Set(ByVal Value As Boolean)
            m_Strach = Value
        End Set
        End Property

        ''' <summary>
        ''' 余白 (左、右、上、下 の順) [1/100インチ]
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Overloads Property prop_Margins() As Printing.Margins
        Get
            Return m_PrintDoc.DefaultPageSettings.Margins
        End Get
        Set(ByVal Value As Printing.Margins)
            m_PrintDoc.DefaultPageSettings.Margins = Value
        End Set
        End Property
#End Region

        ''' <summary>
        ''' プリンタの有無を調査する
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function IsCheckPrinter() As Boolean
            '' インストールされているプリンターがある？？
            If 0 = PrinterSettings.InstalledPrinters.Count Then
                Return False
            End If
            '' デフォルトプリンタがある？
            Using pd As New System.Drawing.Printing.PrintDocument
                If Not pd.DefaultPageSettings.PrinterSettings.IsDefaultPrinter Then
                    Return False
                End If

                '' デフォルトプリンタが使用不可能な場合
                If False = pd.PrinterSettings.IsValid Then
                    Return False
                End If

            End Using

            '' ここまで来たら使用可能なプリンタ有り
            Return True
        End Function


        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' プリンター無し (印刷はできないがハードコピーは可能な為)
            If Not IsCheckPrinter() Then Return

            ''インスタンス作成
            m_PrintDoc = New System.Drawing.Printing.PrintDocument
            AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage   ' PrintDoc のイベントハンドラを追加
            '' 印刷中 ダイヤログ表示禁止
            m_PrintDoc.PrintController = New System.Drawing.Printing.StandardPrintController

            '' 印刷プロパティ 初期化
            Me.prop_Printer = m_PrintDoc.DefaultPageSettings.PrinterSettings.PrinterName    'デフォルトプリンタで印刷する  
            Me.setMargins(10, 10, 10, 10)           '余白 (0にしても、プリンタが未対応なら、ちょっと余白が空いて表示される。そして、ずれた分 最後が切れる)

            Me.prop_PrintSize = "A4"        '用紙サイズ：A4
            Me.prop_Landscape = True        '印刷方向：横
            Me.prop_PrintCount = 1          '印刷枚数：1枚
        End Sub

        ''' <summary>
        ''' デストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Dispose() Implements System.IDisposable.Dispose

            '' インスタンス開放
            If Not m_Bitmap Is Nothing Then m_Bitmap.Dispose()
            If Not m_Preview Is Nothing Then m_Preview.Dispose()
            If Not m_PageSetup Is Nothing Then m_PageSetup.Dispose()
            If Not m_PrintDialog Is Nothing Then m_PrintDialog.Dispose()

            If Not m_PrintDoc Is Nothing Then m_PrintDoc.Dispose()

            '' GCにFinalizeを行わないように通知
            GC.SuppressFinalize(Me)
        End Sub
        Protected Overrides Sub Finalize()
            Dispose()

            MyBase.Finalize()
        End Sub

        ''' <summary>
        ''' プリントダイヤログ インスタンス生成
        ''' </summary>
        ''' <remarks>どのプリンタに印刷するか</remarks>
        Private Sub PrintDialog_Inctance()
            '' インスタンス生成済みかチェック
            If Not m_PrintDialog Is Nothing Then Exit Sub

            '' インスタンス生成 & 設定
            m_PrintDialog = New System.Windows.Forms.PrintDialog
            m_PrintDialog.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' 印刷設定ダイヤログ インスタンス生成
        ''' </summary>
        ''' <remarks>どの用紙にするか</remarks>
        Private Sub PageSetupDialog_Inctance()
            '' インスタンス生成済みかチェック
            If Not m_PageSetup Is Nothing Then Exit Sub

            '' インスタンス生成 & 設定
            m_PageSetup = New System.Windows.Forms.PageSetupDialog
            m_PageSetup.PrinterSettings = m_PrintDoc.PrinterSettings
            m_PageSetup.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' 印刷プレビュー インスタンス生成
        ''' </summary>
        ''' <remarks>印刷イメージ</remarks>
        Private Sub PrintPreviewDialog_Inctance()
            '' インスタンス生成済みかチェック
            If Not m_Preview Is Nothing Then Exit Sub

            '' インスタンス生成 & 設定
            m_Preview = New System.Windows.Forms.PrintPreviewDialog
            m_Preview.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' プリントダイヤログ 表示
        ''' </summary>
        ''' <returns>True:印刷する False:印刷しない</returns>
        ''' <remarks>どのプリンタに印刷するか</remarks>
        Public Function PrintDialog_show() As Boolean
            Dim retc As System.Windows.Forms.DialogResult
            '' インスタンス生成
            PrintDialog_Inctance()
            retc = m_PrintDialog.ShowDialog()
            '' 戻り値セット
            If retc = DialogResult.OK Then Return True
            Return False
        End Function

        ''' <summary>
        ''' 印刷設定ダイヤログ 表示
        ''' </summary>
        ''' <returns>True:印刷する False:印刷しない</returns>
        ''' <remarks>どの用紙にするか</remarks>
        Public Function PageSetupDialog_show() As Boolean
            Dim retc As System.Windows.Forms.DialogResult
            '' インスタンス生成 & 設定
            PageSetupDialog_Inctance()
            retc = m_PageSetup.ShowDialog()
            '' 戻り値セット
            If retc = DialogResult.OK Then Return True
            Return False
        End Function

        ''' <summary>
        ''' 印刷プレビュー 表示
        ''' </summary>
        ''' <param name="cntObj">ハードコピーをとるコントロール</param>
        ''' <returns>True:印刷する False:印刷しない</returns>
        ''' <remarks>印刷イメージ</remarks>
        Public Function PrintPreviewDialog_show(ByVal cntObj As Control) As Boolean
            Dim retc As System.Windows.Forms.DialogResult

            Try
                HCopy(cntObj)
                '' インスタンス生成 & 設定
                PrintPreviewDialog_Inctance()
                retc = m_Preview.ShowDialog()
                '' 戻り値セット
                If retc = DialogResult.OK Then Return True
                Return False
            Catch ex As Exception
                Return False
            End Try
        End Function

        '----------------------------------------------------------------
        ' 概要：
        ' 引数：(in :Control) cnt = 
        ' 戻値：なし
        ' 備考：
        '----------------------------------------------------------------
        ''' <summary>
        ''' 印刷開始
        ''' </summary>
        ''' <param name="cntObj">ハードコピーをとるコントロール (通常:Meを指定しておけばOK)</param>
        ''' <remarks></remarks>
        Public Sub Print(ByVal cntObj As Control)
            Dim ii As Integer

            '' フォームイメージ取得
            HCopy(cntObj)
            '' 印刷
            'なぜか 最初の1回目がエラーになる
            'デバックで1行づつ動かしていくと、たまに正常に行く時もある・・・？
            For ii = 1 To 3
                Try
                    m_PrintDoc.Print()      '印刷開始
                    Exit For
                Catch ex As Exception
                End Try
            Next ii
        End Sub

        '￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
        ' 概要：
        '＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
        ''' <summary>
        ''' 印刷イベント
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
            '' 印刷倍率決定
            Dim ritu As Single = 1
            If m_Strach Then ritu = ImageSize(e)

            '' グラフィックオブジェクトにセット
            e.Graphics.DrawImage(m_Bitmap, e.MarginBounds.X, e.MarginBounds.Y, m_Bitmap.Width * ritu, m_Bitmap.Height * ritu)
            '次のページがないことを通知する
            e.HasMorePages = False
        End Sub

        ''' <summary>
        ''' ストレッチ設定 (1ページを超えない最大サイズを算出する)
        ''' </summary>
        ''' <param name="e">印刷イベントオブジェクト</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ImageSize(ByVal e As System.Drawing.Printing.PrintPageEventArgs) As Single
            Dim rituX As Single         'X倍率
            Dim rituY As Single         'Y倍率
            Dim margin As System.Drawing.Printing.Margins
            '' 拡大縮小倍率を算出
            margin = Me.prop_Margins
            rituX = CSng((e.Graphics.VisibleClipBounds.Width - margin.Top - margin.Bottom) / m_Bitmap.Width)
            rituY = CSng((e.Graphics.VisibleClipBounds.Height - margin.Left - margin.Right) / m_Bitmap.Height)

            '' ビットマップの拡大縮小倍率を決定する (倍率のより小さい方)
            If rituX < rituY Then
                Return rituX
            Else
                Return rituY
            End If
        End Function

        ''' <summary>
        ''' 余白サイズを決定する
        ''' </summary>
        ''' <param name="left">[mm]</param>
        ''' <param name="right"></param>
        ''' <param name="top"></param>
        ''' <param name="bottom"></param>
        ''' <remarks>設定データは [1/100インチ]の為、変換する関数(0にしても、プリンタが未対応なら、ちょっと余白が空いて表示される。そして、ずれた分 最後が切れる)</remarks>
        Public Sub setMargins(ByVal left As Integer, ByVal right As Integer, ByVal top As Integer, ByVal bottom As Integer)
            '' プリンタで表示出来ない部分から、余白設定される為 その部分を紙する
            If left < mc_DefMagin_LEFT Then      '表示出来ない部分
                left = mc_DefMagin_LEFT
            Else
                left -= mc_DefMagin_LEFT
            End If
            If top < mc_DefMagin_TOP Then      '表示出来ない部分
                top = mc_DefMagin_TOP
            Else
                top -= mc_DefMagin_TOP
            End If
            If right < mc_DefMagin_RIGHT Then      '表示出来ない部分
                right = mc_DefMagin_RIGHT
            Else
                right -= mc_DefMagin_RIGHT
            End If
            If bottom < mc_DefMagin_BOT Then      '表示出来ない部分
                bottom = mc_DefMagin_BOT
            Else
                bottom -= mc_DefMagin_BOT
            End If

            '' [mm] → [1/100インチ]
            left = CInt(left / 0.254)
            right = CInt(right / 0.254)
            top = CInt(top / 0.254)
            bottom = CInt(bottom / 0.254)

            '' 余白設定
            Me.prop_Margins = New System.Drawing.Printing.Margins(left, right, top, bottom)
        End Sub

        ''' <summary>
        ''' 指定コントロールのハードコピーを取得する
        ''' </summary>
        ''' <param name="cnt">ハードコピーをとるコントロール</param>
        ''' <param name="bitImg">ハードコピーを取得したビットマップ</param>
        ''' <remarks></remarks>

        Public Overloads Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
            Dim gp As Graphics = cnt.CreateGraphics                  'フォームのGraphicオブジェクトの作成
            bitImg = New Bitmap(cnt.Width, cnt.Height, gp)  'ビットマップ領域確保
            Dim fomG As Graphics = Graphics.FromImage(bitImg)
            'Dim Hdc1 As IntPtr = gp.GetHdc()   'ポインタ取得  (これだとタイトルバーの部分が取得出来ない)
            ' 20180702 キャプチャー不具合対応 ----->>>
            'Dim Hdc1 As IntPtr = GetWindowDC(cnt.Handle)
            Dim Hdc1 As IntPtr = GetWindowDC(IntPtr.Zero)
            ' 20180702 キャプチャー不具合対応 <<<-----
            Dim Hdc2 As IntPtr = fomG.GetHdc()  'ポインタ取得

            '' イメージ取得
            ' 20180702 キャプチャー不具合対応 ----->>>
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY)
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY Or CAPTUREBLT)
            BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, cnt.Left, cnt.Top, SRCCOPY Or CAPTUREBLT)
            ' 20180702 キャプチャー不具合対応 <<<-----

            '' 使ったインスタンス開放
            'gp.ReleaseHdc(Hdc1)
            'ReleaseDC(cnt.Handle, Hdc1)
            ' 20180702 キャプチャー不具合対応 ----->>>
            ReleaseDC(IntPtr.Zero, Hdc1)
            ' 20180702 キャプチャー不具合対応 <<<-----
            fomG.ReleaseHdc(Hdc2)
            gp.Dispose()
        End Sub

        ''' <summary>
        ''' 指定コントロールのハードコピーを保存 (bitmap)
        ''' </summary>
        ''' <param name="cnt">ハードコピーをとるコントロール</param>
        ''' <param name="strPath">ファイルフルパス</param>
        ''' <remarks></remarks>
        Public Overloads Sub HCopy(ByVal cnt As Control, ByVal strPath As String)
            Dim bitImg As Bitmap = Nothing
            HCopy(cnt, bitImg)
            bitImg.Save(strPath, System.Drawing.Imaging.ImageFormat.Bmp)
            bitImg.Dispose()
        End Sub

        ''' <summary>
        ''' 指定コントロールのハードコピーを保存 (形式指定)
        ''' </summary>
        ''' <param name="cnt">ハードコピーをとるコントロール</param>
        ''' <param name="strPath">ファイルフルパス</param>
        ''' <param name="format">フォーマット(ImageFormat.jpeg, ImageFormat.bmp) </param>       
        ''' <remarks></remarks>
        Public Overloads Sub HCopy(ByVal cnt As Control, ByVal strPath As String, ByVal format As System.Drawing.Imaging.ImageFormat)
            Dim bitImg As Bitmap = Nothing
            HCopy(cnt, bitImg)
            bitImg.Save(strPath, format)
            bitImg.Dispose()
        End Sub

        ''' <summary>
        ''' 指定コントロールのハードコピーを取得する
        ''' </summary>
        ''' <param name="cnt">ハードコピーをとるコントロール</param>
        ''' <remarks></remarks>
        Private Overloads Sub HCopy(ByVal cnt As Control)
            Dim gp As Graphics = cnt.CreateGraphics                 'フォームのGraphicオブジェクトの作成
            m_Bitmap = New Bitmap(cnt.Width, cnt.Height, gp)  'ビットマップ領域確保
            Dim fomG As Graphics = Graphics.FromImage(m_Bitmap)
            'Dim Hdc1 As IntPtr = gp.GetHdc()   'ポインタ取得
            ' 20200514 キャプチャー不具合対応 ----->>>
            'Dim Hdc1 As IntPtr = GetWindowDC(cnt.Handle)
            Dim Hdc1 As IntPtr = GetWindowDC(IntPtr.Zero)
            ' 20200514 キャプチャー不具合対応 <<<-----
            Dim Hdc2 As IntPtr = fomG.GetHdc()  'ポインタ取得

            '' イメージ取得
            ' 20200514 キャプチャー不具合対応 ----->>>
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY)
            BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, cnt.Left, cnt.Top, SRCCOPY Or CAPTUREBLT)
            ' 20200514 キャプチャー不具合対応 <<<-----

            '' 使ったインスタンス開放
            'gp.ReleaseHdc(Hdc1)
            ' 20200514 キャプチャー不具合対応 ----->>>
            'ReleaseDC(cnt.Handle, Hdc1)
            ReleaseDC(IntPtr.Zero, Hdc1)
            ' 20200514 キャプチャー不具合対応 <<<-----
            fomG.ReleaseHdc(Hdc2)
            gp.Dispose()
        End Sub

    End Class
End Namespace





#Region "バック"

''''Namespace tClasstest


''''    Public Class tClass_HeadCopy
''''        Implements IDisposable                          'デストラクタ定義


''''        '********************************************************************
''''        'API定義
''''        '********************************************************************
''''        '画面のイメージを取得
''''        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
''''        Const SRCCOPY As Integer = &HCC0020

''''        '********************************************************************
''''        'メンバー変数定義
''''        '********************************************************************
''''        Private m_PrintDoc As System.Drawing.Printing.PrintDocument      'プリント制御コントロール
''''        Private m_PrintDialog As System.Windows.Forms.PrintDialog       'プリントダイヤログ
''''        'Private mPrintSet As System.Windows.Forms.PageSetupDialog      '印刷設定ダイヤログ
''''        'Private mPrintView As System.Windows.Forms.PrintPreviewDialog      '印刷プレビュー
''''        Private m_PrintPoint As System.Drawing.Point                     '印刷開始位置

''''       ' Private m_bmp As Bitmap         '画面イメージ
''''        Private m_cntObj As Control
''''        Private m_pointF As PointF      'イメージ表示位置(左上の座標)

''''        Public Sub New()
''''            ''' イメージ表示位置
''''            m_pointF = New PointF(0, 0)








''''                ''' プリンタ設定
''''                Dim PSize As System.Drawing.Printing.PaperSize          '用紙サイズ変更

''''                'インスタンス作成
''''                m_PrintDoc = New System.Drawing.Printing.PrintDocument
'''' '               m_PrintDialog = New System.Windows.Forms.PrintDialog
''''        '        mPrintSet = New System.Windows.Forms.PageSetupDialog
''''        'mPrintView = New System.Windows.Forms.PrintPreviewDialog

''''                'PrintDoc のイベントハンドラを追加
''''                AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

''''                '印刷プレビューとプリントドキュメントを関連付け
''''  '              m_PrintDialog.Document = m_PrintDoc
''''        '        mPrintSet.Document = mPrintDoc
''''        'mPrintView.Document = mPrintDoc
''''                '''データセット
''''                '印刷方向 (True:横  false:縦)
''''                'mPrintSet.PageSettings.Landscape = True
''''                '印刷プレビュー表示フラグ

''''                '用紙サイズ取得
''''                For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
''''                    If PSize.PaperName.StartsWith("A4") Then
''''                        '用紙サイズ設定
''''                        m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
''''                        Exit For
''''                    End If
''''                Next

''''                '印刷枚数セット
''''                m_PrintDoc.PrinterSettings.Copies = 1

''''        End Sub

''''        Public Sub Dispose() Implements System.IDisposable.Dispose
''''           ' If Not m_bmp Is Nothing Then m_bmp.Dispose()

''''        End Sub


''''        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
''''            Dim bmp As Bitmap         '画面イメージ
''''            HCopy(m_cntObj, bmp)
''''            e.Graphics.DrawImage(bmp, m_pointF)
''''            bmp.Dispose()
''''        End Sub



''''        Public Sub Print(ByVal cntObj As Control)
''''            m_cntObj = cntObj
''''            Try
'''' '               ''' プリンタ設定
'''' '               Dim PSize As System.Drawing.Printing.PaperSize          '用紙サイズ変更

'''' '               'インスタンス作成
'''' '               m_PrintDoc = New System.Drawing.Printing.PrintDocument
'''' ''               m_PrintDialog = New System.Windows.Forms.PrintDialog
'''' '       '        mPrintSet = New System.Windows.Forms.PageSetupDialog
'''' '       'mPrintView = New System.Windows.Forms.PrintPreviewDialog

'''' '               'PrintDoc のイベントハンドラを追加
'''' '               AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

'''' '               '印刷プレビューとプリントドキュメントを関連付け
'''' ' '              m_PrintDialog.Document = m_PrintDoc
'''' '       '        mPrintSet.Document = mPrintDoc
'''' '       'mPrintView.Document = mPrintDoc
'''' '               '''データセット
'''' '               '印刷方向 (True:横  false:縦)
'''' '               'mPrintSet.PageSettings.Landscape = True
'''' '               '印刷プレビュー表示フラグ

'''' '               '用紙サイズ取得
'''' '               For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
'''' '                   If PSize.PaperName.StartsWith("A4") Then
'''' '                       '用紙サイズ設定
'''' '                       m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
'''' '                       Exit For
'''' '                   End If
'''' '               Next

'''' '               '印刷枚数セット
'''' '               m_PrintDoc.PrinterSettings.Copies = 1
'''' m_PrintDoc.DocumentName = "test1"
''''                m_PrintDoc.Print()
''''            Catch ex As Exception
''''MsgBox("")
''''    End Try


''''        End Sub




''''        '----------------------------------------------------------------
''''        ' 概要：ハードコピーを取得する
''''        ' 引数：(in :)  = 
''''        ' 戻値：なし
''''        ' 備考：
''''        '----------------------------------------------------------------
''''        Public Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
''''            Dim gp As Graphics = cnt.CreateGraphics                 'フォームのGraphicオブジェクトの作成
''''            bitImg = New Bitmap(cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, gp)  'ビットマップ領域確保
''''            Dim fomG As Graphics = gp.FromImage(bitImg)
''''            Dim Hdc1 As IntPtr = gp.GetHdc()    'ポインタ取得
''''            Dim Hdc2 As IntPtr = fomG.GetHdc()  'ポインタ取得

''''            ''' イメージ取得
''''            BitBlt(Hdc2, 0, 0, cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, Hdc1, 0, 0, SRCCOPY)

''''            ''' 使ったインスタンス開放
''''            gp.ReleaseHdc(Hdc1)
''''            fomG.ReleaseHdc(Hdc2)
''''            gp.Dispose()
''''        End Sub
''''    End Class
''''End Namespace

#End Region
#Region "バック"

''''Namespace tClasstest2


''''    Public Class tClass_HeadCopy
''''        Implements IDisposable                          'デストラクタ定義


''''        '********************************************************************
''''        'API定義
''''        '********************************************************************
''''        '画面のイメージを取得
''''        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
''''        Const SRCCOPY As Integer = &HCC0020

''''        '********************************************************************
''''        'メンバー変数定義
''''        '********************************************************************
''''        Private m_PrintDoc As System.Drawing.Printing.PrintDocument      'プリント制御コントロール
''''        Private m_PrintDialog As System.Windows.Forms.PrintDialog       'プリントダイヤログ

''''        Private m_PrintPoint As System.Drawing.Point                     '印刷開始位置


''''        Private m_cntObj As Control
''''        Private m_pointF As PointF      'イメージ表示位置(左上の座標)

''''        Public Sub New()
''''            ''' イメージ表示位置
''''            m_pointF = New PointF(0, 0)

''''                ''' プリンタ設定
''''                Dim PSize As System.Drawing.Printing.PaperSize          '用紙サイズ変更

''''                'インスタンス作成
''''                m_PrintDoc = New System.Drawing.Printing.PrintDocument
''''                'PrintDoc のイベントハンドラを追加
''''                AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

''''                '用紙サイズ取得
''''                For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
''''                    If PSize.PaperName.StartsWith("A4") Then
''''                        '用紙サイズ設定
''''                        m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
''''                        Exit For
''''                    End If
''''                Next

''''                '印刷枚数セット
''''                m_PrintDoc.PrinterSettings.Copies = 1
''''        End Sub

''''        Public Sub Dispose() Implements System.IDisposable.Dispose
''''        End Sub


''''        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
''''            Dim bmp As Bitmap         '画面イメージ
''''            HCopy(m_cntObj, bmp)
''''            e.Graphics.DrawImage(bmp, m_pointF)
''''            bmp.Dispose()
''''        End Sub



''''        Public Sub Print(ByVal cntObj As Control)
''''            m_cntObj = cntObj
''''            Try
'''' m_PrintDoc.DocumentName = "test2"
''''                m_PrintDoc.Print()
''''            Catch ex As Exception
''''MsgBox("")
''''    End Try


''''        End Sub

''''        '----------------------------------------------------------------
''''        ' 概要：ハードコピーを取得する
''''        ' 引数：(in :)  = 
''''        ' 戻値：なし
''''        ' 備考：
''''        '----------------------------------------------------------------
''''        Public Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
''''            Dim gp As Graphics = cnt.CreateGraphics                 'フォームのGraphicオブジェクトの作成
''''            bitImg = New Bitmap(cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, gp)  'ビットマップ領域確保
''''            Dim fomG As Graphics = gp.FromImage(bitImg)
''''            Dim Hdc1 As IntPtr = gp.GetHdc()    'ポインタ取得
''''            Dim Hdc2 As IntPtr = fomG.GetHdc()  'ポインタ取得

''''            ''' イメージ取得
''''            BitBlt(Hdc2, 0, 0, cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, Hdc1, 0, 0, SRCCOPY)

''''            ''' 使ったインスタンス開放
''''            gp.ReleaseHdc(Hdc1)
''''            fomG.ReleaseHdc(Hdc2)
''''            gp.Dispose()
''''        End Sub
''''    End Class
''''End Namespace
#End Region