#ifndef FAQ_GOOGLESHEETDATAACCESS_HPP
#define FAQ_GOOGLESHEETDATAACCESS_HPP
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "FAQRow.hpp"
#include "IDataAccess.hpp"
#include "cpp-httplib/httplib.h"
#include "json/single_include/nlohmann/json.hpp"
#include <iostream>
using json = nlohmann::json;
class GoogleSheetDataAccess : public IDataAccess
{
  public:
    GoogleSheetDataAccess(const std::string &pSpreadsheetId, const std::string &pApiKey, const std::string &pTab)
        : mSpreadsheetId(pSpreadsheetId), mApiKey(pApiKey), mTab(pTab)
    {
    }
    virtual bool createQuestion(const std::string &question)
    {
        return false;
    }
    /* Method for getting all validated (shown) questions.
     * @return optional with all questions/answers
     */
    virtual std::optional<std::vector<FAQRow>> getAllValidated()
    {
        auto res = mHttpClient.Get("/v4/spreadsheets/" + mSpreadsheetId + "/values:batchGet?ranges=" + mTab +
                                   "&key=" + mApiKey);

        if (!res->body.empty())
        {
            std::cout << res->body << std::endl;

            std::vector<FAQRow> returnValue;

            json sheet = json::parse(res->body);

            auto lines = sheet["valueRanges"][0]["values"];

            if (!lines.empty())
            {
                lines.erase(lines.begin());

                for (auto &line : lines)
                {
                    bool show = line[3] == "TRUE" ? true : false;
                    if (show)
                    {
                        try
                        {
                            FAQRow row;
                            std::string rowidstr = line[0];

                            row.ROWID = std::stoi(rowidstr);
                            row.QUESTION = line[1];
                            row.RESPONSE = line[2];
                            row.SHOW = true;

                            returnValue.push_back(row);
                        }
                        catch (const std::invalid_argument &e)
                        {
                            std::cout << "Cannot instantiate FAQRow : " << e.what() << " : " << line[0] << "\n";
                        }
                        catch (const std::out_of_range &e)
                        {
                            std::cout << "Cannot instantiate FAQRow : " << e.what() << " : " << line[0] << "\n";
                        }
                    }
                }
                return {returnValue};
            }
        }
        return std::nullopt;
    }

  private:
    httplib::Client mHttpClient{"https://sheets.googleapis.com"};
    std::string mSpreadsheetId;
    std::string mTab;
    std::string mApiKey;
};
#endif
