'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2012/10/02  �V�K�쐬
'
'	[����]
'		�EVB���` ���̂܂܃R�s�y�łn�j�B�������́A�������ɒu������
'*********************************************************************************
Option Strict On                    ' �S�r���A�S�v���O�����K�{

Imports tClassLibrary               ' �r��VB���C�u�����B
Imports KizuLibCLI                  ' �r��CLI���C�u�����B���[���X���b�g���g�p���K�{
Imports System.Reflection



Public Module modMain

	Public Const XX_VBSMP As String = "XX_VBSMP"	' �� ����́A�e�X�g�f�[�^�B�z���g�͒萔���g�p
	Public Const SYSTEM_TITLE As String = "test"	' �� ����́A�e�X�g�f�[�^�B�z���g�͒萔���g�p


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���[�o���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing				' ���O�Ǘ��N���X (�{��)
    Public g_NOA As Integer                                             ' NOA�ڑ��t���O (0:�I�����C��  1:���u)
    Public g_LineName As String                                         ' �P�x�␳�l�̕ۊǐ�Ɏg�����C����
    Public g_LineCollor As Dictionary(Of String, String)                ' �`�悷����̐F�̐ݒ�
    Public g_MoveAverage As String                                      ' �O���ʏI�����ɕۑ����ꂽ�ړ����σT�C�Y
    Public g_LineProNama As String                                      ' �`�悷����̐F(�c�v���W�F�N�V������)
    Public g_LineProAverage As String                                   ' �`�悷����̐F(�c�v���W�F�N�V�����ړ�����)
    Public g_LineCorrected As String                                    ' �`�悷����̐F(�␳�P�x��)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ���C���X���b�h�Ǘ��p (�قڕK�{�̊�{�̌`)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' �����N���X �i�ʃX���b�h�Ȃ̂Œ��Ӂj
    Private m_ap As ApplicationContext                                  ' �R���e�L�X�g

    Private mfrm_KidoHosei As frmKidoHosei


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Task_First()

        '//======================================================
        '  ��ʎ��s

        '' ��ʕ\��
        Dim showMontor As Integer = 1       ' ���j���[�\���Ώۂ̃��j�^�[No (1�I���W��)

        '' �� �����Ŋe��ʐ؂�ւ�(���j���[��ʂ̕\��)
        mfrm_KidoHosei = New frmKidoHosei(gcls_Log)
        mfrm_KidoHosei.Show()


        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N
        m_ap.Dispose()

        Task_Last()
    End Sub

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()

        mcls_Mod = New KizuMod
        'mcls_Mod = New KizuLibCLI.KizuMod

        '' ���O�N���X����
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager("KIDO_HOSEI", Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�I�y���[�^��ʋN��")


    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()
        '//======================================================
        '  �I������
        Try
            '''' �����܂ŗ����� �I����
            ' �t�H�[���J��
            'If Not mfrm_KidoHosei Is Nothing Then
            '    mfrm_KidoHosei.Close()
            '    mfrm_KidoHosei.Dispose()
            '    mfrm_KidoHosei = Nothing
            'End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0} [{1}]", "���C�������I���ُ�", ex.Message))
        End Try

        '' ���̑��N���X���
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�I�y���[�^��ʏI��")
        gcls_Log.Dispose()
        gcls_Log = Nothing

        '' �^�X�N�I��
        mcls_Mod.Task_Exit()

    End Sub

	''' <summary>
	''' �^�X�N�I������
	''' </summary>
	''' <remarks></remarks>
	Public Sub TaskEnd()
		m_ap.ExitThread()										' ���C���X���b�h�I��
	End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[���^�C�g���ݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

		frmData.Text = SYSTEM_TITLE & Space(2) & frmData.Text

    End Sub

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
    Public Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                             ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                             ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                             Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' ��w�b�_�[����
            clm.HeaderText = sHeader
            ' ��
            clm.Width = nWidth
            ' ���X�N���[�����̃��b�N
            'clm.Frozen = bFrozen
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
            clm.FillWeight = 1
            ' ��쐬
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�`�F�b�N�{�b�N�X��ݒ�
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
    Public Sub dgvColSetCheck(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer, _
                              ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode, _
                              ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment, _
                              Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewCheckBoxColumn
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

    ''' <summary>
    ''' DB�ۑ����̃n�[�h�R�s�[���c���Ă���
    ''' </summary>
    ''' <param name="myForm"></param>
    ''' <remarks></remarks>
    Public Function HardCopy(ByVal myForm As Form) As String
        Dim strFPath As String
        Dim strFolder As String
        Dim strName As String

        strFolder = IMG_PATH
        strName = myForm.Text & "_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg"

        '''' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '''' �t�H���_�̗L���`�F�b�N(�����������낤)
        tMod.CheckFolder(strFolder)

        '''' �n�[�h�R�s�[
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Call cls_Hcopy.HCopy(myForm, strFPath, Drawing.Imaging.ImageFormat.Jpeg)

        Return strFPath
    End Function

    ''' <summary>
    ''' KizuLibCLI.DLL�̓Ǎ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit�؂�ւ��Ή�</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function

End Module
