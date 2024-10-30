'*********************************************************************************
'Excel����̌p���N���X
'	[Ver]
'		Ver.01    2006/03/04  vs2005 �Ή�
'		Ver.04    2008/01/08  ���s���ɑΉ��i�قڑS����蒼���j
'
'	[����]
'		�ڍׂ́AtClass_Excel_Base�ɋL�q
'*********************************************************************************

Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)



'********************************************************************
'�؂�ւ�
'********************************************************************
'' �{�N���X�𖢎g�p���̓X�C�b�`���Ă������� �����Ԃ� Ver.04����́A���g�p��OK�̂͂��B (���[�J����DLL�𒼂ŎQ�Ƃ��Ă����)
#Const tExcelON = True						' ���N���X���g�p���Ȃ��ꍇ�́AFalse

'' PC�ŃX�C�b�`���Ă������� �����Ԃ� Ver.04����́A2003�Œ��OK�̂͂��B�i�Q�Ƃ��Ă���DLL��2003�Œ�ɂ����ׁj
'#Const tExcelVer2000 = 2000                     ' Excel 2000 �p
#Const tExcelVer2003 = 2003						' Excel 2003 �p



'********************************************************************
'���ۂ̏���
'********************************************************************
#If tExcelON Then			'// ---------------------------------------------->>>>>

' Imports Excel					' ���C�u�����͑S�������I�ɏ����Ă��������ׁA����
' Imports Microsoft.Office		' ���C�u�����͑S�������I�ɏ����Ă��������ׁA����
Imports System.Drawing


Namespace tClass
    ''' <summary>
    ''' Excel����̌p���N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Excel
        Inherits tClass_Excel_Base

        '********************************************************************
        '���\�b�h��` (��{)
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            MyBase.New()
        End Sub

        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Overrides Sub Dispose()
            MyBase.Dispose()
        End Sub

        ''' <summary>
        ''' Finalize�f�X�g���N�^
        ''' </summary>
        ''' <remarks>Dispose���s���Ă��Ȃ��ꍇ�̂�</remarks>
        Protected Overrides Sub Finalize()
            MyBase.Finalize()
        End Sub

        '********************************************************************
        '���\�b�h��` (�g��)
        '********************************************************************
        ''' <summary>
        ''' �Z���̌r�� ��`�� �r���v���p�e�B�ɊY��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="LineStyle">���� [Nothing:�Z�b�g����]</param>
        ''' <param name="Weight">�� [Nothing:�Z�b�g����]</param>
        ''' <param name="color">�F [Nothing:�Z�b�g����]</param>
        ''' <param name="borderIndex">�`��ʒu Nothing:�S��</param>
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
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>        
        ''' <param name="LineStyle">���� [Nothing:�Z�b�g����]</param>
        ''' <param name="Weight">�� [Nothing:�Z�b�g����]</param>
        ''' <param name="color">�F [Nothing:�Z�b�g����]</param>
        ''' <param name="borderIndex">�`��ʒu Nothing:�S��</param>
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
        ' �T�v�F�Z�� �T�C�Y �t�H���g�v���p�e�B�ɊY��
        ' �����F(in :Integer) row1 = �s (1�ؼ��)
        ' �����F(in :Integer) col1 = �� (1�ؼ��)
        ' �����F(in :Integer) size = �t�H���g�T�C�Y
        ' �����F(in :Color) color = �����F
        ' �ߒl�F() 
        '----------------------------------------------------------------
        ''' <summary>
        ''' �Z�� �T�C�Y �t�H���g�v���p�e�B�ɊY��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="size">�t�H���g�T�C�Y</param>
        ''' <remarks></remarks>
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal size As Integer)
            MyBase.Cells(row1, col1).Font.Size = size
        End Sub
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
        ''' <param name="size">�t�H���g�T�C�Y</param>
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal size As Integer)
            MyBase.Cells(row1, col1, row2, col2).Font.Size = size
        End Sub
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="size">�t�H���g�T�C�Y</param>
        ''' <param name="color">�t�H���g�F</param>            
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal size As Integer, ByVal color As Color)
            Dim f As Excel.Font = MyBase.Cells(row1, col1).Font
            f.Size = size
            f.Color = ColorToRGB(color)
        End Sub
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
        ''' <param name="size">�t�H���g�T�C�Y</param>
        ''' <param name="color">�t�H���g�F</param>        
        Public Overloads Sub Font(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal size As Integer, ByVal color As Color)
            Dim f As Excel.Font = MyBase.Cells(row1, col1, row2, col2).Font
            f.Size = size
            f.Color = ColorToRGB(color)
        End Sub

        ''' <summary>
        ''' �Z�� �h��ׂ� �p�^�[���v���p�e�B�ɊY��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="color">�Z���F</param>
        ''' <param name="pattern">�Z���h��ׂ��p�^�[��</param>
        ''' <remarks></remarks>
        Public Overloads Sub Interior(ByVal row1 As Integer, ByVal col1 As Integer, ByVal color As Color, Optional ByVal pattern As Excel.XlPattern = Excel.XlPattern.xlPatternSolid)
            Dim inter As Excel.Interior = MyBase.Cells(row1, col1).Interior		'�h��ׂ��I�u�W�F�N�g
            inter.Color = ColorToRGB(color)
            inter.Pattern = pattern
        End Sub
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
        ''' <param name="color">�Z���F</param>
        ''' <param name="pattern">�Z���h��ׂ��p�^�[��</param>
        Public Overloads Sub Interior(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal color As Color, Optional ByVal pattern As Excel.XlPattern = Excel.XlPattern.xlPatternSolid)
            Dim inter As Excel.Interior = MyBase.Cells(row1, col1, row2, col2).Interior	   '�h��ׂ��I�u�W�F�N�g
            inter.Color = ColorToRGB(color)
            inter.Pattern = pattern
        End Sub

        ''' <summary>
        ''' �Z�� �z�u�v���p�e�B�ɊY��
        ''' </summary>
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
        ''' <param name="blnON">�A����� True:�A��</param>
        ''' <param name="x">���z�u</param>
        ''' <param name="y">�c�z�u</param>
        ''' <remarks></remarks>
        Public Overloads Sub Merge(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer, ByVal blnON As Boolean, Optional ByVal x As Excel.XlHAlign = Excel.XlHAlign.xlHAlignCenter, Optional ByVal y As Excel.XlVAlign = Excel.XlVAlign.xlVAlignBottom)
            Dim range As Excel.Range = MyBase.Cells(row1, col1, row2, col2)
            '' ��Ώ���
            range.WrapText = False		'�����܂�Ԃ�
            range.ShrinkToFit = False	'�����k��
            range.AddIndent = False		'�C���f���g
            range.Orientation = 0		'�p�x

            '' �C�ӏ���
            range.MergeCells = blnON		'�A��
            range.HorizontalAlignment = x	'���z�u
            range.VerticalAlignment = y		'�c�z�u
        End Sub

        '----------------------------------------------------------------
        ' �T�v�F�Z�� �t�H�}�b�g �\���`���v���p�e�B�ɊY��
        ' �����F(in :Integer) row1 = �s (1�ؼ��)
        ' �����F(in :Integer) col1 = �� (1�ؼ��)
        ' �����F(in :Integer) type = �t�H�[�}�b�g�^ (���l:0 ������:1 ���t:2 �t�H�[�}�b�g�w��:9)
        ' �����F(in :String) strFormat = �t�H�[�}�b�g(�t�H�[�}�b�g����)
        ' �ߒl�F() 
        '----------------------------------------------------------------
        ''' <summary>
        ''' �Z�� �t�H�}�b�g �\���`���v���p�e�B�ɊY��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="type">�t�H�[�}�b�g�^ (���l:0 ������:1 ���t:2 �t�H�[�}�b�g�w��:9)</param>
        ''' <param name="strFormat">�t�H�[�}�b�g(�t�H�[�}�b�g����)</param>
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
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
        ''' <param name="type">�t�H�[�}�b�g�^ (���l:0 ������:1 ���t:2 �t�H�[�}�b�g�w��:9)</param>
        ''' <param name="strFormat">�t�H�[�}�b�g(�t�H�[�}�b�g����)</param>
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
        ''' �s�� �w��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="size">�����T�C�Y</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowHeight(ByVal row1 As Integer, ByVal size As Integer)
            MyClass.RowHeight(row1, row1, size)
        End Sub
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="size">�����T�C�Y</param>
        Public Overloads Sub RowHeight(ByVal row1 As Integer, ByVal row2 As Integer, ByVal size As Integer)
            Dim strVal As String = row1 & ":" & row2
            CType(MyBase.m_xlsSheet.Rows(strVal), Excel.Range).RowHeight = size
            'MyBase.m_xlsSheet.Rows(strVal).RowHeight = size
        End Sub

        ''' <summary>
        ''' �� �w��
        ''' </summary>
        ''' <param name="col1">�� (1�ؼ��)</param>
        ''' <param name="size">���T�C�Y</param>
        ''' <remarks></remarks>
        Public Overloads Sub ColumnWidth(ByVal col1 As Integer, ByVal size As Integer)
            ColumnWidth(col1, col1, size)
        End Sub
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>          
        ''' <param name="size">���T�C�Y</param>
        Public Overloads Sub ColumnWidth(ByVal col1 As Integer, ByVal col2 As Integer, ByVal size As Integer)
            Dim strVal As String = MyBase.GetColumnName(col1, col2)
            CType(MyBase.m_xlsSheet.Columns(strVal), Excel.Range).ColumnWidth = size
            'MyBase.m_xlsSheet.Columns(strVal).ColumnWidth = size
        End Sub

        ''' <summary>
        ''' �s �}��
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowInsert(ByVal row1 As Integer)
            CType(MyBase.m_xlsSheet.Rows(row1), Excel.Range).Insert(Shift:=Excel.XlDirection.xlDown)
            'MyBase.m_xlsSheet.Rows(row1).Insert(Shift:=Excel.XlDirection.xlDown)
        End Sub
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>               
        Public Overloads Sub RowInsert(ByVal row1 As Integer, ByVal col1 As Integer, ByVal col2 As Integer)
            MyBase.m_xlsSheet.Range(MyBase.m_xlsSheet.Cells(row1, col1), MyBase.m_xlsSheet.Cells(row1, col2)).Insert(Shift:=Excel.XlDirection.xlDown)
        End Sub


        ''' <summary>
        ''' �s �R�s�[���y�[�X�g
        ''' </summary>
        ''' <param name="CopyRow">�R�s�[���s</param>
        ''' <param name="CopyCnt">�R�s�[����</param>
        ''' <param name="PasteRow">�y�[�X�g��s</param>
        ''' <param name="PasteCnt">�y�[�X�g����</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowCopyAndPaste(ByVal CopyRow As Integer, ByVal CopyCnt As Integer, ByVal PasteRow As Integer, ByVal PasteCnt As Integer)
            Rows(CopyRow, CopyCnt).Copy()
            Rows(PasteRow, PasteCnt).PasteSpecial(Excel.XlPasteType.xlPasteAll)
        End Sub
        ''' <summary>
        ''' �s �R�s�[���}��
        ''' </summary>
        ''' <param name="CopyRow">�R�s�[���s</param>
        ''' <param name="CopyCnt">�R�s�[����</param>
        ''' <param name="PasteRow">�y�[�X�g��s</param>
        ''' <param name="PasteCnt">�y�[�X�g����</param>
        ''' <remarks></remarks>
        Public Overloads Sub RowCopyAndInsert(ByVal CopyRow As Integer, ByVal CopyCnt As Integer, ByVal PasteRow As Integer, ByVal PasteCnt As Integer)
            Rows(CopyRow, CopyCnt).Copy()	' �R�s�[
            Rows(PasteRow, PasteCnt).Insert(Shift:=Excel.XlDirection.xlDown) '�I�� & �}��
        End Sub
        ''' <summary>
        ''' �� �R�s�[���}��
        ''' </summary>
        ''' <param name="CopyColumn">�R�s�[����</param>
        ''' <param name="CopyCnt">�R�s�[����</param>
        ''' <param name="PasteColumn">�y�[�X�g���</param>
        ''' <param name="PasteCnt">�y�[�X�g����</param>
        ''' <remarks></remarks>
        Public Overloads Sub ColumnCopyAndInsert(ByVal CopyColumn As Integer, ByVal CopyCnt As Integer, ByVal PasteColumn As Integer, ByVal PasteCnt As Integer)
            Columns(CopyColumn, CopyCnt).Copy()	' �R�s�[
            Columns(PasteColumn, PasteCnt).Insert(Shift:=Excel.XlDirection.xlToRight)  '�I�� & �}��
        End Sub



        ''' <summary>
        ''' �w��͈� �R�s�[���}��
        ''' </summary>
        ''' <param name="Frow1">�R�s�[�� �J�n�s (1�ؼ��)</param>
        ''' <param name="Fcol1">�R�s�[�� �J�n�� (1�ؼ��)</param>
        ''' <param name="Frow2">�R�s�[�� �I���s (1�ؼ��)</param>
        ''' <param name="Fcol2">�R�s�[�� �I���� (1�ؼ��)</param>
        ''' <param name="Trow1">�R�s�[�� �J�n�s (1�ؼ��)</param>
        ''' <param name="Tcol1">�R�s�[�� �J�n�� (1�ؼ��)</param>
        ''' <param name="Trow2">�R�s�[�� �I���s (1�ؼ��)</param>
        ''' <param name="Tcol2">�R�s�[�� �I���� (1�ؼ��)</param>
        ''' <param name="nMode">���[�h(0:�E�փV�t�g  1:���փV�t�g )</param>
        ''' <remarks></remarks>
        Public Overloads Sub CellCopyAndInsert(ByVal Frow1 As Integer, ByVal Fcol1 As Integer, ByVal Frow2 As Integer, ByVal Fcol2 As Integer, _
           ByVal Trow1 As Integer, ByVal Tcol1 As Integer, ByVal Trow2 As Integer, ByVal Tcol2 As Integer, _
           ByVal nMode As Integer)

            ' �R�s�[
            Dim Frange As Excel.Range = MyBase.Cells(Frow1, Fcol1, Frow2, Fcol2)
            Frange.Copy()

            ' �}��
            Dim Trange As Excel.Range = MyBase.Cells(Trow1, Tcol1, Trow2, Tcol2)

            If 0 = nMode Then
                Trange.Insert(Shift:=Excel.XlDirection.xlToRight)
            ElseIf 1 = nMode Then
                Trange.Insert(Shift:=Excel.XlDirection.xlDown)
            End If
        End Sub





        ''' <summary>
        ''' �Z�� �ʒu
        ''' </summary>
        ''' <param name="row1">�s (1�ؼ��)</param>
        ''' <param name="col1">�� (1�ؼ��)</param>
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
        ''' <param name="row1">�J�n�s (1�ؼ��)</param>
        ''' <param name="col1">�J�n�� (1�ؼ��)</param>
        ''' <param name="row2">�I���s (1�ؼ��)</param>
        ''' <param name="col2">�I���� (1�ؼ��)</param>  
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