#include <crow.h>
#include <crow/mustache.h>
int main(int argc, char *argv[])
{
    crow::SimpleApp app;
    crow::mustache::context ctx;
    auto page = crow::mustache::load("faq.mustache.html");
    CROW_ROUTE(app, "/test")([]() { return "ok"; });
    CROW_ROUTE(app, "/")([&page, &ctx]() { return page.render(ctx); });
    app.port(18080).multithreaded().run();
    return 0;
}
