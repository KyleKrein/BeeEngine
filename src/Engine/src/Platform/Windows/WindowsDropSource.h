//
// Created by alexl on 24.11.2023.
//

#pragma once
#include <windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <oleidl.h>     // for IDropSource, IDataObject
namespace BeeEngine::Internal
{
    class WindowsDropSource: public IDropSource
    {
    public:
        static IDropSource *CreateInstance();

        WindowsDropSource();

    private:
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
        virtual ULONG STDMETHODCALLTYPE AddRef();
        virtual ULONG STDMETHODCALLTYPE Release();

        HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
        HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

        virtual ~WindowsDropSource();

    protected:
        LONG m_nRefCount;
    };

    class WindowsDropTarget: public IDropTarget
    {
    public:
        static IDropTarget *CreateInstance();

        WindowsDropTarget();

        HRESULT DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

        HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

        HRESULT DragLeave(void) override;

        HRESULT Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

        HRESULT QueryInterface(const IID &riid, void **ppvObject) override;

        ULONG AddRef(void) override;

        ULONG Release(void) override;
    private:
        void HandleStartDrag();
        ULONG m_nRefCount = 0;
        bool m_StartedDrag = false;

        void FinishDragEvent();
    };

    IDataObject *GetFileDataObject(const TCHAR *pszFile, REFIID riid);
} // BeeEngine::Internal
