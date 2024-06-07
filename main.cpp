#include "GoogleSheetDataAccess.hpp"
#include "IDataAccess.hpp"
#include "MockedDataAccess.hpp"
#include "Tools.hpp"
#include <crow.h>
#include <crow/mustache.h>
int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        std::ifstream configfileStream(argv[1]);
        json config = json::parse(configfileStream);

        crow::SimpleApp app;
        crow::mustache::context ctx;
        auto page = crow::mustache::load("faq.mustache.html");

        GoogleSheetDataAccess googleDataAccess(config["spreadsheetId"], config["apiKey"], config["sheetId"],
                                               config["privateKey"], config["gAccount"]);

        IDataAccess &dataAccess = googleDataAccess;

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

        CROW_ROUTE(app, "/question").methods(crow::HTTPMethod::Post)([&dataAccess](const crow::request &req) {
            // Define a return value
            std::string returnValue = "Error.";
            // Retrieving body params from POST request.
            auto bodyParams = req.get_body_params();
            std::string question = bodyParams.get("input-question");

            if (!question.empty() && question.length() <= 200)
            {
                if (dataAccess.createQuestion(question))
                {
                    returnValue = "Ok.";
                }
            }

            return returnValue;
        });

        app.port(18080).multithreaded().run();
        return 0;
    }
    else
    {
        std::cout << "Number of arguments invalid" << std::endl;
        return 1;
    }
}
