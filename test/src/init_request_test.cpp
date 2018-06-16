#include "doctest.h"
#include "tux/init_request.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("init_request");

TEST_CASE("init_request default construct")
{
    init_request x;
    CHECK(x.as_tpinit() == nullptr);
    CHECK((bool)x == false);
}

TEST_CASE("init_request copy construct")
{
    init_request a;
    a.username("user");
    a.auth_data("userpassword");
    init_request b(a);
    CHECK(a.username() == "user");
    CHECK(a.auth_data() == "userpassword");
    CHECK(b.username() == "user");
    CHECK(b.auth_data() == "userpassword");   
}

TEST_CASE("init_request copy assign")
{
    init_request a,b;
    a.username("user_a");
    a.auth_data("userpassword_a");
    b.username("user_b");
    b.auth_data("userpassword_b");
    b = a;
    CHECK(a.username() == "user_a");
    CHECK(a.auth_data() == "userpassword_a");
    CHECK(b.username() == "user_a");
    CHECK(b.auth_data() == "userpassword_a");   
}

TEST_CASE("init_request move construct")
{
    init_request a;
    a.username("user");
    a.auth_data("userpassword");
    init_request b(move(a));
    CHECK(a.as_tpinit() == nullptr);
    CHECK(b.username() == "user");
    CHECK(b.auth_data() == "userpassword");   
}

TEST_CASE("init_request move assign")
{
    init_request a,b;
    a.username("user_a");
    a.auth_data("userpassword_a");
    b.username("user_b");
    b.auth_data("userpassword_b");
    b = move(a);
    CHECK(a.as_tpinit() == nullptr);
    CHECK(b.username() == "user_a");
    CHECK(b.auth_data() == "userpassword_a");   
}

TEST_CASE("init_request set/get fields")
{
    init_request a;
    a.username("user");
    a.client_name("client");
    a.app_password("app_password");
    a.groupname("group");
    a.flags(TPU_IGN | TPSA_FASTPATH | TPMULTICONTEXTS);
    a.auth_data("user_password");
    
    CHECK(a.username() == "user");
    CHECK(a.client_name() == "client");
    CHECK(a.app_password() == "app_password");
    CHECK(a.groupname() == "group");
    CHECK(a.flags() == (TPU_IGN | TPSA_FASTPATH | TPMULTICONTEXTS));
    CHECK(a.auth_data() == "user_password");
}

TEST_CASE("init_request bool conversion")
{
    init_request a;
    CHECK((bool)a == false);
    a.username("value");
    CHECK((bool)a == true);
}

TEST_CASE("init_request comparison")
{
    init_request a,b;
    CHECK(a == b);
    a.username("user_a");
    a.auth_data("userpassword_a");
    CHECK(a != b);
    b.username("user_b");
    b.auth_data("userpassword_b");
    CHECK(a != b);
    a = b;
    CHECK(a == b);
}

TEST_SUITE_END();
