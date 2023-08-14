using System.Collections;
using System.Collections.Generic;
using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;

namespace BeeEngine.Collections.Weak
{
    public sealed class WeakDictionary<TKey, TValue> : IDictionary<TKey, TValue>, IDisposable
      where TKey : class
      where TValue : class
    {
        private readonly object locker = new object();
        private ConditionalWeakTable<TKey, WeakKeyHolder> keyHolderMap = new ConditionalWeakTable<TKey, WeakKeyHolder>();
        private Dictionary<WeakReference, TValue> valueMap = new Dictionary<WeakReference, TValue>(new ObjectReferenceEqualityComparer<WeakReference>());

        private class WeakKeyHolder
        {
            private WeakDictionary<TKey, TValue> outer;

            public WeakKeyHolder(WeakDictionary<TKey, TValue> outer, TKey key)
            {
                this.outer = outer;
                this.WeakRef = new WeakReference(key);
            }

            public WeakReference WeakRef { get; private set; }

            ~WeakKeyHolder()
            {
                this.outer?.onKeyDrop(this.WeakRef);  // Nullable operator used just in case this.outer gets set to null by GC before this finalizer runs. But I haven't had this happen.
            }
        }

        private void onKeyDrop(WeakReference weakKeyRef)
        {
            lock (this.locker)
            {
                if (!this.bAlive)
                    return;

                this.valueMap.Remove(weakKeyRef);
            }
        }

        // The reason for this is in case (for some reason which I have never seen) the finalizer trigger doesn't work
        // There is not much performance penalty with this, since this is only called in cases when we would be enumerating the inner collections anyway.
        private void manualShrink()
        {
            var keysToRemove = this.valueMap.Keys.Where(k => !k.IsAlive).ToList();

            foreach (var key in keysToRemove)
                valueMap.Remove(key);
        }

        private Dictionary<TKey, TValue> currentDictionary
        {
            get
            {
                lock (this.locker)
                {
                    this.manualShrink();
                    return this.valueMap.ToDictionary(p => (TKey)p.Key.Target, p => p.Value);
                }
            }
        }

        public TValue this[TKey key]
        {
            get
            {
                if (this.TryGetValue(key, out var val))
                    return val;

                throw new KeyNotFoundException();
            }

            set
            {
                this.set(key, value, isUpdateOkay: true);
            }
        }

        private bool set(TKey key, TValue val, bool isUpdateOkay)
        {
            lock (this.locker)
            {
                if (this.keyHolderMap.TryGetValue(key, out var weakKeyHolder))
                {
                    if (!isUpdateOkay)
                        return false;

                    this.valueMap[weakKeyHolder.WeakRef] = val;
                    return true;
                }

                weakKeyHolder = new WeakKeyHolder(this, key);
                this.keyHolderMap.Add(key, weakKeyHolder);
                this.valueMap.Add(weakKeyHolder.WeakRef, val);

                return true;
            }
        }

        public ICollection<TKey> Keys
        {
            get
            {
                lock (this.locker)
                {
                    this.manualShrink();
                    return this.valueMap.Keys.Select(k => (TKey)k.Target).ToList();
                }
            }
        }

        public ICollection<TValue> Values
        {
            get
            {
                lock (this.locker)
                {
                    this.manualShrink();
                    return this.valueMap.Select(p => p.Value).ToList();
                }
            }
        }

        public int Count
        {
            get
            {
                lock (this.locker)
                {
                    this.manualShrink();
                    return this.valueMap.Count;
                }
            }
        }

        public bool IsReadOnly => false;

        public void Add(TKey key, TValue value)
        {
            if (!this.set(key, value, isUpdateOkay: false))
                throw new ArgumentException("Key already exists");
        }

        public void Add(KeyValuePair<TKey, TValue> item)
        {
            this.Add(item.Key, item.Value);
        }

        public void Clear()
        {
            lock (this.locker)
            {
                this.keyHolderMap = new ConditionalWeakTable<TKey, WeakKeyHolder>();
                this.valueMap.Clear();
            }
        }

        public bool Contains(KeyValuePair<TKey, TValue> item)
        {
            WeakKeyHolder weakKeyHolder = null;
            object curVal = null;

            lock (this.locker)
            {
                if (!this.keyHolderMap.TryGetValue(item.Key, out weakKeyHolder))
                    return false;

                curVal = weakKeyHolder.WeakRef.Target;
            }

            return (curVal?.Equals(item.Value) == true);
        }

        public bool ContainsKey(TKey key)
        {
            lock (this.locker)
            {
                return this.keyHolderMap.TryGetValue(key, out var weakKeyHolder);
            }
        }

        public void CopyTo(KeyValuePair<TKey, TValue>[] array, int arrayIndex)
        {
            ((IDictionary<TKey, TValue>)this.currentDictionary).CopyTo(array, arrayIndex);
        }

        public IEnumerator<KeyValuePair<TKey, TValue>> GetEnumerator()
        {
            return this.currentDictionary.GetEnumerator();
        }

        public bool Remove(TKey key)
        {
            lock (this.locker)
            {
                if (!this.keyHolderMap.TryGetValue(key, out var weakKeyHolder))
                    return false;

                this.keyHolderMap.Remove(key);
                this.valueMap.Remove(weakKeyHolder.WeakRef);

                return true;
            }
        }

        public bool Remove(KeyValuePair<TKey, TValue> item)
        {
            lock (this.locker)
            {
                if (!this.keyHolderMap.TryGetValue(item.Key, out var weakKeyHolder))
                    return false;

                if (weakKeyHolder.WeakRef.Target?.Equals(item.Value) != true)
                    return false;

                this.keyHolderMap.Remove(item.Key);
                this.valueMap.Remove(weakKeyHolder.WeakRef);

                return true;
            }
        }

        public bool TryGetValue(TKey key, out TValue value)
        {
            lock (this.locker)
            {
                if (!this.keyHolderMap.TryGetValue(key, out var weakKeyHolder))
                {
                    value = default(TValue);
                    return false;
                }

                value = this.valueMap[weakKeyHolder.WeakRef];
                return true;
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        private bool bAlive = true;

        public void Dispose()
        {
            this.Dispose(true);
        }

        private void Dispose(bool bManual)
        {
            if (bManual)
            {
                Monitor.Enter(this.locker);

                if (!this.bAlive)
                    return;
            }

            try
            {
                this.keyHolderMap = null;
                this.valueMap = null;
                this.bAlive = false;
            }
            finally
            {
                if (bManual)
                    Monitor.Exit(this.locker);
            }
        }

        ~WeakDictionary()
        {
            this.Dispose(false);
        }
    }

    public sealed class ObjectReferenceEqualityComparer<T> : IEqualityComparer<T>
    {
        public static ObjectReferenceEqualityComparer<T> Default = new ObjectReferenceEqualityComparer<T>();

        public bool Equals(T x, T y)
        {
            return ReferenceEquals(x, y);
        }

        public int GetHashCode(T obj)
        {
            return RuntimeHelpers.GetHashCode(obj);
        }
    }
}