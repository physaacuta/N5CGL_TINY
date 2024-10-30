Public Class frmPopDspSet

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���J�C�x���g
    Public Event tEv_ReSetKizuOvl()                                     ' �I�[�o�[���C�ăZ�b�g

    Private m_doc As DrawMemoriProperties

    '/ /////////////////////////////////////////////////////////////////////////////////
    '''' ���J�ϐ�
    Public g_bTypeDsp As Boolean = True                                     ' �r���\��
    Public g_nTypeFSize As Integer = 12                                     ' �������T�C�Y

    Public Sub SetMemoriProp(doc As DrawMemoriProperties)
        m_doc = doc
        nudScalCntX.Value = m_doc.g_nScalCntX
        nudScalCntY.Value = m_doc.g_nScalCntY
        nudScalStringX.Value = m_doc.g_nScalStringX
        nudScalStringY.Value = m_doc.g_nScalStringY
        nudScalFsize.Value = m_doc.g_ScaleFont.size
    End Sub

    Private Sub frmPopDspSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���I�t�Z�b�g
    Private Sub trbXoffset_ValueChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles trbXoffset.ValueChanged
        If m_doc IsNot Nothing Then
            m_doc.g_nXoffset = trbXoffset.Value
            m_doc.Emit()
            lblXoffset.Text = trbXoffset.Value.ToString
        End If
    End Sub
    ''' <summary>
    ''' �����l
    ''' </summary>
    Private Sub btnXoffset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnXoffset.Click
        trbXoffset.Value = 0
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �������ύX
    Private Sub btnScal_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnScal.Click
        m_doc.g_nScalCntX = CInt(nudScalCntX.Value)
        m_doc.g_nScalCntY = CInt(nudScalCntY.Value)
        m_doc.g_nScalStringX = CInt(nudScalStringX.Value)
        m_doc.g_nScalStringY = CInt(nudScalStringY.Value)
        m_doc.g_ScaleFont.size = CInt(nudScalFsize.Value)
        m_doc.Emit()
    End Sub
    Private Sub Label7_Click(sender As Object, e As EventArgs) Handles Label7.Click
        m_doc.g_ScaleFont.QueryDialog()
        m_doc.Emit()
        nudScalFsize.Value = m_doc.g_ScaleFont.size
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r���\��
    Private Sub chbTypeDsp_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbTypeDsp.CheckedChanged, nudTypeFsize.ValueChanged
        Call SetType()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub
    Private Sub SetType()
        g_bTypeDsp = ckbTypeDsp.Checked
        g_nTypeFSize = CInt(nudTypeFsize.Value)
    End Sub


End Class


