'*********************************************************************************
'Excel操作の基底クラス
'	[Ver]
'		Ver.01    2006/02/11  vs2005 対応
'		Ver.02    2006/03/22  getColumnName 不具合対応
'		Ver.03    2007/03/06  切り替えを追加
'		Ver.04    2008/01/08  実行環境に対応（ほぼ全部作り直し）
'       Ver.05    2008/01/11  CopySheet処理追加
'
'	[メモ]
'		☆本クラスは、Microsoft Excel 9.0 Object Library を使用して関数等を作成しています。(Excel2000ベース)
'		本クラスは、参照設定（参照）で以下のDllを追加しています。(ローカルコピー禁止。その為、Excel使用時は、Exeと同一階層に、ExcelDll2000のdllを全てコピー)
'			・Interop.Excel.dll						(これがメイン)
'			・Interop.Office.dll					(今の所、Pictuerにしかつかわれていない)
'			・Interop.Microsoft.Office.Core.dll		(今の所、Pictuerにしかつかわれていない) (これは2003ベース時)
'			（場所は、ローカルのExcelDll2000です。） 
'		実行環境でExcelを使用したい場合は、Exeと同じフォルダ内に、ExcelDll2000のdllを全てコピーしておけば、Excel2000でも2003でも2007でも実行できます。
'		COMを厳密に開放しているのは、Web(ASP.NET)で使用する際に必要 (ユーザーが普通と違う為、よく終了してもExcelが残る為)
'
'	[メモ2]
'		ExcelDll2000,2003とあるのは、以下の為です。
'						実行PC
'		コンパイル   2000	2003	2007
'			2000	  ○	 ○		 ○
'			2003	  ×	 ○		 ○
'			2007	  コンパイルが通らない
'*********************************************************************************

Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)


'********************************************************************
'切り替え
'********************************************************************
'' 本クラスを未使用時はスイッチしてください ←たぶん Ver.04からは、未使用でOKのはず。 (ローカルのDLLを直で参照している為)
#Const tExcelON = True						' 当クラスを使用しない場合は、False

'' PCでスイッチしてください ←たぶん Ver.04からは、2000固定でOKのはず。（参照しているDLLを2000固定にした為）
#Const tExcelVer2000 = 2000						' Excel 2000 用
'#Const tExcelVer2003 = 2003						' Excel 2003 用



'********************************************************************
'実際の処理
'********************************************************************
#If tExcelON Then			'// ---------------------------------------------->>>>>

' Imports Excel					' ライブラリは全部明示的に書いておきたい為、ヤメ
' Imports Microsoft.Office		' ライブラリは全部明示的に書いておきたい為、ヤメ
Imports System.Drawing

Namespace tClass
    ''' <summary>
    ''' Excel操作の基底クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Excel_Base
        Implements IDisposable							' デストラクタ定義

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private disposedValue As Boolean = False				' 重複する呼び出しを検出するには
        Protected m_xlsApp As Excel.ApplicationClass = Nothing	' これ1つで1つのExcel アプリケーション プロセス
        Protected m_xlsBooks As Excel.Workbooks = Nothing		' ブックス
        Protected m_xlsBook As Excel.Workbook = Nothing			' 現在のアクティブ ブック
        Protected m_xlsSheets As Excel.Sheets = Nothing			' シートス
        Protected m_xlsSheet As Excel.Worksheet = Nothing		' 現在のアクティブ シート

        '********************************************************************
        'プロパティ定義
        '********************************************************************
