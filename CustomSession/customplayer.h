// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#ifndef PLAYER_H
#define PLAYER_H

#include <new>
#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

// Media Foundation headers
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <evr.h>
#include <wrl/client.h>
#include "resource.h"


const UINT WM_APP_PLAYER_EVENT = WM_APP + 1;   

// WPARAM = IMFMediaEvent*, WPARAM = MediaEventType

enum PlayerState
{
    Closed = 0,     // No session.
    Ready,          // Session was created, ready to open a file. 
    OpenPending,    // Session is opening a file.
    Started,        // Session is playing a file.
    Paused,         // Session is paused.
    Stopped,        // Session is stopped (ready to play). 
    Closing         // Application has closed the session, but is waiting for MESessionClosed.
};

class CPlayer : public IMFAsyncCallback
{
public:
    static HRESULT CreateInstance(HWND hVideo, HWND hEvent, CPlayer **ppPlayer);

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)override;
    STDMETHODIMP_(ULONG) AddRef()override;
    STDMETHODIMP_(ULONG) Release()override;

    // IMFAsyncCallback methods
    STDMETHODIMP  GetParameters(DWORD*, DWORD*)override
    {
        // Implementation of this method is optional.
        return E_NOTIMPL;
    }
    STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult)override;

    // Playback
    HRESULT       OpenURL(const WCHAR *sURL);
    HRESULT       Play();
    HRESULT       Pause();
    HRESULT       Stop();
    HRESULT       Shutdown();
    HRESULT       HandleEvent(UINT_PTR pUnkPtr);
    PlayerState   GetState() const { return m_state; }

    // Video functionality
    HRESULT       Repaint();
    HRESULT       ResizeVideo(WORD width, WORD height);

    BOOL          HasVideo() const { return (m_pVideoDisplay != NULL);  }

protected:

    // Constructor is private. Use static CreateInstance method to instantiate.
    CPlayer(HWND hVideo, HWND hEvent);

    // Destructor is private. Caller should call Release.
    virtual ~CPlayer(); 

    HRESULT Initialize();
    HRESULT CreateSession();
    HRESULT CloseSession();
    HRESULT StartPlayback();

    // Media event handlers
    virtual HRESULT OnTopologyStatus(const Microsoft::WRL::ComPtr<IMFMediaEvent> &pEvent);
    virtual HRESULT OnPresentationEnded(const Microsoft::WRL::ComPtr<IMFMediaEvent> &pEvent);
    virtual HRESULT OnNewPresentation(const Microsoft::WRL::ComPtr<IMFMediaEvent> &pEvent);

    // Override to handle additional session events.
    virtual HRESULT OnSessionEvent(const Microsoft::WRL::ComPtr<IMFMediaEvent>&, MediaEventType)
    { 
        return S_OK; 
    }

protected:
    long                    m_nRefCount;        // Reference count.

    Microsoft::WRL::ComPtr<IMFMediaSession>         m_pSession;
    Microsoft::WRL::ComPtr<IMFMediaSource>          m_pSource;
    Microsoft::WRL::ComPtr<IMFVideoDisplayControl>  m_pVideoDisplay;

    HWND                    m_hwndVideo;        // Video window.
    HWND                    m_hwndEvent;        // App window to receive events.
    PlayerState             m_state;            // Current state of the media session.
    HANDLE                  m_hCloseEvent;      // Event to wait on while closing.
};

#endif PLAYER_H
