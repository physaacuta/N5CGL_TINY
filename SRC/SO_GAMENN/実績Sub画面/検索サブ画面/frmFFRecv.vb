Option Strict Off
Imports tClassLibrary


Public Class frmFFRecv

    Private Const MAX_KEN_COL As Integer = 5
    ' 0.1m�� �Ή� --->>>
    Private Const MAX_DEFECT_COL As Integer = 11           '�f�[�^��MAX
    Private Const MAX_COIL_ROW As Integer = 21             '�R�C�����sMAX
    Private Const MAX_TUHAN_COL As Integer = 6
    Private Const MAX_RCL_ROW As Integer = 5               '
    Private Const MAX_BUNKATU_COL As Integer = 6
    ' 0.1m�� �Ή� <<<---

    '�O�H���r���
    Private Structure FF_MAE_DEFECT_INF
        Dim SetNo As String             'FF�O�H���r���i�[�ʒu(�c�[���p�̏��)
        Dim Kind As String              '�r��
        Dim Grade As String             '�r�O���[�h
        Dim Kizukbn As String           '�r���o�H��
        Dim kbn As String               '�r�敪
        Dim DefectPos As String         '�r���������ʒu[0.1m]
        Dim WidPos As String            '�r�Е����ʒu
        Dim Len As String               '�r����[0.1m]
        Dim No As String                '�rNO
        Dim SysKbn As String            '�V�X�e���敪
        Dim Filler As String            '�\��
    End Structure

    '�R�C�����O���b�h�ݒ�
    Private Enum EM_COIL_COL_ID
        iTiTle = 0      '�R�C�����
        iData           '�f�[�^
    End Enum

    Private Enum EM_COIL_ROW_ID
        iReiCoilNo = 0     '// �≄�R�C���ԍ�
        iReiCoilG          '// �≄�R�C���d��
        iReiCoilL          '// �≄�R�C������
        iReiCoilAtu        '// �≄��
        iItaatuMin         '// ���@�����@MIN
        iItaatuMax         '// ���@�����@MAX
        iMukesaki1i        '// ������P�@��
        iKensa             '// �����
        iMaki              '// ��������
        iZairyou           '// �ޗ���
        iMukesaki1kMin     '// ������P�@�Ќ����@MIN
        iMukesaki1kMax     '// ������P�@�Ќ����@MAX
        ' 0.1m�� �Ή� --->>>
        'iSplit             '// �X�v���b�g�F��
        ' 0.1m�� �Ή� <<<---
        iMukesaki1L        '// ������P�@����(���b�h����)
        iMukesaki1N        '// ������P�@���a
        iItaatu            '// ��
        iIrigawa           '// �������a
        iMukesaki1KL       '// ������P�@���f��
        iMakoutei          '// �O�H���R�[�h
        iRealMakoutei      '// ���O�H���R�[�h
        iTCMFlag           '// ����/TCM�����ރt���O
        'iYobi             '// �\��
    End Enum

    ' 0.1m�� �Ή� --->>>
    Private Enum EM_RCL_ROW_ID
        iKizukenNo = 0      '���zRCL�r���Ǘ�No
        iCoilNo             '�≄�R�C��No
        iFromSai            '�̎�J�n�ʒu�����iFrom�j [0.1m]
        iToSai              '�̎�I���ʒu�����iTo�j	[0.1m]
        iKoban              '�̎�q��
    End Enum
    ' 0.1m�� �Ή� <<<---

    ' ���o���O���b�h�ݒ�
    Private Enum EM_KEN_COL_ID
        iProc = 0           '���o�H��
        iKizuKenNo          '�r���Ǘ�No
        iMode               '�r���������[�h
        iCnt                '�r�ݒ��
        iStatus             '�r���I�[�o�[�X�e�[�^�X
    End Enum

    ' �r���O���b�h�ݒ�
    Private Enum EM_DEFECT_COL_ID
        iNO = 0             'NO
        iTypeID             '�r��
        iGradeID            '�r�O���[�h
        iKizuOut            '���o�敪
        iKizuDiv            '�r�敪
        iTotalLen           '�r�����������ʒu
        iHaba               '�r�Е����ʒu
        iLen                '�r����
        iKizuNo             '�rNo
        iSysKbn             '�V�X�e���敪
        iSetNO              '�i�[�ʒu
        iFiller             '�\��
    End Enum

    '�ʔ���
    Private Enum EM_TUHAN_COL_ID
        iJissekiCode = 0    '���эH���R�[�h
        iMeireiNo           '���ߔԍ�
        iThuhanCnt          '�ʔ�
        iGenpinNo           '�������i�Ǘ�NO
        iCoilLen            '�����R�C������
        ' 0.1m�� �Ή� --->>>
        iFiller             '�\��
        ' 0.1m�� �Ή� <<<---
    End Enum

    '�������
    Private Enum EM_BUNKATU_COL_ID
        iNo = 0
        iMakiHoukou         '���ъ�����(1:��o�㌞, 2:��o����, 3:���o�㌞, 4:���o����)
        iCoilLen            '�R�C����
        iCutkbn             '�J�b�g�ꏊ�敪(I:�����J�b�g, O:�o���J�b�g)
        iGenpinNo           '�������i�Ǘ�NO
        ' 0.1m�� �Ή� --->>>
        iFiller             '�\��
        ' 0.1m�� �Ή� <<<---
    End Enum

    ' 0.1m�� �Ή� --->>>
    ' ���zRCL�r���Ǘ�No
    Private Enum EM_RCL_COL_ID
        iKizukenNo = 0      '���zRCL�r���Ǘ�No
        iCoilNo             '�r���Ǘ�No
        iFromSai            '�≄�R�C��No
        iToSai              '�̎�J�n�ʒu�����iFrom�j [0.1m]
        iKoban              '�̎�I���ʒu�����iTo�j	[0.1m]
    End Enum
    ' 0.1m�� �Ή� <<<---

    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�r���Ǘ�No

    Private m_typData_O As FF_MAE_JISSEKI = Nothing     '�O�H������_�\
    Private m_typData_U As FF_MAE_JISSEKI = Nothing     '�O�H������_��

    ''' <summary>
    ''' �p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
    ''' </summary>
    ''' <param name="mode">1:�I�����C���p�A0:�c�[���p</param>
    ''' <param name="strKizukenNo">�Ǘ�No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        '�����o�ϐ��Z�b�g
        m_mode = mode                   '�p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
        m_strKizukenNo = strKizukenNo   '�Ǘ�No

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' ��ʏI��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '��ʏI��
        Me.Close()

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmFFRecv_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Dim ii As Integer

        Me.tProp_Moveable = True

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        ''��ʏ�����
        Call InitDsp()

        '�H�����
        cmbKindO.Items.Add("CC�r���")
        cmbKindO.Items.Add("HOT�r���")
        cmbKindO.Items.Add("PL�r���")
        cmbKindO.Items.Add("TCM�����")
        ' 0.1m�� �Ή� --->>>
        'cmbKindO.Items.Add("TCM�r���")
        cmbKindO.Items.Add("�����r��� CAPL")
        ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
        'cmbKindO.Items.Add("�����r��� ���� 2FLL���~�O")
        cmbKindO.Items.Add("TCM�r���")
        ' 20221021 2TCM���@���ɔ������H������ ���� <<<---
        cmbKindO.Items.Add("�����r��� ���� 2FLL���~��")
        'cmbKindO.Items.Add("�r���\��")   '2013.03.19 �쌴
        ' 0.1m�� �Ή� <<<---
        cmbKindO.SelectedIndex = 0

        cmbKindU.Items.Add("CC�r���")
        cmbKindU.Items.Add("HOT�r���")
        cmbKindU.Items.Add("PL�r���")
        cmbKindU.Items.Add("TCM�����")
        ' 0.1m�� �Ή� --->>>
        'cmbKindU.Items.Add("TCM�r���")
        cmbKindU.Items.Add("�����r��� CAPL")
        ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
        'cmbKindU.Items.Add("�����r��� ���� 2FLL���~�O")
        cmbKindU.Items.Add("TCM�r���")
        ' 20221021 2TCM���@���ɔ������H������ ���� <<<---
        cmbKindU.Items.Add("�����r��� ���� 2FLL���~��")
        'cmbKindU.Items.Add("�r���\��")   '2013.03.19 �쌴
        ' 0.1m�� �Ή� <<<---
        cmbKindU.SelectedIndex = 0

        For ii = 0 To MAX_FF_MAE_THAN_CNT - 1
            cmbTuhanO.Items.Add(CStr(ii + 1))
            cmbTuhanU.Items.Add(CStr(ii + 1))
        Next

        cmbTuhanO.SelectedIndex = 0
        cmbTuhanU.SelectedIndex = 0

        'ID���X�g�쐬
        Call MakeIDList()

        If 1 = m_mode Then                  ' �I�����C�����[�h
            btnRead.Visible = False         ' �Ǎ��{�^���u��\���v
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' �c�[�����[�h
            btnRead.Visible = True          ' �Ǎ��{�^���u�\���v
        End If

    End Sub

    ''' <summary>
    ''' ID���X�g�쐬
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        strSQL = ""

        If m_mode = 1 Then           '�I�����C�����[�h
            ' �I�����C�����[�h�̏ꍇ�́A�Ǘ�No.���猟������
            strSQL &= " SELECT A.ID"
            strSQL &= " FROM " & DB_BUF_FF_R_MAEINF & " as A"
            strSQL &= " INNER JOIN " & DB_COIL_INF & " as B"
            strSQL &= " ON A.�L�[��� = B.�L�[���"
            strSQL &= " AND B.�Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY A.�o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 200 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
            strSQL &= " ORDER BY �o�^���t DESC"
        End If

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' �f�[�^�擾
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' ��ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '�H���R�[�h
        lblProcO.Text = ""

        '����No
        lblMeiNoO.Text = ""

        '�H���R�[�h
        lblProcU.Text = ""

        '����No
        lblMeiNoU.Text = ""

        '�R�C�����O���b�h
        Call dgvCoilInitialize(dgvCoilO)
        Call dgvCoilInitialize(dgvCoilU)

        '�r���o�O���b�h������
        Call dgvKenInitialize(dgvKenO)
        Call dgvKenInitialize(dgvKenU)

        '�r���O���b�h������
        Call dgvDefectInitialize(dgvDefectO)
        Call dgvDefectInitialize(dgvDefectU)

        ' 0.1m�� �Ή� --->>>
        Call dgvRclInitialize(dgvRclO)
        Call dgvRclInitialize(dgvRclU)
        ' 0.1m�� �Ή� <<<---

        '�ʔ���
        Call dgvTuhanInitialize(dgvJisO)
        Call dgvTuhanInitialize(dgvJisU)

        '����
        Call dgvBunkatuInitialize(dgvBunKatuO)
        Call dgvBunkatuInitialize(dgvBunKatuU)


    End Sub


    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData1() As Byte  '���`���o�C�i���f�[�^
        Dim bytData2() As Byte  '���`���o�C�i���f�[�^


        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT ���`��1,"
        strSQL &= "        ���`��2 "
        strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY �o�^���t DESC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' �f�[�^�擾
            sqlRead.Read()

            '���`��
            bytData1 = CType(sqlRead.GetValue(0), Byte())

            '���`��
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            tMod.MemSet(bytData1, 0, m_typData_O, tMod.SizeOf(m_typData_O))
            tMod.MemSet(bytData2, 0, m_typData_U, tMod.SizeOf(m_typData_U))

        Catch ex As Exception
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function

    ''' <summary>
    ''' ID���X�g�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click2(sender, e)
    End Sub

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        Dim cmbIDText As String

        cmbIDText = cmbID.Text

        'ID���X�g�쐬
        Call MakeIDList()

        cmbID.Text = cmbIDText

        Call btnRead_Click2(sender, e)

    End Sub

    Private Sub btnRead_Click2(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        '�f�[�^�擾
        If GetData(cmbID.Text) Then

            '�w�b�_�[���
            Call DspHeader()

            '�R�C�����
            Call SetCoilData(m_typData_O.Hd, dgvCoilO)
            Call SetCoilData(m_typData_U.Hd, dgvCoilU)

            ' 0.1m�� �Ή� --->>>
            Call SetRclData(m_typData_O.VRcl, m_typData_O.FromTo, dgvRclO)
            Call SetRclData(m_typData_U.VRcl, m_typData_U.FromTo, dgvRclU)
            ' 0.1m�� �Ή� <<<---

            If -1 <> cmbKindO.SelectedIndex Then
                Call SetKenData(cmbKindO.SelectedIndex, 0, dgvKenO)
                Call SetDefectData(cmbKindO.SelectedIndex, 0, dgvDefectO)
            End If
            If -1 <> cmbKindU.SelectedIndex Then
                Call SetKenData(cmbKindU.SelectedIndex, 1, dgvKenU)
                ' 0.1m�� �Ή� --->>>
                Call SetDefectData(cmbKindU.SelectedIndex, 1, dgvDefectU)
                ' 0.1m�� �Ή� <<<---
            End If

            If -1 <> cmbTuhanO.SelectedIndex Then
                Call SetJisData(cmbTuhanO.SelectedIndex, dgvJisO, m_typData_O, dgvBunKatuO)
            End If

            If -1 <> cmbTuhanU.SelectedIndex Then
                Call SetJisData(cmbTuhanU.SelectedIndex, dgvJisU, m_typData_U, dgvBunKatuU)
            End If

        End If
    End Sub

    '�w�b�_�[���\��
    Private Sub DspHeader()

        '�H���R�[�h
        lblProcO.Text = tMod.ByteToString(m_typData_O.Hd.Line_Code)

        '����No
        lblMeiNoO.Text = tMod.ByteToString(m_typData_O.Hd.Meino)

        '�H���R�[�h
        lblProcU.Text = tMod.ByteToString(m_typData_U.Hd.Line_Code)

        '����No
        lblMeiNoU.Text = tMod.ByteToString(m_typData_U.Hd.Meino)

    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvDefectInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_FF_DFCT_CNT

            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing         '

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = MAX_DEFECT_COL           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' ��w�b�_�T�C�Y

            '��0 (�A�h���X) �r���o�敪
            .Columns(EM_DEFECT_COL_ID.iNO).HeaderText = "NO"
            .Columns(EM_DEFECT_COL_ID.iNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iNO).Width = 40
            .Columns(EM_DEFECT_COL_ID.iNO).SortMode = DataGridViewColumnSortMode.NotSortable

            '��1 (�A�h���X) �r��
            .Columns(EM_DEFECT_COL_ID.iTypeID).HeaderText = "�r��"
            .Columns(EM_DEFECT_COL_ID.iTypeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iTypeID).Width = 50
            .Columns(EM_DEFECT_COL_ID.iTypeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '��2 (�A�h���X) �r�O���[�h
            .Columns(EM_DEFECT_COL_ID.iGradeID).HeaderText = "�r" & vbNewLine & "��ڰ��"
            .Columns(EM_DEFECT_COL_ID.iGradeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iGradeID).Width = 60
            .Columns(EM_DEFECT_COL_ID.iGradeID).SortMode = DataGridViewColumnSortMode.NotSortable

            '��3 (�A�h���X) �r���o�H��
            .Columns(EM_DEFECT_COL_ID.iKizuOut).HeaderText = "�r���o" & vbNewLine & "�H��"
            .Columns(EM_DEFECT_COL_ID.iKizuOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuOut).Width = 60
            .Columns(EM_DEFECT_COL_ID.iKizuOut).SortMode = DataGridViewColumnSortMode.NotSortable


            '��4(�A�h���X) �r�敪
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).HeaderText = "�r�敪"
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).Width = 60
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).SortMode = DataGridViewColumnSortMode.NotSortable


            '��5 (�A�h���X) �r�����������ʒu
            .Columns(EM_DEFECT_COL_ID.iTotalLen).HeaderText = "����" & vbNewLine & "�ʒu"
            .Columns(EM_DEFECT_COL_ID.iTotalLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iTotalLen).Width = 60
            .Columns(EM_DEFECT_COL_ID.iTotalLen).SortMode = DataGridViewColumnSortMode.NotSortable

            '��6 (�A�h���X) �r�Е����ʒu
            .Columns(EM_DEFECT_COL_ID.iHaba).HeaderText = "��" & vbNewLine & "�ʒu"
            .Columns(EM_DEFECT_COL_ID.iHaba).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iHaba).Width = 60
            .Columns(EM_DEFECT_COL_ID.iHaba).SortMode = DataGridViewColumnSortMode.NotSortable


            '��7 (�A�h���X) �r����
            .Columns(EM_DEFECT_COL_ID.iLen).HeaderText = "�r" & vbNewLine & "����"
            .Columns(EM_DEFECT_COL_ID.iLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iLen).Width = 50
            .Columns(EM_DEFECT_COL_ID.iLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '��8 (�A�h���X) �rNo
            .Columns(EM_DEFECT_COL_ID.iKizuNo).HeaderText = "�rNO"
            .Columns(EM_DEFECT_COL_ID.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuNo).Width = 90
            .Columns(EM_DEFECT_COL_ID.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable

            '��9 (�A�h���X) �rNo
            .Columns(EM_DEFECT_COL_ID.iSysKbn).HeaderText = "����" & vbNewLine & "�敪"
            .Columns(EM_DEFECT_COL_ID.iSysKbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iSysKbn).Width = 50
            .Columns(EM_DEFECT_COL_ID.iSysKbn).SortMode = DataGridViewColumnSortMode.NotSortable

            '��10 (�A�h���X) �i�[�ʒu
            .Columns(EM_DEFECT_COL_ID.iSetNO).HeaderText = "�i�[" & vbNewLine & "�ʒu"
            .Columns(EM_DEFECT_COL_ID.iSetNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iSetNO).Width = 60
            .Columns(EM_DEFECT_COL_ID.iSetNO).SortMode = DataGridViewColumnSortMode.NotSortable

            ''��11(�A�h���X) 
            '.Columns(EM_DEFECT_COL_ID.iFiller).HeaderText = "�\��"
            '.Columns(EM_DEFECT_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            '.Columns(EM_DEFECT_COL_ID.iFiller).Width = 60
            '.Columns(EM_DEFECT_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvKenInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv


            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 1

            '' ���`
            .ColumnCount = MAX_KEN_COL           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' ��w�b�_�T�C�Y

            '��0 (�A�h���X) �r���o�H��
            .Columns(EM_KEN_COL_ID.iProc).HeaderText = "�r���o" & vbNewLine & "�H��"
            .Columns(EM_KEN_COL_ID.iProc).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iProc).Width = 60
            .Columns(EM_KEN_COL_ID.iProc).SortMode = DataGridViewColumnSortMode.NotSortable


            '��P (�A�h���X) �r���Ǘ�NO
            .Columns(EM_KEN_COL_ID.iKizuKenNo).HeaderText = "�r���Ǘ�NO"
            .Columns(EM_KEN_COL_ID.iKizuKenNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iKizuKenNo).Width = 150
            .Columns(EM_KEN_COL_ID.iKizuKenNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '��Q (�A�h���X) �r���������[�h
            .Columns(EM_KEN_COL_ID.iMode).HeaderText = "�r������Ӱ��"
            .Columns(EM_KEN_COL_ID.iMode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iMode).Width = 80
            .Columns(EM_KEN_COL_ID.iMode).SortMode = DataGridViewColumnSortMode.NotSortable


            '��R (�A�h���X) �r�ݒ��
            .Columns(EM_KEN_COL_ID.iCnt).HeaderText = "�r�ݒ�" & vbNewLine & "��"
            .Columns(EM_KEN_COL_ID.iCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iCnt).Width = 60
            .Columns(EM_KEN_COL_ID.iCnt).SortMode = DataGridViewColumnSortMode.NotSortable


            '��S (�A�h���X) �r���I�[�o�[�X�e�[�^�X
            .Columns(EM_KEN_COL_ID.iStatus).HeaderText = "�r�����ް�ð��"
            .Columns(EM_KEN_COL_ID.iStatus).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iStatus).Width = 101
            .Columns(EM_KEN_COL_ID.iStatus).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub

    ''' <summary>
    ''' �r���o�H��
    ''' </summary>
    ''' <param name="nIndex"></param>
    ''' <param name="nOuDiv"></param>
    ''' <param name="dgvData"></param>
    ''' <remarks></remarks>
    Private Sub SetKenData(ByVal nIndex As Integer, ByVal nOuDiv As Integer, ByVal dgvData As tCnt.tCnt_DataGridView)

        Dim typData As FF_MAE_TYPE_INF

        If 0 = nOuDiv Then
            typData = m_typData_O.Inf(nIndex)
        Else
            typData = m_typData_U.Inf(nIndex)
        End If

        dgvData(EM_KEN_COL_ID.iProc, 0).Value = tMod.ByteToString(typData.Kizukbn)
        dgvData(EM_KEN_COL_ID.iKizuKenNo, 0).Value = tMod.ByteToString(typData.Id)
        dgvData(EM_KEN_COL_ID.iMode, 0).Value = tMod.ByteToString(typData.Mode)
        dgvData(EM_KEN_COL_ID.iCnt, 0).Value = tMod.ByteToString(typData.DataDefault)
        dgvData(EM_KEN_COL_ID.iStatus, 0).Value = tMod.ByteToString(typData.OverStatus)

    End Sub

    Private Sub cmbKindO_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbKindU.SelectedIndexChanged, cmbKindO.SelectedIndexChanged

        If m_typData_O.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbKindO.SelectedIndex Then
            Call SetKenData(cmbKindO.SelectedIndex, 0, dgvKenO)
            Call SetDefectData(cmbKindO.SelectedIndex, 0, dgvDefectO)
        End If

    End Sub

    Private Sub cmbKindU_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbKindU.SelectedIndexChanged, cmbKindO.SelectedIndexChanged

        If m_typData_U.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbKindU.SelectedIndex Then
            Call SetKenData(cmbKindU.SelectedIndex, 1, dgvKenU)
            Call SetDefectData(cmbKindU.SelectedIndex, 1, dgvDefectU)
        End If

    End Sub

    Private Sub SetDefectData(ByVal nIndex As Integer, ByVal nOuDiv As Integer, ByVal dgvData As tCnt.tCnt_DataGridView)

        Dim ii As Integer

        Dim strInfProc As String
        Dim strDefProc As String

        Dim typInf As FF_MAE_TYPE_INF
        Dim typData As FF_MAE_TYPE_DATA

        Dim typDefInf As FF_MAE_DEFECT_INF
        Dim lstDefInf As New List(Of FF_MAE_DEFECT_INF)

        '������
        dgvData.RowCount = 0
        lstDefInf.Clear()

        '�r���o�H�����擾
        If nOuDiv = 0 Then  '�\
            typInf = m_typData_O.Inf(nIndex)
        Else                '��
            typInf = m_typData_U.Inf(nIndex)
        End If
        strInfProc = tMod.ByteToString(typInf.Kizukbn)  '�r���o�H��

        If strInfProc = "" Or strInfProc = "  " Then
            Exit Sub
        End If

        '�r��񌟍�
        For ii = 0 To MAX_FF_MAE_DEF_CNT - 1
            '�r���擾
            If nOuDiv = 0 Then  '�\
                typData = m_typData_O.def(ii)
            Else                '��
                typData = m_typData_U.def(ii)
            End If
            strDefProc = tMod.ByteToString(typData.Kizukbn) '�r���o�H��
            If strInfProc = strDefProc Then '�H����v
                ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                If 4 - 1 = nIndex Then
                    If ii >= (MAX_FF_MAE_CC_CNT + MAX_FF_MAE_HOT_CNT + MAX_FF_MAE_PL_CNT + MAX_FF_MAE_TCM_ANA_CNT + MAX_FF_MAE_YOBI_CNT + MAX_FF_MAE_ATU_CNT) Then
                        Continue For
                    End If

                ElseIf 6 - 1 = nIndex Then
                    If ii < (MAX_FF_MAE_CC_CNT + MAX_FF_MAE_HOT_CNT + MAX_FF_MAE_PL_CNT + MAX_FF_MAE_TCM_ANA_CNT + MAX_FF_MAE_YOBI_CNT + MAX_FF_MAE_ATU_CNT) Then
                        Continue For
                    End If
                End If
                ' 20221021 2TCM���@���ɔ������H������ ���� <<<---
                typDefInf.Kind = tMod.ByteToString(typData.Kind)            '�r��
                typDefInf.Grade = tMod.ByteToString(typData.Grade)          '�r�O���[�h
                    typDefInf.Kizukbn = tMod.ByteToString(typData.Kizukbn)      '�r���o�H��
                    typDefInf.kbn = tMod.ByteToString(typData.kbn)              '�r�敪
                    typDefInf.DefectPos = tMod.ByteToString(typData.DefectPos)  '�r���������ʒu[0.1m]
                    typDefInf.WidPos = tMod.ByteToString(typData.WidPos)        '�r�Е����ʒu
                    typDefInf.Len = tMod.ByteToString(typData.Len)              '�r����[0.1m]

                    Dim sNoHex As String = tMod.ByteToString(typData.No)
                    Dim nNo As Integer
                    Dim sNo As String = sNoHex

                    If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
                        sNo &= String.Format("({0,5})", nNo)
                    End If

                    typDefInf.No = sNo

                    typDefInf.SysKbn = tMod.ByteToString(typData.SysKbn)        '�V�X�e���敪
                    typDefInf.Filler = tMod.ByteToString(typData.Filler)        '�\��
                    lstDefInf.Add(typDefInf)
                End If
        Next

        '�r���\��
        dgvData.RowCount = lstDefInf.Count
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_DEFECT_COL_ID.iNO, ii).Value = CStr(ii + 1)
            dgvData(EM_DEFECT_COL_ID.iTypeID, ii).Value = lstDefInf(ii).Kind
            dgvData(EM_DEFECT_COL_ID.iGradeID, ii).Value = lstDefInf(ii).Grade
            dgvData(EM_DEFECT_COL_ID.iKizuOut, ii).Value = lstDefInf(ii).Kizukbn
            dgvData(EM_DEFECT_COL_ID.iKizuDiv, ii).Value = lstDefInf(ii).kbn
            dgvData(EM_DEFECT_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(lstDefInf(ii).DefectPos), lstDefInf(ii).DefectPos.Trim, "0")) / 10, "0.0")
            dgvData(EM_DEFECT_COL_ID.iHaba, ii).Value = lstDefInf(ii).WidPos
            dgvData(EM_DEFECT_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(lstDefInf(ii).Len), lstDefInf(ii).Len.Trim, "0")) / 10, "0.0")
            dgvData(EM_DEFECT_COL_ID.iKizuNo, ii).Value = lstDefInf(ii).No
            dgvData(EM_DEFECT_COL_ID.iSysKbn, ii).Value = lstDefInf(ii).SysKbn
            dgvData(EM_DEFECT_COL_ID.iSetNO, ii).Value = lstDefInf(ii).SetNo
            'dgvData(EM_DEFECT_COL_ID.iFiller, ii).Value = lstDefInf(ii).Filler
        Next ii

    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvCoilInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv


            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = False                                   ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' �I���Z���F��`
            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = 2           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' ��w�b�_�T�C�Y

            .RowCount = MAX_COIL_ROW
            .Columns(EM_COIL_COL_ID.iTiTle).Width = 140
            .Columns(EM_COIL_COL_ID.iData).Width = 125
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilNo).Value = "�≄�R�C���ԍ�"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilG).Value = "�≄�R�C���d��"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilL).Value = "�≄�R�C������"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilAtu).Value = "�≄��"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatuMin).Value = "������MIN"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatuMax).Value = "������MAX"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1i).Value = "������1��"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iKensa).Value = "�����"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMaki).Value = "��������"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iZairyou).Value = "�ޗ���"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1kMin).Value = "������1�Ќ����@MIN"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1kMax).Value = "������1�Ќ����@MAX"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1L).Value = "������1����(���b�h����)"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1N).Value = "������1���a"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatu).Value = "��"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iIrigawa).Value = "�������a"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1KL).Value = "������1���f��"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMakoutei).Value = "�O�H���R�[�h"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iRealMakoutei).Value = "���O�H���R�[�h"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iTCMFlag).Value = "����/TCM�����t���O"
            '.Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iYobi).Value = "�\��"

        End With
    End Sub


    ''' <summary>
    ''' �R�C�����Z�b�g
    ''' </summary>
    ''' <param name="m_typData">�f�[�^</param>
    ''' <param name="dgv">�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub SetCoilData(ByVal m_typData As FF_MAE_TYPE_HEAD, ByVal dgv As DataGridView)

        ' 0.1m�� �Ή� --->>>
        Dim Maki(2) As Byte

        Maki(0) = m_typData.Maki(0)
        ' 0.1m�� �Ή� <<<---

        With dgv
            ' 0.1m�� �Ή� --->>>
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilNo).Value = tMod.ByteToString(m_typData.ReiCoilNo)

            Dim wkReiCoilG As String = tMod.ByteToString(m_typData.ReiCoilG)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilG).Value = Format(CSng(IIf(IsNumeric(wkReiCoilG), wkReiCoilG.Trim, "0")) / 1000, "0.000")

            Dim wkReiLen As String = tMod.ByteToString(m_typData.ReiCoilL)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilL).Value = Format(CSng(IIf(IsNumeric(wkReiLen), wkReiLen.Trim, "0")) / 10, "0.0")

            Dim wkReiCoilAtu As String = tMod.ByteToString(m_typData.ReiCoilAtu)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilAtu).Value = Format(CSng(IIf(IsNumeric(wkReiCoilAtu), wkReiCoilAtu.Trim, "0")) / 1000, "0.000")

            Dim wkItaatuMin As String = tMod.ByteToString(m_typData.ItaatuMin)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatuMin).Value = Format(CSng(IIf(IsNumeric(wkItaatuMin), wkItaatuMin.Trim, "0")) / 1000, "0.000")

            Dim wkItaatuMax As String = tMod.ByteToString(m_typData.ItaatuMax)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatuMax).Value = Format(CSng(IIf(IsNumeric(wkItaatuMax), wkItaatuMax.Trim, "0")) / 1000, "0.000")

            Dim wkMukesaki1i As String = tMod.ByteToString(m_typData.Mukesaki1i)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1i).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1i), wkMukesaki1i.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iKensa).Value = tMod.ByteToString(m_typData.Kensa)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMaki).Value = tMod.ByteToString(Maki)

            Dim wkZairyou As String = tMod.ByteToString(m_typData.Zairyou)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iZairyou).Value = Format(CSng(IIf(IsNumeric(wkZairyou), wkZairyou.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1kMin As String = tMod.ByteToString(m_typData.Mukesaki1kMin)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1kMin).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1kMin), wkMukesaki1kMin.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1kMax As String = tMod.ByteToString(m_typData.Mukesaki1kMax)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1kMax).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1kMax), wkMukesaki1kMax.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1L).Value = tMod.ByteToString(m_typData.Mukesaki1L)

            Dim wkMukesaki1N As String = tMod.ByteToString(m_typData.Mukesaki1N)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1N).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1N), wkMukesaki1N.Trim, "0")) / 10, "0.0")

            Dim wkItaatu As String = tMod.ByteToString(m_typData.Itaatu)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatu).Value = Format(CSng(IIf(IsNumeric(wkItaatu), wkItaatu.Trim, "0")) / 1000, "0.000")

            Dim wkIrigawa As String = tMod.ByteToString(m_typData.Irigawa)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iIrigawa).Value = Format(CSng(IIf(IsNumeric(wkIrigawa), wkIrigawa.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1KL As String = tMod.ByteToString(m_typData.Mukesaki1KL)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1KL).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1KL), wkMukesaki1KL.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMakoutei).Value = tMod.ByteToString(m_typData.Makoutei)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iRealMakoutei).Value = tMod.ByteToString(m_typData.RealMakoutei)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iTCMFlag).Value = tMod.ByteToString(m_typData.TCMFlag)
            '.Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iYobi).Value = tMod.ByteToString(m_typData.Yobi)
            ' 0.1m�� �Ή� <<<---

        End With


    End Sub

    ' 0.1m�� �Ή� --->>>
    ''' <summary>
    ''' �R�C�����Z�b�g
    ''' </summary>
    ''' <param name="m_typData">�f�[�^</param>
    ''' <param name="dgv">�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub SetRclData(ByVal m_typData As FF_MAE_TYPE_VRCLINF, ByVal m_typData2 As FF_MAE_TYPE_FROMTO, ByVal dgv As DataGridView)

        With dgv
            ' 0.1m�� �Ή� --->>>
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iKizukenNo).Value = tMod.ByteToString(m_typData.KizukenNo)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iCoilNo).Value = tMod.ByteToString(m_typData2.CoilNo)

            Dim wkFromSai As String = tMod.ByteToString(m_typData2.From)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iFromSai).Value = Format(CSng(IIf(IsNumeric(wkFromSai), wkFromSai.Trim, "0")) / 10, "0.0")

            Dim wkToSai As String = tMod.ByteToString(m_typData2.To2)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iToSai).Value = Format(CSng(IIf(IsNumeric(wkToSai), wkToSai.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iKoban).Value = tMod.ByteToString(m_typData2.Koban)
            ' 0.1m�� �Ή� <<<---
        End With
    End Sub
    ' 0.1m�� �Ή� <<<---

    ' 0.1m�� �Ή� --->>>
    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvRclInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv

            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = False                                   ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = 2           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' ��w�b�_�T�C�Y

            .RowCount = MAX_RCL_ROW
            .Columns(EM_COIL_COL_ID.iTiTle).Width = 240
            .Columns(EM_COIL_COL_ID.iData).Width = 150
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iKizukenNo).Value = "���zRCL�r���Ǘ�No"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iCoilNo).Value = "�≄�R�C��No"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iFromSai).Value = "�̎�J�n�ʒu�����iFrom�j [0.1m]"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iToSai).Value = "�̎�I���ʒu�����iTo�j	[0.1m]"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iKoban).Value = "�̎�q��"
        End With
    End Sub
    ' 0.1m�� �Ή� <<<---

    Protected Friend Overridable Sub dgvTuhanInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv


            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = MAX_TUHAN_COL           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' ��w�b�_�T�C�Y

            '��0 (�A�h���X) ���эH���R�[�h
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).HeaderText = "���эH���R�[�h"
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).Width = 130
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).SortMode = DataGridViewColumnSortMode.NotSortable

            '��1 (�A�h���X) ���ߔԍ�
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).HeaderText = "���ߔԍ�"
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).Width = 100
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '��2 (�A�h���X) �ʔ�
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).HeaderText = "�ʔ�"
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).Width = 100
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).SortMode = DataGridViewColumnSortMode.NotSortable

            '��3 (�A�h���X) �������i�Ǘ�NO
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).HeaderText = "�������i�Ǘ�NO"
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).Width = 130
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '��4(�A�h���X) �����R�C������
            .Columns(EM_TUHAN_COL_ID.iCoilLen).HeaderText = "�����R�C������"
            .Columns(EM_TUHAN_COL_ID.iCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iCoilLen).Width = 130
            .Columns(EM_TUHAN_COL_ID.iCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable


            ' 0.1m�� �Ή� --->>>
            '��4(�A�h���X) �����R�C������
            .Columns(EM_TUHAN_COL_ID.iFiller).HeaderText = "�\��"
            .Columns(EM_TUHAN_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iFiller).Width = 130
            .Columns(EM_TUHAN_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable
            ' 0.1m�� �Ή� <<<---

        End With
    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvBunkatuInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv


            '' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = MAX_FF_MAE_BNKT_CNT

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = MAX_BUNKATU_COL          ' textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' ��w�b�_�T�C�Y

            '��0 (�A�h���X) NO
            .Columns(EM_BUNKATU_COL_ID.iNo).HeaderText = "NO"
            .Columns(EM_BUNKATU_COL_ID.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iNo).Width = 60
            .Columns(EM_BUNKATU_COL_ID.iNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '��1 (�A�h���X) ���ъ�����
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).HeaderText = "���ъ�����"
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).Width = 110
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).SortMode = DataGridViewColumnSortMode.NotSortable

            '��2 (�A�h���X) �R�C����
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).HeaderText = "�R�C����"
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).Width = 90
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '��3 (�A�h���X) �J�b�g�ꏊ�敪
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).HeaderText = "�J�b�g�ꏊ�敪"
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).Width = 120
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).SortMode = DataGridViewColumnSortMode.NotSortable

            '��4 (�A�h���X) �������i�Ǘ�NO
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).HeaderText = "�������i�Ǘ�NO"
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).Width = 130
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).SortMode = DataGridViewColumnSortMode.NotSortable

            ' 0.1m�� �Ή� --->>>
            '��5 �\��
            .Columns(EM_BUNKATU_COL_ID.iFiller).HeaderText = "�\��"
            .Columns(EM_BUNKATU_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iFiller).Width = 83
            .Columns(EM_BUNKATU_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable
            ' 0.1m�� �Ή� <<<---
        End With
    End Sub

    Private Sub SetJisData(ByVal nIndex As Integer, ByVal dgvData As tCnt.tCnt_DataGridView,
                            ByVal typFFData As FF_MAE_JISSEKI, ByVal dgvBukatu As tCnt.tCnt_DataGridView)

        Dim typData As FF_MAE_TYPE_THUHAN
        Dim ii As Integer

        typData = typFFData.Th(nIndex)

        dgvData(EM_TUHAN_COL_ID.iJissekiCode, 0).Value = tMod.ByteToString(typData.JissekiCode)
        dgvData(EM_TUHAN_COL_ID.iMeireiNo, 0).Value = tMod.ByteToString(typData.MeireiNo)
        dgvData(EM_TUHAN_COL_ID.iThuhanCnt, 0).Value = tMod.ByteToString(typData.ThuhanCnt)
        dgvData(EM_TUHAN_COL_ID.iGenpinNo, 0).Value = tMod.ByteToString(typData.GenpinNo)

        Dim wkCoilLen As String = tMod.ByteToString(typData.CoilLen)
        dgvData(EM_TUHAN_COL_ID.iCoilLen, 0).Value = Format(CSng(IIf(IsNumeric(wkCoilLen), wkCoilLen.Trim, "0")) / 10, "0.0")

        ' 0.1m�� �Ή� --->>>
        dgvData(EM_TUHAN_COL_ID.iFiller, 0).Value = tMod.ByteToString(typData.Filler)
        ' 0.1m�� �Ή� <<<---

        For ii = 0 To MAX_FF_MAE_BNKT_CNT - 1

            dgvBukatu(EM_BUNKATU_COL_ID.iNo, ii).Value = CStr(ii + 1)
            dgvBukatu(EM_BUNKATU_COL_ID.iMakiHoukou, ii).Value = tMod.ByteToString(typData.Bu(ii).MakiHoukou)

            Dim wkCoilTmp As String = tMod.ByteToString(typData.Bu(ii).CoilLen)
            dgvBukatu(EM_BUNKATU_COL_ID.iCoilLen, ii).Value = Format(CSng(IIf(IsNumeric(wkCoilTmp), wkCoilTmp.Trim, "0")) / 10, "0.0")

            dgvBukatu(EM_BUNKATU_COL_ID.iCutkbn, ii).Value = tMod.ByteToString(typData.Bu(ii).Cutkbn)
            dgvBukatu(EM_BUNKATU_COL_ID.iGenpinNo, ii).Value = tMod.ByteToString(typData.Bu(ii).GenpinNo)
            ' 0.1m�� �Ή� --->>>
            dgvBukatu(EM_BUNKATU_COL_ID.iFiller, ii).Value = tMod.ByteToString(typData.Bu(ii).Filler)
            ' 0.1m�� �Ή� <<<---

        Next ii

    End Sub

    Private Sub cmbTuhanO_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbTuhanU.SelectedIndexChanged, cmbTuhanO.SelectedIndexChanged

        If m_typData_O.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbTuhanO.SelectedIndex Then
            Call SetJisData(cmbTuhanO.SelectedIndex, dgvJisO, m_typData_O, dgvBunKatuO)
        End If

    End Sub

    Private Sub cmbTuhanU_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbTuhanU.SelectedIndexChanged, cmbTuhanO.SelectedIndexChanged

        If m_typData_U.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbTuhanU.SelectedIndex Then
            Call SetJisData(cmbTuhanU.SelectedIndex, dgvJisU, m_typData_U, dgvBunKatuU)
        End If

    End Sub

End Class