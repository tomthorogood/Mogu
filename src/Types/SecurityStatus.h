#ifndef SECURITY_STATUS_H_
#define SECURITY_STATUS_H_

enum class : unsigned char Security_Status
{
    err_unknown
    , err_user_exists
    , err_user_not_found
    , err_bad_auth
    , err_no_group_selected
    , ok_register
    , ok_login
    , ok_password_reset
    , ok_remove
};

#endif
