using System.Collections;
using System;
using System.Collections.Generic;

namespace BeeEngine.Collections.Weak
{
    public sealed class WeakCollection<T> : ICollection<T> where T : class
    {
        private readonly List<WeakReference<T>> m_List = new List<WeakReference<T>>();

        private readonly object m_Locker = new object();

        public void Add(T item)
        {
            lock (m_Locker)
            {
                m_List.Add(new WeakReference<T>(item));
            }
        }

        public void Clear()
        {
            lock (m_Locker)
            {
                m_List.Clear();
            }
        }

        public int Count
        {
            get
            {
                int i;
                lock (m_Locker)
                {
                    i = m_List.Count;
                }
                return i;
            }
        }

        public bool IsReadOnly => false;

        public bool Contains(T item)
        {
            lock (m_Locker)
            {
                for (var index = 0; index < this.Count; index++)
                {
                    var element = this[index];
                    if (Equals(element, item))
                        return true;
                }

                return false;
            }
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            lock (m_Locker)
            {
                for (var index = 0; index < this.Count; index++)
                {
                    var element = this[index];
                    if(element is null)
                        continue;
                    array[arrayIndex++] = element;
                }
            }
        }

        public bool Remove(T item)
        {
            lock (m_Locker)
            {
                for (int i = 0; i < m_List.Count; i++)
                {
                    if (!m_List[i].TryGetTarget(out T? target))
                        continue;
                    if (Equals(target, item))
                    {
                        m_List.RemoveAt(i);
                        return true;
                    }
                }
                return false;
            }
        }

        public bool RemoveAt(int index)
        {
            lock (m_Locker)
            {
                m_List.RemoveAt(index);
                return true;
            }
        }

        public IEnumerator<T> GetEnumerator()
        {
            lock (m_Locker)
            {
                for (int i = m_List.Count - 1; i >= 0; i--)
                {
                    if (!m_List[i].TryGetTarget(out T? element))
                    {
                        m_List.RemoveAt(i);
                        continue;
                    }
                    yield return element;
                }
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public T? this[int index]
        {
            get
            {
                lock (m_Locker)
                {
                    m_List[index].TryGetTarget(out T? result);
                    return result;
                }
            }
            set
            {
                lock (m_Locker)
                {
                    DebugLog.AssertAndThrow(value is not null, "Value is null");
                    m_List[index].SetTarget(value!);
                }
            }
        }

        /*internal void Sort(Comparison<T> comparison)
        {
            Comparer<T> comparer = Comparer<T>.Create(comparison);
            list.Sort((WeakReference<T> x, WeakReference<T> y) =>
            {
                x.TryGetTarget(out T a);
                x.TryGetTarget(out T b);
                return comparer.Compare(a, b);
            });
        }*/
    }
}