#Region "プロパティ"
        '====================================================================
        'ワークブック取得 プロパティーの設定
        '====================================================================
        ''' <summary>
        ''' 現在のアクティブ ブック の取得・設定
        ''' </summary>
        ''' <value>切り替え先ワークブック</value>
        ''' <returns>現在のワークブック</returns>
        ''' <remarks></remarks>
        Public Overloads Property XlsBook() As Excel.Workbook
            Get
                Return m_xlsBook
            End Get
            Set(ByVal Value As Excel.Workbook)
                '' ブックの切り替え
                m_xlsBook = Value
                '' ブックが切り替わるとシートも変更する
                MyClass.MRComObject(m_xlsSheets)
                m_xlsSheets = m_xlsBook.Worksheets
                MyClass.MRComObject(m_xlsSheet)
                m_xlsSheet = CType(m_xlsSheets.Item(1), Excel.Worksheet)
            End Set
        End Property

        ''' <summary>
        ''' 指定 ブック の取得
        ''' </summary>
        ''' <param name="Index">ブック番号 1ｵﾘｼﾞﾝ</param>
        ''' <value></value>
        ''' <returns>現在のワークブック</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsBook(ByVal Index As Integer) As Excel.Workbook
            Get
                Try
                    '' 引数チェック
                    If index <= 0 Or index > m_xlsBooks.Count Then Return Nothing
                    '' ブック取得
                    Return CType(m_xlsBooks.Item(Index), Excel.Workbook)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property
        ''' <param name="strName">"test"でも"test.xls"でも"c:\test.xls"でも 指定可能</param>
        ''' <value></value>
        ''' <returns>現在のワークブック</returns>
        Public Overloads ReadOnly Property XlsBook(ByVal strName As String) As Excel.Workbook
            Get
                '''' ファイル名に変換
                'ファイル名取得
                Dim strFileName As String = IO.Path.GetFileName(strName)		'ブックのファイル名
                If strFileName = "" Then Return Nothing '空白 又は "c:\test\" 形式

                '拡張子チェック
                Dim wk As String = IO.Path.GetExtension(strFileName).ToLower
                If Not wk = ".xls" Then	'"test" 又は "test.aaa" 形式 
                    If wk = "" Then
                        '拡張子を付与
                        strFileName &= ".xls"
                    Else
                        Return Nothing
                    End If
                End If

                '''' ブック取得
                Try
                    Return CType(m_xlsBooks.Item(strFileName), Excel.Workbook)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        '====================================================================
        'ワークシート取得 プロパティーの設定
        '====================================================================
        ''' <summary>
        ''' 現在のアクティブ シート の取得・設定
        ''' </summary>
        ''' <value>シート</value>
        ''' <returns>アクティブシート</returns>
        ''' <remarks></remarks>
        Public Overloads Property XlsSheet() As Excel.Worksheet
            Get
                Return m_xlsSheet
            End Get
            Set(ByVal Value As Excel.Worksheet)
                m_xlsSheet = Value
            End Set
        End Property
        ''' <summary>
        ''' 指定 シート の取得・設定
        ''' </summary>
        ''' <param name="Index">1ｵﾘｼﾞﾝ</param>
        ''' <value></value>
        ''' <returns>シート</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsSheet(ByVal Index As Integer) As Excel.Worksheet
            Get
                '''' 引数チェック
                If index <= 0 Or index > m_xlsSheets.Count Then Return Nothing
                Try
                    Return CType(m_xlsSheets.Item(Index), Excel.Worksheet)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        ''' <summary>
        ''' 指定 シート の取得・設定
        ''' </summary>
        ''' <param name="strName">"Sheet1"</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsSheet(ByVal strName As String) As Excel.Worksheet
            Get
                '''' 引数チェック
                If strName = "" Then Return Nothing
                Try
                    Return CType(m_xlsSheets.Item(strName), Excel.Worksheet)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        '====================================================================
        'セル値 プロパティーの設定
        '====================================================================
        ''' <summary>
        ''' アクティブシート の 指定セルのデータ の取得・設定
        ''' </summary>
        ''' <param name="row">行</param>
        ''' <param name="col">列</param>
        ''' <value>文字列</value>
        ''' <returns>セル値</returns>
        ''' <remarks></remarks>
        Public Overloads Property CellVal(ByVal row As Integer, ByVal col As Integer) As String
            Get
                If row < 1 And col < 1 Then Return Nothing
                'Return m_xlsSheet.Cells(row, col).ToString()
                'Return CStr(CType(m_xlsSheet.Cells(row, col), Excel.Range).Value)
                Return CType(m_xlsSheet.Cells(row, col), Excel.Range).Value.ToString
            End Get
            Set(ByVal Value As String)
                'Dim rang As Excel.Range
                'rang = m_xlsSheet.Cells(row, col)
                'rang.Value = Value
                'MRComObject(rang)
                If m_xlsSheet Is Nothing Then Exit Property
                CType(m_xlsSheet.Cells(row, col), Excel.Range).Value = Value
            End Set
        End Property

        ''' <summary>
        ''' アクティブシート の 指定範囲セルのデータ の取得・設定
        ''' </summary>
        ''' <param name="address">"A1"の形式 ("A1:A1"でも可)</param>
        ''' <value>文字列</value>
        ''' <returns>セル値</returns>
        ''' <remarks></remarks>
        Public Overloads Property CellVal(ByVal address As String) As String
            Get
                If address = "" Then Return Nothing
                Return CStr(m_xlsSheet.Range(address).Value)
            End Get
            Set(ByVal Value As String)
                m_xlsSheet.Range(address).Value = Value
            End Set
        End Property
        '====================================================================
        'セルのオブジェクト プロパティーの設定
        '====================================================================

        ''' <summary>
        ''' 行単位のオブジェクト の取得・設定
        ''' </summary>
        ''' <param name="row">行位置</param>
        ''' <param name="cnt">行数</param>
        ''' <value>セルオブジェクト</value>
        ''' <returns>セルオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads Property Rows(ByVal row As Integer, Optional ByVal cnt As Integer = 1) As Excel.Range
            Get
                If row < 1 And cnt < 1 Then Return Nothing
                Return CType(m_xlsSheet.Rows(String.Format("{0}:{1}", row, row + cnt - 1)), Excel.Range)
            End Get
            Set(ByVal cell As Excel.Range)
                m_xlsSheet.Rows(String.Format("{0}:{1}", row, row + cnt - 1)) = cell
            End Set
        End Property
        ''' <summary>
        ''' 列単位のオブジェクト の取得・設定
        ''' </summary>
        ''' <param name="col">列位置</param>
        ''' <param name="cnt">列数</param>
        ''' <value>セルオブジェクト</value>
        ''' <returns>セルオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads Property Columns(ByVal col As Integer, Optional ByVal cnt As Integer = 1) As Excel.Range
            Get
                If col < 1 And cnt < 1 Then Return Nothing
                Return CType(m_xlsSheet.Columns(GetColumnName(col, col + cnt - 1)), Excel.Range)

            End Get
            Set(ByVal value As Excel.Range)
                m_xlsSheet.Columns(GetColumnName(col, col + cnt - 1)) = value
            End Set
        End Property

        ''' <summary>
        ''' アクティブシート の 指定セルのオブジェクト の取得・設定
        ''' </summary>
        ''' <param name="row">行</param>
        ''' <param name="col">列</param>
        ''' <value>セルオブジェクト</value>
        ''' <returns>セルオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads Property Cells(ByVal row As Integer, ByVal col As Integer) As Excel.Range
            Get
                If row < 1 And col < 1 Then Return Nothing
                Return CType(m_xlsSheet.Cells(row, col), Excel.Range)
            End Get
            Set(ByVal cell As Excel.Range)
                m_xlsSheet.Cells(row, col) = cell
            End Set
        End Property

        ''' <summary>
        ''' アクティブシート の 指定範囲セルのオブジェクト の取得・設定
        ''' </summary>
        ''' <param name="row1">開始行</param>
        ''' <param name="col1">開始列</param>
        ''' <param name="row2">終了行</param>
        ''' <param name="col2">終了列</param>
        ''' <value></value>
        ''' <returns>セルオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property Cells(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer) As Excel.Range
            Get
                If row1 < 1 And col1 < 1 And row2 < 1 And col2 < 1 Then Return Nothing
                Return m_xlsSheet.Range(m_xlsSheet.Cells(row1, col1), m_xlsSheet.Cells(row2, col2))
            End Get
        End Property
        ''' <summary>
        ''' アクティブシート の 指定範囲セルのオブジェクトの取得・設定
        ''' </summary>
        ''' <param name="address">"A1"の形式 ("A1:A1"でも可)</param>
        ''' <value></value>
        ''' <returns>セルオブジェクト</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property Cells(ByVal address As String) As Excel.Range
            Get
                If address = "" Then Return Nothing
                Return m_xlsSheet.Range(address)
            End Get
        End Property

        Public Shared Function ObjToString(ByVal pf As Object) As String
            Return CStr(pf)
        End Function

        ''' <summary>
        ''' 配列の情報を一括で設定・取得する
        ''' </summary>
        ''' <param name="row1">開始行</param>
        ''' <param name="col1">開始列</param>
        ''' <param name="row2">終了行</param>
        ''' <param name="col2">終了列</param>
        ''' <value>Zの順で、データセット</value>
        ''' <returns>Zの順で、データ取得</returns>
        ''' <remarks></remarks>
        Public Overloads Property CellsVal(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer) As String(,)
            Get
                Dim sWk(row2 - row1, col2 - col1) As String
                For ii As Integer = row1 To row2
                    For jj As Integer = col1 To col2
                        sWk(ii - row1, jj - col1) = CellVal(ii, jj)
                    Next
                Next
                Return sWk
                'Return CType(m_xlsSheet.Range(m_xlsSheet.Cells(row1, col1), m_xlsSheet.Cells(row2, col2)).Value, String(,))
            End Get
            Set(ByVal value As String(,))
                Try
                    m_xlsSheet.Range(m_xlsSheet.Cells(row1, col1), m_xlsSheet.Cells(row2, col2)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property
        Public Overloads WriteOnly Property CellsVal(ByVal row As Integer, ByVal col As Integer) As String(,)
            Set(ByVal value As String(,))
                Dim nRowCnt As Integer = value.GetUpperBound(0)
                Dim nColCnt As Integer = value.GetUpperBound(1)

                Try
                    m_xlsSheet.Range(m_xlsSheet.Cells(row, col), m_xlsSheet.Cells(row + nRowCnt, col + nColCnt)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property

        ''' <summary>
        ''' 1行の情報を一括で設定・取得する
        ''' </summary>
        ''' <param name="row">指定行</param>
        ''' <param name="col1">終了行</param>
        ''' <param name="col2">終了列</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Property RowsVal(ByVal row As Integer, ByVal col1 As Integer, ByVal col2 As Integer) As String()
            Get
                Dim sWk(col2 - col1) As String
                For ii As Integer = col1 To col2
                    sWk(ii - col1) = CellVal(row, ii)
                Next
                Return sWk
                'Return CType(m_xlsSheet.Range(m_xlsSheet.Cells(row, col1), m_xlsSheet.Cells(row, col2)).Value, String())

            End Get
            Set(ByVal value As String())
                Try
                    If value.Length <> (col2 - col1) + 1 Then Return

                    m_xlsSheet.Range(m_xlsSheet.Cells(row, col1), m_xlsSheet.Cells(row, col2)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property
        ''' <summary>
        ''' 1列の情報を一括で設定・取得する
        ''' </summary>
        ''' <param name="row1">開始行</param>
        ''' <param name="row2">終了行</param>
        ''' <param name="col">指定列</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Property ColumnsVal(ByVal row1 As Integer, ByVal row2 As Integer, ByVal col As Integer) As String()
            Get
                Dim sWk(row2 - row1) As String
                For ii As Integer = row1 To row2
                    sWk(ii - row1) = CellVal(ii, col)
                Next
                Return sWk
                'Return CType(m_xlsSheet.Range(m_xlsSheet.Cells(row1, col), m_xlsSheet.Cells(row2, col)).Value, String())

            End Get
            Set(ByVal value As String())
                Try
                    If value.Length <> (row2 - row1) + 1 Then Return

                    m_xlsSheet.Range(m_xlsSheet.Cells(row1, col), m_xlsSheet.Cells(row2, col)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property

        ' 20130626 オブジェクト版--------------------------------------------------->>>>
        ''' <summary>
        ''' 配列の情報を一括で設定・取得する(オブジェクト形式)
        ''' </summary>
        ''' <param name="row1">開始行</param>
        ''' <param name="col1">開始列</param>
        ''' <param name="row2">終了行</param>
        ''' <param name="col2">終了列</param>
        ''' <value>Zの順で、データセット</value>
        ''' <returns>Zの順で、データ取得</returns>
        ''' <remarks></remarks>
        Public Overloads Property CellsValObject(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer) As Object(,)
            Get
                Dim objWk(row2 - row1, col2 - col1) As Object
                For ii As Integer = row1 To row2
                    For jj As Integer = col1 To col2
                        objWk(ii - row1, jj - col1) = CellVal(ii, jj)
                    Next
                Next
                Return objWk
            End Get
            Set(ByVal value As Object(,))
                Try
                    m_xlsSheet.Range(m_xlsSheet.Cells(row1, col1), m_xlsSheet.Cells(row2, col2)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property
        Public Overloads WriteOnly Property CellsValObject(ByVal row As Integer, ByVal col As Integer) As Object(,)
            Set(ByVal value As Object(,))
                Dim nRowCnt As Integer = value.GetUpperBound(0)
                Dim nColCnt As Integer = value.GetUpperBound(1)

                Try
                    m_xlsSheet.Range(m_xlsSheet.Cells(row, col), m_xlsSheet.Cells(row + nRowCnt, col + nColCnt)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property

        ''' <summary>
        ''' 1行の情報を一括で設定・取得する
        ''' </summary>
        ''' <param name="row">指定行</param>
        ''' <param name="col1">開始行</param>
        ''' <param name="col2">終了列</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Property RowsValObject(ByVal row As Integer, ByVal col1 As Integer, ByVal col2 As Integer) As Object()
            Get
                Dim oWk(col2 - col1) As Object
                For ii As Integer = col1 To col2
                    oWk(ii - col1) = CellVal(row, ii)
                Next
                Return oWk
            End Get
            Set(ByVal value As Object())
                Try
                    If value.Length <> (col2 - col1) + 1 Then Return

                    m_xlsSheet.Range(m_xlsSheet.Cells(row, col1), m_xlsSheet.Cells(row, col2)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property

        ''' <summary>
        ''' 1列の情報を一括で設定・取得する
        ''' </summary>
        ''' <param name="row1">開始行</param>
        ''' <param name="row2">終了行</param>
        ''' <param name="col">指定列</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Property ColumnsValObject(ByVal row1 As Integer, ByVal row2 As Integer, ByVal col As Integer) As Object()
            Get
                Dim oWk(row2 - row1) As Object
                For ii As Integer = row1 To row2
                    oWk(ii - row1) = CellVal(ii, col)
                Next
                Return oWk

            End Get
            Set(ByVal value As Object())
                Try
                    If value.Length <> (row2 - row1) + 1 Then Return

                    m_xlsSheet.Range(m_xlsSheet.Cells(row1, col), m_xlsSheet.Cells(row2, col)).Value = value
                Catch ex As Exception
                End Try
            End Set
        End Property
        ' 20130626 オブジェクト版<<<<---------------------------------------------------

#End Region


        '********************************************************************
        'スタティックメソッド
        '********************************************************************

        ''' <summary>
        ''' Excelがインストールされているか確認
        ''' </summary>
        ''' <returns></returns>
        Public Shared Function Check_Install_Excel() As Boolean
            Dim xlsApp As Excel.ApplicationClass = Nothing

            Try
                '' Excelインスタンス生成でエラー時は、Excelがインストールされていないと判断する
                xlsApp = New Excel.ApplicationClass
                xlsApp.Quit()
                System.Runtime.InteropServices.Marshal.ReleaseComObject(xlsApp)
                Return True

            Catch ex As Exception
                Return False

            Finally
                GC.Collect()
            End Try
        End Function

        ''' <summary>
        ''' 非同期でExcelファイルを起動する
        ''' </summary>
        ''' <param name="strFPath">実行したいExcelファイル</param>
        ''' <returns>起動したプロセス</returns>
        ''' <remarks></remarks>
        Public Shared Function Start_ExcelFile(ByVal strFPath As String) As Process
            Try
                Return System.Diagnostics.Process.Start("Excel", strFPath)

            Catch ex As Exception
                Return Nothing
            End Try
        End Function


        ''' <summary>
        ''' .NETの色指定からVB時の色指定方法に変換する ARGB→RGB
        ''' </summary>
        ''' <param name="color">カラーオブジェクト</param>
        ''' <returns>RGB形式</returns>
        ''' <remarks></remarks>
        Public Shared Function ColorToRGB(ByVal color As Color) As Integer
            Return RGB(color.R, color.G, color.B)
        End Function


        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '''' Excel オブジェクト生成
            m_xlsApp = New Excel.ApplicationClass
            m_xlsBooks = m_xlsApp.Workbooks

            '''' 実行速度向上
            m_xlsApp.ScreenUpdating = False		  '画面更新禁止
            m_xlsApp.Interactive = False		  '非対話モード
            m_xlsApp.DisplayAlerts = False		  'ダイヤログ出力禁止
        End Sub


        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                '''' Excelクローズ
                MyClass.CloseAll()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>使用後、必ずコールする事</remarks>
        Public Overridable Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' Finalizeデストラクタ
        ''' </summary>
        ''' <remarks>Disposeを行っていない場合のみ</remarks>
        Protected Overrides Sub Finalize()
            '''' Excelクローズ
            MyClass.CloseAll()
        End Sub



        '********************************************************************
        ' COMの開放 (Excel開放)
        '********************************************************************
#Region "COMの開放"
        '''' <summary>
        '''' COMの開放
        '''' </summary>
        '''' <param name="objxls">開放するオブジェクト</param>
        '''' <remarks></remarks>
        'Protected Sub MRComObject(ByRef objxls As Object)
        '    '''' COMの開放 詳細は(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
        '    '''' Excel 終了処理時のプロシージャ
        '    If IsNothing(objxls) Then Exit Sub
        '    Try
        '        '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
        '        System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
        '    Catch
        '    Finally
        '        objxls = Nothing
        '    End Try
        'End Sub

        ''' <summary>
        ''' COMの開放
        ''' </summary>
        ''' <param name="objxls">開放するオブジェクト</param>
        ''' <remarks></remarks>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.ApplicationClass)
            '''' COMの開放 詳細は(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
            '''' Excel 終了処理時のプロシージャ
            If IsNothing(objxls) Then Exit Sub
            Try
                '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">開放するオブジェクト</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Workbooks)
            If IsNothing(objxls) Then Exit Sub
            Try
                '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">開放するオブジェクト</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Workbook)
            If IsNothing(objxls) Then Exit Sub
            Try
                '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">開放するオブジェクト</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Sheets)
            If IsNothing(objxls) Then Exit Sub
            Try
                '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">開放するオブジェクト</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Worksheet)
            If IsNothing(objxls) Then Exit Sub
            Try
                '提供されたランタイム呼び出し可能ラッパーの参照カウントをデクリメントします
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
#End Region

        '********************************************************************
        ' 一般的な操作
        '********************************************************************

        ''' <summary>
        ''' ブックをオープン
        ''' </summary>
        ''' <param name="strFPath">Excelファイルフルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function Open(ByVal strFPath As String) As Boolean
            If IsNothing(m_xlsApp) Then Return False
            Try
                '''' Excelファイルオープン
                MyClass.XlsBook = m_xlsBooks.Open(Filename:=strFPath)
                Return True
            Catch ex As Exception
                MyClass.MRComObject(m_xlsSheets)
                MyClass.MRComObject(m_xlsBook)
                '''' オープン失敗
                Return False
            End Try
        End Function
        ''' <param name="strFPath">Excelファイルフルパス</param>
        ''' <param name="strSheetName">Excelシート名</param>
        ''' <returns>復帰情報</returns>
        Public Overloads Function Open(ByVal strFPath As String, ByVal strSheetName As String) As Boolean
            '''' オープン
            If Not Open(strFPath) Then Return False

            '''' セット対象シート変更
            MyClass.SheetChenge(strSheetName)
            Return True
        End Function
        ''' <param name="strFPath">Excelファイルフルパス</param>
        ''' <param name="sheetIndex">シート番号 (1ｵﾘｼﾞﾝ)</param>
        ''' <returns>復帰情報</returns>
        Public Overloads Function Open(ByVal strFPath As String, ByVal sheetIndex As Integer) As Boolean
            '''' オープン
            If Not Open(strFPath) Then Return False

            '''' セット対象シート変更
            MyClass.SheetChenge(sheetIndex)
            Return True
        End Function
        ''' <summary>
        ''' 新しいブックを追加
        ''' </summary>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function OpenAdd() As Boolean
            If IsNothing(m_xlsApp) Then Return False
            Try
                '''' Excelファイルオープン
                MyClass.XlsBook = m_xlsBooks.Add()
                Return True
            Catch ex As Exception
                MyClass.MRComObject(m_xlsSheets)
                MyClass.MRComObject(m_xlsBook)
                '''' オープン失敗
                Return False
            End Try
        End Function
        ''' <param name="strSheetName">ブックを追加した際、指定したシートをアクティグシートとする</param>
        ''' <returns></returns>
        Public Overloads Function OpenAdd(ByVal strSheetName As String) As Boolean
            '''' オープン
            If Not OpenAdd() Then Return False

            '''' セット対象シート変更
            MyClass.SheetChenge(strSheetName)
            Return True
        End Function
        ''' <param name="sheetindex">ブックを追加した際、指定したシートをアクティグシートとする</param>
        ''' <returns></returns>
        Public Overloads Function OpenAdd(ByVal sheetIndex As Integer) As Boolean
            '''' オープン
            If Not OpenAdd() Then Return False

            '''' セット対象シート変更
            MyClass.SheetChenge(sheetIndex)
            Return True
        End Function

        ''' <summary>
        ''' 指定ブックの終了
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Close()
            '''' 現在アクティブなブックを終了
            m_xlsBook.Close()
            MyClass.MRComObject(m_xlsBook)
        End Sub
        ''' <param name="bookIndex">ブック番号 (1オリジン)</param>
        ''' <remarks></remarks>
        Public Sub Close(ByVal bookIndex As Integer)
            Dim book As Excel.Workbook = MyClass.XlsBook(bookIndex)
            book.Close()
            MyClass.MRComObject(book)
        End Sub
        ''' <param name="bookName">ブック名称</param>
        ''' <remarks></remarks>
        Public Sub Close(ByVal bookName As String)
            Dim book As Excel.Workbook = MyClass.XlsBook(bookName)
            book.Close()
            MyClass.MRComObject(book)
        End Sub

        ''' <summary>
        ''' Excelクローズ
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub CloseAll()
            On Error GoTo ErrAri

            '''' COMの開放 詳細は(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
            '''' シート 開放
            MyClass.MRComObject(m_xlsSheet)
            MyClass.MRComObject(m_xlsSheets)

            '''' 全ブッククローズ
            For ii As Integer = 1 To m_xlsBooks.Count
                MyClass.Close(ii)
            Next ii

            '''' ブック 開放
            MyClass.MRComObject(m_xlsBooks)

            '''' 実行速度向上
            m_xlsApp.ScreenUpdating = True		 '画面更新禁止
            m_xlsApp.Interactive = True			  '非対話モード
            m_xlsApp.DisplayAlerts = True		  'ダイヤログ出力禁止

            '''' Excelを終了
            m_xlsApp.Quit()

            '''' Excel アプリケーション 開放
            MyClass.MRComObject(m_xlsApp)

            '''' ここでもダメな場合がある
            '''' CreateObjectで確保したオブジェクトが、CreateObjectを発行した*スレッド*によって参照されているため、ガベージコレクタがガベージではないと判断しているため
            '''' スレッド処理クラスの中でNothingにするだけでなく、スレッド処理クラス自身をNothingしなければ成らない
            '''' つまり、呼び元で Newしたインスタンスを Nothing した後 GC.Collect() をすればOKのはず

            ''''''' 上の処理だけでは VB.NETでは開放されない
            ''''''' なんでって ガベージコレクションがまだ収集していない為です。
            ''''''' って事で Excelの全てのメモリの参照を開放してから ガベージコレクションを明示的に実行させちゃいます。
            '''''''  もしこれでもダメな時は、GC.Collect(2) にしてみてください 85,000byte以上のオブジェクトは常にジェネレーション2に置かれているからです

            '''' 通常 COMの開放 を厳密に行っておけば GC を呼ばなくても良いはず（念の為にコール）
            GC.Collect()
            Return
ErrAri:
        End Sub

        ''' <summary>
        ''' Excelの保存
        ''' </summary>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Save() As Boolean
            On Error GoTo ErrAri
            '''' オブジェクトチェック
            If m_xlsBook Is Nothing Then Return False
            '''' 読込み専用の場合も異常とする
            If m_xlsBook.ReadOnly Then Return False

            '''' Excelを上書きする
            m_xlsBook.Save()

            Return True
ErrAri:
            Return False
        End Function

        ''' <summary>
        ''' Excelの名前を付けて保存
        ''' </summary>
        ''' <param name="strFPath">Excelファイルパス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function Save(ByVal strFPath As String) As Boolean
            On Error GoTo ErrAri
            '''' オブジェクトチェック
            If m_xlsBook Is Nothing Then Return False
            If strFPath = "" Then Return False
            '''' Excelを名前を付けて保存
            m_xlsBook.SaveAs(Filename:=strFPath)
            Return True
ErrAri:
            Return False
        End Function

        ''' <summary>
        ''' 対象ブックの変更（この後は必ずシートの変更も行う）
        ''' </summary>
        ''' <param name="bookIndex">ブックのインデックス (1ｵﾘｼﾞﾝ)</param>
        ''' <remarks></remarks>
        Public Overloads Sub BookChenge(ByVal bookIndex As Integer)
            Try
                '''' セット対象シート変更
                MyClass.XlsBook = MyClass.XlsBook(bookIndex)
                '''' 関連オブジェクトにセット
                m_xlsSheets = MyClass.XlsBook.Worksheets
            Catch ex As Exception
            End Try
        End Sub
        ''' <param name="bookName">ブック名称</param>
        Public Overloads Sub BookChenge(ByVal bookName As String)
            Try
                '''' セット対象シート変更
                MyClass.XlsBook = MyClass.XlsBook(bookName)
                '''' 関連オブジェクトにセット
                m_xlsSheets = MyClass.XlsBook.Worksheets
            Catch ex As Exception
            End Try
        End Sub
        ''' <summary>
        ''' 対象シートの変更
        ''' </summary>
        ''' <param name="sheetIndex">シートのインデックス (1ｵﾘｼﾞﾝ)</param>
        ''' <remarks></remarks>
        Public Overloads Sub SheetChenge(ByVal sheetIndex As Integer)
            '''' セット対象シート変更
            MyClass.XlsSheet = MyClass.XlsSheet(sheetIndex)
        End Sub
        ''' <param name="sheetName">シート名称</param>
        Public Overloads Sub SheetChenge(ByVal sheetName As String)
            '''' セット対象シート変更
            MyClass.XlsSheet = MyClass.XlsSheet(sheetName)
        End Sub

        ''' <summary>
        ''' マクロの実行
        ''' </summary>
        ''' <param name="strMacroName">マクロ名</param>
        ''' <remarks>シート指定時、"Sheet5.Test"。 "Test"だと標準モジュールの関数名が実行される</remarks>
        Public Overloads Function MacroRun(ByVal strMacroName As String) As Boolean
            Try
                m_xlsApp.Run(strMacroName)
            Catch ex As Exception
                Return False
            End Try
            Return True
        End Function
        Public Overloads Function MacroRun(ByVal strMacroName As String, ByVal o1 As Object) As Boolean
            Try
                m_xlsApp.Run(strMacroName, o1)
            Catch ex As Exception
                Return False
            End Try
            Return True
        End Function
        Public Overloads Function MacroRun(ByVal strMacroName As String, ByVal o1 As Object, ByVal o2 As Object) As Boolean
            Try
                m_xlsApp.Run(strMacroName, o1, o2)
            Catch ex As Exception
                Return False
            End Try
            Return True
        End Function
        Public Overloads Function MacroRun(ByVal strMacroName As String, ByVal o1 As Object, ByVal o2 As Object, ByVal o3 As Object) As Boolean
            Try
                m_xlsApp.Run(strMacroName, o1, o2, o3)
            Catch ex As Exception
                Return False
            End Try
            Return True
        End Function



        ''' <summary>
        ''' 指定した列の列名を取得する 1 と指定すると "A"が返る
        ''' </summary>
        ''' <param name="col1">指定列</param>
        ''' <returns>アドレス"A"</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetColumnName(ByVal col1 As Integer) As String
            Dim strwk() As String
            'Ver.02
            'strwk = CStr(CType(m_xlsSheet.Columns(1), Excel.Range).Address(ColumnAbsolute:=False)).Split(":"c)
            strwk = CStr(CType(m_xlsSheet.Columns(col1), Excel.Range).Address(ColumnAbsolute:=False)).Split(":"c)
            Return strwk(0)
        End Function
        ''' <param name="col1">開始列</param>
        ''' <param name="col2">終了列</param>
        ''' <returns>アドレス"A:B"</returns>
        Public Overloads Function GetColumnName(ByVal col1 As Integer, ByVal col2 As Integer) As String
            Return MyClass.GetColumnName(col1) & ":" & MyClass.GetColumnName(col2)
        End Function

        ''' <summary>
        ''' 指定シートの指定列の最終行を求める
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="SerchCol">検索列</param>
        ''' <param name="CheckRow">チェック開始行 (この値より小さくなる事は無い)</param>
        ''' <returns>行 1ｵﾘｼﾞﾝ</returns>
        ''' <remarks>非表示行列には未対応</remarks>
        Public Function GetEndRow(ByVal sheet As Excel.Worksheet, ByVal SerchCol As Integer, ByVal CheckRow As Integer) As Integer
            Dim iVal As Integer
            iVal = CType(sheet.Cells(65536, SerchCol), Excel.Range).End(Excel.XlDirection.xlUp).Row
            Return CInt(IIf(iVal < CheckRow, CheckRow, iVal))
        End Function

        ''' <summary>
        ''' 指定シートの指定行の最終列を求める
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="SerchRow">検索行</param>
        ''' <param name="CheckCol">チェック開始列 (この値より小さくなる事は無い)</param>
        ''' <returns>列 1ｵﾘｼﾞﾝ</returns>
        ''' <remarks>非表示行列には未対応</remarks>
        Public Function GetEndCol(ByVal sheet As Excel.Worksheet, ByVal SerchRow As Integer, ByVal CheckCol As Integer) As Integer
            Dim iVal As Integer
            iVal = CType(sheet.Cells(SerchRow, 256), Excel.Range).End(Excel.XlDirection.xlToLeft).Column
            Return CInt(IIf(iVal < CheckCol, CheckCol, iVal))
        End Function

        ''' <summary>
        ''' Pixel単位からポイント単位に変換する
        ''' </summary>
        ''' <param name="nVal">Pixel単位</param>
        ''' <returns>ポイント単位</returns>
        ''' <remarks></remarks>
        Public Function GetPixelToPoint(ByVal nVal As Integer) As Single
            Return CSng(nVal * 3 / 4)
        End Function
        ''' <summary>
        ''' ポイント単位からPixel単位に変換する
        ''' </summary>
        ''' <param name="nVal">ポイント単位</param>
        ''' <returns>Pixel単位</returns>
        ''' <remarks></remarks>
        Public Function GetPointToPixel(ByVal nVal As Integer) As Single
            Return CSng(nVal * 4 / 3)
        End Function

        '********************************************************************
        ' ちょっと
        '****************************s****************************************

        ''' <summary>
        ''' 指定シートのコントロールを取得する
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="strName">コントロール名称</param>
        ''' <returns>コントロール</returns>
        ''' <remarks></remarks>
        Public Function GetControl(ByVal sheet As Excel.Worksheet, ByVal strName As String) As Excel.Shape
            For ii As Integer = 1 To sheet.Shapes.Count
                If sheet.Shapes.Item(ii).Name = strName Then
                    Return sheet.Shapes.Item(ii)
                End If
            Next ii
            Return Nothing
        End Function

        ''' <summary>
        ''' 指定シートのコントロールを取得する
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="strName">コントロール名称</param>
        ''' <returns>コントロール</returns>
        ''' <remarks></remarks>
        Public Function GetOLEControl(ByVal sheet As Excel.Worksheet, ByVal strName As String) As Object
            Try
                'Return sheet.OLEObjects.Item(strName)
                Return sheet.OLEObjects(strName)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' 図形追加
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="strPath">図形パス</param>
        ''' <param name="left">左端からの位置[pixel]</param>
        ''' <param name="top">上端からの位置[pixel]</param>
        ''' <param name="width">幅[pixel]</param>
        ''' <param name="height">高さ[pixel]</param>
        ''' <remarks></remarks>
        Public Overloads Sub AddPictuer(ByVal sheet As Excel.Worksheet, ByVal strPath As String, ByVal left As Integer, ByVal top As Integer, ByVal width As Integer, ByVal height As Integer)
#If tExcelVer2000 Then
            '' Microsoft Excel 9.0 Object Library 用
            sheet.Shapes.AddPicture(strPath, Office.MsoTriState.msoFalse, Office.MsoTriState.msoTrue, GetPixelToPoint(left), GetPixelToPoint(top), GetPixelToPoint(width), GetPixelToPoint(height))
#ElseIf tExcelVer2003 Then
            '' Microsoft Excel 11.0 Object Library 用
            sheet.Shapes.AddPicture(strPath, Microsoft.Office.Core.MsoTriState.msoFalse, Microsoft.Office.Core.MsoTriState.msoTrue, GetPixelToPoint(left), GetPixelToPoint(top), GetPixelToPoint(width), GetPixelToPoint(height))
#End If
        End Sub
        ' 現在のシート
        Public Overloads Sub AddPictuer(ByVal strPath As String, ByVal left As Integer, ByVal top As Integer, ByVal width As Integer, ByVal height As Integer)
            Call AddPictuer(m_xlsSheet, strPath, left, top, width, height)
        End Sub

        ''' <summary>
        ''' 図形追加
        ''' </summary>
        ''' <param name="sheet">どのシート</param>
        ''' <param name="row">行位置</param>
        ''' <param name="col">列位置</param>
        ''' <param name="img">画像イメージ</param>
        Public Overloads Sub AddPictuer(ByVal sheet As Excel.Worksheet, ByVal row As Integer, ByVal col As Integer, ByVal img As Image)
            ' 画像データをクリップボードへ
            My.Computer.Clipboard.SetImage(img)
            ' 書き込み位置を選択
            CType(sheet.Cells(row, col), Excel.Range).Select()
            ' 書き込み
            sheet.PasteSpecial(Format:="ビットマップ", Link:=False, DisplayAsIcon:=False)
        End Sub
        ' 現在のシート
        Public Overloads Sub AddPictuer(ByVal row As Integer, ByVal col As Integer, ByVal img As Image)
            Call AddPictuer(m_xlsSheet, row, col, img)
        End Sub


        ''' <summary>
        ''' シートコピー
        ''' </summary>
        ''' <param name="sheetIndex">コピー元シートインデックス</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function CopySheet(ByVal sheetIndex As Integer, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet

            Try

                ''''コピー元のシートをセット
                xlsFromSheet = CType(m_xlsSheets.Item(sheetIndex), Excel.Worksheet)

                If bPos Then
                    ''''コピー元の前にコピー
                    xlsFromSheet.Copy(xlsFromSheet)
                Else

                    ''''コピー元の後ろにコピー
                    xlsFromSheet.Copy(, xlsFromSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try


        End Function

        ''' <param name="sheetName">コピー元シート名</param>
        Public Overloads Function CopySheet(ByVal sheetName As String, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet

            Try

                ''''コピー元のシートをセット
                xlsFromSheet = CType(m_xlsSheets.Item(sheetName), Excel.Worksheet)

                If bPos Then
                    ''''コピー元の前にコピー
                    xlsFromSheet.Copy(xlsFromSheet)
                Else
                    ''''コピー元の後ろにコピー
                    xlsFromSheet.Copy(, xlsFromSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <summary>
        ''' シートコピー
        ''' </summary>
        ''' <param name="sheetIndex">コピー元シートインデックス</param>
        ''' <param name="sheetToIndex">基準シートインデックス</param>
        ''' <param name="bPos">コピー位置(True:任意のシートの前,False:コピー元の後ろ</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function CopySheet(ByVal sheetIndex As Integer, ByVal sheetToIndex As Integer, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet
            Dim xlsToSheet As Excel.Worksheet


            Try

                ''''コピー元のシート,基準シートをセット
                xlsFromSheet = CType(m_xlsSheets.Item(sheetIndex), Excel.Worksheet)
                xlsToSheet = CType(m_xlsSheets.Item(sheetToIndex), Excel.Worksheet)

                If bPos Then
                    ''''基準シートの前にコピー
                    xlsFromSheet.Copy(xlsToSheet)
                Else

                    ''''基準シートの後ろにコピー
                    xlsFromSheet.Copy(, xlsToSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <param name="sheetName">コピー元シート名</param>
        ''' <param name="sheetToName">基準シート名</param>
        Public Overloads Function CopySheet(ByVal sheetName As String, ByVal sheetToName As String, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet
            Dim xlsToSheet As Excel.Worksheet

            Try

                ''''コピー元のシート,基準シートをセット
                xlsFromSheet = CType(m_xlsSheets.Item(sheetName), Excel.Worksheet)
                xlsToSheet = CType(m_xlsSheets.Item(sheetToName), Excel.Worksheet)

                If bPos Then
                    ''''基準シートの前にコピー
                    xlsFromSheet.Copy(xlsToSheet)
                Else

                    ''''基準シートの後ろにコピー
                    xlsFromSheet.Copy(, xlsToSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <summary>
        ''' シートの有無チェック
        ''' </summary>
        ''' <param name="sheetIndex">シートインデックス</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function CheckSheet(ByVal sheetIndex As Integer) As Boolean

            For Each xlsSheet As Excel.Worksheet In m_xlsSheets

                '一致するシートをチェック
                If xlsSheet.Index = sheetIndex Then
                    Return True
                End If

            Next xlsSheet

            Return False

        End Function
        ''' <param name="sheetname">シート名称</param>
        Public Overloads Function CheckSheet(ByVal sheetName As String) As Boolean

            For Each xlsSheet As Excel.Worksheet In m_xlsSheets

                '一致するシートをチェック
                If xlsSheet.Name = sheetName Then
                    Return True
                End If

            Next xlsSheet

            Return False

        End Function

        ''' <summary>
        ''' シート名称変更
        ''' </summary>
        ''' <param name="sheetName">シート名称変更</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Function ChangeSheetName(ByVal sheetName As String) As Boolean

            Try
                'シート名セット
                m_xlsSheet.Name = sheetName
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function


        ''' <summary>
        ''' 印刷 (指定シート全部印刷)
        ''' </summary>
        ''' <param name="sheetIndex">シートインデックス</param>
        ''' <param name="printerName">プリンタ名</param>
        ''' <param name="printCout">プリント枚数</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function PrintOut(ByVal sheetIndex As Integer, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                'シートチェック
                If Not CheckSheet(sheetIndex) Then Return False

                '''' 該当シートをアクティブにする
                SheetChenge(sheetIndex)

                If "" <> printerName Then
                    'プリンター名を指定して印刷
                    m_xlsSheet.PrintOut(, , printCout, , printerName, , True)
                Else
                    m_xlsSheet.PrintOut(, , printCout, , , , True)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <param name="sheetName">シート名</param>
        Public Overloads Function PrintOut(ByVal sheetName As String, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                'シートチェック
                If Not CheckSheet(sheetName) Then Return False

                '''' 該当シートをアクティブにする
                SheetChenge(sheetName)

                If "" <> printerName Then
                    'プリンター名を指定して印刷
                    m_xlsSheet.PrintOut(, , printCout, , printerName, , True)
                Else
                    m_xlsSheet.PrintOut(, , printCout, , , , True)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' 印刷 (印刷ページ範囲指定) Ver.1.2.1.0
        ''' </summary>
        ''' <param name="sheetIndex">シートインデックス</param>
        ''' <param name="PageFrom">印刷開始ページNo</param>
        ''' <param name="PageTo">印刷終了ページNo</param>
        ''' <param name="printerName">プリンタ名</param>
        ''' <param name="printCout">プリント枚数</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Overloads Function PrintOut(ByVal sheetIndex As Integer, ByVal PageFrom As Integer, ByVal PageTo As Integer, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                'シートチェック
                If Not CheckSheet(sheetIndex) Then Return False

                '''' 該当シートをアクティブにする
                SheetChenge(sheetIndex)

                If "" <> printerName Then
                    'プリンター名を指定して印刷
                    m_xlsSheet.PrintOut(PageFrom, PageTo, printCout, , printerName, , True)
                Else
                    m_xlsSheet.PrintOut(PageFrom, PageTo, printCout, , , , True)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function
        Public Overloads Function PrintOut(ByVal sheetName As String, ByVal PageFrom As Integer, ByVal PageTo As Integer, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                'シートチェック
                If Not CheckSheet(sheetName) Then Return False

                '''' 該当シートをアクティブにする
                SheetChenge(sheetName)

                If "" <> printerName Then
                    'プリンター名を指定して印刷
                    m_xlsSheet.PrintOut(PageFrom, PageTo, printCout, , printerName, , True)
                Else
                    m_xlsSheet.PrintOut(PageFrom, PageTo, printCout, , , , True)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

    End Class
End Namespace


#End If	'// <<< ----------------------------------------------