'*******************************************************************************
'�@�������ь������
'	[Ver]
'		Ver.01    2015/08/28  ����
'
'	[����]
'	�������щ�ʋ@�\��\������	
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Imports System.Data.SqlClient

Public Class frmSearch

    '---------------------------------------------------------------------------
    ' �񋓑�
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �������ʗ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_RESULT_COL
        nDATE = 0           '�ʔ��t
        nCoilNo             '���YNo
        nOrderNo            '����No
        nMaterial           '�ގ�
        nHyomen             '�\�ʏ���
        nCoilLen            '������(m)
        nProConLen          '��ۺݒ�(m)
        nDspLen             '�\����(m)
        nAtu                '�ޗ���(mm)
        nWid                '�ޗ���(mm)
        nKensaKijun_O       '�����(�\)
        'nKensaKijun_U       '�����(��)
        nMixedRate          '������[%](�\/��)
        nMuke               '����
        nHinCode            '�i��
        nNextCode           '�v�掟�H��
        nDefCnt_O           '�\���r��(�\)
        nDefCnt_U           '�\���r��(��)
        nDefCnt_Total       '�\�����r��
        nDelHarmfulCnt_O    '�\�폜�L�Q�r��
        nDelHarmfulCnt_U    '���폜�L�Q�r��
        nDelHarmlessCnt_O   '�\�폜���Q�r��
        nDelHarmlessCnt_U   '���폜���Q�r��
        nSaveKizu_O         '�\�ۑ��r��
        nSaveKizu_U         '���ۑ��r��
        nPCode              '�����p�^�[��
        nSCode              '�����\�ʏ��(�\/��)
        nKanriNo            '�r�Ǘ�No
        nUntenStat          '�^�]���
        nKakuno             '�i�[�敪
        nKensaStat_O        '�������(�\)
        nKensaStat_U        '�������(��)
        nKikiStat_O         '�@����(�\)
        nKikiStat_U         '�@����(��)
        nToyuSplr           '�h���X�v���[���u���
        nPCCoil             'P/C�R�C��
        nPCCut              'P/C�J�b�g
        nPCSaisun           'P/C�̐�
        nPCZiseki           'P/C����
        nFFRecv             'FF��M(�O�H�����)
        nFFSend             'FF���M
        nKeyInfo            '�L�[���(��\��)
        MAX_RESULT_COL      '�������ʗ�
    End Enum

    ''' <summary>
    ''' ���ь�����ʂ̖߂�l
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum SEARCH_RETURN
        emSELECT_COIL = 0                                                       ' �R�C���I��
        emCANCEL                                                                ' �L�����Z��
    End Enum

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ�
    Private mcls_Param As clsParamManager                                       ' �p�����[�^�Ǘ�
    Private mcls_PreParam As clsPreParamManager                                     ' �p�����[�^�Ǘ�

    Private m_nSelectRowNo As Integer                                           ' ���݂̑I���R�C���C���f�b�N�X
    Private m_nRetCode As SEARCH_RETURN                                         ' ���̃N���X�̖߂�l

    Private mcls_DataMap As clsDataMap                                          ' �}�b�v���
    Private mcls_ReadDatamap As clsDataMap                                      ' �ǂݍ��ݗp�̃}�b�v���

    Private m_nSearchTimeOut As Integer                                         ' �R�}���h���s�^�C���A�E�g����[�����p(�P�ʁF�b)]

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
#Region "�v���p�e�B"
    ''' <summary>
    ''' �������ʂ̃}�b�v�����擾���܂�
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DataMap() As clsDataMap
        Get
            Return mcls_DataMap
        End Get
    End Property

    ''' <summary>
    ''' �W�J���̒���������]�w����擾���܂��i���E���]�j
    ''' </summary>
    ''' <returns>True:���]���� False:���]���Ȃ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsTurnNagate() As Boolean
        Get
            If chkNagate.Checked Then
                Return True
            Else
                Return False
            End If
            Return False
        End Get
    End Property

    ''' <summary>
    ''' �W�J���̕��������]�w����擾���܂�
    ''' </summary>
    ''' <returns>True:���]���� False:���]���Ȃ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsTurnDS() As Boolean
        Get
            Return chkHaba.Checked
        End Get
    End Property

    ''' <summary>
    ''' ���^�[���R�[�h
    ''' </summary>
    ''' <returns>�������쌋��</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ReturnCode() As SEARCH_RETURN
        Get
            Return m_nRetCode
        End Get
    End Property

#End Region


    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' ���ь�����ʂ��쐬���܂�
    ''' </summary>
    ''' <param name="param">�p�����[�^�Ǘ�</param>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal param As clsParamManager, ByRef preparam As clsPreParamManager, ByVal log As tClass_LogManager)
        Try
            ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
            InitializeComponent()
            ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

            '-------------------------------------------------------------------
            ' �p�����[�^�̏�����
            '-------------------------------------------------------------------
            mcls_Log = log
            mcls_Param = param
            mcls_PreParam = preparam
            m_nSelectRowNo = -1
            m_nRetCode = SEARCH_RETURN.emCANCEL

            '-------------------------------------------------------------------
            ' ���C�A�E�g�̏�����
            '-------------------------------------------------------------------
            txtCoil.Text = ""                                                   ' ���������YNo
            txtKizukenNo.Text = ""                                              ' ����������No
            txtKanriNo.Text = ""                                                ' �������Ǘ�No
            lblGuide.Text = ""                                                  ' �|�b�v�A�b�v���b�Z�[�W
            dgvInitialize(dgvResult)                                            ' ����������

            chkDateSet.Checked = True                                           ' ���t�t�B���^�[������
            rdoSortDesc.Checked = True                                          ' �������ʂ��~���ɂ���
            chkKizuNon.Checked = True                                           ' �r0�R�̂��͕̂\�����Ȃ�
            chkNagate.Checked = False
            radDspLikeFFsnd.Visible = True
            Me.tProp_Moveable = True                                            ' ��ʈړ�����

            '-------------------------------------------------------------------
            ' ���N���X�ݒ�
            '-------------------------------------------------------------------
            '''' �t�B���^�����ݒ�
            '' �r��E�O���[�h�t�B���^�[�̏������N���X�ɃZ�b�g
            MyBase.tProp_SettingIniName = LOCALINI_NAME
            MyBase.tProp_SettingIniSession = SO_GAMENN

            Call MyBase.SetFilterData(dgvDispType, DB_TYPE_NAME)                ' �r��`�F�b�N���X�g������
            Call MyBase.ReadFilterData(dgvDispType, DB_TYPE_NAME)               ' �t�B���^�[�ۑ��l���f
            Call MyBase.SetFilterData(dgvDispGrade, DB_GRADE_NAME)              ' �O���[�h�`�F�b�N���X�g������
            Call MyBase.ReadFilterData(dgvDispGrade, DB_GRADE_NAME)             ' �t�B���^�[�ۑ��l���f

            '-------------------------------------------------------------------
            ' �t�@���N�V�����L�[�ݒ�
            '-------------------------------------------------------------------
            AddFunctionButton(btnSearch, 5 - 1)
            AddFunctionButton(btnSelect, 6 - 1)
            AddFunctionButton(btnEnd, 12 - 1)
            '-------------------------------------------------------------------
            ' �\���T�C�Y�ݒ�
            '-------------------------------------------------------------------
            Dim clsMonitor() As Screen = Screen.AllScreens()                    '���j�^�[�ꗗ
            Dim nFormPos As Integer = g_nFormPos
            If clsMonitor.Length < nFormPos Then                                '�\�����������j�^�[���Ȃ��ꍇ
                nFormPos = 1                                                    '�v���C�}���f�B�X�v���C�ɕ\��
            End If
            Dim nH As Integer = clsMonitor(nFormPos - 1).Bounds.Size.Height
            Dim nW As Integer = clsMonitor(nFormPos - 1).Bounds.Size.Width

            '�t�H�[���T�C�Y�����j�^�T�C�Y���傫���Ƃ����j�^�T�C�Y�ɍ��킹��
            If Me.Height > nH Then
                Me.Height = nH
            End If

            '�^�C�g���ݒ�
            Call SetTitleName(Me)

        Catch ex As Exception
            log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʐ����ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �������� �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)

        Try
            With dgv
                .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[
                .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �Z���I��
                .ColumnHeadersHeight = 38                                       ' ��w�b�_��������
                .DefaultCellStyle.WrapMode = DataGridViewTriState.False

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
                '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
                .DefaultCellStyle().SelectionForeColor = Color.Black            '
                .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
                .DefaultCellStyle().ForeColor = Color.Black
                .RowCount = 0

                For col As EM_RESULT_COL = EM_RESULT_COL.nDATE To EM_RESULT_COL.MAX_RESULT_COL
                    Select Case col
                        Case EM_RESULT_COL.nDATE
                            ' �ʔ���
                            dgvColSetText(dgv, "�ʔ���", 178, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nCoilNo
                            ' ���YNo
                            dgvColSetText(dgv, "���YNo", 120, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nOrderNo
                            ' ����No
                            dgvColSetText(dgv, "����No", 80, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMaterial
                            ' �ގ�
                            dgvColSetText(dgv, "�ގ�", 50, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nHyomen
                            ' �\�ʏ���
                            dgvColSetText(dgv, "�\�ʏ���", 50, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nCoilLen
                            ' �R�C����
                            dgvColSetText(dgv, "�R�C����" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nProConLen
                            ' ��ۺݒ�
                            dgvColSetText(dgv, "��ۺݒ�" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDspLen
                            ' �\����
                            dgvColSetText(dgv, "�\����" & vbCrLf & "[m]", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nAtu
                            ' �ޗ���
                            dgvColSetText(dgv, "�ޗ���" & vbCrLf & "[mm]", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nWid
                            ' �ޗ���
                            dgvColSetText(dgv, "�ޗ���" & vbCrLf & "[mm]", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaKijun_O
                            ' �����
                            dgvColSetText(dgv, "����" & vbCrLf & "�", 60, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        'Case EM_RESULT_COL.nKensaKijun_U
                        '    ' �����
                        '    dgvColSetText(dgv, "�����" & vbCrLf & "(��)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMixedRate
                            ' ������
                            dgvColSetText(dgv, "������[%]" & vbCrLf & "(�\/��)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nMuke
                            ' ����
                            dgvColSetText(dgv, "����", 300, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nHinCode
                            ' �i��
                            dgvColSetText(dgv, "�i��", 50, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nNextCode
                            ' �v�掟�H��
                            dgvColSetText(dgv, "�v��" & vbCrLf & "���H��", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_O
                            ' �\���r��(�\)
                            dgvColSetText(dgv, "�\���r" & vbCrLf & "��(�\)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_U
                            ' �\���r��(�\)
                            dgvColSetText(dgv, "�\���r" & vbCrLf & "��(��)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDefCnt_Total
                            ' �\���r��(�\)
                            dgvColSetText(dgv, "�\���r" & vbCrLf & "����", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmfulCnt_O
                            ' �\�폜�L�Q�r��
                            dgvColSetText(dgv, "�폜�L�Q" & vbCrLf & "��(�\)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmfulCnt_U
                            ' ���폜�L�Q�r��
                            dgvColSetText(dgv, "�폜�L�Q" & vbCrLf & "��(��)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmlessCnt_O
                            ' �\�폜���Q�r��
                            dgvColSetText(dgv, "�폜���Q" & vbCrLf & "��(�\)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nDelHarmlessCnt_U
                            ' ���폜���Q�r��
                            dgvColSetText(dgv, "�폜���Q" & vbCrLf & "��(��)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSaveKizu_O
                            ' �\�ۑ��r��
                            dgvColSetText(dgv, "�ۑ��r" & vbCrLf & "��(�\)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSaveKizu_U
                            ' ���ۑ��r��
                            dgvColSetText(dgv, "�ۑ��r" & vbCrLf & "��(��)", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCode
                            ' �����p�^�[��
                            dgvColSetText(dgv, "����" & vbCrLf & "�����", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nSCode
                            ' �����\�ʏ��
                            dgvColSetText(dgv, "����" & vbCrLf & "�\�ʏ��", 100, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKanriNo
                            ' �Ǘ�No
                            dgvColSetText(dgv, "�r���Ǘ�No", 160, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nUntenStat
                            ' �^�]���
                            dgvColSetText(dgv, "�^�]���", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKakuno
                            ' �i�[�敪
                            dgvColSetText(dgv, "�i�[�敪", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaStat_O
                            ' �������(�\)
                            dgvColSetText(dgv, "�������" & vbCrLf & "(�\)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKensaStat_U
                            ' �������(��)
                            dgvColSetText(dgv, "�������" & vbCrLf & "(��)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKikiStat_O
                            ' �@����(�\)
                            dgvColSetText(dgv, "�@����" & vbCrLf & "(�\)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKikiStat_U
                            ' �@����(��)
                            dgvColSetText(dgv, "�@����" & vbCrLf & "(��)", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nToyuSplr
                            ' �h���X�v���[���u���
                            dgvColSetText(dgv, "�h�����ڰ" & vbCrLf & "���u���", 96, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCCoil
                            'P/C���
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "���", "���", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCCut
                            'P/C���
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "���", "���", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCSaisun
                            'P/C�̐�
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "�̐�", "�̐�", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nPCZiseki
                            'P/C����
                            dgbColSetButton(dgv, "P/C" & vbCrLf & "����", "����", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nFFRecv
                            'P/C����
                            dgbColSetButton(dgv, "FF" & vbCrLf & "��M", "FF��", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nFFSend
                            'P/C����
                            dgbColSetButton(dgv, "FF" & vbCrLf & "���M", "FF��", 60, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_RESULT_COL.nKeyInfo
                            ' �L�[���(��\��)
                            dgvColSetText(dgv, "�L�[���", 200, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)
                    End Select
                Next
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʌ��ʗ��������ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �R�C������SQL����
    ''' </summary>
    ''' <returns>�R�C������SQL����</returns>
    ''' <remarks></remarks>
    Private Function GetSQLSearchCoil() As String
        Dim strSQL As String = ""       ' �R�C������SQL
        Dim strFilter As String = ""    ' �r��E�O���[�h�`�F�b�N���X�g����
        Dim bWk As Boolean = True       ' �\�ʏ�Ԍ����t�B���^���ڒǉ�

        Try
            '���׏��t�B���^�[
            With dgvDispType
                For ii As Integer = 0 To .RowCount - 1
                    If Not CType(dgvDispType(EM_FILTER_COL.emCheck, ii).Value, Boolean) Then
                        strFilter &= " AND NOT d.�����r�� = " & dgvDispType(EM_FILTER_COL.emEdasysID, ii).Value.ToString
                    End If
                Next ii
            End With

            '�O���[�h�t�B���^�[
            With dgvDispGrade
                For ii As Integer = 0 To .RowCount - 1
                    If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, ii).Value, Boolean) Then
                        strFilter &= " AND NOT d.����O���[�h = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, ii).Value.ToString
                    End If
                Next ii
            End With

            strSQL = ""
            '-------------------------------------------------------------------
            ' SELECT��
            '-------------------------------------------------------------------
            strSQL &= "SELECT"
            strSQL &= " RES.�o�^���t as �ʔ��t"
            strSQL &= " ,ISNULL(INF.���YNo,'') as ���YNo"
            strSQL &= " ,ISNULL(INF.����No,'') as ����No"
            strSQL &= " ,ISNULL(INF.��ގ�,'') as ��ގ�"
            strSQL &= " ,ISNULL(INF.�\�ʏ���1����,'') as �\�ʏ���"
            strSQL &= " ,ISNULL(INF.�i��,'') as �i��"
            strSQL &= " ,ISNULL(RES.����������,0) as ����������"
            strSQL &= " ,ISNULL(RES.�v���R��������,0) as �v���R��������"
            strSQL &= " ,ISNULL(RES.�\��������,0) as �\��������"
            strSQL &= " ,ISNULL(INF.�ޗ���,0) as �ޗ���"
            strSQL &= " ,ISNULL(INF.�ޗ���,0) as �ޗ���"
            strSQL &= " ,ISNULL(INF.�����_�\,'') as �����_�\"
            strSQL &= " ,ISNULL(INF.�����_��,'') as �����_��"
            strSQL &= " ,ISNULL(CNT_T.��, 0) as �\���r���\"
            strSQL &= " ,ISNULL(CNT_B.��, 0) as �\���r����"
            strSQL &= " ,ISNULL(INF.����,'') as ����"
            strSQL &= " ,ISNULL(INF.�v�掟�H���R�[�h,'') as �v�掟�H���R�[�h"
            'strSQL &= " ,ISNULL(RES.�\�ۑ��Ώ��r��,0) as �\�ۑ��Ώ��r��"
            'strSQL &= " ,ISNULL(RES.���ۑ��Ώ��r��,0) as ���ۑ��Ώ��r��"
            strSQL &= " ,ISNULL(RES.�\�폜�L�Q�r��,0) as �\�폜�L�Q�r��"
            strSQL &= " ,ISNULL(RES.���폜�L�Q�r��,0) as ���폜�L�Q�r��"
            strSQL &= " ,ISNULL(RES.�\�폜���Q�r��,0) as �\�폜���Q�r��"
            strSQL &= " ,ISNULL(RES.���폜���Q�r��,0) as ���폜���Q�r��"
            strSQL &= " ,ISNULL(RES.�\�ۑ��r��,0) as �\�ۑ��r��"
            strSQL &= " ,ISNULL(RES.���ۑ��r��,0) as ���ۑ��r��"

            strSQL &= " ,ISNULL(RES.�����p�^�[���R�[�h_�\,'') as �����p�^�[��_�\"
            strSQL &= " ,ISNULL(RES.�����p�^�[���R�[�h_��,'') as �����p�^�[��_��"
            strSQL &= " ,ISNULL(RES.�����\�ʏ��No_�\,'') as �����\�ʏ��_�\"
            strSQL &= " ,ISNULL(RES.�����\�ʏ��No_��,'') as �����\�ʏ��_��"

            strSQL &= " ,ISNULL(RES.�^�]���,0) as �^�]���"
            strSQL &= " ,ISNULL(RES.���ъi�[�敪,0) as ���ъi�[�敪"

            strSQL &= " ,ISNULL(RES.�������_�\,0) as �������_�\"
            strSQL &= " ,ISNULL(RES.�������_��,0) as �������_��"
            strSQL &= " ,ISNULL(RES.�@����_�\,0) as �@����_�\"
            strSQL &= " ,ISNULL(RES.�@����_��,0) as �@����_��"
            strSQL &= " ,ISNULL(RES.�h���X�v���[���u���,0) as �h���X�v���[���u���"

            strSQL &= " ,ISNULL(INF.�L�[���,'') as �L�[���"
            strSQL &= " ,ISNULL(RES.�␳��,1) as �␳��"
            strSQL &= " ,RES.�Ǘ�No"

            '-------------------------------------------------------------------
            ' FROM�� �o�������R�C�����уe�[�u��
            '-------------------------------------------------------------------
            strSQL &= " FROM " & DB_COIL_RESULT & " RES "

            '-------------------------------------------------------------------
            ' �O������ �R�C�����
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " INF "
            strSQL &= " ON RES.�Ǘ�No = INF.�Ǘ�No "

            '-------------------------------------------------------------------
            ' �O������  �\ �\���r��
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN (SELECT count(*) as ��, d.�Ǘ�No from "
            'strSQL &= DB_DEFECT_INF_T & " d "

            If radDspLikeFFsnd.Checked Then
                'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " df "
                strSQL &= DB_ACCESS_NEXT_DEFECT_INF & " df , "
            End If

            strSQL &= DB_DEFECT_INF_T & " d "

            strSQL &= " WHERE d.�d������ <> 1 "

            If radDspLikeFFsnd.Checked Then
                strSQL &= "AND ((df.�Ǘ�No = d.�Ǘ�No) AND (df.�rNo=d.�rNo)) "
            End If

            '�T����ʂƓ���\���`�F�b�N�I��
            If radDspLikeTansho.Checked Then
                strSQL &= "AND d.�\���Ώۋ敪_�T�����=1 "
            End If

            '�t�B���^�[�\�������`�F�b�N�I��
            If radNormalDsp.Checked Then
                strSQL &= strFilter
            End If

            strSQL &= " Group by d.�Ǘ�No ) CNT_T "
            strSQL &= " ON RES.�Ǘ�No = CNT_T.�Ǘ�No "

            '-------------------------------------------------------------------
            ' �O������  �� �\���r��
            '-------------------------------------------------------------------
            strSQL &= " LEFT OUTER JOIN (SELECT count(*) as ��, d.�Ǘ�No from "
            'strSQL &= DB_DEFECT_INF_B & " d "

            If radDspLikeFFsnd.Checked Then
                'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " df "
                strSQL &= DB_ACCESS_NEXT_DEFECT_INF & " df , "
            End If

            strSQL &= DB_DEFECT_INF_B & " d "

            strSQL &= " WHERE d.�d������ <> 1 "

            If radDspLikeFFsnd.Checked Then
                strSQL &= "AND ((df.�Ǘ�No = d.�Ǘ�No) AND (df.�rNo=d.�rNo)) "
            End If

            '�T����ʂƓ���\���`�F�b�N�I��
            If radDspLikeTansho.Checked Then
                strSQL &= "AND d.�\���Ώۋ敪_�T�����=1 "
            End If

            '�t�B���^�[�\�������`�F�b�N�I��
            If radNormalDsp.Checked Then
                strSQL &= strFilter
            End If

            strSQL &= " Group by d.�Ǘ�No ) CNT_B "
            strSQL &= " ON RES.�Ǘ�No = CNT_B.�Ǘ�No "

            '-------------------------------------------------------------------
            ' ������
            '-------------------------------------------------------------------
            strSQL &= " WHERE 1 = 1"                                            ' �擪�̓R��

            '�R�C������
            If "" <> txtCoil.Text.Trim Then                                     ' ���YNo
                strSQL = strSQL & " and INF.���YNo like '" & txtCoil.Text.Trim & "' "
            End If

            If "" <> txtKizukenNo.Text.Trim Then                                ' ����No
                strSQL = strSQL & " and INF.����No like '" & txtKizukenNo.Text.Trim & "' "
            End If

            If "" <> txtKanriNo.Text.Trim Then                                ' �Ǘ�No
                strSQL = strSQL & " and RES.�Ǘ�No like '" & txtKanriNo.Text.Trim & "' "
            End If

            If chkDateSet.Checked Then                                          ' �����i�J�n�j
                strSQL = strSQL & " and RES.�o�^���t >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
                If chkTimeSet.Checked = True Then                               ' ���Ԏw�莞
                    strSQL &= " " & Format(dtpTimeStart.Value, "HH:mm:ss") & "' "
                Else
                    strSQL &= " " & "00:00:00' "
                End If
            End If

            If chkDateSet.Checked Then                                          ' �����i�I���j
                strSQL = strSQL & " and RES.�o�^���t <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
                If chkTimeSet.Checked = True Then                               ' ���Ԏw�莞
                    strSQL &= " " & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' "
                Else
                    strSQL &= "23:59:59' "
                End If
            End If

            '-------------------------------------------------------------------
            ' �I�[�_�[��
            '-------------------------------------------------------------------
            If rdoSortAsc.Checked = True Then
                strSQL &= " ORDER BY RES.�o�^���t ASC"
            Else
                strSQL &= " ORDER BY RES.�o�^���t DESC"
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃR�C������SQL�����ُ� [{0}] SQL={1}", ex.Message, strSQL))
            strSQL = ""
        End Try

        Return strSQL

    End Function


    ''' <summary>
    ''' �R�C������ �� �������ʕ\��
    ''' </summary>
    ''' <param name="strSQL">����SQL</param>
    ''' <remarks></remarks>
    Private Sub SearchCoil(ByVal strSQL As String)
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim nRow As Integer = 0

        Try
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim bDbRet As Boolean = False

            bDbRet = clsDB.Open()                                               ' DB�Ƃ̐ڑ��J�n
            If bDbRet = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃR�C������ DB�ڑ����s"))
                Exit Sub
            End If

            '�R�}���h���s�^�C���A�E�g���ԃZ�b�g
            clsDB.SetCommandTimeOut(m_nSearchTimeOut)
            read = clsDB.SelectExecute(strSQL)                                  ' �R�C���������s
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃR�C������ DB�f�[�^�擾���s"))
                Exit Sub
            End If

            dgvResult.RowCount = 0

            With read
                While .Read() = True
                    Dim nCoilLen As Integer = CInt(.GetSqlInt32(.GetOrdinal("����������")))
                    Dim nKizuNum_O As Integer = CInt(.GetSqlInt32(.GetOrdinal("�\���r���\")))
                    Dim nKizuNum_U As Integer = CInt(.GetSqlInt32(.GetOrdinal("�\���r����")))
                    Dim dCorrect As Double = CDbl(.GetDouble(.GetOrdinal("�␳��")))

                    If chkKizuNon.Checked Then
                        ' 0�����т�\�����Ȃ�
                        If 0 = nKizuNum_O And 0 = nKizuNum_U Then
                            Continue While
                        End If
                    End If
                    If chkCorrect.Checked Then
                        '�␳���g�p
                        nCoilLen = CInt(nCoilLen * dCorrect)
                    End If

                    dgvResult.RowCount += 1

                    ' �ʔ��t
                    dgvResult(EM_RESULT_COL.nDATE, nRow).Value = .GetDateTime(.GetOrdinal("�ʔ��t")).ToString("yyyy/MM/dd HH:mm:ss")
                    ' ���YNo
                    dgvResult(EM_RESULT_COL.nCoilNo, nRow).Value = .GetSqlValue(.GetOrdinal("���YNo")).ToString().Trim
                    ' ����No
                    dgvResult(EM_RESULT_COL.nOrderNo, nRow).Value = .GetSqlValue(.GetOrdinal("����No")).ToString().Trim
                    ' �ގ�
                    dgvResult(EM_RESULT_COL.nMaterial, nRow).Value = .GetSqlValue(.GetOrdinal("��ގ�")).ToString().Trim
                    ' �\�ʏ���
                    dgvResult(EM_RESULT_COL.nHyomen, nRow).Value = .GetSqlValue(.GetOrdinal("�\�ʏ���")).ToString().Trim
                    ' �R�C����
                    dgvResult(EM_RESULT_COL.nCoilLen, nRow).Value = Format(CSng(nCoilLen / 1000), "#0.0")
                    ' �v���R����
                    dgvResult(EM_RESULT_COL.nProConLen, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("�v���R��������"))) / 1000), "#0.0")
                    ' �\����
                    dgvResult(EM_RESULT_COL.nDspLen, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("�\��������"))) / 1000), "#0.0")
                    ' �ޗ���
                    dgvResult(EM_RESULT_COL.nAtu, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("�ޗ���"))) / 1000), "#0.000")
                    ' �ޗ���
                    dgvResult(EM_RESULT_COL.nWid, nRow).Value = Format(CSng(CInt(.GetSqlInt32(.GetOrdinal("�ޗ���")))), "#0")
                    ' �����(�\)
                    dgvResult(EM_RESULT_COL.nKensaKijun_O, nRow).Value = .GetSqlValue(.GetOrdinal("�����_�\")).ToString().Trim
                    ' ������[%](�\/��)
                    dgvResult(EM_RESULT_COL.nMixedRate, nRow).Value = String.Format("{0:0.0}/{1:0.0}",
                                                                        nKizuNum_O / CSng(nCoilLen / 1000) * 100,
                                                                        nKizuNum_U / CSng(nCoilLen / 1000) * 100)
                    ' ����
                    dgvResult(EM_RESULT_COL.nMuke, nRow).Value = .GetSqlValue(.GetOrdinal("����")).ToString().Trim
                    ' �i��
                    dgvResult(EM_RESULT_COL.nHinCode, nRow).Value = .GetSqlValue(.GetOrdinal("�i��")).ToString().Trim
                    ' �v�掟�H��
                    dgvResult(EM_RESULT_COL.nNextCode, nRow).Value = .GetSqlValue(.GetOrdinal("�v�掟�H���R�[�h")).ToString().Trim
                    ' �\���r��(�\)
                    dgvResult(EM_RESULT_COL.nDefCnt_O, nRow).Value = Format(nKizuNum_O, "#,##0")
                    ' �\���r��(��)
                    dgvResult(EM_RESULT_COL.nDefCnt_U, nRow).Value = Format(nKizuNum_U, "#,##0")
                    ' �\���r����
                    dgvResult(EM_RESULT_COL.nDefCnt_Total, nRow).Value = Format(nKizuNum_O + nKizuNum_U, "#,##0")
                    ' �\�폜�L�Q�r��
                    dgvResult(EM_RESULT_COL.nDelHarmfulCnt_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("�\�폜�L�Q�r��"))), "#,##0")
                    ' ���폜�L�Q�r��
                    dgvResult(EM_RESULT_COL.nDelHarmfulCnt_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("���폜�L�Q�r��"))), "#,##0")
                    ' �\�폜���Q�r��
                    dgvResult(EM_RESULT_COL.nDelHarmlessCnt_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("�\�폜���Q�r��"))), "#,##0")
                    ' ���폜���Q�r��
                    dgvResult(EM_RESULT_COL.nDelHarmlessCnt_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("���폜���Q�r��"))), "#,##0")
                    ' �\�ۑ��r��
                    dgvResult(EM_RESULT_COL.nSaveKizu_O, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("�\�ۑ��r��"))), "#,##0")
                    ' ���ۑ��r��
                    dgvResult(EM_RESULT_COL.nSaveKizu_U, nRow).Value = Format(CInt(.GetSqlInt32(.GetOrdinal("���ۑ��r��"))), "#,##0")
                    ' �����p�^�[��
                    dgvResult(EM_RESULT_COL.nPCode, nRow).Value = CInt(.GetSqlInt32(.GetOrdinal("�����p�^�[��_�\"))).ToString & "/" & CInt(.GetSqlInt32(.GetOrdinal("�����p�^�[��_��"))).ToString
                    ' �\�ʏ��
                    dgvResult(EM_RESULT_COL.nSCode, nRow).Value = CInt(.GetSqlInt32(.GetOrdinal("�����\�ʏ��_�\"))).ToString & "/" & CInt(.GetSqlInt32(.GetOrdinal("�����\�ʏ��_��"))).ToString
                    ' �Ǘ�No
                    dgvResult(EM_RESULT_COL.nKanriNo, nRow).Value = .GetSqlValue(.GetOrdinal("�Ǘ�No"))
                    ' �^�]���
                    dgvResult(EM_RESULT_COL.nUntenStat, nRow).Value =
                        KizuForm.ucStatusAria.DspUnten(CType(.GetSqlInt32(.GetOrdinal("�^�]���")), KizuForm.EM_DIV_UNTEN))
                    ' �i�[�敪
                    Dim strWk As String = "-"
                    Select Case .GetSqlInt32(.GetOrdinal("���ъi�[�敪"))
                        Case 1
                            strWk = "�ʏ�ؑ�"
                        Case 2
                            strWk = "�ޯ�����"
                        Case 3
                            strWk = "�蓮��~"
                        Case 4
                            strWk = "������~"
                    End Select
                    dgvResult(EM_RESULT_COL.nKakuno, nRow).Value = strWk
                    ' �������(�\)
                    dgvResult(EM_RESULT_COL.nKensaStat_O, nRow).Value =
                        KizuForm.ucStatusAria.DspKensa(CType(.GetSqlInt32(.GetOrdinal("�������_�\")), KizuForm.EM_DIV_KENSA))
                    ' �������(��)
                    dgvResult(EM_RESULT_COL.nKensaStat_U, nRow).Value =
                        KizuForm.ucStatusAria.DspKensa(CType(.GetSqlInt32(.GetOrdinal("�������_��")), KizuForm.EM_DIV_KENSA))
                    ' �@����(�\)
                    dgvResult(EM_RESULT_COL.nKikiStat_O, nRow).Value =
                        KizuForm.ucStatusAria.DspKiki(CType(.GetSqlInt32(.GetOrdinal("�@����_�\")), KizuForm.EM_DIV_KIKI))
                    ' �@����(��)
                    dgvResult(EM_RESULT_COL.nKikiStat_U, nRow).Value =
                        KizuForm.ucStatusAria.DspKiki(CType(.GetSqlInt32(.GetOrdinal("�@����_��")), KizuForm.EM_DIV_KIKI))
                    ' �h���X�v���[���u���
                    strWk = "-"
                    Select Case .GetSqlInt32(.GetOrdinal("�h���X�v���[���u���"))
                        Case 0
                            strWk = "�����t"
                        Case 1
                            strWk = "���t"
                    End Select
                    dgvResult(EM_RESULT_COL.nToyuSplr, nRow).Value = strWk
                    ' �L�[���
                    dgvResult(EM_RESULT_COL.nKeyInfo, nRow).Value = .GetSqlValue(.GetOrdinal("�L�[���")).ToString().Trim

                    ''�J�E���g�ǉ�
                    nRow += 1

                End While
            End With

        Catch ex As Exception

        End Try
        If nRow <> 0 Then
            lblGuide.Text = String.Format("{0}��������܂���", nRow)
        Else
            lblGuide.Text = "�f�[�^��������܂���ł���"
        End If

    End Sub

    ''' <summary>
    ''' ���̃R�C����ǂݍ��܂�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadNextCoil()
        Try
            If ExistNext() = True Then
                m_nSelectRowNo += 1
                dgvResult(EM_RESULT_COL.nOrderNo, m_nSelectRowNo).Selected = True
                Visible = True
                ReadMap()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���̎��уf�[�^�ǂݍ��ُ݈� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �O�̃R�C����ǂݍ��܂�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPreviousCoil()
        Try
            If ExistPrevious() = True Then
                m_nSelectRowNo -= 1
                dgvResult(EM_RESULT_COL.nOrderNo, m_nSelectRowNo).Selected = True
                Visible = True
                ReadMap()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�O�̎��уf�[�^�ǂݍ��ُ݈� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ���ɓǂݍ��ރR�C�������邩�擾���܂�
    ''' </summary>
    ''' <returns>True:���̃R�C�������� False:����܂���</returns>
    ''' <remarks></remarks>
    Public Function ExistNext() As Boolean
        If m_nSelectRowNo < (dgvResult.RowCount - 1) Then
            Return True
        Else
            Return False
        End If
    End Function

    ''' <summary>
    ''' �O�ɓǂݍ��ރR�C�������邩�擾���܂�
    ''' </summary>
    ''' <returns>True:�O�̃R�C�������� False:����܂���</returns>
    ''' <remarks></remarks>
    Public Function ExistPrevious() As Boolean
        If 0 < m_nSelectRowNo Then
            Return True
        Else
            Return False
        End If
    End Function

    ''' <summary>
    ''' ���уf�[�^�ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadMap()
        Try
            If dgvResult.ColumnCount = 0 Then
                Exit Sub    ' �������ʂ������ꍇ�͏������Ȃ�
            End If

            Dim nRow As Integer = dgvResult.CurrentRow.Index

            '-------------------------------------------------------------------
            '�ǂݍ��݃p�����[�^�̍쐬
            '-------------------------------------------------------------------
            Dim stReadParam As clsDataMap.READ_PARAM = Nothing
            With stReadParam
                .strKanriNo = dgvResult(EM_RESULT_COL.nKanriNo, nRow).Value.ToString()
                .bDS = chkHaba.Checked
                .bNagate = chkNagate.Checked
                .nTidList = New List(Of Integer)
                .nGidList = New List(Of Integer)
                .bTansho = radDspLikeTansho.Checked
                .bFFSnd = radDspLikeFFsnd.Checked
                .bCorrect = chkCorrect.Checked

                For row As Integer = 0 To dgvDispType.RowCount - 1              ' �\���ΏۊO���r�탊�X�g���쐬
                    If CBool(dgvDispType(EM_FILTER_COL.emCheck, row).Value) = False Then
                        .nTidList.Add(CInt(dgvDispType(EM_FILTER_COL.emEdasysID, row).Value))
                    End If
                Next

                For row As Integer = 0 To dgvDispGrade.RowCount - 1             ' �\���ΏۊO�̃O���[�h���X�g���쐬
                    If CBool(dgvDispGrade(EM_FILTER_COL.emCheck, row).Value) = False Then
                        .nGidList.Add(CInt(dgvDispGrade(EM_FILTER_COL.emEdasysID, row).Value))
                    End If
                Next
            End With

            '-------------------------------------------------------------------
            ' ���уf�[�^�ǂݍ���
            '-------------------------------------------------------------------
            mcls_ReadDatamap = New clsDataMap(mcls_Param, mcls_PreParam, mcls_Log)  ' �������J���͎��щ�ʂł��肢���܂��B
            mcls_ReadDatamap.Interval(EM_LONG_POS.SMALL) = mcls_Param.MapStageSmall
            mcls_ReadDatamap.Interval(EM_LONG_POS.BIG) = mcls_Param.MapStageBig
            AddHandler mcls_ReadDatamap.StartDataNti, AddressOf clsDataMap_StartData
            AddHandler mcls_ReadDatamap.ReadDataNti, AddressOf clsDataMap_ReadData

            SetEnableControl(False)
            Dim bRet As Boolean = mcls_ReadDatamap.Read(stReadParam)
            SetEnableControl(True)

            If bRet = False Then
                If mcls_ReadDatamap.IsReadCancel = False Then                   ' ���[�U�[���L�����Z�����Ă��Ȃ�
                    lblGuide.Text = "���уf�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If
            Else
                mcls_DataMap = mcls_ReadDatamap                                 ' ����̌������ʂ�ݒ肷��
                m_nRetCode = SEARCH_RETURN.emSELECT_COIL
                m_nSelectRowNo = dgvResult.CurrentRow.Index
                Me.Hide()                                                       ' �����ď�����
            End If

            '-------------------------------------------------------------------
            ' �㏈��
            '-------------------------------------------------------------------
            stReadParam.nGidList.Clear()
            stReadParam.nGidList = Nothing
            stReadParam.nTidList.Clear()
            stReadParam.nTidList = Nothing

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���уf�[�^�ǂݍ��ُ݈� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �R���g���[������ېݒ�
    ''' </summary>
    ''' <param name="bVisible">True:����� False:����s��</param>
    ''' <remarks></remarks>
    Private Sub SetEnableControl(ByVal bVisible As Boolean)
        pnlMsg.Visible = Not bVisible                                           ' ���b�Z�[�W�\�����邽�ߗL���ɂ���
        btnEnd.Enabled = bVisible
        btnSearch.Enabled = bVisible
        btnSelect.Enabled = bVisible
        dgvResult.Enabled = bVisible
    End Sub

    '***************************************************************************
    ' ���ѓǂݍ��݃C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �}�b�v�f�[�^�ǂݍ��݊J�n�C�x���g
    ''' </summary>
    ''' <param name="nKind">�f�[�^���</param>
    ''' <param name="nMax">�ǂݍ��݌���</param>
    ''' <remarks></remarks>
    Private Sub clsDataMap_StartData(ByVal nKind As clsDataMap.DATA_KIND, ByVal nMax As Integer)
        Try
            Select Case nKind
                Case clsDataMap.DATA_KIND.emCOIL_POS
                    txtMsg.Text = "�R�C���ʒu���擾��"
                Case clsDataMap.DATA_KIND.emCOIL_INF
                    txtMsg.Text = "�r���擾��"
                Case clsDataMap.DATA_KIND.emCUT_INF
                    txtMsg.Text = "�J�b�g���擾��"
            End Select

            prbMsg.Minimum = 0
            prbMsg.Maximum = CInt(IIf(nMax > prbMsg.Minimum, nMax, prbMsg.Minimum + 1))
            prbMsg.Value = 0
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ѓǂݍ��݊J�n�C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �}�b�v�f�[�^�ǂݍ��݃C�x���g
    ''' </summary>
    ''' <param name="nKind">�f�[�^���</param>
    ''' <param name="nCount">�ǂݍ��݌���</param>
    ''' <remarks></remarks>
    Private Sub clsDataMap_ReadData(ByVal nKind As clsDataMap.DATA_KIND, ByVal nCount As Integer)
        Try
            prbMsg.Value = CInt(IIf(nCount < prbMsg.Maximum, nCount, prbMsg.Maximum))
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ѓǂݍ��݃C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �t�H�[���C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �����j��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSearch_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʎ�������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frmSearch_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 5 - 1                                                      ' �R�C������
                    If btnSearch.Enabled Then
                        btnSearch_Click(Nothing, Nothing)
                    End If
                    '-----------------------------------------------------------
                Case 6 - 1                                                      ' �I��
                    btnSelect_Click(Nothing, Nothing)
                    '-----------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃt�@���N�V�����C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub
    '***************************************************************************
    ' �{�^���C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            If txtCoil.Text.Trim = "" And txtKizukenNo.Text.Trim = "" And txtKanriNo.Text.Trim = "" And
             Not chkDateSet.Checked And Not chkTimeSet.Checked Then             '�����������w�肳��Ă��Ȃ���΁A�G���[
                lblGuide.Text = "�����L�[����͂��Ă�������"
                Exit Sub
            End If

            Me.Cursor = Cursors.WaitCursor
            Dim strSQL As String = GetSQLSearchCoil()
            If strSQL.Length > 0 Then
                SearchCoil(strSQL)
            End If
            Me.Cursor = Cursors.Arrow
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʌ����{�^�������C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' �L�����Z���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCancel_MouseClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles btnCancel.MouseClick
        Try
            If mcls_ReadDatamap Is Nothing = False Then
                mcls_ReadDatamap.CancelRead()                                   ' �ǂݍ��݃L�����Z��
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃL�����Z�������C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' ����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            m_nRetCode = SEARCH_RETURN.emCANCEL
            Me.Hide()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʕ���{�^�������C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �I���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try
            ReadMap()                                                           ' ���ѓǂݍ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʑI���{�^�������C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' CSV�{�^����������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsv.Click
        Try
            Call CsvOut(CSV_PATH, "�������ь������.csv", True)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь������CSV�o�̓{�^�������C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �}�E�X�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �}�E�X�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellDoubleClick
        Try
            If e.RowIndex < 0 Then                                              ' �I���s���s���Ȃ�I���
                Exit Sub
            End If

            ReadMap()                                                           ' ���ь���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���ь�����ʌ������ʃ_�u���N���b�N�C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' ���̑� ���ꏈ��
    '***************************************************************************
    ''' <summary>
    ''' ����t�B���^�[�I���Ȃ��`�F�b�N�ύX����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkNormalDsp_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles radNormalDsp.CheckedChanged

        ChangeKizuGradeFilter(radNormalDsp.Checked)

    End Sub


    '***************************************************************************
    ' ���̑� �����֐�
    '***************************************************************************
    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�e�L�X�g�{�b�N�X��ݒ�
    ''' </summary>
    ''' <param name="dgv">�R���g���[��</param>
    ''' <param name="sHeader">��w�b�_�[����</param>
    ''' <param name="nWidth">��</param>
    ''' <param name="bFrozen">���X�N���[�����̃��b�N</param>
    ''' <param name="bRead">�Ǎ���p</param>
    ''' <param name="nSort">���בւ����[�h</param>
    ''' <param name="nDefaultCellStyle">��X�^�C��</param>
    ''' <param name="nHeaderCellStyle">��w�b�_�[�X�^�C��</param>
    ''' <param name="sNullVal">�f�[�^��NULL���̕\�����e</param>
    ''' <param name="sDataNullVal">NULL���͎��̃f�[�^���e</param>
    ''' <param name="bDispVis">�\���E��\��</param>
    ''' <remarks></remarks>
    Private Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer,
     ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode,
     ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
     Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���X�N���[�����̃��b�N
            clm.Frozen = bFrozen
            ' �Ǎ���p�ݒ�
            clm.ReadOnly = bRead
            ' ���בւ����[�h
            clm.SortMode = nSort
            ' ��X�^�C��
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' ��w�b�_�[�X�^�C��
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' �\���E��\��
            clm.Visible = bDispVis
            ' �f�[�^��NULL���̕\�����e 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL���͎��̃f�[�^���e 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal

            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    Private Sub dgbColSetButton(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal sColumn As String, ByVal nWidth As Integer,
ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
Optional ByVal bDispVis As Boolean = True)
        Using clm As New DataGridViewButtonColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���בւ����[�h
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            ' ��X�^�C��
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' ��w�b�_�[�X�^�C��
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle

            clm.DefaultCellStyle.SelectionBackColor = SystemColors.Control
            clm.Visible = bDispVis
            '�{�^������
            clm.UseColumnTextForButtonValue = True
            clm.Text = sColumn

            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑� �������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ݒ�l�ۑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SaveSetting()
        ' �ۑ�
        Call MyBase.SaveFilterData(dgvDispType, DB_TYPE_NAME)
        Call MyBase.SaveFilterData(dgvDispGrade, DB_GRADE_NAME)
    End Sub




    ''' <summary>
    ''' �v���R���R�C������M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <remarks></remarks>
    Private Function RecvCheckProCoil(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�L�[���w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' �v���R���J�b�g���ю�M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function RecvCheckProCut(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�L�[���w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_CUT
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' �v���R���̐����ю�M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function RecvCheckProSaisun(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�L�[���w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' �v���R���������ё��M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SendCheckProZiseki(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�Ǘ�No�w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' FF�O�H���r����M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <remarks></remarks>
    Private Function RecvCheckFFRecv(ByVal strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�L�[���w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' FF�������ё��M�f�[�^���擾
    ''' </summary>
    ''' <param name="strKeyInfo">�L�[���</param>
    ''' <param name="nDataCnt">�f�[�^��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SendCheckFFSend(ByRef strKeyInfo As String, ByRef nDataCnt As Integer) As Boolean

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        lblGuide.Text = ""

        nDataCnt = 0

        strSQL = ""
        '�Ǘ�No�w��\��
        strSQL = ""
        strSQL &= " SELECT count(*)"
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE �L�[��� = '" & strKeyInfo & "' "

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
            Return False
        End If

        Try
            ' �f�[�^�擾
            If sqlRead.Read() Then
                '�f�[�^�J�E���g���Z�b�g
                nDataCnt = sqlRead.GetInt32(0)
            Else
                sqlRead.Close()
                sqlRead = Nothing
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
                Return False
            End If

        Catch ex As Exception
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
    ''' �������ʓ��̃{�^���������ꂽ�Ƃ��̏����i�`���Q�Ɖ�ʋN���j
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub
        Dim strKeyInfo As String = dgv(EM_RESULT_COL.nKeyInfo, e.RowIndex).Value.ToString.Trim
        Dim strKanriNo As String = dgv(EM_RESULT_COL.nKanriNo, e.RowIndex).Value.ToString.Trim
        Dim strID As String = ""
        Dim nDataCnt As Integer = 0

        If 0 = dgvResult.RowCount Then Exit Sub
        lblGuide.Text = ""

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex

            Case EM_RESULT_COL.nPCCoil              'P/C���
                Dim frm As frmCoilInfo
                If RecvCheckProCoil(strKeyInfo, nDataCnt) Then   '�v���R���R�C������M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmCoilInfo(1, strKanriNo)  '�v���R���R�C������M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "�R�C�����(P/C)��M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "�R�C�����(P/C)��M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If

            Case EM_RESULT_COL.nPCCut               'P/C���
                Dim frm As frmCut
                If RecvCheckProCut(strKeyInfo, nDataCnt) Then  '�v���R���J�b�g���ю�M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmCut(1, strKanriNo)  '�v���R���J�b�g���ю�M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "�J�b�g����(P/C)��M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "�J�b�g����(P/C)��M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If

            Case EM_RESULT_COL.nPCSaisun            'P/C�̐�
                Dim frm As frmSaisun
                If RecvCheckProSaisun(strKeyInfo, nDataCnt) Then  '�v���R���̐����ю�M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmSaisun(1, strKanriNo)  '�v���R���̐����ю�M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "�̐�����(P/C)��M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "�̐�����(P/C)��M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If

            Case EM_RESULT_COL.nPCZiseki            'P/C����
                Dim frm As frmMixedRateTool

                If SendCheckProZiseki(strKeyInfo, nDataCnt) Then        '�v���R���������ё��M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then                        '�f�[�^�L
                        lblGuide.Text = ""
                        frm = New frmMixedRateTool(1, strKanriNo)  '�v���R���������ё��M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "��������(P/C)���M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "��������(P/C)���M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If

            Case EM_RESULT_COL.nFFRecv            'FF�O�H���r����M
                Dim frm As frmFFRecv
                If RecvCheckFFRecv(strKeyInfo, nDataCnt) Then  'FF�O�H���r����M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then
                        lblGuide.Text = ""
                        frm = New frmFFRecv(1, strKanriNo)  'FF�O�H���r����M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "FF�O�H���r���(FF)��M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "FF�O�H���r���(FF)��M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If

            Case EM_RESULT_COL.nFFSend            'FF�������ё��M
                Dim frm As frmFF

                If SendCheckFFSend(strKeyInfo, nDataCnt) Then        'FF�������ё��M�f�[�^�`�F�b�N
                    If nDataCnt > 0 Then                        '�f�[�^�L
                        lblGuide.Text = ""
                        frm = New frmFF(1, strKanriNo)  'FF�������ё��M(=1�F�Ǘ�No�n��)
                        frm.ShowDialog()
                        frm.Dispose()
                    Else
                        lblGuide.Text = "FF��������(FF)���M�f�[�^��������܂���ł���"
                    End If
                Else
                    lblGuide.Text = "FF��������(FF)���M�f�[�^�̓ǂݍ��݂Ɏ��s���܂���"
                End If


        End Select
    End Sub

    ''' <summary>
    ''' �r��A�O���[�h�\���t�B���^�[�R���g���[���ύX
    ''' </summary>
    ''' <param name="bEnable">True:�L���AFalse:����</param>
    Private Sub ChangeKizuGradeFilter(ByVal bEnable As Boolean)

        Dim ii As Integer

        If bEnable Then
            '�ΐF�\���i�r��j
            For ii = 0 To dgvDispType.RowCount - 1
                If CBool(dgvDispType(0, ii).Value) Then
                    dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.LightGreen
                    dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGreen
                Else
                    dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
                    dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If
            Next ii

            '�ΐF�\���i�r��j
            For ii = 0 To dgvDispGrade.RowCount - 1
                If CBool(dgvDispGrade(0, ii).Value) Then
                    dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.LightGreen
                    dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGreen
                Else
                    dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.Empty
                    dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If
            Next ii
        Else
            '�D�F�\���i�r��j
            For ii = 0 To dgvDispType.RowCount - 1
                dgvDispType.Rows(ii).DefaultCellStyle.BackColor = Color.LightGray
                dgvDispType.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGray
            Next ii

            '�D�F�\���i�r��j
            For ii = 0 To dgvDispGrade.RowCount - 1
                dgvDispGrade.Rows(ii).DefaultCellStyle.BackColor = Color.LightGray
                dgvDispGrade.Rows(ii).DefaultCellStyle.SelectionBackColor = Color.LightGray
            Next ii
        End If

        ''�r��AGr���X�g
        dgvDispType.Enabled = bEnable
        dgvDispGrade.Enabled = bEnable
        ''�r��AGr�S�I���^�����{�^��
        btnTypeAllSelect.Enabled = bEnable
        btnTypeAllCancel.Enabled = bEnable
        btnGradeAllSelect.Enabled = bEnable
        btnGradeAllCancel.Enabled = bEnable

    End Sub

End Class
