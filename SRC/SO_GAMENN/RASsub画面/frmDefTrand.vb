
'*********************************************************************************
' �r�������g�����h���
'	[Ver]
'		Ver.01    2012/10/03  �r�������g�����h��ʒǉ��Ή�(�V�K�쐬)
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports SO_GAMENN.frmDefIncidenceGraph

Public Class frmDefTrand

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const INI_TYPE_KEY As String = "KizuCountTrandType"      '�r��t�B���^�[�`�F�b�N�ۑ��p KizuLocal.ini�L�[
    Private Const INI_GRADE_KEY As String = "KizuCountTrandGrade"    '�O���[�h�t�B���^�[�`�F�b�N�ۑ��p KizuLocal.ini�L�[

    Private Const CSV_DEFCOUNT_FILENAME As String = "�r�������g�����h���.csv" 'CSV�ۑ��t�@�C����

    '�r��E�O���[�h�t�B���^�[��
    Public Enum EM_FILTER_COL                       ' �r��
        emCheck = 0                                 ' �`�F�b�N�{�b�N�X
        emName                                      ' ����
        emEdasysID                                  ' EdasysID (��L�[)
        emMapName                                   ' ���́i�r��̂݁j
    End Enum

    '�������� �w�b�_��
    Private Enum EM_RESULT_HEAD_COL
        nTsuNit = 0         '�ʔ̓���
        nCoilNo             '�R�C��No
        nLen                '����������
        nKizuKanNo          '�Ǘ�No(��\������)
        nOrderNo            '����No(��\������)
        nHinCode            '�i��(��\������)
        nSyuZai             '��ގ�(��\������)
        nNextCode           '�v�掟�H��(��\������)
        nKenKijun           '�����(�\)(��\������)
        nCoilAtu            '�ޗ���(��\������)
        nCoilWidth          '�ޗ���(��\������)
        nCoilAtuSei         '������(��\������)
        nCoilWidSei         '������(��\������)
        nTotalO				'�S�̌�(�\)
		nTotalU				'�S�̌�(��)

		nDetalStart			'�e�r���J�n
	End Enum

    ''' <summary>
    ''' �r���g�����h������ʂ̖߂�l
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum KIZU_TRAND_RETURN
        nShowKizuTrand = 0          '�r���g�����h��ʕ\��
        nCancel                     '�L�����Z��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    '/ /////////////////////////////////////////////////////////////////////////////////

    '�������� �w�b�_���f�[�^�\����
    Private Structure TINIY_DEFCOUNT_INF
		Dim strTourokuNit As String			'�o�^����
		Dim strKanriNO As String			'�Ǘ�No
		Dim strCoilNO As String				'�R�C��No
		Dim dDefOCount As Double			'�\�r��
		Dim dDefUCount As Double			'���r��
		Dim dDefACount As Double			'���v�r��

		'�C�j�V�����C�Y
		Public Sub initialize()
			strKanriNO = ""
			strCoilNO = ""
			dDefOCount = 0					'�\�r��
			dDefUCount = 0					'���r��
			dDefACount = 0					'���v�r��
		End Sub

	End Structure

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strIniName As String = LOCALINI_NAME	'�`�F�b�N����ۑ�����INI�t�@�C����
	Private m_proID As System.Diagnostics.Process = Nothing	'EXCEL�p�v���Z�XID
	Private m_dtSearchDateStr As Date	   '�J�n��
	Private m_dtSearchDateEnd As Date	   '�I����
	Private m_dtSearchTimeStr As Date	   '�J�n����
	Private m_dtSearchTimeEnd As Date      '�I������

    Private m_bSortFlg As Boolean          '�\���� True:���� False:�~��

    Private m_typKizuTrandInf As TYP_KIZU_TRAND_INF '�r���g�����h�O���t�\���p�f�[�^
    Private m_nRetCode As KIZU_TRAND_RETURN '
    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
