Option Strict Off

Imports tClassLibrary

Public Class frmMain

    ''' <summary>
    ''' �������уv���R���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPro_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPro.Click

        Dim frm As New frmMixedRateTool(0)

        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----

    End Sub

    ''' <summary>
    ''' ��������FF�N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFF.Click

        Dim frm As New frmFF(0)

        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----


    End Sub
    ''' <summary>
    ''' �̐����уN���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSaisun_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSaisun.Click

        Dim frm As New frmSaisun(0)

        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----

    End Sub
    ''' <summary>
    ''' �J�b�g���уN���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCut_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCut.Click

        Dim frm As New frmCut(0)


        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----


    End Sub

    ''' <summary>
    ''' �O�H�����я��N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFFRecv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFFRecv.Click


        Dim frm As New frmFFRecv(0)


        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----

    End Sub


    ''' <summary>
    ''' �R�C�����`���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCoilInfo_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoilInfo.Click

        Dim frm As New frmCoilInfo(0)

        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
        'frm.ShowDialog()
        'frm.Dispose()
        'frm = Nothing
        frm.Show()
        '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----

    End Sub

    '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
    ''' <summary>
    ''' FF������ԓ`���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFFStstus_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFFStstus.Click
        Dim frm As New frmKensaStatus(0, "")

        frm.Show()
        'frm.Dispose()
        'frm = Nothing
    End Sub
    '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----
End Class
