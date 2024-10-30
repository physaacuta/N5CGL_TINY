using System;
using System.Collections.Generic;
using System.Text;

namespace DBAccessN5CGL_TINY
{
    /// <summary>
    /// �o���R�C��
    /// </summary>
    public class CoilListItem
    {
        #region �R�C���ꗗ�����o

        /// <summary>
        /// �o�^���t
        /// </summary>
        private DateTime m_datetime;

        /// <summary>
        /// �Ǘ�No
        /// </summary>
        private string m_filename;

        /// <summary>
        /// �R�C��No
        /// </summary>
        private string m_coilnum;

        /// <summary>
        /// �J�b�g�ԍ�(0�`) (�q��)
        /// </summary>
        private int m_cut_no;

        /// <summary>
        /// �����R�C���̃J�b�g��
        /// </summary>
        private int m_ncut;

        /// <summary>
        /// �����R�C���̍̐��J�n�ʒu[mm]
        /// </summary>
        private int m_pos_from;

        /// <summary>
        /// �����R�C���̍̐��I���ʒu[mm]
        /// </summary>
        private int m_pos_to;

        /// <summary>
        /// �����R�C���̍̐��I���ʒu[mm] (�r��T���Ƃ��Ɏg��)
        /// </summary>
        private int m_pos_to_defect;

        /// <summary>
        /// �ޗ���[mm]
        /// </summary>
        private int m_width;

        /// <summary>
        /// ������[mm]
        /// </summary>
        private float m_thickness;

        /// <summary>
        /// ����������[mm]
        /// </summary>
        private int m_length_base;

        /// <summary>
        /// �����\�ʏ��_�\
        /// </summary>
        private string m_surface_up;

        /// <summary>
        /// �����\�ʏ��_��
        /// </summary>
        private string m_surface_dn;

        /// <summary>
        /// �����_�\
        /// </summary>
        private string m_insp_level_up;

        /// <summary>
        /// �����_��
        /// </summary>
        private string m_insp_level_dn;

        /// <summary>
        /// ��ގ��R�[�h
        /// </summary>
        private string m_primary_context;

        /// <summary>
        /// �\�ʏ���1����
        /// </summary>
        private string m_surf_proc_dig1;

        /// <summary>
        /// �\�ʏ���2����
        /// </summary>
        private string m_surf_proc_dig2;

        /// <summary>
        /// �v�掟�H���R�[�h
        /// </summary>
        private string m_next_proc_sched;

        /// <summary>
        /// �\�ۑ��r��
        /// </summary>
        private int m_total_ndefects_up;

        /// <summary>
        /// ���ۑ��r��
        /// </summary>
        private int m_total_ndefects_dn;

        #endregion


