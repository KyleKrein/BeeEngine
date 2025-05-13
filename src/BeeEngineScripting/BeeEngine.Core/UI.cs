using BeeEngine.Internal;

namespace BeeEngine.UI;
public delegate void UIEventCallback();
public enum UIEventId : ushort
{
		Invalid,

		// Core events
		Mousedown,
		Mousescroll,
		Mouseover,
		Mouseout,
		Focus,
		Blur,
		Keydown,
		Keyup,
		Textinput,
		Mouseup,
		Click,
		Dblclick,
		Load,
		Unload,
		Show,
		Hide,
		Mousemove,
		Dragmove,
		Drag,
		Dragstart,
		Dragover,
		Dragdrop,
		Dragout,
		Dragend,
		Handledrag,
		Resize,
		Scroll,
		Animationend,
		Transitionend,

		// Form control events
		Change,
		Submit,
		Tabchange,

		NumDefinedIds,

		// Custom IDs start here
		FirstCustomId = NumDefinedIds,

		// The maximum number of IDs. This limits the number of possible custom IDs to MaxNumIds - FirstCustomId.
		MaxNumIds = 0xffff
}
public class Document
{
		static Dictionary<ulong, Dictionary<string, Dictionary<UIEventId, /*WeakReference<*/UIEventCallback/*>*/>>> s_Events = new();
		internal static void EmitEvent(ulong id, string elementId, UIEventId eventType, InternalCalls.ArrayInfo eventData)
		{
				Log.Info("UI Event {0} on {1} (document {2})", eventType, elementId, id);
				if (!s_Events.ContainsKey(id) || !s_Events[id].ContainsKey(elementId) || !s_Events[id][elementId].ContainsKey(eventType))
				{
						Log.Info("Haven't found a target for event invokation: {0} on {1} (document {2})", eventType, elementId, id);
						return;
				}
				try
				{
						s_Events[id][elementId][eventType]?.Invoke();
				}
				catch (Exception exception)
				{
						Log.Error("Exception thrown when handling UI Event! {}", exception.InnerException);
				}
		}
		public Document(string name)
		{
				if (!name.Contains(".rml"))
				{
						name += ".rml";
				}
				m_Handle = InternalCalls.UI_CreateDocument(name);
				if (m_Handle == 0)
				{
						throw new FileNotFoundException(name);
				}
		}
		public void Show()
		{
				ThrowIfInvalid();
				Log.Info("Document {0} shown", m_Handle);
				InternalCalls.UI_ShowDocument(m_Handle);
		}
		public void Hide()
		{
				ThrowIfInvalid();
				Log.Info("Document {0} hidden", m_Handle);
				InternalCalls.UI_HideDocument(m_Handle);
		}
		public void Close()
		{
				ThrowIfInvalid();
				Log.Info("Trying to close Document {0}", m_Handle);
				InternalCalls.UI_CloseDocument(m_Handle);
				if (s_Events.ContainsKey(m_Handle))
				{
						s_Events.Remove(m_Handle);
						Log.Info("Removed all events from Document {0}", m_Handle);
				}
				Log.Info("Document {0} closed", m_Handle);
				m_Handle = 0;
		}
		public void SetText(string elementId, string text)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetText(m_Handle, elementId, text);
		}
		public void SetVisibility(string elementId, bool visible)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetVisibility(m_Handle, elementId, visible);
		}
		public void SetClass(string elementId, string className, bool add)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetClass(m_Handle, elementId, className, add);
		}
		public void SetAttribute(string elementId, string attributeName, string value)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetAttribute(m_Handle, elementId, attributeName, value);
		}
		public void BindEvent(string elementId, UIEventId eventType, UIEventCallback callback)
		{
				ThrowIfInvalid();
				if (!InternalCalls.UI_BindEvent(m_Handle, elementId, eventType))
				{
						Log.Error("Could not register event {} for element {}. This error can show, that this event type does not exist or not supported by the element.", eventType, elementId);
						return;
				}
				if (!s_Events.ContainsKey(m_Handle))
				{
						s_Events[m_Handle] = new();
				}
				var elements = s_Events[m_Handle];
				if (!elements.ContainsKey(elementId))
				{
						elements[elementId] = new();
				}
				var events = elements[elementId];
				if (!events.ContainsKey(eventType))
				{
						events[eventType] = callback;
				}
				else
				{
						events[eventType] += callback;
				}
				Log.Info("Registered {0} event for {1} element (document {2})", eventType, elementId, m_Handle);

		}
		public string? CreateElement(string parentId, string tagName, string? elementId = null)
		{
				ThrowIfInvalid();
				if (elementId is null)
				{
						elementId = Guid.NewGuid().ToString();
				}
				return InternalCalls.UI_CreateElement(m_Handle, parentId, tagName, elementId);
		}
		public void DeleteElement(string elementId)
		{
				ThrowIfInvalid();
				InternalCalls.UI_DeleteElement(m_Handle, elementId);
		}
		public void SetStyle(string elementId, string property, string value)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetStyle(m_Handle, elementId, property, value);
		}
		public void ClearChildren(string elementId)
		{
				ThrowIfInvalid();
				InternalCalls.UI_ClearChildren(m_Handle, elementId);
		}
		void ThrowIfInvalid()
		{
				Log.AssertAndThrow(m_Handle != 0, "Document is invalid");
		}
		private ulong m_Handle;
}
