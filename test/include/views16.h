#include "tux/util.hpp"

struct my_struct {
	float	f;		/* null=0.000000 */
	double	d;		/* null=0.000000 */
	long	l;		/* null=0 */
};

ENABLE_TYPE_NAME(my_struct)

struct string_info {
	char	original_string[100];		/* null="\0" */
	long	byte_count;		/* null=0 */
	long	ascii_sum;		/* null=0 */
	char	most_frequent_char;		/* null='\0' */
};

ENABLE_TYPE_NAME(string_info)