        /// <summary>
        /// �R���X�g���N�^
        /// </summary>
        /// <param name="datetime">�o�^���t</param>
        /// <param name="filename">�Ǘ�No</param>
        /// <param name="coilnum">�R�C��No</param>
        /// <param name="cut_no">�J�b�g�ԍ�</param>
        /// <param name="ncut">�����R�C���̃J�b�g��</param>
        /// <param name="pos_from">�����R�C���̍̐��J�n�ʒu</param>
        /// <param name="pos_to">�����R�C���̍̐��I���ʒu</param>
        /// <param name="pos_to_defect">�����R�C���̍̐��I���ʒu (�r��T���Ƃ��Ɏg��)</param>
        /// <param name="width">�ޗ���</param>
        /// <param name="thickness">������</param>
        /// <param name="length_base">����������</param>
        /// <param name="surface_up">�����\�ʏ��_�\</param>
        /// <param name="surface_dn">�����\�ʏ��_��</param>
        /// <param name="insp_level_up">�����_�\</param>
        /// <param name="insp_level_dn">�����_��</param>
        /// <param name="primary_context">��ގ��R�[�h</param>
        /// <param name="surf_proc_dig1">�\�ʏ���1����</param>
        /// <param name="surf_proc_dig2">�\�ʏ���2����</param>
        /// <param name="next_proc_sched">�v�掟�H���R�[�h</param>
        /// <param name="total_ndefects_up">�\�ۑ��r��</param>
        /// <param name="total_ndefects_dn">���ۑ��r��</param>
        public CoilListItem(
            DateTime datetime,
            string filename,
            string coilnum,
            int cut_no,
            int ncut,
            int pos_from,
            int pos_to,
            int pos_to_defect,
            int width,
            float thickness,
            int length_base,
            string surface_up,
            string surface_dn,
            string insp_level_up,
            string insp_level_dn,
            string primary_context,
            string surf_proc_dig1,
            string surf_proc_dig2,
            string next_proc_sched,
            int total_ndefects_up,
            int total_ndefects_dn)
        {
            m_datetime = datetime;
            m_filename = filename;
            m_coilnum = coilnum + "_" + (cut_no+1) + "_" + (ncut+1);
            m_cut_no = cut_no;
            m_ncut = ncut;
            m_pos_from = pos_from;
            m_pos_to = pos_to;
            m_pos_to_defect = pos_to_defect;
            m_width = width;
            m_thickness = thickness;
            m_length_base = length_base;
            m_surface_up = surface_up;
            m_surface_dn = surface_dn;
            m_insp_level_up = insp_level_up;
            m_insp_level_dn = insp_level_dn;
            m_primary_context = primary_context;
            m_surf_proc_dig1 = surf_proc_dig1;
            m_surf_proc_dig2 = surf_proc_dig2;
            m_next_proc_sched = next_proc_sched;
            m_total_ndefects_up = total_ndefects_up;
            m_total_ndefects_dn = total_ndefects_dn;
        }

        #region �R�C���ꗗ�����o(�O���Q��)

        /// <summary>
        /// �o�^���t
        /// </summary>
        /// <remarks>�K�{</remarks>
        public DateTime DateTime
        {
            get { return m_datetime; }
        }

        /// <summary>
        /// �Ǘ�No
        /// </summary>
        /// <remarks>�K�{</remarks>
        public string Filename
        {
            get { return m_filename; }
        }

        /// <summary>
        /// �R�C��No
        /// </summary>
        public string CoilNum
        {
            get { return m_coilnum; }
        }

        /// <summary>
        /// �o���P�ʂƂ��Ă̒���[mm]
        /// </summary>
        public int LengthAfterCut
        {
            get { return m_pos_to - m_pos_from; }
        }

        /// <summary>
        /// �����P�ʂƂ��Ă̒���[mm]
        /// </summary>
        public int LengthBase
        {
            get { return m_length_base; }
        }

        /// <summary>
        /// �����R�C���̍̐��J�n�ʒu[mm]
        /// </summary>
        public int PosFrom
        {
            get { return m_pos_from; }
        }

        /// <summary>
        /// �����R�C���̍̐��I���ʒu[mm]
        /// </summary>
        public int PosTo
        {
            get { return m_pos_to; }
        }

        /// <summary>
        /// �����R�C���̍̐��I���ʒu[mm] (�r��T���Ƃ��Ɏg��)
        /// </summary>
        internal int PosToDefect
        {
            get { return m_pos_to_defect; }
        }

        /// <summary>
        /// �ޗ���[mm]
        /// </summary>
        public int Width
        {
            get { return m_width; }
        }

        /// <summary>
        /// ������[mm]
        /// </summary>
        public float Thickness
        {
            get { return m_thickness; }
        }

        /// <summary>
        /// �����\�ʏ��_�\
        /// </summary>
        public string SurfaceUp
        {
            get { return m_surface_up; }
        }

        /// <summary>
        /// �����\�ʏ��_��
        /// </summary>
        public string SurfaceDown
        {
            get { return m_surface_dn; }
        }

        /// <summary>
        /// �����_�\
        /// </summary>
        public string InspLevelUp
        {
            get { return m_insp_level_up; }
        }

        /// <summary>
        /// �����_��
        /// </summary>
        public string InspLevelDown
        {
            get { return m_insp_level_dn; }
        }

