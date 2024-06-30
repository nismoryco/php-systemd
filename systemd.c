    #ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_systemd.h"
#include <systemd/sd-daemon.h>
#include <systemd/sd-journal.h>

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(sd_journal_send_args, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(sd_notify_args, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, state, IS_STRING, 0)
ZEND_END_ARG_INFO()

zend_function_entry systemd_functions[] = {
    PHP_FE(sd_journal_send, sd_journal_send_args)
    PHP_FE(sd_notify, sd_notify_args)
    {NULL, NULL, NULL} // Sentinel
};

zend_module_entry systemd_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_SYSTEMD_EXTNAME,
    systemd_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_SYSTEMD_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SYSTEMD
ZEND_GET_MODULE(systemd)
#endif

PHP_FUNCTION(sd_journal_send)
{
    struct iovec *iov = NULL;
    zval *args;
    int argc, len, i;
    char *val;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) != SUCCESS) {
        return;
    }

    // Allocate sufficient iovector space for the arguments.
    iov = safe_emalloc(argc, sizeof(struct iovec), 0);
    if (!iov) {
        // Should probably raise a more clear error.
        RETURN_FALSE;
    }

    // Iterate through the PHP arguments and fill the iovector.
    for (i = 0; i < argc; ++i) {
        convert_to_string_ex(&args[i]);
        val = Z_STRVAL(args[i]);
        len = Z_STRLEN(args[i]);
        iov[i].iov_base = val;
        iov[i].iov_len = len;
    }

    // Send the iovector to journald.
    sd_journal_sendv(iov, argc);

    // Free the iovector. The actual strings
    // are already managed by PHP.
    efree(iov);

    RETURN_TRUE;
}

PHP_FUNCTION(sd_notify)
{
    zend_string *state = NULL;
    int retval = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &state) != SUCCESS) {
        return;
    }

    retval = sd_notify(0, ZSTR_VAL(state));

    RETURN_LONG(retval);
}
