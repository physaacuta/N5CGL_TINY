'*********************************************************************************
' �R�C�������(OPL)
'	[Ver]
'		Ver.01    2012/07/13  ����
'
'	[����]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmDspCoil
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_SqlGetRow(3 - 1) As DataRow           ' 0:���R�C���A1:���R�C���A2:�O�R�C��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �O���b�h�@��ݒ�
    Private Enum EM_GRID_COL_ID
        em_txtName = 0      '����
        em_txtData          '�f�[�^
        em_txtInfo          '���e
        em_Cnt              '��
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const m_WidthMin As Integer = 661       ' �t�H�[���T�C�Y�ύX���A�����̍ŏ��l
    Private Const m_HeightMin As Integer = 305      ' �t�H�[���T�C�Y�ύX���A�����̍ŏ��l
    Private m_cKizukenNo As String                  ' �o���Ǘ�No
    Private m_dctItem As Dictionary(Of String, String)

    ''' <summary>
    ''' �t�H�[���A�N�e�B�u
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Activated

        '' �ړ��\
        'Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
        'Me.MaximizeBox = False
        'Me.MinimizeBox = False
        'Me.tProp_EndButtonEnable = False

        '' �k���ł���T�C�Y�̌��E���w�肷��
        'Me.MinimumSize = New Size(m_WidthMin, m_HeightMin)

    End Sub


    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        ' ������
        For ii As Integer = 0 To m_SqlGetRow.Length - 1
            m_SqlGetRow(ii) = Nothing
        Next ii

        ' �t�@���N�V�����{�^����`
        MyBase.AddFunctionButton(BtnCoilNext, 0)
        MyBase.AddFunctionButton(BtnCoilBack, 1)
        MyBase.AddFunctionButton(BtnClose, 11)

        If BtnCSV.Visible = True Then
            MyBase.AddFunctionButton(BtnCSV, 10)
        End If

        ' dgv�����ݒ�
        Call DgvInit(dgvCoil)

        ' DB ���ږ��擾
        Call GetclmSQL()

        ' SQL�擾(�����\�� �ŐV�R�C���̎擾)
        Call GetSql(0, "")

        ' DB �f�[�^��\��
        Call SqlDsp()

    End Sub

    ''' <summary>
    ''' �t�H�[�����T�C�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_ResizeEnd(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.ResizeEnd

        ' �t�H�[���T�C�Y���ύX����Ă����������ϓ��Ɋ���t���邽�߂̏���
        'dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill
        dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDspCoil_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Dim nMode As Integer                    ' SQL�擾���[�h
        Dim strKanriNo As String                ' �Ǘ�No

        ' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��

            Case 0                              ' F01 ���R�C��
                nMode = 1

                ' �Ǘ�No�擾
                strKanriNo = m_SqlGetRow(0).Item("�Ǘ�No").ToString

                ' �R�C�����擾
                Call GetSql(nMode, strKanriNo)

                ' DB �f�[�^��\��
                Call SqlDsp()

            Case 1                              ' F02 �O�R�C��
                nMode = 2

                ' �Ǘ�No�擾
                strKanriNo = m_SqlGetRow(2).Item("�Ǘ�No").ToString

                ' �R�C�����擾
                Call GetSql(nMode, strKanriNo)

                ' DB �f�[�^��\��
                Call SqlDsp()

            Case 10                             ' F11 CSV�o��
                Call CsvOut(CSV_PATH, "�R�C�������.csv", True)
                
            Case 11                             ' F12 ����
                Me.Dispose()
        End Select

    End Sub

    ''' <summary>
    ''' DB���ږ��擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetclmSQL()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlAdpter As New SqlClient.SqlDataAdapter
        Dim sqlData As New DataSet
        Dim strSQL As String                    ' SQL��
        Dim ii As Integer                       ' INDEX

        Try
            ' DB�I�[�v��
            If Not tcls_DB.Open() Then Exit Sub

            ' SQL���쐬
            strSQL = ""
            strSQL &= " SELECT NAME"
            strSQL &= " FROM SYS.COLUMNS"
            strSQL &= " WHERE OBJECT_ID = OBJECT_ID('" & DB_COIL_INF & "')"

            ' Select�����s 
            sqlData.Clear()
            sqlAdpter = tcls_DB.SelectAdapter(strSQL)

            sqlAdpter.Fill(sqlData)

            ' DB���ږ��̃Z�b�g
            Dim sqlGet() As DataRow = sqlData.Tables(0).Select()

            ' dgv�\������
            dgvCoil.RowCount = sqlData.Tables(0).Select().Length
            For ii = 0 To dgvCoil.RowCount - 1
                dgvCoil(0, ii).Value = sqlGet(ii).Item("NAME")

                If m_dctItem.ContainsKey(CStr(dgvCoil(0, ii).Value)) Then
                    dgvCoil(2, ii).Value = m_dctItem(CStr(dgvCoil(0, ii).Value))
                End If
            Next ii

            sqlAdpter.Dispose()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[" & DB_COIL_INF & "] ���̎擾���s")
        Finally
            tcls_DB.Cancel()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub



    ''' <summary>
    ''' SQL�����s
    ''' </summary>
    ''' <param name="Mode">SQL���擾���[�h</param>
    ''' <param name="KanriNo">�Ǘ�No</param>
    ''' <remarks></remarks>
    Private Sub GetSql(ByVal Mode As Integer, ByVal KanriNo As String)

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlAdpter As New SqlClient.SqlDataAdapter
        Dim sqlData As New DataSet
        Dim strSQL As String                    ' SQL��

        Try
            ' DB�I�[�v��
            If Not tcls_DB.Open() Then Exit Sub

            ' 0:�����\�� 1:���R�C�� 2:�O�R�C��
            Select Case Mode

                Case 0
                    ' SQL���쐬
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.�Ǘ�No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.�Ǘ�No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " ORDER BY �Ǘ�No DESC"
                    End If

                    ' Select�����s 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DB�̒l���擾
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(ii) = Nothing
                        End If
                    Next ii

                Case 1
                    ' SQL���쐬
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.�Ǘ�No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.�Ǘ�No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " WHERE �Ǘ�No >= ('" & KanriNo & "') "
                        strSQL &= " ORDER BY �Ǘ�No ASC"
                    End If

                    ' Select�����s 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DB�̒l���擾
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    m_SqlGetRow(2) = m_SqlGetRow(1)
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(2 - ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(2 - ii) = Nothing
                        End If
                    Next ii

                Case 2
                    ' SQL���쐬
                    If m_cKizukenNo IsNot Nothing Then
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 INF.*"
                        strSQL &= " FROM " & DB_COIL_INF & " as INF"
                        strSQL &= " WHERE INF.�Ǘ�No = '" & m_cKizukenNo & "'"
                        strSQL &= " ORDER BY INF.�Ǘ�No DESC"
                    Else
                        strSQL = ""
                        strSQL &= " SELECT TOP 2 *"
                        strSQL &= " FROM " & DB_COIL_INF
                        strSQL &= " WHERE �Ǘ�No <= ('" & KanriNo & "') "
                        strSQL &= " ORDER BY �Ǘ�No DESC"
                    End If

                    ' Select�����s 
                    sqlData.Clear()
                    sqlAdpter = tcls_DB.SelectAdapter(strSQL)
                    sqlAdpter.Fill(sqlData)

                    sqlAdpter.Dispose()

                    ' DB�̒l���擾
                    Dim sqlGet() As DataRow = sqlData.Tables(0).Select()
                    m_SqlGetRow(0) = m_SqlGetRow(1)
                    For ii As Integer = 1 To m_SqlGetRow.Length - 1
                        If ii <= sqlGet.Length Then
                            m_SqlGetRow(ii) = sqlGet(ii - 1)
                        Else
                            m_SqlGetRow(ii) = Nothing
                        End If
                    Next ii

            End Select

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[" & DB_COIL_INF & "] �f�[�^�擾���s")
        Finally
            tcls_DB.Cancel()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub


    ''' <summary>
    ''' DB�f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SqlDsp()

        Try
            If Not m_SqlGetRow(1) Is Nothing Then

                ' dgv�\������
                For ii As Integer = 0 To m_SqlGetRow(1).ItemArray.Length - 1
                    dgvCoil(1, ii).Value = m_SqlGetRow(1).ItemArray(ii)
                Next ii

            End If

            ' ���R�C���A�O�R�C���`�F�b�N
            Call CoilCheck()

            If BtnCSV.Visible = True Then
                BtnCSV.Enabled = True
            End If


        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�f�[�^�\�����s")

        End Try

    End Sub

    ''' <summary>
    ''' ���R�C���A�O�R�C���L���`�F�b�N
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilCheck()

        ' ���R�C�������邩���f
        If m_SqlGetRow(0) Is Nothing Then
            BtnCoilNext.Enabled = False
        Else
            BtnCoilNext.Enabled = True
        End If

        ' �O�R�C�������邩���f
        If m_SqlGetRow(2) Is Nothing Then
            BtnCoilBack.Enabled = False
        Else
            BtnCoilBack.Enabled = True
        End If

    End Sub

    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Both                                       ' �����X�N���[���o�[�̕\��
            .ColumnHeadersVisible = False                                       ' ��w�b�_�\��
            .AllowUserToResizeColumns = True                                    ' ���[�U�[ ��T�C�Y�ύX

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

            .DefaultCellStyle().ForeColor = Color.Black

            ' �t�H�[���̃T�C�Y�ɍ��킹�ĉ������ϓ��Ɋ���t���鏈��
            'dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill
            dgvCoil.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None

            '' ���`
            .ColumnCount = EM_GRID_COL_ID.em_Cnt        'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '��0 (�A�h���X) ����
            .Columns(EM_GRID_COL_ID.em_txtName).HeaderText = "����"
            .Columns(EM_GRID_COL_ID.em_txtName).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtName).Width = 240
            .Columns(EM_GRID_COL_ID.em_txtName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtName).SortMode = DataGridViewColumnSortMode.NotSortable

            '��1 (�A�h���X) �f�[�^
            .Columns(EM_GRID_COL_ID.em_txtData).HeaderText = "�f�[�^"
            .Columns(EM_GRID_COL_ID.em_txtData).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtData).Width = 220
            .Columns(EM_GRID_COL_ID.em_txtData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtData).SortMode = DataGridViewColumnSortMode.NotSortable

            '��2 (�A�h���X) ����
            .Columns(EM_GRID_COL_ID.em_txtInfo).HeaderText = "����"
            .Columns(EM_GRID_COL_ID.em_txtInfo).ReadOnly = True
            .Columns(EM_GRID_COL_ID.em_txtInfo).Width = 400
            .Columns(EM_GRID_COL_ID.em_txtInfo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtInfo).SortMode = DataGridViewColumnSortMode.NotSortable

        End With

    End Sub

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="cKizukenNo"></param>
    ''' <remarks></remarks>
    Public Sub New(Optional ByVal cKizukenNo As String = Nothing)

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        m_cKizukenNo = cKizukenNo       ' �Ǘ�No
        If m_cKizukenNo Is Nothing Then
            BtnCSV.Visible = False
        End If

        m_dctItem = New Dictionary(Of String, String)
        m_dctItem.Add("�R�C����", "[m]")
        m_dctItem.Add("�ޗ���", "[��m]")
        m_dctItem.Add("�ޗ���", "[mm]")
        m_dctItem.Add("������", "[��m]")
        m_dctItem.Add("������", "[mm]")
        m_dctItem.Add("���H���g������", "[mm]")
        m_dctItem.Add("SPM�L�ї�", "[10-2%]")
        m_dctItem.Add("TLV�L�ї�", "[10-2%]")
        m_dctItem.Add("�����t�����g�[����", "[mm]")
        m_dctItem.Add("�����e�[���[����", "[mm]")
        m_dctItem.Add("SPM���[�N���[���a��", "[10-1mm]")
        m_dctItem.Add("SPM���[�N���[���a��", "[10-1mm]")
        m_dctItem.Add("�N�����[�g����_���", "���FE�A���FC�A�����F��")
        m_dctItem.Add("�N�����[�g����_�L��", "�L��F0�A�����FX")
        m_dctItem.Add("�i��", "�H���L��1���ځiH,Z,G,A,B,S,U,Q�j")
        m_dctItem.Add("���b�L���", "�H���L��2���ځi-,X,Y,A,B,C�j")
        m_dctItem.Add("��ގ�", "�H���L��3,4���ځi1*,2*,3*,4*,5*,6*,7*�j")
        m_dctItem.Add("�f�B�t�@�����V�����}�[�N", "�H���L��5���ځi-,Z,A�`Y�j")
        m_dctItem.Add("���敪", "�H���L��6���ځiL,H,C�j")
        m_dctItem.Add("���b�L�T�C�N��", "�H���L��7���ځiF,A,M,N,P,S,T,H�j")
        m_dctItem.Add("���x���[_�C�����C��SPM", "�H���L��8���ځi0�`6,A,B,C,D,E,F,M,X�j")
        m_dctItem.Add("���w�����h��", "�H���L��9���ځiK,W,M,S,Y,N,E,C,U,R,X�j")
        m_dctItem.Add("�I�t���C��SPM������", "�H���L��10���ځiF,2,4,6,8,0,D,L,P,T,B,A,H,E,J,K�j")
        m_dctItem.Add("���b�L�敪", "�H���L��11���ځi1,2,3�j")
        m_dctItem.Add("�\�ʏ���1����", "�H���L��12���ځiL�����FL,�������FM,��ҰāFC�j")
        m_dctItem.Add("�\�ʏ���2����", "�H���L��13����")
        m_dctItem.Add("�h��", "�H���L��14����")
        m_dctItem.Add("�H���L���\��", "�H���L��15����")
        m_dctItem.Add("�\���R�C�����", "�H���L��16����")
        m_dctItem.Add("SPM�����L��", "�L��F0�A�����FX")
        m_dctItem.Add("�i��敪", "E�FEG�i�v�掟�H��=87EGL�j�AM:�┖�iE�ȊO�j")
        m_dctItem.Add("GA_CR�敪", "G�FGA�ށAC�FCR��")
        m_dctItem.Add("SPM�������їL��", "�L��F0�A�����FX")

    End Sub

    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
    ''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
    ''' <param name="bDelComma">�J���}������</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1�s�̃f�[�^
        Dim strWk() As String
        Dim strWkData As String

        '' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '' �t�H���_�̗L���`�F�b�N(�����������낤)
        tMod.CheckFolder(strFolder)

        '' CSV�o��
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' �I�[�v��
                cls_Stram.Open()

                '�w�b�_���o��
                strData = ""
                For ii As Integer = 0 To dgvCoil.ColumnCount - 1

                    '���s�R�[�h�����ׂĎ�菜��
                    strWk = dgvCoil.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
                    strWkData = ""
                    For jj As Integer = 0 To strWk.Length - 1
                        strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
                    Next jj

                    strData &= Trim(strWkData) & ","
                Next ii

                '�Ō�̃^�u��r��
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)

                '�f�[�^�o��
                For ii As Integer = 0 To dgvCoil.Rows.Count - 1
                    strData = ""
                    For jj As Integer = 0 To dgvCoil.ColumnCount - 1
                        '���ڂ�Tab�ŘA������
                        If dgvCoil(jj, ii).Value IsNot Nothing Then
                            If True = bDelComma Then
                                strData &= dgvCoil(jj, ii).Value.ToString.Replace(",", "")
                            Else
                                strData &= dgvCoil(jj, ii).Value.ToString
                            End If
                        End If
                        strData &= ","
                    Next jj

                    '�Ō�̃^�u��r��
                    strData = strData.Remove(strData.Length - 1, 1)

                    cls_Stram.WriteLine(strData)
                Next ii

                ' �N���[�Y
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

            Catch ex As Exception
                strMsg = "CSV�o�͂Ɏ��s���܂����B"
            End Try
        End Using

        '''' �K�C�_���X�\��
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using
    End Sub

End Class