        /// <summary>
        /// ��ގ��R�[�h
        /// </summary>
        public string PrimaryContext
        {
            get { return m_primary_context; }
        }

        /// <summary>
        /// �\�ʏ���1����
        /// </summary>
        public string SurfProcDig1
        {
            get { return m_surf_proc_dig1; }
        }

        /// <summary>
        /// �\�ʏ���2����
        /// </summary>
        public string SurfProcDig2
        {
            get { return m_surf_proc_dig2; }
        }

        /// <summary>
        /// �v�掟�H���R�[�h
        /// </summary>
        public string NextProcSched
        {
            get { return m_next_proc_sched; }
        }

        /// <summary>
        /// �\�ۑ��r��
        /// </summary>
        public int TotalNDefectsUp
        {
            get { return m_total_ndefects_up; }
        }

        /// <summary>
        /// ���ۑ��r��
        /// </summary>
        public int TotalNDefectsDn
        {
            get { return m_total_ndefects_dn; }
        }

        #endregion

        #region �����r��A�O���[�h���Ƃ��r��

        /// <summary>
        /// �r�킲�Ƃ��r��
        /// </summary>
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_type_up = new Dictionary<int, int>();
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_type_dn = new Dictionary<int, int>();

        /// <summary>
        /// �O���[�h���Ƃ��r��
        /// </summary>
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_sevirity_up = new Dictionary<int, int>();
        private System.Collections.Generic.Dictionary<int, int> m_ndefects_on_sevirity_dn = new Dictionary<int, int>();

        /// <summary>
        /// �r�킲�Ƃ��r�����Z�b�g
        /// </summary>
        /// <param name="insp_surface">�\or��</param>
        /// <param name="type">�r��</param>
        /// <param name="n">�r��</param>
        internal void SetNDefectsOnType(SwdData.InspSurface insp_surface, int type, int n)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    m_ndefects_on_type_up[type] = n;
                    break;

                case SwdData.InspSurface.Down:
                    m_ndefects_on_type_dn[type] = n;
                    break;
            }
        }

        /// <summary>
        /// �r�킲�Ƃ��r�����擾
        /// </summary>
        /// <param name="insp_surface">�\or��</param>
        /// <param name="type">�r��</param>
        /// <returns>�r��</returns>
        public int GetNDefectsOnType(SwdData.InspSurface insp_surface, int type)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    return m_ndefects_on_type_up.ContainsKey(type) ? m_ndefects_on_type_up[type] : 0;

                case SwdData.InspSurface.Down:
                    return m_ndefects_on_type_dn.ContainsKey(type) ? m_ndefects_on_type_dn[type] : 0;
            }
            return 0;
        }

        /// <summary>
        /// �O���[�h���Ƃ��r�����Z�b�g
        /// </summary>
        /// <param name="insp_surface">�\or��</param>
        /// <param name="sevirity">�O���[�h</param>
        /// <param name="n">�r��</param>
        internal void SetNDefectsOnSevirity(SwdData.InspSurface insp_surface, int sevirity, int n)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    m_ndefects_on_sevirity_up[sevirity] = n;
                    break;

                case SwdData.InspSurface.Down:
                    m_ndefects_on_sevirity_dn[sevirity] = n;
                    break;
            }
        }

        /// <summary>
        /// �O���[�h���Ƃ��r�����擾
        /// </summary>
        /// <param name="insp_surface">�\or��</param>
        /// <param name="sevirity">�O���[�h</param>
        /// <returns>�r��</returns>
        public int GetNDefectsOnSevirity(SwdData.InspSurface insp_surface, int sevirity)
        {
            switch (insp_surface)
            {
                case SwdData.InspSurface.Up:
                    return m_ndefects_on_sevirity_up.ContainsKey(sevirity) ? m_ndefects_on_sevirity_up[sevirity] : 0;

                case SwdData.InspSurface.Down:
                    return m_ndefects_on_sevirity_dn.ContainsKey(sevirity) ? m_ndefects_on_sevirity_dn[sevirity] : 0;
            }
            return 0;
        }

#endregion

    }
}
