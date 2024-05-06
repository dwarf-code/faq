#include "include/IDataAccess.hpp"
#include "include/MockedDataAccess.hpp"
#include "include/Tools.hpp"
#include <crow.h>
#include <crow/mustache.h>
int main(int argc, char *argv[])
{
    crow::SimpleApp app;
    crow::mustache::context ctx;
    auto page = crow::mustache::load("faq.mustache.html");

    MockedDataAccess mockedDataAccess;
    IDataAccess &dataAccess = mockedDataAccess;

    CROW_ROUTE(app, "/faq")
    ([&page, &ctx, &dataAccess]() {
        auto allValidatedQ = dataAccess.getAllValidated();

        if (allValidatedQ.has_value())
        {
            auto vectorValidatedQ = allValidatedQ.value();

            std::for_each(vectorValidatedQ.begin(), vectorValidatedQ.end(),
                          [](auto &elt) { std::cout << elt.QUESTION << " | " << elt.RESPONSE << std::endl; });
            ctx["allQr"] = Tools::convertListToWValue(vectorValidatedQ);
        }
        return page.render(ctx);
    });

    app.port(18080).multithreaded().run();
    return 0;
}
