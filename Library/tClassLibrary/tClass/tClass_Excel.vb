'*********************************************************************************
'Excel操作の継承クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'		Ver.04    2008/01/08  実行環境に対応（ほぼ全部作り直し）
'
'	[メモ]
'		詳細は、tClass_Excel_Baseに記述
'*********************************************************************************

Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)



'********************************************************************
'切り替え
'********************************************************************
'' 本クラスを未使用時はスイッチしてください ←たぶん Ver.04からは、未使用でOKのはず。 (ローカルのDLLを直で参照している為)
#Const tExcelON = True						' 当クラスを使用しない場合は、False

'' PCでスイッチしてください ←たぶん Ver.04からは、2003固定でOKのはず。（参照しているDLLを2003固定にした為）
'#Const tExcelVer2000 = 2000                     ' Excel 2000 用
#Const tExcelVer2003 = 2003						' Excel 2003 用



'********************************************************************
'実際の処理
'********************************************************************
#If tExcelON Then			'// ---------------------------------------------->>>>>

' Imports Excel					' ライブラリは全部明示的に書いておきたい為、ヤメ
' Imports Microsoft.Office		' ライブラリは全部明示的に書いておきたい為、ヤメ
Imports System.Drawing


Namespace tClass
    ''' <summary>
    ''' Excel操作の継承クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Excel
        Inherits tClass_Excel_Base

        '********************************************************************
        'メソッド定義 (基本)
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            MyBase.New()
        End Sub

        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Overrides Sub Dispose()
            MyBase.Dispose()
        End Sub

        ''' <summary>
        ''' Finalizeデストラクタ
        ''' </summary>
        ''' <remarks>Disposeを行っていない場合のみ</remarks>
        Protected Overrides Sub Finalize()
            MyBase.Finalize()
        End Sub

        '********************************************************************
        'メソッド定義 (拡張)
        '********************************************************************
        ''' <summary>
        ''' セルの罫線 を描画 罫線プロパティに該当
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="LineStyle">線種 [Nothing:セット無し]</param>
        ''' <param name="Weight">幅 [Nothing:セット無し]</param>
        ''' <param name="color">色 [Nothing:セット無し]</param>
        ''' <param name="borderIndex">描画位置 Nothing:全体</param>
        ''' <remarks></remarks>
        Public Overloads Sub LineBorder(ByVal row1 As Integer, ByVal col1 As Integer, ByVal LineStyle As Excel.XlLineStyle, ByVal Weight As Excel.XlBorderWeight, ByVal color As Color, Optional ByVal borderIndex As Excel.XlBordersIndex = Nothing)
            If IsNothing(borderIndex) OrElse borderIndex = 0 Then
                Dim borders As Excel.Borders = MyBase.Cells(row1, col1).Borders()
                If Not IsNothing(LineStyle) Then borders.LineStyle = LineStyle
                If Not IsNothing(Weight) Then borders.Weight = Weight
                If Not IsNothing(color) Then borders.Color = ColorToRGB(color)
                'MRComObject(borders)
            Else
                Dim border As Excel.Border = MyBase.Cells(row1, col1).Borders(borderIndex)
                If Not IsNothing(LineStyle) Then border.LineStyle = LineStyle
                If Not IsNothing(Weight) Then border.Weight = Weight
                If Not IsNothing(color) Then border.Color = ColorToRGB(color)
                'MRComObject(border)
            End If
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>        
        ''' <param name="LineStyle">線種 [Nothing:セット無し]</param>
        ''' <param name="Weight">幅 [Nothing:セット無し]</param>
        ''' <param name="color">色 [Nothing:セット無し]</param>
        ''' <param name="borderIndex">描画位置 Nothing:全体</param>
        Public Overloads Sub LineBorder(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal LineStyle As Excel.XlLineStyle, ByVal Weight As Excel.XlBorderWeight, ByVal color As Color, Optional ByVal borderIndex As Excel.XlBordersIndex = Nothing)
            If IsNothing(borderIndex) OrElse borderIndex = 0 Then
                Dim borders As Excel.Borders = MyBase.Cells(row1, col1, row2, col2).Borders()
                If Not IsNothing(LineStyle) Then borders.LineStyle = LineStyle
                If Not IsNothing(Weight) And Weight <> 0 Then borders.Weight = Weight
                If Not IsNothing(color) Then borders.Color = ColorToRGB(color)
                'MRComObject(borders)
            Else
                Dim border As Excel.Border = MyBase.Cells(row1, col1, row2, col2).Borders(borderIndex)
                If Not IsNothing(LineStyle) Then border.LineStyle = LineStyle
                If Not IsNothing(Weight) And Weight <> 0 Then border.Weight = Weight
                If Not IsNothing(color) Then border.Color = ColorToRGB(color)
                'MRComObject(border)
            End If
        End Sub

        '----------------------------------------------------------------
        ' 概要：セル サイズ フォントプロパティに該当
        ' 引数：(in :Integer) row1 = 行 (1ｵﾘｼﾞﾝ)
        ' 引数：(in :Integer) col1 = 列 (1ｵﾘｼﾞﾝ)
        ' 引数：(in :Integer) size = フォントサイズ
        ' 引数：(in :Color) color = 文字色
        ' 戻値：() 
        '----------------------------------------------------------------
        ''' <summary>
        ''' セル サイズ フォントプロパティに該当
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="size">フォントサイズ</param>
        ''' <remarks></remarks>
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal size As Integer)
            MyBase.Cells(row1, col1).Font.Size = size
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="size">フォントサイズ</param>
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal size As Integer)
            MyBase.Cells(row1, col1, row2, col2).Font.Size = size
        End Sub
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="size">フォントサイズ</param>
        ''' <param name="color">フォント色</param>            
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal size As Integer, ByVal color As Color)
            Dim f As Excel.Font = MyBase.Cells(row1, col1).Font
            f.Size = size
            f.Color = ColorToRGB(color)
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="size">フォントサイズ</param>
        ''' <param name="color">フォント色</param>        
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal size As Integer, ByVal color As Color)
            Dim f As Excel.Font = MyBase.Cells(row1, col1, row2, col2).Font
            f.Size = size
            f.Color = ColorToRGB(color)
        End Sub

        ''' <summary>
        ''' セル 塗り潰し パターンプロパティに該当
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="color">セル色</param>
        ''' <param name="pattern">セル塗り潰しパターン</param>
        ''' <remarks></remarks>
        Public Overloads Sub Interior(ByVal row1 As Integer, ByVal col1 As Integer, ByVal color As Color, Optional ByVal pattern As Excel.XlPattern = Excel.XlPattern.xlPatternSolid)
            Dim inter As Excel.Interior = MyBase.Cells(row1, col1).Interior		'塗り潰しオブジェクト
            inter.Color = ColorToRGB(color)
            inter.Pattern = pattern
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="color">セル色</param>
        ''' <param name="pattern">セル塗り潰しパターン</param>
        Public Overloads Sub Interior(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal color As Color, Optional ByVal pattern As Excel.XlPattern = Excel.XlPattern.xlPatternSolid)
            Dim inter As Excel.Interior = MyBase.Cells(row1, col1, row2, col2).Interior	   '塗り潰しオブジェクト
            inter.Color = ColorToRGB(color)
            inter.Pattern = pattern
        End Sub

        ''' <summary>
        ''' セル 配置プロパティに該当
        ''' </summary>
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="blnON">連結状態 True:連結</param>
        ''' <param name="x">横配置</param>
        ''' <param name="y">縦配置</param>
        ''' <remarks></remarks>
        Public Overloads Sub Merge(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal blnON As Boolean, Optional ByVal x As Excel.XlHAlign = Excel.XlHAlign.xlHAlignCenter, Optional ByVal y As Excel.XlVAlign = Excel.XlVAlign.xlVAlignBottom)
            Dim range As Excel.Range = MyBase.Cells(row1, col1, row2, col2)
            '' 絶対条件
            range.WrapText = False		'自動折り返し
            range.ShrinkToFit = False	'自動縮小
            range.AddIndent = False		'インデント
            range.Orientation = 0		'角度

            '' 任意条件
            range.MergeCells = blnON		'連結
            range.HorizontalAlignment = x	'横配置
            range.VerticalAlignment = y		'縦配置
        End Sub

        '----------------------------------------------------------------
        ' 概要：セル フォマット 表示形式プロパティに該当
        ' 引数：(in :Integer) row1 = 行 (1ｵﾘｼﾞﾝ)
        ' 引数：(in :Integer) col1 = 列 (1ｵﾘｼﾞﾝ)
        ' 引数：(in :Integer) type = フォーマット型 (数値:0 文字列:1 日付:2 フォーマット指定:9)
        ' 引数：(in :String) strFormat = フォーマット(フォーマット書式)
        ' 戻値：() 
        '----------------------------------------------------------------
        ''' <summary>
        ''' セル フォマット 表示形式プロパティに該当
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="type">フォーマット型 (数値:0 文字列:1 日付:2 フォーマット指定:9)</param>
        ''' <param name="strFormat">フォーマット(フォーマット書式)</param>
        ''' <remarks></remarks>
        Public Overloads Sub NumberFormat(ByVal row1 As Integer, ByVal col1 As Integer, ByVal type As Integer, Optional ByVal strFormat As String = "")
            Dim strType As String = ""
            Select Case type
                Case 0
                    strType = "0_ "
                Case 1
                    strType = "@"
                Case 2
                    strType = "yyyy/MM/dd hh:mm:ss"
                Case 9
                    strType = strFormat
                Case Else
                    Return
            End Select
            If strFormat <> "" Then
                strType = strFormat
            End If
            MyBase.Cells(row1, col1).NumberFormat = strType
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="type">フォーマット型 (数値:0 文字列:1 日付:2 フォーマット指定:9)</param>
        ''' <param name="strFormat">フォーマット(フォーマット書式)</param>
        Public Overloads Sub NumberFormat(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal type As Integer, Optional ByVal strFormat As String = "")
            Dim strType As String = ""
            Select Case type
                Case 0
                    strType = "0_ "
                Case 1
                    strType = "@"
                Case 2
                    strType = "yyyy/MM/dd hh:mm:ss"
                Case 9
                    strType = strFormat
                Case Else
                    Return
            End Select
            If strFormat <> "" Then
                strType = strFormat
            End If
            MyBase.Cells(row1, col1, row2, col2).NumberFormat = strType
        End Sub

        ''' <summary>
        ''' 行幅 指定
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="size">高さサイズ</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowHeight(ByVal row1 As Integer, ByVal size As Integer)
            MyClass.RowHeight(row1, row1, size)
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="size">高さサイズ</param>
        Public Overloads Sub RowHeight(ByVal row1 As Integer, ByVal row2 As Integer, ByVal size As Integer)
            Dim strVal As String = row1 & ":" & row2
            CType(MyBase.m_xlsSheet.Rows(strVal), Excel.Range).RowHeight = size
            'MyBase.m_xlsSheet.Rows(strVal).RowHeight = size
        End Sub

        ''' <summary>
        ''' 列幅 指定
        ''' </summary>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="size">幅サイズ</param>
        ''' <remarks></remarks>
        Public Overloads Sub ColumnWidth(ByVal col1 As Integer, ByVal size As Integer)
            ColumnWidth(col1, col1, size)
        End Sub
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>          
        ''' <param name="size">幅サイズ</param>
        Public Overloads Sub ColumnWidth(ByVal col1 As Integer, ByVal col2 As Integer, ByVal size As Integer)
            Dim strVal As String = MyBase.GetColumnName(col1, col2)
            CType(MyBase.m_xlsSheet.Columns(strVal), Excel.Range).ColumnWidth = size
            'MyBase.m_xlsSheet.Columns(strVal).ColumnWidth = size
        End Sub

        ''' <summary>
        ''' 行 挿入
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowInsert(ByVal row1 As Integer)
            CType(MyBase.m_xlsSheet.Rows(row1), Excel.Range).Insert(Shift:=Excel.XlDirection.xlDown)
            'MyBase.m_xlsSheet.Rows(row1).Insert(Shift:=Excel.XlDirection.xlDown)
        End Sub
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>               
        Public Overloads Sub RowInsert(ByVal row1 As Integer, ByVal col1 As Integer, ByVal col2 As Integer)
            MyBase.m_xlsSheet.Range(MyBase.m_xlsSheet.Cells(row1, col1), MyBase.m_xlsSheet.Cells(row1, col2)).Insert(Shift:=Excel.XlDirection.xlDown)
        End Sub


        ''' <summary>
        ''' 行 コピー＆ペースト
        ''' </summary>
        ''' <param name="CopyRow">コピー元行</param>
        ''' <param name="CopyCnt">コピー件数</param>
        ''' <param name="PasteRow">ペースト先行</param>
        ''' <param name="PasteCnt">ペースト件数</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowCopyAndPaste(ByVal CopyRow As Integer, ByVal CopyCnt As Integer, ByVal PasteRow As Integer, ByVal PasteCnt As Integer)
            Rows(CopyRow, CopyCnt).Copy()
            Rows(PasteRow, PasteCnt).PasteSpecial(Excel.XlPasteType.xlPasteAll)
        End Sub
        ''' <summary>
        ''' 行 コピー＆挿入
        ''' </summary>
        ''' <param name="CopyRow">コピー元行</param>
        ''' <param name="CopyCnt">コピー件数</param>
        ''' <param name="PasteRow">ペースト先行</param>
        ''' <param name="PasteCnt">ペースト件数</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowCopyAndInsert(ByVal CopyRow As Integer, ByVal CopyCnt As Integer, ByVal PasteRow As Integer, ByVal PasteCnt As Integer)
            Rows(CopyRow, CopyCnt).Copy()	' コピー
            Rows(PasteRow, PasteCnt).Insert(Shift:=Excel.XlDirection.xlDown) '選択 & 挿入
        End Sub
        ''' <summary>
        ''' 列 コピー＆挿入
        ''' </summary>
        ''' <param name="CopyColumn">コピー元列</param>
        ''' <param name="CopyCnt">コピー件数</param>
        ''' <param name="PasteColumn">ペースト先列</param>
        ''' <param name="PasteCnt">ペースト件数</param>
        ''' <remarks></remarks>
        Public Overloads Sub ColumnCopyAndInsert(ByVal CopyColumn As Integer, ByVal CopyCnt As Integer, ByVal PasteColumn As Integer, ByVal PasteCnt As Integer)
            Columns(CopyColumn, CopyCnt).Copy()	' コピー
            Columns(PasteColumn, PasteCnt).Insert(Shift:=Excel.XlDirection.xlToRight)  '選択 & 挿入
        End Sub



        ''' <summary>
        ''' 指定範囲 コピー＆挿入
        ''' </summary>
        ''' <param name="Frow1">コピー元 開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Fcol1">コピー元 開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Frow2">コピー元 終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Fcol2">コピー元 終了列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Trow1">コピー先 開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Tcol1">コピー先 開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Trow2">コピー先 終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="Tcol2">コピー先 終了列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="nMode">モード(0:右へシフト  1:下へシフト )</param>
        ''' <remarks></remarks>
        Public Overloads Sub CellCopyAndInsert(ByVal Frow1 As Integer, ByVal Fcol1 As Integer, ByVal Frow2 As Integer, ByVal Fcol2 As Integer, _
           ByVal Trow1 As Integer, ByVal Tcol1 As Integer, ByVal Trow2 As Integer, ByVal Tcol2 As Integer, _
           ByVal nMode As Integer)

            ' コピー
            Dim Frange As Excel.Range = MyBase.Cells(Frow1, Fcol1, Frow2, Fcol2)
            Frange.Copy()

            ' 挿入
            Dim Trange As Excel.Range = MyBase.Cells(Trow1, Tcol1, Trow2, Tcol2)

            If 0 = nMode Then
                Trange.Insert(Shift:=Excel.XlDirection.xlToRight)
            ElseIf 1 = nMode Then
                Trange.Insert(Shift:=Excel.XlDirection.xlDown)
            End If
        End Sub





        ''' <summary>
        ''' セル 位置
        ''' </summary>
        ''' <param name="row1">行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="align">left:0 right:1 center:2</param>
        ''' <remarks></remarks>
        Public Overloads Sub CellAlign(ByVal row1 As Integer, ByVal col1 As Integer, ByVal align As Integer)
            Dim wk As Excel.XlHAlign
            Select Case align
                Case 0
                    wk = Excel.XlHAlign.xlHAlignLeft
                Case 1
                    wk = Excel.XlHAlign.xlHAlignRight
                Case 2
                    wk = Excel.XlHAlign.xlHAlignCenter
                Case Else
                    Return
            End Select
            MyBase.Cells(row1, col1).HorizontalAlignment = wk
        End Sub
        ''' <param name="row1">開始行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col1">開始列 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="row2">終了行 (1ｵﾘｼﾞﾝ)</param>
        ''' <param name="col2">終了列 (1ｵﾘｼﾞﾝ)</param>  
        ''' <param name="align">left:0 right:1 center:2</param>
        Public Overloads Sub CellAlign(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal align As Integer)
            Dim wk As Excel.XlHAlign = Excel.XlHAlign.xlHAlignCenter
            Select Case align
                Case 0
                    wk = Excel.XlHAlign.xlHAlignLeft
                Case 1
                    wk = Excel.XlHAlign.xlHAlignRight
                Case 2
                    wk = Excel.XlHAlign.xlHAlignCenter
                Case Else
                    Return
            End Select
            MyBase.Cells(row1, col1, row2, col2).HorizontalAlignment = wk
        End Sub
    End Class
End Namespace

#End If	'// <<< ----------------------------------------------