#pragma once
//∏≈≈©∑Œ
#define JHWND_EXCEPT( hr ) Window::HrException (__LINE__,__FILE__ ,hr)
#define JHWND_LAST_EXCEPT() Window::HrException (__LINE__,__FILE__ , GetLastError())
#define JHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )