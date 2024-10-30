'*********************************************************************************
' ��RAS���
'	[Ver]
'		Ver.01    2012/03/12  
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tClass.tClass_SQLServer
Public Class frmRas
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    '���W�I�{�^���敪
    Private Enum EM_RAD_KUBUN
        nErr = 0      '�ُ�̂ݕ\��
        nDefult = 1   '����{�ُ�(�K�C�_���X�͏���)
    End Enum
    '// �f�[�^�O���b�h�萔
    Public Enum en_KikiList                 ' �k�ޏ�ԕ\��
        iNo = 0                             ' NO
        iKiKiName                           ' �@�햼��
        iIjouHani                           ' �ُ�͈�
        iKensa                              ' �������
        iKiki                               ' �@����
        iData                               ' ���l
        iBtn                                ' ���������{�^��
        iMsg                                ' �ُ탁�b�Z�[�W
        iBikou                              ' ���l
    End Enum
    '// �f�[�^�O���b�h�萔
    Public Enum en_GuideList                 ' �k�ޏ�ԕ\��
        emNo = 0                             ' NO
        emName                               ' �@�햼��
        emData                               ' ���l
        emMsg                                ' �ُ탁�b�Z�[�W
    End Enum
    '// �f�[�^�O���b�h�萔
    Public Enum en_PcList                 ' �k�ޏ�ԕ\��
        emNo = 0                           ' PCID
        emPCkind                           ' PC���
        emPCName                           ' PC����
        emPCName2                          ' PC��
        emOnOff                            ' �I�����C���^�I�t���C��
        emBtn1                             ' ���O�I�t
        emBtn2                             ' �ċN��
        emBtn3                             ' �V���b�g�_�E��
        emKiki                             ' �@����
    End Enum

    Private Enum EM_DGVBTN_TYPE
        emBtnNG = 0   '�{�^�������s��
        emBtnOK = 1   '�{�^�������\
    End Enum

    Private Structure TYP_SOGYO_STATUS
        Dim emKadou As EM_DIV_KADOU             '�ғ����
        Dim emUnten As EM_DIV_UNTEN             '�^�]���[�h
        Dim emSogyo As EM_DIV_SOGYO             '���Ə��
        Dim emPlg As EM_DIV_PLG                 'PLG�敪
    End Structure

    Private Enum EM_BTN_TYPE
        emBtn_LogOff = 0        '���O�I�t
        emBtn_Reset = 1         '�ċN��
        emBtn_Exit = 2          '�V���b�g�_�E��
    End Enum

    Private Structure TYP_LOV_ITEM_DATA
        Dim DispData As String
        Dim ItemNo As Integer
    End Structure

    Private Structure TYP_STATUS_INF
        Dim nId As Integer                  ' �@��ID
        Dim nStatus As Integer              ' �@����
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const ERR_CHECK_TIME As Integer = 500           '�G���[�������ݸ�����^�C�}�[

    Private m_frmSecurityDisp As frmSecurityDisp            ' �����������

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nRadKubun As EM_RAD_KUBUN            '���W�I�{�^���I��(0:�ُ�̂ݕ\��,1:����{�ُ�,2:�S��)
    Private m_strResetPass As String                                            ' PC���Z�b�g�p�X���[�h
    Private m_nLineID As Integer = LINE_ID                                      ' ���C��ID
    Private m_sMasterTask As String = KS_MASTER                                 ' PC�}�X�^�[�Ǘ��v���Z�X��
    Private m_sSubTask As String = KS_SLAVE                                     ' PC�X���[�u�Ǘ��v���Z�X��
    Private m_sMasterPC As String = KizuLibCLI.KizuMod.MasterPcName             ' PC�}�X�^�[�Ǘ����N������PC��
    Private m_lstLovItem As New List(Of TYP_LOV_ITEM_DATA)
    Private m_clOK As Color                      '���펞�̐F
    Private m_clErr As Color                     '�ُ펞�̐F
    Private m_clCare As Color                    '���ӎ��̐F
    Private m_clNormal As Color = Color.FromKnownColor(KnownColor.Control)  '�R���g���[���̔w�i�F
    Private m_clOnLineNG As Color                '�I�����C���@��ُ̈�F
    Private m_clOffLineNG As Color               '�I�t���C���@��ُ̈�F
    Private m_dctSecurity As New Dictionary(Of Integer, Control)
    Private m_lstStatus As New List(Of TYP_STATUS_INF)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        Dim s As Size
        s.Width = 1920
        s.Height = 1080
        Me.MaximumSize = s
        s.Width = 800
        s.Height = 400
        Me.MinimumSize = s

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub


    ''' <summary>
    ''' ��ʃ��[�h�� �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmRas_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        '�^�C�g���ݒ�
        modMain.SetTitleName(Me)

        Log(LNO.MSG, "RAS��ʋN��")
        LogEx(LNO.MSG, "RAS��ʋN��")

        '���u��
        If NOA_ONLINE <> g_NOA Then
            '�t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If
        ' ���u��
        If NOA_OFFLINE = g_NOA Then
            btnAllPc_ShutDown.Enabled = False
            btnAllPc_ReStart.Enabled = False
            btnAllPc_LogOff.Enabled = False
        End If

        ' ����������ʂ̃C���X�^���X����
        m_frmSecurityDisp = New frmSecurityDisp

        'RAS�\���F���擾
        GetColor()

        '�p�X���[�h���Z�b�g
        m_strResetPass = PASSWORD
        m_nLineID = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^�O���b�h �����ݒ�
        dgvPcInitialize(dgvPc)
        dgvKikiInitialize(dgvKiki)
        dgvGuideInitialize(dgvGuide)

        '�ُ�͈͕����擾
        Dim typ As TYP_LOV_ITEM_DATA
        m_lstLovItem.Clear()
        typ.DispData = "�S��"
        typ.ItemNo = 0
        m_lstLovItem.Add(typ)
        typ.DispData = "�\��"
        typ.ItemNo = 1
        m_lstLovItem.Add(typ)
        typ.DispData = "����"
        typ.ItemNo = 2
        m_lstLovItem.Add(typ)
        typ.DispData = "��ײ݂̂�"
        typ.ItemNo = -1
        m_lstLovItem.Add(typ)
        '�f�t�H���g�ُ���̂ݕ\��
        m_nRadKubun = EM_RAD_KUBUN.nErr
        SetButtonColor()

        m_dctSecurity.Clear()
        m_lstStatus.Clear()
        m_dctSecurity.Add(DIV_SECURITY_DOOR_ID, m_frmSecurityDisp.lblStat11)
        m_dctSecurity.Add(DIV_SECURITY_KEY_ID, m_frmSecurityDisp.lblStat12)

        '�^�C�}�[�����ݒ�
        timGetStatus.Enabled = True
        timGetStatus.Interval = tMod.tMod_IniFile.ReadIniInt(SO_GAMENN, "RAS_TIMER", TASKINI_NAME, 2000)

        timErrCheck.Enabled = True
        timErrCheck.Interval = ERR_CHECK_TIME

        '�����\��
        tmrDsp_Tick(Nothing, Nothing)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ PC�ꗗ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' PC�ꗗ �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvPcInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0

            '' �����`
            .ReadOnly = True                                             ' �ǂݎ���p
            .ScrollBars = ScrollBars.Both                                                           '�X�N���[���o�[�L��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ���[�U�[ ��T�C�Y�ύX

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1�s�����ɐF�ւ�

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC���
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �I�����C���^�I�t���C��
            .Columns.Add(New DataGridViewButtonColumn)    ' ���O�I�t(�{�^��)
            .Columns.Add(New DataGridViewButtonColumn)    ' �ċN��(�{�^��)
            .Columns.Add(New DataGridViewButtonColumn)    ' �V���b�g�_�E��(�{�^��)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@����

            '''' ���`
            '1���:PCID
            .Columns(en_PcList.emNo).HeaderText = "NO" & vbCrLf
            .Columns(en_PcList.emNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(en_PcList.emNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emNo).Width = 24
            .Columns(en_PcList.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emNo).Frozen = True


            '2���:PC���
            .Columns(en_PcList.emPCkind).HeaderText = "PC���" & vbCrLf
            .Columns(en_PcList.emPCkind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCkind).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCkind).Width = 166
            .Columns(en_PcList.emPCkind).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:PC����
            .Columns(en_PcList.emPCName).HeaderText = "PC����" & vbCrLf
            .Columns(en_PcList.emPCName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCName).Width = 160
            .Columns(en_PcList.emPCName).SortMode = DataGridViewColumnSortMode.NotSortable

            '4���:PC��(��\��)
            .Columns(en_PcList.emPCName2).HeaderText = "PC��" & vbCrLf
            .Columns(en_PcList.emPCName2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCName2).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCName2).Width = 0
            .Columns(en_PcList.emPCName2).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emPCName2).Visible = False

            '5���:�敪
            .Columns(en_PcList.emOnOff).HeaderText = "�敪" & vbCrLf
            .Columns(en_PcList.emOnOff).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emOnOff).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emOnOff).Width = 44
            .Columns(en_PcList.emOnOff).SortMode = DataGridViewColumnSortMode.NotSortable


            '6���:�{�^��
            .Columns(en_PcList.emBtn1).HeaderText = "۸޵�" & vbCrLf
            .Columns(en_PcList.emBtn1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn1).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn1).Width = 50
            .Columns(en_PcList.emBtn1).SortMode = DataGridViewColumnSortMode.NotSortable


            '7���:�{�^��
            .Columns(en_PcList.emBtn2).HeaderText = "�ċN��" & vbCrLf
            .Columns(en_PcList.emBtn2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn2).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn2).Width = 60
            .Columns(en_PcList.emBtn2).SortMode = DataGridViewColumnSortMode.NotSortable


            '8���:�{�^��
            .Columns(en_PcList.emBtn3).HeaderText = "�����޳�" & vbCrLf
            .Columns(en_PcList.emBtn3).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn3).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn3).Width = 70
            .Columns(en_PcList.emBtn3).SortMode = DataGridViewColumnSortMode.NotSortable

            '9���:�@����(��\��)
            .Columns(en_PcList.emKiki).HeaderText = "�@����" & vbCrLf
            .Columns(en_PcList.emKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emKiki).Width = 0
            .Columns(en_PcList.emKiki).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emKiki).Visible = False

        End With
    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_PC()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nMaxRowCnt As Integer  '�ő�s��
        Dim nRowCnt As Integer = 0

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '�s���擾SQL�쐬
        strSQL = GetSQL_PC(False)

        '���R�[�h�ǂݍ���
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "�f�[�^�擾�G���[(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�ő�s���Z�b�g
        dgvPc.RowCount = nMaxRowCnt

        '�\���f�[�^�̍��ڎ擾
        strSQL = GetSQL_PC(True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' �f�[�^�\��
            While sqlRead.Read()

                ' �f�[�^���o��
                dgvPc(en_PcList.emNo, nRowCnt).Value = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))               'PCID
                dgvPc(en_PcList.emPCkind, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("PC���"))        'PC���
                dgvPc(en_PcList.emPCName, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("�p�\�R������"))  '�p�\�R������
                dgvPc(en_PcList.emPCName2, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("PC��"))         'PC��

                ' �ُ�͈�
                If 0 = sqlRead.GetInt32(sqlRead.GetOrdinal("PC�敪")) Then
                    dgvPc(en_PcList.emOnOff, nRowCnt).Value = "��ײ�"
                Else
                    dgvPc(en_PcList.emOnOff, nRowCnt).Value = "��ײ�"
                End If

                'dgvPc(en_PcList.emNo, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCkind, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCName, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCName2, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emOnOff, nRowCnt).Style.BackColor = Color.Empty

                dgvPc(en_PcList.emBtn1, nRowCnt).Value = "۸޵�"        '���O�I�t
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvPc(en_PcList.emBtn2, nRowCnt).Value = "�ċN��"       '�ċN��
                dgvPc(en_PcList.emBtn2, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn2, nRowCnt).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvPc(en_PcList.emBtn3, nRowCnt).Value = "�����޳�"     '�V���b�g�_�E��
                dgvPc(en_PcList.emBtn3, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                '�}�X�^�[PC�̓{�^���������Ȃ��悤�ɂ���B(�ꊇ�ōs������)
                If m_sMasterPC = sqlRead.GetString(sqlRead.GetOrdinal("PC��")) Then
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                Else
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                End If

                '���u���͑S�Ė���
                If NOA_OFFLINE = g_NOA Then
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                End If

                dgvPc(en_PcList.emKiki, nRowCnt).Value = 0

                '''' ���s����
                nRowCnt += 1

            End While

            sqlRead.Close()

            '�@���Ԏ擾
            strSQL = GetSQL_PcStatus(True)

            ' Select�����s
            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then Exit Sub
            ' �f�[�^�\��
            While sqlRead.Read()
                Dim nPcID As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))                 'PCID
                Dim nSts As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("�@����"))              '�@����
                If 0 <> nSts Then
                    For ii As Integer = 0 To dgvPc.RowCount - 1
                        If nPcID = CInt(dgvPc.Item(en_PcList.emNo, ii).Value) Then
                            dgvPc(en_PcList.emKiki, ii).Value = nSts
                        End If
                    Next
                End If

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("PC�f�[�^�\�����ɗ�O[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL���擾
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PC(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL = " SELECT "
            strSQL &= " M.PCID as PCID,"
            strSQL &= " M.�p�\�R������ as �p�\�R������,"
            strSQL &= " M.PC�� as PC��,"
            strSQL &= " I.PC��� as PC���,"
            strSQL &= " I.PC�敪 as PC�敪"
        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        '// �ُ�̂ݕ\���͌���̂܂�
        '// ���ׂĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        strSQL &= " FROM " & DB_PC_MASTER & " M "
        strSQL &= " INNER JOIN " & DB_PC_INF & " I ON I.PCID = M.PCID"
        If bMode Then
            strSQL &= " ORDER BY PCID ASC "
        End If

        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function

    ''' <summary>
    ''' SQL���擾(���)
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PcStatus(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL &= " SELECT a.PCID,"
            strSQL &= "        b.�@����,"
            strSQL &= "        c.PC�敪,"

        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.�@��ID = b.�@��ID "
        strSQL &= " INNER JOIN " & DB_PC_INF & " c"
        strSQL &= " ON a.PCID = c.PCID "
        strSQL &= " WHERE (a.�����ُ�x <> 0 or a.�@��ُ�x <> 0) "
        strSQL &= " AND a.PCID <> 0 "
        strSQL &= " AND b.�@���� <> 0 "


        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �K�C�_���X�ꗗ
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �K�C�_���X�ꗗ �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuideInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0

            '' �����`
            .ReadOnly = True                                             ' �ǂݎ���p
            .ScrollBars = ScrollBars.Both                                                           '�X�N���[���o�[�L��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ���[�U�[ ��T�C�Y�ύX

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1�s�����ɐF�ւ�

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@�햼��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���l
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���b�Z�[�W

            '''' ���`
            '1���:NO
            .Columns(en_GuideList.emNo).HeaderText = "NO" & vbCrLf
            .Columns(en_GuideList.emNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emNo).Width = 30
            .Columns(en_GuideList.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_GuideList.emNo).Frozen = True


            '2���:�@�햼��
            .Columns(en_GuideList.emName).HeaderText = "�@�햼��" & vbCrLf
            .Columns(en_GuideList.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_GuideList.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emName).Width = 220
            .Columns(en_GuideList.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:���l
            .Columns(en_GuideList.emData).HeaderText = "�l" & vbCrLf
            .Columns(en_GuideList.emData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(en_GuideList.emData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emData).Width = 50
            .Columns(en_GuideList.emData).SortMode = DataGridViewColumnSortMode.NotSortable

            '4���:���b�Z�[�W
            .Columns(en_GuideList.emMsg).HeaderText = "MSG" & vbCrLf
            .Columns(en_GuideList.emMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_GuideList.emMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emMsg).Width = 340
            .Columns(en_GuideList.emMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub
    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_Guidance()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nMaxRowCnt As Integer  '�ő�s��
        Dim nRowCnt As Integer = 0

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(�K�C�_���X) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '�s���擾SQL�쐬
        strSQL = GetSQL_Guidance(False)

        '���R�[�h�ǂݍ���
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "�f�[�^�擾�G���[(�K�C�_���X) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�ő�s���Z�b�g
        dgvGuide.RowCount = nMaxRowCnt

        '�\���f�[�^�̍��ڎ擾
        strSQL = ""
        strSQL = GetSQL_Guidance(True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' �f�[�^�\��
            While sqlRead.Read()

                ' �f�[�^���o��
                dgvGuide(en_GuideList.emNo, nRowCnt).Value = sqlRead.GetInt32(0)       '�@��ID
                dgvGuide(en_GuideList.emName, nRowCnt).Value = sqlRead.GetString(1)    '�@�햼


                '���l�Ƃ��ċ@���Ԃ�\������B
                dgvGuide(en_GuideList.emData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '���l

                dgvGuide(en_GuideList.emMsg, nRowCnt).Value = sqlRead.GetString(5)          '�ُ�MSG

                '''' ���s����
                nRowCnt += 1

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("�K�C�_���X�f�[�^�\�����ɗ�O[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub
    ''' <summary>
    ''' SQL���擾
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_Guidance(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL &= " SELECT a.�@��ID,"
            strSQL &= "        a.�@�햼,"
            strSQL &= "        a.�ُ�͈�,"
            strSQL &= "        a.�����ُ�x,"
            strSQL &= "        a.�@��ُ�x,"
            strSQL &= "        ISNULL(a.�ُ�MSG,''),"
            strSQL &= "        b.�@����,"

        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        '// �ُ�̂ݕ\���͌���̂܂�
        '// ���ׂĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.�@��ID = b.�@��ID "
        strSQL &= " WHERE (a.�����ُ�x = 0 and a.�@��ُ�x = 0) and a.�ُ�͈� <> -1"


        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetSecurityData()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        If 0 = m_dctSecurity.Count Then
            Exit Sub
        End If

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(�K�C�_���X) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        m_lstStatus.Clear()
        Dim keycoll As Dictionary(Of Integer, Control).KeyCollection = m_dctSecurity.Keys

        '�s���擾SQL�쐬
        strSQL = ""
        strSQL &= " SELECT a.�@��ID, b.�@���� "
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.�@��ID = b.�@��ID "
        strSQL &= " WHERE a.�@��ID IN(0"
        For Each key As Integer In keycoll
            strSQL &= ("," & key.ToString)
        Next
        strSQL &= ")"

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' �f�[�^�\��
            While sqlRead.Read()

                Dim nId As Integer = sqlRead.GetInt32(0)             '�@��ID

                If m_dctSecurity.ContainsKey(nId) Then
                    Dim typStat As TYP_STATUS_INF
                    typStat.nId = nId
                    typStat.nStatus = sqlRead.GetInt32(1)
                    m_lstStatus.Add(typStat)
                End If

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("�K�C�_���X�f�[�^�\�����ɗ�O[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��Ԉꗗ �O���b�h������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ��Ԉꗗ �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvKikiInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0

            '' �����`
            .ReadOnly = True                                             ' �ǂݎ���p
            .ScrollBars = ScrollBars.Both                                                           '�X�N���[���o�[�L��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ���[�U�[ ��T�C�Y�ύX

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1�s�����ɐF�ւ�

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@�햼��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���l
            .Columns.Add(New DataGridViewButtonColumn)    ' ��������(�{�^��)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���b�Z�[�W

            '''' ���`
            '1���:NO
            .Columns(en_KikiList.iNo).HeaderText = "NO" & vbCrLf
            .Columns(en_KikiList.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).Width = 30
            .Columns(en_KikiList.iNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_KikiList.iNo).Frozen = True


            '2���:�@�햼��
            .Columns(en_KikiList.iKiKiName).HeaderText = "�@�햼��" & vbCrLf
            .Columns(en_KikiList.iKiKiName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiKiName).Width = 220
            .Columns(en_KikiList.iKiKiName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:�ُ�͈�
            .Columns(en_KikiList.iIjouHani).HeaderText = "�ُ�͈�" & vbCrLf
            .Columns(en_KikiList.iIjouHani).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iIjouHani).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iIjouHani).Width = 72
            .Columns(en_KikiList.iIjouHani).SortMode = DataGridViewColumnSortMode.NotSortable


            '4���:�������
            .Columns(en_KikiList.iKensa).HeaderText = "�������" & vbCrLf
            .Columns(en_KikiList.iKensa).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKensa).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKensa).Width = 64
            .Columns(en_KikiList.iKensa).SortMode = DataGridViewColumnSortMode.NotSortable


            '5���:�@����
            .Columns(en_KikiList.iKiki).HeaderText = "�@����" & vbCrLf
            .Columns(en_KikiList.iKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiki).Width = 64
            .Columns(en_KikiList.iKiki).SortMode = DataGridViewColumnSortMode.NotSortable


            '6���:���l
            .Columns(en_KikiList.iData).HeaderText = "���l" & vbCrLf
            .Columns(en_KikiList.iData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).Width = 50
            .Columns(en_KikiList.iData).SortMode = DataGridViewColumnSortMode.NotSortable

            '7���:��������
            .Columns(en_KikiList.iBtn).HeaderText = "��������" & vbCrLf
            .Columns(en_KikiList.iBtn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iBtn).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iBtn).Width = 64
            .Columns(en_KikiList.iBtn).SortMode = DataGridViewColumnSortMode.NotSortable


            '8���:�ُ�MSG
            .Columns(en_KikiList.iMsg).HeaderText = "�ُ�MSG" & vbCrLf
            .Columns(en_KikiList.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).Width = 1000
            .Columns(en_KikiList.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_Status()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nMaxRowCnt As Integer  '�ő�s��
        Dim nRowCnt As Integer = 0

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(��Ԉꗗ) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '�s���擾SQL�쐬
        strSQL = GetSQL_Status(False)

        '���R�[�h�ǂݍ���
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "�f�[�^�擾�G���[(��Ԉꗗ) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�ő�s���Z�b�g
        dgvKiki.RowCount = nMaxRowCnt

        '�\���f�[�^�̍��ڎ擾
        strSQL = ""
        strSQL = GetSQL_Status(True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' �f�[�^�\��
            While sqlRead.Read()

                ' �f�[�^���o��
                dgvKiki(en_KikiList.iNo, nRowCnt).Value = sqlRead.GetInt32(0)           '�@��ID
                dgvKiki(en_KikiList.iKiKiName, nRowCnt).Value = sqlRead.GetString(1)     '�@�햼

                ' �ُ�͈�
                Dim nWk As Integer = sqlRead.GetInt32(2)
                dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = GetErrRangeString(nWk)
                dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty

                '������ԕ\��
                If 0 = sqlRead.GetInt32(3) Then
                    '�����ُ�x�Ȃ�
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then
                    '�@���Ԃ��ُ�̏ꍇ�͌����ُ�x�����ĕ\������
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = ucStatusAria.DspKensa(CType(sqlRead.GetInt32(3), KizuForm.EM_DIV_KENSA))
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                Else

                    '�����ُ�x����A�@���Ԑ���
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "����"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                End If

                '�@���ԕ\��
                If 0 = sqlRead.GetInt32(4) Then

                    '�@��ُ�x�Ȃ�
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '�@���Ԃ��ُ�̏ꍇ�́A�@��ُ�x�����ĕ\������
                    Dim emKiki As KizuForm.EM_DIV_KIKI = CType(sqlRead.GetInt32(4), KizuForm.EM_DIV_KIKI)
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = ucStatusAria.DspKiki(emKiki)
                    Select Case emKiki
                        Case KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clCare
                        Case KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clErr
                        Case Else
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                    End Select
                Else
                    '�@��ُ�x����A�@���Ԑ���
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "����"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clOK
                End If

                '���l�Ƃ��ċ@���Ԃ�\������B
                dgvKiki(en_KikiList.iData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '���l

                dgvKiki(en_KikiList.iBtn, nRowCnt).Value = "��������"    '��������
                dgvKiki(en_KikiList.iBtn, nRowCnt).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvKiki(en_KikiList.iBtn, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                If 0 = sqlRead.GetInt32(3) And 0 = sqlRead.GetInt32(4) Then
                    dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Gray
                    dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                Else
                    ' �I�����C���A���u���͉����Ȃ�
                    If Not (NOA_ONLINE = g_NOA Or NOA_OFFLINE = g_NOA) Then
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Black
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    Else
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Gray
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    End If

                End If

                dgvKiki(en_KikiList.iMsg, nRowCnt).Value = sqlRead.GetString(5)          '�ُ�MSG

                '''' ���s����
                nRowCnt += 1

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("�@���ԃf�[�^�\�����ɗ�O[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL���擾
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_Status(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL &= " SELECT a.�@��ID,"
            strSQL &= "        a.�@�햼,"
            strSQL &= "        a.�ُ�͈�,"
            strSQL &= "        a.�����ُ�x,"
            strSQL &= "        a.�@��ُ�x,"
            strSQL &= "        ISNULL(a.�ُ�MSG,''),"
            strSQL &= "        b.�@����,"

        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        '// �ُ�̂ݕ\���͌���̂܂�
        '// ���ׂĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.�@��ID = b.�@��ID "

        ' �I���{�^���ɂ�镪��
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            ' �ُ���̂ݕ\��
            strSQL &= " WHERE (a.�����ُ�x <> 0 or a.�@��ُ�x <> 0) "
            strSQL &= " AND b.�@���� <> 0"
        ElseIf m_nRadKubun = EM_RAD_KUBUN.nDefult Then
            ' ����{�ُ� ���\��
            strSQL &= " WHERE (a.�����ُ�x <> 0 or a.�@��ُ�x <> 0) "
        Else
            ' �S�ĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        End If

        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���Ə�ԕ\����
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_SogyoStatus()
        Dim sts As TYP_SOGYO_STATUS = Nothing
        If Get_SogyoStatus(sts) Then
            DspSogyoStatus(sts, lblSogyo)
            DspKadouStatus(sts, lblKadou)
            DspUntenStatus(sts, lblUnten)
            DspPlgSelect(sts, lblPlg)
        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Get_SogyoStatus(ByRef sts As TYP_SOGYO_STATUS) As Boolean
        Dim bRet As Boolean = False
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nRowCnt As Integer = 0
        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB�ڑ��G���[(����) [" & DB_STATUS_MASTER & "]")
            Return False
        End If



        '�s���擾SQL�쐬
        strSQL = ""
        strSQL &= "SELECT "
        strSQL &= " �@��ID,"
        strSQL &= " �@���� "
        strSQL &= " FROM " & DB_STATUS_INF
        strSQL &= " WHERE "
        strSQL &= "    �@��ID=" & DIV_KADOU_ID
        strSQL &= " OR �@��ID=" & DIV_UNTEN_ID
        strSQL &= " OR �@��ID=" & DIV_SOGYO_ID
        strSQL &= " OR �@��ID=" & DIV_PLGMODE_ID

        Try


            ' Select�����s
            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then
                Exit Try
            End If


            ' �f�[�^�\��
            While sqlRead.Read()

                Select Case sqlRead.GetInt32(0)
                    Case DIV_KADOU_ID
                        sts.emKadou = CType(sqlRead.GetInt32(1), EM_DIV_KADOU)
                    Case DIV_UNTEN_ID
                        sts.emUnten = CType(sqlRead.GetInt32(1), EM_DIV_UNTEN)
                    Case DIV_SOGYO_ID
                        sts.emSogyo = CType(sqlRead.GetInt32(1), EM_DIV_SOGYO)
                    Case DIV_PLGMODE_ID
                        sts.emPlg = CType(sqlRead.GetInt32(1), EM_DIV_PLG)
                End Select

            End While

            bRet = True
        Catch ex As Exception
            Log(LNO.ERR, String.Format("���Ə�ԃf�[�^�擾���ɗ�O[{0}]", ex.Message))
            bRet = False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try
        Return bRet
    End Function

    ''' <summary>
    ''' �ғ���Ԃ̕\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspKadouStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emKadou

            Case EM_DIV_KADOU.DIV_KADOU_INIT
                lbl.Text = "��������"
                lbl.BackColor = g_ColorKadouInit
            Case EM_DIV_KADOU.DIV_KADOU_STOP
                lbl.Text = "��~��"
                lbl.BackColor = g_ColorKadouStop
            Case EM_DIV_KADOU.DIV_KADOU_START

                '�^�]��Ԃ�����
                If sts.emUnten = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
                    lbl.Text = "�ғ���"
                Else
                    lbl.Text = "�e�X�g��"
                End If
                lbl.BackColor = g_ColorKadouStart
            Case EM_DIV_KADOU.DIV_KADOU_RETRY
                lbl.Text = "��ײ��"
                lbl.BackColor = g_ColorKadouRetry
        End Select

    End Sub
    ''' <summary>
    ''' �^�]���[�h�̕\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspUntenStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emUnten
            Case EM_DIV_UNTEN.DIV_UNTEN_STOP
                lbl.Text = "��~"
                lbl.BackColor = g_ColorUntenStop
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
                lbl.Text = "�ʏ�^�]"
                lbl.BackColor = g_ColorUntenNomal
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
                lbl.Text = "�T���v���B�e"
                lbl.BackColor = g_ColorUntenSamp
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
                lbl.Text = "�J��������"
                lbl.BackColor = g_ColorUntenCamera
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
                lbl.Text = "�V�~�����[�V����"
                lbl.BackColor = g_ColorUntenSmyu
        End Select

    End Sub
    ''' <summary>
    ''' PLG�I���̕\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspPlgSelect(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emPlg
            Case EM_DIV_PLG.DIV_PLG_LINE
                lbl.Text = "���C��PLG"
                lbl.BackColor = g_ColorPlgLine
            Case EM_DIV_PLG.DIV_PLG_TOP
                lbl.Text = "�\�Z��"
                lbl.BackColor = g_ColorPlgTop
            Case EM_DIV_PLG.DIV_PLG_BOT
                lbl.Text = "���Z��"
                lbl.BackColor = g_ColorPlgBot
            Case EM_DIV_PLG.DIV_PLG_TEST
                lbl.Text = "TEST�p�p���X"
                lbl.BackColor = g_ColorPlgTest
        End Select

    End Sub

    ''' <summary>
    ''' ���Ə�Ԃ̕\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspSogyoStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emSogyo
            Case EM_DIV_SOGYO.DIV_SOGYO_NON
                lbl.Text = "�s��"
                lbl.BackColor = g_ColorSogyoNon
            Case EM_DIV_SOGYO.DIV_SOGYO_STOP
                lbl.Text = "�x�~��"
                lbl.BackColor = g_ColorSogyoStop
            Case EM_DIV_SOGYO.DIV_SOGYO_SLOW
                lbl.Text = "�ᑬ����"
                lbl.BackColor = g_ColorSogyoSlow
            Case EM_DIV_SOGYO.DIV_SOGYO_RUN
                lbl.Text = "���ƒ�"
                lbl.BackColor = g_ColorSogyoRun
        End Select

    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �I���s�`�F���W
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvKiki.SelectionChanged

        '�I��������
        dgvKiki.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' �s�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowEnter

        '�s�̔w�i��ύX
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub

    ''' <summary>
    ''' �s�ړ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowLeave

        '�s�̔w�i���f�t�H���g�F�ɖ߂�
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' �I���s�`�F���W
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvGuide.SelectionChanged

        '�I��������
        dgvGuide.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' �s�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGuide.RowEnter

        '�s�̔w�i��ύX
        dgvGuide.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub

    ''' <summary>
    ''' �s�ړ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGuide.RowLeave

        '�s�̔w�i���f�t�H���g�F�ɖ߂�
        dgvGuide.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    '''' <summary>
    '''' �I���s�`�F���W
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPc.SelectionChanged

    '    '�I��������
    '    dgvPc.CurrentCell.Selected = False

    'End Sub

    '''' <summary>
    '''' �s�I��
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.RowEnter

    '    '�s�̔w�i��ύX
    '    dgvPc.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    'End Sub

    '''' <summary>
    '''' �s�ړ�
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.RowLeave

    '    '�s�̔w�i���f�t�H���g�F�ɖ߂�
    '    dgvPc.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    'End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���������{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���������{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvkiki_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub
        Dim nKikiNo As Integer = CInt(dgv(en_KikiList.iNo, e.RowIndex).Value)
        Dim strID As String = ""

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case en_KikiList.iBtn      '���������{�^��
                If EM_DGVBTN_TYPE.emBtnNG = CType(dgv(en_KikiList.iBtn, e.RowIndex).Tag, EM_DGVBTN_TYPE) Then
                    Exit Sub
                End If
                If Windows.Forms.DialogResult.OK = DspMsg(nKikiNo) Then
                    If Not KikiData_Init(nKikiNo) Then
                        MsgBox("�������� ���s ID=" & nKikiNo)
                    End If
                Else
                End If
        End Select
    End Sub
    ''' <summary>
    ''' �@���ԋ�������
    ''' </summary>
    ''' <param name="nKikiID">�@��ID</param>	
    ''' <remarks></remarks>
    Private Function KikiData_Init(ByVal nKikiID As Integer) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sSQL As String
        Try
            '' DB�ڑ�
            If Not tcls_DB.Open() Then Return False


            '-------------------------------
            '' SQL����
            sSQL = "UPDATE " & DB_STATUS_INF
            sSQL &= " SET "
            '''' �A�b�v�f�[�g�́A�r���̂�

            sSQL &= tClass_SQLServer.sqlAddUpdate("�@����", 0)
            tClass_SQLServer.sqlColumnTrim(sSQL)

            sSQL &= " WHERE �@��ID=" & nKikiID

            '-------------------------------
            '' ���s
            If 0 <= tcls_DB.DMLExecute(sSQL) Then
                Log(LNO.MSG, "�@���ԋ������� ���{ [ID=" & nKikiID & "]")
                LogEx(LNO.MSG, "�@���ԋ������� ���{ [ID=" & nKikiID & "]")
                Return True
            Else
                Log(LNO.ERR, "�@���ԋ������� ���s [ID=" & nKikiID & "]")
                Return False
            End If

        Catch ex As Exception
            Log(LNO.ERR, "�@���ԋ������� �ُ� [ID=" & nKikiID & "][" & ex.Message & "]")
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' �m�F���b�Z�[�W
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function DspMsg(ByVal nKikiID As Integer) As DialogResult
        Dim strMsg As String    '���b�Z�[�W 
        Dim bRet As DialogResult


        'PC�����Ȃ��ꍇ�́A�V�X�e���I��
        strMsg = "�@��ID = " & nKikiID & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "�������������܂��B" & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "��낵���ł����H" & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "���j������ȂǂŃZ�b�g���Ă���@��ID��" & vbCrLf
        strMsg &= "    �����Ɉُ�ɂȂ邱�Ƃ�����܂��B" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        bRet = frmMsg.ShowDialog()

        Return bRet

    End Function



    '***************************************************************************
    ' �^�C�}�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' ������^�C�}�[
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick

        'DB�ڑ��`�F�b�N
        If Not OpenCheck(TASKINI_NAME, INI_DB) Then
            Log(LNO.ERR, "DB�ڑ��G���[(Open�`�F�b�N) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�f�[�^�\��
        Call DspData_PC()
        Call DspData_Status()
        Call DspData_Guidance()
        Call DspData_SogyoStatus()
        Call GetSecurityData()
    End Sub


    '***************************************************************************
    ' �t�@���N�V�����L�[�֘A
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        '' �f�o�b�O���O�\��
        If 1 - 1 = FncNo AndAlso onKey = Keys.Shift Then
            ShowDebugLog()
            Exit Sub
        End If

        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��

            Case 0                              'F1
                ' �̏Ⴭ�O��ʒ��ڕ\��
                Dim frm As New frmLog
                frm.Owner = Me.my_frmParent
                frm.SetParentForm(Me.my_frmParent)
                Me.SetParentForm(Nothing)
                frm.Show()
                ExecFromClose()

            Case 1                              'F2
                m_frmSecurityDisp.ShowDialog()

            Case 2                              'F3
                m_nRadKubun = EM_RAD_KUBUN.nErr
                DspData_Status()
                SetButtonColor()

            Case 3                              'F4
                m_nRadKubun = EM_RAD_KUBUN.nDefult
                DspData_Status()
                SetButtonColor()

            Case 4                              'F5
                ' �ُ��ԕ\��
                Dim frm As New frmStart
                frm.Owner = Me
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing

            Case 5                              'F6
                ' ��{����ꗗ
                Dim frm As New frmBaseList
                frm.SetParentForm(Me)
                frm.Owner = Me
                frm.Show()
                Me.Visible = False

            Case 6                              'F7
                ' �G�b�W���o������ʕ\��
                Dim frm As New frmEdgeList
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing

            Case 7                              'F8
                ' �r���g�����h�O���t��ʕ\��
                Dim frmGraph As New frmDefIncidenceGraph(gcls_Log)
                frmGraph.SetParentForm(Me)
                frmGraph.Owner = Me
                frmGraph.Show()
                Me.Visible = False

            Case 8                              'F9

            Case 9                              'F10
                ' ��ʈ��
                Call PrintOut()

            Case 10                             'F11                
                ' F11 �n�[�h�R�s�[
                Call HardCopy(IMG_PATH, "RAS���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case 11
                MyBase.ExecFromClose()          ' �I��
        End Select
    End Sub

    ''' <summary>
    ''' �ُ���̂݃{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnErrOnly_Click(sender As System.Object, e As System.EventArgs) Handles btnErrOnly.Click
        m_nRadKubun = EM_RAD_KUBUN.nErr
        DspData_Status()
        SetButtonColor()
    End Sub

    ''' <summary>
    ''' �S�ă{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnErrAll_Click(sender As System.Object, e As System.EventArgs) Handles btnErrAll.Click
        m_nRadKubun = EM_RAD_KUBUN.nDefult
        DspData_Status()
        SetButtonColor()
    End Sub

    ''' <summary>
    ''' �u�ُ���̂݁v�u�S�āv�{�^���F�ւ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetButtonColor()
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            btnErrOnly.BackColor = Color.Cyan
            btnErrAll.BackColor = Color.Empty
        Else
            btnErrOnly.BackColor = Color.Empty
            btnErrAll.BackColor = Color.Cyan
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���O�I�t�A�ċN���A�V���b�g�_�E���֘A
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �SPC�ꊇ���O�I�t�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_LogOff_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_LogOff.Click
        ' ���u���A�f�o�b�O���͉������Ȃ�
        If Not (NOA_ONLINE = g_NOA Or NOA_DEBUG = g_NOA Or NOA_KAISEKI = g_NOA) Then
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' ���Z�b�g�m�F
        '-----------------------------------------------------------------------
        Dim nResetMode As Windows.Forms.DialogResult                            ' ���Z�b�g���[�h (Yes:���O�I�t No:�ċN�� Cancel:���~)
        nResetMode = CheckAllMsg("���O�I�t")                            ' ���Z�b�g�m�F�_�C�A���O�\��
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' �p�X���[�h�m�F�B����Ă���L�����Z��
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' ���Z�b�g�v�����M
        '-----------------------------------------------------------------------
        Call SendResetMail(FACT_KS_MASTER_08, "*")
    End Sub

    ''' <summary>
    ''' �SPC�ꊇ�ċN���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_ReStart_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_ReStart.Click
        ' ���u���͉������Ȃ�
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' ���Z�b�g�m�F
        '-----------------------------------------------------------------------
        Dim nResetMode As Windows.Forms.DialogResult                            ' ���Z�b�g���[�h (Yes:���O�I�t No:�ċN�� Cancel:���~)
        nResetMode = CheckAllMsg("�ċN��")                            ' ���Z�b�g�m�F�_�C�A���O�\��
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' �p�X���[�h�m�F�B����Ă���L�����Z��
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' ���Z�b�g�v�����M
        '-----------------------------------------------------------------------
        Call SendResetMail(FACT_KS_MASTER_07, "*")
    End Sub

    ''' <summary>
    ''' �SPC�ꊇ�V���b�g�_�E���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_ShutDown_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_ShutDown.Click
        Dim sts As TYP_SOGYO_STATUS = Nothing
        ' ���u���͉������Ȃ�
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If

        If Get_SogyoStatus(sts) Then
            '�V�X�e����Ԃ��`�F�b�N
            If EM_DIV_KADOU.DIV_KADOU_STOP = sts.emKadou Then

                '��~��
                Call ExitSystem(True)           ' F08 �V�X�e���I��
            Else

                '��~�łȂ�
                Call ExitSystem(False)          ' �V�X�e���I��
            End If
        Else
            Call ExitSystem(True)           ' F08 �V�X�e���I��
        End If
    End Sub



    ''' <summary>
    ''' PC���f�[�^�O���b�h�r���[�N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPc_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.CellContentClick

        Dim nResetMode As Windows.Forms.DialogResult                    ' ���Z�b�g���[�h (Yes:���s Cancel:���~)
        Dim strDspPcName As String                                      ' ���b�Z�[�W�ɕ\������PC��
        Dim strPcName As String = Nothing                               ' PC���擾
        Dim nPcKind As Integer = 0
        Dim emBtnType As EM_BTN_TYPE
        Dim strTypeMsg As String = ""

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        If e.ColumnIndex = en_PcList.emBtn1 Then
            emBtnType = EM_BTN_TYPE.emBtn_LogOff
            strTypeMsg = "���O�I�t"
        ElseIf e.ColumnIndex = en_PcList.emBtn2 Then
            emBtnType = EM_BTN_TYPE.emBtn_Reset
            strTypeMsg = "�ċN��"
        ElseIf e.ColumnIndex = en_PcList.emBtn3 Then
            emBtnType = EM_BTN_TYPE.emBtn_Exit
            strTypeMsg = "�V���b�g�_�E��"
        Else
            Exit Sub
        End If

        ' ���u���͉������Ȃ�
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If
        '�����s���͉������Ȃ�
        If EM_DGVBTN_TYPE.emBtnNG = CType(dgvPc(e.ColumnIndex, e.RowIndex).Tag, EM_DGVBTN_TYPE) Then
            Exit Sub
        End If

        nPcKind = CInt(dgvPc.Item(en_PcList.emNo, e.RowIndex).Value)
        strPcName = CStr(dgvPc.Item(en_PcList.emPCName2, e.RowIndex).Value)

        '�N���b�N�����s����PC�����擾���āA���Z�b�g�y�у��O�I�t���s��
        strDspPcName = CStr(dgvPc.Item(en_PcList.emPCName, e.RowIndex).Value)

        '-----------------------------------------------------------------------
        ' ���Z�b�g�m�F
        '-----------------------------------------------------------------------
        nResetMode = CheckMsg(strDspPcName, strTypeMsg)                            ' ���Z�b�g�m�F�_�C�A���O�\��
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' �p�X���[�h�m�F�B����Ă���L�����Z��
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' ���Z�b�g�v�����M
        '-----------------------------------------------------------------------
        Select Case emBtnType
            Case EM_BTN_TYPE.emBtn_LogOff
                Call SendResetMail(FACT_KS_MASTER_08, strPcName)
            Case EM_BTN_TYPE.emBtn_Reset
                Call SendResetMail(FACT_KS_MASTER_07, strPcName)
            Case EM_BTN_TYPE.emBtn_Exit
                Call SendResetMailSub(FACT_KS_SLAVE_04, strPcName)
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' PC���Z�b�gor�V�X�e���I�����[�����M
    ''' </summary>
    ''' <param name="nEventNo">�C�x���gNo</param>
    ''' <param name="strPcName">PC��</param>
    ''' <remarks></remarks>
    Private Sub SendResetMail(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")
        Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

        que.nEventNo = nEventNo                                                 ' PC���Z�b�g
        que.nLineNo = m_nLineID
        que.data = strPcName

        Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)      ' ���[�����M 
    End Sub

    ''' <summary>
    ''' �}�X�^�[PC�ȊO�̃V���b�g�_�E�����[�����M
    ''' </summary>
    ''' <param name="nEventNo">�C�x���gNo</param>
    ''' <param name="strPcName">PC��</param>
    ''' <remarks></remarks>
    Private Sub SendResetMailSub(ByVal nEventNo As Integer, ByVal strPcName As String)
        Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

        que.nEventNo = nEventNo                                                 ' PC���Z�b�g
        que.nLineNo = m_nLineID
        que.data = strPcName

        Call KizuLibCLI.KizuMod.Send_Mail(m_sSubTask, strPcName, que)      ' ���[�����M 
    End Sub

    ''' <summary>
    ''' �m�F
    ''' </summary>
    ''' <param name="strPcName">���b�Z�[�W�ɕ\������PC��</param>
    ''' <param name="strTypMsg">���b�Z�[�W�ɕ\������C�x���g(���O�I�t�A�ċN���A�V���b�g�_�E��)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckMsg(ByVal strPcName As String, ByVal strTypMsg As String) As DialogResult
        Dim strMsg As String                                                    ' �\�����b�Z�[�W 

        strMsg = strPcName & "��" & strTypMsg & "�����s���܂����H"

        ' ���ʃ��b�Z�[�W
        strMsg &= vbCrLf & vbCrLf
        strMsg &= "�͂�   �F �p�\�R����" & strTypMsg & "�����s" & vbCrLf
        strMsg &= "��ݾ�  �F �I�����L�����Z��" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, DialogResult.Yes, DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' �m�F
    ''' </summary>
    ''' <param name="strTypMsg">���b�Z�[�W�ɕ\������C�x���g(���O�I�t�A�ċN���A�V���b�g�_�E��)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckAllMsg(ByVal strTypMsg As String) As DialogResult
        Dim strMsg As String                                                    ' �\�����b�Z�[�W 

        strMsg = "�SPC��" & strTypMsg & "�����s���܂����H"

        ' ���ʃ��b�Z�[�W
        strMsg &= vbCrLf & vbCrLf
        strMsg &= "�͂�   �F �p�\�R����" & strTypMsg & "�����s" & vbCrLf
        strMsg &= "��ݾ�  �F �I�����L�����Z��" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, DialogResult.Yes, DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' �V�X�e���I���m�F
    ''' </summary>
    ''' <param name="bStopFlg">��~�t���O(True:��~,False:��~�łȂ�)</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function CheckShatdown(Optional ByVal bStopFlg As Boolean = False) As DialogResult
        Dim strMsg As String    '���b�Z�[�W 

        If bStopFlg Then                                                        ' PC�����Ȃ��ꍇ�́A�V�X�e���I��
            strMsg = "�SPC�̃V���b�g�_�E�������s���܂����H"
        Else
            strMsg = "�V�X�e�����ғ����ł��B" &
            vbNewLine & "����ł��A�SPC�̃V���b�g�_�E�������s���܂����H"
        End If

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' �p�X���[�h�m�F
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function CheckPass() As DialogResult
        ' �p�X���[�h���ݒ�̏ꍇ
        If "" = m_strResetPass Then
            Return DialogResult.OK
        End If

        Dim frmPass As New tClassForm.frmPop_Pass(m_strResetPass)
        Dim bRet As DialogResult = frmPass.ShowDialog()                         ' �p�X���[�h�m�F
        Return bRet
    End Function

    ''' <summary>
    ''' �V�X�e���I������
    ''' <param name="bStopFlg">��~���t���O(True:��~��,False:����ȊO)</param>
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)
        If Windows.Forms.DialogResult.OK <> CheckShatdown(bStopFlg) Then         ' ���Z�b�g�m�F
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' �p�X���[�h�m�F
            Exit Sub
        End If
        Call SendResetMail(FACT_KS_MASTER_06)                                   ' �V�X�e���I�����[�����M
    End Sub

    ''' <summary>
    ''' �ُ�͈͕����擾
    ''' </summary>
    ''' <param name="nNo"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetErrRangeString(ByVal nNo As Integer) As String
        Dim strItem As String = ""
        For ii As Integer = 0 To m_lstLovItem.Count - 1
            If m_lstLovItem(ii).ItemNo = nNo Then
                strItem = m_lstLovItem(ii).DispData
                Exit For
            End If
        Next
        Return strItem
    End Function

    ''' <summary>
    ''' RAS��ʂɕ\������\���F���擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetColor()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT RAS����F,"
        strSQL &= "        RAS�ُ�F,"
        strSQL &= "        RAS���ӐF"
        strSQL &= " FROM " & DB_COMMON_MASTER
        'strSQL &= " WHERE LINEID = " & KizuLibCLI.KizuMod.GetLineID()

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            ' �f�[�^�\��
            sqlRead.Read()

            m_clOK = Color.FromArgb(CInt(sqlRead.GetValue(0)))
            m_clErr = Color.FromArgb(CInt(sqlRead.GetValue(1)))
            m_clCare = Color.FromArgb(CInt(sqlRead.GetValue(2)))

        Catch ex As Exception

        Finally

            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' �ُ�@�����ݸ����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timErrCheck_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timErrCheck.Tick
        Dim ii As Integer

        '�F�ݒ�(�_�ł����邽��)
        m_clOnLineNG = CType(IIf(m_clErr = m_clOnLineNG, m_clNormal, m_clErr), Color)
        m_clOffLineNG = CType(IIf(m_clCare = m_clOffLineNG, m_clNormal, m_clCare), Color)

        'PC���_��
        For ii = 0 To dgvPc.RowCount - 1
            Dim nSts As Integer = CInt(dgvPc(en_PcList.emKiki, ii).Value)
            If 0 <> nSts Then
                If "��ײ�" = CStr(dgvPc(en_PcList.emOnOff, ii).Value) Then
                    'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOnLineNG
                    dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOnLineNG
                Else
                    'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOffLineNG
                    dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOffLineNG
                End If
            Else
                'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOK
                dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOK
            End If
        Next

        '��Ԉꗗ�_��
        'For ii = 0 To dgvKiki.RowCount - 1
        '    Dim str As String = CStr(dgvKiki(en_KikiList.iKiki, ii).Value)
        '    If ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOnLineNG
        '    ElseIf ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOffLineNG
        '    ElseIf ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_OK) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOK
        '    Else
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = Color.Empty
        '    End If
        'Next

        '��������
        For ii = 0 To m_lstStatus.Count - 1
            If m_dctSecurity.ContainsKey(m_lstStatus(ii).nId) Then
                Dim clrDspColor As Color
                If 0 <> m_lstStatus(ii).nStatus Then
                    clrDspColor = m_clOnLineNG
                Else
                    clrDspColor = m_clOK
                End If
                '�X�e�[�^�X��ԃZ�b�g
                m_frmSecurityDisp.SetData(m_dctSecurity(m_lstStatus(ii).nId).Name, m_lstStatus(ii).nStatus, clrDspColor)
            End If

        Next

        'F2�{�^��(��������)�{�^���F�ύX
        If m_frmSecurityDisp.ChkF02btn Then
            btnF02.BackColor = m_clNormal
        Else
            btnF02.BackColor = m_clOnLineNG
        End If

    End Sub




End Class
