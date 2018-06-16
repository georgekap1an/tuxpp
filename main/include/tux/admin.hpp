/** @defgroup admin admin
Programmatically administer the application */

/** @file admin.hpp
Routines for administration.
@ingroup admin */
#include <string>
#include "tpadm.h"
#include "tux/fml32.hpp"

namespace tux
{
    
/** Convert a CLIENTID to a string [@c tpconvert].
For use with TMIB functions. @ingroup admin */
std::string to_string(CLIENTID const& clt);

/** Convert a string to a CLIENTID [@c tpconvert].
For use with TMIB functions. @ingroup admin */
CLIENTID to_clientid(std::string const& str);

/** Convert a TPTRANID to a string [@c tpconvert].
For use with TMIB functions. @ingroup admin */
std::string to_string(TPTRANID const& tranid);

/** Convert a string to a TPTRANID [@c tpconvert].
For use with TMIB functions. @ingroup admin */
TPTRANID to_tranid(std::string const& str);

#if TUXEDO_VERSION >= 1213
/** Set out of band metadata on a buffer [@c tpsetcallinfo].
@since Tuxedo 12.1.3 @ingroup admin */   
void set_call_info(fml32 const& call_info, buffer& dest);

/** Get out of band metadata on a buffer [@c tpgetcallinfo].
@since Tuxedo 12.1.3 @ingroup admin */
fml32 get_call_info(buffer const& x);
#endif

/** Perform an admin call with pre-allocated output buffer [@c tpadmcall]. @ingroup admin */
void admin_call(fml32 const& request, fml32& response);

/** Perform an admin call [@c tpadmcall]. @ingroup admin */
fml32 admin_call(fml32 const& request);


/** Set metadata repository information [@c tpsetrepos].
@param filename the location of the metadata repository file
@param input data structure containing info to set
@returns fml32 with retrieved data and status information. @ingroup admin */
fml32 set_repository_info(std::string const& filename, fml32 const& input);

/** Get metadata repository information [@c tpgetrepos].
@param filename the location of the metadata repository file
@param input fml32 query
@returns fml32 with retrieved data and status information. @ingroup admin */
fml32 get_repository_info(std::string const& filename, fml32 const& input);
    
}




