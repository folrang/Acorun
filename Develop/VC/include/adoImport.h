/////////////////////////////////////////////////////////////////////////////
// 
// adoImport.h
// 
// Header file for ado2.h and adox.h
//
// Created by Theo Buys, 27-4-2005
// 
// Last revision: 
//   $Author: Buys_t $
//     $Date: 4-09-12 12:41 $
// $Revision: 2 $
//
// msado15.dll has namespace ADODB 
// msadox.dll has namespace ADOX
// msjro.dll has namespace JRO
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// 2012-10-12
// * ado2.h 클래스 라이브러리 사용시에, 
//		Windows7 SP1, KB2640696 업데이트에서 ADO 인터페이스들의 GUID 변경에 따른 문제 해결을 위한 추가 헤더 파일
//	http://www.codeproject.com/Articles/1075/A-set-of-ADO-classes-version-2-20 페이지에서 
//		- "Broken ADO when compiling at Windows 7 SP1 [modified]" 댓글 참조
//		- "Re: Solving the Issue of building on Windows SP1 and run on XP machine, but lost CJetEngine functionalities [modified]" 댓글 참조
//
// * 프로젝트 추가 폴더에 "$(CommonProgramFiles)\system\ado" 폴더 추가 필요
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADOIMPORT_H__A8F183D1_116B_4869_9125_16CFF9A03ADA__INCLUDED_)
#define AFX_ADOIMPORT_H__A8F183D1_116B_4869_9125_16CFF9A03ADA__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comdef.h>
 
//#pragma message ( "*** adoImport.h:" )
//#pragma message ( "*** Make sure you have set the library directory for msadox.dll, msado15.dll and msjro.dll." )
//#pragma message ( "*** In Visual Studio 2010 go to"  )
//#pragma message ( "*** View > Property Manager > Properties > Microsoft.Cpp.Win32.user > VC++ Directories > Library Directories")
//#pragma message ( "*** and add $(CommonProgramFiles)\\system\\ado")
 
// CG : In order to use this code against a different version of ADO, the appropriate
// ADO library needs to be used in the #import statement
 
#import <msadox.dll> 
 
/////////////////////////////////////////////////////////////////////////////
// msado15.dll and type libraries (.tlb) issues for building in Windows 7 SP1
// see: http://support.microsoft.com/kb/2640696
//
// 1) Consider the scenario where you are a C++ developer, and you include the 
// following line of code in the application: 
//
// #import <msado15.dll> rename("EOF", "EndOfFile")
//
// 2) Consider the scenario that you recompile an application that must run in 
// Windows Vista, in Windows Server 2008, or in later versions of Windows. 
// And, you are not using MSJRO. In this scenario, you must change 
// #import msado15.dll to the following:
// 
// #import <msado60.tlb> rename("EOF", "EndOfFile")
//
// 3) Consider the scenario that you are using MSJRO, and you recompile an application 
// that must run in Windows Vista, in Windows Server 2008, or in a later 
// version of Windows. 
// 4) Consider the scenario that you recompile your application that must run in Windows XP 
// or in Windows Server 2003. In both scenarios, you must change #import msado15.dll 
// to the following:
//
// #import <msado28.tlb> rename("EOF", "EndOfFile")
 
#import <msado28.tlb> rename("EOF", "EndOfFile")
 
/////////////////////////////////////////////////////////////////////////////
// msjro.dll issues
//
// #import <msjro.dll> no_namespace rename("ReplicaTypeEnum", "_ReplicaTypeEnum")
//
// Enable the namespace and suppress warning C4336: 
// import cross-referenced type library 'msado28.tlb' before importing 'msjro.dll'
 
#import <msjro.dll> rename("ReplicaTypeEnum", "_ReplicaTypeEnum")
	
#endif // !defined(AFX_ADOIMPORT_H__A8F183D1_116B_4869_9125_16CFF9A03ADA__INCLUDED_)
