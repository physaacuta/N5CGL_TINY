'*********************************************************************************
' �̏Ⴭ�O���
'	[Ver]
'		Ver.01    2007/06/29  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
'Imports KizuForm

''' <summary>
''' �̏Ⴭ�O���
''' </summary>
''' <remarks></remarks>


Public Class frmStatusLog
    Inherits SO_GAMENN.frmMain_StatusLog


    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��

            Case 3                                  ' �L�����Z��
                Call ExecCancel_Log()
            Case 4                                  ' ���O����
                Call ExecSelect_Log()
            Case 8                                  'CSV�o��
                Call CsvOut(CSV_PATH, "������ԃ��O���.csv")
            Case 9                                  '��ʈ��
                Call PrintOut()
            Case 10                                 'ʰ�޺�߰
                Call HardCopy(IMG_PATH, "������ԃ��O���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 11                                 ' �I��
                MyBase.ExecFromClose()
        End Select
    End Sub

    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub


    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmStatusLog_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        ' ���u��
        If NOA_ONLINE <> g_NOA Then
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' �t�H�[���T�C�Y�ύX�\
        End If


        '''' �t�B���^�[�ؑփ^�O����
        tabFilter_KUBUN.TabVisible = False

        '''' ���̃t�B���^�[�\��
        tabFilter_KUBUN.SelectedIndex = 0


        '''' �敪�t�B���^��`
        '''' �ғ����
        MyClass.AddCheckBoxKadKind(chkKad_Init, CStr(EM_DIV_KADOU.DIV_KADOU_INIT)) '������
        MyClass.AddCheckBoxKadKind(chkKad_Stp, CStr(EM_DIV_KADOU.DIV_KADOU_STOP)) '��~��
        MyClass.AddCheckBoxKadKind(chkKad_Kad, CStr(EM_DIV_KADOU.DIV_KADOU_START)) '�ғ���
        MyClass.AddCheckBoxKadKind(chkKad_Ret, CStr(EM_DIV_KADOU.DIV_KADOU_RETRY)) '���g���C��

        '''' �@���ԁi�\�j
        MyClass.AddCheckBoxKikiKind_O(chkKik_Ok_O, CStr(EM_DIV_KIKI.DIV_KIKI_OK)) '����
        MyClass.AddCheckBoxKikiKind_O(chkKik_Kei_O, CStr(EM_DIV_KIKI.DIV_KIKI_KEI)) '�y�̏�
        MyClass.AddCheckBoxKikiKind_O(chkKik_Jyu_O, CStr(EM_DIV_KIKI.DIV_KIKI_JYU))   '�d�̏�

        '''' �@���ԁi���j
        MyClass.AddCheckBoxKikiKind_U(chkKik_Ok_U, CStr(EM_DIV_KIKI.DIV_KIKI_OK)) '����
        MyClass.AddCheckBoxKikiKind_U(chkKik_Kei_U, CStr(EM_DIV_KIKI.DIV_KIKI_KEI)) '�y�̏�
        MyClass.AddCheckBoxKikiKind_U(chkKik_Jyu_U, CStr(EM_DIV_KIKI.DIV_KIKI_JYU))   '�d�̏�


        '''' ������ԁi�\�j
        MyClass.AddCheckBoxKenKind_O(chkKen_Stp_O, CStr(EM_DIV_KENSA.DIV_KENSA_NON)) '��~
        MyClass.AddCheckBoxKenKind_O(chkKen_Ok_O, CStr(EM_DIV_KENSA.DIV_KENSA_OK)) '����
        MyClass.AddCheckBoxKenKind_O(chkKen_Syu_O, CStr(EM_DIV_KENSA.DIV_KENSA_NG)) '�k��
        MyClass.AddCheckBoxKenKind_O(chkKen_Edge_O, CStr(EM_DIV_KENSA.DIV_KENSA_EDGENG)) '���ޕs��
        MyClass.AddCheckBoxKenKind_O(chkKen_NoSave_O, CStr(EM_DIV_KENSA.DIV_KENSA_DBNON)) '���ۑ�
        MyClass.AddCheckBoxKenKind_O(chkKen_NotChk_O, CStr(EM_DIV_KENSA.DIV_KENSA_SKIP)) '������
        MyClass.AddCheckBoxKenKind_O(chkKen_StpSys_O, CStr(EM_DIV_KENSA.DIV_KENSA_STOP)) '������~


        '''' ������ԁi���j
        MyClass.AddCheckBoxKenKind_U(chkKen_Stp_U, CStr(EM_DIV_KENSA.DIV_KENSA_NON)) '��~
        MyClass.AddCheckBoxKenKind_U(chkKen_Ok_U, CStr(EM_DIV_KENSA.DIV_KENSA_OK)) '����
        MyClass.AddCheckBoxKenKind_U(chkKen_Syu_U, CStr(EM_DIV_KENSA.DIV_KENSA_NG)) '�k��
        MyClass.AddCheckBoxKenKind_U(chkKen_Edge_U, CStr(EM_DIV_KENSA.DIV_KENSA_EDGENG)) '���ޕs��
        MyClass.AddCheckBoxKenKind_U(chkKen_NoSave_U, CStr(EM_DIV_KENSA.DIV_KENSA_DBNON)) '���ۑ�
        MyClass.AddCheckBoxKenKind_U(chkKen_NotChk_U, CStr(EM_DIV_KENSA.DIV_KENSA_SKIP)) '������
        MyClass.AddCheckBoxKenKind_U(chkKen_StpSys_U, CStr(EM_DIV_KENSA.DIV_KENSA_STOP)) '������~


        '''' ����������
        Call GetDbInfToSetData()


    End Sub

    ''' <summary>
    ''' �\���F/�ő�\�������擾/�f�[�^�Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetDbInfToSetData()
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String

        Dim rowMaxCnt As Integer = 0        ' �s��
        Dim clrEvt As Integer = 0           ' �C�x���g�F
        Dim clrJyu As Integer = 0           ' �d�̏�F
        Dim clrKei As Integer = 0           ' �y�̏�F


        strSQL = "SELECT LOG�\������, LOG�C�x���g�F, LOG�d�̏�F, LOG�y�̏�F FROM " & DB_COMMON_MASTER
        tcls_DB.Open()

        Dim sqlRead As SqlClient.SqlDataReader = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return
        Try
            sqlRead.Read()
            rowMaxCnt = sqlRead.GetInt32(0)
            clrEvt = sqlRead.GetInt32(1)
            clrJyu = sqlRead.GetInt32(2)
            clrKei = sqlRead.GetInt32(3)

        Catch ex As Exception
            Return
        Finally
            sqlRead.Close()
        End Try

        sqlRead.Close()
        tcls_DB.Close()
        tcls_DB.Dispose()

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �I�[�o�[���C�h
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' Me.Activated �ł͎g�����ɂȂ�Ȃ��ׁALoad�̏I����񂾂��񓯊��Ń��\�b�h���R�[�����Ă�����C�x���g���������B
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' ���X�̃��\�b�h���s
        MyBase.LoadEnding()

        ''''' �g����
        '' ��ʂ��J���I������^�C�~���O�� ��񌟍����s��
        Call ExecSelect_Log()
    End Sub

    ''' <summary>
    ''' ���O�����J�n
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecStart()
        my_btnFunction(0).Enabled = False
        my_btnFunction(8).Enabled = False
        my_btnFunction(9).Enabled = False
        my_btnFunction(10).Enabled = False

        '' �x�[�X���s
        MyBase.ExecStart()
    End Sub

    ''' <summary>
    ''' ���O��������
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overrides Sub ExecEnd()
        my_btnFunction(0).Enabled = True
        my_btnFunction(8).Enabled = True
        my_btnFunction(10).Enabled = True

        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            my_btnFunction(9).Enabled = False
        Else
            my_btnFunction(9).Enabled = True
        End If

        '' �x�[�X���s
        MyBase.ExecEnd()

    End Sub

    ''' <summary>
    ''' �ғ���Ԗ��擾(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKadouStatusName(ByVal nVal As KizuCommon.EM_DIV_KADOU) As String
        Select Case nVal
            Case EM_DIV_KADOU.DIV_KADOU_INIT        '��������
                Return "��������"
            Case EM_DIV_KADOU.DIV_KADOU_STOP        '��~��
                Return "��~��"
            Case EM_DIV_KADOU.DIV_KADOU_START       '�ғ���
                Return "�ғ���"
            Case EM_DIV_KADOU.DIV_KADOU_RETRY       '���g���C��
                Return "���g���C��"
        End Select

        Return ""

    End Function


    ''' <summary>
    ''' ������Ԗ��擾(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKensaStatusName(ByVal nVal As KizuCommon.EM_DIV_KENSA) As String
        Select Case nVal
            Case EM_DIV_KENSA.DIV_KENSA_NON
                Return "��~"
            Case EM_DIV_KENSA.DIV_KENSA_OK
                Return "����"
            Case EM_DIV_KENSA.DIV_KENSA_NG
                Return "�k��"
            Case EM_DIV_KENSA.DIV_KENSA_MENTE
                Return "��Ò�"
            Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return "������~"
            Case EM_DIV_KENSA.DIV_KENSA_EDGENG
                Return "���ޕs��"
            Case EM_DIV_KENSA.DIV_KENSA_DBNON
                Return "���ۑ�"
            Case EM_DIV_KENSA.DIV_KENSA_SKIP
                Return "������"
        End Select

        Return ""

    End Function

    ''' <summary>
    ''' �@���Ԗ��擾(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function GetKiKiStatusName(ByVal nVal As KizuCommon.EM_DIV_KIKI) As String
        Select Case nVal
            Case EM_DIV_KIKI.DIV_KIKI_OK
                Return "����"
            Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return "�y�̏�"
            Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return "�d�̏�"
        End Select

        Return ""
    End Function
    ''' <summary>
    ''' �F�ς�(�ғ����)(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kado(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        '        MyBase.ChangeColor_Kado(e)
        Select Case CStr(e.Value)
            Case "��������"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouInit
            Case "��~��"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouStop
            Case "�ғ���"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouStart
            Case "���g���C��"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKadouRetry
        End Select
    End Sub

    ''' <summary>
    ''' �F�ς�(�@����)(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kiki(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        Select Case CStr(e.Value)
            Case "����"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = Color.White
            Case "�y�̏�"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKikiKei
            Case "�d�̏�"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKikiJyu
        End Select
    End Sub

    ''' <summary>
    ''' �F�ς�(�������)(�e���C���p�F�x�[�X��frmMain_StatusLog)
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Overrides Sub ChangeColor_Kensa(ByVal e As System.Windows.Forms.DataGridViewCellFormattingEventArgs)
        Select Case CStr(e.Value)
            Case "��~"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = g_ColorKensaNon
            Case "����"
                e.CellStyle.ForeColor = Color.Black
                e.CellStyle.BackColor = Color.White
            Case "�k��"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaNg
            Case "���ޕs��"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaEdgeNg
            Case "���ۑ�"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaDbNon
            Case "������"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaSkip
            Case "������~"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorKensaStop
            Case "��Ò�"
                e.CellStyle.ForeColor = Color.Yellow
                e.CellStyle.BackColor = g_ColorMente
        End Select
    End Sub

End Class
