Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Data.SqlClient
Imports System.Text.RegularExpressions

Public Class frmPopCoilSearch

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �ϗ�Ή�
    Private Const MC_MAX_COIL_S As Integer = 5              ' �O���ő�ϗ�
    Private Const MC_MAX_COIL_E As Integer = 5              ' �㔼�ő�ϗ�
    Private Const MC_MAX_COIL As Integer = MC_MAX_COIL_S + MC_MAX_COIL_E

    Private Enum EM_COIL_S
        emKey = 0                   ' �r���Ǘ�No
        emName                      ' �R�C��No
        emDay                       ' �o�^���t
        emScode                     ' Scode
        emRpiKey                    ' ���ʂ͊Ǘ�No�B�啪���ȂǁARPI�̊Ǘ�No�Ǝ��ъǗ�No���قȂ�Ƃ��悤

        em0                         ' 0�`4�܂ŉ�
    End Enum
    ' �I����`
    Private Structure TYP_COIL_S
        Dim sName As String         ' �\������
        Dim sSelect As String       ' Select��
        Dim nWhereCnt As Integer    ' Where��̐�
        Dim sWhere As String        ' Where��
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔

    ' ���X�g��
    Private Enum EM_LIST_COL
        emDate = 0
        emCoil
        emKizukenNo
        emScode
        emRpiKey        ' ��\����
        em0             ' �O��5��

        emRpi = em0 + MC_MAX_COIL_S     ' �{�^��
        ' �㔼5��́Am_nColumnCoilE ���g������
    End Enum

    ' RPI�t�@�C���R�t���p�\����
    Private Structure TYP_RPI_INF
        Dim sKizukenNo As String
        Dim sKey As String          ' �r���Ǘ�No
        Dim sName As String         ' �R�C��No
        Dim sDay As String          ' ���t
        Dim sScode() As String        ' SCODE
        Dim sRpiKey As String       ' �Ǘ�No�i�����p�j
        Dim sExt() As String        ' �g����

        ' ����
        Dim bRpiUmu(,) As Integer   ' �������� (0:�܂� 1:�L�� -1:����)
        Dim sFPath(,) As String     ' �t���p�X

        Dim bInputList() As Integer ' �ʒu�A�gCSV���� (0:�܂� 1:�L�� -1:����)

        Public Sub initialize(ByVal nRpiNum As Integer)
            ReDim bRpiUmu(2, nRpiNum - 1)
            ReDim sFPath(2, nRpiNum - 1)
            ReDim bInputList(2)
            ReDim sScode(2)
            ReDim sExt(MC_MAX_COIL - 1)
        End Sub
        Public Function FormatRpiName(ByVal m As Match) As String
            Dim x As String = m.ToString()
            Select Case x
                Case "{S_KEY}"              ' �r���Ǘ�No
                    Return sKey
                Case "{S_NAME}"             ' �R�C��No
                    Return sName
                Case "{S_DAY}"              ' �o�^���t
                    Return sDay
                Case "{S_SCODE_T}"          ' SCODE�i�\�j
                    Return sScode(0)
                Case "{S_SCODE_B}"          ' SCODE�i���j
                    Return sScode(1)
                Case "{S_RPI_KEY}"          ' �����p�Ǘ�No
                    Return sRpiKey
                Case "{S_FOLDER_NAME}"      ' �t�H���_��
                    Return GetFolderName(sKey)
            End Select
            For ii As Integer = 0 To MC_MAX_COIL - 1
                ' {S_X}(X=0,1,2,...)        
                If x = "{S_" & ii.ToString() & "}" Then
                    Return sExt(ii)
                End If
            Next
            Return x
        End Function
    End Structure



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �N���X
    Private mcls_Param As New clsParamManager                       ' �p�����[�^�Ǘ��N���X (�{��)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[
    Private m_sLine As String                                       ' �I�����C��
    Private m_sRpiForder(2 - 1, 10 - 1) As String                   ' RPI�t�H���_(��, �����t�H���_)
    Private m_nRpiNum As Integer                                    ' RPI�t�@�C���̕Жʕӂ�̐�
    Private m_sRpiFormat As String                                  ' RPI�t�@�C������t�H�[�}�b�g (�r���Ǘ�No,��,�p�xor�F)
    Private m_sCsvPath As String                                    ' �ʒu�A�gCSV�̃p�X
    Private m_sRpiOutputPath As String                              ' RPI�ۑ��̃p�X

    ' �ϗ�Ή�
    Private m_lstCoilS(5 + MC_MAX_COIL + -1) As TYP_COIL_S          ' Ini�t�@�C���Ǎ���`
    Private mcnt_txtS(MC_MAX_COIL - 1) As TextBox
    Private mcnt_lblS(MC_MAX_COIL - 1) As Label
    Private m_nColumnCoilE As Integer                               ' �f�[�^�O���b�h�r���[ �㔼�����̗�ԍ�

    ' RPI�t�@�C�������֌W
    Private mcls_WThread As New tClass_WorkThread                   ' ���[�J�[�X���b�h�{��
    Private m_CoilList As New List(Of TYP_RPI_INF)                  ' �������ʎ�n�p
    'Private m_SelectDay(2 - 1) As DateTime							' ���t

    ' �O��I��
    Private m_nLastRow As Integer = -1                              ' �ŏI
    Private m_nLastColumn As Integer = -1                           ' �ŏI

    ' �ʒu�A�g �@�\
    Private m_bInputLine As Boolean                                 ' �ʒu�A�g �@�\�L��
    Private m_nInputLineColumn As Integer                           ' �ʒu�A�g ��No

    Private m_nImageSelectColumn As Integer                         ' �摜�I�� ��No
    Private m_nSaveSelectColumn As Integer                          ' �摜�ۑ��I�� ��No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    'Public Event tEv_RpiSelect(ByVal sFPath As String, ByVal bInit As Boolean)			  ' Rpi�t�@�C���I��
    Public Event tEv_RpiSelect(ByVal sFPath As String, ByVal bInit As Boolean, ByVal cKanriNo As String) ' Rpi�t�@�C���I��


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g

    ''' <summary>
    ''' �O������ΏۃR�C���ύX
    ''' </summary>
    ''' <param name="nMode">-1:�� 1:��</param>
    ''' <remarks></remarks>
    Public Sub SelectChangeCoil(ByVal nMode As Integer)
        If -1 = m_nLastRow Then Return
        If -1 = m_nLastColumn Then Return ' ���I���̏ꍇ ����

        ' �T�[�`
        Dim nRow As Integer = m_nLastRow + nMode
        While True
            If nRow < 0 Then Return
            If nRow > dgvList.RowCount - 1 Then Return

            Dim sFPath As String = GetRpiFileName(m_nLastColumn, nRow)


            '' �I���s�ύX�֌W
            dgvChangeSelectRow(m_nLastRow, nRow)

            ' �C�x���g����
            Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, nRow).Value.ToString
            RaiseEvent tEv_RpiSelect(sFPath, True, sKanriNo)
            Exit While
        End While
    End Sub

    ''' <summary>
    ''' �O������ΏۖʕύX
    ''' </summary>
    Public Sub SelectChangeMen()
        If -1 = m_nLastRow Then Return
        If -1 = m_nLastColumn Then Return ' ���I���̏ꍇ ����

        Dim nMen As Integer = (m_nLastColumn - EM_LIST_COL.emRpi) \ m_nRpiNum   ' �����_
        Dim nColumn As Integer = (m_nLastColumn - EM_LIST_COL.emRpi) Mod m_nRpiNum  ' �p�x�̂ǂ�H

        nMen = CInt(IIf(0 = nMen, 1, 0))
        nColumn = nMen * m_nRpiNum + nColumn + EM_LIST_COL.emRpi    ' ��ʒu�Z�o

        ' �X�V
        Dim sFPath As String = GetRpiFileName(nColumn, m_nLastRow)

        m_nLastColumn = nColumn
        Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, m_nLastRow).Value.ToString
        RaiseEvent tEv_RpiSelect(sFPath, False, sKanriNo)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B

    Public WriteOnly Property tProp_InputLine() As Boolean
        Set(value As Boolean)
            m_bInputLine = value
        End Set
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{����
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPopCoilSearch_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        SetFormLocationAtOnce = True

        '' �R���g���[��������
        mcnt_txtS(0) = txtSelect0
        mcnt_txtS(1) = txtSelect1
        mcnt_txtS(2) = txtSelect2
        mcnt_txtS(3) = txtSelect3
        mcnt_txtS(4) = txtSelect4
        mcnt_txtS(5) = txtSelect5
        mcnt_txtS(6) = txtSelect6
        mcnt_txtS(7) = txtSelect7
        mcnt_txtS(8) = txtSelect8
        mcnt_txtS(9) = txtSelect9

        mcnt_lblS(0) = lblSelect0
        mcnt_lblS(1) = lblSelect1
        mcnt_lblS(2) = lblSelect2
        mcnt_lblS(3) = lblSelect3
        mcnt_lblS(4) = lblSelect4
        mcnt_lblS(5) = lblSelect5
        mcnt_lblS(6) = lblSelect6
        mcnt_lblS(7) = lblSelect7
        mcnt_lblS(8) = lblSelect8
        mcnt_lblS(9) = lblSelect9



        '' �R���g���[��������
        Call DspMsg(1, "")
        tmrRpiDsp.Interval = 300

        ' ����
        dtpDateStart.Value = Now
        dtpTimeStart.Text = Format(Now, "yyyy/MM/dd 00:00:00")
        dtpDateEnd.Value = Now
        dtpTimeEnd.Text = Format(Now, "yyyy/MM/dd 23:59:59")


        'RPI�t�@�C���o�̓t�H���_�擾�i�O��p�X�j
        m_sRpiOutputPath = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "RPI_OUTPUT_PATH", LOCALINI_NAME) ' RPI�ۑ���

        '' ���C���Ǎ���
        Dim sWk As String
        Dim sKey As String
        For ii As Integer = 1 To MC_LINE_CNT
            ' ini�t�@�C���擾
            sKey = String.Format("L_{0}", ii)
            sWk = tMod.tMod_IniFile.ReadIniStr("LINE", sKey, TASKINI_NAME)
            sWk.Trim()
            If 0 >= sWk.Length Then Continue For

            ' �l����
            Dim sAry() As String = sWk.Split(CChar(","))
            If 2 <> sAry.Length Then Continue For
            sAry(0).Trim()
            sAry(1).Trim()
            If 0 >= sAry(0).Length Then Continue For
            If 0 >= sAry(1).Length Then Continue For


            ' ��������DB�L���ȏꍇ�̂�
            If 1 = tMod.tMod_IniFile.ReadIniInt(sAry(1), "ENABLE_DB", TASKINI_NAME) Then
                ' ���X�g�ɍs�ǉ�
                Dim item As New tClass_CmbItem(sAry(1), sAry(0))
                Call cmbLine.Items.Add(item)
            End If

        Next
        If cmbLine.Items.Count <> 0 Then cmbLine.SelectedIndex = 0
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���C���ύX

    ''' <summary>
    ''' �Ώۃ��C���ύX
    ''' </summary>
    Private Sub cmbLine_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbLine.SelectedIndexChanged
        m_sLine = CType(cmbLine.SelectedItem, tClass_CmbItem).g_sKey
        m_nLastRow = -1
        m_nLastColumn = -1

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("Coil�I��[{0}]", m_sLine))
        End If

        ' �K�v�ȃp�����[�^�擾
        mcls_Param.Set_DbSession(m_sLine)
        If Not mcls_Param.ReadParam_Scode() Then
            Call DspMsg(0, "scode �擾�ُ�")
        End If


        ' Ini�t�@�C������RPI�t�H���_����
        For ii As Integer = 0 To 1
            For jj As Integer = 0 To 10 - 1
                m_sRpiForder(ii, jj) = ""
                m_sRpiForder(ii, jj) = tMod.tMod_IniFile.ReadIniStr(m_sLine, String.Format("S_{0}_{1}", ii, jj + 1), TASKINI_NAME)
            Next
        Next

        ' ���ʕ��Ǎ�
        m_nRpiNum = tMod.tMod_IniFile.ReadIniInt(m_sLine, "RPI_NUM", TASKINI_NAME)
        m_sRpiFormat = tMod.tMod_IniFile.ReadIniStr(m_sLine, "RPI_FORMAT", TASKINI_NAME)
        m_sCsvPath = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)   ' �ʒu�A�gCSV



        ' �R���g���[��������
        Call InitControl(m_sLine)           ' ���܂��܊g����
        Call SetListScode()                 ' scode ���X�g
        Call dgvInitialize(dgvList)         ' �ꗗ���X�g

        If 0 = tMod.tMod_IniFile.ReadIniInt(m_sLine, "SEARCH_THREAD_STOP", TASKINI_NAME, 0) Then
            ' SEARCH_THREAD_STOP=0�̂Ƃ��̓`�F�b�N
            chkCheckRpi.Checked = True
        Else
            ' SEARCH_THREAD_STOP=1�̂Ƃ��͔�`�F�b�N
            chkCheckRpi.Checked = False
        End If
    End Sub



    ''' <summary>
    ''' �e���C���p�ɃR���g���[��������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitControl(ByVal sLine As String)

        ' �Œ蕔 Ini�t�@�C���Ǎ�
        m_lstCoilS(EM_COIL_S.emKey) = GetTypCoilS(sLine, "COIL_S_KEY")
        m_lstCoilS(EM_COIL_S.emName) = GetTypCoilS(sLine, "COIL_S_NAME")
        m_lstCoilS(EM_COIL_S.emDay) = GetTypCoilS(sLine, "COIL_S_DAY")
        m_lstCoilS(EM_COIL_S.emScode) = GetTypCoilS(sLine, "COIL_S_SCODE")
        m_lstCoilS(EM_COIL_S.emRpiKey) = GetTypCoilS(sLine, "COIL_S_RPI_KEY")
        If m_lstCoilS(EM_COIL_S.emRpiKey).sName Is Nothing Then
            m_lstCoilS(EM_COIL_S.emRpiKey) = m_lstCoilS(EM_COIL_S.emKey)
            m_lstCoilS(EM_COIL_S.emRpiKey).sSelect += " as wkKey"   ' �Ǘ�No��t�^����ꍇ�A����񖼂�NG�ɂȂ邽�߂̏��u
        End If

        ' ���̔��f
        lblKizukenNo.Text = m_lstCoilS(EM_COIL_S.emKey).sName   ' �r���Ǘ�No
        lblCoilNo.Text = m_lstCoilS(EM_COIL_S.emName).sName     ' �R�C��No

        ' �g����  Ini�t�@�C���Ǎ�
        For ii As Integer = 0 To MC_MAX_COIL - 1
            m_lstCoilS(EM_COIL_S.em0 + ii) = GetTypCoilS(sLine, "COIL_S_" & ii.ToString)

            If m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                mcnt_lblS(ii).Text = ""
                mcnt_txtS(ii).Enabled = False
            Else
                mcnt_lblS(ii).Text = m_lstCoilS(EM_COIL_S.em0 + ii).sName
                mcnt_txtS(ii).Enabled = True
            End If
        Next

        ' �A�N�Z�X��
        Dim sTar As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_TARGET", TASKINI_NAME)
        If 0 = sTar.Length Then
            btnAccessPass.Enabled = False
        Else
            btnAccessPass.Enabled = True
        End If

        chkRpiSaveSelect.Enabled = False
        btnRpiSave.Enabled = False
    End Sub


    ''' <summary>
    ''' ��`�擾
    ''' </summary>
    ''' <param name="sSec">Ini�Z�N�V����</param>
    ''' <param name="sKey">Ini�L�[</param>
    ''' <returns>�擾���</returns>
    Private Function GetTypCoilS(ByVal sSec As String, ByVal sKey As String) As TYP_COIL_S
        Dim typWk As TYP_COIL_S = Nothing
        Dim sWk As String
        sWk = tMod.tMod_IniFile.ReadIniStr(sSec, sKey, TASKINI_NAME)

        Dim sAry() As String = sWk.Split(CChar(";"))
        If 4 <> sAry.Length Then Return typWk

        typWk.sName = sAry(0).Trim()
        typWk.sSelect = sAry(1).Trim()
        typWk.nWhereCnt = CInt(sAry(2).Trim())
        typWk.sWhere = sAry(3).Trim()
        Return typWk
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A�N�Z�X���ݒ�
    Private Sub btnAccessPass_Click(sender As System.Object, e As System.EventArgs) Handles btnAccessPass.Click
        Dim sTar As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_TARGET", TASKINI_NAME)
        Dim sUser As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_USER", TASKINI_NAME)
        Dim sPass As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_PASS", TASKINI_NAME)


        Dim exe As String = System.Environment.GetEnvironmentVariable("ComSpec")    ' �R�}���h�v�����v�g�p�X
        Dim arg As String = String.Format("cmdkey /add:{0} /user:{1} /pass:{2}", sTar, sUser, sPass)    '��������

        ' /C �R�}���h���s��A�I���̃I�v�V�������Ȃ��� �v���Z�X���c����ςȂ��ɂȂ�A�R�}���h���������Ȃ��B hide�����ς��ƌ�������Ȃ��B
        Shell(exe & " /C " & arg, AppWinStyle.Hide)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ scode
    ''' <summary>
    ''' scode���X�g�ꗗ�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetListScode()
        clbScode.Items.Clear()

        For Each wk As clsParamManager.PARAM_SCODE In mcls_Param.GetParamScode()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nID
            clbScode.Items.Add(item, False)
        Next
    End Sub

    ''' <summary>
    ''' �S�I���A�S����
    ''' </summary>
    Private Sub AllSetList(ByVal clb As CheckedListBox, ByVal bMode As Boolean)
        For ii As Integer = 0 To clb.Items.Count - 1
            clb.SetItemChecked(ii, bMode)
        Next
    End Sub
    Private Sub btnTypeAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnScodeAllOk.Click
        AllSetList(clbScode, True)
    End Sub
    Private Sub btnTypeAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnScodeAllNg.Click
        AllSetList(clbScode, False)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' �m��
    ''' </summary>
    Private Sub frmPopCoilSearch_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.F5 Then
            Call CoilSearch(0, "")
        ElseIf e.KeyCode = Keys.F8 Then
            Call CoilBetween()
        ElseIf e.KeyCode = Keys.F12 Then
            Call Me.Close()
        End If
    End Sub
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSearch.Click
        chkRpiSaveSelect.Checked = False
        Call CoilSearch(0, "")
        chkRpiSaveSelect.Enabled = (0 < dgvList.RowCount)
    End Sub
    Private Sub btnBetween_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnBetween.Click
        Call CoilBetween()
    End Sub


    ''' <summary>
    ''' ��ʃK�C�_���X
    ''' </summary>
    ''' <param name="nMode">0:NG  1:����</param>
    ''' <param name="sMsg">���b�Z�[�W</param>
    ''' <remarks></remarks>
    Private Sub DspMsg(ByVal nMode As Integer, ByVal sMsg As String)
        txtMsg.Text = sMsg
        If 0 = nMode Then
            txtMsg.ForeColor = Color.Red
        Else
            txtMsg.ForeColor = Color.Black
        End If

        txtMsg.Refresh()
    End Sub



    ''' <summary>
    ''' �Z���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvList_CellContentClick1(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvList.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub


        ' RPI�{�^���H
        If e.ColumnIndex < EM_LIST_COL.emRpi Then Exit Sub
        If e.ColumnIndex > EM_LIST_COL.emRpi + m_nRpiNum * 2 Then Exit Sub

        ' RPI�p�X���o��
        Dim sFPath As String = GetRpiFileName(e.ColumnIndex, e.RowIndex)

        ' �X�V
        m_nLastRow = e.RowIndex
        m_nLastColumn = e.ColumnIndex


        ' �C�x���g����
        Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, e.RowIndex).Value.ToString
        RaiseEvent tEv_RpiSelect(sFPath, True, sKanriNo)

    End Sub


    ''' <summary>
    ''' �R�C���������X�g�C���f�b�N�X����RPI�t�@�C�����擾
    ''' </summary>
    ''' <param name="nColId">�s</param>
    ''' <param name="nRowId">��</param>
    ''' <returns>�t�@�C����</returns>
    ''' <remarks></remarks>
    Private Function GetRpiFileName(ByVal nColId As Integer, ByVal nRowId As Integer) As String
        ' �S���摜�t�@�C���L�������p�X���b�h���쎞�͒�~������
        RpiThreadStop()
        ' ���Ƀt�@�C�����擾�ς݂̏ꍇ�͎擾�ς݂̃t�@�C������Ԃ�
        If "" <> CStr(dgvList(nColId, nRowId).Tag) Then
            Return CStr(dgvList(nColId, nRowId).Tag)
        End If
        Dim SelectRpiNo As Integer = nColId - EM_LIST_COL.emRpi ' �Ώۂ̑S���摜�̔ԍ�
        Dim nOu As Integer = SelectRpiNo \ m_nRpiNum            ' �\���敪
        Dim nNum As Integer = SelectRpiNo Mod m_nRpiNum         ' �p�x

        ' �S���摜�t�@�C�����擾����
        RpiMainExec(nRowId, nOu, nNum)
        ' �^�C�}�[�X�V
        tmrRpiDsp_Tick(Nothing, Nothing)

        Return CStr(dgvList(nColId, nRowId).Tag)
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    ''' <summary>
    ''' �O���b�h�ݒ�
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            '.ColumnCount = 100
            '.RowCount = 100

            '' ��`
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both


            '' �I���Z���F��`
            .DefaultCellStyle.SelectionForeColor = Color.Black          '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            '.DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
            .DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle.ForeColor = Color.Black

            '' ���`
            '.ColumnCount = EM_SELECT_LIST.emEND					 'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            '.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' ��w�b�_�T�C�Y

            '.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells		' ����T�C�Y��������


            '.DefaultCellStyle().SelectionBackColor = Color.LightPink
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().SelectionForeColor = Color.Blue

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emDay).sName ' ��w�b�_�[����
                clm.Width = 170              ' ��
                clm.Frozen = True       ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using

            '�R�C��No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emName).sName    ' ��w�b�_�[����
                clm.Width = 120
                clm.Frozen = True       ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using
            '�r���Ǘ�No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emKey).sName     ' ��w�b�_�[����
                clm.Width = 150
                clm.Frozen = True       ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using
            '�����\�ʏ��
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emScode).sName      ' ��w�b�_�[����
                clm.Width = 50
                clm.Frozen = False      ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using
            'RPI�Ǘ�No ��\����
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emRpiKey).sName     ' ��w�b�_�[����
                clm.Width = 150
                clm.Frozen = False      ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = False
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using
            ' �g����
            For ii As Integer = 0 To MC_MAX_COIL_S - 1

                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = m_lstCoilS(EM_COIL_S.em0 + ii).sName      ' ��w�b�_�[����
                    clm.Width = 100
                    clm.Frozen = False      ' ���X�N���[�����̃��b�N
                    clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                    clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 

                    If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                        clm.Visible = True
                    Else
                        clm.Visible = False
                        clm.Width = 0
                    End If

                    .Columns.Add(clm)
                End Using
            Next


            '' �{�^�����̎擾
            Dim sWk As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "RPI_IDX_NAME", TASKINI_NAME)
            Dim sAry() As String = sWk.Split(CChar(","))        ' �{�^����

            '�摜�I���
            m_nImageSelectColumn = .ColumnCount
            '' RPI�t�@�C�� ���
            For ii As Integer = 1 To 2
                For jj As Integer = 1 To m_nRpiNum
                    Using clm As New DataGridViewButtonColumnColored
                        clm.HeaderText = CStr(IIf(ii = 1, "�\", "��"))     ' ��w�b�_�[����
                        clm.Width = 30
                        clm.Frozen = False      ' ���X�N���[�����̃��b�N
                        clm.ReadOnly = True     ' �Ǎ���p�ݒ�

                        'clm.FlatStyle = FlatStyle.Standard
                        clm.DefaultCellStyle.Font = New Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                        clm.SortMode = DataGridViewColumnSortMode.NotSortable
                        clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                        clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                        clm.Visible = True

                        clm.UseColumnTextForButtonValue = True
                        If sAry.Length < jj OrElse "" = sAry(jj - 1) Then
                            clm.Text = CStr(jj)
                        Else
                            clm.Text = sAry(jj - 1)
                        End If
                        'clm.DefaultCellStyle().SelectionBackColor = Color.Transparent
                        'clm.DefaultCellStyle().BackColor = Color.Red
                        .Columns.Add(clm)
                    End Using
                Next
            Next
            m_nSaveSelectColumn = .ColumnCount
            For ii As Integer = 1 To 2
                For jj As Integer = 1 To m_nRpiNum
                    Using clm As New DataGridViewCheckBoxColumn
                        clm.HeaderText = CStr(IIf(ii = 1, "�\", "��"))     ' ��w�b�_�[����
                        clm.Width = 30
                        clm.Frozen = False      ' ���X�N���[�����̃��b�N

                        clm.FlatStyle = FlatStyle.Standard
                        clm.DefaultCellStyle.Font = New Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                        clm.SortMode = DataGridViewColumnSortMode.NotSortable
                        clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                        clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                        clm.Visible = False

                        .Columns.Add(clm)
                    End Using
                Next
            Next

            '' �ʒu�A�g
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "�A�g"
                clm.Width = 50
                clm.Frozen = False      ' ���X�N���[�����̃��b�N
                clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = m_bInputLine
                clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 
                .Columns.Add(clm)
            End Using
            m_nInputLineColumn = .ColumnCount - 1


            '' �㔼����
            m_nColumnCoilE = .ColumnCount '- 1	' �㔼�����擪�ʒu���Z�b�g
            For ii As Integer = 0 To MC_MAX_COIL_E - 1

                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sName         ' ��w�b�_�[����
                    clm.Width = 100
                    clm.Frozen = False      ' ���X�N���[�����̃��b�N
                    clm.ReadOnly = True     ' �Ǎ���p�ݒ�
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DefaultCellStyle.NullValue = ""      ' �f�[�^��NULL���̕\�����e 
                    clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL���͎��̃f�[�^���e 

                    If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                        clm.Visible = True
                    Else
                        clm.Visible = False
                        clm.Width = 0
                    End If

                    .Columns.Add(clm)
                End Using
            Next
        End With
    End Sub


    ''' <summary>
    ''' �I���s�ύX
    ''' </summary>
    ''' <param name="nOldRow">���s</param>
    ''' <param name="nNewRow">�V�����s</param>
    ''' <remarks></remarks>
    Private Sub dgvChangeSelectRow(ByVal nOldRow As Integer, ByVal nNewRow As Integer)

        '�J�����g�s�ύX���āA�I���̂݉�������ƁA�v�����Ƃ���̓����ɂȂ�
        Dim a As DataGridViewCell = dgvList(0, nNewRow)
        dgvList.CurrentCell = a
        a.Selected = False

        ' ���ꂾ�ƃ_��
        'dgvList.Rows(nNewRow).Selected = True
        'dgvList.Rows(nNewRow).Selected = False


        ' �w�i�F �F��
        dgvList.Rows(nOldRow).DefaultCellStyle.BackColor = Color.Empty      '�s�̔w�i���f�t�H���g�F�ɖ߂�
        dgvList.Rows(nNewRow).DefaultCellStyle.BackColor = Color.LightPink          '�s�̔w�i��ύX

        ' �O��l�Ƃ��ĕێ�
        m_nLastRow = nNewRow

        ' ������͈͂Ɉړ�
        dgvList.FirstDisplayedScrollingRowIndex = nNewRow
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �R�C������
    ''' </summary>
    ''' <param name="nMode">���[�h (0:�ʏ�  1:�O��10��)</param>
    ''' <param name="sKeyval">�O��10���̊ �r���Ǘ�No</param>
    ''' <remarks></remarks>
    Private Sub CoilSearch(ByVal nMode As Integer, ByVal sKeyval As String)

        '-------------------------------------------------------------------
        '' �O����
        ' DB�ڑ��m�F
        Dim clsDB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sLine)
        If Not clsDB.Open() Then
            If Not gcls_Log Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ��ُ� (CoilSearch)"))
            End If
            Call DspMsg(0, "DB�ڑ����s")
            Exit Sub
        End If

        ' �O��X���b�h�������Ă������~
        RpiThreadStop()
        m_CoilList.Clear()


        ' ���s
        Dim strSQL As String
        If 0 = nMode Then
            strSQL = GetSQLSearchCoil()     ' SQL�擾
        Else
            strSQL = GetSQLBetweenCoil(sKeyval)
        End If

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("�R�C���������s({0})", strSQL))
        End If

        Dim read As SqlDataReader = clsDB.SelectExecute(strSQL)                                 ' �R�C���������s
        If read Is Nothing = True Then
            If Not gcls_Log Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C���������s���s"))
            End If
            Call DspMsg(0, "�R�C���������s" & vbCrLf & strSQL)
            Exit Sub
        End If


        '-------------------------------------------------------------------
        ' �擾
        Dim nRow As Integer = 0
        dgvList.RowCount = 0
        With read
            While .Read() = True                                            ' ���ʂ��L��ԁA�ǂݍ��ݑ�����
                dgvList.RowCount += 1                                       ' �s���ǉ�
                ' ��n���X�g�֓o�^
                Dim typ As New TYP_RPI_INF
                typ.initialize(m_nRpiNum)
                typ.sKizukenNo = .GetSqlValue(4).ToString().Trim()
                ' �����o�[�ϐ��ɓ���
                ' �g�����͂����Ȃ�
                Dim dDateWk As Date = .GetDateTime(0)
                typ.sDay = dDateWk.ToString("yyyyMMdd HHmmss")          ' ���t
                typ.sName = .GetSqlValue(1).ToString()                  ' �R�C��No
                typ.sKey = .GetSqlValue(2).ToString()                   ' �r���Ǘ�No
                Dim sScodeWk As String = .GetSqlValue(3).ToString()
                typ.sScode = sScodeWk.Split(CChar("/"))                 ' SCODE
                typ.sRpiKey = .GetSqlValue(4).ToString()                ' �Ǘ�No
                For ii As Integer = 0 To MC_MAX_COIL - 1
                    typ.sExt(ii) = .GetSqlValue(5 + ii).ToString()      ' �g����
                Next

                ' �f�[�^�O���b�h�ɓ���
                dgvList(EM_LIST_COL.emDate, nRow).Value = dDateWk.ToString("yyyy/MM/dd HH:mm:ss")       ' �o�^���t
                dgvList(EM_LIST_COL.emCoil, nRow).Value = typ.sName.Trim()                              ' �R�C��No
                dgvList(EM_LIST_COL.emKizukenNo, nRow).Value = typ.sKey.Trim()                          ' �r���Ǘ�No
                'dgvList.Rows(nRow).Tag = .GetSqlValue(2).ToString().Trim()
                dgvList(EM_LIST_COL.emScode, nRow).Value = sScodeWk.Trim()                              ' Scode
                dgvList(EM_LIST_COL.emRpiKey, nRow).Value = typ.sRpiKey.Trim()                          ' �����p�Ǘ�No
                dgvList.Rows(nRow).Tag = typ.sKizukenNo

                ' �g����
                For ii As Integer = 0 To MC_MAX_COIL_S - 1
                    dgvList(EM_LIST_COL.em0 + ii, nRow).Value = typ.sExt(ii).Trim()
                Next
                '�ۑ��摜�I���`�F�b�N�̏�����
                For ii As Integer = 0 To (2 * m_nRpiNum) - 1
                    dgvList(m_nSaveSelectColumn + ii, nRow).Value = False
                Next

                For ii As Integer = 0 To MC_MAX_COIL_E - 1
                    dgvList(m_nColumnCoilE + ii, nRow).Value = typ.sExt(MC_MAX_COIL_S + ii).Trim()
                Next

                ' �{�^���w�i�F
                For ii As Integer = EM_LIST_COL.emRpi To EM_LIST_COL.emRpi + m_nRpiNum * 2 - 1
                    dgvList(ii, nRow).Style.BackColor = Color.Gray
                Next

                m_CoilList.Add(typ)


                nRow += 1
            End While
        End With

        Call DspMsg(1, nRow & "�� �R�C����������")


        '-------------------------------------------------------------------
        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvList.DataGridViewSelectionRowColorChange(Color.LightPink)


        '-------------------------------------------------------------------
        ' �O��10���̂݌ŗL����
        If 1 = nMode Then
            nRow = 0
            For ii As Integer = 0 To dgvList.RowCount - 1
                If sKeyval = dgvList.Rows(ii).Tag.ToString Then
                    nRow = ii
                    Exit For
                End If
            Next

            ' �L��
            If 0 <> nRow Then
                '' �w�i�F �F��
                'dgvList(1, nRow).Selected = True
                'Focus(dgvList.Rows(nRow))
                'dgvList.Rows(nRow).Selected = True
                dgvChangeSelectRow(0, nRow)
                'dgvList.FirstDisplayedScrollingRowIndex = nRow
            End If
        End If


        '-------------------------------------------------------------------
        '' �X���b�h�N��
        'm_SelectDay(0) = New DateTime(dtpDateStart.Value.Year, dtpDateStart.Value.Month, dtpDateStart.Value.Day, dtpTimeStart.Value.Hour, dtpTimeStart.Value.Minute, dtpTimeStart.Value.Second)
        'm_SelectDay(1) = New DateTime(dtpDateEnd.Value.Year, dtpDateEnd.Value.Month, dtpDateEnd.Value.Day, dtpTimeEnd.Value.Hour, dtpTimeEnd.Value.Minute, dtpTimeEnd.Value.Second)

        ' �t�@�C�������p�X���b�h�N���Ȃ��̏ꍇ�̓X���b�h�N�����s��Ȃ�

        If True = chkCheckRpi.Checked Then
            RpiThreadStart()
        End If
    End Sub


    ''' <summary>
    ''' �R�C���O��10��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilBetween()
        Dim sKeyVal As String = ""                      ' �I����L�[
        If dgvList.CurrentCell Is Nothing Then Return

        Dim nRow As Integer = dgvList.CurrentCell.RowIndex

        sKeyVal = dgvList(EM_LIST_COL.emKizukenNo, nRow).Value.ToString

        Call CoilSearch(1, sKeyVal)
    End Sub





    ''' <summary>
    ''' �R�C������SQL����
    ''' </summary>
    ''' <returns>�R�C������SQL����</returns>
    ''' <remarks></remarks>
    Private Function GetSQLSearchCoil() As String
        Dim strFormat As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "COIL_SELECT", TASKINI_NAME)                                            ' �R�C������SQL
        Dim sSelect As String = ""
        Dim sWhere As String = ""
        Dim sWk As String



        '-------------------------------------------------------------------
        ' SELECT��
        ' ���Ԋm�蕔
        sSelect &= m_lstCoilS(EM_COIL_S.emDay).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emName).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emKey).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emScode).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emRpiKey).sSelect

        '�g����
        For ii As Integer = 0 To MC_MAX_COIL_S - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + ii).sSelect
            Else
                sSelect &= ", '' as s" & ii & " "   ' ���O�����Ă����鎖�K�{�Bbetween�ňُ�ɂȂ�
            End If
        Next
        For ii As Integer = 0 To MC_MAX_COIL_E - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect
            Else
                sSelect &= ", '' as s" & MC_MAX_COIL_S + ii & " " ' ���O�����Ă����鎖�K�{�Bbetween�ňُ�ɂȂ�
            End If
        Next


        '-------------------------------------------------------------------
        ' WHERE�� �m�蕔
        sWhere &= " 1=1 "
        ' �r���Ǘ�No
        If "" <> txtKizukenNo.Text.Trim Then
            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emKey), txtKizukenNo.Text)
        End If

        ' �R�C��No
        If "" <> txtCoilNo.Text.Trim Then
            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emName), txtCoilNo.Text)
        End If

        ' �J�n��
        If chkDay.Checked Then
            sWhere &= " AND "

            sWhere &= GetSQLWhere_2(m_lstCoilS(EM_COIL_S.emDay), _
                String.Format("{0} {1}", dtpDateStart.Value.ToString("yyyy-MM-dd"), dtpTimeStart.Value.ToString("HH:mm:ss")), _
                String.Format("{0} {1}", dtpDateEnd.Value.ToString("yyyy-MM-dd"), dtpTimeEnd.Value.ToString("HH:mm:ss")))
        End If


        ' scode
        Dim nCheckCnt As Integer = clbScode.CheckedIndices.Count()
        If 0 <> nCheckCnt And clbScode.Items.Count <> nCheckCnt Then
            sWk = ""
            For Each item As TYP_ITEM In clbScode.CheckedItems
                If 0 <> sWk.Length Then sWk &= ", "
                sWk &= item.nKey.ToString
            Next

            sWhere &= " AND "
            If 1 = m_lstCoilS(EM_COIL_S.emScode).nWhereCnt Then
                sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emScode), sWk)
            ElseIf 2 = m_lstCoilS(EM_COIL_S.emScode).nWhereCnt Then
                sWhere &= GetSQLWhere_2(m_lstCoilS(EM_COIL_S.emScode), sWk, sWk)
            End If
        End If

        '-------------------------------------------------------------------
        ' WHERE�� �g����

        For ii As Integer = 0 To MC_MAX_COIL - 1
            If m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then Continue For
            If "" = mcnt_txtS(ii).Text.Trim Then Continue For

            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.em0 + ii), mcnt_txtS(ii).Text)
        Next


        '-------------------------------------------------------------------
        ' ����
        Return String.Format(strFormat, sSelect, sWhere)
    End Function


    ''' <summary>
    ''' WHERE���`(����)
    ''' </summary>
    ''' <param name="item"></param>
    ''' <param name="str"></param>
    ''' <returns></returns>
    Private Function GetSQLWhere_1(ByVal item As TYP_COIL_S, ByVal str As String) As String
        Return String.Format(item.sWhere, str.Trim)
    End Function

    Private Function GetSQLWhere_2(ByVal item As TYP_COIL_S, ByVal str1 As String, ByVal str2 As String) As String
        Return String.Format(item.sWhere, str1.Trim, str2.Trim)
    End Function



    ''' <summary>
    ''' �O��10�R�C������SQL����
    ''' </summary>
    ''' <returns>�R�C������SQL����</returns>
    ''' <remarks></remarks>
    Private Function GetSQLBetweenCoil(ByVal sKeyVal As String) As String
        Dim sSelect As String = ""
        Dim sWhere(1) As String
        Dim sSqlBetween(1) As String

        Dim sKey As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "KEY_NO", TASKINI_NAME)  ' ��L�[

        '''' ���ʂŎg���Ă��錟��SQL��"ORDER BY"��ȍ~���폜
        Dim sFormat As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "COIL_SELECT", TASKINI_NAME)                                          ' �R�C������SQL
        Dim sSqlBase As String = sFormat.Substring(0, sFormat.LastIndexOf("ORDER BY", StringComparison.OrdinalIgnoreCase))



        '-------------------------------------------------------------------
        ' SELECT��
        ' 
        ' ���Ԋm�蕔
        sSelect &= m_lstCoilS(EM_COIL_S.emDay).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emName).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emKey).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emScode).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emRpiKey).sSelect

        '�g����
        For ii As Integer = 0 To MC_MAX_COIL_S - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + ii).sSelect
            Else
                sSelect &= ", '' as s" & ii & " "   ' ���O�����Ă����鎖�K�{�Bbetween�ňُ�ɂȂ�
            End If
        Next
        For ii As Integer = 0 To MC_MAX_COIL_E - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect
            Else
                sSelect &= ", '' as s" & MC_MAX_COIL_S + ii & " " ' ���O�����Ă����鎖�K�{�Bbetween�ňُ�ɂȂ�
            End If
        Next

        '-------------------------------------------------------------------
        ' WHERE�� �ȍ~
        sWhere(0) = String.Format("r.{0}>='{1}' ORDER BY r.{0} ASC", sKey, sKeyVal)
        sWhere(1) = String.Format("r.{0}< '{1}' ORDER BY r.{0} DESC", sKey, sKeyVal)


        '-------------------------------------------------------------------
        ' ����
        ' �O�����ƌ�����
        sSqlBetween(0) = String.Format(sSqlBase, " TOP 11 " & sSelect, sWhere(0))   ' �������܂߂�
        sSqlBetween(1) = String.Format(sSqlBase, " TOP 10 " & sSelect, sWhere(1))   ' �����͊܂߂Ȃ�

        ' �S��������
        sSelect = String.Format("SELECT * FROM ({0} UNION ALL {1} ) AS uni ORDER BY uni.{2} DESC", sSqlBetween(0), sSqlBetween(1), sKey)
        Return sSelect
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ RPI�t�@�C������ �ʃX���b�h
    Dim m_ThStop As Boolean = False

    ''' <summary>
    ''' �X���b�h�X�^�[�g
    ''' </summary>
    Public Function RpiThreadStart(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
        m_ThStop = False
        tmrRpiDsp.Start()
        '' ���C���X���b�h���s
        mcls_WThread.ThreadStart(AddressOf RpiMainThread, priority, "", True)
        Return True
    End Function

    ''' <summary>
    ''' �X���b�h�X�g�b�v
    ''' </summary>
    Public Sub RpiThreadStop()
        '' �C���X�^���X�m�F
        If Not mcls_WThread Is Nothing Then
            m_ThStop = True
            tmrRpiDsp.Stop()
            '' ���C���X���b�h�I��
            mcls_WThread.ThreadStop(2000)
            ' �X���b�h����~���̂Ƃ��́uRPI�t�@�C���L���`�F�b�N�v�{�^�����R���g���[���F�ɂ���
            chkCheckRpi.BackColor = System.Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' RPI�t�@�C����������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RpiMainThread()
        Try
            For ii As Integer = 0 To m_CoilList.Count - 1
                ' ����̌����Ώ�
                Dim typ As TYP_RPI_INF = m_CoilList(ii)

                ' =====================================
                ' RPI ����
                For ou As Integer = 0 To 1
                    For num As Integer = 0 To m_nRpiNum - 1
                        If m_ThStop Then Return ' ���f�v����t

                        ' �O�񒆒f�����Ƃ���܂Ń��[�v
                        If typ.bRpiUmu(ou, num) <> 0 Then
                            Continue For
                        End If

                        GetRpiFile(typ, ou, num)
                    Next
                Next

                ' ���ʃZ�b�g
                m_CoilList(ii) = typ
            Next
        Catch ex As Threading.ThreadAbortException
            '' ===>> �⊪ �n�C�h���Ή� 20161219
            ' ��Windows���i���Ȃ��ɂ���ăt�@�C���A�N�Z�X���s�̎��ɃR�����g���o�͂���@�\
            ' ������������ɋ@�\�ǉ��i���͈̔͂̃R�����g������������΋@�\�͒ǉ������j
            'Catch ex As System.IO.IOException
            '    If ex.Message.StartsWith("���O�I�����s") Then
            '        MsgBox("�S���摜�t�@�C���Q�Ǝ��s�B" + vbCrLf + "RPI�Q�ƃt�H���_�A�N�Z�X�����ݒ�{�^�������čČ������Ă��������B")
            '    End If
            '' <<===
        Catch ex As Exception
        Finally
            m_ThStop = True
        End Try
    End Sub

    ''' <summary>
    ''' RPI�t�@�C�����������i�P�t�@�C���j
    ''' </summary>
    ''' <param name="ii">�������X�g�s�C���f�b�N�X(0�I���W��)</param>
    ''' <param name="ou">�\���敪(�\=0,��=1)</param>
    ''' <param name="num">�p�x(0�I���W��)</param>
    ''' <remarks></remarks>
    Private Sub RpiMainExec(ByVal ii As Integer, ByVal ou As Integer, ByVal num As Integer)
        Try
            ' ����̌����Ώ�
            ' �S�t�H���_�[�T�[�`
            GetRpiFile(m_CoilList(ii), ou, num)

        Catch ex As Threading.ThreadAbortException
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' RPI�t�@�C�����������i�P�t�@�C��
    ''' </summary>
    ''' <param name="typ">RPI�t�@�C�����</param>
    ''' <param name="ou">�\���敪</param>
    ''' <param name="num">�R�C��No</param>
    ''' <remarks></remarks>
    Private Sub GetRpiFile(ByRef typ As TYP_RPI_INF, ByVal ou As Integer, ByVal num As Integer)
        For jj As Integer = 0 To 10 - 1
            If "" = m_sRpiForder(ou, jj) Then Continue For

            ' �t�H�[�}�b�g����
            Dim sSearchName As String = m_reParen.Replace(m_sRpiFormat, AddressOf typ.FormatRpiName)
            ' �t�@�C������������
            sSearchName = String.Format(sSearchName, typ.sKizukenNo, ou, num + 1, typ.sName)
            ' ������.rpi�ŏI����Ă��Ȃ����"*.rpi"�𖖔��ɂ���
            If sSearchName.EndsWith(".rpi", StringComparison.CurrentCultureIgnoreCase) Then
                Dim FullPath As String = System.IO.Path.Combine(m_sRpiForder(ou, jj), sSearchName)
                If System.IO.File.Exists(FullPath) Then
                    typ.sFPath(ou, num) = FullPath
                    typ.bRpiUmu(ou, num) = 1
                End If
            Else
                sSearchName &= "*.rpi"
                Try
                    Dim sFpath() As String = IO.Directory.GetFiles(m_sRpiForder(ou, jj), sSearchName, IO.SearchOption.TopDirectoryOnly)
                    If 1 = sFpath.Length Then      ' �� ��������ꍇ�ǂ����悤�B���肦�Ȃ��͂�������
                        typ.sFPath(ou, num) = sFpath(0)
                        typ.bRpiUmu(ou, num) = 1
                        Exit For
                    End If
                Catch de As System.IO.DirectoryNotFoundException
                End Try
            End If
        Next
        ' �������ʍX�V�iRPI�t�@�C�������j
        If 0 = typ.bRpiUmu(ou, num) Then typ.bRpiUmu(ou, num) = -1

        ' =====================================
        ' �ʒu�A�gCSV ����
        If m_bInputLine Then
            If typ.bInputList(ou) <> 0 Then Return
            typ.bInputList(ou) = -1
            If Not String.IsNullOrEmpty(typ.sFPath(ou, num)) Then
                '�ʒu�A�g�t�@�C������
                Dim sWk As String = String.Format("{0}@{1}*.csv", typ.sKizukenNo, ou)
                Dim sOldFpath() As String = IO.Directory.GetFiles(m_sCsvPath, sWk, IO.SearchOption.TopDirectoryOnly)
                If 0 < sOldFpath.Length Then
                    typ.bInputList(ou) = 1
                End If
            End If
        End If
    End Sub

    ''' <summary>
    ''' �X�V�^�C�}�[
    ''' </summary>
    Private Sub tmrRpiDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tmrRpiDsp.Tick
        Dim bStop As Boolean = m_ThStop ' �ŏ��ɍ��̏�Ԃ��擾


        ' ���ʂ����X�g�ɔ��f
        For ii As Integer = 0 To dgvList.RowCount - 1
            Dim typ As TYP_RPI_INF = m_CoilList(ii)

            '''' RPI
            For ou As Integer = 0 To 1
                For num As Integer = 0 To m_nRpiNum - 1
                    Dim nCol As Integer = EM_LIST_COL.emRpi + (m_nRpiNum * ou) + num

                    If 1 = typ.bRpiUmu(ou, num) Then
                        dgvList(nCol, ii).Style.BackColor = Color.LightGreen
                        dgvList(nCol, ii).Tag = CStr(typ.sFPath(ou, num))
                    ElseIf -1 = typ.bRpiUmu(ou, num) Then
                        dgvList(nCol, ii).Style.BackColor = Color.Red
                    End If
                Next
            Next

            '''' �ʒu�A�gCSV
            Dim sWk As String = ""
            For ou As Integer = 0 To 1
                If 0 = typ.bInputList(ou) Then sWk &= "�@"
                If 1 = typ.bInputList(ou) Then sWk &= "��"
                If -1 = typ.bInputList(ou) Then sWk &= "�|"
            Next
            dgvList(m_nInputLineColumn, ii).Value = sWk

        Next


        If bStop Then
            ' �X���b�h����~���̂Ƃ��́uRPI�t�@�C���L���`�F�b�N�v�{�^�����R���g���[���F�ɂ���
            chkCheckRpi.BackColor = System.Drawing.SystemColors.Control
        Else
            ' �X���b�h�����쒆�̂Ƃ��́uRPI�t�@�C���L���`�F�b�N�v�{�^����ԐF�ɂ���
            chkCheckRpi.BackColor = Color.Red
        End If
        ' ���for�������������������
        If bStop Then
            tmrRpiDsp.Stop()
        End If
    End Sub

    ''' <summary>
    ''' �S���摜�t�@�C���L���`�F�b�N�{�^���ύX
    ''' </summary>
    Private Sub chkCheckRpi_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkCheckRpi.CheckedChanged
        If chkCheckRpi.Checked Then
            ' �S���摜�t�@�C���L���`�F�b�N�J�n
            RpiThreadStart()
        Else
            ' �S���摜�t�@�C���L���`�F�b�N��~
            RpiThreadStop()
        End If
    End Sub


    ''' <summary>
    ''' RPI�ۑ��I���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkRpiSaveSelect_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkRpiSaveSelect.CheckedChanged

        btnRpiSave.Enabled = chkRpiSaveSelect.Checked

        '�摜�I���^�ۑ��I���̕\���ؑ�
        For ii As Integer = 0 To (2 * m_nRpiNum) - 1
            dgvList.Columns.Item(m_nImageSelectColumn + ii).Visible = Not chkRpiSaveSelect.Checked
            dgvList.Columns.Item(m_nSaveSelectColumn + ii).Visible = chkRpiSaveSelect.Checked
        Next

    End Sub

    ''' <summary>
    ''' RPI�ۑ����s�{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpiSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpiSave.Click

        '�ۑ��Ώۃt�@�C�����쐬
        Dim lstRpiFiles As New List(Of String)
        Dim lstErrorRpiFile As New List(Of String)
        For ii As Integer = 0 To dgvList.RowCount - 1
            For jj As Integer = 0 To (2 * m_nRpiNum) - 1
                '�`�F�b�N�̕t���Ă鍀�ڂ̂ݑΏ�
                If dgvList(m_nSaveSelectColumn + jj, ii).Value.Equals(True) Then
                    ' RPI�p�X���o��
                    Dim sFPath As String = GetRpiFileName(m_nImageSelectColumn + jj, ii)
                    ' �摜���݃`�F�b�N
                    If Not String.IsNullOrEmpty(sFPath) Then
                        lstRpiFiles.Add(sFPath)
                    Else
                        '�t�@�C���p�X�͂Ȃ��̂ŁA�Ǘ�NO��\������
                        lstErrorRpiFile.Add(dgvList(EM_COIL_S.emRpiKey, ii).Value.ToString)
                    End If
                End If
            Next
        Next

        '�ۑ��Ώۃ`�F�b�N
        If 0 = lstRpiFiles.Count Then
            If lstErrorRpiFile.Count = 0 Then
                MsgBox("�ۑ�����RPI�t�@�C����I�����Ă��������B")
            Else
                MsgBox("�ۑ�����RPI�t�@�C���������瑶�݂��Ȃ��͗l�ł��B" & String.Join(vbCrLf, lstErrorRpiFile.ToArray()))
            End If
            Return
        End If

        '�ۑ���I��
        Dim dlgFolderSelect As New SaveFileDialog
        dlgFolderSelect.Title = "RPI�t�@�C����ۑ�����t�H���_��I�����Ă��������B"
        dlgFolderSelect.FileName = System.IO.Path.GetFileName(lstRpiFiles(0).ToString())
        dlgFolderSelect.AutoUpgradeEnabled = True
        dlgFolderSelect.Filter = "*.rpi|*.rpi"
        dlgFolderSelect.RestoreDirectory = True
        If Not String.IsNullOrEmpty(m_sRpiOutputPath) Then
            dlgFolderSelect.InitialDirectory = m_sRpiOutputPath
        End If
        If DialogResult.OK <> dlgFolderSelect.ShowDialog(Me) Then
            Return
        End If
        m_sRpiOutputPath = System.IO.Path.GetDirectoryName(dlgFolderSelect.FileName)

        'INI�t�@�C���X�V
        '�C���X�^���X����INI�t�@�C���l�̐����������킹�邽�߁A�����X�V
        tMod.tMod_IniFile.WriteIni("RPI_VIEWER", "RPI_OUTPUT_PATH", LOCALINI_NAME, m_sRpiOutputPath)

        '�ۑ����s�i���s���͑���s�j
        Me.Enabled = False
        SHAPI.CopyFilesToFolder(Me.Handle, m_sRpiOutputPath, lstRpiFiles.ToArray())
        Me.Enabled = True

        '���ʕ\��
        If lstErrorRpiFile.Count > 0 Then
            MsgBox("RPI�t�@�C���ۑ��������܂����B" & vbCrLf & "�������A�ȉ���RPI�t�@�C���͌�����Ȃ��͗l�ł��B" & vbCrLf & String.Join(vbCrLf, lstErrorRpiFile.ToArray()))
        Else
            '�S�Ẵ`�F�b�N���N���A
            For ii As Integer = 0 To dgvList.RowCount - 1
                For jj As Integer = 0 To (2 * m_nRpiNum) - 1
                    dgvList(m_nSaveSelectColumn + jj, ii).Value = False
                Next
            Next

            'RPI�ۑ��I�����[�h����
            chkRpiSaveSelect.Checked = False

            MsgBox("RPI�t�@�C���ۑ��������܂���")
        End If

    End Sub
 
End Class
