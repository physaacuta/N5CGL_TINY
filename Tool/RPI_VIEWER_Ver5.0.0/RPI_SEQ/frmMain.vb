'Option Strict On	'���L�������A�N�Z�X�����邽�߁AOff��
Option Strict Off

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    Private WithEvents mcls_RecvMail As clsRecvMail = Nothing						' ���[���X���b�g��M
    Private m_memTrkPtr As IntPtr = IntPtr.Zero							' ���L�������|�C���^(���H���ԘA�g)
    Private m_cntItem(TBL_RPI_REC_MAX - 1) As cntItem					' �e�A�C�e���R���g���[��


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' ���[���X���b�g
        mcls_RecvMail = New clsRecvMail(Me)


        '''' ���L�������m�� (�������Ȃ玩���Ő����B�����ς݂Ȃ�ڑ�����)
        m_memTrkPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_TRK, TBL_RPI_TRK_NAME)

        ' �����ݒ� (�Ǎ����s�΍�)
        Dim tMem As New TYPE_TRK        ' ���L�������S�f�[�^
        tMem.initialize()
        tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMem)




        '''' �R���g���[������
        '' ���[�v���g����悤��
        m_cntItem(0) = cntItem1
        m_cntItem(1) = cntItem2
        m_cntItem(2) = cntItem3
        m_cntItem(3) = cntItem4
        m_cntItem(4) = cntItem5
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Prop_No = ii
        Next


        '' ���j�^���擾
        Call DspSelectInit()



        '''' �����J�n
        timCycle.Start()

        '''' �f�t�H���g��Ԃ��m��
        ckbSeq.Checked = True
    End Sub

    ''' <summary>
    ''' �I��
    ''' </summary>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        '' ������~
        timCycle.Stop()


        '' �R���g���[���J��
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Dispose()
        Next

        ' ���L�������J��
        If m_memTrkPtr <> IntPtr.Zero Then
            tClass_GlovalMemory.FreeMapped(m_memTrkPtr, TBL_RPI_TRK_NAME)
        End If

        ' ���[���X���b�g�I��
        If Not mcls_RecvMail Is Nothing Then
            mcls_RecvMail.Dispose()
            mcls_RecvMail = Nothing
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[���X���b�g��M

    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="nEvtNo"></param>
    ''' <param name="que"></param>
    ''' <remarks></remarks>
    Private Sub mcls_RecvMail_tEv_RecvMail(ByVal nEvtNo As Integer, ByVal que As COMMON_QUE_STRMIX) Handles mcls_RecvMail.tEv_RecvMail
        Dim nLineId As Integer = que.nLineNo			   ' �Ώۂ̔z��� (0:�S�� 1�`:�Ώ�ID�̂�)

        ' ���[�v�p
        Dim iiS As Integer
        Dim iiE As Integer
        If 0 = nLineId Then
            iiS = 0
            iiE = TBL_RPI_REC_MAX - 1
        Else
            iiS = nLineId - 1
            iiE = nLineId - 1
        End If


        ' �����ɑ΂��Ċ֌W -------------------------------------------------
        ' �I��
        If FACT_RPIV_END = nEvtNo Then
            Me.Close()
            Return
        End If

        ' �A���ݒ�
        If FACT_RPIS_SETTING = nEvtNo Then
            For ii As Integer = iiS To iiE
                m_cntItem(ii).OutParamExec(que)
            Next
            Return
        End If









        ' �`�F�[���֌W -------------------------------------------------
        If FACT_RPIV_DSP_MOVE = nEvtNo Then
            ' �ړ��̏ꍇ�A�����A�����̓A�N�e�B�u�����L���ɂȂ�Ȃ��΍�
            Dim quewk As COMMON_QUE_STRMIX = que
            quewk.nEventNo = FACT_RPIV_ACTIVE

            For ii As Integer = iiS To iiE
                m_cntItem(ii).Mail_Send_Chain(quewk)
                m_cntItem(ii).Mail_Send_Chain(que)
            Next


        ElseIf FACT_RPIV_DSPING = nEvtNo Then		' �N��
            For ii As Integer = iiS To iiE
                m_cntItem(ii).ExecRpiV()
                m_cntItem(ii).Mail_Send_Chain(que)
            Next

        ElseIf FACT_RPIS_DSP_POS = nEvtNo Then		' �\���ʒu
            For ii As Integer = iiS To iiE
                ' Window ���ނ�ύX
                Dim nSize As New Size
                Dim nPoint As New Point

                Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
                Dim MainDsp As Rectangle = monitor(que.idata(0) - 1).Bounds

                If 1 <> que.idata(5) Then
                    ' �w�軲�ނɻ��ޕύX
                    nPoint.X = MainDsp.Location.X + que.idata(1)	' �\���ʒuX
                    nPoint.Y = MainDsp.Location.Y + que.idata(2)	' �\���ʒuY
                    nSize.Width = que.idata(3)						' �\������X
                    nSize.Height = que.idata(4)						' �\������Y
                Else
                    ' �ő��������ނɻ��ޕύX
                    nPoint.X = MainDsp.Location.X					' �\���ʒuX
                    nPoint.Y = MainDsp.Location.Y					' �\���ʒuY
                    nSize.Width = MainDsp.Width						' �\������X
                    nSize.Height = MainDsp.Height					' �\������Y
                End If
                m_cntItem(ii).DspMove(nSize, nPoint)
            Next

        ElseIf FACT_RPIS_RENDO = nEvtNo Then		' �A���J�n�^��~
            If que.idata(0) Then
                ckbSeq.Checked = True
            Else
                ckbSeq.Checked = False
            End If

        ElseIf FACT_RPIS_OFFSET = nEvtNo Then		' �I�t�Z�b�g�l���f
            btnSetOffset.PerformClick()

        ElseIf FACT_RPIS_CLOSE = nEvtNo Then		' �q�o�h�r���[�A�N���[�Y
            For ii As Integer = iiS To iiE
                m_cntItem(ii).OutMail_RpiEnd()
            Next

        ElseIf FACT_RPIV_FIRST_DSPING = nEvtNo Then
            For ii As Integer = iiS To iiE
                m_cntItem(ii).ExecRpiV()
            Next
        Else

            ' �P���]��
            For ii As Integer = iiS To iiE
                m_cntItem(ii).Mail_Send_Chain(que)
            Next
        End If
    End Sub








    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A������

    ''' <summary>
    ''' ��������s
    ''' </summary>
    Private Sub timCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timCycle.Tick
        Dim tMem As New TYPE_TRK        ' ���L�������S�f�[�^
        tMem.initialize()
        Dim nId As Integer			' ���C�� (0�I���W��)

        '===============================================

        '''' ���L�������Q��
        tClass_GlovalMemory.GetMapped(m_memTrkPtr, 0, tMem.GetType(), tMem)
        nId = tMem.head.nActiveID


        '===============================================
        ' �A������
        If ckbSeq.Checked Then
            '''' ����̎�̕␳�O��������c��
            Dim nBaseY As Integer = (tMem.rec(nId).nY - m_cntItem(nId).Prop_Offset(1)) / m_cntItem(nId).Prop_Rate()
            Dim nBaseX As Integer = (tMem.rec(nId).nX - m_cntItem(nId).Prop_Offset(0))


            '''' ��̋����� �]�ɔ��f
            For ii As Integer = 0 To m_cntItem.Length - 1
                If nId = ii Then Continue For

                tMem.rec(ii).nY = CInt(nBaseY * m_cntItem(ii).Prop_Rate() + m_cntItem(ii).Prop_Offset(1))
                tMem.rec(ii).nX = CInt(nBaseX + m_cntItem(ii).Prop_Offset(0))
            Next
        End If

        '===============================================
        ' ���ʕ\��

        '''' ����Z�b�g
        For ii As Integer = 0 To m_cntItem.Length - 1
            ' ���݂̋���
            m_cntItem(ii).SetNowData(tMem.rec(ii))
            ' �A�N�e�B�u�R���g���[��
            If nId = ii Then
                m_cntItem(ii).SetActive(True)
            Else
                m_cntItem(ii).SetActive(False)
            End If
        Next

        '''' ���ʏ���
        tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMem)
    End Sub


    ''' <summary>
    ''' �I�t�Z�b�g����
    ''' </summary>
    Private Sub btnSetOffset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetOffset.Click
        Dim tMem As TYPE_TRK = Nothing		  ' ���L�������S�f�[�^
        tMem.initialize()

        '''' ���L�������Q��
        tClass_GlovalMemory.GetMapped(m_memTrkPtr, 0, tMem.GetType(), tMem)


        '===============================================
        ' 1�{� 

        '''' �S��1�{��ōl���邱�ƁI
        Dim nBaseY(TBL_RPI_REC_MAX - 1) As Integer	  ' 1�{��̒���[m]
        Dim nBaseX(TBL_RPI_REC_MAX - 1) As Integer	  ' 1�{��̒���[m]
        For ii As Integer = 0 To TBL_RPI_REC_MAX - 1
            nBaseY(ii) = tMem.rec(ii).nY / m_cntItem(ii).Prop_Rate()
            nBaseX(ii) = tMem.rec(ii).nX
        Next

        '''' �ŏ��ʒu�̃��C����������
        Dim nMinY As Integer = nBaseY(0)	' 1�{��̍ŏ��덷
        Dim nMinX As Integer = nBaseX(0)
        For ii As Integer = 0 To m_cntItem.Length - 1
            If Not m_cntItem(ii).CheckRpiRun() Then Continue For

            nMinY = Math.Min(nMinY, nBaseY(ii))
            nMinX = Math.Min(nMinX, nBaseX(ii))
        Next


        '===============================================
        ' ���ɖ߂� 

        '''' �ŏ�����̃Y����
        For ii As Integer = 0 To m_cntItem.Length - 1
            m_cntItem(ii).Prop_Offset(1) = tMem.rec(ii).nY - (nMinY * m_cntItem(ii).Prop_Rate())
            m_cntItem(ii).Prop_Offset(0) = tMem.rec(ii).nX - nMinX
        Next
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �A�����
    ''' </summary>
    Private Sub ckbSeq_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ckbSeq.CheckedChanged
        '''' ��ԕύX
        If ckbSeq.Checked Then
            ckbSeq.Text = "�A����"
            ckbSeq.BackColor = Color.Orange
        Else
            ckbSeq.Text = "�A������"
            ckbSeq.BackColor = SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' RPIV�̋N���ς�Index�̃��X�g (0�I���W��)
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ExecRpiVList() As List(Of Integer)
        Dim l As New List(Of Integer)

        For ii As Integer = 0 To m_cntItem.Length - 1
            If m_cntItem(ii).CheckRpiRun() Then l.Add(ii)
        Next
        Return l
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j�^�֌W

    ''' <summary>
    ''' ���j�^�擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspSelectInit()
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens()
        cmbDspSelect.Items.Clear()
        For ii As Integer = 0 To monitor.Length - 1
            cmbDspSelect.Items.Add(ii + 1)
        Next
        cmbDspSelect.SelectedIndex = 0


        cmb11_Rpi.Items.Clear()
        For ii As Integer = 0 To m_cntItem.Length - 1
            cmb11_Rpi.Items.Add(ii)
        Next
        cmb11_Rpi.SelectedIndex = 0
    End Sub



    ''' <summary>
    ''' �Q�~�Q �� RpiV�𐮗񂳂���
    ''' </summary>
    Private Sub tlp22_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp22.DoubleClick, lbl22_1.DoubleClick, lbl22_2.DoubleClick, lbl22_3.DoubleClick, lbl22_4.DoubleClick
        Dim pIndex As List(Of Integer) = ExecRpiVList()		' RPIV�̋N���ς�Index�̃��X�g (0�I���W��)

        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size
        s.Width = MainDsp.Size.Width / 2
        s.Height = MainDsp.Size.Height / 2
        Dim p As Point = MainDsp.Location


        '' ����
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To 2 - 1
            p.Y = MainDsp.Location.Y + (s.Height * ii)
            For jj As Integer = 0 To 2 - 1
                If nCnt >= pIndex.Count Then Exit For

                p.X = MainDsp.Location.X + (s.Width * jj)

                Call m_cntItem(pIndex(nCnt)).DspMove(s, p)

                nCnt += 1
            Next
        Next
    End Sub

    ''' <summary>
    ''' �P�~X �� RpiV�𐮗񂳂���
    ''' </summary>
    Private Sub tlp13_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp13.DoubleClick, lbl13_1.DoubleClick, lbl13_2.DoubleClick, lbl13_3.DoubleClick
        Dim pIndex As List(Of Integer) = ExecRpiVList()		' RPIV�̋N���ς�Index�̃��X�g (0�I���W��)

        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size
        s.Width = MainDsp.Size.Width / pIndex.Count
        s.Height = MainDsp.Size.Height
        Dim p As Point = MainDsp.Location


        '' ����
        Dim nCnt As Integer = 0
        For ii As Integer = 0 To pIndex.Count - 1
            If nCnt >= pIndex.Count Then Exit For

            p.X = MainDsp.Location.X + (s.Width * ii)
            Call m_cntItem(pIndex(nCnt)).DspMove(s, p)
            nCnt += 1
        Next
    End Sub

    ''' <summary>
    ''' �P�~1 �� RpiV�𐮗񂳂���
    ''' </summary>
    Private Sub tlp11_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlp11.DoubleClick
        Dim nDspId As Integer = cmbDspSelect.SelectedIndex
        Dim monitor() As System.Windows.Forms.Screen = System.Windows.Forms.Screen.AllScreens
        Dim MainDsp As Rectangle = monitor(nDspId).Bounds

        Dim s As Size = MainDsp.Size
        Dim p As Point = MainDsp.Location


        Dim nIndex As Integer = cmb11_Rpi.SelectedIndex
        If Not m_cntItem(nIndex).CheckRpiRun() Then Return
        Call m_cntItem(nIndex).DspMove(s, p)
    End Sub
End Class
