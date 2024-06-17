#include "GoogleSheetDataAccess.hpp"
#include "IDataAccess.hpp"
#include "SQLiteDataAccess.hpp"
#include "Tools.hpp"
#include <crow.h>
#include <crow/mustache.h>
#include <memory>
#include <string>
using pDataAccess = std::unique_ptr<IDataAccess>;
int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        std::ifstream configfileStream(argv[1]);
        json config = json::parse(configfileStream);

        crow::SimpleApp app;
        crow::mustache::context ctx;
        auto page = crow::mustache::load("faq.mustache.html");

        pDataAccess dataAccess = std::make_unique<GoogleSheetDataAccess>(
            config["spreadsheetId"], config["apiKey"], config["sheetId"], config["privateKey"], config["gAccount"]);

        if (argv[2] != nullptr)
        {
            std::string argv2 = argv[2];
            if (argv2 == "--local")
            {
                std::cout << "Local database mode" << std::endl;
                dataAccess = std::make_unique<SQLiteDataAccess>("faq.db");
            }
        }
        CROW_ROUTE(app, "/faq")
        ([&page, &ctx, &dataAccess]() {
            auto allValidatedQ = dataAccess->getAllValidated();

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
                if (dataAccess->createQuestion(question))
                {
                    returnValue = "Ok.";
                }
            }

            return returnValue;
        });
        CROW_ROUTE(app, "/question/<int>")
            .methods(crow::HTTPMethod::Get)([&dataAccess](const crow::request &req, int rowid) {
                auto page = crow::mustache::load("edit.mustache.html");
                crow::mustache::context ctx;

                auto rowOpt = dataAccess->getOne(rowid);

                if (rowOpt.has_value())
                {
                    auto faqrow = rowOpt.value();
                    ctx["Qr"] = Tools::convertToWValue(faqrow);
                }
                return page.render(ctx);
            });
        CROW_ROUTE(app, "/question/<int>")
            .methods(crow::HTTPMethod::Delete)([&dataAccess](const crow::request &req, int rowid) {
                crow::response response;
                response.code = 400;
                if (dataAccess->deleteQuestion(rowid))
                {
                    response.set_header("HX-Redirect", "/");
                    response.code = 200;
                }
                return response;
            });

        CROW_ROUTE(app, "/question/<int>")
            .methods(crow::HTTPMethod::Put)([&dataAccess](const crow::request &req, int rowid) {
                auto bodyParams = req.get_body_params();
                auto keys = bodyParams.keys();

                std::string stringRowid = std::to_string(rowid);
                crow::response response;
                response.code = 400;

                if (dataAccess->updateQuestion(
                        rowid, bodyParams.get("question-" + stringRowid), bodyParams.get("response-" + stringRowid),
                        std::count(keys.begin(), keys.end(), "show-" + stringRowid) > 0 ? true : false))
                {
                    response.set_header("HX-Redirect", "/");
                    response.code = 200;
                }

                return response;
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
