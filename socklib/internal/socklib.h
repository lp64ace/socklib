#pragma once

#include "sockdefs.h"

#define ACE_SOCK_ASSERT_FUNC(f)			{int r=f;if(r!=ACE_SOCK_OK)return r;}

namespace ace {

	int SockInit ( int major = 2 , int minor = 2 );
	int SockQuit ( );

	int SockStatus ( );

}