#Region "�v���p�e�B"
    ''' <summary>
    ''' �r���g�����h�O���t�\���p�f�[�^
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizuTrandInf() As TYP_KIZU_TRAND_INF
        Get
            Return m_typKizuTrandInf
        End Get
    End Property


    ''' <summary>
    ''' ���^�[���R�[�h
    ''' </summary>
    ''' <returns>���쌋��</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ReturnCode() As KIZU_TRAND_RETURN
        Get
            Return m_nRetCode
        End Get
    End Property
#End Region


#Region "�C�x���g"
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        '���u��
        If NOA_ONLINE <> g_NOA Then
            Me.tProp_Moveable = True                                                ' ��ʈړ�����
        End If

        '�r��E�O���[�h�t�B���^�[������
        Call dgvInitialize_FilterGrid(dgvDispType)
        Call dgvInitColumn_FilterGrid(dgvDispType, "�r�햼��")
        Call dgvFilter_AddHandler(dgvDispType)

        Call dgvInitialize_FilterGrid(dgvDispGrade)
        Call dgvInitColumn_FilterGrid(dgvDispGrade, "�O���[�h����")
        Call dgvFilter_AddHandler(dgvDispGrade)

        '�r��t�B���^�[�\��
        SetFilterData(dgvDispType, DB_TYPE_NAME, SO_GAMENN, INI_TYPE_KEY)

        '�O���[�h�t�B���^�[�\��
        SetFilterData(dgvDispGrade, DB_GRADE_NAME, SO_GAMENN, INI_GRADE_KEY)

        '�������ʏ�����
        Call dgvInitialize_ResultGrid(dgvResult)
        Call dgvInitColumn_ResultGrid(dgvResult)

        '�r��S�I���A�S�����ɊY���O���b�h�����Z�b�g
        btnTypeAllSelect.Tag = dgvDispType
        btnTypeAllCancel.Tag = dgvDispType

        '�O���[�h�S�I���A�S�����ɊY���O���b�h�����Z�b�g
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(btnSearch, 5 - 1)
        AddFunctionButton(btnCsv, 6 - 1)
        AddFunctionButton(btnGraph, 7 - 1)
        AddFunctionButton(btnEnd, 12 - 1)

    End Sub

    ''' <summary>
    ''' �t�H�[���N���[�Y�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefTrand_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed

        '�t�B���^�[�O���b�h�̃`�F�b�N��ini�t�@�C���ɕۑ�����
        Call SaveCheck()
    End Sub

    ''' <summary>
    ''' ����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        m_nRetCode = KIZU_TRAND_RETURN.nCancel
        Me.Hide()
    End Sub

    ''' <summary>
    ''' �����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '�������̓`�F�b�N
        If Not CheckSearch() Then
            '�������͂�����Ă��炸�A���t�̃`�F�b�N�������Ă��Ȃ��Ƃ����b�Z�[�W
            lblGuide.Text = "�����L�[����͂��Ă�������"
            Exit Sub
        End If

        '�J�[�\�����������ɕύX
        Me.Cursor = Cursors.WaitCursor

        '�R�C������
        Call SearchCoil()

        '�O���b�h��\���؂�ւ�
        Call ChangeDspCol()

        '�J�n���E�I�����擾(EXCEL �͈͗p)
        If chkDateSet.Checked = True Then
            m_dtSearchDateStr = dtpDateStart.Value
            m_dtSearchDateEnd = dtpDateEnd.Value
        Else
            m_dtSearchDateStr = Nothing
            m_dtSearchDateEnd = Nothing
        End If

        '�J�n�����E�I�������擾(EXCEL �͈͗p)
        If chkTimeSet.Checked = True Then
            m_dtSearchTimeStr = dtpTimeStart.Value
            m_dtSearchTimeEnd = dtpTimeEnd.Value
        Else
            m_dtSearchTimeStr = Nothing
            m_dtSearchTimeEnd = Nothing
        End If

        '�\����
        If rdoSortAsc.Checked = True Then
            m_bSortFlg = True
        Else
            m_bSortFlg = False
        End If

        '�J�[�\����ʏ�ɖ߂�
        Me.Cursor = Cursors.Arrow
    End Sub

    ''' <summary>
    ''' �O���t�o�̓{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnGraph_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim bRet As Boolean

        '�r���g�����h�O���t��ʗp�f�[�^�擾
        bRet = SetTrandGraphData()
        If bRet Then
            m_nRetCode = KIZU_TRAND_RETURN.nShowKizuTrand
            Me.Hide()
        End If

    End Sub

    ''' <summary>
    ''' CSV�o�̓{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        'CSV�o��
        Call CsvOut(CSV_PATH, CSV_DEFCOUNT_FILENAME)
    End Sub

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
    ''' ���t�t�B���^�[ �L���^�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkDateSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkDateSet.CheckedChanged

        ' �R���g���[���̗L������
        If chkDateSet.Checked = True Then
            dtpDateStart.Enabled = True
            dtpDateEnd.Enabled = True
        Else
            dtpDateStart.Enabled = False
            dtpDateEnd.Enabled = False
        End If

        '���t�t�B���^�[�������Ȃ�A�����t�B���^�[�������ɂ���
        If Not chkDateSet.Checked Then If chkTimeSet.Checked = True Then chkTimeSet.Checked = False

    End Sub
    ''' <summary>
    ''' �����t�B���^�[ �L���^�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkTimeSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkTimeSet.CheckedChanged

        ' �R���g���[���̗L������
        If chkTimeSet.Checked = True Then
            dtpTimeStart.Enabled = True
            dtpTimeEnd.Enabled = True
        Else
            dtpTimeStart.Enabled = False
            dtpTimeEnd.Enabled = False
        End If

        '���t�t�B���^�[���L���Ȃ�A���t�t�B���^�[���L���ɂ���
        If chkTimeSet.Checked Then If chkDateSet.Checked = False Then chkDateSet.Checked = True

    End Sub

    ''' <summary>
    ''' �S�I���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

        '�`�F�b�N�{�b�N�X��S��ON
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub

    ''' <summary>
    ''' �S�����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        '�`�F�b�N�{�b�N�X��S��OFF
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub
#End Region

    ''' <summary>
    ''' ���������`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckSearch() As Boolean

        '�����������w�肳��Ă��Ȃ���΁A�G���[
        If txtCoil.Text.Trim = "" And txtKanriNo.Text.Trim = "" And
           Not chkDateSet.Checked And Not chkTimeSet.Checked Then
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �������ʃO���b�h ��\���ؑ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeDspCol()

        Dim nTypeCol As Integer     '�r��s�J�E���g
        Dim nTypeLoop As Integer    '�t�B���^�[�O���b�h���[�v�p

        nTypeCol = EM_RESULT_HEAD_COL.nDetalStart

        '�\�r
        For nTypeLoop = 0 To dgvDispType.RowCount - 1
            '�`�F�b�N�{�b�N�X�̃`�F�b�N�L���ŕ\���ؑ�
            If Not CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                dgvResult.Columns(nTypeCol).Visible = False
            Else
                dgvResult.Columns(nTypeCol).Visible = True
            End If

            nTypeCol += 1
        Next

        '���r
        For nTypeLoop = 0 To dgvDispType.RowCount - 1
            '�`�F�b�N�{�b�N�X�̃`�F�b�N�L���ŕ\���ؑ�
            If Not CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                dgvResult.Columns(nTypeCol).Visible = False
            Else
                dgvResult.Columns(nTypeCol).Visible = True
            End If

            nTypeCol += 1
        Next

    End Sub

    ''' <summary>
    ''' �R�C������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SearchCoil()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim Adapter As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[

        Dim strSql As String = ""       '�r�P�ʎ擾SQL���쐬�p
        Dim strGroupSql As String = ""  '�O���[�v(�R�C���P��) �擾SQL�쐬�p
        Dim nRowNo As Integer           '�O���[�v�s��

        Dim strKensaO As String = ""    '�����_�\
        Dim strKensaU As String = ""    '�����_��
        Dim strDspKensa As String = ""  '�\�����錟���

        Dim nGridCol As Integer         '�O���b�h��
        Dim nTypeLoop As Integer        '�r�탋�[�v�p

        Dim dsData As New DataSet       '�r��P�ʃf�[�^�ۑ��p
        Dim dsGroupData As New DataSet  '�R�C���P�ʃf�[�^�ۑ��p

        Dim nWork As Integer            '��������H�p �ꎞ���[�N

        Dim drowList() As DataRow       '�f�[�^���E

        Dim nKizuCount As Integer = dgvDispType.RowCount        '�r�퐔

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_INF & "]")
            Exit Sub
        End If

        '�O���[�vSQL���쐬
        Call CreGroupSql(strGroupSql)

        '�r��SQL���쐬
        Call CreateSql(strSql)

        Try
            '�O���b�h����U������
            dgvResult.RowCount = 0

            '�O���[�v�pSQL���s
            Adapter = tcls_DB.SelectAdapter(strGroupSql)

            '�O���[�v�p�f�[�^���f�[�^�Z�b�g�ɓo�^
            Adapter.Fill(dsGroupData)

            'SQL���s
            Adapter = tcls_DB.SelectAdapter(strSql)

            '�f�[�^���f�[�^�Z�b�g�ɓo�^
            Adapter.Fill(dsData)

            '�R�C���P�ʂŃ��[�v
            For nRowNo = 0 To dsGroupData.Tables(0).Rows.Count - 1
                Dim nCoilLen As Integer = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("����������"))

                '�O���b�h���{1�s
                dgvResult.RowCount += 1

                '�ʔ̓���
                dgvResult(EM_RESULT_HEAD_COL.nTsuNit, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�o�^���t").ToString

                '�R�C��No
                dgvResult(EM_RESULT_HEAD_COL.nCoilNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("���YNo").ToString

                '����������
                dgvResult(EM_RESULT_HEAD_COL.nLen, nRowNo).Value = Format(CSng(nCoilLen / 1000), "#0.0")

                '�Ǘ�No
                dgvResult(EM_RESULT_HEAD_COL.nKizuKanNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�Ǘ�No").ToString

                '����No
                dgvResult(EM_RESULT_HEAD_COL.nOrderNo, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("����No").ToString

                '�i��
                dgvResult(EM_RESULT_HEAD_COL.nHinCode, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�i��").ToString

                '��ގ�
                dgvResult(EM_RESULT_HEAD_COL.nSyuZai, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("��ގ�").ToString

                '�v�掟�H��
                dgvResult(EM_RESULT_HEAD_COL.nNextCode, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�v�掟�H���R�[�h").ToString

                '�����(�\)
                dgvResult(EM_RESULT_HEAD_COL.nKenKijun, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�����_�\").ToString

                '�ޗ���
                dgvResult(EM_RESULT_HEAD_COL.nCoilAtu, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�ޗ���").ToString

                '�ޗ���
                dgvResult(EM_RESULT_HEAD_COL.nCoilWidth, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("�ޗ���").ToString

                '������
                dgvResult(EM_RESULT_HEAD_COL.nCoilAtuSei, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("������").ToString

                '������
                dgvResult(EM_RESULT_HEAD_COL.nCoilWidSei, nRowNo).Value = dsGroupData.Tables(0).Rows(nRowNo).Item("������").ToString

                dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value = "0"
                dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value = "0"

                nGridCol = EM_RESULT_HEAD_COL.nDetalStart

                For nTypeLoop = 0 To dgvDispType.RowCount - 1

                    '�r��t�B���^�[�Ƀ`�F�b�N�������Ă����r��̂ݏo��
                    If CType(dgvDispType(EM_FILTER_COL.emCheck, nTypeLoop).Value, Boolean) Then
                        drowList = dsData.Tables(0).Select(" �Ǘ�No = '" & dsGroupData.Tables(0).Rows(nRowNo).Item("�Ǘ�No").ToString & "' " &
                            " AND �����r�� = '" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString & "' ")

                        '0�ŏ�����
                        dgvResult(nGridCol, nRowNo).Value = "0"
                        dgvResult(nKizuCount + nGridCol, nRowNo).Value = "0"

                        '�\���敪�� ���[�v�i�ő�2���[�v�j
                        For nListLoop As Integer = 0 To UBound(drowList)
                            '�\���敪�̒l�ŃZ�b�g����s��ς���
                            If drowList(nListLoop).Item("�\���敪").ToString = "0" Then
                                '�\

                                '�r����
                                nWork = CInt(drowList(nListLoop).Item("�r����").ToString)
                                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")

                                '�S�̌�
                                nWork = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value) +
                                  CInt(drowList(nListLoop).Item("�r����").ToString)
                                dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowNo).Value = Format(nWork, "#,##0")

                            Else
                                '��

                                '�r����
                                nWork = CInt(drowList(nListLoop).Item("�r����").ToString)
                                dgvResult(nKizuCount + nGridCol, nRowNo).Value = Format(nWork, "#,##0")

                                '�S�̌�
                                nWork = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value) +
                                  CInt(drowList(nListLoop).Item("�r����").ToString)
                                dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowNo).Value = Format(nWork, "#,##0")

                            End If
                        Next

                    End If

                    '����Z
                    nGridCol += 1
                Next

                nGridCol += nKizuCount

                '�\�ۑ��r��
                nWork = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("�\�ۑ��r��").ToString)
                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")
                nGridCol += 1

                '���ۑ��r��
                nWork = CInt(dsGroupData.Tables(0).Rows(nRowNo).Item("���ۑ��r��").ToString)
                dgvResult(nGridCol, nRowNo).Value = Format(nWork, "#,##0")
                nGridCol += 1

            Next

            '��������
            If 0 <> nRowNo Then
                '�擾�L��
                lblGuide.Text = CStr(nRowNo) & "��������܂���"
            Else
                '�擾����
                lblGuide.Text = "�f�[�^��������܂���ł����B"
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�R�C����񌟍��G���[ [" & ex.Message & "]")
        Finally

            'DB�N���X �㏈��
            If Not tcls_DB Is Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub

    ''' <summary>
    ''' �R�C�������pSQL���쐬
    ''' </summary>
    ''' <param name="strSql"></param>
    ''' <remarks></remarks>
    Private Sub CreateSql(ByRef strSql As String)

        Dim nCnt As Integer         '�O���[�h�t�B���^�[�O���b�h �s�J�E���g
        Dim strWhereAnd As String   '������ �ڑ�������

        'SQL�擾

        strSql = ""
        strSql &= " SELECT "
        strSql &= "    R.�o�^���t, "
        strSql &= "    C.���YNo, "
        strSql &= "    D.�Ǘ�No, "
        strSql &= "    D.�����r��, "
        strSql &= "    D.�\���敪, "
        strSql &= "    D.�r���� "
        strSql &= " FROM "

        '�\�r
        strSql &= " ("
        strSql &= "   SELECT �Ǘ�No, "
        strSql &= "          �����r��, "
        strSql &= "          0 AS �\���敪, "
        strSql &= "          COUNT(1) �r���� "
        strSql &= "   FROM " & DB_DEFECT_INF_T
        strSql &= "   WHERE  �d������ = 0 "

        '�O���[�h�t�B���^�[
        With dgvDispGrade
            For nCnt = 0 To .RowCount - 1
                '�O���[�h�O���b�h�̃`�F�b�N�������Ă��Ȃ��O���[�h�����O
                If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, nCnt).Value, Boolean) Then
                    strSql &= " AND NOT ����O���[�h = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, nCnt).Value.ToString
                End If
            Next nCnt
        End With
        strSql &= "   GROUP BY �Ǘ�No, �����r��"
        strSql &= "   UNION ALL "

        '���r
        strSql &= "   SELECT �Ǘ�No, "
        strSql &= "          �����r��, "
        strSql &= "          1 AS OU_KBN, "
        strSql &= "          COUNT(1) �r���� "
        strSql &= "   FROM " & DB_DEFECT_INF_B
        strSql &= "   WHERE  �d������ = 0 "

        '�O���[�h�t�B���^�[
        With dgvDispGrade
            For nCnt = 0 To .RowCount - 1
                '�O���[�h�O���b�h�̃`�F�b�N�������Ă��Ȃ��O���[�h�����O
                If Not CType(dgvDispGrade(EM_FILTER_COL.emCheck, nCnt).Value, Boolean) Then
                    strSql &= " AND NOT ����O���[�h = " & dgvDispGrade(EM_FILTER_COL.emEdasysID, nCnt).Value.ToString
                End If
            Next nCnt
        End With

        strSql &= "   GROUP BY �Ǘ�No, �����r��"
        strSql &= " ) AS D "

        '�R�C�����A�R�C�����тƌ���
        strSql &= " INNER JOIN " & DB_COIL_INF & " AS C ON D.�Ǘ�No = C.�Ǘ�No "
        strSql &= " INNER JOIN " & DB_COIL_RESULT & " AS R ON D.�Ǘ�No = R.�Ǘ�No "

        strWhereAnd = " WHERE "

        '' �擾����
        '�R�C��No(��No)
        If "" <> txtCoil.Text.Trim Then
            strSql &= strWhereAnd & " C.���YNo like '" & txtCoil.Text.Trim & "' "
            strWhereAnd = " AND "
        End If

        '�Ǘ�No
        If "" <> txtKanriNo.Text.Trim Then
            strSql &= strWhereAnd & " D.�Ǘ�No like '" & txtKanriNo.Text.Trim & "' "
            strWhereAnd = " AND "
        End If


        '�����i�J�n�j
        If chkDateSet.Checked Then
            '���w��
            strSql &= strWhereAnd & " R.�o�^���t >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '�����i�I���j
        If chkDateSet.Checked Then
            '���w��
            strSql &= strWhereAnd & " R.�o�^���t <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' �I�[�_�[��
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY R.�o�^���t ASC",
         strSql & " ORDER BY R.�o�^���t DESC"))

        strSql &= " ,D.�Ǘ�No ASC"

    End Sub

    ''' <summary>
    ''' �R�C�������O���[�v(�R�C���P��)�pSQL���쐬
    ''' </summary>
    ''' <param name="strSql">SQL��</param>
    ''' <remarks></remarks>
    Private Sub CreGroupSql(ByRef strSql As String)

        Dim strWhereAnd As String

        'SQL�쐬

        strSql = ""
        strSql &= " SELECT "
        strSql &= "    ISNULL(R.�o�^���t,'') AS �o�^���t "
        strSql &= "    ,ISNULL(C.���YNo, '') AS ���YNo "
        strSql &= "    ,ISNULL(C.����No, '') AS ����No  "
        strSql &= "    ,ISNULL(R.�Ǘ�No, '') AS �Ǘ�No  "
        strSql &= "    ,ISNULL(R.�\�ۑ��r��, 0) AS �\�ۑ��r��  "
        strSql &= "    ,ISNULL(R.���ۑ��r��, 0) AS ���ۑ��r��  "
        strSql &= "    ,ISNULL(R.����������,0) as ���������� "
        strSql &= "    ,ISNULL(C.�ޗ���,0) as �ޗ��� "
        strSql &= "    ,ISNULL(C.�ޗ���,0) as �ޗ��� "
        strSql &= "    ,ISNULL(C.������,0) as ������ "
        strSql &= "    ,ISNULL(C.������,0) as ������ "
        strSql &= "    ,ISNULL(C.�i��,'') as �i�� "
        strSql &= "    ,ISNULL(C.��ގ�,'') as ��ގ� "
        strSql &= "    ,ISNULL(C.�v�掟�H���R�[�h,'') as �v�掟�H���R�[�h "
        strSql &= "    ,ISNULL(C.�����_�\,'') as �����_�\ "
        strSql &= " FROM " & DB_COIL_RESULT & " AS R "
        strSql &= " INNER JOIN " & DB_COIL_INF & " AS C ON R.�Ǘ�No = C.�Ǘ�No "

        strWhereAnd = " WHERE "

        '�R�C��No(��No)
        If "" <> txtCoil.Text.Trim Then
            strSql &= strWhereAnd & " C.���YNo like '" & txtCoil.Text.Trim & "' "
            strWhereAnd = " AND "
        End If

        '�Ǘ�No
        If "" <> txtKanriNo.Text.Trim Then
            strSql &= strWhereAnd & " R.�Ǘ�No like '" & txtKanriNo.Text.Trim & "' "
            strWhereAnd = " AND "
        End If


        '�����i�J�n�j
        If chkDateSet.Checked Then
            '���w��
            strSql &= strWhereAnd & " R.�o�^���t >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '�����i�I���j
        If chkDateSet.Checked Then
            '���w��
            strSql &= strWhereAnd & " R.�o�^���t <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            strWhereAnd = " AND "
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' �I�[�_�[��
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY R.�o�^���t ASC",
         strSql & " ORDER BY R.�o�^���t DESC"))

    End Sub

    ''' <summary>
    ''' �r��E�O���[�h�t�B���^�[�����\��
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h</param>
    ''' <param name="strTable">�e�[�u����</param>
    ''' <param name="strSection">�Z�N�V������</param>
    ''' <param name="strKey">�L�[</param>
    ''' <remarks></remarks>
    Private Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String,
                                ByVal strSection As String, ByVal strKey As String)

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)    'DB�N���X
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""               'Sql���쐬�p
        Dim nCnt As Integer = 0                 '�O���b�h�s
        Dim strSetString As String = ""

        'INI�t�@�C���̐ݒ���擾
        strSetString = tClassLibrary.tMod.ReadIniStr(strSection, strKey, m_strIniName)

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT Name, "
        strSQL &= "        EdasysID "
        '�r��̂Ƃ��̓}�b�v���̂��擾
        If INI_TYPE_KEY = strKey Then
            strSQL &= "    ,�}�b�v�\������ "
        End If
        strSQL &= " FROM " & strTable
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()

                dgv.RowCount += 1   '�\���s����ݒ�

                ''�ݒ�s�� �w��
                dgv(EM_FILTER_COL.emName, nCnt).ReadOnly = True

                '�f�[�^�Z�b�g
                dgv(EM_FILTER_COL.emName, nCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.emName, nCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.emEdasysID, nCnt).Value = sqlRead.GetInt32(1)
                If INI_TYPE_KEY = strKey Then
                    dgv(EM_FILTER_COL.emMapName, nCnt).Value = sqlRead.GetString(2)
                End If

                '���o�^�܂��́A1���ݒ肳��Ă���ꍇ�́A�`�F�b�N������
                If "" = strSetString OrElse "1" = Mid(strSetString, nCnt + 1, 1) Then
                    dgv(EM_FILTER_COL.emCheck, nCnt).Value = True
                Else
                    dgv(EM_FILTER_COL.emCheck, nCnt).Value = False
                End If

                nCnt += 1

            End While

        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()

            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub

#Region "�O���b�h�n"
    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h������
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            ' '' ��`
            '.ColumnHeadersHeight = 25										' �w�b�_�[�̍���
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .ScrollBars = ScrollBars.Vertical                               ' �X�N���[���o�[�̕\��

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub

    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h���`
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <param name="strName">2��ڂ̍��ږ�</param>
    ''' <remarks></remarks>
    Private Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
        With dgv
            ''''' ��ǉ�
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' �`�F�b�N�{�b�N�X
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����

            '''' ���`
            '1���:�`�F�b�N�{�b�N�X
            .Columns(EM_FILTER_COL.emCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.emCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.emCheck).Width = 20
            .Columns(EM_FILTER_COL.emCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2���:�r�햼��
            .Columns(EM_FILTER_COL.emName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.emName).Width = .ClientSize.Width - 20 - 3
            .Columns(EM_FILTER_COL.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:EdasysID
            .Columns(EM_FILTER_COL.emEdasysID).Visible = False  '��\��

            '4���:����
            .Columns(EM_FILTER_COL.emMapName).Visible = False  '��\��
        End With
    End Sub

    ''' <summary>
    ''' �������ʃO���b�h ������
    ''' </summary>
    ''' <param name="dgv">�������ʃO���b�h</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize_ResultGrid(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)

        '''' �Ǝ��̍��ڂ�ݒ�
        With dgv
            ' '' ��`
            .ColumnHeadersHeight = 40                                       ' �w�b�_�[�̍���
            '��w�b�_������
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .ScrollBars = ScrollBars.Both
            .ReadOnly = True

            '' �I���Z���F��`                                                      '�X�N���[���o�[
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 0
        End With
    End Sub

    ''' <summary>
    ''' �������ʃO���b�h ���`
    ''' </summary>
    ''' <param name="dgv">�������ʃO���b�h</param>
    ''' <remarks></remarks>
    Private Sub dgvInitColumn_ResultGrid(ByVal dgv As DataGridView)

        Dim nTypeLoop As Integer = 0
        Dim nColNum As Integer

        With dgv

            '''' ���`
            '1���:�ʔ̓���
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �ʔ���
            '.Columns(EM_RESULT_HEAD_COL.nTsuNit).HeaderText = "�ʔ̓���"
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).HeaderText = "�ʔ���"
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).Width = 180
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nTsuNit).Frozen = True

            '2���:�R�C��No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �R�C��No
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).HeaderText = "���YNo"
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).Width = 120
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilNo).Frozen = True

            '3���:����������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����������
            .Columns(EM_RESULT_HEAD_COL.nLen).HeaderText = "�R�C����" & vbCrLf & "[m]"
            .Columns(EM_RESULT_HEAD_COL.nLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nLen).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nLen).SortMode = DataGridViewColumnSortMode.NotSortable

            '4���:�Ǘ�No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �Ǘ�No
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nKizuKanNo).Visible = False

            '5���:����No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����No
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nOrderNo).Visible = False

            '6���:�i��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �i��
            .Columns(EM_RESULT_HEAD_COL.nHinCode).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nHinCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nHinCode).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nHinCode).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nHinCode).Visible = False

            '7���:��ގ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ��ގ�
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nSyuZai).Visible = False

            '8���:�v�掟�H��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �v�掟�H��
            .Columns(EM_RESULT_HEAD_COL.nNextCode).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nNextCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_HEAD_COL.nNextCode).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nNextCode).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nNextCode).Visible = False

            '9���:�����(�\)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �����(�\)
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nKenKijun).Visible = False

            '10���:�ޗ���
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �ޗ���
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilAtu).Visible = False

            '11���:�ޗ���
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �ޗ���
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilWidth).Visible = False

            '12���:������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ������
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilAtuSei).Visible = False

            '13���:������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ������
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).HeaderText = ""
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).Width = 0
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_HEAD_COL.nCoilWidSei).Visible = False

            '14���:�S�̌���(�\)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �S�̌���(�\)
            .Columns(EM_RESULT_HEAD_COL.nTotalO).HeaderText = "�S�̌���" & vbCrLf & "(�\)"
            .Columns(EM_RESULT_HEAD_COL.nTotalO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nTotalO).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nTotalO).SortMode = DataGridViewColumnSortMode.NotSortable

            '15���:�S�̌���(��)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �S�̌���(��)
            .Columns(EM_RESULT_HEAD_COL.nTotalU).HeaderText = "�S�̌���" & vbCrLf & "(��)"
            .Columns(EM_RESULT_HEAD_COL.nTotalU).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_HEAD_COL.nTotalU).Width = 80
            .Columns(EM_RESULT_HEAD_COL.nTotalU).SortMode = DataGridViewColumnSortMode.NotSortable

            nColNum += EM_RESULT_HEAD_COL.nDetalStart

            For nTypeLoop = 0 To dgvDispType.RowCount - 1
                '16��ڈȍ~�`:�r��(�\)
                .Columns.Add(New DataGridViewTextBoxColumn)
                .Columns(nColNum).HeaderText = dgvDispType(EM_FILTER_COL.emMapName, nTypeLoop).Value.ToString & vbCrLf &
                                               "(�\)" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString
                .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(nColNum).Width = 78

                .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable

                nColNum += 1
            Next


            For nTypeLoop = 0 To dgvDispType.RowCount - 1
                '�\�{16��ڈȍ~�`:�r��(��)
                .Columns.Add(New DataGridViewTextBoxColumn)
                .Columns(nColNum).HeaderText = dgvDispType(EM_FILTER_COL.emMapName, nTypeLoop).Value.ToString & vbCrLf &
                                               "(��)" & dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value.ToString
                .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .Columns(nColNum).Width = 78
                .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable

                nColNum += 1
            Next

            '�ۑ���(�\)
            .Columns.Add(New DataGridViewTextBoxColumn)
            .Columns(nColNum).HeaderText = "�ۑ���" & vbCrLf & "(�\)"
            .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(nColNum).Width = 80
            .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable
            nColNum += 1

            '�ۑ���(��)
            .Columns.Add(New DataGridViewTextBoxColumn)
            .Columns(nColNum).HeaderText = "�ۑ���" & vbCrLf & "(��)"
            .Columns(nColNum).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(nColNum).Width = 80
            .Columns(nColNum).SortMode = DataGridViewColumnSortMode.NotSortable
            nColNum += 1

        End With
    End Sub
#End Region

#Region "�r���g�����h�O���t��ʕ\���n"
    ''' <summary>
    ''' �r���g�����h�O���t��ʗp�f�[�^�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Function SetTrandGraphData() As Boolean

        Dim typKizuInf As New TYP_KIZU_TRAND_INF

        ' �������ʂ�����
        If dgvResult.ColumnCount = 0 Then
            Return False
        End If

        '�R�C�����I��
        If 1 > dgvResult.SelectedRows.Count Then
            txtMsg.Text = "�R�C����I�����Ă��������B"
            Return False
        End If

        '�f�[�^������
        typKizuInf.initialize()

        '�f�[�^�擾
        If Not GetKizuTrandData(typKizuInf) Then
            txtMsg.Text = "�r���g�����h���̎擾�Ɏ��s���܂����B"
            Return False
        End If

        '�r���g�����h���̐ݒ�
        m_typKizuTrandInf = typKizuInf

        Return True

    End Function

    ''' <summary>
    ''' �r���g�����h��ʗp�f�[�^�擾
    ''' </summary>
    ''' <param name="typKizuInf">�r���g�����h���</param>
    ''' <remarks></remarks>
    Private Function GetKizuTrandData(ByRef typKizuInf As TYP_KIZU_TRAND_INF) As Boolean

        Try
            '���������擾
            typKizuInf.typSearch.nSelCoilIdx = dgvResult.SelectedRows.Item(0).Index

            '�������{���̊J�n���t�E�I�����t�擾
            typKizuInf.typSearch.strStartDate = IIf(Nothing = m_dtSearchDateStr, String.Empty, m_dtSearchDateStr.ToString("yyyy/MM/dd"))
            typKizuInf.typSearch.strEndDate = IIf(Nothing = m_dtSearchDateEnd, String.Empty, m_dtSearchDateEnd.ToString("yyyy/MM/dd"))
            typKizuInf.typSearch.strStartTime = IIf(Nothing = m_dtSearchTimeStr, String.Empty, m_dtSearchTimeStr.ToString("HH:mm:ss"))
            typKizuInf.typSearch.strEndTime = IIf(Nothing = m_dtSearchTimeEnd, String.Empty, m_dtSearchTimeEnd.ToString("HH:mm:ss"))

            '�R�C�����擾
            Dim typData As TYP_KIZU_TRAND_COILINF = Nothing  '�r���g�����h���

            For nRowCnt As Integer = 0 To dgvResult.Rows.Count - 1
                '�\���̏�����
                typData.initialize()

                ' �R�C�����
                typData.strOrderNo = dgvResult(EM_RESULT_HEAD_COL.nOrderNo, nRowCnt).Value.ToString                 ' ����No
                typData.strCoilNo = dgvResult(EM_RESULT_HEAD_COL.nCoilNo, nRowCnt).Value.ToString                   ' ���YNo
                typData.strHinCode = dgvResult(EM_RESULT_HEAD_COL.nHinCode, nRowCnt).Value.ToString                 ' �i��
                typData.strSyuZai = dgvResult(EM_RESULT_HEAD_COL.nSyuZai, nRowCnt).Value.ToString                   ' ��ގ�
                typData.strNextCode = dgvResult(EM_RESULT_HEAD_COL.nNextCode, nRowCnt).Value.ToString               ' �v�掟�H��
                typData.strKenKijun = dgvResult(EM_RESULT_HEAD_COL.nKenKijun, nRowCnt).Value.ToString               ' ������i�\�j
                typData.nCoilAtu = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilAtu, nRowCnt).Value)                      ' �ޗ���
                typData.nCoilWidth = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilWidth, nRowCnt).Value)                  ' �ޗ���
                typData.nCoilAtuSei = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilAtuSei, nRowCnt).Value)                ' ������
                typData.nCoilWidSei = CInt(dgvResult(EM_RESULT_HEAD_COL.nCoilWidSei, nRowCnt).Value)                ' ������
                typData.nOKizuCnt = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalO, nRowCnt).Value)                      ' �\�r����
                typData.nUKizuCnt = CInt(dgvResult(EM_RESULT_HEAD_COL.nTotalU, nRowCnt).Value)                      ' ���r����
                typData.nAKizuCnt = typData.nOKizuCnt + typData.nUKizuCnt                                           ' ���v�r����

                ' �r���
                Dim dLen As Single = CSng(dgvResult(EM_RESULT_HEAD_COL.nLen, nRowCnt).Value.ToString)              ' ����������
                Dim nCnt As Integer = EM_RESULT_HEAD_COL.nDetalStart
                Dim typKizu As TYP_KIZU_TRAND_KIZUINF = Nothing  '�r���

                For nTypeLoop As Integer = 0 To dgvDispType.RowCount - 1
                    ' �r���̒l��ݒ�
                    typKizu.nEdasysID = CInt(dgvDispType(EM_FILTER_COL.emEdasysID, nTypeLoop).Value)
                    typKizu.nOKizuCnt = CInt(dgvResult(nCnt, nRowCnt).Value)
                    typKizu.nUKizuCnt = CInt(dgvResult(nCnt + dgvDispType.RowCount, nRowCnt).Value)
                    typKizu.nAKizuCnt = typKizu.nOKizuCnt + typKizu.nUKizuCnt

                    If 0 <> dLen Then
                        typKizu.dOKizuRate = (typKizu.nOKizuCnt / dLen) * 100
                        typKizu.dUKizuRate = (typKizu.nUKizuCnt / dLen) * 100
                        typKizu.dAKizuRate = (typKizu.nAKizuCnt / dLen) * 100
                    Else
                        typKizu.dOKizuRate = 0
                        typKizu.dUKizuRate = 0
                        typKizu.dAKizuRate = 0
                    End If

                    ' ���擾�r��
                    If False = typKizuInf.lstAllKizu.Contains(typKizu.nEdasysID) AndAlso 0 < typKizu.nAKizuCnt Then
                        typKizuInf.lstAllKizu.Add(typKizu.nEdasysID)    ' �r�ꗗ���r��ǉ�
                    End If

                    ' �r���ő�l�̍X�V
                    If typKizu.nOKizuCnt > typKizuInf.typSearch.nOKizuCntMax Then
                        typKizuInf.typSearch.nOKizuCntMax = typKizu.nOKizuCnt
                    End If
                    If typKizu.nUKizuCnt > typKizuInf.typSearch.nUKizuCntMax Then
                        typKizuInf.typSearch.nUKizuCntMax = typKizu.nUKizuCnt
                    End If
                    If typKizu.nAKizuCnt > typKizuInf.typSearch.nAKizuCntMax Then
                        typKizuInf.typSearch.nAKizuCntMax = typKizu.nAKizuCnt
                    End If

                    ' �r�������ő�l�̍X�V
                    If typKizu.dOKizuRate > typKizuInf.typSearch.dOKizuRateMax Then
                        typKizuInf.typSearch.dOKizuRateMax = typKizu.dOKizuRate
                    End If
                    If typKizu.dUKizuRate > typKizuInf.typSearch.dUKizuRateMax Then
                        typKizuInf.typSearch.dUKizuRateMax = typKizu.dUKizuRate
                    End If
                    If typKizu.dAKizuRate > typKizuInf.typSearch.dAKizuRateMax Then
                        typKizuInf.typSearch.dAKizuRateMax = typKizu.dAKizuRate
                    End If


                    ' �r��񃊃X�g�ǉ�
                    typData.lstKizuInf.Add(typKizu)

                    nCnt += 1
                Next

                ' �R�C����񃊃X�g�ǉ�
                typKizuInf.lstCoil.Add(typData)
            Next

            typKizuInf.lstAllKizu.Sort()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r���g�����h���擾�G���[ [" & ex.Message & "]")
            Return False
        End Try

    End Function

#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �t�B���^�[�O���b�h �����`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h���� �n���h�����蓖��
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvFilter_AddHandler(ByVal dgv As tCnt.tCnt_DataGridView)
        AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
        AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv.dgvData_CurrentCellDirtyStateChanged
    End Sub

    ''' <summary>
    ''' �Z���`�F���W����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, tCnt.tCnt_DataGridView)
        '�w�b�_�͏������Ȃ�
        If e.RowIndex = -1 Then Exit Sub

        If e.ColumnIndex = EM_FILTER_COL.emCheck Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '' �`�F�b�N��t������
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
            Else
                '' �`�F�b�N���O������
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If

    End Sub

    ''' <summary>
    ''' �O���b�h�̃`�F�b�N�{�b�N�X�Z�b�g
    ''' </summary>
    ''' <param name="dgvData">�f�[�^�O���b�h</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' �O���b�h�̑S�Ẵ`�F�b�N�{�b�N�X��ݒ�
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
        Next ii

    End Sub

    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
    ''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
    ''' <remarks></remarks>
    Private Sub CsvOut(ByVal strFolder As String, ByVal strName As String)
        Dim strFPath As String
        Dim strMsg As String = ""
        Dim strData As String = ""                      '1�s�̃f�[�^
        Dim strWk() As String
        Dim strWkData As String

        '' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '' �t�H���_�̗L���`�F�b�N(������������)
        tMod.CheckFolder(strFolder)

        '' CSV�o��
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' �I�[�v��
                cls_Stram.Open()

                Dim ii As Integer
                Dim jj As Integer

                '�w�b�_���o��
                strData = ""
                For ii = 0 To dgvResult.ColumnCount - 1

                    '�B����͏o�͂��Ȃ��悤�ɂ���
                    If dgvResult.Columns(ii).Visible = True Then
                        '���s�R�[�h�����ׂĎ�菜��
                        strWk = dgvResult.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
                        strWkData = ""
                        For jj = 0 To strWk.Length - 1
                            strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
                        Next jj

                        strData &= Trim(strWkData) & ","
                    End If

                Next ii

                '�Ō�̃^�u��r��
                strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)

                '�f�[�^�o��
                For ii = 0 To dgvResult.Rows.Count - 1
                    strData = ""
                    For jj = 0 To dgvResult.ColumnCount - 1

                        '�B����͏o�͂��Ȃ��悤�ɂ���
                        If dgvResult.Columns(jj).Visible = True Then

                            strWkData = ""
                            '���l�̃J���}�����ׂĎ�菜��
                            strWk = dgvResult(jj, ii).Value.ToString.Split(","c)
                            For nLoop As Integer = 0 To strWk.Length - 1
                                strWkData &= strWk(nLoop)
                            Next
                            '���ڂ��J���}�ŘA������
                            strData &= strWkData & ","
                        End If

                    Next jj

                    '�Ō�̃J���}��r��
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

    ''' <summary>
    ''' �r��A�O���[�h�t�B���^�[�̃`�F�b�N�ۑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SaveCheck()

        Dim strSetString As String
        Dim ii As Integer

        '�r��,�O���[�h�t�B���^�[�̏�����INI�t�@�C���֓o�^
        With dgvDispType
            strSetString = ""
            For ii = 0 To dgvDispType.RowCount - 1
                strSetString = CStr(IIf(CType(dgvDispType(EM_FILTER_COL.emCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
            Next ii
            tClassLibrary.tMod.WriteIni(SO_GAMENN, INI_TYPE_KEY, m_strIniName, strSetString)
        End With


        With dgvDispGrade
            strSetString = ""
            For ii = 0 To dgvDispGrade.RowCount - 1
                strSetString = CStr(IIf(CType(dgvDispGrade(EM_FILTER_COL.emCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
            Next ii
            tClassLibrary.tMod.WriteIni(SO_GAMENN, INI_GRADE_KEY, m_strIniName, strSetString)
        End With

    End Sub

    ''' <summary>
    ''' �����o�C�g���擾�֐�
    ''' </summary>
    ''' <param name="strText">�Ώە�����</param>
    ''' <returns>������o�C�g��</returns>
    ''' <remarks></remarks>
    Private Function LenB(ByVal strText As String) As Integer
        Dim ByteArray As Byte() = System.Text.Encoding.GetEncoding("SJIS").GetBytes(strText)
        Return ByteArray.Length
    End Function

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
                Case 5 - 1                                                      ' ����
                    btnSearch_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 6 - 1                                                      ' CSV�o��
                    btnCsv_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 7 - 1                                                      ' �O���t�o��
                    btnGraph_Click(Nothing, Nothing)
                    '---------------------------------------------------------------
                Case 12 - 1														' �I��
					btnEnd_Click(Nothing, Nothing)
			End Select
		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�t�@���N�V�����C�x���g�ُ� [{0}]", ex.Message))
		End Try
	End Sub

    '***************************************************************************
    ' �}�E�X�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �������ʃ_�u���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvResult_CellDoubleClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvResult.CellDoubleClick
        Try
            If e.RowIndex < 0 Then                                              ' �I���s���s���Ȃ�I���
                Exit Sub
            End If

            btnGraph_Click(Nothing, Nothing)                                    ' �O���t�o�̓{�^���������Ɠ�������
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�������ʃ_�u���N���b�N�C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub
End Class
