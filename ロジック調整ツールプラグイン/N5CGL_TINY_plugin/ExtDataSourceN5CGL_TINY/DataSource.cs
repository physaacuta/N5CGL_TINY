using System;
using System.Collections.Generic;
using System.Text;

namespace ExtDataSourceN5CGL_TINY
{
    public class DataSource : SwdOnlineIF.IDataSource, SwdOnlineIF.IDataSourceDefectSearch
    {
        public DataSource()
        {
        }

        private DBAccessN5CGL_TINY.DBAccess m_db = new DBAccessN5CGL_TINY.DBAccess();

        private SwdData.ITermListConst m_termlist = null;

        private int[] m_defect_types = null;

        #region IDataSource �����o

        public string Title
        {
            get { return "N5CGL_TINY(�R�C���ꗗ)"; }
        }

        public string Description
        {
            get { return "���É�5CGL�ɔ����������u��SQL�T�[�o���ɂ����r�f�[�^"; }
        }

        public string Path
        {
            get { return System.Reflection.Assembly.GetExecutingAssembly().Location; }
        }

        public string Version
        {
            get { return System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(); }
        }

        public void Init(System.Data.IDbConnection connection, int command_timeout)
        {
            m_db.CommandTimeout = command_timeout;

            // ������
            m_db.Init(connection);

            // ���ڃ��X�g
            m_termlist = m_db.GetTermlist(connection);

            // �r���ID��z��ɂ���
            System.Collections.Generic.ICollection<SwdData.TermItem> types = m_termlist.GetSortedTermItems(SwdData.Terms.DefectType);
            m_defect_types = new int[types.Count];
            int i = 0;
            foreach (SwdData.TermItem type in types)
            {
                m_defect_types[i++] = type.Id;
            }
        }

        public void Dispose()
        {
            m_db.Dispose();
            m_termlist = null;
        }

        public SwdData.ITermListConst GetTermlist()
        {
            return m_termlist;
        }

        /// <summary>
        /// �R�C���ꗗ�ɕ\������񖼏�
        /// </summary>
        public string[] ListColumnTitle
        {
            get
            {
                System.Collections.Generic.List<string> titles = new List<string>();

                titles.Add("�o�^���t");
                titles.Add("�R�C��No");
                titles.Add("�����_�\");
                titles.Add("�����_��");
                titles.Add("��ގ�");
                titles.Add("�\�ʏ���1����");
                titles.Add("�v�掟�H���R�[�h");
                titles.Add("�\�r��");
                titles.Add("���r��");
                foreach (int type in m_defect_types)
                {
                    titles.Add(m_termlist.GetName(SwdData.Terms.DefectType, type) + "(�\)");
                    titles.Add(m_termlist.GetName(SwdData.Terms.DefectType, type) + "(��)");
                }
                titles.Add("�ޗ���[mm]");
                titles.Add("��[m]");
                titles.Add("������[mm]");
                titles.Add("�\�ʏ��(�\)");
                titles.Add("�\�ʏ��(��)");

                return titles.ToArray();
            }
        }

        /// <summary>
        /// �R�C���ꗗ�ɕ\�������̃f�[�^�^
        /// </summary>
        public SwdData.ListViewSorter ListViewSorter
        {
            get
            {
                System.Collections.Generic.List<SwdData.ListViewSorter.ColumnTypes> types = new List<SwdData.ListViewSorter.ColumnTypes>();

                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Integer);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Integer);
                for (int i = 0; i < m_defect_types.Length; i++)
                {
                    types.Add(SwdData.ListViewSorter.ColumnTypes.Integer); // �\
                    types.Add(SwdData.ListViewSorter.ColumnTypes.Integer); // ��
                }
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.Float);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);
                types.Add(SwdData.ListViewSorter.ColumnTypes.String);

