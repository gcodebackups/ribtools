//==================================================================
/// DExceptions.h
///
/// Created by Davide Pasca - 2011/9/4
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DEXCEPTIONS_H
#define DEXCEPTIONS_H

#include "DUtils_Base.h"

//==================================================================
#if !defined(ANDROID)

# include <new>
# include <stdexcept>
# include <memory>
# include <string.h>

# define __DSHORT_FILE__ \
	(strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : \
		(strrchr(__FILE__,'\\') ? strrchr(__FILE__,'\\')+1 : __FILE__) )


# define	DEX_RUNTIME_ERROR(_FMT_,...)	{ char buff[512]; snprintf( buff, sizeof(buff), "%s (%i) -> "_FMT_, __DSHORT_FILE__, __LINE__, ##__VA_ARGS__ ); throw std::runtime_error( std::string(buff) ); }
# define	DEX_OUT_OF_RANGE(_FMT_,...)		{ char buff[512]; snprintf( buff, sizeof(buff), "%s (%i) -> "_FMT_, __DSHORT_FILE__, __LINE__, ##__VA_ARGS__ ); throw std::out_of_range( std::string(buff) ); }
//# define	DEX_BAD_ALLOC(_FMT_,...)		{ char buff[512]; snprintf( buff, sizeof(buff), "%s:%i -> "_FMT_, __DSHORT_FILE__, __LINE__, ##__VA_ARGS__ ); throw std::bad_alloc( buff ); }
# define	DEX_BAD_ALLOC(_FMT_,...)		{ DUT::DVerbose(  "%s:%i -> "_FMT_, __DSHORT_FILE__, __LINE__, ##__VA_ARGS__ ); throw std::bad_alloc(); }

#else

# define	DEX_RUNTIME_ERROR(_FMT_,...)	{ DASSTHROW( false, "runtime_error: " _FMT_, ##__VA_ARGS__ ); }
# define	DEX_OUT_OF_RANGE(_FMT_,...)		{ DASSTHROW( false, "out_of_range: " _FMT_, ##__VA_ARGS__ ); }
# define	DEX_BAD_ALLOC(_FMT_,...)		{ DASSTHROW( false, "bad_alloc: " _FMT_, ##__VA_ARGS__ ); }

#endif


#endif
