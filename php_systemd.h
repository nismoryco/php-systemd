#ifndef PHP_SYSTEMD_H
#define PHP_SYSTEMD_H 1
#define PHP_SYSTEMD_VERSION "0.2.0"
#define PHP_SYSTEMD_EXTNAME "systemd"

PHP_FUNCTION(sd_journal_send);
PHP_FUNCTION(sd_notify);

extern zend_module_entry systemd_module_entry;
#define phpext_systemd_ptr &systemd_module_entry

#endif