                return new SwdData.ListViewSorter(types.ToArray());
            }
        }

        /// <summary>
        /// �R�C���ꗗ�̍X�V�{�^��������
        /// </summary>
        public SwdOnlineIF.IProduct[] GetProducts(System.Data.IDbConnection connection, 
            DateTime from, DateTime to, bool count_ndefects, SwdData.IFormProgress progress)
        {
            DBAccessN5CGL_TINY.CoilListItem[] coils = m_db.GetCoils(connection, from, to, count_ndefects, false, progress);
            if (null == coils || 0 == coils.Length) return null;

            SwdOnlineIF.IProduct[] products = new SwdOnlineIF.IProduct[coils.Length];
            int i = 0;
            foreach (DBAccessN5CGL_TINY.CoilListItem coil in coils)
            {
                products[i++] = new Coil(coil, m_defect_types);
            }

            return products;
        }

        /// <summary>
        /// �R�C���ꗗ�̌����{�^��������
        /// </summary>
        public SwdOnlineIF.IProduct[] GetProducts(System.Data.IDbConnection connection, string product_num, bool count_ndefects)
        {
            DBAccessN5CGL_TINY.CoilListItem[] coils = m_db.GetCoil(connection, product_num, count_ndefects, false);
            if (null == coils || 0 == coils.Length) return null;

            SwdOnlineIF.IProduct[] products = new SwdOnlineIF.IProduct[coils.Length];
            int i = 0;
            foreach (DBAccessN5CGL_TINY.CoilListItem coil in coils)
            {
                products[i++] = new Coil(coil, m_defect_types);
            }

            return products;
        }

        /// <summary>
        /// �R�C���ꗗ�̃R�C���I�������r���\����
        /// </summary>
        public int GetDefects(System.Data.IDbConnection connection,
            SwdOnlineIF.IProduct product, int[] types, int[] sevirities,
            float postop_from, float postop_to, float postail_from, float postail_to,
            SwdData.InspSurface insp_surface,
            SwdData.IDefectItemCollection defect_items, SwdData.IFormProgress progress)
        {
            Coil coil = product as Coil;
            if (null == coil) return 0;

            List<DBAccessN5CGL_TINY.DefectHandle> handles = new List<DBAccessN5CGL_TINY.DefectHandle>();
            if (float.IsNaN(postop_from) && float.IsNaN(postop_to) && float.IsNaN(postail_from) && float.IsNaN(postail_to))
            {
                // �R�C���S�����Ώ�
                m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, float.NaN, float.NaN, insp_surface, handles);
            }
            else
            {
                if (!float.IsNaN(postop_from) || !float.IsNaN(postop_to))
                {
                    m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, postop_from, postop_to, insp_surface, handles);
                }
                if (!float.IsNaN(postail_from) || !float.IsNaN(postail_to))
                {
                    float from = float.IsNaN(postail_to) ? float.NaN : coil.ProductLength - postail_to;
                    float to = float.IsNaN(postail_from) ? float.NaN : coil.ProductLength - postail_from;
                    m_db.GetDefectHandle(connection, coil.CoilBase, types, sevirities, from, to, insp_surface, handles);
                }
            }
            if (0 == handles.Count) return 0;

            if (null != progress)
            {
                progress.Val = 0;
                progress.Max = handles.Count;
            }
            return m_db.GetDefectByHandle(connection, handles, defect_items, progress);
        }

        public bool IsPosTopRangeSupported
        {
            get { return true; }
        }

        public bool IsPosTailRangeSupported
        {
            get { return true; }
        }

        #endregion

        #region IDataSourceDefectSearch �����o

        string SwdOnlineIF.IDataSourceDefectSearch.Title
        {
            get { return "N5CGL_TINY(�r����)"; }
        }

        string SwdOnlineIF.IDataSourceDefectSearch.Description
        {
            get { return "���É�5CGL�ɔ����������u��SQL�T�[�o���ɂ����r�f�[�^����������"; }
        }                

        public object[] SearchDefects(System.Data.IDbConnection connection, SwdOnlineIF.SearchCondition condition, SwdData.IFormProgress progress)
        {
            DBAccessN5CGL_TINY.NumFeatSearchCond[] numfeat_cond = null;
            if (null != condition.NumFeatureCond)
            {
                numfeat_cond = new DBAccessN5CGL_TINY.NumFeatSearchCond[condition.NumFeatureCond.Length];
                for (int i = 0; i < numfeat_cond.Length; i++)
                {
                    numfeat_cond[i] = new DBAccessN5CGL_TINY.NumFeatSearchCond(
                        condition.NumFeatureCond[i].Id,
                        condition.NumFeatureCond[i].Lower,
                        condition.NumFeatureCond[i].Upper);
                }
            }

            // �r�����p �����L�[�̓��͏�Ԏ擾
            string insp_level_up = null;
            string insp_level_dn = null;
            string prim_cntxt = null;
            string surf_proc_dig1 = null;
            string next_proc_sched = null;
            if (null != condition.ExtraSearchCond)
            {
                foreach (SwdOnlineIF.ExtraSearchCond cond in condition.ExtraSearchCond)
                {
                    switch (cond.Name)
                    {
                        case "�����_�\":
                            insp_level_up = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "�����_��":
                            insp_level_dn = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "��ޗ�":
                            prim_cntxt = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "�\�ʏ���1����":
                            surf_proc_dig1 = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;

                        case "�v�掟�H���R�[�h":
                            next_proc_sched = (cond as SwdOnlineIF.ExtraSearchCondString).Value;
                            break;
                    }
                }
            }

            return m_db.SearchDefects(connection,
                condition.UseDateTime, condition.DateTimeFrom, condition.DateTimeTo,
                condition.ProductNum,
                condition.UseSurface, condition.Surface,
                condition.Types, condition.Sevirities,
                condition.UseWidth, condition.WidthLower, condition.WidthUpper,
                condition.UseLength, condition.LengthLower, condition.LengthUpper,
                condition.UsePosLeft, condition.PosLeftLower, condition.PosLeftUpper,
                condition.UsePosRight, condition.PosRightLower, condition.PosRightUpper,
                condition.UsePosTop, condition.PosTopLower, condition.PosTopUpper,
                numfeat_cond,
                insp_level_up,
                insp_level_dn,
                prim_cntxt,
                surf_proc_dig1,
                next_proc_sched,
                progress);
        }

        public int GetDefects(System.Data.IDbConnection connection, object[] handles, SwdData.IDefectItemCollection defect_items, SwdData.IFormProgress progress)
        {
            progress.Max = handles.Length;
            progress.Val = 0;
            int nerror = 0;
            foreach (DBAccessN5CGL_TINY.DefectHandle handle in handles)
            {
                SwdData.IDefectItem defect_item = m_db.GetDefectByHandle(connection, handle);
                if (null == defect_item)
                {
                    nerror++;
                }
                else
                {
                    defect_items.Add(defect_item);
                }
                progress.Val++;
                if (progress.IsCanceled) break;
            }
            return nerror;
        }

        public SwdOnlineIF.ExtraSearchCondConfig[] GetExtraSearchCondConfigs()
        {
            return new SwdOnlineIF.ExtraSearchCondConfig[]
            {
                // �r���� ���������ǉ��L�[���`
                new SwdOnlineIF.ExtraSearchCondConfig("�����_�\",        typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("�����_��",        typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("��ޗ�",             typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("�\�ʏ���1����",      typeof(string)),
                new SwdOnlineIF.ExtraSearchCondConfig("�v�掟�H���R�[�h",   typeof(string))
            };
        }

        #endregion
    }
}
