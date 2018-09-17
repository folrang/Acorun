// CmnHdr.h
//	- 프로젝트 공용 포함 파일
// 
////////////////////////////////////////////////////////////////////////////////
#pragma once   

// When using Unicode Windows functions, use Unicode C-Runtime functions too.
#ifdef UNICODE
   #ifndef _UNICODE
      #define _UNICODE
   #endif
#endif