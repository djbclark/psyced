#include <net.h>

#if defined(STORAGE_MYSQL) 
# if __EFUN_DEFINED__(db_connect)

/* 
 * from ldmud's concepts/mysql:
 *
 * As mySQL "limits" the number of connections to 100 and as every
 * connection to the mySQL-server takes time, you should use
 * database serverobjects in your MUD which constantly keep the
 * connection to the mySQL-server.
 *
 * hence we use this instead of letting each object have its own connection.
 *
 * Actually we aren't using this code at all, since psyced makes no 
 * operations by which it makes sense to use SQL rather than flat files.
 */

volatile int handle;

create() {
    handle = db_connect(STORAGE_MYSQL_DATABASE, STORAGE_MYSQL_USER, STORAGE_MYSQL_PASSWORD);
}

mixed query(string q, varargs mixed args) {
    // TODO: it might be wise to db_conv_string on each arg to 
    // avoid sql injections
    int res;
    mixed row;
    mixed *data = ({ });
    res = db_exec(handle, sprintf(q, args...));
    unless(res) return ({ });
    while(row = db_fetch(handle)) 
	data += ({ row });
    return data;
}

# endif
#endif
