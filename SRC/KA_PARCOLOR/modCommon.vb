Public Module modCommon
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���ID��`
    Public Const GAMEN_ID_EPC_COMMON As String = "EPC_COMMON"                   '�Œ�p�����[�^�ݒ�
    Public Const GAMEN_ID_EPC_PARA As String = "EPC_PARA"                       '���W�b�N�p�������ݒ�
    Public Const GAMEN_ID_EPC_CAMERA As String = "EPC_CAMERA"                   '�J�����ݒ�
    Public Const GAMEN_ID_EPC_MIXCOLOR As String = "EPC_MIXCOLOR"               '���F�����ݒ�
    Public Const GAMEN_ID_EPC_EDGE As String = "EPC_EDGE"                       '�G�b�W���o�ݒ�
    Public Const GAMEN_ID_EPC_SHADING As String = "EPC_SHADING"                 '�V�F�[�f�B���O�␳�ݒ�
    Public Const GAMEN_ID_EPC_THRESHOLD As String = "EPC_THRESHOLD"             '臒l�ݒ�
    Public Const GAMEN_ID_EPC_DEFECT As String = "EPC_DEFECT"                   '�r���o�ݒ�
    Public Const GAMEN_ID_EPC_MERGE As String = "EPC_MERGE"                     '�r���������ݒ�
    ' <20221117> <����> <�ŏI�H���������u���@�\������1> --->>>
    Public Const GAMEN_ID_EPC_TAJUUTHRESHOLD As String = "EPC_TAJUUTHRESHOLD"   '���d����\臒l�ݒ�
    ' <20221117> <����> <�ŏI�H���������u���@�\������1> ---<<<

    'Public Const SELECT_KEY_PCOMMON As String = "SELECT [Key],{0} FROM {1} where [Key]={2}"
    'Public Const SELECT_KEY_PCAMSET As String = "SELECT {0} FROM {1} ORDER BY camset ASC"
    'Public Const SELECT_KEY_PCAMSET_ANGLE As String = "SELECT {0} FROM {1} ORDER BY camangle, camset ASC"
    'Public Const SELECT_KEY_PSCODE As String = "SELECT {0} FROM {1} ORDER BY scode ASC"
    'Public Const SELECT_KEY_PCAMSET_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camset ASC"
    'Public Const SELECT_KEY_PCAMANGLE_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camangle ASC"
    'Public Const SELECT_KEY_PCAMSET_ANGLE_SCODE As String = "SELECT {0} FROM {1} ORDER BY scode, camangle, camset ASC"

    'Public Const UPDATE_KEY_PSCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMSET_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camset = b.camset AND a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMANGLE_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle AND a.scode IN({5})"
    'Public Const UPDATE_KEY_PCAMSET_ANGLE_SCODE As String = "UPDATE {0} SET {1} FROM {2} a, (SELECT * FROM {3} WHERE scode= {4}) b WHERE a.camangle = b.camangle And a.camset = b.camset AND a.scode IN({5})"

    'Public Const SELECT_KEY As String = "SELECT [Key],{0} FROM {1}"
    Public Const SELECT_KEY_ORDER As String = "SELECT {0} FROM {1} ORDER BY {2} ASC"

    '---------------------------------------------------------------------------
    '// ������������ް���د�ޕ\���ݒ�p��`
    '---------------------------------------------------------------------------
    '// GRID_FIRST_DISP_GID                 '// �����\�����ID
    '// GRID_FIRST_DISP_NAME                '// �����\�����ID�\������
    '---------------------------------------------------------------------------
    '// ��ʕʒ�`
    '// XXXXXXXXXX_GRID_GID                 '// ���ID��DB�o�^�l 
    '//                                         �V�X�e����ɉ�ʂ����݂��Ȃ��ꍇ�͒l���Z�b�g���Ȃ��ƕ\���ΏۊO�ƂȂ�
    '// XXXXXXXXXX_GRID_NAME                '// ��ʕ\����̉��ID
    '// XXXXXXXXXX_GRID_CNT                 '// ��ʂɕR�t���O���b�h��
    '// XXXXXXXXXX_GRID_ID                  '// �O���b�hID��DB�o�^�l(�O���b�h�������J���}��؂�Œ�`)
    '// XXXXXXXXXX_GRID                     '// ��ʕ\����̃O���b�hID(�O���b�h�������J���}��؂�Œ�`)
    '---------------------------------------------------------------------------
    Public Const GRID_FIRST_DISP_GID As String = EPC_COMMON_GRID_GID                            '// �����\�����ID
    Public Const GRID_FIRST_DISP_NAME As String = EPC_COMMON_GRID_NAME                          '// �����\�����ID�\������
    '�Œ�p�����[�^�ݒ���
    Public Const EPC_COMMON_GRID_GID As String = GAMEN_ID_EPC_COMMON                            '// ���ID��DB�o�^�l
    Public Const EPC_COMMON_GRID_NAME As String = "�Œ�p�����[�^�ݒ�"                          '// ��ʕ\����̉��ID
    Public Const EPC_COMMON_GRID_CNT As Integer = 2                                             '// ��ʂɕR�t���O���b�h��
    Public Const EPC_COMMON_GRID_ID As String = "1,2"                                           '// �O���b�hID��DB�o�^�l
    Public Const EPC_COMMON_GRID As String = "����,�J�����Z�b�g��"                              '// ��ʕ\����̃O���b�hID
    '���W�b�N�p�������ݒ���
    Public Const EPC_PARA_GRID_GID As String = GAMEN_ID_EPC_PARA                                '// ���ID��DB�o�^�l
    Public Const EPC_PARA_GRID_NAME As String = "���W�b�N�p�������ݒ�"                          '// ��ʕ\����̉��ID
    Public Const EPC_PARA_GRID_CNT As Integer = 3                                               '// ��ʂɕR�t���O���b�h��
    Public Const EPC_PARA_GRID_ID As String = "1,2,3"                                           '// �O���b�hID��DB�o�^�l
    Public Const EPC_PARA_GRID As String = "����,�J�����Z�b�g��,�����\�ʏ�ԕ�"
    '�J�����ݒ���
    Public Const EPC_CAMERA_GRID_GID As String = GAMEN_ID_EPC_CAMERA                            '// ���ID��DB�o�^�l
    Public Const EPC_CAMERA_GRID_NAME As String = "�J�����ݒ�"                                  '// ��ʕ\����̉��ID
    Public Const EPC_CAMERA_GRID_CNT As Integer = 3                                             '// ��ʂɕR�t���O���b�h��
    Public Const EPC_CAMERA_GRID_ID As String = "1,2,3"                                         '// �O���b�hID��DB�o�^�l
    Public Const EPC_CAMERA_GRID As String = "����,��׊p�x�E�����\�ʏ�ԕ�,��׊p�x�E��׾�āE�����\�ʏ�ԕ�" '// ��ʕ\����̃O���b�hID
    '���F�����ݒ���
    Public Const EPC_MIXCOLOR_GRID_GID As String = GAMEN_ID_EPC_MIXCOLOR                        '// ���ID��DB�o�^�l
    Public Const EPC_MIXCOLOR_GRID_NAME As String = "���F�����ݒ�"                              '// ��ʕ\����̉��ID
    Public Const EPC_MIXCOLOR_GRID_CNT As Integer = 1                                           '// ��ʂɕR�t���O���b�h��
    Public Const EPC_MIXCOLOR_GRID_ID As String = "1"                                           '// �O���b�hID��DB�o�^�l
    Public Const EPC_MIXCOLOR_GRID As String = "�J�����Z�b�g��"                                 '// ��ʕ\����̃O���b�hID
    '�G�b�W���o�ݒ���
    Public Const EPC_EDGE_GRID_GID As String = GAMEN_ID_EPC_EDGE                                '// ���ID��DB�o�^�l
    Public Const EPC_EDGE_GRID_NAME As String = "�G�b�W���o�ݒ�"                                '// ��ʕ\����̉��ID
    Public Const EPC_EDGE_GRID_CNT As Integer = 4                                               '// ��ʂɕR�t���O���b�h��
    Public Const EPC_EDGE_GRID_ID As String = "1,2,3,4"                                         '// �O���b�hID��DB�o�^�l
    Public Const EPC_EDGE_GRID As String = "�J�����Z�b�g��,��׾�āE�����\�ʏ�ԕ�,��׊p�x�E��׾�āE�����\�ʏ�ԕ�,�����\�ʏ�ԕ�" '// ��ʕ\����̃O���b�hID
    '����ިݸޕ␳�ݒ���
    Public Const EPC_SHADING_GRID_GID As String = GAMEN_ID_EPC_SHADING                          '// ���ID��DB�o�^�l
    Public Const EPC_SHADING_GRID_NAME As String = "����ިݸޕ␳�ݒ�"                          '// ��ʕ\����̉��ID
    Public Const EPC_SHADING_GRID_CNT As Integer = 2                                            '// ��ʂɕR�t���O���b�h��
    Public Const EPC_SHADING_GRID_ID As String = "1,2"                                          '// �O���b�hID��DB�o�^�l
    Public Const EPC_SHADING_GRID As String = "��׊p�x�E�����\�ʏ�ԕ�,�����\�ʏ�ԕ�"          '// ��ʕ\����̃O���b�hID
    '臒l�ݒ���
    Public Const EPC_THRESHOLD_GRID_GID As String = GAMEN_ID_EPC_THRESHOLD                      '// ���ID��DB�o�^�l
    Public Const EPC_THRESHOLD_GRID_NAME As String = "臒l�ݒ�"                                 '// ��ʕ\����̉��ID
    Public Const EPC_THRESHOLD_GRID_CNT As Integer = 1                                          '// ��ʂɕR�t���O���b�h��
    Public Const EPC_THRESHOLD_GRID_ID As String = "1"                                          '// �O���b�hID��DB�o�^�l
    Public Const EPC_THRESHOLD_GRID As String = "��׊p�x�E�����\�ʏ�ԕ�"                       '// ��ʕ\����̃O���b�hID
    '�r���o�ݒ���
    Public Const EPC_DEFECT_GRID_GID As String = GAMEN_ID_EPC_DEFECT                            '// ���ID��DB�o�^�l
    Public Const EPC_DEFECT_GRID_NAME As String = "�r���o�ݒ�"                                  '// ��ʕ\����̉��ID
    Public Const EPC_DEFECT_GRID_CNT As Integer = 2                                             '// ��ʂɕR�t���O���b�h��
    Public Const EPC_DEFECT_GRID_ID As String = "1,2"                                           '// �O���b�hID��DB�o�^�l
    Public Const EPC_DEFECT_GRID As String = "�����\�ʏ�ԕ�,��׊p�x�E�����\�ʏ�ԕ�"           '// ��ʕ\����̃O���b�hID
    '���������ݒ���
    Public Const EPC_MERGE_GRID_GID As String = GAMEN_ID_EPC_MERGE                              '// ���ID��DB�o�^�l
    Public Const EPC_MERGE_GRID_NAME As String = "���������ݒ�"                                 '// ��ʕ\����̉��ID
    Public Const EPC_MERGE_GRID_CNT As Integer = 1                                              '// ��ʂɕR�t���O���b�h��
    Public Const EPC_MERGE_GRID_ID As String = "1"                                              '// �O���b�hID��DB�o�^�l
    Public Const EPC_MERGE_GRID As String = "����"                                              '// ��ʕ\����̃O���b�hID
    ' <20221117> <����> <�ŏI�H�����@�\������1> --->>>
    '臒l�ݒ���
    Public Const EPC_TAJUUTHRESHOLD_GRID_GID As String = GAMEN_ID_EPC_TAJUUTHRESHOLD             '// ���ID��DB�o�^�l
    Public Const EPC_TAJUUTHRESHOLD_GRID_NAME As String = "���d����\臒l�ݒ�"                   '// ��ʕ\����̉��ID
    Public Const EPC_TAJUUTHRESHOLD_GRID_CNT As Integer = 3                                      '// ��ʂɕR�t���O���b�h��
    Public Const EPC_TAJUUTHRESHOLD_GRID_ID As String = "1,2,3"                                  '// �O���b�hID��DB�o�^�l
    Public Const EPC_TAJUUTHRESHOLD_GRID As String = "�����\�ʏ�ԕ�(�������x��),�����\�ʏ�ԕ�(�P����l��),�����\�ʏ�ԕ�(�t�B���^��l��)"      '// ��ʕ\����̃O���b�hID
    ' <20221117> <����> <�ŏI�H�����@�\������1> ---<<<
    '---------------------------------------------------------------------------

End Module
