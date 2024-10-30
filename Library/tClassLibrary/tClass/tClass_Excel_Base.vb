'*********************************************************************************
'Excel����̊��N���X
'	[Ver]
'		Ver.01    2006/02/11  vs2005 �Ή�
'		Ver.02    2006/03/22  getColumnName �s��Ή�
'		Ver.03    2007/03/06  �؂�ւ���ǉ�
'		Ver.04    2008/01/08  ���s���ɑΉ��i�قڑS����蒼���j
'       Ver.05    2008/01/11  CopySheet�����ǉ�
'
'	[����]
'		���{�N���X�́AMicrosoft Excel 9.0 Object Library ���g�p���Ċ֐������쐬���Ă��܂��B(Excel2000�x�[�X)
'		�{�N���X�́A�Q�Ɛݒ�i�Q�Ɓj�ňȉ���Dll��ǉ����Ă��܂��B(���[�J���R�s�[�֎~�B���ׁ̈AExcel�g�p���́AExe�Ɠ���K�w�ɁAExcelDll2000��dll��S�ăR�s�[)
'			�EInterop.Excel.dll						(���ꂪ���C��)
'			�EInterop.Office.dll					(���̏��APictuer�ɂ��������Ă��Ȃ�)
'			�EInterop.Microsoft.Office.Core.dll		(���̏��APictuer�ɂ��������Ă��Ȃ�) (�����2003�x�[�X��)
'			�i�ꏊ�́A���[�J����ExcelDll2000�ł��B�j 
'		���s����Excel���g�p�������ꍇ�́AExe�Ɠ����t�H���_���ɁAExcelDll2000��dll��S�ăR�s�[���Ă����΁AExcel2000�ł�2003�ł�2007�ł����s�ł��܂��B
'		COM�������ɊJ�����Ă���̂́AWeb(ASP.NET)�Ŏg�p����ۂɕK�v (���[�U�[�����ʂƈႤ�ׁA�悭�I�����Ă�Excel���c���)
'
'	[����2]
'		ExcelDll2000,2003�Ƃ���̂́A�ȉ��ׂ̈ł��B
'						���sPC
'		�R���p�C��   2000	2003	2007
'			2000	  ��	 ��		 ��
'			2003	  �~	 ��		 ��
'			2007	  �R���p�C�����ʂ�Ȃ�
'*********************************************************************************

Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)


