#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "xmpp/xmpp.h"
#include "ui/ui.h"
#include "command/commands.h"

static void test_with_connection_status(jabber_conn_status_t status)
{
    CommandHelp *help = malloc(sizeof(CommandHelp));
    
    will_return(jabber_get_connection_status, status);
    expect_string(cons_show, output, "You are either connected already, or a login is in process.");
    
    gboolean result = cmd_connect(NULL, *help);
    assert_true(result);

    free(help);
}

void cmd_connect_shows_message_when_disconnecting(void **state)
{
    test_with_connection_status(JABBER_DISCONNECTING);
}

void cmd_connect_shows_message_when_connecting(void **state)
{
    test_with_connection_status(JABBER_CONNECTING);
}

void cmd_connect_shows_message_when_connected(void **state)
{
    test_with_connection_status(JABBER_CONNECTED);
}

void cmd_connect_shows_message_when_undefined(void **state)
{
    test_with_connection_status(JABBER_UNDEFINED);
}

void cmd_connect_when_no_account(void **state)
{
    CommandHelp *help = malloc(sizeof(CommandHelp));
    gchar *args[] = { "user@server.org", NULL };

    will_return(jabber_get_connection_status, JABBER_DISCONNECTED);
    
    expect_string(accounts_get_account, name, "user@server.org");
    will_return(accounts_get_account, NULL);

    will_return(ui_ask_password, strdup("password"));

    expect_string(cons_show, output, "Connecting as user@server.org");

    expect_string(jabber_connect_with_details, jid, "user@server.org");
    expect_string(jabber_connect_with_details, passwd, "password");
    expect_any(jabber_connect_with_details, altdomain);
    will_return(jabber_connect_with_details, JABBER_CONNECTING);

    gboolean result = cmd_connect(args, *help);
    assert_true(result);

    free(help);
}
