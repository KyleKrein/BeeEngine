using System;
using System.Collections.Generic;
using System.Linq;

namespace BeeEngine.Events
{
    public class WeakEvent<TEventArgs>
    {
        private event EventHandler<TEventArgs> _eventKeeper;
        private readonly HashSet<OwnWeakReference<TEventArgs>> _activeListenersOfThisType = new HashSet<OwnWeakReference<TEventArgs>>();
        public string EventID { get; }= Guid.NewGuid().ToString();
        private readonly object locker = new object();

        public bool HasDuplicates(object listener)
        {
            lock(locker)
            {
                return _activeListenersOfThisType.Any(k => k.holder == listener);
            }
        }

        internal void AddToEvent(object listener, EventHandler<TEventArgs> action)
        {
            lock(locker)
            {
                var newAction = new OwnWeakReference<TEventArgs>(listener, action);
                _activeListenersOfThisType.Add(newAction);
                _eventKeeper += newAction.EventHandler;
            }
        }

        public static WeakEvent<TEventArgs> operator +(WeakEvent<TEventArgs> Event, EventHandler<TEventArgs> action)
        {
            if (action.Target == null)
            {
                throw new Exception("Use lyambda expression when subscribing or unsubscribing to WeakEvent");
            }
            Event.AddToEvent(action.Target, action);
            return Event;
        }
        public static WeakEvent<TEventArgs> operator -(WeakEvent<TEventArgs> Event, EventHandler<TEventArgs> action)
        {
            if (Event == null)
            {
                return null;
            }
            if (action.Target == null)
            {
                throw new Exception("Use lyambda expression when subscribing or unsubscribing to WeakEvent");
            }
            Event.RemoveFromEvent(action.Target);
            return Event;
        }
        public static WeakEvent<TEventArgs> operator -(WeakEvent<TEventArgs> Event, object listener)
        {
            if (listener == null)
                throw new ArgumentNullException();
            if (Event == null)
            {
                return null;
            }
            Event.RemoveFromEvent(listener);
            return Event;
        }

        internal void RemoveFromEvent(object listener)
        {
            lock(locker)
            {
                var currentEvent = _activeListenersOfThisType.FirstOrDefault(k => k.holder.Target == listener);
                if (currentEvent.holder != null)
                {
                    _eventKeeper -= currentEvent.EventHandler;
                    _activeListenersOfThisType.Remove(currentEvent);
                }
            }
        }

        public void Invoke(object/*?*/ sender, TEventArgs eventArgs)
        {
            lock(locker)
            {
                if (_activeListenersOfThisType.Any(k => k.IsDead()))
                {
                    var failObjList = _activeListenersOfThisType.Where(k => k.IsDead());
                    foreach (var fail in failObjList.ToArray())
                    {
                        _eventKeeper -= fail.EventHandler;
                        _activeListenersOfThisType.Remove(fail);
                    }
                }

                _eventKeeper?.Invoke(sender, eventArgs);
            }
        }

        internal string DebugInfo()
        {
            lock(locker)
            {
                string info = string.Empty;
                info += _activeListenersOfThisType.Count + " Count \n";
                foreach (var c in _activeListenersOfThisType)
                {
                    if (c.holder.Target != null)
                        info += c.holder.Target + "\n";
                }
                return info;
            }
        }
    }

    internal struct OwnWeakReference<T>
    {
        public WeakReference holder;
        public EventHandler<T> EventHandler;

        public OwnWeakReference(object obj, EventHandler<T> eventHandler)
        {
            holder = new WeakReference(obj);
            EventHandler = eventHandler;
        }

        public bool IsDead()
        {
            return holder.Target == null || holder.Target.ToString() == "null" || !holder.IsAlive;
        }
    }
}
