'*********************************************************************************
'�@�J�b�g���уN���X
'	[Ver]
'		Ver.01    2007/09/22  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmCut

    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�r���Ǘ�No

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

    Private Sub frmCut_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.tProp_Moveable = True

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '��ʏ�����
        Call InitDsp()

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
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_CUT
            strSQL &= " WHERE �Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY �o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_CUT
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

        ''����No
        lblMeiNo.Text = ""

        ''���YNO
        lblProductNo.Text = ""

        ''���ы敪
        lblJisDiv.Text = ""

        ''�J�n�ʒu
        lblStartPos.Text = ""

        ''�I���ʒu
        lblEndPos.Text = ""

        '����
        lblKoban.Text = ""

        '�O�����g�t���O
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

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
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">�J�b�g���ѓ`��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_CUT) As Boolean


        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData() As Byte        '���`���o�C�i���f�[�^

        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT ���`��"
        strSQL &= " FROM " & DB_BUF_PRO_R_CUT
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
            bytData = CType(sqlRead.GetValue(0), Byte())
            Dim ii As Integer
            ii = tMod.SizeOf(typZiseki)
            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

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
        Call btnRead_Click(sender, e)
    End Sub
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki As PRO_TYPE_CUT = Nothing  '�r�������уf�[�^

        '�f�[�^�\��
        If GetData(cmbID.Text, typZiseki) Then
            Call SetData(typZiseki)
        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="typZiseki">�J�b�g����</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_CUT)

        ''����No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        ''���YNO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        ''���ы敪
        lblJisDiv.Text = typZiseki.data.nMode

        ''�J�n�ʒu
        lblStartPos.Text = Format(CSng(typZiseki.data.nStart) / 10, "0.0")
        'lblStartPos.Text = typZiseki.data.nStart

        ''�I���ʒu
        lblEndPos.Text = Format(CSng(typZiseki.data.nEnd) / 10, "0.0")
        'lblEndPos.Text = typZiseki.data.nEnd

        '����
        lblKoban.Text = typZiseki.data.nKoban

        '�O�����g�t���O
        If 0 = typZiseki.data.Common.nGrandFlg Then
            lblGrant.Text = "����"
            lblGrant.BackColor = Color.Blue
            lblGrant.ForeColor = Color.Yellow
        Else
            lblGrant.Text = "�ُ�"
            lblGrant.BackColor = Color.Red
            lblGrant.ForeColor = Color.Yellow
        End If

    End Sub



End Class
