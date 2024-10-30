Imports tClassLibrary

Public Class frmMain
    ' �萔
    Private Const nWriteFileNum As Integer = 5          ' Insert����̧�ق̐��i1�`�j

    ' �A�C�e��
    Private m_objFilePath As TextBox()                  ' �o�C�i���f�[�^�ւ̃p�X
    Private m_objFileLabel As Label()                   ' �o�C�i���f�[�^�̃��x��
    Private m_objFileBtn As Button()                    ' �t�@�C���I���{�^��

    ''' <summary>
    ''' ���[�h
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' �o�C�i���f�[�^����0�ȉ��Ȃ�I��
        If 1 > nWriteFileNum Then
            MsgBox("Error!!" & Environment.NewLine & " nWriteFileNum��0�ȉ�!!", MsgBoxStyle.Critical)
            End
        End If

        Const nYohaku As Integer = 5

        m_objFilePath = New TextBox(nWriteFileNum - 1) {}                   ' �o�C�i���f�[�^�ւ̃p�X
        m_objFileLabel = New Label(nWriteFileNum - 1) {}                    ' �o�C�i���f�[�^�̃��x��
        m_objFileBtn = New Button(nWriteFileNum - 1) {}                     ' �t�@�C���I���{�^��

        ' �o�C�i���f�[�^�I��p�A�C�e���̒ǉ�
        Dim ii As Integer
        For ii = 0 To nWriteFileNum - 1

            ' //////////////
            '' ���x�� �i@F1,@F2,�E�E�E�j
            m_objFileLabel(ii) = New Label
            BinDataGroup.Controls.Add(m_objFileLabel(ii))
            With m_objFileLabel(ii)
                .Text = "@F" & (ii + 1)
                .Top = CInt((.Height + nYohaku) * ii + BinDataGroup.Font.Size + nYohaku)
                .Left = nYohaku
                .Width = CInt(.Text.Length * .Font.Size + nYohaku)
            End With

            ' //////////////
            '' [�c]�{�^�� 
            m_objFileBtn(ii) = New Button
            BinDataGroup.Controls.Add(m_objFileBtn(ii))
            With m_objFileBtn(ii)
                .Text = "�c"
                .Top = m_objFileLabel(ii).Top
                .Left = m_objFileLabel(ii).Right + nYohaku
                .Width = CInt(.Text.Length * .Font.Size + nYohaku * 2)
                AddHandler .Click, AddressOf m_objFileBtn_Click                     ' �{�^���������̃C�x���g
            End With

            ' //////////////
            '' �e�L�X�g�{�b�N�X
            m_objFilePath(ii) = New TextBox
            BinDataGroup.Controls.Add(m_objFilePath(ii))
            With m_objFilePath(ii)
                .Text = ""
                .Top = m_objFileLabel(ii).Top
                .Left = m_objFileBtn(ii).Right + nYohaku
                .Width = BinDataGroup.Width - .Left - nYohaku
                .AllowDrop = True                                                   ' �h���b�O&�h���b�v��L���ɂ��邽�߂̎��ʎq
                AddHandler .DragEnter, AddressOf m_objFilePath_DragEnter            ' �t�@�C���h���b�O���̃C�x���g
                AddHandler .DragDrop, AddressOf m_objFilePath_DragDrop              ' �t�@�C���h���b�v���̃C�x���g
            End With
        Next

        ' �ǉ��������������̃A�C�e�������炷
        Dim nWk As Integer
        nWk = BinDataGroup.Height
        BinDataGroup.Height = m_objFileLabel(nWriteFileNum - 1).Bottom + nYohaku    ' �o�C�i���f�[�^�Ǎ�
        nWk = BinDataGroup.Height - nWk
        SqlGroup.Top += nWk                                                         ' SQL����
        Me.Height += nWk                                                            ' �_�C�A���O

        ' ���΃p�X�̃J�����g�f�B���N�g�������s�t�@�C���̃f�B���N�g���ɏ�����
        System.Environment.CurrentDirectory = _
            System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �o�C�i���t�@�C�� �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �o�C�i���t�@�C������
    ''' </summary>
    Private Sub m_objFileBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim idx As Integer = GetIndex(m_objFileBtn, sender)
        ''
        ' �t�@�C���I��
        Dim dlg As New OpenFileDialog
        dlg.Title = "�t�@�C���I��"
        dlg.Multiselect = False
        dlg.ShowHelp = False
        dlg.ShowReadOnly = False
        dlg.InitialDirectory = ""
        dlg.FileName = ""
        dlg.ShowDialog()

        If "" <> dlg.FileName Then
            ' �o�C�i���t�@�C���ւ̃p�X�ύX
            m_objFilePath(idx).Text = dlg.FileName
        End If
    End Sub

    ''' <summary>
    ''' �e�L�X�g�{�b�N�X�Ƀt�@�C���h���b�O
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_objFilePath_DragEnter(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs)
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            ' �t�@�C�����h���b�O
            e.Effect = DragDropEffects.Copy
        Else
            ' ����ȊO���h���b�O
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' �e�L�X�g�{�b�N�X�Ƀt�@�C���h���b�v
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_objFilePath_DragDrop(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs)
        Dim idx As Integer = GetIndex(m_objFilePath, sender)      ' �I�����ꂽ�e�L�X�g�{�b�N�X�̃C���f�b�N�X
        ' idx��-1�̎���Return
        If -1 = idx Then Return
        ''
        ' �t�@�C���܂ł̃t���p�X
        Dim strFName() As String = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        ''
        ' �o�C�i���t�@�C���ւ̃p�X�ύX
        m_objFilePath(idx).Text = strFName(0)
    End Sub

    ''' <summary>
    ''' �o�C�i���t�@�C�������[�J���ϐ��Ɋi�[
    ''' </summary>
    ''' <param name="strFileName">�t�@�C���ւ̃t���p�X</param>
    ''' <param name="bData">�o�C�i���f�[�^</param>
    ''' <remarks></remarks>
    Private Function SetBinData(ByVal strFileName As String, ByRef bData As Byte()) As Boolean
        If System.IO.File.Exists(strFileName) Then
            ' �t�@�C���Ǎ�
            Using os As New IO.FileStream(strFileName, IO.FileMode.Open, IO.FileAccess.Read)
                Dim bWk(CInt(os.Length) - 1) As Byte
                os.Read(bWk, 0, CInt(os.Length))
                bData = bWk
            End Using
            Return True
        Else
            Return False
        End If
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�[�^�x�[�X �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnDbWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDbWrite.Click
        ' �m�F
        If MsgBox("DB�ɏ����݂܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "DB�����m�F") = MsgBoxResult.No Then Return

        '' �������`�F�b�N
        If 0 > CheckIntegrity() Then
            Return
        End If

        '' ���[�v�@�\�̐������`�F�b�N
        Dim nRet As Integer = ChkLoop()
        If 0 > nRet Then
            Return
        End If

        ' �o�C�i���f�[�^���X�g�쐬
        Dim para As New List(Of SqlClient.SqlParameter)              ' �o�C�i���f�[�^
        If False = PutSqlParameter(para) Then
            Return
        End If


        ' //////////////
        ' DB�ڑ���SQL�����s
        If 1 = nRet Then
            'If 0 <> ChkSql() Then
            '    lblStatus.Text = "���[�v�@�\���g�p����Ƃ���INSERT�Ŏ��s���Ă�������"
            '    Return
            'End If
            Dim nStart As Integer = CInt(txtLoopStart.Text)
            Dim nEnd As Integer = CInt(txtLoopEnd.Text)
            ' SQL���[�v���s
            If 0 > nExecSql(para, nStart, nEnd) Then
                Return
            End If
        ElseIf 0 = nRet Then
            ' SQL���s
            If 0 > ExecSql(para) Then
                Return
            End If
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ SQL�� �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INSERT��^���o��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>INSERT INTO $t VALUES( *,*,* );</remarks>
    Private Sub btnInsert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnInsert.Click
        ' �㏑���m�F
        If "" <> txtDbWriteSql.Text Then
            If MsgBox("SQL����INSERT��^���ɏ㏑�����Ă���낵���ł����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "�㏑���m�F") = MsgBoxResult.No Then Return
        End If

        ' SQL��
        Dim strWk As String = ""
        strWk &= "INSERT INTO " & txtTableName.Text & Environment.NewLine
        strWk &= "VALUES( "
        Dim ii As Integer
        For ii = 0 To m_objFilePath.Length - 1
            If System.IO.File.Exists(m_objFilePath(ii).Text) Then
                strWk &= m_objFileLabel(ii).Text & ", "
            End If
        Next
        strWk = strWk.TrimEnd(" "c, ","c)
        strWk &= " ); " & Environment.NewLine

        txtDbWriteSql.Text = strWk

    End Sub

    ''' <summary>
    ''' UPDATE��^���o��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>UPDATE $t SET [**]=***  WHERE *****</remarks>
    Private Sub btnUpdate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpdate.Click
        ' �㏑���m�F
        If "" <> txtDbWriteSql.Text Then
            If MsgBox("SQL����UPDATE��^���ɏ㏑�����Ă���낵���ł����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "�㏑���m�F") = MsgBoxResult.No Then Return
        End If

        ' SQL��
        Dim strWk As String = ""
        strWk &= "UPDATE " & txtTableName.Text & Environment.NewLine
        strWk &= "SET "
        Dim ii As Integer
        For ii = 0 To m_objFilePath.Length - 1
            If System.IO.File.Exists(m_objFilePath(ii).Text) Then
                strWk &= Environment.NewLine
                strWk &= "[] = " & m_objFileLabel(ii).Text & ","

            End If
        Next
        strWk = strWk.TrimEnd(","c)
        strWk &= Environment.NewLine
        strWk &= "WHERE ;"

        txtDbWriteSql.Text = String.Copy(strWk)
    End Sub

    ''' <summary>
    ''' SQL�����̃R�����g����菜��
    ''' </summary>
    ''' <param name="strsql">�g�����Ώ�SQL��</param>
    ''' <returns>�R�����g������菜����SQL��</returns>
    ''' <remarks>���ׂĂ�"--"������s�܂� or "/*"����"*/"�܂ł̕�������폜����</remarks>
    Private Function TrimComments(ByVal strsql As String) As String
        Dim strWk As String = strsql
        Dim stcWk As Stack(Of Integer) = New Stack(Of Integer)
        Dim nStart As Integer = 0
        Dim nEnd As Integer = 0
        Dim com1 As String = "(/\*.*\*/)|(--)"        ' �R�����g���̗L�������m���邽�߂̐��K�\��
        Dim com2 As String = "(/\*)|(\*/)|(--)"       ' �R�����g�����J�n�A�I�������m���邽�߂̐��K�\��

        ' SQL������"/*","*/","--"��S�擾
        Dim m As System.Text.RegularExpressions.Match = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
        ' SQL�����ɃR�����g���������Ȃ�܂Ń��[�v
        While System.Text.RegularExpressions.Regex.IsMatch(strWk, com1, System.Text.RegularExpressions.RegexOptions.Compiled Or System.Text.RegularExpressions.RegexOptions.Singleline)
            If "/*" = m.Value Then                  ' /*�̏ꍇ
                stcWk.Push(m.Index)
                ' ���̃R�����g
                m = m.NextMatch
            ElseIf "*/" = m.Value Then              ' */�̏ꍇ
                If 1 = stcWk.ToArray.Length Then    ' �X�^�b�N���̃A�C�e�������
                    ' �J�n�A�I���ʒu�ݒ�
                    nStart = stcWk.Pop()
                    nEnd = m.Index - nStart + 2
                    strWk = strWk.Remove(nStart, nEnd)
                    ' ���̃R�����g�A�Č���
                    m = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
                ElseIf 1 < stcWk.ToArray.Length Then ' �X�^�b�N����2�ȏ�̃A�C�e��
                    stcWk.Pop()
                    ' ���̃R�����g
                    m = m.NextMatch
                Else
                    ' ���̃R�����g
                    m = m.NextMatch
                End If
            ElseIf "" = m.Value Then    ' �i�v���[�v���pIF
                nStart = strWk.IndexOf("/*")
                strWk = strWk.Remove(nStart)
                Exit While
            Else                                    ' --�̏ꍇ
                If 0 = stcWk.ToArray.Length Then    ' �X�^�b�N���ɃA�C�e������
                    nStart = m.Index
                    nEnd = strWk.Substring(m.Index).IndexOf(Environment.NewLine)
                    If 0 >= nEnd Then
                        strWk = strWk.Remove(nStart)
                    Else
                        strWk = strWk.Remove(nStart, nEnd)
                    End If

                    ' ���̃R�����g�A�Č���
                    m = System.Text.RegularExpressions.Regex.Match(strWk, com2, System.Text.RegularExpressions.RegexOptions.Compiled)
                Else
                    ' ���̃R�����g
                    m = m.NextMatch
                End If
            End If
        End While
        Return strWk
    End Function


    ''' <summary>
    ''' SQL���`�F�b�N
    ''' </summary>
    ''' <returns>�`�F�b�N����(0:INSERT,1:UPDATE,-1:����ȊO)</returns>
    ''' <remarks>���s�A�󔒈ȊO�̍ŏ��̕�����Ŕ��f</remarks>
    Private Function ChkSql() As Integer
        Dim strWk As String = TrimComments(txtDbWriteSql.Text)
        If strWk.Replace(Environment.NewLine, " ").Trim.StartsWith("INSERT ", StringComparison.CurrentCultureIgnoreCase) Then
            Return 0
        ElseIf strWk.Replace(Environment.NewLine, " ").Trim.StartsWith("UPDATE ", StringComparison.CurrentCultureIgnoreCase) Then
            Return 1
        Else
            Return -1
        End If
    End Function

    ''' <summary>
    ''' ���[�v�@�\�̐������`�F�b�N
    ''' </summary>
    ''' <returns>�`�F�b�N����(0:�񃋁[�v��,1:���[�v��,-1:�J�n��,-2:�I����,-3:�J�n�����ȊO,-4:�I�������ȊO,-5:�J�n>�I��)</returns>
    ''' <remarks></remarks>
    Private Function ChkLoop() As Integer
        Dim strWk As String = TrimComments(txtDbWriteSql.Text)

        If Not strWk.Contains("$n") Then
            ' SQL������$n�̕������Ȃ�
            Return 0
        Else
            ' ////////////////////////////////
            ' �J�n�A�I���ɕ��������͂���Ă��Ȃ�
            If "" = txtLoopStart.Text Then
                lblStatus.Text = "���[�v�J�n�l���ݒ肳��Ă��܂���"
                Return -1
            End If
            If "" = txtLoopEnd.Text Then
                lblStatus.Text = "���[�v�I���l���ݒ肳��Ă��܂���"
                Return -2
            End If

            ' ////////////////////////////////
            ' �J�n�A�I���ɐ����ȊO�̕��������͂���Ă���
            If System.Text.RegularExpressions.Regex.IsMatch(txtLoopStart.Text, "[^0-9]+") Then
                lblStatus.Text = "���[�v�J�n�l�ɐ����ȊO�̕��������͂���Ă��܂�"
                Return -3
            End If
            If System.Text.RegularExpressions.Regex.IsMatch(txtLoopEnd.Text, "[^0-9]+") Then
                lblStatus.Text = "���[�v�I���l�ɐ����ȊO�̕��������͂���Ă��܂�"
                Return -4
            End If

            ' ////////////////////////////////
            ' ���[�v�J�n�l�������[�v�I���l�̕���������
            If CInt(txtLoopStart.Text) > CInt(txtLoopEnd.Text) Then
                lblStatus.Text = "���[�v�J�n�l > ���[�v�I���l�ɂȂ��Ă܂�"
                Return -5
            End If
            Return 1
        End If

    End Function
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �C���f�b�N�X�擾
    ''' </summary>
    ''' <param name="objBox">�I�u�W�F�N�g�z��</param>
    ''' <param name="sender">�I���I�u�W�F�N�g</param>
    ''' <returns>�C���f�b�N�X�ԍ�(�ُ펞:-1)</returns>
    ''' <remarks>�o�C�i���f�[�^�Ǎ��O���[�v��p</remarks>
    Private Function GetIndex(ByVal objBox() As Object, ByVal sender As System.Object) As Integer
        Dim index As Integer = -1           ' �C���f�b�N�X
        Dim ii As Integer                   ' ���[�v�ϐ�
        For ii = 0 To objBox.Length - 1
            If objBox(ii).Equals(sender) Then
                index = ii
                Exit For
            End If
        Next
        Return index
    End Function

    ''' <summary>
    ''' �������`�F�b�N
    ''' </summary>
    ''' <returns>�`�F�b�N����(0:����,-1:�A�h���X������,-2:DB��������,-3:���[�U�[������,-4:�p�X������,-5:UPDATE,INSERT�ȊO)</returns>
    ''' <remarks></remarks>
    Private Function CheckIntegrity() As Integer
        ' �ϐ���`
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ���ڽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' �f�[�^�x�[�X��
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ���[�U�[
        Dim strPass As String = txtDbWritePass.Text.Trim                ' �p�X

        ' ////////////////////////////////
        '  �������`�F�b�N
        '' //////////////////
        '' DB���֘A�`�F�b�N
        '' ���ڽ
        If "" = strAddless Then
            lblStatus.Text = "address�������͂ł�"
            Return -1
        End If
        '' �f�[�^�x�[�X��
        If "Database_" = strDbName Or "" = strDbName Then
            lblStatus.Text = "DB���������͂ł�"
            Return -2
        End If
        '' ���[�U�[
        If "" = strUser Then
            lblStatus.Text = "User���������͂ł�"
            Return -3
        End If
        '' �p�X
        If "" = strPass Then
            lblStatus.Text = "Pass�������͂ł�"
            Return -4
        End If

        '' //////////////////
        '' SQL���֘A�`�F�b�N
        '' INSERT��UPDATE�Ŏn�܂��Ă��Ȃ����͎��s���Ȃ�
        If 0 > ChkSql() Then
            lblStatus.Text = "UPDATE�AINSERT���ȊO��SQL�������s���悤�Ƃ��Ă܂�"
            Return -5
        End If

        Return 0
    End Function

    ''' <summary>
    ''' �o�C�i���f�[�^���X�g�쐬
    ''' </summary>
    ''' <param name="para">�p�����[�^</param>
    ''' <returns>true:�쐬����,false:�쐬���s</returns>
    ''' <remarks></remarks>
    Private Function PutSqlParameter(ByRef para As List(Of SqlClient.SqlParameter)) As Boolean
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL��
        Dim bWk() As Byte = New Byte() {}
        Dim ii As Integer

        Dim nChk As Integer = ChkLoop()
        For ii = 0 To nWriteFileNum - 1
            If strSql.Replace(Environment.NewLine, " ").Trim.Contains(m_objFileLabel(ii).Text) Then
                bWk = New Byte() {}
                If False = SetBinData(m_objFilePath(ii).Text, bWk) Then
                    lblStatus.Text = m_objFileLabel(ii).Text & "�̃p�X�̐�Ƀt�@�C�������݂��܂���"
                    Return False
                End If
                '' �X�J���ϐ�����@F1,@F2,@F3,�c
                para.Add(New SqlClient.SqlParameter(m_objFileLabel(ii).Text, bWk))
            ElseIf nChk = 1 And strSql.Replace(Environment.NewLine, " ").Trim.Contains("@F$n") Then
                If CInt(txtLoopStart.Text) <= ii + 1 And CInt(txtLoopEnd.Text) >= ii + 1 Then
                    bWk = New Byte() {}
                    If False = SetBinData(m_objFilePath(ii).Text, bWk) Then
                        lblStatus.Text = m_objFileLabel(ii).Text & "�̃p�X�̐�Ƀt�@�C�������݂��܂���"
                        Return False
                    End If
                    '' �X�J���ϐ�����@F1,@F2,@F3,�c
                    para.Add(New SqlClient.SqlParameter(m_objFileLabel(ii).Text, bWk))
                End If
            End If
        Next
        Return True
    End Function

    ''' <summary>
    ''' �p�����[�^�tSQL�����s
    ''' </summary>
    ''' <param name="para">�p�����[�^</param>
    ''' <returns>���s����(0:����,-1:SQL���s���s,-2:�e���͈�0�s,-3:�������f,-9:DB�ڑ����s)</returns>
    ''' <remarks></remarks>
    Private Function ExecSql(ByVal para As List(Of SqlClient.SqlParameter)) As Integer
        ' �ϐ���`
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ���ڽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' �f�[�^�x�[�X��
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ���[�U�[
        Dim strPass As String = txtDbWritePass.Text.Trim                ' �p�X
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL��

        '' DB�N���X
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        Try
            '' DB�ڑ�
            If Not tcls_db.Open() Then
                lblStatus.Text = "DB�ڑ��Ɏ��s���܂���"
                Return -9
            End If

            Dim ii As Integer
            ' SQL���i�p�����[�^�t���j���s
            Dim nResult As Integer
            Dim strWk As String = strSql.Replace("$n", CStr(ii))

            '' //////////////////
            '' ���s��̉e���͈͎擾
            ' �g�����U�N�V�����J�n
            tcls_db.TransactStart()
            ' SQL�����s
            nResult = tcls_db.DMLExecute(strWk, para.ToArray)
            ' ���[���o�b�N
            tcls_db.TransactRollback()
            ' �e���͈̓`�F�b�N
            If -1 = nResult Then
                lblStatus.Text = "SQL�����s���s"
                Return -1
            ElseIf 0 = nResult Then
                lblStatus.Text = "�e���͈͂�0�s�ł���"
                Return -2
            ElseIf 1 < nResult Then
                ' �e���͈͂�2�s�ȏ�̂Ƃ��̓R�~�b�g���邩�m�F
                If MsgBox(nResult & "�s�ɉe�����o��\��������܂��B��낵���ł��傤���H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "�������f�m�F") = MsgBoxResult.No Then
                    lblStatus.Text = "�����𒆎~���܂���"
                    Return -3
                End If
            End If

            ' SQL�����s
            nResult = tcls_db.DMLExecute(strWk, para.ToArray)
            ' �e���͈̓`�F�b�N
            If -1 = nResult Then
                lblStatus.Text = "SQL�����s���s"
                Return -1
            ElseIf 0 = nResult Then
                lblStatus.Text = "�e���͈͂�0�s�ł���"
                Return -2
            End If
            lblStatus.Text = "SQL�����s�����I" & nResult & "�s��������܂���"
            Return 0
        Catch ex As Exception
            lblStatus.Text = ex.Message
        Finally
            tcls_db.Close()
            tcls_db.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' �p�����[�^�tSQL�����[�v���s
    ''' </summary>
    ''' <param name="para">�p�����[�^</param>
    ''' <returns>���s����(0:�S�Đ���,-1:�ُ킠��,-9:DB�ڑ�)</returns>
    ''' <remarks></remarks>
    Private Function nExecSql(ByVal para As List(Of SqlClient.SqlParameter), ByVal nStart As Integer, ByVal nEnd As Integer) As Integer
        ' �ϐ���`
        Dim strAddless As String = txtDBWriteAddless.Text.Trim          ' ���ڽ
        Dim strDbName As String = txtDbName.Text.Trim                   ' �f�[�^�x�[�X��
        Dim strUser As String = txtDbWriteUser.Text.Trim                ' ���[�U�[
        Dim strPass As String = txtDbWritePass.Text.Trim                ' �p�X
        Dim strSql As String = txtDbWriteSql.Text                       ' SQL��

        '' DB�N���X
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        Try
            '' DB�ڑ�
            If Not tcls_db.Open() Then
                lblStatus.Text = "DB�ڑ��Ɏ��s���܂���"
                Return -1
            End If

            Dim ii As Integer
            Dim nSuccess As Integer = 0
            Dim nFailure As Integer = 0
            Dim nChgNothing As Integer = 0
            Dim nBreak As Integer = 0
            For ii = nStart To nEnd
                ' SQL���i�p�����[�^�t���j���s
                Dim nResult As Integer
                Dim strWk As String = strSql.Replace("$n", CStr(ii))

                '' //////////////////
                '' ���s��̉e���͈͎擾
                ' �g�����U�N�V�����J�n
                tcls_db.TransactStart()
                ' SQL�����s
                nResult = tcls_db.DMLExecute(strWk, para.ToArray)
                ' ���[���o�b�N
                tcls_db.TransactRollback()
                ' �e���͈̓`�F�b�N
                If -1 = nResult Then
                    nfailure += 1
                    Continue For
                ElseIf 0 = nResult Then
                    nChgNothing += 1
                    Continue For
                ElseIf 1 < nResult Then
                    ' �e���͈͂�2�s�ȏ�̂Ƃ��̓R�~�b�g���邩�m�F
                    If MsgBox(nResult & "�s�ɉe�����o��\��������܂��B��낵���ł��傤���H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question, "�������f�m�F") = MsgBoxResult.No Then
                        nBreak += 1
                        Continue For
                    End If
                End If

                ' SQL�����s
                nResult = tcls_db.DMLExecute(strWk, para.ToArray)
                ' �e���͈̓`�F�b�N
                If -1 = nResult Then
                    nfailure += 1
                    Continue For
                ElseIf 0 = nResult Then
                    nChgNothing += 1
                    Continue For
                End If
                nSuccess += nResult
                Continue For
            Next
            lblStatus.Text = "�SSQL�����s�I���I�i����:" & nSuccess & "�s,���s�F" & nfailure & "�s,�ύX�Ȃ��F" & nChgNothing & "�s,���f�F" & nBreak & "��j"
            If nfailure > 0 Or nChgNothing > 0 Or nBreak > 0 Then Return -1
            Return 0
        Catch ex As Exception
            lblStatus.Text = ex.Message
        Finally
            tcls_db.Close()
            tcls_db.Dispose()
        End Try
    End Function
End Class
