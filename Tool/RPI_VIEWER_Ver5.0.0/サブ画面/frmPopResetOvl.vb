Imports tClassLibrary

Public Class frmPopResetOvl


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    Public Event tEv_ReSetKizuOvl()										' �I�[�o�[���C�ăZ�b�g



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�

    ' �f�[�^
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' �r��񃊃X�g


    ' �R�C���P��
    Private m_nStartCamSet As Integer				' �ŏ��̃J�����Z�b�g
    Private m_nOffSetVal() As Integer				' �J��������



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B

    ''' <summary>
    ''' �r��񃊃X�g
    ''' </summary>
    ''' <param name="lst">�r��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPopResetOvl_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
    End Sub


    ''' <summary>
    ''' �R�C���ύX
    ''' </summary>
    ''' <param name="nStartCamSet">�ŏ��̃J�����Z�b�g</param>
    ''' <param name="nCamSetNum">�J�����Z�b�g��</param>
    ''' <remarks></remarks>
    Public Sub ChangeCoil(ByVal nStartCamSet As Integer, ByVal nCamSetNum As Integer)
        ' �����o�[������
        m_nStartCamSet = nStartCamSet
        ReDim m_nOffSetVal(nCamSetNum - 1)
        lblNowOffset.Text = ""




        Call SetCamItem(nStartCamSet, nCamSetNum)
        Call ClearOffSet()
    End Sub





    ''' <summary>
    ''' �I�t�Z�b�g����
    ''' </summary>
    Private Sub btnClearOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClearOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Call ClearOffSet()

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub

    ''' <summary>
    ''' �w�蔽�f
    ''' </summary>
    Private Sub btnSetOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        Dim nVal As Integer = CInt(nudOffset.Value)

        Call SetOffSet(nCamPos, 0, nVal)

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub
    ''' <summary>
    ''' ���Z���f
    ''' </summary>
    Private Sub btnAddOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        Dim nVal As Integer = CInt(nudOffset.Value)

        Call SetOffSet(nCamPos, 1, nVal)

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub


    ''' <summary>
    ''' ���݃I�t�Z�b�g�ʕ\��
    ''' </summary>
    Private Sub cmbCam_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbCam.SelectedIndexChanged
        Call DspNowOffset()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' ���݃I�t�Z�b�g�ʕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspNowOffset()
        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        If -1 = nCamPos Then
            lblNowOffset.Text = ""
        Else
            lblNowOffset.Text = m_nOffSetVal(nCamPos).ToString

        End If
    End Sub



    ''' <summary>
    ''' �J�������Z�b�g
    ''' </summary>
    ''' <param name="nStartCamSet">�ŏ��̃J�����Z�b�g</param>
    ''' <param name="nCamSetNum">�J�����Z�b�g��</param>
    ''' <remarks></remarks>
    Private Sub SetCamItem(ByVal nStartCamSet As Integer, ByVal nCamSetNum As Integer)
        cmbCam.Items.Clear()

        For ii As Integer = -1 To nCamSetNum - 1
            Dim sName As String


            If -1 = ii Then
                sName = "�S��"
            Else
                sName = (nStartCamSet + ii).ToString
            End If

            ' ���X�g�ɍs�ǉ�
            Dim item As New tClass_CmbItem(ii, sName)
            Call cmbCam.Items.Add(item)
        Next
        cmbCam.SelectedIndex = 0
    End Sub



    ''' <summary>
    ''' �I�t�Z�b�g�ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearOffSet()
        For ii As Integer = 0 To m_lstDetect.Count - 1		' �l�����������邽�߁A�R���N�V�����ł�NG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)

            data.nFrameNo = data.nFrameNoBase

            m_lstDetect(ii) = data			' �X�V
        Next

        ' ����Z�b�g
        For ii As Integer = 0 To m_nOffSetVal.Length - 1
            m_nOffSetVal(ii) = 0
        Next
    End Sub

    ''' <summary>
    ''' �I�t�Z�b�g�ʃZ�b�g
    ''' </summary>
    ''' <param name="nCamPos">�J�����ʒu(-1:�S�� 0�`:�w��̂�)</param>
    ''' <param name="nMode">(0:��Βl 1:���Βl)</param>
    ''' <param name="nVal">�I�t�Z�b�g��</param>
    ''' <remarks></remarks>
    Private Sub SetOffSet(ByVal nCamPos As Integer, ByVal nMode As Integer, ByVal nVal As Integer)
        Dim nS As Integer
        Dim nE As Integer

        If -1 = nCamPos Then
            nS = 0
            nE = m_nOffSetVal.Length - 1
        Else
            nS = nCamPos
            nE = nCamPos
        End If

        ' �l���f
        For ii As Integer = 0 To m_lstDetect.Count - 1		' �l�����������邽�߁A�R���N�V�����ł�NG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)


            If nS <= data.nCamPos And nE >= data.nCamPos Then
                If 0 = nMode Then
                    ' ���
                    data.nFrameNo = data.nFrameNoBase + nVal
                Else
                    ' ����
                    data.nFrameNo += nVal
                End If
            End If


            m_lstDetect(ii) = data			' �X�V
        Next


        ' ����Z�b�g
        For ii As Integer = nS To nE
            If 0 = nMode Then
                ' ���
                m_nOffSetVal(ii) = nVal
            Else
                ' ����
                m_nOffSetVal(ii) += nVal
            End If
        Next
    End Sub


End Class