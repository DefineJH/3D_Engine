#pragma once

//hrcall 은 HRESULT를 리턴하는 코드. 또한 hr이 이미 지역스코프 안에 정의되어 있다는 전제를 하고 있다.
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
//명령이 불려진 시점부터 명령이 불러진 다음까지 메시지가 발생시 에러발생
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// throw_info 매크로들은 현재 로컬스코프 안에서 접근할 수 있는 infomanager를 기대한다.
// bindable의 getinfomanager함수를 매크로로 만들어 infomanager를 암시적으로 사용할 수 있게끔 한다
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
//디버깅상태일때만 infomanager를 가져올 수 있다 ( bindable 의 static 함수 )
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager(gfx)
#endif