'********************************************************************
'�؂�ւ�
'********************************************************************
'' �{�N���X�𖢎g�p���̓X�C�b�`���Ă������� �����Ԃ� Ver.04����́A���g�p��OK�̂͂��B (���[�J����DLL�𒼂ŎQ�Ƃ��Ă����)
#Const tExcelON = True						' ���N���X���g�p���Ȃ��ꍇ�́AFalse

'' PC�ŃX�C�b�`���Ă������� �����Ԃ� Ver.04����́A2000�Œ��OK�̂͂��B�i�Q�Ƃ��Ă���DLL��2000�Œ�ɂ����ׁj
#Const tExcelVer2000 = 2000						' Excel 2000 �p
'#Const tExcelVer2003 = 2003						' Excel 2003 �p



'********************************************************************
'���ۂ̏���
'********************************************************************
#If tExcelON Then			'// ---------------------------------------------->>>>>

' Imports Excel					' ���C�u�����͑S�������I�ɏ����Ă��������ׁA����
' Imports Microsoft.Office		' ���C�u�����͑S�������I�ɏ����Ă��������ׁA����
Imports System.Drawing

Namespace tClass
    ''' <summary>
    ''' Excel����̊��N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Excel_Base
        Implements IDisposable							' �f�X�g���N�^��`

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False				' �d������Ăяo�������o����ɂ�
        Protected m_xlsApp As Excel.ApplicationClass = Nothing	' ����1��1��Excel �A�v���P�[�V���� �v���Z�X
        Protected m_xlsBooks As Excel.Workbooks = Nothing		' �u�b�N�X
        Protected m_xlsBook As Excel.Workbook = Nothing			' ���݂̃A�N�e�B�u �u�b�N
        Protected m_xlsSheets As Excel.Sheets = Nothing			' �V�[�g�X
        Protected m_xlsSheet As Excel.Worksheet = Nothing		' ���݂̃A�N�e�B�u �V�[�g

        '********************************************************************
        '�v���p�e�B��`
        '********************************************************************
#Region "�v���p�e�B"
        '====================================================================
        '���[�N�u�b�N�擾 �v���p�e�B�[�̐ݒ�
        '====================================================================
        ''' <summary>
        ''' ���݂̃A�N�e�B�u �u�b�N �̎擾�E�ݒ�
        ''' </summary>
        ''' <value>�؂�ւ��惏�[�N�u�b�N</value>
        ''' <returns>���݂̃��[�N�u�b�N</returns>
        ''' <remarks></remarks>
        Public Overloads Property XlsBook() As Excel.Workbook
            Get
                Return m_xlsBook
            End Get
            Set(ByVal Value As Excel.Workbook)
                '' �u�b�N�̐؂�ւ�
                m_xlsBook = Value
                '' �u�b�N���؂�ւ��ƃV�[�g���ύX����
                MyClass.MRComObject(m_xlsSheets)
                m_xlsSheets = m_xlsBook.Worksheets
                MyClass.MRComObject(m_xlsSheet)
                m_xlsSheet = CType(m_xlsSheets.Item(1), Excel.Worksheet)
            End Set
        End Property

        ''' <summary>
        ''' �w�� �u�b�N �̎擾
        ''' </summary>
        ''' <param name="Index">�u�b�N�ԍ� 1�ؼ��</param>
        ''' <value></value>
        ''' <returns>���݂̃��[�N�u�b�N</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsBook(ByVal Index As Integer) As Excel.Workbook
            Get
                Try
                    '' �����`�F�b�N
                    If index <= 0 Or index > m_xlsBooks.Count Then Return Nothing
                    '' �u�b�N�擾
                    Return CType(m_xlsBooks.Item(Index), Excel.Workbook)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property
        ''' <param name="strName">"test"�ł�"test.xls"�ł�"c:\test.xls"�ł� �w��\</param>
        ''' <value></value>
        ''' <returns>���݂̃��[�N�u�b�N</returns>
        Public Overloads ReadOnly Property XlsBook(ByVal strName As String) As Excel.Workbook
            Get
                '''' �t�@�C�����ɕϊ�
                '�t�@�C�����擾
                Dim strFileName As String = IO.Path.GetFileName(strName)		'�u�b�N�̃t�@�C����
                If strFileName = "" Then Return Nothing '�� ���� "c:\test\" �`��

                '�g���q�`�F�b�N
                Dim wk As String = IO.Path.GetExtension(strFileName).ToLower
                If Not wk = ".xls" Then	'"test" ���� "test.aaa" �`�� 
                    If wk = "" Then
                        '�g���q��t�^
                        strFileName &= ".xls"
                    Else
                        Return Nothing
                    End If
                End If

                '''' �u�b�N�擾
                Try
                    Return CType(m_xlsBooks.Item(strFileName), Excel.Workbook)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        '====================================================================
        '���[�N�V�[�g�擾 �v���p�e�B�[�̐ݒ�
        '====================================================================
        ''' <summary>
        ''' ���݂̃A�N�e�B�u �V�[�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <value>�V�[�g</value>
        ''' <returns>�A�N�e�B�u�V�[�g</returns>
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
        ''' �w�� �V�[�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="Index">1�ؼ��</param>
        ''' <value></value>
        ''' <returns>�V�[�g</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsSheet(ByVal Index As Integer) As Excel.Worksheet
            Get
                '''' �����`�F�b�N
                If index <= 0 Or index > m_xlsSheets.Count Then Return Nothing
                Try
                    Return CType(m_xlsSheets.Item(Index), Excel.Worksheet)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        ''' <summary>
        ''' �w�� �V�[�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="strName">"Sheet1"</param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property XlsSheet(ByVal strName As String) As Excel.Worksheet
            Get
                '''' �����`�F�b�N
                If strName = "" Then Return Nothing
                Try
                    Return CType(m_xlsSheets.Item(strName), Excel.Worksheet)
                Catch ex As Exception
                    Return Nothing
                End Try
            End Get
        End Property

        '====================================================================
        '�Z���l �v���p�e�B�[�̐ݒ�
        '====================================================================
        ''' <summary>
        ''' �A�N�e�B�u�V�[�g �� �w��Z���̃f�[�^ �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="row">�s</param>
        ''' <param name="col">��</param>
        ''' <value>������</value>
        ''' <returns>�Z���l</returns>
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
        ''' �A�N�e�B�u�V�[�g �� �w��͈̓Z���̃f�[�^ �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="address">"A1"�̌`�� ("A1:A1"�ł���)</param>
        ''' <value>������</value>
        ''' <returns>�Z���l</returns>
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
        '�Z���̃I�u�W�F�N�g �v���p�e�B�[�̐ݒ�
        '====================================================================

        ''' <summary>
        ''' �s�P�ʂ̃I�u�W�F�N�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="row">�s�ʒu</param>
        ''' <param name="cnt">�s��</param>
        ''' <value>�Z���I�u�W�F�N�g</value>
        ''' <returns>�Z���I�u�W�F�N�g</returns>
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
        ''' ��P�ʂ̃I�u�W�F�N�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="col">��ʒu</param>
        ''' <param name="cnt">��</param>
        ''' <value>�Z���I�u�W�F�N�g</value>
        ''' <returns>�Z���I�u�W�F�N�g</returns>
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
        ''' �A�N�e�B�u�V�[�g �� �w��Z���̃I�u�W�F�N�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="row">�s</param>
        ''' <param name="col">��</param>
        ''' <value>�Z���I�u�W�F�N�g</value>
        ''' <returns>�Z���I�u�W�F�N�g</returns>
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
        ''' �A�N�e�B�u�V�[�g �� �w��͈̓Z���̃I�u�W�F�N�g �̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="row1">�J�n�s</param>
        ''' <param name="col1">�J�n��</param>
        ''' <param name="row2">�I���s</param>
        ''' <param name="col2">�I����</param>
        ''' <value></value>
        ''' <returns>�Z���I�u�W�F�N�g</returns>
        ''' <remarks></remarks>
        Public Overloads ReadOnly Property Cells(ByVal row1 As Integer, ByVal col1 As Integer, ByVal row2 As Integer, ByVal col2 As Integer) As Excel.Range
            Get
                If row1 < 1 And col1 < 1 And row2 < 1 And col2 < 1 Then Return Nothing
                Return m_xlsSheet.Range(m_xlsSheet.Cells(row1, col1), m_xlsSheet.Cells(row2, col2))
            End Get
        End Property
        ''' <summary>
        ''' �A�N�e�B�u�V�[�g �� �w��͈̓Z���̃I�u�W�F�N�g�̎擾�E�ݒ�
        ''' </summary>
        ''' <param name="address">"A1"�̌`�� ("A1:A1"�ł���)</param>
        ''' <value></value>
        ''' <returns>�Z���I�u�W�F�N�g</returns>
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
        ''' �z��̏����ꊇ�Őݒ�E�擾����
        ''' </summary>
        ''' <param name="row1">�J�n�s</param>
        ''' <param name="col1">�J�n��</param>
        ''' <param name="row2">�I���s</param>
        ''' <param name="col2">�I����</param>
        ''' <value>Z�̏��ŁA�f�[�^�Z�b�g</value>
        ''' <returns>Z�̏��ŁA�f�[�^�擾</returns>
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
        ''' 1�s�̏����ꊇ�Őݒ�E�擾����
        ''' </summary>
        ''' <param name="row">�w��s</param>
        ''' <param name="col1">�I���s</param>
        ''' <param name="col2">�I����</param>
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
        ''' 1��̏����ꊇ�Őݒ�E�擾����
        ''' </summary>
        ''' <param name="row1">�J�n�s</param>
        ''' <param name="row2">�I���s</param>
        ''' <param name="col">�w���</param>
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

        ' 20130626 �I�u�W�F�N�g��--------------------------------------------------->>>>
        ''' <summary>
        ''' �z��̏����ꊇ�Őݒ�E�擾����(�I�u�W�F�N�g�`��)
        ''' </summary>
        ''' <param name="row1">�J�n�s</param>
        ''' <param name="col1">�J�n��</param>
        ''' <param name="row2">�I���s</param>
        ''' <param name="col2">�I����</param>
        ''' <value>Z�̏��ŁA�f�[�^�Z�b�g</value>
        ''' <returns>Z�̏��ŁA�f�[�^�擾</returns>
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
        ''' 1�s�̏����ꊇ�Őݒ�E�擾����
        ''' </summary>
        ''' <param name="row">�w��s</param>
        ''' <param name="col1">�J�n�s</param>
        ''' <param name="col2">�I����</param>
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
        ''' 1��̏����ꊇ�Őݒ�E�擾����
        ''' </summary>
        ''' <param name="row1">�J�n�s</param>
        ''' <param name="row2">�I���s</param>
        ''' <param name="col">�w���</param>
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
        ' 20130626 �I�u�W�F�N�g��<<<<---------------------------------------------------

#End Region


        '********************************************************************
        '�X�^�e�B�b�N���\�b�h
        '********************************************************************

        ''' <summary>
        ''' Excel���C���X�g�[������Ă��邩�m�F
        ''' </summary>
        ''' <returns></returns>
        Public Shared Function Check_Install_Excel() As Boolean
            Dim xlsApp As Excel.ApplicationClass = Nothing

            Try
                '' Excel�C���X�^���X�����ŃG���[���́AExcel���C���X�g�[������Ă��Ȃ��Ɣ��f����
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
        ''' �񓯊���Excel�t�@�C�����N������
        ''' </summary>
        ''' <param name="strFPath">���s������Excel�t�@�C��</param>
        ''' <returns>�N�������v���Z�X</returns>
        ''' <remarks></remarks>
        Public Shared Function Start_ExcelFile(ByVal strFPath As String) As Process
            Try
                Return System.Diagnostics.Process.Start("Excel", strFPath)

            Catch ex As Exception
                Return Nothing
            End Try
        End Function


        ''' <summary>
        ''' .NET�̐F�w�肩��VB���̐F�w����@�ɕϊ����� ARGB��RGB
        ''' </summary>
        ''' <param name="color">�J���[�I�u�W�F�N�g</param>
        ''' <returns>RGB�`��</returns>
        ''' <remarks></remarks>
        Public Shared Function ColorToRGB(ByVal color As Color) As Integer
            Return RGB(color.R, color.G, color.B)
        End Function


        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '''' Excel �I�u�W�F�N�g����
            m_xlsApp = New Excel.ApplicationClass
            m_xlsBooks = m_xlsApp.Workbooks

            '''' ���s���x����
            m_xlsApp.ScreenUpdating = False		  '��ʍX�V�֎~
            m_xlsApp.Interactive = False		  '��Θb���[�h
            m_xlsApp.DisplayAlerts = False		  '�_�C�����O�o�͋֎~
        End Sub


        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                '''' Excel�N���[�Y
                MyClass.CloseAll()
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks>�g�p��A�K���R�[�����鎖</remarks>
        Public Overridable Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' Finalize�f�X�g���N�^
        ''' </summary>
        ''' <remarks>Dispose���s���Ă��Ȃ��ꍇ�̂�</remarks>
        Protected Overrides Sub Finalize()
            '''' Excel�N���[�Y
            MyClass.CloseAll()
        End Sub



        '********************************************************************
        ' COM�̊J�� (Excel�J��)
        '********************************************************************
#Region "COM�̊J��"
        '''' <summary>
        '''' COM�̊J��
        '''' </summary>
        '''' <param name="objxls">�J������I�u�W�F�N�g</param>
        '''' <remarks></remarks>
        'Protected Sub MRComObject(ByRef objxls As Object)
        '    '''' COM�̊J�� �ڍׂ�(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
        '    '''' Excel �I���������̃v���V�[�W��
        '    If IsNothing(objxls) Then Exit Sub
        '    Try
        '        '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
        '        System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
        '    Catch
        '    Finally
        '        objxls = Nothing
        '    End Try
        'End Sub

        ''' <summary>
        ''' COM�̊J��
        ''' </summary>
        ''' <param name="objxls">�J������I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.ApplicationClass)
            '''' COM�̊J�� �ڍׂ�(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
            '''' Excel �I���������̃v���V�[�W��
            If IsNothing(objxls) Then Exit Sub
            Try
                '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">�J������I�u�W�F�N�g</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Workbooks)
            If IsNothing(objxls) Then Exit Sub
            Try
                '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">�J������I�u�W�F�N�g</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Workbook)
            If IsNothing(objxls) Then Exit Sub
            Try
                '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">�J������I�u�W�F�N�g</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Sheets)
            If IsNothing(objxls) Then Exit Sub
            Try
                '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
        ''' <param name="objxls">�J������I�u�W�F�N�g</param>
        Protected Overloads Sub MRComObject(ByRef objxls As Excel.Worksheet)
            If IsNothing(objxls) Then Exit Sub
            Try
                '�񋟂��ꂽ�����^�C���Ăяo���\���b�p�[�̎Q�ƃJ�E���g���f�N�������g���܂�
                System.Runtime.InteropServices.Marshal.ReleaseComObject(objxls)
            Catch
            Finally
                objxls = Nothing
            End Try
        End Sub
#End Region

        '********************************************************************
        ' ��ʓI�ȑ���
        '********************************************************************

        ''' <summary>
        ''' �u�b�N���I�[�v��
        ''' </summary>
        ''' <param name="strFPath">Excel�t�@�C���t���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function Open(ByVal strFPath As String) As Boolean
            If IsNothing(m_xlsApp) Then Return False
            Try
                '''' Excel�t�@�C���I�[�v��
                MyClass.XlsBook = m_xlsBooks.Open(Filename:=strFPath)
                Return True
            Catch ex As Exception
                MyClass.MRComObject(m_xlsSheets)
                MyClass.MRComObject(m_xlsBook)
                '''' �I�[�v�����s
                Return False
            End Try
        End Function
        ''' <param name="strFPath">Excel�t�@�C���t���p�X</param>
        ''' <param name="strSheetName">Excel�V�[�g��</param>
        ''' <returns>���A���</returns>
        Public Overloads Function Open(ByVal strFPath As String, ByVal strSheetName As String) As Boolean
            '''' �I�[�v��
            If Not Open(strFPath) Then Return False

            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.SheetChenge(strSheetName)
            Return True
        End Function
        ''' <param name="strFPath">Excel�t�@�C���t���p�X</param>
        ''' <param name="sheetIndex">�V�[�g�ԍ� (1�ؼ��)</param>
        ''' <returns>���A���</returns>
        Public Overloads Function Open(ByVal strFPath As String, ByVal sheetIndex As Integer) As Boolean
            '''' �I�[�v��
            If Not Open(strFPath) Then Return False

            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.SheetChenge(sheetIndex)
            Return True
        End Function
        ''' <summary>
        ''' �V�����u�b�N��ǉ�
        ''' </summary>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function OpenAdd() As Boolean
            If IsNothing(m_xlsApp) Then Return False
            Try
                '''' Excel�t�@�C���I�[�v��
                MyClass.XlsBook = m_xlsBooks.Add()
                Return True
            Catch ex As Exception
                MyClass.MRComObject(m_xlsSheets)
                MyClass.MRComObject(m_xlsBook)
                '''' �I�[�v�����s
                Return False
            End Try
        End Function
        ''' <param name="strSheetName">�u�b�N��ǉ������ہA�w�肵���V�[�g���A�N�e�B�O�V�[�g�Ƃ���</param>
        ''' <returns></returns>
        Public Overloads Function OpenAdd(ByVal strSheetName As String) As Boolean
            '''' �I�[�v��
            If Not OpenAdd() Then Return False

            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.SheetChenge(strSheetName)
            Return True
        End Function
        ''' <param name="sheetindex">�u�b�N��ǉ������ہA�w�肵���V�[�g���A�N�e�B�O�V�[�g�Ƃ���</param>
        ''' <returns></returns>
        Public Overloads Function OpenAdd(ByVal sheetIndex As Integer) As Boolean
            '''' �I�[�v��
            If Not OpenAdd() Then Return False

            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.SheetChenge(sheetIndex)
            Return True
        End Function

        ''' <summary>
        ''' �w��u�b�N�̏I��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Close()
            '''' ���݃A�N�e�B�u�ȃu�b�N���I��
            m_xlsBook.Close()
            MyClass.MRComObject(m_xlsBook)
        End Sub
        ''' <param name="bookIndex">�u�b�N�ԍ� (1�I���W��)</param>
        ''' <remarks></remarks>
        Public Sub Close(ByVal bookIndex As Integer)
            Dim book As Excel.Workbook = MyClass.XlsBook(bookIndex)
            book.Close()
            MyClass.MRComObject(book)
        End Sub
        ''' <param name="bookName">�u�b�N����</param>
        ''' <remarks></remarks>
        Public Sub Close(ByVal bookName As String)
            Dim book As Excel.Workbook = MyClass.XlsBook(bookName)
            book.Close()
            MyClass.MRComObject(book)
        End Sub

        ''' <summary>
        ''' Excel�N���[�Y
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub CloseAll()
            On Error GoTo ErrAri

            '''' COM�̊J�� �ڍׂ�(http://homepage1.nifty.com/MADIA/vb/vb_bbs2/200408_04080020.html)
            '''' �V�[�g �J��
            MyClass.MRComObject(m_xlsSheet)
            MyClass.MRComObject(m_xlsSheets)

            '''' �S�u�b�N�N���[�Y
            For ii As Integer = 1 To m_xlsBooks.Count
                MyClass.Close(ii)
            Next ii

            '''' �u�b�N �J��
            MyClass.MRComObject(m_xlsBooks)

            '''' ���s���x����
            m_xlsApp.ScreenUpdating = True		 '��ʍX�V�֎~
            m_xlsApp.Interactive = True			  '��Θb���[�h
            m_xlsApp.DisplayAlerts = True		  '�_�C�����O�o�͋֎~

            '''' Excel���I��
            m_xlsApp.Quit()

            '''' Excel �A�v���P�[�V���� �J��
            MyClass.MRComObject(m_xlsApp)

            '''' �����ł��_���ȏꍇ������
            '''' CreateObject�Ŋm�ۂ����I�u�W�F�N�g���ACreateObject�𔭍s����*�X���b�h*�ɂ���ĎQ�Ƃ���Ă��邽�߁A�K�x�[�W�R���N�^���K�x�[�W�ł͂Ȃ��Ɣ��f���Ă��邽��
            '''' �X���b�h�����N���X�̒���Nothing�ɂ��邾���łȂ��A�X���b�h�����N���X���g��Nothing���Ȃ���ΐ���Ȃ�
            '''' �܂�A�Ăь��� New�����C���X�^���X�� Nothing ������ GC.Collect() �������OK�̂͂�

            ''''''' ��̏��������ł� VB.NET�ł͊J������Ȃ�
            ''''''' �Ȃ�ł��� �K�x�[�W�R���N�V�������܂����W���Ă��Ȃ��ׂł��B
            ''''''' ���Ď��� Excel�̑S�Ẵ������̎Q�Ƃ��J�����Ă��� �K�x�[�W�R���N�V�����𖾎��I�Ɏ��s�������Ⴂ�܂��B
            '''''''  ��������ł��_���Ȏ��́AGC.Collect(2) �ɂ��Ă݂Ă������� 85,000byte�ȏ�̃I�u�W�F�N�g�͏�ɃW�F�l���[�V����2�ɒu����Ă��邩��ł�

            '''' �ʏ� COM�̊J�� �������ɍs���Ă����� GC ���Ă΂Ȃ��Ă��ǂ��͂��i�O�ׂ̈ɃR�[���j
            GC.Collect()
            Return
ErrAri:
        End Sub

        ''' <summary>
        ''' Excel�̕ۑ�
        ''' </summary>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Save() As Boolean
            On Error GoTo ErrAri
            '''' �I�u�W�F�N�g�`�F�b�N
            If m_xlsBook Is Nothing Then Return False
            '''' �Ǎ��ݐ�p�̏ꍇ���ُ�Ƃ���
            If m_xlsBook.ReadOnly Then Return False

            '''' Excel���㏑������
            m_xlsBook.Save()

            Return True
ErrAri:
            Return False
        End Function

        ''' <summary>
        ''' Excel�̖��O��t���ĕۑ�
        ''' </summary>
        ''' <param name="strFPath">Excel�t�@�C���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function Save(ByVal strFPath As String) As Boolean
            On Error GoTo ErrAri
            '''' �I�u�W�F�N�g�`�F�b�N
            If m_xlsBook Is Nothing Then Return False
            If strFPath = "" Then Return False
            '''' Excel�𖼑O��t���ĕۑ�
            m_xlsBook.SaveAs(Filename:=strFPath)
            Return True
ErrAri:
            Return False
        End Function

        ''' <summary>
        ''' �Ώۃu�b�N�̕ύX�i���̌�͕K���V�[�g�̕ύX���s���j
        ''' </summary>
        ''' <param name="bookIndex">�u�b�N�̃C���f�b�N�X (1�ؼ��)</param>
        ''' <remarks></remarks>
        Public Overloads Sub BookChenge(ByVal bookIndex As Integer)
            Try
                '''' �Z�b�g�ΏۃV�[�g�ύX
                MyClass.XlsBook = MyClass.XlsBook(bookIndex)
                '''' �֘A�I�u�W�F�N�g�ɃZ�b�g
                m_xlsSheets = MyClass.XlsBook.Worksheets
            Catch ex As Exception
            End Try
        End Sub
        ''' <param name="bookName">�u�b�N����</param>
        Public Overloads Sub BookChenge(ByVal bookName As String)
            Try
                '''' �Z�b�g�ΏۃV�[�g�ύX
                MyClass.XlsBook = MyClass.XlsBook(bookName)
                '''' �֘A�I�u�W�F�N�g�ɃZ�b�g
                m_xlsSheets = MyClass.XlsBook.Worksheets
            Catch ex As Exception
            End Try
        End Sub
        ''' <summary>
        ''' �ΏۃV�[�g�̕ύX
        ''' </summary>
        ''' <param name="sheetIndex">�V�[�g�̃C���f�b�N�X (1�ؼ��)</param>
        ''' <remarks></remarks>
        Public Overloads Sub SheetChenge(ByVal sheetIndex As Integer)
            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.XlsSheet = MyClass.XlsSheet(sheetIndex)
        End Sub
        ''' <param name="sheetName">�V�[�g����</param>
        Public Overloads Sub SheetChenge(ByVal sheetName As String)
            '''' �Z�b�g�ΏۃV�[�g�ύX
            MyClass.XlsSheet = MyClass.XlsSheet(sheetName)
        End Sub

        ''' <summary>
        ''' �}�N���̎��s
        ''' </summary>
        ''' <param name="strMacroName">�}�N����</param>
        ''' <remarks>�V�[�g�w�莞�A"Sheet5.Test"�B "Test"���ƕW�����W���[���̊֐��������s�����</remarks>
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
        ''' �w�肵����̗񖼂��擾���� 1 �Ǝw�肷��� "A"���Ԃ�
        ''' </summary>
        ''' <param name="col1">�w���</param>
        ''' <returns>�A�h���X"A"</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetColumnName(ByVal col1 As Integer) As String
            Dim strwk() As String
            'Ver.02
            'strwk = CStr(CType(m_xlsSheet.Columns(1), Excel.Range).Address(ColumnAbsolute:=False)).Split(":"c)
            strwk = CStr(CType(m_xlsSheet.Columns(col1), Excel.Range).Address(ColumnAbsolute:=False)).Split(":"c)
            Return strwk(0)
        End Function
        ''' <param name="col1">�J�n��</param>
        ''' <param name="col2">�I����</param>
        ''' <returns>�A�h���X"A:B"</returns>
        Public Overloads Function GetColumnName(ByVal col1 As Integer, ByVal col2 As Integer) As String
            Return MyClass.GetColumnName(col1) & ":" & MyClass.GetColumnName(col2)
        End Function

        ''' <summary>
        ''' �w��V�[�g�̎w���̍ŏI�s�����߂�
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="SerchCol">������</param>
        ''' <param name="CheckRow">�`�F�b�N�J�n�s (���̒l��菬�����Ȃ鎖�͖���)</param>
        ''' <returns>�s 1�ؼ��</returns>
        ''' <remarks>��\���s��ɂ͖��Ή�</remarks>
        Public Function GetEndRow(ByVal sheet As Excel.Worksheet, ByVal SerchCol As Integer, ByVal CheckRow As Integer) As Integer
            Dim iVal As Integer
            iVal = CType(sheet.Cells(65536, SerchCol), Excel.Range).End(Excel.XlDirection.xlUp).Row
            Return CInt(IIf(iVal < CheckRow, CheckRow, iVal))
        End Function

        ''' <summary>
        ''' �w��V�[�g�̎w��s�̍ŏI������߂�
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="SerchRow">�����s</param>
        ''' <param name="CheckCol">�`�F�b�N�J�n�� (���̒l��菬�����Ȃ鎖�͖���)</param>
        ''' <returns>�� 1�ؼ��</returns>
        ''' <remarks>��\���s��ɂ͖��Ή�</remarks>
        Public Function GetEndCol(ByVal sheet As Excel.Worksheet, ByVal SerchRow As Integer, ByVal CheckCol As Integer) As Integer
            Dim iVal As Integer
            iVal = CType(sheet.Cells(SerchRow, 256), Excel.Range).End(Excel.XlDirection.xlToLeft).Column
            Return CInt(IIf(iVal < CheckCol, CheckCol, iVal))
        End Function

        ''' <summary>
        ''' Pixel�P�ʂ���|�C���g�P�ʂɕϊ�����
        ''' </summary>
        ''' <param name="nVal">Pixel�P��</param>
        ''' <returns>�|�C���g�P��</returns>
        ''' <remarks></remarks>
        Public Function GetPixelToPoint(ByVal nVal As Integer) As Single
            Return CSng(nVal * 3 / 4)
        End Function
        ''' <summary>
        ''' �|�C���g�P�ʂ���Pixel�P�ʂɕϊ�����
        ''' </summary>
        ''' <param name="nVal">�|�C���g�P��</param>
        ''' <returns>Pixel�P��</returns>
        ''' <remarks></remarks>
        Public Function GetPointToPixel(ByVal nVal As Integer) As Single
            Return CSng(nVal * 4 / 3)
        End Function

        '********************************************************************
        ' �������
        '****************************s****************************************

        ''' <summary>
        ''' �w��V�[�g�̃R���g���[�����擾����
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="strName">�R���g���[������</param>
        ''' <returns>�R���g���[��</returns>
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
        ''' �w��V�[�g�̃R���g���[�����擾����
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="strName">�R���g���[������</param>
        ''' <returns>�R���g���[��</returns>
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
        ''' �}�`�ǉ�
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="strPath">�}�`�p�X</param>
        ''' <param name="left">���[����̈ʒu[pixel]</param>
        ''' <param name="top">��[����̈ʒu[pixel]</param>
        ''' <param name="width">��[pixel]</param>
        ''' <param name="height">����[pixel]</param>
        ''' <remarks></remarks>
        Public Overloads Sub AddPictuer(ByVal sheet As Excel.Worksheet, ByVal strPath As String, ByVal left As Integer, ByVal top As Integer, ByVal width As Integer, ByVal height As Integer)
#If tExcelVer2000 Then
            '' Microsoft Excel 9.0 Object Library �p
            sheet.Shapes.AddPicture(strPath, Office.MsoTriState.msoFalse, Office.MsoTriState.msoTrue, GetPixelToPoint(left), GetPixelToPoint(top), GetPixelToPoint(width), GetPixelToPoint(height))
#ElseIf tExcelVer2003 Then
            '' Microsoft Excel 11.0 Object Library �p
            sheet.Shapes.AddPicture(strPath, Microsoft.Office.Core.MsoTriState.msoFalse, Microsoft.Office.Core.MsoTriState.msoTrue, GetPixelToPoint(left), GetPixelToPoint(top), GetPixelToPoint(width), GetPixelToPoint(height))
#End If
        End Sub
        ' ���݂̃V�[�g
        Public Overloads Sub AddPictuer(ByVal strPath As String, ByVal left As Integer, ByVal top As Integer, ByVal width As Integer, ByVal height As Integer)
            Call AddPictuer(m_xlsSheet, strPath, left, top, width, height)
        End Sub

        ''' <summary>
        ''' �}�`�ǉ�
        ''' </summary>
        ''' <param name="sheet">�ǂ̃V�[�g</param>
        ''' <param name="row">�s�ʒu</param>
        ''' <param name="col">��ʒu</param>
        ''' <param name="img">�摜�C���[�W</param>
        Public Overloads Sub AddPictuer(ByVal sheet As Excel.Worksheet, ByVal row As Integer, ByVal col As Integer, ByVal img As Image)
            ' �摜�f�[�^���N���b�v�{�[�h��
            My.Computer.Clipboard.SetImage(img)
            ' �������݈ʒu��I��
            CType(sheet.Cells(row, col), Excel.Range).Select()
            ' ��������
            sheet.PasteSpecial(Format:="�r�b�g�}�b�v", Link:=False, DisplayAsIcon:=False)
        End Sub
        ' ���݂̃V�[�g
        Public Overloads Sub AddPictuer(ByVal row As Integer, ByVal col As Integer, ByVal img As Image)
            Call AddPictuer(m_xlsSheet, row, col, img)
        End Sub


        ''' <summary>
        ''' �V�[�g�R�s�[
        ''' </summary>
        ''' <param name="sheetIndex">�R�s�[���V�[�g�C���f�b�N�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function CopySheet(ByVal sheetIndex As Integer, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet

            Try

                ''''�R�s�[���̃V�[�g���Z�b�g
                xlsFromSheet = CType(m_xlsSheets.Item(sheetIndex), Excel.Worksheet)

                If bPos Then
                    ''''�R�s�[���̑O�ɃR�s�[
                    xlsFromSheet.Copy(xlsFromSheet)
                Else

                    ''''�R�s�[���̌��ɃR�s�[
                    xlsFromSheet.Copy(, xlsFromSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try


        End Function

        ''' <param name="sheetName">�R�s�[���V�[�g��</param>
        Public Overloads Function CopySheet(ByVal sheetName As String, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet

            Try

                ''''�R�s�[���̃V�[�g���Z�b�g
                xlsFromSheet = CType(m_xlsSheets.Item(sheetName), Excel.Worksheet)

                If bPos Then
                    ''''�R�s�[���̑O�ɃR�s�[
                    xlsFromSheet.Copy(xlsFromSheet)
                Else
                    ''''�R�s�[���̌��ɃR�s�[
                    xlsFromSheet.Copy(, xlsFromSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <summary>
        ''' �V�[�g�R�s�[
        ''' </summary>
        ''' <param name="sheetIndex">�R�s�[���V�[�g�C���f�b�N�X</param>
        ''' <param name="sheetToIndex">��V�[�g�C���f�b�N�X</param>
        ''' <param name="bPos">�R�s�[�ʒu(True:�C�ӂ̃V�[�g�̑O,False:�R�s�[���̌��</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function CopySheet(ByVal sheetIndex As Integer, ByVal sheetToIndex As Integer, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet
            Dim xlsToSheet As Excel.Worksheet


            Try

                ''''�R�s�[���̃V�[�g,��V�[�g���Z�b�g
                xlsFromSheet = CType(m_xlsSheets.Item(sheetIndex), Excel.Worksheet)
                xlsToSheet = CType(m_xlsSheets.Item(sheetToIndex), Excel.Worksheet)

                If bPos Then
                    ''''��V�[�g�̑O�ɃR�s�[
                    xlsFromSheet.Copy(xlsToSheet)
                Else

                    ''''��V�[�g�̌��ɃR�s�[
                    xlsFromSheet.Copy(, xlsToSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <param name="sheetName">�R�s�[���V�[�g��</param>
        ''' <param name="sheetToName">��V�[�g��</param>
        Public Overloads Function CopySheet(ByVal sheetName As String, ByVal sheetToName As String, ByVal bPos As Boolean) As Boolean

            Dim xlsFromSheet As Excel.Worksheet
            Dim xlsToSheet As Excel.Worksheet

            Try

                ''''�R�s�[���̃V�[�g,��V�[�g���Z�b�g
                xlsFromSheet = CType(m_xlsSheets.Item(sheetName), Excel.Worksheet)
                xlsToSheet = CType(m_xlsSheets.Item(sheetToName), Excel.Worksheet)

                If bPos Then
                    ''''��V�[�g�̑O�ɃR�s�[
                    xlsFromSheet.Copy(xlsToSheet)
                Else

                    ''''��V�[�g�̌��ɃR�s�[
                    xlsFromSheet.Copy(, xlsToSheet)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <summary>
        ''' �V�[�g�̗L���`�F�b�N
        ''' </summary>
        ''' <param name="sheetIndex">�V�[�g�C���f�b�N�X</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function CheckSheet(ByVal sheetIndex As Integer) As Boolean

            For Each xlsSheet As Excel.Worksheet In m_xlsSheets

                '��v����V�[�g���`�F�b�N
                If xlsSheet.Index = sheetIndex Then
                    Return True
                End If

            Next xlsSheet

            Return False

        End Function
        ''' <param name="sheetname">�V�[�g����</param>
        Public Overloads Function CheckSheet(ByVal sheetName As String) As Boolean

            For Each xlsSheet As Excel.Worksheet In m_xlsSheets

                '��v����V�[�g���`�F�b�N
                If xlsSheet.Name = sheetName Then
                    Return True
                End If

            Next xlsSheet

            Return False

        End Function

        ''' <summary>
        ''' �V�[�g���̕ύX
        ''' </summary>
        ''' <param name="sheetName">�V�[�g���̕ύX</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Function ChangeSheetName(ByVal sheetName As String) As Boolean

            Try
                '�V�[�g���Z�b�g
                m_xlsSheet.Name = sheetName
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function


        ''' <summary>
        ''' ��� (�w��V�[�g�S�����)
        ''' </summary>
        ''' <param name="sheetIndex">�V�[�g�C���f�b�N�X</param>
        ''' <param name="printerName">�v�����^��</param>
        ''' <param name="printCout">�v�����g����</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function PrintOut(ByVal sheetIndex As Integer, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                '�V�[�g�`�F�b�N
                If Not CheckSheet(sheetIndex) Then Return False

                '''' �Y���V�[�g���A�N�e�B�u�ɂ���
                SheetChenge(sheetIndex)

                If "" <> printerName Then
                    '�v�����^�[�����w�肵�Ĉ��
                    m_xlsSheet.PrintOut(, , printCout, , printerName, , True)
                Else
                    m_xlsSheet.PrintOut(, , printCout, , , , True)
                End If

                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function

        ''' <param name="sheetName">�V�[�g��</param>
        Public Overloads Function PrintOut(ByVal sheetName As String, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                '�V�[�g�`�F�b�N
                If Not CheckSheet(sheetName) Then Return False

                '''' �Y���V�[�g���A�N�e�B�u�ɂ���
                SheetChenge(sheetName)

                If "" <> printerName Then
                    '�v�����^�[�����w�肵�Ĉ��
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
        ''' ��� (����y�[�W�͈͎w��) Ver.1.2.1.0
        ''' </summary>
        ''' <param name="sheetIndex">�V�[�g�C���f�b�N�X</param>
        ''' <param name="PageFrom">����J�n�y�[�WNo</param>
        ''' <param name="PageTo">����I���y�[�WNo</param>
        ''' <param name="printerName">�v�����^��</param>
        ''' <param name="printCout">�v�����g����</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Function PrintOut(ByVal sheetIndex As Integer, ByVal PageFrom As Integer, ByVal PageTo As Integer, Optional ByVal printerName As String = "", Optional ByVal printCout As Integer = 1) As Boolean
            Try
                '�V�[�g�`�F�b�N
                If Not CheckSheet(sheetIndex) Then Return False

                '''' �Y���V�[�g���A�N�e�B�u�ɂ���
                SheetChenge(sheetIndex)

                If "" <> printerName Then
                    '�v�����^�[�����w�肵�Ĉ��
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
                '�V�[�g�`�F�b�N
                If Not CheckSheet(sheetName) Then Return False

                '''' �Y���V�[�g���A�N�e�B�u�ɂ���
                SheetChenge(sheetName)

                If "" <> printerName Then
                    '�v�����^�[�����w�肵�Ĉ��
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